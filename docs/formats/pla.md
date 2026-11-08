# `.pla` — places and facilities (`map/scenarioN/*.pla`, 32 files)

Reader: dispatch **0x559FF0** (`CreatePlace` 0x9D83EC -> reader 0x55A090, `CreateFacility` 0x9D83D8 -> reader 0x55AE40).
Grammar: [block.md](block.md) with two **list sentinels**: the bare fields `ENTRANCE_END;` and `PARKING_END;` terminate
(possibly empty) repeated groups. Keys are read positionally, in this order:
```
CreatePlace {
    MODEL_NAME: UNKNOWN|<smo>; PLACE_NAME: <text>; PLACE_SCALE: <scale>; X; Y; Z; HD; WIDTH; LENGTH; HEIGHT;
    [ENTRANCE_X; ENTRANCE_Y; ENTRANCE_Z; ENTRANCE_DIR;]* ENTRANCE_END;
    [PARKING_X;  PARKING_Y;  PARKING_Z;  PARKING_DIR;]*  PARKING_END;
    KINGDOM: <nation>|UNKNOWN;
}
CreateFacility {
    FACILITY_CLASS: <class>; PLACE_NAME; FACILITY_NAME; X; Y; Z; HD; WIDTH; LENGTH; HEIGHT;
    [ENTRANCE_X; ENTRANCE_Y; ENTRANCE_Z; ENTRANCE_DIR;]* ENTRANCE_END;
    [RECRUIT_LIST: <name>; WORKSHOP_TYPE: <type>; WORKSHOP_RESOURCE: <int>;]      ; workshops only
}
```
`X Y Z HD` are floats (two decimals), `WIDTH LENGTH HEIGHT` ints. Scripts find these by `search_place_by_name` /
`search_facility_by_name` (localised names!).

## Observations over all 32 files
- 610 `CreatePlace` + 910 `CreateFacility` blocks. `HD` is always `0`.
- **Entrances:** every `ENTRANCE_END;` is empty (1,520 of 1,520), so the `ENTRANCE_X/Y/Z/DIR` spelling is *unverified*
  (assumed by symmetry with parking; `TODO(re)`: read 0x55A090).
- **Parking:** 87 `PARKING_X/Y/Z/DIR` groups, only in `CreatePlace` (facilities have no parking list).
- `PLACE_SCALE` (case varies, compare case-insensitively): `Castle` (castle/Castle/CASTLE), `Town` (Town/TOWN),
  `umbrella` (umbrella/Umbrella/UMBRELLA), `Village`.
- `FACILITY_CLASS`: `BARRACK` (also written `Barrack`), `SHOP`, `WORKSHOP`, `RECRUIT`.
- Workshops (13): `RECRUIT_LIST` (e.g. `S5L5_wood_workshop`), `WORKSHOP_TYPE` `IRON|WOOD|NORMAL`, `WORKSHOP_RESOURCE`
  `0|4000|10000`; they follow `ENTRANCE_END;`.
- `KINGDOM`: `english|French|Mercen` in any case, or `UNKNOWN` (208 places).
