# signatures of registered functions: fn name -> (ret, [param types], method name, real addr)
import re,os,collections,json,sys
ROOT='./'
S='/tmp/openjoan-facades/'
fnreal={}
for l in open(ROOT+'src/fn_names.hpp'):
    m=re.match(r'inline constexpr Code (\w+)\{\d+\};\s*// 0x[0-9a-f]+(?: -> 0x([0-9a-f]+))?\s+(\S+)?',l)
    if m and m.group(2): fnreal[m.group(1)]=(int(m.group(2),16),m.group(3))
    m2=re.match(r'inline constexpr Code (\w+)\{\d+\};\s*// 0x([0-9a-f]+) (?:game|library)\s+(.+)$',l)
    if m2 and m2.group(1) not in fnreal: fnreal[m2.group(1)]=(int(m2.group(2),16),m2.group(3).strip())
regs={}
for l in open(S+'regs.txt'):
    f=l.rstrip('\n').split('\t')
    if f[0].startswith('0x'): regs[int(f[0],16)]=f
ALIAS={'u8':'std::uint8_t','i8':'std::int8_t','u16':'std::uint16_t','i16':'std::int16_t','u32':'std::uint32_t','i32':'std::int32_t',
 'dword':'std::uint32_t','word':'std::uint16_t','byte':'std::uint8_t','int':'std::int32_t','unsigned':'std::uint32_t','short':'std::int16_t','char':'char','float':'float','double':'double','void':'void',
 'std::uint8_t':'std::uint8_t','std::int8_t':'std::int8_t','std::uint16_t':'std::uint16_t','std::int16_t':'std::int16_t','std::uint32_t':'std::uint32_t','std::int32_t':'std::int32_t','std::uintptr_t':'std::uint32_t','bool':'bool','long':'std::int32_t','unsigned int':'std::uint32_t','unsigned short':'std::uint16_t','unsigned char':'std::uint8_t','signed char':'std::int8_t'}
def norm(t):
    t=' '.join(t.replace('&',' & ').replace('*',' * ').split())
    t=re.sub(r'\bconst\b','',t).strip(); t=' '.join(t.split())
    stars=t.count('*'); base=t.replace('*','').strip()
    if '&' in base: return None
    if stars:
        if base in ('char',): return 'const char*' if stars==1 else None
        return 'void*' if stars==1 else None
    return ALIAS.get(base)
_src={}
def src(f):
    if f not in _src: _src[f]=open(ROOT+'src/'+f).read()
    return _src[f]
def block_of(text,line):
    lines=text.split('\n'); opened=[]; spans=[]
    for i,ln in enumerate(lines,1):
        m=re.match(r'^namespace (\w+) \{',ln)
        if m: opened.append((m.group(1),i))
        m=re.match(r'^\}  // namespace (\w+)\s*$',ln)
        if m:
            for j in range(len(opened)-1,-1,-1):
                if opened[j][0]==m.group(1): spans.append((opened[j][1],i)); del opened[j]; break
    best=None
    for b,e in spans:
        if b<=line<=e and (best is None or b>best[0]): best=(b,e)
    if not best: return text
    return '\n'.join(lines[best[0]-1:best[1]])
def sig(name):
    if name not in fnreal: return None,'nofn'
    real,regname=fnreal[name]
    r=regs.get(real)
    if not r: return None,'noreg'
    fx=r[7]; m=re.match(r'&(?:H::)?(\w+)$',fx)
    if not m: return None,'expr'
    member=m.group(1)
    blk=block_of(src(r[5]),int(r[6]))
    dm=re.search(r'(?:^|\n)\s*(?:static\s+|inline\s+)?([\w:<>]+(?:\s*\*+)?(?:\s+const)?(?:\s*\*+)?)\s+(?:__\w+\s+)?'+member+r'\s*\(([^()]*)\)\s*(?:const\s*)?(?:noexcept\s*)?\{',blk)
    if not dm: return None,'nodef'
    ret=norm(dm.group(1))
    if ret is None: return None,'ret:'+dm.group(1)
    ps=[]
    if dm.group(2).strip() and dm.group(2).strip()!='void':
        for p in dm.group(2).split(','):
            p=re.sub(r'/\*.*?\*/','',p); p=re.sub(r'=.*','',p).strip()
            mm=re.match(r'^(.*[\s\*&])(\w+)$',p)
            t=mm.group(1).strip() if mm and not mm.group(1).strip().endswith('::') and mm.group(1).strip() not in ('unsigned','signed') else p
            t2=norm(t)
            if t2 is None: return None,'param:'+p
            ps.append(t2)
    return (ret,ps,regname,real),'ok'
if __name__=='__main__':
    pref=sys.argv[1]
    cnt=[l.split() for l in open('/tmp/thiscall_counts.txt')]
    st=collections.Counter(); out={}
    for n,name in cnt:
        if not name.startswith(pref+'_'): continue
        s,why=sig(name); st[why.split(':')[0]]+=int(n)
        if s: out[name]=s
        elif why!='ok': print(name,why)
    print(st)
    json.dump(out,open(S+'r9/sig_%s.json'%pref,'w'),indent=0)
