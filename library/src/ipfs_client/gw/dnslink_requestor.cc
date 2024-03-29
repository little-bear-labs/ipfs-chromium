#include <ipfs_client/gw/dnslink_requestor.h>

#include "ipfs_client/ipld/dnslink_name.h"

#include <ipfs_client/gw/gateway_request.h>
#include <ipfs_client/ipld/dag_node.h>

#include <ipfs_client/client.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/partition.h>

#include "log_macros.h"

#include <absl/base/options.h>

using Self = ipfs::gw::DnsLinkRequestor;
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
                   std::shared_ptr<ipfs::Client> const&);
}
auto Self::handle(ipfs::gw::RequestPtr req) -> HandleOutcome {
  if (req->type != Type::DnsLink) {
    return HandleOutcome::NOT_HANDLED;
  }
  // std::function requires target be copy-constructible
  auto success = std::make_shared<bool>();
  *success = false;
  auto a = api_;
  auto res = [req, success, a](std::vector<std::string> const& results) {
    *success = *success || parse_results(req, results, a);
  };
  auto don = [success, req]() {
    if (!*success) {
      req->dependent->finish(ipfs::Response::HOST_NOT_FOUND_RESPONSE);
    }
  };
  api_->dns_txt().SendDnsTextRequest("_dnslink." + req->main_param, res,
                                     std::move(don));
  return HandleOutcome::PENDING;
}
namespace {
bool parse_results(ipfs::gw::RequestPtr req,
                   std::vector<std::string> const& results,
                   std::shared_ptr<ipfs::Client> const& api) {
  constexpr auto prefix = "dnslink="sv;
  VLOG(1) << "Scanning " << results.size() << " DNS TXT records for "
          << req->main_param << " looking for dnslink...";
  for (auto& result : results) {
    if (starts_with(result, prefix)) {
      VLOG(1) << "DNSLink result=" << result;
      req->RespondSuccessfully(result.substr(prefix.size()), api);
      return true;
    } else {
      LOG(INFO) << "Irrelevant TXT result, ignored: " << result;
    }
  }
  return false;
}
}  // namespace
