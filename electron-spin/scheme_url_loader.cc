#include "scheme_url_loader.h"

#include "components/ipfs/url_loader_factory.h"

void electron_spin::CreateLoaderFactories(
    SchemeToLoaderMap* in_out,
    content::BrowserContext* context,
    network::mojom::URLLoaderFactory* default_factory,
    network::mojom::NetworkContext* net_ctxt,
    PrefService* pref_svc) {
  ipfs::IpfsURLLoaderFactory::Create(in_out, context, default_factory, net_ctxt,
                                     pref_svc);
}
