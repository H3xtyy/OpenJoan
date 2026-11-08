// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Ui class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Texture;

#pragma pack(push, 1)
struct Ui {
#include "ui_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Ui, tex_iunitatt) == 0x0);
static_assert(offsetof(Ui, tex_commele) == 0x4);
#endif

}  // namespace joan::orig
