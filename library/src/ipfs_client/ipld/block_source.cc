#include <ipfs_client/ipld/block_source.h>

#include <ctime>
#include <sstream>
#include <string>
#include <string_view>
#include "ipfs_client/gw/gateway_request_type.h"

using Self = ipfs::ipld::BlockSource;


/*
namespace {
#ifdef __cpp_lib_constexpr_string
constexpr auto a =
    Self::Category{"a", ipfs::gw::GatewayRequestType::Block, true};
constexpr auto b =
    Self::Category{"a", ipfs::gw::GatewayRequestType::Identity, true};
static_assert(a < b);
#endif
}  // namespace
*/

auto Self::Serialize() const -> std::string {
  std::ostringstream result;
  result << name(cat.request_type) << ' ' << cat.cached << ' ' << cid << ' '
         << Clock::to_time_t(fetched_at) << ' ' << cat.gateway_url;
  return result.str();
}
void Self::Deserialize(std::string_view sv) {
  std::istringstream iss{std::string{sv}};
  std::string s;
  std::time_t t;
  iss >> s >> cat.cached >> cid >> t;
  cat.request_type = gw::from_name(s).value_or(gw::GatewayRequestType::Zombie);
  fetched_at = Clock::from_time_t(t);
  iss >> cat.gateway_url;
}
