#include "pre_main_message_loop_run.h"

#include "components/ipfs/inter_request_state.h"

void electron_spin::PreMainMessageLoopRun(content::BrowserContext* c,
                                          PrefService* p) {
  ipfs::InterRequestState::CreateForBrowserContext(c, p);
}
