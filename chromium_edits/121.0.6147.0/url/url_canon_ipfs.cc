#include "url_canon_internal.h"

#include <ipfs_client/identity_cid.h>
#include <libp2p/multi/content_identifier_codec.hpp>

#include <sstream>

namespace m = libp2p::multi;
using Cid = m::ContentIdentifier;
using CidCodec = m::ContentIdentifierCodec;

bool url::CanonicalizeIpfsURL(const char* spec,
                              int spec_len,
                              const Parsed& parsed,
                              SchemeType scheme_type,
                              CharsetConverter* charset_converter,
                              CanonOutput* output,
                              Parsed* output_parsed) {
  if (spec_len < 1 || !spec) {
    return false;
  }
  if (parsed.host.len < 1) {
    return false;
  }
  std::string cid_str{spec + parsed.host.begin,
                      static_cast<std::size_t>(parsed.host.len)};
  auto maybe_cid = CidCodec::fromString(cid_str);
  if (!maybe_cid.has_value()) {
    auto e = libp2p::multi::Stringify(maybe_cid.error());
    std::ostringstream err;
    err << e << ' '
        << std::string_view{spec, static_cast<std::size_t>(spec_len)};
    maybe_cid = ipfs::id_cid::forText(err.str());
  }
  auto cid = maybe_cid.value();
  if (cid.version == Cid::Version::V0) {
    cid = Cid{Cid::Version::V1, cid.content_type, cid.content_address};
  }
  auto as_str = CidCodec::toString(cid);
  if (!as_str.has_value()) {
    return false;
  }
  std::string stdurl{spec, static_cast<std::size_t>(parsed.host.begin)};
  stdurl.append(as_str.value());
  stdurl.append(spec + parsed.host.end(), spec_len - parsed.host.end());
  spec = stdurl.data();
  spec_len = static_cast<int>(stdurl.size());
  Parsed parsed_input;
  ParseStandardURL(spec, spec_len, &parsed_input);
  return CanonicalizeStandardURL(spec, spec_len, parsed_input, scheme_type,
                                 charset_converter, output, output_parsed);
}
bool url::CanonicalizeIpfsURL(const char16_t* spec,
                              int spec_len,
                              const Parsed& parsed,
                              SchemeType scheme_type,
                              CharsetConverter* query_converter,
                              CanonOutput* output,
                              Parsed* new_parsed) {
  RawCanonOutput<2048> as8;
  ConvertUTF16ToUTF8(spec, spec_len, &as8);
  return CanonicalizeIpfsURL(as8.data(), as8.length(), parsed, scheme_type,
                             query_converter, output, new_parsed);
}
