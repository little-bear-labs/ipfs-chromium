#ifndef IPFS_STRINGIFY_H_
#define IPFS_STRINGIFY_H_

#include <sstream>

namespace ipfs {
namespace {
template <class T>
std::string Stringify(T const& t) {
  std::ostringstream oss;
  oss << t;
  return oss.str();
}
}  // namespace
}  // namespace ipfs

#endif  // IPFS_STRINGIFY_H_
