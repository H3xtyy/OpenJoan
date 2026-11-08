// game/raceres.cpp: functions of the original d:\ww\raceres.cpp (re/attribution.csv).
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
#include <cstdarg>
#include "../original/records.hpp"
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
using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
//
//      0x004ca350 RaceRes::free            (thiscall, ret 0)
//
//      0x004ca400 RaceRes::find            (thiscall(name), ret 4) - records of 0x34 bytes.
namespace {
namespace game_b93_02 {

using joan::call::cdecl_call;

void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_race_find, "RaceRes::find");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp
void mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70
using joan::orig::helpers::line_of;

struct H {
    void raceres_free() {  // 0x004ca350
        if (jo::self<jo::RaceRes>(this)->loaded != 0) {
            if (jo::self<jo::RaceRes>(this)->races != nullptr) {
                mem_free(jo::self<jo::RaceRes>(this)->races, str(d::s_d_ww_raceres_cpp), line_of(d::g_a02950, 5));  // "d:\ww\raceres.cpp"
                jo::self<jo::RaceRes>(this)->races = nullptr;
            }
            jo::view<u32>(jo::self<jo::RaceRes>(this)->count) = 0;
            jo::self<jo::RaceRes>(this)->loaded = 0;
        }
    }

    // ------------------------------------------------------------------------ 0x004ca400
    i32 race_find(const char* name) {
        c_race_find.hit();
        for (i32 i = 1; i <= jo::self<jo::RaceRes>(this)->count; ++i) {
            if (stricmp_(name, jo::view<u8*>(jo::self<jo::RaceRes>(this)->races) + (i - 1) * 0x34 + 1) == 0) return i;
        }
        return 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("RaceRes::free", 0x004ca350, &H::raceres_free);
B93("RaceRes::find", 0x004ca400, &H::race_find);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b103_04.cpp (game/b103_04)
//
//    - Obj::set_44 0x004c93b0 (v), ret 4 -> 1. Array34::item 0x004ca3d0 (i), ret 4.
namespace {
namespace game_b103_04 {

using joan::orig::helpers::mul;

struct H {
    u8* item_34(i32 i) {  // 0x004ca3d0: the record (a pointer); records of 0x34 bytes, all chars (no pointer: same on x64)
        return static_cast<u8*>(jo::self<jo::RaceRes>(this)->races) + mul(i, 0x34) - 0x34;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("Array34::item", 0x004ca3d0, &H::item_34);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
//
//      0x004ca480 RaceRes::load                (thiscall, ret 0) - "RACE" table, 0x34-byte records.
namespace {
namespace game_b98_03 {

using joan::call::thiscall;

using joan::orig::helpers::memset_;  // memset

JOAN_COUNTER(c_race_load, "RaceRes::load");

struct H {
    // ------------------------------------------------------------------------ 0x004ca480
    void race_load() {
        c_race_load.hit();
        void* const res = joan::orig::arg::as<void*>(joan::orig::as_game_set(obj(d::g_game_set))->open_db_58b180(str(d::s_RACE)));  // 0x0058b180 "RACE"
        jo::self<jo::RaceRes>(this)->count = joan::orig::arg::as<i32>(joan::orig::as_database(res)->rec_count());                                       // 0x00415280
        const i32 line = static_cast<i32>(global<i16>(d::g_a02954)) + 5;
        jo::self<jo::RaceRes>(this)->races = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(static_cast<u32>(jo::self<jo::RaceRes>(this)->count) * 0x34u), str(d::s_d_ww_raceres_cpp), joan::orig::arg::i32(line));  // 0x0058e7b0 "d:\ww\raceres.cpp"
        memset_(jo::self<jo::RaceRes>(this)->races, 0, static_cast<u32>(jo::self<jo::RaceRes>(this)->count) * 0x34u);
        for (i32 i = 0; i < jo::self<jo::RaceRes>(this)->count; ++i) {
            u8* const src = joan::orig::arg::as<u8*>(joan::orig::as_database(res)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            u8* const dst = jo::view<u8*>(jo::self<jo::RaceRes>(this)->races) + static_cast<u32>(i) * 0x34u;
            dst[0] = static_cast<u8>(i + 1);
            joan::orig::as_misc(obj(d::g_misc))->copy_field_590210(joan::orig::arg::str(dst + 1), joan::orig::arg::str(src), 8);              // 0x00590210
            joan::orig::as_misc(obj(d::g_misc))->copy_field_590210(joan::orig::arg::str(dst + 0xa), joan::orig::arg::str(src + 8), 0x14);     // 0x00590210
            joan::orig::as_misc(obj(d::g_misc))->copy_field_590210(joan::orig::arg::str(dst + 0x1f), joan::orig::arg::str(src + 0x1c), 0x14); // 0x00590210
        }
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("RaceRes::load", 0x004ca480, &H::race_load);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
//
//    - Obj::once_4ca310 0x004ca310: when dword +0 == 0, 0x004ca480 and +0 = 1.
namespace {
namespace game_b100_05 {

using u32 = std::uint32_t;

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------ 0x004ca310
    u32 once_4ca310() {
        if (jo::self<jo::RaceRes>(this)->loaded != 0) return joan::call::register_low_bits(this);
        joan::orig::as_race_res(this)->load();  // 0x004ca480
        jo::self<jo::RaceRes>(this)->loaded = 1;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Obj::once_4ca310", 0x004ca310, &H::once_4ca310);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    void* zero_c() {                                             // 0x004ca2e0 RaceRes ctor (the global at 0x00a52758)
        memset_(this, 0, static_cast<u32>(sizeof(jo::RaceRes)));  // 0xc on x86
        return this;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("Struct12::zero", 0x004ca2e0, &H::zero_c);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::RaceRes::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_03::H::zero_c>(this)); }  // 0x4ca2e0
auto joan::orig::RaceRes::free() -> void { joan::bind::member<&::game_b93_02::H::raceres_free>(this); }  // 0x4ca350
auto joan::orig::RaceRes::find(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_02::H::race_find>(this, a0)); }  // 0x4ca400
auto joan::orig::RaceRes::load() -> void { joan::bind::member<&::game_b98_03::H::race_load>(this); }  // 0x4ca480
