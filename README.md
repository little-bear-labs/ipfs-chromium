# IPFS racing multi-gateway integration for Chromium

## Current state

[vid](http://chomp:8080/ipfs/QmWvWKiCvknAYQ1v6h8DKysk18HWcHstNGoFb1wmSijjqe/c.mkv)

### TODO
* DCHECK and redefine to analogues, in ipfs_client 
* Unit tests coverage for ipfs_client, using GTest
* unixfs_path_resolver needs to return something reasonable for a directory
* cull hard-coded list to remove those that don't allow block-level access
* Plug resolver into Loader - request single blocks
* https://orchestrator.strn.pl/nodes/nearby
* Do real URLLoader management for gateway requests
* Proper CID library
* Validation!!
* ipns resolution
* dnslink resolution
* User settings
* https://orchestrator.strn.pl/nodes/nearby
* ChromiumStyle() (accessors_only())
* Persist promote/demote (perhaps integrated with user settings)
* Build fresh without circular steps, CI to verify

## Planned Design

See [DESIGN.md](DESIGN.md)

## Building

### Just libipfs_client alone

Found in the library/ directory, ipfs_client is a library with a fairly standard [CMake](https://cmake.org/cmake/help/latest/) build.

If you have python3 installed and findable, it will use [Conan](https://docs.conan.io/en/latest/introduction.html) to manage dependencies.

#### Steps
* `mkdir /your/build/dir`
* `cd /your/build/dir`
* `cmake -G Ninja -D CMAKE_BUILD_TYPE=Debug /path/to/ipfs-chromium`
   - It is required to set CMAKE_BUILD_TYPE to one of the standard variants, in particular I recommend either Debug or Release.
   - Specifying the generator is optional, since pretty much any of them should work. You do, obviously, need to pick one that you have installed. See the end of the output from `cmake --help` to see which ones you have available
* Expect warnings about how it doesn't have access to a Chromium source tree and so won't be building that.
* `cmake --build .`
   - Or invoke your underlying build system directly. In my example that would be `ninja`

### The ipfs component in the Chromium source tree
* Clone chromium and [get it set up](https://chromium.googlesource.com/chromium/src/+/main/docs/linux/build_instructions.md)
* Follow steps to build libipfs_client, except also set:
  - CHROMIUM_SOURCE_TREE - points to the git checkout of chromium
  - CHROMIUM_PROFILE - the profile you're building. Defaults to `Default`. ${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE} should exist.
  - DEPOT_TOOLS_DIRECTORY - Path to scripts like gn, autoninja, and ninja.py. If these are executable and in your `PATH` environment variable, you can leave this as its default value which is "DETECT_FROM_PATH" and does what one might expect.
* Example cmake configure command: `cmake -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CHROMIUM_SOURCE_TREE=/code/chromium/src /path/to/ipfs-chromium`
  - If ipfs is not mentioned in //chrome/browser/BUILD.gn , this command will apply the patch file presumably adding //components/ipfs as a dependency to //chrome/browser
* `cmake --build . --target setup_component`
  - If you build the default target (`all`), that will work too, though it will likely report an error.
* Re-run your `gn gen` command again, with all the normal arguments.
  - Previous run(s) will not have seen //components/ipfs and so you need to include that in your generation
* Run a full build of ipfs-chromium
  - `cmake --build . `
  - The `all` target includes building //components/ipfs in the tree, just to check.
* Build chrome
  - `cd /path/to/chromium/src`
  - `autoninja -j 9 -C out/Default chrome` (or however you like to do it)

## Outstanding Questions
 * Importance of what shows in the address bar when looking at an ipfs:// addressed html file
 * Handling of relative URLs
 * In what ways will be interact with browser cache? 
   - If you request 2 different paths within the same path, there is duplication higher on the root, etc..
   - If we send them to a different gateway, this fools browser cache.
   - Should we maintain our own cache? 
 * Changes needed to get the Omnibar to prefer treating ipfs://_ as a URL over a search term
 * Is there a better way to work around security checks on the response being passed back from an ipfs:// URLLoader
 * Chromium DNS TXT record access - may/should we use //net for TxtRecordRdata and the like?
 * Best way to steal from cpp-libp2p, while minimizing cross-maintenance
 * Is there existing C++ implementation for [UnixFS/PB-DAG](https://ipld.io/specs/codecs/dag-pb/spec/#implementations). 
   - Only a subset of functionality needed. And Chromium does have protobuf support available already.
   - Could be implemented as part of this effort, but better not to duplicate.
 * "Access to image at 'ipfs://bafybeifszd4wbkeekwzwitvgijrw6zkzijxutm4kdumkxnc6677drtslni/ipfs-logo-768px.png' from origin 'null' has been blocked by CORS policy: Cross origin requests are only supported for protocol schemes: http, data, isolated-app, chrome-extension, chrome, https, chrome-untrusted.", source: ipfs://QmTCo5Q1pyLfw9NNcNeyxbF96ni1EpWXnAMBEviQWG2H9d (17)