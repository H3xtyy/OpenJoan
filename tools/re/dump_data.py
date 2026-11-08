#!/usr/bin/env python3
"""dump_data.py — print what lives at virtual addresses of a PE (for RE notes): C strings, dword/float tables.

Usage: dump_data.py EXE VA[:N] ...    (VA hex; N = number of dwords to show as a table, default: C string)
"""
from __future__ import annotations
import struct, sys
import pefile

def main() -> None:
    pe = pefile.PE(sys.argv[1], fast_load=True); d = open(sys.argv[1], 'rb').read()
    base = pe.OPTIONAL_HEADER.ImageBase
    def off(va):
        for s in pe.sections:
            v = base + s.VirtualAddress
            if v <= va < v + s.SizeOfRawData:
                return s.PointerToRawData + va - v
        return None
    for arg in sys.argv[2:]:
        va_s, _, n = arg.partition(':')
        va = int(va_s, 16); o = off(va)
        if o is None:
            print(f"{va:#x}: not in file (bss?)"); continue
        if n:
            words = [struct.unpack_from('<I', d, o + 4 * i)[0] for i in range(int(n))]
            flts = [struct.unpack_from('<f', d, o + 4 * i)[0] for i in range(int(n))]
            print(f"{va:#x}: dwords " + ' '.join(f"{w:#010x}" for w in words))
            print(f"{'':>12} floats " + ' '.join(f"{f:.4g}" for f in flts))
            print(f"{'':>12} bytes  " + d[o:o + 4 * int(n)].hex(' '))
        else:
            end = d.index(b'\0', o)
            print(f"{va:#x}: {d[o:end][:200]!r}")

if __name__ == '__main__':
    main()
