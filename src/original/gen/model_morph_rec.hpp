// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ModelMorphRec class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Morph;

#pragma pack(push, 1)
struct ModelMorphRec {
#include "model_morph_rec_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ModelMorphRec) == 0xa4);
static_assert(offsetof(ModelMorphRec, name) == 0x0);
static_assert(offsetof(ModelMorphRec, file) == 0x20);
static_assert(offsetof(ModelMorphRec, anim) == 0x60);
static_assert(offsetof(ModelMorphRec, morph) == 0xa0);
#endif

}  // namespace joan::orig
