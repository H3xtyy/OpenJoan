#!/usr/bin/env python3
"""Replace the numeric data addresses of src/** C++ with the names of src/data_names.hpp (re/gen_data_names.py).
Behaviour-preserving: every name has exactly the value of the literal it replaces.

Usage (from the repository root):
  rewrite_data_names.py <hook dir> [--dry-run] [--report out.tsv]

Rewritten: every hex literal that is a data address (.rdata / .data / .bss object start, IAT slot) in C++ code -
global<T>(...), obj(...), str(...), at<>/ptr<> helpers, import_call / stdcall_import slots, reinterpret_cast<T*>(...),
kName = ... constants and other assignments, template arguments, tables, call arguments.
Left alone: comments and string literals, __asm blocks (an MSVC inline-asm operand cannot be a C++ constant),
registration keys and macro token arguments (re/fn_scan.py macro_keys: JOAN_REPLACE* address / IAT slot, module macro
arguments that are stringized, pasted or used in __asm, like S(name, stub, IAT slot) in lib/imports/import_stubs.cpp; the other
arguments of those macros - &obj_call<0x00a3ce90u, ...> - are named), #if lines, and the generated files (src/data/,
loader/data_image.*, import_table.inc, image_layout.inc, code_slots.inc, fn_names.hpp,
data_names.hpp).
Files that get a name also get `#include "<rel>/data_names.hpp"` (it defines the alias `d` = joan::d); files that use
__LINE__ keep their line numbers (the blank line after their include block is dropped).
Since S7b-prep the names are typed (img::Sym<T>, src/runtime/image.hpp); re/rewrite_image_types.py and the compiler take the
code from there (helpers, casts, template parameters, stores).
"""
import collections
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fn_scan as F  # noqa: E402
import gen_data_names as G  # noqa: E402

HELPER = re.compile(r'([A-Za-z_][\w:]*)\s*(<(?:[^<>;]|<[^<>;]*>)*>)?\s*\(\s*$')
INC_RE = re.compile(r'\s*#\s*include\s+"([^"]*)"')


def category(code, s, v, imports):
    """report category of the literal at code[s]"""
    before = code[max(0, s - 200):s]
    region = 'iat' if v in imports else 'rdata' if v < 0x9f4000 else 'data' if v < 0xa34000 else 'bss'
    m = HELPER.search(before)
    if m:
        h = m.group(1).split('::')[-1]
        if h in ('global', 'obj', 'str', 'reinterpret_cast', 'static_cast', 'import_call', 'stdcall_import'):
            return region, h
        return region, 'helper'
    if re.search(r'(?<![=!<>])=\s*$', before):
        return region, 'assign'
    if re.search(r'<\s*$|<[^<>;()]*,\s*$', before):
        return region, 'template_arg'
    if re.search(r'[{,]\s*$', before):
        return region, 'list'
    return region, 'other'


def add_include(src, rel, header):
    """src with #include of <header> (at the hook root) added to its first block of quoted includes (alphabetical
    position), or after the angled includes. Files that use __LINE__ in code keep their line numbers: the blank line
    that follows their include block is dropped."""
    inc = '../' * rel.count('/') + header
    line = '#include "%s"' % inc
    lines = src.split('\n')
    quoted = [i for i, ln in enumerate(lines) if INC_RE.match(ln)]
    angled = [i for i, ln in enumerate(lines) if re.match(r'\s*#\s*include\s+<', ln)]
    if '__LINE__' in F.strip(src):
        last = max(quoted[-1:] + angled[-1:]) if (quoted or angled) else None
        if last is None or last + 1 >= len(lines) or lines[last + 1].strip() != '':
            raise RuntimeError('%s: uses __LINE__ and has no blank line after its includes' % rel)
        del lines[last + 1]
    if quoted:
        block = [quoted[0]]
        for i in quoted[1:]:
            if i == block[-1] + 1:
                block.append(i)
            else:
                break
        pos = block[-1] + 1
        for i in block:
            if INC_RE.match(lines[i]).group(1) > inc:
                pos = i
                break
        lines.insert(pos, line)
    elif '__LINE__' in F.strip(src):
        lines.insert(angled[-1] + 1, line)
    else:
        if not angled:
            raise RuntimeError('%s: no #include to anchor %s' % (rel, header))
        lines[angled[-1] + 1:angled[-1] + 1] = ['', line]
    return '\n'.join(lines)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    hook_dir = args[0]
    dry = '--dry-run' in sys.argv
    report = sys.argv[sys.argv.index('--report') + 1] if '--report' in sys.argv else None
    names = F.load_data_names(os.path.join(hook_dir, 'data_names.hpp'))
    by_value = {v: n for n, v in names.items()}
    assert len(by_value) == len(names)
    imports = G.load_imports(hook_dir)
    stats = collections.Counter()
    files = collections.Counter()
    rows = []
    for rel, path in G.hook_sources(hook_dir):
        if os.path.basename(rel) in ('code_map.inc', 'code_slots.inc', 'census_table.inc', 'import_table.inc', 'image_layout.inc'):
            continue
        src = open(path, errors='replace', newline='').read()
        code = F.strip(src)
        reps = []
        for s, e, v, kind in G.scan_source(src, imports):
            ok = kind == 'code' and v in by_value
            if kind == 'code':
                region, cat = category(code, s, v, imports)
                if not ok:
                    cat += '_unnamed'
            else:
                region, cat = ('iat' if v in imports else 'image'), kind
            stats[(region, cat, ok)] += 1
            ln = src.count('\n', 0, s) + 1
            text = src[src.rfind('\n', 0, s) + 1:src.find('\n', s) if src.find('\n', s) >= 0 else len(src)]
            rows.append((rel, ln, '0x%08x' % v, region, cat, 'named' if ok else 'kept', text.strip()))
            if ok:
                reps.append((s, e, v))
        if not reps:
            continue
        files[rel] = len(reps)
        out = src
        for s, e, v in sorted(reps, key=lambda r: -r[0]):
            out = out[:s] + 'd::' + by_value[v] + out[e:]
        if not re.search(r'#\s*include\s+"(?:\.\./)*data_names\.hpp"', out):
            out = add_include(out, rel, 'data_names.hpp')
        if not dry:
            with open(path, 'w', newline='') as o:
                o.write(out)
    for (region, cat, ok), n in sorted(stats.items()):
        print('  %-6s %-22s %-6s %6d' % (region, cat, 'named' if ok else 'kept', n))
    print('%d files %s, %d literals named' % (len(files), 'would change' if dry else 'changed', sum(files.values())))
    if report:
        with open(report, 'w') as o:
            for r in rows:
                o.write('\t'.join(str(x) for x in r) + '\n')


if __name__ == '__main__':
    main()
