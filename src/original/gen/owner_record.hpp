// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's OwnerRecord class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct OwnerRecord {
#include "owner_record_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(OwnerRecord) == 0x12);
static_assert(offsetof(OwnerRecord, a) == 0x0);
static_assert(offsetof(OwnerRecord, c) == 0x2);
static_assert(offsetof(OwnerRecord, b) == 0x4);
static_assert(offsetof(OwnerRecord, d) == 0x6);
static_assert(offsetof(OwnerRecord, e) == 0xa);
static_assert(offsetof(OwnerRecord, f) == 0xe);
#endif

}  // namespace joan::orig
