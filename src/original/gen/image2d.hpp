// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Image2D class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "gadget.hpp"

namespace joan::orig {

struct BmpRes;
struct Font2d;

#pragma pack(push, 1)
struct Image2D : Gadget {
#include "image2d_fields.inc"
#include "image2d_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Image2D) == 0xb7);
static_assert(offsetof(Image2D, active) == 0x70);
static_assert(offsetof(Image2D, kind) == 0x71);
static_assert(offsetof(Image2D, sprite_set) == 0x72);
static_assert(offsetof(Image2D, sprite_name) == 0x76);
static_assert(offsetof(Image2D, sprite_86) == 0x86);
static_assert(offsetof(Image2D, src_rect) == 0x88);
static_assert(offsetof(Image2D, font) == 0x90);
static_assert(offsetof(Image2D, text) == 0x94);
static_assert(offsetof(Image2D, text_mode) == 0x98);
static_assert(offsetof(Image2D, text_9a) == 0x9a);
static_assert(offsetof(Image2D, scale_x) == 0x9c);
static_assert(offsetof(Image2D, scale_y) == 0xa0);
static_assert(offsetof(Image2D, wrap_a4) == 0xa4);
static_assert(offsetof(Image2D, wrap_a6) == 0xa6);
static_assert(offsetof(Image2D, wrap_a8) == 0xa8);
static_assert(offsetof(Image2D, wrap_aa) == 0xaa);
static_assert(offsetof(Image2D, wrap_ab) == 0xab);
static_assert(offsetof(Image2D, colour) == 0xad);
static_assert(offsetof(Image2D, queue_b1) == 0xb1);
static_assert(offsetof(Image2D, queue_b3) == 0xb3);
static_assert(offsetof(Image2D, queue_b5) == 0xb5);
#endif

}  // namespace joan::orig
