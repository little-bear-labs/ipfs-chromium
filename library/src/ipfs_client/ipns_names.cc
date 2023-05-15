#include <ipfs_client/ipns_names.h>

#include <libp2p/multi/content_identifier_codec.hpp>
#include "log_macros.h"

using Self = ipfs::IpnsNames;

void Self::NoSuchName(std::string const& name) {
  names_[name];  // If it already exists, leave it.
}
void Self::AssignName(std::string const& name, ValidatedIpns entry) {
  auto& res = entry.value;
  if (res.size() && res.front() == '/') {
    res.erase(0, 1);
  }
  auto endofcid = res.find_first_of("/?#", 6);
  using namespace libp2p::multi;
  auto cid_str = res.substr(5, endofcid);
  LOG(INFO) << "IPNS points to CID " << cid_str;
  auto dec_res = ContentIdentifierCodec::fromString(cid_str);
  if (dec_res.has_value()) {
    auto cid = dec_res.value();
    if (dec_res.value().version == ContentIdentifier::Version::V0) {
      // TODO - implement ipns properly. A peer ID could actually fail this
      // check, I believe.
      DCHECK_EQ(res.substr(0, 5), "ipfs/");
      cid =
          ContentIdentifier(ContentIdentifier::Version::V1,
                            MulticodecType::Code::DAG_PB, cid.content_address);
    }
    auto enc_res = ContentIdentifierCodec::toStringOfBase(
        cid, MultibaseCodec::Encoding::BASE32_LOWER);
    DCHECK(enc_res.has_value());
    auto desensitized = res.substr(0, 5);
    desensitized.append(enc_res.value());
    if (endofcid < res.size()) {
      auto extra = res.substr(endofcid);
      LOG(INFO) << name << " resolution contains oddity '" << extra;
      desensitized.append(extra);
    }
    LOG(INFO) << name << " now resolves to (desensitized)" << desensitized;
    entry.value = desensitized;
  } else {
    LOG(INFO) << name << " now resolves to (extra level)" << res;
  }
  auto it = names_.find(name);
  if (it == names_.end()) {
    names_.emplace(name, std::move(entry));
  } else if (it->second.sequence < entry.sequence) {
    LOG(INFO) << "Updating IPNS record for " << name << " from sequence "
              << it->second.sequence << " where it pointed to "
              << it->second.value << " to sequence " << entry.sequence
              << " where it points to " << entry.value;
    it->second = entry;
  } else {
    LOG(INFO) << "Discarding redundant IPNS record for " << name;
  }
}
void Self::AssignDnsLink(std::string const& name, std::string_view target) {
  ValidatedIpns v;
  v.value.assign(target);
  auto t = std::time(nullptr);
  v.use_until = v.cache_until = t + 300;
  AssignName(name, std::move(v));
}

std::string_view Self::NameResolvedTo(std::string_view original_name) const {
  std::string name{original_name};
  std::string_view prev = "";
  auto trailer = names_.end();
  auto trail_step = false;
  auto now = std::time(nullptr);
  while (true) {
    auto it = names_.find(name);
    if (names_.end() == it) {
      LOG(WARNING) << "Host not in immediate access map: " << name << " ("
                   << std::string{original_name} << ')';
      return prev;
    } else if (it == trailer) {
      LOG(ERROR) << "Host cycle found in IPNS: " << std::string{original_name}
                 << ' ' << name;
      return "";
    }
    auto& target = it->second.value;
    if (target.empty()) {
      return kNoSuchName;
    }
    if (target.at(2) == 'f') {
      return target;
    }
    if (it->second.use_until < now) {
      return prev;
    }
    if (trail_step) {
      if (trailer == names_.end()) {
        trailer = names_.find(name);
      } else {
        trailer = names_.find(trailer->second.value.substr(5));
      }
    }
    trail_step = !trail_step;
    prev = it->second.value;
    name.assign(prev, 5);
  }
}
auto Self::Entry(std::string const& name) -> ValidatedIpns const* {
  auto it = names_.find(name);
  return it == names_.end() ? nullptr : &(it->second);
}

Self::IpnsNames() {}
Self::~IpnsNames() {}
