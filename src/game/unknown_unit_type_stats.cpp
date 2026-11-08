// game/unknown_unit_type_stats.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/misc_00498b30.cpp; re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their
// original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
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

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::img::obj;


using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b104_03.cpp (game/b104_03)
namespace {
namespace game_b104_03 {

struct H {
    H* sdtor_498b30(u32 flags) {
        joan::orig::as_map_object(this)->destruct();  // 0x00415790
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_03", name, addr, fn)
B("Obj::scalar_dtor_498b30", 0x00498b30, &H::sdtor_498b30);
#undef B

}  // namespace game_b104_03
}  // namespace

// ---------------------------------------- from modules/game_b88_04.cpp (game/b88_04)
namespace {
namespace game_b88_04 {

void* const kArrays = obj(d::g_misc);  // 0x005911a0 / 0x005910f0 / 0x00591070

JOAN_COUNTER(c_toggle_station, "Nation::toggle_station");

struct H {
    // ------------------------------------------------------------ 0x00498b70 (ret 4)
    void toggle_station(std::uint8_t* key_src) {
        c_toggle_station.hit();
        std::int16_t key = jo::as_place(key_src)->index;  // ebp-0x8 (word); key_src: the Place
        std::int32_t pos = 0;                             // ebp-0xc (always written by 0x005911a0)
        // found: the 1-based record number or 0 (an index, not a pointer)
        const std::int32_t found = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(kArrays)->bin_search_5911a0(joan::orig::arg::mptr(static_cast<void*>(&key)), joan::orig::arg::str(jo::view<void*>(jo::self<jo::Nation>(this)->stations)), static_cast<std::int32_t>(jo::self<jo::Nation>(this)->station_count), 2, fn::code_value(fn::compare_short_4b5e10), &pos));  // 0x005911a0
        if (jo::as_place(key_src)->nation == static_cast<std::int32_t>(jo::self<jo::Nation>(this)->id))
        {
            if (found == 0) {
                jo::view<void*>(jo::self<jo::Nation>(this)->stations) = joan::orig::arg::as<void*>(joan::orig::grow_array(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Nation>(this)->stations)), joan::orig::arg::mptr(&jo::self<jo::Nation>(this)->station_cap), joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Nation>(this)->station_count) + 1)));
                joan::orig::as_misc(kArrays)->insert_array_rec_5910f0(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Nation>(this)->stations)), static_cast<std::int32_t>(jo::self<jo::Nation>(this)->station_count), 2, joan::orig::arg::i32(pos), joan::orig::arg::mptr(static_cast<void*>(&key)));  // 0x005910f0
                jo::self<jo::Nation>(this)->station_count = static_cast<std::int16_t>(jo::self<jo::Nation>(this)->station_count + 1);
            }
        } else if (found != 0) {
            joan::orig::as_misc(kArrays)->del_array_rec_591070(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Nation>(this)->stations)), static_cast<std::int32_t>(jo::self<jo::Nation>(this)->station_count), 2, found);  // 0x00591070
            jo::self<jo::Nation>(this)->station_count = static_cast<std::int16_t>(jo::self<jo::Nation>(this)->station_count - 1);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_04", name, addr, fn)
B("Nation::toggle_station", 0x00498b70, &H::toggle_station);
#undef B

}  // namespace game_b88_04
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

JOAN_COUNTER(c_release_498d30, "Obj::release_498d30");

struct H {
    // ------------------------------------------------------------------ 0x00498d30
    H* release_498d30() {
        c_release_498d30.hit();
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::MapObject>(this)->active)) == 0) return this;
        jo::self<jo::MapObject>(this)->active = 0;
        jo::self<jo::MapObject>(this)->refresh_depth = static_cast<u8>(jo::self<jo::MapObject>(this)->refresh_depth + 1);
        virtual_call<u32>(this, 0x38);
        for (i32 i = 0; i < 2; ++i) {
            if (joan::orig::as_dynarray(obj(d::g_particle_effect_array))->empty_at(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i + 2])) != 0) {
                continue;  // 0x0041bd90
            }
            u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_particle_effect_array))->ptr_at(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i + 2])));  // 0x0041bd60
            if (jo::self<jo::MapObject>(this)->actor != nullptr && jo::as_model_obj(p)->owner == jo::self<jo::MapObject>(this)->actor) {
                joan::orig::as_effect(p)->set_owner(joan::orig::arg::mptr(0), joan::orig::arg::mptr(0));  // 0x0041b0d0
            }
        }
        if (jo::self<jo::MapObject>(this)->actor_b != nullptr && jo::self<jo::MapObject>(this)->actor_b->scene != nullptr) {
            joan::orig::as_scene(jo::view<void*>(jo::self<jo::MapObject>(this)->actor_b->scene))->detach(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->actor_b));  // 0x005efdb0
        }
        if (jo::self<jo::MapObject>(this)->actor != nullptr && jo::self<jo::MapObject>(this)->actor->scene != nullptr) {
            joan::orig::as_scene(jo::view<void*>(jo::self<jo::MapObject>(this)->actor->scene))->detach(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->actor));  // 0x005efdb0
        }
        virtual_call<u32>(this, 0x14);
        jo::self<jo::MapObject>(this)->refresh_depth = static_cast<u8>(jo::self<jo::MapObject>(this)->refresh_depth - 1);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::release_498d30", 0x00498d30, &H::release_498d30);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b101_04.cpp (game/b101_04)
namespace {
namespace game_b101_04 {

using joan::hook::global;

JOAN_COUNTER(c_load_stats, "UnitType::load_stats");

struct H {
    // ------------------------------------------------------------------ 0x00498e90
    void set_stats(img::Addr a32, img::Addr a36, img::Addr a3a, img::Addr i3e, img::Addr i42, img::Addr i46, img::Addr a4a, img::Addr a4e,
                   img::Addr a52) {
        jo::self<jo::Missile>(this)->lifetime_ms = global<u32>(a32);
        jo::self<jo::Missile>(this)->max_range = global<u32>(a36);
        jo::self<jo::Missile>(this)->u32_3a = global<u32>(a3a);
        jo::view<float>(jo::self<jo::Missile>(this)->u32_3e) = static_cast<float>(global<i32>(i3e));
        jo::view<float>(jo::self<jo::Missile>(this)->u32_42) = static_cast<float>(global<i32>(i42));
        jo::view<float>(jo::self<jo::Missile>(this)->u32_46) = static_cast<float>(global<i32>(i46));
        jo::self<jo::Missile>(this)->straight_speed = global<u32>(a4a);
        jo::self<jo::Missile>(this)->u32_4e = global<u32>(a4e);
        jo::self<jo::Missile>(this)->u32_52 = global<u32>(a52);
    }
    void load_stats() {
        c_load_stats.hit();
        switch (jo::self<jo::Missile>(this)->state) {
            case 0: set_stats(d::g_a39d88, d::g_a39cb4, d::g_throw_arw_m, d::g_a39d74, d::g_a39d84, d::g_a39cc0, d::g_a39d60, d::g_a39d5c, d::g_throw_arw_gl); break;
            case 1: set_stats(d::g_a39db8, d::g_a39db4, d::g_throw_proj_m, d::g_a39dc0, d::g_a39da4, d::g_a39dc4, d::g_a39cd4, d::g_a39cd8, d::g_throw_proj_gl); break;
            case 2: set_stats(d::g_a39d7c, d::g_a39d3c, d::g_a39d50, d::g_a39d0c, d::g_a39d1c, d::g_a39d24, d::g_a39cf0, d::g_a39d18, d::g_throw_expl_gl); break;
            case 3: set_stats(d::g_a39d38, d::g_a39d08, d::g_throw_stm_m, d::g_a39d94, d::g_a39d10, d::g_a39cf4, d::g_a39dc8, d::g_a39d20, d::g_a39ce0); break;
            case 4: set_stats(d::g_a39da8, d::g_a39d80, d::g_throw_stc_m, d::g_a39d58, d::g_a39ce4, d::g_a39d68, d::g_a39cbc, d::g_a39cec, d::g_throw_stc_gl); break;
            case 5: set_stats(d::g_a39dcc, d::g_a39d70, d::g_throw_can_m, d::g_a39d00, d::g_a39d64, d::g_a39d54, d::g_a39d28, d::g_a39cc8, d::g_throw_can_gl); break;
            default: break;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_04", name, addr, fn)
B("UnitType::load_stats", 0x00498e90, &H::load_stats);
#undef B

}  // namespace game_b101_04
}  // namespace

// ---------------------------------------- from modules/game_b89_04.cpp (game/b89_04)
namespace {
namespace game_b89_04 {


std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }

void* const kSounds = obj(d::g_particle_effect_array);

JOAN_COUNTER(c_sound_play, "SoundSlots::play");

struct H {
    // ------------------------------------------------------------------------ 0x004991e0
    void sound_play(const void* a1, const void* a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5,
                    std::uint32_t a6) {
        c_sound_play.hit();
        std::int32_t slot = -1;  // ebp-0x8
        for (std::int32_t i = 0; i < 2; ++i) {
            if (joan::orig::as_dynarray(kSounds)->empty_at(joan::orig::arg::i32(sx16(jo::self<jo::MapObject>(this)->sound_slots[i + 2]))) != 0) {  // 0x0041bd90
                slot = i;
                break;
            }
            if (jo::self<jo::MapObject>(this)->actor != nullptr) {
                std::uint8_t* const s =
                    joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kSounds)->ptr_at(joan::orig::arg::i32(sx16(jo::self<jo::MapObject>(this)->sound_slots[i + 2]))));  // 0x0041bd60
                if (jo::as_model_obj(s)->owner != jo::self<jo::MapObject>(this)->actor) {
                    slot = i;
                    break;
                }
            }
        }
        if (slot == -1) {
            std::uint32_t oldest = 0;  // ebp-0x10
            for (std::int32_t i = 0; i < 2; ++i) {
                std::uint8_t* const s =
                    joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kSounds)->ptr_at(joan::orig::arg::i32(sx16(jo::self<jo::MapObject>(this)->sound_slots[i + 2]))));  // ebp-0x14
                if (oldest == 0 || jo::view<std::uint32_t>(jo::as_model_obj(s)->start_time) < oldest ||
                    joan::orig::arg::as<std::int32_t>(joan::orig::as_timer(s)->running()) != 0) {  // 0x0041b090
                    oldest = jo::view<std::uint32_t>(jo::as_model_obj(s)->start_time);
                    slot = i;
                }
            }
            void* const s = joan::orig::as_dynarray(kSounds)->ptr_at(joan::orig::arg::i32(sx16(jo::self<jo::MapObject>(this)->sound_slots[slot + 2])));
            joan::orig::as_effect(s)->set_owner(joan::orig::arg::mptr(std::uint32_t{0}), joan::orig::arg::mptr(std::uint32_t{0}));  // 0x0041b0d0
        }
        const std::uint32_t a6_pushed = (a5 & 0xffffff00u) | (a6 & 0xffu);  // mov cl, [ebp+0x1c] over ecx = a5
        const std::uint32_t id = thiscall<std::uint32_t>(
            fn::ModelList_add, kSounds, static_cast<void*>(jo::self<jo::MapObject>(this)->actor->scene), a1, a2,
            static_cast<void*>(jo::self<jo::MapObject>(this)->pos), a6_pushed, a3, a4, a5);  // 0x0041baf0
        jo::self<jo::MapObject>(this)->sound_slots[slot + 2] = static_cast<std::int16_t>(id);
        void* const s = joan::orig::as_dynarray(kSounds)->ptr_at(joan::orig::arg::i32(sx16(jo::self<jo::MapObject>(this)->sound_slots[slot + 2])));
        joan::orig::as_effect(s)->set_owner(joan::orig::arg::mptr(jo::self<jo::MapObject>(this)->actor), joan::orig::arg::mptr(std::uint32_t{0}));  // 0x0041b0d0
    }
};

#define B89(name, addr, fn) JOAN_REPLACE("game/b89_04", name, addr, fn)
B89("SoundSlots::play", 0x004991e0, &H::sound_play);
#undef B89

}  // namespace game_b89_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Nation::toggle_station(void* a0) -> void { joan::bind::member<&::game_b88_04::H::toggle_station>(this, a0); }  // 0x498b70
auto joan::orig::Missile::load_stats() -> void { joan::bind::member<&::game_b101_04::H::load_stats>(this); }  // 0x498e90
auto joan::orig::MapObject::play_sound(const void* a0, const void* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> void { joan::bind::member<&::game_b89_04::H::sound_play>(this, a0, a1, a2, a3, a4, a5); }  // 0x4991e0
