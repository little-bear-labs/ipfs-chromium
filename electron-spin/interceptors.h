#ifndef SPIN_INTERCEPTORS_H_
#define SPIN_INTERCEPTORS_H_

#include <memory>
#include <vector>

namespace content {
class URLLoaderRequestInterceptor;
}
namespace network::mojom {
class URLLoaderFactory;
class NetworkContext;
}  // namespace network::mojom

using Interceptor = std::unique_ptr<content::URLLoaderRequestInterceptor>;

namespace electron_spin {
void AddInterceptors(std::vector<Interceptor>& in_out,
                     network::mojom::URLLoaderFactory* handles_http,
                     network::mojom::NetworkContext*);
}

#endif  // SPIN_INTERCEPTORS_H_
