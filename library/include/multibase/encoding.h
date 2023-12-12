#pragma once
#include <istream>
#include <ostream>

namespace multibase {

enum class encoding : unsigned char {
  base_unknown = '?',
  base_256 = 0,
  base_16 = 'f',
  base_16_upper = 'F',
  base_32 = 'b',
  base_32_upper = 'B',
  base_36 = 'k',
  base_58_btc = 'Z',
  base_64 = 'm',
  base_64_pad = 'M'

};

}  // namespace multibase
