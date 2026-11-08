# fix arg::{i32,u32,i16,u16}(X) at clang "requested here" locations: pointers -> arg::as<T>(X), floats -> arg::bits(X)
import re,sys,collections
T={'i32':'std::int32_t','u32':'std::uint32_t','i16':'std::int16_t','u16':'std::uint16_t'}
locs=collections.defaultdict(list)
for l in open(sys.argv[1]):
    m=re.search(r": (\S+?):(\d+):(\d+): note: in instantiation of function template specialization 'joan::orig::arg::(i32|u32|i16|u16)<([^']*)>' requested here",l)
    if m: locs[m.group(1)].append((int(m.group(2)),int(m.group(3)),m.group(4),m.group(5)))
n=0
for f,L in locs.items():
    lines=open(f).read().split('\n')
    for ln,col,kind,ty in sorted(set(L),key=lambda x:(-x[0],-x[1])):
        s=lines[ln-1]
        # the call starts at col (1-based) pointing at the function name ('u32'); find 'joan::orig::arg::' before it
        k=col-1; start=s.rfind('joan::orig::arg::',0,k+1)
        if start<0 or not s.startswith(kind+'(',start+len('joan::orig::arg::')): print('?',f,ln,col,s[k-20:k+20]); continue
        if ty in ('float','double'):
            new='joan::orig::arg::bits(' if ty=='float' else None
            if not new: print('double',f,ln); continue
            lines[ln-1]=s[:start]+new+s[start+len('joan::orig::arg::'+kind+'('):]
        else:
            lines[ln-1]=s[:start]+'joan::orig::arg::as<%s>('%T[kind]+s[start+len('joan::orig::arg::'+kind+'('):]
        n+=1
    open(f,'w').write('\n'.join(lines))
print(n)
