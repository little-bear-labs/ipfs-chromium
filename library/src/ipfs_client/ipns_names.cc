#include <ipfs_client/ipns_names.h>

#include <vocab/log_macros.h>
#include <libp2p/multi/content_identifier_codec.hpp>

void ipfs::IpnsNames::AssignName(std::string const& name, std::string res) {
  auto endofcid = res.find_first_of("/?#", 6);
  using namespace libp2p::multi;
  auto dec_res = ContentIdentifierCodec::fromString(res.substr(5, endofcid));
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
    names_[name] = std::move(desensitized);
  } else {
    LOG(INFO) << name << " now resolves to (extra DNSLink)" << res;
    names_[name] = std::move(res);
  }
}

std::string_view ipfs::IpnsNames::NameResolvedTo(
    std::string_view original_name) const {
  std::string name{original_name};
  std::string_view prev = "";
  auto trailer = names_.end();
  auto trail_step = false;
  while (true) {
    auto it = names_.find(name);
    if (names_.end() == it) {
      LOG(WARNING) << "Host not already cached: " << name << " ("
                   << std::string{original_name} << ')';
      return prev;
    } else if (it == trailer) {
      LOG(ERROR) << "Host cycle found in IPNS: " << std::string{original_name}
                 << ' ' << name;
      return "";
    }
    if (it->second.at(2) == 'f') {
      return it->second;
    }
    if (trail_step) {
      if (trailer == names_.end()) {
        trailer = names_.find(name);
      } else {
        trailer = names_.find(trailer->second.substr(5));
      }
    }
    trail_step = !trail_step;
    prev = it->second;
    name.assign(prev, 5);
  }
}

ipfs::IpnsNames::IpnsNames() {}
ipfs::IpnsNames::~IpnsNames() {}