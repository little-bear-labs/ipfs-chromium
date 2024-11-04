#include "providers_response.h"

#include <ipfs_client/client.h>
#include <ipfs_client/dag_json_value.h>

#include <vocab/slash_delimited.h>
#include <string_view>
#include <string>

#include "log_macros.h"

using namespace std::literals;

namespace prov = ipfs::providers;

namespace {
class Scheme {
  char last_ = '!';
 public:
  Scheme(std::string_view const name)
  {
    if (name.starts_with("http")) {
      last_ = name.back();
    }
  }
  [[nodiscard]] auto is_default_port(std::string_view port) const -> bool {
    switch (last_) {
      case 'p': return port == "80";
      case 's': return port == "443";
      default: return false;
    }
  }
};

auto MultiaddrToGatewayPrefix(ipfs::SlashDelimited maddr, bool http) -> std::string {
  /* auto addr_proto = */ maddr.pop();
  auto host = maddr.pop();
  auto tcp = maddr.pop();
  DCHECK_EQ(tcp, "tcp");
  auto port = maddr.pop();
  auto app_proto = maddr.pop();
  if (app_proto == "http" && !http) {
    LOG(INFO) << "Rejecting http:// gateway discovery due to config.";
    return "";
  }
  DCHECK_EQ(app_proto.substr(0, 4), "http");
  std::string prefix{app_proto};
  prefix.append("://").append(host);
  Scheme const schm{app_proto};
  if (!schm.is_default_port(port)) {
    prefix.append(":").append(port);
  }
  prefix.append("/").append(maddr.to_view());
  return prefix;
}

auto ParseProvider(ipfs::DagJsonValue const& provider, ipfs::Client& api) -> bool {
  auto proto = provider["Protocol"sv];
  if (!proto) {
    // Perhaps Schema == peer. Not an error, but not used as of now.
    return false;
  }
  if (auto proto_name = proto->get_if_string()) {
    if (proto_name.value() != "transport-ipfs-gateway-http") {
      // Perhaps transport-bitswap or transport-graphsync-filecoinv1.
      //   Not an error, but not used here yet.
      return false;
    }
  } else {
    LOG(ERROR) << ".Providers[x].Protocol was not a string?";
    return false;
  }
  auto addrs = provider["Addrs"];
  if (!addrs) {
    LOG(WARNING) << "Gateway with no addrs";
    return false;
  }
  bool res = false;
  auto handle_addr = [&api, &res](ipfs::DagJsonValue const& addr) {
    if (auto s = addr.get_if_string()) {
      auto& c = api.gw_cfg();
      auto http = c.RoutingApiDiscoveryOfUnencryptedGateways();
      ipfs::SlashDelimited const sd{s.value()};
      auto gw_pre = MultiaddrToGatewayPrefix(sd, http);
      if (static_cast<unsigned int>(!gw_pre.empty()) != 0U) {
        c.AddGateway(gw_pre, c.RoutingApiDiscoveryDefaultRate());
        res = true;
      }
    } else {
      LOG(ERROR) << ".Providers[x].Addrs[x] is not a string";
    }
  };
  if (addrs->iterate_list(handle_addr)) {
    LOG(INFO) << "Handled provider address(es) in list.";
  } else {
    LOG(WARNING) << ".Providers[x].Addrs is not a list";
    handle_addr(*addrs);
  }
  return res;
}
}  // namespace

auto prov::ProcessResponse(std::string_view json_str, Client& api) -> bool {
  if (json_str.empty()) {
    LOG(ERROR) << "Empty body in response to routing/v1 providers request.";
    return false;
  }
  auto parsed = api.json().Parse(json_str);
  if (!parsed) {
    LOG(ERROR)
        << "Response to routing/v1 providers request did not parse as JSON: "
        << json_str;
    return false;
  }
  auto list = (*parsed)["Providers"];
  if (!list) {
    LOG(ERROR) << "Response to routing/v1 providers request did not contain a "
                  "top-level key 'Providers': "
               << json_str;
    return false;
  }
  bool rv = false;
  auto parse_one = [&rv, &api](auto& p) { rv = ParseProvider(p, api) || rv; };
  if (!list->iterate_list(parse_one)) {
    VLOG(2) << ".Providers was not a list.";
  }
  return rv;
}
