# gen_free.py LIST OVERRIDES.csv SPEC.py
# Typed facades for the cdecl / stdcall functions game code calls through slots (R20):
#   cdecl_call<R>(fn::X, args...)  ->  joan::orig::<ns>::<name>(args...)
# <ns> is the snake_case class part of the registered name ("Script::set_mode" -> script::set_mode, "crt::atan" ->
# libc::atan); functions without a class go straight into joan::orig. Writes src/original/gen/free_functions.hpp
# (included at the end of direct.hpp) and a mod2 spec (acc None, PAT for cdecl_call / stdcall_call).
# LIST: lines "count conv name" (conv = cdecl | stdcall). OVERRIDES: fn_name,sig ("ret|p0;p1;..." , "..." for varargs).
import csv, os, re, sys, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sigs, sigs2

ROOT = './src/'
KIND = {'std::int32_t': 'i32', 'std::uint32_t': 'u32', 'std::int16_t': 'i16', 'std::uint16_t': 'u16', 'void*': 'mptr',
        'const char*': 'str', 'float': 'f32', 'double': 'f64', 'std::uint8_t': 'u8', 'std::int8_t': 'i8', 'char': 'char',
        'bool': 'b', 'struct': 'val', '...': '...'}
W = {'bool': 1, 'std::uint8_t': 1, 'std::int8_t': 1, 'char': 1, 'std::uint16_t': 2, 'std::int16_t': 2}
KW = set('alignas alignof and asm auto bool break case catch char class const constexpr continue default delete do double else enum explicit export extern false float for friend goto if inline int long mutable namespace new noexcept not nullptr operator or private protected public register return short signed sizeof static struct switch template this throw true try typedef typeid typename union unsigned using virtual void volatile while xor free malloc'.split())
PLACEHOLDER = {'Obj', 'X', 'Object', 'Pair', 'Record', 'Ref', 'Rec', 'Holder', 'Globals', 'StaticInit', 'Init'}


def snake(c):
    return re.sub(r'(?<=[a-z0-9])([A-Z])', r'_\1', c).lower().replace('3_d', '3d').replace('2_d', '2d')


list_file, over_file, spec_file = sys.argv[1:4]
over = {r['fn_name']: r['sig'] for r in csv.DictReader(open(over_file))} if os.path.exists(over_file) else {}
used = collections.defaultdict(set)
funcs = collections.defaultdict(list)  # ns -> lines
M, RW, stats, fails = {}, {}, collections.Counter(), []
for line in open(list_file):
    n, conv, name = line.split()
    if name in M: continue
    if name not in sigs.fnreal:
        stats['sig:nofn'] += 1; fails.append((name, 'nofn')); continue
    if name in over:
        ret, _, ps = over[name].partition('|')
        ps = [p for p in ps.split(';') if p]
    else:
        s, why = sigs2.sig(name)
        if not s:
            stats['sig:' + why.split(':')[0]] += 1; fails.append((name, why)); continue
        ret, ps = s[0], list(s[1])
    if any(p not in KIND for p in ps) or (ret not in KIND and ret != 'void'):
        stats['type'] += 1; fails.append((name, 'type %s %s' % (ret, ps))); continue
    real, regname = sigs.fnreal[name]
    m = re.match(r'^(\w+)::(~?\w+)', regname)
    dll = re.match(r'^(OpenAL32|comdlg32|d3d8|dsound|dinput8?|ddraw|winmm|kernel32|user32|gdi32)\s+(\w+)$', regname or '')
    flt = re.match(r'^crt_(?:(\w+)_)?[0-9a-f]{6}$', name) if regname and regname.startswith('crt_') and ps and set(ps) == {'float'} else None
    if dll:  # an imported DLL function: <dll>::<api in snake_case> (OpenAL32 -> openal)
        ns = 'openal' if dll.group(1) == 'OpenAL32' else dll.group(1).lower()
        meth = dll.group(2)
    elif regname and re.match(r'^D3DX(?:[A-Z]\w*| [\w ]+)$', regname):  # D3DX helpers: d3dx::matrix_multiply
        ns, meth = 'd3dx', '_'.join(regname[4:].split())
    elif flt:  # the game's float wrappers of a CRT math function (float arguments, the CRT's double left in ST0)
        ns, meth = 'libc', (flt.group(1) or 'fabs') + '_f'
    elif m and m.group(1) not in PLACEHOLDER:
        ns = 'libc' if m.group(1) == 'crt' else snake(m.group(1))
        if ns in ('box', 'jpeg'): ns += '_fns'  # clash with joan::orig::box() and the jpeg namespace
        meth = m.group(2).replace('~', 'destroy_')
    else:
        ns = ''
        meth = re.sub(r'\W', '_', (m.group(2) if m else regname.split('(')[0].split()[0] if regname else name)) or name
    meth = snake(meth) if re.search('[A-Z]', meth) and not meth.isupper() else meth.lower() if meth.isupper() else meth
    if meth in KW or meth[0].isdigit(): meth = meth + '_'
    if meth in used[ns]: meth = '%s_%x' % (meth, real & 0xffffff)
    used[ns].add(meth)
    tparams = [i for i, p in enumerate(ps) if p == 'struct']
    var = ps and ps[-1] == '...'
    fixed = ps[:-1] if var else ps
    args = ', '.join(('T%d a%d' % (i, i)) if p == 'struct' else '%s a%d' % (p, i) for i, p in enumerate(fixed))
    if var: args += (', ' if args else '') + 'A... rest'
    call = ''.join(', a%d' % i for i in range(len(fixed))) + (', rest...' if var else '')
    tps = ['class T%d' % i for i in tparams] + (['class... A'] if var else [])
    body = '::joan::call::%s_call<%s>(::joan::fn::%s%s)' % (conv, ret, name, call)
    text = '/// 0x%08x %s\n' % (real, regname)
    if tps: text += 'template <%s>\n' % ', '.join(tps)
    text += 'inline %s %s(%s) { %s%s; }' % (ret, meth, args, '' if ret == 'void' else 'return ', body)
    funcs[ns].append(text)
    qual = 'joan::orig::' + (ns + '::' if ns else '') + meth
    M[name] = (None, qual, [KIND[p] for p in ps], ret != 'void')
    if ret in W: RW[name] = W[ret]
    stats['ok'] += 1

out = ['// Generated (re/facades/gen_free.py): typed free functions for the cdecl / stdcall functions game code calls',
       '// through their slots. Each forwards to the function\'s slot (the registered replacement).',
       '#pragma once', '#include <cstdint>', '', '#include "../../call.hpp"', '#include "../../fn_names.hpp"', '',
       'namespace joan::orig {', '']
for ns in sorted(funcs):
    if ns: out.append('namespace %s {' % ns)
    out += funcs[ns]
    if ns: out.append('}  // namespace %s' % ns)
    out.append('')
out.append('}  // namespace joan::orig')
open(ROOT + 'original/gen/free_functions.hpp', 'w').write('\n'.join(out) + '\n')
open(spec_file, 'w').write('M=%r\nINCLUDE=\'"../original/direct.hpp"\'\nSKIP_FILES=[]\nDEFAULTS={}\nNARROW=()\nRET_WIDTH=%r\n'
                           'PAT=%r\n' % (M, RW, r'\b(?:cdecl|stdcall)_call(<([^>()]*)>)?\(fn::(\w+)\s*[,)]'))
open(spec_file + '_fails.txt', 'w').write('\n'.join('%s %s' % f for f in fails) + '\n')
print(stats, len(funcs), 'namespaces')
