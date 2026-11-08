# UnitScriptRes (uscrres.cpp) — unit AI table entry, sizeof = 0x28 (40)

Table built by `uscr_load` 0x5462B0 from `SCRIPT\UNIT\unitAI.res` (18 entries). Header object ("UnitAiRes"): `+0 i32 count`, `+4 UnitScriptRes* records`,
`+8 u8 initialised` (note: count first, unlike animscr/texscr whose pointer comes first). 1-based accessor 0x484540 = `base + (i-1)*0x28`.
Format and class list: [formats/unitai.md](../../formats/unitai.md); functions: [functions/script_res.md](../functions/script_res.md) §3.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 2 | u16 | recno | 1-based index = ResLX entry index (the value stored in the unit-type record at unitres `+0x125`) | 0x5462B0, 0x49D810 |
| 0x02 | 0x20 | char[32] | name | `strncpy(…, ResLX entry name, 0x20)`, upper case in the file (`HUMAN_LIGHT_GENERAL`) | 0x5462B0 |
| 0x22 | 4 | Script* | script | lazily created script object (virtual dtor in slot 0, `delete`d by `uscr_release_scripts` 0x546200); 0 after load. Creator = 0x546970 (not exported) | 0x546200 |
| 0x26 | 2 | u16 | ai_class | `uscr_class_from_name(name)` 0x546520; 0 = no native handler (`PIG`), 1..15 = index into the native handler switch 0x5467B0 | 0x5462B0, 0x5467B0 |

Total = 0x28. Status: verified (all four fields written/read in at least two functions).
