#!/usr/bin/env python3

from os.path import dirname, join
import subprocess

here = join(dirname(__file__), '..')


def git(git_args):
    cmd_args = ['git', '-C', here] + git_args
    result = subprocess.run(args=cmd_args, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)
    return result.stdout.strip()


def tag_to_version(tag):
    segments = tag.split('.')
    if len(segments) != 4:
        return 0
    try:
        result = 0
        for i in map(int, segments):
            if i < 0 or i > 9:
                return 0
            result *= 10
            result += i
        return result
    except Exception:
        return 0


def deduce():
    on_tag = git(['describe', '--tags', '--exact-match', 'HEAD'])
    if on_tag and tag_to_version(on_tag) > 0:
        return on_tag
    git(['fetch', '--tags'])
    tags = git(['tag']).splitlines()
    versions = map(tag_to_version, tags)
    last = max(versions)
    next = last + 1
    major = str(next//1000)
    minor = str((next//100) % 10)
    revision = str((next//10) % 10)
    build = str(next % 10)
    return '.'.join([major, minor, revision, build])


if __name__ == "__main__":
    print(deduce(), end='')
