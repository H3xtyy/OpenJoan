// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SmsNode class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SmsNode {
#include "sms_node_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SmsNode) == 0x14);
static_assert(offsetof(SmsNode, text) == 0x0);
static_assert(offsetof(SmsNode, kind) == 0x4);
static_assert(offsetof(SmsNode, replace) == 0x8);
static_assert(offsetof(SmsNode, prev) == 0xc);
static_assert(offsetof(SmsNode, next) == 0x10);
#endif

}  // namespace joan::orig
