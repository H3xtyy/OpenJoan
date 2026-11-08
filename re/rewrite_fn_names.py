#!/usr/bin/env python3
"""Replace the numeric function addresses of src/** C++ with the names of src/fn_names.hpp (re/gen_fn_names.py).
Behaviour-preserving: every name has exactly the value of the literal it replaces.

Usage (from the repository root):
  rewrite_fn_names.py <hook dir> [--values] [--dry-run] [--report out.tsv]

Rewritten (re/fn_scan.py classifies every hex literal in 0x00401000..0x007e50a0 outside comments / strings):
- thunk addresses (0x00401005 + 5 * i): every use (call target or value: tables, callbacks, wrapper arguments),
  except registration macro arguments, __asm blocks, `#if` lines, and non-call uses in canvas_hooks.cpp /
  display.cpp / fixes.cpp (lookup keys there).
- real game / library addresses: only as a direct call target (call helpers, original<>, ORIG, function pointer casts
  called right away, kc_ / named constants used as call targets).
- --values (stage S7a-tail): also real addresses used as values - callbacks (atexit / qsort / CreateThread ...),
  template arguments (init_pair<a, b>, reg_atexit<a> ...), tables, constants - when they are code entries of
  src/runtime/code_slots.inc (re/fn_scan.py value_is_code). gen_fn_names.py --values names those addresses first.
Left alone: registration keys (registry macros and the key / token arguments of module macros, re/fn_scan.py
macro_keys), __asm operands (an MSVC inline-asm identifier is a memory operand, not an immediate), IDs (lookup
tables of canvas_hooks.cpp / display.cpp / fixes.cpp, range bounds, instruction addresses that are not code entries:
canvas size reads, immediates, return addresses), data. re/check_s7.py code lists what is left.
Files that get a name also get `#include "<rel>/fn_names.hpp"` (it defines the alias `fn` = joan::fn).
"""
import collections
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fn_scan as F  # noqa: E402


def decide(rel, u, entries=None):
    """(replace?, category) for a Use; entries (code_slots.inc entry addresses) given = --values"""
    if u.kind in ('reg', 'asm', 'pp_if'):
        return False, u.kind
    if F.is_thunk(u.value):
        if rel in F.KEY_FILES and u.kind != 'call':
            return False, 'thunk_key'
        return True, 'thunk_' + (u.ctx if u.kind == 'call' else 'value')
    if not F.in_code(u.value):
        return False, 'not_code'
    region = 'game' if u.value < F.GAME_HI else 'lib'
    if u.kind == 'call':
        return True, '%s_%s' % (region, u.ctx)
    if entries is not None:
        if F.value_is_code(rel, u, entries):
            return True, region + '_value'
        return False, region + ('_id_file' if rel in F.ID_FILES or u.value in F.BOUNDS else '_id_not_entry')
    return False, region + '_value'


def add_include(src, rel):
    """src with #include of fn_names.hpp added to its quoted include block (or after the last include).
    Files that use __LINE__ in code (debug allocator arguments, registrar names) keep their line numbers: the include
    takes the place of the blank line before the quoted include block."""
    depth = rel.count('/')
    inc = '../' * depth + 'fn_names.hpp'
    line = '#include "%s"' % inc
    lines = src.split('\n')
    quoted = [i for i, ln in enumerate(lines) if re.match(r'\s*#\s*include\s+"', ln)]
    if '__LINE__' in F.strip(src):
        if quoted and quoted[0] > 0 and lines[quoted[0] - 1].strip() == '':
            lines[quoted[0] - 1] = line
            return '\n'.join(lines)
        raise RuntimeError('%s: uses __LINE__ and has no blank line before its quoted includes' % rel)
    if quoted:
        # alphabetical position among the first block of quoted includes
        block = [quoted[0]]
        for i in quoted[1:]:
            if i == block[-1] + 1:
                block.append(i)
            else:
                break
        pos = block[-1] + 1
        for i in block:
            if re.match(r'\s*#\s*include\s+"([^"]*)"', lines[i]).group(1) > inc:
                pos = i
                break
        lines.insert(pos, line)
    else:
        angled = [i for i, ln in enumerate(lines) if re.match(r'\s*#\s*include\s+<', ln)]
        if not angled:
            raise RuntimeError('%s: no #include to anchor fn_names.hpp' % rel)
        lines[angled[-1] + 1:angled[-1] + 1] = ['', line]
    return '\n'.join(lines)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    hook_dir = args[0]
    dry = '--dry-run' in sys.argv
    report = sys.argv[sys.argv.index('--report') + 1] if '--report' in sys.argv else None
    entries = F.load_code_entries(hook_dir) if '--values' in sys.argv else None
    names = F.load_fn_names(os.path.join(hook_dir, 'fn_names.hpp'))
    by_value = {v: n for n, v in names.items()}
    assert len(by_value) == len(names)
    stats = collections.Counter()
    files = collections.Counter()
    rows = []
    for rel, (path, src, uses) in F.scan_hook(hook_dir).items():
        reps = []
        for u in uses:
            ok, cat = decide(rel, u, entries)
            if ok and u.value not in by_value:
                ok, cat = False, cat + '_unnamed'
            stats[(cat, ok)] += 1
            rows.append((rel, u.line, '0x%08x' % u.value, cat, 'named' if ok else 'kept', u.text.strip()))
            if ok:
                reps.append(u)
        if not reps:
            continue
        files[rel] = len(reps)
        out = src
        for u in sorted(reps, key=lambda u: -u.start):
            out = out[:u.start] + 'fn::' + by_value[u.value] + out[u.end:]
        if not re.search(r'#\s*include\s+"(?:\.\./)*fn_names\.hpp"', out):
            out = add_include(out, rel)
        if not dry:
            with open(path, 'w', newline='') as o:
                o.write(out)
    for (cat, ok), n in sorted(stats.items()):
        print('  %-24s %-6s %6d' % (cat, 'named' if ok else 'kept', n))
    print('%d files %s, %d literals named' % (len(files), 'would change' if dry else 'changed', sum(files.values())))
    if report:
        with open(report, 'w') as o:
            for r in rows:
                o.write('\t'.join(str(x) for x in r) + '\n')


if __name__ == '__main__':
    main()
