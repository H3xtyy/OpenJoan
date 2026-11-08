// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's DP class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct IDirectMusicLoader8;
struct IDirectMusicPerformance8;
struct IDirectMusicScript8;

#pragma pack(push, 1)
struct DP {
#include "dp_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(DP) == 0x10);
static_assert(offsetof(DP, vtable) == 0x0);
static_assert(offsetof(DP, performance) == 0x4);
static_assert(offsetof(DP, loader) == 0x8);
static_assert(offsetof(DP, script) == 0xc);
#endif

}  // namespace joan::orig
