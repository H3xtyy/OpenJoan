// SPDX-License-Identifier: GPL-3.0-or-later
// enparticle/peffecta.cpp: functions of the original d:\ww\enparticle\peffecta.cpp (re/attribution.csv). The file name
// is the Restaurant Empire 2 engine's (enparticle/PEffectA.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__
// string for it.

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
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using u32 = std::uint32_t;

using i32 = std::int32_t;

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

struct H {
    // ------------------------------------------------------------------ SEH dtors
    u32 dtor_41b4a0() {
        joan::orig::as_list(this)->destroy_41b530();              // 0x0041b530
        return thiscall<u32>(fn::dtor_DynArrayB, this);  // 0x00585710
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("ParticleEffectArray::~ParticleEffectArray", 0x0041b4a0, &H::dtor_41b4a0);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

std::int32_t list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

JOAN_COUNTER(c_destroy1, "List::destroy_41b530");

struct H {
    // ---- 0x0041b530 / 0x0046cdc0 destroy every entry, then the list ---------------------------
    std::intptr_t destroy_41b530() {
        c_destroy1.hit();
        for (std::int32_t i = list_count(this); i > 0; --i) {
            if (joan::orig::as_dynarray(this)->empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x0041bd90
            void* const p = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0041bd60
            if (p != nullptr) joan::orig::as_effect(p)->destroy(1u);              // 0x0041b5f0
            joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));            // 0x005858c0
        }
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_sound(this)->reset_585d20());  // 0x00585d20
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("ParticleEffectArray::deinit", 0x0041b530, &H::destroy_41b530);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- direct calls (re/bind/gen_bridges.py)
auto joan::orig::List::destroy_41b530() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::destroy_41b530>(this)); }  // 0x41b530
