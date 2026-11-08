// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MaterialSpan class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MaterialSpan {
#include "material_span_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MaterialSpan) == 0x4);
static_assert(offsetof(MaterialSpan, first) == 0x0);
static_assert(offsetof(MaterialSpan, count) == 0x2);
#endif

}  // namespace joan::orig
