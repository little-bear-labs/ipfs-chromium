* Upstreaming
  - Link to blog in existing Chromium ticket
  - Intent to intent to prototype precursor emails
  - Intent to prototype
  - [ChromiumStyle](https://chromium.googlesource.com/chromium/src/+/main/styleguide/c++/c++.md)
* Documentation
  - Re-record demo video with updated/more links?
  - Publish blog
  - Document re-use of library/ipfs_client classes (doxygen/cldoc/doxypress/standardese?)
  - Update design.md to as-implemented
* Maintainability
  - Chromium tag -> patch file mapping
  - Our scripts switch branches or do checkout on Chromium repo _unless_ they _just_ fetched it
  - Rename classes to better fit their responsibility (see class-level doc for inspiration)
* Production features
  - User settings
  - Integrate with chromium cache
  - Devtools (network tab doesn't show enough)
  - MDNS discovery of gateways
* QoI
  - Handle different types of errors differently (don't always call DagListener::FourOhFour)
  - Persist promote/demote (perhaps integrated with user settings)
  - Real URLLoader (not just Simple*) for gateway requests (SetPriority, pause, etc.)
  - Implement SetPriority, having it Scheduler parameters
  - Other hash algos
  - Other multibase encodings
  - Enforce maximum block size
  - IPNS name resolution - receive multiple records and pick highest sequence #
  - Etag
  - Resolve identity CIDs internally
* Dev QoL
  - Docker builds verifying every documented build approach
  - Github Action release including uploading library artifacts
  - Discovery doesn't really belong in gateway_requests.cc
  - Streamline Windows release, e.g. package_browser 
  - library published in Conan
  - Publish binaries in Conan
  - Allow library/ users to set log level in a normalish way and don't prefix everything with pb


