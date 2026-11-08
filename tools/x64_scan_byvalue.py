#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""x64 scan (Phase 2, skill oj-x64-port): by-value struct arguments of facade templates whose member kinds (float vs integer) differ between the caller's struct
and the replacement's parameter struct: identical 12 stack bytes on x86, different registers (XMM vs integer) on x64.
Usage: python3 tools/x64_scan_byvalue.py   (run from the repository root). Found the 13 vectors of 5675bc0; the
parameter and anonymous-struct forms added on 2026-10-30 find the compiled-script waypoint chain (escomp -> esfuncs -> minires).

A scan is advisory: every hit is checked against the code; a fix is proven with objcmp / the replay gate (CLAUDE.md 5).
"""
import re, pathlib, sys, collections
gen = pathlib.Path('src/original/gen')
facades = {}   # (class, method) -> (addr, [template positions])
addr_of = {}
for f in list(gen.glob('*_methods.inc')) + [gen / 'free_functions.hpp']:
    cls = None
    hpp = gen / (f.name[:-len('_methods.inc')] + '.hpp')
    if hpp.exists():
        m = re.search(r'^struct (\w+)', hpp.read_text(), re.M); cls = m.group(1) if m else None
    lines = f.read_text().splitlines()
    for i, l in enumerate(lines):
        m = re.match(r'/// (0x[0-9a-f]{8}) ', l)
        if not m or i + 2 >= len(lines) or not lines[i+1].startswith('template'): continue
        d = re.match(r'\s*(?:inline\s+)?(.+?)\s+(\w+)\(([^)]*)\)', lines[i+2])
        if not d: continue
        params = [p.strip() for p in d.group(3).split(',') if p.strip()]
        tpos = [k for k, p in enumerate(params) if re.match(r'T\d+\b', p)]
        facades[(cls or 'free', d.group(2))] = (int(m.group(1), 16), tpos)
def struct_kinds(text, name, before=None):
    ms = list(re.finditer(r'struct\s+' + re.escape(name) + r'\s*(?::[^{]*)?\{([^{}]*)\}', text))
    if before is not None: ms = [x for x in ms if x.start() < before] or ms[:1]
    if not ms: return None
    m = ms[-1]
    kinds = ''
    for decl in re.findall(r'^\s*((?:const\s+)?[\w:]+)\s+([\w, \[\]]+);', m.group(1), re.M):
        ty, names = decl
        n = len([x for x in names.split(',') if x.strip()])
        arr = re.findall(r'\[(\d+)\]', names)
        if arr: n = n - len(arr) + sum(int(a) for a in arr)
        kinds += ('F' if ty.replace('const ', '') in ('float', 'double') else 'I') * n
    return kinds
reg_re = re.compile(r'^\s*[A-Z][A-Za-z0-9_]*\((?:"[^"]*",\s*)?"([^"]+)",\s*(0x[0-9a-fA-F]+)u?,\s*&(?:(\w+)::)?(\w+)\)', re.M)  # B("name", addr, &fn) and JOAN_REPLACE("module", "name", addr, &fn)
impl = {}   # addr -> (file, [param types])
files = {}
for f in sorted(pathlib.Path('src').rglob('*.cpp')):
    s = str(f)
    if '/original/gen/' in s or '/data/' in s: continue
    t = f.read_text(errors='replace'); files[s] = t
    for m in reg_re.finditer(t):
        addr, fn = int(m.group(2), 16), m.group(4)
        d = re.search(r'\n\s*(?:static\s+)?[\w:<>\*&\s]+?\s+(?:JOAN_\w+\s+)?' + re.escape(fn) + r'\(([^)]*)\)\s*(?:const\s*)?(?:->[^{]*)?\{', t)
        if d: impl[addr] = (s, [p.strip() for p in d.group(1).split(',') if p.strip()])
rows = []
call_re = re.compile(r'->(\w+)\(')
def split_args(text, start):
    depth, args, cur = 0, [], ''
    i = start
    while i < len(text):
        c = text[i]
        if c == '-' and text[i + 1:i + 2] == '>':  # a member arrow is not a template close
            cur += '->'; i += 2; continue
        if c in '([{<': depth += 1
        elif c in ')]}>':
            if depth == 0: args.append(cur); return args, i
            depth -= 1
        if c == ',' and depth == 0: args.append(cur); cur = ''
        else: cur += c
        i += 1
    return args, i
for s, t in files.items():
    for m in call_re.finditer(t):
        key = None
        for (cls, meth), v in facades.items():
            if meth == m.group(1): key = (cls, meth); break
        if not key: continue
        addr, tpos = facades[key]
        args, _ = split_args(t, m.end())
        for pos in tpos:
            if pos >= len(args): continue
            a = args[pos].strip()
            var = re.match(r'^[A-Za-z_]\w*$', a)
            caller_type = None
            anon_kinds = None
            if var:
                before = t[max(0, m.start()-6000):m.start()]
                # a local, a member or a parameter declaration (`V3 v,` / `V3 v)` included: 2026-10-30, the waypoint chain)
                dm = list(re.finditer(r'\b(?:const\s+)?([A-Z]\w*)\s+' + re.escape(a) + r'\b\s*[;={(,)]', before))
                if dm: caller_type = dm[-1].group(1)
                # an anonymous struct built at the call: `const struct { u32 x, y, z; } pos{...}`
                am = list(re.finditer(r'(?:const\s+)?struct\s*\{([^{}]*)\}\s+' + re.escape(a) + r'\b\s*[{=;]', before))
                if am and (not dm or am[-1].start() > dm[-1].start()):
                    caller_type = '(anonymous struct)'
                    anon_kinds = struct_kinds('struct Anon {' + am[-1].group(1) + '}', 'Anon')
            else:
                cm = re.search(r'reinterpret_cast<(?:const\s+)?([A-Z]\w*)\s*\*?>', a) or re.search(r'^([A-Z]\w*)\s*[{(]', a)
                if cm: caller_type = cm.group(1)
            if not caller_type: continue
            ck = anon_kinds if anon_kinds is not None else struct_kinds(t, caller_type, m.start())
            ia = impl.get(addr)
            if not ia or not ck: continue
            ifile, iparams = ia
            # the facade param position maps to the impl's parameter at the same index (both without `this`)
            if pos >= len(iparams): continue
            itype = re.sub(r'\s*\w+$', '', iparams[pos]).replace('const ', '').replace('&', '').strip()
            ik = struct_kinds(files.get(ifile, ''), itype.split('::')[-1])
            if ik is None: continue
            if ck != ik:
                rows.append((s, t[:m.start()].count('\n')+1, f'{key[0]}::{key[1]}', f'{addr:#x}', caller_type, ck, itype, ik, ifile))
for r in sorted(set(rows)): print('%s:%d %s %s caller %s[%s] impl %s[%s] (%s)' % r)
print(len(set(rows)), 'mismatches', file=sys.stderr)
