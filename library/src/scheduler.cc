#include <ipfs_client/scheduler.h>

#include <algorithm>

ipfs::Scheduler::Scheduler(GatewayList&& initial_list)
: unproven_{initial_list}
{
}
ipfs::Scheduler::~Scheduler() {}

void ptr_make_available(ipfs::Gateway* p) {
    p->make_available();
}

auto ipfs::Scheduler::schedule(std::string const& suffix) -> BusyGateway {
    for ( GatewayList* list : {&good_, &unproven_} ) {
        auto match = std::find_if(
            list->begin()
          , list->end()
          , [&suffix](auto&gw){return gw.accept(suffix);}
          );
        if ( match != list->end() ) {
           return {
                &*match
              , ptr_make_available
              };
        }
    }
    return BusyGateway{
          nullptr
        , ptr_make_available
    };
}
