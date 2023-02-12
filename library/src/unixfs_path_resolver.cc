#include "ipfs_client/unixfs_path_resolver.h"

#include "ipfs_client/block_storage.h"

#include <iostream>

#define TODO                                                                \
  {                                                                         \
    std::clog << "TODO " << __PRETTY_FUNCTION__ << " @ " << __FILE__ << ':' \
              << __LINE__ << '\n';                                          \
  }

void ipfs::UnixFsPathResolver::Step(std::shared_ptr<UnixFsPathResolver> p) {
  Block const* block = storage_.Get(cid_);
  if (!block) {
    storage_.AddListening(p);
    request_required_(cid_);
    return;
  }
  if (block->is_directory()) {
    auto start = path_.find_first_not_of("/");
    if (start == std::string::npos) {
      on_complete_();
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
    path_.erase(0, end);
    std::clog << "Descending path: " << next << " . " << path_ << '\n';
    Step(p);
  } else if (block->type() == Block::Type::FileChunk) {
    receive_bytes_(block->chunk_data());
    on_complete_();
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
      on_complete_();
    }
  } else {
    TODO
  }
}

ipfs::UnixFsPathResolver::UnixFsPathResolver(BlockStorage& store,
                                             std::string cid,
                                             std::string path,
                                             RequestByCid request_required,
                                             RequestByCid request_prefetch,
                                             FileContentReceiver receive_bytes,
                                             NullanaryHook on_complete)
    : storage_{store},
      cid_{cid},
      path_{path},
      request_required_(request_required),
      request_prefetch_(request_prefetch),
      receive_bytes_(receive_bytes),
      on_complete_(on_complete) {}
ipfs::UnixFsPathResolver::~UnixFsPathResolver() noexcept {}
