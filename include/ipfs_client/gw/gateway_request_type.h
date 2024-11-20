#ifndef IPFS_CHROMIUM_GATEWAY_REQUEST_TYPE_H
#define IPFS_CHROMIUM_GATEWAY_REQUEST_TYPE_H

#include <optional>
#include <string_view>

namespace ipfs::gw {
enum class GatewayRequestType : char {
  Block,
  Car,
  Ipns,
  DnsLink,
  Providers,
  Identity,
  Zombie
};
std::string_view name(GatewayRequestType t);
std::optional<GatewayRequestType> from_name(std::string_view);
constexpr std::size_t RequestTypeCount =
    static_cast<std::size_t>(gw::GatewayRequestType::Zombie);
}  // namespace ipfs::gw

#endif  // IPFS_CHROMIUM_GATEWAY_REQUEST_TYPE_H
