#include "chromium_ipfs_context.h"

#include "block_http_request.h"
#include "chromium_cbor_adapter.h"
#include "chromium_dns_txt_lookup.h"
#include "chromium_http.h"
#include "chromium_json_adapter.h"
#include "inter_request_state.h"
#include "json_parser_adapter.h"

#include <services/network/public/cpp/simple_url_loader.h>
#include <services/network/public/mojom/url_response_head.mojom.h>

#include <net/base/mime_sniffer.h>
#include <net/base/mime_util.h>

#include <url/gurl.h>

#include <base/strings/escape.h>

#include <base/logging.h>

#include <ipfs_client/crypto/openssl_signature_verifier.h>
#include <ipfs_client/ipfs_request.h>

namespace {
std::string DeduceMimeType(std::string extension,
                           std::string_view content,
                           std::string const& url) {
  std::string result;
  auto fp_ext = base::FilePath::FromUTF8Unsafe(extension).value();
  if (extension.empty()) {
    result.clear();
  } else if (!net::GetWellKnownMimeTypeFromExtension(fp_ext, &result)) {
    result.clear();
  }
  auto head_size = std::min(content.size(), static_cast<std::size_t>(999'999));
  net::SniffMimeType({content.data(), head_size}, GURL{url}, result,
                     net::ForceSniffFileUrlsForHtml::kDisabled, &result);
  if (result.empty() || result == "application/octet-stream") {
    net::SniffMimeTypeFromLocalData({content.data(), head_size}, &result);
  }
  return result;
}
std::string Unescape(std::string_view comp) {
  using Rule = base::UnescapeRule;
  auto rules = Rule::PATH_SEPARATORS |
               Rule::URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS | Rule::SPACES;
  auto result = base::UnescapeURLComponent({comp.data(), comp.size()}, rules);
  return result;
}
}  // namespace

auto ipfs::CreateContext(InterRequestState& stat, PrefService* pref)
    -> std::shared_ptr<Client> {
  LOG(WARNING) << __PRETTY_FUNCTION__;
  using K = crypto::SigningKeyType;
  auto result = std::make_shared<Client>();
  result->with(std::make_unique<ChromiumIpfsGatewayConfig>(pref))
      .with(std::make_unique<ChromiumDnsTxtLookup>(stat))
      .with(&DeduceMimeType)
      .with(&Unescape)
      .with(std::make_unique<ChromiumCborAdapter>())
      .with(std::make_unique<JsonParserAdapter>())
      .with(K::RSA,
            std::make_unique<crypto::OpensslSignatureVerifier>(EVP_PKEY_RSA))
      .with(K::Ed25519, std::make_unique<crypto::OpensslSignatureVerifier>(
                            EVP_PKEY_ED25519))
      .with([pref](){return DnsFallbackPref(pref);})
      ;
  return result;
}
