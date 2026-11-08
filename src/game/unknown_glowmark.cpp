// game/unknown_glowmark.cpp: functions whose original source file is unknown (formerly game/_unplaced/glowmark.cpp;
// re/attribution.csv: between ww/gfile3.cpp and ww/grassres.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/records.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

namespace {
// v3d "M" results (length, length_sq, dot_product): the replacements (src/lib/v3d/v3d87.cpp) return double. On x86
// the caller reads st0 as the float it always used (same code as before); x64 returns the double in xmm0, so it is
// read as a double there.
template <class... A>
float v3d_float(joan::img::Addr slot, A... args) {
#if UINTPTR_MAX == 0xffffffffu
    return joan::call::import_call<float>(slot, args...);
#else
    return static_cast<float>(joan::call::import_call<double>(slot, args...));
#endif
}
}  // namespace

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

using u8 = std::uint8_t;


using joan::call::cdecl_call;
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ 0x00486650 / 0x004aa2e0 / 0x004afbc0
    H* clear_55() {
        memset_(this, 0, static_cast<u32>(sizeof(jo::Marker)));  // 0x55 on x86 (billboard is a pointer)
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Block55::clear", 0x00486650, &H::clear_55);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    std::uint32_t fwd_486680() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_marker(this)->release_billboard()); }  // 0x004866b0
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("fwd_00486680", 0x00486680, &H::fwd_486680);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

struct Obj {};
// Virtual call through vtable slot `index` (thiscall).
template <class R = void, class... A>
R vcall(void* o, std::int32_t index, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(o)[index], sizeof(void*));
    return (reinterpret_cast<Obj*>(o)->*method)(args...);
}

struct H {
    // ------------------------------------------------------------------ 0x004866b0
    std::uintptr_t destroy_51() {
        if (jo::self<jo::Marker>(this)->billboard == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Marker>(this)->billboard))->release(0u);  // 0x006356c0
        void* const p = jo::view<void*>(jo::self<jo::Marker>(this)->billboard);
        if (p == nullptr) return 0;
        return vcall<std::uintptr_t>(p, 0, 1);  // scalar deleting destructor
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Obj::destroy_51", 0x004866b0, &H::destroy_51);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

using joan::call::cdecl_call;

struct V3u {  // 12-byte vector passed by value: three floats (System V: XMM registers, like the callers)
    float x, y, z;
};

using joan::orig::helpers::game_new;  // operator new

using joan::img::str;

static_assert(sizeof(V3u) == 12, "vector size");

JOAN_COUNTER(c_init_486720, "Obj::init_486720");

struct H {
    // ------------------------------------------------------------------ 0x00486720
    u32 init_486720(i32 a1, V3u v1, V3u v2, i32 a8, const char* name) {
        c_init_486720.hit();
        jo::self<jo::Marker>(this)->kind = a1;
        jo::view<V3u>(jo::self<jo::Marker>(this)->pos) = v1;
        jo::view<V3u>(jo::self<jo::Marker>(this)->pos2) = v2;
        jo::self<jo::Marker>(this)->size = a8;
        (void)std::strcmp(joan::orig::arg::str(reinterpret_cast<u8*>(this) + 0x24), joan::orig::arg::str(name));  // strcmp, result unused
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Billboard)));  // 0x37 on x86
        void* const p = mem != nullptr ? joan::orig::as_billboard(mem)->construct() : nullptr;  // 0x006351a0
        jo::view<void*>(jo::self<jo::Marker>(this)->billboard) = p;
        u32 colour_buf;
        const u32* const colour = joan::orig::arg::as<u32*>(joan::orig::as_colour(&colour_buf)->construct(0xff, 0xff, 0xff, 0xff));  // 0x0042a260
        const u32 colour_value = *colour;
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Marker>(this)->billboard))->create(joan::orig::arg::bits(v1.x), joan::orig::arg::bits(v1.y), joan::orig::arg::bits(v1.z), joan::orig::arg::u32(a8), joan::orig::arg::u32(a8), joan::orig::arg::str(name), 0, colour_value, 0, 0, 0, 0, 0xb, 1,
                 2);  // 0x00635260
        u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Marker>(this)->billboard))->plane());  // 0x00635e80
        jo::view<i32>(jo::as_plane(m)->materials->raster) = 0x100;
        joan::orig::as_plane(joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Marker>(this)->billboard))->plane()))->set_format(4);   // 0x005cdc70
        joan::orig::as_plane(joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Marker>(this)->billboard))->plane()))->set_104(1u);   // 0x005d6e80
        joan::orig::as_plane(joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Marker>(this)->billboard))->plane()))->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        const u32 now = global<u32>(d::g_now);
        jo::self<jo::Marker>(this)->fade_time = now;
        return now;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Obj::init_486720", 0x00486720, &H::init_486720);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b101_03.cpp (game/b101_03)
namespace {
namespace game_b101_03 {

using joan::hook::x87::Fpu;
using i8 = std::int8_t;

using joan::call::import_call;
using joan::img::obj;

struct FVec3 {
    float x, y, z;
};
static_assert(sizeof(FVec3) == 12, "FVec3");

void* const kWorld = obj(d::g_nations);

u32 now() { return global<u32>(d::g_now); }

bool eq_or_nan(double v) { return !(v < 0.0 || v > 0.0); }  // fcomp 0 / test ah,0x40 jnz

JOAN_COUNTER(c_update_fade, "Obj::update_fade");
JOAN_COUNTER(c_expire_486c40, "List::expire_486c40");

struct H {
    // 0x00486910 helpers: +0x45 ramps (fild of the unsigned elapsed time, fmul, fadd / fsubr).
    double step() {
        const double dt = static_cast<double>(static_cast<u32>(now() - jo::self<jo::Marker>(this)->fade_time));
        return Fpu{true}.mul(dt, static_cast<double>(global<float>(d::flt_0_0069999998)));  // 0.007
    }
    void ramp_down() {  // f45 > 0 checked by the caller
        jo::self<jo::Marker>(this)->alpha = static_cast<float>(Fpu{true}.sub(static_cast<double>(jo::self<jo::Marker>(this)->alpha), step()));
        if (!(jo::self<jo::Marker>(this)->alpha >= 0.0f)) jo::self<jo::Marker>(this)->alpha = 0.0f;  // test ah,1 (less or unordered)
    }
    void ramp_up() {
        if (!(jo::self<jo::Marker>(this)->alpha >= jo::self<jo::Marker>(this)->target_alpha)) {  // test ah,1: less or unordered
            jo::self<jo::Marker>(this)->alpha = static_cast<float>(Fpu{true}.add(step(), static_cast<double>(jo::self<jo::Marker>(this)->alpha)));
            if (jo::self<jo::Marker>(this)->alpha > jo::self<jo::Marker>(this)->target_alpha) jo::view<u32>(jo::self<jo::Marker>(this)->alpha) = jo::view<u32>(jo::self<jo::Marker>(this)->target_alpha);  // bit copy (mov)
        } else {
            jo::view<u32>(jo::self<jo::Marker>(this)->alpha) = jo::view<u32>(jo::self<jo::Marker>(this)->target_alpha);
        }
    }

    // ------------------------------------------------------------------ 0x00486910
    i32 update_fade(i8 on) {
        c_update_fade.hit();
        alignas(8) u8 cam[0x28];
        joan::orig::as_camera(global<void*>(d::g_camera))->frame(joan::orig::arg::mptr(static_cast<void*>(cam)));  // 0x006140a0
        FVec3 d;
        FVec3* const r = joan::orig::arg::as<FVec3*>(joan::orig::vec_sub(&d, static_cast<void*>(jo::self<jo::Marker>(this)->pos), static_cast<void*>(cam + 4)));  // 0x0060cc90 (out = a - b)
        FVec3 v = *r;
        const float len = v3d_float(d::iat_v3d_87_length, static_cast<void*>(&v));  // vector length
        if (len > global<float>(d::flt_2500)) {                                     // 2500
            jo::view<u32>(jo::self<jo::Marker>(this)->target_alpha) = 0;
        } else if (len > global<float>(d::flt_1000)) {  // 1000
            float x = len - global<float>(d::flt_1000);
            x = x / global<float>(d::flt_1500);  // 1500
            jo::self<jo::Marker>(this)->target_alpha = global<float>(d::flt_1) - x;  // 1.0 - x
        } else {
            jo::view<u32>(jo::self<jo::Marker>(this)->target_alpha) = 0x3f800000;
        }
        i32 result = 0;
        if (static_cast<i32>(on) != 0) {
            if (eq_or_nan(jo::self<jo::Marker>(this)->target_alpha)) {
                if (jo::self<jo::Marker>(this)->alpha > 0.0f) {
                    ramp_down();
                    jo::self<jo::Marker>(this)->lit = 0;
                }
            } else {
                if (jo::self<jo::Marker>(this)->billboard != nullptr) result = 1;
                if (jo::self<jo::Marker>(this)->billboard != nullptr &&
                    static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_marker(this)->line_of_sight(joan::orig::arg::mptr(global<jo::Camera*>(d::g_camera))))) != 0) {  // 0x00486de0
                    ramp_up();
                    jo::self<jo::Marker>(this)->lit = 1;
                } else if (jo::self<jo::Marker>(this)->alpha > 0.0f) {
                    ramp_down();
                    jo::self<jo::Marker>(this)->lit = 0;
                }
            }
        } else if (static_cast<i32>(jo::view<i8>(jo::self<jo::Marker>(this)->lit)) != 0) {
            ramp_up();
        } else if (jo::self<jo::Marker>(this)->alpha > 0.0f) {
            ramp_down();
        }
        jo::self<jo::Marker>(this)->fade_time = now();
        return result;
    }

    // ------------------------------------------------------------------ 0x00486c40
    i32 expire_486c40() {
        c_expire_486c40.hit();
        i32 i = joan::orig::arg::as<i32>(joan::orig::as_talk(this)->count());  // 0x004b8c40
        for (; i > 0; --i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_talk(this)->is_deleted(joan::orig::arg::i32(i))) != 0) continue;  // 0x004e4110
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_talk(this)->get_msg(joan::orig::arg::i32(i)));       // 0x004e3fe0
            u8* const w = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kWorld)->ptr_at(static_cast<i32>(jo::as_treaty(e)->from)));  // 0x0047b600
            if (static_cast<i32>(jo::view<i8>(jo::as_nation(w)->state)) == 3 && static_cast<i32>(jo::as_treaty(e)->reply) == 1 &&
                global<i32>(d::g_get_a47bb0) > static_cast<i32>(jo::as_treaty(e)->time + 0x3cu)) {
                jo::view<u8>(jo::as_treaty(e)->reply) = 3;
            }
            if (global<i32>(d::g_get_a47bb0) > static_cast<i32>(jo::as_treaty(e)->time + 0x16du)) {
                joan::orig::as_talk(this)->remove_message(joan::orig::arg::i32(i));  // 0x0046ad90
            }
        }
        return i;  // eax at the exit: the last index stored (the count when <= 0)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_03", name, addr, fn)
B("Obj::update_fade", 0x00486910, &H::update_fade);
B("List::expire_486c40", 0x00486c40, &H::expire_486c40);
#undef B

}  // namespace game_b101_03
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx2)
namespace {
namespace game_fx {

// x87 comparisons (fcomp + fnstsw): C0 = below or unordered, C3 = equal or unordered.
bool below_or_nan(float a, float b) { return !(a >= b); }

constexpr fn::Code kPlaneDraw = fn::Plane_render;   // 0x005d0570
constexpr fn::Code kPlaneOf = fn::Billboard_plane;     // 0x00635e80
constexpr fn::Code kBillboardPlace = fn::Billboard_place;  // 0x00635760

struct H {
    // 0x00486d10 Marker::draw(ctx): fade +0x45 (skipped below 0), billboard +0x51.
    void marker_draw(void* ctx) {
        if (below_or_nan(jo::self<jo::Marker>(this)->alpha, 0.0f)) return;
        const std::int32_t alpha = joan::hook::x87::ftol(jo::self<jo::Marker>(this)->alpha * 255.0f);
        std::uint32_t colour_obj = 0;
        std::uint32_t packed = 0;
        void* const c = joan::orig::as_colour(&colour_obj)->construct(0xffu, 0xffu, 0xffu, static_cast<std::uint32_t>(alpha & 0xff));  // 0x0042a260
        joan::orig::as_colour(&packed)->from_rgba(joan::orig::arg::mptr(c));                                  // 0x005e95b0
        void* const billboard = jo::view<void*>(jo::self<jo::Marker>(this)->billboard);
        joan::orig::as_plane(thiscall<void*>(kPlaneOf, billboard))->set_fade(joan::orig::arg::u32(packed));  // 0x005d5e80 Plane::set_fade
        thiscall(kBillboardPlace, billboard, ctx);
        // [Fixes] GlowDepth: the glow billboards use blend mode 11 (additive, ZENABLE off, set by
        // 0x00486720), so lantern glows show through walls, logs and posts. The fix keeps every
        // state but turns the depth test on (no depth write) for this draw.
        const bool fix = joan::hook::settings().fix_glow_depth;
        if (fix) joan::render::overrides().depth_test = true;
        thiscall(kPlaneDraw, thiscall<void*>(kPlaneOf, billboard), ctx, 2, 0);
        if (fix) joan::render::overrides().depth_test = false;
    }
};

#define FX2(name, addr, fn) JOAN_REPLACE("game/fx2", name, addr, fn)
FX2("Marker::draw", 0x00486d10, &H::marker_draw);
#undef FX2

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

using i32 = std::int32_t;

struct H {
    // ------------------------------------------------------------------ 0x00487600
    void* list4_ctor() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0, 0);  // 0x005856a0 (pointers: 4 on x86)
        jo::self<jo::List4>(this)->u32_56 = 1;
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("List4::List4", 0x00487600, &H::list4_ctor);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
namespace {
namespace game_b99_02 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

struct V3u {  // 12-byte vector passed by value: three floats (System V: XMM registers, like the callers)
    float x, y, z;
};

using joan::orig::helpers::game_new;  // operator new

static_assert(sizeof(V3u) == 12, "vector size");

JOAN_COUNTER(c_add_new, "List::add_new_4877e0");

struct H {
    // ------------------------------------------------------------------ 0x00487640
    u32 dtor_487640() {
        joan::orig::as_markers(this)->delete_all();  // 0x004876d0
        return thiscall<u32>(fn::dtor_DynArrayB, this);  // 0x00585710
    }

    // ------------------------------------------------------------------ 0x004877e0
    u32 add_new(i32 a1, V3u v1, V3u v2, i32 a8, const char* a9) {
        c_add_new.hit();
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Marker)));  // 0x55 on x86
        void* item = nullptr;
        if (mem != nullptr) item = joan::orig::as_marker(mem)->construct();  // 0x00486650
        joan::orig::as_marker(item)->init(joan::orig::arg::i32(a1), v1, v2, joan::orig::arg::i32(a8), joan::orig::arg::str(a9));  // 0x00486720
        joan::orig::dynarray_b(this)->linkin(&item);  // 0x005857a0
        const u32 id = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
        jo::self<jo::Marker>(item)->index = id;
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("List::~List_487640", 0x00487640, &H::dtor_487640);
B99("List::add_new_4877e0", 0x004877e0, &H::add_new);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b105_04.cpp (game/b105_04)
namespace {
namespace game_b105_04 {

struct H {
    // ---------------------------------------------------------------- empty stubs
    void stub0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_04", name, addr, fn)
B("Obj::stub_4876b0", 0x004876b0, &H::stub0);
#undef B

}  // namespace game_b105_04
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

using i32 = std::int32_t;

using joan::call::thiscall;

using joan::call::thiscall;

JOAN_COUNTER(c_list_487790, "List487790::delete_all");

std::int32_t list_count(void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0
void list_remove(void* list, std::int32_t i) { joan::orig::dynarray_b(list)->linkout(joan::orig::arg::i32(i)); }  // 0x005858c0

struct H {
    // 0x004876d0 List::delete_all (entries deleted with 0x00487790), then cleared.
    void list_487790_delete_all() {
        c_list_487790.hit();
        for (std::int32_t i = list_count(this); i >= 1; --i) {
            if (joan::orig::as_dynarray(this)->empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00487bb0
            void* const e = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));           // 0x00487b80
            if (e != nullptr) joan::orig::as_marker(e)->destroy(1u);          // 0x00487790 delete
            list_remove(this, i);
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("List487790::delete_all", 0x004876d0, &H::list_487790_delete_all);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

using joan::call::thiscall;

using joan::orig::helpers::game_delete;                         // operator delete

struct H {
    void* sdd_487790(std::uint32_t flags) {
        joan::orig::as_marker(this)->destruct();  // 0x00486680
        if (flags & 1) game_delete(this);
        return this;
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("sdd_00487790", 0x00487790, &H::sdd_487790);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x004878f0
    u32 table_remove(u32 k) {
        void* const p = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(k));  // 0x00487b80
        if (p != nullptr) joan::orig::as_marker(p)->destroy(1u);     // 0x00487790
        return thiscall<u32>(fn::linkout_5858c0, this, k);             // 0x005858c0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Table::remove", 0x004878f0, &H::table_remove);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
namespace {
namespace game_b102_02 {

using i32 = std::int32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_slots, "Slots::advance");

// ---------------------------------------------------------------- shared game helpers
i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    // 0x00487960
    i32 slots_advance() {
        c_slots.hit();
        const i32 c0 = list_count(this);
        if (c0 <= 0) return c0;
        if (jo::self<jo::Slots>(this)->i32_56 > list_count(this)) jo::self<jo::Slots>(this)->i32_56 = list_count(this);
        else if (jo::self<jo::Slots>(this)->i32_56 == 0) jo::self<jo::Slots>(this)->i32_56 = 1;
        for (i32 n = list_count(this); n > 0; --n) {
            if (joan::orig::as_dynarray(this)->empty_at(joan::orig::arg::i32(n)) != 0) continue;  // 0x00487bb0
            void* const s = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(n));  // 0x00487b80
            joan::orig::as_marker(s)->update_fade(static_cast<std::int8_t>(joan::orig::arg::i32(0)));                         // 0x00486910
        }
        i32 idx = jo::self<jo::Slots>(this)->i32_56;
        for (;;) {
            if (joan::orig::as_dynarray(this)->empty_at(joan::orig::arg::i32(idx)) == 0) {
                void* const s = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(idx));
                if (joan::orig::arg::as<i32>(joan::orig::as_marker(s)->update_fade(static_cast<std::int8_t>(joan::orig::arg::i32(1)))) == 1) break;
            }
            ++idx;
            if (idx > list_count(this)) idx = 1;
            if (idx == jo::self<jo::Slots>(this)->i32_56) break;
        }
        jo::self<jo::Slots>(this)->i32_56 = idx;
        jo::self<jo::Slots>(this)->i32_56 = jo::self<jo::Slots>(this)->i32_56 + 1;
        const i32 c = list_count(this);
        if (jo::self<jo::Slots>(this)->i32_56 > c) jo::self<jo::Slots>(this)->i32_56 = 1;
        return c;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("Slots::advance", 0x00487960, &H::slots_advance);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx_2 {

using joan::call::thiscall;

// Thunks.
constexpr fn::Code kListCount = fn::List_count_417ad0;   // 0x00417ad0
constexpr fn::Code kListGet = fn::List_get_41b3f0;     // 0x0041b3f0

std::int32_t list_count(const void* list) { return thiscall<std::int32_t>(kListCount, list); }
void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

JOAN_COUNTER(c_markers, "Markers::draw");

struct H {
    void* list_get_wrap(std::int32_t i) { return list_get(this, i); }                      // 0x0041b3c0 / 0x0041bd60 / 0x00487b80
    std::int32_t list_empty_at(std::int32_t i) { return list_get(this, i) == nullptr ? 1 : 0; }  // 0x0041bd90 / 0x00428b70 / 0x00487bb0

    // 0x00487b00 Markers::draw(ctx) (list at 0xa3ce90). Returns the final loop index like the
    // original (count if it was <= 0, else 0).
    std::int32_t markers_draw(void* ctx) {
        c_markers.hit();
        std::int32_t i = list_count(this);
        for (; i > 0; --i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->empty_at_c(joan::orig::arg::i32(i))) != 0) continue;  // 0x00487bb0
            void* const marker = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_d(joan::orig::arg::i32(i)));         // 0x00487b80
            joan::orig::as_marker(marker)->draw(joan::orig::arg::mptr(ctx));                                // 0x00486d10
        }
        return i;
    }
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
#define FX2(name, addr, fn) JOAN_REPLACE("game/fx2", name, addr, fn)
FX1("List::get_d", 0x00487b80, &H::list_get_wrap);
FX1("List::empty_at_c", 0x00487bb0, &H::list_empty_at);
FX2("Markers::draw", 0x00487b00, &H::markers_draw);
#undef FX1
#undef FX2

}  // namespace game_fx_2
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx2)
//
//  * module game/fx2: Marker::line_of_sight 0x00486de0 (the [Fixes] GlowDepth switch over the
//    original behaviour in game_b115_04.cpp).
namespace joan::game_b115_04 {
char marker_line_of_sight(void* marker, void* camera);  // 0x00486de0, modules/game_b115_04.cpp
}

namespace {
namespace game_fx_3 {

struct H {
    // 0x00486de0 Marker::line_of_sight(camera) (ret 4, al): 0 outside the view (0x00615840 with
    // radius 0) or farther than 2500, otherwise a ray from the camera to the light against the
    // scene; the update fades the glow out while this is 0. With [Fixes] GlowDepth the glow is
    // depth tested per pixel, so the ray test is skipped: a thin pillar in front of the light then
    // hides only the part of the glow behind it instead of the whole glow. The caller (0x00486910)
    // only asks while the distance fade +0x49 is non-zero, i.e. within the same 2500 limit.
    char marker_line_of_sight(void* camera) {
        if (!joan::hook::settings().fix_glow_depth)  // the original behaviour (exact C++, game_b115_04.cpp)
            return joan::game_b115_04::marker_line_of_sight(this, camera);
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(camera)->sphere_visible(joan::orig::arg::mptr(jo::self<jo::Marker>(this)->pos), 0.0f)) == 0) return 0;  // 0x00615840
        return 1;
    }
};

#define FX2(name, addr, fn) JOAN_REPLACE("game/fx2", name, addr, fn)
FX2("Marker::line_of_sight", 0x00486de0, &H::marker_line_of_sight);
#undef FX2

}  // namespace game_fx_3
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Marker::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_02::H::clear_55>(this)); }  // 0x486650
auto joan::orig::Marker::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_18::H::fwd_486680>(this)); }  // 0x486680
auto joan::orig::Marker::release_billboard() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b83_07::H::destroy_51>(this)); }  // 0x4866b0
auto joan::orig::Marker::update_fade(std::int8_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_03::H::update_fade>(this, a0)); }  // 0x486910
auto joan::orig::List::expire_486c40() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_03::H::expire_486c40>(this)); }  // 0x486c40
auto joan::orig::Marker::draw(void* a0) -> void { joan::bind::member<&::game_fx::H::marker_draw>(this, a0); }  // 0x486d10
auto joan::orig::Marker::line_of_sight(void* a0) -> char { return joan::bind::conv<char>(joan::bind::member<&::game_fx_3::H::marker_line_of_sight>(this, a0)); }  // 0x486de0
auto joan::orig::Markers::delete_all() -> void { joan::bind::member<&::game_leaves81::H::list_487790_delete_all>(this); }  // 0x4876d0
auto joan::orig::Marker::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b::H::sdd_487790>(this, a0)); }  // 0x487790
auto joan::orig::Table::remove(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_04::H::table_remove>(this, a0)); }  // 0x4878f0
auto joan::orig::Markers::draw(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_fx_2::H::markers_draw>(this, a0)); }  // 0x487b00
auto joan::orig::List::get_d(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx_2::H::list_get_wrap>(this, a0)); }  // 0x487b80
auto joan::orig::List::empty_at_c(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_fx_2::H::list_empty_at>(this, a0)); }  // 0x487bb0
