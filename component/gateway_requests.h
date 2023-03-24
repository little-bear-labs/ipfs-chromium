#ifndef IPFS_GATEWAY_REQUESTS_H_
#define IPFS_GATEWAY_REQUESTS_H_

#include <ipfs_client/block_storage.h>
#include <ipfs_client/networking_api.h>
#include <ipfs_client/scheduler.h>

#include <map>

namespace network {
class SimpleURLLoader;
namespace mojom {
class URLLoaderFactory;
}
}  // namespace network

namespace ipfs {
class InterRequestState;

class GatewayRequests final : public NetworkingApi {
  struct GatewayUrlLoader : public ipfs::GatewayRequest {
    GatewayUrlLoader(BusyGateway&&);
    GatewayUrlLoader(GatewayRequest&&);
    ~GatewayUrlLoader() noexcept override;
    std::unique_ptr<network::SimpleURLLoader> loader;
  };

  network::mojom::URLLoaderFactory* loader_factory_ = nullptr;
  InterRequestState& state_;
  Scheduler sched_;
  std::function<void(std::vector<std::string>)> disc_cb_;

  void Request(std::string task, std::shared_ptr<DagListener>, Priority);
  void RequestByCid(std::string cid,
                    std::shared_ptr<DagListener>,
                    Priority) override;
  std::shared_ptr<GatewayRequest> InitiateGatewayRequest(BusyGateway) override;
  std::string MimeType(std::string extension,
                       std::string_view content,
                       std::string const& url) const override;
  std::string UnescapeUrlComponent(std::string_view) const override;

  void OnResponse(std::shared_ptr<NetworkingApi>,
                  std::shared_ptr<GatewayUrlLoader>,
                  std::unique_ptr<std::string>);
  bool ProcessResponse(BusyGateway&, network::SimpleURLLoader*, std::string*);

 public:
  GatewayRequests(InterRequestState&);
  ~GatewayRequests();
  void SetLoaderFactory(network::mojom::URLLoaderFactory&);
  Scheduler& scheduler();
  void Discover(std::function<void(std::vector<std::string>)>) override;
};

}  // namespace ipfs

#endif  // IPFS_GATEWAY_REQUESTS_H_
