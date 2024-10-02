#include <ipfs_client/ipld/dag_node.h>
#include <mock_api.h>

#include <ipfs_client/ipld/dag_json_node.h>

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
