#include "network_requestor.h"

#include "gateway_requests.h"
#include "inter_request_state.h"

using Self = ipfs::NetworkRequestor;

Self::NetworkRequestor(InterRequestState& state) : state_{state} {}
Self::~NetworkRequestor() noexcept = default;

void Self::RequestByCid(std::string cid,
                        std::shared_ptr<DagListener> listen,
                        Priority prio) {
  auto api = state_.api();
  api->RequestByCid(cid, listen, prio);
}
