#ifndef CACHE_REQUESTOR_H_
#define CACHE_REQUESTOR_H_

#include <net/base/cache_type.h>
#include <net/base/io_buffer.h>
#include <net/disk_cache/disk_cache.h>

#include <base/memory/scoped_refptr.h>
#include <base/time/time.h>

#include <ipfs_client/block_requestor.h>

#include <memory>

namespace ipfs {

class BlockStorage;
class InterRequestState;

class CacheRequestor : public BlockRequestor {
 public:
  CacheRequestor(net::CacheType, InterRequestState&);
  virtual ~CacheRequestor() noexcept;
  void Store(std::string cid, std::string headers, std::string body);
  void FetchEntry(std::string key,
                net::RequestPriority priority,
                std::function<void(std::string_view, std::string_view)> hit,
                std::function<void()> miss);

  std::string_view name() const;

 private:
  struct Task {
    Task();
    Task(Task const&);
    ~Task() noexcept;
    std::string key;
    std::shared_ptr<DagListener> listener;
    base::TimeTicks start = base::TimeTicks::Now();
    std::string header;
    std::string body;
    scoped_refptr<net::IOBufferWithSize> buf;
    std::shared_ptr<disk_cache::Entry> entry;
    std::function<void(std::string_view, std::string_view)> hit;
    std::function<void()> miss;

    void SetHeaders(std::string_view);
    void Fail();
  };
  net::CacheType const type_;
  InterRequestState& state_;
  std::unique_ptr<disk_cache::Backend> cache_;
  bool pending_ = false;

  void RequestByCid(std::string cid,
                    std::shared_ptr<DagListener>,
                    Priority) override;

  void Start();

  base::FilePath path() const;
  void StartFetch(Task& t, net::RequestPriority priority);
  void Assign(disk_cache::BackendResult);
  void OnOpen(Task, disk_cache::EntryResult);
  void OnHeaderRead(Task, int);
  void OnBodyRead(Task, int);

  void OnEntryCreated(std::string c,
                      std::string h,
                      std::string b,
                      disk_cache::EntryResult);
  void OnHeaderWritten(scoped_refptr<net::StringIOBuffer> buf,
                       std::string body,
                       std::shared_ptr<disk_cache::Entry> entry,
                       int);
};
}  // namespace ipfs

#endif  // CACHE_REQUESTOR_H_
