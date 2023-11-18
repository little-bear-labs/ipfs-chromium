#include "chromium_ipfs_context.h"

#include "block_http_request.h"
#include "chromium_cbor_adapter.h"
#include "crypto_api.h"
#include "inter_request_state.h"
#include "ipns_cbor.h"

#include <components/cbor/reader.h>
#include <net/base/mime_sniffer.h>
#include <net/base/mime_util.h>
#include <services/network/public/cpp/simple_url_loader.h>
#include "base/strings/escape.h"
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

void Self::SetLoaderFactory(network::mojom::URLLoaderFactory& lf) {
  loader_factory_ = &lf;
}

std::string Self::MimeType(std::string extension,
                           std::string_view content,
                           std::string const& url) const {
  std::string result;
  auto fp_ext = base::FilePath::FromUTF8Unsafe(extension).value();
  VLOG(2) << "extension=" << extension << "content.size()=" << content.size()
          << "(as-if) url for mime type:" << url;
  if (extension.empty()) {
    result.clear();
  } else if (net::GetWellKnownMimeTypeFromExtension(fp_ext, &result)) {
    VLOG(2) << "Got " << result << " from extension " << extension << " for "
            << url;
  } else {
    result.clear();
  }
  auto head_size = std::min(content.size(), 999'999UL);
  if (net::SniffMimeType({content.data(), head_size}, GURL{url}, result,
                         net::ForceSniffFileUrlsForHtml::kDisabled, &result)) {
    VLOG(2) << "Got " << result << " from content of " << url;
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
bool Self::verify_key_signature(SigningKeyType t,
                                ByteView signature,
                                ByteView data,
                                ByteView key_bytes) const {
  return crypto_api::VerifySignature(static_cast<ipns::KeyType>(t), signature,
                                     data, key_bytes);
}
auto Self::ParseCbor(ipfs::ContextApi::ByteView bytes) const
    -> std::unique_ptr<DagCborValue> {
  cbor::Reader::Config cfg;
  cfg.parse_tags = true;
  auto parsed = cbor::Reader::Read(as_octets(bytes), cfg);
  if (parsed.has_value()) {
    return std::make_unique<ChromiumCborAdapter>(std::move(parsed.value()));
  }
  LOG(ERROR) << "Failed to parse CBOR.";
  return {};
}

Self::ChromiumIpfsContext(
    InterRequestState& state,
    raw_ptr<network::mojom::NetworkContext> network_context)
    : network_context_{network_context}, state_{state} {}
Self::~ChromiumIpfsContext() {
  LOG(WARNING) << "API dtor - are all URIs loaded?";
}

