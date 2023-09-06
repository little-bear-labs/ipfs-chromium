# IPFS HTTP Embedded Client Explainer

## What is IPFS?

[IPFS is an open system to manage data without a central server](https://ipfs.tech)

IPFS involves distributed storage. Content-addressed resources mean that `ipfs://` URLs indicate _what_ is being requested, rather than from where.

The ecosystem, and this proposal, also includes [IPNS](https://docs.ipfs.tech/concepts/ipns/), whose URLs are based on a public key, effectively a mutable pointer to immutable data. 

## What is an IPFS client?

The term 'client' is being used here for any software that requests IPFS resources without participating in other aspects of the ecosystem, for example file sharing. We're describing an asymmetric relationship, unlike most of the IPFS ecosystem.

## Why HTTP(s)?

Although the peer-to-peer communication between nodes happens over a number of transports (often quic), it is a custom protocol designed for synchronizing a [DHT](https://en.wikipedia.org/wiki/Distributed_hash_table) and isn't the most appropriate for a strict client.
Existing "gateways" - that is software intended to support clients - are HTTP servers.

In addition, it is perceived that HTTP requests may be more at home in a browser based around HTTP requests than a custom protocol.

## How do Chrome users interact with IPFS today?

One can talk directly to a single IPFS gateway. 
This would allow you to access a URL like 
`ipfs://bafybeidcw4v62rt3c5474dn5teesjlk6oa22ntcysa77ncx7vp22mxz7tu/`
if you rewrite it to something like 
`http://bafybeidcw4v62rt3c5474dn5teesjlk6oa22ntcysa77ncx7vp22mxz7tu.ipfs.localhost:8080/`
If you happen to have your node & gateway running locally, this can have good technical characteristics.
There are some usability concerns, in particular the need to re-write URLs and issues around sharing URLs.
There is also the concern of headaches around needing to install a separate program and remembering to start it.

If you're redirecting to a single public instance for all requests, this could raise concerns availability, privacy, and differences in filter lists.  

There is an extension, [IPFS Companion](https://chrome.google.com/webstore/detail/ipfs-companion/nibjojkomfdiaoajekhjakgkdhaomnch), which can help to work around some of the user experience problems.

Brave, a Chromium derivative, has built-in code to redirect ipfs and ipns schemes to your choice of a public gateway or, if you prefer, it will install a node locally and redirect to that.
Since its omnibar understands the schemes, this has a reasonable user interface - no need to rewrite URLs and URLs copy-paste easily when sharing with other Brave users.
Redirecting to a single public node has privacy and trust concerns. There's also a content availability concern.
Having the browser install and manage a node locally has a maintainability concern, initial cost on the user, possible conflict with other nodes, and also provides full peer-to-peer functionality which a user might not want.

All of these approaches ultimately do a 1:1 translation to a single HTTP URL, similar to a custom handler.
This could make sense if there is a reliable and trusted gateway readily available, whose server logs are not a privacy concern.
And maintaining a local node has the aforementioned concerns with maintainability and users who do not want to fully participate in the peer-to-peer network.
However, this also leaves value on the table.

Our approach raise the browser to a first-class citizen of the IPFS ecosystem (doing its own content verification).
It enables taking advantage of characteristics built into the protocol, for example caching (in the case of an IPFS block, indefinitely; in the case of an IPNS record, until the specified expiration) by binary CID rather than gateway- and multibase-specific URLs.
It also provides for opportunities down the road for natural synergies, for example using Chromium's mDNS abilities to discover [nearby Kubo](https://github.com/ipfs/kubo/blob/master/docs/config.md#discoverymdnsenabled) gateways.

## This repository

This [git repository](https://github.com/little-bear-labs/ipfs-chromium) contains an early experiment into implementing an integrated HTTP-based IPFS client.
As implemented, this is a purely racing gateway, that uses a separate request for each node of each DAG. 
That same request is typically sent to multiple gateways, with the first verified response causing all others to be cancelled. 

The directory [library](https://github.com/little-bear-labs/ipfs-chromium/tree/main/library) contains libipfs_client, which implements some of the features needed by such a client in a Chromium-agnostic way, with the intention of being reusable.
When building a Chromium variant, its code gets copied into `//third_party/ipfs_client`.

The directory [component](https://github.com/little-bear-labs/ipfs-chromium/tree/main/component), contains both:
* Code that similarly gets copied into a Chromium component: `//components/ipfs`
* A per-Chromium-version patch file (found in component/patches), which modifies code outside of that component to integrate it into the browser. This patch file is intended to be small to minimize conflicts, and contains fewer than 300 lines of code.

The perhaps-misnamed directory [cmake](https://github.com/little-bear-labs/ipfs-chromium/tree/main/cmake) contains build-time scripting for building ipfs_client as a standalone library, build the Chromium variant with IPFS support enabled, and integration for CMake, conan, gclient, lcov, etc..

Info on building this code can be found in [building.md](building.md). If you'd rather use pre-built binaries, both the library and the browser can be found in [Github releases](https://github.com/little-bear-labs/ipfs-chromium/releases/tag/0.0.0.3).

## Future plans

### Chromium integration

Landing code in Chromium upstream remains a primary goal. 
Some things kept in mind:
* Remain open at all times to feedback from Chromium developers, integrating suggested changes
* Create a branch of the Chromium repository that has all changes, rather only than a separate overlay repository. 
* All IPFS code should be behind a Runtime Enabled Feature (disabled by default), and also a build flag.
* Code should be adapted to fit any feasible guidelines.
* In-Chromium-tree testing should be added.

### Features

This initial pass was at best a proof-of-work.
Some of the additional desired to be implemented include:
* IPFS-specific devtools
* Support for V2-only IPNS records
* _redirects for PWA/SPA
* DAG-JSON & DAG-CBOR
* Use of IPFS Content routing API to supplement gateway list
* Additional hash types 
* Additional bases
* Improved HTML preview of UnixFS directories
* Additional features as recommended by either Chromium devs and/or feature sponsor(s).

## Further reading

* A short [demo video](https://www.youtube.com/watch?v=9XJOktFizlo).
* A [blog post](https://blog.ipfs.tech/2023-05-multigateway-chromium-client/) intended for a more general audience.
* Chromium [Issue](https://bugs.chromium.org/p/chromium/issues/detail?id=1440503#c_ts1682598269)
* ChromeStatus [Feature](https://chromestatus.com/feature/5105580464668672)
* IPFS [Specs](https://specs.ipfs.tech/), especially:
  - [IPLD Responses](https://specs.ipfs.tech/http-gateways/trustless-gateway/#accept-request-header)
  - [UnixFS](https://github.com/ipfs/specs/blob/main/UNIXFS.md#data-format)
  - [IPNS Records](https://specs.ipfs.tech/ipns/ipns-record/#ipns-record)
  - [DNSLink](https://specs.ipfs.tech/http-gateways/dnslink-gateway/#leveraging-dns-for-content-routing)
  - [Routing](https://specs.ipfs.tech/routing/http-routing-v1/#get-routing-v1-providers-cid) API
  - [_redirects](https://specs.ipfs.tech/http-gateways/web-redirects-file/)
  - [Kubo](https://github.com/ipfs/kubo/blob/master/docs/gateway.md), currently the most common IPFS gateway software
  - [Bifrost](https://github.com/ipfs/bifrost-gateway#how-does-high-level-overview-look-like), an IPFS gateway exploring IPFS networking best practices
