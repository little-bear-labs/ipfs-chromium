#include <ipfs_client/gw/block_request_splitter.h>

#include <ipfs_client/gw/gateway_request.h>

#include <algorithm>

using Self = ipfs::gw::BlockRequestSplitter;

std::string_view Self::name() const {
  return "BlockRequestSplitter";
}
auto Self::handle(ipfs::gw::RequestPtr r) -> HandleOutcome {
  if (r->type != Type::Car) {
    return HandleOutcome::NOT_HANDLED;
  }
  {
    auto br = std::make_shared<gw::GatewayRequest>(*r);
    br->type = Type::Block;
    br->path.clear();
    forward(br);
  }
  auto it = std::find(recent_provider_requests.begin(),
                      recent_provider_requests.end(), r->affinity);
  if (recent_provider_requests.end() == it) {
    auto i = old_provider_request % recent_provider_requests.size();
    recent_provider_requests[i] = r->affinity;
    ++old_provider_request;
    auto pr = std::make_shared<gw::GatewayRequest>(*r);
    pr->type = Type::Providers;
    pr->path.clear();
    pr->affinity.clear();
    forward(pr);
  }
  return HandleOutcome::NOT_HANDLED;
}