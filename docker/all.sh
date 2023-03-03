#!/bin/bash -ex
cd `dirname "${0}"`/..
docker build -f docker/debian-base -t ipfs-chromium-debian-base .
for variant in python boost
do
  for profile in Debug Release
  do
    echo -e "\n   ###   ${variant}/${profile}   ###"
    sleep 3
    docker build -f docker/debian-${variant} --build-arg "PROFILE=${profile}" .
  done
done
