// game/unknown_esfuncs.cpp: functions whose original source file is unknown (formerly game/_unplaced/esfuncs.cpp;
// re/attribution.csv: between ww/uti3d/esstate2.cpp and ww/uti3d/lflare.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cstddef>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/cxx_eh.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../en3d/scene_lists.hpp"

namespace jo = joan::orig;

namespace {
// v3d "M" results (length, length_sq, dot_product): the replacements (src/lib/v3d/v3d87.cpp) return double. On x86
// the caller reads st0 as the float it always used (same code as before); x64 returns the double in xmm0, so it is
// read as a double there.
template <class... A>
float v3d_float(joan::img::Addr slot, A... args) {
#if UINTPTR_MAX == 0xffffffffu
    return joan::call::import_call<float>(slot, args...);
#else
    return static_cast<float>(joan::call::import_call<double>(slot, args...));
#endif
}
}  // namespace

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using i8 = std::int8_t;

struct Obj {};

using joan::hook::global;

using u16 = std::uint16_t;

// Virtual call through vtable slot `offset` (thiscall, no args).
template <class R>
R vcall0(void* o, u32 offset) {
    using V = R (Obj::*)();
    JOAN_MEMBER_POINTER_CHECK(V);
    V method{};
    std::memcpy(&method, &joan::call::vtable_of(o)[joan::call::x86_vslot(offset)], sizeof(void*));  // the address word (JOAN_MEMBER_POINTER_CHECK)  // slot offset / 4 (x86 byte offset)
    return (static_cast<Obj*>(o)->*method)();
}

using joan::hook::x87::ftol;

using joan::img::str;

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using joan::hook::x87::Fpu;

using joan::call::import_call;

void* const kList8f50 = obj(d::g_item_res);

void* const kFlags = obj(d::g_game_state);

void* const kCameras = obj(d::g_camera_man);

using joan::orig::helpers::stricmp_;  // _stricmp
float heading(const void* unit) { return joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading()); }  // 0x004ff850 (fld [+0x1a])

}  // namespace

// ---------------------------------------- from modules/game_b105_18.cpp (game/b105_18)
namespace {
namespace game_b105_18 {

void* const kUnits = obj(d::g_units);
i32 sx(u32 raw) { return static_cast<i32>(static_cast<i16>(raw)); }  // movsx word

i32 unit_dead(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_at(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i8 unit_busy(u8* u) { return joan::orig::arg::as<i8>(joan::orig::as_game(u)->byte_2e1()); }               // 0x00432b90
void unit_order(u8* u, u32 a, u32 b) { joan::orig::as_unit(u)->set_stance(joan::orig::arg::u32(a), joan::orig::arg::u32(b)); }  // 0x00519800

JOAN_COUNTER(c_unit_reset, "unit_reset_655420");
JOAN_COUNTER(c_unit_order, "unit_order_65daf0");
JOAN_COUNTER(c_units_kind, "units_order_kind_65dc20");

// ------------------------------------------------------------------ static initialisers
void JOAN_CDECL init_650fb0() { joan::orig::init_a6a250(); }
void JOAN_CDECL init_651040() { joan::orig::ratio_half(); }
void JOAN_CDECL init_6510b0() { joan::orig::init_a6a25c(); }
void JOAN_CDECL init_6511a0() { joan::orig::ratio_quarter(); }

u8 JOAN_CDECL true_651090() { return 1; }                // 0x00651090 (dead; mov al,1)
u8 JOAN_CDECL true_651410() { return 1; }                // 0x00651410 (dead; mov al,1)

// 0x00654020
u32 JOAN_CDECL unit_call_654020(u32 a, u32 b) {
    void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_place_array))->get_c70(joan::orig::arg::i32(sx(a))));  // 0x00456c70
    return joan::orig::arg::as<u32>(joan::orig::as_place(o)->count_enemy_units(joan::orig::arg::u32(sx(b))));                           // 0x0049e500
}

// 0x00655420
i32 JOAN_CDECL unit_reset_655420(u32 id_raw) {
    c_unit_reset.hit();
    const i32 id = sx(id_raw);
    if (unit_dead(id) != 0) return 0;
    u8* const u = unit_at(id);
    joan::orig::as_unit(u)->release_slot();  // 0x00528300
    if (jo::as_unit(u)->mount_id > 0) {
        if (unit_dead(jo::as_unit(u)->mount_id) == 0) joan::orig::as_units(kUnits)->remove_51e1f0(joan::orig::arg::i32(jo::as_unit(u)->mount_id));  // 0x0051e1f0
    }
    if (jo::as_unit(u)->rank >= 4) joan::orig::as_unit(u)->clear_markers();  // 0x0050b390
    joan::orig::as_units(kUnits)->remove_51e1f0(joan::orig::arg::i32(id));                 // 0x0051e1f0
    return 1;
}

// 0x0065daf0
i32 JOAN_CDECL unit_order_65daf0(i32 id, u32 a) {
    c_unit_order.hit();
    if (unit_dead(id) != 0) return 0;
    if (unit_busy(unit_at(id)) != 0) {
        if (static_cast<i32>(jo::as_unit(unit_at(id))->anim_kind) == 7) return 1;
    }
    unit_order(unit_at(id), a, 1);
    return 1;
}

// 0x0065dc20
i32 JOAN_CDECL units_order_kind_65dc20(i32 kind, u32 a, u32 b) {
    c_units_kind.hit();
    for (i32 i = joan::orig::as_dynarray(kUnits)->size(); i > 0; --i) {  // 0x00417ad0
        if (unit_dead(i) != 0) continue;
        if (static_cast<i32>(jo::as_unit(unit_at(i))->group_id) != kind) continue;
        if (unit_busy(unit_at(i)) != 0) continue;
        unit_order(unit_at(i), a, static_cast<u32>(sx(b)));  // word arg, upper half garbage
    }
    return 1;
}

struct H {
    void nop_661020(u32) {}                                        // 0x00661020
    u8 true_661040(u32) { return 1; }                              // 0x00661040 (mov al,1)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_18", name, addr, fn)
B("init_650fb0", 0x00650fb0, &init_650fb0);
B("init_651040", 0x00651040, &init_651040);
B("true_651090", 0x00651090, &true_651090);
B("init_6510b0", 0x006510b0, &init_6510b0);
B("init_6511a0", 0x006511a0, &init_6511a0);
B("true_651410", 0x00651410, &true_651410);
B("unit_call_654020", 0x00654020, &unit_call_654020);
B("unit_reset_655420", 0x00655420, &unit_reset_655420);
B("unit_order_65daf0", 0x0065daf0, &unit_order_65daf0);
B("units_order_kind_65dc20", 0x0065dc20, &units_order_kind_65dc20);
B("Obj::nop_661020", 0x00661020, &H::nop_661020);
B("Obj::true_661040", 0x00661040, &H::true_661040);
#undef B

}  // namespace game_b105_18
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

void* const kUnits = obj(d::g_items);
void* const kTypes = obj(d::g_item_res);

JOAN_COUNTER(c_script_attach, "script_attach");

using joan::orig::helpers::game_new;                              // operator new

void JOAN_CDECL init_a6a250() {  // 0x00650fd0
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a6a250) = static_cast<float>(t * global<double>(d::dbl_4));  // 4.0
}

i32 JOAN_CDECL unit_byte_d0(u32 id) {  // 0x00652660
    if (joan::orig::as_units(kUnits)->list.empty_at(joan::orig::arg::i32(id)) != 0) return -1;  // 0x00425d50
    u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
    if (vcall0<u8*>(unit, 0x4c) == nullptr) return -1;
    u8* const r = vcall0<u8*>(unit, 0x4c);
    return static_cast<i32>(jo::view<i8>(jo::as_gate(r)->attacked));  // slot 0x4c returns the Gate
}

u32 JOAN_CDECL unit_value_2ba(i16 id) {  // 0x00652280
    u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(static_cast<i32>(id)));  // 0x0041b3c0
    u8* const owner = joan::orig::arg::as<u8*>(joan::orig::as_unit(unit)->leader());                                    // 0x004216d0
    if (jo::view<jo::TaskSlot>(jo::as_unit(unit)->task).kind != 0) return jo::view<jo::TaskSlot>(jo::as_unit(unit)->task).u32_15;
    if (owner != nullptr) return jo::view<jo::TaskSlot>(jo::as_unit(owner)->task).u32_15;
    return 0;
}

u32 JOAN_CDECL script_attach(const char* a, const char* b) {  // 0x00651ad0
    c_script_attach.hit();
    const u32 id = joan::orig::arg::as<u32>(joan::orig::as_name_table(kTypes)->find_4a36c0(a));                       // 0x004a36c0
    const u32 key = joan::orig::arg::as<u32>(joan::orig::as_units(kUnits)->create_49ea90(joan::orig::arg::u32(id)));                     // 0x0049ea90
    u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(key)));  // 0x00425d20
    u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(static_cast<i16>(id))));  // 0x00425ce0
    const u32 r20 = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(b));             // 0x004b91a0
    const u32 r24 = joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(r20)));           // 0x004b67e0
    if (jo::view<i8>(jo::as_item_def(type)->pickable) != 0) {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::WorldItem)));  // 0x52 on x86
        void* const o = mem != nullptr ? joan::orig::as_world_item(mem)->construct() : nullptr;  // 0x00456eb0
        u8* const t1 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(static_cast<i16>(id))));  // 0x00425ce0
        const i16 w2a = jo::view<i16>(jo::as_item_def(t1)->stats[3]);
        u8* const t2 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(static_cast<i16>(id))));  // 0x00425ce0
        const i16 w36 = jo::view<i16[2]>(jo::as_item_def(t2)->min_level)[0];
        joan::orig::as_item_stack(o)->init(joan::orig::arg::i16(static_cast<i16>(id)), joan::orig::arg::i16(w36), joan::orig::arg::i16(w2a), 0u, 0u, 0u);  // 0x00456cd0
        jo::view<u16>(jo::as_world_item(o)->object_id) = jo::view<u16>(jo::as_map_object(unit)->index);
        jo::as_world_item(o)->side = static_cast<u16>(r24);
        joan::orig::as_world_item(o)->set_state(global<jo::World*>(d::g_world));  // 0x00457120
        jo::view<void*>(jo::as_map_object(unit)->world_item) = o;
    }
    jo::view<u16>(jo::as_map_object(unit)->side) = static_cast<u16>(r24);
    return key;
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("init_a6a250", 0x00650fd0, &init_a6a250);
B95("script_attach", 0x00651ad0, &script_attach);
B95("unit_value_2ba", 0x00652280, &unit_value_2ba);
B95("unit_byte_d0", 0x00652660, &unit_byte_d0);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b93_04.cpp (game/b93_04)
namespace {
namespace game_b93_04 {

void* const kRandom = obj(d::g_misc);
void* const kUnits = obj(d::g_items);
void* const kUnitTable = obj(d::g_units);
void* const kGame = obj(d::g_game_a5d380);

JOAN_COUNTER(c_unit_value, "script_unit_value");
JOAN_COUNTER(c_script_flag, "script_flag");

// Unit table 0xa47c08: 0x00428b70 "missing", 0x0041b3c0 lookup.
i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
void* unit_get(i32 id) { return joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------ 0x00654ab0
    void* set_4a(i16 v) {
        jo::self<jo::CamZones>(this)->hand_back = v;
        return this;
    }

    // ------------------------------------------------------------------ 0x00660ff0
    void* set_c(i32 v) {
        jo::self<jo::ScriptNode>(this)->value_type = v;
        return this;
    }
};

// ---------------------------------------------------------------------------- 0x00651010
static i32 JOAN_CDECL script_random(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540

// ---------------------------------------------------------------------------- 0x00652f80
static i32 JOAN_CDECL script_unit_value(i16 id, i32 b) {
    c_unit_value.hit();
    if (unit_missing(static_cast<i32>(id)) != 0 || unit_missing(b) != 0) return 0;
    void* const unit = unit_get(static_cast<i32>(id));
    const float v = joan::orig::arg::as<float>(joan::orig::as_unit(unit)->heading_to_unit(joan::orig::arg::i32(b)));  // 0x00505960 (st0 = a float load)
    return joan::hook::x87::ftol(static_cast<double>(v));
}

// ---------------------------------------------------------------------------- 0x00656af0
static i32 JOAN_CDECL script_unit_384(i16 id) {
    if (unit_missing(static_cast<i32>(id)) != 0) return -1;
    return jo::as_unit(unit_get(static_cast<i32>(id)))->weapon;
}

// ---------------------------------------------------------------------------- 0x00656b50
static i32 JOAN_CDECL script_unit_38c(i16 id) {
    if (unit_missing(static_cast<i32>(id)) != 0) return -1;
    return static_cast<i32>(jo::as_unit(unit_get(static_cast<i32>(id)))->crew_slots_ready);
}

// ---------------------------------------------------------------------------- 0x00657610
static i32 JOAN_CDECL script_unit_508b90(i16 id) {
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    void* const unit = unit_get(static_cast<i32>(id));
    joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065b0e0
static i32 JOAN_CDECL game_state_is_7() {
    if (joan::orig::arg::as<i8>(joan::orig::as_shop(kGame)->active()) == 0) return 1;  // 0x0042ced0
    return joan::orig::arg::as<i32>(joan::orig::as_shop(kGame)->state()) == 7 ? 1 : 0;  // 0x0065afb0
}

// ---------------------------------------------------------------------------- 0x0065b570
static i32 JOAN_CDECL units_lookup(const char* p) {
    if (p == nullptr) return 0;
    return joan::orig::arg::as<i32>(joan::orig::as_item_list(kUnits)->find_by_name(p));  // 0x0049f420
}

// ---------------------------------------------------------------------------- 0x0065dfa0
static i32 JOAN_CDECL script_unit_388(i32 id) {
    if (unit_missing(id) != 0) return 0;
    return jo::as_unit(unit_get(id))->weapon_slot;
}

// ---------------------------------------------------------------------------- 0x0065f600
static i32 JOAN_CDECL script_flag(i32 a) {
    c_script_flag.hit();
    if (joan::orig::arg::as<i8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) == 0) {  // 0x004301c0
        if (joan::orig::arg::as<i8>(joan::orig::as_shop(kGame)->active()) == 0) return 1;  // 0x0042ced0
    }
    if (a == 0) return 0;
    return global<i8>(d::g_a5d3c0) != 0 ? 1 : 0;
}

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("script_random", 0x00651010, &script_random);
B93("script_unit_value", 0x00652f80, &script_unit_value);
B93("Obj::set_4a", 0x00654ab0, &H::set_4a);
B93("script_unit_384", 0x00656af0, &script_unit_384);
B93("script_unit_38c", 0x00656b50, &script_unit_38c);
B93("script_unit_508b90", 0x00657610, &script_unit_508b90);
B93("game_state_is_7", 0x0065b0e0, &game_state_is_7);
B93("units_lookup", 0x0065b570, &units_lookup);
B93("script_unit_388", 0x0065dfa0, &script_unit_388);
B93("script_flag", 0x0065f600, &script_flag);
B93("Obj::set_c", 0x00660ff0, &H::set_c);
#undef B93

}  // namespace game_b93_04
}  // namespace

// ---------------------------------------- from modules/game_b99_08.cpp (game/b99_08)
namespace {
namespace game_b99_08 {

using joan::orig::helpers::at;
// Virtual thiscall: vtable slot at byte offset `off`.
template <class R = void, class... A>
R vcall(void* o, std::int32_t off, A... args) {
    return thiscall<R>(reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(o)[joan::call::x86_vslot(off)]), o, args...);  // slot off / 4 (x86 byte offset); o: object of varying class
}

void* const kUnitTable = obj(d::g_units);
void* const kObjects = obj(d::g_items);
void* const kBuildings = obj(d::g_place_array);
void* const kPlaces = obj(d::g_places);

JOAN_COUNTER(c_remove_objects, "script_remove_objects");
JOAN_COUNTER(c_unit_set_2e, "script_unit_set_2e");
JOAN_COUNTER(c_place_setup, "script_place_setup");
JOAN_COUNTER(c_load_unit, "script_load_unit");
JOAN_COUNTER(c_set_2eb, "script_building_units_set_2eb");
JOAN_COUNTER(c_group_move, "script_unit_group_move");
JOAN_COUNTER(c_units_set_2f, "script_units_set_2f");
JOAN_COUNTER(c_stat_table, "script_stat_table");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 unit_count() { return joan::orig::as_dynarray(kUnitTable)->size(); }  // 0x00417ad0
u8* unit_at(i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(i))); }  // 0x0041b3f0

struct H {
    // ------------------------------------------------------------------ 0x00652a10
    void* set_word_20(i16 v) {
        jo::self<jo::WeatherSystem>(this)->active = v;
        return this;
    }

    // ------------------------------------------------------------------ 0x00655ba0
    void* set_16(i32 v) {
        jo::self<jo::BuyScreen>(this)->unit = v;
        return this;
    }
};

// ---------------------------------------------------------------------------- 0x00651060
static void JOAN_CDECL ratio_half() { global<float>(d::g_a6a258) = global<float>(d::g_a6a250) / global<float>(d::flt_2); }

// ---------------------------------------------------------------------------- 0x006511c0
static void JOAN_CDECL ratio_quarter() {
    global<float>(d::g_a6a260) = global<float>(d::g_a6a250) / global<float>(d::flt_4);
}

// ---------------------------------------------------------------------------- 0x00651ea0
static i32 JOAN_CDECL script_remove_objects(const char* key) {
    c_remove_objects.hit();
    i32 r = 0;
    do {
        r = joan::orig::arg::as<i32>(joan::orig::units_lookup(key));  // 0x0065b570
        if (r != 0) joan::orig::as_units(kObjects)->remove_49f370(joan::orig::arg::i32(r));  // 0x0049f370
    } while (r != 0);
    return 1;
}

// ---------------------------------------------------------------------------- 0x00652440
static i32 JOAN_CDECL script_slot1_d1() {
    if (joan::orig::arg::as<i32>(joan::orig::as_types(obj(d::g_a50530))->empty_at(1)) != 0) return -1;  // 0x00485410
    void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_a50530))->at_plus4(1u));    // 0x004853e0
    return static_cast<i32>(jo::as_gate(vcall<void*>(o, 0x4c))->state);  // slot 0x4c returns the Gate
}

// ---------------------------------------------------------------------------- 0x00653590
static i32 JOAN_CDECL script_unit_set_2e(i16 id, i8 v) {
    c_unit_set_2e.hit();
    if (static_cast<i32>(id) == 0) {
        for (i32 i = unit_count(); i > 0; --i) {
            u8* const e = unit_at(i);
            if (e == nullptr) continue;
            jo::as_unit(e)->frozen = v;
        }
        return 1;
    }
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    jo::as_unit(unit_get(static_cast<i32>(id)))->frozen = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00653c20
static i32 JOAN_CDECL script_not_a4fc98_flag() {
    return static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_shop(obj(d::g_buy))->is_open())) != 0 ? 0 : 1;  // 0x006c9040
}

// ---------------------------------------------------------------------------- 0x00653ef0
static u32 JOAN_CDECL script_building_49c240(i16 id, const char* a) {
    void* const b = joan::orig::arg::as<void*>(joan::orig::as_list(kBuildings)->get_c70(static_cast<i32>(id)));  // 0x00456c70
    return joan::orig::arg::as<u32>(joan::orig::as_place(b)->find_unit(a));                                        // 0x0049c240
}

// ---------------------------------------------------------------------------- 0x00654240
static i32 JOAN_CDECL script_building_4c4440(i16 id, i32 a, i32 b) {
    void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(kBuildings)->get_c70(static_cast<i32>(id)));  // 0x00456c70
    joan::orig::as_formation(o)->place_units(joan::orig::arg::i32(a), joan::orig::arg::u32(b));                                                 // 0x004c4440
    return 1;
}

// ---------------------------------------------------------------------------- 0x00654960
static i32 JOAN_CDECL script_place_467ea0(i16 id, i16 a, i16 b, i16 c, i32 d) {
    void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(kPlaces)->get_e(static_cast<i32>(id)));  // 0x0043c450
    void* const o = vcall<void*>(p, 0x18);
    joan::orig::as_unit(o)->push_order(static_cast<i32>(a), static_cast<i32>(b), static_cast<i32>(c), joan::orig::arg::i32(d));  // 0x00467ea0
    return 1;
}

// ---------------------------------------------------------------------------- 0x00654d70
static i32 JOAN_CDECL script_place_setup(i16 id, i32 name, i8 c, i32 /*unused +0x14*/, i32 a, i32 b, i32 d, i32 e,
                                      i32 g) {
    c_place_setup.hit();
    void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(kPlaces)->get_e(static_cast<i32>(id)));  // 0x0043c450
    u8* const o = vcall<u8*>(p, 0x18);
    const i32 r = joan::orig::arg::as<i32>(joan::orig::action::from_name(joan::orig::arg::str(name)));  // 0x004bbbc0
    jo::view<i32>(jo::as_building(o)->order_arg) = r;
    jo::view<i8>(jo::as_building(o)->order_b) = c;
    jo::view<i32>(jo::as_building(o)->order_c) = a;
    jo::view<i32>(jo::as_building(o)->order_d) = b;
    jo::view<i32>(jo::as_building(o)->order_e) = d;
    jo::view<i32>(jo::as_building(o)->order_f) = e;
    jo::view<i32>(jo::as_building(o)->delay) = g;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00655200
static i32 JOAN_CDECL script_load_unit(const char* a, const char* b) {
    c_load_unit.hit();
    if (static_cast<i32>(global<i8>(d::g_a58f95)) == 0) return 0;
    void* const mgr = joan::orig::arg::as<void*>(joan::orig::as_profile_list(obj(d::g_profile_list))->current());  // 0x004ca290
    if (mgr == nullptr) return 0;
    void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Squad)));  // 0x4b4 on x86; operator new
    u8* const unit = mem != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_squad(mem)->construct()) : nullptr;  // 0x004155e0
    if (joan::orig::arg::as<i32>(joan::orig::as_profile(mgr)->load_unit(a, joan::orig::arg::mptr(unit))) == 0) {  // 0x004c91c0
        joan::orig::as_units(kUnitTable)->remove_51e1f0(static_cast<i32>(jo::as_unit(unit)->id));  // 0x0051e1f0
        return 0;
    }
    const i32 r = joan::orig::arg::as<i32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(b));  // 0x004b91a0
    if (r == 0) return static_cast<i32>(jo::as_unit(unit)->id);
    const i32 g = joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(r)));  // 0x004b67e0
    joan::orig::as_unit(unit)->set_side(static_cast<i32>(static_cast<i16>(g)));  // 0x00509250 (word)
    jo::as_unit(unit)->layer = 1;
    joan::orig::as_unit(unit)->set_value_135(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_unit(unit)->hit_points_max())));  // 0x004fe510(0x00523320())
    joan::orig::as_unit(unit)->set_145(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_unit(unit)->value_149())));  // 0x004fe5d0(0x00523700())
    if (jo::as_unit(unit)->group != nullptr) {
        for (i32 i = static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->count); i > 0; --i) {
            const i32 id = static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->members[i - 1]);
            if (unit_missing(id) != 0) continue;
            const i32 id2 = static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->members[i - 1]);
            joan::orig::as_unit(unit_get(id2))->set_side(static_cast<i32>(static_cast<i16>(g)));  // 0x00509250 (word)
        }
    }
    return static_cast<i32>(jo::as_unit(unit)->id);
}

// ---------------------------------------------------------------------------- 0x006559e0
static i32 JOAN_CDECL script_unit_51a250(i16 id, i16 b) {
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    joan::orig::as_unit(unit_get(static_cast<i32>(id)))->set_mount(static_cast<i32>(b));  // 0x0051a250
    return static_cast<i32>(id);
}

// ---------------------------------------------------------------------------- 0x00655d30
static i32 JOAN_CDECL script_unit_2e7(i16 id) {
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    return static_cast<i32>(jo::as_unit(unit_get(static_cast<i32>(id)))->interact_stage);
}

// ---------------------------------------------------------------------------- 0x00656030
static i32 JOAN_CDECL script_building_units_set_2eb(i16 type, i16 id, i16 v) {
    c_set_2eb.hit();
    if (joan::orig::arg::as<i32>(joan::orig::as_list(kBuildings)->is_missing_ca0(static_cast<i32>(id))) != 0) return 0;  // 0x00456ca0
    u8* const b = joan::orig::arg::as<u8*>(joan::orig::as_list(kBuildings)->get_c70(static_cast<i32>(id)));     // 0x00456c70
    if (jo::as_place(b)->world == nullptr) return 0;
    i32 n = 0;  // [ebp-0x8], written by 0x00429090
    void** const arr = joan::orig::arg::as<void**>(joan::orig::as_world(jo::as_place(b)->world)->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(jo::as_place(b)->box)), joan::orig::arg::mptr(static_cast<void*>(&n))));  // 0x00429090
    for (i32 i = 0; i < n; ++i) {
        u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(arr[i])->get_8c());  // 0x0054ee10
        u8* cur = first;
        if (cur == nullptr) continue;
        if (static_cast<i32>(jo::as_unit(cur)->type_id) != static_cast<i32>(type)) continue;
        do {
            const u32 in = joan::orig::arg::as<u32>(joan::orig::as_aabb(static_cast<void*>(jo::as_place(b)->box))->contains(static_cast<void*>(jo::as_unit(cur)->position_))) &
                           0xffu;  // 0x0060d5f0
            if (in != 0 && static_cast<i32>(jo::as_unit(cur)->bribe_state) != 2) jo::as_unit(cur)->bribe_state = v;
            cur = jo::view<u8*>(jo::as_unit(cur)->cell_prev);
        } while (cur != nullptr && cur != first);
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x006563b0
static std::uintptr_t JOAN_CDECL script_unit_name(void* s, i32 id) {
    if (unit_missing(id) != 0) return joan::orig::arg::as<u32>(joan::orig::as_string(s)->assign(str(d::kEmpty)));  // 0x005a1440
    char* const p = jo::as_unit(unit_get(id))->name_;
    return joan::orig::arg::as<u32>(joan::orig::as_string(s)->assign(joan::orig::arg::str(static_cast<void*>(p))));  // 0x005a1440
}

// ---------------------------------------------------------------------------- 0x006575c0
static i32 JOAN_CDECL script_unit_2f_zero(i16 id) {
    if (unit_missing(static_cast<i32>(id)) != 0) return -1;
    return static_cast<i32>(jo::as_unit(unit_get(static_cast<i32>(id)))->speech_off) == 0 ? 1 : 0;
}

// ---------------------------------------------------------------------------- 0x00658810
static i32 JOAN_CDECL script_unit_group_move(i16 id, i32 x, i32 y, i32 z, float deg) {
    c_group_move.hit();
    u8* const unit = unit_get(static_cast<i32>(id));
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->is_busy()) == 0) return 0;  // 0x004b62b0
    u8* const grp = joan::orig::arg::as<u8*>(joan::orig::as_unit(unit)->leader());      // 0x004216d0
    if (grp == nullptr) return 0;
    if (jo::as_unit(grp)->group == nullptr) return 0;
    jo::view<i32>(jo::as_group(jo::as_unit(grp)->group)->x) = x;
    jo::view<i32>(jo::as_group(jo::as_unit(grp)->group)->y) = y;
    jo::view<i32>(jo::as_group(jo::as_unit(grp)->group)->z) = z;
    jo::as_group(jo::as_unit(grp)->group)->angle = deg * global<float>(d::flt_0_017453292);
    jo::as_group(jo::as_unit(grp)->group)->u32_54 = jo::view<u32>(jo::as_group(jo::as_unit(grp)->group)->angle);
    {
        jo::Group* const g = jo::as_group(jo::as_unit(grp)->group);
        const u32 d0 = jo::view<u32>(g->x);
        jo::view<u32>(g->front_x) = d0;
        const u32 d1 = jo::view<u32>(g->y);
        jo::view<u32>(g->front_y) = d1;
        const u32 d2 = jo::view<u32>(g->z);
        g->u32_50 = d2;
    }
    joan::orig::as_group(jo::as_unit(grp)->group)->arrange(1, 0);  // 0x004fa0f0
    return static_cast<i32>(id);
}

// ---------------------------------------------------------------------------- 0x00658e10
static i32 JOAN_CDECL script_units_set_2f(i32 g, i32 v) {
    c_units_set_2f.hit();
    for (i32 i = unit_count(); i > 0; --i) {
        u8* const e = unit_at(i);
        if (e == nullptr) continue;
        if (static_cast<i32>(jo::as_unit(e)->group_id) == g) jo::view<u8>(jo::as_unit(e)->speech_off) = v == 0 ? 1 : 0;
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659610
static i32 JOAN_CDECL script_set_range_pair(i32 v) {
    if (global<i32>(d::g_mission) != 0) {
        global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].time_limit_ms = v;
        global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].time_left_ms = v;
    } else {
        global<i32>(d::g_a59161) = v;
        global<i32>(d::g_a59165) = v;
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659890
static i32 JOAN_CDECL script_set_a48cdc(i32 v) {
    global<float>(d::g_a48cdc) = static_cast<float>(v);
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659a30
static i32 JOAN_CDECL script_set_a591d4(i32 v) {
    global<i32>(d::g_scenario_rescue_limit) = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659d10
static i32 JOAN_CDECL script_set_a591ba(i32 v) {
    global<i32>(d::g_get_a591ba) = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065a010
static i32 JOAN_CDECL script_stat_table(const char* stat, i32 i) {
    c_stat_table.hit();
    i32 row = 0;
    if (joan::crt::stricmp(joan::orig::arg::str(stat), str(d::s_STRENGTH)) == 0) {         // 'STRENGTH'
        row = 0;
    } else if (joan::crt::stricmp(joan::orig::arg::str(stat), str(d::s_DEXTERITY)) == 0) {  // 'DEXTERITY'
        row = 1;
    } else if (joan::crt::stricmp(joan::orig::arg::str(stat), str(d::s_LEADERSHIP)) == 0) {  // 'LEADERSHIP'
        row = 2;
    } else if (joan::crt::stricmp(joan::orig::arg::str(stat), str(d::s_DEFENSE)) == 0) {  // 'DEFENSE'
        row = 3;
    }
    return global<i32>(d::g_a59240 + static_cast<u32>(row * 0x14 + i * 4));
}

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_08", name, addr, fn)
B99("ratio_half", 0x00651060, &ratio_half);
B99("ratio_quarter", 0x006511c0, &ratio_quarter);
B99("script_remove_objects", 0x00651ea0, &script_remove_objects);
B99("script_slot1_d1", 0x00652440, &script_slot1_d1);
B99("Obj::set_word_20", 0x00652a10, &H::set_word_20);
B99("script_unit_set_2e", 0x00653590, &script_unit_set_2e);
B99("script_not_a4fc98_flag", 0x00653c20, &script_not_a4fc98_flag);
B99("script_building_49c240", 0x00653ef0, &script_building_49c240);
B99("script_building_4c4440", 0x00654240, &script_building_4c4440);
B99("script_place_467ea0", 0x00654960, &script_place_467ea0);
B99("script_place_setup", 0x00654d70, &script_place_setup);
B99("script_load_unit", 0x00655200, &script_load_unit);
B99("script_unit_51a250", 0x006559e0, &script_unit_51a250);
B99("Obj::set_16", 0x00655ba0, &H::set_16);
B99("script_unit_2e7", 0x00655d30, &script_unit_2e7);
B99("script_building_units_set_2eb", 0x00656030, &script_building_units_set_2eb);
B99("script_unit_name", 0x006563b0, &script_unit_name);
B99("script_unit_2f_zero", 0x006575c0, &script_unit_2f_zero);
B99("script_unit_group_move", 0x00658810, &script_unit_group_move);
B99("script_units_set_2f", 0x00658e10, &script_units_set_2f);
B99("script_set_range_pair", 0x00659610, &script_set_range_pair);
B99("script_set_a48cdc", 0x00659890, &script_set_a48cdc);
B99("script_set_a591d4", 0x00659a30, &script_set_a591d4);
B99("script_set_a591ba", 0x00659d10, &script_set_a591ba);
B99("script_stat_table", 0x0065a010, &script_stat_table);
#undef B99

}  // namespace game_b99_08
}  // namespace

// ---------------------------------------- from modules/game_b100_12.cpp (game/b100_12)
namespace {
namespace game_b100_12 {

void* const kMouse = obj(d::g_mouse);
void* const kUnits = obj(d::g_units);
void* const kObjects = obj(d::g_items);
void* const kRecords = obj(d::g_a45f08);
void* const kBuy = obj(d::g_buy);

i32 list_count(void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0
void* list_get(void* list, i32 i) { return joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0
u8* unit_rec(i32 k) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(k))); }  // 0x0041b3c0
i32 unit_missing(i32 k) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(k)); }  // 0x00428b70
i32 object_missing(i32 k) { return joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(k)); }  // 0x00425d50
void* object_get(i32 k) { return joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(k)); }  // 0x00425d20

JOAN_COUNTER(c_kill_type, "script_unit_kill_type");
JOAN_COUNTER(c_any_quest, "script_any_quest_item");
JOAN_COUNTER(c_set_2e, "script_unit_set_2e");
JOAN_COUNTER(c_open_buy, "script_open_buy");

static void JOAN_CDECL init_a6a25c() {  // 0x006510d0
    global<float>(d::g_a6a25c) = global<float>(d::g_a6a250) / global<float>(d::flt_3);
}

static i32 JOAN_CDECL script_set_a50298(i16 v) {  // 0x00651100
    joan::orig::as_weather_system(obj(d::g_weather_system))->set_word_20(joan::orig::arg::i16(static_cast<i32>(v)));  // 0x00652a10 (reads a word)
    return 1;
}

static i32 JOAN_CDECL script_a460a0_49f370(i16 v) {  // 0x00651e70
    joan::orig::as_units(kObjects)->remove_49f370(static_cast<i32>(v));  // 0x0049f370
    return 1;
}

static u32 JOAN_CDECL script_rgb_component(const char* s, i32 which) {  // 0x006511f0
    u32 a = 0, b = 0, c = 0;
    joan::orig::pack_chars_12(joan::orig::arg::str(s), &a, &b, &c);  // 0x004bbac0 (writes all three)
    switch (which) {
    case 1:
        return a;
    case 2:
        return b;
    case 3:
        return c;
    default:
        return 0;
    }
}

static i32 JOAN_CDECL script_unit_kill_type(i16 type) {  // 0x006523c0
    c_kill_type.hit();
    for (i32 i = list_count(kUnits); i > 0; --i) {
        u8* const u = static_cast<u8*>(list_get(kUnits, i));
        if (u == nullptr) continue;
        if (static_cast<i32>(jo::as_unit(u)->group_id) == static_cast<i32>(type)) joan::orig::as_unit(u)->release_slot();  // 0x00528300
    }
    return 1;
}

static i32 JOAN_CDECL script_obj_set_ce(i32 k, i8 v) {  // 0x00652520
    if (object_missing(k) != 0) return -1;
    u8* const sub = virtual_call<u8*>(object_get(k), 0x4c);
    jo::view<i8>(jo::as_gate(sub)->need_key) = v;  // slot 0x4c returns the Gate
    return static_cast<i32>(v);
}

static i32 JOAN_CDECL script_unit_set_52(i16 k, i8 v) {  // 0x00652de0
    if (unit_missing(k) != 0) return 0;
    if (jo::as_unit(unit_rec(k))->draw == nullptr) return 0;
    jo::view<i8>(jo::as_unit(unit_rec(k))->draw->u8_52) = v;
    return 1;
}

static i32 JOAN_CDECL script_any_quest_item(const char* p) {  // 0x00653400
    c_any_quest.hit();
    if (p == nullptr) return 0;
    for (i32 i = 1; i <= list_count(kRecords); ++i) {
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kRecords)->empty_at_561320(joan::orig::arg::i32(i))) != 0) continue;  // 0x00561320
        auto rec = [i]() { return joan::orig::arg::as<u8*>(joan::orig::as_units(kRecords)->get_420240(joan::orig::arg::i32(i))); };  // 0x00420240
        if (jo::view<i8>(jo::as_world(rec())->loaded) == 0) continue;
        for (i32 j = 0; j < jo::as_world(rec())->cell_count; ++j) {
            if (jo::as_world(rec())->cells[j].item == nullptr) continue;
            if (jo::view<i8>(jo::as_world(rec())->cells[j].item->loaded) == 0) continue;
            if (jo::as_world(rec())->cells[j].item->tracks == nullptr) continue;
            jo::Cell* const e = jo::as_world(rec())->cells[j].item;
            void* const t = joan::orig::arg::as<void*>(joan::orig::as_name_record(e->tracks)->get_c());  // 0x00453cc0
            if (joan::orig::arg::as<i32>(joan::orig::as_name_list(t)->find_4ec320(p)) != 0) return 1;              // 0x004ec320
        }
    }
    return 0;
}

static i32 JOAN_CDECL script_unit_set_2e(i16 k, i8 v) {  // 0x00653640
    c_set_2e.hit();
    if (k == 0) {
        for (i32 i = list_count(kUnits); i > 0; --i) {
            u8* const u = static_cast<u8*>(list_get(kUnits, i));
            if (u == nullptr) continue;
            jo::as_unit(u)->frozen = v;
        }
        return 1;
    }
    jo::as_unit(unit_rec(k))->frozen = v;
    if (jo::as_unit(unit_rec(k))->group == nullptr) return 1;
    for (i32 j = jo::as_group(jo::view<u8*>(jo::as_unit(unit_rec(k))->group))->count; j > 0; --j) {
        const i16 m = jo::as_group(jo::view<u8*>(jo::as_unit(unit_rec(k))->group))->members[j - 1];
        if (unit_missing(m) != 0) continue;
        const i16 m2 = jo::as_group(jo::view<u8*>(jo::as_unit(unit_rec(k))->group))->members[j - 1];
        jo::as_unit(unit_rec(m2))->frozen = v;
    }
    return 1;
}

static i32 JOAN_CDECL script_open_buy(i16 a, i16 b) {  // 0x00653890
    c_open_buy.hit();
    if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_places))->empty_at_e(static_cast<i32>(a))) != 0) return 0;  // 0x0044cf80
    if (unit_missing(b) != 0) return 0;
    joan::orig::as_shop(kBuy)->set_open(1);                       // 0x006c8d00
    joan::orig::as_shop(kBuy)->set_12(static_cast<i32>(a));     // 0x00655b70
    joan::orig::as_shop(kBuy)->set_16(static_cast<i32>(b));     // 0x00655ba0
    joan::orig::as_cursor(kMouse)->reset_pos();                        // 0x00592c30
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_12", name, addr, fn)
B("init_a6a25c", 0x006510d0, &init_a6a25c);
B("script_set_a50298", 0x00651100, &script_set_a50298);
B("script_rgb_component", 0x006511f0, &script_rgb_component);
B("script_a460a0_49f370", 0x00651e70, &script_a460a0_49f370);
B("script_unit_kill_type", 0x006523c0, &script_unit_kill_type);
B("script_obj_set_ce", 0x00652520, &script_obj_set_ce);
B("script_unit_set_52", 0x00652de0, &script_unit_set_52);
B("script_any_quest_item", 0x00653400, &script_any_quest_item);
B("script_unit_set_2e", 0x00653640, &script_unit_set_2e);
B("script_open_buy", 0x00653890, &script_open_buy);
#undef B

}  // namespace game_b100_12
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

struct Vec3 {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kObjects = obj(d::g_items);

JOAN_COUNTER(c_set_pos, "object_set_pos");

// ---------------------------------------------------------------------------- 0x00651130
static i32 JOAN_CDECL script_capture_place(i32 k) {
    if (joan::orig::arg::as<u8>(joan::orig::as_bytes(obj(d::g_weather_system))->at(joan::orig::arg::i32(k))) == 0) {  // 0x0041bf10
        if (k == 1)
            global<u32>(d::g_a4839c) = global<u32>(d::g_calendar_minute_of_day);
        else if (k == 2)
            global<u32>(d::g_a483ac) = global<u32>(d::g_calendar_minute_of_day);
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x00651f00
static i32 JOAN_CDECL object_set_pos(i16 id, Vec3 v) {
    c_set_pos.hit();
    if (joan::orig::as_units(kObjects)->list.empty_at(static_cast<i32>(id)) != 0) return 0;  // 0x00425d50
    const Vec3 copy = v;
    void* const o = joan::orig::as_units(kObjects)->list.ptr_at(static_cast<i32>(id));  // 0x00425d20
    joan::orig::as_map_object(o)->set_world(global<jo::World*>(d::g_world));                         // 0x004994b0
    joan::orig::as_map_object(o)->set_position(copy);                                            // 0x0041ff50
    return static_cast<i32>(id);
}

// ---------------------------------------------------------------------------- 0x00652590
static i32 JOAN_CDECL object_byte_ce(i32 id) {
    if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(id)) != 0) return -1;  // 0x00425d50
    void* const o = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
    u8* const p = virtual_call<u8*>(o, 0x4c);
    return static_cast<i32>(jo::view<i8>(jo::as_gate(p)->need_key));  // slot 0x4c returns the Gate
}

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("script_capture_place", 0x00651130, &script_capture_place);
B("object_set_pos", 0x00651f00, &object_set_pos);
B("object_byte_ce", 0x00652590, &object_byte_ce);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

void* const kChars = obj(d::g_items);  // script character table
void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_set_type, "script_set_type");

// ---------------------------------------------------------------------------- 0x00651260
i32 JOAN_CDECL script_unit_2a1_1d(u32 id) {
    const i32 i = static_cast<i32>(static_cast<i16>(id));
    if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(i)) != 0) return 0;  // 0x00428b70
    u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3c0
    if (jo::as_unit(u)->orders == nullptr) return 0;
    return jo::as_order(jo::view<u8*>(jo::as_unit(u)->orders))->current.kind;
}

// ---------------------------------------------------------------------------- 0x00652010
i32 JOAN_CDECL script_set_type(u32 id, const char* key) {
    c_set_type.hit();
    const i32 i = static_cast<i32>(static_cast<i16>(id));
    if (joan::orig::as_units(kChars)->list.empty_at(joan::orig::arg::i32(i)) != 0) return 0;  // 0x00425d50
    u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_units(kChars)->list.ptr_at(joan::orig::arg::i32(i)));  // 0x00425d20
    const i32 r = joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(key);  // 0x004b91a0 (an id)
    if (r == 0) {
        jo::view<u16>(jo::as_map_object(o)->side) = 0;
        return 0;
    }
    const i32 s = joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(r));  // 0x004b67e0
    if (s == 0) {
        jo::view<u16>(jo::as_map_object(o)->side) = 0;
        return 0;
    }
    jo::view<u16>(jo::as_map_object(o)->side) = static_cast<u16>(s);
    return s;
}

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("script_unit_2a1_1d", 0x00651260, &script_unit_2a1_1d);
B96("script_set_type", 0x00652010, &script_set_type);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b90_05.cpp (game/b90_05)
namespace {
namespace game_b90_05 {

using joan::orig::helpers::at;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12);

constexpr img::Addr kUnitTable(d::g_units);
constexpr img::Addr kUnits(d::g_items);
constexpr img::Addr kGroups(d::g_place_array);

std::int32_t sx16(std::int32_t v) { return static_cast<std::int16_t>(v); }
std::uint32_t bits(float v) {
    std::uint32_t b;
    std::memcpy(&b, &v, 4);
    return b;
}

std::int32_t unit_invalid(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kUnitTable))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kUnitTable))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::int32_t c_strcmp(const char* a, const char* b) { return std::strcmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

JOAN_COUNTER(c_is_building, "Script::is_building");
JOAN_COUNTER(c_in_arc, "Script::in_arc");
JOAN_COUNTER(c_set_target_mode, "Script::set_target_mode");
JOAN_COUNTER(c_formation_slot, "Script::formation_slot");
JOAN_COUNTER(c_angle_to, "Script::angle_to");
JOAN_COUNTER(c_distance_obj, "Script::distance_to_object");
JOAN_COUNTER(c_distance, "Script::distance");

// 0x006512d0
std::int32_t JOAN_CDECL script_is_building(std::int32_t id) {
    c_is_building.hit();
    if (unit_invalid(sx16(id)) != 0) return 0;
    std::uint8_t* const u = unit_get(sx16(id));
    const std::int32_t k = static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::as_unit(u)->anim_kind));
    if (k == 0x23 || k == 0x24 || k == 0x25 || k == 0x26 || k == 0x27 || k == 0x28 || k == 0x40 || k == 0x41 ||
        k == 0x42 || k == 0x29 || k == 0x2a)
        return 1;
    return 0;
}

// 0x00651430: -90 < angle(other) < 90 for some kinds.
std::int32_t JOAN_CDECL script_in_arc(std::int32_t other, std::int32_t id) {
    c_in_arc.hit();
    if (unit_invalid(sx16(other)) != 0 || unit_invalid(sx16(id)) != 0) return 0;
    std::uint8_t* const u = unit_get(sx16(id));
    const std::int32_t k = static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::as_unit(u)->anim_kind));
    if (!(k == 0x1c || k == 0x1e || k == 0x1f || k == 0x20 || k == 0x21 || k == 0x53)) return 0;
    const float a = joan::orig::arg::as<float>(joan::orig::as_unit(u)->heading_to_unit(joan::orig::arg::i32(sx16(other))));  // 0x00505960
    if (a > global<float>(d::flt_m90) && a < global<float>(d::flt_90)) return 1;
    return 0;
}

// 0x00651550
std::int32_t JOAN_CDECL script_mode_gt1(std::int32_t id) {
    return jo::as_unit(unit_get(sx16(id)))->idle_time > 1 ? 1 : 0;
}
// 0x00651590
std::int32_t JOAN_CDECL script_mode_is1(std::int32_t id) {
    return jo::as_unit(unit_get(sx16(id)))->idle_time == 1 ? 1 : 0;
}

// 0x006515d0
std::int32_t JOAN_CDECL script_set_mode(std::int32_t id, const char* mode) {
    std::uint8_t* const u = unit_get(sx16(id));
    if (c_strcmp(mode, &img::global<const char>(d::s_IDLE)) == 0) {  // "IDLE"
        jo::as_unit(u)->idle_time = 3;
    } else if (c_strcmp(mode, &img::global<const char>(d::s_ATTACK_9cf380)) == 0) {  // "ATTACK"
        jo::as_unit(u)->idle_time = 1;
    } else if (c_strcmp(mode, &img::global<const char>(d::s_ACTIVE)) == 0) {  // "ACTIVE"
        jo::as_unit(u)->idle_time = 2;
    }
    return 0;
}

// 0x00651690
std::int32_t JOAN_CDECL script_set_target_mode(std::int32_t id, const char* mode) {
    c_set_target_mode.hit();
    if (unit_invalid(sx16(id)) != 0) return 0;
    std::uint8_t* const u = unit_get(sx16(id));
    if (c_strcmp(mode, &img::global<const char>(d::s_NEAREST)) == 0) {  // "NEAREST"
        jo::as_unit(u)->target_select = 1;
    } else if (c_strcmp(mode, &img::global<const char>(d::s_WEAKEST)) == 0) {  // "WEAKEST"
        jo::as_unit(u)->target_select = 2;
    } else if (c_strcmp(mode, &img::global<const char>(d::s_IMPORTANT)) == 0) {  // "IMPORTANT"
        jo::as_unit(u)->target_select = 3;
    } else if (c_strcmp(mode, &img::global<const char>(d::s_NEAREST_ARCHER)) == 0) {  // "NEAREST_ARCHER"
        jo::as_unit(u)->target_select = 4;
    } else if (c_strcmp(mode, &img::global<const char>(d::s_NEAREST_HIGH_PRIORITY)) == 0) {  // "NEAREST_HIGH_PRIORITY"
        jo::as_unit(u)->target_select = 5;
    }
    return 0;
}

// 0x006520d0
std::int32_t JOAN_CDECL script_formation(std::int32_t id) {
    if (unit_invalid(sx16(id)) != 0) return 0;
    std::uint8_t* const u = unit_get(sx16(id));
    std::uint8_t* const leader = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_unit(u)->leader());  // 0x004216d0
    if (jo::view<jo::TaskSlot>(jo::as_unit(u)->task).kind != 0) return jo::view<jo::TaskSlot>(jo::as_unit(u)->task).kind;
    if (leader == nullptr) return 0;
    if (jo::view<jo::TaskSlot>(jo::as_unit(leader)->task).kind == 0xc) return 0;
    jo::view<std::int32_t>(jo::as_unit(u)->name_index) = jo::view<std::int32_t>(jo::as_unit(leader)->name_index);
    return jo::view<jo::TaskSlot>(jo::as_unit(leader)->task).kind;
}

// 0x00652190
std::int32_t JOAN_CDECL script_formation_slot(std::int32_t id, std::int32_t which) {
    c_formation_slot.hit();
    std::uint8_t* const u = unit_get(sx16(id));
    std::uint8_t* const leader = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_unit(u)->leader());  // 0x004216d0
    if (jo::view<jo::TaskSlot>(jo::as_unit(u)->task).kind != 0) {
        if (which == 1) return jo::view<std::int32_t>(jo::view<jo::TaskSlot>(jo::as_unit(u)->task).target);
        if (which == 2) return jo::view<std::int32_t>(jo::view<jo::TaskSlot>(jo::as_unit(u)->task).arg_9);
        if (which == 3) return jo::view<std::int32_t>(jo::view<jo::TaskSlot>(jo::as_unit(u)->task).arg_d);
        return 0;
    }
    if (leader == nullptr) return 0;
    if (which == 1) return jo::view<std::int32_t>(jo::view<jo::TaskSlot>(jo::as_unit(leader)->task).target);
    if (which == 2) return jo::view<std::int32_t>(jo::view<jo::TaskSlot>(jo::as_unit(leader)->task).arg_9);
    if (which == 3) return jo::view<std::int32_t>(jo::view<jo::TaskSlot>(jo::as_unit(leader)->task).arg_d);
    return 0;
}

// 0x00652370
std::int32_t JOAN_CDECL script_unit_528300(std::int32_t id) {
    if (unit_invalid(sx16(id)) != 0) return 0;
    std::uint8_t* const u = unit_get(sx16(id));
    return joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(u)->release_slot());  // 0x00528300
}

// 0x00652ee0: ftol(heading * 180 / pi - angle(other)).
std::int32_t JOAN_CDECL script_angle_to(std::int32_t id, std::int32_t other) {
    c_angle_to.hit();
    if (unit_invalid(sx16(id)) != 0 || unit_invalid(other) != 0) return 0;
    std::uint8_t* const u = unit_get(sx16(id));
    const Fpu fpu{true};
    const float heading = joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading());  // 0x004ff850 (fld [this+0x1a])
    double d = fpu.mul(heading, global<float>(d::flt_180));
    d = fpu.div(d, global<double>(d::dbl_3_141592653589793));  // stored as a qword local (already float-rounded)
    const float a = joan::orig::arg::as<float>(joan::orig::as_unit(u)->heading_to_unit(joan::orig::arg::i32(other)));  // 0x00505960
    return joan::hook::x87::ftol(fpu.sub(d, a));
}

// 0x00653060
std::int32_t JOAN_CDECL script_distance_to_object(std::int32_t id, std::int32_t obj) {
    c_distance_obj.hit();
    if (unit_invalid(sx16(id)) != 0 || joan::orig::as_units(img::obj(kUnits))->list.empty_at(joan::orig::arg::i32(obj)) != 0) return 0;  // 0x00425d50
    std::uint8_t* const o = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(img::obj(kUnits))->list.ptr_at(joan::orig::arg::i32(obj)));  // 0x00425d20
    std::uint8_t* const u = unit_get(sx16(id));
    Vec3 tmp;
    const Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, static_cast<void*>(jo::as_unit(u)->position_), static_cast<void*>(jo::as_map_object(o)->pos)));  // 0x0060cc90 sub
    return joan::hook::x87::ftol(v3d_float(d::iat_v3d_87_length, static_cast<const void*>(&d)));  // length
}

// 0x00653150
std::int32_t JOAN_CDECL script_distance(std::int32_t id, std::int32_t other) {
    c_distance.hit();
    if (unit_invalid(sx16(id)) != 0 || unit_invalid(sx16(other)) != 0) return 0;
    std::uint8_t* const b = unit_get(sx16(other));
    std::uint8_t* const a = unit_get(sx16(id));
    Vec3 tmp;
    const Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, static_cast<void*>(jo::as_unit(a)->position_), static_cast<void*>(jo::as_unit(b)->position_)));  // 0x0060cc90 sub
    return joan::hook::x87::ftol(v3d_float(d::iat_v3d_87_length, static_cast<const void*>(&d)));  // length
}

// 0x00653f30
std::int32_t JOAN_CDECL script_group_contains(std::int32_t group, std::int32_t id) {
    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(img::obj(kGroups))->is_missing_ca0(joan::orig::arg::i32(group))) != 0) return 0;  // 0x00456ca0
    if (unit_invalid(id) != 0) return 0;
    std::uint8_t* const g = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(img::obj(kGroups))->get_c70(joan::orig::arg::i32(group)));  // 0x00456c70
    std::uint8_t* const u = unit_get(id);
    const std::uint8_t r = joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(jo::as_place(g)->box)->contains(static_cast<void*>(jo::as_unit(u)->position_)));  // 0x0060d5f0
    return r != 0 ? 1 : 0;
}

// 0x006547a0
std::int32_t JOAN_CDECL script_group_angle(std::int32_t group) {
    std::uint8_t* const g = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(img::obj(kGroups))->get_c70(joan::orig::arg::i32(group)));  // 0x00456c70
    return joan::hook::x87::ftol(Fpu{true}.div(jo::as_place(g)->angle_2, global<float>(d::flt_0_017453292)));
}

// 0x006547e0
std::int32_t JOAN_CDECL script_group_set_angle(std::int32_t group, std::int32_t degrees) {
    const float r = static_cast<float>(Fpu{true}.mul(static_cast<double>(degrees), global<float>(d::flt_0_017453292)));
    std::uint8_t* const g = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(img::obj(kGroups))->get_c70(joan::orig::arg::i32(group)));  // 0x00456c70
    jo::view<std::uint32_t>(jo::as_place(g)->angle_2) = bits(r);
    return 1;
}

// 0x006558d0
std::int32_t JOAN_CDECL script_set_2d7(std::int32_t id, std::int32_t v) {
    if (unit_invalid(sx16(id)) != 0) return 0;
    std::uint8_t* const u = unit_get(sx16(id));
    jo::view<std::uint16_t>(jo::as_unit(u)->target_building) = static_cast<std::uint16_t>(v);
    return sx16(id);
}

// 0x006569f0
std::int32_t JOAN_CDECL script_leader_id(std::int32_t id) {
    if (unit_invalid(id) != 0) return 0;
    std::uint8_t* const u = unit_get(id);
    std::uint8_t* const leader = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_unit(u)->leader());  // 0x004216d0
    if (leader != nullptr && static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::as_unit(leader)->id)) != id)
        return static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::as_unit(leader)->id));
    return 0;
}

#define B9005(name, addr, fn) JOAN_REPLACE("game/b90_05", name, addr, fn)
B9005("Script::is_building", 0x006512d0, &script_is_building);
B9005("Script::in_arc", 0x00651430, &script_in_arc);
B9005("Script::mode_gt1", 0x00651550, &script_mode_gt1);
B9005("Script::mode_is1", 0x00651590, &script_mode_is1);
B9005("Script::set_mode", 0x006515d0, &script_set_mode);
B9005("Script::set_target_mode", 0x00651690, &script_set_target_mode);
B9005("Script::formation", 0x006520d0, &script_formation);
B9005("Script::formation_slot", 0x00652190, &script_formation_slot);
B9005("Script::unit_528300", 0x00652370, &script_unit_528300);
B9005("Script::angle_to", 0x00652ee0, &script_angle_to);
B9005("Script::distance_to_object", 0x00653060, &script_distance_to_object);
B9005("Script::distance", 0x00653150, &script_distance);
B9005("Script::group_contains", 0x00653f30, &script_group_contains);
B9005("Script::group_angle", 0x006547a0, &script_group_angle);
B9005("Script::group_set_angle", 0x006547e0, &script_group_set_angle);
B9005("Script::set_2d7", 0x006558d0, &script_set_2d7);
B9005("Script::leader_id", 0x006569f0, &script_leader_id);
#undef B9005

}  // namespace game_b90_05
}  // namespace

// ---------------------------------------- from modules/game_b91_01.cpp (game/b91_01)
namespace {
namespace game_b91_01 {

using joan::orig::helpers::game_new;  // operator new

// ---------------------------------------------------------------------------------------------
// Units / groups. 0x00a47c08 = unit table (0x00428b70 is_free(id), 0x0041b3c0 get(id)).
constexpr img::Addr kUnits(d::g_units);
std::int32_t unit_invalid(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
void* unit_get(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0

void* new_order() {  // new(0x3e) + 0x004bab50
    void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Order)));  // 0x3e on x86
    if (mem == nullptr) return nullptr;
    return joan::orig::as_order(mem)->construct();  // 0x004bab50
}

JOAN_COUNTER(c_group_order, "group_order");
JOAN_COUNTER(c_group_release, "group_release");

// 0x00661340
std::int32_t JOAN_CDECL group_order(std::int32_t id, std::int32_t kind, std::int8_t c, std::int8_t flag,
                                 std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8,
                                 std::int32_t a9) {
    c_group_order.hit();
    if (unit_invalid(id) != 0) return 0;
    void* const u = unit_get(id);
    if (jo::as_unit(u)->orders == nullptr) jo::as_unit(u)->orders = static_cast<jo::Order*>(new_order());
    if (kind == 0xf && flag != 0) {
        void* const g = joan::orig::arg::as<void*>(joan::orig::as_unit(u)->leader());  // 0x004216d0
        if (g != nullptr) {
            for (std::int32_t i = static_cast<std::int32_t>(jo::as_group(jo::as_unit(g)->group)->count) - 1;
                 i >= 0; --i) {
                if (unit_invalid(jo::as_group(jo::as_unit(g)->group)->members[i]) != 0) continue;
                void* const v = unit_get(jo::as_group(jo::as_unit(g)->group)->members[i]);
                if (jo::as_unit(v)->orders == nullptr) continue;
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(jo::as_unit(v)->orders)->contains(joan::orig::arg::i32(kind))) != 0) {  // 0x004bb8d0
                    flag = 0;
                    break;
                }
            }
        }
    }
    // (local ebp-0x14 is always 1)
    joan::orig::as_order(jo::as_unit(u)->orders)->add(u, joan::orig::arg::i32(kind), static_cast<std::int32_t>(c), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(a7), joan::orig::arg::i32(a8), joan::orig::arg::i32(global<std::int32_t>(d::g_now) + a9), 0, 0);  // 0x004bac60
    if (flag == 0 && kind != 0xc) return 1;
    void* const g = joan::orig::arg::as<void*>(joan::orig::as_unit(u)->leader());  // 0x004216d0
    if (g != nullptr) {
        for (std::int32_t i = static_cast<std::int32_t>(jo::as_group(jo::as_unit(g)->group)->count) - 1;
             i >= 0; --i) {
            if (unit_invalid(jo::as_group(jo::as_unit(g)->group)->members[i]) != 0) continue;
            void* const v = unit_get(jo::as_group(jo::as_unit(g)->group)->members[i]);
            if (jo::view<jo::TaskSlot>(jo::as_unit(g)->task).kind != 0 && jo::view<jo::TaskSlot>(jo::as_unit(v)->task).kind != 0 &&
                jo::view<jo::TaskSlot>(jo::as_unit(g)->task).kind != jo::view<jo::TaskSlot>(jo::as_unit(v)->task).kind)
                continue;
            if (kind == 0xf) {
                void* const r = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<std::int32_t>(jo::as_unit(v)->type_id))));  // 0x0042d0a0
                if (jo::self<jo::UnitType>(r)->unit_class == 8) continue;
            }
            if (kind == 0xc) {
                if (jo::as_unit(v)->target_building != 0) continue;
                if (jo::as_unit(v)->orders != nullptr && jo::as_order(jo::as_unit(v)->orders)->current.kind == 0xc) continue;
            }
            if (jo::as_unit(v)->orders == nullptr) jo::as_unit(v)->orders = static_cast<jo::Order*>(new_order());
            joan::orig::as_order(jo::as_unit(v)->orders)->add(v, joan::orig::arg::i32(kind), joan::orig::arg::i32(static_cast<std::int32_t>(c) - 1), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(a7), joan::orig::arg::i32(a8), joan::orig::arg::i32(global<std::int32_t>(d::g_now) + a9), 1, static_cast<std::int32_t>(jo::as_unit(u)->id));  // 0x004bac60
        }
    }
    if (g == nullptr || g == u || kind == 0xf) return 1;
    if (jo::view<jo::TaskSlot>(jo::as_unit(g)->task).kind != 0 && jo::view<jo::TaskSlot>(jo::as_unit(u)->task).kind != 0 &&
        jo::view<jo::TaskSlot>(jo::as_unit(g)->task).kind != jo::view<jo::TaskSlot>(jo::as_unit(u)->task).kind)
        return 1;
    std::int32_t ok = 1;
    if (kind == 0xc) {
        if (jo::as_unit(g)->target_building != 0 ||
            (jo::as_unit(g)->orders != nullptr && jo::as_order(jo::as_unit(g)->orders)->current.kind == 0xc))
            ok = 0;
    }
    if (ok == 0) return 1;
    if (jo::as_unit(g)->orders == nullptr) jo::as_unit(g)->orders = static_cast<jo::Order*>(new_order());
    joan::orig::as_order(jo::as_unit(g)->orders)->add(g, joan::orig::arg::i32(kind), joan::orig::arg::i32(static_cast<std::int32_t>(c) - 1), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(a7), joan::orig::arg::i32(a8), joan::orig::arg::i32(global<std::int32_t>(d::g_now) + a9), 1, static_cast<std::int32_t>(jo::as_unit(u)->id));  // 0x004bac60
    return 1;
}

// 0x006618f0
std::int32_t JOAN_CDECL group_release(std::int32_t id) {
    c_group_release.hit();
    if (unit_invalid(id) != 0) return 0;
    void* const u = unit_get(id);
    if (jo::as_unit(u)->orders != nullptr) {
        if (jo::as_order(jo::as_unit(u)->orders)->current.kind != 0)
            joan::orig::as_action(jo::as_unit(u)->orders)->dispatch(joan::orig::arg::mptr(u));  // 0x004bb660
        void* const o = jo::as_unit(u)->orders;
        if (o != nullptr) joan::orig::as_ai(o)->destroy(1u);  // 0x00415220 deleting dtor
        jo::as_unit(u)->orders = nullptr;
        joan::orig::as_squad(u)->clear();  // 0x00422f10
    }
    return 1;
}

// 0x006517c0
std::int32_t JOAN_CDECL unit_forward_54b480(std::int32_t id, const char* a2, std::int32_t a3, std::int32_t a4,
                                         std::int32_t a5, std::int32_t a6, std::int32_t a7) {
    const std::int32_t w = static_cast<std::int16_t>(id);
    if (unit_invalid(w) != 0) return 0;
    return joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit_get(w))->find_target_54b480(a2, joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), joan::orig::arg::i32(a5), joan::orig::arg::i32(a6), joan::orig::arg::i32(a7)));  // 0x0054b480
}

// 0x0065c050
std::int32_t JOAN_CDECL unit_set_mood(std::int32_t id, const char* name) {
    const std::int32_t w = static_cast<std::int16_t>(id);
    if (unit_invalid(w) != 0) return 0;
    void* const u = unit_get(w);
    const std::int32_t mood = joan::orig::arg::as<std::int32_t>(joan::orig::anim_kind_from_name(joan::orig::arg::str(name)));  // 0x00416aa0
    jo::as_unit(u)->anim_special = static_cast<std::int16_t>(mood);
    return 1;
}

// 0x00653c60
std::int32_t JOAN_CDECL unit_state_is_42(std::int32_t id) {
    void* const u = unit_get(static_cast<std::int16_t>(id));
    return jo::as_unit(u)->anim_kind == 0x2a ? 1 : 0;
}

// 0x0065b130
std::int32_t JOAN_CDECL game_flag_744() {
    return joan::orig::arg::as<std::int8_t>(joan::orig::as_game(obj(d::g_game_state))->flag_744());  // 0x004301c0 (movsx al)
}

#define B(name, addr, fn) JOAN_REPLACE("game/b91_01", name, addr, fn)
B("group_order", 0x00661340, &group_order);
B("group_release", 0x006618f0, &group_release);
B("unit_forward_54b480", 0x006517c0, &unit_forward_54b480);
B("unit_set_mood", 0x0065c050, &unit_set_mood);
B("unit_state_is_42", 0x00653c60, &unit_state_is_42);
B("game_flag_744", 0x0065b130, &game_flag_744);
#undef B

}  // namespace game_b91_01
}  // namespace

// ---------------------------------------- from modules/game_b107_10.cpp (game/b107_10)
namespace {
namespace game_b107_10 {

JOAN_COUNTER(c_group_find, "group_try_find_651830");

// ------------------------------------------------------------------ 0x00651830
// 0x0054b480 (thunk 0x00405475), ret 0x18.
i32 find_on(void* unit, u32 range) {
    return joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->find_target_54b480(img::str(d::s_E_9cfc2c), joan::orig::arg::i32(range), i32{0}, i32{0}, i32{0}, i32{0}));
}

static i32 JOAN_CDECL group_try_find(i32 id_arg, u32 range) {
    c_group_find.hit();
    void* const units = obj(d::g_units);
    const i32 id = static_cast<i16>(id_arg);
    if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
    void* const unit = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->is_busy()) == 0) {                 // 0x004b62b0
        return find_on(unit, range) != 0 ? 1 : 0;
    }
    void* const leader = joan::orig::arg::as<void*>(joan::orig::as_unit(unit)->leader());  // 0x004216d0
    if (find_on(leader, range) != 0) return 1;
    if (leader == nullptr || jo::as_unit(leader)->group == nullptr) return 0;
    for (i32 i = 0; i < static_cast<i32>(jo::as_group(jo::as_unit(leader)->group)->count); ++i) {
        const i32 mid = jo::as_group(jo::as_unit(leader)->group)->members[i];
        if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(mid)) != 0) continue;  // 0x00428b70
        void* const member = joan::orig::as_dynarray(units)->ptr_at(static_cast<i32>(jo::as_group(jo::as_unit(leader)->group)->members[i]));
        if (find_on(member, range) != 0) return 1;
    }
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_10", name, addr, fn)
B("group_try_find_651830", 0x00651830, &group_try_find);
#undef B

}  // namespace game_b107_10
}  // namespace

// ---------------------------------------- from modules/game_b107_12.cpp (game/b107_12)
namespace {
namespace game_b107_12 {

// 0x006519d0
u32 JOAN_CDECL unit_call_503680(i32 unit_raw, const char* a, u32 b, u32 c) {
    const i32 unit = static_cast<i16>(unit_raw);  // movsx word [ebp+8]
    if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(unit)) != 0)  // 0x00428b70
        return 0;
    void* const u = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(unit));  // 0x0041b3c0
    return joan::orig::arg::as<u32>(joan::orig::as_unit(u)->pick_target_503680(a, joan::orig::arg::i32(b), joan::orig::arg::i32(c)));                         // 0x00503680
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_12", name, addr, fn)
B("Units::call_503680_6519d0", 0x006519d0, &unit_call_503680);
#undef B

}  // namespace game_b107_12
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

// 0x00651a30
std::int32_t JOAN_CDECL call_4a36c0(const char* a) {
    return joan::orig::arg::as<std::int32_t>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(a));  // 0x004a36c0
}

// 0x006522f0
std::int32_t JOAN_CDECL set_2ba(std::int16_t id, std::uint32_t value) {
    void* const p = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(static_cast<std::int32_t>(id));  // 0x0041b3c0
    void* const q = joan::orig::as_unit(p)->leader();                                 // 0x004216d0
    if (jo::view<jo::TaskSlot>(jo::as_unit(p)->task).kind != 0) {
        jo::view<jo::TaskSlot>(jo::as_unit(p)->task).u32_15 = value;
        return 1;
    }
    if (q != nullptr) {
        jo::view<jo::TaskSlot>(jo::as_unit(p)->task).u32_15 = value;
        return 1;
    }
    return 0;
}

// 0x00652e60
std::int32_t JOAN_CDECL reset_all_652e60() {
    for (std::int32_t n = joan::orig::as_dynarray(obj(d::g_units))->size(); n > 0; --n) {  // 0x00417ad0
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(n)) != 0) continue;  // 0x00428b70
        joan::orig::as_unit(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(n)))->release_slot();  // 0x00528300 / 0x0041b3c0
    }
    return 1;
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Script::call_4a36c0", 0x00651a30, &call_4a36c0);
B94("Script::set_2ba", 0x006522f0, &set_2ba);
B94("Script::reset_all", 0x00652e60, &reset_all_652e60);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b101_13.cpp (game/b101_13)
namespace {
namespace game_b101_13 {

// thiscall through the object's vtable slot `off`.
template <class R = void, class... A>
R vcall(const void* self, u32 off, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    void* const fn = joan::call::vtable_of(self)[joan::call::x86_vslot(off)];
    std::memcpy(&method, &fn, sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}
void* const kUnits = obj(d::g_units);
void* const kPlaces = obj(d::g_places);
void* const kObjects = obj(d::g_items);
void* const kTypes = obj(d::g_types);
void* const kGroups = obj(d::g_nations);
void* const kMission = obj(d::g_player_hud);

u8* unit(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* place(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_list(kPlaces)->get_e(joan::orig::arg::i32(id))); }      // 0x0043c450

JOAN_COUNTER(c_area_cmd, "ScriptNode::area_cmd");
JOAN_COUNTER(c_6537a0, "script_unit_6537a0");
JOAN_COUNTER(c_654c80, "script_place_654c80");
JOAN_COUNTER(c_658ad0, "script_unit_658ad0");
JOAN_COUNTER(c_65a4a0, "script_objective_state");
JOAN_COUNTER(c_65d450, "script_unit_65d450");

// ---------------------------------------------------------------------- 0x00651a60
u32 JOAN_CDECL script_651a60(const char* key) {
    const u32 v = joan::orig::arg::as<u32>(joan::orig::as_name_table(kList8f50)->find_4a36c0(key));  // 0x004a36c0
    if (v != 0) {
        void* const e = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kList8f50)->get(joan::orig::arg::i16(static_cast<i16>(v))));  // 0x00425ce0
        joan::orig::as_item_def(e)->acquire_anim(0, 0);                                         // 0x004a39f0
        joan::orig::as_scenario(obj(d::g_mission))->push_word_4cd960(joan::orig::arg::i16(v));                               // 0x004cd960
    }
    return 1;
}

// ---------------------------------------------------------------------- 0x006537a0
u32 JOAN_CDECL script_unit_6537a0(i16 id, const char* a, i32 b, u32 c, i16 d, i16 e) {
    c_6537a0.hit();
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    u8* const u = unit(static_cast<i32>(id));
    if (jo::view<u8*>(jo::as_unit(u)->cell) == nullptr || jo::view<void*>(jo::as_cell(jo::view<u8*>(jo::as_unit(u)->cell))->camera_zones) == nullptr) return 0;
    joan::orig::as_track(jo::view<void*>(jo::as_cell(jo::view<u8*>(jo::as_unit(u)->cell))->camera_zones))->set_word_44(joan::orig::arg::u32(d));  // 0x00654a80
    joan::orig::as_camera_zones(jo::view<void*>(jo::as_cell(jo::view<u8*>(jo::as_unit(u)->cell))->camera_zones))->set_speed(joan::orig::arg::u32(c));  // 0x00428a70
    joan::orig::as_track(jo::view<void*>(jo::as_cell(jo::view<u8*>(jo::as_unit(u)->cell))->camera_zones))->set_4a(joan::orig::arg::i16(e));  // 0x00654ab0
    const i32 t = global<i32>(d::g_now) + b;
    const i16 r = joan::orig::arg::as<i16>(joan::orig::as_camera_zones(jo::view<void*>(jo::as_cell(jo::view<u8*>(jo::as_unit(u)->cell))->camera_zones))->start_track(a, joan::orig::arg::u32(t)));  // 0x00428850
    return static_cast<u32>(static_cast<i32>(r));
}

// ---------------------------------------------------------------------- 0x00653970
u32 JOAN_CDECL script_653970() {
    joan::orig::as_hud(kMission)->blink_restart();  // 0x00707ce0
    return 1;
}

// ---------------------------------------------------------------------- 0x00653a50
u32 JOAN_CDECL script_653a50(i16 a, u32 b, u8 c) {
    joan::orig::as_mini_map(obj(d::g_mini_map))->set_waypoint_style(joan::orig::arg::i16(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c));  // 0x007180f0
    return 1;
}

// ---------------------------------------------------------------------- 0x00654060
u32 JOAN_CDECL script_654060(i16 a, i16 b) {
    void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_place_array))->get_c70(static_cast<i32>(a)));  // 0x00456c70
    return joan::orig::arg::as<u32>(joan::orig::as_light_sys(p)->lit_by(joan::orig::arg::u32(static_cast<i32>(b))));                            // 0x004a65d0
}

// ---------------------------------------------------------------------- 0x00654160
u32 JOAN_CDECL script_654160(i16 a, const char* b) {  // b: a script string value (Army::count_in_range)
    void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_place_array))->get_c70(static_cast<i32>(a)));  // 0x00456c70
    return joan::orig::arg::as<u32>(joan::orig::as_army(p)->count_in_range(b));                                              // 0x004c2b50
}

// ---------------------------------------------------------------------- 0x006548b0
u32 JOAN_CDECL script_6548b0(const char* a) {
    if (a == nullptr) return 0;
    return joan::orig::arg::as<u32>(joan::orig::as_facility_list(kPlaces)->find_by_name(a));  // 0x0046d200
}

// ---------------------------------------------------------------------- 0x00654c80
u32 JOAN_CDECL script_place_654c80(i16 id, i16 mode) {
    c_654c80.hit();
    u8* const o = place(static_cast<i32>(id));
    u8* const r = vcall<u8*>(o, 0x24);
    if (static_cast<i32>(mode) == 1) {
        jo::as_recruit_facility(r)->owner->captured = 1;
        u8* const e = unit(global<i32>(d::g_player_unit_recno));
        const i32 v = static_cast<i32>(jo::as_unit(e)->side);
        jo::as_recruit_facility(r)->owner->nation = v;
        const u32 w = joan::orig::arg::as<u32>(joan::orig::as_place(jo::as_recruit_facility(r)->owner)->recruit_cap());  // 0x004693b0
        joan::orig::as_unit(jo::as_recruit_facility(r)->owner)->set_52(joan::orig::arg::i32(w));              // 0x00469330
        // The place id through FlagFacility (+0x91 on x86), the view its writer (script_place_654f10, 0x00654f10) and every
        // other reader use: RecruitFacility types +0x7d as a 4-byte gap where FlagFacility has a pointer, so on x64
        // its `place` sits 4 bytes before the stored value and read 0 (place lookup null, crash).
        u8* const o2 = place(static_cast<i32>(jo::as_flag_facility(r)->i16_91));
        void* const s = vcall<void*>(o2, 0x18);
        joan::orig::as_unit(s)->clear_79();  // 0x00467f90
        joan::orig::as_recruit_facility(r)->set_flag_models();  // 0x00468c10
    }
    return 1;
}

// ---------------------------------------------------------------------- 0x00654e30
u32 JOAN_CDECL script_place_654e30(i16 id, u32 b) {
    if (joan::orig::arg::as<i32>(joan::orig::as_list(kPlaces)->empty_at_e(static_cast<i32>(id))) != 0) return 0;  // 0x0044cf80
    u8* const o = place(static_cast<i32>(id));
    void* const r = vcall<void*>(o, 0x20);
    joan::orig::as_shop_facility(r)->load_stock(joan::orig::arg::i32(b));  // 0x004697c0
    return 1;
}

// ---------------------------------------------------------------------- 0x006550d0
u32 JOAN_CDECL script_6550d0(const char* a, u32 b) {
    const u32 t = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(a));  // 0x004b91a0
    if (t == 0) return 0;
    const u32 g = joan::orig::arg::as<u32>(joan::orig::as_list(kGroups)->find_by_word2(joan::orig::arg::i32(t)));  // 0x004b67e0
    if (g == 0) return 0;
    void* const o = joan::orig::as_dynarray(kGroups)->ptr_at(joan::orig::arg::i32(g));  // 0x0047b600
    joan::orig::as_nation(o)->recount_1389(joan::orig::arg::i32(b));                          // 0x004b5580
    return 1;
}

// ---------------------------------------------------------------------- 0x00658ad0
void reset_unit_658ad0(u8* u) {
    if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0) return;                     // 0x0042d0e0
    u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(u)->type_id)));  // 0x0042d0a0
    if ((jo::self<jo::UnitType>(t)->trigger_flags & 1) == 0) return;
    jo::view<u32>(jo::as_unit(u)->state_120) = 0;
    joan::orig::as_unit(u)->set_value_135(0);                             // 0x004fe510
    joan::orig::as_unit(u)->die_7c6240(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));         // 0x007c6240
}

u32 JOAN_CDECL script_unit_658ad0(u32 arg) {
    c_658ad0.hit();
    const i32 id = joan::orig::arg::as<i32>(joan::orig::find_unit_in_groups(joan::orig::arg::i16(static_cast<i16>(arg))));  // 0x006581f0
    if (unit_missing(id) == 0) {
        if (jo::as_unit(unit(id))->group != nullptr) {
            for (i32 c = static_cast<i32>(jo::as_group(jo::view<u8*>(jo::as_unit(unit(id))->group))->count); c > 0; --c) {
                if (unit_missing(static_cast<i32>(jo::as_group(jo::view<u8*>(jo::as_unit(unit(id))->group))->members[c - 1])) != 0) continue;
                u8* const u = unit(static_cast<i32>(jo::as_group(jo::view<u8*>(jo::as_unit(unit(id))->group))->members[c - 1]));
                reset_unit_658ad0(u);
            }
        }
        reset_unit_658ad0(unit(id));
    } else {
        for (i32 c = joan::orig::as_dynarray(kUnits)->size(); c > 0; --c) {  // 0x00417ad0
            u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(c)));  // 0x0041b3f0
            if (u == nullptr) continue;
            if (static_cast<i32>(jo::as_unit(u)->group_id) != static_cast<i32>(arg)) continue;
            reset_unit_658ad0(u);
        }
    }
    return 1;
}

// ---------------------------------------------------------------------- 0x0065a4a0
i16& objective_word(i32 n) {
    return global<i16>(d::g_a592a4 + static_cast<u32>(n - 1) * 0x1au);
}
i32 str_cmp(const char* a, img::Addr b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(str(b))); }

u32 JOAN_CDECL script_objective_state(i32 n, const char* s) {
    c_65a4a0.hit();
    constexpr img::Addr kWin = d::s_WIN, kLose = d::s_LOSE, kNormal = d::s_NORMAL;
    if (str_cmp(s, kWin) == 0 && static_cast<i32>(objective_word(n)) == 1) return 1;
    if (str_cmp(s, kLose) == 0 && static_cast<i32>(objective_word(n)) == 2) return 1;
    if (str_cmp(s, kNormal) == 0 && static_cast<i32>(objective_word(n)) == 0) return 1;
    if (str_cmp(s, kWin) == 0) {
        objective_word(n) = 1;
        joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_OBJECTIVE_FINISH));  // 0x004ceef0 "OBJECTIVE_FINISH"
        joan::orig::as_hud(kMission)->restart_log_entry(joan::orig::arg::i32(n - 1));                     // 0x00707960
    } else if (str_cmp(s, kLose) == 0) {
        objective_word(n) = 2;
        joan::orig::as_hud(kMission)->blink_all(joan::orig::arg::i32(n - 1));  // 0x00707b20
    } else if (str_cmp(s, kNormal) == 0) {
        objective_word(n) = 0;
        joan::orig::as_hud(kMission)->push_log_entry(joan::orig::arg::i32(n - 1));  // 0x00707400
    }
    return 1;
}

// ---------------------------------------------------------------------- 0x0065a8d0 / 0x0065a990
u32 JOAN_CDECL script_65a8d0(u32 a, u8 b) {
    joan::orig::as_player(kFlags)->set_flag_bit(joan::orig::arg::i32(a), joan::orig::arg::u32(b));  // 0x00472150
    return 1;
}
u32 JOAN_CDECL script_65a990(u32 a) {
    return static_cast<u32>(static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_player(kFlags)->flag_bit_472370(joan::orig::arg::i32(a)))));  // 0x00472370
}

// ---------------------------------------------------------------------- 0x0065d450
u32 JOAN_CDECL script_unit_65d450(u32 a, u32 b, u32 c) {
    c_65d450.hit();
    if (unit_missing(static_cast<i32>(a)) != 0) return 0;
    u8* const u = unit(static_cast<i32>(a));
    void* const o = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(b));  // 0x00425d20
    joan::orig::as_map_object(o)->set_target_unit(joan::orig::arg::mptr(u));                           // 0x004633e0
    switch (jo::view<u32>(jo::as_unit(u)->siege_tower_state)) {
        case 0:
            if (c != 0) jo::view<u32>(jo::as_unit(u)->siege_tower_state) = 1;
            break;
        case 1:
            if (c == 0) jo::view<u32>(jo::as_unit(u)->siege_tower_state) = 0;
            break;
        case 2:
            if (c == 0) jo::view<u32>(jo::as_unit(u)->siege_tower_state) = 3;
            break;
        case 3:
            if (c != 0) jo::view<u32>(jo::as_unit(u)->siege_tower_state) = 2;
            break;
        default: break;
    }
    return 1;
}

// ---------------------------------------------------------------------- 0x0065dff0
u32 JOAN_CDECL script_object_65dff0(u32 a, u32 b) {
    if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(a)) != 0) return 0;  // 0x00425d50
    void* const o = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(a));  // 0x00425d20
    void* const r = vcall<void*>(o, 0x64);
    joan::orig::as_mouse_cursor(r)->set_mode(2, joan::orig::arg::u32(b));  // 0x00491c40
    return 1;
}

// ---------------------------------------------------------------------- 0x0065f910
u32 JOAN_CDECL script_65f910(i16 a) {
    joan::orig::as_letter(obj(d::g_letter))->load_6e0b70(joan::orig::arg::i16(a));  // 0x006e0b70
    joan::orig::as_letter(obj(d::g_letter))->open();     // 0x006e0900
    return 1;
}

struct H {
    // ScriptNode::value of an integer node: the integer lives in the first 4 bytes of the pointer-typed slot
    // (x86: the whole slot; x64: the low half, set_value / 0x0063f140 store it zero-extended).
    static u32 node_int(void* node) {
        u32 r;
        std::memcpy(&r, &jo::as_script_node(node)->value, sizeof r);
        return r;
    }

    // 0x0063e9f0: n-th parameter node; its value is the dword at +0x10.
    u8* param(i32 n) { return joan::orig::arg::as<u8*>(joan::orig::as_reader(this)->list_get(joan::orig::arg::i32(n))); }

    // ------------------------------------------------------------------ 0x00661060
    u32 area_cmd(std::uintptr_t) {
        c_area_cmd.hit();
        if (static_cast<i32>(jo::self<jo::ScriptCall>(this)->func_id) == 0x143) {
            joan::orig::as_script_node(this)->set_type(1);  // 0x00660ff0
            const u32 p9 = node_int(param(9));
            const u32 p8 = node_int(param(8));
            const u32 p7 = node_int(param(7));
            const u32 p6 = node_int(param(6));
            const u32 p5 = node_int(param(5));
            const u8 p4 = static_cast<u8>(reinterpret_cast<std::uintptr_t>(jo::as_script_node(param(4))->value));
            const u8 p3 = static_cast<u8>(reinterpret_cast<std::uintptr_t>(jo::as_script_node(param(3))->value));
            const i32 k = static_cast<i32>(jo::self<jo::ScriptCall>(this)->arg_id);
            const u32 p1 = node_int(param(1));
            jo::self<jo::ScriptCall>(this)->value = reinterpret_cast<void*>(static_cast<std::uintptr_t>(joan::orig::arg::as<u32>(joan::orig::group_order(joan::orig::arg::i32(p1), joan::orig::arg::i32(k), static_cast<std::int8_t>(joan::orig::arg::i32(p3)), static_cast<std::int8_t>(joan::orig::arg::i32(p4)), joan::orig::arg::i32(p5), joan::orig::arg::i32(p6), joan::orig::arg::i32(p7), joan::orig::arg::i32(p8), joan::orig::arg::i32(p9)))));  // 0x00661340; x64: integer stored in the pointer-typed ScriptNode::value
        }
        const i32 code = static_cast<i32>(jo::self<jo::ScriptCall>(this)->func_id);
        if (code == 0x144) {
            joan::orig::as_script_node(this)->set_type(1);
            const u32 p1 = node_int(param(1));
            const u32 r = joan::orig::arg::as<u32>(joan::orig::group_release(joan::orig::arg::i32(p1)));  // 0x006618f0
            jo::self<jo::ScriptCall>(this)->value = reinterpret_cast<void*>(static_cast<std::uintptr_t>(r));  // x64: integer stored in the pointer-typed ScriptNode::value
            return r;
        }
        if (code == 0x146 || code == 0x147) {
            joan::orig::as_script_node(this)->set_type(1);
            const u32 p7 = node_int(param(7));
            const u32 p6 = node_int(param(6));
            const u32 p5 = node_int(param(5));
            const u32 p4 = node_int(param(4));
            const u8 p3 = static_cast<u8>(reinterpret_cast<std::uintptr_t>(jo::as_script_node(param(3))->value));
            const i32 k = static_cast<i32>(jo::self<jo::ScriptCall>(this)->arg_id);
            const u32 p1 = node_int(param(1));
            const u32 r = cdecl_call<u32>(code == 0x146 ? fn::script_send_order   // 0x006619d0
                                                        : fn::script_unit_order,  // 0x00661b40
                                          p1, k, p3, p4, p5, p6, p7);
            jo::self<jo::ScriptCall>(this)->value = reinterpret_cast<void*>(static_cast<std::uintptr_t>(r));  // x64: integer stored in the pointer-typed ScriptNode::value
            return r;
        }
        return static_cast<u32>(code);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_13", name, addr, fn)
B("script_651a60", 0x00651a60, &script_651a60);
B("script_unit_6537a0", 0x006537a0, &script_unit_6537a0);
B("script_653970", 0x00653970, &script_653970);
B("script_653a50", 0x00653a50, &script_653a50);
B("script_654060", 0x00654060, &script_654060);
B("script_654160", 0x00654160, &script_654160);
B("script_6548b0", 0x006548b0, &script_6548b0);
B("script_place_654c80", 0x00654c80, &script_place_654c80);
B("script_place_654e30", 0x00654e30, &script_place_654e30);
B("script_6550d0", 0x006550d0, &script_6550d0);
B("script_unit_658ad0", 0x00658ad0, &script_unit_658ad0);
B("script_objective_state", 0x0065a4a0, &script_objective_state);
B("script_65a8d0", 0x0065a8d0, &script_65a8d0);
B("script_65a990", 0x0065a990, &script_65a990);
B("script_unit_65d450", 0x0065d450, &script_unit_65d450);
B("script_object_65dff0", 0x0065dff0, &script_object_65dff0);
B("script_65f910", 0x0065f910, &script_65f910);
B("ScriptNode::area_cmd", 0x00661060, &H::area_cmd);
#undef B

}  // namespace game_b101_13
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

using joan::orig::helpers::at;

struct Vec3 {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_spawn_named, "spawn_named");

// 0x00651c90
std::uint32_t JOAN_CDECL spawn_named(std::uint32_t kind, const char* name, std::uint32_t x, std::uint32_t y, std::uint32_t z,
                                  std::int32_t angle, std::uint32_t next) {
    c_spawn_named.hit();
    const std::uint32_t h = joan::orig::arg::as<std::uint32_t>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(joan::orig::arg::str(kind)));  // 0x004a36c0
    const std::uint32_t id = joan::orig::arg::as<std::uint32_t>(joan::orig::as_units(obj(d::g_items))->create_49ea90(joan::orig::arg::u32(h)));  // 0x0049ea90
    void* u = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
    if (u == nullptr) return 0;
    void* def = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<std::uint32_t>(h & 0xffffu))));  // 0x00425ce0
    Vec3 pos;  // the script's dwords are the floats' bits
    std::memcpy(&pos.x, &x, 4);
    std::memcpy(&pos.y, &y, 4);
    std::memcpy(&pos.z, &z, 4);
    joan::orig::as_map_object(u)->set_world(global<jo::World*>(d::g_world));  // 0x004994b0
    joan::orig::as_map_object(u)->set_position(pos);  // 0x0041ff50
    const joan::hook::x87::Fpu fpu{true};
    const float rad = static_cast<float>(fpu.mul(static_cast<double>(angle), global<float>(d::flt_0_017453292)));
    joan::orig::as_map_object(u)->set_yaw(joan::orig::arg::bits(rad));  // 0x00499fd0
    std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_map_object(u)->name)), joan::orig::arg::str(name), 0x40u);  // strncpy
    const std::int16_t s1 = static_cast<std::int16_t>(joan::orig::arg::as<std::uint32_t>(joan::orig::as_names(obj(d::g_randList))->find_463ed0(joan::orig::arg::str(next))));  // 0x00463ed0
    const std::int16_t s2 = static_cast<std::int16_t>(joan::orig::arg::as<std::uint32_t>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(joan::orig::arg::str(next))));  // 0x004a36c0
    if (u != nullptr && jo::view<std::int16_t>(jo::as_item_def(def)->class_id) == 10) {
        void* r = virtual_call<void*>(u, 0x50);  // slot 0x50: the Order0b (drop_item +0xc2, drop_count +0xc4 on x86)
        if (s1 != 0) {
            void* q = joan::orig::arg::as<void*>(joan::orig::as_names(obj(d::g_randList))->at(joan::orig::arg::i16(static_cast<std::uint32_t>(static_cast<std::uint16_t>(s1)))));  // 0x00464100
            joan::orig::as_grass(q)->pick(&jo::as_order0b(r)->drop_item, &jo::as_order0b(r)->drop_count);  // 0x00464030
        } else if (s2 != 0) {
            jo::as_order0b(r)->drop_item = s2;
            void* e = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<std::uint32_t>(static_cast<std::uint16_t>(s2)))));  // 0x00425ce0
            {
                const std::uint32_t level = jo::view<std::uint32_t>(jo::as_item_def(e)->min_level);
                std::memcpy(&jo::as_order0b(r)->drop_count, &level, sizeof level);  // 4 bytes: drop_count and u8_c6[2]
            }
        }
    }
    return id;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("spawn_named", 0x00651c90, &spawn_named);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b97_07.cpp (game/b97_07)
namespace {
namespace game_b97_07 {

template <class R = void, class... A>
R vcall(void* object, std::int32_t slot, A... args) {  // thiscall virtual
    return reinterpret_cast<R(JOAN_THISCALL*)(void*, A...)>(joan::call::vtable_of(object)[slot])(object, args...);
}
using joan::orig::helpers::at;

void* const kUnitTable = obj(d::g_units);
void* const kObjects = obj(d::g_items);
void* const kPlaces = obj(d::g_places);
void* const kNames = obj(d::g_names);
void* const kFx = obj(d::g_mini_map);

JOAN_COUNTER(c_facing_check, "script_unit_facing_check");
JOAN_COUNTER(c_place_sum, "script_place_sum");
JOAN_COUNTER(c_key_alive, "script_unit_key_alive");
JOAN_COUNTER(c_group_442820, "script_unit_group_442820");
JOAN_COUNTER(c_select_unit, "script_select_unit");
JOAN_COUNTER(c_objective_set, "script_objective_set");
JOAN_COUNTER(c_random_lookup, "random_table_lookup");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct V3 {  // 12-byte by-value vector, copied bitwise: float members, because the callee (MiniMap::set_slot 0x00718010)
    float x, y, z;  // takes three floats, which System V x86-64 passes in XMM registers (three dwords go in integer registers)
};
static_assert(sizeof(V3) == 12, "vec3");

// ---------------------------------------------------------------------------- 0x00651fa0
static i32 JOAN_CDECL script_object_set_heading(i16 id, i32 deg) {
    if (joan::orig::as_units(kObjects)->list.empty_at(static_cast<i32>(id)) != 0) return 0;  // 0x00425d50
    const float rad = static_cast<float>(
        Fpu{true}.mul(static_cast<double>(deg), static_cast<double>(global<float>(d::flt_0_017453292))));
    void* const o = joan::orig::as_units(kObjects)->list.ptr_at(static_cast<i32>(id));  // 0x00425d20
    joan::orig::as_map_object(o)->set_yaw(joan::orig::arg::bits(rad));                                                 // 0x00499fd0
    return 1;
}

// ---------------------------------------------------------------------------- 0x006526e0
static i32 JOAN_CDECL script_unit_facing_check(i16 a, i32 b) {
    c_facing_check.hit();
    u8* const ua = unit_get(static_cast<i32>(a));
    (void)unit_missing(b);  // result unused in the original
    {
        // Dead: _ftol(heading(b) * 180.0f / pi) stored to an unused local.
        const double hb = static_cast<double>(heading(unit_get(b)));
        const Fpu fpu{true};
        (void)joan::hook::x87::ftol(
            fpu.div(fpu.mul(hb, static_cast<double>(global<float>(d::flt_180))), global<double>(d::dbl_3_141592653589793)));
    }
    {
        // Dead: heading(a) * 180.0f / pi stored to an unused float local.
        const double ha = static_cast<double>(heading(ua));
        const Fpu fpu{true};
        (void)fpu.div(fpu.mul(ha, static_cast<double>(global<float>(d::flt_180))), global<double>(d::dbl_3_141592653589793));
    }
    float pb[3];
    joan::orig::as_unit(unit_get(b))->position(joan::orig::arg::mptr(static_cast<void*>(pb)));  // 0x004fedb0
    float pa[3];
    joan::orig::as_unit(ua)->position(joan::orig::arg::mptr(static_cast<void*>(pa)));  // 0x004fedb0
    u8 angle[4] = {0, 0, 0, 0};  // [ebp-0x2c], byte angle object (ctor writes nothing)
    joan::orig::as_angle(static_cast<void*>(angle))->construct();  // 0x00576120
    const float dx = pb[0] - pa[0];
    const float dy = pb[1] - pa[1];
    joan::orig::as_angle(static_cast<void*>(angle))->from_atan2(joan::orig::arg::f32(dx), joan::orig::arg::f32(dy));  // 0x00576da0
    const float deg = static_cast<float>(joan::orig::arg::as<double>(joan::orig::as_angle(static_cast<void*>(angle))->byte_to_float()));  // 0x00576300
    const float h = joan::orig::arg::as<float>(joan::orig::as_unit(ua)->float_1e());  // 0x0050ac00 (fld [+0x1e])
    const double n = static_cast<double>(joan::hook::x87::ftol(static_cast<double>(deg)));
    const double hd = static_cast<double>(h);
    return (!(n < hd) && !(n > hd)) ? 1 : 0;  // equal or unordered
}

// ---------------------------------------------------------------------------- 0x00653300
static i32 JOAN_CDECL script_unit_427b50(i16 id, const char* v) {
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    joan::orig::as_unit(unit_get(static_cast<i32>(id)))->set_name_index(joan::orig::arg::mptr(v));  // 0x00427b50
    return 1;
}

// ---------------------------------------------------------------------------- 0x006539e0
static i32 JOAN_CDECL script_fx_718010(i16 a, V3 v, i8 b, i8 c) {
    joan::orig::as_mini_map(kFx)->set_waypoint(joan::orig::arg::i16(static_cast<i32>(a)), static_cast<char>(joan::orig::arg::i32(static_cast<i32>(b))), v, static_cast<char>(joan::orig::arg::i32(static_cast<i32>(c))));  // 0x00718010
    return 1;
}

// ---------------------------------------------------------------------------- 0x00653fe0
static u32 JOAN_CDECL script_49c470(i16 a, i16 b) {
    void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_place_array))->get_c70(static_cast<i32>(a)));  // 0x00456c70
    return joan::orig::arg::as<u32>(joan::orig::as_place(o)->count_hostiles(joan::orig::arg::u32(static_cast<i32>(b))));                           // 0x0049c470
}

// ---------------------------------------------------------------------------- 0x006548f0
static i32 JOAN_CDECL script_place_467dd0(i16 a, i16 b, i16 c, i32 d, const char* e, const char* g) {
    void* const place = joan::orig::arg::as<void*>(joan::orig::as_list(kPlaces)->get_e(static_cast<i32>(a)));  // 0x0043c450
    void* const q = vcall<void*>(place, 6);
    joan::orig::as_barracks_facility(q)->set_texts(joan::orig::arg::i16(static_cast<i32>(b)), joan::orig::arg::i16(static_cast<i32>(c)), joan::orig::arg::i16(d), e, g);  // 0x00467dd0
    return 1;
}

// ---------------------------------------------------------------------------- 0x00654b30
static i32 JOAN_CDECL script_place_sum(i16 id) {
    c_place_sum.hit();
    void* const place = joan::orig::arg::as<void*>(joan::orig::as_list(kPlaces)->get_e(static_cast<i32>(id)));  // 0x0043c450
    // The place's vtable slot 6 returns its Building (the one script_place_467dd0 calls Building::set_texts on).
    jo::Building* const q = jo::self<jo::Building>(vcall<void*>(place, 6));
    i32 sum = 0;
    for (i32 i = 0; i < static_cast<i32>(q->queue_n); ++i) {
        sum = static_cast<i32>(static_cast<u32>(sum) + static_cast<u32>(static_cast<i32>(q->queue_count[i])));
    }
    return sum;
}

// ---------------------------------------------------------------------------- 0x00655680
static i32 JOAN_CDECL script_unit_heading_deg(i16 id) {
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    const float q = heading(unit_get(static_cast<i32>(id))) / global<float>(d::flt_0_017453292);
    return joan::hook::x87::ftol(static_cast<double>(q));
}

// ---------------------------------------------------------------------------- 0x00655930
static i32 JOAN_CDECL script_unit_4fc220(i16 a, i16 b) {
    if (unit_missing(static_cast<i32>(a)) != 0) return 0;
    joan::orig::as_unit(unit_get(static_cast<i32>(a)))->mount(static_cast<i32>(b));  // 0x004fc220
    return static_cast<i32>(a);
}

// ---------------------------------------------------------------------------- 0x00655c80
static i32 JOAN_CDECL script_unit_2e5(i16 id) {
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    return static_cast<i32>(jo::as_unit(unit_get(static_cast<i32>(id)))->interact_item);
}

// ---------------------------------------------------------------------------- 0x00655e90
static i32 JOAN_CDECL script_unit_set_2eb(i16 id, i16 v) {
    if (unit_missing(static_cast<i32>(id)) == 0 && static_cast<i32>(jo::as_unit(unit_get(static_cast<i32>(id)))->bribe_state) != 2) {
        jo::as_unit(unit_get(static_cast<i32>(id)))->bribe_state = v;
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x00656350
static i32 JOAN_CDECL script_unit_set_name(i32 id, const char* name) {
    if (unit_missing(id) == 0 && name != nullptr) {
        std::strncpy(static_cast<char*>(static_cast<void*>(jo::as_unit(unit_get(id))->name_)), joan::orig::arg::str(name), 0x40u);  // strncpy
    }
    return 0;
}

// ---------------------------------------------------------------------------- 0x00656a60
static i32 JOAN_CDECL script_unit_key_alive(i32 key) {
    c_key_alive.hit();
    for (i32 i = joan::orig::as_dynarray(kUnitTable)->size(); i >= 1; --i) {  // 0x00417ad0
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
        if (e == nullptr) continue;
        if (static_cast<i32>(jo::as_unit(e)->bribe_state) == 2) continue;
        if (static_cast<i32>(jo::as_unit(e)->group_id) == key) return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------- 0x00658730
static i32 JOAN_CDECL script_unit_group_442820(i16 id) {
    c_group_442820.hit();
    u8* const unit = unit_get(static_cast<i32>(id));
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->is_busy()) == 0) return 0;  // 0x004b62b0
    u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_unit(unit)->leader());       // 0x004216d0
    if (g == nullptr || jo::as_unit(g)->group == nullptr) return 0;
    for (i32 k = static_cast<i32>(jo::as_group(jo::as_unit(g)->group)->count); k > 0; --k) {
        if (unit_missing(static_cast<i32>(jo::as_group(jo::as_unit(g)->group)->members[k - 1])) != 0) continue;
        u8* const m = unit_get(static_cast<i32>(jo::as_group(jo::as_unit(g)->group)->members[k - 1]));
        joan::orig::as_cell_object(m)->attach();  // 0x00442820
    }
    return 0;
}

// ---------------------------------------------------------------------------- 0x00658f70
static i32 JOAN_CDECL script_select_unit(i16 id, i16 mode) {
    c_select_unit.hit();
    u8* const unit = unit_get(static_cast<i32>(id));
    if (static_cast<i32>(mode) == 1) {
        jo::as_unit(unit)->layer = 0;
        global<i16>(d::g_a47c36) = id;
        joan::orig::as_player(obj(d::g_player_unit_recno))->set_unit(static_cast<i32>(id));  // 0x0042f9c0
        global<i32>(d::g_player_side) = static_cast<i32>(jo::as_unit(unit)->side);
        joan::orig::as_mini_map(kFx)->track_unit(joan::orig::arg::i16(static_cast<i32>(id)));  // 0x0070fb70
        u8 cam[0x28];                                     // [ebp-0x2c], 10 dwords
        void* const c1 = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCameras)->motion(0));  // 0x00563c80
        joan::orig::as_camera_motion(c1)->end(joan::orig::arg::mptr(static_cast<void*>(cam)));          // 0x00423fe0
        jo::view<i32>(jo::self<jo::CamPose>(cam)->target[0]) = jo::view<i32>(jo::as_unit(unit)->heading);
        void* const c2 = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCameras)->motion(0));  // 0x00563c80
        joan::orig::as_camera_motion(c2)->retarget(joan::orig::arg::mptr(static_cast<void*>(cam)));          // 0x00423f40
    } else if (static_cast<i32>(mode) == 2) {
        global<i32>(d::g_a5b9ec) = static_cast<i32>(id);
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659960
static i32 JOAN_CDECL script_clear_a59175() {
    global<i32>(d::g_get_a59175) = 0;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659ce0
static i32 JOAN_CDECL script_set_a591c6(i8 v) {
    global<i8>(d::g_a591c6) = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659e10
static i32 JOAN_CDECL script_a58fa7_4cdc40(i16 v) {
    joan::orig::as_scenario(obj(d::g_mission))->remove_id(joan::orig::arg::i16(static_cast<i32>(v)));  // 0x004cdc40
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065a2f0
static i32 JOAN_CDECL script_a5b9e8_432300(i32 v) {
    joan::orig::as_player(obj(d::g_player_unit_recno))->select_hero(joan::orig::arg::i32(v));  // 0x00432300
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065a720
static i16& objective_word(i32 n) {
    const u32 off = (static_cast<u32>(n) - 1u) * 0x1au;
    return global<i16>(d::g_a592aa + off);
}
static i32 JOAN_CDECL script_objective_set(i32 n, i32 v) {
    c_objective_set.hit();
    if (static_cast<i32>(objective_word(n)) == 0 && v != 0) {
        joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_OBJECTIVE_ADD));  // 0x004ceef0 'OBJECTIVE_ADD'
    }
    if (static_cast<i32>(objective_word(n)) != v) {
        objective_word(n) = static_cast<i16>(v);
        const i32 k = static_cast<i32>(static_cast<u32>(n) - 1u);
        if (v == 0) {
            joan::orig::as_hud(obj(d::g_player_hud))->remove_slot(joan::orig::arg::u32(k));  // 0x007072c0
        } else {
            joan::orig::as_hud(obj(d::g_player_hud))->push_log_entry(joan::orig::arg::i32(k));  // 0x00707400
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065aa20
static i32 JOAN_CDECL script_set_19a(i32 v) {
    if (global<i32>(d::g_mission) != 0) {
        jo::view<i32>(global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].u32_19a) = v;
    } else {
        global<i32>(d::g_a59155) = v;
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065b160
static u32 JOAN_CDECL script_type_5270b0(const char* v) {
    return joan::orig::arg::as<u32>(joan::orig::as_unit_types(obj(d::g_types))->find(v));  // 0x005270b0
}

// ---------------------------------------------------------------------------- 0x0065b7c0
static u32 JOAN_CDECL script_4cad00(const char* v) {
    if (v == nullptr) return 0;
    return joan::orig::arg::as<u32>(joan::orig::as_table(obj(d::g_recruit_res))->find_b4(v));  // 0x004cad00
}

// ---------------------------------------------------------------------------- 0x0065c1a0
static i32 JOAN_CDECL script_unit_124(i32 id) {
    if (unit_missing(id) != 0) return -1;
    return static_cast<i32>(jo::as_unit(unit_get(id))->level);
}

// ---------------------------------------------------------------------------- 0x0065c4d0
static i32 JOAN_CDECL script_a6a254_below() {
    return static_cast<u32>(global<i32>(d::g_a6a254)) < static_cast<u32>(global<i32>(d::g_now)) ? 1 : 0;
}

// ---------------------------------------------------------------------------- 0x0065cf80
static u32 JOAN_CDECL script_name_set_19(i32 i, i32 v) {
    if (joan::orig::arg::as<i32>(joan::orig::as_list(kNames)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) return 0;  // 0x004af300
    u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(kNames)->at_4af2d0(joan::orig::arg::u32(i)));       // 0x004af2d0
    jo::view<i32>(jo::as_message(e)->sound_id) = v;
    return joan::call::register_low_bits(e);  // eax = entry (x64: low half; also stored by escomp.cpp case 346 into the node value, scripts do not read it)
}

// ---------------------------------------------------------------------------- 0x0065e060
static std::uintptr_t JOAN_CDECL format_into(void* self, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    void* const text = joan::orig::arg::as<void*>(joan::orig::strings::format(joan::orig::arg::str(fmt), joan::orig::arg::mptr(static_cast<void*>(args))));  // 0x005a31e0
    va_end(args);
    return joan::orig::arg::as<u32>(joan::orig::as_string(self)->assign(joan::orig::arg::str(text)));  // 0x005a1440
}

// ---------------------------------------------------------------------------- 0x0065e930
static i32 JOAN_CDECL script_object_set_7e(i32 id, i32 flag) {
    if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00425d50
    u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
    jo::view<i32>(jo::as_map_object(o)->interact_trigger) = flag != 0 ? 0x200 : 0;
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065edd0
static i32 JOAN_CDECL script_a4f381_set_11(i16 v) {
    joan::orig::as_hud(kFx)->set_word_11(static_cast<i32>(v));  // 0x0065ee00
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065f740
static i32 JOAN_CDECL script_a4f381_7196e0(i32 a, i32 b, i32 c, i32 d, i32 e) {
    joan::orig::as_mini_map(kFx)->set_reveal_rect(joan::orig::arg::i32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d), joan::orig::arg::u32(e));  // 0x007196e0
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065fb50
static i32 JOAN_CDECL script_unit_set_30(i32 id, i8 v) {
    if (unit_missing(id) != 0) return 0;
    jo::as_unit(unit_get(id))->excluded = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00661e10
static i32 JOAN_CDECL random_table_lookup(const char* name, i32* out) {
    c_random_lookup.hit();
    for (i32 i = 0; i < 0x197; ++i) {
        const std::uintptr_t row = static_cast<std::uintptr_t>(i) * 0x3c;
        if (stricmp_(str(d::g_a1a254 + row), name) == 0) {
            *out = img::global<const i32>(d::g_a1a288 + row);
            return img::global<const i32>(d::g_a1a250 + row);
        }
    }
    return 0;
}

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_07", name, addr, fn)
B97("script_object_set_heading", 0x00651fa0, &script_object_set_heading);
B97("script_unit_facing_check", 0x006526e0, &script_unit_facing_check);
B97("script_unit_427b50", 0x00653300, &script_unit_427b50);
B97("script_fx_718010", 0x006539e0, &script_fx_718010);
B97("script_49c470", 0x00653fe0, &script_49c470);
B97("script_place_467dd0", 0x006548f0, &script_place_467dd0);
B97("script_place_sum", 0x00654b30, &script_place_sum);
B97("script_unit_heading_deg", 0x00655680, &script_unit_heading_deg);
B97("script_unit_4fc220", 0x00655930, &script_unit_4fc220);
B97("script_unit_2e5", 0x00655c80, &script_unit_2e5);
B97("script_unit_set_2eb", 0x00655e90, &script_unit_set_2eb);
B97("script_unit_set_name", 0x00656350, &script_unit_set_name);
B97("script_unit_key_alive", 0x00656a60, &script_unit_key_alive);
B97("script_unit_group_442820", 0x00658730, &script_unit_group_442820);
B97("script_select_unit", 0x00658f70, &script_select_unit);
B97("script_clear_a59175", 0x00659960, &script_clear_a59175);
B97("script_set_a591c6", 0x00659ce0, &script_set_a591c6);
B97("script_a58fa7_4cdc40", 0x00659e10, &script_a58fa7_4cdc40);
B97("script_a5b9e8_432300", 0x0065a2f0, &script_a5b9e8_432300);
B97("script_objective_set", 0x0065a720, &script_objective_set);
B97("script_set_19a", 0x0065aa20, &script_set_19a);
B97("script_type_5270b0", 0x0065b160, &script_type_5270b0);
B97("script_4cad00", 0x0065b7c0, &script_4cad00);
B97("script_unit_124", 0x0065c1a0, &script_unit_124);
B97("script_a6a254_below", 0x0065c4d0, &script_a6a254_below);
B97("script_name_set_19", 0x0065cf80, &script_name_set_19);
B97("format_into", 0x0065e060, &format_into);
B97("script_object_set_7e", 0x0065e930, &script_object_set_7e);
B97("script_a4f381_set_11", 0x0065edd0, &script_a4f381_set_11);
B97("script_a4f381_7196e0", 0x0065f740, &script_a4f381_7196e0);
B97("script_unit_set_30", 0x0065fb50, &script_unit_set_30);
B97("random_table_lookup", 0x00661e10, &random_table_lookup);
#undef B97

}  // namespace game_b97_07
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

using joan::orig::helpers::at;

// Virtual thiscall: vtable slot at byte offset `off`.
template <class R = void, class... A>
R vcall(void* o, std::int32_t off, A... args) {
    return thiscall<R>(reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(o)[joan::call::x86_vslot(off)]), o, args...);  // slot off / 4 (x86 byte offset); o: object of varying class
}

void* const kUnits = obj(d::g_units);
void* const kObjects = obj(d::g_items);

std::int32_t list_count(void* l) { return joan::orig::as_dynarray(l)->size(); }  // 0x00417ad0
void* list_item(void* l, std::int32_t i) { return joan::orig::as_dynarray(l)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0
std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
void* unit_get(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
std::int32_t object_missing(std::int32_t id) { return joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(id)); }  // 0x00425d50
void* object_get(std::int32_t id) { return joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id)); }  // 0x00425d20

struct V3 {
    float x, y, z;
};

JOAN_COUNTER(c_enemy_near, "unit_has_enemy_near");
JOAN_COUNTER(c_check, "unit_check_652c10");
JOAN_COUNTER(c_find_group, "find_unit_in_groups");
JOAN_COUNTER(c_refresh, "unit_refresh_items");

struct H {
    // 0x0065afb0: the dword at this+0xf. Every caller (esfuncs.cpp, 5 sites) passes the static object at 0x00a5d380,
    // whose bytes the data image keeps as separate slots: +0xf is the i32 slot 0x00a5d38f (on x64 the slots need not
    // stay 0xf apart).
    std::uint32_t get_f_65afb0() { return static_cast<std::uint32_t>(global<std::int32_t>(d::g_a5d38f)); }
};

// 0x006524a0
std::int32_t JOAN_CDECL object_d1(std::int32_t id) {
    if (object_missing(id) != 0) return -1;
    void* const o = object_get(id);
    if (static_cast<std::int32_t>(jo::as_map_object(o)->class_id) != 1) return -1;
    return jo::as_gate(vcall<void*>(o, 0x4c))->state;  // slot 0x4c returns the Gate
}

// 0x00652820
std::int32_t JOAN_CDECL unit_has_enemy_near(std::int16_t id) {
    c_enemy_near.hit();
    if (unit_missing(id) != 0) return 0;
    std::uint8_t found = 0;
    void* const unit = unit_get(id);
    V3 pos;
    joan::orig::as_unit(unit)->position(&pos);  // 0x004fedb0
    for (std::int32_t i = list_count(kUnits); i > 0; --i) {
        void* const other = list_item(kUnits, i);
        if (other == nullptr) continue;
        if (static_cast<std::int32_t>(id) == i) continue;
        const std::int32_t kind = jo::as_unit(other)->anim_kind;
        if (kind != 0x40 && kind != 0x41 && kind != 0x42) continue;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(static_cast<std::int16_t>(i)))) == 0) continue;  // 0x005094d0
        const float a = joan::orig::arg::as<float>(joan::orig::as_unit(unit)->heading_to_unit(joan::orig::arg::i32(i)));  // 0x00505960
        if (!(a >= global<float>(d::flt_m90))) continue;  // a < -90 or NaN
        if (a > global<float>(d::flt_90)) continue;      // a > 90
        V3 opos;
        joan::orig::as_unit(other)->position(&opos);  // 0x004fedb0
        V3 tmp;
        const V3 d = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &pos, &opos));  // 0x0060cc90
        const V3 copy = d;
        if (import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(&copy)) > static_cast<double>(global<float>(d::flt_3600)))
            continue;  // length_sq > 3600
        found = 1;
        break;
    }
    return found != 0 ? 1 : 0;
}

// 0x00652c10
std::int32_t JOAN_CDECL unit_check_652c10(std::int16_t id) {
    c_check.hit();
    if (unit_missing(id) != 0) return 0;
    void* const unit = unit_get(id);
    if (unit_missing(global<std::int32_t>(d::g_player_unit_recno)) == 0) {
        void* const leader = unit_get(global<std::int32_t>(d::g_player_unit_recno));
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(leader)->hostile_to(joan::orig::arg::i16(id))) != 0) return 1;  // 0x005094d0
    }
    const joan::hook::x87::Fpu fpu{true};
    const double hundred = global<float>(d::flt_100);
    const std::int32_t a = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->value_149());  // 0x00523700
    const float scale = static_cast<float>(
        fpu.div(fpu.mul(static_cast<double>(a), static_cast<double>(global<std::int32_t>(d::g_unit_egatk_mp))), hundred));
    const std::int32_t x = joan::orig::arg::as<std::int32_t>(joan::orig::as_modifiers(jo::as_unit(unit)->modifiers)->sum(0x15, static_cast<std::int32_t>(jo::as_unit(unit)->id)));  // 0x00497e10
    const std::int32_t need = joan::hook::x87::ftol(fpu.div(fpu.mul(static_cast<double>(x), static_cast<double>(scale)), hundred));
    const std::int32_t y = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hit_points_max());  // 0x00523320
    const std::int32_t lim = joan::hook::x87::ftol(
        fpu.div(fpu.mul(static_cast<double>(y), static_cast<double>(global<std::int32_t>(d::g_a3cd1c))), hundred));
    if (jo::as_unit(unit)->mana < need) return 0;
    if (!(jo::as_unit(unit)->hit_points > static_cast<std::int32_t>(static_cast<std::uint32_t>(lim) * 3u))) return 0;
    if (jo::as_unit(unit)->anim_set != nullptr) {
        const std::int32_t r = joan::orig::arg::as<std::int32_t>(joan::orig::as_list(jo::as_unit(unit)->anim_set)->index_of_416dc0(0x1e));  // 0x00416dc0
        if (r != 0) {
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::as_unit(unit)->anim_set)->entry(joan::orig::arg::u32(r)));  // 0x004168d0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_set_entry(p)->is_yes()) != 0) return 1;              // 0x00416ed0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->find_link(0xd, joan::orig::arg::str(0))) >= 1) return 1;    // 0x0050d3d0
        }
    }
    return 0;
}

// 0x006581f0
std::int32_t JOAN_CDECL find_unit_in_groups(std::int16_t kind) {
    c_find_group.hit();
    void* const groups = obj(d::g_nations);
    for (std::int32_t g = list_count(groups); g > 0; --g) {
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(groups)->is_null(joan::orig::arg::i32(g))) != 0) continue;  // 0x00484870
        void* const grp = joan::orig::as_dynarray(groups)->ptr_at(joan::orig::arg::i32(g));  // 0x0047b600
        for (std::int32_t j = jo::view<std::int16_t>(jo::as_nation(grp)->leader_count); j > 0; --j) {
            const std::int32_t id = jo::as_nation(grp)->leaders[j - 1].value;
            if (unit_missing(id) != 0) continue;
            void* const u = unit_get(id);
            if (static_cast<std::int32_t>(jo::as_unit(u)->group_id) == static_cast<std::int32_t>(kind)) return id;
        }
    }
    return 0;
}

// 0x0065b700
std::int32_t JOAN_CDECL object_is_type1(std::int32_t id) {
    if (object_missing(id) != 0) return 0;
    return static_cast<std::int32_t>(jo::as_map_object(object_get(id))->class_id) == 1 ? 1 : 0;
}
// 0x0065b760
std::int32_t JOAN_CDECL object_is_type4(std::int32_t id) {
    if (object_missing(id) != 0) return 0;
    return static_cast<std::int32_t>(jo::as_map_object(object_get(id))->class_id) == 4 ? 1 : 0;
}

// 0x0065b9b0 helper: one item slot. `ref` = the resolved object id (or 0).
void refresh_item(void* unit, std::int32_t ref, std::int32_t which) {
    if (ref == 0) return;
    void* const o = object_get(ref);
    vcall(o, 0x1c);
    void* const o2 = object_get(ref);
    void* const def = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::as_map_object(o2)->type_id)));  // 0x00425ce0
    joan::orig::as_cell_object(unit)->attach_unit(joan::orig::arg::i16(static_cast<std::int16_t>(ref)), static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_item_def(def)->bone)), static_cast<std::int8_t>(joan::orig::arg::i32(which)), joan::orig::arg::mptr(0));  // 0x0050a480
}
std::int32_t resolve_item(void* unit, std::int16_t jo::Unit::* field, std::int32_t which) {
    if (object_missing(jo::as_unit(unit)->*field) == 0) return jo::as_unit(unit)->*field;
    void* const q = joan::orig::arg::as<void*>(joan::orig::as_modifiers(jo::as_unit(unit)->modifiers)->slot(joan::orig::arg::i32(which)));  // 0x004974f0
    if (q == nullptr) return 0;
    return joan::orig::arg::as<std::int32_t>(joan::orig::as_units(kObjects)->create_49ea90(joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_inv_item(q)->type))));  // 0x0049ea90
}
std::int32_t JOAN_CDECL unit_refresh_items(std::int16_t id) {
    c_refresh.hit();
    if (unit_missing(id) != 0) return 0;
    void* const unit = unit_get(id);
    void* const inv = jo::as_unit(unit)->modifiers;
    if (jo::as_modifiers(inv)->hand_mode == 0) return 1;
    if (static_cast<std::int32_t>(jo::as_modifiers(jo::as_unit(unit)->modifiers)->hand_mode) <= 1) {
        refresh_item(unit, resolve_item(unit, &jo::Unit::carried_a, 0), 0);
        refresh_item(unit, resolve_item(unit, &jo::Unit::carried_b, 1), 1);
    } else {
        std::int32_t ref;
        if (object_missing(jo::as_unit(unit)->carried_b) == 0) {
            ref = jo::as_unit(unit)->carried_b;
        } else {
            void* const q = joan::orig::arg::as<void*>(joan::orig::as_unit_data(jo::as_unit(unit)->modifiers)->slot_a());  // 0x00497560
            ref = q != nullptr ? joan::orig::arg::as<std::int32_t>(joan::orig::as_units(kObjects)->create_49ea90(joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_inv_item(q)->type))))
                               : 0;  // 0x0049ea90
        }
        refresh_item(unit, ref, 1);
    }
    return 1;
}

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("object_d1", 0x006524a0, &object_d1);
B91("unit_has_enemy_near", 0x00652820, &unit_has_enemy_near);
B91("unit_check_652c10", 0x00652c10, &unit_check_652c10);
B91("find_unit_in_groups", 0x006581f0, &find_unit_in_groups);
B91("Obj65a::get_f", 0x0065afb0, &H::get_f_65afb0);
B91("object_is_type1", 0x0065b700, &object_is_type1);
B91("object_is_type4", 0x0065b760, &object_is_type4);
B91("unit_refresh_items", 0x0065b9b0, &unit_refresh_items);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b96_07.cpp (game/b96_07)
namespace {
namespace game_b96_07 {

using joan::orig::helpers::at;

struct Vec3Raw {  // 12-byte by-value vector of three floats: the callee (GroundObject::set_position 0x004fedf0) takes floats, and on
    float x, y, z;    // System V x86-64 three floats travel in XMM registers while three dwords would go in integer registers
};
static_assert(sizeof(Vec3Raw) == 12, "Vec3Raw");
Vec3Raw vec3_raw(u32 x, u32 y, u32 z) {  // the script's dwords are the floats' bits
    Vec3Raw v;
    std::memcpy(&v.x, &x, 4);
    std::memcpy(&v.y, &y, 4);
    std::memcpy(&v.z, &z, 4);
    return v;
}

void* const kUnitTable = obj(d::g_units);
void* const kObjects = obj(d::g_items);
void* const kSquads = obj(d::g_place_array);
void* const kPlaces = obj(d::g_places);
void* const kGroups = obj(d::g_nations);

JOAN_COUNTER(c_squad_rates, "script_squad_set_rates");
JOAN_COUNTER(c_unit_place, "script_unit_place");
JOAN_COUNTER(c_set_group, "script_units_set_group");
JOAN_COUNTER(c_clear_1c5, "script_units_clear_1c5");
JOAN_COUNTER(c_attribute, "script_attribute");
JOAN_COUNTER(c_apply_group, "script_unit_apply_group");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

// ---------------------------------------------------------------------- 0x006525f0
static i32 JOAN_CDECL script_object_set_cf(i32 id, i32 v) {
    if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(id)) != 0) return -1;  // 0x00425d50
    void* const o = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
    u8* const r = vcall0<u8*>(o, 0x4c);
    jo::view<i8>(jo::as_gate(r)->invincible) = static_cast<i8>(v);  // slot 0x4c returns the Gate
    return static_cast<i32>(static_cast<i8>(v));
}

// ---------------------------------------------------------------------- 0x006532a0
static i32 JOAN_CDECL script_unit_set_1c7(i32 id, i32 v) {
    const i32 n = static_cast<i16>(id);
    if (unit_missing(n) != 0) return 0;
    jo::as_unit(unit_get(n))->treasure_level = v;
    return 1;
}

// ---------------------------------------------------------------------- 0x006539a0
static i32 JOAN_CDECL script_set_a5027e(i32 v) {
    global<u8>(d::g_a5027e) = static_cast<u8>(v);
    global<u8>(d::g_a5027f) = 2;
    joan::orig::as_game(obj(d::g_game_a50088))->set_state(0xcu);  // 0x004d7e70
    return 1;
}

// ---------------------------------------------------------------------- 0x006541a0
static i32 JOAN_CDECL script_squad_1ca(i32 id) {
    u8* const s = joan::orig::arg::as<u8*>(joan::orig::as_list(kSquads)->get_c70(static_cast<i32>(static_cast<i16>(id))));  // 0x00456c70
    return jo::as_place(s)->spawn_count;
}

// ---------------------------------------------------------------------- 0x00654830
static i32 JOAN_CDECL script_squad_set_rates(i32 id, i32 deg) {
    c_squad_rates.hit();
    u8* const s = joan::orig::arg::as<u8*>(joan::orig::as_list(kSquads)->get_c70(joan::orig::arg::i32(id)));  // 0x00456c70
    for (i32 i = 0; i < jo::as_place(s)->slot_count; ++i) {
        jo::as_place(s)->slot_yaw[i + 1] = static_cast<float>(
            Fpu{true}.mul(static_cast<double>(deg), static_cast<double>(global<float>(d::flt_0_017453292))));
    }
    return 1;
}

// ---------------------------------------------------------------------- 0x00654c20
static i32 JOAN_CDECL script_place_set_98(i32 id, i32 v) {
    void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(kPlaces)->get_e(static_cast<i32>(static_cast<i16>(id))));  // 0x0043c450
    u8* const r = vcall0<u8*>(p, 0x24);
    jo::self<jo::FlagFacility>(r)->flag_98 = static_cast<u8>(v);
    return 1;
}

// ---------------------------------------------------------------------- 0x006554f0
static i32 JOAN_CDECL script_unit_place(i32 id, u32 x, u32 y, u32 z) {
    c_unit_place.hit();
    const i32 n = static_cast<i16>(id);
    if (unit_missing(n) != 0) return 0;
    const Vec3Raw v = vec3_raw(x, y, z);
    u8* const unit = unit_get(n);
    joan::orig::as_unit(unit)->set_group(global<jo::World*>(d::g_world));  // 0x004fed00
    joan::orig::as_ground_object(unit)->set_position(v);                       // 0x004fedf0
    return n;
}

// ---------------------------------------------------------------------- 0x00655870
static i32 JOAN_CDECL script_unit_call_442820(i32 id) {
    const i32 n = static_cast<i16>(id);
    if (unit_missing(n) != 0) return 0;
    joan::orig::as_cell_object(unit_get(n))->attach();  // 0x00442820
    return n;
}

// ---------------------------------------------------------------------- 0x00655bd0
static i32 JOAN_CDECL script_unit_2e3(i32 id) {
    const i32 n = static_cast<i16>(id);
    if (unit_missing(n) != 0) return 0;
    return jo::as_unit(unit_get(n))->hit_by;
}

// ---------------------------------------------------------------------- 0x00655e30
static i32 JOAN_CDECL script_unit_set_2e9(i32 id, i32 v) {
    const i32 n = static_cast<i16>(id);
    if (unit_missing(n) == 0) jo::view<u16>(jo::as_unit(unit_get(n))->can_interact) = static_cast<u16>(v);
    return 1;
}

// ---------------------------------------------------------------------- 0x00656210
static i32 JOAN_CDECL script_unit_set_114(i32 id, i32 v) {
    if (unit_missing(id) == 0) jo::as_unit(unit_get(id))->markers = v;
    return 1;
}

// ---------------------------------------------------------------------- 0x00656990
static i32 JOAN_CDECL script_unit_set_135(i32 id, i32 v) {
    if (unit_missing(id) != 0) return 0;
    jo::as_unit(unit_get(id))->hit_points = v;
    return 1;
}

// ---------------------------------------------------------------------- 0x00658420
static i32 JOAN_CDECL script_units_set_group(i32 g, const char* k) {
    c_set_group.hit();
    const i32 key = joan::orig::arg::as<i32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(k));  // 0x004b91a0
    const i32 count = joan::orig::arg::as<i32>(joan::orig::as_list(kGroups)->find_by_word2(joan::orig::arg::i32(key)));      // 0x004b67e0
    if (count <= 0) return 0;
    for (i32 i = joan::orig::as_dynarray(kUnitTable)->size(); i > 0; --i) {  // 0x00417ad0
        if (unit_missing(i) != 0) continue;
        u8* const unit = unit_get(i);
        if (static_cast<i32>(jo::as_unit(unit)->group_id) == static_cast<i32>(static_cast<i16>(g)))
            jo::view<u16>(jo::as_unit(unit)->side) = static_cast<u16>(count);
    }
    return 1;
}

// ---------------------------------------------------------------------- 0x00658d90
static i32 JOAN_CDECL script_units_clear_1c5(i32 g) {
    c_clear_1c5.hit();
    for (i32 i = joan::orig::as_dynarray(kUnitTable)->size(); i > 0; --i) {  // 0x00417ad0
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
        if (unit == nullptr) continue;
        if (static_cast<i32>(jo::as_unit(unit)->group_id) == g) jo::view<u16>(jo::as_unit(unit)->drop_item) = 0;
    }
    return 1;
}

// ---------------------------------------------------------------------- 0x006596a0
static i32 JOAN_CDECL script_current_1aa() {
    if (global<i32>(d::g_mission) == 0) return global<i32>(d::g_a59165);
    return global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].time_left_ms;
}

// ---------------------------------------------------------------------- 0x006598c0
static i32 JOAN_CDECL script_set_a591cc(i32 v) {
    global<i32>(d::g_get_a591cc) = v;
    return 1;
}

// ---------------------------------------------------------------------- 0x00659a60
static i32 JOAN_CDECL script_add_a591d8(i32 v) {
    global<u32>(d::g_get_a591d8) += static_cast<u32>(v);
    return 1;
}

// ---------------------------------------------------------------------- 0x00659db0
static i32 JOAN_CDECL script_counter_post_inc() {
    const i32 old = global<i32>(d::g_get_a591c8);
    global<u32>(d::g_get_a591c8) = static_cast<u32>(old) + 1u;
    return old;
}

// ---------------------------------------------------------------------- 0x0065a1b0
static i32 JOAN_CDECL script_attribute(const char* name, i32 n) {
    c_attribute.hit();
    u32 k = 0;
    if (stricmp_(name, str(d::s_STRENGTH)) == 0) {         // "STRENGTH"
        k = 0;
    } else if (stricmp_(name, str(d::s_DEXTERITY)) == 0) {  // "DEXTERITY"
        k = 1;
    } else if (stricmp_(name, str(d::s_LEADERSHIP)) == 0) {  // "LEADERSHIP"
        k = 2;
    } else if (stricmp_(name, str(d::s_DEFENSE)) == 0) {  // "DEFENSE"
        k = 3;
    }
    return global<i8>(d::g_a5922f + k * 5u + static_cast<u32>(n));
}

// ---------------------------------------------------------------------- 0x0065a6a0
static i32 JOAN_CDECL script_table_set_word(i32 n, i32 v) {
    global<u16>(d::g_a592a6 + (static_cast<u32>(n) - 1u) * 0x1au) = static_cast<u16>(v);
    return 1;
}

// ---------------------------------------------------------------------- 0x0065a960
static i32 JOAN_CDECL script_set_bit(i32 bit, i32 v) {
    // `mov al, byte; push eax`: callee masks the value with 0xff.
    joan::orig::as_game(obj(d::g_game_state))->set_flag_1bd(joan::orig::arg::i32(bit), static_cast<i32>(static_cast<u8>(v)));  // 0x004722a0
    return 1;
}

// ---------------------------------------------------------------------- 0x0065af50
static i32 JOAN_CDECL script_state_busy() {
    void* const s = obj(d::g_game_a5d380);
    if (joan::orig::arg::as<i32>(joan::orig::as_shop(s)->state()) == 0) return 1;  // 0x0065afb0
    if (joan::orig::arg::as<i32>(joan::orig::as_shop(s)->state()) == 2) return 1;  // 0x0065afb0
    if (global<i8>(d::g_a5d3c0) != 0) return 1;
    return 0;
}

// ---------------------------------------------------------------------- 0x0065b5b0
static u32 JOAN_CDECL script_object_49f4b0(const char* id) {
    if (id == nullptr) return 0;
    return joan::orig::arg::as<u32>(joan::orig::as_group(kObjects)->find_name(id));  // 0x0049f4b0
}

// ---------------------------------------------------------------------- 0x0065c220
static i32 JOAN_CDECL script_add_a596c4(i32 v) {
    global<u32>(d::g_get_a596c4) += static_cast<u32>(v);
    return 1;
}

// ---------------------------------------------------------------------- 0x0065c500
static i32 JOAN_CDECL script_set_a596ab(i32 v) {
    global<u8>(d::g_a596ab) = static_cast<u8>(v);
    return 1;
}

// ---------------------------------------------------------------------- 0x0065d350
static i32 JOAN_CDECL script_unit_apply_group(i32 id, i32 v) {
    c_apply_group.hit();
    if (unit_missing(id) != 0) return 0;
    joan::orig::script_set_unit_light(joan::orig::arg::i32(id), joan::orig::arg::i32(v));  // 0x0065d0d0
    if (jo::as_unit(unit_get(id))->group == nullptr) return 1;
    for (i32 i = jo::as_group(jo::as_unit(unit_get(id))->group)->count; i > 0; --i) {
        const i32 m = jo::as_group(jo::as_unit(unit_get(id))->group)->members[i - 1];
        if (unit_missing(m) != 0) continue;
        const i32 m2 = jo::as_group(jo::as_unit(unit_get(id))->group)->members[i - 1];
        joan::orig::script_set_unit_light(joan::orig::arg::i32(m2), joan::orig::arg::i32(v));  // 0x0065d0d0
    }
    return 1;
}

// ---------------------------------------------------------------------- 0x0065e7d0
static i32 JOAN_CDECL script_set_a50522(i32 v) {
    if (v <= 0) return 0;
    global<i32>(d::g_a50522) = v;
    return 1;
}

// ---------------------------------------------------------------------- 0x0065e9a0
static i32 JOAN_CDECL script_unit_set_3c0(i32 id, i32 v) {
    if (unit_missing(id) != 0) return 0;
    jo::as_unit(unit_get(id))->can_be_ridden = static_cast<u8>(v);
    return 1;
}

// ---------------------------------------------------------------------- 0x0065f820
static i32 JOAN_CDECL script_call_7198d0(i32 v) {
    joan::orig::as_mini_map(obj(d::g_mini_map))->clear_reveal(joan::orig::arg::i32(v));  // 0x007198d0
    return 1;
}

// ---------------------------------------------------------------------- 0x0065fba0
static i32 JOAN_CDECL script_set_a596c8(i32 v) {
    global<i32>(d::g_a596c8) = v;
    return 1;
}

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // ------------------------------------------------------------------ 0x0065ee00
    void set_word_11(i32 v) { jo::view<u16>(jo::self<jo::MiniMap>(this)->show_gold) = static_cast<u16>(v); }  // x86: +0x11 of the MiniMap object (g_a4f381)
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_07", name, addr, fn)
B96("script_object_set_cf", 0x006525f0, &script_object_set_cf);
B96("script_unit_set_1c7", 0x006532a0, &script_unit_set_1c7);
B96("script_set_a5027e", 0x006539a0, &script_set_a5027e);
B96("script_squad_1ca", 0x006541a0, &script_squad_1ca);
B96("script_squad_set_rates", 0x00654830, &script_squad_set_rates);
B96("script_place_set_98", 0x00654c20, &script_place_set_98);
B96("script_unit_place", 0x006554f0, &script_unit_place);
B96("script_unit_call_442820", 0x00655870, &script_unit_call_442820);
B96("script_unit_2e3", 0x00655bd0, &script_unit_2e3);
B96("script_unit_set_2e9", 0x00655e30, &script_unit_set_2e9);
B96("script_unit_set_114", 0x00656210, &script_unit_set_114);
B96("script_unit_set_135", 0x00656990, &script_unit_set_135);
B96("script_units_set_group", 0x00658420, &script_units_set_group);
B96("script_units_clear_1c5", 0x00658d90, &script_units_clear_1c5);
B96("script_current_1aa", 0x006596a0, &script_current_1aa);
B96("script_set_a591cc", 0x006598c0, &script_set_a591cc);
B96("script_add_a591d8", 0x00659a60, &script_add_a591d8);
B96("script_counter_post_inc", 0x00659db0, &script_counter_post_inc);
B96("script_attribute", 0x0065a1b0, &script_attribute);
B96("script_table_set_word", 0x0065a6a0, &script_table_set_word);
B96("script_set_bit", 0x0065a960, &script_set_bit);
B96("script_state_busy", 0x0065af50, &script_state_busy);
B96("script_object_49f4b0", 0x0065b5b0, &script_object_49f4b0);
B96("script_add_a596c4", 0x0065c220, &script_add_a596c4);
B96("script_set_a596ab", 0x0065c500, &script_set_a596ab);
B96("script_unit_apply_group", 0x0065d350, &script_unit_apply_group);
B96("script_set_a50522", 0x0065e7d0, &script_set_a50522);
B96("script_unit_set_3c0", 0x0065e9a0, &script_unit_set_3c0);
B96("Obj::set_word_11", 0x0065ee00, &H::set_word_11);
B96("script_call_7198d0", 0x0065f820, &script_call_7198d0);
B96("script_set_a596c8", 0x0065fba0, &script_set_a596c8);
#undef B96

}  // namespace game_b96_07
}  // namespace

// ---------------------------------------- from modules/game_b107_04.cpp (game/b107_04)
namespace {
namespace game_b107_04 {

// 0x00652a40
i32 JOAN_CDECL unit_check_652a40(u32 id_raw) {
    const i32 id = static_cast<i16>(static_cast<u16>(id_raw));
    void* const units = obj(d::g_units);
    if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
    void* const unit = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
    return static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->use_spiritual_item_4fd980(0)));   // 0x004fd980
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_04", name, addr, fn)
B("unit_check_652a40", 0x00652a40, &unit_check_652a40);
#undef B

}  // namespace game_b107_04
}  // namespace

// ---------------------------------------- from modules/game_b104_12.cpp (game/b104_12)
namespace {
namespace game_b104_12 {

void* const kUnits = obj(d::g_units);
void* const kFormations = obj(d::g_types);

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12);
struct Mat44 {
    float m[16];
};

JOAN_COUNTER(c_assign_slot, "Formation::assign_slot");
JOAN_COUNTER(c_unit_flag, "Script::get_unit_flag22");

i32 unit_dead(i16 id) { return joan::orig::as_dynarray(kUnits)->empty_at(static_cast<i32>(id)); }  // 0x00428b70
void* unit_ptr(i16 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(id)); }  // 0x0041b3c0

// ---------------------------------------------------------------------------- getters
i32 JOAN_CDECL get_a46059() { return global<i16>(d::g_get_a46059); }
i32 JOAN_CDECL get_word_ptr(const i16* p) { return *p; }
u32 JOAN_CDECL get_a5b9e8() { return global<u32>(d::g_player_unit_recno); }
u32 JOAN_CDECL get_a47bb0() { return global<u32>(d::g_get_a47bb0); }
u32 JOAN_CDECL get_a47bb8() { return global<u32>(d::g_get_a47bb8); }
u32 JOAN_CDECL get_a47bbc() { return global<u32>(d::g_get_a47bbc); }
u32 JOAN_CDECL get_a47bc0() { return global<u32>(d::g_get_a47bc0); }
u32 JOAN_CDECL get_a591cc() { return global<u32>(d::g_get_a591cc); }
u32 JOAN_CDECL get_a59171() { return global<u32>(d::g_get_a59171); }
u32 JOAN_CDECL get_a59175() { return global<u32>(d::g_get_a59175); }
u32 JOAN_CDECL get_a59179() { return global<u32>(d::g_get_a59179); }
u32 JOAN_CDECL get_a591d8() { return global<u32>(d::g_get_a591d8); }
u32 JOAN_CDECL get_a591a9() { return global<u32>(d::g_get_a591a9); }
u32 JOAN_CDECL get_a591b6() { return global<u32>(d::g_get_a591b6); }
u32 JOAN_CDECL get_a591be() { return global<u32>(d::g_get_a591be); }
u32 JOAN_CDECL get_a591c2() { return global<u32>(d::g_get_a591c2); }
u32 JOAN_CDECL get_a591ba() { return global<u32>(d::g_get_a591ba); }
u32 JOAN_CDECL get_a591c8() { return global<u32>(d::g_get_a591c8); }
u32 JOAN_CDECL get_a591ad() { return global<u32>(d::g_get_a591ad); }
u32 JOAN_CDECL get_a591b1() { return global<u32>(d::g_get_a591b1); }
u32 JOAN_CDECL get_a5918d() { return global<u32>(d::g_get_a5918d); }
u32 JOAN_CDECL get_a59199() { return global<u32>(d::g_get_a59199); }
u32 JOAN_CDECL get_a591a5() { return global<u32>(d::g_get_a591a5); }
u32 JOAN_CDECL get_a59294() { return global<u32>(d::g_get_a59294); }
u32 JOAN_CDECL get_a596c4() { return global<u32>(d::g_get_a596c4); }
u32 JOAN_CDECL get_a5b9f8() { return global<u32>(d::g_control_mode); }
i32 JOAN_CDECL get_a591c7() { return global<i8>(d::g_get_a591c7); }

float JOAN_CDECL wrap_659740() { return joan::orig::arg::as<float>(joan::orig::as_calendar(obj(d::g_calendar))->seconds()); }  // 0x00659760
u32 JOAN_CDECL wrap_65a2d0() { return joan::orig::arg::as<u32>(joan::orig::as_counter(obj(d::g_scenarios))->at_least_10()); }      // 0x0041c7e0
u32 JOAN_CDECL wrap_65a350() { return joan::orig::arg::as<u32>(joan::orig::as_game(obj(d::g_game_state))->add_end_condition()); }      // 0x00471e20
u32 JOAN_CDECL wrap_65a370() { return joan::orig::arg::as<u32>(joan::orig::as_game(obj(d::g_game_state))->pop_end_condition()); }      // 0x00471ef0
u32 JOAN_CDECL wrap_65afd0() { return joan::orig::arg::as<u32>(joan::orig::as_shop(obj(d::g_game_a5d380))->state()); }      // 0x0065afb0

// ---------------------------------------------------------------------------- 0x00652aa0
i32 JOAN_CDECL unit_flag22(i16 id) {
    c_unit_flag.hit();
    if (unit_dead(id) != 0) return 0;
    void* const u = unit_ptr(id);
    return static_cast<i8>(joan::orig::arg::as<char>(joan::orig::as_unit(u)->has_order_for(0x22u)));  // 0x00500d40
}

// ---------------------------------------------------------------------------- 0x0065f160
i32 JOAN_CDECL assign_slot(i16 unit_id, i16 leader_id, const char* kind) {
    c_assign_slot.hit();
    if (unit_dead(unit_id) != 0) return -1;
    if (unit_dead(leader_id) != 0) return -1;
    void* const a = unit_ptr(unit_id);
    void* const b = unit_ptr(leader_id);
    i8 mask;
    if (static_cast<i8>(kind[0]) == 0x4d) {
        mask = 1;
    } else if (static_cast<i8>(kind[0]) == 0x46) {
        mask = 2;
    } else {
        return -1;
    }
    if (static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_unit(a)->is_kind_special())) == 0) return 0;  // 0x00508ab0
    Vec3 tmp;
    joan::orig::as_unit(a)->position(&tmp);  // 0x004fedb0
    u8* const form = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kFormations)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(b)->type_id))));  // 0x0042d0a0
    for (i32 i = 0; i < jo::self<jo::UnitType>(form)->slot_count; ++i) {
        if ((static_cast<i32>(jo::view<i8*>(jo::as_unit_type(form)->buffers[2])[i]) & static_cast<i32>(mask)) == 0) continue;
        if (jo::view<i32*>(jo::as_unit(b)->mask_slots)[i] > 0) continue;

        Vec3 slot = reinterpret_cast<const Vec3*>(jo::view<u8*>(jo::as_unit_type(form)->buffers[0]))[i];
        void* m;
        Mat44 local;
        if (jo::as_unit(b)->actor != nullptr) {
            m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_actor(jo::as_unit(b)->actor)->bone))->matrix());  // 0x0060ef80
        } else {
            joan::orig::matrix_rotation(&local, joan::orig::arg::f32(jo::view<u32>(jo::as_unit(b)->heading)));  // 0x0060bc50
            import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, &local, &jo::as_unit(b)->position_[0]);  // matrix_multiply_translation
            m = &local;
        }
        Vec3 pos;
        import_call<void*>(d::iat_v3d_87_point_transform_3, &pos, &slot, m);  // point_transform(out, in, m)
        joan::orig::as_ground_object(a)->set_position(pos);                     // 0x004fedf0
        jo::view<i32*>(jo::as_unit(b)->mask_slots)[i] = jo::as_unit(a)->id;
        jo::view<i32*>(jo::as_unit(b)->mask_slots_b)[i] = 0;
        jo::as_unit(a)->weapon = jo::as_unit(b)->id;
        jo::as_unit(a)->weapon_slot = i;
        const float heading = joan::orig::arg::as<float>(joan::orig::as_ground_object(b)->heading()) / global<float>(d::flt_0_017453292);  // 0x004ff850
        joan::orig::as_unit(a)->set_heading(joan::orig::arg::f32(heading + jo::view<float*>(jo::as_unit_type(form)->buffers[1])[i]));                  // 0x0050abd0
        u8* const form2 = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kFormations)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(b)->type_id))));
        const i32 type = jo::self<jo::UnitType>(form2)->kind;
        switch (static_cast<u32>(type - 4)) {
            case 0: jo::as_unit(a)->anim_special = 5; break;
            case 1: jo::as_unit(a)->anim_special = 6; break;
            case 2: jo::as_unit(a)->anim_special = 8; break;
            case 3: break;
            case 4: jo::as_unit(a)->anim_special = 9; break;
            case 5: jo::as_unit(a)->anim_special = 7; break;
            case 6:
                if (i == 0) {
                    jo::as_unit(a)->anim_special = 0xa;
                } else if (i == 1) {
                    jo::as_unit(a)->anim_special = 0xb;
                } else if (i == 2) {
                    jo::as_unit(a)->anim_special = 0xc;
                }
                break;
            case 7:
                if (mask == 1) {
                    jo::as_unit(a)->anim_special = 0xd;
                } else if (mask == 2) {
                    if (!(slot.y >= global<float>(d::k_9cf5c4))) {  // y < 0 or NaN
                        jo::as_unit(a)->anim_special = 0xf;
                    } else {
                        jo::as_unit(a)->anim_special = 0xe;
                    }
                }
                break;
            default: break;
        }
        joan::orig::as_unit(a)->set_action(static_cast<i32>(jo::as_unit(a)->anim_kind), 0, 1);  // 0x00501190
        return 1;
    }
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b104_12", name, addr, fn)
B("Script::get_unit_flag22", 0x00652aa0, &unit_flag22);
B("Script::get_a46059", 0x00653110, &get_a46059);
B("Script::get_word_ptr", 0x00653130, &get_word_ptr);
B("Script::get_a5b9e8", 0x00659060, &get_a5b9e8);
B("Script::get_a47bb0", 0x00659720, &get_a47bb0);
B("Script::wrap_659740", 0x00659740, &wrap_659740);
B("Script::get_a47bb8", 0x006597d0, &get_a47bb8);
B("Script::get_a47bbc", 0x006597f0, &get_a47bbc);
B("Script::get_a47bc0", 0x00659810, &get_a47bc0);
B("Script::get_a591cc", 0x006598f0, &get_a591cc);
B("Script::get_a59171", 0x00659940, &get_a59171);
B("Script::get_a59175", 0x00659990, &get_a59175);
B("Script::get_a59179", 0x006599e0, &get_a59179);
B("Script::get_a591d8", 0x00659ac0, &get_a591d8);
B("Script::get_a591a9", 0x00659b10, &get_a591a9);
B("Script::get_a591b6", 0x00659b60, &get_a591b6);
B("Script::get_a591be", 0x00659ca0, &get_a591be);
B("Script::get_a591c2", 0x00659cc0, &get_a591c2);
B("Script::get_a591ba", 0x00659d40, &get_a591ba);
B("Script::get_a591c8", 0x00659d90, &get_a591c8);
B("Script::get_a591ad", 0x00659e40, &get_a591ad);
B("Script::get_a591b1", 0x00659e60, &get_a591b1);
B("Script::get_a5918d", 0x00659e80, &get_a5918d);
B("Script::get_a59199", 0x00659ed0, &get_a59199);
B("Script::get_a591a5", 0x00659f20, &get_a591a5);
B("Script::get_a59294", 0x0065a280, &get_a59294);
B("Script::wrap_65a2d0", 0x0065a2d0, &wrap_65a2d0);
B("Script::wrap_65a350", 0x0065a350, &wrap_65a350);
B("Script::wrap_65a370", 0x0065a370, &wrap_65a370);
B("Script::wrap_65afd0", 0x0065afd0, &wrap_65afd0);
B("Script::get_a596c4", 0x0065c200, &get_a596c4);
B("Script::get_a5b9f8", 0x0065ed50, &get_a5b9f8);
B("Formation::assign_slot", 0x0065f160, &assign_slot);
B("Script::get_a591c7", 0x0065fbd0, &get_a591c7);
#undef B

}  // namespace game_b104_12
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
namespace {
namespace game_b92_05 {

void* const kUnits = obj(d::g_units);
void* const kObjects = obj(d::g_items);
void* const kTypes = obj(d::g_types);

std::int32_t unit_invalid(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t* type_of(const void* unit) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<std::int32_t>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
}

JOAN_COUNTER(c_unit_busy, "script_unit_busy");
JOAN_COUNTER(c_heading_delta, "script_heading_delta");
JOAN_COUNTER(c_in_zone, "script_unit_in_zone");
JOAN_COUNTER(c_hardpoint, "script_hardpoint_xyz");
JOAN_COUNTER(c_unit_light, "script_set_unit_light");
JOAN_COUNTER(c_hardpoint_dist, "script_target_hardpoint_dist");
JOAN_COUNTER(c_heading_to, "script_unit_heading_to");
JOAN_COUNTER(c_send_order, "script_send_order");

// 0x00652b00
std::int32_t JOAN_CDECL script_unit_busy(std::int32_t id_arg) {
    c_unit_busy.hit();
    const std::int16_t id = static_cast<std::int16_t>(id_arg);
    if (unit_invalid(id) != 0) return 0;
    std::uint8_t* const u = unit_get(id);
    if (unit_invalid(global<std::int32_t>(d::g_player_unit_recno)) == 0) {
        // `mov ax, id; push eax` with eax = 0 from the test above: zero-extended word.
        const std::uint32_t zid = static_cast<std::uint16_t>(id);
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit_get(global<std::int32_t>(d::g_player_unit_recno)))->hostile_to(joan::orig::arg::i16(zid))) != 0)  // 0x005094d0
            return 1;
    }
    if (jo::as_unit(u)->anim_set != nullptr) {
        const std::int32_t x = joan::orig::arg::as<std::int32_t>(joan::orig::as_list(jo::as_unit(u)->anim_set)->index_of_416dc0(0x53));  // 0x00416dc0
        if (x != 0) {
            void* const y = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::as_unit(u)->anim_set)->entry(joan::orig::arg::u32(x)));  // 0x004168d0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_set_entry(y)->is_yes()) != 0) return 1;           // 0x00416ed0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(u)->has_item(0xc)) != 0) return 1;      // 0x0050d320
        }
    }
    return 0;
}

// 0x00653000
std::int32_t JOAN_CDECL script_heading_delta(std::int32_t id_arg, std::int32_t target) {
    c_heading_delta.hit();
    const joan::hook::x87::Fpu fp{true};
    std::uint8_t* const u = unit_get(static_cast<std::int16_t>(id_arg));
    const float f = joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading());  // 0x004ff850
    const double t = fp.div(fp.mul(f, global<float>(d::flt_180)), global<double>(d::dbl_3_141592653589793));  // 180.0f / pi
    const float h = joan::orig::arg::as<float>(joan::orig::as_unit(u)->face_object(joan::orig::arg::i32(target)));                                     // 0x005059b0
    return joan::hook::x87::ftol(fp.sub(t, h));
}

// 0x00653ca0
std::int32_t JOAN_CDECL script_unit_in_zone(std::int32_t id_arg, std::int32_t zone) {
    c_in_zone.hit();
    std::uint8_t* const u = unit_get(static_cast<std::int16_t>(id_arg));
    void* const zones = obj(d::g_places);
    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(zones)->empty_at_e(joan::orig::arg::i32(zone))) != 0) return 0;  // 0x0044cf80
    std::uint8_t* const z = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(zones)->get_e(joan::orig::arg::i32(zone)));  // 0x0043c450
    if (static_cast<void*>(jo::as_facility(z)->owner->world) == jo::as_unit(u)->world &&
        joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(jo::as_facility(z)->bounds)->contains(static_cast<void*>(jo::as_unit(u)->position_))) != 0)  // 0x0060d5f0
        return 1;
    return 0;
}

// 0x00657110 / 0x00657240 / 0x00657370: column `c` (0 / 1 / 2) of a unit's hardpoint k in world space.
std::int32_t hardpoint_coord(std::int32_t id_arg, std::int32_t k, std::int32_t c) {
    c_hardpoint.hit();
    const std::int16_t id = static_cast<std::int16_t>(id_arg);
    if (unit_invalid(id) != 0) return -1;
    std::uint8_t* const u = unit_get(id);
    const float* const row = jo::view<float*>(jo::as_unit_type(type_of(u))->buffers[0]) + k * 3;  // float[3] rows
    const float v0 = row[0], v1 = row[1], v2 = row[2];
    float local[16];
    const float* m;
    if (jo::as_unit(u)->actor != nullptr) {
        m = joan::orig::arg::as<float*>(joan::orig::as_bone(jo::view<void*>(jo::as_actor(jo::as_unit(u)->actor)->bone))->matrix());  // 0x0060ef80
    } else {
        joan::orig::matrix_rotation(joan::orig::arg::mptr(static_cast<void*>(local)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::as_unit(u)->heading)));  // 0x0060bc50
        import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, static_cast<void*>(local), static_cast<void*>(jo::as_unit(u)->position_));
        m = local;
    }
    float s = v0 * m[c];
    s = s + v1 * m[c + 4];
    s = s + v2 * m[c + 8];
    s = s + m[c + 12];
    return joan::hook::x87::ftol(s);
}
std::int32_t JOAN_CDECL hardpoint_x(std::int32_t id, std::int32_t k) { return hardpoint_coord(id, k, 0); }
std::int32_t JOAN_CDECL hardpoint_y(std::int32_t id, std::int32_t k) { return hardpoint_coord(id, k, 1); }
std::int32_t JOAN_CDECL hardpoint_z(std::int32_t id, std::int32_t k) { return hardpoint_coord(id, k, 2); }

// 0x0065d0d0
std::int32_t JOAN_CDECL script_set_unit_light(std::int32_t id, std::int32_t on) {
    c_unit_light.hit();
    if (unit_invalid(id) != 0) return 0;
    std::uint8_t* const u = unit_get(id);
    if (on != 0) {
        std::uint8_t* const t = type_of(u);
        if (jo::as_unit(u)->actor != nullptr) {
            switch (jo::self<jo::UnitType>(t)->shape_kind) {
                case 0: {
                    std::uint8_t* const p = jo::view<std::uint8_t*>(jo::as_unit(u)->actor);
                    if (jo::as_actor(p)->def != nullptr && jo::as_actor(p)->def->model != nullptr) {
                        jo::view<std::int32_t>(jo::as_actor(jo::as_unit(u)->actor)->aim_mode) = 1;
                        const float* const src = jo::as_actor(jo::as_unit(u)->actor)->def->model->sphere;
                        float* const dst = jo::as_actor(jo::as_unit(u)->actor)->aim_sphere;
                        for (int i = 0; i < 4; ++i) jo::view<std::uint32_t>(dst[i]) = jo::view<std::uint32_t>(src[i]);
                    } else {
                        jo::view<std::int32_t>(jo::as_actor(jo::as_unit(u)->actor)->aim_mode) = 0;
                    }
                    break;
                }
                case 1: {
                    jo::as_actor(jo::as_unit(u)->actor)->aim_mode = jo::self<jo::UnitType>(t)->shape_kind;
                    float* const dst = jo::as_actor(jo::as_unit(u)->actor)->aim_sphere;
                    for (int i = 0; i < 4; ++i) jo::view<std::uint32_t>(dst[i]) = jo::view<std::uint32_t>(jo::self<jo::UnitType>(t)->shape_sphere[i]);
                    break;
                }
                case 2: {
                    jo::as_actor(jo::as_unit(u)->actor)->aim_mode = jo::self<jo::UnitType>(t)->shape_kind;
                    float* const dst = jo::as_actor(jo::as_unit(u)->actor)->aim_box;
                    for (int i = 0; i < 6; ++i) jo::view<std::uint32_t>(dst[i]) = jo::view<std::uint32_t>(jo::self<jo::UnitType>(t)->shape_box[i]);
                    break;
                }
                case 3: {
                    jo::as_actor(jo::as_unit(u)->actor)->aim_mode = jo::self<jo::UnitType>(t)->shape_kind;
                    // collide_kind, collide_offset[2], collide_radius, collide_height <- shape_cylinder[5]
                    std::memcpy(&jo::as_actor(jo::as_unit(u)->actor)->collide_kind, jo::self<jo::UnitType>(t)->shape_cylinder, 5 * sizeof(float));
                    break;
                }
                default: break;
            }
        }
    } else if (jo::as_unit(u)->actor != nullptr) {
        jo::view<std::int32_t>(jo::as_unit(u)->actor->aim_mode) = 0;
    }
    return 1;
}

// 0x0065ddb0
double JOAN_CDECL script_target_hardpoint_dist(std::int32_t id) {
    c_hardpoint_dist.hit();
    if (unit_invalid(id) != 0) return global<float>(d::k_9cf5c4);  // 0.0f
    std::uint8_t* const u = unit_get(id);
    if (unit_invalid(jo::as_unit(u)->weapon) != 0) return global<float>(d::flt_m1);  // -1.0f
    std::uint8_t* const t = unit_get(jo::as_unit(u)->weapon);
    std::uint8_t* const type = type_of(t);
    if (jo::as_unit(u)->weapon_slot < 0 || !(jo::as_unit(u)->weapon_slot < jo::self<jo::UnitType>(type)->slot_count))
        return global<float>(d::flt_m1);
    float out[3];
    if (jo::as_unit(t)->actor != nullptr) {
        void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_actor(jo::as_unit(t)->actor)->bone))->matrix());  // 0x0060ef80
        void* const row = jo::view<float*>(jo::self<jo::UnitType>(type)->buffers[0]) + jo::as_unit(u)->weapon_slot * 3;  // float[3] rows
        import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(out), row, m);
    } else {
        float local[16];
        joan::orig::matrix_rotation(joan::orig::arg::mptr(static_cast<void*>(local)), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::as_unit(t)->heading)));  // 0x0060bc50
        import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, static_cast<void*>(local), static_cast<void*>(jo::as_unit(t)->position_));
        void* const row = jo::view<float*>(jo::self<jo::UnitType>(type)->buffers[0]) + jo::as_unit(u)->weapon_slot * 3;  // float[3] rows
        import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(out), row, static_cast<void*>(local));
    }
    float pos[3];
    joan::orig::as_unit(u)->position(joan::orig::arg::mptr(static_cast<void*>(pos)));  // 0x004fedb0
    const float dx = out[0] - pos[0];
    const float dy = out[1] - pos[1];
    const float s = dx * dx + dy * dy;
    return joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(s)));  // 0x004256d0 sqrt wrapper, st0 returned as is
}

// 0x0065f0f0
std::int32_t JOAN_CDECL script_unit_heading_to(std::int32_t id, std::int32_t target) {
    c_heading_to.hit();
    if (unit_invalid(id) != 0 || joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(target)) != 0) return 0;  // 0x00425d50
    std::uint8_t* const u = unit_get(id);
    return joan::hook::x87::ftol(joan::orig::arg::as<float>(joan::orig::as_unit(u)->face_object(joan::orig::arg::i32(target))));  // 0x005059b0
}

// 0x006619d0
std::int32_t JOAN_CDECL script_send_order(std::int32_t id, std::int32_t kind, std::int32_t b, std::int32_t c,
                                       std::int32_t d, std::int32_t e, std::int32_t delay) {
    c_send_order.hit();
    if (unit_invalid(id) != 0) return 0;
    std::uint8_t* const u = unit_get(id);
    alignas(4) std::uint8_t order[0x1c] = {};  // 0x19-byte order record at ebp-0x20
    const std::int32_t when = delay != 0 ? global<std::int32_t>(d::g_now) + delay : 0;
    joan::orig::as_unit_task(static_cast<void*>(order))->set(joan::orig::arg::u32(kind), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<std::int32_t>(static_cast<std::int8_t>(b)))), joan::orig::arg::u32(c), joan::orig::arg::u32(d), joan::orig::arg::u32(e), joan::orig::arg::u32(when));  // 0x004e4520
    joan::orig::as_unit(u)->set_task(joan::orig::arg::mptr(static_cast<void*>(order)));  // 0x00527fb0
    if (kind == 0xb && d == 1) {
        std::uint8_t* const grp = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_unit(u)->leader());  // 0x004216d0
        if (grp != nullptr) {
            for (std::int32_t i = static_cast<std::int32_t>(jo::as_group(jo::as_unit(grp)->group)->count) - 1; i >= 0; --i) {
                const std::int32_t m = jo::as_group(jo::as_unit(grp)->group)->members[i];
                if (unit_invalid(m) != 0) continue;
                std::uint8_t* const mu = unit_get(jo::as_group(jo::as_unit(grp)->group)->members[i]);
                joan::orig::as_unit(mu)->set_task(joan::orig::arg::mptr(static_cast<void*>(order)));
            }
        }
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("script_unit_busy", 0x00652b00, &script_unit_busy);
B("script_heading_delta", 0x00653000, &script_heading_delta);
B("script_unit_in_zone", 0x00653ca0, &script_unit_in_zone);
B("script_hardpoint_x", 0x00657110, &hardpoint_x);
B("script_hardpoint_y", 0x00657240, &hardpoint_y);
B("script_hardpoint_z", 0x00657370, &hardpoint_z);
B("script_set_unit_light", 0x0065d0d0, &script_set_unit_light);
B("script_target_hardpoint_dist", 0x0065ddb0, &script_target_hardpoint_dist);
B("script_unit_heading_to", 0x0065f0f0, &script_unit_heading_to);
B("script_send_order", 0x006619d0, &script_send_order);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b98_07.cpp (game/b98_07)
namespace {
namespace game_b98_07 {

using joan::orig::helpers::at;

void* const kUnitTable = obj(d::g_units);
void* const kTypes = obj(d::g_types);
void* const kPlaces = obj(d::g_places);
void* const kGroups = obj(d::g_nations);

JOAN_COUNTER(c_set_1c5, "script_unit_set_1c5");
JOAN_COUNTER(c_spawn_unit, "script_spawn_unit");
JOAN_COUNTER(c_spawn_members, "script_unit_spawn_members");
JOAN_COUNTER(c_set_outcome, "script_set_outcome");
JOAN_COUNTER(c_set_type, "script_unit_set_type");
JOAN_COUNTER(c_count_allies, "script_count_allies");
JOAN_COUNTER(c_spawn_effect, "script_spawn_effect");
JOAN_COUNTER(c_list_remove, "script_unit_list_remove");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* type_rec(u32 word_arg) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(word_arg))); }  // 0x0042d0a0
u32 sx(i16 v) { return static_cast<u32>(static_cast<i32>(v)); }
u32 low8(u32 eax, u8 al) { return (eax & 0xffffff00u) | al; }

struct H {
    // ------------------------------------------------------------------ 0x00655b70
    void* set_12(i32 v) {
        jo::self<jo::BuyScreen>(this)->place = v;
        return this;
    }
};

// ---------------------------------------------------------------------------- 0x00653210
static i32 JOAN_CDECL script_unit_set_1c5(i32 id_arg, i32 v_arg) {
    c_set_1c5.hit();
    const i16 id = static_cast<i16>(id_arg);
    const i16 v = static_cast<i16>(v_arg);
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    u8* const unit = unit_get(static_cast<i32>(id));
    jo::as_unit(unit)->drop_item = v;
    if (static_cast<i32>(v) != 0) {
        // `mov dx, word [ebp+0xc]; push edx` with edx = unit pointer.
        const u32 arg = (joan::call::register_low_bits(unit) & 0xffff0000u) | static_cast<u16>(v);
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(arg)));  // 0x00425ce0
        jo::as_unit(unit)->treasure_level = jo::as_item_def(rec)->min_level;
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x00653930
static i32 JOAN_CDECL script_toggle_a56398() {
    if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_inventory(obj(d::g_a56398))->is_open())) != 0) {  // 0x00430340
        joan::orig::as_panel(obj(d::g_a56398))->close();                           // 0x006f7e90
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x00653eb0
static u32 JOAN_CDECL script_a484d0_4c54a0(const char* a) {
    if (a == nullptr) return 0;
    return joan::orig::arg::as<u32>(joan::orig::as_place_list(obj(d::g_place_array))->find(a));  // 0x004c54a0
}

// ---------------------------------------------------------------------------- 0x00654280
static u32 JOAN_CDECL script_a484d0_654530(i32 a) {
    void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_place_array))->get_c70(joan::orig::arg::i32(a)));  // 0x00456c70
    return joan::orig::arg::as<u32>(joan::orig::as_unit(o)->field_1ce());                               // 0x00654530
}

// ---------------------------------------------------------------------------- 0x00654ae0
static i32 JOAN_CDECL script_place_set_13c(i32 a, i32 v) {
    u8* const place = joan::orig::arg::as<u8*>(joan::orig::as_list(kPlaces)->get_e(joan::orig::arg::i32(a)));  // 0x0043c450
    using Virtual = u8* (Obj::*)();
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(place)[0x18 / 4], sizeof(void*));
    u8* const target = (reinterpret_cast<Obj*>(place)->*method)();
    jo::as_building(target)->target = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00655150
static i32 JOAN_CDECL script_spawn_unit(const char* type, const char* group) {
    c_spawn_unit.hit();
    const u32 t = joan::orig::arg::as<u32>(joan::orig::as_unit_types(kTypes)->find(type));          // 0x005270b0
    const i32 id = joan::orig::arg::as<i32>(joan::orig::as_units(kUnitTable)->create_51dfc0(joan::orig::arg::i32(t)));         // 0x0051dfc0
    const i32 g = joan::orig::arg::as<i32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(group));  // 0x004b91a0
    if (g == 0) return id;
    const u32 v = joan::orig::arg::as<u32>(joan::orig::as_list(kGroups)->find_by_word2(joan::orig::arg::i32(g)));  // 0x004b67e0 (pushed as full eax)
    joan::orig::as_unit(unit_get(id))->set_side(joan::orig::arg::i32(v));           // 0x00509250
    jo::as_unit(unit_get(id))->layer = 1;
    return id;
}

// ---------------------------------------------------------------------------- 0x00655810
static i32 JOAN_CDECL script_unit_41fcc0(i32 id_arg) {
    const i16 id = static_cast<i16>(id_arg);
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    joan::orig::as_unit(unit_get(static_cast<i32>(id)))->detach_scene();  // 0x0041fcc0
    return static_cast<i32>(id);
}

// ---------------------------------------------------------------------------- 0x00655d80
static i32 JOAN_CDECL script_unit_set_2e7(i32 id_arg, i32 v_arg) {
    const i16 id = static_cast<i16>(id_arg);
    if (unit_missing(static_cast<i32>(id)) == 0) {
        jo::as_unit(unit_get(static_cast<i32>(id)))->interact_stage = static_cast<i16>(v_arg);
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x00656260
static i32 JOAN_CDECL script_unit_114(i32 id) {
    if (unit_missing(id) != 0) return 0;
    return jo::as_unit(unit_get(id))->markers;
}

// ---------------------------------------------------------------------------- 0x00656940
static i32 JOAN_CDECL script_unit_135(i32 id) {
    if (unit_missing(id) != 0) return 0;
    return jo::as_unit(unit_get(id))->hit_points;
}

// ---------------------------------------------------------------------------- 0x006584e0
static i32 JOAN_CDECL script_unit_spawn_members(i32 id_arg, const char* type, i32 n) {
    c_spawn_members.hit();
    const i16 id = static_cast<i16>(id_arg);
    const u32 t = joan::orig::arg::as<u32>(joan::orig::as_unit_types(kTypes)->find(type));  // 0x005270b0
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    u8* const unit = unit_get(static_cast<i32>(id));
    u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_unit(unit)->leader());  // 0x004216d0
    if (g == nullptr) return 0;
    for (i32 i = 0; i < n; ++i) {
        if (static_cast<i32>(jo::as_group(jo::as_unit(g)->group)->count) == 0x14) break;
        const i32 nid = joan::orig::arg::as<i32>(joan::orig::as_units(kUnitTable)->create_51dfc0(joan::orig::arg::i32(t)));  // 0x0051dfc0
        u8* const m = unit_get(nid);
        u8* const rec = type_rec(sx(static_cast<i16>(t)));  // word arg (garbage upper bits)
        joan::orig::as_unit(m)->set_level_state(static_cast<i32>(jo::self<jo::UnitType>(rec)->rank));  // 0x005089e0
        joan::orig::as_unit(m)->set_side(joan::orig::arg::i32(sx(jo::as_unit(unit)->side)));  // 0x00509250 (word arg)
        joan::orig::as_unit(m)->set_group(jo::as_unit(unit)->world);  // 0x004fed00
        joan::orig::as_unit(m)->link_to(static_cast<i32>(jo::as_unit(g)->id));  // 0x00438f80
        jo::as_unit(m)->layer = 1;
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x006594e0
static i32 JOAN_CDECL script_set_outcome(const char* s) {
    c_set_outcome.hit();
    if (stricmp_(s, str(d::s_WIN)) == 0) {  // "WIN"
        global<u8>(d::g_flag_a59298) = 1;
    } else if (stricmp_(s, str(d::s_LOSE)) == 0) {  // "LOSE"
        global<u8>(d::g_flag_a59298) = 2;
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x006596f0
static u32 JOAN_CDECL script_a52928_57f990(i32 a, i32 b, i32 c) {
    return joan::orig::arg::as<u32>(joan::orig::as_date(obj(d::g_date))->julian(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c)));  // 0x0057f990
}

// ---------------------------------------------------------------------------- 0x00659a00
static i32 JOAN_CDECL script_set_a591d0(i32 v) {
    global<i32>(d::g_scenario_rescue_count) = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659b30
static i32 JOAN_CDECL script_clear_a591a9() {
    global<i32>(d::g_get_a591a9) = 0;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659ea0
static i32 JOAN_CDECL script_clear_a59199() {
    global<i32>(d::g_get_a59199) = 0;
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065a390
static i32 JOAN_CDECL script_table_set_byte(i32 n, i32 c) {
    const u32 off = (static_cast<u32>(n) - 1u) * 0x1au;
    global<u8>(d::g_a5929b + off) = static_cast<u8>(c);
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065a7e0
static i32 JOAN_CDECL script_table_word_aa(i32 n) {
    const u32 off = (static_cast<u32>(n) - 1u) * 0x1au;
    return static_cast<i32>(global<i16>(d::g_a592aa + off));
}

// ---------------------------------------------------------------------------- 0x0065a9c0
static i32 JOAN_CDECL script_a58f90_set_bit(i32 n, i32 c) {
    // `mov al, byte [ebp+0xc]; push eax`: 0x004723f0 masks the arg with 0xff.
    joan::orig::as_game(obj(d::g_game_state))->set_flag_1c1(joan::orig::arg::i32(n), joan::orig::arg::u32(static_cast<u32>(static_cast<u8>(c))));  // 0x004723f0
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065b200
static u32 JOAN_CDECL script_type_51fd50(const char* a) {
    if (a == nullptr) return 0;
    const u32 t = joan::orig::arg::as<u32>(joan::orig::as_unit_types(kTypes)->find(a));  // 0x005270b0
    return joan::orig::arg::as<u32>(joan::orig::as_units(kUnitTable)->find_by_3b(joan::orig::arg::i32(t)));     // 0x0051fd50
}

// ---------------------------------------------------------------------------- 0x0065b5f0
static u32 JOAN_CDECL script_a501e0_4ce900(const char* a) {
    if (a == nullptr) return 0;
    return joan::orig::arg::as<u32>(joan::orig::as_table(obj(d::g_sell_item_res))->find_name_4ce900(a));  // 0x004ce900
}

// ---------------------------------------------------------------------------- 0x0065c2e0
static i32 JOAN_CDECL script_unit_set_type(i32 id_arg, i32 t_arg) {
    c_set_type.hit();
    const i16 id = static_cast<i16>(id_arg);
    const i16 t = static_cast<i16>(t_arg);
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    u8* const unit = unit_get(static_cast<i32>(id));
    const i32 flag = static_cast<i32>(jo::view<i8>(jo::as_unit(unit)->in_scene));
    if (flag != 0) {
        const i32 v1 = jo::as_unit(unit)->collision_id;
        // eax = movsx byte (upper half 0 / 0xffff) with ax = t.
        u8* const r1 = type_rec((static_cast<u32>(flag) & 0xffff0000u) | static_cast<u16>(t));  // 0x0042d0a0
        joan::orig::as_panel(r1)->open(joan::orig::arg::i32(v1));                                                     // 0x00527520
        const i32 v2 = jo::as_unit(unit)->collision_id;
        u8* const r2 = type_rec(sx(jo::as_unit(unit)->type_id));  // 0x0042d0a0 (word arg)
        joan::orig::as_unit_type(r2)->release_model_slot(joan::orig::arg::i32(v2));                 // 0x00527690
    }
    jo::as_unit(unit)->type_id = t;
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065c470
static u32 JOAN_CDECL script_a48a38_byte() {
    return joan::orig::arg::as<u32>(joan::orig::as_fade_title(obj(d::g_fade_title))->is_idle()) & 0xffu;  // 0x007616f0
}

// ---------------------------------------------------------------------------- 0x0065cd00
static i32 JOAN_CDECL script_count_allies(i32 id) {
    c_count_allies.hit();
    if (unit_missing(id) != 0) return 0;
    u8* const unit = unit_get(id);
    if (jo::as_unit(unit)->world == nullptr) return 0;
    i32 count = 0;
    for (i32 i = joan::orig::as_dynarray(kUnitTable)->size(); i > 0; --i) {  // 0x00417ad0
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
        if (e == nullptr) continue;
        if (static_cast<i32>(jo::as_unit(e)->speech_off) != 0) continue;
        if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(e)->dead())) != 0) continue;  // 0x0042d0e0
        if (jo::as_unit(e)->world != jo::as_unit(unit)->world) continue;
        // eax = unit pointer with ax = e+0x3b.
        u8* const rec = type_rec((joan::call::register_low_bits(unit) & 0xffff0000u) | jo::view<u16>(jo::as_unit(e)->type_id));  // 0x0042d0a0
        const i32 k = static_cast<i32>(jo::self<jo::UnitType>(rec)->kind);
        if (k != 0 && k != 1) continue;
        const i32 w = static_cast<i32>(jo::self<jo::UnitType>(rec)->unit_class);
        if (w == 0 || w == 8 || w == 9) continue;
        if (static_cast<i32>(jo::as_unit(unit)->side) == static_cast<i32>(jo::as_unit(e)->side)) ++count;
    }
    return count;
}

// ---------------------------------------------------------------------------- 0x0065da50
static i32 JOAN_CDECL script_spawn_effect(i32 a, i32 b, i32 c) {
    c_spawn_effect.hit();
    u32 colour_obj = 0;  // [ebp-8], 4 bytes written by 0x0042a260
    const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_colour(static_cast<void*>(&colour_obj))->construct(0xff, 0xff, 0xff, 0xff));  // 0x0042a260
    const char* const res = joan::orig::as_misc(obj(d::g_misc))->format_int_58fca0(joan::orig::arg::i32(a), 4);  // 0x0058fca0 (ret 8)
    joan::orig::as_text(obj(d::g_text_a5d140))->draw_line(joan::orig::arg::i32(sx(static_cast<i16>(b))), joan::orig::arg::i32(sx(static_cast<i16>(c))), 1u, res, joan::orig::arg::u32(colour), 0xffffffff, 0, 1, 2, 0, joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0x3f800000u));  // 0x006255d0 (ret 0x30)
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065e900
static i32 JOAN_CDECL script_a50518_6e01d0() {
    joan::orig::as_goal_panel(obj(d::g_goal_panel))->close_second();  // 0x006e01d0
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065ed70
static i32 JOAN_CDECL script_state_is_4() { return global<i32>(d::g_control_mode) == 4 ? 1 : 0; }

// ---------------------------------------------------------------------------- 0x0065eea0
static i32 JOAN_CDECL script_not_a53cb0() {
    return static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_obj_menu(obj(d::g_objective_menu))->is_open())) != 0 ? 0 : 1;  // 0x004303d0
}

// ---------------------------------------------------------------------------- 0x0065f950
static i32 JOAN_CDECL script_not_a46040() {
    return static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_letter(obj(d::g_letter))->get_8())) == 0 ? 1 : 0;  // 0x006e0a90
}

// ---------------------------------------------------------------------------- 0x00661ca0
// Returns al; the upper eax bits are those of the last call / register, as in the original.
static u32 JOAN_CDECL script_unit_list_remove(i32 id, i32 key) {
    c_list_remove.hit();
    const u32 missing = static_cast<u32>(unit_missing(id));
    if (missing != 0) return low8(missing, 1);
    u8* const unit = unit_get(id);
    if (jo::as_unit(unit)->orders == nullptr) return low8(joan::call::register_low_bits(unit), 1);
    const i32 k = joan::orig::arg::as<i32>(joan::orig::as_shop(obj(d::g_game_a5d380))->state());  // 0x0065afb0
    bool scan = false;
    if (k == 0) {
        scan = true;
    } else if (k == 6 || k == 7 || k == 8) {
        scan = static_cast<i32>(global<i8>(d::g_a5d3bf)) == 0;
    }
    if (scan) {
        if (jo::as_order(jo::as_unit(unit)->orders)->current.kind == key) {
            const u32 r = joan::orig::arg::as<u32>(joan::orig::as_action(jo::as_unit(unit)->orders)->dispatch(joan::orig::arg::mptr(static_cast<void*>(unit))));  // 0x004bb660
            return low8(r, 1);
        }
        for (i32 j = joan::orig::as_dynarray(jo::as_unit(unit)->orders)->size(); j > 0; --j) {  // 0x00417ad0
            i32* const e = joan::orig::arg::as<i32*>(joan::orig::as_list(jo::as_unit(unit)->orders)->at_484570(joan::orig::arg::i32(j)));     // 0x00484570
            if (*e == key) {
                const u32 r = joan::orig::arg::as<u32>(joan::orig::as_list(jo::as_unit(unit)->orders)->forward_584a10(joan::orig::arg::i32(j)));  // 0x004baee0
                return low8(r, 1);
            }
        }
    }
    const u32 r = joan::orig::arg::as<u32>(joan::orig::as_list(jo::as_unit(unit)->orders)->contains(joan::orig::arg::i32(key)));  // 0x004bb8d0
    if (r != 0) return low8(r, 0);
    return low8(r, 1);
}

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_07", name, addr, fn)
B98("script_unit_set_1c5", 0x00653210, &script_unit_set_1c5);
B98("script_toggle_a56398", 0x00653930, &script_toggle_a56398);
B98("script_a484d0_4c54a0", 0x00653eb0, &script_a484d0_4c54a0);
B98("script_a484d0_654530", 0x00654280, &script_a484d0_654530);
B98("script_place_set_13c", 0x00654ae0, &script_place_set_13c);
B98("script_spawn_unit", 0x00655150, &script_spawn_unit);
B98("script_unit_41fcc0", 0x00655810, &script_unit_41fcc0);
B98("Obj::set_12", 0x00655b70, &H::set_12);
B98("script_unit_set_2e7", 0x00655d80, &script_unit_set_2e7);
B98("script_unit_114", 0x00656260, &script_unit_114);
B98("script_unit_135", 0x00656940, &script_unit_135);
B98("script_unit_spawn_members", 0x006584e0, &script_unit_spawn_members);
B98("script_set_outcome", 0x006594e0, &script_set_outcome);
B98("script_a52928_57f990", 0x006596f0, &script_a52928_57f990);
B98("script_set_a591d0", 0x00659a00, &script_set_a591d0);
B98("script_clear_a591a9", 0x00659b30, &script_clear_a591a9);
B98("script_clear_a59199", 0x00659ea0, &script_clear_a59199);
B98("script_table_set_byte", 0x0065a390, &script_table_set_byte);
B98("script_table_word_aa", 0x0065a7e0, &script_table_word_aa);
B98("script_a58f90_set_bit", 0x0065a9c0, &script_a58f90_set_bit);
B98("script_type_51fd50", 0x0065b200, &script_type_51fd50);
B98("script_a501e0_4ce900", 0x0065b5f0, &script_a501e0_4ce900);
B98("script_unit_set_type", 0x0065c2e0, &script_unit_set_type);
B98("script_a48a38_byte", 0x0065c470, &script_a48a38_byte);
B98("script_count_allies", 0x0065cd00, &script_count_allies);
B98("script_spawn_effect", 0x0065da50, &script_spawn_effect);
B98("script_a50518_6e01d0", 0x0065e900, &script_a50518_6e01d0);
B98("script_state_is_4", 0x0065ed70, &script_state_is_4);
B98("script_not_a53cb0", 0x0065eea0, &script_not_a53cb0);
B98("script_not_a46040", 0x0065f950, &script_not_a46040);
B98("script_unit_list_remove", 0x00661ca0, &script_unit_list_remove);
#undef B98

}  // namespace game_b98_07
}  // namespace

// ---------------------------------------- from modules/game_b95_07.cpp (game/b95_07)
namespace {
namespace game_b95_07 {

using joan::orig::helpers::at;
template <class R = void, class... A>
R vcall(void* o, std::int32_t off, A... args) {
    return thiscall<R>(reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(o)[joan::call::x86_vslot(off)]), o, args...);  // slot off / 4 (x86 byte offset); o: object of varying class
}

void* const kUnitTable = obj(d::g_units);
void* const kObjects = obj(d::g_items);
void* const kPlaces = obj(d::g_places);

JOAN_COUNTER(c_find_unit, "script_find_unit_for");
JOAN_COUNTER(c_group_flag, "script_unit_group_flag_2f");
JOAN_COUNTER(c_group_call, "script_unit_group_call_41fcc0");
JOAN_COUNTER(c_unit_dist, "script_unit_unit_dist");
JOAN_COUNTER(c_goldbags, "script_goldbags_to_barrels");
JOAN_COUNTER(c_set_stat, "script_set_stat");
JOAN_COUNTER(c_store_rect, "script_store_rect");
JOAN_COUNTER(c_unit_order, "script_unit_order");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 object_missing(i32 id) { return joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(id)); }  // 0x00425d50
u8* object_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x00425d20

struct H {
    // ------------------------------------------------------------------ 0x00654530
    i32 field_1ce() { return jo::view<i32>(jo::self<jo::Place>(this)->captured); }

    // ------------------------------------------------------------------ 0x00659760
    float seconds() { return jo::self<jo::Calendar>(this)->time_prev / global<float>(d::flt_60); }  // 60.0f
};

// ---------------------------------------------------------------------------- 0x00653360
static u32 JOAN_CDECL script_unit_group_entry(i16 id, const char* k) {
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    u8* const unit = unit_get(static_cast<i32>(id));
    if (k == nullptr) return 0;
    if (jo::as_unit(unit)->cell == nullptr || jo::view<void*>(jo::as_cell(jo::as_unit(unit)->cell)->tracks) == nullptr) return 0;
    void* const o = joan::orig::arg::as<void*>(joan::orig::as_name_record(jo::view<void*>(jo::as_cell(jo::as_unit(unit)->cell)->tracks))->get_c());  // 0x00453cc0
    return joan::orig::arg::as<u32>(joan::orig::as_name_list(o)->find_4ec320(k));                                               // 0x004ec320
}

// ---------------------------------------------------------------------------- 0x00653d60
static i32 JOAN_CDECL script_find_unit_for(i32 a) {
    c_find_unit.hit();
    for (i32 i = joan::orig::as_dynarray(kUnitTable)->size(); i > 0; --i) {  // 0x00417ad0
        if (joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(i)) == nullptr) continue;  // 0x0041b3f0
        const i32 r = joan::orig::arg::as<i32>(joan::orig::script_unit_in_zone(static_cast<i32>(static_cast<i16>(i)), joan::orig::arg::i32(a)));  // 0x00653ca0
        if (static_cast<i16>(r) != 0) return i;
    }
    return 0;
}

// ---------------------------------------------------------------------------- 0x00654bc0
static i32 JOAN_CDECL script_place_set_97(i16 id, i8 v) {
    void* const place = joan::orig::arg::as<void*>(joan::orig::as_list(kPlaces)->get_e(static_cast<i32>(id)));  // 0x0043c450
    jo::view<i8>(jo::self<jo::FlagFacility>(vcall<void*>(place, 0x24))->flag_97) = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00655620
static i32 JOAN_CDECL script_unit_set_50abd0(i16 id, i32 v) {
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    const float fv = static_cast<float>(v);
    joan::orig::as_unit(unit_get(static_cast<i32>(id)))->set_heading(joan::orig::arg::f32(fv));  // 0x0050abd0
    return 1;
}

// ---------------------------------------------------------------------------- 0x00655990
static i32 JOAN_CDECL script_unit_1e2(i16 id) {
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    return jo::as_unit(unit_get(static_cast<i32>(id)))->mount_id;
}

// ---------------------------------------------------------------------------- 0x00655c20
static i32 JOAN_CDECL script_unit_set_2e3(i16 id, i16 v) {
    if (unit_missing(static_cast<i32>(id)) == 0) jo::as_unit(unit_get(static_cast<i32>(id)))->hit_by = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00655f10
static i32 JOAN_CDECL script_unit_2eb(i16 id) {
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    if (static_cast<i32>(jo::as_unit(unit_get(static_cast<i32>(id)))->bribe_state) == 2) return 0;
    return static_cast<i32>(jo::as_unit(unit_get(static_cast<i32>(id)))->bribe_state);
}

// ---------------------------------------------------------------------------- 0x006562b0
static i32 JOAN_CDECL script_unit_set_15f(i32 id, i16 v) {
    if (unit_missing(id) == 0) jo::as_unit(unit_get(id))->stats[5] = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x006574a0
static i32 JOAN_CDECL script_unit_group_flag_2f(i16 id, i32 v) {
    c_group_flag.hit();
    if (unit_missing(static_cast<i32>(id)) != 0) return -1;
    u8* const unit = unit_get(static_cast<i32>(id));
    jo::view<u8>(jo::as_unit(unit)->speech_off) = v == 0 ? 1 : 0;
    if (jo::as_unit(unit)->group == nullptr) return 0;
    if (static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->leader) != static_cast<i32>(id)) return 1;
    if (static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->count) <= 0) return 1;
    for (i32 i = 0; i < static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->count); ++i) {
        if (unit_missing(static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->members[i])) != 0) continue;
        const u8 flag = v == 0 ? 1 : 0;
        jo::view<u8>(jo::as_unit(unit_get(static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->members[i])))->speech_off) = flag;
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x00658650
static i32 JOAN_CDECL script_unit_group_call_41fcc0(i16 id) {
    c_group_call.hit();
    u8* const unit = unit_get(static_cast<i32>(id));
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->is_busy()) == 0) return 0;  // 0x004b62b0
    u8* const grp = joan::orig::arg::as<u8*>(joan::orig::as_unit(unit)->leader());    // 0x004216d0
    if (grp == nullptr || jo::as_unit(grp)->group == nullptr) return 0;
    for (i32 k = static_cast<i32>(jo::as_group(jo::as_unit(grp)->group)->count); k > 0; --k) {
        if (unit_missing(static_cast<i32>(jo::as_group(jo::as_unit(grp)->group)->members[k - 1])) != 0) continue;
        u8* const m = unit_get(static_cast<i32>(jo::as_group(jo::as_unit(grp)->group)->members[k - 1]));
        joan::orig::as_unit(m)->detach_scene();  // 0x0041fcc0
    }
    return 0;
}

// ---------------------------------------------------------------------------- 0x00659400
static i32 JOAN_CDECL script_unit_unit_dist(i32 a, i32 b) {
    c_unit_dist.hit();
    if (unit_missing(a) != 0) return -1;
    if (unit_missing(b) != 0) return -1;
    u8* const ua = unit_get(a);
    u8* const ub = unit_get(b);
    float pa[3];  // [ebp-0x14]
    float pb[3];  // [ebp-0x20]
    joan::orig::as_unit(ua)->position(joan::orig::arg::mptr(static_cast<void*>(pa)));  // 0x004fedb0
    joan::orig::as_unit(ub)->position(joan::orig::arg::mptr(static_cast<void*>(pb)));  // 0x004fedb0
    float tmp[3];
    const float* const d = joan::orig::arg::as<const float*>(joan::orig::vec_sub(static_cast<void*>(tmp), static_cast<void*>(pb), static_cast<void*>(pa)));  // 0x0060cc90
    float v[3];
    std::memcpy(v, d, sizeof v);
    return joan::hook::x87::ftol(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(v)));  // length, _ftol
}

// ---------------------------------------------------------------------------- 0x006599b0
static i32 JOAN_CDECL script_clear_a59179() {
    global<i32>(d::g_get_a59179) = 0;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659b80
static i32 JOAN_CDECL script_goldbags_to_barrels() {
    c_goldbags.hit();
    const i32 bag = joan::orig::arg::as<i32>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(str(d::s_S7_LGOLDBAG)));  // 0x004a36c0 'S7_LGOLDBAG'
    for (i32 i = 1; i <= joan::orig::as_dynarray(obj(d::g_unitList))->size(); ++i) {  // 0x00417ad0
        if (object_missing(i) != 0) continue;
        if (static_cast<i32>(jo::as_map_object(object_get(i))->class_id) != 0xa) continue;
        if (stricmp_(jo::as_map_object(object_get(i))->name, str(d::s_s7_goldbag)) != 0) continue;  // 's7_goldbag'
        void* const o = object_get(i);
        jo::as_order0b(vcall<void*>(o, 0x50))->drop_item = static_cast<std::int16_t>(bag);  // slot 0x50: the Order0b
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_map_object(object_get(i))->name)), str(d::s_S7_Barrels));  // 'S7_Barrels'
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659f40
static i32 JOAN_CDECL script_set_stat(const char* name, i32 i, i32 v) {
    c_set_stat.hit();
    i32 k = 0;
    if (stricmp_(name, str(d::s_STRENGTH)) == 0) {         // 'STRENGTH'
        k = 0;
    } else if (stricmp_(name, str(d::s_DEXTERITY)) == 0) {  // 'DEXTERITY'
        k = 1;
    } else if (stricmp_(name, str(d::s_LEADERSHIP)) == 0) {  // 'LEADERSHIP'
        k = 2;
    } else if (stricmp_(name, str(d::s_DEFENSE)) == 0) {  // 'DEFENSE'
        k = 3;
    }
    global<i32>(d::g_a59240 + (static_cast<u32>(k) * 0x14u + static_cast<u32>(i) * 4u)) = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065a320
static i32 JOAN_CDECL script_42e040() {
    joan::orig::as_unit(obj(d::g_player_unit_recno))->set_state(joan::orig::arg::i32(global<i32>(d::g_control_base_mode)), static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x0042e040
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065a810
static i32 JOAN_CDECL script_table_set(i32 n, i32 v) {
    global<i32>(d::g_a592ac + (static_cast<u32>(n) - 1u) * 0x1au) = v;
    return v;
}

// ---------------------------------------------------------------------------- 0x0065aca0
static i32 JOAN_CDECL script_store_rect() {
    c_store_rect.hit();
    if (global<i32>(d::g_mission) != 0) {
        global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].u32_18a = global<u32>(d::g_a58f97);
        global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].u32_18e = global<u32>(d::g_a58f9b);
        global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].u32_192 = global<u32>(d::g_a58f9f);
        global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].u32_196 = global<u32>(d::g_a58fa3);
    } else {
        global<u32>(d::g_a59145) = global<u32>(d::g_a58f97);
        global<u32>(d::g_a59149) = global<u32>(d::g_a58f9b);
        global<u32>(d::g_a5914d) = global<u32>(d::g_a58f9f);
        global<u32>(d::g_a59151) = global<u32>(d::g_a58fa3);
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065b630
static i32 JOAN_CDECL script_object_name_differs(i32 id, const char* name) {
    if (object_missing(id) != 0) return 1;
    u8* const o = object_get(id);
    return stricmp_(jo::as_map_object(o)->name, name) != 0 ? 1 : 0;
}

// ---------------------------------------------------------------------------- 0x0065c250
static i32 JOAN_CDECL script_unit_523190(i32 id, i32 v) {
    if (unit_missing(id) != 0) return 0;
    joan::orig::as_unit(unit_get(id))->set_level_523190(joan::orig::arg::i32(v));                        // 0x00523190
    const i32 r = joan::orig::arg::as<i32>(joan::orig::as_unit(unit_get(id))->hit_points_max());        // 0x00523320
    joan::orig::as_unit(unit_get(id))->set_value_135(joan::orig::arg::i32(r));                        // 0x004fe510
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065c9b0
static i32 JOAN_CDECL script_time_since() {
    return static_cast<i32>(global<u32>(d::g_now) - global<u32>(d::g_a5916d));
}

// ---------------------------------------------------------------------------- 0x0065e850
static i32 JOAN_CDECL script_6dff10() {
    joan::orig::as_goal_panel(obj(d::g_goal_panel))->close_first();  // 0x006dff10
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065eb50
static float JOAN_CDECL script_object_height(i32 id) {
    if (object_missing(id) != 0) return global<float>(d::k_9cf5c4);
    u8* const o = object_get(id);
    if (jo::as_map_object(o)->world_item != nullptr) return jo::as_map_object(o)->world_item->pos[2];
    return jo::as_map_object(object_get(id))->pos[2];
}

// ---------------------------------------------------------------------------- 0x0065eee0
static i32 JOAN_CDECL script_flush_436190() {
    joan::orig::as_player(obj(d::g_player_unit_recno))->set_follow(1);  // 0x00436190
    joan::orig::as_player(obj(d::g_player_unit_recno))->set_follow(0);  // 0x00436190
    while (global<i32>(d::g_a48cef) == 0 && static_cast<i32>(global<i8>(d::g_a5ba5c)) != 0) {
        joan::orig::as_player(obj(d::g_player_unit_recno))->update_camera();  // 0x004384e0
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065f980
static i32 JOAN_CDECL script_unit_set_3c1(i32 id, i32 v) {
    if (unit_missing(id) != 0) return 0;
    jo::view<i32>(jo::as_unit(unit_get(id))->vision_range) = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00661b40
static i32 JOAN_CDECL script_unit_order(i32 id, i32 a, i8 b, i32 c, i32 d, i32 e, i32 t) {
    c_unit_order.hit();
    if (unit_missing(id) != 0) return 0;
    u8* const unit = unit_get(id);
    u8 rec[0x20];  // [ebp-0x20]; 0x004e4520 writes 0x19 bytes
    std::memset(rec, 0, sizeof rec);
    if (t != 0) {
        const i32 when = static_cast<i32>(global<u32>(d::g_now) + static_cast<u32>(t));
        joan::orig::as_unit_task(static_cast<void*>(rec))->set(joan::orig::arg::u32(a), static_cast<std::uint8_t>(joan::orig::arg::u32(b)), joan::orig::arg::u32(c), joan::orig::arg::u32(d), joan::orig::arg::u32(e), joan::orig::arg::u32(when));  // 0x004e4520
    } else {
        joan::orig::as_unit_task(static_cast<void*>(rec))->set(joan::orig::arg::u32(a), static_cast<std::uint8_t>(joan::orig::arg::u32(b)), joan::orig::arg::u32(c), joan::orig::arg::u32(d), joan::orig::arg::u32(e), 0u);  // 0x004e4520
    }
    joan::orig::as_unit(unit)->set_task(joan::orig::arg::mptr(static_cast<void*>(rec)));  // 0x00527fb0
    u8* const grp = joan::orig::arg::as<u8*>(joan::orig::as_unit(unit)->leader());      // 0x004216d0
    if (grp == nullptr) return 1;
    for (i32 k = static_cast<i32>(jo::as_group(jo::as_unit(grp)->group)->count) - 1; k >= 0; --k) {
        if (unit_missing(static_cast<i32>(jo::as_group(jo::as_unit(grp)->group)->members[k])) != 0) continue;
        u8* const m = unit_get(static_cast<i32>(jo::as_group(jo::as_unit(grp)->group)->members[k]));
        joan::orig::as_unit(m)->set_task(joan::orig::arg::mptr(static_cast<void*>(rec)));  // 0x00527fb0
    }
    return 1;
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_07", name, addr, fn)
B95("script_unit_group_entry", 0x00653360, &script_unit_group_entry);
B95("script_find_unit_for", 0x00653d60, &script_find_unit_for);
B95("Unit::field_1ce", 0x00654530, &H::field_1ce);
B95("script_place_set_97", 0x00654bc0, &script_place_set_97);
B95("script_unit_set_50abd0", 0x00655620, &script_unit_set_50abd0);
B95("script_unit_1e2", 0x00655990, &script_unit_1e2);
B95("script_unit_set_2e3", 0x00655c20, &script_unit_set_2e3);
B95("script_unit_2eb", 0x00655f10, &script_unit_2eb);
B95("script_unit_set_15f", 0x006562b0, &script_unit_set_15f);
B95("script_unit_group_flag_2f", 0x006574a0, &script_unit_group_flag_2f);
B95("script_unit_group_call_41fcc0", 0x00658650, &script_unit_group_call_41fcc0);
B95("script_unit_unit_dist", 0x00659400, &script_unit_unit_dist);
B95("Obj::seconds", 0x00659760, &H::seconds);
B95("script_clear_a59179", 0x006599b0, &script_clear_a59179);
B95("script_goldbags_to_barrels", 0x00659b80, &script_goldbags_to_barrels);
B95("script_set_stat", 0x00659f40, &script_set_stat);
B95("script_42e040", 0x0065a320, &script_42e040);
B95("script_table_set", 0x0065a810, &script_table_set);
B95("script_store_rect", 0x0065aca0, &script_store_rect);
B95("script_object_name_differs", 0x0065b630, &script_object_name_differs);
B95("script_unit_523190", 0x0065c250, &script_unit_523190);
B95("script_time_since", 0x0065c9b0, &script_time_since);
B95("script_6dff10", 0x0065e850, &script_6dff10);
B95("script_object_height", 0x0065eb50, &script_object_height);
B95("script_flush_436190", 0x0065eee0, &script_flush_436190);
B95("script_unit_set_3c1", 0x0065f980, &script_unit_set_3c1);
B95("script_unit_order", 0x00661b40, &script_unit_order);
#undef B95

}  // namespace game_b95_07
}  // namespace

// ---------------------------------------- from modules/game_b94_07.cpp (game/b94_07)
namespace {
namespace game_b94_07 {

void* const kUnitTable = obj(d::g_units);
void* const kObjects = obj(d::g_items);
void* const kTypes = obj(d::g_types);
void* const kPlaces = obj(d::g_places);
void* const kGroups = obj(d::g_nations);
void* const kNames = obj(d::g_names);

JOAN_COUNTER(c_reset_first, "script_reset_first_unit");
JOAN_COUNTER(c_object_dist, "script_unit_object_dist");
JOAN_COUNTER(c_set_heading, "script_unit_set_heading");
JOAN_COUNTER(c_unlink_1f5, "script_unit_unlink_1f5");
JOAN_COUNTER(c_release_slots, "script_unit_release_slots");
JOAN_COUNTER(c_group_size, "script_unit_group_size");
JOAN_COUNTER(c_attach, "script_unit_attach");
JOAN_COUNTER(c_name_check, "script_unit_name_check");
JOAN_COUNTER(c_group_size_of, "script_group_size_of");
JOAN_COUNTER(c_group_transfer, "script_group_transfer");
JOAN_COUNTER(c_name_unused, "script_name_unused");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

// Unit type record (word +0x3b pushed with garbage upper bits; 0x0042d0a0 reads the word).
u8* type_of(const void* unit) {
    return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
}

// Shared body of the two release paths of 0x00657cc0 (slot k of the owner).
void release_member(u8* owner, u8* member, i32 k) {
    const float h = joan::orig::arg::as<float>(joan::orig::as_ground_object(owner)->heading()) / global<float>(d::flt_0_017453292);  // 0x004ff850 / K
    joan::orig::as_unit(member)->set_heading(joan::orig::arg::f32(h));                                                 // 0x0050abd0
    jo::as_unit(member)->weapon = 0;
    jo::as_unit(member)->weapon_slot = 0;
    jo::as_unit(member)->slot_index = -1;
    joan::orig::as_unit(member)->play_idle(0, 1);  // 0x00508b90
    jo::as_unit(owner)->mask_slots[k].value = 0;
    jo::as_unit(owner)->mask_slots_b[k].value = 0;
    jo::as_unit(member)->anim_special = 0;
}

// ---------------------------------------------------------------------------- 0x00653a90
static i32 JOAN_CDECL script_reset_first_unit() {
    c_reset_first.hit();
    for (i32 i = 0; i < jo::as_world(global<void*>(d::g_world))->cell_count; ++i) {
        jo::Cell* const e = jo::as_world(global<void*>(d::g_world))->cells[i].item;
        if (static_cast<i32>(jo::view<i8>(e->loaded)) == 0) continue;
        if (e->camera_zones == nullptr) continue;
        if (static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_track(e->camera_zones)->is_type1())) == 0) continue;  // 0x00428aa0
        if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_game(obj(d::g_game_state))->flag_744())) != 0) return 0;      // 0x004301c0
        joan::orig::as_track(jo::as_world(global<void*>(d::g_world))->cells[i].item->camera_zones)->restart();  // 0x004289e0
        joan::orig::as_track(jo::as_world(global<void*>(d::g_world))->cells[i].item->camera_zones)->set_4a(joan::orig::arg::i16(0));  // 0x00654ab0
        if (unit_missing(global<i32>(d::g_player_unit_recno)) == 0) {
            u8 cam[0x28];  // [ebp-0x2c], filled by 0x00423fe0 (10 dwords)
            void* const c1 = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCameras)->motion(0));  // 0x00563c80
            joan::orig::as_camera_motion(c1)->end(joan::orig::arg::mptr(static_cast<void*>(cam)));          // 0x00423fe0
            jo::view<i32>(jo::self<jo::CamPose>(cam)->target[0]) = jo::view<i32>(jo::as_unit(unit_get(global<i32>(d::g_player_unit_recno)))->heading);
            void* const c2 = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCameras)->motion(0));                    // 0x00563c80
            joan::orig::as_camera_motion(c2)->start(joan::orig::arg::mptr(static_cast<void*>(cam)), joan::orig::arg::mptr(static_cast<void*>(cam)));  // 0x00423ec0
        }
        return 1;
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x006549c0
static i32 JOAN_CDECL script_unit_object_dist(i32 a, i32 b) {
    c_object_dist.hit();
    if (joan::orig::arg::as<i32>(joan::orig::as_list(kPlaces)->empty_at_e(joan::orig::arg::i32(b))) != 0) return 0xffff;  // 0x0044cf80
    if (unit_missing(a) != 0) return 0xffff;
    float* const place = jo::as_facility(joan::orig::arg::as<u8*>(joan::orig::as_list(kPlaces)->get_e(joan::orig::arg::i32(b))))->pos;  // 0x0043c450
    float* const pos = jo::as_unit(unit_get(a))->position_;
    float tmp[3];
    const float* const d = joan::orig::arg::as<const float*>(joan::orig::vec_sub(static_cast<void*>(tmp), joan::orig::arg::ptr(pos), joan::orig::arg::ptr(place)));  // 0x0060cc90
    float v[3];
    std::memcpy(v, d, sizeof v);
    return joan::hook::x87::ftol(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(v)));  // length, _ftol
}

// ---------------------------------------------------------------------------- 0x00655590
static i32 JOAN_CDECL script_unit_set_heading(i16 id, i32 deg) {
    c_set_heading.hit();
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    const float rad = static_cast<float>(
        Fpu{true}.mul(static_cast<double>(deg), static_cast<double>(global<float>(d::flt_0_017453292))));
    joan::orig::as_unit(unit_get(static_cast<i32>(id)))->set_target(joan::orig::arg::f32(rad));  // 0x004ff870
    const float fdeg = static_cast<float>(deg);
    joan::orig::as_unit(unit_get(static_cast<i32>(id)))->set_heading(joan::orig::arg::f32(fdeg));  // 0x0050abd0
    return 1;
}

// ---------------------------------------------------------------------------- 0x00655a40
static i32 JOAN_CDECL script_unit_unlink_1f5(i16 id) {
    c_unlink_1f5.hit();
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit_get(static_cast<i32>(id)))->get_1f5()) != 0) {  // 0x004f2f60
        if (unit_missing(jo::as_unit(unit_get(static_cast<i32>(id)))->mount_ref) == 0) {
            const i32 other = jo::as_unit(unit_get(static_cast<i32>(id)))->mount_ref;
            jo::as_unit(unit_get(other))->rider_ref = 0;
        }
        jo::as_unit(unit_get(static_cast<i32>(id)))->mount_ref = 0;
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x00655de0
static i32 JOAN_CDECL script_unit_2e9(i16 id) {
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    return static_cast<i32>(jo::as_unit(unit_get(static_cast<i32>(id)))->can_interact);
}

// ---------------------------------------------------------------------------- 0x00656300
static i32 JOAN_CDECL script_unit_15f(i32 id) {
    if (unit_missing(id) != 0) return 0;
    return static_cast<i32>(jo::as_unit(unit_get(id))->stats[5]);
}

// ---------------------------------------------------------------------------- 0x00657cc0
static i32 JOAN_CDECL script_unit_release_slots(i16 id, i32 k) {
    c_release_slots.hit();
    if (unit_missing(static_cast<i32>(id)) != 0) return -1;
    u8* const owner = unit_get(static_cast<i32>(id));
    if (k < 0) {
        for (i32 i = jo::self<jo::UnitType>(type_of(owner))->slot_count - 1; i >= 0; --i) {
            u8* const member = unit_get(jo::as_unit(owner)->mask_slots[i].value);
            release_member(owner, member, i);
            joan::orig::as_cell_object(member)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));                                   // 0x0050a480
            joan::orig::unit_refresh_items(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(member)->type_id)));      // 0x0065b9b0
        }
        return 1;
    }
    if (k >= jo::self<jo::UnitType>(type_of(owner))->slot_count) return 0;
    if (unit_missing(jo::as_unit(owner)->mask_slots[k].value) != 0) {
        jo::as_unit(owner)->mask_slots[k].value = 0;
        jo::as_unit(owner)->mask_slots_b[k].value = 0;
        return 1;
    }
    u8* const member = unit_get(jo::as_unit(owner)->mask_slots[k].value);
    release_member(owner, member, k);
    joan::orig::unit_refresh_items(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(member)->type_id)));  // 0x0065b9b0
    return 1;
}

// ---------------------------------------------------------------------------- 0x00658160
static i32 JOAN_CDECL script_unit_group_size(i16 id) {
    c_group_size.hit();
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    u8* const unit = unit_get(static_cast<i32>(id));
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->is_busy()) != 0) {                  // 0x004b62b0
        u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_unit(unit)->leader());           // 0x004216d0
        if (g != nullptr && jo::as_unit(g)->group != nullptr) return static_cast<i32>(jo::as_group(jo::as_unit(g)->group)->count);
    }
    return 0;
}

// ---------------------------------------------------------------------------- 0x00658ea0
static i32 JOAN_CDECL script_unit_attach(i16 id, i16 b, const char* bone, i8 slot) {
    c_attach.hit();
    if (unit_missing(static_cast<i32>(id)) != 0) return 0;
    if (static_cast<i32>(b) == 0) {
        joan::orig::as_cell_object(unit_get(static_cast<i32>(id)))->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<i32>(slot))), joan::orig::arg::mptr(0));  // 0x0050a480
        return 1;
    }
    if (joan::orig::arg::as<i32>(joan::orig::bone_id_of_name(joan::orig::arg::str(bone))) == 0) return 0;  // 0x00423220
    const i32 anim = joan::orig::arg::as<i32>(joan::orig::bone_id_of_name(joan::orig::arg::str(bone)));    // 0x00423220
    joan::orig::as_cell_object(unit_get(static_cast<i32>(id)))->attach_unit(joan::orig::arg::i16(static_cast<i32>(b)), joan::orig::arg::i32(anim), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<i32>(slot))), joan::orig::arg::mptr(0));  // 0x0050a480
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659830
static i32 JOAN_CDECL script_set_a58faf(i32 v) {
    global<i32>(d::g_target_stage) = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659a90
static i32 JOAN_CDECL script_set_a591d8(i32 v) {
    global<i32>(d::g_get_a591d8) = v;
    return 1;
}

// ---------------------------------------------------------------------------- 0x00659ef0
static i32 JOAN_CDECL script_clear_a591a5() {
    global<i32>(d::g_get_a591a5) = 0;
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065a670
static i32 JOAN_CDECL script_table_word(i32 n) {
    const u32 off = (static_cast<u32>(n) - 1u) * 0x1au;
    return static_cast<i32>(global<i16>(d::g_a592a4 + off));
}

// ---------------------------------------------------------------------------- 0x0065adb0
static i32 JOAN_CDECL script_set_flag_a501da(i32 v) {
    global<u8>(d::g_a501da) = v != 0 ? 1 : 0;
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065c0d0
static i32 JOAN_CDECL script_unit_1ae(i16 id) {
    if (unit_missing(static_cast<i32>(id)) != 0) return -1;
    return static_cast<i32>(jo::view<i16>(jo::as_unit(unit_get(static_cast<i32>(id)))->kill_points));
}

// ---------------------------------------------------------------------------- 0x0065c930
static i32 JOAN_CDECL script_unit_name_check(i32 id, const char* name) {
    c_name_check.hit();
    if (unit_missing(id) != 0) return 1;
    u8* const unit = unit_get(id);
    if (stricmp_(jo::as_unit(unit)->name_, name) != 0) return 1;
    return static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->dead()));  // 0x0042d0e0
}

// ---------------------------------------------------------------------------- 0x0065d8a0
static i32 JOAN_CDECL script_group_size_of(i32 key) {
    c_group_size_of.hit();
    for (i32 i = joan::orig::as_dynarray(kGroups)->size(); i > 0; --i) {  // 0x00417ad0
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kGroups)->is_null(joan::orig::arg::i32(i))) != 0) continue;   // 0x00484870
        u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kGroups)->ptr_at(joan::orig::arg::i32(i)));  // 0x0047b600
        for (i32 k = static_cast<i32>(jo::view<i16>(jo::as_nation(g)->leader_count)); k > 0; --k) {
            const i32 id = static_cast<i32>(jo::as_nation(g)->leaders[k - 1].value);
            if (unit_missing(id) != 0) continue;
            u8* const unit = unit_get(id);
            if (static_cast<i32>(jo::as_unit(unit)->group_id) != key) continue;
            if (jo::as_unit(unit)->group == nullptr) continue;
            return static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->count);
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------- 0x0065ea00
static i32 JOAN_CDECL script_object_a2(i32 id) {
    if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00425d50
    return static_cast<i32>(reinterpret_cast<std::uintptr_t>(jo::as_map_object(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id)))->world_item));  // 0x00425d20; x64: the script gets the pointer as an int
}

// ---------------------------------------------------------------------------- 0x0065ef40
static i32 JOAN_CDECL script_group_transfer(i32 a, i32 b, i32 n) {
    c_group_transfer.hit();
    if (unit_missing(a) != 0) return 0;
    if (unit_missing(b) != 0) return 0;
    u8* const ua = unit_get(a);
    u8* const gb = jo::view<u8*>(jo::as_unit(unit_get(b))->group);
    u8* const ga = jo::view<u8*>(jo::as_unit(ua)->group);
    if (gb == nullptr || ga == nullptr) return 0;
    for (i32 i = 0; i < static_cast<i32>(jo::as_group(gb)->count); ++i) {
        if (n == 0) break;
        if (!(joan::orig::arg::as<i32>(joan::orig::as_unit(ua)->morale_level()) > static_cast<i32>(jo::as_group(ga)->count))) break;  // 0x00448080
        if (unit_missing(static_cast<i32>(jo::as_group(gb)->members[i])) != 0) continue;
        u8* const m = unit_get(static_cast<i32>(jo::as_group(gb)->members[i]));
        joan::orig::as_member(m)->release();                                         // 0x0042fcb0
        joan::orig::as_unit(m)->link_to(static_cast<i32>(jo::as_group(ga)->leader));       // 0x00438f80
        if (jo::as_unit(m)->draw != nullptr) {
            joan::orig::as_unit_draw(jo::as_unit(m)->draw)->label_delete();  // 0x005213b0
            joan::orig::as_unit_draw(jo::as_unit(m)->draw)->create_label();  // 0x00520e80
        }
        --n;
    }
    return 1;
}

// ---------------------------------------------------------------------------- 0x0065fbf0
static i32 JOAN_CDECL script_name_unused(const char* name) {
    c_name_unused.hit();
    for (i32 i = joan::orig::as_dynarray(kNames)->size(); i > 0; --i) {  // 0x00417ad0
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kNames)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;   // 0x004af300
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(kNames)->at_4af2d0(joan::orig::arg::u32(i)));        // 0x004af2d0
        if (stricmp_(jo::as_message(e)->text, name) == 0) return 0;
    }
    return 1;
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("script_reset_first_unit", 0x00653a90, &script_reset_first_unit);
B94("script_unit_object_dist", 0x006549c0, &script_unit_object_dist);
B94("script_unit_set_heading", 0x00655590, &script_unit_set_heading);
B94("script_unit_unlink_1f5", 0x00655a40, &script_unit_unlink_1f5);
B94("script_unit_2e9", 0x00655de0, &script_unit_2e9);
B94("script_unit_15f", 0x00656300, &script_unit_15f);
B94("script_unit_release_slots", 0x00657cc0, &script_unit_release_slots);
B94("script_unit_group_size", 0x00658160, &script_unit_group_size);
B94("script_unit_attach", 0x00658ea0, &script_unit_attach);
B94("script_set_a58faf", 0x00659830, &script_set_a58faf);
B94("script_set_a591d8", 0x00659a90, &script_set_a591d8);
B94("script_clear_a591a5", 0x00659ef0, &script_clear_a591a5);
B94("script_table_word", 0x0065a670, &script_table_word);
B94("script_set_flag_a501da", 0x0065adb0, &script_set_flag_a501da);
B94("script_unit_1ae", 0x0065c0d0, &script_unit_1ae);
B94("script_unit_name_check", 0x0065c930, &script_unit_name_check);
B94("script_group_size_of", 0x0065d8a0, &script_group_size_of);
B94("script_object_a2", 0x0065ea00, &script_object_a2);
B94("script_group_transfer", 0x0065ef40, &script_group_transfer);
B94("script_name_unused", 0x0065fbf0, &script_name_unused);
#undef B94

}  // namespace game_b94_07
}  // namespace

// ---------------------------------------- from modules/game_b100_13.cpp (game/b100_13)
namespace {
namespace game_b100_13 {

using joan::orig::helpers::at;
i32 sx16(u32 v) { return static_cast<i32>(static_cast<i16>(static_cast<u16>(v))); }

void* const kNames = obj(d::g_nation_res);
void* const kSides = obj(d::g_nations);
void* const kUnits = obj(d::g_units);
void* const kPlaces = obj(d::g_place_array);
void* const kTypes = obj(d::g_types);
void* const kObjects = obj(d::g_map_object_list);
void* const kKinds = obj(d::g_item_res);
void* const kFactory = obj(d::g_places);
void* const kMission = obj(d::g_mission);

struct Vec3 {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

i32 name_id(const char* name) { return joan::orig::arg::as<i32>(joan::orig::as_name_table(kNames)->find_4b91a0(name)); }          // 0x004b91a0
i32 side_of(i32 id) { return joan::orig::arg::as<i32>(joan::orig::as_list(kSides)->find_by_word2(joan::orig::arg::i32(id))); }               // 0x004b67e0
i32 count(void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0
void* item(void* list, i32 i) { return joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0
i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
void* unit(i32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
void* factory_get(i32 id) { return joan::orig::arg::as<void*>(joan::orig::as_list(kFactory)->get_e(joan::orig::arg::i32(id))); }     // 0x0043c450
i32 factory_missing(i32 id) { return joan::orig::arg::as<i32>(joan::orig::as_list(kFactory)->empty_at_e(joan::orig::arg::i32(id))); }     // 0x0044cf80
void* objective(u32 k) { return obj(d::g_a5929b + (k - 1u) * 0x1au); }

JOAN_COUNTER(c_relation, "Script::set_side_relation");
JOAN_COUNTER(c_place_switch, "Script::place_switch_side");
JOAN_COUNTER(c_place_set, "Script::place_set_side");
JOAN_COUNTER(c_unit_side, "Script::unit_set_side");
JOAN_COUNTER(c_units_2eb, "Script::units_set_2eb");
JOAN_COUNTER(c_join_group, "Script::unit_join_group");
JOAN_COUNTER(c_group_side, "Script::group_set_side");
JOAN_COUNTER(c_follow, "Script::group_follow_leader");
JOAN_COUNTER(c_set_stat, "Script::set_stat");

// Place scan shared by 0x6542b0 / 0x654560 (two literal copies in the original): units of type
// class 8 inside the place area take `unit_side`, objects of kind 4 / 1 inside get +8 =
// `object_side`.
void place_scan(void* place, i32 unit_side, i32 object_side, bool place_owner_compare) {
    for (i32 i = count(kUnits); i > 0; --i) {
        void* const u = item(kUnits, i);
        if (u == nullptr) continue;
        const i32 us = static_cast<i32>(jo::as_unit(u)->side);
        if (place_owner_compare) {
            if (us == jo::as_place(place)->nation) continue;
        } else {
            if (us == unit_side) continue;
        }
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0) continue;  // 0x0042d0e0
        u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(sx16(jo::view<u16>(jo::as_unit(u)->type_id)))));  // 0x0042d0a0
        if (static_cast<i32>(jo::self<jo::UnitType>(type)->unit_class) != 8) continue;
        Vec3 pos;
        joan::orig::as_unit(u)->position(&pos);  // 0x004fedb0
        if ((joan::orig::arg::as<u32>(joan::orig::as_aabb(jo::as_place(place)->box)->contains(&pos)) & 0xffu) != 0)  // 0x0060d5f0
            joan::orig::as_unit(u)->set_side(joan::orig::arg::i32(sx16(static_cast<u32>(unit_side))));       // 0x00509250
    }
    for (i32 i = count(kObjects); i > 0; --i) {
        void* const e = item(kObjects, i);
        if (e == nullptr) continue;
        i16* const kind = joan::orig::arg::as<i16*>(joan::orig::as_records_2bf(kKinds)->get(joan::orig::arg::i16(sx16(jo::view<u16>(jo::as_map_object(e)->type_id)))));  // 0x00425ce0
        if (static_cast<i32>(*kind) != 4 && static_cast<i32>(*kind) != 1) continue;
        Vec3 pos;
        joan::orig::as_map_object(e)->position(&pos);  // 0x00499930
        if ((joan::orig::arg::as<u32>(joan::orig::as_aabb(jo::as_place(place)->box)->contains(&pos)) & 0xffu) != 0)  // 0x0060d5f0
            jo::view<u16>(jo::as_map_object(e)->side) = static_cast<u16>(object_side);
    }
}

// ---------------------------------------------------------------------------- cdecl commands
static i32 JOAN_CDECL set_side_relation(const char* name1, const char* name2, u32 value) {  // 0x00653df0 (the names are string values of the script)
    c_relation.hit();
    const i32 n1 = name_id(name1);
    const i32 n2 = name_id(name2);
    if (n1 == 0 || n2 == 0) return 0;
    const i32 s1 = side_of(n1);
    const i32 s2 = side_of(n2);
    if (s1 == 0 || s2 == 0) return 0;
    void* const rec = joan::orig::as_dynarray(kSides)->ptr_at(joan::orig::arg::i32(s1));  // 0x0047b600
    joan::orig::as_nation(rec)->set_relation_value(joan::orig::arg::i32(s2), joan::orig::arg::u32(value));                         // 0x0042a1c0
    return 1;
}

static i32 JOAN_CDECL reset_object_1ca(u32 id) {  // 0x006541d0
    jo::view<u32>(jo::as_facility(factory_get(sx16(id)))->owner->spawn_count) = 0;
    void* const o = factory_get(sx16(id));
    void* const r = virtual_call<void*>(o, 0x18);
    joan::orig::as_unit(r)->clear_79();  // 0x00467f90
    return 1;
}

static i32 JOAN_CDECL place_switch_side(u32 place_id, u32 flag) {  // 0x006542b0
    c_place_switch.hit();
    void* const place = joan::orig::arg::as<void*>(joan::orig::as_list(kPlaces)->get_c70(joan::orig::arg::i32(place_id)));  // 0x00456c70
    joan::orig::as_place(place)->set_1ce(joan::orig::arg::u32(flag));                                   // 0x004c46f0
    const i32 french = side_of(name_id(img::str(d::s_FRENCH)));                     // "FRENCH"
    const i32 english = side_of(name_id(img::str(d::s_ENGLISH)));                    // "ENGLISH"
    i32 target;
    if (jo::as_place(place)->nation == french) {
        target = flag != 0 ? english : french;
    } else if (jo::as_place(place)->nation == english) {
        target = flag != 0 ? french : english;
    } else {
        return 1;
    }
    place_scan(place, target, target, false);
    return 1;
}

static i32 JOAN_CDECL place_set_side(u32 place_id, const char* name) {  // 0x00654560 (name: a string value of the script)
    c_place_set.hit();
    void* const place = joan::orig::arg::as<void*>(joan::orig::as_list(kPlaces)->get_c70(joan::orig::arg::i32(place_id)));  // 0x00456c70
    i32 side = 0;
    const i32 n = name_id(name);
    if (n != 0) side = side_of(n);
    if (side == 0) return 1;
    joan::orig::as_place(place)->join_group(name);  // 0x0042acd0 Place::join_group(const char*)
    const i32 unit_side = side_of(name_id(name));
    place_scan(place, unit_side, side, true);
    return 1;
}

static i32 JOAN_CDECL object_set_81(u32 id, u32 v) {  // 0x00654ea0
    if (factory_missing(sx16(id)) != 0) return 0;
    void* const o = factory_get(sx16(id));
    // Vtable slot 8 (+0x20) is the Shop cast: Shop's 0x00469e40 returns `this`, the other facilities 0. The
    // field at +0x81 (x86) is Shop::i32_81, read by Shop::restock (f_shop.cpp). Written at the raw x86 offset it
    // landed on the facility's `next` link on x64 and the list unlink crashed when the mission was left.
    jo::Shop* const shop = virtual_call<jo::Shop*>(o, 0x20);
    shop->i32_81 = static_cast<i32>(v);
    return 1;
}

static i32 JOAN_CDECL object_call_46b730(u32 id, u32 v) {  // 0x00654fa0
    if (factory_missing(sx16(id)) != 0) return 0;
    void* const o = factory_get(sx16(id));
    void* const r = virtual_call<void*>(o, 0x1c);
    joan::orig::as_workshop_facility(r)->load_types(joan::orig::arg::i32(v));  // 0x0046b730
    return 1;
}

static i32 JOAN_CDECL unit_set_5089e0(u32 id, const char* name) {  // 0x006556e0
    if (unit_missing(sx16(id)) != 0) return 0;
    const u32 v = joan::orig::arg::as<u32>(joan::orig::as_unit(kTypes)->rank_of_name(name));  // 0x00527130
    void* const u = unit(sx16(id));
    joan::orig::as_unit(u)->set_level_state(joan::orig::arg::i32(v));  // 0x005089e0
    return sx16(id);
}

static i32 JOAN_CDECL unit_set_side(u32 id, const char* name) {  // 0x00655750 (name: a string value of the script)
    c_unit_side.hit();
    if (unit_missing(sx16(id)) != 0) return 0;
    void* const u = unit(sx16(id));
    const i32 n = name_id(name);
    if (n == 0) {
        jo::view<u16>(jo::as_unit(u)->side) = 0;
        return 0;
    }
    const i32 side = side_of(n);
    if (side != 0) {
        joan::orig::as_unit(u)->set_side(joan::orig::arg::i32(sx16(static_cast<u32>(side))));  // 0x00509250
        return side;
    }
    jo::view<u16>(jo::as_unit(u)->side) = 0;
    return 0;
}

static u32 JOAN_CDECL unit_field_1e2(u32 id) {  // 0x00655b20
    if (unit_missing(sx16(id)) != 0) return 0;
    return jo::view<u32>(jo::as_unit(unit(sx16(id)))->mount_id);
}

static i32 JOAN_CDECL unit_set_2e5(u32 id, u32 w) {  // 0x00655cd0
    if (unit_missing(sx16(id)) == 0) jo::view<u16>(jo::as_unit(unit(sx16(id)))->interact_item) = static_cast<u16>(w);
    return 1;
}

static i32 JOAN_CDECL units_set_2eb(u32 group, u32 w) {  // 0x00655f80
    c_units_2eb.hit();
    for (i32 i = 1; i <= count(kUnits); ++i) {
        void* const u = item(kUnits, i);
        if (u == nullptr) continue;
        if (static_cast<i32>(jo::as_unit(u)->type_id) != sx16(group)) continue;
        if (static_cast<i32>(jo::as_unit(u)->bribe_state) == 2) continue;
        jo::view<u16>(jo::as_unit(u)->bribe_state) = static_cast<u16>(w);
    }
    return 1;
}

static i32 JOAN_CDECL unit_2eb_is_2(u32 id) {  // 0x006561b0
    if (unit_missing(sx16(id)) != 0) return 0;
    return static_cast<i32>(jo::as_unit(unit(sx16(id)))->bribe_state) == 2 ? 1 : 0;
}

static i32 JOAN_CDECL unit_copy_13d(u32 id) {  // 0x006568d0
    if (unit_missing(static_cast<i32>(id)) == 0) {
        void* const src = unit(static_cast<i32>(id));
        void* const dst = unit(static_cast<i32>(id));
        jo::view<u32>(jo::as_unit(dst)->hit_points) = jo::view<u32>(jo::as_unit(src)->hit_points_base);
    }
    return 1;
}

static i32 JOAN_CDECL unit_join_group(u32 id, u32 group) {  // 0x00657fd0
    c_join_group.hit();
    if (unit_missing(sx16(id)) != 0) return 0;
    void* const u = unit(sx16(id));
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->leads()) != 0) {  // 0x0044a660
        joan::orig::as_unit(u)->sync_shadow();              // 0x00447600
        jo::view<u16>(jo::as_unit(u)->group_id) = static_cast<u16>(group);
        for (i32 i = count(kUnits); i > 0; --i) {
            void* const e = item(kUnits, i);
            if (e == nullptr) continue;
            if (i == static_cast<i32>(jo::as_unit(u)->id)) continue;
            if (static_cast<i32>(jo::as_unit(e)->group_id) == sx16(group))
                joan::orig::as_unit(e)->link_to(joan::orig::arg::i32(sx16(id)));  // 0x00438f80
        }
        return 1;
    }
    for (i32 i = count(kUnits); i > 0; --i) {
        void* const e = item(kUnits, i);
        if (e == nullptr) continue;
        if (static_cast<i32>(jo::as_unit(e)->group_id) != sx16(group)) continue;
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(e)->leads()) == 0) continue;  // 0x0044a660
        joan::orig::as_unit(u)->link_to(static_cast<i32>(jo::as_unit(e)->id));  // 0x00438f80
        return 1;
    }
    jo::view<u16>(jo::as_unit(u)->group_id) = static_cast<u16>(group);
    return 0;
}

static i32 JOAN_CDECL group_set_side(u32 name, const char* side_name) {  // 0x006582f0 (side_name: a string value of the script)
    c_group_side.hit();
    const i32 id = joan::orig::arg::as<i32>(joan::orig::find_unit_in_groups(joan::orig::arg::i16(name)));  // 0x006581f0 (reads the word)
    if (unit_missing(id) != 0) return 0;
    void* const u = unit(id);
    const i32 side = side_of(name_id(side_name));
    if (!(side > 0)) return 0;
    jo::view<u16>(jo::as_unit(u)->side) = static_cast<u16>(side);
    for (i32 i = 0; i < static_cast<i32>(jo::as_group(jo::as_unit(u)->group)->count); ++i) {
        if (unit_missing(static_cast<i32>(jo::as_group(jo::as_unit(u)->group)->members[i])) != 0) continue;
        void* const m = unit(static_cast<i32>(jo::as_group(jo::as_unit(u)->group)->members[i]));
        jo::view<u16>(jo::as_unit(m)->side) = static_cast<u16>(side);
    }
    return 1;
}

static i32 JOAN_CDECL group_follow_leader(u32 id) {  // 0x00658920
    c_follow.hit();
    void* const u = unit(sx16(id));
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->is_busy()) == 0) return 0;  // 0x004b62b0
    void* const rec = joan::orig::arg::as<void*>(joan::orig::as_unit(u)->leader());  // 0x004216d0
    if (rec == nullptr || jo::as_unit(rec)->group == nullptr) return 0;
    Vec3 pos;
    joan::orig::as_unit(u)->position(&pos);                        // 0x004fedb0
    const float heading = joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading());  // 0x004ff850
    u32 heading_bits;
    std::memcpy(&heading_bits, &heading, 4);
    for (i32 i = 0; i < static_cast<i32>(jo::as_group(jo::as_unit(rec)->group)->count); ++i) {
        if (unit_missing(static_cast<i32>(jo::as_group(jo::as_unit(rec)->group)->members[i])) != 0) continue;
        void* const m = unit(static_cast<i32>(jo::as_group(jo::as_unit(rec)->group)->members[i]));
        Vec3 sum;
        const Vec3 v = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&sum, &pos, joan::orig::arg::ptr(jo::as_unit(m)->formation_offset)));  // 0x0060cc20
        joan::orig::as_unit(m)->set_group(jo::as_unit(rec)->world);  // 0x004fed00
        joan::orig::as_ground_object(m)->set_position(v);                    // 0x004fedf0
        joan::orig::as_unit(m)->set_target(joan::orig::arg::f32(heading_bits));         // 0x004ff870
    }
    return 0;
}

static i32 JOAN_CDECL flag_a59298() { return global<i8>(d::g_flag_a59298) != 0 ? 1 : 0; }  // 0x006595e0

static i32 JOAN_CDECL call_51c4b0(u32 b) {  // 0x00659860
    joan::orig::as_player(obj(d::g_player_unit_recno))->set_byte_c4(joan::orig::arg::u32(b & 0xffu));  // 0x0051c4b0 (reads the byte)
    return 1;
}

static i32 JOAN_CDECL clear_a59171() {  // 0x00659910
    global<u32>(d::g_get_a59171) = 0;
    return 1;
}
static i32 JOAN_CDECL clear_a5918d() {  // 0x00659ae0
    global<u32>(d::g_get_a5918d) = 0;
    return 1;
}
static i32 JOAN_CDECL clear_a591c8() {  // 0x00659d60
    global<u32>(d::g_get_a591c8) = 0;
    return 1;
}

static i32 JOAN_CDECL call_4cdd40(u32 w) {  // 0x00659de0
    joan::orig::as_scenario(kMission)->push_word_4cdd40(joan::orig::arg::i16(sx16(w)));  // 0x004cdd40 (reads the word)
    return 1;
}

static i32 JOAN_CDECL set_stat(const char* name, u32 i, u32 b) {  // 0x0065a0e0
    c_set_stat.hit();
    u32 k = 0;
    if (joan::crt::stricmp(joan::orig::arg::str(name), str(d::s_STRENGTH)) == 0) {         // "STRENGTH"
        k = 0;
    } else if (joan::crt::stricmp(joan::orig::arg::str(name), str(d::s_DEXTERITY)) == 0) {  // "DEXTERITY"
        k = 1;
    } else if (joan::crt::stricmp(joan::orig::arg::str(name), str(d::s_LEADERSHIP)) == 0) {  // "LEADERSHIP"
        k = 2;
    } else if (joan::crt::stricmp(joan::orig::arg::str(name), str(d::s_DEFENSE)) == 0) {  // "DEFENSE"
        k = 3;
    }
    global<u8>(d::g_a5922f + (k * 5u + i)) = static_cast<u8>(b);
    return 1;
}

static i32 JOAN_CDECL objective_467300(u32 k, const char* v) {  // 0x0065a3c0
    joan::orig::as_end_condition(objective(k))->set_text(v);  // 0x00467300
    return 1;
}

static i32 JOAN_CDECL objective_update(u32 k, const char* v) {  // 0x0065a400
    joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_OBJECTIVE_UPDATE));  // 0x004ceef0 "OBJECTIVE_UPDATE"
    joan::orig::as_end_condition(objective(k))->set_text(v);                    // 0x00467300
    joan::orig::as_hud(obj(d::g_player_hud))->blink(joan::orig::arg::i32(k - 1u));            // 0x007076c0
    return 1;
}

static i32 JOAN_CDECL objective_467390(u32 k, const char* v) {  // 0x0065a460
    joan::orig::as_end_condition(objective(k))->set_subtext(v);  // 0x00467390
    return 1;
}

static u32 JOAN_CDECL objective_set_15(u32 k, u32 v) {  // 0x0065a840
    global<u32>(d::g_a592b0 + (k - 1u) * 0x1au) = v;
    return v;
}

static i32 JOAN_CDECL call_472000(u32 a, u32 b) {  // 0x0065a870
    joan::orig::as_game(kFlags)->set_flag_1b5(joan::orig::arg::i32(a), joan::orig::arg::i32(b & 0xffu));  // 0x00472000 (masks the byte)
    return 1;
}

static i32 JOAN_CDECL call_472220(u32 a) {  // 0x0065a900
    return static_cast<i32>(static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_player(kFlags)->flag_bit_472220(joan::orig::arg::i32(a)))));  // 0x00472220
}

static i32 JOAN_CDECL call_472540() {  // 0x0065a930
    joan::orig::as_game(kFlags)->clear_flags_1b9();  // 0x00472540
    return 1;
}

static u32 JOAN_CDECL current_19a() {  // 0x0065aa80
    if (global<u32>(d::g_mission) != 0) {
        return global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].u32_19a;
    }
    return global<u32>(d::g_a59155);
}

static i32 JOAN_CDECL call_4cde30(const char* a) {  // 0x0065ab10
    void* const p = joan::orig::arg::as<void*>(joan::orig::as_scenario(kMission)->current());  // 0x004cd840
    joan::orig::as_scenario(p)->set_name_4cde30(a);                              // 0x004cde30
    return 1;
}

struct H {
    void* set_word_44(u32 w) {  // 0x00654a80
        jo::view<u16>(jo::self<jo::CamZones>(this)->looping) = static_cast<u16>(w);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_13", name, addr, fn)
B("Script::set_side_relation", 0x00653df0, &set_side_relation);
B("Script::reset_object_1ca", 0x006541d0, &reset_object_1ca);
B("Script::place_switch_side", 0x006542b0, &place_switch_side);
B("Script::place_set_side", 0x00654560, &place_set_side);
B("Obj::set_word_44", 0x00654a80, &H::set_word_44);
B("Script::object_set_81", 0x00654ea0, &object_set_81);
B("Script::object_call_46b730", 0x00654fa0, &object_call_46b730);
B("Script::unit_set_5089e0", 0x006556e0, &unit_set_5089e0);
B("Script::unit_set_side", 0x00655750, &unit_set_side);
B("Script::unit_field_1e2", 0x00655b20, &unit_field_1e2);
B("Script::unit_set_2e5", 0x00655cd0, &unit_set_2e5);
B("Script::units_set_2eb", 0x00655f80, &units_set_2eb);
B("Script::unit_2eb_is_2", 0x006561b0, &unit_2eb_is_2);
B("Script::unit_copy_13d", 0x006568d0, &unit_copy_13d);
B("Script::unit_join_group", 0x00657fd0, &unit_join_group);
B("Script::group_set_side", 0x006582f0, &group_set_side);
B("Script::group_follow_leader", 0x00658920, &group_follow_leader);
B("Script::flag_a59298", 0x006595e0, &flag_a59298);
B("Script::call_51c4b0", 0x00659860, &call_51c4b0);
B("Script::clear_a59171", 0x00659910, &clear_a59171);
B("Script::clear_a5918d", 0x00659ae0, &clear_a5918d);
B("Script::clear_a591c8", 0x00659d60, &clear_a591c8);
B("Script::call_4cdd40", 0x00659de0, &call_4cdd40);
B("Script::set_stat", 0x0065a0e0, &set_stat);
B("Script::objective_467300", 0x0065a3c0, &objective_467300);
B("Script::objective_update", 0x0065a400, &objective_update);
B("Script::objective_467390", 0x0065a460, &objective_467390);
B("Script::objective_set_15", 0x0065a840, &objective_set_15);
B("Script::call_472000", 0x0065a870, &call_472000);
B("Script::call_472220", 0x0065a900, &call_472220);
B("Script::call_472540", 0x0065a930, &call_472540);
B("Script::current_19a", 0x0065aa80, &current_19a);
B("Script::call_4cde30", 0x0065ab10, &call_4cde30);
#undef B

}  // namespace game_b100_13
}  // namespace

// ---------------------------------------- from modules/game_b102_13.cpp (game/b102_13)
namespace {
namespace game_b102_13 {

// thiscall through the object's vtable slot `off`.
template <class R = void, class... A>
R vcall(const void* self, u32 off, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    void* const fn = joan::call::vtable_of(self)[joan::call::x86_vslot(off)];
    std::memcpy(&method, &fn, sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}
void* const kUnits = obj(d::g_units);
void* const kPlaces = obj(d::g_places);

u8* unit(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70

JOAN_COUNTER(c_check_cmd, "ScriptNode::check_cmd");
JOAN_COUNTER(c_654f10, "script_place_654f10");
JOAN_COUNTER(c_659550, "script_659550");
JOAN_COUNTER(c_65cfd0, "script_unit_65cfd0");

// ---------------------------------------------------------------------- 0x006540a0 / 0x006540e0 / 0x00654120
void* type_obj(i16 a) { return joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_place_array))->get_c70(static_cast<i32>(a))); }  // 0x00456c70

u32 JOAN_CDECL script_6540a0(i16 a, i16 b) {
    return joan::orig::arg::as<u32>(joan::orig::as_place(type_obj(a))->has_enemy_of(static_cast<i32>(b)));  // 0x004a8c20
}
u32 JOAN_CDECL script_6540e0(i16 a, const char* b) {
    return joan::orig::arg::as<u32>(joan::orig::as_place(type_obj(a))->has_unit_of(b));  // 0x004ab020
}
u32 JOAN_CDECL script_654120(i16 a, const char* b) {
    return joan::orig::arg::as<u32>(joan::orig::as_army(type_obj(a))->count_strength(b));  // 0x004b79a0
}

// ---------------------------------------------------------------------- 0x00654f10
u32 JOAN_CDECL script_place_654f10(i16 id, i16 w, u32 a, u32 b) {
    c_654f10.hit();
    if (joan::orig::arg::as<i32>(joan::orig::as_list(kPlaces)->empty_at_e(static_cast<i32>(id))) != 0) return 0;  // 0x0044cf80
    u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_list(kPlaces)->get_e(static_cast<i32>(id)));       // 0x0043c450
    u8* const r = vcall<u8*>(o, 0x24);
    joan::orig::as_recruit_facility(r)->load_flag_groups(joan::orig::arg::i32(a), joan::orig::arg::i32(b));  // 0x00468790
    jo::as_flag_facility(r)->i16_91 = w;
    return 1;
}

// ---------------------------------------------------------------------- 0x00659550
u32 JOAN_CDECL script_659550(u32 a, u32 b, u32 c, i32 d, i32 e, i32 f) {
    c_659550.hit();
    joan::orig::as_calendar(obj(d::g_calendar))->start(joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c));  // 0x00495290
    global<float>(d::g_calendar_day_start_minute) = static_cast<float>(e);
    global<float>(d::g_a47bc8) = static_cast<float>(f);
    const double hi = static_cast<double>(f);
    const double lo = static_cast<double>(global<float>(d::g_calendar_day_start_minute));
    const double v = static_cast<double>(d);
    global<float>(d::g_calendar_minute_of_day) = joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->limit_double(joan::orig::arg::f64(v), joan::orig::arg::f64(lo), joan::orig::arg::f64(hi)));  // 0x00590b00
    joan::orig::as_army(obj(d::g_light_sys))->pick_formation();  // 0x004a5630
    return 1;
}

// ---------------------------------------------------------------------- 0x0065ca40
u32 JOAN_CDECL script_unit_65ca40(const char* a, i32 id) {
    if (unit_missing(id) != 0) return 0;
    u8* const u = unit(id);
    const u32 v = joan::orig::arg::as<u32>(joan::orig::as_name_table(kList8f50)->find_49fab0(a));  // 0x0049fab0
    if (v == 0) return 0;
    return joan::orig::arg::as<u32>(joan::orig::as_unit(u)->squad_call_4985f0(joan::orig::arg::u32(v)));  // 0x00510120
}

// ---------------------------------------------------------------------- 0x0065cb20
u32 JOAN_CDECL script_unit_65cb20(u32 a, i32 id, u32 c) {
    if (unit_missing(id) != 0) return 0;
    u8* const u = unit(id);
    return joan::orig::arg::as<u32>(joan::orig::as_unit(u)->squad_call_498970(joan::orig::arg::u32(a), joan::orig::arg::u32(c)));  // 0x005101c0
}

// ---------------------------------------------------------------------- 0x0065cfd0
u32 JOAN_CDECL script_unit_65cfd0(i32 id, i32 m) {
    c_65cfd0.hit();
    if (unit_missing(id) != 0) return 0;
    u8* const u = unit(id);
    if (m == 1) joan::orig::as_unit(u)->on_flag(1, static_cast<std::uint8_t>(joan::orig::arg::u32(1)));  // 0x0050cbc0
    if (m == 0) joan::orig::as_unit(u)->switch_mount(0);       // 0x0050c8a0
    else if (m == 1) joan::orig::as_unit(u)->switch_mount(1);  // 0x0050c8a0
    return 1;
}

// ---------------------------------------------------------------------- 0x0065fcc0
u32 JOAN_CDECL script_65fcc0(i8 a) {
    if (static_cast<i32>(a) >= 0 && static_cast<i32>(a) <= 1)
        joan::orig::as_ending(obj(d::g_ending_screen))->open(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u8>(a))));  // 0x006d19e0
    return 1;
}

struct H {
    u8* param(i32 n) { return joan::orig::arg::as<u8*>(joan::orig::as_reader(this)->list_get(joan::orig::arg::i32(n))); }  // 0x0063e9f0
    i32 nparams() { return joan::orig::arg::as<i32>(joan::orig::as_reader(this)->count()); }          // 0x0063ecd0

    // ------------------------------------------------------------------ 0x00660e60
    u32 check_cmd(u32) {
        c_check_cmd.hit();
        const i32 code = static_cast<i32>(jo::self<jo::ScriptCheckCall>(this)->func_id);
        if (code == 0x13c) {
            if ((joan::orig::arg::as<u32>(joan::orig::as_dialogue(obj(d::g_dialogue))->all_ready()) & 0xffu) == 0 &&  // 0x0063d7b0
                joan::orig::arg::as<i8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) != 0 &&             // 0x004301c0
                global<i8>(d::g_a5d3c0) == 0)
                return 0;
            const u32 id = *vcall<u32*>(param(1), 0x1c);
            u32 v = 0;
            if (nparams() == 3) v = *vcall<u32*>(param(3), 0x1c);
            else if (nparams() == 7) v = *vcall<u32*>(param(7), 0x1c);
            u32 eax = static_cast<u32>(unit_missing(static_cast<i32>(id)));
            if (eax == 0 && v != 0xffffff9cu) eax = joan::call::register_low_bits(unit(static_cast<i32>(id)));  // dead store in the original
            return (eax & 0xffffff00u) | 1u;
        }
        if (code == 0x13f) {
            return thiscall<u32>(fn::Dialogue_hit_choice, obj(d::g_dialogue), *static_cast<u32*>(joan::orig::arg::mptr(jo::self<jo::ScriptCheckCall>(this)->value)));  // 0x0063d860
        }
        return (static_cast<u32>(static_cast<i32>(jo::self<jo::ScriptCheckCall>(this)->func_id)) & 0xffffff00u) | 1u;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_13", name, addr, fn)
B("script_6540a0", 0x006540a0, &script_6540a0);
B("script_6540e0", 0x006540e0, &script_6540e0);
B("script_654120", 0x00654120, &script_654120);
B("script_place_654f10", 0x00654f10, &script_place_654f10);
B("script_659550", 0x00659550, &script_659550);
B("script_unit_65ca40", 0x0065ca40, &script_unit_65ca40);
B("script_unit_65cb20", 0x0065cb20, &script_unit_65cb20);
B("script_unit_65cfd0", 0x0065cfd0, &script_unit_65cfd0);
B("script_65fcc0", 0x0065fcc0, &script_65fcc0);
B("ScriptNode::check_cmd", 0x00660e60, &H::check_cmd);
#undef B

}  // namespace game_b102_13
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kObjects = obj(d::g_units);
void* const kUnits = obj(d::g_items);
void* const kTypes = obj(d::g_nations);

JOAN_COUNTER(c_fn_655010, "Script::fn_655010");
JOAN_COUNTER(c_mount, "Script::mount_65bce0");

// 0x00655010 Script: (a, b, flag) -> 1 when both objects resolve.
i32 JOAN_CDECL fn_655010(const char* a, const char* b, char flag) {
    c_fn_655010.hit();
    const u32 pa = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(a));  // 0x004b91a0
    const u32 pb = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(b));
    if (pa == 0 || pb == 0) return 0;
    const i32 ia = joan::orig::arg::as<i32>(joan::orig::as_list(kTypes)->find_by_word2(joan::orig::arg::i32(pa)));  // 0x004b67e0
    const i32 ib = joan::orig::arg::as<i32>(joan::orig::as_list(kTypes)->find_by_word2(joan::orig::arg::i32(pb)));
    if (ia == 0 || ib == 0) return 0;
    void* const t = joan::orig::as_dynarray(kTypes)->ptr_at(joan::orig::arg::i32(ia));  // 0x0047b600
    joan::orig::as_nation(t)->set_relation_state(joan::orig::arg::i32(static_cast<i16>(ib)), joan::orig::arg::i32(flag), static_cast<i32>(0));  // 0x00499770
    return 1;
}

// 0x0065aff0
i32 JOAN_CDECL fn_65aff0(char a, char b) {
    const i8 r = joan::orig::arg::as<i8>(joan::orig::as_cutscene(obj(d::g_game_a5d380))->start_6cc6a0(joan::orig::arg::u32(global<u32>(d::g_now)), 0xc3000000u, joan::orig::arg::u32(a), joan::orig::arg::u32(b)));  // 0x006cc6a0
    return static_cast<i32>(r);
}

// 0x0065bce0 Script: put unit b's carrier into object a's slot.
i32 JOAN_CDECL mount_65bce0(i16 a, i16 b, char c) {
    c_mount.hit();
    if (joan::orig::as_dynarray(kObjects)->empty_at(static_cast<i32>(a)) != 0) return 0;  // 0x00428b70
    if (joan::orig::as_units(kUnits)->list.empty_at(static_cast<i32>(b)) != 0) return 0;  // 0x00425d50
    u8* const p4 = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kObjects)->ptr_at(static_cast<i32>(a)));  // 0x0041b3c0
    u8* const p8 = joan::orig::arg::as<u8*>(joan::orig::as_units(kUnits)->list.ptr_at(static_cast<i32>(b)));  // 0x00425d20
    const u32 id = joan::orig::arg::as<u32>(joan::orig::as_units(kUnits)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(jo::as_map_object(p8)->type_id))));  // 0x0049ea90
    u8* const p10 = joan::orig::arg::as<u8*>(joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(id)));
    {
        using Fn = u32 (Obj::*)();
        JOAN_MEMBER_POINTER_CHECK(Fn);
        Fn method{};
        std::memcpy(&method, &joan::call::vtable_of(p10)[0x1c / 4], sizeof(void*));
        (reinterpret_cast<Obj*>(p10)->*method)();
    }
    if (jo::as_map_object(p8)->actor != nullptr) {
        jo::view<u32>(jo::as_map_object(p10)->actor->kind) = jo::view<u32>(jo::as_map_object(p8)->actor->kind);
        joan::en3d::unit_index_actor_kind(jo::as_map_object(p10)->actor);  // [Debug] UnitIndex
    }
    jo::view<u8*>(jo::as_map_object(p10)->actor->owner) = p10;
    u8* const bone = jo::view<u8*>(jo::as_map_object(p8)->actor->bone);
    Vec3 pos{0.0f, 0.0f, 0.0f};
    joan::orig::matrix_position(&pos, joan::orig::arg::ptr(jo::as_bone(bone)->matrix_));  // 0x0060c310
    float ra = 0.0f, rb = 0.0f, rc = 0.0f;
    joan::orig::matrix_to_euler(joan::orig::arg::mptr(jo::as_bone(bone)->matrix_), &ra, &rb, &rc);  // 0x0060c430
    joan::orig::as_map_object(p10)->set_world(jo::as_unit(p4)->world);  // 0x004994b0
    joan::orig::as_map_object(p10)->set_position(pos);                 // 0x0041ff50
    joan::orig::as_scene_object(jo::view<void*>(jo::as_map_object(p10)->actor))->set_angles(joan::orig::arg::bits(ra), joan::orig::arg::bits(rb), joan::orig::arg::bits(rc));  // 0x005acaf0
    joan::orig::as_bone(jo::view<void*>(jo::as_map_object(p10)->actor->bone))->update();  // 0x0060ee60
    joan::orig::as_cell_object(p4)->attach_unit(joan::orig::arg::i16(static_cast<i32>(0)), static_cast<i32>(0), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::mptr(static_cast<i32>(0)));  // 0x0050a480
    return 1;
}

// 0x0065d070
i32 JOAN_CDECL fn_65d070(i32 id, u32 v) {
    if (joan::orig::as_dynarray(kObjects)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
    void* const o = joan::orig::as_dynarray(kObjects)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
    joan::orig::as_unit(o)->toggle_3b8(joan::orig::arg::i32(v));                                    // 0x005102a0
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Script::fn_655010", 0x00655010, &fn_655010);
B("Script::fn_65aff0", 0x0065aff0, &fn_65aff0);
B("Script::mount_65bce0", 0x0065bce0, &mount_65bce0);
B("Script::fn_65d070", 0x0065d070, &fn_65d070);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b103_13.cpp (game/b103_13)
namespace {
namespace game_b103_13 {

void* const kUnits = obj(d::g_units);
void* const kObjects = obj(d::g_items);

JOAN_COUNTER(c_detach, "Units::detach_656410");

// One plane reset of 0x00656410 (the four copies are identical; the local the colour goes
// through is per copy in the original but its value is consumed immediately).
void reset_plane(u8* unit, u8* plane) {
    jo::view<u32>(jo::as_actor(plane)->fade) = 0xbdcccccdu;  // -0.1f
    jo::as_actor(plane)->fade_38 = global<u32>(d::g_now);
    if (jo::as_unit(unit)->markers != -1) {
        jo::view<u32>(jo::as_actor(plane)->alpha) = 0xff;
    } else {
        u32 tmp = 0;
        const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_plane(jo::view<void*>(jo::as_actor(plane)->plane))->colour(&tmp));  // 0x005d5eb0
        jo::view<u32>(jo::as_actor(plane)->alpha) = colour >> 24;
    }
}
// The unit-plane copies store +0x30 then +0x38; the object-plane copies store +0x38 then
// +0x30 - both orders kept.
void reset_object_plane(u8* unit, u8* plane) {
    jo::as_actor(plane)->fade_38 = global<u32>(d::g_now);
    jo::view<u32>(jo::as_actor(plane)->fade) = 0xbdcccccdu;
    if (jo::as_unit(unit)->markers != -1) {
        jo::view<u32>(jo::as_actor(plane)->alpha) = 0xff;
    } else {
        u32 tmp = 0;
        const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_plane(jo::view<void*>(jo::as_actor(plane)->plane))->colour(&tmp));  // 0x005d5eb0
        jo::view<u32>(jo::as_actor(plane)->alpha) = colour >> 24;
    }
}
void reset_object(u8* unit, i32 object_id) {
    if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(object_id)) != 0) return;  // 0x00425d50
    u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(object_id)));  // 0x00425d20
    if (jo::view<u8*>(jo::as_map_object(o)->actor) == nullptr) return;
    if (static_cast<i32>(jo::as_unit(unit)->layer) == 0) return;
    reset_object_plane(unit, jo::view<u8*>(jo::as_map_object(o)->actor));
}

// 0x00656410 (cdecl id)
i32 JOAN_CDECL detach_656410(i32 id) {
    c_detach.hit();
    if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
    u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
    if (jo::view<u8*>(jo::as_unit(unit)->actor) != nullptr) {
        jo::as_actor(jo::view<u8*>(jo::as_unit(unit)->actor))->aim_mode = 0;
        if (static_cast<i32>(jo::as_unit(unit)->layer) != 0) {
            reset_plane(unit, jo::view<u8*>(jo::as_unit(unit)->actor));
            if (jo::view<u8*>(jo::as_unit(unit)->model) != nullptr) reset_plane(unit, jo::view<u8*>(jo::as_unit(unit)->model));
        }
    }
    reset_object(unit, static_cast<i32>(jo::as_unit(unit)->carried_a));
    reset_object(unit, static_cast<i32>(jo::as_unit(unit)->carried_b));
    if (jo::as_unit(unit)->draw != nullptr) {
        void* const p = jo::as_unit(unit)->draw;
        if (p != nullptr) joan::orig::as_unit_draw(p)->destroy(1u);  // 0x00416640
        jo::as_unit(unit)->draw = nullptr;
    }
    if (jo::view<u8*>(jo::as_unit(unit)->model) != nullptr) {
        u8* const p = jo::view<u8*>(jo::as_unit(unit)->model);
        if (jo::self<jo::SceneObject>(p)->scene != nullptr) joan::orig::as_scene(jo::self<jo::SceneObject>(p)->scene)->detach(joan::orig::arg::mptr(p));  // 0x005efdb0
        void* const q = jo::as_unit(unit)->model;
        if (q != nullptr) joan::orig::as_scene_object(q)->destroy(1u);  // 0x00420840
        jo::as_unit(unit)->model = nullptr;
    }
    if (jo::as_unit(unit)->actor != nullptr) joan::orig::as_actor(jo::as_unit(unit)->actor)->set_byte_fd(static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x005ad000
    joan::orig::as_unit(unit)->set_value_135(0);                                                         // 0x004fe510
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_13", name, addr, fn)
B("Units::detach_656410", 0x00656410, &detach_656410);
#undef B

}  // namespace game_b103_13
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
namespace {
namespace game_b90_06 {

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

constexpr img::Addr kUnitTable(d::g_units);
constexpr img::Addr kUnitList(d::g_items);
constexpr img::Addr kUnitTypes(d::g_types);

// 0x00428b70: 1 when the id is NOT in the table.
std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kUnitTable))->empty_at(joan::orig::arg::i32(id)); }
void* unit_get(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kUnitTable))->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
void* unit_type(std::int16_t t) { return joan::orig::arg::as<void*>(joan::orig::as_records_203(img::obj(kUnitTypes))->get(joan::orig::arg::i16(t))); }  // 0x0042d0a0

JOAN_COUNTER(c_guard, "Unit::guard_check");
JOAN_COUNTER(c_rank, "Unit::rank_value");
JOAN_COUNTER(c_range, "Unit::range_for_level");
JOAN_COUNTER(c_dist, "Unit::distance_to");

// 0x00656bb0: checks the unit's guard slots matching the kind letter ('M' 1, 'F' 2):
// -1 unknown unit, 0 not guarded / stale slot (cleared), 1 all matching slots valid.
std::int32_t JOAN_CDECL guard_check(std::int16_t id, const char* kind) {
    c_guard.hit();
    if (unit_missing(id) != 0) return -1;
    void* const u = unit_get(id);
    void* const type = unit_type(jo::as_unit(u)->type_id);
    std::int8_t mask = 0;
    if (static_cast<std::int8_t>(*kind) == 0x4d)
        mask = 1;
    else if (static_cast<std::int8_t>(*kind) == 0x46)
        mask = 2;
    if (jo::as_unit(u)->crew_slots_ready == 0) return 0;
    if (jo::self<jo::UnitType>(type)->slot_count <= 0) return 0;
    for (std::int32_t i = jo::self<jo::UnitType>(type)->slot_count - 1; i >= 0; --i) {
        const std::int8_t flags = jo::view<std::int8_t*>(jo::as_unit_type(type)->buffers[2])[i];
        if ((static_cast<std::int32_t>(flags) & static_cast<std::int32_t>(mask)) == 0) continue;
        std::int32_t* const ids = jo::view<std::int32_t*>(jo::as_unit(u)->mask_slots);
        if (ids[i] <= 0) return 0;
        if (unit_missing(ids[i]) != 0) {
            jo::view<std::int32_t*>(jo::as_unit(u)->mask_slots)[i] = 0;
            jo::view<std::int32_t*>(jo::as_unit(u)->mask_slots_b)[i] = 0;
            return 0;
        }
        void* const other = unit_get(jo::view<std::int32_t*>(jo::as_unit(u)->mask_slots)[i]);
        if (jo::as_unit(other)->weapon != static_cast<std::int32_t>(id)) {
            jo::view<std::int32_t*>(jo::as_unit(u)->mask_slots)[i] = 0;
            jo::view<std::int32_t*>(jo::as_unit(u)->mask_slots_b)[i] = 0;
            return 0;
        }
    }
    return 1;
}

// 0x006592c0: 200 + 100 * rank band of the unit's experience (record +0x33: +0x37 kind,
// +2 level).
std::int32_t JOAN_CDECL rank_value(std::int16_t id) {
    c_rank.hit();
    if (id == 0) return 0;
    if (unit_missing(id) != 0) return 0;
    void* const u = unit_get(id);
    if (jo::as_unit(u)->group == nullptr) return 0xc8;
    std::int16_t band = 0;
    void* const r = jo::as_unit(u)->group;
    if (jo::as_group(r)->formation == 1) {
        const std::int32_t level = jo::as_group(r)->count;
        if (level <= 1)
            band = 1;
        else if (level <= 4)
            band = 2;
        else if (level <= 9)
            band = 3;
        else if (level <= 0x10)
            band = 4;
        else if (level <= 0x19)
            band = 5;
    }
    return static_cast<std::int32_t>(band) * 0x64 + 0xc8;
}

// 0x0065b280: range by unit class (type +2, 1..8) and level 1..3 (4 for class 8).
std::int32_t JOAN_CDECL range_for_level(std::int32_t id, std::int32_t level) {
    c_range.hit();
    if (unit_missing(id) != 0) return 0;
    void* const u = unit_get(id);
    const std::int32_t cls = jo::self<jo::UnitType>(unit_type(jo::as_unit(u)->type_id))->unit_class;
    const std::uint32_t k = static_cast<std::uint32_t>(cls - 1);
    if (k > 7) return 0;
    switch (k) {
        case 0:
            if (level == 1) return 0x320;
            if (level == 2) return 0x640;
            if (level == 3) return 0x9c4;
            return 0;
        case 1:
            if (level == 1) return 0x258;
            if (level == 2) return 0x4b0;
            if (level == 3) return 0x9c4;
            return 0;
        case 2:
            if (level == 1) return 0x258;
            if (level == 2) return 0x320;
            if (level == 3) return 0x9c4;
            return 0;
        case 3:
            if (level == 1) return 0x3e8;
            if (level == 2) return 0x7d0;
            if (level == 3) return 0x9c4;
            return 0;
        case 4:
            if (level == 1) return 0xfa;
            if (level == 2) return 0x4b0;
            if (level == 3) return 0x640;
            return 0;
        case 5:
            if (level == 1) return 0xfa;
            if (level == 2) return 0x578;
            if (level == 3) return 0x708;
            return 0;
        case 7:
            if (level == 1) return 0x190;
            if (level == 2) return 0x640;
            if (level == 3) return 0x640;
            if (level == 4) return 0xc80;
            return 0;
        default:  // case 6
            return 0;
    }
}

// 0x0065b6a0: unit list entry has kind (+0x4e) 10.
std::int32_t JOAN_CDECL is_kind10(std::int32_t id) {
    if (joan::orig::as_units(img::obj(kUnitList))->list.empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00425d50
    void* const e = joan::orig::as_units(img::obj(kUnitList))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
    return jo::as_map_object(e)->class_id == 0xa ? 1 : 0;
}

// 0x0065b800: truncated distance from the unit to (x, y, z); -1 unknown unit.
std::int32_t JOAN_CDECL distance_to(std::int16_t id, std::int32_t x, std::int32_t y, std::int32_t z) {
    c_dist.hit();
    if (unit_missing(id) != 0) return -1;
    void* const u = unit_get(id);
    V3 pos;
    joan::orig::as_unit(u)->position(&pos);  // 0x004fedb0 position(out)
    V3 p;
    p.x = static_cast<float>(x);
    p.y = static_cast<float>(y);
    p.z = static_cast<float>(z);
    V3 tmp;
    const V3* const d = joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &p, &pos));  // 0x0060cc90
    V3 diff = *d;
    return joan::hook::x87::ftol(import_call<double>(d::iat_v3d_87_length, static_cast<void*>(&diff)));  // length
}

// 0x0065c8d0: 0 unknown unit, else the unit's idle flag (0x0042d0e0, sign-extended).
std::int32_t JOAN_CDECL unit_idle(std::int32_t id) {
    if (unit_missing(id) != 0) return 0;
    void* const u = unit_get(id);
    return static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(u)->dead()));  // 0x0042d0e0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Unit::guard_check", 0x00656bb0, &guard_check);
B("Unit::rank_value", 0x006592c0, &rank_value);
B("Unit::range_for_level", 0x0065b280, &range_for_level);
B("Unit::is_kind10", 0x0065b6a0, &is_kind10);
B("Unit::distance_to", 0x0065b800, &distance_to);
B("Unit::idle", 0x0065c8d0, &unit_idle);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
namespace {
namespace game_b103_14 {

struct Vec3 {
    float x, y, z;
};

void* const kUnitTable = obj(d::g_units);
void* const kTypes = obj(d::g_types);
void* const kObjects = obj(d::g_items);

JOAN_COUNTER(c_take_slot, "script_unit_take_slot");
JOAN_COUNTER(c_bind_input, "script_bind_input");
JOAN_COUNTER(c_set_6b8260, "script_set_6b8260");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* type_of(const void* unit) {  // word +0x3b (callee reads the low word)
    return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
}

// ------------------------------------------------------------------ 0x00657670
i32 JOAN_CDECL script_unit_take_slot(i32 a_id, i32 b_id, const char* mode, i32 radius) {
    c_take_slot.hit();
    if (unit_missing(static_cast<i16>(a_id)) != 0) return -1;
    if (unit_missing(static_cast<i16>(b_id)) != 0) return -1;
    u8* const a = unit_get(static_cast<i16>(a_id));
    u8* const b = unit_get(static_cast<i16>(b_id));
    i8 m;
    if (static_cast<i32>(static_cast<i8>(*mode)) == 0x4d) {
        m = 1;
    } else if (static_cast<i32>(static_cast<i8>(*mode)) == 0x46) {
        m = 2;
    } else {
        return -1;
    }
    if (static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_unit(a)->is_kind_special())) == 0) return 0;  // 0x00508ab0
    Vec3 pos;
    joan::orig::as_unit(a)->position(&pos);  // 0x004fedb0
    u8* const type = type_of(b);
    for (i32 i = 0; i < jo::as_unit_type(type)->slot_count; ++i) {
        if ((static_cast<i32>(jo::view<i8*>(jo::as_unit_type(type)->buffers[2])[i]) & static_cast<i32>(m)) == 0) continue;
        if (jo::as_unit(b)->mask_slots[i].value > 0) continue;
        const Vec3 pt = jo::view<Vec3*>(jo::as_unit_type(type)->buffers[0])[i];
        void* matrix;
        float local_matrix[16];
        if (jo::as_unit(b)->actor != nullptr) {
            matrix = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_actor(jo::as_unit(b)->actor)->bone))->matrix());  // 0x0060ef80
        } else {
            joan::orig::matrix_rotation(joan::orig::arg::mptr(static_cast<void*>(local_matrix)), joan::orig::arg::f32(jo::as_unit(b)->heading));  // 0x0060bc50
            import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, static_cast<void*>(local_matrix), static_cast<void*>(jo::as_unit(b)->position_));
            matrix = local_matrix;
        }
        Vec3 t;
        import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&t), static_cast<const void*>(&pt), matrix);
        const float dx = pos.x - t.x;
        const float dx2 = dx * dx;
        const float dy = pos.y - t.y;
        const float dy2 = dy * dy;
        const float sum = dx2 + dy2;
        const i32 r2 = static_cast<i32>(static_cast<u32>(radius) * static_cast<u32>(radius));
        if (!(static_cast<double>(r2) >= static_cast<double>(sum))) continue;  // C0: r2 < sum or NaN

        u32 angle_a, angle_b, angle_c, diff, tmp;
        joan::orig::as_angle(&angle_a)->construct_577190();  // 0x00577190
        joan::orig::as_angle(&angle_a)->set_wrapped(joan::orig::arg::f32(heading(a)));  // 0x00577340
        joan::orig::as_angle(&angle_b)->construct_577190();
        const float hb = heading(b);
        const float off = jo::view<float*>(jo::as_unit_type(type)->buffers[1])[i] * global<float>(d::flt_0_017453292);
        joan::orig::as_angle(&angle_b)->set_wrapped(joan::orig::arg::f32(hb + off));
        diff = *joan::orig::arg::as<u32*>(joan::orig::as_angle(&angle_a)->distance(&tmp, &angle_b));  // 0x00577920
        joan::orig::as_angle(&angle_c)->construct_577190();
        joan::orig::as_angle(&angle_c)->set_degrees(2.0f);  // 0x005773e0 (push 0x40000000)
        if (joan::orig::arg::as<u8>(joan::orig::as_angle(&diff)->gt(&angle_c)) != 0) {  // 0x00577770
            const float h = heading(b) / global<float>(d::flt_0_017453292);
            joan::orig::as_unit(a)->set_heading(joan::orig::arg::f32(h + jo::view<float*>(jo::as_unit_type(type)->buffers[1])[i]));  // 0x0050abd0
            continue;
        }
        jo::as_unit(b)->mask_slots[i].value = static_cast<i32>(jo::as_unit(a)->id);
        jo::as_unit(b)->mask_slots_b[i].value = 0;
        jo::as_unit(a)->weapon = static_cast<i32>(jo::as_unit(b)->id);
        jo::as_unit(a)->weapon_slot = i;
        {
            const float h = heading(b) / global<float>(d::flt_0_017453292);
            joan::orig::as_unit(a)->set_heading(joan::orig::arg::f32(h + jo::view<float*>(jo::as_unit_type(type)->buffers[1])[i]));  // 0x0050abd0
        }
        u32 object = 0;
        if (static_cast<i32>(jo::view<i16*>(jo::as_unit_type(type)->buffers[3])[i]) != 0) {
            object = joan::orig::arg::as<u32>(joan::orig::as_units(kObjects)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(jo::view<i16*>(jo::as_unit_type(type)->buffers[3])[i]))));  // 0x0049ea90
        }
        joan::orig::as_cell_object(a)->attach_unit(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(object))), 2, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
        const u32 kind = static_cast<u32>(static_cast<i32>(jo::self<jo::UnitType>(type_of(b))->kind) - 4);
        switch (kind) {
            case 0: jo::as_unit(a)->anim_special = 5; break;
            case 1: jo::as_unit(a)->anim_special = 6; break;
            case 2: jo::as_unit(a)->anim_special = 8; break;
            case 4: jo::as_unit(a)->anim_special = 9; break;
            case 5: jo::as_unit(a)->anim_special = 7; break;
            case 6:
                if (i == 0) {
                    jo::as_unit(a)->anim_special = 0xa;
                } else if (i == 1) {
                    jo::as_unit(a)->anim_special = 0xb;
                } else if (i == 2) {
                    jo::as_unit(a)->anim_special = 0xc;
                }
                break;
            case 7:
                if (static_cast<i32>(m) == 1) {
                    jo::as_unit(a)->anim_special = 0xd;
                } else if (static_cast<i32>(m) == 2) {
                    if (!(pt.y >= global<float>(d::k_9cf5c4))) {  // y < 0 or NaN
                        jo::as_unit(a)->anim_special = 0xf;
                    } else {
                        jo::as_unit(a)->anim_special = 0xe;
                    }
                }
                break;
            default: break;
        }
        joan::orig::as_unit(a)->set_action(static_cast<i32>(jo::as_unit(a)->anim_kind), 0, 1);  // 0x00501190
        return 1;
    }
    return 0;
}

// ------------------------------------------------------------------ 0x0065cac0
u32 JOAN_CDECL script_unit_call_510170(i32 arg, i32 id) {
    if (unit_missing(id) != 0) return 0;
    u8* const u = unit_get(id);
    return joan::orig::arg::as<u32>(joan::orig::as_unit(u)->count_modifiers_of_kind(joan::orig::arg::u32(arg)));  // 0x00510170
}

// ------------------------------------------------------------------ 0x0065e0a0
struct Binding {
    img::Addr name;
    i32 code;
};
const Binding kBindings[] = {
    {d::s_A_ATTACK, 0x0d},  // A_ATTACK
    {d::s_B_ATTACK, 0x0e},  // B_ATTACK
    {d::s_DEFEND, 0x0c},  // DEFEND
    {d::s_ARROW_SHOOTING, 0x12},  // ARROW_SHOOTING
    {d::s_INTERACT, 0x11},  // INTERACT
    {d::s_RIDE, 0x11},  // RIDE
    {d::s_CHARACTER_INTERFACE, 0x23},  // CHARACTER_INTERFACE
    {d::s_DOWN_ATTACK, 0x0c},  // DOWN_ATTACK
    {d::s_USE_HEAL_ITEM, 0x13},  // USE_HEAL_ITEM
    {d::s_COMBO_INTERFACE, 0x24},  // COMBO_INTERFACE
    {d::s_SWITCH_CHARACTER, 0x18},  // SWITCH_CHARACTER
    {d::s_FORWARD, 0x04},  // FORWARD
    {d::s_BACKWARD, 0x05},  // BACKWARD
    {d::s_TURN_LEFT, 0x08},  // TURN_LEFT
    {d::s_TURN_RIGHT, 0x09},  // TURN_RIGHT
    {d::s_SHIFT_LEFT, 0x06},  // SHIFT_LEFT
    {d::s_SHIFT_RIGHT, 0x07},  // SHIFT_RIGHT
    {d::s_SHIFT, 0x03},  // SHIFT
    {d::s_SHIFT_CAMERA, 0x02},  // SHIFT_CAMERA
    {d::s_ZOOM_IN, 0x30},  // ZOOM_IN
    {d::s_ZOOM_OUT, 0x31},  // ZOOM_OUT
    {d::s_ENERGY_ATTACK, 0x0f},  // ENERGY_ATTACK
    {d::s_SWITCH_JOAN, 0x19},  // SWITCH_JOAN
    {d::s_SWITCH_JEAN, 0x1a},  // SWITCH_JEAN
    {d::s_SWITCH_LA_HIRE, 0x1b},  // SWITCH_LA_HIRE
    {d::s_GAME_OBJECTIVE_KEY, 0x28},  // GAME_OBJECTIVE_KEY
    {d::s_RTS_MODE, 0x17},  // RTS_MODE
};
std::uintptr_t JOAN_CDECL script_bind_input(void* target, const char* name) {
    c_bind_input.hit();
    u32 result = 0;
    for (const Binding& b : kBindings) {
        const i32 r = stricmp_(name, str(b.name));
        result = static_cast<u32>(r);
        if (r == 0) {
            const char* const key = joan::orig::as_table(obj(d::g_keys_a48820))->lookup(joan::orig::arg::i32(global<i32>(d::g_control_mode)), joan::orig::arg::i32(b.code));  // 0x004cb5b0 (the key name)
            return joan::orig::arg::as<u32>(joan::orig::as_string(target)->assign(joan::orig::arg::str(key)));       // 0x005a1440
        }
    }
    return result;  // last _stricmp result (non-zero)
}

// ------------------------------------------------------------------ 0x0065ebd0
i32 JOAN_CDECL script_set_6b8260(const char* value) {
    c_set_6b8260.hit();
    joan::orig::as_message_list(obj(d::g_names))->reset();  // 0x004ae6c0
    void* const sys = obj(d::g_game_over_screen);
    if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_screen(sys)->flag_9ec())) == 0) {  // 0x004af410
        joan::orig::as_screen(sys)->close_6b8af0();                         // 0x006b8af0
    }
    joan::orig::as_game_over(sys)->set_text(value);  // 0x006b8260
    if (unit_missing(global<i32>(d::g_player_unit_recno)) == 0) {
        if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(unit_get(global<i32>(d::g_player_unit_recno)))->dead())) == 0) {  // 0x0042d0e0
            joan::orig::as_unit(unit_get(global<i32>(d::g_player_unit_recno)))->play_idle(0, 1);                    // 0x00508b90
        }
    }
    return 1;
}

struct H {
    // ScriptNode::value of an integer node: the integer lives in the first 4 bytes of the pointer-typed slot
    // (x86: the whole slot; x64: the low half, set_value / 0x0063f140 store it zero-extended).
    static u32 node_int(void* node) {
        u32 r;
        std::memcpy(&r, &jo::as_script_node(node)->value, sizeof r);
        return r;
    }

    // eax residue of 0x006612e0 (callers test only al): see joan::call::register_low_bits.
    u32 self_bits() { return joan::call::register_low_bits(this); }

    // ------------------------------------------------------------------ 0x006612e0
    u32 check_143(i32) {
        if (static_cast<i32>(jo::self<jo::ScriptCall>(this)->func_id) != 0x143) return (self_bits() & 0xffffff00u) | 1u;
        const i32 w = static_cast<i32>(jo::self<jo::ScriptCall>(this)->arg_id);
        void* const e = joan::orig::arg::as<void*>(joan::orig::as_reader(this)->list_get(1));       // 0x0063e9f0
        return joan::orig::arg::as<u32>(joan::orig::script_unit_list_remove(joan::orig::arg::i32(node_int(e)), joan::orig::arg::i32(w)));   // 0x00661ca0
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("script_unit_take_slot", 0x00657670, &script_unit_take_slot);
B103("script_unit_call_510170", 0x0065cac0, &script_unit_call_510170);
B103("script_bind_input", 0x0065e0a0, &script_bind_input);
B103("script_set_6b8260", 0x0065ebd0, &script_set_6b8260);
B103("Obj::check_143", 0x006612e0, &H::check_143);
#undef B103

}  // namespace game_b103_14
}  // namespace

// ---------------------------------------- from modules/game_b106_18.cpp (game/b106_18)
namespace {
namespace game_b106_18 {

JOAN_COUNTER(c_release, "script_release_group");

void* const kUnits = obj(d::g_units);

// ---------------------------------------------------------------- 0x00659080
i32 JOAN_CDECL script_release_group(i32 id) {
    c_release.hit();
    const i32 v = joan::orig::arg::as<i32>(joan::orig::find_unit_in_groups(joan::orig::arg::i16(static_cast<i16>(id))));  // 0x006581f0
    if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(v)) == 0) {  // 0x00428b70
        if (jo::view<u8*>(jo::as_unit(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(v)))->group) != nullptr) {  // 0x0041b3c0
            for (i32 i = jo::as_group(jo::view<u8*>(jo::as_unit(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(v)))->group))->count; i > 0; --i) {
                const i32 m = jo::as_group(jo::view<u8*>(jo::as_unit(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(v)))->group))->members[i - 1];
                if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(m)) != 0) continue;
                const i16 mw = jo::as_group(jo::view<u8*>(jo::as_unit(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(v)))->group))->members[i - 1];
                joan::orig::unit_reset_655420(joan::orig::arg::u32(mw));  // 0x00655420
            }
        }
        joan::orig::unit_reset_655420(joan::orig::arg::u32(static_cast<i16>(v)));  // 0x00655420
    } else {
        for (i32 i = joan::orig::as_dynarray(kUnits)->size(); i > 0; --i) {  // 0x00417ad0
            void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (u == nullptr) continue;
            if (static_cast<i32>(jo::as_unit(u)->group_id) != id) continue;
            if (jo::as_unit(u)->rank >= 4) joan::orig::as_unit(u)->clear_markers();  // 0x0050b390
            joan::orig::unit_reset_655420(joan::orig::arg::u32(static_cast<i16>(i)));            // 0x00655420
        }
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b106_18", name, addr, fn)
B("script_release_group", 0x00659080, &script_release_group);
#undef B

}  // namespace game_b106_18
}  // namespace

// ---------------------------------------- from modules/game_b106_10.cpp (game/b106_10)
namespace {
namespace game_b106_10 {

JOAN_COUNTER(c_release_owned, "Units::release_owned_659210");

constexpr img::Addr kUnits = d::g_units;

// 0x00659210
u32 JOAN_CDECL release_owned(i32 owner) {
    c_release_owned.hit();
    for (i32 i = joan::orig::as_dynarray(obj(kUnits))->size(); i > 0; --i) {  // 0x00417ad0
        u8* u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
        if (u == nullptr) continue;
        if (static_cast<i32>(jo::as_unit(u)->group_id) != owner) continue;
        if (joan::orig::arg::as<u32>(joan::orig::as_unit(u)->leads()) != 0) continue;  // 0x0044a660
        if (jo::as_unit(u)->rank >= 4) joan::orig::as_unit(u)->clear_markers();  // 0x0050b390
        joan::orig::unit_reset_655420(joan::orig::arg::u32(static_cast<i16>(i)));           // 0x00655420
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b106_10", name, addr, fn)
B("Units::release_owned", 0x00659210, &release_owned);
#undef B

}  // namespace game_b106_10
}  // namespace

// ---------------------------------------- from modules/game_b111_13.cpp (game/b111_13)
namespace {
namespace game_b111_13 {

// ---------------------------------------------------------------------------- 0x00659790
double JOAN_CDECL minutes_mod() {
    const double x = static_cast<double>(global<float>(d::g_calendar_minute_of_day));
    return joan::orig::arg::as<double>(joan::orig::libc::fmod(joan::orig::arg::f64(x), joan::orig::arg::f64(60.0)));  // fmod (direct call), exact result
}

#define B111(name, addr, fn) JOAN_REPLACE("game/b111_13", name, addr, fn)
B111("Clock::minutes_mod", 0x00659790, &minutes_mod);
#undef B111

}  // namespace game_b111_13
}  // namespace

// ---------------------------------------- from modules/game_b99_09.cpp (game/b99_09)
namespace {
namespace game_b99_09 {


struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

void* const kSoldiers = obj(d::g_units);

JOAN_COUNTER(c_set_374, "Script::soldier_set_374");
JOAN_COUNTER(c_near_target, "Script::soldier_near_target");
JOAN_COUNTER(c_object_height, "Script::object_height");
JOAN_COUNTER(c_flush_a50088, "Script::flush_a50088");
JOAN_COUNTER(c_set_side, "Script::set_side");

i32 soldier_invalid(i32 id) { return joan::orig::as_dynarray(kSoldiers)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* soldier_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 strcmp_(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

// ------------------------------------------------------------------------- script (cdecl)
i32 JOAN_CDECL set_a59294(u32 v) {  // 0x0065a2a0
    global<u32>(d::g_get_a59294) = v;
    return 1;
}
i32 JOAN_CDECL set_a592a8_word(i32 idx, u16 v) {  // 0x0065a6e0
    const u32 off = static_cast<u32>(idx - 1) * 0x1au;
    *reinterpret_cast<u16*>(img::addr(d::g_a592a8 + off)) = v;
    return 1;
}
i32 JOAN_CDECL call_4720d0(u32 v) {  // 0x0065a8a0
    return static_cast<i8>(joan::orig::arg::as<char>(joan::orig::as_player(obj(d::g_game_state))->flag_bit_4720d0(joan::orig::arg::i32(v))));  // 0x004720d0
}
i32 JOAN_CDECL call_4724c0(u32 v) {  // 0x0065a9f0
    return static_cast<i8>(joan::orig::arg::as<char>(joan::orig::as_game(obj(d::g_game_state))->get_flag_1c1(joan::orig::arg::i32(v))));  // 0x004724c0
}
i32 JOAN_CDECL call_4cd570(const char* v) {  // 0x0065aad0
    const u32 t = joan::orig::arg::as<u32>(joan::orig::scenario_mode(v));      // 0x004cd790
    joan::orig::as_scenario(obj(d::g_mission))->set_1ae(joan::orig::arg::u32(t));     // 0x004cd570
    return 1;
}
u32 JOAN_CDECL soldier_51fdd0(const char* id) {  // 0x0065b240
    if (id == nullptr) return 0;
    return joan::orig::arg::as<u32>(joan::orig::as_list(kSoldiers)->find_name_51fdd0(id));  // 0x0051fdd0
}
i32 JOAN_CDECL soldier_set_374(i16 id, u32 v) {  // 0x0065b8c0
    c_set_374.hit();
    if (soldier_invalid(id) != 0) return 0;
    u8* s = soldier_get(id);
    jo::view<u32>(jo::as_unit(s)->getting_weapon) = v;
    jo::view<u16>(jo::as_unit(s)->anim_special) = v != 0 ? 4 : 0;
    return 1;
}
i32 JOAN_CDECL soldier_sum_1b0(i32 id) {  // 0x0065c130
    if (soldier_invalid(id) != 0) return -1;
    u8* s = soldier_get(id);
    return static_cast<i32>(jo::as_unit(s)->kills) + static_cast<i32>(jo::as_unit(s)->kills_high_rank);
}
i32 JOAN_CDECL call_761680(const char* a, u32 b, u32 c, u32 d) {  // 0x0065c420
    joan::orig::as_fade_title(obj(d::g_fade_title))->show(joan::orig::arg::i32(global<u32>(d::g_now)), a, joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d));  // 0x00761680
    return 1;
}
i32 JOAN_CDECL soldier_5229a0(i32 id, u32 v) {  // 0x0065c9e0
    if (soldier_invalid(id) != 0) return 0;
    u8* s = soldier_get(id);
    joan::orig::as_unit(s)->add_experience(joan::orig::arg::i32(v), static_cast<std::int8_t>(joan::orig::arg::i32(u32{0})), static_cast<std::int8_t>(joan::orig::arg::i32(u32{0})));  // 0x005229a0
    return 1;
}
i32 JOAN_CDECL soldier_near_target(i32 id) {  // 0x0065d560
    c_near_target.hit();
    if (soldier_invalid(id) != 0) return 0;
    u8* s = soldier_get(id);
    if (static_cast<i32>(jo::as_unit(s)->place_index) == 0) return 0;
    if (static_cast<i32>(jo::as_unit(s)->place_slot) == 0) return 0;
    void* const objs = obj(d::g_place_array);
    if (joan::orig::arg::as<i32>(joan::orig::as_list(objs)->is_missing_ca0(static_cast<i32>(jo::as_unit(s)->place_index))) != 0) return 0;  // 0x00456ca0
    u8* o = joan::orig::arg::as<u8*>(joan::orig::as_list(objs)->get_c70(static_cast<i32>(jo::as_unit(s)->place_index)));           // 0x00456c70
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_place(o)->slot_pos(static_cast<i32>(jo::as_unit(s)->place_slot)));              // 0x004c2470
    const float h = joan::orig::arg::as<float>(joan::orig::as_place(o)->slot_angle(static_cast<i32>(jo::as_unit(s)->place_slot)));    // 0x004c24b0
    const i32 k = jo::as_unit(s)->place_slot;
    if (k < 1) return 0;
    if (k > jo::as_place(o)->slot_count) return 0;
    V3 tmp;
    const V3* r = joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, static_cast<void*>(jo::as_unit(s)->position_), static_cast<void*>(p)));  // 0x0060cc90
    V3 d = *r;
    const double len = import_call<double>(d::iat_v3d_87_length, static_cast<void*>(&d));  // length
    if (len >= static_cast<double>(global<float>(d::flt_10))) return 0;  // test ah,1 je: a<b or NaN go on
    const float dy = jo::as_unit(s)->heading - h;
    const double a = joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(dy)));  // 0x00427d60
    if (a >= global<double>(d::dbl_0_1)) return 0;  // 0.1; a<b or NaN go on
    return 1;
}
i32 JOAN_CDECL call_6dfe10(char v) {  // 0x0065e810
    if (static_cast<i32>(global<i8>(d::g_a50520)) == 0)
        joan::orig::as_goal_panel(obj(d::g_goal_panel))->open_first(static_cast<std::int8_t>(joan::orig::arg::i32(v)));  // 0x006dfe10
    return 1;
}
i32 JOAN_CDECL call_6e00b0(char v) {  // 0x0065e8c0
    if (static_cast<i32>(global<i8>(d::g_a50521)) == 0)
        joan::orig::as_goal_panel(obj(d::g_goal_panel))->open_second(static_cast<std::int8_t>(joan::orig::arg::i32(v)));  // 0x006e00b0
    return 1;
}
float JOAN_CDECL object_height(u32 id) {  // 0x0065ead0
    c_object_height.hit();
    void* const objs = obj(d::g_items);
    if (joan::orig::as_units(objs)->list.empty_at(joan::orig::arg::i32(id)) != 0) return global<float>(d::k_9cf5c4);  // 0x00425d50
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_units(objs)->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
    if (jo::as_map_object(p)->world_item != nullptr) return jo::as_map_object(p)->world_item->pos[1];
    u8* q = joan::orig::arg::as<u8*>(joan::orig::as_units(objs)->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
    return jo::as_map_object(q)->pos[1];
}
u32 JOAN_CDECL call_71d1e0(const char* v) {  // 0x0065ecf0
    void* const g = obj(d::g_mini_map);
    if ((joan::orig::arg::as<u32>(joan::orig::as_marker(g)->byte_e()) & 0xff) == 0) return 0;                      // 0x0044cf30
    if (static_cast<i32>(static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_marker(g)->word_8()))) == 0) return 0;  // 0x0044cf10
    return joan::orig::arg::as<u32>(joan::orig::as_table(global<void*>(d::g_a4f385))->byte_by_name(v));  // 0x0071d1e0
}
i32 JOAN_CDECL set_a4f858(u8 v) {  // 0x0065ee30
    global<u8>(d::g_a4f858) = v;
    return 1;
}
i32 JOAN_CDECL flush_a50088() {  // 0x0065f6a0
    c_flush_a50088.hit();
    if (global<u8>(d::g_game_running) != 0) {
        global<u8>(d::g_game_running) = 0;
        global<u16>(d::g_saved_game_speed) =
            static_cast<u16>(joan::hook::x87::ftol(static_cast<double>(global<float>(d::g_game_speed))));
        joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::u32(u32{0}));  // 0x0042f930
    }
    if (static_cast<i32>(static_cast<i8>(joan::orig::arg::as<char>(joan::orig::as_screen(obj(d::g_game_over_screen))->flag_9ec()))) == 0) {  // 0x004af410
        joan::orig::as_victory_screen(obj(d::g_victory_screen))->open();  // 0x006e8c80
        global<u32>(d::g_get_a59171) = 0;
        global<u32>(d::g_get_a59175) = 0;
        global<u32>(d::g_get_a59179) = 0;
    }
    return 1;
}
i32 JOAN_CDECL set_side(const char* a, const char* name) {  // 0x0065f850
    c_set_side.hit();
    void* const tbl = obj(d::g_territory_array);
    const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_table(tbl)->find_by_name(a));                    // 0x004e4b20
    const u32 t = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(joan::orig::arg::str(name)));       // 0x004b91a0
    i32 r = joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(t)));                // 0x004b67e0
    if (r == 0 && strcmp_(name, str(d::s_Neutral)) == 0) r = -1;           // "Neutral"
    jo::self<jo::ZoneDef>(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(tbl)->get_420ff0(joan::orig::arg::i32(idx))))->owner = r;               // 0x00420ff0
    jo::view<u8>(jo::self<jo::ZoneDef>(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(tbl)->get_420ff0(joan::orig::arg::i32(idx))))->flag_46) = 0;
    return 1;
}
i32 JOAN_CDECL soldier_set_358(i32 id, u8 v) {  // 0x0065fd10
    if (soldier_invalid(id) != 0) return 0;
    jo::as_unit(soldier_get(id))->prisoner_attackable = v;
    return 1;
}

#define B99(name, addr_, fn) JOAN_REPLACE("game/b99_09", name, addr_, fn)
B99("Script::set_a59294", 0x0065a2a0, &set_a59294);
B99("Script::set_a592a8_word", 0x0065a6e0, &set_a592a8_word);
B99("Script::call_4720d0", 0x0065a8a0, &call_4720d0);
B99("Script::call_4724c0", 0x0065a9f0, &call_4724c0);
B99("Script::call_4cd570", 0x0065aad0, &call_4cd570);
B99("Script::soldier_51fdd0", 0x0065b240, &soldier_51fdd0);
B99("Script::soldier_set_374", 0x0065b8c0, &soldier_set_374);
B99("Script::soldier_sum_1b0", 0x0065c130, &soldier_sum_1b0);
B99("Script::call_761680", 0x0065c420, &call_761680);
B99("Script::soldier_5229a0", 0x0065c9e0, &soldier_5229a0);
B99("Script::soldier_near_target", 0x0065d560, &soldier_near_target);
B99("Script::call_6dfe10", 0x0065e810, &call_6dfe10);
B99("Script::call_6e00b0", 0x0065e8c0, &call_6e00b0);
B99("Script::object_height", 0x0065ead0, &object_height);
B99("Script::call_71d1e0", 0x0065ecf0, &call_71d1e0);
B99("Script::set_a4f858", 0x0065ee30, &set_a4f858);
B99("Script::flush_a50088", 0x0065f6a0, &flush_a50088);
B99("Script::set_side", 0x0065f850, &set_side);
B99("Script::soldier_set_358", 0x0065fd10, &soldier_set_358);
#undef B99

}  // namespace game_b99_09
}  // namespace

// ---------------------------------------- from modules/game_b100_14.cpp (game/b100_14)
namespace {
namespace game_b100_14 {

void* const kUnitTable = obj(d::g_units);
void* const kObjects = obj(d::g_items);
void* const kTypes = obj(d::g_types);
void* const kGroups = obj(d::g_nations);
void* const kList58fa7 = obj(d::g_mission);
void* const kSys4f381 = obj(d::g_mini_map);

JOAN_COUNTER(c_rect, "script_copy_rect_a58f97");
JOAN_COUNTER(c_type_activate, "script_type_activate");
JOAN_COUNTER(c_count_allies, "script_unit_count_allies");
JOAN_COUNTER(c_place_effect, "script_place_effect");
JOAN_COUNTER(c_object_height, "script_object_height");
JOAN_COUNTER(c_set_3c1, "script_unit_set_3c1");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* type_get(i16 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(w)))); }  // 0x0042d0a0

// ---------------------------------------------------------------------- 0x0065ab50
static i32 JOAN_CDECL script_list_a58fa7_call(const char* a) {
    void* const p = joan::orig::arg::as<void*>(joan::orig::as_scenario(kList58fa7)->current());  // 0x004cd840
    joan::orig::as_scenario(p)->set_name_4cdf00(a);                               // 0x004cdf00
    return 1;
}

// ---------------------------------------------------------------------- 0x0065ab90
static i32 JOAN_CDECL script_copy_rect_a58f97() {
    c_rect.hit();
    if (global<i32>(d::g_mission) != 0) {
        global<u32>(d::g_a58f97) = global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].u32_18a;
        global<u32>(d::g_a58f9b) = global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].u32_18e;
        global<u32>(d::g_a58f9f) = global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].u32_192;
        global<u32>(d::g_a58fa3) = global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].u32_196;
    } else {
        global<u32>(d::g_a58f97) = global<u32>(d::g_a59145);
        global<u32>(d::g_a58f9b) = global<u32>(d::g_a59149);
        global<u32>(d::g_a58f9f) = global<u32>(d::g_a5914d);
        global<u32>(d::g_a58fa3) = global<u32>(d::g_a59151);
    }
    return 1;
}

// ---------------------------------------------------------------------- 0x0065b190
static i32 JOAN_CDECL script_type_activate(const char* a) {
    c_type_activate.hit();
    const i32 v = joan::orig::arg::as<i32>(joan::orig::as_unit_types(kTypes)->find(a));  // 0x005270b0
    if (v != 0) {
        u8* const t = type_get(static_cast<i16>(v));  // 0x0042d0a0 (low word of v)
        joan::orig::as_panel(t)->open(0);                   // 0x00527520
        joan::orig::as_scenario(kList58fa7)->push_word_4cd8b0(joan::orig::arg::u32(v));          // 0x004cd8b0
    }
    return 1;
}

// ---------------------------------------------------------------------- 0x0065b500
static i32 JOAN_CDECL script_unit_135_pct(i32 id) {
    if (unit_missing(id) != 0) return 0;
    u8* const unit = unit_get(id);
    const i32 num = static_cast<i32>(static_cast<u32>(jo::as_unit(unit)->hit_points) * 100u);
    const i32 den = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->hit_points_max());  // 0x00523320
    return num / den;
}

// ---------------------------------------------------------------------- 0x0065b950
static i32 JOAN_CDECL script_unit_374(i16 id) {
    if (unit_missing(id) != 0) return 0;
    return jo::as_unit(unit_get(id))->getting_weapon;
}

// ---------------------------------------------------------------------- 0x0065c3a0
static i32 JOAN_CDECL script_unit_1c1_word2(i32 id) {
    if (unit_missing(id) != 0) return 0;
    if (joan::orig::arg::as<void*>(joan::orig::as_unit_data(jo::as_unit(unit_get(id))->modifiers)->slot_e()) == nullptr) return 0;  // 0x00497580
    void* const p = joan::orig::arg::as<void*>(joan::orig::as_unit_data(jo::as_unit(unit_get(id))->modifiers)->slot_e());          // 0x00497580
    return static_cast<i32>(jo::as_inv_item(p)->count);
}

// ---------------------------------------------------------------------- 0x0065c4a0
static i32 JOAN_CDECL script_set_a6a254(i32 a) {
    global<u32>(d::g_a6a254) = global<u32>(d::g_now) + static_cast<u32>(a);
    return 1;
}

// ---------------------------------------------------------------------- 0x0065c870
static i32 JOAN_CDECL script_unit_call_510030(i32 id, i32 a) {
    if (unit_missing(id) != 0) return 0;
    joan::orig::as_unit(unit_get(id))->set_10c(joan::orig::arg::i32(a));  // 0x00510030
    return 1;
}

// ---------------------------------------------------------------------- 0x0065cb80
static i32 JOAN_CDECL script_unit_count_allies(i32 id) {
    c_count_allies.hit();
    if (unit_missing(id) != 0) return 0;
    u8* const unit = unit_get(id);
    if (jo::as_unit(unit)->world == nullptr) return 0;
    i32 count = 0;
    for (i32 i = joan::orig::as_dynarray(kUnitTable)->size(); i > 0; --i) {  // 0x00417ad0
        u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
        if (o == nullptr) continue;
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(o)->dead()) != 0) continue;  // 0x0042d0e0
        if (jo::as_unit(o)->world != jo::as_unit(unit)->world) continue;
        u8* const t = type_get(jo::as_unit(o)->type_id);
        if (jo::self<jo::UnitType>(t)->kind != 0 && jo::self<jo::UnitType>(t)->kind != 1) continue;
        if (jo::self<jo::UnitType>(t)->unit_class == 0) continue;
        if (joan::orig::arg::as<i32>(joan::orig::as_players(kGroups)->hostile(static_cast<i32>(jo::as_unit(unit)->side), static_cast<i32>(jo::as_unit(o)->side))) != 0)  // 0x004b6860
            ++count;
    }
    return count;
}

// ---------------------------------------------------------------------- 0x0065d850
static i32 JOAN_CDECL script_unit_3c9(i32 id) {
    if (unit_missing(id) != 0) return 0;
    return jo::view<i32>(jo::as_unit(unit_get(id))->name_index);
}

// ---------------------------------------------------------------------- 0x0065d9c0
static i32 JOAN_CDECL script_place_effect(const char* a, i16 b, i16 c) {
    c_place_effect.hit();
    u32 colour_obj = 0;  // [ebp-0x8]; the ctor writes all 4 bytes
    const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_colour(static_cast<void*>(&colour_obj))->construct(static_cast<u8>(0xff), static_cast<u8>(0xff), static_cast<u8>(0xff), static_cast<u8>(0xff)));  // 0x0042a260
    joan::orig::as_text(obj(d::g_text_a5d140))->draw_line(joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::u32(static_cast<i32>(1)), a, joan::orig::arg::u32(colour), static_cast<i32>(-1), static_cast<i32>(0), static_cast<i32>(1), static_cast<i32>(2), static_cast<i32>(0), 1.0f, 1.0f);  // 0x006255d0
    return 1;
}

// ---------------------------------------------------------------------- 0x0065e880
static i32 JOAN_CDECL script_set_a50526(i32 a) {
    if (a > 0) {
        global<i32>(d::g_a50526) = a;
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------- 0x0065ea50
static float JOAN_CDECL script_object_height(i32 id) {
    c_object_height.hit();
    if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(id)) != 0) return global<float>(d::k_9cf5c4);  // 0x00425d50
    u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
    if (jo::as_map_object(o)->world_item != nullptr) return jo::as_map_object(o)->world_item->pos[0];
    return jo::as_map_object(joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id))))->pos[0];  // 0x00425d20
}

// ---------------------------------------------------------------------- 0x0065ec90
static i32 JOAN_CDECL script_a4f381_call(const char* a, i32 b) {
    if (joan::orig::arg::as<u8>(joan::orig::as_marker(kSys4f381)->byte_e()) == 0) return 0;   // 0x0044cf30
    if (joan::orig::arg::as<i16>(joan::orig::as_marker(kSys4f381)->word_8()) == 0) return 0;  // 0x0044cf10
    return joan::orig::arg::as<i32>(joan::orig::as_mini_res(global<void*>(d::g_a4f385))->set_flag_by_name(a, static_cast<std::int8_t>(joan::orig::arg::i32(b))));  // 0x0071d150
}

// ---------------------------------------------------------------------- 0x0065eda0
static i32 JOAN_CDECL script_set_word_a4f3ac(i16 v) {
    global<i16>(d::g_rotation) = v;
    return 1;
}

// ---------------------------------------------------------------------- 0x0065f670
static i32 JOAN_CDECL script_set_byte_a5ba64(u8 v) {
    global<u8>(d::g_a5ba64) = v;
    return 1;
}

// ---------------------------------------------------------------------- 0x0065f780
static i32 JOAN_CDECL script_a4f381_719770(i32 a, i32 b, i32 c, i32 d) {
    joan::orig::as_mini_map(kSys4f381)->set_reveal_circle(joan::orig::arg::i32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d));  // 0x00719770
    return 1;
}

// ---------------------------------------------------------------------- 0x0065f7c0
static i32 JOAN_CDECL script_a4f381_7197f0(u32 a, u32 b, u32 c, u32 d, u32 e, u32 g, u32 h, u32 i, u32 j) {
    joan::orig::as_mini_map(kSys4f381)->set_reveal_quad(joan::orig::arg::i32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d), joan::orig::arg::u32(e), joan::orig::arg::u32(g), joan::orig::arg::u32(h), joan::orig::arg::u32(i), joan::orig::arg::u32(j));  // 0x007197f0
    return 1;
}

// ---------------------------------------------------------------------- 0x0065f9e0
static i32 JOAN_CDECL script_unit_set_3c1(i32 id, i32 v) {
    c_set_3c1.hit();
    if (unit_missing(id) != 0) return 0;
    u8* const unit = unit_get(id);
    jo::view<i32>(jo::as_unit(unit)->vision_range) = v;
    if (jo::as_unit(unit)->group != nullptr) {
        for (i32 i = 0; i < static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->count); ++i) {
            if (unit_missing(static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->members[i])) != 0) continue;
            jo::view<i32>(jo::as_unit(unit_get(static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->members[i])))->vision_range) = v;
        }
    } else {
        for (i32 j = joan::orig::as_dynarray(kUnitTable)->size(); j > 0; --j) {  // 0x00417ad0
            if (unit_missing(j) != 0) continue;
            if (static_cast<i32>(jo::as_unit(unit_get(j))->group_id) == static_cast<i32>(jo::as_unit(unit)->group_id))
                jo::view<i32>(jo::as_unit(unit_get(j))->vision_range) = v;
        }
    }
    return 1;
}

// ---------------------------------------------------------------------- 0x0065fc90
static i32 JOAN_CDECL script_set_byte_a5d386(u8 v) {
    global<u8>(d::g_a5d386) = v;
    return 1;
}

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_14", name, addr, fn)
B100("script_list_a58fa7_call", 0x0065ab50, &script_list_a58fa7_call);
B100("script_copy_rect_a58f97", 0x0065ab90, &script_copy_rect_a58f97);
B100("script_type_activate", 0x0065b190, &script_type_activate);
B100("script_unit_135_pct", 0x0065b500, &script_unit_135_pct);
B100("script_unit_374", 0x0065b950, &script_unit_374);
B100("script_unit_1c1_word2", 0x0065c3a0, &script_unit_1c1_word2);
B100("script_set_a6a254", 0x0065c4a0, &script_set_a6a254);
B100("script_unit_call_510030", 0x0065c870, &script_unit_call_510030);
B100("script_unit_count_allies", 0x0065cb80, &script_unit_count_allies);
B100("script_unit_3c9", 0x0065d850, &script_unit_3c9);
B100("script_place_effect", 0x0065d9c0, &script_place_effect);
B100("script_set_a50526", 0x0065e880, &script_set_a50526);
B100("script_object_height", 0x0065ea50, &script_object_height);
B100("script_a4f381_call", 0x0065ec90, &script_a4f381_call);
B100("script_set_word_a4f3ac", 0x0065eda0, &script_set_word_a4f3ac);
B100("script_set_byte_a5ba64", 0x0065f670, &script_set_byte_a5ba64);
B100("script_a4f381_719770", 0x0065f780, &script_a4f381_719770);
B100("script_a4f381_7197f0", 0x0065f7c0, &script_a4f381_7197f0);
B100("script_unit_set_3c1", 0x0065f9e0, &script_unit_set_3c1);
B100("script_set_byte_a5d386", 0x0065fc90, &script_set_byte_a5d386);
#undef B100

}  // namespace game_b100_14
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

i32 JOAN_CDECL camera_nudge(i32 mode) {  // 0x0065ade0
    if (mode == 1)
        return static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_cutscene(obj(d::g_game_a5d380))->start_6cb9e0(joan::orig::arg::u32(global<u32>(d::g_now)), 0x43000000u, static_cast<std::int8_t>(joan::orig::arg::i32(1u)), static_cast<std::int8_t>(joan::orig::arg::i32(0u)), 0, 0u, 0u, 0u, static_cast<std::int8_t>(joan::orig::arg::i32(0u)))));  // 0x006cb9e0
    return static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_cutscene(obj(d::g_game_a5d380))->start_6cc6a0(joan::orig::arg::u32(global<u32>(d::g_now)), 0xc3000000u, 0u, 0u)));  // 0x006cc6a0
}

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("Camera::nudge_6cb9e0", 0x0065ade0, &camera_nudge);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b107_17.cpp (game/b107_17)
namespace {
namespace game_b107_17 {

JOAN_COUNTER(c_spawn_fx, "spawn_fx_65ae60");

// ---- 0x0065ae60 ----
struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12);
i32 JOAN_CDECL spawn_fx_65ae60(i8 a, i8 b, i32 x, i32 y, i32 z, u32 p, u32 q, u32 r, i8 s) {
    c_spawn_fx.hit();
    const Vec3 v{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
    u32 g = 0;
    std::memcpy(&g, img::obj(d::g_now), 4);  // unaligned global
    const i8 res = joan::orig::arg::as<i8>(joan::orig::as_cutscene(obj(d::g_game_a5d380))->start(g, u32{0x43000000}, a, b, joan::orig::arg::bits(v.x), joan::orig::arg::bits(v.y), joan::orig::arg::bits(v.z), p, q, r, s));  // 0x006cad50 (ret 0x2c)
    return static_cast<i32>(res);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_17", name, addr, fn)
B("spawn_fx_65ae60", 0x0065ae60, &spawn_fx_65ae60);
#undef B

}  // namespace game_b107_17
}  // namespace

// ---------------------------------------- from modules/game_b107_06.cpp (game/b107_06)
namespace {
namespace game_b107_06 {

JOAN_COUNTER(c_units_sweep, "units_sweep_65dcf0");

// ---- 0x0065aef0 ----
i32 JOAN_CDECL menu_text_65aef0(char a, char b, u32 c, u32 d, u32 e, u32 g, char h) {
    const char r = joan::orig::arg::as<char>(joan::orig::as_cutscene(obj(d::g_game_a5d380))->start_6cb9e0(joan::orig::arg::u32(global<u32>(d::g_now)), 0x43000000u, static_cast<std::int8_t>(joan::orig::arg::i32(a)), static_cast<std::int8_t>(joan::orig::arg::i32(b)), joan::orig::arg::i32(c), joan::orig::arg::u32(d), joan::orig::arg::u32(e), joan::orig::arg::u32(g), static_cast<std::int8_t>(joan::orig::arg::i32(h))));  // 0x006cb9e0
    return static_cast<i32>(r);
}

// ---- 0x0065dcf0 ----
i32 JOAN_CDECL units_sweep_65dcf0(i32 kind) {
    c_units_sweep.hit();
    void* const units = obj(d::g_units);
    for (i32 k = joan::orig::as_dynarray(units)->size(); k > 0; --k) {  // 0x00417ad0
        if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(k)) != 0) continue;  // 0x00428b70
        if (static_cast<i32>(jo::as_unit(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(k))))->group_id) != kind) continue;  // 0x0041b3c0
        if (joan::orig::arg::as<char>(joan::orig::as_game(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(k))))->byte_2e1()) != 0) {  // 0x00432b90
            joan::orig::as_unit(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(k))))->drop_carried();  // 0x005199b0
        }
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_06", name, addr, fn)
B("menu_text_65aef0", 0x0065aef0, &menu_text_65aef0);
B("units_sweep_65dcf0", 0x0065dcf0, &units_sweep_65dcf0);
#undef B

}  // namespace game_b107_06
}  // namespace

// ---------------------------------------- from modules/game_b107_20.cpp (game/b107_20)
namespace {
namespace game_b107_20 {

// 0x0065b030
i32 JOAN_CDECL forward_6cd720(i8 a, u32 b, u32 c, u32 d, u32 e) {
    return static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_cutscene(obj(d::g_game_a5d380))->focus_unit(static_cast<char>(joan::orig::arg::i32(a)), joan::orig::arg::i32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d), joan::orig::arg::u32(e))));  // 0x006cd720
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_20", name, addr, fn)
B("forward_6cd720", 0x0065b030, &forward_6cd720);
#undef B

}  // namespace game_b107_20
}  // namespace

// ---------------------------------------- from modules/game_b107_14.cpp (game/b107_14)
namespace {
namespace game_b107_14 {

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12);

// 0x0065b070
i32 JOAN_CDECL fx_at_cell_65b070(char kind, i32 x, i32 y, i32 z, u32 a, u32 b, u32 c) {
    const V3 v{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
    const char r = joan::orig::arg::as<char>(joan::orig::as_cutscene(obj(d::g_game_a5d380))->move_camera(joan::orig::arg::u32(kind), joan::orig::arg::bits(v.x), joan::orig::arg::bits(v.y), joan::orig::arg::bits(v.z), a, b, c));  // 0x006ccf00
    return static_cast<i32>(static_cast<signed char>(r));
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_14", name, addr, fn)
B("fx_at_cell_65b070", 0x0065b070, &fx_at_cell_65b070);
#undef B

}  // namespace game_b107_14
}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
namespace {
namespace game_b83_04 {

JOAN_COUNTER(c_save_add, "SaveList::add");

// 0x0065bed0
std::int32_t JOAN_CDECL save_add(const char* name, const char* desc, std::uint32_t flag) {
    c_save_add.hit();
    void* const list = obj(d::g_saves);
    char file[0x108];
    std::strcpy(static_cast<char*>(joan::orig::arg::mptr(file)), joan::orig::arg::str(name));  // strcpy
    std::strcat(static_cast<char*>(joan::orig::arg::mptr(file)), joan::orig::arg::str(&img::global<const char>(d::s_SAV_9cfb78)));  // strcat ".SAV"
    std::uint8_t* entry = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_paths(list)->find(joan::orig::arg::str(static_cast<const char*>(file))));  // 0x00477640
    if (entry == nullptr) {
        const std::int16_t index = joan::orig::arg::as<std::int16_t>(joan::orig::as_save_dir(list)->add(joan::orig::arg::str(static_cast<const char*>(nullptr))));  // 0x00477440
        entry = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(list)->get_430410(joan::orig::arg::i16(index)));                                                  // 0x00430410
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(entry + 3)), joan::orig::arg::str(&img::global<const char>(d::s_SAVE)));  // strcpy "SAVE\"
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(entry + 3)), joan::orig::arg::str(static_cast<const char*>(file)));  // strcat
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_game_file(entry)->name)), joan::orig::arg::str(desc));  // strcpy
    }
    jo::as_game_file(entry)->u32_19a = global<std::uint32_t>(d::g_mission);
    jo::as_game_file(entry)->u8_19e = static_cast<std::uint8_t>(flag);
    joan::orig::as_game_file(entry)->save();                                      // 0x00478180
    joan::orig::as_game_file(entry)->save_preview(joan::orig::arg::mptr(global<void*>(d::g_camera)));   // 0x004731e0
    joan::orig::as_list(list)->renumber(fn::code_value(fn::cmp_477bd0));  // 0x00477b10 (compare 0x00477bd0)
    return 1;
}

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("SaveList::add", 0x0065bed0, &save_add);
#undef B83_04

}  // namespace game_b83_04
}  // namespace

// ---------------------------------------- from modules/game_b111_19.cpp (fix/uninit)
namespace {
namespace game_b111_19 {

using joan::orig::helpers::wrap_add;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");

JOAN_COUNTER(c_post, "script_post_event_65c530 (fix/uninit)");

// 0x004eef00 (thunk 0x0040a164, ret 0x40) on the event queue 0xa5ccd0.
void post_event(u32 type, i32 w10, i32 w18, Vec3 v, u32 t) {
    joan::orig::as_triggers(obj(d::g_a5ccd0))->add(joan::orig::arg::u32(type), joan::orig::arg::u32(u32{2}), joan::orig::arg::i16(w10), joan::orig::arg::u32(u32{1}), joan::orig::arg::i16(w18), static_cast<std::int8_t>(joan::orig::arg::i32(u32{1})), v, joan::orig::arg::u32(u32{0x3f800000}), static_cast<std::int8_t>(joan::orig::arg::i32(u32{0})), joan::orig::arg::u32(t), joan::orig::arg::u32(t), static_cast<std::int8_t>(joan::orig::arg::i32(u32{1})), nullptr, joan::orig::arg::i16(u32{0}));
}

// ---- fix/uninit: 0x0065c530 ---------------------------------------------------------------------
i32 JOAN_CDECL script_post_event(u32 id, i32 dt) {
    c_post.hit();
    if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00425d50
    const u32 t = static_cast<u32>(wrap_add(global<i32>(d::g_now), dt));
    const i32 w = global<i16>(d::g_player_unit_recno);
    u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
    // fix/uninit: the original passes the never-written stack vector [ebp-0xc..-0x4]; zero here.
    post_event(0x20, jo::as_map_object(p)->portrait_unit, w, Vec3{0.0f, 0.0f, 0.0f}, t);
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: 0x0065c530 - never-written event vector -> zero vector.
F("script_post_event_65c530", 0x0065c530, &script_post_event);
#undef F

}  // namespace game_b111_19
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_65c5f0, "unit_order_65c5f0");

static u32 JOAN_CDECL f_65ee60() {
    global<u32>(d::g_screen_pending) = 6;
    joan::orig::as_menu(obj(d::g_screen_mgr))->set_state_6f5cc0();  // 0x006f5cc0
    return 1;
}

// 0x0065c5f0: (u32 a1, word a2, word a3, u32 unit_id)
static u32 JOAN_CDECL f_65c5f0(const char* a1, i32 a2, i32 a3, u32 unit_id) {
    c_65c5f0.hit();
    if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(unit_id)) != 0) return 0;  // 0x00428b70
    void* const unit = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(unit_id));  // 0x0041b3c0
    const u32 s = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(a1));    // 0x004a36c0
    if (s == 0) return 0;
    const i32 sw = static_cast<i16>(s);
    const i32 w2 = static_cast<i16>(a2);
    const i32 w3 = static_cast<i16>(a3);
    // 0x0050fd90 reads its first three args as words only.
    const u32 r = joan::orig::arg::as<u32>(joan::orig::as_unit(unit)->split_50fd90(joan::orig::arg::u32(sw), joan::orig::arg::u32(w3), joan::orig::arg::u32(w2), joan::orig::arg::u32(i32{0})));
    if (r == 0 || unit_id != global<u32>(d::g_player_unit_recno)) return r;
    u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(sw)));  // 0x00425ce0
    const char* const name = joan::orig::format_positional(joan::orig::arg::str(jo::as_item_def(q)->message_name), w3);                  // 0x005a6660
    const char* const txt = joan::orig::as_sms(obj(d::g_strings))->open_4(name);                    // 0x004dd6e0
    joan::orig::as_sms(obj(d::g_sms))->add(txt, i32{0}, i32{0});                     // 0x006e5700
    u8* const q2 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(sw)));  // 0x00425ce0
    if (static_cast<i8>(jo::as_item_def(q2)->show_armory) == 0) return r;  // ItemDef +0x1e8 on x86 (behind seven pointers on x64)
    if (static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_game(obj(d::g_game_state))->has_word_728(joan::orig::arg::i16(sw)))) != 0) return r;  // 0x00472a90
    joan::orig::as_game(obj(d::g_game_state))->push_word_728(joan::orig::arg::i32(sw));                                      // 0x004729c0
    joan::orig::as_armory_screen(obj(d::g_armory_screen))->set_item(joan::orig::arg::i16(sw), joan::orig::arg::i16(w2));                                  // 0x006bbc90
    joan::orig::as_armory_screen(obj(d::g_armory_screen))->open();                                          // 0x006ba4b0
    return r;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("unit_order_65c5f0", 0x0065c5f0, &f_65c5f0);
B("f_65ee60", 0x0065ee60, &f_65ee60);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

u32 hi(u32 v) { return v & 0xffff0000u; }

JOAN_COUNTER(c_script_icon, "script_icon_65c770");

// ---- 0x0065c770 ---------------------------------------------------------------------------
i32 JOAN_CDECL script_icon(const char* a1, u32 a2, u32 a3, u32 a4) {
    c_script_icon.hit();
    if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(a4)) != 0) return 0;  // 0x00428b70
    void* const unit = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(a4));  // 0x0041b3c0
    const u32 o = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(a1));          // 0x004a36c0
    if (o == 0) return 0;
    // 0x0050fd90 (ret 0x10) reads its first three args as words.
    const i32 r = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->split_50fd90(joan::orig::arg::u32(static_cast<u32>(static_cast<u16>(o))), joan::orig::arg::u32(static_cast<u32>(static_cast<u16>(a3))), joan::orig::arg::u32(hi(o) | static_cast<u16>(a2)), 1u));
    if (r != 0 && a4 == global<u32>(d::g_player_unit_recno)) {
        u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<u32>(static_cast<u16>(o)))));  // 0x00425ce0
        const char* const s = joan::orig::format_positional(joan::orig::arg::str(jo::as_item_def(q)->message_name), static_cast<i32>(static_cast<i16>(a3)));  // 0x005a6660
        const char* const m = joan::orig::as_sms(obj(d::g_strings))->open_4(s);           // 0x004dd6e0
        joan::orig::as_sms(obj(d::g_sms))->add(m, 0, 0);                    // 0x006e5700
    }
    return r;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("script_icon_65c770", 0x0065c770, &script_icon);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

// ------------------------------------------------------------------ 0x0065cea0
static u32 JOAN_CDECL message_65cea0(const char* id, u32 tagged) {
    if (id == nullptr) return 0;
    if (tagged != 0)
        return joan::orig::arg::as<u32>(joan::orig::as_message_list(obj(d::g_names))->post(id, img::str(d::s_TMESS), joan::orig::arg::u32(i32{0})));  // 0x004ad710 'TMESS'
    return joan::orig::arg::as<u32>(joan::orig::as_message_list(obj(d::g_names))->post(id, nullptr, joan::orig::arg::u32(i32{0})));
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("message_65cea0", 0x0065cea0, &message_65cea0);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

// 0x0065cf00
u32 JOAN_CDECL post_65cf00(const char* a, const char* b, u32 c) {
    if (a == nullptr) return 0;
    return joan::orig::arg::as<u32>(joan::orig::as_message_list(obj(d::g_names))->post(a, b, joan::orig::arg::u32(c)));  // 0x004ad710
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("Msg::post_65cf00", 0x0065cf00, &post_65cf00);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

// ------------------------------------------------------------------ 0x0065cf40
static u32 JOAN_CDECL script_call_65cf40(const char* a, u32 b, u32 c) {
    if (a == nullptr) return 0;
    return joan::orig::arg::as<u32>(joan::orig::as_message_list(obj(d::g_names))->post_speaker(a, joan::orig::arg::u32(b), joan::orig::arg::u32(c)));  // 0x004ad9a0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B("Script::parse_assign_644680", 0x00644680, &H::parse_assign);
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B("Script::parse_compare_645210", 0x00645210, &H::parse_compare);
B("script_call_65cf40", 0x0065cf40, &script_call_65cf40);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b111_08.cpp (game/b111_08)
namespace {
namespace game_b111_08 {

JOAN_COUNTER(c_dist, "Unit::marker_distance_65d6f0");

// ---------------------------------------------------------------------- 0x0065d6f0
struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

float JOAN_CDECL marker_distance(i32 id, i32 marker, i32 flag) {
    c_dist.hit();
    void* const units = obj(d::g_units);
    if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(id)) != 0) return global<float>(d::k_9cf5c4);  // 0x00428b70
    u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
    if (jo::as_unit(u)->cell == nullptr || jo::as_cell(jo::as_unit(u)->cell)->tracks == nullptr) return global<float>(d::k_9cf5c4);
    void* const set = joan::orig::arg::as<void*>(joan::orig::as_name_record(jo::as_cell(jo::as_unit(u)->cell)->tracks)->get_c());  // 0x00453cc0
    u8* const d = joan::orig::arg::as<u8*>(joan::orig::as_table52(set)->slot(joan::orig::arg::i16(marker)));                           // 0x0049ab40 (word)
    if (d == nullptr) return global<float>(d::k_9cf5c4);
    V3 target;
    if (flag != 0) {
        target = jo::view<V3>(jo::as_path_graph(d)->nodes[jo::as_path_graph(d)->node_count - 1].pos);
    } else {
        target = jo::view<V3>(jo::as_path_graph(d)->nodes[0].pos);
    }
    V3 pos{0.0f, 0.0f, 0.0f};  // fully written by 0x004fedb0 (12-byte copy)
    joan::orig::as_unit(u)->position(&pos);  // 0x004fedb0
    const float dx = target.x - pos.x;
    const float dy = target.y - pos.y;
    const float a = dx * dx;
    const float b = dy * dy;
    const float sum = a + b;
    // 0x004256d0 sqrt wrapper (CRT st0, read as double); the caller stores it as a float.
    return static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(sum))));
}

JOAN_REPLACE("game/b111_08", "Unit::marker_distance_65d6f0", 0x0065d6f0, &marker_distance);

}  // namespace game_b111_08
}  // namespace

// ---------------------------------------- from modules/game_b107_08.cpp (game/b107_08)
namespace {
namespace game_b107_08 {

void* const kUnits = obj(d::g_units);

i32 unit_valid_check(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

// 0x0065dba0
i32 JOAN_CDECL unit_kick(i32 id) {
    if (unit_valid_check(id) != 0) return 0;
    if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_game(unit_get(id))->byte_2e1())) != 0) {  // 0x00432b90
        joan::orig::as_unit(unit_get(id))->drop_carried();                             // 0x005199b0
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_08", name, addr, fn)
B("unit_kick_65dba0", 0x0065dba0, &unit_kick);
#undef B

}  // namespace game_b107_08
}  // namespace

// ---------------------------------------- from modules/game_b115_09.cpp (fix/uninit)
namespace {
namespace game_b115_09 {

u8* bp(void* p) { return static_cast<u8*>(p); }

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_script, "ScriptCmd::exec_65fd70");

void strncpy_(void* d, const void* s, u32 n) { std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }  // strncpy

// ======================================================================= 0x0065fd70
struct HScript {
    u8* self() { return bp(this); }
    void* arg(i32 n) { return joan::orig::arg::as<void*>(joan::orig::as_reader(this)->list_get(joan::orig::arg::i32(n))); }  // 0x0063e9f0
    void* argv(i32 n) {  // virtual +0x1c of the argument (value pointer)
        void* const a = arg(n);
        return thiscall<void*>((*static_cast<std::uintptr_t* const*>(a))[0x1c / 4], a);  // vtable slot 7
    }
    i32 nargs() { return joan::orig::arg::as<i32>(joan::orig::as_reader(this)->count()); }  // 0x0063ecd0
    u8* owner() {  // walk +4 until a node with +8 == 1 (or the last)
        u8* p = self();
        while (jo::as_script_node(p)->parent != nullptr) {
            p = jo::view<u8*>(jo::as_script_node(p)->parent);
            if (jo::as_script_node(p)->kind == 1) break;
        }
        return p;
    }
    // Entry table lookups of a sprite set (0x00638150).
    static u8* entry(void* set, i32 idx) { return joan::orig::arg::as<u8*>(joan::orig::as_dlg_res(set)->entry(joan::orig::arg::i32(idx))); }
    // strncpy(str, entry->name, min(entry->len, 0x190)); str[len] = 0.
    static void copy_entry(void* set, i32 idx, void* s) {
        const i32 len = joan::orig::arg::as<i32>(joan::orig::min_int(joan::orig::arg::i32(static_cast<i32>(jo::as_dlg_res_entry(entry(set, idx))->len)), i32{0x190}));  // 0x0046f020 min
        const char* const src = jo::view<const char*>(jo::as_dlg_res_entry(entry(set, idx))->text);
        char* const dst = joan::orig::as_string(s)->c_str();  // 0x00417d90
        strncpy_(dst, src, static_cast<u32>(len));
        joan::orig::as_string(s)->c_str()[len] = 0;
    }
    // fix/uninit: the original copies (strchr(name, '.') - name) bytes into a 0x104-byte buffer.
    static void copy_base(char* buf, const char* name) {
        const char* const dot = joan::orig::arg::as<const char*>(joan::orig::libc::strchr(joan::orig::arg::str(name), i32{0x2e}));  // strchr
        i32 len = dot != nullptr ? static_cast<i32>(dot - name) : static_cast<i32>(std::strlen(name));
        if (len > 0x103) len = 0x103;
        strncpy_(buf, name, static_cast<u32>(len));
        buf[len] = 0;
    }
    i32 word_arg(i32 n) { return static_cast<i32>(*static_cast<i16*>(argv(n))); }
    i32 atoi_arg(i32 n) { return joan::crt::atoi(joan::orig::arg::str(static_cast<char*>(argv(n)) + 1)); }

    void exec(i32 /*unused*/) {
        c_script.hit();
        void* const mgr = obj(d::g_dialogue);
        const i32 op = static_cast<i32>(jo::self<jo::ScriptFuncCall>(this)->func_id);
        if (op == 0x13c) {
            joan::orig::as_script_node(this)->set_type(i32{1});  // 0x00660ff0
            const i32 id = *static_cast<i32*>(argv(1));
            joan::orig::as_script(mgr)->purge_63d150();  // 0x0063d150
            i32 d18 = 0;
            alignas(4) u8 s[0x194];               // [ebp-0x1ac]
            joan::orig::as_string(s)->construct();  // 0x005a1170
            u8* const p = owner();                // [ebp-0x1b4]
            void* const set = joan::orig::arg::as<void*>(joan::orig::as_script(p)->dlg_res());  // 0x006509c0
            const char* v1b8 = nullptr;
            i32 w1bc = 0;
            i32 w1c0 = 0;
            if (set != nullptr) {
                const i32 idx = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_table(set)->index_of_ref(joan::orig::arg::str(argv(2)))));  // 0x006380b0
                if (idx != 0) {
                    copy_entry(set, idx, s);
                    if (jo::view<i8>(jo::as_dlg_res_entry(entry(set, idx))->kind) == 0) {
                        v1b8 = nullptr;
                        w1bc = 0;
                    } else {
                        v1b8 = jo::as_dlg_res_entry(entry(set, idx))->text2;  // Dialog::show_message_63cdc0 a4 -> Dialogue::speech_text
                        w1bc = static_cast<i32>(jo::as_dlg_res_entry(entry(set, idx))->len2);
                    }
                    const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_script(p)->name());  // 0x006509a0
                    char base[0x104];                                               // [ebp-0x2d4]
                    copy_base(base, name);
                    const i32 n = atoi_arg(2);
                    w1c0 = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_sound_res_table(obj(d::g_sound_res_table))->find_positive(base, joan::orig::arg::u32(n))));  // 0x00639380
                } else {
                    joan::orig::as_string(s)->assign(joan::orig::arg::str(jo::as_script_node(arg(2))->value));  // 0x005a1440
                }
            } else {
                joan::orig::as_string(s)->assign(joan::orig::arg::str(argv(2)));  // 0x005a1440
            }
            if (nargs() == 2 || nargs() == 3) {
                jo::self<jo::ScriptNode>(this)->value = reinterpret_cast<void*>(static_cast<std::uintptr_t>(joan::orig::arg::as<u32>(joan::orig::as_dialog(mgr)->show_message_63cdc0(joan::orig::arg::i32(id), static_cast<void*>(s), joan::orig::arg::u32(w1c0), joan::orig::arg::u32(i32{5}), v1b8, joan::orig::arg::u32(w1bc), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1})))));  // 0x0063cdc0; x64: integer stored in the pointer-typed ScriptNode::value
                if (nargs() == 3) d18 = *static_cast<i32*>(argv(3));
            } else if (nargs() == 6 || nargs() == 7) {
                const i32 a6 = word_arg(6);
                const i32 a5 = word_arg(5);
                const i32 a4 = word_arg(4);
                const i32 a3 = word_arg(3);
                jo::self<jo::ScriptNode>(this)->value = reinterpret_cast<void*>(static_cast<std::uintptr_t>(joan::orig::arg::as<u32>(joan::orig::as_dialog(mgr)->show_message_63cdc0(joan::orig::arg::i32(id), static_cast<void*>(s), joan::orig::arg::u32(w1c0), joan::orig::arg::u32(i32{5}), v1b8, joan::orig::arg::u32(w1bc), joan::orig::arg::u32(a3), joan::orig::arg::u32(a4), joan::orig::arg::u32(a5), joan::orig::arg::u32(a6)))));  // 0x0063cdc0; x64: integer stored in the pointer-typed ScriptNode::value
                if (nargs() == 7) d18 = *static_cast<i32*>(argv(7));
            }
            for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_name(s)->length()) - 1; i >= 0; --i) {  // 0x0045e110
                const i8 c = joan::orig::arg::as<i8>(joan::orig::as_bytes(s)->get(joan::orig::arg::i32(i)));                // 0x0063bf10
                if (static_cast<i32>(c) != 0x2e) {
                    const i8 c2 = joan::orig::arg::as<i8>(joan::orig::as_bytes(s)->get(joan::orig::arg::i32(i)));
                    if (static_cast<i32>(c2) != 0x20) break;
                }
                if (i == 0) d18 = -100;
            }
            if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)) == 0 && d18 != -100)  // 0x00428b70
                (void)joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0 (result unused)
            return;
        }
        if (op == 0x13d) {
            joan::orig::as_dialogue(mgr)->delete_line(joan::orig::arg::u32(*static_cast<i32*>(argv(1))));  // 0x0063d120
            return;
        }
        if (op == 0x13e) {
            joan::orig::as_script(mgr)->purge_63d150();  // 0x0063d150
            return;
        }
        if (op == 0x13f) {
            joan::orig::as_script_node(this)->set_type(i32{1});  // 0x00660ff0
            const i32 id = *static_cast<i32*>(argv(1));
            joan::orig::as_script(mgr)->purge_63d150();  // 0x0063d150
            alignas(4) u8 s[0x194];     // [ebp-0x474]
            joan::orig::as_string(s)->construct(joan::orig::arg::str(argv(2)));  // 0x005a11a0
            const char* v478 = nullptr;
            i32 w47c = 0;
            i32 w480 = 0;
            if (joan::orig::arg::as<i32>(joan::orig::as_name(s)->length()) > 0) {  // 0x0045e110
                u8* const p = owner();               // [ebp-0x488]
                void* const set = joan::orig::arg::as<void*>(joan::orig::as_script(p)->dlg_res());  // 0x006509c0
                if (set != nullptr) {
                    const i32 idx = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_table(set)->index_of_ref(joan::orig::arg::str(argv(2)))));  // 0x006380b0
                    copy_entry(set, idx, s);
                    v478 = jo::as_dlg_res_entry(entry(set, idx))->text2;  // Dialog::show_message_63cdc0 a4 -> Dialogue::speech_text
                    w47c = static_cast<i32>(jo::as_dlg_res_entry(entry(set, idx))->len2);
                    const i32 n = atoi_arg(2);
                    const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_script(p)->name());  // 0x006509a0
                    w480 = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_sound_res_table(obj(d::g_sound_res_table))->find_positive(name, joan::orig::arg::u32(n))));  // 0x00639380
                }
            }
            const u32 sprite = joan::orig::arg::as<u32>(joan::orig::as_dialog(mgr)->show_message_63cdc0(joan::orig::arg::i32(id), static_cast<void*>(s), joan::orig::arg::u32(w480), joan::orig::arg::u32(i32{5}), v478, joan::orig::arg::u32(w47c), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1})));  // 0x0063cdc0
            jo::self<jo::ScriptNode>(this)->value = reinterpret_cast<void*>(static_cast<std::uintptr_t>(sprite));  // x64: integer stored in the pointer-typed ScriptNode::value
            for (i32 k = 3; k <= nargs(); k += 2) {
                alignas(4) u8 s2[0x194];          // [ebp-0x62c]
                joan::orig::as_string(s2)->construct();  // 0x005a1170
                u8* const p = owner();            // [ebp-0x634]
                void* const set = joan::orig::arg::as<void*>(joan::orig::as_script(p)->dlg_res());  // 0x006509c0
                if (set != nullptr) {
                    const i32 idx = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_table(set)->index_of_ref(joan::orig::arg::str(argv(k)))));  // 0x006380b0
                    copy_entry(set, idx, s2);
                } else {
                    joan::orig::as_string(s2)->assign(joan::orig::arg::str(argv(k)));  // 0x005a1440
                }
                const i32 flag = *static_cast<i32*>(argv(k + 1)) != 0 ? 1 : 0;
                joan::orig::as_text(mgr)->add_line_63d250(joan::orig::arg::u32(sprite), joan::orig::arg::mptr(static_cast<void*>(s2)), joan::orig::arg::u32(flag), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}));  // 0x0063d250
            }
            return;
        }
        if (op == 0x140) {
            joan::orig::as_script_node(this)->set_type(i32{1});  // 0x00660ff0
            jo::self<jo::ScriptNode>(this)->value = reinterpret_cast<void*>(static_cast<std::uintptr_t>(joan::orig::arg::as<u32>(joan::orig::as_dialogue(mgr)->choice_of(joan::orig::arg::u32(*static_cast<i32*>(argv(1)))))));  // 0x0063d770; x64: integer stored in the pointer-typed ScriptNode::value
            return;
        }
        if (op == 0x141) {
            if (jo::self<jo::ScriptNode>(this)->value == nullptr) {
                void* const mem = joan::crt::operator_new(u32{0x191});  // operator new
                void* const s = mem != nullptr ? joan::orig::as_string(mem)->construct() : nullptr;  // 0x005a1170
                jo::self<jo::ScriptNode>(this)->value = s;
            }
            joan::orig::as_script_node(this)->set_type(i32{3});  // 0x00660ff0
            alignas(4) u8 s[0x194];              // [ebp-0x7d4]
            joan::orig::as_string(s)->construct();  // 0x005a1170
            u8* const p = owner();               // [ebp-0x7dc]
            void* const set = joan::orig::arg::as<void*>(joan::orig::as_script(p)->dlg_res());  // 0x006509c0
            const i32 idx = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_table(set)->index_of_ref(joan::orig::arg::str(argv(1)))));  // 0x006380b0
            copy_entry(set, idx, s);
            char* const text = joan::orig::as_string(s)->c_str();  // 0x00417d90
            joan::orig::as_string(jo::self<jo::ScriptNode>(this)->value)->assign(joan::orig::arg::str(text));  // 0x005a1440
            return;
        }
        if (op == 0x142) {
            joan::orig::as_script_node(this)->set_type(i32{1});  // 0x00660ff0
            u8* const p = owner();               // [ebp-0x7ec]
            void* const set = joan::orig::arg::as<void*>(joan::orig::as_script(p)->dlg_res());  // 0x006509c0
            joan::orig::as_table(set)->index_of_ref(joan::orig::arg::str(argv(2)));           // 0x006380b0 (result unused)
            const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_script(p)->name());  // 0x006509a0
            char base[0x104];                                               // [ebp-0x8fc]
            copy_base(base, name);
            const i32 n = atoi_arg(2);
            const i16 w900 = joan::orig::arg::as<i16>(joan::orig::as_sound_res_table(obj(d::g_sound_res_table))->find_positive(base, joan::orig::arg::u32(n)));  // 0x00639380
            u8* const sprite = joan::orig::arg::as<u8*>(joan::orig::as_streams(mgr)->get(joan::orig::arg::i32(static_cast<i32>(reinterpret_cast<std::uintptr_t>(jo::as_script_node(arg(1))->value)))));                  // 0x0063d1e0
            jo::as_dialogue(sprite)->i16_340 = w900;
            jo::self<jo::ScriptNode>(this)->value = reinterpret_cast<void*>(std::uintptr_t{1});  // x64: integer stored in the pointer-typed ScriptNode::value
        }
    }
};

static_assert(sizeof(i16) == 2 && sizeof(float) == 4, "widths");

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: base-name copy without '.' / longer than 0x103 chars -> whole name, clamped.
FIX("ScriptCmd::exec_65fd70", 0x0065fd70, &HScript::exec);
#undef FIX

}  // namespace game_b115_09
}  // namespace

// ---------------------------------------- from modules/game_b111_16.cpp (game/b111_16)
//
//      0x00656dc0 Seat::find_free_656dc0      (cdecl(i16 a, i16 b, const char* cls)) -> i32.
//                 A class string not starting with 'M' / 'F' leaves the mask byte uninitialised:
//                 that case runs the original body (after the early checks, which only call the
//                 pure unit-list queries 0x00428b70 / 0x0041b3c0 that the original repeats).
//  * module fix/uninit (installed only with [Fixes] UninitReads=1):
//
//  * batch 118/03 (stage S5 maps no original body): with module fix/uninit enabled
//    (joan::hook::module_enabled("fix/uninit"), read on the first call) the original-body case
//    above runs the C++ version with the never-written value set to its neutral value instead:
//      0x00656dc0 class not 'M' / 'F': mask 0 - no slot matches, -1 (no seat taken);
//    With fix/uninit off (mapped mode) the original body still runs.
namespace {
namespace game_b111_16 {

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

i32 ftol(double v) { return joan::hook::x87::ftol(v); }
float as_float(u32 bits) {
    float f = 0.0f;
    std::memcpy(&f, &bits, 4);
    return f;
}
u32 as_bits(float f) {
    u32 b = 0;
    std::memcpy(&b, &f, 4);
    return b;
}

struct Vec {
    float x, y, z;
};
static_assert(sizeof(Vec) == 12, "vec3 by value");

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);

JOAN_COUNTER(c_seat, "Seat::find_free_656dc0");

// ------------------------------------------------------------------ 0x00656dc0 (cdecl)
static i32 JOAN_CDECL find_free_seat(i16 a, i16 b, const char* cls) {
    if (joan::orig::as_dynarray(kUnits)->empty_at(static_cast<i32>(a)) != 0) return -1;  // 0x00428b70
    if (joan::orig::as_dynarray(kUnits)->empty_at(static_cast<i32>(b)) != 0) return -1;
    u8* const u1 = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(a)));  // 0x0041b3c0
    u8* const u2 = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(b)));
    if (jo::as_unit(u2)->crew_slots_ready == 0) return -1;
    i32 mask = 0;
    if (cls[0] == 'M') {
        mask = 1;
    } else if (cls[0] == 'F') {
        mask = 2;
    } else if (!fix_uninit()) {
        // The mask byte [ebp-0xc] stays uninitialised: run the untouched original body (the
        // queries above are pure and are simply repeated there).
        return joan::orig::arg::as<i32>(joan::orig::seat::find_free_656dc0_656dc0(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::str(cls)));
    }  // fix/uninit: mask 0 - no seat class matches, the loop below finds nothing (-1)
    c_seat.hit();
    i32 best_slot = -1;
    u32 best_bits = 0xbf800000u;
    Vec p1{};
    joan::orig::as_unit(u1)->position(&p1);                                    // 0x004fedb0
    u8* const tbl = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(u2)->type_id)));  // 0x0042d0a0
    const Fpu fpu{true};
    for (i32 j = 0; j < jo::self<jo::UnitType>(tbl)->slot_count; ++j) {
        if ((static_cast<i32>(jo::view<i8*>(jo::as_unit_type(tbl)->buffers[2])[j]) & mask) == 0) continue;
        if (jo::view<i32*>(jo::as_unit(u2)->mask_slots)[j] > 0) continue;
        if (jo::view<i32*>(jo::as_unit(u2)->mask_slots_b)[j] > 0) continue;
        const float* const sp = jo::view<float*>(jo::as_unit_type(tbl)->buffers[0]) + j * 3;  // float[3] rows
        const float sx = sp[0], sy = sp[1], sz = sp[2];
        const float* m = nullptr;
        float local[16] = {};
        if (jo::as_unit(u2)->actor != nullptr) {
            m = joan::orig::arg::as<const float*>(joan::orig::as_bone(jo::as_actor(jo::as_unit(u2)->actor)->bone)->matrix());  // 0x0060ef80
        } else {
            joan::orig::matrix_rotation(joan::orig::arg::mptr(static_cast<void*>(local)), joan::orig::arg::f32(jo::view<u32>(jo::as_unit(u2)->heading)));  // 0x0060bc50
            import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, static_cast<void*>(local), jo::as_unit(u2)->position_);  // multiply_translation
            m = local;
        }
        const i32 x = ftol(((sx * m[0] + sy * m[4]) + sz * m[8]) + m[12]);
        const i32 y = ftol(((sx * m[1] + sy * m[5]) + sz * m[9]) + m[13]);
        const i32 dx = ftol(fpu.sub(p1.x, static_cast<double>(x)));  // fild; fsubr
        const i32 dy = ftol(fpu.sub(p1.y, static_cast<double>(y)));
        const u32 dd = static_cast<u32>(dx) * static_cast<u32>(dx) + static_cast<u32>(dy) * static_cast<u32>(dy);
        const float d = static_cast<float>(static_cast<i32>(dd));
        if (best_bits == 0xbf800000u || !(d >= as_float(best_bits))) {  // test ah,1: less or NaN
            best_bits = as_bits(d);
            if (best_slot != -1) jo::view<i32*>(jo::as_unit(u2)->mask_slots_b)[best_slot] = 0;
            best_slot = j;
            jo::view<i32*>(jo::as_unit(u2)->mask_slots_b)[j] = jo::as_unit(u1)->id;
        }
    }
    return best_slot;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b111_16", name, addr, fn)
B("Seat::find_free_656dc0", 0x00656dc0, &find_free_seat);
#undef B

}  // namespace game_b111_16
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_a6a250() -> void { joan::bind::free<&::game_b95_06::init_a6a250>(); }  // 0x650fd0
auto joan::orig::script_random(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_04::script_random>(a0)); }  // 0x651010
auto joan::orig::ratio_half() -> void { joan::bind::free<&::game_b99_08::ratio_half>(); }  // 0x651060
auto joan::orig::init_a6a25c() -> void { joan::bind::free<&::game_b100_12::init_a6a25c>(); }  // 0x6510d0
auto joan::orig::script_set_a50298(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_12::script_set_a50298>(a0)); }  // 0x651100
auto joan::orig::script_capture_place(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_06::script_capture_place>(a0)); }  // 0x651130
auto joan::orig::ratio_quarter() -> void { joan::bind::free<&::game_b99_08::ratio_quarter>(); }  // 0x6511c0
auto joan::orig::script_rgb_component(const char* a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b100_12::script_rgb_component>(a0, a1)); }  // 0x6511f0
auto joan::orig::script_unit_2a1_1d(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_06::script_unit_2a1_1d>(a0)); }  // 0x651260
auto joan::orig::script::is_building(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_is_building>(a0)); }  // 0x6512d0
auto joan::orig::script::in_arc(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_in_arc>(a0, a1)); }  // 0x651430
auto joan::orig::script::mode_gt1(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_mode_gt1>(a0)); }  // 0x651550
auto joan::orig::script::mode_is1(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_mode_is1>(a0)); }  // 0x651590
auto joan::orig::script::set_mode(std::int32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_set_mode>(a0, a1)); }  // 0x6515d0
auto joan::orig::script::set_target_mode(std::int32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_set_target_mode>(a0, a1)); }  // 0x651690
auto joan::orig::unit_forward_54b480(std::int32_t a0, const char* a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_01::unit_forward_54b480>(a0, a1, a2, a3, a4, a5, a6)); }  // 0x6517c0
auto joan::orig::group_try_find_651830(std::int32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b107_10::group_try_find>(a0, a1)); }  // 0x651830
auto joan::orig::units::call_503680_6519d0(std::int32_t a0, const char* a1, std::uint32_t a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b107_12::unit_call_503680>(a0, a1, a2, a3)); }  // 0x6519d0
auto joan::orig::script::call_4a36c0(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_06::call_4a36c0>(a0)); }  // 0x651a30
auto joan::orig::script_651a60(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_651a60>(a0)); }  // 0x651a60
auto joan::orig::script_attach(const char* a0, const char* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b95_06::script_attach>(a0, a1)); }  // 0x651ad0
auto joan::orig::spawn_named(std::uint32_t a0, const char* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::int32_t a5, std::uint32_t a6) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b97_06::spawn_named>(a0, a1, a2, a3, a4, a5, a6)); }  // 0x651c90
auto joan::orig::script_a460a0_49f370(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_12::script_a460a0_49f370>(a0)); }  // 0x651e70
auto joan::orig::script_remove_objects(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_remove_objects>(a0)); }  // 0x651ea0
auto joan::orig::script_object_set_heading(std::int16_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_object_set_heading>(a0, a1)); }  // 0x651fa0
auto joan::orig::script_set_type(std::uint32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_06::script_set_type>(a0, a1)); }  // 0x652010
auto joan::orig::script::formation(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_formation>(a0)); }  // 0x6520d0
auto joan::orig::script::formation_slot(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_formation_slot>(a0, a1)); }  // 0x652190
auto joan::orig::unit_value_2ba(std::int16_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b95_06::unit_value_2ba>(a0)); }  // 0x652280
auto joan::orig::script::set_2ba(std::int16_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_06::set_2ba>(a0, a1)); }  // 0x6522f0
auto joan::orig::script::unit_528300(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_unit_528300>(a0)); }  // 0x652370
auto joan::orig::script_unit_kill_type(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_12::script_unit_kill_type>(a0)); }  // 0x6523c0
auto joan::orig::script_slot1_d1() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_slot1_d1>()); }  // 0x652440
auto joan::orig::object_d1(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_04::object_d1>(a0)); }  // 0x6524a0
auto joan::orig::script_obj_set_ce(std::int32_t a0, std::int8_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_12::script_obj_set_ce>(a0, a1)); }  // 0x652520
auto joan::orig::object_byte_ce(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_06::object_byte_ce>(a0)); }  // 0x652590
auto joan::orig::script_object_set_cf(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_object_set_cf>(a0, a1)); }  // 0x6525f0
auto joan::orig::unit_byte_d0(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_06::unit_byte_d0>(a0)); }  // 0x652660
auto joan::orig::script_unit_facing_check(std::int16_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_unit_facing_check>(a0, a1)); }  // 0x6526e0
auto joan::orig::unit_has_enemy_near(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_04::unit_has_enemy_near>(a0)); }  // 0x652820
auto joan::orig::WeatherSystem::set_word_20(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_08::H::set_word_20>(this, a0)); }  // 0x652a10
auto joan::orig::unit_check_652a40(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b107_04::unit_check_652a40>(a0)); }  // 0x652a40
auto joan::orig::script::get_unit_flag22(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b104_12::unit_flag22>(a0)); }  // 0x652aa0
auto joan::orig::script_unit_busy(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b92_05::script_unit_busy>(a0)); }  // 0x652b00
auto joan::orig::unit_check_652c10(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_04::unit_check_652c10>(a0)); }  // 0x652c10
auto joan::orig::script_unit_set_52(std::int16_t a0, std::int8_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_12::script_unit_set_52>(a0, a1)); }  // 0x652de0
auto joan::orig::script::reset_all() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_06::reset_all_652e60>()); }  // 0x652e60
auto joan::orig::script::angle_to(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_angle_to>(a0, a1)); }  // 0x652ee0
auto joan::orig::script_unit_value(std::int16_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_04::script_unit_value>(a0, a1)); }  // 0x652f80
auto joan::orig::script_heading_delta(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b92_05::script_heading_delta>(a0, a1)); }  // 0x653000
auto joan::orig::script::distance_to_object(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_distance_to_object>(a0, a1)); }  // 0x653060
auto joan::orig::script::get_a46059() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b104_12::get_a46059>()); }  // 0x653110
auto joan::orig::script::get_word_ptr(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b104_12::get_word_ptr>(a0)); }  // 0x653130
auto joan::orig::script::distance(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_distance>(a0, a1)); }  // 0x653150
auto joan::orig::script_unit_set_1c5(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_unit_set_1c5>(a0, a1)); }  // 0x653210
auto joan::orig::script_unit_set_1c7(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_unit_set_1c7>(a0, a1)); }  // 0x6532a0
auto joan::orig::script_unit_427b50(std::int16_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_unit_427b50>(a0, a1)); }  // 0x653300
auto joan::orig::script_unit_group_entry(std::int16_t a0, const char* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b95_07::script_unit_group_entry>(a0, a1)); }  // 0x653360
auto joan::orig::script_any_quest_item(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_12::script_any_quest_item>(a0)); }  // 0x653400
auto joan::orig::script_unit_set_2e_653590(std::int16_t a0, std::int8_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_unit_set_2e>(a0, a1)); }  // 0x653590
auto joan::orig::script_unit_set_2e(std::int16_t a0, std::int8_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_12::script_unit_set_2e>(a0, a1)); }  // 0x653640
auto joan::orig::script_unit_6537a0(std::int16_t a0, const char* a1, std::int32_t a2, std::uint32_t a3, std::int16_t a4, std::int16_t a5) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_unit_6537a0>(a0, a1, a2, a3, a4, a5)); }  // 0x6537a0
auto joan::orig::script_open_buy(std::int16_t a0, std::int16_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_12::script_open_buy>(a0, a1)); }  // 0x653890
auto joan::orig::script_toggle_a56398() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_toggle_a56398>()); }  // 0x653930
auto joan::orig::script_653970() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_653970>()); }  // 0x653970
auto joan::orig::script_set_a5027e(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_set_a5027e>(a0)); }  // 0x6539a0
auto joan::orig::script_653a50(std::int16_t a0, std::uint32_t a1, std::uint8_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_653a50>(a0, a1, a2)); }  // 0x653a50
auto joan::orig::script_reset_first_unit() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_reset_first_unit>()); }  // 0x653a90
auto joan::orig::script_not_a4fc98_flag() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_not_a4fc98_flag>()); }  // 0x653c20
auto joan::orig::unit_state_is_42(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_01::unit_state_is_42>(a0)); }  // 0x653c60
auto joan::orig::script_unit_in_zone(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b92_05::script_unit_in_zone>(a0, a1)); }  // 0x653ca0
auto joan::orig::script_find_unit_for(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_find_unit_for>(a0)); }  // 0x653d60
auto joan::orig::script::set_side_relation(const char* a0, const char* a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::set_side_relation>(a0, a1, a2)); }  // 0x653df0
auto joan::orig::script_a484d0_4c54a0(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b98_07::script_a484d0_4c54a0>(a0)); }  // 0x653eb0
auto joan::orig::script_building_49c240(std::int16_t a0, const char* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b99_08::script_building_49c240>(a0, a1)); }  // 0x653ef0
auto joan::orig::script::group_contains(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_group_contains>(a0, a1)); }  // 0x653f30
auto joan::orig::script_49c470(std::int16_t a0, std::int16_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b97_07::script_49c470>(a0, a1)); }  // 0x653fe0
auto joan::orig::unit_call_654020(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b105_18::unit_call_654020>(a0, a1)); }  // 0x654020
auto joan::orig::script_654060(std::int16_t a0, std::int16_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_654060>(a0, a1)); }  // 0x654060
auto joan::orig::script_6540a0(std::int16_t a0, std::int16_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b102_13::script_6540a0>(a0, a1)); }  // 0x6540a0
auto joan::orig::script_6540e0(std::int16_t a0, const char* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b102_13::script_6540e0>(a0, a1)); }  // 0x6540e0
auto joan::orig::script_654120(std::int16_t a0, const char* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b102_13::script_654120>(a0, a1)); }  // 0x654120
auto joan::orig::script_654160(std::int16_t a0, const char* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_654160>(a0, a1)); }  // 0x654160
auto joan::orig::script_squad_1ca(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_squad_1ca>(a0)); }  // 0x6541a0
auto joan::orig::script::reset_object_1ca(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::reset_object_1ca>(a0)); }  // 0x6541d0
auto joan::orig::script_building_4c4440(std::int16_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_building_4c4440>(a0, a1, a2)); }  // 0x654240
auto joan::orig::script::place_switch_side(std::uint32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::place_switch_side>(a0, a1)); }  // 0x6542b0
auto joan::orig::Unit::field_1ce() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_07::H::field_1ce>(this)); }  // 0x654530
auto joan::orig::script::place_set_side(std::uint32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::place_set_side>(a0, a1)); }  // 0x654560
auto joan::orig::script::group_angle(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_group_angle>(a0)); }  // 0x6547a0
auto joan::orig::script::group_set_angle(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_group_set_angle>(a0, a1)); }  // 0x6547e0
auto joan::orig::script_squad_set_rates(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_squad_set_rates>(a0, a1)); }  // 0x654830
auto joan::orig::script_6548b0(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_6548b0>(a0)); }  // 0x6548b0
auto joan::orig::script_place_467dd0(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int32_t a3, const char* a4, const char* a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_place_467dd0>(a0, a1, a2, a3, a4, a5)); }  // 0x6548f0
auto joan::orig::script_place_467ea0(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::int32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_place_467ea0>(a0, a1, a2, a3, a4)); }  // 0x654960
auto joan::orig::script_unit_object_dist(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_unit_object_dist>(a0, a1)); }  // 0x6549c0
auto joan::orig::Track::set_word_44(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_13::H::set_word_44>(this, a0)); }  // 0x654a80
auto joan::orig::Track::set_4a(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_04::H::set_4a>(this, a0)); }  // 0x654ab0
auto joan::orig::script_place_set_13c(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_place_set_13c>(a0, a1)); }  // 0x654ae0
auto joan::orig::script_place_sum(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_place_sum>(a0)); }  // 0x654b30
auto joan::orig::script_place_set_97(std::int16_t a0, std::int8_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_place_set_97>(a0, a1)); }  // 0x654bc0
auto joan::orig::script_place_set_98(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_place_set_98>(a0, a1)); }  // 0x654c20
auto joan::orig::script_place_654c80(std::int16_t a0, std::int16_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_place_654c80>(a0, a1)); }  // 0x654c80
auto joan::orig::script_place_setup(std::int16_t a0, std::int32_t a1, std::int8_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_place_setup>(a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x654d70
auto joan::orig::script_place_654e30(std::int16_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_place_654e30>(a0, a1)); }  // 0x654e30
auto joan::orig::script::object_set_81(std::uint32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::object_set_81>(a0, a1)); }  // 0x654ea0
auto joan::orig::script_place_654f10(std::int16_t a0, std::int16_t a1, std::uint32_t a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b102_13::script_place_654f10>(a0, a1, a2, a3)); }  // 0x654f10
auto joan::orig::script::object_call_46b730(std::uint32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::object_call_46b730>(a0, a1)); }  // 0x654fa0
auto joan::orig::script::fn_655010(const char* a0, const char* a1, char a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b108_08::fn_655010>(a0, a1, a2)); }  // 0x655010
auto joan::orig::script_6550d0(const char* a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_6550d0>(a0, a1)); }  // 0x6550d0
auto joan::orig::unit_reset_655420(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b105_18::unit_reset_655420>(a0)); }  // 0x655420
auto joan::orig::script_unit_set_50abd0(std::int16_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_unit_set_50abd0>(a0, a1)); }  // 0x655620
auto joan::orig::script_unit_heading_deg(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_unit_heading_deg>(a0)); }  // 0x655680
auto joan::orig::script::unit_set_5089e0(std::uint32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::unit_set_5089e0>(a0, a1)); }  // 0x6556e0
auto joan::orig::script::unit_set_side(std::uint32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::unit_set_side>(a0, a1)); }  // 0x655750
auto joan::orig::script_unit_41fcc0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_unit_41fcc0>(a0)); }  // 0x655810
auto joan::orig::script_unit_call_442820(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_unit_call_442820>(a0)); }  // 0x655870
auto joan::orig::script::set_2d7(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_set_2d7>(a0, a1)); }  // 0x6558d0
auto joan::orig::script_unit_4fc220(std::int16_t a0, std::int16_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_unit_4fc220>(a0, a1)); }  // 0x655930
auto joan::orig::script_unit_1e2(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_unit_1e2>(a0)); }  // 0x655990
auto joan::orig::script_unit_51a250(std::int16_t a0, std::int16_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_unit_51a250>(a0, a1)); }  // 0x6559e0
auto joan::orig::script_unit_unlink_1f5(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_unit_unlink_1f5>(a0)); }  // 0x655a40
auto joan::orig::script::unit_field_1e2(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b100_13::unit_field_1e2>(a0)); }  // 0x655b20
auto joan::orig::Shop::set_12(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_07::H::set_12>(this, a0)); }  // 0x655b70
auto joan::orig::Shop::set_16(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_08::H::set_16>(this, a0)); }  // 0x655ba0
auto joan::orig::script_unit_2e3(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_unit_2e3>(a0)); }  // 0x655bd0
auto joan::orig::script_unit_set_2e3(std::int16_t a0, std::int16_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_unit_set_2e3>(a0, a1)); }  // 0x655c20
auto joan::orig::script_unit_2e5(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_unit_2e5>(a0)); }  // 0x655c80
auto joan::orig::script::unit_set_2e5(std::uint32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::unit_set_2e5>(a0, a1)); }  // 0x655cd0
auto joan::orig::script_unit_2e7(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_unit_2e7>(a0)); }  // 0x655d30
auto joan::orig::script_unit_set_2e7(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_unit_set_2e7>(a0, a1)); }  // 0x655d80
auto joan::orig::script_unit_2e9(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_unit_2e9>(a0)); }  // 0x655de0
auto joan::orig::script_unit_set_2e9(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_unit_set_2e9>(a0, a1)); }  // 0x655e30
auto joan::orig::script_unit_set_2eb(std::int16_t a0, std::int16_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_unit_set_2eb>(a0, a1)); }  // 0x655e90
auto joan::orig::script_unit_2eb(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_unit_2eb>(a0)); }  // 0x655f10
auto joan::orig::script::units_set_2eb(std::uint32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::units_set_2eb>(a0, a1)); }  // 0x655f80
auto joan::orig::script_building_units_set_2eb(std::int16_t a0, std::int16_t a1, std::int16_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_building_units_set_2eb>(a0, a1, a2)); }  // 0x656030
auto joan::orig::script::unit_2eb_is_2(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::unit_2eb_is_2>(a0)); }  // 0x6561b0
auto joan::orig::script_unit_set_114(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_unit_set_114>(a0, a1)); }  // 0x656210
auto joan::orig::script_unit_114(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_unit_114>(a0)); }  // 0x656260
auto joan::orig::script_unit_set_15f(std::int32_t a0, std::int16_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_unit_set_15f>(a0, a1)); }  // 0x6562b0
auto joan::orig::script_unit_15f(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_unit_15f>(a0)); }  // 0x656300
auto joan::orig::script_unit_set_name(std::int32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_unit_set_name>(a0, a1)); }  // 0x656350
auto joan::orig::script_unit_name(void* a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b99_08::script_unit_name>(a0, a1)); }  // 0x6563b0
auto joan::orig::units::detach_656410(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b103_13::detach_656410>(a0)); }  // 0x656410
auto joan::orig::script::unit_copy_13d(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::unit_copy_13d>(a0)); }  // 0x6568d0
auto joan::orig::script_unit_135(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_unit_135>(a0)); }  // 0x656940
auto joan::orig::script_unit_set_135(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_unit_set_135>(a0, a1)); }  // 0x656990
auto joan::orig::script::leader_id(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_05::script_leader_id>(a0)); }  // 0x6569f0
auto joan::orig::script_unit_key_alive(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_unit_key_alive>(a0)); }  // 0x656a60
auto joan::orig::script_unit_384(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_04::script_unit_384>(a0)); }  // 0x656af0
auto joan::orig::script_unit_38c(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_04::script_unit_38c>(a0)); }  // 0x656b50
auto joan::orig::unit::guard_check(std::int16_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_06::guard_check>(a0, a1)); }  // 0x656bb0
auto joan::orig::seat::find_free_656dc0_656dc0(std::int16_t a0, std::int16_t a1, const char* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b111_16::find_free_seat>(a0, a1, a2)); }  // 0x656dc0
auto joan::orig::script_hardpoint_x(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b92_05::hardpoint_x>(a0, a1)); }  // 0x657110
auto joan::orig::script_hardpoint_y(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b92_05::hardpoint_y>(a0, a1)); }  // 0x657240
auto joan::orig::script_hardpoint_z(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b92_05::hardpoint_z>(a0, a1)); }  // 0x657370
auto joan::orig::script_unit_group_flag_2f(std::int16_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_unit_group_flag_2f>(a0, a1)); }  // 0x6574a0
auto joan::orig::script_unit_2f_zero(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_unit_2f_zero>(a0)); }  // 0x6575c0
auto joan::orig::script_unit_508b90(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_04::script_unit_508b90>(a0)); }  // 0x657610
auto joan::orig::script_unit_take_slot(std::int32_t a0, std::int32_t a1, const char* a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b103_14::script_unit_take_slot>(a0, a1, a2, a3)); }  // 0x657670
auto joan::orig::script_unit_release_slots(std::int16_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_unit_release_slots>(a0, a1)); }  // 0x657cc0
auto joan::orig::script::unit_join_group(std::uint32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::unit_join_group>(a0, a1)); }  // 0x657fd0
auto joan::orig::script_unit_group_size(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_unit_group_size>(a0)); }  // 0x658160
auto joan::orig::find_unit_in_groups(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_04::find_unit_in_groups>(a0)); }  // 0x6581f0
auto joan::orig::script::group_set_side(std::uint32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::group_set_side>(a0, a1)); }  // 0x6582f0
auto joan::orig::script_units_set_group(std::int32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_units_set_group>(a0, a1)); }  // 0x658420
auto joan::orig::script_unit_spawn_members(std::int32_t a0, const char* a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_unit_spawn_members>(a0, a1, a2)); }  // 0x6584e0
auto joan::orig::script_unit_group_call_41fcc0(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_unit_group_call_41fcc0>(a0)); }  // 0x658650
auto joan::orig::script_unit_group_442820(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_unit_group_442820>(a0)); }  // 0x658730
auto joan::orig::script_unit_group_move(std::int16_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, float a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_unit_group_move>(a0, a1, a2, a3, a4)); }  // 0x658810
auto joan::orig::script::group_follow_leader(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::group_follow_leader>(a0)); }  // 0x658920
auto joan::orig::script_unit_658ad0(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_unit_658ad0>(a0)); }  // 0x658ad0
auto joan::orig::script_units_clear_1c5(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_units_clear_1c5>(a0)); }  // 0x658d90
auto joan::orig::script_units_set_2f(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_units_set_2f>(a0, a1)); }  // 0x658e10
auto joan::orig::script_unit_attach(std::int16_t a0, std::int16_t a1, const char* a2, std::int8_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_unit_attach>(a0, a1, a2, a3)); }  // 0x658ea0
auto joan::orig::script_select_unit(std::int16_t a0, std::int16_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_select_unit>(a0, a1)); }  // 0x658f70
auto joan::orig::script::get_a5b9e8() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a5b9e8>()); }  // 0x659060
auto joan::orig::script_release_group(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b106_18::script_release_group>(a0)); }  // 0x659080
auto joan::orig::units::release_owned(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b106_10::release_owned>(a0)); }  // 0x659210
auto joan::orig::unit::rank_value(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_06::rank_value>(a0)); }  // 0x6592c0
auto joan::orig::script_unit_unit_dist(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_unit_unit_dist>(a0, a1)); }  // 0x659400
auto joan::orig::script_set_outcome(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_set_outcome>(a0)); }  // 0x6594e0
auto joan::orig::script_659550(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b102_13::script_659550>(a0, a1, a2, a3, a4, a5)); }  // 0x659550
auto joan::orig::script::flag_a59298() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::flag_a59298>()); }  // 0x6595e0
auto joan::orig::script_set_range_pair(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_set_range_pair>(a0)); }  // 0x659610
auto joan::orig::script_current_1aa() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_current_1aa>()); }  // 0x6596a0
auto joan::orig::script_a52928_57f990(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b98_07::script_a52928_57f990>(a0, a1, a2)); }  // 0x6596f0
auto joan::orig::script::get_a47bb0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a47bb0>()); }  // 0x659720
auto joan::orig::script::wrap_659740() -> float { return joan::bind::conv<float>(joan::bind::free<&::game_b104_12::wrap_659740>()); }  // 0x659740
auto joan::orig::Calendar::seconds() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b95_07::H::seconds>(this)); }  // 0x659760
auto joan::orig::clock::minutes_mod() -> double { return joan::bind::conv<double>(joan::bind::free<&::game_b111_13::minutes_mod>()); }  // 0x659790
auto joan::orig::script::get_a47bb8() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a47bb8>()); }  // 0x6597d0
auto joan::orig::script::get_a47bbc() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a47bbc>()); }  // 0x6597f0
auto joan::orig::script::get_a47bc0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a47bc0>()); }  // 0x659810
auto joan::orig::script_set_a58faf(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_set_a58faf>(a0)); }  // 0x659830
auto joan::orig::script::call_51c4b0(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::call_51c4b0>(a0)); }  // 0x659860
auto joan::orig::script_set_a48cdc(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_set_a48cdc>(a0)); }  // 0x659890
auto joan::orig::script_set_a591cc(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_set_a591cc>(a0)); }  // 0x6598c0
auto joan::orig::script::get_a591cc() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a591cc>()); }  // 0x6598f0
auto joan::orig::script::clear_a59171() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::clear_a59171>()); }  // 0x659910
auto joan::orig::script::get_a59171() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a59171>()); }  // 0x659940
auto joan::orig::script_clear_a59175() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_clear_a59175>()); }  // 0x659960
auto joan::orig::script::get_a59175() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a59175>()); }  // 0x659990
auto joan::orig::script_clear_a59179() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_clear_a59179>()); }  // 0x6599b0
auto joan::orig::script::get_a59179() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a59179>()); }  // 0x6599e0
auto joan::orig::script_set_a591d0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_set_a591d0>(a0)); }  // 0x659a00
auto joan::orig::script_set_a591d4(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_set_a591d4>(a0)); }  // 0x659a30
auto joan::orig::script_add_a591d8(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_add_a591d8>(a0)); }  // 0x659a60
auto joan::orig::script_set_a591d8(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_set_a591d8>(a0)); }  // 0x659a90
auto joan::orig::script::get_a591d8() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a591d8>()); }  // 0x659ac0
auto joan::orig::script::clear_a5918d() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::clear_a5918d>()); }  // 0x659ae0
auto joan::orig::script::get_a591a9() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a591a9>()); }  // 0x659b10
auto joan::orig::script_clear_a591a9() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_clear_a591a9>()); }  // 0x659b30
auto joan::orig::script::get_a591b6() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a591b6>()); }  // 0x659b60
auto joan::orig::script_goldbags_to_barrels() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_goldbags_to_barrels>()); }  // 0x659b80
auto joan::orig::script::get_a591be() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a591be>()); }  // 0x659ca0
auto joan::orig::script::get_a591c2() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a591c2>()); }  // 0x659cc0
auto joan::orig::script_set_a591c6(std::int8_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_set_a591c6>(a0)); }  // 0x659ce0
auto joan::orig::script_set_a591ba(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_set_a591ba>(a0)); }  // 0x659d10
auto joan::orig::script::get_a591ba() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a591ba>()); }  // 0x659d40
auto joan::orig::script::clear_a591c8() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::clear_a591c8>()); }  // 0x659d60
auto joan::orig::script::get_a591c8() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a591c8>()); }  // 0x659d90
auto joan::orig::script_counter_post_inc() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_counter_post_inc>()); }  // 0x659db0
auto joan::orig::script::call_4cdd40(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::call_4cdd40>(a0)); }  // 0x659de0
auto joan::orig::script_a58fa7_4cdc40(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_a58fa7_4cdc40>(a0)); }  // 0x659e10
auto joan::orig::script::get_a591ad() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a591ad>()); }  // 0x659e40
auto joan::orig::script::get_a591b1() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a591b1>()); }  // 0x659e60
auto joan::orig::script::get_a5918d() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a5918d>()); }  // 0x659e80
auto joan::orig::script_clear_a59199() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_clear_a59199>()); }  // 0x659ea0
auto joan::orig::script::get_a59199() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a59199>()); }  // 0x659ed0
auto joan::orig::script_clear_a591a5() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_clear_a591a5>()); }  // 0x659ef0
auto joan::orig::script::get_a591a5() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a591a5>()); }  // 0x659f20
auto joan::orig::script_set_stat(const char* a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_set_stat>(a0, a1, a2)); }  // 0x659f40
auto joan::orig::script_stat_table(const char* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_stat_table>(a0, a1)); }  // 0x65a010
auto joan::orig::script::set_stat(const char* a0, std::uint32_t a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::set_stat>(a0, a1, a2)); }  // 0x65a0e0
auto joan::orig::script_attribute(const char* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_attribute>(a0, a1)); }  // 0x65a1b0
auto joan::orig::script::get_a59294() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a59294>()); }  // 0x65a280
auto joan::orig::script::set_a59294(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::set_a59294>(a0)); }  // 0x65a2a0
auto joan::orig::script::wrap_65a2d0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::wrap_65a2d0>()); }  // 0x65a2d0
auto joan::orig::script_a5b9e8_432300(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_a5b9e8_432300>(a0)); }  // 0x65a2f0
auto joan::orig::script_42e040() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_42e040>()); }  // 0x65a320
auto joan::orig::script::wrap_65a350() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::wrap_65a350>()); }  // 0x65a350
auto joan::orig::script::wrap_65a370() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::wrap_65a370>()); }  // 0x65a370
auto joan::orig::script_table_set_byte(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_table_set_byte>(a0, a1)); }  // 0x65a390
auto joan::orig::script::objective_467300(std::uint32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::objective_467300>(a0, a1)); }  // 0x65a3c0
auto joan::orig::script::objective_update(std::uint32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::objective_update>(a0, a1)); }  // 0x65a400
auto joan::orig::script::objective_467390(std::uint32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::objective_467390>(a0, a1)); }  // 0x65a460
auto joan::orig::script_objective_state(std::int32_t a0, const char* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_objective_state>(a0, a1)); }  // 0x65a4a0
auto joan::orig::script_table_word(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_table_word>(a0)); }  // 0x65a670
auto joan::orig::script_table_set_word(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_table_set_word>(a0, a1)); }  // 0x65a6a0
auto joan::orig::script::set_a592a8_word(std::int32_t a0, std::uint16_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::set_a592a8_word>(a0, a1)); }  // 0x65a6e0
auto joan::orig::script_objective_set(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_objective_set>(a0, a1)); }  // 0x65a720
auto joan::orig::script_table_word_aa(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_table_word_aa>(a0)); }  // 0x65a7e0
auto joan::orig::script_table_set(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_table_set>(a0, a1)); }  // 0x65a810
auto joan::orig::script::objective_set_15(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b100_13::objective_set_15>(a0, a1)); }  // 0x65a840
auto joan::orig::script::call_472000(std::uint32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::call_472000>(a0, a1)); }  // 0x65a870
auto joan::orig::script::call_4720d0(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::call_4720d0>(a0)); }  // 0x65a8a0
auto joan::orig::script_65a8d0(std::uint32_t a0, std::uint8_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_65a8d0>(a0, a1)); }  // 0x65a8d0
auto joan::orig::script::call_472220(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::call_472220>(a0)); }  // 0x65a900
auto joan::orig::script::call_472540() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::call_472540>()); }  // 0x65a930
auto joan::orig::script_set_bit(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_set_bit>(a0, a1)); }  // 0x65a960
auto joan::orig::script_65a990(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_65a990>(a0)); }  // 0x65a990
auto joan::orig::script_a58f90_set_bit(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_a58f90_set_bit>(a0, a1)); }  // 0x65a9c0
auto joan::orig::script::call_4724c0(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::call_4724c0>(a0)); }  // 0x65a9f0
auto joan::orig::script_set_19a(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_set_19a>(a0)); }  // 0x65aa20
auto joan::orig::script::current_19a() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b100_13::current_19a>()); }  // 0x65aa80
auto joan::orig::script::call_4cd570(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::call_4cd570>(a0)); }  // 0x65aad0
auto joan::orig::script::call_4cde30(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_13::call_4cde30>(a0)); }  // 0x65ab10
auto joan::orig::script_list_a58fa7_call(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_list_a58fa7_call>(a0)); }  // 0x65ab50
auto joan::orig::script_copy_rect_a58f97() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_copy_rect_a58f97>()); }  // 0x65ab90
auto joan::orig::script_store_rect() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_store_rect>()); }  // 0x65aca0
auto joan::orig::script_set_flag_a501da(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_set_flag_a501da>(a0)); }  // 0x65adb0
auto joan::orig::camera::nudge_6cb9e0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b108_06::camera_nudge>(a0)); }  // 0x65ade0
auto joan::orig::spawn_fx_65ae60(std::int8_t a0, std::int8_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::int8_t a8) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b107_17::spawn_fx_65ae60>(a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x65ae60
auto joan::orig::menu_text_65aef0(char a0, char a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, char a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b107_06::menu_text_65aef0>(a0, a1, a2, a3, a4, a5, a6)); }  // 0x65aef0
auto joan::orig::script_state_busy() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_state_busy>()); }  // 0x65af50
auto joan::orig::Shop::state() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_04::H::get_f_65afb0>(this)); }  // 0x65afb0
auto joan::orig::script::wrap_65afd0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::wrap_65afd0>()); }  // 0x65afd0
auto joan::orig::script::fn_65aff0(char a0, char a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b108_08::fn_65aff0>(a0, a1)); }  // 0x65aff0
auto joan::orig::forward_6cd720(std::int8_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b107_20::forward_6cd720>(a0, a1, a2, a3, a4)); }  // 0x65b030
auto joan::orig::fx_at_cell_65b070(char a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b107_14::fx_at_cell_65b070>(a0, a1, a2, a3, a4, a5, a6)); }  // 0x65b070
auto joan::orig::game_state_is_7() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_04::game_state_is_7>()); }  // 0x65b0e0
auto joan::orig::game_flag_744() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_01::game_flag_744>()); }  // 0x65b130
auto joan::orig::script_type_5270b0(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b97_07::script_type_5270b0>(a0)); }  // 0x65b160
auto joan::orig::script_type_activate(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_type_activate>(a0)); }  // 0x65b190
auto joan::orig::script_type_51fd50(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b98_07::script_type_51fd50>(a0)); }  // 0x65b200
auto joan::orig::script::soldier_51fdd0(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b99_09::soldier_51fdd0>(a0)); }  // 0x65b240
auto joan::orig::unit::range_for_level(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_06::range_for_level>(a0, a1)); }  // 0x65b280
auto joan::orig::script_unit_135_pct(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_unit_135_pct>(a0)); }  // 0x65b500
auto joan::orig::units_lookup(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_04::units_lookup>(a0)); }  // 0x65b570
auto joan::orig::script_object_49f4b0(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b96_07::script_object_49f4b0>(a0)); }  // 0x65b5b0
auto joan::orig::script_a501e0_4ce900(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b98_07::script_a501e0_4ce900>(a0)); }  // 0x65b5f0
auto joan::orig::script_object_name_differs(std::int32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_object_name_differs>(a0, a1)); }  // 0x65b630
auto joan::orig::unit::is_kind10(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_06::is_kind10>(a0)); }  // 0x65b6a0
auto joan::orig::object_is_type1(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_04::object_is_type1>(a0)); }  // 0x65b700
auto joan::orig::object_is_type4(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_04::object_is_type4>(a0)); }  // 0x65b760
auto joan::orig::script_4cad00(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b97_07::script_4cad00>(a0)); }  // 0x65b7c0
auto joan::orig::unit::distance_to(std::int16_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_06::distance_to>(a0, a1, a2, a3)); }  // 0x65b800
auto joan::orig::script::soldier_set_374(std::int16_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::soldier_set_374>(a0, a1)); }  // 0x65b8c0
auto joan::orig::script_unit_374(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_unit_374>(a0)); }  // 0x65b950
auto joan::orig::unit_refresh_items(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_04::unit_refresh_items>(a0)); }  // 0x65b9b0
auto joan::orig::script::mount_65bce0(std::int16_t a0, std::int16_t a1, char a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b108_08::mount_65bce0>(a0, a1, a2)); }  // 0x65bce0
auto joan::orig::save_list::add(const char* a0, const char* a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b83_04::save_add>(a0, a1, a2)); }  // 0x65bed0
auto joan::orig::unit_set_mood(std::int32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_01::unit_set_mood>(a0, a1)); }  // 0x65c050
auto joan::orig::script_unit_1ae(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_unit_1ae>(a0)); }  // 0x65c0d0
auto joan::orig::script::soldier_sum_1b0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::soldier_sum_1b0>(a0)); }  // 0x65c130
auto joan::orig::script_unit_124(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_unit_124>(a0)); }  // 0x65c1a0
auto joan::orig::script::get_a596c4() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a596c4>()); }  // 0x65c200
auto joan::orig::script_add_a596c4(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_add_a596c4>(a0)); }  // 0x65c220
auto joan::orig::script_unit_523190(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_unit_523190>(a0, a1)); }  // 0x65c250
auto joan::orig::script_unit_set_type(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_unit_set_type>(a0, a1)); }  // 0x65c2e0
auto joan::orig::script_unit_1c1_word2(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_unit_1c1_word2>(a0)); }  // 0x65c3a0
auto joan::orig::script::call_761680(const char* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::call_761680>(a0, a1, a2, a3)); }  // 0x65c420
auto joan::orig::script_a48a38_byte() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b98_07::script_a48a38_byte>()); }  // 0x65c470
auto joan::orig::script_set_a6a254(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_set_a6a254>(a0)); }  // 0x65c4a0
auto joan::orig::script_a6a254_below() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_a6a254_below>()); }  // 0x65c4d0
auto joan::orig::script_set_a596ab(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_set_a596ab>(a0)); }  // 0x65c500
auto joan::orig::script_post_event_65c530(std::uint32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b111_19::script_post_event>(a0, a1)); }  // 0x65c530
auto joan::orig::unit_order_65c5f0(const char* a0, std::int32_t a1, std::int32_t a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b108_12::f_65c5f0>(a0, a1, a2, a3)); }  // 0x65c5f0
auto joan::orig::script_icon_65c770(const char* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b108_02::script_icon>(a0, a1, a2, a3)); }  // 0x65c770
auto joan::orig::script_unit_call_510030(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_unit_call_510030>(a0, a1)); }  // 0x65c870
auto joan::orig::unit::idle(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b90_06::unit_idle>(a0)); }  // 0x65c8d0
auto joan::orig::script_unit_name_check(std::int32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_unit_name_check>(a0, a1)); }  // 0x65c930
auto joan::orig::script_time_since() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_time_since>()); }  // 0x65c9b0
auto joan::orig::script::soldier_5229a0(std::int32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::soldier_5229a0>(a0, a1)); }  // 0x65c9e0
auto joan::orig::script_unit_65ca40(const char* a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b102_13::script_unit_65ca40>(a0, a1)); }  // 0x65ca40
auto joan::orig::script_unit_call_510170(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b103_14::script_unit_call_510170>(a0, a1)); }  // 0x65cac0
auto joan::orig::script_unit_65cb20(std::uint32_t a0, std::int32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b102_13::script_unit_65cb20>(a0, a1, a2)); }  // 0x65cb20
auto joan::orig::script_unit_count_allies(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_unit_count_allies>(a0)); }  // 0x65cb80
auto joan::orig::script_count_allies(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_count_allies>(a0)); }  // 0x65cd00
auto joan::orig::message_65cea0(const char* a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b108_04::message_65cea0>(a0, a1)); }  // 0x65cea0
auto joan::orig::msg::post_65cf00(const char* a0, const char* a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b108_09::post_65cf00>(a0, a1, a2)); }  // 0x65cf00
auto joan::orig::script_call_65cf40(const char* a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b108_10::script_call_65cf40>(a0, a1, a2)); }  // 0x65cf40
auto joan::orig::script_name_set_19(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b97_07::script_name_set_19>(a0, a1)); }  // 0x65cf80
auto joan::orig::script_unit_65cfd0(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b102_13::script_unit_65cfd0>(a0, a1)); }  // 0x65cfd0
auto joan::orig::script::fn_65d070(std::int32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b108_08::fn_65d070>(a0, a1)); }  // 0x65d070
auto joan::orig::script_set_unit_light(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b92_05::script_set_unit_light>(a0, a1)); }  // 0x65d0d0
auto joan::orig::script_unit_apply_group(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_unit_apply_group>(a0, a1)); }  // 0x65d350
auto joan::orig::script_unit_65d450(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_unit_65d450>(a0, a1, a2)); }  // 0x65d450
auto joan::orig::script::soldier_near_target(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::soldier_near_target>(a0)); }  // 0x65d560
auto joan::orig::unit::marker_distance_65d6f0(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_b111_08::marker_distance>(a0, a1, a2)); }  // 0x65d6f0
auto joan::orig::script_unit_3c9(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_unit_3c9>(a0)); }  // 0x65d850
auto joan::orig::script_group_size_of(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_group_size_of>(a0)); }  // 0x65d8a0
auto joan::orig::script_place_effect(const char* a0, std::int16_t a1, std::int16_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_place_effect>(a0, a1, a2)); }  // 0x65d9c0
auto joan::orig::script_spawn_effect(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_spawn_effect>(a0, a1, a2)); }  // 0x65da50
auto joan::orig::unit_order_65daf0(std::int32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b105_18::unit_order_65daf0>(a0, a1)); }  // 0x65daf0
auto joan::orig::unit_kick_65dba0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b107_08::unit_kick>(a0)); }  // 0x65dba0
auto joan::orig::units_order_kind_65dc20(std::int32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b105_18::units_order_kind_65dc20>(a0, a1, a2)); }  // 0x65dc20
auto joan::orig::units_sweep_65dcf0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b107_06::units_sweep_65dcf0>(a0)); }  // 0x65dcf0
auto joan::orig::script_target_hardpoint_dist(std::int32_t a0) -> double { return joan::bind::conv<double>(joan::bind::free<&::game_b92_05::script_target_hardpoint_dist>(a0)); }  // 0x65ddb0
auto joan::orig::script_unit_388(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_04::script_unit_388>(a0)); }  // 0x65dfa0
auto joan::orig::script_object_65dff0(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_object_65dff0>(a0, a1)); }  // 0x65dff0
auto joan::orig::script_bind_input(void* a0, const char* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b103_14::script_bind_input>(a0, a1)); }  // 0x65e0a0
auto joan::orig::script_set_a50522(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_set_a50522>(a0)); }  // 0x65e7d0
auto joan::orig::script::call_6dfe10(char a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::call_6dfe10>(a0)); }  // 0x65e810
auto joan::orig::script_6dff10() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_6dff10>()); }  // 0x65e850
auto joan::orig::script_set_a50526(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_set_a50526>(a0)); }  // 0x65e880
auto joan::orig::script::call_6e00b0(char a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::call_6e00b0>(a0)); }  // 0x65e8c0
auto joan::orig::script_a50518_6e01d0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_a50518_6e01d0>()); }  // 0x65e900
auto joan::orig::script_object_set_7e(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_object_set_7e>(a0, a1)); }  // 0x65e930
auto joan::orig::script_unit_set_3c0(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_unit_set_3c0>(a0, a1)); }  // 0x65e9a0
auto joan::orig::script_object_a2(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_object_a2>(a0)); }  // 0x65ea00
auto joan::orig::script_object_height_65ea50(std::int32_t a0) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_b100_14::script_object_height>(a0)); }  // 0x65ea50
auto joan::orig::script::object_height(std::uint32_t a0) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_b99_09::object_height>(a0)); }  // 0x65ead0
auto joan::orig::script_object_height(std::int32_t a0) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_b95_07::script_object_height>(a0)); }  // 0x65eb50
auto joan::orig::script_set_6b8260(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b103_14::script_set_6b8260>(a0)); }  // 0x65ebd0
auto joan::orig::script_a4f381_call(const char* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_a4f381_call>(a0, a1)); }  // 0x65ec90
auto joan::orig::script::call_71d1e0(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b99_09::call_71d1e0>(a0)); }  // 0x65ecf0
auto joan::orig::script::get_a5b9f8() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_12::get_a5b9f8>()); }  // 0x65ed50
auto joan::orig::script_state_is_4() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_state_is_4>()); }  // 0x65ed70
auto joan::orig::script_set_word_a4f3ac(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_set_word_a4f3ac>(a0)); }  // 0x65eda0
auto joan::orig::script_a4f381_set_11(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_a4f381_set_11>(a0)); }  // 0x65edd0
auto joan::orig::Hud::set_word_11(std::int32_t a0) -> void { joan::bind::member<&::game_b96_07::H::set_word_11>(this, a0); }  // 0x65ee00
auto joan::orig::script::set_a4f858(std::uint8_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::set_a4f858>(a0)); }  // 0x65ee30
auto joan::orig::f_65ee60() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b108_12::f_65ee60>()); }  // 0x65ee60
auto joan::orig::script_not_a53cb0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_not_a53cb0>()); }  // 0x65eea0
auto joan::orig::script_flush_436190() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_flush_436190>()); }  // 0x65eee0
auto joan::orig::script_group_transfer(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_group_transfer>(a0, a1, a2)); }  // 0x65ef40
auto joan::orig::script_unit_heading_to(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b92_05::script_unit_heading_to>(a0, a1)); }  // 0x65f0f0
auto joan::orig::formation::assign_slot(std::int16_t a0, std::int16_t a1, const char* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b104_12::assign_slot>(a0, a1, a2)); }  // 0x65f160
auto joan::orig::script_flag(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_04::script_flag>(a0)); }  // 0x65f600
auto joan::orig::script_set_byte_a5ba64(std::uint8_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_set_byte_a5ba64>(a0)); }  // 0x65f670
auto joan::orig::script::flush_a50088() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::flush_a50088>()); }  // 0x65f6a0
auto joan::orig::script_a4f381_7196e0(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_a4f381_7196e0>(a0, a1, a2, a3, a4)); }  // 0x65f740
auto joan::orig::script_a4f381_719770(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_a4f381_719770>(a0, a1, a2, a3)); }  // 0x65f780
auto joan::orig::script_a4f381_7197f0(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_a4f381_7197f0>(a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x65f7c0
auto joan::orig::script_call_7198d0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_call_7198d0>(a0)); }  // 0x65f820
auto joan::orig::script::set_side(const char* a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::set_side>(a0, a1)); }  // 0x65f850
auto joan::orig::script_65f910(std::int16_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_13::script_65f910>(a0)); }  // 0x65f910
auto joan::orig::script_not_a46040() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::script_not_a46040>()); }  // 0x65f950
auto joan::orig::script_unit_set_3c1_65f980(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07::script_unit_set_3c1>(a0, a1)); }  // 0x65f980
auto joan::orig::script_unit_set_3c1(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_unit_set_3c1>(a0, a1)); }  // 0x65f9e0
auto joan::orig::script_unit_set_30(std::int32_t a0, std::int8_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::script_unit_set_30>(a0, a1)); }  // 0x65fb50
auto joan::orig::script_set_a596c8(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_07::script_set_a596c8>(a0)); }  // 0x65fba0
auto joan::orig::script::get_a591c7() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b104_12::get_a591c7>()); }  // 0x65fbd0
auto joan::orig::script_name_unused(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_07::script_name_unused>(a0)); }  // 0x65fbf0
auto joan::orig::script_set_byte_a5d386(std::uint8_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::script_set_byte_a5d386>(a0)); }  // 0x65fc90
auto joan::orig::script_65fcc0(std::int8_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b102_13::script_65fcc0>(a0)); }  // 0x65fcc0
auto joan::orig::script::soldier_set_358(std::int32_t a0, std::uint8_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_09::soldier_set_358>(a0, a1)); }  // 0x65fd10
auto joan::orig::ScriptNode::set_type(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_04::H::set_c>(this, a0)); }  // 0x660ff0
auto joan::orig::group_order(std::int32_t a0, std::int32_t a1, std::int8_t a2, std::int8_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_01::group_order>(a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x661340
auto joan::orig::group_release(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_01::group_release>(a0)); }  // 0x6618f0
auto joan::orig::script_send_order(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b92_05::script_send_order>(a0, a1, a2, a3, a4, a5, a6)); }  // 0x6619d0
auto joan::orig::script_unit_list_remove(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b98_07::script_unit_list_remove>(a0, a1)); }  // 0x661ca0
auto joan::orig::random_table_lookup(const char* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_07::random_table_lookup>(a0, a1)); }  // 0x661e10
