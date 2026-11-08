# `.por` — anti-portals (occlusion planes) (`map/scenarioN/*.por`, 32 files)

Reader (`src/game/world_s.cpp`): **0x5593E0** (`Anti-Portal` 0x9D8390; per-block
reader 0x559470). Own syntax (not the block grammar); blank lines and CRLF are ignored:
```
Anti-Portal Number: N
N x Anti-Portal {
   ((x, y, z),                               ; four quad corners, world units, one per line
    (x, y, z),
    (x, y, z),
    (x, y, z))
   Pos1: x, y, z                             ; negative half-extents in portal space
   Pos2: x, y, z                             ; positive half-extents
   Center: x, y, z                           ; quad centre, world units
   HDir: radians                             ; own line
   VDir: radians                             ; own line (not on the HDir line)
}
```
Corners are written in zig-zag order (top-right, top-left, bottom-right, bottom-left in the sample), not around the
perimeter. `Number` equals the block count in all 32 files (total 312 portals; five files have 0). The angles
range from about -0.9 to 6.11 radians, i.e. they are not normalised. Renderer-only data (occlusion).
