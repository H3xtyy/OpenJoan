// SPDX-License-Identifier: GPL-3.0-or-later
// Walking around other units (docs/PATHFINDING.md): the game's side of enpath/navsteer.hpp.
#pragma once

namespace joan::unit_steer {

/// Unit::move_update (0x0052daa0), follow_path: where the unit should head for instead of its waypoint when another unit
/// stands in its way. (`x`, `y`) is the unit's position and (`aim_x`, `aim_y`) comes in as the waypoint, world units; the
/// aim is changed only if the unit has to go around something.
void aim(void* unit, float x, float y, float& aim_x, float& aim_y);

}  // namespace joan::unit_steer
