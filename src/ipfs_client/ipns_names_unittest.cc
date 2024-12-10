#include <ipfs_client/ipns_names.h>

#include <google/protobuf/stubs/logging.h>
#include <gtest/gtest.h>

namespace {
struct IpnsNamesTest : public ::testing::Test {
  IpnsNamesTest();
  ipfs::IpnsNames tested;
  google::protobuf::LogSilencer no_log;
  void fake(std::string n, std::string t);
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
  fake("2", "ipns/1");
  fake("1", "ipfs/cid");
  fake("cycle1", "ipns/cycle1");
  fake("cycle2", "ipns/cycle2b");
  fake("cycle2b", "ipns/cycle2");
  fake("dangle", "ipns/NoSuchName");
}
void IpnsNamesTest::fake(std::string n, std::string t) {
  ipfs::ValidatedIpns e;
  e.value = t;
  e.use_until = 9876543210;
  tested.AssignName(n, std::move(e));
}
}  // namespace
