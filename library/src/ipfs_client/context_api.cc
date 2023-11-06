#include <ipfs_client/context_api.h>

#include <ipfs_client/gateway.h>

ipfs::GatewayRequest::GatewayRequest(BusyGateway&& bg)
    : gateway(std::move(bg)) {}

ipfs::GatewayRequest::~GatewayRequest() noexcept {}

std::string ipfs::GatewayRequest::url() const {
  return gateway.url();
}
std::string ipfs::GatewayRequest::task() const {
  return gateway.task();
}
bool ipfs::HttpRequestDescription::operator==(
    HttpRequestDescription const& r) const {
  return url == r.url && accept == r.accept;
}
