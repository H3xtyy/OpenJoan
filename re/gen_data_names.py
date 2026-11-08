#!/usr/bin/env python3
"""Generate src/data_names.hpp: a name for every joan.exe data address our C++ uses (namespace joan::d, alias d).

Usage (from the repository root):
  gen_data_names.py <joan.exe> <hook dir> <out data_names.hpp>

Covered: every address of joan.exe's data image (.rdata 0x9cf000.., .data 0x9f4000.., .bss ..0xa6cd44) and every IAT
slot (src/runtime/import_table.inc) that src/** names - as a literal (comments, string literals, src/data/ and the other
generated tables excluded) or as a name of an existing data_names.hpp (so regenerating after re/rewrite_data_names.py
keeps them). The data classifier (re/data_classify.py) makes every address our C++ names the start of an object, so
each such address is a live object of src/data/data_symbols.inc and gets its own name (no `Name + offset` forms are
generated; code may still write `d::Name + 0x10`, which the scanners read as `0x... + 0x10`, like the literal form).

Names (the value is always exactly the address the code used), in this order:
- IAT slots: iat_<dll>_<function> (`iat_KERNEL32_CreateFileA`; C++-decorated v3d_87 exports undecorated to
  [Class_]function, overloads `_<parameter count>`: iat_v3d_87_vector_transform_2 / _3; ordinal imports `ord<n>`).
- char arrays (.rdata / .data strings): s_<text> (the string's words, 28 characters at most; punctuation-only strings
  spell their characters: s_dot_dot_dot, s_dash40).
- .rdata float / double constants: flt_<value> / dbl_<value> (`flt_0_5`, `flt_m1`, `dbl_6_283185307179586`; shortest
  round-trip decimal, `m` = minus).
- the name our C++ gives the object - accessor functions `T name() { return global<T>(0x...) }` (3 votes) and
  `kName = 0x...` / `kName = obj(0x...)` / `kName = reinterpret_cast<T*>(0x...)` (1 vote) - when one spelling group
  (kScreenW = screen_w = g_screen_w) holds a strict majority of the votes: .rdata keeps a kName spelling, .data /
  .bss become g_<name> (`kUnits` -> `g_units`). Disagreeing names (kFx / kSpeech for one address) give no name.
- vtables / function pointer tables: vt_<Class> / fns_<Class> (data_symbols.inc).
- .data / .bss globals: a name evident from comments of src/** (the only data address on a line whose whole
  comment is `g_name`, a CRT variable `_name` / `__name` that is not a function, or a class the registry knows
  (`Class::...`)), agreed by all such comments; else g_<address>. Other .rdata objects: k_<address>; other classes
  keep their generic data_symbols.inc name (blob_, guid_, strs_, ptrs_, ld_ ... with the address).
Duplicates (same text at several addresses, or names that clash): every holder gets `_<address>` appended.
Each line documents the section, the C type and the data_symbols.inc name (or DLL and decorated import) in a comment.
"""
import collections
import decimal
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fn_scan as F  # noqa: E402
from pe import PE  # noqa: E402
from gen_fn_names import KEYWORDS  # noqa: E402

IMAGE = (0x9cf000, 0xa6cd44)
SYM_RE = re.compile(r'^JOAN_DATA_SYMBOL\((\w+), ([^,]+), (\w+), 0x([0-9a-f]+)u\)$', re.M)
# by name ("Func") or by ordinal (nullptr, e.g. DSOUND.dll #1)
IMP_RE = re.compile(r'^\s*\{0x([0-9a-f]{8})u, (\d+), (\d+), (?:"((?:\\.|[^"\\])*)"|nullptr)\},', re.M)
DLL_RE = re.compile(r'kImportDlls\[\]\s*=\s*\{(.*?)\};', re.S)
HEX_RE = re.compile(r'\b0x([0-9a-fA-F]{6,8})(?:[uUlL]{0,3})\b')
# the hook's own names of an address: accessor functions `T name() { return global<T>(0x...)` and constants
# `kName = 0x...` / `kName = obj(0x...)` / `kName = reinterpret_cast<T*>(0x...)` (also g_name / s_name)
ACCESSOR_RE = re.compile(r'(\w+)\s*\(\s*\)\s*(?:const\s*)?\{\s*return\s+(?:[\w:]*::)?global\s*<[^;{}]*?>\s*\(\s*'
                         r'0x([0-9a-fA-F]+)')
KNAME_RE = re.compile(r'\b(k[A-Z]\w*|g_\w+|s_\w+)\s*(?:=|\{)\s*(?:(?:obj|ptr|reinterpret_cast\s*<[^;<>]*>)\s*\(\s*)?'
                      r'0x([0-9a-fA-F]+)')
GENERIC = {'k', 'g', 'b', 'flt', 'dbl', 'ld', 'mmx', 'guid', 'blob', 'pad', 'strs', 'ptrs', 'xc_table',
           'type_descriptor', 'funcinfo', 'unwindmap', 'tryblockmap', 'handlertype', 'catchable_type',
           'catchable_type_array', 'throwinfo', 'vt_anon', 'fns_anon'}
# identifiers the Windows / CRT headers define as macros (a name equal to one would be macro-expanded)
MACROS = {'s_addr', 's_host', 's_net', 's_imp', 's_impno', 's_lh', 's_un', 'g_nul'}


def cname(s):
    s = re.sub(r'[^A-Za-z0-9_]+', '_', s)
    return re.sub(r'__+', '_', s).strip('_')


def short_float(x, bits):
    """shortest decimal that round-trips x at float (32) / double (64) precision"""
    if x != x:
        return 'nan'
    if x in (float('inf'), float('-inf')):
        return 'inf' if x > 0 else '-inf'
    if bits == 64:
        s = repr(x)
    else:
        for p in range(1, 10):
            s = '%.*g' % (p, x)
            if struct.unpack('<f', struct.pack('<f', float(s)))[0] == x:
                break
    if 'e' in s and 1e-6 <= abs(x) < 1e10:
        s = format(decimal.Decimal(s), 'f')
    if '.' in s and 'e' not in s:
        s = s.rstrip('0').rstrip('.')
    return s


PUNCT = {' ': 'sp', '.': 'dot', ',': 'comma', ';': 'semi', ':': 'colon', '_': 'us', '-': 'dash', '+': 'plus',
         '*': 'star', '/': 'slash', '\\': 'bslash', '|': 'bar', '#': 'hash', '"': 'quote', '!': 'excl', '>': 'gt',
         '<': 'lt', '(': 'lparen', ')': 'rparen', '[': 'lbrack', ']': 'rbrack', '{': 'lbrace', '}': 'rbrace',
         '%': 'pct', '=': 'eq', '?': 'q', '\n': 'nl', '\r': 'cr', '\t': 'tab'}


def string_name(text):
    """identifier part of a string's name: its words (28 characters at most); a string of punctuation only spells
    its characters (`...` -> dot_dot_dot), runs of one character counted (`dash40`)"""
    n = cname(text)[:28].rstrip('_')
    if n or not text:
        return n or 'empty'
    parts = []
    for m in re.finditer(r'(.)\1*', text):
        k = len(m.group(0))
        w = PUNCT.get(m.group(1), 'x%02x' % ord(m.group(1)))
        parts.extend([w] * k if k <= 3 else ['%s%d' % (w, k)])
    return '_'.join(parts)[:28].rstrip('_')


def value_name(prefix, s):
    s = s.replace('-', 'm').replace('+', '').replace('.', '_')
    return '%s_%s' % (prefix, s)


def load_symbols(hook_dir):
    """address -> (section, ctype, data_symbols name)"""
    src = open(os.path.join(hook_dir, 'data', 'data_symbols.inc')).read()
    return {int(m.group(4), 16): (m.group(1), m.group(2).strip(), m.group(3)) for m in SYM_RE.finditer(src)}


def load_imports(hook_dir):
    """slot address -> (dll, ordinal, function name)"""
    src = open(os.path.join(hook_dir, 'import_table.inc')).read()
    dlls = re.findall(r'"([^"]+)"', DLL_RE.search(src).group(1))
    return {int(m.group(1), 16): (dlls[int(m.group(2))], int(m.group(3)), (m.group(4) or '').replace('\\?', '?'))
            for m in IMP_RE.finditer(src)}


def hook_sources(hook_dir):
    for rel, path in F.hook_files(hook_dir):
        if rel.startswith('data/') or F.SKIP_REL.match(rel) or os.path.basename(rel) == 'image_layout.inc':
            continue
        yield rel, path


def is_data(v, imports):
    return IMAGE[0] <= v < IMAGE[1] or v in imports


def scan_source(src, imports):
    """[(start, end, value, kind)] for every data address literal of src (comments / string literals excluded); kind:
       'reg'   a key argument of a registration macro (re/fn_scan.py macro_keys: JOAN_REPLACE* address / IAT slot,
               module macro arguments that are stringized, pasted, used in __asm - like S(name, stub, IAT slot) in
               lib/imports/import_stubs.cpp - or passed on as such a key); all arguments of a macro defined elsewhere that looks
               like a registration X("name", 0x..., ...)
       'pp_if' on an #if / #elif line
       'asm'   inside an __asm block (an MSVC inline-asm operand: a C++ constant cannot stand there)
       'code'  anything else: replaced by its name"""
    code = F.strip(src)
    reg = F.key_arg_mask(code)
    keys = F.macro_keys(code)
    for m in F.REG_RE.finditer(F.strip(src, strings=False)):
        if m.group(1) in keys:
            continue                       # a macro of this file / the registry: only its key arguments (above)
        i = code.index('(', m.start())
        depth, j = 0, i
        while j < len(code):
            depth += {'(': 1, ')': -1}.get(code[j], 0)
            if depth == 0:
                break
            j += 1
        j = min(j, len(code) - 1)
        reg[i:j + 1] = b'\1' * (j + 1 - i)
    amask = F.asm_mask(code)
    out = []
    for m in HEX_RE.finditer(code):
        v = int(m.group(1), 16)
        if not is_data(v, imports):
            continue
        s = m.start()
        if reg[s]:
            kind = 'reg'
        elif F.PP_IF.match(code[code.rfind('\n', 0, s) + 1:s]):
            kind = 'pp_if'
        elif amask[s]:
            kind = 'asm'
        else:
            kind = 'code'
        out.append((s, m.end(), v, kind))
    return out


def used_addresses(hook_dir, existing, imports):
    """data addresses src/** C++ names (literals outside __asm / registration arguments / #if, or names of an
    existing data_names.hpp, so regenerating after the rewrite keeps them), and per address the comments of lines
    that name only it"""
    used = set()
    comments = collections.defaultdict(collections.Counter)
    votes = collections.defaultdict(collections.Counter)
    called = set()
    for rel, path in hook_sources(hook_dir):
        src = F.expand_data_names(open(path, errors='replace', newline='').read(), existing)
        code = F.strip(src)
        nocomment = F.strip(src, strings=False)
        called.update(re.findall(r'\b([A-Za-z_]\w*)\s*\(', code))
        for m in ACCESSOR_RE.finditer(nocomment):
            votes[int(m.group(2), 16)][m.group(1)] += 3
        for m in KNAME_RE.finditer(nocomment):
            votes[int(m.group(2), 16)][m.group(1)] += 1
        for s, e, v, kind in scan_source(src, imports):
            if kind == 'code':
                used.add(v)
        for cl, rl in zip(code.split('\n'), src.split('\n')):
            i = rl.find('//')
            if i < 0 or cl[i:].strip():
                continue                       # no comment, or the `//` is inside a string literal
            vs = {int(m.group(1), 16) for m in HEX_RE.finditer(cl)}
            vs = {v for v in vs if IMAGE[0] <= v < IMAGE[1]}
            if len(vs) == 1:
                comments[vs.pop()][rl[i + 2:].strip()] += 1
    return used, comments, called, votes


def norm(n):
    """spelling-independent key of a hook name: kScreenW / screen_w / g_screen_w -> screenw"""
    n = re.sub(r'^(?:k(?=[A-Z])|g_|s_)', '', n)
    return n.replace('_', '').lower()


def hook_name(votes):
    """the hook's name for an object: the spelling group (norm) holding a strict majority of the votes (accessor
    functions count 3, `kName = 0x...` / `kName = obj(0x...)` ... 1), its most used spelling; None when the hook's
    names disagree"""
    groups = collections.defaultdict(collections.Counter)
    for n, w in votes.items():
        groups[norm(n)][n] += w
    total = sum(votes.values())
    best = max(groups.values(), key=lambda g: sum(g.values()), default=None)
    if not best or 2 * sum(best.values()) <= total:
        return None
    return best


def styled(group, sec):
    """a hook name in the style of its section: .rdata kName, .data / .bss g_name"""
    ranked = sorted(group.items(), key=lambda kv: (-kv[1], kv[0]))
    if sec == 'rdata':
        k = [n for n, w in ranked if re.match(r'^k[A-Z]', n)]
        return k[0] if k else ranked[0][0]
    g = [n for n, w in ranked if not re.match(r'^k[A-Z]', n)]
    n = g[0] if g else ranked[0][0]
    if re.match(r'^k[A-Z]', n):
        n = n[1].lower() + n[2:]
    return n if n.startswith('g_') else 'g_' + n


def comment_name(cms, classes, functions):
    """a global's name from the comments of the lines that name only it (None unless all candidates agree); a CRT-style
    `_name` that is a function (registered, imported or called in src/**) is the comment of a call, not a name"""
    cands = set()
    for c in cms:
        c = c.rstrip('*').strip()
        if re.match(r'^g_[A-Za-z]\w*$', c):
            cands.add(c)
        elif re.match(r'^_{1,2}[a-z]\w*$', c) and c not in functions and c.lstrip('_') not in functions:
            cands.add('g_' + c.lstrip('_'))
        elif re.match(r'^[A-Z]\w*$', c) and c in classes:
            cands.add('g_' + c)
    return cands.pop() if len(cands) == 1 else None


def base_name(a, sec, ctype, sym, pe, comments, votes, classes, functions):
    """(base name, origin) for a data_symbols object"""
    m = re.match(r'^(\w*?)_([0-9a-f]{6})$', sym)
    base = m.group(1) if m and int(m.group(2), 16) == a else sym
    if re.match(r'^char\[\d+\]$', ctype) and sec in ('rdata', 'data'):
        o = pe.va2off(a)
        text = pe.data[o:pe.data.index(b'\0', o)].decode('latin1')
        return 's_' + string_name(text), 'text'
    if sec == 'rdata' and ctype == 'float':
        x = struct.unpack('<f', pe.data[pe.va2off(a):pe.va2off(a) + 4])[0]
        return value_name('flt', short_float(x, 32)), 'value'
    if sec == 'rdata' and ctype == 'double':
        x = struct.unpack('<d', pe.data[pe.va2off(a):pe.va2off(a) + 8])[0]
        return value_name('dbl', short_float(x, 64)), 'value'
    group = hook_name(votes.get(a, {}))
    if group:
        return styled(group, sec), 'hook'
    if base.startswith(('vt_', 'fns_')) and base not in GENERIC:
        return base, 'class'
    if sec in ('data', 'bss') and base in ('g', 'b', 'flt', 'dbl', 'ld', 'mmx'):
        c = comment_name(comments.get(a, {}), classes, functions)
        if c:
            return c, 'comment'
        return 'g_%06x' % a, 'address'
    if base not in GENERIC:                # a hook name without majority
        return ('k_%06x' if sec == 'rdata' else 'g_%06x') % a, 'address'
    if base in ('k', 'g', 'b'):
        return ('k_%06x' if sec == 'rdata' else 'g_%06x') % a, 'address'
    return sym, 'address'


SCALAR_TYPES = {'u8': 'std::uint8_t', 'i8': 'std::int8_t', 'u16': 'std::uint16_t', 'i16': 'std::int16_t',
                'u32': 'std::uint32_t', 'i32': 'std::int32_t', 'u64': 'std::uint64_t', 'uptr': 'std::uintptr_t',
                'Guid': 'std::uint8_t[16]'}   # data_common.hpp's Guid (16 bytes); the header stays free of src/data


def sym_type(a, syms):
    """T of img::Sym<T> for address a: the data_symbols.inc C type (u32 -> std::uint32_t, Guid -> std::uint8_t[16],
    arrays kept), IAT slots std::uintptr_t"""
    if a not in syms:
        return 'std::uintptr_t'
    m = re.match(r'^(\w+)((?:\[\d+\])*)$', syms[a][1])
    if not m:
        raise SystemExit('0x%06x: unsupported C type %r in data_symbols.inc' % (a, syms[a][1]))
    t = SCALAR_TYPES.get(m.group(1), m.group(1))
    if '[' in t and m.group(2):
        raise SystemExit('0x%06x: array of %s' % (a, m.group(1)))
    return t + m.group(2)


def _msvc_type(s, i):
    """index after one MSVC-decorated type at s[i] (the subset v3d_87.dll's exports use)"""
    c = s[i]
    if c.isdigit() or c in 'CDEFGHIJKMNOX':
        return i + 1
    if c == '_':
        return i + 2
    if c in 'PQAB':
        return _msvc_type(s, i + 2)                    # pointer / reference + cv qualifier
    if c in 'UV':
        i += 1
        while s[i] != '@':                              # qualified name: fragments `name@` or back-reference digits
            i = i + 1 if s[i].isdigit() else s.index('@', i) + 1
        return i + 1
    raise ValueError('unsupported MSVC type %r in %s' % (c, s))


def msvc_arg_count(func):
    """number of parameters of a decorated free function `?name@@YA<ret><args>@Z` (None when not of that form)"""
    m = re.match(r'^\?\w+@@YA', func)
    if not m:
        return None
    i = _msvc_type(func, m.end())
    if func[i:] == 'XZ':
        return 0
    n = 0
    while func[i] != '@':
        i = _msvc_type(func, i)
        n += 1
    return n


def iat_name(dll, ordinal, func):
    d = cname(re.sub(r'\.dll$', '', dll, flags=re.I))
    if ordinal or not func:
        f = 'ord%d' % ordinal
    else:
        m = re.match(r'^\?([A-Za-z_]\w*)@(?:([A-Za-z_]\w*)@)?@', func)    # ?name@@ / ?method@Class@@
        f = cname(((m.group(2) + '_') if m.group(2) else '') + m.group(1) if m else func)
    return 'iat_%s_%s' % (d, f)


def build(syms, imports, used, comments, votes, classes, functions, pe, existing=None):
    """[(name, address, comment, kind)]; the names of `existing` (name -> address, the current data_names.hpp) are
    kept, a new name that clashes with one gets the address suffix"""
    rows = []
    for a in sorted(used):
        if a in imports:
            dll, ordinal, func = imports[a]
            rows.append([iat_name(dll, ordinal, func), a, '%s %s' % (dll, func or 'ordinal %d' % ordinal), 'iat'])
        else:
            if a not in syms:
                raise SystemExit('0x%06x: named by src/** but not a live object of data_symbols.inc - regenerate '
                                 'src/data (re/gen_data_curated.py) first' % a)
            sec, ctype, sym = syms[a]
            name, origin = base_name(a, sec, ctype, sym, pe, comments, votes, classes, functions)
            rows.append([name, a, '%s %s %s' % (sec, ctype, sym), sec])
    for r in rows:
        if r[0] in MACROS or r[0] in KEYWORDS:
            r[0] += '_'
    # names are API once code uses them: an address with a name in the existing data_names.hpp keeps it
    keep = {v: n for n, v in (existing or {}).items()}
    for r in rows:
        if r[1] in keep:
            r[0] = keep[r[1]]
            r.append('kept')
    # overloads of one decorated v3d_87 function: `_<parameter count>`
    count = collections.Counter(r[0] for r in rows)
    for r in rows:
        if r[3] == 'iat' and count[r[0]] > 1 and r[-1] != 'kept':
            n = msvc_arg_count(imports[r[1]][2])
            if n is not None:
                r[0] = '%s_%d' % (r[0], n)
    count = collections.Counter(r[0] for r in rows)
    for r in rows:
        if count[r[0]] > 1 and r[-1] != 'kept':
            r[0] = '%s_%06x' % (r[0], r[1])
    names = [r[0] for r in rows]
    assert len(set(names)) == len(names), [n for n, k in collections.Counter(names).items() if k > 1][:20]
    return rows


def main():
    exe, hook_dir, out = sys.argv[1:4]
    pe = PE(exe)
    syms = load_symbols(hook_dir)
    imports = load_imports(hook_dir)
    existing = F.load_data_names(os.path.join(hook_dir, 'data_names.hpp'))
    existing.update(F.load_data_names(out))
    used, comments, called, votes = used_addresses(hook_dir, existing, imports)
    regnames = set(F.registry_names(hook_dir).values())
    classes = {n.split('::')[0] for n in regnames if '::' in n}
    functions = regnames | {f for d, o, f in imports.values()} | called
    rows = build(syms, imports, used, comments, votes, classes, functions, pe, existing)
    kinds = collections.Counter(r[3] for r in rows)
    with open(out, 'w', newline='\n') as o:
        o.write('// Generated by re/gen_data_names.py - do not edit. Regenerate (from the repository root):\n'
                '//   python3 re/gen_data_names.py <joan.exe> hook src/data_names.hpp\n'
                '//\n'
                '// Names of the joan.exe data addresses our C++ uses (stage S7a): .rdata / .data / .bss objects and IAT\n'
                '// slots. The values are exactly the addresses the code used; each is the start of a live object of\n'
                '// src/data/data_symbols.inc (comment: section, C type, data_symbols name) or an IAT slot of\n'
                '// src/runtime/import_table.inc (comment: DLL and function). Names: s_<text> strings, flt_ / dbl_<value> constants\n'
                '// (`m` = minus), vt_<Class> vtables, the hook\'s own names (kName -> g_name for globals), names evident from\n'
                '// comments, else g_<address> / k_<address>; `_<address>` appended where a name is not unique.\n'
                '// Each name is a typed constant img::Sym<T> (src/runtime/image.hpp; T = the data_symbols.inc type, IAT slots\n'
                '// std::uintptr_t): global<T>(d::Name) / ref(d::Name) / obj / str turn it into a pointer, img::addr(d::Name)\n'
                '// into a number; `d::Name + 0x10` is an img::Addr. There is no implicit conversion to an integer.\n'
                '// re/gen_code_slots.py, re/data_classify.py and re/fn_scan.py read this file: `d::Name` counts exactly like\n'
                '// the literal it stands for (and `d::Name + 0x10` like `0x... + 0x10`).\n'
                '#pragma once\n\n#include <cstdint>\n\n#include "image.hpp"\n\n'
                'namespace joan::d {\n')
        for title, kind in (('.rdata', 'rdata'), ('.data', 'data'), ('.bss', 'bss'), ('import address table slots',
                                                                                        'iat')):
            o.write('\n// ---- %s (%d)\n' % (title, kinds[kind]))
            for n, a, cm, k, *_ in rows:
                if k == kind:
                    o.write('inline constexpr img::Sym<%s> %s{0x%08xu};  // %s\n' % (sym_type(a, syms), n, a, cm))
        o.write('\n}  // namespace joan::d\n\nnamespace d = joan::d;\n')
    print('%d names (%s) -> %s' % (len(rows), ', '.join('%s %d' % kv for kv in sorted(kinds.items())), out))


if __name__ == '__main__':
    main()
