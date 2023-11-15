#include "chromium_ipfs_context.h"

#include "block_http_request.h"
#include "crypto_api.h"
#include "inter_request_state.h"
#include "ipns_cbor.h"

#include <services/network/public/cpp/simple_url_loader.h>
#include "base/strings/escape.h"
#include "net/base/mime_sniffer.h"
#include "net/base/mime_util.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "url/gurl.h"

#include <ipfs_client/dag_block.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/ipns_record.h>

#include <libp2p/multi/content_identifier_codec.hpp>
#include <libp2p/peer/peer_id.hpp>

#include <base/logging.h>

using Self = ipfs::ChromiumIpfsContext;

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
    VLOG(1) << "Got " << result << " from extension " << extension << " for "
            << url;
  } else {
    result.clear();
  }
  auto head_size = std::min(content.size(), 999'999UL);
  if (net::SniffMimeType({content.data(), head_size}, GURL{url}, result,
                         net::ForceSniffFileUrlsForHtml::kDisabled, &result)) {
    VLOG(1) << "Got " << result << " from content of " << url;
  }
  if (result.empty() || result == "application/octet-stream") {
    // C'mon, man
    net::SniffMimeTypeFromLocalData({content.data(), head_size}, &result);
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
void Self::SendDnsTextRequest(std::string host,
                              DnsTextResultsCallback res,
                              DnsTextCompleteCallback don) {
  if (dns_reqs_.find(host) != dns_reqs_.end()) {
    LOG(ERROR) << "Requested resolution of DNSLink host " << host
               << " multiple times.";
  }
  auto don_wrap = [don, this, host]() {
    don();
    LOG(INFO) << "Finished resolving " << host << " via DNSLink";
    dns_reqs_.erase(host);
  };
  dns_reqs_[host] = std::make_unique<DnsTxtRequest>(host, res, don_wrap,
                                                    network_context_.get());
}
void Self::SendHttpRequest(HttpRequestDescription req_inf,
                           HttpCompleteCallback cb) const {
  DCHECK(loader_factory_);
  auto ptr = std::make_shared<BlockHttpRequest>(req_inf, cb);
  ptr->send(loader_factory_);
}
auto Self::deserialize_cbor(ByteView bytes) const -> IpnsCborEntry {
  return ParseCborIpns(bytes);
}
bool Self::verify_key_signature(SigningKeyType t,
                                ByteView signature,
                                ByteView data,
                                ByteView key_bytes) const {
  return crypto_api::VerifySignature(static_cast<ipns::KeyType>(t), signature,
                                     data, key_bytes);
}

Self::ChromiumIpfsContext(
    InterRequestState& state,
    raw_ptr<network::mojom::NetworkContext> network_context)
    : network_context_{network_context}, state_{state} {}
Self::~ChromiumIpfsContext() {
  LOG(WARNING) << "API dtor - are all URIs loaded?";
}

