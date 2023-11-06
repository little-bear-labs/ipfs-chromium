#ifndef IPFS_CONTEXT_API_H_
#define IPFS_CONTEXT_API_H_

#include "busy_gateway.h"
#include "ipns_cbor_entry.h"
#include "signing_key_type.h"

#include <vocab/byte_view.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>

namespace ipfs {
class IpfsRequest;

/*!
 * \brief Represents an issued request
 */
class GatewayRequest {
 public:
  BusyGateway gateway;  ///< The assignment

  GatewayRequest(BusyGateway&&);  ///< Move-only
  virtual ~GatewayRequest() noexcept;
  std::string task() const;  ///< Same as gateway.task()
  std::string url() const;   ///< Same as gateway.url()
};

struct HttpRequestDescription {
  std::string url;
  int timeout_seconds;
  std::string accept;
  std::optional<std::size_t> max_response_size;
  bool operator==(HttpRequestDescription const&) const;
};

/**
 * \brief Interface that provides functionality from whatever
 *    environment you're using this library in.
 * \note  A user of this library must implement this, but will probably do so
 *    only once.
 */
class ContextApi : public std::enable_shared_from_this<ContextApi> {
 public:
  using HttpRequestDescription = ::ipfs::HttpRequestDescription;
  using HeaderAccess = std::function<std::string(std::string_view)>;
  using HttpCompleteCallback =
      std::function<void(std::int16_t, std::string_view, HeaderAccess)>;
  virtual void SendHttpRequest(HttpRequestDescription,
                               HttpCompleteCallback cb) const = 0;

  using DnsTextResultsCallback =
      std::function<void(std::vector<std::string> const&)>;
  using DnsTextCompleteCallback = std::function<void(void)>;
  virtual void SendDnsTextRequest(std::string hostname,
                                  DnsTextResultsCallback,
                                  DnsTextCompleteCallback) = 0;

  /*!
   * \brief Determine a mime type for a given file.
   * \param extension - "File extension" not including ., e.g. "html"
   * \param content   - The content of the resource or a large prefix thereof
   * \param url       - A URL it was fetched from (of any sort, ipfs:// is fine)
   */
  virtual std::string MimeType(std::string extension,
                               std::string_view content,
                               std::string const& url) const = 0;

  /*!
   * \brief Remove URL escaping, e.g. %20
   * \param url_comp - a single component of the URL, e.g. a element of the path
   *    not including /
   * \return The unescaped string
   */
  virtual std::string UnescapeUrlComponent(std::string_view url_comp) const = 0;

  using IpnsCborEntry = ::ipfs::IpnsCborEntry;
  // TODO: accept the Type as a parameter as a reference to a base that knows
  // about its members
  virtual IpnsCborEntry deserialize_cbor(ByteView) const = 0;

  using SigningKeyType = ::ipfs::SigningKeyType;
  using ByteView = ::ipfs::ByteView;
  virtual bool verify_key_signature(SigningKeyType,
                                    ByteView signature,
                                    ByteView data,
                                    ByteView key_bytes) const = 0;

  /*!
   * \brief Discover more gateways.
   * \details The in-chromium implementation of this hits
   *    https://orchestrator.strn.pl/nodes/nearby
   *    Another implementation might kick off an mDNS probe?
   * \param cb - A callback, called with a list of gateway
   *    prefixes, e.g. https://gateway.ipfs.io/
   * \note TRAILING SLASH (/) EXPECTED!
   */
  virtual void Discover(std::function<void(std::vector<std::string>)> cb) = 0;

  // TODO drop
  virtual std::shared_ptr<GatewayRequest> InitiateGatewayRequest(
      BusyGateway) = 0;
};

}  // namespace ipfs

#endif
