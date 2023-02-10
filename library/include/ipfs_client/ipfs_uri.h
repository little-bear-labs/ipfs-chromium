#ifndef IPFS_IPFS_URI_H_
#define IPFS_IPFS_URI_H_

#include <string>
#include <string_view>

namespace ipfs {

constexpr std::string_view IPFS_OVER_HTTP_DOMAIN{"ipfs-over-http.localhost"};
std::string IpfsUri2IpfsOverHttpUrl(std::string_view ipfs_or_ipns_uri);
std::string IpfsOverHttpUrl2IpfsGatewayPath(std::string_view url);

}  // namespace ipfs

#endif  // IPFS_IPFS_URI_H_
