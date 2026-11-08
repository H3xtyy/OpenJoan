// game/musicres.cpp: functions of the original d:\ww\musicres.cpp (re/attribution.csv).
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
#include "../runtime/replay/replay.hpp"
#include <cmath>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

void* const kMem = obj(d::g_mem);

}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
//
//      0x004b32f0 MusicRes::free           (thiscall, ret 0)
namespace {
namespace game_b93_02 {

void mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70
using joan::orig::helpers::line_of;

struct H {
    // ------------------------------------------------------------------------ resource frees
    void res_free_4_8(img::Addr line_global, const char* file) {  // 0x004b32f0 / 0x004b8f90
        if (jo::view<i8>(jo::self<jo::MusicRes>(this)->loaded) != 0) {
            if (jo::self<jo::MusicRes>(this)->tracks != nullptr) {
                mem_free(jo::view<void*>(jo::self<jo::MusicRes>(this)->tracks), file, line_of(line_global, 6));
                jo::self<jo::MusicRes>(this)->tracks = nullptr;
            }
            jo::self<jo::MusicRes>(this)->loaded = 0;
        }
    }
    void musicres_free() { res_free_4_8(d::g_a0175c, str(d::s_d_ww_musicres_cpp)); }  // "d:\ww\musicres.cpp"
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("MusicRes::free", 0x004b32f0, &H::musicres_free);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b104_04.cpp (game/b104_04)
//
//    - MusicRes::load 0x004b3360 (musicres.cpp): 'MUSIC' table (0x0058b180 / 0x00415280) ->
//      count [this], 0x68-byte entries [this+4] (alloc 0x0058e7b0 + memset); per row: id word,
//      name (0x10) / file (0x40) copies (0x00590210), atoi fields (0x00590170 + atoi 0x007b15cb)
//      to floats +0x54/+0x58 (fild -> float) and words +0x5c/+0x5e, letter codes at +0x50 / +0x5d
//      to +0x64 / +0x60. eax at return = loop index (== count when >= 0); the only caller
//      (0x004b32b0) overwrites eax, returned anyway.
namespace {
namespace game_b104_04 {

using u16 = std::uint16_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

void* const kResSets = obj(d::g_game_set);
void* const kText = obj(d::g_misc);

void* res_table(img::Addr name) { return joan::orig::arg::as<void*>(joan::orig::as_game_set(kResSets)->open_db_58b180(joan::orig::arg::str(str(name)))); }  // 0x0058b180
i32 res_count(void* t) { return joan::orig::arg::as<i32>(joan::orig::as_database(t)->rec_count()); }                                    // 0x00415280
u8* res_row(void* t, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_database(t)->read(joan::orig::arg::i32(i))); }  // 0x00580c70
void* mem_alloc(u32 size, img::Addr file, i32 line) {
    return joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(size), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058e7b0
}
void* game_memset(void* p, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }
void text_copy(void* dst, const void* src, i32 n) { joan::orig::as_misc(kText)->copy_field_590210(joan::orig::arg::str(dst), joan::orig::arg::str(src), joan::orig::arg::i32(n)); }  // 0x00590210
char* text_field(const void* src, i32 n) { return joan::orig::arg::as<char*>(joan::orig::as_misc(kText)->rtrim_field_590170(joan::orig::arg::str(src), joan::orig::arg::i32(n))); }        // 0x00590170
i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }

JOAN_COUNTER(c_music, "MusicRes::load_4b3360");

struct H {
    // ---------------------------------------------------------------- 0x004b3360
    u32 music_load() {
        c_music.hit();
        void* const t = res_table(d::s_MUSIC_9d3508);  // 'MUSIC'
        jo::self<jo::MusicRes>(this)->count = res_count(t);
        jo::view<void*>(jo::self<jo::MusicRes>(this)->tracks) = mem_alloc(static_cast<u32>(jo::self<jo::MusicRes>(this)->count * 0x68), d::s_d_ww_musicres_cpp,
                                static_cast<i32>(global<i16>(d::g_a01760)) + 9);
        game_memset(jo::view<void*>(jo::self<jo::MusicRes>(this)->tracks), 0, static_cast<u32>(jo::self<jo::MusicRes>(this)->count * 0x68));
        i32 i = 0;
        for (; i < jo::self<jo::MusicRes>(this)->count; ++i) {
            u8* const row = res_row(t, i + 1);
            jo::MusicTrack* const e = jo::as_music_track(jo::view<u8*>(jo::self<jo::MusicRes>(this)->tracks) + i * 0x68);
            e->id = static_cast<u16>(i + 1);
            text_copy(e->name, row, 0x10);
            text_copy(e->file, row + 0x10, 0x40);
            e->range_from = static_cast<float>(atoi_(text_field(row + 0x51, 4)));
            e->range_to = static_cast<float>(atoi_(text_field(row + 0x55, 4)));
            jo::view<u16>(e->level_min) = static_cast<u16>(atoi_(text_field(row + 0x59, 2)));
            jo::view<u16>(e->level_max) = static_cast<u16>(atoi_(text_field(row + 0x5b, 2)));
            const i8 a = static_cast<i8>(row[0x50]);
            if (a == 'N' || a == 'n') e->kind = 1;
            else if (a == 'B' || a == 'b') e->kind = 2;
            else if (a == 'M' || a == 'm') e->kind = 3;
            else if (a == 'E' || a == 'e') e->kind = 4;
            else if (a == 'S' || a == 's') e->kind = 5;
            else if (a == 'G' || a == 'g') e->kind = 6;
            else if (a == 'W' || a == 'w') e->kind = 7;
            else if (a == 'C' || a == 'c') e->kind = 8;
            else if (a == '1') e->kind = 9;
            else if (a == '0') e->kind = 10;
            const i8 b = static_cast<i8>(row[0x5d]);
            if (b == 'M' || b == 'm') e->owner = 1;
            else if (b == 'A' || b == 'a') e->owner = 2;
            else if (b == 'N' || b == 'n') e->owner = 3;
            else if (b == 'R' || b == 'r') e->owner = 4;
            else if (b == 'S' || b == 's') e->owner = 5;
            else if (b == 'C' || b == 'c') e->owner = 6;
            else if (b == 'I' || b == 'i') e->owner = 7;
            else if (b == 'B' || b == 'b') e->owner = 8;
            else if (b == 'W' || b == 'w') e->owner = 9;
            else e->owner = 0;
        }
        return static_cast<u32>(i);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_04", name, addr, fn)
B("MusicRes::load_4b3360", 0x004b3360, &H::music_load);
#undef B

}  // namespace game_b104_04
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
//
//    - Obj::init_4b32b0 0x004b32b0 (ret 0): 0x004b32f0, 0x004b3360, byte +8 = 1, eax = this.
namespace {
namespace game_b105_05 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------ 0x004b32b0
    H* init_4b32b0() {
        joan::orig::as_music_res(this)->free();  // 0x004b32f0
        joan::orig::as_music_res(this)->load();  // 0x004b3360
        jo::self<jo::MusicRes>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("Obj::init_4b32b0", 0x004b32b0, &H::init_4b32b0);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

using i32 = std::int32_t;

using joan::call::cdecl_call;

struct H {
    // ------------------------------------------------------------------ 0x004b38c0
    i32 find_name_4b38c0(const char* name) {
        for (i32 i = 0; i < jo::self<jo::MusicRes>(this)->count; ++i) {
            const char* const e = jo::view<const char*>(jo::self<jo::MusicRes>(this)->tracks) + i * 0x68 + 2;
            if (joan::crt::stricmp(joan::orig::arg::str(e), joan::orig::arg::str(name)) == 0) return i + 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Table::find_name_4b38c0", 0x004b38c0, &H::find_name_4b38c0);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b101_05.cpp (game/b101_05)
namespace {
namespace game_b101_05 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

void* const kMath = obj(d::g_misc);

i32 random(i32 n) { return joan::orig::as_misc(kMath)->random(joan::orig::arg::i32(n)); }  // 0x00590540

JOAN_COUNTER(c_pick_random, "Table::pick_random");

struct H {
    jo::MusicTrack* rec(i32 i) { return jo::as_music_track(jo::view<u8*>(jo::self<jo::MusicRes>(this)->tracks) + i * 0x68); }

    // ------------------------------------------------------------------ 0x004b3940
    i32 pick_random(i32 kind, i32 owner, i32 level) {
        c_pick_random.hit();
        if (kind != 5) {
            i32 count = 0;
            for (i32 i = 0; i < jo::self<jo::MusicRes>(this)->count; ++i) {
                if (rec(i)->kind != kind) continue;
                if (level != 0) {
                    if (static_cast<i32>(rec(i)->level_min) > level) continue;
                    if (static_cast<i32>(rec(i)->level_max) < level) continue;
                }
                if (owner == 0) {
                    ++count;
                } else if (rec(i)->owner == owner) {
                    ++count;
                }
            }
            if (count == 0) return 0;
            i32 r = random(count);
            for (i32 i = 0; i < jo::self<jo::MusicRes>(this)->count; ++i) {
                if (rec(i)->kind != kind) continue;
                if (level != 0) {
                    if (static_cast<i32>(rec(i)->level_min) > level) continue;
                    if (static_cast<i32>(rec(i)->level_max) < level) continue;
                }
                if (owner != 0 && rec(i)->owner != owner) continue;
                if (r == 0) return i + 1;
                --r;
            }
            return 0;
        }
        if (owner == 0) return 0;
        i32 count = 0;
        for (i32 i = 0; i < jo::self<jo::MusicRes>(this)->count; ++i) {
            if (rec(i)->kind == kind && rec(i)->owner == owner) ++count;
        }
        if (count == 0) return 0;
        i16 r = static_cast<i16>(random(count) + 1);
        for (i32 i = 0; i < jo::self<jo::MusicRes>(this)->count; ++i) {
            if (rec(i)->kind != kind || rec(i)->owner != owner) continue;
            if (static_cast<i32>(r) == 1) return i + 1;
            r = static_cast<i16>(r - 1);
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_05", name, addr, fn)
B("Table::pick_random", 0x004b3940, &H::pick_random);
#undef B

}  // namespace game_b101_05
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

using i32 = std::int32_t;
using u8 = std::uint8_t;

JOAN_COUNTER(c_ranges_find, "Ranges::find");

struct H {
    // ------------------------------------------------------------------------ 0x004b3c70
    i32 ranges_find(i32 key, float v) {
        c_ranges_find.hit();
        for (i32 i = 0; i < jo::self<jo::MusicRes>(this)->count; ++i) {
            const jo::MusicTrack* const e = jo::as_music_track(jo::view<u8*>(jo::self<jo::MusicRes>(this)->tracks) + i * 0x68);
            if (e->kind != key) continue;
            const float a = e->range_from;
            const float b = e->range_to;
            if (!(a >= b)) {  // a < b or unordered
                if (!(a > v) && b > v) return i + 1;
            } else {
                if (a > v) return i + 1;
                if (!(b > v)) return i + 1;
            }
        }
        return 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("Ranges::find", 0x004b3c70, &H::ranges_find);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using u8 = std::uint8_t;

struct H {
    // ------------------------------------------------------------------ 0x004b3280
    void* clear_flag8() {
        jo::self<jo::MusicRes>(this)->loaded = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Obj::clear_flag8", 0x004b3280, &H::clear_flag8);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::MusicRes::free() -> void { joan::bind::member<&::game_b93_02::H::musicres_free>(this); }  // 0x4b32f0
auto joan::orig::MusicRes::load() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_04::H::music_load>(this)); }  // 0x4b3360
auto joan::orig::Table::pick_random(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_05::H::pick_random>(this, a0, a1, a2)); }  // 0x4b3940
auto joan::orig::MusicRes::find_track(std::int32_t a0, float a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_02::H::ranges_find>(this, a0, a1)); }  // 0x4b3c70
