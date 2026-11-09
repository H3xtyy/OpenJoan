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
#include "../x87.hpp"

namespace jo = joan::orig;
using joan::hook::global;
using joan::img::obj;

namespace joan::unit_steer {

namespace {

using i32 = std::int32_t;
using u32 = std::uint32_t;

constexpr u32 kSampleMs = 150;        // a unit's speed is measured over this much game time
constexpr double kMovedUnits = 4.0;   // moving at all: at least this far per sample
constexpr double kNearby = 400.0;     // units further than this are not looked at
constexpr u32 kKeepSideMs = 1500;     // a side chosen is kept this long after the unit in the way was last seen
constexpr u32 kRecheckMs = 250;       // how often a chosen side is checked against the level
constexpr u32 kPassMs = 1200;         // how long a unit in a jam may walk through friendly units, at least
constexpr double kInsideShare = 0.7;  // of the two radii together: closer than this, two bodies are inside each other
constexpr u32 kStillOverlapMs = 250;  // and this much longer each time it is still inside one

struct Sample {
    const void* record = nullptr;
    float x = 0.0f, y = 0.0f;
    u32 time = 0;
    bool moving = false;
    float dir_x = 0.0f, dir_y = 0.0f;
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
            sample = Sample{record, position[0], position[1], now, false, 0.0f, 0.0f};
        } else if (now - sample.time >= kSampleMs) {
            const double dx = static_cast<double>(position[0] - sample.x), dy = static_cast<double>(position[1] - sample.y);
            const double moved = std::sqrt(dx * dx + dy * dy);
            sample.moving = moved >= kMovedUnits;
            if (sample.moving) {
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
    const bool same_recent = now - side.seen <= kKeepSideMs;
    const nav::Dodge dodge = nav::plan_dodge(me, body_radius(type), {static_cast<double>(aim_x), static_cast<double>(aim_y)}, nearby.data(), nearby.size(), same_recent ? side.side : 0);
    if (!dodge.needed) return;

    // A side that runs off the level is no way round. The check is against the mesh, so it is made a few times a second.
    const bool keep = same_recent && side.blocker == dodge.blocker && side.side == dodge.first_side && now - side.checked < kRecheckMs;
    nav::Point target = dodge.first;
    std::int32_t chosen = dodge.first_side;
    if (!keep) {
        void* const space = joan::orig::arg::as<void*>(joan::orig::as_world_cell(unit->cell)->grid(joan::orig::arg::i16(static_cast<i32>(unit->layer))));  // 0x005600c0
        if (space == nullptr || jo::as_path_space(space)->zones == nullptr) return;
        const double origin_x = static_cast<double>(jo::as_cell(unit->cell)->origin_x), origin_y = static_cast<double>(jo::as_cell(unit->cell)->origin_y);
        const auto walkable = [&](nav::Point p) { return nav::walkable(space, p.x - origin_x, p.y - origin_y); };
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

// What Unit::move_to (0x00505dd0) will ask for a unit on foot whose goal is in its own cell: the goal rounded to the coarse
// grid of the world's path space, then the search on the cell's mesh (replan, 0x00506240).
void prepare_route(void* unit_record, float x, float y, float z) {
    jo::Unit* const unit = jo::as_unit(unit_record);
    if (unit->cell == nullptr || unit->actor == nullptr) return;
    if (jo::as_unit_type(type_of(unit_record))->unit_class == 8 || joan::orig::arg::as<i32>(joan::orig::as_unit(unit_record)->mounted()) != 0) return;  // 0x00430190
    void* const cell = unit->cell;
    const float goal[3] = {x, y, z};
    if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(jo::as_cell(cell)->box)->contains(joan::orig::arg::ptr(goal))) == 0 || jo::view<std::int8_t>(jo::as_cell(cell)->loaded) == 0) return;  // 0x0060d5f0
    void* const space = joan::orig::arg::as<void*>(joan::orig::as_world_cell(cell)->grid(joan::orig::arg::i16(static_cast<i32>(unit->layer))));  // 0x005600c0
    if (space == nullptr || jo::as_path_space(space)->zones == nullptr) return;

    float position[3] = {0.0f, 0.0f, 0.0f};
    joan::orig::as_unit(unit_record)->position(joan::orig::arg::mptr(position));  // 0x004fedb0
    const joan::hook::x87::Fpu fpu{true};
    const auto in_cell = [&](float v, i32 origin) { return static_cast<float>(fpu.sub(static_cast<double>(v), static_cast<double>(origin))); };
    const auto coarse = [&](float v) {
        const float cells = static_cast<float>(fpu.div(static_cast<double>(v), static_cast<double>(global<float>(d::flt_32))));
        return static_cast<float>(joan::hook::x87::ftol(static_cast<double>(cells))) * 32.0f + 16.0f;
    };
    const auto origin_x = static_cast<i32>(jo::as_cell(cell)->origin_x), origin_y = static_cast<i32>(jo::as_cell(cell)->origin_y);
    const float from_x = in_cell(position[0], origin_x), from_y = in_cell(position[1], origin_y);
    const float radius = static_cast<float>(static_cast<i32>(global<u32>(d::g_unit_body_r) << 1));
    const i32 zone = joan::orig::arg::as<i32>(joan::orig::as_path_space(space)->pick(joan::orig::arg::f32(from_x), joan::orig::arg::f32(from_y), joan::orig::arg::f32(position[2]), joan::orig::arg::bits(radius)));  // 0x0066c020
    if (zone == 0) return;
    nav::prepare_route(space, zone, from_x, from_y, in_cell(coarse(x), origin_x), in_cell(coarse(y), origin_y), radius);
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
