#ifndef IPFS_BLOCK_HTTP_REQUEST_H_
#define IPFS_BLOCK_HTTP_REQUEST_H_

#include <ipfs_client/gw/gateway_request.h>

#include <ipfs_client/ctx/http_api.h>
#include <vocab/raw_ptr.h>

namespace network {
struct ResourceRequest;
class SimpleURLLoader;
}  // namespace network
namespace network::mojom {
class URLLoaderFactory;
class URLResponseHead;
}  // namespace network::mojom
class GURL;

namespace ipfs {

/*! Manages lifetime for a single HTTP request to an IPFS gateway.
 *  Not strictly for a block necessarily, thought that was the case when the name was chosen.
 */
class BlockHttpRequest : public std::enable_shared_from_this<BlockHttpRequest> {
  std::unique_ptr<network::SimpleURLLoader> loader_;

 public:
  using HttpCompleteCallback = ctx::HttpApi::OnComplete;

  /*! Initialize a request
   */
  BlockHttpRequest(ipfs::HttpRequestDescription, HttpCompleteCallback);
  ~BlockHttpRequest() noexcept;

  /*! \brief Send the HTTP request to the gateway
   *  \param loader_factory Used to create URL Loaders for HTTP(s)
   */
  void Send(raw_ptr<network::mojom::URLLoaderFactory> loader_factory);
  void Cancel();

 private:
  ipfs::HttpRequestDescription const inf_;
  HttpCompleteCallback callback_;
  std::string status_line_;
  ctx::HttpApi::Hdrs header_accessor_ = [](auto) {
    return std::string{};
  };

  void OnResponseHead(GURL const&, network::mojom::URLResponseHead const&);
  void OnResponse(std::shared_ptr<BlockHttpRequest>,
                  std::unique_ptr<std::string> body);
};
}  // namespace ipfs

#endif  // IPFS_BLOCK_HTTP_REQUEST_H_
