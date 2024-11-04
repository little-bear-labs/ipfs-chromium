#ifndef IPFS_CHROMIUM_CBOR_PARSER_H
#define IPFS_CHROMIUM_CBOR_PARSER_H

#include <vocab/byte_view.h>

#include <memory>

namespace ipfs {
class DagCborValue;
}

namespace ipfs::ctx {
/*! A base class for  your way of parsing CBOR
 */
class CborParser {
 public:
  /*! @param cbor The CBOR to parse
   *  @return The DOM value, or NULL if there was an error
   */
  virtual std::unique_ptr<DagCborValue> Parse(ByteView cbor) = 0;
  virtual ~CborParser() noexcept {}
};
}  // namespace ipfs::ctx

#endif  // IPFS_CHROMIUM_CBOR_PARSER_H
