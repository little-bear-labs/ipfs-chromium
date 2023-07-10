#include "summarize_headers.h"

#include "base/logging.h"
#include "services/network/public/mojom/url_response_head.mojom.h"

#include <ipfs_client/block_storage.h>

void ipfs::summarize_headers(std::vector<std::string> const& cids,
                             std::string const& root,
                             net::HttpResponseHeaders& out,
                             BlockStorage& storage) {
  LOG(INFO) << std::time(nullptr);
  std::map<std::string, std::size_t> bytes_per;
  std::map<std::string, std::size_t> blocks_per;
  std::map<std::string, std::size_t> millis_per;
  for (auto& cid : cids) {
    auto* raw = storage.GetHeaders(cid);
    if (!raw || raw->empty()) {
      LOG(ERROR) << "Trouble fetching headers from gateway response " << cid;
      return;
    }
    auto gw_heads = base::MakeRefCounted<net::HttpResponseHeaders>(*raw);
    if (!gw_heads) {
      std::ostringstream escaped;
      for (auto c : *raw) {
        if (std::isgraph(c)) {
          escaped << c;
        } else {
          escaped << '<' << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned>(c) << '>';
        }
      }
      LOG(ERROR) << "Failed to parse raw string as headers for " << cid << " : "
                 << escaped.str();
      return;
    }
    std::size_t i = 0UL;
    std::string name, value;
    while (gw_heads->EnumerateHeaderLines(&i, &name, &value)) {
      if (name == "Server-Timing" && cid == root) {
        auto dur = value.find("dur=");
        if (dur < value.size()) {
          out.AddHeader(name, "ipfs-ttfb;desc=\"til first block(root)\";" +
                                  value.substr(dur));
        } else {
          LOG(ERROR) << "Server-Timing with no 'dur=': '" << value << "'.";
        }
      } else if (name == "Server-Timing") {
        auto desc = value.find(";desc=\"");
        if (desc >= value.size()) {
          LOG(WARNING) << "Server-Timing with no desc";
          continue;
        }
        auto dur = value.find(";dur=");
        if (dur >= value.size()) {
          LOG(WARNING) << "Server-Timing header with no dur on CID " << cid
                       << " (from HTTP) : " << value;
          continue;
        }
        std::string gw = "cache";
        auto load = value.find(" : load over http(s)", desc);
        if (load < value.size()) {
          auto start = desc + 7;
          auto len = load - start;
          gw = value.substr(start, len);
        }
        auto ms = std::atol(value.c_str() + dur + 5);
        millis_per[gw] += ms;
      } else if (name == "Block-Source") {
        auto comma = value.find(", ");
        if (comma >= value.size()) {
          continue;
        }
        auto at = value.find(" @", comma);
        if (at >= value.size()) {
          continue;
        }
        auto start = comma + 2;
        auto len = at - start;
        auto gw = value.substr(start, len);
        blocks_per[gw]++;
        bytes_per[gw] += storage.Get(cid)->unparsed().size();
      } else if (!name.find("Block-Cache-")) {
        blocks_per["cache"]++;
        bytes_per["cache"] += storage.Get(cid)->unparsed().size();
      } else {
        //        LOG(INFO) << "Dropping header '" << name << "' for " << cid;
      }
    }
  }
  for (auto [gateway, block_count] : blocks_per) {
    std::ostringstream val;
    val << gateway << ";blocks=" << block_count;
    auto it = bytes_per.find(gateway);
    if (it != bytes_per.end()) {
      val << ";bytes=" << it->second;
    }
    it = millis_per.find(gateway);
    if (it != millis_per.end()) {
      val << ";cdur=" << it->second;
    }
    VLOG(1) << "Add header Ipfs-Block-Source: " << val.str();
    out.AddHeader("Ipfs-Block-Source", val.str());
  }
}
