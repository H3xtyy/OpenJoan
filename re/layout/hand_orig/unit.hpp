// The few unit fields the game reads through out-of-line accessors (refactor R6b, docs/RUNTIME.md). Only the bytes
// these accessors touch are laid out; the rest of the unit is opaque here.
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "../call.hpp"
#include "../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)

struct Unit {
    std::uint8_t head[0xe];
    float position_[3];           // 0x0e  x, y, z
    std::uint8_t mid[0x120 - 0x1a];
    std::int32_t state_120;       // 0x120 nonzero keeps a unit without hit points from counting as dead
    std::uint8_t mid2[0x135 - 0x124];
    std::int32_t hit_points;      // 0x135

    /// 0x004fedb0 Unit::position(out): copies the 12 position bytes to out and returns out.
    void* position(void* out) const noexcept {
        std::memcpy(out, position_, sizeof position_);
        return out;
    }
    /// 0x0042d0e0 Unit::dead(): 1 when hit points <= 0 and +0x120 is 0 (the original returns al).
    [[nodiscard]] std::uint8_t dead() const noexcept { return hit_points <= 0 && state_120 == 0 ? 1 : 0; }

    // Every other Unit function the game code calls (typed, through the function's slot; refactor R9).
#include "gen/unit_methods.inc"
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Unit, position_) == 0x0e);
static_assert(offsetof(Unit, state_120) == 0x120);
static_assert(offsetof(Unit, hit_points) == 0x135);
#endif

}  // namespace joan::orig
