#include "directory_shard.h"

#include "log_macros.h"

#include <ipfs_client/context_api.h>
#include <vocab/endian.h>

#include <smhasher/MurmurHash3.h>

#include <array>
#include <iomanip>
#include <sstream>

using namespace std::literals;

using Self = ipfs::ipld::DirShard;

auto Self::resolve(ResolutionState& parms) -> ResolveResult {
  if (parms.IsFinalComponent()) {
    auto index_parm = parms.WithPath("index.html"sv);
    auto result = resolve(index_parm);
    // TODO generate index.html if not present
    auto resp = std::get_if<Response>(&result);
    if (resp) {
      resp->mime_ = "text/html";
    }
    return result;
  }
  std::string name{parms.NextComponent(api_.get())};
  auto hash = hexhash(name);
  return resolve_internal(hash.begin(), hash.end(), name, parms);
}
auto Self::resolve_internal(ipfs::ipld::DirShard::HashIter hash_b,
                            ipfs::ipld::DirShard::HashIter hash_e,
                            std::string_view human_name,
                            ResolutionState& parms) -> ResolveResult {
  auto hash_chunk = hash_b == hash_e ? std::string{} : *hash_b;
  for (auto& [name, link] : links_) {
    if (!starts_with(name, hash_chunk)) {
      continue;
    }
    if (ends_with(name, human_name)) {
      return CallChild(parms, name);
    }
    auto node = parms.GetBlock(link.cid);
    if (!node) {
      // Unfortunately we can't really append more path and do a full Car
      // request
      //  The gateway would hash whatever we gave it and compare it to a
      //  partially-consumed hash
      return MoreDataNeeded{{"/ipfs/" + link.cid}};
    }
    auto downcast = node->as_hamt();
    if (downcast) {
      if (hash_b == hash_e) {
        LOG(ERROR) << "Ran out of hash bits.";
        return ProvenAbsent{};
      }
      return downcast->resolve_internal(std::next(hash_b), hash_e, human_name,
                                        parms);
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
