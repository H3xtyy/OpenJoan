// Small game containers whose accessors the game calls out of line (refactor R5, docs/RUNTIME.md). The names are
// ours; the layouts are what the accessors read.
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstdint>

#include "dynarray.hpp"
#include "gen/item_def.hpp"
#include "gen/item_res.hpp"
#include "gen/unit_type.hpp"

namespace joan::orig {

#pragma pack(push, 1)

/// A table with a pointer list at +4 (the unit table g_units, ...): 0x00425d20 get / 0x00425d50 empty_at.
struct Units {
    std::uint8_t head[4];
    DynArray list;

    // The other Units functions the game code calls (typed, through their slots).
#include "gen/units_methods.inc"
};

struct PathSpace;

/// The player table: pointer list at +0xb20 (0x006af310 get: null when the record is out of range or empty).
struct Players {
    std::uint8_t head[0xb20];
    DynArray list;

    // The other Players functions the game code calls (typed, through their slots).
#include "gen/players_methods.inc"
};

/// Fixed-stride record arrays, 1-based, no bounds check. The index is a 16-bit value (the original pushes the word
/// with garbage upper bits and the callee sign-extends the low word).
struct Records203 {  // 0x0042d0a0: data pointer at +0, records of 0x203 bytes
    std::uint8_t* data;
    // The object is the UnitRes (the data pointer is UnitRes::types, the records are UnitType): the stride follows
    // the x64 layout (0x203 on x86).
    [[nodiscard]] std::uint8_t* get(std::int16_t i) const {
        return data + i * static_cast<std::int32_t>(sizeof(UnitType)) - static_cast<std::int32_t>(sizeof(UnitType));
    }

    // The other Records203 functions the game code calls (typed, through their slots).
#include "gen/records203_methods.inc"
};
struct Records2bf {  // 0x00425ce0: data pointer at +0x14, records of 0x2bf bytes
    std::uint8_t head[0x14];
    std::uint8_t* data;
    // The object is the ItemRes (the data pointer is ItemRes::items, the records are ItemDef): read through those
    // types so the offset and the stride follow the x64 layout (+0x14 / 0x2bf on x86).
    [[nodiscard]] std::uint8_t* get(std::int16_t i) const {
        std::uint8_t* const items = reinterpret_cast<std::uint8_t*>(reinterpret_cast<const ItemRes*>(this)->items);
        return items + i * static_cast<std::int32_t>(sizeof(ItemDef)) - static_cast<std::int32_t>(sizeof(ItemDef));
    }
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Units) == 4 + sizeof(DynArray));
static_assert(sizeof(Records2bf) == 0x18);
#endif

}  // namespace joan::orig
