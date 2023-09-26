#include "interceptor.h"

#include "inter_request_state.h"
#include "ipfs_url_loader.h"
#include "ipns_url_loader.h"

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
  if (req.url.SchemeIs("ipns")) {
    auto ipns_loader = std::make_shared<IpnsUrlLoader>(
        state, req.url.host(), network_context_, *loader_factory_);
    std::move(loader_callback)
        .Run(base::BindOnce(&ipfs::IpnsUrlLoader::StartHandling, ipns_loader));
  } else if (req.url.SchemeIs("ipfs")) {
    auto hdr_str = req.headers.ToString();
    std::replace(hdr_str.begin(), hdr_str.end(), '\r', ' ');
    LOG(INFO) << req.url.spec() << " getting intercepted! Headers: \n"
              << hdr_str;
    DCHECK(context);
    auto loader =
        std::make_shared<ipfs::IpfsUrlLoader>(*loader_factory_, state);
    std::move(loader_callback)
        .Run(base::BindOnce(&ipfs::IpfsUrlLoader::StartRequest, loader));
  } else {
    VLOG(1) << req.url.spec() << " has host '" << req.url.host()
            << "' and is not being intercepted.";
    std::move(loader_callback).Run({});  // SEP
  }
}
