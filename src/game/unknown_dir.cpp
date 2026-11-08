// game/unknown_dir.cpp: functions whose original source file is unknown (formerly game/_unplaced/dir.cpp;
// re/attribution.csv: segment edge ww/ilib/db.cpp|ww/ilib/dynarr.cpp; class/call graph -> ww/ilib/dyna). They move to
// their original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../platform/win32_types.hpp"
#include <cstring>
#include <cstdint>
#include <cstdarg>
#include <cstddef>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../original/engine.hpp"
#include "../original/gameset.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"

// ---------------------------------------- from modules/game_b103_08.cpp (game/b103_08)
namespace {
namespace game_b103_08 {

using i32 = std::int32_t;

using joan::call::cdecl_call;

// ---------------------------------------------------------------------------- 0x00582bf0
static i32 JOAN_CDECL stricmp_wrap(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("stricmp_wrap_582bf0", 0x00582bf0, &stricmp_wrap);
#undef B

}  // namespace game_b103_08
}  // namespace

// ---------------------------------------- from ilib/gameset.cpp (ilib/dir.cpp)
namespace {
namespace ilib_gameset {

using joan::orig::Directory;

JOAN_REPLACE("ilib/dir.cpp", "Directory::Directory", 0x00582ad0, &Directory::construct);

}  // namespace ilib_gameset
}  // namespace

// ---------------------------------------- from ilib/gameset.cpp (ilib/dir.cpp)
namespace {
namespace ilib_gameset {

using joan::orig::Directory;

JOAN_REPLACE("ilib/dir.cpp", "Directory::read", 0x00582b00, &Directory::read);

}  // namespace ilib_gameset
}  // namespace
