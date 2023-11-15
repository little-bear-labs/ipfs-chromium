#ifndef IPFS_BLOCK_HTTP_REQUEST_H_
#define IPFS_BLOCK_HTTP_REQUEST_H_

#include <ipfs_client/gw/gateway_request.h>

#include <ipfs_client/context_api.h>
#include <vocab/raw_ptr.h>

namespace network {
struct ResourceRequest;
class SimpleURLLoader;
}  // namespace network
namespace network::mojom {
class URLLoaderFactory;
}

namespace ipfs {
class BlockHttpRequest : public std::enable_shared_from_this<BlockHttpRequest> {
  // TODO ween oneself off of SimpleURLLoader
  //  std::array<char, gw::BLOCK_RESPONSE_BUFFER_SIZE> buffer_;
  std::unique_ptr<network::SimpleURLLoader> loader_;

 public:
  using HttpCompleteCallback = ipfs::ContextApi::HttpCompleteCallback;
  BlockHttpRequest(ipfs::HttpRequestDescription, HttpCompleteCallback);
  ~BlockHttpRequest() noexcept;

  void send(raw_ptr<network::mojom::URLLoaderFactory>);

 private:
  ipfs::HttpRequestDescription const inf_;
  HttpCompleteCallback callback_;

  void OnResponse(std::shared_ptr<BlockHttpRequest>,
                  std::unique_ptr<std::string> body);
};
}  // namespace ipfs

#endif  // IPFS_BLOCK_HTTP_REQUEST_H_
