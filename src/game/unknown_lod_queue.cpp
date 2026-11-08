// game/unknown_lod_queue.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/misc_00490eb0.cpp; re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their
// original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
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

using joan::hook::global;

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;


}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

using i8 = std::int8_t;

struct Pair8 {  // 8-byte by-value argument built by 0x005a81a0
    u32 a, b;
};
static_assert(sizeof(Pair8) == 8, "8-byte by-value pair");

JOAN_COUNTER(c_lod_update, "LodQueue::update");

struct H {
    // ------------------------------------------------------------------ 0x00490eb0
    void lod_emit(u8* entry, i32 k) {
        const bool by_id = jo::as_top4(entry)->sample != nullptr;  // tested before the argument is built
        // (the entries are Se::entries: Top4 records, sample pointer +0, record number +4, count +6, levels +8 on x86)
        Pair8 item;
        joan::orig::as_volume(&item)->ctor(joan::orig::arg::mptr(entry + k * static_cast<i32>(sizeof(jo::Top4Entry)) + offsetof(jo::Top4, e0)));  // 0x005a81a0 copy ctor
        if (by_id) {
            joan::orig::as_audio(jo::view<void*>(jo::self<jo::LodQueue>(this)->audio))->play_memory(joan::orig::arg::mptr(jo::view<std::uintptr_t>(jo::as_top4(entry)->sample)), item.a, item.b);  // 0x0057b580 (item by value = 2 dwords)
        } else {
            joan::orig::as_audio(jo::view<void*>(jo::self<jo::LodQueue>(this)->audio))->play_resource(joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_top4(entry)->rec_no))), joan::orig::arg::i32(item.a), joan::orig::arg::i32(item.b));  // 0x0057b0d0 (item by value = 2 dwords)
        }
    }
    u8* lod_entry(i32 idx) { return jo::view<u8*>(jo::self<jo::LodQueue>(this)->entries) + idx * static_cast<i32>(sizeof(jo::Top4)); }  // 0x28 on x86
    u8& lod_flag(i32 idx) { return jo::self<jo::LodQueue>(this)->flags[idx]; }
    i16& lod_list(i32 j) { return jo::view<i16>(jo::self<jo::LodQueue>(this)->arr_10[j]); }
    u32 lod_update() {
        c_lod_update.hit();
        if (jo::self<jo::LodQueue>(this)->enabled == 0 || static_cast<i8>(global<u8>(d::g_a57ff8)) == 0) {
            joan::orig::as_sound_effects(this)->clear_recent();  // 0x0059e220
            return 0;  // void callee; the one caller (misc_004994b0.cpp, flush()) discards the result
        }
        i32 budget = joan::orig::arg::as<i32>(joan::orig::as_audio(jo::view<void*>(jo::self<jo::LodQueue>(this)->audio))->reap_buffers());  // 0x0057b9c0
        i32 active = 0;
        i32 total = 0;
        jo::self<jo::LodQueue>(this)->active_count = 0;
        {
            u8* e = jo::view<u8*>(jo::self<jo::LodQueue>(this)->entries);
            for (i32 i = 0; i < jo::self<jo::LodQueue>(this)->entry_count; ++i, e += sizeof(jo::Top4)) {
                if (jo::as_top4(e)->count > 0) {
                    ++active;
                    total += jo::as_top4(e)->count;
                }
                if (jo::as_top4(e)->count > 0 || static_cast<i8>(lod_flag(i)) != 0) {
                    if (jo::self<jo::LodQueue>(this)->active_count < 0x20) {
                        lod_list(jo::self<jo::LodQueue>(this)->active_count) = static_cast<i16>(i);
                        jo::self<jo::LodQueue>(this)->active_count += 1;
                    }
                }
            }
        }
        if (total <= budget) {
            for (i32 j = 0; j < jo::self<jo::LodQueue>(this)->active_count; ++j) {
                const i32 idx = lod_list(j);
                u8* const e = lod_entry(idx);
                lod_flag(idx) = 0;
                for (i32 k = jo::as_top4(e)->count - 1; k >= 0; --k) {
                    lod_emit(e, k);
                    lod_flag(idx) = static_cast<u8>(lod_flag(idx) + 1);
                    --budget;
                }
            }
        } else if (active <= budget) {
            for (i32 j = 0; j < jo::self<jo::LodQueue>(this)->active_count; ++j) {
                const i32 idx = lod_list(j);
                u8* const e = lod_entry(idx);
                lod_flag(idx) = 0;
                if (jo::as_top4(e)->count > 0) {
                    const i32 k = joan::orig::arg::as<i32>(joan::orig::as_table(e)->argmax());  // 0x004554e0
                    lod_emit(e, k);
                    lod_flag(idx) = static_cast<u8>(lod_flag(idx) + 1);
                }
            }
        } else {
            i32 j = 0;
            while (j < jo::self<jo::LodQueue>(this)->active_count && jo::self<jo::LodQueue>(this)->u32_0 > lod_list(j)) ++j;
            if (j >= jo::self<jo::LodQueue>(this)->active_count) j = 0;
            for (i32 m = 0; budget > 0 && m < jo::self<jo::LodQueue>(this)->active_count; ++m) {
                ++j;
                if (j >= jo::self<jo::LodQueue>(this)->active_count) j = 0;
                const i32 idx = lod_list(j);
                u8* const e = lod_entry(idx);
                if (jo::as_top4(e)->count > 0 && static_cast<i8>(lod_flag(idx)) == 0) {
                    const i32 k = joan::orig::arg::as<i32>(joan::orig::as_table(e)->argmax());  // 0x004554e0
                    lod_emit(e, k);
                    lod_flag(idx) = static_cast<u8>(lod_flag(idx) + 1);
                    --budget;
                    jo::self<jo::LodQueue>(this)->u32_0 = idx;
                } else {
                    lod_flag(idx) = 0;
                }
            }
        }
        joan::orig::as_sound_effects(this)->clear_recent();  // 0x0059e220
        return 0;  // void callee; the one caller discards the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("LodQueue::update", 0x00490eb0, &H::lod_update);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b104_03.cpp (game/b104_03)
namespace {
namespace game_b104_03 {

using joan::img::obj;

using joan::call::import_call;

struct FVec3 {
    float x, y, z;
};
static_assert(sizeof(FVec3) == 12, "FVec3");

void* const kObjects = obj(d::g_items);
void* const kModels = obj(d::g_physic_sys);

u32 now() { return global<u32>(d::g_now); }

JOAN_COUNTER(c_expire_4913c0, "List::expire_4913c0");

struct H {
    // ------------------------------------------------------------------ 0x004913c0
    void remove_4913c0(u8* e) { joan::orig::as_list(this)->remove_delete_4ea9c0(joan::orig::arg::u32(jo::as_missile(e)->index)); }  // 0x004ea9c0
    i32 expire_4913c0() {
        c_expire_4913c0.hit();
        i32 i = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
        for (; i > 0; --i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->empty_at_f(joan::orig::arg::i32(i))) != 0) continue;  // 0x00485760
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->get_f(joan::orig::arg::i32(i)));       // 0x00485730
            if (jo::as_missile(e)->launch_time == 0) continue;
            if (static_cast<i32>(jo::as_missile(e)->stopped) != 0) {
                remove_4913c0(e);
                continue;
            }
            if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(jo::as_missile(e)->object_id)) != 0) {  // 0x00425d50
                remove_4913c0(e);
                continue;
            }
            u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_players(kModels)->list.ptr_at(joan::orig::arg::i32(jo::as_missile(e)->model_id)));  // 0x006af310
            if (u == nullptr) {
                remove_4913c0(e);
                continue;
            }
            u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(jo::as_missile(e)->object_id)));  // 0x00425d20
            FVec3 p;
            std::memcpy(&p, img::obj(d::g_pos), sizeof p);
            joan::orig::as_map_object(o)->position(joan::orig::arg::mptr(static_cast<void*>(&p)));  // 0x00499930 (writes all 3)
            p.z = 0.0f;
            FVec3 tmp;
            FVec3* const d = joan::orig::arg::as<FVec3*>(joan::orig::vec_sub(&tmp, &p, static_cast<void*>(jo::as_missile(e)->origin)));  // 0x0060cc90
            FVec3 diff = *d;
            const double len = import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&diff));  // v3d length
            if (!(static_cast<double>(jo::view<i32>(jo::as_missile(e)->max_range)) >= len)) {  // fild / fcompp / test ah,1
                remove_4913c0(e);
                continue;
            }
            if (jo::as_missile(e)->expired == 0 && static_cast<i32>(jo::view<i16>(jo::as_shot(u)->hit)) != 0) {
                remove_4913c0(e);
            } else if (jo::as_missile(e)->expired == 0 && jo::as_missile(e)->launch_time + jo::as_missile(e)->lifetime_ms < now()) {
                remove_4913c0(e);
            } else {
                joan::orig::as_marker(e)->update_missile();  // 0x004fcc00
            }
        }
        return i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_03", name, addr, fn)
B("List::expire_4913c0", 0x004913c0, &H::expire_4913c0);
#undef B

}  // namespace game_b104_03
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

using joan::call::cdecl_call;

// ---------------------------------------------------------------------- static initialisers
template <fn::Code Worker>
void JOAN_CDECL static_init() { cdecl_call<void>(Worker); }

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("init_4915d0", 0x004915d0, &static_init<fn::init_a39c54>);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SoundEffects::flush() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_18::H::lod_update>(this)); }  // 0x490eb0
