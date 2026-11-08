# SPDX-License-Identifier: GPL-3.0-or-later
# x64 sweep 2026-10-31 (see README.md in this folder). Run from the folder holding x86.json / x64.json.
import json,re,pathlib,collections
X86=json.load(open('x86.json')); X64=json.load(open('x64.json'))
R='./'
bss={}
for m in re.finditer(r'::joan::orig::(\w+) b_([0-9a-f]+);\s*// 0x([0-9a-f]+) obj:(\w+)', open(R+'src/data/bss.inc').read()):
    bss[int(m.group(3),16)]=m.group(4)
names={}
for m in re.finditer(r'Sym<[^>]*>\s+(\w+)\{0x([0-9a-f]+)u\}', open(R+'src/data_names.hpp').read()):
    names[m.group(1)]=int(m.group(2),16)
def snake(n): return re.sub(r'(?<!^)(?=[A-Z])','_',n).lower()
s2c={}
for n in X64: s2c[snake(n)]=n
def omap(c):
    a={f[0]:f for f in X86[c]['fields'] if not f[3].startswith('S:')}
    b={f[0]:f for f in X64[c]['fields'] if not f[3].startswith('S:')}
    return {a[k][1]:(b[k][1],k) for k in a if k in b}
def drift(c,r):
    if c not in X86 or r not in X86 or c not in X64 or r not in X64: return None
    mc,mr=omap(c),omap(r); bad=[]
    for o,(o64,k) in mc.items():
        if o in mr and mr[o][0]!=o64: bad.append((k,hex(o),hex(o64),mr[o][1],hex(mr[o][0])))
    return bad
pat=re.compile(r'as_(\w+)\(\s*(?:img::|joan::img::)?obj\(d::(\w+)\)\s*\)')
alias=re.compile(r'(\w+)\s*=\s*(?:img::|joan::img::)?obj\(d::(\w+)\)')
hits=collections.defaultdict(list)
for f in pathlib.Path(R+'src').rglob('*.cpp'):
    s=str(f)
    if '/original/gen/' in s or '/data/' in s: continue
    t=f.read_text(errors='replace')
    al={m.group(1):m.group(2) for m in alias.finditer(t)}
    for ln,line in enumerate(t.split('\n'),1):
        for m in pat.finditer(line): 
            hits[(m.group(1),m.group(2))].append(f"{s.split('/src/')[1]}:{ln}")
        for m in re.finditer(r'as_(\w+)\(\s*(\w+)\s*\)',line):
            if m.group(2) in al: hits[(m.group(1),al[m.group(2)])].append(f"{s.split('/src/')[1]}:{ln}")
rows=[]
for (cs,sym),locs in hits.items():
    a=names.get(sym); real=bss.get(a)
    c=s2c.get(cs)
    if not real or not c or c==real: continue
    d=drift(c,real)
    if d: rows.append((c,real,sym,len(locs),d,locs))
rows.sort(key=lambda r:-r[3])
for c,real,sym,n,d,locs in rows:
    print(f"{c:18} on {sym:16} (obj:{real:16}) x{n:3}  drift {len(d)}: {d[0]}  | {' '.join(locs[:3])}")
print(len(rows))
