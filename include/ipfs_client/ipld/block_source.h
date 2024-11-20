#ifndef IPFS_CHROMIUM_BLOCK_SOURCE_H
#define IPFS_CHROMIUM_BLOCK_SOURCE_H

#include <ipfs_client/gw/gateway_request_type.h>

#include <chrono>
#include <string>

namespace ipfs::ipld {
/*! Description of where a particular block was fetched from, for diagnostics
 */
struct BlockSource {
  /*! Source of now(). Defines timestamp type.
   */
  using Clock = std::chrono::system_clock;

  /*! Categorization of sources
   */
  struct Category {
    std::string gateway_url;
    gw::GatewayRequestType request_type = gw::GatewayRequestType::Zombie;
    bool cached = false;
    // Oddly github version of apple clang doesn't have string::<=>
    //    auto operator<=>(Category const&) const = default;
    bool operator<(Category const& r) const {
      if (cached != r.cached) {
        return cached;
      }
      if (request_type != r.request_type) {
        return request_type < r.request_type;
      }
      return gateway_url < r.gateway_url;
    }
  };
  std::string cid;  // or similar
  Clock::time_point fetched_at = Clock::now();
  Clock::duration load_duration = std::chrono::seconds(0);

  Category cat;

  void Deserialize(std::string_view);
  std::string Serialize() const;
};
}  // namespace ipfs::ipld

#endif  // IPFS_CHROMIUM_BLOCK_SOURCE_H
