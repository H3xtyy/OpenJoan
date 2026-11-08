#!/usr/bin/env bash
# Capture a Direct3D 8 trace of the original game under Wine with the Windows build of apitrace.
# Usage: tools/re/apitrace_capture.sh <name> [seconds]   → $JOA_WORK_DIR/traces/<name>.trace
# Needs: JOA_WORK_DIR (copy of the install at $JOA_WORK_DIR/game with joan_steam.exe), WINEPREFIX (32-bit),
#        $JOA_WORK_DIR/tools/apitrace-14.0-win32/ (see docs/re/ENVIRONMENT.md). Never run against JOA_GAME_DIR.
# The wrapper d3d8.dll is copied next to the exe only for the duration of the run and removed afterwards.
# apitrace writes the trace to the Wine user's Desktop (C:\users\<user>\Desktop\joan_steam.trace); we move it.
set -euo pipefail
name=${1:?trace name}; secs=${2:-0}
: "${JOA_WORK_DIR:?}"; : "${WINEPREFIX:=$JOA_WORK_DIR/wine32}"; export WINEPREFIX
game="$JOA_WORK_DIR/game"; wrap="$JOA_WORK_DIR/tools/apitrace-14.0-win32/lib/wrappers/d3d8.dll"
[ -f "$wrap" ] || { echo "missing $wrap" >&2; exit 1; }
[ -f "$game/joan_steam.exe" ] || { echo "missing $game/joan_steam.exe" >&2; exit 1; }
mkdir -p "$JOA_WORK_DIR/traces"
cp "$wrap" "$game/d3d8.dll"
trap 'rm -f "$game/d3d8.dll"' EXIT
cd "$game"
if [ "$secs" -gt 0 ]; then
  WINEDEBUG=-all timeout -s INT "$secs" wine joan_steam.exe || true   # SIGINT lets apitrace flush
else
  WINEDEBUG=-all wine joan_steam.exe || true                          # play, then quit the game normally
fi
wineserver -w
src=$(find "$WINEPREFIX/drive_c/users" -maxdepth 3 -name 'joan_steam.trace' -newer "$wrap" | head -1)
[ -n "$src" ] || { echo "no trace produced" >&2; exit 1; }
mv "$src" "$JOA_WORK_DIR/traces/$name.trace"
echo "trace: $JOA_WORK_DIR/traces/$name.trace ($(du -h "$JOA_WORK_DIR/traces/$name.trace" | cut -f1))"
echo "analyse: apitrace dump $JOA_WORK_DIR/traces/$name.trace | grep SetRenderState | sort | uniq -c"
