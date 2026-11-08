#!/usr/bin/env python3
"""Codemod A2 (docs/X64.md): raw field accesses of `this` -> data members of the function's class.

In every member function whose original address has a class in re/layout/layouts.json (and in unregistered
helpers of a `struct H` whose registered members all have one class), `f<T>(0x25)` and `at<T>(this, 0x25)` become

    jo::self<jo::DynArrayB>(this)->last_ele                 (T is the member's type)
    jo::view<T>(jo::self<jo::DynArrayB>(this)->last_ele)    (another type of the same width)
    jo::self<jo::Unit>(this)->stats[3]                      (an element of an array member)
    jo::self<jo::Button>(this)->state_97.alpha              (a member of an embedded object)

`jo::self` / `jo::view` (src/original/layout_access.hpp) compile to exactly the address arithmetic of at<T>, so
the x86 objects stay the same. Accesses that do not land exactly on a member (inside a field, in a gap, a width
that is not the member's) stay raw and are reported. The member layout is read from the compiled headers
(libclang over a TU including original/direct.hpp), so it is exactly what the compiler sees.

    re/layout/codemod_members.py [--dry-run] [--report out.txt] <src-relative .cpp files...> | --all
"""
import argparse
import collections
import json
import os
import re
import sys

import clang.cindex as ci

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import extract as X  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(HERE))
SRC = os.path.join(ROOT, 'src')
K = ci.CursorKind
FILLER = re.compile(r'^(gap_[0-9a-f]+|head|head_[0-9a-f]+|mid|mid2|tail|pad_?[0-9a-f]*)$')
T = ci.TypeKind


class Model:
    """Member layout of every struct of joan::orig as the compiler lays it out."""

    def __init__(self):
        tmp = os.path.join(SRC, f'_codemod_probe_{os.getpid()}.cpp')
        open(tmp, 'w').write('#include "original/direct.hpp"\n')
        try:
            tu = ci.Index.create().parse(tmp, args=X.ARGS, options=0)
            self.recs = {}
            for cur in tu.cursor.walk_preorder():
                if cur.kind == K.STRUCT_DECL and cur.is_definition() and cur.semantic_parent.spelling == 'orig' \
                        and cur.semantic_parent.semantic_parent.spelling == 'joan':
                    self.recs[cur.spelling] = self.record(cur)
        finally:
            os.remove(tmp)

    def record(self, cur):
        fields, base = [], None
        for ch in cur.get_children():
            if ch.kind == K.CXX_BASE_SPECIFIER and base is None:
                base = ch.type.get_declaration().spelling
            elif ch.kind == K.FIELD_DECL:
                t = ch.type
                ent = {'name': ch.spelling, 'off': cur.type.get_offset(ch.spelling) // 8, 'size': t.get_size(),
                       'canon': t.get_canonical().spelling}
                ct = t.get_canonical()
                if ct.kind == T.CONSTANTARRAY:
                    et = ct.element_type
                    ent.update(array=ct.element_count, elem=et.get_size(), elem_canon=et.get_canonical().spelling,
                               elem_rec=et.get_declaration().spelling if et.kind == T.RECORD else None)
                elif ct.kind == T.RECORD:
                    ent['rec'] = ct.get_declaration().spelling
                fields.append(ent)
        return {'base': base, 'fields': fields}

    def resolve(self, cls, off, size, canon, depth=0):
        """Member path for an access of `size` bytes at `off`: (path text, member canonical type) or None."""
        r = self.recs.get(cls)
        if r is None or depth > 12:
            return None
        for f in r['fields']:
            if not (f['off'] <= off < f['off'] + f['size']):
                continue
            if FILLER.match(f['name']) and f.get('elem_canon') in ('unsigned char', 'uint8_t'):
                return None  # a gap / padding byte array: the spec has no member here
            d = off - f['off']
            if 'array' in f:
                i, rem = divmod(d, f['elem'])
                if f.get('elem_rec') and f['elem_rec'] in self.recs:
                    sub = self.resolve(f['elem_rec'], rem, size, canon, depth + 1)
                    return (f'{f["name"]}[{i}].{sub[0]}', sub[1]) if sub else None
                if rem == 0 and size == f['elem']:
                    return (f'{f["name"]}[{i}]', f['elem_canon'])
                return None
            if 'rec' in f and f['rec'] in self.recs:
                sub = self.resolve(f['rec'], d, size, canon, depth + 1)
                return (f'{f["name"]}.{sub[0]}', sub[1]) if sub else None
            if d == 0 and size == f['size']:
                return (f['name'], f['canon'])
            return None
        if r['base']:
            sub = self.resolve(r['base'], off, size, canon, depth + 1)
            if sub is None:
                return None
            first = re.match(r'\w+', sub[0]).group(0)
            if '::' not in sub[0].split('.')[0] and self.hides(cls, r['base'], first):
                return (f'{self.base_owner(r["base"], first)}::{sub[0]}', sub[1])  # a derived member hides it
            return sub
        return None

    def hides(self, cls, base, name):
        """True if `cls` (or a class between it and `base`) declares a member `name`."""
        return any(f['name'] == name for f in self.recs.get(cls, {}).get('fields', []))

    def base_owner(self, base, name):
        c = base
        while c in self.recs:
            if any(f['name'] == name for f in self.recs[c]['fields']):
                return c
            c = self.recs[c]['base']
        return base


def load_classes():
    lay = json.load(open(os.path.join(HERE, 'layouts.json')))
    return lay['functions']


def implicit_this(call):
    """True if a member call `f<T>(...)` is made on `this` (implicitly or as `this->f`)."""
    ref = next(iter(call.get_children()), None)
    while ref is not None and ref.kind == K.UNEXPOSED_EXPR:
        ref = next(iter(ref.get_children()), None)
    if ref is None or ref.kind != K.MEMBER_REF_EXPR:
        return True
    base = [c for c in ref.get_children() if c.kind != K.TEMPLATE_REF and c.kind != K.TYPE_REF]
    return not base or X.strip(base[0]).kind == K.CXX_THIS_EXPR


def template_arg_text(call):
    toks = [t.spelling for t in call.get_tokens()]
    if '<' not in toks:
        return None
    i = toks.index('<')
    depth, j = 0, i
    while j < len(toks):
        if toks[j] == '<':
            depth += 1
        elif toks[j] == '>':
            depth -= 1
            if depth == 0:
                break
        elif toks[j] == '>>':
            depth -= 2
            if depth <= 0:
                break
        j += 1
    out = ''
    for t in toks[i + 1:j]:
        if out and (re.match(r'\w', t) and re.match(r'.*\w$', out)):
            out += ' '
        out += t
    return out


class Mod:
    def __init__(self, model, fn_class):
        self.M = model
        self.fn_class = fn_class
        self.stats = collections.Counter()
        self.left = collections.Counter()
        self.log = []

    def run(self, rel, dry):
        path = os.path.join(SRC, rel)
        main = os.path.realpath(path)
        try:
            tu = ci.Index.create().parse(path, args=X.ARGS, options=0)
        except ci.TranslationUnitLoadError:
            self.left['file not parsed (libclang)'] += 1
            self.log.append(f'{rel}: libclang could not parse the file - left as is')
            return 0
        src = open(path, 'rb').read()
        regs = {}
        for cur in tu.cursor.walk_preorder():
            if cur.kind == K.VAR_DECL and cur.spelling.startswith('joan_replace_') and X.in_main(cur, main):
                addr, fn = None, None
                for n in cur.walk_preorder():
                    if n.kind == K.INTEGER_LITERAL and addr is None:
                        addr = X.evaluate(n)
                    elif n.kind in (K.DECL_REF_EXPR, K.MEMBER_REF_EXPR) and n.referenced is not None and \
                            n.referenced.kind in X.FUNC_KINDS and n.referenced.spelling != 'code_address':
                        fn = n.referenced.get_usr()
                if fn and addr is not None:
                    regs.setdefault(fn, addr)
        # class per method
        methods = []
        for cur in tu.cursor.walk_preorder():
            if cur.kind == K.CXX_METHOD and cur.is_definition() and X.in_main(cur, main) and \
                    cur.semantic_parent.spelling == 'H' and not cur.is_static_method():
                methods.append(cur)
        by_h = collections.defaultdict(list)
        cls_of = {}
        for m in methods:
            usr = m.get_usr()
            by_h[m.semantic_parent.get_usr()].append(m)
            if usr in regs:
                c = self.fn_class.get(f'{regs[usr]:#x}')
                if c and c in self.M.recs:
                    cls_of[usr] = c
        for h, ms in by_h.items():
            cs = {cls_of[m.get_usr()] for m in ms if m.get_usr() in cls_of}
            if len(cs) == 1:
                c = next(iter(cs))
                for m in ms:
                    if m.get_usr() not in regs:
                        cls_of.setdefault(m.get_usr(), c)
        edits = []
        for m in methods:
            c = cls_of.get(m.get_usr())
            for call in m.walk_preorder():
                if call.kind != K.CALL_EXPR or call.spelling not in ('f', 'at'):
                    continue
                rt = X.raw_type(call)
                if rt is None:
                    continue
                args = list(call.get_arguments())
                if call.spelling == 'f':
                    if len(args) != 1 or not implicit_this(call):
                        continue  # `other->f<T>(off)` reads another object (not `this`)
                    off_e = args[0]
                else:
                    if len(args) != 2 or X.strip(args[0]) is None or X.strip(args[0]).kind != K.CXX_THIS_EXPR:
                        continue
                    off_e = args[1]
                self.stats['this accesses'] += 1
                if c is None:
                    self.left['function has no class'] += 1
                    continue
                off = X.evaluate(off_e)
                if off is None:
                    self.left['variable offset'] += 1
                    continue
                st, en = call.extent.start, call.extent.end
                if st.file is None or os.path.realpath(st.file.name) != main or en.file is None:
                    self.left['in a macro'] += 1
                    continue
                canon = rt.get_canonical().spelling
                res = self.M.resolve(c, off, rt.get_size(), canon)
                if res is None:
                    self.left['no member at the offset'] += 1
                    self.log.append(f'{rel}:{st.line}: {c}+{off:#x} ({rt.get_size()} B {canon}) not on a member')
                    continue
                targ = template_arg_text(call)
                if targ is None:
                    self.left['template argument not found'] += 1
                    continue
                expr = f'jo::self<jo::{c}>(this)->{res[0]}'
                if res[1] != canon:
                    expr = f'jo::view<{targ}>({expr})'
                    self.stats['converted (view)'] += 1
                else:
                    self.stats['converted'] += 1
                edits.append((st.offset, en.offset, expr))
        if edits and not dry:
            edits.sort()
            for a, b in zip(edits, edits[1:]):
                if b[0] < a[1]:
                    raise SystemExit(f'{rel}: overlapping edits at {a} {b}')
            out = bytearray(src)
            for s0, e0, text in reversed(edits):
                out[s0:e0] = text.encode()
            text = out.decode('utf-8')
            if 'namespace jo = joan::orig;' not in text:
                text = add_prelude(text)
            open(path, 'w', encoding='utf-8').write(text)
        return len(edits)


class Chains:
    """Pass 2: `at<T>(base, off)` where `base` is (or views) a pointer whose pointee class has a layout:
    `at<T>(jo::view<void*>(jo::self<jo::C>(this)->m), 0x10)` -> `jo::self<jo::C>(this)->m->field` (or a view)."""

    def __init__(self, model):
        self.M = model
        self.stats = collections.Counter()
        self.log = []

    def pointee(self, e):
        """(record name, source text of an expression of that pointer type) for a base expression, or None."""
        x = X.strip(e)
        for _ in range(3):
            if x is None:
                return None
            t = x.type.get_canonical()
            if t.kind == T.POINTER and t.get_pointee().get_canonical().kind == T.RECORD:
                rec = t.get_pointee().get_canonical().get_declaration()
                if rec.spelling in self.M.recs and rec.semantic_parent.spelling == 'orig':
                    return rec.spelling, x
            if x.kind == K.CALL_EXPR and x.spelling == 'view':
                args = list(x.get_arguments())
                if len(args) == 1:
                    x = X.strip(args[0])
                    continue
            return None
        return None

    def run(self, rel, dry):
        path = os.path.join(SRC, rel)
        main = os.path.realpath(path)
        try:
            tu = ci.Index.create().parse(path, args=X.ARGS, options=0)
        except ci.TranslationUnitLoadError:
            self.log.append(f'{rel}: libclang could not parse the file')
            return 0
        src = open(path, 'rb').read()
        edits = []
        for call in tu.cursor.walk_preorder():
            if call.kind != K.CALL_EXPR or call.spelling != 'at':
                continue
            st, en = call.extent.start, call.extent.end
            if st.file is None or os.path.realpath(st.file.name) != main or en.file is None:
                continue
            rt = X.raw_type(call)
            args = list(call.get_arguments())
            if rt is None or len(args) != 2:
                continue
            pe = self.pointee(args[0])
            if pe is None:
                continue
            self.stats['through a typed pointer'] += 1
            off = X.evaluate(args[1])
            if off is None:
                self.stats['left: variable offset'] += 1
                continue
            rec, base = pe
            canon = rt.get_canonical().spelling
            res = self.M.resolve(rec, off, rt.get_size(), canon)
            if res is None:
                self.stats['left: no member at the offset'] += 1
                self.log.append(f'{rel}:{st.line}: {rec}+{off:#x} ({rt.get_size()} B {canon}) not on a member')
                continue
            targ = template_arg_text(call)
            bs, be = base.extent.start, base.extent.end
            if targ is None or bs.file is None or os.path.realpath(bs.file.name) != main:
                self.stats['left: macro / template text'] += 1
                continue
            btext = src[bs.offset:be.offset].decode('utf-8')
            if not re.fullmatch(r'[\w:<>\->\.\[\]\(\)]*', btext) or btext.count('(') != btext.count(')'):
                btext = f'({btext})'
            expr = f'{btext}->{res[0]}'
            if res[1] != canon:
                expr = f'jo::view<{targ}>({expr})'
                self.stats['converted (view)'] += 1
            else:
                self.stats['converted'] += 1
            edits.append((st.offset, en.offset, expr))
        # nested chains: keep the outermost-first rule simple - drop edits contained in another edit (next run gets them)
        edits.sort()
        keep = []
        for e in edits:
            if keep and e[0] < keep[-1][1]:
                self.stats['deferred (nested)'] += 1
                continue
            keep.append(e)
        if keep and not dry:
            out = bytearray(src)
            for s0, e0, text in reversed(keep):
                out[s0:e0] = text.encode()
            text = out.decode('utf-8')
            if 'namespace jo = joan::orig;' not in text:
                text = add_prelude(text).replace('@LA@', '../' * rel.count('/') + 'original/layout_access.hpp')
            open(path, 'w', encoding='utf-8').write(text)
        return len(keep)


def add_prelude(text):
    """`#include "original/layout_access.hpp"` + `namespace jo = joan::orig;` after the file's first #include block."""
    lines = text.split('\n')
    last = None
    for j, line in enumerate(lines):
        if line.startswith('#include'):
            last = j
        elif line.strip() and not line.startswith('//') and not line.startswith('#'):
            break
    lines[last + 1:last + 1] = ['#include "@LA@"', '', 'namespace jo = joan::orig;']
    return '\n'.join(lines)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('files', nargs='*')
    ap.add_argument('--all', action='store_true')
    ap.add_argument('--dry-run', action='store_true')
    ap.add_argument('--chains', action='store_true', help='pass 2: accesses through typed pointer members')
    ap.add_argument('--fnmap', help='JSON {"0xaddr": "Class"}: extra / corrected function classes (A3 rounds)')
    ap.add_argument('--report', default=os.path.join(HERE, 'codemod_members_report.txt'))
    a = ap.parse_args()
    files = a.files
    if a.all:
        files = sorted(os.path.relpath(os.path.join(r, f), SRC) for r, _, fs in os.walk(SRC) for f in fs
                       if f.endswith('.cpp') and not r.startswith(os.path.join(SRC, 'lib')))
    model = Model()
    if a.chains:
        ch = Chains(model)
        for rel in files:
            print(f'{rel}: {ch.run(rel, a.dry_run)}', flush=True)
        rep = ['stats: ' + json.dumps(dict(ch.stats)), ''] + ch.log
        open(a.report, 'w').write('\n'.join(rep) + '\n')
        print(rep[0])
        return
    fc = load_classes()
    if a.fnmap:
        fc.update({f'{int(k, 16):#x}': v for k, v in json.load(open(a.fnmap)).items()})
    mod = Mod(model, fc)
    changed = 0
    for rel in files:
        n = mod.run(rel, a.dry_run)
        if n and not a.dry_run:
            # fix the include path relative to the file
            p = os.path.join(SRC, rel)
            depth = rel.count('/')
            t = open(p, encoding='utf-8').read().replace('@LA@', '../' * depth + 'original/layout_access.hpp')
            open(p, 'w', encoding='utf-8').write(t)
        changed += 1 if n else 0
        print(f'{rel}: {n}', flush=True)
    rep = [f'files changed: {changed}', 'stats: ' + json.dumps(dict(mod.stats)), 'left raw: ' + json.dumps(dict(mod.left)), ''] + mod.log
    open(a.report, 'w').write('\n'.join(rep) + '\n')
    print('\n'.join(rep[:3]))


if __name__ == '__main__':
    main()
