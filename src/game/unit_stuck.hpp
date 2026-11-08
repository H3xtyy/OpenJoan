// SPDX-License-Identifier: GPL-3.0-or-later
// [Fixes] UnitStuck, part 2: units that push against an obstacle the path data does not know.
//
// The path search (Nav::find_path_66cb40) knows only the navigation mesh. The collision also has the models' own
// shapes - a bridge's parapet, a gate's corner - so a route the mesh calls free can run into a wall: the unit keeps
// its run animation and stays where it is. The original notices a blocked unit only when it bumps into another unit
// (Unit::move_update 0x0052daa0, body bump counter Unit +0x402), never a wall, and a follower's escort order
// (Escort::update_4be770) hands it the same route again when it ends. Recording ref-x86-2026-11-1b, mission 5 at the
// castle gate: units 96 and 99 from frame 7,468 to the end.
//
// With the fix, Unit::move_update asks watch() before it follows the route: a unit that has not come kMinMove closer
// to the waypoint it walks to in kWindowMs of game time stops (as at the end of a route). Distance to the waypoint,
// not distance moved: a unit pushed against a wall wobbles back and forth by about ten units without getting
// anywhere. The escort order then asks gave_up() before it plans: while the unit stands where it stopped and its spot
// is still the one it could not reach, the order counts as arrived instead of sending it against the wall again - for
// kRetryMs, after which it tries once more (what blocked it may have moved: in that recording unit 99 was also in
// unit 96's way, and 96 got through as soon as 99 stopped).
//
// The state is a table of ours keyed by the unit's record (not saved: after a load every unit starts unwatched);
// every value comes from game state and game time, so a replay sees the same decisions.
#pragma once

#include <cstdint>

namespace joan::unit_stuck {

inline constexpr float kMinMove = 8.0f;             // world units a following unit must come closer to its waypoint ...
inline constexpr std::uint32_t kWindowMs = 1000;    // ... within this much game time (ms)
inline constexpr float kSameSpot = 32.0f;           // a spot / position within this of the given-up one is the same
inline constexpr std::uint32_t kRetryMs = 5000;     // a given-up spot is tried again after this much game time (ms)

/// Unit::move_update (0x0052daa0), [Fixes] UnitStuck, just before the unit follows its route: records the unit's
/// progress at position (x, y) (world units) toward waypoint `index` of its route at (wx, wy) at game time `now_ms`.
/// Returns true when it has come less than kMinMove closer to that waypoint in the last kWindowMs: the caller ends the
/// route, and the route's target (tx, ty) is remembered as given up from (x, y). `unit` is the unit record (only its
/// address and `id` are kept).
bool watch(const void* unit, std::int16_t id, float x, float y, std::int32_t index, float wx, float wy, float tx, float ty,
           std::uint32_t now_ms);

/// Escort::update_4be770 (0x004be770), [Fixes] UnitStuck: whether `unit` (at (x, y)) gave up a route to a spot
/// within kSameSpot of (sx, sy) less than kRetryMs before `now_ms` and still stands within kSameSpot of where it gave up.
bool gave_up(const void* unit, std::int16_t id, float x, float y, float sx, float sy, std::uint32_t now_ms);

}  // namespace joan::unit_stuck
