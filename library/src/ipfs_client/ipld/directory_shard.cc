#include "directory_shard.h"

#include "log_macros.h"

#include <ipfs_client/context_api.h>
#include <vocab/endian.h>

#include <smhasher/MurmurHash3.h>

#define ABSL_USES_STD_STRING_VIEW 1
#include <absl/strings/match.h>

#include <array>
#include <iomanip>
#include <sstream>

using namespace std::literals;

using Self = ipfs::ipld::DirShard;

auto Self::resolve(ipfs::SlashDelimited relpath,
                   ipfs::ipld::DagNode::BlockLookup blu,
                   std::string& to_here) -> ResolveResult {
  LOG(INFO) << "DirShard::resolve(" << to_here << " / " << relpath.to_string()
            << ')';
  if (!relpath) {
    // TODO check if index.html is present and if not implement indexing
    auto result = resolve("index.html"sv, blu, to_here);
    auto resp = std::get_if<Response>(&result);
    if (resp) {
      resp->mime_ = "text/html";
    }
    return result;
  }
  auto name = api_->UnescapeUrlComponent(relpath.pop());
  auto hash = hexhash(name);
  return resolve_internal(hash.begin(), hash.end(), name, relpath, blu,
                          to_here);
}
auto Self::resolve_internal(ipfs::ipld::DirShard::HashIter hash_b,
                            ipfs::ipld::DirShard::HashIter hash_e,
                            std::string_view element_name,
                            ipfs::SlashDelimited path_after_dir,
                            ipfs::ipld::DagNode::BlockLookup blu,
                            std::string& path_to_dir) -> ResolveResult {
  auto hash_chunk = hash_b == hash_e ? std::string{} : *hash_b;
  VLOG(1) << "Scanning directory shard looking for " << hash_chunk << ','
          << element_name;
  for (auto& [name, link] : links_) {
    if (!absl::StartsWith(name, hash_chunk)) {
      continue;
    }
    if (!link.node) {
      link.node = blu(link.cid);
    }
    if (!link.node) {
      return MoreDataNeeded{std::vector{"/ipfs/" + link.cid}};
    }
    if (absl::EndsWith(name, element_name)) {
      LOG(INFO) << "Found " << element_name
                << ", leaving HAMT sharded directory " << name << "->"
                << link.cid;
      return link.node->resolve(path_after_dir, blu, path_to_dir);
    }
    auto downcast = link.node->as_hamt();
    if (downcast) {
      if (hash_b == hash_e) {
        LOG(ERROR) << "Ran out of hash bits.";
        return ProvenAbsent{};
      }
      LOG(INFO) << "Found hash chunk, continuing to next level of HAMT sharded "
                   "directory "
                << name << "->" << link.cid;
      return downcast->resolve_internal(std::next(hash_b), hash_e, element_name,
                                        path_after_dir, blu, path_to_dir);
    } else {
      return ProvenAbsent{};
    }
  }
  return ProvenAbsent{};
}
std::vector<std::string> Self::hexhash(std::string_view path_element) const {
  auto hex_width = 0U;
  for (auto x = fanout_; (x >>= 4); ++hex_width)
    ;
  std::array<std::uint64_t, 2> digest = {0U, 0U};
  MurmurHash3_x64_128(path_element.data(), path_element.size(), 0,
                      digest.data());
  auto corrected_digest = htobe64(digest[0]);
  VLOG(1) << "Hash: " << digest[0] << ' ' << digest[1] << " -> "
          << corrected_digest;
  std::vector<std::string> result;
  for (auto d : digest) {
    auto hash_bits = htobe64(d);
    while (hash_bits) {
      // 2. Pop the log2(fanout_) lowest bits from the path component hash
      // digest,...
      auto popped = hash_bits % fanout_;
      hash_bits /= fanout_;
      std::ostringstream oss;
      // ... then hex encode (using 0-F) using little endian those bits ...
      oss << std::setfill('0') << std::setw(hex_width) << std::uppercase
          << std::hex << popped;
      result.push_back(oss.str());
    }
  }
  return result;
}

Self::DirShard(std::uint64_t fanout) : fanout_{fanout} {}
Self::~DirShard() {}
Self* Self::as_hamt() {
  return this;
}
