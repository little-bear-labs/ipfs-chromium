#include "car.h"

#include <mock_api.h>

#include <ipfs_client/ctx/null_cbor_parser.h>

#include <algorithm>
#include <filesystem>
#include <fstream>

using T = ipfs::Car;
namespace fs = std::filesystem;

namespace {
struct CarTest : public testing::Test {
  std::shared_ptr<MockApi> api = std::make_shared<MockApi>();
  fs::path dir() const {
    auto cars_unittest_cc = fs::path{__FILE__};
    auto ipfs_client = cars_unittest_cc.parent_path();
    auto src = ipfs_client.parent_path();
    auto library = src.parent_path();
    auto project_root = library.parent_path();
    return project_root / "test_data" / "cars";
  }
  std::vector<i::Cid> list(std::string file_name) {
    std::vector<char> bytes;
    auto fp = dir() / file_name;
    std::ifstream f{fp};
    bytes.resize(file_size(fp));
    EXPECT_TRUE(f.read(bytes.data(), bytes.size()));
    T car(ipfs::as_bytes(bytes), api->cbor());
    std::vector<i::Cid> rv;
    while (auto block = car.NextBlock()) {
      rv.push_back(block->cid);
    }
    return rv;
  }
};
}  // namespace

#if HAS_JSON_CBOR_ADAPTER

TEST_F(CarTest, SmallishDirectoryAndAllItsContents) {
  auto result =
      list("bafybeifyeyhzmhj5fg2f5v7k4gsvazw7iwsjehpq77o54vd4kwdd3fud3y");
  EXPECT_EQ(result.size(), 4UL);
  EXPECT_EQ(result.at(0).to_string(),
            "bafybeifyeyhzmhj5fg2f5v7k4gsvazw7iwsjehpq77o54vd4kwdd3fud3y");
  EXPECT_EQ(result.at(1).to_string(),
            "bafkreigks6arfsq3xxfpvqrrwonchxcnu6do76auprhhfomao6c273sixm");
  EXPECT_EQ(result.at(2).to_string(),
            "bafkreih3ryqpylsmh4siyygdtplff46bgrzjro4xpofu2widxbifkyqgam");
  EXPECT_EQ(result.at(3).to_string(),
            "bafkreif2pall7dybz7vecqka3zo24irdwabwdi4wc55jznaq75q7eaavvu");
}

TEST_F(CarTest, V2FromOfficialTestFixture) {
  auto result = list("carv2-basic.car");
  EXPECT_EQ(result.size(), 5UL);
  EXPECT_EQ(result.at(0).to_string(),
            "bafybeih3c32qqnas54jxdubr5vfkeomqhwco7ww7dor42z4omr23dirs7a");
  EXPECT_EQ(result.at(1).to_string(),
            "bafybeigzydkto3jg6gjr66wvfv5myah4ccinf3nqqcf7mhxlbiksqjxwey");
  EXPECT_EQ(result.at(2).to_string(),
            "bafybeigxiw3xk723iwj65k3yeaygy66gj22jnj2bbigqpx32gt76ys4x6e");
  EXPECT_EQ(result.at(3).to_string(),
            "bafkreifuosuzujyf4i6psbneqtwg2fhplc2wxptc5euspa2gn3bwhnihfu");
  EXPECT_EQ(result.at(4).to_string(),
            "bafkreifc4hca3inognou377hfhvu2xfchn2ltzi7yu27jkaeujqqqdbjju");
}
#endif
TEST_F(CarTest, NullParserFail) {
  api->with(std::make_unique<ic::NullCborParser>());
  auto result =
      list("bafybeifyeyhzmhj5fg2f5v7k4gsvazw7iwsjehpq77o54vd4kwdd3fud3y");
  EXPECT_EQ(result.size(),0UL);
}
