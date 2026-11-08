// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ShadowModel class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct FileLoader;
struct Plane;
struct Skin;

#pragma pack(push, 1)
struct ShadowModel {
#include "shadow_model_fields.inc"
#include "shadow_model_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(ShadowModel, bytes_0) == 0x0);
static_assert(offsetof(ShadowModel, u32_24) == 0x24);
static_assert(offsetof(ShadowModel, ptr_28) == 0x28);
static_assert(offsetof(ShadowModel, ptr_2c) == 0x2c);
static_assert(offsetof(ShadowModel, u32_30) == 0x30);
static_assert(offsetof(ShadowModel, ptr_4c) == 0x4c);
static_assert(offsetof(ShadowModel, lod_streams) == 0x50);
static_assert(offsetof(ShadowModel, ptr_60) == 0x60);
static_assert(offsetof(ShadowModel, ptr_64) == 0x64);
static_assert(offsetof(ShadowModel, u32_68) == 0x68);
#endif

}  // namespace joan::orig
