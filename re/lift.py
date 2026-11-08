#!/usr/bin/env python3
"""Lifts an MSVC 6 /Od function of joan.exe into goto-structured pseudo C.

usage: lift.py hexaddr            (run in the folder holding joan.exe, joan.asm,
                                   thunks.json and imports.json - see analyze.py)

Built for en3d's Direct3D state code: every basic block is evaluated
symbolically (MSVC /Od keeps nothing in registers across blocks), pushes are
collected for calls, device calls are named from the IDirect3DDevice8 vtable in
src/d3d8/d3d8_api.hpp, thunks are resolved and switch jump tables read from
the image. The output is a reading aid for hand-written C++ (tocpp.py names the
render / texture-stage states), not compilable code.
"""
import json
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
WORK = os.getcwd()
sys.path.insert(0, HERE)
from pe import PE  # noqa: E402

pe = PE(os.path.join(WORK, 'joan.exe'))
thunks = json.load(open(os.path.join(WORK, 'thunks.json')))
imports = json.load(open(os.path.join(WORK, 'imports.json')))


def _device_methods():
    """(name, parameter count) per IDirect3DDevice8 vtable slot, from d3d8_api.hpp."""
    src = open(os.path.join(HERE, '..', 'src', 'd3d8', 'd3d8_api.hpp')).read()
    block = src[src.index('#define JOAN_IDIRECT3DDEVICE8_METHODS'):]
    block = block[:block.index('\n\n')]
    return [(m.group(2), len([p for p in m.group(3).split(',') if p.strip()]))
            for m in re.finditer(r'X\((\w+), (\w+), \(([^)]*)\)', block)]


dev_methods = _device_methods()

RS = dict(ZENABLE=7, FILLMODE=8, SHADEMODE=9, ZWRITEENABLE=14, ALPHATESTENABLE=15, LASTPIXEL=16, SRCBLEND=19,
          DESTBLEND=20, CULLMODE=22, ZFUNC=23, ALPHAREF=24, ALPHAFUNC=25, DITHERENABLE=26, ALPHABLENDENABLE=27,
          FOGENABLE=28, SPECULARENABLE=29, FOGCOLOR=34, FOGTABLEMODE=35, FOGSTART=36, FOGEND=37, FOGDENSITY=38,
          ZBIAS=47, RANGEFOGENABLE=48, STENCILENABLE=52, STENCILFAIL=53, STENCILZFAIL=54, STENCILPASS=55,
          STENCILFUNC=56, STENCILREF=57, STENCILMASK=58, STENCILWRITEMASK=59, TEXTUREFACTOR=60, WRAP0=128,
          CLIPPING=136, LIGHTING=137, AMBIENT=139, FOGVERTEXMODE=140, COLORVERTEX=141, LOCALVIEWER=142,
          NORMALIZENORMALS=143, DIFFUSEMATERIALSOURCE=145, SPECULARMATERIALSOURCE=146, AMBIENTMATERIALSOURCE=147,
          EMISSIVEMATERIALSOURCE=148, VERTEXBLEND=151, CLIPPLANEENABLE=152, POINTSIZE=154, POINTSIZE_MIN=155,
          POINTSPRITEENABLE=156, POINTSCALEENABLE=157, MULTISAMPLEANTIALIAS=161, MULTISAMPLEMASK=162,
          POINTSIZE_MAX=166, INDEXEDVERTEXBLENDENABLE=167, COLORWRITEENABLE=168, TWEENFACTOR=170, BLENDOP=171)
RS_NAME = {v: k for k, v in RS.items()}
TSS_NAME = {1: 'COLOROP', 2: 'COLORARG1', 3: 'COLORARG2', 4: 'ALPHAOP', 5: 'ALPHAARG1', 6: 'ALPHAARG2',
            7: 'BUMPENVMAT00', 8: 'BUMPENVMAT01', 9: 'BUMPENVMAT10', 10: 'BUMPENVMAT11', 11: 'TEXCOORDINDEX',
            13: 'ADDRESSU', 14: 'ADDRESSV', 15: 'BORDERCOLOR', 16: 'MAGFILTER', 17: 'MINFILTER', 18: 'MIPFILTER',
            19: 'MIPMAPLODBIAS', 20: 'MAXMIPLEVEL', 21: 'MAXANISOTROPY', 22: 'BUMPENVLSCALE', 23: 'BUMPENVLOFFSET',
            24: 'TEXTURETRANSFORMFLAGS', 25: 'ADDRESSW', 26: 'COLORARG0', 27: 'ALPHAARG0', 28: 'RESULTARG'}
TOP = {1: 'DISABLE', 2: 'SELECTARG1', 3: 'SELECTARG2', 4: 'MODULATE', 5: 'MODULATE2X', 6: 'MODULATE4X', 7: 'ADD',
       8: 'ADDSIGNED', 9: 'ADDSIGNED2X', 10: 'SUBTRACT', 11: 'ADDSMOOTH', 12: 'BLENDDIFFUSEALPHA',
       13: 'BLENDTEXTUREALPHA', 14: 'BLENDFACTORALPHA', 15: 'BLENDTEXTUREALPHAPM', 16: 'BLENDCURRENTALPHA',
       17: 'PREMODULATE', 18: 'MODULATEALPHA_ADDCOLOR', 19: 'MODULATECOLOR_ADDALPHA',
       20: 'MODULATEINVALPHA_ADDCOLOR', 21: 'MODULATEINVCOLOR_ADDALPHA', 22: 'BUMPENVMAP', 23: 'BUMPENVMAPLUMINANCE',
       24: 'DOTPRODUCT3', 25: 'MULTIPLYADD', 26: 'LERP'}
TA = {0: 'DIFFUSE', 1: 'CURRENT', 2: 'TEXTURE', 3: 'TFACTOR', 4: 'SPECULAR', 5: 'TEMP'}
BLEND = {1: 'ZERO', 2: 'ONE', 3: 'SRCCOLOR', 4: 'INVSRCCOLOR', 5: 'SRCALPHA', 6: 'INVSRCALPHA', 7: 'DESTALPHA',
         8: 'INVDESTALPHA', 9: 'DESTCOLOR', 10: 'INVDESTCOLOR', 11: 'SRCALPHASAT'}


def ta_name(v):
    base = TA.get(v & 0xf, hex(v & 0xf))
    if v & 0x10: base += '|COMPLEMENT'
    if v & 0x20: base += '|ALPHAREPLICATE'
    return base


def annotate_state(method, args):
    """Readable comment for SetRenderState / SetTextureStageState."""
    def num(x):
        try:
            return int(x, 0)
        except ValueError:
            return None
    if method == 'SetRenderState' and len(args) == 2:
        s, v = num(args[0]), num(args[1])
        name = RS_NAME.get(s, str(s)) if s is not None else args[0]
        val = args[1]
        if s in (19, 20) and v is not None: val = BLEND.get(v, args[1])
        return f'{name} = {val}'
    if method == 'SetTextureStageState' and len(args) == 3:
        st, s, v = args[0], num(args[1]), num(args[2])
        name = TSS_NAME.get(s, str(s)) if s is not None else args[1]
        val = args[2]
        if v is not None:
            if s in (1, 4): val = TOP.get(v, args[2])
            elif s in (2, 3, 5, 6, 26, 27, 28): val = ta_name(v)
        return f'[{st}] {name} = {val}'
    return None


def u32(va):
    return struct.unpack_from('<I', pe.data, pe.va2off(va))[0]


def u8(va):
    return pe.data[pe.va2off(va)]


# --- instruction stream ------------------------------------------------------
LINE = re.compile(r'^\s+([0-9a-f]+):\t(.*)$')


_ASM = None


def _asm():
    global _ASM
    if _ASM is None:
        addrs, texts, index = [], [], {}
        with open(os.path.join(WORK, 'joan.asm')) as f:
            for ln in f:
                m = LINE.match(ln)
                if not m:
                    continue
                a = int(m.group(1), 16)
                index[a] = len(addrs)
                addrs.append(a)
                texts.append(m.group(2).split('\t')[-1].strip())
        _ASM = (addrs, texts, index)
    return _ASM


def load_function(start):
    """Returns ({addr: (mnemonic, operands)}, order) from joan.asm starting at `start`,
    up to int3 padding (jump tables are cut later)."""
    addrs, texts, index = _asm()
    insns, order = {}, []
    i = index[start]
    while i < len(addrs) and len(order) < 20000:
        text = texts[i]
        if text.startswith('int3'):
            break
        parts = text.split(None, 1)
        insns[addrs[i]] = (parts[0] if parts else '', parts[1] if len(parts) > 1 else '')
        order.append(addrs[i])
        i += 1
    return insns, order


# --- operand parsing -------------------------------------------------------------
MEM = re.compile(r'(?:(BYTE|WORD|DWORD|QWORD|TBYTE) PTR )?(?:(\w+):)?\[([^\]]+)\]|(?:(BYTE|WORD|DWORD|QWORD) PTR )?ds:(0x[0-9a-f]+)')
REGS32 = ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp']
SUB = {'al': 'eax', 'ah': 'eax', 'ax': 'eax', 'bl': 'ebx', 'bh': 'ebx', 'bx': 'ebx', 'cl': 'ecx', 'ch': 'ecx',
       'cx': 'ecx', 'dl': 'edx', 'dh': 'edx', 'dx': 'edx', 'si': 'esi', 'di': 'edi'}
SIZE = {'BYTE': 'u8', 'WORD': 'u16', 'DWORD': 'u32', 'QWORD': 'u64', 'TBYTE': 'f80'}


def split_ops(s):
    out, depth, cur = [], 0, ''
    for c in s:
        if c == '[': depth += 1
        if c == ']': depth -= 1
        if c == ',' and depth == 0:
            out.append(cur.strip()); cur = ''
        else:
            cur += c
    if cur.strip(): out.append(cur.strip())
    return out


class Lifter:
    def __init__(self, start, this_name='S'):
        self.start = start
        self.this_name = this_name
        self.insns, self.order = load_function(start)
        self.tables = {}  # jmp addr -> list of targets
        self.labels = set()
        self.ret_n = None
        self._find_end_and_tables()
        self.this_slot = None  # ebp offset where the prologue spills ecx (thiscall)
        for a in self.order[:14]:
            mn, ops = self.insns[a]
            m = re.match(r'DWORD PTR \[ebp-(0x[0-9a-f]+)\],ecx$', ops)
            if mn == 'mov' and m:
                self.this_slot = -int(m.group(1), 16)
                break
            if 'ecx' in ops.split(',')[0]:
                break  # ecx overwritten first: not a thiscall

    def _find_end_and_tables(self):
        # The function ends at the last `ret` reachable before the jump tables; collect jump tables first.
        code = []
        for a in self.order:
            mn, ops = self.insns[a]
            code.append(a)
        # tables: jmp DWORD PTR [reg*4+T], bound by a preceding `cmp X, N` + ja.
        prev = []
        for a in self.order:
            mn, ops = self.insns[a]
            m = re.match(r'DWORD PTR \[(\w+)\*4\+(0x[0-9a-f]+)\]', ops)
            if mn == 'jmp' and m:
                table = int(m.group(2), 16)
                n = None
                bytemap = None
                for pa in reversed(prev[-8:]):
                    pmn, pops = self.insns[pa]
                    mm = re.match(r'(?:DWORD PTR )?\[ebp-0x[0-9a-f]+\],(0x[0-9a-f]+)$', pops) or re.match(r'\w+,(0x[0-9a-f]+)$', pops)
                    bm = re.match(r'\w+,BYTE PTR \[\w+\+(0x[0-9a-f]+)\]', pops)
                    if pmn == 'mov' and bm:
                        bytemap = int(bm.group(1), 16)
                    if pmn == 'cmp' and mm and n is None:
                        n = int(mm.group(1), 16) + 1
                if bytemap is not None:
                    idx = [u8(bytemap + i) for i in range(n)]
                    targets = [u32(table + 4 * k) for k in range(max(idx) + 1)]
                    self.tables[a] = ('byte', [targets[i] for i in idx], table, bytemap)
                else:
                    self.tables[a] = ('dword', [u32(table + 4 * k) for k in range(n)], table, None)
            prev.append(a)
        # Cut the instruction list at the first table address (data, not code).
        cut = min([t[2] for t in self.tables.values()] + [t[3] for t in self.tables.values() if t[3]] + [1 << 40])
        self.order = [a for a in self.order if a < cut]
        # End at the last ret.
        last_ret = max(a for a in self.order if self.insns[a][0] == 'ret')
        self.order = [a for a in self.order if a <= last_ret]
        r = self.insns[last_ret][1]
        self.ret_n = int(r, 16) if r else 0
        for a in self.order:
            mn, ops = self.insns[a]
            if mn.startswith('j') and re.match(r'0x[0-9a-f]+$', ops):
                self.labels.add(int(ops, 16))
        for t in self.tables.values():
            self.labels.update(t[1])

    # --- symbolic evaluation ---------------------------------------------------------
    def reset(self):
        self.regs = {}
        self.stack = []
        self.fpu = []
        self.flags = None

    def name_mem(self, size, base_expr, off):
        if base_expr == self.this_name and off == 8:
            return 'dev'
        f = f'{base_expr}.f{off:x}' if base_expr == self.this_name else f'{base_expr}[+0x{off:x}]'
        if size and size != 'u32':
            f += f'.{size}'
        return f

    def mem(self, op):
        m = MEM.search(op)
        if not m:
            return None
        if m.group(5):
            size = SIZE.get(m.group(4) or 'DWORD')
            v = int(m.group(5), 16)
            name = f'g_{v:x}'
            return name + ('' if size == 'u32' else f'.{size}')
        size = SIZE.get(m.group(1) or 'DWORD')
        inner = m.group(3)
        terms = re.findall(r'[+-]?[^+-]+', inner)
        base = None
        idx = None
        off = 0
        for t in terms:
            sign = -1 if t.startswith('-') else 1
            t = t.lstrip('+-')
            if t.startswith('0x'):
                off += sign * int(t, 16)
            elif '*' in t:
                r, k = t.split('*')
                idx = f'{self.reg(r)}*{k}'
            elif t in REGS32:
                if base is None: base = t
                else: idx = self.reg(t)
        if base == 'ebp':
            if off == self.this_slot:
                return 'this'
            if off > 0:
                return f'arg{(off - 8) // 4}' + ('' if size == 'u32' else f'.{size}')
            return f'l{-off:x}' + ('' if size == 'u32' else f'.{size}')
        be = self.reg(base) if base else '0'
        if idx:
            sign = '-' if off < 0 else '+'
            return f'{be}[{idx}{sign}0x{abs(off):x}]' + ('' if size == 'u32' else f'.{size}')
        return self.name_mem(size, be, off)

    def reg(self, r):
        full = SUB.get(r, r)
        return self.regs.get(full, f'?{full}')

    def val(self, op):
        op = op.strip()
        if re.match(r'^0x[0-9a-f]+$', op):
            return op
        if op in REGS32 or op in SUB:
            return self.reg(op)
        if 'PTR' in op or '[' in op or 'ds:' in op:
            v = self.mem(op)
            return 'S' if v == 'this' else v
        return op

    def setreg(self, r, v):
        self.regs[SUB.get(r, r)] = v

    def _epilogue(self):
        quiet = {('pop', 'edi'), ('pop', 'esi'), ('pop', 'ebx'), ('pop', 'ebp'), ('mov', 'esp,ebp'), ('leave', '')}
        start = None
        for a in reversed(self.order):
            mn, ops = self.insns[a]
            if mn == 'ret' or (mn, ops) in quiet or (mn == 'mov' and ops.startswith('DWORD PTR fs:0x0,')) \
                    or (mn == 'mov' and re.match(r'ecx,DWORD PTR \[ebp-0x[0-9a-f]+\]$', ops) and start is not None and False):
                start = a
                continue
            # SEH epilogue: mov ecx,[ebp-0xc] ; mov fs:0,ecx
            if mn == 'mov' and re.match(r'ecx,DWORD PTR \[ebp-0xc\]$', ops):
                start = a
                continue
            break
        return start

    def lift(self):
        out = [f'// lifted 0x{self.start:x}  (ret {self.ret_n})']
        self.reset()
        self.epi = self._epilogue()
        self.done = False
        for a in self.order:
            mn, ops = self.insns[a]
            if self.epi is not None and a == self.epi:
                if not self.done:
                    out.append(f'    return {self.regs.get("eax", "?eax")};  // fall into epilogue')
                break
            if a in self.labels:
                out.append(f'L_{a:x}:')
                self.reset()
            self.done = False
            o = split_ops(ops)
            try:
                line = self.step(a, mn, o, ops)
            except Exception as e:  # keep going, show raw
                line = f'/* ?? {mn} {ops} ({e}) */'
            if line:
                out.append('    ' + line if not line.startswith('L_') else line)
        return '\n'.join(out)

    def cond(self, jcc):
        if not self.flags:
            return f'?flags {jcc}'
        kind, a, b = self.flags
        if kind == 'test' and a == b:
            m = {'je': '== 0', 'jne': '!= 0', 'jle': '<= 0', 'jg': '> 0', 'js': '< 0', 'jns': '>= 0'}
            return f'{a} {m.get(jcc, jcc)}'
        if kind == 'test':
            m = {'je': '== 0', 'jne': '!= 0'}
            return f'({a} & {b}) {m.get(jcc, jcc)}'
        if kind == 'fpu':
            return f'fpu_status({a}) {jcc}'
        if kind == 'x87':
            (x, y), mask = a, b
            rel = {0x40: ('==', '!='), 0x01: ('<', '>='), 0x41: ('<=', '>'), 0x05: ('<', '>='), 0x44: ('==', '!=')}
            if mask in rel and jcc in ('jne', 'je'):
                op = rel[mask][0 if jcc == 'jne' else 1]
                return f'{x} {op} {y} /*x87: NaN counts as {"true" if jcc == "jne" else "false"}*/'
            return f'x87({x} cmp {y}) test 0x{mask:x} {jcc}'
        ops = {'je': '==', 'jne': '!=', 'jl': '<', 'jle': '<=', 'jg': '>', 'jge': '>=',
               'jb': '<u', 'jbe': '<=u', 'ja': '>u', 'jae': '>=u'}
        return f'{a} {ops.get(jcc, jcc)} {b}'

    def step(self, a, mn, o, raw):
        if mn in ('push',) and o and o[0] in ('ebp', 'ebx', 'esi', 'edi') and a < self.start + 8:
            return None
        if mn in ('pop',) or (mn == 'mov' and o == ['ebp', 'esp']) or (mn == 'mov' and o == ['esp', 'ebp']):
            return None
        if mn == 'sub' and o[0] == 'esp':
            return None
        if mn == 'mov' and self.this_slot is not None and o[0] == f'DWORD PTR [ebp-0x{-self.this_slot:x}]' and o[1] == 'ecx' \
                and a in self.order[:14]:
            return None
        if mn == 'mov':
            dst, src = o
            v = self.val(src)
            if dst in REGS32 or dst in SUB:
                if dst in ('al', 'bl', 'cl', 'dl', 'ax', 'bx', 'cx', 'dx') and self.regs.get(SUB[dst]) == '0':
                    self.setreg(dst, v)  # xor r,r ; mov rl,X  = zero extension
                else:
                    self.setreg(dst, v)
                return None
            d = self.mem(dst)
            d = 'S' if d == 'this' else d
            return f'{d} = {v};'
        if mn in ('movsx', 'movzx'):
            self.setreg(o[0], f'({"s" if mn == "movsx" else "u"}){self.val(o[1])}')
            return None
        if mn == 'xor' and o[0] == o[1]:
            self.setreg(o[0], '0'); return None
        if mn in ('and', 'or', 'add', 'sub', 'shl', 'shr', 'sar', 'imul', 'xor') and len(o) == 2:
            sym = {'and': '&', 'or': '|', 'add': '+', 'sub': '-', 'shl': '<<', 'shr': '>>', 'sar': '>>s', 'imul': '*', 'xor': '^'}[mn]
            if o[0] == 'esp' and mn == 'add':
                return None
            if o[0] in REGS32 or o[0] in SUB:
                v = self.val(o[1])
                if mn == 'and' and o[0] in ('al', 'bl', 'cl', 'dl') and v.startswith('0x'):
                    v = hex(int(v, 16) | 0xffffff00)  # byte op keeps bits 8..31
                self.setreg(o[0], f'({self.reg(o[0])} {sym} {v})')
                return None
            d = self.mem(o[0])
            return f'{d} {sym}= {self.val(o[1])};'
        if mn == 'imul' and len(o) == 3:
            self.setreg(o[0], f'({self.val(o[1])} * {self.val(o[2])})'); return None
        if mn == 'not':
            self.setreg(o[0], f'~{self.reg(o[0])}'); return None
        if mn == 'neg':
            v = self.reg(o[0])
            self.last_neg = (SUB.get(o[0], o[0]), v)
            self.setreg(o[0], v[1:] if v.startswith('-') else f'-{v}'); return None
        if mn == 'sbb' and len(o) == 2 and o[0] == o[1] and getattr(self, 'last_neg', (None,))[0] == SUB.get(o[0], o[0]):
            self.setreg(o[0], f'-({self.last_neg[1]} != 0)'); return None
        if mn == 'lea':
            m = re.match(r'\[ebp-(0x[0-9a-f]+)\]', o[1])
            if m:
                self.setreg(o[0], f'&l{int(m.group(1), 16):x}')
            else:
                inner = o[1].strip('[]')
                expr = inner
                for r in REGS32:
                    expr = re.sub(rf'\b{r}\b', lambda _m, r=r: self.reg(r), expr)
                self.setreg(o[0], f'({expr})')
            return None
        if mn == 'push':
            self.stack.append(self.val(o[0]))
            return None
        if mn == 'test' and o[0] == 'ah' and getattr(self, 'fpu_cmp', None) and self.regs.get('eax') == 'fsw':
            # x87 status after fcomp: C0 (0x01) = st0 < src, C3 (0x40) = equal; both set when unordered
            self.flags = ('x87', self.fpu_cmp, int(o[1], 16)); return None
        if mn in ('cmp', 'test'):
            self.flags = (mn, self.val(o[0]), self.val(o[1]))
            return None
        if mn.startswith('j') and mn != 'jmp':
            return f'if ({self.cond(mn)}) goto L_{int(o[0], 16):x};'
        if mn == 'jmp':
            if a in self.tables:
                kind, targets, table, bm = self.tables[a]
                sel = re.match(r'DWORD PTR \[(\w+)\*4', raw).group(1)
                cases = ', '.join(f'{i}: L_{t:x}' for i, t in enumerate(targets))
                return f'switch ({self.reg(sel)}) {{ {cases} }}'
            target = int(o[0], 16)
            if self.epi is not None and target >= self.epi:
                self.done = True
                return f'return {self.regs.get("eax", "?eax")};'
            return f'goto L_{target:x};'
        if mn == 'call':
            return self.call(o[0])
        if mn == 'ret':
            return 'return' + (f' {self.regs.get("eax", "")}' if 'eax' in self.regs else '') + ';'
        if mn == 'sete':
            self.setreg(o[0], f'({self.cond("je").replace(" == ", " == ")})'); return None
        if mn in ('fld', 'fild'):
            self.fpu.append(self.val(o[0])); return None
        if mn in ('fmul', 'fdiv', 'fadd', 'fsub'):
            sym = {'fmul': '*', 'fdiv': '/', 'fadd': '+', 'fsub': '-'}[mn]
            t = self.fpu.pop() if self.fpu else '?st0'
            self.fpu.append(f'({t} {sym} {self.val(o[0])})'); return None
        if mn in ('fstp', 'fst'):
            t = self.fpu.pop() if self.fpu and mn == 'fstp' else (self.fpu[-1] if self.fpu else '?st0')
            d = self.mem(o[0])
            return f'{d} = (float){t};'
        if mn in ('fcomp', 'fcom'):
            t = self.fpu.pop() if self.fpu and mn == 'fcomp' else (self.fpu[-1] if self.fpu else '?st0')
            self.fpu_cmp = (t, self.val(o[0]))
            self.flags = ('fpu', f'{t} cmp {self.val(o[0])}', None); return None
        if mn == 'fnstsw':
            self.setreg('eax', f'fsw'); return None
        if mn == 'fchs':
            self.fpu.append(f'-{self.fpu.pop()}'); return None
        if mn == 'cdq':
            return None
        if mn == 'idiv':
            self.setreg('eax', f'({self.reg("eax")} / {self.val(o[0])})')
            self.setreg('edx', f'({self.reg("eax")} % {self.val(o[0])})'); return None
        if mn == 'rep':
            return f'/* rep {raw} : memcpy(edi={self.reg("edi")}, esi={self.reg("esi")}, ecx={self.reg("ecx")} dwords) */'
        return f'/* {mn} {raw} */'

    def call(self, target):
        m = re.match(r'DWORD PTR \[(\w+)\+(0x[0-9a-f]+)\]', target)
        if m:  # virtual call: this is the last push
            off = int(m.group(2), 16)
            name, n = dev_methods[off // 4] if off // 4 < len(dev_methods) else (f'vfn_{off:x}', 0)
            obj = self.stack.pop() if self.stack else '?'
            args = [self.stack.pop() if self.stack else '?' for _ in range(n)]
            self.regs['eax'] = f'hr'
            note = annotate_state(name, args)
            return f'{obj}->{name}({", ".join(args)});' + (f'  // {note}' if note else '')
        m = re.match(r'DWORD PTR ds:(0x[0-9a-f]+)', target)
        if m:
            k = '%08x' % int(m.group(1), 16)
            name = imports.get(k, f'import_{k}')
            args = list(reversed(self.stack)); self.stack = []
            self.regs['eax'] = f'{name}(...)'
            return f'{name}({", ".join(args)});  // cdecl, args as pushed'
        t = int(target, 16)
        real = thunks.get('%08x' % t)
        real_addr = int(real, 16) if real else t
        n = self.callee_ret(real_addr)
        if n is None:  # cdecl: take everything pushed
            args = list(reversed(self.stack)); self.stack = []
            conv = 'cdecl'
        else:
            args = [self.stack.pop() if self.stack else '?' for _ in range(n // 4)]
            conv = 'thiscall'
        ecx = self.regs.get('ecx', '?ecx')
        res = f'fn_{real_addr:x}' + (f'@{t:x}' if real else '')
        self.regs = {k: v for k, v in self.regs.items() if k in ('ebx', 'esi', 'edi')}
        self.regs['eax'] = f'{res}()'
        if conv == 'thiscall':
            return f'r = {res}({ecx}{", " if args else ""}{", ".join(args)});  // thiscall'
        return f'r = {res}({", ".join(args)});  // cdecl'

    _ret_cache = {}

    def callee_ret(self, addr):
        if addr in Lifter._ret_cache:
            return Lifter._ret_cache[addr]
        insns, order = load_function(addr)
        n = None
        for a in order:
            mn, ops = insns[a]
            if mn == 'ret':
                n = int(ops, 16) if ops else None
                if ops == '':
                    n = None if not self._uses_ecx(insns, order) else 0
                break
        Lifter._ret_cache[addr] = n
        return n

    def _uses_ecx(self, insns, order):
        for a in order[:10]:
            mn, ops = insns[a]
            if mn == 'mov' and ops.endswith(',ecx'):
                return True
        return False


if __name__ == '__main__':
    for arg in sys.argv[1:]:  # several addresses: one listing load
        try:
            print(Lifter(int(arg, 16)).lift())
        except Exception as e:  # keep going in batch mode
            print(f'// lifted 0x{arg}: failed ({e})')
