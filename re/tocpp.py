#!/usr/bin/env python3
"""Turns lift.py device-call lines into named C++ calls (rs / tss helpers).

usage: tocpp.py lifted.c  > converted.c
Other lines pass through unchanged.
"""
import re
import sys

RS = {7: 'kRsZEnable', 8: 'kRsFillMode', 14: 'kRsZWriteEnable', 15: 'kRsAlphaTestEnable', 19: 'kRsSrcBlend',
      20: 'kRsDestBlend', 22: 'kRsCullMode', 24: 'kRsAlphaRef', 27: 'kRsAlphaBlendEnable', 29: 'kRsSpecularEnable',
      47: 'kRsZBias', 52: 'kRsStencilEnable', 57: 'kRsStencilRef', 60: 'kRsTextureFactor', 137: 'kRsLighting',
      142: 'kRsLocalViewer', 151: 'kRsVertexBlend'}
TSS = {1: 'kTssColorOp', 2: 'kTssColorArg1', 3: 'kTssColorArg2', 4: 'kTssAlphaOp', 5: 'kTssAlphaArg1',
       6: 'kTssAlphaArg2', 7: 'kTssBumpEnvMat00', 8: 'kTssBumpEnvMat01', 9: 'kTssBumpEnvMat10',
       10: 'kTssBumpEnvMat11', 11: 'kTssTexCoordIndex', 13: 'kTssAddressU', 14: 'kTssAddressV',
       15: 'kTssBorderColor', 16: 'kTssMagFilter', 17: 'kTssMinFilter', 18: 'kTssMipFilter',
       22: 'kTssBumpEnvLScale', 23: 'kTssBumpEnvLOffset', 24: 'kTssTextureTransformFlags', 26: 'kTssColorArg0',
       28: 'kTssResultArg'}
TOP = ['', 'kTopDisable', 'kTopSelectArg1', 'kTopSelectArg2', 'kTopModulate', 'kTopModulate2x', 'kTopModulate4x',
       'kTopAdd', 'kTopAddSigned', 'kTopAddSigned2x', 'kTopSubtract', 'kTopAddSmooth', 'kTopBlendDiffuseAlpha',
       'kTopBlendTextureAlpha', 'kTopBlendFactorAlpha', 'kTopBlendTextureAlphaPm', 'kTopBlendCurrentAlpha',
       'kTopPremodulate', 'kTopModulateAlphaAddColor', 'kTopModulateColorAddAlpha', 'kTopModulateInvAlphaAddColor',
       'kTopModulateInvColorAddAlpha', 'kTopBumpEnvMap', 'kTopBumpEnvMapLuminance', 'kTopDotProduct3',
       'kTopMultiplyAdd', 'kTopLerp']
TA = ['kTaDiffuse', 'kTaCurrent', 'kTaTexture', 'kTaTFactor', 'kTaSpecular', 'kTaTemp']
BLEND = ['', 'kBlendZero', 'kBlendOne', 'kBlendSrcColor', 'kBlendInvSrcColor', 'kBlendSrcAlpha', 'kBlendInvSrcAlpha',
         'kBlendDestAlpha', 'kBlendInvDestAlpha', 'kBlendDestColor', 'kBlendInvDestColor', 'kBlendSrcAlphaSat']
ADDR = ['', 'kTaddressWrap', 'kTaddressMirror', 'kTaddressClamp', 'kTaddressBorder']
FILT = ['kTexfNone', 'kTexfPoint', 'kTexfLinear']
CULL = ['', 'kCullNone', 'kCullCw', 'kCullCcw']
TTFF = {0: 'kTtffDisable', 2: 'kTtffCount2', 3: 'kTtffCount3', 0x102: 'kTtffCount2 | kTtffProjected',
        0x103: 'kTtffCount3 | kTtffProjected', 0x104: 'kTtffCount4 | kTtffProjected'}
TCI = {0: 'kTciPassThru', 0x10000: 'kTciCameraSpaceNormal', 0x20000: 'kTciCameraSpacePosition',
       0x30000: 'kTciCameraSpaceReflection'}


def num(x):
    try:
        return int(x, 0)
    except ValueError:
        return None


def expr(x):
    """Lifter expression -> C++-ish expression."""
    x = x.strip()
    v = num(x)
    if v is not None:
        return str(v) if v < 0x10000 else hex(v)
    return x


def ta(v):
    s = TA[v & 0xf]
    if v & 0x10: s += ' | kTaComplement'
    if v & 0x20: s += ' | kTaAlphaReplicate'
    return s


def value(state, x, is_tss):
    v = num(x)
    if v is None:
        m = re.match(r'\((.+) \| (0x[0-9a-f]+)\)$', x)
        if is_tss and state == 11 and m and int(m.group(2), 16) in TCI:
            return f'{expr(m.group(1))} | {TCI[int(m.group(2), 16)]}'
        return expr(x)
    if is_tss:
        if state in (1, 4) and 0 < v < len(TOP): return TOP[v]
        if state in (2, 3, 5, 6, 26, 28): return ta(v)
        if state in (13, 14) and 0 < v < len(ADDR): return ADDR[v]
        if state in (16, 17, 18) and v < len(FILT): return FILT[v]
        if state == 24 and v in TTFF: return TTFF[v]
        if state == 11:
            if v & 0xffff0000 and (v & 0xffff0000) in TCI:
                return f'{v & 0xffff} | {TCI[v & 0xffff0000]}' if v & 0xffff else TCI[v & 0xffff0000]
            return str(v)
        return expr(x)
    if state in (19, 20) and 0 < v < len(BLEND): return BLEND[v]
    if state == 22 and 0 < v < len(CULL): return CULL[v]
    return expr(x)


def convert(line):
    m = re.match(r'(\s*)dev->SetTextureStageState\((.+?), (0x[0-9a-f]+), (.+)\);', line)
    if m:
        ind, stage, st, v = m.group(1), m.group(2), int(m.group(3), 16), m.group(4)
        return f'{ind}tss({expr(stage)}, {TSS.get(st, hex(st))}, {value(st, v, True)});'
    m = re.match(r'(\s*)dev->SetRenderState\((0x[0-9a-f]+), (.+)\);', line)
    if m:
        ind, st, v = m.group(1), int(m.group(2), 16), m.group(3)
        return f'{ind}rs({RS.get(st, hex(st))}, {value(st, v, False)});'
    return line


for ln in open(sys.argv[1]):
    print(convert(ln.rstrip('\n')))
