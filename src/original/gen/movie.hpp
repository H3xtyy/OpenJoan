// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Movie class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "video_player.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Movie : VideoPlayer {
#include "movie_fields.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
