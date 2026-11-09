// SPDX-License-Identifier: GPL-3.0-or-later
// docs/PATHFINDING.md. Floating point here is + - * / and sqrt on doubles only.
#include "navsteer.hpp"

#include <cmath>

namespace joan::nav {

namespace {
constexpr double kMargin = 8.0;           // world units left between two bodies when passing
constexpr double kLookAhead = 1.75;        // how far ahead a unit counts, in units of the distance two bodies keep
constexpr double kSameWay = 0.3;          // a unit whose direction has at least this much in common with ours goes our way
}  // namespace

Dodge plan_dodge(Point me, double radius, Point waypoint, const Neighbour* neighbours, std::size_t count, std::int32_t preferred_side) {
    Dodge dodge;
    const double to_x = waypoint.x - me.x, to_y = waypoint.y - me.y;
    const double to_waypoint = std::sqrt(to_x * to_x + to_y * to_y);
    if (to_waypoint == 0.0) return dodge;
    const double dir_x = to_x / to_waypoint, dir_y = to_y / to_waypoint;

    double nearest = 0.0, nearest_lateral = 0.0, nearest_clear = 0.0;
    const Neighbour* blocker = nullptr;
    for (std::size_t i = 0; i < count; ++i) {
        const Neighbour& n = neighbours[i];
        const double rx = n.at.x - me.x, ry = n.at.y - me.y;
        const double along = rx * dir_x + ry * dir_y;
        const double clear = radius + n.radius + kMargin;
        if (along <= 0.0 || along > clear * kLookAhead || along >= to_waypoint) continue;  // behind, far, or past the waypoint
        const double lateral = dir_x * ry - dir_y * rx;
        if (std::fabs(lateral) >= clear) continue;
        if (n.moving && n.heading.x * dir_x + n.heading.y * dir_y >= kSameWay) continue;
        if (blocker == nullptr || along < nearest) {
            blocker = &n;
            nearest = along;
            nearest_lateral = lateral;
            nearest_clear = clear;
        }
    }
    if (blocker == nullptr) return dodge;

    // Away from the side it stands on, which is the shorter way round; head-on, both units keep to their right.
    const std::int32_t side = preferred_side != 0 ? preferred_side : (nearest_lateral > 0.0 ? -1 : 1);
    const std::int32_t chosen = preferred_side == 0 && std::fabs(nearest_lateral) < 1.0 ? -1 : side;
    const Point left{-dir_y, dir_x};
    const auto beside = [&](std::int32_t s, double distance) {
        return Point{blocker->at.x + left.x * static_cast<double>(s) * distance, blocker->at.y + left.y * static_cast<double>(s) * distance};
    };
    dodge.needed = true;
    dodge.first = beside(chosen, nearest_clear);
    dodge.second = beside(-chosen, nearest_clear);
    dodge.first_room = beside(chosen, nearest_clear + radius);
    dodge.second_room = beside(-chosen, nearest_clear + radius);
    dodge.first_side = chosen;
    dodge.blocker = blocker->id;
    return dodge;
}

}  // namespace joan::nav
