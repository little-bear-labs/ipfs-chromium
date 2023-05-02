#!/usr/bin/env python3

from os import listdir
from os.path import join, realpath, splitext
from subprocess import check_call, check_output
from sys import argv, stderr

class Patcher:
    def __init__(self,chromium_source_dir, patch_dir, git_bin):
        self.csrc = chromium_source_dir
        self.pdir = patch_dir
        self.gbin = git_bin
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
        print('Best patch file is',win)
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
        w = '111.0.5563.152'
        for a in self.available():
            w = self.maybe_newer(a,w)
        return w
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

if __name__ == '__main__':
    if argv[1] == 'apply':
        Patcher('/mnt/big/lbl/code/chromium/src','/mnt/big/lbl/code/ipfs-chromium/component/patches','git').apply()
    elif argv[1] == 'rec':
        print(Patcher('/mnt/big/lbl/code/chromium/src','/mnt/big/lbl/code/ipfs-chromium/component/patches','git').recommend())
    else:
        Patcher(*argv[1:]).create_patch_file()
