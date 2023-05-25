#!/usr/bin/env python3

from cache_vars import build_dir, vars, verbose
from patch import Patcher

from glob import glob
from os import environ, makedirs, remove
from os.path import basename, dirname, getmtime, isdir, isfile, join, relpath, pathsep
from shutil import copyfile, which
from sys import argv, executable, stderr

import subprocess

depot_tools_dir = vars['DEPOT_TOOLS_DIRECTORY']
src = vars['CHROMIUM_SOURCE_TREE']
src = src.rstrip('/')
chromium_dir = dirname(src)
verbose('dirname(', src, ')=', chromium_dir)
ipfs_chromium_source_dir = vars['ipfs-chromium_SOURCE_DIR']
profile = vars['CHROMIUM_PROFILE']
git_binary = vars['GIT_EXECUTABLE']
jobs = vars['parallel_jobs']
gnargs = vars['GN_ARGS']
build_type = vars['CMAKE_BUILD_TYPE']
python = executable

patcher = Patcher(src, git_binary, build_type)

def run(args, fail_ok = False):
    if isdir(src):
        verbose(src,'exists. Run command there.')
        cwd = src
    elif isdir(chromium_dir):
        cwd = chromium_dir
    else:
        cwd = None
    verbose('Run',args,'in',cwd)
    res = subprocess.run(args=args, cwd=cwd)
    # print(res)
    ec = res.returncode
    if ec == 0:
        return True
    print('Command failed with exit code',ec,':',args, file=stderr)
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
    verbose('Run',args,'in',cwd,'and return output.')
    res = subprocess.run(args=args, cwd=cwd, stdout=subprocess.PIPE)
    output = res.stdout.decode('utf-8').strip()
    if res.returncode != 0:
        print('Command failed',res,':',args,'output was',output,file=stderr)
        exit(res.returncode)
    return output

def runpy(args,silent = False):
    if silent:
        out([python] + args)
    else:
        run([python] + args)

if depot_tools_dir == 'DETECT_FROM_PATH':
    gclient = which('gclient')
    if gclient:
        depot_tools_dir = dirname( gclient )
    else:
        print('You set DEPOT_TOOLS_DIRECTORY to DETECT_FROM_PATH, but there is no gclient in your path. Will download/use private copy instead.',file=stderr)
        depot_tools_dir = ''
if depot_tools_dir == '':
    depot_tools_dir = join(build_dir,'depot_tools')
if not isdir(join(depot_tools_dir,'.git')):
    run([git_binary,'clone','https://chromium.googlesource.com/chromium/tools/depot_tools.git',depot_tools_dir])

if 'PATH' in environ:
    environ['PATH'] = depot_tools_dir + pathsep + environ['PATH']
else:
    environ['PATH'] = depot_tools_dir

runpy(['-m', 'pip', 'install', 'httplib2'], True)
if not isdir(src):
    verbose(src,'is not a directory. First create',chromium_dir)
    makedirs(chromium_dir)
    open(join(build_dir,'fresh'), 'a').close()

if isfile(join(build_dir,'fresh')):
    if not isdir(join(src,'.git')):
        runpy([join(depot_tools_dir,'fetch.py'),'--nohooks','chromium'])
    branch = out([git_binary, 'rev-parse', '--abbrev-ref', 'HEAD'])
    tag = patcher.recommend()
    target_branch = 'ipfs-chromium/' + tag
    if branch == target_branch:
        verbose('Already on branch',branch)
        remove(join(build_dir,'fresh'))
    else:
        print('You are on branch',branch,'not',target_branch,file=stderr)
        run([git_binary,'branch',target_branch,tag],True)
        run([git_binary,'checkout',target_branch])
        run([python, join(depot_tools_dir,'gclient.py'), 'sync', '-j', jobs, '--with_branch_heads'])
        run([python, join(depot_tools_dir,'gclient.py'), 'runhooks', '-j', jobs])

if not isfile(join(src,'.landmines')):
    run([python, join(depot_tools_dir,'gclient.py'), 'runhooks', '-j', jobs])

with open(join(src,'chrome', 'browser', 'BUILD.gn')) as w:
    content = w.read()
    if 'components/ipfs' in content:
        verbose('Chromium seems to be already patched.')
    else:
        print('Apply patch file...',file=stderr)
        patcher.apply()

ipfs_dir = join(src,'components','ipfs')
if not isdir(ipfs_dir):
    makedirs(ipfs_dir)
in_tree_sources = []
other_in_trees = [ 'BUILD.gn' ]
for d in ['component',join('library','include'),join('library','src')]:
    root = join(ipfs_chromium_source_dir,d)
    verbose('root',root)
    for ext in ['h','cc','hpp']:
        for p in glob(f'{root}/**/*.{ext}', recursive=True):
            if 'unittest' in p:
                continue
            rp = relpath( p, root )
            if ext == 'cc':
                in_tree_sources.append( rp )
            else:
                other_in_trees.append( rp )
            verbose('Will place',rp,'in BUILD.gn')
            f = p # join( ipfs_chromium_source_dir, d, rp )
            t = join( ipfs_dir, rp )
            td = dirname( t )
            verbose('Comparing',f,'to',t,'maybe copy the former to the latter?')
            if not isdir(td):
                makedirs( td )
            if not isfile(t) or getmtime(f) > getmtime(t):
                verbose( 'copy', f, t )
                copyfile( f, t )
            else:
                verbose(t,'is already up-to-date.')
protos = []
for f in glob(join(ipfs_chromium_source_dir,'library','components','ipfs','*.proto')):
    bn = basename( f )
    protos.append( bn )
    t = join( ipfs_dir, bn )
    if not isfile(t) or getmtime(f) > getmtime(t):
        verbose('Copy',f,t)
        copyfile(f,t)
formatted_sources = '\n'.join(map(lambda s: f'    "{s}",',in_tree_sources))
formatted_protos = '\n'.join(map(lambda s: f'    "{s}",',protos))
build_gn = open(join(dirname(__file__),'..','component','BUILD.gn.in')).read() \
    .replace('@gn_sources@',formatted_sources) \
    .replace('@gn_protos@',formatted_protos)
f=join(ipfs_dir,'BUILD.gn')
old_build_gn = ''
try:
    with open(join(ipfs_dir,'BUILD.gn')) as old_file:
        old_build_gn = old_file.read()
except:
    pass
if build_gn != old_build_gn:
    print("Updating BUILD.gn",file=stderr)
    open(join(ipfs_dir,'BUILD.gn'), 'w').write(build_gn)

verbose(other_in_trees)
for p in glob(f'{ipfs_dir}/**/*', recursive=True):
    rp = relpath( p, ipfs_dir )
    if rp in in_tree_sources:
        verbose(rp,'is a current in-tree C++ source')
    elif rp in protos:
        verbose(rp,'is a current in-tree protobuf source')
    elif rp in other_in_trees:
        verbose(rp,'is a current in-tree "other file" (usually header)')
    elif isdir(p):
        verbose('Ignoring directory',rp)
    else:
        print(rp,'no longer an in-tree file. Removing.',file=stderr)
        remove(p)
        break

if 'PYTHONPATH' in environ:
    environ['PYTHONPATH'] = src + '/third_party/protobuf/python' + pathsep + environ['PYTHONPATH']
else:
    environ['PYTHONPATH'] = src + '/third_party/protobuf/python'

out = join(src,'out',profile)
n = join(out,'obj','components','ipfs','ipfs.ninja')
if not isfile(n) or getmtime(t) > getmtime(n):
    run([python,join(depot_tools_dir,'gn.py'),'gen','--args='+gnargs.replace("'",""),out])

for target in argv[2:]:
    print("Build target",target,file=stderr)
    run([python,join(depot_tools_dir,'ninja.py'),'-C',out,'-j',jobs,target])
