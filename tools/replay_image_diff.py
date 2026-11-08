#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Phase 2 step 4: compare two ReplayDumpImage files (an x86 and an x64 replay of the same recording) word by word.

Each file holds (u32 frame, N bytes of the normalised hashed range) records; frames present in both are compared. Words in
the mask file (one `0xADDR[+LEN]` per line or comma-separated, with `#` comments) are the known layout-dependent values
(DynArray element sizes, buffer byte counts, the renderer's ring bookkeeping and draw statistics) and are left out. The first differing frame and its words
are printed with their data symbols (src/data/data_symbols.inc).

  python3 tools/replay_image_diff.py <x86 dump> <x64 dump> [--mask FILE] [--frames N] [--words M] [--va 0x009f4000]

Exit status: 0 no unmasked difference, 1 a difference, 2 no frame in common (nothing was compared).
"""
import argparse, re, struct, sys, bisect, pathlib

ap = argparse.ArgumentParser()
ap.add_argument('a'); ap.add_argument('b')
ap.add_argument('--mask', default=None, help='words to leave out (default: re/replay/x64_layout_words.txt; --mask none for none)'); ap.add_argument('--frames', type=int, default=3); ap.add_argument('--words', type=int, default=40)
ap.add_argument('--va', default=None, help='original address of the hashed range (default: read from src/replay)')
ap.add_argument('--size', type=int, default=None)
args = ap.parse_args()

root = pathlib.Path(__file__).resolve().parent.parent
def const(name):
    for f in (root / 'src/replay').glob('*.[ch]pp'):
        m = re.search(name + r'\s*=\s*(0x[0-9a-fA-F]+|\d+)', f.read_text())
        if m: return int(m.group(1), 0)
    return None
va = int(args.va, 0) if args.va else const('kDataVa')
end = const('kDataEnd')
size = args.size or (end - va if (end is not None and va is not None) else None)
if va is None or size is None: sys.exit('cannot determine the hashed range; pass --va and --size')

syms = []
for line in (root / 'src/data/data_symbols.inc').read_text().splitlines():
    m = re.search(r'JOAN_DATA_SYMBOL\(\s*\w+\s*,\s*([^,]+),\s*(\w+)\s*,\s*(0x[0-9a-f]+)', line)
    if m: syms.append((int(m.group(3), 16), m.group(2), m.group(1).strip()))
syms.sort(); sym_addrs = [s[0] for s in syms]
def symbol(addr):
    i = bisect.bisect_right(sym_addrs, addr) - 1
    if i < 0: return f'{addr:#010x}'
    a, name, ty = syms[i]
    return f'{name}+{addr - a:#x} ({ty})'

masked = set()
# The replay's typed table: every pointer (4 bytes at any alignment, normalised to allocation-dependent tokens that
# differ between an x86 and an x64 run) and every untyped range (raw bytes, pointers at any offset) are left out:
# what remains is typed non-pointer data, which should be identical except for the masked width-dependent words.
typed_skip = set()
for line in (root / 'src/runtime/replay/hash_types.inc').read_text().splitlines():
    m = re.match(r'JOAN_HASH_PTR(?:_BASE)?\((0x[0-9a-f]+)u', line)
    if m:
        a = int(m.group(1), 16)
        for x in range(a & ~3, a + 4, 4): typed_skip.add(x)
        continue
    m = re.match(r'JOAN_HASH_UNTYPED(?:_BASE)?\((0x[0-9a-f]+)u,\s*(0x[0-9a-f]+)u', line)
    if m:
        a, n = int(m.group(1), 16), int(m.group(2), 16)
        for x in range(a & ~3, a + n, 4): typed_skip.add(x)
masked |= typed_skip
mask_path = root / 're/replay/x64_layout_words.txt' if args.mask is None else (None if args.mask == 'none' else pathlib.Path(args.mask))
if mask_path:
    text = re.sub(r'#.*', '', mask_path.read_text())
    for tok in re.split(r'[\s,]+', text):
        if not tok: continue
        m = re.match(r'(0x[0-9a-fA-F]+)(?:\+(0x[0-9a-fA-F]+|\d+))?$', tok)
        if not m: sys.exit(f'bad mask entry {tok}')
        a = int(m.group(1), 0); n = int(m.group(2), 0) if m.group(2) else 4
        for x in range(a & ~3, a + n, 4): masked.add(x)

def frames(path):
    # streamed: (frame, bytes) records in file order (the dumps hold the same frames in the same order)
    with open(path, 'rb') as f:
        rec = 4 + size
        while True:
            blob = f.read(rec)
            if len(blob) < rec: return
            yield struct.unpack('<I', blob[:4])[0], blob[4:]
def pairs():
    ga, gb = frames(args.a), frames(args.b)
    a = next(ga, None); b = next(gb, None)
    while a is not None and b is not None:
        if a[0] == b[0]:
            yield a[0], a[1], b[1]; a = next(ga, None); b = next(gb, None)
        elif a[0] < b[0]: a = next(ga, None)
        else: b = next(gb, None)
print(f'{args.a} vs {args.b}: {len(masked)} masked words ({len(typed_skip)} pointer / untyped words of the typed table; mask {mask_path})')
shown = 0
compared = 0
for fr, A, B in pairs():
    compared += 1
    if A == B: continue
    diffs = []
    for off in range(0, size, 4):
        if A[off:off+4] != B[off:off+4] and (va + off) not in masked:
            diffs.append(off)
    if not diffs: continue
    # Rank: value-like words (small integers, flags, floats) before pointer-like ones (heap addresses: aligned, large,
    # both differing in the same way), which the untyped blobs of the image hold raw and which differ between builds.
    def pointer_like(x):
        return x >= 0x01000000 and x < 0xa2000000 and (x & 7) == 0 and not (0x3c000000 <= x < 0x4c000000)
    def rank(off):
        a = struct.unpack('<I', A[off:off+4])[0]; b = struct.unpack('<I', B[off:off+4])[0]
        return 1 if (pointer_like(a) and pointer_like(b)) else 0
    diffs.sort(key=lambda off: (rank(off), off))
    values = sum(1 for off in diffs if rank(off) == 0)
    print(f'frame {fr}: {len(diffs)} differing words, {values} value-like')
    for off in diffs[:args.words]:
        a = struct.unpack('<I', A[off:off+4])[0]; b = struct.unpack('<I', B[off:off+4])[0]
        print(f'  {"ptr?" if rank(off) else "    "} {va + off:#010x} {symbol(va + off):48} x86 {a:#010x}  x64 {b:#010x}')
    shown += 1
    if shown >= args.frames: break
print(f'{compared} frames compared')
if compared == 0:
    print('NOTHING COMPARED: the dumps have no frame in common')
    sys.exit(2)
if shown == 0:
    print('no unmasked difference in the common frames')
    sys.exit(0)
sys.exit(1)
