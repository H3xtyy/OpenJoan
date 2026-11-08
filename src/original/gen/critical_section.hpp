// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CriticalSection class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CriticalSection {
#include "critical_section_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CriticalSection) == 0x18);
static_assert(offsetof(CriticalSection, debug_info) == 0x0);
static_assert(offsetof(CriticalSection, lock_count) == 0x4);
static_assert(offsetof(CriticalSection, recursion_count) == 0x8);
static_assert(offsetof(CriticalSection, owning_thread) == 0xc);
static_assert(offsetof(CriticalSection, lock_semaphore) == 0x10);
static_assert(offsetof(CriticalSection, spin_count) == 0x14);
#endif

}  // namespace joan::orig
