// game/unknown_mlight.cpp: functions whose original source file is unknown (formerly game/_unplaced/mlight.cpp;
// re/attribution.csv: between ww/en3d/light.cpp and ww/en3d/morph.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::img::obj;

using u32 = std::uint32_t;

using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
namespace {
namespace en3d_b83_03 {

struct H {
    // 0x005c5870 (eax from 0x005c5b50)
    std::uint32_t shadow_model_dtor() { joan::orig::as_light(this)->release(); return 0; }  // eax dropped by the caller
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("ShadowModel::~ShadowModel", 0x005c5870, &H::shadow_model_dtor);
#undef B83_03

}  // namespace en3d_b83_03
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
namespace {
namespace game_b105_16 {

u32 JOAN_CDECL cb_5c5b30(void* ctx) { joan::orig::as_light(ctx)->invalidate(); return 0; }  // 0x005c5c00

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("cb_5c5b30", 0x005c5b30, &cb_5c5b30);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16 {

JOAN_COUNTER(c_ambient, "lights_ambient");
JOAN_COUNTER(c_light_assign, "Light::assign");

// 0x005c65e0: sum of the ambient bytes of every active light, clamped, alpha 0xff.
std::uint32_t JOAN_CDECL lights_ambient() {
    c_ambient.hit();
    std::uint32_t r = 0, g = 0, b = 0;
    for (std::int32_t i = 0; i < global<std::int32_t>(d::g_ambient_light_count); ++i) {
        const std::uint8_t* const light = (&img::global<const std::uint8_t*>(d::g_ambient_light_list))[i];
        r += jo::as_light(light)->colour.r;
        g += jo::as_light(light)->colour.g;
        b += jo::as_light(light)->colour.b;
    }
    const std::uint32_t c0 = r < 0xff ? r : 0xff, c1 = g < 0xff ? g : 0xff, c2 = b < 0xff ? b : 0xff;
    return (c0 & 0xff) | ((c1 & 0xff) << 8) | ((c2 & 0xff) << 16) | 0xff000000u;
}

struct H {
    bool light_equals(const std::uint8_t* o) {  // 0x005c6560
        return jo::self<jo::Light>(this)->type == jo::as_light(o)->type && std::memcmp(jo::self<jo::Light>(this)->position, jo::as_light(o)->position, offsetof(jo::Light, dirty) - offsetof(jo::Light, position)) == 0;
    }
    // 0x005c5b50 Light::release: bone (+4) deleted, unlinked from its list (+8).
    void light_release() {
        if (jo::view<void*>(jo::self<jo::Light>(this)->bone_) != nullptr) {
            void* const bone = jo::view<void*>(jo::self<jo::Light>(this)->bone_);
            if (bone != nullptr) joan::orig::as_bone(bone)->delete_(1u);  // 0x005ab400 scalar deleting dtor
            jo::view<void*>(jo::self<jo::Light>(this)->bone_) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Light>(this)->children) != nullptr) joan::orig::as_children(jo::view<void*>(jo::self<jo::Light>(this)->children))->remove_child(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x004b0370
    }

    // 0x005c63c0 Light::operator= (bone +4 created / deleted / copied).
    H* light_assign(const std::uint8_t* o) {
        c_light_assign.hit();
        jo::self<jo::Light>(this)->type = jo::as_light(o)->type;
        if (jo::view<void*>(jo::as_light(o)->bone_) == nullptr) {
            void* const bone = jo::view<void*>(jo::self<jo::Light>(this)->bone_);
            if (bone != nullptr) joan::orig::as_bone(bone)->delete_(1u);  // 0x005ab400
            jo::view<void*>(jo::self<jo::Light>(this)->bone_) = nullptr;
        } else {
            if (jo::view<void*>(jo::self<jo::Light>(this)->bone_) == nullptr) {
                void* b = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Bone)));  // 0xa0 on x86; operator new
                if (b != nullptr) b = joan::orig::arg::as<void*>(joan::orig::as_bone(b)->Bone_60eb90());  // 0x0060eb90 Bone::Bone
                jo::view<void*>(jo::self<jo::Light>(this)->bone_) = b;
            } else if (jo::self<jo::Light>(this)->bone_->parent != nullptr) {
                joan::orig::as_bone(jo::view<void*>(jo::self<jo::Light>(this)->bone_))->set_parent(joan::orig::arg::mptr(static_cast<void*>(nullptr)));  // 0x0060ec70 set_parent
            }
            std::memcpy(jo::view<void*>(jo::self<jo::Light>(this)->bone_), joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_light(o)->bone_))->matrix()), 64);  // 0x0060ef80
            joan::orig::as_bone(jo::view<void*>(jo::self<jo::Light>(this)->bone_))->invalidate();                                         // 0x0060ee10
        }
        std::memmove(jo::self<jo::Light>(this)->position, jo::as_light(o)->position, offsetof(jo::Light, dirty) - offsetof(jo::Light, position));
        joan::orig::as_light(this)->invalidate();  // 0x005c5c00
        return this;
    }
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
#define HELPER17(name, addr, fn) JOAN_REPLACE("en3d/helpers17", name, addr, fn)
HELPER16("Light::equals", 0x005c6560, &H::light_equals);
HELPER16("Light::release", 0x005c5b50, &H::light_release);
HELPER17("lights_ambient", 0x005c65e0, &lights_ambient);
HELPER17("Light::assign", 0x005c63c0, &H::light_assign);
#undef HELPER16
#undef HELPER17

}  // namespace en3d_helpers16
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint32_t light_c6() { return jo::view<std::uint32_t>(jo::self<jo::Light>(this)->enabled); }   // 0x005c6710
    std::uint32_t light_casts() { return jo::self<jo::Light>(this)->casts_shadows; }  // 0x005c6770
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Light::c6", 0x005c6710, &H::light_c6);
HELPER("Light::casts", 0x005c6770, &H::light_casts);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
namespace {
namespace game_b103_11 {

struct H {
    // 0x005c6740
    H* set_54(u32 v) {
        jo::self<jo::Light>(this)->casts_shadows = v;
        return this;
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Obj::set_54", 0x005c6740, &H::set_54);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

struct H {
    // 0x005c6790
    std::uint8_t flag59() { return jo::self<jo::Light>(this)->flag_59_; }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Obj::flag59", 0x005c6790, &H::flag59);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ShadowModel::dtor() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b83_03::H::shadow_model_dtor>(this)); }  // 0x5c5870
auto joan::orig::Light::release() -> void { joan::bind::member<&::en3d_helpers16::H::light_release>(this); }  // 0x5c5b50
auto joan::orig::Light::assign(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::light_assign>(this, a0)); }  // 0x5c63c0
auto joan::orig::Light::equals(void* a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::en3d_helpers16::H::light_equals>(this, a0)); }  // 0x5c6560
auto joan::orig::lights_ambient() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::en3d_helpers16::lights_ambient>()); }  // 0x5c65e0
auto joan::orig::Light::c6() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::light_c6>(this)); }  // 0x5c6710
auto joan::orig::Light::casts() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::light_casts>(this)); }  // 0x5c6770
auto joan::orig::Light::flag_59() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b94_06::H::flag59>(this)); }  // 0x5c6790
