// SPDX-License-Identifier: GPL-3.0-or-later
// [Fixes] UnitStuck, part 2 (see unit_stuck.hpp).
#include "unit_stuck.hpp"

#include <cmath>
#include <unordered_map>

namespace joan::unit_stuck {
namespace {

struct Watch {
    std::int16_t id = 0;
    std::int32_t index = -1;                  // the waypoint walked to
    float best = 0.0f;                        // the closest the unit came to it, this window
    std::uint32_t since = 0;                  // game time the window started
    std::uint32_t seen = 0;                   // game time of the last watch()
    bool gave = false;                        // a route was given up:
    float spot_x = 0.0f, spot_y = 0.0f;       //   its target
    float from_x = 0.0f, from_y = 0.0f;       //   where the unit stood
    std::uint32_t gave_at = 0;                //   when
};

// Keyed by the unit record's address; `id` and `seen` tell a record reused for another unit (or a unit not watched
// for a while: another mission, a load) from the one the entry was made for. Only looked up, never iterated.
std::unordered_map<const void*, Watch>& table() {
    static std::unordered_map<const void*, Watch> t;
    return t;
}

float dist2(float ax, float ay, float bx, float by) { return (ax - bx) * (ax - bx) + (ay - by) * (ay - by); }

}  // namespace

bool watch(const void* unit, std::int16_t id, float x, float y, std::int32_t index, float wx, float wy, float tx, float ty,
           std::uint32_t now_ms) {
    Watch& w = table()[unit];
    if (w.id != id || now_ms < w.seen || now_ms - w.seen > 2 * kWindowMs) {
        w = Watch{};
        w.id = id;
    }
    w.seen = now_ms;
    const float d = std::sqrt(dist2(x, y, wx, wy));
    if (index != w.index || d < w.best - kMinMove) {  // a new waypoint, or real progress toward this one
        w.index = index;
        w.best = d;
        w.since = now_ms;
        return false;
    }
    if (now_ms - w.since < kWindowMs) return false;
    w.gave = true;
    w.spot_x = tx;
    w.spot_y = ty;
    w.from_x = x;
    w.from_y = y;
    w.gave_at = now_ms;
    w.index = -1;
    w.since = now_ms;
    return true;
}

bool gave_up(const void* unit, std::int16_t id, float x, float y, float sx, float sy, std::uint32_t now_ms) {
    const auto it = table().find(unit);
    if (it == table().end()) return false;
    const Watch& w = it->second;
    if (!w.gave || w.id != id || now_ms < w.gave_at || now_ms - w.gave_at >= kRetryMs) return false;
    return dist2(x, y, w.from_x, w.from_y) <= kSameSpot * kSameSpot &&
           dist2(sx, sy, w.spot_x, w.spot_y) <= kSameSpot * kSameSpot;
}

}  // namespace joan::unit_stuck
