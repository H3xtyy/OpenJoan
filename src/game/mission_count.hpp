// SPDX-License-Identifier: GPL-3.0-or-later
// [Fixes] MissionCount: the campaign ends after the last mission the data holds, not after mission 8.
//
// A mission is data: SCENARIO\scenario_NN.scr names its world, unit file and scripts (Scenario::load 0x004ccfb0 builds
// the name for any number), and "continue" on the results screen loads number + 1 without a bound (0x00755d80). The
// original nevertheless ends the campaign at 8 in two places: the objectives screen labels the continue button "end"
// for mission 8 (CampScreen::open 0x007226a0) and opens the ending instead of the next mission (CampScreen::update,
// the continue branch, 0x00722950 family in interfac/objmenu.cpp). Tables in the executable that hold one value per
// mission (the debriefing grade limits at 0x009df1a4..0x009df264) have eight rows, and the victory title is the
// VICTS text of the mission's number.
//
// With the fix the last mission is the highest N for which scenario_01.scr .. scenario_NN.scr all exist, so a ninth
// mission is played by adding scenario_09.scr (and its scripts and maps). Missions past 8 grade with mission 8's
// limits, and a mission without a VICTS text shows the victory title of the debug scenario ("Yow win!"). The other
// per-mission assets keep the original's fallbacks: the loading screen shows mission 1's pictures (Seq::draw 0x00756430),
// the briefing has no voice (0x00724bf0), the objectives screen no map mask (0x0071efc0). With the shipped data the last
// mission is 8, so the game plays as before.
//
// The scan uses the game's own file check (Misc::is_file_exist 0x00590b80), which the replay records; it runs once,
// on the first question, and only while the switch is on.
#pragma once

#include <cstdint>

namespace joan::mission_count {

/// The number of the campaign's last mission: 8 with the switch off (the original's constant); with it on, the highest
/// N such that SCENARIO\scenario_01.scr .. scenario_NN.scr exist (at most 99; 8 when not even scenario_01 exists).
std::int32_t last_mission();

/// The row of an eight-row per-mission table of the executable for mission `mission`: the mission itself, or 8 for a
/// mission past 8 while the switch is on.
std::uint32_t table_mission(std::uint32_t mission);

/// Whether the text table `strings` (the game's interface texts, t_interf.txr) has the VICTS entry the victory screen
/// (0x006e7900) shows for `mission`. Always true with the switch off (the original reads the entry unchecked).
bool has_victory_text(void* strings, std::int32_t mission);

}  // namespace joan::mission_count
