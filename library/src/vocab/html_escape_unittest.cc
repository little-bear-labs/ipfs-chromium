#include <vocab/html_escape.h>

#include <gtest/gtest.h>

using namespace std::literals;

TEST(HTMLEscapeTest, CommonForbiddenCharacters) {
  std::string actual;
  for (auto c : "<a href='favicon.ico' class=\"icon\">&</a>"sv) {
    actual.append(html_escape(c));
  }
  auto expected =
      "&lt;a href=&apos;favicon.ico&apos; "
      "class=&quot;icon&quot;&gt;&amp;&lt;/a&gt;";
  EXPECT_EQ(expected, actual);
}
