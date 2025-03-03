#include <ipfs_client/client.h>
#include <memory>
#include <openssl/evp.h>
#include <optional>
#include <vector>
#include <string>
#include <string_view>
#include <utility>

#include "crypto/openssl_sha2_256.h"
#include "ipfs_client/crypto/openssl_signature_verifier.h"
#include "ipfs_client/crypto/identity_hasher.h"
#include "ipfs_client/crypto/signing_key_type.h"
#include "ipfs_client/crypto/signature_verifier.h"
#include "ipfs_client/ctx/http_api.h"
#include "ipfs_client/ctx/dns_txt_lookup.h"
#include "ipfs_client/ctx/gateway_config.h"
#include "ipfs_client/ctx/json_parser.h"
#include "ipfs_client/ctx/cbor_parser.h"
#include "ipfs_client/ctx/null_cbor_parser.h"
#include "ipfs_client/ctx/nlohmann_cbor_parser.h"
#include "ipfs_client/ctx/nlohmann_json_parser.h"
#include "ipfs_client/ctx/null_dns_txt_lookup.h"
#include "ipfs_client/ctx/null_http_provider.h"
#include "ipfs_client/ctx/transitory_gateway_config.h"
#include "ipfs_client/gw/default_requestor.h"

#include "ipfs_client/multi_hash.h"
#include "ipfs_client/json_cbor_adapter.h"
#include "ipfs_client/gw/requestor.h"
#include "ipfs_client/partition.h"
#include "ipfs_client/web_util.h"

#include "log_macros.h"
#include "vocab/byte.h"

using Self = ipfs::Client;

Self::Client() {
#if HAS_OPENSSL_SHA
  hashers_.emplace(HashType::SHA2_256,
                   std::make_unique<crypto::OpensslSha2_256>());
#endif
  hashers_.emplace(HashType::IDENTITY, std::make_unique<crypto::IdentityHasher>());
#if HAS_OPENSSL_EVP
  with(SigningKeyType::RSA,
       std::make_unique<crypto::OpensslSignatureVerifier>(EVP_PKEY_RSA));
  with(SigningKeyType::Ed25519,
       std::make_unique<crypto::OpensslSignatureVerifier>(EVP_PKEY_ED25519));
#endif
}

auto Self::Hash(HashType hash_type, ByteView data)
    -> std::optional<std::vector<Byte>> {
  auto it = hashers_.find(hash_type);
  if (hashers_.end() == it || !(it->second)) {
    return std::nullopt;
  }
  return it->second->hash(data);
}
auto Self::VerifyKeySignature(SigningKeyType typ,
                              ByteView signature,
                              ByteView data,
                              ByteView key_bytes) const -> bool {
  auto it = verifiers_.find(typ);
  if (verifiers_.end() == it || !(it->second)) {
    LOG(ERROR)
        << "Don't have an implementation to verify signature with key of type "
        << static_cast<int>(typ);
    return false;
  }
  return it->second->VerifySignature(signature, data, key_bytes);
}
auto Self::MimeType(std::string extension,
                           std::string_view content,
                           std::string const& url) -> std::string {
  if (!deduce_mime_type_) {
    LOG(WARNING) << "No mime-type deduction algo provided. Will do something "
                    "trivial/inaccurate.";
    deduce_mime_type_ = util::TrivialMimeGuess;
  }
  return deduce_mime_type_(extension, content, url);
}
auto Self::UnescapeUrlComponent(std::string_view url_comp) -> std::string {
  if (!unescape_) {
    LOG(WARNING)
        << "No URL (un)escaping algo provided. Will do something simple/wrong.";
    unescape_ = util::RoughlyUnescapeUrlComponent;
  }
  return unescape_(url_comp);
}
auto Self::DnslinkFallback() const -> bool {
  if (dns_fb_) {
    return dns_fb_();
  }
  return false;
}

auto Self::partition(std::string key) -> std::shared_ptr<Partition> {
  auto& part = partitions_[key];
  if (!part) {
    part = std::make_shared<Partition>(requestor(), shared_from_this());
  }
  return part;
}
auto Self::http() -> ctx::HttpApi& {
  if (!http_api_) {
    http_api_ = std::make_unique<ctx::NullHttpProvider>();
  }
  return *http_api_;
}
auto Self::dns_txt() -> ctx::DnsTxtLookup& {
  if (!dns_txt_) {
    dns_txt_ = std::make_unique<ctx::NullDnsTxtLookup>();
  }
  return *dns_txt_;
}
auto Self::gw_cfg() -> ctx::GatewayConfig& {
  if (!gateway_config_) {
    LOG(INFO) << "No persistent configuration of gateways available.";
    gateway_config_ = std::make_unique<ctx::TransitoryGatewayConfig>();
  }
  return *gateway_config_;
}
auto Self::json() -> ctx::JsonParser& {
  if (!json_parser_) {
#if HAS_JSON_CBOR_ADAPTER
    json_parser_ = std::make_unique<ctx::NlohmannJsonParser>();
#else
    LOG(FATAL) << "A JSON parser must be provided.";
#endif
  }
  DCHECK(!!json_parser_);
  return *json_parser_;
}
auto Self::cbor() -> ctx::CborParser& {
  if (!cbor_parser_) {
#if HAS_JSON_CBOR_ADAPTER
    cbor_parser_ = std::make_unique<ctx::NlohmannCborParser>();
#else
    LOG(ERROR) << "CBOR parsing requested, but no valid parser added to ipfs::Client context!";
    cbor_parser_ = std::make_unique<ctx::NullCborParser>();
#endif
  }
  DCHECK(!!cbor_parser_);
  return *cbor_parser_;
}
auto Self::requestor() -> std::shared_ptr<gw::Requestor> {
  if (!rtor_) {
    LOG(INFO) << "No gateway requestor provided. Will use default with no "
                 "caching or custom behavior.";
    rtor_ = gw::default_requestor({}, shared_from_this());
  }
  return rtor_;
}
auto Self::with(std::unique_ptr<ctx::JsonParser> p) -> Self& {
  json_parser_ = std::move(p);
  return *this;
}
auto Self::with(std::unique_ptr<ctx::CborParser> p) -> Self& {
  cbor_parser_ = std::move(p);
  return *this;
}
auto Self::with(std::unique_ptr<ctx::HttpApi> p) -> Self& {
  http_api_ = std::move(p);
  return *this;
}
auto Self::with(std::unique_ptr<ctx::DnsTxtLookup> p) -> Self& {
  dns_txt_ = std::move(p);
  return *this;
}
auto Self::with(SigningKeyType t,
                 std::unique_ptr<crypto::SignatureVerifier> p) -> Self& {
  if (p) {
    verifiers_[t] = std::move(p);
  } else {
    verifiers_.erase(t);
  }
  return *this;
}
auto Self::with(ipfs::Client::MimeTypeDeduction deduce_mime_type) -> Self& {
  deduce_mime_type_ = deduce_mime_type;
  return *this;
}
auto Self::with(ipfs::Client::UrlUnescaping unescape) -> Self& {
  unescape_ = unescape;
  return *this;
}
auto Self::with(std::unique_ptr<ctx::GatewayConfig> p) -> Self& {
  gateway_config_ = std::move(p);
  return *this;
}
auto Self::with(std::shared_ptr<gw::Requestor> p) -> Self& {
  rtor_ = p;
  return *this;
}
auto Self::with(DnslinkFallbackSwitch sw) -> Self& {
  dns_fb_ = sw;
  return *this;
}
