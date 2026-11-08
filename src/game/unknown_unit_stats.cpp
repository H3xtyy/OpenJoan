// game/unknown_unit_stats.cpp: functions whose original source file is unknown (formerly game/_unplaced/unit_stats.cpp;
// re/attribution.csv: between ww/unit.cpp and ww/unitres.cpp). They move to their original file when evidence names it
// (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/game_units.cpp (game/units1)
namespace {
namespace game_units {

struct H {
    std::int32_t hit_points_max() {  // 0x00523320
        std::int32_t bonus = 0;
        if (jo::self<jo::Unit>(this)->modifiers != nullptr)
            bonus = joan::orig::arg::as<std::int32_t>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->sum(1, static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id)));  // 0x00497e10
        return jo::self<jo::Unit>(this)->hit_points_base + bonus;
    }
};

#define U1(name, addr, fn) JOAN_REPLACE("game/units1", name, addr, fn)
U1("Unit::hit_points_max", 0x00523320, &H::hit_points_max);
#undef U1

}  // namespace game_units
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {

struct H {
    // 0x00523700
    std::int32_t value_149() {
        std::int32_t v = 0;
        if (jo::self<jo::Unit>(this)->modifiers != nullptr)
            v = joan::orig::arg::as<std::int32_t>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->sum(2, static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id)));  // 0x00497e10
        return jo::self<jo::Unit>(this)->base_max_ep + v;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Unit::value_149", 0x00523700, &H::value_149);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

struct H {
    // 0x005238c0
    std::int32_t level_sum() {
        std::int32_t extra = 0;
        if (jo::self<jo::Unit>(this)->modifiers != nullptr) {
            extra = joan::orig::arg::as<std::int32_t>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->sum(5, static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id)));  // 0x00497e10
        }
        return static_cast<std::int32_t>(jo::self<jo::Unit>(this)->stats[2]) + extra;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Unit::level_sum", 0x005238c0, &H::level_sum);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

struct H {
    // 0x005239e0
    void* slot(std::int32_t i) {
        return jo::self<jo::UnitTable>(this)->entries + (static_cast<std::ptrdiff_t>(i) - 1);  // 4-byte UnitTableEntry records
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("Table::slot", 0x005239e0, &H::slot);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
namespace {
namespace game_b100_08 {

using i16 = std::int16_t;

// ------------------------------------------------------------------------------- thiscall
struct H {
    // 0x00523e50
    bool has_active_child(u32, u32, u32) {
        if (jo::self<jo::Unit>(this)->cell == nullptr) return false;
        return static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_world_cell(jo::self<jo::Unit>(this)->cell)->path_data_flags())) != 0;  // 0x00560060
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("Obj::has_active_child", 0x00523e50, &H::has_active_child);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b107_17.cpp (game/b107_17)
namespace {
namespace game_b107_17 {

void JOAN_CDECL init_523f50() { joan::orig::init_523f80(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b107_17", name, addr, fn)
B("init_523f50", 0x00523f50, &init_523f50);
#undef B

}  // namespace game_b107_17
}  // namespace

// ---------------------------------------- from modules/game_b106_03.cpp (game/b106_03)
namespace {
namespace game_b106_03 {

// ---- 0x00523f80 ---------------------------------------------------------------------------
void JOAN_CDECL init_523f80() { global<float>(d::g_a3cd7c) = global<float>(d::g_a3cd78) / global<float>(d::flt_2); }

#define B(name, addr, fn) JOAN_REPLACE("game/b106_03", name, addr, fn)
B("init_523f80", 0x00523f80, &init_523f80);
#undef B

}  // namespace game_b106_03
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

void JOAN_CDECL init_a3cd80() {  // 0x00524a80
    global<float>(d::g_a3cd80) = global<float>(d::g_a3cd78) / global<float>(d::flt_3);  // 3.0f
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("init_a3cd80", 0x00524a80, &init_a3cd80);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
namespace {
namespace game_b102_05 {

void JOAN_CDECL init_524af0() { joan::orig::div_a3cd78(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("init_524af0", 0x00524af0, &init_524af0);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

void JOAN_CDECL div_a3cd78() {  // 0x00524bd0
    global<float>(d::g_a3cd84) = global<float>(d::g_a3cd78) / global<float>(d::flt_4);
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("Globals::div_a3cd78", 0x00524bd0, &div_a3cd78);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

using u8 = std::uint8_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    void* reset_7() {  // 0x00524c50
        joan::orig::as_res_idx(reinterpret_cast<u8*>(this) + sizeof(jo::UnitRes))->construct();  // 0x004179f0 (+7 on x86: the ResIdx after the UnitRes 0xa48d88)
        memset_(this, 0, static_cast<u32>(sizeof(jo::UnitRes)));  // 7 on x86
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("Struct7::reset", 0x00524c50, &H::reset_7);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::call::thiscall;

using joan::call::thiscall;
struct H {
    // 0x005237e0 / 0x00523860 / 0x00523920
    std::int32_t stat_with_bonus(std::int32_t kind, std::int32_t stat) {
        std::int32_t v = 0;
        if (jo::self<jo::Unit>(this)->modifiers != nullptr)
            v = joan::orig::arg::as<std::int32_t>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->sum(joan::orig::arg::i32(kind), static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id)));  // 0x00497e10
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->stats[stat])) +
                                         static_cast<std::uint32_t>(v));
    }
    std::int32_t stat_3() { return stat_with_bonus(3, 0); }
    std::int32_t stat_4() { return stat_with_bonus(4, 1); }
    std::int32_t stat_6() { return stat_with_bonus(6, 3); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Unit::stat_3", 0x005237e0, &H::stat_3);
B("Unit::stat_4", 0x00523860, &H::stat_4);
B("Unit::stat_6", 0x00523920, &H::stat_6);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b103_06.cpp (game/b103_06)
#define M "game/b103_06"

namespace {
namespace game_b103_06 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- static initialisers (direct calls, as the original) ----
u32 JOAN_CDECL init_523f00() {
    joan::orig::init_a3cd78();
    return 0;  // void callee; _initterm ignores the eax
}
u32 JOAN_CDECL init_524a30() {
    joan::orig::init_a3cd80();
    return 0;  // void callee; _initterm ignores the eax
}

JOAN_REPLACE(M, "init_523f00", 0x00523f00, &init_523f00);
JOAN_REPLACE(M, "init_524a30", 0x00524a30, &init_524a30);

}  // namespace game_b103_06
}  // namespace
#undef M

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Unit::stat_3() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::stat_3>(this)); }  // 0x5237e0
auto joan::orig::Unit::stat_4() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::stat_4>(this)); }  // 0x523860
auto joan::orig::Unit::level_sum() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_04::H::level_sum>(this)); }  // 0x5238c0
auto joan::orig::Unit::stat_6() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::stat_6>(this)); }  // 0x523920
auto joan::orig::Table::slot(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_02::H::slot>(this, a0)); }  // 0x5239e0
auto joan::orig::Unit::cell_has_path_data(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b100_08::H::has_active_child>(this, a0, a1, a2)); }  // 0x523e50
auto joan::orig::init_523f80() -> void { joan::bind::free<&::game_b106_03::init_523f80>(); }  // 0x523f80
auto joan::orig::init_a3cd80() -> void { joan::bind::free<&::game_b99_05::init_a3cd80>(); }  // 0x524a80
auto joan::orig::div_a3cd78() -> void { joan::bind::free<&::game_b94_04::div_a3cd78>(); }  // 0x524bd0
auto joan::orig::UnitTypes::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_04::H::reset_7>(this)); }  // 0x524c50
