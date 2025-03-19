#include "chromium_http.h"

#include "block_http_request.h"

#include <base/logging.h>

using Self = ipfs::ChromiumHttp;

Self::ChromiumHttp(network::mojom::URLLoaderFactory& delegate)
    : loader_factory_{&delegate} {}

auto Self::SendHttpRequest(ReqDesc desc, OnComplete cb) const -> Canceller {
  auto ptr = std::make_shared<BlockHttpRequest>(desc, cb);
  ptr->Send(loader_factory_);
  std::weak_ptr<BlockHttpRequest> w = ptr;
  return [w](){
    auto p = w.lock();
    if (p) {
      p->Cancel();
    }
  };
}
