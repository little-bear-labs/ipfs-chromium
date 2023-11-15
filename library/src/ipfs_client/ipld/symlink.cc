#include "symlink.h"

#include <libp2p/multi/content_identifier_codec.hpp>

#include "log_macros.h"

using Self = ipfs::ipld::Symlink;

Self::Symlink(std::string target)
    : style_{from_target(target)}, target_{target} {}

Self::~Symlink() {}

auto Self::resolve(SlashDelimited path, BlockLookup, std::string& to_here)
    -> ResolveResult {
  std::string result;
  switch (style_) {
    case Style::Absolute:
      result.assign(target_);
      break;
    case Style::Relative: {
      auto c = to_here.find_last_not_of('/');
      c = to_here.find_last_of('/', c);
      DCHECK(c != to_here.size()) << to_here;
      result.assign(to_here, 0, c + 1).append(target_);
    } break;
    case Style::FromRoot:
      result.assign(SlashDelimited{to_here}.pop_n(2))
          .append("/")
          .append(target_);
  }
  if (path) {
    result.append("/").append(path.pop_all());
  }
  std::size_t i;
  while ((i = result.find("//")) != std::string::npos) {
    result.erase(i, 1);
  }
  DCHECK_GT(result.size(), 0U);
  if (result.back() == '/') {
    result.resize(result.size() - 1);
  }
  return PathChange{result};
}

auto Self::from_target(std::string const& target) -> Style {
  DCHECK_GT(target.size(), 0U);
  if (target.at(0) != '/') {
    return Style::Relative;
  }
  SlashDelimited t{target};
  auto ns = t.pop();
  if (ns != "ipfs" && ns != "ipns") {
    return Style::FromRoot;
  }
  auto root = t.pop();
  using namespace libp2p::multi;
  auto cid = ContentIdentifierCodec::fromString(root);
  if (!cid.has_value()) {
    return Style::FromRoot;
  }
  if (cid.value().content_type == MulticodecType::Code::LIBP2P_KEY) {
    return ns == "ipns" ? Style::Absolute : Style::FromRoot;
  } else {
    return ns == "ipfs" ? Style::Absolute : Style::FromRoot;
  }
}
