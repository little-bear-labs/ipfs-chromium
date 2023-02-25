#include <ipfs_client/block_storage.h>
#include <ipfs_client/cid_util.h>
#include <ipfs_client/framework_api.h>
#include <ipfs_client/unixfs_path_resolver.h>

#include <libp2p/multi/multibase_codec/codecs/base58.hpp>

#include <smhasher/MurmurHash3.h>

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

void show_hash(std::string_view next) {
  std::array<std::uint64_t, 2> digest = {0U, 0U};
  // Rust's fastmurmur3 also passes 0 for seed, and iroh uses that.
  MurmurHash3_x64_128(next.data(), next.size(), 0, digest.data());
  auto bug_compatible_digest = htobe64(digest[0]);
  std::clog << next << " Hash: " << std::hex << digest[0] << ' ' << digest[1]
            << " -> " << bug_compatible_digest << '\n';
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
  bool got_ = false;
  void BlocksComplete(std::string type) {
    std::clog << "Got " << type << ". File contents (" << file_contents.size()
              << " B). Writing to " << out_fn << '\n';
    std::ofstream f{out_fn};
    f.write(file_contents.c_str(), file_contents.size());
    file_contents.clear();
    got_ = true;
  }

  std::string MimeType(std::string extension,
                       std::string_view content,
                       std::string const& url) const {
    if (content.find("<html>")) {
      return "text/html";
    }
    return "";
  }
  std::string UnescapeUrlComponent(std::string_view s) const {
    std::string t{s};
    for (auto i = 0;
         i < t.size() && (i = t.find('%', i)) != std::string::npos;) {
      auto cp = 0U;
      std::istringstream iss{t.substr(i + 1, 2)};
      iss >> std::hex >> cp;
      std::clog << t.substr(i, 3) << " becomes codepoint "
                << static_cast<unsigned>(cp) << '\n';
      t.erase(i + 1, 2);
      t[i] = cp;
      ++i;
    }
    return t;
  }
  void FourOhFour(std::string_view cid, std::string_view path) {
    std::clog << "DAG 404 on " << cid << " , " << path << '\n';
    std::exit(9);
  }
};
auto api = std::make_shared<StubbedApi>();
}  // namespace
void parse_block_file(char const* file_name) {
  std::clog << "Will attempt to load from file " << file_name << '\n';
  std::ifstream file{file_name};
  ipfs::Block node{cid::bin::get_multicodec(cid::mb::to_bin(file_name)), file};
  if (!node.valid()) {
    std::clog << "Failed to parse '" << file_name << "'\n";
    return;
  }
  std::clog << "Loaded: " << file_name << ' ' << node.type() << ' '
            << node.file_size() << "\n  data:";
  auto i = 0;
  for (auto byte : node.fsdata().data()) {
    if (std::isgraph(byte) || byte == ' ') {
      std::clog.put(byte);
    } else {
      std::clog << '<' << std::setw(2) << std::setfill('0') << std::hex
                << static_cast<unsigned>(byte) << '>';
    }
    if (i++ > 999) {
      break;
    }
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
  file_contents.clear();
  api->got_ = false;
  std::replace(api->out_fn.begin(), api->out_fn.end(), '/', '_');
  auto resolver = std::make_shared<ipfs::UnixFsPathResolver>(blocks, cid, path);
  resolver->Step(api);
  if (resolver->waiting_on().size()) {
    resolver->Step(api);
  }
  if (file_contents.empty()) {
    resolver->Step(api);
  }
  if (!api->got_) {
    resolver->Step(api);
  }
}
