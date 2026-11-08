// SPDX-License-Identifier: GPL-3.0-or-later
// en2d/icon3d.cpp: functions of the original d:\ww\en2d\icon3d.cpp (re/attribution.csv). The file name is the
// Restaurant Empire 2 engine's (en2d/icon3d.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using u8 = std::uint8_t;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using i32 = std::int32_t;

using joan::call::thiscall;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b101_12.cpp (game/b101_12)
namespace {
namespace game_b101_12 {

using joan::call::cdecl_call;

using joan::img::str;

using joan::orig::helpers::op_new;

JOAN_COUNTER(c_model_load, "ModelHolder::load");

// Material of the holder's model: [[[holder] + 0x90] + 0x6c].
jo::Material* model_material(void* holder) { return jo::as_overlay2d(holder)->actor->plane->materials; }

struct H {
    // 0x00628ed0
    std::uintptr_t model_load(const char* name, const char* anim) {
        c_model_load.hit();
        void* const m = op_new(static_cast<u32>(sizeof(jo::SceneObject)));  // 0x10c on x86
        void* const holder = m != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(m)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor) = holder;
        void* const mm = op_new(static_cast<u32>(sizeof(jo::ActorDef)));  // 0x2d8 on x86
        void* const model = mm != nullptr ? joan::orig::as_actor_def(mm)->construct() : nullptr;  // 0x005af390
        jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor->def) = model;
        if (anim != nullptr) {
            void* const ma = op_new(static_cast<u32>(sizeof(jo::AnimDef)));  // 0x18b on x86
            void* const an = ma != nullptr ? joan::orig::as_anim_def(ma)->construct() : nullptr;  // 0x005b2390
            jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor->anim) = an;
            joan::orig::as_anim_def(jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor->anim))->load(str(d::s_ANIM), joan::orig::arg::str(anim), 0u);  // 0x005b23d0
            joan::orig::as_actor_def(jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor->def))->load2(str(d::s_MODEL_9d3750), joan::orig::arg::str(name), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), anim, jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor->anim));  // 0x005af3d0
        } else {
            joan::orig::as_actor_def(jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor->def))->load(str(d::s_MODEL_9d3750), joan::orig::arg::str(name), joan::orig::arg::i16(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));  // 0x005afdd0
        }
        joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor))->init();  // 0x005aab70
        model_material(this)->blend = 3;
        if ((model_material(this)->texture_mode & 0x20000u) != 0) {
            u32 v = 0x20000;
            switch (model_material(this)->texture_mode & 3u) {
            case 1:
                v |= 1;
                break;
            case 2:
                v |= 2;
                break;
            default:
                break;
            }
            switch (model_material(this)->texture_mode & 0x30u) {
            case 0x10:
                v |= 0x10;
                break;
            case 0x20:
                v |= 0x20;
                break;
            default:
                break;
            }
            model_material(this)->texture_mode = v;
        } else if ((model_material(this)->texture_mode & 0x10000u) == 0) {
            model_material(this)->texture_mode = 0x10002;
        }
        model_material(this)->pass_mask = 2;
        return joan::orig::arg::as<u32>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor->plane))->invalidate(1u));  // 0x005cf920
    }
};

#define R12(name, addr, fn) JOAN_REPLACE("game/b101_12", name, addr, fn)
R12("Icon3D::init", 0x00628ed0, &H::model_load);
#undef R12

}  // namespace game_b101_12
}  // namespace

