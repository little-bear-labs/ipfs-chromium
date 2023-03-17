#ifndef IPFS_GATEWAY_REQUESTS_H_
#define IPFS_GATEWAY_REQUESTS_H_

#include <ipfs_client/block_storage.h>
#include <ipfs_client/framework_api.h>

#include <map>

namespace network {
class SimpleURLLoader;
namespace mojom {
class URLLoaderFactory;
}
}  // namespace network

namespace ipfs {
class InterRequestState;

class GatewayRequests final : public FrameworkApi {
  struct GatewayRequest {
    GatewayRequest(BusyGateway&&);
    GatewayRequest(GatewayRequest&&);
    ~GatewayRequest();
    std::unique_ptr<network::SimpleURLLoader> loader;
    BusyGateway gateway;
  };
  using Outstanding = std::vector<GatewayRequest>;

  network::mojom::URLLoaderFactory* loader_factory_ = nullptr;
  std::map<std::string, Outstanding> outstanding_;
  InterRequestState& state_;
  Scheduler sched_;

  void RequestByCid(std::string cid,
                    std::shared_ptr<DagListener>,
                    Scheduler::Priority) override;
  void InitiateGatewayRequest(BusyGateway,
                              std::shared_ptr<DagListener>) override;
  std::string MimeType(std::string extension,
                       std::string_view content,
                       std::string const& url) const override;
  std::string UnescapeUrlComponent(std::string_view) const override;

  void OnResponse(std::shared_ptr<FrameworkApi>,
                  std::shared_ptr<DagListener>,
                  std::string,
                  std::string,
                  std::unique_ptr<std::string>);
  bool ProcessResponse(Gateway*,
                       std::shared_ptr<DagListener>,
                       network::SimpleURLLoader*,
                       std::string*);

 public:
  GatewayRequests(InterRequestState&);
  ~GatewayRequests();
  void SetLoaderFactory(network::mojom::URLLoaderFactory&);
  Scheduler& scheduler();
};

}  // namespace ipfs

#endif  // IPFS_GATEWAY_REQUESTS_H_
