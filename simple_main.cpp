#include <ipfs_client/block_storage.h>
#include <ipfs_client/framework_api.h>
#include <ipfs_client/unixfs_path_resolver.h>
#include <libp2p/multi/multibase_codec/codecs/base58.hpp>

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
    } else if (arg[0] == 'Q' && arg[1] == 'm') {
      auto binary = libp2p::multi::detail::decodeBase58(arg + 4).value();
      for (unsigned char byte : binary) {
        std::clog << std::hex << static_cast<unsigned>(byte);
      }
      std::clog.put('\n');
    } else {
      std::clog << "Unrecognized arg: " << arg << '\n';
      std::exit(99);
    }
  }
}

int main(int argc, char const* const argv[]) {
  std::for_each(std::next(argv), std::next(argv, argc), handle_arg);
}
namespace {
ipfs::BlockStorage blocks;
std::string file_contents;
class StubbedApi : public ipfs::FrameworkApi {
 public:
  void RequestByCid(std::string c, ipfs::Scheduler::Priority p) {
    if (p == ipfs::Scheduler::Priority::Optional) {
      std::clog << "Consider prefetching " << c << '\n';
      return;
    }
    std::clog << "Unsatisfied required CID: " << c << ' ';
    if (std::filesystem::is_regular_file(c)) {
      std::clog << "It exists on-disk. Load it next time.\n";
    } else {
      std::clog << "Attempting to fetch it from Kubo.\n";
      auto cmd = "curl 'http://chomp:8080/ipfs/" + c;
      cmd.append("?format=raw' > ").append(c);
      std::system(cmd.c_str());
    }
  }

  void InitiateGatewayRequest(ipfs::BusyGateway) {}
  void ReceiveBlockBytes(std::string_view s) {
    std::clog << "Receiving " << s.size() << " bytes.\n";
    file_contents.append(s);
  }
  std::string out_fn;
  void BlocksComplete(std::string type) {
    std::clog << "Got " << type << ". File contents (" << file_contents.size()
              << " B). Writing to " << out_fn << '\n';
    std::ofstream f{out_fn};
    f.write(file_contents.c_str(), file_contents.size());
    file_contents.clear();
  }

  std::string MimeType(std::string extension,
                       std::string_view content,
                       std::string const& url) const {
    return "";
  }
  std::string UnescapeUrlComponent(std::string_view s) const {
    return std::string{s};
  }
};
auto api = std::make_shared<StubbedApi>();
}  // namespace
void parse_block_file(char const* file_name) {
  std::ifstream file{file_name};
  ipfs::Block node{file};
  if (!node.valid()) {
    std::clog << "Failed to parse '" << file_name << "'\n";
    return;
  }
  std::clog << "Loaded: " << file_name << ' ' << node.type() << ' '
            << node.file_size() << "\n  data:";
  for (auto byte : node.fsdata().data()) {
    std::clog << std::hex << static_cast<unsigned>(byte);
  }
  std::clog.put('\n');
  node.List([](auto n, auto c) {
    std::clog << "  " << n << " => " << c << '\n';
    return true;
  });
  blocks.Store(api, file_name, std::move(node));
}
void resolve_unixfs_path(std::string cid, std::string path) {
  api->out_fn = path;
  std::replace(api->out_fn.begin(), api->out_fn.end(), '/', '_');
  auto resolver = std::make_shared<ipfs::UnixFsPathResolver>(blocks, cid, path);
  resolver->Step(api);
  if (resolver->waiting_on().size()) {
    resolver->Step(api);
  }
}
