#include <ipfs_client/client.h>

#include "crypto/openssl_sha2_256.h"
#include "ipfs_client/crypto/openssl_signature_verifier.h"
#include "ipfs_client/ctx/boost_beast_http.h"
#include "ipfs_client/ctx/nlohmann_cbor_parser.h"
#include "ipfs_client/ctx/nlohmann_json_parser.h"
#include "ipfs_client/ctx/null_dns_txt_lookup.h"
#include "ipfs_client/ctx/null_http_provider.h"
#include "ipfs_client/ctx/transitory_gateway_config.h"
#include "ipfs_client/gw/default_requestor.h"

#include "ipfs_client/partition.h"
#include "ipfs_client/web_util.h"

#include "log_macros.h"

using Self = ipfs::Client;

Self::Client() {
#if HAS_OPENSSL_SHA
  hashers_.emplace(HashType::SHA2_256,
                   std::make_unique<crypto::OpensslSha2_256>());
#endif
#if HAS_OPENSSL_EVP
  with(SigningKeyType::RSA,
       std::make_unique<crypto::OpensslSignatureVerifier>(EVP_PKEY_RSA));
  with(SigningKeyType::Ed25519,
       std::make_unique<crypto::OpensslSignatureVerifier>(EVP_PKEY_ED25519));
#endif
}

auto Self::Hash(HashType ht, ByteView data)
    -> std::optional<std::vector<Byte>> {
  auto it = hashers_.find(ht);
  if (hashers_.end() == it || !(it->second)) {
    return std::nullopt;
  }
  return it->second->hash(data);
}
bool Self::VerifyKeySignature(SigningKeyType typ,
                              ByteView signature,
                              ByteView data,
                              ByteView key_bytes) const {
  auto it = verifiers_.find(typ);
  if (verifiers_.end() == it || !(it->second)) {
    LOG(ERROR)
        << "Don't have an implementation to verify signature with key of type "
        << static_cast<int>(typ);
    return false;
  }
  return it->second->VerifySignature(signature, data, key_bytes);
}
std::string Self::MimeType(std::string extension,
                           std::string_view content,
                           std::string const& url) {
  if (!deduce_mime_type_) {
    LOG(WARNING) << "No mime-type deduction algo provided. Will do something "
                    "trivial/inaccurate.";
    deduce_mime_type_ = util::TrivialMimeGuess;
  }
  return deduce_mime_type_(extension, content, url);
}
std::string Self::UnescapeUrlComponent(std::string_view url_comp) {
  if (!unescape_) {
    LOG(WARNING)
        << "No URL (un)escaping algo provided. Will do something simple/wrong.";
    unescape_ = util::RoughlyUnescapeUrlComponent;
  }
  return unescape_(url_comp);
}

auto Self::partition(std::string key) -> std::shared_ptr<Partition> {
  auto& part = partitions_[key];
  if (!part) {
    VLOG(1) << "New partition for key '" << key << "'.";
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
  DCHECK(json_parser_);
  return *json_parser_;
}
auto Self::cbor() -> ctx::CborParser& {
  if (!cbor_parser_) {
#if HAS_JSON_CBOR_ADAPTER
    cbor_parser_ = std::make_unique<ctx::NlohmannCborParser>();
#else
    LOG(FATAL) << "A CBOR parser must be provided.";
#endif
  }
  DCHECK(cbor_parser_);
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
Self& Self::with(std::unique_ptr<ctx::JsonParser> p) {
  json_parser_ = std::move(p);
  return *this;
}
Self& Self::with(std::unique_ptr<ctx::CborParser> p) {
  cbor_parser_ = std::move(p);
  return *this;
}
Self& Self::with(std::unique_ptr<ctx::HttpApi> p) {
  http_api_ = std::move(p);
  return *this;
}
Self& Self::with(std::unique_ptr<ctx::DnsTxtLookup> p) {
  dns_txt_ = std::move(p);
  return *this;
}
Self& Self::with(SigningKeyType t,
                 std::unique_ptr<crypto::SignatureVerifier> p) {
  if (p) {
    verifiers_[t] = std::move(p);
  } else {
    verifiers_.erase(t);
  }
  return *this;
}
Self& Self::with(ipfs::Client::MimeTypeDeduction deduce_mime_type) {
  deduce_mime_type_ = deduce_mime_type;
  return *this;
}
Self& Self::with(ipfs::Client::UrlUnescaping unescape) {
  unescape_ = unescape;
  return *this;
}
Self& Self::with(std::unique_ptr<ctx::GatewayConfig> p) {
  gateway_config_ = std::move(p);
  return *this;
}
Self& Self::with(std::shared_ptr<gw::Requestor> p) {
  rtor_ = p;
  return *this;
}
