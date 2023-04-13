#include "small_file.h"

#include "guess_content_type.h"

#include <ipfs_client/block.h>
#include <ipfs_client/dag_listener.h>

void ipfs::unix_fs::ProcessSmallFile(NetworkingApi& api,
                                     DagListener& listener,
                                     std::string_view path,
                                     ipfs::Block const& block) {
  listener.ReceiveBlockBytes(block.chunk_data());
  listener.BlocksComplete(GuessContentType(api, path, block.chunk_data()));
}
