// game/unknown_gadget.cpp: functions whose original source file is unknown (formerly game/_unplaced/gadget.cpp;
// re/attribution.csv: between ww/en3d/vshader2.cpp and ww/en2d/bmpres.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::hook::x87::Fpu;

using joan::call::cdecl_call;

using i16 = std::int16_t;
using u16 = std::uint16_t;

// true when the double d lies exactly halfway between two floats (then double -> float may differ
// from the direct rounding of the 64-bit st0 it came from).
bool float_tie(double d) {
    if (!std::isfinite(d)) return false;
    const float f = static_cast<float>(d);
    const double fd = f;
    if (fd == d) return false;
    const float other = std::nextafter(f, d > fd ? std::numeric_limits<float>::infinity()
                                                 : -std::numeric_limits<float>::infinity());
    return (fd + static_cast<double>(other)) * 0.5 == d;
}

using joan::orig::helpers::at;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b105_17.cpp (game/b105_17)
namespace {
namespace game_b105_17 {

using i8 = std::int8_t;

JOAN_COUNTER(c_slide_step, "Widget::slide_step");
JOAN_COUNTER(c_fade_step, "Widget::fade_step");

float kf(img::Addr a) { return global<float>(a); }
double crt_atan(double x) { return joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(x))); }
float crt_atan_f(double x) { return joan::orig::arg::as<float>(joan::orig::libc::atan(joan::orig::arg::f64(x))); }
float crt_cos_f(double x) { return joan::orig::arg::as<float>(joan::orig::libc::cos(joan::orig::arg::f64(x))); }
float crt_sin_f(double x) { return joan::orig::arg::as<float>(joan::orig::libc::sin(joan::orig::arg::f64(x))); }

void JOAN_CDECL init_61dc70() { joan::orig::init_a69cf8(); }
void JOAN_CDECL init_61dcd0() { joan::orig::init_a69cfc(); }
void JOAN_CDECL init_61dd20() { joan::orig::init_a69d00(); }
void JOAN_CDECL init_61dd70() { joan::orig::div4_61dd90(); }

struct H {
    // one axis of 0x0061f450: pos float, words w1 / w2, delta d
    void slide_axis(float& pos, i16& w1, i16& w2, i32 d) {
        if (!(pos >= kf(d::flt_1))) return;
        const Fpu fpu{true};
        if (d > 0) {
            const i32 n = joan::hook::x87::ftol(pos);
            const i32 r = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->limit_int(joan::orig::arg::i32(n), 1, joan::orig::arg::i32(d)));  // 0x00590aa0
            pos = static_cast<float>(fpu.sub(pos, static_cast<double>(r)));
            jo::view<u16>(w1) = static_cast<u16>(jo::view<u16>(w1) + static_cast<u16>(r));
            jo::view<u16>(w2) = static_cast<u16>(jo::view<u16>(w2) + static_cast<u16>(r));
        } else if (d < 0) {
            const i32 n = joan::hook::x87::ftol(pos);
            const i32 r = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->limit_int(joan::orig::arg::i32(n), 1, joan::orig::arg::i32(-d)));  // 0x00590aa0
            pos = static_cast<float>(fpu.sub(pos, static_cast<double>(r)));
            jo::view<u16>(w1) = static_cast<u16>(jo::view<u16>(w1) - static_cast<u16>(r));
            jo::view<u16>(w2) = static_cast<u16>(jo::view<u16>(w2) - static_cast<u16>(r));
        }
    }

    // 0x0061f450
    void slide_step() {
        c_slide_step.hit();
        if (jo::view<i8>(jo::self<jo::Widget>(this)->moving) == 0 || jo::view<i8>(jo::self<jo::Widget>(this)->move_36) != 0) return;
        if (jo::self<jo::Widget>(this)->target_x == jo::self<jo::Widget>(this)->x && jo::self<jo::Widget>(this)->target_y == jo::self<jo::Widget>(this)->y) {
            joan::orig::as_widget(this)->stop_move();  // 0x0061e2d0
            return;
        }
        i32 dt = global<i32>(d::g_frame_elapsed_ms);
        if (jo::view<i32>(jo::self<jo::Widget>(this)->move_arg) > dt) {
            jo::view<i32>(jo::self<jo::Widget>(this)->move_arg) = jo::view<i32>(jo::self<jo::Widget>(this)->move_arg) - dt;
            return;
        }
        dt = dt - jo::view<i32>(jo::self<jo::Widget>(this)->move_arg);
        jo::view<i32>(jo::self<jo::Widget>(this)->move_arg) = 0;
        if (dt == 0) return;
        const Fpu fpu{true};
        if (jo::view<i8>(jo::self<jo::Widget>(this)->resizing) != 0 && (jo::view<i8>(jo::self<jo::Widget>(this)->resize_41) == 0 || jo::view<i8>(jo::self<jo::Widget>(this)->resize_41) == 1)) {
            const i32 ny = static_cast<i32>(jo::self<jo::Widget>(this)->target_y) - static_cast<i32>(jo::self<jo::Widget>(this)->y);
            const i32 nx = static_cast<i32>(jo::self<jo::Widget>(this)->target_x) - static_cast<i32>(jo::self<jo::Widget>(this)->x);
            const double q = fpu.div(static_cast<double>(ny), static_cast<double>(nx));  // fild / fidiv
            const double a = crt_atan(q);                                    // st0 -> fstp qword
            float af = static_cast<float>(a);                                // fst dword [ebp-0xc]
            if (float_tie(a)) af = crt_atan_f(q);                            // exact st0 -> float
            jo::self<jo::Widget>(this)->move_cos = crt_cos_f(a);                                   // fstp dword
            jo::self<jo::Widget>(this)->move_sin = crt_sin_f(static_cast<double>(af));             // fstp dword
        }
        const i32 dx = static_cast<i32>(jo::self<jo::Widget>(this)->target_x) - static_cast<i32>(jo::self<jo::Widget>(this)->x);
        const i32 dy = static_cast<i32>(jo::self<jo::Widget>(this)->target_y) - static_cast<i32>(jo::self<jo::Widget>(this)->y);
        const float accel = joan::orig::arg::as<float>(joan::orig::as_widget(this)->get_3b());  // 0x0061f910
        if (accel < kf(d::k_9cf5c4) || accel > kf(d::k_9cf5c4)) {
            joan::orig::as_widget(this)->set_move_time(joan::orig::arg::i32(dt));  // 0x0061f400
        }
        const float speed = joan::orig::arg::as<float>(joan::orig::as_widget(this)->get_37());  // 0x0061f8f0
        if (!(speed > kf(d::k_9cf5c4))) {
            jo::view<u16>(jo::self<jo::Widget>(this)->x) = static_cast<u16>(jo::view<u16>(jo::self<jo::Widget>(this)->x) + static_cast<u16>(dx));
            jo::view<u16>(jo::self<jo::Widget>(this)->x2) = static_cast<u16>(jo::view<u16>(jo::self<jo::Widget>(this)->x2) + static_cast<u16>(dx));
            jo::view<u16>(jo::self<jo::Widget>(this)->y) = static_cast<u16>(jo::view<u16>(jo::self<jo::Widget>(this)->y) + static_cast<u16>(dy));
            jo::view<u16>(jo::self<jo::Widget>(this)->y2) = static_cast<u16>(jo::view<u16>(jo::self<jo::Widget>(this)->y2) + static_cast<u16>(dy));
            return;
        }
        const float t = static_cast<float>(
            fpu.mul(fpu.div(static_cast<double>(dt), kf(d::flt_1000)), jo::self<jo::Widget>(this)->move_speed));
        jo::view<float>(jo::self<jo::Widget>(this)->move_25) = t * jo::self<jo::Widget>(this)->move_cos + jo::view<float>(jo::self<jo::Widget>(this)->move_25);
        jo::view<float>(jo::self<jo::Widget>(this)->move_29) = t * jo::self<jo::Widget>(this)->move_sin + jo::view<float>(jo::self<jo::Widget>(this)->move_29);
        slide_axis(jo::view<float>(jo::self<jo::Widget>(this)->move_25), jo::self<jo::Widget>(this)->x, jo::self<jo::Widget>(this)->x2, dx);
        slide_axis(jo::view<float>(jo::self<jo::Widget>(this)->move_29), jo::self<jo::Widget>(this)->y, jo::self<jo::Widget>(this)->y2, dy);
    }

    // 0x00620810
    void fade_step() {
        c_fade_step.hit();
        if (jo::view<i8>(jo::self<jo::Widget>(this)->scaling) == 0 || jo::view<i8>(jo::self<jo::Widget>(this)->scale_5f) != 0) return;
        if (!(jo::self<jo::Widget>(this)->scale_target < jo::self<jo::Widget>(this)->scale || jo::self<jo::Widget>(this)->scale_target > jo::self<jo::Widget>(this)->scale)) {
            joan::orig::as_widget(this)->stop_scale();  // 0x0061f340
            return;
        }
        i32 dt = global<i32>(d::g_frame_elapsed_ms);
        if (jo::view<i32>(jo::self<jo::Widget>(this)->scale_arg) > dt) {
            jo::view<i32>(jo::self<jo::Widget>(this)->scale_arg) = jo::view<i32>(jo::self<jo::Widget>(this)->scale_arg) - dt;
            return;
        }
        dt = dt - jo::view<i32>(jo::self<jo::Widget>(this)->scale_arg);
        jo::view<i32>(jo::self<jo::Widget>(this)->scale_arg) = 0;
        if (dt == 0) return;
        const float d = jo::self<jo::Widget>(this)->scale_target - jo::self<jo::Widget>(this)->scale;
        const float accel = joan::orig::arg::as<float>(joan::orig::as_widget(this)->get_6c());  // 0x00620a00
        if (accel < kf(d::k_9cf5c4) || accel > kf(d::k_9cf5c4)) {
            joan::orig::as_widget(this)->set_scale_time(joan::orig::arg::i32(dt));  // 0x006207c0
        }
        const Fpu fpu{true};
        jo::view<float>(jo::self<jo::Widget>(this)->scale_64) = static_cast<float>(fpu.add(
            fpu.mul(fpu.div(static_cast<double>(dt), kf(d::flt_1000)), jo::self<jo::Widget>(this)->scale_speed), jo::view<float>(jo::self<jo::Widget>(this)->scale_64)));
        if (d > kf(d::k_9cf5c4)) {
            const float r = static_cast<float>(thiscall<double>(
                fn::limit_double, obj(d::g_misc), static_cast<double>(jo::view<float>(jo::self<jo::Widget>(this)->scale_64)), 0.0,
                static_cast<double>(d)));  // 0x00590b00 clamp
            jo::view<float>(jo::self<jo::Widget>(this)->scale_64) = jo::view<float>(jo::self<jo::Widget>(this)->scale_64) - r;
            jo::self<jo::Widget>(this)->scale = r + jo::self<jo::Widget>(this)->scale;
        } else {
            const float lim = d * kf(d::flt_m1);
            const float r = static_cast<float>(thiscall<double>(
                fn::limit_double, obj(d::g_misc), static_cast<double>(jo::view<float>(jo::self<jo::Widget>(this)->scale_64)), 0.0,
                static_cast<double>(lim)));  // 0x00590b00 clamp
            jo::view<float>(jo::self<jo::Widget>(this)->scale_64) = jo::view<float>(jo::self<jo::Widget>(this)->scale_64) - r;
            jo::self<jo::Widget>(this)->scale = jo::self<jo::Widget>(this)->scale - r;
        }
    }
};

JOAN_REPLACE("game/b105_17", "init_61dc70", 0x0061dc70, &init_61dc70);
JOAN_REPLACE("game/b105_17", "init_61dcd0", 0x0061dcd0, &init_61dcd0);
JOAN_REPLACE("game/b105_17", "init_61dd20", 0x0061dd20, &init_61dd20);
JOAN_REPLACE("game/b105_17", "init_61dd70", 0x0061dd70, &init_61dd70);
JOAN_REPLACE("game/b105_17", "Widget::slide_step", 0x0061f450, &H::slide_step);
JOAN_REPLACE("game/b105_17", "Widget::fade_step", 0x00620810, &H::fade_step);

}  // namespace game_b105_17
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

// ---------------------------------------------------------------------------- 0x0061dc90
void JOAN_CDECL init_a69cf8() {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a69cf8) = static_cast<float>(a * global<double>(d::dbl_4));  // * 4.0 (exact)
}

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("init_a69cf8", 0x0061dc90, &init_a69cf8);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

void JOAN_CDECL init_a69cfc() {  // 0x0061dcf0
    global<float>(d::g_a69cfc) = global<float>(d::g_a69cf8) / global<float>(d::flt_2);
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("init_a69cfc", 0x0061dcf0, &init_a69cfc);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

static void JOAN_CDECL init_a69d00() {  // 0x0061dd40
    global<float>(d::g_a69d00) = global<float>(d::g_a69cf8) / global<float>(d::flt_3);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("init_a69d00", 0x0061dd40, &init_a69d00);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

JOAN_COUNTER(c_gadget_ctor, "Gadget::ctor");

// 0x0061dd90
void JOAN_CDECL init_div4_61dd90() { global<float>(d::g_a69d04) = global<float>(d::g_a69cf8) / global<float>(d::flt_4); }

struct H {
    // 0x0061de00 Gadget::ctor
    void* gadget_ctor() {
        c_gadget_ctor.hit();
        joan::orig::as_gadget_flags(&jo::self<jo::Gadget>(this)->child)->construct();  // 0x00634b20
        jo::self<jo::Gadget>(this)->vtable = obj(d::vt_Obj_9da64c);
        jo::view<std::uint16_t>(jo::self<jo::Gadget>(this)->x) = 0xffff;
        jo::view<std::uint16_t>(jo::self<jo::Gadget>(this)->y) = 0xffff;
        jo::view<std::uint16_t>(jo::self<jo::Gadget>(this)->x2) = 0xffff;
        jo::view<std::uint16_t>(jo::self<jo::Gadget>(this)->y2) = 0xffff;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->scale) = 0x3f800000;
        jo::view<std::uint16_t>(jo::self<jo::Gadget>(this)->target_x) = 0xffff;
        jo::view<std::uint16_t>(jo::self<jo::Gadget>(this)->target_y) = 0xffff;
        jo::self<jo::Gadget>(this)->move_25 = 0;
        jo::self<jo::Gadget>(this)->move_29 = 0;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->move_cos) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->move_sin) = 0;
        jo::self<jo::Gadget>(this)->moving = 0;
        jo::self<jo::Gadget>(this)->move_36 = 0;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->move_speed) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->move_accel) = 0;
        jo::self<jo::Gadget>(this)->move_arg = 0;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->resize_56) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->resize_5a) = 0;
        jo::view<std::uint16_t>(jo::self<jo::Gadget>(this)->target_w) = 0xffff;
        jo::view<std::uint16_t>(jo::self<jo::Gadget>(this)->target_h) = 0xffff;
        jo::self<jo::Gadget>(this)->resize_41 = 0;
        jo::self<jo::Gadget>(this)->resizing = 0;
        jo::self<jo::Gadget>(this)->resize_40 = 0;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->resize_speed) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->resize_accel) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->resize_arg) = 0;
        jo::self<jo::Gadget>(this)->scale_64 = 0;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->scale_target) = 0xbf800000;
        jo::self<jo::Gadget>(this)->scaling = 0;
        jo::self<jo::Gadget>(this)->scale_5f = 0;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->scale_speed) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Gadget>(this)->scale_accel) = 0;
        jo::self<jo::Gadget>(this)->scale_arg = 0;
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Init::div4_61dd90", 0x0061dd90, &init_div4_61dd90);
B94("Gadget::ctor", 0x0061de00, &H::gadget_ctor);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
namespace {
namespace game_leaves80a {

struct H {
    // 0x0061ddc0: rep movsd of 0x1a dwords (forward copy).
    void* light_params(void* out) {
        const std::uint32_t* src = jo::self<jo::Light>(this)->d3d_light;
        std::uint32_t* dst = static_cast<std::uint32_t*>(out);
        for (std::int32_t n = 0; n < 0x1a; ++n) *dst++ = *src++;
        return out;
    }
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Light::params", 0x0061ddc0, &H::light_params);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b86_01.cpp (game/b86_01)
namespace {
namespace game_b86_01 {

struct H {
    // 0x0061dfb0 destructor: vtable 0x009da64c, then the +4 member's destructor (the
    // original's `this ? this + 4 : 0` base-adjust check; this is never null after the store
    // through it). ret 0. eax = 0x0061e000's.
    std::uint32_t object_dtor() {
        jo::self<jo::Gadget>(this)->vtable = obj(d::vt_Obj_9da64c);
        joan::orig::as_widget_child(reinterpret_cast<std::uint8_t*>(this) + sizeof(void*))->destruct();  // 0x0061e000 (+4 on x86: after the leading pointer)
        return 0;  // a destructor: its eax is never used
    }
};

#define GAME86(name, addr, fn) JOAN_REPLACE("game/b86_01", name, addr, fn)
GAME86("Object::~Object", 0x0061dfb0, &H::object_dtor);
#undef GAME86

}  // namespace game_b86_01
}  // namespace

// ---------------------------------------- from modules/en3d_b87_02.cpp (game/b87_02)
namespace {
namespace en3d_b87_02 {

struct H {
    // 0x0061e000
    void nop_61e000() {}
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b87_02", name, addr, fn)
GAME("Object::nop_61e000", 0x0061e000, &H::nop_61e000);
#undef GAME

}  // namespace en3d_b87_02
}  // namespace

// ---------------------------------------- from modules/game_ui2.cpp (game/ui2)
namespace {
namespace game_ui2 {

#define U2(name, addr, fn) JOAN_REPLACE("game/ui2", name, addr, fn)
U2("Widget::move_to", 0x0061e020, &joan::orig::Widget::move_to);
U2("Widget::scale_to", 0x0061f1b0, &joan::orig::Widget::scale_to);
#undef U2

}  // namespace game_ui2
}  // namespace

// ---------------------------------------- from modules/game_ui8.cpp (game/ui8)
namespace {
namespace game_ui8 {

struct H {
    // 0x0061e270 / 0x0061e2a0 / 0x0061f2e0 / 0x0061f310 (float stored as dword). ret 4.
    void set_move_speed(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->move_speed) = v; }
    void set_move_accel(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->move_accel) = v; }
    void set_scale_speed(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->scale_speed) = v; }
    void set_scale_accel(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->scale_accel) = v; }

    // 0x0061e2d0. ret 0.
    void stop_move() {
        jo::self<jo::Widget>(this)->move_arg = 0;
        jo::self<jo::Widget>(this)->move_25 = 0;
        jo::self<jo::Widget>(this)->move_29 = 0;
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->move_cos) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->move_sin) = 0;
        jo::view<std::uint16_t>(jo::self<jo::Widget>(this)->target_x) = 0xffff;
        jo::view<std::uint16_t>(jo::self<jo::Widget>(this)->target_y) = 0xffff;
        jo::self<jo::Widget>(this)->moving = 0;
        jo::self<jo::Widget>(this)->move_36 = 0;
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->move_speed) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->move_accel) = 0;
    }

    // 0x0061f340. ret 0.
    void stop_scale() {
        jo::self<jo::Widget>(this)->scale_arg = 0;
        jo::self<jo::Widget>(this)->scale_64 = 0;
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->scale_target) = 0xbf800000u;  // -1.0f
        jo::self<jo::Widget>(this)->scaling = 0;
        jo::self<jo::Widget>(this)->scale_5f = 0;
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->scale_speed) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->scale_accel) = 0;
    }
};

#define U8(name, addr, fn) JOAN_REPLACE("game/ui8", name, addr, fn)
U8("Widget::set_move_speed", 0x0061e270, &H::set_move_speed);
U8("Widget::set_move_accel", 0x0061e2a0, &H::set_move_accel);
U8("Widget::stop_move", 0x0061e2d0, &H::stop_move);
U8("Widget::set_scale_speed", 0x0061f2e0, &H::set_scale_speed);
U8("Widget::set_scale_accel", 0x0061f310, &H::set_scale_accel);
U8("Widget::stop_scale", 0x0061f340, &H::stop_scale);
#undef U8

}  // namespace game_ui8
}  // namespace

// ---------------------------------------- from modules/game_b82_11.cpp (game/b82_11)
namespace {
namespace game_b82_11 {

JOAN_COUNTER(c_resize, "Widget::resize_to");
JOAN_COUNTER(c_scale_size, "Widget::scale_size");

std::int32_t s16(std::uint32_t v) { return static_cast<std::int16_t>(v); }
std::int32_t s8(std::uint32_t v) { return static_cast<std::int8_t>(v); }

struct H {
    // ---------------------------------------------------------------- shared by 0x0061e370 / 0x0061ea80
    std::int32_t width() { return s16(jo::view<std::uint16_t>(jo::self<jo::Widget>(this)->x2)) - s16(jo::view<std::uint16_t>(jo::self<jo::Widget>(this)->x)); }
    std::int32_t height() { return s16(jo::view<std::uint16_t>(jo::self<jo::Widget>(this)->y2)) - s16(jo::view<std::uint16_t>(jo::self<jo::Widget>(this)->y)); }

    // Immediate resize to (nw, nh) around the anchor (jump tables 0x0061e8a8 / 0x0061efa8).
    void apply_anchor(std::int32_t anchor, std::int32_t nw, std::int32_t nh) {
        const bool x_low = anchor == 0 || anchor == 3;                    // [9] += dw
        const bool x_mid = anchor == 1 || anchor == 5 || anchor == 6;     // centred
        const bool x_high = anchor == 2 || anchor == 4;                   // [d] -= dw
        const bool y_low = anchor == 0 || anchor == 4 || anchor == 6;     // [b] += dh
        const bool y_mid = anchor == 1;                                   // centred
        const bool y_high = anchor == 2 || anchor == 3 || anchor == 5;    // [f] -= dh
        if (x_low) {
            const std::int32_t d = width() - nw;
            jo::self<jo::Widget>(this)->x = static_cast<std::int16_t>(jo::self<jo::Widget>(this)->x + d);
        } else if (x_mid) {
            const std::int32_t d = width() - nw;
            jo::self<jo::Widget>(this)->x = static_cast<std::int16_t>(jo::self<jo::Widget>(this)->x + (d - d / 2));
            const std::int32_t d2 = width() - nw;
            jo::self<jo::Widget>(this)->x2 = static_cast<std::int16_t>(jo::self<jo::Widget>(this)->x2 - d2 / 2);
        } else if (x_high) {
            const std::int32_t d = width() - nw;
            jo::self<jo::Widget>(this)->x2 = static_cast<std::int16_t>(jo::self<jo::Widget>(this)->x2 - d);
        }
        if (y_low) {
            const std::int32_t d = height() - nh;
            jo::self<jo::Widget>(this)->y = static_cast<std::int16_t>(jo::self<jo::Widget>(this)->y + d);
        } else if (y_mid) {
            const std::int32_t d = height() - nh;
            jo::self<jo::Widget>(this)->y = static_cast<std::int16_t>(jo::self<jo::Widget>(this)->y + (d - d / 2));
            const std::int32_t d2 = height() - nh;
            jo::self<jo::Widget>(this)->y2 = static_cast<std::int16_t>(jo::self<jo::Widget>(this)->y2 - d2 / 2);
        } else if (y_high) {
            const std::int32_t d = height() - nh;
            jo::self<jo::Widget>(this)->y2 = static_cast<std::int16_t>(jo::self<jo::Widget>(this)->y2 - d);
        }
    }

    // Animation set-up (0x0061e7b1 / 0x0061eeb7).
    void start_resize(std::uint32_t arg, std::uint8_t anchor, std::int32_t nw, std::int32_t nh, float speed, float accel) {
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->resize_arg) = arg;
        jo::self<jo::Widget>(this)->resize_41 = anchor;
        jo::self<jo::Widget>(this)->target_w = static_cast<std::int16_t>(nw);
        jo::self<jo::Widget>(this)->target_h = static_cast<std::int16_t>(nh);
        const std::int32_t ady = joan::orig::arg::as<std::int32_t>(joan::orig::libc::abs(joan::orig::arg::i32(height() - nh)));  // abs
        const float fy = static_cast<float>(ady);                                      // fild ; fstp dword
        const std::int32_t adx = joan::orig::arg::as<std::int32_t>(joan::orig::libc::abs(joan::orig::arg::i32(width() - nw)));   // abs
        const double ratio = Fpu{true}.div(static_cast<double>(fy), static_cast<double>(adx));  // fild ; fdivr
        const double angle = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(ratio)));                              // atan
        float angle_f = static_cast<float>(angle);                                               // fst dword
        if (float_tie(angle)) angle_f = joan::orig::arg::as<float>(joan::orig::libc::atan(joan::orig::arg::f64(ratio)));  // exact rounding of the 64-bit st0
        jo::self<jo::Widget>(this)->resize_42 = joan::orig::arg::as<float>(joan::orig::libc::cos(joan::orig::arg::f64(angle)));                          // cos (qword of st0)
        jo::self<jo::Widget>(this)->resize_46 = joan::orig::arg::as<float>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(angle_f))));  // sin (of the stored float)
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->resize_56) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->resize_5a) = 0;
        joan::orig::as_widget(this)->set_4a(joan::orig::arg::bits(speed));  // 0x0061ea20
        joan::orig::as_widget(this)->set_4e(joan::orig::arg::bits(accel));  // 0x0061ea50
        jo::self<jo::Widget>(this)->resizing = 1;
        jo::self<jo::Widget>(this)->resize_40 = 0;
    }

    // ---------------------------------------------------------------- 0x0061e370
    void resize_to(std::uint32_t w, std::uint32_t h, float speed, float accel, std::uint32_t anchor, std::uint32_t instant,
                   std::uint32_t arg) {
        const std::int32_t an = s8(anchor);
        if (an < 0 || an > 3) return;
        c_resize.hit();
        if (s16(w) == width() && s16(h) == height()) {
            joan::orig::as_widget(this)->stop_fade();  // 0x0061f120
            return;
        }
        if (s8(instant) != 0) {
            apply_anchor(an, s16(w), s16(h));
            joan::orig::as_widget(this)->stop_fade();  // 0x0061f120
            return;
        }
        const float zero = global<float>(d::k_9cf5c4);  // 0.0
        if (!(speed >= zero)) {  // test ah,1: below or unordered
            joan::orig::as_widget(this)->stop_fade();
            return;
        }
        if (!(speed != zero) && !(accel > zero)) {  // equal ; then not above
            joan::orig::as_widget(this)->stop_fade();
            return;
        }
        start_resize(arg, static_cast<std::uint8_t>(anchor), s16(w), s16(h), speed, accel);
    }

    // ---------------------------------------------------------------- 0x0061ea80
    void scale_size(float factor, float speed, float accel, std::uint32_t anchor, std::uint32_t instant, std::uint32_t arg) {
        const std::int32_t an = s8(anchor);
        if (an < 0 || an > 6) return;
        c_scale_size.hit();
        const float zero = global<float>(d::k_9cf5c4);  // 0.0
        if (!(factor >= zero)) {
            joan::orig::as_widget(this)->stop_fade();  // 0x0061f120
            return;
        }
        std::uint32_t bits = 0;
        std::memcpy(&bits, &factor, 4);
        if (bits == 0x3f800000u) {  // integer compare with 1.0f
            joan::orig::as_widget(this)->stop_fade();
            return;
        }
        const std::int32_t nw = joan::hook::x87::ftol(Fpu{true}.mul(static_cast<double>(width()), static_cast<double>(factor)));
        const std::int32_t nh = joan::hook::x87::ftol(Fpu{true}.mul(static_cast<double>(height()), static_cast<double>(factor)));
        if (s8(instant) != 0) {
            apply_anchor(an, nw, nh);
            joan::orig::as_widget(this)->stop_fade();  // 0x0061f120
            return;
        }
        if (!(speed >= zero)) {
            joan::orig::as_widget(this)->stop_fade();
            return;
        }
        if (!(speed != zero) && !(accel > zero)) {
            joan::orig::as_widget(this)->stop_fade();
            return;
        }
        start_resize(arg, static_cast<std::uint8_t>(anchor), nw, nh, speed, accel);
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b82_11", name, addr, fn)
B11("Widget::resize_to", 0x0061e370, &H::resize_to);
B11("Widget::scale_size", 0x0061ea80, &H::scale_size);
#undef B11

}  // namespace game_b82_11
}  // namespace

// ---------------------------------------- from modules/game_b83_05.cpp (game/b83_05)
namespace {
namespace game_b83_05 {

struct H {
    // 0x0061f120. ret 0.
    void* stop_fade() {
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->resize_arg) = 0;
        jo::self<jo::Widget>(this)->resize_41 = 0;
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->resize_56) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->resize_5a) = 0;
        jo::view<std::uint16_t>(jo::self<jo::Widget>(this)->target_w) = 0xffff;
        jo::view<std::uint16_t>(jo::self<jo::Widget>(this)->target_h) = 0xffff;
        jo::self<jo::Widget>(this)->resizing = 0;
        jo::self<jo::Widget>(this)->resize_40 = 0;
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->resize_speed) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->resize_accel) = 0;
        return this;
    }

    // 0x0061ea50 / 0x0061ea20 (float stored as dword). ret 4.
    void set_4e(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->resize_accel) = v; }
    void set_4a(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::Widget>(this)->resize_speed) = v; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_05", name, addr, fn)
B("Widget::stop_fade", 0x0061f120, &H::stop_fade);
B("Widget::set_4e", 0x0061ea50, &H::set_4e);
B("Widget::set_4a", 0x0061ea20, &H::set_4a);
#undef B

}  // namespace game_b83_05
}  // namespace

// ---------------------------------------- from modules/game_b107_10.cpp (game/b107_10)
namespace {
namespace game_b107_10 {

JOAN_COUNTER(c_update_61f3b0, "Obj::update_61f3b0");

// Virtual slot 0 (thiscall, no arguments, ret 0) of a widget, as 0x0061f3d4..0x0061f3dc calls it; its eax is
// Obj::update_61f3b0's result. The original also had eax = this at the call, so an empty void override (e.g.
// 0x0045bf60) "returned" this; that value is never used: update_61f3b0 has no data reference and no thunk
// pointer, and every caller (all ours) discards the result or passes it on to a caller that discards it
// (objmenu.cpp 0x00721270 / release3, objm_strat 0x00736480, objm_army 0x00728230, i_buy 0x006d1980,
// optmenui 0x00741960 / release_61f3b0). Vtable entries are code pointers (4 bytes on x86, 8 on x64).
u32 vcall_slot0(void* self) {
    const auto* const vtable = static_cast<const std::uintptr_t*>(jo::self<jo::Widget>(self)->vtable);
    return thiscall<u32>(vtable[0], self);
}

struct H {
    // ------------------------------------------------------------------ 0x0061f3b0
    u32 update_61f3b0() {
        c_update_61f3b0.hit();
        joan::orig::as_widget(this)->slide_step();  // 0x0061f450
        joan::orig::as_rect(this)->step_61f980();  // 0x0061f980
        joan::orig::as_widget(this)->fade_step();  // 0x00620810
        return vcall_slot0(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_10", name, addr, fn)
B("Obj::update_61f3b0", 0x0061f3b0, &H::update_61f3b0);
#undef B

}  // namespace game_b107_10
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

struct H {
    // ------------------------------------------------------------------ 0x0061f400
    u32 set_time_61f400(i32 t) {
        const Fpu fpu{true};
        const double v = fpu.add(fpu.mul(fpu.div(static_cast<double>(t), static_cast<double>(global<float>(d::flt_1000))),
                                         static_cast<double>(jo::self<jo::Widget>(this)->move_accel)),
                                 static_cast<double>(jo::self<jo::Widget>(this)->move_speed));
        joan::orig::as_widget(this)->set_move_speed(joan::orig::arg::bits(static_cast<float>(v)));  // 0x0061e270
        return 0;  // callers discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Obj::set_time_61f400", 0x0061f400, &H::set_time_61f400);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
namespace {
namespace game_b104_11 {

struct H {
    float get_37() { return jo::self<jo::Widget>(this)->move_speed; }
    float get_3b() { return jo::self<jo::Widget>(this)->move_accel; }
    float get_4e() { return jo::self<jo::Widget>(this)->resize_accel; }
    float get_6c() { return jo::self<jo::Widget>(this)->scale_accel; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("Widget::get_37", 0x0061f8f0, &H::get_37);
B("Widget::get_3b", 0x0061f910, &H::get_3b);
B("Widget::get_4e", 0x006207a0, &H::get_4e);
B("Widget::get_6c", 0x00620a00, &H::get_6c);
#undef B

}  // namespace game_b104_11
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

struct H {
    // 0x0061f930
    std::uint32_t set_time_61f930(std::int32_t ms) {
        const joan::hook::x87::Fpu fpu{true};
        const double q = fpu.div(static_cast<double>(ms), global<float>(d::flt_1000));
        const float v = static_cast<float>(fpu.add(fpu.mul(q, jo::self<jo::Widget>(this)->resize_accel), jo::self<jo::Widget>(this)->resize_speed));
        joan::orig::as_widget(this)->set_4a(joan::orig::arg::bits(v));  // 0x0061ea20
        return 0;  // callers discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Obj::set_time_61f930", 0x0061f930, &H::set_time_61f930);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b106_17.cpp (game/b106_17)
namespace {
namespace game_b106_17 {

JOAN_COUNTER(c_step, "Rect::step_61f980");

void* const kRandom = obj(d::g_misc);

using joan::orig::helpers::crt_abs;

// ---- 0x0061f980 helpers ------------------------------------------------------------------
float sub_int(float a, i32 n) { return static_cast<float>(Fpu{true}.sub(a, static_cast<double>(n))); }
i32 half(i32 v) { return v / 2; }  // cdq; sub eax, edx; sar eax, 1
void addw(i16& w, i32 v) { jo::view<u16>(w) = static_cast<u16>(jo::view<u16>(w) + static_cast<u16>(v)); }
void subw(i16& w, i32 v) { jo::view<u16>(w) = static_cast<u16>(jo::view<u16>(w) - static_cast<u16>(v)); }

// One axis of 0x0061f980. d = remaining size delta, acc = the float accumulator, lo / hi =
// the two rectangle words. Case kinds per direction byte +0x41 (0..6): L moves lo, H moves
// hi, S splits between both (X: L S H L H S S, Y: L S H H L H L). Positive d: lo -= / hi +=,
// negative d: lo += / hi -=. Every case first does acc -= e (fild / fsubr / fstp dword).
void step_axis(jo::Widget* self, i32 d, float& acc, i16& lo, i16& hi, const char* kinds) {
    if (crt_abs(d) <= 0) return;
    const i32 lim = joan::orig::arg::as<i32>(joan::orig::as_misc(kRandom)->limit_int(i32{2}, i32{1}, joan::orig::arg::i32(crt_abs(d))));  // 0x00590aa0 clamp
    if (static_cast<double>(lim) > static_cast<double>(acc)) return;  // test ah,0x41; je
    const bool pos = d > 0;
    const i32 n = pos ? d : static_cast<i32>(0u - static_cast<u32>(d));  // imul -1
    const i32 e = joan::orig::arg::as<i32>(joan::orig::as_misc(kRandom)->limit_int(joan::orig::arg::i32(joan::hook::x87::ftol(acc)), i32{1}, joan::orig::arg::i32(n)));
    const i32 dir = static_cast<i32>(jo::view<std::int8_t>(self->resize_41));
    if (static_cast<u32>(dir) > 6u) return;
    acc = sub_int(acc, e);
    switch (kinds[dir]) {
    case 'L':
        if (pos) subw(lo, e); else addw(lo, e);
        break;
    case 'H':
        if (pos) addw(hi, e); else subw(hi, e);
        break;
    default: {  // 'S'
        const i32 other = (e % 2 == 0) ? half(e) : e - half(e);
        if (pos) {
            subw(lo, half(e));
            addw(hi, other);
        } else {
            addw(lo, half(e));
            subw(hi, other);
        }
        break;
    }
    }
}

struct H {
    // 0x0061f980
    void step_61f980() {
        if (jo::view<std::int8_t>(jo::self<jo::Widget>(this)->resizing) == 0) return;
        if (jo::view<std::int8_t>(jo::self<jo::Widget>(this)->resize_40) != 0) return;
        if (static_cast<i32>(jo::self<jo::Widget>(this)->target_w) == static_cast<i32>(jo::self<jo::Widget>(this)->x2) - static_cast<i32>(jo::self<jo::Widget>(this)->x) &&
            static_cast<i32>(jo::self<jo::Widget>(this)->target_h) == static_cast<i32>(jo::self<jo::Widget>(this)->y2) - static_cast<i32>(jo::self<jo::Widget>(this)->y)) {
            joan::orig::as_widget(this)->stop_fade();  // 0x0061f120
            return;
        }
        c_step.hit();
        i32 dt = global<i32>(d::g_frame_elapsed_ms);
        if (jo::self<jo::Widget>(this)->resize_arg > dt) {
            jo::self<jo::Widget>(this)->resize_arg = jo::self<jo::Widget>(this)->resize_arg - dt;
            return;
        }
        dt = dt - jo::self<jo::Widget>(this)->resize_arg;
        jo::self<jo::Widget>(this)->resize_arg = 0;
        if (dt == 0) return;
        const i32 dx = static_cast<i32>(jo::self<jo::Widget>(this)->target_w) - (static_cast<i32>(jo::self<jo::Widget>(this)->x2) - static_cast<i32>(jo::self<jo::Widget>(this)->x));
        const i32 dy = static_cast<i32>(jo::self<jo::Widget>(this)->target_h) - (static_cast<i32>(jo::self<jo::Widget>(this)->y2) - static_cast<i32>(jo::self<jo::Widget>(this)->y));
        const float rate = joan::orig::arg::as<float>(joan::orig::as_widget(this)->get_4e());  // 0x006207a0, fld dword +0x4e
        if (rate < 0.0f || rate > 0.0f) joan::orig::as_widget(this)->set_time_4a(joan::orig::arg::i32(dt));  // 0x0061f930 (fcomp 0.0, test ah,0x40)
        const Fpu fpu{true};
        const float t = static_cast<float>(
            fpu.mul(fpu.div(static_cast<double>(dt), static_cast<double>(global<float>(d::flt_1000))),  // 1000.0f
                    static_cast<double>(jo::self<jo::Widget>(this)->resize_speed)));
        jo::self<jo::Widget>(this)->resize_56 = t * jo::self<jo::Widget>(this)->resize_42 + jo::self<jo::Widget>(this)->resize_56;
        jo::self<jo::Widget>(this)->resize_5a = t * jo::self<jo::Widget>(this)->resize_46 + jo::self<jo::Widget>(this)->resize_5a;
        jo::Widget* const self = jo::self<jo::Widget>(this);
        step_axis(self, dx, self->resize_56, self->x, self->x2, "LSHLHSS");
        step_axis(self, dy, self->resize_5a, self->y, self->y2, "LSHHLHL");
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_17", name, addr, fn)
B("Rect::step_61f980", 0x0061f980, &H::step_61f980);
#undef B

}  // namespace game_b106_17
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

struct H {
    // ------------------------------------------------------------------ 0x006207c0
    u32 set_time_6207c0(i32 t) {
        const Fpu fpu{true};
        double v = fpu.div(static_cast<double>(t), static_cast<double>(global<float>(d::flt_1000)));
        v = fpu.mul(v, static_cast<double>(jo::self<jo::Widget>(this)->scale_accel));
        v = fpu.add(v, static_cast<double>(jo::self<jo::Widget>(this)->scale_speed));
        joan::orig::as_widget(this)->set_scale_speed(joan::orig::arg::bits(static_cast<float>(v)));  // 0x0061f2e0
        return 0;  // callers discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Obj::set_time_6207c0", 0x006207c0, &H::set_time_6207c0);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b90_05.cpp (game/b90_05)
namespace {
namespace game_b90_05 {

void* c_memset(void* p, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // 0x00620a20
    void* rect9_clear() {
        c_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::BmpRes)));  // 9 on x86: every caller clears a BmpRes (the 59 static BmpRes globals, World::bitmaps)
        return this;
    }
};

#define B9005(name, addr, fn) JOAN_REPLACE("game/b90_05", name, addr, fn)
B9005("Rect9::clear", 0x00620a20, &H::rect9_clear);
#undef B9005

}  // namespace game_b90_05
}  // namespace

// ---------------------------------------- from game/_unplaced/after_en3d_vgalock.cpp
// Out-of-line definitions from modules/game_ui2.cpp (declared in original/ui.hpp, registered above).
namespace {

JOAN_COUNTER(c_move_to, "Widget::move_to");
JOAN_COUNTER(c_scale_to, "Widget::scale_to");

}  // namespace

namespace joan::orig {

// 0x0061e020 Widget::move_to (+0x9 / +0xb position, +0xd / +0xf, +0x21 / +0x23 target).
// ret 0x18.
Widget* Widget::move_to(std::int16_t new_x, std::int16_t new_y, float speed, float accel, char instant, std::uint32_t arg) {
    c_move_to.hit();
    const float zero = global<float>(d::k_9cf5c4);  // 0.0
    if (static_cast<std::int32_t>(new_x) == static_cast<std::int32_t>(x) &&
        static_cast<std::int32_t>(new_y) == static_cast<std::int32_t>(y)) {
        joan::orig::as_widget(this)->stop_move();  // 0x0061e2d0
        return this;
    }
    if (static_cast<std::int32_t>(instant) != 0) {
        x2 = static_cast<std::int16_t>(static_cast<std::int32_t>(new_x) +
                                       (static_cast<std::int32_t>(x2) - static_cast<std::int32_t>(x)));
        y2 = static_cast<std::int16_t>(static_cast<std::int32_t>(new_y) +
                                       (static_cast<std::int32_t>(y2) - static_cast<std::int32_t>(y)));
        x = new_x;
        y = new_y;
        joan::orig::as_widget(this)->stop_move();
        return this;
    }
    if (!(speed >= zero)) {  // fcomp ; test ah,1 : below or unordered
        joan::orig::as_widget(this)->stop_move();
        return this;
    }
    if (!(speed != zero) && !(accel > zero)) {  // equal ; then not above
        joan::orig::as_widget(this)->stop_move();
        return this;
    }
    move_arg = arg;
    target_x = new_x;
    target_y = new_y;
    const std::int32_t dy = static_cast<std::int32_t>(target_y) - static_cast<std::int32_t>(y);
    const float ady = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(static_cast<float>(dy))));  // 0x00427d60 (CRT fabs), fstp dword
    const std::int32_t dx = static_cast<std::int32_t>(target_x) - static_cast<std::int32_t>(x);
    const double adx = joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(static_cast<float>(dx))));  // st0 kept
    const double ratio = joan::hook::x87::Fpu{true}.div(static_cast<double>(ady), adx);  // fdivr
    const double angle = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(ratio)));                           // atan
    float angle_f = static_cast<float>(angle);                                            // fst dword
    if (float_tie(angle)) angle_f = joan::orig::arg::as<float>(joan::orig::libc::atan(joan::orig::arg::f64(ratio)));  // exact rounding of the 64-bit st0
    move_cos = joan::orig::arg::as<float>(joan::orig::libc::cos(joan::orig::arg::f64(angle)));                       // cos (qword of st0)
    move_sin = joan::orig::arg::as<float>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(angle_f))));  // sin (of the stored float)
    move_25 = 0;
    move_29 = 0;
    joan::orig::as_widget(this)->set_move_speed(joan::orig::arg::bits(speed));  // 0x0061e270
    joan::orig::as_widget(this)->set_move_accel(joan::orig::arg::bits(accel));  // 0x0061e2a0
    moving = 1;
    move_36 = 0;
    return this;
}

// 0x0061f1b0 Widget::scale_to (+0x11 current, +0x60 target). ret 0x14.
Widget* Widget::scale_to(std::int16_t target, float speed, float accel, char instant, std::uint32_t arg) {
    c_scale_to.hit();
    const float zero = global<float>(d::k_9cf5c4);  // 0.0
    const double t = static_cast<double>(static_cast<std::int32_t>(target));
    const double current = scale;
    if (!(t < current) && !(t > current)) {  // fcomp ; test ah,0x40 : equal or unordered
        joan::orig::as_widget(this)->stop_scale();           // 0x0061f340
        return this;
    }
    if (static_cast<std::int32_t>(instant) != 0) {
        scale = static_cast<float>(static_cast<std::int32_t>(target));
        joan::orig::as_widget(this)->stop_scale();
        return this;
    }
    if (!(speed >= zero)) {
        joan::orig::as_widget(this)->stop_scale();
        return this;
    }
    if (!(speed != zero) && !(accel > zero)) {
        joan::orig::as_widget(this)->stop_scale();
        return this;
    }
    scale_arg = arg;
    scale_target = static_cast<float>(static_cast<std::int32_t>(target));
    scale_64 = 0;
    joan::orig::as_widget(this)->set_scale_speed(joan::orig::arg::bits(speed));  // 0x0061f2e0
    joan::orig::as_widget(this)->set_scale_accel(joan::orig::arg::bits(accel));  // 0x0061f310
    scaling = 1;
    scale_5f = 0;
    return this;
}

}  // namespace joan::orig

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_a69cf8() -> void { joan::bind::free<&::game_b96_06::init_a69cf8>(); }  // 0x61dc90
auto joan::orig::init_a69cfc() -> void { joan::bind::free<&::game_b95_06::init_a69cfc>(); }  // 0x61dcf0
auto joan::orig::init_a69d00() -> void { joan::bind::free<&::game_b100_11::init_a69d00>(); }  // 0x61dd40
auto joan::orig::div4_61dd90() -> void { joan::bind::free<&::game_b94_06::init_div4_61dd90>(); }  // 0x61dd90
auto joan::orig::Light::params(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves80a::H::light_params>(this, a0)); }  // 0x61ddc0
auto joan::orig::Gadget::ctor() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_06::H::gadget_ctor>(this)); }  // 0x61de00
auto joan::orig::Gadget::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b86_01::H::object_dtor>(this)); }  // 0x61dfb0
auto joan::orig::WidgetChild::destruct() -> void { joan::bind::member<&::en3d_b87_02::H::nop_61e000>(this); }  // 0x61e000
auto joan::orig::Widget::set_move_speed(std::uint32_t a0) -> void { joan::bind::member<&::game_ui8::H::set_move_speed>(this, a0); }  // 0x61e270
auto joan::orig::Widget::set_move_accel(std::uint32_t a0) -> void { joan::bind::member<&::game_ui8::H::set_move_accel>(this, a0); }  // 0x61e2a0
auto joan::orig::Widget::stop_move() -> void { joan::bind::member<&::game_ui8::H::stop_move>(this); }  // 0x61e2d0
auto joan::orig::Widget::resize_to(std::uint32_t a0, std::uint32_t a1, float a2, float a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6) -> void { joan::bind::member<&::game_b82_11::H::resize_to>(this, a0, a1, a2, a3, a4, a5, a6); }  // 0x61e370
auto joan::orig::Widget::set_4a(std::uint32_t a0) -> void { joan::bind::member<&::game_b83_05::H::set_4a>(this, a0); }  // 0x61ea20
auto joan::orig::Widget::set_4e(std::uint32_t a0) -> void { joan::bind::member<&::game_b83_05::H::set_4e>(this, a0); }  // 0x61ea50
auto joan::orig::Widget::scale_size(float a0, float a1, float a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> void { joan::bind::member<&::game_b82_11::H::scale_size>(this, a0, a1, a2, a3, a4, a5); }  // 0x61ea80
auto joan::orig::Widget::stop_fade() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_05::H::stop_fade>(this)); }  // 0x61f120
auto joan::orig::Widget::set_scale_speed(std::uint32_t a0) -> void { joan::bind::member<&::game_ui8::H::set_scale_speed>(this, a0); }  // 0x61f2e0
auto joan::orig::Widget::set_scale_accel(std::uint32_t a0) -> void { joan::bind::member<&::game_ui8::H::set_scale_accel>(this, a0); }  // 0x61f310
auto joan::orig::Widget::stop_scale() -> void { joan::bind::member<&::game_ui8::H::stop_scale>(this); }  // 0x61f340
auto joan::orig::Widget::update() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_10::H::update_61f3b0>(this)); }  // 0x61f3b0
auto joan::orig::Widget::set_move_time(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_07::H::set_time_61f400>(this, a0)); }  // 0x61f400
auto joan::orig::Widget::slide_step() -> void { joan::bind::member<&::game_b105_17::H::slide_step>(this); }  // 0x61f450
auto joan::orig::Widget::get_37() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b104_11::H::get_37>(this)); }  // 0x61f8f0
auto joan::orig::Widget::get_3b() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b104_11::H::get_3b>(this)); }  // 0x61f910
auto joan::orig::Widget::set_time_4a(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_06::H::set_time_61f930>(this, a0)); }  // 0x61f930
auto joan::orig::Rect::step_61f980() -> void { joan::bind::member<&::game_b106_17::H::step_61f980>(this); }  // 0x61f980
auto joan::orig::Widget::get_4e() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b104_11::H::get_4e>(this)); }  // 0x6207a0
auto joan::orig::Widget::set_scale_time(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_06::H::set_time_6207c0>(this, a0)); }  // 0x6207c0
auto joan::orig::Widget::fade_step() -> void { joan::bind::member<&::game_b105_17::H::fade_step>(this); }  // 0x620810
auto joan::orig::Widget::get_6c() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b104_11::H::get_6c>(this)); }  // 0x620a00
auto joan::orig::Rect9::clear() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_05::H::rect9_clear>(this)); }  // 0x620a20
