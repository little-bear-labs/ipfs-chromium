#pragma once

#include "cbor_parser.h"

namespace ipfs::ctx {
/*! A non-implementation
 */
class NullCborParser : public CborParser {
 public:
  /*! @return NULL, always
   */
  std::unique_ptr<DagCborValue> Parse(ByteView) override {
    return {};
  }
  ~NullCborParser() noexcept override = default;
};
}  // namespace ipfs::ctx
