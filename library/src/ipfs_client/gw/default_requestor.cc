#include <ipfs_client/gw/default_requestor.h>

#include <ipfs_client/gw/block_request_splitter.h>
#include <ipfs_client/gw/dnslink_requestor.h>
#include <ipfs_client/gw/gateway_http_requestor.h>
#include <ipfs_client/gw/inline_request_handler.h>
#include <ipfs_client/gw/multi_gateway_requestor.h>
#include <ipfs_client/gw/terminating_requestor.h>

auto ipfs::gw::default_requestor(ipfs::GatewayList /* gws */,
                                 std::shared_ptr<Requestor> early,
                                 std::shared_ptr<ContextApi> api)
    -> std::shared_ptr<Requestor> {
  auto result = std::make_shared<gw::InlineRequestHandler>();
  result->or_else(std::make_shared<gw::BlockRequestSplitter>());
  if (early) {
    result->or_else(early);
    early->api(api);
  }
  result->or_else(std::make_shared<gw::DnsLinkRequestor>(api))
      .or_else(std::make_shared<MultiGatewayRequestor>())
      .or_else(std::make_shared<gw::TerminatingRequestor>());
  return result;
}
