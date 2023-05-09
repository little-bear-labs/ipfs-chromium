#include "dir_shard.h"

#include <ipfs_client/block_storage.h>
#include <ipfs_client/unixfs_path_resolver.h>

#include <smhasher/MurmurHash3.h>

#include <vocab/endian.h>

#include "log_macros.h"

#include <absl/strings/match.h>

using Self = ipfs::unix_fs::DirShard;

bool Self::Process(std::unique_ptr<NodeHelper>& next_helper,
                   std::shared_ptr<DagListener> listener,
                   std::function<void(std::string, Priority)> requestor,
                   std::string& target_cid) {
  L_VAR((void*)this)
  L_VAR(next_path_element_)
  if (!block()) {
    requestor(cid_, resolver_->priority());
    return false;
  }
  auto& block = *(this->block());
  if (!(block.valid())) {
    LOG(ERROR) << "DirShard dealing with an invalid block: " << cid_ << " / "
               << resolver_->original_path();
    listener->FourOhFour(cid_, resolver_->original_path());
    return false;
  }
  SomePrefetch(requestor);

  // node.Data.hashType indicates a multihash function to use to digest path
  // components used for sharding.
  //  It MUST be murmur3-x64-64 (multihash 0x22).
  GOOGLE_DCHECK_EQ(block.fsdata().hashtype(), 0x22);
  if (next_path_element_.empty()) {
    // TODO - there could one day be a HAMT directory with no index.html
    next_path_element_ = "index.html";
    hamt_hexs_.clear();
  }
  /* stolen from spec
   * ####### Path resolution on HAMTs
   * Steps:
   * 1. Take the current path component...
   */
  bool missing_descendents = false;
  if (missing_descendents) {
    LOG(WARNING)
        << "Waiting on more blocks before dealing with this HAMT node.";
    return false;
  }
  L_VAR(next_path_element_)
  auto fanout = block.fsdata().has_fanout() ? block.fsdata().fanout() : 256;
  if (hamt_hexs_.empty()) {
    LOG(INFO) << "Had no hexes, hash path element: " << next_path_element_;
    HashPathElement(fanout);
  }
  if (hamt_hexs_.empty()) {
    LOG(ERROR) << "Somehow failed to hash out the path element?";
    return false;
  }
  //  for (auto& h : hamt_hexs_) {
  //    LOG(INFO) << "Hex: " << h;
  //  }
  bool found = false;
  block.List([&](auto& name, auto cid) {
    VLOG(1) << "Listing a child node of a HAMT shard node... " << name << '='
            << cid << " next hex=" << hamt_hexs_.front();
    // Fun fact: there is a spec-defined sort order to these children.
    // We *could* do a binary search.
    if (!absl::StartsWith(name, hamt_hexs_.front())) {
      //        LOG(INFO) << name << " isn't the right child for " <<
      //        hamt_hexs_.front() ;
      return true;
    }
    found = true;
    LOG(INFO) << "As we move down a Hamt, " << cid_ << " -> " << name << " / "
              << cid << " fanout=" << block.fsdata().fanout();
    target_cid = cid;
    if (absl::EndsWith(name, next_path_element_)) {
      LOG(INFO) << "Found our calling! Leaving the Hamt in favor of " << name;
      next_helper.reset();  // Let higher detect what the next level down is
    } else if (hamt_hexs_.front() == name) {
      LOG(INFO) << "One more level down the Hamt, following " << name;
      auto help = std::make_unique<DirShard>(next_path_element_);
      Delegate(*help);
      help->cid(cid);
      help->hamt_hexs_.assign(std::next(hamt_hexs_.begin()), hamt_hexs_.end());
      next_helper = std::move(help);
    } else {
      LOG(ERROR) << "Was looking for " << cid_ << '/'
                 << resolver_->original_path()
                 << " and got as far as HAMT hash bits " << hamt_hexs_.front()
                 << " but the corresponding link is " << name
                 << " and does not end with " << next_path_element_;
      found = false;
    }
    return false;
  });
  if (found) {
    return true;
  }
  listener->FourOhFour(cid_, resolver_->original_path());
  return false;
}

void Self::HashPathElement(std::uint64_t fanout) {
  //  L_VAR(fanout);
  GOOGLE_DCHECK_GT(fanout, 0);
  auto hex_width = 0U;
  for (auto x = fanout; (x >>= 4); ++hex_width)
    ;
  L_VAR(hex_width);
  // ...  then hash it using the multihash id provided in Data.hashType.
  //  absl::uint128 digest{};
  std::array<std::uint64_t, 2> digest = {0U, 0U};
  // Rust's fastmurmur3 also passes 0 for seed, and iroh uses that.
  MurmurHash3_x64_128(next_path_element_.data(), next_path_element_.size(), 0,
                      digest.data());
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
      // L_VAR(hash_bits);
      std::ostringstream oss;
      // ... then hex encode (using 0-F) using little endian thoses bits ...
      oss << std::setfill('0') << std::setw(hex_width) << std::uppercase
          << std::hex << popped;
      hamt_hexs_.push_back(oss.str());
    }
  }
}
void Self::SomePrefetch(std::function<void(std::string, Priority)> requestor) {
  auto i = 0;
  block()->List([this, requestor, &i](auto&, auto cid) {
    //    if (storage_->Get(cid, false)) {
    if (storage_->Get(cid)) {
      return true;
    }
    if (++i > skips_per_pass_) {
      ++skips_per_pass_;
      requestor(cid, 0);
      return false;
    }
    return true;
  });
}

Self::DirShard(std::string next_path_element)
    : next_path_element_{next_path_element} {}
Self::~DirShard() noexcept {}
