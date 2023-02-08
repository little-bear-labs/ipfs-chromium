#ifndef IPFS_SCHEDULER_H_
#define IPFS_SCHEDULER_H_

#include "gateways.h"

#include <memory>

namespace ipfs {
    using BusyGateway = std::unique_ptr<Gateway,void(*)(Gateway*)>;
    class Scheduler
    {
        GatewayList good_ = {};
        GatewayList unproven_;
    public:
        explicit Scheduler(GatewayList&& initial_list);
        ~Scheduler();
        enum Result {
            Scheduled,
            InProgress,
            Failed
        };
        std::pair<Result,BusyGateway> schedule(std::string const& suffix);
    };
}

#endif  // IPFS_SCHEDULER_H_
