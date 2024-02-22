# IPFS Embedded Client Explainer

[TOC]

## TL;DR

Adding native support for ipfs:// and ipns:// schemes to browsers.

## What is IPFS?

[IPFS is an open system to manage data without a central server](https://ipfs.tech)

IPFS involves distributed storage. Content-addressed resources mean that `ipfs://` URLs indicate _what_ is being requested, rather than from where.

The ecosystem, and this proposal, also includes [IPNS](https://docs.ipfs.tech/concepts/ipns/), whose URLs are based on a public key, effectively a mutable pointer to immutable data. 

## Problem overview

To date the dominant ways of sharing IPFS URLs, including linking to them, involve specifying a particular gateway.

See [Addressing IPFS on the web](https://docs.ipfs.tech/how-to/address-ipfs-on-web/#ipfs-addressing-in-brief)

This has a number of problems, from privacy to performance to availability.
And of course it's largely subverted many of the reasons one may have chosen IPFS in the first place.

The biggest problem with using [IPFS-schemed URLs](https://docs.ipfs.tech/how-to/address-ipfs-on-web/#native-urls) (sometimes referred to as "Native URLs") is sharing them.
Typically one cannot assume your audience has a native client available.
For example, one will note that this document _*about* IPFS URLs_ has not yet linked to a document using such a URL.
To see why, click [this static version of the previous link](ipfs://QmbKWyoaCK9nKDfSsNAkJQrbxMDLriyyqkXsTSSwt9B227/how-to/address-ipfs-on-web/#native-urls)

A lesser problem with all existing addressing attempts: 
even if you're using an IPFS scheme, it's generally easier to get away with doing 1:1 rewriting of IPFS URLs to HTTP URLs.
So [many projects](https://ecosystem.ipfs.tech/?filters=enabled&tags=ipfs-http-client,py-ipfs-http-client,js-ipfs-https-client,http-gateway)
do just that, especially if those projects want to interact with a Web browser.
This isn't as bad if the user can make their own choice of gateway 
and has access to information needed to make that decision wisely.
But it's still leaving quite a lot of value on the table.

## Some Sample Uses

### ipfs URL scheme

Refers to immutable data. A few example uses:

* Static websites
* Data archival
* Data transfer
* Decentralized [applications](https://github.com/ipfs/awesome-ipfs#apps)

### ipns URL scheme

Mutable pointer to an ipfs:// URL (or another ipns:// URL).

Uses: updatable versions of the above

2 variants:
* (true) IPNS - Uses key signing to avoid need for centralized authority.
* DNSLink - uses DNS TXT records for a more readable origin (it's a hostname)

## What is an IPFS client?

The term 'client' is being used here for any software that requests IPFS resources without participating in other aspects of the ecosystem, for example file sharing. We're describing an asymmetric relationship, unlike most of the IPFS ecosystem.

## Making use of HTTP(s) gateways 

Although the peer-to-peer communication between nodes happens over a number of transports (often quic), 
it is a set of custom protocol designed for synchronizing a [DHT](https://en.wikipedia.org/wiki/Distributed_hash_table) and isn't the most appropriate for a strict client.
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

## Key differences with this approach

* Verifying - the most common IPFS clients use a subdomain gateway, but not trustless gateway. If your gateway is not under your control, this raises security concerns.
* Multi-gateway - parallelism primarily for performance reasons. 
* Native URLs - the url does not contain information about the gateways you're using, for less friction around caching, sharing of URLs, and flexible intra-website requests.

## Compatibility

At a high level, the most important features for compatibility are those analagous to [subdomain gateway](https://specs.ipfs.tech/http-gateways/subdomain-gateway/) 
features, but implemented in the client obviating the need for a separate HTTP server,
using data from [trustless gateways](https://specs.ipfs.tech/http-gateways/trustless-gateway/) and performing hash verification on end user's machine (Chromium).

As discussed in the proposal, for the purposes of cross-implementation compatibility, we'll seek a common subset of URLs that all implementations support. 
This implies a common universally-supported-as-input subset of:
* Codec (in the CID field sense)
* Hash algo
* Multibase

Implementations should also canonicalize URLs to a given case-insensitive multibase. 
To avoid user confusion, it would be wise if we all chose the same one.

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

## Considerations

### Origin

The concept of an origin is a bit different here.
In many schemes, including HTTP, an origin is essentially a network endpoint, possibly with user credentials.
In a native IPFS URL, the origin contains the CID and so specifies a hash to verify the content. 
It does not specify a network source, but rather it is locked in place when the content was created, and stays constant regardless of where it is hosted.
Similarly for an IPNS URL, the origin contains a public key (or a hash thereof). 
It's specifying the author & owner of the key pair, not where the record is hosted.

### Privacy

IPFS has privacy characteristics different from HTTP.
If content is being delivered over HTTP, that server can log any/all requests to it.

Since an IPFS gateway is an HTTP server, it can do this too. 
However, instead of one entity having knowledge of all requests for all URLs of a given origin,
it has knowledge of all requests made from users who chose to use that particular gateway, regardless of which origin it is.

In some situations this is preferable, but depending on one's threat model mixing knowledge about requests for different domains on a single gateway could be worse.
It will be important to include clear and concise messaging in the end-user interface where one selects their gateways and gateway discovery mechanisms.
Brave has a [thoughtful write-up](https://support.brave.com/hc/en-us/articles/360051406452-How-does-IPFS-Impact-my-Privacy-) that the browser settings impacting this link to.

For multi-gateway implementations, such as is being recommended here, this specific privacy concerns might be lower, since there's not a single point of observation of all the user's activity.
However, to date, no implementation has attempted to optimize for this (carefully selecting which gateways get to observe what), rather prioritizing performance characteristics and content availability.

### Security

Non-validating implementations, such as those using path/subdomain gateways, could have concerns with invalid responses.
Today this is most commonly observed with CDN intermediation like rate limiting - one may be expecting the third chunk of a video file and instead get an HTML file prompting the user to solve a CAPTCHA.
However, in principle a gateway could one day become malicious, and validation would be invaluable at protecting against this.

For single-gateway implementations, if that single gateway is under the user's control (localhost) and (presumably) doing its own validation, a user could have the same level of confidence, just with an extra moving piece to keep track of.

If one chooses a remote insecure HTTP gateway, then naturally all the well-known security problems around MitM and so on apply.

There have been concerns with IPFS path gateways being among the technologies abused by phishing campaigns, by linking to path gateways over HTTPS. 
The primary reason being able to have a wider number of URLs pointing to the same content.
Unfortunately, this behavior will continue regardless of what we do here, since path gateways and similar approaches will continue to exist either way.
It is worth noting, though, that if a scammer were to use a link with an ipfs or ipns scheme, it would be less convenient to vary for obfuscation. 
That is, one could still use a different hash algorithm, different codec, or chunk differently to arrive at a different set of hashes and thus different CID, that is already the case and not the typical approach. 
Switching to native URLs only removes the ability to arbitrarily switch gateway domain, since the gateway is no longer specified in the URL.

Client-side software meant to protect the user could pretty easily cue off the scheme to decide how the URL is treated or user notified. 
One might consider using deny lists for CIDs on the client side, as is [currently done by some gateways](https://github.com/ipfs-shipyard/nopfs).
It might also be worth considering an allow list on IPNS names. 
For example, if I regularly use IPFS to share files with my colleagues, it isn't unreasonable for them to ask that I sign to certify what content I've uploaded.
And adding/subscribing to a list of keys in one's organization might not be too much friction.

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
