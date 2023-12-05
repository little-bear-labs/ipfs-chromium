#ifndef IPFS_CONTEXT_API_H_
#define IPFS_CONTEXT_API_H_

#include "dag_cbor_value.h"
#include "http_request_description.h"
#include "ipns_cbor_entry.h"
#include "signing_key_type.h"

#include <vocab/byte_view.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>

namespace ipfs {
class IpfsRequest;
class DagJsonValue;

/**
 * \brief Interface that provides functionality from whatever
 *    environment you're using this library in.
 * \note  A user of this library must implement this, but will probably do so
 *    only once.
 */
class ContextApi : public std::enable_shared_from_this<ContextApi> {
 public:
  virtual ~ContextApi() noexcept {}

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

  virtual std::unique_ptr<DagCborValue> ParseCbor(ByteView) const = 0;
  virtual std::unique_ptr<DagJsonValue> ParseJson(std::string_view) const = 0;

  using IpnsCborEntry = ::ipfs::IpnsCborEntry;

  using SigningKeyType = ::ipfs::SigningKeyType;
  using ByteView = ::ipfs::ByteView;
  virtual bool verify_key_signature(SigningKeyType,
                                    ByteView signature,
                                    ByteView data,
                                    ByteView key_bytes) const = 0;

};

}  // namespace ipfs

#endif
