#include "loader.h"

namespace i = ipfs;
namespace n = network;
namespace m = n::mojom;

i::Loader::Loader( n::URLLoaderContext& ctxt, n::ResourceRequest const& res_req )
: URLLoader(
          ctxt
        , DeleteCallback{}
        , mojo::PendingReceiver<m::URLLoader> {}
        , 0
        , res_req
        , mojo::PendingRemote<m::URLLoaderClient>{}
        , base::WeakPtr<m::URLLoaderClient>{}
        , net::NetworkTrafficAnnotationTag::NotReached()
        , 0
        , 0
        , {}
        , {}
        , {}
        , {}
        , {}
        , {}
        , {}
        , {}
        , {}
        )
{
}
