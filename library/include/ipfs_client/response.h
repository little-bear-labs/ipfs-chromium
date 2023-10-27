#ifndef IPFS_RESPONSE_H_
#define IPFS_RESPONSE_H_

#include <cstdint>

#include <string>

namespace ipfs {

struct Response {
  std::string mime_;
  std::uint16_t status_;
  std::string body_;
  std::string location_;

  static Response PLAIN_NOT_FOUND;
};

}  // namespace ipfs

#endif  // IPFS_RESPONSE_H_
