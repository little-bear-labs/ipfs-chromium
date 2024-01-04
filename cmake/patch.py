#!/usr/bin/env python3
import sys
from enum import auto, Enum
from glob import glob
from os import listdir, makedirs, remove
from os.path import exists, dirname, isdir, isfile, join, realpath, relpath, splitext
from shutil import copyfile, rmtree
from subprocess import call, check_call, check_output, DEVNULL
from sys import argv, executable, platform, stderr
from time import ctime
from verbose import verbose

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


def content_differs(ap,bp):
    if not isfile(ap) or not isfile(bp):
        return True
    with open(ap) as a:
        with open(bp) as b:
            return a.read() != b.read()

class Result(Enum):
    Output = auto()
    RawOutput = auto()
    OrDie = auto()
    ExitCode = auto()
    ExitCodeOnly = auto()
    StrippedOutput = Output

class Patcher:
    def __init__(self, chromium_source_dir, git_bin, build_type):
        self.csrc = chromium_source_dir
        self.pdir = realpath(join(dirname(__file__), '..', 'component', 'patches'))
        self.edir = realpath(join(dirname(__file__), '..', 'chromium_edits'))
        self.gbin = git_bin
        self.btyp = build_type
        self.up_rels = {}

    def create_patch_file(self):
        tag = self.tag_name()
        name = tag
        write_dir = join(self.edir, name)
        if exists(write_dir):
            rmtree(write_dir)
        paths = self.git(['diff', tag, '--name-only'], Result.RawOutput).splitlines()
        for lin in self.git(['status', '--porcelain'], Result.RawOutput).splitlines():
            if lin[0:3] != '?? ':
                continue
            verbose('Unversioned', lin[3:])
            paths.append( lin[3:] )
        for path in paths:
            from_path = join(self.csrc, path)
            to_path = join(write_dir, path)
            to_dir = dirname(to_path)
            if 'components/ipfs' in path:
                verbose('Not putting component into edit tree')
            elif 'third_party/ipfs_client' in path:
                verbose('Not putting library into edit tree')
            elif not self.file_in_branch(tag, path):
                print('Copy', from_path, '->', to_path)
                makedirs(to_dir, exist_ok=True)
                copyfile(from_path, to_path)
            elif not self.file_in_branch('HEAD', path):
                to_path += '.rm'
                print('Remembering the removal of', from_path, 'with', to_path)
                makedirs(to_dir, exist_ok=True)
                with open(to_path, 'w') as rm_f:
                    rm_f.write('//Remember to remove the corresponding file from the Chromium source tree')
            else:
                diff_out = self.git(['diff', '--patch', tag, path], Result.RawOutput)
                if diff_out:
                    makedirs(to_dir, exist_ok=True)
                    to_path += '.patch'
                    with open(to_path, 'w') as to_f:
                        to_f.write(diff_out)
                        print(to_path)
        self.git(['add', 'url/url_canon_ipfs.cc'], Result.OrDie)
        diff = self.git(['diff', '--patch', tag], Result.RawOutput)
        file_name = join(self.pdir, name+'.patch')
        print('Old patch file:', file_name)
        with open(file_name, 'w') as patch_file:
            patch_file.write(diff+"\n")

    def file_in_branch(self, ref: str, path: str):
        out = self.git(['ls-tree', '--name-only', ref, path], Result.Output)
        verbose('ls-tree gave me', out)
        return out == path

    def apply(self):
        win = ''
        win_dist = 9876543210
        for ref in self.available():
            d = self.distance(ref)
            print(d, 'commits away from', ref, file=stderr)
            if d < win_dist or (d == win_dist and len(ref) < len(win)):
                win_dist = d
                win = ref
        edits_dir = join(self.edir, win)
        edit_glob = f'{edits_dir}/**/*'
        print('Best edits version is', win, 'look for edits by', edit_glob, file=stderr)
        for edit in glob(edit_glob, recursive=True):
            if not isfile(edit):
                continue
            verbose('Have edit:', edit)
            ext = splitext(edit)[1]
            rel = relpath(edit, edits_dir)
            to_path = join(self.csrc, rel)
            if ext == '.patch':
                self.check_patch(edit, rel, to_path)
            elif ext == '.rm':
                if isfile(to_path):
                    print("Remove", to_path, 'due to', edit)
                    remove(to_path)
                else:
                    verbose(f"{to_path} already removed")
            elif not isfile(to_path):
                print('Copy', edit, '->', to_path)
                copyfile(edit, to_path)
            elif content_differs(edit, to_path):
                print('Warning:', to_path, 'exists, is different from ', edit, ' and is not being overwritten.')
            else:
                verbose(f"{to_path} already copied")
        verbose('Done patching')


    def check_patch(self, patch_path: str, relative: str, target_path: str):
        if 0 == self.git(['apply', '--check', '--reverse', '--verbose', patch_path], Result.ExitCodeOnly):
            verbose(patch_path, 'already applied.')
            return
        src = splitext(relative)[0]
        ec = self.git(['apply', '--verbose', patch_path], Result.ExitCode)
        verbose('Applying patch', patch_path, 'gave exit code', ec)
        if ec == 0:
            print('Patched', src, 'with', patch_path)
        else:
            with open(join(self.csrc,src)) as target_file:
                text = target_file.read()
                if 'ipfs' in text:
                    print("Patch file", patch_path, 'may have already been applied, or otherwise hand-edited. Ignoring.')
                else:
                    print("Failed to patch", src, '( at', join(self.csrc,src), ') with', patch_path)
                    exit(8)

    def git(self, args: list[str], result: Result) -> str:
        a = [self.gbin, '-C', self.csrc] + args
        verbose('Running', a)
        match result:
            case Result.RawOutput:
                return check_output(a, text=True)
            case Result.StrippedOutput:
                return check_output(a, text=True).strip()
            case Result.OrDie:
                check_call(a)
            case Result.ExitCode:
                return call(a)
            case Result.ExitCodeOnly:
                return call(a, stdout=DEVNULL, stderr=DEVNULL)
            case _:
                raise RuntimeError('result type not handled')

    def tag_name(self) -> str:
        return self.git(['describe', '--tags', '--abbrev=0'], Result.Output)

    def curr_hash(self) -> str:
        return self.hash_of('HEAD')

    def hash_of(self, ref) -> str:
        return self.git(['rev-parse', ref], Result.Output)

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
        a = int(self.git(['rev-list', '--count', frm+'..'+ref], Result.Output))
        b = int(self.git(['rev-list', '--count', ref+'..'+frm], Result.Output))
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
        fudge = 59893
        def check(version, version_set, s):
            i = as_int(version)
            by = (fudge,0)
            for a in avail:
                d = abs(a-i)
                if d < fudge:
                    return True
                elif d < by[0]:
                    by = ( d, a )
            if version not in version_set:
                sortable = [int(c) for c in version.split('.')]
                version_set[version] = [sortable, version, s]
            elif s not in version_set[version]:
                #print('2 Adding',version,s)
                version_set[version].append(s)
            return False
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

    def out_of_date(self, p):
        dir_path = f'{self.edir}/{p}'
        if not isdir(dir_path):
            return True
        file_path = f'{self.pdir}/{p}.patch'
        with open(file_path) as f:
            lines = f.readlines()
            if not Patcher.has_file_line(lines, 'chrome/browser/flag-metadata.json', '+    "name": "enable-ipfs",'):
                verbose(p, 'does not have enable-ipfs in flag-metadata.json', file_path, file=sys.stderr)
                return True
            if not Patcher.has_file_line(lines, 'chrome/browser/chrome_content_browser_client.cc', '+    main_parts->AddParts(std::make_unique<IpfsExtraParts>());'):
                verbose(p, 'does not have enable-ipfs in flag-metadata.json', file_path, file=sys.stderr)
                return True
        return False

    @staticmethod
    def has_file_line(lines: list[str], path: str, line: str):
        fl = Patcher.file_lines(lines, path)
        return (line + '\n') in fl

    @staticmethod
    def file_lines(lines: list[str], path):
        start = f'diff --git a/{path} b/{path}\n'
        if not start in lines:
            # print('Did not find',start,'in',lines)
            return []
        i = lines.index(start) + 1
        for j in range(i, i + 9999):
            if len(lines) == j or lines[j].startswith('diff --git'):
                return lines[i:j]
        return []

    def list_ood(self, to_check: list[str], sense: bool):
        to_check.sort()
        for p in to_check:
            if self.out_of_date(p) == sense:
                print(p)


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
        for chan in ['Dev', 'Beta', 'Stable', 'Extended']:
            for os in ['Linux', 'Mac', 'Windows']:
                rels = p.release_versions(chan, os)
                if len(rels) > 0:
                    print(f'     {chan:9}{os:7}', rels[0][1])
                if len(rels) > 1:
                    print(f'Prev {chan:9}{os:7}', rels[1][1])
        print("Electron's main branch:", p.electron_version())
    elif argv[1] == 'old':
        pr = Patcher('/mnt/big/lbl/code/chromium/src', 'git', 'Debug')
        if len(argv) > 2:
            pr.list_ood(argv[2:], True)
        else:
            pr.list_ood(list(pr.available()), True)
    elif argv[1] == 'new':
        pr = Patcher('/mnt/big/lbl/code/chromium/src', 'git', 'Debug')
        if len(argv) > 2:
            pr.list_ood(argv[2:], False)
        else:
            pr.list_ood(list(pr.available()), False)
    elif argv[1] == 'git':
        pr = Patcher(realpath(join(dirname(__file__), '..')), 'git', 'Debug')
        pre = '?? component/patches/'
        suf = 'patch'
        for line in pr.git(['status','--porcelain'], Result.RawOutput).splitlines():
            if line.startswith(pre) and line.endswith(suf):
                end = len(line) - len(suf) - 1
                pch = line[len(pre):end]
                if pr.out_of_date(pch):
                    exit(9)
                else:
                    pr.git(['add', line[3:]], Result.OrDie)
    else:
        Patcher(*argv[1:]).create_patch_file()
