// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's NatRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "nation_res.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct NatRes : NationRes {
#include "nat_res_fields.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
