#include "url_canon_internal.h"

#include <ipfs_client/cid.h>
#include <ipfs_client/identity_cid.h>

#include <sstream>

bool url::CanonicalizeIpfsURL(const char* spec,
                             int spec_len,
                             const Parsed& parsed,
                             SchemeType scheme_type,
                             CharsetConverter* charset_converter,
                             CanonOutput* output,
                             Parsed* output_parsed) {
  if ( spec_len < 1 || !spec ) {
    return false;
  }
  if ( parsed.host.len < 1 ) {
    return false;
  }
  std::string_view cid_str{ spec + parsed.host.begin, static_cast<std::size_t>(parsed.host.len) };
  auto cid = ipfs::Cid(cid_str);
  if ( !cid.valid() ) {
    cid = ipfs::id_cid::forText( std::string{cid_str} + " is not a valid CID." );
  }
  auto as_str = cid.to_string();
  if ( as_str.empty() ) {
    return false;
  }
  std::string stdurl{ spec, static_cast<std::size_t>(parsed.host.begin) };
  stdurl.append( as_str );
  stdurl.append( spec + parsed.host.end(), spec_len - parsed.host.end() );
  spec = stdurl.data();
  spec_len = static_cast<int>(stdurl.size());
  Parsed parsed_input;
  ParseStandardURL(spec, spec_len, &parsed_input);
  return CanonicalizeStandardURL(
      spec,
      spec_len,
      parsed_input,
      scheme_type,
      charset_converter,
      output, output_parsed
    );
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
  return CanonicalizeIpfsURL(as8.data(), as8.length(), parsed, scheme_type, query_converter, output, new_parsed);
}
