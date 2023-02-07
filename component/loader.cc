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

#include <iostream>

#define CP { std::clog << "\n ### " << __PRETTY_FUNCTION__ << " ### " << base::PlatformThread::GetName() << " ### \n"; }

ipfs::Loader::Loader(network::mojom::URLLoaderFactory* handles_http, GatewayList&& initial_gws)
: lower_loader_factory_{handles_http}
, sched_{std::move(initial_gws)}
{
  CP
}
ipfs::Loader::~Loader() noexcept {
  CP
}

void ipfs::Loader::FollowRedirect(
      std::vector<std::string> const& //removed_headers
    , net::HttpRequestHeaders  const& //modified_headers
    , net::HttpRequestHeaders  const& //modified_cors_exempt_headers
    , absl::optional<::GURL>   const& //new_url
    )
{
  CP
}
void ipfs::Loader::SetPriority(net::RequestPriority /*priority*/, int32_t /*intra_priority_value*/)
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
  auto path = resource_request.url.spec();
  path.erase(4,2);
  me->startup(me, path);
}

void ipfs::Loader::startup(ptr me, std::string requested_path, unsigned concurrent)
{
    CP
    while ( concurrent > 0 && start_gateway_request(me, requested_path) ) {
        --concurrent;
    }
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
bool ipfs::Loader::start_gateway_request( ptr me, std::string requested_path )
{
    std::clog << "start_gateway_request(" << static_cast<void*>(me.get()) << ',' << requested_path << ") in " << __PRETTY_FUNCTION__  << '\n';
    auto assigned = sched_.schedule(requested_path);
    //TODO lots of things wrong with this function
    auto req = std::make_unique<network::ResourceRequest>();
    req->url = GURL{assigned->url()};
    auto idx = gateway_requests_.size();
    gateway_requests_.emplace_back(
        std::move(assigned)
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
    return true;
}
void ipfs::Loader::on_gateway_response( ptr me, std::string requested_path, std::size_t req_idx, std::unique_ptr<std::string> body )
{
    auto& gw = gateway_requests_.at(req_idx).first;
    auto& http_loader = gateway_requests_.at(req_idx).second;
    if ( handle_response(gw.get(), http_loader.get(), body.get()) )
    {
        std::iter_swap( gateway_requests_.begin(), gateway_requests_.begin() + req_idx );
        //TODO remove only identical requests
        gateway_requests_.erase(std::next(gateway_requests_.begin()),gateway_requests_.end());
    }
    else
    {
        gw.reset();
        http_loader.reset();
        std::clog << "Response failure, starting new request.\n";
        start_gateway_request( me, requested_path );
    }
}
bool ipfs::Loader::handle_response( Gateway* gw, network::SimpleURLLoader* gw_req, std::string* body )
{
    CP;
    if ( !body ) {
        return false;
    }
    network::mojom::URLResponseHead const* head = gw_req->ResponseInfo();
    if ( !head ) {
        std::clog << gw->url() << " Null head.\n";
        return false;
    }
    std::string reported_path;
    head->headers->EnumerateHeader(nullptr, "X-Ipfs-Path", &reported_path);
    if ( reported_path.empty() ) {
        std::clog << gw->url() << " gave us a non-ipfs response.\n";
        return false;
    }
    if ( gw->url().find(reported_path) == std::string::npos ) {
        std::clog << "Requested " << gw->url() << " but got a response to " << reported_path << ". Most likely this is just a different hash algo.\n";
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

    head_out->mime_type = head->mime_type;//"text/html";
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
