#include "chunk.h"

#include "log_macros.h"

using Chunk = ipfs::ipld::Chunk;

Chunk::Chunk(std::string data) : data_{data} {}
Chunk::~Chunk() {}

auto Chunk::resolve(ipfs::SlashDelimited path,
                    ipfs::ipld::DagNode::BlockLookup,
                    std::string&) -> ResolveResult {
  if (path) {
    LOG(ERROR) << "Can't resolve a path (" << path.to_string()
               << ") inside of a file chunk!";
    return ProvenAbsent{};
  }
  return Response{"", 200, data_, {}};
}

