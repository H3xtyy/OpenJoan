// game/unknown_triggers.cpp: functions whose original source file is unknown (formerly game/_unplaced/triggers.cpp;
// re/attribution.csv: between ww/trackres.cpp and ww/unit.cpp). They move to their original file when evidence names it
// (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/dynarray.hpp"
#include "../x87.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using i32 = std::int32_t;

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

struct H {
    // ------------------------------------------------------------------------ 0x004eed50
    void* ctor_4eed50() {
        joan::orig::dynarray(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0);  // 0x005843d0 (pointers: 4 on x86)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Obj::ctor_4eed50", 0x004eed50, &H::ctor_4eed50);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

using joan::call::thiscall;

struct H {
    u32 dtor_4eed80() {  // 0x004eed80 (SEH state only guards the first call)
        joan::orig::as_list(this)->delete_all_eee10();              // 0x004eee10
        joan::orig::as_dynarray(this)->destruct_5844b0();        // 0x005844b0
        return 0;  // only caller: the atexit stub 0x00569dc0 (result discarded); ~DynArray returns nothing
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("X::dtor_4eed80", 0x004eed80, &H::dtor_4eed80);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::nop_4eedf0", 0x004eedf0, &H::nop);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
namespace {
namespace game_b84_03 {

using joan::call::thiscall;

using joan::call::thiscall;

JOAN_COUNTER(c_delete_all, "List::delete_all");

struct H {
    // ------------------------------------------------------------------ 0x004eee10
    std::uint32_t delete_all_eee10() {
        c_delete_all.hit();
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i >= 1; --i) {  // 0x00417ad0
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_485f20(joan::orig::arg::u32(i)));                      // 0x00485f20
            if (p != nullptr) joan::orig::as_trigger(p)->destroy(1u);                       // 0x004eeeb0
            joan::orig::dynarray(this)->linkout(joan::orig::arg::i32(i));                       // 0x00584a10
        }
        joan::orig::as_dynarray(this)->zap_584580(1);  // 0x00584580
        return 0;  // callers (0x00471010, 0x00485f50, 0x004eed80 and their replacements) discard the result; zap returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("List::delete_all_eee10", 0x004eee10, &H::delete_all_eee10);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::call::thiscall;

using joan::orig::helpers::game_delete;                      // operator delete
using joan::orig::helpers::memset_;

using joan::orig::helpers::scalar_delete;

struct H {
    void* sdd_4eeeb0(std::uint32_t flags) { joan::orig::as_wrap(this)->f_4effd0(); scalar_delete(this, flags); return this; }  // 0x004effd0

    void* zero_43() { memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::Trigger))); return this; }  // 0x004eff60 (0x43 on x86)
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj4eeeb0::sdd", 0x004eeeb0, &H::sdd_4eeeb0);
B85("Obj4eff60::ctor", 0x004eff60, &H::zero_43);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b84_03_2.cpp (game/b84_03)
namespace {
namespace game_b84_03_2 {

using joan::call::thiscall;

// 12-byte vector passed by value: float members like the callers' (System V passes them in XMM registers).
struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

JOAN_COUNTER(c_list_add, "List43::add");

using joan::orig::helpers::game_new;  // operator new

struct H {
    // ------------------------------------------------------------------ 0x004eef00
    std::int32_t list_add(std::uint32_t a1, std::uint32_t a2, std::int16_t w3, std::uint32_t a4, std::int16_t w5,
                          std::int8_t b6, V3 v, std::uint32_t a10, std::int8_t b11, std::uint32_t a12,
                          std::uint32_t a13, std::int8_t b14, void* a15, std::int16_t w16) {
        c_list_add.hit();
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Trigger)));  // 0x43 on x86
        void* item = mem != nullptr ? joan::orig::as_trigger(mem)->construct() : nullptr;  // 0x004eff60
        joan::orig::as_save_entry(item)->set(joan::orig::arg::i32(a1), a2, joan::orig::arg::u16(w3), a4, joan::orig::arg::u16(w5), static_cast<std::uint8_t>(b6), joan::orig::arg::bits(v.x), joan::orig::arg::bits(v.y), joan::orig::arg::bits(v.z), a10,

                                             static_cast<std::uint8_t>(b11), a12, a13, static_cast<std::uint8_t>(b14), a15, joan::orig::arg::u16(w16));  // 0x0047e200
        joan::orig::dynarray(this)->linkin(static_cast<void*>(&item));  // 0x00584620
        return joan::orig::as_dynarray(this)->cursor();         // 0x00417520
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("List43::add", 0x004eef00, &H::list_add);
#undef B

}  // namespace game_b84_03_2
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x004ef020
    u32 remove_4ef020(u32 key) {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_485f20(joan::orig::arg::u32(key)));  // 0x00485f20
        if (p != nullptr) joan::orig::as_trigger(p)->destroy(1u);    // 0x004eeeb0 scalar deleting dtor
        joan::orig::as_dynarray(this)->linkout_584a10(joan::orig::arg::i32(key));             // 0x00584a10
        return 0;  // callers (Triggers::update 0x004ef090 and its replacement) discard the result; linkout returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("List::remove_4ef020", 0x004ef020, &H::remove_4ef020);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b109_19.cpp (game/b109_19)
namespace {
namespace game_b109_19 {

using u8 = std::uint8_t;

using joan::hook::x87::ftol;
using i8 = std::int8_t;

using joan::call::thiscall;

using i16 = std::int16_t;

using joan::img::obj;

using joan::hook::global;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

// v3d_87.dll exports are cdecl, called through their IAT slot.
using joan::orig::helpers::v3d_call;
using joan::orig::helpers::fbits;
i32 ftolf(float f) { return ftol(static_cast<double>(f)); }

struct V3 {  // 12 bytes copied as raw dwords
    u32 x, y, z;
};

V3 load_v3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

void* const kUnits = obj(d::g_units);
i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0
u8* list_get(const void* list, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i))); }  // 0x0041b3f0
i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
// length_sq returns double in the replacement (src/lib/v3d/v3d87.cpp): on x86 st0 is read as the float it always
// was (same code as before); x64 returns the double in xmm0, so it is read as a double there.
#if UINTPTR_MAX == 0xffffffffu
float vec_length_sq(const void* v) { return v3d_call<float>(d::iat_v3d_87_length_sq, v); }       // length_sq
#else
float vec_length_sq(const void* v) { return static_cast<float>(v3d_call<double>(d::iat_v3d_87_length_sq, v)); }
#endif
// 0x0060cc90 vector subtract (cdecl out, a, b): out = a - b.
V3 vsub(const void* a, const void* b) {
    V3 out{0, 0, 0};
    V3 copy = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(static_cast<void*>(&out), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
    return copy;
}

JOAN_COUNTER(c_triggers, "Triggers::update_4ef090");

struct H {
    // -------------------------------------------------------------- 0x004ef090
    // (flags & 1 || flags & 0x400) && +0xa == 1 and the referenced unit is gone / busy: skip.
    static bool skip_inactive(u8* r) {
        if ((jo::as_trigger(r)->flags & 1) == 0 && (jo::as_trigger(r)->flags & 0x400) == 0) return false;
        if (jo::as_trigger(r)->ref_kind != 1) return false;
        if (unit_missing(jo::as_trigger(r)->ref_id) != 0) return true;
        u8* const u = unit_get(jo::as_trigger(r)->ref_id);
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->is_kind_510f80()) == 0) return true;  // 0x00510f80
        if (jo::as_unit(u)->anim_kind == 0x49 && global<u32>(d::g_now) - jo::as_actor(jo::as_unit(u)->actor)->start_time < 0x50 &&
            jo::as_trigger(r)->sub == 0)
            return true;
        return false;
    }
    // flags & 0x10 && +0xa == 1: the referenced unit must exist and be of kind 0x3a.
    static bool skip_kind(u8* r) {
        if ((jo::as_trigger(r)->flags & 0x10) == 0 || jo::as_trigger(r)->ref_kind != 1) return false;
        if (unit_missing(jo::as_trigger(r)->ref_id) != 0) return true;
        return jo::as_unit(unit_get(jo::as_trigger(r)->ref_id))->anim_kind != 0x3a;
    }
    // Area check against the hero [0xa5b9e8]: true when inside (radius +0x1f around +0x13).
    static bool hero_inside(u8* r) {
        if (unit_missing(global<i32>(d::g_player_unit_recno)) != 0) return false;
        u8* const hero = unit_get(global<i32>(d::g_player_unit_recno));
        const V3 d = vsub(jo::as_trigger(r)->center, jo::as_unit(hero)->position_);
        const float lsq = vec_length_sq(&d);
        const float rr = jo::as_trigger(r)->range * jo::as_trigger(r)->range;
        return rr >= lsq;  // fcompp + test ah,1: C0 (less or unordered) skips
    }
    // Unit u against trigger r (0x004ef757 .. 0x004ef9c3): true when inside.
    static bool unit_inside(u8* r, u8* u) {
        V3 v{0, 0, 0};
        joan::orig::as_unit(u)->position(&v);  // 0x004fedb0
        const u32 flags = jo::as_trigger(r)->flags;
        if ((flags & 0x2000) != 0 || (jo::as_trigger(r)->flags & 0x4000) != 0 || (jo::as_trigger(r)->flags & 0x40) != 0) {
            u8* const d = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(u)->type_id)));  // 0x0042d0a0
            if (jo::as_unit_type(d)->shape_kind == 2) {
                float inv[16];
                float local[16];
                std::memset(inv, 0, sizeof inv);
                std::memset(local, 0, sizeof local);
                if (jo::as_unit(u)->actor != nullptr) {
                    void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::as_actor(jo::as_unit(u)->actor)->bone)->matrix());  // 0x0060ef80
                    v3d_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), m);                   // matrix_inverse
                } else {
                    joan::orig::matrix_rotation(joan::orig::arg::mptr(static_cast<void*>(local)), joan::orig::arg::f32(jo::view<u32>(jo::as_unit(u)->heading)));         // 0x0060bc50
                    v3d_call<void*>(d::iat_v3d_87_matrix_multiply_translation, static_cast<void*>(local), static_cast<void*>(jo::as_unit(u)->position_));  // multiply_translation
                    v3d_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), static_cast<void*>(local));    // matrix_inverse
                }
                float p[3] = {0.0f, 0.0f, 0.0f};
                v3d_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(p), static_cast<void*>(jo::as_trigger(r)->center),
                                static_cast<void*>(inv));  // point_transform
                const float h = jo::as_trigger(r)->range;
                float box[6];
                box[0] = p[0] - h;
                box[1] = p[1] - h;
                box[2] = p[2] - h;
                box[3] = p[0] + h;
                box[4] = p[1] + h;
                box[5] = p[2] + h;
                return (joan::orig::arg::as<u32>(joan::orig::as_box(jo::as_unit_type(d)->shape_box)->overlaps(static_cast<void*>(box))) & 0xff) != 0;  // 0x0060d6b0
            }
            const float dx1 = jo::as_trigger(r)->center[0] - fbits(v.x);
            const float dx2 = jo::as_trigger(r)->center[0] - fbits(v.x);
            const float px = dx1 * dx2;
            const float dy1 = jo::as_trigger(r)->center[1] - fbits(v.y);
            const float dy2 = jo::as_trigger(r)->center[1] - fbits(v.y);
            const float py = dy1 * dy2;
            const float planar = px + py;
            const float rr = jo::as_trigger(r)->range * jo::as_trigger(r)->range;
            if (!(rr >= planar)) return false;
            const float dz = jo::as_trigger(r)->center[2] - fbits(v.z);
            const double adz = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(dz))));  // fabs
            return static_cast<double>(jo::as_trigger(r)->range) >= adz;
        }
        const V3 d = vsub(jo::as_trigger(r)->center, &v);
        const float lsq = vec_length_sq(&d);
        const float rr = jo::as_trigger(r)->range * jo::as_trigger(r)->range;
        return rr >= lsq;  // NaN: outside (as the original)
    }
    void triggers_update() {
        c_triggers.hit();
        if (list_count(this) != 0) joan::orig::dynarray(this)->quick_sort(joan::orig::arg::cmp(fn::code_value(fn::cmp_byte12_45bdf0)));  // 0x005850b0
        for (i32 idx = list_count(this); idx > 0; --idx) {
            if (joan::orig::arg::as<i32>(joan::orig::as_triggers(this)->is_empty_at(joan::orig::arg::u32(idx))) != 0) continue;  // 0x004f0290
            u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->at_485f20(joan::orig::arg::u32(idx)));       // 0x00485f20
            if ((jo::as_trigger(r)->flags & 4) != 0) {
                if (hero_inside(r)) joan::orig::as_scene_s(jo::as_trigger(r)->scene)->set_casts_shadows(static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})));  // 0x005f0340
                continue;
            }
            if ((jo::as_trigger(r)->flags & 8) != 0) {
                if (hero_inside(r)) joan::orig::as_scene_s(jo::as_trigger(r)->scene)->set_casts_shadows(static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})));  // 0x005f0340
                continue;
            }
            if ((jo::as_trigger(r)->flags & 0x80) != 0) {
                if (hero_inside(r)) global<float>(d::g_a48cdc) = static_cast<float>(jo::as_trigger(r)->value24);
                continue;
            }
            if (static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_trigger(r)->timer_expired())) != 0) {  // 0x0043a530
                joan::orig::as_list(this)->remove_4ef020(joan::orig::arg::u32(idx));                       // 0x004ef020
                continue;
            }
            joan::orig::as_trigger(r)->follow_target();  // 0x00468a80
            if (jo::as_trigger(r)->next_time > global<u32>(d::g_now)) continue;
            jo::view<u8>(jo::as_trigger(r)->active) = 0;
            const i16 idx16 = static_cast<i16>(idx);
            if (jo::as_trigger(r)->target_kind == 2) {
                if (jo::as_trigger(r)->target_id <= 0) {
                    for (i32 j = list_count(obj(d::g_map_object_list)); j > 0; --j) {
                        u8* const o = list_get(obj(d::g_map_object_list), j);
                        if (o == nullptr) continue;
                        if (skip_inactive(r)) continue;
                        V3 dummy{0, 0, 0};
                        joan::orig::as_map_object(o)->position(&dummy);  // 0x00499930
                        const i32 radius = ftolf(jo::as_trigger(r)->range);
                        const V3 c = load_v3(jo::as_trigger(r)->center);
                        if (static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_unit(o)->in_footprint_41c5e0(joan::orig::arg::f32(c.x), joan::orig::arg::f32(c.y), c.z, radius))) == 0) continue;  // 0x0041c5e0
                        virtual_call(o, 0x48, idx16);
                    }
                } else {
                    if (joan::orig::arg::as<i32>(joan::orig::as_types(obj(d::g_a50530))->empty_at(i32{jo::as_trigger(r)->target_id})) != 0) continue;  // 0x00485410
                    u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_a50530))->at_plus4(joan::orig::arg::u32(i32{jo::as_trigger(r)->target_id})));      // 0x004853e0
                    if (skip_inactive(r)) continue;
                    virtual_call(o, 0x48, idx16);
                }
                continue;
            }
            if (jo::as_trigger(r)->target_kind == 1) {
                if (unit_missing(jo::as_trigger(r)->target_id) != 0) {
                    for (i32 j = list_count(kUnits); j > 0; --j) {
                        u8* const u = list_get(kUnits, j);
                        if (u == nullptr) continue;
                        if (jo::as_trigger(r)->ref_id == j && jo::as_trigger(r)->ref_kind == 1) continue;
                        u8* const d = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(u)->type_id)));  // 0x0042d0a0
                        if ((jo::as_trigger(r)->flags & jo::as_unit_type(d)->trigger_flags) == 0) continue;
                        if (skip_inactive(r)) continue;
                        if (skip_kind(r)) continue;
                        if (!unit_inside(r, u)) continue;
                        if (static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_unit(u)->is_kind_40_42())) != 0) {  // 0x00508fb0
                            if (jo::as_trigger(r)->ref_kind == 1 && unit_missing(jo::as_trigger(r)->ref_id) == 0 &&
                                jo::as_unit(unit_get(jo::as_trigger(r)->ref_id))->anim_kind == 0x46)
                                continue;
                            if (jo::as_trigger(r)->sub != 0) continue;
                        }
                        const i8 res = static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_unit(u)->on_message_75d540(joan::orig::arg::u32(static_cast<i32>(idx16)))));  // 0x0075d540
                        if (res == 2) break;
                    }
                } else {
                    u8* const u = unit_get(jo::as_trigger(r)->target_id);
                    u8* const d = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(u)->type_id)));  // 0x0042d0a0
                    if ((jo::as_trigger(r)->flags & jo::as_unit_type(d)->trigger_flags) == 0) continue;
                    if (skip_inactive(r)) continue;
                    if (skip_kind(r)) continue;
                    joan::orig::as_unit(unit_get(jo::as_trigger(r)->target_id))->on_message_75d540(joan::orig::arg::u32(idx16));  // 0x0075d540
                }
                continue;
            }
            if (jo::as_trigger(r)->target_kind == 4) {
                if (jo::as_trigger(r)->ref_kind == 1) {
                    if (unit_missing(jo::as_trigger(r)->ref_id) != 0) {
                        joan::orig::as_list(this)->remove_4ef020(joan::orig::arg::u32(idx));  // 0x004ef020
                        continue;
                    }
                } else if (jo::as_trigger(r)->ref_kind == 2) {
                    if (joan::orig::as_units(obj(d::g_items))->list.empty_at(i32{jo::as_trigger(r)->ref_id}) != 0) {  // 0x00425d50
                        joan::orig::as_list(this)->remove_4ef020(joan::orig::arg::u32(idx));                                          // 0x004ef020
                        continue;
                    }
                }
                joan::orig::as_sound_list(obj(d::g_sfx_player))->play_zone_sound(joan::orig::arg::i16(idx16));  // 0x004c0680
            }
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_19", name, addr, fn)
B("Triggers::update_4ef090", 0x004ef090, &H::triggers_update);
#undef B

}  // namespace game_b109_19
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
namespace {
namespace game_b86_06 {

using joan::call::thiscall;

struct H {
    // ---- wrappers ---------------------------------------------------------------------------
    void wrap_4f0500() { joan::orig::dynarray(this)->destruct(); }  // 0x005844b0
    void wrap_4effd0() { joan::orig::as_trigger(this)->destruct_base(); }  // 0x004f0030
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("Wrap::f_4f0500", 0x004f0500, &H::wrap_4f0500);
B("Wrap::f_4effd0", 0x004effd0, &H::wrap_4effd0);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_b87_01.cpp (game/b87_01)
namespace {
namespace game_b87_01 {

struct H {
    // ---- 0x004f0030 (empty) -------------------------------------------------------------------
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_01", name, addr, fn)
B("Obj::nop_4f0030", 0x004f0030, &H::nop);
#undef B

}  // namespace game_b87_01
}  // namespace

// ---------------------------------------- from modules/game_b100_07.cpp (game/b100_07)
namespace {
namespace game_b100_07 {

struct H {
    // ------------------------------------------------------------------------ 0x004f0290
    u32 lacks(u32 a) { return joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(a)) == nullptr ? 1u : 0u; }  // 0x0041b3f0
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_07", name, addr, fn)
B100("X::lacks", 0x004f0290, &H::lacks);
#undef B100

}  // namespace game_b100_07
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Triggers::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_05::H::ctor_4eed50>(this)); }  // 0x4eed50
auto joan::orig::List::delete_all_eee10() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b84_03::H::delete_all_eee10>(this)); }  // 0x4eee10
auto joan::orig::Trigger::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::sdd_4eeeb0>(this, a0)); }  // 0x4eeeb0
auto joan::orig::List::remove_4ef020(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_04::H::remove_4ef020>(this, a0)); }  // 0x4ef020
auto joan::orig::Trigger::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::zero_43>(this)); }  // 0x4eff60
auto joan::orig::Wrap::f_4effd0() -> void { joan::bind::member<&::game_b86_06::H::wrap_4effd0>(this); }  // 0x4effd0
auto joan::orig::Trigger::destruct_base() -> void { joan::bind::member<&::game_b87_01::H::nop>(this); }  // 0x4f0030
auto joan::orig::Triggers::is_empty_at(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_07::H::lacks>(this, a0)); }  // 0x4f0290
auto joan::orig::Wrap::f_4f0500() -> void { joan::bind::member<&::game_b86_06::H::wrap_4f0500>(this); }  // 0x4f0500
