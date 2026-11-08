// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VbAlloc class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "reorder_batch.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VbAlloc : ReorderBatch {
#include "vb_alloc_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VbAlloc) == 0x28);
#endif

}  // namespace joan::orig
