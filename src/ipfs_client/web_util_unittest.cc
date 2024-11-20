#include "web_util.h"
#include <gtest/gtest.h>

namespace u = ipfs::util;

TEST(RoughlyUnescapeUrlComponent,SomeStolenTestCases) {
  // https://greenbytes.de/tech/webdav/urldecomp.html
  auto actual = u::RoughlyUnescapeUrlComponent(",A%20brief%20note%25foo#bar");
  EXPECT_EQ(actual, ",A brief note%foo#bar");
  actual = u::RoughlyUnescapeUrlComponent("a-umlaut-%C3%A4");
  EXPECT_EQ(actual, "a-umlaut-ä");
  actual = u::RoughlyUnescapeUrlComponent("a%");
  EXPECT_EQ(actual, "a%");
}
