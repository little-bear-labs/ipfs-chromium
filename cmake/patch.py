#!/usr/bin/env python3

from os import listdir
from os.path import dirname, join, realpath, splitext
from subprocess import check_call, check_output
from sys import argv, platform, stderr
from time import ctime

import requests

def osname():
    match platform:
        case 'linux':
            return 'Linux'
        case 'darwn':
            return 'Mac'
        case _:
            return 'Windows'

class Patcher:
    def __init__(self,chromium_source_dir, git_bin, build_type):
        self.csrc = chromium_source_dir
        self.pdir = realpath(join(dirname(__file__),'..','component','patches'))
        self.gbin = git_bin
        self.btyp = build_type
    def create_patch_file(self):
        tag = self.tag_name()
        diff = self.git(['diff', '--patch', tag])
        name = tag
        if self.curr_hash() != self.hash_of(tag):
            print('NOT ON A TAG. Patching for hash instead')
            name = self.curr_hash()
        file_name = join(self.pdir,name+'.patch')
        print('Patch file:', file_name)
        with open(file_name, 'w') as patch_file:
            patch_file.write(diff+"\n")
    def apply(self):
        win = ''
        win_dist = 9876543210
        for ref in self.available():
            d = self.distance(ref)
            print(d,'commits away from',ref,file=stderr)
            if d < win_dist or (d == win_dist and len(ref) < len(win)):
                win_dist = d
                win = ref
        print('Best patch file is',win,file=stderr)
        self.git(['apply','--verbose',join(self.pdir,win+'.patch')],out=False)
    def git(self,args,out=True) -> str:
        print(args,file=stderr)
        if out:
            return check_output([self.gbin, '-C', self.csrc] + args, text=True).strip()
        else:
            check_call([self.gbin, '-C', self.csrc] + args, text=True)
            return ''
    def tag_name(self) -> str:
        return self.git(['describe', '--tags', '--abbrev=0'])
    def curr_hash(self) -> str:
        return self.hash_of('HEAD')
    def hash_of(self,ref) -> str:
        return self.git(['rev-parse', ref])
    def distance(self, ref) -> int:
        #base = self.git(['merge-base','HEAD',ref])
        a, b = self.distances('HEAD',ref)
        return a + b
    def recommend(self) -> str:
        channels = [ 'Dev', 'Beta', 'Stable', 'Extended' ]
        avail = list(self.available())
        if self.btyp == 'Release':
            channels.reverse()
        print('Avail:',avail)
        for channel in channels:
            print("Considering channel",channel,file=stderr)
            versions = self.release_versions(channel)
            for when, version in versions:
                print(f"Found a release version: '{version}'",file=stderr)
                if version in avail:
                    print('Suggesting version',version,'which was the',osname(),channel,'channel as of',ctime(when),file=stderr)
                    return version
        raise EnvironmentError(f"Can't find an appropriate tag for {osname()}, anymore!")
    def available(self):
        return map(lambda p:splitext(p)[0],listdir(self.pdir))
    def distances(self,frm,ref):
        a = int(self.git(['rev-list','--count',frm+'..'+ref]))
        b = int(self.git(['rev-list','--count',ref+'..'+frm]))
        return (a,b)
    def maybe_newer(self,x,y):
        a, b = self.distances(x,y)
        if a > b or (a == b and len(x) < len(y)):
            return y
        else:
            return x
    def release_versions(self,channel):
        parms = {'platform': osname(), 'channel': channel}
        resp = requests.get(url='https://chromiumdash.appspot.com/fetch_releases', params=parms)
        result = list(map(lambda x: (x['time'] / 1000, x['version']), resp.json()))
        result.sort(reverse=True)
        return result

if __name__ == '__main__':
    if argv[1] == 'apply':
        Patcher('/mnt/big/lbl/code/chromium/src','git', 'Debug').apply()
    elif argv[1] == 'rec':
        print(osname())
        print(Patcher('/mnt/big/lbl/code/chromium/src','git', argv[2]).recommend())
    else:
        Patcher(*argv[1:]).create_patch_file()
