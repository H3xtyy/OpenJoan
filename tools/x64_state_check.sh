#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# x64_state_check: the x86-vs-x64 state check of ADR-0005 (docs/X64_STATE_CHECK.md) for one recording.
#
#   tools/x64_state_check.sh <recording> [every] [--x64-recording]
#
# Replays <recording> ($JOAN_WORK_DIR/recordings/<recording>) on the x86 game copy ($JOAN_X86_COPY, default
# $JOAN_WORK_DIR/game) and on the x64 copy ($JOAN_X64_COPY, default $JOAN_WORK_DIR/game-x64) with the normalised
# image dumped every <every>-th frame (default 25), with the settings as recorded otherwise, then compares the dumps
# with tools/replay_image_diff.py (typed non-pointer data minus re/replay/x64_layout_words.txt).
#
# The x86 replay of an x86 recording must match the recording frame for frame (exit 0): that ties the comparison to
# the recorded game. An x64 recording (--x64-recording) cannot match on x86 (its hashes cover layout-dependent
# words), so there the x86 replay only has to reach the end.
#
# Kept: everything printed, in <recording>.statecheck-<date>.txt next to the recording, and both replays' reports
# (<recording>.x86.report.txt, .x64.report.txt). The dumps (~0.5 MB per compared frame and build) are deleted.
# Each copy's openjoan.ini / joan_re.ini is restored afterwards; a replay that did not end leaves its Save folder as
# *.replayed-<stamp>-crash and the one from before the replay is put back. Exit status: 0 the check passed, 1 not.
set -u
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/.." && pwd)
W=${JOAN_WORK_DIR:?set JOAN_WORK_DIR}
X86=${JOAN_X86_COPY:-$W/game}
X64=${JOAN_X64_COPY:-$W/game-x64}
N=${1:?recording name}
E=25
x64rec=0
for a in "${@:2}"; do
    case $a in --x64-recording) x64rec=1 ;; *) E=$a ;; esac
done
R=$W/recordings
[ -f "$R/$N" ] || { echo "no recording $R/$N"; exit 1; }
out=$R/$N.statecheck-$(date +%Y%m%d-%H%M%S).txt
if pgrep -x openjoan >/dev/null; then echo "openjoan already running"; exit 1; fi

ini_of() { if [ -f "$1/openjoan.ini" ]; then echo "$1/openjoan.ini"; else echo "$1/joan_re.ini"; fi; }

# replay <copy> <dump> <tag>: one replay with the image dumped; prints its exit status and the report's last lines
replay() {
    local dir=$1 dump=$2 tag=$3 ini rc b T
    ini=$(ini_of "$dir")
    cp "$ini" "$ini.before-statecheck"
    sed -i "s|^ReplayOnDivergence=.*$|ReplayOnDivergence=stop\nReplayDumpImage=$dump\nReplayDumpImageEvery=$E|" "$ini"
    (cd "$dir" && timeout 5000 ./openjoan --replay "$R/$N" >/dev/null 2>&1); rc=$?
    mv "$ini.before-statecheck" "$ini"
    echo "$tag replay rc=$rc"
    grep -a "exits where\|DIVERGENCE\|state hash mismatches in\|replay summary" "$R/$N.report.txt" | tail -3 | sed 's/^/  /'
    cp "$R/$N.report.txt" "$R/$N.$tag.report.txt"
    b=$(cd "$dir" && ls -d Save.before-replay-* SAVE.before-replay-* 2>/dev/null | sort | tail -1)
    if [ -n "$b" ] && [ "${b#*before-replay-}" \> "$(date -d '-2 hours' +%Y%m%d-%H%M%S)" ]; then
        T=${b#*before-replay-}; local s=${b%%.before-replay-*}
        echo "  $tag replay left $b: restoring it"
        (cd "$dir" && mv "$s" "$s.replayed-$T-crash" && mv "$b" "$s" &&
         if [ -e "CONFIG.DAT.before-replay-$T" ]; then mv CONFIG.DAT "CONFIG.DAT.replayed-$T-crash" && mv "CONFIG.DAT.before-replay-$T" CONFIG.DAT; fi)
    fi
    return $rc
}

{
    echo "state check of $N, every ${E}th frame, $(date -Is); x86: $X86, x64: $X64; commit $(git -C "$root" rev-parse --short HEAD)"
    mkdir -p "$W/dumps"; rm -f "$W/dumps/state-x86.bin" "$W/dumps/state-x64.bin"
    replay "$X86" "$W/dumps/state-x86.bin" x86; rc86=$?
    replay "$X64" "$W/dumps/state-x64.bin" x64; rc64=$?
    python3 "$root/tools/replay_image_diff.py" "$W/dumps/state-x86.bin" "$W/dumps/state-x64.bin" --frames 5 --words 30
    rcd=$?
    rm -f "$W/dumps/state-x86.bin" "$W/dumps/state-x64.bin"
    ok=1
    # x86: 0 = every frame hash matched; x64: 0 or 4 (ran to the end with hash mismatches, ADR-0005)
    if [ $x64rec = 0 ] && [ $rc86 != 0 ]; then echo "FAIL: the x86 replay does not match the x86 recording"; ok=0; fi
    if [ $x64rec = 1 ] && [ $rc86 != 0 ] && [ $rc86 != 4 ]; then echo "FAIL: the x86 replay did not reach the end"; ok=0; fi
    if [ $rc64 != 0 ] && [ $rc64 != 4 ]; then echo "FAIL: the x64 replay did not reach the end"; ok=0; fi
    if [ $rcd != 0 ]; then echo "FAIL: replay_image_diff.py exit $rcd"; ok=0; fi
    [ $ok = 1 ] && echo "PASS" || echo "STATE CHECK FAILED"
    [ $ok = 1 ]
} 2>&1 | tee "$out"
rc=${PIPESTATUS[0]}
echo "kept: $out"
exit $rc
