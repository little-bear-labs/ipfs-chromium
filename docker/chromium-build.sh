#!/bin/bash -ex

source source /ipfs-chromium/docker/shared.env

# ... you will need to make sure that the following tools are available:
for tool in curl git lsb_release sudo
do
  echo ${tool} ; ${tool} --version
done
