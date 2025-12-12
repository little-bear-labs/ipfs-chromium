#include "cache_requestor.h"

#include "chromium_ipfs_context.h"
#include "inter_request_state.h"

#include <base/timer/timer.h>
#include <net/http/http_response_headers.h>

using Self = ipfs::CacheRequestor;
namespace dc = disk_cache;

namespace net {
  class CacheEncryptionDelegate; // In case you're in a version where this type does not exist
}

namespace {
  using old_signature = disk_cache::BackendResult (*)(
      net::CacheType,
      net::BackendType,
      scoped_refptr<disk_cache::BackendFileOperationsFactory>,
      base::FilePath const&,
      int64_t,
      disk_cache::ResetHandling ,
      net::NetLog*,
      disk_cache::BackendResultCallback
    );
  using new_signature = disk_cache::BackendResult (*)(
      net::CacheType,
      net::BackendType,
      scoped_refptr<disk_cache::BackendFileOperationsFactory>,
      base::FilePath const&,
      int64_t,
      disk_cache::ResetHandling ,
      net::NetLog*,
      net::CacheEncryptionDelegate*,
      disk_cache::BackendResultCallback
    );

  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunused-function"

    disk_cache::BackendResult create_backend_compatibly(Self* assign_to, base::FilePath path, old_signature real_create) {
      return real_create(
        net::CacheType::DISK_CACHE, net::CACHE_BACKEND_DEFAULT, {}, path, 0,
        dc::ResetHandling::kResetOnError, nullptr,
        base::BindOnce(&Self::Assign, base::Unretained(assign_to))
      );
    }
    disk_cache::BackendResult create_backend_compatibly(Self* assign_to, base::FilePath path, new_signature real_create) {
      return real_create(
        net::CacheType::DISK_CACHE, net::CACHE_BACKEND_DEFAULT, {}, path, 0,
        dc::ResetHandling::kResetOnError, nullptr,
        nullptr,
        base::BindOnce(&Self::Assign, base::Unretained(assign_to))
      );
    }

  #pragma clang diagnostic pop

}

std::string_view Self::name() const {
  return "Disk Cache";
}
Self::CacheRequestor(InterRequestState& state, base::FilePath base)
    // : state_{state}
{
  if (!base.empty()) {
    path_ = base.AppendASCII("IpfsBlockCache");
  }
  Start();
}
void Self::Start() {
  if (startup_pending_) {
    return;
  }
  auto result = create_backend_compatibly(this, path_, &dc::CreateCacheBackend);
  startup_pending_ = result.net_error == net::ERR_IO_PENDING;
  if (!startup_pending_) {
    Assign(std::move(result));
  }
}
Self::~CacheRequestor() noexcept = default;

void Self::Assign(dc::BackendResult res) {
  startup_pending_ = false;
  if (res.net_error == net::OK) {
    cache_.swap(res.backend);
  } else {
    LOG(ERROR) << "Trouble opening " << name() << ": " << res.net_error;
    Start();
  }
}
auto Self::handle(RequestPtr req) -> HandleOutcome {
  if (req->type == gw::GatewayRequestType::Car) {
    req->Hook([this](auto key, ByteView bytes, ipld::BlockSource const& src) {
      Store(std::string{key}, src.Serialize(), bytes);
    });
    return HandleOutcome::NOT_HANDLED;
  }
  if (startup_pending_ || !(req->cachable())) {
    return HandleOutcome::NOT_HANDLED;
  }
  Task task;
  task.key = req->root_component();
  task.request = req;
  StartFetch(task, net::MAXIMUM_PRIORITY);
  return HandleOutcome::PENDING;
}
void Self::StartFetch(Task& task, net::RequestPriority priority) {
  if (startup_pending_) {
    Start();
    Miss(task);
    return;
  }
  auto bound = base::BindOnce(&Self::OnOpen, base::Unretained(this), task);
  auto res = cache_->OpenEntry(task.key, priority, std::move(bound));
  if (res.net_error() != net::ERR_IO_PENDING) {
    OnOpen(task, std::move(res));
  }
}
void Self::Miss(Task& task) {
  if (task.request) {
    auto req = task.request;
    task.request->Hook([this](std::string_view key, ByteView bytes,
                              ipld::BlockSource const& src) {
      Store(std::string{key}, src.Serialize(), bytes);
    });
    forward(req);
  }
}
namespace {
std::shared_ptr<dc::Entry> GetEntry(dc::EntryResult& result) {
  auto* e = result.ReleaseEntry();
  auto deleter = [](auto e) {
    if (e) {
      e->Close();
    }
  };
  return {e, deleter};
}
}  // namespace

void Self::OnOpen(Task task, dc::EntryResult res) {
  if (res.net_error() != net::OK) {
    Miss(task);
    return;
  }
  task.entry = GetEntry(res);
  DCHECK(task.entry);
  task.buf = base::MakeRefCounted<net::IOBufferWithSize>(2 * 1024 * 1024);
  DCHECK(task.buf);
  auto bound =
      base::BindOnce(&Self::OnHeaderRead, base::Unretained(this), task);
  auto code = task.entry->ReadData(0, 0, task.buf.get(), task.buf->size(),
                                   std::move(bound));
  if (code != net::ERR_IO_PENDING) {
    OnHeaderRead(task, code);
  }
}
void Self::OnHeaderRead(Task task, int code) {
  if (code <= 0) {
    LOG(ERROR) << "Failed to read headers for entry " << task.key << " in "
               << name() << " " << code;
    //    Miss(task);
    //    return;
  }
  task.header.assign(task.buf->data(), static_cast<std::size_t>(code));
  task.orig_src.Deserialize({task.buf->data(), static_cast<std::size_t>(code)});
  auto bound = base::BindOnce(&Self::OnBodyRead, base::Unretained(this), task);
  code = task.entry->ReadData(1, 0, task.buf.get(), task.buf->size(),
                              std::move(bound));
  if (code != net::ERR_IO_PENDING) {
    OnBodyRead(task, code);
  }
}
void Self::OnBodyRead(Task task, int code) {
  if (code <= 0) {
    Miss(task);
    return;
  }
  task.body.assign(task.buf->data(), static_cast<std::size_t>(code));
  if (task.request) {
    task.orig_src.load_duration = std::chrono::system_clock::now() - task.start;
    task.orig_src.cat.cached = true;
    bool valid = false;
    task.request->RespondSuccessfully(task.body, api_, task.orig_src, "", &valid);
    if (!valid) {
      VLOG(2) << "Had a bad or expired cached response for " << task.key;
      Expire(task.key);
      Miss(task);
    }
  }
}
void Self::Store(std::string key, std::string headers, ByteView body) {
  std::string body_s{reinterpret_cast<char const*>(body.data()), body.size()};
  auto bound = base::BindOnce(&Self::OnEntryCreated, base::Unretained(this),
                              key, headers, body_s);
  auto res = cache_->OpenOrCreateEntry(key, net::LOW, std::move(bound));
  if (res.net_error() != net::ERR_IO_PENDING) {
    OnEntryCreated(key, headers, body_s, std::move(res));
  }
}
void Self::OnEntryCreated(std::string cid,
                          std::string headers,
                          std::string body,
                          disk_cache::EntryResult result) {
  if (result.opened()) {
    // No need to write this entry as it is already there and immutable.";
  } else if (result.net_error() == net::OK) {
    auto entry = GetEntry(result);
    auto buf = base::MakeRefCounted<net::StringIOBuffer>(headers);
    DCHECK(buf);
    auto bound = base::BindOnce(&Self::OnHeaderWritten, base::Unretained(this),
                                buf, body, entry);
    auto code =
        entry->WriteData(0, 0, buf.get(), buf->size(), std::move(bound), true);
    if (code != net::ERR_IO_PENDING) {
      OnHeaderWritten(buf, body, entry, code);
    }
  } else {
    LOG(ERROR) << "Failed to create an entry for " << cid << " in " << name();
  }
}
void Self::OnHeaderWritten(scoped_refptr<net::StringIOBuffer> buf,
                           std::string body,
                           std::shared_ptr<disk_cache::Entry> entry,
                           int code) {
  if (code < 0) {
    LOG(ERROR) << "Failed to write header info for " << entry->GetKey()
               << " in " << name();
    return;
  }
  buf = base::MakeRefCounted<net::StringIOBuffer>(body);
  DCHECK(buf);
  auto f = [](scoped_refptr<net::StringIOBuffer>, int) {};
  auto bound = base::BindOnce(f, buf);
  entry->WriteData(1, 0, buf.get(), buf->size(), std::move(bound), true);
}

void Self::Expire(std::string const& key) {
  if (cache_ && !startup_pending_) {
    cache_->DoomEntry(key, net::RequestPriority::LOWEST, base::DoNothing());
  }
}

Self::Task::Task() = default;
Self::Task::Task(Task const&) = default;
Self::Task::~Task() noexcept = default;
