#!/bin/bash -ex
cd `dirname "${0}"`/../component/patches
#okregex='kEnableIpfs'
#okregex='deps .= . "//components/ipfs" '
okregex='flag-metadata.json'
if ! grep -qE "${okregex}" *.patch
then
  echo "Regex broken: ${okregex}"
  exit 9
elif grep -LE "${okregex}" *.patch | grep -F .
then
  grep -LE "${okregex}" *.patch | sort -V | head -n 2 | xargs git rm
else
  echo "Update regex - it currently covers _all_ patches: '${okregex}'"
  exit 8
fi
