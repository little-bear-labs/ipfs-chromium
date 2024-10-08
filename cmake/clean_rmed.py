#!/usr/bin/env python3

from glob import glob
from json import load
from os import remove
from os.path import splitext

known_obj = [c['output'] for c in load(open('compile_commands.json', encoding='utf-8'))]
for o in glob('*/CMakeFiles/**/*.o', recursive=True):
    if not o in known_obj:
        base = splitext(o)[0]
        for garbage in glob(base+'*'):
            print(garbage)
            remove(garbage)
