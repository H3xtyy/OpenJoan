// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Lconv class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Lconv {
#include "lconv_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Lconv) == 0x30);
static_assert(offsetof(Lconv, decimal_point) == 0x0);
static_assert(offsetof(Lconv, thousands_sep) == 0x4);
static_assert(offsetof(Lconv, grouping) == 0x8);
static_assert(offsetof(Lconv, int_curr_symbol) == 0xc);
static_assert(offsetof(Lconv, currency_symbol) == 0x10);
static_assert(offsetof(Lconv, mon_decimal_point) == 0x14);
static_assert(offsetof(Lconv, mon_thousands_sep) == 0x18);
static_assert(offsetof(Lconv, mon_grouping) == 0x1c);
static_assert(offsetof(Lconv, positive_sign) == 0x20);
static_assert(offsetof(Lconv, negative_sign) == 0x24);
static_assert(offsetof(Lconv, int_frac_digits) == 0x28);
static_assert(offsetof(Lconv, frac_digits) == 0x29);
static_assert(offsetof(Lconv, p_cs_precedes) == 0x2a);
static_assert(offsetof(Lconv, p_sep_by_space) == 0x2b);
static_assert(offsetof(Lconv, n_cs_precedes) == 0x2c);
static_assert(offsetof(Lconv, n_sep_by_space) == 0x2d);
static_assert(offsetof(Lconv, p_sign_posn) == 0x2e);
static_assert(offsetof(Lconv, n_sign_posn) == 0x2f);
#endif

}  // namespace joan::orig
