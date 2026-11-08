// game/unknown_flock_x1.cpp: functions whose original source file is unknown (formerly game/_unplaced/flock_x1.cpp;
// re/attribution.csv: between ww/f_shop.cpp and ww/flocksys.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../runtime/replay/replay.hpp"

// ---------------------------------------- from modules/game_b105_03.cpp (game/b105_03)
namespace {
namespace game_b105_03 {

struct H {
    // ---------------------------------------------------------------- empty stubs
    void stub0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_03", name, addr, fn)
B("Obj::stub_46db80", 0x0046db80, &H::stub0);
#undef B

}  // namespace game_b105_03
}  // namespace
