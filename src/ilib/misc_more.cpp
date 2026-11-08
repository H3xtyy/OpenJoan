// The remaining live Misc functions (layout in original/misc.hpp): the
// "move around a point" spiral offsets and their cache, the Misc constructor,
// and the fill-except-regions helper used by object constructors. The rest
// of the 0x0058ef00-0x00592050 range has no callers in this build.
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <vector>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../original/engine.hpp"
#include "../original/misc.hpp"
#include "../registry.hpp"

namespace {
using joan::hook::global;
using joan::hook::original;
using joan::orig::Misc;

JOAN_COUNTER(c_move, "Misc::move_around_a_point");
JOAN_COUNTER(c_move_cached, "Misc::move_around_a_point_cached");
JOAN_COUNTER(c_init_table, "Misc::init_move_around_table");
JOAN_COUNTER(c_ctor, "Misc::Misc");

constexpr std::int32_t kCached = 900;
const std::int8_t* table_x() { return &global<std::int8_t>(d::g_a64a3b); }  // index 1..900
const std::int8_t* table_y() { return &global<std::int8_t>(d::g_a64dbf); }
std::int16_t& table_count() { return global<std::int16_t>(d::g_table_count); }

// The 1-based position on the spiral, wrapped to the (min side + 1)^2 area (as a short).
std::int16_t spiral_index(std::int16_t num, std::int16_t width, std::int16_t height) {
    const auto side = static_cast<std::int16_t>(width > height ? height + 1 : width + 1);
    return static_cast<std::int16_t>((num - 1) % (side * side) + 1);
}
}  // namespace

namespace joan::orig {

// 0x00591480
void Misc::move_around_a_point(std::int16_t num, std::int16_t width, std::int16_t height, std::int32_t* x,
                               std::int32_t* y) {
    const std::int32_t n = spiral_index(num, width, height);
    if (n <= 1) {
        *y = 0;
        *x = 0;
        return;
    }
    std::int32_t ring = 1;  // side length of the ring holding n (odd)
    while (ring < 210) {
        if (n <= ring * ring) break;
        ring += 2;
    }
    const std::int32_t half = (ring - 1) / 2;
    std::int32_t pos = n - (ring - 2) * (ring - 2);
    pos = (pos - 1 - (ring - 3) / 2) % (ring * 4 - 4) + 1;
    std::int32_t s;
    if (pos < ring) s = pos - 1;
    else if (pos <= (ring - 1) * 3) s = ring * 2 - 1 - pos;
    else s = pos + 3 - ring * 4;  // pos >= 3 * ring - 2 (the original's last case; the cases are exhaustive)
    if (s > 0) *x = s > half ? half : s;
    else *x = -s > half ? -half : s;
    const std::int32_t t = (ring - 1) - (s < 0 ? -s : s);
    if (pos > ring && pos <= ring * 3 - 3) *y = t > half ? -half : -t;
    else *y = t > half ? half : t;
}

// 0x005913b0
void Misc::move_around_a_point_cached(std::int16_t num, std::int16_t width, std::int16_t height, std::int32_t* x,
                                      std::int32_t* y) {
    const std::int16_t n = spiral_index(num, width, height);
    if (n <= kCached) {  // (n <= 0 reads in front of the tables, like the original)
        *x = table_x()[n];
        *y = table_y()[n];
        return;
    }
    using Fn = void (Misc::*)(std::int16_t, std::int16_t, std::int16_t, std::int32_t*, std::int32_t*);
    (this->*original<Fn>(fn::move_around_a_point))(num, width, height, x, y);  // via thunk
}

// 0x00591710
void Misc::init_move_around_table() {
    if (table_count() == kCached) return;
    using Fn = void (Misc::*)(std::int16_t, std::int16_t, std::int16_t, std::int32_t*, std::int32_t*);
    for (std::int32_t i = 1; i <= kCached; ++i) {
        std::int32_t x, y;
        (this->*original<Fn>(fn::move_around_a_point))(static_cast<std::int16_t>(i), kCached, kCached, &x, &y);
        const_cast<std::int8_t*>(table_x())[i] = static_cast<std::int8_t>(x);
        const_cast<std::int8_t*>(table_y())[i] = static_cast<std::int8_t>(y);
    }
    table_count() = kCached;
}

// 0x00591cb0
Misc* Misc::construct() {
    c_ctor.hit();
    flag_79 = 0;
    (this->*original<void (Misc::*)()>(fn::init_move_around_table))();  // init_move_around_table, via thunk
    return this;
}

}  // namespace joan::orig

// ---------------------------------------------------------------------------
// fill_except (0x005919a0)
// ---------------------------------------------------------------------------
namespace {

// ---------------------------------------------------------------------------
// Entry points
// ---------------------------------------------------------------------------
struct Entry : Misc {
    void move_entry(std::int16_t num, std::int16_t w, std::int16_t h, std::int32_t* x, std::int32_t* y) {
        c_move.hit();
        move_around_a_point(num, w, h, x, y);
    }
    void move_cached_entry(std::int16_t num, std::int16_t w, std::int16_t h, std::int32_t* x, std::int32_t* y) {
        c_move_cached.hit();
        move_around_a_point_cached(num, w, h, x, y);
    }
    void init_table_entry() {
        c_init_table.hit();
        init_move_around_table();
    }
};
static_assert(sizeof(Entry) == sizeof(Misc));
}  // namespace

#define MISC(name, addr, fn) JOAN_REPLACE("ilib/misc.cpp", "Misc::" name, addr, fn)
MISC("cal_move_around_a_point_v2", 0x00591480, &Entry::move_entry);
MISC("cal_move_around_a_point", 0x005913b0, &Entry::move_cached_entry);
MISC("init_move_around_table", 0x00591710, &Entry::init_table_entry);
#undef MISC
