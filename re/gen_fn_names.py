#!/usr/bin/env python3
"""Generate src/fn_names.hpp: a name for every code address our C++ calls (namespace joan::fn, alias fn).

Usage (from the repository root):
  gen_fn_names.py <thunks.json> <hook dir> <out fn_names.hpp> [--values]

Names:
- every incremental-link thunk (0x00401005 + 5 * i, 8015 of them): the registry name of its target (the name given in
  JOAN_REPLACE* / module registration macros; `Class::method` -> `Class_method`), `fn_<real>` when the target has no
  registered name. The value is the THUNK address: a call through it reaches whatever is installed for the target.
- every real game / library address that src/** calls directly (call helpers, original<>, ORIG, function pointer
  casts, kc_ / named constants that feed calls; see re/fn_scan.py), with --values also every real code entry used as a
  value (callbacks, template arguments, tables; re/fn_scan.py value_is_code), plus the real-address names src/** already uses
  (so regenerating after re/rewrite_fn_names.py keeps them). Same naming; `_body` is appended when a thunk leads to that
  address (the thunk owns the plain name).
Stage S7c-main: each name is a code slot index (`inline constexpr Code Name{index};  // 0xaddress ...`); a name keeps
its index across regenerations, new names take free indices; re/gen_code_slots.py then writes src/runtime/code_slots.inc.
Duplicates: names of different targets that sanitise to the same identifier get the real address as suffix
(`Plane_draw_5d0570`); several thunks of one target get the thunk address as suffix (`Name_t401234`).
Each line documents the real address (and the registered name) in a trailing comment.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fn_scan as F  # noqa: E402

OPS = [('operator new[]', 'op_new_array'), ('operator delete[]', 'op_delete_array'), ('operator new', 'op_new'),
       ('operator delete', 'op_delete'), ('operator[]', 'op_index'), ('operator()', 'op_call'),
       ('operator==', 'op_eq'), ('operator!=', 'op_ne'), ('operator<=', 'op_le'), ('operator>=', 'op_ge'),
       ('operator+=', 'op_add_assign'), ('operator-=', 'op_sub_assign'), ('operator*=', 'op_mul_assign'),
       ('operator/=', 'op_div_assign'), ('operator<<', 'op_shl'), ('operator>>', 'op_shr'), ('operator=', 'op_assign'),
       ('operator+', 'op_add'), ('operator-', 'op_sub'), ('operator*', 'op_mul'), ('operator/', 'op_div'),
       ('operator<', 'op_lt'), ('operator>', 'op_gt'), ('operator!', 'op_not'), ('operator&', 'op_and'),
       ('operator|', 'op_or'), ('operator^', 'op_xor'), ('operator~', 'op_compl')]
KEYWORDS = set('''alignas alignof and and_eq asm auto bitand bitor bool break case catch char char8_t char16_t char32_t
class compl concept const consteval constexpr constinit const_cast continue co_await co_return co_yield decltype
default delete do double dynamic_cast else enum explicit export extern false float for friend goto if inline int long
mutable namespace new noexcept not not_eq nullptr operator or or_eq private protected public register
reinterpret_cast requires return short signed sizeof static static_assert static_cast struct switch template this
thread_local throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor
xor_eq'''.split())


def sanitise(name):
    s = name.strip()
    for op, rep in OPS:
        s = s.replace(op, rep)
    # Class::~Class -> Class_dtor, other ~X -> dtor_X
    s = re.sub(r'(\b\w+)::~\1\b', r'\1::dtor', s)
    s = s.replace('~', 'dtor_')
    s = s.replace('&', '_ref').replace('*', '_ptr')
    s = re.sub(r'[^A-Za-z0-9_]+', '_', s)
    s = re.sub(r'_+', '_', s).strip('_')
    if not s or s[0].isdigit():
        s = 'fn_' + s
    if s in KEYWORDS:
        s += '_'
    return s


def collect_real_calls(hook_dir, existing, entries=None):
    """real (non-thunk) code addresses used as direct call targets in src/** (with entries = --values: also those
    used as values, re/fn_scan.py value_is_code), plus real addresses of the names of an existing fn_names.hpp that
    src/** still uses"""
    reals = set()
    for rel, (path, src, uses) in F.scan_hook(hook_dir).items():
        for u in uses:
            if u.kind == 'call' and not F.is_thunk(u.value) and F.in_code(u.value):
                reals.add(u.value)
            elif entries is not None and F.value_is_code(rel, u, entries):
                reals.add(u.value)
        if existing:
            for m in F.FN_USE_RE.finditer(F.strip(src)):
                v = existing.get(m.group(1))
                if v is not None and not F.is_thunk(v):
                    reals.add(v)
    return reals


def build(thunks, regnames, reals, existing=None):
    """[(name, value, real, registered name or '', kind)] sorted by value; the names of `existing` (name -> value of
    the current fn_names.hpp) are kept, a new name that would clash with one gets the real address as suffix"""
    by_target = {}
    for t, r in thunks.items():
        by_target.setdefault(r, []).append(t)
    entries = []  # [value, real, base, kind]
    for t in sorted(thunks):
        r = thunks[t]
        entries.append([t, r, sanitise(regnames[r]) if r in regnames else 'fn_%06x' % r, 'thunk'])
    for r in sorted(reals):
        entries.append([r, r, sanitise(regnames[r]) if r in regnames else 'fn_%06x' % r, 'real'])
    # different targets with one base name -> real address suffix
    targets_of = {}
    for v, r, b, k in entries:
        targets_of.setdefault(b, set()).add(r)
    for e in entries:
        if len(targets_of[e[2]]) > 1 and not e[2].endswith('_%06x' % e[1]):
            e[2] = '%s_%06x' % (e[2], e[1])
    res = []
    for v, r, b, k in entries:
        if k == 'thunk':
            name = b if len(by_target[r]) == 1 else '%s_t%06x' % (b, v)
        else:
            name = b + '_body' if r in by_target else b
        res.append((name, v, r, regnames.get(r, ''), k))
    keep = {v: n for n, v in (existing or {}).items()}
    kept = set(keep.values())
    res = [(keep[v] if v in keep else (n if n not in kept else '%s_%06x' % (n, r)), v, r, reg, k)
           for n, v, r, reg, k in res]
    names = [n for n, *_ in res]
    dup = {n for n in names if names.count(n) > 1} if len(set(names)) != len(names) else set()
    assert not dup, sorted(dup)[:20]
    return sorted(res, key=lambda e: e[1])


def assign_slots(rows, old_slots, old_order):
    """name -> slot index (fn::Code is the index into the code slot table). A name keeps
    its index (old_slots: name -> index of the current fn_names.hpp; old_order: the names of a pre-S7c-main file in
    file order, numbered in that order); a new name takes the lowest free index."""
    slots = dict(old_slots)
    if not slots:
        slots = {n: i for i, n in enumerate(old_order)}
    names = {r[0] for r in rows}
    slots = {n: i for n, i in slots.items() if n in names}
    used = set(slots.values())
    free = (i for i in range(10 ** 7) if i not in used)
    for r in rows:
        if r[0] not in slots:
            slots[r[0]] = next(free)
    return slots


def file_order(path):
    """names of an fn_names.hpp in file order (pre-S7c-main form included)"""
    res = []
    if path and os.path.exists(path):
        for ln in open(path):
            m = F.CODE_LINE.match(ln) or F.NAME_LINE.match(ln)
            if m:
                res.append(m.group(1))
    return res


def main():
    tjson, hook_dir, out = [a for a in sys.argv[1:] if not a.startswith('--')][:3]
    thunks = F.load_thunks(tjson)
    regnames = F.registry_names(hook_dir)
    current = os.path.join(hook_dir, 'fn_names.hpp')
    existing = F.load_fn_names(current)
    existing.update(F.load_fn_names(out))
    old_slots = {n: s for n, (s, _) in F.load_fn_slots(current).items()}
    old_slots.update({n: s for n, (s, _) in F.load_fn_slots(out).items()})
    reals = collect_real_calls(hook_dir, existing, F.load_code_entries(hook_dir) if '--values' in sys.argv else None)
    rows = build(thunks, regnames, reals, existing)
    slots = assign_slots(rows, old_slots, file_order(current) or file_order(out))
    with open(out, 'w', newline='\n') as o:
        o.write('// Generated by re/gen_fn_names.py - do not edit. Regenerate (from the repository root), then\n'
                '// re/gen_code_slots.py (src/runtime/code_slots.inc numbers its slots after these):\n'
                '//   python3 re/gen_fn_names.py <thunks.json> hook src/fn_names.hpp --values\n'
                '//\n'
                '// Names of the joan.exe code addresses our C++ calls or uses as values (callbacks, template arguments, tables).\n'
                '// Each name is a code SLOT (fn::Code, src/runtime/code.hpp; docs/RUNTIME.md): the value is the index into\n'
                '// joan_code[] (src/runtime/code_table.cpp), which holds what a call of the original address reaches once the\n'
                '// replacements are installed (a C++ replacement, or a trap stub: "original code needed"). The comment starts with\n'
                '// the original address the name stands for (its ID: registry keys, logs; fn::va()):\n'
                '// - thunks (0x00401005 + 5 * i): incremental-link `jmp` stubs of joan.exe; the slot reaches whatever is installed\n'
                '//   for the thunk\'s target. Comment: `0xthunk -> real address  registered name`.\n'
                '// - real addresses called directly or used as values (library entries, original bodies, CRT initialisers,\n'
                '//   callbacks). Comment: `0xaddress game|library  registered name`;\n'
                '//   `_body` = a thunk leads to the same function and owns the plain name.\n'
                '// Names come from the registry (JOAN_REPLACE* / module macros); `fn_<real>` when nothing registers the\n'
                '// target; duplicates get `_<real>` (different targets) or `_t<thunk>` (several thunks of one target).\n'
                '// A name keeps its slot index when the file is regenerated (new names take free indices).\n'
                '// Typed (fn::Code): no conversion to integers; calls go through src/call.hpp, values through fn::code_value.\n'
                '// re/gen_code_slots.py, re/fn_scan.py read this file: a name used in src/** counts like the address it stands for.\n'
                '#pragma once\n\n#include "code.hpp"\n\nnamespace joan::fn {\n\n')
        o.write('// ---- incremental-link thunks (%d)\n' % sum(1 for r in rows if r[4] == 'thunk'))
        for n, v, r, reg, k in rows:
            if k == 'thunk':
                o.write('inline constexpr Code %s{%d};  // 0x%08x -> 0x%08x%s\n' % (
                    n, slots[n], v, r, ('  ' + reg) if reg else ''))
        o.write('\n// ---- real addresses called directly or used as values (%d)\n' % sum(1 for r in rows if r[4] == 'real'))
        for n, v, r, reg, k in rows:
            if k == 'real':
                o.write('inline constexpr Code %s{%d};  // 0x%08x %s%s\n' % (
                    n, slots[n], v, 'game' if v < F.GAME_HI else 'library', ('  ' + reg) if reg else ''))
        o.write('\n}  // namespace joan::fn\n\nnamespace fn = joan::fn;\n')
    print('%d names (%d thunks, %d real; slots 0..%d) -> %s' % (len(rows), sum(1 for r in rows if r[4] == 'thunk'),
                                                              sum(1 for r in rows if r[4] == 'real'),
                                                              max(slots.values()), out))


if __name__ == '__main__':
    main()
