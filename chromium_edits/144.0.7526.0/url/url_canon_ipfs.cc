#include "url_canon_internal.h"

#include <ipfs_client/cid.h>
#include <ipfs_client/identity_cid.h>
#include <base/logging.h>

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
  LOG(ERROR) << "stdurl starts as '" << stdurl
             << "' before I append '" << as_str
             << "' while the CID string was '" << cid_str
             << "' parsed.host: len=" << parsed.host.len << " begin=" << parsed.host.begin
             << " parsed.host.end=" << parsed.host.end();
  stdurl.append( as_str );
  spec.remove_prefix(parsed.host.end());
  stdurl.append(spec);
  auto len = static_cast<int>(stdurl.size());
  Parsed parsed_input;
  ParseStandardURL(stdurl.data(), len, &parsed_input);
  return CanonicalizeStandardUrl(
      stdurl.data(),
      parsed_input,
      scheme_type,
      charset_converter,
      output, output_parsed
    );
}
namespace {

using from_view = bool (*)(std::u16string_view, url::CanonOutput*);
using from_ptr = bool (*)(char16_t const*, std::size_t, url::CanonOutput*);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
void do_convert(from_view foo, std::u16string_view input, url::CanonOutput* output) {
  foo(input, output);
}
void do_convert(from_ptr foo, std::u16string_view input, url::CanonOutput* output) {
  foo(input.data(), input.size(), output);
}
#pragma clang diagnostic pop

}
bool url::CanonicalizeIpfsURL(std::basic_string_view<char16_t> spec,
                              const Parsed& parsed,
                              SchemeType scheme_type,
                              CharsetConverter* query_converter,
                              CanonOutput* output,
                              Parsed* new_parsed) {
  RawCanonOutput<2048> as8;
  do_convert(&ConvertUTF16ToUTF8, spec, &as8);
  return CanonicalizeIpfsURL({as8.data(), as8.length()}, parsed, scheme_type,
                             query_converter, output, new_parsed);
}
