# Counts the diagnostics that are new on x86_64 (by location) in a census.sh output directory (docs/X64.md).
import os,re,collections,sys
S=sys.argv[1]
pat=re.compile(r'^(\S+?):(\d+):\d+: (error|warning): (.*)$')
def load(a,f):
    out=collections.Counter()
    for l in open(f'{S}/{a}/{f}',errors='replace'):
        m=pat.match(l.rstrip())
        if m: out[(m.group(1),m.group(2),m.group(3),m.group(4))]+=1
    return out
cat=collections.Counter(); catfiles=collections.defaultdict(set); perfile=collections.Counter(); samples={}
locs=collections.defaultdict(set)
for f in sorted(os.listdir(f'{S}/x64')):
    if not f.endswith('.log'): continue
    a=load('x86',f); b=load('x64',f)
    for k,v in b.items():
        if a.get(k,0)>=v: continue
        msg=re.sub(r"'[^']*'","X",k[3])
        key=(k[2],msg)
        if (k[0],k[1],key) in locs[key]: continue
        locs[key].add((k[0],k[1],key))
        cat[key]+=1; catfiles[key].add(k[0]); perfile[k[0]]+=1; samples.setdefault(key,f'{k[0]}:{k[1]}: {k[3]}')
tot=sum(cat.values())
print('unique new diagnostics (by location):',tot, 'errors:',sum(v for k,v in cat.items() if k[0]=='error'),'warnings:',sum(v for k,v in cat.items() if k[0]=='warning'))
for k,v in cat.most_common(45): print(f'{v:6} {len(catfiles[k]):4}f {k[0][:1]} {k[1][:110]}\n           e.g. {samples[k][:170]}')
print('\nfiles with most:')
for f,v in perfile.most_common(25): print(v,f)
