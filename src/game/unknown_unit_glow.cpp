// game/unknown_unit_glow.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/misc_0049c080.cpp; re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their
// original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::img::obj;

using joan::hook::global;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
namespace {
namespace game_b84_06 {

JOAN_COUNTER(c_psounds, "Group::pause_sounds");

void* const kSounds = obj(d::g_sfx_player);

using joan::orig::helpers::count_of;  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------ 0x0049c080
    void pause_sounds(std::int8_t flag) {
        c_psounds.hit();
        const std::uintptr_t fn = flag != 0 ? fn::code_value(fn::Sounds_pause) /* 0x004ced90 */ : fn::code_value(fn::Sounds_resume) /* 0x00470c90 */;
        for (std::int32_t i = count_of(this); i > 0; --i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;  // 0x004af300
            std::uint8_t* const m = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(i)));  // 0x004af2d0
            if (jo::view<std::int32_t>(jo::as_message(m)->sound_id) != 0) thiscall<std::uint32_t>(fn, kSounds, jo::view<std::int32_t>(jo::as_message(m)->sound_id));
        }
    }

    // ------------------------------------------------------------------ 0x0049cac0
    void set_6e(std::uint32_t v) {
        jo::self<jo::MapObject>(this)->scale = v;
        if (jo::self<jo::MapObject>(this)->actor != nullptr) jo::view<std::uint32_t>(jo::self<jo::MapObject>(this)->actor->scale) = v;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("Group::pause_sounds", 0x0049c080, &H::pause_sounds);
B("Obj::set_6e", 0x0049cac0, &H::set_6e);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {

using u16 = std::uint16_t;

JOAN_COUNTER(c_release_92, "Obj::release_92");

struct H {
    // ------------------------------------------------------------------------ 0x0049c1a0
    void release_92() {
        c_release_92.hit();
        void* const pool = obj(d::g_lights);
        if (joan::orig::arg::as<i32>(joan::orig::as_list(pool)->is_null_at_4(static_cast<i32>(jo::self<jo::MapObject>(this)->light_slot))) != 0) return;  // 0x00484740
        void* const item = joan::orig::arg::as<void*>(joan::orig::as_list(pool)->get_at_4(static_cast<i32>(jo::self<jo::MapObject>(this)->light_slot)));  // 0x00484710
        u8* const node = joan::orig::arg::as<u8*>(joan::orig::as_children(item)->get(i32{1}));                              // 0x004b0220
        if (jo::view<void*>(jo::as_light(node)->bone_) != nullptr) joan::orig::as_bone(jo::view<void*>(jo::as_light(node)->bone_))->set_parent(joan::orig::arg::mptr(i32{0}));  // 0x0060ec70
        joan::orig::as_scene(jo::view<void*>(jo::as_world(global<u8*>(d::g_world))->scene))->remove_light(joan::orig::arg::mptr(node));  // 0x005eb700
        joan::orig::as_light_faders(pool)->remove_group(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::MapObject>(this)->light_slot)));            // 0x004b0700
        jo::view<u16>(jo::self<jo::MapObject>(this)->light_slot) = 0;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::release_92", 0x0049c1a0, &H::release_92);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

using i8 = std::int8_t;

void* const kGroups = obj(d::g_nations);

void* const kTypes = obj(d::g_types);

JOAN_COUNTER(c_find_unit, "Obj::find_unit");

struct H {
    // ------------------------------------------------------------------ 0x0049c240
    i32 find_unit(const char* kind) {
        c_find_unit.hit();
        if (jo::view<void*>(jo::self<jo::Place>(this)->world) == nullptr) return 0;
        void* g1 = nullptr;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kGroups)->is_null(joan::orig::arg::i32(jo::self<jo::Place>(this)->nation))) == 0)    // 0x00484870
            g1 = joan::orig::as_dynarray(kGroups)->ptr_at(joan::orig::arg::i32(jo::self<jo::Place>(this)->nation));  // 0x0047b600
        float* const area = jo::self<jo::Place>(this)->box;
        i32 count;
        u8** const cells = joan::orig::arg::as<u8**>(joan::orig::as_world(jo::view<void*>(jo::self<jo::Place>(this)->world))->cells_in_box(joan::orig::arg::mptr(area), &count));  // 0x00429090
        for (i32 i = 0; i < count; ++i) {
            u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(cells[i])->get_8c());  // 0x0054ee10
            u8* p = first;
            if (p == nullptr) continue;
            do {
                if (match(p, kind, area, g1)) return static_cast<i32>(jo::as_unit(p)->id);
                p = jo::view<u8*>(jo::as_unit(p)->cell_prev);
            } while (p != nullptr && p != first);
        }
        return 0;
    }
    bool match(u8* p, const char* kind, float* area, void* g1) {
        if (!(jo::as_unit(p)->hit_points > 0) && jo::as_unit(p)->state_120 == 0) return false;
        if (joan::orig::arg::as<u8>(joan::orig::as_aabb(area)->contains(joan::orig::arg::ptr(jo::as_unit(p)->position_))) == 0) return false;  // 0x0060d5f0
        const i32 c = static_cast<i32>(static_cast<i8>(*kind));
        if (c == 0x50) return static_cast<i32>(jo::as_unit(p)->id) == global<i32>(d::g_player_unit_recno);
        u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(p)->type_id))));  // 0x0042d0a0
        if ((jo::as_unit_type(type)->trigger_flags & 0x2443u) == 0) return false;
        if (c == 0x45)
            return joan::orig::arg::as<i32>(joan::orig::as_players(kGroups)->hostile(joan::orig::arg::i32(jo::self<jo::Place>(this)->nation), static_cast<i32>(jo::as_unit(p)->side))) != 0;  // 0x004b6860
        if (c != 0x46) return false;
        void* g2 = nullptr;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kGroups)->is_null(static_cast<i32>(jo::as_unit(p)->side))) == 0)    // 0x00484870
            g2 = joan::orig::as_dynarray(kGroups)->ptr_at(static_cast<i32>(jo::as_unit(p)->side));  // 0x0047b600
        if (g1 == nullptr || g2 == nullptr) return false;
        if (g1 == g2) return true;
        const i8 rel = joan::orig::arg::as<i8>(joan::orig::as_table(g1)->byte_58(joan::orig::arg::i16(static_cast<i32>(jo::as_nation(g2)->id))));  // 0x004b4360
        return !(static_cast<i32>(rel) < 4);
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Obj::find_unit", 0x0049c240, &H::find_unit);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

JOAN_COUNTER(c_count_units, "Town::count_units");

struct H {
    // ------------------------------------------------------------------------ 0x0049c470
    i32 count_units(u32 id) {
        c_count_units.hit();
        void* const kTown = obj(d::g_units);
        void* const kMap = obj(d::g_nations);
        if (joan::orig::as_dynarray(kTown)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kTown)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        if (jo::self<jo::Region>(this)->world == nullptr) return 0;
        i32 cnt = 0;
        i16* q = nullptr;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kMap)->is_null(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Region>(this)->nation)))) == 0)    // 0x00484870
            q = joan::orig::arg::as<i16*>(joan::orig::as_dynarray(kMap)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Region>(this)->nation))));  // 0x0047b600
        i32 n;
        u8** const arr = joan::orig::arg::as<u8**>(joan::orig::as_world(jo::view<void*>(jo::self<jo::Region>(this)->world))->cells_in_box(joan::orig::arg::mptr(jo::self<jo::Region>(this)->box), &n));  // 0x00429090
        for (i32 i = 0; i < n; ++i) {
            u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(arr[i])->get_8c());  // 0x0054ee10
            u8* cur = first;
            if (cur == nullptr) continue;
            do {
                if (jo::as_unit(cur)->hit_points > 0 || jo::view<u32>(jo::as_unit(cur)->state_120) != 0) {
                    if ((joan::orig::arg::as<u32>(joan::orig::as_aabb(jo::self<jo::Region>(this)->box)->contains(joan::orig::arg::ptr(jo::as_unit(cur)->position_))) & 0xffu) != 0) {  // 0x0060d5f0
                        if (static_cast<i32>(jo::as_unit(cur)->side) != 0 &&
                            joan::orig::arg::as<i32>(joan::orig::as_players(kMap)->hostile(static_cast<i32>(jo::as_unit(t)->side), static_cast<i32>(jo::as_unit(cur)->side))) != 0)  // 0x004b6860
                            ++cnt;
                    }
                }
                cur = jo::view<u8*>(jo::as_unit(cur)->cell_prev);
            } while (cur != nullptr && cur != first);
        }
        u8* r = joan::orig::arg::as<u8*>(joan::orig::as_place(this)->facility_head());  // 0x004c48b0
        if (static_cast<i32>(*q) == 0) return cnt;
        if (joan::orig::arg::as<i32>(joan::orig::as_players(kMap)->hostile(static_cast<i32>(*q), static_cast<i32>(jo::as_unit(t)->side))) == 0) return cnt;
        while (r != nullptr) {
            if (static_cast<i32>(jo::as_facility(r)->kind) == 1) {
                if (static_cast<i32>(jo::as_building(virtual_call<u8*>(r, 0x18))->unit_kind) != 0) ++cnt;  // slot 0x18: the building
                for (i32 k = 0; k < static_cast<i32>(jo::as_building(virtual_call<u8*>(r, 0x18))->queue_n); ++k)
                    cnt += static_cast<i32>(jo::as_building(virtual_call<u8*>(r, 0x18))->queue_count[k]);
            }
            r = jo::view<u8*>(jo::as_facility(r)->prev);
        }
        return cnt;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("Town::count_units", 0x0049c470, &H::count_units);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b101_04.cpp (game/b101_04)
namespace {
namespace game_b101_04 {

void* const kSlots = obj(d::g_slots_a3ce90);

struct H {
    // ------------------------------------------------------------------ 0x0049c920
    u32 release_slot() {
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_dynarray(kSlots)->empty_at(static_cast<i32>(jo::self<jo::MapObject>(this)->glow_slot)));  // 0x00487bb0
        if (r != 0) return r;
        const u32 r2 = joan::orig::arg::as<u32>(joan::orig::as_table(kSlots)->remove(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::MapObject>(this)->glow_slot))));  // 0x004878f0
        jo::self<jo::MapObject>(this)->glow_slot = 0;
        return r2;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_04", name, addr, fn)
B("Obj::release_slot", 0x0049c920, &H::release_slot);
#undef B

}  // namespace game_b101_04
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

JOAN_COUNTER(c_find_49c970, "Table::find_49c970");

struct H {
    // ------------------------------------------------------------------ 0x0049c970
    i16 find_49c970() {
        c_find_49c970.hit();
        void* const tab = obj(d::g_date);
        const u32 a = joan::orig::arg::as<u32>(joan::orig::as_date_info(tab)->year_4957e0(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Calendar>(this)->day))));  // 0x004957e0
        const u32 b = joan::orig::arg::as<u32>(joan::orig::as_date_info(tab)->month_495810(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Calendar>(this)->day))));  // 0x00495810
        const u32 c = joan::orig::arg::as<u32>(joan::orig::as_pool(tab)->alloc44(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Calendar>(this)->day))));  // 0x00495840
        const u32 d = joan::orig::arg::as<u32>(joan::orig::as_date(tab)->day_of_week(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Calendar>(this)->day))));  // 0x0057fc00
        for (i16 i = 1; static_cast<i32>(i) <= static_cast<i32>(global<i16>(d::g_a56f5c)); i = static_cast<i16>(i + 1)) {
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_sdate_res(obj(d::g_sdate_res))->entry(joan::orig::arg::u32(i)));  // 0x004958f0
            if (jo::as_sdate_entry(e)->year != 0 && jo::as_sdate_entry(e)->year != a) continue;
            if (jo::as_sdate_entry(e)->month != 0 && jo::as_sdate_entry(e)->month != b) continue;
            if (jo::as_sdate_entry(e)->day != 0 && jo::as_sdate_entry(e)->day != c) continue;
            if (jo::as_sdate_entry(e)->weekday != 0 && jo::as_sdate_entry(e)->weekday != d) continue;
            return jo::view<i16>(jo::as_sdate_entry(e)->id);
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Table::find_49c970", 0x0049c970, &H::find_49c970);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03 {

struct H {
    // ------------------------------------------------------------------ 0x0049ca90 / 0x004c2240
    float value_49ca90() { return jo::view<float>(jo::self<jo::MapObject>(this)->scale); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Obj::value_49ca90", 0x0049ca90, &H::value_49ca90);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::img::str;

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

void* const kTypes = obj(d::g_item_res);

u8* type_record(u16 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0

struct Vec3 {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_setup_glow, "Unit::setup_glow");

struct H {
    // ------------------------------------------------------------------ 0x0049c710
    std::uintptr_t setup_glow() {
        c_setup_glow.hit();
        if (jo::self<jo::MapObject>(this)->glow_slot != 0) return reinterpret_cast<std::uintptr_t>(this);
        if (jo::self<jo::MapObject>(this)->actor == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        u8* const rec = type_record(jo::view<u16>(jo::self<jo::MapObject>(this)->type_id));
        if (static_cast<i32>(jo::view<i8>(jo::as_item_def(rec)->has_light2d)) == 0) return 0;
        float px = jo::self<jo::MapObject>(this)->pos[0], py = jo::self<jo::MapObject>(this)->pos[1], pz = jo::self<jo::MapObject>(this)->pos[2];
        u32 angle = 0;
        joan::orig::as_angle(&angle)->construct();                         // 0x00576120
        const float heading = joan::orig::arg::as<float>(joan::orig::as_map_object(this)->yaw());     // 0x00499e20
        joan::orig::as_angle(&angle)->set(joan::orig::arg::f32(heading));                  // 0x00576240
        float s = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos());               // 0x00576ac0
        const float v1c = s * jo::as_item_def(rec)->light[0];
        float c = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin());               // 0x005768d0
        float x = c * jo::as_item_def(rec)->light[1];
        x = v1c - x;
        px = x + px;
        s = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos());
        const float v20 = s * jo::as_item_def(rec)->light[1];
        c = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin());
        float y = c * jo::as_item_def(rec)->light[0];
        y = y + v20;
        py = y + py;
        pz = pz + jo::as_item_def(rec)->light[2];
        const float radius = static_cast<float>(jo::as_item_def(rec)->light_kind);
        Vec3 pos;
        std::memcpy(&pos.x, &px, 4);
        std::memcpy(&pos.y, &py, 4);
        std::memcpy(&pos.z, &pz, 4);
        const Vec3 colour{global<float>(d::g_pos), global<float>(d::g_a69cdc), global<float>(d::g_a69ce0)};  // the same bits, read as floats
        const u32 id = joan::orig::arg::as<u32>(joan::orig::as_markers(obj(d::g_slots_a3ce90))->add(1, pos, colour, joan::orig::arg::bits(radius), str(d::s_glow2dlight)));  // 0x004877e0 "glow2dlight"
        jo::view<u16>(jo::self<jo::MapObject>(this)->glow_slot) = static_cast<u16>(id);
        return id;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Unit::setup_glow", 0x0049c710, &H::setup_glow);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Group::pause_sounds(std::int8_t a0) -> void { joan::bind::member<&::game_b84_06::H::pause_sounds>(this, a0); }  // 0x49c080
auto joan::orig::MapObject::release_light() -> void { joan::bind::member<&::game_b94_02::H::release_92>(this); }  // 0x49c1a0
auto joan::orig::Place::find_unit(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_02::H::find_unit>(this, a0)); }  // 0x49c240
auto joan::orig::Place::count_hostiles(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_02::H::count_units>(this, a0)); }  // 0x49c470
auto joan::orig::Unit::setup_glow() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_04::H::setup_glow>(this)); }  // 0x49c710
auto joan::orig::MapObject::release_marker() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_04::H::release_slot>(this)); }  // 0x49c920
auto joan::orig::MapObject::set_scale(std::uint32_t a0) -> void { joan::bind::member<&::game_b84_06::H::set_6e>(this, a0); }  // 0x49cac0
