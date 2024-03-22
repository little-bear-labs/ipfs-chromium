#include <ipfs_client/ctx/boost_beast_http.h>

#include <gtest/gtest.h>

#include <ipfs_client/client.h>

TEST(BoostBeastHttpTest, GoogleHomePage) {
  boost::asio::ssl::context ssl_sux{boost::asio::ssl::context::sslv23_client};
  boost::asio::io_context io;
  ipfs::ctx::BoostBeastHttp t{io};
  ipfs::HttpRequestDescription d_;
  d_.timeout_seconds = 9;
  d_.accept = "text/html";
  d_.max_response_size = 1024UL * 1024UL;
  d_.url = "http://www.google.com/";
  std::int16_t status = 0;
  std::string body;
  t.SendHttpRequest(d_, [&](auto s, auto b, auto h) {
    status = s;
    body.assign(b);
  });
  for (auto i = 0; !status && i < 9; ++i) {
    io.run();
  }
  EXPECT_EQ(status, 200);
  EXPECT_TRUE(body.find("Google has many special features") < body.size())
      << body;
}
