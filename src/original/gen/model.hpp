// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Model class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct BoneTree;
struct FileLoader;
struct Plane;
struct Skin;

#pragma pack(push, 1)
struct Model {
#include "model_fields.inc"
#include "model_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Model) == 0x69);
static_assert(offsetof(Model, loaded) == 0x0);
static_assert(offsetof(Model, kind) == 0x2);
static_assert(offsetof(Model, name) == 0x4);
static_assert(offsetof(Model, u8_24) == 0x24);
static_assert(offsetof(Model, mesh) == 0x28);
static_assert(offsetof(Model, skin) == 0x2c);
static_assert(offsetof(Model, has_sphere) == 0x30);
static_assert(offsetof(Model, sphere) == 0x34);
static_assert(offsetof(Model, bones) == 0x44);
static_assert(offsetof(Model, u32_48) == 0x48);
static_assert(offsetof(Model, stream) == 0x4c);
static_assert(offsetof(Model, lod_streams) == 0x50);
static_assert(offsetof(Model, shadow_stream) == 0x60);
static_assert(offsetof(Model, volume_stream) == 0x64);
static_assert(offsetof(Model, load_flag) == 0x68);
#endif

}  // namespace joan::orig
