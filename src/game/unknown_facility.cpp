// game/unknown_facility.cpp: functions whose original source file is unknown (formerly game/_unplaced/facility.cpp;
// re/attribution.csv: between ww/f_shop.cpp and ww/flocksys.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::hook::global;

using i32 = std::int32_t;

using joan::call::thiscall;

using joan::img::obj;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {

void JOAN_CDECL init_46cbb0() { joan::orig::init_pi_a39158(); }
void JOAN_CDECL init_46cc10() { joan::orig::init_a3915c(); }
void JOAN_CDECL init_46cc60() { joan::orig::init_a39160(); }
void JOAN_CDECL init_46ccb0() { joan::orig::init_a39164(); }

struct H {
    // ------------------------------------------------------------------ tiny methods
    void nop() {}
    u32 true_46d7c0(u32) { return 1; }
    float value_46d700() { return jo::self<jo::Facility>(this)->angle; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("init_46cbb0", 0x0046cbb0, &init_46cbb0);
B("init_46cc10", 0x0046cc10, &init_46cc10);
B("init_46cc60", 0x0046cc60, &init_46cc60);
B("init_46ccb0", 0x0046ccb0, &init_46ccb0);
B("Obj::value_46d700", 0x0046d700, &H::value_46d700);
B("Obj::true_46d7c0", 0x0046d7c0, &H::true_46d7c0);
B("Obj::nop_46cda0", 0x0046cda0, &H::nop);
B("Obj::nop_46d7e0", 0x0046d7e0, &H::nop);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

struct RV3 {  // 12-byte vector copied as raw dwords
    u32 x, y, z;
};
static_assert(sizeof(RV3) == 12, "RV3");

// ---------------------------------------------------------------------- static initialisers
void init_pi(img::Addr g) {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(g) = static_cast<float>(t * 4.0);        // qword 0x9cf788 = 4.0 (exact)
}
static void JOAN_CDECL init_pi_a39158() { init_pi(d::g_a39158); }  // 0x0046cbd0

struct H {
    // ------------------------------------------------------------------ 0x0046d670
    u32 get_29(RV3* out) {
        const RV3 v = jo::view<RV3>(jo::self<jo::Facility>(this)->pos);
        out->x = v.x;
        out->y = v.y;
        out->z = v.z;
        return v.z;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("init_pi_a39158", 0x0046cbd0, &init_pi_a39158);
B98("Obj::get_29", 0x0046d670, &H::get_29);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
namespace {
namespace game_b100_02 {

i32 inv_size(void* inv) { return joan::orig::as_dynarray(inv)->size(); }  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------ 0x0046d200
    i32 find_name(const char* name) {
        for (i32 i = inv_size(this); i > 0; --i) {
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (e == nullptr) continue;
            if (joan::crt::stricmp(joan::orig::arg::str(jo::as_facility(e)->name), joan::orig::arg::str(name)) == 0) return i;  // _stricmp
        }
        return 0;
    }
};

static void JOAN_CDECL init_a3915c() { global<float>(d::g_a3915c) = global<float>(d::g_a39158) / global<float>(d::flt_2); }

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("init_a3915c", 0x0046cc30, &init_a3915c);
B100("NamedList::find", 0x0046d200, &H::find_name);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

// g_out = g_in / g_div (fld dword / fdiv dword / fstp dword).
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a39160() { div_global(d::g_a39160, d::g_a39158, d::flt_3); }  // 0x0046cc80

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("init_a39160", 0x0046cc80, &init_a39160);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_is_free, "Area::is_free");

void JOAN_CDECL init_a39164() { global<float>(d::g_a39164) = global<float>(d::g_a39158) / global<float>(d::flt_4); }

struct H {
    // ------------------------------------------------------------------ 0x0046d630
    void* notify_owner() {
        if (jo::view<void*>(jo::self<jo::Facility>(this)->owner) != nullptr) joan::orig::as_list(jo::view<void*>(jo::self<jo::Facility>(this)->owner))->unlink(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x004c3d80
        return this;  // EAX: this on both paths (0x004c3d80 leaves its arg in EAX)
    }

    // ------------------------------------------------------------------ 0x0046da90
    i32 is_free() {
        c_is_free.hit();
        for (i32 i = joan::orig::as_dynarray(kUnits)->size(); i > 0; --i) {  // 0x00417ad0
            u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (unit == nullptr) continue;
            if (!(jo::view<float[3]>(jo::self<jo::Unit>(unit)->position_)[0] >= jo::self<jo::Area>(this)->bounds[0])) continue;  // < or NaN
            if (jo::view<float[3]>(jo::self<jo::Unit>(unit)->position_)[0] > jo::self<jo::Area>(this)->bounds[3]) continue;
            if (!(jo::view<float[3]>(jo::self<jo::Unit>(unit)->position_)[1] >= jo::self<jo::Area>(this)->bounds[1])) continue;
            if (jo::view<float[3]>(jo::self<jo::Unit>(unit)->position_)[1] > jo::self<jo::Area>(this)->bounds[4]) continue;
            return 0;
        }
        return 1;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("init_a39164", 0x0046ccd0, &init_a39164);
B96("Obj::notify_owner", 0x0046d630, &H::notify_owner);
B96("Area::is_free", 0x0046da90, &H::is_free);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
namespace {
namespace game_b99_02 {

using joan::orig::helpers::u;  // a constructor's result is `this`: pointer-sized

struct H {
    // ------------------------------------------------------------------ 0x0046cd00
    std::uintptr_t ctor_46cd00() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x1e, 0, 0);  // 0x005856a0 (pointers: 4 on x86)
        return u(this);
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("List::List_46cd00", 0x0046cd00, &H::ctor_46cd00);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

struct H {
    // ------------------------------------------------------------------ 0x0046cd30
    u32 dtor_46cd30() {
        joan::orig::as_list(this)->destroy_46cdc0();              // 0x0046cdc0
        return thiscall<u32>(fn::dtor_DynArrayB, this);  // 0x00585710
    }

    // ------------------------------------------------------------------ 0x0046d5b0
    u32 dtor_46d5b0() {
        jo::self<jo::Facility>(this)->vtable = img::obj(d::vt_Obj_9d0a80);
        joan::orig::as_facility(this)->detach();  // 0x0046d630
        return 0;  // 0x0046d630 returns nothing; every caller chain discards the result
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Obj::dtor_46cd30", 0x0046cd30, &H::dtor_46cd30);
B97("Obj::dtor_46d5b0", 0x0046d5b0, &H::dtor_46d5b0);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

std::int32_t list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

JOAN_COUNTER(c_destroy2, "List::destroy_46cdc0");

struct H {
    std::int32_t destroy_46cdc0() {
        c_destroy2.hit();
        for (std::int32_t i = list_count(this); i > 0; --i) {
            void* const p = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (p == nullptr) continue;
            virtual_call<void*>(p, 0, static_cast<std::uint32_t>(1));  // scalar deleting dtor
            joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));   // 0x005858c0
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
        return 0;  // 0x00585d20 returns nothing; callers discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("List::destroy_46cdc0", 0x0046cdc0, &H::destroy_46cdc0);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b87_04.cpp (game/b87_04)
namespace {
namespace game_b87_04 {

using joan::img::str;

std::int32_t stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

JOAN_COUNTER(c_kind, "facility_kind");

struct Vec3 {  // float members: the same System V classification (SSE) as the caller's Vec3 (world_s.cpp
    float x, y, z;  // World::create_facility), which a struct of three dwords (INTEGER) does not share on x64
};
static_assert(sizeof(Vec3) == 12, "vec3");

std::int32_t JOAN_CDECL facility_kind(const char* name) {  // 0x0046d820
    c_kind.hit();
    if (stricmp_(name, str(d::s_BARRACK)) == 0) return 1;     // BARRACK
    if (stricmp_(name, str(d::s_WORKSHOP)) == 0) return 0x10;  // WORKSHOP
    if (stricmp_(name, str(d::s_SHOP)) == 0) return 2;     // SHOP
    if (stricmp_(name, str(d::s_RECRUIT)) == 0) return 3;     // RECRUIT
    if (stricmp_(name, str(d::s_STABLE)) == 0) return 4;     // STABLE
    if (stricmp_(name, str(d::s_WEAPON)) == 0) return 5;     // WEAPON
    if (stricmp_(name, str(d::s_GRANARY)) == 0) return 6;     // GRANARY
    if (stricmp_(name, str(d::s_HOUSE)) == 0) return 7;     // HOUSE
    if (stricmp_(name, str(d::s_PUB)) == 0) return 8;     // PUB
    if (stricmp_(name, str(d::s_CHAPEL)) == 0) return 9;     // CHAPEL
    if (stricmp_(name, str(d::s_POST)) == 0) return 0xa;   // POST
    if (stricmp_(name, str(d::s_GUARD)) == 0) return 0xb;   // GUARD
    if (stricmp_(name, str(d::s_KING)) == 0) return 0xc;   // KING
    if (stricmp_(name, str(d::s_PRISON)) == 0) return 0xd;   // PRISON
    if (stricmp_(name, str(d::s_HALL)) == 0) return 0xe;   // HALL
    if (stricmp_(name, str(d::s_MONASTERY)) == 0) return 0xf;   // MONASTERY
    return 0;
}

struct H {
    // ---- 0x0046ce70 FacilityFactory::create(kind) ret 4 -------------------------------------
    std::int32_t create(std::int32_t kind) {
        std::uint8_t* p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_facility_list(this)->new_facility(joan::orig::arg::i32(kind)));  // 0x0046cef0
        joan::orig::dynarray_b(this)->linkin(static_cast<void*>(&p));              // 0x005857a0
        jo::view<std::uint16_t>(jo::as_facility(p)->i16_6) = joan::orig::arg::as<std::uint16_t>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
        jo::view<std::uint16_t>(jo::as_facility(p)->kind) = static_cast<std::uint16_t>(kind);
        jo::view<std::uint16_t>(jo::as_facility(p)->i16_6) = joan::orig::arg::as<std::uint16_t>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
        return static_cast<std::int32_t>(jo::as_facility(p)->i16_6);
    }

    // ---- 0x0046d750 Facility::add_entrance(vec*) ret 4 --------------------------------------
    void add_entrance(const Vec3* v) {
        if (jo::self<jo::Facility>(this)->n_entrances >= 2) return;
        std::memcpy(&jo::self<jo::Facility>(this)->entrances[jo::self<jo::Facility>(this)->n_entrances * 3], v, 12);
        jo::self<jo::Facility>(this)->n_entrances = jo::self<jo::Facility>(this)->n_entrances + 1;
    }

    // ---- 0x0046d6b0 Facility::set_placement(vec3, box*) ret 0x10 -----------------------------
    void set_placement(Vec3 pos, const void* box) {
        std::memcpy(jo::self<jo::Facility>(this)->pos, &pos, 12);
        std::memcpy(jo::self<jo::Facility>(this)->bounds, box, 24);
    }

    // ---- 0x0046d720 Facility::set_angle(angle) ret 4 ----------------------------------------
    void set_angle(std::uint32_t angle) { jo::view<std::uint32_t>(jo::self<jo::Facility>(this)->angle) = angle; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_04", name, addr, fn)
B("facility_kind", 0x0046d820, &facility_kind);
B("FacilityFactory::create", 0x0046ce70, &H::create);
B("Facility::add_entrance", 0x0046d750, &H::add_entrance);
B("Facility::set_placement", 0x0046d6b0, &H::set_placement);
B("Facility::set_angle", 0x0046d720, &H::set_angle);
#undef B

}  // namespace game_b87_04
}  // namespace

// ---------------------------------------- from modules/game_b88_03.cpp (game/b88_03)
namespace {
namespace game_b88_03 {

using joan::orig::helpers::game_new;  // operator new

JOAN_COUNTER(c_factory, "BuildingFactory::create");

struct H {
    // ------------------------------------------------------------------------ 0x0046cef0
    void* create(std::int32_t type) {
        c_factory.hit();
        switch (type) {
            case 1: {
                void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Building)));  // 0x140 on x86
                return mem != nullptr ? joan::orig::as_barracks_facility(mem)->construct() : nullptr;  // 0x00467420
            }
            case 2: {
                void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Shop)));  // 0x8e on x86
                return mem != nullptr ? joan::orig::as_shop_facility(mem)->construct() : nullptr;  // 0x00469560
            }
            case 3: {
                void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::FlagFacility)));  // 0x99 on x86
                return mem != nullptr ? joan::orig::as_recruit_facility(mem)->construct() : nullptr;  // 0x00468300
            }
            case 16: {
                void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Siege)));  // 0x2f14 on x86
                return mem != nullptr ? joan::orig::as_workshop_facility(mem)->construct() : nullptr;  // 0x00469e90
            }
            default:
                return nullptr;
        }
    }
};

#define B88(name, addr, fn) JOAN_REPLACE("game/b88_03", name, addr, fn)
B88("BuildingFactory::create", 0x0046cef0, &H::create);
#undef B88

}  // namespace game_b88_03
}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
namespace {
namespace game_b102_02 {

using i16 = std::int16_t;

JOAN_COUNTER(c_find_type, "List::find_type");
JOAN_COUNTER(c_find_46d3d0, "Obj::find_46d3d0");

// ---------------------------------------------------------------- shared game helpers
i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0
void* list_get(const void* list, i32 i) { return joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0

// 0x0046d290 / 0x0046d330 body.
i32 find_type(void* self, i16 type, const char* name) {
    c_find_type.hit();
    for (i32 i = list_count(self); i > 0; --i) {
        u8* const e = static_cast<u8*>(list_get(self, i));
        if (e == nullptr) continue;
        if (static_cast<i32>(jo::as_facility(e)->kind) != type) continue;
        if (joan::crt::stricmp(joan::orig::arg::str(jo::as_facility(e)->name), joan::orig::arg::str(name)) == 0) return i;
    }
    return 0;
}

struct H {
    // 0x0046d0f0
    i32 type_size(i32 type) {
        switch (type) {
        case 1: return 0x140;
        case 2: return 0x8e;
        case 3: return 0x99;
        case 16: return 0x2f14;
        default: return 0;
        }
    }

    i32 find_type1(const char* name) { return find_type(this, 1, name); }    // 0x0046d290
    i32 find_type16(const char* name) { return find_type(this, 16, name); }  // 0x0046d330

    // 0x0046d3d0
    i32 find_46d3d0(u32 a, u8* b, u32 c) {
        c_find_46d3d0.hit();
        if (b == nullptr) {
            const std::int32_t r = joan::orig::as_list(obj(d::g_place_array))->find_owned(joan::orig::arg::ptr(a), joan::orig::arg::mptr(c));  // 0x004c5530 (a: the position pointer, c: the World)
            if (r == 0) return 0;
            b = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_place_array))->get_c70(r));  // 0x00456c70
        }
        // (the original then stores Place::world into c when c == 0: a dead store, not reproduced)
        for (u8* it = joan::orig::arg::as<u8*>(joan::orig::as_place(b)->facility_head()); it != nullptr;  // 0x004c48b0
             it = joan::orig::arg::as<u8*>(joan::orig::get_71(joan::orig::arg::mptr(it)))) {                  // 0x004c4a60
            if (joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::as_facility(it)->bounds)->contains(joan::orig::arg::ptr(a))) != 0) return static_cast<i32>(jo::as_facility(it)->i16_6);  // 0x0060d5f0
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("Obj::type_size", 0x0046d0f0, &H::type_size);
B("List::find_type1", 0x0046d290, &H::find_type1);
B("List::find_type16", 0x0046d330, &H::find_type16);
B("Obj::find_46d3d0", 0x0046d3d0, &H::find_46d3d0);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_b103_02.cpp (game/b103_02)
namespace {
namespace game_b103_02 {

using joan::orig::helpers::game_delete;            // operator delete

struct H {
    void* sdd_46d560(u32 flags) {
        joan::orig::as_facility(this)->destruct();  // 0x0046d5b0
        if ((flags & 1u) != 0) game_delete(this);
        return this;
    }

    // ------------------------------------------------------------------ 0x0046d190
    u32 delete_and_remove(u32 id) {
        void* const p = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3f0
        if (p != nullptr) virtual_call<u32>(p, 0, 1u);           // scalar deleting dtor
        return thiscall<u32>(fn::linkout_5858c0, this, id);              // 0x005858c0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_02", name, addr, fn)
B("Obj::delete_and_remove", 0x0046d190, &H::delete_and_remove);
B("Obj::sdd_46d560", 0x0046d560, &H::sdd_46d560);
#undef B

}  // namespace game_b103_02
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

JOAN_COUNTER(c_slot10_all, "List::call_slot10_all");

struct H {
    // ------------------------------------------------------------------ 0x0046d4a0
    i32 call_slot10_all() {
        c_slot10_all.hit();
        i32 i = 1;
        for (;;) {
            const i32 n = list_count(this);
            if (i > n) return n;
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->empty_at_e(joan::orig::arg::i32(i))) == 0) {          // 0x0044cf80 (slot empty)
                void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_e(joan::orig::arg::i32(i)));  // 0x0043c450
                virtual_call(p, 0x10);
            }
            ++i;
        }
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("List::call_slot10_all", 0x0046d4a0, &H::call_slot10_all);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

struct H {
    // 0x0046d520
    void* record_ctor() {
        jo::self<jo::Facility>(this)->vtable = img::obj(d::vt_Obj_9d0a80);
        std::memset(static_cast<void*>(&jo::self<jo::Facility>(this)->kind), 0, sizeof(jo::Facility) - offsetof(jo::Facility, kind));  // memset 0x75 on x86 (owner / prev / next are pointers)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("Record::Record", 0x0046d520, &H::record_ctor);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::img::str;

using joan::img::str;

struct H {
    // 0x0046d5e0
    char* entry_set(std::int16_t id, const char* name) {
        jo::self<jo::Facility>(this)->kind = id;
// strncpy without terminator into the fixed 32-byte name field (name_end is cleared after), as the original does.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
        char* const r = std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::Facility>(this)->name)), joan::orig::arg::str(name), 0x20u);  // strncpy
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
        jo::self<jo::Facility>(this)->name_end = 0;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Entry::set", 0x0046d5e0, &H::entry_set);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b105_03.cpp (game/b105_03)
namespace {
namespace game_b105_03 {

struct H {
    void stub8(u32, u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_03", name, addr, fn)
// empty stubs
B("Obj::stub_46d800", 0x0046d800, &H::stub8);
#undef B

}  // namespace game_b105_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_pi_a39158() -> void { joan::bind::free<&::game_b98_01::init_pi_a39158>(); }  // 0x46cbd0
auto joan::orig::init_a3915c() -> void { joan::bind::free<&::game_b100_02::init_a3915c>(); }  // 0x46cc30
auto joan::orig::init_a39160() -> void { joan::bind::free<&::game_b94_01::init_a39160>(); }  // 0x46cc80
auto joan::orig::init_a39164() -> void { joan::bind::free<&::game_b96_01::init_a39164>(); }  // 0x46ccd0
auto joan::orig::List::destroy_46cdc0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::destroy_46cdc0>(this)); }  // 0x46cdc0
auto joan::orig::FacilityList::create(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b87_04::H::create>(this, a0)); }  // 0x46ce70
auto joan::orig::FacilityList::new_facility(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b88_03::H::create>(this, a0)); }  // 0x46cef0
auto joan::orig::FacilityList::find_by_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_02::H::find_name>(this, a0)); }  // 0x46d200
auto joan::orig::Facility::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_02::H::record_ctor>(this)); }  // 0x46d520
auto joan::orig::Facility::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_01::H::dtor_46d5b0>(this)); }  // 0x46d5b0
auto joan::orig::Facility::set_kind_name(std::int16_t a0, const char* a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b91_02::H::entry_set>(this, a0, a1)); }  // 0x46d5e0
auto joan::orig::Facility::detach() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_01::H::notify_owner>(this)); }  // 0x46d630
auto joan::orig::Facility::get_position(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_01::H::get_29>(this, a0)); }  // 0x46d670
auto joan::orig::Facility::set_angle(std::uint32_t a0) -> void { joan::bind::member<&::game_b87_04::H::set_angle>(this, a0); }  // 0x46d720
auto joan::orig::Facility::add_entrance(void* a0) -> void { joan::bind::member<&::game_b87_04::H::add_entrance>(this, a0); }  // 0x46d750
auto joan::orig::facility_kind(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b87_04::facility_kind>(a0)); }  // 0x46d820
auto joan::orig::Facility::is_free() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_01::H::is_free>(this)); }  // 0x46da90
