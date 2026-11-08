# Adding a mission

With `[Fixes] MissionCount=1` (the default; `src/game/mission_count.hpp`) the campaign ends after the last mission the
game folder holds instead of after mission 8. This page describes what a mission is made of and what a ninth mission
gets from the engine. Work in a copy of the game folder, never in the original install.

## What a mission is

Mission N is the file `scenario\scenario_NN.scr` (two digits: `scenario_09.scr`, `scenario_10.scr`; format in
[formats/scr.md](../formats/scr.md)). It names six files:

| Key | Example (mission 6) | Used for |
|---|---|---|
| `WorldFileName` | `scenario5\S5L1w` | the world under `map\` |
| `UnitFileName` | `scenario5\S5L1.uni` | the units placed on it ([formats/uni.md](../formats/uni.md)) |
| `ScriptFileName` | `S9L1` | the main script `script\scenario\S9L1.sce` (and its `S9L1EX.res`, when present) |
| `DescriptionFileName` | `S9L1_des` | the script run once on load (`_des.sce`) |
| `ObjectiveFileName` | `S9L1_obj` | the objectives text (`script\scenario\S9L1_obj.obj`) |
| `ObjectiveMapFileName` | `S5L1_map` | the objectives map picture (`.bm`) |

The internal names do not follow the campaign order: the shipped mission 2 is `S3L1`, mission 3 is `S2L1`, mission 6 is
`S9L1` on mission 5's world, mission 8 is `S6L1`. Mission 6 shows the cheapest way to make a new mission: reuse an
existing world and units with a new script. The script language is [LANGUAGE.md](LANGUAGE.md), its builtins
[BUILTINS.md](BUILTINS.md).

## How the campaign moves on

"Continue" on the results screen loads mission number + 1 (0x00755d80). The objectives screen of the last mission
labels that button with the ending text and opens the ending instead (0x007226a0 and its update, `interfac/objmenu.cpp`).
The last mission is the highest N for which `scenario_01.scr` up to `scenario_NN.scr` all exist, so the numbers must not
have gaps. With the switch off (or in a replay of a recording made without it) the last mission is 8, as in the
original.

## What a mission after 8 gets

| Asset | Missions 1-8 | Mission 9 and later |
|---|---|---|
| Victory title | text table `Resource\t_interf.txr`, entry `VICTS\|N` | the same entry if you add it, otherwise "Yow win!" |
| Debriefing grade (time, kills, level limits) | eight-row tables in the executable (0x009df1a4..) | mission 8's limits |
| Loading screen pictures | `M1`..`M8` | mission 1's |
| Briefing voice | `MISSION_BRIEF_1`..`8` | none |
| Objectives map mask | `S1_SCR`..`S8_SCR` | none |
| Autosave name | `AUTOSAVE\|1` formats the number (`Mission%1d-AutoSave`) | works as is |
| Music | `pick_random(3, 0, N)` over the music table's level ranges | whatever the music table gives for N (none unless a range covers it) |

Missions 5, 6 and 8 have code of their own in the engine (mission 5's world parts on the loading screen and its world
kept on retry, mission 6's skipped world arrays and its own autosave on retry, mission 8's damage scaling); a new
mission gets none of it.
