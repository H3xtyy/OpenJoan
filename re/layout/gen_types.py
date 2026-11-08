#!/usr/bin/env python3
"""Header generator (docs/X64.md): turns re/layout/layouts.json into real data members of the game's classes.

For every class with fields / members / a base it writes src/original/gen/<snake>_fields.inc (the data members, with
explicit gaps, packed like the original) and (re)writes the class header src/original/gen/<snake>.hpp:

    #pragma pack(push, 1)
    struct Plane : Resource {
    #include "plane_fields.inc"
    #include "plane_methods.inc"      // the slot facades (re/facades/gen2.py), if the class has any
    };
    #pragma pack(pop)
    // x86 layout checks (only where pointers are 4 bytes): sizeof / offsetof of every member

Classes typed by hand in src/original/*.hpp (File, DynArray, Unit, Widget, ...) keep their headers; for those it
writes only a report of how the hand layout and the spec differ (gen_types_hand.txt) - merged by hand.
Pointer members are `T*` (forward-declared), so on x64 they become 8 bytes and the layout grows naturally.

    re/layout/gen_types.py [--dry-run]       report: re/layout/gen_types_report.txt
"""
import argparse
import collections
import glob
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
GEN = os.path.join(ROOT, 'src', 'original', 'gen')
ORIG = os.path.join(ROOT, 'src', 'original')

SCALAR = {'i8': ('std::int8_t', 1), 'u8': ('std::uint8_t', 1), 'char': ('char', 1), 'bool8': ('std::uint8_t', 1),
          'i16': ('std::int16_t', 2), 'u16': ('std::uint16_t', 2), 'i32': ('std::int32_t', 4),
          'u32': ('std::uint32_t', 4), 'f32': ('float', 4), 'f64': ('double', 8), 'u8*': ('std::uint8_t*', 4)}
KEYWORDS = set('''alignas alignof and asm auto bitand bitor bool break case catch char class compl concept const
consteval constexpr constinit const_cast continue co_await co_return co_yield decltype default delete do double
dynamic_cast else enum explicit export extern false float for friend goto if inline int long mutable namespace new
noexcept not nullptr operator or private protected public register reinterpret_cast requires return short signed
sizeof static static_assert static_cast struct switch template this thread_local throw true try typedef typeid
typename union unsigned using virtual void volatile wchar_t while xor'''.split()) | {'offsetof', 'size_t'}


PAD = re.compile(r'^(head|mid|gap|pad|tail|unk|rest|reserved|raw|bytes|filler)(_?[0-9a-fx]*)?\d*_?$')


def hand_fields():
    """Data members of the hand-typed structs of src/original/*.hpp (libclang over a TU that includes direct.hpp)."""
    import clang.cindex as ci
    sys.path.insert(0, HERE)
    import extract as X
    tmp = os.path.join(ROOT, 'src', '_gen_types_probe.cpp')
    open(tmp, 'w').write('#include "original/direct.hpp"\n')
    try:
        tu = ci.Index.create().parse(tmp, args=X.ARGS, options=0)
        out = {}
        for cur in tu.cursor.walk_preorder():
            if cur.kind != ci.CursorKind.STRUCT_DECL or not cur.is_definition() or cur.location.file is None:
                continue
            fn = cur.location.file.name
            if '/original/' not in fn or '/gen/' in fn or cur.semantic_parent.spelling != 'orig':
                continue
            fs = []
            for ch in cur.get_children():
                if ch.kind == ci.CursorKind.FIELD_DECL:
                    fs.append({'name': ch.spelling, 'off': cur.type.get_offset(ch.spelling) // 8,
                               'size': ch.type.get_size(), 'type': ch.type.spelling,
                               'line': ch.extent.start.line, 'end': ch.extent.end.line})
            out[cur.spelling] = {'file': os.path.realpath(fn), 'size': cur.type.get_size(), 'fields': fs,
                                 'sline': cur.extent.start.line}
        return out
    finally:
        os.remove(tmp)


def snake(c):
    return re.sub(r'(?<=[a-z0-9])([A-Z])', r'_\1', c).lower().replace('3_d', '3d').replace('2_d', '2d')


def num(x):
    return None if x is None else (int(x, 16) if isinstance(x, str) else int(x))


def ident(name):
    n = re.sub(r'\W', '_', name or '')
    if not n or n[0].isdigit():
        n = 'f_' + n
    if n in KEYWORDS:
        n += '_'
    return n


FILES = {}


def method_names(cls):
    """Identifiers already declared in the class body by its methods include (a data member may not reuse them)."""
    p = os.path.join(GEN, f'{FILES.get(cls, snake(cls))}_methods.inc')
    if not os.path.exists(p):
        return set()
    return set(re.findall(r'^\s*(?:[\w:<>,*&\s]+?)\b(\w+)\s*\(', open(p).read(), re.M))


class Gen:
    def __init__(self, classes):
        self.C = classes
        self.size_memo = {}
        self.report = []
        self.facades = {os.path.basename(p)[:-4]: p for p in glob.glob(os.path.join(GEN, '*.hpp'))}
        self.file = {}  # class -> base file name (without .hpp) in src/original/gen
        for p in glob.glob(os.path.join(GEN, '*.hpp')):
            for m in re.finditer(r'^struct (\w+) \{', open(p).read(), re.M):
                self.file[m.group(1)] = os.path.basename(p)[:-4]
        taken = set(self.file.values()) | {'classes', 'accessors', 'free_functions'}
        for c in sorted(classes):
            if c not in self.file:
                f = snake(c)
                while f in taken:
                    f += '_t'
                self.file[c] = f
                taken.add(f)
        self.hand = {}
        for p in glob.glob(os.path.join(ORIG, '*.hpp')):
            t = open(p).read()
            for m in re.finditer(r'^struct (\w+)\b[^;{]*\{', t, re.M):
                self.hand[m.group(1)] = p
            for m in re.finditer(r'^using (\w+) =', t, re.M):
                self.hand[m.group(1)] = p

    def generated(self, c):
        return c in self.C and c not in self.hand

    # size the generated struct will have on x86 (declared size, else its extent)
    def size(self, c, depth=0):
        if c in self.size_memo:
            return self.size_memo[c]
        self.size_memo[c] = 0
        v = self.C.get(c)
        if v is None or depth > 30:
            return 0
        s = num(v.get('size'))
        ext = self.base_end(c, depth)
        for f in v.get('fields', []):
            ext = max(ext, num(f['off']) + self.field_bytes(f))
        for m in v.get('members', []):
            ext = max(ext, num(m['off']) + self.size(m['class'], depth + 1) * int(m.get('count', 1) or 1))
        r = s if s is not None and s >= ext else ext
        self.size_memo[c] = r
        return r

    def base_end(self, c, depth=0):
        b = self.C.get(c, {}).get('base')
        return self.size(b, depth + 1) if b and b in self.C and b != c else 0

    def field_bytes(self, f):
        return int(f.get('size', 0)) * int(f.get('count', 1) or 1)

    def ctype(self, f, fwd, incs, owner):
        t = f['type']
        if t.startswith('ptr:'):
            tgt = t[4:]
            if tgt in ('void', 'fn', 'vtable', ''):
                return 'void*'
            if tgt == 'char':
                return 'char*'
            if tgt in self.C or tgt in self.hand:
                if tgt != owner:
                    fwd.add(tgt)
                return f'{tgt}*'
            self.report.append(f'{owner}.{f["name"]}: pointer to unknown class {tgt} -> void*')
            return 'void*'
        if t in SCALAR:
            ct, sz = SCALAR[t]
            if sz != int(f.get('size', sz)):
                self.report.append(f'{owner}.{f["name"]}: type {t} but size {f.get("size")} -> bytes')
                return None
            return ct
        if t in self.C:  # a record by value written as a field type
            incs.add(t)
            return t
        self.report.append(f'{owner}.{f["name"]}: unknown type {t} -> bytes')
        return None

    def emit_class(self, c):
        v = self.C[c]
        fwd, incs = set(), set()
        taken = method_names(c)
        items = []  # (off, bytes, decl, name)
        for f in v.get('fields', []):
            off, nb = num(f['off']), self.field_bytes(f)
            ct = self.ctype(f, fwd, incs, c)
            cnt = int(f.get('count', 1) or 1)
            name = ident(f['name'])
            if ct is None:
                ct, cnt, nb = 'std::uint8_t', nb, nb
                elem = 1
            else:
                elem = int(f.get('size', 1))
            items.append([off, nb, ct, name, cnt, elem, f])
        for m in v.get('members', []):
            mc = m['class']
            if mc not in self.C and mc not in self.hand:
                self.report.append(f'{c}.{m["name"]}: member of unknown class {mc} -> skipped')
                continue
            incs.add(mc)
            cnt = int(m.get('count', 1) or 1)
            msz = self.size(mc) if mc in self.C else None
            items.append([num(m['off']), (msz or 0) * cnt, mc, ident(m['name']), cnt, msz or 0, m])
        items.sort(key=lambda x: x[0])
        base = v.get('base')
        bend = 0
        if base and (base in self.C or base in self.hand) and base != c:
            bend = self.base_end(c) if base in self.C else 0
            if items and items[0][0] < bend:
                self.report.append(f'{c}: base {base} ends at {bend:#x} but field {items[0][3]} starts at '
                                   f'{items[0][0]:#x} -> base dropped')
                base, bend = None, 0
            else:
                incs.add(base)
        else:
            base = None
        lines, used, pos, asserts = [], set(), bend, []
        for off, nb, ct, name, cnt, elem, src in items:
            if off < pos:
                self.report.append(f'{c}.{name}: at {off:#x} overlaps the previous member (ends {pos:#x}) -> skipped')
                continue
            if off > pos:
                lines.append(f'    std::uint8_t gap_{pos:x}[0x{off - pos:x}];')
            n = name
            while n in used or n in taken:
                n += '_'
            used.add(n)
            arr = f'[{cnt}]' if cnt > 1 else ''
            note = src.get('notes') or ''
            cmt = f'  // 0x{off:x}' + (f' {note}' if note and len(note) < 90 else '')
            lines.append(f'    {ct} {n}{arr};{cmt}')
            asserts.append((n, off))
            pos = off + (nb if nb else 0)
        total = self.size(c)
        if total > pos:
            lines.append(f'    std::uint8_t gap_{pos:x}[0x{total - pos:x}];')
        s = self.file[c]
        inc = [f'// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of {c}',
               f'// ({v.get("defined_in", "?")}, confidence {v.get("confidence", "?")}). Do not edit; change the spec.']
        inc += lines
        open(os.path.join(GEN, f'{s}_fields.inc'), 'w').write('\n'.join(inc) + '\n')
        return {'base': base, 'fwd': sorted(fwd - incs - {c}), 'incs': sorted(incs - {c}), 'asserts': asserts,
                'size': total if v.get('size') else None, 'empty': not lines}

    # --- hand-typed classes: fill their padding / tail from the spec, keep every hand-written member ------------
    def flat_items(self, c, base_off=0, depth=0, prefix=''):
        """Spec fields of c and (flattened) its base chain: [(off, bytes, field-or-member dict, kind)]."""
        v = self.C.get(c, {})
        out = []
        b = v.get('base')
        if b and b in self.C and b != c and depth < 20:
            out += self.flat_items(b, base_off, depth + 1)
        for f in v.get('fields', []):
            out.append((base_off + num(f['off']), self.field_bytes(f), f, 'field'))
        for m in v.get('members', []):
            sz = self.size(m['class']) if m['class'] in self.C else 0
            out.append((base_off + num(m['off']), sz * int(m.get('count', 1) or 1), m, 'member'))
        return out

    def emitted(self, c):
        """True when gen_types writes a header for c (a generated class with data members or a base)."""
        v = self.C.get(c, {})
        return self.generated(c) and bool(v.get('fields') or v.get('members') or v.get('base'))

    def decl(self, off, nb, item, kind, taken):
        name = ident(item['name'])
        while name in taken:
            name += '_'
        taken.add(name)
        mc = item['class'] if kind == 'member' else item['type']
        cnt = int(item.get('count', 1) or 1)
        if mc in self.C and self.emitted(mc) and nb == self.size(mc) * cnt:
            # round P8: an embedded member of a generated class (or a field typed with one) is that class (its x64
            # size follows its pointers); the hand header includes the generated header (merge_hand)
            self.member_incs.add(mc)
            return f'    {mc} {name}' + (f'[{cnt}]' if cnt > 1 else '') + f';  // 0x{off:x}'
        if kind == 'member' or item['type'] not in SCALAR and not item['type'].startswith('ptr:'):
            return f'    std::uint8_t {name}[0x{nb:x}];  // 0x{off:x} ({item.get("class") or item["type"]})'
        if item['type'].startswith('ptr:'):
            tgt = item['type'][4:]
            note = ''
            if tgt == 'char':
                ct = 'char*'
            elif tgt in self.C or tgt in self.hand:
                ct = f'{tgt}*'
                self.fwd.add(tgt)
            else:
                ct = 'void*'
        else:
            ct, note = SCALAR[item['type']][0], ''
        cnt = int(item.get('count', 1) or 1)
        return f'    {ct} {name}' + (f'[{cnt}]' if cnt > 1 else '') + f';  // 0x{off:x}{note}'

    def fill(self, start, end, items, taken):
        """Declarations covering [start, end) exactly: the spec items inside, gaps elsewhere."""
        lines, pos = [], start
        for off, nb, item, kind in sorted(items, key=lambda x: x[0]):
            if off < pos or off + nb > end or nb <= 0:
                continue
            if off > pos:
                lines.append(f'    std::uint8_t gap_{pos:x}[0x{off - pos:x}];')
            lines.append(self.decl(off, nb, item, kind, taken))
            pos = off + nb
        if end > pos:
            lines.append(f'    std::uint8_t gap_{pos:x}[0x{end - pos:x}];')
        return lines

    def merge_hand(self, hand):
        edits = collections.defaultdict(list)  # file -> [(first line, last line, replacement lines)]
        merged = []
        file_incs = collections.defaultdict(set)  # hand header -> generated headers its embedded members need
        for c, h in sorted(hand.items()):
            if c not in self.C or not h['fields']:
                continue
            fields = h['fields']
            if any(f['line'] != f['end'] for f in fields) or len({f['line'] for f in fields}) != len(fields):
                self.report.append(f'{c} (hand): members not one per line -> left as is')
                continue
            self.fwd = set()
            self.member_incs = set()
            text = open(h['file']).read()
            taken = {f['name'] for f in fields} | set(re.findall(r'\b(\w+)\s*\(', text)) | method_names(c)
            items = self.flat_items(c)
            # pads: u8 arrays and single bytes named gap_* / pad* / ... (filled with the spec's members)
            keep = [f for f in fields if not (PAD.match(f['name']) and (f['type'].startswith('std::uint8_t[')
                                                                        or f['type'] == 'std::uint8_t'))]
            pads = [f for f in fields if f not in keep]

            def free(off, nb):
                return all(off + nb <= k['off'] or off >= k['off'] + k['size'] for k in keep)
            items = [it for it in items if free(it[0], it[1])]
            changed = False
            for p in pads:
                inside = [it for it in items if it[0] >= p['off'] and it[0] + it[1] <= p['off'] + p['size']]
                if inside:
                    edits[h['file']].append((p['line'], p['line'], self.fill(p['off'], p['off'] + p['size'], inside, taken)))
                    changed = True
            hend = h['size']
            tail = [it for it in items if it[0] >= hend]
            spec_size = num(self.C[c].get('size')) or 0
            tend = max([hend, spec_size] + [it[0] + it[1] for it in tail])
            if tend > hend:
                last = max(f['line'] for f in fields)
                edits[h['file']].append((last + 1, last, ['    // --- members from re/layout/layouts.json (gen_types.py) ---']
                                         + self.fill(hend, tend, tail, taken)))
                changed = True
            self.size_memo[c] = tend
            fwd = sorted(t for t in self.fwd if t != c and not re.search(rf'^struct {t}\b', text, re.M))
            if fwd:
                edits[h['file']].append((h['sline'], h['sline'] - 1, [f'struct {t};' for t in fwd] + ['']))
            file_incs[h['file']] |= self.member_incs
            if changed:
                merged.append(c)
        for path, incs in file_incs.items():
            lines = open(path).read().split('\n')
            have = {m.group(1) for m in (re.match(r'#include "gen/(\w+)\.hpp"', l) for l in lines) if m}
            add = sorted(self.file[c] for c in incs if self.file[c] not in have)
            if add:
                top = next(i for i, l in enumerate(lines, 1) if l.startswith('namespace '))
                last = max(i for i, l in enumerate(lines[:top], 1) if l.startswith('#include'))
                edits[path].append((last + 1, last, [f'#include "gen/{f}.hpp"' for f in add]))
        for path, es in edits.items():
            lines = open(path).read().split('\n')
            for first, last, repl in sorted(es, key=lambda e: -e[0]):
                lines[first - 1:last] = repl  # 1-based; last < first means insert before `first`
            open(path, 'w').write('\n'.join(lines))
        return merged

    def header(self, c, info):
        s = self.file[c]
        has_methods = os.path.exists(os.path.join(GEN, f'{s}_methods.inc'))
        out = [f'// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):',
               f'// the game\'s {c} class.', '#pragma once', '#include <cstddef>', '#include <cstdint>', '',
               '#include "../../call.hpp"', '#include "../../fn_names.hpp"']
        for i in info['incs']:
            out.append(f'#include "{self.include_of(i)}"')
        out += ['', 'namespace joan::orig {', '']
        fwd = set(info['fwd'])
        if has_methods:  # class types the facade declarations use (pointers / references: forward-declared)
            text = open(os.path.join(GEN, f'{s}_methods.inc')).read()
            known = set(self.file) | set(self.hand)
            fwd |= {n for n in re.findall(r'\b([A-Z]\w*)\s*(?:const\s*)?[*&]', text)
                    if n in known and n != c and n not in info['incs']}
        for f in sorted(fwd):
            out.append(f'struct {f};')
        if fwd:
            out.append('')
        out.append('#pragma pack(push, 1)')
        out.append(f'struct {c}' + (f' : {info["base"]}' if info['base'] else '') + ' {')
        out.append(f'#include "{s}_fields.inc"')
        if has_methods:
            out.append(f'#include "{s}_methods.inc"')
        out += ['};', '#pragma pack(pop)', '']
        if info['asserts'] or info['size']:
            out.append('#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout')
            if info['size']:
                out.append(f'static_assert(sizeof({c}) == 0x{info["size"]:x});')
            for n, off in info['asserts']:
                out.append(f'static_assert(offsetof({c}, {n}) == 0x{off:x});')
            out.append('#endif')
            out.append('')
        out.append('}  // namespace joan::orig')
        open(os.path.join(GEN, f'{s}.hpp'), 'w').write('\n'.join(out) + '\n')

    def include_of(self, c):
        if c in self.hand:
            return '../' + os.path.basename(self.hand[c])
        return f'{self.file[c]}.hpp'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--dry-run', action='store_true')
    a = ap.parse_args()
    lay = json.load(open(os.path.join(HERE, 'layouts.json')))['classes']
    g = Gen(lay)
    FILES.update(g.file)
    todo = [c for c, v in sorted(lay.items()) if g.generated(c) and (v.get('fields') or v.get('members') or v.get('base'))]
    new = [c for c in todo if g.file[c] not in g.facades]
    infos = {}
    if not a.dry_run:
        # hand headers are merged from their hand-written originals (re/layout/hand_orig), so a spec change
        # regenerates the filled-in members instead of keeping the previous generation's
        for p in glob.glob(os.path.join(HERE, 'hand_orig', '*.hpp')):
            open(os.path.join(ORIG, os.path.basename(p)), 'w').write(open(p).read())
        merged = g.merge_hand(hand_fields())
        print(f'hand-typed classes extended from the spec: {len(merged)}: ' + ', '.join(merged))
        for c in todo:
            infos[c] = g.emit_class(c)
        for c in todo:
            g.header(c, infos[c])
        # classes.hpp / accessors.hpp: the new classes
        cl = os.path.join(GEN, 'classes.hpp')
        text = open(cl).read()
        add = [f'#include "{g.file[c]}.hpp"' for c in new if f'#include "{g.file[c]}.hpp"' not in text]
        if add:
            open(cl, 'w').write(text.rstrip('\n') + '\n' + '\n'.join(add) + '\n')
        acc = os.path.join(GEN, 'accessors.hpp')
        text = open(acc).read()
        existing = set(re.findall(r'\bas_(\w+)\(', text))
        for p in glob.glob(os.path.join(ORIG, '*.hpp')):
            existing |= set(re.findall(r'\bas_(\w+)\(', open(p).read()))
        adds = [f'template <class P>\ninline {c}* as_{g.file[c]}(P p) noexcept {{ return object<{c}>(p); }}'
                for c in new if g.file[c] not in existing]
        if adds:
            i = text.rindex('}  // namespace joan::orig')
            text = text[:i] + '\n'.join(adds) + '\n\n' + text[i:]
            open(acc, 'w').write(text)
    hand_diff = [c for c, v in sorted(lay.items()) if c in g.hand and (v.get('fields') or v.get('members'))]
    rep = [f'classes generated: {len(todo)} ({len(new)} new headers); hand-typed (not generated): {len(hand_diff)}: '
           + ', '.join(hand_diff), ''] + g.report
    open(os.path.join(HERE, 'gen_types_report.txt'), 'w').write('\n'.join(rep) + '\n')
    print(rep[0])
    print(f'{len(g.report)} report lines')


if __name__ == '__main__':
    main()
