#include <ipfs_client/ipld/dag_headers.h>

#include <mock_api.h>

using T = ii::DagHeaders;
using S = ii::BlockSource;

TEST(DagHeadersTest, DifferentCategoriesDontBucket) {
  T t;
  S s;
  EXPECT_EQ(t.headers().size(), 0UL);
  t.Add(s);
  EXPECT_EQ(t.headers().size(), 1UL);
  s.cat.cached = true;
  t.Add(s);
  EXPECT_EQ(t.headers().size(), 2UL);
  s.cat.gateway_url = "http://127.0.0.1:8080/";
  t.Add(s);
  EXPECT_EQ(t.headers().size(), 3UL);
  s.cat.cached = false;
  t.Add(s);
  EXPECT_EQ(t.headers().size(), 4UL);
  s.cat.request_type = RT::Block;
  t.Add(s);
  EXPECT_EQ(t.headers().size(), 5UL);
}
