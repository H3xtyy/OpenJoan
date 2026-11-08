#!/usr/bin/env python3
"""Generate hook/census_table.inc: every code entry of joan.exe with the bytes a census trampoline may relocate.

Usage:
  gen_census.py <joan.exe> <joan.asm> <functions.json> <thunks.json> <out census_table.inc>
                [--names <file>...]   extra name sources (hook/modules/*.cpp registrations, ids.md tables)

joan.asm is `objdump -d -M intel --no-show-raw-insn` of joan.exe (instruction lengths come from consecutive
addresses, the bytes from joan.exe itself).

Entries (STANDALONE.md stage S2): the function starts of functions.json inside the real code ranges
  0x00414000..0x007d5000  game code + statically linked libraries (CRT, D3DX8, libjpeg, ...)
  0x007dd410..0x007e50a0  .text$x: C++ EH unwind funclets / catch blocks and __ehhandler stubs (`mov eax,FuncInfo;
                          jmp ___CxxFrameHandler`), reached only through FuncInfo tables (.rdata) and SEH records
(0x007d5000..0x007dd410 is int3 fill; 0x007e50a0..0x009ce700 is the high-entropy block decoded as junk - no
reference from real code or data reaches it), plus starts named in the --names id tables (real boundaries that
functions.json merged), plus the CRT entry. functions.json already contains every call target, thunk target,
data-referenced code address (vtables, callbacks, SEH scope-table filters / handlers, EH funclets) and code
address pushed / moved as an immediate.

For each entry the generator walks instructions from the entry until at least 5 bytes are covered. The walk
fails (length 0, reason given) when
  - an instruction is a call that returns into entry..entry+4 (that return address would be overwritten) or an
    indirect / far call, loop/jcxz, a far or 16-bit-operand branch, or undecodable,
  - any code jumps / calls into entry+1..entry+N-1, a relocation points there, or another entry starts there,
  - the body ends (ret / jmp) inside the first 5 bytes and the following bytes are not int3 / nop padding,
  - a relocated dword of the image (.reloc) does not sit inside one instruction's operand (data in code).
Relative branches are allowed and recorded as fixups: rel32 (E9 / 0F 8x: the 4-byte operand is rewritten at
install time), short jmp (EB -> E9 rel32) and short jcc (7x -> 0F 8x rel32). A branch back to the entry itself
is kept (it re-enters through the trampoline and is counted again). A direct call (E8) that ends at or after
entry+5 ends the relocated bytes: the trampoline pushes the ORIGINAL return address and jumps to the callee, so the
callee returns into the untouched original bytes and sees the same return address as without the census.
Starts that functions.json found only by its "code after int3 padding" heuristic, with no reference and no name,
are listed but never patched (some are data inside .text, e.g. the "VC20XC00" signature at 0x007bbe50).
"""
import glob
import json
import re
import sys

sys.path.insert(0, __import__('os').path.dirname(__file__))
from pe import PE  # noqa: E402

CODE_RANGES = [(0x00414000, 0x007d5000), (0x007dd410, 0x007e50a0)]
LIB_LO, LIB_HI = 0x00763d00, 0x007d5000
PREFIXES = {0x66, 0x67, 0xF2, 0xF3, 0x2E, 0x3E, 0x26, 0x36, 0x64, 0x65, 0xF0}
MAX_BYTES = 13  # hook/census.cpp: 19 + 3 * MAX_BYTES + 5 <= the 64-byte stub slot

R_OK, R_SHORT, R_CALL, R_INTO, R_DATA, R_BAD, R_LOOP, R_END, R_UNREF = range(9)
REASONS = ['ok', 'next entry within 5 bytes', 'call returning into the first 5 bytes', 'branch target inside',
           'data in code', 'undecodable', 'loop/jcxz/far/16-bit branch', 'body ends before 5 bytes',
           'unreferenced heuristic start']
FIX_REL32, FIX_JMP8, FIX_JCC8, FIX_CALL32 = 1, 2, 3, 4


def in_code(a):
    return any(lo <= a < hi for lo, hi in CODE_RANGES)


def main():
    args = sys.argv[1:]
    names_files = []
    if '--names' in args:
        i = args.index('--names')
        names_files = args[i + 1:]
        args = args[:i]
    exe, asm, fjson, tjson, out = args
    pe = PE(exe)
    img = pe.data

    def byte(va):
        return img[pe.va2off(va)]

    # ---- instructions
    line_re = re.compile(r'^\s*([0-9a-f]+):\s+(\S+)\s*(.*)$')
    addrs, mnems, opss = [], [], []
    for ln in open(asm):
        m = line_re.match(ln)
        if not m:
            continue
        a = int(m.group(1), 16)
        if a > CODE_RANGES[-1][1] + 16:
            break
        addrs.append(a)
        mnems.append(m.group(2))
        opss.append(m.group(3))
    index = {a: i for i, a in enumerate(addrs)}

    def length(i):
        return addrs[i + 1] - addrs[i]

    # ---- addresses that code or data can transfer control to (forbidden inside a relocated range)
    into, imm = set(), set()
    tgt_re = re.compile(r'^0x([0-9a-f]+)\b')
    for i, a in enumerate(addrs):
        if not in_code(a) and a >= 0x00414000:
            continue
        mn = mnems[i]
        if mn.startswith('j') or mn.startswith('call') or mn.startswith('loop'):
            m = tgt_re.match(opss[i])
            if m:
                into.add(int(m.group(1), 16))
        if mn in ('push', 'mov'):  # code addresses taken as immediates (callbacks, EH handlers)
            for h in re.findall(r'0x([0-9a-f]+)', opss[i]):
                imm.add(int(h, 16))
    relocs = pe.relocs()
    thunks = {int(k, 16): int(v, 16) for k, v in json.load(open(tjson)).items()}
    reloc_field = set()
    for loc in relocs:
        v = pe.u32(loc)
        v = thunks.get(v, v)
        if in_code(v):
            into.add(v)
        if in_code(loc):
            reloc_field.add(loc)

    # ---- names
    names = {}
    name_pat = re.compile(r'"([^"\n]{1,120})"\s*,\s*0x0*([0-9a-fA-F]{6})\b')
    ids_pat = re.compile(r'^\|\s*(?:0x)?0*([0-9a-f]{6})\s*\|(?:[^|]*\|)?\s*([^|]+?)\s*\|')
    extra_starts = set()
    for pattern in names_files:
        for path in sorted(glob.glob(pattern)):
            text = open(path, errors='replace').read()
            if path.endswith('.md'):
                for ln in text.splitlines():
                    m = ids_pat.match(ln)
                    if not m:
                        continue
                    a = int(m.group(1), 16)
                    # name column (2nd or 3rd): the identifier directly followed by '(' ("D3DXMATRIX* WINAPI
                    # D3DXMatrixMultiply(out, ...)"), else the column's first word ("_matherr default (...)")
                    cols = [c.strip().strip('`').strip() for c in ln.strip().strip('|').split('|')]
                    nm = None
                    for c in cols[1:3]:
                        mm = re.search(r'([A-Za-z_][\w:~]*(?<=operator)[-+*/=!<>\[\]]{1,3}|[A-Za-z_][\w:~]*)\(', c) or re.match(r'([A-Za-z_][\w:~]*)\b', c)
                        if mm and mm.group(1) not in ('PURE', 'STATEFUL', 'GAME', 'HRESULT', 'void', 'int'):
                            nm = mm.group(1)
                            break
                    if nm and len(nm) > 2:
                        names.setdefault(a, nm)
                    if in_code(a) and a in index:
                        extra_starts.add(a)
            else:
                for m in name_pat.finditer(text):
                    a = int(m.group(2), 16)
                    names.setdefault(a, m.group(1))

    funcs = json.load(open(fjson))
    files = {}
    starts = {pe.entry}
    for f in funcs:
        a = int(f['addr'], 16)
        if in_code(a):
            starts.add(a)
            if f.get('file'):
                files[a] = f['file']
    starts |= extra_starts
    # functions.json also holds starts found only by its "code after int3 padding" heuristic; some of those are
    # data inside .text (the "VC20XC00" signature at 0x7bbe50, float / jump tables). Without any reference and
    # without a name from a registration or id table they are listed, never patched.
    thunk_targets = set(thunks.values())
    referenced = into | {thunks.get(v, v) for v in imm} | thunk_targets | extra_starts | {pe.entry}
    starts = sorted(s for s in starts if s in index)
    start_set = set(starts)

    # ---- per-entry relocation analysis
    entries, fixups = [], []
    stats = [0] * len(REASONS)
    maxlen = 0
    for k, s in enumerate(starts):
        nxt = starts[k + 1] if k + 1 < len(starts) else CODE_RANGES[-1][1]
        i = index[s]
        total, reason, fx, ended = 0, R_OK, [], False
        if s not in referenced and s not in names:
            reason = R_UNREF
        while total < 5 and reason == R_OK:
            a = addrs[i]
            if a >= nxt:
                reason = R_SHORT
                break
            if a != s and (a in into or a in start_set):
                reason = R_INTO
                break
            n = length(i)
            if mnems[i] == '(bad)' or n <= 0 or n > 15:
                reason = R_BAD
                break
            raw = bytes(byte(a + j) for j in range(n))
            # relocated fields must sit inside this instruction's operand, never on its first byte
            for j in range(n):
                if a + j in reloc_field and (j == 0 or j + 4 > n):
                    reason = R_DATA
            if a == s and any(r in reloc_field for r in (s - 1, s - 2, s - 3)):
                reason = R_DATA  # a relocated dword that starts before the entry spans into it
            if reason:
                break
            if ended:  # bytes after ret / jmp: only padding may follow
                if raw[0] not in (0xCC, 0x90):
                    reason = R_END
                    break
                total += n
                i += 1
                continue
            p = 0
            while p < n and raw[p] in PREFIXES:
                p += 1
            op = raw[p] if p < n else None
            has66 = 0x66 in raw[:p] or 0x67 in raw[:p]
            if op is None:
                reason = R_BAD
                break
            modrm_reg = (raw[p + 1] >> 3) & 7 if p + 1 < n else -1
            if op == 0xE8 and p == 0 and a + n >= s + 5 and tgt_re.match(opss[i]):
                # direct call ending at or after entry+5: relocated as `push <original return address>; jmp
                # target`, so the callee returns into the untouched original bytes (identical return address)
                fx.append((a - s, FIX_CALL32, int(tgt_re.match(opss[i]).group(1), 16)))
                total += n
                break
            if op in (0xE8, 0x9A) or (op == 0xFF and modrm_reg in (2, 3)):
                reason = R_CALL
                break
            if op in (0xE0, 0xE1, 0xE2, 0xE3, 0xEA) or (op == 0xFF and modrm_reg == 5):
                reason = R_LOOP
                break
            m = tgt_re.match(opss[i])
            if op == 0xE9 or op == 0xEB or 0x70 <= op <= 0x7F or (op == 0x0F and p + 1 < n and 0x80 <= raw[p + 1] <= 0x8F):
                if has66 or not m:
                    reason = R_LOOP
                    break
                target = int(m.group(1), 16)
                off = a - s
                if op == 0xE9:
                    fx.append((off + p + 1, FIX_REL32, target))
                elif op == 0x0F:
                    fx.append((off + p + 2, FIX_REL32, target))
                elif op == 0xEB:
                    fx.append((off + p, FIX_JMP8, target))
                else:
                    fx.append((off + p, FIX_JCC8, target))
            if op in (0xC3, 0xC2, 0xCB, 0xCA, 0xE9, 0xEB) or (op == 0xFF and modrm_reg == 4):
                ended = True
            total += n
            i += 1
        if reason == R_OK and s + total > nxt:
            reason = R_SHORT
        if reason == R_OK and total > MAX_BYTES:
            reason = R_BAD
        stats[reason] += 1
        nm = names.get(s)
        if not nm:
            kind = 'funclet' if s >= 0x007dd410 else 'lib' if s >= LIB_LO else None
            if s == pe.entry:
                nm = 'WinMainCRTStartup'
            elif kind:
                nm = '[%s]' % kind
            elif s in files:
                nm = '[%s]' % files[s]
        if reason == R_OK:
            raw = bytes(byte(s + j) for j in range(total))
            maxlen = max(maxlen, total)
            entries.append((s, total, reason, len(fixups), len(fx), nxt - s, raw, nm))
            fixups.extend(fx)
        else:
            entries.append((s, 0, reason, 0, 0, nxt - s, b'', nm))

    def cstr(b):
        return '"' + ''.join('\\x%02x' % c for c in b) + '"'

    def cname(nm):
        if not nm:
            return 'nullptr'
        return '"' + nm.replace('\\', '\\\\').replace('"', "'") + '"'

    with open(out, 'w', newline='\n') as o:
        o.write('// Generated by re/gen_census.py - do not edit. Census table for hook/census.cpp (STANDALONE.md S2).\n')
        o.write('// Regenerate (from the repository root): python3 re/gen_census.py <joan.exe> <joan.asm> <functions.json>\n'
                '//   <thunks.json> hook/census_table.inc --names "hook/modules/*.cpp" "<scratchpad>/w113_*/ids.md"\n')
        o.write('// %d entries: %s\n' % (len(entries), ', '.join('%s %d' % (REASONS[r], c) for r, c in enumerate(stats) if c)))
        o.write('// %d fixups. Entry: address, relocatable length (0 = not instrumentable), reason, first fixup,\n'
                '// fixup count, bytes up to the next entry, original bytes, best name.\n'
                '// Fixup: offset in the bytes, kind (1 rel32 operand, 2 short jmp EB, 3 short jcc 7x,\n'
                '// 4 call E8 = last instruction), absolute target.\n' % len(fixups))
        o.write('#define JOAN_CENSUS_MAX_BYTES %d\n' % max(maxlen, 5))
        o.write('#define JOAN_CENSUS_REASONS %s\n' % ', '.join('"%s"' % r for r in REASONS))
        o.write('#ifdef JOAN_CENSUS_ENTRIES\n')
        for s, n, r, f0, fn, size, raw, nm in entries:
            o.write('{0x%08x, %d, %d, %d, %d, %d, %s, %s},\n' % (s, n, r, f0, fn, size, cstr(raw), cname(nm)))
        o.write('#endif\n#ifdef JOAN_CENSUS_FIXUPS\n')
        for off, kind, target in fixups:
            o.write('{%d, %d, 0x%08x},\n' % (off, kind, target))
        if not fixups:
            o.write('{0, 0, 0},\n')
        o.write('#endif\n')
    print('%d entries -> %s' % (len(entries), out))
    for r, c in enumerate(stats):
        print('  %-30s %d' % (REASONS[r], c))
    print('  fixups %d, longest relocation %d bytes' % (len(fixups), maxlen))


if __name__ == '__main__':
    main()
