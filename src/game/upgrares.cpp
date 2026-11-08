// game/upgrares.cpp: functions of the original d:\ww\upgrares.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cstddef>
#include <cmath>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

void* const kMem = obj(d::g_mem);

using u16 = std::uint16_t;

}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
//
//      0x00531690 UpgraRes::free           (thiscall, ret 0)
namespace {
namespace game_b93_02 {

using i8 = std::int8_t;

JOAN_COUNTER(c_upgra_free, "UpgraRes::free");

void mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70
using joan::orig::helpers::line_of;

struct H {

    void upgrares_free() {  // 0x00531690
        c_upgra_free.hit();
        if (jo::view<i8>(jo::self<jo::UpgradeRes>(this)->loaded) == 0) return;
        if (jo::self<jo::UpgradeRes>(this)->items != nullptr) {
            mem_free(jo::self<jo::UpgradeRes>(this)->items, str(d::s_d_ww_upgrares_cpp), line_of(d::g_a085d4, 6));  // "d:\ww\upgrares.cpp"
            jo::self<jo::UpgradeRes>(this)->items = nullptr;
        }
        if (jo::self<jo::UpgradeRes>(this)->units != nullptr) {
            for (i32 i = 1; i <= jo::self<jo::UpgradeRes>(this)->unit_count; ++i) {
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_upgrade_res(this)->unit_upgrade_list(joan::orig::arg::i32(i)));  // 0x00531e70
                joan::orig::as_unit_upgrade_list(e)->free();                         // 0x00530d00
            }
            mem_free(jo::view<void*>(jo::self<jo::UpgradeRes>(this)->units), str(d::s_d_ww_upgrares_cpp), line_of(d::g_a085d4, 0x12));
            jo::self<jo::UpgradeRes>(this)->units = nullptr;
        }
        jo::self<jo::UpgradeRes>(this)->loaded = 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("UpgraRes::free", 0x00531690, &H::upgrares_free);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
//
//      0x00531790 UpgradeTable::load      (thiscall, ret 0) - 'UPITEM' rows of 100 bytes.
namespace {
namespace game_b99_05 {

using joan::call::cdecl_call;

void* const kTables = obj(d::g_game_set);
void* const kText = obj(d::g_misc);

JOAN_COUNTER(c_upgrade_table, "UpgradeTable::load");

using joan::orig::helpers::memset_;  // memset
i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }                       // atoi

void* game_alloc(u32 size, const char* file, i32 line) {
    return joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058e7b0
}
// Table text field: 0x00590170(src, n).
char* text_field(const void* src, i32 n) { return joan::orig::arg::as<char*>(joan::orig::as_misc(kText)->rtrim_field_590170(joan::orig::arg::str(src), joan::orig::arg::i32(n))); }
// Copy table text field: 0x00590210(dst, src, n).
void text_copy(void* dst, const void* src, i32 n) { joan::orig::as_misc(kText)->copy_field_590210(joan::orig::arg::str(dst), joan::orig::arg::str(src), joan::orig::arg::i32(n)); }

struct H {

    // ------------------------------------------------------------------------ 0x00531790
    void upgrade_table_load() {
        c_upgrade_table.hit();
        void* const rec = joan::orig::arg::as<void*>(joan::orig::as_game_set(kTables)->open_db_58b180(str(d::s_UPITEM)));  // 0x0058b180 'UPITEM'
        jo::self<jo::UpgradeRes>(this)->item_count = joan::orig::arg::as<i32>(joan::orig::as_database(rec)->rec_count());                              // 0x00415280
        jo::self<jo::UpgradeRes>(this)->items = static_cast<jo::UpgradeItem*>(game_alloc(static_cast<u32>(jo::self<jo::UpgradeRes>(this)->item_count * static_cast<i32>(sizeof(jo::UpgradeItem))), str(d::s_d_ww_upgrares_cpp),
                                 static_cast<i32>(global<i16>(d::g_a085d8)) + 9));
        memset_(jo::self<jo::UpgradeRes>(this)->items, 0, static_cast<u32>(jo::self<jo::UpgradeRes>(this)->item_count * static_cast<i32>(sizeof(jo::UpgradeItem))));
        for (i32 i = 0; i < jo::self<jo::UpgradeRes>(this)->item_count; ++i) {
            u8* const row = joan::orig::arg::as<u8*>(joan::orig::as_database(rec)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            jo::UpgradeItem* const e = jo::self<jo::UpgradeRes>(this)->items + i;
            e->id = static_cast<u16>(i + 1);
            text_copy(e->name, row + 0x10, 0x20);
            text_copy(e->description, row + 0x30, 0x20);
            char* const t = text_field(row, 0x10);
            jo::view<u16>(e->type) = static_cast<u16>(joan::orig::arg::as<i32>(joan::orig::as_upgrade_res(this)->attribute_id(joan::orig::arg::str(t))));  // 0x00531ba0
            for (i32 j = 0; j < 3; ++j) text_copy(e->bonus_text + j * 7, row + 0x50 + j * 6, 6);
            jo::view<u16>(e->cost) = static_cast<u16>(atoi_(text_field(row + 0x62, 2)));
            text_copy(e->required, row + 0x64, 6);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("UpgradeTable::load", 0x00531790, &H::upgrade_table_load);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b101_08.cpp (game/b101_08)
//
//    - UpgradeRes::load 0x005319a0 (ret 0): "UPGRADE" table (0x0058b180) -> count (0x00415280)
//      at +8, heap array of count * 0x35 bytes at +0xc ("d:\ww\upgrares.cpp", line
//      [0xa085dc] + 9), zeroed; each record: word id = i + 1, name (0x20) at +2, 8 bytes at
//      +0x23 (0x00590210 copies), then 0x00530cc0 on every record. Returns this (eax).
namespace {
namespace game_b101_08 {

void* const kHeap = obj(d::g_mem);
void* const kTokens = obj(d::g_misc);  // also the random generator object

JOAN_COUNTER(c_upgrade_load, "UpgradeRes::load");

struct H {

    // ------------------------------------------------------------------ 0x005319a0
    void* upgrade_load() {
        c_upgrade_load.hit();
        void* const res = joan::orig::arg::as<void*>(joan::orig::as_game_set(obj(d::g_game_set))->open_db_58b180(str(d::s_UPGRADE)));  // 0x0058b180 "UPGRADE"
        jo::self<jo::UpgradeRes>(this)->unit_count = joan::orig::arg::as<i32>(joan::orig::as_database(res)->rec_count());                                         // 0x00415280
        const i32 line = static_cast<i32>(global<i16>(d::g_a085dc)) + 9;
        jo::view<void*>(jo::self<jo::UpgradeRes>(this)->units) = joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(static_cast<u32>(jo::self<jo::UpgradeRes>(this)->unit_count) * static_cast<u32>(sizeof(jo::UnitTable))), joan::orig::arg::str(// 0x0058e7b0
                                        str(d::s_d_ww_upgrares_cpp)), joan::orig::arg::i32(line));
        std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::UpgradeRes>(this)->units)), i32{0}, joan::orig::arg::u32(static_cast<u32>(jo::self<jo::UpgradeRes>(this)->unit_count) * static_cast<u32>(sizeof(jo::UnitTable))));  // memset
        for (i32 i = 0; i < jo::self<jo::UpgradeRes>(this)->unit_count; ++i) {
            u8* const src = joan::orig::arg::as<u8*>(joan::orig::as_database(res)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            jo::UnitTable* const rec = &jo::self<jo::UpgradeRes>(this)->units[i];
            rec->id = static_cast<u16>(i + 1);
            joan::orig::as_misc(kTokens)->copy_field_590210(joan::orig::arg::str(rec->name), joan::orig::arg::str(src), i32{0x20});        // 0x00590210
            joan::orig::as_misc(kTokens)->copy_field_590210(joan::orig::arg::str(rec->table_name), joan::orig::arg::str(src + 0x20), i32{8});  // 0x00590210
        }
        for (i32 i = 0; i < jo::self<jo::UpgradeRes>(this)->unit_count; ++i) {
            jo::UnitTable* const rec = &jo::self<jo::UpgradeRes>(this)->units[i];
            joan::orig::as_unit_upgrade_list(rec)->reload();  // 0x00530cc0
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_08", name, addr, fn)
B("UpgradeRes::load", 0x005319a0, &H::upgrade_load);
#undef B

}  // namespace game_b101_08
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
//
//      0x00531620 UpgraRes::dtor          (ret 0) - 0x00531690.
//
//  - 0x00531620 / 0x00560fe0 / 0x00546140 return void (callers ignore eax; 0x00531690 is
//    itself replaced as void in game/b93_02).
namespace {
namespace game_b97_04 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x00531620
    void upgrares_dtor() { joan::orig::as_upgrade_res(this)->free(); }  // 0x00531690
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("UpgraRes::dtor", 0x00531620, &H::upgrares_dtor);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
//
//    - Obj::init_531650 0x00531650 (ret 0) -> this: 0x531690, 0x531790, 0x5319a0,
//      byte +0x10 = 1.
namespace {
namespace game_b102_05 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {

    // ------------------------------------------------------------------ 0x00531650
    void* init_531650() {
        joan::orig::as_upgrade_res(this)->free();  // 0x00531690
        joan::orig::as_upgrade_res(this)->load_items();  // 0x00531790
        joan::orig::as_upgrade_res(this)->load_upgrades();  // 0x005319a0
        jo::self<jo::UpgradeRes>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("Obj::init_531650", 0x00531650, &H::init_531650);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
//
//    - attribute_id 0x00531ba0 (name, ret 4): _stricmp table STRENGTH .. HORSE_ATTACK_B.
namespace {
namespace game_b96_04 {

using joan::call::cdecl_call;
using joan::img::str;

using i32 = std::int32_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_attr_id, "attribute_id");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x00531ba0
    i32 attribute_id(const char* name) {
        c_attr_id.hit();
        struct Entry {
            img::Addr s;
            i32 id;
        };
        static const Entry kTable[] = {
            {d::s_STRENGTH, 0x1},   // STRENGTH
            {d::s_DEXTERITY, 0x2},   // DEXTERITY
            {d::s_DEFENSE, 0x3},   // DEFENSE
            {d::s_CHARISMA, 0x4},   // CHARISMA
            {d::s_LEADERSHIP, 0x5},   // LEADERSHIP
            {d::s_HIT_POINT, 0x6},   // HIT_POINT
            {d::s_ENERGY_POINT, 0x7},   // ENERGY_POINT
            {d::s_NEW_COMBO, 0x9},   // NEW_COMBO
            {d::s_JUMP_ATTACK_B, 0xa},   // JUMP_ATTACK_B
            {d::s_DOWN_ATTACK, 0xb},   // DOWN_ATTACK
            {d::s_RUSH_ATTACK, 0xc},   // RUSH_ATTACK
            {d::s_ENERGY_ATTACK, 0xd},   // ENERGY_ATTACK
            {d::s_RANGE_ATTACK, 0xe},   // RANGE_ATTACK
            {d::s_HORSE_ATTACK_A, 0xf},   // HORSE_ATTACK_A
            {d::s_HORSE_ATTACK_B, 0x10},  // HORSE_ATTACK_B
        };
        for (u32 k = 0; k < sizeof kTable / sizeof kTable[0]; ++k)
            if (stricmp_(name, str(kTable[k].s)) == 0) return kTable[k].id;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("attribute_id", 0x00531ba0, &H::attribute_id);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
//
//      0x00531e70 UpgradeRes::get_unit_upgrade_list_res(i)    thiscall, ret 4
namespace {
namespace game_b90_02 {

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

struct H {

    // 0x00531e70
    void* upgrade_list_res(std::int32_t i) {
        if (i < 1 || i > jo::self<jo::UpgradeRes>(this)->unit_count)
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_UpgradeRes_get_unit_upgrade));  // 0x00586cb0
        return jo::self<jo::UpgradeRes>(this)->units + (i - 1);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("UpgradeRes::get_unit_upgrade_list_res", 0x00531e70, &H::upgrade_list_res);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;


JOAN_COUNTER(c_find_name, "Table::find_name");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {

    // ------------------------------------------------------------------------ 0x00531b20
    i32 find_name(const char* name) {
        c_find_name.hit();
        for (i32 i = 0; i < jo::self<jo::UpgradeRes>(this)->item_count; ++i) {
            if (stricmp_(jo::self<jo::UpgradeRes>(this)->items[i].name, name) == 0) return i + 1;
        }
        return 0;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("Table::find_name", 0x00531b20, &H::find_name);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
namespace {
namespace game_b100_08 {

using joan::call::cdecl_call;

using i32 = std::int32_t;


// ------------------------------------------------------------------------------- thiscall
struct H {

    // 0x00531df0
    i32 find(const char* name) {
        for (i32 i = 0; i < jo::self<jo::UpgradeRes>(this)->unit_count; ++i) {
            if (joan::crt::stricmp(joan::orig::arg::str(jo::self<jo::UpgradeRes>(this)->units[i].name), joan::orig::arg::str(name)) == 0) return i + 1;  // _stricmp
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("NameTable::find", 0x00531df0, &H::find);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b88_05.cpp (game/b88_05)
namespace {
namespace game_b88_05 {

using joan::call::cdecl_call;

using joan::call::thiscall;
using joan::img::str;

using joan::img::obj;


JOAN_COUNTER(c_is_special, "UpgradeRes::is_special");

struct H {

    // ------------------------------------------------------------ 0x00531ed0 (ret 4)
    std::uint8_t is_special(std::int32_t index) {
        c_is_special.hit();
        if (index < 1 || index > jo::self<jo::UpgradeRes>(this)->item_count) {
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_UpgradeRes_operator));  // 0x00586cb0
        }
        const jo::UpgradeItem* const e = joan::orig::arg::as<jo::UpgradeItem*>(joan::orig::as_table(this)->entry(joan::orig::arg::u32(index)));  // 0x0050d240
        const std::uint32_t k = static_cast<std::uint32_t>(static_cast<std::int32_t>(e->type) - 1);
        if (k > 0xf) return 0;
        return k >= 7 ? 1 : 0;  // byte table 0x531f50: 0..6 -> 0 / 2 (xor al), 7..15 -> 1
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_05", name, addr, fn)
B("UpgradeRes::is_special", 0x00531ed0, &H::is_special);
#undef B

}  // namespace game_b88_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::UpgradeRes::free() -> void { joan::bind::member<&::game_b93_02::H::upgrares_free>(this); }  // 0x531690
auto joan::orig::UpgradeRes::load_items() -> void { joan::bind::member<&::game_b99_05::H::upgrade_table_load>(this); }  // 0x531790
auto joan::orig::UpgradeRes::load_upgrades() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b101_08::H::upgrade_load>(this)); }  // 0x5319a0
auto joan::orig::Table::find_name_531b20(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_04::H::find_name>(this, a0)); }  // 0x531b20
auto joan::orig::UpgradeRes::attribute_id(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_04::H::attribute_id>(this, a0)); }  // 0x531ba0
auto joan::orig::NameTable::find_531df0(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_08::H::find>(this, a0)); }  // 0x531df0
auto joan::orig::UpgradeRes::unit_upgrade_list(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_02::H::upgrade_list_res>(this, a0)); }  // 0x531e70
auto joan::orig::UpgradeRes::is_special(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b88_05::H::is_special>(this, a0)); }  // 0x531ed0
