// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AlBuffer class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "sound3dbuffer.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct AlBuffer : Sound3DBuffer {
#include "al_buffer_fields.inc"
#include "al_buffer_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
