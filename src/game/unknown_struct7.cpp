// game/unknown_struct7.cpp: functions whose original source file is unknown (formerly game/_unplaced/misc_004de9f0.cpp;
// re/attribution.csv: between ww/speech_s.cpp and ww/t_scrres.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdarg>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/records.hpp"

// ---------------------------------------- from modules/game_b100_07.cpp (game/b100_07)
namespace {
namespace game_b100_07 {

using u32 = std::uint32_t;

using i32 = std::int32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ 0x004de9f0 AnimDb ctor (the global
    // b_a47c01, static init 0x00567730)
    void* zero_7() {
        memset_(this, 0, static_cast<u32>(sizeof(joan::orig::AnimDb)));  // 7 on x86
        return this;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_07", name, addr, fn)
B100("Struct7::zero", 0x004de9f0, &H::zero_7);
#undef B100

}  // namespace game_b100_07
}  // namespace
