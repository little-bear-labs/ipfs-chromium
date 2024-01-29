#ifndef IPFS_CHROMIUM_CBOR_PARSER_H
#define IPFS_CHROMIUM_CBOR_PARSER_H

#include <vocab/byte_view.h>

#include <memory>

namespace ipfs {
class DagCborValue;
}

namespace ipfs::ctx {
class CborParser {
 public:
  virtual std::unique_ptr<DagCborValue> Parse(ByteView) = 0;
  virtual ~CborParser() noexcept {}
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_CBOR_PARSER_H
