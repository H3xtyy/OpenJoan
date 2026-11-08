// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ModelEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ActorDef;
struct ModelMorphRec;

#pragma pack(push, 1)
struct ModelEntry {
#include "model_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ModelEntry) == 0x2b);
static_assert(offsetof(ModelEntry, path) == 0x0);
static_assert(offsetof(ModelEntry, id) == 0x4);
static_assert(offsetof(ModelEntry, dir) == 0x6);
static_assert(offsetof(ModelEntry, file) == 0xa);
static_assert(offsetof(ModelEntry, anim) == 0xe);
static_assert(offsetof(ModelEntry, ref_count) == 0x10);
static_assert(offsetof(ModelEntry, load_state) == 0x14);
static_assert(offsetof(ModelEntry, type) == 0x15);
static_assert(offsetof(ModelEntry, anim_db_id) == 0x16);
static_assert(offsetof(ModelEntry, i8_1a) == 0x1a);
static_assert(offsetof(ModelEntry, param_a) == 0x1b);
static_assert(offsetof(ModelEntry, param_b) == 0x1c);
static_assert(offsetof(ModelEntry, param_anim) == 0x1d);
static_assert(offsetof(ModelEntry, actor_def) == 0x1f);
static_assert(offsetof(ModelEntry, morph_count) == 0x23);
static_assert(offsetof(ModelEntry, morphs) == 0x27);
#endif

}  // namespace joan::orig
