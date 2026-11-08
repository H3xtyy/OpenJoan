#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""objcmp for moves: compare the UNION of the functions of several objects before and after a change.

  tools/objcmp_union.py <before tree> <after tree> <outdir> <file>...

A move takes function blocks from one source file to another, so one object loses functions and another gains them;
tools/objcmp.sh (file by file) cannot show that nothing else changed. This tool compiles each listed file in each tree
where it exists (a new destination exists only after, an emptied file only before), with the reference build's
command as tools/objcmp.sh does (OBJCMP_BUILD, default build/linux-x86; the command of a new file must be in it, so
add the file to src/CMakeLists.txt and reconfigure the reference build first), disassembles every function and
compares the two unions by symbol name:

  - every function present before must be present after with the same normalised code, and no new function may
    appear (same symbol = same function: anonymous-namespace and static symbols keep their mangled names);
  - normalised: addresses, string-literal labels (.LC<n>), offsets into .rodata.str and .bss (literals and counters
    move between objects), .rodata.cst constants (compared by value), call targets by relocation symbol, the
    anonymous-namespace part of batch-namespace symbols, alignment padding; an inline function that differs between
    two objects of one tree is noted, not an error;
  - a move changes what the compiler can inline (a callee now in the caller's object, or no longer): with
    --no-inline both trees compile without inlining and the IPA passes, and the result must then be SAME up to
    register allocation in functions whose object changed - the proof used for Phase 3 curation (with a replay).
  - static initialisers (_GLOBAL__sub_I_<file>, __static_initialization_and_destruction) are named after their file
    and are listed apart, not compared: check the registrations with re/check_registrations.py --compare.

Output: SAME (every function identical) or DIFF with the differing / missing / new functions; exit 0 on SAME.
"""
import json
import os
import re
import shlex
import subprocess
import sys

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# --no-inline: compile both trees without inlining and the interprocedural passes, so a function that moved away from
# its callers (or to them) compiles as it did before; the strict proof for moves (Phase 3 wave 1). OBJCMP_EXTRA adds
# flags of one's own.
NO_INLINE = ['-fno-inline', '-fno-inline-functions-called-once', '-fno-ipa-icf', '-fno-ipa-cp', '-fno-ipa-sra',
             '-fno-ipa-ra', '-fno-ipa-pure-const', '-fno-ipa-modref', '-fno-ipa-reference', '-fno-partial-inlining']
EXTRA = (NO_INLINE if '--no-inline' in sys.argv else []) + os.environ.get('OBJCMP_EXTRA', '').split()
sys.argv = [a for a in sys.argv if a != '--no-inline']
before, after, out = (os.path.abspath(a) for a in sys.argv[1:4])
files = sys.argv[4:]
build = os.path.abspath(os.environ.get('OBJCMP_BUILD', os.path.join(root, 'build', 'linux-x86')))
cc = json.load(open(os.path.join(build, 'compile_commands.json')))
src_root = root
try:
    for ln in open(os.path.join(build, 'CMakeCache.txt')):
        if ln.startswith('CMAKE_HOME_DIRECTORY:INTERNAL='):
            src_root = ln.split('=', 1)[1].strip()
except OSError:
    pass
os.makedirs(out, exist_ok=True)


def command(tree, f, obj):
    # a file without a command of its own (renamed or moved away since the reference build was configured) compiles
    # with the command of another src/ file of the same target, its path substituted
    entries = [e for e in cc if e['file'] in (src_root + '/' + f, f)]
    if not entries and f.startswith('src/'):
        entries = [dict(e, file=src_root + '/' + f) for e in cc
                   if e['file'].startswith(src_root + '/src/game/') and e['file'].endswith('.cpp')][:1]
        for e in entries:
            old = e['file'] if 'command' not in e else None
            src = next(x for x in cc if x['file'].startswith(src_root + '/src/game/') and x['file'].endswith('.cpp'))['file']
            if 'command' in e:
                e['command'] = e['command'].replace(src, src_root + '/' + f)
            else:
                e['arguments'] = [a.replace(src, src_root + '/' + f) for a in e['arguments']]
    for e in entries:
        if True:
            args = shlex.split(e['command']) if 'command' in e else list(e['arguments'])
            res, skip = [], False
            for a in args:
                if skip:
                    skip = False
                    continue
                if a in ('-o', '-c'):
                    skip = True
                    continue
                if a == '-Winvalid-pch':
                    continue
                if a.endswith('cmake_pch.hxx'):
                    res.pop()
                    continue
                if a.startswith(src_root + '/') and '/build/' not in a:
                    a = tree + a[len(src_root):]
                elif a.startswith('-I' + src_root + '/') and '/build/' not in a:
                    a = '-I' + tree + a[len('-I' + src_root):]
                res.append(a)
            return res + EXTRA + ['-frandom-seed=' + f, '-c', tree + '/' + f, '-o', obj]
    return None



def norm_sym(t):
    out=''; i=0
    pat=re.compile(r'12_GLOBAL__N_1(\d+)game_')
    while True:
        m=pat.search(t,i)
        if not m:
            out+=t[i:]; break
        out+=t[i:m.start()]+'@ANON@'
        n=int(m.group(1))
        i=m.start()+len('12_GLOBAL__N_1')+len(m.group(1))+n
    return out

STATIC = re.compile(r'_GLOBAL__sub_I_|__static_initialization_and_destruction|_GLOBAL__sub_D_')


def functions(obj):
    dis = subprocess.run(['objdump', '-d', '-r', '--no-show-raw-insn', obj], capture_output=True, text=True).stdout
    res, cur, secs = {}, None, {}
    for ln in dis.splitlines():
        m = re.match(r'^[0-9a-f]+ <(.+)>:$', ln)
        if m:
            cur = m.group(1)
            res[cur] = []
            continue
        if cur is None or not ln.strip() or ln.startswith('Disassembly of section'):
            continue
        mp = re.match(r'^\s+[0-9a-f]+: R_386_PC32\s+(\S+)$', ln)
        if mp and res[cur] and '<' in res[cur][-1]:
            res[cur][-1] = re.sub(r'<[^>]*>', '<' + mp.group(1) + '>', res[cur][-1])
            continue
        mb = re.match(r'^\s+[0-9a-f]+: R_\w+\s+\.bss$', ln)
        if mb and res[cur]:
            res[cur][-1] = re.sub(r'(?<![\w%])\$?0x[0-9a-f]+(?=,|$|\()', 'BSS', res[cur][-1], count=1)
            res[cur].append('R_BSS')
            continue
        mr = re.match(r'^\s+[0-9a-f]+: R_\w+\s+(\.rodata\.cst(\d+))$', ln)
        if mr and res[cur]:
            sec, sz = mr.group(1), int(mr.group(2))
            if sec not in secs:
                dump = subprocess.run(['objdump', '-s', '-j', sec, obj], capture_output=True, text=True).stdout
                data = bytearray()
                for l2 in dump.splitlines():
                    m2 = re.match(r'^ [0-9a-f]+ ((?:[0-9a-f]{2,8} ?)+)', l2)
                    if m2:
                        data += bytes.fromhex(m2.group(1).replace(' ', ''))
                secs[sec] = bytes(data)
            prev = res[cur][-1]
            md = re.search(r'(?<![\w%$])0x([0-9a-f]+)(?=,|$|\()', prev)
            if md:
                off = int(md.group(1), 16)
                res[cur][-1] = prev.replace('0x' + md.group(1), 'CONST[' + secs[sec][off:off + sz].hex() + ']', 1)
            continue
        t = re.sub(r'^\s*[0-9a-f]+:\s*', '', ln)
        t = re.sub(r'\b[0-9a-f]+ <', '<', t)                 # branch target addresses
        t = re.sub(r'\.LC\d+', '.LC', t)                     # literal labels
        t = re.sub(r'(\.rodata[.\w]*)[+-]0x[0-9a-f]+', r'\1', t)  # offsets into the literal sections
        t = norm_sym(t)
        if re.match(r'^(nop|xchg\s+%ax,%ax|data16.*|cs nopw.*|lea\s+(%cs:)?0x0\(%e[sd]i(,%eiz,1)?\),%e[sd]i)\s*$', t.strip()):
            continue
        res[cur].append(t.strip())
    return res


def union(tree, tag):
    u, static = {}, []
    for f in files:
        if not os.path.exists(os.path.join(tree, f)):
            continue
        obj = os.path.join(out, tag + '_' + f.replace('/', '_') + '.o')
        cmd = command(tree, f, obj)
        if cmd is None:
            sys.exit('no compile command for %s in %s (add it to src/CMakeLists.txt and reconfigure)' % (f, build))
        r = subprocess.run(cmd, cwd=build, capture_output=True, text=True)
        if r.returncode:
            # (a file that compiles only with the precompiled header - a missing include - fails here in both trees)
            open(os.path.join(out, tag + '_' + f.replace('/', '_') + '.log'), 'w').write(r.stderr)
            noobj.append('%s (%s)' % (f, tag))
            continue
        subprocess.run(['objcopy', '--strip-debug', obj], check=True)
        for name, body in functions(obj).items():
            if STATIC.search(name):
                static.append((f, name))
            elif name in u and u[name] != body:
                print('note: %s differs between two objects of the %s tree (an inline function; first kept)' % (name, tag)); continue
            else:
                u[name] = body
    return u, static


noobj = []
A, sa = union(before, 'before')
B, sb = union(after, 'after')
missing = sorted(set(A) - set(B))
new = sorted(set(B) - set(A))
diff = sorted(n for n in set(A) & set(B) if A[n] != B[n])
rep = []
for n in missing:
    rep.append('missing after: ' + n)
for n in new:
    rep.append('new after:     ' + n)
for n in diff:
    rep.append('differs:       ' + n)
open(os.path.join(out, 'union.txt'), 'w').write('\n'.join(rep) + '\n')
print('functions: %d before, %d after; static initialisers (not compared): %d before, %d after'
      % (len(A), len(B), len(sa), len(sb)))
if noobj:
    print('NOOBJ (compile failed, see %s/*.log): %s' % (out, ', '.join(noobj)))
    rep += ['noobj: ' + x for x in noobj]
if rep:
    print('DIFF: %d missing, %d new, %d differing (%s)' % (len(missing), len(new), len(diff),
                                                          os.path.join(out, 'union.txt')))
    for x in rep[:20]:
        print('  ' + x)
    sys.exit(1)
print('SAME (union of %d files)' % len(files))
