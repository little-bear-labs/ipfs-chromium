#!/usr/bin/env python3

from verbose import verbose
from cache_vars import build_dir, CACHE_VARS
from patch import Patcher

from glob import glob
from os import environ, makedirs, remove, rename
from os.path import basename, dirname, getmtime, isdir, isfile, join, pathsep, realpath, relpath, splitext
from shutil import copyfile, which
from sys import argv, stderr

import filecmp
import subprocess

import requests
import tarfile

depot_tools_dir = CACHE_VARS['DEPOT_TOOLS_DIRECTORY']
src = CACHE_VARS['CHROMIUM_SOURCE_TREE']
src = src.rstrip('/')
chromium_dir = dirname(src)
verbose('dirname(', src, ')=', chromium_dir)
ipfs_chromium_source_dir = CACHE_VARS['ipfs-chromium_SOURCE_DIR']
profile = CACHE_VARS['CHROMIUM_PROFILE']
git_binary = CACHE_VARS['GIT_EXECUTABLE']
jobs = CACHE_VARS['parallel_jobs']
gnargs = CACHE_VARS['GN_ARGS']
build_type = CACHE_VARS['CMAKE_BUILD_TYPE']
branding_dir = CACHE_VARS['BRANDING_DIR']
python = CACHE_VARS['_Python3_EXECUTABLE']
ipfs_client_version = CACHE_VARS['IPFS_CLIENT_VERSION']

patcher = Patcher(src, git_binary, build_type)
UPDATED = 'chromium_source_updated'

prof_gn = profile.lower()
if 'test' in prof_gn or 'debug' in prof_gn:
    gn_inc = 'testing'
else:
    gn_inc = 'release'
electron_args_file = join(src, 'electron', 'build', 'args', gn_inc + '.gn')
if isfile(electron_args_file):
    toks = gnargs.split()
    # electron defines is_debug by profile convention, and unfortunately they disagree with me
    toks = filter(lambda x: not x.startswith('is_debug'), toks)
    gnargs = ' '.join(toks) + ' import(\"//electron/build/args/testing.gn\") '


def into_repo(p):
    for (f, t) in [('../../components/ipfs/', 'component'), ('../../third_party/ipfs_client/', 'library')]:
        if p.startswith(f):
            verbose(p, 'instead of', f, 'find in', t)
            return join(t, p[len(f):])
    return p


def run(args, fail_ok=False, cwd=None):
    if cwd:
        print('Run in', cwd)
    elif isdir(src):
        verbose(src, 'exists. Run command there.')
        cwd = src
    elif isdir(chromium_dir):
        cwd = chromium_dir
    else:
        cwd = None
    verbose('Run', args, 'in', cwd)
    res = subprocess.run(args=args, cwd=cwd, capture_output=True, text=True, check=False)
    # print(res)
    ec = res.returncode
    if ec == 0:
        return True
    in_repo = join(ipfs_chromium_source_dir, 'library', 'src')
    ds = [in_repo, cwd, join(src, 'out', profile), '.', ipfs_chromium_source_dir]
    for line in res.stdout.split("\n"):
        p = into_repo(line.split(':')[0])
        rem = ':' + ':'.join(line.split(':')[1:])
        found = False
        for d in ds:
            if isfile(join(d, p)):
                print(realpath(join(d, p)) + rem)
                found = True
                break
        if not found:
            print(p + rem)
    print('Command failed with exit code', ec, ':', args, file=stderr)
    if fail_ok:
        return False
    exit(ec)


def out(args, cwd=None):
    if cwd:
        print('Run in', cwd)
    elif isdir(src):
        cwd = src
    elif isdir(chromium_dir):
        cwd = chromium_dir
    else:
        cwd = None
    verbose('Run', args, 'in', cwd, 'and return output.')
    res = subprocess.run(args=args, cwd=cwd, stdout=subprocess.PIPE, check=False)
    output = res.stdout.decode('utf-8').strip()
    if res.returncode != 0:
        print('Command failed', res, ':', args, 'output was', output, file=stderr)
        exit(res.returncode)
    return output


def runpy(args, silent=False, cwd=None):
    if silent:
        out([python] + args, cwd)
    else:
        run([python] + args, False, cwd)


if depot_tools_dir == 'DETECT_FROM_PATH':
    gclient = which('gclient')
    if gclient:
        depot_tools_dir = dirname(gclient)
    else:
        print('You set DEPOT_TOOLS_DIRECTORY to DETECT_FROM_PATH, but there is no gclient in your path.', file=stderr)
        print('Will download/use private copy instead.', file=stderr)
        depot_tools_dir = ''
if depot_tools_dir == '':
    depot_tools_dir = join(build_dir, 'depot_tools')
if not isdir(join(depot_tools_dir, '.git')):
    run([git_binary, 'clone', 'https://chromium.googlesource.com/chromium/tools/depot_tools.git', depot_tools_dir])

if 'PATH' in environ:
    environ['PATH'] = depot_tools_dir + pathsep + environ['PATH']
else:
    environ['PATH'] = depot_tools_dir

runpy(['-m', 'pip', 'install', 'httplib2'], True)
if not isdir(chromium_dir):
    makedirs(chromium_dir)
if not isdir(src):
    makedirs(src)
    open(join(build_dir, 'fresh'), 'a', encoding='utf-8').close()

if isfile(join(build_dir, 'fresh')):
    if not isdir(join(src, '.git')):
        runpy([join(depot_tools_dir, 'fetch.py'), '--nohooks', 'chromium'], False, chromium_dir)
    branch = out([git_binary, 'rev-parse', '--abbrev-ref', 'HEAD'])
    tag = patcher.recommend()
    target_branch = 'ipfs-chromium/' + tag
    if branch == target_branch:
        verbose('Already on branch', branch)
        remove(join(build_dir, 'fresh'))
    else:
        print('You are on branch', branch, 'not', target_branch, file=stderr)
        run([git_binary, 'branch', target_branch, tag], True)
        run([git_binary, 'checkout', target_branch])
        run([python, join(depot_tools_dir, 'gclient.py'), 'sync', '-j', jobs, '--with_branch_heads'])
        run([python, join(depot_tools_dir, 'gclient.py'), 'runhooks', '-j', jobs])

if not isfile(join(src, '.landmines')):
    run([python, join(depot_tools_dir, 'gclient.py'), 'runhooks', '-j', jobs])

patcher.apply()

ipfs_dir = join(src, 'components', 'ipfs')
elec_dir = join(src, 'electron')


def files_content_differ(left, right):
    if isfile(left):
        if isfile(right):
            return not filecmp.cmp(left, right, shallow=False)
        else:
            return True
    else:
        return isfile(right)


source_exts = ['.h', '.cc', '.hpp']
ignore_exts = ['.patch', '.orig', '.txt', '.in']


def touch_update(s):
    with open(UPDATED, 'w', encoding='utf-8') as f:
        f.write(s)


def copy_missing_and_changed_files(source_dir, target_dir):
    sources = []
    protos = []
    for s in glob(f'{source_dir}/**/*', recursive=True):
        rel = relpath(s, source_dir)
        t = join(target_dir, rel)
        if isdir(s):
            if not isdir(t):
                makedirs(t)
            continue
        if s.endswith('_unittest.cc') or basename(s).startswith('opinionated_'):
            continue
        ext = splitext(s)[-1]
        if ext in ignore_exts:
            continue
        if ext in source_exts:
            sources.append(rel)
        elif ext == '.proto':
            rel = rel.replace('proto/ipfs_client/', '')
            t = join(target_dir, rel)
            verbose('s', s, 'rel', rel, 't', t)
            protos.append(rel)
        if files_content_differ(s, t):
            print('Copying', s, 'to', t, 'while synchronizing', source_dir, 'to', target_dir)
            if not isdir(dirname(t)):
                makedirs(dirname(t))
            touch_update(s + t)
            copyfile(s, t)
    return (sources, protos)


def remove_danglers(source_dir, target_dir, sources, protos):
    for t in glob(f'{target_dir}/**/*', recursive=True):
        if isdir(t):
            continue
        rel = relpath(t, target_dir)
        if rel == 'BUILD.gn' or rel in sources or rel in protos:
            continue
        s = join(source_dir, rel)
        if not isfile(s) and rel not in protos:
            print('Removing dangling file', rel, 'while synchronizing', source_dir, 'into', target_dir)
            touch_update('rm ' + t)
            remove(t)


def sync_dir(source_relative, target_relative, complete=True):
    source = join(ipfs_chromium_source_dir, source_relative)
    if not isdir(source):
        print(f"Can't sync from dir '{source}', as it does not exist!!", file=stderr)
        exit(9)
    target = join(src, target_relative)
    (sources, protos) = copy_missing_and_changed_files(source, target)
    if complete:
        remove_danglers(source, target, sources, protos)
    sources.sort()
    protos.sort()
    formatted_sources = '\n'.join(map(lambda s: f'    "{s}",', sources))
    formatted_protos = '\n'.join(map(lambda s: f'    "{s}",', protos))
    build_in = join(source, 'BUILD.gn.in')
    if isfile(build_in):
        with open(build_in, encoding='utf-8') as bif:
            bn = bif.read()
        bn = bn.replace('@gn_sources@', formatted_sources)
        bn = bn.replace('@gn_protos@', formatted_protos)
        old_build_gn = join(target, 'BUILD.gn')
        obn = ''
        try:
            with open(old_build_gn, encoding='utf-8') as obif:
                obn = obif.read()
        except FileNotFoundError:
            pass
        if bn != obn:
            print("Updating BUILD.gn", file=stderr)
            open(old_build_gn, 'w', encoding='utf-8').write(bn)
            touch_update('BUILD.gn')


sync_dir('component', 'components/ipfs')
third = join(src, 'third_party')
ic_dir = join(third, 'ipfs_client')
print('ic_dir=', ic_dir)
if not isdir(ic_dir):
    # run([git_binary, '-C', join(src, 'third_party'), 'clone', '--branch', 'main', 'git@gitlab.com:jbt/ipfs_client.git'])
    ipfs_client_url = f'https://gitlab.com/jbt/ipfs_client/-/archive/{ipfs_client_version}/ipfs_client-{ipfs_client_version}.tar.gz'
    print('Extracting', ipfs_client_url, 'to', third)
    with requests.get(ipfs_client_url, stream=True, timeout=99) as rx, tarfile.open(fileobj=rx.raw, mode="r:gz") as tarobj:
        tarobj.extractall(third)
    extracted = join(third, f'ipfs_client-{ipfs_client_version}')
    print('Rename extracted directory from', extracted, 'to', ic_dir)
    rename(extracted, ic_dir)

if isdir(elec_dir):
    sync_dir('electron-spin', 'electron-spin')

if not branding_dir or len(branding_dir) == 0:
    verbose("Branding turned off via empty BRANDING_DIR.")
elif isdir(branding_dir):
    sync_dir(join(branding_dir, 'installer'), join('chrome', 'installer'), False)
    sync_dir(join(branding_dir, 'theme'), join('chrome', 'app', 'theme', 'chromium'), False)
else:
    print(f"BRANDING_DIR '{branding_dir}' does not point to a directory.", file=stderr)
    exit(8)
if 'PYTHONPATH' in environ:
    environ['PYTHONPATH'] = src + '/third_party/protobuf/python' + pathsep + environ['PYTHONPATH']
else:
    environ['PYTHONPATH'] = src + '/third_party/protobuf/python'

out = join(src, 'out', profile)
if not isdir(out):
    makedirs(out)
n = join(out, 'obj', 'components', 'ipfs', 'ipfs.ninja')
if not isfile(n) or (isfile(UPDATED) and getmtime(UPDATED) > getmtime(n)):
    gn_gen_cmd = [python, join(depot_tools_dir, 'gn.py'), 'gen', '--args=' + gnargs.replace("'", ""), out]
    verbose('Running gn gen', gn_gen_cmd)
    run(gn_gen_cmd)

for ninja_target in argv[2:]:
    print("Build target", ninja_target, file=stderr)
    run([python, join(depot_tools_dir, 'ninja.py'), '-C', out, '-j', jobs, ninja_target])
