# Install CMake 3.22+

Before proceeding with any combination of the steps below, make sure you have in your PATH:
* CMake version 3.22 or higher
  - run `cmake --version` to check
  - If your system's preferred way to install software has only an older version, consider [these instructions](https://cmake.org/install/)
* Python 3.6+ and its pip 
* git

Then decide which of these approaches are appropriate for you:

## ipfs_client (without Chromium)

Found in the library/ directory, ipfs_client is a library with a fairly standard [CMake](https://cmake.org/cmake/help/latest/) build.

As for dependencies, you have some choices: 

### With Python
If you have python3 and installed and findable, it will use [Conan](https://docs.conan.io/en/latest/introduction.html) to manage dependencies.

See also: docker/debian-python

### With C++23 (standard library in particular, but assuming the compiler too)

Be sure to set CXX_VERSION=23 in the CMake configure step.
This covers your need for 'vocabulary types', so you shouldn't need Boost or Abseil.
Otherwise, follow the next subsection.

### With system-installed or otherwise CMake-findable dependencies
Required dependencies as they exist today:
* protobuf (both compiler and library)
* OpenSSL - TODO probably should de-require, easiest path is disabling sha at runtime. Also, alternatives.
* vocabulary types: one of the following
    - boost (See also: docker/debian-boost)
    - abseil & boost (will prefer Abseil, fills in gap with boost)

To enable testing, these are also required:
* GTest
* GMock

### Steps

* `mkdir -p /your/build/dir`
* `cmake -D CMAKE_BUILD_TYPE=Debug -S /path/to/ipfs-chromium -B /your/build/dir`
    - Set CMAKE_BUILD_TYPE to one of Debug or Release.
    - Specifying the generator (with -G) is optional, but I recommend picking one that knows about compile_commands.json. You do, obviously, need to pick one that you have installed. See the end of the output from `cmake --help` to see which ones you have available
* Expect warnings about how it doesn't have access to a Chromium source tree and so won't be building that.
* `cmake --build /your/build/dir --target ipfs_client`
    - Or invoke your underlying build system directly, with make or ninja, or what have you.

## Chromium with the ipfs component 

### If you already have a "chrome" build available on your system you'd like to patch with IPFS 

#### Pre-steps

* See [the docs](https://chromium.googlesource.com/chromium/src/+/main/docs/linux/build_instructions.md) explaining these steps.
* You already did the fetch & runhooks
* You already have a build directory (e.g. out/Default) via gn gen
* You have a handful of targets already built (TODO: get a specific list). 
  - These are all dependencies of chrome, so if you've built chrome you're good.

#### Steps (TODO - finish docker)
* `mkdir -p /an/ipfs-chromium/build/dir` 
* `cmake -D CMAKE_BUILD_TYPE=Debug -D CHROMIUM_SOURCE_TREE=/your/checkout/chromium/src -D CHROMIUM_PROFILE=Default -S /path/to/ipfs-chromium -B /an/ipfs-chromium/build/dir`
    - CMAKE_BUILD_TYPE - Either Debug or Release. Choose Debug unless you use is_debug=false in args.gn. 
    - CHROMIUM_SOURCE_TREE - points to the git checkout of chromium. Should end in /src
    - CHROMIUM_PROFILE - the profile you're building. Defaults to ${CMAKE_BUILD_TYPE}. ${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE} should exist.
    - DEPOT_TOOLS_DIRECTORY - Path to scripts like gn, autoninja, and ninja.py. If these are executable and in your `PATH` environment variable, you can leave this as its default value which is "DETECT_FROM_PATH" and does what one might expect.
    - If ipfs is not mentioned in //chrome/browser/BUILD.gn , this command will apply the patch file presumably adding //components/ipfs as a dependency to //chrome/browser
* `cmake --build /an/ipfs-chromium/build/dir --target chrome_browser`

### If you want ipfs-chromium's build do all your setup for you.

If you take this approach, ipfs-chromium will download a private copy of the Chromium source tree and depot_tools into its own build directory.

*NOTE* This takes a long time, about 55GB of disk space, and a fair bit of memory.

*NOTE* The previous section assumes you must have gotten all the dependencies of Chromium, because if you had not you wouldn't have successfully built chrome.
However, here we cannot make that assumption. So...

#### Pre-step: make sure you have Chromium's dependencies
See the docs for:
* [Linux](https://chromium.googlesource.com/chromium/src/+/main/docs/linux/build_instructions.md#Install-additional-build-dependencies)
  - Those instructions assume you've already fetched the Chromium source tree
  - However, if you haven't, and your system is supported by install-build-deps.sh, you should be able to get a sufficient version of it from [here](https://raw.githubusercontent.com/chromium/chromium/main/build/install-build-deps.sh), although it may want to call other scripts from subdirectories of build/, so you may want to recursively fetch the whole directory.
  - The author is writing this on Fedora, where the not-guaranteed [note](https://chromium.googlesource.com/chromium/src/+/main/docs/linux/build_instructions.md#Fedora) seems to work fine.
  - If your system isn't supported by install-build-deps.sh, and doesn't have a functioning 'note' on that document, I suggest looking at the package lists from the other distros' notes to get an idea of what is actually needed.
* [Mac](https://chromium.googlesource.com/chromium/src/+/main/docs/mac_build_instructions.md#System-requirements)
  - Make any necessary upgrades/installations to meet the "System Requirements" mentioned in the doc.
* [Windows](https://chromium.googlesource.com/chromium/src/+/main/docs/windows_build_instructions.md#Setting-up-Windows)
  - Make any upgrades/installations mentioned in the "Setting up Windows" section mentioned in the doc that you don't already have.
  - Note: building on Windows is untested

#### Steps
* `mkdir -p /an/ipfs-chromium/build/dir`
* `cmake -D CMAKE_BUILD_TYPE=Debug -D DOWNLOAD_CHROMIUM=TRUE -S /path/to/ipfs-chromium -B /an/ipfs-chromium/build/dir`
  - CMAKE_BUILD_TYPE - Either Debug or Release. If you choose Release it will set is_debug=false in args.gn.
  - If `ccache` is in your path, it will instruct not just the CMake but also the gn build to use it. I recommend this.
  - This step will take much, much longer than you may be used to as a normal CMake user.
* `cmake --build /an/ipfs-chromium/build/dir --target chrome_browser`
  - This will also take much, much longer than you're used to. (TODO - time these things for a ballpark figure)

*NOTE* - anwhere in this document you see `cmake --build`, feel free to replace that with a call to your chosen underlying build system. For example, `cmake --build X Y` could become `make -C X Y` 
