// SPDX-License-Identifier: GPL-3.0-or-later
// Walking around other units (docs/PATHFINDING.md): the game's side of enpath/navsteer.hpp.
#pragma once

#include <cstdint>

namespace joan::unit_steer {

/// Unit::move_update (0x0052daa0), follow_path: where the unit should head for instead of its waypoint when another unit
/// stands in its way. (`x`, `y`) is the unit's position and (`aim_x`, `aim_y`) comes in as the waypoint, world units; the
/// aim is changed only if the unit has to go around something.
void aim(void* unit, float x, float y, float& aim_x, float& aim_y);

/// Unit::move_update (0x0052daa0): the unit has been pressed against `body` (a physics record id, Shot::hit_obj) for half a
/// second. If `body` is a unit not hostile to it, the unit may now walk through friendly units until it is clear of them,
/// and true is returned; otherwise nothing changes and the caller does what the original does.
bool start_passing(void* unit, std::uint32_t body);

/// Whether `unit` is walking through friendly units at the moment.
bool passing(const void* unit);

/// Players::collide (0x0068de40): whether the unit that owns `actor` walks through the unit `other` at the moment: it is
/// passing and `other` is not the player, or the two are already inside each other. Hostile units are never walked through.
bool walks_through(const void* actor, const void* other);

}  // namespace joan::unit_steer
