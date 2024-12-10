#!/bin/bash -ex

here="$(dirname "${0}")"
export here
source "${here}/ubuntu-deps.env"
sudo apt install doxygen

echo Configure
          mkdir -p build
          cmake \
            -G Ninja \
            -S ipfs_chromium \
            -B build \
            -D USE_DOXYGEN=TRUE \
            -D CMAKE_BUILD_TYPE=Debug
if cmake --build build --config Debug --target doc
then
  find ./ -iname index.html
  ls -lrth build/
  ls -lrth build/doc/
  ls -lrth build/doc/html
  mv -v ipfs_chromium/.git .
else
  find build/ -iname '*proto*'
fi
