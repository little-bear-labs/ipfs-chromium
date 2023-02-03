#include "interceptor.h"

#include "loader.h"

#include "base/supports_user_data.h"
#include "content/public/browser/browser_context.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/url_loader_factory.h"
#include "url/url_util.h"

#include <iostream>

using Interceptor = ipfs::Interceptor;
//constexpr char user_data_key[] = "ipfs_request_userdata";

Interceptor::Interceptor(network::mojom::URLLoaderFactory* handles_http)
: loader_factory_{handles_http}
{
}

void handle(
      network::mojom::URLLoaderFactory* loader_factory
    , content::BrowserContext* context
    , ipfs::Interceptor::Interceptor::LoaderCallback&& loader_callback
    )
{
//  base::SupportsUserData::Data* inter_request_state = context
//        ? context->GetUserData(user_data_key)
//        : nullptr
//        ;
  std::move(loader_callback).Run(
    base::BindOnce(
        &ipfs::Loader::StartRequest
      , std::make_shared<ipfs::Loader>(loader_factory)
      )
    );
}

void Interceptor::MaybeCreateLoader(network::ResourceRequest const& req, content::BrowserContext* context, LoaderCallback loader_callback)
{
  std::clog << "\n\n ### HIT " << __PRETTY_FUNCTION__ << " : " << req.url << " ### \n\n";
  if ( req.url.SchemeIs("ipfs") )
  {
    std::clog << "\n ## IT IS IPFS ## \n";
    handle(loader_factory_, context, std::move(loader_callback));
  }
  else if ( req.url.SchemeIs("ipns") )
  {
    std::clog << "\n ## IT IS IPNS ## \n";
    handle(loader_factory_, context, std::move(loader_callback));
  }
  else
  {
    std::clog << "\n ## Not my URL! \n";
    std::move(loader_callback).Run({});//SEP
  }
}
