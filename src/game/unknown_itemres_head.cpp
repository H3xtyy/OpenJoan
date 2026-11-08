// game/unknown_itemres_head.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/itemres_head.cpp; re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their
// original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::call::cdecl_call;

using i32 = std::int32_t;

using joan::img::obj;

using i16 = std::int16_t;

}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

struct H {
    // ------------------------------------------------------------------------ 0x0049e8d0
    void* ctor_j() {
        joan::orig::dynarray_b(&jo::self<jo::Units>(this)->list)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0, 0);  // 0x005856a0 (pointers: 4 on x86)
        jo::self<jo::Units>(this)->vtable = img::obj(d::vt_Obj_9d4090);
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("ObjJ::ctor", 0x0049e8d0, &H::ctor_j);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

struct H {
    // ------------------------------------------------------------------ 0x0049e910
    u32 dtor_49e910() {
        jo::self<jo::Units>(this)->vtable = img::obj(d::vt_Obj_9d4090);  // vtable
        joan::orig::as_item_list(this)->clear();  // 0x0049e9c0
        // (the original's this != 0 test before taking this+4 cannot fail for a live object)
        void* const list = &jo::as_units(this)->list;
        return thiscall<u32>(fn::dtor_DynArrayB, list);  // 0x00585710
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Obj::~Obj_49e910", 0x0049e910, &H::dtor_49e910);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

struct H {
    void nop0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("Obj::nop_49e9a0", 0x0049e9a0, &H::nop0);
B("Obj::nop_49f550", 0x0049f550, &H::nop0);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
namespace {
namespace game_b84_06 {

JOAN_COUNTER(c_clear1, "ItemList::clear");
JOAN_COUNTER(c_findname, "ItemList::find_by_name");

using joan::orig::helpers::stricmp_;
using joan::orig::helpers::count_of;  // 0x00417ad0
void remove_at(const void* list, std::int32_t i) { joan::orig::dynarray_b(list)->linkout(joan::orig::arg::i32(i)); }  // 0x005858c0
// void callee; its one use is ItemList::clear's result, which both callers (game.cpp, dtor_49e910) discard.
std::uint32_t compact(const void* list) {
    joan::orig::as_sound(list)->reset_585d20();  // 0x00585d20
    return 0;
}

struct H {
    // ------------------------------------------------------------------ 0x0049e9c0
    std::uint32_t item_list_clear() {
        c_clear1.hit();
        void* const list = &jo::as_units(this)->list;
        for (std::int32_t i = count_of(list); i >= 1; --i) {
            if (joan::orig::as_units(this)->list.empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00425d50
            // (dead "if (i == 15) i = i;")
            void* const e = joan::orig::as_units(this)->list.ptr_at(joan::orig::arg::i32(i));  // 0x00425d20
            if (e != nullptr) {
                const std::uintptr_t dtor = static_cast<const std::uintptr_t*>(jo::as_map_object(e)->vtable)[0];  // vtable[0]: scalar deleting dtor
                thiscall<void*>(dtor, e, static_cast<std::uint32_t>(1));
            }
            remove_at(list, i);
        }
        return compact(list);
    }

    // ------------------------------------------------------------------ 0x0049f420
    std::int32_t find_by_name(const char* name) {
        c_findname.hit();
        for (std::int32_t i = count_of(&jo::as_units(this)->list); i > 0; --i) {
            if (joan::orig::as_units(this)->list.empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00425d50
            std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(this)->list.ptr_at(joan::orig::arg::i32(i)));  // 0x00425d20
            if (stricmp_(jo::as_map_object(e)->name, name) == 0) return i;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("ItemList::clear", 0x0049e9c0, &H::item_list_clear);
B("ItemList::find_by_name", 0x0049f420, &H::find_by_name);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- from modules/game_b82_16.cpp (game/b82_16)
namespace {
namespace game_b82_16 {

using joan::img::str;

using joan::orig::helpers::game_new;  // operator new

JOAN_COUNTER(c_create, "Units::create");

struct H {
    // ------------------------------------------------------------------ 0x0049ea90
    std::uint32_t units_create(std::uint32_t raw_id) {
        c_create.hit();
        std::uint8_t* const rec =
            joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<std::int16_t>(raw_id))));  // 0x00425ce0
        std::uint32_t size;  // x86: 0xdb / 0xce / 0xce / 0xcd / 0xc2 / 0xef / 0x107 / 0xc2 / 0xce / 0xc2 (MapObject + the derived fields)
        std::uintptr_t ctor;
        const std::int16_t kind = jo::view<std::int16_t>(jo::as_item_def(rec)->class_id);
        bool copy_name = false;
        if (kind == 1) {
            size = sizeof(jo::MapObject) + 0x19, ctor = fn::code_value(fn::Order19_ctor);  // 0x0048c250
        } else if (kind == 2) {
            size = sizeof(jo::Order0c_a), ctor = fn::code_value(fn::Order0c_a_ctor);  // 0x00434380
        } else if (kind == 3) {
            size = sizeof(jo::Order0c_c), ctor = fn::code_value(fn::Order0c_c_ctor);  // 0x0048ea70
        } else if (kind == 0xa) {
            size = sizeof(jo::Order0b), ctor = fn::code_value(fn::Order0b_ctor);  // 0x00489ea0
        } else if (kind == 0x12) {
            size = sizeof(jo::Order00), ctor = fn::code_value(fn::Order00_ctor);  // 0x0048f740
        } else if (kind == 4) {
            size = sizeof(jo::Order2d), ctor = fn::code_value(fn::Order2d_ctor);  // 0x00432210
        } else if (kind == 0xd) {
            size = sizeof(jo::Order45), ctor = fn::code_value(fn::Order45_ctor);  // 0x00434040
        } else if (kind == 0x11) {
            size = sizeof(jo::MapObject), ctor = fn::code_value(fn::Obj_ctor_416a50);  // 0x00416a50
        } else if (kind == 0xe) {
            size = sizeof(jo::Order0c_b), ctor = fn::code_value(fn::Order0c_b_ctor);  // 0x004352b0
        } else if (kind == 0xf || kind == 0x13) {
            size = sizeof(jo::MapObject), ctor = fn::code_value(fn::Obj_ctor_416a50);  // 0x00416a50
            copy_name = true;
        } else {
            size = sizeof(jo::MapObject), ctor = fn::code_value(fn::Obj_ctor_416a50);  // 0x00416a50
        }
        void* const mem = game_new(size);
        void* unit = mem != nullptr ? thiscall<void*>(ctor, mem) : nullptr;
        thiscall(static_cast<const std::uintptr_t*>(jo::as_map_object(unit)->vtable)[8 / 4], unit, raw_id);  // virtual init(id), vtable +0x8
        joan::orig::dynarray_b(&jo::as_units(this)->list)->linkin(&unit);                             // 0x005857a0 add
        const std::int32_t n = joan::orig::as_dynarray(&jo::as_units(this)->list)->cursor();  // 0x00417520 count
        jo::as_map_object(unit)->index = static_cast<std::int16_t>(n);
        if (copy_name) std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_map_object(unit)->name)), joan::orig::arg::str(jo::as_item_def(rec)->file_name));  // strcpy
        if (jo::view<std::int16_t>(jo::as_item_def(rec)->class_id) == 0xb) {
            const std::int32_t k = joan::orig::as_dynarray(&jo::as_units(this)->list)->cursor();
            jo::view<std::int32_t>(jo::as_map_object(joan::orig::as_units(this)->list.ptr_at(joan::orig::arg::i32(k)))->interact_trigger) = 0x200;  // 0x00425d20
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_item_def(rec)->is_soldier()) != 0)  // 0x004a4050
            joan::orig::as_list(obj(d::g_a50530))->add_425d20(joan::orig::arg::i32(joan::orig::as_dynarray(&jo::as_units(this)->list)->cursor()));  // 0x004afa80
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(&jo::as_units(this)->list)->cursor());
    }
};

#define B16(name, addr, fn) JOAN_REPLACE("game/b82_16", name, addr, fn)
B16("Units::create", 0x0049ea90, &H::units_create);
#undef B16

}  // namespace game_b82_16
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

struct Obj {};

JOAN_COUNTER(c_units_remove, "Units::remove");

void list_remove(void* list, std::int32_t i) { joan::orig::dynarray_b(list)->linkout(joan::orig::arg::i32(i)); }  // 0x005858c0

struct H {
    // 0x0049f370 Units::remove(id): the unit's type (+6) unregistered from 0xa50530 when
    // known, the unit deleted (virtual slot 0), the slot removed from +4.
    void units_remove(std::int32_t id) {
        c_units_remove.hit();
        std::uint8_t* const u = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(this)->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
        void* const types = img::obj(d::g_a50530);
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_types(types)->empty_at(static_cast<std::int32_t>(jo::as_map_object(u)->portrait_unit))) == 0)  // 0x00485410
            joan::orig::as_types(types)->remove(static_cast<std::int32_t>(jo::as_map_object(u)->portrait_unit));  // 0x004afaf0
        if (u != nullptr) {
            using Virtual = void* (Obj::*)(std::uint32_t);
            JOAN_MEMBER_POINTER_CHECK(Virtual);
            Virtual method{};
            std::memcpy(&method, &joan::call::vtable_of(u)[0], sizeof(void*));
            (reinterpret_cast<Obj*>(u)->*method)(1u);
        }
        list_remove(&jo::as_units(this)->list, id);
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("Units::remove", 0x0049f370, &H::units_remove);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {


using joan::orig::helpers::stricmp_;
i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0
// Model record table 0xa58f50 (word index).
u8* model_rec(i16 idx) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(idx)))); }  // 0x00425ce0

JOAN_COUNTER(c_group_find, "Group::find_name");

struct H {

    // ------------------------------------------------------------------ 0x0049f4b0
    i32 group_find(const char* name) {
        c_group_find.hit();
        for (i32 i = list_count(&jo::as_units(this)->list); i > 0; --i) {
            if (joan::orig::as_units(this)->list.empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00425d50
            u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_units(this)->list.ptr_at(joan::orig::arg::i32(i)));  // 0x00425d20
            u8* const rec = model_rec(jo::as_map_object(m)->type_id);
            if (stricmp_(jo::as_item_def(rec)->name, name) == 0) return i;
        }
        return 0;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Group::find_name", 0x0049f4b0, &H::group_find);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ItemList::clear() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b84_06::H::item_list_clear>(this)); }  // 0x49e9c0
auto joan::orig::Units::create_49ea90(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_16::H::units_create>(this, a0)); }  // 0x49ea90
auto joan::orig::Units::remove_49f370(std::int32_t a0) -> void { joan::bind::member<&::game_leaves81::H::units_remove>(this, a0); }  // 0x49f370
auto joan::orig::ItemList::find_by_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_06::H::find_by_name>(this, a0)); }  // 0x49f420
auto joan::orig::Group::find_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_02::H::group_find>(this, a0)); }  // 0x49f4b0
