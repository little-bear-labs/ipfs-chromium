#ifndef IPFS_GUESS_CONTENT_TYPE_H_
#define IPFS_GUESS_CONTENT_TYPE_H_

#include <string>
#include <string_view>

namespace ipfs {
class NetworkingApi;
namespace unix_fs {
std::string GuessContentType(NetworkingApi& api,
                             std::string_view path,
                             std::string_view content);
}
}  // namespace ipfs

#endif  // IPFS_GUESS_CONTENT_TYPE_H_
