// game/unknown_modelres_pre.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/modelres_pre.cpp; re/attribution.csv: between ww/lightsys.cpp and ww/modelres.cpp). They move to their
// original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../canvas_api.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

struct H {
    // ------------------------------------------------------------------------ 0x004b0980
    void* ctor_l() {
        jo::self<jo::ModelRes>(this)->strings.construct();  // 0x005a1730
        jo::self<jo::ModelRes>(this)->loaded = 0;
        jo::self<jo::ModelRes>(this)->capacity = 0;
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("ObjL::ctor", 0x004b0980, &H::ctor_l);
#undef B96

}  // namespace game_b96_02
}  // namespace
