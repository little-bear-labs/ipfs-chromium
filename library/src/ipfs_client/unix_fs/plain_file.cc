#include "plain_file.h"

#include <ipfs_client/dag_block.h>
#include <ipfs_client/context_api.h>
#include <ipfs_client/unixfs_path_resolver.h>

#include "guess_content_type.h"

#include "log_macros.h"

using Self = ipfs::unix_fs::PlainFile;

Self::PlainFile() {}

bool Self::Process(std::unique_ptr<NodeHelper>&,
                   std::shared_ptr<DagListener> listener,
                   std::function<void(std::string, Priority)>,
                   std::string& target_cid) {
  listener->ReceiveBlockBytes(block()->chunk_data());
  listener->BlocksComplete(GuessContentType(*api_, resolver_->original_path(),
                                            block()->chunk_data()));
  target_cid.clear();
  return true;
}

Self::~PlainFile() noexcept {}
