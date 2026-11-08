// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Flc class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "flc_header.hpp"

namespace joan::orig {

struct TFAnimation;

#pragma pack(push, 1)
struct Flc {
#include "flc_fields.inc"
#include "flc_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Flc) == 0x21);
static_assert(offsetof(Flc, animation) == 0x0);
static_assert(offsetof(Flc, frame_buf) == 0x4);
static_assert(offsetof(Flc, palette) == 0x8);
static_assert(offsetof(Flc, header) == 0xc);
static_assert(offsetof(Flc, opened) == 0x1d);
#endif

}  // namespace joan::orig
