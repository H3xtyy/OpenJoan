// SPDX-License-Identifier: GPL-3.0-or-later
// game/pixfmt.hpp: direct entry points of the pixel-format ops in pixfmt.cpp that have no free facade.
#pragma once

#include <cstdint>

namespace joan::pixfmt {

/// 0x00571215 Pixel::unpack_color: a surface pixel of the current pixel format (set_pixel_format 0x005706f0) as
/// 0x00RRGGBB in the low three bytes (the upper bits as the original's register leaves them). The generated facade is
/// the Colour member `unpack_color`, a thiscall whose `this` the one-argument stdcall replacement does not take: on
/// x64 it would take the colour's register. Calls the replacement directly, as the free facades' bridges do.
std::uint32_t unpack_color(std::uint32_t pixel);

}  // namespace joan::pixfmt
