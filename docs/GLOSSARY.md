# Glossary

Game and engine terms. One definition each; the source in parentheses is a script builtin name from
`docs/scripting/BUILTINS.md` or an original module from `docs/re/MODULES.md`. Definitions inferred only from names
are marked `(TODO(re))` until the semantics are verified in the binary.

## Game concepts

- **recno**: record number, a 1-based index into a resource DB array; the original's universal id for units,
  facilities, items and places (`get_interact_unit_recno`, `set_facility_follow_unit_recno`). Exact base `TODO(re)`.
- **unit**: a character or creature in the world (`ww\unit.cpp`, `search_unit_by_name`).
- **unit code**: the language-independent code string of a unit type; mapped to an id by `unit_code_to_id`.
- **unit name**: the localised display name of a unit; scripts look units up by it
  (`search_unit_by_name "Bastard z Orleanu"`), so comparison is on decoded UTF-8 of the same release.
- **nation**: a faction; units, items, places and territories belong to one (`set_unit_nation`,
  `set_nation_friendship`, `ww\nation.cpp`).
- **place**: a named location on a scenario map (`search_place_by_name`, `get_place_captured`).
- **facility**: a building with a function, such as a barracks, shop or workshop (`search_facility_by_name`,
  `facility_barrack_add_member`, `interfac\i_barr.cpp`, `i_buy.cpp`, `i_work.cpp`).
- **barracks / shop / workshop**: facility kinds with their own interface screens (`interfac\i_barr.cpp`, `i_buy.cpp`,
  `i_work.cpp`).
- **team**: a group of units commanded together (`set_unit_team`, `del_unit_team`, `get_team_size`, `kill_team`).
- **team leader**: the unit leading a team (`get_unit_team_leader_by_team_id`, `facility_barrack_add_leader`).
- **team id**: integer identifying a team within a scenario (`get_unit_team_leader_by_team_id`).
- **stage / scenario stage**: scripted progress state of a scenario (`get_scenario_stage`, `set_scenario_stage`,
  `ww\scenario.cpp`). Not the same as day stage.
- **day stage**: a lighting profile for a time of day, from `.lgt` and `.pro` files (`ww\lightsys.cpp`).
- **interact stage**: state of an interactable object or unit (`get_interact_stage`, `set_interact_stage`).
- **interact flag**: whether a unit or item can be interacted with (`set_can_interact_flag`,
  `set_item_can_interact_flag`).
- **cut scene**: scripted non-interactive sequence (`set_cut_scene_flag`, `is_cut_scene_finished`,
  `change_camera_in_cut_scene`, `set_unit_can_process_in_cut_scene`).
- **end condition**: win/lose rule of a scenario that scripts enable or disable (`set_end_condition_enabled`,
  `set_end_condition_territory_link`).
- **scenario bit flag**: persistent integer-indexed boolean set by scripts (`get_scenario_bit_flag`,
  `set_scenario_bit_flag`).
- **event bit flag**: a second flag set for scenario events (`get_scenario_event_bit_flag`); difference from the
  scenario bit flag `TODO(re)`.
- **item**: an object a unit can carry or use (`search_item_by_name`, `give_item_to_unit`,
  `get_item_quantity_in_unit_inventory`, `ww\itemres.cpp`).
- **drop item**: item a unit may drop when killed (`set_unit_drop_item_id`, `set_unit_drop_item_level`,
  `disable_team_drop_item`, `dropitem.res`).
- **shop item**: item offered for sale by a shop (`shop_set_sell_item_list`, `search_item_sell_list_by_code`,
  `ww\sitemres.cpp`).
- **gate / wall**: destructible structures with hit points, optional key and invincibility (`get_gate_state`,
  `get_gate_hp`, `set_gate_need_key`, `set_gate_invincible`, `get_wall_hp`, `break_wall`, `ww\i_gate.cpp`,
  `i_wall.cpp`).
- **territory**: map region owned by a nation (`set_territory_nation`).
- **minimap icon**: marker on the minimap that scripts can show or hide (`set_minimap_icon_display_enable`).
- **way point**: minimap marker for a destination, optionally following a unit (`minimap_set_way_point`,
  `minimap_set_way_point_follow_unit`).
- **attention call message**: on-screen alert text shown by scripts (`attention_call_message`,
  `attention_call_message_2`).
- **track**: either a camera track (`.cm`, `ww\camres.cpp`) or a unit movement track (`set_unit_track`,
  `unit_track_to_id`, `is_unit_track_exist`); the unit-track format is `TODO(re)`.
- **control flag**: per-unit or per-team flag restricting or granting player control (`set_control_flag`,
  `set_team_control_flag`); exact bits `TODO(re)`.
- **retreat**: unit/AI retreat state set by scripts (`set_unit_retreat`, `get_unit_retreat`).
- **bribe**: buying over an enemy unit with money (`set_can_bribe_flag`, `set_units_can_bribe_flag`,
  `get_enemy_bribed_cost`, `get_bribed_enemy_kill_count`).
- **RTS mode vs character (charmove) mode**: the two control modes, strategic command view versus direct
  character control; the `CHARMOVE` section exists in `Gameplay.res` (`ww\gameplay.cpp`). Switching rules `TODO(re)`.

## Data and file-format terms

- **resdb / res**: `.res` resource databases, binary record DBs or `~`-separated text depending on the file
  (`ilib\resdb.cpp`, `resx.cpp`, `reslx.cpp`, `docs/formats/INVENTORY.md`).
- **set**: `.set` game-set files such as `Interfac.set` and `Std.set` (`ilib\gameset.cpp`).
- **txr**: `.txr` text resource tables (`KEY|n`), e.g. `t_basic.txr` (`ilib\txtres.cpp`).
- **fin**: `.fin` bitmap font metrics; glyphs are in `int8x6/<name>.dds` (`en2d\font2d.cpp`).
- **string id**: language-independent id such as `[0001N]` preceding a dialog or objective string; kept separate from
  the text.
- **code page**: the 8-bit (or DBCS) character encoding of one language release, e.g. cp1250 for the Polish install;
  decoded to UTF-8 on load.

## Engine terms

- **FVF**: Direct3D flexible vertex format, a bitfield describing a vertex layout; used by the Direct3D 8 device the game expects
  (`en3d\plane.cpp`).
- **plane**: a sub-mesh in the `en3d` engine (`en3d\plane.cpp`, `plane_sh.cpp`).
- **actor**: a model instance in the world, built from an `.smo` definition (`en3d\actor.cpp`, `actordef.cpp`).
- **bone tree**: skeleton hierarchy used for animation and skinning (`en3d\bonetree.cpp`).
- **morph**: vertex morph-target animation (`en3d\morph.cpp`).
- **pscript**: particle script, a text file in `script/pscript/*.txt` (`docs/formats/INVENTORY.md`).

## OpenJoan terms

- **code table**: `joan_code[]`, one slot per original code address; a replacement function is registered into its
  slot (`src/runtime/code_table.cpp`; see [RUNTIME.md](RUNTIME.md)).
- **data image**: `joan_image`, the one object holding the original's `.rdata/.data/.bss/.idata` with a typed layout,
  committed as source (`src/data/README.md`).
- **registration**: `JOAN_REPLACE(name, module, fn::Address, &impl)`, the macro that puts a function into the code
  table; its name (`"Unit::cleanup"`) is the stable ID in logs and the ini.
- **facade**: a generated method on a typed layout (`src/original/gen/`) that calls the game function through the code
  table.
