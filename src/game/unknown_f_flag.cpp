// game/unknown_f_flag.cpp: functions whose original source file is unknown (formerly game/_unplaced/f_flag.cpp;
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

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::orig::helpers::addr;  // eax residue as a full pointer (results of slots)

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using i16 = std::int16_t;

using joan::call::cdecl_call;
using joan::img::obj;

using joan::orig::helpers::u;

// Scalar deleting destructor 0x00463fc0 (flags 1) when non-null.
void delete_463fc0(void* p) {
    if (p != nullptr) joan::orig::as_dynarray(p)->destroy(1u);  // 0x00463fc0
}

using joan::orig::helpers::op_new;

using joan::hook::global;

using i8 = std::int8_t;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;

// ---------------------------------------------------------------- shared game helpers
i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

using joan::orig::helpers::memset_;

struct H {
    void* self() { return static_cast<void*>(this); }

    void* ctor_base(img::Addr vtable, std::uint32_t n) {
        joan::orig::as_facility(self())->construct();  // 0x0046d520
        jo::view<std::uintptr_t>(jo::self<jo::FlagFacility>(this)->vtable) = img::addr(vtable);
        memset_(&jo::self<jo::FlagFacility>(this)->list_79, 0, n);
        return self();
    }
    // The FlagFacility members after the Facility base (0x20 bytes at +0x79 on x86).
    void* ctor_468300() { return ctor_base(d::vt_Obj_9d07f8, static_cast<std::uint32_t>(sizeof(jo::FlagFacility) - sizeof(jo::Facility))); }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Obj::ctor_468300", 0x00468300, &H::ctor_468300);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

void* const kTypes = obj(d::g_item_res);

using joan::orig::helpers::op_delete;
i32 word_arg(u32 raw) { return static_cast<i32>(static_cast<i16>(raw)); }

JOAN_COUNTER(c_release_slots, "Unit::release_slots_4691f0");

struct H {
    void* scalar_dtor_468350(u32 flags) {
        joan::orig::as_recruit_facility(this)->destruct();  // 0x004683a0
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }

    // ------------------------------------------------------------------ 0x00468b30 / 0x00468ba0
    u32 count_79_468b30() {
        if (jo::self<jo::FlagFacility>(this)->list_79 == nullptr) return 0;
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_79))->size());  // 0x00417ad0
    }
    u32 count_7d_468ba0() {
        if (jo::self<jo::FlagFacility>(this)->list_7d == nullptr) return 0;
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_7d))->size());  // 0x00417ad0
    }
    // ------------------------------------------------------------------ 0x00468b70 / 0x00468be0
    std::uintptr_t get_79_468b70(u32 i) { return joan::orig::arg::as<u32>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_79))->get(joan::orig::arg::i32(i))); }  // 0x00417200
    std::uintptr_t get_7d_468be0(u32 i) { return joan::orig::arg::as<u32>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_7d))->get(joan::orig::arg::i32(i))); }  // 0x00417200
    // ------------------------------------------------------------------ 0x00468ca0
    void* set_98_468ca0(u32 v) {
        jo::self<jo::FlagFacility>(this)->flag_98 = static_cast<u8>(v);
        return this;
    }
    // ------------------------------------------------------------------ 0x004691f0
    void release_slot(jo::Actor*& slot, const i32& arg) {
        if (slot == nullptr) return;
        void* p = slot;
        if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840 (scalar deleting dtor)
        slot = nullptr;
        void* rec = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(word_arg(jo::view<u16>(jo::self<jo::Wall>(this)->type_id)))));  // 0x00425ce0
        joan::orig::as_res(rec)->release_slot(joan::orig::arg::i32(jo::view<u32>(arg)), 0);                    // 0x004a3e30
    }
    void* release_slots_4691f0() {
        c_release_slots.hit();
        joan::orig::as_unit(this)->release_419950();  // 0x00419950
        release_slot(jo::self<jo::Wall>(this)->actor_a, jo::self<jo::Wall>(this)->level_a);
        release_slot(jo::self<jo::Wall>(this)->actor_b, jo::self<jo::Wall>(this)->level_b);
        jo::self<jo::Wall>(this)->u16_e4 = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::scalar_dtor_468350", 0x00468350, &H::scalar_dtor_468350);
B("Obj::count_79_468b30", 0x00468b30, &H::count_79_468b30);
B("Obj::get_79_468b70", 0x00468b70, &H::get_79_468b70);
B("Obj::count_7d_468ba0", 0x00468ba0, &H::count_7d_468ba0);
B("Obj::get_7d_468be0", 0x00468be0, &H::get_7d_468be0);
B("Obj::set_98_468ca0", 0x00468ca0, &H::set_98_468ca0);
B("Unit::release_slots_4691f0", 0x004691f0, &H::release_slots_4691f0);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b101_02.cpp (game/b101_02)
namespace {
namespace game_b101_02 {

using joan::orig::helpers::game_new;      // operator new

i32 sw(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }  // word argument

JOAN_COUNTER(c_flag_groups, "Obj::load_flag_groups");

i32 list_size(void* l) { return joan::orig::as_dynarray(l)->size(); }  // 0x00417ad0
u8* list_at(void* l, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(l)->get(joan::orig::arg::i32(i))); }  // 0x00417200
void list_add(void* l, void* e) { joan::orig::dynarray(l)->linkin(joan::orig::arg::ptr(e)); }  // 0x00584620
// new(0x1d) + 0x005843d0(elem, 0x1e, 0) or null.
void* new_list(i32 elem) {
    void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
    return mem != nullptr ? joan::orig::dynarray(mem)->construct(joan::orig::arg::i32(elem), 0x1e, 0) : nullptr;  // 0x005843d0
}

struct H {
    // ------------------------------------------------------------------ 0x004683a0
    u32 dtor_4683a0() {
        jo::view<std::uintptr_t>(jo::self<jo::FlagFacility>(this)->vtable) = img::addr(d::vt_Obj_9d07f8);
        joan::orig::as_recruit_facility(this)->release();              // 0x004684e0
        return joan::orig::arg::as<u32>(joan::orig::as_facility(this)->destruct());  // 0x0046d5b0
    }

    // ------------------------------------------------------------------ 0x00468790
    std::uintptr_t load_flag_groups(i32 a, i32 b) {
        c_flag_groups.hit();
        u8 tmp1[0xa];  // [ebp-0x20]
        u8 tmp2[0xa];  // [ebp-0x34]
        if (jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_79) != nullptr) {
            delete_463fc0(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_79));
            jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_79) = nullptr;
        }
        jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_79) = new_list(0xa);
        u8* rec = joan::orig::arg::as<u8*>(joan::orig::as_recruit_res(obj(d::g_recruit_res))->group_at(joan::orig::arg::u32(sw(a))));  // 0x00469520
        if (jo::view<void*>(jo::as_recruit_entry(rec)->rows) == nullptr) return u(rec);
        jo::self<jo::FlagFacility>(this)->group_a = a;
        for (i32 i = 1;; ++i) {
            if (i > list_size(jo::view<void*>(jo::as_recruit_entry(rec)->rows))) break;
            std::memset(tmp1, 0, sizeof(tmp1));
            u8* const e = list_at(jo::view<void*>(jo::as_recruit_entry(rec)->rows), i);
            std::memcpy(&jo::as_recruit_order(tmp1)->kind, &jo::as_recruit_row_item(e)->kind, 2);
            std::memcpy(&jo::as_recruit_order(tmp1)->arg, &jo::as_recruit_row_item(e)->rate, 4);
            std::memcpy(&jo::as_recruit_order(tmp1)->count, &jo::as_recruit_row_item(e)->count, 4);  // i16 count + the 2 pad bytes
            list_add(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_79), tmp1);
        }
        jo::self<jo::FlagFacility>(this)->recruit_value = jo::view<i32>(jo::as_recruit_entry(rec)->value);
        if (jo::as_recruit_entry(rec)->level > 0) jo::self<jo::FlagFacility>(this)->flag_98 = jo::view<u8[4]>(jo::as_recruit_entry(rec)->level)[0];
        if (jo::as_recruit_entry(rec)->population > 0) {
            joan::orig::as_unit(jo::view<void*>(jo::self<jo::FlagFacility>(this)->owner))->set_56(joan::orig::arg::i32(jo::as_recruit_entry(rec)->population));  // 0x00469380
            joan::orig::as_unit(jo::view<void*>(jo::self<jo::FlagFacility>(this)->owner))->set_52(joan::orig::arg::i32(jo::as_recruit_entry(rec)->population));  // 0x00469330
        }
        if (jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_7d) != nullptr) {
            delete_463fc0(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_7d));
            jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_7d) = nullptr;
        }
        jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_7d) = new_list(0xa);
        rec = joan::orig::arg::as<u8*>(joan::orig::as_recruit_res(obj(d::g_recruit_res))->group_at(joan::orig::arg::u32(sw(b))));  // 0x00469520
        if (jo::view<void*>(jo::as_recruit_entry(rec)->rows) == nullptr) return u(rec);
        jo::self<jo::FlagFacility>(this)->group_b = b;
        i32 n = 0;
        for (i32 i = 1;; ++i) {
            n = list_size(jo::view<void*>(jo::as_recruit_entry(rec)->rows));
            if (i > n) break;
            std::memset(tmp1, 0, sizeof(tmp1));  // the original clears [ebp-0x20] here
            u8* const e = list_at(jo::view<void*>(jo::as_recruit_entry(rec)->rows), i);
            std::memcpy(&jo::as_recruit_order(tmp2)->kind, &jo::as_recruit_row_item(e)->kind, 2);
            std::memcpy(&jo::as_recruit_order(tmp2)->arg, &jo::as_recruit_row_item(e)->rate, 4);
            std::memcpy(&jo::as_recruit_order(tmp2)->count, &jo::as_recruit_row_item(e)->count, 4);  // i16 count + the 2 pad bytes
            list_add(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_7d), tmp2);
        }
        return static_cast<u32>(n);
    }
};

#define B101(name, addr, fn) JOAN_REPLACE("game/b101_02", name, addr, fn)
B101("Obj::dtor_4683a0", 0x004683a0, &H::dtor_4683a0);
B101("Obj::load_flag_groups", 0x00468790, &H::load_flag_groups);
#undef B101

}  // namespace game_b101_02
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

JOAN_COUNTER(c_init_468410, "Panel::init_468410");

struct H {
    // ---------------------------------------------------------------- 0x00468410
    std::uintptr_t init_468410(u32 a, const char* b) {
        c_init_468410.hit();
        joan::orig::as_facility(this)->set_kind_name(joan::orig::arg::i16(a), b);  // 0x0046d5e0
        jo::self<jo::FlagFacility>(this)->flag_97 = 1;
        jo::self<jo::FlagFacility>(this)->flag_98 = 1;
        void* const mem = op_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
        jo::DynArray* const p = mem ? joan::orig::arg::as<jo::DynArray*>(joan::orig::dynarray(mem)->construct(0xa, 0x1e, 0)) : nullptr;  // 0x005843d0
        jo::self<jo::FlagFacility>(this)->list_89 = p;
        return joan::orig::arg::as<u32>(p);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Panel::init_468410", 0x00468410, &H::init_468410);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
namespace {
namespace game_b100_02 {

using joan::img::str;

void* const kItemTypes = obj(d::g_item_res);

JOAN_COUNTER(c_release3, "Obj::release_79_7d_89");

struct H {
    // ------------------------------------------------------------------ 0x004684e0
    std::uintptr_t release_79_7d_89() {
        c_release3.hit();
        if (jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_79) != nullptr) {
            delete_463fc0(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_79));
            jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_79) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_7d) != nullptr) {
            delete_463fc0(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_7d));
            jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_7d) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_89) != nullptr) {
            joan::orig::as_recruit_facility(this)->clear_orders();  // 0x00468d70
            delete_463fc0(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_89));
            jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_89) = nullptr;
        }
        return joan::orig::arg::as<u32>(joan::orig::as_facility(this)->detach());  // 0x0046d630
    }

    // ------------------------------------------------------------------ 0x00468c10
    u32 set_flag_models() {
        const i32 eng = joan::orig::as_name_table(kItemTypes)->find_4a36c0(str(d::s_Object_Models_Items_Flags_En));  // 0x004a36c0 english_flag (a type id)
        const i32 fra = joan::orig::as_name_table(kItemTypes)->find_4a36c0(str(d::s_Object_Models_Items_Flags_Fr));  // 0x004a36c0 french_flag
        // void callee; callers discard the result. The type ids go in as the dwords the callee reads.
        if (jo::view<i32>(jo::self<jo::FlagFacility>(this)->owner->captured) != 0)
            joan::orig::as_formation(jo::view<void*>(jo::self<jo::FlagFacility>(this)->owner))->place_units(eng, static_cast<std::uint32_t>(fra));  // 0x004c4440
        else
            joan::orig::as_formation(jo::view<void*>(jo::self<jo::FlagFacility>(this)->owner))->place_units(fra, static_cast<std::uint32_t>(eng));  // 0x004c4440
        return 0;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("Obj::release_79_7d_89", 0x004684e0, &H::release_79_7d_89);
B100("Obj::set_flag_models", 0x00468c10, &H::set_flag_models);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
namespace {
namespace game_b102_02 {

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

JOAN_COUNTER(c_update, "Obj::update_468600");

void* list_at(const void* list, i32 i) { return joan::orig::arg::as<void*>(joan::orig::as_dynarray(list)->get(joan::orig::arg::i32(i))); }  // 0x00417200

struct H {
    // 0x00468600
    std::uintptr_t update_468600() {
        c_update.hit();
        const i32 a = joan::orig::arg::as<i32>(joan::orig::as_place(jo::view<void*>(jo::self<jo::FlagFacility>(this)->owner))->recruit_pool());  // 0x00469360
        const i32 b = joan::orig::arg::as<i32>(joan::orig::as_place(jo::view<void*>(jo::self<jo::FlagFacility>(this)->owner))->recruit_cap());  // 0x004693b0
        const Fpu fpu{true};
        const double scaled = fpu.mul(static_cast<double>(b), global<double>(d::dbl_0_95));  // 0.95
        jo::self<jo::FlagFacility>(this)->i32_93 = ftol(fpu.sub(static_cast<double>(a), scaled));
        void* const holder = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_places))->get_e(static_cast<i32>(jo::self<jo::FlagFacility>(this)->i16_91)));  // 0x0043c450
        u8* const v = virtual_call<u8*>(holder, 0x18);
        if (jo::as_building(v)->queue_n != 0) return addr(v);
        const i32 n = list_count(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_89));
        if (n < 1) return static_cast<u32>(n);
        u8* const e = static_cast<u8*>(list_at(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_89), 1));
        joan::orig::as_unit(v)->push_order(static_cast<i32>(jo::as_recruit_order(e)->kind), joan::orig::arg::i32(jo::as_recruit_order(e)->arg), 1, static_cast<i32>(jo::view<i8>(jo::self<jo::FlagFacility>(this)->flag_98)));  // 0x00467ea0
        const i32 z = ftol(static_cast<double>(jo::self<jo::FlagFacility>(this)->pos[2]));
        const i32 y = ftol(static_cast<double>(jo::self<jo::FlagFacility>(this)->pos[1]));
        const i32 x = ftol(static_cast<double>(jo::self<jo::FlagFacility>(this)->pos[0]));
        joan::orig::as_order_entry(&jo::as_building(v)->order_arg)->set(7u, static_cast<std::uint8_t>(joan::orig::arg::u32(5)), joan::orig::arg::u32(x), joan::orig::arg::u32(y), joan::orig::arg::u32(z), 0u, 0x1388u, 0u, 0u);  // 0x004bb950
        jo::as_recruit_order(e)->count = jo::as_recruit_order(e)->count - 1;
        if (jo::as_recruit_order(e)->count != 0) return static_cast<u32>(jo::as_recruit_order(e)->count);
        return thiscall<u32>(fn::Obj_call_584a10_468d20, this);  // 0x00468d20
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("Obj::update_468600", 0x00468600, &H::update_468600);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
namespace {
namespace game_b99_02 {

JOAN_COUNTER(c_apply_468a80, "Obj::apply_468a80");

struct H {
    // ------------------------------------------------------------------ 0x00468a80
    std::uintptr_t apply_468a80() {
        c_apply_468a80.hit();
        if (jo::view<i8>(jo::self<jo::SaveEntry>(this)->follow_ref) == 0) return u(this);
        const i32 kind = jo::view<i32>(jo::self<jo::SaveEntry>(this)->ref_kind);
        if (kind == 1) {
            const i32 r = joan::orig::as_dynarray(obj(d::g_units))->empty_at(static_cast<i32>(jo::view<i16>(jo::self<jo::SaveEntry>(this)->ref_id)));  // 0x00428b70
            if (r != 0) return static_cast<u32>(r);
            void* const item = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(static_cast<i32>(jo::view<i16>(jo::self<jo::SaveEntry>(this)->ref_id)));  // 0x0041b3c0
            return joan::orig::arg::as<u32>(joan::orig::as_unit(item)->position(joan::orig::arg::mptr(jo::self<jo::SaveEntry>(this)->values)));  // 0x004fedb0
        }
        if (kind == 2) {
            const i32 r = joan::orig::as_units(obj(d::g_items))->list.empty_at(static_cast<i32>(jo::view<i16>(jo::self<jo::SaveEntry>(this)->ref_id)));  // 0x00425d50
            if (r != 0) return static_cast<u32>(r);
            void* const unit = joan::orig::as_units(obj(d::g_items))->list.ptr_at(static_cast<i32>(jo::view<i16>(jo::self<jo::SaveEntry>(this)->ref_id)));  // 0x00425d20
            return joan::orig::arg::as<u32>(joan::orig::as_map_object(unit)->position(joan::orig::arg::mptr(jo::self<jo::SaveEntry>(this)->values)));  // 0x00499930
        }
        return static_cast<u32>(kind);
    }

    // ------------------------------------------------------------------ 0x00468d70
    u32 remove_all_89() {
        i32 i = list_count(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_89));
        u32 r = static_cast<u32>(i);
        for (; i >= 1; --i) joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_89))->linkout_584a10(i);  // 0x00584a10
        return r;  // callers discard the result
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::apply_468a80", 0x00468a80, &H::apply_468a80);
B99("Obj::remove_all_89", 0x00468d70, &H::remove_all_89);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

struct H {
    // ------------------------------------------------------------------ 0x00468cd0
    void add_record(i16 w, u32 a, u32 b) {
        u8 rec[12] = {};
        std::memcpy(&jo::as_recruit_order(rec)->kind, &w, 2);
        std::memcpy(&jo::as_recruit_order(rec)->arg, &a, 4);
        std::memcpy(&jo::as_recruit_order(rec)->count, &b, 4);
        joan::orig::dynarray(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_89))->linkin(static_cast<void*>(rec));  // 0x00584620
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::add_record", 0x00468cd0, &H::add_record);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

struct H {
    // ------------------------------------------------------------------ 0x00468d20
    void call_584a10() {
        if (joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_89))->size() >= 1)  // 0x00417ad0
            joan::orig::dynarray(jo::view<void*>(jo::self<jo::FlagFacility>(this)->list_89))->linkout(1);        // 0x00584a10
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Obj::call_584a10", 0x00468d20, &H::call_584a10);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b104_02.cpp (game/b104_02)
namespace {
namespace game_b104_02 {

using joan::call::import_call;
struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kKinds = obj(d::g_types);

i32 word_arg(u16 w) { return static_cast<i32>(static_cast<i16>(w)); }

u8* kind_of(u16 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(word_arg(w)))); }  // 0x0042d0a0

JOAN_COUNTER(c_edge_walk, "Obj::edge_walk_468dd0");

struct H {
    // ------------------------------------------------------------------ 0x00468dd0
    float margin(float r) {
        const float ten = global<float>(d::flt_10);
        return ten > r ? 10.0f : r;  // fcomp + test ah,0x41 jne: 10 only when 10 > r
    }
    u32 edge_walk_468dd0(Vec3* out, float* ang, u32 type_raw) {
        c_edge_walk.hit();
        const u8* const kind = kind_of(static_cast<u16>(type_raw));
        float r = 0.0f;  // [ebp-0xc]
        switch (jo::view<i32>(jo::self<jo::UnitType>(kind)->shape_kind)) {
            case 1: r = jo::self<jo::UnitType>(kind)->shape_sphere[3]; break;
            case 2: {
                const Vec3 a{jo::self<jo::UnitType>(kind)->shape_box[0], jo::self<jo::UnitType>(kind)->shape_box[1], 0.0f};
                const Vec3 b{jo::self<jo::UnitType>(kind)->shape_box[3], jo::self<jo::UnitType>(kind)->shape_box[4], 0.0f};
                Vec3 tmp;  // [ebp-0x58]
                const Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &a, &b));  // 0x0060cc90
                const double len = import_call<double>(d::iat_v3d_87_length, &d);       // length
                const Fpu fpu{true};
                r = static_cast<float>(fpu.div(len, static_cast<double>(global<float>(d::flt_2))));
                break;
            }
            case 3: r = jo::self<jo::UnitType>(kind)->shape_cylinder[3]; break;
            default: break;
        }
        const float ten = global<float>(d::flt_10);
        out->x = jo::self<jo::Facility>(this)->bounds[0] - ten;
        out->y = jo::self<jo::Facility>(this)->bounds[1] - ten - r;
        jo::view<u32>(ang[0]) = 0x4096cbe4u;
        i16 state = 0;  // [ebp-0x28]
        while (state < 4) {
            u8* const map = jo::view<u8*>(jo::self<jo::Facility>(this)->owner->cell);
            if (map != nullptr) {
                u8* const cell = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(map)->grid(joan::orig::arg::i16(0)));  // 0x005600c0
                if (cell != nullptr) {
                    const Fpu fpu{true};
                    const float fx = static_cast<float>(fpu.div(
                        fpu.sub(static_cast<double>(out->x), static_cast<double>(jo::as_world_cell(map)->origin_x)),
                        static_cast<double>(jo::as_path_space(cell)->cell_w)));
                    const float fy = static_cast<float>(fpu.div(
                        fpu.sub(static_cast<double>(out->y), static_cast<double>(jo::as_world_cell(map)->origin_y)),
                        static_cast<double>(jo::as_path_space(cell)->cell_h)));
                    const i32 ix = ftol(static_cast<double>(fx));
                    const i32 iy = ftol(static_cast<double>(fy));
                    const i32 even = (static_cast<i32>(static_cast<u32>(ix) + static_cast<u32>(iy)) % 2) == 0 ? 1 : 0;
                    out->z = joan::orig::arg::as<float>(joan::orig::as_terrain(jo::view<void*>(jo::as_path_space(cell)->grid))->height_4c7b70(joan::orig::arg::f32(fx), joan::orig::arg::f32(fy), static_cast<char>(joan::orig::arg::i32(even))));  // 0x004c7b70
                } else {
                    out->z = jo::self<jo::Facility>(this)->pos[2];
                }
            } else {
                out->z = jo::self<jo::Facility>(this)->pos[2];
            }
            switch (state) {
                case 0:
                    out->x = out->x + ten;
                    out->y = jo::self<jo::Facility>(this)->bounds[1] - margin(r);
                    jo::view<u32>(ang[0]) = 0x4096cbe4u;
                    if (!(jo::self<jo::Facility>(this)->bounds[3] + ten >= out->x)) {
                        out->x = jo::self<jo::Facility>(this)->bounds[3] + ten;
                        state = 1;
                    }
                    break;
                case 1:
                    out->y = out->y + ten;
                    out->x = margin(r) + jo::self<jo::Facility>(this)->bounds[3];
                    jo::view<u32>(ang[0]) = 0;
                    if (!(jo::self<jo::Facility>(this)->bounds[4] + ten >= out->y)) {
                        out->y = jo::self<jo::Facility>(this)->bounds[4] + ten;
                        state = 2;
                    }
                    break;
                case 2:
                    out->x = out->x - ten;
                    out->y = margin(r) + jo::self<jo::Facility>(this)->bounds[4];
                    jo::view<u32>(ang[0]) = 0x3fc90fdbu;
                    if (jo::self<jo::Facility>(this)->bounds[0] - ten > out->x) {
                        out->x = jo::self<jo::Facility>(this)->bounds[0] - ten;
                        state = 3;
                    }
                    break;
                case 3:
                    out->y = out->y - ten;
                    out->x = jo::self<jo::Facility>(this)->bounds[0] - margin(r);
                    jo::view<u32>(ang[0]) = 0x40490fdbu;
                    if (jo::self<jo::Facility>(this)->bounds[1] - ten > out->y) {
                        out->y = jo::self<jo::Facility>(this)->bounds[1] - ten;
                        state = 4;
                    }
                    break;
                default: break;
            }
        }
        const u32 e = static_cast<u32>(static_cast<i32>(state));
        return state < 4 ? ((e & 0xffffff00u) | 1u) : (e & 0xffffff00u);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_02", name, addr, fn)
B("Obj::edge_walk_468dd0", 0x00468dd0, &H::edge_walk_468dd0);
#undef B

}  // namespace game_b104_02
}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {

struct H {
    void* self() { return this; }
    u32 zero() { return 0; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("Obj::self_469310", 0x00469310, &H::self);
B("Obj::zero_4692f0", 0x004692f0, &H::zero);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {

struct H {
    // 0x00469380 / 0x00469330
    void set_56(std::int32_t v) { jo::view<std::int32_t>(jo::self<jo::Place>(this)->counter_max) = v; }
    void set_52(std::int32_t v) { jo::view<std::int32_t>(jo::self<jo::Place>(this)->counter) = v; }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Unit::set_56", 0x00469380, &H::set_56);
M("Unit::set_52", 0x00469330, &H::set_52);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

struct H {
    // 0x00469360 / 0x004693b0 / 0x004aef10
    std::int32_t get_52() { return jo::view<std::int32_t>(jo::self<jo::Place>(this)->counter); }
    std::int32_t get_56() { return jo::view<std::int32_t>(jo::self<jo::Place>(this)->counter_max); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("get_52", 0x00469360, &H::get_52);
B("get_56", 0x004693b0, &H::get_56);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::RecruitFacility::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::ctor_468300>(this)); }  // 0x468300
auto joan::orig::RecruitFacility::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_02::H::dtor_4683a0>(this)); }  // 0x4683a0
auto joan::orig::RecruitFacility::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_02::H::release_79_7d_89>(this)); }  // 0x4684e0
auto joan::orig::RecruitFacility::load_flag_groups(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_02::H::load_flag_groups>(this, a0, a1)); }  // 0x468790
auto joan::orig::Trigger::follow_target() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::apply_468a80>(this)); }  // 0x468a80
auto joan::orig::RecruitFacility::set_flag_models() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_02::H::set_flag_models>(this)); }  // 0x468c10
auto joan::orig::RecruitFacility::add_order(std::int16_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b95_01::H::add_record>(this, a0, a1, a2); }  // 0x468cd0
auto joan::orig::RecruitFacility::pop_order() -> void { joan::bind::member<&::game_b97_01::H::call_584a10>(this); }  // 0x468d20
auto joan::orig::RecruitFacility::clear_orders() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::remove_all_89>(this)); }  // 0x468d70
auto joan::orig::Unit::set_52(std::int32_t a0) -> void { joan::bind::member<&::game_b88_06::H::set_52>(this, a0); }  // 0x469330
auto joan::orig::Place::recruit_pool() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::get_52>(this)); }  // 0x469360
auto joan::orig::Unit::set_56(std::int32_t a0) -> void { joan::bind::member<&::game_b88_06::H::set_56>(this, a0); }  // 0x469380
auto joan::orig::Place::recruit_cap() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::get_56>(this)); }  // 0x4693b0
