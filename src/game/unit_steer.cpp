// SPDX-License-Identifier: GPL-3.0-or-later
// Walking around other units (docs/PATHFINDING.md). Everything here comes from game state and game time, and the units are
// visited in record order, so a replay sees the same decisions. The tables are not saved: after a load every unit starts
// without a side chosen and without a known speed, which settles within a moment.
#include "unit_steer.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "../data_names.hpp"
#include "../enpath/navgame.hpp"
#include "../enpath/navsteer.hpp"
#include "../original/direct.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../registry.hpp"

namespace jo = joan::orig;
using joan::hook::global;
using joan::img::obj;

namespace joan::unit_steer {

namespace {

using i32 = std::int32_t;
using u32 = std::uint32_t;

constexpr u32 kSampleMs = 150;        // a unit's speed is measured over this much game time
constexpr u32 kHeadwayMs = 600;       // a unit with somewhere to go that has not moved for this long counts as standing
constexpr double kMovedUnits = 4.0;   // moving at all: at least this far per sample
constexpr double kNearby = 400.0;     // units further than this are not looked at
constexpr u32 kKeepSideMs = 1500;     // a side chosen is kept this long after the unit in the way was last seen
constexpr u32 kRecheckMs = 250;       // how often a chosen side is checked against the level
constexpr u32 kNoProgressMs = 450;    // no closer to the waypoint for this long: something the level does not know is in the way
constexpr double kProgress = 8.0;     // closer by at least this much is progress
constexpr u32 kAsideMs = 350;         // a step aside lasts this long,
constexpr double kAsideBy = 96.0;     // aims this far to the side,
constexpr double kAsideOn = 32.0;     // and this far on
constexpr std::int32_t kMostAsides = 2;  // and is tried this many times before the unit counts as stuck (game/unit_stuck.hpp)
constexpr u32 kPassMs = 1200;         // how long a unit in a jam may walk through friendly units, at least
constexpr double kInsideShare = 0.7;  // of the two radii together: closer than this, two bodies are inside each other
constexpr u32 kStillOverlapMs = 250;  // and this much longer each time it is still inside one

struct Sample {
    const void* record = nullptr;
    float x = 0.0f, y = 0.0f;
    u32 time = 0;
    bool moving = false;
    float dir_x = 0.0f, dir_y = 0.0f;
    u32 last_moved = 0;   // when it was last seen to move
};

struct Known {
    const void* record = nullptr;
    const void* cell = nullptr;
    std::int16_t layer = 0;
    nav::Neighbour neighbour;
};

struct Side {
    std::int16_t blocker = 0;
    std::int32_t side = 0;
    u32 seen = 0;
    u32 checked = 0;
    u32 passing_until = 0;
    float waypoint_x = 0.0f, waypoint_y = 0.0f;  // the waypoint progress is measured against
    double best = 0.0;                           // the closest the unit came to it
    u32 best_at = 0;                             // when
    u32 last_aim = 0;                            // the last time the unit asked
    u32 aside_until = 0;                         // stepping aside until then
    std::int32_t aside_side = 0;                 // +1 left, -1 right
    std::int32_t attempts = 0;                   // steps aside since the last progress
};

std::vector<Sample> g_samples;   // by unit id
std::vector<Known> g_units;      // the units this tick
u32 g_units_time = 0xffffffffu;
std::unordered_map<const void*, Side> g_sides;  // looked up only

const std::uint8_t* type_of(const void* unit) {
    return joan::orig::arg::as<const std::uint8_t*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
}

double body_radius(const std::uint8_t* type) {
    const double body = static_cast<double>(global<i32>(d::g_unit_body_r));
    switch (jo::view<i32>(jo::as_unit_type(type)->shape_kind)) {
        case 1: return static_cast<double>(jo::as_unit_type(type)->shape_sphere[3]);
        case 3: return static_cast<double>(jo::as_unit_type(type)->shape_cylinder[3]);
        case 2: {
            double largest = 0.0;
            for (const int i : {0, 1, 3, 4}) largest = std::max(largest, std::fabs(static_cast<double>(jo::as_unit_type(type)->shape_box[i])));
            return largest;
        }
        default: return body;
    }
}

bool hostile(const void* a, const void* b) {
    return joan::orig::arg::as<i32>(joan::orig::as_players(obj(d::g_nations))->hostile(static_cast<i32>(jo::as_unit(a)->side), joan::orig::arg::i32(static_cast<i32>(jo::as_unit(b)->side)))) != 0;  // 0x004b6860
}

bool is_player(const void* unit) { return static_cast<i32>(jo::as_unit(unit)->id) == global<i32>(d::g_player_unit_recno); }

double actor_radius(const void* actor, const void* unit) {
    switch (jo::view<i32>(jo::as_actor(actor)->aim_mode)) {
        case 1: return static_cast<double>(jo::as_actor(actor)->aim_sphere[3]);
        case 3: return static_cast<double>(jo::as_actor(actor)->collide_radius);
        default: return body_radius(type_of(unit));
    }
}

// Clearly inside each other, not merely touching: bodies that touch are as far apart as the sum of their radii.
bool overlapping(const void* actor, const void* owner, const void* other) {
    const void* const other_actor = jo::as_unit(other)->actor;
    if (other_actor == nullptr) return false;
    const float* const a = jo::view<float[3]>(jo::as_unit(owner)->position_);
    const float* const b = jo::view<float[3]>(jo::as_unit(other)->position_);
    const double dx = static_cast<double>(a[0] - b[0]), dy = static_cast<double>(a[1] - b[1]);
    const double limit = (actor_radius(actor, owner) + actor_radius(other_actor, other)) * kInsideShare;
    return dx * dx + dy * dy < limit * limit;
}

void refresh_units(u32 now) {
    if (g_units_time == now) return;
    g_units_time = now;
    g_units.clear();
    void* const list = obj(d::g_units);
    const i32 last = joan::orig::as_dynarray(list)->size();
    if (static_cast<std::size_t>(last) + 1 > g_samples.size()) g_samples.resize(static_cast<std::size_t>(last) + 1);
    for (i32 i = 1; i <= last; ++i) {
        if (joan::orig::as_dynarray(list)->empty_at(joan::orig::arg::i32(i)) != 0) continue;
        auto* const record = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)));
        jo::Unit* const unit = jo::as_unit(record);
        if (unit->actor == nullptr || unit->cell == nullptr) continue;
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(record)->dead()) != 0) continue;   // 0x0042d0e0
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(record)->mounted()) != 0) continue;  // 0x00430190: the horse is what stands there
        float position[3] = {0.0f, 0.0f, 0.0f};
        joan::orig::as_unit(record)->position(joan::orig::arg::mptr(position));  // 0x004fedb0

        Sample& sample = g_samples[static_cast<std::size_t>(i)];
        if (sample.record != record) {
            sample = Sample{record, position[0], position[1], now, false, 0.0f, 0.0f, now};
        } else if (now - sample.time >= kSampleMs) {
            const double dx = static_cast<double>(position[0] - sample.x), dy = static_cast<double>(position[1] - sample.y);
            const double moved = std::sqrt(dx * dx + dy * dy);
            sample.moving = moved >= kMovedUnits;
            if (sample.moving) {
                sample.last_moved = now;
                sample.dir_x = static_cast<float>(dx / moved);
                sample.dir_y = static_cast<float>(dy / moved);
            }
            sample.x = position[0];
            sample.y = position[1];
            sample.time = now;
        }

        Known known;
        known.record = record;
        known.cell = unit->cell;
        known.layer = unit->layer;
        known.neighbour.at = {static_cast<double>(position[0]), static_cast<double>(position[1])};
        known.neighbour.radius = body_radius(type_of(record));
        known.neighbour.moving = sample.moving;
        known.neighbour.heading = {static_cast<double>(sample.dir_x), static_cast<double>(sample.dir_y)};
        // Where it means to go counts as well as what it did: speed is only known after a moment, and a whole formation
        // setting off would otherwise look like a crowd of standing units. One that has made no headway for a while is standing.
        const float* intent = nullptr;
        if (unit->scripted_move != 0) {
            intent = jo::view<float[3]>(unit->move_target);
        } else if (unit->path_index < joan::orig::arg::as<i32>(joan::orig::as_list(&unit->path)->count_66e8a0())) {  // 0x0066e8a0
            intent = joan::orig::arg::as<const float*>(joan::orig::as_list(&unit->path)->from_end(joan::orig::arg::i32(unit->path_index + 1)));  // 0x0066e8d0
        }
        if (intent != nullptr && now - sample.last_moved < kHeadwayMs) {
            const double dx = static_cast<double>(intent[0] - position[0]), dy = static_cast<double>(intent[1] - position[1]);
            const double to_go = std::sqrt(dx * dx + dy * dy);
            if (to_go > 0.0) {
                known.neighbour.moving = true;
                known.neighbour.heading = {dx / to_go, dy / to_go};
            }
        }
        known.neighbour.id = unit->id;
        g_units.push_back(known);
    }
}

}  // namespace

void aim(void* unit_record, float x, float y, float& aim_x, float& aim_y) {
    const u32 now = global<u32>(d::g_now);
    refresh_units(now);
    jo::Unit* const unit = jo::as_unit(unit_record);
    if (unit->cell == nullptr || unit->actor == nullptr) return;
    const auto* const type = type_of(unit_record);
    if (jo::as_unit_type(type)->unit_class == 8) return;                                  // siege engines and the like go straight
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit_record)->mounted()) != 0) return;  // 0x00430190

    static std::vector<nav::Neighbour> nearby;
    nearby.clear();
    const nav::Point me{static_cast<double>(x), static_cast<double>(y)};
    for (const Known& k : g_units) {
        if (k.record == unit_record || k.cell != unit->cell || k.layer != unit->layer) continue;
        const double dx = k.neighbour.at.x - me.x, dy = k.neighbour.at.y - me.y;
        if (dx * dx + dy * dy <= kNearby * kNearby) nearby.push_back(k.neighbour);
    }
    Side& side = g_sides[unit_record];
    if (side.passing_until > now) {  // walking through: straight on, for as long as it is inside someone
        const double own = body_radius(type);
        for (const nav::Neighbour& n : nearby) {
            const double dx = n.at.x - me.x, dy = n.at.y - me.y, touching = own + n.radius;
            if (dx * dx + dy * dy < touching * touching) {
                side.passing_until = std::max(side.passing_until, now + kStillOverlapMs);
                break;
            }
        }
        return;
    }

    // Running against something the level's mesh does not know, a wall of a model for instance: step aside, one way then the other.
    void* const space = joan::orig::arg::as<void*>(joan::orig::as_world_cell(unit->cell)->grid(joan::orig::arg::i16(static_cast<i32>(unit->layer))));  // 0x005600c0
    if (space == nullptr || jo::as_path_space(space)->zones == nullptr) return;
    const double origin_x = static_cast<double>(jo::as_cell(unit->cell)->origin_x), origin_y = static_cast<double>(jo::as_cell(unit->cell)->origin_y);
    const auto walkable = [&](nav::Point p) { return nav::walkable(space, p.x - origin_x, p.y - origin_y); };
    const double to_x = static_cast<double>(aim_x) - me.x, to_y = static_cast<double>(aim_y) - me.y, to_waypoint = std::sqrt(to_x * to_x + to_y * to_y);
    if (now - side.last_aim > kNoProgressMs || std::fabs(aim_x - side.waypoint_x) > 1.0f || std::fabs(aim_y - side.waypoint_y) > 1.0f) {
        side.waypoint_x = aim_x;
        side.waypoint_y = aim_y;
        side.best = to_waypoint;
        side.best_at = now;
        side.aside_until = 0;
        side.attempts = 0;
    }
    side.last_aim = now;
    if (to_waypoint < side.best - kProgress) {
        side.best = to_waypoint;
        side.best_at = now;
        side.attempts = 0;
    } else if (now - side.best_at >= kNoProgressMs && now >= side.aside_until && side.attempts < kMostAsides) {
        side.aside_side = (((unit->id & 1) != 0) == ((side.attempts & 1) != 0)) ? 1 : -1;
        side.attempts += 1;
        side.aside_until = now + kAsideMs;
        side.best_at = now;
    }
    if (now < side.aside_until && to_waypoint > 0.0) {
        const double dir_x = to_x / to_waypoint, dir_y = to_y / to_waypoint;
        const double sideways = static_cast<double>(side.aside_side) * kAsideBy;
        const nav::Point target{me.x - dir_y * sideways + dir_x * kAsideOn, me.y + dir_x * sideways + dir_y * kAsideOn};
        if (walkable(target)) {
            aim_x = static_cast<float>(target.x);
            aim_y = static_cast<float>(target.y);
            return;
        }
        side.aside_until = now;
    }

    const bool same_recent = now - side.seen <= kKeepSideMs;
    const nav::Dodge dodge = nav::plan_dodge(me, body_radius(type), {static_cast<double>(aim_x), static_cast<double>(aim_y)}, nearby.data(), nearby.size(), same_recent ? side.side : 0);
    if (!dodge.needed) return;

    // A side that runs off the level is no way round. The check is against the mesh, so it is made a few times a second.
    const bool keep = same_recent && side.blocker == dodge.blocker && side.side == dodge.first_side && now - side.checked < kRecheckMs;
    nav::Point target = dodge.first;
    std::int32_t chosen = dodge.first_side;
    if (!keep) {
        if (!walkable(dodge.first) || !walkable(dodge.first_room)) {
            if (!walkable(dodge.second) || !walkable(dodge.second_room)) return;
            target = dodge.second;
            chosen = -dodge.first_side;
        }
        side.checked = now;
    }
    side.blocker = dodge.blocker;
    side.side = chosen;
    side.seen = now;
    aim_x = static_cast<float>(target.x);
    aim_y = static_cast<float>(target.y);
}

bool start_passing(void* unit_record, std::uint32_t body) {
    void* const bodies = obj(d::g_physic_sys);
    if (body == 0 || joan::orig::as_players(bodies)->list.empty_at(joan::orig::arg::i32(body)) != 0) return false;
    auto* const shot = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(bodies)->list.ptr_at(joan::orig::arg::i32(body)));  // 0x006af310
    const auto* const actor = jo::view<std::uint8_t*>(jo::as_shot(shot)->actor);
    if (actor == nullptr || jo::as_actor(actor)->kind != 0xb) return false;
    if (hostile(unit_record, jo::as_actor(actor)->owner) || is_player(jo::as_actor(actor)->owner)) return false;
    g_sides[unit_record].passing_until = global<u32>(d::g_now) + kPassMs;
    return true;
}

bool passing(const void* unit_record) {
    const auto it = g_sides.find(unit_record);
    return it != g_sides.end() && it->second.passing_until > global<u32>(d::g_now);
}

bool busy(const void* unit_record) {
    const auto it = g_sides.find(unit_record);
    if (it == g_sides.end()) return false;
    const u32 now = global<u32>(d::g_now);
    return it->second.passing_until > now || it->second.aside_until > now;
}

bool walks_through(const void* actor, const void* other) {
    if (jo::as_actor(actor)->kind != 0xb) return false;
    const void* const owner = jo::as_actor(actor)->owner;
    if (owner == other) return false;
    const bool inside = overlapping(actor, owner, other);
    if (!inside && !passing(owner)) return false;
    if (hostile(owner, other)) return false;
    return inside || !is_player(other);  // two bodies already inside each other must be able to part; the player is never walked through
}

}  // namespace joan::unit_steer
