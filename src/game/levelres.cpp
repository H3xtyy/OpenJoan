// game/levelres.cpp: functions of the original d:\ww\levelres.cpp (re/attribution.csv).
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
#include "../original/records.hpp"
#include <cmath>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

void* const kMem = obj(d::g_mem);

}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
//
//      0x004a4c30 LevelRes::free         (thiscall, ret 0)
namespace {
namespace game_b93_01 {

using i8 = std::int8_t;

void mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

struct H {
    // ------------------------------------------------------------------------ 0x004a4c30
    void levelres_free() {
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::LevelRes>(this)->loaded)) == 0) return;
        if (jo::self<jo::LevelRes>(this)->levels != nullptr) {
            mem_free(jo::self<jo::LevelRes>(this)->levels, str(d::s_d_ww_levelres_cpp), static_cast<i32>(global<i16>(d::g_a00c9c)) + 6);  // levelres.cpp
            jo::self<jo::LevelRes>(this)->levels = nullptr;
        }
        jo::self<jo::LevelRes>(this)->loaded = 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("LevelRes::free", 0x004a4c30, &H::levelres_free);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
//
//      0x004a4ca0 LevelRes::load        (thiscall, ret 0) - "LEVEL" table, 7-byte records.
namespace {
namespace game_b99_03 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using u32 = std::uint32_t;

void* const kText = obj(d::g_misc);
void* const kResDb = obj(d::g_game_set);

JOAN_COUNTER(c_level_load, "LevelRes::load");

using joan::orig::helpers::memset_;  // memset
i32 atoi_(const void* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }                              // atoi

void* mem_alloc(u32 size, img::Addr file, i32 line) {
    return joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(size), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058e7b0
}
// 0x00590210: copies n chars of src into dst and NUL-terminates.
void text_copy(void* dst, const void* src, i32 n) { joan::orig::as_misc(kText)->copy_field_590210(joan::orig::arg::str(dst), joan::orig::arg::str(src), joan::orig::arg::i32(n)); }

struct H {
    // ------------------------------------------------------------------------ 0x004a4ca0
    void level_load() {
        c_level_load.hit();
        void* const res = joan::orig::arg::as<void*>(joan::orig::as_game_set(kResDb)->open_db_58b180(str(d::s_LEVEL)));  // 0x0058b180 "LEVEL"
        jo::self<jo::LevelRes>(this)->count = joan::orig::arg::as<i32>(joan::orig::as_database(res)->rec_count());                               // 0x00415280
        const i32 line = static_cast<i32>(global<i16>(d::g_a00ca0)) + 0xa;
        jo::self<jo::LevelRes>(this)->levels = static_cast<jo::LevelRec*>(mem_alloc(static_cast<u32>(jo::self<jo::LevelRes>(this)->count * static_cast<i32>(sizeof(jo::LevelRec))), d::s_d_ww_levelres_cpp, line));  // levelres.cpp
        memset_(jo::self<jo::LevelRes>(this)->levels, 0, static_cast<u32>(jo::self<jo::LevelRes>(this)->count * static_cast<i32>(sizeof(jo::LevelRec))));
        for (i32 i = 0; i < jo::self<jo::LevelRes>(this)->count; ++i) {
            const u8* const src = joan::orig::arg::as<const u8*>(joan::orig::as_database(res)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            jo::LevelRec* const e = &jo::self<jo::LevelRes>(this)->levels[i];
            char buf[0x40];
            text_copy(buf, src, 2);
            e->level = static_cast<u8>(atoi_(buf));
            text_copy(buf, src + 2, 9);
            e->experience = atoi_(buf);
            text_copy(buf, src + 0xb, 2);
            jo::view<u8>(e->gain_12c) = static_cast<u8>(atoi_(buf));
            text_copy(buf, src + 0xd, 2);
            jo::view<u8>(e->stat_points) = static_cast<u8>(atoi_(buf));
        }
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("LevelRes::load", 0x004a4ca0, &H::level_load);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
//
//    - Obj::init_4a4bf0 0x004a4bf0 / Obj::ctor_4a4ed0 0x004a4ed0 (ret 0).
namespace {
namespace game_b100_04 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------ 0x004a4bf0
    void* init_4a4bf0() {
        joan::orig::as_level_res(this)->free();  // 0x004a4c30
        joan::orig::as_level_res(this)->load();  // 0x004a4ca0
        jo::self<jo::LevelRes>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Obj::init_4a4bf0", 0x004a4bf0, &H::init_4a4bf0);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

using u8 = std::uint8_t;

struct H {
    // ------------------------------------------------------------------ 0x004a4bc0
    void* clear_byte8() {
        jo::self<jo::LevelRes>(this)->loaded = 0;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Obj::clear_byte8", 0x004a4bc0, &H::clear_byte8);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {

JOAN_COUNTER(c_find7, "Table7::find");

struct H {
    // 0x004a4e60
    std::uint8_t* find7(std::int32_t id) {
        c_find7.hit();
        for (std::int32_t i = 0; i < jo::self<jo::LevelRes>(this)->count; ++i) {
            std::uint8_t* const e = jo::view<std::uint8_t*>(jo::self<jo::LevelRes>(this)->levels) + i * static_cast<std::int32_t>(sizeof(jo::LevelRec));
            if (id == static_cast<std::int32_t>(static_cast<std::int8_t>(*e))) return e;
        }
        return nullptr;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Table7::find", 0x004a4e60, &H::find7);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::LevelRes::free() -> void { joan::bind::member<&::game_b93_01::H::levelres_free>(this); }  // 0x4a4c30
auto joan::orig::LevelRes::load() -> void { joan::bind::member<&::game_b99_03::H::level_load>(this); }  // 0x4a4ca0
auto joan::orig::LevelRes::find(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b88_06::H::find7>(this, a0)); }  // 0x4a4e60
