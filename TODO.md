* Upstreaming
  - Link to blog in existing Chromium ticket
  - Intent to intent to prototype precursor emails
  - Intent to prototype
  - [ChromiumStyle](https://chromium.googlesource.com/chromium/src/+/main/styleguide/c++/c++.md)
* Documentation
  - Re-record demo video with updated/more links?
  - Publish blog
  - Document re-use of library/
  - Update design.md to as-implemented
* Maintainability
  - Chromium tag -> patch file mapping
* Production features
  - User settings
  - Integrate with chromium cache
  - Devtools (network tab doesn't show enough)
  - MDNS discovery of gateways
* QoI
  - Persist promote/demote (perhaps integrated with user settings)
  - Real URLLoader (not just Simple*) for gateway requests (SetPriority, pause, etc.)
  - Implement SetPriority, having it Scheduler parameters
  - Other hash algos
  - Other multibase encodings
  - Enforce maximum block size
  - IPNS name resolution - receive multiple records and pick highest sequence #
* Dev QoL
  - Docker builds verifying every documented build approach
  - Github Action release including uploading library artifacts
  - Discovery doesn't really belong in gateway_requests.cc
  - Streamline Windows release, e.g. package_browser 
