#ifndef IPFS_B32_UPPER_H_
#define IPFS_B32_UPPER_H_

#include <multibase/basic_algorithm.h>

namespace multibase {
template <>
struct traits<::multibase::encoding::base_32> {
  constexpr static const std::array<char, 32> charset = {
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
      'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
      'w', 'x', 'y', 'z', '2', '3', '4', '5', '6', '7'};
  constexpr static const char name[] = "base_32";
  using execution_style = multibase::algorithm::block_tag;
  constexpr static const char padding = 0;
};
template <>
struct traits<::multibase::encoding::base_32_upper> {
  constexpr static const std::array<char, 32> charset = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
      'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
      'W', 'X', 'Y', 'Z', '2', '3', '4', '5', '6', '7'};
  constexpr static const char name[] = "base_32_upper";
  using execution_style = multibase::algorithm::block_tag;
  constexpr static const char padding = 0;
};
}  // namespace multibase

namespace ipfs::mb {
using base_32 = multibase::basic_algorithm<multibase::encoding::base_32>;
using base_32_upper =
    multibase::basic_algorithm<multibase::encoding::base_32_upper>;
}  // namespace ipfs::mb

#endif  // IPFS_B32_UPPER_H_
