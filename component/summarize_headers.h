#ifndef IPFS_SUMMARIZE_HEADERS_H_
#define IPFS_SUMMARIZE_HEADERS_H_

#include <string>
#include <vector>

namespace net {
class HttpResponseHeaders;
}

namespace ipfs {
class BlockStorage;

void summarize_headers(std::vector<std::string> const& cids,
                       std::string const& root,
                       net::HttpResponseHeaders& out,
                       BlockStorage& storage);
}  // namespace ipfs

#endif  // IPFS_SUMMARIZE_HEADERS_H_
