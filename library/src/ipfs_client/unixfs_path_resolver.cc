#include "ipfs_client/unixfs_path_resolver.h"

#include "generated_directory_listing.h"

#if __has_include("base/debug/stack_trace.h")
#include "base/debug/stack_trace.h"
#endif

#include "ipfs_client/block.h"
#include "ipfs_client/block_storage.h"
#include "ipfs_client/framework_api.h"
#include "smhasher/MurmurHash3.h"
#include "vocab/log_macros.h"
#include "vocab/stringify.h"

#include <endian.h>

#include <iomanip>

namespace {
bool starts_with(std::string_view a, std::string_view b) {
  if (a.size() < b.size()) {
    return false;
  }
  return a.substr(0, b.size()) == b;
}
bool ends_with(std::string_view a, std::string_view b) {
  if (a.size() < b.size()) {
    return false;
  }
  a.remove_prefix(a.size() - b.size());
  return a == b;
}
}  // namespace

void ipfs::UnixFsPathResolver::Step(std::shared_ptr<DagListener> listener) {
  LOG(INFO) << "Step(" << cid_ << ',' << path_ << ',' << original_path_ << ')';
  if (awaiting_.size() && storage_.Get(awaiting_)) {
    awaiting_.clear();
  }
  if (cid_.empty()) {
    LOG(ERROR) << "No CID?";
    return;
  }
  if (hamt_hexs_.size()) {
    LOG(INFO) << "Appear to be mid-Hamt traversal: " << hamt_hexs_.front();
  }
  Block const* block = storage_.Get(cid_);
  if (!block) {
    LOG(INFO) << "Current block " << cid_ << " not found.";
    Request(listener, cid_, Scheduler::Priority::Required);
    return;
  }
  LOG(INFO) << "Process block of type " << ipfs::Stringify(block->type());
  switch (block->type()) {
    case Block::Type::Directory:
      ProcessDirectory(listener, *block);
      break;
    case Block::Type::FileChunk:
      listener->ReceiveBlockBytes(block->chunk_data());
      listener->BlocksComplete(GuessContentType(block->chunk_data()));
      cid_.clear();
      break;
    case Block::Type::File:
      ProcessLargeFile(listener, *block);
      break;
    case Block::Type::HAMTShard:
      ProcessDirShard(listener, *block);
      break;
    default:
      LOG(FATAL) << "TODO : Unhandled UnixFS node of type "
                 << Stringify(block->type());
  }
}
void ipfs::UnixFsPathResolver::ProcessLargeFile(
    std::shared_ptr<DagListener>& listener,
    Block const& block) {
  bool writing = false;
  if (written_through_.empty()) {
    // First time here, probably
    // Request any missing blocks
    block.List([this, &listener](auto, auto child_cid) {
      if (storage_.Get(child_cid) == nullptr) {
        Request(listener, child_cid, Scheduler::Priority::Required);
      }
      return true;
    });
    writing = true;
  }
  block.List([&writing, listener, this](auto, auto child_cid) {
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
          if (head_.empty()) {
            head_ = child->unparsed();
          }
          listener->ReceiveBlockBytes(child->unparsed());
          break;
        case Block::Type::FileChunk:
          if (head_.empty()) {
            head_ = child->chunk_data();
          }
          listener->ReceiveBlockBytes(child->chunk_data());
          break;
        default:
          LOG(FATAL) << " unhandled child-of-file block type: "
                     << ipfs::Stringify(child->type());
      }
      written_through_.assign(child_cid);
      return true;
    }
    // This is the next block to be written, but we don't have it. Wait.
    return (writing = false);
  });
  if (writing) {
    listener->BlocksComplete(GuessContentType(head_));
  }
}
void ipfs::UnixFsPathResolver::CompleteDirectory(
    std::shared_ptr<DagListener>& listener,
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
    Step(listener);
  } else {
    listener->ReceiveBlockBytes(list.Finish());
    listener->BlocksComplete("text/html");
    cid_.clear();
  }
}

void ipfs::UnixFsPathResolver::ProcessDirectory(
    std::shared_ptr<DagListener>& listener,
    Block const& block) {
  block.List([this, &listener](auto&, auto cid) {
    Request(listener, cid, Scheduler::Priority::Optional);
    return true;
  });
  auto start = path_.find_first_not_of("/");
  if (start == std::string::npos) {
    CompleteDirectory(listener, block);
    return;
  }
  auto end = path_.find('/', start);
  std::string_view next_comp = path_;
  auto next = api_->UnescapeUrlComponent(next_comp.substr(start, end - start));
  auto found = false;
  block.List([&found, this, next](auto& name, auto cid) {
    if (name == next) {
      cid_ = cid;
      found = true;
      return false;
    }
    return true;
  });
  if (found) {
    LOG(INFO) << "Descending path: " << next << " . " << path_;
    path_.erase(0, end);
    Step(listener);
  } else {
    listener->FourOhFour(cid_, path_);
  }
}
void ipfs::UnixFsPathResolver::ProcessDirShard(
    std::shared_ptr<DagListener>& listener,
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
  if (path_.empty()) {
    // TODO - there could one day be a HAMT directory with no index.html
    path_ = "index.html";
  }
  /* stolen from spec
   * ####### Path resolution on HAMTs
   * Steps:
   * 1. Take the current path component...
   */
  auto non_slash = path_.find_first_not_of("/");
  if (non_slash && non_slash < path_.size()) {
    LOG(INFO) << "Removing " << non_slash << " leading slashes from " << path_;
    path_.erase(0, non_slash);
  }
  auto slash = path_.find('/');
  L_VAR(slash);
  auto next = path_.substr(0, slash);
  bool missing_descendents = false;
  //  RequestHamtDescendents(api, missing_descendents, block, hex_width);
  if (missing_descendents) {
    LOG(WARNING)
        << "Waiting on more blocks before dealing with this HAMT node.";
    return;
  }
  L_VAR(next)
  L_VAR(path_)
  if (next.empty()) {
    GeneratedDirectoryListing listing{original_path_};
    if (ListHamt(listener, block, listing, hex_width)) {
      LOG(INFO) << "Returning generated listing for a HAMT";
      listener->ReceiveBlockBytes(listing.Finish());
      listener->BlocksComplete("text/html");
      this->cid_.clear();
    }
    return;
  }
  auto component = api_->UnescapeUrlComponent(next);
  L_VAR(component);
  if (hamt_hexs_.empty()) {
    // ...  then hash it using the multihash id provided in Data.hashType.
    //  absl::uint128 digest{};
    std::array<std::uint64_t, 2> digest = {0U, 0U};
    // Rust's fastmurmur3 also passes 0 for seed, and iroh uses that.
    MurmurHash3_x64_128(component.data(), component.size(), 0, digest.data());
    auto bug_compatible_digest = htobe64(digest[0]);
    LOG(INFO) << "Hash: " << digest[0] << ' ' << digest[1] << " -> "
              << bug_compatible_digest;
    for (auto d : digest) {
      auto hash_bits = htobe64(d);
      while (hash_bits) {
        // 2. Pop the log2(fanout) lowest bits from the path component hash
        // digest,...
        auto popped = hash_bits % fanout;
        hash_bits /= fanout;
        LOG(INFO) << "popped=" << popped << " remaining digest=" << hash_bits;
        std::ostringstream oss;
        // ... then hex encode (using 0-F) using little endian thoses bits ...
        oss << std::setfill('0') << std::setw(hex_width) << std::uppercase
            << std::hex << popped;
        hamt_hexs_.push_back(oss.str());
      }
    }
    Step(listener);
  } else {
    bool found = false;
    block.List([&](auto& name, auto cid) {
      // Fun fact: there is a spec-defined sort order to these children.
      // We *could* do a binary search.
      if (!starts_with(name, hamt_hexs_.front())) {
        //        LOG(INFO) << name << " isn't the right child for " <<
        //        hamt_hexs_.front() ;
        return true;
      }
      found = true;
      LOG(INFO) << "As we move down a Hamt, " << cid_ << " -> " << name << " / "
                << cid << " fanout=" << block.fsdata().fanout();
      cid_ = cid;
      if (ends_with(name, component)) {
        LOG(INFO) << "Found our calling! Leaving the Hamt";
        hamt_hexs_.clear();
        auto slash = path_.find('/');
        if (slash == std::string::npos) {
          path_.clear();
        } else {
          path_.erase(0, slash + 1);
        }
      } else if (hamt_hexs_.front() == name) {
        LOG(INFO) << "One more level down the Hamt";
        hamt_hexs_.erase(hamt_hexs_.begin());
      } else {
        LOG(ERROR) << "Was looking for " << cid_ << '/' << path_
                   << " and got as far as HAMT hash bits " << hamt_hexs_.front()
                   << " but the corresponding link is " << name
                   << " and does not end with " << component;
        listener->FourOhFour(cid_, hamt_hexs_.front() + component);
      }
      return false;
    });
    if (found) {
      Step(listener);
    } else {
      listener->FourOhFour(cid_, path_);
    }
  }
}

void ipfs::UnixFsPathResolver::RequestHamtDescendents(
    std::shared_ptr<DagListener> listener,
    bool& missing_children,
    Block const& block,
    unsigned hex_width) const {
  block.List([this, hex_width, listener, &missing_children](
                 std::string const& name, auto cid) {
    GOOGLE_DCHECK_GE(name.size(), hex_width);
    auto block = storage_.Get(cid);
    if (block) {
      if (name.size() == hex_width) {
        this->RequestHamtDescendents(listener, missing_children, *block,
                                     hex_width);
      }
    } else {
      api_->RequestByCid(cid, listener, Scheduler::Priority::Optional);
    }
    return true;
  });
}
bool ipfs::UnixFsPathResolver::ListHamt(std::shared_ptr<DagListener>& listener,
                                        Block const& block,
                                        GeneratedDirectoryListing& out,
                                        unsigned hex_width) {
  LOG(INFO) << hex_width;
  bool got_all = true;
  block.List([&](std::string const& link_name, auto cid) {
    if (link_name.size() > hex_width) {
      auto entry_name = std::string_view{link_name}.substr(hex_width);
      if (entry_name == "index.html") {
        Request(listener, cid, Scheduler::Priority::Required);
        this->cid_ = cid;
        this->path_.clear();
        return got_all = false;
      } else {
        out.AddEntry(entry_name);
      }
    } else if (auto child = storage_.Get(cid)) {
      if (!this->ListHamt(listener, *child, out, hex_width)) {
        got_all = false;
      }
    } else {
      // TODO - can't do an independent listing (no index.html) of a Hamt
      // without this node
      //       Request(api, cid, Scheduler::Priority::Optional);
      got_all = false;
    }
    return true;
  });
  return got_all;
}

void ipfs::UnixFsPathResolver::Request(std::shared_ptr<DagListener>& listener,
                                       std::string const& cid,
                                       Scheduler::Priority prio) {
  if (storage_.Get(cid)) {
    return;
  }
  LOG(INFO) << "Request(" << cid << ',' << static_cast<long>(prio) << ')';
  if (prio == Scheduler::Priority::Required) {
    storage_.AddListening(this);
    if (cid_ != cid) {
      awaiting_ = cid;
    } else {
      awaiting_.clear();
    }
  }
  auto it = already_requested_.find(cid);
  auto t = std::time(nullptr);
  if (it == already_requested_.end()) {
    already_requested_[cid] = {prio, t};
    api_->RequestByCid(cid, listener, prio);
  } else if (prio == Scheduler::Priority::Required &&
             it->second.first == Scheduler::Priority::Optional) {
    it->second.first = Scheduler::Priority::Required;
    it->second.second = t;
    api_->RequestByCid(cid, listener, Scheduler::Priority::Required);
  } else if (t - it->second.second > 2) {
    if (it->second.first == Scheduler::Priority::Required) {
      it->second.first = Scheduler::Priority::Optional;
      it->second.second = t;
    } else {
      already_requested_.erase(it);
    }
  }
}

const std::string& ipfs::UnixFsPathResolver::waiting_on() const {
  if (awaiting_.empty()) {
    return cid_;
  } else {
    return awaiting_;
  }
}
std::string ipfs::UnixFsPathResolver::describe() const {
  auto rv = original_cid_;
  rv.append("//")
      .append(original_path_)
      .append(" ... ")
      .append(cid_)
      .append("//")
      .append(path_);
  return rv;
}

ipfs::UnixFsPathResolver::UnixFsPathResolver(BlockStorage& store,
                                             Scheduler& sched,
                                             std::string cid,
                                             std::string path,
                                             std::shared_ptr<FrameworkApi> api)
    : storage_{store},
      sched_(sched),
      cid_{cid},
      path_{path},
      original_cid_(cid),
      original_path_(path),
      api_(api) {
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
ipfs::UnixFsPathResolver::~UnixFsPathResolver() noexcept {
  /*
#ifdef BASE_DEBUG_STACK_TRACE_H_
  LOG(ERROR) << base::debug::StackTrace();
#else
  LOG(ERROR)
      << "Don't have stack trace available, but in UnixFsPathResolver dtor";
#endif
   */
  storage_.StopListening(this);
}

std::string ipfs::UnixFsPathResolver::GuessContentType(

    std::string_view content) {
  auto dot = original_path_.rfind('.');
  auto slash = original_path_.rfind('/');
  std::string ext;
  if (dot < original_path_.size() &&
      (slash < dot || slash == std::string::npos)) {
    ext = original_path_.substr(dot + 1);
  }
  auto mime = api_->MimeType(ext, content, "ipfs://" + original_path_);
  if (mime.size()) {
    // TODO, store mime in block
    LOG(INFO) << "Detected mime " << mime << " for " << original_path_
              << " based on the file contents (likely magic number).";
    return mime;
  }
  // TODO fetch the mime from block if available
  LOG(ERROR) << "\n\t###\tTODO:\tCould not determine mime type for '"
             << original_path_ << "'.\t###\n\n";
  return "TODO";
}
