#ifndef IPFS_MULTI_GATEWAY_REQUESTOR_H_
#define IPFS_MULTI_GATEWAY_REQUESTOR_H_

#include "gateway_state.h"

#include <ipfs_client/gw/requestor.h>

#include <ipfs_client/client.h>

#include <chrono>
#include <deque>
#include <map>

namespace ipfs::gw {
/*! Sends requests to multiple gateways
 */
class MultiGatewayRequestor : public Requestor {
  std::map<std::string, GatewayState> state_;
  std::deque<RequestPtr> q;
  bool Process(RequestPtr const&);
  void DoSend(RequestPtr, std::string const&, GatewayState&);
  using HeaderAccess = ctx::HttpApi::Hdrs;
  void HandleResponse(HttpRequestDescription const&,
                      RequestPtr,
                      std::string const&,
                      std::int16_t,
                      std::string_view,
                      HeaderAccess,
                      bool,
                      std::chrono::system_clock::time_point);
  void Next();

 public:
  /*! @return "multi-gateway requestor"
   */
  std::string_view name() const override;
  HandleOutcome handle(RequestPtr) override;
};
}  // namespace ipfs::gw

#endif  // IPFS_MULTI_GATEWAY_REQUESTOR_H_
