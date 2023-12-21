#!/bin/bash -ex

echo Install dependencies
          sudo apt-get update
          sudo apt-get install --yes cmake ninja-build lcov binutils doxygen graphviz libc6{,-dev} valgrind
          npm install -g @marp-team/marp-cli

echo Configure
          mkdir build
          cmake \
            -G Ninja \
            -S ipfs_chromium \
            -B build \
            -D CMAKE_BUILD_TYPE=Debug

echo Run Tests
     cmake --build build --config Debug --target run_tests
echo Generate Coverage Report
     cmake --build build --config Debug --target cov
