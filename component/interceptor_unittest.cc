#include "interceptor.h"

// Despite the Google C++ style guide deprecating the _unittest.cc suffix for
// unit test files, in Chromium we still use this suffix to distinguish unit
// tests from browser tests, which are written in files with the _browsertest.cc
// suffix.
// - https://chromium.googlesource.com/chromium/src/+/HEAD/styleguide/c++/c++.md

#include <gtest/gtest.h>

TEST(InterceptorTest, convert_paththess_ipfs_uri) {
  EXPECT_EQ(1, 2);
  GURL uri{
      "ipfs://bafybeifszd4wbkeekwzwitvgijrw6zkzijxutm4kdumkxnc6677drtslni"};
  auto actual = ipfs::Interceptor::TranslateUri(uri);
  std::string expected =
      "https://"
      "ipfs-over-http!.ipfs."
      "bafybeifszd4wbkeekwzwitvgijrw6zkzijxutm4kdumkxnc6677drtslni/";
  EXPECT_EQ(actual, expected);
}
