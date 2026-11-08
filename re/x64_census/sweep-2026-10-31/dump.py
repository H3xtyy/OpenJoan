# SPDX-License-Identifier: GPL-3.0-or-later
# x64 sweep 2026-10-31 (see README.md in this folder). Run from the folder holding x86.json / x64.json.
import gdb, json, sys
names=open(NAMES).read().split()
out={}
def flat(t, base, path, acc, depth=0):
    t=t.strip_typedefs()
    for f in t.fields():
        if not hasattr(f,'bitpos'): continue
        off=base+f.bitpos//8
        ft=f.type.strip_typedefs()
        nm=(path+'.' if path else '')+(f.name or '<base>')
        if f.is_base_class:
            flat(ft, off, path, acc, depth+1); continue
        if ft.code==gdb.TYPE_CODE_STRUCT and depth<6:
            acc.append((nm,off,ft.sizeof,'S:'+str(ft.name)))
            flat(ft, off, nm, acc, depth+1); continue
        kind='P' if ft.code==gdb.TYPE_CODE_PTR else ('A' if ft.code==gdb.TYPE_CODE_ARRAY else 'V')
        if kind=='A':
            el=ft.target().strip_typedefs()
            if el.code==gdb.TYPE_CODE_PTR: kind='AP'
            elif el.code==gdb.TYPE_CODE_STRUCT: kind='AS:'+str(el.name)
        acc.append((nm,off,ft.sizeof,kind))
for n in names:
    try: t=gdb.lookup_type('joan::orig::'+n)
    except Exception: continue
    try:
        acc=[]; flat(t,0,'',acc); out[n]={'size':t.sizeof,'fields':acc}
    except Exception as e: pass
json.dump(out,open(OUT,'w'))
print('classes',len(out))
