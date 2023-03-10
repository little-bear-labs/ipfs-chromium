#!/bin/bash -ex
source /ipfs-chromium/docker/shared.env
mkdir -p /ipfs-chromium-${PROFILE}
cmake -G Ninja -D CMAKE_BUILD_TYPE=${PROFILE} -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -S /ipfs-chromium -B /ipfs-chromium-${PROFILE}
ninja -C ipfs-chromium-${PROFILE} -j 1
ninja -C ipfs-chromium-${PROFILE} -j 1 test
