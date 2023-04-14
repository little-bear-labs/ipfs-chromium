#include "plain_directory.h"

#include <ipfs_client/dag_block.h>
#include <ipfs_client/networking_api.h>
#include <ipfs_client/unixfs_path_resolver.h>

#include "ipfs_client/generated_directory_listing.h"
#include "log_macros.h"

namespace {
void CompleteDirectory(std::shared_ptr<ipfs::DagListener>& listener,
                       ipfs::Block const& block,
                       std::string_view path,
                       ipfs::UnixFsPathResolver& resolver);
}

void ipfs::unix_fs::ProcessDirectory(NetworkingApi& api,
                                     std::shared_ptr<DagListener>& listener,
                                     UnixFsPathResolver& resolver,
                                     std::string_view path,
                                     Block const& block) {
  block.List([&resolver, &listener](auto&, auto cid) {
    resolver.Request(listener, cid, 0);
    return true;
  });
  auto start = path.find_first_not_of("/");
  if (start == std::string::npos) {
    CompleteDirectory(listener, block, path, resolver);
    return;
  }
  auto end = path.find('/', start);
  std::string_view next_comp = path;
  auto next = api.UnescapeUrlComponent(next_comp.substr(start, end - start));
  auto found = false;
  block.List([&found, &resolver, next](auto& name, auto cid) {
    if (name == next) {
      resolver.current_cid(cid);
      found = true;
      return false;
    }
    return true;
  });
  if (found) {
    LOG(INFO) << "Descending path: " << next << " . " << path;
    if (end < path.size()) {
      resolver.current_path(path.substr(end));
    } else {
      resolver.current_path("");
    }
    resolver.Step(listener);
  } else {
    listener->FourOhFour(resolver.current_cid(), path);
  }
}

namespace {
void CompleteDirectory(std::shared_ptr<ipfs::DagListener>& listener,
                       ipfs::Block const& block,
                       std::string_view path,
                       ipfs::UnixFsPathResolver& resolver) {
  auto has_index_html = false;
  ipfs::GeneratedDirectoryListing list{path};
  block.List([&resolver, &list, &has_index_html](auto& name, auto cid) {
    if (name == "index.html") {
      resolver.current_cid(cid);
      resolver.current_path("");
      has_index_html = true;
      auto orig = resolver.original_path();
      if (orig.empty() || orig.back() != '/') {
        orig.push_back('/');
      }
      orig.append("index.html");
      resolver.original_path(orig);
      return false;
    } else {
      list.AddEntry(name);
    }
    return true;
  });
  if (has_index_html) {
    resolver.Step(listener);
  } else {
    listener->ReceiveBlockBytes(list.Finish());
    listener->BlocksComplete("text/html");
    resolver.current_cid("");
  }
}
}  // namespace
