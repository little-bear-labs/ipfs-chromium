#include <ipfs_client/gw/gateway_request.h>

#include <ipfs_client/ipld/chunk.h>
#include <ipfs_client/ipld/dag_node.h>
#include <ipfs_client/ipld/ipns_name.h>

#include <ipfs_client/dag_block.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/ipns_record.h>
#include <ipfs_client/orchestrator.h>
#include <ipfs_client/response.h>

#include "log_macros.h"

#include <libp2p/multi/content_identifier_codec.hpp>

using namespace std::literals;

using Self = ipfs::gw::GatewayRequest;
using CidCodec = libp2p::multi::ContentIdentifierCodec;

std::shared_ptr<Self> Self::fromIpfsPath(ipfs::SlashDelimited p) {
  auto name_space = p.pop();
  auto r = std::make_shared<Self>();
  r->main_param = p.pop();
  auto maybe_cid = CidCodec::fromString(r->main_param);
  if (maybe_cid.has_value()) {
    r->cid = maybe_cid.value();
  } else {
    r->cid = std::nullopt;
  }
  if (name_space == "ipfs") {
    if (!r->cid.has_value()) {
      LOG(ERROR) << "IPFS request with invalid/unsupported CID "
                 << r->main_param;
      return {};
    }
    if (r->cid.value().content_address.getType() ==
        libp2p::multi::HashType::identity) {
      r->type = Type::Identity;
    } else {
      r->path = p.pop_all();
      r->type = r->path.empty() ? Type::Block : Type::Car;
    }
  } else if (name_space == "ipns") {
    r->path = p.pop_all();
    if (CidCodec::fromString(r->main_param).has_value()) {
      r->type = Type::Ipns;
    } else {
      r->type = Type::DnsLink;
    }
  } else {
    LOG(FATAL) << "Unsupported namespace in ipfs path: /" << name_space << '/'
               << p.pop_all();
  }
  return r;
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
    case Type::Identity:
    case Type::Zombie:
      return {};
  }
  LOG(FATAL) << "Unhandled gateway request type: " << static_cast<int>(type);
  return {};
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
    case Type::Identity:
    case Type::Zombie:
      return {};
  }
  LOG(FATAL) << "Invalid gateway request type: " << static_cast<int>(type);
  return {};
}
short Self::timeout_seconds() const {
  switch (type) {
    case Type::DnsLink:
      return 16;
    case Type::Block:
      return 39;
    case Type::Providers:
      return 64;
    case Type::Car:
      return 128;
    case Type::Ipns:
      return 256;
    case Type::Identity:
    case Type::Zombie:
      return 0;
  }
  LOG(FATAL) << "timeout_seconds() called for unsupported gateway request type "
             << static_cast<int>(type);
  return 0;
}

auto Self::identity_data() const -> std::string_view {
  if (type != Type::Identity) {
    return "";
  }
  auto hash = cid.value().content_address.getHash();
  auto d = reinterpret_cast<char const*>(hash.data());
  return std::string_view{d, hash.size()};
}

bool Self::is_http() const {
  return type != Type::DnsLink && type != Type::Identity;
}
auto Self::describe_http() const -> std::optional<HttpRequestDescription> {
  if (!is_http()) {
    return {};
  }
  return HttpRequestDescription{url_suffix(), timeout_seconds(),
                                std::string{accept()}, max_response_size()};
}
std::optional<std::size_t> Self::max_response_size() const {
  switch (type) {
    case Type::Identity:
      return 0;
    case Type::DnsLink:
      return std::nullopt;
    case Type::Ipns:
      return MAX_IPNS_PB_SERIALIZED_SIZE;
    case Type::Block:
      return BLOCK_RESPONSE_BUFFER_SIZE;
    case Type::Car: {
      auto n = std::count(path.begin(), path.end(), '/');
      // now n >= number of path components, but there is also a block for root
      n++;
      return n * BLOCK_RESPONSE_BUFFER_SIZE;
    }
    case Type::Zombie:
      return 0;
    case Type::Providers:
      // This one's tricky.
      //   One could easily guess a pracitical limit to the size of a Peer,
      //   and the spec says it SHOULD be limited to 100 peers.
      //   But there's no guaranteed limits. A peer could have an unlimited
      //    number of multiaddrs. And they're allowed to throw in arbitrary
      //    fields I'm supposed to ignore. So in theory it could be infinitely
      //    large.
      return std::nullopt;
  }
  LOG(ERROR) << "Invalid gateway request type " << static_cast<int>(type);
  return std::nullopt;
}
std::string_view ipfs::gw::name(ipfs::gw::Type t) {
  using ipfs::gw::Type;
  switch (t) {
    case Type::Block:
      return "Block";
    case Type::Car:
      return "Car";
    case Type::Ipns:
      return "Ipns";
    case Type::DnsLink:
      return "DnsLink";
    case Type::Providers:
      return "Providers";
    case Type::Identity:
      return "Identity";
    case Type::Zombie:
      return "CompletedRequest";
  }
  static std::array<char, 19> buf;
  std::sprintf(buf.data(), "InvalidType %d", static_cast<std::int8_t>(t));
  return buf.data();
}
std::string Self::debug_string() const {
  std::ostringstream oss;
  oss << "Request{Type=" << type << ' ' << main_param;
  if (!path.empty()) {
    oss << ' ' << path;
  }
  if (dependent) {
    oss << " for=" << dependent->path().to_string();
  }
  oss << " plel=" << parallel << '}';
  return oss.str();
}
bool Self::RespondSuccessfully(std::string_view bytes,
                               std::shared_ptr<ContextApi> const& api) {
  using namespace ipfs::ipld;
  bool success = false;
  switch (type) {
    case Type::Block: {
      DCHECK(cid.has_value());
      if (!cid.has_value()) {
        LOG(ERROR) << "Your CID doesn't even hae a value!";
        return false;
      }
      auto node = DagNode::fromBytes(api, cid.value(), bytes);
      success = orchestrator_->add_node(main_param, node);
    } break;
    case Type::Identity:
      success = orchestrator_->add_node(
          main_param, std::make_shared<Chunk>(std::string{bytes}));
      break;
    case Type::Ipns:
      if (cid.has_value()) {
        DCHECK(api);
        auto byte_ptr = reinterpret_cast<ipfs::Byte const*>(bytes.data());
        auto rec = ipfs::ValidateIpnsRecord({byte_ptr, bytes.size()},
                                            cid.value(), *api);
        if (rec.has_value()) {
          auto node = DagNode::fromIpnsRecord(rec.value());
          success = orchestrator_->add_node(main_param, node);
        } else {
          LOG(ERROR) << "IPNS record failed to validate!";
          return false;
        }
      }
      break;
    case Type::DnsLink:
      LOG(INFO) << "Resolved " << debug_string() << " to " << bytes;
      if (orchestrator_) {
        success = orchestrator_->add_node(
            main_param, std::make_shared<ipld::IpnsName>(bytes));
      } else {
        LOG(FATAL) << "I have no orchestrator!!";
      }
      break;
    case Type::Car:
      LOG(WARNING) << "TODO - handle responses to CAR requests.";
      break;
    default:
      LOG(ERROR) << "TODO " << static_cast<int>(type);
  }
  if (success) {
    for (auto& hook : bytes_received_hooks) {
      hook(bytes);
    }
    bytes_received_hooks.clear();
    orchestrator_->build_response(dependent);
  }
  return success;
}
void Self::Hook(std::function<void(std::string_view)> f) {
  bytes_received_hooks.push_back(f);
}
void Self::orchestrator(std::shared_ptr<Orchestrator> const& orc) {
  orchestrator_ = orc;
}
