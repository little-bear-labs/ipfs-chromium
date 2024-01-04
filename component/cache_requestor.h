#ifndef CACHE_REQUESTOR_H_
#define CACHE_REQUESTOR_H_

#include <net/base/cache_type.h>
#include <net/base/io_buffer.h>
#include <net/disk_cache/disk_cache.h>

#include <base/memory/raw_ref.h>
#include <base/memory/scoped_refptr.h>
#include <base/time/time.h>

#include <ipfs_client/gw/requestor.h>

#include <memory>

namespace ipfs {

class BlockStorage;
class InterRequestState;

class CacheRequestor : public gw::Requestor {
 public:
  CacheRequestor(InterRequestState&, base::FilePath);
  ~CacheRequestor() noexcept override;
  void Store(std::string key, std::string headers, std::string body);
  void Expire(std::string const& key);

  std::string_view name() const override;

 private:
  struct Task {
    Task();
    Task(Task const&);
    ~Task() noexcept;
    std::string key;
    base::TimeTicks start = base::TimeTicks::Now();
    std::string header;
    std::string body;
    scoped_refptr<net::IOBufferWithSize> buf;
    std::shared_ptr<disk_cache::Entry> entry;
    gw::RequestPtr request;

    void SetHeaders(std::string_view);
  };
  raw_ref<InterRequestState> state_;
  std::unique_ptr<disk_cache::Backend> cache_;
  bool startup_pending_ = false;
  base::FilePath path_;

  void Start();

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
  void Miss(Task&);
  HandleOutcome handle(RequestPtr) override;
};
}  // namespace ipfs

#endif  // CACHE_REQUESTOR_H_
