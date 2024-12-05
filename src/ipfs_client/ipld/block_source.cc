#include <ipfs_client/ipld/block_source.h>

#include <ctime>
#include <sstream>
#include <string>
#include <string_view>
#include "ipfs_client/gw/gateway_request_type.h"

using Self = ipfs::ipld::BlockSource;

auto Self::Serialize() const -> std::string {
  std::ostringstream result;
  result << name(cat.request_type) << ' ' << cat.cached << ' ' << cid << ' '
         << Clock::to_time_t(fetched_at) << ' ' << cat.gateway_url;
  return result.str();
}
void Self::Deserialize(std::string_view from_string_view) {
  std::istringstream iss{std::string{from_string_view}};
  std::string s;
  std::time_t t;
  iss >> s >> cat.cached >> cid >> t;
  cat.request_type = gw::from_name(s).value_or(gw::GatewayRequestType::Zombie);
  fetched_at = Clock::from_time_t(t);
  iss >> cat.gateway_url;
}
