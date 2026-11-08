// en3d/actor.cpp: functions of the original d:\ww\en3d\actor.cpp (re/attribution.csv).
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
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/overrides.hpp"
#include "../render/ambient_occlusion.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;                                                     // operator delete

using joan::img::obj;

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_units4.cpp (game/units4)
//
//      - Actor::release 0x005aaf10 (en3d/actor.cpp, the body of the actor destructor):
//        detaches from the scene, frees owned buffers, drops effects owned by the actor,
//        deletes bone / mesh / definition, frees the sub-mesh array.
namespace {
namespace game_units4 {

struct Obj {};

// Virtual thiscall through vtable slot `offset` (bytes).
template <class R = void, class... A>
R virtual_call(void* object, std::int32_t offset, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(object)[joan::call::x86_vslot(offset)], sizeof(void*));
    return (static_cast<Obj*>(object)->*method)(args...);
}

constexpr img::Addr kEffects(d::g_particle_effect_array);  // effect registry (id -> effect)
constexpr img::Addr kMemory(d::g_mem);

std::int32_t effect_missing(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kEffects))->empty_at(joan::orig::arg::i32(id)); }  // 0x0041bd90
void* effect_get(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kEffects))->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041bd60
void scene_detach(void* scene, void* node) { joan::orig::as_scene(scene)->detach(joan::orig::arg::mptr(node)); }                             // 0x005efdb0
std::int32_t list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

JOAN_COUNTER(c_actor_release, "Actor::release");

struct H {
    // 0x005aaf10 Actor::release()
    void actor_release() {
        c_actor_release.hit();
        if (jo::view<void*>(jo::self<jo::Actor>(this)->scene) != nullptr) scene_detach(jo::view<void*>(jo::self<jo::Actor>(this)->scene), this);
        jo::self<jo::Actor>(this)->cursor_owner = nullptr;  // the key-cache owner AnimDef::animate stores there
        if (jo::view<void*>(jo::self<jo::Actor>(this)->bone_mats) != nullptr) {
            game_delete(jo::view<void*>(jo::self<jo::Actor>(this)->bone_mats));
            jo::view<void*>(jo::self<jo::Actor>(this)->bone_mats) = nullptr;
        }
        jo::view<std::int32_t>(jo::self<jo::Actor>(this)->key_count) = 0;
        if (jo::view<void*>(jo::self<jo::Actor>(this)->keys) != nullptr) {
            game_delete(jo::view<void*>(jo::self<jo::Actor>(this)->keys));
            jo::view<void*>(jo::self<jo::Actor>(this)->keys) = nullptr;
        }
        if (effect_missing(jo::self<jo::Actor>(this)->effect) == 0) {
            void* const e = effect_get(jo::self<jo::Actor>(this)->effect);
            if (static_cast<void*>(jo::as_model_obj(e)->owner) == static_cast<void*>(this))  // the effects are ModelObj records
                joan::orig::as_effect(e)->set_owner(joan::orig::arg::mptr(0), joan::orig::arg::mptr(0));  // 0x0041b0d0
            joan::orig::as_effect_registry(img::obj(kEffects))->remove(joan::orig::arg::i32(jo::self<jo::Actor>(this)->effect));                                 // 0x0041bbf0
            jo::self<jo::Actor>(this)->effect = 0;
        }
        if (jo::view<void*>(jo::self<jo::Actor>(this)->plane) != nullptr) {
            void* const mesh = jo::view<void*>(jo::self<jo::Actor>(this)->plane);
            if (mesh != nullptr) joan::orig::as_plane(mesh)->destroy(1u);  // 0x00459c40
            jo::view<void*>(jo::self<jo::Actor>(this)->plane) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Actor>(this)->bone) != nullptr) {
            void* const bone = jo::view<void*>(jo::self<jo::Actor>(this)->bone);
            if (bone != nullptr) joan::orig::as_bone(bone)->delete_(1u);  // 0x005ab400
            jo::view<void*>(jo::self<jo::Actor>(this)->bone) = nullptr;
        }
        if (jo::self<jo::Actor>(this)->record != 0) joan::orig::as_players(img::obj(d::g_physic_sys))->remove_record(joan::orig::arg::as<std::int32_t>(jo::self<jo::Actor>(this)->record));  // 0x006adf70
        if (jo::self<jo::Actor>(this)->kind != 2) {
            for (std::int32_t i = list_count(img::obj(kEffects)); i > 0; --i) {
                if (effect_missing(i) != 0) continue;
                void* const e = effect_get(i);
                if (static_cast<void*>(jo::as_model_obj(e)->owner) != static_cast<void*>(this)) continue;
                joan::orig::as_effect(e)->set_owner(joan::orig::arg::mptr(0), joan::orig::arg::mptr(0));
                joan::orig::as_effect_registry(img::obj(kEffects))->remove(joan::orig::arg::i32(i));
            }
        }
        if (jo::self<jo::Actor>(this)->holder_kind != 0) {
            switch (jo::self<jo::Actor>(this)->holder_kind) {
            case 1: {
                void* const def = jo::view<void*>(jo::self<jo::Actor>(this)->def);
                if (def != nullptr) joan::orig::as_actor_def(def)->destroy(1u);  // 0x004b0c20
                jo::view<void*>(jo::self<jo::Actor>(this)->def) = nullptr;
                void* const holder = jo::view<void*>(jo::self<jo::Actor>(this)->holder);
                joan::orig::as_billboard(holder)->release(1u);  // 0x006356c0
                if (holder != nullptr) virtual_call<void*>(holder, 0, 1);
                jo::view<void*>(jo::self<jo::Actor>(this)->holder) = nullptr;
                break;
            }
            case 2:
            case 3: {
                void* const def = jo::view<void*>(jo::self<jo::Actor>(this)->def);
                if (def != nullptr) joan::orig::as_actor_def(def)->destroy(1u);
                jo::view<void*>(jo::self<jo::Actor>(this)->def) = nullptr;
                jo::view<void*>(jo::self<jo::Actor>(this)->holder) = nullptr;
                break;
            }
            default:
                break;
            }
        }
        if (jo::view<void**>(jo::self<jo::Actor>(this)->morphs) == nullptr) return;
        for (std::int32_t i = 0; i < jo::self<jo::Actor>(this)->morph_count; ++i) {
            void* const sub = jo::view<void**>(jo::self<jo::Actor>(this)->morphs)[i];
            if (sub != nullptr) joan::orig::as_morph(sub)->destroy(1u);  // 0x004b0c70
        }
        joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Actor>(this)->morphs)), joan::orig::arg::str(&img::global<const char>(d::s_d_ww_en3d_actor_cpp)), joan::orig::arg::i32(// 0x0058ed70
                 static_cast<std::int32_t>(global<std::int16_t>(d::g_a16e50)) + 0x76));         // actor.cpp
        jo::view<void*>(jo::self<jo::Actor>(this)->morphs) = nullptr;
        jo::self<jo::Actor>(this)->morph_count = 0;
        jo::self<jo::Actor>(this)->morph_cap = 0;
    }
};

#define U4(name, addr, fn) JOAN_REPLACE("game/units4", name, addr, fn)
U4("Actor::deinit", 0x005aaf10, &H::actor_release);
#undef U4

}  // namespace game_units4
}  // namespace

// ---------------------------------------- from modules/en3d_helpers20.cpp (en3d/helpers20)
namespace {
namespace en3d_helpers20 {

struct H {
    H* bone_delete(std::uint32_t flags) {  // 0x005ab400 scalar deleting destructor
        joan::orig::as_bone(this)->dtor();         // 0x0060ec40 ~Bone
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
};

#define HELPER20(name, addr, fn) JOAN_REPLACE("en3d/helpers20", name, addr, fn)
HELPER20("Bone::delete", 0x005ab400, &H::bone_delete);
#undef HELPER20

}  // namespace en3d_helpers20
}  // namespace

// ---------------------------------------- from modules/game_b104_09.cpp (game/b104_09)
//
//    - Obj::stub_59edc0 0x0059edc0 / stub_5a18b0 0x005a18b0 / stub_5a1d60 0x005a1d60 /
//      stub_5a1fe0 0x005a1fe0 / stub_5a8bc0 0x005a8bc0 / stub_5ab450 0x005ab450 (ret 0): empty;
//      no caller reads eax afterwards (5a8bc0's caller 0x00564740 is an atexit handler).
namespace {
namespace game_b104_09 {

struct H {
    void stub_void() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_09", name, addr, fn)
B("Obj::stub_5ab450", 0x005ab450, &H::stub_void);
#undef B

}  // namespace game_b104_09
}  // namespace

// ---------------------------------------- from modules/en3d_helpers4.cpp (en3d/helpers4)
namespace {
namespace en3d_helpers4 {


JOAN_COUNTER(c_actor_visible, "Actor::visible");

struct H {
    // 0x005ab470: pass 1 decides (and caches in +0xa7, distance +0xa8) whether the actor is
    // drawn for this camera: shown (+0x10a), sphere in view and not occluded (through the
    // mirror camera 0xa69ab8 when the camera is 0xa69ab4), in front of the near range for a
    // camera with +0xcc set and a type-10 actor, and 0x005ab620. Later passes return the cache.
    std::int32_t actor_visible(void* camera, std::int32_t pass) {
        c_actor_visible.hit();
        if (pass != 1) return jo::self<jo::Actor>(this)->vis_a7;
        jo::view<std::uint8_t>(jo::self<jo::Actor>(this)->vis_a7) = 0;
        if (jo::self<jo::Actor>(this)->visible == 0) return 0;
        float sphere[4];
        joan::orig::as_actor(this)->bounding_sphere(joan::orig::arg::mptr(sphere), 1u);  // 0x005acc80
        void* const mirror = global<void*>(d::g_a69ab4);
        void* const test_camera = (mirror != nullptr && camera == mirror) ? global<void*>(d::g_a69ab8) : camera;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(test_camera)->sphere_in_view(joan::orig::arg::mptr(sphere), 0.0f)) == 0) return 0;  // 0x006159f0
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(test_camera)->sphere_occluded(joan::orig::arg::mptr(sphere), 0.0f)) != 0) return 0;  // 0x00615ed0
        if (jo::view<std::int8_t>(jo::as_camera(camera)->u8_cc) != 0 && jo::self<jo::Actor>(this)->kind == 10) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(test_camera)->sphere_near(joan::orig::arg::mptr(sphere))) == 0) return 0;  // 0x00615e10
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_actor(this)->visible_5ab620()) == 0) return 0;  // 0x005ab620
        jo::self<jo::Actor>(this)->vis_time = joan::orig::arg::as<float>(joan::orig::as_camera(camera)->near_distance(joan::orig::arg::mptr(sphere)));  // 0x00615e80
        jo::view<std::uint8_t>(jo::self<jo::Actor>(this)->vis_a7) = 1;
        return 1;
    }
};

#define HELPER4(name, addr, fn) JOAN_REPLACE("en3d/helpers4", name, addr, fn)
HELPER4("Actor::visible", 0x005ab470, &H::actor_visible);
#undef HELPER4

}  // namespace en3d_helpers4
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {


// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- Actor (en3d/actor.cpp) ---------------------------------------------------------------
    std::uint32_t actor_visible() {  // 0x005ab620
        if (global<std::uint32_t>(d::g_a0382c) == 0) return 0;
        if (global<std::uint32_t>(d::g_draw_overlay_mesh) == 0 && jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->kind) == 0xa) return 0;
        if (jo::self<jo::Actor>(this)->visible == 0) return 0;
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->visible());  // Plane visible 0x005d0480
    }
    std::uintptr_t actor_choose_mesh(void* ctx, std::int32_t pass, void** plane, void* extra) {  // 0x005ab6a0
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_actor(self())->select_mesh(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass), joan::orig::arg::mptr(plane), joan::orig::arg::mptr(extra)));           // 0x005ad060
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Actor::can_render", 0x005ab620, &H::actor_visible);
HELPER("Actor::render_update", 0x005ab6a0, &H::actor_choose_mesh);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/en3d_helpers9.cpp (en3d/helpers9)
namespace {
namespace en3d_helpers9 {


bool below_or_nan(double a, double b) { return !(a >= b); }
using joan::orig::helpers::differs;

JOAN_COUNTER(c_actor_fade, "Actor::fade_update");

struct H {
    // --- Actor (en3d/actor.cpp) ---------------------------------------------------------------
    // 0x005ab6e0: time fade (+0x30 rate per second, sign = direction; +0x34 full alpha; +0x38
    // start time against the clock 0xa500ba). Ends once 1000 / CRT(rate) ms have passed; while
    // fading the plane's materials switch to blended (texture mode 0x20010 for mode 1, blend 9
    // or 1, pass 2, flag 0x100) and get alpha (elapsed / 1000 * full * rate) clamped to 0..full
    // (for a negative rate: full + that, at least 0).
    void actor_fade_update(void* camera) {
        c_actor_fade.hit();
        if (!differs(jo::self<jo::Actor>(this)->fade, 0.0f)) return;
        const std::uint32_t elapsed = global<std::uint32_t>(d::g_now) - jo::self<jo::Actor>(this)->fade_38;
        {
            const double x = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(jo::self<jo::Actor>(this)->fade))));  // CRT (fabs)
            const float duration = static_cast<float>(1000.0 / x);  // fdivr at 24-bit precision
            if (below_or_nan(static_cast<double>(duration), static_cast<double>(elapsed))) return;
        }
        if (global<std::uint32_t>(d::g_frame_step_ms) == 0) return;
        std::uint8_t* plane = nullptr;
        void* extra = nullptr;  // select_mesh writes a pointer (the bone) here: pointer-sized, or x64 clobbers the neighbouring local
        joan::orig::as_actor(this)->select_mesh(joan::orig::arg::mptr(camera), 1, &plane, &extra);  // 0x005ad060
        for (std::int32_t i = 0; i < jo::as_plane(plane)->material_count; ++i) {
            std::uint8_t* const m = jo::view<std::uint8_t*>(jo::as_plane(plane)->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material));
            if (jo::as_material(m)->texture_mode == 1) jo::as_material(m)->texture_mode = 0x20010;
            if (jo::as_material(m)->blend == 9 || jo::as_material(m)->blend == 8)
                jo::as_material(m)->blend = 9;
            else
                jo::as_material(m)->blend = 1;
            jo::as_material(m)->pass_mask = 2;
            jo::as_material(m)->raster = jo::as_material(m)->raster | 0x100;
        }
        jo::as_plane(plane)->skip_draw = 0;
        jo::self<jo::Actor>(this)->plane->skip_draw = 0;
        const std::uint32_t now = global<std::uint32_t>(d::g_now) - jo::self<jo::Actor>(this)->fade_38;
        float t = static_cast<float>(static_cast<double>(now) / 1000.0);  // fild qword / 1000.0f
        t = t * static_cast<float>(jo::self<jo::Actor>(this)->alpha);                    // fimul
        t = t * jo::self<jo::Actor>(this)->fade;
        std::int32_t alpha = static_cast<std::int32_t>(static_cast<std::int64_t>(t));  // _ftol
        if (!(jo::self<jo::Actor>(this)->fade >= 0.0f)) {  // x87: < 0 or NaN
            alpha += jo::self<jo::Actor>(this)->alpha;
            if (alpha <= 0) alpha = 0;
        } else if (jo::self<jo::Actor>(this)->fade > 0.0f) {
            if (alpha >= jo::self<jo::Actor>(this)->alpha) alpha = jo::self<jo::Actor>(this)->alpha;
        }
        std::int32_t a = 0 + alpha;
        if (a < 0)
            a = 0;
        else if (a > jo::self<jo::Actor>(this)->alpha)
            a = jo::self<jo::Actor>(this)->alpha;
        std::uint8_t colour_obj[8];
        std::uint32_t d3d = 0;
        void* const c = joan::orig::as_colour(colour_obj)->construct(0xffu, 0xffu, 0xffu, static_cast<std::uint32_t>(static_cast<std::uint8_t>(a)));
        joan::orig::as_colour(&d3d)->from_rgba(joan::orig::arg::mptr(c));  // 0x005e95b0
        joan::orig::as_plane(plane)->set_104(1u);       // 0x005d6e80
        joan::orig::as_plane(plane)->set_fade(joan::orig::arg::u32(d3d));      // 0x005d5e80
    }
};

JOAN_REPLACE("en3d/helpers9", "Actor::render_update_alpha", 0x005ab6e0, &H::actor_fade_update);

}  // namespace en3d_helpers9
}  // namespace

// ---------------------------------------- from modules/scene_draw.cpp (en3d/actor.cpp)
namespace {
namespace scene_draw {

// x87 `fcomp 0.0; test ah,0x40` is also true for NaN (unordered sets C3).
bool x87_is_zero(float f) { return f == 0.0f || f != f; }

JOAN_COUNTER(c_actor_draw, "Actor::draw");

// Thunks of the functions called (original addresses in comments).
void plane_draw(void* plane, void* ctx, std::int32_t pass, std::uint32_t arg) {
    joan::orig::as_plane(plane)->draw(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass), joan::orig::arg::u32(arg));  // Plane::draw 0x005d0570
}
void plane_set_owner(void* plane, void* owner) { joan::orig::as_plane(plane)->set_owner(joan::orig::arg::mptr(owner)); }  // 0x005d03f0

// --- the replacements ------------------------------------------------------------
struct Actor {
    void draw(void* ctx, std::int32_t pass);
};

// 0x005aba20, thiscall, ret 8.
void Actor::draw(void* ctx, std::int32_t pass) {
    c_actor_draw.hit();
    if (pass == 2 && jo::view<std::int8_t>((jo::as_actor(this)->plane)->skip_draw) == 1) return;
    void* const anim = jo::as_actor(this)->anim;  // read before the update below
    if (!x87_is_zero(jo::as_actor(this)->fade)) joan::orig::as_actor(this)->fade_update(joan::orig::arg::mptr(ctx));  // 0x005ab6e0
    void* plane = nullptr;
    void* unused = nullptr;  // select_mesh writes a pointer here (the bone): pointer-sized, or x64 clobbers `plane`
    joan::orig::as_actor(this)->choose_mesh(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass), &plane, &unused);  // 0x005ab6a0 choose the mesh
    if (jo::view<std::uint32_t>(jo::as_actor(this)->kind) == 0x27) joan::orig::as_trail(jo::as_actor(this)->owner)->update();  // 0x00687880
    if (plane == nullptr) return;
    if (anim != nullptr && pass == 1) {
        const void* const frame = joan::orig::arg::as<const void*>(joan::orig::as_anim_set(anim)->frame(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_actor(this)->action)), joan::orig::arg::u32(static_cast<std::uint16_t>(jo::as_actor(this)->sub_action))));  // 0x005b5dd0
        const auto* const table = jo::view<std::uint32_t*>(jo::as_actor(this)->def->mesh_list_18);
        if (jo::as_anim_action(frame)->value == 1 && table != nullptr && table[jo::view<std::uint32_t>(jo::as_actor(this)->action)] != 0) {
            // Drawn with this actor's bone matrices on the mesh's skeleton.
            void* const skin_owner = joan::orig::arg::as<void*>(joan::orig::as_plane(plane)->skin_owner());  // 0x005aedb0
            void* const target = skin_owner ? skin_owner : plane;
            void* const bones = jo::as_actor(this)->bone;
            joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(target)->bone_5cf8d0()))->set_parent(joan::orig::arg::mptr(bones));  // 0x0060ec70 (0x005cf8d0)
            joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(target)->bone_5cf8d0()))->invalidate();          // 0x0060ee10
            plane_set_owner(target, this);
            jo::as_plane(plane)->transformed = 0;
            plane_draw(plane, ctx, pass, jo::as_actor(this)->skin);
            joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(target)->bone_5cf8d0()))->set_parent(joan::orig::arg::mptr(0u));
            plane_set_owner(target, nullptr);
            return;
        }
    }
    plane_draw(plane, ctx, pass, jo::as_actor(this)->skin);
}

JOAN_REPLACE("en3d/actor.cpp", "Actor::render", 0x005aba20, &Actor::draw);

}  // namespace scene_draw
}  // namespace

// ---------------------------------------- from modules/en3d_helpers8.cpp (en3d/helpers8)
namespace {
namespace en3d_helpers8 {


JOAN_COUNTER(c_actor_entry, "Actor::draw_entry");

struct H {

    // --- Actor (en3d/actor.cpp) -----------------------------------------------------------------
    // 0x005abc50: draw the actor's plane for this pass (0x005ab6a0 picks it). In pass 1, when
    // the current animation frame (0x005b5dd0, +0x44 / +0x48) is a type-1 attachment and the
    // definition has a plane for it (+0 -> +0x18[+0x44]), the plane is temporarily parented to
    // the actor's bone (+0x8c) and owned by the actor around the draw.
    void actor_draw_entry(void* lights, void* camera, std::int32_t pass) {
        c_actor_entry.hit();
        if (pass == 2 && jo::view<std::int8_t>(jo::self<jo::Actor>(this)->plane->skip_draw) == 1) return;
        void* const anim = jo::view<void*>(jo::self<jo::Actor>(this)->anim);
        void* plane = nullptr;
        void* extra = nullptr;  // select_mesh writes a pointer here (the bone): pointer-sized, or x64 clobbers `plane`
        joan::orig::as_actor(this)->choose_mesh(joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass), &plane, &extra);  // 0x005ab6a0
        if (plane == nullptr) return;
        if (anim != nullptr && pass == 1) {
            const std::uint32_t sub = static_cast<std::uint16_t>(jo::self<jo::Actor>(this)->sub_action);
            const std::uint8_t* const frame = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(anim)->frame(joan::orig::arg::i32(jo::self<jo::Actor>(this)->action), joan::orig::arg::u32(sub)));
            void** const planes = jo::view<void**>(jo::self<jo::Actor>(this)->def->mesh_list_18);
            if (jo::view<std::int32_t>(jo::as_anim_action(frame)->value) == 1 && planes != nullptr && planes[jo::self<jo::Actor>(this)->action] != nullptr) {
                void* const owner_plane = joan::orig::arg::as<void*>(joan::orig::as_plane(plane)->skin_owner());  // 0x005aedb0
                void* const target = owner_plane ? owner_plane : plane;
                joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(target)->bone_5cf8d0()))->set_parent(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Actor>(this)->bone)));  // 0x005cf8d0 bone -> 0x0060ec70
                joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(target)->bone_5cf8d0()))->invalidate();                  // 0x0060ee10
                joan::orig::as_plane(target)->set_owner(joan::orig::arg::mptr(this));                                         // 0x005d03f0
                jo::as_plane(plane)->transformed = 0;
                joan::orig::as_plane(plane)->submit_5d35b0(joan::orig::arg::mptr(lights), joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass));  // 0x005d35b0
                joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(target)->bone_5cf8d0()))->set_parent(joan::orig::arg::mptr(nullptr));
                joan::orig::as_plane(target)->set_owner(joan::orig::arg::mptr(nullptr));
                return;
            }
        }
        joan::orig::as_plane(plane)->submit_5d35b0(joan::orig::arg::mptr(lights), joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass));
    }
};

#define HELPER8(name, addr, fn) JOAN_REPLACE("en3d/helpers8", name, addr, fn)
HELPER8("Actor::render_ordered", 0x005abc50, &H::actor_draw_entry);
#undef HELPER8

}  // namespace en3d_helpers8
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (en3d/b82_17)
//
//      0x005abe30 Actor::start_anim(anim, id, a, b, keys, c) (ret 0x18) - action lookup
//                 (0x005b4a60), key reset (0x005b8770), then the anim's sound events (+0x15b)
//                 inside the current frame window are started (0x0041baf0 / 0x0041b0d0)
//
//      0x005add40 Actor::set_skin(id) (ret 4)    - texture swap on the +0x90 / def materials
//
//      0x005adf50 Actor::add_part(a, b, c, d) (ret 0x10) - grows +0xb4 by 2, new 0x84 part
//                 (0x005c67b0 / 0x005c6820) appended
//
//      0x005acc30 Actor::get_xyz(&a, &b, &c) (ret 0xc) - +0xfe / +0x102 / +0x106
//
//      0x005acf60 Part::set(v) (ret 4)           - +0x18 = 0, +0x1c = v
namespace {
namespace en3d_b82_17 {

using joan::img::str;

void* const kMemory = obj(d::g_mem);  // allocator (0x0058eb30 realloc / 0x0058ed70 free)

using joan::orig::helpers::game_new;   // operator new

JOAN_COUNTER(c_start_anim, "Actor::start_anim");
JOAN_COUNTER(c_set_skin, "Actor::set_skin");
JOAN_COUNTER(c_add_part, "Actor::add_part");

struct H {

    // 0x005abe30
    void start_anim(void* anim, const char* id, std::uint32_t a, std::uint32_t b, std::uint32_t keys, std::uint32_t c) {
        c_start_anim.hit();
        // [ebp-8] / [ebp-0xc]: word slots initialised to 0 (written as words by 0x005b4a60).
        std::int32_t slot8 = 0;
        std::int32_t slotc = 0;
        jo::view<void*>(jo::self<jo::Actor>(this)->anim) = anim;
        joan::orig::as_anim_def(anim)->find_action_rec(joan::orig::arg::mptr(static_cast<void*>(&slot8)), joan::orig::arg::mptr(static_cast<void*>(&slotc)), id, static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<std::uint8_t>(jo::self<jo::Actor>(this)->kind))), static_cast<void*>(this));  // 0x005b4a60
        const std::int16_t action = *reinterpret_cast<std::int16_t*>(&slot8);
        jo::self<jo::Actor>(this)->action = action;
        jo::self<jo::Actor>(this)->sub_action = *reinterpret_cast<std::int8_t*>(&slotc);
        jo::self<jo::Actor>(this)->start_time = a;
        jo::self<jo::Actor>(this)->anim_flags = b;
        if (keys != 0 && jo::view<void*>(jo::self<jo::Actor>(this)->keys) != nullptr) {
            std::uint8_t* key = jo::view<std::uint8_t*>(jo::self<jo::Actor>(this)->keys);
            jo::Bone* m = jo::self<jo::Actor>(this)->plane->bones;
            for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::Actor>(this)->plane->bone_count); ++i) {
                joan::orig::as_key(key)->set(0u, joan::orig::arg::mptr(static_cast<void*>(m)));  // 0x005b8770
                ++m;  // 0xa0 bytes on x86
                key += sizeof(jo::AnimKey);  // 0x20
            }
            jo::self<jo::Actor>(this)->key_count = keys;
            jo::self<jo::Actor>(this)->blend_ms = c;
        } else {
            jo::self<jo::Actor>(this)->key_count = 0;
        }
        if (jo::self<jo::Actor>(this)->kind == -1) return;
        if (jo::view<void*>(jo::as_anim_def(anim)->events) == nullptr) return;
        const std::int16_t sub = *reinterpret_cast<std::int16_t*>(&slotc);
        std::uint8_t* const frame = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(anim)->frame(static_cast<std::int32_t>(action), joan::orig::arg::u32(sub)));  // 0x005b5dd0
        void* const kEvents = obj(d::g_pemi_res);
        void* const kEffects = obj(d::g_particle_effect_array);
        void* const kPos = obj(d::g_pos);
        for (std::int32_t i = 0; i < jo::as_anim_def(anim)->events->count; ++i) {
            std::uint8_t* const e = jo::view<std::uint8_t*>(jo::as_anim_def(anim)->events->events) + i * 0x3c;
            if (jo::as_anim_event(e)->time > jo::as_anim_action(frame)->end) return;  // fcomp / test ah,0x41 / jne
            if (jo::as_anim_action(frame)->start > jo::as_anim_event(e)->time) continue;
            void* const def = joan::orig::arg::as<void*>(joan::orig::as_name_list(kEvents)->find_4c2060(joan::orig::arg::str(static_cast<void*>(e + 8)), static_cast<std::int8_t>(joan::orig::arg::i32(1u))));  // 0x004c2060
            std::int32_t owned = 0;
            if (def == nullptr) continue;
            const std::int32_t t = joan::hook::x87::ftol(static_cast<double>(jo::as_anim_event(e)->time - jo::as_anim_action(frame)->start));
            std::int32_t effect;
            if (jo::view<std::int32_t>(jo::self<jo::Actor>(this)->anim_flags) != 0) {
                const std::uint8_t flag = jo::as_anim_event(e)->flag;
                std::uint8_t* const ed = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_name_list(kEvents)->entry(static_cast<std::int32_t>(joan::orig::arg::as<std::intptr_t>(def))));  // 0x0041b440
                effect = joan::orig::arg::as<std::int32_t>(joan::orig::as_model_list(kEffects)->add(jo::self<jo::Actor>(this)->scene, jo::as_pemi_script(ed)->effect, kPos, kPos, static_cast<std::int8_t>(joan::orig::arg::i32(flag)), 0, 0, 0));  // 0x0041baf0
                jo::self<jo::Actor>(this)->effect = effect;
                owned = 2;
            } else {
                const std::uint8_t flag = jo::as_anim_event(e)->flag;
                std::uint8_t* const ed = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_name_list(kEvents)->entry(static_cast<std::int32_t>(joan::orig::arg::as<std::intptr_t>(def))));  // 0x0041b440
                effect = joan::orig::arg::as<std::int32_t>(joan::orig::as_model_list(kEffects)->add(jo::self<jo::Actor>(this)->scene, jo::as_pemi_script(ed)->effect, kPos, kPos, static_cast<std::int8_t>(joan::orig::arg::i32(flag)), 0, joan::orig::arg::i32(static_cast<std::uint32_t>(t)), joan::orig::arg::i32(static_cast<std::uint32_t>(t) + 0x1388u)));  // 0x0041baf0
            }
            if (jo::as_anim_event(e)->bone == -999) {
                void* const fx = joan::orig::as_dynarray(kEffects)->ptr_at(joan::orig::arg::i32(effect));  // 0x0041bd60
                joan::orig::as_effect(fx)->set_owner(joan::orig::arg::mptr(static_cast<void*>(this)), joan::orig::arg::mptr(static_cast<void*>(nullptr)));  // 0x0041b0d0
            } else {
                std::int32_t bone_index;
                if (jo::self<jo::Actor>(this)->def->bone_map != nullptr)
                    bone_index = jo::view<std::int32_t*>(jo::self<jo::Actor>(this)->def->bone_map)[jo::as_anim_event(e)->bone];
                else
                    bone_index = jo::as_anim_event(e)->bone;
                void* const bone = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->bone_5cf8f0(joan::orig::arg::i32(bone_index)));  // 0x005cf8f0
                void* const fx = joan::orig::as_dynarray(kEffects)->ptr_at(joan::orig::arg::i32(effect));  // 0x0041bd60
                joan::orig::as_effect(fx)->set_owner(joan::orig::arg::mptr(static_cast<void*>(this)), joan::orig::arg::mptr(bone));                    // 0x0041b0d0
            }
            if (owned != 0 && joan::orig::as_dynarray(kEffects)->empty_at(joan::orig::arg::i32(effect)) == 0)  // 0x0041bd90
                jo::as_model_obj(joan::orig::as_dynarray(kEffects)->ptr_at(joan::orig::arg::i32(effect)))->attached = 2  /* +0x1a: dead (ModelObj, not the Effect view) */;  // 0x0041bd60
        }
    }

    // 0x005add40
    void set_skin(std::uint32_t id) {
        c_set_skin.hit();
        jo::self<jo::Actor>(this)->skin = id;
        if (id == 0) return;
        std::uint8_t* const skin = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_db(obj(d::g_texture_effect_res))->entry(joan::orig::arg::u32(static_cast<std::int16_t>(id))));  // 0x004b27b0
        void* const kTextures = obj(d::g_texture_array);
        for (std::int32_t i = jo::self<jo::Actor>(this)->plane->material_count; i > 0; --i) {
            std::uint8_t* const m1 = jo::view<std::uint8_t*>(jo::self<jo::Actor>(this)->plane->materials) + (i - 1) * static_cast<std::int32_t>(sizeof(jo::Material));
            std::uint8_t* const m2 = jo::view<std::uint8_t*>(jo::self<jo::Actor>(this)->def->model->mesh->materials) + (i - 1) * static_cast<std::int32_t>(sizeof(jo::Material));
            std::int32_t j = 0;
            for (; j < 6; ++j)
                if (i == static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_tex_script_rec(skin)->material_indices[j]))) break;
            if (j == 6) continue;
            for (std::int32_t k = jo::view<std::int16_t>(jo::as_tex_script_rec(skin)->stage_count[j]); k > 0; --k) {
                void* const tex = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(kTextures)->load(joan::orig::arg::str(static_cast<void*>(&jo::as_tex_script_rec(skin)->stage_names[j * 0x84 + (k - 1) * 0x21])), 1, 0u));  // 0x006099c0
                joan::orig::as_material(m1)->set_texture(joan::orig::arg::i32(k - 1), joan::orig::arg::mptr(tex));   // 0x005d5650
                joan::orig::as_material(m2)->set_texture(joan::orig::arg::i32(k - 1), joan::orig::arg::mptr(tex));   // 0x005d5650
                joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(tex));   // 0x0060a550
                jo::as_material(m1)->texture_mode = id | 0x1000000u | (static_cast<std::uint32_t>(j) << 16);
                jo::as_material(m1)->raster = jo::as_material(m1)->raster | 0x100u;
                jo::as_material(m2)->texture_mode = id | 0x1000000u | (static_cast<std::uint32_t>(j) << 16);
                jo::as_material(m2)->raster = jo::as_material(m2)->raster | 0x100u;
            }
        }
    }

    // 0x005adf50 (eax = this)
    void* add_part(const char* a, const char* b, const char* c, void* d) {
        c_add_part.hit();
        if (jo::self<jo::Actor>(this)->morph_cap == jo::self<jo::Actor>(this)->morph_count) {
            jo::self<jo::Actor>(this)->morph_cap = jo::self<jo::Actor>(this)->morph_cap + 2;
            jo::view<void*>(jo::self<jo::Actor>(this)->morphs) = joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Actor>(this)->morphs)), joan::orig::arg::u32(jo::self<jo::Actor>(this)->morph_cap * static_cast<std::int32_t>(sizeof(jo::MorphPtr))), str(d::s_d_ww_en3d_actor_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a16e54)) + 4));  // 0x0058eb30
        }
        void* mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Morph)));  // 0x84 on x86
        void* part = mem != nullptr ? joan::orig::as_morph(mem)->construct() : nullptr;  // 0x005c67b0
        joan::orig::as_morph(part)->load(a, b, c, d);                                    // 0x005c6820
        jo::view<void**>(jo::self<jo::Actor>(this)->morphs)[jo::self<jo::Actor>(this)->morph_count] = part;
        jo::self<jo::Actor>(this)->morph_count = jo::self<jo::Actor>(this)->morph_count + 1;
        return this;
    }

    // 0x005acc30 (eax = c)
    void* get_xyz(std::uint32_t* a, std::uint32_t* b, std::uint32_t* c) {
        *a = jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->yaw);
        *b = jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->pitch);
        *c = jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->roll);
        return c;
    }

    // 0x005acf60 (eax = this)
    void* part_set(jo::Scene* v) {
        jo::self<jo::Actor>(this)->scene = nullptr;       // +0x18 (was Part::u32_18: a 4-byte store into a pointer on x64)
        jo::self<jo::Actor>(this)->scene_params0 = v;
        return this;
    }
};

#define B82_17E(name, addr, fn) JOAN_REPLACE("en3d/b82_17", name, addr, fn)
B82_17E("Actor::start_anim", 0x005abe30, &H::start_anim);
B82_17E("Actor::set_skin", 0x005add40, &H::set_skin);
B82_17E("Actor::create_morph_control_5adf50", 0x005adf50, &H::add_part);
B82_17E("Actor::get_hvr_dir", 0x005acc30, &H::get_xyz);
B82_17E("Actor::set_scene_ptr", 0x005acf60, &H::part_set);
#undef B82_17E

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16 {

struct H {
    H* quaternion_ctor() { return this; }  // 0x005ac230
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
HELPER16("Quaternion::Quaternion", 0x005ac230, &H::quaternion_ctor);
#undef HELPER16

}  // namespace en3d_helpers16
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
//
//    - empty thiscall stubs (never write eax; no references at all - dead code):
//      0x00597870 / 0x005af040 / 0x005b0a10 / 0x005b49e0 / 0x005c50d0 / 0x005e4410 /
//      0x005eb5b0 (ret 0), 0x005a0290 / 0x005ac250 / 0x005af220 / 0x005b0a30 / 0x005eb570
//      (ret 4), 0x0059fda0 / 0x0059fdc0 / 0x005a0250 / 0x005a0270 (ret 0xc).
//
//    - Anim::select_frame_5ad830 0x005ad830 (out_tex, out_b), ret 8: called at 0x005f1f16 /
//      0x005f1f45 (eax unused).
//
//  - 0x005ad830: the word +0x48 argument is pushed through a 32-bit register with stale upper
//    bits; 0x005b5dd0 / 0x005b5e40 read only the low word (movsx), so it is passed
//    sign-extended. fild qword of the (u32) frame delta with a zero high dword is an exact load,
//    fstp rounds once to float (u32 -> double -> float here). The frame index computation
//    (fdivr, fimul int, fadd qword 0.5, _ftol) uses Fpu{true}; fild / fidiv uses Fpu{true}.div.
//    The locals [ebp-0x1c] / [ebp-0x28] (and [ebp-0x18] after the index) are never read
//    afterwards - all calls (0x0042d130 up to four times, CRT fmod 0x007b2920) and the table
//    load are kept.
namespace {
namespace game_b105_16 {

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;
using u16 = std::uint16_t;


i32 word_arg(u32 raw) { return static_cast<i32>(static_cast<i16>(raw)); }

JOAN_COUNTER(c_select_frame, "Anim::select_frame_5ad830");

struct H {

    void stub1(u32) {}
    void* get_00_5ad810() { return jo::self<jo::Actor>(this)->def; }

    // ---------------------------------------------------------------- 0x005ad830
    void select_frame_5ad830(void** out_tex, void** out_b) {
        c_select_frame.hit();
        const u32 now = jo::view<u32>(jo::self<jo::Actor>(this)->kind) == 0xffffffffu ? global<u32>(d::g_game_time_ms) : global<u32>(d::g_now);
        void* const anim = jo::view<void*>(jo::self<jo::Actor>(this)->anim);
        if (joan::orig::arg::as<u32>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->field_ec()) == 0) {  // 0x005d72e0
            *out_tex = nullptr;
            *out_b = nullptr;
            return;
        }
        bool path_a = false;
        if (anim != nullptr) {
            const u8* r = joan::orig::arg::as<const u8*>(joan::orig::as_anim_set(anim)->frame(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Actor>(this)->action)), joan::orig::arg::u32(word_arg(static_cast<u16>(jo::self<jo::Actor>(this)->sub_action)))));  // 0x005b5dd0
            if (jo::as_anim_action(r)->value == 0 || jo::self<jo::Actor>(this)->def->mesh_list_18 == nullptr ||
                jo::view<u32*>(jo::self<jo::Actor>(this)->def->mesh_list_18)[jo::self<jo::Actor>(this)->action] == 0)
                path_a = true;
        }
        if (path_a) {
            void* tex = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->field_ec());  // 0x005d72e0
            void* sink = nullptr;
            if (now != jo::self<jo::Actor>(this)->anim_time) {
                if (tex != nullptr) {
                    sink = joan::orig::arg::as<void*>(joan::orig::as_skin(jo::view<void*>(jo::self<jo::Actor>(this)->def->model->skin))->linked_36());  // 0x005f50e0
                } else {
                    tex = jo::self<jo::Actor>(this)->plane;
                    sink = jo::self<jo::Actor>(this)->def->model->skin;
                }
                void* const v38 = jo::self<jo::Actor>(this)->key_count != 0 ? static_cast<void*>(jo::self<jo::Actor>(this)->keys) : nullptr;
                const float t = static_cast<float>(static_cast<double>(now - jo::self<jo::Actor>(this)->start_time));
                joan::orig::as_anim_def(anim)->animate(joan::orig::arg::mptr(tex), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Actor>(this)->action)), joan::orig::arg::u32(word_arg(static_cast<u16>(jo::self<jo::Actor>(this)->sub_action))), joan::orig::arg::f32(t), joan::orig::arg::u32(jo::self<jo::Actor>(this)->anim_flags), joan::orig::arg::mptr(jo::self<jo::Actor>(this)->def->bone_map), joan::orig::arg::mptr(v38), joan::orig::arg::f32(jo::self<jo::Actor>(this)->blend_ms), joan::orig::arg::mptr(0u), joan::orig::arg::mptr(0u));  // 0x005b5e40
                joan::orig::as_skin(sink)->deform(joan::orig::arg::mptr(tex));  // 0x005f4c10
                jo::self<jo::Actor>(this)->anim_time = now;
            } else if (tex == nullptr) {
                tex = jo::self<jo::Actor>(this)->plane;
            }
            *out_tex = tex;
            *out_b = nullptr;
        } else if (anim != nullptr &&
                   jo::as_anim_action(joan::orig::arg::as<const u8*>(joan::orig::as_anim_set(anim)->frame(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Actor>(this)->action)), joan::orig::arg::u32(word_arg(static_cast<u16>(jo::self<jo::Actor>(this)->sub_action))))))->value == 1 &&
                   jo::self<jo::Actor>(this)->def->mesh_list_18 != nullptr && jo::view<u32*>(jo::self<jo::Actor>(this)->def->mesh_list_18)[jo::self<jo::Actor>(this)->action] != 0) {
            float time = static_cast<float>(static_cast<double>(now - jo::self<jo::Actor>(this)->start_time));  // [ebp-0x18]
            float scaled = 0.0f;                                                        // [ebp-0x1c]
            void* const frames = joan::orig::arg::as<void*>(joan::orig::as_anim_set(anim)->frame(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Actor>(this)->action)), joan::orig::arg::u32(word_arg(static_cast<u16>(jo::self<jo::Actor>(this)->sub_action)))));  // 0x005b5dd0
            if (jo::self<jo::Actor>(this)->anim_flags == 0) {
                if (!(time >= global<float>(d::k_9cf5c4))) time = 0.0f;
                const float dur = joan::orig::arg::as<float>(joan::orig::as_range(frames)->span());  // 0x0042d130
                if (!(dur >= time)) time = joan::orig::arg::as<float>(joan::orig::as_range(frames)->span());
            } else {
                const float span = jo::as_anim_action(frames)->end - jo::as_anim_action(frames)->start;
                time = static_cast<float>(
                    joan::orig::arg::as<double>(joan::orig::libc::fmod(joan::orig::arg::f64(static_cast<double>(time)), joan::orig::arg::f64(static_cast<double>(span)))));  // fmod
            }
            const Fpu fpu{true};
            const float dur = joan::orig::arg::as<float>(joan::orig::as_range(frames)->span());  // 0x0042d130
            const i32 last = jo::view<i32*>(jo::self<jo::Actor>(this)->def->mesh_list_1c)[jo::self<jo::Actor>(this)->action] - 1;
            double q = fpu.div(static_cast<double>(time), static_cast<double>(dur));
            q = fpu.mul(q, static_cast<double>(last));
            q = fpu.add(q, global<double>(d::dbl_0_5));
            i32 idx = ftol(q);
            if (idx < 0) {
                idx = 0;
            } else if (idx >= jo::view<i32*>(jo::self<jo::Actor>(this)->def->mesh_list_1c)[jo::self<jo::Actor>(this)->action]) {
                idx = jo::view<i32*>(jo::self<jo::Actor>(this)->def->mesh_list_1c)[jo::self<jo::Actor>(this)->action] - 1;
            }
            const i32 n1 = jo::view<i32*>(jo::self<jo::Actor>(this)->def->mesh_list_1c)[jo::self<jo::Actor>(this)->action] - 1;
            if (n1 != 0) {
                const float frac = static_cast<float>(fpu.div(static_cast<double>(idx), static_cast<double>(n1)));
                const float d3 = joan::orig::arg::as<float>(joan::orig::as_range(frames)->span());  // 0x0042d130
                scaled = d3 * frac;
            }
            const volatile u32 frame_entry = jo::view<u32**>(jo::self<jo::Actor>(this)->def->mesh_list_18)[jo::self<jo::Actor>(this)->action][idx];  // [ebp-0x28]
            (void)frame_entry;
            (void)scaled;
            void* tex = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->field_ec());  // 0x005d72e0
            if (tex == nullptr) tex = jo::self<jo::Actor>(this)->plane;
            *out_b = jo::self<jo::Actor>(this)->bone;
            *out_tex = tex;
        } else {
            void* tex = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->field_ec());  // 0x005d72e0
            if (tex == nullptr) tex = jo::self<jo::Actor>(this)->plane;
            *out_tex = tex;
            *out_b = jo::self<jo::Actor>(this)->bone;
        }
        jo::self<jo::Actor>(this)->last_model = *out_tex;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Obj::stub_5ac250", 0x005ac250, &H::stub1);
B("Obj::get_00_5ad810", 0x005ad810, &H::get_00_5ad810);
B("Actor::get_shadow_plane", 0x005ad830, &H::select_frame_5ad830);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
//
//  * module game/leaves80a:
//      Font::~Font 0x00623e70 (empty), String::c_str 0x00417d90 (returns this),
//      Table::last_slot 0x00417520 (+0x08), SoundEntry::id 0x004d0440 (+0x04),
//      SceneNode::attachments 0x005efff0 (+0xce), Textures::mode / set_mode 0x0060a720 /
//      0x0060a740 (+0x30), File::File 0x004179c0 (+0x105 = -1), Bytes::at 0x0041bf10 (byte
//      [this + i + 1]), SoundList::entry 0x004cf690 (this + i * 0x18 - 4), Sound::play / pause
//      0x0059f620 / 0x0059f650 (OpenAL alSourcePlay / alSourcePause), Reader::done 0x005b4140
//      (byte +1 == 0), Light::params 0x0061ddc0 (copies 0x68 bytes from +0x5e), Actor::position
//      0x005ac270 (bone matrix translation), Loader::Loader 0x00577be0 (zeroed, +0x13c = -1),
//      `vector constructor iterator' 0x00449af0 (stdcall), scalar deleting destructor 0x00459c40
//      (destructor 0x005cc750), Progress::redraw 0x0075f4f0 (Game::draw 0x004d48f0 on the
//      loading screen), Model::update_parts 0x005b77b0, Unit::attach_a / attach_b 0x00421960 /
//      0x005227f0 (adds the +0x4e / +0x4a object to the world's scene list +0x6e),
//      Texture::set_name 0x00605ad0, the float sqrt wrapper 0x004256d0 (CRT sqrt 0x007b2344
//      runs fsqrt under the caller's precision control, so its st0 is exactly a double: the
//      C++ double return is exact; the negative-argument result is the x87 indefinite NaN,
//      which also survives the double).
namespace {
namespace game_leaves80a {


struct H {
    // 0x005ac270: returns what 0x0060c310 returns (left in eax).
    void* position(void* out) { return joan::orig::arg::as<void*>(joan::orig::matrix_position(joan::orig::arg::mptr(out), joan::orig::arg::ptr(jo::view<void*>(jo::self<jo::Actor>(this)->bone)))); }  // 0x0060c310
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Actor::get_pos", 0x005ac270, &H::position);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
//
//    - Model::get_matrix 0x005ac2b0 (out), ret 4 (dead code): 0x40 bytes from [+0x8c], row 3 = 0.
//
//    - Model::set_matrix 0x005ac650 (m), ret 4 (dead code): 0x60c270([+0x8c], m), 0x60ee10.
//
//    - Model::get_102 0x005ac950 / Model::get_106 0x005acac0 (float) (dead code).
//
//    - Model::get_8c 0x005acde0 (unused), ret 4 (dead code).
//
//    - key_free 0x005ace80 (cdecl, char) (dead code): byte [0xa67640 + c] == 0.
namespace {
namespace game_b103_10 {


// 0x005ace80 (dead code)
static i32 JOAN_CDECL key_free(i8 c) {
    if (c < 0) c = static_cast<i8>(static_cast<u8>(static_cast<u8>(c) - 0xffu));
    return global<u8>(d::g_a67640 + c) == 0 ? 1 : 0;
}

struct H {
    // ------------------------------------------------------------------ 0x005ac2b0 (dead code)
    u32* get_matrix(u32* out) {
        std::memcpy(out, jo::view<const void*>(jo::self<jo::Actor>(this)->bone), 0x40);  // rep movsd, 16 dwords
        out[0xc] = 0;
        out[0xd] = 0;
        out[0xe] = 0;
        return out;
    }

    // ------------------------------------------------------------------ 0x005ac650 (dead code)
    u32 set_matrix(const void* m) {
        joan::orig::matrix_copy3x3(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Actor>(this)->bone)), joan::orig::arg::mptr(m));  // 0x0060c270
        joan::orig::as_bone(jo::view<void*>(jo::self<jo::Actor>(this)->bone))->invalidate();  // 0x0060ee10
        return 0;  // dead code (no callers); Bone::invalidate returns nothing
    }

    // ------------------------------------------------------------------ 0x005ac950 / 0x005acac0 / 0x005acde0
    float get_102() { return jo::self<jo::Actor>(this)->pitch; }
    float get_106() { return jo::self<jo::Actor>(this)->roll; }
    void* get_8c(u32 /*unused*/) { return jo::self<jo::Actor>(this)->bone; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("Model::get_matrix", 0x005ac2b0, &H::get_matrix);
B("Model::set_matrix", 0x005ac650, &H::set_matrix);
B("Model::get_102", 0x005ac950, &H::get_102);
B("Model::get_106", 0x005acac0, &H::get_106);
B("Model::get_8c", 0x005acde0, &H::get_8c);
B("key_free", 0x005ace80, &key_free);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- from modules/game_actors.cpp (game/actors1)
//
//  * module game/actors1: Actor::set_faded 0x005ae590 (see-through when blocking the
//    camera), Actor::set_position 0x005ac310 (moves the bone and the owner), getters
//    0x005aede0 / 0x005aee10 / 0x005d5eb0. (Actor::visible / can_draw are in en3d/helpers4 /
//    en3d/helpers.)
namespace {
namespace game_actors {

using joan::orig::helpers::at;

constexpr img::Addr kUnits(d::g_items);

JOAN_COUNTER(c_position, "Actor::set_position");

struct H {
    // 0x005ac310 Actor::set_position(pos): bone translation, then the owner by kind (1 / 10
    // unit view +0x56, 6..9 unit by id, 11 object +0xe).
    void set_position(const float* pos) {
        c_position.hit();
        std::uint8_t* const bone = jo::view<std::uint8_t*>(jo::self<jo::Actor>(this)->bone);
        std::memcpy(&jo::as_bone(bone)->local[12], pos, 12);
        joan::orig::as_bone(jo::view<void*>(jo::self<jo::Actor>(this)->bone))->invalidate();  // 0x0060ee10
        switch (jo::self<jo::Actor>(this)->kind) {
        case 1:
        case 10: {
            std::uint8_t* const owner = jo::view<std::uint8_t*>(jo::self<jo::Actor>(this)->owner);
            std::memcpy(jo::as_map_object(owner)->pos, pos, 12);
            joan::orig::as_unit(owner)->update_view();  // 0x004311d0
            break;
        }
        case 6:
        case 7:
        case 8:
        case 9: {
            std::uint8_t* const owner = jo::view<std::uint8_t*>(jo::self<jo::Actor>(this)->owner);
            // kinds 6..9 are missiles: the owner is a Missile, its object_id (+0x8) the index in the objects list
            const std::int32_t object_id = jo::as_missile(owner)->object_id;
            if (joan::orig::as_units(img::obj(kUnits))->list.empty_at(joan::orig::arg::i32(object_id)) != 0) break;  // 0x00425d50
            std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(img::obj(kUnits))->list.ptr_at(joan::orig::arg::i32(object_id)));  // 0x00425d20
            std::memcpy(jo::as_map_object(unit)->pos, pos, 12);
            joan::orig::as_unit(unit)->update_view();
            break;
        }
        case 11: {
            std::uint8_t* const owner = jo::view<std::uint8_t*>(jo::self<jo::Actor>(this)->owner);
            std::memcpy(jo::as_ground_object(owner)->position_, pos, 12);
            joan::orig::as_ground_object(owner)->update_position();  // 0x004fee60
            break;
        }
        default:
            break;
        }
    }
};

#define A1(name, addr, fn) JOAN_REPLACE("game/actors1", name, addr, fn)
A1("Actor::set_pos", 0x005ac310, &H::set_position);
#undef A1

}  // namespace game_actors
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
//
//      0x005ac4b0 Actor::set_position(x, y, z) (ret 0xc) - node +0x8c (+0x30, 0x0060ee10), then by
//                 +8 kind: 1 -> +0xc +0x56 (0x004311d0), 6..9 -> 0xa460a0 lookup (0x00425d50 /
//                 0x00425d20) +0x56 (0x004311d0), 11 -> +0xc +0xe (0x004fee60)
namespace {
namespace en3d_b85_04 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_actor_set_pos, "Actor::set_position");

struct H {
    // 0x005ac4b0 (x, y, z copied as raw dwords)
    void actor_set_pos(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
        c_actor_set_pos.hit();
        void* const node = jo::view<void*>(jo::self<jo::Actor>(this)->bone);
        jo::view<std::uint32_t>(jo::as_bone(node)->local[12]) = x;
        jo::view<std::uint32_t>(jo::as_bone(node)->local[13]) = y;
        jo::view<std::uint32_t>(jo::as_bone(node)->local[14]) = z;
        joan::orig::as_bone(jo::view<void*>(jo::self<jo::Actor>(this)->bone))->invalidate();  // 0x0060ee10
        switch (jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->kind) - 1u) {
            case 0: {  // kind 1
                void* const o = jo::self<jo::Actor>(this)->owner;
                jo::view<std::uint32_t>(jo::as_map_object(o)->pos[0]) = x;
                jo::view<std::uint32_t>(jo::as_map_object(o)->pos[1]) = y;
                jo::view<std::uint32_t>(jo::as_map_object(o)->pos[2]) = z;
                joan::orig::as_unit(o)->update_view();  // 0x004311d0
                break;
            }
            case 5:
            case 6:
            case 7:
            case 8: {  // kinds 6..9
                void* const o = jo::self<jo::Actor>(this)->owner;
                // kinds 6..9 are missiles: the owner is a Missile, its object_id (+0x8) the index in the objects list
                const std::uint32_t object_id = jo::view<std::uint32_t>(jo::as_missile(o)->object_id);
                if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(object_id)) == 0) {  // 0x00425d50
                    void* const p = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(object_id));  // 0x00425d20
                    jo::view<std::uint32_t>(jo::as_map_object(p)->pos[0]) = x;
                    jo::view<std::uint32_t>(jo::as_map_object(p)->pos[1]) = y;
                    jo::view<std::uint32_t>(jo::as_map_object(p)->pos[2]) = z;
                    joan::orig::as_unit(p)->update_view();  // 0x004311d0
                }
                break;
            }
            case 10: {  // kind 11
                void* const o = jo::self<jo::Actor>(this)->owner;
                reinterpret_cast<std::uint32_t*>(jo::as_ground_object(o)->position_)[0] = x;
                reinterpret_cast<std::uint32_t*>(jo::as_ground_object(o)->position_)[1] = y;
                reinterpret_cast<std::uint32_t*>(jo::as_ground_object(o)->position_)[2] = z;
                joan::orig::as_ground_object(o)->update_position();  // 0x004fee60
                break;
            }
            default:
                break;
        }
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("Actor::set_position", 0x005ac4b0, &H::actor_set_pos);
#undef E3D

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/game_fx5.cpp (game/fx6)
//
//  * module game/fx6: the part kinds and their helpers - PartEmitter::update 0x0067f380
//    (age / move / spin the live particles, spawn new ones by rate), Particle::update
//    0x00662f60, PartEmitter::spawn 0x00681070, PartBillboards::draw 0x0067faf0 (camera-
//    facing quads into the effect plane), PartEmitter::apply_keys 0x00681b10, PartKey::time
//    0x00681e50, Particle::reset 0x00662e20, SlotPool::alloc 0x00663800, SceneObject angle
//    setters 0x005ac6a0 / 0x005ac810 / 0x005ac980 / 0x005acaf0 and getter 0x005ac7e0.
namespace {
namespace game_fx5 {


bool equal_or_nan(double a, double b) { return !(a < b || a > b); }

JOAN_COUNTER(c_angles, "SceneObject::set_angle");

struct H {
    // SceneObject angles (+0xfe yaw, +0x102 pitch, +0x106 roll): rotation 0x0060bf40, scaled
    // by +0x28 unless 1, into the bone (+0x8c) 3x3.
    void angles_apply(std::uint32_t yaw, std::uint32_t pitch, std::uint32_t roll) {
        const joan::hook::x87::Fpu fpu{true};
        void* const bone = jo::view<void*>(jo::self<jo::Actor>(this)->bone);
        float m[16];
        joan::orig::matrix_rotation_ypr(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::u32(yaw), joan::orig::arg::u32(pitch), joan::orig::arg::u32(roll));  // 0x0060bf40
        if (!equal_or_nan(jo::self<jo::Actor>(this)->scale, 1.0)) {
            const std::int32_t idx[9] = {0, 1, 2, 4, 5, 6, 8, 9, 10};
            for (std::int32_t k = 0; k < 9; ++k) m[idx[k]] = static_cast<float>(fpu.mul(m[idx[k]], jo::self<jo::Actor>(this)->scale));
        }
        joan::orig::matrix_copy3x3(joan::orig::arg::mptr(bone), joan::orig::arg::mptr(static_cast<void*>(m)));  // 0x0060c270 matrix_copy3x3
        joan::orig::as_bone(jo::view<void*>(jo::self<jo::Actor>(this)->bone))->invalidate();                       // 0x0060ee10 Bone::update
    }
    void set_yaw(std::uint32_t a) {  // 0x005ac6a0
        c_angles.hit();
        const std::uint32_t pitch = jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->pitch);
        const std::uint32_t roll = jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->roll);
        jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->yaw) = a;
        angles_apply(a, pitch, roll);
    }
    void set_pitch(std::uint32_t a) {  // 0x005ac810
        c_angles.hit();
        const std::uint32_t yaw = jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->yaw);
        const std::uint32_t roll = jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->roll);
        jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->pitch) = a;
        angles_apply(yaw, a, roll);
    }
    void set_roll(std::uint32_t a) {  // 0x005ac980
        c_angles.hit();
        const std::uint32_t yaw = jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->yaw);
        const std::uint32_t pitch = jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->pitch);
        jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->roll) = a;
        angles_apply(yaw, pitch, a);
    }
    void set_angles(std::uint32_t yaw, std::uint32_t pitch, std::uint32_t roll) {  // 0x005acaf0
        c_angles.hit();
        jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->yaw) = yaw;
        jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->pitch) = pitch;
        jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->roll) = roll;
        angles_apply(yaw, pitch, roll);
    }
    float yaw() { return jo::self<jo::Actor>(this)->yaw; }  // 0x005ac7e0
};

#define FX6(name, addr, fn) JOAN_REPLACE("game/fx6", name, addr, fn)
FX6("Actor::set_h_dir", 0x005ac6a0, &H::set_yaw);
FX6("Actor::set_v_dir", 0x005ac810, &H::set_pitch);
FX6("Actor::set_r_dir", 0x005ac980, &H::set_roll);
FX6("Actor::set_hvr_dir", 0x005acaf0, &H::set_angles);
FX6("SceneObject::yaw", 0x005ac7e0, &H::yaw);
#undef FX6

}  // namespace game_fx5
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
//
//      0x005ace10 Actor::matrix         (thiscall(out), ret 4) - 64-byte copy of the bone matrix.
namespace {
namespace game_b93_03 {


struct H {
    void* actor_matrix(void* out) {  // 0x005ace10
        void* const bone = jo::view<void*>(jo::self<jo::Actor>(this)->bone);
        if (jo::as_bone(bone)->parent == nullptr) {
            std::memcpy(out, bone, 0x40);
        } else {
            void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(bone)->matrix());  // 0x0060ef80
            std::memcpy(out, m, 0x40);
        }
        return out;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Actor::matrix", 0x005ace10, &H::actor_matrix);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
//
//      0x005acec0 set_flag_a67640(char idx, char v) (cdecl) - idx < 0 -> idx + 1 (byte);
//                 byte [0xa67640 + idx] = (v == 0); returns movsx idx.
//
//      0x005acfd0 Obj::byte_fc                    (thiscall, ret 0) - movsx byte +0xfc.
namespace {
namespace game_b103_11 {


// 0x005acec0
i32 JOAN_CDECL set_flag_a67640(i8 idx, u8 v) {
    if (static_cast<i32>(idx) < 0) idx = static_cast<i8>(static_cast<u8>(idx) + 1);  // sub cl,0xff
    global<u8>(d::g_a67640 + static_cast<i32>(idx)) = v == 0 ? 1 : 0;
    return static_cast<i32>(idx);
}

struct H {
    // 0x005acfd0
    i32 byte_fc() { return static_cast<i32>(jo::self<jo::Actor>(this)->byte_fc); }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("set_flag_a67640", 0x005acec0, &set_flag_a67640);
B103("Obj::byte_fc", 0x005acfd0, &H::byte_fc);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
//
//  * module game/fx3: weapon trails (0x00687880 update, 0x006883d0 add node, 0x00688550
//    age, 0x006889c0 build ribbon, Hermite / Catmull-Rom helpers, node list), scene-node
//    linking (0x005efc50 / 0x005efdb0 / 0x005acf10, 0x006891d0).
namespace {
namespace game_fx {


struct H {
    // eax: *scene when scene != 0 (the +0x1c value), else this.
    std::uintptr_t node_set_scene(void* scene) {                    // 0x005acf10
        jo::view<void*>(jo::self<jo::Actor>(this)->scene) = scene;
        jo::self<jo::Actor>(this)->scene_params0 = scene != nullptr ? *static_cast<jo::Scene**>(scene) : nullptr;  // the sector's +0 scene
        return scene != nullptr ? reinterpret_cast<std::uintptr_t>(jo::self<jo::Actor>(this)->scene_params0)
                                : reinterpret_cast<std::uintptr_t>(this);
    }
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("Actor::set_sector_ptr", 0x005acf10, &H::node_set_scene);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
//
//    - Actor::set_byte_fc 0x005acfa0 (char), ret 4: +0xfc = arg.
//
//  - Return values: callers of 0x5acfa0, 0x5b8a50, 0x5c80d0, 0x5e7ce0, 0x601140, 0x616dd0,
//    0x623bd0 ignore eax (checked); 0x5d5fe0's callers use al only.
namespace {
namespace game_b100_11 {


struct H {
    // ------------------------------------------------------------------ 0x005acfa0
    H* set_byte_fc(i8 v) {
        jo::self<jo::Actor>(this)->byte_fc = v;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Actor::set_byte_fc", 0x005acfa0, &H::set_byte_fc);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
//
//  * module game/b91_03:
//    TextCursor::advanced 0x0058c120, to_lower 0x0058e330, empty methods 0x0058e720 /
//    0x0059d1b0, Stream::forward 0x005939f0, Joystick::in_rect 0x00594360,
//    Joystick::pressed_kind 0x00594a10, Key::repeat_count 0x00595fb0, word getters
//    0x005979a0 / 0x005979c0, Pad::axis 0x00597c80, Pad::axis_range 0x00597e00,
//    same16 0x00598700, Pair::clear 0x005a1730, Bitmap::address 0x005a93b0,
//    MusicStream::release 0x005a98a0, MusicStream::read_header 0x005a9ad0,
//    MusicStream::reset 0x005a9b40, MusicStream::rewind 0x005aa040, Actor::set_byte_fd
//    0x005ad000, Table::get 0x005af330, Table::missing 0x005af360, AnimDef::lookup_word
//    0x005b4a00, Names::get 0x005ba910, Rect::set 0x005bed90 / 0x00612ee0, Array::at1
//    0x005ea410, RenderTarget::create_depth 0x006032d0, RenderTarget::release_depth
//    0x00603380, z_plane_hit 0x0060e6a0, BitWriter::put 0x0060ff70, BitWriter::drop
//    0x006100a0, Viewport::set_int 0x00614110, Sound3d::set 0x00616a90, getter 0x00616b60,
//    Dialog::reset_name 0x00616f80, base dtor bodies 0x00628ad0 / 0x0062c720,
//    TextBox::init 0x0062b8d0, Scroll::set 0x0062be10 / 0x0062cd40, Scroll::at_end
//    0x0062c040 / 0x0062cf70, Scroll::set_end 0x0062ced0, TextBox::cursor_x 0x00630770,
//    TextBox::line_y 0x00630ab0, getter 0x00631770, Table16::entry 0x00638150.
namespace {
namespace game_b91_03 {


struct H {
    void set_byte_fd(std::uint8_t v) { jo::self<jo::Actor>(this)->flag_fd_ = v; }  // 0x005ad000
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("Actor::set_byte_fd", 0x005ad000, &H::set_byte_fd);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
//
//      0x005ad030 Actor::flag_fd                    (thiscall, ret 0) - (signed char) +0xfd.
namespace {
namespace game_b96_06 {


struct H {
    // ------------------------------------------------------------------ 0x005ad030
    i32 flag_fd() { return static_cast<i32>(jo::view<i8>(jo::self<jo::Actor>(this)->flag_fd_)); }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Actor::flag_fd", 0x005ad030, &H::flag_fd);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/en3d_helpers11.cpp (en3d/helpers11)
namespace {
namespace en3d_helpers11 {

using joan::call::import_call;

bool below_or_nan(float a, float b) { return !(a >= b); }

struct Vec3 {
    float x, y, z;
};

// Actor fields used here: +0x00 ActorDef* (+0x0c -> +0x2c mesh state, +0x14, +0x18 frame planes
// per animation, +0x1c their counts), +0x04 animation set, +0x08 clock select (-1: 0xa500b2,
// else 0xa500ba), +0x30 fade rate, +0x3c, +0x44 / +0x48 animation / sub, +0x4c start time,
// +0x50 looping, +0x54 last update time, +0x78 / +0x7c, +0x80, +0x84, +0x88, +0x8c bone,
// +0x90 plane, +0x94 current plane, +0x9c last LOD, +0xb0 / +0xb4 attachments.
std::uint32_t actor_clock(const void* actor) {
    return jo::as_actor(actor)->kind == -1 ? global<std::uint32_t>(d::g_game_time_ms) : global<std::uint32_t>(d::g_now);
}
float elapsed_float(std::uint32_t now, const void* actor) {  // fild qword of the unsigned difference
    return static_cast<float>(static_cast<double>(now - jo::as_actor(actor)->start_time));
}
void* anim_frame(const void* actor) {  // 0x005b5dd0
    return joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::as_actor(actor)->anim)->frame(joan::orig::arg::i32(jo::as_actor(actor)->action), joan::orig::arg::u32(static_cast<std::uint32_t>(static_cast<std::uint16_t>(jo::as_actor(actor)->sub_action)))));
}
// 0x005b5e40: skin / animate a plane for the actor at time t.
void animate(const void* actor, void* plane, float t, std::uint32_t looping, void* a, void* b,
             std::uint32_t c, const void* d, void* e) {
    joan::orig::as_anim_def(jo::as_actor(actor)->anim)->animate(joan::orig::arg::mptr(plane), joan::orig::arg::i32(jo::as_actor(actor)->action), joan::orig::arg::u32(static_cast<std::uint32_t>(static_cast<std::uint16_t>(jo::as_actor(actor)->sub_action))), joan::orig::arg::f32(t), joan::orig::arg::u32(looping), joan::orig::arg::mptr(a), joan::orig::arg::mptr(b), joan::orig::arg::f32(c), joan::orig::arg::mptr(d), joan::orig::arg::mptr(e));
}

JOAN_COUNTER(c_select_mesh, "Actor::select_mesh");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // 0x005ad060: returns `out_plane`.
    void** actor_select_mesh(void* camera, std::int32_t, void** out_plane, void** out_extra) {
        c_select_mesh.hit();
        const std::uint32_t now = actor_clock(this);
        void* const anim = jo::view<void*>(jo::self<jo::Actor>(this)->anim);
        void* mesh_state = nullptr;
        void* const def = jo::view<void*>(jo::self<jo::Actor>(this)->def);
        void* const def_mesh = jo::view<void*>(jo::as_actor_def(def)->model->skin);
        bool frame_planes = false;
        if (anim != nullptr) {
            const std::uint8_t* const frame = static_cast<std::uint8_t*>(anim_frame(this));
            frame_planes = jo::as_anim_action(frame)->value != 0 && jo::view<void**>(jo::as_actor_def(def)->mesh_list_18) != nullptr &&
                           jo::view<void**>(jo::as_actor_def(def)->mesh_list_18)[jo::self<jo::Actor>(this)->action] != nullptr;
        }
        if (anim != nullptr && !frame_planes) {
            // Skinned plane (with LOD).
            std::int32_t lod = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->lod(joan::orig::arg::mptr(camera)));  // 0x005d4760
            void* plane = nullptr;
            void* state = nullptr;
            if (lod != 0) plane = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->level(&lod, 1u));  // 0x005d45e0
            if (plane != nullptr) {
                state = joan::orig::arg::as<void*>(joan::orig::as_mesh(def_mesh)->lod(joan::orig::arg::i32(lod)));  // 0x005f4fb0
            } else {
                plane = jo::view<void*>(jo::self<jo::Actor>(this)->plane);
                state = def_mesh;
            }
            if (now != jo::self<jo::Actor>(this)->last_eval || jo::self<jo::Actor>(this)->last_lod != lod) {
                void* const registry = img::obj(d::g_particle_effect_array);
                if (joan::orig::as_dynarray(registry)->empty_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->effect))) == 0)  // 0x0041bd90
                    joan::orig::as_effect(joan::orig::as_dynarray(registry)->ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->effect))))->update();  // 0x0041bd60 -> 0x0041ab50
                void* const p78 = jo::self<jo::Actor>(this)->key_count != 0 ? static_cast<void*>(jo::self<jo::Actor>(this)->keys) : nullptr;
                animate(this, plane, elapsed_float(now, this), jo::self<jo::Actor>(this)->anim_flags, jo::as_actor_def(def)->bone_map, p78,
                        jo::self<jo::Actor>(this)->blend_ms, &jo::self<jo::Actor>(this)->cursor_owner, jo::self<jo::Actor>(this)->bone_mats);  // +0x84 (the key-cache owner)
                joan::orig::as_skin(state)->deform(joan::orig::arg::mptr(plane));  // 0x005f4c10
                jo::self<jo::Actor>(this)->last_eval = now;
                jo::self<jo::Actor>(this)->last_lod = lod;
            }
            *out_plane = lod != 0 ? plane : jo::view<void*>(jo::self<jo::Actor>(this)->plane);
            *out_extra = nullptr;
            mesh_state = lod != 0 ? joan::orig::arg::as<void*>(joan::orig::as_mesh(def_mesh)->lod(joan::orig::arg::i32(lod))) : def_mesh;
        } else if (anim != nullptr && jo::as_anim_action(anim_frame(this))->value == 1 && jo::view<void**>(jo::as_actor_def(def)->mesh_list_18) != nullptr &&
                   jo::view<void**>(jo::as_actor_def(def)->mesh_list_18)[jo::self<jo::Actor>(this)->action] != nullptr) {
            // Per-frame planes: pick the frame by time.
            float t = elapsed_float(now, this);
            float frame_time = 0.0f;
            void* const frame = anim_frame(this);
            if (jo::self<jo::Actor>(this)->anim_flags == 0) {
                if (below_or_nan(t, 0.0f)) t = 0.0f;
                if (below_or_nan(joan::orig::arg::as<float>(joan::orig::as_range(frame)->span()), t)) t = joan::orig::arg::as<float>(joan::orig::as_range(frame)->span());  // 0x0042d130 duration
            } else {
                const float span = jo::as_anim_action(frame)->end - jo::as_anim_action(frame)->start;
                t = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::fmod(joan::orig::arg::f64(static_cast<double>(t)), joan::orig::arg::f64(static_cast<double>(span)))));  // CRT fmod
            }
            const std::int32_t frames = jo::view<std::int32_t*>(jo::as_actor_def(def)->mesh_list_1c)[jo::self<jo::Actor>(this)->action];
            float q = t / joan::orig::arg::as<float>(joan::orig::as_range(frame)->span());
            q = q * static_cast<float>(frames - 1);
            q = q + 0.5f;
            std::int32_t index = static_cast<std::int32_t>(static_cast<std::int64_t>(q));  // _ftol
            if (index < 0)
                index = 0;
            else if (index >= jo::view<std::int32_t*>(jo::as_actor_def(def)->mesh_list_1c)[jo::self<jo::Actor>(this)->action])
                index = jo::view<std::int32_t*>(jo::as_actor_def(def)->mesh_list_1c)[jo::self<jo::Actor>(this)->action] - 1;
            if (jo::view<std::int32_t*>(jo::as_actor_def(def)->mesh_list_1c)[jo::self<jo::Actor>(this)->action] - 1 != 0) {
                const float r = static_cast<float>(index) / static_cast<float>(jo::view<std::int32_t*>(jo::as_actor_def(def)->mesh_list_1c)[jo::self<jo::Actor>(this)->action] - 1);
                frame_time = joan::orig::arg::as<float>(joan::orig::as_range(frame)->span()) * r;
            }
            void* const frame_plane = static_cast<void**>(jo::view<void**>(jo::as_actor_def(def)->mesh_list_18)[jo::self<jo::Actor>(this)->action])[index];
            std::int32_t lod = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->lod(joan::orig::arg::mptr(camera)));
            void* plane = nullptr;
            if (lod != 0) plane = joan::orig::arg::as<void*>(joan::orig::as_plane(frame_plane)->level(&lod, 1u));
            if (plane == nullptr) {
                plane = frame_plane;
                *out_extra = jo::self<jo::Actor>(this)->bone;
            }
            if (jo::self<jo::Actor>(this)->last_eval != now) {
                animate(this, jo::view<void*>(jo::self<jo::Actor>(this)->plane), frame_time, jo::self<jo::Actor>(this)->anim_flags, jo::as_actor_def(def)->bone_map, nullptr, 0, nullptr, nullptr);
                jo::self<jo::Actor>(this)->last_eval = now;
            }
            *out_plane = plane;
        } else {
            // Static plane (with LOD).
            std::int32_t lod = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->lod(joan::orig::arg::mptr(camera)));
            void* plane = nullptr;
            if (lod != 0) plane = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->level(&lod, 1u));
            if (plane == nullptr) {
                plane = jo::view<void*>(jo::self<jo::Actor>(this)->plane);
                *out_extra = jo::self<jo::Actor>(this)->bone;
            }
            *out_plane = plane;
            *out_extra = nullptr;
        }
        // The original works out the distance from the camera to the world bounding sphere here (0x005acc80,
        // 0x0060c310 of 0x00616e90, 0x0060cc90, length) and never uses it: left out (its only side effect is the
        // bone's lazy update, which the draw that follows makes anyway). Then the attachments.
        for (std::int32_t i = 0; i < jo::self<jo::Actor>(this)->morph_count; ++i)
            joan::orig::as_morph(jo::view<void**>(jo::self<jo::Actor>(this)->morphs)[i])->update(joan::orig::arg::mptr(*out_plane), joan::orig::arg::mptr(mesh_state));  // 0x005c8960
        jo::self<jo::Actor>(this)->last_model = *out_plane;
        return out_plane;
    }
};

JOAN_REPLACE("en3d/helpers11", "Actor::get_render_plane", 0x005ad060, &H::actor_select_mesh);

}  // namespace en3d_helpers11
}  // namespace

// ---------------------------------------- from modules/game_b107_17.cpp (game/b107_17)
//
//    - Actor::add_child 0x005ae0c0 (thiscall, 2 args, ret 8): grows the pointer array +0xb4
//      (capacity +0xac, count +0xb0) through 0x0058eb30 on 0xa48838 with "actor.cpp", line
//      [0xa16e58] + 4; new 0x84 bytes + ctor 0x005c67b0, init 0x005c7370(a, b); returns this.
//      SEH frame only guards the constructor (normal path kept).
namespace {
namespace game_b107_17 {


JOAN_COUNTER(c_add_child, "Actor::add_child");

struct H {
    // ---- 0x005ae0c0 ----
    H* add_child(void* a, u32 b) {
        c_add_child.hit();
        if (jo::self<jo::Actor>(this)->morph_cap == jo::self<jo::Actor>(this)->morph_count) {
            jo::self<jo::Actor>(this)->morph_cap = jo::self<jo::Actor>(this)->morph_cap + 2;
            const i32 line = static_cast<i32>(global<i16>(d::g_a16e58)) + 4;
            jo::view<void*>(jo::self<jo::Actor>(this)->morphs) = joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Actor>(this)->morphs)), joan::orig::arg::u32(static_cast<u32>(jo::self<jo::Actor>(this)->morph_cap) * static_cast<u32>(sizeof(jo::MorphPtr))), joan::orig::arg::str(&img::global<const char>(d::s_d_ww_en3d_actor_cpp)), joan::orig::arg::i32(line));  // 0x0058eb30
        }
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Morph)));  // 0x84 on x86; operator new
        void* const child = mem != nullptr ? joan::orig::as_morph(mem)->construct() : nullptr;  // 0x005c67b0
        joan::orig::as_morph(child)->copy_from(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b));                                         // 0x005c7370 (ret 8)
        jo::view<void**>(jo::self<jo::Actor>(this)->morphs)[jo::self<jo::Actor>(this)->morph_count] = child;
        jo::self<jo::Actor>(this)->morph_count = jo::self<jo::Actor>(this)->morph_count + 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_17", name, addr, fn)
B("Actor::create_morph_control_5ae0c0", 0x005ae0c0, &H::add_child);
#undef B

}  // namespace game_b107_17
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (en3d/b82_17)
//
//      0x005ae400 Actor::attach(name, a, b) (ret 0xc) - first part named name (+1) gets a new
//                 0x10 object (0x005cae20 / 0x005cae80), +0x7c = time, +0x78 = 1
namespace {
namespace en3d_b82_17_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::game_new;   // operator new
std::int32_t stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

JOAN_COUNTER(c_attach, "Actor::attach");

struct H {
    // 0x005ae400
    void attach(const char* name, const char* a, const char* b) {
        c_attach.hit();
        for (std::int32_t i = 0; i < jo::self<jo::Actor>(this)->morph_count; ++i) {
            if (stricmp_(name, jo::view<std::uint8_t**>(jo::self<jo::Actor>(this)->morphs)[i] + 1) != 0) continue;
            void* mem = game_new(static_cast<std::uint32_t>(sizeof(jo::MorphAnim)));  // 0x10 on x86
            void* o = mem != nullptr ? joan::orig::as_morph_anim(mem)->construct() : nullptr;  // 0x005cae20
            jo::view<void*>((jo::as_morph(jo::view<std::uint8_t**>(jo::self<jo::Actor>(this)->morphs)[i]))->anim) = o;
            std::uint8_t* const part = jo::view<std::uint8_t**>(jo::self<jo::Actor>(this)->morphs)[i];
            joan::orig::as_morph_anim(jo::view<void*>((jo::as_morph(jo::view<std::uint8_t**>(jo::self<jo::Actor>(this)->morphs)[i]))->anim))->load(a, b, joan::orig::arg::mptr(static_cast<void*>(part)));  // 0x005cae80
            (jo::as_morph(jo::view<std::uint8_t**>(jo::self<jo::Actor>(this)->morphs)[i]))->anim_start = global<std::uint32_t>(d::g_now);
            (jo::as_morph(jo::view<std::uint8_t**>(jo::self<jo::Actor>(this)->morphs)[i]))->anim_playing = 1;
            return;
        }
    }
};

#define B82_17E(name, addr, fn) JOAN_REPLACE("en3d/b82_17", name, addr, fn)
B82_17E("Actor::load_morph_animation", 0x005ae400, &H::attach);
#undef B82_17E

}  // namespace en3d_b82_17_2
}  // namespace

// ---------------------------------------- from modules/game_actors.cpp (game/actors1)
//
//  * module game/actors1: Actor::set_faded 0x005ae590 (see-through when blocking the
//    camera), Actor::set_position 0x005ac310 (moves the bone and the owner), getters
//    0x005aede0 / 0x005aee10 / 0x005d5eb0. (Actor::visible / can_draw are in en3d/helpers4 /
//    en3d/helpers.)
namespace {
namespace game_actors_2 {

using joan::call::thiscall;

JOAN_COUNTER(c_faded, "Actor::set_faded");

std::uint8_t* material(void* plane, std::int32_t i) { return jo::view<std::uint8_t*>(jo::as_plane(plane)->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material)); }
void plane_set_flag(void* plane, std::int32_t on) { joan::orig::as_plane(plane)->set_104(joan::orig::arg::u32(on)); }      // 0x005d6e80
void plane_set_colour(void* plane, std::uint32_t c) { joan::orig::as_plane(plane)->set_fade(joan::orig::arg::u32(c)); }    // 0x005d5e80
std::uint32_t packed_colour(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    std::uint32_t tmp = 0, packed = 0;
    void* const c = joan::orig::as_colour(&tmp)->construct(r, g, b, a);  // 0x0042a260
    joan::orig::as_colour(&packed)->from_rgba(joan::orig::arg::mptr(c));                         // 0x005e95b0
    return packed;
}
// Copies the blend modes (+0x38 / +0x40) of the definition's materials into a plane: one to
// one when the counts match, else the first one to all.
void restore_blend(void* source, void* plane) {
    if (jo::as_plane(source)->material_count == jo::as_plane(plane)->material_count) {
        for (std::int32_t i = 0; i < jo::as_plane(plane)->material_count; ++i) {
            (jo::as_material(material(plane, i)))->blend = (jo::as_material(material(source, i)))->blend;
            (jo::as_material(material(plane, i)))->pass_mask = (jo::as_material(material(source, i)))->pass_mask;
        }
    } else if (jo::as_plane(source)->material_count >= 1) {
        for (std::int32_t i = 0; i < jo::as_plane(plane)->material_count; ++i) {
            (jo::as_material(material(plane, i)))->blend = (jo::as_material(material(source, 0)))->blend;
            (jo::as_material(material(plane, i)))->pass_mask = (jo::as_material(material(source, 0)))->pass_mask;
        }
    }
}
void set_alpha_blend(void* plane) {
    for (std::int32_t i = 0; i < jo::as_plane(plane)->material_count; ++i) {
        std::uint8_t* const m = material(plane, i);
        jo::as_material(m)->blend = 1;
        jo::as_material(m)->pass_mask = 2;
        jo::as_material(m)->raster = jo::as_material(m)->raster & 0xfffff0ffu;
        jo::as_material(m)->raster = jo::as_material(m)->raster | 0x100u;
    }
}

struct H {

    // 0x005aede0 / 0x005aee10 (registry names Actor::*): called on the actor's Plane (+0xe4 lod_count, +0xd0 children)
    std::int32_t sub_count() { return static_cast<std::int8_t>(jo::self<jo::Plane>(this)->lod_count); }  // 0x005aede0
    void** sub_planes() { return reinterpret_cast<void**>(jo::self<jo::Plane>(this)->children); }  // 0x005aee10: this + 0xd0

    void set_faded(char on) {  // 0x005ae590
        c_faded.hit();
        jo::self<jo::Actor>(this)->faded = static_cast<std::int32_t>(on);
        if (jo::view<void*>(jo::self<jo::Actor>(this)->plane) == nullptr) return;
        void* const mesh = jo::view<void*>(jo::self<jo::Actor>(this)->plane);
        if (jo::self<jo::Actor>(this)->faded == 0) {
            plane_set_flag(mesh, 0);
            plane_set_colour(mesh, packed_colour(0xff, 0xff, 0xff, 0xff));
            void* const source = jo::view<void*>(jo::self<jo::Actor>(this)->def->model->mesh);
            restore_blend(source, jo::view<void*>(jo::self<jo::Actor>(this)->plane));
            for (std::int32_t j = 0; j < joan::orig::arg::as<std::int32_t>(joan::orig::as_actor(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->sub_count()); ++j) {  // 0x005aede0
                void** const subs = joan::orig::arg::as<void**>(joan::orig::as_actor(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->sub_planes());                    // 0x005aee10
                if (subs[j] == nullptr) continue;
                plane_set_flag(subs[j], 0);
                restore_blend(jo::view<void*>(jo::self<jo::Actor>(this)->def->model->mesh), subs[j]);
            }
            return;
        }
        plane_set_flag(mesh, 1);
        set_alpha_blend(jo::view<void*>(jo::self<jo::Actor>(this)->plane));
        for (std::int32_t j = 0; j < joan::orig::arg::as<std::int32_t>(joan::orig::as_actor(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->sub_count()); ++j) {
            void** const subs = joan::orig::arg::as<void**>(joan::orig::as_actor(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->sub_planes());
            if (subs[j] == nullptr) continue;
            plane_set_flag(subs[j], 1);
            std::uint32_t c = 0;
            std::uint32_t colour = *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_plane(subs[j])->colour(&c));  // 0x005d5eb0
            colour = (colour & 0x00ffffffu) | 0x60000000u;
            plane_set_colour(subs[j], colour);
            set_alpha_blend(subs[j]);
        }
        plane_set_colour(jo::view<void*>(jo::self<jo::Actor>(this)->plane), packed_colour(0xff, 0xff, 0xff, 0x60));
    }
};

#define A1(name, addr, fn) JOAN_REPLACE("game/actors1", name, addr, fn)
A1("Actor::sub_count", 0x005aede0, &H::sub_count);
A1("Actor::sub_planes", 0x005aee10, &H::sub_planes);
A1("Actor::set_tfactor_enabled", 0x005ae590, &H::set_faded);
#undef A1

}  // namespace game_actors_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers_2 {


// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint32_t actor_layer() {  // 0x005aed50
        const std::uint32_t type = jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->kind);
        if (type == 0x2 || type == 0x27 || type == 0x10) return 3;
        return type == 0xb ? 1 : 2;
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Actor::layer", 0x005aed50, &H::actor_layer);
#undef HELPER

}  // namespace en3d_helpers_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers4.cpp (en3d/helpers4)
//
//  en3d leaf helpers, batch 4: actor visibility (Actor::bounding_sphere).
namespace {
namespace en3d_helpers4_2 {

using joan::call::import_call;

struct Vec3 {
    float x, y, z;
};

struct H {
    // --- Actor (en3d/actor.cpp) ---------------------------------------------------------------------
    // 0x005acc80: bounding sphere (definition +0 -> +0xc -> +0x34, centre + radius); with `world`,
    // moved to the attachment (+0x10 kind 1 / 2 via +0x14) or the bone (+0x8c) and the radius
    // scaled by +0x28.
    float* actor_bounding_sphere(float* out, std::uint32_t world) {
        std::memcpy(out, jo::self<jo::Actor>(this)->def->model->sphere, sizeof(jo::Model::sphere));  // model + 0x34, 16 bytes
        if ((world & 0xff) == 0) return out;
        if (jo::self<jo::Actor>(this)->holder_kind != 0) {
            Vec3 t;
            if (jo::self<jo::Actor>(this)->holder_kind == 1)
                joan::orig::vec_add_in_place(joan::orig::arg::mptr(out), joan::orig::arg::ptr(joan::orig::arg::as<Vec3*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Actor>(this)->holder))->position(&t))));  // 0x005aee40
            else if (jo::self<jo::Actor>(this)->holder_kind == 2)
                joan::orig::vec_add_in_place(joan::orig::arg::mptr(out), joan::orig::arg::ptr(joan::orig::arg::as<Vec3*>(joan::orig::as_effect_state(jo::view<void*>(jo::self<jo::Actor>(this)->holder))->position(&t))));  // 0x005aee80
        } else {
            void* const bone = jo::view<void*>(jo::self<jo::Actor>(this)->bone);
            if (jo::as_bone(bone)->parent == nullptr)
                import_call<void*>(d::iat_v3d_87_point_transform_2, out, bone);  // point_transform in place
            else
                import_call<void*>(d::iat_v3d_87_point_transform_2, out, joan::orig::arg::as<void*>(joan::orig::as_bone(bone)->matrix()));  // 0x0060ef80
        }
        out[3] = out[3] * jo::self<jo::Actor>(this)->scale;
        return out;
    }

};

#define HELPER4(name, addr, fn) JOAN_REPLACE("en3d/helpers4", name, addr, fn)
HELPER4("Actor::get_bounding_sphere", 0x005acc80, &H::actor_bounding_sphere);
#undef HELPER4

}  // namespace en3d_helpers4_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers32.cpp (en3d/helpers32)
namespace {
namespace en3d_helpers32 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::stdcall_call;


JOAN_COUNTER(c_scene_init, "SceneObject::init");

void* op_new(std::uint32_t size) { return joan::crt::operator_new(joan::orig::arg::u32(size)); }

struct H {
    // 0x005aab20 SceneObject::SceneObject().
    void* scene_ctor() {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::Actor));  // memset 0x10c (x86)
        jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->scale) = 0x3f800000u;
        jo::self<jo::Actor>(this)->visible = 1;
        return this;
    }

    // 0x005aab70 SceneObject::init().
    void* scene_init() {
        c_scene_init.hit();
        if (jo::view<void*>(jo::self<jo::Actor>(this)->bone) == nullptr) {
            void* bone = op_new(static_cast<std::uint32_t>(sizeof(jo::Bone)));  // 0xa0 on x86
            if (bone != nullptr)
                bone = joan::orig::arg::as<void*>(joan::orig::as_bone(bone)->Bone_60eae0(static_cast<void*>(this), fn::code_ptr(fn::empty_cb_5aaec0)));  // 0x0060eae0
            jo::view<void*>(jo::self<jo::Actor>(this)->bone) = bone;
            std::memcpy(jo::view<void*>(jo::self<jo::Actor>(this)->bone), img::obj(d::g_identity_matrix), 0x40);
            joan::orig::as_bone(jo::view<void*>(jo::self<jo::Actor>(this)->bone))->invalidate();  // 0x0060ee10
        }
        void* plane = op_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
        if (plane != nullptr) {
            void* const mesh = jo::view<void*>(jo::self<jo::Actor>(this)->def->model->mesh);
            plane = joan::orig::arg::as<void*>(joan::orig::as_plane(plane)->Plane_copy(joan::orig::arg::mptr(mesh), 1u));  // 0x005cbbf0
        }
        jo::view<void*>(jo::self<jo::Actor>(this)->plane) = plane;
        jo::self<jo::Actor>(this)->plane->list_kind = 4;
        joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->set_owner(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x005d03f0
        jo::self<jo::Actor>(this)->plane->load_state = 0;
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::Actor>(this)->plane->bone_count); ++i) {
            std::uint8_t* const sub = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->bone_5cf8f0(joan::orig::arg::i32(i)));  // 0x005cf8f0
            if (jo::as_bone(sub)->parent == nullptr) {
                void* const bone = jo::view<void*>(jo::self<jo::Actor>(this)->bone);
                void* const sub2 = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Actor>(this)->plane))->bone_5cf8f0(joan::orig::arg::i32(i)));
                joan::orig::as_bone(sub2)->set_parent(joan::orig::arg::mptr(bone));  // 0x0060ec70
            }
        }
        const std::int32_t n = jo::self<jo::Actor>(this)->plane->bone_count;
        void* keys = op_new(static_cast<std::uint32_t>(n) << 5);
        if (keys != nullptr)
            joan::orig::vector(joan::orig::arg::mptr(keys), 0x20, joan::orig::arg::i32(n), joan::orig::arg::mptr(reinterpret_cast<void*>(fn::code_value(fn::AnimKey_AnimKey))));  // 0x00449af0 vector ctor
        jo::view<void*>(jo::self<jo::Actor>(this)->keys) = keys;
        jo::self<jo::Actor>(this)->key_count = 0;
        jo::self<jo::Actor>(this)->cursor_owner = nullptr;  // the key-cache owner
        if (jo::self<jo::Actor>(this)->def->model->kind == 1)
            jo::view<void*>(jo::self<jo::Actor>(this)->bone_mats) = op_new(static_cast<std::uint32_t>(jo::self<jo::Actor>(this)->plane->bone_count) << 2);  // one i32 key index per bone (4 bytes on every architecture)
        else
            jo::view<void*>(jo::self<jo::Actor>(this)->bone_mats) = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->action) = 1;
        jo::self<jo::Actor>(this)->start_time = global<std::uint32_t>(d::g_now);
        jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->scale) = 0x3f800000u;
        jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->alpha) = 0xff;
        jo::self<jo::Actor>(this)->flag_fd_ = 1;
        jo::view<std::uint32_t>(jo::self<jo::Actor>(this)->faded) = 0;
        return this;
    }
};

#define HELPER32(name, addr, fn) JOAN_REPLACE("en3d/helpers32", name, addr, fn)
HELPER32("SceneObject::init", 0x005aab70, &H::scene_init);
HELPER32("Actor::Actor", 0x005aab20, &H::scene_ctor);
#undef HELPER32

}  // namespace en3d_helpers32
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
namespace {
namespace game_b105_16_2 {

// The original leaves the caller's eax (= ctx, loaded by 0x0060ee10 right before the call) and
// 0x0060ee10 passes that eax on as its own return value - return ctx to keep it.
void* JOAN_CDECL empty_cb(void* ctx) { return ctx; }

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("empty_cb_5aaec0", 0x005aaec0, &empty_cb);
#undef B

}  // namespace game_b105_16_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers13.cpp (en3d/helpers13)
namespace {
namespace en3d_helpers13 {

using joan::call::thiscall;

struct H {
    H* key_construct() {                                                          // 0x005aaee0
        joan::orig::as_quaternion(jo::self<jo::AnimKey>(this)->rot)->construct();  // 0x005ac230 quaternion ctor
        return this;
    }
};

#define HELPER13(name, addr, fn) JOAN_REPLACE("en3d/helpers13", name, addr, fn)
HELPER13("AnimKey::AnimKey", 0x005aaee0, &H::key_construct);
#undef HELPER13

}  // namespace en3d_helpers13
}  // namespace

// ---------------------------------------- from modules/game_b102_10.cpp (game/b102_10)
namespace {
namespace game_b102_10 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;


i32 crt_stricmp(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

struct H {
    // 0x005ae220
    void call_by_name(const char* name, u32 arg) {
        for (i32 i = 0; i < jo::self<jo::Actor>(this)->morph_count; ++i) {
            if (crt_stricmp(name, jo::view<u8**>(jo::self<jo::Actor>(this)->morphs)[i] + 1) == 0) {
                joan::orig::as_morph(jo::view<u8**>(jo::self<jo::Actor>(this)->morphs)[i])->select(joan::orig::arg::str(arg));  // 0x005c7ff0
                return;
            }
        }
    }
    // 0x005ae2c0
    void call_by_name2(const char* name, u32 a, u32 b) {
        for (i32 i = 0; i < jo::self<jo::Actor>(this)->morph_count; ++i) {
            if (crt_stricmp(name, jo::view<u8**>(jo::self<jo::Actor>(this)->morphs)[i] + 1) == 0) {
                joan::orig::as_morph(jo::view<u8**>(jo::self<jo::Actor>(this)->morphs)[i])->set_frame(joan::orig::arg::u32(a), joan::orig::arg::i32(b));  // 0x005c80d0
                return;
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_10", name, addr, fn)
B("Actor::set_morph_state", 0x005ae220, &H::call_by_name);
B("Actor::morph_to_state_5ae2c0", 0x005ae2c0, &H::call_by_name2);
#undef B

}  // namespace game_b102_10
}  // namespace

// ---------------------------------------- from modules/game_b105_16_3.cpp (game/b105_16)
namespace {
namespace game_b105_16_3 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;


JOAN_COUNTER(c_find_name, "Obj::find_name_5ae360");

struct H {
    // ---------------------------------------------------------------- 0x005ae360
    u32 find_name_5ae360(const char* name, u32 a, u32 b, u32 c) {
        c_find_name.hit();
        i32 i = 0;
        for (; i < jo::self<jo::Actor>(this)->morph_count; ++i) {
            u8* const e = jo::view<u8**>(jo::self<jo::Actor>(this)->morphs)[i];
            if (joan::crt::stricmp(joan::orig::arg::str(name), joan::orig::arg::str(reinterpret_cast<const char*>(e + 1))) == 0)
                return joan::orig::arg::as<u32>(joan::orig::as_morph(jo::view<u8**>(jo::self<jo::Actor>(this)->morphs)[i])->blend_to(joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c)));  // 0x005c8360
        }
        return static_cast<u32>(i);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Actor::morph_to_state_5ae360", 0x005ae360, &H::find_name_5ae360);
#undef B

}  // namespace game_b105_16_3
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
namespace {
namespace en3d_b85_04_2 {

using joan::call::cdecl_call;


std::int32_t game_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

JOAN_COUNTER(c_defs_find, "ActorDefs::find");

struct H {
    // 0x005aec90
    void* defs_find(const char* name) {
        c_defs_find.hit();
        for (std::int32_t i = 0; i < jo::self<jo::Actor>(this)->morph_count; ++i) {
            if (game_stricmp(name, jo::view<char**>(jo::self<jo::Actor>(this)->morphs)[i] + 1) == 0) return jo::view<void**>(jo::self<jo::Actor>(this)->morphs)[i];
        }
        return nullptr;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("ActorDefs::find", 0x005aec90, &H::defs_find);
#undef E3D

}  // namespace en3d_b85_04_2
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
namespace {
namespace game_b103_11_2 {

using u32 = std::uint32_t;


struct H {
    // 0x005aed20
    u32 field_b0() { return jo::view<u32>(jo::self<jo::Actor>(this)->morph_count); }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Obj::field_b0", 0x005aed20, &H::field_b0);
#undef B103

}  // namespace game_b103_11_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers_3 {


// Every helper shares one class so `this` is just the object's address.
struct H {
    void* plane_skin_owner() { return jo::view<void*>(jo::self<jo::Plane>(this)->parent); }  // 0x005aedb0
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Plane::skin_owner", 0x005aedb0, &H::plane_skin_owner);
#undef HELPER

}  // namespace en3d_helpers_3
}  // namespace

// ---------------------------------------- from modules/en3d_helpers4.cpp (en3d/helpers4)
namespace {
namespace en3d_helpers4_3 {


struct Vec3 {
    float x, y, z;
};

struct H {
    Vec3* attach_position_9(Vec3* out) {  // 0x005aee40
        *out = jo::view<Vec3>(jo::self<jo::Billboard>(this)->pos);
        return out;
    }
    Vec3* attach_position_44(Vec3* out) {  // 0x005aee80
        *out = jo::view<Vec3>(jo::self<jo::EffectState>(this)->pos);
        return out;
    }
};

#define HELPER4(name, addr, fn) JOAN_REPLACE("en3d/helpers4", name, addr, fn)
HELPER4("Attachment::position_9", 0x005aee40, &H::attach_position_9);
HELPER4("Attachment::position_44", 0x005aee80, &H::attach_position_44);
#undef HELPER4

}  // namespace en3d_helpers4_3
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SceneObject::SceneObject_5aab20() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers32::H::scene_ctor>(this)); }  // 0x5aab20
auto joan::orig::SceneObject::init() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers32::H::scene_init>(this)); }  // 0x5aab70
auto joan::orig::AnimKey::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers13::H::key_construct>(this)); }  // 0x5aaee0
auto joan::orig::Actor::release_5aaf10() -> void { joan::bind::member<&::game_units4::H::actor_release>(this); }  // 0x5aaf10
auto joan::orig::Bone::delete_(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers20::H::bone_delete>(this, a0)); }  // 0x5ab400
auto joan::orig::Actor::on_release() -> void { joan::bind::member<&::game_b104_09::H::stub_void>(this); }  // 0x5ab450
auto joan::orig::Actor::visible_5ab470(void* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers4::H::actor_visible>(this, a0, a1)); }  // 0x5ab470
auto joan::orig::Actor::visible_5ab620() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::actor_visible>(this)); }  // 0x5ab620
auto joan::orig::Actor::choose_mesh(void* a0, std::int32_t a1, void* a2, void* a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::actor_choose_mesh>(this, a0, a1, a2, a3)); }  // 0x5ab6a0
auto joan::orig::Actor::fade_update(void* a0) -> void { joan::bind::member<&::en3d_helpers9::H::actor_fade_update>(this, a0); }  // 0x5ab6e0
auto joan::orig::Actor::draw_entry(void* a0, void* a1, std::int32_t a2) -> void { joan::bind::member<&::en3d_helpers8::H::actor_draw_entry>(this, a0, a1, a2); }  // 0x5abc50
auto joan::orig::Actor::start_anim(void* a0, const char* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> void { joan::bind::member<&::en3d_b82_17::H::start_anim>(this, a0, a1, a2, a3, a4, a5); }  // 0x5abe30
auto joan::orig::Quaternion::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::quaternion_ctor>(this)); }  // 0x5ac230
auto joan::orig::Actor::position(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves80a::H::position>(this, a0)); }  // 0x5ac270
auto joan::orig::Actor::set_position_5ac310(void* a0) -> void { joan::bind::member<&::game_actors::H::set_position>(this, a0); }  // 0x5ac310
auto joan::orig::Actor::set_position_5ac4b0(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::en3d_b85_04::H::actor_set_pos>(this, a0, a1, a2); }  // 0x5ac4b0
auto joan::orig::SceneObject::set_yaw(std::uint32_t a0) -> void { joan::bind::member<&::game_fx5::H::set_yaw>(this, a0); }  // 0x5ac6a0
auto joan::orig::SceneObject::yaw() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_fx5::H::yaw>(this)); }  // 0x5ac7e0
auto joan::orig::SceneObject::set_pitch(std::uint32_t a0) -> void { joan::bind::member<&::game_fx5::H::set_pitch>(this, a0); }  // 0x5ac810
auto joan::orig::SceneObject::set_roll(std::uint32_t a0) -> void { joan::bind::member<&::game_fx5::H::set_roll>(this, a0); }  // 0x5ac980
auto joan::orig::SceneObject::set_angles(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_fx5::H::set_angles>(this, a0, a1, a2); }  // 0x5acaf0
auto joan::orig::Actor::get_xyz(void* a0, void* a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17::H::get_xyz>(this, a0, a1, a2)); }  // 0x5acc30
auto joan::orig::Actor::bounding_sphere(void* a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers4_2::H::actor_bounding_sphere>(this, a0, a1)); }  // 0x5acc80
auto joan::orig::Actor::matrix(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_03::H::actor_matrix>(this, a0)); }  // 0x5ace10
auto joan::orig::SceneNode::set_scene(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_fx::H::node_set_scene>(this, a0)); }  // 0x5acf10
auto joan::orig::Actor::set_scene(Scene* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17::H::part_set>(this, a0)); }  // 0x5acf60
auto joan::orig::Actor::set_byte_fc(std::int8_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_11::H::set_byte_fc>(this, a0)); }  // 0x5acfa0
auto joan::orig::Actor::set_byte_fd(std::uint8_t a0) -> void { joan::bind::member<&::game_b91_03::H::set_byte_fd>(this, a0); }  // 0x5ad000
auto joan::orig::Actor::flag_fd() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_06::H::flag_fd>(this)); }  // 0x5ad030
auto joan::orig::Actor::select_mesh(void* a0, std::int32_t a1, void* a2, void* a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers11::H::actor_select_mesh>(this, a0, a1, a2, a3)); }  // 0x5ad060
auto joan::orig::Anim::select_frame_5ad830(void* a0, void* a1) -> void { joan::bind::member<&::game_b105_16::H::select_frame_5ad830>(this, a0, a1); }  // 0x5ad830
auto joan::orig::Actor::set_skin(std::uint32_t a0) -> void { joan::bind::member<&::en3d_b82_17::H::set_skin>(this, a0); }  // 0x5add40
auto joan::orig::Actor::add_part(const char* a0, const char* a1, const char* a2, void* a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17::H::add_part>(this, a0, a1, a2, a3)); }  // 0x5adf50
auto joan::orig::Actor::add_child(void* a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b107_17::H::add_child>(this, a0, a1)); }  // 0x5ae0c0
auto joan::orig::Actor::attach(const char* a0, const char* a1, const char* a2) -> void { joan::bind::member<&::en3d_b82_17_2::H::attach>(this, a0, a1, a2); }  // 0x5ae400
auto joan::orig::Actor::set_faded(char a0) -> void { joan::bind::member<&::game_actors_2::H::set_faded>(this, a0); }  // 0x5ae590
auto joan::orig::Actor::find_morph(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b85_04_2::H::defs_find>(this, a0)); }  // 0x5aec90
auto joan::orig::Actor::layer() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers_2::H::actor_layer>(this)); }  // 0x5aed50
auto joan::orig::Plane::skin_owner() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers_3::H::plane_skin_owner>(this)); }  // 0x5aedb0
auto joan::orig::Actor::sub_count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_actors_2::H::sub_count>(this)); }  // 0x5aede0
auto joan::orig::Actor::sub_planes() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_actors_2::H::sub_planes>(this)); }  // 0x5aee10
auto joan::orig::Billboard::position(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers4_3::H::attach_position_9>(this, a0)); }  // 0x5aee40
auto joan::orig::EffectState::position(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers4_3::H::attach_position_44>(this, a0)); }  // 0x5aee80
