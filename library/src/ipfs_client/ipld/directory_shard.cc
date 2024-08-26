#include "directory_shard.h"

#include "log_macros.h"

#include <ipfs_client/client.h>
#include <vocab/endian.h>

#include <smhasher/MurmurHash3.h>

#include <array>
#include <iomanip>
#include <sstream>

using namespace std::literals;

using Self = ipfs::ipld::DirShard;

namespace {
  ipfs::Response DynamicListingHtml(std::string_view path);
}

auto Self::resolve(ResolutionState& parms) -> ResolveResult {
  if (parms.IsFinalComponent()) {
    if (parms.Semantic() == ResponseSemantic::Listing) {
      return Response{"application/json", 200, listing_json(), {}, {}};
    }
    // index.html hashes A0 6D 7E C8 78 79 38 1D B3 8D 36 0D 76 FA 7B BF
    auto index_parm = parms.WithPath("index.html"sv);
    auto result = resolve(index_parm);
    auto resp = std::get_if<Response>(&result);
    if (resp) {
      resp->mime_ = "text/html";
    } else if (std::holds_alternative<ProvenAbsent>(result)){
      return DynamicListingHtml(parms.MyPath().to_view());
    }
    return result;
  }
  std::string name{parms.NextComponent(api_.get())};
  auto hash = hexhash(name);
  return resolve_internal(hash.begin(), hash.end(), name, parms);
}
auto Self::resolve_internal(ipfs::ipld::DirShard::HashIter hash_b,
                            ipfs::ipld::DirShard::HashIter hash_e,
                            std::string_view human_name,
                            ResolutionState& parms) -> ResolveResult {
  auto hash_chunk = hash_b == hash_e ? std::string{} : *hash_b;
  auto needed_size = hash_chunk.size() + human_name.size();
  for (auto& [name, link] : links_) {
    if (!starts_with(name, hash_chunk)) {
      continue;
    }
    if (name.size() == needed_size && ends_with(name, human_name)) {
      return CallChild(parms, name);
    }
    auto node = parms.GetBlock(link.cid);
    if (!node) {
      // Unfortunately we can't really append more path and do a full Car
      // request
      //  The gateway would hash whatever we gave it and compare it to a
      //  partially-consumed hash
      return MoreDataNeeded{{"/ipfs/" + link.cid}};
    }
    auto downcast = node->as_hamt();
    if (downcast) {
      if (hash_b == hash_e) {
        LOG(ERROR) << "Ran out of hash bits.";
        return ProvenAbsent{};
      }
      return downcast->resolve_internal(std::next(hash_b), hash_e, human_name,
                                        parms);
    } else {
      return ProvenAbsent{};
    }
  }
  return ProvenAbsent{};
}
std::string Self::listing_json() const {
  std::string result{"{"};
  auto first = true;
  auto const w = hex_width();
  for (auto& [index, link] : links_) {
    auto hash_bits = std::string_view{index}.substr(0, w);
    if (first) {
      first = false;
    } else {
      result.push_back(',');
    }
    result.append("\n  ").push_back('"');
    result.append(hash_bits)
      .append("\": {\"cid\": \"")
      .append(link.cid)
      .push_back('"');
    if (index.size() > w) {
      auto name = index.substr(w);
      std::size_t quot;
      while ((quot = name.find('"')) < name.size()) {
        name.insert(quot, 1UL, '\\');
      }
      result.append(", \"name\": \"")
        .append(name)
        .push_back('"');
    }
    result.push_back('}');
  }
  result.append("\n}");
  return result;
}
std::size_t Self::hex_width() const {
  auto rv = 0U;
  for (auto x = fanout_; x > 1; ++rv) {
    x >>= 4;
  }
  if (rv != 2UL) {
    LOG(ERROR) << "fanout=" << fanout_ << " width=" << rv;
  }
  return rv;
}

std::vector<std::string> Self::hexhash(std::string_view path_element) const {
  std::array<std::uint64_t, 2> digest = {0U, 0U};
  MurmurHash3_x64_128(path_element.data(), path_element.size(), 0,
                      digest.data());
  std::vector<std::string> result;
  for (auto d : digest) {
    auto hash_bits = htobe64(d);
    while (hash_bits) {
      // 2. Pop the log2(fanout_) lowest bits from the path component hash
      // digest,...
      auto popped = hash_bits % fanout_;
      hash_bits /= fanout_;
      std::ostringstream oss;
      // ... then hex encode (using 0-F) using little endian those bits ...
      oss << std::setfill('0') << std::setw(hex_width()) << std::uppercase
          << std::hex << popped;
      result.push_back(oss.str());
    }
  }
  return result;
}

Self::DirShard(std::uint64_t fanout) : fanout_{fanout} {}
Self::~DirShard() noexcept = default;
Self* Self::as_hamt() {
  return this;
}

namespace {
  ipfs::Response DynamicListingHtml(std::string_view path) {
    std::string body = "<html>\n\t<head>\n\t\t<title>Directory Listing for ";
    body.append(path);
    if (!path.ends_with('/')) {
      body.push_back('/');
    }
    body.append(R"(</title>
        <script>
          function parse(resp) {
            return resp.json();
          }
          function send_request(url) {
            const headers = { headers: { Semantic:"listing" } };
            const request = new Request(url, headers);
            fetch(request).then(parse).then(on_response);
          }
          const q = [];
          function send_next() {
            const nxt = q.pop();
            if (nxt) {
              send_request( nxt );
            }
          }
          let item_count = 0;
          let fetched_count = 1;
          let estimate = 9;
          function add_item(parent, key) {
            item_count++;
            const tr = document.createElement("tr");
            if (typeof parent == 'object') {
              for (const exist of parent.children) {
                if (! (typeof exist == 'object')) {
                  console.log("How do you have an item in the parent that's not an object?", JSON.stringify(exist));
                } else if (exist.children.length == 0) {
                  console.log('List element has no text.');
                } else if (exist.children[0].textContent > key) {
                  parent.insertBefore(tr, exist);
                  return tr;
                }
              }
              console.log(key, 'is the first/last item in the list?');
              parent.appendChild(tr);
            } else {
              console.log("You tried to insert into a ", JSON.stringify(parent));
            }
            return tr;
          }
          function update_estimate() {
            if (q.length) {
              const items_per_block = item_count / fetched_count;
              if (items_per_block < 2) {
                items_per_block = 2;
              }
              const raw_est = q.length * items_per_block;
              if (estimate < raw_est) {
                estimate = raw_est;
              } else {
                estimate = (estimate + raw_est) / 2;
              }
              estimate = Math.ceil(estimate);
              document.getElementById('nshow').textContent = item_count;
              document.getElementById('est').textContent = estimate;
            } else {
              document.getElementById('more').style.visibility = "hidden";
            }
          }
          function on_response(j) {
            fetched_count++;
            const out = document.getElementById('list');
            let added_entry = false;
            for (const hash_bits in j) {
              const entry = j[hash_bits]
              const url = 'ipfs://' + entry.cid;
              if ('name' in entry) {
                const tr = add_item(out, entry.name);
                let td = document.createElement("td");
                tr.append(td);
                let a = document.createElement("a");
                td.append(a);
                a.setAttribute("href", window.location + "/" + entry.name);
                a.textContent = entry.name;
                td = document.createElement("td");
                tr.append(td);
                const p = document.createElement("p");
                td.append(p);
                p.textContent = entry.cid;
                added_entry = true;
              } else {
                q.push(url)
              }
            }
            if (added_entry) {
              update_estimate();
            } else {
              requestIdleCallback(send_next);
            }
          }
        </script>
      </head>
      <body onload="send_request(window.location.href);">
        <p>
          <h1>Index for )");
    body.append(path);
    if (!path.ends_with('/')) {
      body.push_back('/');
    }
    body.append(R"(</h1>
            </p>
            <p>
                <table border=1>
                  <tbody id='list' />
                </table>
            </p>
            <div id='more'>
              <p>Showing <b id='nshow'>0</b> of the estimated <b id='est'>0</b> items.</p>
              <button type="button" onclick='send_next()'>Load more</button>
            </div>
          </body>
        </html>)");
    return {
      "text/html",
      200,
      body,
      "",
      {}
    };
  }
}
