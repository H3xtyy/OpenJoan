# SPDX-License-Identifier: GPL-3.0-or-later
# x64 sweep 2026-10-31 (see README.md in this folder). Run from the folder holding x86.json / x64.json.
import json,re,collections
exec(open('pairs.py').read().split('res=[]')[0])
SPEC=json.load(open('./re/layout/layouts.json'))['classes']
def pk(k): return 'P' if k in('P','AP') else 'V'
views=[n for n in names if SPEC.get(n,{}).get('size') in (None,'',0)]
out=[]
for v in views:
    Vl=[l for l in LV[v] if not gapre.search(l[0])]
    if not Vl or not use[v]: continue
    vk={(l[1],l[2]):l for l in Vl}
    best=None
    for r in names:
        if r==v: continue
        Rl=[l for l in LV[r] if not gapre.search(l[0])]
        rx={(l[1],l[2]):l for l in Rl}
        co=[(vk[k],rx[k]) for k in vk if k in rx]
        if not co or len(co) < max(2, 0.6*len(Vl)): continue
        if any(pk(a[5])!=pk(b[5]) for a,b in co): continue
        drift=[(a,b) for a,b in co if a[3]!=b[3]]
        sc=(len(co)/len(Vl), len(co))
        if best is None or sc>best[0]: best=(sc,r,co,drift)
    if best and best[3]:
        used=[(a,b) for a,b in best[3] if use[v][a[0].split('.')[0]]]
        if used: out.append((v,best[1],len(Vl),len(best[2]),used))
out.sort(key=lambda x:-sum(use[x[0]][a[0].split('.')[0]] for a,b in x[4]))
print(len(views),'views;',len(out),'drifting')
for v,r,nv,nc,u in out:
    n=sum(use[v][a[0].split('.')[0]] for a,b in u)
    files=sorted(set().union(*[where[(v,a[0].split('.')[0])] for a,b in u]))
    print(f"{v:20} ~ {r:20} {nc}/{nv} uses={n:3} "+', '.join(f"{a[0]} {a[3]:#x}!={b[3]:#x}" for a,b in u[:4])+'  | '+' '.join(files[:4]))
