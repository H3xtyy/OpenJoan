// game/unknown_mglist.cpp: functions whose original source file is unknown (formerly game/_unplaced/mglist.cpp;
// re/attribution.csv: between ww/lightsys.cpp and ww/modelres.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cmath>
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../canvas_api.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {


using joan::call::thiscall;

using i32 = std::int32_t;


using joan::img::obj;

}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u8 = std::uint8_t;
using u32 = std::uint32_t;


struct H {
    // ------------------------------------------------------------------ 0x004af910
    void* ctor_4af910() {
        joan::orig::dynarray_b(&jo::self<jo::MgList>(this)->items)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0, 0);  // 0x005856a0 (pointers: 4 on x86)
        jo::self<jo::MgList>(this)->vtable = img::obj(d::vt_List);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Obj::ctor_4af910", 0x004af910, &H::ctor_4af910);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x004af950
    u32 dtor_4af950() {
        jo::self<jo::MgList>(this)->vtable = img::obj(d::vt_List);
        joan::orig::as_list(this)->clear_4afa00();                                            // 0x004afa00
        return thiscall<u32>(fn::dtor_DynArrayB, &jo::self<jo::MgList>(this)->items);     // 0x00585710
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Obj::~Obj_4af950", 0x004af950, &H::dtor_4af950);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

struct H {
    void nop0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("Obj::nop_4af9e0", 0x004af9e0, &H::nop0);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
namespace {
namespace game_b84_06 {

JOAN_COUNTER(c_clear4, "List::clear_4afa00");

using joan::orig::helpers::count_of;  // 0x00417ad0
void remove_at(const void* list, std::int32_t i) { joan::orig::dynarray_b(list)->linkout(joan::orig::arg::i32(i)); }  // 0x005858c0
std::uint32_t compact(const void* list) { joan::orig::as_sound(list)->reset_585d20(); return 0; }  // 0x00585d20 (void; clear_4afa00's result is unused)

struct H {
    // ------------------------------------------------------------------ 0x004afa00
    std::uint32_t clear_4afa00() {
        c_clear4.hit();
        void* const list = &jo::self<jo::MgList>(this)->items;
        for (std::int32_t i = count_of(list); i >= 1; --i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_types(this)->empty_at(joan::orig::arg::i32(i))) == 0) remove_at(list, i);  // 0x00485410
        }
        return compact(list);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("List::clear_4afa00", 0x004afa00, &H::clear_4afa00);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

struct H {
    // ------------------------------------------------------------------ 0x004afa80
    std::int32_t list_add(std::int32_t arg) {
        void* item = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(arg));  // 0x00425d20
        joan::orig::dynarray_b(&jo::self<jo::MgList>(this)->items)->linkin(static_cast<void*>(&item));  // 0x005857a0
        jo::as_map_object(item)->portrait_unit = joan::orig::arg::as<std::int16_t>(joan::orig::as_dynarray(&jo::self<jo::MgList>(this)->items)->cursor());  // 0x00417520
        return joan::orig::as_dynarray(&jo::self<jo::MgList>(this)->items)->cursor();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("List::add_425d20", 0x004afa80, &H::list_add);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

struct H {
    std::uint32_t remove_4afaf0(std::int32_t i) { return thiscall<std::uint32_t>(fn::linkout_5858c0, &jo::self<jo::MgList>(this)->items, i); }  // 0x005858c0 (this: the MgList 0x00a50530)
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Types::remove", 0x004afaf0, &H::remove_4afaf0);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

struct Obj {};

using u8 = std::uint8_t;
using i16 = std::int16_t;

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

JOAN_COUNTER(c_update_items, "List::update_items");

struct H {
    // ------------------------------------------------------------------------ 0x004afb20
    void update_items() {
        c_update_items.hit();
        void* const list = &jo::self<jo::MgList>(this)->items;
        for (i32 i = joan::orig::as_dynarray(list)->size(); i >= 1; --i) {  // 0x00417ad0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (e == nullptr) continue;
            if (virtual_call<i32>(e, 0x44) != 0)
                joan::orig::as_units(obj(d::g_items))->remove_49f370(static_cast<i32>(jo::as_map_object(e)->index));  // 0x0049f370
            else
                virtual_call<void>(e, 0xc);
        }
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("List::update_items", 0x004afb20, &H::update_items);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::List::clear_4afa00() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b84_06::H::clear_4afa00>(this)); }  // 0x4afa00
auto joan::orig::List::add_425d20(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::list_add>(this, a0)); }  // 0x4afa80
auto joan::orig::Types::remove(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves82b::H::remove_4afaf0>(this, a0)); }  // 0x4afaf0
