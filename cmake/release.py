#!/usr/bin/env python3


from version import deduce, on_tag

from glob import glob
from os import chdir, environ, getcwd, listdir
from os.path import join, isdir, isfile
from subprocess import call, check_call, check_output
from sys import argv, executable, stderr

GITHUB_ORG = 'little-bear-labs'
GITHUB_REPO = 'ipfs-chromium'

version = deduce()
build_dir = getcwd()

try:
    from ghapi.all import GhApi
except ModuleNotFoundError:
    call([executable, '-m', 'pip', '--user', 'install', 'ghapi'])
    try:
        from ghapi.all import GhApi
    except ModuleNotFoundError:
        for artifact in glob(f"{build_dir}/ipfs-client-*"):
            args = ['gh', 'release', 'upload', version, artifact]
            print('Running via cli forking', args)
            call(args)
        exit(0)


def cli_arg(a):
    if a in argv:
        argv.remove(a)
        return True
    return False


library_only = cli_arg('--library-only')
if len(argv) > 1:
    print('Unrecognized arguments!', argv[1:], file=stderr)
    exit(1)


def build_target(target):
    argvs = ['cmake', '--build', build_dir, '--config', 'Release', '--target', target]
    check_call(args=argvs)


if not isfile('CMakeCache.txt'):
    print(__file__, 'must be run from inside the build directory.', file=stderr)
    exit(2)


def line_to_var(line):
    try:
        colon = line.index(':')
        k = line[0:colon]
        v = line[colon:]
        equal = v.index('=')
        return (k, v[equal+1:])
    except ValueError:
        return ('', None)


cmak_vars = check_output(['cmake', '-L', '-N', '-B', '.'], text=True).splitlines()
cmak_vars = dict(map(line_to_var, cmak_vars))
chromium_out = join(cmak_vars['CHROMIUM_SOURCE_TREE'], 'out', cmak_vars['CHROMIUM_PROFILE'])
tok = environ.get('GITHUB_TOKEN')
if tok:
    gh = GhApi()
else:
    print('GITHUB_TOKEN not set!', file=stderr)
    exit(3)
if not library_only:
    build_target('package_browser')
    if not isdir(chromium_out):
        print(chromium_out, 'is not a directory?!', file=stderr)
        exit(4)
build_target('package')

if on_tag():
    print('Will be adding to release', version)
    gh_rel = gh.repos.get_release_by_tag(owner=GITHUB_ORG, repo=GITHUB_REPO, tag=version)
else:
    print('Creating brand new release!')
    gh_rel = gh.repos.create_release(owner=GITHUB_ORG, repo=GITHUB_REPO, tag_name=version)
artifact_extensions = ['.tar.gz', '.rpm', '.deb', '.dmg', '.zip']


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
if library_only:
    exit(0)
chdir(chromium_out)
upload('unstable')
