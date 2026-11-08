// game/unknown_camcmd.cpp: functions whose original source file is unknown (formerly game/_unplaced/camcmd.cpp;
// re/attribution.csv: between ww/camres.cpp and ww/cpuusage.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../display.hpp"
#include "../x87.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using u8 = std::uint8_t;

using i8 = std::int8_t;
using i16 = std::int16_t;


void* const kCams = obj(d::g_camera_man);

struct Vec3 {
    float x, y, z;
};
using u16 = std::uint16_t;

using joan::call::import_call;

void* const kUnitTable = obj(d::g_units);

using joan::hook::x87::Fpu;
using joan::orig::helpers::at;
}  // namespace

// ---------------------------------------- from modules/game_b88_05.cpp (game/b88_05)
namespace {
namespace game_b88_05 {

void* const kArrays = obj(d::g_misc);     // 0x00591070 remove, 0x005911a0 search, 0x00590660 random

JOAN_COUNTER(c_stance, "Unit::random_stance");

struct H {
    // ------------------------------------------------------------ 0x00429ec0 (ret 8)
    std::uint32_t random_stance(std::uint32_t, std::uint32_t) {
        c_stance.hit();
        const float r = joan::orig::arg::as<float>(joan::orig::as_misc(kArrays)->rand_unit_inclusive());  // 0x00590660
        if (r > global<float>(d::g_a3cc94)) {
            joan::orig::as_unit(this)->set_action(2, 0, 1);  // 0x00501190
            return 0;
        }
        if (r > global<float>(d::g_a3cc30)) {
            joan::orig::as_unit(this)->set_action(3, 0, 1);
            return 0;
        }
        joan::orig::as_unit(this)->set_action(1, 0, 1);
        return 0;
    }

    // ------------------------------------------------------------ 0x0042a4d0 (ret 4)
    std::uint8_t can_use(std::int32_t kind) {
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(this)->dead()) != 0) return 0;  // 0x0042d0e0
        if (kind == 2) return jo::self<jo::Unit>(this)->stats_block.values[1] != 0 ? 1 : 0;
        if (kind == 3) return jo::self<jo::Unit>(this)->stats_block.values[3] != 0 ? 1 : 0;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_05", name, addr, fn)
B("Unit::random_stance", 0x00429ec0, &H::random_stance);
B("Unit::can_use", 0x0042a4d0, &H::can_use);
#undef B

}  // namespace game_b88_05
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01 {

struct H {
    void nop_429f30() {}
    void nop_429f50() {}
    u32 one_42a1a0(void*) { return 1; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("Obj::nop_429f30", 0x00429f30, &H::nop_429f30);
B("Obj::nop_429f50", 0x00429f50, &H::nop_429f50);
B("Obj::one_42a1a0", 0x0042a1a0, &H::one_42a1a0);
#undef B

}  // namespace game_b105_01
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

using joan::orig::helpers::at;

using joan::orig::helpers::op_delete;
using joan::orig::helpers::memset_;

struct H {
    void* scalar_dtor_429fd0(u32 flags) {
        joan::orig::as_beam(this)->destruct();  // 0x0042a020
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
    // ------------------------------------------------------------------ 0x00429f70
    void* ctor_429f70() {
        joan::orig::as_billboard(this)->construct();  // 0x006351a0
        jo::self<jo::Beam>(this)->vtable = img::obj(d::vt_Obj_9cf8f4);
        memset_(&jo::self<jo::Beam>(this)->light_slot, 0, 0x84);  // light_slot .. to_time
        jo::view<u32>(jo::self<jo::Beam>(this)->light_slot) = 0xffffffffu;
        return this;
    }
    // ------------------------------------------------------------------ 0x0042a1f0
    u32 pick_42a1f0(u32 a) {
        alignas(4) u8 buf[0x1c];  // 0x0042a2b0 writes 7 floats when it returns != -1
        jo::view<u32>(jo::self<jo::Beam>(this)->light_slot) = joan::orig::arg::as<u32>(joan::orig::as_light(this)->window_colour(joan::orig::arg::mptr(static_cast<void*>(buf)), joan::orig::arg::f32(global<u32>(d::g_calendar_minute_of_day))));  // 0x0042a2b0
        if (jo::view<u32>(jo::self<jo::Beam>(this)->light_slot) != 0xffffffffu) {
            joan::orig::as_light(this)->set(joan::orig::arg::mptr(static_cast<void*>(buf)));  // 0x0042a550
            joan::orig::as_billboard(this)->place(joan::orig::arg::mptr(a));                        // 0x00635760
        }
        return jo::view<u32>(jo::self<jo::Beam>(this)->light_slot);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::ctor_429f70", 0x00429f70, &H::ctor_429f70);
B("Obj::scalar_dtor_429fd0", 0x00429fd0, &H::scalar_dtor_429fd0);
B("Obj::pick_42a1f0", 0x0042a1f0, &H::pick_42a1f0);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

struct H {
    u32 dtor_42a020() {
        jo::self<jo::Beam>(this)->vtable = img::obj(d::vt_Obj_9cf8f4);  // vtable
        return joan::orig::arg::as<u32>(joan::orig::as_billboard(this)->destruct());              // 0x00635230
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::dtor_42a020", 0x0042a020, &H::dtor_42a020);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {


JOAN_COUNTER(c_init_42a050, "Beam::init_42a050");

struct H {
    // ---------------------------------------------------------------- 0x0042a050
    u32 init_42a050(const void* a1, const void* a2, u32 a3, u32 a4, const void* a5, const void* a6, u32 a7, u32 a8, u32 a9, u32 a10,
                    const char* a11) {  // a11: the billboard texture name (Billboard::create's const char*)
        c_init_42a050.hit();
        std::memcpy(jo::self<jo::Beam>(this)->from, a1, 28);
        joan::orig::as_beam(this)->step_delta(joan::orig::arg::mptr(a1), joan::orig::arg::mptr(a2), joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::Beam>(this)->from_delta)), joan::orig::arg::f32(a4));  // 0x0042a5f0
        jo::self<jo::Beam>(this)->from_steps = a3;
        jo::self<jo::Beam>(this)->from_time = a4;
        std::memcpy(jo::self<jo::Beam>(this)->to, a5, 28);
        joan::orig::as_beam(this)->step_delta(joan::orig::arg::mptr(a5), joan::orig::arg::mptr(a6), joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::Beam>(this)->to_delta)), joan::orig::arg::f32(a8));
        jo::self<jo::Beam>(this)->to_steps = a7;
        jo::self<jo::Beam>(this)->to_time = a8;
        u32 tmp = 0;
        const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(0xffu, 0xffu, 0xffu, 0xffu));  // 0x0042a260
        // the position (from + 0x10) as Billboard::create's x, y, z float-bit parameters (three dwords on the x86
        // stack; one 12-byte struct by value would be passed by reference on x64)
        u32 pos[3];
        std::memcpy(pos, &jo::self<jo::Beam>(this)->from[4], sizeof pos);
        return thiscall<u32>(fn::Billboard_create, this, pos[0], pos[1], pos[2], a9, a10, a11, i16{0}, colour, 0u, 0x3f666666u,
                             0x3f666666u, 0x3f666666u, 3u, 0x20021u, 2u);  // 0x00635260
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Beam::init_42a050", 0x0042a050, &H::init_42a050);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01 {

struct H {
    void set_slot(std::int32_t i, std::uint32_t v) {  // 0x0042a1c0
        // relation i (1-based) of the Rel records from relation1 on (sizeof(Rel) apart)
        jo::Rel* const r = reinterpret_cast<jo::Rel*>(reinterpret_cast<std::uint8_t*>(&jo::self<jo::Nation>(this)->relation1) + static_cast<std::int32_t>(static_cast<std::uint32_t>(i - 1) * static_cast<std::uint32_t>(sizeof(jo::Rel))));
        jo::view<std::uint32_t>(r->opinion) = v;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("Obj::set_slot", 0x0042a1c0, &H::set_slot);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/en3d_helpers22.cpp (en3d/helpers22)
namespace {
namespace en3d_helpers22 {

struct H {
    H* colour_ctor(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {  // 0x0042a260
        joan::orig::as_colour(this)->construct(r, g, b, a);
        return this;
    }
};

#define HELPER22(name, addr, fn) JOAN_REPLACE("en3d/helpers22", name, addr, fn)
HELPER22("Colour::Colour", 0x0042a260, &H::colour_ctor);
#undef HELPER22

}  // namespace en3d_helpers22
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

JOAN_COUNTER(c_window, "Light::window_colour");

struct H {
    // ------------------------------------------------------------------ 0x0042a2b0
    i32 window_colour(float* out, float t) {
        c_window.hit();
        const float k1440 = global<float>(d::flt_1440);
        const float zero = global<float>(d::k_9cf5c4);
        for (i32 i = 0; i < 2; ++i) {
            // phase0 / phase1 (sizeof(DayLightPhase) apart); the object is a Beam, phase0 starts at its from (+0x3b), DayLight is a partial view
            const jo::DayLightPhase* const ph = reinterpret_cast<const jo::DayLightPhase*>(reinterpret_cast<const std::uint8_t*>(&jo::self<jo::Beam>(this)->from) + i * static_cast<i32>(sizeof(jo::DayLightPhase)));
            const float c = ph->start;
            const float e = joan::orig::arg::as<float>(joan::orig::libc::fmod(joan::orig::arg::f64(static_cast<double>(c + ph->length)), joan::orig::arg::f64(1440.0)));  // fmod
            bool hit;
            if (e >= c) {
                hit = t > c && !(t >= e);
            } else if (t > c && !(t > k1440)) {
                hit = true;
            } else if (!(t >= zero)) {
                hit = false;
            } else {
                hit = !(t >= e);
            }
            if (!hit) continue;
            float d = t - c;
            if (!(d >= zero)) d = d + k1440;
            for (i32 k = 0; k < 7; ++k) out[k] = d * ph->slope[k] + ph->base[k];
            return i;
        }
        return -1;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Light::window_colour", 0x0042a2b0, &H::window_colour);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

using joan::hook::x87::ftol;

void* const kUnits = obj(d::g_units);

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
void mount_set_42cf20(void* m, std::int32_t v) { joan::orig::as_model_instance(m)->set_10a(joan::orig::arg::u32(v)); }                          // 0x0042cf20

struct H {
    // 0x0042a550
    void light_set(const float* p) {
        const std::int32_t a3 = ftol(p[3]);
        const std::int32_t a2 = ftol(p[2]);
        const std::int32_t a1 = ftol(p[1]);
        const std::int32_t a0 = ftol(p[0]);
        std::uint32_t tmp = 0;  // ebp-0xc, written by the ctor
        const std::uint32_t* const c = joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&tmp)->construct(a0, a1, a2, a3));  // 0x0042a260
        const std::uint32_t colour = *c;
        joan::orig::as_billboard(this)->set_colors(joan::orig::arg::u32(colour));  // 0x00635de0
        std::uint32_t v[3];
        std::memcpy(v, p + 4, 12);
        joan::orig::as_billboard(this)->set_position(joan::orig::arg::u32(v[0]), joan::orig::arg::u32(v[1]), joan::orig::arg::u32(v[2]));  // 0x00635d40
    }

    // 0x0042a6c0
    void reset_42a6c0() {
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return;
        std::uint8_t* const u = unit_get(jo::self<jo::Player>(this)->unit_id_);
        jo::as_unit(u)->look_b = 0;
        jo::as_unit(u)->look_a = 0;
        jo::as_unit(u)->use_action_alt = 1;
        jo::as_unit(u)->anim_kind_alt = 1;
        if (jo::as_unit(u)->actor != nullptr) mount_set_42cf20(jo::as_unit(u)->actor, 0);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("Light::set", 0x0042a550, &H::light_set);
B("UnitRef::reset_42a6c0", 0x0042a6c0, &H::reset_42a6c0);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

JOAN_COUNTER(c_delta7, "delta7");

struct H {
    // ------------------------------------------------------------------ 0x0042a5f0
    const float* delta7(const float* a, const float* b, float* out, float d) {
        c_delta7.hit();
        for (i32 i = 0; i < 7; ++i) out[i] = (b[i] - a[i]) / d;
        return b;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("delta7", 0x0042a5f0, &H::delta7);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

void* const kGameUnits = obj(d::g_units);  // unit manager

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kGameUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_reset_42a770, "UnitCmd::reset_42a770");

struct H {
    void reset_42a770() {  // 0x0042a770
        c_reset_42a770.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return;
        std::uint8_t* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        if (jo::view<std::int8_t>(jo::self<jo::Player>(this)->riding) != 0) {
            jo::view<std::uint16_t>(jo::as_unit(unit)->look_b) = 0;
            jo::view<std::uint16_t>(jo::as_unit(unit)->look_a) = 0;
        }
        jo::as_unit(unit)->use_action_alt = 0;
        jo::self<jo::Player>(this)->riding = 0;
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("UnitCmd::reset_42a770", 0x0042a770, &H::reset_42a770);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

u32 low_word_1(u32 eax) { return (eax & 0xffff0000u) | 1u; }

struct H {
    // -------------------------------------------------------------- 0x0042a810
    u32 cmd_42a810(u32 a, u32 b, u32 c, u32 d) {
        u32 r = static_cast<u32>(jo::self<jo::IngameMenu>(obj(d::g_ingame_menu))->open);  // 0x0042d160 (char; x86: byte +0x24; g_a59a98 is an IngameMenu, SaveMenu::busy_ drifts on x64)
        r = static_cast<u32>(static_cast<i32>(static_cast<i8>(r & 0xffu)));
        if (r != 0) return low_word_1(r);
        r = joan::orig::arg::as<u32>(joan::orig::as_player(this)->switch_unit(joan::orig::arg::i32(a)));  // 0x0042b6e0 (short)
        if (static_cast<i16>(r & 0xffffu) != 0) return low_word_1(r);
        r = joan::orig::arg::as<u32>(joan::orig::as_cmd(this)->aim_unit(joan::orig::arg::i32(b)));  // 0x0042ba40 (short)
        if (static_cast<i16>(r & 0xffffu) != 0) return low_word_1(r);
        r = joan::orig::arg::as<u32>(joan::orig::as_camera(this)->move_42bcd0(joan::orig::arg::u32(c), joan::orig::arg::i32(d)));  // 0x0042bcd0
        return low_word_1(r);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Mission::cmd_42a810", 0x0042a810, &H::cmd_42a810);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b82_16.cpp (game/b82_16)
namespace {
namespace game_b82_16 {

using joan::img::str;

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

// 4-byte colour built by 0x0042a260.
struct alignas(4) Rgba {
    std::uint8_t b[4];
};

JOAN_COUNTER(c_icon, "ArcherIcon::update");
JOAN_COUNTER(c_icon_orig, "ArcherIcon::update (original)");
JOAN_COUNTER(c_icon_fix, "ArcherIcon::update (fix/uninit: phase outside 1..12, NULL icon)");

std::uint32_t make_rgba(Rgba* c, std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    return *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(c)->construct(r, g, b, a));  // 0x0042a260
}

struct H {
    // ------------------------------------------------------------------ 0x0042a8a0
    void archer_icon() {
        const std::int32_t phase = jo::view<std::int8_t>(jo::self<jo::Player>(this)->aiming);
        const bool bad_phase = static_cast<std::uint32_t>(phase - 1) > 11u;
        if (bad_phase && !fix_uninit()) {
            c_icon_orig.hit();
            thiscall(fn::ArcherIcon_update_body, this);  // the untouched original body (uninitialised icon local)
            return;
        }
        c_icon.hit();
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->unit_id_)) != 0) {  // 0x00428b70
            void* const u = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->unit_id_));  // 0x0041b3c0
            if (joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(u)->dead()) != 0) return;                           // 0x0042d0e0
        }
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_shop(obj(d::g_game_a5d380))->active()) != 0) return;  // 0x0042ced0
        std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->unit_id_)));
        if (jo::view<std::int8_t>(jo::self<jo::Player>(this)->riding) == 0) return;
        if (global<std::uint8_t>(d::g_game_running) == 0) return;
        const std::uint32_t saved = joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(obj(d::g_texture_array))->mode());  // 0x0060a720
        joan::orig::as_texture_manager(obj(d::g_texture_array))->set_mode(1u);                                         // 0x0060a740
        const std::uint32_t now = global<std::uint32_t>(d::g_now);
        const bool locked = jo::self<jo::Player>(this)->aim_lock_id != 0;
        std::uintptr_t name = 0;
        void* icon = nullptr;  // [ebp-0x10]
        switch (jo::view<std::int8_t>(jo::self<jo::Player>(this)->aiming)) {
            case 1:
            case 2:
            case 11:
            case 12: name = img::addr(locked ? img::Addr{d::s_ARCHERICONLOCKED1} : img::Addr{d::s_ARCHERICON1}); break;
            case 3:
            case 10: name = img::addr(locked ? img::Addr{d::s_ARCHERICONLOCKED2} : img::Addr{d::s_ARCHERICON2}); break;
            case 4:
            case 9: name = img::addr(locked ? img::Addr{d::s_ARCHERICONLOCKED3} : img::Addr{d::s_ARCHERICON3}); break;
            case 5:
            case 8: name = img::addr(locked ? img::Addr{d::s_ARCHERICONLOCKED4} : img::Addr{d::s_ARCHERICON4}); break;
            case 6:
            case 7: name = img::addr(locked ? img::Addr{d::s_ARCHERICONLOCKED5} : img::Addr{d::s_ARCHERICON5}); break;
            default:  // outside 1..12 (fix/uninit only; checked at entry)
                // The original's switch default (0x0042aaaf) resets the phase to 1 and loads no
                // icon: [ebp-0x10] stays uninitialised and is drawn and released below.
                // fix/uninit: NULL icon - 0x006332b0 queues nothing for NULL, the release
                // 0x0042cef0 (which dereferences it) is skipped.
                jo::view<std::int8_t>(jo::self<jo::Player>(this)->aiming) = 1;
                c_icon_fix.hit();
                break;
        }
        if (name != 0) icon = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(str(name)), 1, 0u));  // 0x006099c0
        joan::orig::as_texture_manager(obj(d::g_texture_array))->set_mode(joan::orig::arg::u32(saved));
        Rgba c18;
        joan::orig::as_colour(&c18)->construct_default();  // 0x0042ad30
        std::uint32_t colour;
        Rgba tmp;
        if (jo::as_unit(unit)->modifiers != nullptr && joan::orig::arg::as<void*>(joan::orig::as_unit_data(jo::as_unit(unit)->modifiers)->slot_e()) != nullptr &&
            jo::view<std::int16_t>(jo::as_held_item(joan::orig::arg::as<void*>(joan::orig::as_unit_data(jo::as_unit(unit)->modifiers)->slot_e()))->count) != 0) {  // 0x00497580
            if (jo::self<jo::Player>(this)->aim_lock_id != 0)
                colour = make_rgba(&tmp, 0u, 0xffu, 0u, 0xffu);
            else
                colour = make_rgba(&tmp, 0xffu, 0xffu, 0u, 0xffu);
        } else {
            colour = make_rgba(&tmp, 0xaau, 0u, 0u, 0xffu);
        }
        // The icon at the screen centre - at the HUD's scale ([Display] UiScale, not part of the original game:
        // the end corner x2 / y2 from the texture's size times the scale; -1 = the texture's size, as the original).
        std::int32_t ix = global<std::int32_t>(d::g_screen_w) / 2 - 0xf, iy = global<std::int32_t>(d::g_screen_h) / 2 - 0xf;
        std::int32_t ix2 = -1, iy2 = -1;
        const float ui = joan::display::ui_scale();
        if (ui != 1.0f && icon != nullptr) {
            const auto w = static_cast<float>(jo::view<std::int32_t>(jo::as_texture(icon)->width));
            const auto h = static_cast<float>(jo::view<std::int32_t>(jo::as_texture(icon)->height));
            ix = global<std::int32_t>(d::g_screen_w) / 2 - static_cast<std::int32_t>(std::lround(15.0f * ui));
            iy = global<std::int32_t>(d::g_screen_h) / 2 - static_cast<std::int32_t>(std::lround(15.0f * ui));
            ix2 = ix + static_cast<std::int32_t>(std::lround(w * ui)) - 1;
            iy2 = iy + static_cast<std::int32_t>(std::lround(h * ui)) - 1;
        }
        const joan::display::SelfScaled self_scaled;  // sized here: not "unscaled 2D"
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->blit_image(joan::orig::arg::i16(ix), joan::orig::arg::i16(iy), 0xau, joan::orig::arg::mptr(icon), joan::orig::arg::u32(colour), joan::orig::arg::i16(2), joan::orig::arg::i16(0), joan::orig::arg::i16(0), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(0), joan::orig::arg::i16(ix2), joan::orig::arg::i16(iy2));  // 0x006332b0
        if (icon != nullptr || !bad_phase) joan::orig::texture_ref::release(joan::orig::arg::mptr(icon));  // 0x0042cef0
        if (jo::self<jo::Player>(this)->aim_time + 100u < now) {
            jo::view<std::int8_t>(jo::self<jo::Player>(this)->aiming) = static_cast<std::int8_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->aiming) + 1);
            if (jo::view<std::int8_t>(jo::self<jo::Player>(this)->aiming) > 0xc) jo::view<std::int8_t>(jo::self<jo::Player>(this)->aiming) = 1;
            jo::self<jo::Player>(this)->aim_time = now;
        }
    }
};

#define B16(name, addr, fn) JOAN_REPLACE("game/b82_16", name, addr, fn)
B16("ArcherIcon::update", 0x0042a8a0, &H::archer_icon);
#undef B16

}  // namespace game_b82_16
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
namespace {
namespace game_leaves80b {

JOAN_COUNTER(c_cell_unload, "MapCell::unload");

struct H {
    // 0x0042ac40
    void cell_unload() {
        c_cell_unload.hit();
        if (static_cast<std::int8_t>(jo::self<jo::Cell>(this)->loaded) == 0) return;
        if (global<void*>(d::g_a502ba) == static_cast<void*>(this)) joan::orig::as_sound3d(img::obj(d::g_weather_system))->detach_channels();  // 0x0068aa10
        joan::orig::as_throw_items(img::obj(d::g_orders))->cleanup_in_cell(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x0044cdd0
        void* next = nullptr;
        for (void* p = jo::view<void*>(jo::self<jo::Cell>(this)->units_head); p != nullptr; p = next) {
            next = jo::as_unit(p)->cell_next;
            joan::orig::as_unit(p)->detach_scene();  // 0x0041fcc0
        }
        joan::orig::as_grass(img::obj(d::g_dyn_grass))->remove_patch(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x00466f40
        joan::orig::as_cell(this)->unload_files();                                                           // 0x00449780
        jo::self<jo::Cell>(this)->loaded = 0;
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("MapCell::unload", 0x0042ac40, &H::cell_unload);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

struct H {
    // ------------------------------------------------------------------ 0x0042acd0
    void join_group(const char* key) {  // key: the side name (a pointer)
        i32 group = 0;
        const i32 x = joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(key);  // 0x004b91a0 (an id, not a pointer)
        if (x != 0) group = joan::orig::as_list(obj(d::g_nations))->find_by_word2(x);  // 0x004b67e0
        if (group != 0) joan::orig::as_place(this)->set_nation(group);  // 0x0044f5c0
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::join_group", 0x0042acd0, &H::join_group);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

struct H {
    H* self() { return this; }                               // 0x0042ad30
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Obj::self", 0x0042ad30, &H::self);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b104_01.cpp (game/b104_01)
namespace {
namespace game_b104_01 {

using joan::hook::x87::Fpu;

struct Obj {};

using u16 = std::uint16_t;

void* const kItemTypes = obj(d::g_item_res);
void* const kCams = obj(d::g_camera_man);

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
using joan::orig::helpers::at;
// eax residue: only the low 16 bits reach the callee (unit_goto's a0 is passed through arg::i16)
u32 addr(const void* p) { return joan::call::register_low_bits(p); }

JOAN_COUNTER(c_update_42ad50, "Order::update_42ad50");

constexpr img::Addr kUnits = d::g_units;
void* const kOrders = obj(d::g_items);

i32 unit_dead(u32 id) { return joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_rec(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8 unit_busy(void* rec) { return joan::orig::arg::as<u8>(joan::orig::as_unit(rec)->dead()); }                 // 0x0042d0e0
void unit_anim(void* rec, u32 a, u32 b, u32 c) { joan::orig::as_unit(rec)->set_action(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c)); }  // 0x00501190
u8* item_type(u32 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItemTypes)->get(joan::orig::arg::i16(w))); }  // 0x00425ce0
i32 order_busy(u32 id) { return joan::orig::as_units(kOrders)->list.empty_at(joan::orig::arg::i32(id)); }  // 0x00425d50
u8* order_rec(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_units(kOrders)->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x00425d20
void unit_goto(void* rec, u32 a, u32 b, u32 c, u32 d) { joan::orig::as_cell_object(rec)->attach_unit(joan::orig::arg::i16(a), joan::orig::arg::i32(b), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::mptr(d)); }  // 0x0050a480
u8* inv_cur(void* list) { return joan::orig::arg::as<u8*>(joan::orig::as_unit_data(list)->slot_e()); }              // 0x00497580
i32 inv_count(void* list, u32 kind, u32 id) { return joan::orig::arg::as<i32>(joan::orig::as_modifiers(list)->sum(joan::orig::arg::i32(kind), joan::orig::arg::i32(id))); }  // 0x00497e10
u32 g_now() { return global<u32>(d::g_now); }
using joan::orig::helpers::cf;

struct H {
    // ------------------------------------------------------------ 0x0042ad50
    void update_42ad50() {
        c_update_42ad50.hit();
        if (jo::view<i8>(jo::self<jo::Player>(this)->riding) == 0) return;
        if (unit_dead(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) return;
        u8* rec = unit_rec(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        u32 pos[3];
        joan::orig::as_unit(rec)->position(joan::orig::arg::mptr(pos));  // 0x004fedb0 (result unused)
        if (jo::as_unit(rec)->hit_counter != 0 || unit_busy(rec) != 0) {
            jo::self<jo::Player>(this)->riding = 0;
            jo::as_unit(rec)->use_action_alt = 0;
            joan::orig::as_camera_man(kCams)->restore_mode();  // 0x004297d0
            jo::view<u16>(jo::as_unit(rec)->hit_counter) = 0;
            joan::orig::as_camera_zones(jo::view<void*>(jo::as_world_cell(jo::as_unit(rec)->cell)->camera_zones))->set_track_set(1u);  // 0x00427cc0
            const i16 v18 = jo::as_unit(rec)->carried_a;
            unit_goto(rec, 0, 0, 0, 0);
            if (order_busy(static_cast<u32>(static_cast<i32>(v18))) == 0) {
                joan::orig::as_units(kOrders)->remove_49f370(static_cast<i32>(v18));  // 0x0049f370
            }
            (void)joan::orig::arg::as<u32>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_unit(rec)->type_id)))));  // 0x0042d0a0
            u8* v20 = joan::orig::arg::as<u8*>(joan::orig::as_modifiers(jo::as_unit(rec)->modifiers)->slot(0));  // 0x004974f0
            if (v20 != nullptr && jo::as_unit(rec)->carried_a == 0) {
                const u32 v24 = joan::orig::arg::as<u32>(joan::orig::as_units(kOrders)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_item_stack(v20)->item)))));  // 0x0049ea90
                if (order_busy(v24) == 0) {
                    virtual_call<u32>(order_rec(v24), 0x18, 1u);
                    virtual_call<u32>(order_rec(v24), 0x24);
                    u8* t = item_type(jo::as_item_stack(v20)->item);
                    unit_goto(rec, (addr(t) & 0xffff0000u) | (v24 & 0xffffu),
                              static_cast<u32>(static_cast<i32>(jo::view<i16>(jo::as_item_def(t)->bone))), 0, 0);
                }
            }
            const i16 k = jo::as_unit(rec)->anim_kind;
            if (k == 0x30 || k == 0x31 || k == 0x32 || k == 0x33 || k == 0x35 || k == 0x36 || k == 0x37 ||
                k == 0x38 || k == 0x39) {
                joan::orig::as_unit(rec)->play_idle(0, 1);  // 0x00508b90
            }
            if (jo::as_unit(rec)->actor != nullptr) {
                joan::orig::as_model_instance(jo::as_unit(rec)->actor)->set_10a(1u);  // 0x0042cf20
            }
            return;
        }
        joan::orig::as_player(this)->aim_update();  // 0x0042cb20
        if (jo::as_unit(rec)->anim_kind == 0x30) {
            u8* p = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(rec)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::as_unit(rec)->anim_entry))));  // 0x004168d0
            jo::AnimSlot* pp = joan::orig::arg::as<jo::AnimSlot*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::as_anim_group_rec(p)->anim_id))));    // 0x004174f0
            void* const v28 = pp->anim;  // AnimSlot::anim is a pointer (read as u32 and widened through obj() before)
            void* o = joan::orig::arg::as<void*>(joan::orig::as_anim_set(v28)->frame(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(rec)->actor)->action)), 0u));  // 0x005b5dd0
            const float f2c = joan::orig::arg::as<float>(joan::orig::as_range(o)->span());  // 0x0042d130
            (void)inv_count(jo::as_unit(rec)->modifiers, 0x11, static_cast<u32>(static_cast<i32>(jo::as_unit(rec)->id)));
            const u32 dt = g_now() - jo::self<jo::Player>(this)->press_time;
            const float f4c = static_cast<float>(static_cast<double>(dt));  // fild qword (zero-extended)
            const float f50 = f2c * cf(d::flt_100);                          // * 100
            const i32 n =
                inv_count(jo::as_unit(rec)->modifiers, 0x11, static_cast<u32>(static_cast<i32>(jo::as_unit(rec)->id))) + 0x64;
            const Fpu fpu{true};
            const double q = fpu.div(f50, static_cast<double>(n));
            if (!(q > static_cast<double>(f4c))) {
                bool block = true;
                if (order_busy(static_cast<u32>(static_cast<i32>(jo::as_unit(rec)->carried_a))) == 0) {
                    const i32 a = jo::as_map_object(order_rec(static_cast<u32>(static_cast<i32>(jo::as_unit(rec)->carried_a))))->type_id;
                    const i32 b = jo::view<i16>(jo::as_item_stack(inv_cur(jo::as_unit(rec)->modifiers))->item);
                    if (a == b) block = false;
                }
                if (block) {
                    u8* cur = inv_cur(jo::as_unit(rec)->modifiers);
                    const i32 v34 = jo::view<i16>(jo::as_item_def(item_type(jo::as_item_stack(cur)->item))->bone);
                    cur = inv_cur(jo::as_unit(rec)->modifiers);
                    const u32 v38 = joan::orig::arg::as<u32>(joan::orig::as_units(kOrders)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_item_stack(cur)->item)))));  // 0x0049ea90
                    virtual_call<u32>(order_rec(v38), 0x18, 1u);
                    u8* o2 = order_rec(v38);
                    u8* t = item_type(jo::view<u16>(jo::as_map_object(o2)->type_id));
                    unit_goto(rec, (addr(t) & 0xffff0000u) | (v38 & 0xffffu),
                              static_cast<u32>(static_cast<i32>(jo::view<i16>(jo::as_item_def(t)->bone))), 0, 0);
                    std::memcpy(&jo::as_map_object(order_rec(v38))->sound_slots[4], &v34, sizeof v34);  // the i32 at +0x9e over sound_slots[4..5]
                    if (jo::as_map_object(order_rec(v38))->actor != nullptr) {
                        joan::orig::as_model_instance(jo::as_map_object(order_rec(v38))->actor)->set_10a(0u);  // 0x0042cf20
                    }
                }
                unit_anim(rec, 0x36, 0, 1);
            }
        }
        if (joan::orig::arg::as<i16>(joan::orig::as_unit(rec)->is_kind_special()) != 0 &&                             // 0x00508ab0 (ax)
            joan::orig::arg::as<i32>(joan::orig::as_unit_data(jo::as_unit(rec)->modifiers)->slot_a()) != 0 &&           // 0x00497560
            inv_cur(jo::as_unit(rec)->modifiers) != nullptr && jo::view<i16>(jo::as_item_stack(inv_cur(jo::as_unit(rec)->modifiers))->count) != 0) {
            unit_anim(rec, 0x30, 0, 1);
            jo::self<jo::Player>(this)->press_time = g_now();
            jo::self<jo::Player>(this)->order_state = 0;
        }
        if (jo::self<jo::Player>(this)->manual_aim == 0) {
            const u32 v = joan::orig::arg::as<u32>(joan::orig::as_unit(this)->nearest_target_42c260());  // 0x0042c260
            joan::orig::as_player(this)->set_target(joan::orig::arg::i32(v));             // 0x0042c7c0
            joan::orig::as_player(this)->follow_camera();                // 0x0042c830
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_01", name, addr, fn)
B("Order::update_42ad50", 0x0042ad50, &H::update_42ad50);
#undef B

}  // namespace game_b104_01
}  // namespace

// ---------------------------------------- from modules/game_b87_05.cpp (game/b87_05)
namespace {
namespace game_b87_05 {

struct Obj {};

void* const kPlayer = obj(d::g_player_unit_recno);

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
// eax residue of Player::switch_unit (0x0042b6e0): callers test only the low word (cmd_42a810) or ignore it
std::uint32_t u32(const void* p) { return joan::call::register_low_bits(p); }

static_assert(sizeof(Vec3) == 12, "vec3");

void* const kGameUnits = obj(d::g_units);  // 0x00428b70 missing / 0x0041b3c0 get
void* const kUnits = obj(d::g_items);      // scene units / markers
void* const kTypes = obj(d::g_types);      // 0x0042d0a0
void* const kAnims = obj(d::g_physic_sys);      // 0x006af310 / 0x006af370 / 0x006aee40
void* const kRecords = obj(d::g_item_res);

std::uint32_t unit_missing(std::int32_t id) { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(kGameUnits)->empty_at(joan::orig::arg::i32(id))); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_switch, "Player::switch_unit");
JOAN_COUNTER(c_set_pose, "Place::set_pose");

struct H {
    // ------------------------------------------------------------ 0x0042b6e0 (ret 4)
    std::uint32_t switch_unit(std::int32_t arg) {
        c_switch.hit();
        const std::uint32_t missing = unit_missing(jo::self<jo::Player>(this)->unit_id_);
        if (missing != 0) return missing & 0xffff0000u;
        std::uint8_t* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);  // ebp-0x8
        std::int16_t go = 0;                                      // ebp-0xc
        const std::uint32_t mode = jo::view<std::uint32_t>(jo::self<jo::Player>(this)->mode);
        switch (mode) {
            case 0: go = static_cast<std::int16_t>(arg); break;
            case 1:
                if (arg == 0) go = 1;
                break;
            case 2: break;
            case 3: go = static_cast<std::int16_t>(arg); break;
            case 4: return 0;  // eax = 4, xor ax,ax
            default: break;
        }
        if (static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->riding)) == 0 || static_cast<std::int32_t>(go) == 0)
            return u32(this) & 0xffff0000u;
        joan::orig::as_unit(unit)->set_formation_slot_5124b0(joan::orig::arg::i16(1));  // 0x005124b0
        jo::as_unit(unit)->use_action_alt = 0;
        jo::self<jo::Player>(this)->riding = 0;
        jo::self<jo::Player>(this)->aim_lock_id = 0;
        jo::as_unit(unit)->hit_counter = 0;
        if (jo::view<void*>(jo::as_world_cell(jo::as_unit(unit)->cell)->camera_zones) != nullptr) {
            joan::orig::as_camera_man(kCams)->restore_mode();                                         // 0x004297d0
            joan::orig::as_camera_zones(jo::view<void*>(jo::as_world_cell(jo::as_unit(unit)->cell)->camera_zones))->set_track_set(1u);  // 0x00427cc0
        }
        const std::int16_t slot = jo::as_unit(unit)->carried_a;  // ebp-0x10
        joan::orig::as_cell_object(unit)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));                   // 0x0050a480
        if (joan::orig::as_units(kUnits)->list.empty_at(static_cast<std::int32_t>(slot)) == 0)  // 0x00425d50
            joan::orig::as_units(kUnits)->remove_49f370(static_cast<std::int32_t>(slot));                     // 0x0049f370
        (void)joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id));  // 0x0042d0a0 (ebp-0x14, dead)
        std::int16_t* const carrier = joan::orig::arg::as<std::int16_t*>(joan::orig::as_modifiers(jo::as_unit(unit)->modifiers)->slot(0));  // 0x004974f0
        if (carrier != nullptr && static_cast<std::int32_t>(jo::as_unit(unit)->carried_a) == 0) {
            const std::uint32_t id = joan::orig::arg::as<std::uint32_t>(joan::orig::as_units(kUnits)->create_49ea90(joan::orig::arg::u32(static_cast<std::int32_t>(*carrier))));  // 0x0049ea90
            if (joan::orig::as_units(kUnits)->list.empty_at(joan::orig::arg::i32(id)) == 0) {  // 0x00425d50
                virtual_call(joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(id)), 0x18, 1);  // 0x00425d20
                virtual_call(joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(id)), 0x24);
                std::uint8_t* const rec = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(kRecords)->get(joan::orig::arg::i16(*carrier)));  // 0x00425ce0
                // arg 1 is read as a word by 0x0050a480.
                joan::orig::as_cell_object(unit)->attach_unit(joan::orig::arg::i16(static_cast<std::int32_t>(static_cast<std::int16_t>(id))), static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_item_def(rec)->bone)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
            }
        }
        if (static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->mounted)) == 0 && static_cast<std::int32_t>(jo::self<jo::Player>(this)->squad_mode) == 0)
            joan::orig::as_unit(unit)->play_idle(0, 1);  // 0x00508b90
        joan::orig::as_player(kPlayer)->set_follow(1);      // 0x00436190
        joan::orig::as_player(kPlayer)->set_follow(0);
        while (global<std::int32_t>(d::g_a48cef) == 0 && static_cast<std::int32_t>(global<std::int8_t>(d::g_a5ba5c)) != 0)
            joan::orig::as_player(kPlayer)->update_camera();  // 0x004384e0
        std::uint32_t eax = u32(unit);
        if (jo::as_unit(unit)->actor != nullptr) eax = joan::orig::arg::as<std::uint32_t>(joan::orig::as_model_instance(jo::as_unit(unit)->actor)->set_10a(1u));  // 0x0042cf20
        return (eax & 0xffff0000u) | 1u;
    }

    // ------------------------------------------------------------ 0x0042b350 (ret 0x10)
    std::uint32_t set_pose(Vec3 pos, const void* box) {
        c_set_pose.hit();
        std::memcpy(jo::self<jo::Place>(this)->box, box, 0x18);
        if (jo::view<void*>(jo::self<jo::Place>(this)->model) != nullptr) return joan::orig::arg::as<std::uint32_t>(joan::orig::as_players(kAnims)->place(jo::view<void*>(jo::self<jo::Place>(this)->model), &pos));  // 0x006aee40
        std::memcpy(jo::self<jo::Place>(this)->position_, &pos, 12);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_place(this)->update_station());  // 0x00437e20
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_05", name, addr, fn)
B("Player::switch_unit", 0x0042b6e0, &H::switch_unit);
B("Place::set_pose", 0x0042b350, &H::set_pose);
#undef B

}  // namespace game_b87_05
}  // namespace

// ---------------------------------------- from modules/game_b104_01.cpp (game/b104_01)
namespace {
namespace game_b104_01_2 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_start_42b3c0, "Order::start_42b3c0");

constexpr img::Addr kUnits = d::g_units;
void* const kOrders = obj(d::g_items);

i32 unit_dead(u32 id) { return joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_rec(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8 unit_busy(void* rec) { return joan::orig::arg::as<u8>(joan::orig::as_unit(rec)->dead()); }                 // 0x0042d0e0
u8 unit_alive(void* rec) { return joan::orig::arg::as<u8>(joan::orig::as_unit(rec)->can_act()); }                // 0x00500ba0
void unit_anim(void* rec, u32 a, u32 b, u32 c) { joan::orig::as_unit(rec)->set_action(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c)); }  // 0x00501190
i32 order_busy(u32 id) { return joan::orig::as_units(kOrders)->list.empty_at(joan::orig::arg::i32(id)); }  // 0x00425d50
void unit_goto(void* rec, u32 a, u32 b, u32 c, u32 d) { joan::orig::as_cell_object(rec)->attach_unit(joan::orig::arg::i16(a), joan::orig::arg::i32(b), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::mptr(d)); }  // 0x0050a480
u8* inv_cur(void* list) { return joan::orig::arg::as<u8*>(joan::orig::as_unit_data(list)->slot_e()); }              // 0x00497580
void* cam_entry(u32 i) { return joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCams)->motion(joan::orig::arg::i32(i))); }        // 0x00563c80
u32 g_now() { return global<u32>(d::g_now); }
using joan::orig::helpers::cf;

struct H {
    // ------------------------------------------------------------ 0x0042b3c0
    i16 start_42b3c0(u32 arg) {
        c_start_42b3c0.hit();
        if (unit_dead(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) return 0;
        u8* rec = unit_rec(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        if (jo::as_unit(rec)->actor == nullptr) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_scenario(obj(d::g_mission))->mode()) == 1) return arg != 0 ? 1 : 0;  // 0x004cd510
        if (arg == 0) return 0;
        if (unit_alive(rec) == 0) return 0;
        if (jo::as_unit(rec)->action == 1) return 0;
        if (unit_busy(rec) != 0) return 0;
        joan::orig::as_player(this)->reset_unit();  // 0x0042a6c0
        unit_anim(rec, 1, 0, 1);
        jo::view<u16>(jo::as_unit(rec)->hit_counter) = 0;
        jo::self<jo::Player>(this)->riding = 1;
        jo::self<jo::Player>(this)->aiming = 1;
        jo::self<jo::Player>(this)->aim_time = g_now();
        jo::self<jo::Player>(this)->press_time = g_now();
        joan::orig::as_camera_man(kCams)->set_mode(1);                                   // 0x00429860
        joan::orig::as_camera_zones(jo::view<void*>(jo::as_world_cell(jo::as_unit(rec)->cell)->camera_zones))->set_track_set(0u);    // 0x00427cc0
        jo::view<u16>(jo::self<jo::Player>(this)->manual_aim) = 0;
        jo::view<u32>(jo::self<jo::Player>(this)->aim_lock_id) = 0;
        jo::self<jo::Player>(this)->order_state = 0;
        jo::view<u32>(jo::self<jo::Player>(this)->aim_pitch_deg) = 0;
        jo::self<jo::Player>(this)->aim_yaw_deg = joan::orig::arg::as<float>(joan::orig::as_ground_object(rec)->heading()) / cf(d::flt_0_017453292);  // 0x004ff850
        if (!(jo::self<jo::Player>(this)->aim_yaw_deg >= cf(d::k_9cf5c4))) jo::self<jo::Player>(this)->aim_yaw_deg = jo::self<jo::Player>(this)->aim_yaw_deg + cf(d::flt_360);
        jo::self<jo::Player>(this)->aim_yaw_rad = jo::self<jo::Player>(this)->aim_yaw_deg * cf(d::flt_0_017453292);
        joan::orig::as_player(this)->aim_update();  // 0x0042cb20
        u32 cam[10];
        joan::orig::as_camera_motion(cam_entry(1))->end(joan::orig::arg::mptr(cam));  // 0x00423fe0
        cam[5] = 0;
        joan::orig::as_camera_motion(cam_entry(1))->start(joan::orig::arg::mptr(cam), joan::orig::arg::mptr(cam));  // 0x00423ec0
        if (inv_cur(jo::as_unit(rec)->modifiers) != nullptr && jo::view<i16>(jo::as_item_stack(inv_cur(jo::as_unit(rec)->modifiers))->count) != 0) {
            unit_anim(rec, 0x30, 0, 1);
        }
        const u32 v = joan::orig::arg::as<u32>(joan::orig::as_unit(this)->nearest_target_42c260());  // 0x0042c260
        joan::orig::as_player(this)->set_target(joan::orig::arg::i32(v));             // 0x0042c7c0
        if (order_busy(static_cast<u32>(static_cast<i32>(jo::as_unit(rec)->carried_a))) == 0) unit_goto(rec, 0, 0, 0, 0);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_01", name, addr, fn)
B("Order::start_42b3c0", 0x0042b3c0, &H::start_42b3c0);
#undef B

}  // namespace game_b104_01_2
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

using joan::img::str;

JOAN_COUNTER(c_remove_material, "Plane::remove_material");
JOAN_COUNTER(c_read_sectors, "World::read_sectors");

using joan::orig::helpers::stricmp_;

struct H {
    // ---------------------------------------------------------------- misc
    // 0x0042b640 Plane::remove_material(i), 1-based: 0 when out of 1..+0x64.
    std::int32_t remove_material(std::int32_t i) {
        c_remove_material.hit();
        if (i <= 0 || i > jo::self<jo::Cell>(this)->sector_count) return 0;
        void* const m = jo::self<jo::Cell>(this)->sectors[i - 1].item;
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::Cell>(this)->world->scene))->remove_group(joan::orig::arg::mptr(m));  // 0x005ea5e0
        joan::orig::as_misc(obj(d::g_misc))->del_array_rec_591070(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Cell>(this)->sectors)), joan::orig::arg::i32(jo::self<jo::Cell>(this)->sector_count), static_cast<std::int32_t>(sizeof(jo::WorldSectorPtrArray)), joan::orig::arg::i32(i));     // 0x00591070
        joan::orig::as_misc(obj(d::g_misc))->del_array_rec_591070(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Cell>(this)->sector_names)), joan::orig::arg::i32(jo::self<jo::Cell>(this)->sector_count), 0x21, joan::orig::arg::i32(i));  // 0x00591070
        jo::self<jo::Cell>(this)->sector_count = jo::self<jo::Cell>(this)->sector_count - 1;
        return 1;
    }

    // 0x0042c6b0 World::read_sectors(stream): skips 3 tokens, reads +0x4c / +0x4e (atoi), then
    // 0x00551440(stream, 1) for every "CreateSceneSector" token until the end.
    std::int32_t read_sectors(void* stream) {
        c_read_sectors.hit();
        char buf[256];
        joan::orig::as_file_txt(stream)->get_token(1);    // 0x00589c70
        joan::orig::as_file_txt(stream)->get_token(1);
        joan::orig::as_file_txt(stream)->get_token(1);
        joan::orig::as_file_txt(stream)->read_line(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(buf))), 0xff);  // 0x00589f10
        jo::view<std::uint16_t>(jo::self<jo::Cell>(this)->sector_rows) = static_cast<std::uint16_t>(joan::crt::atoi(joan::orig::arg::str(static_cast<const char*>(buf))));  // atoi
        joan::orig::as_file_txt(stream)->get_token(1);
        joan::orig::as_file_txt(stream)->read_line(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(buf))), 0xff);
        jo::view<std::uint16_t>(jo::self<jo::Cell>(this)->sector_cols) = static_cast<std::uint16_t>(joan::crt::atoi(joan::orig::arg::str(static_cast<const char*>(buf))));
        for (;;) {
            const char* const tok = joan::orig::arg::as<const char*>(joan::orig::as_file_txt(stream)->get_token(1));
            if (tok == nullptr) break;
            if (stricmp_(tok, str(d::s_CreateSceneSector)) == 0) joan::orig::as_world(this)->read_scene_sector(joan::orig::arg::mptr(stream), 1);  // 0x00551440
        }
        return 1;
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Plane::remove_material", 0x0042b640, &H::remove_material);
L82("World::read_sectors", 0x0042c6b0, &H::read_sectors);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01_2 {

using u32 = std::uint32_t;

void* const kUtil = obj(d::g_misc);

JOAN_COUNTER(c_find_42bc50, "List::find_42bc50");

struct H {
    // ---------------------------------------------------------------- 0x0042b9a0
    // x86: scene +0x6e, count +0x86, array +0x8e = World::scene / entry_count / entries; read through World (the size-less
    // CamGroupSet view put them 8 bytes short of the real World on x64). No ported caller records the owner.
    u32 remove_at_42b9a0(i32 i) {
        if (i <= 0 || i > jo::self<jo::World>(this)->entry_count) return 0;
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::World>(this)->scene))->remove_group(joan::orig::arg::mptr(jo::view<jo::PtrSlot*>(jo::self<jo::World>(this)->entries)[i - 1].p));         // 0x005ea5e0
        joan::orig::as_misc(kUtil)->del_array_rec_591070(joan::orig::arg::str(jo::view<void*>(jo::self<jo::World>(this)->entries)), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::World>(this)->entry_count)), static_cast<i32>(sizeof(jo::PtrSlot)), joan::orig::arg::i32(i));              // 0x00591070
        jo::self<jo::World>(this)->entry_count = jo::self<jo::World>(this)->entry_count - 1;
        return 1;
    }

    // ---------------------------------------------------------------- 0x0042bc50
    u32 find_42bc50(u32 key) {
        c_find_42bc50.hit();
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->empty_at_f(joan::orig::arg::i32(i))) != 0) continue;    // 0x00485760
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->get_f(joan::orig::arg::i32(i)));        // 0x00485730
            if (jo::as_missile(e)->shooter_unit_id == key && jo::as_missile(e)->expired != 0) {
                joan::orig::as_missile(e)->expire();  // 0x004e6fb0
                return jo::as_missile(e)->expired;
            }
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("List::remove_at_42b9a0", 0x0042b9a0, &H::remove_at_42b9a0);
B("List::find_42bc50", 0x0042bc50, &H::find_42bc50);
#undef B

}  // namespace game_b102_01_2
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using u32 = std::uint32_t;

struct RV3 {  // 12-byte vector copied as raw dwords
    u32 x, y, z;
};
static_assert(sizeof(RV3) == 12, "RV3");

JOAN_COUNTER(c_aim_unit, "Cmd::aim_unit");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
RV3 vsub(const void* a, const void* b) {  // 0x0060cc90 (a - b), result copied out of the temp
    RV3 tmp;
    return *joan::orig::arg::as<RV3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
}
void set_anim(void* u, i32 a) { joan::orig::as_unit(u)->set_action(joan::orig::arg::i32(a), 0, 1); }  // 0x00501190

struct H {
    // ------------------------------------------------------------------ 0x0042ba40
    i16 aim_unit(i32 enable) {
        c_aim_unit.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return 0;
        u8* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        if (enable == 0) return 0;
        if (static_cast<i32>(jo::as_unit(unit)->anim_kind) != 0x36) return 0;
        if (jo::as_unit(unit)->modifiers == nullptr) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_unit_data(jo::view<void*>(jo::as_unit(unit)->modifiers))->slot_a()) == 0) return 0;  // 0x00497560
        if (joan::orig::arg::as<i32>(joan::orig::as_unit_data(jo::view<void*>(jo::as_unit(unit)->modifiers))->slot_e()) == 0) return 0;  // 0x00497580
        u32 s[10];                                                               // ebp-0x3c
        joan::orig::as_camera_motion(joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(1)))->end(joan::orig::arg::mptr(static_cast<void*>(s)));  // 0x00563c80 / 0x00423fe0
        RV3 v;  // ebp-0x14
        if (unit_missing(jo::self<jo::Player>(this)->aim_lock_id) == 0) {
            RV3 a;  // ebp-0x48
            a.x = s[1];
            a.y = s[2];
            a.z = s[3];
            u8* const other = unit_get(jo::self<jo::Player>(this)->aim_lock_id);
            RV3 pos = global<RV3>(d::g_pos);  // ebp-0x58
            joan::orig::as_unit(other)->position(&pos);   // 0x004fedb0
            float z;
            std::memcpy(&z, &pos.z, 4);
            z = z + global<float>(d::flt_60);  // + 60.0f
            std::memcpy(&pos.z, &z, 4);
            v = vsub(&pos, &a);
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(&v));  // vector_normalize (in place)
        } else {
            u32 m[16];  // ebp-0x98
            joan::orig::orbit_matrix(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::f32(s[4]), joan::orig::arg::f32(s[5]), joan::orig::arg::f32(s[6]), joan::orig::arg::f32(0u));  // 0x0060bd00
            joan::orig::matrix_axis_z(static_cast<void*>(&v), static_cast<void*>(m));  // 0x0060c220
        }
        std::memcpy(jo::as_unit(unit)->look_dir, &v, 12);
        set_anim(unit, 0x33);
        jo::self<jo::Player>(this)->press_time = global<u32>(d::g_now);
        jo::self<jo::Player>(this)->order_state = 0;
        return 1;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Cmd::aim_unit", 0x0042ba40, &H::aim_unit);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

using u32 = std::uint32_t;

// eax residue of Camera::move_42bcd0: its only user (cmd_42a810) tests the low word
u32 bits(const void* p) { return joan::call::register_low_bits(p); }

JOAN_COUNTER(c_camera_move, "Camera::move_42bcd0");

// Camera block copied by 0x00423fe0 / 0x00423f40 (0x28 bytes, ints / floats).
struct alignas(4) CamBlock {
    jo::CamParams p;
};

struct H {
    // 0x0042bcd0
    u32 camera_move_42bcd0(u32 angle_bits, i32 mode) {
        c_camera_move.hit();
        const u32 busy = joan::orig::arg::as<u32>(joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))));  // 0x00428b70
        if (busy != 0) return busy;
        void* const unit = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        if (angle_bits == 0xbf800000u || mode == 0) {
            jo::view<u16>(jo::self<jo::Player>(this)->manual_aim) = 0;
            return bits(unit);
        }
        CamBlock cam;  // [ebp-0x30], filled by 0x00423fe0
        joan::orig::as_camera_motion(joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(1)))->end(&cam);  // 0x00423fe0(0x00563c80(1))
        float& cx = cam.p.orbit[0];  // [ebp-0x20]
        float& cy = cam.p.orbit[1];  // [ebp-0x1c]
        u32 dir = 0;                        // [ebp-0x34]
        joan::orig::as_angle(&dir)->construct_577190();              // 0x00577190
        joan::orig::as_angle(&dir)->set_degrees(joan::orig::arg::f32(angle_bits));  // 0x005773e0
        const Fpu fpu{true};
        double st = static_cast<double>(joan::orig::arg::as<float>(joan::orig::as_player(this)->look_sensitivity()));  // 0x004322d0
        st = fpu.add(st, static_cast<double>(global<float>(d::flt_1)));
        st = fpu.mul(st, global<double>(d::dbl_0_7));
        st = fpu.add(st, global<double>(d::dbl_0_2));
        const float speed = static_cast<float>(fpu.mul(st, static_cast<double>(global<float>(d::flt_0_08))));  // [ebp-0x3c]
        const float k884 = global<float>(d::flt_0_017453292);
        auto edge_scroll = [&]() {
            const float sign = global<i8>(d::g_cfg_invert_look) != 0 ? -1.0f : 1.0f;
            double t = fpu.mul(static_cast<double>(global<i32>(d::g_mouse_dy)), static_cast<double>(k884));
            t = fpu.mul(t, static_cast<double>(speed));
            t = fpu.mul(t, static_cast<double>(sign));
            cy = static_cast<float>(fpu.add(t, static_cast<double>(cy)));
            const i32 n = static_cast<i32>(0u - global<u32>(d::g_mouse_look_dx));
            t = fpu.mul(static_cast<double>(n), static_cast<double>(k884));
            t = fpu.mul(t, static_cast<double>(speed));
            cx = static_cast<float>(fpu.add(t, static_cast<double>(cx)));
        };
        auto dir_scroll = [&](img::Addr k_addr) {
            const double k = static_cast<double>(global<float>(k_addr));
            const double s = joan::orig::arg::as<double>(joan::orig::as_turns(&dir)->sin_577b10());  // 0x00577b10 (sin, raw st0)
            cy = static_cast<float>(fpu.add(fpu.mul(s, k), static_cast<double>(cy)));
            const double c = joan::orig::arg::as<double>(joan::orig::as_turns(&dir)->cos_577b50());  // 0x00577b50 (cos, raw st0)
            cx = static_cast<float>(fpu.add(fpu.mul(c, k), static_cast<double>(cx)));
        };
        if (mode == 1) {
            if (jo::self<jo::Player>(this)->mode == 3) edge_scroll(); else dir_scroll(d::flt_m0_017453292);
        } else if (mode == 2) {
            if (jo::self<jo::Player>(this)->mode == 3) edge_scroll(); else dir_scroll(d::flt_m0_034906585);
        } else if (mode == 3) {
            if (jo::self<jo::Player>(this)->mode == 3) edge_scroll(); else dir_scroll(d::flt_m0_05235988);
        }
        const float lo = global<float>(d::flt_m3_1415927);
        const float hi_ = global<float>(d::flt_3_1415927);
        const float span = global<float>(d::flt_6_2831855);
        if (!(cx >= lo)) cx = cx + span;
        if (cx > hi_) cx = cx - span;
        if (!(cy >= lo)) cy = cy + span;
        if (cy > hi_) cy = cy - span;
        if (!(cx >= global<float>(d::k_9cf5c4))) {
            const float t = global<float>(d::flt_2) * global<float>(d::g_a336ac);
            cx = t + cx;
        }
        for (;;) {
            bool adjust = false;
            {
                const float hb = jo::self<jo::Player>(this)->aim_yaw_deg * k884;
                const float upper = hb + global<float>(d::g_a336ac);
                if (!(upper >= cx)) {
                    adjust = true;
                } else {
                    const float hb2 = jo::self<jo::Player>(this)->aim_yaw_deg * k884;
                    const float lower = hb2 - global<float>(d::g_a336ac);
                    if (lower > cx) adjust = true;
                }
            }
            if (!adjust) break;
            const float h3 = jo::self<jo::Player>(this)->aim_yaw_deg * k884;
            if (!(h3 >= cx)) {
                const float t = global<float>(d::flt_2) * global<float>(d::g_a336ac);
                cx = cx - t;
            } else {
                const float h4 = jo::self<jo::Player>(this)->aim_yaw_deg * k884;
                if (h4 > cx) {
                    const float t = global<float>(d::flt_2) * global<float>(d::g_a336ac);
                    cx = t + cx;
                }
            }
        }
        const float dx = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(cx / k884 - jo::self<jo::Player>(this)->aim_yaw_deg)));  // 0x00427d60 fabs
        const float dy = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(cy / k884 - jo::self<jo::Player>(this)->aim_pitch_deg)));
        // (eax there holds an FPU status word in ax: not reproduced, see notes)
        // test ah,1 / je exit: leaves only when ordered and not less (a NaN distance moves on).
        if (jo::self<jo::Player>(this)->mode != 3 && dx >= global<float>(d::g_aim_ha)) return 0;
        if (dy >= global<float>(d::g_aim_ha)) return 0;
        joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(cx / k884));  // 0x0050abd0
        u32 cx_bits = 0;
        std::memcpy(&cx_bits, &cx, 4);
        joan::orig::as_unit(unit)->set_target(joan::orig::arg::f32(cx_bits));  // 0x004ff870
        jo::view<u16>(jo::as_unit(unit)->look_a) = 0;
        jo::view<u32>(jo::self<jo::Player>(this)->aim_yaw_rad) = cx_bits;
        joan::orig::as_camera_motion(joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(1)))->retarget(&cam);  // 0x00423f40(0x00563c80(1))
        jo::view<u32>(jo::self<jo::Player>(this)->aim_lock_id) = 0;
        jo::view<u16>(jo::self<jo::Player>(this)->manual_aim) = 1;
        return bits(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Camera::move_42bcd0", 0x0042bcd0, &H::camera_move_42bcd0);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b82_16.cpp (game/b82_16)
namespace {
namespace game_b82_16_2 {

JOAN_COUNTER(c_link, "Region::link_unit");

struct H {
    // `this` through a volatile copy (the original tests a null this).
    std::uint8_t* maybe_null_self() {
        std::uint8_t* volatile p = reinterpret_cast<std::uint8_t*>(this);
        return p;
    }

    // ------------------------------------------------------------------ 0x0042c140
    // eax: `unit` when nothing is linked (me == old, or a null this), else this.
    std::uint8_t* link_unit(std::uint8_t* unit) {
        c_link.hit();
        std::uint8_t* const me = maybe_null_self();
        std::uint8_t* const old = jo::view<std::uint8_t*>(jo::as_world_item(unit)->cell);
        if (me == old) return unit;
        if (old != nullptr) {
            std::uint8_t* const prev = jo::view<std::uint8_t*>(jo::as_world_item(unit)->cell_prev);
            std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_world_item(unit)->cell_next);
            if (prev != nullptr)
                jo::view<std::uint8_t*>(jo::as_world_item(prev)->cell_next) = next;
            else
                jo::view<std::uint8_t*>(jo::as_world_cell(old)->items_head) = next;
            if (next != nullptr)
                jo::view<std::uint8_t*>(jo::as_world_item(next)->cell_prev) = prev;
            else
                jo::view<std::uint8_t*>(jo::as_world_cell(old)->items_tail) = prev;
            jo::view<std::uint8_t*>(jo::as_world_item(unit)->cell_prev) = nullptr;
            jo::view<std::uint8_t*>(jo::as_world_item(unit)->cell_next) = nullptr;
            jo::view<std::uint8_t*>(jo::as_world_item(unit)->cell) = nullptr;
        }
        if (me != nullptr) {
            std::uint8_t* const head = jo::view<std::uint8_t*>(jo::as_world_cell(me)->items_head);
            jo::view<std::uint8_t*>(jo::as_world_cell(me)->items_head) = unit;  // (prev is the constant null)
            if (head != nullptr)
                jo::view<std::uint8_t*>(jo::as_world_item(head)->cell_prev) = unit;
            else
                jo::view<std::uint8_t*>(jo::as_world_cell(me)->items_tail) = unit;
            jo::view<std::uint8_t*>(jo::as_world_item(unit)->cell_prev) = nullptr;
            jo::view<std::uint8_t*>(jo::as_world_item(unit)->cell_next) = head;
            jo::view<std::uint8_t*>(jo::as_world_item(unit)->cell) = me;
            return me;
        }
        return unit;
    }
};

#define B16(name, addr, fn) JOAN_REPLACE("game/b82_16", name, addr, fn)
B16("Region::link_unit", 0x0042c140, &H::link_unit);
#undef B16

}  // namespace game_b82_16_2
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01_2 {

void* const kCamMan = obj(d::g_camera_man);
void* const kKinds = obj(d::g_types);

using u32 = std::uint32_t;

using joan::orig::helpers::at;

static_assert(sizeof(Vec3) == 12, "Vec3");
struct alignas(4) CamState {  // 0x00423fe0 copies 10 dwords
    jo::CamParams p;
};

void* const kUnits = obj(d::g_units);

i32 word_arg(u32 raw) { return static_cast<i32>(static_cast<i16>(raw)); }

JOAN_COUNTER(c_nearest, "Unit::nearest_target_42c260");

struct H {
    // ------------------------------------------------------------------ 0x0042c260
    i32 nearest_target_42c260() {
        c_nearest.hit();
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->unit_id_)) != 0) return 0;  // 0x00428b70
        i32 best = 0;
        float best_d = global<float>(d::g_a33758);
        Vec3 ref = global<Vec3>(d::g_pos);                              // overwritten below
        (void)joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->unit_id_));  // 0x0041b3c0 (unused)
        CamState cam1;
        joan::orig::as_camera_motion(joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCamMan)->motion(1)))->end(&cam1);  // 0x00563c80, 0x00423fe0
        float& yaw = cam1.p.orbit[0];
        const float k_rad = global<float>(d::flt_0_017453292);
        for (;;) {
            const float a = global<float>(d::g_a336ac);
            if (!(jo::self<jo::Player>(this)->aim_yaw_deg * k_rad + a >= yaw) || jo::self<jo::Player>(this)->aim_yaw_deg * k_rad - a > yaw) {
                if (!(jo::self<jo::Player>(this)->aim_yaw_deg * k_rad >= yaw)) {
                    yaw = yaw - global<float>(d::flt_2) * global<float>(d::g_a336ac);
                } else if (jo::self<jo::Player>(this)->aim_yaw_deg * k_rad > yaw) {
                    yaw = global<float>(d::flt_2) * global<float>(d::g_a336ac) + yaw;
                }
                continue;
            }
            break;
        }
        const float d48 = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(yaw / k_rad - jo::self<jo::Player>(this)->aim_yaw_deg)));                    // 0x00427d60
        const float d4c = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(cam1.p.orbit[1] / k_rad - jo::self<jo::Player>(this)->aim_pitch_deg)));  // 0x00427d60
        if (jo::self<jo::Player>(this)->mode != 3) {
            if (d48 > global<float>(d::g_aim_ha)) return best;
        }
        if (d4c > global<float>(d::g_aim_ha)) return best;

        float m98[16];
        joan::orig::orbit_matrix(joan::orig::arg::mptr(static_cast<void*>(m98)), joan::orig::arg::f32(jo::view<u32>(cam1.p.orbit[0])), joan::orig::arg::f32(jo::view<u32>(cam1.p.orbit[1])), joan::orig::arg::f32(jo::view<u32>(cam1.p.orbit[2])), joan::orig::arg::f32(0));  // 0x0060bd00
        Vec3 axis;
        joan::orig::matrix_axis_z(&axis, static_cast<void*>(m98));  // 0x0060c220
        CamState cam2;
        joan::orig::as_camera_motion(joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCamMan)->motion(1)))->end(&cam2);  // 0x00563c80, 0x00423fe0
        ref.x = cam2.p.position[0];
        ref.y = cam2.p.position[1];
        ref.z = cam2.p.position[2];
        for (i32 i = joan::orig::as_dynarray(kUnits)->size(); i > 0; --i) {  // 0x00417ad0
            u8* u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (u == nullptr || i == jo::self<jo::Player>(this)->unit_id_) continue;
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->hostile_to(joan::orig::arg::i16(word_arg(jo::view<u16[2]>(jo::self<jo::Player>(this)->unit_id_)[0])))) == 0) continue;  // 0x005094d0
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(u)->dead())) != 0) continue;       // 0x0042d0e0
            const u8* kind = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(word_arg(jo::view<u16>(jo::as_unit(u)->type_id)))));  // 0x0042d0a0
            if (jo::as_unit_type(kind)->kind == 2 || jo::as_unit_type(kind)->kind == 7) continue;
            Vec3 pos;
            joan::orig::as_unit(u)->position(&pos);  // 0x004fedb0
            const u8* kind2 = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(word_arg(jo::view<u16>(jo::as_unit(u)->type_id)))));  // 0x0042d0a0
            const Fpu fpu{true};
            pos.z = static_cast<float>(fpu.add(
                fpu.mul(static_cast<double>(jo::as_unit_type(kind2)->height), static_cast<double>(global<float>(d::flt_0_5))),
                static_cast<double>(pos.z)));
            Vec3 tmp;
            const Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &pos, &ref));  // 0x0060cc90
            const float len_sq = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, &d));
            const float dot = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, &d, &axis));
            const float p = joan::orig::arg::as<float>(joan::orig::libc::pow_f(joan::orig::arg::f32(dot), 2.0f));  // 0x00428810
            const double axis_sq = import_call<double>(d::iat_v3d_87_length_sq, &axis);
            const float proj = static_cast<float>(fpu.div(static_cast<double>(p), axis_sq));
            const float perp = joan::orig::arg::as<float>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(len_sq - proj)));  // 0x004256d0
            const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &d));
            if (!(len / global<float>(d::flt_100) + global<float>(d::g_a3373c) >= perp)) continue;
            if (!(len >= best_d)) {
                best = i;
                best_d = len;
            }
        }
        return best;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Unit::nearest_target_42c260", 0x0042c260, &H::nearest_target_42c260);
#undef B

}  // namespace game_b103_01_2
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

void* const kUnits = obj(d::g_units);

i32 unit_invalid(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------------ 0x0042c7c0
    void unitref_set_word(i32 v) {
        if (unit_invalid(jo::self<jo::Player>(this)->unit_id_) != 0) return;
        jo::self<jo::Player>(this)->aim_lock_id = v;
        u8* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        jo::view<u16>(jo::as_unit(unit)->look_a) = static_cast<u16>(v);
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("UnitRef::set_word", 0x0042c7c0, &H::unitref_set_word);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01_2 {

using u32 = std::uint32_t;

struct RV3 {  // 12-byte vector copied as raw dwords
    u32 x, y, z;
};
static_assert(sizeof(RV3) == 12, "RV3");

JOAN_COUNTER(c_update_camera, "Follow::update_camera");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------ 0x0042c830
    void update_camera() {
        c_update_camera.hit();
        if (unit_missing(jo::self<jo::Player>(this)->aim_lock_id) != 0) return;
        u8* const me = unit_get(jo::self<jo::Player>(this)->unit_id_);       // ebp-0x8
        u8* const tgt = unit_get(jo::self<jo::Player>(this)->aim_lock_id);   // ebp-0xc
        RV3 a = global<RV3>(d::g_pos);          // ebp-0x18
        RV3 b = global<RV3>(d::g_pos);          // ebp-0x24
        joan::orig::as_unit(me)->position(&a);        // 0x004fedb0
        joan::orig::as_unit(tgt)->position(&b);       // 0x004fedb0
        RV3 tmp;                                  // ebp-0x6c
        RV3 d = *joan::orig::arg::as<RV3*>(joan::orig::vec_sub(&tmp, &b, &a));  // 0x0060cc90
        const double len = import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&d));  // length (st0)
        if (len > static_cast<double>(global<float>(d::g_a33758))) {
            jo::view<u32>(jo::self<jo::Player>(this)->aim_lock_id) = 0;
            jo::view<u16>(jo::as_unit(me)->look_a) = 0;
            return;
        }
        float cam[10];  // ebp-0x4c (40-byte record, raw copy)
        joan::orig::as_camera_motion(joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(1)))->end(joan::orig::arg::mptr(static_cast<void*>(cam)));  // 0x00563c80 / 0x00423fe0
        const float k = global<float>(d::flt_0_017453292);
        for (;;) {
            const float hi = jo::self<jo::Player>(this)->aim_yaw_deg * k + global<float>(d::g_a336ac);
            if (hi >= cam[4]) {  // test ah,1 jne: less or unordered goes to the adjust step
                const float lo = jo::self<jo::Player>(this)->aim_yaw_deg * k - global<float>(d::g_a336ac);
                if (!(lo > cam[4])) break;
            }
            const float c = jo::self<jo::Player>(this)->aim_yaw_deg * k;
            if (!(c >= cam[4])) {
                cam[4] = cam[4] - global<float>(d::flt_2) * global<float>(d::g_a336ac);  // 2.0f * step
            } else {
                const float c2 = jo::self<jo::Player>(this)->aim_yaw_deg * k;
                if (c2 > cam[4]) {
                    cam[4] = global<float>(d::flt_2) * global<float>(d::g_a336ac) + cam[4];
                }
            }
        }
        const float dx = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(cam[4] / k - jo::self<jo::Player>(this)->aim_yaw_deg)));  // 0x00427d60 fabs, ebp-0x50
        const float dy = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(cam[5] / k - jo::self<jo::Player>(this)->aim_pitch_deg)));  // ebp-0x54
        const float lim = global<float>(d::g_aim_ha);
        if ((jo::self<jo::Player>(this)->mode != 3 && dx > lim) || dy > lim) {
            jo::view<u32>(jo::self<jo::Player>(this)->aim_lock_id) = 0;
            jo::view<u16>(jo::as_unit(me)->look_a) = 0;
            return;
        }
        joan::orig::as_camera_motion(joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(1)))->retarget(joan::orig::arg::mptr(static_cast<void*>(cam)));  // 0x00563c80 / 0x00423f40
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Follow::update_camera", 0x0042c830, &H::update_camera);
#undef B99

}  // namespace game_b99_01_2
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x0042ca90
    u32 on_mode(i32 mode) {
        if (mode == 0) joan::orig::as_game(this)->set_state(joan::orig::arg::u32(static_cast<u8>(0xe)));  // 0x004d7e70
        joan::orig::as_parser(obj(d::g_mouse))->drain();                                   // 0x005959c0
        global<u8>(d::g_a50099) = 1;
        joan::orig::as_game(this)->run(joan::orig::arg::i32(mode));                    // 0x00499960
        joan::orig::as_slot(obj(d::g_snd))->release();                    // 0x0059ae00
        joan::orig::as_slots(obj(d::g_mus))->release(0);            // 0x005861a0
        joan::orig::as_group(obj(d::g_names))->stop_sounds();                    // 0x004aec10
        return joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_dialogue))->release_all());        // 0x0063c800
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("Game::on_mode", 0x0042ca90, &H::on_mode);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

using u32 = std::uint32_t;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_aim_update, "Aim::update");

void* unit(i32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0

struct H {
    // 0x0042cb20
    void aim_update() {
        c_aim_update.hit();
        V3 pos = global<V3>(d::g_pos);
        void* const u = unit(jo::self<jo::Player>(this)->unit_id_);
        (void)joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading());  // 0x004ff850 (result unused)
        if (jo::as_unit(u)->actor == nullptr) return;
        joan::orig::as_unit(u)->position(&pos);  // 0x004fedb0
        u32 s[10];
        joan::orig::as_camera_motion(joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(1)))->end(joan::orig::arg::mptr(s));  // 0x00563c80 / 0x00423fe0
        V3 m1, m2, t0, t1, t2, t3;
        void* const bone = jo::view<void*>(jo::as_actor(jo::as_unit(u)->actor)->bone);
        joan::orig::matrix_axis_x(&m1, joan::orig::arg::mptr(joan::orig::arg::as<void*>(joan::orig::as_bone(bone)->matrix())));  // 0x0060c180 (0x0060ef80)
        joan::orig::matrix_row1(&m2, joan::orig::arg::mptr(joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_actor(jo::as_unit(u)->actor)->bone))->matrix())));  // 0x0060c1d0
        const V3 a = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t0, &m2, 10.0f));  // 0x0060cd00
        const V3 b = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t1, &m1, 0.0f));
        const V3 c = *joan::orig::arg::as<V3*>(joan::orig::vec_add(&t2, &pos, &b));  // 0x0060cc20
        const V3 d = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&t3, &c, &a));    // 0x0060cc90
        std::memcpy(&s[1], &d, 12);
        const float z = pos.z + global<float>(d::flt_70);
        std::memcpy(&s[3], &z, 4);
        s[4] = jo::view<u32>(jo::self<jo::Player>(this)->aim_yaw_rad);
        float v;
        std::memcpy(&v, &s[4], 4);
        s[7] = 0;
        const float k = global<float>(d::flt_0_017453292);
        for (;;) {
            const float a1 = jo::self<jo::Player>(this)->aim_yaw_deg * k + global<float>(d::g_a336ac);
            if (a1 >= v) {
                const float b1 = jo::self<jo::Player>(this)->aim_yaw_deg * k - global<float>(d::g_a336ac);
                if (!(b1 > v)) break;
            }
            const float c1 = jo::self<jo::Player>(this)->aim_yaw_deg * k;
            if (!(c1 >= v)) {
                v = v - global<float>(d::flt_2) * global<float>(d::g_a336ac);
                std::memcpy(&s[4], &v, 4);
            } else if (jo::self<jo::Player>(this)->aim_yaw_deg * k > v) {
                v = global<float>(d::flt_2) * global<float>(d::g_a336ac) + v;
                std::memcpy(&s[4], &v, 4);
            }
        }
        const float r1 = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(v / k - jo::self<jo::Player>(this)->aim_yaw_deg)));  // 0x00427d60
        float w;
        std::memcpy(&w, &s[5], 4);
        const float r2 = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(w / k - jo::self<jo::Player>(this)->aim_pitch_deg)));
        if (jo::self<jo::Player>(this)->mode != 3) {
            if (r1 > global<float>(d::g_aim_ha)) return;
            if (r2 > global<float>(d::g_aim_ha)) return;
        }
        joan::orig::as_camera_motion(joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(1)))->start(joan::orig::arg::mptr(s), joan::orig::arg::mptr(s));  // 0x00423ec0
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Aim::update", 0x0042cb20, &H::aim_update);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

struct H {
    // 0x0042ced0: called on the CutScene object (0xa5d380, the facade is Shop::active): its +4 is CutScene::active; the
    // Shop view read it through Facility, whose vtable pointer moves +4 to +8 on x64 (the dialog then never counted as active
    // and the unit labels stayed on screen).
    std::uint8_t byte_4() { return static_cast<std::uint8_t>(jo::self<jo::CutScene>(this)->active); }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("get_byte4", 0x0042ced0, &H::byte_4);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

// 0x0042cef0 TextureRef::release(ref): owner (+0x14)->0x0060a550(ref).
std::int32_t JOAN_CDECL texture_ref_release(void* ref) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_texture_manager(jo::as_texture(ref)->manager)->release(joan::orig::arg::mptr(ref))); }

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("TextureRef::release", 0x0042cef0, &texture_ref_release);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    std::uint32_t set_10a(std::uint32_t v) {  // 0x0042cf20 (eax = this)
        jo::self<jo::Actor>(this)->visible = static_cast<std::uint8_t>(v);
        return joan::call::register_low_bits(this);  // eax = this; every caller ignores the result
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("set_byte_10a", 0x0042cf20, &H::set_10a);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01_2 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- CRT initialiser wrappers: call the body directly, return its eax ----
template <fn::Code Body>
u32 JOAN_CDECL init_wrapper() {
    return cdecl_call<u32>(Body);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("init_0x0042cf50", 0x0042cf50, &init_wrapper<fn::init_pi_a336ac>);
B("init_0x0042cfb0", 0x0042cfb0, &init_wrapper<fn::init_a336b0>);
B("init_0x0042d000", 0x0042d000, &init_wrapper<fn::init_a336b4>);
B("init_0x0042d050", 0x0042d050, &init_wrapper<fn::init_42d070>);
#undef B

}  // namespace game_b105_01_2
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01_2 {

using joan::call::cdecl_call;

// ---------------------------------------------------------------------- static initialisers
void init_pi(img::Addr g) {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(g) = static_cast<float>(t * 4.0);        // qword 0x9cf788 = 4.0 (exact)
}

static void JOAN_CDECL init_pi_a336ac() { init_pi(d::g_a336ac); }  // 0x0042cf70

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("init_pi_a336ac", 0x0042cf70, &init_pi_a336ac);
#undef B98

}  // namespace game_b98_01_2
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01_2 {

void JOAN_CDECL init_a336b0() { global<float>(d::g_a336b0) = global<float>(d::g_a336ac) / global<float>(d::flt_2); }

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("init_a336b0", 0x0042cfd0, &init_a336b0);
#undef B96

}  // namespace game_b96_01_2
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01_2 {

// g_out = g_in / g_div (fld dword / fdiv dword / fstp dword).
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a336b4() { div_global(d::g_a336b4, d::g_a336ac, d::flt_3); }  // 0x0042d020

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("init_a336b4", 0x0042d020, &init_a336b4);
#undef B94

}  // namespace game_b94_01_2
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01_2 {

// ---- static initialisers / free functions ----
using joan::orig::helpers::div_init;
void JOAN_CDECL init_42d070() { div_init(d::g_a336b8, d::g_a336ac, d::flt_4); }

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("init_42d070", 0x0042d070, &init_42d070);
#undef B

}  // namespace game_b100_01_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Unit::random_stance(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b88_05::H::random_stance>(this, a0, a1)); }  // 0x429ec0
auto joan::orig::Track::destruct() -> void { joan::bind::member<&::game_b105_01::H::nop_429f30>(this); }  // 0x429f30
auto joan::orig::Beam::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_01::H::dtor_42a020>(this)); }  // 0x42a020
auto joan::orig::Ai::cond_always_1_42a1a0(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_01::H::one_42a1a0>(this, a0)); }  // 0x42a1a0
auto joan::orig::Nation::set_relation_value(std::int32_t a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b92_01::H::set_slot>(this, a0, a1); }  // 0x42a1c0
auto joan::orig::Light::window_colour(void* a0, float a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_01::H::window_colour>(this, a0, a1)); }  // 0x42a2b0
auto joan::orig::Unit::can_use(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b88_05::H::can_use>(this, a0)); }  // 0x42a4d0
auto joan::orig::Light::set(void* a0) -> void { joan::bind::member<&::game_b100_01::H::light_set>(this, a0); }  // 0x42a550
auto joan::orig::Beam::step_delta(void* a0, void* a1, void* a2, float a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_01::H::delta7>(this, a0, a1, a2, a3)); }  // 0x42a5f0
auto joan::orig::Player::reset_unit() -> void { joan::bind::member<&::game_b100_01::H::reset_42a6c0>(this); }  // 0x42a6c0
auto joan::orig::Player::reset_command() -> void { joan::bind::member<&::game_b85_06::H::reset_42a770>(this); }  // 0x42a770
auto joan::orig::Player::command(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_20::H::cmd_42a810>(this, a0, a1, a2, a3)); }  // 0x42a810
auto joan::orig::Player::archer_icon_update() -> void { joan::bind::member<&::game_b82_16::H::archer_icon>(this); }  // 0x42a8a0
auto joan::orig::Cell::unload() -> void { joan::bind::member<&::game_leaves80b::H::cell_unload>(this); }  // 0x42ac40
auto joan::orig::Place::join_group(const char* a0) -> void { joan::bind::member<&::game_b95_01::H::join_group>(this, a0); }  // 0x42acd0
auto joan::orig::Colour::construct_default() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::self>(this)); }  // 0x42ad30
auto joan::orig::Order::update_42ad50() -> void { joan::bind::member<&::game_b104_01::H::update_42ad50>(this); }  // 0x42ad50
auto joan::orig::Order::start_42b3c0(std::uint32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b104_01_2::H::start_42b3c0>(this, a0)); }  // 0x42b3c0
auto joan::orig::Plane::remove_material(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves82b::H::remove_material>(this, a0)); }  // 0x42b640
auto joan::orig::Player::switch_unit(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b87_05::H::switch_unit>(this, a0)); }  // 0x42b6e0
auto joan::orig::Cmd::aim_unit(std::int32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b97_01::H::aim_unit>(this, a0)); }  // 0x42ba40
auto joan::orig::Camera::move_42bcd0(std::uint32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_02::H::camera_move_42bcd0>(this, a0, a1)); }  // 0x42bcd0
auto joan::orig::MapRegion::link_object(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_16_2::H::link_unit>(this, a0)); }  // 0x42c140
auto joan::orig::Unit::nearest_target_42c260() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_01_2::H::nearest_target_42c260>(this)); }  // 0x42c260
auto joan::orig::World::read_sectors(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves82b::H::read_sectors>(this, a0)); }  // 0x42c6b0
auto joan::orig::Player::set_target(std::int32_t a0) -> void { joan::bind::member<&::game_b93_01::H::unitref_set_word>(this, a0); }  // 0x42c7c0
auto joan::orig::Player::follow_camera() -> void { joan::bind::member<&::game_b99_01_2::H::update_camera>(this); }  // 0x42c830
auto joan::orig::Game::on_mode(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_03::H::on_mode>(this, a0)); }  // 0x42ca90
auto joan::orig::Player::aim_update() -> void { joan::bind::member<&::game_b94_01::H::aim_update>(this); }  // 0x42cb20
auto joan::orig::Shop::active() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_world::H::byte_4>(this)); }  // 0x42ced0
auto joan::orig::texture_ref::release(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_fx::texture_ref_release>(a0)); }  // 0x42cef0
auto joan::orig::ModelInstance::set_10a(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_18::H::set_10a>(this, a0)); }  // 0x42cf20
