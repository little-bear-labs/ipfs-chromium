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
          out.AddHeader(
              name, "ipfs-ttfb;desc=\"time til the first block was fetched\";" +
                        value.substr(dur));
        } else {
          LOG(ERROR) << "Server-Timing with no 'dur=': '" << value << "'.";
        }
      } else if (name == "Server-Timing" && !value.find("blockcache-")) {
        auto dur = value.find(";dur=");
        if (dur >= value.size()) {
          LOG(WARNING) << "Server-Timing header with no dur on CID " << cid
                       << " : " << value;
          continue;
        }
        auto ms = std::atol(value.c_str() + dur + 5);
        millis_per["cache"] += ms;
        VLOG(1) << "cache MS " << ms << " from " << value << " += -> "
                << millis_per["cache"];
      } else if (name == "Server-Timing") {
        auto desc = value.find(";desc=\"");
        if (desc >= value.size()) {
          continue;
        }
        auto load = value.find(" : load over http(s)", desc);
        if (load >= value.size()) {
          continue;
        }
        auto dur = value.find(";dur=", load);
        if (dur >= value.size()) {
          LOG(WARNING) << "Server-Timing header with no dur on CID " << cid
                       << " (from HTTP) : " << value;
          continue;
        }
        auto start = desc + 7;
        auto len = load - start;
        auto gw = value.substr(start, len);
        auto ms = std::atol(value.c_str() + dur + 5);
        LOG(INFO) << "MS " << ms << " from " << value;
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
  LOG(INFO) << std::time(nullptr);
  for (auto [gw, ms] : millis_per) {
    LOG(INFO) << "millis_per " << gw << ' ' << ms << "ms.";
    auto nm = gw;
    auto slashes = nm.find("//");
    if (slashes < nm.size()) {
      nm.erase(0UL, slashes + 2);
    }
    nm.erase(std::remove_if(nm.begin(), nm.end(),
                            [](auto c) { return !std::isalnum(c); }),
             nm.end());
    std::ostringstream server_timing_summary;
    server_timing_summary << nm << ";desc=\"fetching " << blocks_per[gw]
                          << " blocks (" << bytes_per[gw]
                          << " bytes in size) from " << gw << "\";dur=" << ms;
    LOG(INFO) << server_timing_summary.str();
    out.AddHeader("Server-Timing", server_timing_summary.str());
  }
  LOG(INFO) << std::time(nullptr);
}
