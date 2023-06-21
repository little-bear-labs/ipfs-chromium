#include "cache_requestor.h"

#include "gateway_requests.h"
#include "inter_request_state.h"

#include <base/timer/timer.h>
#include <net/http/http_response_headers.h>

#include <ipfs_client/dag_listener.h>

using Self = ipfs::CacheRequestor;
namespace dc = disk_cache;

Self::CacheRequestor(net::CacheType typ,
                     InterRequestState& state,
                     base::FilePath base)
    : type_{typ}, state_{state} {
  if (!base.empty()) {
    path_ = base.AppendASCII("IpfsBlockCache");
  }
  Start();
}
void Self::Start() {
  if (pending_) {
    return;
  }
  auto result = dc::CreateCacheBackend(
      type_, net::CACHE_BACKEND_DEFAULT, {}, path_, 0,
      dc::ResetHandling::kNeverReset,
      //     dc::ResetHandling::kResetOnError,
      nullptr, base::BindOnce(&Self::Assign, base::Unretained(this)));
  LOG(INFO) << "Start(" << result.net_error << ')' << result.net_error;
  pending_ = result.net_error == net::ERR_IO_PENDING;
  if (!pending_) {
    Assign(std::move(result));
  }
}
Self::~CacheRequestor() noexcept = default;

void Self::Assign(dc::BackendResult res) {
  pending_ = false;
  if (res.net_error == net::OK) {
    LOG(INFO) << "Initialized a cache of type " << name();
    cache_.swap(res.backend);
  } else {
    LOG(ERROR) << "Trouble opening " << name() << ": " << res.net_error;
    Start();
  }
}
void Self::FetchEntry(
    std::string key,
    net::RequestPriority priority,
    std::function<void(std::string_view, std::string_view)> hit,
    std::function<void()> miss) {
  Task task;
  task.key = key;
  task.hit = hit;
  task.miss = miss;
  StartFetch(task, priority);
}
void Self::RequestByCid(std::string cid,
                        std::shared_ptr<DagListener> listen,
                        Priority prio) {
  DCHECK(listen);
  Task task;
  task.key = cid;
  task.listener = listen;
  auto p = std::min(prio + 0, net::MAXIMUM_PRIORITY + 0);
  StartFetch(task, static_cast<net::RequestPriority>(p));
}
void Self::StartFetch(Task& task, net::RequestPriority priority) {
  if (pending_) {
    Start();
    task.Fail();
    return;
  }
  auto bound = base::BindOnce(&Self::OnOpen, base::Unretained(this), task);
  auto res = cache_->OpenEntry(task.key, priority, std::move(bound));
  if (res.net_error() != net::ERR_IO_PENDING) {
    OnOpen(task, std::move(res));
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
  VLOG(2) << "OnOpen(" << res.net_error() << ")";
  if (res.net_error() != net::OK) {
    VLOG(2) << "Failed to find " << task.key << " in " << name();
    task.Fail();
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
               << name();
    task.Fail();
    return;
  }
  task.header.assign(task.buf->data(), static_cast<std::size_t>(code));
  auto bound = base::BindOnce(&Self::OnBodyRead, base::Unretained(this), task);
  code = task.entry->ReadData(1, 0, task.buf.get(), task.buf->size(),
                              std::move(bound));
  if (code != net::ERR_IO_PENDING) {
    OnBodyRead(task, code);
  }
}
void Self::OnBodyRead(Task task, int code) {
  if (code <= 0) {
    LOG(ERROR) << "Failed to read body for entry " << task.key << " in "
               << name();
    task.Fail();
    return;
  }
  task.body.assign(task.buf->data(), static_cast<std::size_t>(code));
  if (task.listener) {
    VLOG(1) << "Cache hit on " << task.key;
    task.SetHeaders(name());
    auto& stor = state_.storage();
    stor.Store(task.key, std::move(task.header), std::move(task.body));
    state_.scheduler().IssueRequests(state_.api());
  } else {
    task.hit(task.body, task.header);
  }
}

void Self::Store(std::string cid, std::string headers, std::string body) {
  VLOG(2) << "Store(" << name() << ',' << cid << ',' << headers.size() << ','
          << body.size() << ')';
  auto bound = base::BindOnce(&Self::OnEntryCreated, base::Unretained(this),
                              cid, headers, body);
  auto res = cache_->OpenOrCreateEntry(cid, net::LOW, std::move(bound));
  if (res.net_error() != net::ERR_IO_PENDING) {
    OnEntryCreated(cid, headers, body, std::move(res));
  }
}
void Self::OnEntryCreated(std::string cid,
                          std::string headers,
                          std::string body,
                          disk_cache::EntryResult result) {
  if (result.opened()) {
    VLOG(1) << "No need to write an entry for " << cid << " in " << name()
            << " as it is already there and immutable.";
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
  auto f = [](scoped_refptr<net::StringIOBuffer>, int c) {
    VLOG(1) << "body write " << c;
  };
  auto bound = base::BindOnce(f, buf);
  entry->WriteData(1, 0, buf.get(), buf->size(), std::move(bound), true);
}

std::string_view Self::name() const {
  switch (type_) {
    case net::CacheType::DISK_CACHE:
      return "disk";
    case net::CacheType::MEMORY_CACHE:
      return "memory";
    default:
      return "other";
  }
}

void Self::Task::Fail() {
  VLOG(2) << "TaskFail for key: " << key;
  if (listener) {
    listener->NotHere(key, "<any/all>");
  }
  if (miss) {
    miss();
  }
}
void Self::Task::SetHeaders(std::string_view source) {
  auto heads = base::MakeRefCounted<net::HttpResponseHeaders>(header);
  DCHECK(heads);
  std::string value{"blockcache-"};
  value.append(key);
  value.append(";desc=\"Load from local browser block cache\";dur=");
  auto dur = base::TimeTicks::Now() - start;
  value.append(std::to_string(dur.InMillisecondsRoundedUp()));
  heads->SetHeader("Server-Timing", value);
  VLOG(1) << "From cache: Server-Timing: " << value << "; Block-Cache-" << key
          << ": " << source;
  heads->SetHeader("Block-Cache-" + key, {source.data(), source.size()});
  header = heads->raw_headers();
}
void Self::Expire(std::string const& key) {
  if (cache_ && !pending_) {
    cache_->DoomEntry(key, net::RequestPriority::LOWEST, base::DoNothing());
  }
}

Self::Task::Task() = default;
Self::Task::Task(Task const&) = default;
Self::Task::~Task() noexcept = default;
