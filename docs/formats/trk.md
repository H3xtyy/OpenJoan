# `.trk` — unit tracks (`map/scenarioN/*.trk`, 32 files)

Reader: dispatch `world_s.cpp` 0x4F2F80; block reader **0x4EBB40** (`CreateTrack`/`CreateTrackArray`
0x9D72C4/0x9D72D4; header reader 0x4EC240, per-track reader 0x4EBD50, record size 0x52 bytes = 82 per track, neither
reader is identified further). Grammar: [block.md](block.md).

```
CreateTrackArray { size:N; }
CreateTrack {                          ; × N, fields have NO ';' terminator (a few files keep one on map_name/code)
    map_name:sample                    ; always "sample"
    code:<name>                        ; track name used by scripts; NOT unique ("Track01" repeats 8× in S2L1_2.trk)
    track_point_count:K
    K × {                              ; one waypoint, six keys in this order
        track_point_recno:<1..K>       ; 1-based, always == position
        track_point_type:0             ; 0 in all 215 points (meaning unknown)
        track_point_pos_x:<float>      ; world units
        track_point_pos_y:<float>
        track_point_pos_z:<float>
        adj_point_count:<int>          ; degree of the point in the edge graph
    }
    E × { edge_start:<recno>; edge_end:<recno> }    ; undirected edge, E = 0..K+2
}
```
In the shipped files `track_point_count` points follow, edges come in pairs and every edge
end refers to a point 1..K. Invariant over all 24 tracks: `sum(adj_point_count) == 2 * E` (each edge counts at both
ends); graphs may contain cycles (S4L1_1 `default`: K=35, E=37; S7L1_2 `GoldTrk2`: K=58, E=58).

## Install census
Only 24 `CreateTrack` blocks exist, in 7 files: `scenario2/S2L1.trk` (5: CampTent1..4, ScoutA, K=2),
`scenario2/S2L1_2.trk` (8× `Track01`, K=2), `scenario4/S4L1_1.trk` (`default`, K=35), `scenario4/S4L1_2.trk`
(`CartTrk`, K=24), `scenario5/S5L1_5.trk` (`cartTrack` K=3, `moveCartTrack` K=11), `scenario6/S6L1_5.trk`
(`Track01`, K=3), `scenario7/S7L1_1.trk` (`GoldTrk1`, K=32), `scenario7/S7L1_2.trk` (`GoldTrk2` K=58, `BAT_TRK` K=9,
`ics1`, `ics2` K=2), `scenario7/S7L1_5.trk` (`BAT_TRK` K=10). (The earlier note that tracks only exist in
scenario4..7 was wrong; scenario2 has 13.) The other 25 files are `size:0;` stubs (34 bytes).

Scripts use tracks via `set_unit_track(unit, "name")`, `unit_track_to_id`, `is_unit_track_exist`.

## Open questions
* Meaning of `track_point_type` (always 0) and how the engine walks the graph at branches (`TODO(re)` 0x4EBD50).
* Which of duplicate `code` names a script lookup returns (first match presumed).
