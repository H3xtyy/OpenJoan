# Plasma (`plasma.cpp`) — signed 16-bit 2D grid ("plasma" height field)

Despite the name there is no visual effect: `plasma.cpp` is a tiny container for a `(w+1) x (h+1)` grid of `i16`, used by the world/terrain loader
(`world_s.cpp` 0x55B720 and 0x4F2F80) as the **terrain height grid** of one block; the sampler functions in the 0x4C5xxx..0x4C7xxx range are its
accessors. (Name = the diamond-square "plasma fractal" lineage of height maps; the shipped maps store heights, not generated noise.)
Reference build: joan.exe. Related: [WorldGridCell](WorldGridCell.md), [gri.md](../../formats/gri.md), [pln.md](../../formats/pln.md) (height `z` from the sampler 0x4C7B70).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 2 | i16 | w | cell columns; the grid has `w + 1` samples per row | 0x4C58A0 |
| 0x02 | 2 | i16 | h | cell rows (`h + 1` rows) | 0x4C58A0 |
| 0x04 | 4 | i16* | data | `(w+1)*(h+1)*2` bytes, row-major: `data[y*(w+1)+x]` | 0x4C5A20, 0x4C58A0 |
| 0x08 | 12 | bytes | (copied) | copy constructors copy 0x14 bytes before re-allocating `data`; 12 bytes unknown | 0x4C56F0, 0x4C57B0 |

## Functions
| address | proposed name | purpose |
|---|---|---|
| 0x4C56F0 | `Plasma::Plasma(const Plasma&)` | memcpy 0x14 bytes, allocate `(w+1)(h+1)*2`, copy samples |
| 0x4C57B0 | `Plasma::operator=` | free, then as above |
| 0x4C58A0 | `Plasma::create(w, h, fill)` | free; allocate; fill all samples with `fill` (`0x591960` word fill, forwards pairs of i16) |
| 0x4C5960 / 0x4C5870 | `Plasma::free` | free `data`, zero `w`,`h`,`data` |
| 0x4C5A20 | `Plasma::set(x, y, v)` | `data[y*(w+1)+x] = v` |
| 0x4C59D0 | `Plasma::get(x, y) -> i16` | not exported; inferred from the samplers |
| 0x4C7B70 | `Plasma::sample_flat(x, y, mode)` | triangle-split linear height. cell = floor(x), floor(y) clamped to `[0, w-1]`; fractions u,v. `mode 0` (diagonal A) and `mode 1` (diagonal B) pick the triangle by `u+v > 1` / `v-u > 0` and interpolate linearly; any other mode returns 0.0 |
| 0x4C7100 | `Plasma::sample_smooth(x, y, &dzdx, &dzdy) -> float` | bicubic (Hermite-patch) height with analytic partials; falls back to bilinear-style edge cases at borders; helpers 0x4C6CD0 / 0x4C6E40 / 0x4C6FB0 (x/y/xy derivative estimates, not exported). Used to lay object vertices onto terrain and to tilt them by slope (0x55B720 rotates by `atan`-style x/y slopes) |

The loader fills the grid from 5-byte sub-vertex records (`R,G,B,s16 height`; height at `+3`) of each block, `n+1` x `n+1` samples.
`TODO(re)`: exact constants `0x9D56C8..0x9D56D4`, `0x9CF798` used inside `sample_smooth`; derivative helpers.
