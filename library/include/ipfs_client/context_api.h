#ifndef IPFS_CONTEXT_API_H_
#define IPFS_CONTEXT_API_H_

#include "busy_gateway.h"

#include <functional>
#include <memory>

namespace ipfs {

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

/**
 * \brief Interface that provides functionality from whatever
 *    environment you're using this library in.
 * \note  A user of this library must implement this, but will probably do so
 *    only once.
 */
class ContextApi : public std::enable_shared_from_this<ContextApi> {
 public:
  ///< Send a single http request to its gateway as scheduled
  virtual std::shared_ptr<GatewayRequest> InitiateGatewayRequest(
      BusyGateway) = 0;

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
};

}  // namespace ipfs

#endif
