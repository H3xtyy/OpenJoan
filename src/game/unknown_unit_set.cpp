// game/unknown_unit_set.cpp: functions whose original source file is unknown (formerly game/_unplaced/unit_set.cpp;
// re/attribution.csv: between ww/unit.cpp and ww/unitres.cpp). They move to their original file when evidence names it
// (Phase 3).
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
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {
using u16 = std::uint16_t;

using u32 = std::uint32_t;

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b103_05.cpp (game/b103_05)
namespace {
namespace game_b103_05 {

using u16 = std::uint16_t;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ setters
    std::uintptr_t set_1bd(u32 a, u32 b) {  // 0x004fe6a0
        jo::view<u16>(jo::self<jo::Unit>(this)->carried_a) = static_cast<u16>(a);
        jo::view<u16>(jo::self<jo::Unit>(this)->carried_b) = static_cast<u16>(b);
        return (reinterpret_cast<std::uintptr_t>(this) & 0xffff0000u) | (b & 0xffffu);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_05", name, addr, fn)
B("Unit::set_1bd", 0x004fe6a0, &H::set_1bd);
#undef B

}  // namespace game_b103_05
}  // namespace

// ---------------------------------------- from modules/game_b103_05.cpp (game/b103_05)
namespace {
namespace game_b103_05_2 {

struct H {
    H* set_124(u32 v) { jo::view<u16>(jo::self<jo::Unit>(this)->level) = static_cast<u16>(v); return this; }  // 0x004fe830
    H* set_12e(u32 v) { jo::view<u16>(jo::self<jo::Unit>(this)->i16_12e) = static_cast<u16>(v); return this; }  // 0x004fe860
    H* set_161(u32 v) { jo::view<u16>(jo::self<jo::Unit>(this)->stats[6]) = static_cast<u16>(v); return this; }  // 0x004fe890
    H* set_165(u32 v) { jo::view<u16>(jo::self<jo::Unit>(this)->stats[8]) = static_cast<u16>(v); return this; }  // 0x004fe9c0
    H* set_167(u32 v) { jo::view<u32>(jo::self<jo::Unit>(this)->idle_time) = v; return this; }                    // 0x004fe9f0
    H* set_141(u32 v) { jo::view<u32>(jo::self<jo::Unit>(this)->regen_hp) = v; return this; }                    // 0x004fea20
    H* set_14d(u32 v) { jo::view<u32>(jo::self<jo::Unit>(this)->regen_ep) = v; return this; }                    // 0x004fea50
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_05", name, addr, fn)
B("Unit::set_124", 0x004fe830, &H::set_124);
B("Unit::set_12e", 0x004fe860, &H::set_12e);
B("Unit::set_161", 0x004fe890, &H::set_161);
B("Unit::set_165", 0x004fe9c0, &H::set_165);
B("Unit::set_167", 0x004fe9f0, &H::set_167);
B("Unit::set_141", 0x004fea20, &H::set_141);
B("Unit::set_14d", 0x004fea50, &H::set_14d);
#undef B

}  // namespace game_b103_05_2
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
namespace {
namespace game_b92_04 {

using joan::img::obj;

void* const kRandom = obj(d::g_misc);

JOAN_COUNTER(c_roll_morale, "Unit::roll_morale");

i32 stat_497e10(void* army, i32 kind, i32 id) { return joan::orig::arg::as<i32>(joan::orig::as_modifiers(army)->sum(joan::orig::arg::i32(kind), joan::orig::arg::i32(id))); }  // 0x00497e10

struct H {
    // ------------------------------------------------------------------------ 0x004fe8c0
    void roll_morale(i32 n) {
        c_roll_morale.hit();
        if (n < static_cast<i32>(jo::self<jo::Unit>(this)->stats[7]) && static_cast<i32>(jo::self<jo::Unit>(this)->group_id) != 0) {
            u8* const leader = joan::orig::arg::as<u8*>(joan::orig::as_unit(this)->leader());  // 0x004216d0
            if (leader != nullptr && jo::as_unit(leader)->modifiers != nullptr) {
                if (stat_497e10(jo::self<jo::Unit>(this)->modifiers, 0x13, static_cast<i32>(jo::as_unit(leader)->id)) == 0) return;
                const i32 v = stat_497e10(jo::self<jo::Unit>(this)->modifiers, 0x12, static_cast<i32>(jo::as_unit(leader)->id));
                if (v < static_cast<i32>(jo::self<jo::Unit>(this)->stats[7])) return;
                if (n < v) {
                    jo::view<u16>(jo::self<jo::Unit>(this)->stats[7]) = static_cast<u16>(v);
                    return;
                }
            }
        }
        jo::view<u16>(jo::self<jo::Unit>(this)->stats[7]) = joan::orig::arg::as<u16>(joan::orig::as_misc(kRandom)->limit_int(joan::orig::arg::i32(n), i32{0}, i32{0x64}));  // 0x00590aa0
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Unit::roll_morale", 0x004fe8c0, &H::roll_morale);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

using joan::hook::global;

JOAN_COUNTER(c_set_morale, "Unit::set_morale");

struct H {
    // ------------------------------------------------------------------------ 0x004fea80
    void set_morale(i32 v) {
        c_set_morale.hit();
        if (v < 0)
            jo::view<u16>(jo::self<jo::Unit>(this)->morale) = 0;
        else if (v > 0x64)
            jo::view<u16>(jo::self<jo::Unit>(this)->morale) = 0x64;
        else
            jo::view<u16>(jo::self<jo::Unit>(this)->morale) = static_cast<u16>(v);
        if (jo::self<jo::Unit>(this)->morale != 0) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->mounted()) != 0) {  // 0x00430190
            joan::orig::as_unit(this)->set_action(i32{0x4e}, i32{0}, i32{1});  // 0x00501190
            const u32 x = joan::orig::arg::as<u32>(joan::orig::as_cell_object(this)->find_anim_state());                // 0x00504010
            joan::orig::as_cell_object(this)->set_anim_state(joan::orig::arg::i32(x), joan::orig::arg::i32(global<u32>(d::g_now)), i32{-1});  // 0x004ff9f0
            jo::self<jo::Unit>(this)->mount_busy = 0;
        } else {
            joan::orig::as_unit(this)->pick_idle_anim(i32{0});  // 0x00509000
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("Unit::set_morale", 0x004fea80, &H::set_morale);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

struct H {
    void nop2(u32, u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::nop_4feb60", 0x004feb60, &H::nop2);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Unit::roll_morale(std::int32_t a0) -> void { joan::bind::member<&::game_b92_04::H::roll_morale>(this, a0); }  // 0x4fe8c0
auto joan::orig::Unit::set_morale(std::int32_t a0) -> void { joan::bind::member<&::game_b93_02::H::set_morale>(this, a0); }  // 0x4fea80
