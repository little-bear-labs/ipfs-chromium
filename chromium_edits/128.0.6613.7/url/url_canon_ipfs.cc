#include "url_canon_internal.h"

#include <ipfs_client/cid.h>
#include <ipfs_client/identity_cid.h>

#include <sstream>

bool url::CanonicalizeIpfsURL(std::string_view spec,
                              const Parsed& parsed,
                              SchemeType scheme_type,
                              CharsetConverter* charset_converter,
                              CanonOutput* output,
                              Parsed* output_parsed) {
  if (spec.size() < 1) {
    return false;
  }
  if ( parsed.host.len < 1 ) {
    return false;
  }
  auto cid_str = spec.substr( parsed.host.begin, static_cast<std::size_t>(parsed.host.len) );
  auto cid = ipfs::Cid(cid_str);
  if ( !cid.valid() ) {
    cid = ipfs::id_cid::forText( std::string{cid_str} + " is not a valid CID." );
  }
  auto as_str = cid.to_string();
  if ( as_str.empty() ) {
    return false;
  }
  std::string stdurl{ spec.substr(0UL, static_cast<std::size_t>(parsed.host.begin)) };
  stdurl.append( as_str );
  spec.remove_prefix(parsed.host.end());
  stdurl.append(spec);
  auto len = static_cast<int>(stdurl.size());
  Parsed parsed_input;
  ParseStandardURL(stdurl.data(), len, &parsed_input);
  return CanonicalizeStandardURL(
      stdurl.data(),
      parsed_input,
      scheme_type,
      charset_converter,
      output, output_parsed
    );
}
bool url::CanonicalizeIpfsURL(std::basic_string_view<char16_t> spec,
                              const Parsed& parsed,
                              SchemeType scheme_type,
                              CharsetConverter* query_converter,
                              CanonOutput* output,
                              Parsed* new_parsed) {
  RawCanonOutput<2048> as8;
  ConvertUTF16ToUTF8(spec.data(), spec.size(), &as8);
  return CanonicalizeIpfsURL({as8.data(), as8.length()}, parsed, scheme_type,
                             query_converter, output, new_parsed);
}
