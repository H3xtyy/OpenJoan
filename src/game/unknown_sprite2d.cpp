// game/unknown_sprite2d.cpp: functions whose original source file is unknown (formerly game/_unplaced/sprite2d.cpp;
// re/attribution.csv: between ww/en2d/txtbox2d.cpp and ww/en2d/widget2d.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstddef>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../canvas_api.hpp"
#include "../registry.hpp"
#include "stream_cache_api.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::call::thiscall;

using i32 = std::int32_t;
using u16 = std::uint16_t;

using joan::hook::global;
using u8 = std::uint8_t;

using joan::call::cdecl_call;
using joan::img::obj;

using i16 = std::int16_t;

using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

struct H {
    // ------------------------------------------------------------------------ 0x006317a0
    void* ctor_6317a0() {
        joan::orig::as_camera(this)->construct();  // 0x00613650
        jo::self<jo::Sprite2D>(this)->scene = nullptr;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Obj631::ctor", 0x006317a0, &H::ctor_6317a0);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b106_05.cpp (game/b106_05)
namespace {
namespace game_b106_05 {

struct H {
    // ------------------------------------------------------------------------ 0x006317e0
    u32 dtor_6317e0() {
        joan::orig::as_screen(this)->close_631c60();         // 0x00631c60
        return joan::orig::arg::as<u32>(joan::orig::as_camera(this)->destruct());   // 0x00613700
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_05", name, addr, fn)
B("Panel::dtor_6317e0", 0x006317e0, &H::dtor_6317e0);
#undef B

}  // namespace game_b106_05
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

JOAN_COUNTER(c_camera_build, "Camera::build_631850");

struct H {
    // ------------------------------------------------------------------ 0x00631850
    u32 camera_build() {
        c_camera_build.hit();
        joan::orig::as_sys_en(this)->init_613730();  // 0x00613730
        alignas(4) u8 view[0x28];
        joan::orig::as_camera_params(view)->construct();  // 0x00616d50
        jo::view<u32>(jo::as_cam_params(view)->projection) = 1;
        jo::view<u32>(jo::as_cam_params(view)->orbit[0]) = 0x3fc90fdbu;
        jo::view<u32>(jo::as_cam_params(view)->orbit[1]) = 0xbfc90fdbu;
        jo::as_cam_params(view)->orbit[3] = jo::self<jo::Sprite2D>(this)->near_plane + 2.0f;
        jo::as_cam_params(view)->aspect = 1.0f;
        joan::orig::as_camera(this)->set_frame(joan::orig::arg::mptr(static_cast<void*>(view)));  // 0x00614040
        joan::orig::as_camera(this)->set_viewport(static_cast<jo::Surface*>(obj(d::g_vga_back)), obj(d::g_a56ef0), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(// 0x00613bf0
                      global<i32>(d::g_a5243c) - 1), joan::orig::arg::u32(global<i32>(d::g_a52440) - 1));
        joan::orig::as_camera(this)->set_cf(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(global<u32>(d::g_init_black)));  // 0x00613d30
        joan::orig::as_camera(this)->dirty();                                           // 0x00614170
        if (joan::orig::arg::as<i32>(joan::orig::as_sprite_queue(this)->build_hook()) == 0) return 0;                        // 0x00631d10
        if (jo::self<jo::Sprite2D>(this)->scene == nullptr) {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Scene)));  // 0xd5 on x86
            void* const o = mem != nullptr ? joan::orig::as_scene(mem)->construct() : nullptr;  // 0x005e9a00
            jo::self<jo::Sprite2D>(this)->scene = static_cast<jo::Scene*>(o);
            joan::orig::as_scene(jo::view<void*>(jo::self<jo::Sprite2D>(this)->scene))->reset();                        // 0x005e9ac0
            joan::orig::as_camera(this)->set_scene(jo::self<jo::Sprite2D>(this)->scene);                  // 0x00613db0
        }
        const i32 sw = global<i32>(d::g_screen_w);
        const i32 sh = global<i32>(d::g_screen_h);
        float rect[6];
        rect[0] = static_cast<float>(static_cast<i32>(0u - static_cast<u32>(sw)));
        rect[1] = static_cast<float>(static_cast<i32>(0u - static_cast<u32>(sh)));
        rect[2] = 0.0f;
        rect[3] = static_cast<float>(sw);
        rect[4] = static_cast<float>(sh);
        rect[5] = jo::self<jo::Sprite2D>(this)->far_ortho;
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::Sprite2D>(this)->scene))->setup(joan::orig::arg::mptr(static_cast<void*>(rect)));  // 0x005e9e50
        const float step = (jo::self<jo::Sprite2D>(this)->far_ortho - jo::self<jo::Sprite2D>(this)->near_plane) / 10.0f;
        float lo = 0.0f;
        float hi = step;
        for (i32 i = 0; i < 10; ++i) {
            void* const layer = joan::orig::arg::as<void*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Sprite2D>(this)->scene))->add_node());  // 0x005ea460
            const i32 w0 = global<i32>(d::g_screen_w);
            const i32 h0 = global<i32>(d::g_screen_h);
            float r[6];
            r[0] = static_cast<float>(static_cast<i32>(0u - static_cast<u32>(w0)));
            r[1] = static_cast<float>(static_cast<i32>(0u - static_cast<u32>(h0)));
            r[2] = lo;
            r[3] = static_cast<float>(global<i32>(d::g_screen_w));
            r[4] = static_cast<float>(global<i32>(d::g_screen_h));
            r[5] = hi;
            joan::orig::as_scene_source(layer)->reset(joan::orig::arg::mptr(static_cast<void*>(r)));  // 0x005ef880
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
            void* const item = mem != nullptr ? joan::orig::as_plane(mem)->construct(joan::orig::arg::u32(i32{4}), joan::orig::arg::u32(i32{1})) : nullptr;  // 0x005cbaf0
            joan::orig::as_plane(item)->init(i32{0}, i32{0}, i32{0}, joan::orig::arg::u32(i32{1}));  // 0x005cc7a0
            jo::view<u32>(jo::as_plane(item)->list_kind) = 7;
            joan::orig::as_scene_source(layer)->set_38(joan::orig::arg::mptr(item));  // 0x005efbe0
            lo = lo + step;
            hi = hi + step;
        }
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::Sprite2D>(this)->scene))->build_tree();  // 0x005ea730
        jo::view<u16>(jo::self<jo::Sprite2D>(this)->count) = 0;
        jo::self<jo::Sprite2D>(this)->u16_4d0 = 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("Camera::build_631850", 0x00631850, &H::camera_build);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b105_17.cpp (game/b105_17)
namespace {
namespace game_b105_17 {

JOAN_COUNTER(c_close_631c60, "Screen::close_631c60");

struct H {
    // 0x00631c60
    u32 close_631c60() {
        c_close_631c60.hit();
        joan::orig::as_sprite_queue(this)->close_hook();  // 0x00631d30 (result unused)
        joan::orig::as_model(this)->release_parts();  // 0x00613a10
        if (jo::view<void*>(jo::self<jo::Sprite2D>(this)->scene) != nullptr) {
            void* p = jo::view<void*>(jo::self<jo::Sprite2D>(this)->scene);
            if (p != nullptr) joan::orig::as_scene(p)->destroy(1u);  // 0x004201f0 scalar deleting dtor
            jo::view<void*>(jo::self<jo::Sprite2D>(this)->scene) = nullptr;
        }
        return 1;
    }
};

JOAN_REPLACE("game/b105_17", "Screen::close_631c60", 0x00631c60, &H::close_631c60);

}  // namespace game_b105_17
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
namespace {
namespace game_b104_11 {

struct alignas(4) Material {  // 0x54 on x86 (0x005d52a0 memsets all of it); the typed Material is larger on x64
    u8 bytes[((sizeof(jo::Material) > 0x54 ? sizeof(jo::Material) : 0x54) + 3u) & ~3u];
};

// x87 compare "equal or unordered" (fcomp + test ah,0x40 jnz)
bool eq_or_nan(float a, float b) { return !(a < b) && !(a > b); }

JOAN_COUNTER(c_triangle, "Overlay::triangle_6324b0");

struct H {
    u32 true1() { return 1; }

    // ------------------------------------------------------------------ 0x006324b0
    u32 triangle(u32 x1, u32 y1, u32 x2, u32 y2, u32 x3, u32 y3, u32 group, u32 c1, u32 c2, u32 c3) {
        c_triangle.hit();
        const joan::hook::x87::Fpu fpu{true};
        auto centred = [&](u32 v, img::Addr size) {
            const double half = fpu.div(static_cast<double>(global<i32>(size)), 2.0);  // 0x9cf794
            return static_cast<float>(fpu.sub(static_cast<double>(static_cast<i16>(v)), half));
        };
        float xs[3], ys[3];
        xs[0] = centred(x1, d::g_screen_w);
        xs[1] = centred(x2, d::g_screen_w);
        xs[2] = centred(x3, d::g_screen_w);
        ys[0] = centred(y1, d::g_screen_h);
        ys[1] = centred(y2, d::g_screen_h);
        ys[2] = centred(y3, d::g_screen_h);
        auto lo = [](float a, float b) { return joan::orig::arg::as<float>(joan::orig::fmin(joan::orig::arg::f32(a), joan::orig::arg::f32(b))); };  // 0x00634ae0
        auto hi = [](float a, float b) { return joan::orig::arg::as<float>(joan::orig::fmax(joan::orig::arg::f32(a), joan::orig::arg::f32(b))); };  // 0x00634aa0
        const float min_x = lo(lo(xs[0], xs[1]), xs[2]);
        const float max_x = hi(hi(xs[0], xs[1]), xs[2]);
        const float min_y = lo(lo(ys[0], ys[1]), ys[2]);
        const float max_y = hi(hi(ys[0], ys[1]), ys[2]);
        for (i32 i = 0; i < 3; ++i) {
            if (eq_or_nan(xs[i], min_x)) xs[i] = xs[i] - 0.5f;  // 0x9cf77c
            if (eq_or_nan(xs[i], max_x)) xs[i] = xs[i] + 0.5f;
            if (eq_or_nan(ys[i], min_y)) ys[i] = ys[i] - 0.5f;
            if (eq_or_nan(ys[i], max_y)) ys[i] = ys[i] + 0.5f;
        }
        void* const grp = joan::orig::arg::as<void*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Sprite2D>(this)->scene))->group(joan::orig::arg::i32(group)));  // 0x005ea6e0
        void* const mesh = jo::view<void*>(jo::as_scene_s(grp)->plane);
        if (jo::as_plane(mesh)->vertex_count + 3 > 0x1000 || jo::as_plane(mesh)->triangle_count + 1 > 0x1555) return 0;
        Material mat;
        joan::orig::as_material(&mat)->construct();  // 0x005d52a0
        jo::as_material(&mat)->pass_mask = 2;
        jo::as_material(&mat)->blend = 3;
        jo::as_material(&mat)->texture_mode = 0x10000;
        jo::as_material(&mat)->filter = 1;
        i32 m = jo::as_plane(mesh)->material_count - 1;
        bool same = false;
        if (m >= 0) {
            same = (joan::orig::arg::as<u32>(joan::orig::as_material(&mat)->equals(joan::orig::arg::mptr(&jo::as_plane(mesh)->materials[m]))) & 0xffu) != 0;  // 0x005d5500
        }
        if (!same) {
            m = joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->add_materials(1));  // 0x005cd470
            joan::orig::as_material(&jo::as_plane(mesh)->materials[m])->assign(&mat);  // 0x005d53e0
        }
        joan::orig::as_material(&mat)->dtor();  // 0x005d5360
        const u32 uv[6] = {0u, 0u, 0x3f800000u, 0u, 0x3f800000u, 0x3f800000u};
        u32 cols[3] = {c1, c2, c3};
        float pos[9] = {xs[0], ys[0], 0.0f, xs[1], ys[1], 0.0f, xs[2], ys[2], 0.0f};
        const i32 v = joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->add_vertices(3));  // 0x005cd7f0
        jo::VertexF4* vp = &jo::as_plane(mesh)->vertices_f4[v];
        for (i32 i = 0; i < 3; ++i, ++vp) {
            std::memcpy(vp, &pos[i * 3], 12);
            joan::orig::as_colour(vp->attr)->from_rgba(&cols[i]);  // 0x005e95b0
            jo::view<u32>(jo::as_vertex_f4(vp)->attr[1]) = uv[i * 2];
            jo::view<u32>(jo::as_vertex_f4(vp)->attr[2]) = uv[i * 2 + 1];
        }
        const i32 t = joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->add_triangles(1));  // 0x005ce6f0
        joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(t), joan::orig::arg::u32(m), joan::orig::arg::u32(v), joan::orig::arg::u32(v + 2), joan::orig::arg::u32(v + 1));  // 0x005ce800
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("Obj::true_631d10", 0x00631d10, &H::true1);
B("Obj::true_631d30", 0x00631d30, &H::true1);
B("Overlay::triangle_6324b0", 0x006324b0, &H::triangle);
#undef B

}  // namespace game_b104_11
}  // namespace

// ---------------------------------------- from modules/game_ui7.cpp (game/ui7)
namespace {
namespace game_ui7 {

constexpr img::Addr kSprites = d::g_sprites_a46128;   // 2D sprite queue

JOAN_COUNTER(c_blit_image, "SpriteQueue::blit_image");
JOAN_COUNTER(c_release, "0x00631dc0 release list");

// 0x00632a70 sprite queue (16 args, ret 0x40): x1, y1, x2, y2 (words), layer, colour,
// texture, mode (word), 3 words, sx1, sy1, sx2, sy2, rot (words).
std::int32_t queue(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::int32_t layer, std::uint32_t colour,
                   void* texture, std::int32_t mode, std::int32_t a, std::int32_t b, std::int32_t c, std::int32_t sx1,
                   std::int32_t sy1, std::int32_t sx2, std::int32_t sy2, std::int32_t rot) {
    return joan::orig::arg::as<std::int32_t>(joan::orig::as_sprite2d(obj(kSprites))->queue(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(layer), joan::orig::arg::u32(colour), joan::orig::arg::mptr(texture), joan::orig::arg::i32(mode), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(sx1), joan::orig::arg::i32(sy1), joan::orig::arg::i32(sx2), joan::orig::arg::i32(sy2), joan::orig::arg::i32(rot)));  // 0x00632a70
}

struct H {
    // 0x006332b0 SpriteQueue::blit_image. ret 0x3c.
    std::int32_t blit_image(std::int16_t x, std::int16_t y, std::uint32_t layer, void* texture, std::uint32_t colour, std::int16_t a,
                            std::int16_t b, std::int16_t c, std::int16_t sx1, std::int16_t sy1, std::int16_t sx2, std::int16_t sy2,
                            std::int16_t rot, std::int16_t x2, std::int16_t y2) {
        c_blit_image.hit();
        if (texture == nullptr || joan::orig::arg::as<std::int32_t>(joan::orig::as_texture(texture)->ready()) == 0) return 0;  // 0x00605b70
        const std::int16_t l = static_cast<std::int32_t>(sx1) == -1 ? 0 : sx1;
        const std::int16_t t = static_cast<std::int32_t>(sy1) == -1 ? 0 : sy1;
        const std::int16_t r = static_cast<std::int16_t>(static_cast<std::int32_t>(sx2) == -1 ? jo::view<std::int32_t>(jo::as_texture(texture)->width) - 1 : static_cast<std::int32_t>(sx2));
        const std::int16_t bo = static_cast<std::int16_t>(static_cast<std::int32_t>(sy2) == -1 ? jo::view<std::int32_t>(jo::as_texture(texture)->height) - 1 : static_cast<std::int32_t>(sy2));
        const bool even = static_cast<std::int32_t>(rot) % 2 == 0;
        const std::int16_t w = static_cast<std::int16_t>(even ? static_cast<std::int32_t>(r) - l + 1 : static_cast<std::int32_t>(bo) - t + 1);
        const std::int16_t h = static_cast<std::int16_t>(even ? static_cast<std::int32_t>(bo) - t + 1 : static_cast<std::int32_t>(r) - l + 1);
        const std::int32_t ey = static_cast<std::int32_t>(y2) == -1 ? static_cast<std::int32_t>(y) + h - 1 : static_cast<std::int32_t>(y2);
        const std::int32_t ex = static_cast<std::int32_t>(x2) == -1 ? static_cast<std::int32_t>(x) + w - 1 : static_cast<std::int32_t>(x2);
        return queue(x, y, static_cast<std::int16_t>(ex), static_cast<std::int16_t>(ey), static_cast<std::int32_t>(layer), colour, texture, 3, a, b,
                     c, sx1, sy1, sx2, sy2, rot);
    }

    // 0x00631d50. ret 0.
    std::uint32_t forward_614630() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_camera(this)->begin_frame()); }  // 0x00614630

    // 0x00631d80. ret 0.
    std::int32_t reset_631d80() {
        joan::orig::as_camera(this)->render();  // 0x00614760
        joan::orig::as_sprite_queue(this)->release_list();  // 0x00631dc0
        return 1;
    }

    // 0x00631dc0. ret 0.
    std::int32_t release_list() {
        c_release.hit();
        if (jo::self<jo::Sprite2D>(this)->scene != nullptr && joan::orig::arg::as<void*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Sprite2D>(this)->scene))->group(1)) != nullptr) {  // 0x005ea6e0
            for (std::int32_t i = 1; i <= 0xa; ++i) {
                std::uint8_t* const p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Sprite2D>(this)->scene))->group(joan::orig::arg::i32(i)));  // 0x005ea6e0
                if (p == nullptr) continue;
                void* const q = jo::view<void*>(jo::as_scene_s(p)->plane);
                if (q != nullptr) joan::orig::as_plane(q)->clear();  // 0x005cd2d0
                joan::orig::as_scene_s(p)->release_actors();                     // 0x005eff10
            }
        }
        jo::self<jo::Sprite2D>(this)->count = 0;
        return 1;
    }
};

#define U7(name, addr, fn) JOAN_REPLACE("game/ui7", name, addr, fn)
U7("SpriteQueue::blit_image", 0x006332b0, &H::blit_image);
U7("0x00631d50 forward", 0x00631d50, &H::forward_614630);
U7("0x00631d80 reset", 0x00631d80, &H::reset_631d80);
U7("0x00631dc0 release list", 0x00631dc0, &H::release_list);
#undef U7

}  // namespace game_ui7
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
namespace {
namespace game_b84_04 {

void* const kFiles = obj(d::g_misc);

// 0x00590a20 round(double) (thiscall on the file object)
std::int32_t round_d(double v) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(kFiles)->round(joan::orig::arg::f64(v))); }

JOAN_COUNTER(c_add_point, "Sprite2D::add_point");
JOAN_COUNTER(c_scaled_rect, "Sprite2D::scaled_rect");

// 0x00634aa0 max (a < b or NaN -> b)
float JOAN_CDECL fmax_(float a, float b) { return (a < b || a != a || b != b) ? b : a; }
// 0x00634ae0 min (a < b or NaN -> a)
float JOAN_CDECL fmin_(float a, float b) { return (a < b || a != a || b != b) ? a : b; }

struct H {
    // ---- 2D helpers --------------------------------------------------------------------
    // 0x00631e90 adds one screen point (centred on the viewport) to the mesh of a sprite
    // layer: 0 when the mesh is full.
    std::int32_t add_point(std::int16_t sx, std::int16_t sy, std::int32_t layer, std::uint32_t colour) {
        c_add_point.hit();
        const joan::hook::x87::Fpu fpu{true};
        const float x = static_cast<float>(
            fpu.sub(static_cast<double>(sx), fpu.div(static_cast<double>(global<std::int32_t>(d::g_screen_w)), 2.0)));  // 0x009cf794
        const float y = static_cast<float>(
            fpu.sub(static_cast<double>(sy), fpu.div(static_cast<double>(global<std::int32_t>(d::g_screen_h)), 2.0)));
        std::uint8_t* const mesh = jo::view<std::uint8_t*>(jo::as_scene_s(joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Sprite2D>(this)->scene))->group(joan::orig::arg::i32(layer))))->plane);  // 0x005ea6e0
        if (jo::as_plane(mesh)->vertex_count + 1 > 0x1000 || jo::as_plane(mesh)->triangle_count + 1 > 0x1555) return 0;
        alignas(alignof(void*)) std::uint8_t mat[((sizeof(jo::Material) > 0x54 ? sizeof(jo::Material) : 0x54) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger Material
        joan::orig::as_material(mat)->construct();  // 0x005d52a0 Material::Material
        jo::as_material(mat)->pass_mask = 2;
        jo::as_material(mat)->blend = 3;
        jo::as_material(mat)->texture_mode = 0x10000;
        jo::as_material(mat)->filter = 1;
        std::int32_t mi = jo::as_plane(mesh)->material_count - 1;
        bool same = false;
        if (mi >= 0)
            same = (joan::orig::arg::as<std::uint32_t>(joan::orig::as_material(mat)->equals(joan::orig::arg::mptr(&jo::as_plane(mesh)->materials[mi]))) & 0xffu) != 0;  // 0x005d5500 equals
        if (!same) {
            mi = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->add_materials(1));                                  // 0x005cd470
            joan::orig::as_material(&jo::as_plane(mesh)->materials[mi])->assign(joan::orig::arg::mptr(static_cast<void*>(mat)));  // 0x005d53e0
        }
        joan::orig::as_material(mat)->dtor();  // 0x005d5360
        const std::int32_t vi = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->add_vertices(1));  // 0x005cd7f0
        {
            jo::VertexF4* const v = &jo::as_plane(mesh)->vertices_f4[vi];
            jo::as_vertex_f4(v)->pos[0] = x;
            jo::as_vertex_f4(v)->pos[1] = y;
            jo::view<std::uint32_t>(jo::as_vertex_f4(v)->pos[2]) = 0;
        }
        joan::orig::as_colour(jo::as_plane(mesh)->vertices_f4[vi].attr)->from_rgba(joan::orig::arg::mptr(static_cast<void*>(&colour)));  // 0x005e95b0
        {
            jo::VertexF4* const v = &jo::as_plane(mesh)->vertices_f4[vi];
            jo::view<std::uint32_t>(jo::as_vertex_f4(v)->attr[1]) = 0;
            jo::view<std::uint32_t>(jo::as_vertex_f4(v)->attr[2]) = 0;
        }
        const std::int32_t ti = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->add_triangles(1));  // 0x005ce6f0
        joan::orig::as_plane(mesh)->set3(joan::orig::arg::u32(ti), joan::orig::arg::u32(mi), joan::orig::arg::u32(vi));                              // 0x005ce900
        return 1;
    }

    // 0x00633ce0 scales the rectangle (l, t, r, b) by (sx, sy) about the origin, writing the
    // rounded corners.
    std::int32_t scaled_rect(std::int16_t l, std::int16_t t, std::int16_t r, std::int16_t b, float sx, float sy, std::int16_t* out_l,
                             std::int16_t* out_t, std::int16_t* out_r, std::int16_t* out_b) {
        c_scaled_rect.hit();
        const joan::hook::x87::Fpu fpu{true};
        const double w = static_cast<double>(static_cast<std::int32_t>(r) - static_cast<std::int32_t>(l) + 1);
        const double h = static_cast<double>(static_cast<std::int32_t>(b) - static_cast<std::int32_t>(t) + 1);
        const double cx = fpu.add(static_cast<double>(l), fpu.div(w, 2.0));  // 0x009d0670
        const double cy = fpu.add(static_cast<double>(t), fpu.div(h, 2.0));
        const double sw = fpu.mul(sx, w);
        const double sh = fpu.mul(sy, h);
        const double scx = fpu.mul(sx, cx);
        const double scy = fpu.mul(sy, cy);
        *out_l = static_cast<std::int16_t>(round_d(fpu.sub(scx, fpu.div(sw, 2.0))));
        *out_t = static_cast<std::int16_t>(round_d(fpu.sub(scy, fpu.div(sh, 2.0))));
        const std::int32_t left = *out_l;
        *out_r = static_cast<std::int16_t>(left + static_cast<std::int16_t>(round_d(sw)) - 1);
        const std::int32_t top = *out_t;
        *out_b = static_cast<std::int16_t>(top + static_cast<std::int16_t>(round_d(sh)) - 1);
        return 1;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("Sprite2D::add_point", 0x00631e90, &H::add_point);
M("Sprite2D::scaled_rect", 0x00633ce0, &H::scaled_rect);
M("fmin", 0x00634ae0, &fmin_);
M("fmax", 0x00634aa0, &fmax_);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- from modules/game_b83_05.cpp (game/b83_05)
namespace {
namespace game_b83_05 {

using joan::hook::x87::ftol;

std::uint32_t bits(float v) {
    std::uint32_t b = 0;
    std::memcpy(&b, &v, 4);
    return b;
}
std::int16_t low16(std::int32_t v) { return static_cast<std::int16_t>(static_cast<std::uint16_t>(static_cast<std::uint32_t>(v))); }

constexpr fn::Code kQueueSprite = fn::Sprite2D_queue;  // 0x00632a70 (hooked by canvas_hooks.cpp)
constexpr fn::Code kFitRect = fn::Sprite2D_fit_rect;      // 0x00633e60
constexpr std::uint32_t kOne = 0x3f800000u;          // 1.0f

JOAN_COUNTER(c_fit_tri, "Sprite2D::fit_triangle");
JOAN_COUNTER(c_fit_rect, "Sprite2D::fit_rect");
JOAN_COUNTER(c_line, "Sprite2D::queue_line");
JOAN_COUNTER(c_frame, "Sprite2D::queue_frame");

// One vertex of Sprite2D::fit_triangle: a vertex on the left / right edge of the bounding box
// keeps its relative height on the fitted box, one on the top / bottom edge its relative x.
// (fild ; fcomp qword ; test ah,0x40 ; je next: equal only - the operands are integers.)
void fit_vertex(std::int16_t vx, std::int16_t vy, std::int16_t* ox, std::int16_t* oy, double minx, double miny,
                double maxx, double maxy, std::int16_t a, std::int16_t b, std::int16_t c, std::int16_t d) {
    const Fpu fpu{true};
    const double x = static_cast<double>(static_cast<std::int32_t>(vx));
    const double y = static_cast<double>(static_cast<std::int32_t>(vy));
    auto along_y = [&]() {
        const float t = static_cast<float>(fpu.div(fpu.sub(y, miny), fpu.sub(maxy, miny)));
        const double span = static_cast<double>(static_cast<std::int32_t>(d) - static_cast<std::int32_t>(b));
        return low16(ftol(fpu.add(fpu.mul(span, static_cast<double>(t)), static_cast<double>(static_cast<std::int32_t>(b)))));
    };
    auto along_x = [&]() {
        const float t = static_cast<float>(fpu.div(fpu.sub(x, minx), fpu.sub(maxx, minx)));
        const double span = static_cast<double>(static_cast<std::int32_t>(c) - static_cast<std::int32_t>(a));
        return low16(ftol(fpu.add(fpu.mul(span, static_cast<double>(t)), static_cast<double>(static_cast<std::int32_t>(a)))));
    };
    if (x == minx) {
        *ox = a;
        *oy = along_y();
    } else if (x == maxx) {
        *ox = c;
        *oy = along_y();
    } else if (y == miny) {
        *ox = along_x();
        *oy = b;
    } else if (y == maxy) {
        *ox = along_x();
        *oy = d;
    }
}

struct H {
    // 0x006340e0. ret 0x38.
    std::int32_t fit_triangle(std::int16_t x0, std::int16_t y0, std::int16_t x1, std::int16_t y1, std::int16_t x2,
                              std::int16_t y2, float* sx, float* sy, std::int16_t* ox0, std::int16_t* oy0,
                              std::int16_t* ox1, std::int16_t* oy1, std::int16_t* ox2, std::int16_t* oy2) {
        c_fit_tri.hit();
        *ox0 = x0;
        *oy0 = y0;
        *ox1 = x1;
        *oy1 = y1;
        *ox2 = x2;
        *oy2 = y2;
        jo::view<std::uint32_t>(*sx) = kOne;
        jo::view<std::uint32_t>(*sy) = kOne;
        auto fl = [](std::int32_t v) { return static_cast<float>(v); };  // fild ; fstp dword (exact: shorts / ints of shorts)
        const std::int32_t i0 = joan::orig::arg::as<std::int32_t>(joan::orig::min_int(static_cast<std::int32_t>(x0), static_cast<std::int32_t>(x1)));  // 0x0046f020
        const double minx = joan::orig::arg::as<float>(joan::orig::fmin(joan::orig::arg::f32(fl(i0)), joan::orig::arg::f32(fl(x2))));                                                           // 0x00634ae0
        const std::int32_t i1 = joan::orig::arg::as<std::int32_t>(joan::orig::min_int(static_cast<std::int32_t>(y0), static_cast<std::int32_t>(y1)));
        const double miny = joan::orig::arg::as<float>(joan::orig::fmin(joan::orig::arg::f32(fl(i1)), joan::orig::arg::f32(fl(y2))));
        const std::int32_t i2 = joan::orig::arg::as<std::int32_t>(joan::orig::int_max(static_cast<std::int32_t>(x0), static_cast<std::int32_t>(x1)));  // 0x004893c0
        const double maxx = joan::orig::arg::as<float>(joan::orig::fmax(joan::orig::arg::f32(fl(i2)), joan::orig::arg::f32(fl(x2))));                                                           // 0x00634aa0
        const std::int32_t i3 = joan::orig::arg::as<std::int32_t>(joan::orig::int_max(static_cast<std::int32_t>(y0), static_cast<std::int32_t>(y1)));
        const double maxy = joan::orig::arg::as<float>(joan::orig::fmax(joan::orig::arg::f32(fl(i3)), joan::orig::arg::f32(fl(y2))));
        std::int16_t a = 0, b = 0, c = 0, d = 0;  // -0x28 x1, -0x2c y1, -0x30 x2, -0x34 y2 (all written by 0x00633e60)
        thiscall<std::int32_t>(kFitRect, this, low16(ftol(minx)), low16(ftol(miny)), low16(ftol(maxx)), low16(ftol(maxy)), sx, sy,
                               &a, &b, &c, &d, static_cast<std::int16_t>(0x280), static_cast<std::int16_t>(0x1e0));
        fit_vertex(x0, y0, ox0, oy0, minx, miny, maxx, maxy, a, b, c, d);
        fit_vertex(x1, y1, ox1, oy1, minx, miny, maxx, maxy, a, b, c, d);
        fit_vertex(x2, y2, ox2, oy2, minx, miny, maxx, maxy, a, b, c, d);
        return 1;
    }

    // 0x00633e60. ret 0x30.
    std::int32_t fit_rect(std::int16_t x1, std::int16_t y1, std::int16_t x2, std::int16_t y2, float* sx, float* sy,
                          std::int16_t* ox1, std::int16_t* oy1, std::int16_t* ox2, std::int16_t* oy2, std::int16_t w,
                          std::int16_t h) {
        c_fit_rect.hit();
        const Fpu fpu{true};
        *ox1 = x1;
        *oy1 = y1;
        *ox2 = x2;
        *oy2 = y2;
        jo::view<std::uint32_t>(*sx) = kOne;
        jo::view<std::uint32_t>(*sy) = kOne;
        *sx = static_cast<float>(fpu.div(static_cast<double>(global<std::int32_t>(d::g_screen_w)),
                                         static_cast<double>(static_cast<std::int32_t>(w))));
        *sy = static_cast<float>(fpu.div(static_cast<double>(global<std::int32_t>(d::g_screen_h)),
                                         static_cast<double>(static_cast<std::int32_t>(h))));
        const std::uint32_t by = jo::view<std::uint32_t>(*sy);
        const std::uint32_t bx = jo::view<std::uint32_t>(*sx);
        joan::orig::as_sprite2d(this)->scaled_rect(joan::orig::arg::i16(x1), joan::orig::arg::i16(y1), joan::orig::arg::i16(x2), joan::orig::arg::i16(y2), joan::orig::arg::f32(bx), joan::orig::arg::f32(by), joan::orig::arg::mptr(ox1), joan::orig::arg::mptr(oy1), joan::orig::arg::mptr(ox2), joan::orig::arg::mptr(oy2));  // 0x00633ce0
        return 1;
    }

    // 0x00632100. ret 0x1c.
    std::int32_t queue_line(std::int16_t x0, std::int16_t y0, std::int16_t x1, std::int16_t y1, std::int32_t id,
                            std::uint32_t colour0, std::uint32_t colour1) {
        c_line.hit();
        {
            // canvas (not part of the original game): lines queued by a screen laid out for 1024x768 (the edit box
            // caret and selection, 0x0062d730 / 0x0062ed60) are placed like the sprite queue's quads, or they stay
            // at their 1024x768 coordinates on a larger screen. Each end point is mapped as a point (the first
            // corner of a degenerate rectangle: the second corner is the inclusive one, mapped one pixel wider).
            std::int32_t a[16] = {x0, y0, x0, y0, id};
            std::int32_t b[16] = {x1, y1, x1, y1, id};
            joan::canvas::queue_sprite_args(a);
            joan::canvas::queue_sprite_args(b);
            x0 = static_cast<std::int16_t>(a[0]), y0 = static_cast<std::int16_t>(a[1]);
            x1 = static_cast<std::int16_t>(b[0]), y1 = static_cast<std::int16_t>(b[1]);
        }
        const Fpu fpu{true};
        const double half = static_cast<double>(global<float>(d::flt_0_5));  // 0.5f
        auto centred = [&](std::int16_t v, img::Addr size) {
            return static_cast<float>(fpu.sub(static_cast<double>(static_cast<std::int32_t>(v)),
                                              fpu.mul(static_cast<double>(global<std::int32_t>(size)), half)));
        };
        const float px0 = centred(x0, d::g_screen_w);
        const float py0 = centred(y0, d::g_screen_h);
        const float px1 = centred(x1, d::g_screen_w);
        const float py1 = centred(y1, d::g_screen_h);
        joan::orig::as_sprite2d(this)->add_point(joan::orig::arg::i16(x1), joan::orig::arg::i16(y1), joan::orig::arg::i32(id), joan::orig::arg::u32(colour1));  // 0x00631e90
        void* const owner = joan::orig::arg::as<void*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Sprite2D>(this)->scene))->group(joan::orig::arg::i32(id)));  // 0x005ea6e0
        void* const plane = jo::view<void*>(jo::as_scene_s(owner)->plane);
        if (static_cast<std::int32_t>(jo::view<std::uint32_t>(jo::as_plane(plane)->vertex_count) + 2u) > 0x1000 ||
            static_cast<std::int32_t>(jo::view<std::uint32_t>(jo::as_plane(plane)->triangle_count) + 1u) > 0x1555)
            return 0;
        alignas(alignof(void*)) std::uint8_t material[((sizeof(jo::Material) > 0x54 ? sizeof(jo::Material) : 0x54) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger Material
        joan::orig::as_material(material)->construct();  // 0x005d52a0
        jo::as_material(material)->pass_mask = 2;
        jo::as_material(material)->blend = 3;
        jo::as_material(material)->texture_mode = 0x10000;
        jo::as_material(material)->filter = 1;
        std::int32_t index = jo::as_plane(plane)->material_count - 1;
        bool same = false;
        if (index >= 0) {
            void* const last = &jo::as_plane(plane)->materials[index];
            same = (joan::orig::arg::as<std::uint32_t>(joan::orig::as_material(material)->equals(joan::orig::arg::mptr(last))) & 0xffu) != 0;  // 0x005d5500
        }
        if (!same) {
            index = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_materials(1));                        // 0x005cd470
            joan::orig::as_material(&jo::as_plane(plane)->materials[index])->assign(joan::orig::arg::mptr(material));  // 0x005d53e0
        }
        joan::orig::as_material(material)->dtor();  // 0x005d5360
        const std::int32_t v = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_vertices(2));  // 0x005cd7f0
        {
            jo::VertexF4* const p = &jo::as_plane(plane)->vertices_f4[v];
            jo::view<std::uint32_t>(jo::as_vertex_f4(p)->pos[0]) = bits(px0);
            jo::view<std::uint32_t>(jo::as_vertex_f4(p)->pos[1]) = bits(py0);
            jo::view<std::uint32_t>(jo::as_vertex_f4(p)->pos[2]) = 0;
        }
        joan::orig::as_colour(jo::as_plane(plane)->vertices_f4[v].attr)->from_rgba(&colour0);  // 0x005e95b0
        {
            jo::VertexF4* const p = &jo::as_plane(plane)->vertices_f4[v];
            jo::view<std::uint32_t>(jo::as_vertex_f4(p)->attr[1]) = 0;
            jo::view<std::uint32_t>(jo::as_vertex_f4(p)->attr[2]) = 0;
        }
        {
            jo::VertexF4* const p = &jo::as_plane(plane)->vertices_f4[v + 1];
            jo::view<std::uint32_t>(jo::as_vertex_f4(p)->pos[0]) = bits(px1);
            jo::view<std::uint32_t>(jo::as_vertex_f4(p)->pos[1]) = bits(py1);
            jo::view<std::uint32_t>(jo::as_vertex_f4(p)->pos[2]) = 0;
        }
        joan::orig::as_colour(jo::as_plane(plane)->vertices_f4[v + 1].attr)->from_rgba(&colour1);
        {
            jo::VertexF4* const p = &jo::as_plane(plane)->vertices_f4[v + 1];
            jo::view<std::uint32_t>(jo::as_vertex_f4(p)->attr[1]) = kOne;
            jo::view<std::uint32_t>(jo::as_vertex_f4(p)->attr[2]) = 0;
        }
        const std::int32_t prim = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_triangles(1));  // 0x005ce6f0
        joan::orig::as_plane(plane)->set(joan::orig::arg::u32(prim), joan::orig::arg::u32(index), joan::orig::arg::u32(v), joan::orig::arg::u32(v + 1));                      // 0x005ce8a0
        return 1;
    }

    // 0x00633070. ret 0x40.
    std::int32_t queue_frame(std::int16_t x1, std::int16_t y1, std::int16_t x2, std::int16_t y2, std::uint32_t a5,
                             std::uint32_t a6, std::int16_t t, void* a8, std::int16_t s9, std::int16_t s10,
                             std::int16_t s11, std::int16_t s12, std::int16_t s13, std::int16_t s14, std::int16_t s15,
                             std::int16_t s16) {
        c_frame.hit();
        const std::int32_t X1 = x1, Y1 = y1, X2 = x2, Y2 = y2, T = t;
        auto bar = [&](std::int32_t ax, std::int32_t ay, std::int32_t bx, std::int32_t by) {
            return thiscall<std::int32_t>(kQueueSprite, this, ax, ay, bx, by, a5, a6, a8 /* the texture */, 3u, static_cast<std::int32_t>(s9),
                                          static_cast<std::int32_t>(s10), static_cast<std::int32_t>(s11),
                                          static_cast<std::int32_t>(s12), static_cast<std::int32_t>(s13),
                                          static_cast<std::int32_t>(s14), static_cast<std::int32_t>(s15),
                                          static_cast<std::int32_t>(s16));
        };
        if (bar(X1, Y1, X2 - T, Y1 + T - 1) == 0) return 0;
        if (bar(X1, Y1 + T, X1 + T - 1, Y2) == 0) return 0;
        if (bar(X1 + T, Y2 - T + 1, X2, Y2) == 0) return 0;
        if (bar(X2 - T + 1, Y1, X2, Y2 - T) == 0) return 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_05", name, addr, fn)
B("Sprite2D::fit_triangle", 0x006340e0, &H::fit_triangle);
B("Sprite2D::queue_line", 0x00632100, &H::queue_line);
B("Sprite2D::queue_frame", 0x00633070, &H::queue_frame);
B("Sprite2D::fit_rect", 0x00633e60, &H::fit_rect);
#undef B

}  // namespace game_b83_05
}  // namespace

// ---------------------------------------- from modules/game_b116_11.cpp (game/b116_11)
namespace {
namespace game_b116_11 {

u32 f_bits(float f) {
    u32 b = 0;
    std::memcpy(&b, &f, 4);
    return b;
}

constexpr Fpu kFpu{true};
constexpr img::Addr kScreenW = d::g_screen_w, kScreenH = d::g_screen_h;

// fild [W or H]; fdiv 2.0f (rounded once to float).
float half(img::Addr g) { return static_cast<float>(static_cast<double>(global<i32>(g)) * 0.5); }

JOAN_COUNTER(c_queue, "Sprite2D::queue");
JOAN_COUNTER(c_place, "Layer2D::place_model");

struct H {
    // 0x00632a70
    u32 queue(i32 x1_arg, i32 y1_arg, i32 x2_arg, i32 y2_arg, u32 layer, u32 colour, void* tex, i32 blend, i32 src_mode, i32 dst_mode,
              i32 op_mode, i32 u1_arg, i32 v1_arg, i32 u2_arg, i32 v2_arg, i32 rotation) {
        {
            // canvas: what the entry wrapper did (canvas_api.hpp) - the 16 arguments as 32-bit words, rewritten
            // before anything else runs (only x1, y1, x2, y2 are read or written; the texture pointer's slot is
            // never read nor written back, it holds 0 on x64: the pointer does not fit a word).
            std::int32_t args[16] = {x1_arg, y1_arg, x2_arg, y2_arg, static_cast<i32>(layer), static_cast<i32>(colour),
                                     0, blend,
                                     src_mode, dst_mode, op_mode, u1_arg, v1_arg, u2_arg, v2_arg, rotation};
            joan::canvas::queue_sprite_args(args, JOAN_CANVAS_FRAME());
            x1_arg = args[0], y1_arg = args[1], x2_arg = args[2], y2_arg = args[3];
            layer = static_cast<u32>(args[4]), colour = static_cast<u32>(args[5]);
            blend = args[7], src_mode = args[8], dst_mode = args[9], op_mode = args[10];
            u1_arg = args[11], v1_arg = args[12], u2_arg = args[13], v2_arg = args[14], rotation = args[15];
        }
        c_queue.hit();
        if (tex && joan::orig::arg::as<u32>(joan::orig::as_texture(tex)->ready()) == 0) return 0;  // 0x00605b70
        // Corner positions (4 x vec3) followed by the UV pairs, as in the original frame.
        u32 fr[20];
        const float px1 = (static_cast<float>(static_cast<i16>(x1_arg)) - 0.5f) - half(kScreenW);
        const float px2 = (static_cast<float>(static_cast<i16>(x2_arg)) + 0.5f) - half(kScreenW);
        const float py1 = (static_cast<float>(static_cast<i16>(y1_arg)) - 0.5f) - half(kScreenH);
        const float py2 = (static_cast<float>(static_cast<i16>(y2_arg)) + 0.5f) - half(kScreenH);
        u32 u1 = 0, v1 = 0, u2 = 0, v2 = 0;
        if (tex) {
            const i16 a = static_cast<i16>(u1_arg), b = static_cast<i16>(v1_arg);
            const i16 c = static_cast<i16>(u2_arg), d = static_cast<i16>(v2_arg);
            u1 = a == -1 ? 0u : f_bits(static_cast<float>(kFpu.div(a, jo::view<i32>(jo::as_texture(tex)->width))));
            v1 = b == -1 ? 0u : f_bits(static_cast<float>(kFpu.div(b, jo::view<i32>(jo::as_texture(tex)->height))));
            u2 = c == -1 ? 0x3f800000u : f_bits(static_cast<float>(kFpu.div(c + 1, jo::view<i32>(jo::as_texture(tex)->width))));
            v2 = d == -1 ? 0x3f800000u : f_bits(static_cast<float>(kFpu.div(d + 1, jo::view<i32>(jo::as_texture(tex)->height))));
        }
        u8* const mesh = jo::view<u8*>(jo::as_scene_s(joan::orig::arg::as<u8*>(joan::orig::as_scene(jo::view<u8*>(jo::self<jo::Sprite2D>(this)->scene))->group(joan::orig::arg::i32(layer))))->plane);  // 0x005ea6e0
        if (jo::as_plane(mesh)->vertex_count + 4 > 0x1000 || jo::as_plane(mesh)->triangle_count + 2 > 0x1555) return 0;
        alignas(alignof(void*)) u8 mat[((sizeof(jo::Material) > 0x54 ? sizeof(jo::Material) : 0x54) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger Material
        joan::orig::as_material(mat)->construct();  // 0x005d52a0
        jo::as_material(mat)->pass_mask = 2;
        jo::view<i32>(jo::as_material(mat)->blend) = static_cast<i16>(blend);
        jo::as_material(mat)->texture_mode = global<u32>(d::g_a1977c + static_cast<std::uintptr_t>(4 * static_cast<i32>(static_cast<i16>(src_mode)))) |
                             0x10000u |
                             global<u32>(d::g_a19790 + static_cast<std::uintptr_t>(4 * static_cast<i32>(static_cast<i16>(dst_mode))));
        jo::as_material(mat)->filter = global<u32>(d::g_a197a4 + static_cast<std::uintptr_t>(4 * static_cast<i32>(static_cast<i16>(op_mode))));
        joan::orig::as_material(mat)->set_texture(i32{0}, joan::orig::arg::mptr(tex));  // 0x005d5650
        i32 mi = jo::as_plane(mesh)->material_count - 1;
        if (mi < 0 || static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_material(mat)->equals(joan::orig::arg::mptr(&jo::as_plane(mesh)->materials[mi])))) == 0) {  // 0x005d5500
            mi = joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->add_materials(i32{1}));                    // 0x005cd470
            joan::orig::as_material(&jo::as_plane(mesh)->materials[mi])->assign(joan::orig::arg::mptr(mat));  // 0x005d53e0
        }
        joan::orig::as_material(mat)->dtor();  // 0x005d5360
        fr[0] = f_bits(px1), fr[1] = f_bits(py1), fr[2] = 0;
        fr[3] = f_bits(px2), fr[4] = f_bits(py1), fr[5] = 0;
        fr[6] = f_bits(px2), fr[7] = f_bits(py2), fr[8] = 0;
        fr[9] = f_bits(px1), fr[10] = f_bits(py2), fr[11] = 0;
        fr[12] = u1, fr[13] = v1, fr[14] = u2, fr[15] = v1;
        fr[16] = u2, fr[17] = v2, fr[18] = u1, fr[19] = v2;
        const i32 base = joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->add_vertices(i32{4}));  // 0x005cd7f0
        jo::VertexF4* v = &jo::as_plane(mesh)->vertices_f4[base];
        for (i32 i = 0; i < 4; ++i, ++v) {
            std::memcpy(v, &fr[3 * i], 12);
            joan::orig::as_colour(v->attr)->from_rgba(&colour);  // 0x005e95b0
            const i32 k = (static_cast<i32>(static_cast<i16>(rotation)) + i) % 4;  // -3..3 (MSVC remainder)
            jo::view<u32>(jo::as_vertex_f4(v)->attr[1]) = fr[12 + 2 * k];
            jo::view<u32>(jo::as_vertex_f4(v)->attr[2]) = fr[13 + 2 * k];
        }
        const i32 tri = joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->add_triangles(i32{2}));  // 0x005ce6f0
        joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(tri), joan::orig::arg::u32(mi), joan::orig::arg::u32(base), joan::orig::arg::u32(base + 3), joan::orig::arg::u32(base + 1));      // 0x005ce800
        joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(tri + 1), joan::orig::arg::u32(mi), joan::orig::arg::u32(base + 1), joan::orig::arg::u32(base + 3), joan::orig::arg::u32(base + 2));  // 0x005ce800
        // The stream cache (game/unknown_d3dsys.cpp) checks this layer's vertices in full on their next fill: the
        // layer is rebuilt every frame and a hover or a text change moves only a few quads, which its sample missed.
        joan::stream_cache_api::vertices_written(joan::orig::arg::as<void*>(joan::orig::as_plane(mesh)->buffer(joan::orig::arg::mptr(0))));  // 0x005cdae0
        return 1;
    }

    // 0x006349b0
    u32 place_model(i32 x_arg, i32 y_arg, u32 id, u8** model, u32 rx, u32 ry, u32 rz, u32 scale) {
        {
            // canvas: what the entry wrapper did (canvas_api.hpp) - the 8 arguments as 32-bit words, rewritten before
            // anything else runs (x, y and the scale bits are read and written; the model pointer's slot is never
            // read nor written back, it holds 0 on x64: the pointer does not fit a word).
            std::int32_t args[8] = {x_arg, y_arg, static_cast<i32>(id), 0,

                                    static_cast<i32>(rx), static_cast<i32>(ry), static_cast<i32>(rz), static_cast<i32>(scale)};
            joan::canvas::place_model_args(args);
            x_arg = args[0], y_arg = args[1], id = static_cast<u32>(args[2]);
            rx = static_cast<u32>(args[4]), ry = static_cast<u32>(args[5]), rz = static_cast<u32>(args[6]), scale = static_cast<u32>(args[7]);
        }
        c_place.hit();
        u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_scene(jo::view<u8*>(jo::self<jo::Sprite2D>(this)->scene))->group(joan::orig::arg::i32(id)));  // 0x005ea6e0
        const float x = static_cast<float>(static_cast<i16>(x_arg)) - half(kScreenW);
        const float y = static_cast<float>(static_cast<i16>(y_arg)) - half(kScreenH);
        jo::view<u32>(jo::as_actor(*model)->scale) = scale;
        const float z = (jo::view<float>(jo::as_scene_s(m)->desc[2]) + jo::view<float>(jo::as_scene_s(m)->desc[5])) * 0.5f;
        joan::orig::as_actor(*model)->set_position_5ac4b0(joan::orig::arg::bits(x), joan::orig::arg::bits(y), joan::orig::arg::bits(z));       // 0x005ac4b0
        joan::orig::as_scene_object(*model)->set_angles(joan::orig::arg::u32(rx), joan::orig::arg::u32(ry), joan::orig::arg::u32(rz));    // 0x005acaf0
        joan::orig::as_scene(m)->attach(joan::orig::arg::mptr(*model));             // 0x005efc50
        return 1;
    }
};

#define B(name, address, fn) JOAN_REPLACE("game/b116_11", name, address, fn)
B("Sprite2D::queue", 0x00632a70, &H::queue);
B("Layer2D::place_model", 0x006349b0, &H::place_model);
JOAN_CANVAS_SELF(0x00632a70);  // queue_sprite_args in the body (no entry wrapper)
JOAN_CANVAS_SELF(0x006349b0);  // place_model_args in the body (no entry wrapper)
#undef B

}  // namespace game_b116_11
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

using i8 = std::int8_t;

void* const kSprites = obj(d::g_sprites_a46128);

JOAN_COUNTER(c_draw_entry, "Sprites::draw_entry_6338b0");

struct H {
    // ------------------------------------------------------------------------ 0x006338b0
    u32 draw_entry(i16 x, i16 y, u32 a10, const char* a14, void* a18, u32 a1c, i16 a20, i16 a24, i16 a28, i16 a2c, i16 a30,
                   i16 a34, i16 a38, i16 a3c, i16 a40, i16 a44, i16 a48, i16 a4c) {
        c_draw_entry.hit();
        if (a14 == nullptr || jo::view<i8>(jo::as_bmp_res(a18)->loaded) == 0) return 0;

        void* const e = joan::orig::arg::as<void*>(joan::orig::as_bmp_res(a18)->find(joan::orig::arg::str(a14)));  // 0x00620b40
        if (e == nullptr) return 0;
        void* const t = joan::orig::arg::as<void*>(joan::orig::as_textures(obj(d::g_texture_array))->find(joan::orig::arg::str(jo::as_bmp_entry(e)->file)));  // 0x0060a4b0
        if (t == nullptr || joan::orig::arg::as<i32>(joan::orig::as_texture(t)->ready()) == 0) return 0;          // 0x00605b70
        const i16 s10 = jo::view<i16>(jo::as_bmp_entry(e)->x0) == -1 ? i16{0} : jo::view<i16>(jo::as_bmp_entry(e)->x0);
        const i16 s14 = jo::view<i16>(jo::as_bmp_entry(e)->y0) == -1 ? i16{0} : jo::view<i16>(jo::as_bmp_entry(e)->y0);
        const i16 s18 = jo::view<i16>(jo::as_bmp_entry(e)->x1) == -1 ? static_cast<i16>(jo::view<i32>(jo::as_texture(t)->width) - 1) : jo::view<i16>(jo::as_bmp_entry(e)->x1);
        const i16 s1c = jo::view<i16>(jo::as_bmp_entry(e)->y1) == -1 ? static_cast<i16>(jo::view<i32>(jo::as_texture(t)->height) - 1) : jo::view<i16>(jo::as_bmp_entry(e)->y1);
        const i16 s20 = a2c == -1 ? s10 : static_cast<i16>(s10 + a2c);
        const i16 s24 = a30 == -1 ? s14 : static_cast<i16>(s14 + a30);
        const i16 s28 = a34 == -1 ? s18 : static_cast<i16>(s10 + a34);
        const i16 s2c = a38 == -1 ? s1c : static_cast<i16>(s14 + a38);
        const bool even = (static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(e)->u16_32)) + a3c) % 2 == 0;
        const i16 wx = static_cast<i16>(s28 - s20 + 1);
        const i16 wy = static_cast<i16>(s2c - s24 + 1);
        const i16 s30 = even ? wx : wy;
        const i16 s34 = even ? wy : wx;
        i16 s38 = x, s3c = y, s40 = a40, s44 = a44;
        float f48 = 1.0f, f4c = 1.0f;
        const i16 s78 = a44 == -1 ? static_cast<i16>(y + s34 - 1) : a44;
        const i16 s7c = a40 == -1 ? static_cast<i16>(x + s30 - 1) : a40;
        joan::orig::as_sprite2d(kSprites)->fit_rect(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::i16(s7c), joan::orig::arg::i16(s78), &f48, &f4c, &s38, &s3c, &s40, &s44, joan::orig::arg::i16(a48), joan::orig::arg::i16(a4c));  // 0x00633e60
        return joan::orig::arg::as<u32>(joan::orig::as_sprite2d(this)->queue(joan::orig::arg::i32(s38), joan::orig::arg::i32(s3c), joan::orig::arg::i32(s40), joan::orig::arg::i32(s44), joan::orig::arg::u32(a10), joan::orig::arg::u32(a1c), joan::orig::arg::mptr(t), i32{3}, joan::orig::arg::i32(a20), joan::orig::arg::i32(a24), joan::orig::arg::i32(a28), joan::orig::arg::i32(s20), joan::orig::arg::i32(s24), joan::orig::arg::i32(s28), joan::orig::arg::i32(s2c), joan::orig::arg::i32(static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(e)->u16_32)) + a3c)));  // 0x00632a70
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Sprites::draw_entry_6338b0", 0x006338b0, &H::draw_entry);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
namespace {
namespace game_b102_12 {

JOAN_COUNTER(c_scale_rect, "Screen::scale_rect");

struct H {
    // ------------------------------------------------------------------ 0x00633f50
    u32 scale_rect(u16 l, u16 t, u16 r, u16 b, float* sx, float* sy, i16* pl, i16* pt, i16* pr, i16* pb) {
        c_scale_rect.hit();
        *pl = static_cast<i16>(l);
        *pt = static_cast<i16>(t);
        *pr = static_cast<i16>(r);
        *pb = static_cast<i16>(b);
        jo::view<u32>(*sx) = 0x3f800000u;
        jo::view<u32>(*sy) = 0x3f800000u;
        const Fpu fpu{true};
        // fild dword; fdiv dword: one rounding of int / float
        *sx = static_cast<float>(fpu.div(static_cast<double>(global<i32>(d::g_screen_w)),
                                          static_cast<double>(global<float>(d::flt_640))));  // 640
        *sy = static_cast<float>(fpu.div(static_cast<double>(global<i32>(d::g_screen_h)),
                                          static_cast<double>(global<float>(d::flt_480))));  // 480
        joan::orig::as_sprite2d(this)->scaled_rect(joan::orig::arg::i16(l), joan::orig::arg::i16(t), joan::orig::arg::i16(r), joan::orig::arg::i16(b), joan::orig::arg::f32(jo::view<u32>(*sx)), joan::orig::arg::f32(jo::view<u32>(*sy)), joan::orig::arg::mptr(pl), joan::orig::arg::mptr(pt), joan::orig::arg::mptr(pr), joan::orig::arg::mptr(pb));  // 0x00633ce0
        const i16 w = static_cast<i16>(static_cast<i32>(static_cast<i16>(r)) - static_cast<i32>(static_cast<i16>(l)));
        const i16 h = static_cast<i16>(static_cast<i32>(static_cast<i16>(b)) - static_cast<i32>(static_cast<i16>(t)));
        {
            const i32 c = *pl;
            const i32 d = static_cast<i32>(*pr) - static_cast<i32>(*pl) - static_cast<i32>(w);
            *pl = static_cast<i16>(c + d / 2);
        }
        {
            const i32 c = *pt;
            const i32 d = static_cast<i32>(*pb) - static_cast<i32>(*pt) - static_cast<i32>(h);
            *pt = static_cast<i16>(c + d / 2);
        }
        *pr = static_cast<i16>(static_cast<i32>(*pl) + static_cast<i32>(w) - 1);
        *pb = static_cast<i16>(static_cast<i32>(*pt) + static_cast<i32>(h) - 1);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("Screen::scale_rect", 0x00633f50, &H::scale_rect);
#undef B

}  // namespace game_b102_12
}  // namespace

// ---------------------------------------- from modules/game_b93_04.cpp (game/b93_04)
namespace {
namespace game_b93_04 {

struct H {
    // ------------------------------------------------------------------ 0x00634b20
    void* flags_ctor() {
        jo::self<jo::Flags>(this)->u16_0 = 0;
        jo::self<jo::Flags>(this)->flag_a = 1;
        jo::self<jo::Flags>(this)->flag_b = 1;
        jo::self<jo::Flags>(this)->flag_c = 1;
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("Flags::Flags", 0x00634b20, &H::flags_ctor);
#undef B93

}  // namespace game_b93_04
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

struct H {
    H* clear_15() {  // 0x00634b60
        std::memset(static_cast<void*>(this), 0, 0xfu);                        // memset; x64: class unknown (15 bytes; sizeof once typed)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Obj::clear_15", 0x00634b60, &H::clear_15);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_b111_10.cpp (game/b111_10)
//
//      0x00633500 SpriteQueue::draw_named(17 args) (thiscall, ret 0x44) - 0x00632a70 wrapper
//
//  - Word arguments the original pushes from a 16-bit register (upper bits stale) are passed
//    sign-extended; every callee concerned reads the low word only (checked: 0x00632a70 args
//    1-4 / 8-16, 0x00626130, 0x006255d0, 0x00626080, 0x0042d0a0, 0x005094d0, and 0x00633500
//    itself). Byte arguments (0x0042a260 colour ctor, 0x005f0340) are read as bytes.
namespace {
namespace game_b111_10 {

using i8 = std::int8_t;

i32 sx(u16 v) { return static_cast<i32>(static_cast<i16>(v)); }

JOAN_COUNTER(c_draw_named, "SpriteQueue::draw_named");

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

}  // namespace game_b111_10
}  // namespace

namespace joan::orig {

using game_b111_10::c_draw_named;
using game_b111_10::i8;
using game_b111_10::sx;

// ------------------------------------------------------------------ 0x00633500
// Looks the named sprite up in a sprite set, fills the -1 defaults of the source rectangle /
// destination corner from the set entry and the texture size and queues it (0x00632a70).
u32 SpriteQueue::draw_named(i16 x, i16 y, u32 a10, const char* name, void* set, u32 a1c, i16 a20, i16 a24, i16 a28, i16 a2c,
                            i16 a30, i16 a34, i16 a38, i16 a3c, i16 a40, i16 a44, i16 a48) {
    c_draw_named.hit();
    if (name == nullptr || *static_cast<i8*>(set) == 0) return 0;
    u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(set)->find(joan::orig::arg::str(name)));  // 0x00620b40
    if (e == nullptr) return 0;
    u8* const tex = joan::orig::arg::as<u8*>(joan::orig::as_textures(obj(d::g_texture_array))->find(joan::orig::arg::str(e + 9)));  // 0x0060a4b0
    if (tex == nullptr || joan::orig::arg::as<i32>(joan::orig::as_texture(tex)->ready()) == 0) return 0;  // 0x00605b70
    const i16 sx0 = static_cast<i16>(sx(jo::as_bmp_entry(e)->x0) == -1 ? 0 : sx(jo::as_bmp_entry(e)->x0));
    const i16 sy0 = static_cast<i16>(sx(jo::as_bmp_entry(e)->y0) == -1 ? 0 : sx(jo::as_bmp_entry(e)->y0));
    const i16 sx1 = static_cast<i16>(sx(jo::as_bmp_entry(e)->x1) == -1 ? jo::view<i32>(jo::as_texture(tex)->width) - 1 : sx(jo::as_bmp_entry(e)->x1));
    const i16 sy1 = static_cast<i16>(sx(jo::as_bmp_entry(e)->y1) == -1 ? jo::view<i32>(jo::as_texture(tex)->height) - 1 : sx(jo::as_bmp_entry(e)->y1));
    const i32 o30 = sx(static_cast<u16>(a30)), o34 = sx(static_cast<u16>(a34));
    const i32 o38 = sx(static_cast<u16>(a38)), o3c = sx(static_cast<u16>(a3c));
    const i16 u0 = static_cast<i16>(o30 == -1 ? sx0 : sx0 + o30);
    const i16 v0 = static_cast<i16>(o34 == -1 ? sy0 : sy0 + o34);
    const i16 u1 = static_cast<i16>(o38 == -1 ? sx1 : sx0 + o38);
    const i16 v1 = static_cast<i16>(o3c == -1 ? sy1 : sy0 + o3c);
    const i32 rot = sx(jo::as_bmp_entry(e)->u16_32) + sx(static_cast<u16>(a40));
    const bool even = rot % 2 == 0;
    const i16 w = static_cast<i16>(even ? u1 - u0 + 1 : v1 - v0 + 1);
    const i16 h = static_cast<i16>(even ? v1 - v0 + 1 : u1 - u0 + 1);
    const i32 x0 = sx(static_cast<u16>(x)), y0 = sx(static_cast<u16>(y));
    const i32 y1 = sx(static_cast<u16>(a48)) == -1 ? y0 + h - 1 : sx(static_cast<u16>(a48));
    const i32 x1 = sx(static_cast<u16>(a44)) == -1 ? x0 + w - 1 : sx(static_cast<u16>(a44));
    return joan::orig::arg::as<u32>(joan::orig::as_sprite2d(this)->queue(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(sx(static_cast<u16>(x1))), joan::orig::arg::i32(sx(static_cast<u16>(y1))), joan::orig::arg::u32(a10), joan::orig::arg::u32(a1c), joan::orig::arg::mptr(tex), joan::orig::arg::i32(sx(static_cast<u16>(a20))), joan::orig::arg::i32(sx(static_cast<u16>(a24))), joan::orig::arg::i32(sx(static_cast<u16>(a28))), joan::orig::arg::i32(sx(static_cast<u16>(a2c))), i32{u0}, i32{v0}, i32{u1}, i32{v1}, joan::orig::arg::i32(rot)));  // 0x00632a70
}

}  // namespace joan::orig

namespace {
namespace game_b111_10 {

#define B(name, addr, fn) JOAN_REPLACE("game/b111_10", name, addr, fn)
B("SpriteQueue::draw_named_633500", 0x00633500, &joan::orig::SpriteQueue::draw_named);
#undef B

}  // namespace game_b111_10
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Panel::dtor_6317e0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_05::H::dtor_6317e0>(this)); }  // 0x6317e0
auto joan::orig::Camera::build_631850() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_11::H::camera_build>(this)); }  // 0x631850
auto joan::orig::Screen::close_631c60() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_17::H::close_631c60>(this)); }  // 0x631c60
auto joan::orig::SpriteQueue::build_hook() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_11::H::true1>(this)); }  // 0x631d10
auto joan::orig::SpriteQueue::close_hook() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_11::H::true1>(this)); }  // 0x631d30
auto joan::orig::SpriteQueue::begin_frame() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_ui7::H::forward_614630>(this)); }  // 0x631d50
auto joan::orig::SpriteQueue::end_frame() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_ui7::H::reset_631d80>(this)); }  // 0x631d80
auto joan::orig::SpriteQueue::release_list() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_ui7::H::release_list>(this)); }  // 0x631dc0
auto joan::orig::Sprite2D::add_point(std::int16_t a0, std::int16_t a1, std::int32_t a2, std::uint32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_04::H::add_point>(this, a0, a1, a2, a3)); }  // 0x631e90
auto joan::orig::Sprite2D::queue_line(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::int32_t a4, std::uint32_t a5, std::uint32_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_05::H::queue_line>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x632100
auto joan::orig::Sprite2D::queue_frame(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::uint32_t a4, std::uint32_t a5, std::int16_t a6, void* a7, std::int16_t a8, std::int16_t a9, std::int16_t a10, std::int16_t a11, std::int16_t a12, std::int16_t a13, std::int16_t a14, std::int16_t a15) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_05::H::queue_frame>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15)); }  // 0x633070
auto joan::orig::SpriteQueue::blit_image(std::int16_t a0, std::int16_t a1, std::uint32_t a2, void* a3, std::uint32_t a4, std::int16_t a5, std::int16_t a6, std::int16_t a7, std::int16_t a8, std::int16_t a9, std::int16_t a10, std::int16_t a11, std::int16_t a12, std::int16_t a13, std::int16_t a14) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_ui7::H::blit_image>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)); }  // 0x6332b0
auto joan::orig::Sprite2D::scaled_rect(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, float a4, float a5, void* a6, void* a7, void* a8, void* a9) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_04::H::scaled_rect>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x633ce0
auto joan::orig::Sprite2D::fit_rect(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, void* a4, void* a5, void* a6, void* a7, void* a8, void* a9, std::int16_t a10, std::int16_t a11) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_05::H::fit_rect>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)); }  // 0x633e60
auto joan::orig::Sprite2D::fit_triangle(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::int16_t a4, std::int16_t a5, void* a6, void* a7, void* a8, void* a9, void* a10, void* a11, void* a12, void* a13) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_05::H::fit_triangle>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)); }  // 0x6340e0
auto joan::orig::fmax(float a0, float a1) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_b84_04::fmax_>(a0, a1)); }  // 0x634aa0
auto joan::orig::fmin(float a0, float a1) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_b84_04::fmin_>(a0, a1)); }  // 0x634ae0
auto joan::orig::GadgetFlags::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_04::H::flags_ctor>(this)); }  // 0x634b20
