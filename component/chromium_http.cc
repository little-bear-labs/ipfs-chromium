#include "chromium_http.h"

#include "block_http_request.h"

using Self = ipfs::ChromiumHttp;

Self::ChromiumHttp(network::mojom::URLLoaderFactory& lf)
    : loader_factory_{&lf} {}

void Self::SendHttpRequest(ipfs::ctx::HttpApi::HttpRequestDescription desc,
                           ipfs::ctx::HttpApi::HttpCompleteCallback cb) const {
  auto ptr = std::make_shared<BlockHttpRequest>(desc, cb);
  ptr->send(loader_factory_);
}