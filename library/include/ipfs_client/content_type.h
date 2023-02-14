#ifndef IPFS_CONTENT_TYPE_H_
#define IPFS_CONTENT_TYPE_H_

#include <string>
#include <string_view>

namespace ipfs {
std::string GuessContentType(std::string_view filename,
                             std::string_view content);
}

#endif  // IPFS_CONTENT_TYPE_H_
