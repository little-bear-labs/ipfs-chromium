#include "providers_response.h"

#include <mock_api.h>

namespace {
constexpr std::string_view Example{
    R"JSON({
"Providers": [
  {
    "Addrs": [
      "/ip4/212.6.53.91/tcp/24002"
    ],
    "ID": "12D3KooWHEzPJNmo4shWendFFrxDNttYf8DW4eLC7M2JzuXHC1hE",
    "Metadata": "kBKjaFBpZWNlQ0lE2CpYKAABgeIDkiAgPoyOt5HOvEwokGEOteJjHL53/aKyqJyU5EpGuQ5n4BtsVmVyaWZpZWREZWFs9W1GYXN0UmV0cmlldmFs9Q==",
    "Protocol": "transport-graphsync-filecoinv1",
    "Schema": "graphsync-filecoinv1"
  },
  {
    "Schema": "bitswap",
    "Protocol": "transport-bitswap",
    "ID": "12D3KooWFzZYEChh6Dsy4jHyTopK42oRrk41WVfnpMG5r8eoXa6u",
    "Addrs": [
      "/ip4/212.6.53.91/tcp/8888"
    ]
  },
  {
    "Addrs": [
      "/ip4/212.6.53.91/tcp/80/http"
    ],
    "ID": "12D3KooWHEzPJNmo4shWendFFrxDNttYf8DW4eLC7M2JzuXHC1hE",
    "Metadata": "oBIA",
    "Protocol": "transport-ipfs-gateway-http",
    "Schema": "unknown"
  },
  {
    "Addrs": [
      "/ip4/212.6.53.91/tcp/24002"
    ],
    "ID": "12D3KooWHEzPJNmo4shWendFFrxDNttYf8DW4eLC7M2JzuXHC1hE",
    "Metadata": "kBKjaFBpZWNlQ0lE2CpYKAABgeIDkiAgrdmoGr43Odjpk3bzdYyaw8AS5k1MCrzgNUF5SQlrmgtsVmVyaWZpZWREZWFs9W1GYXN0UmV0cmlldmFs9Q==",
    "Protocol": "transport-graphsync-filecoinv1",
    "Schema": "graphsync-filecoinv1"
  },
  {
    "Addrs": [
      "/ip4/45.141.104.43/tcp/11337"
    ],
    "ID": "12D3KooWKGCcFVSAUXxe7YP62wiwsBvpCmMomnNauJCA67XbmHYj",
    "Metadata": "kBKjaFBpZWNlQ0lE2CpYKAABgeIDkiAgrdmoGr43Odjpk3bzdYyaw8AS5k1MCrzgNUF5SQlrmgtsVmVyaWZpZWREZWFs9W1GYXN0UmV0cmlldmFs9Q==",
    "Protocol": "transport-graphsync-filecoinv1",
    "Schema": "graphsync-filecoinv1"
  },
  {
    "Schema": "bitswap",
    "Protocol": "transport-bitswap",
    "ID": "12D3KooWJw2CwXKUYvRvNgm7pT4bJuUdaqz6c7ie55rUAokQokB8",
    "Addrs": [
      "/ip4/45.141.104.43/tcp/41337"
    ]
  },
  {
    "Addrs": [
      "/ip4/45.141.104.43/tcp/31337/http"
    ],
    "ID": "12D3KooWKGCcFVSAUXxe7YP62wiwsBvpCmMomnNauJCA67XbmHYj",
    "Metadata": "oBIA",
    "Protocol": "transport-ipfs-gateway-http",
    "Schema": "unknown"
  },
  {
    "Addrs": [
      "/ip4/45.141.104.43/tcp/11337"
    ],
    "ID": "12D3KooWKGCcFVSAUXxe7YP62wiwsBvpCmMomnNauJCA67XbmHYj",
    "Metadata": "kBKjaFBpZWNlQ0lE2CpYKAABgeIDkiAgrdmoGr43Odjpk3bzdYyaw8AS5k1MCrzgNUF5SQlrmgtsVmVyaWZpZWREZWFs9W1GYXN0UmV0cmlldmFs9A==",
    "Protocol": "transport-graphsync-filecoinv1",
    "Schema": "graphsync-filecoinv1"
  },
  {
    "Addrs": [
      "/ip4/185.7.192.36/tcp/24001"
    ],
    "ID": "12D3KooWEHx9v2SGvFewuatTvzzGxQC1PQZPRer2ys8fYipiM1zi",
    "Metadata": "kBKjaFBpZWNlQ0lE2CpYKAABgeIDkiAgPoyOt5HOvEwokGEOteJjHL53/aKyqJyU5EpGuQ5n4BtsVmVyaWZpZWREZWFs9W1GYXN0UmV0cmlldmFs9Q==",
    "Protocol": "transport-graphsync-filecoinv1",
    "Schema": "graphsync-filecoinv1"
  },
  {
    "Schema": "bitswap",
    "Protocol": "transport-bitswap",
    "ID": "QmQzqxhK82kAmKvARFZSkUVS6fo9sySaiogAnx5EnZ6ZmC",
    "Addrs": [
      "/dns4/elastic.dag.house/tcp/443/wss"
    ]
  },
  {
    "Addrs": [
      "/dns4/dag.w3s.link/tcp/443/https"
    ],
    "ID": "QmUA9D3H7HeCYsirB3KmPSvZh3dNXMZas6Lwgr4fv1HTTp",
    "Metadata": "oBIA",
    "Protocol": "transport-ipfs-gateway-http",
    "Schema": "unknown"
  },
  {
    "Addrs": [
      "/ip4/185.7.192.36/tcp/24001"
    ],
    "ID": "12D3KooWEHx9v2SGvFewuatTvzzGxQC1PQZPRer2ys8fYipiM1zi",
    "Metadata": "kBKjaFBpZWNlQ0lE2CpYKAABgeIDkiAgrdmoGr43Odjpk3bzdYyaw8AS5k1MCrzgNUF5SQlrmgtsVmVyaWZpZWREZWFs9W1GYXN0UmV0cmlldmFs9Q==",
    "Protocol": "transport-graphsync-filecoinv1",
    "Schema": "graphsync-filecoinv1"
  },
  {
    "Addrs": [
      "/ip4/212.6.53.53/tcp/24002",
      "/ip6/2a10:2080::53/tcp/24002"
    ],
    "ID": "12D3KooWJ8YAF6DiRxrzcxoeUVjSANYxyxU55ruFgNvQB4EHibpG",
    "Metadata": "kBKjaFBpZWNlQ0lE2CpYKAABgeIDkiAgrdmoGr43Odjpk3bzdYyaw8AS5k1MCrzgNUF5SQlrmgtsVmVyaWZpZWREZWFs9W1GYXN0UmV0cmlldmFs9Q==",
    "Protocol": "transport-graphsync-filecoinv1",
    "Schema": "graphsync-filecoinv1"
  },
  {
    "Schema": "bitswap",
    "Protocol": "transport-bitswap",
    "ID": "12D3KooWJqxfjvbDkKByJQeT5urmeFBavNhNvidsAeqjWr6hf7jT",
    "Addrs": [
      "/ip4/212.6.53.53/tcp/8888"
    ]
  },
  {
    "Addrs": [
      "/ip4/127.0.0.1/tcp/4001",
      "/ip4/192.168.1.30/tcp/4001",
      "/ip4/192.168.1.30/udp/4001/quic-v1",
      "/ip4/192.168.1.30/udp/4001/quic-v1/webtransport/certhash/uEiDZo7acNa2kQxb5ORDgFa8mCoIM0dkgpRBcb3kl625-Yg/certhash/uEiCsylNiUH7WPsPsmkxWJWiTU8H2yStng5kh04T-zC7VzQ",
      "/ip4/127.0.0.1/udp/4001/quic-v1",
      "/ip6/::1/udp/4001/quic-v1/webtransport/certhash/uEiDZo7acNa2kQxb5ORDgFa8mCoIM0dkgpRBcb3kl625-Yg/certhash/uEiCsylNiUH7WPsPsmkxWJWiTU8H2yStng5kh04T-zC7VzQ",
      "/ip4/127.0.0.1/udp/4001/quic-v1/webtransport/certhash/uEiDZo7acNa2kQxb5ORDgFa8mCoIM0dkgpRBcb3kl625-Yg/certhash/uEiCsylNiUH7WPsPsmkxWJWiTU8H2yStng5kh04T-zC7VzQ",
      "/ip6/::1/tcp/4001",
      "/ip6/::1/udp/4001/quic-v1"
    ],
    "ID": "12D3KooWEK7juErczBSwPg8JNCrhRwz94qJUac3eLeorRZoeGmJu",
    "Schema": "peer"
  },
  {
    "Addrs": [
      "/ip4/127.0.0.1/udp/4003/quic",
      "/ip6/::1/udp/4003/quic",
      "/ip4/45.63.0.182/udp/4001/quic/p2p/12D3KooWJeNjNsFfbFxwdvNWarEoyT37KACeqwkbw1v78Le5tZhp/p2p-circuit",
      "/ip4/89.117.59.120/udp/4001/quic-v1/webtransport/certhash/uEiBGzxS1Qs0nWmCyxKmZ4SUWJnniTEqf8aE5xLYGWO3n4Q/certhash/uEiBETLuOZfjfe6tmzfqvNtKyDBunqSzpQmE6BrSphLJb0w/p2p/12D3KooWPK4QMbHgB3iKw8jTAmULRb8Po1nHVseuCXLYx2waeL7q/p2p-circuit",
      "/ip6/::1/udp/4003/quic-v1/webtransport/certhash/uEiCx7YT7FwdfC51vXaR9S5yeOsiuOfrJZwuOSHGKvSQNNw/certhash/uEiD8EtjJk2adeDVrQzY79UIQLA7deN7Z98sXam5kZHqUkA",
      "/ip4/45.63.0.182/udp/4001/quic-v1/p2p/12D3KooWJeNjNsFfbFxwdvNWarEoyT37KACeqwkbw1v78Le5tZhp/p2p-circuit",
      "/ip4/127.0.0.1/udp/4003/quic-v1",
      "/ip4/127.0.0.1/udp/4003/quic-v1/webtransport/certhash/uEiCx7YT7FwdfC51vXaR9S5yeOsiuOfrJZwuOSHGKvSQNNw/certhash/uEiD8EtjJk2adeDVrQzY79UIQLA7deN7Z98sXam5kZHqUkA",
      "/ip4/192.168.1.145/tcp/4003",
      "/ip4/192.168.1.145/udp/4003/quic-v1/webtransport/certhash/uEiCx7YT7FwdfC51vXaR9S5yeOsiuOfrJZwuOSHGKvSQNNw/certhash/uEiD8EtjJk2adeDVrQzY79UIQLA7deN7Z98sXam5kZHqUkA",
      "/ip6/::1/tcp/4003",
      "/ip6/::1/udp/4003/quic-v1",
      "/ip4/127.0.0.1/tcp/4003",
      "/ip4/192.168.1.145/udp/4003/quic",
      "/ip4/192.168.1.145/udp/4003/quic-v1",
      "/ip4/45.63.0.182/udp/4001/quic-v1/webtransport/certhash/uEiBQGsxwn3qvsZYsQlLsQxPeiIrAHlNmAzVDNFriuLE4hA/certhash/uEiBn5GiHbeq26ASwFtICJ9a2o3KO_Ibs8Cywxv1epYZEQw/p2p/12D3KooWJeNjNsFfbFxwdvNWarEoyT37KACeqwkbw1v78Le5tZhp/p2p-circuit",
      "/ip4/89.117.59.120/udp/4001/quic-v1/p2p/12D3KooWPK4QMbHgB3iKw8jTAmULRb8Po1nHVseuCXLYx2waeL7q/p2p-circuit"
    ],
    "ID": "12D3KooWMDzafp4zYh9BxBV7CeFb8MRT3QkGYsmQq4Ap5bVBbALT",
    "Schema": "peer"
  },
  {
    "Addrs": [
      "/ip4/127.0.0.1/tcp/4001",
      "/ip4/192.168.0.110/udp/4001/quic-v1",
      "/ip4/192.168.0.110/udp/4001/quic-v1/webtransport/certhash/uEiBRy1Y-qOilERGytvzaLVdqdy5DslwUypUTPIwMZpR4kw/certhash/uEiA4LFSdjmE2fej6x3PvvL3lPNI3UAJH1_UaxKaNi9mctg",
      "/ip6/::1/udp/4001/quic-v1",
      "/ip4/127.0.0.1/udp/4001/quic-v1",
      "/ip6/::1/tcp/4001",
      "/ip6/::1/udp/4001/quic-v1/webtransport/certhash/uEiBRy1Y-qOilERGytvzaLVdqdy5DslwUypUTPIwMZpR4kw/certhash/uEiA4LFSdjmE2fej6x3PvvL3lPNI3UAJH1_UaxKaNi9mctg",
      "/ip4/127.0.0.1/udp/4001/quic-v1/webtransport/certhash/uEiBRy1Y-qOilERGytvzaLVdqdy5DslwUypUTPIwMZpR4kw/certhash/uEiA4LFSdjmE2fej6x3PvvL3lPNI3UAJH1_UaxKaNi9mctg",
      "/ip4/192.168.1.76/udp/51508/quic-v1",
      "/ip4/192.168.1.76/udp/51508/quic-v1/webtransport/certhash/uEiBRy1Y-qOilERGytvzaLVdqdy5DslwUypUTPIwMZpR4kw/certhash/uEiA4LFSdjmE2fej6x3PvvL3lPNI3UAJH1_UaxKaNi9mctg",
      "/ip4/192.168.0.110/tcp/4001",
      "/ip4/192.168.1.76/tcp/51508"
    ],
    "ID": "12D3KooWGDMwwqrpcYKpKCgxuKT2NfqPqa94QnkoBBpqvCaiCzWd",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooWSBi4DtGgA7xpPR4mMf8Hqy16DEJ2oJrmDZ9QDXgQFCaY",
    "Schema": "peer"
  },
  {
    "Addrs": [
      "/ip4/65.108.229.198/tcp/4001",
      "/ip6/2a01:4f9:1a:a9da::2/tcp/4001",
      "/ip4/65.108.229.198/udp/4001/quic",
      "/ip6/2a01:4f9:1a:a9da::2/udp/4001/quic"
    ],
    "ID": "12D3KooWNdrk6acFmQKqvJE4wnsNjnJmr1HRsJEL5X8ideB5fKRR",
    "Schema": "peer"
  },
  {
    "Addrs": [
      "/ip4/95.216.8.12/tcp/110",
      "/ip4/95.216.8.12/tcp/4001",
      "/ip4/95.216.8.12/udp/110/quic-v1",
      "/ip4/95.216.8.12/udp/4001/quic-v1"
    ],
    "ID": "Qmd7QHZU8UjfYdwmjmq1SBh9pvER9AwHpfwQvnvNo3HBBo",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooWDWKWnHSNp1QE1ucyuUyrDWtdXYurfwYWm3UCQo1gPFNK",
    "Schema": "peer"
  },
  {
    "Addrs": [
      "/ip4/109.123.244.248/udp/4001/quic/p2p/12D3KooWNGCKqANoACRgUyawxwqL35snpJ4sA9DCqU1rkPVzPhFt/p2p-circuit",
      "/ip4/109.123.244.248/udp/4001/quic-v1/webtransport/certhash/uEiB8GMjAQkQ5AEqbJGWq0IK_dnVpWU18Z6PAPJfnxN5b8w/certhash/uEiAFvbNeXXNuETTj8pNrN94Q9Gb-toKo2aG0twYC3IYuyg/p2p/12D3KooWNGCKqANoACRgUyawxwqL35snpJ4sA9DCqU1rkPVzPhFt/p2p-circuit",
      "/ip4/51.195.44.67/tcp/4001/p2p/12D3KooWC6cy5wUHVe9WdxhmqHLbFmMuNztrfR9sjZLxY6zExCCf/p2p-circuit",
      "/ip4/51.195.44.67/udp/3697/quic/p2p/12D3KooWC6cy5wUHVe9WdxhmqHLbFmMuNztrfR9sjZLxY6zExCCf/p2p-circuit",
      "/ip4/109.123.244.248/udp/4001/quic-v1/p2p/12D3KooWNGCKqANoACRgUyawxwqL35snpJ4sA9DCqU1rkPVzPhFt/p2p-circuit"
    ],
    "ID": "12D3KooWDrNJ7ZNsH6k51SjS4Pw9rtDeSG1VpzE44UiJHRNy7NGK",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooWDtXB7G1P2hZjHjGfNTdWZuX2sPBEo8nfWxrM1rJKPYhk",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooWE2R7c8U1r7XKgqHudTRixVVMhVjdmYwLQGAU77VGxUB7",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooW9rsxcvpWLnw2RnAoyuyXQr7YNvUGY3XKp3PPCkrvk9HW",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooW9yNeRwovTWXnALCSmFjKTPfsGjexEvJJWBZzGVRxdrpw",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooWACMJr9jMFcb7e9AbkqRZAy3cRNK7KQvZjJSUydKJykSU",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooWA9yxpmgpHNg3LmfNgUDs9MRGf2ekBxeJFQKzAPsVcghx",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooWAM6XEKQjbQiXypo3bqW3fqJy3RGasSaCFPvHxhRmnfhG",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooWAM2SUTd6CegPUCuWGih18gaVBRE6ryG9TJrqWHeX4v7H",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooWAFpSVAKftm9zR6uCcPBZp3FuRDk6Ztfwr51n7awiXvii",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooWARd1HyrzP2Gwt7DW23aWSDT3SHfe6LCmZawGMdPVN5CM",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooWAciV3Vh2RzG8fck8M455RxZGzSAfVsXMFHuE5zh2Mx3c",
    "Schema": "peer"
  },
  {
    "ID": "12D3KooWAVyf8PwYw6qyYMXZSG3Bj6Dx7TZpBm4HrG9SKhqSRd7K",
    "Schema": "peer"
  }
]
})JSON"};
}  // namespace

TEST(ProvidersResponseTest, ExampleAnswer) {
  MockApi api;
  ipfs::providers::ProcessResponse(Example, api);
  auto& g = api.gw_->gateways_added;
  EXPECT_EQ(g.size(), 3UL);
  EXPECT_EQ(g.at(0), "http://212.6.53.91/");
  EXPECT_EQ(g.at(1), "http://45.141.104.43:31337/");
  EXPECT_EQ(g.at(2), "https://dag.w3s.link/");
}
