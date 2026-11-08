# SPDX-License-Identifier: GPL-3.0-or-later
# x64 sweep 2026-10-31 (see README.md in this folder). Run from the folder holding x86.json / x64.json.
import json,re,pathlib,collections,sys
X86=json.load(open('x86.json')); X64=json.load(open('x64.json'))
names=[n for n in X86 if n in X64]
def leaves(n):
    a={f[0]:f for f in X86[n]['fields'] if not f[3].startswith('S:')}
    b={f[0]:f for f in X64[n]['fields'] if not f[3].startswith('S:')}
    return [(k,a[k][1],a[k][2],b[k][1],b[k][2],a[k][3]) for k in a if k in b]
LV={n:leaves(n) for n in names}
gapre=re.compile(r'(^|\.)(gap_|bytes_|pad)')
def snake(n): return re.sub(r'(?<!^)(?=[A-Z])','_',n).lower()
# hand-code field use per class
src=pathlib.Path('./src')
use=collections.defaultdict(collections.Counter); where=collections.defaultdict(set)
pat=re.compile(r'(?:jo::self<jo::(\w+)>\([^;]*?\)|jo::as_(\w+)\([^;]*?\)|jo::object<jo::(\w+)>\([^;]*?\))->(\w+)')
s2c={snake(n):n for n in names}
for f in src.rglob('*.cpp'):
    s=str(f)
    if '/original/gen/' in s or '/data/' in s: continue
    t=f.read_text(errors='replace')
    for m in pat.finditer(t):
        c=m.group(1) or s2c.get(m.group(2)) or m.group(3)
        if c: use[c][m.group(4)]+=1; where[(c,m.group(4))].add(s.split('/src/')[1])
res=[]
for v in names:
    Vl=[l for l in LV[v] if not gapre.search(l[0])]
    if not Vl or not use[v]: continue
    vx86={(l[1],l[2]):l for l in Vl}
    for r in names:
        if r==v: continue
        Rl=[l for l in LV[r] if not gapre.search(l[0])]
        rx={(l[1],l[2]):l for l in Rl}
        co=[(vx86[k],rx[k]) for k in vx86 if k in rx]
        if len(co)<3: continue
        # base relation: same x86 and x64 everywhere -> fine
        drift=[(a,b) for a,b in co if a[3]!=b[3] or a[4]!=b[4]]
        if not drift: continue
        used=[(a,b) for a,b in drift if use[v][a[0].split('.')[0]] or use[v][a[0]]]
        if not used: continue
        res.append((v,r,len(co),len(drift),used))
res.sort(key=lambda x:(-len(x[4]),-x[2]))
json.dump([(v,r,c,d,[(a[0],hex(a[1]),hex(a[3]),b[0],hex(b[3])) for a,b in u]) for v,r,c,d,u in res],open('pairs.json','w'),indent=0)
print(len(res))
for v,r,c,d,u in res[:60]:
    print(f"{v:22} vs {r:22} co={c:3} drift={d:3} used={len(u)}: "+', '.join(f"{a[0]}@{a[1]:#x} x64 {a[3]:#x}/{b[3]:#x}" for a,b in u[:3]))
