// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Nation class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "color.hpp"
#include "rel.hpp"

namespace joan::orig {

struct ArrI16;

#pragma pack(push, 1)
struct Nation {
#include "nation_fields.inc"
#include "nation_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Nation) == 0x1545);
static_assert(offsetof(Nation, id) == 0x0);
static_assert(offsetof(Nation, kind) == 0x2);
static_assert(offsetof(Nation, dirty) == 0x4);
static_assert(offsetof(Nation, state) == 0x5);
static_assert(offsetof(Nation, king_unit) == 0x6);
static_assert(offsetof(Nation, kind32) == 0x8);
static_assert(offsetof(Nation, colour) == 0xc);
static_assert(offsetof(Nation, unit_count) == 0x10);
static_assert(offsetof(Nation, name_) == 0x14);
static_assert(offsetof(Nation, relation1) == 0x55);
static_assert(offsetof(Nation, relation2) == 0x149);
static_assert(offsetof(Nation, relation3) == 0x23d);
static_assert(offsetof(Nation, relation4) == 0x331);
static_assert(offsetof(Nation, relation5) == 0x425);
static_assert(offsetof(Nation, relation6) == 0x519);
static_assert(offsetof(Nation, relation7) == 0x60d);
static_assert(offsetof(Nation, relation8) == 0x701);
static_assert(offsetof(Nation, relation9) == 0x7f5);
static_assert(offsetof(Nation, relation10) == 0x8e9);
static_assert(offsetof(Nation, relation11) == 0x9dd);
static_assert(offsetof(Nation, relation12) == 0xad1);
static_assert(offsetof(Nation, relation13) == 0xbc5);
static_assert(offsetof(Nation, relation14) == 0xcb9);
static_assert(offsetof(Nation, relation15) == 0xdad);
static_assert(offsetof(Nation, relation16) == 0xea1);
static_assert(offsetof(Nation, relation17) == 0xf95);
static_assert(offsetof(Nation, relation18) == 0x1089);
static_assert(offsetof(Nation, relation19) == 0x117d);
static_assert(offsetof(Nation, relation20) == 0x1271);
static_assert(offsetof(Nation, at_war) == 0x1365);
static_assert(offsetof(Nation, was_at_war) == 0x1366);
static_assert(offsetof(Nation, war_day) == 0x1367);
static_assert(offsetof(Nation, mood) == 0x136d);
static_assert(offsetof(Nation, gold) == 0x1371);
static_assert(offsetof(Nation, time) == 0x1375);
static_assert(offsetof(Nation, population) == 0x1385);
static_assert(offsetof(Nation, military_strength) == 0x1389);
static_assert(offsetof(Nation, income) == 0x138d);
static_assert(offsetof(Nation, income_prev) == 0x1391);
static_assert(offsetof(Nation, trade_income) == 0x1395);
static_assert(offsetof(Nation, trade_income_prev) == 0x1399);
static_assert(offsetof(Nation, f32_139d) == 0x139d);
static_assert(offsetof(Nation, f32_13a1) == 0x13a1);
static_assert(offsetof(Nation, income_by_kind) == 0x13a5);
static_assert(offsetof(Nation, income_by_kind_prev) == 0x13c5);
static_assert(offsetof(Nation, income_total) == 0x13e5);
static_assert(offsetof(Nation, income_total_prev) == 0x13e9);
static_assert(offsetof(Nation, spend_by_kind) == 0x13ed);
static_assert(offsetof(Nation, spend_by_kind_prev) == 0x1431);
static_assert(offsetof(Nation, spent) == 0x1475);
static_assert(offsetof(Nation, spent_prev) == 0x1479);
static_assert(offsetof(Nation, f32_147d) == 0x147d);
static_assert(offsetof(Nation, f32_1481) == 0x1481);
static_assert(offsetof(Nation, time_gained) == 0x1485);
static_assert(offsetof(Nation, time_gained_prev) == 0x1489);
static_assert(offsetof(Nation, time_transferred) == 0x148d);
static_assert(offsetof(Nation, time_transferred_prev) == 0x1491);
static_assert(offsetof(Nation, time_net) == 0x1495);
static_assert(offsetof(Nation, time_net_prev) == 0x1499);
static_assert(offsetof(Nation, mood_delta) == 0x149d);
static_assert(offsetof(Nation, mood_delta_prev) == 0x14a1);
static_assert(offsetof(Nation, goods) == 0x14a5);
static_assert(offsetof(Nation, demand) == 0x14a9);
static_assert(offsetof(Nation, price_trend) == 0x14ad);
static_assert(offsetof(Nation, production) == 0x14b1);
static_assert(offsetof(Nation, tax) == 0x14b5);
static_assert(offsetof(Nation, sold) == 0x14b9);
static_assert(offsetof(Nation, sold_prev) == 0x14bd);
static_assert(offsetof(Nation, surplus) == 0x14c1);
static_assert(offsetof(Nation, surplus_prev) == 0x14c5);
static_assert(offsetof(Nation, consume_limit) == 0x14c9);
static_assert(offsetof(Nation, own_tax) == 0x14cd);
static_assert(offsetof(Nation, import_tax) == 0x14d1);
static_assert(offsetof(Nation, export_tax) == 0x14d5);
static_assert(offsetof(Nation, score_goods) == 0x14d9);
static_assert(offsetof(Nation, strength) == 0x14dd);
static_assert(offsetof(Nation, wealth) == 0x14e1);
static_assert(offsetof(Nation, mood_copy) == 0x14e5);
static_assert(offsetof(Nation, mood_score) == 0x14f9);
static_assert(offsetof(Nation, recruit_count) == 0x14fd);
static_assert(offsetof(Nation, recruit_ages) == 0x1501);
static_assert(offsetof(Nation, traits) == 0x1515);
static_assert(offsetof(Nation, leader_count) == 0x152d);
static_assert(offsetof(Nation, leader_cap) == 0x152f);
static_assert(offsetof(Nation, leaders) == 0x1531);
static_assert(offsetof(Nation, idle_count) == 0x1535);
static_assert(offsetof(Nation, idle_cap) == 0x1537);
static_assert(offsetof(Nation, idle_units) == 0x1539);
static_assert(offsetof(Nation, station_count) == 0x153d);
static_assert(offsetof(Nation, station_cap) == 0x153f);
static_assert(offsetof(Nation, stations) == 0x1541);
#endif

}  // namespace joan::orig
