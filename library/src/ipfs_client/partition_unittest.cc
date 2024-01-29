#include "ipfs_client/partition.h"

#include <mock_cbor.h>

#include <ipfs_client/dag_json_value.h>
#include <ipfs_client/ipfs_request.h>
#include <ipfs_client/ipns_record.h>
#include <ipfs_client/ipns_record.pb.h>
#include <ipfs_client/logger.h>
#include <ipfs_client/pb_dag.h>

#include "ipld/dnslink_name.h"

#include <filesystem>
#include <fstream>

namespace i = ipfs;
namespace ii = i::ipld;
namespace ig = i::gw;
using namespace std::literals;
using Success = i::Response;

namespace {
struct MockApi final : public ipfs::Client {
  MockApi() { i::log::SetLevel(i::log::Level::Off); }
  bool VerifyKeySignature(SigningKeyType,
                          ByteView signature,
                          ByteView data,
                          ByteView key_bytes) const {
    return true;
  }
  std::string MimeType(std::string e,
                       std::string_view c,
                       std::string const& u) const {
    auto result = "Mime from extension=" + e + " 'url'=" + u + " content='";
    if (c.size() < 9) {
      result.append(c);
    } else {
      result.append(c.substr(0, 8)).append("...");
    }
    return result + "'";
  }
  std::string UnescapeUrlComponent(std::string_view url_comp) {
    return std::string{url_comp};
  }
  std::string UnescapeUrlComponent(std::string_view u) const {
    return std::string{u};
  }
  std::unique_ptr<ipfs::DagJsonValue> ParseJson(std::string_view) const {
    return {};
  }
  auto GetGateway(std::size_t index) const -> std::optional<i::GatewaySpec> {
    return std::nullopt;
  }
  void Discover(std::function<void(std::vector<std::string>)> cb) {}
  std::vector<MockCbor> mutable cbors;
  std::unique_ptr<ipfs::DagCborValue> ParseCbor(ByteView) const {
    if (cbors.empty()) {
      return {};
    }
    auto r = std::make_unique<MockCbor>(cbors.front());
    cbors.erase(cbors.begin());
    return std::move(r);
  }
  unsigned GetGatewayRate(std::string_view) { return 120U; }
  void AddGateway(std::string_view) {}
};
struct TestRequestor final : public ig::Requestor {
  TestRequestor() { api_ = std::make_shared<MockApi>(); }
  std::string_view name() const { return "return test requestor"; }
  HandleOutcome handle(ig::RequestPtr r) {
    auto cid = r->main_param;
    auto base_dir = std::filesystem::path{__FILE__};
    while (!is_directory(base_dir / "test_data" / "blocks") &&
           base_dir.generic_string().size() > 2) {
      base_dir = base_dir.parent_path();
    }
    switch (r->type) {
      case i::gw::Type::Ipns: {
        auto dir = base_dir / "test_data" / "names";
        auto f = dir / cid;
        EXPECT_TRUE(is_regular_file(f)) << cid << " missing";
        if (!is_regular_file(f)) {
          auto cmd =
              "ipfs routing get /ipns/" + cid + " > " + f.generic_string();
          std::cout << cmd << std::endl;
          auto ec = std::system(cmd.c_str());
          EXPECT_EQ(ec, 0);
        }
        std::ifstream fs{f};
        std::string buf(std::filesystem::file_size(f) + 1, '\0');
        fs.read(buf.data(), buf.size());
        auto api = std::make_shared<MockApi>();
        ipfs::ipns::IpnsEntry entry;
        EXPECT_TRUE(entry.ParseFromArray(buf.data(), buf.size() - 1));
        //        api.ipns_entries.push_back({entry.value(), entry.validity(),
        //                                    static_cast<unsigned>(entry.validitytype()),
        //                                    entry.sequence(), entry.ttl()});
        MockCbor cbor;
        cbor.set("Value", entry.value());
        cbor.set("Validity", entry.validity());
        cbor.set("ValidityType", entry.validitytype());
        cbor.set("Sequence", entry.sequence());
        cbor.set("TTL", entry.ttl());
        api->cbors.push_back(cbor);
        buf.resize(buf.size() - 1);
        r->RespondSuccessfully(buf, api);
      } break;
      case i::gw::Type::Car:
        r->type = i::gw::Type::Block;
        [[fallthrough]];
      case i::gw::Type::Block: {
        auto blocs_dir = base_dir / "test_data" / "blocks";
        EXPECT_TRUE(is_directory(blocs_dir));
        auto f = blocs_dir / cid;
        auto canon = i::Cid{cid}.to_string();
        if (!is_regular_file(f) && cid != canon) {
          f = blocs_dir / canon;
        }
        EXPECT_TRUE(is_regular_file(f)) << cid << " missing";
        if (is_regular_file(f)) {
          std::ifstream fs{f};
          std::string buf(std::filesystem::file_size(f) + 1, '\0');
          fs.read(buf.data(), buf.size());
          buf.resize(buf.size() - 1);
          r->RespondSuccessfully(buf, api_);
        } else {
          auto cmd =
              "ipfs block get " + cid + " > '" + f.generic_string() + "'";
          std::cout << cmd << '\n';
          auto ec = std::system(cmd.c_str());
          EXPECT_EQ(ec, 0);
          return HandleOutcome::DONE;
        }
      } break;
      case i::gw::Type::DnsLink: {
        auto dir = base_dir / "test_data" / "names";
        auto f = dir / cid;
        EXPECT_TRUE(is_regular_file(f)) << cid << " missing";
        if (!is_regular_file(f)) {
          auto cmd = "host -t TXT _dnslink." + cid +
                     " | grep dnslink= | cut -d '=' -f 2- | tr -d '\"' > " +
                     f.generic_string();
          auto ec = std::system(cmd.c_str());
          EXPECT_EQ(ec, 0);
        }
        std::ifstream fs{f};
        std::string target;
        std::getline(fs, target);
        r->RespondSuccessfully(target, api_);
      } break;
      default:
        return HandleOutcome::DONE;
    }

    return HandleOutcome::DONE;
  }
};
struct OrchestratingRealData : public ::testing::Test {
  std::shared_ptr<MockApi> api_ = std::make_shared<MockApi>();
  std::shared_ptr<i::Partition> orc_;
  OrchestratingRealData() {
    i::log::SetLevel(i::log::Level::Off);
    auto f = [](auto) {};
    resp_.body_ = "No response received.";
    resp_.status_ = 0;
    resp_.mime_ = "uninit - no mime provided";
    resp_.location_ = "uninit";
    api_->with(std::make_shared<TestRequestor>());
    orc_ = api_->partition({});
  }
  i::Response resp_;
  void dorequest(std::string_view ipfs_path) {
    std::cout << "dorequest(" << ipfs_path << ")\n";
    auto f = [this](auto&, auto& r) { resp_ = r; };
    auto top_req = std::make_shared<i::IpfsRequest>(std::string{ipfs_path}, f);
    orc_->build_response(top_req);
  }
  std::string abs_path(std::string rest) {
    return "/ipfs/QmYBhLYDwVFvxos9h8CGU2ibaY66QNgv8hpfewxaQrPiZj" + rest;
  }
};
}  // namespace

TEST_F(OrchestratingRealData, one_html_present) {
  ipfs::log::SetLevel(ipfs::log::Level::Off);
  dorequest(abs_path("/one.html"));
  EXPECT_EQ(resp_.status_, 200);
  EXPECT_EQ(resp_.body_, "my one\n");
  EXPECT_EQ(resp_.mime_, "text/html");
}

TEST_F(OrchestratingRealData, hit_404_splat) {
  dorequest(abs_path("/not-found/definitely_not_there"));
  EXPECT_EQ(resp_.status_, 404);
  EXPECT_EQ(resp_.body_, "my 404\n");
}
TEST_F(OrchestratingRealData, hit_index_catchall) {
  dorequest(abs_path("/definitely_not_there"));
  EXPECT_EQ(resp_.status_, 200);
  EXPECT_EQ(resp_.body_, "my index\n");
  EXPECT_EQ(resp_.mime_, "text/html");
}
TEST_F(OrchestratingRealData, normal_permanent_redirect) {
  dorequest(abs_path("/redirect-one"));
  EXPECT_EQ(resp_.status_, 301);
  EXPECT_EQ(resp_.body_, "");
  EXPECT_EQ(resp_.location_, "/one.html");
}
TEST_F(OrchestratingRealData, multinodefile_hit) {
  dorequest(
      "/ipfs/bafybeif5shuqnuh2psa3syipw62scqokgbta43vv6xtfdjl6qirahmcxeq/bdir/"
      "cdir/multinodefile.txt");
  EXPECT_EQ(resp_.status_, 200);
  auto i = 0ul;
  for (auto a = 'A'; a <= 'Z'; ++a) {
    for (auto b = 'a'; a <= 'z'; ++a) {
      for (auto c = '0'; a <= '9'; ++a) {
        EXPECT_LT(i + 4, resp_.body_.size());
        EXPECT_EQ(resp_.body_.at(i++), a);
        EXPECT_EQ(resp_.body_.at(i++), b);
        EXPECT_EQ(resp_.body_.at(i++), c) << i;
        EXPECT_EQ(resp_.body_.at(i++), ' ') << i;
      }
    }
  }
}
TEST_F(OrchestratingRealData, multinodefile_pbdagleaves) {
  dorequest(
      "/ipfs/QmbT19Pd1N1G6Qmb5aRxJijTUumkqsPWtasAbXRBCgdH1F/bdir/"
      "cdir/multinodefile.txt");
  EXPECT_EQ(resp_.status_, 200);
  auto i = 0ul;
  for (auto a = 'A'; a <= 'Z'; ++a) {
    for (auto b = 'a'; a <= 'z'; ++a) {
      for (auto c = '0'; a <= '9'; ++a) {
        EXPECT_LT(i + 4, resp_.body_.size());
        EXPECT_EQ(resp_.body_.at(i++), a);
        EXPECT_EQ(resp_.body_.at(i++), b);
        EXPECT_EQ(resp_.body_.at(i++), c) << i;
        EXPECT_EQ(resp_.body_.at(i++), ' ') << i;
      }
    }
  }
}
TEST_F(OrchestratingRealData, examples_has_index_html) {
  i::log::SetLevel(i::log::Level::Info);
  dorequest("/ipfs/QmQyqMY5vUBSbSxyitJqthgwZunCQjDVtNd8ggVCxzuPQ4/examples");
  EXPECT_EQ(resp_.status_, 200);
  EXPECT_EQ(resp_.body_, "my index\n");
  EXPECT_EQ(resp_.mime_, "text/html");
}
TEST_F(OrchestratingRealData, examples_articles_generates_list) {
  dorequest(
      "/ipfs/QmQyqMY5vUBSbSxyitJqthgwZunCQjDVtNd8ggVCxzuPQ4/examples/articles");
  EXPECT_EQ(resp_.status_, 200);
  EXPECT_EQ(resp_.mime_, "text/html");
  EXPECT_EQ(resp_.body_, R"(<html>
  <title>/examples/articles/ (directory listing)</title>
  <body>
    <ul>
      <li>
        <a href='/examples/'>..</a>
      </li>
      <li>
        <a href='/examples/articles/2022'>2022</a>
      </li>
    </ul>
  </body>
</html>
)");
}
TEST_F(OrchestratingRealData, dirshard_immediate_hit) {
  i::log::SetLevel(i::log::Level::Trace);
  dorequest(
      "/ipfs/bafybeiaysi4s6lnjev27ln5icwm6tueaw2vdykrtjkwiphwekaywqhcjze/"
      "index.html");
  EXPECT_EQ(resp_.status_, 200);
  EXPECT_EQ(resp_.mime_, "text/html");
  EXPECT_EQ(resp_.body_, R"(<head>
    <noscript>
        <meta http-equiv="refresh" content="3; url=wiki/" />
    </noscript>

    <script>
        location.replace('wiki/');
    </script>
</head>)");
}
TEST_F(OrchestratingRealData, dirshard_hit_index_html_without_specifying_it) {
  dorequest(
      "/ipfs/bafybeiaysi4s6lnjev27ln5icwm6tueaw2vdykrtjkwiphwekaywqhcjze/");
  EXPECT_EQ(resp_.status_, 200);
  EXPECT_EQ(resp_.mime_, "text/html");
  EXPECT_EQ(resp_.body_, R"(<head>
    <noscript>
        <meta http-equiv="refresh" content="3; url=wiki/" />
    </noscript>

    <script>
        location.replace('wiki/');
    </script>
</head>)");
}
TEST_F(OrchestratingRealData, dirshard_deep) {
  dorequest(
      "/ipfs/bafybeiaysi4s6lnjev27ln5icwm6tueaw2vdykrtjkwiphwekaywqhcjze/wiki/"
      "Madlener_House");
  // The root CID is the root of a sharded directory
  //    you immediately hit 58wiki ->
  //    bafybeihn2f7lhumh4grizksi2fl233cyszqadkn424ptjajfenykpsaiw4
  // ...kpsaiw4 is the root of another sharded dir (wiki)
  // Descend through multiple layers of the wiki (internal) tree without pathing
  // 3B->bafybeidzeeqkore2fxhrhadwipv6277m7iqvitkknn6i32wkyeuvibrgce
  // FA->bafybeiasemiepkyez6ukxpktj5fo5ubhgzpzjvjzs2n3smfdwtn7jw7ylm
  // FC->bafybeihxfki3tytoi2qslwr5bzbfxgws5c7eqryakzk3bq6qavzuo7uhcy
  // 48Madlener_House->bafkreig2aa7xzxfshmfv6v55rlu455kaclrnrk5hx3ayo2vf7ekr24xdfm
  EXPECT_EQ(resp_.status_, 200);
  EXPECT_EQ(resp_.mime_, "application/octet-stream");
  EXPECT_EQ(resp_.body_.substr(0, 22), "<!DOCTYPE html>\n<html ");
}
TEST_F(OrchestratingRealData, ipns) {
  dorequest(
      "/ipns/k51qzi5uqu5dku8zqce3b7kmpcw6uua9w00b5boyaevowmzls2rpie0itokett");
  EXPECT_EQ(resp_.status_, 200);
  EXPECT_EQ(resp_.mime_, "text/html");
  EXPECT_EQ(resp_.body_.substr(0, 212), R"(<!DOCTYPE html>
<html  lang="en-US">
<head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>An open system to manage data without a central server | IPFS</title>
)");
}
TEST_F(OrchestratingRealData, dnslink) {
  dorequest("/ipns/ipfs.tech");
  EXPECT_EQ(resp_.status_, 200);
  EXPECT_EQ(resp_.mime_, "text/html");
  EXPECT_EQ(resp_.body_.substr(0, 212), R"(<!DOCTYPE html>
<html  lang="en-US">
<head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>An open system to manage data without a central server | IPFS</title>
)");
}
