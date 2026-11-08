// game/unknown_unittask.cpp: functions whose original source file is unknown (formerly game/_unplaced/unittask.cpp;
// re/attribution.csv: between ww/unitres.cpp and ww/unitures.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

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

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::obj;

using i32 = std::int32_t;

using joan::hook::global;

using i16 = std::int16_t;

using joan::call::cdecl_call;

using u8 = std::uint8_t;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b88_05.cpp (game/b88_05)
namespace {
namespace game_b88_05 {

void* const kArrays = obj(d::g_misc);     // 0x00591070 remove, 0x005911a0 search, 0x00590660 random
void* const kGameUnits = obj(d::g_units);  // 0x00428b70 missing / 0x0041b3c0 get

void array_remove(void* data, std::int32_t count, std::int32_t size, std::int32_t index) {
    joan::orig::as_misc(kArrays)->del_array_rec_591070(joan::orig::arg::str(data), joan::orig::arg::i32(count), joan::orig::arg::i32(size), joan::orig::arg::i32(index));  // 0x00591070
}

JOAN_COUNTER(c_release_slot, "Unit::release_slot");
JOAN_COUNTER(c_remove_member, "Group::remove_member");
JOAN_COUNTER(c_needs_upgrade, "Unit::needs_upgrade");

struct H {
    // ------------------------------------------------------------ 0x00528300
    std::int32_t release_slot() {
        c_release_slot.hit();
        if (jo::self<jo::Unit>(this)->task.kind == 0xf && jo::self<jo::Unit>(this)->slot_index >= 0 &&
            joan::orig::as_dynarray(kGameUnits)->empty_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Unit>(this)->task.target))) == 0) {  // 0x00428b70
            std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Unit>(this)->task.target))));  // 0x0041b3c0
            jo::view<std::uint32_t*>(jo::as_unit(unit)->mask_slots_b)[jo::self<jo::Unit>(this)->slot_index] = 0;
            jo::view<std::uint32_t*>(jo::as_unit(unit)->mask_slots)[jo::self<jo::Unit>(this)->slot_index] = 0;
            jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->weapon) = 0;
            jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->weapon_slot) = 0;
            jo::self<jo::Unit>(this)->slot_index = -1;
        }
        joan::orig::as_unit_task(&jo::self<jo::Unit>(this)->task)->set(0u, static_cast<std::uint8_t>(joan::orig::arg::u32(0)), 0u, 0u, 0u, 0u);  // 0x004e4520
        joan::orig::as_unit_task(&jo::self<jo::Unit>(this)->task_next)->set(0u, static_cast<std::uint8_t>(joan::orig::arg::u32(0)), 0u, 0u, 0u, 0u);  // 0x004e4520
        return 1;
    }

    // ------------------------------------------------------------ 0x005288f0 (ret 4)
    void remove_member(std::int32_t id) {
        c_remove_member.hit();
        for (std::int32_t i = 0; i < static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Group>(this)->count)); ++i) {
            if (static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::self<jo::Group>(this)->members[i])) != id) continue;
            array_remove(jo::self<jo::Group>(this)->members, static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Group>(this)->count)), 2, i + 1);
            jo::view<std::uint8_t>(jo::self<jo::Group>(this)->count) = static_cast<std::uint8_t>(jo::view<std::uint8_t>(jo::self<jo::Group>(this)->count) - 1);
            std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
            std::uint16_t* const counter = &jo::view<std::uint16_t>(jo::self<jo::Group>(this)->class_counts[jo::as_unit(unit)->rank]);
            *counter = static_cast<std::uint16_t>(*counter - 1);
            joan::orig::as_group(this)->arrange(1, 1);  // 0x004fa0f0
            break;
        }
    }

    // ------------------------------------------------------------ 0x00527f10 (ret 4)
    std::int32_t needs_upgrade(const std::uint8_t* rec) {
        c_needs_upgrade.hit();
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_game(this)->byte_2e1()) != 0) return 0;  // 0x00432b90
        if (jo::self<jo::Unit>(this)->task.kind == 0) return 1;
        if (jo::self<jo::Unit>(this)->task.priority < static_cast<std::int8_t>(rec[4])) return 1;
        if (jo::self<jo::Unit>(this)->task.expire_time == 0) return 0;
        if (jo::self<jo::Unit>(this)->task.expire_time < global<std::uint32_t>(d::g_now)) return 1;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_05", name, addr, fn)
B("Unit::release_slot", 0x00528300, &H::release_slot);
B("Group::remove_member", 0x005288f0, &H::remove_member);
B("Unit::needs_upgrade", 0x00527f10, &H::needs_upgrade);
#undef B

}  // namespace game_b88_05
}  // namespace

// ---------------------------------------- from modules/game_b87_03.cpp (game/b87_03)
namespace {
namespace game_b87_03 {

using joan::hook::x87::ftol;

constexpr img::Addr kUnits(d::g_units);
constexpr img::Addr kTasks(d::g_tasks);

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }
bool unit_invalid(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(id)) != 0; }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_set_task, "Unit::set_task");
JOAN_COUNTER(c_add_unit, "Group::add_unit");

struct H {
    // ------------------------------------------------------------------------ 0x00527fb0
    std::int32_t set_task(std::uint8_t* rec) {
        c_set_task.hit();
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->needs_upgrade(joan::orig::arg::mptr(rec))) == 0) return 0;  // 0x00527f10
        auto pair = [&](std::uintptr_t thunk) {
            if (unit_invalid(jo::view<std::int32_t>(jo::self<jo::TaskSlot>(rec)->target))) return;
            std::uint8_t* const o = unit_get(jo::view<std::int32_t>(jo::self<jo::TaskSlot>(rec)->target));
            const std::int32_t z = ftol(jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[2]);
            const std::int32_t y = ftol(jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[1]);
            const std::int32_t x = ftol(jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[0]);
            thiscall(thunk, img::obj(kTasks), sx16(jo::self<jo::Unit>(this)->type_id), sx16(jo::as_unit(o)->type_id),
                     sx16(jo::self<jo::Unit>(this)->side), sx16(jo::as_unit(o)->side), x, y, z);
        };
        switch (jo::view<std::uint32_t>(jo::self<jo::TaskSlot>(rec)->kind)) {  // (task - 2) <= 0xb, table 0x528247 / 0x528233
            case 2:
                pair(fn::code_value(fn::TaskList_add_31));  // 0x004ba1f0
                break;
            case 4:
                pair(fn::code_value(fn::X_add_entry));  // 0x004ba170
                break;
            case 13:
                pair(fn::code_value(fn::TaskList_add_31));  // 0x004ba1f0
                break;
            case 5: {
                const std::int32_t z = ftol(jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[2]);
                const std::int32_t y = ftol(jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[1]);
                const std::int32_t x = ftol(jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[0]);
                joan::orig::as_spawner(img::obj(kTasks))->spawn(joan::orig::arg::i32(sx16(jo::self<jo::Unit>(this)->type_id)), joan::orig::arg::i32(sx16(jo::self<jo::Unit>(this)->side)), joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(z));  // 0x004ba270
                break;
            }
            default:
                break;
        }
        joan::orig::as_unit(this)->release_slot();  // 0x00528300
        jo::TaskSlot* const slot = jo::self<jo::TaskSlot>(rec);
        joan::orig::as_unit_task(&jo::self<jo::Unit>(this)->task)->set(joan::orig::arg::u32(slot->kind), static_cast<std::uint8_t>(joan::orig::arg::u32(slot->priority)), joan::orig::arg::u32(jo::view<std::int32_t>(slot->target)), joan::orig::arg::u32(jo::view<std::int32_t>(slot->arg_9)), joan::orig::arg::u32(jo::view<std::int32_t>(slot->arg_d)), joan::orig::arg::u32(jo::view<std::int32_t>(slot->expire_time)));  // 0x004e4520
        joan::orig::as_unit_task(&jo::self<jo::Unit>(this)->task_next)->set(0u, static_cast<std::uint8_t>(joan::orig::arg::u32(0)), 0u, 0u, 0u, 0u);
        if (jo::self<jo::TaskSlot>(rec)->kind == 0xf) {
            jo::self<jo::Unit>(this)->task_kind = 1;
            joan::orig::as_unit(this)->idle_tick();  // 0x0049d810
        }
        return 1;
    }

    // ------------------------------------------------------------------------ 0x005287d0
    void add_unit(std::int32_t id) {
        c_add_unit.hit();
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::Group>(this)->count); ++i)
            if (sx16(jo::self<jo::Group>(this)->members[i]) == id) return;
        jo::self<jo::Group>(this)->members[static_cast<std::int32_t>(jo::self<jo::Group>(this)->count)] = static_cast<std::int16_t>(id);
        jo::view<std::uint8_t>(jo::self<jo::Group>(this)->count) = static_cast<std::uint8_t>(jo::view<std::uint8_t>(jo::self<jo::Group>(this)->count) + 1);
        std::int16_t& n = jo::self<jo::Group>(this)->class_counts[jo::as_unit(unit_get(id))->rank];
        n = static_cast<std::int16_t>(n + 1);
        joan::orig::as_group(this)->arrange(1, 1);  // 0x004fa0f0
    }

    // ------------------------------------------------------------------------ 0x00528660
    void* group_ctor() {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::Group));  // memset (0x58 on x86)
        return this;
    }
};

#define B87(name, addr, fn) JOAN_REPLACE("game/b87_03", name, addr, fn)
B87("Unit::set_task", 0x00527fb0, &H::set_task);
B87("Group::add_unit", 0x005287d0, &H::add_unit);
B87("Group::Group", 0x00528660, &H::group_ctor);
#undef B87

}  // namespace game_b87_03
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

using joan::orig::helpers::addr_of;

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00528440
    std::uintptr_t poke() {
        if (jo::self<jo::Unit>(this)->task.expire_time == 0) return addr_of(self());
        if (jo::self<jo::Unit>(this)->task.expire_time >= global<std::uint32_t>(d::g_now)) return addr_of(self());
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_unit_task(&jo::self<jo::Unit>(this)->task)->set(0u, static_cast<std::uint8_t>(joan::orig::arg::u32(0)), 0u, 0u, 0u, 0u));  // 0x004e4520
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Unit::poke", 0x00528440, &H::poke);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
namespace {
namespace game_b102_05 {

using u16 = std::uint16_t;

using u32 = std::uint32_t;


void* const kUnits = obj(d::g_units);
void* const kRandom = obj(d::g_misc);

i32 unit_dead(u32 t) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(t)); }  // 0x00428b70

JOAN_COUNTER(c_purge_dead, "IdList::purge_dead_528a30");

void JOAN_CDECL init_52ad40() { joan::orig::init_a3cd90(); }
void JOAN_CDECL init_52b270() { joan::orig::div_a3cd94(); }

struct H {
    // ------------------------------------------------------------------ 0x00528a30
    i32 purge_dead_528a30() {
        c_purge_dead.hit();
        i32 removed = 0;
        for (i32 i = static_cast<i32>(jo::self<jo::Group>(this)->count) - 1; i >= 0; --i) {
            const i32 id = static_cast<i32>(jo::self<jo::Group>(this)->members[i]);
            if (unit_dead(static_cast<u32>(id)) != 0) {
                joan::orig::as_misc(kRandom)->del_array_rec_591070(joan::orig::arg::str(jo::self<jo::Group>(this)->members), static_cast<i32>(jo::self<jo::Group>(this)->count), 2, joan::orig::arg::i32(i + 1));  // 0x00591070
                jo::self<jo::Group>(this)->count = static_cast<i8>(jo::self<jo::Group>(this)->count - 1);
                removed = 1;
            }
        }
        return removed;
    }

    // ------------------------------------------------------------------ 0x00528af0
    void clear_528af0() {
        for (i32 i = static_cast<i32>(jo::self<jo::Group>(this)->count) - 1; i >= 0; --i) {
            const i32 id = static_cast<i32>(jo::self<jo::Group>(this)->members[i]);
            if (unit_dead(static_cast<u32>(id)) != 0) continue;
            jo::view<u16>(jo::as_unit(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))))->leader_id) = 0;  // 0x0041b3c0
        }
        jo::view<u8>(jo::self<jo::Group>(this)->count) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("IdList::purge_dead_528a30", 0x00528a30, &H::purge_dead_528a30);
B("IdList::clear_528af0", 0x00528af0, &H::clear_528af0);
B("init_52ad40", 0x0052ad40, &init_52ad40);
B("init_52b270", 0x0052b270, &init_52b270);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_apply_units, "Obj::apply_to_units");

struct H {
    // ------------------------------------------------------------------------ 0x00528c60
    void apply_to_units(u8* list, i32 owner, i32 n) {
        c_apply_units.hit();
        for (i32 i = 0; i < static_cast<i32>(static_cast<i8>(list[2])) && n != 0; ++i) {
            if (joan::orig::as_dynarray(kUnits)->empty_at(static_cast<i32>(jo::as_group(list)->members[i])) != 0)
                continue;  // 0x00428b70
            void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(jo::as_group(list)->members[i]));  // 0x0041b3c0
            if (jo::as_unit(u)->rank == owner) {
                joan::orig::as_member(u)->release();                                     // 0x0042fcb0
                joan::orig::as_unit(u)->link_to(static_cast<i32>(jo::self<jo::Unit>(this)->id));       // 0x00438f80
                --n;
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Obj::apply_to_units", 0x00528c60, &H::apply_to_units);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b101_07.cpp (game/b101_07)
namespace {
namespace game_b101_07 {

struct H {
    // ------------------------------------------------------------------ 0x00528de0
    void nop_528de0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_07", name, addr, fn)
B("Obj::nop_528de0", 0x00528de0, &H::nop_528de0);
#undef B

}  // namespace game_b101_07
}  // namespace

// ---------------------------------------- from modules/game_b104_06.cpp (game/b104_06)
namespace {
namespace game_b104_06 {

struct H {
    // ------------------------------------------------------------------------ 0x005290c0
    void nop_5290c0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_06", name, addr, fn)
B("Obj::nop_5290c0", 0x005290c0, &H::nop_5290c0);
#undef B

}  // namespace game_b104_06
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

void* const kSoldiers = obj(d::g_units);

JOAN_COUNTER(c_count_owner, "Group::count_owner");

struct H {
    // ------------------------------------------------------------------------ 0x00529810
    i32 count_owner(i32 owner) {
        c_count_owner.hit();
        i32 n = 0;
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Group>(this)->count); ++i) {
            const i32 id = jo::self<jo::Group>(this)->members[i];
            if (joan::orig::as_dynarray(kSoldiers)->empty_at(joan::orig::arg::i32(id)) != 0) continue;  // 0x00428b70
            u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
            if (jo::as_unit(unit)->rank == owner) ++n;
        }
        return n;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("Group::count_owner", 0x00529810, &H::count_owner);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b104_07.cpp (game/b104_07)
namespace {
namespace game_b104_07 {

using joan::hook::x87::Fpu;

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // ------------------------------------------------------------------ 0x00529920
    float percent_529920() {
        // x64: class unknown: raw +0x114 i32 left; no caller in joan.exe (thunk 0x0040a402 and 0x00529920 unreferenced)
        // and none in C++
        return static_cast<float>(Fpu{true}.div(static_cast<double>(f<i32>(0x114)),
                                                static_cast<double>(global<float>(d::flt_100))));  // 100.0f
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_07", name, addr, fn)
B("Obj::percent_529920", 0x00529920, &H::percent_529920);
#undef B

}  // namespace game_b104_07
}  // namespace

// ---------------------------------------- from modules/game_b107_09.cpp (game/b107_09)
namespace {
namespace game_b107_09 {

using u32 = std::uint32_t;

u32 JOAN_CDECL init_529ee0() {
    joan::orig::init_pi_a3cd8c();
    return 0;  // void callee; _initterm ignores the eax
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_09", name, addr, fn)
B("init_529ee0", 0x00529ee0, &init_529ee0);
#undef B

}  // namespace game_b107_09
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

void JOAN_CDECL init_pi_a3cd8c() {  // 0x0052ab80
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));      // atan
    global<float>(d::g_a3cd8c) = static_cast<float>(t * 4.0);  // qword 0x9cf788 = 4.0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("init_pi_a3cd8c", 0x0052ab80, &init_pi_a3cd8c);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b101_08.cpp (game/b101_08)
namespace {
namespace game_b101_08 {

// ---------------------------------------------------------------------- 0x0052af00
static void JOAN_CDECL init_a3cd90() { global<float>(d::g_a3cd90) = global<float>(d::g_a3cd8c) / global<float>(d::flt_2); }

// ---------------------------------------------------------------------- 0x0052b570
static void JOAN_CDECL init_a3cd98_wrap() { joan::orig::div_a3cd8c(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b101_08", name, addr, fn)
B("init_a3cd90", 0x0052af00, &init_a3cd90);
B("init_a3cd98_wrap", 0x0052b570, &init_a3cd98_wrap);
#undef B

}  // namespace game_b101_08
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
namespace {
namespace game_b100_08 {

void JOAN_CDECL div_a3cd94() {  // 0x0052b320
    global<float>(d::g_a3cd94) = global<float>(d::g_a3cd8c) / global<float>(d::flt_3);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("div_a3cd94", 0x0052b320, &div_a3cd94);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

void JOAN_CDECL div_a3cd8c() {  // 0x0052b7c0
    global<float>(d::g_a3cd98) = global<float>(d::g_a3cd8c) / global<float>(d::flt_4);
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("Globals::div_a3cd8c", 0x0052b7c0, &div_a3cd8c);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Unit::needs_upgrade(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b88_05::H::needs_upgrade>(this, a0)); }  // 0x527f10
auto joan::orig::Unit::set_task(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b87_03::H::set_task>(this, a0)); }  // 0x527fb0
auto joan::orig::Unit::release_slot() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b88_05::H::release_slot>(this)); }  // 0x528300
auto joan::orig::Unit::poke() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b89_06::H::poke>(this)); }  // 0x528440
auto joan::orig::Group::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b87_03::H::group_ctor>(this)); }  // 0x528660
auto joan::orig::Group::add_unit(std::int32_t a0) -> void { joan::bind::member<&::game_b87_03::H::add_unit>(this, a0); }  // 0x5287d0
auto joan::orig::Group::remove_member(std::int32_t a0) -> void { joan::bind::member<&::game_b88_05::H::remove_member>(this, a0); }  // 0x5288f0
auto joan::orig::Group::transfer_units(void* a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::member<&::game_b99_05::H::apply_to_units>(this, a0, a1, a2); }  // 0x528c60
auto joan::orig::Group::count_owner(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_04::H::count_owner>(this, a0)); }  // 0x529810
auto joan::orig::init_pi_a3cd8c() -> void { joan::bind::free<&::game_b97_04::init_pi_a3cd8c>(); }  // 0x52ab80
auto joan::orig::init_a3cd90() -> void { joan::bind::free<&::game_b101_08::init_a3cd90>(); }  // 0x52af00
auto joan::orig::div_a3cd94() -> void { joan::bind::free<&::game_b100_08::div_a3cd94>(); }  // 0x52b320
auto joan::orig::div_a3cd8c() -> void { joan::bind::free<&::game_b94_04::div_a3cd8c>(); }  // 0x52b7c0
