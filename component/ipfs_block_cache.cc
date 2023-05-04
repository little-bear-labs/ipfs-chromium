#include "ipfs_block_cache.h"

#include <ipfs_client/block_storage.h>

#include <libp2p/multi/content_identifier_codec.hpp>

#include <base/logging.h>

using CidCodec = libp2p::multi::ContentIdentifierCodec;

namespace {
void Assign(std::unique_ptr<disk_cache::Backend>* p,
            bool* pending,
            disk_cache::BackendResult r) {
  if (r.backend && r.net_error == net::OK) {
    p->swap(r.backend);
    LOG(INFO) << "Got a disk cache!" << (void*)(p->get());
  } else {
    *pending = false;
    LOG(ERROR) << "Disc cache creation failed: " << r.net_error;
  }
}
void ParamRelease(scoped_refptr<net::StringIOBuffer>, int) {}
auto GetEntry(auto& result) {
  auto* e = result.ReleaseEntry();
  return std::shared_ptr<disk_cache::Entry>{e, [](auto e) {
                                              if (e) {
                                                e->Close();
                                              }
                                            }};
}
void WriteEntry(std::string_view cid,
                std::string_view to,
                std::string headers,
                std::string block,
                disk_cache::EntryResult res) {
  LOG(INFO) << "Write " << cid << " with " << headers.size()
            << " bytes of header, and " << block.size() << " bytes of body to "
            << to;
  auto entry = GetEntry(res);
  if (entry) {
    auto hbuf = base::MakeRefCounted<net::StringIOBuffer>(headers);
    entry->WriteData(0, 0, hbuf.get(), hbuf->size(),
                     base::BindOnce(ParamRelease, hbuf), true);
    auto bbuf = base::MakeRefCounted<net::StringIOBuffer>(block);
    entry->WriteData(1, 0, bbuf.get(), bbuf->size(),
                     base::BindOnce(ParamRelease, bbuf), true);
  }
}
}  // namespace

disk_cache::Backend* ipfs::BlockCache::dsk() {
  if (!dsk_ && !disk_cache_pending_) {
    disk_cache_pending_ = true;
    LOG(INFO) << "Try to load disk cache.";
    auto disk = disk_cache::CreateCacheBackend(
        net::DISK_CACHE, net::CACHE_BACKEND_DEFAULT, {},
        base::FilePath("/home/lbl/.cache/chromium/Default/Cache/ipfs"), 0,
        disk_cache::ResetHandling::kResetOnError, nullptr,
        base::BindOnce(Assign, &dsk_, &disk_cache_pending_));
    if (disk.net_error != net::ERR_IO_PENDING) {
      LOG(INFO) << "Got an immediate response to creation of a disk cache.";
      Assign(&dsk_, &disk_cache_pending_, std::move(disk));
    }
  }
  return dsk_.get();
}
disk_cache::Backend* ipfs::BlockCache::mem() {
  if (!mem_) {
    auto memory = disk_cache::CreateCacheBackend(
        net::MEMORY_CACHE, net::CACHE_BACKEND_DEFAULT, {}, base::FilePath(), 0,
        disk_cache::ResetHandling::kResetOnError, nullptr,
        base::BindOnce([](disk_cache::BackendResult) {}));
    if (memory.net_error == net::OK) {
      LOG(INFO) << "Got a memory cache.";
      mem_.swap(memory.backend);
      //      assign(&mem_, &disregard, std::move(memory));
    } else {
      LOG(ERROR) << "Trouble creating a memory cache: " << memory.net_error;
    }
  }
  return mem_.get();
}

void ipfs::BlockCache::Store(std::string cid,
                             std::string headers,
                             std::string block) {
  LOG(INFO) << "BlockCache::Store(" << cid << ",headers.size()"
            << headers.size() << ",block.size()" << block.size() << ')';
  known_absent_.erase(cid);
  Store("memory", mem(), cid, headers, block);
  Store("disk", dsk(), cid, headers, block);
}

void ipfs::BlockCache::Store(std::string_view to,
                             disk_cache::Backend* be,
                             std::string cid,
                             std::string headers,
                             std::string block) {
  if (be) {
    auto res = be->OpenOrCreateEntry(
        cid, net::MAXIMUM_PRIORITY,
        base::BindOnce(WriteEntry, cid, to, headers, block));
    if (res.net_error() == net::OK) {
      WriteEntry(cid, to, headers, block, std::move(res));
    }
  } else {
    LOG(WARNING) << to << " backend did not initiaze correctly.";
  }
}
void ipfs::BlockCache::Load(std::string cid, BlockStorage* into) {
  LOG(INFO) << "Load(" << cid << ", @" << (void*)into << ')';
  if (!mem() || !dsk()) {
    LOG(INFO) << "Too early to load from cache.";
    return;
  }
  if (known_absent_.count(cid)) {
    VLOG(1) << "Not doing Load(" << cid << "): already known to be absent.";
    return;
  }
  auto noop_res = [](disk_cache::EntryResult r) {
    LOG(INFO) << "mem cache read err: " << r.net_error();
  };
  auto noop_int = [](int ec) { LOG(INFO) << "L1: ec=" << ec; };
  auto res =
      mem()->OpenEntry(cid, net::MAXIMUM_PRIORITY, base::BindOnce(noop_res));
  LOG(INFO) << "mem OpenEntry(" << cid << ")->" << res.net_error();
  if (res.opened() && res.net_error() == net::OK) {
    std::string header, body;
    auto e = GetEntry(res);
    {
      auto buf = base::MakeRefCounted<net::IOBufferWithSize>(2 * 1024 * 1024);
      e->ReadData(0, 0, buf.get(), buf->size(), base::BindOnce(noop_int));
      header.assign(buf->data(), e->GetDataSize(0));
    }
    {
      auto buf = base::MakeRefCounted<net::IOBufferWithSize>(2 * 1024 * 1024);
      e->ReadData(1, 0, buf.get(), buf->size(), base::BindOnce(noop_int));
      body.assign(buf->data(), e->GetDataSize(1));
    }
    LOG(INFO) << "L1: Loaded " << header.size() << " bytes of header and "
              << body.size() << " bytes of body for " << cid;
    into->Store(cid, std::move(header), std::move(body));
  } else if (disk_load_pending_.count(cid)) {
    LOG(WARNING) << "Not double-loading " << cid;
  } else {
    // TODO lifetime on this
    res = dsk()->OpenEntry(cid, net::MAXIMUM_PRIORITY,
                           base::BindOnce(&BlockCache::OnDiskOpen,
                                          base::Unretained(this), cid, into));
    LOG(INFO) << "dsk OpenEntry(" << cid << ")->" << res.net_error();
    if (res.opened() && res.net_error() == net::OK) {
      OnDiskOpen(cid, into, std::move(res));
    } else {
      disk_load_pending_.insert(cid);
    }
  }
}

void ipfs::BlockCache::OnDiskOpen(std::string cid,
                                  ipfs::BlockStorage* into,
                                  disk_cache::EntryResult res) {
  disk_load_pending_.erase(cid);
  if (!res.opened()) {
    LOG(INFO) << "Did not find " << cid << " in on-disk cache.";
    known_absent_.insert(cid);
    return;
  }
  LOG(INFO) << "Loading " << cid << " from disk.";
  auto e = GetEntry(res);
  if (!e) {
    LOG(ERROR) << "Entry is null for " << cid;
    return;
  }
  auto buf = base::MakeRefCounted<net::IOBufferWithSize>(2 * 1024 * 1024);
  if (!buf) {
    LOG(ERROR) << "Failed to allocate buffer.";
    return;
  }
  auto ec =
      e->ReadData(0, 0, buf.get(), buf->size(),
                  base::BindOnce(&BlockCache::OnDiskHeader,
                                 base::Unretained(this), cid, into, buf, e));
  if (ec != net::ERR_IO_PENDING) {
    OnDiskHeader(cid, into, buf, e, ec);
  }
}
void ipfs::BlockCache::OnDiskHeader(std::string cid,
                                    ipfs::BlockStorage* into,
                                    scoped_refptr<net::IOBufferWithSize> buf,
                                    std::shared_ptr<disk_cache::Entry> e,
                                    int res) {
  if (res < 0 || !e) {
    LOG(ERROR) << "Failed to read HTTP headers for " << cid
               << " from disk: " << res;
    return;
  }
  std::string header;
  header.assign(buf->data(), res);
  LOG(INFO) << "Read " << res << " bytes of headers: " << header;
  res = e->ReadData(
      1, 0, buf.get(), buf->size(),
      base::BindOnce(&BlockCache::OnDiskBody, base::Unretained(this), cid, into,
                     buf, e, header));
  if (res != net::ERR_IO_PENDING) {
    OnDiskBody(cid, into, buf, e, header, res);
  }
}
void ipfs::BlockCache::OnDiskBody(std::string cid,
                                  ipfs::BlockStorage* into,
                                  scoped_refptr<net::IOBufferWithSize> buf,
                                  std::shared_ptr<disk_cache::Entry>,
                                  std::string header,
                                  int ec) {
  if (ec < 0) {
    return;
  }
  std::string body;
  body.assign(buf->data(), ec);
  LOG(INFO) << "L2: " << cid << ' ' << ec << " bytes";
  Store("memory (from disk read)", mem(), cid, header, body);
  into->Store(cid, header, body);
}

ipfs::BlockCache::BlockCache() = default;
ipfs::BlockCache::~BlockCache() noexcept = default;
