#!/bin/bash -ex
mkdir build
mkdir ipfs-client-dev
if which "${1}"
then
  CC=`which "${1}"`
  export CC
fi
if which "${2}"
then
  CXX=`which "${2}"`
  export CXX
fi
if cmake \
  -G "${3}" \
  -S . \
  -B build \
  -D CMAKE_INSTALL_PREFIX=`pwd`/ipfs-client-dev \
  -D "CMAKE_BUILD_TYPE=${4}" \
  -D USE_DOXYGEN=OFF \
  -D CXX_VERSION=20
then
  echo "Configured."
else
  find "$(pwd)/build/" -iname '*proto*'
fi

