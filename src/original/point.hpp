// The mouse / cursor object's position test (refactor R6b): Point::inside 0x00593980 is called on the Input object
// g_mouse (0x00a516b0, bss type Input) and reads its cursor position, x at +0x14 and y at +0x18 on x86. The
// Point fields below are the x86 picture; on x86-64 the three DirectInput pointers of Input before the cursor move
// cur_x / cur_y, so inside() reads them through the generated Input layout (same offsets, same code, on x86).
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include "gen/input.hpp"
#include <cstddef>
#include <cstdint>

namespace joan::orig {

#pragma pack(push, 1)

/// The x86 picture of the Input object up to the cursor position (see the file comment); only inside() is used.
struct Point {
    std::int32_t x_;  ///< 0x0
    std::int32_t y_;  ///< 0x4
    std::int32_t z;  ///< 0x8
    std::uint8_t gap_c[0x8];  ///< 0xc bytes of the Input object this view does not name
    std::int32_t x;  ///< 0x14
    std::int32_t y;  ///< 0x18

    /// 0x00593980 Point::inside(x0, y0, x1, y1): 1 when x0 <= x <= x1 and y0 <= y <= y1 (full dword compares) for
    /// the cursor position of the Input object this is called on (Input::cur_x / cur_y; +0x14 / +0x18 on x86).
    [[nodiscard]] std::int32_t inside(std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1) const noexcept {
        const Input* const in = reinterpret_cast<const Input*>(this);
        return in->cur_x >= x0 && in->cur_y >= y0 && in->cur_x <= x1 && in->cur_y <= y1 ? 1 : 0;
    }

    // The other Point functions the game code calls (typed, through their slots).
#include "gen/point_methods.inc"
};

#pragma pack(pop)

static_assert(offsetof(Point, x) == 0x14);
static_assert(offsetof(Point, y) == 0x18);
static_assert(sizeof(void*) != 4 || (offsetof(Input, cur_x) == 0x14 && offsetof(Input, cur_y) == 0x18),
              "Point::inside reads Input::cur_x / cur_y: the x86 picture is +0x14 / +0x18");

}  // namespace joan::orig
