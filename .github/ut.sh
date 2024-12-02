#!/bin/bash -ex

here="$(dirname "${0}")"
export here
source "${here}/ubuntu-deps.env"

conan profile detect

echo Configure
          mkdir build
          cmake \
            -G Ninja \
            -S ipfs_chromium \
            -B build \
            -D USE_DOXYGEN=FALSE \
            -D CMAKE_BUILD_TYPE=Debug

echo Run Tests
     cmake --build build --config Debug --target run_tests
echo Generate Coverage Report
     cmake --build build --config Debug --target cov
