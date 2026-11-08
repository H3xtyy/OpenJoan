#!/usr/bin/env python3
"""Dead imports of joan.exe (cleanup C2, docs/X64.md CLEANUP).

For every import of joan.exe's import table (src/runtime/import_table.inc) decide statically whether anything in src/ can
reach its IAT slot. Ways in (any one makes the import LIVE):
  - a C++ / asm use of its IAT slot symbol d::iat_<DLL>_<name> (src/data_names.hpp), outside src/data/ and the
    generated name / table files;
  - its raw slot address (0x00a6d830, 0xa6d830u, ...) in code, e.g. the `jmp [slot]` import stubs of
    src/lib/imports/import_stubs.cpp (S(name, stub, slot)) or `[0x00a6d994]` operands;
  - a `joan_image + 0x<offset>` memory operand whose offset is the slot's (offset = slot - 0x009cf000);
  - a use of the code name fn::<DLL>_<name> of the import's `jmp [slot]` stub in joan.exe (src/fn_names.hpp marks
    them "library  <DLL> <name>"): calls through that code slot reach the IAT when the stub is registered;
  - a reloc of the data image (src/data/data_relocs.inc) whose value is the slot.
Comments are stripped before searching (C/C++ // and /* */, asm ;). Not a use: src/runtime/replay/imports.cpp (the record /
replay wrappers only wrap a slot; they never call it on their own). A token paste that could build an iat_ name
(`iat_ ##`) aborts the run: the analysis could not see those uses.

Writes src/runtime/loader/dead_imports.inc (one X-macro line per dead import, in import table order) and prints a summary per
DLL - imports, overridden (every JOAN_IMPORT_OVERRIDE* registration, also through local wrapper macros such as
`#define F(name, fn) JOAN_IMPORT_OVERRIDE("KERNEL32.dll", #name, ...)`), dead, still resolved from the DLL (live and
not overridden) - also written to re/a4/c2_imports.txt. Deterministic and re-runnable:
    python3 re/gen_dead_imports.py            (from the repository root)
    python3 re/gen_dead_imports.py --check    (exit 1 when src/runtime/loader/dead_imports.inc is out of date)
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'src')
OUT_INC = os.path.join(SRC, 'runtime', 'loader', 'dead_imports.inc')
OUT_TXT = os.path.join(ROOT, 're', 'a4', 'c2_imports.txt')
IMAGE_VA = 0x009cf000

# Files that are not uses: generated tables that list every import / name, the data image, and the replay wrappers.
EXCLUDE_FILES = {
    'src/data_names.hpp', 'src/runtime/import_table.inc', 'src/fn_names.hpp', 'src/runtime/code_slots.inc',
    'src/runtime/loader/dead_imports.inc', 'src/runtime/replay/imports.cpp',
}
EXCLUDE_DIRS = ('src/data/',)
CODE_EXT = ('.cpp', '.hpp', '.h', '.c', '.cc', '.inc', '.ipp', '.asm', '.s', '.S')


def rel(path):
    return os.path.relpath(path, ROOT).replace(os.sep, '/')


def read(path):
    with open(path, encoding='utf-8', errors='replace') as f:
        return f.read()


# ---------------------------------------------------------------- C/C++ lexing helpers

def strip_comments(text, asm=False):
    """Comments -> spaces (newlines kept); string / char literals kept as they are."""
    out = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if asm:
            if c == ';':
                j = text.find('\n', i)
                j = n if j < 0 else j
                i = j
                continue
            out.append(c)
            i += 1
            continue
        if c == '/' and i + 1 < n and text[i + 1] == '/':
            j = i
            while True:  # a // comment continues over a backslash-newline
                k = text.find('\n', j)
                if k < 0:
                    j = n
                    break
                if text[k - 1] == '\\':
                    j = k + 1
                    continue
                j = k
                break
            i = j
            continue
        if c == '/' and i + 1 < n and text[i + 1] == '*':
            j = text.find('*/', i + 2)
            j = n if j < 0 else j + 2
            out.append(''.join('\n' if ch == '\n' else ' ' for ch in text[i:j]))
            i = j
            continue
        if c == 'R' and i + 1 < n and text[i + 1] == '"' and (i == 0 or not (text[i - 1].isalnum() or text[i - 1] == '_')):
            m = re.match(r'R"([^(\s]*)\(', text[i:])
            if m:
                end = text.find(')' + m.group(1) + '"', i)
                end = n if end < 0 else end + len(m.group(1)) + 2
                out.append(text[i:end])
                i = end
                continue
        if c == '"' or c == "'":
            if c == "'" and i > 0 and (text[i - 1].isalnum()):  # digit separator 1'000
                out.append(c)
                i += 1
                continue
            j = i + 1
            while j < n and text[j] != c and text[j] != '\n':
                j += 2 if text[j] == '\\' else 1
            out.append(text[i:j + 1])
            i = j + 1
            continue
        out.append(c)
        i += 1
    return ''.join(out)


def match_paren(text, i):
    """text[i] == '(' -> index after the matching ')', and the top-level comma separated arguments."""
    assert text[i] == '('
    depth, j, n = 0, i, len(text)
    args, start = [], i + 1
    while j < n:
        c = text[j]
        if c == '"' or c == "'":
            k = j + 1
            while k < n and text[k] != c:
                k += 2 if text[k] == '\\' else 1
            j = k + 1
            continue
        if c in '([{':
            depth += 1
        elif c in ')]}':
            depth -= 1
            if depth == 0:
                args.append(text[start:j].strip())
                return j + 1, args
        elif c == ',' and depth == 1:
            args.append(text[start:j].strip())
            start = j + 1
        j += 1
    raise ValueError('unbalanced parentheses')


def string_value(expr):
    """Concatenated string literals -> their value; None when expr is not only string literals."""
    parts = re.findall(r'"((?:[^"\\]|\\.)*)"', expr)
    if not parts or re.sub(r'"((?:[^"\\]|\\.)*)"', '', expr).strip():
        return None
    return ''.join(re.sub(r'\\(.)', r'\1', p) for p in parts)


# ---------------------------------------------------------------- inputs

def load_imports():
    text = read(os.path.join(SRC, 'runtime', 'import_table.inc'))
    body = text[text.index('kImportDlls[]'):]
    dlls = re.findall(r'^\s*"([^"]+)",\s*$', body[:body.index('};')], re.M)
    imports = []
    for m in re.finditer(r'\{0x([0-9a-fA-F]+)u, (\d+), (\d+), (nullptr|"((?:[^"\\]|\\.)*)")\}', text):
        name = None if m.group(4) == 'nullptr' else re.sub(r'\\(.)', r'\1', m.group(5))
        imports.append({'slot': int(m.group(1), 16), 'dll': dlls[int(m.group(2))], 'ordinal': int(m.group(3)),
                        'name': name})
    if not imports:
        sys.exit('gen_dead_imports: no imports in src/runtime/import_table.inc')
    return dlls, imports


def load_iat_symbols():
    syms = {}
    for m in re.finditer(r'Sym<[^>]*>\s+(iat_\w+)\{0x([0-9a-fA-F]+)u\}', read(os.path.join(SRC, 'data_names.hpp'))):
        syms[int(m.group(2), 16)] = m.group(1)
    return syms


def load_stub_names():
    """fn:: names of joan.exe's `jmp [IAT]` import stubs: (DLL base, import name) -> [fn name]."""
    stubs = {}
    for m in re.finditer(r'inline constexpr Code (\w+)\{\d+\};\s*//\s*0x[0-9a-fA-F]+ library\s+(\S+) (\S+)\s*$',
                         read(os.path.join(SRC, 'fn_names.hpp')), re.M):
        if m.group(1) == '%s_%s' % (m.group(2), m.group(3)):
            stubs.setdefault((m.group(2).lower(), m.group(3)), []).append(m.group(1))
    return stubs


def load_reloc_targets():
    targets = set()
    for root, _, files in os.walk(os.path.join(SRC, 'data')):
        for f in sorted(files):
            if not f.endswith('.inc'):
                continue
            for m in re.finditer(r'JOAN_DATA_RELOC\(0x[0-9a-fA-F]+u, (\w+), 0x([0-9a-fA-F]+)u\)',
                                 read(os.path.join(root, f))):
                targets.add(int(m.group(2), 16))
    return targets


def source_files():
    files = []
    for root, dirs, names in os.walk(SRC):
        dirs.sort()
        for f in sorted(names):
            p = os.path.join(root, f)
            r = rel(p)
            if r in EXCLUDE_FILES or r.startswith(EXCLUDE_DIRS) or not f.endswith(CODE_EXT):
                continue
            files.append(p)
    return files


# ---------------------------------------------------------------- import override registrations

OVERRIDE_RE = re.compile(r'\bJOAN_IMPORT_OVERRIDE(_ORDINAL_IF|_ORDINAL|_IF)?\s*\(')


def expand_macro(params, body, args):
    if len(args) != len(params) and not (params == [] and args == ['']):
        raise ValueError('argument count')
    sub = dict(zip(params, args))

    def stringify(m):
        return '"' + sub[m.group(1)].replace('\\', '\\\\').replace('"', '\\"') + '"'

    if params:
        body = re.sub(r'(?<!#)#\s*(%s)\b' % '|'.join(map(re.escape, params)), stringify, body)
        body = re.sub(r'\b(%s)\b' % '|'.join(map(re.escape, params)), lambda m: sub[m.group(1)], body)
    return re.sub(r'\s*##\s*', '', body)


def expand_text(text, macros):
    """Expand the file-local macros that lead to JOAN_IMPORT_OVERRIDE* (repeatedly, for nested wrappers)."""
    for _ in range(8):
        if not macros:
            return text
        pat = re.compile(r'\b(%s)\s*\(' % '|'.join(map(re.escape, sorted(macros))))
        out, pos, changed = [], 0, False
        for m in pat.finditer(text):
            if m.start() < pos:
                continue
            end, args = match_paren(text, m.end() - 1)
            params, body = macros[m.group(1)]
            out.append(text[pos:m.start()])
            out.append(expand_macro(params, body, args))
            pos, changed = end, True
        out.append(text[pos:])
        text = ''.join(out)
        if not changed:
            return text
    raise ValueError('macro expansion does not end')


def registrations_in(path):
    """Every JOAN_IMPORT_OVERRIDE* registration of one file: dicts dll, name / ordinal, conditional, label, where."""
    text = strip_comments(read(path))
    lines = text.replace('\\\n', ' \x00').split('\n')  # directive lines joined (\x00 keeps a line count)
    macros, chunk, found = {}, [], []

    def flush(line_no):
        t = expand_text('\n'.join(chunk), macros)
        for m in OVERRIDE_RE.finditer(t):
            _, args = match_paren(t, m.end() - 1)
            kind = m.group(1) or ''
            dll = string_value(args[0])
            if dll is None:
                raise ValueError('%s: import override with a computed DLL: %s' % (rel(path), args[0]))
            r = {'dll': dll, 'name': None, 'ordinal': 0, 'conditional': kind.endswith('_IF'),
                 'label': string_value(args[3]) if len(args) > 3 else None, 'where': rel(path)}
            if kind.startswith('_ORDINAL'):
                r['ordinal'] = int(args[1], 0)
            else:
                r['name'] = string_value(args[1])
                if r['name'] is None:
                    raise ValueError('%s: import override with a computed name: %s' % (rel(path), args[1]))
            found.append(r)
        chunk.clear()

    for no, line in enumerate(lines):
        s = line.strip()
        if s.startswith('#'):
            flush(no)
            d = re.match(r'#\s*define\s+(\w+)\(([^)]*)\)\s*(.*)$', s.replace('\x00', ''))
            if d:
                body = d.group(3)
                if OVERRIDE_RE.search(body) or any(re.search(r'\b%s\s*\(' % re.escape(k), body) for k in macros):
                    if d.group(1).startswith('JOAN_IMPORT_OVERRIDE'):
                        continue
                    macros[d.group(1)] = ([p.strip() for p in d.group(2).split(',') if p.strip()], body)
                continue
            u = re.match(r'#\s*undef\s+(\w+)', s)
            if u:
                macros.pop(u.group(1), None)
            continue
        chunk.append(line)
    flush(len(lines))
    return found


def load_overrides(files):
    regs = []
    for p in files:
        if rel(p) == 'src/runtime/loader/import_overrides.hpp':
            continue
        if 'JOAN_IMPORT_OVERRIDE' in read(p):
            regs.extend(registrations_in(p))
    return regs


# ---------------------------------------------------------------- liveness

def find_uses(files, imports, iat_syms, stub_names, reloc_targets):
    slots = {im['slot'] for im in imports}
    sym_to_slot = {s: a for a, s in iat_syms.items()}
    stub_to_slot = {}
    for im in imports:
        key = (im['dll'].rsplit('.', 1)[0].lower(), im['name'] or '')
        for fn in stub_names.get(key, []):
            stub_to_slot[fn] = im['slot']
    uses = {s: [] for s in slots}
    for p in files:
        r = rel(p)
        raw = read(p)
        text = strip_comments(raw, asm=p.lower().endswith(('.asm', '.s')))
        if re.search(r'\biat_\w*\s*##|##\s*iat_|"iat_', text):
            sys.exit('gen_dead_imports: %s builds iat_ names by token pasting / strings: cannot analyse it' % r)
        for m in re.finditer(r'\b(iat_\w+)\b', text):
            if m.group(1) in sym_to_slot:
                uses[sym_to_slot[m.group(1)]].append('%s: %s' % (r, m.group(1)))
        for m in re.finditer(r'\b0[xX]([0-9a-fA-F]+)[uUlL]*\b', text):
            v = int(m.group(1), 16)
            if v in slots:
                uses[v].append('%s: raw slot 0x%08x' % (r, v))
        for m in re.finditer(r'\bjoan_image\s*\+\s*(0[xX][0-9a-fA-F]+|\d+)\b(?!\s*[-+*])', text):
            v = IMAGE_VA + int(m.group(1), 0)
            if v in slots:
                uses[v].append('%s: joan_image + 0x%x' % (r, v - IMAGE_VA))
        for m in re.finditer(r'\b(\w+_\w+)\b', text):
            if m.group(1) in stub_to_slot:
                uses[stub_to_slot[m.group(1)]].append('%s: import stub fn::%s' % (r, m.group(1)))
    for s in slots & reloc_targets:
        uses[s].append('src/data/data_relocs.inc: data reloc -> 0x%08x' % s)
    return uses


def label(im):
    return im['name'] if im['name'] is not None else '#%u' % im['ordinal']


def main():
    check = '--check' in sys.argv[1:]
    dlls, imports = load_imports()
    iat_syms = load_iat_symbols()
    files = source_files()
    uses = find_uses(files, imports, iat_syms, load_stub_names(), load_reloc_targets())
    regs = load_overrides(files)

    def overrides_of(im):
        return [o for o in regs if o['dll'].lower() == im['dll'].lower() and
                (o['name'] == im['name'] if o['name'] is not None else im['name'] is None and o['ordinal'] == im['ordinal'])]

    lines = [
        '// Generated by re/gen_dead_imports.py from src/** - do not edit. Regenerate (repository root):',
        '//     python3 re/gen_dead_imports.py',
        '// after any change to the uses of joan.exe\'s IAT slots (d::iat_* symbols, raw slot addresses, `jmp [slot]`',
        '// import stubs, joan_image + offset operands). Imports of joan.exe (src/runtime/import_table.inc) whose IAT slot nothing',
        '// in src/ can reach; the loader (src/runtime/loader/joan_re.cpp, resolve_imports) gives each one without an active',
        '// import override a trap that reports the call and ends the game instead of the DLL\'s export.',
        '// X-macro: define JOAN_DEAD_IMPORT(dll, name, ordinal) before including; name is nullptr for an import by',
        '// ordinal. In import table order.',
        '#ifndef JOAN_DEAD_IMPORT',
        '#error define JOAN_DEAD_IMPORT(dll, name, ordinal) before including dead_imports.inc',
        '#endif',
    ]
    report = []
    totals = [0, 0, 0, 0]
    for dll in dlls:
        mine = [im for im in imports if im['dll'] == dll]
        n_over = n_cond = n_dead = n_dead_over = 0
        from_dll, from_dll_cond, dead_names = [], [], []
        for im in mine:
            ovs = overrides_of(im)
            hard = any(not o['conditional'] for o in ovs)
            dead = not uses[im['slot']]
            if ovs:
                n_over += 1
                if not hard:
                    n_cond += 1
            if dead:
                n_dead += 1
                dead_names.append(label(im))
                if ovs:
                    n_dead_over += 1
                sym = iat_syms.get(im['slot'])
                name = 'nullptr' if im['name'] is None else '"%s"' % im['name'].replace('?', '\\?')
                lines.append('JOAN_DEAD_IMPORT("%s", %s, %u)  // 0x%08x %s%s' % (
                    dll, name, im['ordinal'], im['slot'], sym or '(no iat_ symbol)',
                    '; overridden: ' + ', '.join(sorted({o['where'] for o in ovs})) if ovs else ''))
            elif not ovs:
                from_dll.append(label(im))
            elif not hard:
                from_dll_cond.append(label(im))
        totals[0] += len(mine)
        totals[1] += n_over
        totals[2] += n_dead
        totals[3] += len(from_dll)
        report.append('%s: %u imports, %u overridden%s, %u dead%s, %u from the DLL%s' % (
            dll, len(mine), n_over, ' (%u conditional: _IF)' % n_cond if n_cond else '', n_dead,
            ' (%u of them overridden)' % n_dead_over if n_dead_over else '', len(from_dll),
            ': ' + ', '.join(from_dll) if from_dll else ' -> not loaded' if not from_dll_cond else ''))
        if from_dll_cond:
            report.append('    live, overridden only by an _IF override (from the DLL when it is off): ' +
                          ', '.join(from_dll_cond))
        if dead_names:
            report.append('    dead: ' + ', '.join(dead_names))
    for o in regs:
        if not any(o in overrides_of(im) for im in imports):
            report.append('warning: override %s!%s (%s) matches no import' % (
                o['dll'], o['name'] if o['name'] is not None else '#%u' % o['ordinal'], o['where']))
    dead_keys = {(im['dll'].lower(), im['name']) for im in imports if not uses[im['slot']]}
    for m in re.finditer(r'\bH\("([^"]+)", (\w+)\)', strip_comments(read(os.path.join(SRC, 'runtime', 'replay', 'imports.cpp')))):
        if (m.group(1).lower(), m.group(2)) in dead_keys:
            report.append('note: src/runtime/replay/imports.cpp still wraps the dead import %s!%s (its wrapper can go)' % (
                m.group(1), m.group(2)))
    report.append('total: %u imports, %u overridden, %u dead, %u from a DLL; %u override registrations in %u files' % (
        totals[0], totals[1], totals[2], totals[3], len(regs), len({o['where'] for o in regs})))
    inc = '\n'.join(lines) + '\n'
    txt = ('# Generated by re/gen_dead_imports.py - per DLL of joan.exe\'s import table: imports, overridden '
           '(JOAN_IMPORT_OVERRIDE*), dead\n# (no reachable use of the IAT slot in src/), still resolved from the DLL '
           '(live and not overridden).\n' + '\n'.join(report) + '\n')
    if check:
        ok = os.path.exists(OUT_INC) and read(OUT_INC) == inc
        print('src/runtime/loader/dead_imports.inc is %s' % ('up to date' if ok else 'OUT OF DATE'))
        return 0 if ok else 1
    for path, content in ((OUT_INC, inc), (OUT_TXT, txt)):
        with open(path, 'w', encoding='utf-8', newline='\n') as f:
            f.write(content)
    print('\n'.join(report))
    if '-v' in sys.argv[1:]:
        for im in imports:
            print('%s!%s: %s' % (im['dll'], label(im), '; '.join(uses[im['slot']]) or 'DEAD'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
