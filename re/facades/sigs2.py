# sigs2.py: sigs.sig() plus the registrations sigs.py gives up on (R14):
#  - `static_cast<R (C::*)(P...)>(&C::m)`: the signature is the cast's type,
#  - `&A::B::m` (a qualified member, not H::): the member's definition / declaration in the registration's file, then
#    in the struct's header (src/original/*.hpp) or an out-of-line `R C::m(P...)` anywhere in src/,
#  - `bool` parameters / results are reported as 'bool' (gen3 types them; call sites use arg::b),
#  - reference parameters `T&` are passed as pointers (the slot call passed the address; same ABI): 'void*'.
import re, glob, os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sigs

ROOT = sigs.ROOT
_hdr = None


def headers():
    global _hdr
    if _hdr is None:
        _hdr = {f: open(f, errors='replace').read() for f in glob.glob(ROOT + 'src/**/*.hpp', recursive=True)
                if '/original/gen/' not in f}
    return _hdr


def norm(t):
    t = ' '.join(t.split())
    if t.endswith('&') and not t.endswith('&&'):
        base = t[:-1].strip()
        return 'void*' if sigs.norm(base) is not None or re.match(r'^(const\s+)?[\w:]+$', base) else None
    if re.match(r'^(const\s+)?[\w:]+(\s*\*\s*(const\s*)?){2,}$', t): return 'void*'  # T** etc.: a pointer
    return sigs.norm(t)


def parse_params(ptext):
    ps = []
    ptext = re.sub(r'/\*.*?\*/', '', ptext).strip()
    if not ptext or ptext == 'void': return ps
    for p in ptext.split(','):
        p = re.sub(r'=.*', '', p).strip()
        if p == '...':
            ps.append('...'); continue
        mu = re.match(r'^(unsigned|signed)(\s+long|\s+int)?\s+(\w+)$', p)
        if mu and mu.group(3) not in ('int', 'char', 'short', 'long'):
            ps.append('std::uint32_t' if mu.group(1) == 'unsigned' else 'std::int32_t'); continue
        mm = re.match(r'^(.*[\s\*&])(\w+)$', p)
        t = mm.group(1).strip() if mm and not mm.group(1).strip().endswith('::') and mm.group(1).strip() not in ('unsigned', 'signed') and mm.group(2) not in ('int', 'char', 'short', 'long', 'float', 'double', 'bool') else p
        t2 = norm(t)
        if t2 is None and re.match(r'^(const\s+)?[A-Za-z_][\w:]*$', t) and t.split()[-1] not in ('int', 'long', 'short', 'unsigned', 'signed'):
            t2 = 'struct'  # a struct by value: the facade takes the caller's type as is (template parameter)
        if t2 is None: return None
        ps.append(t2)
    return ps


RET = r'((?:const\s+)?[\w:<>]+(?:\s*\*+)?(?:\s+const)?(?:\s*\*+)?)'


def find_def_before(text, cls, member, line):
    # the last definition of member starting before the registration line (several local H structs per file)
    lim = sum(len(l) + 1 for l in text.split('\n')[:max(line - 1, 0)])
    best = None
    pat = re.compile(r'(?:^|[\n;{}])\s*(?:static\s+|inline\s+|virtual\s+)*' + RET + r'\s+(?:__\w+\s+)?(?:' + re.escape(cls) + r'::)?' +
                     re.escape(member) + r'\s*\(([^()]*)\)\s*(?:const\s*)?(?:noexcept\s*)?(?:override\s*)?[{;]')
    for m in pat.finditer(text):
        if m.start() > lim: break
        if m.group(1) in ('return', 'else', 'new', 'delete', 'case'): continue
        best = (m.group(1), m.group(2))
    return best


def find_def(text, cls, member):
    pat = re.compile(r'(?:^|[\n;{}])\s*(?:static\s+|inline\s+|virtual\s+)*' + RET + r'\s+(?:__\w+\s+)?(?:' + re.escape(cls) + r'::)?' +
                     re.escape(member) + r'\s*\(([^()]*)\)\s*(?:const\s*)?(?:noexcept\s*)?(?:override\s*)?[{;]')
    for m in pat.finditer(text):
        if m.group(1) in ('return', 'else', 'new', 'delete', 'case'): continue
        return m.group(1), m.group(2)
    return None


def sig(name):
    s, why = sigs.sig(name)
    if why == 'nofn' or why == 'noreg': return s, why
    real, regname = sigs.fnreal[name]
    r = sigs.regs.get(real)
    fx = r[7]
    mh = re.match(r'^&(?:H::)?(\w+)$', fx)
    if mh:
        # sigs.sig takes the first definition in the registration's namespace block; files without block namespaces
        # hold several local H structs with the same member names: the right one is the last before the registration
        d = find_def_before(sigs.src(r[5]), 'H', mh.group(1), int(r[6]))
        if d:
            ret = norm(d[0]); ps = parse_params(d[1])
            if ret is None: return None, 'ret:' + d[0]
            if ps is None: return None, 'param2:' + d[1][:60]
            return (ret, ps, regname, real), 'ok'
    if s or why not in ('expr', 'nodef') and not why.startswith('param'):
        return s, why
    m = re.match(r'^static_cast<\s*' + RET + r'\s*\(\s*([\w:]+)::\*\s*\)\s*\(([^()]*)\)\s*(?:const\s*)?>\(&[\w:]+\)$', fx)
    if m:
        ret = norm(m.group(1)); ps = parse_params(m.group(3))
        if ret is None or ps is None: return None, 'cast-types'
        return (ret, ps, regname, real), 'ok'
    m = re.match(r'^&((?:\w+::)*)(\w+)::(\w+)$', fx)
    if not m: return None, why
    cls, member = m.group(2), m.group(3)
    d = find_def_before(sigs.src(r[5]), cls, member, int(r[6]))
    texts = [] if d else [sigs.src(r[5])]
    texts += [] if d else [t for f, t in headers().items() if re.search(r'\bstruct ' + cls + r'\b', t)]
    for t in ([] if d else texts):
        d = find_def(t, cls, member)
        if d: break
    if not d:
        for f in glob.glob(ROOT + 'src/**/*.cpp', recursive=True):
            t = sigs.src(f[len(ROOT) + 4:])
            if cls + '::' + member in t:
                d = find_def(t, cls, member)
                if d: break
    if not d: return None, 'nodef2'
    ret = norm(d[0]); ps = parse_params(d[1])
    if ret is None: return None, 'ret:' + d[0]
    if ps is None: return None, 'param2:' + d[1][:60]
    return (ret, ps, regname, real), 'ok'
