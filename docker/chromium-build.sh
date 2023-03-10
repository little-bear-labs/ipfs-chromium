#!/bin/bash -ex

source source /ipfs-chromium/docker/shared.env

# ... you will need to make sure that the following tools are available:
for tool in curl git lsb_release sudo
do
  echo ${tool} ; ${tool} --version
done
RUN mkdir -p /chromium/src/out/${PROFILE}
RUN cd /chromium/src && gn gen /chromium/src/out/${PROFILE}
mkdir -p /ipfs-chromium-${PROFILE}
cmake -G Ninja -D CMAKE_BUILD_TYPE=${PROFILE} -D CHROMIUM_SOURCE_TREE=/chromium/src -D CHROMIUM_PROFILE=${PROFILE} -S /ipfs-chromium -B /ipfs-chromium-${PROFILE}
ninja -C ipfs-chromium-${PROFILE} -j 1 setup_component
RUN cd /chromium/src && gn gen /chromium/src/out/${PROFILE}
RUN autoninja -j 1 -C /chromium/src/out/${PROFILE} chrome

