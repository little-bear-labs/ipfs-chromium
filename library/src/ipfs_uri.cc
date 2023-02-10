#include <ipfs_client/ipfs_uri.h>

std::string ipfs::IpfsUri2IpfsOverHttpUrl(std::string_view ipfs_or_ipns_uri) {
  auto scheme = ipfs_or_ipns_uri.substr(0, 4);
  ipfs_or_ipns_uri.remove_prefix(7);
  auto next_slash = ipfs_or_ipns_uri.find('/');
  auto cid_or_name = ipfs_or_ipns_uri.substr(0, next_slash);
  std::string http_url{"http://"};
  http_url.append(cid_or_name);
  http_url.push_back('.');
  http_url.append(scheme);
  http_url.push_back('.');
  http_url.append(IPFS_OVER_HTTP_DOMAIN).push_back('/');
  if (next_slash < ipfs_or_ipns_uri.size()) {
    http_url.append(ipfs_or_ipns_uri.substr(next_slash + 1));
  }
  return http_url;
}

std::string ipfs::IpfsOverHttpUrl2IpfsGatewayPath(std::string_view url) {
  url.remove_prefix(7);  // strlen("http://")
  auto predefined_start = url.find(ipfs::IPFS_OVER_HTTP_DOMAIN);
  auto scheme_start = predefined_start - 5;  // 5 = length(ip_s.)
  auto path_start = predefined_start + ipfs::IPFS_OVER_HTTP_DOMAIN.size();

  auto cid_or_name = url.substr(0, scheme_start - 1);  // 1 for .
  auto scheme = url.substr(scheme_start, 4);
  auto remainder = url.substr(path_start);

  std::string path{scheme};
  path.push_back('/');
  path.append(cid_or_name);
  if (remainder.empty()) {
      path.push_back('/');
  } else {
    path.append(remainder);
  }
  return path;
}
