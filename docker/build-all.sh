#!/bin/bash -ex
cd `dirname "${0}"`/..
docker build -f docker/debian-base -t ipfs-chromium-debian-base .
for variant in python
do
  for profile in Debug Release
  do
#     lprof=`tr '[:upper:]' '[:lower:]' <<< "${profile}"`
    docker build -f docker/debian-${variant} --build-arg "PROFILE=${profile}" .
  done
done
