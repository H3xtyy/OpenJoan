// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TexturePtr class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Texture;

#pragma pack(push, 1)
struct TexturePtr {
#include "texture_ptr_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TexturePtr) == 0x4);
static_assert(offsetof(TexturePtr, p) == 0x0);
#endif

}  // namespace joan::orig
