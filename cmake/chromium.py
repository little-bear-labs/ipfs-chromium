#!/usr/bin/env python3

from cache_vars import build_dir, vars, verbose
from patch import Patcher

from glob import glob
from os import environ, makedirs, remove
from os.path import dirname, getmtime, isdir, isfile, join, pathsep, relpath, splitext
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
UPDATED = 'chromium_source_updated'


def run(args, fail_ok=False):
    if isdir(src):
        verbose(src, 'exists. Run command there.')
        cwd = src
    elif isdir(chromium_dir):
        cwd = chromium_dir
    else:
        cwd = None
    verbose('Run', args, 'in', cwd)
    res = subprocess.run(args=args, cwd=cwd)
    # print(res)
    ec = res.returncode
    if ec == 0:
        return True
    print('Command failed with exit code', ec, ':', args, file=stderr)
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
    verbose('Run', args, 'in', cwd, 'and return output.')
    res = subprocess.run(args=args, cwd=cwd, stdout=subprocess.PIPE)
    output = res.stdout.decode('utf-8').strip()
    if res.returncode != 0:
        print('Command failed', res, ':', args, 'output was', output, file=stderr)
        exit(res.returncode)
    return output


def runpy(args, silent=False):
    if silent:
        out([python] + args)
    else:
        run([python] + args)


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
    open(join(build_dir, 'fresh'), 'a').close()

if isfile(join(build_dir, 'fresh')):
    if not isdir(join(src, '.git')):
        runpy([join(depot_tools_dir, 'fetch.py'), '--nohooks', 'chromium'])
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

with open(join(src, 'chrome', 'browser', 'BUILD.gn')) as w:
    content = w.read()
    if 'components/ipfs' in content:
        verbose('Chromium seems to be already patched.')
    else:
        print('Apply patch file...', file=stderr)
        patcher.apply()

ipfs_dir = join(src, 'components', 'ipfs')


def files_content_differ(a, b):
    try:
        with open(a) as af:
            with open(b) as bf:
                ac = af.read()
                bc = bf.read()
                return ac != bc
    except FileNotFoundError:
        return True


source_exts = ['.h', '.cc', '.hpp']
ignore_exts = ['.patch', '.orig', '.txt', '.in']


def touch_update(s):
    with open(UPDATED, 'w') as f:
        f.write(s)


def copy_missing_and_changed_files(source, target):
    sources = []
    protos = []
    for s in glob(f'{source}/**/*', recursive=True):
        rel = relpath(s, source)
        t = join(target, rel)
        if isdir(s):
            if not isdir(t):
                makedirs(t)
            continue
        if s.endswith('_unittest.cc'):
            continue
        ext = splitext(s)[-1]
        if ext in ignore_exts:
            continue
        if ext in source_exts:
            sources.append(rel)
        elif ext == '.proto':
            rel = rel.replace('proto/ipfs_client/', '')
            t = join(target, rel)
            verbose('s', s, 'rel', rel, 't', t)
            protos.append(rel)
        if files_content_differ(s, t):
            print('Copying', s, 'to', t, 'while synchronizing', source, 'to', target)
            if not isdir(dirname(t)):
                makedirs(dirname(t))
            touch_update(s+t)
            copyfile(s, t)
    return (sources, protos)


def remove_danglers(source, target, sources, protos):
    for t in glob(f'{target}/**/*', recursive=True):
        if isdir(t):
            continue
        rel = relpath(t, target)
        if rel == 'BUILD.gn' or rel in sources or rel in protos:
            continue
        s = join(source, rel)
        if not isfile(s) and rel not in protos:
            print('Removing dangling file', rel, 'while synchronizing', source, 'into', target)
            touch_update('rm '+t)
            remove(t)


def sync_dir(source_relative, target_relative, complete=True):
    source = join(ipfs_chromium_source_dir, source_relative)
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
        with open(build_in) as bif:
            bn = bif.read()
        bn = bn.replace('@gn_sources@', formatted_sources)
        bn = bn.replace('@gn_protos@', formatted_protos)
        old_build_gn = join(target, 'BUILD.gn')
        obn = ''
        try:
            with open(old_build_gn) as obif:
                obn = obif.read()
        except Exception:
            pass
        if bn != obn:
            print("Updating BUILD.gn", file=stderr)
            open(old_build_gn, 'w').write(bn)
            touch_update('BUILD.gn')


sync_dir('component', 'components/ipfs')
sync_dir('library', 'third_party/ipfs_client')
if 'branding' in argv:
    argv.remove('branding')
    sync_dir('component/branding', 'chrome', False)
if 'PYTHONPATH' in environ:
    environ['PYTHONPATH'] = src + '/third_party/protobuf/python' + pathsep + environ['PYTHONPATH']
else:
    environ['PYTHONPATH'] = src + '/third_party/protobuf/python'

out = join(src, 'out', profile)
if not isdir(out):
    makedirs(out)
n = join(out, 'obj', 'components', 'ipfs', 'ipfs.ninja')
if not isfile(n) or (isfile(UPDATED) and getmtime(UPDATED) > getmtime(n)):
    a = [python, join(depot_tools_dir, 'gn.py'), 'gen', '--args='+gnargs.replace("'", ""), out]
    print('Running gn gen', a)
    run(a)

for target in argv[2:]:
    print("Build target", target, file=stderr)
    run([python, join(depot_tools_dir, 'ninja.py'), '-C', out, '-j', jobs, target])
