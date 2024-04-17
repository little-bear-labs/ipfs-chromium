#include <ipfs_client/gw/gateway_request.h>

#include <ipfs_client/gw/providers_response.h>
#include <ipfs_client/ipld/chunk.h>
#include <ipfs_client/ipld/dag_node.h>
#include <ipfs_client/ipld/dnslink_name.h>
#include <ipfs_client/ipld/ipns_name.h>

#include <ipfs_client/car.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/ipns_record.h>
#include <ipfs_client/partition.h>
#include <ipfs_client/pb_dag.h>
#include <ipfs_client/response.h>

#include "log_macros.h"

#include <array>
#include <sstream>

using namespace std::literals;

using Self = ipfs::gw::GatewayRequest;

std::shared_ptr<Self> Self::fromIpfsPath(ipfs::SlashDelimited p) {
  auto name_space = p.pop();
  auto r = std::make_shared<Self>();
  r->main_param = p.pop();
  Cid cid(r->main_param);
  if (cid.valid()) {
    r->cid = std::move(cid);
  } else {
    r->cid = std::nullopt;
  }
  if (name_space == "ipfs") {
    if (!r->cid.has_value()) {
      LOG(ERROR) << "IPFS request with invalid/unsupported CID "
                 << r->main_param;
      return {};
    }
    if (r->cid.value().hash_type() == HashType::IDENTITY) {
      r->type = GatewayRequestType::Identity;
    } else {
      r->path = p.pop_all();
      r->type =
          r->path.empty() ? GatewayRequestType::Block : GatewayRequestType::Car;
    }
  } else if (name_space == "ipns") {
    r->path = p.pop_all();
    if (Cid(r->main_param).valid()) {
      r->type = GatewayRequestType::Ipns;
    } else {
      r->type = GatewayRequestType::DnsLink;
    }
  } else {
    LOG(FATAL) << "Unsupported namespace in ipfs path: /" << name_space << '/'
               << p.pop_all();
  }
  return r;
}

std::string Self::url_suffix() const {
  switch (type) {
    case GatewayRequestType::Block:
      return "/ipfs/" + main_param;
    case GatewayRequestType::Car:
      //      return "/ipfs/" + main_param + "/" + path + "?dag-scope=entity";
      return "/ipfs/" + main_param + "/" + path +
             "?entity-bytes=0:" + std::to_string(BLOCK_RESPONSE_BUFFER_SIZE);
    case GatewayRequestType::Ipns:
      return "/ipns/" + main_param;
    case GatewayRequestType::Providers:
      return "/routing/v1/providers/" + main_param;
    case GatewayRequestType::DnsLink:
      LOG(FATAL) << "Don't try to use HTTP(s) for DNS TXT records.";
#ifdef __clang__
#pragma clang diagnostic ignored "-Wunreachable-code-return"
#endif
      return {};
    case GatewayRequestType::Identity:
    case GatewayRequestType::Zombie:
      return {};
  }
  LOG(FATAL) << "Unhandled gateway request type: " << static_cast<int>(type);
  return {};
}
std::string_view Self::accept() const {
  switch (type) {
    case GatewayRequestType::Block:
      return "application/vnd.ipld.raw"sv;
    case GatewayRequestType::Ipns:
      return "application/vnd.ipfs.ipns-record"sv;
    case GatewayRequestType::Car:
      return "application/vnd.ipld.car"sv;
    case GatewayRequestType::Providers:
      return "application/json"sv;
    case GatewayRequestType::DnsLink:
      // TODO : not sure this advice is 100% good, actually.
      //   If the user's system setup allows for text records to actually work,
      //   it would be good to respect their autonomy and try to follow the
      //   system's DNS setup. However, it's extremely easy to get yourself in a
      //   situation where Chromium _cannot_ access text records. If you're in
      //   that scenario, it might be better to try to use an IPFS gateway with
      //   DNSLink capability.
      LOG(FATAL) << "Don't try to use HTTP(s) for DNS TXT records.";
      return {};
    case GatewayRequestType::Identity:
    case GatewayRequestType::Zombie:
      return {};
  }
  LOG(FATAL) << "Invalid gateway request type: " << static_cast<int>(type);
  return {};
}
short Self::timeout_seconds() const {
  switch (type) {
    case GatewayRequestType::DnsLink:
      return 8;
    case GatewayRequestType::Providers:
      return 16;
    case GatewayRequestType::Block:
      return 32;
    case GatewayRequestType::Car:
      return 64;
    case GatewayRequestType::Ipns:
      return 128;
    case GatewayRequestType::Identity:
    case GatewayRequestType::Zombie:
      return 0;
  }
  LOG(FATAL) << "timeout_seconds() called for unsupported gateway request type "
             << static_cast<int>(type);
  return 0;
}

auto Self::identity_data() const -> std::string_view {
  if (type != GatewayRequestType::Identity) {
    return "";
  }
  auto hash = cid.value().hash();
  auto d = reinterpret_cast<char const*>(hash.data());
  return std::string_view{d, hash.size()};
}

bool Self::is_http() const {
  switch (type) {
    case GatewayRequestType::Ipns:
    case GatewayRequestType::Car:
    case GatewayRequestType::Block:
    case GatewayRequestType::Providers:
      return true;
    case GatewayRequestType::Identity:
    case GatewayRequestType::DnsLink:
    case GatewayRequestType::Zombie:
      return false;
  }
  return true;
}
auto Self::describe_http(std::string_view prefix) const
    -> std::optional<HttpRequestDescription> {
  if (!is_http()) {
    return {};
  }
  DCHECK(!prefix.empty());
  auto url = url_suffix();
  if (url.front() == '/' && prefix.back() == '/') {
    prefix.remove_suffix(1UL);
  } else if (url.front() != '/' && prefix.back() != '/') {
    url.insert(0UL, 1UL, '/');
  }
  url.insert(0UL, prefix);
  return HttpRequestDescription{url, timeout_seconds(), std::string{accept()},
                                max_response_size()};
}
std::optional<std::size_t> Self::max_response_size() const {
  switch (type) {
    case GatewayRequestType::Identity:
      return 0;
    case GatewayRequestType::DnsLink:
      return std::nullopt;
    case GatewayRequestType::Ipns:
      return MAX_IPNS_PB_SERIALIZED_SIZE;
    case GatewayRequestType::Block:
      return BLOCK_RESPONSE_BUFFER_SIZE;
    case GatewayRequestType::Car: {
      return BLOCK_RESPONSE_BUFFER_SIZE * 2;
    }
    case GatewayRequestType::Zombie:
      return 0;
    case GatewayRequestType::Providers:
      // This one's tricky.
      //   One could easily guess a practical limit to the size of a Peer's
      //    json, and the spec says it SHOULD be limited to 100 peers.
      //   But there's no guaranteed limits. A peer could have an unlimited
      //    number of multiaddrs. And they're allowed to throw in arbitrary
      //    fields I'm supposed to ignore. So in theory it could be arbitrarily
      //    large.
      return std::nullopt;
  }
  LOG(ERROR) << "Invalid gateway request type " << static_cast<int>(type);
  return std::nullopt;
}
bool Self::cachable() const {
  using ipfs::gw::GatewayRequestType;
  switch (type) {
      //    case GatewayRequestType::Car:
      //      return path.find("/ipns/") == std::string::npos;
    case GatewayRequestType::Block:
    case GatewayRequestType::Ipns:
      return true;
    case GatewayRequestType::Car:
    case GatewayRequestType::DnsLink:
    case GatewayRequestType::Providers:
    case GatewayRequestType::Identity:
    case GatewayRequestType::Zombie:
      return false;
  }
  LOG(ERROR) << "Unhandled request type: " << debug_string();
  return false;
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
std::string Self::Key() const {
  auto rv = main_param;
  //  rv.append(" ").append(name(type)).append(" ").append(path);
  return rv;
}
bool Self::RespondSuccessfully(std::string_view bytes,
                               std::shared_ptr<Client> const& api,
                               ipld::BlockSource src,
                               bool* valid) {
  using namespace ipfs::ipld;
  bool success = false;
  if (valid) {
    *valid = false;
  }
  FleshOut(src);
  switch (type) {
    case GatewayRequestType::Block: {
      DCHECK(cid.has_value());
      if (!cid.has_value()) {
        LOG(ERROR) << "Your CID doesn't even have a value!";
        return false;
      }
      DCHECK(api);
      auto node = DagNode::fromBytes(api, cid.value(), bytes);
      if (!node) {
        return false;
      } else {
        node->source(src);
        if (valid) {
          *valid = true;
        }
      }
      success = orchestrator_->add_node(main_param, node);
    } break;
    case GatewayRequestType::Identity: {
      auto node = std::make_shared<Chunk>(std::string{bytes});
      if (node) {
        node->source(src);
      }
      success = orchestrator_->add_node(main_param, node);
      if (valid) {
        *valid = true;
      }
    } break;
    case GatewayRequestType::Ipns:
      if (cid.has_value()) {
        DCHECK(api);
        auto byte_ptr = reinterpret_cast<ipfs::Byte const*>(bytes.data());
        auto rec = ipfs::ValidateIpnsRecord({byte_ptr, bytes.size()},
                                            cid.value(), *api);
        if (rec.has_value()) {
          auto node = std::make_shared<IpnsName>(rec.value());
          if (!node || node->expired()) {
            return false;
          }
          node->source(src);
          success = orchestrator_->add_node(main_param, node);
          if (valid) {
            *valid = !node->expired();
            VLOG(2) << "IPNS node created " << main_param << ' ' << success
                    << " vs. " << *valid;
          }
        } else {
          LOG(ERROR) << "IPNS record failed to validate!";
          return false;
        }
      }
      break;
    case GatewayRequestType::DnsLink: {
      auto node = std::make_shared<ipld::DnsLinkName>(bytes);
      if (node) {
        node->source(src);
      }
      if (orchestrator_) {
        success = orchestrator_->add_node(main_param, node);
      } else {
        LOG(FATAL) << "I have no orchestrator!!";
      }
      if (valid) {
        *valid = !node->expired();
      }
    } break;
    case GatewayRequestType::Car: {
      DCHECK(api);
      Car car(as_bytes(bytes), *api);
      while (auto block = car.NextBlock()) {
        auto cid_s = block->cid.to_string();
        auto n = DagNode::fromBytes(api, block->cid, block->bytes);
        if (!n) {
          LOG(ERROR) << "Unable to handle block from CAR: " << cid_s;
          continue;
        }
        if (valid) {
          *valid = true;
        }
        if (n) {
          n->source(src);
        }
        if (orchestrator_->add_node(cid_s, n)) {
          success = true;
          for (auto& hook : bytes_received_hooks) {
            hook(cid_s, block->bytes, src);
          }
        }
      }
      break;
    }
    case GatewayRequestType::Providers:
      success = providers::ProcessResponse(bytes, *api);
      if (valid) {
        *valid = success;
      }
      break;
    case GatewayRequestType::Zombie:
      LOG(WARNING) << "Responding to a zombie is ill-advised.";
      break;
    default:
      LOG(ERROR) << "TODO " << static_cast<int>(type);
  }
  if (success) {
    if (type != GatewayRequestType::Car) {
      for (auto& hook : bytes_received_hooks) {
        hook(main_param, as_bytes(bytes), src);
      }
    }
    bytes_received_hooks.clear();
    orchestrator_->build_response(dependent);
    type = GatewayRequestType::Zombie;
  }
  return success;
}
void Self::Hook(BytesReceivedHook f) {
  bytes_received_hooks.push_back(f);
}
void Self::orchestrator(std::shared_ptr<Partition> const& orc) {
  orchestrator_ = orc;
}
bool Self::PartiallyRedundant() const {
  if (Finished()) {
    return true;
  }
  if (!orchestrator_) {
    return false;
  }
  return orchestrator_->has_key(main_param);
}
bool Self::Finished() const {
  if (type == GatewayRequestType::Zombie) {
    return true;
  }
  if (type == GatewayRequestType::Providers) {
    return false;
  }
  return !dependent || dependent->done();
}
void Self::FleshOut(ipld::BlockSource& s) const {
  if (cid.has_value() && cid->valid()) {
    s.cid = cid->to_string();
  } else {
    s.cid = main_param;
  }
  s.cat.request_type = type;
}
