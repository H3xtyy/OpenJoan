#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""x64 scan (Phase 2, skill oj-x64-port): pointers narrowed to 32 bits at a call site with `joan::orig::arg::as<u32>(p)` /
`arg::as<std::uint32_t>(p)` / `arg::u32(p)` / `static_cast<u32>(reinterpret_cast<uintptr_t>(p))`, where p is a local,
parameter or member declared with a pointer type in the same file (the declaration is looked up by name above the use).
Such a value is fine on x86 and in a replay (the deterministic heap lies below 4 GB) and truncates a live x64 pointer
(texturea.cpp add_sorted(arg::as<u32>(texture)) was found after Unit::set_group crashed the owner's mission start).

  python3 tools/x64_scan_ptr_as_u32.py [paths...]      (default: src, without src/original/gen and src/data)

A scan is advisory: every hit is checked against the code; a fix is proven with objcmp / the replay gate (CLAUDE.md 5).
"""
import re, sys, pathlib
roots = [pathlib.Path(p) for p in sys.argv[1:]] or [pathlib.Path('src')]
narrow = re.compile(r'(?:arg::as<(?:std::)?u(?:int32_t|32)>|arg::u32|static_cast<(?:std::)?u(?:int32_t|32)>\(\s*reinterpret_cast<(?:std::)?uintptr_t>)\(\s*([A-Za-z_]\w*)\s*\)')
decl_tpl = r'(?:^|[\s(,])((?:const\s+)?(?:[A-Za-z_][\w:]*)\s*\*+\s*(?:const\s+)?){NAME}\b(?:\s*[=;,)]|\s*\[)'  # `T* name` as a declaration (a `d * n` product also matches: check the hit)
hits = 0
for root in roots:
    for f in sorted(root.rglob('*.cpp')):
        s = str(f)
        if '/original/gen/' in s or '/data/' in s: continue
        lines = f.read_text(errors='replace').split('\n')
        for i, l in enumerate(lines):
            for m in narrow.finditer(l):
                name = m.group(1)
                if name in ('this',):
                    print(f'{s}:{i+1}: this narrowed | {l.strip()[:120]}'); hits += 1; continue
                decl = re.compile(decl_tpl.format(NAME=re.escape(name)))
                lo = max(0, i - 80)
                for j in range(i, lo, -1):
                    if j < i and re.match(r'\s*(case\s+-?\w+:|default:)', lines[j]): break  # another switch arm: other locals
                    if j < i and re.match(r'\S', lines[j]) and lines[j].rstrip().endswith('{'): break  # a new top-level definition
                    if decl.search(lines[j]):
                        print(f'{s}:{i+1}: {name} is {decl.search(lines[j]).group(1).strip()} (line {j+1}) | {l.strip()[:110]}'); hits += 1
                        break
print(hits, 'hits', file=sys.stderr)
