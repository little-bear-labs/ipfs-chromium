#ifndef IPFS_GENERATED_DIRECTORY_LISTING_H_
#define IPFS_GENERATED_DIRECTORY_LISTING_H_

#include <string>
#include <string_view>

namespace ipfs {
class GeneratedDirectoryListing {
 public:
  GeneratedDirectoryListing(std::string_view base_path);

  void AddEntry(std::string_view name);
  std::string const& Finish();

 private:
  std::string html_;
  std::string base_path_;

  void AddLink(std::string_view name, std::string_view path);
};
}  // namespace ipfs

#endif  // IPFS_GENERATED_DIRECTORY_LISTING_H_
