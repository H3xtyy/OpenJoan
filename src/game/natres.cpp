// game/natres.cpp: functions of the original d:\ww\natres.cpp (re/attribution.csv).
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

void* const kMem = obj(d::g_mem);

}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
//
//      0x004b8f90 NatRes::free             (thiscall, ret 0)
namespace {
namespace game_b93_02 {

using i8 = std::int8_t;

void mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70
using joan::orig::helpers::line_of;

struct H {
    // ------------------------------------------------------------------------ resource frees
    void res_free_4_8(img::Addr line_global, const char* file) {  // 0x004b32f0 / 0x004b8f90
        if (jo::view<i8>(jo::self<jo::NationRes>(this)->loaded) != 0) {
            if (jo::self<jo::NationRes>(this)->nations != nullptr) {
                mem_free(jo::self<jo::NationRes>(this)->nations, file, line_of(line_global, 6));
                jo::self<jo::NationRes>(this)->nations = nullptr;
            }
            jo::self<jo::NationRes>(this)->loaded = 0;
        }
    }
    void natres_free() { res_free_4_8(d::g_a0188c, str(d::s_d_ww_natres_cpp)); }    // "d:\ww\natres.cpp"
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("NatRes::free", 0x004b8f90, &H::natres_free);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
//
//      0x004b9000 NationRes::load       (thiscall, ret 0) - "NATION" table, 0x4e-byte records.
namespace {
namespace game_b99_03 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using u16 = std::uint16_t;

void* const kText = obj(d::g_misc);
void* const kResDb = obj(d::g_game_set);

JOAN_COUNTER(c_nation_load, "NationRes::load");

using joan::orig::helpers::memset_;  // memset

void* mem_alloc(u32 size, img::Addr file, i32 line) {
    return joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(size), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058e7b0
}
// 0x00590210: copies n chars of src into dst and NUL-terminates.
void text_copy(void* dst, const void* src, i32 n) { joan::orig::as_misc(kText)->copy_field_590210(joan::orig::arg::str(dst), joan::orig::arg::str(src), joan::orig::arg::i32(n)); }

struct H {
    // ------------------------------------------------------------------------ 0x004b9000
    void nation_load() {
        c_nation_load.hit();
        void* const res = joan::orig::arg::as<void*>(joan::orig::as_game_set(kResDb)->open_db_58b180(str(d::s_NATION)));  // 0x0058b180 "NATION"
        jo::self<jo::NationRes>(this)->count = joan::orig::arg::as<i32>(joan::orig::as_database(res)->rec_count());                               // 0x00415280
        const i32 line = static_cast<i32>(global<i16>(d::g_a01890)) + 9;
        jo::self<jo::NationRes>(this)->nations = static_cast<jo::NationDef*>(mem_alloc(static_cast<u32>(jo::self<jo::NationRes>(this)->count * static_cast<i32>(sizeof(jo::NationDef))), d::s_d_ww_natres_cpp, line));  // natres.cpp
        memset_(jo::self<jo::NationRes>(this)->nations, 0, static_cast<u32>(jo::self<jo::NationRes>(this)->count * static_cast<i32>(sizeof(jo::NationDef))));
        for (i32 i = 0; i < jo::self<jo::NationRes>(this)->count; ++i) {
            const u8* const src = joan::orig::arg::as<const u8*>(joan::orig::as_database(res)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            jo::NationDef* const e = &jo::self<jo::NationRes>(this)->nations[i];
            e->id = static_cast<u16>(i + 1);
            text_copy(e->name, src, 8);
            text_copy(e->text, src + 8, 0x40);
            const char* const race = joan::orig::arg::as<const char*>(joan::orig::as_misc(kText)->rtrim_field_590170(joan::orig::arg::str(src + 0x48), 8));  // 0x00590170
            e->race = joan::orig::arg::as<u16>(joan::orig::as_race_res(obj(d::g_race_res))->find(joan::orig::arg::str(race)));               // 0x004ca400
            if (static_cast<i32>(jo::view<i16>(e->race)) == 0) {
                joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Unknown_Race_s_in_NationRes), race);  // 0x00586cb0
            }
        }
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("NationRes::load", 0x004b9000, &H::nation_load);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
//
//    - Obj::init_4b8f50 0x004b8f50: 0x004b8f90, 0x004b9000, byte +8 = 1, returns this.
namespace {
namespace game_b100_05 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------ 0x004b8f50
    H* init_4b8f50() {
        joan::orig::as_nation_res(this)->free();  // 0x004b8f90
        joan::orig::as_nation_res(this)->load();  // 0x004b9000
        jo::self<jo::NationRes>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Obj::init_4b8f50", 0x004b8f50, &H::init_4b8f50);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::NationRes::free() -> void { joan::bind::member<&::game_b93_02::H::natres_free>(this); }  // 0x4b8f90
auto joan::orig::NationRes::load() -> void { joan::bind::member<&::game_b99_03::H::nation_load>(this); }  // 0x4b9000
