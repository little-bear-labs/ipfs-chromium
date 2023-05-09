#ifndef IPFS_GUESS_CONTENT_TYPE_H_
#define IPFS_GUESS_CONTENT_TYPE_H_

#include <string>
#include <string_view>

namespace ipfs {
class ContextApi;
namespace unix_fs {
std::string GuessContentType(ContextApi& api,
                             std::string_view path,
                             std::string_view content);
}
}  // namespace ipfs

#endif  // IPFS_GUESS_CONTENT_TYPE_H_
