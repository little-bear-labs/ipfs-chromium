#include "gateway_requests.h"

#include "inter_request_state.h"

#include "base/strings/escape.h"
#include "net/base/mime_sniffer.h"
#include "net/base/mime_util.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "url/gurl.h"

#include <ipfs_client/block.h>
#include <ipfs_client/block_storage.h>
#include <libp2p/multi/content_identifier_codec.hpp>

#include <base/logging.h>

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
void ipfs::GatewayRequests::RequestByCid(std::string cid,
                                         std::shared_ptr<DagListener> listener,
                                         Priority prio) {
  scheduler().Enqueue(shared_from_this(), listener,
                      "ipfs/" + cid + "?format=raw", prio);
  scheduler().IssueRequests(shared_from_this(), listener);
}
auto ipfs::GatewayRequests::InitiateGatewayRequest(
    BusyGateway assigned,
    std::shared_ptr<DagListener> listener) -> std::shared_ptr<GatewayRequest> {
  auto url = assigned->url();
  LOG(INFO) << "InitiateGatewayRequest(" << url << ")";
  GOOGLE_DCHECK_GT(url.size(), 0U);
  auto url_suffix = assigned->current_task();
  auto req = std::make_unique<network::ResourceRequest>();
  req->url = GURL{url};
  req->priority = net::HIGHEST;  // TODO
  auto out = std::make_shared<GatewayUrlLoader>(std::move(assigned));
  GOOGLE_DCHECK_GT(out->gateway->url_prefix().size(), 0U);
  out->loader = network::SimpleURLLoader::Create(std::move(req),
                                                 kTrafficAnnotation, FROM_HERE);
  out->listener = listener;
  auto cb = base::BindOnce(&ipfs::GatewayRequests::OnResponse,
                           base::Unretained(this), shared_from_this(), out);
  DCHECK(loader_factory_);
  // TODO - proper requesting with full features (SetPriority, etc.).
  out->loader->DownloadToStringOfUnboundedSizeUntilCrashAndDie(loader_factory_,
                                                               std::move(cb));
  return out;
}
void ipfs::GatewayRequests::OnResponse(std::shared_ptr<NetworkingApi> api,
                                       std::shared_ptr<GatewayUrlLoader> req,
                                       std::unique_ptr<std::string> body) {
  DCHECK(req);
  auto task = req->task();
  if (task.empty()) {
    LOG(ERROR) << "Got a response for an empty task!";
    return;
  }
  auto url = req->url();
  LOG(INFO) << "Got a response for " << task << " via " << url;
  auto& bg = req->gateway;
  auto& ldr = req->loader;
  auto listener = req->listener;
  if (ProcessResponse(bg.get(), listener, ldr.get(), body.get())) {
    bg.Success(state_.gateways(), shared_from_this(), listener);
  } else {
    bg.Failure(state_.gateways(), shared_from_this(), listener);
  }
}

bool ipfs::GatewayRequests::ProcessResponse(
    Gateway* gw,
    std::shared_ptr<DagListener> listener,
    network::SimpleURLLoader* ldr,
    std::string* body) {
  if (!gw) {
    LOG(ERROR) << "No gateway.";
    return false;
  }
  LOG(INFO) << "ProcessBlockResponse(" << gw->url() << ')';
  if (!ldr) {
    LOG(ERROR) << "No loader for processing " << gw->url();
    return false;
  }
  if (!body) {
    LOG(INFO) << "ProcessBlockResponse(" << gw->url()
              << ") Null body - presumably http error.\n";
    return false;
  }
  network::mojom::URLResponseHead const* head = ldr->ResponseInfo();
  if (!head) {
    LOG(INFO) << "ProcessBlockResponse(" << gw->url() << ") Null head.\n";
    return false;
  }
  GOOGLE_DCHECK_LT(gw->url().find("?format=raw"), gw->url().size());
  std::string reported_content_type;
  head->headers->EnumerateHeader(nullptr, "Content-Type",
                                 &reported_content_type);
  if (reported_content_type != "application/vnd.ipld.raw") {
    LOG(ERROR) << '\n'
               << gw->url() << " reported a content type of "
               << reported_content_type
               << " strongly implying that it's a full request, not a single "
                  "block. TODO: Remove "
               << gw->url_prefix() << " from list of gateways?\n";
    state_.gateways().demote(gw->url_prefix());
    return false;
  }
  auto cid_str = gw->current_task();
  cid_str.erase(0, 5);  // ipfs/
  cid_str.erase(cid_str.find('?'));
  if (state_.storage().Get(cid_str)) {
    LOG(INFO) << "Got multiple successful responses for " << cid_str;
    return true;
  }
  auto cid = libp2p::multi::ContentIdentifierCodec::fromString(cid_str);
  if (!cid.has_value()) {
    LOG(ERROR) << "Invalid CID '" << cid_str << "'.";
    return false;
  }
  Block block{cid.value(), *body};
  if (block.cid_matches_data()) {
    LOG(INFO) << "Storing CID=" << cid_str;
    state_.storage().Store(listener, cid_str, std::move(block));
    // resolver_->Step(); TODO - is this needed?
    scheduler().IssueRequests(shared_from_this(), listener);
    return true;
  } else {
    LOG(ERROR) << "You tried to store some bytes as a block for a CID ("
               << cid_str << ") that does not correspond to those bytes!";
    // TODO ban the gateway outright
    return false;
  }
}

auto ipfs::GatewayRequests::scheduler() -> Scheduler& {
  return sched_;
}
void ipfs::GatewayRequests::SetLoaderFactory(
    network::mojom::URLLoaderFactory& lf) {
  loader_factory_ = &lf;
}

std::string ipfs::GatewayRequests::MimeType(std::string extension,
                                            std::string_view content,
                                            std::string const& url) const {
  std::string result;
  if (extension.size() &&
      net::GetWellKnownMimeTypeFromExtension(extension, &result)) {
    LOG(INFO) << "Got " << result << " from extension " << extension << " for "
              << url;
  } else {
    result.clear();
  }
  if (net::SniffMimeType({content.data(), content.size()}, GURL{url}, result,
                         net::ForceSniffFileUrlsForHtml::kDisabled, &result)) {
    LOG(INFO) << "Got " << result << " from content of " << url;
  }
  return result;
}
std::string ipfs::GatewayRequests::UnescapeUrlComponent(
    std::string_view comp) const {
  using Rule = base::UnescapeRule;
  auto rules = Rule::PATH_SEPARATORS |
               Rule::URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS | Rule::SPACES;
  auto result = base::UnescapeURLComponent({comp.data(), comp.size()}, rules);
  LOG(INFO) << "UnescapeUrlComponent(" << comp << ")->'" << result << "'";
  return result;
}

ipfs::GatewayRequests::GatewayRequests(InterRequestState& state)
    : state_{state}, sched_(state_.gateways().GenerateList()) {}
ipfs::GatewayRequests::~GatewayRequests() {
  LOG(WARNING) << "API dtor - are all URIs loaded?";
}

ipfs::GatewayRequests::GatewayUrlLoader::GatewayUrlLoader(BusyGateway&& bg)
    : GatewayRequest(std::move(bg)) {}
ipfs::GatewayRequests::GatewayUrlLoader::~GatewayUrlLoader() {}
