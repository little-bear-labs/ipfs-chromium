#include <ipfs_client/gw/dnslink_requestor.h>

#include "ipfs_client/ipld/ipns_name.h"

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipld/dag_node.h>

#include <ipfs_client/context_api.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/orchestrator.h>

#include "log_macros.h"

#define ABSL_USES_STD_STRING_VIEW 1  // Thanks, Windows!
#include <absl/strings/match.h>

using Self = ipfs::gw::DnsLinkRequestor;
using namespace std::literals;

Self::DnsLinkRequestor(std::shared_ptr<ContextApi> api) {
  api_ = api;
}
std::string_view Self::name() const {
  return "DNSLink requestor";
}
namespace {
bool parse_results(ipfs::gw::RequestPtr req,
                   std::vector<std::string> const& results);
}
auto Self::handle(ipfs::gw::RequestPtr req) -> HandleOutcome {
  if (req->type != Type::DnsLink) {
    return HandleOutcome::NOT_HANDLED;
  }
  // std::function requires target be copy-constructible
  auto success = std::make_shared<bool>();
  *success = false;
  auto res = [req, success](std::vector<std::string> const& results) {
    *success = *success || parse_results(req, results);
  };
  auto don = [success, req]() {
    LOG(INFO) << "DNSLink request completed for " << req->main_param
              << " success=" << *success;
    if (!*success) {
      req->dependent->finish(ipfs::Response::HOST_NOT_FOUND);
    }
  };
  api_->SendDnsTextRequest("_dnslink." + req->main_param, res, std::move(don));
  return HandleOutcome::PENDING;
}
namespace {
bool parse_results(ipfs::gw::RequestPtr req,
                   std::vector<std::string> const& results) {
  constexpr auto prefix = "dnslink="sv;
  LOG(INFO) << "Scanning " << results.size() << " DNS TXT records for "
            << req->main_param << " looking for dnslink...";
  for (auto& result : results) {
    if (absl::StartsWith(result, prefix)) {
      LOG(INFO) << "DNSLink result=" << result;
      auto target = result.substr(prefix.size());
      auto node = std::make_shared<ipfs::ipld::IpnsName>(target);
      req->orchestrator->add_node(req->main_param, node);
      req->orchestrator->build_response(req->dependent);
      return true;
    } else {
      LOG(INFO) << "Irrelevant TXT result, ignored: " << result;
    }
  }
  return false;
}
}  // namespace
