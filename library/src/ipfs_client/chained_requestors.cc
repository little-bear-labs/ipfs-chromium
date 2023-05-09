#include <ipfs_client/chained_requestors.h>

#include <ipfs_client/dag_listener.h>

#include <vocab/raw_ptr.h>

#include "log_macros.h"

using Self = ipfs::ChainedRequestors;

void Self::Add(Ptr p) {
  chain_.push_back(p);
}
bool Self::Valid() const {
  return !chain_.empty();
}
void Self::RequestByCid(std::string cid,
                        std::shared_ptr<DagListener> listen,
                        Priority priority) {
  struct NextChainLinkListener : public ipfs::DagListener {
    Priority prio;
    std::shared_ptr<DagListener> real;
    std::weak_ptr<NextChainLinkListener> me;
    raw_ptr<Self> chain;
    std::size_t index = 0UL;
    std::string cid;
    void ReceiveBlockBytes(std::string_view b) override {
      real->ReceiveBlockBytes(b);
    }
    void BlocksComplete(std::string mime_type) override {
      LOG(INFO) << "L" << (index + 1) << ": hit" << cid;
      real->BlocksComplete(mime_type);
    }
    void FourOhFour(std::string_view c, std::string_view path) override {
      DCHECK_EQ(c, cid);
      if (++index < chain->chain_.size()) {
        LOG(INFO) << cid << " / " << path << " missed on approach #" << index;
        auto next = chain->chain_.at(index);
        next->RequestByCid(std::string{cid}, me.lock(), prio);
      } else {
        LOG(ERROR) << cid << " / " << path << " has failed completely.";
        real->FourOhFour(cid, path);
      }
    }
    NextChainLinkListener(Self* c) : chain{c} {}
    virtual ~NextChainLinkListener() noexcept = default;
  };
  auto chainer = std::make_shared<NextChainLinkListener>(this);
  chainer->prio = priority;
  chainer->real = listen;
  chainer->cid = cid;
  chainer->me = chainer;
  chain_.at(0UL)->RequestByCid(cid, chainer, priority);
}

Self::ChainedRequestors() = default;
Self::~ChainedRequestors() noexcept = default;
