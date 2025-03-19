#!/usr/bin/env python3

from re import compile, Pattern, Match
from subprocess import check_call, check_output

from typeguard import typechecked


SERVER_ENTRY = compile(r'([\w-]+): ([\w:/_\.-]+) \[Verify SSL: (True|False), Enabled: (True|False)\]')
# SERVER_ENTRY = compile(r'(\w+): ([\w:/_\.-]+)')


@typechecked
class Server:
    def __init__(self, match: Match) -> None:
        self.slug = match[1]
        self.url = match[2]
        self.ssl = match[3] == 'True'
        self.enabled = match[4] == 'True'

    def __str__(self) -> str:
        return str(self.__dict__)

    def __repr__(self) -> str:
        return str(self.__dict__)


@typechecked
def list_remotes() -> list[Server]:
    args = ['conan', 'remote', 'list']
    out = check_output(args, text=True)
    lines = out.splitlines()
    parsed = map(SERVER_ENTRY.match, lines)
    no_goofy_warnings = filter(lambda x: x is not None, parsed)
    structured = map(Server, no_goofy_warnings)
    return list(structured)


@typechecked
def add_remote(slug: str, url: str) -> None:
    args = ['conan', 'remote', 'add', slug, url]
    print(args)
    check_call(args)


@typechecked
def add_missing_remotes() -> None:
    existing = list_remotes()
    for name, url in [('conan-2', 'https://center2.conan.io'),
                      ('ipfs_client', 'https://gitlab.com/api/v4/projects/65828117/packages/conan')]:
        if any(map(lambda x: url == x.url, existing)):
            print(name, '=', url)
        else:
            add_remote(name, url)


if __name__ == '__main__':
    add_missing_remotes()
