#!/usr/bin/env python3


from version import deduce, on_tag

from os import chdir, environ, getcwd, listdir
from os.path import join, isdir, isfile, splitext
from subprocess import check_call, check_output
from sys import executable, stderr

GITHUB_ORG = 'little-bear-labs'
GITHUB_REPO = 'ipfs-chromium'
# GITHUB_ORG = 'John-LittleBearLabs'
# GITHUB_REPO = 'temp'

try:
    from ghapi.all import GhApi
except ModuleNotFoundError:
    check_call([executable, '-m', 'pip', 'install', 'ghapi'])
    from ghapi.all import GhApi

build_dir = getcwd()


def build_target(target):
    argv = ['cmake', '--build', build_dir, '--target', target]
    check_call(args=argv)


if not isfile('CMakeCache.txt'):
    print(__file__, 'must be run from inside the build directory.', file=stderr)
    exit(1)


def line_to_var(line):
    try:
        colon = line.index(':')
        k = line[0:colon]
        v = line[colon:]
        equal = v.index('=')
        return (k, v[equal+1:])
    except ValueError:
        return ('', None)


vars = check_output(['cmake', '-L', '-N', '-B', '.'], text=True).splitlines()
vars = dict(map(line_to_var, vars))
chromium_out = join(vars['CHROMIUM_SOURCE_TREE'], 'out', vars['CHROMIUM_PROFILE'])
tok = environ.get('GITHUB_TOKEN')
if tok:
    gh = GhApi()
else:
    print('GITHUB_TOKEN not set!', file=stderr)
    exit(2)
build_target('package_browser')
if not isdir(chromium_out):
    print(chromium_out, 'is not a directory?!', file=stderr)
    exit(3)
build_target('package')
version = deduce()
if on_tag():
    print('Will be adding to release', version)
    gh_rel = gh.repos.get_release_by_tag(owner=GITHUB_ORG, repo=GITHUB_REPO, tag=version)
else:
    print('Creating brand new release!')
    gh_rel = gh.repos.create_release(owner=GITHUB_ORG, repo=GITHUB_REPO, tag_name=version)
artifact_extensions = ['.tar.gz', '.rpm', '.deb', '.dmg']


def upload(contains):
    for f in listdir('.'):
        if contains not in f:
            continue
        for ext in artifact_extensions:
            if f.endswith(ext):
                print('Uploading', f, 'to', GITHUB_ORG, GITHUB_REPO, 'release', version)
                gh.upload_file(gh_rel, f)
                break


upload('client')
chdir(chromium_out)
upload('unstable')
