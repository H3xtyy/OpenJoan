// game/unknown_unitdata.cpp: functions whose original source file is unknown (formerly game/_unplaced/unitdata.cpp;
// re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their original file when evidence names
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
#include <cmath>

#include "../lib/crt/crt.hpp"
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

using joan::call::cdecl_call;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::game_new;  // operator new

using joan::call::thiscall;

using joan::img::obj;

using i16 = std::int16_t;

using joan::orig::helpers::game_delete;                                    // operator delete

using u16 = std::uint16_t;

using i8 = std::int8_t;
using u8 = std::uint8_t;

using joan::hook::global;

// UnitData's item pointers hands[0], hands[1], equipped, extra_item (contiguous members) as the original's table:
// held(p, list * 2 + slot - 2) is the pointer at this + list * 8 - 6 + slot * 4 on x86.
static_assert(offsetof(jo::UnitData, extra_item) == offsetof(jo::UnitData, hands) + 3 * sizeof(jo::HeldItem*), "held");
jo::HeldItem*& held(const void* p, i32 i) { return (&jo::self<jo::UnitData>(p)->hands[0])[i]; }
// One of UnitData's three item grids (list_12, grid_16, grid_1a).
using GridField = jo::Grid* jo::UnitData::*;

}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

using joan::orig::helpers::memset_;

JOAN_COUNTER(c_new_record, "Owner::new_record");

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x004973a0
    void* new_record(std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5,
                     std::uint32_t a6) {
        c_new_record.hit();
        jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item) = game_new(static_cast<std::uint32_t>(sizeof(jo::HeldItem)));  // 0x12 on x86
        jo::self<jo::UnitData>(this)->extra_item->item = static_cast<std::uint16_t>(a1);
        jo::self<jo::UnitData>(this)->extra_item->count = static_cast<std::uint16_t>(a3);
        jo::view<std::uint32_t>(jo::self<jo::UnitData>(this)->extra_item->uses) = a4;
        jo::view<std::uint32_t>(jo::self<jo::UnitData>(this)->extra_item->max_uses) = a5;
        jo::self<jo::UnitData>(this)->extra_item->u32_e = a6;
        jo::self<jo::UnitData>(this)->extra_item->u16_4 = static_cast<std::uint16_t>(a2);
        return jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item);
    }

    void* zero_1e() {  // 0x00495950
        memset_(self(), 0, static_cast<std::uint32_t>(sizeof(jo::UnitData)));  // 0x1e on x86
        return self();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Owner::new_record", 0x004973a0, &H::new_record);
M("Obj::zero_1e", 0x00495950, &H::zero_1e);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
namespace {
namespace game_b92_02 {

struct H {

    // 0x00495980
    std::uintptr_t dtor_body() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_unit_data(this)->release()); }  // 0x00495ce0

    // 0x004971e0
    void slots_free(std::int32_t i, std::int32_t j) {
        const std::int32_t si = static_cast<std::int16_t>(i);
        if (si > 1) return;
        const std::int32_t sj = static_cast<std::int16_t>(j);
        jo::HeldItem*& slot = held(this, si * 2 + sj - 2);
        if (slot == nullptr) return;
        joan::crt::operator_delete(joan::orig::arg::mptr(slot));  // operator delete
        slot = nullptr;
    }

    // 0x00497440
    void free_e() {
        if (jo::self<jo::UnitData>(this)->extra_item == nullptr) return;
        joan::crt::operator_delete(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item)));  // operator delete
        jo::self<jo::UnitData>(this)->extra_item = nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("UnitData::dtor_body", 0x00495980, &H::dtor_body);
B("Slots::free", 0x004971e0, &H::slots_free);
B("Obj::free_e", 0x00497440, &H::free_e);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b89_05.cpp (game/b89_05)
namespace {
namespace game_b89_05 {

using joan::orig::helpers::s16;

// new Channel (0x3e bytes, ctor 0x004571e0); null when the allocation failed.
void* new_channel() {
    void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Grid)));  // 0x3e on x86
    if (mem == nullptr) return nullptr;
    return joan::orig::arg::as<void*>(joan::orig::as_grid(mem)->Grid_4571e0());  // 0x004571e0
}
void channel_setup(void* ch, std::int32_t a, std::int32_t b, std::int32_t c) {
    joan::orig::as_grid(ch)->alloc(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c));  // 0x00457290
}

void* const kItems = obj(d::g_item_res);  // 0x00425ce0

JOAN_COUNTER(c_create_missing, "Sounds::create_missing");
JOAN_COUNTER(c_create, "Sounds::create");
JOAN_COUNTER(c_names_set, "Names::set");
JOAN_COUNTER(c_set_item, "Names::set_item");

struct H {
    // ------------------------------------------------------------ 0x00495b40
    void create_missing() {
        c_create_missing.hit();
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16) == nullptr) {
            jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16) = new_channel();
            channel_setup(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16), 1, 6, 8);
        }
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a) == nullptr) {
            jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a) = new_channel();
            channel_setup(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a), 1, 6, 4);
        }
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->list_12) == nullptr) {
            jo::view<void*>(jo::self<jo::UnitData>(this)->list_12) = new_channel();
            channel_setup(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12), 1, 4, 1);
        }
    }

    // ------------------------------------------------------------ 0x004959b0 (ret 4)
    void create(std::uint32_t on) {
        c_create.hit();
        if (static_cast<std::int8_t>(on & 0xffu) == 0) return;
        jo::view<void*>(jo::self<jo::UnitData>(this)->list_12) = new_channel();
        channel_setup(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12), 1, 4, 1);
        jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16) = new_channel();
        channel_setup(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16), 1, 6, 8);
        jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a) = new_channel();
        channel_setup(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a), 1, 6, 4);
    }

    // ------------------------------------------------------------ 0x004970c0 (ret 0x20)
    std::int32_t names_set(std::uint32_t list, std::uint32_t slot, std::uint32_t id, std::uint32_t w4,
                           std::uint32_t w2, std::uint32_t d6, std::uint32_t da, std::uint32_t de) {
        c_names_set.hit();
        if (s16(list) > 1) return 1;
        void* const rec = game_new(static_cast<std::uint32_t>(sizeof(jo::HeldItem)));  // 0x12 on x86
        const std::int32_t index = s16(list) * 2 - 2 + s16(slot);
        jo::view<void*>(held(this, index)) = rec;
        held(this, index)->item = static_cast<std::uint16_t>(id);
        held(this, index)->count = static_cast<std::uint16_t>(w2);
        jo::view<std::uint32_t>(held(this, index)->uses) = d6;
        jo::view<std::uint32_t>(held(this, index)->max_uses) = da;
        held(this, index)->u32_e = de;
        held(this, index)->u16_4 = static_cast<std::uint16_t>(w4);
        return 1;
    }

    // ------------------------------------------------------------ 0x00497270 (ret 0x18)
    std::int32_t set_item(std::uint32_t id, std::uint32_t w4, std::uint32_t w2, std::uint32_t d6, std::uint32_t da,
                          std::uint32_t de) {
        c_set_item.hit();
        const std::int16_t item = static_cast<std::int16_t>(id);
        if (s16(jo::as_item_def(joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kItems)->get(joan::orig::arg::i16(item))))->type_id) != 0xb &&  // 0x00425ce0
            s16(jo::as_item_def(joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kItems)->get(joan::orig::arg::i16(item))))->type_id) != 0xc) {
            return 0;
        }
        jo::view<void*>(jo::self<jo::UnitData>(this)->equipped) = game_new(static_cast<std::uint32_t>(sizeof(jo::HeldItem)));  // 0x12 on x86
        jo::self<jo::UnitData>(this)->equipped->item = static_cast<std::uint16_t>(id);
        jo::self<jo::UnitData>(this)->equipped->count = static_cast<std::uint16_t>(w2);
        jo::view<std::uint32_t>(jo::self<jo::UnitData>(this)->equipped->uses) = d6;
        jo::view<std::uint32_t>(jo::self<jo::UnitData>(this)->equipped->max_uses) = da;
        jo::self<jo::UnitData>(this)->equipped->u32_e = de;
        jo::self<jo::UnitData>(this)->equipped->u16_4 = static_cast<std::uint16_t>(w4);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b89_05", name, addr, fn)
B("Sounds::create_missing", 0x00495b40, &H::create_missing);
B("Sounds::create", 0x004959b0, &H::create);
B("Names::set", 0x004970c0, &H::names_set);
B("Names::set_item", 0x00497270, &H::set_item);
#undef B

}  // namespace game_b89_05
}  // namespace

// ---------------------------------------- from modules/game_b91_01.cpp (game/b91_01)
namespace {
namespace game_b91_01 {

JOAN_COUNTER(c_unitdata_release, "UnitData::release");

struct H {
    // 0x00495ce0
    void* unitdata_release() {
        c_unitdata_release.hit();
        for (std::int32_t i = 0; i < 1; ++i) {
            if (jo::self<jo::UnitData>(this)->hands[i * 2] != nullptr) {
                game_delete(jo::view<void*>(jo::self<jo::UnitData>(this)->hands[i * 2]));
                jo::self<jo::UnitData>(this)->hands[i * 2] = nullptr;
            }
            if (jo::self<jo::UnitData>(this)->hands[i * 2 + 1] != nullptr) {
                game_delete(jo::view<void*>(jo::self<jo::UnitData>(this)->hands[i * 2 + 1]));
                jo::self<jo::UnitData>(this)->hands[i * 2 + 1] = nullptr;
            }
        }
        if (jo::self<jo::UnitData>(this)->equipped != nullptr) {
            game_delete(jo::view<void*>(jo::self<jo::UnitData>(this)->equipped));
            jo::self<jo::UnitData>(this)->equipped = nullptr;
        }
        if (jo::self<jo::UnitData>(this)->extra_item != nullptr) {
            game_delete(jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item));
            jo::self<jo::UnitData>(this)->extra_item = nullptr;
        }
        void* r = this;  // eax as left by the original
        if (jo::self<jo::UnitData>(this)->list_12 != nullptr) {
            r = joan::orig::arg::as<void*>(joan::orig::as_grid(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12))->destroy(1u));  // 0x00495ef0 deleting dtor
            jo::self<jo::UnitData>(this)->list_12 = nullptr;
        }
        if (jo::self<jo::UnitData>(this)->grid_16 != nullptr) {
            joan::orig::as_grid(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16))->destroy(1u);  // 0x00495ef0
            r = this;
            jo::self<jo::UnitData>(this)->grid_16 = nullptr;
        }
        if (jo::self<jo::UnitData>(this)->grid_1a != nullptr) {
            r = joan::orig::arg::as<void*>(joan::orig::as_grid(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a))->destroy(1u));  // 0x00495ef0
            jo::self<jo::UnitData>(this)->grid_1a = nullptr;
        }
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_01", name, addr, fn)
B("UnitData::release", 0x00495ce0, &H::unitdata_release);
#undef B

}  // namespace game_b91_01
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

struct H {
    // ------------------------------------------------------------------ 0x00495ef0
    void* scalar_dtor(u32 flags) {
        joan::orig::as_inventory(this)->destruct();  // 0x00457220
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }

    // ------------------------------------------------------------------ 0x00497490
    void release_count(i32 n) {
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item) == nullptr) return;
        u16& w = jo::self<jo::UnitData>(this)->extra_item->count;
        w = static_cast<u16>(w - static_cast<u16>(n));
        if (!(static_cast<i32>(jo::view<i16>(jo::self<jo::UnitData>(this)->extra_item->count)) > 0)) joan::orig::as_unit_data(this)->free_slot_e();  // 0x00497440
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Obj::scalar_dtor_495ef0", 0x00495ef0, &H::scalar_dtor);
B97("Obj::release_count", 0x00497490, &H::release_count);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b115_08.cpp (fix/uninit)
namespace {
namespace game_b115_08 {

using joan::orig::helpers::wadd;

using uptr = std::uintptr_t;

// Virtual thiscall: vtable slot at x86 byte offset `off` (slot off / 4, pointer-sized entries).
template <class R = void, class... A>
R vcall(void* o, i32 off, A... args) {
    return thiscall<R>((*static_cast<const uptr* const*>(o))[joan::call::x86_vslot(off)], o, args...);  // x86 byte offset -> slot
}
i32 sx16(u32 v) { return static_cast<i32>(static_cast<i16>(static_cast<u16>(v))); }

JOAN_COUNTER(c_inv, "Inventory::add_495f40");

i32 list_count(const void* l) { return joan::orig::as_dynarray(l)->size(); }  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------ 0x00495f40 (fix)
    i32 inventory_add(u32 a8, u32 ac, u32 a10, u32 a14, u32 a18, u32 a1c, u32 a20, u32 a24) {
        c_inv.hit();
        u32 cnt = a10;  // [ebp+0x10]; 0x004577a0 / 0x00456d60 rewrite its low word through &cnt
        auto n = [&cnt]() { return sx16(cnt); };
        u8* const ur = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(a14)));  // 0x0041b3c0 -> [ebp-8]
        void* const types = obj(d::g_item_res);
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(types)->get(joan::orig::arg::i16(a8)));  // 0x00425ce0 -> [ebp-0xc]
        auto kind2 = [e]() { return static_cast<i32>(jo::view<i16>(jo::as_item_def(e)->type_id)); };
        auto find = [this, e](GridField grid, u8* sp, u8* sx, u8* sy) {
            return joan::orig::arg::as<i32>(joan::orig::as_grid(jo::self<jo::UnitData>(this)->*grid)->find_place(joan::orig::arg::u32(static_cast<u32>(jo::view<u8[2]>(jo::as_item_def(e)->stats[0])[0])), joan::orig::arg::u32(static_cast<u32>(jo::view<u8[2]>(jo::as_item_def(e)->stats[1])[0])), joan::orig::arg::mptr(sp), joan::orig::arg::mptr(sx), joan::orig::arg::mptr(sy), 0));  // 0x004575a0
        };
        auto place = [&](u8 sp, u8 sx, u8 sy, u32 second) {
            joan::orig::as_unit_data(this)->add_to_grid16_1a(joan::orig::arg::u16(a8), joan::orig::arg::u16(ac), joan::orig::arg::u16(cnt), joan::orig::arg::u32(a18), joan::orig::arg::u32(a1c), joan::orig::arg::u32(a20), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(sp))), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(sx))), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(sy))), static_cast<std::uint8_t>(joan::orig::arg::u32(second)));  // 0x004976e0
        };
        auto notify = [e, a8]() {
            if (jo::view<i8>(jo::as_item_def(e)->unique_item) != 0) joan::orig::as_game(obj(d::g_game_state))->add_word(joan::orig::arg::i16(a8));  // 0x00472620
        };
        auto matches = [a8, a18, a1c](const u8* s) {
            return jo::view<i16>(jo::as_held_item(s)->item) == static_cast<i16>(a8) && jo::view<u32>(jo::as_held_item(s)->uses) == a18 && jo::view<u32>(jo::as_held_item(s)->max_uses) == a1c;
        };
        auto sum_list = [&](GridField grid, i32& sum) {
            for (i32 i = list_count(jo::self<jo::UnitData>(this)->*grid); i > 0; --i) {
                if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(jo::self<jo::UnitData>(this)->*grid)->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00457f60
                const u8* const s = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(jo::self<jo::UnitData>(this)->*grid)->get(joan::orig::arg::i32(i)));  // 0x00457e90
                if (matches(s)) sum = wadd(sum, jo::view<i16>(jo::as_item_def(e)->stats[4]) - jo::view<i16>(jo::as_held_item(s)->count));
            }
        };
        auto spread = [&](bool first) {
            if (first) joan::orig::as_inventory(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16))->add_stack(joan::orig::arg::i32(a8), joan::orig::arg::i32(ac), &cnt, joan::orig::arg::i32(a18), joan::orig::arg::i32(a1c), joan::orig::arg::i32(a20), 1);  // 0x004577a0
            if (n() > 0) joan::orig::as_inventory(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a))->add_stack(joan::orig::arg::i32(a8), joan::orig::arg::i32(ac), &cnt, joan::orig::arg::i32(a18), joan::orig::arg::i32(a1c), joan::orig::arg::i32(a20), 1);
        };
        {
            const i32 k0 = jo::view<i16>(jo::as_item_def(e)->class_id);
            if (k0 == 7 || k0 == 0xf || k0 == 0x10 || k0 == 0x13) {
                u8 sp = 0, sx = 0, sy = 0;
                i32 r1 = find(&jo::UnitData::grid_16, &sp, &sx, &sy);
                const i32 r2 = find(&jo::UnitData::grid_1a, &sp, &sx, &sy);
                i32 sum = 0;
                if (a24 != 0)
                    r1 = 0;
                else
                    sum_list(&jo::UnitData::grid_16, sum);
                if (r1 == 0 && r2 == 0 && sum < n()) return 0;
                spread(a24 == 0 && n() > 0);
                notify();
                return n() > 0 ? -n() : 1;
            }
        }
        {
            const i32 t = kind2();
            if (t == 0xd || t == 0xe || t == 0xf || t == 0x10 || t == 0x11 || t == 0x12) {
                u8 sp = 0, sx = 0, sy = 0;  // [ebp-0x30 / -0x34 / -0x38]
                const i32 r3 = find(&jo::UnitData::grid_16, &sp, &sx, &sy);
                if (n() < 0) {
                    if (r3 != 0) place(sp, sx, sy, 0);  // fix/uninit: no free slot -> no placement
                    notify();
                    return 1;
                }
                const i32 r4 = find(&jo::UnitData::grid_1a, &sp, &sx, &sy);
                i32 placed = 0;
                if (jo::self<jo::UnitData>(this)->extra_item == nullptr) {
                    i32 want = -1;
                    const i32 k = kind2();
                    if (k == 0xd || k == 0xe || k == 0xf)
                        want = 0xb;
                    else if (k == 0x10 || k == 0x11 || k == 0x12)
                        want = 0xc;
                    if (want >= 0 &&
                        (jo::self<jo::UnitData>(this)->equipped == nullptr ||
                         jo::view<i16>(jo::as_item_def(joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(types)->get(joan::orig::arg::i16(static_cast<u32>(jo::self<jo::UnitData>(this)->equipped->item)))))->type_id) == want)) {
                        const i16 lim = n() < jo::view<i16>(jo::as_item_def(e)->stats[4]) ? static_cast<i16>(cnt) : jo::view<i16>(jo::as_item_def(e)->stats[4]);
                        joan::orig::as_unit_data(this)->new_stack_e(joan::orig::arg::u32(a8), joan::orig::arg::u32(ac), joan::orig::arg::u32(static_cast<u32>(static_cast<u16>(lim))), joan::orig::arg::u32(a18), joan::orig::arg::u32(a1c), joan::orig::arg::u32(a20));  // 0x004973a0
                        placed = 1;
                        const i32 rest = n() > jo::view<i16>(jo::as_item_def(e)->stats[4]) ? n() - jo::view<i16>(jo::as_item_def(e)->stats[4]) : 0;
                        cnt = (cnt & 0xffff0000u) | static_cast<u16>(rest);
                    }
                }
                i32 sum = 0;
                {
                    const u8* const cur = jo::view<u8*>(jo::self<jo::UnitData>(this)->extra_item);
                    if (cur != nullptr && matches(cur) && jo::view<i16>(jo::as_held_item(cur)->count) >= 0)
                        sum = wadd(sum, jo::view<i16>(jo::as_item_def(e)->stats[4]) - jo::view<i16>(jo::as_held_item(cur)->count));
                }
                sum_list(&jo::UnitData::grid_16, sum);
                sum_list(&jo::UnitData::grid_1a, sum);
                if (placed == 0 && r3 == 0 && r4 == 0 && sum < n()) return 0;
                {
                    const u8* const cur = jo::view<u8*>(jo::self<jo::UnitData>(this)->extra_item);
                    if (cur != nullptr && matches(cur) && n() >= 0) joan::orig::as_item_stack(jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item))->take(&cnt);  // 0x00456d60
                }
                spread(n() > 0);
                notify();
                return n() > 0 ? -n() : 1;
            }
        }
        auto owner_ok = [e, ur]() { return jo::view<i16>(jo::as_item_def(e)->usable_unit) == 0 || jo::view<i16>(jo::as_item_def(e)->usable_unit) == jo::as_unit(ur)->type_id; };
        auto place_any = [&]() {  // 0x00496881 / 0x00496a81: back pack, else belt; 0 when both are full
            u8 sp = 0, sx = 0, sy = 0;
            if (find(&jo::UnitData::grid_16, &sp, &sx, &sy) != 0) {
                place(sp, sx, sy, 0);
                return true;
            }
            if (find(&jo::UnitData::grid_1a, &sp, &sx, &sy) != 0) {
                place(sp, sx, sy, 1);
                return true;
            }
            return false;
        };
        {
            const i32 t = kind2();
            if (t == 8 || t == 9 || t == 0xa) {
                if (owner_ok()) {
                    for (i32 k = 0; k < 1; ++k) {
                        if (jo::self<jo::UnitData>(this)->hands[k * 2] != nullptr || jo::self<jo::UnitData>(this)->hands[k * 2 + 1] != nullptr) continue;
                        joan::orig::as_names(this)->set_4970c0(joan::orig::arg::u32(k + 1), joan::orig::arg::u32(kind2() == 8 ? 1 : 0), joan::orig::arg::u32(a8), joan::orig::arg::u32(ac), joan::orig::arg::u32(cnt), joan::orig::arg::u32(a18), joan::orig::arg::u32(a1c), joan::orig::arg::u32(a20));  // 0x004970c0
                        if (joan::orig::arg::as<i32>(joan::orig::as_world(ur)->flag_1fd()) != 0) return 1;  // 0x0042d690
                        const i32 h = joan::orig::arg::as<i32>(joan::orig::as_units(obj(d::g_items))->create_49ea90(joan::orig::arg::u32(sx16(a8))));  // 0x0049ea90
                        if (h != 0) {
                            void* const o1 = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(h));  // 0x00425d20
                            vcall(o1, 0x18, 1);
                            void* const o2 = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(h));
                            vcall(o2, 0x1c);
                            const u8* const tt = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(types)->get(joan::orig::arg::i16(a8)));
                            joan::orig::as_cell_object(ur)->attach_unit(joan::orig::arg::i16(static_cast<u32>(h)), static_cast<i32>(jo::view<i16>(jo::as_item_def(tt)->bone)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
                        }
                        notify();
                        return 1;
                    }
                }
                if (!place_any()) return 0;
                notify();
                return 1;
            }
            if (t == 0xb || t == 0xc) {
                if (owner_ok() && jo::self<jo::UnitData>(this)->equipped == nullptr) {
                    bool ok = true;
                    if (jo::self<jo::UnitData>(this)->extra_item != nullptr) {
                        const u8* const cur = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(types)->get(joan::orig::arg::i16(static_cast<u32>(jo::self<jo::UnitData>(this)->extra_item->item))));
                        const i32 c2 = jo::view<i16>(jo::as_item_def(cur)->type_id);
                        if (kind2() == 0xc && !(c2 == 0x10 || c2 == 0x11 || c2 == 0x12)) {
                            ok = false;
                        } else if (kind2() == 0xb) {
                            const i32 c3 = jo::view<i16>(jo::as_item_def(cur)->type_id);
                            if (!(c3 == 0xd || c3 == 0xe || c3 == 0xf)) ok = false;
                        }
                    }
                    if (ok) {
                        joan::orig::as_names(this)->set_item(joan::orig::arg::u32(a8), joan::orig::arg::u32(ac), joan::orig::arg::u32(cnt), joan::orig::arg::u32(a18), joan::orig::arg::u32(a1c), joan::orig::arg::u32(a20));  // 0x00497270
                        notify();
                        return 1;
                    }
                }
                if (!place_any()) return 0;
                notify();
                return 1;
            }
        }
        if (jo::view<i16>(jo::as_item_def(e)->class_id) == 9) {
            u8 sp = 0, sx = 0, sy = 0;  // [ebp-0x88 / -0x8c / -0x90], read only after a success
            i32 r = 0;
            if (owner_ok()) r = find(&jo::UnitData::list_12, &sp, &sx, &sy);
            if (r != 0) {
                joan::orig::as_unit_data(this)->add_to_grid12(joan::orig::arg::u16(a8), joan::orig::arg::u16(ac), joan::orig::arg::u16(cnt), joan::orig::arg::u32(a18), joan::orig::arg::u32(a1c), joan::orig::arg::u32(a20), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(sp))), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(sx))), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(sy))));  // 0x004975d0
            } else if (!place_any()) {
                return 0;
            }
        }
        notify();
        return 1;
    }
};

#define FIX(name, address, fn) JOAN_REPLACE("fix/uninit", name, address, fn)
// fix/uninit: negative count without a free slot (0x004575a0 outs unwritten) -> no placement
FIX("Inventory::add", 0x00495f40, &H::inventory_add);
#undef FIX

}  // namespace game_b115_08
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

using joan::call::import_call;
struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");
Vec3 load3(const void* p) {
    Vec3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

JOAN_COUNTER(c_aim, "aim_velocity");

// ---------------------------------------------------------------- 0x00496d40
// out = 1000 * s * dir, dir: launch direction from a to b (gravity g, speed / time params).
static void* JOAN_CDECL aim_velocity(Vec3* out, Vec3 a, Vec3 b, float s, float max_len, u32 /*unused*/,
                                  float speed, float g, float min_z) {
    c_aim.hit();
    Vec3 tmp;
    const Vec3 d0 = load3(joan::orig::arg::as<void*>(joan::orig::vec_sub(&tmp, &b, &a)));  // 0x0060cc90
    float len = v3d_float(d::iat_v3d_87_length, &d0);                      // length
    Vec3 tmp2;
    Vec3 dn0 = load3(joan::orig::arg::as<void*>(joan::orig::vec_sub(&tmp2, &b, &a)));      // 0x0060cc90
    const Vec3 dir = load3(import_call<void*>(d::iat_v3d_87_vector_normalize_1, &dn0));         // normalize in place
    if (len > max_len) {  // test ah,0x41 jne: skipped when <=, equal or unordered
        Vec3 t3;
        const Vec3 sc = load3(joan::orig::arg::as<void*>(joan::orig::vec_scale(&t3, &dir, joan::orig::arg::f32(max_len))));  // 0x0060cd00
        Vec3 t4;
        b = load3(joan::orig::arg::as<void*>(joan::orig::vec_add(&t4, &a, &sc)));  // 0x0060cc20
        len = max_len;
    }
    float t = len / speed;
    const float k1 = global<float>(d::flt_1), k2 = global<float>(d::flt_46_1);
    const float kg = global<float>(d::flt_9_81), k3 = global<float>(d::flt_2);
    Vec3 v;
    v.x = (b.x - a.x) * k1 / k2 / t;
    v.y = (b.y - a.y) * k1 / k2 / t;
    v.z = ((b.z - a.z) * k1 / k2 + g * kg * t * t / k3) / t;
    Vec3 n = v;
    import_call<void*>(d::iat_v3d_87_vector_normalize_1, &n);  // normalize in place
    t = 0.1f;                            // 0x3dcccccd
    Vec3 bp = b, ap = a;
    bp.z = 0.0f;
    ap.z = 0.0f;
    Vec3 t5;
    const Vec3 hd = load3(joan::orig::arg::as<void*>(joan::orig::vec_sub(&t5, &bp, &ap)));  // 0x0060cc90
    const float hlen = v3d_float(d::iat_v3d_87_length, &hd);
    float lim;
    if (!(hlen >= global<float>(d::flt_800))) {  // test ah,1 (less or unordered)
        lim = global<float>(d::flt_1_5) * hlen / global<float>(d::flt_800) + global<float>(d::flt_m1_5);
    } else {
        lim = min_z;
    }
    while (!(n.z >= lim)) {  // test ah,1 je exit: loop while less or unordered
        t = t + global<float>(d::flt_0_2);
        v.x = (b.x - a.x) * k1 / k2 / t;
        v.y = (b.y - a.y) * k1 / k2 / t;
        v.z = ((b.z - a.z) * k1 / k2 + g * kg * t * t / k3) / t;
        import_call<void*>(d::iat_v3d_87_vector_normalize_2, &n, &v);  // normalize(out, in)
    }
    Vec3 t6;
    const Vec3 r1 = load3(joan::orig::arg::as<void*>(joan::orig::vec_scale(&t6, &v, joan::orig::arg::f32(s))));  // 0x0060cd00
    Vec3 t7;
    const Vec3 r2 = load3(joan::orig::arg::as<void*>(joan::orig::vec_scale(&t7, &r1, 1000.0f)));  // 0x447a0000
    *out = r2;
    return out;
}

struct H {
    // ------------------------------------------------------------------ 0x004976b0
    u32 call_457ec0(i16 v) {
        joan::orig::as_grid(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12))->fill_from(static_cast<i32>(v));  // 0x00457ec0
        return 0;  // Grid::fill_from returns nothing; callers (0x006db491, i_buy.cpp) discard the result
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("aim_velocity", 0x00496d40, &aim_velocity);
B95("Obj::call_457ec0", 0x004976b0, &H::call_457ec0);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

struct H {
    // ------------------------------------------------------------------------ 0x00497350
    void free_0a() {
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->equipped) == nullptr) return;
        game_delete(jo::view<void*>(jo::self<jo::UnitData>(this)->equipped));
        jo::view<void*>(jo::self<jo::UnitData>(this)->equipped) = nullptr;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Obj::free_0a", 0x00497350, &H::free_0a);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_units4.cpp (game/units4)
namespace {
namespace game_units4 {

constexpr img::Addr kRecords(d::g_item_res);  // 0x2bf-byte item records

void* record_get(std::int16_t id) { return joan::orig::arg::as<void*>(joan::orig::as_records_2bf(img::obj(kRecords))->get(joan::orig::arg::i16(id))); }  // 0x00425ce0
std::int32_t list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

using joan::orig::helpers::wrap_add;
using joan::orig::helpers::wrap_mul;

// The jump tables of 0x00497e10 (all five identical for kinds 1..23). Returns false for
// the veto (kind 19 with a non-zero value: the whole sum returns 0).
bool combine(std::int32_t kind, std::int32_t& v, std::int32_t x) {
    switch (kind) {
    case 1: case 2: case 3: case 4: case 5: case 6: case 20:
        v = wrap_add(v, x);
        break;
    case 7: case 8: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 23:
        v = wrap_add(v, wrap_mul(wrap_add(v, 100), x) / 100);
        break;
    case 9: case 22:
        v = wrap_add(v, wrap_mul(static_cast<std::int32_t>(100u - static_cast<std::uint32_t>(v)), x) / 100);
        break;
    case 18:
        if (x > v) v = x;
        break;
    case 19:
        if (x != 0) return false;
        break;
    case 21:
        v = wrap_mul(v, x) / 100;
        break;
    default:
        break;
    }
    return true;
}

JOAN_COUNTER(c_mod_sum, "Modifiers::sum");

struct H {
    // 0x00497e10 Modifiers::sum(kind, arg) (ret 8). The original leaves the accumulator
    // uninitialised for kinds outside 1..23; all 39 call sites pass constants 1..23, so 0 is
    // used there.
    std::int32_t mod_sum(std::int32_t kind, std::int32_t arg) {
        c_mod_sum.hit();
        std::int32_t v = 0;
        if (kind == 19 || kind == 21) v = 100;
        std::int32_t x = 0;
        if (static_cast<std::int32_t>(jo::self<jo::UnitData>(this)->hand_mode) > 0 && static_cast<std::int32_t>(jo::self<jo::UnitData>(this)->hand_mode) <= 1) {
            for (std::int32_t slot = 0; slot < 2; ++slot) {
                std::int16_t* const item = joan::orig::arg::as<std::int16_t*>(joan::orig::as_modifiers(this)->slot(joan::orig::arg::i32(slot)));  // 0x004974f0
                if (item == nullptr || item == jo::view<std::int16_t*>(jo::self<jo::UnitData>(this)->equipped)) continue;
                void* const rec = record_get(item[0]);
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_mod_table(rec)->value(joan::orig::arg::i32(kind), static_cast<std::int32_t>(item[2]), &x)) == 0)  // 0x004a4200
                    continue;
                if (!combine(kind, v, x)) return 0;
            }
            if (jo::view<std::int16_t*>(jo::self<jo::UnitData>(this)->equipped) != nullptr) {
                void* const rec = record_get(jo::view<std::int16_t*>(jo::self<jo::UnitData>(this)->equipped)[0]);
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_mod_table(rec)->value(joan::orig::arg::i32(kind), static_cast<std::int32_t>(jo::view<std::int16_t*>(jo::self<jo::UnitData>(this)->equipped)[2]), &x)) != 0) {
                    if (!combine(kind, v, x)) return 0;
                }
            }
        }
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->list_12) == nullptr) return v;
        for (std::int32_t i = list_count(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12)); i > 0; --i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_ptr_list(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12))->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00457f60
            std::int16_t* const entry = joan::orig::arg::as<std::int16_t*>(joan::orig::as_ptr_list(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12))->get(joan::orig::arg::i32(i)));  // 0x00457e90
            void* const rec = record_get(entry[0]);
            std::int32_t best = 1;
            for (std::int32_t j = list_count(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12)); j > 0; --j) {
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_ptr_list(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12))->is_empty_at(joan::orig::arg::i32(j))) != 0) continue;
                std::int16_t* const other = joan::orig::arg::as<std::int16_t*>(joan::orig::as_ptr_list(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12))->get(joan::orig::arg::i32(j)));
                if (static_cast<std::int32_t>(other[0]) != static_cast<std::int32_t>(entry[0])) continue;
                if (static_cast<std::int32_t>(other[2]) > static_cast<std::int32_t>(entry[2])) {
                    best = 0;
                    break;
                }
                if (static_cast<std::int32_t>(other[2]) == static_cast<std::int32_t>(entry[2]) && j > i) {
                    best = 0;
                    break;
                }
            }
            if (best == 0) continue;
            std::int32_t y = 0;
            if (jo::as_mod_table(rec)->damage_type == kind) {
                y = joan::orig::arg::as<std::int32_t>(joan::orig::as_mod_table(rec)->value_own(static_cast<std::int32_t>(entry[2])));  // 0x004a41c0
            } else if (joan::orig::arg::as<std::int32_t>(joan::orig::as_mod_table(rec)->value(joan::orig::arg::i32(kind), static_cast<std::int32_t>(entry[2]), &y)) == 0) {
                continue;
            }
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_mod_table(rec)->requirements_met_4a42f0(static_cast<std::int32_t>(entry[2]), joan::orig::arg::i32(arg))) == 0) continue;  // 0x004a42f0
            if (!combine(kind, v, y)) return 0;
        }
        return v;
    }

    // --- small callees -------------------------------------------------------------------
    // 0x004974f0 Modifiers::slot(index): kind 0 -> 0, 2 -> the mount (+0xa), <= 1 -> the
    // dword at this + kind * 8 - 6 + index * 4, else the kind itself.
    void* mod_slot(std::int32_t index) {
        const std::int32_t k = static_cast<std::int32_t>(jo::self<jo::UnitData>(this)->hand_mode);
        if (k == 0) return nullptr;
        if (k == 2) return jo::self<jo::UnitData>(this)->equipped;
        if (k > 1) return reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(k)));  // x64: integer passed as pointer
        return held(this, k * 2 - 2 + static_cast<std::int32_t>(static_cast<std::int16_t>(index)));
    }
};

#define U4(name, addr, fn) JOAN_REPLACE("game/units4", name, addr, fn)
U4("Modifiers::sum", 0x00497e10, &H::mod_sum);
U4("Modifiers::slot", 0x004974f0, &H::mod_slot);
#undef U4

}  // namespace game_units4
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    jo::HeldItem* get_0a() { return jo::self<jo::UnitData>(this)->equipped; }  // 0x00497560
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("get_dword_0a", 0x00497560, &H::get_0a);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

struct H {
    jo::HeldItem* get_e() { return jo::self<jo::UnitData>(this)->extra_item; }  // 0x00497580
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Obj::get_e", 0x00497580, &H::get_e);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

struct H {
    // ------------------------------------------------------------------------ 0x004978a0
    u32 forward_457ec0(u32 a, u32 b) {
        const i32 v = static_cast<i16>(a);
        // Grid::fill_from returns nothing; callers (0x006dac07, 0x006db10a, i_buy.cpp) discard the result.
        if (static_cast<i8>(b) == 0) {
            joan::orig::as_grid(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16))->fill_from(v);  // 0x00457ec0
            return 0;
        }
        joan::orig::as_grid(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a))->fill_from(v);  // 0x00457ec0
        return 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("X::forward_457ec0", 0x004978a0, &H::forward_457ec0);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

void* const kTypes = obj(d::g_item_res);

u8* type_record(i16 type) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(type))); }  // 0x00425ce0

JOAN_COUNTER(c_count_type, "Slots::count_type");

struct H {
    // ------------------------------------------------------------------ 0x004978f0
    i32 count_type(i32 kind) {
        c_count_type.hit();
        i32 n = 0;
        for (i32 i = 0; i < 1; ++i) {
            if (i == static_cast<i32>(jo::self<jo::UnitData>(this)->hand_mode) - 1) continue;
            u8* const a = jo::view<u8*>(jo::self<jo::UnitData>(this)->hands[i * 2]);
            u8* const b = jo::view<u8*>(jo::self<jo::UnitData>(this)->hands[i * 2 + 1]);
            if (a == nullptr && b == nullptr) continue;
            if (a != nullptr && jo::view<i32>(jo::as_item_def(type_record(jo::view<i16>(jo::as_held_item(a)->item)))->colour) == kind &&
                jo::as_held_item(a)->uses < jo::as_held_item(a)->max_uses) {
                ++n;
            }
            if (b != nullptr && jo::view<i32>(jo::as_item_def(type_record(jo::view<i16>(jo::as_held_item(b)->item)))->colour) == kind &&
                jo::as_held_item(b)->uses < jo::as_held_item(b)->max_uses) {
                ++n;
            }
        }
        return n;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Slots::count_type", 0x004978f0, &H::count_type);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

JOAN_COUNTER(c_copy, "UnitData::copy");

struct H {
    // 0x00497a10
    void* copy(const std::uint8_t* src) {
        c_copy.hit();
        joan::orig::as_unit_data(this)->release();  // 0x00495ce0
        jo::view<std::uint16_t>(jo::self<jo::UnitData>(this)->hand_mode) = jo::view<std::uint16_t>(jo::as_unit_data(src)->hand_mode);
        for (std::int32_t i = 0; i < 1; ++i) {
            for (std::int32_t j = 0; j < 2; ++j) {
                if (jo::view<void*>(jo::as_unit_data(src)->hands[i * 2 + j]) != nullptr) {
                    void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::HeldItem)));  // 0x12 on x86
                    jo::view<void*>(jo::self<jo::UnitData>(this)->hands[i * 2 + j]) = mem;
                    std::memcpy(jo::view<void*>(jo::self<jo::UnitData>(this)->hands[i * 2 + j]), jo::view<void*>(jo::as_unit_data(src)->hands[i * 2 + j]), sizeof(jo::HeldItem));
                } else {
                    jo::self<jo::UnitData>(this)->hands[i * 2 + j] = nullptr;
                }
            }
        }
        for (std::int32_t k = 2; k <= 3; ++k) {  // equipped, extra_item
            if (held(src, k) != nullptr) {
                void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::HeldItem)));  // 0x12 on x86
                jo::view<void*>(held(this, k)) = mem;
                std::memcpy(held(this, k), held(src, k), sizeof(jo::HeldItem));
            } else {
                held(this, k) = nullptr;
            }
        }
        struct GridArgs {
            GridField off;
            std::int32_t n, size, m;
        };
        const GridArgs grids[3] = {{&jo::UnitData::list_12, 1, 4, 1}, {&jo::UnitData::grid_16, 1, 6, 8}, {&jo::UnitData::grid_1a, 1, 6, 4}};
        for (std::int32_t k = 0; k < 3; ++k) {
            const GridArgs& g = grids[k];
            if (jo::self<jo::UnitData>(src)->*g.off != nullptr) {
                void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Grid)));  // 0x3e on x86
                void* const grid = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_grid(mem)->Grid_4571e0()) : nullptr;  // 0x004571e0
                jo::view<void*>(jo::self<jo::UnitData>(this)->*g.off) = grid;
                joan::orig::as_grid(jo::self<jo::UnitData>(this)->*g.off)->alloc(joan::orig::arg::i32(g.n), joan::orig::arg::i32(g.size), joan::orig::arg::i32(g.m));  // 0x00457290
                joan::orig::as_grid(jo::self<jo::UnitData>(this)->*g.off)->assign(joan::orig::arg::mptr(jo::self<jo::UnitData>(src)->*g.off));  // 0x00458530
            } else {
                jo::self<jo::UnitData>(this)->*g.off = nullptr;
            }
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("UnitData::copy", 0x00497a10, &H::copy);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

using joan::hook::x87::Fpu;

void* const kGroups = obj(d::g_nations);

JOAN_COUNTER(c_rank, "Obj::rank");

i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------ 0x00498460
    float rank(i32 id) {
        c_rank.hit();
        i32 n = 0, above_me = 0, above_id = 0;
        for (i32 i = 1; i <= list_count(kGroups); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(kGroups)->is_unset(joan::orig::arg::i32(i))) != 0) continue;  // 0x004b6780
            ++n;
            const u8* const gi = joan::orig::arg::as<const u8*>(joan::orig::as_dynarray(kGroups)->ptr_at(joan::orig::arg::i32(i)));  // 0x0047b600
            const u8* const gid = joan::orig::arg::as<const u8*>(joan::orig::as_dynarray(kGroups)->ptr_at(joan::orig::arg::i32(id)));  // 0x0047b600
            // test ah,0x41 jne skip: count only when strictly greater (not unordered).
            if (jo::as_rules(gi)->f32_14dd > jo::as_rules(gid)->f32_14dd) ++above_id;
            const u8* const gi2 = joan::orig::arg::as<const u8*>(joan::orig::as_dynarray(kGroups)->ptr_at(joan::orig::arg::i32(i)));  // 0x0047b600
            if (jo::as_rules(gi2)->f32_14dd > jo::self<jo::Rules>(this)->f32_14dd) ++above_me;
        }
        if (n >= 3) {
            constexpr Fpu fpu{true};
            const float t1 = static_cast<float>(
                fpu.div(static_cast<double>(above_id + above_me), static_cast<double>(n << 1)));
            const u8* const rec = joan::orig::arg::as<const u8*>(joan::orig::as_player(this)->relation(joan::orig::arg::i32(id)));  // 0x004b42e0
            const double q = fpu.div(static_cast<double>(jo::self<jo::Relation>(rec)->opinion), static_cast<double>(global<float>(d::flt_100)));
            const float t2 = static_cast<float>(fpu.mul(q, static_cast<double>(global<float>(d::flt_0_25))));
            return t1 + t2;
        }
        const float k = global<float>(d::flt_0_3);
        return k - jo::self<jo::Rules>(this)->f32_1521 * k;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Obj::rank", 0x00498460, &H::rank);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b101_04.cpp (game/b101_04)
namespace {
namespace game_b101_04 {

JOAN_COUNTER(c_count_kind, "Obj::count_of_kind");

struct H {
    // ------------------------------------------------------------------ 0x004987d0
    i32 count_of_kind(i32 k) {
        c_count_kind.hit();
        u32 sum = 0;
        for (i32 i = 0; i < 1; ++i) {
            for (i32 j = 0; j < 2; ++j) {
                u8* const s = jo::view<u8*>(jo::self<jo::UnitData>(this)->hands[i * 2 + j]);
                if (s == nullptr) continue;
                if (static_cast<i32>(jo::view<i16>(jo::as_held_item(s)->item)) != k) continue;
                sum += static_cast<u32>(static_cast<i32>(jo::view<i16>(jo::as_held_item(s)->count)));
            }
        }
        if (jo::view<u8*>(jo::self<jo::UnitData>(this)->equipped) != nullptr && static_cast<i32>(jo::view<i16>(jo::self<jo::UnitData>(this)->equipped->item)) == k)
            sum += static_cast<u32>(static_cast<i32>(jo::view<i16>(jo::self<jo::UnitData>(this)->equipped->count)));
        if (jo::view<u8*>(jo::self<jo::UnitData>(this)->extra_item) != nullptr && static_cast<i32>(jo::view<i16>(jo::self<jo::UnitData>(this)->extra_item->item)) == k)
            sum += static_cast<u32>(static_cast<i32>(jo::view<i16>(jo::self<jo::UnitData>(this)->extra_item->count)));
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->list_12) != nullptr) sum += joan::orig::arg::as<u32>(joan::orig::as_inventory(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12))->count(joan::orig::arg::i32(k)));  // 0x00458390
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16) != nullptr) sum += joan::orig::arg::as<u32>(joan::orig::as_inventory(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16))->count(joan::orig::arg::i32(k)));  // 0x00458390
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a) != nullptr) sum += joan::orig::arg::as<u32>(joan::orig::as_inventory(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a))->count(joan::orig::arg::i32(k)));  // 0x00458390
        return static_cast<i32>(sum);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_04", name, addr, fn)
B("Obj::count_of_kind", 0x004987d0, &H::count_of_kind);
#undef B

}  // namespace game_b101_04
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

void* const kTypes = obj(d::g_item_res);
void* const kUnitDefs = obj(d::g_units);

u8* type_record(u16 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0

JOAN_COUNTER(c_add_list12, "Holder::add_list12");
JOAN_COUNTER(c_add_list16, "Holder::add_list16_1a");
JOAN_COUNTER(c_count_of, "Holder::count_of");
JOAN_COUNTER(c_remove_type, "Holder::remove_type");

struct H {
    // ------------------------------------------------------------------ 0x004975a0
    void* set_word0(u16 w) {
        jo::view<u16>(jo::self<jo::UnitData>(this)->hand_mode) = w;
        return this;
    }

    u8* add_entry(GridField list_off, u16 id, u16 w4, u16 w2, u32 d6, u32 da, u32 de, u8 b20, u8 b24, u8 b28) {
        const u16 r24 = jo::as_item_def(type_record(id))->stats[0];
        const u16 r26 = jo::as_item_def(type_record(id))->stats[1];
        const u32 slot = joan::orig::arg::as<u32>(joan::orig::as_list(jo::self<jo::UnitData>(this)->*list_off)->add_item(static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u8>(r24))), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u8>(r26))), static_cast<std::uint8_t>(joan::orig::arg::u32(b20)), static_cast<std::uint8_t>(joan::orig::arg::u32(b24)), static_cast<std::uint8_t>(joan::orig::arg::u32(b28))));  // 0x004576f0
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(jo::self<jo::UnitData>(this)->*list_off)->get(joan::orig::arg::i32(slot)));  // 0x00457e90
        jo::as_held_item(e)->item = id;
        jo::as_held_item(e)->count = w2;
        jo::view<u32>(jo::as_held_item(e)->uses) = d6;
        jo::view<u32>(jo::as_held_item(e)->max_uses) = da;
        jo::as_held_item(e)->u32_e = de;
        jo::as_held_item(e)->u16_4 = w4;
        return e;
    }

    // ------------------------------------------------------------------ 0x004975d0
    // eax left by the original: the last dword argument (de).
    u32 add_list12(u16 id, u16 w4, u16 w2, u32 d6, u32 da, u32 de, u8 b20, u8 b24, u8 b28) {
        c_add_list12.hit();
        add_entry(&jo::UnitData::list_12, id, w4, w2, d6, da, de, b20, b24, b28);
        return de;
    }

    // ------------------------------------------------------------------ 0x004976e0
    // eax left by the original: high word of de (list 0x16) / of the entry pointer (list 0x1a),
    // low word w4 (mov ax, w4).
    std::uintptr_t add_list16_1a(u16 id, u16 w4, u16 w2, u32 d6, u32 da, u32 de, u8 b20, u8 b24, u8 b28, u8 second) {
        c_add_list16.hit();
        if (static_cast<i32>(static_cast<i8>(second)) == 0) {
            add_entry(&jo::UnitData::grid_16, id, w4, w2, d6, da, de, b20, b24, b28);
            return (de & 0xffff0000u) | w4;
        }
        u8* const e = add_entry(&jo::UnitData::grid_1a, id, w4, w2, d6, da, de, b20, b24, b28);
        return (reinterpret_cast<std::uintptr_t>(e) & 0xffff0000u) | w4;

    }

    // ------------------------------------------------------------------ 0x004985f0
    i32 count_of(i32 k) {
        c_count_of.hit();
        i32 sum = 0;
        for (i32 i = 0; i < 1; ++i) {
            for (i32 j = 0; j < 2; ++j) {
                u8* const e = jo::view<u8*>(jo::self<jo::UnitData>(this)->hands[i * 2 + j]);
                if (e == nullptr) continue;
                if (static_cast<i32>(jo::view<i16>(jo::as_item_def(type_record(jo::as_held_item(e)->item))->class_id)) != k) continue;
                sum += jo::view<i16>(jo::as_held_item(jo::self<jo::UnitData>(this)->hands[i * 2 + j])->count);
            }
        }
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->equipped) != nullptr && static_cast<i32>(jo::view<i16>(jo::as_item_def(type_record(jo::self<jo::UnitData>(this)->equipped->item))->class_id)) == k)
            sum += jo::view<i16>(jo::self<jo::UnitData>(this)->equipped->count);
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item) != nullptr && static_cast<i32>(jo::view<i16>(jo::as_item_def(type_record(jo::self<jo::UnitData>(this)->extra_item->item))->class_id)) == k)
            sum += jo::view<i16>(jo::self<jo::UnitData>(this)->extra_item->count);
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->list_12) != nullptr) sum += joan::orig::arg::as<i32>(joan::orig::as_list(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12))->count_of(joan::orig::arg::i32(k)));  // 0x004582e0
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16) != nullptr) sum += joan::orig::arg::as<i32>(joan::orig::as_list(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16))->count_of(joan::orig::arg::i32(k)));
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a) != nullptr) sum += joan::orig::arg::as<i32>(joan::orig::as_list(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a))->count_of(joan::orig::arg::i32(k)));
        return sum;
    }

    // ------------------------------------------------------------------ 0x00498970
    u32 remove_type(i32 k, u32 a, u32 b) {
        c_remove_type.hit();
        for (i32 i = 1; i <= 1; ++i) {
            for (i32 j = 0; j < 2; ++j) {
                u8* const e = jo::view<u8*>(jo::self<jo::UnitData>(this)->hands[(i - 1) * 2 + j]);
                if (e == nullptr) continue;
                if (static_cast<i32>(jo::view<i16>(jo::as_held_item(e)->item)) != k) continue;
                void* const def = joan::orig::as_dynarray(kUnitDefs)->ptr_at(joan::orig::arg::i32(a));  // 0x0041b3c0
                joan::orig::as_cell_object(def)->attach_unit(joan::orig::arg::i16(0), joan::orig::arg::i32(i - 1), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(j))), joan::orig::arg::mptr(0));  // 0x0050a480
                joan::orig::as_slots(this)->free(joan::orig::arg::i32(static_cast<u16>(i)), joan::orig::arg::i32(static_cast<u16>(j)));  // 0x004971e0
            }
        }
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->equipped) != nullptr && static_cast<i32>(jo::view<i16>(jo::self<jo::UnitData>(this)->equipped->item)) == k)
            joan::orig::as_unit_data(this)->free_slot_a();  // 0x00497350
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->extra_item) != nullptr && static_cast<i32>(jo::view<i16>(jo::self<jo::UnitData>(this)->extra_item->item)) == k)
            joan::orig::as_unit_data(this)->free_slot_e();  // 0x00497440
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->list_12) != nullptr) joan::orig::as_item_list(jo::view<void*>(jo::self<jo::UnitData>(this)->list_12))->remove(joan::orig::arg::i32(k), 0);  // 0x00458430
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16) != nullptr) joan::orig::as_item_list(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_16))->remove(joan::orig::arg::i32(k), joan::orig::arg::i32(b));
        if (jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a) != nullptr) joan::orig::as_item_list(jo::view<void*>(jo::self<jo::UnitData>(this)->grid_1a))->remove(joan::orig::arg::i32(k), joan::orig::arg::i32(b));
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Obj::set_word0", 0x004975a0, &H::set_word0);
B("Holder::add_list12", 0x004975d0, &H::add_list12);
B("Holder::add_list16_1a", 0x004976e0, &H::add_list16_1a);
B("Holder::count_of", 0x004985f0, &H::count_of);
B("Holder::remove_type", 0x00498970, &H::remove_type);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::UnitData::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::zero_1e>(this)); }  // 0x495950
auto joan::orig::UnitData::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_02::H::dtor_body>(this)); }  // 0x495980
auto joan::orig::Sounds::create(std::uint32_t a0) -> void { joan::bind::member<&::game_b89_05::H::create>(this, a0); }  // 0x4959b0
auto joan::orig::Sounds::create_missing() -> void { joan::bind::member<&::game_b89_05::H::create_missing>(this); }  // 0x495b40
auto joan::orig::UnitData::release() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_01::H::unitdata_release>(this)); }  // 0x495ce0
auto joan::orig::Grid::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_02::H::scalar_dtor>(this, a0)); }  // 0x495ef0
auto joan::orig::Inventory::add_495f40(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b115_08::H::inventory_add>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x495f40
auto joan::orig::Names::set_4970c0(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b89_05::H::names_set>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x4970c0
auto joan::orig::Slots::free(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b92_02::H::slots_free>(this, a0, a1); }  // 0x4971e0
auto joan::orig::Names::set_item(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b89_05::H::set_item>(this, a0, a1, a2, a3, a4, a5)); }  // 0x497270
auto joan::orig::UnitData::free_slot_a() -> void { joan::bind::member<&::game_b93_01::H::free_0a>(this); }  // 0x497350
auto joan::orig::UnitData::new_stack_e(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::new_record>(this, a0, a1, a2, a3, a4, a5)); }  // 0x4973a0
auto joan::orig::UnitData::free_slot_e() -> void { joan::bind::member<&::game_b92_02::H::free_e>(this); }  // 0x497440
auto joan::orig::UnitData::release_count(std::int32_t a0) -> void { joan::bind::member<&::game_b97_02::H::release_count>(this, a0); }  // 0x497490
auto joan::orig::Modifiers::slot(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_units4::H::mod_slot>(this, a0)); }  // 0x4974f0
auto joan::orig::UnitData::slot_a() -> HeldItem* { return joan::bind::conv<HeldItem*>(joan::bind::member<&::game_b82_18::H::get_0a>(this)); }  // 0x497560
auto joan::orig::UnitData::slot_e() -> HeldItem* { return joan::bind::conv<HeldItem*>(joan::bind::member<&::game_b83_07::H::get_e>(this)); }  // 0x497580
auto joan::orig::UnitData::set_owner_id(std::uint16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_04::H::set_word0>(this, a0)); }  // 0x4975a0
auto joan::orig::UnitData::add_to_grid12(std::uint16_t a0, std::uint16_t a1, std::uint16_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint8_t a6, std::uint8_t a7, std::uint8_t a8) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_04::H::add_list12>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x4975d0
auto joan::orig::UnitData::fill_grid12(std::int16_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_02::H::call_457ec0>(this, a0)); }  // 0x4976b0
auto joan::orig::UnitData::add_to_grid16_1a(std::uint16_t a0, std::uint16_t a1, std::uint16_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint8_t a6, std::uint8_t a7, std::uint8_t a8, std::uint8_t a9) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_04::H::add_list16_1a>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x4976e0
auto joan::orig::UnitData::fill_grid16_1a(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_02::H::forward_457ec0>(this, a0, a1)); }  // 0x4978a0
auto joan::orig::UnitData::copy(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_02::H::copy>(this, a0)); }  // 0x497a10
auto joan::orig::Modifiers::sum(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_units4::H::mod_sum>(this, a0, a1)); }  // 0x497e10
auto joan::orig::Nation::rank(std::int32_t a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b98_02::H::rank>(this, a0)); }  // 0x498460
auto joan::orig::UnitData::count_in_grids(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_04::H::count_of>(this, a0)); }  // 0x4985f0
auto joan::orig::UnitData::count_of_kind(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_04::H::count_of_kind>(this, a0)); }  // 0x4987d0
auto joan::orig::UnitData::remove_type(std::int32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_04::H::remove_type>(this, a0, a1, a2)); }  // 0x498970
