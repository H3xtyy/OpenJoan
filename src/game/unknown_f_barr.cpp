// game/unknown_f_barr.cpp: functions whose original source file is unknown (formerly game/_unplaced/f_barr.cpp;
// re/attribution.csv: between ww/end_con.cpp and ww/f_shop.cpp). They move to their original file when evidence names
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
#include "../runtime/replay/replay.hpp"
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

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::orig::helpers::addr;  // the slot's result type is pointer-sized (spawn_467590)

using joan::call::cdecl_call;

using u8 = std::uint8_t;
using i16 = std::int16_t;

using joan::hook::global;

using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

using joan::call::thiscall;

using joan::orig::helpers::memset_;

struct H {
    void* self() { return static_cast<void*>(this); }

    void* ctor_base(img::Addr vtable, std::uint32_t n) {
        joan::orig::as_facility(self())->construct();  // 0x0046d520
        jo::self<jo::Building>(this)->vtable = img::obj(vtable);
        memset_(&jo::self<jo::Building>(this)->unit_id, 0, n);
        return self();
    }
    void* ctor_467420() { return ctor_base(d::vt_Obj_9d0780, static_cast<std::uint32_t>(sizeof(jo::Building) - sizeof(jo::Facility))); }  // 0xc7 on x86
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Obj::ctor_467420", 0x00467420, &H::ctor_467420);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

using joan::call::thiscall;

using joan::orig::helpers::op_delete;

struct H {
    void* scalar_dtor_467470(u32 flags) {
        joan::orig::as_barracks_facility(this)->destruct();  // 0x004674c0
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }

    // ------------------------------------------------------------------ 0x00467530
    std::uintptr_t call_46d5e0_467530(u32 a, const char* b) { return joan::orig::arg::as<u32>(joan::orig::as_facility(this)->set_kind_name(joan::orig::arg::i16(a), b)); }  // 0x0046d5e0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::scalar_dtor_467470", 0x00467470, &H::scalar_dtor_467470);
B("Obj::call_46d5e0_467530", 0x00467530, &H::call_46d5e0_467530);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
namespace {
namespace game_b100_02 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x004674c0
    u32 dtor_4674c0() {
        jo::self<jo::Building>(this)->vtable = img::obj(d::vt_Obj_9d0780);
        joan::orig::as_barracks_facility(this)->detach();             // 0x00467560
        return joan::orig::arg::as<u32>(joan::orig::as_facility(this)->destruct());  // 0x0046d5b0
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("Obj::dtor_4674c0", 0x004674c0, &H::dtor_4674c0);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

using joan::call::thiscall;

struct H {
    std::uintptr_t dtor_467560() { return joan::orig::arg::as<u32>(joan::orig::as_facility(this)->detach()); }  // 0x0046d630
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Obj::dtor_467560", 0x00467560, &H::dtor_467560);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b104_02.cpp (game/b104_02)
namespace {
namespace game_b104_02 {

using joan::call::thiscall;

using u16 = std::uint16_t;

using joan::img::obj;

using joan::orig::helpers::op_new;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");
struct alignas(4) Order {  // 0x004e4520 record, [ebp-0x28] .. [ebp-0xc]
    u8 b[0x1c];
};

void* const kUnits = obj(d::g_units);
void* const kKinds = obj(d::g_types);

using joan::orig::helpers::memcpy_;
char* strcpy_(void* d, const void* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
i32 word_arg(u16 w) { return static_cast<i32>(static_cast<i16>(w)); }

// common thunks
i32 unit_dead(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_ptr(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* kind_of(u16 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(word_arg(w)))); }  // 0x0042d0a0

JOAN_COUNTER(c_spawn, "Building::spawn_467590");

struct H {
    // ------------------------------------------------------------------ 0x00467590
    void attach_helper(u8* u) {
        if (jo::as_unit(u)->orders == nullptr) {
            void* const mem = op_new(static_cast<std::uint32_t>(sizeof(jo::Order)));  // 0x3e on x86
            void* const h = mem != nullptr ? joan::orig::as_order(mem)->construct() : nullptr;  // 0x004bab50
            jo::as_unit(u)->orders = static_cast<jo::Order*>(h);
        }
        joan::orig::as_order(jo::as_unit(u)->orders)->add(u, joan::orig::arg::i32(jo::self<jo::Building>(this)->order_arg), joan::orig::arg::i32(static_cast<u32>(jo::self<jo::Building>(this)->order_b)), joan::orig::arg::i32(jo::self<jo::Building>(this)->order_c), joan::orig::arg::i32(jo::self<jo::Building>(this)->order_d), joan::orig::arg::i32(jo::self<jo::Building>(this)->order_e), joan::orig::arg::i32(jo::self<jo::Building>(this)->order_f), joan::orig::arg::i32(global<u32>(d::g_now) + jo::self<jo::Building>(this)->delay), 0, 0);  // 0x004bac60
    }
    void finish_unit(u8* u, u16 level) {
        joan::orig::as_unit(u)->set_level_523190(joan::orig::arg::i32(word_arg(level)));  // 0x00523190
        if (jo::as_unit(u)->rank < 4) joan::orig::as_unit(u)->distribute_levels();  // 0x004a95a0
    }
    std::uintptr_t spawn_467590() {
        c_spawn.hit();
        if (jo::self<jo::Building>(this)->unit_kind == 0 && jo::self<jo::Building>(this)->queue_n == 0) return 0;
        if (jo::self<jo::Building>(this)->unit_kind != 0) {
            if (jo::self<jo::Building>(this)->unit_id == 0) {
                if (joan::orig::arg::as<i32>(joan::orig::as_facility(this)->is_free()) == 0) return 0;  // 0x0046da90
                const i32 id = joan::orig::arg::as<i32>(joan::orig::as_units(kUnits)->create_51dfc0(static_cast<i32>(jo::self<jo::Building>(this)->unit_kind)));  // 0x0051dfc0
                u8* const u = unit_ptr(id);
                joan::orig::as_unit(u)->set_level_state(static_cast<i32>(jo::self<jo::UnitType>(kind_of(jo::view<u16>(jo::self<jo::Building>(this)->unit_kind)))->rank));  // 0x005089e0
                jo::view<u16>(jo::as_unit(u)->layer) = 1;
                joan::orig::as_unit(u)->set_group(global<jo::World*>(d::g_world));  // 0x004fed00
                strcpy_(jo::as_unit(u)->name_b, &jo::self<jo::Building>(this)->text[0x40]);
                strcpy_(jo::as_unit(u)->name_, jo::self<jo::Building>(this)->text);
                joan::orig::as_unit(u)->set_side(joan::orig::arg::i32(word_arg(jo::view<u16[2]>(jo::self<jo::Building>(this)->owner->nation)[0])));  // 0x00509250
                Vec3 pos;                                                             // [ebp-0x24]
                joan::orig::as_barracks_facility(this)->centre(&pos);                              // 0x00467fd0
                joan::orig::as_ground_object(u)->set_position(pos);                                    // 0x004fedf0
                joan::orig::as_unit(u)->set_target(joan::orig::arg::f32(jo::view<u32>(jo::self<jo::Building>(this)->angle)));                           // 0x004ff870
                joan::orig::as_unit(u)->set_heading(joan::orig::arg::f32(jo::self<jo::Building>(this)->angle / global<float>(d::flt_0_017453292)));  // 0x0050abd0
                jo::view<u16>(jo::as_unit(u)->group_id) = jo::view<u16>(jo::self<jo::Building>(this)->unit_level);
                if (jo::self<jo::UnitType>(kind_of(jo::view<u16>(jo::self<jo::Building>(this)->unit_kind)))->rank >= 3) {
                    joan::orig::as_unit(u)->sync_shadow();  // 0x00447600
                    for (i32 i = joan::orig::as_dynarray(kUnits)->size(); i >= 1; --i) {  // 0x00417ad0
                        u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
                        if (o == nullptr) continue;
                        if (jo::as_unit(o)->group_id == jo::as_unit(u)->group_id && joan::orig::as_unit(o)->leader() == nullptr) {  // 0x004216d0
                            joan::orig::as_unit(o)->link_to(static_cast<i32>(jo::as_unit(u)->id));  // 0x00438f80
                        }
                    }
                }
                if (joan::orig::arg::as<i32>(joan::orig::as_scenario(obj(d::g_mission))->mode()) == 2) jo::view<u32>(jo::as_unit(u)->formation_slot) = 2;  // 0x004cd510
                if (jo::self<jo::Building>(this)->order_arg != 0) attach_helper(u);
                jo::view<u16>(jo::self<jo::Building>(this)->unit_id) = static_cast<u16>(id);
                finish_unit(u, jo::view<u16>(jo::self<jo::Building>(this)->i16_7d));
                const u32 v = joan::orig::arg::as<u32>(joan::orig::as_unit(u)->hit_points_max());    // 0x00523320
                return joan::orig::arg::as<u32>(joan::orig::as_unit(u)->set_value_135(joan::orig::arg::i32(v)));        // 0x004fe510
            }
            if (unit_dead(jo::self<jo::Building>(this)->unit_id) != 0) return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->clear_79());  // 0x00467f90
        }
        // 0x004678d9
        if (jo::self<jo::Building>(this)->queue_n == 0) return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->clear_79());  // 0x00467f90
        if (jo::self<jo::Building>(this)->queue_count[0] != 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_facility(this)->is_free()) == 0) return 0;  // 0x0046da90
            const i32 id = joan::orig::arg::as<i32>(joan::orig::as_units(kUnits)->create_51dfc0(static_cast<i32>(jo::self<jo::Building>(this)->queue_kind[0])));  // 0x0051dfc0
            u8* const u = unit_ptr(id);
            joan::orig::as_unit(u)->set_level_state(static_cast<i32>(jo::self<jo::UnitType>(kind_of(jo::view<u16>(jo::self<jo::Building>(this)->queue_kind[0])))->rank));  // 0x005089e0
            jo::view<u16>(jo::as_unit(u)->layer) = 1;
            joan::orig::as_unit(u)->set_group(global<jo::World*>(d::g_world));                 // 0x004fed00
            joan::orig::as_unit(u)->set_side(joan::orig::arg::i32(word_arg(jo::view<u16[2]>(jo::self<jo::Building>(this)->owner->nation)[0])));  // 0x00509250
            jo::view<u16>(jo::as_unit(u)->group_id) = jo::view<u16>(jo::self<jo::Building>(this)->unit_level);
            const u8* const leader = joan::orig::arg::as<u8*>(joan::orig::as_list(kUnits)->find_by_31(static_cast<i32>(jo::as_unit(u)->group_id)));  // 0x0051fe60
            if (leader != nullptr) joan::orig::as_unit(u)->link_to(static_cast<i32>(jo::as_unit(leader)->id));  // 0x00438f80
            Vec3 pos;                                                                 // [ebp-0x44]
            joan::orig::as_barracks_facility(this)->centre(&pos);                                  // 0x00467fd0
            joan::orig::as_ground_object(u)->set_position(pos);                                        // 0x004fedf0
            joan::orig::as_unit(u)->set_target(joan::orig::arg::f32(jo::view<u32>(jo::self<jo::Building>(this)->angle)));                               // 0x004ff870
            joan::orig::as_unit(u)->set_heading(joan::orig::arg::f32(jo::self<jo::Building>(this)->angle / global<float>(d::flt_0_017453292)));  // 0x0050abd0
            if (jo::self<jo::Building>(this)->order_arg != 0) attach_helper(u);
            if (unit_dead(jo::self<jo::Building>(this)->target) == 0) {
                Order rec;  // [ebp-0x60]
                joan::orig::as_unit_task(&rec)->set(0xau, static_cast<std::uint8_t>(joan::orig::arg::u32(5)), joan::orig::arg::u32(jo::self<jo::Building>(this)->target), 0u, 0u, 0u);  // 0x004e4520
                joan::orig::as_unit(u)->set_task(&rec);                                // 0x00527fb0
            }
            finish_unit(u, jo::view<u16>(jo::self<jo::Building>(this)->queue_d[0]));
            const u32 v = joan::orig::arg::as<u32>(joan::orig::as_unit(u)->hit_points_max());  // 0x00523320
            joan::orig::as_unit(u)->set_value_135(joan::orig::arg::i32(v));             // 0x004fe510
            if (joan::orig::arg::as<i32>(joan::orig::as_scenario(obj(d::g_mission))->mode()) == 2) jo::view<u32>(jo::as_unit(u)->formation_slot) = 2;  // 0x004cd510
            jo::view<u16>(jo::self<jo::Building>(this)->queue_count[0]) = static_cast<u16>(jo::view<u16>(jo::self<jo::Building>(this)->queue_count[0]) - 1);
            return addr(this);
        }
        if (jo::self<jo::Building>(this)->queue_n > 1) {
            jo::view<u16>(jo::self<jo::Building>(this)->queue_n) = static_cast<u16>(jo::view<u16>(jo::self<jo::Building>(this)->queue_n) - 1);
            memcpy_(&jo::self<jo::Building>(this)->queue_count[0], &jo::self<jo::Building>(this)->queue_count[1], static_cast<u32>(static_cast<i32>(jo::self<jo::Building>(this)->queue_n) << 1));
            return addr(memcpy_(&jo::self<jo::Building>(this)->queue_kind[0], &jo::self<jo::Building>(this)->queue_kind[1], static_cast<u32>(static_cast<i32>(jo::self<jo::Building>(this)->queue_n) << 1)));
        }
        return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->clear_79());  // 0x00467f90
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_02", name, addr, fn)
B("Building::spawn_467590", 0x00467590, &H::spawn_467590);
#undef B

}  // namespace game_b104_02
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::call::thiscall;

using joan::img::obj;

using joan::call::thiscall;
using joan::call::import_call;
using joan::img::obj;

void* const kUnits = obj(d::g_units);
void* const kObjects = obj(d::g_items);

// Unit table helpers (0x00428b70 = "id invalid", 0x0041b3c0 = unit by id).
std::int32_t unit_invalid(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }   // 0x0041b3c0

struct V3 {  // 12-byte vector moved by dwords
    std::uint32_t d[3];
};
static_assert(sizeof(V3) == 12, "v3");

std::uint32_t bits_of(float v) {
    std::uint32_t b;
    std::memcpy(&b, &v, 4);
    return b;
}
float float_of(std::uint32_t b) {
    float v;
    std::memcpy(&v, &b, 4);
    return v;
}

JOAN_COUNTER(c_nearest, "Unit::nearest_building");

struct H {
    // 0x00467c30
    std::int32_t nearest_building(const char* kind, std::int32_t range) {
        c_nearest.hit();
        void* const list = obj(d::g_orders);
        std::uint32_t best = 0xbf800000u;  // -1.0f
        std::int32_t result = 0;
        for (std::int32_t i = joan::orig::as_dynarray(list)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(list)->empty_at_f(joan::orig::arg::i32(i))) != 0) continue;  // 0x00485760
            std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(list)->get_f(joan::orig::arg::i32(i)));  // 0x00485730
            if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Missile>(e)->object_id))) != 0) continue;  // 0x00425d50
            if (unit_invalid(jo::view<std::int32_t>(jo::self<jo::Missile>(e)->shooter_unit_id)) != 0) continue;
            if (jo::self<jo::Unit>(this)->target_building != 0 && jo::view<std::int32_t>(jo::self<jo::Missile>(e)->shooter_unit_id) != static_cast<std::int32_t>(jo::self<jo::Unit>(this)->target_building))
                continue;
            std::uint8_t* const d = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Missile>(e)->object_id))));  // 0x00425d20
            std::uint8_t* const u = unit_get(jo::view<std::int32_t>(jo::self<jo::Missile>(e)->shooter_unit_id));
            if (*kind == 0x45 && joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->hostile_to(joan::orig::arg::i16(jo::as_unit(u)->id))) == 0)  // 0x005094d0
                continue;
            V3 tmp;
            const V3 diff = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, static_cast<void*>(jo::self<jo::Unit>(this)->position_), static_cast<void*>(jo::as_map_object(d)->pos)));  // 0x0060cc90
            const float l = v3d_float(d::iat_v3d_87_length_sq, &diff);  // length_sq
            const std::int32_t rr = static_cast<std::int32_t>(static_cast<std::uint32_t>(range) * static_cast<std::uint32_t>(range));
            if (!(static_cast<double>(rr) >= static_cast<double>(l))) continue;  // test ah,1 / jne
            if (best == 0xbf800000u) {
                best = bits_of(l);
                result = jo::as_map_object(d)->index;
            }
            if (float_of(best) > l) {  // test ah,0x41 / jne skips
                best = bits_of(l);
                result = jo::as_map_object(d)->index;
            }
        }
        return result;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Unit::nearest_building", 0x00467c30, &H::nearest_building);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

JOAN_COUNTER(c_set_texts, "Obj::set_texts");

struct H {
    // 0x00467dd0
    void set_texts(i16 a, i16 b, i16 c, const char* s1, const char* s2) {
        c_set_texts.hit();
        if (static_cast<i32>(jo::self<jo::Building>(this)->unit_kind) != 0) return;
        jo::self<jo::Building>(this)->unit_kind = a;
        jo::self<jo::Building>(this)->unit_level = b;
        jo::self<jo::Building>(this)->unit_id = 0;
        jo::self<jo::Building>(this)->i16_7d = c;
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::Building>(this)->text)), joan::orig::arg::str(s1), 0x40u);  // strncpy
        // strncpy without a terminator into the fixed-size field, as the original does
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(&jo::self<jo::Building>(this)->text[0x40])), joan::orig::arg::str(s2), 0x40u);
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
        if (jo::view<void*>(jo::self<jo::Building>(this)->owner) != nullptr) jo::self<jo::Building>(this)->owner->spawn_count = jo::self<jo::Building>(this)->owner->spawn_count + 1;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::set_texts", 0x00467dd0, &H::set_texts);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
namespace {
namespace game_b92_02 {

struct H {
    // 0x00467ea0
    void push_order(std::int32_t a, std::int32_t b, std::int32_t c, std::int32_t d) {
        if (jo::self<jo::Building>(this)->queue_n >= 4) return;
        jo::view<std::uint16_t>(jo::self<jo::Building>(this)->unit_level) = static_cast<std::uint16_t>(b);
        jo::view<std::uint16_t>(jo::self<jo::Building>(this)->queue_kind[jo::self<jo::Building>(this)->queue_n]) = static_cast<std::uint16_t>(a);
        jo::view<std::uint16_t>(jo::self<jo::Building>(this)->queue_count[jo::self<jo::Building>(this)->queue_n]) = static_cast<std::uint16_t>(c);
        jo::view<std::uint16_t>(jo::self<jo::Building>(this)->queue_d[jo::self<jo::Building>(this)->queue_n]) = static_cast<std::uint16_t>(d);
        jo::view<std::uint16_t>(jo::self<jo::Building>(this)->queue_n) = static_cast<std::uint16_t>(jo::view<std::uint16_t>(jo::self<jo::Building>(this)->queue_n) + 1);
        if (jo::self<jo::Building>(this)->owner != nullptr) {
            void* const o = jo::view<void*>(jo::self<jo::Building>(this)->owner);
            jo::view<std::uint32_t>(jo::as_place(o)->spawn_count) = jo::view<std::uint32_t>(jo::as_place(o)->spawn_count) + static_cast<std::uint32_t>(c);
        }
    }

    // 0x00467f90
    void* clear_79() { return std::memset(static_cast<void*>(&jo::self<jo::Building>(this)->unit_id), 0, 0xc3u); }  // memset unit_id .. target (no pointers: 0xc3 on x64 too)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Unit::push_order", 0x00467ea0, &H::push_order);
B("Unit::clear_79", 0x00467f90, &H::clear_79);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using joan::call::thiscall;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;

JOAN_COUNTER(c_centre, "Area::centre");

struct H {
    // ------------------------------------------------------------------ 0x00467fd0
    void centre(float* out) {
        c_centre.hit();
        out[0] = (jo::self<jo::Area>(this)->bounds[0] + jo::self<jo::Area>(this)->bounds[3]) / global<float>(d::flt_2);
        out[1] = (jo::self<jo::Area>(this)->bounds[1] + jo::self<jo::Area>(this)->bounds[4]) / global<float>(d::flt_2);
        u8* const o = jo::view<u8*>(jo::self<jo::Area>(this)->owner->cell);
        u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(o)->grid(joan::orig::arg::i16(0)));  // 0x005600c0
        if (m != nullptr) {
            const float s = jo::self<jo::Area>(this)->pos[2] + global<float>(d::flt_888) + global<float>(d::flt_64);
            if (s < jo::self<jo::Area>(this)->bounds[5] || s > jo::self<jo::Area>(this)->bounds[5]) {  // not equal, not NaN
                const Fpu fpu{true};
                const float x = static_cast<float>(fpu.div(fpu.sub(static_cast<double>(out[0]), static_cast<double>(jo::as_world_cell(o)->origin_x)),
                                                           static_cast<double>(jo::as_path_space(m)->cell_w)));
                const float y = static_cast<float>(fpu.div(fpu.sub(static_cast<double>(out[1]), static_cast<double>(jo::as_world_cell(o)->origin_y)),
                                                           static_cast<double>(jo::as_path_space(m)->cell_h)));
                const i32 ix = joan::hook::x87::ftol(static_cast<double>(x));
                const i32 iy = joan::hook::x87::ftol(static_cast<double>(y));
                const i32 sum = static_cast<i32>(static_cast<u32>(ix) + static_cast<u32>(iy));
                const i32 even = (sum % 2 == 0) ? 1 : 0;
                out[2] = joan::orig::arg::as<float>(joan::orig::as_terrain(jo::view<void*>(jo::as_path_space(m)->grid))->height_4c7b70(joan::orig::arg::f32(x), joan::orig::arg::f32(y), static_cast<char>(joan::orig::arg::i32(even))));  // 0x004c7b70
                return;
            }
        }
        jo::view<u32>(out[2]) = jo::view<u32>(jo::self<jo::Area>(this)->pos[2]);
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Area::centre", 0x00467fd0, &H::centre);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {

struct H {
    void* self() { return this; }
    u32 zero() { return 0; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("Obj::self_4682e0", 0x004682e0, &H::self);
B("Obj::zero_468120", 0x00468120, &H::zero);
B("Obj::zero_468140", 0x00468140, &H::zero);
B("Obj::zero_468160", 0x00468160, &H::zero);
B("Obj::zero_468180", 0x00468180, &H::zero);
B("Obj::zero_4681a0", 0x004681a0, &H::zero);
B("Obj::zero_4681c0", 0x004681c0, &H::zero);
B("Obj::zero_4681e0", 0x004681e0, &H::zero);
B("Obj::zero_468200", 0x00468200, &H::zero);
B("Obj::zero_468220", 0x00468220, &H::zero);
B("Obj::zero_468240", 0x00468240, &H::zero);
B("Obj::zero_468260", 0x00468260, &H::zero);
B("Obj::zero_468280", 0x00468280, &H::zero);
B("Obj::zero_4682a0", 0x004682a0, &H::zero);
B("Obj::zero_4682c0", 0x004682c0, &H::zero);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::BarracksFacility::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::ctor_467420>(this)); }  // 0x467420
auto joan::orig::BarracksFacility::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_02::H::dtor_4674c0>(this)); }  // 0x4674c0
auto joan::orig::BarracksFacility::detach() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_01::H::dtor_467560>(this)); }  // 0x467560
auto joan::orig::Unit::nearest_building(const char* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::nearest_building>(this, a0, a1)); }  // 0x467c30
auto joan::orig::BarracksFacility::set_texts(std::int16_t a0, std::int16_t a1, std::int16_t a2, const char* a3, const char* a4) -> void { joan::bind::member<&::game_b94_01::H::set_texts>(this, a0, a1, a2, a3, a4); }  // 0x467dd0
auto joan::orig::Unit::push_order(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> void { joan::bind::member<&::game_b92_02::H::push_order>(this, a0, a1, a2, a3); }  // 0x467ea0
auto joan::orig::Unit::clear_79() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_02::H::clear_79>(this)); }  // 0x467f90
auto joan::orig::BarracksFacility::centre(void* a0) -> void { joan::bind::member<&::game_b96_01::H::centre>(this, a0); }  // 0x467fd0
