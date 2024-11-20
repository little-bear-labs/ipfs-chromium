#include <ipfs_client/gw/default_requestor.h>

#include <mock_api.h>
#include <mock_requestor.h>

namespace {
struct Arg {
  std::string name;
  std::shared_ptr<ig::Requestor> next;
};
}  // namespace

void ig::Requestor::TestAccess(void* p) {
  auto a = reinterpret_cast<Arg*>(p);
  a->name.assign(name());
  a->next = next_;
}

TEST(DefaultRequestorTest, name_check) {
  auto early = std::make_shared<MockRequestor>();
  auto api = std::make_shared<MockApi>();
  auto result = ig::default_requestor(early, api);
  Arg a;
  result->TestAccess(&a);
  EXPECT_EQ(a.name, "InlineRequestHandler");
  EXPECT_TRUE(a.next);
  a.next->TestAccess(&a);
  EXPECT_EQ(a.name, "BlockRequestSplitter");
  EXPECT_TRUE(a.next == early);
  a.next->TestAccess(&a);
  EXPECT_EQ(a.name, "MockRequestor");
  EXPECT_TRUE(a.next);
  a.next->TestAccess(&a);
  EXPECT_EQ(a.name, "DNSLink requestor");
  EXPECT_TRUE(a.next);
  a.next->TestAccess(&a);
  EXPECT_EQ(a.name, "multi-gateway requestor");
  //  EXPECT_EQ(a.name, "requestor pool");
  EXPECT_TRUE(a.next);
  a.next->TestAccess(&a);
  EXPECT_EQ(a.name, "Terminating requestor");
  EXPECT_FALSE(a.next);
}
