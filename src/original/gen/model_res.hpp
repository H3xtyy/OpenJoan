// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ModelRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "string_pool.hpp"

namespace joan::orig {

struct ModelEntry;

#pragma pack(push, 1)
struct ModelRes {
#include "model_res_fields.inc"
#include "model_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ModelRes) == 0x15);
static_assert(offsetof(ModelRes, count) == 0x0);
static_assert(offsetof(ModelRes, capacity) == 0x4);
static_assert(offsetof(ModelRes, entries) == 0x8);
static_assert(offsetof(ModelRes, loaded) == 0xc);
static_assert(offsetof(ModelRes, strings) == 0xd);
#endif

}  // namespace joan::orig
