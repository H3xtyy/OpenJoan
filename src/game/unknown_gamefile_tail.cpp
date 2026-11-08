// game/unknown_gamefile_tail.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/gamefile_tail.cpp; re/attribution.csv: between ww/gamefile.cpp and ww/gameplay.cpp). They move to
// their original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
namespace {
namespace game_b102_02 {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::img::obj;

using joan::call::thiscall;
using joan::orig::helpers::addr;

struct Vec3 {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers (List43::add 0x004eef00 takes V3 {float x, y, z})
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_unit_msg, "Unit::on_message");

struct H {
    // 0x00479270
    std::uintptr_t unit_on_message(u32 arg) {
        c_unit_msg.hit();
        u8* const msg = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_a5ccd0))->at_485f20(joan::orig::arg::u32(static_cast<i32>(static_cast<i16>(arg)))));  // 0x00485f20
        if (jo::view<i8>(jo::self<jo::Order45>(this)->target_name[0]) != 0) {
            jo::self<jo::Order45>(this)->target_id = joan::orig::arg::as<u32>(joan::orig::as_item_list(obj(d::g_items))->find_by_name(joan::orig::arg::str(jo::self<jo::Order45>(this)->target_name)));  // 0x0049f420
            std::memset(joan::orig::arg::mptr(jo::self<jo::Order45>(this)->target_name), 0, sizeof jo::self<jo::Order45>(this)->target_name);  // 0x41  // memset
        }
        const std::uintptr_t keep = (addr(msg) & ~std::uintptr_t{0xff}) | 1u;
        if (jo::view<u32>(jo::as_save_entry(msg)->type) != 0x200) return keep;
        if (jo::self<jo::Order45>(this)->interact_trigger == 0) return keep;
        if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(jo::self<jo::Order45>(this)->target_id)) != 0) return 1;  // 0x00425d50
        if (jo::view<i32>(jo::as_save_entry(msg)->ref_kind) != 1) return 1;  // eax = 1 from the 0x00425d50 test
        const u32 r = thiscall<u32>(fn::Unit_can_target_428600, this, static_cast<i32>(jo::view<i16>(jo::as_save_entry(msg)->ref_id)));  // 0x00428600
        if (static_cast<u8>(r) == 0) return r | 1u;
        const u32 now = global<u32>(d::g_now);
        const Vec3 pos = jo::view<Vec3>(jo::self<jo::Order45>(this)->pos);
        const u32 kind = jo::view<u16>(jo::self<jo::Order45>(this)->index);
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Order45>(this)->target_id)));  // 0x00425d20
        const u32 r2 = joan::orig::arg::as<u32>(joan::orig::as_triggers(obj(d::g_a5ccd0))->add(joan::orig::arg::u32(jo::self<jo::Order45>(this)->interact_trigger), 2u, joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_map_object(unit)->portrait_unit))), 2u, joan::orig::arg::i16(kind), static_cast<std::int8_t>(joan::orig::arg::i32(1)), pos, 0x3f800000u, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::u32(now), joan::orig::arg::u32(now), static_cast<std::int8_t>(joan::orig::arg::i32(1)), 0u, joan::orig::arg::i16(0)));  // 0x004eef00
        return (r2 & 0xffffff00u) | 1u;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("Unit::on_message", 0x00479270, &H::unit_on_message);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_b103_02.cpp (game/b103_02)
namespace {
namespace game_b103_02 {

using joan::img::str;

struct H {
    // ------------------------------------------------------------------ 0x004793e0 / 0x00479420
    // GameFile::name (char[128] at +0x11a): the class from the offset and the address (right after the GameFile
    // functions of ww/gamefile.cpp); joan.exe has no caller (thunks 0x00402dd3 / 0x00402720 unreferenced).
    char* set_name_11a(const char* s) {
        return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::GameFile>(this)->name)), joan::orig::arg::str(s));  // strcpy
    }
    u8* name_11a() { return reinterpret_cast<u8*>(jo::self<jo::GameFile>(this)->name); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_02", name, addr, fn)
B("Obj::set_name_11a", 0x004793e0, &H::set_name_11a);
B("Obj::name_11a", 0x00479420, &H::name_11a);
#undef B

}  // namespace game_b103_02
}  // namespace
