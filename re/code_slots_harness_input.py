#!/usr/bin/env python3
"""Inputs of re/code_slots_harness.cpp (stage S7c decision-equivalence harness).

Usage (from the repository root):
  code_slots_harness_input.py <joan_re.log> <out dir>
writes <out dir>/registrations.txt (the registry in install order as the log's install_all lines show it: address,
installed, IAT slot, function id), canvas.txt (the addresses canvas_hooks.cpp wraps) and relocs.txt (the code
relocation values of src/data/data_relocs.inc).
The log must be of one run (the first install_all block is used). Function ids: one per registration (the log does not
say which registrations share a C++ function; the harness's random runs share some on purpose).
"""
import os
import re
import sys

LINE = re.compile(r'^\d\d:\d\d:\d\d\.\d{3} (replace|skip   |FAILED ) (.*) 0x([0-9a-f]{8}) \((.*?)\)')


def main():
    log, out = sys.argv[1:3]
    os.makedirs(out, exist_ok=True)
    hook = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'src')
    regs = []
    started = False
    for ln in open(log, errors='replace'):
        m = LINE.match(ln)
        if not m:
            if started and 'replacements installed' in ln:
                break
            continue
        started = True
        kind, addr, how = m.group(1), int(m.group(3), 16), m.group(4)
        regs.append((addr, 1 if kind == 'replace' else 0, 1 if 'IAT slot' in how else 0))
    with open(os.path.join(out, 'registrations.txt'), 'w') as o:
        for i, (a, inst, iat) in enumerate(regs):
            o.write('%08x %d %d %d\n' % (a, inst, iat, i))
    src = open(os.path.join(hook, 'canvas_hooks.cpp')).read()
    screens = src[src.index('constexpr Screen kScreens[] = {'):]
    screens = screens[:screens.index('\n};')]
    canvas = [int(h, 16) for h in re.findall(r'\{"[^"]*", 0x([0-9a-f]{8}),', screens)]
    for name in ('kQueueSprite', 'kSetViewport', 'kPlaceModel'):  # wrap order of install_canvas_hooks
        canvas.append(int(re.search(r'constexpr std::uintptr_t %s = 0x([0-9a-f]{8});' % name, src).group(1), 16))
    with open(os.path.join(out, 'canvas.txt'), 'w') as o:
        o.write(''.join('%08x\n' % a for a in canvas))
    relocs = re.findall(r'JOAN_DATA_RELOC\(0x[0-9a-f]+u, code, 0x([0-9a-f]+)u\)',
                        open(os.path.join(hook, 'data', 'data_relocs.inc')).read())
    with open(os.path.join(out, 'relocs.txt'), 'w') as o:
        o.write(''.join('%s\n' % v for v in relocs))
    print('%d registrations (%d installed, %d IAT), %d canvas wrappers, %d code relocations -> %s' % (
        len(regs), sum(r[1] for r in regs), sum(r[2] for r in regs), len(canvas), len(relocs), out))


if __name__ == '__main__':
    main()
