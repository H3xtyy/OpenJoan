// game/unknown_mode.cpp: functions whose original source file is unknown (formerly game/_unplaced/mode.cpp;
// re/attribution.csv: between ww/lightsys.cpp and ww/modelres.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
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
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;

using joan::img::obj;

}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03 {

struct H {
    // ------------------------------------------------------------------ 0x004b08a0 (dead code)
    void copy_preset_4b08a0(i32 i) {
        if (i >= 0 && i < 1) std::memcpy(this, img::obj(d::g_a016a0 + i * 0x78), 0x78);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Obj::copy_preset_4b08a0", 0x004b08a0, &H::copy_preset_4b08a0);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

using u32 = std::uint32_t;

void* JOAN_CDECL table_a016a0(i32 i) {  // 0x004b08f0
    if (i >= 0 && i < 1) return obj(d::g_a016a0 + static_cast<u32>(i * 0x78));
    return nullptr;
}

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("table_a016a0", 0x004b08f0, &table_a016a0);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

using u8 = std::uint8_t;

void JOAN_CDECL select_preset(i32 n) {
    if (n >= 0 && n < 1) std::memcpy(obj(d::g_a48858), static_cast<u8*>(obj(d::g_a016a0)) + n * 0x78, 0x78);
}

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("select_preset", 0x004b0930, &select_preset);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::table_a016a0(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b93_02::table_a016a0>(a0)); }  // 0x4b08f0
auto joan::orig::select_preset(std::int32_t a0) -> void { joan::bind::free<&::game_b97_02::select_preset>(a0); }  // 0x4b0930
