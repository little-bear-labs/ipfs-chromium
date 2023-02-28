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

My apologies - there are some bootstrapping issues here, so some repetition is involved.

#### Steps
* Clone chromium and [get it set up](https://chromium.googlesource.com/chromium/src/+/main/docs/linux/build_instructions.md)
* In Chromium source tree, run `gen gen` with arguments appropriate to your build.
* autoninja -C out/${CHROMIUM_PROFILE} chrome
  - You may want to specify a `-j`
* Run `cmake` for ipfs-chromium as above, but also set:
  - CHROMIUM_SOURCE_TREE - points to the git checkout of chromium
  - CHROMIUM_PROFILE - the profile you're building. Defaults to `Default`. ${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE} should exist.
  - DEPOT_TOOLS_DIRECTORY - Path to scripts like gn, autoninja, and ninja.py. If these are executable and in your `PATH` environment variable, you can leave this as its default value which is "DETECT_FROM_PATH" and does what one might expect.
  - Example cmake configure command: `cmake -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CHROMIUM_SOURCE_TREE=/code/chromium/src /path/to/ipfs-chromium`
  - If ipfs is not mentioned in //chrome/browser/BUILD.gn , this command will apply the patch file presumably adding //components/ipfs as a dependency to //chrome/browser
* `cd /your/build/dir && cmake --build . --target setup_component`
* Re-run your `gn gen` command again, with all the normal arguments.
  - Previous run(s) will not have seen //components/ipfs . This one should pick it up.
* Run a full build of ipfs-chromium
  - `cd /your/build/dir && cmake --build . `
  - The `all` target includes building //components/ipfs in the tree, libipfs_client, and various checks.
* Build chrome
  - `cd /path/to/chromium/src`
  - `autoninja -j 9 -C out/Default chrome` (or however you like to do it)

## Outstanding Questions
 * More direct interaction with browser cache? 
 * Changes needed to get the Omnibar to prefer treating ipfs://_ as a URL over a search term
 