#include <ipfs_client/ipld/block_source.h>

using Self = ipfs::ipld::BlockSource;

namespace c = std::chrono;

Self::BlockSource(std::string url,
                  Clock::duration load_took,
                  Clock::time_point fetched_when)
    : gateway_url_{url}, fetched_at_{fetched_when}, load_duration_{load_took} {}

Self::BlockSource(std::string url, Clock::duration load_took)
    : BlockSource(url, load_took, Clock::now()) {}

void Self::ServerTiming(std::string& value, bool& is_first_block) {
  auto ms = c::duration_cast<c::milliseconds>(load_duration_).count();
  value = "TODO " + std::to_string(ms);
  is_first_block = false;
}
