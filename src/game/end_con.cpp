// game/end_con.cpp: functions of the original d:\ww\end_con.cpp (re/attribution.csv).
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

void* const kMem = obj(d::g_mem);

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
//
//    - EndCon::release 0x00467260 (end_con.cpp).
namespace {
namespace game_b84_05 {

void mem_free(void* p, const char* file, std::int32_t line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

JOAN_COUNTER(c_endcon, "EndCon::release");

struct H {
    // ---- 0x00467260 EndCon::release (end_con.cpp) ---------------------------------------------
    void endcon_release() {
        c_endcon.hit();
        if (jo::view<void*>(jo::self<jo::EndCon>(this)->text) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::EndCon>(this)->text), str(d::s_d_ww_end_con_cpp), static_cast<std::int32_t>(global<std::int16_t>(d::g_9fd8a4)) + 3);
            jo::view<void*>(jo::self<jo::EndCon>(this)->text) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::EndCon>(this)->text2) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::EndCon>(this)->text2), str(d::s_d_ww_end_con_cpp), static_cast<std::int32_t>(global<std::int16_t>(d::g_9fd8a4)) + 8);
            jo::view<void*>(jo::self<jo::EndCon>(this)->text2) = nullptr;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("EndCon::release", 0x00467260, &H::endcon_release);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
//
//      0x00467300 EndCon::set_text       (thiscall(str), ret 4)
namespace {
namespace game_b93_01 {

using joan::orig::helpers::strlen_;  // strlen
using joan::orig::helpers::memcpy_;  // memcpy

struct H {
    // ------------------------------------------------------------------------ 0x00467300
    void set_text(const char* s) {
        if (s == nullptr) return;
        const u32 n = strlen_(s) + 1;
        jo::view<void*>(jo::self<jo::EndCon>(this)->text) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::EndCon>(this)->text)), joan::orig::arg::u32(n), str(d::s_d_ww_end_con_cpp), joan::orig::arg::i32(// 0x0058eb30
                                      static_cast<i32>(global<i16>(d::g_9fd8a8)) + 6));   // end_con.cpp
        memcpy_(jo::view<void*>(jo::self<jo::EndCon>(this)->text), s, n);
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("EndCon::set_text", 0x00467300, &H::set_text);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
//
//      0x00467390 Obj::set_text_5(s)            (thiscall, ret 4) - realloc +0x5 (end_con.cpp), memcpy.
//
//  - 0x00467390: the s == 0 path returns the caller's eax; its only caller 0x0065a460 loads
//    eax = s before the call (mov eax,[ebp+0xc]; push eax) and ignores the result - 0 is exact.
namespace {
namespace game_b99_02 {

using joan::orig::helpers::u;

JOAN_COUNTER(c_set_text_5, "Obj::set_text_5");

using joan::orig::helpers::crt_memcpy;
u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }

struct H {
    // ------------------------------------------------------------------ 0x00467390
    std::uintptr_t set_text_5(const char* s) {
        c_set_text_5.hit();
        if (s == nullptr) return 0;  // caller's eax: the only caller (0x0065a460) pushes s from eax, so 0
        const i32 n = static_cast<i32>(crt_strlen(s)) + 1;
        const i32 line = static_cast<i32>(global<i16>(d::g_9fd8ac)) + 6;
        jo::view<void*>(jo::self<jo::EndCon>(this)->text2) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::EndCon>(this)->text2)), joan::orig::arg::u32(n), str(d::s_d_ww_end_con_cpp), joan::orig::arg::i32(line));  // 0x0058eb30
        return u(crt_memcpy(jo::view<void*>(jo::self<jo::EndCon>(this)->text2), s, static_cast<u32>(n)));
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::set_text_5", 0x00467390, &H::set_text_5);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
//
//    - Obj::call_467260_467230 0x00467230 (ret 0, referenced from code at 0x0046f0c4).
namespace {
namespace game_b103_01 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x00467230
    u32 call_467260_467230() {
        joan::orig::as_end_condition(this)->release();  // 0x00467260
        return 0;  // EndCon::release returns nothing; only used as the eh vector iterator's element dtor (result unused)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::call_467260_467230", 0x00467230, &H::call_467260_467230);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::EndCondition::release() -> void { joan::bind::member<&::game_b84_05::H::endcon_release>(this); }  // 0x467260
auto joan::orig::EndCondition::set_text(const char* a0) -> void { joan::bind::member<&::game_b93_01::H::set_text>(this, a0); }  // 0x467300
auto joan::orig::EndCondition::set_subtext(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::set_text_5>(this, a0)); }  // 0x467390
