// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's GrassType class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct GrassType {
#include "grass_type_fields.inc"
#include "grass_type_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(GrassType) == 0x27);
static_assert(offsetof(GrassType, id) == 0x0);
static_assert(offsetof(GrassType, name) == 0x2);
static_assert(offsetof(GrassType, entries) == 0x23);
#endif

}  // namespace joan::orig
