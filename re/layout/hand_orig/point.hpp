// The mouse / cursor object's position test (refactor R6b, docs/RUNTIME.md): x at +0x14, y at +0x18 (g_mouse and
// the other objects 0x00593980 is called on).
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

namespace joan::orig {

#pragma pack(push, 1)

struct Point {
    std::uint8_t head[0x14];
    std::int32_t x;  // 0x14
    std::int32_t y;  // 0x18

    /// 0x00593980 Point::inside(x0, y0, x1, y1): 1 when x0 <= x <= x1 and y0 <= y <= y1 (full dword compares).
    [[nodiscard]] std::int32_t inside(std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1) const noexcept {
        return x >= x0 && y >= y0 && x <= x1 && y <= y1 ? 1 : 0;
    }

    // The other Point functions the game code calls (typed, through their slots).
#include "gen/point_methods.inc"
};

#pragma pack(pop)

static_assert(offsetof(Point, x) == 0x14);
static_assert(offsetof(Point, y) == 0x18);

}  // namespace joan::orig
