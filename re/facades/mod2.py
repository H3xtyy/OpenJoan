import re,glob,collections,sys
ROOT='./'
import importlib,os
spec=importlib.import_module(os.environ['SPEC'])
M=spec.M
INCLUDE=spec.INCLUDE
NS='joan::orig::'
LIT=re.compile(r'^(0x[0-9a-fA-F]+|[0-9]+)[uU]?$')
def conv(kind,a):
    if kind=='i32':
        m=LIT.match(a)
        if m: return m.group(1)
        if re.match(r'^(i32|std::int32_t)\{[^{}]*\}$',a): return a
        if re.match(r'^static_cast<(i32|std::int32_t)>\(',a) and match_paren(a,a.index('('))==len(a)-1: return a
        return f'{NS}arg::i32({a})'
    if kind=='ptr':
        if a in ('0','0u','nullptr'): return 'nullptr'
        if a.startswith('&') and not a.startswith('&&'): return a
        if re.match(r'^static_cast<(const )?void\*>\(',a) and match_paren(a,a.index('('))==len(a)-1: return a
        return f'{NS}arg::ptr({a})'
    if kind=='vptr' or kind=='mptr':
        if a.startswith('&') and not a.startswith('&&'): return a
        return f'{NS}arg::mptr({a})'
    if kind=='string':
        return f'{NS}as_string({a})'
    if kind=='blob':
        return f'{NS}as_blob({a})'
    if kind=='cmptr':
        return f'static_cast<char*>({NS}arg::mptr({a}))'
    if kind=='str':
        if re.match(r'^(str|s)\(d::\w+\)$',a) or re.match(r'^"',a): return a
        return f'{NS}arg::str({a})'
    if kind in ('u32','i16','u16'):
        m=LIT.match(a)
        if m and kind=='u32': return a if a.endswith(('u','U')) else m.group(1)+'u'
        return f'{NS}arg::{kind}({a})'
    if kind=='val' or kind=='raw': return a
    if kind=='b':
        if a in ('true','false'): return a
        if a in ('0','0u'): return 'false'
        if a in ('1','1u'): return 'true'
        return f'{NS}arg::b({a})'
    if kind=='char': return f'static_cast<char>({NS}arg::i32({a}))'
    if kind=='u8': return f'static_cast<std::uint8_t>({NS}arg::u32({a}))'
    if kind=='i8': return f'static_cast<std::int8_t>({NS}arg::i32({a}))'
    if kind=='f32':
        if re.match(r'^-?[0-9.]+f$',a): return a
        return f'{NS}arg::f32({a})'
    if kind=='f64': return f'{NS}arg::f64({a})'
    if kind=='cmp': return f'{NS}arg::cmp({a})'
    if kind=='file': return f'{NS}arg::file({a})'
def blank_comments(s):
    out=list(s); i=0; n=len(s)
    while i<n:
        c=s[i]
        if c=='"' or c=="'":
            q=c; i+=1
            while i<n and s[i]!=q:
                if s[i]=='\\': i+=1
                i+=1
            i+=1; continue
        if s.startswith('//',i):
            j=s.find('\n',i); j=n if j<0 else j
            for k in range(i,j): out[k]=' '
            i=j; continue
        if s.startswith('/*',i):
            j=s.find('*/',i)+2
            for k in range(i,j):
                if out[k]!='\n': out[k]=' '
            i=j; continue
        i+=1
    return ''.join(out)
def match_paren(s,i):
    d=0
    while True:
        c=s[i]
        if c in '([{': d+=1
        elif c in ')]}':
            d-=1
            if d==0: return i
        i+=1
def split_args(t):
    out=[];d=0;cur=''
    for c in t:
        if c in '([{': d+=1
        if c in ')]}': d-=1
        if c==',' and d==0: out.append(cur.strip()); cur=''
        else: cur+=c
    out.append(cur.strip()); return out
WIDTH={'u8':1,'i8':1,'char':1,'bool':1,'std::uint8_t':1,'std::int8_t':1,'unsigned char':1,'signed char':1,'u16':2,'i16':2,'std::uint16_t':2,'std::int16_t':2,'short':2,'unsigned short':2}
pat=re.compile(getattr(spec,'PAT',r'\bthiscall(<([^>()]*)>)?\(fn::(\w+)\s*,'))
stats=collections.Counter(); skipped=[]
files=sorted(set(glob.glob(ROOT+'src/**/*.cpp',recursive=True)))
changed=[]
for f in files:
    if any(f.endswith(x) for x in spec.SKIP_FILES): continue
    s=open(f,errors='replace').read()
    b=blank_comments(s)
    reps=[]
    for m in pat.finditer(b):
        name=m.group(3)
        if name not in M: continue
        acc,meth,kinds,rv=M[name]
        R=(m.group(2) or 'void').strip()
        op=b.index('(',m.start()+8+len(m.group(1) or ''))
        cl=match_paren(b,op)
        args=split_args(s[op+1:cl])[1:]
        if acc is None: obj=None
        else: obj,args=args[0],args[1:]
        if len(args)<len(kinds) and getattr(spec,'DEFAULTS',{}).get(meth,99)<=len(args): kinds=kinds[:len(args)]
        if kinds and kinds[-1]=='...':
            if len(args)<len(kinds)-1:
                skipped.append((f,name,'argc',s[m.start():cl+1])); continue
            kinds=kinds[:-1]+['raw']*(len(args)-len(kinds)+1)
        if len(args)!=len(kinds):
            skipped.append((f,name,'argc',s[m.start():cl+1])); continue
        before=b[:m.start()].rstrip(); after=b[cl+1:].lstrip()
        line_start=before.rfind('\n')+1
        colon_ok = before[-1:]==':' and '?' not in before[line_start:]
        stmt = after[:1]==';' and (before[-1:] in ';{}' or colon_ok or before.endswith('else') or (before.endswith(')') and not re.search(r'\b(return)\s*$',before)))
        # a ')' before could be a cast or a call argument list end: treat as statement only for if/for/while heads
        if stmt and before.endswith(')'):
            # find matching '(' backwards
            d=0;k=len(before)-1
            while k>=0:
                if before[k]==')': d+=1
                elif before[k]=='(':
                    d-=1
                    if d==0: break
                k-=1
            head=before[:k].rstrip()
            stmt=bool(re.search(r'\b(if|for|while)$',head))
        if acc is None: call=f'{meth}(' + ', '.join(conv(k,a) for k,a in zip(kinds,args)) + ')'
        else: call=f'{NS}{acc}({obj})->{meth}(' + ', '.join(conv(k,a) for k,a in zip(kinds,args)) + ')'
        if stmt or R=='void':
            new=call
        elif rv and (name if name in getattr(spec,'RET_WIDTH',{}) else meth) in getattr(spec,'RET_WIDTH',{}) and WIDTH.get(R,4) > spec.RET_WIDTH[name if name in spec.RET_WIDTH else meth]:
            skipped.append((f,name,'narrow-result-as-'+R,s[max(0,m.start()-60):cl+3].replace('\n',' '))); continue
        elif rv and meth in getattr(spec,'NARROW',()) and R not in ('i16','u16','std::int16_t','std::uint16_t','short'):
            skipped.append((f,name,'narrow-result-as-'+R,s[max(0,m.start()-60):cl+3].replace('\n',' '))); continue
        elif rv:
            new=call if R in ('void*',) and acc and meth=='construct' else f'{NS}arg::as<{R}>({call})'
        else:
            skipped.append((f,name,'void-used-as-'+R,s[max(0,m.start()-60):cl+3].replace('\n',' '))); continue
        reps.append((m.start(),cl+1,new)); stats[name]+=1
    if reps:
        # an outer call whose arguments contain another converted call waits for the next pass
        reps=[r for r in reps if not any(o is not r and r[0] <= o[0] and o[1] <= r[1] for o in reps)]
        for a,e,new in reversed(reps): s=s[:a]+new+s[e:]
        rel=os.path.relpath(f,ROOT+'src'); depth=rel.count('/')
        inc='"'+'../'*depth+INCLUDE.strip('"').replace('../','',1)+'"'
        if inc not in s:
            i=s.find('#include "')
            s=s[:i]+'#include '+inc+'\n'+s[i:]
        open(f,'w').write(s); changed.append(f[len(ROOT):])
print(sum(stats.values()),'converted in',len(changed),'files'); print(stats)
print(len(skipped),'skipped')
open(os.environ['SPEC']+'_skipped.txt','w').write('\n'.join(f'{x[1]} {x[2]} {x[0][len(ROOT):]} | {x[3][:200]}' for x in skipped))
open(os.environ['SPEC']+'_changed.txt','w').write('\n'.join(changed))
