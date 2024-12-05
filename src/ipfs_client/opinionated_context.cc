#include <ipfs_client/opinionated_context.h>

#include <ipfs_client/gw/default_requestor.h>
#include <memory>
#include "ipfs_client/client.h"
#include "ipfs_client/crypto/signing_key_type.h"
#include "ipfs_client/ctx/boost_beast_http.h"
#include "ipfs_client/ctx/ares_dns_txt_lookup.h"
#include "ipfs_client/crypto/openssl_signature_verifier.h"
#include <openssl/evp.h>

#if HAS_OPINIONATED_CONTEXT

auto ipfs::start_default(boost::asio::io_context& io)
    -> std::shared_ptr<Client> {
  //    -> std::pair<std::shared_ptr<ContextApi>, std::shared_ptr<Partition>> {
  auto api = std::make_shared<Client>();
  using SKT = crypto::SigningKeyType;
  api->with(std::make_unique<ctx::BoostBeastHttp>(io))
      .with(std::make_unique<ctx::AresDnsTxtLookup>(&io))
      .with(SKT::RSA,
            std::make_unique<crypto::OpensslSignatureVerifier>(EVP_PKEY_RSA))
      .with(SKT::Ed25519, std::make_unique<crypto::OpensslSignatureVerifier>(
                              EVP_PKEY_ED25519))
      .with([](){return true;})
      ;
  auto rtor = gw::default_requestor({}, api);
  api->with(rtor);
  //  auto orc = std::make_shared<Partition>(rtor, api);
  //  return {api, orc};
  return api;
}

#endif  // HAS_OPINIONATED_CONTEXT
