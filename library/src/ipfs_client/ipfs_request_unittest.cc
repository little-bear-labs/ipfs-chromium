#include <ipfs_client/ipfs_request.h>

#include <gtest/gtest.h>

using T = ipfs::IpfsRequest;

TEST(IpfsRequestTest,fromUrlConvertsToPath) {
    auto t = T::fromUrl("ipfs://cid/file.txt", [](auto&,auto&){});
    auto p = t->path();
    EXPECT_EQ(p.pop(),"ipfs");
    EXPECT_EQ(p.pop(),"cid");
    EXPECT_EQ(p.pop(),"file.txt");
}
