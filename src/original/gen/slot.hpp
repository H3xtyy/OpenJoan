// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Slot class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "sound_stream.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Slot : SoundStream {
#include "slot_fields.inc"
#include "slot_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
