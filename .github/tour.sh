#!/bin/bash -ex
echo Clone tester repo.
git clone https://github.com/John-LittleBearLabs/ipfs_client_clitester.git

echo Install dependencies.
sudo apt-get update
sudo apt-get install --yes cmake ninja-build binutils libc6{,-dev}
pip3 install conan
conan profile detect || echo "Profile detection failed. Perhaps the default profile already existed - perhaps this user has already done some conan-based builds."

echo Build conan library
conan create --build=missing ipfs_chromium/library/

echo Configure clitester
mkdir tester_build
cmake \
  -G Ninja \
  -S ipfs_client_clitester \
  -B tester_build \
  -D CMAKE_BUILD_TYPE=Release

echo Build clitester
cmake --build tester_build --config Release

echo Start test server
( timeout 3600 python3 ./ipfs_chromium/test_data/test_server.py 8080 2>&1 | tee server.log & ) &
for t in 1{0..9}
do
  if grep -n . server.log
  then
    break
  elif curl -m $t 'http://localhost:8080/ping'
  then
    sleep 1
  else
    sleep ${t}
  fi
done

function url_case() {
  echo "url_case(" "${@}" ")"
  if timeout 360 ./tester_build/clitester warning "${1}://${2}"
  then
    echo clitester exited with successful status
  else
    echo "Directly reported error code from clitester run - usually means timeout killed it. " "${@}"
    exit 7
  fi
  n=`sed 's,[^A-Za-z0-9\.],_,g' <<< ${2}`
  if cat "_${1}_${n}" | md5sum | cut -d ' ' -f 1 > actual
  then
    ls -lrth _ip?s_*
    if [ $# -ge 3 ]
    then
      echo "${3}" > expected
      if diff actual expected
      then
        echo good
      else
        echo "Got wrong result: " "${@}"
        exit 8
      fi
    fi
  else
    grep -n . server.log || sleep 1
    ls -lrth _ip?s_*
    echo "Failure: ${*}"
    exit 9
  fi
}

url_case ipfs bafkqacdjmrsw45djor4q ff483d1ff591898a9942916050d2ca3f 'Identity (inlined) CID'

url_case ipfs baguqeerah2nswg7r2pvlpbnsz5y4c4pr4wsgbzixdl632w5qxvedqzryf54q 7750fd7b0928f007e1d181763c0dbdb5 'A DAG-JSON document. The block itself md5s to b92348005af4ae4795e6f312844fb359, but the response we are hashing here is an HTML preview page. This does mean this test breaks if you make the preview less ugly.'

url_case ipns en.wikipedia-on-ipfs.org/I/HFE_Too_Slow_1.JPG.webp 09c09b2654e8529740b5a7625e39e0c8 'An image fetched through DNSLink and HAMT sharded directories.'
url_case ipfs bafybeieb33pqideyl5ncd33kho622thym5rqv6sujrmelcuhkjlf2hdpu4/Big%20Buck%20Bunny.webm 06d51286e56badb4455594ebed6daba2 'A large UnixFS file - several hundred blocks.'
url_case ipns k51qzi5uqu5dijv526o4z2z10ejylnel0bfvrtw53itcmsecffo8yf0zb4g9gi/symlinks/relative_link.txt cfe9b69523140b5b5e63874a8e4997e4 'A relative symlink resolves successfully to the file pointed to.'

echo Stop test server.
killall python3 2>/dev/null || true
