# Module map — embedded source paths in joan.exe → subsystems

109 `d:\ww\...` paths survive in the binary (assert/log sites). Use them to attribute functions
(method: [ATTRIBUTION.md](ATTRIBUTION.md)). Columns: original file → inferred subsystem → OpenJoan file(s); the per-function
mapping is `re/attribution.csv`.

## Engine core — `ilib` (Enlight's base library)
| source | role (inferred) | OpenJoan file(s) |
|---|---|---|
| ilib\resdb.cpp, resx.cpp, reslx.cpp | resource databases (`.res` binary record DBs, `Resource/Resource` index) | `src/ilib/resdb.cpp`, `src/ilib/resx.cpp`, `src/ilib/reslx.cpp` |
| ilib\db.cpp | typed field DB (`Cursor.res` style `HFILENAME` descriptors) | `src/ilib/db.cpp` |
| ilib\gameset.cpp | `.set` files (Interfac.set, Std.set) | `src/ilib/gameset.cpp` |
| ilib\txtres.cpp | text resource tables (`.txr`, `_des`, `EX.res`) | `src/ilib/txtres.cpp` |
| ilib\filemem.cpp, filetxt.cpp | file abstraction, text tokenizer | `src/ilib/filemem.cpp`, `src/ilib/filetxt.cpp` |
| ilib\blob.cpp, dynarr.cpp, dynarrb.cpp | containers | `src/ilib/blob.cpp`, `src/ilib/dynarr.cpp`, `src/ilib/dynarrb.cpp` |
| ilib\bitmap*.cpp (bitmap, bitmapd, bitmapm, bitmapt, bitmapw) | 2D bitmaps, DIB, transparent/masked blits | `src/ilib/bitmap.cpp`, `src/ilib/bitmapd.cpp`, `src/ilib/bitmapm.cpp`, `src/ilib/bitmapt.cpp`, `src/ilib/bitmapw.cpp` |
| ilib\flc.cpp | FLC/FLI animation player (legacy) | `src/ilib/flc.cpp` |
| ilib\jpeg.cpp (elib) + libpng/zlib strings | image codecs | `src/elib/jpeg.cpp` |
| ilib\audio.cpp, sound3d.cpp, se.cpp | DirectSound + OpenAL 3D audio, sound effects | `src/ilib/audio.cpp`, `src/ilib/sound3d.cpp`, `src/ilib/se.cpp` |
| ilib\mousecr.cpp | mouse cursor (`Cursor.res`, `I_cursor.res`) | `src/ilib/mousecr.cpp` |

## 3D engine — `en3d`, `uti3d`
| source | role | OpenJoan file(s) |
|---|---|---|
| en3d\sysen.cpp | D3D8 device/system init, caps, present | `src/en3d/sysen.cpp` |
| en3d\vgabuf.cpp, vgalock.cpp | 8-bit VGA palette helpers (RGB → 16/32-bit conversion tables), not vertex buffers — see docs/re/structs/PaletteBuf.md (corrected 2026-10-22) | `src/en3d/vgabuf.cpp`, `src/en3d/vgalock.cpp` |
| en3d\texturea.cpp, texdir.cpp | texture loading (`.dds`), texture directory/cache | `src/en3d/texturea.cpp`, `src/en3d/texdir.cpp` |
| en3d\pshader.cpp, plane_sh.cpp | pixel shaders (`.pso`), shader-based plane rendering | `src/en3d/pshader.cpp`, `src/en3d/plane_sh.cpp` |
| en3d\plane.cpp, scene_s.cpp, reorder.cpp | mesh "planes" (submeshes), scene sorting, draw reordering (transparency) | `src/en3d/plane.cpp`, `src/en3d/scene_s.cpp`, `src/en3d/reorder.cpp` |
| en3d\actor.cpp, actordef.cpp, actordeg.cpp | actors (model instances), definitions (`.smo`), degradation/LOD? | `src/en3d/actor.cpp`, `src/en3d/actordef.cpp`, `src/en3d/actordeg.cpp` |
| en3d\animdef.cpp, bonetree.cpp, morph.cpp, skin.cpp | animations (`.am1/2/3`), bone hierarchy, morph targets, skinning (`hardware_skin.nvo`, `v3d_*.dll`) | `src/en3d/animdef.cpp`, `src/en3d/bonetree.cpp`, `src/en3d/morph.cpp`, `src/en3d/skin.cpp` |
| uti3d\video.cpp, videocap.cpp | WMV playback (wmvcore), video capture | `src/uti3d/video.cpp`, `src/uti3d/videocap.cpp` |
| uti3d\lflare.cpp | lens flare | `src/uti3d/lflare.cpp` |
| uti3d\dlgres.cpp | dialog resources (cut-scene dialog UI) | `src/uti3d/dlgres.cpp` |
| uti3d\esstate2.cpp | effect/engine state machine? | `src/uti3d/esstate2.cpp` |
| uti3d\spindex.cpp, blkarrb.cpp | spatial index, block arrays | `src/uti3d/spindex.cpp`, `src/uti3d/blkarrb.cpp` |

## 2D UI — `en2d`, `interfac`
| source | role | OpenJoan file(s) |
|---|---|---|
| en2d\font2d.cpp | bitmap fonts (`.fin` + `int8x6/*.dds`), GDI TrueType rasterising | `src/en2d/font2d.cpp` |
| en2d\image2d.cpp, bmpres.cpp | UI images | `src/en2d/image2d.cpp`, `src/en2d/bmpres.cpp` |
| en2d\popupwin.cpp, txtbox2d.cpp | popups, text boxes | `src/en2d/popupwin.cpp`, `src/en2d/txtbox2d.cpp` |
| interfac\combobox.cpp, list.cpp, objmenu*.cpp, optmenui.cpp, minires.cpp | widgets, object menus, options menu, minimap resources | `src/interfac/combobox.cpp`, `src/interfac/list.cpp`, `src/interfac/objmenu.cpp`, `src/interfac/objmenub.cpp`, `src/interfac/objmenuw.cpp`, `src/interfac/optmenui.cpp`, `src/interfac/minires.cpp` |
| interfac\i_barr.cpp, i_buy.cpp, i_work.cpp, i_letter.cpp, i_sms.cpp | barracks, shop (buy), workshop, letter, message interfaces | `src/interfac/i_barr.cpp`, `src/interfac/i_buy.cpp`, `src/interfac/i_work.cpp`, `src/interfac/i_letter.cpp`, `src/interfac/i_sms.cpp` |

## Game — `ww` root
| source | role | OpenJoan file(s) |
|---|---|---|
| game.cpp, gameplay.cpp, world.cpp, world_s.cpp | main loop, gameplay rules (`Gameplay.res` sections ECON/THROWI/UNIT/CHARMOVE/WEATHER/GAME), world/terrain (`TERRAIN\`, `c_terrain_reflectance`) | `src/game/game.cpp`, `src/game/gameplay.cpp`, `src/game/world.cpp`, `src/game/world_s.cpp` |
| gamefile.cpp, gamefila.cpp, gamefil2.cpp, gfile3.cpp | world/level/save file I/O (`.uni`, `*w`, `.SAV`, `_AUTO%03d.SAV`) | `src/game/gamefile.cpp`, `src/game/gamefila.cpp`, `src/game/gamefil2.cpp`, `src/game/gfile3.cpp` |
| levelres.cpp, scenario.cpp, sdateres.cpp | level resources, scenario flow (`.scr`, stages), scenario dates (`set_game_start_date 1429 4 29`) | `src/game/levelres.cpp`, `src/game/scenario.cpp`, `src/game/sdateres.cpp` |
| t_scrres.cpp, uscrres.cpp, animscr.cpp | **script resources** — scenario script parser/VM, unit scripts, animation scripts (`animscr.res`) | `src/game/t_scrres.cpp`, `src/game/uscrres.cpp`, `src/game/animscr.cpp` |
| unit.cpp, unitres.cpp, unitlvl.cpp, unitures.cpp, upgrares.cpp, recrures.cpp, raceres.cpp, nation.cpp | units, unit stats res, levels/XP, unit upgrades, recruit lists, races, nations/friendship | `src/game/unit.cpp`, `src/game/unitres.cpp`, `src/game/unitlvl.cpp`, `src/game/unitures.cpp`, `src/game/upgrares.cpp`, `src/game/recrures.cpp`, `src/game/raceres.cpp`, `src/game/nation.cpp` |
| battle.cpp, flocksys.cpp | combat, flocking/formation movement | `src/game/battle.cpp`, `src/game/flocksys.cpp` |
| itemres.cpp, ditemres.cpp, sitemres.cpp, f_shop.cpp | items, drop items (`dropitem.res`), shop items, shop logic | `src/game/itemres.cpp`, `src/game/ditemres.cpp`, `src/game/sitemres.cpp`, `src/game/f_shop.cpp` |
| i_gate.cpp, i_wall.cpp | gates and walls (HP, keys, break) — script FUNCs `get_gate_state`, `break_wall` | `src/game/i_gate.cpp`, `src/game/i_wall.cpp` |
| camres.cpp | camera tracks (`.cm`, `rts.cm`, `cutscene.cm`) | `src/game/camres.cpp` |
| lightsys.cpp, dyngrass.cpp, plasma.cpp | lights/day stages (`.lgt`, `.pro`), dynamic grass, plasma (sky/water effect?) | `src/game/lightsys.cpp`, `src/game/dyngrass.cpp`, `src/game/plasma.cpp` |
| musicres.cpp, speech_s.cpp | music tables (`.wma`), speech scripts ("Error loading Speech Script") | `src/game/musicres.cpp`, `src/game/speech_s.cpp` |
| modelres.cpp, animgrp.cpp, animres.cpp | model and animation resource tables (`.smo`, `.am*`, `zCripple.am1`) | `src/game/modelres.cpp`, `src/game/animgrp.cpp`, `src/game/animres.cpp` |
| help.cpp, msglog.cpp, profile.cpp, remapkey.cpp, cpuusage.cpp | help text (`help.res`), message log, player profile, key remapping, perf counter | `src/game/help.cpp`, `src/game/msglog.cpp`, `src/game/profile.cpp`, `src/game/remapkey.cpp`, `src/game/cpuusage.cpp` |
| stackt.h | stack trace helper (crash handler) | see `re/attribution.csv` |

## Loose ends to attribute
`SOFTBONE.SYS`, `no_save_preview_pict.sys`, `fastflip.sys` (0 bytes) — `.sys` here are data files, not drivers.
`zCripple.am1` — a special animation referenced by name. `MOVIE\TREVOR.WMV` referenced but not shipped.
