#ifndef IPFS_MUTLICODEC_H_
#define IPFS_MUTLICODEC_H_ 1

#include <iosfwd>
#include <limits>
#include <string_view>

#include <cstdint>

namespace ipfs {
enum class MultiCodec : std::uint32_t {
  INVALID = 0xFFFF,
  IDENTITY = 0x00,
  RAW = 0x55,
  DAG_PB = 0x70,
  DAG_CBOR = 0x71,
  LIBP2P_KEY = 0x72,
  DAG_JSON = 0x0129,
};
MultiCodec Validate(MultiCodec);
std::string_view GetName(MultiCodec);
}  // namespace ipfs

namespace std {//glog can't find this operator in the global namespace
auto operator<<(ostream&, ipfs::MultiCodec) -> ostream&;
}
#endif  // IPFS_MUTLICODEC_H_
