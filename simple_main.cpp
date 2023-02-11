#include <ipfs_client/block_storage.h>
#include <ipfs_client/ipfs_uri.h>

#include <filesystem>
#include <fstream>

void parse_block_file(char const* file_name);
void resolve_unixfs_path(std::string cid, std::string path);
void handle_arg(char const* arg) {
  if (std::filesystem::is_regular_file(arg)) {
    parse_block_file(arg);
  } else {
    auto slash = std::strchr(arg, '/');
    if (slash) {
      resolve_unixfs_path(std::string(arg, slash), std::string(slash + 1));
    } else {
      std::clog << "Unrecognized arg: " << arg << '\n';
      std::exit(99);
    }
  }
}

int main(int argc, char const* const argv[]) {
  std::for_each(std::next(argv), std::next(argv, argc), handle_arg);

  // TODO: these obviously should be unit tests.
#define CHECK                                                 \
  if (actual != expected) {                                   \
    std::clog << "'" << actual << "'!='" << expected << '\n'; \
    return 1;                                                 \
  }
  auto actual = ipfs::IpfsUri2IpfsOverHttpUrl(
      "ipfs://bafybeifszd4wbkeekwzwitvgijrw6zkzijxutm4kdumkxnc6677drtslni");
  std::string expected =
      "http://"
      "bafybeifszd4wbkeekwzwitvgijrw6zkzijxutm4kdumkxnc6677drtslni.ipfs.ipfs-"
      "over-http.localhost/";
  CHECK
  actual = ipfs::IpfsOverHttpUrl2IpfsGatewayPath(actual);
  expected =
      "ipfs/bafybeifszd4wbkeekwzwitvgijrw6zkzijxutm4kdumkxnc6677drtslni/";
  CHECK
  actual = ipfs::IpfsOverHttpUrl2IpfsGatewayPath(
      "http://"
      "k51qzi5uqu5dijv526o4z2z10ejylnel0bfvrtw53itcmsecffo8yf0zb4g9gi.ipns."
      "ipfs-over-http.localhost/");
  expected =
      "ipns/k51qzi5uqu5dijv526o4z2z10ejylnel0bfvrtw53itcmsecffo8yf0zb4g9gi/";
  CHECK
  actual = ipfs::IpfsOverHttpUrl2IpfsGatewayPath(
      "http://"
      "k51qzi5uqu5dijv526o4z2z10ejylnel0bfvrtw53itcmsecffo8yf0zb4g9gi.ipns."
      "ipfs-over-http.localhost/path/to/file.html");
  expected =
      "ipns/k51qzi5uqu5dijv526o4z2z10ejylnel0bfvrtw53itcmsecffo8yf0zb4g9gi/"
      "path/to/file.html";
  CHECK
  actual = ipfs::IpfsOverHttpUrl2IpfsGatewayPath(
      "http://en.wikipedia-on-ipfs.org.ipns.ipfs-over-http.localhost/wiki/");
  expected = "ipns/en.wikipedia-on-ipfs.org/wiki/";
  CHECK
}
namespace {
ipfs::BlockStorage blocks;
}
void parse_block_file(char const* file_name) {
  std::ifstream file{file_name};
  ipfs::Block node{file};
  if (!node.valid()) {
    std::clog << "Failed to parse '" << file_name << "'\n";
    return;
  }
  std::clog << file_name << ' ' << node.type() << ' ' << node.file_size() << ' '
            << '\n';
  node.List(
      [](auto n, auto c) { std::clog << "  " << n << " => " << c << '\n'; });
  blocks.Store(file_name, std::move(node));
}
void resolve_unixfs_path(std::string cid, std::string path) {
  auto result = blocks.AttemptResolve(cid, path);
}