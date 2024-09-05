#!/usr/bin/env python3
# This Python file uses the following encoding: utf-8

from json import load
from re import sub
from sys import argv

THRESHOLD = 7.7

def msg_out(msg):
    f = msg["path"]
    l = msg["line"]
    c = msg["column"]
    i = msg["messageId"]
    m = msg["message"]
    t = msg["type"]
    print(f"{f}:{l}:{c}: {t}: {i} {m}")

if __name__ == "__main__":
    with open(argv[1]) as f:
        o = load(f)
    msgs = o['messages']
    for warn in list(filter(lambda x:x['type']=='warning', msgs))[0:9]:
        msg_out(warn)
    for erro in filter(lambda x:x['type']=='error', msgs):
        msg_out(erro)
    score = o['statistics']['score']
    if score < THRESHOLD:
        print('error: score of', score, 'is less than the minimum threshold', THRESHOLD)
        exit(1)
    elif score > THRESHOLD + 0.3:
        update = (score + THRESHOLD) / 2
        print("Increasing threshold to", update)
        s = open(__file__).read()
        s = sub(r'\n\s*THRESHOLD\s*=\s*\d*\.\d*\s*\n', f"\n\nTHRESHOLD = {update:.2}\n\n", s)
        open(__file__, 'w').write(s)
