#!/usr/bin/env python3

from enum import auto, Enum
from glob import glob
import os
from os.path import (
    basename,
    exists,
    dirname,
    isdir,
    isfile,
    join,
    realpath,
    relpath,
    splitext,
)
from shutil import copyfile, rmtree
from subprocess import call, CalledProcessError, check_call, check_output, DEVNULL
import sys
from sys import argv, executable, platform, stderr
from time import ctime, time
from verbose import verbose

try:
    import requests
except ModuleNotFoundError as ex:
    check_call([executable, "-m", "pip", "install", "requests"])
    import requests
    verbose('Installed requests because of', ex)


VERSION_CLOSE_ENOUGH = 30123
LARGE_INT = 9876543210
here = dirname(__file__)


def osname() -> str:
    if platform == "linux":
        return "Linux"
    if platform == "darwin":
        return "Mac"
    return "Windows"


def as_int(ver_str):
    result = 0
    try:
        for c in ver_str.split("."):
            result *= 10000
            result += int(c)
    except ValueError:
        return 0
    return result


def closest_by_version(needle, haystack):
    i = as_int(needle)
    return min(map(lambda x: (abs(as_int(x) - i), x), haystack))


def content_differs(ap, bp):
    if not isfile(ap) or not isfile(bp):
        return True
    with open(ap, encoding="utf-8") as a:
        with open(bp, encoding="utf-8") as b:
            return a.read() != b.read()


class Result(Enum):
    Output = auto()
    RawOutput = auto()
    OrDie = auto()
    ExitCode = auto()
    ExitCodeOnly = auto()
    StrippedOutput = Output


class Patcher:
    def __init__(self, chromium_source_dir, git_bin='git', build_type=None):
        self.csrc = chromium_source_dir
        self.pdir = realpath(join(here, "..", "component", "patches"))
        self.edir = realpath(join(here, "..", "chromium_edits"))
        self.gbin = git_bin
        self.btyp = build_type
        self.up_rels = {}

    def create_patch_file(self):
        tag = self.tag_name()
        name = tag
        write_dir = join(self.edir, name)
        if exists(write_dir):
            rmtree(write_dir)
        paths = self.git(["diff", tag, "--name-only"], Result.RawOutput).splitlines()
        for lin in self.git(["status", "--porcelain"], Result.RawOutput).splitlines():
            if lin[0:3] != "?? ":
                continue
            verbose("Unversioned", lin[3:])
            paths.append(lin[3:])
        for path in paths:
            if path.endswith(".gz") or "." not in basename(path):
                verbose(f"Not recorded because the path doesn't seem to be one we should: {path}")
                continue
            from_path = join(self.csrc, path)
            to_path = join(write_dir, path)
            to_dir = dirname(to_path)
            if "components/ipfs" in path:
                verbose("Not putting component into edit tree")
            elif "third_party/ipfs_client" in path:
                verbose("Not putting library into edit tree")
            elif "electron" in path:
                verbose("electron and electron-spin are handled separately")
            elif isdir(from_path):
                verbose("Ignoring unversioned directory.")
            elif '/theme/' in path or path.endswith('.info'):
                verbose("Ignoring branding.", path)
            elif not self.__file_in_branch(tag, path):
                print("Copy", from_path, "->", to_path)
                os.makedirs(to_dir, exist_ok=True)
                copyfile(from_path, to_path)
            elif not self.__file_in_branch("HEAD", path):
                to_path += ".rm"
                print("Remembering the removal of", from_path, "with", to_path)
                os.makedirs(to_dir, exist_ok=True)
                with open(to_path, "w", encoding='utf-8') as rm_f:
                    rm_f.write(
                        "//Remember to remove the corresponding file from the Chromium source tree"
                    )
            else:
                diff_out = self.git(["diff", "--patch", tag, path], Result.RawOutput)
                if diff_out:
                    os.makedirs(to_dir, exist_ok=True)
                    to_path += ".patch"
                    with open(to_path, "w", encoding='utf-8') as to_f:
                        to_f.write(diff_out)
                        print(to_path)
        self.git(["add", "url/url_canon_ipfs.cc"], Result.OrDie)

    def __file_in_branch(self, ref: str, path: str):
        out = self.git(["ls-tree", "--name-only", ref, path], Result.Output)
        verbose("ls-tree gave me", out)
        return out == path

    def __closest_by_git(self):
        win = ""
        win_dist = LARGE_INT
        for ref in self.available():
            d = self.distance(ref)
            verbose(d, "commits away from", ref)
            if d < win_dist or (d == win_dist and len(ref) < len(win)):
                win_dist = d
                win = ref
        verbose("Best edits version is", win)
        return win

    def apply(self):
        edits_dir = join(self.edir, self.__closest_by_git())
        edit_glob = f"{edits_dir}/**/*"
        print("Look for edits by", edit_glob, file=stderr)
        for edit in glob(edit_glob, recursive=True):
            if not isfile(edit):
                continue
            verbose("Have edit:", edit)
            ext = splitext(edit)[1]
            rel = relpath(edit, edits_dir)
            to_path = join(self.csrc, rel)
            if ext == ".patch":
                self.check_patch(edit, rel)
            elif ext == ".rm":
                if isfile(to_path):
                    print("Remove", to_path, "due to", edit)
                    os.remove(to_path)
                else:
                    verbose(f"{to_path} already removed")
            elif not isfile(to_path):
                print("Copy", edit, "->", to_path)
                copyfile(edit, to_path)
            elif content_differs(edit, to_path):
                print(
                    "Warning:",
                    to_path,
                    "exists, is different from ",
                    edit,
                    " and is not being overwritten.",
                )
            else:
                verbose(f"{to_path} already copied")
        verbose("Done patching")

    @staticmethod
    def edit_evidence(text: str):
        for tok in ['ipfs', 'ReadTagContent', 'SetTag(']:
            if tok in text:
                return True
        return False

    def check_patch(self, patch_path: str, relative: str):
        if 0 == self.git(
            ["apply", "--check", "--reverse", "--verbose", patch_path],
            Result.ExitCodeOnly,
        ):
            verbose(patch_path, "already applied.")
            return
        src = splitext(relative)[0]
        ec = self.git(["apply", "--verbose", patch_path], Result.ExitCode)
        verbose("Applying patch", patch_path, "gave exit code", ec)
        if ec == 0:
            print("Patched", src, "with", patch_path)
        else:
            with open(join(self.csrc, src), encoding='utf-8') as target_file:
                text = target_file.read()
                if self.edit_evidence(text):
                    print(
                        "Patch file ",
                        patch_path,
                        " may have already been applied, ",
                        "or otherwise hand-edited. Ignoring.",
                    )
                else:
                    print(
                        "Failed to patch",
                        src,
                        "( at",
                        join(self.csrc, src),
                        ") with",
                        patch_path,
                    )
                    sys.exit(8)

    def git(self, args: list[str], result: Result) -> str:
        a = [self.gbin, "-C", self.csrc] + args
        verbose("Running", a)
        if result == Result.RawOutput:
            return check_output(a, text=True)
        if result == Result.StrippedOutput:
            return check_output(a, text=True).strip()
        if result == Result.OrDie:
            return check_call(a)
        if result == Result.ExitCode:
            return call(a)
        if result == Result.ExitCodeOnly:
            return call(a, stdout=DEVNULL, stderr=DEVNULL)
        raise RuntimeError("result type not handled")

    def tag_name(self) -> str:
        return self.git(["describe", "--tags", "--abbrev=0"], Result.Output)

    def curr_hash(self) -> str:
        return self.hash_of("HEAD")

    def hash_of(self, ref) -> str:
        '''Get the hash of a Git ref from the Chromium tree'''
        return self.git(["rev-parse", ref], Result.Output)

    def distance(self, ref) -> int:
        try:
            a, b = self.distances("HEAD", ref)
            return a + b
        except CalledProcessError:
            return 1_234_567_890

    def recommend(self) -> str:
        channels = ["Dev", "Beta", "Stable", "Extended"]
        avail = list(self.available())
        if self.btyp == "Release":
            channels.reverse()
        verbose("Avail:", avail)
        win_cand = ""
        win_dist = LARGE_INT
        for channel in channels:
            print("Considering channel", channel, file=stderr)
            versions = self.release_versions(channel)
            for when, rel_ver in versions:
                verbose(f"Found a release version: '{rel_ver}' from {ctime(when)}")
                dist, cand = closest_by_version(rel_ver, avail)
                if dist < win_dist:
                    t = f"Suggesting {cand} which is {dist} close to {rel_ver} which was {channel} @ {ctime(when)}"
                    print(t, file=stderr)
                    win_dist = dist
                    win_cand = cand
                else:
                    win_dist -= 9
            if win_dist < LARGE_INT / 2:
                break
        if win_cand:
            return win_cand
        else:
            raise EnvironmentError(
                f"Can't find an appropriate tag for {osname()}, anymore!"
            )

    def available(self):
        return os.listdir(self.edir)

    def distances(self, frm, ref):
        a = int(self.git(["rev-list", "--count", frm + ".." + ref], Result.Output))
        b = int(self.git(["rev-list", "--count", ref + ".." + frm], Result.Output))
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
        parms = {"platform": pfrm, "channel": channel}
        chrom_url = "https://chromiumdash.appspot.com/fetch_releases"
        resp = requests.get(url=chrom_url, params=parms, timeout=999)
        result = list(map(lambda x: (x["time"] / 1000, x["version"]), resp.json()))
        elec_url = "https://raw.githubusercontent.com/electron/electron/main/DEPS"
        resp = requests.get(url=elec_url, timeout=999)
        result.sort(reverse=True)
        self.up_rels[key] = result
        return result

    def date_of(self, v):
        try:
            git_time = self.git(
                ["log", "--max-count=1", "--format=%at", v], Result.StrippedOutput
            )
            verbose("Git time of", v, "is", git_time)
            return float(git_time)
        except CalledProcessError:
            return time()

    def most(self, ch, pfs, idx):
        vs = []
        for pf in pfs:
            vs = vs + self.release_versions(ch, pf)
        vs = list(map(lambda x: (as_int(x[1]), x[1], x[0]), vs))
        vs += list(
            map(
                lambda x: (as_int(x), x, self.date_of(x)),
                self.electron_versions().values(),
            )
        )
        vs.sort()
        verbose(f"'Most' ({idx}) versions:", vs)
        return vs[idx]

    def newest(self):
        return self.most("Dev", ["Linux", "Mac", "Windows"], -1)

    def oldest(self):
        return self.most("Extended", ["Mac", "Windows"], 0)

    def recent_electron_branch_version(self):
        with open(join(here, "recent-electron.txt"), encoding="utf-8") as revf:
            return int(revf.read())

    def set_recent_electron_branch_version(self, v):
        with open(join(here, "recent-electron.txt"), "w", encoding="utf-8") as revf:
            revf.write(str(v))
            call([self.gbin, "add", join("cmake", "recent-electron.txt")])

    def electron_version(self, branch="main"):
        if f"electron-{branch}" in self.up_rels:
            return self.up_rels[f"electron-{branch}"]
        u = f"https://raw.githubusercontent.com/electron/electron/{branch}/DEPS"
        resp = requests.get(u, timeout=999)
        toks = resp.text.split("'")
        i = toks.index("chromium_version") + 2
        self.up_rels[f"electron-{branch}"] = toks[i]
        return toks[i]

    def electron_versions(self):
        rebv = self.recent_electron_branch_version()
        try:
            ev = rebv + 1
            self.electron_version(f"{ev}-x-y")
            verbose("New electron version!", ev)
            self.set_recent_electron_branch_version(ev)
            rebv = ev
        except ValueError as e:
            verbose("Couldn't get next, possibly-nonexistent, electron version's chromium version.", e, type(e))
        result = {}
        for i in range(-2, 1):
            ev = i + rebv
            result[ev] = self.electron_version(f"{ev}-x-y")
        verbose("Electron versions:", result)
        return result

    def unavailable(self):
        avail = list(map(as_int, self.available()))
        version_set = {}

        def check(version, version_set, s, close):
            verbose("For ", s, " close enough is ", close)
            i = as_int(version)
            by = (close, 0)
            for a in avail:
                d = abs(a - i)
                if d < close:
                    return True
                elif d < by[0]:
                    by = (d, a)
            if version not in version_set:
                sortable = [int(c) for c in version.split(".")]
                version_set[version] = [sortable, version, s]
            elif s not in version_set[version]:
                version_set[version].append(s)
            return False

        for ci, channel in enumerate(["Extended", "Stable", "Beta", "Dev"]):
            for pi, pfrm in enumerate(["Linux", "Mac", "Windows"]):
                try:
                    when, version = self.release_versions(channel, pfrm)[0]
                    s = f"{channel}-{pfrm}-{when}"
                    close = VERSION_CLOSE_ENOUGH * (ci + 1) * (pi + 1)
                    check(version, version_set, s, close)
                except IndexError:
                    pass  # One may assume this is Linux Extended
        close = VERSION_CLOSE_ENOUGH
        for ev, e in self.electron_versions().items():
            close += VERSION_CLOSE_ENOUGH * 2
            check(e, version_set, f"electron-{ev}", close)
        result = list(version_set.values())
        result.sort(reverse=True)
        return map(lambda x: x[1:], result)

    def out_of_date(self, p):
        dir_path = f"{self.edir}/{p}"
        if not isdir(dir_path):
            return True
        file_path = join(dir_path, "components/cbor/reader_unittest.cc.patch")
        if not isfile(file_path):
            return True
        with open(file_path, encoding="utf-8") as f:
            lines = list(map(lambda x: x.strip(), f.readlines()))
            if (
                "+  absl::optional<Value> cbor = Reader::Read(kTaggedCbor, config);"
                in lines
            ):
                verbose(p, "Still relying on absl::optional in unit tests", file_path)
                return True
        file_path = join(dir_path, "chrome/installer/linux/common/desktop.template.patch")
        if not isfile(file_path):
            verbose('No patching of', file_path)
            return True
        with open(file_path, encoding="utf-8") as f:
            lines = list(map(lambda x: x.strip(), f.readlines()))
            if not any(map(lambda x: 'x-scheme-handler/ipfs' in x, lines)):
                verbose(p, "No ipfs scheme in Linux installer", file_path)
                return True
        file_path = f"{self.pdir}/{p}.patch"
        if not isfile(file_path):
            return False
        with open(file_path, encoding="utf-8") as f:
            lines = list(map(lambda x: x.strip(), f.readlines()))
            if not Patcher.has_file_line(
                lines,
                "chrome/browser/flag-metadata.json",
                '+    "name": "enable-ipfs",',
            ):
                verbose(p, "does not have enable-ipfs in flag-metadata.json", file_path)
                return True
            if not Patcher.has_file_line(
                lines,
                "chrome/browser/chrome_content_browser_client.cc",
                "+    main_parts->AddParts(std::make_unique<IpfsExtraParts>());",
            ):
                verbose(p, "does not have enable-ipfs in flag-metadata.json", file_path)
                return True
        return False

    @staticmethod
    def has_file_line(lines: list[str], path: str, line_txt: str):
        fl = Patcher.file_lines(lines, path)
        return (line_txt + "\n") in fl

    @staticmethod
    def file_lines(lines: list[str], path):
        start = f"diff --git a/{path} b/{path}\n"
        if start not in lines:
            # print('Did not find',start,'in',lines)
            return []
        i = lines.index(start) + 1
        for j in range(i, i + 9999):
            if len(lines) == j or lines[j].startswith("diff --git"):
                return lines[i:j]
        return []

    def list_ood(self, to_check: list[str], sense: bool, both: bool = False):
        to_check.sort()
        oldest = self.oldest()
        newest = self.newest()
        gap = newest[0] - oldest[0] + VERSION_CLOSE_ENOUGH
        min_ver = oldest[0] - gap
        min_date = time() - 3600 * 24 * 31 * 6
        verbose(
            f"Oldest supportable version: {oldest} , newest was: {newest} , gap: {gap} -> {min_ver}"
        )
        for p in to_check:
            d = self.date_of(p)
            verbose("Checking", p, d, ctime(d))
            if self.out_of_date(p):
                verbose(p, "failed compatibility checks")
                is_ood = True
            elif d < min_date and as_int(p) < min_ver:
                verbose(d, "<", min_date, "and", as_int(p), "<", min_ver)
                is_ood = True
            else:
                is_ood = False
            if both:
                state = 'Old' if is_ood else 'New'
                print(p, state)
            elif is_ood == sense:
                print(p)


if __name__ == "__main__":
    chromium_src_dir = os.environ['CHROMIUM_SOURCE_TREE']
    if len(argv) < 2:
        print("Give an argument to indicate what you'd like to do.")
    elif argv[1] == "apply":
        Patcher(chromium_src_dir, "git", "Debug").apply()
    elif argv[1] == "rec":
        print(osname())
        BT = "Release" if len(argv) == 2 else argv[2]
        print(Patcher(chromium_src_dir, "git", BT).recommend())
    elif argv[1] == "missing":
        missing = Patcher(
            chromium_src_dir, "git", "Debug"
        ).unavailable()
        for m in missing:
            print(m)
    elif argv[1] == "releases":
        per = Patcher(chromium_src_dir, "git", "Debug")
        for chan in ["Dev", "Beta", "Stable", "Extended"]:
            for osn in ["Linux", "Mac", "Windows"]:
                rels = per.release_versions(chan, osn)
                for (step_desc,verinf) in zip(['Cur', 'Prv', 'Old'], rels):
                    print(f"{verinf[1]:15} {step_desc} {chan:9}{os:7}")
        for (elec_br,chrom_ver) in per.electron_versions().items():
            print(f"{chrom_ver:15} Electron {elec_br}")
        print(f"{per.electron_version():15} Electron main")
        o = per.oldest()
        print(f"{o[1]:15} Oldest maintained Extended:", f"({ctime(o[2])})")
        n = per.newest()
        print("Development at:", n[1], f"({ctime(n[2])})")
    elif argv[1] == "available":
        pr = Patcher(chromium_src_dir, "git", "Debug")
        print(list(pr.available()))
        print(pr.edir)
    elif argv[1] == "old":
        pr = Patcher(chromium_src_dir, "git", "Debug")
        if len(argv) > 2:
            pr.list_ood(argv[2:], True)
        else:
            pr.list_ood(list(pr.available()), True)
    elif argv[1] == "new":
        pr = Patcher(chromium_src_dir, "git", "Debug")
        if len(argv) > 2:
            pr.list_ood(argv[2:], False)
        else:
            pr.list_ood(list(pr.available()), False)
    elif argv[1] == "oldnew":
        pr = Patcher(chromium_src_dir, "git", "Debug")
        pr.list_ood(list(pr.available()), False, True)
    elif argv[1] == "git":
        pr = Patcher(realpath(join(dirname(__file__), "..")), "git", "Debug")
        PREFIX = "?? component/patches/"
        SUFFIX = "patch"
        for output_line in pr.git(["status", "--porcelain"], Result.RawOutput).splitlines():
            if output_line.startswith(PREFIX) and output_line.endswith(SUFFIX):
                end = len(output_line) - len(SUFFIX) - 1
                pch = output_line[len(PREFIX): end]
                if pr.out_of_date(pch):
                    sys.exit(9)
                else:
                    pr.git(["add", output_line[3:]], Result.OrDie)
    else:
        Patcher(*argv[1:4]).create_patch_file()
