#!/usr/bin/env python3
"""sample_dump.py — hex + u32/float views of file heads for RE notes (for documenters without a shell).
Usage: sample_dump.py OUT.md [--bytes N] FILE... ; each file: size, hex of the first N bytes, dword/float table."""
import struct, sys, os
args = sys.argv[1:]; out = args.pop(0); n = 512
if args and args[0] == '--bytes': args.pop(0); n = int(args.pop(0))
with open(out, 'w') as o:
    for f in args:
        d = open(f, 'rb').read()
        o.write(f"\n## `{f}` — {len(d)} bytes\n\n```\n")
        for i in range(0, min(n, len(d)), 16):
            chunk = d[i:i+16]
            o.write(f"{i:08x}: {chunk.hex(' '):<48} {''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)}\n")
        o.write("```\n\nFirst dwords (offset: u32 hex | i32 | f32):\n\n```\n")
        for i in range(0, min(n, len(d) - 3), 4):
            u = struct.unpack_from('<I', d, i)[0]; s = struct.unpack_from('<i', d, i)[0]; fl = struct.unpack_from('<f', d, i)[0]
            o.write(f"{i:06x}: {u:#010x} | {s:>11} | {fl:.6g}\n")
        o.write("```\n")
