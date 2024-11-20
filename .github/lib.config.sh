#!/bin/bash -ex
mkdir build
mkdir ipfs-client-dev
if which ${{ matrix.config.cc }}
then
  CC=`which ${{ matrix.config.cc }}`
  export CC
fi
if which ${{ matrix.config.cxx }}
then
  CXX=`which ${{ matrix.config.cxx }}`
  export CXX
fi
if cmake \
  -G "${{ matrix.config.generators }}" \
  -S . \
  -B build \
  -D CMAKE_INSTALL_PREFIX=`pwd`/ipfs-client-dev \
  -D CMAKE_BUILD_TYPE=${{ matrix.config.build_type }} \
  -D USE_DOXYGEN=OFF \
  -D CXX_VERSION=20
then
  echo "Configured."
else
  find build/ -iname '*proto*'
fi

