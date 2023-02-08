#ifndef IPFS_INTER_REQUEST_STATE_H_
#define IPFS_INTER_REQUEST_STATE_H_

#include "ipfs_client/gateways.h"

#include "base/supports_user_data.h"

namespace ipfs {
  class InterRequestState : public base::SupportsUserData::Data {
      Gateways gws_;

   public:
      Gateways& gateways() { return gws_; }
  };
}

#endif  // IPFS_INTER_REQUEST_STATE_H_
