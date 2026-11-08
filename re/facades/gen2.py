# gen2.py Cls[:accessor] ...   (needs sig_<Cls>.json from sigs.py)
import json,sys,re,os,glob
S='/tmp/openjoan-facades/'
ROOT='./src/'
FLOATS=json.load(open(S+'r9/floats.json'))
KIND={'std::int32_t':'i32','std::uint32_t':'u32','std::int16_t':'i16','std::uint16_t':'u16','void*':'mptr','const char*':'str',
      'float':'f32','double':'f64','std::uint8_t':'u8','std::int8_t':'i8','char':'char'}
W={'std::uint8_t':1,'std::int8_t':1,'char':1,'std::uint16_t':2,'std::int16_t':2}
KW=set('alignas alignof and asm auto bool break case catch char class const constexpr continue default delete do double else enum explicit export extern false float for friend goto if inline int long mutable namespace new noexcept not nullptr operator or private protected public register return short signed sizeof static struct switch template this throw true try typedef typeid typename union unsigned using virtual void volatile while xor'.split())
def snake(c): return re.sub(r'(?<=[a-z0-9])([A-Z])',r'_\1',c).lower().replace('3_d','3d').replace('2_d','2d')
def find_struct(c):
    for h in glob.glob(ROOT+'original/*.hpp'):
        s=open(h).read(); m=re.search(r'^struct '+c+r' \{',s,re.M)
        if m:
            i=s.index('{',m.start()); d=0; k=i
            while True:
                d+= {'{':1,'}':-1}.get(s[k],0); k+=1
                if d==0: break
            return h,m.start(),k-1,s
    return None
acc_lines=[]; cls_includes=[]
if os.path.exists(ROOT+'original/gen/accessors.hpp'):
    acc_lines=[l for l in open(ROOT+'original/gen/accessors.hpp').read().split('\n') if l.startswith('template') or l.startswith('inline')]
if os.path.exists(ROOT+'original/gen/classes.hpp'):
    cls_includes=[l for l in open(ROOT+'original/gen/classes.hpp').read().split('\n') if l.startswith('#include')]
for arg in sys.argv[1:]:
    cls,_,acc=arg.partition(':'); sn=snake(cls); acc=acc or 'as_'+sn
    sig=json.load(open(S+'r9/sig_%s.json'%cls))
    for n,idx in FLOATS.items():
        if n in sig:
            for i in idx: sig[n][1][i]='float'
    fs=find_struct(cls)
    existing=set(re.findall(r'\b\w+\b',fs[3][fs[1]:fs[2]])) if fs else set()
    lines=['// Generated (re/facades/gen2.py): typed members for the %s functions game code calls through their slots.'%cls,
           '// Each forwards to the function\'s slot (the registered replacement).','']
    M={}; RW={}
    for name,(ret,ps,regname,real) in sorted(sig.items(),key=lambda kv:kv[1][3]):
        if any(p=='bool' for p in ps) or ret=='bool': continue
        meth=name[len(cls)+1:]
        if meth==cls: meth='construct'
        if meth in KW: meth=meth+'_'
        if not re.match(r'^[A-Za-z_]\w*$',meth) or meth in existing or meth[0].isdigit(): meth='m_'+meth if meth[0].isdigit() else meth+'_slot'
        args=', '.join('%s a%d'%(p,i) for i,p in enumerate(ps)); call=''.join(', a%d'%i for i in range(len(ps)))
        body='::joan::call::thiscall<%s>(::joan::fn::%s, this%s)'%(ret,name,call)
        lines.append('/// 0x%08x %s'%(real,regname))
        lines.append('%s %s(%s) { %s%s; }'%(ret,meth,args,'' if ret=='void' else 'return ',body))
        M[name]=(acc,meth,[KIND[p] for p in ps],ret!='void')
        if ret in W: RW[meth]=W[ret]
    open(ROOT+'original/gen/%s_methods.inc'%sn,'w').write('\n'.join(lines)+'\n')
    if fs:
        h,b,e,s=fs
        inc='#include "gen/%s_methods.inc"'%sn
        if inc not in s:
            s=s[:e]+'\n    // The other %s functions the game code calls (typed, through their slots).\n%s\n'%(cls,inc)+s[e:]
            if '"../call.hpp"' not in s:
                i=s.index('#include'); s=s[:i]+'#include "../call.hpp"\n#include "../fn_names.hpp"\n'+s[i:]
            open(h,'w').write(s)
    else:
        open(ROOT+'original/gen/%s.hpp'%sn,'w').write('''// Generated (re/facades/gen2.py): the game's %s class as seen by the code that calls it (layout opaque).
#pragma once
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct %s {
#include "%s_methods.inc"
};

}  // namespace joan::orig
''' % (cls,cls,sn))
        l='#include "%s.hpp"'%sn
        if l not in cls_includes: cls_includes.append(l)
        a='template <class P>\ninline %s* %s(P p) noexcept { return object<%s>(p); }'%(cls,acc,cls)
        acc_lines.append(a)
    open(S+'r1/spec_r10_%s.py'%cls.lower(),'w').write('M=%r\nINCLUDE=\'"../original/direct.hpp"\'\nSKIP_FILES=[]\nDEFAULTS={}\nNARROW=()\nRET_WIDTH=%r\n'%(M,RW))
    print(cls,len(M),'methods', 'existing' if fs else 'new')
open(ROOT+'original/gen/classes.hpp','w').write('// Generated (re/facades/gen2.py): the classes that only have generated slot facades.\n#pragma once\n'+'\n'.join(sorted(set(cls_includes)))+'\n')
acc_lines=[]
for g in sorted(glob.glob(ROOT+'original/gen/*.hpp')):
    if g.endswith(('classes.hpp','accessors.hpp')): continue
    c=re.search(r'^struct (\w+) \{',open(g).read(),re.M).group(1)
    acc_lines.append('template <class P>\ninline %s* as_%s(P p) noexcept { return object<%s>(p); }'%(c,snake(c),c))
open(ROOT+'original/gen/accessors.hpp','w').write('// Generated (re/facades/gen2.py): as_<class>(p) for the generated classes (included at the end of direct.hpp).\n#pragma once\n\nnamespace joan::orig {\n\n'+'\n'.join(dict.fromkeys(acc_lines))+'\n\n}  // namespace joan::orig\n')
