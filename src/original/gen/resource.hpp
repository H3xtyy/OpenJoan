// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Resource class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct SysEn;

#pragma pack(push, 1)
struct Resource {
#include "resource_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Resource) == 0x18);
static_assert(offsetof(Resource, callback) == 0x0);
static_assert(offsetof(Resource, context) == 0x4);
static_assert(offsetof(Resource, arg) == 0x8);
static_assert(offsetof(Resource, owner) == 0xc);
static_assert(offsetof(Resource, next) == 0x10);
static_assert(offsetof(Resource, prev) == 0x14);
#endif

}  // namespace joan::orig
