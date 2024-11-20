#!/bin/bash -ex
source build/Debug/generators/conanbuild.sh
protoc "--proto_path=${1}" "--cpp_out=${2}" "${3}"
