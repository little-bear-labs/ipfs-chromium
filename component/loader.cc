#include "loader.h"

#include "ipfs_client/gateways.h"

#include "base/threading/platform_thread.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "services/network/public/cpp/parsed_headers.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "services/network/url_loader_factory.h"

#define CP { std::clog << "\n ### " << __PRETTY_FUNCTION__ << " ### " << base::PlatformThread::GetName() << " ### \n"; }

ipfs::Loader::Loader(network::mojom::URLLoaderFactory* handles_http)
: lower_loader_factory_{handles_http}
{
  CP
}
ipfs::Loader::~Loader() noexcept {
  CP
}

void ipfs::Loader::FollowRedirect(
      std::vector<std::string> const& removed_headers
    , net::HttpRequestHeaders  const& modified_headers
    , net::HttpRequestHeaders  const& modified_cors_exempt_headers
    , absl::optional<::GURL>   const& new_url
    )
{
  CP
}
void ipfs::Loader::SetPriority(net::RequestPriority priority, int32_t intra_priority_value)
{
  CP
}
void ipfs::Loader::PauseReadingBodyFromNet()
{
  CP
}
void ipfs::Loader::ResumeReadingBodyFromNet()
{
  CP
}

void ipfs::Loader::StartRequest(
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
  me->startup(me, resource_request.url.PathForRequest());
}

void ipfs::Loader::startup(ptr me, std::string requested_path, unsigned concurrent)
{
    CP
    while ( concurrent > 0 && start_gateway_request(me, requested_path) ) {
        --concurrent;
    }
}

bool ipfs::Loader::start_gateway_request( ptr me, std::string requested_path )
{
    CP
    for ( auto stature : {GOOD, MAYB} ) {
        auto& from = gateways_[stature | FREE];
        auto& busy = gateways_[stature | BUSY];
        if ( start_gateway_request(me, from, busy, requested_path) ) {
            return true;
        }
    }
    return false;
}
constexpr net::NetworkTrafficAnnotationTag kTrafficAnnotation =
    net::DefineNetworkTrafficAnnotation("ipfs_gateway_request", R"(
      semantics {
        sender: "IPFS component"
        description:
          "Sends a request to an IPFS gateway for a block."
        trigger:
          "Processing of an ipfs:// URL (possibly from an ipns:// URL)."
        data: "None"
        destination: WEBSITE
      }
      policy {
        cookies_allowed: NO
        setting: "Currently, this feature cannot be disabled by settings. TODO"
      }
    )");
bool ipfs::Loader::start_gateway_request( ptr me, GatewayList& free_gws, GatewayList& busy_gws, std::string requested_path )
{
    CP
    auto gw_it = std::find_if(
              free_gws.begin()
            , free_gws.end()
            , [requested_path](auto&gw){return gw.accept(requested_path);}
            );
    if ( free_gws.end() == gw_it ) {
        return false;
    }
    //TODO lots of things wrong with this function
    auto req = std::make_unique<network::ResourceRequest>();
    auto url = gw_it->url_prefix() + "ipfs" + requested_path.substr(1);
    std::clog << "Requesting URL " << url << '\n';
    req->url = GURL{url};
    auto idx = gateway_requests_.size();
    gateway_requests_.emplace_back(
        url
      , network::SimpleURLLoader::Create(
              std::move(req)
            , kTrafficAnnotation
            , FROM_HERE
            )
        );
    auto cb = base::BindOnce(
          &ipfs::Loader::on_gateway_response
        , base::Unretained(this)
        , me
        , requested_path
        , idx
        );
    gateway_requests_.back().second->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
          lower_loader_factory_
        , std::move(cb)
        );
    busy_gws.insert( *gw_it );
    free_gws.erase( gw_it );
    return true;
}
void ipfs::Loader::on_gateway_response( ptr me, std::string requested_path, std::size_t req_idx, std::unique_ptr<std::string> body )
{
    auto from_url = gateway_requests_.at(req_idx).first;
    network::SimpleURLLoader* http_loader = gateway_requests_.at(req_idx).second.get();
    if ( handle_response(from_url, http_loader, body.get()) )
    {
        std::iter_swap( gateway_requests_.begin(), gateway_requests_.begin() + req_idx );
        //TODO remove only identical requests
        gateway_requests_.resize(1);
    }
    else
    {
        std::clog << "Response failure, starting new request.\n";
        start_gateway_request( me, requested_path );
    }
}
bool ipfs::Loader::handle_response( std::string from_url, network::SimpleURLLoader* gw_req, std::string* body )
{
    CP;
    if ( !body ) {
        return false;
    }
    network::mojom::URLResponseHead const* head = gw_req->ResponseInfo();
    if ( !head ) {
        std::clog << from_url << " Null head.\n";
        return false;
    }
    std::string reported_path;
    head->headers->EnumerateHeader(nullptr, "X-Ipfs-Path", &reported_path);
    if ( reported_path.empty() ) {
        std::clog << from_url << " gave us a non-ipfs response.\n";
        return false;
    }
    if ( from_url.find(reported_path) == std::string::npos ) {
        std::clog << "Requested " << from_url << " but got a response to " << reported_path << ". Most likely this is just a different hash algo.\n";
    }
    auto result = mojo::CreateDataPipe(body->size(), pipe_prod_, pipe_cons_);
    if ( result ) {
        std::clog << " ERROR: failed to create data pipe: " << result << '\n';
        return false;
    }
    std::uint32_t write_size = body->size();
    std::clog << " Writing response on pipe. " << write_size << " bytes\n";
    pipe_prod_->WriteData(body->c_str(), &write_size, MOJO_BEGIN_WRITE_DATA_FLAG_ALL_OR_NONE);

    auto head_out = head->Clone();

    auto raw = head_out->headers->raw_headers();
    std::replace(raw.begin(),raw.end(),'\0','\n');
    std::clog << "before headers:|" << raw << "|\n";

    head_out->mime_type = "text/html";
    head_out->content_length = write_size;
    head_out->headers->RemoveHeader("access-control-allow-origin");
    head_out->headers->RemoveHeader("x-content-type-options");
    head_out->headers->RemoveHeader("report-to");
    head_out->headers->RemoveHeader("referrer-policy");
    head_out->headers->RemoveHeader("content-encoding");
    head_out->headers->RemoveHeader("nel");
    head_out->headers->RemoveHeader("link");
    raw = head_out->headers->raw_headers();
    std::replace(raw.begin(),raw.end(),'\0','\n');
    std::clog << "after headers:|" << raw << "|\n";

    auto ipfs_uri = "ipfs:/" + reported_path;
    std::clog << "ipfs_uri=" << ipfs_uri << std::endl;
    head_out->parsed_headers = network::PopulateParsedHeaders(
          head->headers.get()
        , GURL{ipfs_uri}
    );
    head_out->was_fetched_via_spdy = false;

    client_->OnReceiveResponse(
          std::move(head_out)
        , std::move(pipe_cons_) //Danger Will Robinson!d
        , absl::nullopt
        );
    client_->OnComplete(network::URLLoaderCompletionStatus{});
    return true;
}
