#include <ipfs_client/gw/dnslink_requestor.h>

#include "ipfs_client/ipld/dnslink_name.h"

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipld/dag_node.h>

#include <ipfs_client/client.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/partition.h>

#include "ipfs_client/gw/gateway_request_type.h"
#include "log_macros.h"

#include <absl/base/options.h>

using Self = ipfs::gw::DnsLinkRequestor;
using Source = ipfs::ipld::BlockSource;
using namespace std::literals;

Self::DnsLinkRequestor(std::shared_ptr<Client> api) {
  api_ = api;
}
std::string_view Self::name() const {
  return "DNSLink requestor";
}
namespace {
bool parse_results(ipfs::gw::RequestPtr req,
                   std::vector<std::string> const& results,
                   std::shared_ptr<ipfs::Client> const&,
                   Source::Clock::time_point);
}
auto Self::handle(ipfs::gw::RequestPtr req) -> HandleOutcome {
  if (req->type != GatewayRequestType::DnsLink) {
    return HandleOutcome::NOT_HANDLED;
  }
  // std::function requires target be copy-constructible
  auto success = std::make_shared<bool>();
  *success = false;
  auto a = api_;
  auto start = Source::Clock::now();
  auto res = [req, success, a, start](std::vector<std::string> const& results) {
    *success = *success || parse_results(req, results, a, start);
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
  api_->dns_txt().SendDnsTextRequest("_dnslink." + req->main_param, res,
                                     std::move(don));
  return HandleOutcome::PENDING;
}
namespace {
bool parse_results(ipfs::gw::RequestPtr req,
                   std::vector<std::string> const& results,
                   std::shared_ptr<ipfs::Client> const& api,
                   Source::Clock::time_point start) {
  constexpr auto prefix = "dnslink="sv;
  auto t = Source::Clock::now();
  for (auto& result : results) {
    if (starts_with(result, prefix)) {
      Source src;
      src.fetched_at = t;
      src.load_duration = t - start;
      req->RespondSuccessfully(result.substr(prefix.size()), api, src);
      return true;
    } else {
      LOG(INFO) << "Irrelevant TXT result, ignored: " << result;
    }
  }
  return false;
}
}  // namespace
