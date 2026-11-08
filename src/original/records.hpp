// Packed record layouts of resource lists (#pragma pack(1) in one header, so the modules that use them carry no
// pragmas and can be split by re/split_modules.py).
#pragma once
#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstdint>

namespace joan::orig {

#pragma pack(push, 1)
/// 14-byte element of GrassRes (0x00487f70).
/// TODO(re): 0x00487f70 the meaning of a, b, r0 and r1 is not established.
struct GrassEntry {
    std::uint16_t type;  ///< grass type id
    std::uint16_t a;  ///< unknown
    std::uint16_t b;  ///< unknown
    float r0;  ///< unknown
    float r1;  ///< unknown
};
/// 10-byte element of the 0x004ca980 list.
/// TODO(re): 0x004ca980 what `value` is (a rate or a cost) is not established.
struct RecruitRow {
    std::int16_t type;  ///< unit type id
    float value;  ///< unknown scalar
    std::int32_t count;  ///< number of units

    // The other RecruitRow functions the game code calls (typed, through their slots).
#include "gen/recruit_row_methods.inc"
};
/// 14-byte element of the 0x004ce630 list.
/// TODO(re): 0x004ce630 the meaning of value, a and b is not established.
struct ItemRow {
    std::int16_t item;  ///< item id
    float value;  ///< unknown scalar
    std::int32_t a;  ///< unknown
    std::int32_t b;  ///< unknown
};
#pragma pack(pop)
static_assert(sizeof(GrassEntry) == 14, "GrassEntry");
static_assert(sizeof(RecruitRow) == 10, "RecruitRow");
static_assert(sizeof(ItemRow) == 14, "ItemRow");

}  // namespace joan::orig
