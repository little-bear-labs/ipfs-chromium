#include <ipfs_client/gw/default_requestor.h>

#include <ipfs_client/gw/block_request_splitter.h>
#include <ipfs_client/gw/dnslink_requestor.h>
#include <ipfs_client/gw/inline_request_handler.h>
#include <ipfs_client/gw/multi_gateway_requestor.h>
#include <ipfs_client/gw/terminating_requestor.h>

auto ipfs::gw::default_requestor(std::shared_ptr<Requestor> early,
                                 std::shared_ptr<Client> api)
    -> std::shared_ptr<Requestor> {
  auto result = std::make_shared<gw::InlineRequestHandler>();
  result->api(api).or_else(std::make_shared<gw::BlockRequestSplitter>());
  if (early) {
    result->or_else(early);
  }
  result->or_else(std::make_shared<gw::DnsLinkRequestor>(api))
      .or_else(std::make_shared<MultiGatewayRequestor>())
      .or_else(std::make_shared<gw::TerminatingRequestor>());
  return result;
}
