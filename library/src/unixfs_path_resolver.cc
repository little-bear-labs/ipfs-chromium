#include "ipfs_client/unixfs_path_resolver.h"

#include "ipfs_client/block_storage.h"
#include "ipfs_client/content_type.h"

#include <iostream>

#define TODO                                                                \
  {                                                                         \
    std::clog << "TODO " << __PRETTY_FUNCTION__ << " @ " << __FILE__ << ':' \
              << __LINE__ << '\n';                                          \
  }

void ipfs::UnixFsPathResolver::Step(std::shared_ptr<UnixFsPathResolver>) {
  std::clog << "Step(" << cid_ << ',' << path_ << ',' << original_path_
            << ")\n";
  Block const* block = storage_.Get(cid_);
  if (!block) {
    storage_.AddListening(shared_from_this());
    Request(cid_, Scheduler::Priority::Required);
    return;
  }
  std::clog << "Process block of type(" << block->type() << ")\n";
  switch (block->type()) {
    case Block::Type::Directory:
      ProcessDirectory(*block);
      break;
    case Block::Type::FileChunk:
      receive_bytes_(block->chunk_data());
      on_complete_(GuessContentType(original_path_, block->chunk_data()));
      break;
    case Block::Type::File:
      ProcessLargeFile(*block);
      break;
    case Block::Type::HAMTShard:
      ProcessDirShard(*block);
      break;
    default:
      std::clog << "TODO : Unhandled UnixFS node of type " << block->type()
                << std::endl;
      std::exit(42);
  }
}
void ipfs::UnixFsPathResolver::ProcessLargeFile(Block const& block) {
  std::clog << "Re-assembling file from chunks: " << cid_ << '\n';
  bool writing = false;
  if (written_through_.empty()) {
    // First time here, probably
    // Request any missing blocks
    block.List([this](auto, auto child_cid) {
      if (storage_.Get(child_cid) == nullptr) {
        Request(child_cid, Scheduler::Priority::Required);
      }
      return true;
    });
    writing = true;
  }
  block.List([&writing, this](auto, auto child_cid) {
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
    std::clog << "Including file piece: " << child_cid << '\n';
    if (child) {
      switch (child->type()) {
        case Block::Type::NonFs:
          receive_bytes_(child->unparsed());
          break;
        case Block::Type::FileChunk:
          receive_bytes_(child->chunk_data());
          break;
        default:
          TODO
      }
      written_through_.assign(child_cid);
      return true;
    }
    // This is the next block to be written, but we don't have it. Wait.
    return (writing = false);
  });
  if (writing) {
    on_complete_(GuessContentType(original_path_, "TODO"));
  }
}
void ipfs::UnixFsPathResolver::CompleteDirectory(Block const& block) {
  auto has_index_html = false;
  // TODO implement ..
  std::string generated_index{
      "<html>\n"
      "  <title>" +
      original_path_ +
      " (directory listing)</title>\n"
      "  <body>\n"
      "    <ul>\n"};
  std::clog << "original: cid='" << original_cid_ << "' path='"
            << original_path_ << "'\n";
  if (original_path_.empty() || original_path_.at(0) != '/') {
    original_path_.insert(0, "/");
  }
  if (original_path_.back() != '/') {
    original_path_.push_back('/');
  }
  std::clog << "original: cid='" << original_cid_ << "' path='"
            << original_path_ << "'\n";
  if (original_path_.size() > 1UL) {
    std::string_view dotdotpath{original_path_};
    std::clog << ".. , " << dotdotpath;
    dotdotpath.remove_suffix(1);
    auto last_slash = dotdotpath.find_last_of("/");
    std::clog << " , " << dotdotpath << " up through " << last_slash;
    dotdotpath = dotdotpath.substr(0, dotdotpath.find_last_of("/") + 1UL);
    std::clog << " , " << dotdotpath << '\n';
    generated_index.append("      <li>")
        .append("        <a href='ipfs://")
        .append(original_cid_)
        .append(dotdotpath)
        .append("'>..</a>\n")
        .append("      </li>\n");
  }
  block.List([&](auto& name, auto cid) {
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
      generated_index.append("      <li>")
          .append("        <a href='ipfs://")
          .append(original_cid_)
          .append(original_path_)
          .append(name)  // TODO URL encode
          .append("'>")
          .append(name)
          .append("</a>\n")
          .append("      </li>\n");
    }
    return true;
  });
  if (has_index_html) {
    Step(shared_from_this());
  } else {
    generated_index.append(
        "    </ul>\n"
        "  </body>\n"
        "</html>");
    receive_bytes_(generated_index);
    on_complete_("text/html");
  }
}

void ipfs::UnixFsPathResolver::ProcessDirectory(Block const& block) {
  block.List([this](auto&, auto cid) {
    Request(cid, Scheduler::Priority::Optional);
    return true;
  });
  auto start = path_.find_first_not_of("/");
  if (start == std::string::npos) {
    CompleteDirectory(block);
    return;
  }
  auto end = path_.find('/', start);
  std::string_view next = path_;
  next = next.substr(start, end - start);
  block.List([this, next](auto& name, auto cid) {
    if (name == next) {
      cid_ = cid;
      return false;
    }
    return true;
  });
  // TODO 404
  std::clog << "Descending path: " << next << " . " << path_ << '\n';
  path_.erase(0, end);
  Step(shared_from_this());
}
void ipfs::UnixFsPathResolver::ProcessDirShard(Block const& block) {
  std::clog << "HAMTShard FANOUT = " << block.fsdata().fanout() << std::endl;
}
void ipfs::UnixFsPathResolver::Request(const std::string& cid,
                                       Scheduler::Priority prio) {
  if (storage_.Get(cid)) {
    return;
  }
  auto it = already_requested_.find(cid);
  if (it == already_requested_.end()) {
    already_requested_[cid] = prio;
    (prio == Scheduler::Priority::Required ? request_required_
                                           : request_prefetch_)(cid);
  } else if (prio == Scheduler::Priority::Required &&
             it->second == Scheduler::Priority::Optional) {
    it->second = prio;
    request_required_(cid);
  }
}

ipfs::UnixFsPathResolver::UnixFsPathResolver(BlockStorage& store,
                                             std::string cid,
                                             std::string path,
                                             RequestByCid request_required,
                                             RequestByCid request_prefetch,
                                             FileContentReceiver receive_bytes,
                                             CompetionHook on_complete)
    : storage_{store},
      cid_{cid},
      path_{path},
      original_cid_(cid),
      original_path_(path),
      request_required_(request_required),
      request_prefetch_(request_prefetch),
      receive_bytes_(receive_bytes),
      on_complete_(on_complete) {
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
