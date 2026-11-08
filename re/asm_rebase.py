#!/usr/bin/env python3
"""Stage S7b-main (docs/RUNTIME.md): data-image literals inside MSVC inline asm -> the joan_image object.

Usage (from the repository root):
  asm_rebase.py [--check] [--exe joan.exe] [hook dir]

Every hex literal of the data image range [0x009cf000, 0x00a70000) inside an __asm block / __asm line of src/** is
rewritten (re/gen_data_names.py scan_source, kind 'asm'):
  memory operand   `fld qword ptr ds:[0x009d8818]`  -> `fld qword ptr ds:[joan_image + 0x9818]`
                   `mov ax, word ptr [esi+edx*2+0x009d0000]` -> `... [esi+edx*2+joan_image + 0x1000]`
  immediate        `push 0x9d4088` / `mov ecx, 0xa3ce5a` -> `push offset joan_image + 0x5088`
The same instruction comes out, with a DIR32 relocation against joan_image instead of the absolute address (the ds:
prefix is kept as it was). The original address is appended to the line's comment (`// 57fc55 [0x00a6432c]`; a
`/* */` comment on a macro continuation line). re/fn_scan.py expands `joan_image + 0x...` back to the original
literal for the analysis scripts (data_classify, gen_data_names), so the object map does not change.

With --exe, every rewritten IMMEDIATE (a memory operand [absolute] is an address by construction) on a line whose
comment starts with the original instruction address (`// 57fc55`) is checked against joan.exe's base relocations:
the original instruction must carry a relocation whose value is the literal (an address, not a number that happens
to lie in the range). Lines without such a proof are listed for review.
--check: change nothing, exit 1 when a literal is left.
"""
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fn_scan as F  # noqa: E402
import gen_data_names as G  # noqa: E402

KVA, KEND = 0x009cf000, 0x00a70000
GENERATED = {'code_map.inc', 'code_slots.inc', 'census_table.inc', 'import_table.inc', 'image_layout.inc', 'fn_names.hpp',
             'data_names.hpp', 'image.hpp'}
INSN_COMMENT = re.compile(r'//\s*([0-9a-fA-F]{6})\b')


def load_relocs(exe):
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from pe import PE
    p = PE(exe)
    rva, size = p.dirs[5]
    o = p.rva2off(rva)
    end = o + size
    out = {}
    while o < end:
        page, bsz = struct.unpack_from('<II', p.data, o)
        if bsz == 0:
            break
        for k in range((bsz - 8) // 2):
            e = struct.unpack_from('<H', p.data, o + 8 + 2 * k)[0]
            if e >> 12 == 3:
                loc = p.base + page + (e & 0xfff)
                out[loc] = p.u32(loc)
        o += bsz
    return out


def in_brackets(line, col):
    """col (index in line) lies inside [...]"""
    lb = line.rfind('[', 0, col)
    rb = line.rfind(']', 0, col)
    return lb > rb


def rewrite_file(path, imports, relocs, check):
    src = open(path, newline='').read()
    hits = [(s, e, v) for s, e, v, kind in G.scan_source(src, imports) if kind == 'asm' and KVA <= v < KEND]
    if not hits:
        return 0, []
    if check:
        return len(hits), []
    unproven = []
    per_line = {}
    for s, e, v in hits:
        ls = src.rfind('\n', 0, s) + 1
        per_line.setdefault(ls, []).append((s, e, v))
    out = []
    pos = 0
    for ls in sorted(per_line):
        le = src.find('\n', ls)
        le = len(src) if le < 0 else le
        line = src[ls:le]
        new = line
        vals = []
        imms = []
        for s, e, v in sorted(per_line[ls], reverse=True):
            a, b = s - ls, e - ls
            off = v - KVA
            if in_brackets(line, a):
                rep = 'joan_image + 0x%x' % off
            else:
                rep = 'offset joan_image + 0x%x' % off
                imms.append(v)
            new = new[:a] + rep + new[b:]
            vals.append(v)
        # keep a trailing // comment in its column where the line has room (spaces before it)
        grow = len(new) - len(line)
        ci = new.find('//')
        if grow > 0 and ci > 0:
            j = ci
            while j > 0 and new[j - 1] == ' ':
                j -= 1
            spaces = ci - j
            cut = min(grow, spaces - 1)
            if cut > 0:
                new = new[:ci - cut] + new[ci:]
        vals.reverse()
        tag = ' '.join('[0x%08x]' % v for v in vals)
        cont = new.rstrip().endswith('\\')
        if cont:
            body = new.rstrip()[:-1].rstrip()
            pad = len(new.rstrip()) - 1 - len(body)
            new = '%s /* %s */%s\\' % (body, tag, ' ' * max(pad - len(tag) - 7, 1))
        elif '//' in new:
            new = new.rstrip() + ' ' + tag
        else:
            new = new.rstrip() + '  // ' + tag
        if relocs is not None and imms:
            # a memory operand [absolute] is an address by construction; an immediate needs the proof
            m = INSN_COMMENT.search(line)
            ok = False
            if m:
                ia = int(m.group(1), 16)
                ok = all(any(relocs.get(ia + k) == v for k in range(1, 12)) for v in imms)
            if not ok:
                unproven.append((path, src.count('\n', 0, ls) + 1, line.strip()))
        out.append(src[pos:ls])
        out.append(new)
        pos = le
    out.append(src[pos:])
    open(path, 'w', newline='').write(''.join(out))
    return len(hits), unproven


def main():
    args = sys.argv[1:]
    check = '--check' in args
    exe = None
    if '--exe' in args:
        exe = args[args.index('--exe') + 1]
        args = [a for a in args if a not in ('--exe', exe)]
    args = [a for a in args if not a.startswith('--')]
    hook = args[0] if args else 'src'
    imports = G.load_imports(hook)
    relocs = load_relocs(exe) if exe else None
    total, files, unproven = 0, 0, []
    for rel, path in G.hook_sources(hook):
        if os.path.basename(rel) in GENERATED or rel.startswith('data' + os.sep) or rel.startswith('data/'):
            continue
        n, u = rewrite_file(path, imports, relocs, check)
        if n:
            files += 1
            total += n
            print('%-40s %4d' % (rel, n))
        unproven += u
    print('%s: %d asm literals in %d files' % ('left' if check else 'rewritten', total, files))
    for path, ln, text in unproven:
        print('NOT PROVEN by a joan.exe relocation: %s:%d  %s' % (path, ln, text[:110]))
    if relocs is not None:
        print('%d lines without a relocation proof' % len(unproven))
    sys.exit(1 if check and total else 0)


if __name__ == '__main__':
    main()
