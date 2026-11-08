# The cross-architecture state check (x86 recording, x64 replay)

A replay on the x64 build never reproduces an x86 recording's per-frame image hashes, and cannot: the recorded hash
covers values that legitimately differ between the two memory layouts (heap offsets of pointers, element sizes of
pointer lists, the renderer's ring-buffer counters), and a recording keeps only the hashes, not the data behind them.
So the x64 replay is checked another way, against the x86 replay of the same recording, which does match the recording
frame for frame:

1. both builds replay the recording with `ReplayDumpImage` on, writing the normalised `.data/.bss` image of every N-th
   frame (the x64 build through its x86-layout view, `src/runtime/replay/x86_view.hpp`), with every other setting as recorded;
   the x86 replay of an x86 recording must match the recording frame for frame (exit 0), which ties the comparison to
   the recorded game (an x64 recording cannot match on x86 and only has to reach its end there);
2. `tools/replay_image_diff.py` compares the two dumps word by word, leaving out
   - pointers and untyped bytes (the typed table, `src/runtime/replay/hash_types.inc`): their values are addresses of the run;
   - the words of `re/replay/x64_layout_words.txt`: typed values that depend on the layout (pointer-list element
     sizes, byte sizes of pointer arrays, and the renderer state that depends on the stream cache: ring bookkeeping,
     draw statistics, the cached texture factor), each listed with its reason;
3. everything left - the game's typed non-pointer data - must be identical in every compared frame.

`tools/x64_state_check.sh <recording> [every] [--x64-recording]` runs the whole check against the two game copies
(`$JOAN_WORK_DIR/game`, `game-x64`), restores their settings and keeps its output in
`<recording>.statecheck-<date>.txt` next to the recording, with both replays' reports (`<recording>.x86.report.txt`,
`.x64.report.txt`). It exits 0 only when the x86 replay matched (x86 recordings), both reached the end and
`replay_image_diff.py` found no difference (that tool exits 1 on a difference and 2 when no frame was compared).

The renderer's counters depend on the layout through the stream cache ([Graphics] StreamCache, keyed by the arrays'
addresses) and grass batching, which records a patch only when its draw did not come from the cache's ring: on
`ref-x86-2026-11-6j.rec` from frame 47,380 x64 drew two grass patches the normal way that x86 replayed batched. With
StreamCache=0 on both builds 6j matched; the check keeps the recorded setting (so the x86 half stays tied to the
recording) and masks what that choice changes instead: the ring bookkeeping, the draw statistics and SysEn's cached
texture factor (a batched patch bypasses SysEn's state cache, a normally drawn one sets it). The check therefore does
not see how many draws a frame made; a missing model shows in the smoke checklist, not here.

A difference found this way is a real x64 bug or a new layout-dependent word; the first one, 2026-11-06, was the
heal / Energy effects' cone models never drawn on x64 (`EffectState::update_parts` passed float angles to the
SceneObject angle setters, which take the angle's dword: on x64 the float went to an XMM register and the setter read
an integer register). Words are added to the mask only with the reason they differ.

## Coverage (Phase 2 exit, 2026-11-07)

Which missions the check has seen, by the owner's recordings (`$JOAN_WORK_DIR/recordings`, outside the repository).
"x86-anchored": an x86 recording whose x86 replay matched it frame for frame in the check; x64 recordings replay
identically on x64 (exit 0) and are compared with an x86 replay that only reaches the end.

| Mission | Recordings | State check |
|---|---|---|
| 1 | `ref-x86-2026-11-7-mission1.rec` (recorded on x64, 44,772 frames) | no difference, 1,790 frames compared |
| 2 | `ref-x86-2026-11-7-mission2a.rec`, `-mission2b.rec` (x64; 57,613 and 49,568 frames) | no difference, 2,304 and 1,982 frames |
| 3 | `ref-x86-2026-11-6-savetest.rec` (x86, loads an x64-written mission 3 save; 9,183 frames) | no difference, 367 frames, x86-anchored |
| 4, 5, 6 | none kept | **open**: played and replayed on x64 to the end without a crash (2026-10-31 .. 11-05, the `4*` / `5*` recordings, deleted since) before the check existed; the owner will record them later |
| 7, 8 | `ref-x86-2026-11-6a .. 6l.rec` (x86; no `6c`) | no difference in any (6j: 2,792 frames, x86-anchored, after the texture-factor mask) |

Outputs of the runs since 2026-11-07 are kept as `<recording>.statecheck-<date>.txt`; earlier runs printed to the
terminal only.

