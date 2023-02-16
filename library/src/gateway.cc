#include "ipfs_client/gateway.h"
#include <iostream>

ipfs::Gateway::Gateway(std::string url_prefix, unsigned int priority)
    : prefix_{std::move(url_prefix)}, priority_{priority} {}
ipfs::Gateway::Gateway(Gateway const& other)
    : prefix_{other.prefix_}, priority_{other.priority_} {}
ipfs::Gateway::~Gateway() {}

// Less means should-be-preferred
bool ipfs::Gateway::operator<(Gateway const& rhs) const {
  if (proven_ != rhs.proven_) {
    return proven_;
  }
  if (failed_requests_.size() != rhs.failed_requests_.size()) {
    return failed_requests_.size() < rhs.failed_requests_.size();
  }
  if (priority_ != rhs.priority_) {
    return priority_ > rhs.priority_;
  }
  return prefix_ < rhs.prefix_;
}
bool ipfs::Gateway::accept(std::string const& suffix) {
  if (tasked_with_.empty() && !PreviouslyFailed(suffix)) {
    std::clog << prefix_ << ".accept(" << suffix << ")\n";
    tasked_with_.assign(suffix);
    return true;
  }
  return false;
}
std::string const& ipfs::Gateway::url_prefix() const {
  return prefix_;
}
std::string ipfs::Gateway::url() const {
  return url_prefix() + tasked_with_;
}
void ipfs::Gateway::TaskSuccess() {
  std::clog << url() << " TaskSuccess\n";
  tasked_with_.clear();
  ++priority_;
  proven_ = true;
}
void ipfs::Gateway::TaskFailed() {
  std::clog << url() << " TaskFailed\n";
  failed_requests_.insert(tasked_with_);
  priority_ /= 2;
  tasked_with_.clear();
}
void ipfs::Gateway::TaskCancelled() {
  std::clog << url() << " TaskCancelled\n";
  tasked_with_.clear();
}
std::string const& ipfs::Gateway::current_task() const {
  return tasked_with_;
}
bool ipfs::Gateway::PreviouslyFailed(const std::string& suffix) const {
  return failed_requests_.find(suffix) != failed_requests_.end();
}
