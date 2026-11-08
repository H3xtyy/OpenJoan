// The engine's 4-byte colour (refactor R6b, docs/RUNTIME.md): r, g, b, a bytes in this order. The constructor
// 0x0042a260 is called out of line on a stack temporary whose dword the game then reads.
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstdint>
#include <type_traits>

namespace joan::orig {

#pragma pack(push, 1)

/// 4-byte colour, bytes in r, g, b, a order.
struct Colour {
    std::uint8_t r;  ///< red
    std::uint8_t g;  ///< green
    std::uint8_t b;  ///< blue
    std::uint8_t a;  ///< alpha

    /// 0x0042a260 Colour::Colour(r, g, b, a): stores the low byte of each argument (the original takes four dwords
    /// and keeps their low bytes, so any integer type gives the same bytes). Returns this.
    template <class R, class G, class B, class A>
    Colour* construct(R r_, G g_, B b_, A a_) noexcept {
        static_assert((std::is_integral_v<R> || std::is_enum_v<R>) && (std::is_integral_v<G> || std::is_enum_v<G>) &&
                          (std::is_integral_v<B> || std::is_enum_v<B>) && (std::is_integral_v<A> || std::is_enum_v<A>),
                      "Colour::construct takes integers (the original keeps the low byte of each dword)");
        r = static_cast<std::uint8_t>(r_);
        g = static_cast<std::uint8_t>(g_);
        b = static_cast<std::uint8_t>(b_);
        a = static_cast<std::uint8_t>(a_);
        return this;
    }

    // The other Colour functions the game code calls (typed, through their slots).
#include "gen/colour_methods.inc"
};

#pragma pack(pop)

static_assert(sizeof(Colour) == 4);

}  // namespace joan::orig
