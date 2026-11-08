// game/unknown_clouds.cpp: functions whose original source file is unknown (formerly game/_unplaced/clouds.cpp;
// re/attribution.csv: between ww/camres.cpp and ww/cpuusage.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

using joan::call::cdecl_call;
using joan::img::obj;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::orig::helpers::memset_;

struct H {
    H* clear_15() {
        memset_(this, 0, 0x15);
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Obj::clear_15", 0x004586c0, &H::clear_15);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

struct H {
    void forward_459b70() { joan::orig::as_cloud_layers(this)->release(); }  // 0x004586f0
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::forward_459b70", 0x004586f0, &H::forward_459b70);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_misc81.cpp (game/misc81)
namespace {
namespace game_misc81 {

using joan::img::str;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;

bool equal_or_nan(float a, float b) { return !(a < b || a > b); }

JOAN_COUNTER(c_build, "CloudLayers::build");

constexpr img::Addr kTextures(d::g_texture_effect_res);
constexpr img::Addr kTexMan(d::g_texture_array);
constexpr img::Addr kClamp(d::g_misc);

double crt_fabs(double x) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(x))); }
float lerp(float a, float b, float t) { return joan::orig::arg::as<float>(joan::orig::lerp(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(t))); }  // direct call

struct H {
    // Vertex i (0x24 bytes) of the mesh, re-reading the vertex array pointer (+0x1c).
    static std::uint8_t* vtx(void* mesh, std::int32_t i) { return reinterpret_cast<std::uint8_t*>(&jo::as_plane(mesh)->vertices_f1[i]); }

    // slot: CloudLayers::layer1 / layer2
    void layer(jo::Plane*& slot, const char* name, const char* tex_arg, const char* fallback, std::uint32_t z_bits,
               std::int32_t step, std::int32_t nx, std::int32_t ny, float x0, float y0) {
        const joan::hook::x87::Fpu fpu{true};
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // operator new (0x11c on x86)
        void* const created = mem != nullptr ? joan::orig::as_plane(mem)->construct(1u, 1u) : nullptr;  // 0x005cbaf0
        slot = static_cast<jo::Plane*>(created);
        joan::orig::as_plane(slot)->init(0, 0, 0, 1u);  // 0x005cc7a0
        joan::orig::as_plane(slot)->set_100(0u);           // 0x005d6e20
        const std::int32_t mat = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(slot)->add_materials(1));  // 0x005cd470
        jo::Material* const m = &jo::as_plane(slot)->materials[mat];
        joan::orig::as_material(m)->construct();  // 0x005d52a0
        jo::as_material(m)->blend = 0x10;
        std::uint32_t tex = joan::orig::arg::as<std::uint32_t>(joan::orig::as_anim_db(img::obj(kTextures))->find(joan::orig::arg::str(name)));  // 0x004dff70
        if (tex == 0) tex = joan::orig::arg::as<std::uint32_t>(joan::orig::as_anim_db(img::obj(kTextures))->find(joan::orig::arg::str(fallback)));
        jo::as_material(m)->texture_mode = tex | 0x1000000u;
        jo::as_material(m)->pass_mask = 2;
        jo::as_material(m)->raster = 0x100;
        void* const h = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTexMan))->load(joan::orig::arg::str(tex_arg), 1, 0u));  // 0x006099c0
        joan::orig::as_material(m)->set_texture(0, h);       // 0x005d5650
        joan::orig::as_texture_manager(img::obj(kTexMan))->release(h);    // 0x0060a550

        joan::orig::as_plane(slot)->add_vertices(joan::orig::arg::i32(nx * ny));  // 0x005cd7f0
        for (std::int32_t y = 0; y < ny; ++y) {
            for (std::int32_t x = 0; x < nx; ++x) {
                const std::int32_t i = nx * y + x;
                void* mesh = slot;
                jo::as_vertex_f1(vtx(mesh, i))->pos[0] = static_cast<float>(fpu.add(static_cast<double>(x * step), x0));
                jo::as_vertex_f1(vtx(mesh, i))->pos[1] = static_cast<float>(fpu.add(static_cast<double>(y * step), y0));
                jo::view<std::uint32_t>(jo::as_vertex_f1(vtx(mesh, i))->pos[2]) = z_bits;
                jo::VertexF1* const n = jo::as_vertex_f1(vtx(mesh, i));
                jo::view<std::uint32_t>(n->attr[0]) = global<std::uint32_t>(d::g_a18a60);
                jo::view<std::uint32_t>(n->attr[1]) = global<std::uint32_t>(d::g_a18a64);
                jo::view<std::uint32_t>(n->attr[2]) = global<std::uint32_t>(d::g_a18a68);
                jo::view<std::uint32_t>(jo::as_vertex_f1(vtx(mesh, i))->attr[3]) = global<std::uint32_t>(d::g_init_white);
                const double ax = crt_fabs(jo::as_vertex_f1(vtx(mesh, i))->pos[0]);
                const double ay = crt_fabs(jo::as_vertex_f1(vtx(mesh, i))->pos[1]);
                double big;
                if (!(ay >= ax))  // fcomp: |y| < |x| or unordered
                    big = crt_fabs(jo::as_vertex_f1(vtx(mesh, i))->pos[0]);
                else
                    big = crt_fabs(jo::as_vertex_f1(vtx(mesh, i))->pos[1]);
                const double v = fpu.mul(fpu.sub(1.0, fpu.div(big, 1000.0)), 255.0);
                const double c = joan::orig::arg::as<double>(joan::orig::as_misc(img::obj(kClamp))->limit_double(joan::orig::arg::f64(v), joan::orig::arg::f64(0.0), joan::orig::arg::f64(255.0)));  // 0x00590b00
                const std::int32_t a = joan::hook::x87::ftol(c);
                mesh = slot;
                jo::view<std::uint8_t[4]>(jo::as_vertex_f1(vtx(mesh, i))->attr[3])[3] = static_cast<std::uint8_t>(a);  // colour alpha
            }
        }
        {
            void* mesh = slot;
            jo::view<std::uint32_t>(jo::as_vertex_f1(vtx(mesh, 0))->attr[4]) = 0;
            jo::view<std::uint32_t>(jo::as_vertex_f1(vtx(mesh, 0))->attr[5]) = 0;
            jo::view<std::uint32_t>(jo::as_vertex_f1(vtx(mesh, nx - 1))->attr[4]) = 0x40a00000u;  // 5.0
            jo::view<std::uint32_t>(jo::as_vertex_f1(vtx(mesh, nx - 1))->attr[5]) = 0;
            jo::view<std::uint32_t>(jo::as_vertex_f1(vtx(mesh, (ny - 1) * nx))->attr[4]) = 0;
            jo::view<std::uint32_t>(jo::as_vertex_f1(vtx(mesh, (ny - 1) * nx))->attr[5]) = 0x41200000u;  // 10.0
            jo::view<std::uint32_t>(jo::as_vertex_f1(vtx(mesh, (ny - 1) * nx + nx - 1))->attr[4]) = 0x40a00000u;
            jo::view<std::uint32_t>(jo::as_vertex_f1(vtx(mesh, (ny - 1) * nx + nx - 1))->attr[5]) = 0x41200000u;
        }
        for (std::int32_t y = 0; y < ny; ++y) {
            for (std::int32_t x = 0; x < nx; ++x) {
                for (std::int32_t k = 4; k <= 5; ++k) {  // attr[4] u then attr[5] v
                    void* const mesh = slot;
                    if (equal_or_nan(jo::as_vertex_f1(vtx(mesh, 0))->attr[k], jo::as_vertex_f1(vtx(mesh, nx - 1))->attr[k])) {
                        const float t = static_cast<float>(fpu.div(static_cast<double>(y), static_cast<double>(ny - 1)));
                        const float r = lerp(jo::as_vertex_f1(vtx(mesh, 0))->attr[k], jo::as_vertex_f1(vtx(mesh, (ny - 1) * nx))->attr[k], t);
                        jo::as_vertex_f1(vtx(slot, nx * y + x))->attr[k] = r;
                    } else if (equal_or_nan(jo::as_vertex_f1(vtx(mesh, 0))->attr[k], jo::as_vertex_f1(vtx(mesh, (ny - 1) * nx))->attr[k])) {
                        const float t = static_cast<float>(fpu.div(static_cast<double>(x), static_cast<double>(nx - 1)));
                        const float r = lerp(jo::as_vertex_f1(vtx(mesh, 0))->attr[k], jo::as_vertex_f1(vtx(mesh, nx - 1))->attr[k], t);
                        jo::as_vertex_f1(vtx(slot, nx * y + x))->attr[k] = r;
                    }
                }
            }
        }
        joan::orig::as_plane(slot)->add_triangles(joan::orig::arg::i32((nx - 1) * (ny - 1) * 2));  // 0x005ce6f0
        for (std::int32_t y = 0; y < ny - 1; ++y) {
            for (std::int32_t x = 0; x < nx - 1; ++x) {
                const std::int32_t tri = 2 * x + 2 * ((nx - 1) * y);
                joan::orig::as_plane(slot)->set_triangle(joan::orig::arg::i32(tri), joan::orig::arg::u32(mat), joan::orig::arg::u32(nx * y + x), joan::orig::arg::u32(nx * (y + 1) + x), joan::orig::arg::u32(nx * y + x + 1));  // 0x005ce800
                joan::orig::as_plane(slot)->set_triangle(joan::orig::arg::i32(tri + 1), joan::orig::arg::u32(mat), joan::orig::arg::u32(nx * y + x + 1), joan::orig::arg::u32(nx * (y + 1) + x), joan::orig::arg::u32(nx * (y + 1) + x + 1));
            }
        }
        joan::orig::as_plane(slot)->upload();      // 0x005cfa10
        joan::orig::as_plane(slot)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        joan::orig::as_plane(slot)->sort_by_material(1);   // 0x005cf1d0
    }

    // 0x00458720 (ret 8). Layer 1 in +0, layer 2 in +4; note layer 2 looks its texture up by
    // the first name too but passes the second name to 0x006099c0, as the original does.
    void build(const char* name1, const char* name2) {
        joan::orig::as_cloud_layers(this)->release();  // 0x00459b70
        if (jo::self<jo::CloudLayers>(this)->built != 0) return;
        if (global<std::int32_t>(d::g_a52623) == 0) return;
        c_build.hit();
        const std::int32_t step = 0x32;
        const std::int32_t nx = 2000 / step + 2;
        const std::int32_t ny = 2000 / step + 2;
        const float x0 = -1000.0f, y0 = -1000.0f;
        layer(jo::self<jo::CloudLayers>(this)->layer1, name1, name1, &img::global<const char>(d::s_cloud_layer_1), 0x41a00000u, step, nx, ny, x0, y0);  // z 20
        layer(jo::self<jo::CloudLayers>(this)->layer2, name1, name2, &img::global<const char>(d::s_cloud_layer_2), 0x42480000u, step, nx, ny, x0, y0);  // z 50
        jo::self<jo::CloudLayers>(this)->built = 1;
    }
};

JOAN_REPLACE("game/misc81", "CloudLayers::build", 0x00458720, &H::build);

}  // namespace game_misc81
}  // namespace

// ---------------------------------------- from modules/game_b101_01.cpp (game/b101_01)
namespace {
namespace game_b101_01 {

using i8 = std::int8_t;
using i16 = std::int16_t;

JOAN_COUNTER(c_push_apart, "Unit::push_apart_459720");

struct H {
    // ------------------------------------------------------------------ 0x00459720
    void push_apart_459720() {
        c_push_apart.hit();
        if (global<i32>(d::g_player_unit_recno) != static_cast<i32>(jo::self<jo::Unit>(this)->id)) return;
        if (jo::self<jo::Unit>(this)->group == nullptr) return;
        const float r = global<float>(d::flt_300);
        float box[6];
        box[0] = jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[0] - r;
        box[1] = jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[1] - r;
        box[2] = jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[2] - r;
        box[3] = jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[0] + r;
        box[4] = jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[1] + r;
        box[5] = jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[2] + r;
        i32 count = 0;
        void** const nodes = joan::orig::arg::as<void**>(joan::orig::as_world(jo::self<jo::Unit>(this)->world)->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x00429090
        for (i32 i = 0; i < count; ++i) {
            u8* p = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(nodes[i])->get_8c());  // 0x0054ee10
            while (p != nullptr) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(p)->leads()) == 0) {  // 0x0044a660
                    p = joan::orig::arg::as<u8*>(joan::orig::get_33d(joan::orig::arg::mptr(p)));  // 0x0054ee70
                    continue;
                }
                const i32 a = static_cast<i32>(jo::as_group(jo::as_unit(p)->group)->count);
                const i32 b = static_cast<i32>(jo::as_group(jo::self<jo::Unit>(this)->group)->count);
                if (a <= b) {
                    p = joan::orig::arg::as<u8*>(joan::orig::get_33d(joan::orig::arg::mptr(p)));
                    continue;
                }
                if ((joan::orig::arg::as<u32>(joan::orig::as_aabb(static_cast<void*>(box))->contains(static_cast<void*>(jo::as_unit(p)->position_))) & 0xffu) == 0) {  // 0x0060d5f0
                    p = joan::orig::arg::as<u8*>(joan::orig::get_33d(joan::orig::arg::mptr(p)));
                    continue;
                }
                for (i32 k = 0; k < 3; ++k) {
                    const i32 x = joan::orig::arg::as<i32>(joan::orig::as_group(jo::as_unit(p)->group)->count_owner(joan::orig::arg::i32(k)));  // 0x00529810
                    const i32 y = joan::orig::arg::as<i32>(joan::orig::as_group(jo::self<jo::Unit>(this)->group)->count_owner(joan::orig::arg::i32(k)));
                    if (x > y) {
                        const i32 d = static_cast<i32>(static_cast<u32>(x) - static_cast<u32>(y));
                        joan::orig::as_group(jo::self<jo::Unit>(this)->group)->transfer_units(joan::orig::arg::mptr(jo::as_unit(p)->group), joan::orig::arg::i32(k), joan::orig::arg::i32(d / 2));  // 0x00528c60
                    }
                }
                // the original jumps back to the same node (no advance)
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_01", name, addr, fn)
B("Unit::push_apart_459720", 0x00459720, &H::push_apart_459720);
#undef B

}  // namespace game_b101_01
}  // namespace

// ---------------------------------------- from modules/game_b89_04.cpp (game/b89_04)
namespace {
namespace game_b89_04 {

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_unit_event, "Unit::on_event");

struct H {
    // ------------------------------------------------------------------------ 0x00459970
    void notify_linked(std::int32_t message) {
        if (jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->rider_ref) == 0) return;
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->rider_ref))) != 0) return;  // 0x00428b70
        void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->rider_ref)));  // 0x0041b3c0
        joan::orig::as_unit(u)->set_action(joan::orig::arg::i32(message), std::int32_t{0}, std::int32_t{1});  // 0x00501190
    }
    void unit_event(const std::int16_t* event) {
        c_unit_event.hit();
        if (sx16(*event) == 0xa) notify_linked(0x6a);
        if (sx16(*event) == 0xb) notify_linked(0x69);
        if (sx16(*event) == 0xd || sx16(*event) == 0xc) notify_linked(0x69);
        if (sx16(*event) == 0x1) notify_linked(0x68);
    }
};

#define B89(name, addr, fn) JOAN_REPLACE("game/b89_04", name, addr, fn)
B89("Unit::on_event", 0x00459970, &H::unit_event);
#undef B89

}  // namespace game_b89_04
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

// 0x00459b20 lerp(a, b, t): b > a ? (b - a) * t + a : (1 - t) * (a - b) + b (float ops, PC24).
float JOAN_CDECL lerp(float a, float b, float t) {
    if (b > a) return (b - a) * t + a;
    return (global<float>(d::flt_1) - t) * (a - b) + b;  // 1.0f
}

struct H {
    // 0x00459b70 Pair::release: +0 / +4 deleted (0x00459c40) when +8 is set.
    void* pair_release() {
        if (jo::self<jo::CloudLayers>(this)->built == 0) return this;
        if (jo::view<void*>(jo::self<jo::CloudLayers>(this)->layer1) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::CloudLayers>(this)->layer1);
            if (p != nullptr) joan::orig::as_plane(p)->destroy(1u);  // 0x00459c40 delete
            jo::view<void*>(jo::self<jo::CloudLayers>(this)->layer1) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::CloudLayers>(this)->layer2) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::CloudLayers>(this)->layer2);
            if (p != nullptr) joan::orig::as_plane(p)->destroy(1u);
            jo::view<void*>(jo::self<jo::CloudLayers>(this)->layer2) = nullptr;
        }
        jo::self<jo::CloudLayers>(this)->built = 0;
        return this;
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("lerp", 0x00459b20, &lerp);
L82("Pair::release", 0x00459b70, &H::pair_release);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
namespace {
namespace game_leaves80a {

struct H {
    void* scalar_deleting_dtor(std::uint32_t flags) {  // 0x00459c40
        joan::orig::as_plane(this)->dtor();  // 0x005cc750
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Object::scalar_deleting_dtor", 0x00459c40, &H::scalar_deleting_dtor);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

struct H {
    u8* self() { return static_cast<u8*>(static_cast<void*>(this)); }

    // ------------------------------------------------------------------ 0x00459c90
    std::uintptr_t apply_9(void* m) {
        void* const r = joan::orig::arg::as<void*>(joan::orig::as_camera(m)->matrix());                          // 0x00616e90
        return joan::orig::arg::as<u32>(joan::orig::matrix_position(static_cast<void*>(self() + offsetof(jo::CloudLayers, built) + 1), r));  // 0x0060c310 (the camera position after the built flag: +9 on x86, in the image tail)
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Obj::apply_9", 0x00459c90, &H::apply_9);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx2)
namespace {
namespace game_fx {

struct V3 {
    float x, y, z;
};
void store3(void* p, const V3& v) { std::memcpy(p, &v, sizeof v); }

constexpr fn::Code kBoneUpdate = fn::Bone_invalidate;  // 0x0060ee10
constexpr fn::Code kCameraBone = fn::View3D_get_frame;  // 0x00613de0
constexpr fn::Code kPlaneDraw = fn::Plane_render;   // 0x005d0570

// 0x0060c310 matrix_position(out, matrix) of the camera's bone.
V3 camera_position(void* ctx) {
    V3 p;
    void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(thiscall<void*>(kCameraBone, ctx))->matrix());
    joan::orig::matrix_position(static_cast<void*>(&p), joan::orig::arg::ptr(m));
    return p;
}

JOAN_COUNTER(c_follow, "FollowPlanes::draw");

struct H {
    // 0x00459cd0 FollowPlanes::draw(ctx, pass): the two planes (+0, +4) follow the camera.
    void follow_draw(void* ctx, char pass) {
        void* const camera_kind = jo::as_camera(ctx)->scene->sky->materials;
        if (jo::as_material(camera_kind)->texture_mode != 0x10) return;
        if (static_cast<std::int32_t>(pass) != 1) return;
        c_follow.hit();
        for (std::int32_t k = 0; k < 2; ++k) {
            void* const plane = (&jo::self<jo::CloudLayers>(this)->layer1)[k];  // layer1, layer2
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->visible()) == 0) continue;  // 0x005d0480
            const V3 cam = camera_position(ctx);
            void* const bone = *static_cast<void**>(plane);
            store3(&jo::as_bone(bone)->local[12], cam);
            thiscall(kBoneUpdate, *static_cast<void**>(plane));
            thiscall(kPlaneDraw, (&jo::self<jo::CloudLayers>(this)->layer1)[k], ctx, 2, 0);
        }
    }
};

#define FX2(name, addr, fn) JOAN_REPLACE("game/fx2", name, addr, fn)
FX2("FollowPlanes::draw", 0x00459cd0, &H::follow_draw);
#undef FX2

}  // namespace game_fx
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::CloudLayers::build(const char* a0, const char* a1) -> void { joan::bind::member<&::game_misc81::H::build>(this, a0, a1); }  // 0x458720
auto joan::orig::Unit::on_event(void* a0) -> void { joan::bind::member<&::game_b89_04::H::unit_event>(this, a0); }  // 0x459970
auto joan::orig::lerp(float a0, float a1, float a2) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_leaves82b::lerp>(a0, a1, a2)); }  // 0x459b20
auto joan::orig::CloudLayers::release() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b::H::pair_release>(this)); }  // 0x459b70
auto joan::orig::Plane::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves80a::H::scalar_deleting_dtor>(this, a0)); }  // 0x459c40
auto joan::orig::CloudLayers::update_position(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_01::H::apply_9>(this, a0)); }  // 0x459c90
auto joan::orig::CloudLayers::draw_follow(void* a0, char a1) -> void { joan::bind::member<&::game_fx::H::follow_draw>(this, a0, a1); }  // 0x459cd0
