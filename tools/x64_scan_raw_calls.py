#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""x64 scan (Phase 2, skill oj-x64-port): raw slot calls - thiscall<R>(fn::Name, this, args...), cdecl_call<R>(fn::Name,
args...), stdcall_call<R>(fn::Name, args...) - whose argument kinds (float vs integer / pointer) disagree with the
replacement's parameter kinds at the same position. On x86 both are a dword on the stack; on System V x86-64 a float
argument travels in an XMM register and an integer one in a general register, so the callee reads the wrong slot
(Players::find_slot_wrapper got x = y = z = 0 from formmove.cpp, 2026-10-30). The facade path (typed parameters) is
covered by x64_scan_abi_kinds.py; this scan covers the calls that bypass the facades.
Usage: python3 tools/x64_scan_raw_calls.py   (run from the repository root).

A scan is advisory: every hit is checked against the code; a fix is proven with objcmp / the replay gate (CLAUDE.md 5).
"""
import re, pathlib, sys
addr_of = {}
for m in re.finditer(r'inline constexpr Code (\w+)\{\d+\};\s*//\s*(?:0x[0-9a-f]{8} -> )?(0x[0-9a-f]{8})', pathlib.Path('src/fn_names.hpp').read_text()):
    addr_of[m.group(1)] = int(m.group(2), 16)
reg_re = re.compile(r'^\s*[A-Z][A-Za-z0-9_]*\((?:"[^"]*",\s*)?"([^"]+)",\s*(0x[0-9a-fA-F]+)u?,\s*&(?:(\w+)::)?(\w+)\)', re.M)
impl = {}
files = {}
for f in sorted(pathlib.Path('src').rglob('*.cpp')):
    s = str(f)
    if '/original/gen/' in s or '/data/' in s: continue
    t = f.read_text(errors='replace'); files[s] = t
    for m in reg_re.finditer(t):
        addr, fn = int(m.group(2), 16), m.group(4)
        d = re.search(r'\n\s*(?:static\s+)?[\w:<>\*&\s]+?\s+(?:JOAN_\w+\s+)?' + re.escape(fn) + r'\(([^)]*)\)\s*(?:const\s*)?(?:->[^{]*)?\{', t)
        if d: impl[addr] = (s, [p.strip() for p in d.group(1).split(',') if p.strip()])
FLOATISH = re.compile(r'\bLf\(|static_cast<float>|static_cast<double>|\bfloat\b|\bdouble\b|\* kCell\b|\bfpu\.|\bk0\b|\.[xyz]\b(?!\w)|\bf32_|vlen\(|\bdeg\(|\bsqrt')
INTISH = re.compile(r'\bLd\(|\bLw\(|\bLb\(|static_cast<(?:u32|i32|std::u?int32_t|std::u?int16_t|u16|i16|u8|i8|int|unsigned)>|arg::(?:i32|u32|i16|u16|bits|mptr|ptr|str)\(|\b(?:u32|i32)\{|\bnullptr\b|\bobj\(|&\w|\bthis\b|^\s*\d+u?$|^\s*0x[0-9a-f]+u?$')
def kind_of_arg(a):
    a = a.strip()
    if INTISH.search(a): return 'I'
    if FLOATISH.search(a): return 'F'
    return '?'
def kind_of_param(p):
    p = re.sub(r'\s*\w+$', '', p).replace('const ', '').strip()
    if p in ('float', 'double'): return 'F'
    return 'I'
# fn::Name, or a file-local alias `constexpr fn::Code kName = fn::Name;` (psystem.cpp kNodeAngleA, 2026-11-06);
# the template argument list is optional (`thiscall(kX, node, a)` has none).
call_re = re.compile(r'\b(thiscall|cdecl_call|stdcall_call|fastcall_call)(?:<[^;]*?>)?\(\s*(?:fn::)?(\w+)\s*,')
alias_re = re.compile(r'constexpr\s+(?:joan::)?fn::Code\s+(\w+)\s*=\s*(?:joan::)?fn::(\w+)\s*;')
def declared_kind(name, text, pos):
    # a bare identifier argument: the type of its nearest declaration above the call (float / double -> F)
    best = None
    for d in re.finditer(r'\b((?:const\s+)?(?:float|double|std::u?int(?:8|16|32|64)_t|u32|i32|u16|i16|u8|i8|int|unsigned|bool|std::uintptr_t|uptr|void\s*\*|[\w:]+\s*\*))\s+' + re.escape(name) + r'\b\s*[=;,)\[{]', text[:pos]):
        best = d.group(1)
    if best is None: return '?'
    return 'F' if re.search(r'\b(?:float|double)\b', best) and '*' not in best else 'I'
def split_args(text, start):
    depth, args, cur, i = 0, [], '', start
    while i < len(text):
        c = text[i]
        if c == '-' and text[i + 1:i + 2] == '>':  # a member arrow is not a template close
            cur += '->'; i += 2; continue
        if c in '([{<': depth += 1
        elif c in ')]}>':
            if depth == 0: args.append(cur); return args
            depth -= 1
        if c == ',' and depth == 0: args.append(cur); cur = ''
        else: cur += c
        i += 1
    return args
rows = []
for s, t in files.items():
    for m in call_re.finditer(t):
        conv, name = m.group(1), m.group(2)
        aliases = {a.group(1): a.group(2) for a in alias_re.finditer(t)}
        name = aliases.get(name, name)
        addr = addr_of.get(name)
        if addr is None or addr not in impl: continue
        ifile, params = impl[addr]
        args = split_args(t, m.end())
        if conv == 'thiscall': args = args[1:]   # the object
        if len(args) != len(params): continue   # a different arity: left to the reader
        line = t[:m.start()].count('\n') + 1
        for i, (a, p) in enumerate(zip(args, params)):
            ka, kp = kind_of_arg(a), kind_of_param(p)
            ptype = re.sub(r'\s*\w+$', '', p).replace('const ', '').strip()
            if '*' not in ptype and not re.search(r'\b(?:float|double|bool|char|int|unsigned|long|u?int(?:8|16|32|64)_t|u8|i8|u16|i16|u32|i32|uintptr_t|uptr)\b', ptype):
                continue  # a struct by value: x64_scan_byvalue.py
            if ka == '?':
                bare = re.fullmatch(r'\s*-?\s*(\w+)\s*', a)
                if bare: ka = declared_kind(bare.group(1), t, m.start())
            if ka != '?' and ka != kp:
                rows.append((s, line, name, f'{addr:#x}', i, a.strip()[:60], p.strip()[:40], ifile.split('/')[-1], ka))
for r in rows: print('%s:%d %s %s arg %d `%s` is %s-like, impl param `%s` (%s)' % (r[0], r[1], r[2], r[3], r[4], r[5], 'float' if r[8] == 'F' else 'integer', r[6], r[7]))
print(len(rows), 'kind mismatches at raw slot calls', file=sys.stderr)
