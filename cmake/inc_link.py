#!/usr/bin/env python3

from verbose import verbose
from cache_vars import build_dir, CACHE_VARS

from glob import glob
import os
from os import listdir, readlink, set_blocking, symlink
from os.path import dirname, exists, isdir, isfile, islink, join, splitext
from subprocess import DEVNULL, Popen, PIPE
from sys import stderr

import json

inc_link = join(build_dir, 'component','inc_link')
chromium_src = CACHE_VARS['CHROMIUM_SOURCE_TREE'].rstrip('/')
profile = CACHE_VARS['CHROMIUM_PROFILE']
gen_dir = join(chromium_src,'out',profile,'gen')
source_bases = [
    chromium_src,
    gen_dir,
    join(chromium_src, 'v8',  'include'),
    join(chromium_src, 'third_party', 'abseil-cpp'),
    join(chromium_src, 'net', 'third_party', 'quiche', 'overrides'),
    join(chromium_src, 'net', 'third_party', 'quiche', 'src'),
    ]
for g in [f'{chromium_src}/third_party/**/include', f'{chromium_src}/base/**/src', f'{gen_dir}/base/**/src' ]:
    for inc_dir in glob(g, recursive=True):
        if isdir(inc_dir):
            source_bases.append(inc_dir)
verbose('source_bases', source_bases)

def makedirs(p):
    if not isdir(p):
        os.makedirs(p)

# if not exists(join(inc_link,'google','protobuf')):
#     makedirs(join(inc_link,'google'))
#     symlink(join(chromium_src,'third_party','protobuf','src','google','protobuf'), join(inc_link,'google','protobuf'))
if not exists(join(inc_link,'absl')):
    makedirs(inc_link)
    symlink(join(chromium_src,'third_party','abseil-cpp','absl'),join(inc_link,'absl'))

def quoted(inc):
    inc = inc[inc.index('"')+1:]
    return inc[0:inc.index('"')]
def angled(inc):
    inc = inc[inc.index('<')+1:]
    return inc[0:inc.index('>')]

link_count = 0
unfound_count = 0

class Command:
    def __init__(self,c,d):
        self.retry = False
        self.dead = False
        self.command = c
        self.command[self.command.index('-c')] = '-E'
        i = self.command.index('-o')
        del self.command[i:i+2]
        verbose('Command is now',self.command)
        self.directory = d
        self.start()

    def __str__(self):
        return ' '.join(self.command)

    def start(self):
        self.retry = False
        self.dead = False
        self.task = Popen( self.command, cwd=self.directory, stdout=DEVNULL, stderr=PIPE, text=True )
        # self.task = Popen( self.command, cwd=self.directory, stdout=DEVNULL, stderr=PIPE, bufsize=1, text=True )
        self.left_over = ''
        set_blocking(self.task.stderr.fileno(), False)

    def finished(self) -> bool:
        if self.task.poll() is None:
            return False
        for line in self.task.stderr.readlines():
            self.eval_line(line)
        if self.retry:
            verbose('Retrying',self.command)
            self.start()
            return False
        return True

    def eval_line(self, line) -> bool:
        global link_count
        global unfound_count
        verbose('Eval:', line)
        pound = line.find('#include')
        if pound == -1:
            return False
        verbose('Output line',line,self.command)
        line = line[pound:]
        quote = line.find('"')
        angle = line.find('<')
        if angle == -1:
            inc = quoted(line)
        elif quote == -1:
            inc = angled(line)
        elif quote < angle:
            inc = quoted(line)
        else:
            inc = angled(line)
        target = join(inc_link,inc)
        if exists(target):
            print('Message',line,'mentioned',inc,'but it already exists as',target,file=stderr)
            return False
        if not exists( dirname(target) ):
            makedirs(dirname(target))
        verbose('target=', target, 'for line', line)
        if 'boringssl' in inc and not inc.endswith('input.h'):
            with open(target, 'w') as tgt:
                print("//This file included and probably not provided by the conan (newer) version of openssl", file=tgt)
        elif 'openssl' in target:
            # with open(target, 'w') as tgt:
            #     print("//This file included and probably not provided by the conan (newer) version of openssl", file=tgt)
            #     print('Stubbed',tgt)
            return False
        for base in source_bases:
            source = join(base, inc)
            if exists(source):
                verbose(source, 'exists. Attempt symlink to', target)
                try:
                    symlink(source, target)
                except FileExistsError:
                    return False
                print("Symlink", inc, source, '=>', target)
                self.retry = True
                link_count += 1
                return True
            else:
                verbose('Did not find', source, 'for', line)
        print('Failed to resolve:', inc, pound, quote, angle, line, file=stderr)
        unfound_count += 1
        return False


preempt = False


def search() -> bool:
    global link_count
    global unfound_count
    global preempt
    link_count = 0
    unfound_count = 0
    comp_comm_json = join(build_dir, 'compile_commands.json')
    compile_commands = json.load(open(comp_comm_json, encoding='utf-8'))
    commands = []
    for command_obj in compile_commands:
        artifact = command_obj['output']
        if not 'out_of_tree' in artifact:
            continue
        # evaluate_command(command_obj['command'].split(' '), command_obj['directory'])
        commands.append(Command(command_obj['command'].split(' '), command_obj['directory']))
    prev_len = 0
    existing = None
    existing_dir_map = None
    while len(commands):
        if len(commands) != prev_len:
            prev_len = len(commands)
            verbose('Remaining',list(map(str,commands)))
        try:
            doneso = next(filter(lambda x: x.finished(), commands))
            verbose('Finished, removing:',doneso)
            commands.remove(doneso)
        except StopIteration:
            if preempt:
                pass
            elif existing is None:
                existing = glob(inc_link+'/**/*.h',recursive=True)
                verbose('existing=',existing)
            elif existing_dir_map is None:
                existing_dir_map = set()
                for f in existing:
                    if islink(f):
                        t = readlink(f)
                        existing_dir_map.add( (dirname(f), dirname(t)) )
                verbose('existing_dir_map=',existing_dir_map)
            elif len(existing_dir_map):
                t, f = existing_dir_map.pop()
                if not isdir(f):
                    continue
                for entry in listdir(f):
                    source = join(f,entry)
                    target = join(t,entry)
                    if exists(target):
                        continue
                    verbose('Consider source',source)
                    if isfile(source) and not isfile(target) and splitext(entry)[-1] == '.h':
                        try:
                            symlink(source, target)
                        except FileExistsError:
                            continue
                        print("Premptively symlink",entry,source,'=>',target)
                        preempt = True
                        break
            else:
                existing = None
                existing_dir_map = None
    print('Linked',link_count,'new headers. Trouble with',unfound_count,'others.',file=stderr)
    return link_count > unfound_count


def flesh_out() -> bool:
    existing = glob(inc_link+'/**/*.h',recursive=True)
    existing_dir_map = set()
    for f in existing:
        if islink(f):
            t = readlink(f)
            existing_dir_map.add( (dirname(f), dirname(t)) )
    for t, f in existing_dir_map:
        if not isdir(f):
            continue
        for entry in listdir(f):
            source = join(f, entry)
            target = join(t, entry)
            if exists(target):
                continue
            if isfile(source) and splitext(entry)[-1] == '.h':
                symlink(source, target)
                print("Premptively symlink",entry,source,'=>',target)
                return True
    return False


if search():
    search()
elif not preempt:
    flesh_out()
