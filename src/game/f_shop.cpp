// game/f_shop.cpp: functions of the original d:\ww\f_shop.cpp (re/attribution.csv).
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
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include <cstdarg>
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b101_02.cpp (game/b101_02)
//
//      0x004699f0 Shop::refresh_stock(id)           (thiscall, ret 4) - keeps old counts,
//                 round(count / 2) restock (0x00590a20), capped.
//
//  - 0x004699f0: fild dword / fdiv float 2.0 at PC24 -> Fpu{true}.div, passed as a qword to
//    0x00590a20. The early exit (record without list) leaks the copied array like the original.
//
//  - 0x0047fb30 / 0x0047ff50 / 0x00468790 / 0x004699f0: SEH frames only guard new + ctor;
//    normal path reproduced. Objects leaked on load failure, as in the original.
namespace {
namespace game_b101_02 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::orig::helpers::u;
i32 sw(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }  // word argument

void* const kHeap = obj(d::g_mem);
void* const kItemTypes = obj(d::g_item_res);

JOAN_COUNTER(c_shop_refresh, "Shop::refresh_stock");

using joan::orig::helpers::game_new;  // operator new
i32 list_size(void* l) { return joan::orig::as_dynarray(l)->size(); }  // 0x00417ad0
u8* list_at(void* l, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(l)->get(joan::orig::arg::i32(i))); }  // 0x00417200
void list_add(void* l, void* e) { joan::orig::dynarray(l)->linkin(joan::orig::arg::ptr(e)); }  // 0x00584620
u32 heap_free(void* p, const char* file, i32 line) {
    return thiscall<u32>(fn::del, kHeap, p, file, line);  // 0x0058ed70
}
// Scalar deleting destructor 0x00463fc0 (flags 1) when non-null.
void delete_463fc0(void* p) {
    if (p != nullptr) joan::orig::as_dynarray(p)->destroy(1u);  // 0x00463fc0
}
// new(0x1d) + 0x005843d0(elem, 0x1e, 0) or null.
void* new_list(i32 elem) {
    void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
    return mem != nullptr ? joan::orig::dynarray(mem)->construct(joan::orig::arg::i32(elem), 0x1e, 0) : nullptr;  // 0x005843d0
}

struct H {
    // ------------------------------------------------------------------ 0x004699f0
    std::uintptr_t shop_refresh(i32 id) {
        c_shop_refresh.hit();
        const i32 n = list_size(jo::view<void*>(jo::self<jo::Shop>(this)->stock));
        jo::ShopStock* const old = joan::orig::arg::as<jo::ShopStock*>(joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(n) * sizeof(jo::ShopStock))), str(d::s_d_ww_f_shop_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fddcc)) + 6)));  // 0x0058e7b0
        for (i32 i = 1; i <= n; ++i) {
            const jo::ShopStock* const e = jo::as_shop_stock(list_at(jo::view<void*>(jo::self<jo::Shop>(this)->stock), i));
            jo::ShopStock* const o = old + (i - 1);
            o->id = e->id;
            o->u32_a = e->u32_a;
            jo::view<u32>(o->count) = jo::view<u32>(e->count);
            o->u32_2 = e->u32_2;
        }
        delete_463fc0(jo::view<void*>(jo::self<jo::Shop>(this)->stock));
        jo::view<void*>(jo::self<jo::Shop>(this)->stock) = nullptr;
        jo::view<void*>(jo::self<jo::Shop>(this)->stock) = new_list(0xe);
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_sell_item_res(obj(d::g_sell_item_res))->entry_at(joan::orig::arg::i32(sw(id))));  // 0x00469e60
        if (jo::view<void*>(jo::as_sell_item_entry(rec)->rows) == nullptr) return u(rec);
        for (i32 i = 1; i <= list_size(jo::view<void*>(jo::as_sell_item_entry(rec)->rows)); ++i) {
            jo::ShopStock tmp;  // [ebp-0x34]
            std::memset(&tmp, 0, sizeof(tmp));
            const jo::ShopStock* const e = jo::as_shop_stock(list_at(jo::view<void*>(jo::as_sell_item_entry(rec)->rows), i));
            u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItemTypes)->get(joan::orig::arg::i16(sw(jo::view<i16>(e->id)))));  // 0x00425ce0
            if (static_cast<i32>(jo::view<i8>(jo::as_item_def(type)->unique_item)) != 0 &&
                joan::orig::arg::as<u8>(joan::orig::as_game(obj(d::g_game_state))->has_word(joan::orig::arg::i16(sw(jo::view<i16>(e->id))))) != 0)  // 0x00472740
                continue;
            tmp.id = e->id;
            tmp.u32_2 = e->u32_2;
            if (e->count != -1) {
                const joan::hook::x87::Fpu fpu{true};
                const double q = fpu.div(static_cast<double>(e->count),
                                         static_cast<double>(global<float>(d::flt_2)));  // 2.0
                tmp.count = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(q)));  // 0x00590a20
                for (i32 j = 0; j < n; ++j) {
                    const jo::ShopStock* const o = old + j;
                    if (static_cast<i32>(jo::view<i16>(o->id)) != static_cast<i32>(jo::view<i16>(tmp.id))) continue;
                    tmp.count = static_cast<i32>(static_cast<u32>(tmp.count) + jo::view<u32>(o->count));
                    if (tmp.count > e->count) tmp.count = e->count;
                    break;
                }
            } else {
                tmp.count = e->count;
            }
            tmp.u32_a = e->u32_a;
            list_add(jo::view<void*>(jo::self<jo::Shop>(this)->stock), &tmp);
        }
        jo::view<u8>(jo::self<jo::Shop>(this)->fixed_stock) = jo::as_sell_item_entry(rec)->type;
        jo::self<jo::Shop>(this)->stock_kind = jo::view<i32>(jo::as_sell_item_entry(rec)->value);
        jo::self<jo::Shop>(this)->shop_id = id;
        return heap_free(old, str(d::s_d_ww_f_shop_cpp), static_cast<i32>(global<i16>(d::g_9fddcc)) + 0x44);
    }
};

#define B101(name, addr, fn) JOAN_REPLACE("game/b101_02", name, addr, fn)
B101("Shop::refresh_stock", 0x004699f0, &H::shop_refresh);
#undef B101

}  // namespace game_b101_02
}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
//
//      0x00469710 Shop::tick_469710                (thiscall, ret 0) - vtable 0x9d0908; returns this.
namespace {
namespace game_b102_02 {

using joan::hook::global;
using i8 = std::int8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
JOAN_COUNTER(c_shop_tick, "Shop::tick_469710");

struct H {
    // 0x00469710
    void* shop_tick() {
        c_shop_tick.hit();
        if (jo::self<jo::Shop>(this)->i32_81 == 0 && jo::self<jo::Shop>(this)->fixed_stock != 0) return this;
        jo::self<jo::Shop>(this)->restock_timer = jo::self<jo::Shop>(this)->restock_timer + global<u32>(d::g_frame_step_ms);
        if (jo::view<i32>(jo::self<jo::Shop>(this)->restock_timer) < 0x36ee80) return this;
        if (jo::self<jo::Shop>(this)->shop_id > 0) joan::orig::as_shop(this)->refresh_stock(joan::orig::arg::i32(jo::self<jo::Shop>(this)->shop_id));  // 0x004699f0
        jo::self<jo::Shop>(this)->restock_timer = jo::self<jo::Shop>(this)->restock_timer - 0x36ee80u;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("Shop::tick_469710", 0x00469710, &H::shop_tick);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
//
//      0x00469e60 Table1c::slot(short i)     returns +4 + i*0x1c - 0x1c; ret 4
namespace {
namespace game_b92_02 {

struct H {
    // 0x00469e60
    void* slot1c(std::int32_t i) {
        return jo::self<jo::SItemRes>(this)->entries + static_cast<std::int16_t>(i) - 1;  // 0x1c-byte records on x86
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Table1c::slot", 0x00469e60, &H::slot1c);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// xc-table entries (0x9f41e4..0x9f41f0): _initterm ignores the return value (EAX garbage in the original).
u32 JOAN_CDECL init_4693d0() { joan::orig::init_a39144(); return 0; }
u32 JOAN_CDECL init_469430() { joan::orig::init_a39148(); return 0; }
u32 JOAN_CDECL init_469480() { joan::orig::init_a3914c(); return 0; }
u32 JOAN_CDECL init_4694d0() { joan::orig::init_a39150(); return 0; }

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("init_4693d0", 0x004693d0, &init_4693d0);
B("init_469430", 0x00469430, &init_469430);
B("init_469480", 0x00469480, &init_469480);
B("init_4694d0", 0x004694d0, &init_4694d0);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using joan::call::cdecl_call;

using joan::hook::global;

// ---------------------------------------------------------------------------- static inits
using joan::orig::helpers::pi_from_atan;
void JOAN_CDECL init_a39144() { global<float>(d::g_a39144) = pi_from_atan(); }

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("init_a39144", 0x004693f0, &init_a39144);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

using joan::hook::global;

using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a39148() { div_global(d::g_a39148, d::g_a39144, d::flt_2); }  // 0x00469450 (/2)

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("init_a39148", 0x00469450, &init_a39148);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
namespace {
namespace game_b100_02 {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::cdecl_call;
using joan::img::obj;

using joan::hook::global;

using i8 = std::int8_t;

using joan::orig::helpers::u;

using joan::orig::helpers::game_new;      // operator new
// Scalar deleting destructor 0x00463fc0 (flags 1) when non-null.
void delete_463fc0(void* p) {
    if (p != nullptr) joan::orig::as_dynarray(p)->destroy(1u);  // 0x00463fc0
}

void* const kItemTypes = obj(d::g_item_res);

JOAN_COUNTER(c_load_group, "Obj::load_group");

u8* item_type(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItemTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0
i32 inv_size(void* inv) { return joan::orig::as_dynarray(inv)->size(); }  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------ 0x004697c0
    std::uintptr_t load_group(i32 id) {
        c_load_group.hit();
        if (jo::view<void*>(jo::self<jo::Shop>(this)->stock) != nullptr) {
            delete_463fc0(jo::view<void*>(jo::self<jo::Shop>(this)->stock));
            jo::view<void*>(jo::self<jo::Shop>(this)->stock) = nullptr;
        }
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
        void* const list = mem != nullptr ? joan::orig::dynarray(mem)->construct(0xe, 0x1e, 0) : nullptr;  // 0x005843d0
        jo::view<void*>(jo::self<jo::Shop>(this)->stock) = list;
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_sell_item_res(obj(d::g_sell_item_res))->entry_at(static_cast<i32>(static_cast<i16>(id))));  // 0x00469e60
        if (jo::view<void*>(jo::as_sell_item_entry(rec)->rows) == nullptr) return u(rec);
        for (i32 i = 1; i <= inv_size(jo::view<void*>(jo::as_sell_item_entry(rec)->rows)); ++i) {
            u8 tmp[0xe];
            std::memset(tmp, 0, sizeof(tmp));
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::as_sell_item_entry(rec)->rows))->get(joan::orig::arg::i32(i)));  // 0x00417200
            u8* const type = item_type(static_cast<i32>(jo::view<i16>(jo::as_shop_stock(e)->id)));
            if (static_cast<i32>(jo::view<i8>(jo::as_item_def(type)->unique_item)) != 0 &&
                joan::orig::arg::as<u8>(joan::orig::as_game(obj(d::g_game_state))->has_word(joan::orig::arg::i16(static_cast<i32>(jo::view<i16>(jo::as_shop_stock(e)->id))))) != 0)  // 0x00472740
                continue;
            std::memcpy(&jo::as_shop_stock(tmp)->id, &jo::as_shop_stock(e)->id, 2);
            std::memcpy(&jo::as_shop_stock(tmp)->u32_2, &jo::as_shop_stock(e)->u32_2, 4);
            std::memcpy(&jo::as_shop_stock(tmp)->count, &jo::as_shop_stock(e)->count, 4);
            std::memcpy(&jo::as_shop_stock(tmp)->u32_a, &jo::as_shop_stock(e)->u32_a, 4);
            joan::orig::dynarray(jo::view<void*>(jo::self<jo::Shop>(this)->stock))->linkin(static_cast<void*>(tmp));  // 0x00584620
        }
        jo::view<u8>(jo::self<jo::Shop>(this)->fixed_stock) = jo::as_sell_item_entry(rec)->type;
        jo::self<jo::Shop>(this)->stock_kind = jo::view<i32>(jo::as_sell_item_entry(rec)->value);
        jo::self<jo::Shop>(this)->shop_id = id;
        return static_cast<u32>(jo::view<i32>(jo::as_sell_item_entry(rec)->value));
    }
};

static void JOAN_CDECL init_a3914c() { global<float>(d::g_a3914c) = global<float>(d::g_a39144) / global<float>(d::flt_3); }

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("init_a3914c", 0x004694a0, &init_a3914c);
B100("Obj::load_group", 0x004697c0, &H::load_group);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

using joan::hook::global;

// g_out = g_in / g_div (fld dword / fdiv dword / fstp dword).
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a39150() { div_global(d::g_a39150, d::g_a39144, d::flt_4); }  // 0x004694f0

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("init_a39150", 0x004694f0, &init_a39150);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    std::uint8_t* elem_469520(std::uint32_t i) {  // 0x00469520
        return reinterpret_cast<std::uint8_t*>(jo::self<jo::RecruitRes>(this)->entries + static_cast<std::int16_t>(i) - 1);  // 0xb4-byte records on x86
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("elem_00469520", 0x00469520, &H::elem_469520);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

using joan::call::thiscall;

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::orig::helpers::memset_;

struct H {
    void* self() { return static_cast<void*>(this); }

    void* ctor_base(img::Addr vtable, std::uint32_t n) {
        joan::orig::as_facility(self())->construct();  // 0x0046d520
        jo::self<jo::Shop>(this)->vtable = img::obj(vtable);
        memset_(&jo::self<jo::Shop>(this)->stock, 0, n);
        return self();
    }
    // The Shop members after the Facility base (0x15 bytes at +0x79 on x86).
    void* ctor_469560() { return ctor_base(d::vt_Obj_9d08f8, static_cast<std::uint32_t>(sizeof(jo::Shop) - sizeof(jo::Facility))); }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Obj::ctor_469560", 0x00469560, &H::ctor_469560);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b103_02.cpp (game/b103_02)
namespace {
namespace game_b103_02 {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;            // operator delete

struct H {
    // ------------------------------------------------------------------ scalar deleting dtors
    void* sdd_4695b0(u32 flags) {
        joan::orig::as_shop_facility(this)->destruct();  // 0x00469600
        if ((flags & 1u) != 0) game_delete(this);
        return this;
    }

    // ------------------------------------------------------------------ 0x00469670
    std::uintptr_t forward_46d5e0(u32 a, const char* b) {
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_facility(this)->set_kind_name(joan::orig::arg::i16(a), b));  // 0x0046d5e0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_02", name, addr, fn)
B("Obj::sdd_4695b0", 0x004695b0, &H::sdd_4695b0);
B("Obj::forward_46d5e0", 0x00469670, &H::forward_46d5e0);
#undef B

}  // namespace game_b103_02
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
namespace {
namespace game_b99_02 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x00469600
    u32 dtor_469600() {
        jo::self<jo::Shop>(this)->vtable = img::obj(d::vt_Obj_9d08f8);  // vtable
        joan::orig::as_shop_facility(this)->release();  // 0x004696a0
        return joan::orig::arg::as<u32>(joan::orig::as_facility(this)->destruct());  // 0x0046d5b0
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::~Obj_469600", 0x00469600, &H::dtor_469600);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x004696a0
    u32 release_79() {
        if (jo::self<jo::Shop>(this)->stock != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Shop>(this)->stock);
            if (p != nullptr) joan::orig::as_dynarray(p)->destroy(1u);  // 0x00463fc0 (scalar dtor)
            jo::self<jo::Shop>(this)->stock = nullptr;
        }
        joan::orig::as_facility(this)->detach();  // 0x0046d630
        return 0;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Obj::release_79", 0x004696a0, &H::release_79);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
namespace {
namespace game_leaves80b {

using joan::call::thiscall;

using i32 = std::int32_t;

using joan::hook::x87::Fpu;

using joan::orig::helpers::at;

JOAN_COUNTER(c_regions_find, "Regions::find");

struct H {
    // 0x00469d10: rect r (+4 left, +8 top, +0x10 right, +0x14 bottom) contains p within the
    // margin: x87 int (fild) +/- float, rounded to float at PC24, compared to the point.
    void* regions_find(const float* p, std::int32_t margin) {
        c_regions_find.hit();
        const joan::hook::x87::Fpu fpu{true};
        const double m = static_cast<double>(margin);
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0 count
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->empty_at_42d8d0(joan::orig::arg::i32(i))) != 0) continue;              // 0x0042d8d0
            std::uint8_t* const r = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i)));  // 0x00425d90 get
            // the region box (Water +4: x0, y0, z0, x1, y1, z1)
            if (fpu.sub(static_cast<double>(jo::as_water(r)->box[0]), m) > static_cast<double>(p[0])) continue;
            if (!(fpu.add(m, static_cast<double>(jo::as_water(r)->box[3])) >= static_cast<double>(p[0]))) continue;
            if (fpu.sub(static_cast<double>(jo::as_water(r)->box[1]), m) > static_cast<double>(p[1])) continue;
            if (!(fpu.add(m, static_cast<double>(jo::as_water(r)->box[4])) >= static_cast<double>(p[1]))) continue;
            return r;
        }
        return nullptr;
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("Regions::find", 0x00469d10, &H::regions_find);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

using i32 = std::int32_t;

struct H {
    // 0x00469dd0
    std::int32_t count_79() {
        if (jo::self<jo::Shop>(this)->stock == nullptr) return 0;
        return joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Shop>(this)->stock))->size();  // 0x00417ad0
    }

    // 0x00469e10
    std::uintptr_t at_79(std::int32_t i) {
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Shop>(this)->stock))->get(joan::orig::arg::i32(i)));  // 0x00417200
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::count_79", 0x00469dd0, &H::count_79);
M("Obj::at_79", 0x00469e10, &H::at_79);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b105_02_2.cpp (game/b105_02)
namespace {
namespace game_b105_02_2 {

struct H {
    void* self() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("Obj::self_469e40", 0x00469e40, &H::self);
#undef B

}  // namespace game_b105_02_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_a39144() -> void { joan::bind::free<&::game_b96_01::init_a39144>(); }  // 0x4693f0
auto joan::orig::init_a39148() -> void { joan::bind::free<&::game_b98_01::init_a39148>(); }  // 0x469450
auto joan::orig::init_a3914c() -> void { joan::bind::free<&::game_b100_02::init_a3914c>(); }  // 0x4694a0
auto joan::orig::init_a39150() -> void { joan::bind::free<&::game_b94_01::init_a39150>(); }  // 0x4694f0
auto joan::orig::RecruitRes::group_at(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_18::H::elem_469520>(this, a0)); }  // 0x469520
auto joan::orig::ShopFacility::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::ctor_469560>(this)); }  // 0x469560
auto joan::orig::ShopFacility::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::dtor_469600>(this)); }  // 0x469600
auto joan::orig::ShopFacility::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_01::H::release_79>(this)); }  // 0x4696a0
auto joan::orig::ShopFacility::load_stock(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_02::H::load_group>(this, a0)); }  // 0x4697c0
auto joan::orig::Shop::refresh_stock(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_02::H::shop_refresh>(this, a0)); }  // 0x4699f0
auto joan::orig::Waters::find_at(void* a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves80b::H::regions_find>(this, a0, a1)); }  // 0x469d10
auto joan::orig::ShopFacility::stock_count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_03::H::count_79>(this)); }  // 0x469dd0
auto joan::orig::ShopFacility::stock_at(std::int32_t a0) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b90_03::H::at_79>(this, a0)); }  // 0x469e10
auto joan::orig::SellItemRes::entry_at(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_02::H::slot1c>(this, a0)); }  // 0x469e60
