#include "ipfs_url_loader.h"

#include "gateway_requests.h"
#include "inter_request_state.h"

#include "ipfs_client/gateways.h"
#include "ipfs_client/unixfs_path_resolver.h"

#include "base/debug/stack_trace.h"
#include "base/notreached.h"
#include "base/threading/platform_thread.h"
#include "services/network/public/cpp/parsed_headers.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "services/network/url_loader_factory.h"

#include <libp2p/multi/content_identifier_codec.hpp>

#include <fstream>

ipfs::IpfsUrlLoader::IpfsUrlLoader(
    network::mojom::URLLoaderFactory& handles_http,
    InterRequestState& state)
    : state_{state}, lower_loader_factory_{handles_http}, api_{state_.api()} {}
ipfs::IpfsUrlLoader::~IpfsUrlLoader() noexcept {
  LOG(INFO) << "IPFS URLLoader dtor, uri was '" << original_url_ << "'";
}

void ipfs::IpfsUrlLoader::FollowRedirect(
    std::vector<std::string> const&  // removed_headers
    ,
    net::HttpRequestHeaders const&  // modified_headers
    ,
    net::HttpRequestHeaders const&  // modified_cors_exempt_headers
    ,
    absl::optional<::GURL> const&  // new_url
) {
  NOTIMPLEMENTED();
}

void ipfs::IpfsUrlLoader::SetPriority(net::RequestPriority priority,
                                      int32_t intra_priority_value) {
  LOG(INFO) << "TODO SetPriority(" << priority << ',' << intra_priority_value
            << ')';
}

void ipfs::IpfsUrlLoader::PauseReadingBodyFromNet() {
  NOTIMPLEMENTED();
}

void ipfs::IpfsUrlLoader::ResumeReadingBodyFromNet() {
  NOTIMPLEMENTED();
}

void ipfs::IpfsUrlLoader::StartRequest(
    std::shared_ptr<IpfsUrlLoader> me,
    network::ResourceRequest const& resource_request,
    mojo::PendingReceiver<network::mojom::URLLoader> receiver,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client) {
  DCHECK(!me->receiver_.is_bound());
  DCHECK(!me->client_.is_bound());
  me->receiver_.Bind(std::move(receiver));
  me->client_.Bind(std::move(client));
  if (me->original_url_.empty()) {
    me->original_url_ = resource_request.url.spec();
  }
  if (resource_request.url.SchemeIs("ipfs")) {
    auto ref = resource_request.url.spec();
    DCHECK_EQ(ref.substr(0, 7), "ipfs://");
    // TODO these kinds of shenanigans should have their own special utils file
    ref.erase(4, 2);
    auto e = ref.find_first_of("#?");
    if (e < ref.size()) {
      LOG(INFO) << "Dropping params/frags from '" << ref << "'";
      ref.resize(e);
      LOG(INFO) << "Now have '" << ref << "'";
    }
    me->StartUnixFsProc(me, ref);
  } else {
    LOG(ERROR) << "Wrong scheme: " << resource_request.url.scheme();
  }
}

void ipfs::IpfsUrlLoader::StartUnixFsProc(ptr me, std::string_view ipfs_ref) {
  LOG(INFO) << "Requesting " << ipfs_ref << " by blocks.";
  DCHECK_EQ(ipfs_ref.substr(0, 5), "ipfs/");
  auto second_slash = ipfs_ref.find_first_of("/?", 5);
  auto cid = ipfs_ref.substr(5, second_slash - 5);
  second_slash = ipfs_ref.find('/', 5);
  auto qmark = ipfs_ref.find_first_of("?#");
  std::string remainder{"/"};
  if (second_slash < ipfs_ref.size()) {
    remainder.assign(ipfs_ref.substr(second_slash + 1));
  } else if (qmark && qmark < ipfs_ref.size()) {
    remainder.append(ipfs_ref.substr(qmark));
  }
  LOG(INFO) << "cid=" << cid << " remainder=" << remainder;
  me->resolver_ = std::make_shared<UnixFsPathResolver>(
      me->state_.storage(), me->state_.requestor(), std::string{cid}, remainder,
      me->api_);
  me->api_->SetLoaderFactory(lower_loader_factory_);
  me->resolver_->Step(shared_from_this());
}

void ipfs::IpfsUrlLoader::OverrideUrl(GURL u) {
  original_url_ = u.spec();
}

void ipfs::IpfsUrlLoader::BlocksComplete(std::string mime_type) {
  LOG(INFO) << "Resolved from unix-fs dag a file of type: " << mime_type
            << " will report it as " << original_url_;
  if (complete_) {
    return;
  }
  auto result =
      mojo::CreateDataPipe(partial_block_.size(), pipe_prod_, pipe_cons_);
  if (result) {
    LOG(ERROR) << " ERROR: TaskFailed to create data pipe: " << result;
    return;
  }
  complete_ = true;
  auto head = network::mojom::URLResponseHead::New();
  head->mime_type = mime_type;
  std::uint32_t byte_count = partial_block_.size();
  pipe_prod_->WriteData(partial_block_.data(), &byte_count,
                        MOJO_BEGIN_WRITE_DATA_FLAG_ALL_OR_NONE);
  head->content_length = byte_count;
  head->headers =
      net::HttpResponseHeaders::TryToCreate("access-control-allow-origin: *");
  if (!head->headers) {
    LOG(ERROR) << "\n\tFailed to create headers!\n";
  }
  head->headers->SetHeader("Content-Type", mime_type);
  head->headers->SetHeader("Access-Control-Allow-Origin", "*");
  // TODO:
  //   If we're ipns://, ipns_url_loader should have sent us an expiration date
  //   If we're ipfs://, cache eternally?
  //  head->headers->SetHeader("Cache-Control",
  //                           "public, max-age=31449600, immutable");
  head->was_fetched_via_spdy = false;
  for (auto& part_cid : resolver_->involved_cids()) {
    // L_VAR(part_cid);
    AppendGatewayHeaders(part_cid, *head->headers);
  }
  head->parsed_headers =
      network::PopulateParsedHeaders(head->headers.get(), GURL{original_url_});
  LOG(INFO) << "Sending response for " << original_url_ << " with mime type "
            << head->mime_type << " @" << (void*)(this)
      //<< " stack: " << base::debug::StackTrace()
      ;
  client_->OnReceiveResponse(std::move(head), std::move(pipe_cons_),
                             absl::nullopt);
  client_->OnComplete(network::URLLoaderCompletionStatus{});
}

void ipfs::IpfsUrlLoader::FourOhFour(std::string_view cid,
                                     std::string_view path) {
  LOG(ERROR) << "Immutable data 404 for " << cid << '/' << path;
  complete_ = true;
  client_->OnComplete(
      network::URLLoaderCompletionStatus{net::ERR_FILE_NOT_FOUND});
}

void ipfs::IpfsUrlLoader::ReceiveBlockBytes(std::string_view content) {
  partial_block_.append(content);
}

void ipfs::IpfsUrlLoader::AppendGatewayHeaders(std::string const& cid,
                                               net::HttpResponseHeaders& out) {
  VLOG(1) << cid << " was involved in resolving " << original_url_;
  auto* raw = state_.storage().GetHeaders(cid);
  if (!raw || raw->empty()) {
    LOG(ERROR) << "Trouble fetching headers from gateway response " << cid;
    return;
  }
  //  auto gw_heads = net::HttpResponseHeaders::TryToCreate(*raw);
  auto gw_heads = base::MakeRefCounted<net::HttpResponseHeaders>(*raw);
  if (!gw_heads) {
    std::ostringstream escaped;
    for (auto c : *raw) {
      if (std::isgraph(c)) {
        escaped << c;
      } else {
        escaped << '<' << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<unsigned>(c) << '>';
      }
    }
    LOG(ERROR) << "Failed to parse raw string as headers for " << cid << " : "
               << escaped.str();
    return;
  }
  std::size_t i = 0UL;
  std::string name, value;
  while (gw_heads->EnumerateHeaderLines(&i, &name, &value)) {
    VLOG(2) << cid << ' ' << name << ' ' << value;
    if (name == "Server-Timing" || name == "Block-Source") {
      out.AddHeader(name, value);
    } else {
      out.SetHeader("ZZZ-" + cid + "-" + name, value);
    }
  }
}
