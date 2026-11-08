// game/unknown_top4list.cpp: functions whose original source file is unknown (formerly game/_unplaced/top4list.cpp;
// re/attribution.csv: between ww/ilib/resx.cpp and ww/ilib/se.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/dynarray.hpp"
#include "../x87.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using i16 = std::int16_t;
using u16 = std::uint16_t;

}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

using joan::call::thiscall;

JOAN_COUNTER(c_top4_insert, "Top4::insert");

struct H {
    // ------------------------------------------------------------------ 0x0059dd30
    void top4_insert(i32 key, u32 val) {
        c_top4_insert.hit();
        jo::Top4Entry* const e = &jo::self<jo::Top4>(this)->e0;  // e0..e3
        if (static_cast<i32>(jo::self<jo::Top4>(this)->count) < 4) {
            const i32 n = jo::self<jo::Top4>(this)->count;
            e[n].key = key;
            e[n].value = val;
            jo::view<u16>(jo::self<jo::Top4>(this)->count) = static_cast<u16>(jo::view<u16>(jo::self<jo::Top4>(this)->count) + 1);
            return;
        }
        i32 ck = key;
        u32 cv = val;
        for (i32 i = 0; i < 4; ++i) {
            if (e[i].key < ck) {
                u32 tmp[2];
                joan::orig::as_top4_entry(tmp)->construct();  // 0x0059ec90
                tmp[0] = jo::view<u32>(e[i].key);
                tmp[1] = e[i].value;
                e[i].key = ck;
                e[i].value = cv;
                ck = static_cast<i32>(tmp[0]);
                cv = tmp[1];
            }
        }
    }

    // ------------------------------------------------------------------ 0x0059ded0
    H* top4_clear() {
        jo::view<u16>(jo::self<jo::Top4>(this)->count) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Top4::insert", 0x0059dd30, &H::top4_insert);
B("Top4::clear", 0x0059ded0, &H::top4_clear);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b102_09.cpp (game/b102_09)
namespace {
namespace game_b102_09 {

JOAN_COUNTER(c_pair_remove, "PairList::remove");

struct H {
    // ------------------------------------------------------------------------ 0x0059de40
    std::uintptr_t pair_remove(i32 i) {
        c_pair_remove.hit();
        if (i >= static_cast<i32>(jo::self<jo::PairList>(this)->count) || i < 0)
            return reinterpret_cast<std::uintptr_t>(this);
        std::uintptr_t eax = reinterpret_cast<std::uintptr_t>(this);
        i32 j = i + 1;
        if (j < static_cast<i32>(jo::self<jo::PairList>(this)->count)) {
            jo::Top4Entry* const e = &jo::self<jo::PairList>(this)->e0;  // e0..e3
            for (; j < static_cast<i32>(jo::self<jo::PairList>(this)->count); ++j) {
                const u32 lo = jo::view<u32>(e[j].key);
                const u32 hi = e[j].value;
                jo::view<u32>(e[j - 1].key) = lo;
                e[j - 1].value = hi;
            }
            eax = static_cast<u32>(j);
        }
        const u16 cnt = static_cast<u16>(jo::view<u16>(jo::self<jo::PairList>(this)->count) - 1u);
        jo::view<u16>(jo::self<jo::PairList>(this)->count) = cnt;
        return (eax & 0xffff0000u) | cnt;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_09", name, addr, fn)
B("PairList::remove", 0x0059de40, &H::pair_remove);
#undef B

}  // namespace game_b102_09
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Top4List::insert(std::int32_t a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b99_07::H::top4_insert>(this, a0, a1); }  // 0x59dd30
auto joan::orig::Top4List::clear() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_07::H::top4_clear>(this)); }  // 0x59ded0
