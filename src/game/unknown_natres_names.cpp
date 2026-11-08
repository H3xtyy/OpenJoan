// game/unknown_natres_names.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/natres_names.cpp; re/attribution.csv: between ww/natres.cpp and ww/pemires.cpp). They move to their
// original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdarg>
#include <cstddef>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
namespace {
namespace game_b84_06 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_nametab, "NameTable::find");

using joan::orig::helpers::stricmp_;

struct H {
    // ------------------------------------------------------------------ 0x004b91a0
    std::int32_t name_table_find(const char* name) {
        c_nametab.hit();
        for (std::int32_t i = 0; i < jo::self<jo::NationRes>(this)->count; ++i) {
            if (stricmp_(jo::self<jo::NationRes>(this)->nations[i].name, name) == 0) return i + 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("NameTable::find", 0x004b91a0, &H::name_table_find);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::NameTable::find_4b91a0(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_06::H::name_table_find>(this, a0)); }  // 0x4b91a0
