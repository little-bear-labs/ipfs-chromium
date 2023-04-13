#ifndef IPFS_PLAIN_DIRECTORY_H_
#define IPFS_PLAIN_DIRECTORY_H_

#include <memory>
#include <string_view>

namespace ipfs {
class Block;
class DagListener;
class NetworkingApi;
class UnixFsPathResolver;
namespace unix_fs {
void ProcessDirectory(NetworkingApi& api,
                      std::shared_ptr<DagListener>& listener,
                      UnixFsPathResolver& resolver,
                      std::string_view path,
                      Block const& block);
}
}  // namespace ipfs

#endif  // IPFS_PLAIN_DIRECTORY_H_
