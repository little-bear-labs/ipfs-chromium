#include "loader.h"

#include "ipfs_client/gateways.h"

#include "base/threading/platform_thread.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "services/network/url_loader_factory.h"

#define CP { std::clog << "\n ### " << __PRETTY_FUNCTION__ << " ### " << base::PlatformThread::GetName() << " ### \n"; }

using Self = ipfs::Loader;

Self::Loader(network::mojom::URLLoaderFactory* handles_http)
: lower_loader_factory_{handles_http}
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
  me->gateways_[FREE | MAYB] = Gateways{}.get_list();
  me->startup(me);
}

void Self::startup(ptr me, unsigned concurrent)
{
    CP
    while ( concurrent > 0 && start_gateway_request(me) ) {
        --concurrent;
    }
}

bool Self::start_gateway_request( ptr me )
{
    CP
    for ( auto stature : {GOOD, MAYB} ) {
        auto& from = gateways_[stature | FREE];
        auto& busy = gateways_[stature | BUSY];
        if ( start_gateway_request(me, from, busy) ) {
            return true;
        }
    }
    return false;
}
constexpr net::NetworkTrafficAnnotationTag kTrafficAnnotation =
    net::DefineNetworkTrafficAnnotation("app_preload_service_web_installer", R"(
      semantics {
        sender: "App Preload Service"
        description:
          "Sends a request to a Google server to retrieve app installation"
          "information."
        trigger:
          "Requests are sent after the App Preload Service has performed an"
          "initial request to get a list of apps to install."
        data: "None"
        destination: GOOGLE_OWNED_SERVICE
      }
      policy {
        cookies_allowed: NO
        setting: "This feature cannot be disabled by settings."
        policy_exception_justification:
          "This feature is required to deliver core user experiences and "
          "cannot be disabled by policy."
      }
    )");
bool Self::start_gateway_request( ptr me, GatewayList& free_gws, GatewayList& busy_gws )
{
    CP
    std::string suffix = "/ipfs/bafybeiaysi4s6lnjev27ln5icwm6tueaw2vdykrtjkwiphwekaywqhcjze";
    auto gw_it = std::find_if(
              free_gws.begin()
            , free_gws.end()
            , [&suffix](auto&gw){return gw.accept(suffix);}
            );
    if ( free_gws.end() == gw_it ) {
        return false;
    }
    //TODO lots of things wrong with this function
    auto req = std::make_unique<network::ResourceRequest>();
    req->url = GURL{gw_it->url_prefix() + suffix};
    auto idx = gateway_requests_.size();
    gateway_requests_.emplace_back(
        network::SimpleURLLoader::Create(
              std::move(req)
            , kTrafficAnnotation
            , FROM_HERE
            )
        );
    auto cb = base::BindOnce(
          &Self::on_gateway_response
        ,base::Unretained(this)
        , me
        , idx
        );
    gateway_requests_.back()->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
          lower_loader_factory_
        , std::move(cb)
        );
    busy_gws.insert( *gw_it );
    free_gws.erase( gw_it );
    return true;
}
void Self::on_gateway_response(ptr me,std::size_t req_idx, std::unique_ptr<std::string> body)
{
    CP;
    network::mojom::URLResponseHead const* head = gateway_requests_[req_idx]->ResponseInfo();
    if ( head ) {
        std::clog << head->headers->GetStatusLine()
                  << ' ' << head->headers->GetStatusText() << '\n';
    } else {
        std::clog << "Null head.\n";
    }
    if ( body ) {
        std::clog << "Received response with " << body->size() << " bytes.\n";
    } else {
        std::clog << "No body.\n";
        return;
    }
    if ( !head ) {
        return;
    }
    auto result = mojo::CreateDataPipe(body->size(), pipe_prod_, pipe_cons_);
    if ( result ) {
        std::clog << " ERROR: failed to create data pipe: " << result << '\n';
        return;
    }
    std::clog << " Writing response on pipe.\n";
    std::uint32_t write_size = body->size();
    pipe_prod_->WriteData(body.get(), &write_size, MOJO_BEGIN_WRITE_DATA_FLAG_ALL_OR_NONE);
    auto altered_head = head->Clone();
    std::clog << "head->web_bundle_url=" << altered_head->web_bundle_url.spec() << '\n';
    altered_head->web_bundle_url = GURL{"file://" + altered_head->web_bundle_url.spec()};
    client_->OnReceiveResponse(
//          network::mojom::URLResponseHead::From(*head)
          std::move(altered_head)
        , std::move(pipe_cons_) //Danger Will Robinson!d
        , absl::nullopt
        );
    std::iter_swap( gateway_requests_.begin(), gateway_requests_.begin() + req_idx );
    gateway_requests_.resize(1);
}
