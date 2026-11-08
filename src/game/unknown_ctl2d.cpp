// game/unknown_ctl2d.cpp: functions whose original source file is unknown (formerly game/_unplaced/ctl2d.cpp;
// re/attribution.csv: between ww/en2d/font2d.cpp and ww/en2d/icon3d.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

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

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

using u16 = std::uint16_t;

struct H {
    // ------------------------------------------------------------------ 0x00628a30
    H* ctor_628a30() {
        jo::view<std::uintptr_t>(jo::self<jo::Control2D>(this)->vtable) = img::addr(d::vt_crt_9da6f8);
        jo::self<jo::Control2D>(this)->u8_4 = 0;
        jo::view<u16>(jo::self<jo::Control2D>(this)->rect[0]) = 0xffff;
        jo::view<u16>(jo::self<jo::Control2D>(this)->rect[1]) = 0xffff;
        jo::view<u16>(jo::self<jo::Control2D>(this)->rect[2]) = 0xffff;
        jo::view<u16>(jo::self<jo::Control2D>(this)->rect[3]) = 0xffff;
        jo::view<u16>(jo::self<jo::Control2D>(this)->i16_d) = 0;
        jo::view<u16>(jo::self<jo::Control2D>(this)->i16_f) = 0;
        jo::self<jo::Control2D>(this)->flags[0] = 0;
        jo::self<jo::Control2D>(this)->flags[1] = 0;
        jo::self<jo::Control2D>(this)->flags[2] = 0;
        jo::self<jo::Control2D>(this)->flags[3] = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Obj::ctor_628a30", 0x00628a30, &H::ctor_628a30);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

struct H {
    // 0x00628ad0 / 0x0062c720
    H* base_dtor_628ad0() {
        jo::view<std::uintptr_t>(jo::self<jo::Control2D>(this)->vtable) = img::addr(d::vt_crt_9da6f8);
        return this;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("dtor_body_628ad0", 0x00628ad0, &H::base_dtor_628ad0);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

struct H {
    // 0x00628b00 Gadget::set_rect(x, y, w, h, b18, b1c, b20)
    std::int32_t set_rect(std::int16_t x, std::int16_t y, std::int16_t w, std::int16_t h, std::uint8_t b0, std::uint8_t b1,
                          std::uint8_t b2) {
        jo::self<jo::Ctl>(this)->x = x;
        jo::self<jo::Ctl>(this)->y = y;
        jo::self<jo::Ctl>(this)->right = static_cast<std::int16_t>(static_cast<std::int32_t>(x) + static_cast<std::int32_t>(w) - 1);
        jo::self<jo::Ctl>(this)->bottom = static_cast<std::int16_t>(static_cast<std::int32_t>(y) + static_cast<std::int32_t>(h) - 1);
        jo::self<jo::Ctl>(this)->width = w;
        jo::self<jo::Ctl>(this)->height = h;
        jo::view<std::uint8_t>(jo::self<jo::Ctl>(this)->fit_to_screen) = b0;
        jo::view<std::uint8_t>(jo::self<jo::Ctl>(this)->hit_enabled) = b1;
        jo::view<std::uint8_t>(jo::self<jo::Ctl>(this)->activatable) = b2;
        jo::view<std::uint8_t>(jo::self<jo::Ctl>(this)->activated) = 0;
        if (virtual_call<std::int32_t>(this, 0) != 0) {
            jo::view<std::uint8_t>(jo::self<jo::Ctl>(this)->i8_4) = 1;
            return 1;
        }
        return 0;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Gadget::set_rect", 0x00628b00, &H::set_rect);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
namespace {
namespace game_b93_03 {

struct H {
    i32 widget_poll() {  // 0x00628bd0
        const std::uintptr_t slot = reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(this)[1]);
        if (thiscall<i32>(slot, this) != 0) {
            jo::view<u8>(jo::self<jo::Ctl>(this)->i8_4) = 0;
            return 1;
        }
        return 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Widget::poll", 0x00628bd0, &H::widget_poll);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

using i16 = std::int16_t;

using joan::img::obj;

// A word local living in its own dword stack slot (only the low word is meaningful).
struct WordSlot {
    i16 v;
    i16 pad;
};
static_assert(sizeof(WordSlot) == 4, "WordSlot");

void* const kScreen = obj(d::g_sprites_a46128);
void* const kMouse = obj(d::g_mouse);

JOAN_COUNTER(c_hit, "Widget::hit_628c10");

struct H {
    // ------------------------------------------------------------------ 0x00628c10
    i32 hit_628c10(i8 c) {
        c_hit.hit();
        if (jo::self<jo::Ctl>(this)->i8_4 == 0) return 0;
        if (jo::self<jo::Ctl>(this)->activatable == 0 || jo::self<jo::Ctl>(this)->activated == 0) return 0;
        if (jo::self<jo::Ctl>(this)->hit_enabled == 0) return 0;
        WordSlot s8{jo::self<jo::Ctl>(this)->x, 0};
        WordSlot sc{jo::self<jo::Ctl>(this)->y, 0};
        WordSlot s10{jo::self<jo::Ctl>(this)->right, 0};
        WordSlot s14{jo::self<jo::Ctl>(this)->bottom, 0};
        float f18 = 1.0f;
        float f1c = 1.0f;
        if (jo::self<jo::Ctl>(this)->fit_to_screen != 0) {
            joan::orig::as_sprite2d(kScreen)->fit_rect(joan::orig::arg::i16(jo::self<jo::Ctl>(this)->x), joan::orig::arg::i16(jo::self<jo::Ctl>(this)->y), joan::orig::arg::i16(jo::self<jo::Ctl>(this)->right), joan::orig::arg::i16(jo::self<jo::Ctl>(this)->bottom), &f18, &f1c, &s8, &sc, &s10, &s14, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
        }
        i32 inside = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_parser(kMouse)->rect_value(static_cast<i32>(s8.v), static_cast<i32>(sc.v), static_cast<i32>(s10.v), static_cast<i32>(s14.v), 0)) != 0 ||  // 0x005945d0
            joan::orig::arg::as<i32>(joan::orig::as_parser(kMouse)->rect_mode2(static_cast<i32>(s8.v), static_cast<i32>(sc.v), static_cast<i32>(s10.v), static_cast<i32>(s14.v), 0)) != 0) {  // 0x00594900
            inside = 1;
        }
        const i32 r = virtual_call<i32>(this, 8, c);
        return (inside == 0 && r == 0) ? 0 : 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Widget::hit_628c10", 0x00628c10, &H::hit_628c10);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

struct H {
    // ------------------------------------------------------------------ 0x00628df0
    i32 activate(u32 c) {
        if (jo::self<jo::Ctl>(this)->i8_4 == 0) return 0;
        if (jo::self<jo::Ctl>(this)->activatable == 0) {
            jo::view<u8>(jo::self<jo::Ctl>(this)->activated) = 0;
            return 0;
        }
        if (virtual_call<i32>(this, 0xc, static_cast<u8>(c)) != 0) {
            jo::view<u8>(jo::self<jo::Ctl>(this)->activated) = 1;
            return 1;
        }
        return 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Ctl::activate", 0x00628df0, &H::activate);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b83_05.cpp (game/b83_05)
namespace {
namespace game_b83_05 {

struct H {
    // 0x00629230. ret 4 (eax of 0x005cf920 kept).
    std::uintptr_t overlay_attach(void* actor) {
        jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor) = actor;
        jo::self<jo::Overlay2D>(this)->actor->plane->materials->blend = 3;
        jo::self<jo::Overlay2D>(this)->actor->plane->materials->texture_mode = 0x10022;
        jo::self<jo::Overlay2D>(this)->actor->plane->materials->pass_mask = 2;
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor->plane))->invalidate(1u));  // 0x005cf920
    }

    // 0x00628e70. ret 0.
    void* overlay_ctor() {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::Overlay2D));  // CRT memset (the actor pointer; 4 on x86)
        return this;
    }

    // 0x00628ea0. ret 0 (eax of 0x006292c0 kept).
    std::uintptr_t overlay_release() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_overlay2d(this)->free_actor()); }  // 0x006292c0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_05", name, addr, fn)
B("Overlay2D::attach", 0x00629230, &H::overlay_attach);
B("Overlay2D::Overlay2D", 0x00628e70, &H::overlay_ctor);
B("Overlay2D::release", 0x00628ea0, &H::overlay_release);
#undef B

}  // namespace game_b83_05
}  // namespace

// ---------------------------------------- from modules/game_b82_11.cpp (game/b82_11)
namespace {
namespace game_b82_11 {

JOAN_COUNTER(c_release, "0x006292c0 release");

struct H {
    // ---------------------------------------------------------------- 0x006292c0
    void* release() {
        if (jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor) == nullptr) return this;
        c_release.hit();
        if (jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor->anim) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor->anim);
            joan::orig::as_anim(p)->scalar_delete(1u);  // 0x00417250
        }
        void* const p2 = jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor->def);
        if (p2 != nullptr) joan::orig::as_actor_def(p2)->destroy(1u);  // 0x004b0c20
        void* const p3 = jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor);
        void* r = p3;  // eax: the pointer (null) when the call is skipped
        if (p3 != nullptr) r = joan::orig::arg::as<void*>(joan::orig::as_scene_object(p3)->destroy(1u));  // 0x00420840
        jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor) = nullptr;
        return r;
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b82_11", name, addr, fn)
B11("0x006292c0 release", 0x006292c0, &H::release);
#undef B11

}  // namespace game_b82_11
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

struct H {
    // ------------------------------------------------------------------ 0x006293b0
    u32 forward_5abe30(u32 a, u32 b, u32 c, u32 d, u32 e) {
        void* const o = jo::view<void*>(jo::self<jo::Overlay2D>(this)->actor);
        return thiscall<u32>(fn::Actor_start_anim, o, jo::as_actor(o)->anim, a, b, c, d, e);  // 0x005abe30
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Obj::forward_5abe30", 0x006293b0, &H::forward_5abe30);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Ctl2d::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_11::H::ctor_628a30>(this)); }  // 0x628a30
auto joan::orig::Ctl2d::destruct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_03::H::base_dtor_628ad0>(this)); }  // 0x628ad0
auto joan::orig::Gadget::set_rect(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::uint8_t a4, std::uint8_t a5, std::uint8_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_06::H::set_rect>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x628b00
auto joan::orig::Widget::poll() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_03::H::widget_poll>(this)); }  // 0x628bd0
auto joan::orig::Widget::hit_628c10(std::int8_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_06::H::hit_628c10>(this, a0)); }  // 0x628c10
auto joan::orig::Ctl2d::activate(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_06::H::activate>(this, a0)); }  // 0x628df0
auto joan::orig::Overlay2D::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_05::H::overlay_ctor>(this)); }  // 0x628e70
auto joan::orig::Overlay2D::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b83_05::H::overlay_release>(this)); }  // 0x628ea0
auto joan::orig::Overlay2D::attach(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b83_05::H::overlay_attach>(this, a0)); }  // 0x629230
auto joan::orig::Overlay2D::free_actor() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_11::H::release>(this)); }  // 0x6292c0
