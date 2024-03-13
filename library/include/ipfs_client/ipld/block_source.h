#ifndef IPFS_CHROMIUM_BLOCK_SOURCE_H
#define IPFS_CHROMIUM_BLOCK_SOURCE_H

#include <chrono>
#include <string>

namespace ipfs::ipld {
class BlockSource {
 public:
  using Clock = std::chrono::system_clock;

  BlockSource(std::string url, Clock::duration load_took);
  BlockSource(std::string url,
              Clock::duration load_took,
              Clock::time_point fetched_when);
  BlockSource() = default;

  explicit operator bool() const { return !gateway_url_.size(); }
  void ServerTiming(std::string& value, bool& is_first_block);

 private:
  std::string gateway_url_;
  Clock::time_point fetched_at_;
  Clock::duration load_duration_;
};
}  // namespace ipfs::ipld

#endif  // IPFS_CHROMIUM_BLOCK_SOURCE_H
