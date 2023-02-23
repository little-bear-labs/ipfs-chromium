#include "ipfs_client/unixfs_path_resolver.h"

#include "generated_directory_listing.h"

#include "ipfs_client/block_storage.h"
#include "ipfs_client/framework_api.h"
#include "ipfs_client/vocab/log_macros.h"
#include "smhasher/MurmurHash3.h"

#include "absl/numeric/int128.h"

#include <endian.h>

void ipfs::UnixFsPathResolver::Step(std::shared_ptr<FrameworkApi> api) {
  L_INF("Step(" << cid_ << ',' << path_ << ',' << original_path_ << ')');
  Block const* block = storage_.Get(cid_);
  if (!block) {
    storage_.AddListening(this);
    Request(api, cid_, Scheduler::Priority::Required);
    return;
  }
  L_INF("Process block of type " << block->type());
  switch (block->type()) {
    case Block::Type::Directory:
      ProcessDirectory(api, *block);
      break;
    case Block::Type::FileChunk:
      api->ReceiveBlockBytes(block->chunk_data());
      api->BlocksComplete(GuessContentType(api, block->chunk_data()));
      cid_.clear();
      break;
    case Block::Type::File:
      ProcessLargeFile(api, *block);
      break;
    case Block::Type::HAMTShard:
      ProcessDirShard(api, *block);
      break;
    default:
      L_DIE("TODO : Unhandled UnixFS node of type " << block->type());
  }
}
void ipfs::UnixFsPathResolver::ProcessLargeFile(
    std::shared_ptr<FrameworkApi>& api,
    Block const& block) {
  bool writing = false;
  if (written_through_.empty()) {
    // First time here, probably
    // Request any missing blocks
    block.List([this, &api](auto, auto child_cid) {
      if (storage_.Get(child_cid) == nullptr) {
        Request(api, child_cid, Scheduler::Priority::Required);
      }
      return true;
    });
    writing = true;
  }
  block.List([&writing, this, &api](auto, auto child_cid) {
    if (child_cid == written_through_) {
      // Found written_through_
      writing = true;
      return true;
    }
    if (!writing) {
      // Scanning, looking for written_through_
      return true;
    }
    auto child = storage_.Get(child_cid);
    if (child) {
      switch (child->type()) {
        case Block::Type::NonFs:
          api->ReceiveBlockBytes(child->unparsed());
          break;
        case Block::Type::FileChunk:
          api->ReceiveBlockBytes(child->chunk_data());
          break;
        default:
          L_DIE(" unhandled child-of-file block type: " << child->type());
      }
      written_through_.assign(child_cid);
      return true;
    }
    // This is the next block to be written, but we don't have it. Wait.
    return (writing = false);
  });
  if (writing) {
    api->BlocksComplete(GuessContentType(api, "TODO"));
  }
}
void ipfs::UnixFsPathResolver::CompleteDirectory(
    std::shared_ptr<FrameworkApi>& api,
    Block const& block) {
  auto has_index_html = false;
  GeneratedDirectoryListing list{original_path_};
  block.List([this, &list, &has_index_html](auto& name, auto cid) {
    if (name == "index.html") {
      cid_ = cid;
      path_.clear();
      has_index_html = true;
      force_type_dir_ = true;
      if (original_path_.empty() || original_path_.back() != '/') {
        original_path_.push_back('/');
      }
      original_path_.append("index.html");
      return false;
    } else {
      list.AddEntry(name);
    }
    return true;
  });
  if (has_index_html) {
    Step(api);
  } else {
    api->ReceiveBlockBytes(list.Finish());
    api->BlocksComplete("text/html");
    cid_.clear();
  }
}

void ipfs::UnixFsPathResolver::ProcessDirectory(
    std::shared_ptr<FrameworkApi>& api,
    Block const& block) {
  block.List([this, &api](auto&, auto cid) {
    Request(api, cid, Scheduler::Priority::Optional);
    return true;
  });
  auto start = path_.find_first_not_of("/");
  if (start == std::string::npos) {
    CompleteDirectory(api, block);
    return;
  }
  auto end = path_.find('/', start);
  std::string_view next_comp = path_;
  auto next = api->UnescapeUrlComponent(next_comp.substr(start, end - start));
  block.List([this, next](auto& name, auto cid) {
    if (name == next) {
      cid_ = cid;
      return false;
    }
    return true;
  });
  // TODO 404
  L_INF("Descending path: " << next << " . " << path_);
  path_.erase(0, end);
  Step(api);
}
void ipfs::UnixFsPathResolver::ProcessDirShard(
    std::shared_ptr<FrameworkApi>& api,
    Block const& block) {
  // node.Data.hashType indicates a multihash function to use to digest path
  // components used for sharding.
  //  It MUST be murmur3-x64-64 (multihash 0x22).
  GOOGLE_DCHECK_EQ(block.fsdata().hashtype(), 0x22);
  auto fanout = block.fsdata().has_fanout() ? block.fsdata().fanout() : 256;
  L_VAR(fanout);
  GOOGLE_DCHECK_GT(fanout, 0);

  auto hex_width = 0U;
  for (auto x = fanout; (x >>= 4); ++hex_width)
    ;
  L_VAR(hex_width);

  /* stolen from spec
   * ####### Path resolution on HAMTs
   * Steps:
   * 1. Take the current path component...
   */
  auto non_slash = path_.find_first_not_of("/");
  auto next = non_slash == std::string::npos
                  ? std::string_view{}
                  : std::string_view{path_}.substr(0, non_slash);
  bool missing_descendents = false;
  RequestHamtDescendents(api, missing_descendents, next, block, hex_width,
                         false);
  if (missing_descendents) {
    L_WRN("Waiting on more blocks before dealing with this HAMT node.");
    return;
  }
  if (next.empty()) {
    GeneratedDirectoryListing listing{original_path_};
    if (ListHamt(api, block, listing, hex_width)) {
      //      L_INF("Will return an index.html from a HAMT upon the next
      //      Step()");
      //    } else {
      L_INF("Returning generated listing for a HAMT");
      api->ReceiveBlockBytes(listing.Finish());
      api->BlocksComplete("text/html");
      this->cid_.clear();
    }
    return;
  } else if (non_slash) {
    path_.erase(0, non_slash);
  }
  auto slash = path_.find('/');
  auto current_path_component = std::string_view{path_}.substr(0, slash);
  L_INF("current_path_component=" << current_path_component);

  // ...  then hash it using the multihash id provided in Data.hashType.
  //  absl::uint128 digest{};
  std::array<std::uint64_t, 2> digest = {0U, 0U};
  // Rust's fastmurmur3 also passes 0 for seed, and iroh uses that.
  MurmurHash3_x64_128(current_path_component.data(),
                      current_path_component.size(), 0, digest.data()
                      //                      &digest
  );
  auto bug_compatible_digest = htobe64(digest[0]);
  L_INF("Hash: " << std::hex << digest[0] << ' ' << digest[1] << " -> "
                 << bug_compatible_digest);

  /* node.Data.fanout MUST be a power of two. This encode the number of hash
   *   permutations that will be used on each resolution step. The log base 2 of
   *   the fanout indicate how wide the bitmask will be on the hash at for that
   *   step. fanout MUST be between 8 and probably 65536.
   */
  // 2. Pop the log2(fanout) lowest bits from the path component hash digest,...
  auto popped = bug_compatible_digest % fanout;
  bug_compatible_digest /= fanout;
  L_INF("popped=" << std::hex << popped
                  << " remaining digest=" << bug_compatible_digest);

  /* ... then hex encode (using 0-F) thoses bits using little endian thoses bits
and find the link that starts with this hex encoded path.
 3. If the link name is exactly as long as the hex encoded representation,
follow the link and repeat step 2 with the child node and the remaining bit
stack. The child node MUST be a hamt directory else the directory is invalid,
else continue.
 4. Compare the remaining part of the last name you found, if it match the
original name you were trying to resolve you successfully resolved a path
component, everything past the hex encoded prefix is the name of that element
(usefull when listing childs of this directory).
  */
}
void ipfs::UnixFsPathResolver::RequestHamtDescendents(
    std::shared_ptr<FrameworkApi>& api,
    bool& missing_children,
    std::string_view next,
    Block const& block,
    unsigned hex_width,
    bool all_optional) const {
  block.List([this, all_optional, next, hex_width, &missing_children, &api](
                 std::string const& name, auto cid) {
    GOOGLE_DCHECK_GE(name.size(), hex_width);
    auto block = storage_.Get(cid);
    if (block) {
      if (name.size() == hex_width) {
        this->RequestHamtDescendents(api, missing_children, next, *block,
                                     hex_width, true);
      }
    } else {
      auto name_matches =
          0 == name.compare(hex_width, name.size() - hex_width, next);
      if (all_optional) {
        api->RequestByCid(cid, Scheduler::Priority::Optional);
      } else if (name_matches || (next.empty() && name == "index.html")) {
        api->RequestByCid(cid, Scheduler::Priority::Required);
        missing_children = true;
      } else {
        api->RequestByCid(cid, Scheduler::Priority::Optional);
      }
    }
    return true;
  });
}
bool ipfs::UnixFsPathResolver::ListHamt(std::shared_ptr<FrameworkApi>& api,
                                        Block const& block,
                                        GeneratedDirectoryListing& out,
                                        unsigned hex_width) {
  bool doing_index_html = false;
  block.List([&](std::string const& link_name, auto cid) {
    if (link_name.size() > hex_width) {
      auto entry_name = std::string_view{link_name}.substr(hex_width);
      if (entry_name == "index.html") {
        this->cid_ = cid;
        this->path_.clear();
        doing_index_html = true;
        return false;
      } else {
        out.AddEntry(entry_name);
      }
    } else {
      if (this->ListHamt(api, *(storage_.Get(cid)), out, hex_width)) {
        doing_index_html = true;
        return false;
      }
    }
    return true;
  });
  return doing_index_html;
}
void ipfs::UnixFsPathResolver::Request(std::shared_ptr<FrameworkApi>& api,
                                       std::string const& cid,
                                       Scheduler::Priority prio) {
  if (storage_.Get(cid)) {
    return;
  }
  auto it = already_requested_.find(cid);
  if (it == already_requested_.end()) {
    already_requested_[cid] = prio;
    api->RequestByCid(cid, prio);
  } else if (prio == Scheduler::Priority::Required &&
             it->second == Scheduler::Priority::Optional) {
    it->second = Scheduler::Priority::Required;
    api->RequestByCid(cid, Scheduler::Priority::Required);
  }
}

ipfs::UnixFsPathResolver::UnixFsPathResolver(BlockStorage& store,
                                             std::string cid,
                                             std::string path)
    : storage_{store},
      cid_{cid},
      path_{path},
      original_cid_(cid),
      original_path_(path) {
  constexpr std::string_view filename_param{"filename="};
  auto fn = original_path_.find(filename_param);
  if (fn != std::string::npos) {
    original_path_.erase(0, fn + filename_param.size());
    auto amp = original_path_.find('&');
    if (amp != std::string::npos) {
      original_path_.erase(amp);
    }
  }
}
ipfs::UnixFsPathResolver::~UnixFsPathResolver() noexcept {}

std::string ipfs::UnixFsPathResolver::GuessContentType(
    std::shared_ptr<FrameworkApi>& api,
    std::string_view content) {
  auto dot = original_path_.rfind('.');
  auto slash = original_path_.rfind('/');
  std::string ext;
  if (dot < original_path_.size() &&
      (slash < dot || slash == std::string::npos)) {
    ext = original_path_.substr(dot + 1);
  }
  auto mime = api->MimeType(ext, content, "ipfs://" + original_path_);
  if (mime.size()) {
    // TODO, store mime in block
    L_INF("Detected mime "
          << mime << " for " << original_path_
          << " based on the file contents (likely magic number).");
    return mime;
  }
  // TODO fetch the mime from block if available
  L_ERR("\n\t###\tTODO:\tCould not determine mime type for '" << original_path_
                                                              << "'.\t###\n\n");
  return "TODO";
}
