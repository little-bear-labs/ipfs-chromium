#include "ipns_cbor.h"

#include "base/logging.h"
#include "components/cbor/reader.h"

namespace {
bool Assign(cbor::Value const& v, std::string& o) {
  if (v.is_string()) {
    o = v.GetString();
    return true;
  } else if (v.is_bytestring()) {
    auto& b = v.GetBytestring();
    o.assign(reinterpret_cast<char const*>(b.data()), b.size());
    return true;
  }
  return false;
}
bool Assign(cbor::Value const& v, std::uint64_t& o) {
  if (v.is_unsigned()) {
    o = v.GetUnsigned();
    return true;
  }
  return false;
}
template <class V>
void Assign(cbor::Value::MapValue const& m, char const* n, V& out) {
  auto i = m.find(cbor::Value{n});
  if (i == m.end()) {
    LOG(ERROR) << "CBOR contains no key '" << n << "'!";
    return;
  }
  if (Assign(i->second, out)) {
    VLOG(1) << "Assigned successfully '" << n << "'='" << out << "'.";
  } else {
    LOG(ERROR) << "Type mismatch on " << n
               << " type received: " << static_cast<int>(i->second.type());
  }
}

void PrettyPrint(cbor::Value const& v) {
  std::clog << "t=" << static_cast<int>(v.type()) << ' ';
  if (v.is_map()) {
    auto& m = v.GetMap();
    std::clog << "{\n";
    for (auto& e : m) {
      std::clog << "  ";
      PrettyPrint(e.first);
      std::clog << " ==> ";
      PrettyPrint(e.second);
      std::clog << '\n';
    }
    std::clog << '}';
  } else if (v.is_string()) {
    std::clog << '"' << v.GetString() << '"';
  } else if (v.is_integer()) {
    std::clog << v.GetInteger();
  } else {
    std::clog << "<unhandled>";
  }
}

}  // namespace

auto ipfs::ParseCborIpns(ipfs::ByteView bytes) -> IpnsCborEntry {
  auto val = cbor::Reader::Read(as_octets(bytes));
  if (!val) {
    LOG(ERROR) << "Buffer failed to parse as CBOR!";
    return {};
  }
  if (!val->is_map()) {
    LOG(ERROR) << "The CBOR buffer parsed as value of type "
               << static_cast<int>(val->type()) << " rather than a map!";
    return {};
  }
  PrettyPrint(*val);
  IpnsCborEntry result;
  auto& m = val->GetMap();
  Assign(m, "Value", result.value);
  Assign(m, "Validity", result.validity);
  Assign(m, "ValidityType", result.validityType);
  Assign(m, "TTL", result.ttl);
  Assign(m, "Sequence", result.sequence);

  return result;
}