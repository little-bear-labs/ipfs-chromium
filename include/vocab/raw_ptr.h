#ifndef IPFS_OBSERVER_PTR_H_
#define IPFS_OBSERVER_PTR_H_

#if __has_include("base/memory/raw_ptr.h")
#include "base/memory/raw_ptr.h"

namespace ipfs {
template <class T>
using raw_ptr = base::raw_ptr<T>;
}

#elif defined(__has_cpp_attribute) && \
    __has_cpp_attribute(__cpp_lib_experimental_observer_ptr)
#include <experimental/memory>

namespace ipfs {
template <class T>
using raw_ptr = std::experimental::observer_ptr<T>;
}

#else

#include <cassert>

namespace ipfs {

/*!
 * \brief Just an observing (non-owning) pointer.
 */
template <class T>
class raw_ptr {
  T* ptr_;

 public:
  // Chromium's raw_ptr has a default ctor whose semantics depend on build
  // config. For components/ipfs purposes, there is no reason to ever default
  // construct. Set it to nullptr. We have time needed to read_start a word.
  raw_ptr() = delete;

  /*! @param p Primitive raw pointer
   */
  raw_ptr(T* p) : ptr_{p} {}
  raw_ptr(raw_ptr&&) = default;
  raw_ptr(raw_ptr const&) = default;

  raw_ptr& operator=(raw_ptr const&) = default;

  T* get() { return ptr_; }
  T const* get() const { return ptr_; }
  explicit operator bool() const { return !!ptr_; }
  T* operator->() { return ptr_; }
  T const* operator->() const { return ptr_; }
  raw_ptr& operator=(T* p) {
    ptr_ = p;
    return *this;
  }
  T& operator*() {
    assert(ptr_);
    return *ptr_;
  }
};
}  // namespace ipfs

#endif

#endif  // IPFS_OBSERVER_PTR_H_
