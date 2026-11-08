# `.cm` — camera tracks (`Resource/rts.cm`, `Resource/cutscene.cm`, `map/scenarioN/*.cm`)

Loaders: file open/dispatch `world_s.cpp` **0x4F2F80** (`.cm` 0x9D81D0, also `.trk`); block parser `camres.cpp`
**0x428E90** (references `CreateCameraTrackArray` 0x9CF8D8 and `CreateCameraAnimatedTrack` 0x9CF8A0).
Array header reader `camres::fn_00429980` **0x429980** (skips 2 tokens, reads the `size` value with atoi, allocates
`size * 0xACA` bytes, i.e. one 2762-byte camera-track record per track). Grammar: [block.md](block.md).

```
CreateCameraTrackArray { size:N; }                 ; number of tracks that follow
CreateCameraAnimatedTrack {                        ; × N
    map_name: AnyMap | <map>;  track_name: <id>;  track_point_count: K;
    K × { frame_time:<ms int>; x; y; z; h_dir:<deg>f; v_dir:<deg>f; rot_a:<deg>f; cam_dist:<units>f; }
}
```
The eight point keys repeat K times in exactly this order (the field count is `3 + 8*K`). `frame_time` is the key time in milliseconds from track start and is strictly increasing, starting at 0.
Angles in degrees with an optional literal `f` suffix. Scripts reference tracks by `track_name`
(`FUNC("change_camera_in_cut_scene", …)`).

The original loop (0x428E90) also accepts a `CreateCameraTrack` block (string at the same table) and records
`0` (static) vs `1` (animated) in the 4-byte field after the track's 16-bit index; every shipped file only uses the
animated form; the field list of a `CreateCameraTrack` block is unverified (no samples). If `size` is 0 the original stops reading immediately (the loop in `CamRes::load_cm` breaks out as soon as the size field is zero).

## Install census (34 files)
* 31 files are `size:0;` stubs (40 bytes, CRLF): every `map/scenarioN/*.cm` except `scenario2/S2L1.cm`.
* `Resource/rts.cm`: 4 tracks `rts_init`, `rts_deinit`, `rts_aim`, `rts_play`, 5 points each, `map_name:AnyMap`.
* `Resource/cutscene.cm`: 2 tracks `cut_scene_move`, `cut_scene_play`, 15 points each, `AnyMap`.
* `map/scenario2/S2L1.cm` (1507 bytes): 1 track `joan_cut_scene_1`, 11 points, `map_name:sample`
  (so three files are non-empty, not two).
* In all of them `x/y/z` are `0.00` for `AnyMap` tracks (pure camera-parameter ramps), real map coordinates for
  `sample` tracks. Interpolation between points is `TODO(re)` (consumer in camman.cpp).

## Open questions
* Interpolation (linear vs spline) and the unit of `cam_dist`.
* Behaviour of the non-animated `CreateCameraTrack` form.
