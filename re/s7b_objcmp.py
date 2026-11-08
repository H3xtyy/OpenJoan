#!/usr/bin/env python3
"""Stage S7b-main offline proof (docs/RUNTIME.md): the only code / data differences between the objects of two
builds are absolute data-image addresses turned into joan_image relocations with the same offset.

Usage: s7b_objcmp.py <before dir> <after dir> [--verbose]
  The directories hold COFF objects (i386, clang --target=i686-pc-windows-msvc or mingw) of the same sources, named
  alike (re/... cc1 scripts). Build the "after" objects with -DJOAN_IMG_STRAY_CHECK=0 (image.hpp's raw overloads then
  compile to the plain identity, as before).

Per object pair:
  identical   bytes equal except the COFF timestamp
  rebased     every section has the same size, characteristics and bytes, except 4-byte fields where BEFORE holds an
              absolute value V in [0x009cf000, 0x00a70000] without a relocation and AFTER holds V - 0x009cf000 with
              an IMAGE_REL_I386_DIR32 relocation against _joan_image; every other relocation is the same (offset, type,
              symbol name); the symbol tables differ only by the undefined _joan_image (and declarations nothing
              references). The rebased fields are counted per section.
  DIFFERENT   anything else: listed with the first differences (section, offset, function symbol).
Exit status 1 if any pair is DIFFERENT or missing.
"""
import collections
import glob
import os
import struct
import sys

KVA, KEND = 0x009cf000, 0x00a70000
IMAGE_SYM = '_joan_image'
DIR32 = 6           # IMAGE_REL_I386_DIR32
SKIP_SECTIONS = {'.llvm_addrsig', '.sxdata'}   # symbol index lists (indices shift with the extra symbol)


class Coff:
    def __init__(self, path):
        d = open(path, 'rb').read()
        self.data = d
        (self.machine, nsec, self.stamp, symptr, nsym, opt, self.flags) = struct.unpack_from('<HHIIIHH', d, 0)
        self.syms = []
        strtab = symptr + 18 * nsym
        i = 0
        while i < nsym:
            raw = d[symptr + 18 * i:symptr + 18 * i + 18]
            name8, value, secno, typ, cls, naux = struct.unpack('<8sIhHBB', raw)
            if name8[:4] == b'\0\0\0\0':
                off = struct.unpack_from('<I', name8, 4)[0]
                e = d.index(b'\0', strtab + off)
                name = d[strtab + off:e].decode('latin1')
            else:
                name = name8.rstrip(b'\0').decode('latin1')
            aux = d[symptr + 18 * (i + 1):symptr + 18 * (i + 1 + naux)]
            self.syms.append((name, value, secno, typ, cls, aux))
            for _ in range(naux):
                self.syms.append(None)
            i += 1 + naux
        self.sections = []
        for k in range(nsec):
            o = 20 + opt + 40 * k
            (name8, vsize, va, rsize, rptr, relptr, lnptr, nrel, nln, ch) = struct.unpack_from('<8sIIIIIIHHI', d, o)
            name = name8.rstrip(b'\0').decode('latin1')
            if name.startswith('/'):
                off = int(name[1:])
                e = d.index(b'\0', strtab + off)
                name = d[strtab + off:e].decode('latin1')
            raw = d[rptr:rptr + rsize] if rptr else b''
            relocs = []
            if nrel == 0xffff and ch & 0x01000000:
                nrel = struct.unpack_from('<I', d, relptr)[0]
                first = 1
            else:
                first = 0
            for r in range(first, nrel):
                rva, si, t = struct.unpack_from('<IIH', d, relptr + 10 * r)
                relocs.append((rva, t, self.sym_key(si)))
            self.sections.append({'name': name, 'size': rsize, 'ch': ch, 'raw': raw, 'relocs': relocs})

    def sym_key(self, i):
        s = self.syms[i]
        if s is None:
            return ('?aux', i)
        name, value, secno, typ, cls, aux = s
        if cls == 3 and value == 0 and secno > 0:   # section symbol (static, value 0): identify by section number
            return ('section', name, secno)
        if cls == 3:                                 # other static symbol: name + section + value
            return ('static', name, secno, value)
        return ('ext', name)

    def defined_symbols(self):
        out = collections.Counter()
        for s in self.syms:
            if s is None:
                continue
            name, value, secno, typ, cls, aux = s
            out[(name, value, secno, typ, cls, aux if cls != 3 or value != 0 else aux[:14])] += 1
        return out

    def func_at(self, secno, off):
        best = None
        for s in self.syms:
            if s is None:
                continue
            name, value, sn, typ, cls, aux = s
            if sn == secno and value <= off and (typ & 0x20) and (best is None or value > best[1]):
                best = (name, value)
        return '%s+0x%x' % best if best else '?'


def compare(a_path, b_path):
    """('identical' | 'rebased' | 'DIFFERENT', counts or reasons)"""
    da, db = open(a_path, 'rb').read(), open(b_path, 'rb').read()
    if len(da) == len(db) and da[:4] + da[8:] == db[:4] + db[8:]:
        return 'identical', {}
    A, B = Coff(a_path), Coff(b_path)
    why = []
    if len(A.sections) != len(B.sections):
        return 'DIFFERENT', ['section count %d -> %d' % (len(A.sections), len(B.sections))]
    counts = collections.Counter()
    for k, (sa, sb) in enumerate(zip(A.sections, B.sections), 1):
        if sa['name'] in SKIP_SECTIONS:
            continue
        if sa['name'] != sb['name'] or sa['size'] != sb['size'] or sa['ch'] != sb['ch']:
            why.append('section %d %s size 0x%x -> %s size 0x%x' % (k, sa['name'], sa['size'], sb['name'], sb['size']))
            continue
        ra = {r[0]: r for r in sa['relocs']}
        rb = {r[0]: r for r in sb['relocs']}
        if len(ra) != len(sa['relocs']) or len(rb) != len(sb['relocs']):
            why.append('section %d %s: two relocations at one offset' % (k, sa['name']))
            continue
        # relocation offsets of AFTER that are new: must be DIR32 _joan_image with before = absolute image address
        patched = set()
        for off, r in rb.items():
            if off in ra:
                if ra[off] != r:
                    why.append('section %d %s +0x%x (%s): relocation %s -> %s' % (
                        k, sa['name'], off, A.func_at(k, off), ra[off][1:], r[1:]))
                continue
            if r[1] != DIR32 or r[2] != ('ext', IMAGE_SYM):
                why.append('section %d %s +0x%x (%s): new relocation %s' % (k, sa['name'], off, B.func_at(k, off), r[1:]))
                continue
            va = struct.unpack_from('<I', sa['raw'], off)[0]
            addend = struct.unpack_from('<I', sb['raw'], off)[0]
            if not (KVA <= va <= KEND) or addend != va - KVA:
                why.append('section %d %s +0x%x (%s): before 0x%08x, after _joan_image+0x%x' % (
                    k, sa['name'], off, B.func_at(k, off), va, addend))
                continue
            patched.update(range(off, off + 4))
            counts['text' if sa['ch'] & 0x20 else 'data'] += 1
        for off in ra:
            if off not in rb:
                why.append('section %d %s +0x%x (%s): relocation %s dropped' % (k, sa['name'], off, A.func_at(k, off),
                                                                                ra[off][1:]))
        if sa['raw'] != sb['raw']:
            for i, (x, y) in enumerate(zip(sa['raw'], sb['raw'])):
                if x != y and i not in patched:
                    why.append('section %d %s +0x%x (%s): byte 0x%02x -> 0x%02x' % (k, sa['name'], i,
                                                                                    B.func_at(k, i), x, y))
                    break
    # symbols: AFTER = BEFORE + the undefined _joan_image (+ undefined declarations nothing uses)
    sa, sb = A.defined_symbols(), B.defined_symbols()
    extra = sb - sa
    missing = sa - sb
    for (name, value, secno, typ, cls, aux), n in extra.items():
        if name == IMAGE_SYM and secno == 0:
            continue
        if cls == 3 and value == 0 and secno > 0:
            continue   # section symbol aux (checksum) differs with the section bytes
        why.append('symbol added: %s (section %d, class %d)' % (name, secno, cls))
    for (name, value, secno, typ, cls, aux), n in missing.items():
        if cls == 3 and value == 0 and secno > 0:
            continue
        why.append('symbol removed / changed: %s (section %d, value 0x%x)' % (name, secno, value))
    if why:
        return 'DIFFERENT', why
    return 'rebased', counts


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    verbose = '--verbose' in sys.argv
    before, after = args[0], args[1]
    names = sorted(set(os.path.basename(p) for p in glob.glob(before + '/*.o')) |
                   set(os.path.basename(p) for p in glob.glob(after + '/*.o')))
    tally = collections.Counter()
    fields = collections.Counter()
    bad = 0
    for n in names:
        a, b = os.path.join(before, n), os.path.join(after, n)
        if not os.path.exists(a) or not os.path.exists(b):
            print('ONLY %s %s' % ('after' if os.path.exists(b) else 'before', n))
            tally['only one side'] += 1
            continue
        kind, info = compare(a, b)
        tally[kind] += 1
        if kind == 'rebased':
            fields.update(info)
            if verbose:
                print('rebased   %-50s %s' % (n, dict(info)))
        elif kind == 'DIFFERENT':
            bad += 1
            print('DIFFERENT %s' % n)
            for w in info[:8]:
                print('    ' + w)
            if len(info) > 8:
                print('    ... %d more' % (len(info) - 8))
    print('objects: ' + ', '.join('%s %d' % kv for kv in sorted(tally.items())))
    print('rebased fields: %d in code, %d in data (each: absolute image address -> _joan_image + same offset)' % (
        fields['text'], fields['data']))
    sys.exit(1 if bad or tally['only one side'] else 0)


if __name__ == '__main__':
    main()
