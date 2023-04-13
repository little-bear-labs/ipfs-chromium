#!/usr/bin/env python3

from glob import glob
from os import environ, makedirs
from os.path import basename, dirname, getmtime, isdir, isfile, join, pathsep
from shutil import copyfile, which
from sys import argv, executable

import subprocess

build_dir = argv[1]
cmake_cache = open(join(build_dir,'CMakeCache.txt')).readlines()
without_comment = map(lambda x: x.split('//')[0].strip(),cmake_cache)
with_equal = filter(lambda x: '=' in x,without_comment)
kv = map(lambda x: (x.split('=')[0].split(':')[0], '='.join(x.split('=')[1:])), with_equal)
vars = dict(kv)

depot_tools_dir = vars['DEPOT_TOOLS_DIRECTORY']
src = vars['CHROMIUM_SOURCE_TREE']
chromium_dir = dirname(src)
ipfs_chromium_source_dir = vars['ipfs-chromium_SOURCE_DIR']
tag = vars['CHROMIUM_TAG']
profile = vars['CHROMIUM_PROFILE']
git_binary = vars['GIT_EXECUTABLE']
jobs = vars['parallel_jobs']
gnargs = vars['GN_ARGS']
python = executable

def run(args, fail_ok = False):
    if isdir(src):
        cwd = src
    elif isdir(chromium_dir):
        cwd = chromium_dir
    else:
        cwd = None
    print('Run',args,'in',cwd)
    res = subprocess.run(args=args, cwd=cwd)
    print(res)
    ec = res.returncode
    if ec == 0:
        return True
    print('Command failed with exit code',ec,':',args)
    if fail_ok:
        return False
    exit(ec)

def out(args):
    if isdir(src):
        cwd = src
    elif isdir(chromium_dir):
        cwd = chromium_dir
    else:
        cwd = None
    #print('Run',args,'in',cwd,'and return output.')
    res = subprocess.run(args=args, cwd=cwd, stdout=subprocess.PIPE)
    if res.returncode != 0:
        print('Command failed',res,':',args)
    return res.stdout.decode('utf-8').strip()

if depot_tools_dir == 'DETECT_FROM_PATH':
    gclient = which('gclient')
    if gclient == '':
        print('You set DEPOT_TOOLS_DIRECTORY to DETECT_FROM_PATH, but there is no gclient in your path. Will download/use private copy instead.')
        depot_tools_dir = ''
    else:
        depot_tools_dir = dirname( gclient )
if depot_tools_dir == '':
    depot_tools_dir = join(build_dir,'depot_tools')
if not isdir(join(depot_tools_dir,'.git')):
    run([git_binary,'clone','https://chromium.googlesource.com/chromium/tools/depot_tools.git',depot_tools_dir])

environ['PATH'] = depot_tools_dir + pathsep + environ['PATH']

if not isdir(chromium_dir):
    makedirs(chromium_dir)
if not isdir(src):
    run([python,join(depot_tools_dir,'fetch.py'),'--nohooks','chromium'])
branch = out([git_binary, 'rev-parse', '--abbrev-ref', 'HEAD'])
target_branch = 'ipfs-chromium/' + tag
if branch == target_branch:
    print('On branch',branch)
else:
    print('You are on branch',branch,'not',target_branch)
    run([git_binary,'branch',target_branch,tag],True)
    run([git_binary,'checkout',target_branch])
    run([python, join(depot_tools_dir,'gclient.py'), 'sync', '-j', jobs, '--with_branch_heads'])
    run([python, join(depot_tools_dir,'gclient.py'), 'runhooks', '-j', jobs])

if not isfile(join(src,'.landmines')):
    run([python, join(depot_tools_dir,'gclient.py'), 'runhooks', '-j', jobs])

with open(join(src,'chrome', 'browser', 'BUILD.gn')) as w:
    content = w.read()
    if 'components/ipfs' in content:
        print('Chromium seems to be already patched.')
    else:
        print('Apply patch file...')
        run([git_binary, 'apply', '--verbose', join(ipfs_chromium_source_dir,'component','rest_of_chromium.patch')])

ipfs_dir = join(src,'components','ipfs')
if not isdir(ipfs_dir):
    makedirs(ipfs_dir)
for d in ['component',join('library','include'),join('library','src')]:
    root = join(ipfs_chromium_source_dir,d)
    print('root',root)
    for cc in glob('**/*.cc', root_dir = root, recursive=True):
        f=join(ipfs_chromium_source_dir,d,cc)
        t=join(ipfs_dir,basename(cc))
        if not isfile(t) or getmtime(f) > getmtime(t):
            print('copy',f,t)
            copyfile(f,t)
    for h in glob('**/*.h', root_dir = root, recursive=True) + glob('**/*.hpp', root_dir = root, recursive=True):
        f=join(ipfs_chromium_source_dir,d,h)
        t=join(ipfs_dir,h)
        hd = dirname(t)
        if not isdir(hd):
            print('makedirs',hd)
            makedirs(hd)
        if not isfile(t) or getmtime(f) > getmtime(t):
            print('copy',f,t)
            copyfile(f,t)
for f in glob(join(ipfs_chromium_source_dir,'library','*.proto')) + glob(join(ipfs_chromium_source_dir,'library','src', 'ipfs_client','*.h')):
    t = join(ipfs_dir,basename(f))
    if not isfile(t) or getmtime(f) > getmtime(t):
        print('Copy',f,t)
        copyfile(f,t)
t=join(ipfs_dir,'BUILD.gn')
f=join(build_dir,'component','BUILD.gn')
if not isfile(t) or getmtime(f) > getmtime(t):
    print('copy',f,t)
    copyfile(f,t)
out = join(src,'out',profile)
n = join(out,'obj','components','ipfs','ipfs.ninja')
if not isfile(n) or getmtime(t) > getmtime(n):
    run([python,join(depot_tools_dir,'gn.py'),'gen','--args='+gnargs.replace("'",""),out])

for target in argv[2:]:
    print("Build target",target)
    run([python,join(depot_tools_dir,'ninja.py'),'-C',out,'-j',jobs,target])
