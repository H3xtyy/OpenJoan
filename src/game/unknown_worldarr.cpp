// game/unknown_worldarr.cpp: functions whose original source file is unknown (formerly game/_unplaced/worldarr.cpp;
// re/attribution.csv: between ww/world_s.cpp and ww/ilib/angle.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
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

using u32 = std::uint32_t;

using i32 = std::int32_t;

using joan::call::thiscall;

using joan::call::cdecl_call;

using i16 = std::int16_t;

using joan::img::obj;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ 0x00560f90
    void* ctor_560f90() {
        joan::orig::dynarray_b(this)->construct(static_cast<i32>(sizeof(void*)), 0x1e, 0, 0);  // 0x005856a0 (pointer records: 4 on x86)
        memset_(&jo::self<jo::WorldArray>(this)->active, 0, sizeof(jo::WorldArray) - sizeof(jo::DynArrayB));  // the fields after the DynArrayB base
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Obj::ctor_560f90", 0x00560f90, &H::ctor_560f90);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

struct H {
    // ------------------------------------------------------------------------ 0x00560fe0
    void mgr_dtor() {
        joan::orig::as_world_array(this)->clear_all();  // 0x00561090
        joan::orig::dynarray_b(this)->destruct();  // 0x00585710
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("Mgr561090::~Mgr", 0x00560fe0, &H::mgr_dtor);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

struct H {
    void* reset_2e() {  // 0x00561050
        jo::self<jo::WorldArray>(this)->selected = 0;
        jo::self<jo::WorldArray>(this)->u32_36 = 0;
        jo::view<u32>(jo::self<jo::WorldArray>(this)->active) = 1;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("X::reset_2e", 0x00561050, &H::reset_2e);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b84_02.cpp (game/b84_02)
namespace {
namespace game_b84_02 {

JOAN_COUNTER(c_clear_561090, "Mgr561090::clear_all");

struct H {
    // 0x00561090 clear all (0xa45f08), only when +0x2e.
    void clear_561090() {
        c_clear_561090.hit();
        if (jo::self<jo::WorldArray>(this)->active == 0) return;
        joan::orig::as_unit_list(this)->select(0);  // 0x00561350
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->empty_at_561320(joan::orig::arg::i32(i))) != 0) continue;          // 0x00561320
            void* const p = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (p != nullptr) joan::orig::as_map(p)->destroy(1u);                      // 0x00561160
            joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));                   // 0x005858c0
        }
        jo::self<jo::WorldArray>(this)->active = 0;
    }
};

#define B84(name, addr, fn) JOAN_REPLACE("game/b84_02", name, addr, fn)
B84("Mgr561090::clear_all", 0x00561090, &H::clear_561090);
#undef B84

}  // namespace game_b84_02
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::orig::helpers::game_delete;                      // operator delete

using joan::orig::helpers::scalar_delete;

struct H {
    void* sdd_561160(std::uint32_t flags) { joan::orig::as_world(this)->destruct(); scalar_delete(this, flags); return this; }  // 0x004214b0
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj561160::sdd", 0x00561160, &H::sdd_561160);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

using u16 = std::uint16_t;

JOAN_COUNTER(c_add_new, "List::add_new");

struct H {
    // ------------------------------------------------------------------------ 0x005611b0
    i32 add_new() {
        c_add_new.hit();
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(offsetof(jo::World, gap_92)));  // operator new (0x92 on x86: a World, its members end at gap_92)
        void* p = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_battle(mem)->init()) : nullptr;  // 0x0041ea20
        joan::orig::dynarray_b(this)->linkin(&p);              // 0x005857a0
        const u16 n = joan::orig::arg::as<u16>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
        jo::view<u16>(jo::as_world(p)->world_id) = n;  // the 0x92-byte object of 0x0041ea20 ("Battle::init") is a World
        return static_cast<i32>(jo::as_world(p)->world_id);
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("List::add_new", 0x005611b0, &H::add_new);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b102_06.cpp (game/b102_06)
namespace {
namespace game_b102_06 {

using joan::hook::global;

JOAN_COUNTER(c_world_remove, "WorldArray::remove");

struct H {
    // 0x00561270
    u32 world_remove(u32 id) {
        c_world_remove.hit();
        void* const p = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3f0
        if (p == nullptr)
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), joan::orig::arg::str(&img::global<const char>(d::s_WorldArray)));  // 0x00586cb0
        if (id == jo::self<jo::WorldArray>(this)->selected) joan::orig::as_unit_list(this)->select(0);  // 0x00561350
        if (p != nullptr) joan::orig::as_map(p)->destroy(1u);  // 0x00561160
        return thiscall<u32>(fn::linkout_5858c0, this, id);  // 0x005858c0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_06", name, addr, fn)
B("WorldArray::remove", 0x00561270, &H::world_remove);
#undef B

}  // namespace game_b102_06
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

void* list_get(void* list, std::int32_t i) { return joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0

struct H {
    std::int32_t empty_561320(std::int32_t i) { return list_get(this, i) == nullptr ? 1 : 0; }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("List::empty_at_561320", 0x00561320, &H::empty_561320);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

constexpr fn::Code kListGet = fn::List_get_41b3f0;    // 0x0041b3f0
void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

struct H {
    void unit_select(std::int32_t id) {                                                  // 0x00561350
        if (id == jo::self<jo::UnitList>(this)->current_id) return;
        if (jo::view<void*>(jo::self<jo::UnitList>(this)->world) != nullptr) joan::orig::as_world(jo::view<void*>(jo::self<jo::UnitList>(this)->world))->unload();  // 0x004426a0
        jo::self<jo::UnitList>(this)->current_id = id;
        jo::view<void*>(jo::self<jo::UnitList>(this)->world) = list_get(this, id);
        if (jo::view<void*>(jo::self<jo::UnitList>(this)->world) != nullptr) joan::orig::as_world(jo::view<void*>(jo::self<jo::UnitList>(this)->world))->setup_sky();  // 0x0049ae00
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("UnitList::select", 0x00561350, &H::unit_select);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

JOAN_COUNTER(c_find_name, "List::find_name_6");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x005613d0
    i32 find_name_6(const char* name) {
        c_find_name.hit();
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->empty_at_561320(joan::orig::arg::i32(i))) != 0) continue;   // 0x00561320
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_units(this)->get_420240(joan::orig::arg::i32(i)));         // 0x00420240
            if (stricmp_(e + 6, name) == 0) return i;
        }
        return 0;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("List::find_name_6", 0x005613d0, &H::find_name_6);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

void* const kSprites = obj(d::g_sprites_a46128);

JOAN_COUNTER(c_frame_cb, "Button::draw_frame_cb");

i32 JOAN_CDECL draw_frame_cb(u8* btn, u32 /*state*/) {  // 0x00561460
    c_frame_cb.hit();
    joan::orig::as_sprite2d(kSprites)->queue(static_cast<i32>(jo::as_edit_box(btn)->x), static_cast<i32>(jo::as_edit_box(btn)->y), static_cast<i32>(jo::as_edit_box(btn)->x2), static_cast<i32>(jo::as_edit_box(btn)->y2), 1u, 0xffffffffu, joan::orig::arg::mptr(0u), 3, 0, 0, 0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0);  // 0x00632a70
    const i32 y2 = static_cast<i32>(jo::as_edit_box(btn)->y2) + 3;
    const i32 x2 = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_edit_box(btn)->cursor_column())) + 5;  // 0x0062c080
    const i32 y1 = static_cast<i32>(jo::as_edit_box(btn)->y) - 3;
    const u32 x1 = joan::orig::arg::as<u32>(joan::orig::as_edit_box(btn)->cursor_column());  // 0x0062c080
    joan::orig::as_sprite2d(kSprites)->queue(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), 1u, 0xffff0000u, joan::orig::arg::mptr(0u), 3, 0, 0, 0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0);  // 0x00632a70
    return 1;
}

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("Button::draw_frame_cb", 0x00561460, &draw_frame_cb);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::WorldArray::clear_all() -> void { joan::bind::member<&::game_b84_02::H::clear_561090>(this); }  // 0x561090
auto joan::orig::Map::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::sdd_561160>(this, a0)); }  // 0x561160
auto joan::orig::List::add_new() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_02::H::add_new>(this)); }  // 0x5611b0
auto joan::orig::List::empty_at_561320(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves82b::H::empty_561320>(this, a0)); }  // 0x561320
auto joan::orig::UnitList::select(std::int32_t a0) -> void { joan::bind::member<&::game_world::H::unit_select>(this, a0); }  // 0x561350
auto joan::orig::List::find_name_6(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_04::H::find_name_6>(this, a0)); }  // 0x5613d0
