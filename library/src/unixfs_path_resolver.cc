#include "ipfs_client/unixfs_path_resolver.h"

#include "ipfs_client/block_storage.h"
#include "ipfs_client/framework_api.h"

#include <iostream>

void ipfs::UnixFsPathResolver::Step(std::shared_ptr<FrameworkApi> api) {
  std::clog << "Step(" << cid_ << ',' << path_ << ',' << original_path_
            << ")\n";
  Block const* block = storage_.Get(cid_);
  if (!block) {
    storage_.AddListening(this);
    Request(api, cid_, Scheduler::Priority::Required);
    return;
  }
  std::clog << "Process block of type(" << block->type() << ")\n";
  switch (block->type()) {
    case Block::Type::Directory:
      ProcessDirectory(api, *block);
      break;
    case Block::Type::FileChunk:
      api->ReceiveBlockBytes(block->chunk_data());
      api->BlocksComplete(GuessContentType(api, block->chunk_data()));
      break;
    case Block::Type::File:
      ProcessLargeFile(api, *block);
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
void ipfs::UnixFsPathResolver::ProcessLargeFile(
    std::shared_ptr<FrameworkApi>& api,
    Block const& block) {
  std::clog << "Re-assembling file from chunks: " << cid_ << '\n';
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
    std::clog << "Including file piece: " << child_cid << '\n';
    if (child) {
      switch (child->type()) {
        case Block::Type::NonFs:
          api->ReceiveBlockBytes(child->unparsed());
          break;
        case Block::Type::FileChunk:
          api->ReceiveBlockBytes(child->chunk_data());
          break;
        default:
          std::clog << "ERROR: unhandled child-of-file block type:"
                    << child->type() << '\n';
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
    Step(api);
  } else {
    generated_index.append(
        "    </ul>\n"
        "  </body>\n"
        "</html>");
    api->ReceiveBlockBytes(generated_index);
    api->BlocksComplete("text/html");
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
  Step(api);
}
void ipfs::UnixFsPathResolver::ProcessDirShard(Block const& block) {
  std::clog << "HAMTShard FANOUT = " << block.fsdata().fanout() << std::endl;
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
  std::clog << "Looking up Content-Type for " << original_path_ << '\n';
  auto dot = original_path_.rfind('.');
  auto slash = original_path_.rfind('/');
  if (dot < original_path_.size() &&
      (slash < dot || slash == std::string::npos)) {
    auto ext = original_path_.substr(dot + 1);
    auto mime = api->MimeTypeFromExtension(ext);
    if (mime.size()) {
      std::clog << "Assuming " << original_path_ << " is of type " << mime
                << " due to extension " << ext << '\n';
      return mime;  // TODO store the mime in the block
    }
  }
  // TODO sniff mime type from content
  auto mime = api->MimeTypeFromContent(content, "ipfs://" + original_path_);
  if (mime.size()) {
    std::clog << "Detected mime " << mime << " for " << original_path_
              << " based on the file contents (likely magic number).\n";
    return mime;
  }
  // TODO fetch the mime from block if available
  std::clog << "\n\t###\tTODO:\tCould not determine mime type for '"
            << original_path_ << "'.\t###\n\n";
  return "TODO";
}
