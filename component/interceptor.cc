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

Interceptor::Interceptor(network::mojom::URLLoaderFactory* handles_http)
    : loader_factory_{handles_http} {}

#ifdef REDIRECT_THROUGH_HTTP
namespace {
void redirect(network::ResourceRequest const& req,
              mojo::PendingReceiver<network::mojom::URLLoader>,
              mojo::PendingRemote<network::mojom::URLLoaderClient> cpr) {
  mojo::Remote<network::mojom::URLLoaderClient> client;
  client.Bind(std::move(cpr));
  auto to_url = ipfs::IpfsUri2IpfsOverHttpUrl(req.url.spec());
  LOG(WARNING) << "Redirecting " << req.url.spec() << " to " << to_url;
  auto redirect = net::RedirectInfo::ComputeRedirectInfo(
      req.method, req.url, req.site_for_cookies,
      net::RedirectInfo::FirstPartyURLPolicy::UPDATE_URL_ON_REDIRECT,
      net::ReferrerPolicy::NO_REFERRER, req.referrer.spec(), 308, GURL{to_url},
      absl::nullopt, false, false, false);
  client->OnReceiveRedirect(redirect, network::mojom::URLResponseHead::New());
  client->OnComplete(network::URLLoaderCompletionStatus{308});
}
}  // namespace
#endif

void Interceptor::MaybeCreateLoader(network::ResourceRequest const& req,
                                    content::BrowserContext* context,
                                    LoaderCallback loader_callback) {
  if (req.url.SchemeIs("ipfs") || req.url.SchemeIs("ipns")) {
#ifdef REDIRECT_THROUGH_HTTP
    LOG(INFO) << req.url.spec() << " redirecting...";
    std::move(loader_callback).Run(base::BindOnce(redirect));
  } else if (req.url.spec().find(ipfs::IPFS_OVER_HTTP_DOMAIN) !=
             std::string::npos) {
#endif
    auto hdr_str = req.headers.ToString();
    std::replace(hdr_str.begin(), hdr_str.end(), '\r', ' ');
    LOG(INFO) << req.url.spec() << " getting intercepted! Headers: \n"
              << hdr_str;
    DCHECK(context);
    std::move(loader_callback)
        .Run(base::BindOnce(
            &ipfs::Loader::StartRequest,
            std::make_shared<ipfs::Loader>(
                loader_factory_,
                InterRequestState::FromBrowserContext(context))));
  } else {
    LOG(INFO) << req.url.spec() << " has host '" << req.url.host()
              << "' and is not being intercepted.";
    std::move(loader_callback).Run({});  // SEP
  }
}
