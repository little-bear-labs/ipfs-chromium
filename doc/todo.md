# TODO list

## Upstreaming
  - [ChromiumStyle](https://chromium.googlesource.com/chromium/src/+/main/styleguide/c++/c++.md)
## Documentation
  - Document re-use of library/ipfs_client classes
  - Update design.md to as-implemented
## Production features
  - UI for User settings, also prefs for turning off discovery etc.
  - MDNS discovery of gateways
## QoI
  - Real URLLoader (not just Simple*) for gateway requests (SetPriority, pause, etc.)
  - Implement SetPriority
  - Other hash algos
  - Other multibase encodings
  - IPNS name resolution - receive multiple records and pick highest sequence # (partial)
  - Etag
  - IPNS recursion limit
## Dev QoL
  - Docker builds verifying every documented build approach
  - Streamline Windows release, e.g. package_browser 
  - library published in Conan
