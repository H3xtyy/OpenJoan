"""Turn analyze.py output into re/functions.csv (the project's function database).

Usage (from a folder containing joan.exe):
    objdump -d --no-show-raw-insn -M intel joan.exe > joan.asm
    python3 analyze.py joan.exe joan.asm      # -> functions.json, thunks.json
    python3 make_csv.py functions.json thunks.json functions.csv [previous functions.csv]

The 'status' column of a previous CSV (reimplemented / original) is carried over.
Region boundaries for statically linked libraries are approximate.
"""
import csv
import json
import os
import sys


def region(a):
    if a < 0x7693C4:
        return 'game'
    if a < 0x790000:
        return 'd3dx8'
    if a < 0x7A8000:
        return 'libpng/zlib'
    if a < 0x7D5000:
        return 'msvcrt'
    return 'data'  # ~2 MB high-entropy blob inside .text, not code


def main(funcs_json, thunks_json, out_csv, previous=None):
    funcs = json.load(open(funcs_json))
    thunks = json.load(open(thunks_json))
    by_target = {}
    for thunk, target in thunks.items():
        by_target.setdefault(target, []).append(thunk)
    status = {}
    if previous and os.path.exists(previous):
        for row in csv.DictReader(open(previous)):
            status[row['address']] = row['status']
    with open(out_csv, 'w', newline='') as o:
        w = csv.writer(o)
        w.writerow(['address', 'end', 'size', 'region', 'module', 'module_evidence', 'thunks', 'callers', 'status'])
        for f in funcs:
            a = int(f['addr'], 16)
            if region(a) == 'data':
                continue
            addr = '0x' + f['addr']
            w.writerow([addr, '0x' + f['end'], f['size'], region(a), f['file'] or '', f['file_src'] or '',
                        ' '.join('0x' + t for t in by_target.get(f['addr'], [])), f['ncallers'],
                        status.get(addr, 'original')])


if __name__ == '__main__':
    main(*sys.argv[1:])
