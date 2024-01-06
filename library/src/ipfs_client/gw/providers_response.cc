#include "providers_response.h"

#include <ipfs_client/context_api.h>
#include <ipfs_client/dag_json_value.h>

#include <vocab/slash_delimited.h>

#include "log_macros.h"

using namespace std::literals;

namespace prov = ipfs::providers;

namespace {
std::string MultiaddrToGatewayPrefix(ipfs::SlashDelimited ma) {
  auto addr_proto = ma.pop();
  VLOG(2)
      << "Protocol expected to be one of ip4|ip6|dnsaddr|dns|dns4|dns6 , is:"
      << addr_proto;
  auto host = ma.pop();
  auto tcp = ma.pop();
  DCHECK_EQ(tcp, "tcp");
  auto port = ma.pop();
  auto app_proto = ma.pop();
  DCHECK_EQ(app_proto.substr(0, 4), "http");
  std::string rv{app_proto};
  rv.append("://").append(host).append(":").append(port).append("/").append(
      ma.to_view());
  return rv;
}

void ParseProvider(ipfs::DagJsonValue const& provider, ipfs::ContextApi& api) {
  auto proto = provider["Protocol"sv];
  if (!proto) {
    // Perhaps Scheme == peer. Not an error, but not used as of now.
    return;
  }
  if (auto proto_name = proto->get_if_string()) {
    if (proto_name.value() != "transport-ipfs-gateway-http") {
      // Perhaps transport-bitswap or transport-graphsync-filecoinv1.
      //   Not an error, but not used here yet.
      return;
    }
  } else {
    LOG(ERROR) << ".Providers[x].Protocol was not a string?";
    return;
  }
  auto addrs = provider["Addrs"];
  if (!addrs) {
    LOG(WARNING) << "Gateway with no addrs";
    return;
  }
  auto handle_addr = [&api](ipfs::DagJsonValue const& addr) {
    if (auto s = addr.get_if_string()) {
      auto gw_pre = MultiaddrToGatewayPrefix(ipfs::SlashDelimited{s.value()});
      LOG(INFO) << "'" << *s << "' -> '" << gw_pre << "'.";
      api.AddGateway(gw_pre);
    } else {
      LOG(ERROR) << ".Providers[x].Addrs[x] is not a string";
    }
  };
  if (!addrs->iterate_list(handle_addr)) {
    LOG(ERROR) << ".Providers[x].Addrs is not a list";
  }
}
}  // namespace

void prov::ProcessResponse(std::string_view json_str, ContextApi& api) {
  auto parsed = api.ParseJson(json_str);
  if (!parsed) {
    LOG(ERROR)
        << "Response to routing/v1 providers request did not parse as JSON: "
        << json_str;
    return;
  }
  auto list = (*parsed)["Providers"];
  if (!list) {
    LOG(ERROR) << "Response to routing/v1 providers request did not contain a "
                  "top-level key 'Providers': "
               << json_str;
    return;
  }
  if (!list->iterate_list([&api](auto& p) { ParseProvider(p, api); })) {
    LOG(ERROR) << ".Providers was not a list.";
  }
}
