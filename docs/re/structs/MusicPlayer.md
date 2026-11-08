# MusicPlayer and AmbiencePlayer — joan.exe

Per-frame tick 0x59B1C0 (music) and 0x5866D0 (ambience, 3 channels). Streams are [VideoPlayer](VideoPlayer.md) instances (wmvcore reader -> AL buffers).
Clocks: `DAT_00A500B2` = game ms, `DAT_00A500B6` = frame ms ([TIMING.md](../TIMING.md)).

## MusicPlayer (fastcall this)

| offset | type | name | meaning | status |
|---|---|---|---|---|
| 0x00 | i32 | `enabled` | set to 1 by init 0x59AD30 | V |
| 0x04 | i32 | `track` | current track id (0 = none) | V |
| 0x08 | i32 | `mode` | 0 idle, 1 playing (loops), 2 fading out | V |
| 0x0C | i32 | `volume` | init from settings byte `DAT_00A57FF2` | V |
| 0x10 | u32 | `start_ms` | game ms at (re)start; elapsed = now - start | V |
| 0x18 | u32 | `fade_ms` | remaining fade time; decremented by frame ms | V |
| 0x1C | u32 | `last_tick` | last `now` processed | V |
| 0x20 | VideoPlayer* | `stream` | non-zero when a stream exists | V |

Behaviour: mode 1 -> if the stream reports end (state 3/4 from the pump) seek to 0 and set `start_ms = now` (loop). Mode 2 -> gain =
`volume * K * fade_ms / FADE_TOTAL` until `fade_ms == 0`; `is_active` is then false. Gain applied via the source `AL_GAIN`. K = 0.01 (volume is percent 0..100); FADE_TOTAL = 8000 ms (music, `_DAT_009CF7A4`) and 5000 ms (ambience, `_DAT_009CFC34`).

## AmbiencePlayer

Same, 3 channels: `enabled +0`, `track[ch] +4+4*ch (idx 1..3)`, `mode[ch] +0x10`, `start_ms[ch] +0x28`, `fade[ch] +0x34`, `volume +0x1C` (settings byte `DAT_00A57FF4`),
`last_tick +0x40`, `stream[ch] +0x44..` (index 0x11..0x13). Per-channel enable globals `DAT_00A5CFA0/A4/A8`.
