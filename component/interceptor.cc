#include "interceptor.h"

#include "inter_request_state.h"
#include "loader.h"

#include "base/logging.h"
#include "base/supports_user_data.h"
#include "content/public/browser/browser_context.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "services/network/url_loader_factory.h"
#include "url/url_util.h"

using Interceptor = ipfs::Interceptor;
constexpr char user_data_key[] = "ipfs_request_userdata";

Interceptor::Interceptor(network::mojom::URLLoaderFactory* handles_http)
: loader_factory_{handles_http}
{
}

namespace {
    ipfs::InterRequestState& GetOrCreateState(content::BrowserContext* context) {
        if (!context) {
            LOG(WARNING) << "No browser context! Using a default IPFS state.";
            static ipfs::InterRequestState static_state;
            return static_state;
        }
        base::SupportsUserData::Data* existing = context->GetUserData(user_data_key);
        if (existing) {
            LOG(INFO) << "Re-using existing IPFS state.";
            return *static_cast<ipfs::InterRequestState*>(existing);
        }
        LOG(INFO) << "Creating new IPFS state for this browser context.";
        auto owned = std::make_unique<ipfs::InterRequestState>();
        ipfs::InterRequestState* raw = owned.get();
        context->SetUserData(
              user_data_key
            , std::move(owned)
            );
        return *raw;
    }
}
namespace {
    void redirect(
          network::ResourceRequest const& req
        , mojo::PendingReceiver<network::mojom::URLLoader>
        , mojo::PendingRemote<network::mojom::URLLoaderClient> cpr
        )
    {
        mojo::Remote<network::mojom::URLLoaderClient> client;
        client.Bind(std::move(cpr));
        auto head = network::mojom::URLResponseHead::New();
        auto redirect = net::RedirectInfo::ComputeRedirectInfo(
            req.method
            , req.url
            , req.site_for_cookies
            , net::RedirectInfo::FirstPartyURLPolicy::UPDATE_URL_ON_REDIRECT
            , net::ReferrerPolicy::NO_REFERRER
            , req.referrer.spec()
            , 308
            , GURL{"https://ipfs-over-http/ipfs/QmNoscE3kNc83dM5rZNUC5UDXChiTdDcgf16RVtFCRWYuU/"}
            , absl::nullopt
            , false
            , false
            , false
            );
        client->OnReceiveRedirect(redirect, std::move(head));
        client->OnComplete(network::URLLoaderCompletionStatus{308});
    }
}
void Interceptor::MaybeCreateLoader(network::ResourceRequest const& req, content::BrowserContext* context, LoaderCallback loader_callback)
{
    if ( req.url.SchemeIs("ipfs") )
    {
        LOG(INFO) << req.url.spec() << " redirecting...";
        std::move(loader_callback).Run( base::BindOnce(redirect) );
    }
    else if ( req.url.host() == "ipfs-over-http" )//TODO try to force this to be an impossible host
    {
        LOG(INFO) << req.url.spec() << " getting intercepted!";
        DCHECK(context);
        std::move(loader_callback).Run(
            base::BindOnce(
              &ipfs::Loader::StartRequest
            , std::make_shared<ipfs::Loader>(loader_factory_,GetOrCreateState(context))
            )
        );
    }
    else
    {
        LOG(INFO) << req.url.spec() << " = NOT being intercepted.";
        std::move(loader_callback).Run({});//SEP
    }
}
