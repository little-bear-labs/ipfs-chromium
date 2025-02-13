#!/usr/bin/env python3

import json

from glob import glob
from os import remove
from os.path import splitext

with open('compile_commands.json', encoding='utf-8') as compile_commands_json:
    keeps = {}
    for command in json.load(compile_commands_json):
        keeps[command['output']] = command['file']
    for obj in glob('**/*.o', recursive=True):
        if not 'CMakeFiles' in obj:
            continue
        if not obj in keeps:
            print('rm', obj)
            remove(obj)
    for cov in glob('**/*.gc??', recursive=True):
        obj = splitext(cov)[0] + '.o'
        if not obj in keeps:
            print('rm', cov)
            remove(cov)
