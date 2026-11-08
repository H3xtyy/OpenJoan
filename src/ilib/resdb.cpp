// ilib/resdb.cpp: functions of the original d:\ww\ilib\resdb.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstring>
#include "../original/helpers.hpp"

#include "../original/direct.hpp"
#include "../fn_names.hpp"
#include "../original/database.hpp"
#include "../original/engine.hpp"
#include "../registry.hpp"
#include <cstdint>
#include "../data_names.hpp"
#include "../x87.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from ilib/database.cpp (ilib/resdb.cpp)
namespace {
namespace ilib_database {

using joan::orig::ResourceDb;

#define RDB(name, addr, fn) JOAN_REPLACE("ilib/resdb.cpp", "ResourceDb::" name, addr, fn)
RDB("deinit", 0x0059b440, &ResourceDb::deinit);
RDB("read", 0x0059b4f0, &ResourceDb::read);
RDB("get_file", 0x0059b620, &ResourceDb::get_file);
RDB("init_imported", 0x0059b6d0, &ResourceDb::init_imported);
RDB("read_imported", 0x0059b830, &ResourceDb::read_imported);
#undef RDB

}  // namespace ilib_database
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
//
//    - Obj::dtor_596380 0x00596380: 0x59b440 then 0x5872b0 (SEH frame only guards the first).
//
//  - Almost everything here has no references at all (dead code); only the 0xa15834 / 0xa15838
//    case helpers, 0x596380 (2 callers) and 0x5a1060 (qsort) are reachable.
namespace {
namespace game_b103_10 {

using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_dtor_596380, "Obj::dtor_596380");

struct H {
    // ------------------------------------------------------------------ 0x00596380
    u32 dtor_596380() {
        c_dtor_596380.hit();
        joan::orig::as_resource_db(this)->deinit();     // 0x0059b440
        joan::orig::as_file(this)->destruct_5872b0();  // 0x005872b0
        return 0;  // ~File returns nothing; 0x00454210 (the only caller) is an atexit dtor
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("Obj::dtor_596380", 0x00596380, &H::dtor_596380);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- from modules/game_b102_09.cpp (game/b102_09)
//
//    - Speaker::init 0x0059b390 (thiscall(a, b, c, d), ret 0x10): 0x0059b440, 0x00586da0(a, 1, 0),
//      fields +0x10f/+0x113/+0x117/+0x11b, byte +0x10e = 1.
namespace {
namespace game_b102_09 {

using joan::hook::global;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::at;
using joan::img::str;

JOAN_COUNTER(c_speaker_init, "Speaker::init");

struct H {

    // ------------------------------------------------------------------------ 0x0059b390
    std::uintptr_t speaker_init(const char* a, jo::Database* b, u32 c, u32 d) {  // a: the resource file path (x64: a pointer)
        c_speaker_init.hit();
        joan::orig::as_resource_db(this)->deinit();        // 0x0059b440
        joan::orig::as_file(this)->file_open(joan::orig::arg::str(a), 1, static_cast<char>(0));  // 0x00586da0
        jo::self<jo::ResourceDb>(this)->db = b;
        jo::view<u32>(jo::self<jo::ResourceDb>(this)->index_offset) = c;
        jo::view<u32>(jo::self<jo::ResourceDb>(this)->use_common_buf) = d;
        std::uintptr_t eax = d;
        if (jo::view<u32>(jo::self<jo::ResourceDb>(this)->use_common_buf) != 0) {
            char* const buf = global<char*>(d::g_common_res_buf);
            eax = joan::orig::arg::as<std::uintptr_t>(buf);
            jo::self<jo::ResourceDb>(this)->data_buf = buf;
        } else {
            jo::self<jo::ResourceDb>(this)->data_buf = nullptr;
        }
        jo::view<u8>(jo::self<jo::ResourceDb>(this)->init_flag) = 1;
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_09", name, addr, fn)
B("Speaker::init", 0x0059b390, &H::speaker_init);
#undef B

}  // namespace game_b102_09
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ResourceDb::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_10::H::dtor_596380>(this)); }  // 0x596380
