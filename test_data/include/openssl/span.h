#pragma once

#include <span>

namespace bssl {
  template<class T>
  using Span = std::span<T>;
  template <typename T>
  constexpr Span<const T> MakeConstSpan(T *ptr, size_t size) {
    return Span<const T>(ptr, size);
  }
}
