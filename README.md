# IPFS racing multi-gateway integration for Chromium

## Current state

### Basic demo
[demo.webm](https://github.com/little-bear-labs/ipfs-chromium/assets/97759690/ae072a58-f5de-4270-8d48-2c858d9b17b1)


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

See [doc/todo.md](doc/todo.md)

## Originally Planned Design

See [doc/original_design.md](doc/original_design.md)

## Building

See [BUILDING.md](BUILDING.md)


## Outstanding Questions
 * More direct interaction with browser cache? 
 * Changes needed to get the Omnibar to prefer treating ipfs://_ as a URL over a search term
 
