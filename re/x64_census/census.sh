#!/bin/bash
# x64 compiler census (docs/X64.md): syntax-checks every source under src/ for i686 and for x86_64 with llvm-mingw
# and keeps the logs; diff.py then counts only the diagnostics that are new on x86_64.
#   re/x64_census/census.sh <outdir> [jobs]      (then: python3 re/x64_census/diff.py <outdir>)
set -e
OUT=$(realpath -m "$1"); J=${2:-2}
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
SDL=${JOAN_SDL3_INCLUDE:-/tmp/sdl3/include}
mkdir -p "$OUT/x86" "$OUT/x64"
cat > "$OUT/one.sh" <<EOS
#!/bin/bash
A=\$1; R=\$2; B=\$(echo \$R | tr / _)
if [ \$A = x86 ]; then C=/opt/llvm-mingw/bin/i686-w64-mingw32-clang++; else C=/opt/llvm-mingw/bin/x86_64-w64-mingw32-clang++; fi
cd "$ROOT/src"
timeout 600 \$C -std=c++20 -fsyntax-only -fms-extensions -fasm-blocks -ferror-limit=0 -Wall -Wextra -Wshorten-64-to-32 \\
  -Wno-unused-parameter -DWIN32_LEAN_AND_MEAN -DNOMINMAX -I. -isystem "$SDL" \$R > "$OUT/\$A/\$B.log" 2>&1
echo \$? > "$OUT/\$A/\$B.rc"
EOS
chmod +x "$OUT/one.sh"
(cd "$ROOT/src" && find . -name '*.cpp' | sed 's#^\./##' | sort) > "$OUT/files.txt"
(for a in x86 x64; do sed "s/^/$a /" "$OUT/files.txt"; done) | xargs -P "$J" -L 1 "$OUT/one.sh"
echo "done: $(wc -l < "$OUT/files.txt") files"
