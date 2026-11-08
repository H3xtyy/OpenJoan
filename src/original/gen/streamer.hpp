// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Streamer class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "critical_section.hpp"

namespace joan::orig {

struct FileLoader;

#pragma pack(push, 1)
struct Streamer {
#include "streamer_fields.inc"
#include "streamer_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Streamer) == 0xb8);
static_assert(offsetof(Streamer, mode) == 0x0);
static_assert(offsetof(Streamer, thread) == 0x4);
static_assert(offsetof(Streamer, lock) == 0x8);
static_assert(offsetof(Streamer, tail) == 0x20);
static_assert(offsetof(Streamer, head) == 0x24);
static_assert(offsetof(Streamer, current) == 0x28);
static_assert(offsetof(Streamer, u32_2c) == 0x2c);
static_assert(offsetof(Streamer, event_wake) == 0x30);
static_assert(offsetof(Streamer, event_done) == 0x34);
static_assert(offsetof(Streamer, event_38) == 0x38);
static_assert(offsetof(Streamer, event_3c) == 0x3c);
static_assert(offsetof(Streamer, event_finished) == 0x40);
static_assert(offsetof(Streamer, u32_44) == 0x44);
static_assert(offsetof(Streamer, file) == 0x48);
static_assert(offsetof(Streamer, active_count) == 0x4c);
static_assert(offsetof(Streamer, sector_size_cache) == 0x50);
#endif

}  // namespace joan::orig
