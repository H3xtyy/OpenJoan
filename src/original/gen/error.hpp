// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Error class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Error {
#include "error_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Error) == 0xc);
static_assert(offsetof(Error, on_fatal) == 0x0);
static_assert(offsetof(Error, on_report) == 0x4);
static_assert(offsetof(Error, fn_8) == 0x8);
#endif

}  // namespace joan::orig
