#include "ipfs_client/response.h"

using Self = ipfs::Response;

Self Self::PLAIN_NOT_FOUND{std::string{}, static_cast<std::uint16_t>(404),
                           std::string{}, std::string{}};
