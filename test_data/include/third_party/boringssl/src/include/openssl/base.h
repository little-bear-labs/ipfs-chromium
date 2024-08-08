#pragma once

#include <openssl/ossl_typ.h>
#include <openssl/md5.h>

#include <memory>

#undef OPENSSL_EXPORT
#define OPENSSL_EXPORT
#define BSSL_CHECK(X)

typedef struct crypto_buffer_st CRYPTO_BUFFER;

namespace bssl {
  template<class T>
  using UniquePtr = std::unique_ptr<T>;

}

#include "third_party/boringssl/src/pki/input.h"
