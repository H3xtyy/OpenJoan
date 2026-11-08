#!/usr/bin/env python3
"""Record layouts as the compiler sees them, per target (docs/X64.md image): libclang over a translation unit
compiled for i686 or x86_64 (llvm-mingw headers, like re/layout/extract.py).

    sizes(classes, target)            -> {Class: sizeof} of joan::orig classes
    walk(tu_text, record, target)     -> (size, [(offset, size)] of every pointer leaf, recursively through bases,
                                         members and arrays)  - the input of the x64 address translation
"""
import os
import sys
import tempfile

import clang.cindex as ci

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
SRC = os.path.join(ROOT, 'src')
MINGW = '/opt/llvm-mingw'
SDL = '/tmp/sdl3/include'


def host_includes(t):
    """The host toolchain's C++ include directories (OpenJoan on Linux, 2026-10-30: no llvm-mingw installed; the packed
    layouts of layout.hpp are the same on every x86 / x86-64 target, and the Linux x64 build static_asserts them)."""
    import subprocess
    flags = ['-m32'] if t == 'i686' else []
    out = subprocess.run(['clang++', '-E', '-x', 'c++', '-', '-v'] + flags, input='', capture_output=True, text=True).stderr
    dirs, on = [], False
    for line in out.splitlines():
        if line.startswith('#include <...> search starts here'):
            on = True
        elif line.startswith('End of search list'):
            on = False
        elif on and line.strip():
            dirs.append(line.strip())
    return dirs


def args(target):
    t = {'x86': 'i686', 'x64': 'x86_64'}[target]
    common = ['-x', 'c++', '-std=c++20', '-fms-extensions', '-fasm-blocks', '-ferror-limit=0', '-Wno-everything',
              '-DWIN32_LEAN_AND_MEAN', '-DNOMINMAX', '-DJOAN_IMAGE_LAYOUT_PROBE', '-I' + SRC, '-I' + os.path.join(SRC, 'data')]
    if os.path.isdir(MINGW):
        return common + [f'--target={t}-w64-mingw32', '-nostdinc++', '-nostdinc', '-isystem',
                         f'{MINGW}/{t}-w64-mingw32/include/c++/v1', '-isystem', f'{MINGW}/lib/clang/18/include',
                         '-isystem', f'{MINGW}/{t}-w64-mingw32/include', '-isystem', SDL]
    inc = []
    for d in host_includes(t):
        inc += ['-isystem', d]
    return common + [f'--target={t}-linux-gnu', '-nostdinc++', '-nostdinc'] + inc + (['-isystem', SDL] if os.path.isdir(SDL) else [])


def parse(text, target, name='probe.cpp'):
    d = tempfile.mkdtemp(prefix='probe_')
    p = os.path.join(d, name)
    open(p, 'w').write(text)
    tu = ci.Index.create().parse(p, args=args(target), options=ci.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES)
    errs = [str(x) for x in tu.diagnostics if x.severity >= ci.Diagnostic.Error and 'single-inheritance' not in str(x)
            and 'only plain functions' not in str(x)]
    return tu, errs


def find_records(tu, names):
    """qualified name -> canonical Type, for `names` (e.g. 'joan::orig::Unit')"""
    want, out = set(names), {}

    def qual(c):
        parts = []
        while c is not None and c.kind != ci.CursorKind.TRANSLATION_UNIT:
            if c.spelling:
                parts.append(c.spelling)
            c = c.semantic_parent
        return '::'.join(reversed(parts))

    def rec(c):
        for k in c.get_children():
            if k.kind == ci.CursorKind.NAMESPACE:
                rec(k)
            elif k.kind in (ci.CursorKind.STRUCT_DECL, ci.CursorKind.CLASS_DECL, ci.CursorKind.TYPE_ALIAS_DECL,
                            ci.CursorKind.TYPEDEF_DECL):
                q = qual(k)
                if q in want and (q not in out) and (k.is_definition() or k.kind != ci.CursorKind.STRUCT_DECL):
                    t = k.type.get_canonical()
                    if t.get_size() > 0:
                        out[q] = t
    rec(tu.cursor)
    return out


def sizes(classes, headers, target):
    text = ''.join(f'#include "{h}"\n' for h in headers)
    tu, errs = parse(text, target)
    if errs:
        sys.exit('probe: ' + '\n'.join(errs[:10]))
    recs = find_records(tu, [f'joan::orig::{c}' for c in classes])
    return {q.split('::')[-1]: t.get_size() for q, t in recs.items()}


def leaves(t, base=0, out=None, depth=0):
    """pointer leaves (offset, size) of type t at offset base: pointers, references, uptr"""
    if out is None:
        out = []
    s = t.spelling.replace('const ', '')
    if s.endswith('uptr') or s.endswith('uintptr_t'):  # a stored pointer kept as an integer (data_common.hpp)
        out.append((base, t.get_size()))
        return out
    k = t.get_canonical().kind
    if k in (ci.TypeKind.POINTER, ci.TypeKind.MEMBERPOINTER, ci.TypeKind.LVALUEREFERENCE):
        out.append((base, t.get_size()))
    elif k == ci.TypeKind.CONSTANTARRAY:
        while t.kind in (ci.TypeKind.ELABORATED, ci.TypeKind.TYPEDEF):  # keep the element's spelling (uptr)
            if t.kind == ci.TypeKind.TYPEDEF:
                t = t.get_declaration().underlying_typedef_type
            else:
                t = t.get_named_type()
        e = t.element_type
        es, n = e.get_size(), t.element_count
        sub = leaves(e, 0, [], depth + 1)
        if sub:
            for i in range(n):
                out.extend((base + i * es + o, s) for o, s in sub)
    elif k == ci.TypeKind.RECORD:
        t = t.get_canonical()
        decl = t.get_declaration()
        if decl.kind == ci.CursorKind.UNION_DECL:
            return out
        for c in decl.get_children():
            if c.kind == ci.CursorKind.CXX_BASE_SPECIFIER:
                leaves(c.type, base, out, depth + 1)  # single, non-virtual bases at offset 0 (checked by the caller)
        for f in t.get_fields():
            if f.is_bitfield():
                continue
            leaves(f.type, base + f.get_field_offsetof() // 8, out, depth + 1)
    return out


def walk(text, record, target):
    tu, errs = parse(text, target)
    if errs:
        sys.exit('probe: ' + '\n'.join(errs[:10]))
    recs = find_records(tu, [record])
    t = recs[record]
    return t.get_size(), leaves(t)


def class_headers(classes):
    """class -> header (relative to src/) that defines struct <class> in namespace joan::orig"""
    import glob
    import re
    found = {}
    for p in sorted(glob.glob(os.path.join(SRC, 'original', 'gen', '*.hpp'))) + \
            sorted(glob.glob(os.path.join(SRC, 'original', '*.hpp'))):
        text = open(p).read()
        for m in re.finditer(r'^(?:struct (\w+)\b[^;]*?\{|using (\w+) = )', text, re.M):
            found.setdefault(m.group(1) or m.group(2), os.path.relpath(p, SRC))
    return {c: found.get(c) for c in classes}
