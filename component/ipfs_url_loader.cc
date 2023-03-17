#include "ipfs_url_loader.h"

#include "gateway_requests.h"
#include "inter_request_state.h"

#include "ipfs_client/gateways.h"
#include "ipfs_client/unixfs_path_resolver.h"

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
  auto qmark = ipfs_ref.find('?');
  std::string remainder{"/"};
  if (second_slash < ipfs_ref.size()) {
    remainder.assign(ipfs_ref.substr(second_slash + 1));
  } else if (qmark && qmark < ipfs_ref.size()) {
    remainder.append(ipfs_ref.substr(qmark));
  }
  LOG(INFO) << "cid=" << cid << " remainder=" << remainder;
  me->resolver_ = std::make_shared<UnixFsPathResolver>(
      me->state_.storage(), me->api_->scheduler(), std::string{cid}, remainder,
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
  head->parsed_headers =
      network::PopulateParsedHeaders(head->headers.get(), GURL{original_url_});
  head->was_fetched_via_spdy = false;
  client_->OnReceiveResponse(std::move(head), std::move(pipe_cons_),
                             absl::nullopt);
  client_->OnComplete(network::URLLoaderCompletionStatus{});
  complete_ = true;
}

void ipfs::IpfsUrlLoader::FourOhFour(std::string_view cid,
                                     std::string_view path) {
  LOG(ERROR) << "Immutable data 404 for " << cid << '/' << path;
  complete_ = true;
  client_->OnComplete(network::URLLoaderCompletionStatus{404});
}

void ipfs::IpfsUrlLoader::ReceiveBlockBytes(std::string_view content) {
  partial_block_.append(content);
}
