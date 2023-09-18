#include "gateway_requests.h"

#include "crypto_api.h"
#include "inter_request_state.h"
#include "ipns_cbor.h"

#include "base/strings/escape.h"
#include "net/base/mime_sniffer.h"
#include "net/base/mime_util.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "url/gurl.h"

#include <ipfs_client/dag_block.h>
#include <ipfs_client/ipns_record.h>

#include <libp2p/multi/content_identifier_codec.hpp>
#include <libp2p/peer/peer_id.hpp>

#include <base/logging.h>

using Self = ipfs::GatewayRequests;

constexpr net::NetworkTrafficAnnotationTag kTrafficAnnotation =
    net::DefineNetworkTrafficAnnotation("ipfs_gateway_request", R"(
      semantics {
        sender: "IPFS component"
        description:
          "Sends a request to an IPFS gateway for a block."
        trigger:
          "Processing of an ipfs:// URL (possibly from an ipns:// URL)."
        data: "None"
        destination: WEBSITE
      }
      policy {
        cookies_allowed: NO
        setting: "Currently, this feature cannot be disabled by settings. TODO"
      }
    )");
namespace {
constexpr std::size_t MB = 1'000'000UL;
std::unique_ptr<network::SimpleURLLoader> discovery_loader;
void parse_discover_response(std::function<void(std::vector<std::string>)> cb,
                             std::unique_ptr<std::string> body) {
  std::vector<std::string> discovered;
  if (!body) {
    LOG(ERROR) << "Failed to discover gateways.";
    cb(discovered);
    return;
  }
  std::string_view r{*body};
  while (r.size()) {
    VLOG(1) << "Discovered gateways, body remaining to parse: " << r;
    auto i = r.find('"');
    if (i == std::string_view::npos) {
      break;
    }
    r.remove_prefix(++i);
    i = r.find('"');
    discovered.emplace_back(r.substr(0, i));
    r.remove_prefix(++i);
  }
  cb(discovered);
  discovery_loader.reset();
}
}  // namespace
void Self::Discover(std::function<void(std::vector<std::string>)> cb) {
  if (!loader_factory_) {
    LOG(INFO) << "Can't discover as I have no loader factory.";
    disc_cb_ = cb;
    return;
  }
  if (discovery_loader) {
    LOG(INFO)
        << "Not issuing a discovery request, as one has already been sent.";
    return;
  }
  auto req = std::make_unique<network::ResourceRequest>();
  req->url = GURL{"https://orchestrator.strn.pl/nodes/nearby"};
  discovery_loader = network::SimpleURLLoader::Create(
      std::move(req), kTrafficAnnotation, FROM_HERE);
  auto bound = base::BindOnce(&parse_discover_response, cb);
  LOG(INFO) << "Issuing discovery request to: "
               "https://orchestrator.strn.pl/nodes/nearby";
  discovery_loader->DownloadToString(loader_factory_, std::move(bound), MB);
}

auto Self::InitiateGatewayRequest(BusyGateway assigned)
    -> std::shared_ptr<GatewayRequest> {
  auto url = assigned.url();

  GOOGLE_DCHECK_GT(url.size(), 0U);
  auto url_suffix = assigned.task();
  auto req = std::make_unique<network::ResourceRequest>();
  req->url = GURL{url};
  req->priority = net::HIGHEST;  // TODO
  auto out = std::make_shared<GatewayUrlLoader>(std::move(assigned));
  GOOGLE_DCHECK_GT(out->gateway->url_prefix().size(), 0U);
  out->loader = network::SimpleURLLoader::Create(std::move(req),
                                                 kTrafficAnnotation, FROM_HERE);
  if (url.find("format=ipns-record") == std::string::npos) {
    out->loader->SetTimeoutDuration(base::Seconds(32));
  } else {
    LOG(INFO) << "Doing an IPNS record query, so giving it a long timeout.";
    out->loader->SetTimeoutDuration(base::Seconds(256));
  }
  auto start_time = base::TimeTicks::Now();
  //  out->listener = listener;
  auto cb = base::BindOnce(&Self::OnResponse, base::Unretained(this),
                           shared_from_this(), out, start_time);
  DCHECK(loader_factory_);
  // TODO - proper requesting with full features (SetPriority, etc.).
  out->loader->DownloadToString(loader_factory_, std::move(cb), 2UL * MB);
  return out;
}
void Self::OnResponse(std::shared_ptr<ContextApi> api,
                      std::shared_ptr<GatewayUrlLoader> req,
                      base::TimeTicks start_time,
                      std::unique_ptr<std::string> body) {
  DCHECK(req);
  auto task = req->task();
  if (task.empty()) {
    LOG(ERROR) << "Got a response for an empty task!";
    return;
  }
  auto url = req->url();
  //  LOG(INFO) << "Got a response for " << task << " via " << url;
  auto& bg = req->gateway;
  auto& ldr = req->loader;
  //  auto listener = req->listener;
  if (ProcessResponse(bg, ldr.get(), body.get(), start_time)) {
    bg.Success(state_->gateways(), shared_from_this());
  } else {
    bg.Failure(state_->gateways(), shared_from_this());
  }
  state_->storage().CheckListening();
  state_->scheduler().IssueRequests(api);
}

bool Self::ProcessResponse(BusyGateway& gw,
                           network::SimpleURLLoader* ldr,
                           std::string* body,
                           base::TimeTicks start_time) {
  auto end_time = base::TimeTicks::Now();
  if (!gw) {
    LOG(ERROR) << "No gateway.";
    return false;
  }
  LOG(INFO) << "ProcessResponse(" << gw.url() << ')';
  if (!ldr) {
    LOG(ERROR) << "No loader for processing " << gw.url();
    return false;
  }
  // LOG(INFO) << "Neterror(" << ldr->NetError() << ')';
  if (!body) {
    //    LOG(INFO) << "ProcessResponse(" << gw.url()
    //              << ") Null body - presumably http error.\n";
    return false;
  }
  network::mojom::URLResponseHead const* head = ldr->ResponseInfo();
  if (!head) {
    LOG(INFO) << "ProcessResponse(" << gw.url() << ") Null head.\n";
    return false;
  }
  GOOGLE_DCHECK_LT(gw.url().find("?format="), gw.url().size());
  std::string reported_content_type;
  head->headers->EnumerateHeader(nullptr, "Content-Type",
                                 &reported_content_type);
  // application/vnd.ipld.raw
  //  -- OR --
  // application/vnd.ipfs.ipns-record
  constexpr std::string_view content_type_prefix{"application/vnd.ip"};
  if (reported_content_type.compare(0, content_type_prefix.size(),
                                    content_type_prefix)) {
    LOG(WARNING) << '\n'
                 << gw.url() << " reported a content type of "
                 << reported_content_type
                 << " strongly implying that it's a full request, not a single "
                    "block. TODO: Remove "
               << gw->url_prefix() << " from list of gateways?\n";
    state_->gateways().demote(gw->url_prefix());
    return false;
  }
  auto cid_str = gw.task();
  cid_str.erase(0, 5);  // ipfs/
  cid_str.erase(cid_str.find('?'));
  if (state_->storage().Get(cid_str)) {
    // LOG(INFO) << "Got multiple successful responses for " << cid_str;
    return true;
  }
  auto cid = libp2p::multi::ContentIdentifierCodec::fromString(cid_str);
  if (!cid.has_value()) {
    LOG(ERROR) << "Invalid CID '" << cid_str << "'.";
    return false;
  }
  auto duration = (end_time - start_time).InMillisecondsRoundedUp();
  if (cid.value().content_type ==
      libp2p::multi::MulticodecType::Code::LIBP2P_KEY) {
    auto as_peer = libp2p::peer::PeerId::fromHash(cid.value().content_address);
    if (!as_peer.has_value()) {
      LOG(INFO) << cid_str
                << " has the codec of being a libp2p key, like one would "
                   "expect of a Peer ID, but it does not parse as a Peer ID.";
      return false;
    }
    auto* bytes = reinterpret_cast<Byte const*>(body->data());
    auto record =
        ValidateIpnsRecord({bytes, body->size()}, as_peer.value(),
                           crypto_api::VerifySignature, ParseCborIpns);
    if (!record.has_value()) {
      LOG(ERROR) << "IPNS record failed to validate! From: " << gw.url();
      return false;
    }
    LOG(INFO) << "IPNS record from " << gw.url() << " points " << cid_str
              << " to " << record.value().value;
    ValidatedIpns entry{record.value()};
    entry.resolution_ms = duration;
    entry.gateway_source = gw->url_prefix();
    state_->names().AssignName(cid_str, std::move(entry));
    scheduler().IssueRequests(shared_from_this());
    return true;
  } else {
    Block block{cid.value(), *body};
    if (block.cid_matches_data()) {
      head->headers->SetHeader("Block-Source",
                               cid_str + ", " + gw->url_prefix() + " @" +
                                   std::to_string(std::time(nullptr)));
      VLOG(1) << "L3: Storing CID=" << cid_str;
      // Note this header is added _after_ storing in long-term cache
      head->headers->SetHeader(
          "Server-Timing",
          "gateway-fetch-" + cid_str + ";desc=\"" + gw->url_prefix() +
              " : load over http(s)\";dur=" + std::to_string(duration));
      state_->storage().Store(cid_str, cid.value(),
                              head->headers->raw_headers(), *body);
      scheduler().IssueRequests(shared_from_this());
      return true;
    } else {
      LOG(ERROR) << "You tried to store some bytes as a block for a CID ("
                 << cid_str << ") that does not correspond to those bytes!";
      // TODO ban the gateway outright
      return false;
    }
  }
}

auto Self::scheduler() -> Scheduler& {
  return sched_;
}
void Self::SetLoaderFactory(network::mojom::URLLoaderFactory& lf) {
  loader_factory_ = &lf;
  if (disc_cb_) {
    LOG(INFO) << "Have loader factory, calling Discover";
    Discover(disc_cb_);
    disc_cb_ = {};
  }
}

std::string Self::MimeType(std::string extension,
                           std::string_view content,
                           std::string const& url) const {
  std::string result;
  auto fp_ext = base::FilePath::FromUTF8Unsafe(extension).value();
  VLOG(1) << "extension=" << extension << "content.size()=" << content.size()
          << "(as-if) url for mime type:" << url;
  if (extension.empty()) {
    result.clear();
  } else if (net::GetWellKnownMimeTypeFromExtension(fp_ext, &result)) {
    LOG(INFO) << "Got " << result << " from extension " << extension << " for "
              << url;
  } else {
    result.clear();
  }
  if (net::SniffMimeType({content.data(), content.size()}, GURL{url}, result,
                         net::ForceSniffFileUrlsForHtml::kDisabled, &result)) {
    LOG(INFO) << "Got " << result << " from content of " << url;
  }
  if (result.empty() || result == "application/octet-stream") {
    // C'mon, man
    net::SniffMimeTypeFromLocalData({content.data(), content.size()}, &result);
    LOG(INFO) << "Falling all the way back to content type " << result;
  }
  return result;
}
std::string Self::UnescapeUrlComponent(std::string_view comp) const {
  using Rule = base::UnescapeRule;
  auto rules = Rule::PATH_SEPARATORS |
               Rule::URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS | Rule::SPACES;
  auto result = base::UnescapeURLComponent({comp.data(), comp.size()}, rules);
  VLOG(1) << "UnescapeUrlComponent(" << comp << ")->'" << result << "'";
  return result;
}
void Self::RequestByCid(std::string cid,
                        std::shared_ptr<DagListener> listener,
                        Priority prio) {
  auto me = shared_from_this();
  sched_.Enqueue(me, listener, {}, "ipfs/" + cid + "?format=raw", prio);
  sched_.IssueRequests(me);
}

Self::GatewayRequests(InterRequestState& state)
    : state_{state},
      sched_([this]() { return state_->gateways().GenerateList(); }) {}
Self::~GatewayRequests() {
  LOG(WARNING) << "API dtor - are all URIs loaded?";
}

Self::GatewayUrlLoader::GatewayUrlLoader(BusyGateway&& bg)
    : GatewayRequest(std::move(bg)) {}
Self::GatewayUrlLoader::~GatewayUrlLoader() noexcept {}
