#include "chunk.h"

#include "log_macros.h"

using Chunk = ipfs::ipld::Chunk;

Chunk::Chunk(std::string data) : data_{data} {}
Chunk::~Chunk() {}

auto Chunk::resolve(ResolutionState& params) -> ResolveResult {
  if (params.IsFinalComponent()) {
    return Response{"", 200, data_, params.MyPath().to_string(), {}};
  } else {
    LOG(ERROR) << "Can't resolve a path (" << params.MyPath()
               << ") inside of a file chunk!";
    return ProvenAbsent{};
  }
}

