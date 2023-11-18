#include "deduplicating_requestor.h"

#include <ipfs_client/gw/gateway_request.h>

#include "log_macros.h"

using Self = ipfs::gw::DeduplicatingRequestor;

std::string_view Self::name() const {
  return "dedup";
}
auto Self::handle(ipfs::gw::Requestor::RequestPtr r) -> HandleOutcome {
  auto d = r->describe_http();
  if (!d.has_value()) {
    return HandleOutcome::NOT_HANDLED;
  }
  auto& k = d.value();
  auto it = seen_.find(k);
  auto now = std::time(nullptr);
  if (seen_.end() == it) {
    seen_.emplace(k, PastPass{r, now});
    return HandleOutcome::NOT_HANDLED;
  }
  auto& past = it->second;
  if (past.t != now) {
    past.t = now;
    past.ptr = r;
    return HandleOutcome::NOT_HANDLED;
  }
  auto old = past.ptr.lock();
  if (old == r) {
    LOG(INFO)
        << " have seen the EXACT same request pass by (same object in memory).";
    return HandleOutcome::NOT_HANDLED;
  }
  if (old) {
    VLOG(2) << "Dedup squashed a new version of the request "
            << old->debug_string() << " in " << r->debug_string();
    return HandleOutcome::DONE;
  } else {
    LOG(INFO) << r->debug_string()
              << " has occurred before, but the old copy is dead and gone.";
    past.ptr = r;
    past.t = now;
    return HandleOutcome::NOT_HANDLED;
  }
}