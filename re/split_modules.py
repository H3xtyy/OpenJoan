#!/usr/bin/env python3
"""Move reimplemented functions out of the batch modules (src/modules/*.cpp) into files named after their original
source file (re/attribution.csv), keeping behaviour identical.

Usage (from the repository root; needs clang++ 18 and the MSVC / Windows header shims of the syntax checker):
  split_modules.py --group '^ww/en2d/' [--confidence anchor,range] [--module modules/x.cpp ...]
                   --shim DIR [--shim DIR] [--dry-run] [--report out.md] [--no-prune] [-v]
Then re-run re/attribute.py (current_file) and check with re/check_registrations.py --compare.

Targets: ww/<x>.cpp -> src/game/<x>.cpp, ww/<dir>/<x>.cpp -> src/<dir>/<x>.cpp (en2d, en3d, uti3d, interfac, ilib,
elib), unknown stretches -> src/game/_unplaced/after_<left neighbour>.cpp (from the evidence column).

How it works (per batch module that registers a selected function):
 1. clang's JSON AST of the module (MSVC target, the flags of the syntax checker) gives exact source ranges of the
    module's own declarations: the entities of its anonymous namespace(s) and its file-scope using-declarations /
    aliases (helpers, constants, counters, types, macros), the members of its records (`struct H`), and the references
    between them (declaration ids; completed by identifier matching so dependent template code, macros and array
    bounds are covered; members only where used like a member function, so a parameter named like one is not).
 2. Registrations (JOAN_REPLACE* through the module macro) are the Registrar variables of the AST; the function
    they register (`&H::member` / `&free_fn`) and what the registration line names are the roots of a move.
 3. Plan: the closure of the moved roots per target. Pure entities needed by several places (helpers, constants,
    types, using-declarations, macros) are copied; state (non-const namespace-scope variables, JOAN_COUNTERs, static
    data members, functions with static locals) is never copied: it moves when only one place needs it, otherwise the
    roots that share it stay where they are (reported). A moved member that needs a registered function that is not
    moved to the same target (e.g. `this->other_member()` of a function that stays) stays too (reported), and so does
    one that needs a non-anonymous definition of the module. A pure function registered at several addresses that go
    to different files is copied to each.
 4. Each target file gets one block per batch module (ordered by address): `namespace { namespace <module> { ... } }`
    with the needed entities in source order, the record (`struct H`) holding only the needed members (fields and
    static_asserts always), and the registrations inside the block with the module macro (same module key, name and
    address). `::name` of a block-local helper becomes `<module>::name`. Entities that several new blocks copy with
    identical text (and whose dependencies are hoisted too) go once into a shared anonymous namespace before them.
    The module's includes are merged into the target (relative paths rewritten); header-comment items that describe
    only moved functions move along (items naming moved and staying functions are copied).
 5. The batch module loses the moved registrations and every entity no longer needed; it is deleted when no
    registration (and no non-anonymous definition) remains, and src/CMakeLists.txt is updated.
 6. Prune: the changed files are syntax-checked with clang's unused-entity warnings (MSVC C4505 & co.); functions /
    namespace-scope variables that became unused (identifier matching over-approximates) are dropped and the files
    re-emitted. Entities already unused before the move are left alone.

Limits: modules with preprocessor conditionals in their code or clang errors, out-of-line member definitions and
registered functions defined outside the anonymous namespace are left in place (see the report). Comments between
entities stay in the module unless they directly precede a moved entity. Duplicated helpers (one copy per block)
are left for a later per-class clean-up. Const namespace-scope variables with dynamic initialisers are copied
(immutable; evaluated at static initialisation like before).
"""
import argparse
import collections
import csv
import json
import os
import re
import shlex
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fn_scan as F  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HOOK = os.path.join(ROOT, 'src')
SUBDIRS = ('en2d', 'en3d', 'uti3d', 'interfac', 'ilib', 'elib')
IDENT = re.compile(r'[A-Za-z_]\w*')
HEX_ID = re.compile(r'^0x[0-9a-f]+$')
UNUSED_RE = re.compile(r"^(?P<file>[^:]+):(?P<line>\d+):\d+: warning: unused (?P<what>function template|function|"
                       r"member function|variable|const variable|type alias|typedef) '(?P<name>[^']+)'")
# an item of a module's header comment: `- ...` / `* ...` bullets and `0x00xxxxxx Name ...` table rows
ITEM_RE = re.compile(r"^//(\s*)(?:[-*]\s|0x[0-9a-fA-F]{8}\s+[A-Za-z_~])")
BLANK_LINE = re.compile(r'\n[ \t]*\n')
PP_COND = re.compile(r'^[ \t]*#[ \t]*(if|ifdef|ifndef|elif|else|endif|pragma)\b', re.M)
PP_INCLUDE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*([<"])([^>"]+)[>"][^\n]*\n?', re.M)
PP_DEFINE = re.compile(r'^[ \t]*#[ \t]*define[ \t]+([A-Za-z_]\w*)((?:[^\n]*\\\n)*[^\n]*)\n?', re.M)
PP_UNDEF = re.compile(r'^[ \t]*#[ \t]*undef[ \t]+([A-Za-z_]\w*)[^\n]*\n?', re.M)


# ----------------------------------------------------------------------------------------------- attribution


def target_of(orig, evidence):
    """hook-relative target path for an original source file (None if out of scope)"""
    if orig == 'unknown':
        m = re.search(r'between (\S+) and (\S+)', evidence)
        if not m or not m.group(1).startswith('ww/'):
            return None
        left = m.group(1)[3:]
        return 'game/_unplaced/after_' + os.path.splitext(left)[0].replace('/', '_') + '.cpp'
    if not orig.startswith('ww/'):
        return None
    rest = orig[3:]
    d = os.path.dirname(rest)
    if not d:
        return 'game/' + rest
    if d in SUBDIRS:
        return rest
    return None


def load_segments(seg_dirs):
    """several directories (comma-separated, first wins): [(start, end, target)] in priority order"""
    out = []
    for d in seg_dirs.split(','):
        out += load_segments1(d)
    return out


def load_segments1(seg_dir):
    """re/segments/after_*.csv (start, end, name: objects proposed inside an _unplaced stretch) and _named.csv
    (segment -> an existing named file): sorted [(start, end, target)]"""
    named = {}
    p = os.path.join(seg_dir, '_named.csv')
    if os.path.exists(p):
        named = {r['segment']: r['target'] for r in csv.DictReader(open(p))}
    out = []
    for f in sorted(os.listdir(seg_dir)):
        if f.startswith('after_') and f.endswith('.csv'):
            for r in csv.DictReader(open(os.path.join(seg_dir, f))):
                t = (r.get('target') or '').strip() or named.get(r['name'], 'game/_unplaced/%s.cpp' % r['name'])
                out.append((int(r['start'], 16), int(r['end'], 16), t))
    return sorted(out)


def load_selection(csv_path, group, confidences, modules, unplaced=False, segments=None):
    """{module rel path (hook-relative): {address: (target, original file)}}"""
    sel = collections.defaultdict(dict)
    for row in csv.DictReader(open(csv_path)):
        if unplaced and 'stretch between' in row['evidence']:
            # guesses from link-order neighbours are not reliable enough for an original-named file:
            # the whole stretch goes to src/game/_unplaced/after_<left neighbour>.cpp
            row = dict(row, original_file='unknown')
        if not re.search(group, row['original_file']):
            continue
        if row['confidence'] not in confidences:
            continue
        cur = row['current_file']
        if not cur.startswith('src/'):
            continue
        rel = cur[len('src/'):]
        if modules and rel not in modules:
            continue
        if segments is not None:
            a = int(row['address'], 16)
            hit = [t for lo, hi, t in segments if lo <= a <= hi]
            if not hit:
                continue
            t = hit[0]
        else:
            t = target_of(row['original_file'], row['evidence'])
        if t is None or t == rel:
            continue
        sel[rel][int(row['address'], 16)] = (t, row['original_file'])
    return sel


# ----------------------------------------------------------------------------------------------- clang AST


class Clang:
    def __init__(self, shims, extra):
        self.flags = ['-std=c++20', '--target=i686-pc-windows-msvc', '-fms-extensions', '-fms-compatibility',
                      '-nostdinc++'] + ['-I' + s for s in shims] + ['-I.', '-DWIN32_LEAN_AND_MEAN', '-DNOMINMAX',
                                                                     '-include', 'registry.hpp', '-include',
                                                                     'data_names.hpp', '-include', 'fn_names.hpp']
        self.flags += extra

    def ast(self, rel):
        p = subprocess.run(['clang++'] + self.flags + ['-Xclang', '-ast-dump=json', '-fsyntax-only', rel], cwd=HOOK,
                           capture_output=True, text=True)
        if p.returncode != 0:
            raise RuntimeError('clang failed on %s:\n%s' % (rel, p.stderr[-3000:]))
        return json.loads(p.stdout)

    def warnings(self, rel, extra=()):
        p = subprocess.run(['clang++'] + self.flags + ['-fsyntax-only', '-Wall', '-Wextra', '-Wno-unused-parameter',
                                                       '-Wunused-function', '-Wunused-member-function',
                                                       '-Wunused-template', '-Wunused-const-variable'] + list(extra) +
                           [rel], cwd=HOOK, capture_output=True, text=True)
        return p.returncode, p.stderr


def annotate_files(node):
    """clang's JSON writes a location's file only when it changes: give every location dict its file ('_f')"""
    cur = [None]
    stack = [node]
    while stack:
        n = stack.pop()
        if isinstance(n, dict):
            if 'offset' in n:
                if 'file' in n:
                    cur[0] = n['file']
                n['_f'] = cur[0]
            # keep document order: push children reversed
            vals = [v for k, v in n.items() if k != 'includedFrom' and isinstance(v, (dict, list))]
            stack.extend(reversed(vals))
        elif isinstance(n, list):
            stack.extend(reversed(n))


def loc_point(loc):
    if loc is None:
        return None
    if 'expansionLoc' in loc:
        loc = loc['expansionLoc']
    if 'offset' not in loc:
        return None
    return loc


def node_ids(node, acc):
    """every id string in a subtree (own node ids and references)"""
    stack = [node]
    while stack:
        n = stack.pop()
        if isinstance(n, dict):
            if n.get('kind', '').endswith('TemplateDecl') and isinstance(n.get('inner'), list):
                # the pattern only: implicit instantiations reference their template arguments' declarations
                pat, seen = [], False
                for x in n['inner']:
                    if 'Template' in x.get('kind', '') and 'Parm' in x.get('kind', ''):
                        pat.append(x)
                    elif not seen:
                        pat.append(x)
                        seen = True
                n = dict(n)
                n['inner'] = pat
            for k, v in n.items():
                if isinstance(v, str):
                    if (k == 'id' or k in ('referencedMemberDecl', 'typeAliasDeclId', 'previousDecl',
                                           'parentDeclContextId')) and HEX_ID.match(v):
                        acc.add(v)
                elif isinstance(v, (dict, list)):
                    stack.append(v)
        elif isinstance(n, list):
            stack.extend(n)
    return acc


def own_ids(node, acc):
    """ids of the nodes of a subtree (declarations / statements, not references)"""
    stack = [node]
    while stack:
        n = stack.pop()
        if 'id' in n and n.get('kind', '').endswith('Decl'):  # type nodes are shared between declarations
            acc.add(n['id'])
        stack.extend(x for x in n.get('inner', []) if isinstance(x, dict))
    return acc


STMT_KINDS = {'TypeAliasDecl', 'TypedefDecl', 'UsingDecl', 'UsingDirectiveDecl', 'NamespaceAliasDecl', 'VarDecl',
              'FieldDecl', 'StaticAssertDecl'}
GLOBAL_COPYABLE = {'UsingDecl', 'UsingDirectiveDecl', 'NamespaceAliasDecl', 'TypeAliasDecl', 'TypedefDecl'}
FN_KINDS = {'FunctionDecl', 'FunctionTemplateDecl', 'CXXMethodDecl', 'CXXConstructorDecl', 'CXXDestructorDecl',
            'CXXConversionDecl'}


def referenced_decls(node):
    """ids of the declarations a subtree's DeclRefExprs name"""
    res = set()
    stack = [node]
    while stack:
        n = stack.pop()
        if n.get('kind') == 'DeclRefExpr' and 'referencedDecl' in n:
            res.add(n['referencedDecl']['id'])
        stack.extend(x for x in n.get('inner', []) if isinstance(x, dict))
    return res


def has_static_local(node):
    stack = list(node.get('inner', []))
    while stack:
        n = stack.pop()
        if not isinstance(n, dict):
            continue
        if n.get('kind') == 'VarDecl' and n.get('storageClass') == 'static' and not is_const_var(n):
            return True
        if n.get('tls'):
            return True
        stack.extend(n.get('inner', []))
    return False


def is_const_var(n):
    if n.get('constexpr'):
        return True
    t = n.get('type', {}).get('qualType', '').strip()
    if '*' in t or '&' in t:
        return t.endswith('const')
    return t.startswith('const ') or ' const' in t.split('[')[0]


# ----------------------------------------------------------------------------------------------- module model


class Entity:
    def __init__(self, key, kind, names, b, e, node=None):
        self.key = key
        self.kind = kind
        self.names = set(names)
        self.b, self.e = b, e  # extended text range in the module
        self.node = node
        self.ids = set()
        self.deps = set()
        self.stateful = False
        self.record = None  # member: key of its record entity
        self.members = []  # record: member keys in order
        self.always = False  # member that every copy of the record carries (fields, access specifiers, asserts)
        self.registered = False
        self.external = False
        self.follower = False  # static_assert: lives wherever all its dependencies live

    def __repr__(self):
        return '<%s %s>' % (self.kind, self.key)


class Registration:
    def __init__(self, b, e, address, name, module, macro, fn_keys, text):
        self.b, self.e = b, e
        self.address = address
        self.name = name
        self.module = module
        self.macro = macro
        self.fn_keys = fn_keys
        self.text = text


class Module:
    def __init__(self, rel, clang):
        self.rel = rel
        self.path = os.path.join(HOOK, rel)
        self.src = open(self.path).read()
        self.code = F.strip(self.src, strings=True)  # comments / strings blanked, same offsets
        self.problems = []
        self.ents = {}
        self.order = []
        self.regs = []
        self.macros = {}  # name -> (b, e, text)
        self.reg_macros = {}  # name -> (define text, has undef)
        self.ns_spans = []  # (b, e) of the anonymous namespace bodies
        ast = clang.ast(rel)
        annotate_files(ast)
        self._build(ast)

    # -------------------------------------------------- text helpers
    def line_start(self, i):
        return self.src.rfind('\n', 0, i) + 1

    def line_end(self, i):
        j = self.src.find('\n', i)
        return len(self.src) if j < 0 else j + 1

    def is_main(self, loc):
        p = loc_point(loc)
        return p is not None and p.get('_f') is not None and os.path.normpath(p['_f']) == os.path.normpath(self.rel)

    def core_range(self, node):
        r = node.get('range')
        if not r:
            return None
        b, e = loc_point(r.get('begin')), loc_point(r.get('end'))
        if b is None or e is None:
            return None
        if not (self.is_main(r['begin']) and self.is_main(r['end'])):
            return None
        bo, eo = b['offset'], e['offset'] + e.get('tokLen', 0)
        if 'expansionLoc' in r['end']:
            # a macro invocation: the expansion end is the macro name; take the whole invocation
            j = eo
            while j < len(self.src) and self.src[j] in ' \t':
                j += 1
            if j < len(self.src) and self.src[j] == '(':
                _, close = F.split_args(self.code, j)
                eo = close + 1
        return bo, eo

    def to_semicolon(self, b, e, hi):
        """end of a declaration statement: clang's range can stop short (e.g. `using F = void(__stdcall*)(int);`)"""
        j, depth = b, 0
        while j < hi:
            c = self.code[j]
            if c in '([{':
                depth += 1
            elif c in ')]}':
                if depth == 0:
                    return e
                depth -= 1
            elif c == ';' and depth == 0:
                return max(j, e)
            j += 1
        return e

    def extend(self, b, e, lo, hi, stmt=False):
        """core range -> the text that goes with the declaration: trailing ';' and comment, leading comment lines"""
        if stmt:
            e = self.to_semicolon(b, e, hi)
        j = e
        while j < hi and self.src[j] in ' \t':
            j += 1
        if j < hi and self.src[j] == ';':
            e = j + 1
        rest_end = self.line_end(e)
        rest = self.code[e:rest_end]
        if not rest.strip():
            e = min(rest_end, hi) if rest_end <= hi else e
        ls = self.line_start(b)
        if ls >= lo and not self.code[ls:b].strip():
            b = ls
            while True:
                pls = self.line_start(b - 1) if b > 0 else 0
                if pls < lo or b == 0:
                    break
                line = self.src[pls:b]
                if line.strip().startswith('//') and not self.code[pls:b].strip():
                    b = pls
                else:
                    break
        return b, e

    # -------------------------------------------------- building
    def _build(self, ast):
        tops = []  # (node, lo, hi) of entities in anonymous namespaces
        externals = []
        for c in ast.get('inner', []):
            if not self.is_main(c.get('loc')) or c.get('isImplicit'):
                continue
            k = c.get('kind')
            if k == 'NamespaceDecl' and 'name' not in c:
                cr = self.core_range(c)
                if cr is None:
                    self.problems.append('anonymous namespace without a source range')
                    continue
                body_lo = self.src.index('{', cr[0]) + 1
                body_hi = cr[1] - 1
                self.ns_spans.append((body_lo, body_hi))
                for cc in c.get('inner', []):
                    if cc.get('isImplicit') or cc.get('kind') == 'UsingShadowDecl' or cc.get('kind', '').endswith('Attr'):
                        continue
                    if cc.get('kind') == 'VarDecl' and cc.get('name', '').startswith('joan_replace_'):
                        self.regs.append(cc)  # registrations inside the anonymous namespace
                        continue
                    tops.append((cc, body_lo, body_hi))
            elif k == 'VarDecl' and c.get('name', '').startswith('joan_replace_'):
                self.regs.append(c)
            elif k in GLOBAL_COPYABLE:
                # file-scope using-declarations / aliases: copied like the anonymous namespace's
                cr = self.core_range(c)
                lo = 0
                for m in PP_INCLUDE.finditer(self.src):
                    if cr and m.end() <= cr[0]:
                        lo = m.end()
                tops.append((c, lo, len(self.src)))
            else:
                externals.append(c)
        # entities of the anonymous namespaces
        raw = []
        for node, lo, hi in tops:
            cr = self.core_range(node)
            if cr is None:
                self.problems.append('no source range for %s %s' % (node.get('kind'), node.get('name')))
                continue
            raw.append((cr, lo, hi, node))
        raw.sort(key=lambda x: x[0])
        groups = []
        for cr, lo, hi, node in raw:
            if groups and cr[0] < groups[-1][0][1]:
                g = groups[-1]
                g[0] = (g[0][0], max(g[0][1], cr[1]))
                g[3].append(node)
            else:
                groups.append([cr, lo, hi, [node]])
        for cr, lo, hi, nodes in groups:
            b, e = self.extend(cr[0], cr[1], lo, hi, stmt=all(n['kind'] in STMT_KINDS for n in nodes))
            names = set()
            for n in nodes:
                names |= self.decl_names(n)
            kind = nodes[0]['kind'] if len(nodes) == 1 else 'group'
            key = '%s@%d' % ('/'.join(sorted(names)) or kind, cr[0])
            ent = Entity(key, kind, names, b, e)
            ent.core = cr
            for n in nodes:
                own_ids(n, ent.ids)
            ent.nodes = nodes
            self.add(ent)
            if len(nodes) == 1 and nodes[0]['kind'] == 'CXXRecordDecl' and nodes[0].get('completeDefinition'):
                self.split_record(ent, nodes[0])
            for n in nodes:
                if n['kind'] == 'VarDecl' and not is_const_var(n):
                    # JOAN_COUNTERs are call statistics only: a copy per file is fine (the report merges
                    # counters of the same name)
                    if 'joan::hook::Counter' not in n.get('type', {}).get('qualType', ''):
                        ent.stateful = True
                elif n['kind'] in ('FunctionDecl', 'FunctionTemplateDecl', 'CXXMethodDecl') and has_static_local(n):
                    ent.stateful = True
                elif n['kind'] == 'StaticAssertDecl':
                    ent.follower = True
                elif n['kind'] == 'NamespaceDecl':
                    ent.external = True
                elif n['kind'] == 'UsingDirectiveDecl':
                    ent.everywhere = True
        # external (non-anonymous) definitions of the module: kept, never copied
        for n in externals:
            cr = self.core_range(n)
            key = 'external:%s@%s' % (n.get('name', n.get('kind')), cr[0] if cr else '?')
            ent = Entity(key, n.get('kind'), self.decl_names(n), cr[0] if cr else 0, cr[1] if cr else 0)
            own_ids(n, ent.ids)
            ent.external = True
            ent.nodes = [n]
            self.add(ent)
        # macros (outside the registration macros) of the module
        for m in PP_DEFINE.finditer(self.src):
            if not self.code[m.start():m.end()].strip().startswith('#'):
                continue  # inside a comment
            self.macros[m.group(1)] = (m.start(), m.end(), m.group(0))
        for name, (b, e, text) in self.macros.items():
            if re.search(r'\bJOAN_REPLACE\w*\s*\(', text):
                undef = re.search(r'^[ \t]*#[ \t]*undef[ \t]+' + re.escape(name) + r'\b', self.src, re.M)
                self.reg_macros[name] = (text if text.endswith('\n') else text + '\n', bool(undef))
                continue
            ent = Entity('macro:' + name, 'macro', {name}, b, e)
            ent.macro = True
            ent.nodes = []
            self.add(ent)
        # owner of every declaration id
        self.owner = {}
        for k in self.order:
            for i in self.ents[k].ids:
                self.owner.setdefault(i, k)
        # members own their ids (registered before the record's shell took the whole subtree)
        for k in self.order:
            ent = self.ents[k]
            if ent.record:
                for i in ent.ids:
                    self.owner[i] = k
        self.name_index = collections.defaultdict(set)
        for k in self.order:
            ent = self.ents[k]
            if ent.record is None and not ent.external:
                for nm in ent.names:
                    self.name_index[nm].add(k)
        # members first: the record shells collect their always-members' dependencies
        for k in sorted(self.order, key=lambda k: 0 if self.ents[k].record else 1):
            self.compute_deps(self.ents[k])
        everywhere = [k for k in self.order if getattr(self.ents[k], 'everywhere', False)]
        for k in self.order:
            if k not in everywhere and not self.ents[k].external:
                self.ents[k].deps |= set(everywhere)
        self.parse_registrations()
        for lo, hi in self.ns_spans:
            self.attach_gaps([k for k in self.order if self.ents[k].record is None and lo <= self.ents[k].b < hi], lo,
                             [(r.b, r.e) for r in self.regs_parsed if lo <= r.b < hi])
        self.finalize_records()
        conds = [m for m in PP_COND.finditer(self.src) if any(lo <= m.start() < hi for lo, hi in self.ns_spans) or
                 (self.regs and m.start() > min(r.b for r in self.regs_parsed))]
        if conds:
            self.problems.append('preprocessor conditionals / pragmas in the code (line %d)' %
                                 (self.src.count('\n', 0, conds[0].start()) + 1))
        for m in PP_INCLUDE.finditer(self.src):
            if any(lo <= m.start() < hi for lo, hi in self.ns_spans):
                self.problems.append('#include inside the anonymous namespace (line %d)' %
                                     (self.src.count('\n', 0, m.start()) + 1))

    def attach_gaps(self, keys, lo, barriers=()):
        """code between two entities (attributes such as `alignas(16)` / `__declspec(...)` in front of a declaration,
        which clang's range leaves out) goes with the following entity; preprocessor lines are left alone"""
        prev_e = lo
        items = [(self.ents[k].b, k) for k in keys] + [(b, (b, e)) for b, e in barriers]
        for _, k in sorted(items, key=lambda x: x[0]):
            if isinstance(k, tuple):
                prev_e = max(prev_e, k[1])
                continue
            ent = self.ents[k]
            if ent.b > prev_e:
                pos = prev_e
                first = None
                while pos < ent.b:
                    le = min(self.line_end(pos), ent.b)
                    line = self.code[pos:le]
                    if line.strip() and not line.lstrip().startswith('#'):
                        first = pos + (len(line) - len(line.lstrip()))
                        break
                    pos = le
                if first is not None:
                    nb = max(self.line_start(first), prev_e)
                    ent.b, _ = self.extend(nb, nb, prev_e, ent.e)
                    ent.b = min(ent.b, nb)
            prev_e = max(prev_e, ent.e)

    def finalize_records(self):
        """records without registered members are copied whole; in the others the special members (constructors,
        destructors, conversions, operators, virtuals) always go with the record (they are called implicitly)"""
        for k in list(self.order):
            rec = self.ents[k]
            if not rec.members:
                continue
            whole = not any(self.ents[m].registered for m in rec.members)
            for m in rec.members:
                me = self.ents[m]
                n = me.nodes[0] if me.nodes else {}
                special = me.kind in ('CXXConstructorDecl', 'CXXDestructorDecl', 'CXXConversionDecl') or \
                    any(x.startswith('operator') for x in me.names) or n.get('virtual')
                if (whole or special) and not me.registered:
                    me.always = True
            for m in rec.members:
                if self.ents[m].always:
                    rec.deps |= {d for d in self.ents[m].deps if d != rec.key}

    def decl_names(self, n):
        names = set()
        k = n.get('kind')
        nm = n.get('name')
        if k == 'UsingDecl' and nm:
            names.add(nm.split('::')[-1])
        elif nm:
            names.add(nm)
        if k == 'EnumDecl':
            for c in n.get('inner', []):
                if c.get('name'):
                    names.add(c['name'])
        return names

    def add(self, ent):
        self.ents[ent.key] = ent
        self.order.append(ent.key)

    def split_record(self, rec, node):
        brace = self.src.index('{', rec.core[0])
        rec.head_end = self.line_end(brace) if not self.code[brace + 1:self.line_end(brace)].strip() else brace + 1
        close = rec.core[1] - 1  # the '}'
        rec.tail_begin = self.line_start(close) if not self.code[self.line_start(close):close].strip() else close
        for m in node.get('inner', []):
            if m.get('isImplicit') or m.get('kind', '').endswith('Attr'):
                continue
            cr = self.core_range(m)
            if cr is None:
                continue
            b, e = self.extend(cr[0], cr[1], rec.head_end, rec.tail_begin, stmt=m['kind'] in STMT_KINDS)
            names = self.decl_names(m)
            key = '%s::%s@%d' % (sorted(rec.names)[0] if rec.names else 'rec', '/'.join(sorted(names)) or
                                 m['kind'], cr[0])
            if self.ents and key in self.ents:
                continue
            me = Entity(key, m['kind'], names, b, e, m)
            me.core = cr
            me.nodes = [m]
            me.record = rec.key
            own_ids(m, me.ids)
            if m['kind'] in ('FieldDecl', 'AccessSpecDecl', 'StaticAssertDecl', 'IndirectFieldDecl'):
                me.always = True
            if m['kind'] == 'VarDecl' and not is_const_var(m):
                me.stateful = True
            if m['kind'] in ('CXXMethodDecl', 'FunctionTemplateDecl', 'CXXConstructorDecl', 'CXXDestructorDecl') \
                    and has_static_local(m):
                me.stateful = True
            rec.members.append(key)
            self.add(me)
        # members that overlap (e.g. `int a, b;`) are merged into the first
        prev = None
        for k in list(rec.members):
            me = self.ents[k]
            if prev is not None and me.b < self.ents[prev].e:
                pe = self.ents[prev]
                pe.e = max(pe.e, me.e)
                pe.names |= me.names
                pe.ids |= me.ids
                pe.always = pe.always or me.always
                pe.stateful = pe.stateful or me.stateful
                rec.members.remove(k)
                self.order.remove(k)
                del self.ents[k]
                continue
            prev = k
        self.attach_gaps(rec.members, rec.head_end)

    def text_tokens(self, b, e):
        return set(IDENT.findall(self.code[b:e]))

    def compute_deps(self, ent):
        refs = set()
        for n in getattr(ent, 'nodes', []):
            if ent.members:
                mine = set()
                for k in ent.members:
                    mine |= self.ents[k].ids
                n = dict(n)
                n['inner'] = [x for x in n.get('inner', []) if x.get('id') not in mine]
            node_ids(n, refs)
        deps = set()
        for i in refs:
            o = self.owner.get(i)
            if o and o != ent.key:
                deps.add(o)
        if getattr(ent, 'macro', False):
            toks = set(IDENT.findall(F.strip(self.src[ent.b:ent.e], strings=True)))
        elif ent.members:
            # the record's own text outside its members
            toks = self.text_tokens(ent.b, ent.head_end) | self.text_tokens(ent.tail_begin, ent.e)
            for k in ent.members:
                if self.ents[k].always:
                    toks |= self.text_tokens(self.ents[k].b, self.ents[k].e)
        else:
            toks = self.text_tokens(ent.b, ent.e)
        for t in toks:
            for k in self.name_index.get(t, ()):
                if k != ent.key:
                    deps.add(k)
            mk = 'macro:' + t
            if mk in self.ents and mk != ent.key:
                deps.add(mk)
        if ent.record:
            rec = self.ents[ent.record]
            deps.add(rec.key)
            members_by_name = collections.defaultdict(set)
            for k in rec.members:
                for nm in self.ents[k].names:
                    members_by_name[nm].add(k)
            # members by name only where used like a member function (call / template / qualified): a parameter
            # or local named like a member is not a reference (non-dependent references come from the AST)
            code = self.code[ent.b:ent.e]
            mtoks = {m.group(1) for m in re.finditer(r'\b([A-Za-z_]\w*)\s*[(<]', code)} | \
                {m.group(1) for m in re.finditer(r'::\s*([A-Za-z_]\w*)', code)}
            for t in mtoks:
                for k in members_by_name.get(t, ()):
                    if k != ent.key:
                        deps.add(k)
            # nested types / aliases / enums / static data members of the record: any use of the name
            for t in toks:
                for k in members_by_name.get(t, ()):
                    if k != ent.key and self.ents[k].kind not in FN_KINDS and self.ents[k].kind not in (
                            'FieldDecl', 'IndirectFieldDecl'):
                        deps.add(k)
        if ent.members:
            # the record shell needs what its always-members (fields, ...) need, not its other members
            deps = {d for d in deps if self.ents[d].record != ent.key}
            for k in ent.members:
                if self.ents[k].always:
                    deps |= {d for d in self.ents[k].deps if d != ent.key}
        ent.deps = deps

    def parse_registrations(self):
        self.regs_parsed = []
        for n in self.regs:
            cr = self.core_range(n)
            if cr is None:
                self.problems.append('registration %s without a source range' % n.get('name'))
                continue
            b, e = self.extend(cr[0], cr[1], 0, len(self.src))
            text = self.src[cr[0]:cr[1]]
            macro = IDENT.match(text).group(0)
            strings, ints = [], []
            stack = [n]
            while stack:
                x = stack.pop()
                if isinstance(x, dict):
                    if x.get('kind') == 'StringLiteral':
                        strings.append((x['range']['begin'].get('expansionLoc', x['range']['begin']).get('offset', 0),
                                        x['value']))
                    elif x.get('kind') == 'IntegerLiteral':
                        ints.append(int(x['value']))
                    for v in x.values():
                        if isinstance(v, (dict, list)):
                            stack.append(v)
                elif isinstance(x, list):
                    stack.extend(x)
            fn_keys = set()
            for i in referenced_decls(n):
                o = self.owner.get(i)
                if o and self.ents[o].kind in FN_KINDS:
                    fn_keys.add(o)
            ctor_args = self.registrar_args(n)
            name, module, address = ctor_args
            r = Registration(b, e, address, name, module, macro, fn_keys, self.src[b:e])
            # what the registration line itself names (e.g. a using-declaration of a function defined elsewhere)
            r.roots = set(fn_keys)
            for t in self.text_tokens(cr[0], cr[1]) - {macro}:
                r.roots |= self.name_index.get(t, set())
                if 'macro:' + t in self.ents:
                    r.roots.add('macro:' + t)
            self.regs_parsed.append(r)
            for k in fn_keys:
                self.ents[k].registered = True

    def registrar_args(self, n):
        """(name, module, address) from the Registrar constructor call"""
        stack = [n]
        while stack:
            x = stack.pop()
            if isinstance(x, dict):
                if x.get('kind') == 'CXXConstructExpr' and len(x.get('inner', [])) >= 3:
                    args = x['inner']

                    def lit(a, kind):
                        st = [a]
                        while st:
                            y = st.pop()
                            if isinstance(y, dict):
                                if y.get('kind') == kind:
                                    return y['value']
                                st.extend(y.get('inner', []))
                        return None
                    name = lit(args[0], 'StringLiteral')
                    module = lit(args[1], 'StringLiteral')
                    addr = lit(args[2], 'IntegerLiteral')
                    if name is not None and module is not None and addr is not None:
                        return json.loads(name), json.loads(module), int(addr)
                for v in x.values():
                    if isinstance(v, (dict, list)):
                        stack.append(v)
            elif isinstance(x, list):
                stack.extend(x)
        return None, None, None

    def closure(self, roots):
        seen = set()
        stack = list(roots)
        while stack:
            k = stack.pop()
            if k in seen:
                continue
            seen.add(k)
            stack.extend(self.ents[k].deps)
        return seen


# ----------------------------------------------------------------------------------------------- planning


def plan_module(mod, sel, log):
    """decide which registrations move where; returns (moving {reg index: target}, blocked [(reg, reason)])"""
    regs = mod.regs_parsed
    moving = {}
    blocked = []
    if mod.problems:
        for i, r in enumerate(regs):
            if r.address in sel:
                blocked.append((r, 'module: ' + '; '.join(mod.problems)))
        return moving, blocked, {}
    for i, r in enumerate(regs):
        if r.address not in sel:
            continue
        if any(mod.ents[k].external for k in r.fn_keys):
            blocked.append((r, 'registered function is a non-anonymous definition'))
            continue
        moving[i] = sel[r.address][0]
    # a function registered at several addresses that go to different places is copied to each of them when it
    # is pure (no state in its closure); otherwise its registrations stay together
    while True:
        changed = False
        by_ent = collections.defaultdict(set)
        for i, r in enumerate(regs):
            for k in r.fn_keys:
                by_ent[k].add(moving.get(i))
        for i in list(moving):
            r = regs[i]
            for k in r.fn_keys:
                if len(by_ent[k]) > 1:
                    st = [d for d in mod.closure({k}) if mod.ents[d].stateful]
                    if st:
                        blocked.append((r, 'function also registered at an address that does not move with it, '
                                           'and it has state (%s)' % ', '.join(sorted(st))))
                        del moving[i]
                        changed = True
                        break
        if not changed:
            break
    reach = {i: mod.closure(regs[i].roots) for i in moving}
    while True:
        placement = compute_placement(mod, moving)
        need, reasons = placement['need'], {}
        bad = set()
        targets = set(moving.values())
        for t in targets:
            roots_t = placement['roots'][t]
            for k in need[t]:
                ent = mod.ents[k]
                why = None
                if ent.external:
                    why = 'needs %s, a non-anonymous definition of the module' % k
                elif ent.registered and k not in roots_t:
                    why = 'needs %s, a registered function that does not move to %s' % (k, t)
                elif ent.stateful:
                    places = [p for p in placement['places'][k]]
                    if len(places) > 1:
                        why = 'shares state %s with %s' % (k, ', '.join(sorted(p or 'functions staying in the module'
                                                                               for p in places if p != t)))
                if why:
                    for i, tt in moving.items():
                        if tt == t and k in reach[i]:
                            bad.add(i)
                            reasons.setdefault(i, why)
        if not bad:
            return moving, blocked, placement
        for i in bad:
            blocked.append((regs[i], reasons[i]))
            del moving[i]


def compute_placement(mod, moving):
    regs = mod.regs_parsed
    roots = collections.defaultdict(set)
    for i, t in moving.items():
        roots[t] |= regs[i].roots
    moved_all = set().union(*roots.values()) if roots else set()
    reach_moved = mod.closure(moved_all)
    roots_r = set()
    for i, r in enumerate(regs):
        if i not in moving:
            roots_r |= r.roots
    for k in mod.order:
        ent = mod.ents[k]
        if k not in reach_moved and not ent.follower:
            if ent.always and ent.record in reach_moved:
                continue  # fields / special members follow their record (they would keep a dead record alive)
            roots_r.add(k)
    need = {t: mod.closure(rs) for t, rs in roots.items()}
    need_r = mod.closure(roots_r)
    # records: a member needs its record, the record keeps its always-members
    places = collections.defaultdict(set)
    for t, ks in need.items():
        for k in ks:
            places[k].add(t)
    for k in need_r:
        places[k].add('')
    # macros never leave the module (unused macros are harmless)
    for k in mod.order:
        if getattr(mod.ents[k], 'macro', False):
            places[k].add('')
    # followers (static_assert): wherever all their dependencies are
    for k in mod.order:
        ent = mod.ents[k]
        if ent.follower and ent.record is None:
            locs = None
            for d in ent.deps:
                locs = set(places[d]) if locs is None else locs & places[d]
            for p in (locs if locs is not None else {''}):
                places[k].add(p)
                if p:
                    need[p].add(k)
                else:
                    need_r.add(k)
    return {'roots': roots, 'need': need, 'need_r': need_r, 'places': places}


# ----------------------------------------------------------------------------------------------- emission


class Out(list):
    """text pieces plus the spans of the emitted entities: [(start, end, key)]"""
    def __init__(self):
        super().__init__()
        self.n = 0
        self.spans = []

    def append(self, x):
        super().append(x)
        self.n += len(x)

    def entity(self, key, text):
        self.spans.append((self.n, self.n + len(text), key))
        self.append(text)


def block_text(mod, keys, regs, ns):
    """(text, spans) of one target block: entities of keys (source order), records with their needed members, the
    registrations"""
    out = Out()
    keys = set(keys)
    top = [k for k in mod.order if k in keys and mod.ents[k].record is None and not getattr(mod.ents[k], 'macro',
                                                                                              False)]
    macros = [k for k in mod.order if k in keys and getattr(mod.ents[k], 'macro', False)]
    for k in macros:
        out.append(mod.src[mod.ents[k].b:mod.ents[k].e].rstrip('\n') + '\n')
    if macros:
        out.append('\n')
    out.append('namespace {\nnamespace %s {\n\n' % ns)
    top.sort(key=lambda k: mod.ents[k].b)
    local = set()
    for k in top:
        local |= mod.ents[k].names

    def text_of(b, e):
        """source text; `::name` of a block-local entity (global qualification that found it in the anonymous
        namespace) becomes `ns::name`"""
        t = mod.src[b:e]
        code = mod.code[b:e]
        res, pos = [], 0
        for m in re.finditer(r'(?<![\w:>)\]])::\s*([A-Za-z_]\w*)', code):
            if m.group(1) in local:
                res.append(t[pos:m.start()])
                res.append('%s::%s' % (ns, m.group(1)))
                pos = m.end()
        res.append(t[pos:])
        return ''.join(res)

    def sep(prev, ent):
        """a blank line between two emitted entities when the source has one between them"""
        if prev is None:
            return ''
        return '\n' if BLANK_LINE.search(mod.src[prev.e - 1:ent.b]) else ''
    prev = None
    for k in top:
        ent = mod.ents[k]
        out.append(sep(prev, ent))
        prev = ent
        if ent.members:
            out.append(mod.src[ent.b:ent.head_end])
            mprev = None
            for mk in ent.members:
                me = mod.ents[mk]
                if mk in keys or me.always:
                    out.append(sep(mprev, me))
                    out.entity(mk, text_of(me.b, me.e).rstrip('\n') + '\n')
                    mprev = me
            out.append(mod.src[ent.tail_begin:ent.e].rstrip('\n') + '\n')
        else:
            out.entity(k, text_of(ent.b, ent.e).rstrip('\n') + '\n')
    if top:
        out.append('\n')
    used_macros = []
    for r in regs:
        if r.macro in mod.reg_macros and r.macro not in used_macros:
            used_macros.append(r.macro)
    for m in used_macros:
        out.append(mod.reg_macros[m][0])
    for r in regs:
        out.append(mod.src[r.b:r.e].strip('\n') + '\n')
    for m in used_macros:
        out.append('#undef %s\n' % m)
    out.append('\n}  // namespace %s\n}  // namespace\n' % ns)
    for k in macros:
        out.append('#undef %s\n' % mod.ents[k].names.copy().pop())
    return ''.join(out), out.spans


def header_bullets(mod):
    """[(start, end, text, addresses)] of the bullets of the module's leading comment"""
    lines = mod.src.split('\n')
    pos, res, cur = 0, [], None
    offs = []
    for ln in lines:
        offs.append(pos)
        pos += len(ln) + 1
    n = 0
    while n < len(lines) and lines[n].startswith('//'):
        n += 1
    i = 0
    while i < n:
        m = ITEM_RE.match(lines[i])
        if m:
            ind = len(m.group(1))
            j = i + 1
            while j < n:
                mm = re.match(r'^//(\s*)(\S)', lines[j])
                if not mm or len(mm.group(1)) <= ind or ITEM_RE.match(lines[j]):
                    break
                j += 1
            text = '\n'.join(lines[i:j])
            addrs = {int(a, 16) for a in re.findall(r'0x([0-9a-fA-F]{6,8})\b', text)}
            res.append((offs[i], offs[j] if j < len(offs) else len(mod.src), text, addrs, lines[i].startswith('//  *')))
            i = j
        else:
            i += 1
    return res


def rewrite_include(line, src_dir, dst_dir):
    m = re.match(r'^([ \t]*#[ \t]*include[ \t]*)"([^"]+)"(.*)$', line)
    if not m:
        return line
    cand = os.path.normpath(os.path.join(src_dir, m.group(2)))
    if not os.path.exists(cand):
        return line
    rel = os.path.relpath(cand, dst_dir).replace(os.sep, '/')
    return '%s"%s"%s' % (m.group(1), rel, m.group(3))


def include_lines(mod, target_rel):
    res = []
    for m in PP_INCLUDE.finditer(mod.src):
        if any(lo <= m.start() < hi for lo, hi in mod.ns_spans):
            continue
        res.append(rewrite_include(m.group(0).rstrip('\n'), os.path.dirname(mod.path),
                                   os.path.dirname(os.path.join(HOOK, target_rel))))
    return res


def remove_ranges(text, ranges):
    """text without ranges; a seam between two blank lines keeps one blank line"""
    ranges = sorted(ranges)
    merged = []
    for b, e in ranges:
        if merged and b <= merged[-1][1]:
            merged[-1] = (merged[-1][0], max(merged[-1][1], e))
        else:
            merged.append((b, e))
    out, pos, kept = [], 0, []
    for b, e in merged:
        out.append(text[pos:b])
        kept.append((pos, b))
        pos = e
        # drop a blank line that would double the one before the removed text
        before = ''.join(out)
        if (before.endswith('\n\n') or before == '') and text[pos:pos + 1] == '\n':
            pos += 1
    out.append(text[pos:])
    kept.append((pos, len(text)))
    return ''.join(out), kept


def old_offset(kept, new):
    """offset in the original text of offset `new` of remove_ranges' result"""
    n = 0
    for b, e in kept:
        if new < n + (e - b):
            return b + (new - n)
        n += e - b
    return len(kept) and kept[-1][1]


def source_after(mod, moving, placement, header_moves):
    """the module's text without the moved registrations and the entities only moved code needed"""
    regs = mod.regs_parsed
    ranges = [(regs[i].b, regs[i].e) for i in moving]
    places = placement['places']
    for k in mod.order:
        ent = mod.ents[k]
        if ent.external or getattr(ent, 'macro', False) or '' in places.get(k, ()):
            continue
        if ent.record and '' not in places.get(ent.record, ()):
            continue  # the whole record goes
        if ent.always:
            continue  # fields etc. go with their record
        ranges.append((ent.b, ent.e))
    ranges += header_moves
    text, kept = remove_ranges(mod.src, ranges)
    mod.kept = kept
    return text


HOIST_KINDS = {'TypeAliasDecl', 'TypedefDecl', 'UsingDecl', 'UsingDirectiveDecl', 'FunctionTemplateDecl', 'FunctionDecl',
               'CXXRecordDecl', 'VarDecl', 'EnumDecl', 'NamespaceAliasDecl'}


def hoist(blocks):
    """(prelude text, {module rel: hoisted keys}): top-level entities that several blocks of one target copy with
    identical text (and whose dependencies are hoisted too) go once into a shared anonymous namespace before them"""
    occ = collections.defaultdict(list)  # (kind, names / text) -> [(block index, key, normalised text)]
    for bi, (ns, rel, mod, keys, t_regs, bullets) in enumerate(blocks):
        for k in keys:
            ent = mod.ents[k]
            if ent.record or getattr(ent, 'macro', False) or ent.external or ent.stateful or ent.registered:
                continue
            if any(n['kind'] not in HOIST_KINDS for n in ent.nodes):
                continue
            if any(not mod.ents[m].always for m in ent.members):
                continue  # a record whose members are split per block
            core = mod.src[ent.core[0]:ent.core[1]] if hasattr(ent, 'core') else mod.src[ent.b:ent.e]
            norm = ' '.join(F.strip(core, strings=False).split())
            ident = (ent.kind, tuple(sorted(ent.names)) or norm)
            occ[ident].append((bi, k, norm))
    ok = {}
    for ident, lst in occ.items():
        if len({bi for bi, _, _ in lst}) < 2 or len(lst) != len({bi for bi, _, _ in lst}):
            continue
        if len({norm for _, _, norm in lst}) != 1:
            continue
        ok[ident] = lst
    # a name defined differently in some block (different kind) is not hoisted either
    names_bad = set()
    for ident, lst in occ.items():
        if ident not in ok and isinstance(ident[1], tuple):
            names_bad |= set(ident[1])
    ok = {i: l for i, l in ok.items() if not (isinstance(i[1], tuple) and set(i[1]) & names_bad)}
    seen_names = collections.Counter(n for i in ok if isinstance(i[1], tuple) for n in i[1])
    ok = {i: l for i, l in ok.items() if not (isinstance(i[1], tuple) and any(seen_names[n] > 1 for n in i[1]))}
    # dependencies of a hoisted entity must be hoisted too
    while True:
        hk = {(bi, k) for lst in ok.values() for bi, k, _ in lst}
        drop = set()
        for ident, lst in ok.items():
            for bi, k, _ in lst:
                mod = blocks[bi][2]
                if any((bi, d) not in hk for d in mod.ents[k].deps):
                    drop.add(ident)
        if not drop:
            break
        for i in drop:
            del ok[i]
    if not ok:
        return '', {}, []
    # emit in dependency order (source order of the first block that has it as tie-break)
    first = {ident: lst[0] for ident, lst in ok.items()}
    by_key = {(bi, k): ident for ident, lst in ok.items() for bi, k, _ in lst}
    order, done = [], set()

    def visit(ident):
        if ident in done:
            return
        done.add(ident)
        bi, k, _ = first[ident]
        mod = blocks[bi][2]
        for d in sorted(mod.ents[k].deps, key=lambda d: mod.ents[d].b):
            if (bi, d) in by_key:
                visit(by_key[(bi, d)])
        order.append(ident)
    for ident in sorted(ok, key=lambda i: (first[i][0], blocks[first[i][0]][2].ents[first[i][1]].b)):
        visit(ident)
    out, prev = Out(), None
    for ident in order:
        bi, k, _ = first[ident]
        mod = blocks[bi][2]
        ent = mod.ents[k]
        if prev is not None and (prev[0] != bi or BLANK_LINE.search(mod.src[prev[1].e - 1:ent.b])):
            out.append('\n')
        out.entity([(blocks[b][1], kk) for b, kk, _ in ok[ident]], mod.src[ent.b:ent.e].rstrip('\n') + '\n')
        prev = (bi, ent)
    hoisted = collections.defaultdict(set)
    for lst in ok.values():
        for bi, k, _ in lst:
            hoisted[blocks[bi][1]].add(k)
    return ''.join(out), hoisted, out.spans


def line_offset(text, line):
    """offset of the start of 1-based line `line` of text"""
    off = 0
    for _ in range(line - 1):
        j = text.find('\n', off)
        if j < 0:
            return len(text)
        off = j + 1
    return off


def block_ns_name(rel):
    """namespace of a moved block: the batch module's name (modules/game_b98_03.cpp -> game_b98_03), other files
    with their directory (ilib/file.cpp -> ilib_file)"""
    stem = os.path.splitext(rel)[0]
    if stem.startswith('modules/'):
        stem = stem[len('modules/'):]
    return re.sub(r'\W', '_', stem)


def target_header(target_rel, original):
    return ('// %s: functions of the original d:\\%s (re/attribution.csv).\n'
            '//\n'
            '// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a\n'
            '// namespace named after that module and registers its functions with the module\'s key (joan_re.ini\n'
            '// [Hooks] Disable= and the logs use it), name and address unchanged.\n') % (
        target_rel, original.replace('/', '\\'))


# ----------------------------------------------------------------------------------------------- driver


class Split:
    def __init__(self, args):
        self.args = args
        self.clang = Clang(args.shim, [])
        self.report = []
        self.targets = collections.OrderedDict()  # target rel -> {'original', 'includes', 'blocks': [(ns, text, bullets)]}
        self.changed = {}  # module rel -> new text (None = delete)
        self.mods = {}
        self.pre = {}  # target rel -> its text before this run (None = new)

    def run(self):
        a = self.args
        sel = load_selection(os.path.join(ROOT, 're', 'attribution.csv'), a.group, set(a.confidence.split(',')),
                             set(a.module or []), a.unplaced,
                             load_segments(a.segments) if a.segments else None)
        self.sel = sel
        for n, rel in enumerate(sorted(sel)):
            if a.verbose:
                print('[%d/%d] %s' % (n + 1, len(sel), rel), flush=True)
            self.do_module(rel, sel[rel])
        if not a.no_prune and not a.dry_run:
            self.prune()
        self.emit_all(write=not a.dry_run)
        if not a.dry_run:
            self.update_cmake()
        self.write_report()

    def do_module(self, rel, sel):
        try:
            mod = Module(rel, self.clang)
        except RuntimeError as ex:
            self.report.append(('module', rel, 'clang failed', str(ex)[:300]))
            return
        self.mods[rel] = mod
        moving, blocked, placement = plan_module(mod, sel, self.report)
        for r, why in blocked:
            self.report.append(('blocked', rel, '%s 0x%08x' % (r.name, r.address), why))
        missing = set(sel) - {r.address for r in mod.regs_parsed}
        for a_ in sorted(missing):
            self.report.append(('blocked', rel, '0x%08x' % a_, 'no registration of this address found in the module'))
        mod.moving = moving
        mod.placement = placement
        mod.excluded = collections.defaultdict(set)  # location -> entity keys pruned there
        for i, t in sorted(moving.items()):
            r = mod.regs_parsed[i]
            self.report.append(('moved', rel, '%s 0x%08x' % (r.name, r.address), t))

    # ------------------------------------------------------------------ emission
    def module_outputs(self, mod):
        """({target: block text}, new module text or None, {target: [bullet text]})"""
        moving, placement = mod.moving, mod.placement
        if not moving:
            return {}, mod.src, {}
        regs = mod.regs_parsed
        bullets = header_bullets(mod)
        header_moves, tb = [], collections.defaultdict(list)
        moving_addr = {regs[i].address: t for i, t in moving.items()}
        own = {r.address for r in regs}
        for b, e, text, addrs, _ in bullets:
            mine = addrs & own
            if not mine:
                continue
            ts = {moving_addr.get(x) for x in mine}
            if len(ts) == 1 and None not in ts:
                tb[ts.pop()].append(text)
                header_moves.append((b, e))
            else:
                for t in ts - {None}:
                    tb[t].append(text)
        blocks = {}
        for t in sorted(set(moving.values())):
            keys = set(placement['need'][t]) - mod.excluded[t]
            # members need their record
            for k in list(keys):
                if mod.ents[k].record:
                    keys.add(mod.ents[k].record)
            t_regs = [regs[i] for i in sorted(moving) if moving[i] == t]
            blocks[t] = keys, t_regs
        excl = mod.excluded['']
        if excl:
            placement = dict(placement)
            places = collections.defaultdict(set, {k: set(v) for k, v in placement['places'].items()})
            for k in excl:
                places[k].discard('')
            placement['places'] = places
        remaining = len(regs) - len(moving)
        has_external = any(mod.ents[k].external for k in mod.order)
        if remaining == 0 and not has_external:
            new_src = None
        else:
            new_src = source_after(mod, moving, placement, header_moves)
        return blocks, new_src, tb

    def emit_all(self, write):
        self.targets = collections.OrderedDict()
        self.changed = {}
        for rel, mod in self.mods.items():
            blocks, new_src, tb = self.module_outputs(mod)
            if not blocks:
                continue
            self.changed[rel] = new_src
            ns = block_ns_name(rel)
            for t, (keys, t_regs) in blocks.items():
                tgt = self.targets.setdefault(t, {'original': self.sel[rel][t_regs[0].address][1], 'includes': [],
                                                  'blocks': []})
                for inc in include_lines(mod, t):
                    if inc not in tgt['includes']:
                        tgt['includes'].append(inc)
                tgt['blocks'].append((ns, rel, mod, keys, t_regs, tb.get(t, [])))
        self.outputs = {}
        for t, tgt in self.targets.items():
            self.outputs[t] = self.target_text(t, tgt)
        if write:
            for t, text in self.outputs.items():
                p = os.path.join(HOOK, t)
                os.makedirs(os.path.dirname(p), exist_ok=True)
                open(p, 'w').write(text)
            for rel, text in self.changed.items():
                p = os.path.join(HOOK, rel)
                if text is None:
                    if os.path.exists(p):
                        os.remove(p)
                else:
                    open(p, 'w').write(text)

    def target_text(self, t, tgt):
        if t not in self.pre:
            p = os.path.join(HOOK, t)
            self.pre[t] = open(p).read() if os.path.exists(p) else None
        existing = self.pre[t]
        if existing is not None:
            text = existing
            incs = [ln for ln in tgt['includes'] if ln not in text]
            if incs:
                last = list(PP_INCLUDE.finditer(text))[-1]
                text = text[:last.end()] + '\n'.join(incs) + '\n' + text[last.end():]
        else:
            text = target_header(t, tgt['original']) + '\n'
            sys_incs = [i for i in tgt['includes'] if '<' in i.split('include', 1)[1][:3]]
            loc_incs = [i for i in tgt['includes'] if i not in sys_incs]
            text += '\n'.join(sys_incs) + ('\n\n' if sys_incs else '') + '\n'.join(loc_incs) + '\n'
        blocks = sorted(tgt['blocks'], key=lambda b: min(r.address for r in b[4]))
        hoisted = {}
        spans = []  # (start, end, [(module rel, key)])
        if existing is None:
            prelude, hoisted, pspans = hoist(blocks)
            tgt['hoisted'] = hoisted
            if prelude:
                text += '\n// Helpers shared by the blocks below (identical in their batch modules).\nnamespace {\n\n'
                spans += [(len(text) + a, len(text) + b, pairs) for a, b, pairs in pspans]
                text += prelude + '\n}  // namespace\n'
        tgt['spans'] = spans
        for bi, (ns, rel, mod, keys, t_regs, bullets) in enumerate(blocks):
            keys = set(keys) - hoisted.get(rel, set())
            name = ns
            n = 2
            while re.search(r'^namespace %s \{' % re.escape(name), text, re.M):
                name = '%s_%d' % (ns, n)
                n += 1
            mk = t_regs[0].module
            head = '\n// ' + '-' * 40 + ' from %s (%s)\n' % (rel, mk)
            for b in bullets:
                head += '//\n' + b + '\n'
            text += head
            btext, bspans = block_text(mod, keys, t_regs, name)
            spans += [(len(text) + a, len(text) + b, [(rel, k)]) for a, b, k in bspans]
            text += btext
        return text

    def update_cmake(self):
        p = os.path.join(HOOK, 'CMakeLists.txt')
        lines = open(p).read().split('\n')
        start = next(i for i, ln in enumerate(lines) if ln.startswith('add_library(joan_hook_core'))
        end = next(i for i in range(start, len(lines)) if lines[i].strip() == ')')
        listed = {ln.strip() for ln in lines[start + 1:end]}
        new = sorted(t for t in self.outputs if t not in listed)
        for rel, text in self.changed.items():
            if text is None and rel in listed:
                idx = next(i for i in range(start, end) if lines[i].strip() == rel)
                del lines[idx]
                end -= 1
        if new:
            marker = '    # original source files (re/split_modules.py)'
            if marker not in lines[start:end]:
                lines.insert(end, marker)
                end += 1
            m = lines.index(marker, start)
            block_end = m + 1
            while block_end < end and lines[block_end].startswith('    ') and not lines[block_end].strip().startswith('#'):
                block_end += 1
            items = sorted(set(ln.strip() for ln in lines[m + 1:block_end]) | set(new))
            lines[m + 1:block_end] = ['    ' + x for x in items]
        open(p, 'w').write('\n'.join(lines))

    # ------------------------------------------------------------------ prune
    def prune(self):
        """drop copied / kept entities that clang reports unused in the emitted files (identifier matching
        over-approximates); a warning counts when it names an entity at the entity's own place in the file.
        Entities of a module that were already unused before the move are left alone."""
        base = collections.defaultdict(set)
        for rel, mod in self.mods.items():
            if not mod.moving:
                continue
            for line, what, name in self.unused(rel, None, lines=True):
                off = line_offset(mod.src, line)
                for k in mod.order:
                    if mod.ents[k].b <= off < mod.ents[k].e and name.split('::')[-1] in mod.ents[k].names:
                        base[rel].add(k)
        for rnd in range(6):
            self.emit_all(write=True)
            found = False
            for rel, mod in self.mods.items():
                if not mod.moving or self.changed.get(rel) is None:
                    continue
                text = self.changed[rel]
                warned = collections.defaultdict(set)
                for line, what, name in self.unused(rel, None, lines=True):
                    off = old_offset(mod.kept, line_offset(text, line))
                    for k in mod.order:
                        ent = mod.ents[k]
                        if ent.b <= off < ent.e and name.split('::')[-1] in ent.names and self.prunable(ent) \
                                and '' in mod.placement['places'].get(k, ()):
                            warned[k].add(name.split('::')[-1])
                for k, names in warned.items():
                    if mod.ents[k].names <= names and k not in mod.excluded[''] and k not in base[rel]:
                        mod.excluded[''].add(k)
                        found = True
                        self.report.append(('pruned', rel, '/'.join(sorted(names)), 'unused in the module after the move'))
            for t, tgt in self.targets.items():
                text = self.outputs[t]
                warned = collections.defaultdict(set)
                for line, what, name in self.unused(t, None, lines=True):
                    off = line_offset(text, line)
                    for a, b, pairs in tgt.get('spans', []):
                        if not a <= off < b:
                            continue
                        for rel, k in pairs:
                            ent = self.mods[rel].ents[k]
                            if name.split('::')[-1] in ent.names and self.prunable(ent):
                                warned[(rel, k)].add(name.split('::')[-1])
                for (rel, k), names in warned.items():
                    mod = self.mods[rel]
                    if mod.ents[k].names <= names and k not in mod.excluded[t]:
                        mod.excluded[t].add(k)
                        found = True
                        self.report.append(('pruned', t, '/'.join(sorted(names)), 'copied from %s but unused there' % rel))
            if not found:
                break

    @staticmethod
    def prunable(ent):
        if ent.external or ent.registered or getattr(ent, 'macro', False) or ent.always:
            return False
        return ent.kind in FN_KINDS or (ent.record is None and ent.kind in ('VarDecl', 'group'))

    def unused(self, rel, _, lines=False):
        p = os.path.join(HOOK, rel)
        if not os.path.exists(p):
            return set()
        _, err = self.clang.warnings(rel)
        res = set()
        for ln in err.split('\n'):
            m = UNUSED_RE.match(ln)
            if m and os.path.normpath(m.group('file')) == os.path.normpath(rel):
                if lines:
                    res.add((int(m.group('line')), m.group('what'), m.group('name')))
                else:
                    res.add((m.group('what'), m.group('name')))
        return res

    # ------------------------------------------------------------------ report
    def write_report(self):
        lines = ['# split_modules.py report', '']
        by = collections.defaultdict(list)
        for kind, where, what, why in self.report:
            by[kind].append((where, what, why))
        for kind in ('moved', 'blocked', 'pruned', 'module'):
            items = by.get(kind, [])
            lines.append('## %s (%d)' % (kind, len(items)))
            lines.append('')
            for where, what, why in items:
                lines.append('- %s: %s - %s' % (where, what, why))
            lines.append('')
        lines.append('## files')
        lines.append('')
        for t in self.outputs:
            lines.append('- created/extended src/%s' % t)
        for rel, text in self.changed.items():
            lines.append('- %s src/%s' % ('deleted' if text is None else 'changed', rel))
        out = '\n'.join(lines) + '\n'
        if self.args.report:
            open(self.args.report, 'w').write(out)
        n = collections.Counter(k for k, *_ in self.report)
        print('moved %d, blocked %d, pruned %d; %d target files, %d modules changed (%d deleted)' % (
            n['moved'], n['blocked'], n['pruned'], len(self.outputs), len(self.changed),
            sum(1 for v in self.changed.values() if v is None)))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--group', required=True, help='regex on original_file (re/attribution.csv)')
    ap.add_argument('--confidence', default='anchor,range')
    ap.add_argument('--module', action='append', help='hook-relative module path(s) to restrict to')
    ap.add_argument('--shim', action='append', default=[], help='include dir with MSVC / Windows header shims')
    ap.add_argument('--dry-run', action='store_true')
    ap.add_argument('--no-prune', action='store_true')
    ap.add_argument('--segments', help='directory of segment CSVs (re/segments): split _unplaced stretches into the '
                    'proposed objects; the target of a function is its segment\'s file')
    ap.add_argument('--unplaced', action='store_true',
                    help='rows attributed only by a link-order stretch go to game/_unplaced/after_<left>.cpp')
    ap.add_argument('--report')
    ap.add_argument('-v', '--verbose', action='store_true')
    a = ap.parse_args()
    a.shim = [os.path.abspath(s) for s in a.shim] or shlex.split(os.environ.get('JOAN_SHIM_DIRS', ''))
    Split(a).run()


if __name__ == '__main__':
    main()
