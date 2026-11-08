// game/unknown_actorlst.cpp: functions whose original source file is unknown (formerly game/_unplaced/actorlst.cpp;
// re/attribution.csv: between ww/en3d/actor.cpp and ww/en3d/actordef.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
namespace {
namespace game_b105_16 {

JOAN_COUNTER(c_release, "Obj::release_5af240");

template <fn::Code Body>
u32 JOAN_CDECL crt_init() {
    return cdecl_call<u32>(Body);  // direct call in the original (no thunk)
}

struct H {
    // ---------------------------------------------------------------- stubs / getters
    void stub0() {}
    void stub1(u32) {}

    // ---------------------------------------------------------------- 0x005af240
    i32 release_5af240() {
        c_release.hit();
        i32 i = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
        for (; i > 0; --i) {
            if (joan::orig::arg::as<u32>(joan::orig::as_table(this)->missing(joan::orig::arg::u32(i))) != 0) continue;  // 0x005af360
            void* e = joan::orig::arg::as<void*>(joan::orig::as_table(this)->get_5af330(joan::orig::arg::u32(i)));        // 0x005af330
            joan::orig::as_actor(e)->on_release();                           // 0x005ab450
        }
        return i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("crt_init_5aeec0", 0x005aeec0, &crt_init<fn::Init_pi_5aeee0>);
B("crt_init_5aef20", 0x005aef20, &crt_init<fn::set_a67668>);
B("crt_init_5aef70", 0x005aef70, &crt_init<fn::set_a6766c>);
B("crt_init_5aefc0", 0x005aefc0, &crt_init<fn::init_a67670>);
B("Obj::stub_5af040", 0x005af040, &H::stub0);
B("Obj::stub_5af220", 0x005af220, &H::stub1);
B("Obj::release_5af240", 0x005af240, &H::release_5af240);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

// 0x005aeee0 / 0x005f44c0
void JOAN_CDECL init_pi_5aeee0() {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a6763c) = static_cast<float>(a * 4.0);
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Init::pi_5aeee0", 0x005aeee0, &init_pi_5aeee0);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

// ---------------------------------------------------------------------------- 0x005aef40
static void JOAN_CDECL set_a67668() { global<float>(d::g_a67668) = global<float>(d::g_a6763c) / global<float>(d::flt_2); }

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("set_a67668", 0x005aef40, &set_a67668);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

// ---------------------------------------------------------------------------- 0x005aef90
static void JOAN_CDECL set_a6766c() {
    global<float>(d::g_a6766c) = global<float>(d::g_a6763c) / global<float>(d::flt_3);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("set_a6766c", 0x005aef90, &set_a6766c);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

void JOAN_CDECL init_a67670() {  // 0x005aefe0
    global<float>(d::g_a67670) = global<float>(d::g_a6763c) / global<float>(d::flt_4);
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("init_a67670", 0x005aefe0, &init_a67670);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
namespace {
namespace game_b103_11 {

struct H {
    // 0x005af010
    H* array_ctor() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x64, 0, 0);  // 0x005856a0 (pointers: 4 on x86)
        return this;
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Array::Array_5af010", 0x005af010, &H::array_ctor);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/game_b102_10.cpp (game/b102_10)
namespace {
namespace game_b102_10 {

using u8 = std::uint8_t;


using joan::orig::helpers::op_new;

JOAN_COUNTER(c_add_5af0d0, "List::add_5af0d0");

struct H {
    // 0x005af060
    u32 purge_5af060() {
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0 count
            if (joan::orig::arg::as<i32>(joan::orig::as_table(this)->missing(joan::orig::arg::u32(i))) == 0)            // 0x005af360
                joan::orig::as_actor_list(this)->remove(joan::orig::arg::u32(i));                      // 0x005af1b0
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
        return 0;  // void callee; 0x005af060 has no callers (result unused)
    }
    // 0x005af0d0
    u32 add_5af0d0(jo::ActorDef* a) {
        c_add_5af0d0.hit();
        void* const mem = op_new(static_cast<u32>(sizeof(jo::SceneObject)));  // 0x10c on x86
        u8* item = mem != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20 ctor
        jo::as_scene_object(item)->def = a;
        joan::orig::as_scene_object(item)->init();                // 0x005aab70
        joan::orig::dynarray_b(this)->linkin(&item);      // 0x005857a0 add(&item)
        const u32 v = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
        jo::view<u32>(jo::as_scene_object(item)->list_index) = v;
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());
    }
    // 0x005af2b0
    i32 each_5af2b0(void* a, u32 b) {
        i32 i = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
        for (; i > 0; --i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_table(this)->missing(joan::orig::arg::u32(i))) != 0) continue;  // 0x005af360
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_table(this)->get_5af330(joan::orig::arg::u32(i)));    // 0x005af330
            joan::orig::as_actor(e)->draw(a, joan::orig::arg::i32(b));                            // 0x005aba20
        }
        return i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_10", name, addr, fn)
B("List::purge_5af060", 0x005af060, &H::purge_5af060);
B("List::add_5af0d0", 0x005af0d0, &H::add_5af0d0);
B("List::each_5af2b0", 0x005af2b0, &H::each_5af2b0);
#undef B

}  // namespace game_b102_10
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

struct H {
    // 0x005af1b0
    std::uint32_t remove_5af1b0(std::uint32_t key) {
        void* p = joan::orig::arg::as<void*>(joan::orig::as_table(this)->get_5af330(joan::orig::arg::u32(key)));  // 0x005af330
        if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840
        return thiscall<std::uint32_t>(fn::linkout_5858c0, this, key);  // 0x005858c0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Obj::remove_5af1b0", 0x005af1b0, &H::remove_5af1b0);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

struct H {
    // 0x005af330 / 0x005af360
    std::uintptr_t table_get(std::uint32_t key) { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(key))); }  // 0x0041b3f0
    std::int32_t table_missing(std::uint32_t key) {
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(key))) == 0 ? 1 : 0;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("Table::get", 0x005af330, &H::table_get);
B91("Table::missing", 0x005af360, &H::table_missing);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ActorList::remove(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_06::H::remove_5af1b0>(this, a0)); }  // 0x5af1b0
auto joan::orig::Table::get_5af330(std::uint32_t a0) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b91_03::H::table_get>(this, a0)); }  // 0x5af330
auto joan::orig::Table::missing(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_03::H::table_missing>(this, a0)); }  // 0x5af360
