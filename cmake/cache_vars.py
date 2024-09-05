#!/usr/bin/env python3

from os import getcwd
from os.path import  join
from sys import argv

if len(argv) > 1:
    build_dir = argv[1]
else:
    build_dir = getcwd()
cmake_cache = open(join(build_dir,'CMakeCache.txt'), encoding="utf-8").readlines()
without_comment = map(lambda x: x.split('//')[0].strip(),cmake_cache)
with_equal = filter(lambda x: '=' in x,without_comment)
kv = map(lambda x: (x.split('=')[0].split(':')[0], '='.join(x.split('=')[1:])), with_equal)
CACHE_VARS = dict(kv)

if __name__ == '__main__':
    print(vars)
    for k, v in vars.items():
        print(k,'=',v)
