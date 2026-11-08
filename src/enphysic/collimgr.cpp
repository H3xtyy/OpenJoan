// SPDX-License-Identifier: GPL-3.0-or-later
// enphysic/collimgr.cpp: functions of the original d:\ww\enphysic\collimgr.cpp (re/attribution.csv). The file name is
// the Restaurant Empire 2 engine's (enphysic/collimgr.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string
// for it.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstddef>
#include <cstdarg>

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

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

using joan::img::obj;

using joan::call::cdecl_call;

using u8 = std::uint8_t;

using i16 = std::int16_t;

using joan::hook::x87::Fpu;

using joan::orig::helpers::v3d_call;

// ---------------------------------------------------------------- 0x0068de40 helpers
struct Box {  // min x, y, z, max x, y, z (0x0060d430 point box)
    float v[6];
};

using joan::call::import_call;


struct Vec3 {
    float x, y, z;
};

using joan::hook::x87::ftol;

}  // namespace

// ---------------------------------------- from modules/game_b88_04.cpp (game/b88_04)
namespace {
namespace game_b88_04 {

static_assert(sizeof(Vec3) == 12, "vec3");

JOAN_COUNTER(c_is_free, "Placement::is_free");

struct H {
    // ------------------------------------------------------------ 0x00693170 (ret 8)
    std::uint32_t is_free(void* object, std::uint32_t side) {
        c_is_free.hit();
        Vec3 pos;  // ebp-0x10
        joan::orig::as_actor(object)->position(&pos);  // 0x005ac270
        const std::int32_t ix = joan::hook::x87::ftol(pos.x);  // ebp-0x14
        const std::int32_t iy = joan::hook::x87::ftol(pos.y);  // ebp-0x18
        const std::int32_t iz = joan::hook::x87::ftol(pos.z);  // ebp-0x1c
        void* cell = nullptr;                                  // ebp-0x24
        void* slot = nullptr;                                  // ebp-0x20 (written by the callee)
        joan::orig::find_zone_pair_6aae70(joan::orig::arg::mptr(reinterpret_cast<std::uint8_t*>(this) + sizeof(void*)), joan::orig::arg::i32(jo::self<jo::Collisions>(this)->zone_count), joan::orig::arg::bits(static_cast<float>(ix)), joan::orig::arg::bits(static_cast<float>(iy)), joan::orig::arg::bits(static_cast<float>(iz)), 0u, joan::orig::arg::u32(static_cast<std::int32_t>(static_cast<std::int16_t>(side))), &cell, &slot, 1u);  // 0x006aae70 (returns 0 / 1) (+4 on x86: after the leading pointer)
        if (slot == nullptr || cell == nullptr) return 0;  // eax 0 / 1, xor al,al
        const std::int32_t count = joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Collisions>(this)->objects))->size();  // 0x00417ad0
        if (count <= 0) return (static_cast<std::uint32_t>(count) & 0xffffff00u) | 1u;
        for (std::int32_t i = count; i > 0; --i) {
            std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Collisions>(this)->objects))->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (e != nullptr && jo::view<void*>(jo::as_shot(e)->actor) == object) continue;
            if (e == nullptr || jo::view<std::int32_t>(jo::as_shot(e)->id) == 0) continue;
            const std::uint8_t hit = joan::orig::arg::as<std::uint8_t>(joan::orig::shapes_overlap_6a31d0(joan::orig::arg::mptr(object), joan::orig::arg::mptr(jo::view<void*>(jo::as_shot(e)->actor))));  // 0x006a31d0
            if (hit != 0) return 0;
        }
        return 1;  // eax = loop counter 0, mov al,1
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_04", name, addr, fn)
B("CollisionManager::check_valid_position", 0x00693170, &H::is_free);
#undef B

}  // namespace game_b88_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::CollisionGrid::is_free(void* a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b88_04::H::is_free>(this, a0, a1)); }  // 0x693170
