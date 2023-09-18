#!/usr/bin/env python3

from os import listdir, remove
from os.path import dirname, join, realpath, splitext
from subprocess import check_call, check_output
from sys import argv, executable, platform, stderr
from time import ctime

try:
    import requests
except ModuleNotFoundError:
    check_call([executable, '-m', 'pip', 'install', 'requests'])
    import requests


def osname():
    if platform == 'linux':
        return 'Linux'
    if platform == 'darwin':
        return 'Mac'
    return 'Windows'


def as_int(v):
    result = 0
    try:
        for c in v.split('.'):
            result *= 10000
            result += int(c)
    except ValueError:
        return 0
    return result



class Patcher:
    def __init__(self, chromium_source_dir, git_bin, build_type):
        self.csrc = chromium_source_dir
        self.pdir = realpath(join(dirname(__file__), '..', 'component', 'patches'))
        self.gbin = git_bin
        self.btyp = build_type
        self.up_rels = {}

    def create_patch_file(self):
        tag = self.tag_name()
        self.git(['add', 'url/url_canon_ipfs.cc'])
        diff = self.git(['diff', '--patch', tag])
        name = tag
        if self.curr_hash() != self.hash_of(tag):
            print('NOT ON A TAG. Patching for hash instead')
            name = self.curr_hash()
        file_name = join(self.pdir, name+'.patch')
        print('Patch file:', file_name)
        with open(file_name, 'w') as patch_file:
            patch_file.write(diff+"\n")

    def apply(self):
        win = ''
        win_dist = 9876543210
        for ref in self.available():
            d = self.distance(ref)
            print(d, 'commits away from', ref, file=stderr)
            if d < win_dist or (d == win_dist and len(ref) < len(win)):
                win_dist = d
                win = ref
        print('Best patch file is', win, file=stderr)
        patch_path = join(self.pdir, win+'.patch')
        self.git(['apply', '--verbose', patch_path], out=False)

    def git(self, args, out=True) -> str:
        if out:
            return check_output([self.gbin, '-C', self.csrc] + args, text=True).strip()
        else:
            check_call([self.gbin, '-C', self.csrc] + args, text=True)
            return ''

    def tag_name(self) -> str:
        return self.git(['describe', '--tags', '--abbrev=0'])

    def curr_hash(self) -> str:
        return self.hash_of('HEAD')

    def hash_of(self, ref) -> str:
        return self.git(['rev-parse', ref])

    def distance(self, ref) -> int:
        a, b = self.distances('HEAD', ref)
        return a + b

    def recommend(self) -> str:
        channels = ['Dev', 'Beta', 'Stable', 'Extended']
        avail = list(self.available())
        if self.btyp == 'Release':
            channels.reverse()
        print('Avail:', avail)
        for channel in channels:
            print("Considering channel", channel, file=stderr)
            versions = self.release_versions(channel)
            for when, version in versions:
                print(f"Found a release version: '{version}'", file=stderr)
                if version in avail:
                    print('Suggesting version', version, 'which was the', osname(), channel, 'channel as of', ctime(when), file=stderr)
                    return version
        raise EnvironmentError(f"Can't find an appropriate tag for {osname()}, anymore!")

    def available(self):
        for f in listdir(self.pdir):
            if splitext(f)[1] != '.patch':
                print(f"Warning: {f} does not belong in {self.pdir}. Removing.", file=stderr)
                remove(join(self.pdir, f))
        return map(lambda p: splitext(p)[0], listdir(self.pdir))

    def distances(self, frm, ref):
        a = int(self.git(['rev-list', '--count', frm+'..'+ref]))
        b = int(self.git(['rev-list', '--count', ref+'..'+frm]))
        return (a, b)

    def maybe_newer(self, x, y):
        a, b = self.distances(x, y)
        if a > b or (a == b and len(x) < len(y)):
            return y
        else:
            return x

    def release_versions(self, channel, pfrm=None):
        if pfrm is None:
            pfrm = osname()
        key = pfrm + channel
        if key in self.up_rels:
            return self.up_rels[key]
        parms = {'platform': pfrm, 'channel': channel}
        chrom_url = 'https://chromiumdash.appspot.com/fetch_releases'
        resp = requests.get(url=chrom_url, params=parms)
        result = list(map(lambda x: (x['time'] / 1000, x['version']), resp.json()))
        elec_url = 'https://raw.githubusercontent.com/electron/electron/main/DEPS'
        resp = requests.get(url=elec_url)
        result.sort(reverse=True)
        self.up_rels[key] = result
        return result

    def electron_version(self, branch='main'):
        if 'electron-main' in self.up_rels:
            return self.up_rels['electron-main']
        resp = requests.get(f'https://raw.githubusercontent.com/electron/electron/{branch}/DEPS')
        toks = resp.text.split("'")
        i = toks.index('chromium_version') + 2
        self.up_rels['electron-main'] = toks[i]
        return toks[i]


    def unavailable(self):
        avail = list(map(as_int, self.available()))
        version_set = {}
        fuzz = 99999
        def check(version, version_set, s):
            i = as_int(version)
            if any(abs(a-i)<fuzz for a in avail):
                return True
            if version not in version_set:
                sortable = [int(c) for c in version.split('.')]
                # print('Adding',version,s)
                version_set[version] = [sortable, version, s]
            elif s not in version_set[version]:
                print('2 Adding',version,s)
                version_set[version].append(s)
            return False
        while fuzz >= 0 and len(version_set) < 2:
            fuzz -= 1
            for channel in ['Dev', 'Beta', 'Stable', 'Extended']:
                for pfrm in ['Linux', 'Mac', 'Windows']:
                    try:
                        when, version = self.release_versions(channel, pfrm)[0]
                        s = f"{channel}-{pfrm}-{when}"
                        check(version, version_set, s)
                    except IndexError:
                        pass  # One may assume this is Linux Extended
            e = self.electron_version()
            check(e, version_set, 'electron-main')
        result = list(version_set.values())
        result.sort(reverse=True)
        return map(lambda x: x[1:], result)


if __name__ == '__main__':
    if argv[1] == 'apply':
        Patcher('/mnt/big/lbl/code/chromium/src', 'git', 'Debug').apply()
    elif argv[1] == 'rec':
        print(osname())
        print(Patcher('/mnt/big/lbl/code/chromium/src', 'git', argv[2]).recommend())
    elif argv[1] == 'missing':
        missing = Patcher('/mnt/big/lbl/code/chromium/src', 'git', 'Debug').unavailable()
        for m in missing:
            print(m)
    elif argv[1] == 'releases':
        p = Patcher('/mnt/big/lbl/code/chromium/src', 'git', 'Debug')
        for os in ['Linux', 'Mac', 'Windows']:
            for chan in ['Dev', 'Beta', 'Stable', 'Extended']:
                rels = p.release_versions(chan, os)
                for rel in rels:
                    print(chan, os, rel)
    else:
        Patcher(*argv[1:]).create_patch_file()
