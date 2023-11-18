#ifndef IPFS_CHROMIUM_IPFS_CONTEXT_H_
#define IPFS_CHROMIUM_IPFS_CONTEXT_H_

#include "dns_txt_request.h"

#include <ipfs_client/block_storage.h>
#include <ipfs_client/context_api.h>

#include <base/memory/raw_ref.h>
#include <base/time/time.h>

#include <vocab/raw_ptr.h>

#include <map>

namespace network {
class SimpleURLLoader;
namespace mojom {
class URLLoaderFactory;
}
}  // namespace network

namespace ipfs {
class InterRequestState;
class IpfsRequest;
class NetworkRequestor;

class ChromiumIpfsContext final : public ContextApi {
  raw_ptr<network::mojom::URLLoaderFactory> loader_factory_ = nullptr;
  raw_ptr<network::mojom::NetworkContext> network_context_;
  raw_ref<InterRequestState> state_;
  std::map<std::string, std::unique_ptr<DnsTxtRequest>> dns_reqs_;

  std::string MimeType(std::string extension,
                       std::string_view content,
                       std::string const& url) const override;
  std::string UnescapeUrlComponent(std::string_view) const override;
  void SendDnsTextRequest(std::string,
                          DnsTextResultsCallback,
                          DnsTextCompleteCallback) override;
  void SendHttpRequest(HttpRequestDescription req_inf,
                       HttpCompleteCallback cb) const override;
  bool verify_key_signature(SigningKeyType,
                            ByteView signature,
                            ByteView data,
                            ByteView key_bytes) const override;

  std::unique_ptr<DagCborValue> ParseCbor(ByteView) const override;

 public:
  ChromiumIpfsContext(InterRequestState&,
                      raw_ptr<network::mojom::NetworkContext> network_context);
  ~ChromiumIpfsContext() noexcept override;
  void SetLoaderFactory(network::mojom::URLLoaderFactory&);
};

}  // namespace ipfs

#endif  // IPFS_CHROMIUM_IPFS_CONTEXT_H_
