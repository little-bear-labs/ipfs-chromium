# IPFS racing multi-gateway integration for Chromium

## Current state

### Basic demo
[0.webm](https://user-images.githubusercontent.com/97759690/219815679-735fb053-7e20-42de-819c-6057cedfefc5.webm)


### What works
* Navigation ipfs://bafy... (CIDv1 ipfs links with host-legal lowercase charset), requested 1 block at a time
    - Only if the pb-dag nodes are directores, whole file, chunked file.
* ipns:// (requests whole files & resources from a gateway)
* Subresources for ipfs, same restrictions as navigation.

### Some important things that don't work
* HAMTShard (and more exotic nodes, but Shard is important for e.g. Wikipedia)
* CIDv0 navigation
* Validation
* Local resolution of IPNS & DNSLink

### Near-term tasks

See [TODO.md](TODO.md)

## Planned Design

See [DESIGN.md](DESIGN.md)

## Building

See [BUILDING.md](BUILDING.md)


## Outstanding Questions
 * More direct interaction with browser cache? 
 * Changes needed to get the Omnibar to prefer treating ipfs://_ as a URL over a search term
 