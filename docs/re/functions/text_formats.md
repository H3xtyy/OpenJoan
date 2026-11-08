# Text-format loaders — joan.exe (reference build)

Found 2026-10-22 by literal→xref export (`tools/ghidra/ExportByStrings.java`, pseudocode under
`$JOAN_WORK_DIR/decomp/textfmt_loaders/`). Confidence: **literal-xref** unless marked verified (decompiled and read).

| address | name (proposed) | module | role | status |
|---|---|---|---|---|
| 0x589C70 | `FileTxt::get_token` | ilib/filetxt | tokenizer: skips ` ,:\t`, stops at ` =,:\t\r`, 0x1A = EOF, max 0x80 | verified |
| 0x589F10 | `FileTxt::read_line` | ilib/filetxt | rest of line into buffer (n) | verified (callers) |
| 0x5899B0 | `FileTxt::next_line` | ilib/filetxt | advance to the next line | literal-xref |
| 0x558890 | `World::load_uni` | world_s | token loop over `CreateUnit`; post-pass groups units by +0x31 | verified |
| 0x558A60 | `World::read_unit_block` | world_s | positional `.uni` reader (see docs/formats/uni.md) | verified |
| 0x44E5F0 | `World::load_stg_blocks` | world_s | `.stg` dispatcher (Object/Light/Trigger/Water/Particle/Node) | literal-xref |
| 0x42C6B0 | `Scene::read_sector` | world_s | `CreateSceneSector` | literal-xref |
| 0x54A6C0 | `World::load_wor` | world_s | `CreateNode`/`CreateWater` in `.wor` | literal-xref |
| 0x559FF0 | `World::load_pla` | world_s | `CreatePlace`/`CreateFacility` | literal-xref |
| 0x547590 | `Territory::load_ter` | world_s/nation | `CreateTerritory` | literal-xref |
| 0x5593E0 | `World::load_por` | world_s | `Anti-Portal` | literal-xref |
| 0x4F2F80 | `World::open_map_file` | world_s | builds `MAP\…` paths with `.cm`/`.trk` | literal-xref |
| 0x428E90 | `CamRes::load_cm` | camres | `CreateCameraTrackArray`/`CreateCameraAnimatedTrack` | literal-xref |
| 0x4EBB40 | `TrackRes::load_trk` | trackres | `CreateTrack`/`CreateTrackArray` | literal-xref |
| 0x731A60 | `MiniMap::load_bm` | interfac/minires | `CreateLight`/`CreateMapActor` | literal-xref |
| 0x4A5BF0 | `LightSys::load_common` | lightsys | `.lgt` + `.sca` (common/map) | literal-xref |
| 0x4A6840 | `LightSys::load_special` | lightsys | `.lgt` + `.sca` (`special`) | literal-xref |
| 0x4A7450 | `LightSys::load_day` | lightsys | `.day` | literal-xref |
| 0x4D48F0 | `Scatter::apply` | plasma/scatter | consumes `Rayleigh` … | literal-xref |
| 0x4CCFB0 | `Scenario::load_scr` | scenario | `SCENARIO\scenario_%02d.scr` | literal-xref |
| 0x638AA0, 0x63F580 | `TextRes::load_ex` | t_scrres | `*EX.res` string tables | literal-xref |
| 0x63F8A0, 0x63FCA0 | `ScriptParser::parse` | t_scrres | `.sce` parser (`Expecting …` errors) | literal-xref |
| 0x677390 | `PScript::parse` | pscript | particle script parser with version check | literal-xref |
| 0x4C1B40 | `PScript::load_dir` | pscript | `SCRIPT\PSCRIPT\*.txt` scan | literal-xref |
| 0x488400, 0x4884C0, 0x488590 | `HelpRes::*` | help | `help.res` | module attribution |
| 0x4DE530..0x4DE600, 0x454090 | `GamePlay::section_*` | gameplay | `Gameplay.res` sections (`CHARMOVE`) | literal-xref |
| 0x5A21E0 | `TxtRes::load` | ilib/txtres | `.txr` reader (`index\|KEY\|n\|text`), called with the literal path from 0x4DA620 / 0x4DE9C0 | verified (docs/formats/txr.md, Loader behaviour) |
| 0x47AD40 | `World::read_node` | world_s | `CreateNode` block reader inside the `.wor` loop 0x54A6C0 | cited by loader (unverified) |
| 0x553AA0 | `World::read_water` | world_s | `CreateWater` block reader inside the `.wor` loop 0x54A6C0 | cited by loader (unverified) |
| 0x55A090 | `World::read_place` | world_s | `CreatePlace` reader called by 0x559FF0 | cited by loader (unverified) |
| 0x55AE40 | `World::read_facility` | world_s | `CreateFacility` reader called by 0x559FF0 | cited by loader (unverified) |
| 0x559470 | `World::read_antiportal` | world_s | per-block reader for `Anti-Portal` called by 0x5593E0 | cited by loader (unverified) |
| 0x4EBD50 | `TrackRes::read_track` | trackres | per-block reader for `CreateTrack` called by 0x4EBB40 (not exported) | cited by loader (unverified) |
| 0x731B00 | `MiniMap::read_light` | interfac/minires | field reader for `CreateLight` in 0x731A60 (not exported) | cited by loader (unverified) |
| 0x731FF0 | `MiniMap::read_map_actor` | interfac/minires | field reader for `CreateMapActor` in 0x731A60 (not exported) | cited by loader (unverified) |
| 0x5270B0 | `UnitRes::find_by_file_name` | unitres | `UnitFile` name to unit type id (used by 0x558A60) | cited by loader (unverified) |
| 0x4B91A0 | `Nation::find_by_name` | nation | `Kingdom` name to nation recno (used by 0x558A60) | cited by loader (unverified) |
| 0x4E4610 | `Task::find_by_name` | task | `Task` name to task id (used by 0x558A60) | cited by loader (unverified) |
| 0x7B15CB | CRT `atoi` | CRT | numeric parse of the `.uni` `Group` token (non-numeric gives 0) | inferred: callers pass decimal tokens |

Retail-build counterparts: TODO(re) per function (string xrefs on `joan.exe` give them the same way).
