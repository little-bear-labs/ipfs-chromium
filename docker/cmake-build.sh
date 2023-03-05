#!/bin/bash -ex
mkdir -p ipfs-chromium-${PROFILE}
cmake -G Ninja -D CMAKE_BUILD_TYPE=${PROFILE} -S ipfs-chromium -B ipfs-chromium-${PROFILE}
ninja -C ipfs-chromium-${PROFILE} -j 1
ninja -C ipfs-chromium-${PROFILE} -j 1 test
