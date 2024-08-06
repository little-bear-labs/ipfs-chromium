#ifndef IPFS_IPFS_REQUEST_H_
#define IPFS_IPFS_REQUEST_H_

#include <ipfs_client/response_semantic.h>
#include <vocab/slash_delimited.h>

#include <functional>
#include <memory>
#include <string>

namespace ipfs {
struct Response;
class IpfsRequest {
 public:
  using Finisher = std::function<void(IpfsRequest const&, Response const&)>;

 private:
  std::string path_;
  Finisher callback_;
  ResponseSemantic semantic_;

 public:
  IpfsRequest(std::string path, Finisher);

  SlashDelimited path() const { return SlashDelimited{path_}; }
  ResponseSemantic semantic() const { return semantic_; }
  bool done() const;

  IpfsRequest& semantic(std::string_view);
  void finish(Response& r);
  void new_path(std::string_view);

  static std::shared_ptr<IpfsRequest> fromUrl(std::string url, Finisher);
};
}  // namespace ipfs

#endif  // IPFS_IPFS_REQUEST_H_
