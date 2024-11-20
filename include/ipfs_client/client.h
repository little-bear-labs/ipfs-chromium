#ifndef IPFS_CONTEXT_API_H_
#define IPFS_CONTEXT_API_H_

#include "crypto/hasher.h"
#include "crypto/signature_verifier.h"
#include "crypto/signing_key_type.h"
#include "ctx/cbor_parser.h"
#include "ctx/dns_txt_lookup.h"
#include "ctx/gateway_config.h"
#include "ctx/http_api.h"
#include "ctx/json_parser.h"
#include "dag_cbor_value.h"
#include "gw/requestor.h"
#include "http_request_description.h"
#include "ipns_cbor_entry.h"
#include "multi_hash.h"

#include <vocab/byte_view.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>

namespace boost::asio {
class io_context;
}

namespace ipfs {
class Partition;
class IpfsRequest;
class DagJsonValue;
class DagCborValue;

/**
 * \brief Interface that provides functionality from whatever
 *    environment you're using this library in.
 * \note  A user of this library must implement this, but will probably do so
 *    only once.
 */
class Client : public std::enable_shared_from_this<Client> {
 public:
  using SigningKeyType = ::ipfs::crypto::SigningKeyType;
  using MimeTypeDeduction = std::function<
      std::string(std::string, std::string_view, std::string const&)>;
  using UrlUnescaping = std::function<std::string(std::string_view)>;

  //TODO this should probably with GatewayConfig stuff
  using DnslinkFallbackSwitch = std::function<bool()>;

  Client();
  virtual ~Client() noexcept {}

  std::shared_ptr<Partition> partition(std::string key);
  ctx::HttpApi& http();
  ctx::DnsTxtLookup& dns_txt();
  ctx::GatewayConfig& gw_cfg();
  ctx::JsonParser& json();
  ctx::CborParser& cbor();
  std::shared_ptr<gw::Requestor> requestor();

  Client& with(std::unique_ptr<ctx::HttpApi>);
  Client& with(std::unique_ptr<ctx::DnsTxtLookup>);
  Client& with(std::unique_ptr<ctx::GatewayConfig>);
  Client& with(std::unique_ptr<ctx::JsonParser>);
  Client& with(std::unique_ptr<ctx::CborParser>);
  Client& with(std::shared_ptr<gw::Requestor>);
  Client& with(SigningKeyType, std::unique_ptr<crypto::SignatureVerifier>);
  Client& with(MimeTypeDeduction);
  Client& with(UrlUnescaping);
  Client& with(DnslinkFallbackSwitch);

  /*!
   * \brief Determine a mime type for a given file.
   * \param extension - "File extension" not including ., e.g. "html"
   * \param content   - The content of the resource or a large prefix thereof
   * \param url       - A URL it was fetched from (of any sort, ipfs:// is fine)
   */
  std::string MimeType(std::string extension,
                       std::string_view content,
                       std::string const& url);

  /*!
   * \brief Remove URL escaping, e.g. %20
   * \param url_comp - a single component of the URL, e.g. a element of the path
   *    not including /
   * \return The unescaped string
   */
  std::string UnescapeUrlComponent(std::string_view url_comp);

  using ByteView = ::ipfs::ByteView;
  bool VerifyKeySignature(SigningKeyType,
                          ByteView signature,
                          ByteView data,
                          ByteView key_bytes) const;

  std::optional<std::vector<Byte>> Hash(HashType, ByteView data);

  bool DnslinkFallback() const;

 protected:
  std::unordered_map<HashType, std::unique_ptr<crypto::Hasher>> hashers_;
  std::unordered_map<SigningKeyType, std::unique_ptr<crypto::SignatureVerifier>>
      verifiers_;
  std::unique_ptr<ctx::HttpApi> http_api_;
  std::unique_ptr<ctx::DnsTxtLookup> dns_txt_;
  std::unique_ptr<ctx::GatewayConfig> gateway_config_;
  std::unique_ptr<ctx::JsonParser> json_parser_;
  std::unique_ptr<ctx::CborParser> cbor_parser_;
  MimeTypeDeduction deduce_mime_type_;
  UrlUnescaping unescape_;
  std::shared_ptr<gw::Requestor> rtor_;
  std::unordered_map<std::string, std::shared_ptr<Partition>> partitions_;
  DnslinkFallbackSwitch dns_fb_;
};

}  // namespace ipfs

#endif
