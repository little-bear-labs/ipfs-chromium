#ifndef ELECTRON_SPIN_SCHEME_URL_LOADER_H_
#define ELECTRON_SPIN_SCHEME_URL_LOADER_H_

#include "components/ipfs/url_loader_factory.h"

namespace electron_spin {
using SchemeToLoaderMap = ipfs::NonNetworkURLLoaderFactoryMap;
void CreateLoaderFactories(SchemeToLoaderMap* in_out,
                           content::BrowserContext* context,
                           network::mojom::URLLoaderFactory* default_factory,
                           network::mojom::NetworkContext* net_ctxt,
                           PrefService* pref_svc);
}  // namespace electron_spin

#endif  // ELECTRON_SPIN_SCHEME_URL_LOADER_H_
