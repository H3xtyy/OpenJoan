// game/unknown_uscrres_pre.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/uscrres_pre.cpp; re/attribution.csv: between ww/upgrares.cpp and ww/uscrres.cpp). They move to their
// original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstddef>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ small ones
    void* zero_9() {  // 0x005460e0 UScrRes ctor (the global b_a48cf8, static init 0x0056a050)
        memset_(this, 0, static_cast<u32>(sizeof(joan::orig::UScrRes)));  // 9 on x86
        return this;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("Struct9::zero", 0x005460e0, &H::zero_9);
#undef B98

}  // namespace game_b98_04
}  // namespace
