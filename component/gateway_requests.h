#ifndef IPFS_GATEWAY_REQUESTS_H_
#define IPFS_GATEWAY_REQUESTS_H_

#include <ipfs_client/block_storage.h>
#include <ipfs_client/context_api.h>
#include <ipfs_client/scheduler.h>

#include <base/memory/raw_ref.h>
#include <base/time/time.h>

#include <vocab/raw_ptr.h>

#include <map>

namespace network {
class SimpleURLLoader;
namespace mojom {
class URLLoaderFactory;
}
}  // namespace network

namespace ipfs {
class InterRequestState;
class IpfsRequest;
class NetworkRequestor;

class GatewayRequests final : public ContextApi {
  struct GatewayUrlLoader : public ipfs::GatewayRequest {
    GatewayUrlLoader(BusyGateway&&);
    GatewayUrlLoader(GatewayRequest&&);
    ~GatewayUrlLoader() noexcept override;
    std::unique_ptr<network::SimpleURLLoader> loader;
  };

  raw_ptr<network::mojom::URLLoaderFactory> loader_factory_ = nullptr;
  raw_ref<InterRequestState> state_;
  Scheduler sched_;
  std::function<void(std::vector<std::string>)> disc_cb_;

  void Request(std::string task, std::shared_ptr<DagListener>, Priority);
  std::shared_ptr<GatewayRequest> InitiateGatewayRequest(BusyGateway) override;
  std::string MimeType(std::string extension,
                       std::string_view content,
                       std::string const& url) const override;
  std::string UnescapeUrlComponent(std::string_view) const override;

  void OnResponse(std::shared_ptr<ContextApi>,
                  std::shared_ptr<GatewayUrlLoader>,
                  base::TimeTicks,
                  std::unique_ptr<std::string>);
  bool ProcessResponse(BusyGateway&,
                       network::SimpleURLLoader*,
                       std::string*,
                       base::TimeTicks);
  friend class NetworkRequestor;

  void RequestByCid(std::string cid,
                    std::shared_ptr<DagListener>,
                    Priority);

 public:
  GatewayRequests(InterRequestState&);
  ~GatewayRequests();
  void SetLoaderFactory(network::mojom::URLLoaderFactory&);
  Scheduler& scheduler();
  void Discover(std::function<void(std::vector<std::string>)>) override;
};

}  // namespace ipfs

#endif  // IPFS_GATEWAY_REQUESTS_H_
