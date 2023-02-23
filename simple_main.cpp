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
class StubbedApi : public ipfs::FrameworkApi {};
}  // namespace
void parse_block_file(char const* file_name) {
  std::ifstream file{file_name};
  ipfs::Block node{file};
  if (!node.valid()) {
    std::clog << "Failed to parse '" << file_name << "'\n";
    return;
  }
  std::clog << "Loaded: " << file_name << ' ' << node.type() << ' '
            << node.file_size() << ' ' << '\n';
  node.List([](auto n, auto c) {
    std::clog << "  " << n << " => " << c << '\n';
    return true;
  });
  // TODO
  //  blocks.Store(file_name, std::move(node));
}
void resolve_unixfs_path(std::string cid, std::string path) {
  auto request_required = [](auto& c) {
    std::clog << "Unsatisfied required CID: " << c << ' ';
    if (std::filesystem::is_regular_file(c)) {
      std::clog << "It exists on-disk. Load it next time.\n";
    } else {
      std::clog << "Attempting to fetch it from Kubo.\n";
      auto cmd = "curl 'http://chomp:8080/ipfs/" + c;
      cmd.append("?format=raw' > ").append(c);
      std::system(cmd.c_str());
    }
  };
  auto request_prefetch = [](auto& c) {
    std::clog << "Consider prefetching " << c << '\n';
  };
  auto receive_bytes = [](auto& s) {
    std::clog << "Receiving " << s.size() << " bytes.\n";
    file_contents.append(s);
  };
  auto out_fn = std::filesystem::path{path}.filename().string();
  auto on_complete = [out_fn](auto type) {
    std::clog << "Got " << type << ". File contents (" << file_contents.size()
              << " B). Writing to " << out_fn << '\n';
    std::ofstream f{out_fn};
    f.write(file_contents.c_str(), file_contents.size());
  };
  // TODO
  //   auto resolver = std::make_shared<ipfs::UnixFsPathResolver>(blocks, cid,
  //   path, request_required, request_prefetch, receive_bytes,on_complete);
  //   resolver->Step(resolver);
}
