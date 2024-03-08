#include <ipfs_client/gw/gateway_request_type.h>

#include <cstdint>
#include <cstdio>

#include <array>

auto ipfs::gw::from_name(std::string_view name)
    -> std::optional<GatewayRequestType> {
  using T = GatewayRequestType;
  if (name == "Block") {
    return T::Block;
  }
  if (name == "Car") {
    return T::Car;
  }
  if (name == "Ipns") {
    return T::Ipns;
  }
  if (name == "DnsLink") {
    return T::DnsLink;
  }
  if (name == "Providers") {
    return T::Providers;
  }
  if (name == "Identity") {
    return T::Identity;
  }
  if (name == "CompletedRequest") {
    return T::Zombie;
  }
  return std::nullopt;
}
std::string_view ipfs::gw::name(GatewayRequestType t) {
  using ipfs::gw::GatewayRequestType;
  switch (t) {
    case GatewayRequestType::Block:
      return "Block";
    case GatewayRequestType::Car:
      return "Car";
    case GatewayRequestType::Ipns:
      return "Ipns";
    case GatewayRequestType::DnsLink:
      return "DnsLink";
    case GatewayRequestType::Providers:
      return "Providers";
    case GatewayRequestType::Identity:
      return "Identity";
    case GatewayRequestType::Zombie:
      return "CompletedRequest";
  }
  static std::array<char, 19> buf;
  std::sprintf(buf.data(), "InvalidType %d", static_cast<int>(t));
  return buf.data();
}
