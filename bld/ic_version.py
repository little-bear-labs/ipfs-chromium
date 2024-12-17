#!/usr/bin/env python3

from os.path import dirname, join
from sys import argv

import subprocess

here = join(dirname(__file__), '..')


def git(git_args):
    cmd_args = ['git', '-C', here] + git_args
    result = subprocess.run(args=cmd_args, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, check=True)
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


def dist(x):
    return int(git(['rev-list', '--count', f'{x}..HEAD']))


def recent():
    git(['fetch', '--all', '--tags'])
    tags = [(dist(x), x) for x in git(['tag']).splitlines() if tag_to_version(x) > 0]
    return min(tags)[1]


def on_tag():
    git(['fetch', '--all', '--tags'])
    return git(['describe', '--tags', '--exact-match', 'HEAD'])


def deduce():
    try:
        git(['fetch', '--all', '--tags'])
        tag = on_tag()
        if tag and tag_to_version(tag) > 0:
            result = tag
        else:
            git(['fetch', '--tags'])
            tags = git(['tag']).splitlines()
            versions = map(tag_to_version, tags)
            last = max(versions)
            next = last + 1
            major = str(next//1000)
            minor = str((next//100) % 10)
            revision = str((next//10) % 10)
            build = str(next % 10)
            result = '.'.join([major, minor, revision, build])
        with open(join(here, 'version.txt'), 'w') as txt:
            print(result, file=txt)
    except subprocess.CalledProcessError:
        with open(join(here, 'version.txt')) as txt:
            result = txt.read().strip()
    return result


if __name__ == "__main__":
    if len(argv) > 1 and argv[1] == 'recent':
        print(recent(), end='')
    else:
        print(deduce(), end='')
