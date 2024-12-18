#include "ipfs_client/response.h"
#include <string>
#include <string_view>

using Self = ipfs::Response;

Self Self::PLAIN_NOT_FOUND{"text/html", 502, std::string{}, std::string{}, {}};
Self Self::IMMUTABLY_GONE{"text/plain", 404,
                          "Using immutable data it has been proven the "
                          "resource does not exist anywhere.",
                          std::string{},
                          {}};

Self Self::HOST_NOT_FOUND_RESPONSE{
    "text/plain", 503,
    "either a hostname didn't resolve a DNS TXT records for dnslink=, or we "
    "can't find a gateway with the necessary IPNS record",
    std::string{},
    {}};

auto Self::html(std::string_view body, std::string_view location) -> Self {
  return {"text/html", 200, std::string{body}, std::string{location}, {}};
}
