#include "inter_request_state.h"

#include "gateway_requests.h"
#include "network_requestor.h"

#include "base/logging.h"
#include "content/public/browser/browser_context.h"

#include <ipfs_client/dag_listener.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/response.h>
#include <ipfs_client/scheduler.h>

using Self = ipfs::InterRequestState;

namespace {
constexpr char user_data_key[] = "ipfs_request_userdata";
}

auto Self::FromBrowserContext(content::BrowserContext* context)
    -> InterRequestState& {
  if (!context) {
    LOG(WARNING) << "No browser context! Using a default IPFS state.";
    static ipfs::InterRequestState static_state({});
    return static_state;
  }
  base::SupportsUserData::Data* existing = context->GetUserData(user_data_key);
  if (existing) {
    VLOG(1) << "Re-using existing IPFS state.";
    return *static_cast<ipfs::InterRequestState*>(existing);
  }
  LOG(INFO) << "Creating new IPFS state for this browser context.";
  auto owned = std::make_unique<ipfs::InterRequestState>(context->GetPath());
  ipfs::InterRequestState* raw = owned.get();
  context->SetUserData(user_data_key, std::move(owned));
  return *raw;
}
auto Self::serialized_caches() -> std::array<decltype(mem_), 2> {
  if (!mem_) {
    auto p = mem_ = std::make_shared<CacheRequestor>(
        net::CacheType::MEMORY_CACHE, *this, base::FilePath{});
    storage().AddStorageHook(
        [p](auto c, auto h, auto b) { p->Store(c, h, b); });
  }
  if (!dsk_) {
    auto p = dsk_ = std::make_shared<CacheRequestor>(net::CacheType::DISK_CACHE,
                                                     *this, disk_path_);
    storage().AddStorageHook(
        [p](auto c, auto h, auto b) { p->Store(c, h, b); });
  }
  return {mem_, dsk_};
}
auto Self::requestor() -> BlockRequestor& {
  if (!requestor_.Valid()) {
    serialized_caches();
    requestor_.Add(mem_);
    requestor_.Add(dsk_);
    requestor_.Add(std::make_shared<NetworkRequestor>(*this));
  }
  return requestor_;
}
std::shared_ptr<ipfs::GatewayRequests> Self::api() {
  auto existing = api_.lock();
  if (existing) {
    return existing;
  }
  auto created = std::make_shared<ipfs::GatewayRequests>(*this);
  api_ = created;
  auto t = std::time(nullptr);
  if (t - last_discovery_ > 300) {
    created->Discover([this](auto v) { gws_.AddGateways(v); });
    last_discovery_ = t;
  }
  return created;
}
auto Self::scheduler() -> Scheduler& {
  auto api = api_.lock();
  DCHECK(api);
  return api->scheduler();
}

namespace {

void send_gateway_request(Self* me,
                          std::shared_ptr<ipfs::gw::GatewayRequest> req) {
  if (!req->dependent) {
    LOG(FATAL) << "This makes no sense whatsoever - why do you want to request "
                  "things if nothing awaits.";
  }
  struct DagListenerAdapter final : public ipfs::DagListener {
    std::shared_ptr<ipfs::gw::GatewayRequest> gw_req;
    std::string bytes;
    std::shared_ptr<ipfs::GatewayRequests> api;
    void ReceiveBlockBytes(std::string_view b) override {
      LOG(INFO) << "DagListenerAdapter::ReceiveBlockBytes(" << b.size() << "B)";
      bytes.assign(b);
    }
    void BlocksComplete(std::string mime_type) override {
      LOG(INFO) << "DagListenerAdapter::BlocksComplete(" << mime_type << ")";
      ipfs::Response r{mime_type, 200, std::move(bytes), ""};
      gw_req->dependent->finish(r);
    }
    void NotHere(std::string_view cid, std::string_view path) override {
      LOG(INFO) << "DagListenerAdapter::NotHere(" << cid << ',' << path << ")";
      api->scheduler().IssueRequests(api);
    }
    void DoesNotExist(std::string_view cid, std::string_view path) override {
      LOG(INFO) << "DagListenerAdapter::DoesNotExist(" << cid << ',' << path
                << ")";
      ipfs::Response r{"", 404, "", ""};
      gw_req->dependent->finish(r);
    }
  };
  auto dl = std::make_shared<DagListenerAdapter>();
  dl->api = me->api();
  dl->gw_req = req;
  auto& sched = me->scheduler();
  sched.Enqueue(me->api(), dl, {}, req->url_suffix().substr(1), req->accept(),
                9, req);
  sched.IssueRequests(me->api());
}
std::string detect_mime(Self* me,
                        std::string a,
                        std::string_view b,
                        std::string const& c) {
  auto api = me->api();
  return static_cast<ipfs::ContextApi*>(api.get())->MimeType(a, b, c);
}
}  // namespace

auto Self::orchestrator() -> Orchestrator& {
  if (!orc_) {
    auto gwreq = [this](auto p) { send_gateway_request(this, p); };
    auto mimer = [this](auto a, auto b, auto& c) {
      return detect_mime(this, a, b, c);
    };
    orc_ = std::make_shared<Orchestrator>(gwreq, mimer);
  }
  return *orc_;
}

Self::InterRequestState(base::FilePath p) : disk_path_{p} {}
Self::~InterRequestState() noexcept {}
