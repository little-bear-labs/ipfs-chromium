#include "interceptor.h"

#include "loader.h"

#include "services/network/public/cpp/resource_request.h"

#include <iostream>

using Interceptor = ipfs::Interceptor;
Interceptor::Interceptor() {}

void handle(ipfs::Interceptor::Interceptor::LoaderCallback&& loader_callback)
{
  std::move(loader_callback).Run(
    base::BindOnce(
        &ipfs::Loader::StartRequest
      , std::make_shared<ipfs::Loader>()
      )
    );

}

void Interceptor::MaybeCreateLoader(network::ResourceRequest const& req, content::BrowserContext*, LoaderCallback loader_callback)
{
  std::clog << "\n\n ### HIT " << __PRETTY_FUNCTION__ << " : " << req.url << " ### \n\n";
  if ( req.url.SchemeIs("ipfs") )
  {
    std::clog << "\n ## IT IS IPFS ## \n";
    handle(std::move(loader_callback));
  }
  else if ( req.url.SchemeIs("ipns") )
  {
    std::clog << "\n ## IT IS IPNS ## \n";
    handle(std::move(loader_callback));
  }
  else
  {
    std::clog << "\n ## Not my URL! \n";
    std::move(loader_callback).Run({});//SEP
  }
}
