#include "dag_json_node.h"

#include <gtest/gtest.h>

#include <ipfs_client/json_cbor_adapter.h>

namespace i = ipfs;
namespace ii = i::ipld;
using namespace std::literals;

using Super = ii::DagNode;
using Tested = ii::DagJsonNode;

#if HAS_JSON_CBOR_ADAPTER

namespace n = nlohmann;

namespace {
struct DagJsonNodeTest : public ::testing::Test {
  auto noop_blu() const {
    return [](auto&) { return ii::NodePtr{}; };
  }
  std::unique_ptr<Tested> tested_;

  Super& super() { return tested(); }
  Tested& tested() {
    if (!tested_) {
      n::json data =
          // clang-format off
        {//map with keys link;payload;signatures
          {"link", {//link aka map with 1 element whose key is "/"
            {"/", "bafyreiejkvsvdq4smz44yuwhfymcuvqzavveoj2at3utujwqlllspsqr6q"}
          }},
          {"payload", "AXESIIlVZVHDkmZ5zFLHLhgqVhkFakcnQJ7pOibQWtcnyhH0"},
          {"signatures", {//array with 1 element
              {//map with keys protected;signature
                {"protected", "eyJhbGciOiJFZERTQSJ9"},
                {"signature", "-_9J5OZcl5lVuRlgI1NJEzc0FqEb6_2yVskUaQPducRQ4oe-N5ynCl57wDm4SPtm1L1bltrphpQeBOeWjVW1BQ"}
              }
            }
          }
        }
      // clang-format on
      ;
      EXPECT_TRUE(data.is_object());
      EXPECT_EQ(data.size(), 3U);
      EXPECT_TRUE(data.at("link").is_object());
      EXPECT_TRUE(data.at("payload").is_string());
      EXPECT_TRUE(data.at("signatures").is_array());

      EXPECT_EQ(data.at("link").size(), 1U);
      EXPECT_TRUE(data.at("link").at("/").is_string());

      EXPECT_EQ(data.at("signatures").size(), 1U);
      EXPECT_TRUE(data.at("signatures").at(0).is_object());
      EXPECT_EQ(data.at("signatures").at(0).size(), 2U);
      EXPECT_TRUE(data.at("signatures").at(0).at("protected").is_string());
      EXPECT_TRUE(data.at("signatures").at(0).at("signature").is_string());

      auto adapt = std::make_unique<i::JsonCborAdapter>(data);
      EXPECT_TRUE(adapt->is_map());
      tested_ = std::make_unique<Tested>(std::move(adapt));
    }
    return *tested_;
  }
};
}  // namespace

TEST_F(DagJsonNodeTest, preview) {
  std::string p;
  auto result = super().resolve(i::SlashDelimited{""}, noop_blu());
  auto& resp = std::get<i::Response>(result);
  EXPECT_EQ(resp.status_, 200);
  EXPECT_EQ(resp.mime_, "text/html");
  EXPECT_EQ(resp.body_,
            R"HTML(<html><title>Preview of DAG-JSON</title><body>
{<table>
  <tr>
    <td>&nbsp;&nbsp;</td>
    <td style='vertical-align:top'>&quot;link&quot;:</td>
    <td>
<a href='ipfs://bafyreiejkvsvdq4smz44yuwhfymcuvqzavveoj2at3utujwqlllspsqr6q'>bafyreiejkvsvdq4smz44yuwhfymcuvqzavveoj2at3utujwqlllspsqr6q</a>
,    </td>
  </tr>
  <tr>
    <td>&nbsp;&nbsp;</td>
    <td style='vertical-align:top'>&quot;payload&quot;:</td>
    <td>
&quot;AXESIIlVZVHDkmZ5zFLHLhgqVhkFakcnQJ7pOibQWtcnyhH0&quot;,    </td>
  </tr>
  <tr>
    <td>&nbsp;&nbsp;</td>
    <td style='vertical-align:top'>&quot;signatures&quot;:</td>
    <td>
[<table>
  <tr>
    <td>&nbsp;&nbsp;</td>
    <td>
{<table>
  <tr>
    <td>&nbsp;&nbsp;</td>
    <td style='vertical-align:top'>&quot;protected&quot;:</td>
    <td>
&quot;eyJhbGciOiJFZERTQSJ9&quot;,    </td>
  </tr>
  <tr>
    <td>&nbsp;&nbsp;</td>
    <td style='vertical-align:top'>&quot;signature&quot;:</td>
    <td>
&quot;-_9J5OZcl5lVuRlgI1NJEzc0FqEb6_2yVskUaQPducRQ4oe-N5ynCl57wDm4SPtm1L1bltrphpQeBOeWjVW1BQ&quot;,    </td>
  </tr>
</table>}
  </td>
  </tr>
</table>]
,    </td>
  </tr>
</table>}

</body></html>)HTML");
}

#endif
