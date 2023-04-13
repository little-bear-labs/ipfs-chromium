#ifndef IPFS_SMALL_FILE_H_
#define IPFS_SMALL_FILE_H_

#include <memory>
#include <string_view>

namespace ipfs {
class Block;
class DagListener;
class NetworkingApi;
namespace unix_fs {
void ProcessSmallFile(NetworkingApi&,
                      DagListener&,
                      std::string_view path,
                      Block const&);
}
}  // namespace ipfs

#endif  // IPFS_SMALL_FILE_H_
