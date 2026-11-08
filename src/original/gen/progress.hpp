// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Progress class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "save_load.hpp"

namespace joan::orig {

struct GameFile;

#pragma pack(push, 1)
struct Progress : SaveLoad {
#include "progress_fields.inc"
#include "progress_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
