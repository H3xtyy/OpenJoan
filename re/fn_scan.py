"""Shared scanner for re/gen_fn_names.py, re/rewrite_fn_names.py, re/gen_code_slots.py, re/gen_data_names.py and
re/check_s7.py: finds the numeric code addresses of src/** C++ sources (comments, string literals and registration
keys excluded) and classifies their use (call target, value, inside __asm). macro_keys / key_arg_mask tell which
macro arguments are registration keys or tokens (stringized, pasted, __asm) and must stay literals.

Inputs: joan.exe's thunk table (thunks.json: thunk hex -> real address hex) and src/** (registry names).
"""
import glob
import json
import os
import re

THUNK_FIRST, THUNK_COUNT = 0x00401005, 8015
THUNK_END = THUNK_FIRST + 5 * THUNK_COUNT
CODE_RANGES = [(0x00414000, 0x007d5000), (0x007dd410, 0x007e50a0)]
GAME_HI = 0x00763d00
# Files whose code addresses are lookup keys / range bounds (see re/gen_code_slots.py KEY_FILES): only direct calls there
# are rewritten.
KEY_FILES = {'canvas_hooks.cpp', 'display.cpp', 'fixes.cpp'}
# Generated / data files that are never rewritten or scanned for references.
SKIP_FILES = {'code_map.inc', 'code_slots.inc', 'census_table.inc', 'data_image.inc', 'import_table.inc',
              'fn_names.hpp', 'data_names.hpp'}
# src/runtime/image.hpp defines the image range itself (kVa / kEnd), src/s7_probe.cpp is a build probe: not uses.
SKIP_PATHS = {'runtime/image.hpp', 's7_probe.cpp'}
# Files another stage owns (data image) - not touched by the function-name rewrite.
SKIP_REL = re.compile(r'^(runtime/loader/data_image\.[ch]pp|data/)')

HEX_RE = re.compile(r'\b0x([0-9a-fA-F]{6,8})([uUlL]{0,3})\b')
# Registration macro: NAME("a", "b", 0x...) / NAME("b", 0x...) / NAME("a", "b", Class::x, 0x...) (same as
# gen_code_slots.py, but allowed to span lines).
REG_RE = re.compile(r'\b([A-Za-z_]\w*)\s*\(\s*(?:"((?:\\.|[^"\\])*)"\s*,\s*)?"((?:\\.|[^"\\])*)"\s*,\s*'
                    r'(?:[A-Za-z_][\w:]*\s*,\s*)?0x([0-9a-fA-F]{6,8})(?:[uUlL]{0,3})\b')
MOD_RE = re.compile(r'JOAN_REPLACE\w*\s*\(\s*"([^"]+)"')

# Text right before a literal (comments / strings blanked) that makes it the target of a direct call.
CALL_CTX = [
    ('helper', re.compile(r'\b[A-Za-z_]\w*call\w*\s*(?:<[^;()]*(?:\([^;()]*\)[^;()]*)*>)?\s*\(\s*$')),
    ('original', re.compile(r'\boriginal\s*<[^;]*>\s*\(\s*$')),
    ('orig_macro', re.compile(r'\b(?:JOAN_)?ORIG\w*\s*\(\s*$')),
    ('fnptr_cast', re.compile(r'\breinterpret_cast\s*<[^;<>]*\(\s*(?:__\w+\s*)?\*\s*\)\s*\([^;]*\)\s*>\s*\(\s*$')),
    ('fnptr_cast', re.compile(r'\breinterpret_cast\s*<\s*(?:Fn|F|Proc|Func|[A-Z]\w*Fn|\w+_fn|fn_t|\w*Fn\w*)\s*>\s*\(\s*$')),
    ('kc_const', re.compile(r'\bkc_\w+\s*=\s*$')),
    ('helper', re.compile(r'\bcall\s*(?:<[^;()]*>)?\s*\(\s*$')),
]
# `reinterpret_cast<Fn>(0x...)(args)`: a cast to a function pointer typedef that is called right away.
CAST_BEFORE = re.compile(r'\breinterpret_cast\s*<[^;]*>\s*\(\s*$')
CAST_AFTER = re.compile(r'^\s*\)\s*\(')
# `kX = 0x...` (constant / variable initialised with a code address): a call target when the name is used as one.
INIT_BEFORE = re.compile(r'\b([A-Za-z_]\w*)\s*=\s*$')
PP_IF = re.compile(r'^\s*#\s*(?:if|elif)\b')


def ident_called(code, ident):
    """ident is used as a call target in code (stripped text)"""
    i = re.escape(ident)
    return re.search(r'(?:\b[A-Za-z_]\w*call\w*\s*(?:<[^;()]*(?:\([^;()]*\)[^;()]*)*>)?\s*\(\s*' + i + r'\b'
                     r'|\boriginal\s*<[^;]*>\s*\(\s*' + i + r'\b'
                     r'|\breinterpret_cast\s*<[^;]*>\s*\(\s*' + i + r'\s*\)\s*\('
                     r'|\b(?:call|jmp)\s+(?:dword\s+ptr\s*)?(?:ds\s*:\s*)?\[?\s*' + i + r'\b)', code) is not None


def strip(src, strings=True):
    """src with comments (and string / char literals) replaced by spaces, same length and line structure"""
    out = []
    i, n = 0, len(src)
    while i < n:
        c = src[i]
        if c == '/' and src.startswith('//', i):
            j = src.find('\n', i)
            j = n if j < 0 else j
            out.append(' ' * (j - i))
            i = j
        elif c == '/' and src.startswith('/*', i):
            j = src.find('*/', i + 2)
            j = n if j < 0 else j + 2
            out.append(re.sub(r'[^\n]', ' ', src[i:j]))
            i = j
        elif c in '"\'':
            j = i + 1
            while j < n and src[j] != c and src[j] != '\n':
                j += 2 if src[j] == '\\' else 1
            j = min(j + 1, n)
            out.append(c + ' ' * (j - i - 2) + c if strings else src[i:j])
            i = j
        else:
            out.append(c)
            i += 1
    return ''.join(out)


def asm_mask(code):
    """bool per char: inside an __asm block / __asm line (code = stripped text)"""
    mask = bytearray(len(code))
    for m in re.finditer(r'\b_?_asm\b', code):
        i = m.end()
        while i < len(code) and code[i] in ' \t':
            i += 1
        if i < len(code) and code[i] == '{':
            depth, j = 0, i
            while j < len(code):
                if code[j] == '{':
                    depth += 1
                elif code[j] == '}':
                    depth -= 1
                    if depth == 0:
                        break
                j += 1
            lo, hi = m.start(), j + 1
        else:
            j = code.find('\n', i)
            lo, hi = m.start(), (len(code) if j < 0 else j)
        for k in range(lo, hi):
            mask[k] = 1
    return mask


# Registration macros of src/registry.hpp: argument index of the registration key (original address / IAT slot).
KEY_MACROS = {'JOAN_REPLACE': {2}, 'JOAN_REPLACE_YIELDING': {2}, 'JOAN_REPLACE_IMPORT': {2},
              # the opt-in names merged into JOAN_REPLACE in Phase 3 (older trees)
              'JOAN_REPLACE_OPT_IN': {2}, 'JOAN_REPLACE_OPT_IN_YIELDING': {2}, 'JOAN_REPLACE_IMPORT_OPT_IN': {2}}
DEFINE_RE = re.compile(r'^[ \t]*#[ \t]*define[ \t]+([A-Za-z_]\w*)\(([^)]*)\)((?:[^\n]*\\\n)*[^\n]*)', re.M)


def split_args(code, i):
    """code[i] == '(': ([(start, end)] of the top-level arguments, index of the closing ')') - brackets of all kinds
    nest; strings / comments must already be blanked"""
    depth, start, spans, j = 0, i + 1, [], i
    while j < len(code):
        c = code[j]
        if c in '([{':
            depth += 1
        elif c in ')]}':
            depth -= 1
            if depth == 0:
                spans.append((start, j))
                return spans, j
        elif c == ',' and depth == 1:
            spans.append((start, j))
            start = j + 1
        j += 1
    spans.append((start, len(code)))
    return spans, len(code)


def macro_keys(code):
    """{macro name: set of argument indexes} for the function-like macros #defined in code (stripped text) plus the
    registry macros: the arguments that are registration keys or are used as tokens (stringized `#p`, pasted `##`,
    inside __asm, or passed on as the key argument of another such macro). A literal there must stay a literal."""
    keys = {k: set(v) for k, v in KEY_MACROS.items()}
    defs = []
    for m in DEFINE_RE.finditer(code):
        params = [p.strip() for p in m.group(2).split(',')]
        defs.append((m.group(1), params, m.group(3).replace('\\\n', ' ')))
    changed = True
    while changed:
        changed = False
        for name, params, body in defs:
            ks = set(keys.get(name, ()))
            asm = re.search(r'\b_?_asm\b', body)
            for k, p in enumerate(params):
                if not re.match(r'^[A-Za-z_]\w*$', p):
                    continue
                q = re.escape(p)
                stringized = re.search(r'(?<!#)#\s*' + q + r'\b', body)
                pasted = re.search(r'##\s*' + q + r'\b|\b' + q + r'\s*##', body)
                in_asm = asm and re.search(r'\b' + q + r'\b', body[asm.start():])
                if stringized or pasted or in_asm:
                    ks.add(k)
            for im in re.finditer(r'\b([A-Za-z_]\w*)\s*\(', body):
                inner = keys.get(im.group(1))
                if not inner:
                    continue
                spans, _ = split_args(body, im.end() - 1)
                for k in inner:
                    if k < len(spans):
                        a = body[spans[k][0]:spans[k][1]].strip()
                        if a in params:
                            ks.add(params.index(a))
            if ks != keys.get(name, set()):
                keys[name] = ks
                changed = True
    return {k: v for k, v in keys.items() if v}


def key_arg_mask(code):
    """bool per char of code (stripped text): inside a key argument (macro_keys) of a macro invocation"""
    keys = macro_keys(code)
    mask = bytearray(len(code))
    defspans = [(m.start(), m.end()) for m in DEFINE_RE.finditer(code)]
    for m in re.finditer(r'\b([A-Za-z_]\w*)\s*\(', code):
        ks = keys.get(m.group(1))
        if not ks or any(lo <= m.start() < hi for lo, hi in defspans):
            continue
        spans, _ = split_args(code, m.end() - 1)
        for k in ks:
            if k < len(spans):
                lo, hi = spans[k]
                mask[lo:hi] = b'\1' * (hi - lo)
    return mask


def in_code(a):
    return any(lo <= a < hi for lo, hi in CODE_RANGES)


# Stage S7a-tail (rewrite_fn_names.py --values): a real code address used as a VALUE (callback, atexit / qsort
# argument, template argument, table entry, constant feeding an asm memory operand ...) is named when it is a code
# entry of src/runtime/code_slots.inc (function start or other place code arrives). Stays a literal (an ID, never an address
# that is called or stored): registration keys (kind 'reg'), the lookup / patch-site tables of KEY_FILES, the range
# bounds below and the files that hold them, and every constant that is not a code entry (instruction addresses of
# canvas size reads / immediates, mid-function return addresses: the code map's "patch sites").
ID_FILES = KEY_FILES | {'canvas_api.hpp', 'runtime/patch.cpp', 'runtime/patch.hpp', 'runtime/code_table.cpp',
                        'runtime/code_slots_core.hpp', 'runtime/loader/joan_re.cpp', 'runtime/crash.cpp',
                        'runtime/image_runtime.cpp'}
BOUNDS = {0x00401000, 0x009ce700, 0x00414000, 0x00763d00, 0x00765000, 0x007d5000, 0x007dd410, 0x007e50a0}
# src/runtime/code_slots.inc (re/gen_code_slots.py): {original address, thunk target, flags}, one line per code slot.
SLOT_LINE = re.compile(r'^\{0x([0-9a-f]{8})u, 0x([0-9a-f]{8})u, 0x([0-9a-f]{4})\},', re.M)
SLOT_THUNK = 0x80


def load_code_entries(hook_dir):
    """code entry addresses (function starts and every other place code may arrive: the real-address slots) of
    src/runtime/code_slots.inc (re/gen_code_slots.py)"""
    path = os.path.join(hook_dir, 'runtime', 'code_slots.inc')
    if not os.path.exists(path):  # before S7c-main: the S5 code map
        src = open(os.path.join(hook_dir, 'runtime', 'code_map.inc')).read()
        return {int(m.group(1), 16) for m in re.finditer(r'^\{0x([0-9a-f]{8}), \d+, 0x[0-9a-f]{2}\},$', src, re.M)}
    src = open(path).read()
    return {int(m.group(1), 16) for m in SLOT_LINE.finditer(src) if not int(m.group(3), 16) & SLOT_THUNK}


def value_is_code(rel, u, entries):
    """a non-call use of a real code address that names a function (S7a-tail rule above)"""
    return (u.kind == 'value' and not is_thunk(u.value) and in_code(u.value) and rel not in ID_FILES
            and u.value not in BOUNDS and u.value in entries)


def is_thunk(a):
    return THUNK_FIRST <= a < THUNK_END and (a - THUNK_FIRST) % 5 == 0


def hook_files(hook_dir):
    res = []
    for path in sorted(glob.glob(os.path.join(hook_dir, '**', '*'), recursive=True)):
        if not re.search(r'\.(cpp|hpp|h|inc|c)$', path):
            continue
        rel = os.path.relpath(path, hook_dir).replace(os.sep, '/')
        if os.path.basename(path) in SKIP_FILES or rel in SKIP_PATHS:
            continue
        res.append((rel, path))
    return res


def load_thunks(path):
    return {int(k, 16): int(v, 16) for k, v in json.load(open(path)).items()}


def registry_names(hook_dir):
    """real address -> registered name (first registration in path order wins, as in the w120 analysis)"""
    names = {}
    for rel, path in hook_files(hook_dir):
        src = strip(open(path, errors='replace').read(), strings=False)
        for m in REG_RE.finditer(src):
            a = int(m.group(4), 16)
            name = m.group(3)
            if m.group(1).startswith('JOAN_REPLACE_IMPORT'):
                continue
            if (0x401000 <= a < 0x7e50a0) and a not in names:
                names[a] = name
    return names


class Use:
    __slots__ = ('rel', 'start', 'end', 'value', 'kind', 'ctx', 'line', 'text')

    def __init__(self, rel, start, end, value, kind, ctx, line, text):
        self.rel, self.start, self.end, self.value, self.kind, self.ctx = rel, start, end, value, kind, ctx
        self.line, self.text = line, text


def scan_source(rel, src):
    """every hex literal in [0x401000, 0x7e50a0) of src: Use with kind
       'reg'   registration macro address argument
       'asm'   inside an __asm block
       'call'  direct call target (ctx = helper / original / orig_macro / fnptr_cast / kc_const)
       'value' anything else"""
    code = strip(src)
    nocomment = strip(src, strings=False)
    regspans = set()
    for m in REG_RE.finditer(nocomment):
        regspans.add(m.start(4) - 2)
    kmask = key_arg_mask(code)
    amask = asm_mask(code)
    uses = []
    line_starts = [0] + [m.end() for m in re.finditer('\n', src)]
    import bisect
    for m in HEX_RE.finditer(code):
        v = int(m.group(1), 16)
        if not (0x401000 <= v < 0x7e50a0):
            continue
        s = m.start()
        ln = bisect.bisect_right(line_starts, s)
        text = src[line_starts[ln - 1]:(line_starts[ln] - 1 if ln < len(line_starts) else len(src))]
        if s in regspans or kmask[s]:
            kind, ctx = 'reg', ''
        elif PP_IF.match(code[line_starts[ln - 1]:s]):
            kind, ctx = 'pp_if', ''
        elif amask[s]:
            kind, ctx = 'asm', ''
        else:
            before = code[max(0, s - 300):s]
            kind, ctx = 'value', ''
            for name, rx in CALL_CTX:
                if rx.search(before):
                    kind, ctx = 'call', name
                    break
            if kind == 'value' and CAST_BEFORE.search(before) and CAST_AFTER.match(code[m.end():m.end() + 20]):
                kind, ctx = 'call', 'fnptr_cast'
            if kind == 'value':
                im = INIT_BEFORE.search(before)
                if im and not im.group(1).startswith('kc_') and ident_called(code, im.group(1)):
                    kind, ctx = 'call', 'named_const'
        uses.append(Use(rel, s, m.end(), v, kind, ctx, ln, text))
    return uses


def scan_hook(hook_dir):
    res = {}
    for rel, path in hook_files(hook_dir):
        if SKIP_REL.match(rel):
            continue
        src = open(path, errors='replace', newline='').read()
        res[rel] = (path, src, scan_source(rel, src))
    return res


# `inline constexpr std::uintptr_t Name = 0x...;` (fn_names.hpp) or `inline constexpr img::Sym<T> Name{{0x...u}};`
# (data_names.hpp since S7b-prep)
NAME_LINE = re.compile(r'^\s*inline constexpr (?:std::uintptr_t|Code|img::Sym<.*?>|img::Addr) (\w+)\s*(?:=\s*|\{\{?)'
                       r'0x([0-9a-fA-F]{8})u?\b')


# Stage S7c-main: an fn:: name is a slot index of the code table, its original address is the first comment field:
#   inline constexpr Code Video_seek{0};  // 0x00401005 -> 0x00686250  Video::seek
CODE_LINE = re.compile(r'^\s*inline constexpr Code (\w+)\{(\d+)\};\s*//\s*0x([0-9a-fA-F]{8})\b')


def load_fn_names(path):
    """name -> value (original address) from an existing src/fn_names.hpp ({} if missing); also reads the
    data_names.hpp form (load_data_names) and the pre-S7c-main fn_names.hpp form (`Code Name{0x...}`)"""
    res = {}
    if path and os.path.exists(path):
        for ln in open(path):
            m = CODE_LINE.match(ln)
            if m:
                res[m.group(1)] = int(m.group(3), 16)
                continue
            m = NAME_LINE.match(ln)
            if m:
                res[m.group(1)] = int(m.group(2), 16)
    return res


def load_fn_slots(path):
    """name -> (slot index, original address) of src/fn_names.hpp in the S7c-main form ({} if missing / older)"""
    res = {}
    if path and os.path.exists(path):
        for ln in open(path):
            m = CODE_LINE.match(ln)
            if m:
                res[m.group(1)] = (int(m.group(2)), int(m.group(3), 16))
    return res


FN_USE_RE = re.compile(r'\b(?:joan::)?fn::([A-Za-z_]\w*)\b')


# S7c-prep spellings (src/runtime/code.hpp) read as the forms they replaced, so every scanner classifies a site exactly as
# before: fn::code_value(X) / joan::fn::va(X) -> X, fn::code_ptr(X) -> obj(X), JOAN_LATE_CODE(name, X) ->
# std::uint32_t name = X.
CODE_WRAP_RE = re.compile(r'(?:\bjoan::)?\bfn::(code_value|code_ptr|va)\s*\(\s*((?:joan::)?(?:fn::)?[A-Za-z_]\w*)\s*\)')
LATE_CODE_RE = re.compile(r'\bJOAN_LATE_CODE\s*\(\s*([A-Za-z_]\w*)\s*,\s*((?:joan::)?(?:fn::)?[A-Za-z_]\w*)\s*\)')


def unwrap_code(src):
    """src with the S7c-prep code wrappers removed (see CODE_WRAP_RE)"""
    src = LATE_CODE_RE.sub(lambda m: 'std::uint32_t %s = %s' % (m.group(1), m.group(2)), src)
    return CODE_WRAP_RE.sub(lambda m: ('obj(%s)' if m.group(1) == 'code_ptr' else '%s') % m.group(2), src)


def expand_fn_names(src, names):
    """src with every fn::Name / joan::fn::Name replaced by its 0x%08x value (for scanners that look for literals);
    the S7c-prep wrappers are read as the forms they replaced (unwrap_code)"""
    src = unwrap_code(src)

    def sub(m):
        v = names.get(m.group(1))
        return m.group(0) if v is None else '0x%08x' % v
    return FN_USE_RE.sub(sub, src)


# src/data_names.hpp (re/gen_data_names.py): names of the data addresses (.rdata / .data / .bss / IAT slots), typed
# img::Sym<T> constants (S7b-prep). `d::Name` / `joan::d::Name` stands for the literal; `d::Name + 0x10` is therefore
# the expression `0x... + 0x10`, exactly as if the literal had been written (no folding).
load_data_names = load_fn_names
DATA_USE_RE = re.compile(r'\b(?:joan::)?d::([A-Za-z_]\w*)\b')


# Inline-asm operands of the data image object since S7b-main (re/asm_rebase.py): `joan_image + 0x9818` is the original
# address 0x009d8818 (img::kVa + offset).
JOAN_IMAGE_RE = re.compile(r'\bjoan_image\s*\+\s*0x([0-9a-fA-F]+)\b')
JOAN_IMAGE_VA = 0x009cf000


def expand_data_names(src, names):
    """src with every d::Name / joan::d::Name replaced by its 0x%08x value, and every asm operand
       `joan_image + 0xOFF` by the original address it stands for"""
    def sub(m):
        v = names.get(m.group(1))
        return m.group(0) if v is None else '0x%08x' % v
    src = JOAN_IMAGE_RE.sub(lambda m: '0x%08x' % (JOAN_IMAGE_VA + int(m.group(1), 16)), src)
    return DATA_USE_RE.sub(sub, src)


def expand_names(src, fn_names, data_names):
    """src with the names of src/fn_names.hpp and src/data_names.hpp expanded to their literals"""
    return expand_data_names(expand_fn_names(src, fn_names), data_names)
