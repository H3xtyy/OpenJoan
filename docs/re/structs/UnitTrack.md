# `UnitTrack` / `UnitTrackPoint` — unit movement tracks (`trackres.cpp`)

Source: loader `0x4EBB40`, header reader `0x4EC240`, per-track reader `0x4EBD50`. Format: [../../formats/trk.md](../../formats/trk.md).
Both records are byte-packed. Allocation sites cite `d:\ww\trackres.cpp`.

## TrackRes (container; header reader `this`)
| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 4 | `UnitTrack*` | tracks | `count*0x52` bytes, zero-filled, via 0x58EB30 |
| 0x04 | 4 | int | count | `size:N` of `CreateTrackArray`; 0 => stop reading |

1-based indexing: record i is at `tracks + (i-1)*0x52`.

## UnitTrack (sizeof = 0x52 = 82)
| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 4 | int | index | 1-based | verified |
| 0x04 | 33 | char[33] | map_name | first string (`sample`); last char of raw line replaced | read |
| 0x25 | 33 | char[33] | code | track name used by scripts | read |
| 0x46 | 4 | int | point_count | `track_point_count` | verified |
| 0x4A | 4 | `UnitTrackPoint*` | points | `point_count*0x54` bytes, zero-filled; **re-allocated through 0x58EB30 with the old pointer** | verified |

## UnitTrackPoint (sizeof = 0x54 = 84)
| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 4 | int | recno | `track_point_recno` (1-based); the record is stored at slot `recno-1` (not file position) | verified |
| 0x04 | 4 | int | type | `track_point_type` (always 0 in data) | read |
| 0x08 | 4 | float | x | file x + off_x (same two offsets as cameras) | read |
| 0x0C | 4 | float | y | file y + off_y | read |
| 0x10 | 4 | float | z | file z | read |
| 0x14 | 4 | int | adj_count | `adj_point_count`: capacity of adjacency array | verified |
| 0x18 | 4 | `UnitTrackPoint**` | adj | `new int[adj_count]`, zero-filled; filled while reading edges | verified |
| 0x1C..0x4F | 52 | ? | unknown | never written by the loader (runtime state, e.g. occupancy/visited?) | TODO(re) |
| 0x50 | 4 | `UnitTrack*` | owner | back pointer | verified |

Edge reading: after the points, repeat `edge_start:a  edge_end:b` until the next token is not `edge_start`
(the terminator compared is the literal at 0x9D72E8, presumably `}`). For each edge, `b` is put in the first NULL slot of
a's adjacency array and `a` in the first NULL slot of b's: pointers, undirected. Overflow beyond `adj_count` is silently
dropped (no slot found); no bounds validation on recno.
