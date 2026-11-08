// game/unknown_battle_pre.cpp: functions whose original source file is unknown (formerly game/_unplaced/battle_pre.cpp;
// re/attribution.csv: between ww/enparticle/peffecta.cpp and ww/battle.cpp). They move to their original file when
// evidence names it (Phase 3).
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
#include "../render/overrides.hpp"
#include "../original/bind.hpp"

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
namespace {
namespace game_leaves80a {

std::uint8_t* bytes(const void* p, std::int32_t offset = 0) {
    return static_cast<std::uint8_t*>(const_cast<void*>(p)) + offset;
}

struct H {
    // 0x0041bf10: returns only al (the rest of eax is this + i in the original; callers use al).
    std::uint8_t byte_at(std::int32_t i) { return *(bytes(this, i) + 1); }
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Bytes::at", 0x0041bf10, &H::byte_at);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Bytes::at(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_leaves80a::H::byte_at>(this, a0)); }  // 0x41bf10
