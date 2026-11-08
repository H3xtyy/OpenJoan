#!/usr/bin/env python3
"""Struct recovery, step 1 (docs/X64.md): the raw field accesses of one source file, as JSON lines.

For every function body of the file (members of the per-block `struct H`, other records' members, free functions)
it records each `f<T>(off)` / `at<T>(base, off)` with the accessed type and width, the constant offset (or the
expression), what the base is (`this`, a field read by another access, a local initialised from one, a parameter, a
data-image object `d::X`, ...) and whether it is written. It also records the registrations of the file
(name, address, the function they register) and the facade casts of `this` (`as_x(this)`, `object<T>(this)`), which
say what class `this` is.

    re/layout/extract.py <src-relative file.cpp> [-o out.jsonl]

Needs the libclang Python bindings (pip: libclang 18) and llvm-mingw (i686 headers; layouts are the x86 ones).
"""
import argparse
import ctypes
import json
import os
import sys

import clang.cindex as ci

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SRC = os.path.join(ROOT, 'src')
MINGW = os.environ.get('JOAN_MINGW', '/opt/llvm-mingw')
SDL = os.environ.get('JOAN_SDL3_INCLUDE', '/tmp/sdl3/include')
ARGS = ['-x', 'c++', '-std=c++20', '--target=i686-w64-mingw32', '-fms-extensions', '-fasm-blocks', '-nostdinc++',
        '-nostdinc', '-isystem', f'{MINGW}/i686-w64-mingw32/include/c++/v1', '-isystem', f'{MINGW}/lib/clang/18/include',
        '-isystem', f'{MINGW}/i686-w64-mingw32/include', '-isystem', SDL, '-ferror-limit=0', '-Wno-everything',
        '-DWIN32_LEAN_AND_MEAN', '-DNOMINMAX', '-I' + SRC]

lib = ci.conf.lib
lib.clang_Cursor_Evaluate.argtypes = [ci.Cursor]
lib.clang_Cursor_Evaluate.restype = ctypes.c_void_p
lib.clang_EvalResult_getKind.argtypes = [ctypes.c_void_p]
lib.clang_EvalResult_getKind.restype = ctypes.c_int
lib.clang_EvalResult_getAsLongLong.argtypes = [ctypes.c_void_p]
lib.clang_EvalResult_getAsLongLong.restype = ctypes.c_longlong
lib.clang_EvalResult_dispose.argtypes = [ctypes.c_void_p]

K = ci.CursorKind
FUNC_KINDS = (K.CXX_METHOD, K.FUNCTION_DECL, K.CONSTRUCTOR, K.DESTRUCTOR, K.FUNCTION_TEMPLATE)
WRAP = (K.UNEXPOSED_EXPR, K.PAREN_EXPR, K.CSTYLE_CAST_EXPR, K.CXX_STATIC_CAST_EXPR, K.CXX_REINTERPRET_CAST_EXPR,
        K.CXX_CONST_CAST_EXPR, K.CXX_FUNCTIONAL_CAST_EXPR)
RAW = ('at', 'f')
FACADE_PREFIX = ('as_',)
FACADE_NAMES = ('object', 'dynarray', 'file', 'self_as')
SELF_HELPERS = ('self', 'me', 'bytes', 'raw', 'base', 'p', 'self_bytes', 'this_ptr', 'obj')


def evaluate(cur):
    r = lib.clang_Cursor_Evaluate(cur)
    if not r:
        return None
    try:
        return lib.clang_EvalResult_getAsLongLong(r) if lib.clang_EvalResult_getKind(r) == 1 else None
    finally:
        lib.clang_EvalResult_dispose(r)


def strip(x):
    while x is not None and x.kind in WRAP:
        ch = list(x.get_children())
        if not ch:
            break
        x = ch[-1]  # casts: the operand is the last child (the type reference comes first)
    return x


def text(cur, limit=160):
    try:
        t = ' '.join(tok.spelling for tok in cur.get_tokens())
    except Exception:
        t = ''
    return t[:limit]


def in_main(cur, main):
    f = cur.location.file
    return f is not None and os.path.realpath(f.name) == main


def raw_type(call):
    """Accessed type of an at/f call: the referenced function returns T&."""
    r = call.referenced
    if r is None:
        return None
    res = r.type.get_result()
    if res.kind != ci.TypeKind.LVALUEREFERENCE:
        return None
    return res.get_pointee()


def qualified(cur):
    parts = []
    c = cur
    while c is not None and c.kind != K.TRANSLATION_UNIT:
        parts.append(c.spelling or '(anon)')
        c = c.semantic_parent
    return '::'.join(reversed(parts))


class FileScan:
    def __init__(self, rel):
        self.rel = rel
        self.main = os.path.realpath(os.path.join(SRC, rel))
        self.out = []

    def emit(self, rec):
        rec['file'] = self.rel
        self.out.append(rec)

    def run(self):
        idx = ci.Index.create()
        tu = idx.parse(self.main, args=ARGS, options=0)
        fatal = [d for d in tu.diagnostics if d.severity >= 4]
        self.emit({'k': 'tu', 'fatal': len(fatal), 'errors': sum(1 for d in tu.diagnostics if d.severity == 3)})
        self.visit(tu.cursor)
        return self.out

    CONTAINERS = (K.NAMESPACE, K.STRUCT_DECL, K.CLASS_DECL, K.UNION_DECL, K.CLASS_TEMPLATE, K.LINKAGE_SPEC,
                  K.UNEXPOSED_DECL)

    def visit(self, scope):
        for cur in scope.get_children():
            if not in_main(cur, self.main):
                continue
            if cur.kind == K.VAR_DECL and cur.spelling.startswith('joan_replace_'):
                self.registration(cur)
            elif cur.kind in FUNC_KINDS and cur.is_definition():
                self.function(cur)
            elif cur.kind == K.FIELD_DECL:
                self.field(cur)
            elif cur.kind in self.CONTAINERS:
                self.visit(cur)

    # --- registrations -------------------------------------------------------------------------------------------
    def registration(self, var):
        strings, ints, fns = [], [], []
        for n in var.walk_preorder():
            if n.kind == K.STRING_LITERAL:
                strings.append(n.spelling.strip('"'))
            elif n.kind == K.INTEGER_LITERAL:
                v = evaluate(n)
                if v is not None:
                    ints.append(v)
            elif n.kind in (K.DECL_REF_EXPR, K.MEMBER_REF_EXPR, K.OVERLOADED_DECL_REF):
                r = n.referenced
                if r is not None and r.kind in FUNC_KINDS and r.spelling != 'code_address':
                    fns.append(r.get_usr())
        if len(strings) >= 2:
            self.emit({'k': 'reg', 'name': strings[0], 'module': strings[1],
                       'addr': ints[0] if ints else None, 'fn': fns[0] if fns else None, 'line': var.location.line,
                       'scope': qualified(var.semantic_parent)})

    # --- record fields (typed layouts written by hand: original/*.hpp-style structs inside .cpp files) -------------
    def field(self, cur):
        rec = cur.semantic_parent
        try:
            off = rec.type.get_offset(cur.spelling)
        except Exception:
            off = -1
        self.emit({'k': 'field', 'record': qualified(rec), 'name': cur.spelling, 'type': cur.type.spelling,
                   'canon': cur.type.get_canonical().spelling, 'size': cur.type.get_size(),
                   'off': off // 8 if off is not None and off >= 0 else None, 'line': cur.location.line})

    # --- function bodies -----------------------------------------------------------------------------------------
    def function(self, fn):
        self.fn = fn
        self.fn_usr = fn.get_usr()
        self.locals = {}  # local variable USR -> access id of its initialiser (a raw read)
        self.params = {p.get_usr(): i for i, p in enumerate(fn.get_arguments() or [])}
        self.ids = 0
        self.seen = {}
        rec = fn.semantic_parent
        self.emit({'k': 'fn', 'usr': self.fn_usr, 'name': fn.spelling, 'qual': qualified(fn),
                   'record': qualified(rec) if rec.kind in (K.STRUCT_DECL, K.CLASS_DECL) else None,
                   'static': fn.is_static_method() if fn.kind == K.CXX_METHOD else None,
                   'params': [(p.spelling, p.type.spelling) for p in (fn.get_arguments() or [])],
                   'line': fn.location.line, 'end': fn.extent.end.line})
        self.walk(fn, None, 0)

    def walk(self, node, parent, index):
        kind = node.kind
        if kind == K.VAR_DECL:
            init = None
            for ch in node.get_children():
                init = ch
            if init is not None:
                s = strip(init)
                if s is not None and s.kind == K.CALL_EXPR and s.spelling in RAW and raw_type(s) is not None:
                    self.locals[node.get_usr()] = {'b': 'acc', 'id': self.access(s, None, 0), 'via': node.spelling}
                    return  # the initialiser's own children were handled by access()
                if node.type.get_canonical().kind in (ci.TypeKind.POINTER, ci.TypeKind.UINT, ci.TypeKind.INT):
                    d = self.base_of(init)
                    if d['b'] in ('this', 'acc', 'param', 'global'):
                        d = dict(d)
                        d['via'] = node.spelling
                        self.locals[node.get_usr()] = d
        if kind == K.CALL_EXPR:
            if node.spelling in RAW and raw_type(node) is not None:
                self.access(node, parent, index)
                return
            self.facade(node)
        for i, ch in enumerate(node.get_children()):
            self.walk(ch, node, i)

    def base_of(self, expr):
        e = strip(expr)
        if e is None:
            return {'b': 'none'}
        if e.kind == K.CXX_THIS_EXPR:
            return {'b': 'this'}
        if e.kind == K.CALL_EXPR and e.spelling in RAW and raw_type(e) is not None:
            return {'b': 'acc', 'id': self.access(e, None, 0)}
        if e.kind == K.DECL_REF_EXPR and e.referenced is not None:
            r = e.referenced
            usr = r.get_usr()
            if usr in self.locals:
                return self.locals[usr]
            if usr in self.params:
                return {'b': 'param', 'i': self.params[usr], 'name': r.spelling, 'type': r.type.spelling}
            if r.kind == K.VAR_DECL:
                return {'b': 'var', 'name': r.spelling, 'type': r.type.spelling}
        if e.kind == K.CALL_EXPR:
            t = text(e, 120)
            args = list(e.get_arguments())
            if not args and e.spelling in SELF_HELPERS:
                return {'b': 'this', 'via': e.spelling}
            toks = [tok.spelling for tok in e.get_tokens()]
            if any(toks[i] == 'd' and toks[i + 1] == '::' for i in range(len(toks) - 2)):
                i = next(i for i in range(len(toks) - 2) if toks[i] == 'd' and toks[i + 1] == '::')
                sym = 'd::' + toks[i + 2]
                return {'b': 'global', 'sym': sym, 'helper': e.spelling, 'expr': t}
            if len(args) == 1 and e.spelling not in RAW:
                inner = self.base_of(args[0])  # casts written as helper calls: as_bytes(p), ptr(p), self_as(p)...
                if inner['b'] in ('this', 'acc', 'param'):
                    d = dict(inner)
                    d['helper'] = e.spelling
                    return d
            return {'b': 'call', 'fn': e.spelling, 'expr': t}
        if e.kind == K.BINARY_OPERATOR:  # pointer arithmetic: base + n
            ch = list(e.get_children())
            if len(ch) == 2:
                toks = [tok.spelling for tok in e.get_tokens()]
                n_lhs = len(list(ch[0].get_tokens()))
                op = toks[n_lhs] if n_lhs < len(toks) else ''
                if op in ('+', '-'):
                    k = evaluate(ch[1])
                    if k is not None:
                        inner = self.base_of(ch[0])
                        if inner['b'] in ('this', 'acc', 'param', 'global'):
                            d = dict(inner)
                            d['add'] = d.get('add', 0) + (k if op == '+' else -k)
                            return d
            return {'b': 'arith', 'expr': text(e, 120)}
        return {'b': e.kind.name.lower(), 'expr': text(e, 120)}

    def access(self, call, parent, index):
        key = call.extent.start.offset
        if key in self.seen:
            return self.seen[key]
        t = raw_type(call)
        args = list(call.get_arguments())
        if call.spelling == 'f':
            base = {'b': 'this'}
            off_expr = args[0] if args else None
        else:
            base = self.base_of(args[0]) if args else {'b': 'none'}
            if args and base['b'] not in ('this', 'acc'):
                self.walk(args[0], call, 0)  # raw reads inside a computed base (arithmetic, calls)
            off_expr = args[1] if len(args) > 1 else None
        off = evaluate(off_expr) if off_expr is not None else None
        self.ids += 1
        aid = f'{self.fn_usr}#{self.ids}'
        self.seen[key] = aid
        write = False
        if parent is not None and index == 0 and parent.kind in (K.BINARY_OPERATOR, K.COMPOUND_ASSIGNMENT_OPERATOR):
            toks = [tok.spelling for tok in parent.get_tokens()]
            own = len(list(call.get_tokens()))
            op = toks[own] if own < len(toks) else ''
            write = op.endswith('=') and op not in ('==', '!=', '<=', '>=')
        elif parent is not None and parent.kind == K.UNARY_OPERATOR:
            toks = [tok.spelling for tok in parent.get_tokens()]
            write = bool(toks) and (toks[0] in ('++', '--') or toks[-1] in ('++', '--'))
        canon = t.get_canonical()
        self.emit({'k': 'acc', 'id': aid, 'fn': self.fn_usr, 'via': call.spelling, 'type': t.spelling,
                   'canon': canon.spelling, 'size': t.get_size(), 'ptr': canon.kind == ci.TypeKind.POINTER,
                   'off': off, 'off_expr': None if off is not None else (text(off_expr, 80) if off_expr is not None else None),
                   'base': base, 'w': write, 'line': call.location.line})
        # nested accesses inside the offset expression (indices read from fields)
        if off_expr is not None and off is None:
            self.walk(off_expr, call, 1)
        return aid

    def facade(self, call):
        name = call.spelling
        if not (name.startswith(FACADE_PREFIX) or name in FACADE_NAMES):
            return
        args = list(call.get_arguments())
        if not args:
            return
        b = self.base_of(args[0])
        if b['b'] not in ('this', 'acc', 'param'):
            return
        res = call.type
        pointee = res.get_pointee().spelling if res.kind == ci.TypeKind.POINTER else res.spelling
        self.emit({'k': 'cast', 'fn': self.fn_usr, 'facade': name, 'as': pointee, 'base': b,
                   'line': call.location.line})


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('file')
    ap.add_argument('-o')
    a = ap.parse_args()
    recs = FileScan(a.file).run()
    out = open(a.o, 'w') if a.o else sys.stdout
    for r in recs:
        out.write(json.dumps(r) + '\n')


if __name__ == '__main__':
    main()
