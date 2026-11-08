// game/gfile3.cpp: functions of the original d:\ww\gfile3.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <vector>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../save/s5.hpp"
#include "../save/gen/records_s0.hpp"
#include "../save/records_s3.hpp"
#include "../save/gen/records_s2.hpp"
#include "../save/gen/records_s1.hpp"
#include "../save/records_s4.hpp"
#include "../en3d/scene_lists.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

struct Obj {};
using joan::call::thiscall;
using joan::call::cdecl_call;
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
using joan::img::obj;
using joan::img::str;

struct Vec3 {  // a 12-byte float vector, by value (XMM registers under System V, as the callees declare it)
    float x, y, z;
};

void* const kHeap = obj(d::g_mem);

using joan::orig::helpers::game_new;  // operator new

i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

u8* bp(const void* p) { return static_cast<u8*>(const_cast<void*>(p)); }

void* const kMem = obj(d::g_mem);

using joan::orig::helpers::line_of;

using joan::orig::helpers::op_new;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b103_02.cpp (game/b103_02)
//
//      0x0047de30 Obj::load_47de30(file)           (thiscall, ret 4)
//
//      0x00481a70 Obj::save_<n>(file)              (thiscall, ret 4) - 0x00481ba0 + write(this+0xc2, n)
//
//      0x00481ad0 Obj::load_<n>(file)              (thiscall, ret 4) - 0x00481cd0 + read(this+0xc2,
//                 n) + virtual slot 0x78.
//
//      0x00481b40 Obj::reset_d2_da_e7_eb           (thiscall, ret 0) - returns this.
//
//      0x00483430 Obj::load_483430(file)           (thiscall, ret 4)
//
//  - SEH frames in 0x0046ba20 / 0x00476f30 / 0x0047de30 only guard new + ctor (or the
//    member dtor); the normal path is reproduced.
//
//  - 0x0047de30: the alloc line number (word 0x9ffcb4) is read before the second 0x0042d0a0
//    call, as in the original push order.
namespace {
namespace game_b103_02 {

static_assert(sizeof(Vec3) == 12, "by-value 12-byte argument");

void* const kUnitTypes = obj(d::g_types);
void* const kModels = obj(d::g_a45f08);
const char* const kGfile3Cpp = str(d::s_d_ww_gfile3_cpp);  // "d:\\ww\\gfile3.cpp"

JOAN_COUNTER(c_load_47de30, "Obj::load_47de30");
JOAN_COUNTER(c_load_483430, "Obj::load_483430");

void* heap_alloc(u32 size, i32 line) {
    return joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(size), joan::orig::arg::str(kGfile3Cpp), joan::orig::arg::i32(line));  // 0x0058e7b0
}

// File object helpers (thiscall on the file).
i32 file_write(void* file, const void* buf, u32 n) { return joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(buf), joan::orig::arg::u32(n)); }  // 0x005872e0
i32 file_read(void* file, void* buf, u32 n) { return joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(buf), joan::orig::arg::u32(n)); }  // 0x005873a0
void file_fail(void* file) { joan::orig::as_file(file)->file_close(); }                                          // 0x00587260
u16 file_read_word(void* file) { return joan::orig::arg::as<u16>(joan::orig::as_file(file)->file_get_short()); }     // 0x00587c20
i32 file_read_int(void* file) { return joan::orig::as_file(file)->file_get_long(); }                                // 0x00587e20

u8* unit_type(u16 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kUnitTypes)->get(joan::orig::arg::i16(static_cast<u32>(id)))); }  // 0x0042d0a0

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------ 0x0047de30
    i32 load_47de30(void* file) {
        c_load_47de30.hit();
        {  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): save::get_Unit; 0 = the raw read
            i32 ok;
            if (joan::save::fieldwise()) {
                joan::save::In in(*joan::orig::as_file(file), joan::save::kUnit, this);  // File::file_read 0x005873a0
                joan::save::get_Unit(in, *jo::self<jo::Unit>(this));
                ok = in.end();
            } else {
                ok = file_read(file, this, 0x432);
            }
            if (ok == 0) return 0;
        }
        joan::orig::as_unit(this)->reset_links();  // 0x0047e300
        if (jo::self<jo::Unit>(this)->group != nullptr) {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Group)));  // 0x58 on x86
            jo::self<jo::Unit>(this)->group = static_cast<jo::Group*>(mem != nullptr ? joan::orig::as_group(mem)->construct() : nullptr);  // 0x00528660
            if (joan::orig::arg::as<i32>(joan::orig::as_sub(jo::self<jo::Unit>(this)->group)->load_58(joan::orig::arg::mptr(file))) == 0) return 0;          // 0x0047e580
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit_stats(&jo::self<jo::Unit>(this)->stats_block)->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047e600 (+0x16f)
        if (joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047e700 (+0x1aa)
        if (jo::self<jo::Unit>(this)->modifiers != nullptr) {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Modifiers)));  // 0x1e on x86
            jo::self<jo::Unit>(this)->modifiers = static_cast<jo::Modifiers*>(mem != nullptr ? joan::orig::as_unit_data(mem)->construct() : nullptr);  // 0x00495950
            if (joan::orig::arg::as<i32>(joan::orig::as_sub(jo::self<jo::Unit>(this)->modifiers)->load_parts(joan::orig::arg::mptr(file))) == 0) return 0;          // 0x0047e9f0
        }
        if (jo::self<jo::Unit>(this)->orders != nullptr) {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Order)));  // 0x3e on x86
            jo::self<jo::Unit>(this)->orders = static_cast<jo::Order*>(mem != nullptr ? joan::orig::as_order(mem)->construct() : nullptr);  // 0x004bab50
            if (joan::orig::arg::as<i32>(joan::orig::as_order(jo::self<jo::Unit>(this)->orders)->load_entries(joan::orig::arg::mptr(file))) == 0) return 0;          // 0x0047f130
        }
        load_int_table(file, jo::self<jo::Unit>(this)->mask_slots, 0x39);    // +0x378
        load_int_table(file, jo::self<jo::Unit>(this)->mask_slots_b, 0x3f);  // +0x37c
        if (joan::orig::arg::as<i32>(joan::orig::as_point_list(&jo::self<jo::Unit>(this)->path)->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047f290 (+0x432)
        if (joan::orig::arg::as<i32>(joan::orig::as_point_list(&jo::self<jo::Unit>(this)->path_b)->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047f290 (+0x473)
        if (jo::self<jo::Unit>(this)->world != nullptr) {
            const i32 id = static_cast<i32>(static_cast<i16>(file_read_word(file)));
            jo::self<jo::Unit>(this)->world = static_cast<jo::World*>(joan::orig::as_units(kModels)->get_420240(joan::orig::arg::i32(id)));  // 0x00420240
            joan::orig::as_unit(this)->set_group(jo::self<jo::Unit>(this)->world);          // 0x004fed00
            joan::orig::as_ground_object(this)->set_position(*reinterpret_cast<const Vec3*>(jo::self<jo::Unit>(this)->position_));           // 0x004fedf0
        }
        joan::orig::as_unit(this)->set_side(joan::orig::arg::i32(static_cast<u32>(jo::view<u16>(jo::self<jo::Unit>(this)->side))));  // 0x00509250
        return 1;
    }
    // Table of [type(+0x3b)+0x1ad] dwords in slot (Unit +0x378 / +0x37c; allocated when the count is > 0), filled
    // with file ints (0x0047de30).
    void load_int_table(void* file, jo::ArrI32*& slot, i32 line_add) {
        if (jo::as_unit_type(unit_type(jo::view<u16>(jo::self<jo::Unit>(this)->type_id)))->slot_count > 0) {
            const i32 line = static_cast<i32>(global<i16>(d::g_9ffcb4)) + line_add;
            const u32 n = static_cast<u32>(jo::as_unit_type(unit_type(jo::view<u16>(jo::self<jo::Unit>(this)->type_id)))->slot_count);
            slot = static_cast<jo::ArrI32*>(heap_alloc(n << 2, line));
        }
        for (i32 i = 0; i < jo::as_unit_type(unit_type(jo::view<u16>(jo::self<jo::Unit>(this)->type_id)))->slot_count; ++i) {
            const i32 v = file_read_int(file);
            reinterpret_cast<i32*>(slot)[i] = v;
        }
    }

    // ------------------------------------------------------------------ 0x00480200 family
    // Facility tails: N = 0xc7 Building (barracks, vtable 0x9d0780), N = 0x2e9b Siege (workshop, vtable 0x9d0980).
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::put_ / get_BuildingTail / SiegeTail; 0 = raw.
    template <u32 N>
    i32 save_79(void* file) {
        static_assert(N == 0xc7 || N == 0x2e9b, "Facility tail without schema");
        if (joan::orig::arg::as<i32>(joan::orig::as_facility(this)->save_base(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x004800a0
        i32 ok;
        if (joan::save::fieldwise()) {
            if constexpr (N == 0xc7) {
                joan::save::Out o(*joan::orig::as_file(file), joan::save::kBuildingTail, &jo::self<jo::Building>(this)->unit_id);
                joan::save::put_BuildingTail(o, *jo::self<jo::Building>(this));
                ok = o.end();  // File::file_write 0x005872e0
            } else {
                joan::save::Out o(*joan::orig::as_file(file), joan::save::kSiegeTail, &jo::self<jo::Siege>(this)->i32_79);
                joan::save::put_SiegeTail(o, *jo::self<jo::Siege>(this));
                ok = o.end();  // File::file_write 0x005872e0
            }
        } else if constexpr (N == 0xc7) {
            ok = file_write(file, &jo::self<jo::Building>(this)->unit_id, N);
        } else {
            ok = file_write(file, &jo::self<jo::Siege>(this)->i32_79, N);
        }
        if (ok == 0) return 0;
        return 1;
    }
    template <u32 N>
    i32 load_79(void* file) {
        static_assert(N == 0xc7 || N == 0x2e9b, "Facility tail without schema");
        if (joan::orig::arg::as<i32>(joan::orig::as_sub(this)->load_75(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x00480110
        i32 ok;
        if (joan::save::fieldwise()) {
            if constexpr (N == 0xc7) {
                joan::save::In in(*joan::orig::as_file(file), joan::save::kBuildingTail, &jo::self<jo::Building>(this)->unit_id);  // File::file_read 0x005873a0
                joan::save::get_BuildingTail(in, *jo::self<jo::Building>(this));
                ok = in.end();
            } else {
                joan::save::In in(*joan::orig::as_file(file), joan::save::kSiegeTail, &jo::self<jo::Siege>(this)->i32_79);  // File::file_read 0x005873a0
                joan::save::get_SiegeTail(in, *jo::self<jo::Siege>(this));
                ok = in.end();
            }
        } else if constexpr (N == 0xc7) {
            ok = file_read(file, &jo::self<jo::Building>(this)->unit_id, N);
        } else {
            ok = file_read(file, &jo::self<jo::Siege>(this)->i32_79, N);
        }
        if (ok == 0) return 0;
        virtual_call(this, 0x5c);  // Siege: Obj::reset_4803c0 0x004803c0 (life_bar)
        return 1;
    }
    // ------------------------------------------------------------------ 0x00481290 family
    // MapObject tails: N = 0xb Order0b, 0 Order00 (an empty chunk), 0x19 Gate, 0xc Order0c_a / Order0c_c / Order0c_b
    // (same tail shape, one schema), 0x18 the class of 0x00481890 (vtable not referenced, no C++ class: its bytes
    // after the MapObject), 0x45 Order45, 0x2d Wall. [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2):
    // save::put_ / get_<Class>Tail; 0 = raw.
    template <u32 N>
    i32 save_c2(void* file) {
        if (joan::orig::arg::as<i32>(joan::orig::as_map_object(this)->save(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x00481ba0
        i32 ok;
        if (joan::save::fieldwise()) {
            joan::orig::File& fl = *joan::orig::as_file(file);
            const auto put = [&](const joan::save::Record& rec, const auto* obj, auto fn) -> i32 {
                joan::save::Out o(fl, rec, self() + sizeof(jo::MapObject));
                fn(o, *obj);
                return o.end();  // File::file_write 0x005872e0
            };
            if constexpr (N == 0xb) {
                ok = put(joan::save::kOrder0bTail, jo::self<jo::Order0b>(this), [](joan::save::Out& o, const jo::Order0b& r) { joan::save::put_Order0bTail(o, r); });
            } else if constexpr (N == 0x19) {
                ok = put(joan::save::kGateTail, jo::self<jo::Gate>(this), [](joan::save::Out& o, const jo::Gate& r) { joan::save::put_GateTail(o, r); });
            } else if constexpr (N == 0xc) {
                ok = put(joan::save::kOrder0cTail, jo::self<jo::Order0c_a>(this), [](joan::save::Out& o, const jo::Order0c_a& r) { joan::save::put_Order0cTail(o, r); });
            } else if constexpr (N == 0x45) {
                ok = put(joan::save::kOrder45Tail, jo::self<jo::Order45>(this), [](joan::save::Out& o, const jo::Order45& r) { joan::save::put_Order45Tail(o, r); });
            } else if constexpr (N == 0x2d) {
                ok = put(joan::save::kWallTail, jo::self<jo::Wall>(this), [](joan::save::Out& o, const jo::Wall& r) { joan::save::put_WallTail(o, r); });
            } else {
                static_assert(N == 0 || N == 0x18, "MapObject tail without schema");
                const u8* const tail = self() + sizeof(jo::MapObject);  // 0xc2 on x86
                joan::save::Out o(joan::save::port(fl), N, N == 0 ? "Order00Tail" : "MapObject18Tail", tail);
                if constexpr (N != 0) o.bytes(tail, N);
                ok = o.end();  // File::file_write 0x005872e0
            }
        } else {
            ok = file_write(file, self() + sizeof(jo::MapObject), N);
        }
        if (ok == 0) return 0;
        return 1;
    }
    template <u32 N>
    i32 load_c2(void* file) {
        if (joan::orig::arg::as<i32>(joan::orig::as_map_object(this)->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x00481cd0
        i32 ok;
        if (joan::save::fieldwise()) {
            joan::orig::File& fl = *joan::orig::as_file(file);
            const auto get = [&](const joan::save::Record& rec, auto* obj, auto fn) -> i32 {
                joan::save::In in(fl, rec, self() + sizeof(jo::MapObject));  // File::file_read 0x005873a0
                fn(in, *obj);
                return in.end();
            };
            if constexpr (N == 0xb) {
                ok = get(joan::save::kOrder0bTail, jo::self<jo::Order0b>(this), [](joan::save::In& i, jo::Order0b& r) { joan::save::get_Order0bTail(i, r); });
            } else if constexpr (N == 0x19) {
                ok = get(joan::save::kGateTail, jo::self<jo::Gate>(this), [](joan::save::In& i, jo::Gate& r) { joan::save::get_GateTail(i, r); });
            } else if constexpr (N == 0xc) {
                ok = get(joan::save::kOrder0cTail, jo::self<jo::Order0c_a>(this), [](joan::save::In& i, jo::Order0c_a& r) { joan::save::get_Order0cTail(i, r); });
            } else if constexpr (N == 0x45) {
                ok = get(joan::save::kOrder45Tail, jo::self<jo::Order45>(this), [](joan::save::In& i, jo::Order45& r) { joan::save::get_Order45Tail(i, r); });
            } else if constexpr (N == 0x2d) {
                ok = get(joan::save::kWallTail, jo::self<jo::Wall>(this), [](joan::save::In& i, jo::Wall& r) { joan::save::get_WallTail(i, r); });
            } else {
                static_assert(N == 0 || N == 0x18, "MapObject tail without schema");
                u8* const tail = self() + sizeof(jo::MapObject);  // 0xc2 on x86
                joan::save::In in(joan::save::port(fl), N, N == 0 ? "Order00Tail" : "MapObject18Tail", tail);  // File::file_read 0x005873a0
                if constexpr (N != 0) in.bytes(tail, N);
                ok = in.end();
            }
        } else {
            ok = file_read(file, self() + sizeof(jo::MapObject), N);
        }
        if (ok == 0) return 0;
        virtual_call(this, 0x78);  // Gate 0x00481540 / Wall 0x00481b40 null their pointers
        return 1;
    }

    // ------------------------------------------------------------------ 0x00481b40
    H* reset_d2_da_e7_eb() {
        jo::self<jo::Wall>(this)->actor_a = nullptr;
        jo::self<jo::Wall>(this)->actor_b = nullptr;
        jo::self<jo::Wall>(this)->block_ids = 0;
        jo::self<jo::Wall>(this)->block_count = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x00483430
    i32 load_483430(void* file) {
        c_load_483430.hit();
        // Profile 0x48 + name 0x20 field by field (save::s5::read_profile_head, re/a6/SAVES_PLAN.md S5; [Debug]
        // SaveFieldwise=0: the raw reads)
        if (joan::save::s5::read_profile_head(*jo::as_file(file), *jo::self<jo::Profile>(this)) == 0) {  // 0x005873a0 x2
            file_fail(file);
            return 0;
        }
        jo::self<jo::Profile>(this)->unit_count = file_read_int(file);
        if (jo::self<jo::Profile>(this)->unit_count != 0) {
            const i32 line = static_cast<i32>(global<i16>(d::g_9ffcc0)) + 0x14;
            jo::view<u8*>(jo::self<jo::Profile>(this)->units) = static_cast<u8*>(heap_alloc(static_cast<u32>(jo::self<jo::Profile>(this)->unit_count) * static_cast<u32>(sizeof(jo::ProfileUnit)), line));  // 0x77-byte records on x86
            std::memset(joan::orig::arg::mptr(jo::view<u8*>(jo::self<jo::Profile>(this)->units)), 0, joan::orig::arg::u32(static_cast<u32>(jo::self<jo::Profile>(this)->unit_count) * static_cast<u32>(sizeof(jo::ProfileUnit))));  // memset
            for (i32 i = 0; i < jo::self<jo::Profile>(this)->unit_count; ++i) {
                if (joan::orig::arg::as<i32>(joan::orig::as_key(&jo::self<jo::Profile>(this)->units[i])->write(joan::orig::arg::mptr(file))) == 0) {  // 0x004c9420
                    file_fail(file);
                    return 0;
                }
            }
            for (i32 i = 0; i < jo::self<jo::Profile>(this)->unit_count; ++i) {
                if (joan::orig::arg::as<i32>(joan::orig::as_profile_unit(&jo::self<jo::Profile>(this)->units[i])->read(joan::orig::arg::mptr(file))) == 0) {  // 0x004c9530
                    file_fail(file);
                    return 0;
                }
            }
        } else {
            jo::self<jo::Profile>(this)->units = nullptr;
        }
        load_word_table(file, jo::self<jo::Profile>(this)->list_a_count, jo::self<jo::Profile>(this)->list_a, 0x34);  // +0x28 / +0x2c
        load_word_table(file, jo::self<jo::Profile>(this)->list_b_count, jo::self<jo::Profile>(this)->list_b, 0x43);  // +0x30 / +0x34
        jo::self<jo::Profile>(this)->i32_40 = file_read_int(file);
        return 1;
    }
    // count = file int; words table (count << 1 bytes) filled from the file (by member: the x64 offsets differ).
    static void load_word_table(void* file, i32& cnt, jo::ArrU16*& tab, i32 line_add) {
        cnt = file_read_int(file);
        if (cnt != 0) {
            const i32 line = static_cast<i32>(global<i16>(d::g_9ffcc0)) + line_add;
            tab = static_cast<jo::ArrU16*>(heap_alloc(static_cast<u32>(cnt) << 1, line));
            for (i32 j = 0; j < cnt; ++j) {
                const u16 w = file_read_word(file);
                reinterpret_cast<u16*>(tab)[j] = w;
            }
        } else {
            tab = nullptr;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_02", name, addr, fn)
B("Obj::load_47de30", 0x0047de30, &H::load_47de30);
B("Obj::save_480200", 0x00480200, &H::save_79<0xc7>);
B("Obj::load_480260", 0x00480260, &H::load_79<0xc7>);
B("Obj::save_4802f0", 0x004802f0, &H::save_79<0x2e9b>);
B("Obj::load_480350", 0x00480350, &H::load_79<0x2e9b>);
B("Obj::save_481290", 0x00481290, &H::save_c2<0xb>);
B("Obj::load_4812f0", 0x004812f0, &H::load_c2<0xb>);
B("Obj::save_481380", 0x00481380, &H::save_c2<0x0>);
B("Obj::load_4813e0", 0x004813e0, &H::load_c2<0x0>);
B("Obj::save_481470", 0x00481470, &H::save_c2<0x19>);
B("Obj::load_4814d0", 0x004814d0, &H::load_c2<0x19>);
B("Obj::save_4815c0", 0x004815c0, &H::save_c2<0xc>);
B("Obj::load_481620", 0x00481620, &H::load_c2<0xc>);
B("Obj::save_4816b0", 0x004816b0, &H::save_c2<0xc>);
B("Obj::load_481710", 0x00481710, &H::load_c2<0xc>);
B("Obj::save_4817a0", 0x004817a0, &H::save_c2<0xc>);
B("Obj::load_481800", 0x00481800, &H::load_c2<0xc>);
B("Obj::save_481890", 0x00481890, &H::save_c2<0x18>);
B("Obj::load_4818f0", 0x004818f0, &H::load_c2<0x18>);
B("Obj::save_481980", 0x00481980, &H::save_c2<0x45>);
B("Obj::load_4819e0", 0x004819e0, &H::load_c2<0x45>);
B("Obj::save_481a70", 0x00481a70, &H::save_c2<0x2d>);
B("Obj::load_481ad0", 0x00481ad0, &H::load_c2<0x2d>);
B("Obj::reset_d2_da_e7_eb", 0x00481b40, &H::reset_d2_da_e7_eb);
B("Obj::load_483430", 0x00483430, &H::load_483430);
#undef B

}  // namespace game_b103_02
}  // namespace

// ---------------------------------------- from modules/game_b85_01.cpp (game/b85_01)
//
//    - SaveEntry::set 0x0047e200 (16 args) ret 0x40 (gfile3.cpp).
//
//    - List::is_null_at_4 0x00484740 (i) ret 4, List::is_null 0x00484870 (i) ret 4,
//      List::get_at_4 0x00484710 (i) ret 4 (0x0041b3f0 on this / this + 4).
//
//  * 0x0047e200 leaves +0x24/+0x25 and +0x2f..+0x3e untouched (as the original); +0x3f is only
//    written for type 4 / 8 / 0x80.
namespace {
namespace game_b85_01 {

// 0x0041b3f0 List::at(i) ret 4.
void* list_at(const void* list, std::int32_t i) { return joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)); }

JOAN_COUNTER(c_entry_set, "SaveEntry::set");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    // ---- 0x0047e200 SaveEntry::set(...) ret 0x40 (gfile3.cpp) -------------------------------
    void entry_set(std::int32_t type, std::uint32_t a4, std::uint16_t a8, std::uint32_t aa,
                   std::uint16_t ae, std::uint8_t a12, std::uint32_t v0, std::uint32_t v1,
                   std::uint32_t v2, std::uint32_t a1f, std::uint8_t a23, std::uint32_t a26,
                   std::uint32_t a2a, std::uint8_t a2e, void* a3f, std::uint16_t a10) {
        c_entry_set.hit();
        jo::self<jo::SaveEntry>(this)->type = type;
        jo::self<jo::SaveEntry>(this)->target_kind = a4;
        jo::self<jo::SaveEntry>(this)->target_id = a8;
        jo::self<jo::SaveEntry>(this)->ref_kind = aa;
        jo::self<jo::SaveEntry>(this)->ref_id = ae;
        jo::self<jo::SaveEntry>(this)->u8_12 = a12;
        jo::self<jo::SaveEntry>(this)->values[0] = v0;
        jo::self<jo::SaveEntry>(this)->values[1] = v1;
        jo::self<jo::SaveEntry>(this)->values[2] = v2;
        jo::self<jo::SaveEntry>(this)->range = a1f;
        jo::self<jo::SaveEntry>(this)->follow_ref = a23;
        jo::self<jo::SaveEntry>(this)->next_time = a26;
        jo::self<jo::SaveEntry>(this)->expire_time = a2a;
        jo::self<jo::SaveEntry>(this)->active = a2e;
        jo::self<jo::SaveEntry>(this)->sub = a10;
        if (jo::self<jo::SaveEntry>(this)->type == 4 || jo::self<jo::SaveEntry>(this)->type == 8 || jo::self<jo::SaveEntry>(this)->type == 0x80)
            jo::self<jo::SaveEntry>(this)->scene = static_cast<jo::SceneS*>(a3f);
    }

    std::int32_t is_null_at_4(std::int32_t i) { return list_at(&jo::as_units(this)->list, i) == nullptr ? 1 : 0; }  // 0x00484740 (list after a pointer at +0: +4 on x86)
    std::int32_t is_null(std::int32_t i) { return list_at(this, i) == nullptr ? 1 : 0; }         // 0x00484870
    void* get_at_4(std::int32_t i) { return list_at(&jo::as_units(this)->list, i); }  // (+4 on x86)                        // 0x00484710
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_01", name, addr, fn)
B("SaveEntry::set", 0x0047e200, &H::entry_set);
B("List::is_null_at_4", 0x00484740, &H::is_null_at_4);
B("List::is_null", 0x00484870, &H::is_null);
B("List::get_at_4", 0x00484710, &H::get_at_4);
#undef B

}  // namespace game_b85_01
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
//
//      0x0047e300 Unit::reset_links()        clears +0x22.. +0x357, sets +0x299 from the type's group
//
//      0x0047e640 IdList::write(stream)      ret 4
//
//      0x0047e700 IdList::read(stream)       ret 4 (SEH frame only guards new + ctor)
namespace {
namespace game_b92_02 {

JOAN_COUNTER(c_reset_links, "Unit::reset_links");
JOAN_COUNTER(c_idlist_write, "IdList::write");
JOAN_COUNTER(c_idlist_read, "IdList::read");

struct H {
    // 0x0047e300
    void reset_links() {
        c_reset_links.hit();
        jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->move_mode) = 0;
        jo::self<jo::Unit>(this)->u32_26 = 0;
        void* const t = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->type_id))));  // 0x0042d0a0
        if (jo::view<std::uint32_t>(jo::as_unit_type(t)->script_id) != 0) {
            void* const t2 =
                joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->type_id))));  // 0x0042d0a0
            void* const g = joan::orig::arg::as<void*>(joan::orig::as_uscr_res(obj(d::g_uscr_res))->entry(static_cast<std::int32_t>(reinterpret_cast<const std::int16_t&>(jo::as_unit_type(t2)->script_id))));  // 0x00484540
            jo::self<jo::Unit>(this)->script = static_cast<jo::ScriptBlock*>(jo::as_unit_script(g)->object);  // UScrRes::entry: a UnitScript
        } else {
            jo::self<jo::Unit>(this)->script = nullptr;
        }
        jo::self<jo::Unit>(this)->in_scene = 0;
        jo::self<jo::Unit>(this)->instantiated = 0;
        // +0x2f1 .. +0x331
        jo::self<jo::Unit>(this)->anim_set = nullptr;
        jo::self<jo::Unit>(this)->actor = nullptr;
        jo::self<jo::Unit>(this)->actor_b = nullptr;
        jo::self<jo::Unit>(this)->actor_def_b = nullptr;
        jo::self<jo::Unit>(this)->anim_set_b = nullptr;
        jo::self<jo::Unit>(this)->alt_mesh_state = 0;
        jo::self<jo::Unit>(this)->actor_c = nullptr;
        jo::self<jo::Unit>(this)->actor_def_c = nullptr;
        jo::self<jo::Unit>(this)->anim_set_c = nullptr;
        jo::self<jo::Unit>(this)->u32_315 = 0;
        jo::self<jo::Unit>(this)->model = nullptr;
        jo::self<jo::Unit>(this)->draw = nullptr;
        jo::self<jo::Unit>(this)->model_b = nullptr;
        jo::self<jo::Unit>(this)->weapon_model = nullptr;
        jo::self<jo::Unit>(this)->item_ref = 0;
        jo::self<jo::Unit>(this)->trail_a = nullptr;
        jo::self<jo::Unit>(this)->trail_b = nullptr;
        jo::self<jo::Unit>(this)->cell = nullptr;
        jo::self<jo::Unit>(this)->cell_next = nullptr;
        jo::self<jo::Unit>(this)->cell_prev = nullptr;
        jo::self<jo::Unit>(this)->scene = nullptr;
        jo::self<jo::Unit>(this)->u32_34d = 0;
        jo::self<jo::Unit>(this)->u32_349 = 0;
        jo::self<jo::Unit>(this)->use_action_alt = 0;
    }

    // 0x0047e640
    std::int32_t idlist_write(void* stream) {
        c_idlist_write.hit();
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): save::put_UpgradeList / put_UpgradeListEntry;
        // 0 = the raw writes.
        const bool fieldwise = joan::save::fieldwise();
        std::int32_t ok;
        if (fieldwise) {
            joan::save::Out o(*joan::orig::as_file(stream), joan::save::kUpgradeList, this);
            joan::save::put_UpgradeList(o, *jo::self<jo::UpgradeList>(this));
            ok = o.end();  // File::file_write 0x005872e0
        } else {
            ok = joan::orig::as_file(stream)->file_write(static_cast<void*>(this), 4u);  // 0x005872e0
        }
        if (ok == 0) return 0;
        if (jo::self<jo::UpgradeList>(this)->list == nullptr) return 1;
        const std::int32_t n = joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::UpgradeList>(this)->list))->size();  // 0x00417ad0
        joan::orig::as_file(stream)->file_put_long(joan::orig::arg::i32(n));                 // 0x00587da0
        for (std::int32_t i = 1; i <= joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::UpgradeList>(this)->list))->size(); ++i) {  // 0x00417ad0
            void* const item = joan::orig::arg::as<void*>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::UpgradeList>(this)->list))->get(joan::orig::arg::i32(i)));                    // 0x00417200
            if (fieldwise) {
                joan::save::Out o(*joan::orig::as_file(stream), joan::save::kUpgradeListEntry, item);
                joan::save::put_UpgradeListEntry(o, *static_cast<const jo::UpgradeListEntry*>(item));
                ok = o.end();  // File::file_write 0x005872e0
            } else {
                ok = joan::orig::as_file(stream)->file_write(joan::orig::arg::ptr(item), 8u);  // 0x005872e0
            }
            if (ok == 0) return 0;
        }
        return 1;
    }

    // 0x0047e700
    std::int32_t idlist_read(void* stream) {
        c_idlist_read.hit();
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): save::get_UpgradeList / get_UpgradeListEntry;
        // 0 = the raw reads.
        const bool fieldwise = joan::save::fieldwise();
        std::int32_t ok;
        if (fieldwise) {
            joan::save::In in(*joan::orig::as_file(stream), joan::save::kUpgradeList, this);  // File::file_read 0x005873a0
            joan::save::get_UpgradeList(in, *jo::self<jo::UpgradeList>(this));
            ok = in.end();
        } else {
            ok = joan::orig::as_file(stream)->file_read(static_cast<void*>(this), 4u);  // 0x005873a0
        }
        if (ok == 0) return 0;
        if (jo::self<jo::UpgradeList>(this)->list == nullptr) return 1;
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));                                 // operator new (0x1d on x86)
        void* const list = mem != nullptr ? joan::orig::dynarray(mem)->construct(8, 0x1e, 0) : nullptr;  // 0x005843d0
        jo::view<void*>(jo::self<jo::UpgradeList>(this)->list) = list;
        const std::int32_t n = joan::orig::as_file(stream)->file_get_long();            // 0x00587e20
        for (std::int32_t i = 1; i <= n; ++i) {
            joan::orig::dynarray(jo::view<void*>(jo::self<jo::UpgradeList>(this)->list))->linkin(nullptr);                            // 0x00584620
            const std::int32_t last = joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::UpgradeList>(this)->list))->cursor();  // 0x00417520
            void* const item = joan::orig::arg::as<void*>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::UpgradeList>(this)->list))->get(joan::orig::arg::i32(last)));      // 0x00417200
            if (fieldwise) {
                joan::save::In in(*joan::orig::as_file(stream), joan::save::kUpgradeListEntry, item);  // File::file_read 0x005873a0
                joan::save::get_UpgradeListEntry(in, *static_cast<jo::UpgradeListEntry*>(item));
                ok = in.end();
            } else {
                ok = joan::orig::as_file(stream)->file_read(joan::orig::arg::mptr(item), 8u);  // 0x005873a0
            }
            if (ok == 0) return 0;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Unit::reset_links", 0x0047e300, &H::reset_links);
B("IdList::write", 0x0047e640, &H::idlist_write);
B("IdList::read", 0x0047e700, &H::idlist_read);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
//
//      0x0047e540 Obj::write_58(file)            (thiscall, ret 4)
//
//      0x0047f1e0 Obj::write_list(file)          (thiscall, ret 4)
//
//      0x00480c00 List::save(file)               (thiscall, ret 4)   - virtual +0x54 per item.
//
//      0x00482dc0 Obj::write_6c(file)            (thiscall, ret 4)
//
//      0x00483ca0 Obj::read_1b2(file)            (thiscall, ret 4)   - two optional buffers.
//
//      0x00484b60 Obj::clear_484b60              (thiscall, ret 0)
//
//      0x00485bf0 Obj::read_485bf0(file)         (thiscall, ret 4)
namespace {
namespace game_b95_02 {

// File::write(ptr, n) 0x005872e0 / File::read(ptr, n) 0x005873a0
i32 file_write(void* file, const void* p, u32 n) { return joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }
i32 file_read(void* file, void* p, u32 n) { return joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n)); }

JOAN_COUNTER(c_write_list, "Obj::write_list");
JOAN_COUNTER(c_list_save, "List::save");
JOAN_COUNTER(c_read_1b2, "Obj::read_1b2");
JOAN_COUNTER(c_clear_484b60, "Obj::clear_484b60");
JOAN_COUNTER(c_read_485bf0, "Obj::read_485bf0");

struct H {
    // ------------------------------------------------------------------ 0x0047e540
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): save::put_Group; 0 = the raw write.
    i32 write_58(void* file) {
        if (!joan::save::fieldwise()) return file_write(file, this, 0x58) != 0 ? 1 : 0;
        joan::save::Out o(*joan::orig::as_file(file), joan::save::kGroup, this);
        joan::save::put_Group(o, *jo::self<jo::Group>(this));
        return o.end() != 0 ? 1 : 0;  // File::file_write 0x005872e0
    }

    // ------------------------------------------------------------------ 0x0047f1e0
    // The 0x24-byte records go to the file raw. They can hold addresses of the data image (the B0 SaveAudit found a
    // vtable 0x009d9a2c and 0x00a403a0 in them - stale heap bytes of fields nobody sets, as far as the code shows):
    // written as ORIGINAL addresses (img::save_words), as joan.exe wrote them; PointList::load (game_b97_02.cpp)
    // relocates them back. Saves stay identical to the original game's and load in both directions.
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S0): each record field by field (save::put_PathPoint: every
    // dword under Ptr::Image, i.e. the bytes img::save_words gives); SaveFieldwise=0: the old raw path.
    i32 write_list(void* file) {
        c_write_list.hit();
        const bool fieldwise = joan::save::fieldwise();
        alignas(4) u8 rec[0x24];
        const auto put = [&](const void* p) -> i32 {
            if (fieldwise) {
                joan::save::Out o(*joan::orig::as_file(file), joan::save::kPathPoint, p);
                joan::save::put_PathPoint(o, *static_cast<const jo::PathPoint*>(p));
                return o.end();  // File::file_write 0x005872e0
            }
            img::save_words(rec, p, 0x24u);
            return file_write(file, rec, 0x24);
        };
        if (put(&jo::self<jo::PointList>(this)->start) == 0) return 0;  // +0x1d
        joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(list_count(this)));  // 0x00587da0
        for (i32 i = 1; i <= list_count(this); ++i) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(i)));  // 0x00417200
            if (put(e) == 0) return 0;
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00480c00
    i32 list_save(void* file) {
        c_list_save.hit();
        joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(list_count(this)));  // 0x00587da0
        for (i32 i = 1; i <= list_count(this); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->empty_at_e(joan::orig::arg::i32(i))) != 0) {  // 0x0044cf80
                joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(static_cast<i32>(0)));  // 0x00587ba0 (word)
            } else {
                u8* const item = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->get_e(joan::orig::arg::i32(i)));  // 0x0043c450
                joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(item)->index)));  // 0x00587ba0 (word)
                void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_e(joan::orig::arg::i32(i)));   // 0x0043c450
                if (virtual_call<i32>(p, 0x54, file) == 0) return 0;
            }
        }
        joan::orig::dynarray_b(this)->write_empty_room(joan::orig::arg::file(file));  // 0x00585b40
        return 1;
    }

    // ------------------------------------------------------------------ 0x00482dc0
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::put_Order (Missile); 0 = the raw write.
    i32 write_6c(void* file) {
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kOrder, this);
            joan::save::put_Order(o, *jo::self<jo::Missile>(this));
            return o.end() != 0 ? 1 : 0;  // File::file_write 0x005872e0
        }
        return file_write(file, this, 0x6c) != 0 ? 1 : 0;
    }

    // ------------------------------------------------------------------ 0x00483ca0
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S4): save::get_ScenarioEntry; 0 = the raw read.
    i32 read_1b2(void* file) {
        c_read_1b2.hit();
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kScenarioEntry, this);  // File::file_read 0x005873a0
            joan::save::get_ScenarioEntry(in, *jo::self<jo::ScenarioEntry>(this));
            if (in.end() == 0) return 0;
        } else if (file_read(file, this, 0x1b2) == 0) {
            return 0;
        }
        if (jo::self<jo::ScenarioEntry>(this)->name != nullptr) {
            const i32 n = joan::orig::arg::as<i16>(joan::orig::as_file(file)->file_get_short());  // 0x00587c20
            jo::view<void*>(jo::self<jo::ScenarioEntry>(this)->name) = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(n), str(d::s_d_ww_gfile3_cpp), joan::orig::arg::i32(global<i16>(d::g_9ffcc4) + 0xa));  // 0x0058e7b0
            file_read(file, jo::view<void*>(jo::self<jo::ScenarioEntry>(this)->name), static_cast<u32>(n));
        }
        if (jo::self<jo::ScenarioEntry>(this)->name2 != nullptr) {
            const i32 n = joan::orig::arg::as<i16>(joan::orig::as_file(file)->file_get_short());  // 0x00587c20
            jo::view<void*>(jo::self<jo::ScenarioEntry>(this)->name2) = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(n), str(d::s_d_ww_gfile3_cpp), joan::orig::arg::i32(global<i16>(d::g_9ffcc4) + 0x11));  // 0x0058e7b0
            file_read(file, jo::view<void*>(jo::self<jo::ScenarioEntry>(this)->name2), static_cast<u32>(n));
        }
        joan::orig::as_scenario_entry(this)->clear_186();  // 0x00483dd0
        return 1;
    }

    // ------------------------------------------------------------------ 0x00484b60
    void* clear_484b60() {
        c_clear_484b60.hit();
        jo::self<jo::Scene>(this)->sky = nullptr;  // +0x18
        jo::self<jo::Scene>(this)->plane_1c = nullptr;  // +0x1c
        jo::self<jo::Scene>(this)->sectors = nullptr;  // +0x2c
        jo::self<jo::Scene>(this)->sector_count = 0;  // +0x24
        jo::self<jo::Scene>(this)->tree = nullptr;  // +0x30
        jo::self<jo::Scene>(this)->visible = nullptr;  // +0x34
        jo::self<jo::Scene>(this)->visible_count = 0;  // +0x38
        jo::self<jo::Scene>(this)->list_40 = nullptr;  // +0x40
        jo::self<jo::Scene>(this)->list_40_count = 0;  // +0x44
        jo::self<jo::Scene>(this)->light_count = 0;  // +0x4c
        jo::self<jo::Scene>(this)->lights = nullptr;  // +0x54
        jo::self<jo::Scene>(this)->occluder_total = 0;  // +0x5c
        jo::self<jo::Scene>(this)->occluders = nullptr;  // +0x58
        jo::self<jo::Scene>(this)->plane_71 = nullptr;  // +0x71
        jo::self<jo::Scene>(this)->actor_count = 0;  // +0x75
        jo::self<jo::Scene>(this)->actors = nullptr;  // +0x7d
        jo::self<jo::Scene>(this)->plane_81 = nullptr;  // +0x81
        jo::self<jo::Scene>(this)->camera = nullptr;  // +0xb5
        jo::self<jo::Scene>(this)->ptr_b9 = nullptr;  // +0xb9
        jo::self<jo::Scene>(this)->material_count = 0;  // +0xc1
        jo::self<jo::Scene>(this)->materials = nullptr;  // +0xc5
        jo::self<jo::Scene>(this)->region_buffer = nullptr;  // +0xc9
        jo::self<jo::Scene>(this)->buffer_d1 = nullptr;  // +0xd1
        return this;
    }

    // ------------------------------------------------------------------ 0x00485bf0
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S3): the LightSys (g_a47e08) chunks field by field
    // (save::get_LightSys*, the event dwords, then the current light set and the two fade states); 0 = the raw reads.
    // Read results ignored, as in the original.
    i32 read_485bf0(void* file) {
        c_read_485bf0.hit();
        if (joan::save::fieldwise()) {
            namespace sv = joan::save;
            jo::LightSys& ls = *jo::self<jo::LightSys>(this);
            const auto rd = [&](const sv::Record& r, void* legacy, auto fn) {
                sv::In in(*joan::orig::as_file(file), r, legacy);  // File::file_read 0x005873a0
                fn(in);
                in.end();
            };
            rd(sv::kLightSysEventAChance, &ls.event_a_chance, [&](sv::In& c) { sv::get_LightSysEventAChance(c, ls); });
            rd(sv::kLightSysEventARate, &ls.event_a_rate, [&](sv::In& c) { sv::get_LightSysEventARate(c, ls); });
            rd(sv::kLightSysEventATime, &ls.event_a_time, [&](sv::In& c) { sv::get_LightSysEventATime(c, ls); });
            rd(sv::kLightSysEventAState, &ls.event_a_state, [&](sv::In& c) { sv::get_LightSysEventAState(c, ls); });
            rd(sv::kLightSysEventBChance, &ls.event_b_chance, [&](sv::In& c) { sv::get_LightSysEventBChance(c, ls); });
            rd(sv::kLightSysEventBRate, &ls.event_b_rate, [&](sv::In& c) { sv::get_LightSysEventBRate(c, ls); });
            rd(sv::kLightSysEventBTime, &ls.event_b_time, [&](sv::In& c) { sv::get_LightSysEventBTime(c, ls); });
            rd(sv::kLightSysEventBState, &ls.event_b_state, [&](sv::In& c) { sv::get_LightSysEventBState(c, ls); });
            rd(sv::kLightSysCurrent, &ls.current, [&](sv::In& c) { sv::get_LightSysCurrent(c, ls); });
            rd(sv::kLightSysStateA, &ls.state_a, [&](sv::In& c) { sv::get_LightSysStateA(c, ls); });
            rd(sv::kLightSysStateB, &ls.state_b, [&](sv::In& c) { sv::get_LightSysStateB(c, ls); });
            return 1;
        }
        jo::LightSys& l = *jo::self<jo::LightSys>(this);
        void* const events[8] = {&l.event_a_chance, &l.event_a_rate, &l.event_a_time, &l.event_a_state,
                                 &l.event_b_chance, &l.event_b_rate, &l.event_b_time, &l.event_b_state};  // +0x58c .. +0x5a8
        for (void* const e : events) file_read(file, e, 4);
        file_read(file, &l.current, 0x18c);
        file_read(file, &l.state_a, 0x194);
        file_read(file, &l.state_b, 0x194);
        return 1;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Obj::write_58", 0x0047e540, &H::write_58);
B95("Obj::write_list", 0x0047f1e0, &H::write_list);
B95("List::save", 0x00480c00, &H::list_save);
B95("Obj::write_6c", 0x00482dc0, &H::write_6c);
B95("Obj::read_1b2", 0x00483ca0, &H::read_1b2);
B95("Obj::clear_484b60", 0x00484b60, &H::clear_484b60);
B95("Obj::read_485bf0", 0x00485bf0, &H::read_485bf0);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
//
//      0x0047e580 Sub::load_58(file)            (thiscall, ret 4)
//
//      0x0047e9f0 Sub::load_parts(file)         (thiscall, ret 4) - SEH frame (new + ctor 0x004571e0).
//
//      0x0047f290 PointList::load(file)         (thiscall, ret 4)
//
//      0x004801d0 Obj::clear_71_75              (thiscall, ret 0) - returns this.
//
//      0x00482c60 Sub::load_objects(file)       (thiscall, ret 4) - virtual +0x44 call.
//
//      0x00483b10 Sub::save_4(file)             (thiscall, ret 4)
//
//      0x004843d0 Sub::save_14d(file)           (thiscall, ret 4)
//
//      0x00484f50 Obj::clear                    (thiscall, ret 0) - returns this.
//
//      0x00485dc0 List::load_e0(file)           (thiscall, ret 4)
//
//  - 0x0047e9f0: the SEH frame only guards new + ctor (delete on throw); normal path reproduced.
namespace {
namespace game_b97_02 {

void* const kObjects = obj(d::g_items);

JOAN_COUNTER(c_load_parts, "Sub::load_parts");
JOAN_COUNTER(c_pl_load, "PointList::load");
JOAN_COUNTER(c_load_objects, "Sub::load_objects");
JOAN_COUNTER(c_save_14d, "Sub::save_14d");
JOAN_COUNTER(c_load_e0, "List::load_e0");

// File I/O on the save file object.
i32 file_write(void* file, const void* p, i32 n) {
    return joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n));  // 0x005872e0
}
i32 file_read(void* file, void* p, i32 n) {
    return joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n));  // 0x005873a0
}
i32 file_count(void* file) { return joan::orig::as_file(file)->file_get_long(); }   // 0x00587e20
i16 file_word(void* file) { return joan::orig::arg::as<i16>(joan::orig::as_file(file)->file_get_short()); }  // 0x00587c20

struct H {
    // ------------------------------------------------------------------ 0x0047e580
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): save::get_Group; 0 = the raw read.
    i32 load_58(void* file) {
        if (!joan::save::fieldwise()) return file_read(file, this, 0x58) != 0 ? 1 : 0;
        joan::save::In in(*joan::orig::as_file(file), joan::save::kGroup, this);  // File::file_read 0x005873a0
        joan::save::get_Group(in, *jo::self<jo::Group>(this));
        return in.end() != 0 ? 1 : 0;
    }

    // ------------------------------------------------------------------ 0x0047e9f0
    i32 load_parts(void* file) {
        c_load_parts.hit();
        {  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): save::get_UnitData; 0 = the raw read
            i32 ok;
            if (joan::save::fieldwise()) {
                joan::save::In in(*joan::orig::as_file(file), joan::save::kUnitData, this);  // File::file_read 0x005873a0
                joan::save::get_UnitData(in, *jo::self<jo::UnitData>(this));
                ok = in.end();
            } else {
                ok = file_read(file, this, 0x1e);
            }
            if (ok == 0) return 0;
        }
        for (i32 i = 0; i < 1; ++i) {
            for (i32 j = 0; j < 2; ++j) {
                jo::HeldItem*& slot = jo::self<jo::UnitData>(this)->hands[i * 2 + j];  // +2 + i * 8 + j * 4
                if (slot != nullptr) {
                    slot = static_cast<jo::HeldItem*>(game_new(static_cast<std::uint32_t>(sizeof(jo::HeldItem))));  // 0x12 on x86
                    if (joan::orig::arg::as<i32>(joan::orig::as_item_stack(slot)->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047eda0
                }
            }
        }
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->equipped) != nullptr) {
            jo::view<void*>(jo::self<jo::UnitData>(this)->equipped) = game_new(static_cast<std::uint32_t>(sizeof(jo::HeldItem)));  // 0x12 on x86
            if (joan::orig::arg::as<i32>(joan::orig::as_item_stack(jo::view<void*>(jo::self<jo::UnitData>(this)->equipped))->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047eda0
        }
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item) != nullptr) {
            jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item) = game_new(static_cast<std::uint32_t>(sizeof(jo::HeldItem)));  // 0x12 on x86
            if (joan::orig::arg::as<i32>(joan::orig::as_item_stack(jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item))->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047eda0
        }
        if (!load_part(file, jo::self<jo::UnitData>(this)->list_12, 4, 1)) return 0;  // +0x12
        if (!load_part(file, jo::self<jo::UnitData>(this)->grid_16, 6, 8)) return 0;  // +0x16
        if (!load_part(file, jo::self<jo::UnitData>(this)->grid_1a, 6, 4)) return 0;  // +0x1a
        return 1;
    }
    bool load_part(void* file, jo::Grid*& slot, i32 b, i32 c) {
        if (slot == nullptr) return true;
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Grid)));  // 0x3e on x86
        void* const p = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_grid(mem)->Grid_4571e0()) : nullptr;  // 0x004571e0
        slot = static_cast<jo::Grid*>(p);
        joan::orig::as_grid(slot)->alloc(1, joan::orig::arg::i32(b), joan::orig::arg::i32(c));                         // 0x00457290
        return joan::orig::arg::as<i32>(joan::orig::as_grid(slot)->load(joan::orig::arg::mptr(file))) != 0;           // 0x0047ef30
    }

    // ------------------------------------------------------------------ 0x0047f290
    // The records may hold data image addresses, stored as original addresses (Obj::write_list, game_b95_02.cpp):
    // img::load_words moves them to joan_image (a save of joan.exe or of an earlier joan_re loads the same way).
    // SaveFieldwise=1: save::get_PathPoint (relocation per dword as img::load_words); a failed read leaves the raw
    // bytes, unrelocated, as before.
    i32 pl_load(void* file) {
        c_pl_load.hit();
        const bool fieldwise = joan::save::fieldwise();
        const auto get = [&](void* p) -> bool {
            if (fieldwise) {
                joan::save::In in(*joan::orig::as_file(file), joan::save::kPathPoint, p);  // File::file_read 0x005873a0
                if (!in.ok()) {
                    in.bytes(p, 0x24);
                    in.end();
                    return false;
                }
                joan::save::get_PathPoint(in, *static_cast<jo::PathPoint*>(p));
                return in.end() != 0;
            }
            if (file_read(file, p, 0x24) == 0) return false;
            img::load_words(p, 0x24);
            return true;
        };
        if (!get(&jo::self<jo::PointList>(this)->start)) return 0;  // +0x1d
        const i32 n = file_count(file);
        for (i32 i = 1; i <= n; ++i) {
            joan::orig::dynarray(this)->linkin(static_cast<void*>(nullptr));    // 0x00584620
            const i32 last = joan::orig::as_dynarray(this)->cursor();  // 0x00417520
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(last)));  // 0x00417200
            if (!get(e)) return 0;
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x004801d0
    void* clear_71_75() {
        jo::self<jo::Facility>(this)->prev = nullptr;
        jo::self<jo::Facility>(this)->next = nullptr;
        return this;
    }

    // ------------------------------------------------------------------ 0x00482c60
    i32 load_objects(void* file) {
        c_load_objects.hit();
        jo::DynArray* const list = &jo::as_units(this)->list;  // +4 on x86 (g_a50530: a pointer at +0)
        const i32 n = file_count(file);
        for (i32 i = 1; i <= n; ++i) {
            if (file_word(file) == 0) {
                joan::orig::dynarray(list)->add_blank(1);  // 0x00584bd0
            } else {
                const i16 id = file_word(file);
                void* local = joan::orig::as_units(kObjects)->list.ptr_at(static_cast<i32>(id));  // 0x00425d20
                joan::orig::dynarray_b(list)->linkin(&local);                                       // 0x005857a0
            }
        }
        joan::orig::dynarray_b(list)->read_empty_room(joan::orig::arg::file(file));  // 0x00585bb0
        for (i32 i = joan::orig::as_dynarray(list)->size(); i >= 1; --i) {  // 0x00417ad0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (e == nullptr) continue;
            const i16 k = jo::as_map_object(e)->class_id;
            if (k != 1 && k != 2) continue;
            using Virtual = i32 (Obj::*)();
            JOAN_MEMBER_POINTER_CHECK(Virtual);
            Virtual method{};
            std::memcpy(&method, &joan::call::vtable_of(e)[0x44 / 4], sizeof(void*));
            if ((reinterpret_cast<Obj*>(e)->*method)() == 0) continue;
            joan::orig::as_units(kObjects)->remove_49f370(static_cast<i32>(jo::as_map_object(e)->index));  // 0x0049f370
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00483b10
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S4): save::put_BattleState; 0 = the raw write.
    i32 save_4(void* file) {
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kBattleState, this);
            joan::save::put_BattleState(o, *jo::self<jo::BattleState>(this));
            return o.end() != 0 ? 1 : 0;  // File::file_write 0x005872e0
        }
        return file_write(file, this, 4) != 0 ? 1 : 0;
    }

    // ------------------------------------------------------------------ 0x004843d0
    // The Hud's message-log fields in nine chunks (save::kHudLog: the u16[40] arrays +0x157 / +0x1a7 and obj_time
    // +0x1f9 only half, as joan.exe writes them). SaveFieldwise=1: save::put_HudLog; 0 = the raw writes.
    i32 save_14d(void* file) {
        c_save_14d.hit();
        const jo::Hud& hud = *jo::self<jo::Hud>(this);
        const void* const chunk_at[9] = {&hud.blink_on, &hud.blink_state, &hud.blink_time, &hud.blink_start, &hud.obj_alpha, &hud.obj_1a7, &hud.obj_count, &hud.obj_time, &hud.obj_id};  // x64: the members, not this + x86 offset
        if (joan::save::fieldwise()) {
            for (u32 k = 0; k < 9; ++k) {
                const joan::save::HudChunk& c = joan::save::kHudLog[k];
                joan::save::Out o(joan::save::port(*joan::orig::as_file(file)), c.rec, chunk_at[k]);
                joan::save::put_HudLog(o, k, *jo::self<jo::Hud>(this));
                if (o.end() == 0) return 0;  // File::file_write 0x005872e0
            }
            return 1;
        }
        if (file_write(file, chunk_at[0], 1) == 0) return 0;
        if (file_write(file, chunk_at[1], 1) == 0) return 0;
        if (file_write(file, chunk_at[2], 4) == 0) return 0;
        if (file_write(file, chunk_at[3], 4) == 0) return 0;
        if (file_write(file, chunk_at[4], 0x28) == 0) return 0;
        if (file_write(file, chunk_at[5], 0x28) == 0) return 0;
        if (file_write(file, chunk_at[6], 2) == 0) return 0;
        if (file_write(file, chunk_at[7], 0x50) == 0) return 0;
        if (file_write(file, chunk_at[8], 0xa0) == 0) return 0;
        return 1;
    }

    // ------------------------------------------------------------------ 0x00484f50
    void* clear() {
        jo::self<jo::Cell>(this)->ptr_4 = nullptr;
        jo::self<jo::Cell>(this)->path_space = nullptr;
        jo::self<jo::Cell>(this)->path_space_b = nullptr;
        jo::self<jo::Cell>(this)->tracks = nullptr;
        jo::self<jo::Cell>(this)->camera_zones = nullptr;
        jo::self<jo::Cell>(this)->occluder_count = 0;
        jo::self<jo::Cell>(this)->p_84 = nullptr;
        jo::self<jo::Cell>(this)->rows = nullptr;
        jo::self<jo::Cell>(this)->units_tail = nullptr;
        jo::self<jo::Cell>(this)->units_head = nullptr;
        jo::self<jo::Cell>(this)->objects_tail = nullptr;
        jo::self<jo::Cell>(this)->objects_head = nullptr;
        jo::self<jo::Cell>(this)->items_tail = nullptr;
        jo::self<jo::Cell>(this)->items_head = nullptr;
        jo::self<jo::Cell>(this)->places_tail = nullptr;
        jo::self<jo::Cell>(this)->places_head = nullptr;
        jo::self<jo::Cell>(this)->sector_count = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x00485dc0
    i32 load_e0(void* file) {
        c_load_e0.hit();
        joan::orig::as_list(obj(d::g_territory_array))->delete_all_4e49b0();  // 0x004e49b0
        const i32 n = file_count(file);
        for (i32 i = 1; i <= n; ++i) {
            void* local = game_new(static_cast<u32>(sizeof(jo::ZoneDef)));  // 0xe0 on x86
            if (joan::save::fieldwise()) {  // [Debug] SaveFieldwise (SAVES_PLAN S4): save::get_ZoneDef, 0 = the raw read
                joan::save::In in(*joan::orig::as_file(file), joan::save::kZoneDef, local);  // File::file_read 0x005873a0
                joan::save::get_ZoneDef(in, *static_cast<jo::ZoneDef*>(local));
                if (in.end() == 0) return 0;
            } else if (file_read(file, local, 0xe0) == 0) {
                return 0;
            }
            joan::orig::dynarray(this)->linkin(&local);  // 0x00584620
        }
        return 1;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Sub::load_58", 0x0047e580, &H::load_58);
B97("Sub::load_parts", 0x0047e9f0, &H::load_parts);
B97("PointList::load", 0x0047f290, &H::pl_load);
B97("Obj::clear_71_75", 0x004801d0, &H::clear_71_75);
B97("Sub::load_objects", 0x00482c60, &H::load_objects);
B97("Sub::save_4", 0x00483b10, &H::save_4);
B97("Sub::save_14d", 0x004843d0, &H::save_14d);
B97("Obj::clear_484f50", 0x00484f50, &H::clear);
B97("List::load_e0", 0x00485dc0, &H::load_e0);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

i32 rec_read(void* stream, void* self, i32 n) {  // 0x005872e0
    return joan::orig::as_file(stream)->file_write(joan::orig::arg::ptr(self), joan::orig::arg::u32(n)) == 0 ? 0 : 1;
}
i32 rec_write(void* stream, void* self, i32 n) {  // 0x005873a0
    return joan::orig::as_file(stream)->file_read(joan::orig::arg::mptr(self), joan::orig::arg::u32(n)) == 0 ? 0 : 1;
}

struct H {
    // ------------------------------------------------------------------------ records
    // The names are swapped: read_* write the record, write_* read it. [Debug] SaveFieldwise=1 (default,
    // re/a6/SAVES_PLAN.md S1): save::put_ / get_UnitStats (Unit +0x16f) and ItemStack (HeldItem); 0 = the raw path.
    i32 read_3b(void* stream) {  // 0x0047e5c0
        if (!joan::save::fieldwise()) return rec_read(stream, this, 0x3b);
        joan::save::Out o(*joan::orig::as_file(stream), joan::save::kUnitStats, this);
        joan::save::put_UnitStats(o, *jo::self<jo::UnitStats>(this));
        return o.end() == 0 ? 0 : 1;  // File::file_write 0x005872e0
    }
    i32 write_3b(void* stream) {  // 0x0047e600
        if (!joan::save::fieldwise()) return rec_write(stream, this, 0x3b);
        joan::save::In in(*joan::orig::as_file(stream), joan::save::kUnitStats, this);  // File::file_read 0x005873a0
        joan::save::get_UnitStats(in, *jo::self<jo::UnitStats>(this));
        return in.end() == 0 ? 0 : 1;
    }
    i32 read_12(void* stream) {  // 0x0047ed60
        if (!joan::save::fieldwise()) return rec_read(stream, this, 0x12);
        joan::save::Out o(*joan::orig::as_file(stream), joan::save::kItemStack, this);
        joan::save::put_ItemStack(o, *jo::self<jo::HeldItem>(this));
        return o.end() == 0 ? 0 : 1;  // File::file_write 0x005872e0
    }
    i32 write_12(void* stream) {  // 0x0047eda0
        if (!joan::save::fieldwise()) return rec_write(stream, this, 0x12);
        joan::save::In in(*joan::orig::as_file(stream), joan::save::kItemStack, this);  // File::file_read 0x005873a0
        joan::save::get_ItemStack(in, *jo::self<jo::HeldItem>(this));
        return in.end() == 0 ? 0 : 1;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Rec3b::read", 0x0047e5c0, &H::read_3b);
B93("Rec3b::write", 0x0047e600, &H::write_3b);
B93("Rec12::read", 0x0047ed60, &H::read_12);
B93("Rec12::write", 0x0047eda0, &H::write_12);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b101_02.cpp (game/b101_02)
//
//      0x0047e850 Obj::save_47e850(writer)          (thiscall, ret 4) - 0/1.
//
//      0x0047fb30 List::load_4b3db0(reader)         (thiscall, ret 4) - new(0x1545) items.
//
//      0x0047ff50 List::load_422b50(reader)         (thiscall, ret 4) - new(0x1f6) items.
//
//      0x00480690 Obj::save_480690(writer)          (thiscall, ret 4) - vtable 0x9d0840 slot 3.
//
//  - 0x0047fb30 / 0x0047ff50 / 0x00468790 / 0x004699f0: SEH frames only guard new + ctor;
//    normal path reproduced. Objects leaked on load failure, as in the original.
namespace {
namespace game_b101_02 {

JOAN_COUNTER(c_save_47e850, "Obj::save_47e850");
JOAN_COUNTER(c_load_4b3db0, "List::load_4b3db0");
JOAN_COUNTER(c_load_422b50, "List::load_422b50");
JOAN_COUNTER(c_save_480690, "Obj::save_480690");

i32 list_size(void* l) { return joan::orig::as_dynarray(l)->size(); }  // 0x00417ad0
u8* list_at(void* l, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(l)->get(joan::orig::arg::i32(i))); }  // 0x00417200
// Writer 0x005872e0(p, n).
i32 write_bytes(void* wr, const void* p, i32 n) { return joan::orig::as_file(wr)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }

struct H {
    u8* self() { return bp(this); }

    // ------------------------------------------------------------------ 0x0047e850
    i32 save_47e850(void* wr) {
        c_save_47e850.hit();
        {  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): save::put_UnitData; 0 = the raw write
            i32 ok;
            if (joan::save::fieldwise()) {
                joan::save::Out o(*joan::orig::as_file(wr), joan::save::kUnitData, this);
                joan::save::put_UnitData(o, *jo::self<jo::UnitData>(this));
                ok = o.end();  // File::file_write 0x005872e0
            } else {
                ok = write_bytes(wr, this, 0x1e);
            }
            if (ok == 0) return 0;
        }
        for (i32 i = 0; i < 1; ++i) {
            for (i32 j = 0; j < 2; ++j) {
                void* const p = jo::self<jo::UnitData>(this)->hands[i * 2 + j];  // +2 + i * 8 + j * 4
                if (p != nullptr && joan::orig::arg::as<i32>(joan::orig::as_item_stack(p)->save(joan::orig::arg::mptr(wr))) == 0) return 0;  // 0x0047ed60
            }
        }
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->equipped) != nullptr && joan::orig::arg::as<i32>(joan::orig::as_item_stack(jo::view<void*>(jo::self<jo::UnitData>(this)->equipped))->save(joan::orig::arg::mptr(wr))) == 0) return 0;
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item) != nullptr && joan::orig::arg::as<i32>(joan::orig::as_item_stack(jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item))->save(joan::orig::arg::mptr(wr))) == 0) return 0;
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->list_12) != nullptr && joan::orig::arg::as<i32>(joan::orig::as_grid(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12))->save(joan::orig::arg::mptr(wr))) == 0) return 0;  // 0x0047ede0
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16) != nullptr && joan::orig::arg::as<i32>(joan::orig::as_grid(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16))->save(joan::orig::arg::mptr(wr))) == 0) return 0;
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a) != nullptr && joan::orig::arg::as<i32>(joan::orig::as_grid(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a))->save(joan::orig::arg::mptr(wr))) == 0) return 0;
        return 1;
    }

    // ------------------------------------------------------------------ 0x0047fb30
    i32 load_4b3db0(void* rd) {
        c_load_4b3db0.hit();
        joan::orig::as_list(this)->clear_4b6530();                           // 0x004b6530
        jo::self<jo::ListB>(this)->next_id = joan::orig::as_file(rd)->file_get_long();          // 0x00587e20
        const i32 n = joan::orig::as_file(rd)->file_get_long();           // 0x00587e20
        for (i32 i = 1; i <= n; ++i) {
            const i16 k = joan::orig::as_file(rd)->file_get_short();                     // 0x00587c20
            if (static_cast<i32>(k) == 0) {
                joan::orig::dynarray(this)->add_blank(1);  // 0x00584bd0
                continue;
            }
            void* const mem = game_new(static_cast<u32>(sizeof(jo::Nation)));  // 0x1545 on x86
            void* p = mem != nullptr ? joan::orig::as_nation(mem)->construct() : nullptr;  // 0x004b3db0
            if (joan::orig::arg::as<i32>(joan::orig::as_nation(p)->load(joan::orig::arg::mptr(rd))) == 0) return 0;                  // 0x0047f800
            joan::orig::dynarray_b(this)->linkin(static_cast<void*>(&p));                  // 0x005857a0
        }
        joan::orig::dynarray_b(this)->read_empty_room(joan::orig::arg::file(rd));  // 0x00585bb0
        return 1;
    }

    // ------------------------------------------------------------------ 0x0047ff50
    i32 load_422b50(void* rd) {
        c_load_422b50.hit();
        joan::orig::as_list(obj(d::g_place_array))->delete_all_4c5260();         // 0x004c5260
        const i32 n = joan::orig::as_file(rd)->file_get_long();    // 0x00587e20
        for (i32 i = 1; i <= n; ++i) {
            const i16 k = joan::orig::as_file(rd)->file_get_short();                     // 0x00587c20
            if (static_cast<i32>(k) == 0) {
                joan::orig::dynarray(this)->add_blank(1);  // 0x00584bd0
                continue;
            }
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Place)));  // 0x1f6 on x86
            void* p = mem != nullptr ? joan::orig::as_place(mem)->construct() : nullptr;  // 0x00422b50
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(p)->load(joan::orig::arg::mptr(rd))) == 0) return 0;                  // 0x0047fd00
            joan::orig::dynarray_b(this)->linkin(static_cast<void*>(&p));                  // 0x005857a0
        }
        joan::orig::dynarray_b(this)->read_empty_room(joan::orig::arg::file(rd));  // 0x00585bb0
        return 1;
    }

    // ------------------------------------------------------------------ 0x00480690
    i32 save_480690(void* wr) {
        c_save_480690.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_facility(this)->save_base(joan::orig::arg::mptr(wr))) == 0) return 0;  // 0x004800a0
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::put_FlagFacilityTail / put_FlagRow; 0 = raw.
        const bool fieldwise = joan::save::fieldwise();
        i32 ok;
        if (fieldwise) {
            joan::save::Out o(*joan::orig::as_file(wr), joan::save::kFlagFacilityTail, &jo::self<jo::FlagFacility>(this)->list_79);
            joan::save::put_FlagFacilityTail(o, *jo::self<jo::FlagFacility>(this));
            ok = o.end();  // File::file_write 0x005872e0
        } else {
            ok = write_bytes(wr, &jo::self<jo::FlagFacility>(this)->list_79, 0x20);
        }
        if (ok == 0) return 0;
        jo::DynArray* const* const offs[3] = {&jo::self<jo::FlagFacility>(this)->list_79, &jo::self<jo::FlagFacility>(this)->list_7d, &jo::self<jo::FlagFacility>(this)->list_89};
        for (i32 k = 0; k < 3; ++k) {
            void* const l = *offs[k];
            if (l == nullptr) continue;
            joan::orig::as_file(wr)->file_put_long(joan::orig::arg::i32(list_size(*offs[k])));  // 0x00587da0
            for (i32 i = 1; i <= list_size(*offs[k]); ++i) {
                u8* const e = list_at(*offs[k], i);
                if (fieldwise) {
                    joan::save::Out o(*joan::orig::as_file(wr), joan::save::kFlagRow, e);
                    joan::save::put_FlagRow(o, *reinterpret_cast<const jo::RecruitRow*>(e));
                    ok = o.end();  // File::file_write 0x005872e0
                } else {
                    ok = write_bytes(wr, e, 0xa);
                }
                if (ok == 0) return 0;
            }
        }
        return 1;
    }
};

#define B101(name, addr, fn) JOAN_REPLACE("game/b101_02", name, addr, fn)
B101("Obj::save_47e850", 0x0047e850, &H::save_47e850);
B101("List::load_4b3db0", 0x0047fb30, &H::load_4b3db0);
B101("List::load_422b50", 0x0047ff50, &H::load_422b50);
B101("Obj::save_480690", 0x00480690, &H::save_480690);
#undef B101

}  // namespace game_b101_02
}  // namespace

// ---------------------------------------- from modules/game_b100_03.cpp (game/b100_03)
//
//      0x0047ede0 Grid::save                 (thiscall(file), ret 4)
//
//      0x0047f130 List21::load               (thiscall(file), ret 4)
//
//      0x0047f800 Campaign::load             (thiscall(file), ret 4)
//
//      0x0047fa50 ListB::save                (thiscall(file), ret 4)
//
//      0x0047fd00 Unit::load                 (thiscall(file), ret 4)
//
//      0x004810b0 Obj40::save                (thiscall(file), ret 4)
//
//      0x00481220 X::clear_3a_4e             (thiscall, ret 0)
//
//      0x00482e00 Rec6c::load                (thiscall(file), ret 4)
//
//      0x004830f0 Rec52::load                (thiscall(file), ret 4)
//
//      0x00483b90 Rec1b2::save               (thiscall(file), ret 4)
//
//      0x00484ad0 Recd5::save                (thiscall(file), ret 4)
//
//      0x00484e00 Recac::save                (thiscall(file), ret 4)
//
//      0x00485130 Rec92::load                (thiscall(file), ret 4) - SEH frame (normal path only)
//
//      0x00485990 X::reset_state             (thiscall, ret 0)
//
//      0x00485e70 ListC::save                (thiscall(file), ret 4)
//
//      0x004860d0 Rec43::load                (thiscall(file), ret 4)
//
//  - 0x004810b0 pushes edx whose low word is the value and whose high word is the high word of
//    `this` (0x00587ba0 writes only 2 bytes); replicated anyway.
//
//  - 0x00483b90: second string block writes from +0x19e with the length of +0x1a2 (kept).
//
//  - 0x00485130 copies 6 dwords of the stack record into the new heap one (rep movsd), kept.
namespace {
namespace game_b100_03 {

struct RV3 {  // 12-byte float vector passed by value
    float x, y, z;
};
static_assert(sizeof(RV3) == 12, "RV3");

struct alignas(4) LocalRec {  // 0x005e9a00 object (0xd5 bytes), frame slot ebp-0xe8 (0xd8 bytes)
    u8 bytes[0xd8];
};

JOAN_COUNTER(c_grid_save, "Grid::save");
JOAN_COUNTER(c_list21_load, "List21::load");
JOAN_COUNTER(c_campaign_load, "Campaign::load");
JOAN_COUNTER(c_listb_save, "ListB::save");
JOAN_COUNTER(c_unit_load, "Unit::load");
JOAN_COUNTER(c_obj40_save, "Obj40::save");
JOAN_COUNTER(c_rec1b2_save, "Rec1b2::save");
JOAN_COUNTER(c_rec92_load, "Rec92::load");
JOAN_COUNTER(c_reset_state, "X::reset_state");
JOAN_COUNTER(c_listc_save, "ListC::save");

using joan::orig::helpers::strlen_;                            // strlen

// file helpers (thiscall on the file object)
i32 fwrite_(void* file, const void* p, i32 n) { return joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }  // 0x005872e0
i32 fread_(void* file, void* p, i32 n) { return joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n)); }  // 0x005873a0
i32 fwrite_word(void* file, u32 v) { return joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(v)); }  // 0x00587ba0
i32 fwrite_dword(void* file, i32 v) { return joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(v)); }  // 0x00587da0
u16 fread_word(void* file) { return joan::orig::arg::as<u16>(joan::orig::as_file(file)->file_get_short()); }  // 0x00587c20
i32 fread_dword(void* file) { return joan::orig::as_file(file)->file_get_long(); }                           // 0x00587e20

void* alloc_(u32 n, img::Addr file, i32 line) {
    return joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(n), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058e7b0
}

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------------ 0x0047ede0
    i32 grid_save(void* file) {
        c_grid_save.hit();
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): the cell rows as schema-less byte chunks, the
        // elements with save::put_InvItem; 0 = the raw writes.
        const bool fieldwise = joan::save::fieldwise();
        for (i32 i = 0; i < jo::self<jo::Grid>(this)->rows; ++i) {
            for (i32 j = 0; j < jo::self<jo::Grid>(this)->cols; ++j) {
                void* const cell = jo::view<void* const* const*>(jo::self<jo::Grid>(this)->cells)[i][j];
                if (fieldwise) {  // a row of occupancy bytes (Grid::fill 0x00458090): no schema, verbatim
                    joan::save::Out o(joan::save::port(*joan::orig::as_file(file)), static_cast<std::uint32_t>(jo::self<jo::Grid>(this)->cell_size), "GridCell", cell);
                    o.bytes(cell, static_cast<std::uint32_t>(jo::self<jo::Grid>(this)->cell_size));
                    if (o.end() == 0) return 0;  // File::file_write 0x005872e0
                } else if (fwrite_(file, cell, jo::self<jo::Grid>(this)->cell_size) == 0) {
                    return 0;
                }
            }
        }
        fwrite_dword(file, joan::orig::as_dynarray(this)->size());  // 0x00417ad0
        for (i32 i = 1; i <= joan::orig::as_dynarray(this)->size(); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(this)->is_empty_at(joan::orig::arg::i32(i))) != 0) {  // 0x00457f60
                fwrite_word(file, 0);
            } else {
                fwrite_word(file, 1);
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_ptr_list(this)->get(joan::orig::arg::i32(i)));  // 0x00457e90
                if (fieldwise) {
                    joan::save::Out o(*joan::orig::as_file(file), joan::save::kInvItem, e);
                    joan::save::put_InvItem(o, *static_cast<const jo::InvItem*>(e));
                    if (o.end() == 0) return 0;  // File::file_write 0x005872e0
                } else if (fwrite_(file, e, 0x18) == 0) {
                    return 0;
                }
            }
        }
        joan::orig::dynarray_b(this)->write_empty_room(joan::orig::arg::file(file));  // 0x00585b40
        return 1;
    }

    // ------------------------------------------------------------------------ 0x0047f130
    i32 list21_load(void* file) {
        c_list21_load.hit();
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): save::get_OrderEntry; 0 = the raw reads.
        const bool fieldwise = joan::save::fieldwise();
        const auto get = [&](void* p) -> i32 {
            if (!fieldwise) return fread_(file, p, 0x21);
            joan::save::In in(*joan::orig::as_file(file), joan::save::kOrderEntry, p);  // File::file_read 0x005873a0
            joan::save::get_OrderEntry(in, *static_cast<jo::OrderEntry*>(p));
            return in.end();
        };
        if (get(&jo::self<jo::Order>(this)->current) == 0) return 0;  // +0x1d
        const i32 n = fread_dword(file);
        for (i32 i = 1; i <= n; ++i) {
            joan::orig::dynarray(this)->linkin(nullptr);                          // 0x00584620
            const i32 a = joan::orig::as_dynarray(this)->cursor();  // 0x00417520
            void* const e = joan::orig::as_list(this)->at_484570(a);          // 0x00484570
            if (get(e) == 0) return 0;
        }
        return 1;
    }

    // ------------------------------------------------------------------------ 0x0047f800
    void load_words(void* file, jo::ArrI16*& words, const i16& size, const i16& count, i32 line_add) {
        if (words == nullptr) return;
        const i32 line = line_of(d::g_9ffcb8, line_add);
        jo::view<void*>(words) = alloc_(static_cast<u32>(static_cast<i32>(size) << 1), d::s_d_ww_gfile3_cpp, line);
        for (i32 i = 0; i < static_cast<i32>(count); ++i) {
            const u16 w = fread_word(file);
            jo::view<u16>(words[i].value) = w;
        }
    }
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S4): save::get_Nation; 0 = the raw read.
    i32 campaign_load(void* file) {
        c_campaign_load.hit();
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kNation, this);  // File::file_read 0x005873a0
            joan::save::get_Nation(in, *jo::self<jo::Nation>(this));
            if (in.end() == 0) return 0;
        } else if (fread_(file, this, 0x1545) == 0) {
            return 0;
        }
        for (i32 i = 0; i < 0x14; ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_sub(&jo::self<jo::Nation>(this)->relation1 + i)->load_f4(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047f620
        }
        load_words(file, jo::self<jo::Nation>(this)->leaders, jo::view<i16>(jo::self<jo::Nation>(this)->leader_cap), jo::view<i16>(jo::self<jo::Nation>(this)->leader_count), 0x12);  // "d:\ww\gfile3.cpp"
        load_words(file, jo::self<jo::Nation>(this)->idle_units, jo::view<i16>(jo::self<jo::Nation>(this)->idle_cap), jo::view<i16>(jo::self<jo::Nation>(this)->idle_count), 0x1b);
        load_words(file, jo::self<jo::Nation>(this)->stations, jo::view<i16>(jo::self<jo::Nation>(this)->station_cap), jo::self<jo::Nation>(this)->station_count, 0x24);
        return 1;
    }

    // ------------------------------------------------------------------------ 0x0047fa50
    i32 listb_save(void* file) {
        c_listb_save.hit();
        fwrite_dword(file, jo::self<jo::ListB>(this)->next_id);
        fwrite_dword(file, joan::orig::as_dynarray(this)->size());  // 0x00417ad0
        for (i32 i = 1; i <= joan::orig::as_dynarray(this)->size(); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null(joan::orig::arg::i32(i))) != 0) {  // 0x00484870
                fwrite_word(file, 0);
            } else {
                fwrite_word(file, 1);
                void* const e = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0047b600
                if (joan::orig::arg::as<i32>(joan::orig::as_sub(e)->save_1545(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047f660
            }
        }
        joan::orig::dynarray_b(this)->write_empty_room(joan::orig::arg::file(file));  // 0x00585b40
        return 1;
    }

    // ------------------------------------------------------------------------ 0x0047fd00
    i32 unit_load(void* file) {
        c_unit_load.hit();
        i32 ok;  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::get_Place; 0 = the raw read
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kPlace, this);  // File::file_read 0x005873a0
            joan::save::get_Place(in, *jo::self<jo::Place>(this));
            ok = in.end();
        } else {
            ok = fread_(file, this, 0x1f6);
        }
        if (ok == 0) return 0;
        joan::orig::as_place(this)->clear_pointers();  // 0x0047fdf0
        if (jo::self<jo::Place>(this)->world != nullptr) {
            const i32 id = static_cast<i32>(static_cast<i16>(fread_word(file)));
            jo::view<void*>(jo::self<jo::Place>(this)->world) = joan::orig::arg::as<void*>(joan::orig::as_units(obj(d::g_a45f08))->get_420240(joan::orig::arg::i32(id)));  // 0x00420240
            joan::orig::as_place(this)->set_owner(jo::self<jo::Place>(this)->world);                    // 0x004366f0
            RV3 v;
            std::memcpy(&v, jo::self<jo::Place>(this)->position_, sizeof(v));  // +0x2a
            joan::orig::as_place(this)->set_pose(v, joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::Place>(this)->box)));  // +0x3a  // 0x0042b350
        }
        joan::orig::as_place(this)->set_nation(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Place>(this)->nation)));  // 0x0044f5c0
        return 1;
    }

    // ------------------------------------------------------------------------ 0x004810b0
    i32 obj40_save(void* file) {
        c_obj40_save.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_item_stack(this)->save(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047ed60
        i32 ok;  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::put_WorldItem; 0 = the raw write
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kWorldItem, jo::self<jo::WorldItem>(this)->pos);
            joan::save::put_WorldItem(o, *jo::self<jo::WorldItem>(this));
            ok = o.end();  // File::file_write 0x005872e0
        } else {
            ok = fwrite_(file, jo::self<jo::WorldItem>(this)->pos, 0x40);
        }
        if (ok == 0) return 0;
        if (jo::self<jo::WorldItem>(this)->world != nullptr) {
            const u32 edx = (joan::call::register_low_bits(this) & 0xffff0000u) |
                            static_cast<u32>(jo::view<u16>(jo::self<jo::WorldItem>(this)->world->world_id));
            if (fwrite_word(file, edx) == 0) return 0;
        }
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00481220
    void clear_3a_4e() {
        jo::self<jo::WorldItem>(this)->cell = nullptr;
        jo::self<jo::WorldItem>(this)->cell_prev = nullptr;
        jo::self<jo::WorldItem>(this)->cell_next = nullptr;
        jo::self<jo::WorldItem>(this)->sector = nullptr;
        jo::self<jo::WorldItem>(this)->sector_prev = nullptr;
        jo::self<jo::WorldItem>(this)->sector_next = nullptr;
    }

    // ------------------------------------------------------------------------ 0x00482e00
    i32 rec6c_load(void* file) {
        i32 ok;  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::get_Order; 0 = the raw read
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kOrder, this);  // File::file_read 0x005873a0
            joan::save::get_Order(in, *jo::self<jo::Missile>(this));
            ok = in.end();
        } else {
            ok = fread_(file, this, 0x6c);
        }
        if (ok == 0) return 0;
        joan::orig::as_order(this)->clear_pointers();  // 0x00482e50
        return 1;
    }

    // ------------------------------------------------------------------------ plain read / write
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S4): save::get_NewsItem / get_Trigger; 0 = the raw reads.
    i32 rec52_load(void* file) {  // 0x004830f0
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kNewsItem, this);  // File::file_read 0x005873a0
            joan::save::get_NewsItem(in, *jo::self<jo::NewsItem>(this));
            return in.end() == 0 ? 0 : 1;
        }
        return fread_(file, this, 0x52) == 0 ? 0 : 1;
    }
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S3): save::put_Scene / save::put_Cell; 0 = the raw writes.
    i32 recd5_save(void* file) {  // 0x00484ad0
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kScene, this);
            joan::save::put_Scene(o, *jo::self<jo::Scene>(this));
            return o.end() == 0 ? 0 : 1;  // File::file_write 0x005872e0
        }
        return fwrite_(file, this, 0xd5) == 0 ? 0 : 1;
    }
    i32 recac_save(void* file) {  // 0x00484e00
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kCell, this);
            joan::save::put_Cell(o, *jo::self<jo::Cell>(this));
            return o.end() == 0 ? 0 : 1;  // File::file_write 0x005872e0
        }
        return fwrite_(file, this, 0xac) == 0 ? 0 : 1;
    }
    i32 rec43_load(void* file) {  // 0x004860d0
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kTrigger, this);  // File::file_read 0x005873a0
            joan::save::get_Trigger(in, *jo::self<jo::Trigger>(this));
            return in.end() == 0 ? 0 : 1;
        }
        return fread_(file, this, 0x43) == 0 ? 0 : 1;
    }

    // ------------------------------------------------------------------------ 0x00483b90
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S4): save::put_ScenarioEntry; 0 = the raw write.
    i32 rec1b2_save(void* file) {
        c_rec1b2_save.hit();
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kScenarioEntry, this);
            joan::save::put_ScenarioEntry(o, *jo::self<jo::ScenarioEntry>(this));
            if (o.end() == 0) return 0;  // File::file_write 0x005872e0
        } else if (fwrite_(file, this, 0x1b2) == 0) {
            return 0;
        }
        if (jo::self<jo::ScenarioEntry>(this)->name != nullptr) {
            fwrite_word(file, strlen_(jo::view<const char*>(jo::self<jo::ScenarioEntry>(this)->name)) + 1);
            const u32 n = strlen_(jo::view<const char*>(jo::self<jo::ScenarioEntry>(this)->name)) + 1;
            fwrite_(file, jo::view<void*>(jo::self<jo::ScenarioEntry>(this)->name), static_cast<i32>(n));
        }
        if (jo::self<jo::ScenarioEntry>(this)->name2 != nullptr) {
            fwrite_word(file, strlen_(jo::view<const char*>(jo::self<jo::ScenarioEntry>(this)->name2)) + 1);
            const u32 n = strlen_(jo::view<const char*>(jo::self<jo::ScenarioEntry>(this)->name2)) + 1;
            // sic: the original writes from the pointer of +0x19e (name) with the length of +0x1a2 (name2). When name2 is
            // the longer one it read past the end of name's heap block (uninitialised heap bytes into the file; a fault
            // with [Debug] HeapGuard): those bytes are written as zeros here, name's own bytes as before.
            const char* const first = jo::view<const char*>(jo::self<jo::ScenarioEntry>(this)->name);
            const u32 have = first != nullptr ? strlen_(first) + 1 : 0;
            if (n <= have) {
                fwrite_(file, jo::view<void*>(jo::self<jo::ScenarioEntry>(this)->name), static_cast<i32>(n));
            } else {
                std::vector<char> bytes(n, 0);
                if (have != 0) std::memcpy(bytes.data(), first, have);
                fwrite_(file, bytes.data(), static_cast<i32>(n));
            }
        }
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00485130
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S3): save::get_World; 0 = the raw read. Members by name
    // (the x86 offsets of the original: scene +0x6e, cell_count +0x72, cell_capacity +0x76, cells +0x7a, cell_query
    // +0x82; Cell::loaded +0x43).
    i32 rec92_load(void* file) {
        c_rec92_load.hit();
        jo::World& w = *jo::self<jo::World>(this);
        i32 ok;
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kWorld, this);  // File::file_read 0x005873a0
            joan::save::get_World(in, w);
            ok = in.end();
        } else {
            ok = fread_(file, this, 0x92);
        }
        if (ok == 0) return 0;
        joan::orig::as_world(this)->clear_pointers();  // 0x00485450
        if (w.scene != nullptr) {
            // the 0x005e9a00 object (0xd5 bytes) in the frame slot ebp-0xe8 (0xd8 bytes); room for the x64 Scene
            alignas(8) u8 local[sizeof(jo::Scene) > sizeof(LocalRec) ? sizeof(jo::Scene) : sizeof(LocalRec)];
            joan::orig::as_scene(local)->construct();  // 0x005e9a00 ctor
            if (joan::orig::arg::as<i32>(joan::orig::as_sub(local)->load_d5(joan::orig::arg::mptr(file))) == 0) {  // 0x00484b10
                joan::orig::as_scene(local)->dtor();  // 0x005e9a90 dtor
                return 0;
            }
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Scene)));  // operator new (0xd5 on x86)
            void* const p = mem != nullptr ? joan::orig::as_scene(mem)->construct() : nullptr;  // 0x005e9a00
            jo::view<void*>(w.scene) = p;
            std::memcpy(w.scene->params, jo::as_scene(local)->params, sizeof w.scene->params);  // rep movsd x6 (+0..0x18)
            joan::orig::as_scene(w.scene)->set_mode(1u);  // 0x005ef730
            joan::orig::as_scene(local)->dtor();         // 0x005e9a90 dtor
        }
        // two Cell* arrays (cap << 2 on x86)
        jo::view<void*>(w.cells) = alloc_(static_cast<u32>(w.cell_capacity) * static_cast<u32>(sizeof(jo::CellPtrArray)), d::s_d_ww_gfile3_cpp, line_of(d::g_9ffcd8, 0x18));
        w.cell_query = alloc_(static_cast<u32>(w.cell_capacity) * static_cast<u32>(sizeof(void*)), d::s_d_ww_gfile3_cpp, line_of(d::g_9ffcd8, 0x19));  // BoxTree::collect writes void*
        const i32 n = w.cell_count;
        w.cell_count = 0;
        for (i32 i = 0; i < n; ++i) {
            const i32 id = joan::orig::arg::as<i32>(joan::orig::as_map(this)->add_cells(1));        // 0x00547bf0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_map(this)->cell(joan::orig::arg::i32(id)));        // 0x00548600
            if (joan::orig::arg::as<i32>(joan::orig::as_sub(e)->load_ac(joan::orig::arg::mptr(file))) == 0) return 0;    // 0x00484e40
            reinterpret_cast<jo::Cell*>(e)->loaded = 0;
        }
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00485990
    void reset_state() {
        c_reset_state.hit();
        jo::self<jo::Player>(this)->button_down[0] = 0;
        jo::self<jo::Player>(this)->button_down[1] = 0;
        jo::self<jo::Player>(this)->button_down[2] = 0;
        jo::self<jo::Player>(this)->button_down[3] = 0;
        jo::self<jo::Player>(this)->button_up[0] = 1;
        jo::self<jo::Player>(this)->button_up[1] = 1;
        jo::self<jo::Player>(this)->button_up[2] = 1;
        jo::self<jo::Player>(this)->button_up[3] = 1;
        jo::view<u32>(jo::self<jo::Player>(this)->last_dir_key) = 0;
        jo::self<jo::Player>(this)->last_dir_key_ms = 0;
        jo::view<u16>(jo::self<jo::Player>(this)->combo_step) = 0;
        jo::view<u32>(jo::self<jo::Player>(this)->prev_move_angle) = 0;
        jo::self<jo::Player>(this)->cam_zones = nullptr;
        jo::self<jo::Player>(this)->rtsicon_res = nullptr;
        jo::self<jo::Player>(this)->rts_icon = nullptr;
        if (jo::view<u32>(jo::self<jo::Player>(this)->mode) != 4 && jo::view<u32>(jo::self<jo::Player>(this)->mode) != 2) jo::self<jo::Player>(this)->riding = 1;
        jo::self<jo::Player>(this)->u8_7c = 0;
    }

    // ------------------------------------------------------------------------ 0x00485e70
    i32 listc_save(void* file) {
        c_listc_save.hit();
        if (joan::save::fieldwise()) {  // [Debug] SaveFieldwise (SAVES_PLAN S4): the byte +0x1d as a save::Out chunk
            joan::save::Out o(joan::save::port(*joan::orig::as_file(file)), joan::save::kTriggersByte1d, &jo::self<jo::Triggers>(this)->u8_1d);
            o.u8(jo::self<jo::Triggers>(this)->u8_1d);
            if (o.end() == 0) return 0;  // File::file_write 0x005872e0
        } else if (fwrite_(file, &jo::self<jo::Triggers>(this)->u8_1d, 1) == 0) {
            return 0;
        }
        fwrite_dword(file, joan::orig::as_dynarray(this)->size());  // 0x00417ad0
        for (i32 i = 1; i <= joan::orig::as_dynarray(this)->size(); ++i) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_485f20(joan::orig::arg::u32(i)));   // 0x00485f20
            if (joan::orig::arg::as<i32>(joan::orig::as_sub(e)->save_43(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x00486090
        }
        return 1;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_03", name, addr, fn)
B100("Grid::save", 0x0047ede0, &H::grid_save);
B100("List21::load", 0x0047f130, &H::list21_load);
B100("Campaign::load", 0x0047f800, &H::campaign_load);
B100("ListB::save", 0x0047fa50, &H::listb_save);
B100("Unit::load", 0x0047fd00, &H::unit_load);
B100("Obj40::save", 0x004810b0, &H::obj40_save);
B100("X::clear_3a_4e", 0x00481220, &H::clear_3a_4e);
B100("Rec6c::load", 0x00482e00, &H::rec6c_load);
B100("Rec52::load", 0x004830f0, &H::rec52_load);
B100("Rec1b2::save", 0x00483b90, &H::rec1b2_save);
B100("Recd5::save", 0x00484ad0, &H::recd5_save);
B100("Recac::save", 0x00484e00, &H::recac_save);
B100("Rec92::load", 0x00485130, &H::rec92_load);
B100("X::reset_state", 0x00485990, &H::reset_state);
B100("ListC::save", 0x00485e70, &H::listc_save);
B100("Rec43::load", 0x004860d0, &H::rec43_load);
#undef B100

}  // namespace game_b100_03
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
//
//      0x0047ef30 Grid::load(file)             (thiscall, ret 4)
//
//      0x0047fc80 ObjC::save(file)             (thiscall, ret 4)
//
//      0x00481ed0 ObjD::reset                  (thiscall, ret 0)
//
//      0x004830b0 ObjE::save(file)             (thiscall, ret 4)
//
//      0x004842c0 ObjF::load(file)             (thiscall, ret 4)
//
//      0x00484d50 ObjG::reset                  (thiscall, ret 0)
//
//      0x00485ab0 ObjH::save(file)             (thiscall, ret 4)
//
//      0x004864b0 ObjI::load(file)             (thiscall, ret 4)
//
//  - 0x0047ef30: MSVC interleaves the size push (0x18) of 0x005873a0 before the calls of
//    0x00417520 (no args) and 0x00457e90 (1 arg); call order kept.
//
//  - 0x0047fc80: the original pushes ecx with cx = word +2 and the upper half of `this`;
namespace {
namespace game_b96_02 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_grid_load, "Grid::load");
JOAN_COUNTER(c_f_load, "ObjF::load");
JOAN_COUNTER(c_h_save, "ObjH::save");
JOAN_COUNTER(c_i_load, "ObjI::load");

void* mem_alloc(i32 n, img::Addr file, i32 line) {
    return joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(n), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058e7b0
}
i32 file_write(void* file, void* p, i32 n) { return joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }  // 0x005872e0
i32 file_read(void* file, void* p, i32 n) { return joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n)); }  // 0x005873a0
i16 file_read_short(void* file) { return joan::orig::arg::as<i16>(joan::orig::as_file(file)->file_get_short()); }  // 0x00587c20

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------------ 0x0047ef30
    i32 grid_load(void* file) {
        c_grid_load.hit();
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): the cell rows as schema-less byte chunks, the
        // elements with save::get_InvItem; 0 = the raw reads.
        const bool fieldwise = joan::save::fieldwise();
        for (i32 i = 0; i < jo::self<jo::Grid>(this)->rows; ++i) {
            for (i32 j = 0; j < jo::self<jo::Grid>(this)->cols; ++j) {
                void* const cell = jo::view<void* const* const*>(jo::self<jo::Grid>(this)->cells)[i][j];
                const auto n = static_cast<std::uint32_t>(jo::self<jo::Grid>(this)->cell_size);
                if (fieldwise) {
                    joan::save::In in(joan::save::port(*joan::orig::as_file(file)), n, "GridCell", cell);  // File::file_read 0x005873a0
                    in.bytes(cell, n);
                    if (in.end() == 0) return 0;
                } else if (file_read(file, cell, jo::self<jo::Grid>(this)->cell_size) == 0) {
                    return 0;
                }
            }
        }
        const i32 n = joan::orig::as_file(file)->file_get_long();   // 0x00587e20
        for (i32 k = 1; k <= n; ++k) {
            const i32 v = file_read_short(file);
            if (v == 0) {
                joan::orig::dynarray(this)->add_blank(1);  // 0x00584bd0
            } else {
                joan::orig::dynarray_b(this)->linkin(nullptr);                    // 0x005857a0
                const u32 a = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
                void* const b = joan::orig::arg::as<void*>(joan::orig::as_ptr_list(this)->get(joan::orig::arg::i32(a)));      // 0x00457e90
                if (fieldwise) {
                    joan::save::In in(*joan::orig::as_file(file), joan::save::kInvItem, b);  // File::file_read 0x005873a0
                    joan::save::get_InvItem(in, *static_cast<jo::InvItem*>(b));
                    if (in.end() == 0) return 0;
                } else if (file_read(file, b, 0x18) == 0) {
                    return 0;
                }
            }
        }
        joan::orig::dynarray_b(this)->read_empty_room(joan::orig::arg::file(file));  // 0x00585bb0
        return 1;
    }

    // ------------------------------------------------------------------------ 0x0047fc80
    // Saved pointers (S7 save audit): the 0x1f6-byte place is written raw. Its only pointer, +0x1de (owner unit), is
    // re-derived from the id word below by Unit::load 0x0047fd00 (game_b100_03.cpp); the other dwords of
    // +0x1d6..+0x1f5 are zeroed there first (0x0047fdf0). The "image address" SaveAudit reported at +0x4 is text: byte +4 and the first characters
    // of the place name at +5 (Places::find compares +5). [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2):
    // save::put_Place (world as a null flag, the rest of the pointers cleared by the reader); 0 = the raw write.
    i32 save_c(void* file) {
        i32 ok;
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kPlace, this);
            joan::save::put_Place(o, *jo::self<jo::Place>(this));
            ok = o.end();  // File::file_write 0x005872e0
        } else {
            ok = file_write(file, this, 0x1f6);
        }
        if (ok == 0) return 0;
        if (jo::self<jo::Place>(this)->world != nullptr) {
            // only the low word reaches the file (put_short)
            const u32 arg = (joan::call::register_low_bits(this) & 0xffff0000u) | jo::view<u16>(jo::self<jo::Place>(this)->world->world_id);
            if (joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(arg)) == 0) return 0;  // 0x00587ba0
        }
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00481ed0
    void* reset_d() {
        jo::self<jo::MapObject>(this)->actor = nullptr;
        jo::self<jo::MapObject>(this)->u32_8a = 0;
        jo::self<jo::MapObject>(this)->actor_b = nullptr;
        jo::self<jo::MapObject>(this)->active = 0;
        jo::self<jo::MapObject>(this)->refresh_depth = 0;
        jo::self<jo::MapObject>(this)->u16_84 = 0;
        jo::self<jo::MapObject>(this)->cell = nullptr;
        jo::self<jo::MapObject>(this)->cell_prev = nullptr;
        jo::self<jo::MapObject>(this)->cell_next = nullptr;
        jo::self<jo::MapObject>(this)->scene = nullptr;
        jo::self<jo::MapObject>(this)->u32_ba = 0;
        jo::self<jo::MapObject>(this)->u32_be = 0;
        return this;
    }

    // ------------------------------------------------------------------------ 0x004830b0
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S4): save::put_NewsItem; 0 = the raw write.
    i32 save_e(void* file) {
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kNewsItem, this);
            joan::save::put_NewsItem(o, *jo::self<jo::NewsItem>(this));
            return o.end() != 0 ? 1 : 0;  // File::file_write 0x005872e0
        }
        return file_write(file, this, 0x52) != 0 ? 1 : 0;
    }

    // ------------------------------------------------------------------------ 0x004842c0
    i32 load_f(void* file) {
        c_f_load.hit();
        if (joan::save::fieldwise()) {  // [Debug] SaveFieldwise (SAVES_PLAN S4): save::get_EndCon, 0 = the raw read
            joan::save::In in(*joan::orig::as_file(file), joan::save::kEndCon, this);  // File::file_read 0x005873a0
            joan::save::get_EndCon(in, *jo::self<jo::EndCon>(this));
            if (in.end() == 0) return 0;
        } else if (file_read(file, this, 0x1a) == 0) {
            return 0;
        }
        if (jo::self<jo::EndCon>(this)->text != nullptr) {
            const i32 n = file_read_short(file);
            jo::view<void*>(jo::self<jo::EndCon>(this)->text) = mem_alloc(n, d::s_d_ww_gfile3_cpp, line_of(d::g_9ffccc, 0xa));  // "gfile3.cpp"
            file_read(file, jo::view<void*>(jo::self<jo::EndCon>(this)->text), n);
        }
        if (jo::self<jo::EndCon>(this)->text2 != nullptr) {
            const i32 n = file_read_short(file);
            jo::view<void*>(jo::self<jo::EndCon>(this)->text2) = mem_alloc(n, d::s_d_ww_gfile3_cpp, line_of(d::g_9ffccc, 0x10));
            file_read(file, jo::view<void*>(jo::self<jo::EndCon>(this)->text2), n);
        }
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00484d50
    void* reset_g() {
        // this = the sector SceneS (load_484d00 reads the 0xe4-byte record, then calls this): its pointers
        jo::SceneS* const s = jo::self<jo::SceneS>(this);
        s->node = nullptr;
        s->plane = nullptr;
        s->object = nullptr;
        s->plane2 = nullptr;
        s->items = nullptr;
        s->ptr_50 = nullptr;
        s->nodes = nullptr;
        s->nodes_tail = nullptr;
        s->nodes_head = nullptr;
        ++joan::en3d::g_scene_list_edits;  // (SceneS::draw_lists' layer lists)
        joan::en3d::unit_index_touch(this);  // [Debug] UnitIndex
        s->lights = nullptr;
        return this;
    }

    // ------------------------------------------------------------------------ 0x00485ab0
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S3): the LightSys (g_a47e08) chunks field by field
    // (save::put_LightSys*); 0 = the raw writes. Write results ignored, as in the original.
    i32 save_h(void* file) {
        c_h_save.hit();
        if (joan::save::fieldwise()) {
            namespace sv = joan::save;
            const jo::LightSys& ls = *jo::self<jo::LightSys>(this);
            const auto wr = [&](const sv::Record& r, const void* legacy, auto fn) {
                sv::Out o(*joan::orig::as_file(file), r, legacy);
                fn(o);
                o.end();  // File::file_write 0x005872e0
            };
            wr(sv::kLightSysEventAChance, &ls.event_a_chance, [&](sv::Out& c) { sv::put_LightSysEventAChance(c, ls); });
            wr(sv::kLightSysEventARate, &ls.event_a_rate, [&](sv::Out& c) { sv::put_LightSysEventARate(c, ls); });
            wr(sv::kLightSysEventATime, &ls.event_a_time, [&](sv::Out& c) { sv::put_LightSysEventATime(c, ls); });
            wr(sv::kLightSysEventAState, &ls.event_a_state, [&](sv::Out& c) { sv::put_LightSysEventAState(c, ls); });
            wr(sv::kLightSysEventBChance, &ls.event_b_chance, [&](sv::Out& c) { sv::put_LightSysEventBChance(c, ls); });
            wr(sv::kLightSysEventBRate, &ls.event_b_rate, [&](sv::Out& c) { sv::put_LightSysEventBRate(c, ls); });
            wr(sv::kLightSysEventBTime, &ls.event_b_time, [&](sv::Out& c) { sv::put_LightSysEventBTime(c, ls); });
            wr(sv::kLightSysEventBState, &ls.event_b_state, [&](sv::Out& c) { sv::put_LightSysEventBState(c, ls); });
            wr(sv::kLightSysCurrent, &ls.current, [&](sv::Out& c) { sv::put_LightSysCurrent(c, ls); });
            wr(sv::kLightSysStateA, &ls.state_a, [&](sv::Out& c) { sv::put_LightSysStateA(c, ls); });
            wr(sv::kLightSysStateB, &ls.state_b, [&](sv::Out& c) { sv::put_LightSysStateB(c, ls); });
            return 1;
        }
        jo::LightSys& light = *jo::self<jo::LightSys>(this);
        for (i32 k = 0; k < 8; ++k) file_write(file, &light.event_a_chance + k, 4);  // the eight event words, 0x58c..0x5a8 on x86
        file_write(file, &light.current, 0x18c);
        file_write(file, &light.state_a, 0x194);
        file_write(file, &light.state_b, 0x194);
        return 1;
    }

    // ------------------------------------------------------------------------ 0x004864b0
    i32 load_i(void* file) {
        c_i_load.hit();
        if (joan::save::fieldwise()) {  // [Debug] SaveFieldwise (SAVES_PLAN S4): save::get_MsgEntry, 0 = the raw read
            joan::save::In in(*joan::orig::as_file(file), joan::save::kMsgEntry, this);  // File::file_read 0x005873a0
            joan::save::get_MsgEntry(in, *jo::self<jo::MsgEntry>(this));
            if (in.end() == 0) return 0;
        } else if (file_read(file, this, 0x1f) == 0) {
            return 0;
        }
        joan::orig::as_log_message(this)->clear_strings();  // 0x004865c0
        {
            const i32 line = line_of(d::g_9ffcdc, 9);
            jo::view<void*>(jo::self<jo::MsgEntry>(this)->text) = mem_alloc(static_cast<i32>(jo::self<jo::MsgEntry>(this)->text_len) + 1, d::s_d_ww_gfile3_cpp, line);
        }
        file_read(file, jo::self<jo::MsgEntry>(this)->text, static_cast<i32>(jo::self<jo::MsgEntry>(this)->text_len) + 1);
        if (static_cast<i32>(jo::self<jo::MsgEntry>(this)->extra_len) > 0) {
            const i32 line = line_of(d::g_9ffcdc, 0xd);
            jo::view<void*>(jo::self<jo::MsgEntry>(this)->extra) = mem_alloc(static_cast<i32>(jo::self<jo::MsgEntry>(this)->extra_len) + 1, d::s_d_ww_gfile3_cpp, line);
            file_read(file, jo::self<jo::MsgEntry>(this)->extra, static_cast<i32>(jo::self<jo::MsgEntry>(this)->extra_len) + 1);
        }
        return 1;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("Grid::load", 0x0047ef30, &H::grid_load);
B96("ObjC::save", 0x0047fc80, &H::save_c);
B96("ObjD::reset", 0x00481ed0, &H::reset_d);
B96("ObjE::save", 0x004830b0, &H::save_e);
B96("ObjF::load", 0x004842c0, &H::load_f);
B96("ObjG::reset", 0x00484d50, &H::reset_g);
B96("ObjH::save", 0x00485ab0, &H::save_h);
B96("ObjI::load", 0x004864b0, &H::load_i);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
//
//      0x0047f080 List::save_21(file)           (thiscall, ret 4)
//
//      0x0047f660 Sub::save_1545(file)          (thiscall, ret 4)
//
//      0x0047fdf0 Obj::clear_1d6                (thiscall, ret 0) - returns this.
//
//      0x00481fb0 Units::save(file)             (thiscall, ret 4) - virtual save at vtbl +0x70.
//
//      0x00482ea0 List::save_482ea0(file)       (thiscall, ret 4)
//
//      0x00483130 List::save_483130(file)       (thiscall, ret 4)
//
//      0x00484160 Obj::clear_277                (thiscall, ret 0) - returns this.
//
//      0x00484b10 Sub::load_d5(file)            (thiscall, ret 4)
//
//      0x00485450 Obj::clear_6a                 (thiscall, ret 0) - returns this.
//
//      0x00486090 Sub::save_43(file)            (thiscall, ret 4)
//
//  - 0x00481fb0: vtbl +0x70 is Unit::save(file) (0x00481290 .. 0x00481ba0, all ret 4).
namespace {
namespace game_b99_02 {

using joan::orig::helpers::u;

JOAN_COUNTER(c_save_21, "List::save_21");
JOAN_COUNTER(c_save_1545, "Sub::save_1545");
JOAN_COUNTER(c_units_save, "Units::save");
JOAN_COUNTER(c_save_482ea0, "List::save_482ea0");
JOAN_COUNTER(c_save_483130, "List::save_483130");

i32 file_write(void* file, const void* p, i32 n) {
    return joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n));  // 0x005872e0
}
i32 file_read(void* file, void* p, i32 n) {
    return joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n));  // 0x005873a0
}
i32 file_word(void* file, i32 w) { return joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(w)); }  // 0x00587ba0 (low word)
i32 file_dword(void* file, i32 v) { return joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(v)); }  // 0x00587da0

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------ 0x0047f080
    i32 save_21(void* file) {
        c_save_21.hit();
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): save::put_OrderEntry; 0 = the raw writes.
        const bool fieldwise = joan::save::fieldwise();
        const auto put = [&](const void* p) -> i32 {
            if (!fieldwise) return file_write(file, p, 0x21);
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kOrderEntry, p);
            joan::save::put_OrderEntry(o, *static_cast<const jo::OrderEntry*>(p));
            return o.end();  // File::file_write 0x005872e0
        };
        if (put(&jo::self<jo::Order>(this)->current) == 0) return 0;  // +0x1d
        file_dword(file, list_count(this));
        for (i32 i = 1; i <= list_count(this); ++i) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_484570(joan::orig::arg::i32(i)));  // 0x00484570
            if (put(e) == 0) return 0;
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x0047f660
    i32 save_1545(void* file) {
        c_save_1545.hit();
        if (joan::save::fieldwise()) {  // [Debug] SaveFieldwise (SAVES_PLAN S4): save::put_Nation, 0 = the raw write
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kNation, this);
            joan::save::put_Nation(o, *jo::self<jo::Nation>(this));
            if (o.end() == 0) return 0;  // File::file_write 0x005872e0
        } else if (file_write(file, this, 0x1545) == 0) {
            return 0;
        }
        for (i32 i = 0; i < 0x14; ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_relation(&jo::self<jo::Nation>(this)->relation1 + i)->save(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047f5e0
        }
        if (jo::view<void*>(jo::self<jo::Nation>(this)->leaders) != nullptr)
            for (i32 i = 0; i < static_cast<i32>(jo::view<i16>(jo::self<jo::Nation>(this)->leader_count)); ++i)
                file_word(file, static_cast<i32>(jo::self<jo::Nation>(this)->leaders[i].value));
        if (jo::view<void*>(jo::self<jo::Nation>(this)->idle_units) != nullptr)
            for (i32 i = 0; i < static_cast<i32>(jo::view<i16>(jo::self<jo::Nation>(this)->idle_count)); ++i)
                file_word(file, static_cast<i32>(jo::self<jo::Nation>(this)->idle_units[i].value));
        if (jo::view<void*>(jo::self<jo::Nation>(this)->stations) != nullptr)
            for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Nation>(this)->station_count); ++i)
                file_word(file, static_cast<i32>(jo::self<jo::Nation>(this)->stations[i].value));
        return 1;
    }

    // ------------------------------------------------------------------ 0x0047fdf0
    std::uintptr_t clear_1d6() {
        jo::self<jo::Place>(this)->u32_1d6 = 0;
        jo::self<jo::Place>(this)->model = nullptr;
        jo::self<jo::Place>(this)->cell = nullptr;
        jo::self<jo::Place>(this)->cell_prev = nullptr;
        jo::self<jo::Place>(this)->cell_next = nullptr;
        jo::self<jo::Place>(this)->facility_tail = nullptr;
        jo::self<jo::Place>(this)->facility_head_ = nullptr;
        return u(this);
    }

    // ------------------------------------------------------------------ 0x00481fb0
    i32 units_save(void* file) {
        c_units_save.hit();
        jo::DynArray* const list = &jo::as_units(this)->list;  // +4 on x86
        file_dword(file, list_count(list));
        for (i32 i = 1; i <= list_count(list); ++i) {
            if (joan::orig::as_units(this)->list.empty_at(joan::orig::arg::i32(i)) != 0) {  // 0x00425d50
                file_word(file, 0);
                continue;
            }
            u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_units(this)->list.ptr_at(joan::orig::arg::i32(i)));  // 0x00425d20
            void* const type = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(unit)->type_id))));  // 0x00425ce0
            if (joan::orig::arg::as<i32>(joan::orig::as_item_def(type)->is_soldier()) == 0) {  // 0x004a4050
                file_word(file, 0);
                continue;
            }
            u8* const u1 = joan::orig::arg::as<u8*>(joan::orig::as_units(this)->list.ptr_at(joan::orig::arg::i32(i)));  // 0x00425d20
            file_word(file, static_cast<i32>(jo::as_map_object(u1)->class_id));
            u8* const u2 = joan::orig::arg::as<u8*>(joan::orig::as_units(this)->list.ptr_at(joan::orig::arg::i32(i)));  // 0x00425d20
            if (thiscall<i32>(static_cast<const std::uintptr_t*>(jo::as_map_object(u2)->vtable)[0x70 / 4], u2, file) == 0) return 0;  // Unit::save (vtable slot 0x70)
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00482ea0
    i32 save_482ea0(void* file) {
        c_save_482ea0.hit();
        file_dword(file, list_count(this));
        for (i32 i = 1; i <= list_count(this); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->empty_at_f(joan::orig::arg::i32(i))) != 0) {  // 0x00485760
                file_word(file, 0);
                continue;
            }
            file_word(file, 1);
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_f(joan::orig::arg::i32(i)));  // 0x00485730
            if (joan::orig::arg::as<i32>(joan::orig::as_order(e)->save(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x00482dc0
        }
        joan::orig::dynarray_b(this)->write_empty_room(joan::orig::arg::file(file));  // 0x00585b40
        return 1;
    }

    // ------------------------------------------------------------------ 0x00483130
    i32 save_483130(void* file) {
        c_save_483130.hit();
        file_dword(file, list_count(this));
        for (i32 i = 1; i <= list_count(this); ++i) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_news_list(this)->item(joan::orig::arg::i32(i)));  // 0x004b96c0
            if (joan::orig::arg::as<i32>(joan::orig::as_news_item(e)->save(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x004830b0
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00484160
    std::uintptr_t clear_277() {
        jo::view<u16>(jo::self<jo::Scenario>(this)->type_count) = 0;
        jo::self<jo::Scenario>(this)->types = nullptr;
        jo::view<u16>(jo::self<jo::Scenario>(this)->item_count) = 0;
        jo::self<jo::Scenario>(this)->items = nullptr;
        jo::self<jo::Scenario>(this)->squads = nullptr;
        return u(this);
    }

    // ------------------------------------------------------------------ 0x00484b10
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S3): save::get_Scene; 0 = the raw read.
    i32 load_d5(void* file) {
        i32 ok;
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kScene, this);  // File::file_read 0x005873a0
            joan::save::get_Scene(in, *jo::self<jo::Scene>(this));
            ok = in.end();
        } else {
            ok = file_read(file, this, 0xd5);
        }
        if (ok == 0) return 0;
        joan::orig::as_scene(this)->clear_pointers();  // 0x00484b60
        return 1;
    }

    // ------------------------------------------------------------------ 0x00485450
    std::uintptr_t clear_6a() {
        jo::self<jo::World>(this)->path_space = nullptr;
        jo::self<jo::World>(this)->box_tree = nullptr;
        jo::self<jo::World>(this)->entries = nullptr;
        jo::view<u32>(jo::self<jo::World>(this)->entry_count) = 0;
        jo::view<u32>(jo::self<jo::World>(this)->entry_cap) = 0;
        return u(this);
    }

    // ------------------------------------------------------------------ 0x00486090
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S4): save::put_Trigger; 0 = the raw write.
    i32 save_43(void* file) {
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kTrigger, this);
            joan::save::put_Trigger(o, *jo::self<jo::Trigger>(this));
            return o.end() == 0 ? 0 : 1;  // File::file_write 0x005872e0
        }
        return file_write(file, this, 0x43) == 0 ? 0 : 1;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("List::save_21", 0x0047f080, &H::save_21);
B99("Sub::save_1545", 0x0047f660, &H::save_1545);
B99("Obj::clear_1d6", 0x0047fdf0, &H::clear_1d6);
B99("Units::save", 0x00481fb0, &H::units_save);
B99("List::save_482ea0", 0x00482ea0, &H::save_482ea0);
B99("List::save_483130", 0x00483130, &H::save_483130);
B99("Obj::clear_277", 0x00484160, &H::clear_277);
B99("Sub::load_d5", 0x00484b10, &H::load_d5);
B99("Obj::clear_6a", 0x00485450, &H::clear_6a);
B99("Sub::save_43", 0x00486090, &H::save_43);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b106_04.cpp (game/b106_04)
//
//      0x0047f410 List::load_47f410(reader)         thiscall, ret 4 (dead: thunk 0x004056d2 has
//                 no references). Reads a count, then per entry a word: 0 -> push_null(1) on
//                 this+4, else new(0x24) + ctor 0x004afbc0 + load 0x0047f5a0 (returns 0 on
//                 failure, leaking the object like the original) + push(&obj) on this+4.
namespace {
namespace game_b106_04 {

// CRT (direct calls in the original).
void* crt_new(u32 n) { return joan::crt::operator_new(joan::orig::arg::u32(n)); }        // operator new

JOAN_COUNTER(c_load_47f410, "List::load_47f410");

struct H {
    // -----------------------------------------------------------------------------------
    // 0x0047f410
    i32 load_47f410(void* rd) {
        c_load_47f410.hit();
        u8* const self = reinterpret_cast<u8*>(this);
        const i32 n = joan::orig::as_file(rd)->file_get_long();   // 0x00587e20
        for (i32 i = 1; i <= n; ++i) {
            const i16 kind = joan::orig::as_file(rd)->file_get_short();                     // 0x00587c20
            if (kind == 0) {
                joan::orig::dynarray(self + sizeof(void*))->add_blank(1);  // 0x00584bd0
                continue;
            }
            void* const mem = crt_new(static_cast<std::uint32_t>(sizeof(jo::Children)));  // 0x24 on x86
            void* o = mem != nullptr ? joan::orig::as_children(mem)->construct() : nullptr;  // 0x004afbc0
            if (joan::orig::arg::as<i32>(joan::orig::as_children(o)->load(rd)) == 0) return 0;                  // 0x0047f5a0
            joan::orig::dynarray_b(self + sizeof(void*))->linkin(&o);                                 // 0x005857a0
        }
        joan::orig::dynarray_b(self + sizeof(void*))->read_empty_room(joan::orig::arg::file(rd));  // 0x00585bb0
        joan::orig::as_list(self)->on_loaded();          // 0x0047f560
        return 1;
    }

    // -----------------------------------------------------------------------------------
    // 0x00525080
    struct Frame525 {         // ebp-0x16c .. ebp-0xc of the original
        char b16c[0x20];      // ebp-0x16c
        u8 pad14c[4];         // ebp-0x14c (NUL of the 0x20 read)
        char b148[0x28];      // ebp-0x148
        u8 pad120[4];         // ebp-0x120 (NUL of the 0x28 read)
        char b11c[8];         // ebp-0x11c
        u8 pad114[4];         // ebp-0x114 (NUL of the 8 read)
        char b110[0x100];     // ebp-0x110
        i32 i;                // ebp-0x10 (NUL of the 0x100 read lands in its low byte)
    };
    static_assert(sizeof(Frame525) == 0x160 && offsetof(Frame525, i) == 0x15c, "frame");
};

JOAN_REPLACE("game/b106_04", "List::load_47f410", 0x0047f410, &H::load_47f410);

}  // namespace game_b106_04
}  // namespace

// ---------------------------------------- from modules/game_b105_03.cpp (game/b105_03)
//
//    - empty thiscall stubs (never write eax; the C++ versions leave eax untouched as well):
//        ret 0:    0x0046db80, 0x0046e690, 0x0046eca0, 0x0047b510, 0x0047b530, 0x0047b550,
//                  0x0047b570, 0x0047b590, 0x0047b780, 0x0047b7f0, 0x0047b810, 0x0047b920,
//                  0x0047b940, 0x0047b980, 0x0047b9a0, 0x0047b9c0, 0x0047ba00, 0x0047ba20,
//                  0x0047ba40, 0x0047ba60, 0x0047baa0, 0x0047bac0, 0x0047bae0, 0x0047bb20,
//                  0x0047bb40, 0x0047bb60, 0x0047bba0, 0x0047f560, 0x0047f5c0,
//                  0x004802d0 (vtable 0x9d07dc), 0x00480670 (vtable 0x9d0954),
//                  0x00480be0 (vtable 0x9d0854)
//        ret 8:    0x0046d800 (vtable slots 0x9d0794 / 0x9d080c / 0x9d090c / 0x9d0994 / 0x9d0a94)
//        ret 0x10: 0x0046ebe0
//      Direct callers: 0x0046e321, 0x0046e62d, 0x0047f4f8 and the state-switch functions
//      0x00499540 / 0x0052f930 (enter / leave handlers per state; 0x00499540 passes the enter
//      stub's eax out through its own `ret 4`, but all four of its callers overwrite eax).
//      Virtual call sites: the only thiscall 2-arg `call [reg+0x14]` in game code is 0x0044f67c
//      (eax reloaded right after); every thiscall `call [reg+0x5c]` (0x004802a4, 0x00480394,
//      0x00480601, 0x00480b13) is followed by `mov eax,1`. No vtable outside those listed
//      overrides either slot.
//      0x0046db80, 0x0046eca0, 0x0046ebe0, 0x0047b510..0x0047b590, 0x0047b780, 0x0047b7f0 and
//
//      0x0047f5c0 have no references at all (dead code).
namespace {
namespace game_b105_03 {

struct H {
    // ---------------------------------------------------------------- empty stubs
    void stub0() {}

    i32 true_ret4(void*) { return 1; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_03", name, addr, fn)
B("Obj::stub_47f560", 0x0047f560, &H::stub0);
B("Obj::stub_47f5c0", 0x0047f5c0, &H::stub0);
B("Obj::stub_4802d0", 0x004802d0, &H::stub0);
B("Obj::stub_480670", 0x00480670, &H::stub0);
B("Obj::stub_480be0", 0x00480be0, &H::stub0);
B("Obj::true_47f580", 0x0047f580, &H::true_ret4);
B("Obj::true_47f5a0", 0x0047f5a0, &H::true_ret4);
#undef B

}  // namespace game_b105_03
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
//
//      0x0047f5e0 RecF4::read             (thiscall(stream), ret 4) - 0x005872e0(this, 0xf4).
//
//      0x00482b50 List::write             (thiscall(stream), ret 4)
//
//      0x00483dd0 Obj::clear_186          (thiscall, ret 0)
//
//      0x004841d0 Rec1a::read_strings     (thiscall(stream), ret 4)
namespace {
namespace game_b94_02 {

void* const kTypes = obj(d::g_item_res);

JOAN_COUNTER(c_list_write, "List::write");
JOAN_COUNTER(c_read_strings, "Rec1a::read_strings");

u32 strlen_(const void* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }  // strlen

i32 stream_read(void* stream, const void* p, i32 n) { return joan::orig::as_file(stream)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }  // 0x005872e0
i32 stream_put16(void* stream, i32 v) { return joan::orig::as_file(stream)->file_put_short(joan::orig::arg::i16(v)); }  // 0x00587ba0
i32 stream_put32(void* stream, i32 v) { return joan::orig::as_file(stream)->file_put_long(joan::orig::arg::i32(v)); }  // 0x00587da0

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------------ 0x0047f5e0
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S4): save::put_Rel; 0 = the raw write.
    i32 read_f4(void* stream) {
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(stream), joan::save::kRel, this);
            joan::save::put_Rel(o, *jo::self<jo::Rel>(this));
            return o.end() == 0 ? 0 : 1;  // File::file_write 0x005872e0
        }
        return stream_read(stream, this, 0xf4) == 0 ? 0 : 1;
    }

    // ------------------------------------------------------------------------ 0x00482b50
    i32 list_write(void* stream) {
        c_list_write.hit();
        jo::DynArray* const list = &jo::as_units(this)->list;  // +4 on x86 (g_a50530: a pointer at +0)
        stream_put32(stream, joan::orig::as_dynarray(list)->size());  // 0x00417ad0
        for (i32 i = 1; i <= joan::orig::as_dynarray(list)->size(); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_types(this)->empty_at(joan::orig::arg::i32(i))) != 0) {  // 0x00485410
                stream_put16(stream, 0);
                continue;
            }
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->at_plus4(joan::orig::arg::u32(i)));                                      // 0x004853e0
            void* const type = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(e)->type_id))));  // 0x00425ce0
            if (joan::orig::arg::as<i32>(joan::orig::as_item_def(type)->is_soldier()) != 0) {                                            // 0x004a4050
                stream_put16(stream, 1);
                u8* const e2 = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->at_plus4(joan::orig::arg::u32(i)));  // 0x004853e0
                stream_put16(stream, static_cast<i32>(jo::view<u16>(jo::as_map_object(e2)->index)));
            } else {
                stream_put16(stream, 0);
            }
        }
        joan::orig::dynarray_b(list)->write_empty_room(joan::orig::arg::file(stream));  // 0x00585b40
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00483dd0
    void clear_186() { jo::self<jo::ScenarioEntry>(this)->script = nullptr; }

    // ------------------------------------------------------------------------ 0x004841d0
    i32 read_strings(void* stream) {
        c_read_strings.hit();
        if (joan::save::fieldwise()) {  // [Debug] SaveFieldwise (SAVES_PLAN S4): save::put_EndCon, 0 = the raw write
            joan::save::Out o(*joan::orig::as_file(stream), joan::save::kEndCon, this);
            joan::save::put_EndCon(o, *jo::self<jo::EndCon>(this));
            if (o.end() == 0) return 0;  // File::file_write 0x005872e0
        } else if (stream_read(stream, this, 0x1a) == 0) {
            return 0;
        }
        if (jo::view<void*>(jo::self<jo::EndCon>(this)->text) != nullptr) {
            stream_put16(stream, static_cast<i32>(strlen_(jo::view<void*>(jo::self<jo::EndCon>(this)->text)) + 1));
            const i32 n = static_cast<i32>(strlen_(jo::view<void*>(jo::self<jo::EndCon>(this)->text)) + 1);
            stream_read(stream, jo::view<void*>(jo::self<jo::EndCon>(this)->text), n);
        }
        if (jo::view<void*>(jo::self<jo::EndCon>(this)->text2) != nullptr) {
            stream_put16(stream, static_cast<i32>(strlen_(jo::view<void*>(jo::self<jo::EndCon>(this)->text2)) + 1));
            const i32 n = static_cast<i32>(strlen_(jo::view<void*>(jo::self<jo::EndCon>(this)->text2)) + 1);
            stream_read(stream, jo::view<void*>(jo::self<jo::EndCon>(this)->text2), n);
        }
        return 1;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("RecF4::read", 0x0047f5e0, &H::read_f4);
B94("List::write", 0x00482b50, &H::list_write);
B94("Obj::clear_186", 0x00483dd0, &H::clear_186);
B94("Rec1a::read_strings", 0x004841d0, &H::read_strings);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
//
//      0x0047f620 Sub::load_f4(file)            (thiscall, ret 4)
//
//      0x0047fe80 List::save_items(file)        (thiscall, ret 4)
//
//      0x00480110 Sub::load_75(file)            (thiscall, ret 4)
//
//      0x00482e50 Obj::clear_482e50             (thiscall, ret 0) - returns this.
//
//      0x00483b50 Sub::load_4(file)             (thiscall, ret 4)
//
//      0x004845a0 Sub::load_14d(file)           (thiscall, ret 4)
//
//      0x00484e40 Sub::load_ac(file)            (thiscall, ret 4)
//
//      0x00485d30 List::save_e0(file)           (thiscall, ret 4)
namespace {
namespace game_b98_02 {

JOAN_COUNTER(c_save_items, "List::save_items");
JOAN_COUNTER(c_load_75, "Sub::load_75");
JOAN_COUNTER(c_load_14d, "Sub::load_14d");
JOAN_COUNTER(c_load_ac, "Sub::load_ac");
JOAN_COUNTER(c_save_e0, "List::save_e0");

// File I/O on the save file object.
i32 file_write(void* file, const void* p, i32 n) {
    return joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n));  // 0x005872e0
}
i32 file_read(void* file, void* p, i32 n) {
    return joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n));  // 0x005873a0
}
i16 file_word(void* file) { return joan::orig::arg::as<i16>(joan::orig::as_file(file)->file_get_short()); }  // 0x00587c20

struct H {
    // ------------------------------------------------------------------ 0x0047f620
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S4): save::get_Rel; 0 = the raw read.
    i32 load_f4(void* file) {
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kRel, this);  // File::file_read 0x005873a0
            joan::save::get_Rel(in, *jo::self<jo::Rel>(this));
            return in.end() != 0 ? 1 : 0;
        }
        return file_read(file, this, 0xf4) != 0 ? 1 : 0;
    }

    // ------------------------------------------------------------------ 0x0047fe80
    i32 save_items(void* file) {
        c_save_items.hit();
        joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(list_count(this)));  // 0x00587da0
        for (i32 i = 1; i <= list_count(this); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_missing_ca0(joan::orig::arg::i32(i))) != 0) {  // 0x00456ca0
                joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(0));  // 0x00587ba0
            } else {
                joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(1));  // 0x00587ba0
                void* const item = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_c70(joan::orig::arg::i32(i)));  // 0x00456c70
                if (joan::orig::arg::as<i32>(joan::orig::as_place(item)->save(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047fc80
            }
        }
        joan::orig::dynarray_b(this)->write_empty_room(joan::orig::arg::file(file));  // 0x00585b40
        return 1;
    }

    // ------------------------------------------------------------------ 0x00480110
    i32 load_75(void* file) {
        c_load_75.hit();
        i32 ok;  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::get_FacilityBase; 0 = the raw read
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kFacilityBase, reinterpret_cast<u8*>(this) + 4);  // File::file_read 0x005873a0
            joan::save::get_FacilityBase(in, *jo::self<jo::Facility>(this));
            ok = in.end();
        } else {
            ok = file_read(file, reinterpret_cast<u8*>(this) + 4, 0x75);
        }
        if (ok == 0) return 0;
        joan::orig::as_facility(this)->clear_pointers();  // 0x004801d0
        if (jo::self<jo::Facility>(this)->owner != nullptr) {
            jo::self<jo::Facility>(this)->owner = nullptr;
            const i32 id = static_cast<i32>(file_word(file));
            void* const table = obj(d::g_place_array);
            if (joan::orig::arg::as<i32>(joan::orig::as_list(table)->is_missing_ca0(joan::orig::arg::i32(id))) != 0) return 0;  // 0x00456ca0
            void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(table)->get_c70(joan::orig::arg::i32(id)));    // 0x00456c70
            joan::orig::as_place(o)->link_facility(joan::orig::arg::mptr(static_cast<void*>(this)));         // 0x004c39d0
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00482e50
    void* clear_482e50() {
        jo::self<jo::Missile>(this)->trail = nullptr;
        jo::view<i32>(jo::self<jo::Missile>(this)->model_id) = 0;
        jo::view<i32>(jo::self<jo::Missile>(this)->expired) = 0;
        jo::view<i32>(jo::self<jo::Missile>(this)->shooter_record) = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x00483b50
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S4): save::get_BattleState; 0 = the raw read.
    i32 load_4(void* file) {
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kBattleState, this);  // File::file_read 0x005873a0
            joan::save::get_BattleState(in, *jo::self<jo::BattleState>(this));
            return in.end() != 0 ? 1 : 0;
        }
        return file_read(file, this, 4) != 0 ? 1 : 0;
    }

    // ------------------------------------------------------------------ 0x004845a0
    // SaveFieldwise=1: save::get_HudLog per chunk of save::kHudLog (see Sub::save_14d); 0 = the raw reads.
    i32 load_14d(void* file) {
        c_load_14d.hit();
        jo::Hud& hud = *jo::self<jo::Hud>(this);
        void* const chunk_at[9] = {&hud.blink_on, &hud.blink_state, &hud.blink_time, &hud.blink_start, &hud.obj_alpha, &hud.obj_1a7, &hud.obj_count, &hud.obj_time, &hud.obj_id};  // x64: the members, not this + x86 offset
        if (joan::save::fieldwise()) {
            for (u32 k = 0; k < 9; ++k) {
                const joan::save::HudChunk& c = joan::save::kHudLog[k];
                joan::save::In in(joan::save::port(*joan::orig::as_file(file)), c.rec, chunk_at[k]);
                joan::save::get_HudLog(in, k, *jo::self<jo::Hud>(this));
                if (in.end() == 0) return 0;  // File::file_read 0x005873a0
            }
            return 1;
        }
        if (file_read(file, chunk_at[0], 1) == 0) return 0;
        if (file_read(file, chunk_at[1], 1) == 0) return 0;
        if (file_read(file, chunk_at[2], 4) == 0) return 0;
        if (file_read(file, chunk_at[3], 4) == 0) return 0;
        if (file_read(file, chunk_at[4], 0x28) == 0) return 0;
        if (file_read(file, chunk_at[5], 0x28) == 0) return 0;
        if (file_read(file, chunk_at[6], 2) == 0) return 0;
        if (file_read(file, chunk_at[7], 0x50) == 0) return 0;
        if (file_read(file, chunk_at[8], 0xa0) == 0) return 0;
        return 1;
    }

    // ------------------------------------------------------------------ 0x00484e40
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S3): save::get_Cell (world: Ptr::Kept); 0 = the raw read.
    i32 load_ac(void* file) {
        c_load_ac.hit();
        jo::World* const keep = jo::self<jo::Cell>(this)->world;
        i32 ok;
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kCell, this);  // File::file_read 0x005873a0
            joan::save::get_Cell(in, *jo::self<jo::Cell>(this));
            ok = in.end();
        } else {
            ok = file_read(file, this, 0xac);
        }
        if (ok == 0) return 0;
        jo::self<jo::Cell>(this)->world = keep;
        jo::view<void*>(jo::self<jo::Cell>(this)->sectors) = joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(static_cast<u32>(jo::view<i32>(jo::self<jo::Cell>(this)->u32_68)) * static_cast<u32>(sizeof(jo::WorldSectorPtrArray))), str(d::s_d_ww_gfile3_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9ffcd4)) + 9));  // 0x0058e7b0
        const i32 n = jo::view<i32>(jo::self<jo::Cell>(this)->u32_68) * 0x21;
        jo::view<void*>(jo::self<jo::Cell>(this)->sector_names) = joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(n), str(d::s_d_ww_gfile3_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9ffcd4)) + 0xb));  // 0x0058e7b0
        jo::view<void*>(jo::self<jo::Cell>(this)->occluders) = joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(jo::self<jo::Cell>(this)->occluder_cap * static_cast<std::int32_t>(sizeof(jo::Occluder))), str(d::s_d_ww_gfile3_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9ffcd4)) + 0xd));  // 0x0058e7b0
        joan::orig::as_world_cell(this)->clear_pointers();  // 0x00484f50
        return 1;
    }

    // ------------------------------------------------------------------ 0x00485d30
    i32 save_e0(void* file) {
        c_save_e0.hit();
        joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(list_count(this)));  // 0x00587da0
        for (i32 i = 1; i <= list_count(this); ++i) {
            void* const e = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (joan::save::fieldwise()) {  // [Debug] SaveFieldwise (SAVES_PLAN S4): save::put_ZoneDef, 0 = the raw write
                joan::save::Out o(*joan::orig::as_file(file), joan::save::kZoneDef, e);
                joan::save::put_ZoneDef(o, *static_cast<const jo::ZoneDef*>(e));
                if (o.end() == 0) return 0;  // File::file_write 0x005872e0
            } else if (file_write(file, e, 0xe0) == 0) {
                return 0;
            }
        }
        return 1;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Sub::load_f4", 0x0047f620, &H::load_f4);
B98("List::save_items", 0x0047fe80, &H::save_items);
B98("Sub::load_75", 0x00480110, &H::load_75);
B98("Obj::clear_482e50", 0x00482e50, &H::clear_482e50);
B98("Sub::load_4", 0x00483b50, &H::load_4);
B98("Sub::load_14d", 0x004845a0, &H::load_14d);
B98("Sub::load_ac", 0x00484e40, &H::load_ac);
B98("List::save_e0", 0x00485d30, &H::save_e0);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b104_02.cpp (game/b104_02)
//
//    - Obj::reset_4803c0 0x004803c0 (ret 0, vtable slot 0x009d09dc) -> 0x0046ba20's eax.
//
//    - Obj::save_483250 0x00483250 (file), ret 4 -> 1 / 0.
//
//    - Obj::load_4839c0 0x004839c0 (file), ret 4 -> 1 / 0.
//
//    - Obj::load_483f10 0x00483f10 (file), ret 4 -> 1 / 0.
//
//    - Obj::save_484780 0x00484780 (file), ret 4 -> 1 / 0.
//
//  - 0x00467590 / 0x0047d8f0 / 0x004839c0 / 0x00483f10: `new` + constructor under an EH state
//    that only frees the block if the constructor throws; normal path reproduced.
namespace {
namespace game_b104_02 {

JOAN_COUNTER(c_save_483250, "Obj::save_483250");
JOAN_COUNTER(c_load_4839c0, "Obj::load_4839c0");
JOAN_COUNTER(c_load_483f10, "Obj::load_483f10");
JOAN_COUNTER(c_save_484780, "Obj::save_484780");

struct H {
    u32 reset_4803c0() {
        jo::self<jo::Siege>(this)->life_bar = nullptr;
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_workshop_facility(this)->create_life_bar());  // 0x0046ba20
        jo::self<jo::Siege>(this)->u8_2f0e = 0;
        jo::self<jo::Siege>(this)->u8_2f0f = 0;
        return r;
    }

    // ------------------------------------------------------------------ file I/O
    u16 read_word(void* file) { return joan::orig::arg::as<u16>(joan::orig::as_file(file)->file_get_short()); }  // 0x00587c20
    i32 save_483250(void* file) {
        c_save_483250.hit();
        // Profile 0x48 + name 0x20 field by field (save::s5::write_profile_head, re/a6/SAVES_PLAN.md S5; [Debug]
        // SaveFieldwise=0: the raw writes)
        if (joan::save::s5::write_profile_head(*jo::as_file(file), *jo::self<jo::Profile>(this)) == 0) return 0;  // 0x005872e0 x2
        joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Profile>(this)->unit_count)));  // 0x00587da0
        for (i32 i = 0; i < jo::self<jo::Profile>(this)->unit_count; ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_key(&jo::self<jo::Profile>(this)->units[i])->read(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x004c93e0
        }
        for (i32 i = 0; i < jo::self<jo::Profile>(this)->unit_count; ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_profile_unit(&jo::self<jo::Profile>(this)->units[i])->write(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x004c9460
        }
        joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Profile>(this)->list_a_count)));  // 0x00587da0
        for (i32 i = 0; i < jo::self<jo::Profile>(this)->list_a_count; ++i) {
            joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(static_cast<u32>(jo::view<u16*>(jo::self<jo::Profile>(this)->list_a)[i])));  // 0x00587ba0
        }
        joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Profile>(this)->list_b_count)));  // 0x00587da0
        for (i32 i = 0; i < jo::self<jo::Profile>(this)->list_b_count; ++i) {
            joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(static_cast<u32>(jo::view<u16*>(jo::self<jo::Profile>(this)->list_b)[i])));  // 0x00587ba0
        }
        joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Profile>(this)->i32_40)));  // 0x00587da0
        return 1;
    }
    i32 load_4839c0(void* file) {
        c_load_4839c0.hit();
        const i32 n = joan::orig::as_file(file)->file_get_long();   // 0x00587e20
        jo::view<u16>(jo::self<jo::SaveList>(this)->current) = read_word(file);  // +0x2e (by member: x64 offset differs)
        for (i32 i = 1; i <= n; ++i) {
            const u16 tag = read_word(file);
            if (static_cast<i16>(tag) == 0) {
                joan::orig::dynarray(this)->add_blank(1);  // 0x00584bd0
                continue;
            }
            void* const mem = op_new(static_cast<std::uint32_t>(sizeof(jo::Profile)));  // 0x48 on x86
            void* child = mem != nullptr ? joan::orig::as_profile(mem)->construct() : nullptr;  // 0x004c7dc0
            if (joan::orig::arg::as<i32>(joan::orig::as_profile(child)->load_483430(joan::orig::arg::mptr(file))) == 0) return 0;               // 0x00483430
            joan::orig::dynarray_b(this)->linkin(&child);                                   // 0x005857a0
        }
        joan::orig::dynarray_b(this)->read_empty_room(joan::orig::arg::file(file));  // 0x00585bb0
        return 1;
    }
    i32 load_483f10(void* file) {
        c_load_483f10.hit();
        if (joan::save::fieldwise()) {  // [Debug] SaveFieldwise (SAVES_PLAN S4): save::get_Scenario, 0 = the raw read
            joan::save::In in(*joan::orig::as_file(file), joan::save::kScenario, this);  // File::file_read 0x005873a0
            joan::save::get_Scenario(in, *jo::self<jo::Scenario>(this));
            if (in.end() == 0) return 0;
        } else if (joan::orig::as_file(file)->file_read(static_cast<void*>(this), 0x289u) == 0) {  // 0x005873a0
            return 0;
        }
        if (jo::self<jo::Scenario>(this)->entry_count > 0) {
            jo::view<void*>(jo::self<jo::Scenario>(this)->entries) = joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(jo::self<jo::Scenario>(this)->entry_count) * sizeof(jo::ScenarioEntry))), str(d::s_d_ww_gfile3_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9ffcc8)) + 8));  // 0x0058e7b0
        }
        for (i32 i = 0; i < jo::self<jo::Scenario>(this)->entry_count; ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_scenario_entry(jo::self<jo::Scenario>(this)->entries + i)->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x00483ca0
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_scenario_entry(&jo::self<jo::Scenario>(this)->local)->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x00483ca0
        joan::orig::as_scenario_entry(&jo::self<jo::Scenario>(this)->local)->clear_186();                            // 0x00483dd0
        joan::orig::as_campaign_state(this)->clear_pointers();                                       // 0x00484160
        if (jo::self<jo::Scenario>(this)->squad_count != 0) {
            jo::view<void*>(jo::self<jo::Scenario>(this)->squads) = joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Scenario>(this)->squad_count) * static_cast<i32>(sizeof(jo::UnitPtr))), str(d::s_d_ww_gfile3_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9ffcc8)) + 0x1e));  // 0x0058e7b0
            for (i32 i = 0; i < jo::self<jo::Scenario>(this)->squad_count; ++i) {
                void* const mem = op_new(static_cast<std::uint32_t>(sizeof(jo::Unit)));  // 0x4b4 on x86
                void* const child = mem != nullptr ? joan::orig::as_squad(mem)->construct() : nullptr;  // 0x004155e0
                jo::view<void*>(jo::self<jo::Scenario>(this)->squads[i].unit) = child;
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(jo::self<jo::Scenario>(this)->squads[i].unit)->load_record(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047de30
            }
        }
        return 1;
    }
    i32 save_484780(void* file) {
        c_save_484780.hit();
        if (joan::save::fieldwise()) {  // [Debug] SaveFieldwise (SAVES_PLAN S4): save::put_Game, 0 = the raw write
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kGame, this);
            joan::save::put_Game(o, *jo::self<jo::Game>(this));
            if (o.end() == 0) return 0;  // File::file_write 0x005872e0
        } else if (joan::orig::as_file(file)->file_write(static_cast<void*>(this), 0x745u) == 0) {  // 0x005872e0
            return 0;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_campaign_state(&jo::self<jo::Game>(this)->scenario)->save(joan::orig::arg::mptr(file))) == 0) return 0;               // 0x00483e00
        for (i32 i = 0; i < jo::self<jo::Game>(this)->end_condition_count; ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_objective(&jo::self<jo::Game>(this)->end_conditions[i])->save(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x004841d0
        }
        joan::orig::as_file(file)->file_write_array(joan::orig::arg::ptr(global<void*>(d::g_a596ac)), joan::orig::arg::u32(global<u32>(d::g_a596b0)), 2u);  // 0x00587510
        joan::orig::as_file(file)->file_write_array(joan::orig::arg::ptr(global<void*>(d::g_a596b8)), joan::orig::arg::u32(global<u32>(d::g_a596bc)), 2u);  // 0x00587510
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_02", name, addr, fn)
B("Obj::reset_4803c0", 0x004803c0, &H::reset_4803c0);
B("Obj::save_483250", 0x00483250, &H::save_483250);
B("Obj::load_4839c0", 0x004839c0, &H::load_4839c0);
B("Obj::load_483f10", 0x00483f10, &H::load_483f10);
B("Obj::save_484780", 0x00484780, &H::save_484780);
#undef B

}  // namespace game_b104_02
}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
//
//      0x00480410 Obj::save_480410(file)           (thiscall, ret 4) - vtable 0x9d094c, 0/1.
//
//      0x004804f0 Obj::load_4804f0(file)           (thiscall, ret 4) - vtable 0x9d0950, 0/1.
//
//      0x00481540 Obj::reset_481540                (thiscall, ret 0) - vtable 0x9d3904.
//
//      0x00481cd0 Obj::load_481cd0(file)           (thiscall, ret 4) - vtable 0x9d3dec, 0/1.
//
//      0x00483800 Obj::load_483800(file)           (thiscall, ret 4) - (dead code).
//
//      0x004854c0 List::save_4854c0(file)          (thiscall, ret 4) - 0/1.
//
//  - 0x00481cd0: the name buffer is 0x84 bytes in the original frame; the length word comes from
//    the save file (written as strlen + 1 by 0x00481ba0, NUL included). Room given here.
//
//  - SEH frames in 0x0046ec30 / 0x0047c3d0 / 0x004804f0 / 0x00481cd0 only guard destructors or
//    new + ctor: normal path reproduced.
namespace {
namespace game_b102_02 {

static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_save_480410, "Obj::save_480410");
JOAN_COUNTER(c_load_4804f0, "Obj::load_4804f0");
JOAN_COUNTER(c_load_481cd0, "Obj::load_481cd0");
JOAN_COUNTER(c_load_483800, "Obj::load_483800");
JOAN_COUNTER(c_save_4854c0, "List::save_4854c0");

void* const kItems = obj(d::g_units);

void* list_at(const void* list, i32 i) { return joan::orig::arg::as<void*>(joan::orig::as_dynarray(list)->get(joan::orig::arg::i32(i))); }  // 0x00417200

i32 file_write(void* f, const void* p, u32 n) { return joan::orig::as_file(f)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }  // 0x005872e0
i32 file_read(void* f, void* p, u32 n) { return joan::orig::as_file(f)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n)); }  // 0x005873a0
i32 file_write_word(void* f, u32 w) { return joan::orig::as_file(f)->file_put_short(joan::orig::arg::i16(w)); }  // 0x00587ba0
i32 file_write_dword(void* f, u32 v) { return joan::orig::as_file(f)->file_put_long(joan::orig::arg::i32(v)); }  // 0x00587da0
i16 file_read_word(void* f) { return joan::orig::arg::as<i16>(joan::orig::as_file(f)->file_get_short()); }  // 0x00587c20

struct H {
    // 0x00480410
    i32 save_480410(void* file) {
        c_save_480410.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_facility(this)->save_base(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x004800a0
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::put_ShopTail / put_ShopStock; 0 = raw.
        const bool fieldwise = joan::save::fieldwise();
        i32 ok;
        if (fieldwise) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kShopTail, reinterpret_cast<u8*>(this) + 0x79);
            joan::save::put_ShopTail(o, *jo::self<jo::Shop>(this));
            ok = o.end();  // File::file_write 0x005872e0
        } else {
            ok = file_write(file, reinterpret_cast<u8*>(this) + 0x79, 0x15);
        }
        if (ok == 0) return 0;
        if (jo::view<void*>(jo::self<jo::Shop>(this)->stock) == nullptr) return 1;
        file_write_dword(file, static_cast<u32>(list_count(jo::view<void*>(jo::self<jo::Shop>(this)->stock))));
        for (i32 i = 1; i <= list_count(jo::view<void*>(jo::self<jo::Shop>(this)->stock)); ++i) {
            void* const e = list_at(jo::view<void*>(jo::self<jo::Shop>(this)->stock), i);
            if (fieldwise) {
                joan::save::Out o(*joan::orig::as_file(file), joan::save::kShopStock, e);
                joan::save::put_ShopStock(o, *static_cast<const jo::ShopStock*>(e));
                ok = o.end();  // File::file_write 0x005872e0
            } else {
                ok = file_write(file, e, 0xe);
            }
            if (ok == 0) return 0;
        }
        return 1;
    }

    // 0x004804f0
    i32 load_4804f0(void* file) {
        c_load_4804f0.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_sub(this)->load_75(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x00480110
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::get_ShopTail / get_ShopStock; 0 = raw.
        const bool fieldwise = joan::save::fieldwise();
        i32 ok;
        if (fieldwise) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kShopTail, reinterpret_cast<u8*>(this) + 0x79);  // File::file_read 0x005873a0
            joan::save::get_ShopTail(in, *jo::self<jo::Shop>(this));
            ok = in.end();
        } else {
            ok = file_read(file, reinterpret_cast<u8*>(this) + 0x79, 0x15);
        }
        if (ok == 0) return 0;
        if (jo::view<void*>(jo::self<jo::Shop>(this)->stock) != nullptr) {
            void* p = game_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
            if (p != nullptr) p = joan::orig::dynarray(p)->construct(0xe, 0x1e, 0);  // 0x005843d0
            jo::view<void*>(jo::self<jo::Shop>(this)->stock) = p;
            const i32 n = joan::orig::as_file(file)->file_get_long();   // 0x00587e20
            for (i32 i = 1; i <= n; ++i) {
                joan::orig::dynarray(jo::view<void*>(jo::self<jo::Shop>(this)->stock))->linkin(nullptr);                 // 0x00584620
                const i32 slot = joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Shop>(this)->stock))->cursor();  // 0x00417520
                void* const e = list_at(jo::view<void*>(jo::self<jo::Shop>(this)->stock), slot);
                if (fieldwise) {
                    joan::save::In in(*joan::orig::as_file(file), joan::save::kShopStock, e);  // File::file_read 0x005873a0
                    joan::save::get_ShopStock(in, *static_cast<jo::ShopStock*>(e));
                    ok = in.end();
                } else {
                    ok = file_read(file, e, 0xe);
                }
                if (ok == 0) return 0;
            }
        }
        virtual_call<u32>(this, 0x5c);
        return 1;
    }

    // 0x00481540
    i32 reset_481540() {
        jo::self<jo::Gate>(this)->blocked_cells = nullptr;
        jo::view<u32>(jo::self<jo::Gate>(this)->blocked_count) = 0;
        const i32 s = static_cast<i32>(jo::self<jo::Gate>(this)->state);
        if (s == 2) jo::self<jo::Gate>(this)->state = 0;
        else if (s == 3) jo::self<jo::Gate>(this)->state = 1;
        return s;
    }

    // 0x00481cd0
    i32 load_481cd0(void* file) {
        c_load_481cd0.hit();
        i32 ok;  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::get_MapObjectBase; 0 = the raw read
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kMapObjectBase, reinterpret_cast<u8*>(this) + 4);  // File::file_read 0x005873a0
            joan::save::get_MapObjectBase(in, *jo::self<jo::MapObject>(this));
            ok = in.end();
        } else {
            ok = file_read(file, reinterpret_cast<u8*>(this) + 4, 0xbe);
        }
        if (ok == 0) return 0;
        const i32 len = static_cast<i32>(file_read_word(file));
        if (len != 0) {
            alignas(4) char name[0x400];  // 0x84 in the original frame; room
            file_read(file, name, static_cast<u32>(len));
            jo::view<u16>(jo::self<jo::MapObject>(this)->type_id) = static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(joan::orig::arg::str(name))));  // 0x004a36c0
        }
        if (jo::view<void*>(jo::self<jo::MapObject>(this)->world_item) != nullptr) {
            void* p = game_new(static_cast<std::uint32_t>(sizeof(jo::WorldItem)));  // 0x52 on x86
            if (p != nullptr) p = joan::orig::as_world_item(p)->construct();  // 0x00456eb0
            jo::view<void*>(jo::self<jo::MapObject>(this)->world_item) = p;
            if (joan::orig::arg::as<i32>(joan::orig::as_world_item(jo::view<void*>(jo::self<jo::MapObject>(this)->world_item))->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x00481140
        }
        joan::orig::as_map_object(this)->reset_links();  // 0x00481ed0
        if (jo::view<void*>(jo::self<jo::MapObject>(this)->world) != nullptr) {
            const i32 id = static_cast<i32>(file_read_word(file));
            jo::view<void*>(jo::self<jo::MapObject>(this)->world) = joan::orig::arg::as<void*>(joan::orig::as_units(obj(d::g_a45f08))->get_420240(joan::orig::arg::i32(id)));  // 0x00420240
            joan::orig::as_map_object(this)->set_world(jo::self<jo::MapObject>(this)->world);                         // 0x004994b0
            joan::orig::as_map_object(this)->set_position(*reinterpret_cast<const Vec3*>(jo::self<jo::MapObject>(this)->pos));  // 0x0041ff50 (+0x56)
        }
        return 1;
    }

    // 0x00483800
    i32 load_483800(void* file) {
        c_load_483800.hit();
        i32 ok;  // [Debug] SaveFieldwise=1 (default): save::get_ProfileUnit (leader / members: tokens, null iff 0); 0 = raw
        if (joan::save::fieldwise()) {
            joan::save::In in(*jo::as_file(file), joan::save::kProfileUnit, this);
            joan::save::get_ProfileUnit(in, *jo::self<jo::ProfileUnit>(this));
            ok = in.end();  // File::file_read 0x005873a0
        } else {
            ok = file_read(file, this, 0x77);
        }
        if (ok == 0) return 0;
        if (jo::view<void*>(jo::self<jo::ProfileUnit>(this)->leader) != nullptr) {
            const i32 id = static_cast<i32>(file_read_word(file));
            jo::view<void*>(jo::self<jo::ProfileUnit>(this)->leader) = joan::orig::as_dynarray(kItems)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
        }
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::ProfileUnit>(this)->member_count); ++i) {
            if (jo::view<void*>(jo::self<jo::ProfileUnit>(this)->members[i]) == nullptr) continue;
            const i32 id = static_cast<i32>(file_read_word(file));
            jo::view<void*>(jo::self<jo::ProfileUnit>(this)->members[i]) = joan::orig::as_dynarray(kItems)->ptr_at(joan::orig::arg::i32(id));
        }
        return 1;
    }

    // 0x004854c0
    i32 save_4854c0(void* file) {
        c_save_4854c0.hit();
        file_write_dword(file, jo::view<u32>(jo::self<jo::WorldArray>(this)->active));
        file_write_dword(file, jo::self<jo::WorldArray>(this)->selected);
        file_write_dword(file, static_cast<u32>(list_count(this)));
        for (i32 i = 1; i <= list_count(this); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->empty_at_561320(joan::orig::arg::i32(i))) != 0) {  // 0x00561320
                file_write_word(file, 0);
                continue;
            }
            file_write_word(file, 1);
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_units(this)->get_420240(joan::orig::arg::i32(i)));  // 0x00420240
            if (joan::orig::arg::as<i32>(joan::orig::as_world(e)->save(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x00485070
        }
        joan::orig::dynarray_b(this)->write_empty_room(joan::orig::arg::file(file));  // 0x00585b40
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("Obj::save_480410", 0x00480410, &H::save_480410);
B("Obj::load_4804f0", 0x004804f0, &H::load_4804f0);
B("Obj::reset_481540", 0x00481540, &H::reset_481540);
B("Obj::load_481cd0", 0x00481cd0, &H::load_481cd0);
B("Obj::load_483800", 0x00483800, &H::load_483800);
B("List::save_4854c0", 0x004854c0, &H::save_4854c0);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_b101_03.cpp (game/b101_03)
//
//    - Obj::load_480880 0x00480880 (stream), ret 4, vtable 0x9d0850: 0x00480110, reads 0x20 bytes
//      into +0x79; for each of the lists +0x79 / +0x7d / +0x89 that was non-null: new(0x1d) +
//      0x005843d0(10, 0x1e, 0), then n = 0x00587e20() records of 10 bytes read into new slots
//      (0x00584620(0) / 0x00417520 / 0x00417200). Then virtual +0x5c, returns 1 (0 on a failed read).
//
//    - List::load_480ce0 0x00480ce0 (stream), ret 4: n records, each a type short: 0 -> 0x00584bd0(1);
//      1 / 2 / 3 / 16 / other -> new(0x140 / 0x8e / 0x99 / 0x2f14 / 0x79) + ctor, virtual +0x58(stream),
//      0x005857a0(&p). Ends with 0x00585bb0(stream). (byte jump table 0x480fd6 decoded.)
//
//    - Obj::load_481140 0x00481140 (stream), ret 4: 0x0047eda0, 0x40 bytes into +0x12, 0x00481220;
//      when +0x36: +0x36 = 0x00420240(short read), 0x00457120(+0x36), 0x00456fe0(Vec3 +0x12).
//
//    - Obj::save_481ba0 0x00481ba0 (stream), ret 4: 0xbe bytes of +4, the name string of
//      0x00425ce0(+0xa) (+0x10, length-prefixed incl. NUL, or short 0), child +0xa2 save, short of +0xa6.
//
//    - List::load_482f70 0x00482f70 (stream), ret 4: new(0x6c) + 0x00420110, 0x00482e00(stream).
//
//    - List::load_4831c0 0x004831c0 (stream), ret 4: n x (0x00584620(0), 0x004830f0(last, stream)).
//
//    - Obj::save_485070 0x00485070 (stream), ret 4: 0x92 bytes, +0x6e child, +0x7a array of +0x72.
//
//    - List::load_4855b0 0x004855b0 (stream), ret 4: +0x2e / +0x32, new(0x92) + 0x0041ea20 /
//      0x00485130(stream); +0x36 = this->0x00420240(+0x32) when +0x32; 0x00585bb0(stream).
//
//    - Obj::load_485930 0x00485930 (stream), ret 4: 0x126e bytes into this, 0x28 bytes into
//      0xa5ba34 (result ignored), 0x00485990.
//
//    - List::load_485f50 0x00485f50 (stream), ret 4: 0x004eee10, 1 byte +0x1d, new(0x43) + 0x004eff60
//      added by 0x00584620(&p), 0x004860d0(0x00485f20(count), stream).
//
//    - List::save_486110 0x00486110 (stream), ret 4: dwords +0x2e/+0x32/+0x36, 9 + 9 dwords at
//      +0x3a / +0x5e, count, per entry flag short + 0x00486420 save, 0x00585b40(stream).
//
//  - SEH frames (0x480880, 0x480ce0, 0x482f70, 0x4855b0, 0x485f50, 0x487d60) only guard new + ctor
//    or the stack objects' destructors: normal path reproduced.
namespace {
namespace game_b101_03 {

static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kTypes = obj(d::g_item_res);
void* const kRes = obj(d::g_a45f08);

u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }

// Stream helpers (thunks).
i32 s_read(void* s, void* p, u32 n) { return joan::orig::as_file(s)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n)); }  // 0x005873a0
i32 s_write(void* s, const void* p, u32 n) { return joan::orig::as_file(s)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }  // 0x005872e0
i32 s_read_i32(void* s) { return joan::orig::as_file(s)->file_get_long(); }                            // 0x00587e20
i16 s_read_i16(void* s) { return joan::orig::arg::as<i16>(joan::orig::as_file(s)->file_get_short()); }  // 0x00587c20
i32 s_write_i16(void* s, u32 v) { return joan::orig::as_file(s)->file_put_short(joan::orig::arg::i16(v)); }  // 0x00587ba0
i32 s_write_i32(void* s, u32 v) { return joan::orig::as_file(s)->file_put_long(joan::orig::arg::i32(v)); }  // 0x00587da0

JOAN_COUNTER(c_load_480880, "Obj::load_480880");
JOAN_COUNTER(c_load_480ce0, "List::load_480ce0");
JOAN_COUNTER(c_load_481140, "Obj::load_481140");
JOAN_COUNTER(c_save_481ba0, "Obj::save_481ba0");
JOAN_COUNTER(c_load_482f70, "List::load_482f70");
JOAN_COUNTER(c_load_4831c0, "List::load_4831c0");
JOAN_COUNTER(c_save_485070, "Obj::save_485070");
JOAN_COUNTER(c_load_4855b0, "List::load_4855b0");
JOAN_COUNTER(c_load_485930, "Obj::load_485930");
JOAN_COUNTER(c_load_485f50, "List::load_485f50");
JOAN_COUNTER(c_save_486110, "List::save_486110");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // 0x00480880 helper: one list slot (+off) of 0x1d bytes, records of 10 bytes.
    bool load_list_10(void* stream, jo::DynArray*& slot) {
        void* const mem = op_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
        void* const list = mem != nullptr ? joan::orig::dynarray(mem)->construct(0xa, 0x1e, 0)  // 0x005843d0
                                          : nullptr;
        jo::view<void*>(slot) = list;
        const i32 n = s_read_i32(stream);
        for (i32 i = 1; i <= n; ++i) {
            joan::orig::dynarray(slot)->linkin(static_cast<void*>(nullptr));      // 0x00584620
            const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_dynarray(slot)->cursor());  // 0x00417520
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_dynarray(slot)->get(joan::orig::arg::i32(idx)));  // 0x00417200
            i32 ok;  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::get_FlagRow; 0 = the raw read
            if (joan::save::fieldwise()) {
                joan::save::In in(*joan::orig::as_file(stream), joan::save::kFlagRow, p);  // File::file_read 0x005873a0
                joan::save::get_FlagRow(in, *static_cast<jo::RecruitRow*>(p));
                ok = in.end();
            } else {
                ok = s_read(stream, p, 0xa);
            }
            if (ok == 0) return false;
        }
        return true;
    }

    // ------------------------------------------------------------------ 0x00480880
    i32 load_480880(void* stream) {
        c_load_480880.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_sub(this)->load_75(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x00480110
        i32 ok;  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::get_FlagFacilityTail; 0 = raw
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(stream), joan::save::kFlagFacilityTail, &jo::self<jo::FlagFacility>(this)->list_79);  // File::file_read 0x005873a0
            joan::save::get_FlagFacilityTail(in, *jo::self<jo::FlagFacility>(this));
            ok = in.end();
        } else {
            ok = s_read(stream, &jo::self<jo::FlagFacility>(this)->list_79, 0x20);
        }
        if (ok == 0) return 0;
        if (jo::self<jo::FlagFacility>(this)->list_79 != nullptr && !load_list_10(stream, jo::self<jo::FlagFacility>(this)->list_79)) return 0;
        if (jo::self<jo::FlagFacility>(this)->list_7d != nullptr && !load_list_10(stream, jo::self<jo::FlagFacility>(this)->list_7d)) return 0;
        if (jo::self<jo::FlagFacility>(this)->list_89 != nullptr && !load_list_10(stream, jo::self<jo::FlagFacility>(this)->list_89)) return 0;
        virtual_call<u32>(this, 0x5c);
        return 1;
    }

    // ------------------------------------------------------------------ 0x00480ce0
    i32 load_480ce0(void* stream) {
        c_load_480ce0.hit();
        const i32 n = s_read_i32(stream);
        for (i32 i = 1; i <= n; ++i) {
            const i16 type = s_read_i16(stream);
            if (static_cast<i32>(type) == 0) {
                joan::orig::dynarray(this)->add_blank(1);  // 0x00584bd0
                continue;
            }
            u32 size;
            std::uintptr_t ctor;
            switch (static_cast<u32>(static_cast<i32>(type) - 1)) {
                case 0: size = static_cast<u32>(sizeof(jo::Building)); ctor = fn::code_value(fn::Obj_ctor_467420); break;   // 0x00467420
                case 1: size = static_cast<u32>(sizeof(jo::Shop)); ctor = fn::code_value(fn::Obj_ctor_469560); break;    // 0x00469560
                case 2: size = static_cast<u32>(sizeof(jo::FlagFacility)); ctor = fn::code_value(fn::Obj_ctor_468300); break;    // 0x00468300
                case 15: size = static_cast<u32>(sizeof(jo::Siege)); ctor = fn::code_value(fn::Obj_ctor_469e90); break; // 0x00469e90
                default: size = static_cast<u32>(sizeof(jo::Facility)); ctor = fn::code_value(fn::Record_Record); break;   // 0x0046d520
            }
            // x86 sizes 0x140 / 0x8e / 0x99 / 0x2f14 / 0x79 (as FacilityFactory::create 0x0046ce70): on x64 the classes are larger.
            void* const mem = op_new(size);
            void* p = mem != nullptr ? thiscall<void*>(ctor, mem) : nullptr;
            if (virtual_call<i32>(p, 0x58, stream) == 0) return 0;
            joan::orig::dynarray_b(this)->linkin(static_cast<void*>(&p));    // 0x005857a0
        }
        joan::orig::dynarray_b(this)->read_empty_room(joan::orig::arg::file(stream));  // 0x00585bb0
        return 1;
    }

    // ------------------------------------------------------------------ 0x00481140
    i32 load_481140(void* stream) {
        c_load_481140.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_item_stack(this)->load(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x0047eda0
        i32 ok;  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::get_WorldItem; 0 = the raw read
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(stream), joan::save::kWorldItem, jo::self<jo::WorldItem>(this)->pos);  // File::file_read 0x005873a0
            joan::save::get_WorldItem(in, *jo::self<jo::WorldItem>(this));
            ok = in.end();
        } else {
            ok = s_read(stream, jo::self<jo::WorldItem>(this)->pos, 0x40);
        }
        if (ok == 0) return 0;
        joan::orig::as_world_item(this)->clear_links();  // 0x00481220
        if (jo::self<jo::WorldItem>(this)->world != nullptr) {
            const i32 id = static_cast<i32>(s_read_i16(stream));
            jo::self<jo::WorldItem>(this)->world = static_cast<jo::World*>(joan::orig::as_units(kRes)->get_420240(joan::orig::arg::i32(id)));  // 0x00420240
            joan::orig::as_world_item(this)->set_state(jo::self<jo::WorldItem>(this)->world);       // 0x00457120
            joan::orig::as_world_item(this)->set_pos(jo::view<u32>(jo::self<jo::WorldItem>(this)->pos[0]), jo::view<u32>(jo::self<jo::WorldItem>(this)->pos[1]), jo::view<u32>(jo::self<jo::WorldItem>(this)->pos[2]));  // 0x00456fe0
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00481ba0
    // Saved pointers (S7 save audit, data image at a random base): the 0xbe raw bytes (+4..+0xc1 of the item) hold
    // the heap pointers +0xa2 (child) and +0xa6 (unit), which Obj::load_481cd0 (game_b102_02.cpp) only tests for
    // null and then re-creates / looks up by the id written below. The image addresses SaveAudit reported at record
    // +0xa8 / +0xac (item +0xac / +0xb0, inside the runtime dwords +0xaa / +0xae / +0xb2) are never read after a
    // load: load_481cd0 calls ObjD::reset (0x00481ed0), which zeroes +0x82..+0x91 and +0xaa..+0xc1, before anything
    // uses them. [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S2): save::put_MapObjectBase (world_item /
    // world as null flags, the cleared ones as tokens); 0 = the raw write.
    i32 save_481ba0(void* stream) {
        c_save_481ba0.hit();
        i32 ok;
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(stream), joan::save::kMapObjectBase, self() + 4);
            joan::save::put_MapObjectBase(o, *jo::self<jo::MapObject>(this));
            ok = o.end();  // File::file_write 0x005872e0
        } else {
            ok = s_write(stream, self() + 4, 0xbe);
        }
        if (ok == 0) return 0;
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(jo::self<jo::MapObject>(this)->type_id)));  // 0x00425ce0
        if (jo::as_item_def(t)->name != nullptr) {
            s_write_i16(stream, crt_strlen(jo::view<const char*>(jo::as_item_def(t)->name)) + 1);
            const u32 len = crt_strlen(jo::view<const char*>(jo::as_item_def(t)->name)) + 1;
            s_write(stream, jo::view<const char*>(jo::as_item_def(t)->name), len);
        } else {
            s_write_i16(stream, 0);
        }
        if (jo::self<jo::MapObject>(this)->world_item != nullptr) {
            if (joan::orig::arg::as<i32>(joan::orig::as_world_item(jo::view<void*>(jo::self<jo::MapObject>(this)->world_item))->save(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x004810b0
        }
        if (jo::self<jo::MapObject>(this)->world != nullptr) {
            if (s_write_i16(stream, jo::view<u16>(jo::self<jo::MapObject>(this)->world->world_id)) == 0) return 0;
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00482f70
    i32 load_482f70(void* stream) {
        c_load_482f70.hit();
        const i32 n = s_read_i32(stream);
        for (i32 i = 1; i <= n; ++i) {
            if (static_cast<i32>(s_read_i16(stream)) == 0) {
                joan::orig::dynarray(this)->add_blank(1);  // 0x00584bd0
                continue;
            }
            void* const mem = op_new(static_cast<std::uint32_t>(sizeof(jo::Missile)));  // 0x6c on x86
            void* p = mem != nullptr ? joan::orig::as_missile(mem)->construct() : nullptr;  // 0x00420110
            if (joan::orig::arg::as<i32>(joan::orig::as_order(p)->load(joan::orig::arg::mptr(stream))) == 0) return 0;               // 0x00482e00
            joan::orig::dynarray_b(this)->linkin(static_cast<void*>(&p));                 // 0x005857a0
        }
        joan::orig::dynarray_b(this)->read_empty_room(joan::orig::arg::file(stream));  // 0x00585bb0
        return 1;
    }

    // ------------------------------------------------------------------ 0x004831c0
    i32 load_4831c0(void* stream) {
        c_load_4831c0.hit();
        const i32 n = s_read_i32(stream);
        for (i32 i = 1; i <= n; ++i) {
            joan::orig::dynarray(this)->linkin(static_cast<void*>(nullptr));      // 0x00584620
            const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(idx)));  // 0x00417200
            if (joan::orig::arg::as<i32>(joan::orig::as_news_item(p)->load(joan::orig::arg::mptr(stream))) == 0) return 0;       // 0x004830f0
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00485070
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S3): save::put_World; 0 = the raw write.
    i32 save_485070(void* stream) {
        c_save_485070.hit();
        i32 ok;
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(stream), joan::save::kWorld, this);
            joan::save::put_World(o, *jo::self<jo::World>(this));
            ok = o.end();  // File::file_write 0x005872e0
        } else {
            ok = s_write(stream, this, 0x92);
        }
        if (ok == 0) return 0;
        if (jo::self<jo::World>(this)->scene != nullptr) {
            if (joan::orig::arg::as<i32>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::World>(this)->scene))->save(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x00484ad0
        }
        for (i32 i = 0; i < jo::self<jo::World>(this)->cell_count; ++i) {
            void* const e = jo::view<void**>(jo::self<jo::World>(this)->cells)[i];
            if (joan::orig::arg::as<i32>(joan::orig::as_world_cell(e)->save(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x00484e00
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x004855b0
    i32 load_4855b0(void* stream) {
        c_load_4855b0.hit();
        // (S3: WorldArray members by name - x86 offsets active +0x2e, selected +0x32, u32_36 +0x36)
        jo::self<jo::WorldArray>(this)->active = s_read_i32(stream);
        jo::self<jo::WorldArray>(this)->selected = static_cast<u32>(s_read_i32(stream));
        const i32 n = s_read_i32(stream);
        for (i32 i = 1; i <= n; ++i) {
            if (static_cast<i32>(s_read_i16(stream)) == 0) {
                joan::orig::dynarray(this)->add_blank(1);  // 0x00584bd0
                continue;
            }
            void* const mem = op_new(static_cast<std::uint32_t>(offsetof(jo::World, gap_92)));  // 0x92 on x86: a World (its members end at gap_92; the facade runs on to 0x201)
            void* p = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_battle(mem)->init()) : nullptr;  // 0x0041ea20
            if (joan::orig::arg::as<i32>(joan::orig::as_world(p)->load(joan::orig::arg::mptr(stream))) == 0) return 0;               // 0x00485130
            joan::orig::dynarray_b(this)->linkin(static_cast<void*>(&p));                 // 0x005857a0
        }
        if (jo::self<jo::WorldArray>(this)->selected != 0)
            jo::self<jo::WorldArray>(this)->u32_36 = static_cast<jo::World*>(joan::orig::arg::as<void*>(joan::orig::as_units(this)->get_420240(joan::orig::arg::i32(jo::self<jo::WorldArray>(this)->selected))));  // 0x00420240
        joan::orig::dynarray_b(this)->read_empty_room(joan::orig::arg::file(stream));  // 0x00585bb0
        return 1;
    }

    // ------------------------------------------------------------------ 0x00485930
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S3): save::get_Player (u32_1261 as stored, zeroed below)
    // and save::get_Camera (into 0x00a5ba34 = Player +0x4c, result ignored); 0 = the raw reads.
    i32 load_485930(void* stream) {
        c_load_485930.hit();
        if (joan::save::fieldwise()) {
            {
                joan::save::In in(*joan::orig::as_file(stream), joan::save::kPlayer, this);  // File::file_read 0x005873a0
                joan::save::get_Player(in, *jo::self<jo::Player>(this));
                if (in.end() == 0) return 0;
            }
            void* const cam = obj(d::g_a5ba34);
            joan::save::In in(*joan::orig::as_file(stream), joan::save::kCamera, cam);
            joan::save::get_Camera(in, *static_cast<jo::CamPose*>(cam));
            in.end();
        } else {
            if (s_read(stream, this, 0x126e) == 0) return 0;
            s_read(stream, obj(d::g_a5ba34), 0x28);
        }
        joan::orig::as_player(this)->reset_state();  // 0x00485990
        return 1;
    }

    // ------------------------------------------------------------------ 0x00485f50
    i32 load_485f50(void* stream) {
        c_load_485f50.hit();
        joan::orig::as_list(this)->delete_all_eee10();  // 0x004eee10
        if (joan::save::fieldwise()) {  // [Debug] SaveFieldwise (SAVES_PLAN S4): the byte +0x1d as a save::In chunk
            joan::save::In in(joan::save::port(*joan::orig::as_file(stream)), joan::save::kTriggersByte1d, &jo::self<jo::Triggers>(this)->u8_1d);
            jo::self<jo::Triggers>(this)->u8_1d = in.u8();
            if (in.end() == 0) return 0;  // File::file_read 0x005873a0
        } else if (s_read(stream, &jo::self<jo::Triggers>(this)->u8_1d, 1) == 0) {
            return 0;
        }
        const i32 n = s_read_i32(stream);
        for (i32 i = 1; i <= n; ++i) {
            void* const mem = op_new(static_cast<u32>(sizeof(jo::Trigger)));  // 0x43 on x86
            void* p = mem != nullptr ? joan::orig::as_trigger(mem)->construct() : nullptr;  // 0x004eff60
            joan::orig::dynarray(this)->linkin(static_cast<void*>(&p));                   // 0x00584620
            const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_485f20(joan::orig::arg::u32(idx)));                // 0x00485f20
            if (joan::orig::arg::as<i32>(joan::orig::as_save_entry(e)->load(joan::orig::arg::mptr(stream))) == 0) return 0;               // 0x004860d0
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00486110
    i32 save_486110(void* stream) {
        c_save_486110.hit();
        s_write_i32(stream, jo::view<u32>(jo::self<jo::MsgLog>(this)->count));
        s_write_i32(stream, jo::self<jo::MsgLog>(this)->last);
        s_write_i32(stream, jo::view<u32>(jo::self<jo::MsgLog>(this)->cur_kind));
        for (i32 i = 0; i < 9; ++i) s_write_i32(stream, jo::self<jo::MsgLog>(this)->first_of_kind[i]);
        for (i32 i = 0; i < 9; ++i) s_write_i32(stream, jo::view<u32>(jo::self<jo::MsgLog>(this)->count_of_kind[i]));
        s_write_i32(stream, joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->size()));  // 0x00417ad0
        for (i32 i = 1; i <= joan::orig::as_dynarray(this)->size(); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at(joan::orig::arg::i32(i))) != 0) {  // 0x00486620
                s_write_i16(stream, 0);
            } else {
                s_write_i16(stream, 1);
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4865f0(joan::orig::arg::i32(i)));      // 0x004865f0
                if (joan::orig::arg::as<i32>(joan::orig::as_log_message(e)->save(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x00486420
            }
        }
        joan::orig::dynarray_b(this)->write_empty_room(joan::orig::arg::file(stream));  // 0x00585b40
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_03", name, addr, fn)
B("Obj::load_480880", 0x00480880, &H::load_480880);
B("List::load_480ce0", 0x00480ce0, &H::load_480ce0);
B("Obj::load_481140", 0x00481140, &H::load_481140);
B("Obj::save_481ba0", 0x00481ba0, &H::save_481ba0);
B("List::load_482f70", 0x00482f70, &H::load_482f70);
B("List::load_4831c0", 0x004831c0, &H::load_4831c0);
B("Obj::save_485070", 0x00485070, &H::save_485070);
B("List::load_4855b0", 0x004855b0, &H::load_4855b0);
B("Obj::load_485930", 0x00485930, &H::load_485930);
B("List::load_485f50", 0x00485f50, &H::load_485f50);
B("List::save_486110", 0x00486110, &H::save_486110);
#undef B

}  // namespace game_b101_03
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
//
//    - Unit::set_order_480ff0 0x00480ff0 (ret 8, 23 callers): entry 0x004e3fe0(a1); 0x0047b600
//      on 0xa57f80 (result unused); +0x11 = a2, +0x12 = [0xa47bb0]; 0x004b9e20(a1, 1) on 0xa50020
//      when side byte +0xe == [0xa57fae]; then 0x004b8d60(word +0, 0x1e) on
//      0x004b42e0(0x0047b600(byte +0xf), byte +0xe); 0x004e2a00 when +0x11 == 2. eax: entry or
//      the 0x004e2a00 result.
namespace {
namespace game_b108_02 {

std::uintptr_t bits(const void* p) { return reinterpret_cast<std::uintptr_t>(p); }

JOAN_COUNTER(c_set_order, "Unit::set_order_480ff0");

struct H {
    // 0x00480ff0
    std::uintptr_t set_order_480ff0(u32 a1, u8 a2) {
        c_set_order.hit();
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_talk(this)->get_msg(joan::orig::arg::i32(a1)));  // 0x004e3fe0
        (void)joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(static_cast<i32>(jo::as_treaty(p)->from));  // 0x0047b600 (unused)
        jo::view<u8>(jo::as_treaty(p)->reply) = a2;
        jo::as_treaty(p)->u32_12 = global<u32>(d::g_get_a47bb0);
        if (static_cast<i32>(jo::as_treaty(p)->from) == global<i32>(d::g_player_side))
            joan::orig::as_news_list(obj(d::g_tasks))->add_treaty(joan::orig::arg::u32(a1), 1u);  // 0x004b9e20
        const i32 word0 = jo::as_treaty(p)->kind;
        const i32 side = jo::as_treaty(p)->from;
        const i32 kind = jo::as_treaty(p)->to;
        void* const a = joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(joan::orig::arg::i32(kind));  // 0x0047b600
        void* const b = joan::orig::arg::as<void*>(joan::orig::as_player(a)->relation(joan::orig::arg::i32(side)));               // 0x004b42e0
        joan::orig::as_table(b)->raise_to(joan::orig::arg::i32(word0), 0x1e);                           // 0x004b8d60
        if (static_cast<i32>(jo::as_treaty(p)->reply) == 2) return joan::orig::arg::as<u32>(joan::orig::as_treaty(p)->apply());  // 0x004e2a00
        return bits(p);
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Unit::set_order_480ff0", 0x00480ff0, &H::set_order_480ff0);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b105_04.cpp (game/b105_04)
//
//    - empty thiscall stubs (never write eax; the C++ versions leave eax untouched as well):
//        ret 0: 0x00481360, 0x00481450, 0x00481690, 0x00481780, 0x00481870, 0x00481a50,
//               0x0048bf20, 0x0048bf40, 0x0048bf60, 0x0048bf80, 0x0048c800, 0x0048ec30,
//               0x0048ec80, 0x0048eca0, 0x004903b0 (vtable slots in 0x9d37f8..0x9d3da8),
//               0x00486400 (only caller 0x0048639f, eax overwritten with 1 afterwards),
//               0x004876b0 (only caller 0x00470f28 on 0xa3ce90, eax unused),
//               0x00481960, 0x00488370 (no references: dead code; the .text "hits" of 0x407883
//               found by findvt are misaligned instruction bytes, not references).
//        ret 4: 0x0048bfa0, 0x0048bfc0, 0x0048c850, 0x0048c870, 0x0048ecf0, 0x0048ed10
//               (slots 0x3c / 0x40 of the vtables 0x9d37cc / 0x9d388c / 0x9d397c / 0x9d3a18 /
//               0x9d3c88 / 0x9d3d78; every override in the hierarchy - 0x00490700, 0x00490750,
//               0x00491da0, 0x00491dc0, 0x00492480, 0x004924a0 - is empty too, so the virtual is
//               void and no caller reads eax. An -O0 build may load the unused argument into eax.)
//
//    - List::save_4838e0 0x004838e0 (file), ret 4 (only caller 0x00475bf7): writes the count
//      (0x00417ad0) and word +0x2e, then per item i = 1..count either a 0 word (0x00485900(i)
//      != 0) or a 1 word followed by 0x00483250(item, file) on 0x00485790(i); 0 on failure,
//      else 0x00585b40(this, file) and 1.
//
//    - Obj::save_4848a0 0x004848a0 (file), ret 4 (only caller 0x00473947; despite the name it is
//      the LOAD counterpart): frees the buffers +0x71c / +0x728 (0x0058ed70 on 0xa48838), reads
//      this (0x745 bytes, File::file_read 0x005873a0), +0x17 (0x00483f10) and the +0x309 entries
//      (0x004842c0, 0x1a bytes each, at +0x30b), then reallocates (0x0058e7b0) and reads the two
//      word arrays (File::file_read_array 0x00587650) into (+0x71c, count +0x720, capacity
//      +0x724 = count + 10) / (+0x728, +0x72c, +0x730). Returns 1 / 0.
//
//  - 0x004838e0: the original pushes the outer call's `file` before the inner 0x00485790(i) call
//    (MSVC interleaving; 0x00485790 is ret 4, 0x00483250 is ret 4). The word +0x2e is pushed as
//    `mov cx, word; push ecx` with garbage upper bits; 0x00587ba0 writes only the 2 low bytes of
//    its argument (WriteFile from &arg, 2 bytes), so it is passed zero-extended.
namespace {
namespace game_b105_04 {

const char* const kGfile3 = str(d::s_d_ww_gfile3_cpp);  // "d:\ww\gfile3.cpp"

JOAN_COUNTER(c_list_save, "List::save_4838e0");
JOAN_COUNTER(c_obj_save, "Obj::save_4848a0");

struct H {
    // ---------------------------------------------------------------- empty stubs
    void stub0() {}

    // ---------------------------------------------------------------- 0x004838e0
    i32 save_4838e0(void* file) {
        c_list_save.hit();
        const i32 n = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
        joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(n));            // 0x00587da0
        joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::self<jo::SaveList>(this)->current))));  // 0x00587ba0
        for (i32 i = 1; i <= joan::orig::as_dynarray(this)->size(); ++i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_empty_at(joan::orig::arg::i32(i))) != 0) {                // 0x00485900
                joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(0u));   // 0x00587ba0
            } else {
                joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(1u));  // 0x00587ba0
                void* const item = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_485790(joan::orig::arg::i32(i)));  // 0x00485790
                if (joan::orig::arg::as<i32>(joan::orig::as_profile(item)->save(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x00483250
            }
        }
        joan::orig::dynarray_b(this)->write_empty_room(joan::orig::arg::file(file));  // 0x00585b40
        return 1;
    }

    // ---------------------------------------------------------------- 0x004848a0
    i32 save_4848a0(void* file) {
        c_obj_save.hit();
        if (jo::self<jo::Game>(this)->word_set != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_9ffcd0)) + 3;
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::self<jo::Game>(this)->word_set), joan::orig::arg::str(kGfile3), joan::orig::arg::i32(line));  // 0x0058ed70
        }
        if (jo::self<jo::Game>(this)->word_stack != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_9ffcd0)) + 7;
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::self<jo::Game>(this)->word_stack), joan::orig::arg::str(kGfile3), joan::orig::arg::i32(line));  // 0x0058ed70
        }
        if (joan::save::fieldwise()) {  // [Debug] SaveFieldwise (SAVES_PLAN S4): save::get_Game, 0 = the raw read
            joan::save::In in(*joan::orig::as_file(file), joan::save::kGame, this);  // File::file_read 0x005873a0
            joan::save::get_Game(in, *jo::self<jo::Game>(this));
            if (in.end() == 0) return 0;
        } else if (joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(static_cast<void*>(this)), 0x745u) == 0) {  // 0x005873a0
            return 0;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_campaign_state(&jo::self<jo::Game>(this)->scenario)->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x00483f10
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Game>(this)->end_condition_count); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_end_condition(&jo::self<jo::Game>(this)->end_conditions[i])->load(joan::orig::arg::mptr(file))) == 0) {  // 0x004842c0
                return 0;
            }
        }
        jo::view<u32>(jo::self<jo::Game>(this)->word_set_cap) = jo::view<u32>(jo::self<jo::Game>(this)->word_set_count) + 0xa;
        {
            const i32 line = static_cast<i32>(global<i16>(d::g_9ffcd0)) + 0x1f;
            jo::self<jo::Game>(this)->word_set = static_cast<jo::ArrI16*>(joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Game>(this)->word_set_cap) << 1), joan::orig::arg::str(kGfile3), joan::orig::arg::i32(line)));  // 0x0058e7b0
        }
        joan::orig::as_file(file)->file_read_array(joan::orig::arg::mptr(jo::self<jo::Game>(this)->word_set), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Game>(this)->word_set_count)), 2u);  // 0x00587650
        jo::view<u32>(jo::self<jo::Game>(this)->word_stack_cap) = jo::view<u32>(jo::self<jo::Game>(this)->word_stack_count) + 0xa;
        {
            const i32 line = static_cast<i32>(global<i16>(d::g_9ffcd0)) + 0x23;
            jo::self<jo::Game>(this)->word_stack = static_cast<jo::ArrI16*>(joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Game>(this)->word_stack_cap) << 1), joan::orig::arg::str(kGfile3), joan::orig::arg::i32(line)));  // 0x0058e7b0
        }
        joan::orig::as_file(file)->file_read_array(joan::orig::arg::mptr(jo::self<jo::Game>(this)->word_stack), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Game>(this)->word_stack_count)), 2u);  // 0x00587650
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_04", name, addr, fn)
// empty stubs, ret 0
B("Obj::stub_481360", 0x00481360, &H::stub0);
B("Obj::stub_481450", 0x00481450, &H::stub0);
B("Obj::stub_481690", 0x00481690, &H::stub0);
B("Obj::stub_481780", 0x00481780, &H::stub0);
B("Obj::stub_481870", 0x00481870, &H::stub0);
B("Obj::stub_481960", 0x00481960, &H::stub0);
B("Obj::stub_481a50", 0x00481a50, &H::stub0);
B("Obj::stub_486400", 0x00486400, &H::stub0);
// save / load helpers
B("List::save_4838e0", 0x004838e0, &H::save_4838e0);
B("Obj::save_4848a0", 0x004848a0, &H::save_4848a0);
#undef B

}  // namespace game_b105_04
}  // namespace

// ---------------------------------------- from modules/game_b106_03.cpp (game/b106_03)
//
//    - GFile3::load_list 0x004820d0 (file), ret 4 (gfile3.cpp): reads a count (0x00587e20)
//      and a word kind per entry (0x00587c20); builds an object per kind with operator new
//      (0x007b1d35) + ctor, loads it through its vtable slot 0x74 (file) and appends it to
//      the list at +4 (0x005857a0). Kind 0 appends the first dword of a local placeholder
//      (0x00416a50 object, vtable 0x9d3d78) and marks it in a temporary word array (memory
//      manager 0xa48838, debug line [0x9ffcbc] + 3 / + 0xb6); marked entries are removed
//      afterwards (0x005858c0), the others are passed to 0x00425d20. Returns 1, or 0 when a
//      load fails (the local is destroyed, the array leaks, like the original).
//      SEH frame: only protects the ctors after operator new (which returns null, never
//      throws) - normal path kept.
namespace {
namespace game_b106_03 {

JOAN_COUNTER(c_load_4820d0, "GFile3::load_list_4820d0");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ---- 0x004820d0 ------------------------------------------------------------------------
    i32 load_list(void* file) {
        c_load_4820d0.hit();
        const i32 count = joan::orig::as_file(file)->file_get_long();   // 0x00587e20
        i16* const marks = joan::orig::arg::as<i16*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(count * 2), str(d::s_d_ww_gfile3_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9ffcbc)) + 3)));  // 0x0058e7b0
        alignas(alignof(void*)) u8 local[((sizeof(jo::MapObject) > 0xc4 ? sizeof(jo::MapObject) : 0xc4) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger MapObject
        joan::orig::as_map_object(local)->construct();  // 0x00416a50
        void* const list = &jo::as_units(this)->list;  // +4 on x86 (g_a460a0: a pointer at +0)
        for (i32 i = 1; i <= count; ++i) {
            if (i == 0xe) i = 0xe;
            const i16 kind = joan::orig::arg::as<i16>(joan::orig::as_file(file)->file_get_short());  // 0x00587c20
            if (kind == 0) {
                marks[i - 1] = 1;
                joan::orig::dynarray_b(list)->linkin(static_cast<void*>(local));     // 0x005857a0
                continue;
            }
            marks[i - 1] = 0;
            u32 size;
            std::uintptr_t ctor;
            switch (kind) {
                case 1: size = static_cast<u32>(sizeof(jo::MapObject)) + (0xdb - 0xc2), ctor = fn::code_value(fn::Order19_ctor); break;   // 0x0048c250 (0xdb on x86: a MapObject + 0x19)
                case 2: size = static_cast<u32>(sizeof(jo::Order0c_a)), ctor = fn::code_value(fn::Order0c_a_ctor); break;   // 0x00434380 (0xce on x86)
                case 3: size = static_cast<u32>(sizeof(jo::Order0c_c)), ctor = fn::code_value(fn::Order0c_c_ctor); break;   // 0x0048ea70 (0xce on x86)
                case 4: size = static_cast<u32>(sizeof(jo::Wall)), ctor = fn::code_value(fn::Order2d_ctor); break;   // 0x00432210 (0xef on x86: Order2d is Wall-sized)
                case 10: size = static_cast<u32>(sizeof(jo::Order0b)), ctor = fn::code_value(fn::Order0b_ctor); break;  // 0x00489ea0 (0xcd on x86)
                case 13: size = static_cast<u32>(sizeof(jo::Order45)), ctor = fn::code_value(fn::Order45_ctor); break; // 0x00434040 (0x107 on x86)
                case 14: size = static_cast<u32>(sizeof(jo::Order0c_b)), ctor = fn::code_value(fn::Order0c_b_ctor); break;  // 0x004352b0 (0xce on x86)
                case 18: size = static_cast<u32>(sizeof(jo::Order00)), ctor = fn::code_value(fn::Order00_ctor); break;  // 0x0048f740 (0xc2 on x86)
                default: size = static_cast<u32>(sizeof(jo::MapObject)), ctor = fn::code_value(fn::Obj_ctor_416a50); break;  // 0x00416a50 (0xc2 on x86)
            }
            void* const mem = joan::crt::operator_new(joan::orig::arg::u32(size));  // operator new
            void* item = mem != nullptr ? thiscall<void*>(ctor, mem) : nullptr;
            if (virtual_call<i32>(item, 0x74, file) == 0) {
                joan::orig::as_map_object(local)->destruct();  // 0x00415790
                return 0;
            }
            joan::orig::dynarray_b(list)->linkin(static_cast<void*>(&item));     // 0x005857a0
        }
        for (i32 i = 0; i < count; ++i) {
            if (marks[i] == 1) {
                joan::orig::dynarray_b(list)->linkout(joan::orig::arg::i32(i + 1));  // 0x005858c0
            } else {
                (void)joan::orig::as_units(this)->list.ptr_at(joan::orig::arg::i32(i + 1));  // 0x00425d20
            }
        }
        joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(static_cast<void*>(marks)), str(d::s_d_ww_gfile3_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9ffcbc)) + 0xb6));  // 0x0058ed70
        joan::orig::as_map_object(local)->destruct();  // 0x00415790
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_03", name, addr, fn)
B("GFile3::load_list_4820d0", 0x004820d0, &H::load_list);
#undef B

}  // namespace game_b106_03
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
//
//    - Slot::select 0x00482930 (id) ret 4 (gfile3.cpp): bool in al.
//
//  * 0x00482930 pushes eax with the word +0x3b in ax; 0x0042d0a0 reads it as a word only.
namespace {
namespace game_b86_06 {

using joan::orig::helpers::at;

void* const kUnitList = obj(d::g_units);
void* const kTypes = obj(d::g_types);
void* const kAnims = obj(d::g_physic_sys);

JOAN_COUNTER(c_select, "Slot::select");

struct H {
    // ---- 0x00482930 Slot::select(id) ret 4 (gfile3.cpp) ------------------------------------
    std::int8_t select(std::int32_t id) {
        c_select.hit();
        if (id == 0 && jo::self<jo::Unit>(this)->rider_id != 0) {
            jo::self<jo::Unit>(this)->rider_id = 0;
            if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
                void* const anim = joan::orig::as_players(kAnims)->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)));  // 0x006af310
                joan::orig::as_unit(this)->play_idle(0, 1);  // 0x00508b90
                std::uint8_t* const type = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::self<jo::Unit>(this)->type_id)));  // 0x0042d0a0
                jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode = jo::as_unit_type(type)->shape_kind;
                joan::orig::as_anim_table(kAnims)->restart_6af370(joan::orig::arg::u32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)), joan::orig::arg::u32(jo::as_shot(anim)->u32_27a)  /* the 0x006af310 Players list holds Body records */, 6u, 0u, 2u, 1u);  // 0x006af370
                joan::orig::as_body(anim)->set_2a2(joan::orig::arg::i16(0), 0, 0);  // 0x006b1d80
            }
            jo::self<jo::Unit>(this)->layer = 1;
            return 1;
        }
        if (joan::orig::as_dynarray(kUnitList)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
        if (jo::self<jo::Unit>(this)->rider_id != 0) return 0;
        if (jo::self<jo::Unit>(this)->actor == nullptr || jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) == 0) return 0;
        void* const anim = joan::orig::as_players(kAnims)->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)));  // 0x006af310
        jo::self<jo::Unit>(this)->rider_id = id;
        std::uint8_t* const type = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::self<jo::Unit>(this)->type_id)));  // 0x0042d0a0
        jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode = jo::as_unit_type(type)->shape_kind;
        joan::orig::as_anim_table(kAnims)->restart_6af370(joan::orig::arg::u32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)), joan::orig::arg::u32(jo::as_shot(anim)->u32_27a)  /* the 0x006af310 Players list holds Body records */, 7u, 0u, 1u, 1u);                            // 0x006af370
        joan::orig::as_body(anim)->set_2a2(joan::orig::arg::i16(1), 0, 0);   // 0x006b1d80
        joan::orig::as_unit(this)->play_idle(0, 1);      // 0x00508b90
        std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnitList)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        jo::self<jo::Unit>(this)->layer = jo::as_unit(unit)->layer;
        jo::self<jo::Unit>(this)->wait_count = jo::self<jo::Unit>(this)->wait_limit;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("Slot::select", 0x00482930, &H::select);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
//
//    - Obj::save_483e00 0x00483e00 (stream), ret 4 -> 0/1: 0x005872e0 (0x289 bytes), +0xc records of
//      0x1b2 at +0x10 (0x00483b90), +0x14 (0x00483b90), +0x287 (short) pointers at +0x283 (0x0047dbe0).
//
//    - Obj::save_484cc0 0x00484cc0 (stream), ret 4 -> 0/1: 0x005872e0 (0xe4 bytes).
//
//    - Obj::load_484d00 0x00484d00 (stream), ret 4 -> 0/1: 0x005873a0 (0xe4 bytes) + 0x00484d50.
namespace {
namespace game_b103_03 {

JOAN_COUNTER(c_save_483e00, "Obj::save_483e00");

struct H {
    // ------------------------------------------------------------------ 0x00483e00
    i32 save_483e00(void* stream) {
        c_save_483e00.hit();
        if (joan::save::fieldwise()) {  // [Debug] SaveFieldwise (SAVES_PLAN S4): save::put_Scenario, 0 = the raw write
            joan::save::Out o(*joan::orig::as_file(stream), joan::save::kScenario, this);
            joan::save::put_Scenario(o, *jo::self<jo::Scenario>(this));
            if (o.end() == 0) return 0;  // File::file_write 0x005872e0
        } else if (joan::orig::as_file(stream)->file_write(static_cast<void*>(this), 0x289u) == 0) {  // 0x005872e0
            return 0;
        }
        for (i32 i = 0; i < jo::self<jo::Scenario>(this)->entry_count; ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_scenario_entry(&jo::self<jo::Scenario>(this)->entries[i])->save(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x00483b90
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_scenario_entry(&jo::self<jo::Scenario>(this)->local)->save(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x00483b90
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Scenario>(this)->squad_count); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(jo::view<void**>(jo::self<jo::Scenario>(this)->squads)[i])->save_record(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x0047dbe0
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x00484cc0
    // Neither 0x00484cc0 nor 0x00484d00 is referenced in joan.exe (no call / address of them or of their thunks
    // 0x00401be5 / 0x00409692). [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S3): save::put_SceneNode /
    // get_SceneNode; 0 = the raw write / read.
    i32 save_484cc0(void* stream) {
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(stream), joan::save::kSceneNode, this);
            joan::save::put_SceneNode(o, *jo::self<jo::SceneS>(this));
            return o.end() == 0 ? 0 : 1;  // 0x005872e0
        }
        if (joan::orig::as_file(stream)->file_write(static_cast<void*>(this), 0xe4u) == 0) return 0;    // 0x005872e0
        return 1;
    }

    // ------------------------------------------------------------------ 0x00484d00
    i32 load_484d00(void* stream) {
        i32 ok;
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(stream), joan::save::kSceneNode, this);  // 0x005873a0
            joan::save::get_SceneNode(in, *jo::self<jo::SceneS>(this));
            ok = in.end();
        } else {
            ok = joan::orig::as_file(stream)->file_read(joan::orig::arg::mptr(static_cast<void*>(this)), 0xe4u);  // 0x005873a0
        }
        if (ok == 0) return 0;
        thiscall<u32>(fn::ObjG_reset, this);  // 0x00484d50
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Obj::save_483e00", 0x00483e00, &H::save_483e00);
B("Obj::save_484cc0", 0x00484cc0, &H::save_484cc0);
B("Obj::load_484d00", 0x00484d00, &H::load_484d00);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
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
namespace {
namespace game_b89_06 {

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00485900
    std::int32_t is_empty_at(std::int32_t i) {
        return joan::orig::as_dynarray(self())->ptr_at(joan::orig::arg::i32(i)) == nullptr ? 1 : 0;  // 0x0041b3f0
    }
    // 0x00485790
    void* list_at(std::int32_t i) { return joan::orig::as_dynarray(self())->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0
    // 0x00484540
    std::uint8_t* rec_at_28(std::int32_t w) {
        const std::int32_t i = static_cast<std::int16_t>(static_cast<std::uint16_t>(w));
        return reinterpret_cast<std::uint8_t*>(jo::self<jo::UScrRes>(this)->scripts + i - 1);  // 0x28-byte records on x86
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Rec::at_28", 0x00484540, &H::rec_at_28);
M("List::is_empty_at", 0x00485900, &H::is_empty_at);
M("List::at", 0x00485790, &H::list_at);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
//
//    - List::at_484570 0x00484570 (i), ret 4; List::at_4865f0 0x004865f0 (i), ret 4;
//      List::is_null_at 0x00486620 (i), ret 4 -> int.
namespace {
namespace game_b90_03 {

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00484570
    void* at_484570(std::int32_t i) { return joan::orig::as_dynarray(self())->get(joan::orig::arg::i32(i)); }  // 0x00417200
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("List::at_484570", 0x00484570, &H::at_484570);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
//
//    - Item::is_kind_1_or_4 0x004a4180, int_max 0x004893c0 (cdecl), Obj::set_62 0x00499fd0,
//      Obj::set_6a 0x0049adc0, Obj::set_6e 0x0049cac0, Obj::get_62 0x00499e20 (float),
//      gfile3.cpp list getters 0x004853e0 (+4) / 0x00485f20, wrappers 0x004ae6c0 / 0x004b9290.
namespace {
namespace game_b84_06 {

struct H {
    // ------------------------------------------------------------------ 0x004853e0
    void* list4_at(std::uint32_t i) { return joan::orig::as_dynarray(reinterpret_cast<std::uint8_t*>(this) + sizeof(void*))->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0 (the list after the leading vtable / pointer: +4 on x86; the Types facade's gap is 4 bytes)

    // ------------------------------------------------------------------ 0x00485f20
    void* list_at(std::uint32_t i) { return joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("List::at_plus4", 0x004853e0, &H::list4_at);
B("List::at", 0x00485f20, &H::list_at);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
//
//    - Units::get 0x00420240 / List::get 0x0056f320 (0x0041b3f0), List::empty_at 0x00561320 /
//      0x0056f3f0, Types::empty_at 0x00485410 (+4 list), Types::remove 0x004afaf0 (+4 list).
namespace {
namespace game_leaves82b {

void* list_get(void* list, std::int32_t i) { return joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0

struct H {
    std::int32_t empty_485410(std::int32_t i) { return list_get(reinterpret_cast<std::uint8_t*>(this) + sizeof(void*), i) == nullptr ? 1 : 0; }  // the list after the leading vtable (MgList::items): +4 on x86
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Types::empty_at", 0x00485410, &H::empty_485410);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

constexpr fn::Code kListGet = fn::List_get_41b3f0;    // 0x0041b3f0
void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

struct H {
    std::int32_t list_empty_at(std::int32_t i) { return list_get(this, i) == nullptr ? 1 : 0; }  // 0x0044cf80 / 0x00485760 / 0x0063e3f0
    void* list_get_wrap(std::int32_t i) { return list_get(this, i); }                   // 0x0043c450 / 0x00485730 / 0x0063e3c0
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("List::empty_at_f", 0x00485760, &H::list_empty_at);
W1("List::get_f", 0x00485730, &H::list_get_wrap);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b111_07.cpp (fix/uninit)
//
//    - GameFile::write_camera_4857c0 0x004857c0 (thiscall (file), ret 4, returns 0/1; caller
//      0x00475cef): writes the 0x126e-byte object, then a 0x28-byte copy (0x00423fe0) of the
//      camera selected by the camera-manager mode dword 0xa48cef (0xa48cc0 + 0x2f): 0 (or
//      byte +0xac == 0) -> camera 0, else 4 / 1 / 2 / 5. Original: mode 3 (the manager's
//      initial mode, set by 0x00562000.., accepted by the setter 0x00429860) has no case and
//      the uninitialised 0x28-byte stack buffer is written to the save file. Fix: the buffer
//      starts zeroed (no camera data for that mode). The loader (0x00485930) reads it back to
//      this + 0x4c, which no method of the object reads.
namespace {
namespace game_b111_07 {

JOAN_COUNTER(c_write_camera, "GameFile::write_camera");

struct H {
    // ------------------------------------------------------------------------------ 0x004857c0
    // Saved pointers (S7 save audit): in RTS mode +0x1261 of this hero-AI object holds the data image address of
    // 0x00a48f50 (stored by 0x0043f940 / 0x0043fc90) and goes to the file raw, like the TextureManager handle +0x1265.
    // Obj::load_485930 (game_b101_03.cpp) reads the 0x126e bytes back and immediately calls X::reset_state
    // 0x00485990, which zeroes +0x1261 / +0x1265 before any use (0x0043f940 sets them again when RTS mode starts), so
    // the loaded address is never dereferenced.
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S3): save::put_Player, which writes +0x1261 as joan.exe's
    // ORIGINAL address (D1, SAVES_PLAN 2.4; the raw path wrote the relocated one), and save::put_Camera; 0 = the raw
    // writes.
    u32 write_camera(void* file) {
        c_write_camera.hit();
        const bool fieldwise = joan::save::fieldwise();
        if (fieldwise) {
            alignas(4) u8 old[0x126e];  // SaveCompare: the raw bytes with D1 applied
            const void* const legacy = joan::save::kX86Layout && joan::save::hooks().compare() ? joan::save::player_legacy(old, this) : nullptr;
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kPlayer, legacy);
            joan::save::put_Player(o, *jo::self<jo::Player>(this));
            if (o.end() == 0) return 0;  // File::file_write 0x005872e0
        } else if (joan::orig::as_file(file)->file_write(static_cast<void*>(this), u32{0x126e}) == 0) {  // 0x005872e0
            return 0;
        }
        // fix/uninit: zeroed - the original writes uninitialised stack bytes for mode 3.
        alignas(4) u8 buf[0x28];
        std::memset(buf, 0, sizeof buf);
        auto copy = [&](u32 index) {
            void* const cam = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(joan::orig::arg::i32(index)));  // 0x00563c80
            joan::orig::as_camera_motion(cam)->end(joan::orig::arg::mptr(static_cast<void*>(buf)));               // 0x00423fe0
        };
        if (global<u32>(d::g_a48cef) == 0 || jo::view<i8>(jo::self<jo::Player>(this)->riding) == 0) {
            copy(0);
        } else if (global<u32>(d::g_a48cef) == 4) {
            copy(4);
        } else if (global<u32>(d::g_a48cef) == 1) {
            copy(1);
        } else if (global<u32>(d::g_a48cef) == 2) {
            copy(2);
        } else if (global<u32>(d::g_a48cef) == 5) {
            copy(5);
        }
        if (fieldwise) {
            jo::CamPose pose;
            static_assert(sizeof pose == sizeof buf);
            std::memcpy(&pose, buf, sizeof pose);
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kCamera, buf);
            joan::save::put_Camera(o, pose);
            o.end();  // File::file_write 0x005872e0 (result ignored)
        } else {
            joan::orig::as_file(file)->file_write(static_cast<void*>(buf), u32{0x28});  // 0x005872e0
        }
        return 1;
    }
};

// fix/uninit: 0x004857c0 writes a 0x28-byte uninitialised stack buffer to the save file for camera
// mode 3 -> zeroed buffer.
JOAN_REPLACE("fix/uninit", "GameFile::write_camera_4857c0", 0x004857c0, &H::write_camera);

}  // namespace game_b111_07
}  // namespace

// ---------------------------------------- from modules/game_b106_01.cpp (game/b106_01)
//
//    - Gfile3Rec::load_486280 0x00486280 (stream), ret 4 -> 1 / 0: 0x004b31c0, three ints
//      (+0x2e / +0x32 / +0x36), 9 ints at +0x3a and 9 at +0x5e (0x00587e20), then a count of
//      entries: word 0 -> 0x00584bd0(1); else new(0x1f) (0x007b1d35, not null-checked, like
//      the original), 0x004864b0(stream) (0 -> return 0), 0x005857a0(&entry). Finally
//      0x00585bb0(stream), 0x00486400.
namespace {
namespace game_b106_01 {

JOAN_COUNTER(c_load_486280, "Gfile3Rec::load_486280");

struct H {
    // ------------------------------------------------------------------ 0x00486280
    i32 load_486280(void* stream) {
        c_load_486280.hit();
        joan::orig::as_msg_log(this)->reset();                                // 0x004b31c0
        jo::self<jo::MsgLog>(this)->count = joan::orig::as_file(stream)->file_get_long();           // 0x00587e20
        jo::view<i32>(jo::self<jo::MsgLog>(this)->last) = joan::orig::as_file(stream)->file_get_long();
        jo::self<jo::MsgLog>(this)->cur_kind = joan::orig::as_file(stream)->file_get_long();
        for (i32 i = 0; i < 9; ++i) jo::view<i32>(jo::self<jo::MsgLog>(this)->first_of_kind[i]) = joan::orig::as_file(stream)->file_get_long();
        for (i32 i = 0; i < 9; ++i) jo::self<jo::MsgLog>(this)->count_of_kind[i] = joan::orig::as_file(stream)->file_get_long();
        const i32 count = joan::orig::as_file(stream)->file_get_long();
        for (i32 i = 1; i <= count; ++i) {
            const i16 kind = joan::orig::arg::as<i16>(joan::orig::as_file(stream)->file_get_short());  // 0x00587c20
            if (static_cast<i32>(kind) == 0) {
                joan::orig::dynarray(this)->add_blank(i32{1});  // 0x00584bd0
                continue;
            }
            void* entry = joan::crt::operator_new(static_cast<u32>(sizeof(jo::MsgEntry)));  // operator new (0x1f on x86)
            if (joan::orig::arg::as<i32>(joan::orig::as_log_message(entry)->load(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x004864b0
            joan::orig::dynarray_b(this)->linkin(&entry);                        // 0x005857a0
        }
        joan::orig::dynarray_b(this)->read_empty_room(joan::orig::arg::file(stream));  // 0x00585bb0
        joan::orig::as_msg_log(this)->on_loaded();          // 0x00486400
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_01", name, addr, fn)
B("Gfile3Rec::load_486280", 0x00486280, &H::load_486280);
#undef B

}  // namespace game_b106_01
}  // namespace

// ---------------------------------------- from modules/game_b104_02.cpp (game/b104_02)
//
//    - Obj::load_47d8f0 0x0047d8f0 (file), ret 4 -> 1 / 0.
//
//  - 0x00467590 / 0x0047d8f0 / 0x004839c0 / 0x00483f10: `new` + constructor under an EH state
//    that only frees the block if the constructor throws; normal path reproduced.
namespace {
namespace game_b104_02_2 {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::orig::helpers::op_new;

JOAN_COUNTER(c_load_47d8f0, "Obj::load_47d8f0");

struct H {
    // ------------------------------------------------------------------ file I/O
    u16 read_word(void* file) { return joan::orig::arg::as<u16>(joan::orig::as_file(file)->file_get_short()); }  // 0x00587c20
    // 0x0047d8f0 / 0x004839c0 child list
    i32 load_children(void* file, u32 size, std::uintptr_t ctor, std::uintptr_t load) {
        const i32 n = joan::orig::as_file(file)->file_get_long();   // 0x00587e20
        for (i32 i = 1; i <= n; ++i) {
            const u16 tag = read_word(file);
            if (static_cast<i16>(tag) == 0) {
                joan::orig::dynarray(this)->add_blank(1);  // 0x00584bd0
                continue;
            }
            void* const mem = op_new(size);
            void* child = mem != nullptr ? thiscall<void*>(ctor, mem) : nullptr;
            if (thiscall<i32>(load, child, file) == 0) return 0;
            joan::orig::dynarray_b(this)->linkin(&child);    // 0x005857a0
        }
        return 1;
    }
    i32 load_47d8f0(void* file) {
        c_load_47d8f0.hit();
        // .SAV step 13 (re/a6/SAVES_PLAN.md S1): already field-wise (get_short / get_long, Unit records via
        // Obj::load_47de30); the members typed (UnitManager +0x2e, +0x30, +0x38).
        jo::view<u16>(jo::self<jo::UnitManager>(this)->i16_2e) = read_word(file);
        for (i32 i = 0; i < 4; ++i) jo::view<u16>(jo::self<jo::UnitManager>(this)->counts_30[i]) = read_word(file);
        for (i32 i = 0; i < 4; ++i) jo::view<u16>(jo::self<jo::UnitManager>(this)->counts_38[i]) = read_word(file);
        if (load_children(file, static_cast<u32>(sizeof(jo::Unit)), fn::code_value(fn::Squad_Squad), fn::code_value(fn::Obj_load_47de30)) == 0) return 0;  // 0x004155e0, 0x0047de30 (0x4b4 on x86)
        joan::orig::dynarray_b(this)->read_empty_room(joan::orig::arg::file(file));  // 0x00585bb0
        joan::orig::as_units(this)->load_select_models();        // 0x0047dac0
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_02", name, addr, fn)
B("Obj::load_47d8f0", 0x0047d8f0, &H::load_47d8f0);
#undef B

}  // namespace game_b104_02_2
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
//
//      0x004865c0 Obj::clear_17_1b               (thiscall, ret 0)
namespace {
namespace game_b95_02_2 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x004865c0
    void* clear_17_1b() {
        jo::self<jo::MsgEntry>(this)->text = nullptr;
        jo::self<jo::MsgEntry>(this)->extra = nullptr;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Obj::clear_17_1b", 0x004865c0, &H::clear_17_1b);
#undef B95

}  // namespace game_b95_02_2
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
//
//      0x004800a0 Record::load(File*)                          thiscall, ret 4
namespace {
namespace game_b91_02 {

struct H {
    // 0x004800a0
    // Writes the Facility base +4..+0x78 (misnamed: Facility::save_base). [Debug] SaveFieldwise=1 (default,
    // re/a6/SAVES_PLAN.md S2): save::put_FacilityBase; 0 = the raw write.
    std::int32_t record_load(void* file) {
        std::int32_t ok;
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kFacilityBase, reinterpret_cast<std::uint8_t*>(this) + 4);
            joan::save::put_FacilityBase(o, *jo::self<jo::Facility>(this));
            ok = o.end();  // File::file_write 0x005872e0
        } else {
            ok = joan::orig::as_file(file)->file_write(static_cast<void*>(reinterpret_cast<std::uint8_t*>(this) + 4), 0x75u);  // 0x005872e0
        }
        if (ok == 0) return 0;
        if (jo::view<void*>(jo::self<jo::Facility>(this)->owner) != nullptr) joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(jo::view<std::uint16_t>(jo::self<jo::Facility>(this)->owner->index)));  // 0x00587ba0
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Record::load", 0x004800a0, &H::record_load);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
//
//    - Rec::write 0x00486420 (file), ret 4: 0x5872e0(this, 0x1f); then (+0x17, i16 +0x13 + 1)
//      and (+0x1b, i16 +0x15 + 1) when +0x15 > 0; 0 when the first write fails, else 1.
namespace {
namespace game_b100_04 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x00486420
    // Saved pointers (S7 save audit): the 0x1f raw bytes hold the heap text pointers +0x17 / +0x1b; the loader
    // (0x004864b0) zeroes both (0x004865c0) and allocates fresh buffers for the texts that follow. The message texts
    // are what SaveAudit sometimes reports as "image addresses" (e.g. " HP " reads as an address in .rdata).
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S4): save::put_MsgEntry; 0 = the raw write.
    u32 rec_write(void* file) {
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kMsgEntry, this);
            joan::save::put_MsgEntry(o, *jo::self<jo::MsgEntry>(this));
            if (o.end() == 0) return 0;  // File::file_write 0x005872e0
        } else if (joan::orig::as_file(file)->file_write(static_cast<void*>(this), 0x1fu) == 0) {  // 0x005872e0
            return 0;
        }
        joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(jo::self<jo::MsgEntry>(this)->text), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::MsgEntry>(this)->text_len) + 1));
        if (static_cast<i32>(jo::self<jo::MsgEntry>(this)->extra_len) > 0)
            joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(jo::self<jo::MsgEntry>(this)->extra), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::MsgEntry>(this)->extra_len) + 1));
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Rec::write", 0x00486420, &H::rec_write);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_b115_04.cpp (game/b115_04)
//
//  * module game/b115_04:
//    - Marker::line_of_sight 0x00486de0 (thiscall(camera), ret 4, al). NOT registered here: the
//      address is registered by game_fx.cpp (game/fx2, H::marker_line_of_sight), whose
//      [Fixes] GlowDepth=0 path used to run the original bytes through joan::hook::original; it now
//      calls joan::game_b115_04::marker_line_of_sight below. 0 when the light (this+8) is outside
//      the view (0x00615840, radius 0) or farther than 2500 from the camera position; otherwise the
//      segment light -> camera position - view axis * scale (camera frame +4 / +0x10.. +0x1c) is
//      tested against the scene nodes in its bounding box (0x005ef610 on world+0x6e): each node's
//      collision object +0x38 (0x005d4a80), then every child that is not type 0xb, not +0xf8, with
//      a bounding sphere of radius > 200 crossing the segment (0x0060e3f0): collision type 7 objects
//      test the segment in their local space by shape +0xb8 (0: blocked, 1 0x006a0f90, 2 0x00698970,
//      3 0x00699ec0), other collision objects through 0x005d4a80. Any hit -> 0, else 1.
//
// Notes:
//  - 0x00486de0: the segment {light, end} is one 24-byte block ([ebp-0x94], [ebp-0x88]): 0x005d4a80
//    copies 24 bytes from its second argument and 0x0060dfd0 / 0x0060e3f0 read both points. The hit
//    distance [ebp-0xd0] is an out value of 0x005d4a80 (read there only after it wrote it) and of
//    the three shape tests (write only); this function never reads it. It is zero-initialised here,
//    which changes nothing observable. The reversed copy of the segment at [ebp-0xac] is dead.
//    The `child != 0` re-test at 0x004870ff is always true (the loop condition).
namespace {
namespace game_b115_04 {

using joan::call::import_call;
struct V3 {
    float x, y, z;
};
struct Segment {  // [ebp-0x94] .. [ebp-0x7c] of 0x00486de0
    V3 p, q;
};
static_assert(sizeof(Segment) == 24);
struct Blob24 {  // shape parameters passed by value (child +0xcc)
    u32 d[6];
};
struct Blob20 {  // child +0xe4
    u32 d[5];
};

float cf(img::Addr address) { return global<float>(address); }

JOAN_COUNTER(c_los, "Marker::line_of_sight (b115_04)");

struct H {

    // 0x00486de0 (the [Fixes] GlowDepth=0 behaviour; called from game_fx.cpp)
    char line_of_sight(void* camera) {
        float* const light = jo::self<jo::Marker>(this)->pos;  // Marker::line_of_sight
        if (joan::orig::arg::as<i32>(joan::orig::as_camera(camera)->sphere_visible(joan::orig::arg::mptr(static_cast<void*>(light)), joan::orig::arg::f32(0u))) == 0) return 0;  // 0x00615840
        c_los.hit();
        alignas(4) jo::CamPose frame;  // [ebp-0x2c]: eye = position, target = yaw / pitch / distance, +0x1c scale
        joan::orig::as_camera(camera)->frame(joan::orig::arg::mptr(static_cast<void*>(&frame)));  // 0x006140a0
        V3 t0;
        const V3 diff = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&t0, static_cast<void*>(light), joan::orig::arg::ptr(frame.eye)));  // 0x0060cc90
        if (static_cast<float>(import_call<double>(d::iat_v3d_87_length, &diff)) > cf(d::flt_2500)) return 0;  // length > 2500
        void* const scene = jo::as_world(global<void*>(d::g_world))->scene;       // [ebp-0x30]

        float m[16];  // [ebp-0x70]
        joan::orig::orbit_matrix(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::f32(jo::view<u32>(frame.target[0])), joan::orig::arg::f32(jo::view<u32>(frame.target[1])), joan::orig::arg::f32(jo::view<u32>(frame.target[2])), joan::orig::arg::f32(0u));  // 0x0060bd00
        V3 axis;  // [ebp-0x7c]
        joan::orig::matrix_axis_z(&axis, static_cast<void*>(m));  // 0x0060c220
        Segment seg;  // [ebp-0x94]
        std::memcpy(&seg.p, light, sizeof(V3));
        const float scale = jo::view<float>(frame.u32_1c);
        const V3 cam_pos = *reinterpret_cast<const V3*>(frame.eye);
        float s;
        s = axis.x * scale;
        seg.q.x = cam_pos.x - s;
        s = axis.y * scale;
        seg.q.y = cam_pos.y - s;
        s = axis.z * scale;
        seg.q.z = cam_pos.z - s;

        alignas(4) u8 box[0x18];  // [ebp-0xc4]
        joan::orig::as_aabb(static_cast<void*>(box))->set_point(&seg.p);  // 0x0060d430
        joan::orig::as_aabb(static_cast<void*>(box))->extend(&seg.q);  // 0x0060d490
        i32 count = 0;                                                   // [ebp-0xc8]
        void** const nodes = joan::orig::arg::as<void**>(joan::orig::as_scene(scene)->nodes_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x005ef610
        float hit = 0.0f;  // [ebp-0xd0] (out value of the tests, never read here)
        for (i32 i = 0; i < count; ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(&seg, joan::orig::arg::ptr(jo::as_scene_s(nodes[i])->desc))) != 0 &&  // 0x0060dfd0
                jo::as_scene_s(nodes[i])->plane != nullptr &&
                joan::orig::arg::as<i32>(joan::orig::as_plane(jo::as_scene_s(nodes[i])->plane)->segment_hit(&hit, &seg)) != 0)  // 0x005d4a80
                return 0;
            for (void* child = joan::orig::arg::as<void*>(joan::orig::as_scene_s(nodes[i])->first_actor()); child != nullptr;  // 0x005effa0
                 child = jo::as_actor(child)->scene_prev) {
                if (jo::as_actor(child)->kind == 0xb) continue;
                if (jo::as_actor(child)->faded != 0) continue;
                void* const collision = jo::as_actor(child)->plane;  // [ebp-0xd4]
                if (collision != nullptr && static_cast<i32>(jo::as_plane(collision)->vertex_format) == 7) {
                    float sphere[4];  // [ebp-0xe4]
                    joan::orig::as_actor(child)->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(sphere)), 1u);  // 0x005acc80
                    if (!(sphere[3] > cf(d::flt_200))) continue;  // only radius > 200 is tested (NaN skips)
                    if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(&seg, joan::orig::arg::mptr(static_cast<void*>(sphere)))) == 0) continue;  // 0x0060e3f0
                    u8 blocked = 0;  // [ebp-0x108]
                    joan::orig::as_bone(jo::as_actor(child)->bone)->update();  // 0x0060ee60
                    void* const world = jo::as_actor(child)->bone->matrix_;
                    float inv[16];  // [ebp-0x14c]
                    import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), world);  // matrix_inverse
                    V3 a, b;                                                         // [ebp-0xf0], [ebp-0xfc]
                    import_call<void*>(d::iat_v3d_87_point_transform_3, &a, &seg.p, static_cast<void*>(inv));  // point_transform
                    import_call<void*>(d::iat_v3d_87_point_transform_3, &b, &seg.q, static_cast<void*>(inv));
                    const u32 shape = jo::as_actor(child)->aim_mode;
                    if (shape <= 3) {
                        if (shape == 0) return 0;
                        V3 t;
                        const V3 d = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&t, &b, &a));  // 0x0060cc90
                        if (shape == 1) {
                            blocked = static_cast<u8>(
                                joan::orig::arg::as<char>(joan::orig::shape::sphere_hit(&hit, a, d, joan::orig::arg::mptr(static_cast<void*>(jo::as_actor(child)->aim_sphere)))));  // 0x006a0f90
                        } else if (shape == 2) {
                            Blob24 p;
                            std::memcpy(&p, jo::as_actor(child)->aim_box, sizeof p);
                            blocked = static_cast<u8>(joan::orig::arg::as<char>(joan::orig::shape::box_hit(&hit, a, d, p)));  // 0x00698970
                        } else {
                            Blob20 p;
                            std::memcpy(&p, &jo::as_actor(child)->collide_kind, sizeof p);  // collide_kind .. collide_height
                            blocked = static_cast<u8>(joan::orig::arg::as<char>(joan::orig::shape::cylinder_hit(&hit, a, d, p)));  // 0x00699ec0
                        }
                    }
                    if (blocked != 0) return 0;
                    continue;
                }
                if (collision == nullptr) continue;
                float sphere[4];  // [ebp-0xe4]
                joan::orig::as_actor(child)->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(sphere)), 1u);  // 0x005acc80
                if (!(sphere[3] > cf(d::flt_200))) continue;  // only radius > 200 is tested (NaN skips)
                if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(&seg, joan::orig::arg::mptr(static_cast<void*>(sphere)))) == 0) continue;  // 0x0060e3f0
                if (joan::orig::arg::as<i32>(joan::orig::as_plane(jo::as_actor(child)->plane)->segment_hit(&hit, &seg)) != 0) return 0;  // 0x005d4a80
            }
        }
        return 1;
    }
};

}  // namespace game_b115_04
}  // namespace

namespace joan::game_b115_04 {
// 0x00486de0 Marker::line_of_sight, the original behaviour (game_fx.cpp, [Fixes] GlowDepth=0).
char marker_line_of_sight(void* marker, void* camera) { return static_cast<::game_b115_04::H*>(marker)->line_of_sight(camera); }
}  // namespace joan::game_b115_04

// ---------------------------------------- from modules/game_b103_02_2.cpp (game/b103_02)
namespace {
namespace game_b103_02_2 {

using i32 = std::int32_t;

using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i16 = std::int16_t;

JOAN_COUNTER(c_save_47d7a0, "Obj::save_47d7a0");

i32 file_write_word(void* file, u32 w) { return joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(w)); }  // 0x00587ba0
i32 file_write_int(void* file, i32 v) { return joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(v)); }  // 0x00587da0

struct H {
    // ------------------------------------------------------------------ 0x0047d7a0
    i32 save_47d7a0(void* file) {
        c_save_47d7a0.hit();
        // .SAV step 13 (re/a6/SAVES_PLAN.md S1): already field-wise (put_short / put_long); the Unit records go through
        // Obj::save_47dbe0 (save::put_Unit), the empty-room tail through DynArrayB::write_empty_room (S5).
        file_write_word(file, jo::view<u16>(jo::self<jo::UnitManager>(this)->i16_2e));
        for (i32 i = 0; i < 4; ++i) file_write_word(file, jo::view<u16>(jo::self<jo::UnitManager>(this)->counts_30[i]));
        for (i32 i = 0; i < 4; ++i) file_write_word(file, jo::view<u16>(jo::self<jo::UnitManager>(this)->counts_38[i]));
        file_write_int(file, joan::orig::as_dynarray(this)->size());  // 0x00417ad0
        for (i32 i = 1; i <= joan::orig::as_dynarray(this)->size(); ++i) {
            if (joan::orig::as_dynarray(this)->empty_at(joan::orig::arg::i32(i)) != 0) {  // 0x00428b70
                file_write_word(file, 0u);
            } else {
                file_write_word(file, 1u);
                void* const e = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3c0
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(e)->save_record(joan::orig::arg::mptr(file))) == 0) return 0;      // 0x0047dbe0
            }
        }
        joan::orig::dynarray_b(this)->write_empty_room(joan::orig::arg::file(file));  // 0x00585b40
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_02", name, addr, fn)
B("Obj::save_47d7a0", 0x0047d7a0, &H::save_47d7a0);
#undef B

}  // namespace game_b103_02_2
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02_2 {

using joan::img::str;

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::img::obj;

using i16 = std::int16_t;

JOAN_COUNTER(c_select_models, "Obj::load_select_models");

struct H {
    // ------------------------------------------------------------------ 0x0047dac0
    std::uintptr_t load_select_models() {
        c_select_models.hit();
        void* const models = obj(d::g_model_res_mgr);
        jo::self<jo::UnitManager>(this)->select_models[0] = joan::orig::arg::as<i32>(joan::orig::as_model_res(models)->add(str(d::s_MODEL_9d3750), str(d::s_select), 0));  // 0x004b18d0 "select"
        jo::self<jo::UnitManager>(this)->select_models[1] = joan::orig::arg::as<i32>(joan::orig::as_model_res(models)->add(str(d::s_MODEL_9d3750), str(d::s_select2), 0));  // "select2"
        jo::self<jo::UnitManager>(this)->select_models[2] = joan::orig::arg::as<i32>(joan::orig::as_model_res(models)->add(str(d::s_MODEL_9d3750), str(d::s_select3), 0));  // "select3"
        jo::self<jo::UnitManager>(this)->select_models[3] = joan::orig::arg::as<i32>(joan::orig::as_model_res(models)->add(str(d::s_MODEL_9d3750), str(d::s_select4), 0));  // "select4"
        // 0x004b0ec0 reads its first arg as a word (movsx).
        joan::orig::as_model_res_mgr(models)->set_params(joan::orig::arg::i16(static_cast<i32>(reinterpret_cast<const i16&>(jo::self<jo::UnitManager>(this)->select_models[0]))), static_cast<std::int8_t>(joan::orig::arg::i32(0)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i16(0));  // 0x004b0ec0
        joan::orig::as_model_res_mgr(models)->set_params(joan::orig::arg::i16(static_cast<i32>(reinterpret_cast<const i16&>(jo::self<jo::UnitManager>(this)->select_models[1]))), static_cast<std::int8_t>(joan::orig::arg::i32(0)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i16(0));
        joan::orig::as_model_res_mgr(models)->set_params(joan::orig::arg::i16(static_cast<i32>(reinterpret_cast<const i16&>(jo::self<jo::UnitManager>(this)->select_models[2]))), static_cast<std::int8_t>(joan::orig::arg::i32(0)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i16(0));
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_model_res_mgr(models)->set_params(joan::orig::arg::i16(static_cast<i32>(reinterpret_cast<const i16&>(jo::self<jo::UnitManager>(this)->select_models[3]))), static_cast<std::int8_t>(joan::orig::arg::i32(0)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i16(0)));
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Obj::load_select_models", 0x0047dac0, &H::load_select_models);
#undef B98

}  // namespace game_b98_02_2
}  // namespace

// ---------------------------------------- from modules/game_b102_02_2.cpp (game/b102_02)
namespace {
namespace game_b102_02_2 {

using i32 = std::int32_t;

using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using u8 = std::uint8_t;

using joan::img::obj;

using i16 = std::int16_t;

JOAN_COUNTER(c_save_47dbe0, "Obj::save_47dbe0");

i32 file_write(void* f, const void* p, u32 n) { return joan::orig::as_file(f)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }  // 0x005872e0
i32 file_write_word(void* f, u32 w) { return joan::orig::as_file(f)->file_put_short(joan::orig::arg::i16(w)); }  // 0x00587ba0
i32 file_write_dword(void* f, u32 v) { return joan::orig::as_file(f)->file_put_long(joan::orig::arg::i32(v)); }  // 0x00587da0

struct H {
    // 0x0047dbe0
    // Saved pointers (S7 save audit): the 0x432 raw bytes hold pointers (+0x33, +0x1c1, +0x2a1, +0x335, +0x378,
    // +0x37c and the links that Unit::reset_links 0x0047e300 clears). +0x335 (group) can be a data image address
    // (Game::init_sample_world 0x004797a0 passes 0x00a39670 to Unit::set_group); here only the id word of the
    // pointee is written and Obj::load_47de30 (game_b103_02.cpp) replaces +0x335 by a lookup of that id. The other
    // pointers are heap pointers that load_47de30 re-creates, re-allocates or clears; none is used as loaded. The 0x24-byte point
    // lists +0x432 / +0x473 convert their image addresses (Obj::write_list, game_b95_02.cpp).
    i32 save_47dbe0(void* file) {
        c_save_47dbe0.hit();
        {  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): save::put_Unit; 0 = the raw write
            i32 ok;
            if (joan::save::fieldwise()) {
                joan::save::Out o(*joan::orig::as_file(file), joan::save::kUnit, this);
                joan::save::put_Unit(o, *jo::self<jo::Unit>(this));
                ok = o.end();  // File::file_write 0x005872e0
            } else {
                ok = file_write(file, this, 0x432);
            }
            if (ok == 0) return 0;
        }
        if (jo::self<jo::Unit>(this)->group != nullptr && joan::orig::arg::as<i32>(joan::orig::as_group(jo::self<jo::Unit>(this)->group)->save(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047e540
        if (joan::orig::arg::as<i32>(joan::orig::as_unit_stats(&jo::self<jo::Unit>(this)->stats_block)->save(joan::orig::arg::mptr(file))) == 0) return 0;          // 0x0047e5c0 (+0x16f)
        if (joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->save(joan::orig::arg::mptr(file))) == 0) return 0;          // 0x0047e640 (+0x1aa)
        if (jo::self<jo::Unit>(this)->modifiers != nullptr && joan::orig::arg::as<i32>(joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->save(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047e850
        if (jo::self<jo::Unit>(this)->orders != nullptr && joan::orig::arg::as<i32>(joan::orig::as_list(jo::self<jo::Unit>(this)->orders)->save_21(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047f080
        for (i32 i = 0;; ++i) {
            u8* const d = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id))));  // 0x0042d0a0
            if (i >= jo::as_unit_type(d)->slot_count) break;
            file_write_dword(file, jo::view<u32>(jo::self<jo::Unit>(this)->mask_slots[i].value));
        }
        for (i32 i = 0;; ++i) {
            u8* const d = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id))));
            if (i >= jo::as_unit_type(d)->slot_count) break;
            file_write_dword(file, jo::view<u32>(jo::self<jo::Unit>(this)->mask_slots_b[i].value));
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_point_list(&jo::self<jo::Unit>(this)->path)->save(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047f1e0 (+0x432)
        if (joan::orig::arg::as<i32>(joan::orig::as_point_list(&jo::self<jo::Unit>(this)->path_b)->save(joan::orig::arg::mptr(file))) == 0) return 0;  // (+0x473)
        if (jo::self<jo::Unit>(this)->world != nullptr && file_write_word(file, jo::view<u16>(jo::self<jo::Unit>(this)->world->world_id)) == 0) return 0;  // World +2
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("Obj::save_47dbe0", 0x0047dbe0, &H::save_47dbe0);
#undef B

}  // namespace game_b102_02_2
}  // namespace

// ---------------------------------------- from modules/game_b90_03_2.cpp (game/b90_03)
namespace {
namespace game_b90_03_2 {

using i32 = std::int32_t;

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x004865f0
    void* at_4865f0(std::int32_t i) { return joan::orig::as_dynarray(self())->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0

    // 0x00486620
    std::int32_t is_null_at(std::int32_t i) {
        return joan::orig::as_dynarray(self())->ptr_at(joan::orig::arg::i32(i)) == nullptr ? 1 : 0;  // 0x0041b3f0
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("List::at_4865f0", 0x004865f0, &H::at_4865f0);
M("List::is_null_at", 0x00486620, &H::is_null_at);
#undef M

}  // namespace game_b90_03_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Units::load_select_models() -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b98_02_2::H::load_select_models>(this)); }  // 0x47dac0
auto joan::orig::Unit::save_record(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b102_02_2::H::save_47dbe0>(this, a0)); }  // 0x47dbe0
auto joan::orig::Unit::load_record(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_02::H::load_47de30>(this, a0)); }  // 0x47de30
auto joan::orig::SaveEntry::set(std::int32_t a0, std::uint32_t a1, std::uint16_t a2, std::uint32_t a3, std::uint16_t a4, std::uint8_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint8_t a10, std::uint32_t a11, std::uint32_t a12, std::uint8_t a13, void* a14, std::uint16_t a15) -> void { joan::bind::member<&::game_b85_01::H::entry_set>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15); }  // 0x47e200
auto joan::orig::Unit::reset_links() -> void { joan::bind::member<&::game_b92_02::H::reset_links>(this); }  // 0x47e300
auto joan::orig::Group::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_02::H::write_58>(this, a0)); }  // 0x47e540
auto joan::orig::Sub::load_58(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_02::H::load_58>(this, a0)); }  // 0x47e580
auto joan::orig::UnitStats::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_01::H::read_3b>(this, a0)); }  // 0x47e5c0
auto joan::orig::UnitStats::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_01::H::write_3b>(this, a0)); }  // 0x47e600
auto joan::orig::UpgradeList::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_02::H::idlist_write>(this, a0)); }  // 0x47e640
auto joan::orig::UpgradeList::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_02::H::idlist_read>(this, a0)); }  // 0x47e700
auto joan::orig::UnitData::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_02::H::save_47e850>(this, a0)); }  // 0x47e850
auto joan::orig::Sub::load_parts(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_02::H::load_parts>(this, a0)); }  // 0x47e9f0
auto joan::orig::ItemStack::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_01::H::read_12>(this, a0)); }  // 0x47ed60
auto joan::orig::ItemStack::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_01::H::write_12>(this, a0)); }  // 0x47eda0
auto joan::orig::Grid::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::grid_save>(this, a0)); }  // 0x47ede0
auto joan::orig::Grid::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_02::H::grid_load>(this, a0)); }  // 0x47ef30
auto joan::orig::List::save_21(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_02::H::save_21>(this, a0)); }  // 0x47f080
auto joan::orig::Order::load_entries(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::list21_load>(this, a0)); }  // 0x47f130
auto joan::orig::PointList::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_02::H::write_list>(this, a0)); }  // 0x47f1e0
auto joan::orig::PointList::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_02::H::pl_load>(this, a0)); }  // 0x47f290
auto joan::orig::List::on_loaded() -> void { joan::bind::member<&::game_b105_03::H::stub0>(this); }  // 0x47f560
auto joan::orig::Children::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b105_03::H::true_ret4>(this, a0)); }  // 0x47f580
auto joan::orig::Children::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b105_03::H::true_ret4>(this, a0)); }  // 0x47f5a0
auto joan::orig::Relation::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_02::H::read_f4>(this, a0)); }  // 0x47f5e0
auto joan::orig::Sub::load_f4(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_02::H::load_f4>(this, a0)); }  // 0x47f620
auto joan::orig::Sub::save_1545(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_02::H::save_1545>(this, a0)); }  // 0x47f660
auto joan::orig::Nation::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::campaign_load>(this, a0)); }  // 0x47f800
auto joan::orig::Place::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_02::H::save_c>(this, a0)); }  // 0x47fc80
auto joan::orig::Unit::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::unit_load>(this, a0)); }  // 0x47fd00
auto joan::orig::Place::clear_pointers() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::clear_1d6>(this)); }  // 0x47fdf0
auto joan::orig::Facility::save_base(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::record_load>(this, a0)); }  // 0x4800a0
auto joan::orig::Sub::load_75(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_02::H::load_75>(this, a0)); }  // 0x480110
auto joan::orig::Facility::clear_pointers() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_02::H::clear_71_75>(this)); }  // 0x4801d0
auto joan::orig::Unit::set_order_480ff0(std::uint32_t a0, std::uint8_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_02::H::set_order_480ff0>(this, a0, a1)); }  // 0x480ff0
auto joan::orig::WorldItem::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::obj40_save>(this, a0)); }  // 0x4810b0
auto joan::orig::WorldItem::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_03::H::load_481140>(this, a0)); }  // 0x481140
auto joan::orig::WorldItem::clear_links() -> void { joan::bind::member<&::game_b100_03::H::clear_3a_4e>(this); }  // 0x481220
auto joan::orig::MapObject::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_03::H::save_481ba0>(this, a0)); }  // 0x481ba0
auto joan::orig::MapObject::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b102_02::H::load_481cd0>(this, a0)); }  // 0x481cd0
auto joan::orig::MapObject::reset_links() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_02::H::reset_d>(this)); }  // 0x481ed0
auto joan::orig::Slot::select(std::int32_t a0) -> std::int8_t { return joan::bind::conv<std::int8_t>(joan::bind::member<&::game_b86_06::H::select>(this, a0)); }  // 0x482930
auto joan::orig::Order::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_02::H::write_6c>(this, a0)); }  // 0x482dc0
auto joan::orig::Order::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::rec6c_load>(this, a0)); }  // 0x482e00
auto joan::orig::Order::clear_pointers() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_02::H::clear_482e50>(this)); }  // 0x482e50
auto joan::orig::NewsItem::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_02::H::save_e>(this, a0)); }  // 0x4830b0
auto joan::orig::NewsItem::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::rec52_load>(this, a0)); }  // 0x4830f0
auto joan::orig::Profile::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_02::H::save_483250>(this, a0)); }  // 0x483250
auto joan::orig::Profile::load_483430(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_02::H::load_483430>(this, a0)); }  // 0x483430
auto joan::orig::ScenarioEntry::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::rec1b2_save>(this, a0)); }  // 0x483b90
auto joan::orig::ScenarioEntry::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_02::H::read_1b2>(this, a0)); }  // 0x483ca0
auto joan::orig::ScenarioEntry::clear_186() -> void { joan::bind::member<&::game_b94_02::H::clear_186>(this); }  // 0x483dd0
auto joan::orig::CampaignState::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_03::H::save_483e00>(this, a0)); }  // 0x483e00
auto joan::orig::CampaignState::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_02::H::load_483f10>(this, a0)); }  // 0x483f10
auto joan::orig::CampaignState::clear_pointers() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::clear_277>(this)); }  // 0x484160
auto joan::orig::Objective::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_02::H::read_strings>(this, a0)); }  // 0x4841d0
auto joan::orig::EndCondition::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_02::H::load_f>(this, a0)); }  // 0x4842c0
auto joan::orig::UScrRes::entry(std::int32_t a0) -> std::uint8_t* { return joan::bind::conv<std::uint8_t*>(joan::bind::member<&::game_b89_06::H::rec_at_28>(this, a0)); }  // 0x484540
auto joan::orig::List::at_484570(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_03::H::at_484570>(this, a0)); }  // 0x484570
auto joan::orig::List::get_at_4(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_01::H::get_at_4>(this, a0)); }  // 0x484710
auto joan::orig::List::is_null_at_4(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b85_01::H::is_null_at_4>(this, a0)); }  // 0x484740
auto joan::orig::List::is_null(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b85_01::H::is_null>(this, a0)); }  // 0x484870
auto joan::orig::Scene::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::recd5_save>(this, a0)); }  // 0x484ad0
auto joan::orig::Sub::load_d5(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_02::H::load_d5>(this, a0)); }  // 0x484b10
auto joan::orig::Scene::clear_pointers() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_02::H::clear_484b60>(this)); }  // 0x484b60
auto joan::orig::WorldCell::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::recac_save>(this, a0)); }  // 0x484e00
auto joan::orig::Sub::load_ac(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_02::H::load_ac>(this, a0)); }  // 0x484e40
auto joan::orig::WorldCell::clear_pointers() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_02::H::clear>(this)); }  // 0x484f50
auto joan::orig::World::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_03::H::save_485070>(this, a0)); }  // 0x485070
auto joan::orig::World::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::rec92_load>(this, a0)); }  // 0x485130
auto joan::orig::List::at_plus4(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_06::H::list4_at>(this, a0)); }  // 0x4853e0
auto joan::orig::Types::empty_at(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves82b::H::empty_485410>(this, a0)); }  // 0x485410
auto joan::orig::World::clear_pointers() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::clear_6a>(this)); }  // 0x485450
auto joan::orig::List::get_f(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_world::H::list_get_wrap>(this, a0)); }  // 0x485730
auto joan::orig::List::empty_at_f(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_world::H::list_empty_at>(this, a0)); }  // 0x485760
auto joan::orig::List::at_485790(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::list_at>(this, a0)); }  // 0x485790
auto joan::orig::List::is_empty_at(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b89_06::H::is_empty_at>(this, a0)); }  // 0x485900
auto joan::orig::Player::reset_state() -> void { joan::bind::member<&::game_b100_03::H::reset_state>(this); }  // 0x485990
auto joan::orig::List::at_485f20(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_06::H::list_at>(this, a0)); }  // 0x485f20
auto joan::orig::Sub::save_43(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_02::H::save_43>(this, a0)); }  // 0x486090
auto joan::orig::SaveEntry::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_03::H::rec43_load>(this, a0)); }  // 0x4860d0
auto joan::orig::MsgLog::on_loaded() -> void { joan::bind::member<&::game_b105_04::H::stub0>(this); }  // 0x486400
auto joan::orig::LogMessage::save(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_04::H::rec_write>(this, a0)); }  // 0x486420
auto joan::orig::LogMessage::load(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_02::H::load_i>(this, a0)); }  // 0x4864b0
auto joan::orig::LogMessage::clear_strings() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_02_2::H::clear_17_1b>(this)); }  // 0x4865c0
auto joan::orig::List::at_4865f0(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_03_2::H::at_4865f0>(this, a0)); }  // 0x4865f0
auto joan::orig::List::is_null_at(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_03_2::H::is_null_at>(this, a0)); }  // 0x486620
