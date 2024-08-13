#include "interceptors.h"

#include <components/ipfs/interceptor.h>

void electron_spin::AddInterceptors(std::vector<Interceptor>& in_out,
                                    network::mojom::URLLoaderFactory* ulf,
                                    network::mojom::NetworkContext* nc) {
  in_out.push_back(std::make_unique<ipfs::Interceptor>(ulf, nc));
}
