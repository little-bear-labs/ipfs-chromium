#!/usr/bin/env python3

from cache_vars import build_dir, vars

from glob import glob
from os import environ, makedirs, symlink
from os.path import basename, dirname, exists, getmtime, isdir, isfile, join, pathsep, relpath
from subprocess import Popen, PIPE, STDOUT, run
from sys import executable, stderr
from time import sleep

if 'inside_inc_link' in environ:
    exit(0)
environ['inside_inc_link'] = "Don't recurse."

inc_link = join(build_dir,'component','inc_link')
chromium_src = vars['CHROMIUM_SOURCE_TREE'].rstrip('/')
profile = vars['CHROMIUM_PROFILE']
source_bases = [
    chromium_src,
    join(chromium_src,'out',profile,'gen'),
    join(chromium_src,'v8','include'),
    join(chromium_src,'third_party', 'abseil-cpp')
    ]
for inc_dir in glob(f'{chromium_src}/third_party/**/include', recursive=True):
    if isdir(inc_dir):
        source_bases.append(inc_dir)

if not exists(join(inc_link,'google','protobuf')):
    makedirs(join(inc_link,'google'))
    symlink(join(chromium_src,'third_party','protobuf','src','google','protobuf'), join(inc_link,'google','protobuf'))
if not exists(join(inc_link,'absl')):
    symlink(join(chromium_src,'third_party','abseil-cpp','absl'),join(inc_link,'absl'))

def quoted(inc):
    inc = inc[inc.index('"')+1:]
    return inc[0:inc.index('"')]
def angled(inc):
    inc = inc[inc.index('<')+1:]
    return inc[0:inc.index('>')]

def search():
    link_count = 0
    unfound_count = 0
    task = Popen(['cmake','--build',build_dir,'--target','out_of_tree'],stdout=PIPE,stderr=STDOUT)
    while task.poll() is None:
        line = task.stdout.readline().decode('utf-8').strip()
        pound = line.find('#include')
        if pound == -1:
            continue
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
            continue
        if not exists( dirname(target) ):
            makedirs(dirname(target))
        unfound = True
        for base in source_bases:
            source = join(base,inc)
            if exists(source):
                symlink(source, target)
                print("Symlink",inc,source,'=>',target)
                unfound = False
                link_count += 1
                break
        if unfound:
            print('Failed to resolve:',inc,pound,quote,angle,line,file=stderr)
            unfound_count += 1
    print('Linked',link_count,'new headers. Trouble with',unfound_count,'others.',file=stderr)
    return link_count > unfound_count

slowness = 0
while search():
    slowness += 0.01
    print('Continue to look for more.',slowness,file=stderr)
    sleep(slowness)
