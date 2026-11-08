// game/unknown_mlfader.cpp: functions whose original source file is unknown (formerly game/_unplaced/mlfader.cpp;
// re/attribution.csv: between ww/lightsys.cpp and ww/modelres.cpp). They move to their original file when evidence
// names it (Phase 3).
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
#include "../x87.hpp"
#include "../canvas_api.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;

using joan::orig::helpers::at;

using joan::call::thiscall;

using u8 = std::uint8_t;

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {

using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    H* clear_24() {
        memset_(this, 0, static_cast<u32>(sizeof(jo::Children)));  // 0x24 on x86: the Children / Fader object (x64: its pointer is 8 bytes)
        return this;
    }

    // ------------------------------------------------------------------------ 0x004afc20
    void set_8_one() { jo::view<u32>(jo::self<jo::Fader>(this)->direction) = 1; }

    // ------------------------------------------------------------------------ 0x004afcf0
    void set_0(u32 v) { jo::self<jo::Fader>(this)->state = v; }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Block24::clear", 0x004afbc0, &H::clear_24);
B94("Obj::set_8_one", 0x004afc20, &H::set_8_one);
B94("Obj::set_0", 0x004afcf0, &H::set_0);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
namespace {
namespace game_b86_06 {

struct H {
    void wrap_4afbf0() { joan::orig::as_children(this)->destruct(); }  // 0x004afc50
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("Wrap::f_4afbf0", 0x004afbf0, &H::wrap_4afbf0);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_b87_04.cpp (game/b87_04)
namespace {
namespace game_b87_04 {

struct H {
    // ---- 0x004afc50 --------------------------------------------------------------------------
    void clear_4afc50() {
        for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_children(this)->count()); i > 0; --i) {  // 0x004b01e0
            joan::orig::as_children(this)->remove(joan::orig::arg::i32(i));                                              // 0x004b0320
        }
        if (jo::view<void*>(jo::self<jo::Children>(this)->lights) == nullptr) return;
        void* const p = jo::view<void*>(jo::self<jo::Children>(this)->lights);
        if (p != nullptr) joan::orig::as_dynarray(p)->destroy(1u);  // 0x00463fc0 (result dead)
        jo::view<void*>(jo::self<jo::Children>(this)->lights) = nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_04", name, addr, fn)
B("Obj4afc50::clear", 0x004afc50, &H::clear_4afc50);
#undef B

}  // namespace game_b87_04
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using u32 = std::uint32_t;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x004afd20
    u32 set3_4afd20(u32 a, u32 b, u32 c) {
        jo::view<u32>(jo::self<jo::Fader>(this)->lo) = a;
        jo::view<u32>(jo::self<jo::Fader>(this)->hi) = b;
        jo::view<u32>(jo::self<jo::Fader>(this)->speed) = c;
        return b;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Obj::set3_4afd20", 0x004afd20, &H::set3_4afd20);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_b104_03.cpp (game/b104_03)
namespace {
namespace game_b104_03 {

using joan::hook::global;

using u32 = std::uint32_t;

using joan::img::obj;

using joan::hook::x87::Fpu;

void* const kMath = obj(d::g_misc);

u32 now() { return global<u32>(d::g_now); }
bool eq2(float a, float b) { return !(a < b || a > b); }        // equal or unordered

JOAN_COUNTER(c_fader, "Fader::update");

struct H {
    // ------------------------------------------------------------------ 0x004afd60
    double rnd() { return joan::orig::arg::as<double>(joan::orig::as_misc(kMath)->rand_unit_inclusive()); }  // 0x00590660 (st0)
    double clamp(double v, double lo, double hi) {
        return joan::orig::arg::as<double>(joan::orig::as_misc(kMath)->limit_double(joan::orig::arg::f64(v), joan::orig::arg::f64(lo), joan::orig::arg::f64(hi)));  // 0x00590b00 (qword args, ret 0x18)
    }
    void fader_restart() {
        const Fpu fpu{true};
        jo::self<jo::Fader>(this)->start_time = now();
        double v = fpu.sub(static_cast<double>(jo::self<jo::Fader>(this)->hi), static_cast<double>(jo::self<jo::Fader>(this)->lo));
        v = fpu.mul(v, 1000.0);
        v = fpu.div(v, static_cast<double>(jo::self<jo::Fader>(this)->speed));
        jo::self<jo::Fader>(this)->duration = joan::hook::x87::ftol(v);
        const float g = static_cast<float>(static_cast<double>(jo::view<u32>(jo::self<jo::Fader>(this)->duration)));  // fild qword (unsigned) -> float
        double r = fpu.sub(rnd(), 0.25);
        r = fpu.mul(r, 0.5);
        r = fpu.mul(r, static_cast<double>(jo::self<jo::Fader>(this)->duration));  // fimul dword (signed)
        r = fpu.add(r, static_cast<double>(g));
        jo::self<jo::Fader>(this)->duration = joan::hook::x87::ftol(r);
    }
    void fader_update() {
        c_fader.hit();
        const Fpu fpu{true};
        const u32 state = jo::self<jo::Fader>(this)->state;
        if (state == 1) {
            if (joan::orig::arg::as<i32>(joan::orig::as_children(this)->count()) == 0) return;  // 0x004b01e0
            joan::orig::as_children(this)->get(1);                // 0x004b0220 (result unused)
            float progress = 0.0f;
            if (jo::self<jo::Fader>(this)->start_time != 0) {
                const u32 d = jo::view<u32>(jo::self<jo::Fader>(this)->direction) == 1 ? now() - jo::self<jo::Fader>(this)->start_time : jo::self<jo::Fader>(this)->start_time + jo::view<u32>(jo::self<jo::Fader>(this)->duration) - now();
                progress = static_cast<float>(fpu.div(static_cast<double>(d), static_cast<double>(jo::self<jo::Fader>(this)->duration)));
            }
            float c;
            if (jo::self<jo::Fader>(this)->start_time == 0 || !(progress > 0.0f)) {
                c = 0.0f;
                jo::view<u32>(jo::self<jo::Fader>(this)->direction) = 1;
                fader_restart();
            } else if (!(progress < 1.0f)) {
                c = jo::self<jo::Fader>(this)->hi - jo::self<jo::Fader>(this)->lo;
                jo::self<jo::Fader>(this)->direction = -1;
                fader_restart();
            } else {
                const float p2 = progress * progress;
                const float p4 = p2 * p2;
                c = (jo::self<jo::Fader>(this)->hi - jo::self<jo::Fader>(this)->lo) * p4;
            }
            const double hi = static_cast<double>(jo::self<jo::Fader>(this)->hi);
            const double lo = static_cast<double>(jo::self<jo::Fader>(this)->lo);
            double x = fpu.sub(rnd(), 0.5);
            x = fpu.mul(x, 1.0);
            x = fpu.mul(x, static_cast<double>(c));
            x = fpu.add(x, static_cast<double>(jo::self<jo::Fader>(this)->lo));
            c = static_cast<float>(clamp(x, lo, hi));
            for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_children(this)->count()); i > 0; --i) {
                u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_children(this)->get(joan::orig::arg::i32(i)));
                jo::as_light(e)->attenuation1 = c;
                joan::orig::as_light(e)->invalidate();  // 0x005c5c00
            }
        } else if (state == 2) {
            if (joan::orig::arg::as<i32>(joan::orig::as_children(this)->count()) == 0) return;
            u8* const e1 = joan::orig::arg::as<u8*>(joan::orig::as_children(this)->get(1));
            if (eq2(jo::self<jo::Fader>(this)->lo, jo::as_light(e1)->attenuation1)) jo::view<u32>(jo::self<jo::Fader>(this)->direction) = 1;    // test ah,0x40 je: equal or NaN
            if (eq2(jo::self<jo::Fader>(this)->hi, jo::as_light(e1)->attenuation1)) jo::self<jo::Fader>(this)->direction = -1;
            for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_children(this)->count()); i > 0; --i) {
                u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_children(this)->get(joan::orig::arg::i32(i)));
                const double hi = static_cast<double>(jo::self<jo::Fader>(this)->hi);
                const double lo = static_cast<double>(jo::self<jo::Fader>(this)->lo);
                double v = static_cast<double>(global<u32>(d::g_frame_step_ms));  // fild qword (unsigned)
                v = fpu.mul(v, static_cast<double>(jo::self<jo::Fader>(this)->speed));
                v = fpu.div(v, 1000.0);
                v = fpu.mul(v, static_cast<double>(jo::self<jo::Fader>(this)->direction));  // fimul dword
                v = fpu.add(v, static_cast<double>(jo::as_light(e)->attenuation1));
                jo::as_light(e)->attenuation1 = static_cast<float>(clamp(v, lo, hi));
                joan::orig::as_light(e)->invalidate();  // 0x005c5c00
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_03", name, addr, fn)
B("Fader::update", 0x004afd60, &H::fader_update);
#undef B

}  // namespace game_b104_03
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

std::int32_t list_count(void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    // 0x004b0320 Children::remove(i)
    std::uint32_t children_remove(std::int32_t i) {
        jo::as_light(joan::orig::arg::as<void*>(joan::orig::as_children(this)->get(joan::orig::arg::i32(i))))->children = nullptr;           // 0x004b0220
        joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Fader>(this)->lights))->linkout_584a10(i);           // 0x00584a10
        return 0;  // void callee; callers discard the result
    }
    // 0x004b01e0 Children::count
    std::int32_t children_count() {
        if (jo::view<void*>(jo::self<jo::Fader>(this)->lights) == nullptr) return 0;
        return list_count(jo::view<void*>(jo::self<jo::Fader>(this)->lights));
    }
    // 0x004b0220 Children::get(i)
    void* children_get(std::int32_t i) { return joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Fader>(this)->lights))->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("Children::remove", 0x004b0320, &H::children_remove);
L81("Children::count", 0x004b01e0, &H::children_count);
L81("Children::get", 0x004b0220, &H::children_get);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
namespace {
namespace game_b92_03 {

using joan::orig::helpers::game_new;  // operator new

struct H {
    // 0x004b0250
    std::uint32_t push_20(void* v) {
        if (jo::view<void*>(jo::self<jo::Fader>(this)->lights) == nullptr) {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
            void* list = nullptr;
            if (mem != nullptr) list = joan::orig::dynarray(mem)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x1e, 0);  // 0x005843d0 (pointers: 4 on x86)
            jo::view<void*>(jo::self<jo::Fader>(this)->lights) = list;
        }
        joan::orig::dynarray(jo::view<void*>(jo::self<jo::Fader>(this)->lights))->linkin(&v);                // 0x00584620
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Fader>(this)->lights))->cursor());  // 0x00417520
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Obj::push_20", 0x004b0250, &H::push_20);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
namespace {
namespace game_leaves80b {

JOAN_COUNTER(c_child_remove, "ChildList::remove");

struct H {
    // 0x004b0370
    void child_remove(void* child) {
        c_child_remove.hit();
        for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_children(this)->count()); i > 0; --i) {  // 0x004b01e0 count
            if (joan::orig::arg::as<void*>(joan::orig::as_children(this)->get(joan::orig::arg::i32(i))) == child) {                          // 0x004b0220 get
                joan::orig::as_children(this)->remove(joan::orig::arg::i32(i));                                             // 0x004b0320 remove
                break;
            }
        }
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("ChildList::remove", 0x004b0370, &H::child_remove);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

using u32 = std::uint32_t;

using joan::img::str;

JOAN_COUNTER(c_mgr_remove, "Manager::remove");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

// ------------------------------------------------------------------------------- cdecl
i32 JOAN_CDECL attenuation_of_name(const char* name) {  // 0x004b03e0
    if (stricmp_(name, str(d::s_NONE_9d4350)) == 0) return 0;   // "NONE"
    if (stricmp_(name, str(d::s_RANDOM_ATTENUATION)) == 0) return 1;   // "RANDOM_ATTENUATION"
    return 0;
}

struct H {
    // ------------------------------------------------------------------------ 0x004b0700
    u32 mgr_remove(u32 id) {
        c_mgr_remove.hit();
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_at_4(joan::orig::arg::i32(id)));  // 0x00484710
        if (p != nullptr) joan::orig::as_children(p)->destroy(joan::orig::arg::u32(i32{1}));  // 0x004b05f0
        return thiscall<u32>(fn::linkout_5858c0, &jo::self<jo::Mgr>(this)->faders, id);  // 0x005858c0
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("Sound::attenuation_of_name", 0x004b03e0, &attenuation_of_name);
B93("Manager::remove", 0x004b0700, &H::mgr_remove);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x004b0440
    void* ctor_4b0440() {
        joan::orig::dynarray_b(&jo::self<jo::Mgr>(this)->faders)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0, 0);  // 0x005856a0 (pointers: 4 on x86) (+4 on x86: after the leading pointer)
        jo::self<jo::Mgr>(this)->vtable = img::obj(d::vt_Obj_9d5278);  // vtable
        return this;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Obj::Obj_4b0440", 0x004b0440, &H::ctor_4b0440);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

using u32 = std::uint32_t;

using joan::orig::helpers::operator_new;

JOAN_COUNTER(c_add_new, "Obj::add_new_4afbc0");

struct H {
    // ------------------------------------------------------------------ 0x004b0480
    u32 dtor_4b0480() {
        jo::self<jo::Mgr>(this)->vtable = img::obj(d::vt_Obj_9d5278);
        joan::orig::as_list(this)->clear_4b0530();  // 0x004b0530
        // The original's `this ? this + 4 : 0` base adjustment: `this` was already written
        // through above, so the null branch is dead.
        return thiscall<u32>(fn::dtor_DynArrayB, &jo::self<jo::Mgr>(this)->faders);  // 0x00585710
    }

    // ------------------------------------------------------------------ 0x004b0640
    u32 add_new() {
        c_add_new.hit();
        void* const p = operator_new(static_cast<std::uint32_t>(sizeof(jo::Children)));  // 0x24 on x86
        void* item = p != nullptr ? joan::orig::as_children(p)->construct() : nullptr;  // 0x004afbc0
        joan::orig::as_children(item)->reset_direction();                                           // 0x004afc20
        joan::orig::dynarray_b(&jo::self<jo::Mgr>(this)->faders)->linkin(static_cast<void*>(&item));        // 0x005857a0
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(&jo::self<jo::Mgr>(this)->faders)->cursor());  // 0x00417520
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Obj::dtor_4b0480", 0x004b0480, &H::dtor_4b0480);
B95("Obj::add_new_4afbc0", 0x004b0640, &H::add_new);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_update, "Mgr::update_4b0780");

struct H {
    void nop0() {}

    // ------------------------------------------------------------------ 0x004b0780
    i32 update_4b0780() {
        c_update.hit();
        void* const list = &jo::self<jo::Mgr>(this)->faders;  // the DynArray after the vtable pointer
        i32 i = joan::orig::as_dynarray(list)->size();  // 0x00417ad0
        for (; i > 0; --i) {
            void* const e = joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (e == nullptr) continue;
            joan::orig::as_children(e)->update();                                  // 0x004afd60
            if (joan::orig::arg::as<u32>(joan::orig::as_children(e)->count()) == 0) joan::orig::as_light_faders(this)->remove_group(joan::orig::arg::u32(i));  // 0x004b01e0 / 0x004b0700
        }
        return i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("Obj::nop_4b0510", 0x004b0510, &H::nop0);
B("Mgr::update_4b0780", 0x004b0780, &H::update_4b0780);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
namespace {
namespace game_b84_06 {

JOAN_COUNTER(c_clear2, "List::clear_4b0530");

using joan::orig::helpers::count_of;  // 0x00417ad0
void remove_at(const void* list, std::int32_t i) { joan::orig::dynarray_b(list)->linkout(joan::orig::arg::i32(i)); }  // 0x005858c0
void compact(const void* list) { joan::orig::as_sound(list)->reset_585d20(); }  // 0x00585d20

struct H {
    // ------------------------------------------------------------------ 0x004b0530
    std::uint32_t clear_4b0530() {
        c_clear2.hit();
        void* const list = &jo::self<jo::Mgr>(this)->faders;
        for (std::int32_t i = count_of(list); i >= 1; --i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null_at_4(joan::orig::arg::i32(i))) != 0) continue;  // 0x00484740
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_at_4(joan::orig::arg::i32(i)));            // 0x00484710
            if (e != nullptr) joan::orig::as_children(e)->destroy(joan::orig::arg::u32(static_cast<std::uint32_t>(1)));  // 0x004b05f0 deleting dtor
            remove_at(list, i);
        }
        compact(list);
        return 0;  // void callee; callers discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("List::clear_4b0530", 0x004b0530, &H::clear_4b0530);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::orig::helpers::game_delete;                      // operator delete

using joan::orig::helpers::scalar_delete;

struct H {
    void* sdd_4b05f0(std::uint32_t flags) { joan::orig::as_wrap(this)->f_4afbf0(); scalar_delete(this, flags); return this; }  // 0x004afbf0
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj4b05f0::sdd", 0x004b05f0, &H::sdd_4b05f0);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_find4, "List::find_by_4");

i32 count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------------ 0x004b0810
    void* find_by_4(u32 v) {
        c_find4.hit();
        for (i32 i = count(&jo::self<jo::Mgr>(this)->faders); i > 0; --i) {  // this = the Mgr (vtable, DynArrayB faders)
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at_4(joan::orig::arg::i32(i))) != 0) continue;  // 0x00484740
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_at_4(joan::orig::arg::i32(i)));   // 0x00484710
            if (jo::as_fader(e)->u32_4 == v) return e;
        }
        return nullptr;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("List::find_by_4", 0x004b0810, &H::find_by_4);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Children::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_02::H::clear_24>(this)); }  // 0x4afbc0
auto joan::orig::Wrap::f_4afbf0() -> void { joan::bind::member<&::game_b86_06::H::wrap_4afbf0>(this); }  // 0x4afbf0
auto joan::orig::Children::reset_direction() -> void { joan::bind::member<&::game_b94_02::H::set_8_one>(this); }  // 0x4afc20
auto joan::orig::Children::destruct() -> void { joan::bind::member<&::game_b87_04::H::clear_4afc50>(this); }  // 0x4afc50
auto joan::orig::Children::set_type(std::uint32_t a0) -> void { joan::bind::member<&::game_b94_02::H::set_0>(this, a0); }  // 0x4afcf0
auto joan::orig::Children::set_range(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_04::H::set3_4afd20>(this, a0, a1, a2)); }  // 0x4afd20
auto joan::orig::Children::update() -> void { joan::bind::member<&::game_b104_03::H::fader_update>(this); }  // 0x4afd60
auto joan::orig::Children::count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves81::H::children_count>(this)); }  // 0x4b01e0
auto joan::orig::Children::get(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves81::H::children_get>(this, a0)); }  // 0x4b0220
auto joan::orig::Children::add_light(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_03::H::push_20>(this, a0)); }  // 0x4b0250
auto joan::orig::Children::remove(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves81::H::children_remove>(this, a0)); }  // 0x4b0320
auto joan::orig::Children::remove_child(void* a0) -> void { joan::bind::member<&::game_leaves80b::H::child_remove>(this, a0); }  // 0x4b0370
auto joan::orig::sound::attenuation_of_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_02::attenuation_of_name>(a0)); }  // 0x4b03e0
auto joan::orig::List::clear_4b0530() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b84_06::H::clear_4b0530>(this)); }  // 0x4b0530
auto joan::orig::Children::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::sdd_4b05f0>(this, a0)); }  // 0x4b05f0
auto joan::orig::LightFaders::add_group() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_02::H::add_new>(this)); }  // 0x4b0640
auto joan::orig::LightFaders::remove_group(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b93_02::H::mgr_remove>(this, a0)); }  // 0x4b0700
auto joan::orig::List::find_by_4(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_03::H::find_by_4>(this, a0)); }  // 0x4b0810
