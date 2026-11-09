// SPDX-License-Identifier: GPL-3.0-or-later
// Walking around other units (docs/PATHFINDING.md): the route search knows only the level, so a unit standing on a route
// has to be dealt with while walking. This part is plain geometry; game/unit_steer.cpp supplies the units.
#pragma once

#include <cstddef>
#include <cstdint>

#include "navsearch.hpp"

namespace joan::nav {

/// Another unit near the one that is walking.
struct Neighbour {
    Point at;              ///< its position, world units
    double radius = 0.0;   ///< its body radius, world units
    bool moving = false;   ///< it has been moving lately
    Point heading;         ///< its direction of travel (unit length) when moving
    std::int16_t id = 0;   ///< its unit id
};

/// How to get past the unit in the way.
struct Dodge {
    bool needed = false;     ///< something is in the way
    Point first, second;     ///< where to aim to pass it: the preferred side, then the other
    Point first_room, second_room;  ///< the far edge of the walking unit's body at those points: must be on the level too
    std::int32_t first_side = 0;  ///< the side of `first`: +1 left of the direction of travel, -1 right
    std::int16_t blocker = 0;     ///< the id of the unit in the way
};

/// The nearest unit between `me` and `waypoint` that would be walked into, and the two ways round it. Units going the same
/// way are left alone, so that a column keeps flowing.
/// @param me the walking unit's position, world units
/// @param radius its body radius, world units
/// @param preferred_side +1 / -1 to keep passing on that side, 0 to choose
Dodge plan_dodge(Point me, double radius, Point waypoint, const Neighbour* neighbours, std::size_t count, std::int32_t preferred_side);

}  // namespace joan::nav
