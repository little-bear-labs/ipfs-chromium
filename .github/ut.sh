#!/bin/bash -ex

echo Install dependencies
          sudo apt-get update
          sudo apt-get install --yes cmake ninja-build binutils doxygen graphviz libc6{,-dev} valgrind pipx

wget https://github.com/linux-test-project/lcov/releases/download/v2.1/lcov-2.1.tar.gz
tar xzvf lcov-2.1.tar.gz
export PATH+=":${PWD}/lcov-2.1/bin"

pipx ensurepath
pipx install conan
npm install -g @marp-team/marp-cli

echo Show versions:
g++ --version
clang++ --version
conan --version
lcov --version
conan profile detect

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
