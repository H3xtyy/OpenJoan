// game/unknown_i_obj0e.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_obj0e.cpp;
// re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "sound_slots.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::img::obj;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::hook::global;
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

using i16 = std::int16_t;

void* const kHeap = obj(d::g_sound3d_system);

JOAN_COUNTER(c_free_all, "Res13::free_all_491770");

struct H {
    // ------------------------------------------------------------------------ 0x00491770
    u32 free_all() {
        c_free_all.hit();
        jo::Panels* const res = jo::self<jo::Panels>(this);
        jo::Sound3D** const slots[4] = {&res->source_a, &res->source_b, &res->source_c, &res->source_d};  // +0x4, +0x10, +0x8, +0xc
        for (std::int32_t k = 0; k < 4; ++k) {
            if (*slots[k] != nullptr) {
                joan::orig::as_sound3dsystem(kHeap)->delete_source(joan::orig::arg::mptr(*slots[k]));  // 0x005a0cf0
                *slots[k] = nullptr;
            }
        }
        u32 r = 0;
        for (i16 i = 1; i <= joan::sound_slots::count(); ++i) {
            jo::StreamSlot* const e = joan::orig::arg::as<jo::StreamSlot*>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(i)));  // 0x004cf690
            if (e->source != nullptr) {
                joan::orig::as_sound3dsystem(kHeap)->delete_source(joan::orig::arg::mptr(e->source));  // 0x005a0cf0
                e->source = nullptr;
            }
            r = thiscall<u32>(fn::StreamSlot_release, e);  // 0x004cea40
        }
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Res13::free_all_491770", 0x00491770, &H::free_all);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using joan::hook::global;

void JOAN_CDECL init_a39c54() { global<float>(d::g_a39c54) = global<float>(d::g_pi_a39c48) / 4.0f; }  // 0x004918a0

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("init_a39c54", 0x004918a0, &init_a39c54);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

JOAN_COUNTER(c_cursor, "MouseCursor::move");

[[maybe_unused]] constexpr img::Addr kFiles = d::g_misc, kCanvas = d::g_vga_back;

struct H {
    // ------------------------------------------------------------------ 0x004918d0
    // eax: this on the early exit, else the last call's result (update_cursor / call_30).
    std::uintptr_t cursor_move(std::int32_t x, std::int32_t y) {
        c_cursor.hit();
        if (jo::view<std::int8_t>(jo::self<jo::MouseCursor>(this)->processing) != 0 || jo::self<jo::MouseCursor>(this)->icon_bitmap == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        jo::view<std::int8_t>(jo::self<jo::MouseCursor>(this)->processing) = 1;
        joan::orig::as_canvas(this)->callback_597250(static_cast<jo::Canvas*>(obj(kCanvas)), 0u);  // 0x00597250
        joan::orig::as_canvas(this)->callback_597350(static_cast<jo::Canvas*>(obj(kCanvas)), 0u);  // 0x00597350
        if (jo::view<std::int8_t>(jo::self<jo::MouseCursor>(this)->clip_flag) != 0) {
            x = x > jo::self<jo::MouseCursor>(this)->clip_x1 ? x : jo::self<jo::MouseCursor>(this)->clip_x1;
            y = y > jo::self<jo::MouseCursor>(this)->clip_y1 ? y : jo::self<jo::MouseCursor>(this)->clip_y1;
            x = x < jo::self<jo::MouseCursor>(this)->clip_x2 ? x : jo::self<jo::MouseCursor>(this)->clip_x2;
            y = y < jo::self<jo::MouseCursor>(this)->clip_y2 ? y : jo::self<jo::MouseCursor>(this)->clip_y2;
            joan::orig::as_rect(this)->extend(joan::orig::arg::i32(x), joan::orig::arg::i32(y));  // 0x00488ac0
        }
        jo::self<jo::MouseCursor>(this)->x1 = x - jo::self<jo::MouseCursor>(this)->hot_spot_x;
        jo::self<jo::MouseCursor>(this)->y1 = y - jo::self<jo::MouseCursor>(this)->hot_spot_y;
        jo::self<jo::MouseCursor>(this)->x2 = jo::self<jo::MouseCursor>(this)->x1 + jo::self<jo::MouseCursor>(this)->width - 1;
        jo::self<jo::MouseCursor>(this)->y2 = jo::self<jo::MouseCursor>(this)->y1 + jo::self<jo::MouseCursor>(this)->height - 1;
        std::int32_t draw;
        if (jo::view<std::int8_t>(jo::self<jo::MouseCursor>(this)->hide_flag) != 0)
            draw = 0;
        else if (jo::view<std::int8_t>(jo::self<jo::MouseCursor>(this)->frozen) == 0)
            draw = 1;
        else
            draw = joan::orig::arg::as<std::int32_t>(joan::orig::as_rect(this)->overlaps(joan::orig::arg::i32(jo::self<jo::MouseCursor>(this)->old_x1), joan::orig::arg::i32(jo::self<jo::MouseCursor>(this)->old_y1), joan::orig::arg::i32(jo::self<jo::MouseCursor>(this)->old_x2), joan::orig::arg::i32(jo::self<jo::MouseCursor>(this)->old_y2))) == 0  // 0x0045c7f0
                       ? 1
                       : 0;
        if (jo::view<std::int8_t>(jo::self<jo::MouseCursor>(this)->clip_flag) != 0) joan::orig::as_canvas(this)->callback_5972e0(static_cast<jo::Canvas*>(obj(kCanvas)), 0u);  // 0x005972e0
        if (draw != 0) joan::orig::as_canvas(this)->callback_597200(static_cast<jo::Canvas*>(obj(kCanvas)), 0u);                   // 0x00597200
        if (jo::view<std::int8_t>(jo::self<jo::MouseCursor>(this)->clip_flag) != 0) joan::orig::as_canvas(this)->callback_597390(static_cast<jo::Canvas*>(obj(kCanvas)));      // 0x00597390
        std::uint32_t r = 0;
        if (draw != 0) r = joan::orig::arg::as<std::uint32_t>(joan::orig::as_mouse_cursor(this)->update_cursor(obj(kCanvas)));  // 0x0045b4c0
        if (draw == 0) r = joan::orig::as_screen(this)->call_30(obj(kCanvas), 0);                   // 0x0042d530
        jo::view<std::int8_t>(jo::self<jo::MouseCursor>(this)->processing) = 0;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("MouseCursor::move", 0x004918d0, &H::cursor_move);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

using joan::call::cdecl_call;

using joan::orig::helpers::op_delete;  // operator delete

struct H {
    // ------------------------------------------------------------------ deleting destructors
    template <fn::Code Dtor>
    H* deleting_dtor(u32 flags) {
        thiscall<u32>(Dtor, this);
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }

    void nop0() {}
    void nop1(u32) {}
    bool check_491de0(u32) { return true; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("Obj::deleting_dtor_491b10", 0x00491b10, &H::deleting_dtor<fn::Unit_dtor_462b30>);  // 0x00462b30
B("Obj::nop_491d00", 0x00491d00, &H::nop0);
B("Obj::nop_491d20", 0x00491d20, &H::nop0);
B("Obj::nop_491d40", 0x00491d40, &H::nop0);
B("Obj::nop_491d60", 0x00491d60, &H::nop0);
B("Obj::nop_491d80", 0x00491d80, &H::nop0);
B("Obj::nop_491da0", 0x00491da0, &H::nop1);
B("Obj::nop_491dc0", 0x00491dc0, &H::nop1);
B("Obj::check_491de0", 0x00491de0, &H::check_491de0);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

struct H {
    // ------------------------------------------------------------------ 0x00491b70
    std::uintptr_t init_491b70(u32 type) {
        const std::uintptr_t r = joan::orig::arg::as<u32>(joan::orig::as_map_object(this)->set_kind(joan::orig::arg::i16(type)));  // 0x004178e0
        jo::self<jo::Order0c_b>(this)->u32_c2 = 1;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::init_491b70", 0x00491b70, &H::init_491b70);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03 {

using i32 = std::int32_t;

struct H {
    // ------------------------------------------------------------------ 0x00491cb0
    H* self_491cb0() { return this; }
    // ------------------------------------------------------------------ 0x00491e00 / 0x00492590
    i32 zero() { return 0; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Obj::self_491cb0", 0x00491cb0, &H::self_491cb0);
B("Obj::zero_491e00", 0x00491e00, &H::zero);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::addr;

struct H {
    // ------------------------------------------------------------------ 0x00491c40
    std::uintptr_t cursor_set_mode(i32 k, u32 v) {
        jo::self<jo::MouseCursor>(this)->mode = k;
        if (k == 1) {
            jo::self<jo::MouseCursor>(this)->mode_value = v;
            return v;
        }
        if (k == 2) jo::self<jo::MouseCursor>(this)->mode_value = v;
        return addr(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("MouseCursor::set_mode", 0x00491c40, &H::cursor_set_mode);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05_2 {

// ---------------------------------------------------------------------- static initialisers
template <fn::Code Worker>
void JOAN_CDECL static_init() { cdecl_call<void>(Worker); }

struct H {
    // ------------------------------------------------------------------ trivial
    H* self() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("init_490950", 0x00490950, &static_init<fn::init_a39c48>);
B("Obj::self_490930", 0x00490930, &H::self);
#undef B

}  // namespace game_b105_05_2
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

using joan::hook::x87::Fpu;

void pi_global(img::Addr out) {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(out) = static_cast<float>(Fpu{true}.mul(a, global<double>(d::dbl_4)));
}
static void JOAN_CDECL init_a39c48() { pi_global(d::g_pi_a39c48); }                             // 0x00490970

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("init_a39c48", 0x00490970, &init_a39c48);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03_2 {

// ---- static initialisers (CRT init table) ----
void JOAN_CDECL init_4909a0() { joan::orig::init_a39c4c(); }
void JOAN_CDECL init_490ad0() { joan::orig::init_a39c50(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("init_4909a0", 0x004909a0, &init_4909a0);
B("init_490ad0", 0x00490ad0, &init_490ad0);
#undef B

}  // namespace game_b102_03_2
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

void JOAN_CDECL init_a39c4c() { global<float>(d::g_a39c4c) = global<float>(d::g_pi_a39c48) / global<float>(d::flt_2); }

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("init_a39c4c", 0x00490a30, &init_a39c4c);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04_2 {

using joan::hook::global;

using joan::hook::global;

static void JOAN_CDECL init_a39c50() { global<float>(d::g_a39c50) = global<float>(d::g_pi_a39c48) / global<float>(d::flt_3); }  // 0x00490b90

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("init_a39c50", 0x00490b90, &init_a39c50);
#undef B

}  // namespace game_b100_04_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_a39c4c() -> void { joan::bind::free<&::game_b98_02::init_a39c4c>(); }  // 0x490a30
auto joan::orig::init_a39c50() -> void { joan::bind::free<&::game_b100_04_2::init_a39c50>(); }  // 0x490b90
auto joan::orig::SoundList::free_all() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_14::H::free_all>(this)); }  // 0x491770
auto joan::orig::MouseCursor::move(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b83_07::H::cursor_move>(this, a0, a1)); }  // 0x4918d0
auto joan::orig::MouseCursor::set_mode(std::int32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_04::H::cursor_set_mode>(this, a0, a1)); }  // 0x491c40
