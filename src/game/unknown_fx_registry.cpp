// game/unknown_fx_registry.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/fx_registry.cpp; re/attribution.csv: between ww/enparticle/peffecta.cpp and ww/battle.cpp). They move
// to their original file when evidence names it (Phase 3).
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
#include "../runtime/hitch.hpp"

#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using joan::hook::global;

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

using joan::call::cdecl_call;
using joan::img::str;

using joan::orig::helpers::game_new;  // operator new

JOAN_COUNTER(c_model_add, "ModelList::add");

struct H {
    // ------------------------------------------------------------------ 0x0041baf0
    std::int32_t model_add(jo::Scene* a1, void* a2, const void* a3, const void* a4, std::int8_t a5,
                           std::int32_t a6, std::int32_t a7, std::int32_t a8) {
        c_model_add.hit();
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::ModelObj)));  // 0xa3 on x86
        void* m = mem != nullptr ? joan::orig::as_effect(mem)->construct() : nullptr;  // 0x0041a1b0
        joan::orig::as_effect(m)->init(a1, a2, joan::orig::arg::mptr(a3), joan::orig::arg::mptr(a4), static_cast<char>(joan::orig::arg::i32(a5)), joan::orig::arg::u32(a6), joan::orig::arg::i32(a7), joan::orig::arg::i32(a8));           // 0x0041a740
        joan::orig::dynarray_b(this)->linkin(static_cast<void*>(&m));            // 0x005857a0
        jo::self<jo::ModelObj>(m)->kind = joan::orig::arg::as<std::int16_t>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520 (+0: the registry index; the entries are ModelObj)
        return joan::orig::as_dynarray(this)->cursor();
    }

    // ------------------------------------------------------------------ 0x0041baa0
    void set_name(std::int32_t index, const char* name) {
        if (name == nullptr) return;
        char* const d = reinterpret_cast<char*>(jo::view<std::uint8_t*>(jo::self<jo::Cell>(this)->sector_names) + (index - 1) * 0x21);
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(name), 0x20u);
        d[0x20] = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("ModelList::add", 0x0041baf0, &H::model_add);
B("Names::set", 0x0041baa0, &H::set_name);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

// ---- static initialisers / free functions ----
using joan::orig::helpers::div_init;
void JOAN_CDECL init_41bbc0() { div_init(d::g_a3ce18, d::g_a3ce08, d::flt_4); }
void JOAN_CDECL init_41bee0() { div_init(d::g_a33634, d::g_a33628, d::flt_4); }

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("init_41bbc0", 0x0041bbc0, &init_41bbc0);
B("init_41bee0", 0x0041bee0, &init_41bee0);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_units4.cpp (game/units4)
namespace {
namespace game_units4 {

struct H {
    // 0x0041bbf0 EffectRegistry::remove(id): deletes the effect, frees the slot.
    std::uint32_t registry_remove(std::int32_t id) {
        void* const e = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(id));  // 0x0041bd60
        if (e != nullptr) joan::orig::as_effect(e)->destroy(1u);    // 0x0041b5f0
        return thiscall<std::uint32_t>(fn::linkout_5858c0, this, id);   // 0x005858c0
    }
};

#define U4(name, addr, fn) JOAN_REPLACE("game/units4", name, addr, fn)
U4("EffectRegistry::remove", 0x0041bbf0, &H::registry_remove);
#undef U4

}  // namespace game_units4
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

JOAN_COUNTER(c_purge_dead, "Group::purge_dead");

struct H {
    // ------------------------------------------------------------------ 0x0041bc60
    void purge_dead() {
        joan::hitch::Span hitch_span("fx_purge_dead");  // [Debug] HitchLog
        c_purge_dead.hit();
        if (global<void*>(d::g_world) == nullptr) return;
        if (jo::as_world(global<void*>(d::g_world))->scene == nullptr) return;
        if (global<void*>(d::g_camera) == nullptr) return;
        joan::orig::as_scene_s(jo::as_world(global<void*>(d::g_world))->scene)->occluders(joan::orig::arg::mptr(global<void*>(d::g_camera)));  // 0x005eee40
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::as_dynarray(this)->empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x0041bd90
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041bd60
            if (static_cast<i32>(jo::self<jo::ModelObj>(e)->attached) == 2) continue;  // +0x1a: 2 = dead (the Effect view read it at the x86 offset)
            if (joan::orig::arg::as<i32>(joan::orig::as_effect(e)->update()) == 0) {                 // 0x0041ab50
                joan::orig::as_effect_registry(this)->remove(joan::orig::arg::i32(i));                   // 0x0041bbf0
            }
        }
        joan::orig::as_scene(jo::as_world(global<void*>(d::g_world))->scene)->end_frame(joan::orig::arg::mptr(global<void*>(d::g_camera)));  // 0x005ef380
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Group::purge_dead", 0x0041bc60, &H::purge_dead);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using joan::call::cdecl_call;

using joan::orig::helpers::pi_from_atan;

void JOAN_CDECL init_a3cd78() { global<float>(d::g_a3cd78) = pi_from_atan(); }

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("init_a3cd78", 0x0041bd30, &init_a3cd78);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

constexpr fn::Code kListGet = fn::List_get_41b3f0;     // 0x0041b3f0

void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

struct H {
    void* list_get_wrap(std::int32_t i) { return list_get(this, i); }                      // 0x0041b3c0 / 0x0041bd60 / 0x00487b80
    std::int32_t list_empty_at(std::int32_t i) { return list_get(this, i) == nullptr ? 1 : 0; }  // 0x0041bd90 / 0x00428b70 / 0x00487bb0
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("List::get_c", 0x0041bd60, &H::list_get_wrap);
FX1("List::empty_at", 0x0041bd90, &H::list_empty_at);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- CRT initialiser wrappers: call the body directly, return its eax ----
template <fn::Code Body>
u32 JOAN_CDECL init_wrapper() {
    return cdecl_call<u32>(Body);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("init_0x0041bdc0", 0x0041bdc0, &init_wrapper<fn::init_a33628>);
B("init_0x0041be20", 0x0041be20, &init_wrapper<fn::init_a3362c>);
B("init_0x0041be70", 0x0041be70, &init_wrapper<fn::init_a33630>);
B("init_0x0041bec0", 0x0041bec0, &init_wrapper<fn::init_41bee0>);
#undef B

}  // namespace game_b105_01
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using joan::call::cdecl_call;

using joan::orig::helpers::pi_from_atan;

void JOAN_CDECL init_a33628() { global<float>(d::g_a33628) = pi_from_atan(); }

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("init_a33628", 0x0041bde0, &init_a33628);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

// g_out = g_in / g_div (fld dword / fdiv dword / fstp dword).
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a3362c() { div_global(d::g_a3362c, d::g_a33628, d::flt_2); }  // 0x0041be40

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("init_a3362c", 0x0041be40, &init_a3362c);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a33630() { div_global(d::g_a33630, d::g_a33628, d::flt_3); }  // 0x0041be90 (/3)

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("init_a33630", 0x0041be90, &init_a33630);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Names::set_41baa0(std::int32_t a0, const char* a1) -> void { joan::bind::member<&::game_b83_07::H::set_name>(this, a0, a1); }  // 0x41baa0
auto joan::orig::ModelList::add(Scene* a0, void* a1, const void* a2, const void* a3, std::int8_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::model_add>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x41baf0
auto joan::orig::init_41bbc0() -> void { joan::bind::free<&::game_b100_01::init_41bbc0>(); }  // 0x41bbc0
auto joan::orig::EffectRegistry::remove(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_units4::H::registry_remove>(this, a0)); }  // 0x41bbf0
auto joan::orig::init_a3cd78() -> void { joan::bind::free<&::game_b96_01::init_a3cd78>(); }  // 0x41bd30
