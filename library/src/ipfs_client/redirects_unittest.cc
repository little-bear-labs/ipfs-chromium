#include <ipfs_client/redirects.h>

#include <gtest/gtest.h>

#include <ipfs_client/logger.h>

namespace r = ipfs::redirects;
using namespace std::literals;

namespace {
struct RedirectsTest : public ::testing::Test {
  RedirectsTest() { ipfs::log::SetLevel(ipfs::log::Level::OFF); }
  // Sample files for various test cases can be found in QmQyqMY5vUBSbSxyitJqthgwZunCQjDVtNd8ggVCxzuPQ4. Implementations should use it for internal testing.
  r::File sample_file_{
      R"SAMPLE(
          /redirect-one /one.html
          /301-redirect-one /one.html 301
          /302-redirect-two /two.html 302
          /200-index /index.html 200
          /posts/:year/:month/:day/:title /articles/:year/:month/:day/:title 301
          /splat/* /redirected-splat/:splat 301
          /not-found/* /404.html 404
          /gone/* /410.html 410
          /unavail/* /451.html 451
          /* /index.html 200
      )SAMPLE"}
    ;
  r::File no_catchall_{
      R"SAMPLE(
          /redirect-one /one.html
          /301-redirect-one /one.html 301
          /302-redirect-two /two.html 302
          /200-index /index.html 200
          /posts/:year/:month/:day/:title /articles/:year/:month/:day/:title 301
          /splat/* /redirected-splat/:splat 301
          /not-found/* /404.html 404
          /gone/* /410.html 410
          /unavail/* /451.html 451
      )SAMPLE"}
  ;
  r::File ipfs_tech_{
      R"REAL(
        /docs https://docs.ipfs.tech/
        /docs/* https://docs.ipfs.tech/
        /blog https://blog.ipfs.tech/
        /blog/* https://blog.ipfs.tech/:splat
        /companion-privacy https://github.com/ipfs/ipfs-companion/blob/main/PRIVACY-POLICY.md
        /privacy https://protocol.ai/legal/#privacy-policy
        /ipfs /not-a-gateway-404.html 404
        /ipfs/* /not-a-gateway-404.html 404
        /ipns /not-a-gateway-404.html 404
        /ipns/* /not-a-gateway-404.html 404
        /routing/v1/* /not-a-gateway-404.html 404
      )REAL"};
};
}

TEST_F(RedirectsTest,SampleIsValid) {
  EXPECT_TRUE(sample_file_.valid());
  EXPECT_EQ(sample_file_.error(), "");
}
TEST_F(RedirectsTest,redirect_one) {
  auto path = "/redirect-one"s;
  EXPECT_EQ(301, sample_file_.rewrite(path));
  EXPECT_EQ(path,"/one.html");
}
TEST_F(RedirectsTest, redirect_one_explicit301) {
  auto path = "/301-redirect-one"s;
  EXPECT_EQ(301, sample_file_.rewrite(path));
  EXPECT_EQ(path, "/one.html");
}
TEST_F(RedirectsTest,redirect_nope) {
  auto path = "/redirect-nope"s;
  EXPECT_EQ(0, no_catchall_.rewrite(path));
  EXPECT_EQ(path,"/redirect-nope");
}
TEST_F(RedirectsTest,redirect_catch_all) {
  auto path = "/redirect-nope"s;
  EXPECT_EQ(200, sample_file_.rewrite(path));
  EXPECT_EQ(path,"/index.html");
}
TEST_F(RedirectsTest,placeholders) {
  auto path = "/posts/2023/10/27/yay.html"s;
  EXPECT_EQ(301, sample_file_.rewrite(path));
  EXPECT_EQ(path,"/articles/2023/10/27/yay.html");
}
TEST_F(RedirectsTest,splat) {
  auto path = "/splat/one/two/three"s;
  EXPECT_EQ(301, sample_file_.rewrite(path));
  EXPECT_EQ(path,"/redirected-splat/one/two/three");
}
TEST_F(RedirectsTest, invalidFile_justonetokenonanoncommentline) {
  r::File f{"justonetokenforwholefile"};
  EXPECT_FALSE(f.valid());
}
TEST_F(RedirectsTest, simple_302) {
  auto path = "/302-redirect-two"s;
  EXPECT_EQ(302, sample_file_.rewrite(path));
  EXPECT_EQ(path, "/two.html");
}
TEST_F(RedirectsTest, index_200) {
  auto path = "/200-index"s;
  EXPECT_EQ(200, sample_file_.rewrite(path));
  EXPECT_EQ(path, "/index.html");
}
TEST_F(RedirectsTest, notfounddir) {
  auto path = "/not-found/henry.mkv"s;
  EXPECT_EQ(404, sample_file_.rewrite(path));
  EXPECT_EQ(path, "/404.html");
}
TEST_F(RedirectsTest, gone) {
  auto path = "/gone/henry.mkv"s;
  EXPECT_EQ(410, sample_file_.rewrite(path));
  EXPECT_EQ(path, "/410.html");
}
TEST_F(RedirectsTest, unavail) {
  auto path = "/unavail/henry.mkv"s;
  EXPECT_EQ(451, sample_file_.rewrite(path));
  EXPECT_EQ(path, "/451.html");
}
TEST_F(RedirectsTest, twodigitstatus) {
  r::File f{"/a /b 40"};
  EXPECT_FALSE(f.valid());
}
TEST_F(RedirectsTest, inputtooshorttomatch) {
  r::Directive d{"/a/b/c/d/e/f", "/g/h/i", 200};
  auto p = "/a/b/c/d/e"s;
  EXPECT_EQ(0, d.rewrite(p));
  EXPECT_EQ(200, d.rewrite(p.append("/f")));
}
TEST_F(RedirectsTest, invalidFile_onetokenthenwhitespace) {
  r::File f{"justonetokenfollowedbyspace "};
  EXPECT_FALSE(f.valid());
}
TEST_F(RedirectsTest, hits_url_to) {
  auto p = "/blog/2023-05-multigateway-chromium-client/"s;
  auto status = ipfs_tech_.rewrite(p);
  EXPECT_EQ(status, 301);
  EXPECT_EQ(p, "https://blog.ipfs.tech/2023-05-multigateway-chromium-client/");
}