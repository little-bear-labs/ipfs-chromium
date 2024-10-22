#!/bin/bash -ex

echo Install dependencies
    sudo apt-get update
    sudo apt-get install --yes \
        cmake \
        ninja-build \
        doxygen \
        graphviz \
        valgrind \
        pipx

for pl in lib{capture-tiny,datetime}-perl
do
    sudo apt-get install "${pl}"
done

wget https://github.com/linux-test-project/lcov/releases/download/v2.1/lcov-2.1.tar.gz
tar xzvf lcov-2.1.tar.gz
export PATH="${PWD}/lcov-2.1/bin:${PATH}"
hash -r

pipx ensurepath
pipx install conan
npm install -g @marp-team/marp-cli

set +x
echo Show versions:
for p in g++ gcc clang{++,} lcov conan
do
    which "${p}"
    "${p}" --version
done
set -x

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
