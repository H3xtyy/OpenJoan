// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Ai class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "order.hpp"

namespace joan::orig {

struct Unit;

#pragma pack(push, 1)
struct Ai : Order {
#include "ai_fields.inc"
#include "ai_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
