// The video options' draw distance ([0xa58050], snd_slider_60) and dynamic grass draw distance ([0xa58058],
// snd_slider_68) ranges, widened from the original's 2004 tuning: the new minimum is the original maximum and the new
// maximum is four times that. See interfac/optmenui.cpp (sliders), game/unknown_camctl.cpp and rts.cpp (draw
// distance readers), game/game.cpp (DynGrass::init) and game/dyngrass.cpp (patch lookup).
#pragma once

#include <cstdint>

namespace joan::optmenu {

// Draw distance: originally 0x401 + step * 0x3c00 / 20 (1025 .. 16385), 21 slider steps.
inline constexpr std::int32_t kDrawSteps = 0x14;
inline constexpr std::int32_t kDrawMin = 0x4001;         // 16385, the original maximum
inline constexpr std::int32_t kDrawMax = kDrawMin * 4;   // 65540

// Dynamic grass distance: originally step * 20 (0 .. 2000), 101 slider steps.
inline constexpr std::int32_t kGrassSteps = 0x64;
inline constexpr std::int32_t kGrassMin = 0x7d0;         // 2000, the original maximum
inline constexpr std::int32_t kGrassMax = kGrassMin * 4; // 8000

inline constexpr std::int32_t clamp_range(std::int32_t v, std::int32_t lo, std::int32_t hi) { return v < lo ? lo : (v > hi ? hi : v); }

// value -> slider step rounds to the nearest step (the draw distance span is not a multiple of its step count).
inline constexpr std::int32_t from_slider(std::int32_t s, std::int32_t steps, std::int32_t lo, std::int32_t hi) { return lo + clamp_range(s, 0, steps) * (hi - lo) / steps; }
inline constexpr std::int32_t to_slider(std::int32_t v, std::int32_t steps, std::int32_t lo, std::int32_t hi) { return ((clamp_range(v, lo, hi) - lo) * steps + (hi - lo) / 2) / (hi - lo); }

inline constexpr std::int32_t draw_from_slider(std::int32_t s) { return from_slider(s, kDrawSteps, kDrawMin, kDrawMax); }
inline constexpr std::int32_t draw_to_slider(std::int32_t v) { return to_slider(v, kDrawSteps, kDrawMin, kDrawMax); }
inline constexpr std::int32_t grass_from_slider(std::int32_t s) { return from_slider(s, kGrassSteps, kGrassMin, kGrassMax); }
inline constexpr std::int32_t grass_to_slider(std::int32_t v) { return to_slider(v, kGrassSteps, kGrassMin, kGrassMax); }

static_assert(draw_from_slider(0) == kDrawMin && draw_from_slider(kDrawSteps) == kDrawMax);
static_assert(grass_from_slider(0) == kGrassMin && grass_from_slider(kGrassSteps) == kGrassMax);
static_assert(draw_to_slider(draw_from_slider(7)) == 7 && grass_to_slider(grass_from_slider(37)) == 37);

/// Clamps [0xa58050] and [0xa58058] in place into the ranges above. A config saved with the original ranges (or the
/// Config::reset defaults) holds values below the new minimums; every reader of the two globals calls this first.
void clamp_view_ranges();

}  // namespace joan::optmenu
