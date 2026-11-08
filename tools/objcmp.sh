#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# objcmp: proves that a source change is behaviour-free by comparing the 32-bit objects it compiles to.
#
#   tools/objcmp.sh <before tree> <after tree> <outdir> <file>...
#
# <before tree> / <after tree>: two checkouts (each holding src/), e.g. a `git worktree add` of the baseline commit
# and the working tree. <file>: paths relative to the tree (src/game/unit.cpp). Each file is compiled from both trees
# with the exact command of the reference build's compile_commands.json (OBJCMP_BUILD, default build/linux-x86: the
# 32-bit reference build, configured by tools/objcmp-x86.sh), minus the precompiled header (a build-speed measure with no effect on the code) and with a
# fixed -frandom-seed, into <outdir>/before and <outdir>/after; debug information is stripped (line numbers move with
# every edit) and the objects compared byte by byte.
#
# Output, one line per file:  SAME <file>  |  DIFF <file> (<n> differing disassembly lines, see <outdir>/<name>.diff)
#                             |  NOOBJ <file> (no compile command, or a compile failed: <outdir>/<name>.{before,after}.log)
# and a summary line. Exit status 0 when every file is SAME, 1 otherwise. A DIFF that only reorders or renumbers
# (no changed displacement, width, signedness or constant) may be accepted after review ("reviewed DIFF");
# say so in the commit message.
set -u
if [ $# -lt 4 ]; then
    sed -n '3,20p' "$0" | sed 's/^# \{0,1\}//'
    exit 2
fi
before=$(realpath "$1"); after=$(realpath "$2"); out=$(realpath -m "$3"); shift 3
root=$(cd "$(dirname "$0")/.." && pwd)
build=$(realpath -m "${OBJCMP_BUILD:-$root/build/linux-x86}")
cc_json="$build/compile_commands.json"
if [ ! -f "$cc_json" ]; then
    echo "objcmp: $cc_json not found: configure the reference build first (tools/objcmp-x86.sh)" >&2
    exit 2
fi
# the source tree the reference build was configured from (its compile commands name files under it): this checkout,
# or another one such as a worktree of the reference commit
src_root=$(sed -n 's/^CMAKE_HOME_DIRECTORY:INTERNAL=//p' "$build/CMakeCache.txt" 2>/dev/null)
[ -n "$src_root" ] || src_root=$root
mkdir -p "$out/before" "$out/after"
same=0; diff_n=0; noobj=0

# compile <tree> <file> <obj> <log>: the reference command with the tree's paths, no PCH, fixed random seed
compile() {
    local tree=$1 file=$2 obj=$3 log=$4
    local cmd
    cmd=$(python3 - "$cc_json" "$src_root" "$tree" "$file" "$obj" <<'EOF'
import json, shlex, sys
cc_json, root, tree, file, obj = sys.argv[1:]
root = root.rstrip('/'); tree = tree.rstrip('/')
for e in json.load(open(cc_json)):
    if e['file'] == root + '/' + file or e['file'] == file:
        args = shlex.split(e['command']) if 'command' in e else list(e['arguments'])
        out = []
        skip = False
        for a in args:
            if skip:
                skip = False
                continue
            if a in ('-o', '-c'):
                skip = True
                continue
            if a == '-Winvalid-pch' or a.endswith('cmake_pch.hxx'):
                if a.endswith('cmake_pch.hxx'):
                    out.pop()      # the -include before it
                continue
            if a.startswith(root + '/') and '/build/' not in a:
                a = tree + a[len(root):]
            elif a.startswith('-I' + root + '/') and '/build/' not in a:
                a = '-I' + tree + a[len('-I' + root):]
            out.append(a)
        out += ['-frandom-seed=' + file, '-c', tree + '/' + file, '-o', obj]
        print(' '.join(shlex.quote(a) for a in out))
        break
EOF
)
    if [ -z "$cmd" ]; then
        echo "no compile command for $file in $cc_json" > "$log"
        return 1
    fi
    (cd "$build" && eval "$cmd") > "$log" 2>&1
}

for file in "$@"; do
    name=$(echo "$file" | tr / _)
    ob="$out/before/$name.o"; oa="$out/after/$name.o"
    if ! compile "$before" "$file" "$ob" "$out/$name.before.log" || ! compile "$after" "$file" "$oa" "$out/$name.after.log"; then
        echo "NOOBJ $file (no compile command, or a compile failed: $out/$name.{before,after}.log)"
        noobj=$((noobj + 1))
        continue
    fi
    objcopy --strip-debug "$ob" "$ob.stripped" && objcopy --strip-debug "$oa" "$oa.stripped"
    if cmp -s "$ob.stripped" "$oa.stripped"; then
        echo "SAME $file"
        same=$((same + 1))
    else
        objdump -d -r --no-show-raw-insn "$ob.stripped" | sed -e '1,2d' -e 's/^[0-9a-f]* <//' > "$out/$name.before.dis"
        objdump -d -r --no-show-raw-insn "$oa.stripped" | sed -e '1,2d' -e 's/^[0-9a-f]* <//' > "$out/$name.after.dis"
        diff "$out/$name.before.dis" "$out/$name.after.dis" > "$out/$name.diff"
        n=$(/usr/bin/grep -c '^[<>]' "$out/$name.diff")
        if [ "$n" = 0 ]; then
            # same code, different data / symbols / relocations: show the section digest difference
            diff <(readelf -S -s "$ob.stripped" | sed 's/^ *[0-9]*: *[0-9a-f]* //') \
                 <(readelf -S -s "$oa.stripped" | sed 's/^ *[0-9]*: *[0-9a-f]* //') > "$out/$name.diff"
            n=$(/usr/bin/grep -c '^[<>]' "$out/$name.diff")
            echo "DIFF $file (code identical; $n differing data/symbol lines, see $out/$name.diff)"
        else
            echo "DIFF $file ($n differing disassembly lines, see $out/$name.diff)"
        fi
        diff_n=$((diff_n + 1))
    fi
done
echo "objcmp: $same SAME, $diff_n DIFF, $noobj NOOBJ (reference build $build)"
[ "$diff_n" = 0 ] && [ "$noobj" = 0 ]
