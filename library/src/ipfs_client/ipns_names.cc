#include <ipfs_client/ipns_names.h>

#include <ipfs_client/cid.h>
#include <string>
#include <utility>
#include <string_view>
#include <ctime>

#include "ipfs_client/ipns_record.h"
#include "log_macros.h"

using namespace std::literals;
using Self = ipfs::IpnsNames;

void Self::NoSuchName(std::string const& name) {
  names_[name];  // If it already exists, leave it.
}
void Self::AssignName(std::string const& name, ValidatedIpns rec) {
  auto& res = rec.value;
  if ((static_cast<unsigned int>(!res.empty()) != 0U) && res.front() == '/') {
    res.erase(0, 1);
  }
  auto endofcid = res.find_first_of("/?#", "/ipns/"sv.size());
  using namespace libp2p::multi;
  auto cid_str = res.substr(5, endofcid);
  LOG(INFO) << "IPNS points to CID " << cid_str;
  auto cid = Cid(cid_str);
  if (cid.valid()) {
    auto desensitized = res.substr(0, 5);
    desensitized.append(cid_str);
    if (endofcid < res.size()) {
      auto extra = res.substr(endofcid);
      LOG(INFO) << name << " resolution contains oddity '" << extra;
      desensitized.append(extra);
    }
    LOG(INFO) << name << " now resolves to (desensitized)" << desensitized;
    rec.value = desensitized;
  } else {
    LOG(INFO) << name << " now resolves to (extra level)" << res;
  }
  auto nam_it = names_.find(name);
  if (nam_it == names_.end()) {
    names_.emplace(name, std::move(rec));
  } else if (nam_it->second.sequence < rec.sequence) {
    LOG(INFO) << "Updating IPNS record for " << name << " from sequence "
              << nam_it->second.sequence << " where it pointed to "
              << nam_it->second.value << " to sequence " << rec.sequence
              << " where it points to " << rec.value;
    nam_it->second = std::move(rec);
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

auto Self::NameResolvedTo(std::string_view original_name) const -> std::string_view {
  std::string name{original_name};
  std::string_view prev;
  auto trailer = names_.end();
  auto trail_step = false;
  auto now = std::time(nullptr);
  while (true) {
    auto it = names_.find(name);
    if (names_.end() == it) {
      LOG(INFO) << "Host not in immediate access map: " << name << " ("
                << std::string{original_name} << ')';
      return prev;
    } else if (it == trailer) {
      LOG(ERROR) << "Host cycle found in IPNS: " << std::string{original_name}
                 << ' ' << name;
      return "";
    }
    const auto& target = it->second.value;
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

Self::IpnsNames() = default;
Self::~IpnsNames() = default;
