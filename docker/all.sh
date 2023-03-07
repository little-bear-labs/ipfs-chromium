#!/bin/bash -ex
uname | grep Linux # Portability is untested. Would not surprise me if other Unix-like doesn't have fuser and needs a different way to count CPUs
icecc --version
which iceccd
which icecc-scheduler

count_cpus() {
  grep '^processor' /proc/cpuinfo | wc -l
}

if fuser `which icecc-scheduler`
then
  echo icecc-scheduler already running
else
  icecc-scheduler --daemonize
fi
if fuser `which iceccd`
then
  echo iceccd already running
else
  iceccd --daemonize --max-processes $[ `count_cpus` / 9 + 1 ]
fi
cd `dirname "${0}"`/..
docker build --file docker/debian-base --network=host --add-host=host.docker.internal:host-gateway --tag ipfs-chromium-debian-base .
tag() {
  tr '[:upper:]' '[:lower:]' <<< "debian-${variant}-${profile}"
}
for variant in python boost
do
  for profile in Debug Release
  do
    echo -e "\n   ###   BUILDING IMAGE   ###   $(tag)   ###   ${variant}/${profile}   ###"
    sleep 3
    docker build --file docker/debian-${variant} --network=host --add-host=host.docker.internal:host-gateway --build-arg "PROFILE=${profile}" --tag `tag` .
  done
done
for variant in python boost
do
  for profile in Debug Release
  do
    echo -e "\n   ###   BUILDING LIBRARY   ###   $(tag)   ###   ${variant}/${profile}   ###"
    sleep 3
    docker run --network=host --add-host=host.docker.internal:host-gateway `tag` /ipfs-chromium/docker/cmake-build.sh
  done
done
