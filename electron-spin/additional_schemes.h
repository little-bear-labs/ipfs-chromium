#ifndef SPIN_ADDITIONAL_SCHEMES_H_
#define SPIN_ADDITIONAL_SCHEMES_H_

#include <array>
#include <string_view>

namespace electron_spin {
constexpr auto AdditionalSchemes() {
  using namespace std::literals;
  return std::array{"ipfs"sv, "ipns"sv};
}
}  // namespace electron_spin

#endif
