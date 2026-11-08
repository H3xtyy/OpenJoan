"""Disassembler for D3D8 vs.1.0 / vs.1.1 byte code (as dumped by the hook's D3D8 trace)."""
import struct, sys

OPS = {0:('nop',0),1:('mov',2),2:('add',3),3:('sub',3),4:('mad',4),5:('mul',3),6:('rcp',2),7:('rsq',2),
       8:('dp3',3),9:('dp4',3),10:('min',3),11:('max',3),12:('slt',3),13:('sge',3),14:('exp',2),15:('log',2),
       16:('lit',2),17:('dst',3),18:('lrp',4),19:('frc',2),20:('m4x4',3),21:('m4x3',3),22:('m3x4',3),
       23:('m3x3',3),24:('m3x2',3),78:('expp',2),79:('logp',2)}
RT = {0:'r',1:'v',2:'c',3:'a',4:'o',5:'oD',6:'oT'}
RAST = {0:'oPos',1:'oFog',2:'oPts'}
DT = {0:'float1',1:'float2',2:'float3',3:'float4',4:'d3dcolor',5:'ubyte4',6:'short2',7:'short4'}

def reg(t, dst):
    rt = (t >> 28) & 7; n = t & 0x7ff
    if rt == 4: name = RAST.get(n, 'o?%d' % n)
    elif rt == 3: name = 'a0'
    else: name = RT[rt] + str(n)
    if not dst and t & 0x2000: name = name[0] + '[a0.x + %d]' % n
    if dst:
        m = (t >> 16) & 0xf
        if m != 0xf: name += '.' + ''.join(c for i, c in enumerate('xyzw') if m >> i & 1)
    else:
        sw = (t >> 16) & 0xff
        if sw != 0xe4:
            s = ''.join('xyzw'[(sw >> (2 * i)) & 3] for i in range(4))
            name += '.' + (s[0] if len(set(s)) == 1 else s)
        if (t >> 24) & 0xf == 1: name = '-' + name
    return name

def disasm(words):
    out = ['vs.%d.%d' % ((words[0] >> 8) & 0xff, words[0] & 0xff)]
    i = 1
    while i < len(words):
        t = words[i]
        if t == 0x0000ffff: break
        op = t & 0xffff
        if op == 0xfffe:  # comment
            n = (t >> 16) & 0x7fff
            out.append('; comment (%d dwords) %r' % (n, struct.pack('<%dI' % n, *words[i+1:i+1+n])))
            i += 1 + n; continue
        if op == 81:  # def
            vals = struct.unpack('<4f', struct.pack('<4I', *words[i+2:i+6]))
            out.append('def %s, %s' % (reg(words[i+1], True), ', '.join('%g' % v for v in vals)))
            i += 6; continue
        name, n = OPS[op]
        ops = [reg(words[i+1], True)] + [reg(w, False) for w in words[i+2:i+1+n]]
        out.append('%-5s %s' % (name, ', '.join(ops)))
        i += 1 + n
    return out

def decl(words):
    out = []
    for t in words:
        kind = t >> 29
        if t == 0xffffffff: out.append('end'); break
        if kind == 1: out.append('stream %d' % (t & 0xf))
        elif kind == 2: out.append('v%d: %s' % (t & 0x1f, DT[(t >> 16) & 0xf]))
        elif kind == 0 and t == 0: out.append('stream 0')
        else: out.append('token %08x' % t)
    return out

if __name__ == '__main__':
    for path in sys.argv[1:]:
        data = open(path, 'rb').read()
        words = list(struct.unpack('<%dI' % (len(data) // 4), data))
        print('==', path)
        print('\n'.join(decl(words) if path.endswith('_decl.bin') else disasm(words)))
