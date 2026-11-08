#!/usr/bin/env python3
"""Save record generator (re/a6/SAVES_PLAN.md 3.2): re/save/records.json + the game's typed structs -> field-wise
writers / readers.

For every record of a section that is not "todo" it reads the C++ layout of joan::orig::<class> (libclang over a TU
that includes src/original/direct.hpp, i686 target - the members, bases and offsets the compiler really sees, i.e. the
classes src/original/gen/*.hpp generated from re/layout/layouts.json and the hand-typed ones), flattens the members
of [from, from+size) into scalar / array / pointer leaves (member structs and base classes recursed into) and writes

    src/save/gen/records_<section>.hpp   field table kFields_<R>, descriptor k<R>, put_<R>(Out&, const T&) and
                                         get_<R>(In&, T&) as straight-line member accesses (templates over T, so the
                                         test can use the same functions on a mirror struct)
    re/save/gen/schema_<section>.json    the field tables for re/save/savecmp.py

It fails (exit 1, nothing written) when the leaves do not tile [from, from+size) exactly, a pointer member has no
kind in "ptr", a leaf straddles the range, image_words meets a dword it cannot carry as a word, or the class size
differs from layouts.json. The report (stdout) gives per record the typed / gap / opaque bytes.

    python3 re/save/gen_save.py [--section s0 ...] [--check]     --check: compare with the files on disk, write nothing
"""
import argparse
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
SRC = os.path.join(ROOT, 'src')
OUT_HPP = os.path.join(SRC, 'save', 'gen')
OUT_JSON = os.path.join(HERE, 'gen')
sys.path.insert(0, os.path.join(ROOT, 're', 'layout'))

PTR_KINDS = ('Flag', 'Cleared', 'Kept', 'Image', 'Stale')
# canonical clang type kind -> (Type, Out / In method, C++ type for casts)
SCALARS = {
    'UCHAR': ('U8', 'u8'), 'CHAR_U': ('U8', 'u8'), 'SCHAR': ('I8', 'i8'), 'CHAR_S': ('I8', 'i8'),
    'USHORT': ('U16', 'u16'), 'SHORT': ('I16', 'i16'), 'UINT': ('U32', 'u32'), 'INT': ('I32', 'i32'),
    'ULONG': ('U32', 'u32'), 'LONG': ('I32', 'i32'), 'FLOAT': ('F32', 'f32'),
}
UNROLL = 8  # arrays up to this many elements are written out, longer ones become a loop


def num(x):
    return int(x, 16) if isinstance(x, str) else int(x)


class Fail(Exception):
    pass


def parse_tu():
    import clang.cindex as ci
    import extract as X
    tmp = os.path.join(SRC, '_gen_save_probe.cpp')
    open(tmp, 'w').write('#include "original/direct.hpp"\n')
    try:
        tu = ci.Index.create().parse(tmp, args=X.ARGS, options=0)
    finally:
        os.remove(tmp)
    classes = {}
    for cur in tu.cursor.walk_preorder():
        if cur.kind == ci.CursorKind.STRUCT_DECL and cur.is_definition() and cur.semantic_parent.spelling == 'orig':
            classes.setdefault(cur.spelling, cur)
    return ci, classes


class Leaf:
    def __init__(self, off, size, elem, path, type_, method=None, count=1, kind=None, direct=None, arr=False):
        self.off, self.size, self.elem, self.path, self.type, self.method = off, size, elem, path, type_, method
        self.arr = arr  # an array member (its name decays to a pointer)
        self.count, self.kind, self.direct = count, kind, direct  # direct: (member name, class offset) of T itself


def leaves(ci, cur, base, prefix, direct, out):
    """Leaves of struct cursor `cur` placed at byte `base` of the record object, member path `prefix`."""
    TK = ci.TypeKind
    for ch in cur.get_children():
        if ch.kind == ci.CursorKind.CXX_BASE_SPECIFIER:
            bdecl = ch.type.get_canonical().get_declaration()
            boff = cur.type.get_offset_of_base(bdecl) if hasattr(cur.type, 'get_offset_of_base') else None
            if boff is None:
                boff = 0
                # non-virtual single inheritance of a packed struct: the base is at 0 (asserted by the class headers)
            leaves(ci, bdecl, base + boff, prefix, False, out)
            continue
        if ch.kind != ci.CursorKind.FIELD_DECL:
            continue
        off = base + cur.type.get_offset(ch.spelling) // 8
        name = prefix + ch.spelling
        d = (ch.spelling, off) if direct else None
        t = ch.type.get_canonical()
        if t.kind == TK.CONSTANTARRAY:
            et = t.element_type.get_canonical()
            n = t.element_count
            esz = et.get_size()
            if et.kind == TK.RECORD:
                for i in range(n):
                    leaves(ci, et.get_declaration(), off + i * esz, f'{name}[{i}].', False, out)
                    if d:
                        out.append(Leaf(off, 0, 0, name, 'Assert', direct=d))
                        d = None
                continue
            if et.kind in (TK.CHAR_S, TK.CHAR_U) and et.spelling.endswith('char') and 'signed' not in et.spelling \
                    and 'unsigned' not in et.spelling:
                out.append(Leaf(off, n, 1, name, 'Char', direct=d, arr=True))
            elif esz == 1 or et.kind.name not in SCALARS and et.kind != TK.POINTER:
                ty = 'Gap' if re.match(r'^gap_[0-9a-f]+$', ch.spelling) else 'Bytes'
                out.append(Leaf(off, n * esz, 1, name, ty, direct=d, arr=True))
            elif et.kind == TK.POINTER:
                out.append(Leaf(off, n * 4, 4, name, 'Pointer', count=n, direct=d))
            else:
                ty, m = SCALARS[et.kind.name]
                out.append(Leaf(off, n * esz, esz, name, ty, m, count=n, direct=d))
        elif t.kind == TK.RECORD:
            if d:
                out.append(Leaf(off, 0, 0, name, 'Assert', direct=d))
            leaves(ci, t.get_declaration(), off, name + '.', False, out)
        elif t.kind == TK.POINTER:
            if t.get_pointee().get_canonical().kind in (TK.FUNCTIONPROTO, TK.FUNCTIONNOPROTO):
                raise Fail(f'{name}: function pointer member (needs a hand-written policy)')
            out.append(Leaf(off, 4, 4, name, 'Pointer', direct=d))
        elif t.kind.name in SCALARS:
            ty, m = SCALARS[t.kind.name]
            out.append(Leaf(off, t.get_size(), t.get_size(), name, ty, m, direct=d))
        else:  # bool, enum, double, 64-bit integers: the bytes (little-endian on every target)
            out.append(Leaf(off, t.get_size(), t.get_size(), name, 'Bytes', direct=d))


def ident(s):
    return re.sub(r'\W', '_', s)


def build(ci, classes, layouts, rec):
    name, cls = rec['name'], rec['class']
    frm, size = num(rec['from']), num(rec['size'])
    if cls not in classes:
        raise Fail(f'{name}: class {cls} not found in the C++ headers')
    cur = classes[cls]
    csize = cur.type.get_size()
    spec = layouts.get(cls)
    if spec is not None and spec.get('size') is not None and num(spec['size']) != csize:
        raise Fail(f'{name}: {cls} is 0x{csize:x} bytes in C++ but 0x{num(spec["size"]):x} in layouts.json')
    if frm + size > csize:
        raise Fail(f'{name}: range 0x{frm:x}+0x{size:x} beyond sizeof({cls}) 0x{csize:x}')
    raw = []
    leaves(ci, cur, 0, '', True, raw)
    asserts = [lf for lf in raw if lf.type == 'Assert' and frm <= lf.direct[1] < frm + size]
    ls = sorted([lf for lf in raw if lf.type != 'Assert' and lf.size > 0], key=lambda lf: lf.off)
    inside = []
    for lf in ls:
        if lf.off + lf.size <= frm or lf.off >= frm + size:
            continue
        if lf.off < frm or lf.off + lf.size > frm + size:
            raise Fail(f'{name}: member {lf.path} (0x{lf.off:x}+0x{lf.size:x}) straddles the range')
        inside.append(lf)
    pos = frm
    for lf in inside:
        if lf.off != pos:
            raise Fail(f'{name}: members do not tile the range at 0x{pos:x} (next {lf.path} at 0x{lf.off:x})')
        pos = lf.off + lf.size
    if pos != frm + size:
        raise Fail(f'{name}: members end at 0x{pos:x}, range ends at 0x{frm + size:x}')
    ptr = dict(rec.get('ptr', {}))
    for k, v in ptr.items():
        if v not in PTR_KINDS:
            raise Fail(f'{name}: ptr {k}: unknown kind {v}')
    for lf in inside:
        if lf.type == 'Pointer':
            if lf.path not in ptr:
                raise Fail(f'{name}: pointer member {lf.path} has no kind in "ptr"')
            lf.kind = ptr.pop(lf.path)
    if ptr:
        raise Fail(f'{name}: "ptr" names members that are no pointers in the range: {sorted(ptr)}')
    opaque = [(num(a), num(b)) for a, b in rec.get('opaque', [])]
    for a, b in opaque:
        hit = [lf for lf in inside if a <= lf.off and lf.off + lf.size <= a + b]
        if sum(lf.size for lf in hit) != b or any(lf.type not in ('Gap', 'Bytes', 'Char') for lf in hit):
            raise Fail(f'{name}: opaque 0x{a:x}+0x{b:x} must cover whole byte-array members')
        for lf in hit:
            lf.type = 'Opaque'
    if rec.get('image_words'):
        for lf in inside:
            rel = lf.off - frm
            if lf.type in ('U32', 'I32', 'F32') and lf.elem == 4 and rel % 4 == 0:
                lf.type, lf.kind = 'Word', 'Image'
            elif lf.type == 'Pointer':
                pass
            else:  # any whole dword of this leaf at a chunk dword offset?
                first = (rel + 3) // 4 * 4
                if first + 4 <= rel + lf.size:
                    raise Fail(f'{name}: image_words: {lf.path} covers dword +0x{first:x} but is no 32-bit scalar')
    legacy = {'raw': 'Raw', 'image_words': 'ImageWords'}[rec.get('legacy', 'raw')]
    return dict(name=name, cls=cls, frm=frm, size=size, legacy=legacy, leaves=inside, asserts=asserts, csize=csize,
                spec=spec)


def elems(lf):
    return [f'{lf.path}[{i}]' for i in range(lf.count)] if lf.count > 1 else [lf.path]


def byte_src(lf):
    return f'r.{lf.path}' if lf.arr else f'&r.{lf.path}'


def put_lines(lf):
    if lf.type in ('Char', 'Bytes', 'Gap', 'Opaque'):
        return [f'o.bytes({byte_src(lf)}, {lf.size});']
    if lf.type == 'Pointer':
        tmpl = 'o.ptr(r.{e}, Ptr::' + lf.kind + ');'
    elif lf.type == 'Word':
        tmpl = ('o.word(std::bit_cast<std::uint32_t>(r.{e}), Ptr::Image);' if lf.method == 'f32'
                else 'o.word(static_cast<std::uint32_t>(r.{e}), Ptr::Image);')
    elif lf.method == 'f32':  # the bits, never a float value (x87 on x86 would quiet signalling NaNs)
        tmpl = 'o.u32(std::bit_cast<std::uint32_t>(r.{e}));'
    else:
        tmpl = 'o.' + lf.method + '(r.{e});'
    return expand(lf, tmpl)


def get_lines(lf):
    if lf.type in ('Char', 'Bytes', 'Gap', 'Opaque'):
        return [f'i.bytes({byte_src(lf)}, {lf.size});']
    if lf.type == 'Pointer':
        tmpl = 'i.ptr(r.{e}, Ptr::' + lf.kind + ');'
    elif lf.type == 'Word':
        tmpl = ('r.{e} = std::bit_cast<float>(i.word(Ptr::Image));' if lf.method == 'f32'
                else 'r.{e} = static_cast<' + CTYPE[lf.method] + '>(i.word(Ptr::Image));')
    elif lf.method == 'f32':
        tmpl = 'r.{e} = std::bit_cast<float>(i.u32());'
    else:
        tmpl = 'r.{e} = i.' + lf.method + '();'
    return expand(lf, tmpl)


CTYPE = {'u32': 'std::uint32_t', 'i32': 'std::int32_t'}


def expand(lf, tmpl):
    if lf.count > UNROLL:
        return [f'for (int k = 0; k < {lf.count}; ++k) ' + tmpl.format(e=f'{lf.path}[k]')]
    return [tmpl.format(e=e) for e in elems(lf)]


def emit_hpp(section, recs):
    o = [f'// Generated by re/save/gen_save.py from re/save/records.json (section {section}) and the C++ layouts of',
         '// src/original (re/layout/layouts.json). Do not edit; change records.json and re-run the generator.',
         '#pragma once', '', '#include <bit>', '#include <cstddef>', '#include <cstdint>', '', '#include "../io.hpp"', '',
         'namespace joan::save {', '']
    for r in recs:
        n = ident(r['name'])
        o.append(f'// {r["name"]}: joan::orig::{r["cls"]} bytes [0x{r["frm"]:x}, 0x{r["frm"] + r["size"]:x}), '
                 f'legacy {r["legacy"]}')
        o.append(f'inline constexpr Field kFields_{n}[] = {{')
        for lf in r['leaves']:
            kind = lf.kind or 'Flag'
            o.append(f'    {{0x{lf.off - r["frm"]:x}, 0x{lf.size:x}, {lf.elem}, "{lf.path}", Type::{lf.type}, '
                     f'Ptr::{kind}}},')
        o.append('};')
        o.append(f'inline constexpr Record k{n}{{"{r["name"]}", "{r["cls"]}", 0x{r["size"]:x}, Legacy::{r["legacy"]}, '
                 f'kFields_{n}, {len(r["leaves"])}}};')
        o.append('')
        for which, fn, arg, lines in (('put', 'put', 'Out& o, const T& r', put_lines),
                                      ('get', 'get', 'In& i, T& r', get_lines)):
            o.append('template <class T>')
            o.append(f'void {fn}_{n}({arg}) {{')
            if which == 'put' and r['asserts'] + [lf for lf in r['leaves'] if lf.direct]:
                o.append('#if UINTPTR_MAX == 0xffffffffu  // x86: the members are where the chunk has them')
                seen = set()
                for lf in r['asserts'] + [lf for lf in r['leaves'] if lf.direct]:
                    m, off = lf.direct
                    if m in seen:
                        continue
                    seen.add(m)
                    o.append(f'    static_assert(offsetof(T, {m}) == 0x{off:x});')
                o.append('#endif')
            for lf in r['leaves']:
                for ln in lines(lf):
                    o.append('    ' + ln)
            o.append('}')
        o.append('')
    o.append('}  // namespace joan::save')
    return '\n'.join(o) + '\n'


def emit_json(section, recs):
    d = {'section': section, 'records': {}}
    for r in recs:
        d['records'][r['name']] = {
            'class': r['cls'], 'from': r['frm'], 'size': r['size'], 'legacy': r['legacy'],
            'fields': [[lf.off - r['frm'], lf.size, lf.elem, lf.path, lf.type, lf.kind] for lf in r['leaves']]}
    return json.dumps(d, indent=1) + '\n'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--section', action='append')
    ap.add_argument('--check', action='store_true')
    a = ap.parse_args()
    spec = json.load(open(os.path.join(HERE, 'records.json')))
    layouts = json.load(open(os.path.join(ROOT, 're', 'layout', 'layouts.json')))['classes']
    sections = a.section or list(spec['sections'])
    ci, classes = parse_tu()
    outputs, failed = {}, False
    for s in sections:
        recs = []
        for rec in spec['sections'][s]:
            if rec.get('todo'):
                continue
            try:
                r = build(ci, classes, layouts, rec)
            except Fail as e:
                print(f'FAIL {s} {e}')
                failed = True
                continue
            gap = sum(lf.size for lf in r['leaves'] if lf.type == 'Gap')
            opq = sum(lf.size for lf in r['leaves'] if lf.type == 'Opaque')
            ptrs = sum(1 for lf in r['leaves'] if lf.type == 'Pointer')
            print(f'{s} {r["name"]}: {r["cls"]} 0x{r["frm"]:x}+0x{r["size"]:x}: {len(r["leaves"])} fields, '
                  f'typed {r["size"] - gap - opq}/{r["size"]} bytes, gap {gap}, opaque {opq}, pointers {ptrs}, '
                  f'legacy {r["legacy"]}')
            recs.append(r)
        if not recs:
            continue
        outputs[os.path.join(OUT_HPP, f'records_{s}.hpp')] = emit_hpp(s, recs)
        outputs[os.path.join(OUT_JSON, f'schema_{s}.json')] = emit_json(s, recs)
    if failed:
        print('nothing written')
        return 1
    stale = 0
    for p, text in outputs.items():
        old = open(p).read() if os.path.exists(p) else None
        if a.check:
            if old != text:
                print(f'STALE {os.path.relpath(p, ROOT)}')
                stale += 1
            continue
        if old != text:
            os.makedirs(os.path.dirname(p), exist_ok=True)
            open(p, 'w').write(text)
            print(f'wrote {os.path.relpath(p, ROOT)}')
    return 1 if stale else 0


if __name__ == '__main__':
    sys.exit(main())
