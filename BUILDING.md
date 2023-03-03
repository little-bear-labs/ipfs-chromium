# WIP
Much of what is described here is insufficiently tested. 
Will update as more info is acquired.

## Just libipfs_client alone

Found in the library/ directory, ipfs_client is a library with a fairly standard [CMake](https://cmake.org/cmake/help/latest/) build.

### With Python
If you have python3 installed and findable, it will use [Conan](https://docs.conan.io/en/latest/introduction.html) to manage dependencies.

See also: docker/debian-python

### With system-installed or otherwise CMake-findable dependencies
Required dependencies as they exist today:
* protobuf (both compiler and library)
* OpenSSL - TODO probably should de-require, easiest path is disabling sha at runtime
* vocabulary types: one of the following
    - A C++23 version of the C++ standard library (-D CXX_VERSION=23)
    - boost (See also: docker/debian-boost)
    - abseil & boost (will prefer Abseil, fills in gap with boost)

To enable testing:
* GTest
* GMock

### Steps

* `mkdir /your/build/dir`
* `cmake -G Ninja -D CMAKE_BUILD_TYPE=Debug -S /path/to/ipfs-chromium -B /your/build/dir`
    - Set CMAKE_BUILD_TYPE to one of Debug or Release.
    - Specifying the generator is optional, but I recommend picking one that knows about compile_commands.json. You do, obviously, need to pick one that you have installed. See the end of the output from `cmake --help` to see which ones you have available
* Expect warnings about how it doesn't have access to a Chromium source tree and so won't be building that.
* `cmake --build /your/build/dir`
    - Or invoke your underlying build system directly. In my example that would be `ninja -C /your/build/dir`

## The ipfs component in the Chromium source tree

My apologies - there are some bootstrapping issues here, so some repetition is involved.

### Steps
TODO cleanup post-docker proof
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
