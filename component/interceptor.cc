#include "interceptor.h"

#include "inter_request_state.h"
#include "loader.h"

#include "base/logging.h"
#include "base/supports_user_data.h"
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
  if (req.url.SchemeIs("ipfs") || req.url.SchemeIs("ipns")) {
    auto hdr_str = req.headers.ToString();
    std::replace(hdr_str.begin(), hdr_str.end(), '\r', ' ');
    LOG(INFO) << req.url.spec() << " getting intercepted! Headers: \n"
              << hdr_str;
    DCHECK(context);
    std::move(loader_callback)
        .Run(base::BindOnce(&ipfs::Loader::StartRequest,
                            std::make_shared<ipfs::Loader>(
                                loader_factory_,
                                InterRequestState::FromBrowserContext(context)),
                            network_context_));
  } else {
    LOG(INFO) << req.url.spec() << " has host '" << req.url.host()
              << "' and is not being intercepted.";
    std::move(loader_callback).Run({});  // SEP
  }
}
