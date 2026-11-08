// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Credits class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "image2d_t.hpp"
#include "label.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Credits {
#include "credits_fields.inc"
#include "credits_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Credits, active) == 0x0);
static_assert(offsetof(Credits, stage) == 0x1);
static_assert(offsetof(Credits, images) == 0x2);
static_assert(offsetof(Credits, backdrop) == 0x32);
static_assert(offsetof(Credits, label_0) == 0xe9);
static_assert(offsetof(Credits, label_1) == 0x1a0);
static_assert(offsetof(Credits, label_2) == 0x257);
static_assert(offsetof(Credits, label_3) == 0x30e);
static_assert(offsetof(Credits, label_4) == 0x3c5);
#endif

}  // namespace joan::orig
