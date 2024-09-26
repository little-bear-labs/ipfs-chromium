#include <ipfs_client/gw/dnslink_requestor.h>

#include "ipfs_client/ipld/block_source.h"
#include "ipfs_client/gw/requestor.h"

#include <ipfs_client/ipld/dag_node.h>

#include <ipfs_client/client.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/partition.h>

#include "ipfs_client/gw/gateway_request_type.h"
#include "log_macros.h"

#include <memory>
#include <string_view>
#include <vector>
#include <string>
#include <utility>

using Self = ipfs::gw::DnsLinkRequestor;
using Source = ipfs::ipld::BlockSource;
using namespace std::literals;

Self::DnsLinkRequestor(std::shared_ptr<Client> api) {
  api_ = api;
}
auto Self::name() const -> std::string_view {
  return "DNSLink requestor";
}
namespace {
auto parse_results(ipfs::gw::RequestPtr req,
                   std::vector<std::string> const& results,
                   std::shared_ptr<ipfs::Client> const& /*api*/,
                   Source::Clock::time_point /*start*/) -> bool;
}
auto Self::handle(ipfs::gw::RequestPtr req) -> HandleOutcome {
  if (req->type != GatewayRequestType::DnsLink) {
    return HandleOutcome::NOT_HANDLED;
  }
  // std::function requires target be copy-constructible
  auto success = std::make_shared<bool>();
  *success = false;
  auto api = api_;//Copy for lambda bind
  auto start = Source::Clock::now();
  auto res = [req, success, api, start](std::vector<std::string> const& results) {
    *success = *success || parse_results(req, results, api, start);
  };
  auto don = [this,success, req]() {
    if (!*success) {
      if (api_->DnslinkFallback()) {
        forward(req);
      } else {
        req->dependent->finish(ipfs::Response::HOST_NOT_FOUND_RESPONSE);
      }
    }
  };
  std::string host{"_dnslink."};
  host.append(req->root_component());
  api_->dns_txt().SendDnsTextRequest(host, res, std::move(don));
  return HandleOutcome::PENDING;
}
namespace {
auto parse_results(ipfs::gw::RequestPtr req,
                   std::vector<std::string> const& results,
                   std::shared_ptr<ipfs::Client> const& api,
                   Source::Clock::time_point start) -> bool {
  constexpr auto prefix = "dnslink="sv;
  auto fetched_at = Source::Clock::now();
  for (const auto& result : results) {
    if (result.starts_with(prefix)) {
      Source src;
      src.fetched_at = fetched_at;
      src.load_duration = fetched_at - start;
      req->RespondSuccessfully(result.substr(prefix.size()), api, src);
      return true;
    } else {
      LOG(INFO) << "Irrelevant TXT result, ignored: " << result;
    }
  }
  return false;
}
}  // namespace
