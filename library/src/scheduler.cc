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

auto ipfs::Scheduler::schedule(std::string const& suffix) -> std::pair<Result,BusyGateway> {
    auto failure_type = Result::Failed;
    for ( GatewayList* list : {&good_, &unproven_} ) {
        auto match = std::find_if(
            list->begin()
          , list->end()
          , [&suffix](auto&gw){return gw.accept(suffix);}
          );
        if ( match != list->end() ) {
           return {
               Result::Scheduled
             , BusyGateway{&*match, ptr_make_available},
           };
        }
        auto working_on = [suffix](auto&gw){return gw.current_task() == suffix;};
        if ( std::any_of(list->begin(),list->end(),working_on) ) {
            failure_type = Result::InProgress;
        }
    }
    return {
        failure_type
      , BusyGateway{
            nullptr
          , ptr_make_available
          }
      };
}
