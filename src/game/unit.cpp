// game/unit.cpp: functions of the original d:\ww\unit.cpp (re/attribution.csv).
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
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

void* const kMemory = obj(d::g_mem);     // allocator (0x0058ed70 free)
void* const kTypes = obj(d::g_types);      // unit types

}  // namespace

// ---------------------------------------- from modules/game_b86_05.cpp (game/b86_05)
//
//    - Unit::cleanup 0x00461f10: detaches the unit from everything (links, carrier +0x2a1,
//      sound slots +0x329, +0x325 / +0x319 / +0x321 / +0x31d / +0x2f5 / +0x309 / +0x2f9 /
//      +0x32d / +0x331 / +0x1c1 objects, +0x1bd / +0x1bf markers, +0x378 / +0x37c buffers,
//      units +0x1e2 / +0x1e6 / +0x1f5 / +0x1f9).
//
//  - Leftover eax values that are well defined (this, a callee's result) are returned:
//    0x00461f10 / 0x0043ac60 / the setters leave this, 0x004214b0 / 0x00417aa0 their last
//    callee's result, 0x004236d0 this (empty list) or the last 0x0058ed70 result.
//    0x00432300 / 0x00438f80 return void (callers never read eax).
//
//  - 0x00461f10: the +0x1e6 / +0x1f9 branches test +0x2f5, which is always 0 there; the
namespace {
namespace game_b86_05 {

using joan::img::str;

void* const kGameUnits = obj(d::g_units);  // unit manager
void* const kStations = obj(d::g_nations);
void* const kMarkers = obj(d::g_items);
const char* const kUnitCpp = str(d::s_D_ww_unit_cpp);    // "D:\ww\unit.cpp"

void mem_free(void* p, const char* file, std::int32_t line) {
    joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kGameUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t* type_of(std::int16_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x0042d0a0

JOAN_COUNTER(c_cleanup, "Unit::cleanup");

struct H {
    // Scalar-deleting-destructor release of a pointer field (delete p; field = 0).
    template <class T>
    static void drop(T*& field, std::uintptr_t deleter) {
        void* const p = field;
        if (p != nullptr) thiscall<void*>(deleter, p, 1);
        field = nullptr;
    }
    static void detach_scene(jo::Actor* o) {  // object's scene +0x18 forgets it (0x005efdb0)
        if (o->scene != nullptr) {
            joan::orig::as_scene(o->scene)->detach(joan::orig::arg::mptr(o));  // 0x005efdb0
        }
    }

    // ---- 0x00461f10 Unit::cleanup ------------------------------------------------------------
    H* cleanup() {
        c_cleanup.hit();
        if (jo::self<jo::Unit>(this)->leader_id != 0) joan::orig::as_member(this)->release();           // 0x0042fcb0
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->leads()) != 0) joan::orig::as_group(this)->release();  // 0x0044a660, 0x0042dd70
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(kStations)->is_null(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side))) == 0) {  // 0x00484870
            void* const st = joan::orig::as_dynarray(kStations)->ptr_at(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side));  // 0x0047b600
            joan::orig::as_nation(st)->remove_unit(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x004b5cc0
        }
        joan::orig::as_squad(this)->clear();  // 0x00422f10
        if (jo::self<jo::Unit>(this)->orders != nullptr) drop(jo::self<jo::Unit>(this)->orders, fn::code_value(fn::Obj_scalar_dtor_415220));  // 0x00415220
        if (jo::self<jo::Unit>(this)->item_ref != 0) {
            void* s = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(*reinterpret_cast<const std::int16_t*>(&jo::self<jo::Unit>(this)->item_ref))));  // 0x00425ce0
            joan::orig::as_res(s)->release_slot(1, 0);                                                   // 0x004a3e30
            s = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(*reinterpret_cast<const std::int16_t*>(&jo::self<jo::Unit>(this)->item_ref))));
            joan::orig::as_res(s)->release_slot(2, 0);
            jo::self<jo::Unit>(this)->item_ref = 0;
        }
        if (jo::self<jo::Unit>(this)->weapon_model != nullptr) {
            joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(static_cast<std::int16_t>(jo::self<jo::Unit>(this)->weapon_model_id)));  // 0x004b16a0
            detach_scene(jo::self<jo::Unit>(this)->weapon_model);
            drop(jo::self<jo::Unit>(this)->weapon_model, fn::code_value(fn::delete_420840));  // 0x00420840
        }
        if (jo::self<jo::Unit>(this)->modifiers != nullptr) drop(jo::self<jo::Unit>(this)->modifiers, fn::code_value(fn::Obj_scalar_dtor_416600));  // 0x00416600
        {
            const std::int32_t marker = jo::self<jo::Unit>(this)->carried_a;
            joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
            if (joan::orig::as_units(kMarkers)->list.empty_at(joan::orig::arg::i32(marker)) == 0) {  // 0x00425d50
                joan::orig::as_units(kMarkers)->remove_49f370(joan::orig::arg::i32(marker));                       // 0x0049f370
            }
            jo::self<jo::Unit>(this)->carried_a = 0;
        }
        joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::mptr(0));  // 0x0050a480 (ebp-8 = +0x1bf is dead)
        if (joan::orig::as_units(kMarkers)->list.empty_at(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->carried_b)) == 0) {
            joan::orig::as_units(kMarkers)->remove_49f370(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->carried_b));
        }
        jo::self<jo::Unit>(this)->carried_b = 0;
        if (jo::self<jo::Unit>(this)->mask_slots != nullptr) {
            mem_free(jo::self<jo::Unit>(this)->mask_slots, kUnitCpp, static_cast<std::int32_t>(global<std::int16_t>(d::g_a05de4)) + 0x49);
            jo::self<jo::Unit>(this)->mask_slots = nullptr;
        }
        if (jo::self<jo::Unit>(this)->mask_slots_b != nullptr) {
            mem_free(jo::self<jo::Unit>(this)->mask_slots_b, kUnitCpp, static_cast<std::int32_t>(global<std::int16_t>(d::g_a05de4)) + 0x4e);
            jo::self<jo::Unit>(this)->mask_slots_b = nullptr;
        }
        jo::self<jo::Unit>(this)->slot_index = -1;
        joan::orig::as_unit(this)->set_group(0);  // 0x004fed00
        if (jo::self<jo::Unit>(this)->model != nullptr) {
            detach_scene(jo::self<jo::Unit>(this)->model);
            drop(jo::self<jo::Unit>(this)->model, fn::code_value(fn::delete_420840));  // 0x00420840
        }
        if (jo::self<jo::Unit>(this)->model_b != nullptr) {
            detach_scene(jo::self<jo::Unit>(this)->model_b);
            drop(jo::self<jo::Unit>(this)->model_b, fn::code_value(fn::delete_420840));
        }
        if (jo::self<jo::Unit>(this)->draw != nullptr) drop(jo::self<jo::Unit>(this)->draw, fn::code_value(fn::Obj520_scalar_delete));  // 0x00416640
        if (jo::self<jo::Unit>(this)->actor != nullptr) {
            detach_scene(jo::self<jo::Unit>(this)->actor);
            jo::self<jo::Unit>(this)->actor = nullptr;
            const std::uint32_t v = jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->collision_id);
            joan::orig::as_unit_type(type_of(jo::self<jo::Unit>(this)->type_id))->release_model_slot(joan::orig::arg::i32(v));  // 0x00527690
        }
        if (jo::self<jo::Unit>(this)->actor_c != nullptr) {
            drop(jo::self<jo::Unit>(this)->actor_c, fn::code_value(fn::delete_420840));
            jo::self<jo::Unit>(this)->anim_set_c = 0;
        }
        if (jo::self<jo::Unit>(this)->actor_b != nullptr) {
            drop(jo::self<jo::Unit>(this)->actor_b, fn::code_value(fn::delete_420840));
            jo::self<jo::Unit>(this)->anim_set_b = 0;
        }
        if (jo::self<jo::Unit>(this)->trail_a != nullptr) drop(jo::self<jo::Unit>(this)->trail_a, fn::code_value(fn::sdd_00420790));  // 0x00420790
        if (jo::self<jo::Unit>(this)->trail_b != nullptr) drop(jo::self<jo::Unit>(this)->trail_b, fn::code_value(fn::sdd_00420790));
        if (unit_missing(jo::self<jo::Unit>(this)->mount_id) == 0) {
            joan::orig::as_slot(unit_get(jo::self<jo::Unit>(this)->mount_id))->select(0);  // 0x00482930
        }
        if (unit_missing(jo::self<jo::Unit>(this)->rider_id) == 0 && jo::self<jo::Unit>(this)->actor != nullptr) {
            joan::orig::as_unit(unit_get(jo::self<jo::Unit>(this)->rider_id))->mount(0);  // 0x004fc220
        }
        if (unit_missing(jo::self<jo::Unit>(this)->mount_ref) == 0) {
            joan::orig::as_unit(unit_get(jo::self<jo::Unit>(this)->mount_ref))->set_rider(0);  // 0x0051a8a0
        }
        if (unit_missing(jo::self<jo::Unit>(this)->rider_ref) == 0 && jo::self<jo::Unit>(this)->actor != nullptr) {
            joan::orig::as_unit(unit_get(jo::self<jo::Unit>(this)->rider_ref))->set_mount(0);  // 0x0051a250
        }
        jo::self<jo::Unit>(this)->in_scene = 0;
        return this;
    }
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_05", name, addr, fn)
B86("Unit::cleanup", 0x00461f10, &H::cleanup);
#undef B86

}  // namespace game_b86_05
}  // namespace

// ---------------------------------------- from modules/game_b88_02.cpp (game/b88_02)
//
//      0x004f8550 Unit::init(type) - ret 4 (called by Units::create 0x0051dfc0 and 0x004c96f0;
//                 the return value is not used by either). Stores the type word (+0x3b), copies
//                 the type record (0x0042d0a0 on 0x00a48d88) fields into the unit (names +0x3d /
//                 +0x7f / +0xc1, stats through the 0x004fe510..0x004fecd0 setters), creates the
//                 0x1e-byte inventory object (+0x1c1, 0x00495950 / 0x004959b0 / 0x00495b40) and
//                 fills it with the type's items (0x004970c0 / 0x00497270 / 0x004973a0 with
//                 records from 0x00425ce0 on 0x00a58f50), a random treasure roll (0x00590540 vs
//                 per-class percentages) for 0x00464100 / 0x00464030 on 0x00a50288, level record
//                 (0x004a4e60), two type-sized arrays (+0x378 / +0x37c, 0x0058e7b0 + memset),
//                 0x00523240(0), the +0x1aa list re-add loop (0x00531360 / 0x005313a0 /
//                 0x00531520), +0x2df = 0x00511e70(), and clears the task record +0x2be
//                 (0x004e4520).
//
//  - 0x004f8550 has an SEH frame only around operator new + the 0x00495950 ctor (delete on throw);
//    the normal path is reproduced.
namespace {
namespace game_b88_02 {

using joan::call::cdecl_call;
std::uint8_t* bytes(const void* p, std::int32_t offset = 0) {
    return static_cast<std::uint8_t*>(const_cast<void*>(p)) + offset;
}

void* const kItems = obj(d::g_item_res);
void* const kRandom = obj(d::g_misc);
constexpr img::Addr kUnitCpp(d::s_D_ww_unit_cpp);  // "D:\ww\unit.cpp"

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }
jo::UnitType* type_of(std::int16_t id) {
    return jo::as_unit_type(joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(sx16(id)))));  // 0x0042d0a0 (reads the word)
}
jo::ItemDef* item(std::int16_t id) {
    return jo::as_item_def(joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(kItems)->get(joan::orig::arg::i16(sx16(id)))));  // 0x00425ce0 (reads the word)
}
char* strcpy_(void* dst, const void* src) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(dst)), joan::orig::arg::str(src)); }  // strcpy
using joan::orig::helpers::memset_;
std::int32_t random_(std::int32_t n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540
std::int32_t line_base() { return static_cast<std::int32_t>(global<std::int16_t>(d::g_a05de8)); }

JOAN_COUNTER(c_init, "Unit::init");

struct H {
    // ------------------------------------------------------------------------ 0x004f8550
    void init(std::int32_t type) {
        c_init.hit();
        jo::self<jo::Unit>(this)->type_id = static_cast<std::int16_t>(type);
        jo::UnitType* const t = type_of(jo::self<jo::Unit>(this)->type_id);  // ebp-0x14
        if (t->script_id != 0) {
            std::uint8_t* const r =
                joan::orig::arg::as<std::uint8_t*>(joan::orig::as_uscr_res(obj(d::g_uscr_res))->entry(joan::orig::arg::i32(sx16(*reinterpret_cast<const std::int16_t*>(&t->script_id)))));  // 0x00484540
            jo::self<jo::Unit>(this)->script = static_cast<jo::ScriptBlock*>(jo::as_unit_script(r)->object);
        } else {
            jo::self<jo::Unit>(this)->script = nullptr;
        }
        jo::self<jo::Unit>(this)->action = 0;
        jo::self<jo::Unit>(this)->anim_kind = 1;
        jo::self<jo::Unit>(this)->anim_item_using = 1;
        jo::self<jo::Unit>(this)->anim_special = 0;
        jo::self<jo::Unit>(this)->armour_flags[0] = 0xff;
        jo::self<jo::Unit>(this)->armour_flags[6] = 0xff;
        jo::view<std::uint8_t>(jo::self<jo::Unit>(this)->combo_state) = 0;
        jo::view<std::uint8_t>(jo::self<jo::Unit>(this)->combo_pre_len) = 0;
        jo::self<jo::Unit>(this)->combo_queued = 0;
        jo::self<jo::Unit>(this)->anim_done = 0;
        jo::self<jo::Unit>(this)->mount_busy = 0;
        jo::self<jo::Unit>(this)->view = 0;
        jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->path_index) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->i32_3ea) = 0;
        jo::self<jo::Unit>(this)->wait_limit = global<std::uint32_t>(d::g_unit_init_w_c);
        jo::self<jo::Unit>(this)->wait_count = jo::self<jo::Unit>(this)->wait_limit;
        jo::self<jo::Unit>(this)->u8_3e5 = 1;
        strcpy_(jo::self<jo::Unit>(this)->name_, t->title_b4);
        strcpy_(jo::self<jo::Unit>(this)->name_b, t->text_de);
        jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->rank) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->hit_points_base) = jo::view<std::uint32_t>(t->hit_points);
        joan::orig::as_unit(this)->set_value_135(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->hit_points_base)));  // 0x004fe510
        jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->hp_shown) = jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->hit_points_base);
        jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->base_max_ep) = t->base_max_ep;
        joan::orig::as_unit(this)->set_145(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->base_max_ep)));  // 0x004fe5d0
        strcpy_(jo::self<jo::Unit>(this)->name_c, t->str_129);
        jo::self<jo::Unit>(this)->kingdom_id = t->kingdom_id;
        jo::self<jo::Unit>(this)->race_id = t->race;
        joan::orig::as_unit(this)->set_11c(joan::orig::arg::u32(t->lives_max));  // 0x004fe640
        joan::orig::as_unit(this)->set_120(joan::orig::arg::u32(t->lives_max));  // 0x004fe670
        joan::orig::as_unit(this)->set_strength(joan::orig::arg::u32(sx16(t->strength)));  // 0x004fe6e0
        joan::orig::as_unit(this)->set_dexterity(joan::orig::arg::u32(sx16(t->dexterity)));  // 0x004fe710
        joan::orig::as_unit(this)->set_defense(joan::orig::arg::u32(sx16(t->defense)));  // 0x004fe740
        joan::orig::as_unit(this)->set_leadership(joan::orig::arg::u32(sx16(t->leadership)));  // 0x004fe770
        jo::self<jo::Unit>(this)->stats[4] = t->i16_16d;
        jo::self<jo::Unit>(this)->stats[5] = t->armor_code;
        jo::self<jo::Unit>(this)->stats[6] = t->i16_19d;
        jo::self<jo::Unit>(this)->stats[7] = t->i16_19f;
        jo::self<jo::Unit>(this)->stats[8] = t->i16_1a1;

        void* const mem = joan::crt::operator_new(sizeof(jo::UnitData));  // operator new (0x1e on x86)
        void* const inv = mem != nullptr ? joan::orig::as_unit_data(mem)->construct() : nullptr;  // 0x00495950
        jo::self<jo::Unit>(this)->modifiers = static_cast<jo::Modifiers*>(inv);
        joan::orig::as_sounds(jo::self<jo::Unit>(this)->modifiers)->create(0u);  // 0x004959b0
        if (sx16(t->rank) >= 3) joan::orig::as_sounds(jo::self<jo::Unit>(this)->modifiers)->create_missing();  // 0x00495b40
        {
            // `if (1)` in the original.
            const std::int16_t n = static_cast<std::int16_t>(
                joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(kRandom)->random_range(0, 3)));  // 0x00590690, ebp-0x18 (word)
            jo::view<std::uint8_t>(jo::self<jo::Unit>(this)->combo_pre_len) = static_cast<std::uint8_t>(n);
            for (std::int32_t i = 0; i < sx16(n); ++i) jo::self<jo::Unit>(this)->armour_flags[i + 6] = 0x41;
        }
        if (sx16(t->item_weapon) != 0) {
            const std::int16_t id = t->item_weapon;
            const std::uint32_t a = jo::view<std::uint32_t>(item(id)->i32_42);
            const std::uint32_t b = jo::view<std::uint32_t>(item(id)->i32_42);
            const std::int16_t c = *reinterpret_cast<const std::int16_t*>(&item(id)->min_level);
            joan::orig::as_names(jo::self<jo::Unit>(this)->modifiers)->set_4970c0(1u, 0u, joan::orig::arg::u32(sx16(id)), joan::orig::arg::u32(sx16(c)), 1u, joan::orig::arg::u32(b), joan::orig::arg::u32(a), 0u);  // 0x004970c0
            jo::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->hand_mode = 1;
        }
        if (sx16(t->item_weapon_b) != 0) {
            const std::int16_t id = t->item_weapon_b;
            const std::uint32_t a = jo::view<std::uint32_t>(item(id)->i32_42);
            const std::uint32_t b = jo::view<std::uint32_t>(item(id)->i32_42);
            const std::int16_t c = *reinterpret_cast<const std::int16_t*>(&item(id)->min_level);
            joan::orig::as_names(jo::self<jo::Unit>(this)->modifiers)->set_4970c0(1u, 1u, joan::orig::arg::u32(sx16(id)), joan::orig::arg::u32(sx16(c)), 1u, joan::orig::arg::u32(b), joan::orig::arg::u32(a), 0u);  // 0x004970c0
        }
        if (sx16(t->item_shield) != 0) {
            const std::int16_t id = t->item_shield;
            const std::uint32_t a = jo::view<std::uint32_t>(item(id)->i32_42);
            const std::uint32_t b = jo::view<std::uint32_t>(item(id)->i32_42);
            const std::int16_t c = *reinterpret_cast<const std::int16_t*>(&item(id)->min_level);
            joan::orig::as_names(jo::self<jo::Unit>(this)->modifiers)->set_item(joan::orig::arg::u32(sx16(id)), joan::orig::arg::u32(sx16(c)), 1u, joan::orig::arg::u32(b), joan::orig::arg::u32(a), 0u);  // 0x00497270
            if (sx16(jo::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->hand_mode) == 0) jo::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->hand_mode = 2;
        }
        if (sx16(t->item_stack) != 0) {
            const std::int16_t id = t->item_stack;
            const std::int16_t d = jo::view<std::int16_t>(item(id)->stats[3]);
            const std::int16_t e = *reinterpret_cast<const std::int16_t*>(&item(id)->min_level);
            joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->new_stack_e(joan::orig::arg::u32(sx16(id)), joan::orig::arg::u32(sx16(e)), joan::orig::arg::u32(sx16(d)), 0u, 0u, 0u);  // 0x004973a0
        }
        if (sx16(t->treasure_list) != 0) {
            std::int32_t give = 0;  // ebp-0x20
            if (sx16(t->rank) >= 4) {
                give = 1;
            } else if (sx16(t->rank) >= 3) {
                std::uintptr_t pct = 0;
                switch (sx16(t->unit_class)) {
                case 1: pct = img::addr(d::g_a3cc1a); break;
                case 2: pct = img::addr(d::g_a3cbe8); break;
                case 3: pct = img::addr(d::g_a3cca0); break;
                case 4: pct = img::addr(d::g_a3cc38); break;
                case 5: pct = img::addr(d::g_a3ccca); break;
                case 6: pct = img::addr(d::g_a3cbcc); break;
                default: break;
                }
                if (pct != 0 && !(random_(0x64) > sx16(global<std::int16_t>(pct)))) give = 1;
            } else {
                std::uintptr_t pct = 0;
                switch (sx16(t->unit_class)) {
                case 1: pct = img::addr(d::g_a3ccd4); break;
                case 2: pct = img::addr(d::g_a3cc5c); break;
                case 3: pct = img::addr(d::g_a3ccb8); break;
                case 4: pct = img::addr(d::g_a3cca2); break;
                case 5: pct = img::addr(d::g_a3cd18); break;
                case 6: pct = img::addr(d::g_a3cc7c); break;
                default: break;
                }
                if (pct != 0 && !(random_(0x64) > sx16(global<std::int16_t>(pct)))) give = 1;
            }
            if (give != 0) {
                void* const tr = joan::orig::arg::as<void*>(joan::orig::as_names(obj(d::g_randList))->at(joan::orig::arg::i16(sx16(t->treasure_list))));  // 0x00464100
                joan::orig::as_grass(tr)->pick(joan::orig::arg::mptr(&jo::self<jo::Unit>(this)->drop_item), joan::orig::arg::mptr(&jo::self<jo::Unit>(this)->treasure_level));          // 0x00464030
            }
        }
        joan::orig::as_unit(this)->set_16f(joan::orig::arg::u32(sx16(t->i16_183)));  // 0x004fe7a0
        joan::orig::as_unit(this)->set_173(joan::orig::arg::u32(sx16(t->i16_185)));  // 0x004fe7d0
        joan::orig::as_unit(this)->set_177(joan::orig::arg::u32(sx16(t->i16_187)));  // 0x004fe800
        joan::orig::as_unit(this)->set_17b(joan::orig::arg::u32(sx16(t->i16_189)));  // 0x004feb80
        joan::orig::as_unit(this)->set_17f(joan::orig::arg::u32(sx16(t->i16_18b)));  // 0x004febb0
        joan::orig::as_unit(this)->set_183(joan::orig::arg::u32(sx16(t->i16_18d)));  // 0x004febe0
        joan::orig::as_unit(this)->set_187(joan::orig::arg::u32(sx16(t->i16_18f)));  // 0x004fec10
        joan::orig::as_unit(this)->set_18b(joan::orig::arg::u32(sx16(t->i16_191)));  // 0x004fec40
        joan::orig::as_unit(this)->set_18f(joan::orig::arg::u32(sx16(t->i16_193)));  // 0x004fec70
        joan::orig::as_unit(this)->set_193(joan::orig::arg::u32(sx16(t->i16_195)));  // 0x004feca0
        joan::orig::as_unit(this)->set_197(joan::orig::arg::u32(sx16(t->i16_197)));  // 0x004fecd0
        jo::self<jo::Unit>(this)->level = t->level;
        if (sx16(jo::self<jo::Unit>(this)->level) > 1) {
            std::uint8_t* const rec =
                joan::orig::arg::as<std::uint8_t*>(joan::orig::as_level_res(obj(d::g_level_res))->find(joan::orig::arg::i32(sx16(jo::self<jo::Unit>(this)->level) - 1)));  // 0x004a4e60
            jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->experience) = jo::view<std::uint32_t>(jo::as_level_rec(rec)->experience);
        } else {
            jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->experience) = 0;
        }
        jo::self<jo::Unit>(this)->i16_12e = t->i16_19b;
        switch (sx16(t->idle_kind)) {
        case 1: jo::self<jo::Unit>(this)->idle_time = 1; break;
        case 2: jo::self<jo::Unit>(this)->idle_time = 2; break;
        case 3: jo::self<jo::Unit>(this)->idle_time = 3; break;
        case 4: jo::self<jo::Unit>(this)->idle_time = 4; break;
        default: break;
        }
        jo::self<jo::Unit>(this)->regen_hp = sx16(t->regen_hp);
        jo::self<jo::Unit>(this)->regen_ep = sx16(t->regen_ep);
        jo::self<jo::Unit>(this)->morale = t->morale;
        jo::self<jo::Unit>(this)->combo_time = 0;
        std::memcpy(jo::self<jo::Unit>(this)->look_dir, bytes(obj(d::g_pos)), 12);
        jo::self<jo::Unit>(this)->trail_a = nullptr;
        jo::self<jo::Unit>(this)->trail_b = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->mount_id) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->rider_id) = 0;
        jo::self<jo::Unit>(this)->interact_stage = 0;
        jo::view<std::int16_t>(jo::self<jo::Unit>(this)->crit_hits) = 0;
        jo::view<std::int16_t>(jo::self<jo::Unit>(this)->crit_kills) = 0;
        jo::self<jo::Unit>(this)->mask_slots = nullptr;
        jo::self<jo::Unit>(this)->mask_slots_b = nullptr;
        if (type_of(jo::self<jo::Unit>(this)->type_id)->slot_count > 0) {
            {
                const std::int32_t line = line_base() + 0x10b;
                const std::int32_t size = type_of(jo::self<jo::Unit>(this)->type_id)->slot_count << 2;
                jo::self<jo::Unit>(this)->mask_slots = static_cast<jo::ArrI32*>(joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(size), joan::orig::arg::str(img::str(kUnitCpp)), joan::orig::arg::i32(line)));  // 0x0058e7b0
                const std::int32_t n = type_of(jo::self<jo::Unit>(this)->type_id)->slot_count << 2;
                memset_(jo::self<jo::Unit>(this)->mask_slots, 0, static_cast<std::uint32_t>(n));
            }
            jo::self<jo::Unit>(this)->crew_slots_ready = 1;
            {
                const std::int32_t line = line_base() + 0x110;
                const std::int32_t size = type_of(jo::self<jo::Unit>(this)->type_id)->slot_count << 2;
                jo::self<jo::Unit>(this)->mask_slots_b = static_cast<jo::ArrI32*>(joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(size), joan::orig::arg::str(img::str(kUnitCpp)), joan::orig::arg::i32(line)));  // 0x0058e7b0
                const std::int32_t n = type_of(jo::self<jo::Unit>(this)->type_id)->slot_count << 2;
                memset_(jo::self<jo::Unit>(this)->mask_slots_b, 0, static_cast<std::uint32_t>(n));
            }
        }
        jo::self<jo::Unit>(this)->slot_index = -1;
        joan::orig::as_unit(this)->apply_kind_links(0);  // 0x00523240
        for (std::int32_t i = 1; !(i > joan::orig::arg::as<std::int32_t>(joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->count())); ++i) {  // 0x00531360
            const std::int16_t* const w = joan::orig::arg::as<std::int16_t*>(joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->get(joan::orig::arg::i32(i)));   // 0x005313a0
            joan::orig::as_list(&jo::self<jo::Unit>(this)->upgrades)->has_ready(joan::orig::arg::i32(sx16(*w)));                                        // 0x00531520
        }
        jo::self<jo::Unit>(this)->target_select = 1;
        jo::self<jo::Unit>(this)->attack_chance = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->recruit_delay()));  // 0x00511e70
        jo::self<jo::Unit>(this)->can_be_ridden = 1;
        jo::self<jo::Unit>(this)->vision_range = global<std::uint32_t>(d::g_a3cc44);
        joan::orig::as_unit_task(&jo::self<jo::Unit>(this)->task_next)->set(0u, static_cast<std::uint8_t>(joan::orig::arg::u32(0)), 1u, 0u, 0u, 0u);  // 0x004e4520
    }
};

#define B88(name, addr, fn) JOAN_REPLACE("game/b88_02", name, addr, fn)
B88("Unit::init", 0x004f8550, &H::init);
#undef B88

}  // namespace game_b88_02
}  // namespace

// ---------------------------------------- from modules/game_b87_01.cpp (game/b87_01)
//
//    - Unit::set_value_135 0x004fe510 (v) ret 4: clamp(v, 0, 0x00523320()) through 0x00590aa0,
//      counter +0x120 fallback.
namespace {
namespace game_b87_01 {

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::obj;

struct H {
    // ---- 0x004fe510 Unit::set_value_135(v) ret 4 --------------------------------------------
    std::uintptr_t set_value_135(std::int32_t v) {  // eax: hit points or this
        const std::int32_t hi = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->hit_points_max());  // 0x00523320
        jo::self<jo::Unit>(this)->hit_points = v;
        jo::self<jo::Unit>(this)->hit_points = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(obj(d::g_misc))->limit_int(joan::orig::arg::i32(jo::self<jo::Unit>(this)->hit_points), 0, joan::orig::arg::i32(hi)));  // 0x00590aa0
        if (jo::self<jo::Unit>(this)->hit_points == 0 && jo::self<jo::Unit>(this)->state_120 != 0) {
            jo::self<jo::Unit>(this)->state_120 = jo::self<jo::Unit>(this)->state_120 - 1;
            jo::self<jo::Unit>(this)->hit_points = hi;
            jo::self<jo::Unit>(this)->hp_shown = jo::self<jo::Unit>(this)->hit_points;
            return static_cast<std::uint32_t>(jo::self<jo::Unit>(this)->hit_points);  // EAX = +0x135 (= hi), zero-extended
        }
        return reinterpret_cast<std::uintptr_t>(this);  // EAX = this
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_01", name, addr, fn)
B("Unit::set_value_135", 0x004fe510, &H::set_value_135);
#undef B

}  // namespace game_b87_01
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
//
//      0x004fe5d0 Unit::set_145(v) (ret 4)        - +0x145 = 0x00590aa0(v, 0, 0x00523700()) on 0xa45fa8
namespace {
namespace game_b88_06 {

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::obj;

void* const kFiles = obj(d::g_misc);

struct H {
    // 0x004fe5d0
    std::int32_t set_145(std::int32_t v) {
        const std::int32_t extra = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->value_149());  // 0x00523700
        jo::self<jo::Unit>(this)->mana = v;
        jo::self<jo::Unit>(this)->mana = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(kFiles)->limit_int(joan::orig::arg::i32(jo::self<jo::Unit>(this)->mana), 0, joan::orig::arg::i32(extra)));  // 0x00590aa0
        return jo::self<jo::Unit>(this)->mana;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Unit::set_145", 0x004fe5d0, &H::set_145);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
//
//    - Unit::apply_kind_links 0x00523240 (kind), ret 4: list 0xa56a50 (0x00531e70) of the
//      +0x169 entry of 0xa48d88's record (0x0042d0a0, word +0x3b); for each entry 1..count
//      (0x005239e0) whose word [0] == kind calls 0x005313d0(this + 0x1aa, word [2]).
//
//    - List::index_of 0x00416dc0 (id), ret 4; Unit::set_level 0x0045b720 (v), ret 4;
//      Unit::clear_flags 0x0050d0d0; Rec::set 0x004b9c80 (6 args), ret 0x18;
//      Unit::poke 0x00528440; Counter::step 0x005315a0; Obj::kind 0x004cd510;
//      ctors 0x00469e90 / 0x00469560 / 0x00468300 / 0x00467420 (base 0x0046d520, vtable,
//      memset +0x79); scalar deleting dtor 0x00548430; zero-ctors 0x00531140 / 0x004cea10 /
//      0x00495950; 0x0066e4b0 (0x005843d0(0x24, 0x10, 0)); List::is_empty_at 0x00485900;
//      List::at 0x00485790; Rec::at 0x004c2470; Rec::at 0x00484540; 0x0063ed40;
//      Rec::float_at 0x004c24b0; setters 0x00503fe0 / 0x00500d10 / 0x004fecd0 / 0x004feca0 /
//      0x004fec70 / 0x004fec40 / 0x004fec10 / 0x004febe0 / 0x004febb0 / 0x004feb80 /
//      0x004fe800 / 0x004fe7d0 / 0x004fe7a0 / 0x004fe770 / 0x004fe740 / 0x004fe710 /
//      0x004fe6e0 / 0x004fe670 / 0x004fe640 / 0x00503fb0 / 0x004fd630; no-op 0x00518d10.
//
//  - eax is reproduced where the original leaves a meaningful value (this, the callee's
//    return, the stored argument): 0x0045b720's eax is returned by 0x004949f0, so both
//    paths are kept (0x004fe510's eax / the argument). 0x00523240 leaves the loop counter
//    or list pointer in eax; both callers ignore it (void here). 0x00518d10 is a no-op that
//    leaves the caller's eax; its only caller ignores it.
namespace {
namespace game_b89_06 {

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_kind_links, "Unit::apply_kind_links");

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00523240
    void apply_kind_links(std::int32_t kind) {
        c_kind_links.hit();
        void* const rec = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->type_id))));  // 0x0042d0a0
        void* const list = joan::orig::arg::as<void*>(joan::orig::as_upgrade_res(obj(d::g_upgrade_res))->unit_upgrade_list(joan::orig::arg::i32(jo::as_unit_type(rec)->upgrade_id)));  // 0x00531e70
        for (std::int32_t i = 1; i <= jo::as_unit_upgrade_list(list)->slot_count; ++i) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_table(list)->slot(joan::orig::arg::i32(i)));  // 0x005239e0
            if (static_cast<std::int32_t>(jo::as_unit_upgrade_slot(e)->kind) != kind) continue;
            joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->add(static_cast<std::int32_t>(jo::as_unit_upgrade_slot(e)->upgrade));  // 0x005313d0
        }
    }

    void* set_d(std::uint32_t& field, std::uint32_t v) {
        field = v;
        return self();
    }
    void* set_w(std::uint16_t& field, std::uint32_t v) {
        field = static_cast<std::uint16_t>(v);
        return self();
    }
    std::uint32_t& stat(std::int32_t k) { return jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->stats_block.values[k]); }
    void* set_197(std::uint32_t v) { return set_d(stat(10), v); }   // 0x004fecd0
    void* set_193(std::uint32_t v) { return set_d(stat(9), v); }   // 0x004feca0
    void* set_18f(std::uint32_t v) { return set_d(stat(8), v); }   // 0x004fec70
    void* set_18b(std::uint32_t v) { return set_d(stat(7), v); }   // 0x004fec40
    void* set_187(std::uint32_t v) { return set_d(stat(6), v); }   // 0x004fec10
    void* set_183(std::uint32_t v) { return set_d(stat(5), v); }   // 0x004febe0
    void* set_17f(std::uint32_t v) { return set_d(stat(4), v); }   // 0x004febb0
    void* set_17b(std::uint32_t v) { return set_d(stat(3), v); }   // 0x004feb80
    void* set_177(std::uint32_t v) { return set_d(stat(2), v); }   // 0x004fe800
    void* set_173(std::uint32_t v) { return set_d(stat(1), v); }   // 0x004fe7d0
    void* set_16f(std::uint32_t v) { return set_d(stat(0), v); }   // 0x004fe7a0
    void* set_15b(std::uint32_t v) { return set_w(jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->stats[3]), v); }   // 0x004fe770
    void* set_159(std::uint32_t v) { return set_w(jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->stats[2]), v); }   // 0x004fe740
    void* set_157(std::uint32_t v) { return set_w(jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->stats[1]), v); }   // 0x004fe710
    void* set_155(std::uint32_t v) { return set_w(jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->stats[0]), v); }   // 0x004fe6e0
    void* set_120(std::uint32_t v) { return set_d(jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->state_120), v); }   // 0x004fe670
    void* set_11c(std::uint32_t v) { return set_d(jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->lives_max), v); }   // 0x004fe640
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Unit::apply_kind_links", 0x00523240, &H::apply_kind_links);
M("Obj::set_197", 0x004fecd0, &H::set_197);
M("Obj::set_193", 0x004feca0, &H::set_193);
M("Obj::set_18f", 0x004fec70, &H::set_18f);
M("Obj::set_18b", 0x004fec40, &H::set_18b);
M("Obj::set_187", 0x004fec10, &H::set_187);
M("Obj::set_183", 0x004febe0, &H::set_183);
M("Obj::set_17f", 0x004febb0, &H::set_17f);
M("Obj::set_17b", 0x004feb80, &H::set_17b);
M("Obj::set_177", 0x004fe800, &H::set_177);
M("Obj::set_173", 0x004fe7d0, &H::set_173);
M("Obj::set_16f", 0x004fe7a0, &H::set_16f);
M("Obj::set_15b", 0x004fe770, &H::set_15b);
M("Obj::set_159", 0x004fe740, &H::set_159);
M("Obj::set_157", 0x004fe710, &H::set_157);
M("Obj::set_155", 0x004fe6e0, &H::set_155);
M("Obj::set_120", 0x004fe670, &H::set_120);
M("Obj::set_11c", 0x004fe640, &H::set_11c);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b89_05.cpp (game/b89_05)
//
//    - Unit::recruit_delay 0x00511e70: unit type (0x0042d0a0, +0x3b) class +0x149: 4 / 5 -> 90;
//      otherwise kind +2 5 / 6 -> 30, else 50. Short return (callers store ax only).
namespace {
namespace game_b89_05 {

using joan::orig::helpers::at;
using joan::img::obj;
using joan::orig::helpers::s16;

void* const kTypes = obj(d::g_types);  // 0x0042d0a0

JOAN_COUNTER(c_delay, "Unit::recruit_delay");

struct H {
    // ------------------------------------------------------------ 0x00511e70
    std::int16_t recruit_delay() {
        c_delay.hit();
        const std::int16_t type = static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->type_id));
        const std::int32_t cls = s16(jo::view<std::uint16_t>(jo::as_unit_type(joan::orig::arg::as<void*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(type))))->rank));  // 0x0042d0a0
        if (cls != 3 && cls > 3 && cls <= 5) return 0x5a;
        // case 3 and the default run the same test
        if (s16(jo::view<std::uint16_t>(jo::as_unit_type(joan::orig::arg::as<void*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(type))))->unit_class)) == 5) return 0x1e;
        if (s16(jo::view<std::uint16_t>(jo::as_unit_type(joan::orig::arg::as<void*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(type))))->unit_class)) == 6) return 0x1e;
        return 0x32;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b89_05", name, addr, fn)
B("Unit::recruit_delay", 0x00511e70, &H::recruit_delay);
#undef B

}  // namespace game_b89_05
}  // namespace

// ---------------------------------------- from modules/game_b87_03.cpp (game/b87_03)
//
//      0x0051dfc0 Units::create(type) - ret 4 (this = unit table 0x00a47c08): new unit (0x4b4
//                 bytes, 0x004155e0), optional reuse from the 0x00a47ca8 pool, 0x004f8550,
//
// 0x0051dfc0 has an SEH frame only for operator new / ctor failure (delete on throw); the normal
namespace {
namespace game_b87_03 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::orig::helpers::at;
using joan::img::obj;

constexpr img::Addr kTypes(d::g_types);

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }
std::uint8_t* type_of(std::int16_t id) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(img::obj(kTypes))->get(joan::orig::arg::i16(sx16(id))));  // 0x0042d0a0 (reads the word)
}

JOAN_COUNTER(c_create, "Units::create");

struct H {
    // ------------------------------------------------------------------------ 0x0051dfc0
    std::int32_t create(std::int32_t type) {
        c_create.hit();
        auto make = []() -> std::uint8_t* {
            void* const p = joan::crt::operator_new(sizeof(jo::Squad));  // operator new (0x4b4 on x86)
            return p != nullptr ? joan::orig::arg::as<std::uint8_t*>(joan::orig::as_squad(p)->construct()) : nullptr;  // 0x004155e0
        };
        std::uint8_t* u = make();  // ebp-0x14
        std::int32_t done = 0;     // ebp-0x18
        if (global<std::int8_t>(d::g_a58f95) != 0) {
            void* const pool = joan::orig::arg::as<void*>(joan::orig::as_profile_list(obj(d::g_profile_list))->current());  // 0x004ca290
            if (pool != nullptr) {
                std::uint8_t* const def = type_of(static_cast<std::int16_t>(type));
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_profile(pool)->load_unit(joan::orig::arg::str(def + 4), joan::orig::arg::mptr(static_cast<void*>(u)))) !=
                    0) {  // 0x004c91c0
                    joan::orig::as_unit(u)->set_value_135(joan::orig::arg::i32(joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(u)->hit_points_max())));  // 0x004fe510(0x00523320)
                    joan::orig::as_unit(u)->set_145(joan::orig::arg::i32(joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(u)->value_149())));  // 0x004fe5d0(0x00523700)
                    const std::int32_t side = sx16(jo::as_unit(u)->side);         // ebp-0x20
                    jo::as_unit(u)->side = 0;
                    joan::orig::as_unit(u)->set_side(joan::orig::arg::i32(sx16(static_cast<std::int16_t>(side))));  // 0x00509250
                    jo::as_unit(u)->squad_kills = 0;
                    jo::as_unit(u)->squad_losses = 0;
                    done = 1;
                    return joan::orig::as_dynarray(this)->cursor();  // 0x00417520
                }
                joan::orig::as_units(this)->remove_51e1f0(joan::orig::arg::i32(sx16(jo::as_unit(u)->id)));  // 0x0051e1f0
                u = make();
            }
        }
        if (done == 0) joan::orig::as_unit(u)->init(joan::orig::arg::i32(type));  // 0x004f8550
        joan::orig::dynarray_b(this)->linkin(static_cast<void*>(&u));  // 0x005857a0
        jo::as_unit(u)->id = static_cast<std::int16_t>(joan::orig::as_dynarray(this)->cursor());
        return joan::orig::as_dynarray(this)->cursor();
    }
};

#define B87(name, addr, fn) JOAN_REPLACE("game/b87_03", name, addr, fn)
B87("Units::create", 0x0051dfc0, &H::create);
#undef B87

}  // namespace game_b87_03
}  // namespace

// ---------------------------------------- from modules/game_b82_10.cpp (game/b82_10)
namespace {
namespace game_b82_10 {

using joan::orig::helpers::at;

using i32 = std::int32_t;

using joan::call::thiscall;

using i16 = std::int16_t;

using joan::img::obj;

JOAN_COUNTER(c_release_parts, "Unit::release_parts");

struct H {
    // 0x004f07f0 Unit::release_parts.
    void release_parts() {
        c_release_parts.hit();
        void* const pool = obj(d::g_items);
        std::int32_t id = jo::self<jo::Unit>(this)->carried_a;
        joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
        if (joan::orig::as_units(pool)->list.empty_at(joan::orig::arg::i32(id)) == 0) joan::orig::as_units(pool)->remove_49f370(joan::orig::arg::i32(id));  // 0x00425d50 / 0x0049f370
        id = jo::self<jo::Unit>(this)->carried_b;
        joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::mptr(0));
        if (joan::orig::as_units(pool)->list.empty_at(joan::orig::arg::i32(id)) == 0) joan::orig::as_units(pool)->remove_49f370(joan::orig::arg::i32(id));

        if (jo::self<jo::Unit>(this)->weapon_model != nullptr) {
            joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(static_cast<std::int16_t>(jo::self<jo::Unit>(this)->weapon_model_id)));  // 0x004b16a0
            void* const o = jo::self<jo::Unit>(this)->weapon_model;
            joan::orig::as_scene(jo::view<void*>(jo::as_scene_object(o)->scene))->detach(joan::orig::arg::mptr(o));  // 0x005efdb0
            void* const d = jo::self<jo::Unit>(this)->weapon_model;
            if (d != nullptr) joan::orig::as_scene_object(d)->destroy(1u);  // 0x00420840 deleting dtor
            jo::self<jo::Unit>(this)->weapon_model = nullptr;
        }
        if (jo::self<jo::Unit>(this)->model_b != nullptr) {
            if (jo::view<void*>(jo::as_scene_object(jo::self<jo::Unit>(this)->model_b)->scene) != nullptr) {
                void* const o = jo::self<jo::Unit>(this)->model_b;
                joan::orig::as_scene(jo::view<void*>(jo::as_scene_object(o)->scene))->detach(joan::orig::arg::mptr(o));
            }
            void* const d = jo::self<jo::Unit>(this)->model_b;
            if (d != nullptr) joan::orig::as_scene_object(d)->destroy(1u);
            jo::self<jo::Unit>(this)->model_b = nullptr;
        }
        if (jo::self<jo::Unit>(this)->model != nullptr) {
            if (jo::view<void*>(jo::as_scene_object(jo::self<jo::Unit>(this)->model)->scene) != nullptr) {
                void* const o = jo::self<jo::Unit>(this)->model;
                joan::orig::as_scene(jo::view<void*>(jo::as_scene_object(o)->scene))->detach(joan::orig::arg::mptr(o));
            }
            void* const d = jo::self<jo::Unit>(this)->model;
            if (d != nullptr) joan::orig::as_scene_object(d)->destroy(1u);
            jo::self<jo::Unit>(this)->model = nullptr;
        }
        if (jo::self<jo::Unit>(this)->actor != nullptr) {
            if (jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene) != nullptr) {
                void* const o = jo::self<jo::Unit>(this)->actor;
                joan::orig::as_scene(jo::view<void*>(jo::as_scene_object(o)->scene))->detach(joan::orig::arg::mptr(o));
            }
            jo::self<jo::Unit>(this)->actor = nullptr;
            jo::self<jo::Unit>(this)->anim_set = nullptr;
        }
        if (jo::self<jo::Unit>(this)->actor_b != nullptr) {
            void* const d = jo::self<jo::Unit>(this)->actor_b;
            if (d != nullptr) joan::orig::as_scene_object(d)->destroy(1u);
            jo::self<jo::Unit>(this)->actor_b = nullptr;
            jo::self<jo::Unit>(this)->actor_def_b = 0;
        }
        jo::self<jo::Unit>(this)->alt_mesh_state = 0;
        if (jo::self<jo::Unit>(this)->actor_c != nullptr) {
            void* const d = jo::self<jo::Unit>(this)->actor_c;
            if (d != nullptr) joan::orig::as_scene_object(d)->destroy(1u);
            jo::self<jo::Unit>(this)->actor_c = nullptr;
            jo::self<jo::Unit>(this)->actor_def_c = 0;
            // MSVC interleaving: +0x3c5 is pushed before the 0x0042d0a0 call and consumed by
            // 0x00527690.
            const std::uint32_t arg = jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->collision_id);
            void* const type = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::self<jo::Unit>(this)->type_id)));  // 0x0042d0a0
            joan::orig::as_unit_type(type)->release_model_slot(joan::orig::arg::i32(arg));  // 0x00527690
        }
        if (jo::self<jo::Unit>(this)->draw != nullptr) {
            void* const d = jo::self<jo::Unit>(this)->draw;
            if (d != nullptr) joan::orig::as_unit_draw(d)->destroy(1u);  // 0x00416640 deleting dtor
            jo::self<jo::Unit>(this)->draw = nullptr;
        }
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_10", name, addr, fn)
B82("Unit::release_parts", 0x004f07f0, &H::release_parts);
#undef B82

}  // namespace game_b82_10
}  // namespace

// ---------------------------------------- from modules/game_b109_04.cpp (game/b109_04)
namespace {
namespace game_b109_04 {

using joan::call::import_call;
using joan::orig::helpers::at;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;
using i8 = std::int8_t;

using joan::call::thiscall;
using joan::orig::helpers::wadd;

using i16 = std::int16_t;

using joan::img::obj;

using joan::hook::global;

using joan::call::cdecl_call;

using joan::orig::helpers::wsub;
using joan::orig::helpers::gf;
using joan::orig::helpers::gd;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

void* const kSoldiers = obj(d::g_units);
void* const kModels = obj(d::g_physic_sys);
void* const kKinds = obj(d::g_types);

JOAN_COUNTER(c_steer, "Unit::steer_4f0b50");

u8* soldier(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 soldier_gone(u32 id) { return joan::orig::as_dynarray(kSoldiers)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* kind_rec(i16 k) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(static_cast<i32>(k)))); }  // 0x0042d0a0
// The unit's position (+0xe, three floats; laid out as bytes in Unit).
float* unit_pos(const void* u) { return reinterpret_cast<float*>(jo::as_unit(u)->position_); }

// ------------------------------------------------------------------------------------ H
struct H {
    u8* self() const { return static_cast<u8*>(const_cast<void*>(static_cast<const void*>(this))); }

    // ---------------------------------------------------------------- 0x004f0b50
    static void steer_set(u32* ang, float f8, double k, bool plus) {
        const Fpu fpu{true};
        const double v = plus ? fpu.add(static_cast<double>(f8), k) : fpu.sub(static_cast<double>(f8), k);
        joan::orig::as_angle(ang)->set(joan::orig::arg::f32(static_cast<float>(v)));  // 0x00576240
    }

    float ramp(float f10, i32 v) {  // +0x157 ramp between [0xa3cd00] and [0xa3cbdc]
        if (!(v > global<i32>(d::g_a3cd00))) return f10;
        const i32 c = v < global<i32>(d::g_a3cbdc) ? v : global<i32>(d::g_a3cbdc);
        const Fpu fpu{true};
        double t = fpu.sub(static_cast<double>(c), static_cast<double>(global<i32>(d::g_a3cd00)));
        t = fpu.div(t, static_cast<double>(wsub(global<i32>(d::g_a3cbdc), global<i32>(d::g_a3cd00))));
        t = fpu.add(t, static_cast<double>(gf(d::flt_1)));
        t = fpu.mul(t, static_cast<double>(f10));
        return static_cast<float>(t);
    }
    static double slow_bound(double dist) {
        const Fpu fpu{true};
        double x = fpu.mul(dist, static_cast<double>(gf(d::flt_1)));
        x = fpu.div(x, static_cast<double>(gf(d::flt_46_1)));
        x = fpu.mul(x, static_cast<double>(gf(d::flt_1000)));
        return fpu.div(x, static_cast<double>(global<i32>(d::g_frame_step_ms)));
    }
    // waypoint slow-down on o (+0x432 path, +0xe / +0x12 position)
    void approach(u8* o, float& f10, float& f28) {
        if (joan::orig::arg::as<i32>(joan::orig::as_list(&jo::as_unit(o)->path)->count_66e8a0()) == 0) return;  // 0x0066e8a0
        if (jo::as_unit(o)->layer == 0) return;
        const float* v = joan::orig::arg::as<float*>(joan::orig::as_list(&jo::as_unit(o)->path)->from_end(joan::orig::arg::i32(wadd(jo::as_unit(o)->path_index, 1))));  // 0x0066e8d0
        const float dx = v[0] - unit_pos(o)[0];
        const float dx2 = dx * (v[0] - unit_pos(o)[0]);
        const float dy = v[1] - unit_pos(o)[1];
        const float dy2 = dy * (v[1] - unit_pos(o)[1]);
        const double s = joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(dx2 + dy2))));  // sqrt
        f28 = static_cast<float>(s);
        const double x = slow_bound(s);
        if (!(x > static_cast<double>(f10))) f10 = static_cast<float>(slow_bound(static_cast<double>(f28)));
    }

    void steer_4f0b50() {
        c_steer.hit();
        const float f8 = joan::orig::arg::as<float>(joan::orig::as_ground_object(this)->heading());  // 0x004ff850
        u32 ang = 0;
        joan::orig::as_angle(&ang)->construct();            // 0x00576120
        joan::orig::as_angle(&ang)->set(joan::orig::arg::f32(f8));         // 0x00576240
        float f10 = 0.0f;
        i16 w14 = jo::self<jo::Unit>(this)->anim_kind;
        if (jo::view<i8>(jo::self<jo::Unit>(this)->use_action_alt) != 0) w14 = jo::self<jo::Unit>(this)->anim_kind_alt;
        switch (static_cast<i32>(w14)) {
            case 9: f10 = 1.0f; break;
            case 10: f10 = jo::as_unit_type(kind_rec(jo::self<jo::Unit>(this)->type_id))->speed_walk; break;
            case 11: f10 = jo::as_unit_type(kind_rec(jo::self<jo::Unit>(this)->type_id))->speed_run; break;
            case 14:
                f10 = jo::as_unit_type(kind_rec(jo::self<jo::Unit>(this)->type_id))->speed_strafe;
                steer_set(&ang, f8, gd(d::dbl_1_5707963267948966), true);
                break;
            case 15:
                f10 = jo::as_unit_type(kind_rec(jo::self<jo::Unit>(this)->type_id))->speed_strafe_b;
                steer_set(&ang, f8, gd(d::dbl_1_5707963267948966), false);
                break;
            case 16:
                f10 = jo::as_unit_type(kind_rec(jo::self<jo::Unit>(this)->type_id))->speed_back;
                steer_set(&ang, f8, gd(d::dbl_3_141592653589793), false);
                break;
            case 17:
                f10 = jo::as_unit_type(kind_rec(jo::self<jo::Unit>(this)->type_id))->speed_strafe;
                steer_set(&ang, f8, gd(d::dbl_0_7853981633974483), true);
                break;
            case 18:
                f10 = jo::as_unit_type(kind_rec(jo::self<jo::Unit>(this)->type_id))->speed_strafe;
                steer_set(&ang, f8, gd(d::dbl_0_7853981633974483), false);
                break;
            case 19:
                f10 = jo::as_unit_type(kind_rec(jo::self<jo::Unit>(this)->type_id))->speed_strafe;
                steer_set(&ang, f8, gd(d::dbl_2_356194490192345), true);
                break;
            case 20:
                f10 = jo::as_unit_type(kind_rec(jo::self<jo::Unit>(this)->type_id))->speed_strafe;
                steer_set(&ang, f8, gd(d::dbl_2_356194490192345), false);
                break;
            case 21: steer_set(&ang, f8, gd(d::dbl_3_141592653589793), false); break;
            case 22: steer_set(&ang, f8, gd(d::dbl_1_5707963267948966), false); break;
            case 23: steer_set(&ang, f8, gd(d::dbl_1_5707963267948966), true); break;
            case 68: f10 = 3.0f; break;
            case 69: f10 = -3.0f; break;
            case 70: f10 = 5.0f; break;
            default: break;
        }
        if (w14 != 0xe && w14 != 0xf && w14 != 0x10) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->leads()) == 0) {         // 0x0044a660
                u8* const p24 = joan::orig::arg::as<u8*>(joan::orig::as_unit(this)->leader());  // 0x004216d0
                if (p24 != nullptr)
                    f10 = ramp(f10, static_cast<i32>(jo::as_unit(p24)->stats[1]));
                else
                    f10 = ramp(f10, static_cast<i32>(jo::self<jo::Unit>(this)->stats[1]));
            } else {
                f10 = ramp(f10, static_cast<i32>(jo::self<jo::Unit>(this)->stats[1]));
            }
        }
        if (jo::self<jo::Unit>(this)->layer != 0) f10 = f10 * gf(d::g_a3cc70);
        f10 = f10 * gf(d::flt_1_3);
        if (soldier_gone(jo::view<u32>(jo::self<jo::Unit>(this)->rider_id)) != 0) {
            if (jo::self<jo::Unit>(this)->move_mode > 2) {
                f10 = f10 * gf(d::flt_9d7498);
            } else if (jo::self<jo::Unit>(this)->move_mode > 1) {
                if (!(f10 >= gf(d::flt_20))) f10 = f10 * gf(d::flt_1_4);
            }
        }
        float f28 = 0.0f;
        if (soldier_gone(jo::view<u32>(jo::self<jo::Unit>(this)->rider_id)) == 0)
            approach(soldier(jo::view<u32>(jo::self<jo::Unit>(this)->rider_id)), f10, f28);
        else
            approach(self(), f10, f28);
        const float f20 = joan::orig::arg::as<float>(joan::orig::as_angle(&ang)->cos()) * f10;  // 0x00576ac0
        const float f1c = joan::orig::arg::as<float>(joan::orig::as_angle(&ang)->sin()) * f10;  // 0x005768d0
        joan::orig::as_unit(this)->set_vec_6c(V3{f20, f1c, 0.0f});  // 0x00505d90
        if (jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) == 0) {
            const u32 m = joan::orig::arg::as<u32>(joan::orig::as_army(kModels)->fire_shot(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor), joan::orig::arg::bits(60.0f), joan::orig::arg::u32(// 0x006adbf0
                                        static_cast<i32>(jo::self<jo::Unit>(this)->layer)), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{1})));
            jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) = m;
        }
        if (jo::as_unit_type(kind_rec(jo::self<jo::Unit>(this)->type_id))->kind == 2)
            joan::orig::vec_scale_in_place(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor->impulse), 2.0f);  // 0x0060ce80
        void* const p3c = joan::orig::as_players(kModels)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)));  // 0x006af310
        if (jo::self<jo::Unit>(this)->scripted_move == 0) {
            if (jo::self<jo::Unit>(this)->last_update_ms != 0) {
                if (jo::self<jo::Unit>(this)->move_mode > 1) {
                    float f40 = 0.0f;
                    if (f28 > f10 || f10 > gf(d::k_9cf5c4)) {
                        const Fpu fpu{true};
                        const double t = static_cast<double>(global<u32>(d::g_frame_step_ms));
                        double e = fpu.mul(t, static_cast<double>(f28));
                        e = fpu.div(e, static_cast<double>(f10));
                        e = fpu.mul(e, static_cast<double>(gf(d::flt_0_8)));
                        f40 = !(t >= e) ? static_cast<float>(e) : static_cast<float>(t);
                        if (soldier_gone(jo::view<u32>(jo::self<jo::Unit>(this)->rider_id)) == 0)
                            f40 = !(f40 >= gf(d::flt_300)) ? f40 : 300.0f;
                        else
                            f40 = !(f40 >= gf(d::flt_1000)) ? f40 : 1000.0f;
                    }
                    const i32 n = ftol(static_cast<double>(f40));
                    joan::orig::as_body(p3c)->start(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor->impulse), joan::orig::arg::i32(global<u32>(d::g_now)), joan::orig::arg::i32(n));  // 0x006b10a0
                } else {
                    const u32 d = global<u32>(d::g_now) - jo::self<jo::Unit>(this)->last_update_ms;
                    const u32 n = d < global<u32>(d::g_frame_step_ms) ? global<u32>(d::g_now) - jo::self<jo::Unit>(this)->last_update_ms
                                                              : global<u32>(d::g_frame_step_ms);
                    joan::orig::as_body(p3c)->start(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor->impulse), joan::orig::arg::i32(global<u32>(d::g_now)), joan::orig::arg::i32(n));
                }
            } else {
                joan::orig::as_body(p3c)->start(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor->impulse), joan::orig::arg::i32(global<u32>(d::g_now)), joan::orig::arg::i32(global<u32>(d::g_frame_step_ms)));
            }
            return;
        }
        V3 tmp{0.0f, 0.0f, 0.0f};
        V3 v60 = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(jo::self<jo::Unit>(this)->position_), joan::orig::arg::ptr(jo::self<jo::Unit>(this)->move_target)));  // 0x0060cc90
        const float f44 = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &v60));     // length (st0 / double; stored as a float)
        if (!(f44 > gf(d::flt_5))) {
            const V3 dst{jo::self<jo::Unit>(this)->move_target[0], jo::self<jo::Unit>(this)->move_target[1], jo::self<jo::Unit>(this)->move_target[2]};
            joan::orig::as_ground_object(this)->set_position(dst);         // 0x004fedf0
            joan::orig::as_unit(this)->play_idle(i32{0}, i32{1});  // 0x00508b90
            return;
        }
        V3 n;
        n.x = f20 + unit_pos(this)[0];
        n.y = f1c + unit_pos(this)[1];
        n.z = (jo::self<jo::Unit>(this)->move_target[2] - unit_pos(this)[2]) * f10 / f44 + unit_pos(this)[2];
        joan::orig::as_ground_object(this)->set_position(n);  // 0x004fedf0
        float a54 = 0.0f;
        joan::orig::as_angle(&a54)->construct_577190();  // 0x00577190
        const double dy = static_cast<double>(jo::self<jo::Unit>(this)->move_target[1] - unit_pos(this)[1]);
        const double dx = static_cast<double>(jo::self<jo::Unit>(this)->move_target[0] - unit_pos(this)[0]);
        joan::orig::as_angle(&a54)->from_xy(joan::orig::arg::f64(dx), joan::orig::arg::f64(dy));         // 0x00577b90
        const float h = joan::orig::arg::as<float>(joan::orig::as_angle(&a54)->degrees());  // 0x00577450
        joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(h));                      // 0x0050abd0
    }
};

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_04", name, addr, fn)
B("Unit::steer_4f0b50", 0x004f0b50, &H::steer_4f0b50);
#undef B

}  // namespace game_b109_04
}  // namespace

// ---------------------------------------- from modules/game_b111_17.cpp (fix/uninit)
namespace {
namespace game_b111_17 {

using joan::orig::helpers::at;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i16 = std::int16_t;

using joan::img::obj;

using joan::hook::global;

using joan::call::cdecl_call;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using joan::img::str;

struct Blk12 {
    u32 d[3];
};

JOAN_COUNTER(c_set_model, "Actor::set_model_4f1760");

struct H {
    u8* me() { return reinterpret_cast<u8*>(this); }

    // -------------------------------------------------------------- 0x004f1760
    u8* light() { return reinterpret_cast<u8*>(jo::self<jo::MapObject>(this)->actor); }
    void set_model(u32 index) {
        if (jo::self<jo::MapObject>(this)->busy == index) return;
        c_set_model.hit();
        u8* const mgr = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::self<jo::MapObject>(this)->type_id)));  // 0x00425ce0
        const u32 old_res = joan::orig::arg::as<u32>(joan::orig::as_item_def(mgr)->sprite_id(joan::orig::arg::i32(jo::self<jo::MapObject>(this)->busy)));         // 0x004a3f20
        const u32 new_res = joan::orig::arg::as<u32>(joan::orig::as_item_def(mgr)->sprite_id(joan::orig::arg::i32(index)));                 // 0x004a3f20
        jo::Actor* const old_light = jo::self<jo::MapObject>(this)->actor;
        if (jo::self<jo::MapObject>(this)->actor != nullptr) {
            joan::orig::as_item_def(mgr)->acquire_anim(joan::orig::arg::i32(index), 0);  // 0x004a39f0
            virtual_call<u32>(this, 0x3c, index);
        }

        // fix/uninit: zeros when the light vanished before the save (original: stack garbage).
        u32 s5c = 0;
        Blk12 s60{};
        Blk12 s6c{};
        u32 sb8 = 0;
        u32 sbc[4] = {0, 0, 0, 0};
        u32 scc[6] = {0, 0, 0, 0, 0, 0};
        u32 se4[5] = {0, 0, 0, 0, 0};

        if (new_res != old_res) {
            for (i32 i = 0; i < 2; ++i) {
                if (joan::orig::as_dynarray(obj(d::g_particle_effect_array))->empty_at(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i + 2])) != 0)
                    continue;  // 0x0041bd90
                u8* l = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_particle_effect_array))->ptr_at(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i + 2])));  // 0x0041bd60
                if (jo::self<jo::MapObject>(this)->actor != nullptr && jo::as_model_obj(l)->owner == jo::self<jo::MapObject>(this)->actor) {  // the 0x00a57078 registry records are ModelObj (+0x12 owner)
                    joan::orig::as_effect(l)->set_owner(joan::orig::arg::mptr(0u), joan::orig::arg::mptr(0u));  // 0x0041b0d0
                    jo::as_model_obj(l)->owner = old_light;
                }
            }
            if (jo::self<jo::MapObject>(this)->actor != nullptr) {
                s5c = jo::view<u32>(jo::as_actor(light())->record);
                if (jo::view<u32>(jo::as_actor(light())->record) != 0) {
                    u8* r = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(light())->record))));  // 0x006af310
                    jo::as_body(r)->actor = nullptr;
                    jo::view<u32>(jo::as_actor(light())->record) = 0;
                }
                std::memcpy(&s60, jo::as_actor(light())->velocity, 12);
                std::memcpy(&s6c, jo::as_actor(light())->impulse, 12);
                sb8 = jo::as_actor(light())->aim_mode;
                const u32 kind = jo::as_actor(light())->aim_mode;
                if (kind == 1)
                    std::memcpy(sbc, jo::as_actor(light())->aim_sphere, 16);
                else if (kind == 2)
                    std::memcpy(scc, jo::as_actor(light())->aim_box, 24);
                else if (kind == 3)
                    std::memcpy(se4, &jo::as_actor(light())->collide_kind, 20);
                if (jo::as_actor(light())->scene != nullptr)
                    joan::orig::as_scene(jo::as_actor(light())->scene)->detach(joan::orig::arg::mptr(light()));  // 0x005efdb0
                jo::Actor* const d = jo::self<jo::MapObject>(this)->actor;
                if (d != nullptr) joan::orig::as_scene_object(d)->destroy(1u);  // 0x00420840 scalar deleting dtor
                jo::self<jo::MapObject>(this)->actor = nullptr;
            }
        }

        const u32 prev = jo::self<jo::MapObject>(this)->busy;
        jo::self<jo::MapObject>(this)->busy = index;
        if (new_res != old_res) {
            if (old_light != nullptr) {
                void* mem = joan::crt::operator_new(sizeof(jo::SceneObject));  // operator new (0x10c on x86)
                jo::Actor* lo = nullptr;
                if (mem != nullptr) lo = joan::orig::arg::as<jo::Actor*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20());  // 0x005aab20
                jo::self<jo::MapObject>(this)->actor = lo;
                {
                    u8* r = joan::orig::arg::as<u8*>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(new_res)));  // 0x0041a180
                    jo::as_actor(light())->def = jo::as_model_entry(r)->actor_def;
                }
                if (jo::view<u32>(jo::as_item_def(mgr)->slot_anims[jo::view<i32>(jo::self<jo::MapObject>(this)->busy)]) != 0) {
                    jo::AnimDef* const* r = joan::orig::arg::as<jo::AnimDef* const*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::as_item_def(mgr)->slot_anims[jo::view<i32>(jo::self<jo::MapObject>(this)->busy)]))));  // 0x004174f0
                    jo::as_actor(light())->anim = *r;
                }
                joan::orig::as_scene_object(light())->init();  // 0x005aab70
                {
                    u8* r = joan::orig::arg::as<u8*>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(new_res)));  // 0x0041a180
                    joan::orig::as_actor(light())->set_skin(joan::orig::arg::u32(jo::view<u32>(jo::as_model_entry(r)->anim_db_id)));        // 0x005add40
                }
                joan::orig::as_actor(light())->set_position_5ac310(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->pos));  // 0x005ac310
                jo::view<u32>(jo::as_actor(light())->scale) = jo::self<jo::MapObject>(this)->scale;
                joan::orig::as_scene_object(light())->set_yaw(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::MapObject>(this)->yaw_)));  // 0x005ac6a0
                joan::orig::as_scene_object(light())->set_roll(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::MapObject>(this)->roll_)));  // 0x005ac980
                if (static_cast<i32>(jo::view<i16>(jo::as_item_def(mgr)->type_id)) == 6)
                    jo::view<u32>(jo::as_actor(light())->kind) = 0xa;
                else
                    jo::view<u32>(jo::as_actor(light())->kind) = 1;
                jo::as_actor(light())->owner = me();
                if (jo::view<u32>(jo::as_item_def(mgr)->slot_anims[jo::view<i32>(jo::self<jo::MapObject>(this)->busy)]) != 0) {
                    joan::orig::as_actor(light())->start_anim(joan::orig::arg::mptr(jo::as_actor(light())->anim), str(d::s_default_9d3d60), joan::orig::arg::u32(global<u32>(d::g_now)), 1u, 0u, 0u);  // 0x005abe30
                    joan::orig::as_unit(this)->play_named_sound(str(d::s_default_9d3d60));     // 0x0049b4d0
                }
                if (jo::self<jo::MapObject>(this)->scene != nullptr && jo::self<jo::MapObject>(this)->actor != nullptr)
                    joan::orig::as_scene(jo::self<jo::MapObject>(this)->scene)->attach(joan::orig::arg::mptr(light()));  // 0x005efc50
                if (s5c != 0) {
                    jo::view<u32>(jo::as_actor(light())->record) = s5c;
                    u8* r = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(light())->record))));  // 0x006af310
                    jo::as_body(r)->actor = jo::self<jo::MapObject>(this)->actor;
                }
                std::memcpy(jo::as_actor(light())->velocity, &s60, 12);
                std::memcpy(jo::as_actor(light())->impulse, &s6c, 12);
                jo::as_actor(light())->aim_mode = sb8;
                const u32 kind = jo::as_actor(light())->aim_mode;
                if (kind == 1)
                    std::memcpy(jo::as_actor(light())->aim_sphere, sbc, 16);
                else if (kind == 2)
                    std::memcpy(jo::as_actor(light())->aim_box, scc, 24);
                else if (kind == 3)
                    std::memcpy(&jo::as_actor(light())->collide_kind, se4, 20);
            }
            for (i32 i = 0; i < 2; ++i) {
                if (joan::orig::as_dynarray(obj(d::g_particle_effect_array))->empty_at(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i + 2])) != 0)
                    continue;  // 0x0041bd90
                u8* l = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_particle_effect_array))->ptr_at(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i + 2])));  // 0x0041bd60
                if (jo::self<jo::MapObject>(this)->actor != nullptr && jo::as_model_obj(l)->owner == old_light) {
                    jo::as_model_obj(l)->owner = nullptr;
                    joan::orig::as_effect(l)->set_owner(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->actor), joan::orig::arg::mptr(0u));  // 0x0041b0d0
                }
            }
        }
        if (old_light != nullptr) {
            virtual_call<u32>(this, 0x40, index);
            joan::orig::as_res(mgr)->release_slot(joan::orig::arg::i32(prev), 0);  // 0x004a3e30
        }
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: saved light fields 0 when +0x86 vanished before the save (garbage stack restore).
FIX("Actor::set_model_4f1760", 0x004f1760, &H::set_model);
#undef FIX

}  // namespace game_b111_17
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

using joan::orig::helpers::at;

using u32 = std::uint32_t;

struct H {
    u32 get_1e6() { return jo::view<u32>(jo::self<jo::Unit>(this)->rider_id); }  // 0x004f2170
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("X::get_1e6", 0x004f2170, &H::get_1e6);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b106_14.cpp (game/b106_14)
namespace {
namespace game_b106_14 {

using joan::call::import_call;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;

using joan::call::thiscall;

using i16 = std::int16_t;

using joan::img::obj;

using joan::hook::global;

using joan::call::cdecl_call;


using joan::orig::helpers::as_float;
u32 as_bits(float f) {
    u32 b;
    std::memcpy(&b, &f, 4);
    return b;
}
// fcomp + fnstsw + test ah, 1: a < b or unordered.
bool lt_or_nan(float a, float b) { return !(a >= b); }

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12);

JOAN_COUNTER(c_turn_step, "Unit::turn_step");

constexpr double kPi = 3.141592653589793;  // qword 0x9cfbf8
constexpr double k180 = 180.0;             // qword 0x9cfbe8

// ---------------------------------------------------------------- 0x004f2190 helpers
i32 ftol_f(float v) { return joan::hook::x87::ftol(static_cast<double>(v)); }

// `fld 360.0f; fsub x; call _ftol`
i32 ftol_360_minus(float x) {
    return joan::hook::x87::ftol(joan::hook::x87::Fpu{true}.sub(360.0, static_cast<double>(x)));
}

// The heading wrap shared by both turn directions: dword compare with 360.0f, then (a > 360)
// a -= 360, (a < 0 or NaN) a += 360; returns the radians value pushed to 0x004ff870.
float wrap_to_radians(float a) {
    if (as_bits(a) == 0x43b40000u) a = as_float(0);
    if (a > global<float>(d::flt_360)) a = a - global<float>(d::flt_360);
    if (lt_or_nan(a, global<float>(d::k_9cf5c4))) a = a + global<float>(d::flt_360);
    const joan::hook::x87::Fpu fpu{true};
    return static_cast<float>(fpu.div(fpu.mul(static_cast<double>(a), kPi), k180));
}

void set_anim_if_idle(void* u) {
    if (joan::orig::arg::as<i16>(joan::orig::as_unit(u)->is_kind_special()) == 0)  // 0x00508ab0
        joan::orig::as_unit(u)->play_idle(0, 1);  // 0x00508b90
}

i16 kind_215(const void* u) { return jo::as_unit(u)->anim_kind; }

// One attached unit of a kind-8 owner (loop body at 0x4f2602 / 0x4f2aca).
void move_attached(void* self, void* owner, i32 i, float x, i32 turned, i16 anim) {
    const i32 id = jo::as_unit(self)->mask_slots[i].value;
    if (!(id > 0)) return;
    if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::as_unit(self)->mask_slots[i].value)) != 0) return;  // 0x00428b70
    void* u = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::as_unit(self)->mask_slots[i].value));  // 0x0041b3c0
    void* m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::as_unit(self)->actor->bone)->matrix());         // 0x0060ef80
    V3 v;
    import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&v),
                       static_cast<void*>(static_cast<std::uint8_t*>(jo::as_unit_type(owner)->buffers[0]) + i * 12), m);  // point_transform
    {
        const float h = joan::orig::arg::as<float>(joan::orig::as_ground_object(self)->heading());  // 0x004ff850
        const float d = static_cast<float*>(jo::as_unit_type(owner)->buffers[1])[i] * global<float>(d::flt_0_017453292);
        joan::orig::as_unit(u)->set_target(joan::orig::arg::f32(h + d));  // 0x004ff870
    }
    jo::PathSpace* a58 = nullptr;
    i32 a5c = 0, a60 = 0;
    if (jo::as_unit(u)->actor != nullptr && jo::view<u32>(jo::as_unit(u)->actor->record) != 0) {
        void* q = joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(u)->actor->record)));  // 0x006af310
        // a Shot record (the Body facade's +0x212 / +0x20e / +0x218 are Shot::state.sub / zone / cell on x86)
        a58 = static_cast<jo::PathSpace*>(jo::as_shot(q)->state.sub);
        a5c = static_cast<i32>(jo::as_shot(q)->state.zone);
        a60 = ftol_f(jo::view<float>(jo::as_shot(q)->state.cell));
    }
    const i16 hit = joan::orig::arg::as<i16>(joan::orig::as_players(obj(d::g_physic_sys))->find_slot(&a58, &a5c, joan::orig::arg::i16(0), joan::orig::arg::f32(v.x), joan::orig::arg::f32(v.y), joan::orig::arg::f32(v.z), 0, joan::orig::arg::f32(static_cast<float>(a60)), 0));  // 0x006ae8e0
    if (hit != 0) {
        void* p = joan::orig::arg::as<void*>(joan::orig::placements::find(joan::orig::arg::mptr(obj(d::g_placements)), joan::orig::arg::i32(global<u32>(d::g_a57bb2)), a58));  // 0x004a2b20
        const joan::hook::x87::Fpu fpu{true};
        const float dy = static_cast<float>(fpu.sub(static_cast<double>(v.y), static_cast<double>(jo::as_rworld_cell(p)->y)));
        const float dx = static_cast<float>(fpu.sub(static_cast<double>(v.x), static_cast<double>(jo::as_rworld_cell(p)->x)));
        const double g = joan::orig::arg::as<double>(joan::orig::as_terrain(a58)->height_66cae0(joan::orig::arg::i32(a5c), joan::orig::arg::f32(dx), joan::orig::arg::f32(dy)));  // 0x0066cae0
        v.z = static_cast<float>(fpu.add(g, static_cast<double>(*reinterpret_cast<float*>(
                                                static_cast<std::uint8_t*>(jo::as_unit_type(owner)->buffers[0]) + i * 12 + 8))));
    }
    joan::orig::as_ground_object(u)->set_position(v);  // 0x004fedf0
    const bool keep = ftol_f(x) != 0 && ftol_360_minus(x) != 0 && turned != 0;
    const bool flag1 = (static_cast<i32>(static_cast<std::int8_t*>(jo::as_unit_type(owner)->buffers[2])[i]) & 1) != 0;
    if (!keep) {
        if (flag1) {
            set_anim_if_idle(u);
        } else if (kind_215(u) != 0x31 && kind_215(u) != 0x37 && kind_215(u) != 0x35) {
            set_anim_if_idle(u);
        }
    } else {
        if (flag1) {
            if (kind_215(u) != anim) joan::orig::as_unit(u)->set_action(joan::orig::arg::i32(anim), 0, 1);  // 0x00501190
        } else if (jo::as_unit_type(owner)->kind == 0xb) {
            set_anim_if_idle(u);
        } else if (kind_215(u) != 0x31 && kind_215(u) != 0x37 && kind_215(u) != 0x35 && kind_215(u) != 0xa) {
            joan::orig::as_unit(u)->set_action(joan::orig::arg::i32(static_cast<i16>(0xa)), 0, 1);  // 0x00501190
        }
    }
}

// Tail shared by both directions (0x4f25a1.. / 0x4f2a69..).
void update_attached(void* self, void* owner, float x, i32 turned, i16 own_anim) {
    if (jo::as_unit_type(owner)->unit_class == 8) {
        const i16 anim = jo::as_unit_type(owner)->kind == 4 ? static_cast<i16>(0x56) : static_cast<i16>(0x54);
        const i32 n = jo::as_unit_type(owner)->slot_count;
        if (n > 0) {
            for (i32 i = 0; i < n; ++i) move_attached(self, owner, i, x, turned, anim);
        }
    } else if (jo::as_unit_type(owner)->unit_class == 9) {
        if (kind_215(self) != 0xa && kind_215(self) != 0xb) {
            if (ftol_f(x) == 0 || ftol_360_minus(x) == 0)
                joan::orig::as_unit(self)->play_idle(0, 1);  // 0x00508b90
            else
                joan::orig::as_unit(self)->set_action(joan::orig::arg::i32(own_anim), 0, 1);  // 0x00501190
        }
    }
}

struct H {
    // 0x004f2190
    i32 turn_step() {
        c_turn_step.hit();
        void* self = this;
        if (jo::as_unit(self)->weapon > 0) return 0;
        const float heading = joan::orig::arg::as<float>(joan::orig::as_ground_object(self)->heading());  // 0x004ff850
        const joan::hook::x87::Fpu fpu{true};
        const float cur = static_cast<float>(fpu.mul(fpu.div(static_cast<double>(heading), kPi), k180));
        {
            const i32 k = kind_215(self);
            if (k == 0x23 || k == 0x24 || k == 0x25 || k == 0x26 || k == 0x2a || k == 0x4a || k == 0x4b ||
                k == 0x4c || k == 0x4d) {
                joan::orig::as_unit(self)->set_heading(joan::orig::arg::f32(cur));  // 0x0050abd0
                return 0;
            }
        }
        void* owner = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(self)->type_id)));  // 0x0042d0a0
        if (jo::as_unit_type(owner)->unit_class == 8) {
            const i32 n = jo::as_unit_type(owner)->slot_count;
            if (n > 0) {
                bool ok = false;
                for (i32 i = 0; i < n; ++i) {
                    if (!((static_cast<i32>(static_cast<std::int8_t*>(jo::as_unit_type(owner)->buffers[2])[i]) & 1) != 0 ||
                          jo::as_unit_type(owner)->kind == 6 || jo::as_unit_type(owner)->kind == 0xa))
                        continue;
                    if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::as_unit(self)->mask_slots[i].value)) != 0) continue;  // 0x00428b70
                    void* u = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::as_unit(self)->mask_slots[i].value));  // 0x0041b3c0
                    if (jo::as_unit(u)->weapon == static_cast<i32>(jo::as_unit(self)->id)) {
                        ok = true;
                    } else {
                        ok = false;
                        break;
                    }
                }
                if (!ok) {
                    joan::orig::as_unit(self)->set_heading(joan::orig::arg::f32(cur));  // 0x0050abd0
                    return 0;
                }
            }
        }
        float a20, a24, tmp;  // Angle objects (one float each)
        joan::orig::as_angle(&a20)->construct_577190();           // 0x00577190 ctor
        joan::orig::as_angle(&a20)->set_wrapped(joan::orig::arg::f32(heading));   // 0x00577340
        joan::orig::as_angle(&a24)->construct_577190();           // 0x00577190 ctor
        joan::orig::as_angle(&a24)->set_degrees(joan::orig::arg::f32(joan::orig::arg::as<float>(joan::orig::as_unit(self)->float_1e())));  // 0x005773e0(0x0050ac00())
        void* d = joan::orig::arg::as<void*>(joan::orig::as_turn(&a24)->sub(&tmp, &a20));              // 0x005775b0
        float x = joan::orig::arg::as<float>(joan::orig::as_angle(d)->degrees());                             // 0x00577450
        if (lt_or_nan(x, global<float>(d::k_9cf5c4))) x = x + global<float>(d::flt_360);
        if (!(x > global<float>(d::k_9cf5c4))) return 0;
        if (!lt_or_nan(x, global<float>(d::flt_360))) return 0;

        float rate;
        switch (jo::as_unit_type(owner)->kind) {
            case 5: rate = 2.0f; break;
            case 6: rate = 5.0f; break;
            case 4: case 8: case 9: case 10: case 11: case 13: rate = 1.0f; break;
            default: rate = 20.0f; break;  // 7, 12 and out of range
        }
        const u32 elapsed = global<u32>(d::g_now) - jo::as_unit(self)->last_update_ms;
        float t = static_cast<float>(fpu.mul(fpu.div(static_cast<double>(elapsed), static_cast<double>(global<float>(d::flt_33))),
                                             static_cast<double>(rate)));
        if (jo::as_unit(self)->move_mode > 1) {
            if (!lt_or_nan(t, global<float>(d::flt_90))) t = as_float(0x42b40000u);
        } else {
            if (!lt_or_nan(t, global<float>(d::flt_25))) t = as_float(0x41c80000u);
        }

        if (lt_or_nan(x, global<float>(d::flt_180))) {
            const float s = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(x)));  // 0x00427d60
            const float step = lt_or_nan(t, s) ? t : s;
            const float r = wrap_to_radians(cur + step);
            const i32 turned = joan::orig::arg::as<i32>(joan::orig::as_unit(self)->set_target(joan::orig::arg::f32(r)));  // 0x004ff870
            update_attached(self, owner, x, turned, static_cast<i16>(0xc));
            return lt_or_nan(s, t) ? 0 : 1;
        }
        const float u = global<float>(d::flt_360) - x;
        const float step = lt_or_nan(t, u) ? t : u;
        const float r = wrap_to_radians(cur - step);
        const i32 turned = joan::orig::arg::as<i32>(joan::orig::as_unit(self)->set_target(joan::orig::arg::f32(r)));  // 0x004ff870
        update_attached(self, owner, x, turned, static_cast<i16>(0xd));
        return lt_or_nan(u, t) ? 0 : 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_14", name, addr, fn)
B("Unit::turn_step", 0x004f2190, &H::turn_step);
#undef B

}  // namespace game_b106_14
}  // namespace

// ---------------------------------------- from modules/game_b88_06_2.cpp (game/b88_06)
namespace {
namespace game_b88_06_2 {

using joan::orig::helpers::at;

struct H {
    std::int32_t get_1f5() { return jo::self<jo::Unit>(this)->mount_ref; }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Obj::get_1f5", 0x004f2f60, &H::get_1f5);
#undef M

}  // namespace game_b88_06_2
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
namespace {
namespace game_b102_05 {

using joan::call::cdecl_call;

void JOAN_CDECL init_4f3d10() { joan::orig::init_pi_4f4f10(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("init_4f3d10", 0x004f3d10, &init_4f3d10);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

using joan::hook::global;

using joan::call::cdecl_call;

void JOAN_CDECL init_pi_4f4f10() {  // 0x004f4f10
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));      // atan
    global<float>(d::g_pi_a3cb6c) = static_cast<float>(t * 4.0);  // qword 0x9cf788 = 4.0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("init_pi_4f4f10", 0x004f4f10, &init_pi_4f4f10);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b101_07.cpp (game/b101_07)
namespace {
namespace game_b101_07 {

using joan::call::cdecl_call;

// ---------------------------------------------------------------------------- static initialisers
static void JOAN_CDECL init_a3cb74() { joan::orig::init_a3cb74(); }  // 0x004f4f40

#define B(name, addr, fn) JOAN_REPLACE("game/b101_07", name, addr, fn)
B("init_a3cb74", 0x004f4f40, &init_a3cb74);
#undef B

}  // namespace game_b101_07
}  // namespace

// ---------------------------------------- from modules/game_b109_06.cpp (game/b109_06)
namespace {
namespace game_b109_06 {

using joan::orig::helpers::at;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::x87::Fpu;
using i8 = std::int8_t;

using joan::call::thiscall;

using i16 = std::int16_t;

using joan::img::obj;

using joan::hook::global;

using joan::call::cdecl_call;

float as_float(u32 bits) {
    float f = 0.0f;
    std::memcpy(&f, &bits, 4);
    return f;
}

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "vec3");

void* const kTypes = obj(d::g_types);
void* const kRandom = obj(d::g_misc);

u8* unit_type(void* u) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(u)->type_id))); }  // 0x0042d0a0
i32 random(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540

JOAN_COUNTER(c_wander, "Unit::wander_4f7bf0");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // ------------------------------------------------------------------ 0x004f7bf0
    u32 wander() {
        c_wander.hit();
        const Fpu fpu{true};
        V3 pos{0.0f, 0.0f, 0.0f};
        joan::orig::as_unit(this)->position(&pos);  // 0x004fedb0
        u32 angle = 0;
        joan::orig::as_angle(&angle)->construct();  // 0x00576120
        // pos.x/y = cos/sin(angle) * (random(100) + 20) + base
        auto offset = [&](float bx, float by) {
            const float rx = static_cast<float>(random(100) + 0x14);
            const float cx = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos());  // 0x00576ac0
            const float px = cx * rx;
            pos.x = px + bx;
            const float ry = static_cast<float>(random(100) + 0x14);
            const float sy = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin());  // 0x005768d0
            const float py = sy * ry;
            pos.y = py + by;
        };
        auto can_stand = [&]() { return joan::orig::arg::as<i8>(joan::orig::as_unit(this)->cell_has_path_data(joan::orig::arg::bits(pos.x), joan::orig::arg::bits(pos.y), joan::orig::arg::bits(pos.z))) != 0; };  // 0x00523e50
        auto go = [&]() { return joan::orig::arg::as<i8>(joan::orig::as_unit(this)->move_to(pos, joan::orig::arg::u32(i32{0x64}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}))); };  // 0x00505dd0
        const u32 state = jo::self<jo::Unit>(this)->sidestep_phase;
        if (state <= 1) {
            const i32 r = random(0x19);
            float a24 = static_cast<float>(fpu.add(fpu.mul(static_cast<double>(r), global<float>(d::flt_0_017453292)),
                                                   global<double>(d::dbl_1_5707963267948966)));
            u32 ta = 0, tb = 0, diff = 0;
            joan::orig::as_angle(&ta)->construct_577190();  // 0x00577190
            joan::orig::as_angle(&tb)->construct_577190();
            joan::orig::as_angle(&ta)->set_wrapped(joan::orig::arg::f32(jo::view<u32>(jo::self<jo::Unit>(this)->aim_heading)));  // 0x00577340
            joan::orig::as_angle(&tb)->set_wrapped(joan::orig::arg::f32(jo::view<u32>(jo::self<jo::Unit>(this)->heading)));
            void* d = joan::orig::arg::as<void*>(joan::orig::as_turn(&ta)->sub(&diff, &tb));  // 0x005775b0
            const float deg = joan::orig::arg::as<float>(joan::orig::as_angle(d)->degrees());       // 0x00577450
            if (!(deg >= global<float>(d::flt_180))) a24 = -a24;
            const float heading = a24 + jo::self<jo::Unit>(this)->aim_heading;
            joan::orig::as_angle(&angle)->set(joan::orig::arg::f32(heading));  // 0x00576240
            offset(pos.x, pos.y);
            if (!can_stand()) return finish();
            if (go() == 0) return finish();
            void* const list = &jo::self<jo::Unit>(this)->path;
            if (joan::orig::arg::as<i32>(joan::orig::as_list(list)->count_66e8a0()) == 0) return finish();  // 0x0066e8a0
            u8* const last = joan::orig::arg::as<u8*>(joan::orig::as_path(list)->first());           // 0x0066e940
            // A PointList on the stack (0x44 bytes on x86, 0x41 of them used); x64's PointList is 0x45 bytes (its DynArray
            // holds pointers), so the x86-sized shell was overrun by the constructor and the path finder.
            alignas(alignof(void*)) u8 path[sizeof(jo::PointList) > 0x44 ? sizeof(jo::PointList) : 0x44];
            std::memset(path, 0, sizeof path);
            joan::orig::as_path(path)->construct();                     // 0x0066e4b0
            joan::orig::as_angle(&angle)->set(joan::orig::arg::f32(jo::view<u32>(jo::self<jo::Unit>(this)->aim_heading)));       // 0x00576240
            offset(jo::as_vec3f(last)->x, jo::as_vec3f(last)->y);
            void* const finder = joan::orig::arg::as<void*>(joan::orig::as_world_cell(jo::self<jo::Unit>(this)->cell)->grid(joan::orig::arg::i16(jo::self<jo::Unit>(this)->layer)));  // 0x005600c0
            auto rel = [&](float v, i32 jo::Cell::*origin) {  // v - (float)int cell origin
                return static_cast<float>(fpu.sub(v, static_cast<double>(jo::self<jo::Unit>(this)->cell->*origin)));
            };
            bool special = false;
            if (jo::as_unit_type(unit_type(this))->unit_class == 8) special = jo::as_unit_type(unit_type(this))->kind != 0xb;
            if (special) {
                u8* const t = unit_type(this);
                float a80 = static_cast<float>(static_cast<i32>(global<u32>(d::g_unit_body_r) << 1));
                const i32 kind = jo::view<i32>(jo::as_unit_type(t)->shape_kind);
                if (kind == 1) {
                    a80 = as_float(jo::view<u32>(jo::as_unit_type(t)->shape_sphere[3]));
                } else if (kind == 2) {
                    auto fabs_ = [&](i32 k) {
                        return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(jo::as_unit_type(t)->shape_box[k]))));  // CRT fabs
                    };
                    const double c4 = fabs_(0);
                    const double e3 = fabs_(3);
                    const double cc = !(e3 >= c4) ? fabs_(0) : fabs_(3);
                    a80 = static_cast<float>(cc);
                    const double d4 = cc;
                    const double db = fabs_(1);
                    const double dc = !(db >= d4) ? static_cast<double>(a80) : fabs_(1);
                    a80 = static_cast<float>(dc);
                    const double e4 = dc;
                    const double e7 = fabs_(4);
                    const double ec = !(e7 >= e4) ? static_cast<double>(a80) : fabs_(4);
                    a80 = static_cast<float>(ec);
                } else if (kind == 3) {
                    a80 = as_float(jo::view<u32>(jo::as_unit_type(t)->shape_cylinder[3]));
                }
                a80 = a80 * global<float>(d::flt_3);
                joan::orig::as_path_space(finder)->find_path(joan::orig::arg::f32(rel(jo::as_vec3f(last)->x, &jo::Cell::origin_x)), joan::orig::arg::f32(rel(jo::as_vec3f(last)->y, &jo::Cell::origin_y)), joan::orig::arg::f32(jo::view<u32>(jo::as_vec3f(last)->z)), joan::orig::arg::f32(rel(pos.x, &jo::Cell::origin_x)), joan::orig::arg::f32(rel(pos.y, &jo::Cell::origin_y)), joan::orig::arg::f32(pos.z), joan::orig::arg::mptr(static_cast<void*>(path)), joan::orig::arg::bits(a80), i32{0}, i32{0x64}, joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(u32{0x3f000000}));  // 0x0066cb40
            } else {
                const float lim = static_cast<float>(global<i32>(d::g_unit_body_r));
                joan::orig::as_path_space(finder)->find_path(joan::orig::arg::f32(rel(jo::as_vec3f(last)->x, &jo::Cell::origin_x)), joan::orig::arg::f32(rel(jo::as_vec3f(last)->y, &jo::Cell::origin_y)), joan::orig::arg::f32(jo::view<u32>(jo::as_vec3f(last)->z)), joan::orig::arg::f32(rel(pos.x, &jo::Cell::origin_x)), joan::orig::arg::f32(rel(pos.y, &jo::Cell::origin_y)), joan::orig::arg::f32(pos.z), joan::orig::arg::mptr(static_cast<void*>(path)), joan::orig::arg::bits(lim), i32{0}, i32{0x64}, joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(u32{0xbf800000}));  // 0x0066cb40
            }
            const float py = static_cast<float>(jo::self<jo::Unit>(this)->cell->origin_y);
            const float px = static_cast<float>(jo::self<jo::Unit>(this)->cell->origin_x);
            joan::orig::as_path(static_cast<void*>(path))->translate(joan::orig::arg::f32(px), joan::orig::arg::f32(py), joan::orig::arg::f32(i32{0}));  // 0x0066fa00
            const i32 n = joan::orig::arg::as<i32>(joan::orig::as_list(static_cast<void*>(path))->count_66e8a0());  // 0x0066e8a0
            for (i32 i = 1; i <= n; ++i) {
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(static_cast<void*>(path))->from_end(joan::orig::arg::i32(i)));  // 0x0066e8d0
                joan::orig::as_path(&jo::self<jo::Unit>(this)->path)->insert_front(e);                           // 0x0066e870
            }
            joan::orig::as_wrap(static_cast<void*>(path))->f_4f0500();  // 0x004f0500 (dtor)
            return finish();
        }
        if (state == 2 || state == 3) {
            const double base = state == 2 ? fpu.sub(jo::self<jo::Unit>(this)->aim_heading, global<double>(d::dbl_1_5707963267948966))
                                           : fpu.add(jo::self<jo::Unit>(this)->aim_heading, global<double>(d::dbl_1_5707963267948966));
            const i32 r = random(0x2d);
            const double turn = fpu.mul(static_cast<double>(r), global<float>(d::flt_0_017453292));
            const float heading = static_cast<float>(state == 2 ? fpu.sub(base, turn) : fpu.add(turn, base));
            joan::orig::as_angle(&angle)->set(joan::orig::arg::f32(heading));  // 0x00576240
            offset(pos.x, pos.y);
            if (can_stand()) go();
        }
        return finish();
    }
    u32 finish() {
        jo::self<jo::Unit>(this)->i32_3f2 = 1;
        const i32 v = static_cast<i32>(jo::self<jo::Unit>(this)->sidestep_phase + 1u) % 2;
        jo::view<i32>(jo::self<jo::Unit>(this)->sidestep_phase) = v;
        return static_cast<u32>(v);
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_06", name, addr, fn)
B("Unit::wander_4f7bf0", 0x004f7bf0, &H::wander);
#undef B

}  // namespace game_b109_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Unit::cleanup() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b86_05::H::cleanup>(this)); }  // 0x461f10
auto joan::orig::Unit::release_parts() -> void { joan::bind::member<&::game_b82_10::H::release_parts>(this); }  // 0x4f07f0
auto joan::orig::Unit::steer_4f0b50() -> void { joan::bind::member<&::game_b109_04::H::steer_4f0b50>(this); }  // 0x4f0b50
auto joan::orig::Unit::get_1e6() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_04::H::get_1e6>(this)); }  // 0x4f2170
auto joan::orig::Unit::turn_step() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b106_14::H::turn_step>(this)); }  // 0x4f2190
auto joan::orig::Unit::get_1f5() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b88_06_2::H::get_1f5>(this)); }  // 0x4f2f60
auto joan::orig::init_pi_4f4f10() -> void { joan::bind::free<&::game_b99_05::init_pi_4f4f10>(); }  // 0x4f4f10
auto joan::orig::Unit::wander_4f7bf0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b109_06::H::wander>(this)); }  // 0x4f7bf0
auto joan::orig::Unit::init(std::int32_t a0) -> void { joan::bind::member<&::game_b88_02::H::init>(this, a0); }  // 0x4f8550
auto joan::orig::Unit::set_11c(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_11c>(this, a0)); }  // 0x4fe640
auto joan::orig::Unit::set_120(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_120>(this, a0)); }  // 0x4fe670
auto joan::orig::Unit::set_16f(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_16f>(this, a0)); }  // 0x4fe7a0
auto joan::orig::Unit::set_173(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_173>(this, a0)); }  // 0x4fe7d0
auto joan::orig::Unit::set_177(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_177>(this, a0)); }  // 0x4fe800
auto joan::orig::Unit::set_17b(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_17b>(this, a0)); }  // 0x4feb80
auto joan::orig::Unit::set_17f(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_17f>(this, a0)); }  // 0x4febb0
auto joan::orig::Unit::set_183(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_183>(this, a0)); }  // 0x4febe0
auto joan::orig::Unit::set_187(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_187>(this, a0)); }  // 0x4fec10
auto joan::orig::Unit::set_18b(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_18b>(this, a0)); }  // 0x4fec40
auto joan::orig::Unit::set_18f(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_18f>(this, a0)); }  // 0x4fec70
auto joan::orig::Unit::set_193(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_193>(this, a0)); }  // 0x4feca0
auto joan::orig::Unit::set_197(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_197>(this, a0)); }  // 0x4fecd0
auto joan::orig::Unit::recruit_delay() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b89_05::H::recruit_delay>(this)); }  // 0x511e70
auto joan::orig::Unit::apply_kind_links(std::int32_t a0) -> void { joan::bind::member<&::game_b89_06::H::apply_kind_links>(this, a0); }  // 0x523240
