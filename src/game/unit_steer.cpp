// SPDX-License-Identifier: GPL-3.0-or-later
// Walking around other units (docs/PATHFINDING.md). Everything here comes from game state and game time, and the units are
// visited in record order, so a replay sees the same decisions. The tables are not saved: after a load every unit starts
// without a side chosen and without a known speed, which settles within a moment.
#include "unit_steer.hpp"

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
constexpr double kMovedUnits = 4.0;   // moving at all: at least this far per sample
constexpr double kNearby = 400.0;     // units further than this are not looked at
constexpr u32 kKeepSideMs = 1500;     // a side chosen is kept this long after the unit in the way was last seen
constexpr u32 kRecheckMs = 250;       // how often a chosen side is checked against the level

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
        if (!walkable(dodge.first)) {
            if (!walkable(dodge.second)) return;
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

}  // namespace joan::unit_steer
