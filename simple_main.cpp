#include <ipfs_client/gateways.h>
#include <ipfs_client/gen/pb-dag.pb.h>
#include <ipfs_client/ipfs_uri.h>

#include <libp2p/multi/multibase_codec/codecs/base58.hpp>

#include <filesystem>
#include <fstream>

void parse_block_file(char const* file_name);

int main(int argc, char const* const argv[]) {
  ipfs::Gateways gs;
  std::for_each(std::next(argv), std::next(argv, argc), parse_block_file);

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
  std::clog << actual << std::endl;
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

void parse_block_file(char const* file_name) {
  std::ifstream file{file_name};
  PBNode node;
  if (!node.ParseFromIstream(&file)) {
    std::clog << "Failed to parse '" << file_name << "'\n";
    return;
  }
  std::clog << "\nParsed out '" << file_name << "'\n  data=";
  for (char c : node.data()) {
    if (std::isgraph(c) || c == ' ') {
      std::clog.put(c);
    } else {
      std::clog << "\\x" << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<unsigned>(c);
    }
  }
  std::clog.put('\n');
  for (auto& link : node.links()) {
    std::clog << "  link: " << link.name() << " has a hash with " << std::dec
              << link.hash().size()
              << " bytes, decodes to "
              //                  << " 0x" << std::hex <<
              //                  static_cast<unsigned>(link.hash().at(0))
              //                  << " 0x" << std::hex <<
              //                  static_cast<unsigned>(link.hash().at(1))
              //                  << " \n";
              << libp2p::multi::detail::encodeBase58(link.hash()) << '\n';
  }
}
