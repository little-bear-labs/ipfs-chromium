#ifndef IPFS_RESPONSE_H_
#define IPFS_RESPONSE_H_

#include <vocab/byte.h>

#include <cstdint>
#include <string>
#include <vector>

namespace ipfs {

struct Response {
  std::string mime_;
  std::uint16_t status_;
  std::string body_;
  std::string location_;

  static Response PLAIN_NOT_FOUND;
  static Response IMMUTABLY_GONE;
  static Response HOST_NOT_FOUND_RESPONSE;
};

}  // namespace ipfs

#endif  // IPFS_RESPONSE_H_
