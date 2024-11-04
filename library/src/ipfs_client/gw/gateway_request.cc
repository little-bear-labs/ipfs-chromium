#include <ipfs_client/gw/gateway_request.h>

#include <ipfs_client/gw/providers_response.h>
#include <ipfs_client/ipld/chunk.h>
#include <ipfs_client/ipld/dag_node.h>
#include <ipfs_client/ipld/dnslink_name.h>
#include <ipfs_client/ipld/ipns_name.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/car.h>
#include <ipfs_client/ipns_record.h>
#include <ipfs_client/partition.h>
#include <ipfs_client/pb_dag.h>
#include <ipfs_client/multi_hash.h>
#include <ipfs_client/gw/gateway_request_type.h>
#include <ipfs_client/http_request_description.h>
#include <ipfs_client/ipld/block_source.h>

#include <vocab/slash_delimited.h>
#include <vocab/byte.h>
#include <vocab/byte_view.h>

#include <log_macros.h>

#include <memory>
#include <optional>
#include <cstddef>
#include <sstream>
#include <utility>
#include <string>
#include <string_view>

using namespace std::literals;

using Self = ipfs::gw::GatewayRequest;

auto Self::fromIpfsPath(ipfs::SlashDelimited ipfs_path) -> std::shared_ptr<Self> {
  auto name_space = ipfs_path.pop();
  auto r = std::make_shared<Self>();
  r->main_param = ipfs_path.pop();
  r->cid(Cid{r->main_param});
  if (name_space == "ipfs") {
    if (!r->cid().has_value()) {
      LOG(ERROR) << "IPFS request with invalid/unsupported CID "
                 << r->main_param;
      return {};
    }
    if (r->cid().value().hash_type() == HashType::IDENTITY) {
      r->type = GatewayRequestType::Identity;
    } else {
      r->path = ipfs_path.pop_all();
      r->type =
          r->path.empty() ? GatewayRequestType::Block : GatewayRequestType::Car;
    }
  } else if (name_space == "ipns") {
    r->path = ipfs_path.pop_all();
    if (Cid(r->main_param).valid()) {
      r->type = GatewayRequestType::Ipns;
    } else {
      r->type = GatewayRequestType::DnsLink;
    }
  } else {
    LOG(FATAL) << "Unsupported namespace in ipfs path: /" << name_space << '/'
               << ipfs_path.pop_all();
  }
  return r;
}

auto Self::url_suffix() const -> std::string {
  switch (type) {
    case GatewayRequestType::Block:
      return "/ipfs/" + main_param;
    case GatewayRequestType::Car:
      return "/ipfs/" + main_param + "/" + path +
             "?entity-bytes=0:" + std::to_string(2UL*CAR_RESPONSE_BUFFER_SIZE/3UL);
    case GatewayRequestType::Ipns:
      return "/ipns/" + main_param;
    case GatewayRequestType::Providers:
      return "/routing/v1/providers/" + main_param;
    case GatewayRequestType::DnsLink:
      return "/ipns/" + main_param;
    case GatewayRequestType::Identity:
    case GatewayRequestType::Zombie:
      return {};
  }
  LOG(FATAL) << "Unhandled gateway request type: " << static_cast<int>(type);
    #ifdef __clang__
    #pragma clang diagnostic ignored "-Wunreachable-code-return"
    #endif
  return {};
}
auto Self::accept() const -> std::string_view {
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
      return "application/vnd.ipld.raw"sv;
    case GatewayRequestType::Identity:
    case GatewayRequestType::Zombie:
      return {};
  }
  LOG(FATAL) << "Invalid gateway request type: " << static_cast<int>(type);
  return {};
}
auto Self::timeout_seconds() const -> short {
  switch (type) {
    case GatewayRequestType::DnsLink:
      return 33;
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
  auto hash = cid_.value().hash();
  char const* d = reinterpret_cast<char const*>(hash.data());
  return std::string_view{d, hash.size()};
}

auto Self::is_http() const -> bool {
  switch (type) {
    case GatewayRequestType::Ipns:
    case GatewayRequestType::Car:
    case GatewayRequestType::Block:
    case GatewayRequestType::Providers:
    case GatewayRequestType::DnsLink:
      return true;
    case GatewayRequestType::Identity:
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
auto Self::max_response_size() const -> std::optional<std::size_t> {
  switch (type) {
    case GatewayRequestType::Identity:
      return 0;
    case GatewayRequestType::DnsLink:
      return BLOCK_RESPONSE_BUFFER_SIZE;
    case GatewayRequestType::Ipns:
      return MAX_IPNS_PB_SERIALIZED_SIZE;
    case GatewayRequestType::Block:
      return BLOCK_RESPONSE_BUFFER_SIZE;
    case GatewayRequestType::Car: {
      return CAR_RESPONSE_BUFFER_SIZE;
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
auto Self::cachable() const -> bool {
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
auto Self::debug_string() const -> std::string {
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
auto Self::Key() const -> std::string {
  auto rv = main_param;
  //  rv.append(" ").append(name(type)).append(" ").append(path);
  return rv;
}
auto Self::root_component() const -> std::string_view {
    return main_param;
}
void Self::root_component(std::string_view val) {
    main_param.assign(val);
}
auto Self::RespondSuccessfully(std::string_view bytes,
                               std::shared_ptr<Client> const& api,
                               ipld::BlockSource src,
                               std::string_view roots,
                               bool* valid) -> bool {
  using namespace ipfs::ipld;
  bool success = false;
  if (valid != nullptr) {
    *valid = false;
  }
  FleshOut(src);
  switch (type) {
    case GatewayRequestType::Block: {
      AddBlock(bytes, success, src, api, valid);
    } break;
    case GatewayRequestType::Identity: {
      auto node = std::make_shared<Chunk>(std::string{bytes});
      if (node) {
        node->source(src);
      }
      success = orchestrator_->add_node(main_param, node);
      if (valid != nullptr) {
        *valid = true;
      }
    } break;
    case GatewayRequestType::Ipns:
      if (!IpnsResponse(as_bytes(bytes), api, success, valid, src)) {
        return false;
      }
      break;
    case GatewayRequestType::DnsLink: {
      Cid const c{roots};
      if (c.valid()) {
        VLOG(1) << debug_string() << " resolves to " << c.to_string();
        AddDnsLink("/ipfs/" + c.to_string(), success, src);
        cid(c);
        AddBlock(bytes, success, src, api, valid);
      } else {
        AddDnsLink(bytes, success, src);
      }
    } break;
    case GatewayRequestType::Car: {
      DCHECK(api);
      Car car(as_bytes(bytes), api->cbor());
      AddBlocks(car, api, success, valid, src);
      break;
    }
    case GatewayRequestType::Providers:
      success = providers::ProcessResponse(bytes, *api);
      if (valid != nullptr) {
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
auto Self::PartiallyRedundant() const -> bool {
  if (Finished()) {
    return true;
  }
  if (!orchestrator_) {
    return false;
  }
  return orchestrator_->has_key(main_param);
}
auto Self::Finished() const -> bool {
  if (type == GatewayRequestType::Zombie) {
    return true;
  }
  if (type == GatewayRequestType::Providers) {
    return false;
  }
  if (!dependent) {
    LOG(WARNING) << "Gateway request considered finished, because it has no "
                    "dependent request. Unusual case.";
    return true;
  }
  if (dependent->done()) {
    return true;
  }
  return false;
}
void Self::FleshOut(ipld::BlockSource& s) const {
  if (cid().has_value() && cid()->valid()) {
    s.cid = cid()->to_string();
  } else {
    s.cid = main_param;
  }
  s.cat.request_type = type;
}
void Self::AddDnsLink(std::string_view target, bool& success, ipld::BlockSource src) {
    auto node = std::make_shared<ipld::DnsLinkName>(target);
    if (node) {
        node->source(src);
    }
    if (orchestrator_) {
        success = orchestrator_->add_node(main_param, node) || success;
    } else {
        LOG(ERROR) << "I have no orchestrator!!";
    }
}
void Self::AddBlock(std::string_view bytes,
                    bool& success,
                    ipld::BlockSource src,
                    std::shared_ptr<Client> const& api,
                    bool* valid) {
  DCHECK(cid().has_value());
    if (!cid().has_value()) {
        LOG(ERROR) << "Your CID doesn't even have a value!";
        success = false;
        return;
    }
    DCHECK(api);
    auto node = ipld::DagNode::fromBytes(api, cid().value(), bytes);
    if (!node) {
        success = false;
    } else {
        node->source(src);
        if (valid != nullptr) {
            *valid = true;
        }
        success = orchestrator_->add_node(main_param, node) || success;
    }
}
void Self::AddBlocks(Car& car, const std::shared_ptr<Client>& api, bool& success, bool* valid, ipld::BlockSource src) {
  while (auto block = car.NextBlock()) {
    auto cid_s = block->cid.to_string();
    auto n = ipld::DagNode::fromBytes(api, block->cid, block->bytes);
    if (!n) {
      LOG(ERROR) << "Unable to handle block from CAR: " << cid_s;
      continue;
    }
    if (valid != nullptr) {
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
}
auto Self::IpnsResponse(ByteView bytes, std::shared_ptr<Client> const& api, bool& success, bool* valid, ipld::BlockSource src) -> bool {
  if (cid().has_value()) {
    DCHECK(api);
    auto rec = ipfs::ValidateIpnsRecord(bytes, cid().value(), *api);
    if (rec.has_value()) {
      ValidatedIpns const validated{rec.value()};
      auto node = std::make_shared<ipld::IpnsName>(validated);
      if (!node || node->expired()) {
        return false;
      }
      node->source(src);
      success = orchestrator_->add_node(main_param, node);
      if (valid != nullptr) {
        *valid = !node->expired();
      }
    } else {
      LOG(ERROR) << "IPNS record failed to validate!";
      return false;
    }
  }
  return true;
}

auto Self::cid() const -> std::optional<Cid> const& {
  return cid_;
}
void Self::cid(Cid cid) {
  if (cid.valid()) {
    cid_ = cid;
  } else {
    cid_ = {};
  }
}
