"""Minimal PE32 parser (no external deps) used by the analysis scripts."""
import struct


class PE:
    def __init__(self, path):
        self.data = open(path, 'rb').read()
        d = self.data
        pe = struct.unpack_from('<I', d, 0x3C)[0]
        assert d[pe:pe + 4] == b'PE\0\0'
        nsec, = struct.unpack_from('<H', d, pe + 6)
        optsz, = struct.unpack_from('<H', d, pe + 20)
        opt = pe + 24
        self.entry, = struct.unpack_from('<I', d, opt + 16)
        self.base, = struct.unpack_from('<I', d, opt + 28)
        self.dirs = [struct.unpack_from('<II', d, opt + 96 + 8 * i) for i in range(16)]
        self.sections = []
        so = opt + optsz
        for i in range(nsec):
            name = d[so:so + 8].rstrip(b'\0').decode()
            vsize, va, rsize, roff = struct.unpack_from('<IIII', d, so + 8)
            self.sections.append((name, va, vsize, roff, rsize))
            so += 40
        self.entry += self.base

    def rva2off(self, rva):
        for name, va, vs, ro, rs in self.sections:
            if va <= rva < va + max(vs, rs):
                return ro + rva - va
        return None

    def va2off(self, va):
        return self.rva2off(va - self.base)

    def section(self, name):
        for s in self.sections:
            if s[0] == name:
                return s
        return None

    def u32(self, va):
        return struct.unpack_from('<I', self.data, self.va2off(va))[0]

    def cstr(self, va, maxlen=512):
        o = self.va2off(va)
        if o is None:
            return None
        e = self.data.find(b'\0', o, o + maxlen)
        if e < 0:
            return None
        return self.data[o:e]

    def imports(self):
        """Return dict IAT-slot VA -> 'DLL!Name'."""
        res = {}
        rva, size = self.dirs[1]
        o = self.rva2off(rva)
        d = self.data
        while True:
            oft, ts, fwd, name, ft = struct.unpack_from('<IIIII', d, o)
            if name == 0:
                break
            dll = self.cstr(self.base + name).decode()
            thunk = oft or ft
            i = 0
            while True:
                v = struct.unpack_from('<I', d, self.rva2off(thunk) + 4 * i)[0]
                if v == 0:
                    break
                if v & 0x80000000:
                    fn = 'ord%d' % (v & 0xffff)
                else:
                    fn = self.cstr(self.base + v + 2).decode()
                res[self.base + ft + 4 * i] = dll + '!' + fn
                i += 1
            o += 20
        return res

    def relocs(self):
        """Return set of VAs that hold absolute pointers (from .reloc)."""
        rva, size = self.dirs[5]
        o = self.rva2off(rva)
        end = o + size
        out = set()
        d = self.data
        while o < end:
            page, bsz = struct.unpack_from('<II', d, o)
            if bsz == 0:
                break
            for i in range((bsz - 8) // 2):
                e, = struct.unpack_from('<H', d, o + 8 + 2 * i)
                if e >> 12 == 3:
                    out.add(self.base + page + (e & 0xfff))
            o += bsz
        return out
