#include "interceptor.h"

#include "inter_request_state.h"
#include "ipfs_url_loader.h"

#include "base/logging.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "services/network/url_loader_factory.h"
#include "url/url_util.h"

using Interceptor = ipfs::Interceptor;

Interceptor::Interceptor(network::mojom::URLLoaderFactory* handles_http,
                         network::mojom::NetworkContext* network_context)
    : loader_factory_{handles_http}, network_context_{network_context} {}

void Interceptor::MaybeCreateLoader(network::ResourceRequest const& req,
                                    content::BrowserContext* context,
                                    LoaderCallback loader_callback) {
  auto& state = InterRequestState::FromBrowserContext(context);
  state.network_context(network_context_);
  if (req.url.SchemeIs("ipfs") || req.url.SchemeIs("ipns")) {
    VLOG(2) << "Intercepting " << req.url.spec()
      << " because scheme is " << req.url.scheme();
    auto hdr_str = req.headers.ToString();
    std::replace(hdr_str.begin(), hdr_str.end(), '\r', ' ');
    DCHECK(context);
    auto loader =
        std::make_shared<ipfs::IpfsUrlLoader>(*loader_factory_, state);
    std::move(loader_callback)
        .Run(base::BindOnce(&ipfs::IpfsUrlLoader::StartRequest, loader));

  } else {
    VLOG(2) << "Not intercepting " << req.url.spec()
      << " because scheme is " << req.url.scheme();
    std::move(loader_callback).Run({});
  }
}
