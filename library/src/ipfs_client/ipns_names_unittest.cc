#include <ipfs_client/ipns_names.h>

#include <gtest/gtest.h>

namespace {
struct IpnsNamesTest : public ::testing::Test {
  IpnsNamesTest();
  ipfs::IpnsNames tested;
};
}  // namespace

TEST_F(IpnsNamesTest, MissingYieldsEmpty) {
  EXPECT_EQ("", tested.NameResolvedTo("NoSuchName"));
}
TEST_F(IpnsNamesTest, ResolvesToIpfsDirectly) {
  EXPECT_EQ("ipfs/cid", tested.NameResolvedTo("1"));
}
TEST_F(IpnsNamesTest, ResolvesToIpfsIndirectly) {
  EXPECT_EQ("ipfs/cid", tested.NameResolvedTo("2"));
}
TEST_F(IpnsNamesTest, DanglingResolvesToIpns) {
  EXPECT_EQ("ipns/NoSuchName", tested.NameResolvedTo("dangle"));
}
TEST_F(IpnsNamesTest, RealisticCycleResolvesToEmpty) {
  EXPECT_EQ("", tested.NameResolvedTo("cycle1"));
}
TEST_F(IpnsNamesTest, MultiStepCycleResolvesToEmpty) {
  EXPECT_EQ("", tested.NameResolvedTo("cycle2"));
}

namespace {
IpnsNamesTest::IpnsNamesTest() {
  tested.AssignName("2", "ipns/1");
  tested.AssignName("1", "ipfs/cid");
  tested.AssignName("cycle1", "ipns/cycle1");
  tested.AssignName("cycle2", "ipns/cycle2b");
  tested.AssignName("cycle2b", "ipns/cycle2");
  tested.AssignName("dangle", "ipns/NoSuchName");
}
}  // namespace
