#include "providers_response.h"

#include <ipfs_client/client.h>
#include <ipfs_client/dag_json_value.h>

#include <vocab/slash_delimited.h>

#include "log_macros.h"

using namespace std::literals;

namespace prov = ipfs::providers;

namespace {
bool default_port(std::string_view scheme, std::string_view port) {
  if (scheme == "http") {
    return port == "80";
  } else if (scheme == "https") {
    return port == "443";
  } else {
    LOG(ERROR)
        << "Expected gateway's scheme to be either http or https, but it was '"
        << scheme << "'.";
    return false;
  }
}
std::string MultiaddrToGatewayPrefix(ipfs::SlashDelimited ma, bool http) {
  /* auto addr_proto = */ ma.pop();
  auto host = ma.pop();
  auto tcp = ma.pop();
  DCHECK_EQ(tcp, "tcp");
  auto port = ma.pop();
  auto app_proto = ma.pop();
  if (app_proto == "http" && !http) {
    LOG(INFO) << "Rejecting http:// gateway discovery due to config.";
    return "";
  }
  DCHECK_EQ(app_proto.substr(0, 4), "http");
  std::string rv{app_proto};
  rv.append("://").append(host);
  if (!default_port(app_proto, port)) {
    rv.append(":").append(port);
  }
  rv.append("/").append(ma.to_view());
  return rv;
}

bool ParseProvider(ipfs::DagJsonValue const& provider, ipfs::Client& api) {
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
  bool rv = false;
  auto handle_addr = [&api, &rv](ipfs::DagJsonValue const& addr) {
    if (auto s = addr.get_if_string()) {
      auto& c = api.gw_cfg();
      auto http = c.RoutingApiDiscoveryOfUnencryptedGateways();
      ipfs::SlashDelimited sd{s.value()};
      auto gw_pre = MultiaddrToGatewayPrefix(sd, http);
      if (gw_pre.size()) {
        c.AddGateway(gw_pre, c.RoutingApiDiscoveryDefaultRate());
        rv = true;
      }
    } else {
      LOG(ERROR) << ".Providers[x].Addrs[x] is not a string";
    }
  };
  if (!addrs->iterate_list(handle_addr)) {
    LOG(ERROR) << ".Providers[x].Addrs is not a list";
  }
  return rv;
}
}  // namespace

bool prov::ProcessResponse(std::string_view json_str, Client& api) {
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
    LOG(ERROR) << ".Providers was not a list.";
  }
  return rv;
}
