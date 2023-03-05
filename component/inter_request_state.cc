#include "inter_request_state.h"

#include "base/logging.h"
#include "content/public/browser/browser_context.h"

#include <vocab/log_macros.h>
#include <libp2p/multi/content_identifier_codec.hpp>

namespace {
constexpr char user_data_key[] = "ipfs_request_userdata";
}

auto ipfs::InterRequestState::FromBrowserContext(
    content::BrowserContext* context) -> InterRequestState& {
  if (!context) {
    LOG(WARNING) << "No browser context! Using a default IPFS state.";
    static ipfs::InterRequestState static_state;
    return static_state;
  }
  base::SupportsUserData::Data* existing = context->GetUserData(user_data_key);
  if (existing) {
    LOG(INFO) << "Re-using existing IPFS state.";
    return *static_cast<ipfs::InterRequestState*>(existing);
  }
  LOG(INFO) << "Creating new IPFS state for this browser context.";
  auto owned = std::make_unique<ipfs::InterRequestState>();
  ipfs::InterRequestState* raw = owned.get();
  context->SetUserData(user_data_key, std::move(owned));
  return *raw;
}

std::string_view ipfs::InterRequestState::NameResolvedTo(
    std::string const& name) const {
  auto it = names_.find(name);
  if (names_.end() == it) {
    return "";
  }
  return it->second;
}
void ipfs::InterRequestState::AssignName(std::string const& name,
                                         std::string res) {
  auto endofcid = res.find_first_of("/?#", 6);
  using namespace libp2p::multi;
  auto dec_res = ContentIdentifierCodec::fromString(res.substr(5, endofcid));
  if (dec_res.has_value()) {
    auto cid = dec_res.value();
    if (dec_res.value().version == ContentIdentifier::Version::V0) {
      // TODO - implement ipns properly. A peer ID could actually fail this
      // check, I believe.
      GOOGLE_DCHECK_EQ(res.substr(0, 5), "ipfs/");
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
      L_INF(name << " resolution contains oddity '" << extra);
      desensitized.append(extra);
    }
    L_INF(name << " now resolves to (desensitized)" << desensitized);
    names_[name] = std::move(desensitized);
  } else {
    L_INF(name << " now resolves to (weirdness)" << res);
    names_[name] = std::move(res);
  }
}

ipfs::InterRequestState::InterRequestState() {}
ipfs::InterRequestState::~InterRequestState() noexcept {}