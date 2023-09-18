#include "ipfs_client/gateway.h"
#include "log_macros.h"

ipfs::Gateway::Gateway(std::string url_prefix, unsigned int priority)
    : prefix_{std::move(url_prefix)}, priority_{priority} {}
ipfs::Gateway::Gateway(Gateway const& other)
    : prefix_{other.prefix_}, priority_{other.priority_} {}
ipfs::Gateway::~Gateway() {}

// Less means should-be-preferred
bool ipfs::Gateway::operator<(Gateway const& rhs) const {
  if (failed_requests_.size() != rhs.failed_requests_.size()) {
    return failed_requests_.size() < rhs.failed_requests_.size();
  }
  if (priority_ != rhs.priority_) {
    return priority_ > rhs.priority_;
  }
  if (tasks_.size() != rhs.tasks_.size()) {
    return tasks_.size() < rhs.tasks_.size();
  }
  return prefix_ < rhs.prefix_;
}
bool ipfs::Gateway::accept(std::string const& suffix, long need) {
  if (need < 0) {
    return false;
  }
  if (static_cast<std::size_t>(need) < tasks_.size() / 2) {
    return false;
  }
  if (priority_ < tasks_.size() * tasks_.size()) {
    return false;
  }
  if (PreviouslyFailed(suffix)) {
    return false;
  }
  return tasks_.insert(suffix).second;
}
std::string const& ipfs::Gateway::url_prefix() const {
  return prefix_;
}
long ipfs::Gateway::load() const {
  return static_cast<long>(tasks_.size());
}
void ipfs::Gateway::TaskSuccess(std::string const& task) {
  tasks_.erase(task);
  ++priority_;
}
void ipfs::Gateway::TaskFailed(std::string const& task) {
  // LOG(INFO) << prefix_ << task << " TaskFailed";
  failed_requests_[task] = std::time(nullptr);
  priority_ /= 2;
  tasks_.erase(task);
}
void ipfs::Gateway::TaskCancelled(std::string const& task) {
  tasks_.erase(task);
}
bool ipfs::Gateway::PreviouslyFailed(std::string const& suffix) const {
  auto it = failed_requests_.find(suffix);
  if (it == failed_requests_.end()) {
    return false;
  }
  return std::time(nullptr) - it->second < 3;
}
