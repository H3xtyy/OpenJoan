#!/usr/bin/env python3
"""Stage S7c-prep codemod (docs/RUNTIME.md): the module-local copies of the call helpers are deleted in favour
of src/call.hpp (joan::call), one using-declaration per deleted definition. Behaviour-preserving: src/call.hpp has
exactly the same bodies (S7c-prep: the target converts to the original address).

Usage (from the repository root):
  rewrite_call_helpers.py <hook dir> [--dry-run] [--report out.tsv]

A local definition is replaced only when its normalised token sequence equals one of the known forms below
(parameter names, `class` / `typename`, the spelling of the 32-bit address type, `R = void` vs no default for R,
`joan::hook::original` / `original`, `img::global` / `global` aside):
  thiscall      R (Obj::*)(A...) called through original<Fn>(address) on the module's empty `struct Obj {}`
  cdecl_call    reinterpret_cast<R(__cdecl*)(A...)>(address)(args...)
  stdcall_call  reinterpret_cast<R(__stdcall*)(A...)>(address)(args...)
  import_call   __cdecl through an IAT slot (img::Addr): reinterpret_cast<...>(global<std::uintptr_t>(slot)) or
                (*reinterpret_cast<R(__cdecl* [const]*)(A...)>(img::addr(slot)))
  stdcall_import  __stdcall through an IAT slot, same two forms
Every other definition of a helper with one of these names (another calling convention or default return type under
the same name, a raw-address IAT slot, a definition in a class) is a variant: kept, and listed in the report.
In files that use __LINE__ the replacement keeps the line count (blank lines), so those lines do not move.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fn_scan as F  # noqa: E402

NAMES = ('thiscall', 'cdecl_call', 'stdcall_call', 'import_call', 'stdcall_import')
TOK = re.compile(r'[A-Za-z_]\w*|0x[0-9a-fA-F]+|\d+|::|->\*|->|\.\.\.|\S')
ADDR_T = {('std', '::', 'uintptr_t'), ('uintptr_t',), ('uptr',), ('u32',), ('std', '::', 'uint32_t')}


def canon(text):
    """normalised token list of a helper definition (comments / strings already blanked)"""
    t = TOK.findall(text)
    t = ['class' if x == 'typename' else x for x in t]
    s = ' '.join(t)
    s = s.replace('class R = void ,', 'class R ,')
    s = s.replace('joan :: hook :: original', 'original').replace('img :: global', 'global')
    s = s.replace('* const *', '* *')
    for a in sorted(ADDR_T, key=len, reverse=True):
        s = re.sub(r'(?<=\( )' + re.escape(' '.join(a)) + r'(?= \w+ [,)])', 'ADDR', s)
    # parameter names -> P0, P1, ... (the function parameter list: first '(' after the name)
    m = re.search(r'\b(' + '|'.join(NAMES) + r') \(', s)
    if m:
        i = m.end()
        depth, j = 1, i
        toks = s[i:].split()
        params, cur, k = [], [], 0
        while k < len(toks):
            x = toks[k]
            if x in '([<':
                depth += 1
            elif x in ')]>':
                depth -= 1
                if depth == 0:
                    if cur:
                        params.append(cur)
                    break
            if x == ',' and depth == 1:
                params.append(cur)
                cur = []
            else:
                cur.append(x)
            k += 1
        names = [p[-1] for p in params if p and re.match(r'^[A-Za-z_]\w*$', p[-1])]
        for n, name in enumerate(names):
            s = re.sub(r'(?<![\w:])' + re.escape(name) + r'(?!\w)', 'P%d' % n, s)
    return s


FORMS = {
    'thiscall': [
        'template < class R , class ... A > R thiscall ( ADDR P0 , const void * P1 , A ... P2 ) { using Fn = R ( Obj '
        ':: * ) ( A ... ) ; return ( static_cast < Obj * > ( const_cast < void * > ( P1 ) ) ->* original < Fn > ( P0 ) '
        ') ( P2 ... ) ; }'],
    'cdecl_call': [
        'template < class R , class ... A > R cdecl_call ( ADDR P0 , A ... P1 ) { return reinterpret_cast < R ( __cdecl '
        '* ) ( A ... ) > ( P0 ) ( P1 ... ) ; }'],
    'stdcall_call': [
        'template < class R , class ... A > R stdcall_call ( ADDR P0 , A ... P1 ) { return reinterpret_cast < R ( '
        '__stdcall * ) ( A ... ) > ( P0 ) ( P1 ... ) ; }'],
    'import_call': [
        'template < class R , class ... A > R import_call ( img :: Addr P0 , A ... P1 ) { return reinterpret_cast < R ( '
        '__cdecl * ) ( A ... ) > ( global < std :: uintptr_t > ( P0 ) ) ( P1 ... ) ; }',
        'template < class R , class ... A > R import_call ( img :: Addr P0 , A ... P1 ) { return ( * reinterpret_cast < '
        'R ( __cdecl * * ) ( A ... ) > ( img :: addr ( P0 ) ) ) ( P1 ... ) ; }'],
    'stdcall_import': [
        'template < class R , class ... A > R stdcall_import ( img :: Addr P0 , A ... P1 ) { return reinterpret_cast < '
        'R ( __stdcall * ) ( A ... ) > ( global < std :: uintptr_t > ( P0 ) ) ( P1 ... ) ; }',
        'template < class R , class ... A > R stdcall_import ( img :: Addr P0 , A ... P1 ) { return ( * '
        'reinterpret_cast < R ( __stdcall * * ) ( A ... ) > ( img :: addr ( P0 ) ) ) ( P1 ... ) ; }'],
}
DEF_RE = re.compile(r'\b(' + '|'.join(NAMES) + r')\s*\(')


def enclosing_is_namespace(code, pos):
    """the innermost unmatched '{' before pos opens a namespace (or there is none: file scope)"""
    depth = 0
    i = pos - 1
    while i >= 0:
        c = code[i]
        if c == '}':
            depth += 1
        elif c == '{':
            if depth == 0:
                head = code[max(0, i - 200):i]
                head = re.split(r'[;{}]', head)[-1]
                return re.search(r'\bnamespace\b[\w:\s]*$', head) is not None or re.search(r'extern\s+"', head)
            depth -= 1
        i -= 1
    return True


def find_defs(code):
    """[(name, start, end)] of the definitions of NAMES (start = 'template' or the return type, end after '}')"""
    out = []
    for m in DEF_RE.finditer(code):
        spans, j = F.split_args(code, m.end() - 1)
        mm = re.match(r'\s*(?:noexcept\s*)?\{', code[j + 1:j + 64])
        if not mm:
            continue
        p = m.start() - 1
        while p >= 0 and code[p] not in ';{}':
            p -= 1
        # a preceding `template <...>` may contain '>' only; a '}' / ';' / '{' ends the declaration head
        head = code[p + 1:m.start()]
        h = re.sub(r'\btemplate\s*<[^;{}]*?>\s*(?=[\w:])', ' ', head, count=1)
        if not h.strip() or re.search(r'\breturn\b|[=()]|\bif\b|\.|->', h):
            continue
        b = j + 1 + mm.end() - 1
        depth, e = 0, b
        while e < len(code):
            if code[e] == '{':
                depth += 1
            elif code[e] == '}':
                depth -= 1
                if depth == 0:
                    break
            e += 1
        s = p + 1
        while s < m.start() and code[s] in ' \t\n':
            s += 1
        out.append((m.group(1), s, e + 1))
    return out


def rewrite(src, rel, stats, variants):
    code = F.strip(src)
    keep_lines = '__LINE__' in code
    reps = []
    for name, s, e in find_defs(code):
        ln = src.count('\n', 0, s) + 1
        c = canon(code[s:e])
        if c in FORMS[name] and enclosing_is_namespace(code, s):
            # whole lines only: the definition must start a line and end one
            ls = src.rfind('\n', 0, s) + 1
            le = src.find('\n', e)
            le = len(src) if le < 0 else le
            if src[ls:s].strip() or src[e:le].strip():
                variants.append((rel, ln, name, 'not on lines of its own', c))
                continue
            indent = src[ls:s]
            old = src[ls:le]
            new = '%susing joan::call::%s;' % (indent, name)
            if keep_lines:
                new += '\n' * old.count('\n')
            reps.append((ls, le, new))
            stats[name] += 1
        else:
            variants.append((rel, ln, name, 'variant', c))
    for ls, le, new in sorted(reps, reverse=True):
        src = src[:ls] + new + src[le:]
    return src


def main():
    import collections
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    if not args:
        sys.exit(__doc__)
    hook = args[0]
    report = sys.argv[sys.argv.index('--report') + 1] if '--report' in sys.argv else None
    stats, variants, files = collections.Counter(), [], 0
    for root, _, names in os.walk(hook):
        for fn in sorted(names):
            if not fn.endswith(('.cpp', '.hpp', '.h')) or fn in F.SKIP_FILES or fn == 'call.hpp':
                continue
            path = os.path.join(root, fn)
            rel = os.path.relpath(path, hook)
            if F.SKIP_REL.match(rel):
                continue
            src = open(path, newline='').read()
            out = rewrite(src, rel, stats, variants)
            if out != src:
                files += 1
                if '--dry-run' not in sys.argv:
                    open(path, 'w', newline='').write(out)
    for k in NAMES:
        print('%-15s %4d local copies -> using joan::call::%s' % (k, stats[k], k))
    print('%d files changed, %d variants kept' % (files, len(variants)))
    for rel, ln, name, why, c in variants:
        print('  kept %s:%d %s (%s)' % (rel, ln, name, why))
    if report:
        with open(report, 'w') as o:
            for rel, ln, name, why, c in variants:
                o.write('%s\t%d\t%s\t%s\t%s\n' % (rel, ln, name, why, c))


if __name__ == '__main__':
    main()
