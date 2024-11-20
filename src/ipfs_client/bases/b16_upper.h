#ifndef IPFS_B16_UPPER_H_
#define IPFS_B16_UPPER_H_

#include <multibase/basic_algorithm.h>

namespace multibase {

/*! Upper-case base-16 encoding
 */
template <>
struct traits<::multibase::encoding::base_16_upper> {
  constexpr static const std::array<char, 16> charset = {
      '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  constexpr static const char name[] = "BASE_16";
  using execution_style = multibase::algorithm::block_tag;
  constexpr static const char padding = 0;
};
}  // namespace multibase

namespace ipfs::mb {
using base_16_upper =
    multibase::basic_algorithm<multibase::encoding::base_16_upper>;
}  // namespace ipfs::mb

#endif  // IPFS_B16_UPPER_H_
