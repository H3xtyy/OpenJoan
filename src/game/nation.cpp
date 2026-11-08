// game/nation.cpp: functions of the original d:\ww\nation.cpp (re/attribution.csv).
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
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b87_04.cpp (game/b87_04)
//
//    - Nation::clear 0x00499380 (no args) (nation.cpp): 0x004b40d0, then frees the three lists
//      +0x1531 / +0x1539 / +0x1541 (lines g(0xa017ec) + 7 / 0xf / 0x17) and zeroes the counts.
namespace {
namespace game_b87_04 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

void* const kMemory = obj(d::g_mem);  // allocator (0x0058eb30 realloc / 0x0058ed70 free)
const char* const kNationCpp = str(d::s_d_ww_nation_cpp);   // "d:\ww\nation.cpp"

JOAN_COUNTER(c_clear, "Nation::clear");

struct H {
    // ---- 0x00499380 Nation::clear ------------------------------------------------------------
    void free_field(jo::ArrI16*& field, std::int32_t line_add) {
        if (field == nullptr) return;
        joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(field), joan::orig::arg::str(kNationCpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a017ec)) + line_add));  // 0x0058ed70
        field = nullptr;
    }
    void clear() {
        c_clear.hit();
        joan::orig::as_nation(this)->mark();  // 0x004b40d0
        jo::self<jo::Nation>(this)->leader_count = 0;
        free_field(jo::self<jo::Nation>(this)->leaders, 7);
        jo::self<jo::Nation>(this)->leader_cap = 0;
        jo::self<jo::Nation>(this)->idle_count = 0;
        free_field(jo::self<jo::Nation>(this)->idle_units, 0xf);
        jo::self<jo::Nation>(this)->idle_cap = 0;
        jo::view<std::uint16_t>(jo::self<jo::Nation>(this)->station_count) = 0;
        free_field(jo::self<jo::Nation>(this)->stations, 0x17);
        jo::self<jo::Nation>(this)->station_cap = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_04", name, addr, fn)
B("Nation::clear", 0x00499380, &H::clear);
#undef B

}  // namespace game_b87_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Nation::clear() -> void { joan::bind::member<&::game_b87_04::H::clear>(this); }  // 0x499380
