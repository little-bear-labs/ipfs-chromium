#ifndef ELECTRON_SPIN_PRE_MAIN_MESSAGE_LOOP_RUN_H
#define ELECTRON_SPIN_PRE_MAIN_MESSAGE_LOOP_RUN_H

namespace content {
class BrowserContext;
}
class PrefService;

namespace electron_spin {
void PreMainMessageLoopRun(content::BrowserContext*, PrefService*);
}

#endif  // ELECTRON_SPIN_PRE_MAIN_MESSAGE_LOOP_RUN_H
