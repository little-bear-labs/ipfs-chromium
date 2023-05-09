#include "plain_directory.h"

#include <ipfs_client/dag_block.h>
#include <ipfs_client/context_api.h>
#include <ipfs_client/unixfs_path_resolver.h>

#include "ipfs_client/generated_directory_listing.h"
#include "log_macros.h"

using Self = ipfs::unix_fs::PlainDirectory;

Self::PlainDirectory(std::string next_path_element)
    : next_path_element_{next_path_element} {}

bool Self::Process(std::unique_ptr<NodeHelper>&,
                   std::shared_ptr<DagListener> listener,
                   std::function<void(std::string, Priority)> requestor,
                   std::string& target_cid) {
  block()->List([requestor](auto&, auto cid) {
    // Mild prefetch
    requestor(cid, 0);
    return true;
  });
  auto const old_target = target_cid;
  if (next_path_element_.empty()) {
    // The user's URL actually ends in the directory itself
    block()->List([&target_cid](auto& name, auto cid) {
      if (name == "index.html") {
        target_cid = cid;
        return false;
      }
      return true;
    });
    if (target_cid != old_target) {
      // Found an index.html - will descend to that
      return true;
    }
    ipfs::GeneratedDirectoryListing list{resolver_->original_path()};
    block()->List([&list](auto& name, auto) {
      list.AddEntry(name);
      return true;
    });
    listener->ReceiveBlockBytes(list.Finish());
    listener->BlocksComplete("text/html");
    target_cid.clear();
    return true;
  }
  block()->List([this, &target_cid](auto& name, auto cid) {
    if (name == next_path_element_) {
      target_cid = cid;
      return false;
    }
    return true;
  });
  if (target_cid != old_target) {
    // Found the element in the directory, descending the path.
  } else {
    // There was a specific element requested and it does not exist. 404
    listener->FourOhFour(target_cid, resolver_->original_path());
    target_cid.clear();
  }
  return true;
}

Self::~PlainDirectory() noexcept {}
