#include "ipfs_client/gw/gateway_request.h"

#include "ipfs_client/response.h"

#include "log_macros.h"

#include <libp2p/multi/content_identifier_codec.hpp>

using namespace std::literals;

using Self = ipfs::gw::GatewayRequest;
using CidCodec = libp2p::multi::ContentIdentifierCodec;

std::shared_ptr<Self> Self::fromIpfsPath(ipfs::SlashDelimited p) {
  auto name_space = p.pop();
  auto result = std::make_shared<Self>();
  result->main_param = p.pop();
  auto maybe_cid = CidCodec::fromString(result->main_param);
  if (maybe_cid.has_value()) {
    result->cid = maybe_cid.value();
  } else {
    result->cid = std::nullopt;
  }
  if (name_space == "ipfs") {
    if (!result->cid.has_value()) {
      LOG(ERROR) << "IPFS request with invalid/unsupported CID "
                 << result->main_param;
    }
    if (result->cid.value().content_address.getType() ==
        libp2p::multi::HashType::identity) {
      result->type = Type::Identity;
    } else {
      result->path = p.pop_all();
      result->type = result->path.empty() ? Type::Block : Type::Car;
    }
  } else if (name_space == "ipns") {
    result->path = p.pop_all();
    if (CidCodec::fromString(result->main_param).has_value()) {
      result->type = Type::Ipns;
    } else {
      result->type = Type::DnsLink;
    }
  } else {
    LOG(FATAL) << "Unsupported namespace in ipfs path: /" << name_space << '/'
               << p.pop_all();
  }
  return result;
}

std::string Self::url_suffix() const {
  switch (type) {
    case Type::Block:
      return "/ipfs/" + main_param;
    case Type::Car:
      return "/ipfs/" + main_param + "/" + path + "?dag-scope=entity";
    case Type::Ipns:
      return "/ipns/" + main_param;
    case Type::Providers:
      return "/routing/v1/providers/" + main_param;
    case Type::DnsLink:
      LOG(FATAL) << "Don't try to use HTTP(s) for DNS TXT records.";
      return {};
    default:
      LOG(FATAL) << "Invalid gateway request type: " << static_cast<int>(type);
      return {};
  }
}
std::string_view Self::accept() const {
  switch (type) {
    case Type::Block:
      return "application/vnd.ipld.raw"sv;
    case Type::Ipns:
      return "application/vnd.ipfs.ipns-record"sv;
    case Type::Car:
      return "application/vnd.ipld.car"sv;
    case Type::Providers:
      return "application/json"sv;
    case Type::DnsLink:
      // TODO : not sure this advice is 100% good, actually.
      //   If the user's system setup allows for text records to actually work,
      //   it would be good to respect their autonomy and try to follow the
      //   system's DNS setup. However, it's extremely easy to get yourself in a
      //   situation where Chromium _cannot_ access text records. If you're in
      //   that scenario, it might be better to try to use an IPFS gateway with
      //   DNSLink capability.
      LOG(FATAL) << "Don't try to use HTTP(s) for DNS TXT records.";
      return {};
    default:
      LOG(FATAL) << "Invalid gateway request type: " << static_cast<int>(type);
      return {};
  }
}

auto Self::identity_data() const -> std::string_view {
  if (type != Type::Identity) {
    return "";
  }
  auto hash = cid.value().content_address.getHash();
  auto d = reinterpret_cast<char const*>(hash.data());
  return std::string_view{d, hash.size()};
}
