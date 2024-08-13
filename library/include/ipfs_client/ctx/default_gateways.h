#ifndef CHROMIUM_IPFS_GATEWAYS_H_
#define CHROMIUM_IPFS_GATEWAYS_H_

namespace ipfs::ctx {
class GatewayConfig;

bool LoadGatewaysFromEnvironmentVariable(GatewayConfig&);
void LoadStaticGatewayList(GatewayConfig&);

}  // namespace ipfs

#endif  // CHROMIUM_IPFS_GATEWAYS_H_
