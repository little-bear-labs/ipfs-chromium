#ifndef IPFS_CHROMIUM_DAG_HEADERS_H
#define IPFS_CHROMIUM_DAG_HEADERS_H

#include <ipfs_client/ipld/block_source.h>

#include <map>
#include <string>
#include <vector>

namespace ipfs::ipld {
/*! Response headers coming from the IPLD DAG
 */
class DagHeaders {
 public:
  void Add(BlockSource const&);
  void Finish();

  /*! A list of HTTP header names and values
   */
  using HeaderList = std::vector<std::pair<std::string, std::string>>;

  HeaderList const& headers() const { return headers_; }

 private:
  std::map<BlockSource::Category, short> individual_counts_;
  HeaderList headers_;
  std::size_t other_count_ = {};
  BlockSource::Clock::duration other_sum_ = std::chrono::milliseconds(0);
};
}  // namespace ipfs::ipld

#endif  // IPFS_CHROMIUM_DAG_HEADERS_H
