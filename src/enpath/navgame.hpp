// SPDX-License-Identifier: GPL-3.0-or-later
// Connects the route search (navsearch.hpp, docs/PATHFINDING.md) to the game: builds a NavMesh from a PathSpace and writes
// the route into the game's route list.
#pragma once

#include <cstdint>

namespace joan::nav {

/// The arguments of a route request, as the game passes them to PathSpace::find_path (0x0066cb40).
struct GameRequest {
    float x = 0.0f, y = 0.0f, z = 0.0f;     ///< start, world units of this path space
    float tx = 0.0f, ty = 0.0f, tz = 0.0f;  ///< goal
    float radius = 0.0f;                    ///< clearance from walls, world units
    float lane = 0.0f;                      ///< the unit's place across its formation, world units (0 = centre)
    float lane_width = 0.0f;                ///< the formation's width, world units (0 = no formation)
    std::int32_t zone = 0;                  ///< the 1-based PfdZone the unit stands in
};

/// Searches `path_space` for `request` and fills `route` (a PathRoute whose start and goal PathBox::set already wrote).
/// Returns 1 when the route ends at the exact goal, else 0, as PathSpace::find_path does.
/// @param path_space the PathSpace; its mesh is built on the first request and stays until forget_mesh.
std::int32_t find_route(void* path_space, const GameRequest& request, void* route);

/// Drops the mesh built for `path_space`. Called when the PathSpace is destroyed.
void forget_mesh(const void* path_space);

}  // namespace joan::nav
