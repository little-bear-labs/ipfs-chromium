#!/bin/bash -ex
cd `dirname "${0}"`/..

branch=`git rev-parse --abbrev-ref HEAD`
docker build --file docker/${1}.chrome_browser --network=host --add-host=host.docker.internal:host-gateway --build-arg "GIT_REF=${branch}" --tag ${1}-chrome-browser .
