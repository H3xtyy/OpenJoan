# gen3.py MAP.csv SPEC.py
# Typed facades from an explicit function -> (class, method) map (fn_name,class,method,...), for functions whose
# fn:: name does not carry their real class (R13: placeholder prefixes Obj/X/Pair/...).
# Appends to existing gen/<snake>_methods.inc, adds an .inc to a hand-written struct, or creates gen/<snake>.hpp.
# Writes a mod2 spec (M keyed by fn name, RET_WIDTH keyed by fn name).
import csv, json, sys, re, os, glob, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sigs
import sigs2
S = sigs.S
ROOT = './src/'
FLOATS = json.load(open(S + 'r9/floats.json'))
KIND = {'std::int32_t': 'i32', 'std::uint32_t': 'u32', 'std::int16_t': 'i16', 'std::uint16_t': 'u16', 'void*': 'mptr',
        'const char*': 'str', 'float': 'f32', 'double': 'f64', 'std::uint8_t': 'u8', 'std::int8_t': 'i8', 'char': 'char', 'bool': 'b', 'struct': 'val'}
W = {'bool': 1, 'std::uint8_t': 1, 'std::int8_t': 1, 'char': 1, 'std::uint16_t': 2, 'std::int16_t': 2}
KW = set('alignas alignof and asm auto bool break case catch char class const constexpr continue default delete do double else enum explicit export extern false float for friend goto if inline int long mutable namespace new noexcept not nullptr operator or private protected public register return short signed sizeof static struct switch template this throw true try typedef typeid typename union unsigned using virtual void volatile while xor'.split())


def snake(c):
    return re.sub(r'(?<=[a-z0-9])([A-Z])', r'_\1', c).lower().replace('3_d', '3d').replace('2_d', '2d')


def find_struct(c):
    for h in glob.glob(ROOT + 'original/*.hpp') + glob.glob(ROOT + 'original/gen/*.hpp'):
        s = open(h).read()
        m = re.search(r'^struct ' + c + r'(?: : [\w:]+)? \{', s, re.M)
        if m:
            i = s.index('{', m.start()); d = 0; k = i
            while True:
                d += {'{': 1, '}': -1}.get(s[k], 0); k += 1
                if d == 0: break
            return h, m.start(), k - 1, s
    return None


mapfile, specfile = sys.argv[1], sys.argv[2]
rows = list(csv.DictReader(open(mapfile)))
direct = open(ROOT + 'original/direct.hpp').read()
by_cls = collections.defaultdict(list)
for r in rows:
    if r['class'] in ('-', '?', ''): continue
    by_cls[r['class']].append(r)

M = {}; RW = {}; stats = collections.Counter(); fails = []
new_classes = []
for cls, rs in sorted(by_cls.items()):
    sn = snake(cls); acc = 'as_' + sn
    inc_path = ROOT + 'original/gen/%s_methods.inc' % sn
    fs = find_struct(cls)
    existing = set(re.findall(r'\b\w+\b', fs[3][fs[1]:fs[2]])) if fs else set()
    if os.path.exists(inc_path): existing |= set(re.findall(r'\b\w+\b', open(inc_path).read()))
    already = set(re.findall(r'::joan::fn::(\w+)', open(inc_path).read())) if os.path.exists(inc_path) else set()
    lines = []
    for r in rs:
        name = r['fn_name']
        if name in already or name in M: stats['dup'] += 1; continue
        s, why = sigs2.sig(name)
        if r.get('sig'):  # explicit 'ret|p0;p1;...' (functions whose replacement is naked asm or has no parsable definition)
            ret_o, _, ps_o = r['sig'].partition('|')
            real_o, regname_o = sigs.fnreal[name]
            s, why = (ret_o, [p for p in ps_o.split(';') if p], regname_o, real_o), 'ok'
        if not s: stats['sig:' + why.split(':')[0]] += 1; fails.append((name, why)); continue
        ret, ps, regname, real = s
        ps = list(ps)
        for i in FLOATS.get(name, []): ps[i] = 'float'
        meth = re.sub(r'\W', '_', r['method'].strip()) or 'call'
        if meth[0].isdigit(): meth = 'm_' + meth
        if meth in KW: meth += '_'
        if meth in existing or meth == cls: meth = '%s_%x' % (meth, real & 0xffffff)
        if meth in existing: stats['clash'] += 1; fails.append((name, 'clash')); continue
        existing.add(meth)
        args = ', '.join('%s a%d' % ('T%d' % i if p == 'struct' else p, i) for i, p in enumerate(ps))
        tparams = [i for i, p in enumerate(ps) if p == 'struct']
        call = ''.join(', a%d' % i for i in range(len(ps)))
        body = '::joan::call::thiscall<%s>(::joan::fn::%s, this%s)' % (ret, name, call)
        lines.append('/// 0x%08x %s' % (real, regname))
        if tparams:
            # structs passed by value: the caller's own type, pushed as the slot call pushed it
            lines.append('template <%s>' % ', '.join('class T%d' % i for i in tparams))
            chk = ' '.join('static_assert(!std::is_pointer_v<T%d> && std::is_trivially_copyable_v<T%d>, "a struct by value");' % (i, i) for i in tparams)
            lines.append('%s %s(%s) { %s %s%s; }' % (ret, meth, args, chk, '' if ret == 'void' else 'return ', body))
        else:
            lines.append('%s %s(%s) { %s%s; }' % (ret, meth, args, '' if ret == 'void' else 'return ', body))
        M[name] = (acc, meth, [KIND[p] for p in ps], ret != 'void')
        if ret in W: RW[name] = W[ret]
        stats['ok'] += 1
    if not lines: continue
    if os.path.exists(inc_path):
        open(inc_path, 'a').write('\n'.join(lines) + '\n')
        continue
    hdr = ['// Generated (re/facades/gen3.py): typed members for the %s functions game code calls through their slots.' % cls,
           "// Each forwards to the function's slot (the registered replacement).", '']
    open(inc_path, 'w').write('\n'.join(hdr + lines) + '\n')
    if fs:
        h, b, e, s = fs
        inc = '#include "%s_methods.inc"' % sn if '/gen/' in h else '#include "gen/%s_methods.inc"' % sn
        s = s[:e] + '\n    // The other %s functions the game code calls (typed, through their slots).\n%s\n' % (cls, inc) + s[e:]
        if '/gen/' not in h and '"../call.hpp"' not in s:
            i = s.index('#include'); s = s[:i] + '#include "../call.hpp"\n#include "../fn_names.hpp"\n' + s[i:]
        open(h, 'w').write(s)
        if not re.search(r'\b' + acc + r'\(', direct) and '/gen/' not in h:
            fails.append((cls, 'no accessor ' + acc))
    else:
        open(ROOT + 'original/gen/%s.hpp' % sn, 'w').write('''// Generated (re/facades/gen3.py): the game's %s class as seen by the code that calls it (layout opaque).
#pragma once
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct %s {
#include "%s_methods.inc"
};

}  // namespace joan::orig
''' % (cls, cls, sn))
        new_classes.append(cls)

# classes.hpp / accessors.hpp from all gen headers
incs = []; acc_lines = []
for g in sorted(glob.glob(ROOT + 'original/gen/*.hpp')):
    if g.endswith(('classes.hpp', 'accessors.hpp')): continue
    c = re.search(r'^struct (\w+) \{', open(g).read(), re.M).group(1)
    incs.append('#include "%s"' % os.path.basename(g))
    acc_lines.append('template <class P>\ninline %s* as_%s(P p) noexcept { return object<%s>(p); }' % (c, snake(c), c))
open(ROOT + 'original/gen/classes.hpp', 'w').write('// Generated (re/facades/gen2.py, gen3.py): the classes that only have generated slot facades.\n#pragma once\n' + '\n'.join(incs) + '\n')
open(ROOT + 'original/gen/accessors.hpp', 'w').write('// Generated (re/facades/gen2.py, gen3.py): as_<class>(p) for the generated classes (included at the end of direct.hpp).\n#pragma once\n\nnamespace joan::orig {\n\n' + '\n'.join(acc_lines) + '\n\n}  // namespace joan::orig\n')
open(specfile, 'w').write('M=%r\nINCLUDE=\'"../original/direct.hpp"\'\nSKIP_FILES=[]\nDEFAULTS={}\nNARROW=()\nRET_WIDTH=%r\n' % (M, RW))
print(stats); print('new classes', len(new_classes))
open(specfile + '_fails.txt', 'w').write('\n'.join('%s %s' % f for f in fails) + '\n')
