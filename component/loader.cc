#include "loader.h"

#include "gateways.h"

#include "base/threading/platform_thread.h"
#include "services/network/public/cpp/simple_url_loader.h"

#define CP { std::clog << "\n ### " << __PRETTY_FUNCTION__ << " ### " << base::PlatformThread::GetName() << " ### \n"; }

using Self = ipfs::Loader;

Self::Loader()
{
  CP
}
Self::~Loader() noexcept {
  CP
}

void Self::FollowRedirect(
      std::vector<std::string> const& removed_headers
    , net::HttpRequestHeaders  const& modified_headers
    , net::HttpRequestHeaders  const& modified_cors_exempt_headers
    , absl::optional<::GURL>   const& new_url
    )
{
  CP
}
void Self::SetPriority(net::RequestPriority priority, int32_t intra_priority_value)
{
  CP
}
void Self::PauseReadingBodyFromNet()
{
  CP
}
void Self::ResumeReadingBodyFromNet()
{
  CP
}

void Self::StartRequest(
    std::shared_ptr<Loader> me
  , network::ResourceRequest const& resource_request
  , mojo::PendingReceiver<network::mojom::URLLoader      > receiver
  , mojo::PendingRemote  <network::mojom::URLLoaderClient> client
  )
{
  CP
  DCHECK(!me->receiver_.is_bound());
  DCHECK(!me->client_.is_bound());
  me->receiver_.Bind(std::move(receiver));
  me->client_.Bind(std::move(client));
  me->gateways_ = Gateways::instance().get_list();
  me->startup(me);
}

void Self::startup(ptr me, unsigned concurrent)
{
    CP
    while ( concurrent > 0 && start_gateway_request(me) ) {
        --concurrent;
    }
}

bool Self::start_gateway_request(ptr me)
{
    CP
    if ( gateway_index_ < gateways_.size() )
    {
        start_gateway_request( me, gateways_[gateway_index_++] );
        return true;
    }
    return false;
}
void Self::start_gateway_request( ptr me, std::string& gateway )
{
    CP
    //TODO lots of things wrong with this function
    auto req = std::make_unique<network::ResourceRequest>();
    gateway.append("/ipfs/bafybeiaysi4s6lnjev27ln5icwm6tueaw2vdykrtjkwiphwekaywqhcjze");
    req->url = GURL{gateway};
    gateway_requests_.emplace_back(
        network::SimpleURLLoader::Create(
              std::move(req)
            , net::NetworkTrafficAnnotationTag::NotReached()
            , FROM_HERE
            )
        );
    gateway_requests_.back()->DownloadToStringOfUnboundedSizeUntilCrashAndDie(nullptr, base::BindOnce(&Self::on_gateway_response,base::Unretained(this), me));
}
void Self::on_gateway_response(ptr me, std::unique_ptr<std::string> body)
{
    CP
//    client_->OnReceiveResponse(nullptr,body,absl::nullopt);
}
