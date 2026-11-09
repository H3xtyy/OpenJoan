// SPDX-License-Identifier: GPL-3.0-or-later
// docs/PATHFINDING.md.
#include "navgame.hpp"

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../original/direct.hpp"
#include "../original/layout_access.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "navsearch.hpp"

namespace jo = joan::orig;

namespace joan::nav {

namespace {

using i32 = std::int32_t;

constexpr double kNarrowUnit = 200.0;           // units at least this wide keep out of polygons marked narrow (0x0066ce30)
constexpr std::uint32_t kBudget = 60000;        // search nodes: an unreachable goal costs a level's worth of them
constexpr double kLaneScale = 2.5;              // the game's formation numbers to world units (0x0066ece0)
constexpr double kSamePoint = 0.5;              // route points closer than this are one point (0x0066ece0)
constexpr double kWideGap = 400.0;              // a unit outside a formation has no need of its lane in a gap at least this wide
constexpr double kStartCell = 16.0;             // a search starts from the middle of a square of this size around the unit
constexpr std::size_t kMaxRemembered = 8192;    // routes remembered before the memory is cleared

/// The mesh of one PathSpace.
struct GameMesh {
    NavMesh mesh;
    std::uint64_t generation = 0;         // counts the times a polygon opened or closed
    std::vector<jo::PfdZone*> zones;      // per polygon: its zone, for the current blocked state
    std::vector<std::int32_t> xyz;        // per vertex: the original integers, three each
    const void* zones_array = nullptr;    // what the mesh was built from: rebuilt if that changes
};

std::unordered_map<const void*, std::unique_ptr<GameMesh>> g_meshes;  // looked up only, never walked
NavSearch g_search;

JOAN_COUNTER(c_repeated, "Nav::route_repeated");
JOAN_COUNTER(c_searched, "Nav::route_searched");

/// What makes two searches the same search.
struct SearchKey {
    const void* owner = nullptr;      // whose mesh (the PathSpace)
    std::uint64_t generation = 0;     // which state of its blocked polygons
    std::int32_t start_poly = -1;
    double from_x = 0.0, from_y = 0.0, to_x = 0.0, to_y = 0.0, radius = 0.0;
    friend bool operator==(const SearchKey&, const SearchKey&) = default;
};

struct KeyHash {
    std::size_t operator()(const SearchKey& key) const {
        std::uint64_t h = 1469598103934665603ull;
        const auto add = [&](std::uint64_t v) { h = (h ^ v) * 1099511628211ull; };
        add(std::bit_cast<std::uintptr_t>(key.owner));
        add(key.generation);
        add(static_cast<std::uint32_t>(key.start_poly));
        for (const double v : {key.from_x, key.from_y, key.to_x, key.to_y, key.radius}) add(std::bit_cast<std::uint64_t>(v));
        return static_cast<std::size_t>(h);
    }
};
// A route depends only on its key, so this memory changes when work is done, never what comes out. Looked up only.
std::unordered_map<SearchKey, Route, KeyHash> g_remembered;

double to_cell(double v) { return std::floor(v / kStartCell) * kStartCell + kStartCell / 2.0; }

i32 zone_count(const jo::PathSpace* space) {
    return joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::view<void*>(space->zones))->get_2c());  // 0x0066b7a0
}

jo::PfdZone* zone_at(const jo::PathSpace* space, i32 index) {
    return jo::as_pfd_zone(joan::orig::as_billboard(jo::view<void*>(space->zones))->item(joan::orig::arg::i32(index)));  // 0x00637da0
}

std::unique_ptr<GameMesh> build_mesh(const jo::PathSpace* space) {
    auto built = std::make_unique<GameMesh>();
    built->zones_array = jo::view<void*>(space->zones);
    NavMesh& mesh = built->mesh;
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
    for (std::size_t i = 0; i < game.zones.size(); ++i) {
        const i32 state = game.zones[i]->blocked;
        Poly& poly = game.mesh.polys[i];
        if (poly.blocked == (state == 1) && poly.narrow == (state == -1)) continue;
        poly.blocked = state == 1;
        poly.narrow = state == -1;
        game.generation += 1;
    }
}

// Where the unit crosses: its place in the formation, kept inside the usable part of the edge. This is what makes the units
// of one order take different lines through a gap; the numbers are those of the original's smoothing (0x0066ece0).
Point in_lane(const Crossing& crossing, double lane, double width, bool tight_only) {
    const double dx = crossing.right.x - crossing.left.x, dy = crossing.right.y - crossing.left.y;
    const double usable = std::sqrt(dx * dx + dy * dy);
    if (usable == 0.0 || width == 0.0) return crossing.at;
    const double along = ((crossing.at.x - crossing.left.x) * dx + (crossing.at.y - crossing.left.y) * dy) / (usable * usable);
    if (tight_only && along > 0.0 && along < 1.0 && usable >= kWideGap) return crossing.at;  // open ground: the straight line
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

// A unit asks again every few frames while it walks, from a slightly different place. Starting every search from the middle
// of a small square makes those the same search; the walk to the first edge starts at the unit, inside the same polygon.
Request make_query(std::int32_t zone, Point from, Point to, double radius) {
    Request query;
    query.start_poly = zone - 1;
    query.from = {to_cell(from.x), to_cell(from.y)};
    query.to = to;
    query.radius = radius;
    query.avoid_narrow = radius >= kNarrowUnit;
    query.budget = kBudget;
    return query;
}

const Route& route_for(void* path_space, const GameMesh& game, const Request& query) {
    SearchKey key;
    key.owner = path_space;
    key.generation = game.generation;
    key.start_poly = query.start_poly;
    key.from_x = query.from.x, key.from_y = query.from.y;
    key.to_x = query.to.x, key.to_y = query.to.y;
    key.radius = query.radius;
    const auto known = g_remembered.find(key);
    if (known != g_remembered.end()) {
        c_repeated.hit();
        return known->second;
    }
    c_searched.hit();
    if (g_remembered.size() >= kMaxRemembered) g_remembered.clear();
    return g_remembered.try_emplace(key, g_search.find(game.mesh, query)).first->second;
}

// 0x0066cb40 PathSpace::find_path, from the point where the start polygon is known.
std::int32_t find_route(void* path_space, const GameRequest& request, void* route) {
    using joan::hook::x87::ftol;
    const auto* const space = jo::as_path_space(path_space);
    GameMesh& game = mesh_of(space);
    refresh_blocked(game);

    const Point from{static_cast<double>(request.x), static_cast<double>(request.y)};
    const Point to{static_cast<double>(request.tx), static_cast<double>(request.ty)};
    const Request query = make_query(request.zone, from, to, static_cast<double>(request.radius));
    const Route& found = route_for(path_space, game, query);
    if (found.outcome == Outcome::no_start) return 0;

    const double lane = static_cast<double>(request.lane) * kLaneScale;
    const double width = std::fabs(static_cast<double>(request.lane_width)) * kLaneScale;
    const bool tight_only = request.lane_width < 0.0f;
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
        const Point at = in_lane(crossing, lane, width, tight_only);
        if (same_point(at, next) || same_point(at, from)) continue;
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

bool walkable(void* path_space, double x, double y) {
    GameMesh& game = mesh_of(jo::as_path_space(path_space));
    refresh_blocked(game);
    return game.mesh.locate({x, y}) >= 0;
}

void forget_mesh(const void* path_space) {
    g_meshes.erase(path_space);
    g_remembered.clear();
}

}  // namespace joan::nav
