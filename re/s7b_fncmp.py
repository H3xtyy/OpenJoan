#!/usr/bin/env python3
"""Stage S7b-main offline proof, function level (companion of re/s7b_objcmp.py): for object pairs that are not
byte-identical up to the rebased fields, compare the code function by function after normalising the image
addresses:
  before: every number in [0x009cf000, 0x00a70000] in an instruction             -> IMG+<offset>
  after:  the number an IMAGE_REL_I386_DIR32 _joan_image relocation patches       -> IMG+<offset>
Branch targets and instruction addresses are dropped (they shift when an instruction gets longer).

Per object: functions whose normalised instruction lists are equal, and the DIFFERENT ones; for each function also
the multiset of image offsets it references before / after (it must be the same: every image address the old code
used, the new code uses through joan_image, and no other). After objects must contain no absolute image address
left (a number in the range without relocation).

Usage: s7b_fncmp.py <before dir> <after dir> <object names...|@list> [--show N]
       s7b_fncmp.py --scan <after dir>
  --scan: every object of the directory: memory accesses whose displacement is an absolute image address without a
  _joan_image relocation (an access the move would miss). Not counted: LEA (address arithmetic on img::Addr values,
  e.g. table + 8 * i before location()), and displacement -0x009cf000 (the `va - kVa` step of location() / addr()
  for an address computed at run time, whose joan_image part is added in another instruction).
"""
import collections
import os
import re
import subprocess
import sys

KVA, KEND = 0x009cf000, 0x00a70000
HEX = re.compile(r'(?<![\w.])(-?0x[0-9a-fA-F]+|\d+)(?![\w])')
FN_RE = re.compile(r'^[0-9a-f]+ <(.*)>:$')
INSN_RE = re.compile(r'^\s*([0-9a-f]+):\s*(.*)$')
REL_RE = re.compile(r'^\s*([0-9a-f]+):\s+(IMAGE_REL_I386_\w+)\s+(\S+)')
BRANCH = re.compile(r'^(j\w+|call|loop\w*)\s')


def disasm(path):
    """{function: [(text, [image offsets], absolute_left)]} with relocations folded in"""
    out = subprocess.run(['llvm-objdump', '-dr', '-M', 'intel', path], capture_output=True, text=True).stdout
    funcs = collections.OrderedDict()
    cur = None
    last = None
    for ln in out.split('\n'):
        m = FN_RE.match(ln)
        if m:
            cur = funcs.setdefault(m.group(1), [])
            last = None
            continue
        if cur is None:
            continue
        r = REL_RE.match(ln)
        if r and last is not None:
            last['rel'].append((int(r.group(1), 16), r.group(2), r.group(3)))
            continue
        m = INSN_RE.match(ln)
        if m and m.group(2).strip():
            rest = m.group(2)
            raw, _, text = rest.partition('\t')
            try:
                bts = bytes(int(b, 16) for b in raw.split())
            except ValueError:
                bts, text = b'', rest
            if not text.strip():
                # a continuation line of raw bytes (long instruction): append to the previous one
                if last is not None:
                    last['bytes'] += bts
                continue
            last = {'at': int(m.group(1), 16), 'text': text.strip(), 'rel': [], 'bytes': bts}
            cur.append(last)
    return funcs


def norm(insn, after):
    text = insn['text'].split('#')[0].strip()
    text = re.sub(r'\s+', ' ', text)
    offs = []
    image_rels = [r for r in insn['rel'] if r[2] == '_joan_image']
    addends = []
    for r in image_rels:
        k = r[0] - insn['at']
        b = insn['bytes'][k:k + 4]
        addends.append(int.from_bytes(b, 'little') if len(b) == 4 else None)
    other_rels = [r for r in insn['rel'] if r[2] != '_joan_image']
    if BRANCH.match(text):
        # branch target numbers are code offsets: drop them (relocated calls keep their symbol); an indirect call /
        # jump through an image slot keeps its image offset
        def bsub(m):
            tok = m.group(1)
            v = int(tok, 16) if tok.lower().lstrip('-').startswith('0x') else int(tok)
            v &= 0xffffffff
            if after and v in addends:
                addends.remove(v)
                offs.append(v)
                return 'IMG+0x%x' % v
            if not after and KVA <= v <= KEND and '[' in text:
                offs.append(v - KVA)
                return 'IMG+0x%x' % (v - KVA)
            return 'N'
        text = HEX.sub(bsub, text)
        return text + ''.join(' @' + r[2] for r in other_rels), offs, 0
    left = 0

    def sub(m):
        nonlocal left
        tok = m.group(1)
        v = int(tok, 16) if tok.lower().lstrip('-').startswith('0x') else int(tok)
        if v < 0:
            v &= 0xffffffff
        if after:
            if v in addends:
                offs.append(v)
                addends.remove(v)
                return 'IMG+0x%x' % v
            if KVA <= v <= KEND:
                left += 1
            return tok
        if KVA <= v <= KEND:
            offs.append(v - KVA)
            return 'IMG+0x%x' % (v - KVA)
        return tok
    text = HEX.sub(sub, text)
    return text + ''.join(' @' + r[2] for r in other_rels), offs, left


def compare(before, after, show):
    A, B = disasm(before), disasm(after)
    res = collections.Counter()
    diffs = []
    left_total = 0
    for name in list(A) + [n for n in B if n not in A]:
        fa, fb = A.get(name), B.get(name)
        if fa is None or fb is None:
            res['only one side'] += 1
            diffs.append((name, 'only %s' % ('after' if fa is None else 'before'), [], []))
            continue
        na = [norm(i, False) for i in fa]
        nb = [norm(i, True) for i in fb]
        left_total += sum(x[2] for x in nb)
        ta = [x[0] for x in na]
        tb = [x[0] for x in nb]
        oa = collections.Counter(o for x in na for o in x[1])
        ob = collections.Counter(o for x in nb for o in x[1])
        if ta == tb:
            res['same'] += 1
            continue
        res['different'] += 1
        diffs.append((name, 'offsets %s' % ('equal' if oa == ob else 'DIFFER'), ta, tb))
    return res, diffs, left_total


def scan(d):
    import glob
    n = rel = 0
    rows = []
    for p in sorted(glob.glob(d + '/*.o')):
        for fn, ins in disasm(p).items():
            for i in ins:
                t = i['text'].split('#')[0]
                if t.startswith('lea') or t.startswith('nop'):
                    continue
                hit = False
                for m in re.findall(r'\[([^\]]*)\]', t):
                    for sign, tok in re.findall(r'([+-]?)\s*(0x[0-9a-f]+)', m):
                        v = int(tok, 16)
                        if KVA <= v <= KEND and not (sign == '-' and v == KVA):
                            hit = True
                if hit:
                    n += 1
                    if any(r[2] == '_joan_image' for r in i['rel']):
                        rel += 1
                    else:
                        rows.append((os.path.basename(p), fn, t))
    print('memory accesses with an absolute image displacement: %d, all through a _joan_image relocation: %s' % (
        n, 'yes' if not rows else 'NO, %d without' % len(rows)))
    for r in rows:
        print('   %s %s: %s' % r)
    return 1 if rows else 0


def main():
    if sys.argv[1:2] == ['--scan']:
        sys.exit(scan(sys.argv[2]))
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    show = 0
    if '--show' in sys.argv:
        show = int(sys.argv[sys.argv.index('--show') + 1])
        args = [a for a in args if a != str(show)]
    before, after = args[0], args[1]
    names = []
    for a in args[2:]:
        names += [l.strip() for l in open(a[1:])] if a.startswith('@') else [a]
    total = collections.Counter()
    for n in names:
        res, diffs, left = compare(os.path.join(before, n), os.path.join(after, n), show)
        total.update(res)
        total['objects'] += 1
        if left:
            total['absolute image numbers left'] += left
        bad_offsets = [d for d in diffs if 'DIFFER' in d[1] or d[1].startswith('only')]
        print('%-45s same %4d different %3d%s%s' % (n, res['same'], res['different'],
                                                    ('  OFFSETS DIFFER in %d' % len(bad_offsets)) if bad_offsets else '',
                                                    ('  ABSOLUTE LEFT %d' % left) if left else ''))
        for name, what, ta, tb in diffs[:show]:
            print('   %s: %s' % (name[:90], what))
            import difflib
            for d in list(difflib.unified_diff(ta, tb, lineterm='', n=1))[2:14]:
                print('      ' + d)
    print('total: ' + ', '.join('%s %d' % kv for kv in sorted(total.items())))


if __name__ == '__main__':
    main()
