#ifndef IPFS_GATEWAY_REQUESTS_H_
#define IPFS_GATEWAY_REQUESTS_H_

#include <ipfs_client/framework_api.h>

namespace ipfs {

class GatewayRequests final : public FrameworkApi {
  void RequestByCid(std::string cid, Scheduler::Priority) override;

  void InitiateGatewayRequest(BusyGateway) override;
  void ReceiveBlockBytes(std::string_view) override;
  void BlocksComplete(std::string mime_type) override;

  std::string MimeType(std::string extension,
                       std::string_view content,
                       std::string const& url) const override;

  std::string UnescapeUrlComponent(std::string_view) const override;
  void FourOhFour(std::string_view, std::string_view) override;
};

}  // namespace ipfs

#endif  // IPFS_GATEWAY_REQUESTS_H_
