#!/usr/bin/env python3
"""Stage S7b-prep codemod (docs/RUNTIME.md): src/** C++ uses the typed data addresses of src/runtime/image.hpp.
Behaviour-preserving: in S7b-prep img::base() is the identity, every helper folds to the original constant.

Usage (from the repository root):
  rewrite_image_types.py <hook dir> [--dry-run] [--report out.tsv]

Steps (each idempotent):
1. helpers: the module-local copies of the data helpers are deleted in favour of image.hpp, one line for one line
   (files that use __LINE__ keep their line numbers):
     void* obj(std::uintptr_t a) { return reinterpret_cast<void*>(a); }        -> using joan::img::obj;
     const char* str(std::uintptr_t a) { return reinterpret_cast<const char*>(a); } -> using joan::img::str;
     template <class T> T& global(std::uintptr_t a) { return *reinterpret_cast<T*>(a); } (2-4 lines)
                                                                                -> using joan::img::global;
   Only the exact forms (whitespace / parameter name / std::uintptr_t, uptr, u32 spellings aside) are replaced; any
   other definition of obj / str / global (or a helper that only looks alike) is listed as a variant and kept.
2. casts: reinterpret_cast<T*>(d::X ...) -> &img::global<T>(d::X ...), reinterpret_cast<void*>(d::X ...) ->
   img::obj(d::X ...) (const void*: the same; the pointer converts). The qualified img::global keeps the data
   analysis (re/data_classify.py) reading the site as an opaque pointer, exactly like the cast.
Everything else that S7b-prep changes is type-driven (the compiler lists it; see docs/RUNTIME.md).
"""
import collections
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fn_scan as F  # noqa: E402

INT_T = r'(?:std::uintptr_t|uintptr_t|uptr|u32|std::uint32_t)'
OBJ_DEF = re.compile(r'^([ \t]*)(?:static\s+|inline\s+)?void\s*\*\s*obj\s*\(\s*' + INT_T + r'\s+(\w+)\s*\)\s*'
                     r'(?:noexcept\s*)?\{\s*return\s+reinterpret_cast\s*<\s*void\s*\*\s*>\s*\(\s*'
                     r'(?:static_cast\s*<\s*std::uintptr_t\s*>\s*\(\s*\2\s*\)|\2)\s*\)\s*;\s*\}[ \t]*$', re.M)
STR_DEF = re.compile(r'^([ \t]*)(?:static\s+|inline\s+)?const\s+char\s*\*\s*str\s*\(\s*' + INT_T + r'\s+(\w+)\s*\)\s*'
                     r'(?:noexcept\s*)?\{\s*return\s+reinterpret_cast\s*<\s*const\s+char\s*\*\s*>\s*\(\s*'
                     r'(?:static_cast\s*<\s*std::uintptr_t\s*>\s*\(\s*\2\s*\)|\2)\s*\)\s*;\s*\}[ \t]*$', re.M)
GLOBAL_DEF = re.compile(r'^([ \t]*)template\s*<\s*(?:class|typename)\s+(\w+)\s*>\s*(?:inline\s+)?\2\s*&\s*global'
                        r'\s*\(\s*' + INT_T + r'\s+(\w+)\s*\)\s*(?:noexcept\s*)?\{\s*return\s+\*\s*'
                        r'reinterpret_cast\s*<\s*\2\s*\*\s*>\s*\(\s*\3\s*\)\s*;\s*\}[ \t]*$', re.M)
# any other definition of one of the names (a variant to review)
ANY_DEF = re.compile(r'^[ \t]*(?:template\s*<[^>]*>\s*)?(?:static\s+|inline\s+|constexpr\s+)*[\w:<>]+(?:\s*[*&]+)?\s*'
                     r'[*&]?\s*\b(obj|str|global)\s*\([^;{)]*\)\s*(?:const\s*)?(?:noexcept\s*)?\{', re.M)
NAME = r'(?:joan::)?d::[A-Za-z_]\w*'


def keep_lines(old, new):
    """new padded with empty lines to the line count of old"""
    return new + '\n' * (old.count('\n') - new.count('\n'))


def step_helpers(src, stats, variants, rel):
    out = src
    for rx, name in ((OBJ_DEF, 'obj'), (STR_DEF, 'str'), (GLOBAL_DEF, 'global')):
        def sub(m, name=name):
            stats['helper_' + name] += 1
            return keep_lines(m.group(0), '%susing joan::img::%s;' % (m.group(1), name))
        out = rx.sub(sub, out)
    code = F.strip(out)
    for m in ANY_DEF.finditer(code):
        line = out[m.start():out.find('\n', m.start())].strip()
        if 'using joan::img::' in line:
            continue
        variants.append((rel, out.count('\n', 0, m.start()) + 1, m.group(1), line))
    return out


def step_casts(src, stats):
    code = F.strip(src)
    reps = []
    for m in re.finditer(r'\breinterpret_cast\s*<', code):
        # template argument (one nesting level of <>), then '('
        i, depth = m.end(), 1
        while i < len(code) and depth:
            depth += {'<': 1, '>': -1}.get(code[i], 0)
            i += 1
        targ = code[m.end():i - 1].strip()
        j = i
        while j < len(code) and code[j] in ' \t\n':
            j += 1
        if j >= len(code) or code[j] != '(':
            continue
        spans, close = F.split_args(code, j)
        if len(spans) != 1:
            continue
        arg = src[spans[0][0]:spans[0][1]]
        if not re.match(r'^\s*' + NAME + r'\b', arg):
            continue
        tm = re.match(r'^(.*?)\s*\*$', targ)
        if not tm or '(' in targ:
            continue                       # not a plain object pointer type: left to the compiler
        t = tm.group(1).strip()
        if re.match(r'^(?:const\s+)?(?:volatile\s+)?void$', t):
            new = 'img::obj(%s)' % arg.strip()
            stats['cast_obj'] += 1
        else:
            new = '&img::global<%s>(%s)' % (t, arg.strip())
            stats['cast_global'] += 1
        reps.append((m.start(), close + 1, new))
    out = src
    for s, e, new in sorted(reps, reverse=True):
        out = out[:s] + new + out[e:]
    return out


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    hook_dir = args[0]
    dry = '--dry-run' in sys.argv
    report = sys.argv[sys.argv.index('--report') + 1] if '--report' in sys.argv else None
    stats = collections.Counter()
    variants = []
    changed = 0
    for rel, path in F.hook_files(hook_dir):
        if rel.startswith('data/') or F.SKIP_REL.match(rel) or rel in ('runtime/patch.hpp', 'runtime/image.hpp'):
            continue
        src = open(path, errors='replace', newline='').read()
        out = step_helpers(src, stats, variants, rel)
        out = step_casts(out, stats)
        if out != src:
            changed += 1
            if not dry:
                with open(path, 'w', newline='') as o:
                    o.write(out)
    for k, n in sorted(stats.items()):
        print('  %-16s %6d' % (k, n))
    print('%d files %s; %d other definitions of obj / str / global kept (variants):' % (
        changed, 'would change' if dry else 'changed', len(variants)))
    for rel, ln, name, line in variants:
        print('  %s:%d %s' % (rel, ln, line[:110]))
    if report:
        with open(report, 'w') as o:
            for rel, ln, name, line in variants:
                o.write('%s\t%d\t%s\t%s\n' % (rel, ln, name, line))


if __name__ == '__main__':
    main()
