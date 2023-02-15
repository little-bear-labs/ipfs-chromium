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
  Block const* block = storage_.Get(cid_);
  if (!block) {
    storage_.AddListening(shared_from_this());
    request_required_(cid_);
    return;
  }
  if (block->is_directory()) {
    auto start = path_.find_first_not_of("/");
    if (start == std::string::npos) {
      CompleteDirectory(*block);
      return;
    }
    auto end = path_.find('/', start);
    std::string_view next = path_;
    next = next.substr(start, end - start);
    block->List([this, next](auto& name, auto cid) {
      if (name == next) {
        cid_ = cid;
      } else if (!storage_.Get(cid)) {
        request_prefetch_(cid);
      }
      return true;
    });
    // TODO 404
    std::clog << "Descending path: " << next << " . " << path_ << '\n';
    path_.erase(0, end);
    Step(shared_from_this());
  } else if (block->type() == Block::Type::FileChunk) {
    receive_bytes_(block->chunk_data());
    on_complete_(GuessContentType(original_path_, block->chunk_data()));
  } else if (block->is_file()) {
    std::clog << "Re-assembling file from chunks: " << cid_ << '\n';
    bool writing = false;
    if (written_through_.empty()) {
      // First time here, probably
      // Request any missing blocks
      block->List([this](auto, auto child_cid) {
        if (storage_.Get(child_cid) == nullptr) {
          request_required_(child_cid);
        }
        return true;
      });
      writing = true;
    }
    block->List([&writing, this](auto, auto child_cid) {
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
  } else {
    TODO
  }
}

void ipfs::UnixFsPathResolver::CompleteDirectory(Block const& block) {
  auto has_index_html = false;
  // TODO implement ..
  std::string generated_index{
      "<html>\n"
      "  <title>Directory Listing</title>\n"
      "  <body>\n"
      "    <ul>\n"};
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
          .append("        <a href='ipfs://")  // TODO need to keep right dir
                                               // level in case you follow
                                               // relative links inside
          .append(cid)
          .append("'>")
          .append(name)
          .append("</a>\n")
          .append("      </li>");
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
      request_required_(request_required),
      request_prefetch_(request_prefetch),
      receive_bytes_(receive_bytes),
      on_complete_(on_complete) {}
ipfs::UnixFsPathResolver::~UnixFsPathResolver() noexcept {}
