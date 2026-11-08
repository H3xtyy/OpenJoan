// game/unknown_billboard.cpp: functions whose original source file is unknown (formerly game/_unplaced/billboard.cpp;
// re/attribution.csv: between ww/en2d/widget2d.cpp and ww/uti3d/blkarrb.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>
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
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;

using joan::img::obj;

bool below_or_nan(float a, float b) { return !(a >= b); }

using joan::hook::global;

using joan::call::import_call;

struct V3 {
    float x, y, z;
};

// The camera-follow factors f32[3] at +0x23 on x86: anchored on the typed gap_23 so they move with the pointers of
// the layout (correct on x64; a named member would only rename the gap).
constexpr std::int32_t kFollow = static_cast<std::int32_t>(offsetof(jo::Billboard, gap_23));

constexpr fn::Code kCameraBone = fn::View3D_get_frame;  // 0x00613de0

// 0x0060c310 matrix_position(out, matrix) of the camera's bone.
V3 camera_position(void* ctx) {
    V3 p;
    void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(thiscall<void*>(kCameraBone, ctx))->matrix());
    joan::orig::matrix_position(static_cast<void*>(&p), joan::orig::arg::ptr(m));
    return p;
}

using u8 = std::uint8_t;


using i16 = std::int16_t;

}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

struct H {
    void billboard_set_pos(std::uint32_t x, std::uint32_t y, std::uint32_t z) {        // 0x00635d40
        jo::view<std::uint32_t>(jo::self<jo::Billboard>(this)->pos[0]) = x;
        jo::view<std::uint32_t>(jo::self<jo::Billboard>(this)->pos[1]) = y;
        jo::view<std::uint32_t>(jo::self<jo::Billboard>(this)->pos[2]) = z;
    }
    H* billboard_set_15(std::uint32_t v) { jo::self<jo::Billboard>(this)->u32_15 = v; return this; }   // 0x00635d80 (eax = this)
    void billboard_set_19(std::uint32_t v) { jo::self<jo::Billboard>(this)->u32_19 = v; }               // 0x00635db0
    H* billboard_ctor() {                                                                // 0x006351a0
        jo::view<std::uintptr_t>(jo::self<jo::Billboard>(this)->vtable) = img::addr(d::vt_Obj_9da81c);  // vftable
        std::memset(&jo::self<jo::Billboard>(this)->owned, 0, sizeof(jo::Billboard) - offsetof(jo::Billboard, owned));  // 0x33 bytes on x86
        return this;
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("Billboard::set_position", 0x00635d40, &H::billboard_set_pos);
W1("Billboard::set_15", 0x00635d80, &H::billboard_set_15);
W1("Billboard::set_19", 0x00635db0, &H::billboard_set_19);
W1("Billboard::Billboard", 0x006351a0, &H::billboard_ctor);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

using joan::orig::helpers::op_delete;

struct H {
    H* sdtor_6351e0(u32 flags) {
        joan::orig::as_billboard(this)->destruct();  // 0x00635230
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Obj::sdtor_6351e0", 0x006351e0, &H::sdtor_6351e0);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_b93_04.cpp (game/b93_04)
namespace {
namespace game_b93_04 {

struct H {
    // ------------------------------------------------------------------ 0x00635230
    std::intptr_t obja_dtor() {
        jo::view<std::uintptr_t>(jo::self<jo::Billboard>(this)->vtable) = img::addr(d::vt_Obj_9da81c);
        return joan::orig::arg::as<i32>(joan::orig::as_billboard(this)->release(0u));  // 0x006356c0
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("ObjA::~ObjA", 0x00635230, &H::obja_dtor);
#undef B93

}  // namespace game_b93_04
}  // namespace

// ---------------------------------------- from modules/game_overlays.cpp (game/units3)
namespace {
namespace game_overlays {

using joan::orig::helpers::at;

constexpr img::Addr kTextures(d::g_texture_array);

using joan::orig::helpers::operator_new;

JOAN_COUNTER(c_billboard_create, "Billboard::create");
JOAN_COUNTER(c_text_mesh, "Billboard::text");

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // 0x00636040 Billboard::text(font by value, ...): the text mesh (0x00625c50) on this. The font is a bitwise
    // Font2d copy the callee destroys (0x40 bytes on x86; x64 passes it by reference to the caller's copy).
    std::int32_t text(jo::Font2d font, std::int16_t a, std::int16_t b, const char* str, std::uint32_t c, std::int16_t d, std::int16_t e, std::int16_t g) {
        c_text_mesh.hit();
        const std::int32_t r = joan::orig::arg::as<std::int16_t>(joan::orig::as_font(&font)->draw_glyphs(joan::orig::arg::i16(static_cast<std::int32_t>(a)), joan::orig::arg::i16(static_cast<std::int32_t>(b)), joan::orig::arg::str(str), joan::orig::arg::mptr(static_cast<void*>(this)), joan::orig::arg::u32(c), joan::orig::arg::i16(static_cast<std::int32_t>(d)), joan::orig::arg::i16(static_cast<std::int32_t>(e)), joan::orig::arg::u32(static_cast<std::int32_t>(g)), 0u, 1.0f, 1.0f));
        joan::orig::as_font(&font)->dtor();  // 0x00623e70 ~Font
        return r;
    }
    // 0x00636aa0 Billboard::normalise: vertices (y, z) moved into [-0.5, 0.5] (range counted
    // from 0), +0x2f / +0x33 size.
    void normalise() {
        float min_y = 0.0f, min_z = 0.0f, max_y = 0.0f, max_z = 0.0f;
        if (jo::self<jo::Billboard>(this)->plane_ == nullptr) return;
        std::uint8_t* const plane = jo::view<std::uint8_t*>(jo::self<jo::Billboard>(this)->plane_);
        for (std::int32_t i = 0; i < jo::as_plane(plane)->vertex_count; ++i) {
            const std::uint8_t* const v = jo::view<std::uint8_t*>(jo::as_plane(plane)->vertices_f4) + i * 0x18;
            if (jo::as_vertex_f4(v)->pos[1] > max_y) max_y = jo::as_vertex_f4(v)->pos[1];
            if (below_or_nan(jo::as_vertex_f4(v)->pos[1], min_y)) min_y = jo::as_vertex_f4(v)->pos[1];
            if (jo::as_vertex_f4(v)->pos[2] > max_z) max_z = jo::as_vertex_f4(v)->pos[2];
            if (below_or_nan(jo::as_vertex_f4(v)->pos[2], min_z)) min_z = jo::as_vertex_f4(v)->pos[2];
        }
        const float w = (max_y - min_y) + 1.0f;
        const float h = (max_z - min_z) + 1.0f;
        jo::self<jo::Billboard>(this)->width = w;
        jo::self<jo::Billboard>(this)->height = h;
        for (std::int32_t i = 0; i < jo::as_plane(jo::self<jo::Billboard>(this)->plane_)->vertex_count; ++i) {
            std::uint8_t* const v = jo::view<std::uint8_t*>(jo::as_plane(jo::self<jo::Billboard>(this)->plane_)->vertices_f4) + i * 0x18;
            jo::as_vertex_f4(v)->pos[1] = jo::as_vertex_f4(v)->pos[1] - min_y;
            jo::as_vertex_f4(v)->pos[1] = jo::as_vertex_f4(v)->pos[1] / w - 0.5f;
            jo::as_vertex_f4(v)->pos[2] = jo::as_vertex_f4(v)->pos[2] - min_z;
            jo::as_vertex_f4(v)->pos[2] = jo::as_vertex_f4(v)->pos[2] / h - 0.5f;
        }
    }

    // 0x00635260 Billboard::create(x, y, z, scale x, scale y, texture name, kind, colour,
    // +0x1f.., blend modes): a 4-vertex quad plane (+5) facing x.
    H* create(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t sx, std::uint32_t sy, const char* texture, std::int16_t kind,
                std::uint32_t argb, std::uint32_t p28, std::uint32_t p2c, std::uint32_t p30, std::uint32_t p34, std::uint32_t m38,
                std::uint32_t m3c, std::uint32_t m40) {
        if (jo::self<jo::Billboard>(this)->owned != 0) return this;  // eax = this on both exits
        c_billboard_create.hit();
        jo::view<std::uint32_t>(jo::self<jo::Billboard>(this)->pos[0]) = x;
        jo::view<std::uint32_t>(jo::self<jo::Billboard>(this)->pos[1]) = y;
        jo::view<std::uint32_t>(jo::self<jo::Billboard>(this)->pos[2]) = z;
        jo::self<jo::Billboard>(this)->u32_15 = sx;
        jo::self<jo::Billboard>(this)->u32_19 = sy;
        jo::self<jo::Billboard>(this)->i16_1d = kind;
        jo::view<std::uint32_t>(jo::self<jo::Billboard>(this)->offset) = p28;
        // the three follow factors are float bits passed as u32; memcpy keeps the bits (a float store could quiet a NaN)
        std::memcpy(&f<float>(kFollow), &p2c, sizeof(float));  // TODO(re): 0x00635260 the follow factors overlap Billboard::items (+0x24): another class of the same base?
        std::memcpy(&f<float>(kFollow + 4), &p30, sizeof(float));
        std::memcpy(&f<float>(kFollow + 8), &p34, sizeof(float));
        void* tex = nullptr;
        if (texture != nullptr) tex = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(texture), 1, 0u));  // 0x006099c0
        void* const mem = operator_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
        jo::self<jo::Billboard>(this)->plane_ = static_cast<jo::Plane*>(mem != nullptr ? joan::orig::as_plane(mem)->construct(4u, 1u) : nullptr);  // 0x005cbaf0 Plane::Plane
        if (jo::self<jo::Billboard>(this)->plane_ == nullptr) joan::orig::as_error_log(img::obj(d::g_err))->mem_error();  // 0x00586af0 out of memory
        joan::orig::as_plane(jo::self<jo::Billboard>(this)->plane_)->init(4, 2, 1, 1u);  // 0x005cc7a0 Plane::init
        jo::view<std::uint32_t>(jo::as_plane(jo::self<jo::Billboard>(this)->plane_)->list_kind) = 5;
        joan::orig::as_material(jo::view<void*>(jo::as_plane(jo::self<jo::Billboard>(this)->plane_)->materials))->set_texture(0, joan::orig::arg::mptr(tex));  // 0x005d5650 Material::set_texture
        std::uint8_t* const material = jo::view<std::uint8_t*>(jo::as_plane(jo::self<jo::Billboard>(this)->plane_)->materials);
        jo::as_material(material)->blend = m38;
        jo::as_material(material)->texture_mode = m3c;
        jo::as_material(material)->pass_mask = m40;
        const std::uint8_t* const c = reinterpret_cast<const std::uint8_t*>(&argb);
        const std::uint8_t col[4] = {c[2], c[1], c[0], c[3]};
        struct Vertex {
            float x, y, z;
            std::uint8_t c[4];
            float u, v;
        };
        Vertex q[4] = {{0.0f, -0.5f, 0.5f, {}, 0.0f, 0.0f},
                       {0.0f, -0.5f, -0.5f, {}, 0.0f, 1.0f},
                       {0.0f, 0.5f, 0.5f, {}, 1.0f, 0.0f},
                       {0.0f, 0.5f, -0.5f, {}, 1.0f, 1.0f}};
        for (Vertex& v : q) std::memcpy(v.c, col, 4);
        if (jo::self<jo::Billboard>(this)->i16_1d == 1) {  // lying flat
            q[0].x = -0.5f;
            q[0].y = -0.5f;
            q[1].x = 0.5f;
            q[1].y = -0.5f;
            q[2].x = -0.5f;
            q[2].y = 0.5f;
            q[3].x = 0.5f;
            q[3].y = 0.5f;
            q[3].z = 0.0f;
            q[2].z = q[3].z;
            q[1].z = q[2].z;
            q[0].z = q[1].z;
        }
        const std::uint16_t index[6] = {0, 1, 2, 2, 1, 3};
        std::memmove(jo::view<void*>(jo::as_plane(jo::self<jo::Billboard>(this)->plane_)->vertices_f4), q, sizeof q);
        std::memmove(jo::view<void*>(jo::as_plane(jo::self<jo::Billboard>(this)->plane_)->triangles), index, sizeof index);
        std::memset(jo::view<void*>(jo::as_plane(jo::self<jo::Billboard>(this)->plane_)->triangle_materials), 0, 4);
        joan::orig::as_plane(jo::self<jo::Billboard>(this)->plane_)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920 Plane::invalidate
        if (tex != nullptr) joan::orig::texture_ref::release(joan::orig::arg::mptr(tex));  // 0x0042cef0 release
        jo::self<jo::Billboard>(this)->owned = 1;
        return this;
    }
};

#define U3(name, addr, fn) JOAN_REPLACE("game/units3", name, addr, fn)
U3("Billboard::text", 0x00636040, &H::text);
U3("Billboard::normalise", 0x00636aa0, &H::normalise);
U3("Billboard::create", 0x00635260, &H::create);
#undef U3

}  // namespace game_overlays
}  // namespace

// ---------------------------------------- from modules/game_units4.cpp (game/units4)
namespace {
namespace game_units4 {

struct H {
    // 0x006356c0 Holder::release(keep): unless `keep`, releases and deletes the object +5;
    // clears +5 and the flag +4. Returns this, or 0 when the flag +4 was already clear.
    void* holder_release(std::uint32_t keep) {
        if (jo::self<jo::Billboard>(this)->owned == 0) return nullptr;  // eax = zero-extended flag byte = 0
        if ((keep & 0xff) == 0 && jo::self<jo::Billboard>(this)->plane_ != nullptr) {
            joan::orig::as_plane(jo::self<jo::Billboard>(this)->plane_)->release();  // 0x005ccaa0
            void* const o = jo::self<jo::Billboard>(this)->plane_;
            if (o != nullptr) joan::orig::as_plane(o)->destroy(1u);  // 0x00459c40
        }
        jo::self<jo::Billboard>(this)->plane_ = nullptr;
        jo::self<jo::Billboard>(this)->owned = 0;
        return this;
    }
};

#define U4(name, addr, fn) JOAN_REPLACE("game/units4", name, addr, fn)
U4("Holder::release", 0x006356c0, &H::holder_release);
#undef U4

}  // namespace game_units4
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

using joan::orig::helpers::at;

V3 load3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

bool equal_or_nan(float a, float b) { return !(a < b || a > b); }

constexpr fn::Code kPlaneBone = fn::Plane_bone_5cf8d0;   // 0x005cf8d0
constexpr fn::Code kBoneUpdate = fn::Bone_invalidate;  // 0x0060ee10

constexpr img::Addr kIdentity(d::g_identity_matrix);

JOAN_COUNTER(c_billboard, "Billboard::place");

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    void* plane5() { return jo::self<jo::Billboard>(this)->plane_; }                        // 0x00635e80

    // 0x00635760 Billboard::place(ctx): plane (+5) at +9, pulled toward the camera by +0x1f,
    // offset by camera-position * (+0x23, +0x27, +0x2b); scale +0x15 / +0x19 by kind (+0x1d);
    // kind 3 turns toward the camera, kinds 0..2 take the camera rotation.
    void billboard_place(void* ctx) {
        c_billboard.hit();
        void* const m = thiscall<void*>(kPlaneBone, jo::self<jo::Billboard>(this)->plane_);
        V3 pos = load3(jo::self<jo::Billboard>(this)->pos);
        V3 dir{global<float>(d::g_pos), global<float>(d::g_a69cdc), global<float>(d::g_a69ce0)};
        if (!equal_or_nan(jo::self<jo::Billboard>(this)->offset, 0.0f)) {
            const V3 cam = camera_position(ctx);
            dir = V3{cam.x - jo::self<jo::Billboard>(this)->pos[0], cam.y - jo::self<jo::Billboard>(this)->pos[1], cam.z - jo::self<jo::Billboard>(this)->pos[2]};
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, &dir);  // vector_normalize
            pos.x = dir.x * jo::self<jo::Billboard>(this)->offset + pos.x;
            pos.y = dir.y * jo::self<jo::Billboard>(this)->offset + pos.y;
            pos.z = dir.z * jo::self<jo::Billboard>(this)->offset + pos.z;
        }
        // the camera factors f32[3] (gap_23, see kFollow)
        if (!equal_or_nan(f<float>(kFollow), 0.0f) || !equal_or_nan(f<float>(kFollow + 4), 0.0f) || !equal_or_nan(f<float>(kFollow + 8), 0.0f)) {
            const V3 cam = camera_position(ctx);
            pos.x = cam.x * f<float>(kFollow) + pos.x;
            pos.y = cam.y * f<float>(kFollow + 4) + pos.y;
            pos.z = cam.z * f<float>(kFollow + 8) + pos.z;
        }
        std::memcpy(m, img::obj(kIdentity), 0x40);
        const std::int32_t kind = jo::self<jo::Billboard>(this)->i16_1d;
        if (static_cast<std::uint32_t>(kind) <= 3) {
            if (kind == 1) {
                jo::view<std::uint32_t>(jo::as_matrix44(m)->m[0]) = jo::self<jo::Billboard>(this)->u32_19;
                jo::view<std::uint32_t>(jo::as_matrix44(m)->m[5]) = jo::self<jo::Billboard>(this)->u32_15;
            } else {
                jo::view<std::uint32_t>(jo::as_matrix44(m)->m[5]) = jo::self<jo::Billboard>(this)->u32_15;
                jo::view<std::uint32_t>(jo::as_matrix44(m)->m[10]) = jo::self<jo::Billboard>(this)->u32_19;
            }
        }
        float r[16];
        std::memcpy(r, img::obj(kIdentity), sizeof r);
        if (jo::self<jo::Billboard>(this)->i16_1d == 3) {
            const V3 cam = camera_position(ctx);
            V3 d{cam.x - pos.x, cam.y - pos.y, cam.z - pos.z};
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, &d);  // vector_normalize
            const float h = joan::orig::arg::as<float>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(d.x * d.x + d.y * d.y)));  // 0x004256d0 sqrt
            float pitch = joan::orig::arg::as<float>(joan::orig::libc::acos_f(joan::orig::arg::f32(h)));                        // 0x00635e50
            if (below_or_nan(pitch, 0.0f)) pitch = -pitch;
            pitch = 0.0f;  // (the original overwrites it)
            const float sy = d.y / h;
            const float sx = d.x / h;
            float yaw = joan::orig::arg::as<float>(joan::orig::libc::acos_f(joan::orig::arg::f32(sx)));
            if (below_or_nan(sy, 0.0f)) yaw = -yaw;
            joan::orig::matrix_rotation_60bc00(joan::orig::arg::mptr(static_cast<void*>(r)), joan::orig::arg::f32(pitch));  // 0x0060bc00
            float yaw_m[16];
            joan::orig::matrix_rotation(joan::orig::arg::mptr(static_cast<void*>(yaw_m)), joan::orig::arg::f32(yaw));  // 0x0060bc50
            import_call<void*>(d::iat_v3d_87_matrix_multiply_2, static_cast<void*>(r), static_cast<void*>(yaw_m));  // matrix_multiply
        } else {
            float cam_m[16];
            std::memcpy(cam_m, img::obj(kIdentity), sizeof cam_m);
            const std::int32_t k = jo::self<jo::Billboard>(this)->i16_1d;
            if (k == 0) {
                std::memcpy(cam_m, joan::orig::arg::as<void*>(joan::orig::as_bone(thiscall<void*>(kCameraBone, ctx))->matrix()), sizeof cam_m);
            } else if (k > 0 && k <= 2) {
                std::uint32_t frame[10];
                joan::orig::as_camera(ctx)->frame(joan::orig::arg::mptr(static_cast<void*>(frame)));  // 0x006140a0
                joan::orig::orbit_matrix(joan::orig::arg::mptr(static_cast<void*>(cam_m)), joan::orig::arg::f32(frame[4]), joan::orig::arg::f32(0u), joan::orig::arg::f32(frame[6]), joan::orig::arg::f32(0u));  // 0x0060bd00
            }
            r[0] = -cam_m[8];
            r[1] = -cam_m[9];
            r[2] = -cam_m[10];
            std::memcpy(&r[4], &cam_m[0], 12);
            r[8] = -cam_m[4];
            r[9] = -cam_m[5];
            r[10] = -cam_m[6];
        }
        import_call<void*>(d::iat_v3d_87_matrix_multiply_2, m, static_cast<void*>(r));                 // matrix_multiply
        import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, m, static_cast<void*>(&pos));              // matrix_multiply_translation
        thiscall(kBoneUpdate, thiscall<void*>(kPlaneBone, jo::self<jo::Billboard>(this)->plane_));
    }
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
#define FX2(name, addr, fn) JOAN_REPLACE("game/fx2", name, addr, fn)
FX1("Billboard::plane", 0x00635e80, &H::plane5);
FX2("Billboard::place", 0x00635760, &H::billboard_place);
#undef FX1
#undef FX2

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

JOAN_COUNTER(c_set_colors, "Obj::set_colors");

struct H {
    // ------------------------------------------------------------------ 0x00635de0
    u32 set_colors(u32 color) {
        c_set_colors.hit();
        for (i32 i = 0; i < 4; ++i) {
            u8* const e = reinterpret_cast<u8*>(jo::as_plane(jo::self<jo::Billboard>(this)->plane_)->vertices_f4[i].attr);
            joan::orig::as_colour(e)->from_rgba(&color);  // 0x005e95b0
        }
        joan::orig::as_plane(jo::self<jo::Billboard>(this)->plane_)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        return 0;  // 0x005cf920 returns nothing; the only caller (camcmd.cpp) discards the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Obj::set_colors", 0x00635de0, &H::set_colors);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_fx_2.cpp (game/fx2)
namespace {
namespace game_fx_2 {

using joan::orig::helpers::at;

void store3(void* p, const V3& v) { std::memcpy(p, &v, sizeof v); }

JOAN_COUNTER(c_billboard_centre, "Billboard::centre");

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // 0x00635ea0 Billboard::centre(out, ctx): the point +0x1f from +9 toward the camera plus
    // camera-position * (+0x23, +0x27, +0x2b).
    void* billboard_centre(void* out, void* ctx) {
        c_billboard_centre.hit();
        V3 cam = camera_position(ctx);
        V3 d{cam.x - jo::self<jo::Billboard>(this)->pos[0], cam.y - jo::self<jo::Billboard>(this)->pos[1], cam.z - jo::self<jo::Billboard>(this)->pos[2]};
        const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &d));  // length
        joan::orig::vec_scale_in_place(static_cast<void*>(&d), joan::orig::arg::f32(jo::self<jo::Billboard>(this)->offset / len));  // 0x0060ce80
        cam.x = cam.x * f<float>(kFollow);
        cam.y = cam.y * f<float>(kFollow + 4);
        cam.z = cam.z * f<float>(kFollow + 8);
        store3(out, V3{cam.x + jo::self<jo::Billboard>(this)->pos[0] + d.x, cam.y + jo::self<jo::Billboard>(this)->pos[1] + d.y, cam.z + jo::self<jo::Billboard>(this)->pos[2] + d.z});
        return out;
    }
};

#define FX2(name, addr, fn) JOAN_REPLACE("game/fx2", name, addr, fn)
FX2("Billboard::centre", 0x00635ea0, &H::billboard_centre);
#undef FX2

}  // namespace game_fx_2
}  // namespace

// ---------------------------------------- from modules/game_b105_17.cpp (game/b105_17)
namespace {
namespace game_b105_17 {

struct H {
    float get_15() { return jo::view<float>(jo::self<jo::Billboard>(this)->u32_15); }
};

JOAN_REPLACE("game/b105_17", "Obj::get_15", 0x00635fc0, &H::get_15);

}  // namespace game_b105_17
}  // namespace

// ---------------------------------------- from modules/game_b105_18.cpp (game/b105_18)
namespace {
namespace game_b105_18 {

using i8 = std::int8_t;

using joan::orig::helpers::at;

i32 sx(u32 raw) { return static_cast<i32>(static_cast<i16>(raw)); }  // movsx word

JOAN_COUNTER(c_draw_image, "DlgRes::draw_image");

struct H {
    // -------------------------------------------------------------------------- tiny slots
    float get_f19() { return jo::view<float>(jo::self<jo::Billboard>(this)->u32_19); }                     // 0x00635fe0 (dead)

    // ------------------------------------------------------------------------ 0x00636c90
    u32 draw_image(u32 x, u32 y, u32 key, void* name, u32, u32, u32 a20, u32, u32, u32 a2c,
                   u32 a30, u32 a34, u32 a38, u32 a3c, u32, u32) {
        c_draw_image.hit();
        if (key == 0 || static_cast<i8>(jo::as_bmp_res(name)->loaded) == 0) return 0;
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(name)->find(joan::orig::arg::str(key)));  // 0x00620b40
        if (rec == nullptr) return 0;
        u8* const img = joan::orig::arg::as<u8*>(joan::orig::as_textures(obj(d::g_texture_array))->find(joan::orig::arg::str(jo::as_bmp_entry(rec)->file)));  // 0x0060a4b0
        if (img == nullptr || joan::orig::arg::as<i32>(joan::orig::as_texture(img)->ready()) == 0) return 0;  // 0x00605b70
        jo::BmpEntry* const e = jo::as_bmp_entry(rec);
        const i16 v10 = static_cast<i16>(jo::view<i16>(e->x0) == -1 ? 0 : jo::view<i16>(e->x0));
        const i16 v14 = static_cast<i16>(jo::view<i16>(e->y0) == -1 ? 0 : jo::view<i16>(e->y0));
        const i16 v18 = static_cast<i16>(jo::view<i16>(e->x1) == -1 ? jo::view<i32>(jo::as_texture(img)->width) - 1
                                                                  : static_cast<i32>(jo::view<i16>(e->x1)));
        const i16 v1c = static_cast<i16>(jo::view<i16>(e->y1) == -1 ? jo::view<i32>(jo::as_texture(img)->height) - 1
                                                                  : static_cast<i32>(jo::view<i16>(e->y1)));
        const i16 v20 = static_cast<i16>(sx(a2c) == -1 ? v10 : v10 + sx(a2c));
        const i16 v24 = static_cast<i16>(sx(a30) == -1 ? v14 : v14 + sx(a30));
        const i16 v28 = static_cast<i16>(sx(a34) == -1 ? v18 : v10 + sx(a34));
        const i16 v2c = static_cast<i16>(sx(a38) == -1 ? v1c : v14 + sx(a38));
        const i32 frame = static_cast<i32>(jo::view<i16>(e->u16_32)) + sx(a3c);
        const bool odd = (frame % 2) != 0;  // and 0x80000001 / jns fix-up = signed remainder
        const i16 v30 = static_cast<i16>(!odd ? v28 - v20 + 1 : v2c - v24 + 1);
        const i16 v34 = static_cast<i16>(!odd ? v2c - v24 + 1 : v28 - v20 + 1);
        u32 rgba_obj = 0;
        const u32 rgba = *joan::orig::arg::as<u32*>(joan::orig::as_color(&rgba_obj)->Color_dword(0xffffffffu));  // 0x005e9640
        return joan::orig::arg::as<u32>(joan::orig::as_billboard(this)->quad(joan::orig::arg::u32(static_cast<u32>(sx(x))), joan::orig::arg::u32(static_cast<u32>(sx(y))), joan::orig::arg::u32(// 0x00636ff0
                             static_cast<u32>(sx(x) + v30 - 1)), joan::orig::arg::u32(static_cast<u32>(sx(y) + v34 - 1)), joan::orig::arg::u32(rgba), joan::orig::arg::mptr(static_cast<void*>(img)), joan::orig::arg::u32(static_cast<u32>(static_cast<i32>(v10))), joan::orig::arg::u32(static_cast<u32>(static_cast<i32>(v14))), joan::orig::arg::u32(static_cast<u32>(static_cast<i32>(v18))), joan::orig::arg::u32(static_cast<u32>(static_cast<i32>(v1c))), joan::orig::arg::u32(static_cast<u32>(frame)), joan::orig::arg::u32(static_cast<u32>(sx(a20)))));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_18", name, addr, fn)
B("Obj::get_f19", 0x00635fe0, &H::get_f19);
B("DlgRes::draw_image", 0x00636c90, &H::draw_image);
#undef B

}  // namespace game_b105_18
}  // namespace

// ---------------------------------------- from modules/game_b103_13.cpp (game/b103_13)
namespace {
namespace game_b103_13 {

struct H {
    std::uintptr_t colour_636000(void* out) {  // 0x00636000
        return joan::orig::arg::as<u32>(joan::orig::as_color(out)->set_bgr(joan::orig::arg::mptr(jo::as_plane(jo::self<jo::Billboard>(this)->plane_)->vertices_f4->attr)));  // 0x005e9430
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_13", name, addr, fn)
B("Res::colour_636000", 0x00636000, &H::colour_636000);
#undef B

}  // namespace game_b103_13
}  // namespace

// ---------------------------------------- from modules/game_ui2.cpp (game/ui2)
namespace {
namespace game_ui2 {

JOAN_COUNTER(c_blit, "Billboard::blit");
JOAN_COUNTER(c_blit_more, "Billboard::blit_more");

// fild (short) ; fidiv dword [texture + off] at PC24 ; fstp dword.
float tex_coord(std::int32_t v, const std::int32_t& size) {  // size: Texture::width / height
    return static_cast<float>(joan::hook::x87::Fpu{true}.div(static_cast<double>(v), static_cast<double>(size)));
}

// Shared body of 0x006360f0 / 0x00636640 after the material: the stack frame's position
// array (ebp-0x80, 4 x {0, x, y}) directly followed by the uv array (ebp-0x50, 4 x {u, v});
// the uv index (rot + i) % 4 can be negative (C remainder), which reads back into the
// position array exactly like the original.
struct Quad {
    std::uint32_t w[20];
};
Quad make_quad(std::int16_t x1, std::int16_t y1, std::int16_t x2, std::int16_t y2, const void* texture, std::int16_t sx1,
               std::int16_t sy1, std::int16_t sx2, std::int16_t sy2) {
    const float zero = 0.0f;
    const float half = global<float>(d::flt_0_5);   // 0.5
    const float minus = global<float>(d::flt_m1);  // -1.0
    const float fx1 = static_cast<float>(static_cast<float>(static_cast<std::int32_t>(x1)) + zero) - half;
    const float fx2 = static_cast<float>(static_cast<float>(static_cast<std::int32_t>(x2)) + zero) + half;
    const float fy1 = static_cast<float>(static_cast<float>(static_cast<std::int32_t>(y1)) * minus) + half;
    const float fy2 = static_cast<float>(static_cast<float>(static_cast<std::int32_t>(y2)) * minus) - half;
    float u1 = 0.0f, v1 = 0.0f, u2 = 0.0f, v2 = 0.0f;
    if (texture != nullptr) {
        u1 = static_cast<std::int32_t>(sx1) == -1 ? 0.0f : tex_coord(sx1, jo::view<std::int32_t>(jo::as_texture(texture)->width));
        v1 = static_cast<std::int32_t>(sy1) == -1 ? 0.0f : tex_coord(sy1, jo::view<std::int32_t>(jo::as_texture(texture)->height));
        u2 = static_cast<std::int32_t>(sx2) == -1 ? 1.0f : tex_coord(static_cast<std::int32_t>(sx2) + 1, jo::view<std::int32_t>(jo::as_texture(texture)->width));
        v2 = static_cast<std::int32_t>(sy2) == -1 ? 1.0f : tex_coord(static_cast<std::int32_t>(sy2) + 1, jo::view<std::int32_t>(jo::as_texture(texture)->height));
    }
    Quad q{};
    const float vals[20] = {0.0f, fx1, fy1, 0.0f, fx2, fy1, 0.0f, fx2, fy2, 0.0f, fx1, fy2,
                            u1,   v1,  u2,  v1,   u2,  v2,  u1,   v2};
    std::memcpy(q.w, vals, sizeof q.w);
    return q;
}

void* billboard_plane(const void* self) { return joan::orig::arg::as<void*>(joan::orig::as_billboard(self)->plane()); }  // 0x00635e80
bool texture_ready(void* texture) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_texture(texture)->ready()) != 0; }  // 0x00605b70

// 4 vertices (0x18 bytes: position, packed colour +0xc, uv +0x10) and 2 triangles.
std::int32_t add_quad(void* plane, const Quad& q, std::uint32_t* colour, std::int16_t rot, std::int32_t material) {
    const std::int32_t vb = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_vertices(4));  // 0x005cd7f0
    jo::VertexF4* v = &jo::as_plane(plane)->vertices_f4[vb];
    for (std::int32_t i = 0; i < 4; ++i, ++v) {
        std::memcpy(v->pos, &q.w[i * 3], 12);
        joan::orig::as_colour(&v->attr[0])->from_rgba(joan::orig::arg::mptr(colour));  // 0x005e95b0
        const std::int32_t k = (static_cast<std::int32_t>(rot) + i) % 4;
        jo::view<std::uint32_t>(v->attr[1]) = q.w[12 + k * 2];
        jo::view<std::uint32_t>(v->attr[2]) = q.w[13 + k * 2];
    }
    const std::int32_t tri = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_triangles(2));                 // 0x005ce6f0
    joan::orig::as_plane(plane)->set_triangle(joan::orig::arg::i32(tri), joan::orig::arg::u32(material), joan::orig::arg::u32(vb), joan::orig::arg::u32(vb + 3), joan::orig::arg::u32(vb + 1));          // 0x005ce800
    joan::orig::as_plane(plane)->set_triangle(joan::orig::arg::i32(tri + 1), joan::orig::arg::u32(material), joan::orig::arg::u32(vb + 1), joan::orig::arg::u32(vb + 3), joan::orig::arg::u32(vb + 2));  // 0x005ce800
    return 0;  // 0x005ce800 returns nothing; blit ignores the result, blit_more's only caller discards it
}

jo::Material* material_at(void* plane, std::int32_t id) { return &jo::as_plane(plane)->materials[id]; }

struct H {
    // 0x006360f0 Billboard::blit. ret 0x30.
    std::int32_t blit(std::int16_t x1, std::int16_t y1, std::int16_t x2, std::int16_t y2, std::uint32_t colour, void* texture,
                      std::int16_t sx1, std::int16_t sy1, std::int16_t sx2, std::int16_t sy2, std::int16_t rot, std::int16_t mode) {
        c_blit.hit();
        if (texture != nullptr && !texture_ready(texture)) return -1;
        void* const plane = billboard_plane(this);
        const Quad q = make_quad(x1, y1, x2, y2, texture, sx1, sy1, sx2, sy2);
        const std::int32_t id = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_materials(1));  // 0x005cd470
        joan::orig::as_material(material_at(plane, id))->construct();                          // 0x005d52a0
        jo::as_material(material_at(plane, id))->pass_mask = 1;
        jo::as_material(material_at(plane, id))->blend = 6;
        if (static_cast<std::int32_t>(mode) == 0) jo::as_material(material_at(plane, id))->texture_mode = 0x20000;
        else if (static_cast<std::int32_t>(mode) == 1) jo::as_material(material_at(plane, id))->texture_mode = 0x20001;
        jo::as_material(material_at(plane, id))->raster = jo::as_material(material_at(plane, id))->raster;
        joan::orig::as_material(material_at(plane, id))->set_texture(0, joan::orig::arg::mptr(texture));  // 0x005d5650
        add_quad(plane, q, &colour, rot, id);
        return id;
    }

    // 0x00636640 Billboard::blit_more (eax left from the last call / 0 on the early exit).
    // ret 0x30.
    std::int32_t blit_more(std::int16_t x1, std::int16_t y1, std::int16_t x2, std::int16_t y2, std::uint32_t colour, void* texture,
                           std::int32_t material, std::int16_t sx1, std::int16_t sy1, std::int16_t sx2, std::int16_t sy2,
                           std::int16_t rot) {
        c_blit_more.hit();
        if (texture != nullptr && !texture_ready(texture)) return 0;
        void* const plane = billboard_plane(this);
        const Quad q = make_quad(x1, y1, x2, y2, texture, sx1, sy1, sx2, sy2);
        return add_quad(plane, q, &colour, rot, material);
    }
};

#define U2(name, addr, fn) JOAN_REPLACE("game/ui2", name, addr, fn)
U2("Billboard::blit", 0x006360f0, &H::blit);
U2("Billboard::blit_more", 0x00636640, &H::blit_more);
#undef U2

}  // namespace game_ui2
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
namespace {
namespace game_b104_11 {

JOAN_COUNTER(c_quad, "Overlay::quad_636ff0");

struct H {
    // ------------------------------------------------------------------ 0x00636ff0
    u32 quad(u32 x0, u32 y0, u32 x1, u32 y1, u32 colour, void* tex, u32 u0, u32 v0, u32 u1, u32 v1, u32 rot,
             u32 mode) {
        c_quad.hit();
        if (tex != nullptr && joan::orig::arg::as<i32>(joan::orig::as_texture(tex)->ready()) == 0) return 0;  // 0x00605b70
        void* const mesh = joan::orig::arg::as<void*>(joan::orig::as_billboard(this)->plane());  // 0x00635e80
        const joan::hook::x87::Fpu fpu{true};
        const float zero = 0.0f;
        const float fx0 = static_cast<float>(fpu.add(static_cast<double>(static_cast<i16>(x0)), zero));
        const float fx1 = static_cast<float>(fpu.add(static_cast<double>(static_cast<i16>(x1)), zero));
        const float fy0 = static_cast<float>(fpu.mul(static_cast<double>(static_cast<i16>(y0)), -1.0));  // 0x9cf834
        const float fy1 = static_cast<float>(fpu.mul(static_cast<double>(static_cast<i16>(y1)), -1.0));
        float tu0 = 0.0f, tv0 = 0.0f, tu1 = 0.0f, tv1 = 0.0f;
        if (tex != nullptr) {
            auto ratio = [&](i32 n, i32 d) {
                return static_cast<float>(fpu.div(static_cast<double>(n), static_cast<double>(d)));
            };
            tu0 = static_cast<i16>(u0) == -1 ? 0.0f : ratio(static_cast<i16>(u0), jo::view<i32>(jo::as_texture(tex)->width));
            tv0 = static_cast<i16>(v0) == -1 ? 0.0f : ratio(static_cast<i16>(v0), jo::view<i32>(jo::as_texture(tex)->height));
            tu1 = static_cast<i16>(u1) == -1 ? 1.0f : ratio(static_cast<i16>(u1) + 1, jo::view<i32>(jo::as_texture(tex)->width));
            tv1 = static_cast<i16>(v1) == -1 ? 1.0f : ratio(static_cast<i16>(v1) + 1, jo::view<i32>(jo::as_texture(tex)->height));
        }
        const i32 m = joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->add_materials(1));  // 0x005cd470
        auto mat = [&]() { return &jo::as_plane(mesh)->materials[m]; };
        joan::orig::as_material(mat())->construct();  // 0x005d52a0
        jo::as_material(mat())->pass_mask = 1;
        jo::as_material(mat())->blend = 6;
        if (static_cast<i16>(mode) == 0) {
            jo::as_material(mat())->texture_mode = 0x20000;
        } else if (static_cast<i16>(mode) == 1) {
            jo::as_material(mat())->texture_mode = 0x20001;
        }
        jo::as_material(mat())->raster = jo::as_material(mat())->raster | 0x100u;
        joan::orig::as_material(mat())->set_texture(0, joan::orig::arg::mptr(tex));  // 0x005d5650
        // frame [ebp-0x80]: 4 positions (12 floats), then [ebp-0x50]: 4 uv pairs
        float frame[20] = {0.0f, fx0, fy0, 0.0f, fx1, fy0, 0.0f, fx1, fy1, 0.0f, fx0, fy1,
                           tu0,  tv0, tu1, tv0,  tu1, tv1, tu0,  tv1};
        const i32 v = joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->add_vertices(4));  // 0x005cd7f0
        jo::VertexF4* vp = &jo::as_plane(mesh)->vertices_f4[v];
        for (i32 i = 0; i < 4; ++i, ++vp) {
            std::memcpy(vp->pos, &frame[i * 3], 12);
            joan::orig::as_colour(&vp->attr[0])->from_rgba(&colour);  // 0x005e95b0
            const i32 k = (static_cast<i32>(static_cast<i16>(rot)) + i) % 4;
            std::memcpy(&vp->attr[1], &frame[12 + k * 2], 8);  // the uv pair (attr[1], attr[2])
        }
        const i32 t = joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->add_triangles(2));  // 0x005ce6f0
        joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(t), joan::orig::arg::u32(m), joan::orig::arg::u32(v), joan::orig::arg::u32(v + 3), joan::orig::arg::u32(v + 1));  // 0x005ce800
        joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(t + 1), joan::orig::arg::u32(m), joan::orig::arg::u32(v + 1), joan::orig::arg::u32(v + 3), joan::orig::arg::u32(v + 2));  // 0x005ce800
        return 0;  // 0x005ce800 returns nothing; the only caller (DlgRes::draw_image) has no callers
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("Overlay::quad_636ff0", 0x00636ff0, &H::quad);
#undef B

}  // namespace game_b104_11
}  // namespace

// ---------------------------------------- from modules/game_b115_03.cpp (game/b115_03)
//
//  * module game/b115_03 (cdecl, ret 0; the other wrappers of the module are in camman.cpp):
//      crt_acos  0x00635e50 (float)            -> CRT acos  0x007b2274
//
// MSVC 6 /Od frames: each float argument `fld dword; fstp qword` (exact), call the CRT, `fst dword [ebp-4]` (dead
// store, no pop), return the CRT's st0. The C++ returns the CRT result as the double its slot call gives (the
// facade of the wrapper returns double too); re/p1/gamefloat compares both with the original CRT code.
namespace {
namespace game_b115_03 {

double JOAN_CDECL crt_acos(float x) { return joan::orig::arg::as<double>(joan::orig::libc::acos(joan::orig::arg::f64(static_cast<double>(x)))); }  // 0x00635e50

#define R(name, addr, fn) JOAN_REPLACE("game/b115_03", name, addr, fn)
R("crt_acos", 0x00635e50, &crt_acos);
#undef R

}  // namespace game_b115_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Billboard::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_world::H::billboard_ctor>(this)); }  // 0x6351a0
auto joan::orig::Billboard::destruct() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_04::H::obja_dtor>(this)); }  // 0x635230
auto joan::orig::Billboard::create(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, const char* a5, std::int16_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10, std::uint32_t a11, std::uint32_t a12, std::uint32_t a13, std::uint32_t a14) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_overlays::H::create>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)); }  // 0x635260
auto joan::orig::Billboard::release(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_units4::H::holder_release>(this, a0)); }  // 0x6356c0
auto joan::orig::Billboard::place(void* a0) -> void { joan::bind::member<&::game_fx::H::billboard_place>(this, a0); }  // 0x635760
auto joan::orig::Billboard::set_position(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_world::H::billboard_set_pos>(this, a0, a1, a2); }  // 0x635d40
auto joan::orig::Billboard::set_15(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_world::H::billboard_set_15>(this, a0)); }  // 0x635d80
auto joan::orig::Billboard::set_19(std::uint32_t a0) -> void { joan::bind::member<&::game_world::H::billboard_set_19>(this, a0); }  // 0x635db0
auto joan::orig::Billboard::set_colors(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_06::H::set_colors>(this, a0)); }  // 0x635de0
auto joan::orig::Billboard::plane() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::plane5>(this)); }  // 0x635e80
auto joan::orig::Billboard::centre(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx_2::H::billboard_centre>(this, a0, a1)); }  // 0x635ea0
auto joan::orig::Billboard::blit(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::uint32_t a4, void* a5, std::int16_t a6, std::int16_t a7, std::int16_t a8, std::int16_t a9, std::int16_t a10, std::int16_t a11) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_ui2::H::blit>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)); }  // 0x6360f0
auto joan::orig::Billboard::blit_more(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::uint32_t a4, void* a5, std::int32_t a6, std::int16_t a7, std::int16_t a8, std::int16_t a9, std::int16_t a10, std::int16_t a11) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_ui2::H::blit_more>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)); }  // 0x636640
auto joan::orig::Billboard::normalise() -> void { joan::bind::member<&::game_overlays::H::normalise>(this); }  // 0x636aa0
auto joan::orig::Billboard::quad(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, void* a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10, std::uint32_t a11) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_11::H::quad>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)); }  // 0x636ff0
