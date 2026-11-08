// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ReorderBatch class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "batch_key.hpp"

namespace joan::orig {

struct SpanNode;
struct StripNode;

#pragma pack(push, 1)
struct ReorderBatch : BatchKey {
#include "reorder_batch_fields.inc"
#include "reorder_batch_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ReorderBatch) == 0x28);
static_assert(offsetof(ReorderBatch, frame) == 0x14);
static_assert(offsetof(ReorderBatch, spans) == 0x18);
static_assert(offsetof(ReorderBatch, span_tail) == 0x1c);
static_assert(offsetof(ReorderBatch, strips) == 0x20);
static_assert(offsetof(ReorderBatch, strip_tail) == 0x24);
#endif

}  // namespace joan::orig
