#include "ipfs_client/response.h"

using Self = ipfs::Response;

Self Self::PLAIN_NOT_FOUND{std::string{}, static_cast<std::uint16_t>(404),
                           std::string{}, std::string{}};
Self Self::IMMUTABLY_GONE{"text/plain", 410,
                          "Using immutable data it has been proven the "
                          "resource does not exist anywhere.",
                          std::string{}};

Self Self::HOST_NOT_FOUND{
    "text/plain", Self::HOST_NOT_FOUND_STATUS,
    "either a hostname didn't resolve a DNS TXT records for dnslink=, or we "
    "can't find a gateway with the necessary IPNS record",
    std::string{}};
