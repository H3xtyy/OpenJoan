// game/unitlvl.cpp: functions of the original d:\ww\unitlvl.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b87_04.cpp (game/b87_04)
//
//    - UnitLevel::distribute 0x004a95a0 (no args) (unitlvl.cpp): walks the list +0x1aa from the
//      back; entries whose def (0x0050d240 on 0xa56a50) needs points get them through 0x004949f0
//      while the pools +0x12a / +0x12c allow, def kind 9 entries with level +6 < 10 are collected
//      (0x0058eb30 realloc, line g(0xa076b4) + 0x31) and then get the rest of +0x12a one by one
//      at random (0x00590540); buffer freed (0x0058ed70, line g + 0x59).
namespace {
namespace game_b87_04 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

std::int32_t stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

void* const kDefs = obj(d::g_upgrade_res);    // unit defs (0x0050d240 get, 0x00531ed0)
void* const kArrays = obj(d::g_misc);  // array helper (search / insert / remove / random)
void* const kMemory = obj(d::g_mem);  // allocator (0x0058eb30 realloc / 0x0058ed70 free)
const char* const kUnitLvlCpp = str(d::s_d_ww_unitlvl_cpp);  // "d:\ww\unitlvl.cpp"

JOAN_COUNTER(c_distribute, "UnitLevel::distribute");

using Entry = jo::UpgradeListEntry;

jo::UpgradeItem* def_of(const Entry* entry) {  // 0x0050d240
    return joan::orig::arg::as<jo::UpgradeItem*>(joan::orig::as_table(kDefs)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(entry->id))));
}

struct H {
    void* list() { return &jo::self<jo::Unit>(this)->upgrades; }  // the UpgradeList
    std::int32_t list_count() { return joan::orig::arg::as<std::int32_t>(joan::orig::as_upgrade_list(list())->count()); }  // 0x00531360
    Entry* list_get(std::int32_t i) { return joan::orig::arg::as<Entry*>(joan::orig::as_upgrade_list(list())->get(joan::orig::arg::i32(i))); }  // 0x005313a0

    // ---- 0x004a95a0 UnitLevel::distribute ---------------------------------------------------
    void distribute() {
        c_distribute.hit();
        std::int32_t n = 0;                // [ebp-0x8]
        std::int32_t* buf = nullptr;       // [ebp-0xc]
        std::int32_t i = list_count();     // [ebp-0x10]
        for (; i > 0; --i) {
            Entry* const entry = list_get(i);
            jo::UpgradeItem* const def = def_of(entry);
            if (entry->cur < entry->max) {
                for (std::int32_t j = entry->cur; j < entry->max; ++j) {
                    const std::int8_t r = joan::orig::arg::as<std::int8_t>(joan::orig::as_upgrade_res(kDefs)->is_special(joan::orig::arg::i32(static_cast<std::int32_t>(entry->id))));  // 0x00531ed0
                    if (static_cast<std::int32_t>(r) == 1) {
                        const std::int32_t pool = jo::self<jo::Unit>(this)->stat_points;
                        if (pool < def_of(entry)->cost) break;
                        std::int32_t flag = 0;
                        for (std::int32_t k = 1; k <= list_count(); ++k) {
                            Entry* const e2 = list_get(k);
                            jo::UpgradeItem* const d2 = def_of(e2);
                            if (static_cast<std::int8_t>(def->required[0]) == 0) {
                                flag = 1;
                            } else if (e2->cur != 0 && stricmp_(d2->bonus_text, def->required) == 0) {
                                flag = 1;
                            }
                        }
                        if (flag != 0) joan::orig::as_unit(this)->use_item(joan::orig::arg::mptr(static_cast<void*>(entry)));  // 0x004949f0
                    } else {
                        const std::int32_t pool = jo::self<jo::Unit>(this)->attr_points;
                        if (pool < def_of(entry)->cost) break;
                        joan::orig::as_unit(this)->use_item(joan::orig::arg::mptr(static_cast<void*>(entry)));  // 0x004949f0
                    }
                }
            } else if (def_of(entry)->type == 9 && static_cast<std::int16_t>(entry->level) < 10) {
                n = n + 1;
                buf = joan::orig::arg::as<std::int32_t*>(joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(static_cast<void*>(buf)), joan::orig::arg::u32(n << 2), joan::orig::arg::str(kUnitLvlCpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a076b4)) + 0x31)));  // 0x0058eb30
                buf[n - 1] = i;
            }
            if (jo::self<jo::Unit>(this)->attr_points <= 0 && jo::self<jo::Unit>(this)->stat_points <= 0) break;
        }
        for (i = jo::self<jo::Unit>(this)->stat_points; i > 0; --i) {
            if (n == 0) break;
            std::int32_t r = joan::orig::as_misc(kArrays)->random(joan::orig::arg::i32(n));  // 0x00590540
            for (std::int32_t m = 0; m < n; ++m) {
                if (r == 0 && buf[m] != 0) {
                    Entry* const e3 = list_get(buf[m]);
                    e3->level = static_cast<std::uint16_t>(e3->level + 1);
                    jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->stat_points) = static_cast<std::uint16_t>(jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->stat_points) - 1);
                    if (static_cast<std::int16_t>(e3->level) >= 10) {
                        buf[m] = 0;
                        n = n - 1;
                    }
                    break;
                }
                if (buf[m] != 0) r = r - 1;
            }
        }
        if (buf != nullptr) {
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(static_cast<void*>(buf)), joan::orig::arg::str(kUnitLvlCpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a076b4)) + 0x59));  // 0x0058ed70
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_04", name, addr, fn)
B("UnitLevel::distribute", 0x004a95a0, &H::distribute);
#undef B

}  // namespace game_b87_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Unit::distribute_levels() -> void { joan::bind::member<&::game_b87_04::H::distribute>(this); }  // 0x4a95a0
