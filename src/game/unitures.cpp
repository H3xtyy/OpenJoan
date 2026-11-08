// game/unitures.cpp: functions of the original d:\ww\unitures.cpp (re/attribution.csv).
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
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

void* const kMem = obj(d::g_mem);

}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
//
//      0x00530d00 UnitRes::free        (thiscall, ret 0)
namespace {
namespace game_b92_04 {

void mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

struct H {

    // ------------------------------------------------------------------------ 0x00530d00
    void unitres_free() {
        if (static_cast<i32>(jo::view<std::int8_t>(jo::self<jo::UnitTable>(this)->loaded)) != 0) {
            if (jo::self<jo::UnitTable>(this)->entries != nullptr) {
                mem_free(jo::self<jo::UnitTable>(this)->entries, str(d::s_d_ww_unitures_cpp), static_cast<i32>(global<i16>(d::g_a085cc)) + 6);  // unitures.cpp
                jo::self<jo::UnitTable>(this)->entries = nullptr;
            }
            jo::self<jo::UnitTable>(this)->loaded = 0;
        }
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("UnitRes::free", 0x00530d00, &H::unitres_free);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
//
//      0x00530d70 UnitTable::load         (thiscall, ret 0) - table rows: atoi(2 chars), 0x00531b20.
namespace {
namespace game_b99_05 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using u16 = std::uint16_t;

void* const kTables = obj(d::g_game_set);
void* const kText = obj(d::g_misc);

JOAN_COUNTER(c_unit_table, "UnitTable::load");

using joan::orig::helpers::memset_;  // memset
i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }                       // atoi

void* game_alloc(u32 size, const char* file, i32 line) {
    return joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058e7b0
}
// Table text field: 0x00590170(src, n).
char* text_field(const void* src, i32 n) { return joan::orig::arg::as<char*>(joan::orig::as_misc(kText)->rtrim_field_590170(joan::orig::arg::str(src), joan::orig::arg::i32(n))); }

struct H {

    // ------------------------------------------------------------------------ 0x00530d70
    void unit_table_load() {
        c_unit_table.hit();
        void* const rec = joan::orig::arg::as<void*>(joan::orig::as_game_set(kTables)->open_db_58b180(joan::orig::arg::str(jo::self<jo::UnitTable>(this)->table_name)));  // 0x0058b180
        jo::self<jo::UnitTable>(this)->count = joan::orig::arg::as<i32>(joan::orig::as_database(rec)->rec_count());                              // 0x00415280
        jo::self<jo::UnitTable>(this)->entries = static_cast<jo::UnitTableEntry*>(game_alloc(static_cast<u32>(jo::self<jo::UnitTable>(this)->count) << 2, str(d::s_d_ww_unitures_cpp),
                                    static_cast<i32>(global<i16>(d::g_a085d0)) + 9));
        memset_(jo::self<jo::UnitTable>(this)->entries, 0, static_cast<u32>(jo::self<jo::UnitTable>(this)->count) << 2);
        for (i32 i = 0; i < jo::self<jo::UnitTable>(this)->count; ++i) {
            u8* const row = joan::orig::arg::as<u8*>(joan::orig::as_database(rec)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            jo::UnitTableEntry* const e = jo::self<jo::UnitTable>(this)->entries + i;
            e->id = static_cast<u16>(atoi_(text_field(row, 2)));
            char* const name = text_field(row + 2, 0x20);
            e->def = static_cast<u16>(joan::orig::arg::as<i32>(joan::orig::as_table(obj(d::g_upgrade_res))->find_name_531b20(joan::orig::arg::str(name))));  // 0x00531b20
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("UnitTable::load", 0x00530d70, &H::unit_table_load);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
//
//    - Obj::reset_530cc0 0x00530cc0 (ret 0): 0x530d00, 0x530d70, byte +0x34 = 1.
namespace {
namespace game_b100_08 {

using joan::call::thiscall;
using joan::orig::helpers::at;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

// ------------------------------------------------------------------------------- thiscall
struct H {

    // 0x00530cc0
    void reset_530cc0() {
        joan::orig::as_unit_upgrade_list(this)->free();  // 0x00530d00
        joan::orig::as_unit_upgrade_list(this)->load();  // 0x00530d70
        jo::self<jo::UnitTable>(this)->loaded = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("Obj::reset_530cc0", 0x00530cc0, &H::reset_530cc0);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b103_06.cpp (game/b103_06)
//
//    - Obj::call_530d00 0x00530c90 (ret 0).
//
//  - 0x0051fce0, 0x00522c10, 0x00530c60, 0x00530c90 and the getters 0x0054e040 .. 0x0054fd60 have
//    no references (no call, no data dword for the function or its thunk): dead code, replaced for
//    completeness.
#define M "game/b103_06"

namespace {
namespace game_b103_06 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // 0x00530c90
    u32 call_530d00() { joan::orig::as_unit_upgrade_list(this)->free(); return 0; }  // 0x00530d00 (void; 0x00530c90 is unreferenced)
};

JOAN_REPLACE(M, "Obj::call_530d00", 0x00530c90, &H::call_530d00);

}  // namespace game_b103_06
}  // namespace
#undef M

// ---------------------------------------- from modules/game_b84_02.cpp (game/b84_02)
namespace {
namespace game_b84_02 {

using joan::orig::helpers::at;

struct H {

    // 0x00530eb0 Occluder::set_open: +0x70 = 1.
    void occ_set_70() { jo::self<jo::Occluder>(this)->enabled = 1; }
    // 0x00530ee0 Occluder::set_corner(i, v), ret 8.
    void occ_set_corner(std::int32_t i, const std::uint32_t* v) {
        std::uint32_t* const d = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(this) + i * 0xc);
        d[0] = v[0];
        d[1] = v[1];
        d[2] = v[2];
    }
};

#define B84(name, addr, fn) JOAN_REPLACE("game/b84_02", name, addr, fn)
B84("Occluder::set_corner", 0x00530ee0, &H::occ_set_corner);
B84("Occluder::set_70", 0x00530eb0, &H::occ_set_70);
#undef B84

}  // namespace game_b84_02
}  // namespace

// ---------------------------------------- from modules/game_b100_08_2.cpp (game/b100_08)
namespace {
namespace game_b100_08_2 {

using joan::orig::helpers::at;

using joan::hook::global;

using joan::call::import_call;

using i32 = std::int32_t;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12);

JOAN_COUNTER(c_ray_hit, "Quad::ray_hit");

// ------------------------------------------------------------------------------- thiscall
struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // 0x00530f20
    bool ray_hit(Vec3 p, Vec3 q) {
        c_ray_hit.hit();
        if (jo::self<jo::Quad>(this)->valid == 0) return false;
        Vec3 tmp;
        const Vec3 dir = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &q, &p));  // 0x0060cc90 (q - p)
        const Vec3 n{jo::self<jo::Quad>(this)->plane_nx, jo::self<jo::Quad>(this)->plane_ny, jo::self<jo::Quad>(this)->plane_nz};
        const float denom = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, &n, &dir));  // dot_product
        const float num = ((p.x * jo::self<jo::Quad>(this)->plane_nx + p.y * jo::self<jo::Quad>(this)->plane_ny) + p.z * jo::self<jo::Quad>(this)->plane_nz) + jo::self<jo::Quad>(this)->plane_d;
        const float zero = global<float>(d::k_9cf5c4);
        if (!(denom < zero || denom > zero)) return false;  // equal or unordered
        const float t = -num / denom;
        if (!(t > zero)) return false;
        if (!(t < global<float>(d::flt_1))) return false;  // 1.0
        Vec3 hit;
        hit.x = dir.x * t + p.x;
        hit.y = dir.y * t + p.y;
        hit.z = dir.z * t + p.z;
        for (i32 i = 0; i < 4; ++i) {
            Vec3 tmp2;
            Vec3 e = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp2, &hit, joan::orig::arg::ptr(self() + i * 0xc)));  // 0x0060cc90; x64: needs Quad+0x0 Vec3[4]
            Vec3 c;
            import_call<void*>(d::iat_v3d_87_cross_product, &c, &jo::self<jo::Quad>(this)->edges[i * 3], &e);  // cross_product
            if (!(static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, &c, &n)) >= zero)) return false;  // dot_product
        }
        return true;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("Quad::ray_hit", 0x00530f20, &H::ray_hit);
#undef B

}  // namespace game_b100_08_2
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

using joan::orig::helpers::at;

using joan::call::thiscall;

using joan::orig::helpers::memset_;

JOAN_COUNTER(c_has_ready, "List::has_ready");

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00531520
    std::int32_t has_ready(std::int32_t kind) {
        c_has_ready.hit();
        for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_upgrade_list(self())->count()); i > 0; --i) {  // 0x00531360
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_upgrade_list(self())->get(joan::orig::arg::i32(i)));                       // 0x005313a0
            if (static_cast<std::int32_t>(jo::as_upgrade_list_entry(e)->id) != kind) continue;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_counter(e)->step()) != 0) return 1;  // 0x005315a0
        }
        return 0;
    }

    // 0x005315a0
    std::int32_t step() {
        if (static_cast<std::int32_t>(jo::as_upgrade_list_entry(this)->cur) < static_cast<std::int32_t>(jo::as_upgrade_list_entry(this)->max)) {
            jo::as_upgrade_list_entry(this)->cur = static_cast<std::int16_t>(jo::as_upgrade_list_entry(this)->cur + 1);
            return 1;
        }
        return 0;
    }

    void* zero_4() {  // 0x00531140
        memset_(self(), 0, static_cast<u32>(sizeof(jo::UpgradeList)));  // the UpgradeList (DynArray* list; 4 bytes on x86)
        return self();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("List::has_ready", 0x00531520, &H::has_ready);
M("Counter::step", 0x005315a0, &H::step);
M("Obj::zero_4", 0x00531140, &H::zero_4);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b86_02.cpp (game/b86_02)
namespace {
namespace game_b86_02 {

using joan::call::thiscall;

struct H {
    std::uint32_t release_531170() { joan::orig::as_upgrade_list(this)->clear(); return 0; }  // 0x00531170 -> 0x005311c0 (void; result unused)
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_02", name, addr, fn)
B86("Obj531170::release", 0x00531170, &H::release_531170);
#undef B86

}  // namespace game_b86_02
}  // namespace

// ---------------------------------------- from modules/game_b105_07.cpp (game/b105_07)
namespace {
namespace game_b105_07 {

struct H {
    void nop_5311a0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_07", name, addr, fn)
B("Obj::nop_5311a0", 0x005311a0, &H::nop_5311a0);
#undef B

}  // namespace game_b105_07
}  // namespace

// ---------------------------------------- from modules/game_b87_03.cpp (game/b87_03)
namespace {
namespace game_b87_03 {

using joan::orig::helpers::at;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x005311c0
    void holder_reset() {
        void*& p = jo::view<void*>(jo::self<jo::UpgradeList>(this)->list);
        if (p != nullptr) {
            void* const q = p;
            if (q != nullptr) joan::orig::as_dynarray(q)->destroy(1u);  // 0x00463fc0 deleting dtor
            p = nullptr;
        }
    }
};

#define B87(name, addr, fn) JOAN_REPLACE("game/b87_03", name, addr, fn)
B87("Holder::reset", 0x005311c0, &H::holder_reset);
#undef B87

}  // namespace game_b87_03
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (game/b82_17)
namespace {
namespace en3d_b82_17 {

using joan::orig::helpers::at;

struct H {

    // 0x00531360
    std::int32_t ref_count() {
        if (jo::view<void*>(jo::self<jo::UpgradeList>(this)->list) == nullptr) return 0;
        return joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::UpgradeList>(this)->list))->size();  // 0x00417ad0
    }

    // 0x005313a0
    void* ref_get(std::int32_t i) { return joan::orig::arg::as<void*>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::UpgradeList>(this)->list))->get(joan::orig::arg::i32(i))); }  // 0x00417200
};

#define B82_17G(name, addr, fn) JOAN_REPLACE("game/b82_17", name, addr, fn)
B82_17G("Ref::count", 0x00531360, &H::ref_count);
B82_17G("Ref::get", 0x005313a0, &H::ref_get);
#undef B82_17G

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

using joan::orig::helpers::at;

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::orig::helpers::game_new;  // operator new

JOAN_COUNTER(c_add, "UpgradeList::add");

void* new_list() {  // new(0x1d) + 0x005843d0(8, 0x1e, 0)
    void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
    if (mem == nullptr) return nullptr;
    return joan::orig::dynarray(mem)->construct(static_cast<std::int32_t>(sizeof(jo::UpgradeListEntry)), 0x1e, 0);  // 0x005843d0 (8-byte UpgradeListEntry: four shorts, no pointers)
}

struct H {

    // 0x005313d0
    std::int32_t add(std::int32_t key) {
        c_add.hit();
        for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_upgrade_list(this)->count()); i > 0; --i) {  // 0x00531360
            jo::UpgradeListEntry* const entry = joan::orig::arg::as<jo::UpgradeListEntry*>(joan::orig::as_upgrade_list(this)->get(joan::orig::arg::i32(i)));  // 0x005313a0
            if (static_cast<std::int32_t>(entry->id) == key) {
                jo::view<std::uint16_t>(entry->max) = static_cast<std::uint16_t>(jo::view<std::uint16_t>(entry->max) + 1);
                return i;
            }
        }
        if (jo::view<void*>(jo::self<jo::UpgradeList>(this)->list) == nullptr) jo::view<void*>(jo::self<jo::UpgradeList>(this)->list) = new_list();
        std::uint16_t local[4] = {static_cast<std::uint16_t>(key), 1, 0, 1};
        joan::orig::dynarray(jo::view<void*>(jo::self<jo::UpgradeList>(this)->list))->linkin(static_cast<void*>(local));        // 0x00584620
        return joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::UpgradeList>(this)->list))->cursor();  // 0x00417520
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("UpgradeList::add", 0x005313d0, &H::add);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

using joan::orig::helpers::at;

using u8 = std::uint8_t;

struct H {

    void* clear_10() {  // 0x005315f0
        jo::self<jo::UpgradeRes>(this)->loaded = 0;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("X::clear_10", 0x005315f0, &H::clear_10);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02_2 {

using joan::call::cdecl_call;
using joan::orig::helpers::at;

using joan::orig::helpers::game_new;  // operator new

JOAN_COUNTER(c_assign, "UpgradeList::assign");

void* new_list() {  // new(0x1d) + 0x005843d0(8, 0x1e, 0)
    void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
    if (mem == nullptr) return nullptr;
    return joan::orig::dynarray(mem)->construct(static_cast<std::int32_t>(sizeof(jo::UpgradeListEntry)), 0x1e, 0);  // 0x005843d0 (8-byte UpgradeListEntry: four shorts, no pointers)
}

struct H {

    // 0x00531230
    void* assign(void** src) {
        c_assign.hit();
        if (*src != nullptr && jo::view<void*>(jo::self<jo::UpgradeList>(this)->list) == nullptr) jo::view<void*>(jo::self<jo::UpgradeList>(this)->list) = new_list();
        if (jo::view<void*>(jo::self<jo::UpgradeList>(this)->list) != nullptr) joan::orig::dynarray(jo::view<void*>(jo::self<jo::UpgradeList>(this)->list))->zap(1);  // 0x00584580
        if (*src != nullptr) {
            for (std::int32_t i = 1; i <= joan::orig::as_dynarray(*src)->size(); ++i) {  // 0x00417ad0
                void* const item = joan::orig::arg::as<void*>(joan::orig::as_dynarray(*src)->get(joan::orig::arg::i32(i)));  // 0x00417200
                joan::orig::dynarray(jo::view<void*>(jo::self<jo::UpgradeList>(this)->list))->linkin(joan::orig::arg::ptr(item));  // 0x00584620
            }
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("UpgradeList::assign", 0x00531230, &H::assign);
#undef B

}  // namespace game_b90_02_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::UnitUpgradeList::reload() -> void { joan::bind::member<&::game_b100_08::H::reset_530cc0>(this); }  // 0x530cc0
auto joan::orig::UnitUpgradeList::free() -> void { joan::bind::member<&::game_b92_04::H::unitres_free>(this); }  // 0x530d00
auto joan::orig::UnitUpgradeList::load() -> void { joan::bind::member<&::game_b99_05::H::unit_table_load>(this); }  // 0x530d70
auto joan::orig::Occluder::set_enabled() -> void { joan::bind::member<&::game_b84_02::H::occ_set_70>(this); }  // 0x530eb0
auto joan::orig::Occluder::set_corner(std::int32_t a0, void* a1) -> void { joan::bind::member<&::game_b84_02::H::occ_set_corner>(this, a0, a1); }  // 0x530ee0
auto joan::orig::UpgradeList::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::zero_4>(this)); }  // 0x531140
auto joan::orig::UpgradeList::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b86_02::H::release_531170>(this)); }  // 0x531170
auto joan::orig::UpgradeList::clear() -> void { joan::bind::member<&::game_b87_03::H::holder_reset>(this); }  // 0x5311c0
auto joan::orig::UpgradeList::assign(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_02_2::H::assign>(this, a0)); }  // 0x531230
auto joan::orig::UpgradeList::count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b82_17::H::ref_count>(this)); }  // 0x531360
auto joan::orig::UpgradeList::get(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17::H::ref_get>(this, a0)); }  // 0x5313a0
auto joan::orig::UpgradeList::add(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_02::H::add>(this, a0)); }  // 0x5313d0
auto joan::orig::List::has_ready(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b89_06::H::has_ready>(this, a0)); }  // 0x531520
auto joan::orig::Counter::step() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b89_06::H::step>(this)); }  // 0x5315a0
