#!/bin/bash -ex
cd `dirname "${0}"`/..
./docker/library-builds.sh
for profile in Debug Release
do
  for variant in boost python
  do
    echo -e "\n   ###   BUILDING CHROMIUM   ###   $(tag)   ###   ${variant}/${profile}   ###"
    sleep 3
    docker run --network=host --add-host=host.docker.internal:host-gateway `tag` --mount type=bind,source=.docker-debian-cache,target=/cache /ipfs-chromium/docker/chromium-build.sh
  done
done
