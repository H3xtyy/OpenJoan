// game/unknown_crc8.cpp: functions whose original source file is unknown (formerly game/_unplaced/crc8.cpp;
// re/attribution.csv: between ww/world_s.cpp and ww/ilib/angle.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"

// ---------------------------------------- from modules/game_b102_06.cpp (game/b102_06)
namespace {
namespace game_b102_06 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

JOAN_COUNTER(c_crc8, "crc8");

// ---- CRC 0x00576030 ----
u16 crc_step(u16 ax) {
    static const u16 kXor[8] = {0xa580, 0x52c0, 0x2960, 0x14b0, 0x0a58, 0x052c, 0x0296, 0x014b};
    for (int i = 0; i < 8; ++i) {
        if (((ax >> 8) & (0x80u >> i)) != 0) ax = static_cast<u16>(ax ^ kXor[i]);
    }
    return ax;
}
u32 JOAN_STDCALL crc8(const u8* p, u32 len) {
    c_crc8.hit();
    if (len == 0) return 0;
    u16 ax = *p++;
    if (--len != 0) {
        do {
            ax = static_cast<u16>(((ax & 0xffu) << 8) | *p++);
            ax = crc_step(ax);
        } while (--len != 0);
    }
    ax = static_cast<u16>(ax << 8);
    ax = crc_step(ax);
    return ax & 0xffu;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b102_06", name, addr, fn)
B("crc8", 0x00576030, &crc8);
#undef B

}  // namespace game_b102_06
}  // namespace
