#include "chunk.h"

#include <ipfs_client/ipld/resolution_state.h>
#include <ipfs_client/ipld/dag_node.h>
#include <ipfs_client/response.h>

#include "log_macros.h"

#include <cstdint>
#include <utility>
#include <string>

constexpr std::uint16_t cSuccess = 200U;

using Chunk = ipfs::ipld::Chunk;

Chunk::Chunk(std::string data) : data_{std::move(data)} {}
Chunk::~Chunk() noexcept = default;

auto Chunk::resolve(ResolutionState& params) -> ResolveResult {
  if (params.IsFinalComponent()) {
    return Response{"", cSuccess, data_, params.MyPath().to_string(), {}};
  } else {
    VLOG(2) << "Can't resolve a path (" << params.MyPath()
            << ") inside of a file chunk!";
    return ProvenAbsent{};
  }
}

