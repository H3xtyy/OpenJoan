// game/sdateres.cpp: functions of the original d:\ww\sdateres.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
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
using u16 = std::uint16_t;
using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
//
//      0x004ce070 SdateRes::free           (thiscall, ret 0)
namespace {
namespace game_b93_02 {

using i8 = std::int8_t;

void* const kMem = obj(d::g_mem);

void mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70
using joan::orig::helpers::line_of;

struct H {
    void sdateres_free() {  // 0x004ce070
        if (jo::view<i8>(jo::self<jo::SdateRes>(this)->loaded) != 0) {
            if (jo::self<jo::SdateRes>(this)->dates != nullptr) {
                mem_free(jo::self<jo::SdateRes>(this)->dates, str(d::s_d_ww_sdateres_cpp), line_of(d::g_a02e78, 6));  // "d:\ww\sdateres.cpp"
                jo::self<jo::SdateRes>(this)->dates = nullptr;
                jo::self<jo::SdateRes>(this)->count = 0;
            }
            jo::self<jo::SdateRes>(this)->loaded = 0;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("SdateRes::free", 0x004ce070, &H::sdateres_free);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
//
//    - SDate::load 0x004ce0f0 (ret 0): reads the "SDATE" resource into +0 (0x33-byte rows).
namespace {
namespace game_b100_05 {

using joan::call::thiscall;

void* const kHeap = obj(d::g_mem);
void* const kMath = obj(d::g_misc);

using joan::orig::helpers::crt_memset;

JOAN_COUNTER(c_sdate_load, "SDate::load");

struct H {
    // ------------------------------------------------------------------ 0x004ce0f0
    void sdate_load() {
        c_sdate_load.hit();
        void* const res = joan::orig::arg::as<void*>(joan::orig::as_game_set(obj(d::g_game_set))->open_db_58b180(str(d::s_SDATE)));  // 0x0058b180 "SDATE"
        jo::self<jo::SdateRes>(this)->count = joan::orig::arg::as<u16>(joan::orig::as_database(res)->rec_count());                                      // 0x00415280
        const i32 line = static_cast<i32>(global<i16>(d::g_a02e7c)) + 9;
        jo::view<u8*>(jo::self<jo::SdateRes>(this)->dates) = joan::orig::arg::as<u8*>(joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::self<jo::SdateRes>(this)->count)) * static_cast<i32>(sizeof(jo::SdateEntry))), str(d::s_d_ww_sdateres_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        crt_memset(jo::view<u8*>(jo::self<jo::SdateRes>(this)->dates), 0, static_cast<u32>(static_cast<i32>(jo::view<i16>(jo::self<jo::SdateRes>(this)->count)) * static_cast<i32>(sizeof(jo::SdateEntry))));
        for (i32 i = 0; i < static_cast<i32>(jo::view<i16>(jo::self<jo::SdateRes>(this)->count)); ++i) {
            u8* const src = joan::orig::arg::as<u8*>(joan::orig::as_database(res)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            jo::SdateEntry* const dst = &jo::self<jo::SdateRes>(this)->dates[i];
            joan::orig::as_misc(kMath)->copy_field_590210(joan::orig::arg::str(dst->name), joan::orig::arg::str(src), 0x20);                    // 0x00590210
            dst->year = joan::orig::arg::as<u32>(joan::orig::as_misc(kMath)->atoi_field_590270(joan::orig::arg::str(src + 0x20), 4u));  // 0x00590270
            dst->month = joan::orig::arg::as<u32>(joan::orig::as_misc(kMath)->atoi_field_590270(joan::orig::arg::str(src + 0x24), 2u));  // 0x00590270
            dst->day = joan::orig::arg::as<u32>(joan::orig::as_misc(kMath)->atoi_field_590270(joan::orig::arg::str(src + 0x26), 2u));  // 0x00590270
            dst->weekday = joan::orig::arg::as<u32>(joan::orig::as_misc(kMath)->atoi_field_590270(joan::orig::arg::str(src + 0x28), 1u));  // 0x00590270
            dst->id = static_cast<u16>(i + 1);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("SDate::load", 0x004ce0f0, &H::sdate_load);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
//
//      0x004ce000 Obj::forward_4ce070          (thiscall, ret 0) - returns callee eax.
namespace {
namespace game_b98_03 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x004ce000
    u32 forward_4ce070() { joan::orig::as_sdate_res(this)->free(); return 0; }  // 0x004ce070 (void; static dtor)
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("Obj::forward_4ce070", 0x004ce000, &H::forward_4ce070);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b101_06.cpp (game/b101_06)
//
//    - SDate::ensure_loaded 0x004ce030 (ret 0): when byte +6 == 0: 0x004ce0f0, byte +6 = 1.
namespace {
namespace game_b101_06 {

using i8 = std::int8_t;
using u8 = std::uint8_t;

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------------ 0x004ce030
    void sdate_ensure() {
        if (jo::view<i8>(jo::self<jo::SdateRes>(this)->loaded) != 0) return;
        joan::orig::as_sdate_res(this)->load();  // 0x004ce0f0
        jo::self<jo::SdateRes>(this)->loaded = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_06", name, addr, fn)
B("SDate::ensure_loaded", 0x004ce030, &H::sdate_ensure);
#undef B

}  // namespace game_b101_06
}  // namespace

// ---------------------------------------- from modules/game_b95_03.cpp (game/b95_03)
namespace {
namespace game_b95_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    void* zero_7() {  // 0x004cdfd0
        memset_(this, 0, sizeof(jo::SdateRes));  // 7 on x86
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("Struct7::zero", 0x004cdfd0, &H::zero_7);
#undef B95

}  // namespace game_b95_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SDateRes::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_03::H::zero_7>(this)); }  // 0x4cdfd0
auto joan::orig::SDateRes::free() -> void { joan::bind::member<&::game_b93_02::H::sdateres_free>(this); }  // 0x4ce070
auto joan::orig::SDateRes::load() -> void { joan::bind::member<&::game_b100_05::H::sdate_load>(this); }  // 0x4ce0f0
