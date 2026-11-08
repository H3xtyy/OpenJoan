// game/unknown_animscr_fx.cpp: functions whose original source file is unknown (formerly game/_unplaced/animscr_fx.cpp;
// re/attribution.csv: between ww/animscr.cpp and ww/enparticle/peffect.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../runtime/hitch.hpp"

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

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::obj;

using joan::hook::global;

using joan::call::cdecl_call;
using joan::img::str;

struct V3 {
    float x, y, z;
};

using joan::orig::helpers::game_new;  // operator new


}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

using i16 = std::int16_t;


void* const kGroups = obj(d::g_particle_effect_array);
void* const kTypes = obj(d::g_item_res);

void* type_record(i16 type) { return joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(type))); }  // 0x00425ce0
i32 group_free(i32 id) { return joan::orig::as_dynarray(kGroups)->empty_at(joan::orig::arg::i32(id)); }  // 0x0041bd90
void* delete_obj(void* p) { return joan::orig::arg::as<void*>(joan::orig::as_scene_object(p)->destroy(1u)); }           // 0x00420840

JOAN_COUNTER(c_release_419950, "Unit::release_419950");

struct H {
    // Frees the two group ids at +0x96 that are still in use.
    void free_groups_96() {
        for (i32 i = 0; i < 2; ++i) {
            if (group_free(jo::self<jo::MapObject>(this)->sound_slots[i]) != 0) continue;
            joan::orig::as_effect_registry(kGroups)->remove(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i]));  // 0x0041bbf0
            jo::self<jo::MapObject>(this)->sound_slots[i] = 0;
        }
    }

    // ---------------------------------------------------------------- 0x00419950
    u32 release_419950() {
        c_release_419950.hit();
        void* const type = type_record(jo::self<jo::MapObject>(this)->type_id);
        free_groups_96();
        if (jo::self<jo::MapObject>(this)->actor_b != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::MapObject>(this)->actor_b);
            if (p) delete_obj(p);
            jo::self<jo::MapObject>(this)->actor_b = nullptr;
        }
        if (jo::self<jo::MapObject>(this)->actor != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::MapObject>(this)->actor);
            if (p) delete_obj(p);
            jo::self<jo::MapObject>(this)->actor = nullptr;
            joan::orig::as_res(type)->release_slot(joan::orig::arg::i32(jo::self<jo::MapObject>(this)->busy), 0);  // 0x004a3e30
        }
        joan::orig::as_map_object(this)->release_light();                    // 0x0049c1a0
        return joan::orig::arg::as<u32>(joan::orig::as_map_object(this)->release_marker());        // 0x0049c920
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Unit::release_419950", 0x00419950, &H::release_419950);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

// g_out = g_in / g_div (fld dword / fdiv dword / fstp dword).
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a59778() { div_global(d::g_a59778, d::g_a4f81c, d::flt_3); }  // 0x00419a90

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("init_a59778", 0x00419a90, &init_a59778);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

using joan::orig::helpers::memset_;

struct V3raw {  // 12 bytes passed by value: three floats (System V: XMM registers, like the callers)
    float a, b, c;
};

using joan::orig::helpers::strcpy_;
char* strcat_(char* d, const char* s) { return std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

void* const kAnims = obj(d::g_anim_res_mgr);   // AnimResMgr
void* const kModels = obj(d::g_model_res_mgr);  // ModelResMgr

void* anim_entry(std::int32_t id) { return joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(kAnims)->slot_4174f0(joan::orig::arg::u32(id))); }  // 0x004174f0
std::uint8_t* model_info(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_res_mgr(kModels)->entry(joan::orig::arg::u32(id))); }  // 0x0041a180

JOAN_COUNTER(c_slot_stream, "ModelSlot::stream");
JOAN_COUNTER(c_slot_acquire, "ModelSlot::acquire");
JOAN_COUNTER(c_add_default, "Actors::add_default");

struct H {
    // ---- small ---------------------------------------------------------------------------
    std::int32_t slot_idle() { return jo::self<jo::ModelSlot>(this)->refs == 0 ? 1 : 0; }  // 0x00419ff0
    void* modelobj_ctor() {  // 0x0041a1b0
        joan::orig::as_effect_state(reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx))->construct();  // 0x006758f0
        memset_(this, 0, static_cast<std::uint32_t>(offsetof(jo::ModelObj, fx)));  // 0x2b on x86
        return this;
    }
    std::int32_t slot_add_ref() {  // 0x00419cf0
        jo::self<jo::ModelSlot>(this)->handle = jo::self<jo::ModelSlot>(this)->handle + 1;
        if (jo::self<jo::ModelSlot>(this)->handle == 1) {
            const std::int32_t id = jo::self<jo::ModelSlot>(this)->attach_model;
            if (id != 0) jo::self<jo::ModelSlot>(this)->refs = 1;
        }
        return jo::self<jo::ModelSlot>(this)->handle;
    }

    // ---- 0x0041a650 Actors::add_default(V3, a, b, c) ------------------------------------------
    std::int32_t add_default(V3raw pos, std::uint32_t a, std::uint32_t b, std::uint32_t c) {
        c_add_default.hit();
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // 0x10c on x86
        void* p = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        joan::orig::dynarray_b(this)->linkin(static_cast<void*>(&p));                   // 0x005857a0
        jo::as_scene_object(p)->def = static_cast<jo::ActorDef*>(jo::view<void*>(jo::self<jo::Actors>(this)->default_def));
        jo::as_scene_object(p)->anim = static_cast<jo::AnimDef*>(jo::view<void*>(jo::self<jo::Actors>(this)->default_anim));
        joan::orig::as_scene_object(p)->init();  // 0x005aab70
        joan::orig::as_actor(p)->start_anim(joan::orig::arg::mptr(jo::as_scene_object(p)->anim), str(d::s_default_9d3d60), joan::orig::arg::u32(global<std::uint32_t>(d::g_now)), 1u, 0u, 0u);  // 0x005abe30
        joan::orig::as_actor(p)->set_position_5ac310(joan::orig::arg::mptr(static_cast<void*>(&pos)));  // 0x005ac310
        joan::orig::as_scene_object(p)->set_angles(joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c));                    // 0x005acaf0
        return joan::orig::as_dynarray(this)->cursor();  // 0x00417520
    }

    // Shared by 0x00419d50 / 0x00419ae0: the 'A' (animated) model's animation entry.
    void load_anim_entry(std::int32_t model) {
        jo::self<jo::ModelSlot>(this)->attach_anim = joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_res_mgr(kAnims)->add_slot(0u));  // 0x00417470
        char buf[0x104];  // ebp-0x10c
        strcpy_(buf, jo::as_model_entry(model_info(model))->dir);
        strcat_(buf, str(d::s_ANIM));  // "ANIM\"
        const char* const name = jo::as_model_entry(model_info(model))->file;
        void* const entry = anim_entry(jo::self<jo::ModelSlot>(this)->attach_anim);
        joan::orig::as_path(entry)->init(joan::orig::arg::str(static_cast<void*>(buf)), joan::orig::arg::str(name));  // 0x00418550
    }
    void set_model_params(std::int32_t model) {
        if (jo::view<std::int8_t>(jo::as_model_entry(model_info(model))->type) != 0x41)
            joan::orig::as_model_res_mgr(kModels)->set_params(joan::orig::arg::i16(static_cast<std::int16_t>(model)), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::int8_t>(0))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::int8_t>(0))), joan::orig::arg::i16(static_cast<std::int16_t>(0)));  // 0x004b0ec0
        else
            joan::orig::as_model_res_mgr(kModels)->set_params(joan::orig::arg::i16(static_cast<std::int16_t>(model)), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::int8_t>(0))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::int8_t>(jo::self<jo::ModelSlot>(this)->attach_anim != 0 ? 1 : 0))), joan::orig::arg::i16(static_cast<std::int16_t>(jo::self<jo::ModelSlot>(this)->attach_anim)));
    }

    // ---- 0x00419d50 ModelSlot::stream -----------------------------------------------------------
    void slot_stream() {
        c_slot_stream.hit();
        if (jo::self<jo::ModelSlot>(this)->refs == 0) return;
        const std::int32_t model = jo::self<jo::ModelSlot>(this)->attach_model;
        if (jo::self<jo::ModelSlot>(this)->refs == 1) {
            if (jo::view<std::int8_t>(jo::as_model_entry(model_info(model))->type) == 0x41) {
                if (jo::self<jo::ModelSlot>(this)->attach_anim == 0) load_anim_entry(model);
                joan::orig::as_anim_res_mgr(kAnims)->add_ref(joan::orig::arg::i16(static_cast<std::int16_t>(jo::self<jo::ModelSlot>(this)->attach_anim)));  // 0x00417af0
                jo::self<jo::ModelSlot>(this)->refs = 2;
            } else {
                jo::self<jo::ModelSlot>(this)->attach_anim = 0;
                jo::self<jo::ModelSlot>(this)->refs = 3;
            }
            return;
        }
        if (jo::self<jo::ModelSlot>(this)->refs == 2) {
            joan::orig::as_anim_res_mgr(kAnims)->stream(joan::orig::arg::i16(static_cast<std::int16_t>(jo::self<jo::ModelSlot>(this)->attach_anim)));  // 0x00417b60
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_res_mgr(kAnims)->idle(joan::orig::arg::i16(static_cast<std::int16_t>(jo::self<jo::ModelSlot>(this)->attach_anim)))) != 0)  // 0x00417db0
                jo::self<jo::ModelSlot>(this)->refs = 3;
            return;
        }
        if (jo::self<jo::ModelSlot>(this)->refs == 3) {
            set_model_params(model);
            joan::orig::as_model_res_mgr(kModels)->add_ref(joan::orig::arg::i16(static_cast<std::int16_t>(model)));  // 0x00437af0
            jo::self<jo::ModelSlot>(this)->refs = 4;
            return;
        }
        if (jo::self<jo::ModelSlot>(this)->refs != 4) return;
        joan::orig::as_model_res_mgr(kModels)->update_anim(joan::orig::arg::i16(static_cast<std::int16_t>(model)));  // 0x004d2b10
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_model_res_mgr(kModels)->ready(joan::orig::arg::i16(static_cast<std::int16_t>(model)))) != 0) jo::self<jo::ModelSlot>(this)->refs = 0;  // 0x004b1640
    }

    // ---- 0x00419ae0 ModelSlot::acquire ----------------------------------------------------------
    std::int32_t slot_acquire() {
        c_slot_acquire.hit();
        jo::self<jo::ModelSlot>(this)->handle = jo::self<jo::ModelSlot>(this)->handle + 1;
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_model_slot(this)->idle()) == 0) {  // 0x00419ff0
            reinterpret_cast<std::uint32_t(JOAN_STDCALL*)(std::uint32_t, std::int32_t)>(global<std::uintptr_t>(d::iat_KERNEL32_SleepEx))(5, 1);  // SleepEx
            joan::orig::as_model_slot(this)->stream();  // 0x00419d50
        }
        if (jo::self<jo::ModelSlot>(this)->handle == 1) {
            const std::int32_t model = jo::self<jo::ModelSlot>(this)->attach_model;
            if (model != 0) {
                if (jo::view<std::int8_t>(jo::as_model_entry(model_info(model))->type) == 0x41) {
                    if (jo::self<jo::ModelSlot>(this)->attach_anim == 0) load_anim_entry(model);
                    joan::orig::as_anim_set(kAnims)->acquire_417540(joan::orig::arg::u32(static_cast<std::int16_t>(jo::self<jo::ModelSlot>(this)->attach_anim)));  // 0x00417540
                } else {
                    jo::self<jo::ModelSlot>(this)->attach_anim = 0;
                }
                set_model_params(model);
                joan::orig::as_model_res(kModels)->acquire(joan::orig::arg::i16(static_cast<std::int16_t>(model)));  // 0x004b0f40
            }
        }
        return jo::self<jo::ModelSlot>(this)->handle;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("ModelSlot::stream", 0x00419d50, &H::slot_stream);
B("ModelSlot::acquire", 0x00419ae0, &H::slot_acquire);
B("Actors::add_default", 0x0041a650, &H::add_default);
B("ModelSlot::add_ref", 0x00419cf0, &H::slot_add_ref);
B("ModelObj::ctor", 0x0041a1b0, &H::modelobj_ctor);
B("ModelSlot::idle", 0x00419ff0, &H::slot_idle);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

struct H {
    // 0x00419c80 (the GameSet at 0xa56b18)
    H* ctor_419c80() {
        joan::orig::as_anim_set(&jo::self<jo::GameSet>(this)->set_res)->construct();            // 0x00417980
        joan::orig::as_database(&jo::self<jo::GameSet>(this)->set_db)->construct(0, 0, 0);  // 0x00580730
        jo::self<jo::GameSet>(this)->init_flag = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("Obj::ctor_419c80", 0x00419c80, &H::ctor_419c80);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

struct H {
    u32 dtor_419f70() {
        joan::orig::as_game_set(this)->deinit_58ae30();                          // 0x0058ae30
        joan::orig::as_database(&jo::self<jo::GameSet>(this)->set_db)->destruct();        // 0x00580810
        return joan::orig::arg::as<u32>(joan::orig::as_res_idx_file(&jo::self<jo::GameSet>(this)->set_res)->dtor_47cac0());      // 0x0047cac0
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::dtor_419f70", 0x00419f70, &H::dtor_419f70);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01 {

using joan::orig::helpers::s16;

using joan::orig::helpers::stricmp_;

// x87 "fcomp; test ah,0x40; je" falls through for equal or unordered.
bool equal_or_unordered(float a, float b) { return !(a < b) && !(a > b); }

JOAN_COUNTER(c_name_find, "NameTable::find");
JOAN_COUNTER(c_heading, "Object::heading_to(0x41af80)");

struct H {
    // ------------------------------------------------------------ 0x0041a0e0 (ret 4)
    std::int32_t name_find(const char* name) {
        c_name_find.hit();
        for (std::int32_t i = 0; i < s16(jo::view<std::uint16_t>(jo::self<jo::FxNameTable>(this)->count)); ++i) {
            const char* const rec = reinterpret_cast<const char*>(
                jo::view<std::uint8_t*>(jo::self<jo::FxNameTable>(this)->records) + static_cast<std::int32_t>(static_cast<std::uint32_t>(i) * 0x21bu) + 2);
            if (stricmp_(rec, name) == 0) return i + 1;
        }
        return 0;
    }

    // ------------------------------------------------------------ 0x0041af80 (ret 4)
    float heading_to(const float* point) {
        c_heading.hit();
        const joan::hook::x87::Fpu fpu{true};
        const float facing = joan::orig::arg::as<float>(joan::orig::as_map_object(this)->yaw());  // 0x00499e20 (fld [this+0x62])
        const float degrees = static_cast<float>(fpu.div(fpu.mul(facing, 180.0f), global<double>(d::dbl_3_141592653589793)));
        float pos[3];                                    // ebp-0x18
        joan::orig::as_map_object(this)->position(joan::orig::arg::mptr(pos));  // 0x00499930
        if (equal_or_unordered(pos[0], point[0]) && equal_or_unordered(pos[1], point[1])) return 0.0f;
        std::uint32_t dir = 0;                  // ebp-0x1c (0x00577190 leaves it; written by 0x00577b90)
        joan::orig::as_angle(&dir)->construct_577190();      // 0x00577190
        const double dy = static_cast<double>(point[1] - pos[1]);
        const double dx = static_cast<double>(point[0] - pos[0]);
        joan::orig::as_angle(&dir)->from_xy(joan::orig::arg::f64(dx), joan::orig::arg::f64(dy));                // 0x00577b90
        const float unused = joan::orig::arg::as<float>(joan::orig::as_angle(&dir)->degrees());  // 0x00577450 (stored to ebp-0x20)
        (void)unused;
        std::uint32_t face = 0;                       // ebp-0x24
        joan::orig::as_angle(&face)->construct_577190();           // 0x00577190
        joan::orig::as_angle(&face)->set_degrees(joan::orig::arg::f32(degrees));  // 0x005773e0
        std::uint32_t tmp = 0;                        // ebp-0x30
        std::uint32_t diff = *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_turn(&face)->sub(&tmp, &dir));  // 0x005775b0
        float r = joan::orig::arg::as<float>(joan::orig::as_angle(&diff)->degrees());  // 0x00577450
        if (r > 180.0f) r = r - 360.0f;                // test ah,0x41 jne
        if (!(r >= -180.0f)) r = r + 360.0f;           // test ah,1 je (less or unordered adds)
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("NameTable::find", 0x0041a0e0, &H::name_find);
B("Object::heading_to_62", 0x0041af80, &H::heading_to);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01 {

struct H {
    u32 zero_41a160(i32) { return 0; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("Obj::zero_41a160", 0x0041a160, &H::zero_41a160);
#undef B

}  // namespace game_b105_01
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    std::uint8_t* elem_41a180(std::uint32_t i) {  // 0x0041a180
        return reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelRes>(this)->entries[static_cast<std::int32_t>(i) - 1]);
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("elem_0041a180", 0x0041a180, &H::elem_41a180);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

struct H {

    void dtor_41a1f0() {
        joan::orig::as_effect(this)->clear();           // 0x0041a260
        joan::orig::as_zones(&jo::self<jo::ModelObj>(this)->fx)->dtor();     // 0x00675930
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Obj41a1f0::~Obj41a1f0", 0x0041a1f0, &H::dtor_41a1f0);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

// ---------------------------------------------------------------------------- static inits
void JOAN_CDECL init_a5977c() { global<float>(d::g_a5977c) = global<float>(d::g_a4f81c) / global<float>(d::flt_4); }

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("init_a5977c", 0x0041a8d0, &init_a5977c);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

// 0x0041a900 / 0x00425cb0 compare two 0x28-byte camera states.
std::int32_t JOAN_CDECL state_differs(const void* a, const void* b) { return joan::orig::arg::as<std::int32_t>(joan::orig::libc::memcmp(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b), 0x28u)) != 0 ? 1 : 0; }

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("state_differs", 0x0041a900, &state_differs);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------ 0x0041aae0
    void dtor_41aae0() {
        joan::orig::as_panel(this)->reset_6e4720();            // 0x006e4720
        joan::orig::as_widget(self() + 4)->dtor_6294c0();      // 0x006294c0
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::~Obj_41aae0", 0x0041aae0, &H::dtor_41aae0);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

using joan::call::import_call;

V3 load3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}
void store3(void* p, const V3& v) { std::memcpy(p, &v, sizeof v); }

std::uint32_t now() { return global<std::uint32_t>(d::g_now); }  // game clock (ms)
void* a48ccc() { return global<void*>(d::g_camera); }

constexpr fn::Code kNodeAdd = fn::SceneSector_add_actor;     // 0x005efc50

JOAN_COUNTER(c_effect, "Effect::update");

struct H {
    void state_start() { jo::self<jo::EffectState>(this)->started_ = 1; }      // 0x0041b170
    void state_clear() { jo::self<jo::EffectState>(this)->started_ = 0; }      // 0x0041b1c0
    std::uint8_t state_started() { return jo::self<jo::EffectState>(this)->started_; }  // 0x0041b1e0
    void* state_object() { return jo::view<void*>(jo::self<jo::EffectState>(this)->object_); }     // 0x0041b280
    void state_set_position(V3 v) {                     // 0x0041b2a0: previous = current, current = v
        std::memcpy(jo::self<jo::EffectState>(this)->prev_pos, jo::self<jo::EffectState>(this)->pos, 12);
        store3(jo::self<jo::EffectState>(this)->pos, v);
    }
    void state_stop() {                                 // 0x0041b190
        joan::orig::as_effect_state(this)->clear();  // 0x0041b1c0
        joan::orig::as_effect_state(this)->parts_stop();  // 0x00677120
    }

    // 0x0041ab50 Effect::update: starts at +0x1b, ends at +0x23 (0 = never), follows the
    // owner (+0x16 bone or (+0x12)->+0x8c) with offset +2 by kind +0x2d, advances the
    // parts (0x00675e90) by the elapsed time. Returns 0 when the effect is finished.
    std::int32_t effect_update() {
        joan::hitch::Span hitch_span("effect_update");  // [Debug] HitchLog
        c_effect.hit();
        if (global<std::uint32_t>(d::g_9f6a80) == 0) return 1;
        std::uint8_t* const e = reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx);
        const joan::hook::x87::Fpu fpu{true};
        auto seconds = [&fpu](std::uint32_t ms) { return static_cast<float>(fpu.div(static_cast<double>(ms), 1000.0)); };
        auto object = [e]() { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_effect_state(e)->object()); };  // 0x0041b280
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_effect_state(e)->started()) == 0) {  // 0x0041b1e0
            if (now() < jo::view<std::uint32_t>(jo::self<jo::ModelObj>(this)->start_time)) return 1;
            joan::orig::as_effect_state(e)->start();  // 0x0041b170
            joan::orig::as_effect_state(e)->parts_start();  // 0x00677250
        }
        if (now() > jo::view<std::uint32_t>(jo::self<jo::ModelObj>(this)->end_time) && jo::view<std::uint32_t>(jo::self<jo::ModelObj>(this)->end_time) != 0) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_effect_state(e)->parts_alive()) != 0) {  // 0x006772f0
                joan::orig::as_effect_state(e)->parts_hide();                       // 0x006771c0
            } else {
                joan::orig::as_effect_state(e)->stop();  // 0x0041b190
                return 0;
            }
        }
        if (static_cast<std::int8_t>(jo::self<jo::ModelObj>(this)->attached) != 0) {
            void* const bone = jo::view<void*>(jo::self<jo::ModelObj>(this)->bone) != nullptr ? jo::view<void*>(jo::self<jo::ModelObj>(this)->bone) : jo::view<void*>(jo::self<jo::ModelObj>(this)->owner->bone);
            void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(bone)->matrix());
            V3 v = load3(jo::self<jo::ModelObj>(this)->pos);
            import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(&v), m);  // point_transform (in place)
            const std::int32_t kind = static_cast<std::int8_t>(jo::self<jo::ModelObj>(this)->fx.visible);
            if (kind == 1 || kind == 3 || kind == 4) {
                joan::orig::as_actor(object())->set_position_5ac310(joan::orig::arg::mptr(static_cast<void*>(&v)));  // 0x005ac310
            } else if (kind == 2) {
                joan::orig::as_actor(object())->set_position_5ac310(joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::ModelObj>(this)->pos)));
                if (jo::view<void*>(jo::self<jo::ModelObj>(this)->bone) != nullptr)
                    joan::orig::as_bone(jo::as_actor(object())->bone)->set_parent(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ModelObj>(this)->bone)));  // 0x0060ec70
                else
                    joan::orig::as_bone(jo::as_actor(object())->bone)->set_parent(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ModelObj>(this)->owner->bone)));
            } else if (kind == 0) {
                jo::self<jo::ModelObj>(this)->attached = 0;
            }
            joan::orig::as_effect_state(e)->set_position(v);  // 0x0041b2a0
            std::uint8_t* const obj = object();
            if (jo::as_actor(obj)->scene != jo::self<jo::ModelObj>(this)->owner->scene)
                thiscall(kNodeAdd, jo::view<void*>(jo::self<jo::ModelObj>(this)->owner->scene), static_cast<void*>(obj));
        }
        bool advance = true;  // 0x0041adda (else 0x0041ae93)
        if (jo::self<jo::ModelObj>(this)->u32_27 != 0) {
            if (object() != nullptr && joan::orig::arg::as<std::int32_t>(joan::orig::as_actor(object())->visible_5ab470(joan::orig::arg::mptr(a48ccc()), 1)) != 0) {  // 0x005ab470
                advance = true;
            } else if (object() == nullptr) {
                advance = false;
            } else {
                advance = jo::view<std::uint32_t>(jo::as_actor(object())->plane->vertex_count) == 0;
            }
        }
        if (advance) {
            if (jo::self<jo::ModelObj>(this)->u32_27 == 0)
                joan::orig::as_effect_state(e)->update_parts(joan::orig::arg::f32(seconds(global<std::uint32_t>(d::g_frame_step_ms))), joan::orig::arg::mptr(a48ccc()));  // 0x00675e90
            else if (jo::self<jo::ModelObj>(this)->end_time > 0)
                joan::orig::as_effect_state(e)->update_parts(joan::orig::arg::f32(seconds(now() - jo::self<jo::ModelObj>(this)->u32_27)), joan::orig::arg::mptr(a48ccc()));
            else
                joan::orig::as_effect_state(e)->update_parts(joan::orig::arg::f32(seconds(global<std::uint32_t>(d::g_frame_step_ms))), joan::orig::arg::mptr(a48ccc()));
            jo::self<jo::ModelObj>(this)->u32_27 = now();
        } else if (now() - jo::self<jo::ModelObj>(this)->u32_27 >
                   static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::ModelObj>(this)->kind) * 100 + 10000)) {
            if (now() <= jo::view<std::uint32_t>(jo::self<jo::ModelObj>(this)->end_time))
                joan::orig::as_effect_state(e)->update_parts(joan::orig::arg::f32(seconds(global<std::uint32_t>(d::g_frame_step_ms))), joan::orig::arg::mptr(0u));
            else
                joan::orig::as_effect_state(e)->update_parts(joan::orig::arg::f32(seconds(now() - jo::self<jo::ModelObj>(this)->u32_27)), joan::orig::arg::mptr(0u));
            jo::self<jo::ModelObj>(this)->u32_27 = now();
        }
        if (now() <= jo::view<std::uint32_t>(jo::self<jo::ModelObj>(this)->end_time) || jo::view<std::uint32_t>(jo::self<jo::ModelObj>(this)->end_time) == 0) return 1;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_effect_state(e)->parts_alive()) != 0) {
            joan::orig::as_effect_state(e)->parts_hide();
            return 1;
        }
        joan::orig::as_effect_state(e)->stop();
        return 0;
    }
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
#define FX4(name, addr, fn) JOAN_REPLACE("game/fx4", name, addr, fn)
FX1("EffectState::start", 0x0041b170, &H::state_start);
FX1("EffectState::clear", 0x0041b1c0, &H::state_clear);
FX1("EffectState::started", 0x0041b1e0, &H::state_started);
FX1("EffectState::object", 0x0041b280, &H::state_object);
FX1("EffectState::set_position", 0x0041b2a0, &H::state_set_position);
FX1("EffectState::stop", 0x0041b190, &H::state_stop);
FX4("Effect::update", 0x0041ab50, &H::effect_update);
#undef FX1
#undef FX4

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

struct H {
    // 0x0041b090
    std::uint32_t running() {
        if (global<std::uint32_t>(d::g_now) <= jo::view<std::uint32_t>(jo::self<jo::ModelObj>(this)->end_time) || jo::view<std::uint32_t>(jo::self<jo::ModelObj>(this)->end_time) == 0) return 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("Timer::running", 0x0041b090, &H::running);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_camera.cpp (game/camera1)
namespace {
namespace game_camera {

struct H {
    // 0x0041b0d0 Effect::set_owner(bone owner, bone): detaches the parts from the old ones.
    void* effect_set_owner(void* owner, void* bone) {
        if (jo::view<void*>(jo::self<jo::ModelObj>(this)->owner) != nullptr) joan::orig::as_bone(jo::as_actor(joan::orig::arg::as<void*>(joan::orig::as_effect_state(reinterpret_cast<std::uint8_t*>(&jo::self<jo::ModelObj>(this)->fx))->object()))->bone)->set_parent(joan::orig::arg::mptr(static_cast<void*>(nullptr)));  // 0x0041b280 / 0x0060ec70
        if (jo::view<void*>(jo::self<jo::ModelObj>(this)->bone) != nullptr) joan::orig::as_bone(jo::view<void*>(jo::self<jo::ModelObj>(this)->bone))->set_parent(joan::orig::arg::mptr(static_cast<void*>(nullptr)));
        jo::view<void*>(jo::self<jo::ModelObj>(this)->owner) = owner;
        jo::view<void*>(jo::self<jo::ModelObj>(this)->bone) = bone;
        jo::self<jo::ModelObj>(this)->attached = owner != nullptr ? 1 : 0;
        return bone;  // eax = bone (last loaded at 0x0041b11f)
    }
};

#define C1(name, addr, fn) JOAN_REPLACE("game/camera1", name, addr, fn)
C1("Effect::set_owner", 0x0041b0d0, &H::effect_set_owner);
#undef C1

}  // namespace game_camera
}  // namespace

// ---------------------------------------- from modules/game_b86_05.cpp (game/b86_05)
namespace {
namespace game_b86_05 {

struct H {
    // ---- setters (eax = this) / getters --------------------------------------------------
    H* set_62(std::int16_t v) {  // 0x0041b370
        jo::self<jo::EffectState>(this)->i16_62 = v;
        return this;
    }
    H* set_60(std::int16_t v) {  // 0x0041b320
        jo::self<jo::EffectState>(this)->i16_60 = v;
        return this;
    }
    H* set_28(jo::Actor* v) {  // 0x0041b250
        jo::self<jo::EffectState>(this)->object_ = v;
        return this;
    }
    H* set_24(jo::Plane* v) {  // 0x0041b200
        jo::self<jo::EffectState>(this)->parts = v;
        return this;
    }
    std::uint8_t* ptr_3() { return reinterpret_cast<std::uint8_t*>(jo::self<jo::EffectState>(this)->name_); }  // 0x0041b3a0
    std::int16_t get_62() { return jo::self<jo::EffectState>(this)->i16_62; }       // 0x0041b350
    std::int16_t get_60() { return jo::self<jo::EffectState>(this)->i16_60; }       // 0x0041b300
    jo::Plane* get_24() { return jo::self<jo::EffectState>(this)->parts; }     // 0x0041b230
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_05", name, addr, fn)
B86("Obj41b::set_62", 0x0041b370, &H::set_62);
B86("Obj41b::set_60", 0x0041b320, &H::set_60);
B86("Obj41b::set_28", 0x0041b250, &H::set_28);
B86("Obj41b::set_24", 0x0041b200, &H::set_24);
B86("Obj41b::ptr_3", 0x0041b3a0, &H::ptr_3);
B86("Obj41b::get_62", 0x0041b350, &H::get_62);
B86("Obj41b::get_60", 0x0041b300, &H::get_60);
B86("Obj41b::get_24", 0x0041b230, &H::get_24);
#undef B86

}  // namespace game_b86_05
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

using joan::call::thiscall;

using joan::img::obj;

using joan::hook::global;

using joan::hook::global;

using joan::call::thiscall;
using joan::call::stdcall_call;
using joan::img::obj;

JOAN_COUNTER(c_ref_release, "ModelRef::release");

[[maybe_unused]] constexpr img::Addr kAnimRes = d::g_anim_res_mgr, kModelRes = d::g_model_res_mgr, kModelRefs = d::g_model_refs;

void sleep_5() { stdcall_call<std::uint32_t>(global<std::uintptr_t>(d::iat_KERNEL32_SleepEx), 5u, 1u); }  // SleepEx(5, TRUE)

struct H {

    // ------------------------------------------------------------------ 0x0041a020
    std::int32_t ref_release() {
        c_ref_release.hit();
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_model_slot(this)->idle()) == 0) {  // 0x00419ff0
            sleep_5();
            joan::orig::as_model_slot(this)->stream();  // 0x00419d50
        }
        --jo::self<jo::ModelSlot>(this)->handle;
        if (jo::self<jo::ModelSlot>(this)->handle == 0 && jo::view<std::uint32_t>(jo::self<jo::ModelSlot>(this)->attach_model) != 0) {
            joan::orig::as_model_res_mgr(obj(kModelRes))->release(joan::orig::arg::i16(static_cast<std::int16_t>(jo::self<jo::ModelSlot>(this)->attach_model)));  // 0x004b16a0
            if (jo::view<std::uint32_t>(jo::self<jo::ModelSlot>(this)->attach_anim) != 0)
                joan::orig::as_anim_res_mgr(obj(kAnimRes))->release(joan::orig::arg::i16(static_cast<std::int16_t>(jo::self<jo::ModelSlot>(this)->attach_anim)));  // 0x00417e00
        }
        return jo::self<jo::ModelSlot>(this)->handle;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("ModelRef::release", 0x0041a020, &H::ref_release);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Unit::release_419950() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b102_01::H::release_419950>(this)); }  // 0x419950
auto joan::orig::init_a59778() -> void { joan::bind::free<&::game_b94_01::init_a59778>(); }  // 0x419a90
auto joan::orig::ModelSlot::acquire() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::slot_acquire>(this)); }  // 0x419ae0
auto joan::orig::ModelSlot::add_ref() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::slot_add_ref>(this)); }  // 0x419cf0
auto joan::orig::ModelSlot::stream() -> void { joan::bind::member<&::game_b84_05::H::slot_stream>(this); }  // 0x419d50
auto joan::orig::ModelSlot::idle() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::slot_idle>(this)); }  // 0x419ff0
auto joan::orig::ModelSlot::release() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::ref_release>(this)); }  // 0x41a020
auto joan::orig::NameTable::find_41a0e0(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_01::H::name_find>(this, a0)); }  // 0x41a0e0
auto joan::orig::ModelResMgr::entry(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_18::H::elem_41a180>(this, a0)); }  // 0x41a180
auto joan::orig::Effect::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_05::H::modelobj_ctor>(this)); }  // 0x41a1b0
auto joan::orig::Effect::destruct() -> void { joan::bind::member<&::game_leaves82b::H::dtor_41a1f0>(this); }  // 0x41a1f0
auto joan::orig::init_a5977c() -> void { joan::bind::free<&::game_b95_01::init_a5977c>(); }  // 0x41a8d0
auto joan::orig::state_differs(void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_world::state_differs>(a0, a1)); }  // 0x41a900
auto joan::orig::Effect::update() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_fx::H::effect_update>(this)); }  // 0x41ab50
auto joan::orig::MapObject::heading_to(void* a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b92_01::H::heading_to>(this, a0)); }  // 0x41af80
auto joan::orig::Timer::running() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_02::H::running>(this)); }  // 0x41b090
auto joan::orig::Effect::set_owner(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_camera::H::effect_set_owner>(this, a0, a1)); }  // 0x41b0d0
auto joan::orig::EffectState::start() -> void { joan::bind::member<&::game_fx::H::state_start>(this); }  // 0x41b170
auto joan::orig::EffectState::stop() -> void { joan::bind::member<&::game_fx::H::state_stop>(this); }  // 0x41b190
auto joan::orig::EffectState::clear() -> void { joan::bind::member<&::game_fx::H::state_clear>(this); }  // 0x41b1c0
auto joan::orig::EffectState::started() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_fx::H::state_started>(this)); }  // 0x41b1e0
auto joan::orig::EffectState::set_model(Plane* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b86_05::H::set_24>(this, a0)); }  // 0x41b200
auto joan::orig::EffectState::model() -> Plane* { return joan::bind::conv<Plane*>(joan::bind::member<&::game_b86_05::H::get_24>(this)); }  // 0x41b230
auto joan::orig::EffectState::set_actor(Actor* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b86_05::H::set_28>(this, a0)); }  // 0x41b250
auto joan::orig::EffectState::object() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::state_object>(this)); }  // 0x41b280
auto joan::orig::EffectState::source_blend() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b86_05::H::get_60>(this)); }  // 0x41b300
auto joan::orig::EffectState::set_source_blend(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b86_05::H::set_60>(this, a0)); }  // 0x41b320
auto joan::orig::EffectState::dest_blend() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b86_05::H::get_62>(this)); }  // 0x41b350
auto joan::orig::EffectState::set_dest_blend(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b86_05::H::set_62>(this, a0)); }  // 0x41b370
auto joan::orig::EffectState::name() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b86_05::H::ptr_3>(this)); }  // 0x41b3a0
