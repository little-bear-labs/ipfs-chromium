#ifndef IPFS_IPFS_REQUEST_H_
#define IPFS_IPFS_REQUEST_H_

#include <vocab/slash_delimited.h>

#include <functional>
#include <string>

namespace ipfs {
struct Response;
class IpfsRequest {
 public:
  using Finisher = std::function<void(IpfsRequest const&, Response const&)>;

 private:
  std::string path_;
  Finisher callback_;

 public:
  IpfsRequest(std::string path, Finisher);
  SlashDelimited path() const { return std::string_view{path_}; }
  void finish(Response& r);
};
}  // namespace ipfs

#endif  // IPFS_IPFS_REQUEST_H_
