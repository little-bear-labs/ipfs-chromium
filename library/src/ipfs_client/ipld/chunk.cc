#include "chunk.h"

using Chunk = ipfs::ipld::Chunk;

Chunk::Chunk(std::string data) : data_{data} {}
Chunk::~Chunk() {}

auto Chunk::resolve(ipfs::SlashDelimited path,
                    ipfs::ipld::DagNode::BlockLookup,
                    std::string&) -> ResolveResult {
  if (path) {
    return ProvenAbsent{};
  }
  return Response{"", 200, data_, {}};
}

