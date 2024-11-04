#include <ipfs_client/ipld/dag_headers.h>

#include <chrono>
#include <sstream>
#include <string_view>
#include <string>

#include "ipfs_client/gw/gateway_request_type.h"
#include "ipfs_client/ipld/block_source.h"

using Self = ipfs::ipld::DagHeaders;
using ReqTyp = ipfs::gw::GatewayRequestType;
namespace c = std::chrono;

namespace {
auto describe(ReqTyp typ) -> std::string_view {
  switch (typ) {
    case ReqTyp::DnsLink:
      return "Resolving a DNSLink via DNS TXT record";
    case ReqTyp::Ipns:
      return "Resolving a verified IPNS record to the value it points to";
    case ReqTyp::Car:
      return "Content-addressable ARchive - N immutable blocks in one request";
    case ReqTyp::Identity:
      return "Inlined immutable data";
    case ReqTyp::Block:
      return "Immutable data";
    case ReqTyp::Providers:
    case ReqTyp::Zombie:
      return "ERROR: This request type should not show up here!";
  }
  return "ERROR: Unhandled request type";
}
}  // namespace
void Self::Add(BlockSource const& src) {
  auto& cnt = individual_counts_[src.cat];
  if (cnt >= 3) {
    ++other_count_;
    other_sum_ += src.load_duration;
  } else {
    ++cnt;
    std::ostringstream value;
    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Server-Timing
    value << name(src.cat.request_type);
    if (src.cat.cached) {
      value << "-cache";
    }
    value << "-_" << src.cid;
    value << ";desc=\"" << describe(src.cat.request_type);
    if (src.cat.cached) {
      value << " (from cache)";
    }
    value << "\";dur="
          << c::duration_cast<c::milliseconds>(src.load_duration).count();
    headers_.emplace_back("Server-Timing", value.str());
    if (static_cast<unsigned int>(!src.cid.empty()) != 0U) {
      auto from = src.cat.cached ? std::string{"cache"} : src.cat.gateway_url;
      headers_.emplace_back("IPFS-Source-" + src.cid, from);
    }
  }
}
void Self::Finish() {
  if (other_count_ == 0U) {
    return;
  }
  std::ostringstream value;
  value << "other-blocks;desc=\"Time used to fetch " << other_count_
        << " unlisted blocks.\";dur="
        << c::duration_cast<c::milliseconds>(other_sum_).count();
  headers_.emplace_back("Server-Timing", value.str());
}
