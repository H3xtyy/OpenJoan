#!/usr/bin/env python3
"""Stage S7 exit checks (docs/RUNTIME.md): which fixed joan.exe addresses are still literals in src/** C++.

Usage (from the repository root):
  check_s7.py code [hook dir] [--list]   code-range literals (0x00401000..0x007e50a0)
  check_s7.py data [hook dir] [--list]   data-image literals (.rdata / .data / .bss 0x009cf000..0x00a6cd44, IAT slots)
Exit status 1 when a literal is left outside the allowed contexts (printed with file:line); --list also prints the
allowed ones.

code (S7a-tail): allowed are
  reg       registration keys: JOAN_REPLACE* address arguments and the key / token arguments of module macros
            (re/fn_scan.py macro_keys) - the registry is keyed by original address
  id        IDs, never called or stored as code: the lookup / patch tables of canvas_hooks.cpp / display.cpp /
            fixes.cpp, the range bounds of patch / code_table / loader / crash (re/fn_scan.py ID_FILES, BOUNDS),
            and constants that are not code entries of src/runtime/code_slots.inc (canvas size-read / immediate instruction
            addresses, mid-function return addresses, numbers that happen to lie in the range)
  asm       immediates inside __asm (S7c: asm is rewritten with the code slot table)
  pp_if     #if lines
everything else must be an fn:: name (re/rewrite_fn_names.py --values).

data (S7b-prep): allowed are
  reg       registration keys (IAT slots of JOAN_REPLACE_IMPORT, module macro token arguments such as the
            IAT slot that lib/imports/import_stubs.cpp's S() pastes into __asm)
  pp_if     #if lines
  (__asm operands are no longer allowed since S7b-main: re/asm_rebase.py wrote them as `[joan_image + offset]`)
everything else must be a d:: name (re/rewrite_data_names.py), and the raw overloads of src/runtime/image.hpp
(global<T> / obj / str of a computed address) never take an address literal (>= 0x00400000; small resource IDs
such as str(0x66) are fine). Generated files are not scanned (src/data/,
image_layout.inc, import_table.inc, code_slots.inc, fn_names.hpp, data_names.hpp).
"""
import collections
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fn_scan as F  # noqa: E402
import gen_data_names as G  # noqa: E402

GENERATED = {'code_map.inc', 'code_slots.inc', 'census_table.inc', 'import_table.inc', 'image_layout.inc', 'fn_names.hpp',
             'data_names.hpp'}


def check_code(hook_dir):
    entries = F.load_code_entries(hook_dir)
    rows = []
    for rel, (path, src, uses) in F.scan_hook(hook_dir).items():
        if os.path.basename(rel) in GENERATED:
            continue
        for u in uses:
            if u.kind in ('reg', 'asm', 'pp_if'):
                cat = u.kind
            elif u.kind == 'call' or F.is_thunk(u.value) and rel not in F.KEY_FILES:
                cat = 'LITERAL'            # a call target / thunk value that rewrite_fn_names.py names
            elif F.is_thunk(u.value) or not F.in_code(u.value) or not F.value_is_code(rel, u, entries):
                cat = 'id'
            else:
                cat = 'LITERAL'
            rows.append((cat, rel, u.line, u.value, u.text.strip()))
    return rows


RAW_CALL = re.compile(r'\b(?:global\s*<[^;()]*>|obj|str)\s*\(\s*0x([0-9a-fA-F]+)')


def check_data(hook_dir):
    imports = G.load_imports(hook_dir)
    rows = []
    for rel, path in G.hook_sources(hook_dir):
        if os.path.basename(rel) in GENERATED:
            continue
        src = open(path, errors='replace', newline='').read()
        # the raw (computed address) overloads of image.hpp with a literal of the old image / header range
        code = F.strip(src)
        for m in RAW_CALL.finditer(code):
            v = int(m.group(1), 16)
            if v >= 0x400000:
                ln = src.count('\n', 0, m.start()) + 1
                rows.append(('LITERAL', rel, ln, v, 'raw helper with an address literal: ' + m.group(0)))
        for s, e, v, kind in G.scan_source(src, imports):
            cat = kind if kind in ('reg', 'pp_if') else 'LITERAL'
            ln = src.count('\n', 0, s) + 1
            a = src.rfind('\n', 0, s) + 1
            b = src.find('\n', s)
            rows.append((cat, rel, ln, v, src[a:b if b >= 0 else len(src)].strip()))
    return rows


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    if not args or args[0] not in ('code', 'data'):
        sys.exit(__doc__)
    hook_dir = args[1] if len(args) > 1 else 'src'
    rows = check_code(hook_dir) if args[0] == 'code' else check_data(hook_dir)
    count = collections.Counter(r[0] for r in rows)
    files = collections.defaultdict(set)
    for r in rows:
        files[r[0]].add(r[1])
    for cat in sorted(count):
        print('%-8s %6d literals in %3d files' % (cat, count[cat], len(files[cat])))
    bad = [r for r in rows if r[0] == 'LITERAL']
    for cat, rel, ln, v, text in rows:
        if cat == 'LITERAL' or '--list' in sys.argv:
            print('%s %s:%d 0x%08x  %s' % (cat, rel, ln, v, text[:120]))
    print('%s: %d literal%s outside the allowed contexts' % (args[0], len(bad), '' if len(bad) == 1 else 's'))
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
