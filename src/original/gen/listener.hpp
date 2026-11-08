// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Listener class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Listener {
#include "listener_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Listener) == 0x35);
static_assert(offsetof(Listener, gain) == 0x0);
static_assert(offsetof(Listener, muted) == 0x4);
static_assert(offsetof(Listener, position) == 0x5);
static_assert(offsetof(Listener, velocity) == 0x11);
static_assert(offsetof(Listener, at) == 0x1d);
static_assert(offsetof(Listener, up) == 0x29);
#endif

}  // namespace joan::orig
