#include <ipfs_client/ipld/dag_node.h>
#include <mock_api.h>

#include <ipfs_client/ipld/dag_json_node.h>
#include <ipfs_client/ipld/dnslink_name.h>

using T = ii::DagNode;

namespace {
struct DagNodeTest : public ::testing::Test {
    std::shared_ptr<MockApi> api = std::make_shared<MockApi>();
};
}

TEST_F(DagNodeTest,ConstructDagJson) {
    i::Cid cid{"baguqeaaeejugsiq"};
    auto p = T::fromBytes(api, cid, "\"hi\"");
    ASSERT_TRUE(p);
    auto d = dynamic_cast<ii::DagJsonNode*>(p.get());
    ASSERT_TRUE(d);
}
TEST_F(DagNodeTest,DnsLinkIs) {
    ii::DnsLinkName d{"/ipfs/baguqeaaeejugsiq"};
    auto p = static_cast<T*>(&d);
    ASSERT_TRUE(p);
    auto down = p->as_dnslink();
    EXPECT_TRUE(down);
    EXPECT_TRUE(down == &d);
}
TEST_F(DagNodeTest,DagJsonIsNotOtherTHings) {
    i::Cid cid{"baguqeaaeejugsiq"};
    auto p = T::fromBytes(api, cid, "\"hi\"");
    ASSERT_TRUE(p);
    EXPECT_FALSE(p->as_dnslink());
    EXPECT_FALSE(p->as_hamt());
    EXPECT_FALSE(p->as_ipns());
}
