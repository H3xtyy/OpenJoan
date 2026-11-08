// game/unknown_fx_spawn.cpp: functions whose original source file is unknown (formerly game/_unplaced/fx_spawn.cpp;
// re/attribution.csv: between ww/enparticle/peffect.cpp and ww/enparticle/peffecta.cpp). They move to their original
// file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using u32 = std::uint32_t;

using i32 = std::int32_t;

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

JOAN_COUNTER(c_list_get, "List::get");

struct H {
    void* list_get_(std::int32_t i) {  // 0x0041b3f0
        c_list_get.hit();
        if (i < 1 || i > jo::self<jo::List>(this)->last_ele) return nullptr;
        return *reinterpret_cast<void**>(jo::view<std::uint8_t*>(jo::self<jo::List>(this)->body_buf) + (i - 1) * jo::self<jo::List>(this)->ele_size);
    }
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("List::get", 0x0041b3f0, &H::list_get_);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

struct H {
    // ------------------------------------------------------------------ small accessors
    // this: the PemiRes (+4 scripts; 0x28-byte PemiScript records on x86, larger on x64: the effect pointer at +0x24)
    void* entry_28(std::int32_t i) { return reinterpret_cast<std::uint8_t*>(jo::self<jo::PemiRes>(this)->scripts) + i * static_cast<std::int32_t>(sizeof(jo::PemiScript)) - static_cast<std::int32_t>(sizeof(jo::PemiScript)); }                   // 0x0041b440
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Array28::get", 0x0041b440, &H::entry_28);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

struct H {
    // ------------------------------------------------------------------ 0x0041b470
    H* ctor_41b470() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0, 0);  // 0x005856a0 (pointers: 4 on x86)
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Obj::ctor_41b470", 0x0041b470, &H::ctor_41b470);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

struct H {
    void nop() {}  // 0x0041b510
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::nop", 0x0041b510, &H::nop);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::orig::helpers::memset_;

struct H {
    void* zero_ac() { memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::Cell))); return this; }  // 0x0041b5c0 (0xac on x86: a Cell, MapNode is its untyped view)
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Node41b5c0::ctor", 0x0041b5c0, &H::zero_ac);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

using joan::orig::helpers::game_delete;                                    // operator delete

struct H {
    void* sdd_41b5f0(std::uint32_t flags) {
        joan::orig::as_effect(this)->destruct();  // 0x0041a1f0
        if (flags & 1) game_delete(this);
        return this;
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("sdd_0041b5f0", 0x0041b5f0, &H::sdd_41b5f0);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b89_03.cpp (game/b89_03)
namespace {
namespace game_b89_03 {

using joan::img::obj;

using joan::hook::global;

using joan::img::str;

struct V3 {
    float x, y, z;
};

const char* rdata(img::Addr a) { return &img::global<const char>(a); }

static_assert(sizeof(V3) == 12);

constexpr img::Addr kGameUnits(d::g_units);
constexpr img::Addr kMounts(d::g_physic_sys);
constexpr img::Addr kFiles(d::g_pemi_res);
std::int32_t player_id() { return global<std::int32_t>(d::g_player_unit_recno); }
std::int32_t now() { return global<std::int32_t>(d::g_now); }

std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t* mount_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(img::obj(kMounts))->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x006af310

JOAN_COUNTER(c_spawn, "Spawner::try_spawn");

struct H {
    // 0x0041b640 (SEH frame only guards operator new + constructor)
    std::int32_t try_spawn(std::int32_t a1, jo::Scene* a2, std::int32_t slot, std::int32_t unit_id) {
        c_spawn.hit();
        std::uint8_t* const def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_refs(img::obj(d::g_model_refs))->get(joan::orig::arg::i16(a1)));  // 0x00416f20
        std::uint8_t* const u = unit_get(unit_id);
        V3 pos;
        joan::orig::as_unit(u)->position(&pos);  // 0x004fedb0
        if (static_cast<std::int32_t>(jo::as_model_slot(def)->emitter_default) == 1) {
            const std::uint32_t elapsed = static_cast<std::uint32_t>(now()) - jo::as_unit(u)->last_spawn_time;
            if (elapsed < jo::view<std::uint32_t>(jo::as_model_slot(def)->emitter_c[slot])) return 0;
        }
        if (jo::as_model_slot(def)->emitter_default != 0) {
            std::uint8_t* const r = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_waters(img::obj(d::g_water_array))->find_at(&pos, 0));  // 0x00469d10
            if (r != nullptr && !(pos.z >= jo::as_water(r)->box[2])) return 0;  // the region box z (+0xc), as animgrp.cpp
            if (static_cast<std::int32_t>(jo::as_unit(u)->id) != player_id()) return 0;
            void* const keys = img::obj(d::g_weather_system);
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_bytes(keys)->at(1)) != 0) return 0;  // 0x0041bf10
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_bytes(keys)->at(2)) != 0) return 0;
            const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_world_cell(jo::as_unit(u)->cell)->terrain_at(pos));  // 0x00560ee0
            if (joan::crt::stricmp(joan::orig::arg::str(name), joan::orig::arg::str(rdata(d::s_none))) == 0) return 0;     // _stricmp "none"
            alignas(4) std::uint8_t path[0x194];                       // String: char[0x190] + NUL at +0x190
            joan::orig::as_string(path)->construct();                  // 0x005a1170
            joan::orig::as_string(path)->assign(joan::orig::arg::str(&jo::as_model_slot(def)->emitter_names[slot * 0x21]));  // 0x005a1440
            joan::orig::as_string(path)->append(joan::orig::arg::str(rdata(d::s_us_9cf774)));  // 0x005a1550 "_"
            if (joan::orig::arg::as<const char*>(joan::orig::libc::strstr(joan::orig::arg::str(name), joan::orig::arg::str(rdata(d::s_mud)))) != nullptr) {  // strstr "mud"
                joan::orig::as_string(path)->append(joan::orig::arg::str(rdata(d::s_mud)));
            } else if (joan::orig::arg::as<const char*>(joan::orig::libc::strstr(joan::orig::arg::str(name), joan::orig::arg::str(rdata(d::s_snow)))) != nullptr) {  // "snow"
                joan::orig::as_string(path)->append(joan::orig::arg::str(rdata(d::s_snow)));
            } else {
                joan::orig::as_string(path)->append(joan::orig::arg::str(rdata(d::s_other)));  // "other"
            }
            joan::orig::as_string(path)->append(joan::orig::arg::str(rdata(d::s_txt)));  // ".txt"
            const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
            if (joan::orig::arg::as<void*>(joan::orig::as_name_list(img::obj(kFiles))->find_4c2060(joan::orig::arg::str(text), static_cast<std::int8_t>(joan::orig::arg::i32(1)))) == nullptr) return 0;  // 0x004c2060
            if (jo::as_unit(u)->actor != nullptr && jo::view<std::int32_t>(jo::as_actor(jo::as_unit(u)->actor)->record) != 0) {
                std::uint8_t* const m = mount_get(jo::view<std::int32_t>(jo::as_actor(jo::as_unit(u)->actor)->record));
                const float t = jo::view<float>(jo::as_shot(m)->state.cell) * global<float>(d::flt_1) / global<float>(d::flt_46_1) + global<float>(d::flt_0_5);
                if (!(t >= jo::as_shot(m)->state.pos_z)) return 0;
                if (jo::as_shot(m)->state.sub != nullptr && jo::view<std::int32_t>(jo::as_shot(m)->state.zone) != 0) {
                    std::uint8_t* const q = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_billboard(jo::view<void*>(jo::as_path_space(jo::as_shot(m)->state.sub)->zones))->item(joan::orig::arg::i32(// 0x00637da0
                                                                    jo::view<std::int32_t>(jo::as_shot(m)->state.zone))));
                    if (jo::as_pfd_zone(q)->name_id != 0) return 0;
                }
            }
        } else {
            if (jo::as_model_slot(def)->emitter_txt_ids[slot] == 0) return 0;
        }
        void* const memory = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::ModelObj)));  // operator new (0xa3 on x86)
        void* entry = memory != nullptr ? joan::orig::as_effect(memory)->construct() : nullptr;  // 0x0041a1b0
        joan::orig::as_effect(entry)->launch(joan::orig::arg::i16(a1), a2, joan::orig::arg::i32(slot), joan::orig::arg::i32(unit_id));  // 0x0041a2a0
        joan::orig::dynarray_b(this)->linkin(&entry);               // 0x005857a0
        const std::int32_t n = joan::orig::as_dynarray(this)->cursor();  // 0x00417520
        jo::self<jo::ModelObj>(entry)->kind = static_cast<std::int16_t>(n);  // +0: the registry index (the entries are ModelObj)
        return joan::orig::as_dynarray(this)->cursor();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_03", name, addr, fn)
M("Spawner::try_spawn", 0x0041b640, &H::try_spawn);
#undef M

}  // namespace game_b89_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::List::get_41b3f0(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::list_get_>(this, a0)); }  // 0x41b3f0
auto joan::orig::NameList::entry(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::entry_28>(this, a0)); }  // 0x41b440
auto joan::orig::ModelList::load_stub() -> void { joan::bind::member<&::game_b94_01::H::nop>(this); }  // 0x41b510
auto joan::orig::MapNode::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::zero_ac>(this)); }  // 0x41b5c0
auto joan::orig::Effect::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves81::H::sdd_41b5f0>(this, a0)); }  // 0x41b5f0
auto joan::orig::Spawner::try_spawn(std::int32_t a0, Scene* a1, std::int32_t a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b89_03::H::try_spawn>(this, a0, a1, a2, a3)); }  // 0x41b640
