#!/usr/bin/env python3
"""Offline save comparison (re/a6/SAVES_PLAN.md 3.3 / 5.3): compares two save files, or two folders file by file
(e.g. SAVE.replayed-<a> vs SAVE.replayed-<b> of two builds, or joan.exe's saves vs our re-saves).

.SAV files are parsed: "RSav" chunk, the header chunks (records.json "sav".header), the marker chunk 0x2facebee and
20 steps each followed by the marker. A step with a grammar in records.json ("sav".steps: chunk:<record> | chunk:<n>
| long | short) is parsed into records and compared field by field with the generated tables
(re/save/gen/schema_*.json); a step without one is compared as bytes (located by the next marker chunk). Other files
(profiles SAVE\\<name>.dat, CONFIG.DAT, caches) are compared as bytes until S5 / S6 add their grammars.

    savecmp.py A B [--semantic] [--max N]      A, B: files or folders. Exit 0 = no difference.
    savecmp.py --selftest

--exact (default): every byte. --semantic: pointer fields compared as null / non-null, char arrays up to their NUL,
the header's FILETIMEs ("sav".header_ignore) ignored.
"""
import argparse
import glob
import json
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
MARKER = struct.pack('<HI', 4, 0x2facebee)


def load_spec():
    spec = json.load(open(os.path.join(HERE, 'records.json')))
    recs = {}
    for p in sorted(glob.glob(os.path.join(HERE, 'gen', 'schema_*.json'))):
        recs.update(json.load(open(p))['records'])
    return spec['sav'], recs


class Bad(Exception):
    pass


class Reader:
    def __init__(self, data):
        self.d, self.p = data, 0

    def chunk(self):
        if self.p + 2 > len(self.d):
            raise Bad(f'chunk prefix beyond the end at {self.p:#x}')
        n = struct.unpack_from('<H', self.d, self.p)[0]
        if n == 0:
            raise Bad(f'chunk with size prefix 0 (> 0xffff) at {self.p:#x}: not supported')
        start = self.p + 2
        if start + n > len(self.d):
            raise Bad(f'chunk {n:#x} at {self.p:#x} beyond the end')
        self.p = start + n
        return start, self.d[start:start + n]

    def raw(self, n):
        if self.p + n > len(self.d):
            raise Bad(f'{n} raw bytes beyond the end at {self.p:#x}')
        b = self.d[self.p:self.p + n]
        self.p += n
        return b

    def marker(self):
        if self.d[self.p:self.p + 6] != MARKER:
            raise Bad(f'marker chunk expected at {self.p:#x}')
        self.p += 6


def parse_sav(data, sav, recs):
    """[(label, kind, file offset, bytes, record name or None)] in file order."""
    r = Reader(data)
    items = []
    off, b = r.chunk()
    if b != b'RSav':
        raise Bad('no "RSav" chunk')
    items.append(('magic', 'chunk', off, b, None))
    for i, (field, n) in enumerate(sav['header']):
        off, b = r.chunk()
        items.append((f'header[{i}] GameFile+{field}', 'header', off, b, i))
    r.marker()
    for step in range(1, 21):
        grammar = sav.get('steps', {}).get(str(step))
        if grammar:
            for k, g in enumerate(grammar):
                what, _, arg = g.partition(':')
                if what == 'chunk':
                    off, b = r.chunk()
                    name = arg if arg in recs else None
                    if name and len(b) != recs[name]['size']:
                        raise Bad(f'step {step}: {arg} chunk of {len(b):#x} bytes (record {recs[name]["size"]:#x})')
                    items.append((f'step {step} #{k} {arg}', 'record' if name else 'chunk', off, b, name))
                elif what in ('long', 'short'):
                    n = 4 if what == 'long' else 2
                    off = r.p
                    items.append((f'step {step} #{k} {what}', 'raw', off, r.raw(n), None))
                else:
                    raise Bad(f'step {step}: unknown grammar item {g}')
        else:
            end = data.find(MARKER, r.p)
            if end < 0:
                raise Bad(f'step {step}: no marker after {r.p:#x}')
            items.append((f'step {step}', 'bytes', r.p, data[r.p:end], None))
            r.p = end
        r.marker()
    if r.p != len(data):
        items.append(('trailing bytes', 'bytes', r.p, data[r.p:], None))
    return items


def field_diff(rec, a, b, semantic):
    """First differing field of two chunks of record rec: (offset, path, a bytes, b bytes) or None."""
    for off, size, elem, path, typ, kind in rec['fields']:
        x, y = a[off:off + size], b[off:off + size]
        if x == y:
            continue
        if semantic and typ == 'Pointer':
            if all((x[i:i + 4] == b'\0' * 4) == (y[i:i + 4] == b'\0' * 4) for i in range(0, size, 4)):
                continue
        if semantic and typ == 'Char' and x.split(b'\0', 1)[0] == y.split(b'\0', 1)[0]:
            continue
        i = next(i for i in range(size) if x[i] != y[i])
        e = f'{path}[{i // elem}]' if elem > 1 and size > elem else path
        return off + i, f'{e} ({typ}{" " + kind if kind else ""})', x.hex(), y.hex()
    return None


def cmp_sav(da, db, sav, recs, semantic, out):
    ia, ib = parse_sav(da, sav, recs), parse_sav(db, sav, recs)
    diffs = 0
    if len(ia) != len(ib):
        out(f'  structure differs: {len(ia)} vs {len(ib)} parts')
        return 1
    for (la, ka, oa, ba, na), (lb, kb, ob, bb, nb) in zip(ia, ib):
        if la != lb:
            out(f'  structure differs: {la} vs {lb}')
            return diffs + 1
        if ba == bb:
            continue
        if ka == 'header' and semantic and na in sav.get('header_ignore', []):
            continue
        if ka == 'record' and len(ba) == len(bb):
            d = field_diff(recs[na], ba, bb, semantic)
            if d is None:
                continue
            o, path, x, y = d
            out(f'  {la} @ {oa:#x} vs {ob:#x}: {na}+{o:#x} {path}: {x} vs {y}')
        else:
            n = min(len(ba), len(bb))
            i = next((i for i in range(n) if ba[i] != bb[i]), n)
            out(f'  {la}: {len(ba)} vs {len(bb)} bytes, first difference at +{i:#x} (file {oa + i:#x} vs {ob + i:#x})')
        diffs += 1
    return diffs


def cmp_bytes(da, db, out):
    if da == db:
        return 0
    n = min(len(da), len(db))
    i = next((i for i in range(n) if da[i] != db[i]), n)
    out(f'  {len(da)} vs {len(db)} bytes, first difference at {i:#x}')
    return 1


def compare_files(a, b, sav, recs, semantic, out):
    da, db = open(a, 'rb').read(), open(b, 'rb').read()
    if da[:6] == struct.pack('<H', 4) + b'RSav' and db[:6] == da[:6]:
        try:
            return cmp_sav(da, db, sav, recs, semantic, out)
        except Bad as e:
            out(f'  not parsed ({e}): compared as bytes')
    return cmp_bytes(da, db, out)


def run(a, b, semantic, maxn):
    sav, recs = load_spec()
    pairs = []
    if os.path.isdir(a) and os.path.isdir(b):
        names = sorted({os.path.relpath(os.path.join(dp, f), a) for dp, _, fs in os.walk(a) for f in fs} |
                       {os.path.relpath(os.path.join(dp, f), b) for dp, _, fs in os.walk(b) for f in fs})
        pairs = [(n, os.path.join(a, n), os.path.join(b, n)) for n in names]
    else:
        pairs = [(os.path.basename(a), a, b)]
    total = 0
    for name, pa, pb in pairs:
        lines = []
        if not os.path.exists(pa) or not os.path.exists(pb):
            print(f'{name}: only in {"B" if not os.path.exists(pa) else "A"}')
            total += 1
            continue
        d = compare_files(pa, pb, sav, recs, semantic, lines.append)
        print(f'{name}: {"identical" if d == 0 else f"{d} difference(s)"}')
        for ln in lines[:maxn]:
            print(ln)
        if len(lines) > maxn:
            print(f'  ... {len(lines) - maxn} more')
        total += d
    print(f'{len(pairs)} file(s), {total} difference(s) ({"semantic" if semantic else "exact"})')
    return 0 if total == 0 else 1


def selftest():
    """A synthetic .SAV (header, Calendar step, byte steps) compared with itself and with one Calendar field changed."""
    import random
    import tempfile
    sav, recs = load_spec()
    rnd = random.Random(1)

    def ch(b):
        return struct.pack('<H', len(b)) + b

    def build(cal):
        d = ch(b'RSav') + b''.join(ch(bytes(rnd.randrange(256) for _ in range(n))) for _, n in sav['header'])
        d += MARKER + ch(cal) + MARKER
        for step in range(2, 21):
            grammar = sav.get('steps', {}).get(str(step))
            if not grammar:  # byte steps
                d += ch(bytes(rnd.randrange(256) for _ in range(rnd.randrange(1, 40)))) + struct.pack('<i', 7) + MARKER
                continue
            for g in grammar:  # steps with a grammar: random chunks / scalars of the right sizes
                what, _, arg = g.partition(':')
                if what == 'chunk':
                    n = recs[arg]['size'] if arg in recs else int(arg, 0)
                    d += ch(bytes(rnd.randrange(256) for _ in range(n)))
                else:
                    d += bytes(rnd.randrange(256) for _ in range(4 if what == 'long' else 2))
            d += MARKER
        return d
    cal = bytes(rnd.randrange(256) for _ in range(0x50))
    rnd.seed(2)
    a = build(cal)
    month = recs['Calendar']['fields'][5]
    cal2 = bytearray(cal)
    cal2[month[0] + 1] ^= 0xff
    rnd.seed(2)
    b = build(bytes(cal2))
    with tempfile.TemporaryDirectory() as t:
        pa, pb = os.path.join(t, 'a.SAV'), os.path.join(t, 'b.SAV')
        open(pa, 'wb').write(a)
        open(pb, 'wb').write(b)
        same, diff = [], []
        n0 = compare_files(pa, pa, sav, recs, False, same.append)
        n1 = compare_files(pa, pb, sav, recs, False, diff.append)
    ok = n0 == 0 and n1 == 1 and 'Calendar+0x15 month (I32)' in diff[0]
    print('\n'.join(diff))
    print('selftest', 'ok' if ok else 'FAILED')
    return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('a', nargs='?')
    ap.add_argument('b', nargs='?')
    ap.add_argument('--semantic', action='store_true')
    ap.add_argument('--exact', action='store_true', help='the default')
    ap.add_argument('--max', type=int, default=20, help='differences listed per file')
    ap.add_argument('--selftest', action='store_true')
    a = ap.parse_args()
    if a.selftest:
        return selftest()
    if not a.a or not a.b:
        ap.error('two files or folders')
    return run(a.a, a.b, a.semantic, a.max)


if __name__ == '__main__':
    sys.exit(main())
