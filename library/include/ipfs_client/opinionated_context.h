#ifndef IPFS_OPINIONATED_CONTEXT_H_
#define IPFS_OPINIONATED_CONTEXT_H_

#include <ipfs_client/crypto/openssl_signature_verifier.h>
#include <ipfs_client/ctx/ares_dns_txt_lookup.h>
#include <ipfs_client/ctx/boost_beast_http.h>

#include <ipfs_client/client.h>
#include <ipfs_client/partition.h>

#include <memory>

#if HAS_BOOST_BEAST && HAS_ARES && HAS_OPENSSL_EVP

#define HAS_OPINIONATED_CONTEXT 1

#include <google/protobuf/stubs/logging.h>

namespace boost::asio {
class io_context;
}

namespace google::protobuf {
constexpr LogLevel LOGLEVEL_DEBUG = static_cast<LogLevel>(-1);
constexpr LogLevel LOGLEVEL_TRACE = static_cast<LogLevel>(-2);
}  // namespace google::protobuf

class HttpSession;

namespace ipfs {

// std::pair<std::shared_ptr<ContextApi>, std::shared_ptr<Partition>>
std::shared_ptr<Client> start_default(boost::asio::io_context& io);

}  // namespace ipfs

#elif defined(_MSC_VER)
#pragma message("Not ported to MSVC")
#else
#warning \
    "You are missing dependencies necessary for using the opinionated context."
#endif  // HAS_BOOST_BEAST && HAS_ARES && HAS_OPENSSL_EVP

#endif  // IPFS_OPINIONATED_CONTEXT_H_
