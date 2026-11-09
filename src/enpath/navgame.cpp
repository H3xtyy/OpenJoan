// SPDX-License-Identifier: GPL-3.0-or-later
// docs/PATHFINDING.md.
#include "navgame.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../original/direct.hpp"
#include "../original/layout_access.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "navpool.hpp"
#include "navsearch.hpp"

namespace jo = joan::orig;

namespace joan::nav {

namespace {

using i32 = std::int32_t;

constexpr double kNarrowUnit = 200.0;           // units at least this wide keep out of polygons marked narrow (0x0066ce30)
constexpr std::uint32_t kBudget = 60000;        // search nodes: an unreachable goal costs a level's worth of them
constexpr double kLaneScale = 2.5;              // the game's formation numbers to world units (0x0066ece0)
constexpr double kSamePoint = 0.5;              // route points closer than this are one point (0x0066ece0)

/// The mesh of one PathSpace. A published mesh never changes: when a polygon opens or closes, the mesh is copied.
struct GameMesh {
    std::shared_ptr<const NavMesh> mesh;
    std::uint64_t generation = 0;         // counts those copies
    std::vector<jo::PfdZone*> zones;      // per polygon: its zone, for the current blocked state
    std::vector<std::int32_t> xyz;        // per vertex: the original integers, three each
    const void* zones_array = nullptr;    // what the mesh was built from: rebuilt if that changes
};

std::unordered_map<const void*, std::unique_ptr<GameMesh>> g_meshes;  // looked up only, never walked
NavSearch g_search;

JOAN_COUNTER(c_prepared, "Nav::route_ready_from_worker");
JOAN_COUNTER(c_searched, "Nav::route_searched_on_request");

// One thread stays with the game, and a few are plenty for a level's routes.
RoutePool& pool() {
    static RoutePool instance(std::clamp(std::thread::hardware_concurrency(), 2u, 5u) - 1u);
    return instance;
}

i32 zone_count(const jo::PathSpace* space) {
    return joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::view<void*>(space->zones))->get_2c());  // 0x0066b7a0
}

jo::PfdZone* zone_at(const jo::PathSpace* space, i32 index) {
    return jo::as_pfd_zone(joan::orig::as_billboard(jo::view<void*>(space->zones))->item(joan::orig::arg::i32(index)));  // 0x00637da0
}

std::unique_ptr<GameMesh> build_mesh(const jo::PathSpace* space) {
    auto built = std::make_unique<GameMesh>();
    built->zones_array = jo::view<void*>(space->zones);
    auto owned = std::make_shared<NavMesh>();
    NavMesh& mesh = *owned;
    const i32 count = zone_count(space);
    std::unordered_map<const void*, std::uint32_t> vertex_of;  // looked up only
    std::vector<std::uint32_t> corners;
    std::vector<std::int32_t> links;
    for (i32 i = 1; i <= count; ++i) {
        jo::PfdZone* const zone = zone_at(space, i);
        built->zones.push_back(zone);
        corners.clear();
        links.clear();
        double area = 0.0;
        for (i32 k = 0; k < zone->point_count; ++k) {
            const auto* const point = zone->points[k].item;
            const auto [it, added] = vertex_of.try_emplace(point, static_cast<std::uint32_t>(mesh.points.size()));
            if (added) {
                mesh.points.push_back({static_cast<double>(point->xyz[0]), static_cast<double>(point->xyz[1])});
                built->xyz.insert(built->xyz.end(), {point->xyz[0], point->xyz[1], point->xyz[2]});
            }
            corners.push_back(it->second);
            const auto* const across = zone->links[k].item;
            links.push_back(across == nullptr ? -1 : across->id - 1);
        }
        for (std::size_t k = 0; k < corners.size(); ++k) {
            const Point a = mesh.points[corners[k]], b = mesh.points[corners[(k + 1) % corners.size()]];
            area += a.x * b.y - b.x * a.y;
        }
        if (area < 0.0) {  // the few clockwise polygons of the shipped levels: the mesh is counter-clockwise
            std::reverse(corners.begin(), corners.end());
            std::reverse(links.begin(), links.end());
            std::rotate(links.begin(), links.begin() + 1, links.end());
        }
        Poly poly;
        poly.first = static_cast<std::uint32_t>(mesh.corners.size());
        poly.count = static_cast<std::uint32_t>(corners.size());
        poly.weight = jo::view<std::int16_t>(zone->flags);
        mesh.polys.push_back(poly);
        mesh.corners.insert(mesh.corners.end(), corners.begin(), corners.end());
        mesh.links.insert(mesh.links.end(), links.begin(), links.end());
    }
    built->mesh = std::move(owned);
    return built;
}

GameMesh& mesh_of(const jo::PathSpace* space) {
    std::unique_ptr<GameMesh>& entry = g_meshes[space];
    const auto wanted = static_cast<std::size_t>(zone_count(space));
    if (!entry || entry->zones_array != jo::view<void*>(space->zones) || entry->zones.size() != wanted) entry = build_mesh(space);
    return *entry;
}

// Doors and buildings open and close polygons while the game runs (0x006af710).
void refresh_blocked(GameMesh& game) {
    const std::vector<Poly>& polys = game.mesh->polys;
    bool changed = false;
    for (std::size_t i = 0; i < game.zones.size() && !changed; ++i) {
        const i32 state = game.zones[i]->blocked;
        changed = polys[i].blocked != (state == 1) || polys[i].narrow != (state == -1);
    }
    if (!changed) return;
    auto copy = std::make_shared<NavMesh>(*game.mesh);
    for (std::size_t i = 0; i < game.zones.size(); ++i) {
        const i32 state = game.zones[i]->blocked;
        copy->polys[i].blocked = state == 1;
        copy->polys[i].narrow = state == -1;
    }
    game.mesh = std::move(copy);
    ++game.generation;
}

// Where the unit crosses: its place in the formation, kept inside the usable part of the edge. This is what makes the units
// of one order take different lines through a gap; the numbers are those of the original's smoothing (0x0066ece0).
Point in_lane(const Crossing& crossing, double lane, double width) {
    const double dx = crossing.right.x - crossing.left.x, dy = crossing.right.y - crossing.left.y;
    const double usable = std::sqrt(dx * dx + dy * dy);
    if (usable == 0.0 || width == 0.0) return crossing.at;
    const double along = ((crossing.at.x - crossing.left.x) * dx + (crossing.at.y - crossing.left.y) * dy) / (usable * usable);
    const double place = std::clamp(-lane / width + 0.5, 0.0, 1.0);
    double s = 0.0;
    if (usable < width) {
        s = place;  // the formation squeezes into the gap
    } else if (along <= 0.0) {
        s = place * width / usable;  // against the left end
    } else if (along >= 1.0) {
        s = 1.0 - (1.0 - place) * width / usable;  // against the right end
    } else {
        s = along + std::clamp(-lane / width, -0.5, 0.5) * width / usable;
    }
    s = std::clamp(s, 0.0, 1.0);
    return {crossing.left.x + dx * s, crossing.left.y + dy * s};
}

bool same_point(Point a, Point b) { return std::fabs(a.x - b.x) < kSamePoint && std::fabs(a.y - b.y) < kSamePoint; }

}  // namespace

// 0x0066cb40 PathSpace::find_path, from the point where the start polygon is known.
std::int32_t find_route(void* path_space, const GameRequest& request, void* route) {
    using joan::hook::x87::ftol;
    const auto* const space = jo::as_path_space(path_space);
    GameMesh& game = mesh_of(space);
    refresh_blocked(game);

    Request query;
    query.start_poly = request.zone - 1;
    query.from = {static_cast<double>(request.x), static_cast<double>(request.y)};
    query.to = {static_cast<double>(request.tx), static_cast<double>(request.ty)};
    query.radius = static_cast<double>(request.radius);
    query.avoid_narrow = query.radius >= kNarrowUnit;
    query.budget = kBudget;
    std::optional<Route> ready = pool().take(key_of(path_space, game.generation, query));
    if (ready) {
        c_prepared.hit();
    } else {
        c_searched.hit();
        ready = g_search.find(*game.mesh, query);
    }
    const Route& found = *ready;
    if (found.outcome == Outcome::no_start) return 0;

    const double lane = static_cast<double>(request.lane) * kLaneScale;
    const double width = static_cast<double>(request.lane_width) * kLaneScale;
    const auto height = [&](const Crossing& c, Point at) {
        const std::int32_t* const l = &game.xyz[static_cast<std::size_t>(c.left_corner) * 3];
        const std::int32_t* const r = &game.xyz[static_cast<std::size_t>(c.right_corner) * 3];
        const double dx = static_cast<double>(r[0]) - l[0], dy = static_cast<double>(r[1]) - l[1], squared = dx * dx + dy * dy;
        const double t = squared == 0.0 ? 0.0 : std::clamp(((at.x - l[0]) * dx + (at.y - l[1]) * dy) / squared, 0.0, 1.0);
        return static_cast<double>(l[2]) + (static_cast<double>(r[2]) - l[2]) * t;
    };

    const Point end = found.points.back();
    if (found.outcome == Outcome::partial) {
        const double z = found.crossings.empty() ? static_cast<double>(request.z) : height(found.crossings.back(), found.crossings.back().at);
        joan::orig::as_path_box(route)->set_end(joan::orig::arg::i32(ftol(static_cast<float>(end.x))), joan::orig::arg::i32(ftol(static_cast<float>(end.y))),
                                                joan::orig::arg::i32(ftol(static_cast<float>(z))));  // 0x0066e720
    } else {
        joan::orig::as_path_box(route)->set_end(joan::orig::arg::i32(ftol(request.tx)), joan::orig::arg::i32(ftol(request.ty)), joan::orig::arg::i32(ftol(request.tz)));  // 0x0066e720
    }

    // The list holds the route backwards: the goal first, the crossing next to the unit last (0x0066e8d0).
    Point next = end;
    for (std::size_t i = found.crossings.size(); i-- > 0;) {
        const Crossing& crossing = found.crossings[i];
        const Point at = in_lane(crossing, lane, width);
        if (same_point(at, next) || same_point(at, query.from)) continue;
        next = at;
        const std::int32_t* const l = &game.xyz[static_cast<std::size_t>(crossing.left_corner) * 3];
        const std::int32_t* const r = &game.xyz[static_cast<std::size_t>(crossing.right_corner) * 3];
        joan::orig::as_path(route)->add_segment(joan::orig::arg::i32(l[0]), joan::orig::arg::i32(l[1]), joan::orig::arg::i32(l[2]), joan::orig::arg::i32(r[0]), joan::orig::arg::i32(r[1]), joan::orig::arg::i32(r[2]));  // 0x0066e790
        jo::PathPoint* const point = jo::as_path_point(joan::orig::as_list(route)->from_end(joan::orig::arg::i32(1)));  // 0x0066e8d0
        point->pos[0] = static_cast<float>(at.x);
        point->pos[1] = static_cast<float>(at.y);
        point->pos[2] = static_cast<float>(height(crossing, at));
    }
    if (found.outcome != Outcome::reached) return 0;
    return static_cast<double>(ftol(request.tx)) == static_cast<double>(request.tx) && static_cast<double>(ftol(request.ty)) == static_cast<double>(request.ty) ? 1 : 0;
}

void prepare_route(void* path_space, std::int32_t zone, float x, float y, float to_x, float to_y, float radius) {
    GameMesh& game = mesh_of(jo::as_path_space(path_space));
    refresh_blocked(game);
    Request query;
    query.start_poly = zone - 1;
    query.from = {static_cast<double>(x), static_cast<double>(y)};
    query.to = {static_cast<double>(to_x), static_cast<double>(to_y)};
    query.radius = static_cast<double>(radius);
    query.avoid_narrow = query.radius >= kNarrowUnit;
    query.budget = kBudget;
    pool().submit(key_of(path_space, game.generation, query), game.mesh, query);
}

bool walkable(void* path_space, double x, double y) {
    GameMesh& game = mesh_of(jo::as_path_space(path_space));
    refresh_blocked(game);
    return game.mesh->locate({x, y}) >= 0;
}

void forget_mesh(const void* path_space) {
    g_meshes.erase(path_space);
    pool().forget(path_space);
}

}  // namespace joan::nav
