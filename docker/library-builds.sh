#!/bin/bash -ex
uname | grep Linux # Portability is untested. Would not surprise me if other Unix-like doesn't have fuser and needs a different way to count CPUs

cd `dirname "${0}"`/..
branch=`git rev-parse --abbrev-ref HEAD`
docker build --file docker/debian-base --network=host --add-host=host.docker.internal:host-gateway --build-arg "GIT_REF=${branch}" --tag ipfs-chromium-debian-base .
docker build --file docker/debian-base --network=host --add-host=host.docker.internal:host-gateway --build-arg "DEBIAN_TAG=bookworm" --build-arg "GIT_REF=${branch}" --tag ipfs-chromium-debian-base-bookworm .
tag() {
  tr '[:upper:]' '[:lower:]' <<< "debian-${variant}-${profile}"
}
for variant in python boost cxx23
do
  for profile in Debug Release
  do
    echo -e "\n   ###   BUILDING IMAGE   ###   $(tag)   ###   ${variant}/${profile}   ###"
    sleep 3
    docker build --file docker/debian-${variant} --network=host --add-host=host.docker.internal:host-gateway --build-arg "PROFILE=${profile}" --tag `tag` .
  done
done
if ccache --version
then
  export cache_dir=`ccache --get-config cache_dir`/../ipfs-chromium-cache/docker
else
  export cache_dir="${HOME}/ipfs-chromium-cache/docker"
fi
mkdir -p "${cache_dir}"/{ccache,conan}
for profile in Debug Release
do
  for variant in boost python cxx23
  do
    echo -e "\n   ###   BUILDING LIBRARY   ###   $(tag)   ###   ${variant}/${profile}   ###"
    sleep 3
    docker run --volume "${cache_dir}:/cache:Z" --network=host --add-host=host.docker.internal:host-gateway `tag`  /ipfs-chromium/docker/cmake-build.sh
  done
done
