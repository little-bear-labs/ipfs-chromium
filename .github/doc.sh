#!/bin/bash -ex

here="$(dirname "${0}")"
export here
source "${here}/ubuntu-deps.env"
sudo apt install doxygen

echo Configure
mkdir -p build
set -x
find build/ -iname '*.cmake'
find ./ -iname '*proto*'
find build/ -iname '*proto*'
if cmake \
  -G Ninja \
  -S ipfs_chromium \
  -B build \
  -D USE_DOXYGEN=TRUE \
  -D CMAKE_BUILD_TYPE=Debug
then
  cmake --build build --config Debug --target doc
  find ./ -iname index.html
  ls -lrth build/
  ls -lrth build/doc/
  ls -lrth build/doc/html
  mv -v ipfs_chromium/.git .
else
  find build/ -iname '*.cmake'
  find ./ -iname '*proto*'
  find build/ -iname '*proto*'
fi
