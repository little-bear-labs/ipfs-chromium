#include <ipfs_client/ctx/null_dns_txt_lookup.h>

#include <gtest/gtest.h>

using T = ipfs::ctx::NullDnsTxtLookup;

TEST(NullDnsTxtLookup,Fails) {
  T t;
  auto& b = static_cast<ipfs::ctx::DnsTxtLookup&>(t);
  bool called = false;
  b.SendDnsTextRequest("google.com", {}, [&called](){called=true;});
  EXPECT_TRUE(called);
}
