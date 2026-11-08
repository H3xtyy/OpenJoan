// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SaveLoad class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct GameFile;

#pragma pack(push, 1)
struct SaveLoad {
#include "save_load_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SaveLoad) == 0x94);
static_assert(offsetof(SaveLoad, images) == 0x0);
static_assert(offsetof(SaveLoad, fonts) == 0x34);
static_assert(offsetof(SaveLoad, started) == 0x68);
static_assert(offsetof(SaveLoad, base_pct) == 0x69);
static_assert(offsetof(SaveLoad, pct) == 0x6d);
static_assert(offsetof(SaveLoad, shown_pct) == 0x71);
static_assert(offsetof(SaveLoad, stages) == 0x75);
static_assert(offsetof(SaveLoad, stage) == 0x79);
static_assert(offsetof(SaveLoad, substages) == 0x7d);
static_assert(offsetof(SaveLoad, substage) == 0x81);
static_assert(offsetof(SaveLoad, state) == 0x85);
static_assert(offsetof(SaveLoad, state_time) == 0x86);
static_assert(offsetof(SaveLoad, u32_8a) == 0x8a);
static_assert(offsetof(SaveLoad, u8_8e) == 0x8e);
static_assert(offsetof(SaveLoad, mode) == 0x8f);
static_assert(offsetof(SaveLoad, save) == 0x90);
#endif

}  // namespace joan::orig
