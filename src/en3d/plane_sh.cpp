// en3d/plane_sh.cpp: functions of the original d:\ww\en3d\plane_sh.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

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
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../render/shadow_volumes.hpp"
#include "shadow_volume_gpu.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b106_15.cpp (game/b106_15)
//
//      0x005e3040 ShadowPlane::build(...)  (thiscall, ret 0x24, en3d/plane_sh.cpp) - welds
//                 vertices, collects light-facing triangles and their silhouette edges
//                 (0x005e2c80 into 0x00a678b4), extrudes the vertices along the light
//                 direction into the shared buffers 0x00a526ff / 0x00a52707.
//
//  - All callers ignore eax for 0x0043a980, 0x00511060, 0x005e3040, 0x0066ece0 and
//    0x006c0310 (checked at every call site; none has data references). 0x005370d0 and
//
//  - 0x005e3040: the original overwrites the vertex stride from 0x005cdae0 with 0x24 before
//    the extrusion pass - kept. [ebp-0xb4] (cos wrapper 0x00425670 result) and the transformed
//    point [ebp-0x28] are dead stores; the calls are kept. The early exit when no edge was
//    found skips the obj+0xb field reset, as in the original. The 0x2000-entry (16 KB) stack
//    remap buffer is reproduced as a local array.
namespace {
namespace game_b106_15 {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::import_call;
using joan::img::obj;
using joan::img::str;
[[maybe_unused]] float as_float(u32 bits) {
    float f;
    std::memcpy(&f, &bits, 4);
    return f;
}

JOAN_COUNTER(c_shadow, "ShadowPlane::build");

struct H {

    // ---- 0x005e3040 -------------------------------------------------------------------
    void shadow_build(u8* node, u32* light, u8** out_vb, i32* out_vcount, u16** out_ib, i32* out_tris,
                      u8* out_flag, void* out_matrix, float extrude) {
        c_shadow.hit(); const jo::Light* const lt = jo::as_light(light);  // typed view: the x64 layout has pointers before +0xc (light[3] was wrong)
        *out_vb = nullptr;
        *out_ib = nullptr;
        const i32 type = static_cast<i8>(jo::view<u8>(jo::self<jo::ShadowPlane>(this)->vertex_format));
        if (type != 1 && type != 2 && type != 5) return;
        if (jo::self<jo::ShadowPlane>(this)->vertex_count < 3) {
            if (jo::self<jo::ShadowPlane>(this)->data_pending == 0) return;
            if (jo::self<jo::ShadowPlane>(this)->source == nullptr) return;
            if (!(jo::self<jo::ShadowPlane>(this)->vertex_count < 3)) return;
        }
        if (jo::self<jo::ShadowPlane>(this)->data_pending != 0) {
            jo::view<u32>(jo::self<jo::ShadowPlane>(this)->vertex_count) = jo::view<u32>(jo::self<jo::ShadowPlane>(this)->source->vertex_count);
            jo::view<u32>(jo::self<jo::ShadowPlane>(this)->triangle_count) = jo::view<u32>(jo::self<jo::ShadowPlane>(this)->source->triangle_count);
            jo::self<jo::ShadowPlane>(this)->vertices_f1 = jo::self<jo::ShadowPlane>(this)->source->vertices_f1;
            jo::self<jo::ShadowPlane>(this)->vertices_f2 = jo::self<jo::ShadowPlane>(this)->source->vertices_f2;
            jo::self<jo::ShadowPlane>(this)->vertices_f5 = jo::self<jo::ShadowPlane>(this)->source->vertices_f5;
            jo::self<jo::ShadowPlane>(this)->triangles = jo::self<jo::ShadowPlane>(this)->source->triangles;
        }
        float pos[3];   // [ebp-0x10]
        float dir[3];   // [ebp-0x1c]
        u8 rot[0x40];   // [ebp-0x68]
        if (joan::orig::arg::as<void*>(joan::orig::as_light(light)->bone()) != nullptr) {  // 0x005c5be0
            void* m = joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_light(light)->bone()))->matrix());  // 0x0060ef80
            joan::orig::matrix_position(static_cast<void*>(pos), joan::orig::arg::ptr(m));  // 0x0060c310
            m = joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_light(light)->bone()))->matrix());
            joan::orig::matrix_axis_z(static_cast<void*>(dir), joan::orig::arg::ptr(m));  // 0x0060c220
        } else {
            std::memcpy(pos, lt->position, 12);
            joan::orig::orbit_matrix(joan::orig::arg::mptr(static_cast<void*>(rot)), joan::orig::arg::f32(lt->orbit_a), joan::orig::arg::f32(lt->orbit_b), joan::orig::arg::f32(0u), joan::orig::arg::f32(lt->orbit_c));  // 0x0060bd00
            joan::orig::matrix_axis_z(static_cast<void*>(dir), static_cast<void*>(rot));          // 0x0060c220
        }
        if ((lt->type == 2 || lt->type == 3) && jo::self<jo::ShadowPlane>(this)->bones != nullptr) {
            u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::ShadowPlane>(this)->bones))->matrix());  // 0x0060ef80
            dir[0] = reinterpret_cast<const float*>(m)[12] - lt->position[0];  // the matrix's translation
            dir[1] = reinterpret_cast<const float*>(m)[13] - lt->position[1];
            dir[2] = reinterpret_cast<const float*>(m)[14] - lt->position[2];
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(dir));  // vector_normalize
        }
        float point[3];  // [ebp-0x28]
        if (jo::self<jo::ShadowPlane>(this)->bones != nullptr && jo::self<jo::ShadowPlane>(this)->transformed == 0) {
            u8 inv[0x40];  // [ebp-0xac]
            import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::ShadowPlane>(this)->bones))->matrix()));  // matrix_inverse
            import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(pos), static_cast<void*>(inv));    // point_transform
            import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(dir), static_cast<void*>(inv));    // vector_transform
            import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(point), static_cast<void*>(jo::as_camera(node)->params.position), static_cast<void*>(inv));
            std::memcpy(out_matrix, joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::ShadowPlane>(this)->bones))->matrix()), 0x40);
        } else {
            std::memcpy(point, jo::as_camera(node)->params.position, 12);
            std::memcpy(out_matrix, obj(d::g_identity_matrix), 0x40);
        }
        const float length = 300.0f + extrude;  // [ebp-0xb0]: fld 0x9d7280; fadd arg

        // Shared vertex / index buffers.
        const i32 n = jo::self<jo::ShadowPlane>(this)->vertex_count;
        if (global<void*>(d::g_a526ff) != nullptr) {
            if (global<i32>(d::g_a52703) < n * 2) {
                joan::crt::operator_delete(joan::orig::arg::mptr(global<void*>(d::g_a526ff)));  // operator delete
                global<void*>(d::g_a526ff) = joan::crt::operator_new(joan::orig::arg::u32(static_cast<u32>(jo::self<jo::ShadowPlane>(this)->vertex_count) * 2u * 0x24u));
                global<i32>(d::g_a52703) = static_cast<i32>(static_cast<u32>(jo::self<jo::ShadowPlane>(this)->vertex_count) * 2u);
            }
        } else {
            global<void*>(d::g_a526ff) = joan::crt::operator_new(joan::orig::arg::u32(static_cast<u32>(jo::self<jo::ShadowPlane>(this)->vertex_count) * 2u * 0x24u));
            global<i32>(d::g_a52703) = static_cast<i32>(static_cast<u32>(jo::self<jo::ShadowPlane>(this)->vertex_count) * 2u);
        }
        if (global<void*>(d::g_a52707) != nullptr) {
            if (global<i32>(d::g_a5270b) < static_cast<i32>(jo::view<u32>(jo::self<jo::ShadowPlane>(this)->triangle_count) * 6u * 3u)) {
                joan::crt::operator_delete(joan::orig::arg::mptr(global<void*>(d::g_a52707)));
                global<void*>(d::g_a52707) = joan::crt::operator_new(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::ShadowPlane>(this)->triangle_count) * 6u * 3u * 2u));
                global<u32>(d::g_a5270b) = jo::view<u32>(jo::self<jo::ShadowPlane>(this)->triangle_count) * 6u * 3u;
            }
        } else {
            global<void*>(d::g_a52707) = joan::crt::operator_new(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::ShadowPlane>(this)->triangle_count) * 6u * 3u * 2u));
            global<u32>(d::g_a5270b) = jo::view<u32>(jo::self<jo::ShadowPlane>(this)->triangle_count) * 6u * 3u;
        }
        *out_vb = global<u8*>(d::g_a526ff);
        *out_ib = global<u16*>(d::g_a52707);
        *out_vcount = 0;
        *out_tris = 0;
        *out_flag = 0;
        if (lt->type == 2) {
            const float dead = joan::orig::arg::as<float>(joan::orig::libc::cos_f(joan::orig::arg::f32(lt->theta)));  // 0x00425670, [ebp-0xb4] dead
            (void)dead;
        }

        // Vertex weld map (+0x108): map[i] = first earlier equal vertex, else i.
        if (jo::self<jo::ShadowPlane>(this)->shadow_edges == nullptr) {
            jo::view<void*>(jo::self<jo::ShadowPlane>(this)->shadow_edges) = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(jo::self<jo::ShadowPlane>(this)->vertex_count * 2), str(d::s_d_ww_en3d_plane_sh_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a173f4)) + 0x83));  // 0x0058e7b0
            for (i32 i = 0; i < jo::self<jo::ShadowPlane>(this)->vertex_count; ++i) {
                u16 found = 0xffff;  // [ebp-0xbc]
                for (i32 j = 0; j < i; ++j) {
                    const u8 kind = jo::view<u8>(jo::self<jo::ShadowPlane>(this)->vertex_format);
                    u8 same = 0;
                    bool compared = true;
                    if (kind == 1) same = joan::orig::arg::as<u8>(joan::orig::vec_equal(joan::orig::arg::ptr(jo::view<u8*>(jo::self<jo::ShadowPlane>(this)->vertices_f1) + i * 0x24), joan::orig::arg::ptr(jo::view<u8*>(jo::self<jo::ShadowPlane>(this)->vertices_f1) + j * 0x24)));  // 0x0060ced0
                    else if (kind == 2) same = joan::orig::arg::as<u8>(joan::orig::vec_equal(joan::orig::arg::ptr(jo::view<u8*>(jo::self<jo::ShadowPlane>(this)->vertices_f2) + i * 0x3c), joan::orig::arg::ptr(jo::view<u8*>(jo::self<jo::ShadowPlane>(this)->vertices_f2) + j * 0x3c)));
                    else if (kind == 5) same = joan::orig::arg::as<u8>(joan::orig::vec_equal(joan::orig::arg::ptr(jo::view<u8*>(jo::self<jo::ShadowPlane>(this)->vertices_f5) + i * 0x48), joan::orig::arg::ptr(jo::view<u8*>(jo::self<jo::ShadowPlane>(this)->vertices_f5) + j * 0x48)));
                    else compared = false;
                    if (compared && same != 0) found = static_cast<u16>(j);
                    if (found != 0xffff) break;
                }
                jo::self<jo::ShadowPlane>(this)->shadow_edges[i].value = found == 0xffff ? static_cast<u16>(i) : found;
            }
        }

        i32 stride = 0;  // [ebp-0xc4]
        u8* const base = joan::orig::arg::as<u8*>(joan::orig::as_plane(this)->buffer(&stride));  // 0x005cdae0
        u16* ib = *out_ib;                                           // [ebp-0xf0]
        u16* tri = jo::view<u16*>(jo::self<jo::ShadowPlane>(this)->triangles);                                    // [ebp-0xf4]
        i32 edges = 0;                                               // [ebp-0xf8]
        u16* const edge_buf = &img::global<u16>(d::g_a678b4);
        for (i32 t = 0; t < jo::self<jo::ShadowPlane>(this)->triangle_count; ++t, tri += 3) {
            const u16 a = jo::self<jo::ShadowPlane>(this)->shadow_edges[tri[0]].value;
            const u16 b = jo::self<jo::ShadowPlane>(this)->shadow_edges[tri[1]].value;
            const u16 c = jo::self<jo::ShadowPlane>(this)->shadow_edges[tri[2]].value;
            if (a == b || a == c || b == c) continue;
            const float* const pa = reinterpret_cast<const float*>(base + stride * a);
            const float* const pb = reinterpret_cast<const float*>(base + stride * b);
            const float* const pc = reinterpret_cast<const float*>(base + stride * c);
            const float e1x = pb[0] - pa[0], e1y = pb[1] - pa[1], e1z = pb[2] - pa[2];
            const float e2x = pc[0] - pb[0], e2y = pc[1] - pb[1], e2z = pc[2] - pb[2];
            if (lt->type != 1 && lt->type != 3) continue;
            const float t1 = (e1y * e2z - e1z * e2y) * dir[0];
            const float t2 = (e1z * e2x - e1x * e2z) * dir[1];
            const float t3 = (e1x * e2y - e1y * e2x) * dir[2];
            const float dot = (t1 + t2) + t3;
            if (static_cast<double>(dot) >= 1e-07) continue;  // test ah,1: dot < 1e-7 or NaN
            joan::orig::as_plane(this)->toggle_edge(joan::orig::arg::mptr(edge_buf), &edges, joan::orig::arg::u32(b), joan::orig::arg::u32(a));  // 0x005e2c80
            joan::orig::as_plane(this)->toggle_edge(joan::orig::arg::mptr(edge_buf), &edges, joan::orig::arg::u32(c), joan::orig::arg::u32(b));
            joan::orig::as_plane(this)->toggle_edge(joan::orig::arg::mptr(edge_buf), &edges, joan::orig::arg::u32(a), joan::orig::arg::u32(c));
            ib[0] = a;
            ib[1] = b;
            ib[2] = c;
            ib += 3;
            *out_tris = *out_tris + 1;
        }
        if (edges == 0) {
            *out_tris = 0;
            return;
        }
        const u16* e = edge_buf;  // [ebp-0x10c]
        for (i32 k = 0; k < edges; ++k) {
            ib[0] = e[0];
            ib[1] = e[1];
            ib[2] = static_cast<u16>(e[0] + jo::self<jo::ShadowPlane>(this)->vertex_count);
            ib[3] = e[1];
            ib[4] = static_cast<u16>(e[1] + jo::self<jo::ShadowPlane>(this)->vertex_count);
            ib[5] = static_cast<u16>(e[0] + jo::self<jo::ShadowPlane>(this)->vertex_count);
            e += 2;
            ib += 6;
            *out_tris = *out_tris + 2;
        }

        // Extrusion: compact the referenced vertices into the vertex buffer.
        i32 count = 0;  // [ebp-0x110]
        u16 local[0x2000];  // [ebp-0x4114]
        u16* remap = local;  // [ebp-0x4118]
        if (jo::self<jo::ShadowPlane>(this)->vertex_count * 2 > 0x2000) remap = joan::orig::arg::as<u16*>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<u32>(jo::self<jo::ShadowPlane>(this)->vertex_count * 4))));
        std::memset(static_cast<void*>(remap), 0xff, joan::orig::arg::u32(static_cast<u32>(jo::self<jo::ShadowPlane>(this)->vertex_count * 4)));  // memset
        stride = 0x24;
        const float ox = dir[0] * length, oy = dir[1] * length, oz = dir[2] * length;
        ib = *out_ib;
        for (i32 k = 0; k < *out_tris * 3; ++k) {
            const u16 v = *ib;
            if (remap[v] == 0xffff) {
                remap[v] = static_cast<u16>(count);
                u8* const dst = *out_vb + count * 0x24;
                if (static_cast<i32>(v) < jo::self<jo::ShadowPlane>(this)->vertex_count) {
                    const u8* const src = base + stride * v;
                    std::memcpy(dst, src, 12);
                } else {
                    const float* const src = reinterpret_cast<const float*>(base + stride * (v - jo::self<jo::ShadowPlane>(this)->vertex_count));
                    jo::as_vertex_f1(dst)->pos[0] = ox + src[0];
                    jo::as_vertex_f1(dst)->pos[1] = oy + src[1];
                    jo::as_vertex_f1(dst)->pos[2] = oz + src[2];
                }
                (*out_vb + count * 0x24)[0x1b] = 0xff;
                (*out_vb + count * 0x24)[0x1a] = 0;
                (*out_vb + count * 0x24)[0x19] = 0;
                (*out_vb + count * 0x24)[0x18] = 0;
                ++count;
            }
            *ib = remap[v];
            ++ib;
        }
        *out_vcount = count;
        if (remap != local) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(remap)));  // operator delete
        if (jo::self<jo::ShadowPlane>(this)->data_pending != 0) {
            jo::view<u32>(jo::self<jo::ShadowPlane>(this)->vertex_count) = 0;
            jo::view<u32>(jo::self<jo::ShadowPlane>(this)->triangle_count) = 0;
            jo::self<jo::ShadowPlane>(this)->vertices_f1 = nullptr;
            jo::self<jo::ShadowPlane>(this)->vertices_f2 = nullptr;
            jo::self<jo::ShadowPlane>(this)->vertices_f5 = nullptr;
            jo::self<jo::ShadowPlane>(this)->triangles = nullptr;
        }
    }

    // [Graphics] GpuShadowVolumes (render/shadow_volumes.hpp): what shadow_build reads - the same early
    // exits, light direction (model space), world matrix and extrusion length - with the geometry handed to
    // the GPU instead of the volume built here. False = no volume (as shadow_build returning none).
    bool gpu_volume(u32* light, float* out_matrix, float extrude, joan::render::VolumeSource& src, float* dir, float& length) {
        jo::ShadowPlane* const self = jo::self<jo::ShadowPlane>(this); const jo::Light* const lt = jo::as_light(light);
        const i32 type = static_cast<i8>(jo::view<u8>(self->vertex_format));
        if (type != 1 && type != 2 && type != 5) return false;
        if (self->vertex_count < 3) {
            if (self->data_pending == 0) return false;
            if (self->source == nullptr) return false;
            if (!(self->vertex_count < 3)) return false;
        }
        if (lt->type != 1 && lt->type != 3) return false;  // shadow_build keeps no triangle for other lights
        float pos[3];
        u8 rot[0x40];
        if (joan::orig::arg::as<void*>(joan::orig::as_light(light)->bone()) != nullptr) {
            void* m = joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_light(light)->bone()))->matrix());
            joan::orig::matrix_position(static_cast<void*>(pos), joan::orig::arg::ptr(m));
            m = joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_light(light)->bone()))->matrix());
            joan::orig::matrix_axis_z(static_cast<void*>(dir), joan::orig::arg::ptr(m));
        } else {
            std::memcpy(pos, lt->position, 12);
            joan::orig::orbit_matrix(joan::orig::arg::mptr(static_cast<void*>(rot)), joan::orig::arg::f32(lt->orbit_a), joan::orig::arg::f32(lt->orbit_b), joan::orig::arg::f32(0u), joan::orig::arg::f32(lt->orbit_c));
            joan::orig::matrix_axis_z(static_cast<void*>(dir), static_cast<void*>(rot));
        }
        if (lt->type == 3 && self->bones != nullptr) {
            u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_bone(jo::view<void*>(self->bones))->matrix());
            dir[0] = reinterpret_cast<const float*>(m)[12] - lt->position[0];
            dir[1] = reinterpret_cast<const float*>(m)[13] - lt->position[1];
            dir[2] = reinterpret_cast<const float*>(m)[14] - lt->position[2];
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(dir));
        }
        if (self->bones != nullptr && self->transformed == 0) {
            u8 inv[0x40];
            import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(self->bones))->matrix()));
            import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(dir), static_cast<void*>(inv));
            std::memcpy(out_matrix, joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(self->bones))->matrix()), 0x40);
        } else {
            std::memcpy(out_matrix, obj(d::g_identity_matrix), 0x40);
        }
        length = 300.0f + extrude;

        // The geometry (a geometry-sharing plane borrows its source's for the call, as shadow_build).
        const bool borrow = self->data_pending != 0;
        if (borrow) {
            jo::view<u32>(self->vertex_count) = jo::view<u32>(self->source->vertex_count);
            jo::view<u32>(self->triangle_count) = jo::view<u32>(self->source->triangle_count);
            self->vertices_f1 = self->source->vertices_f1;
            self->vertices_f2 = self->source->vertices_f2;
            self->vertices_f5 = self->source->vertices_f5;
            self->triangles = self->source->triangles;
        }
        i32 stride = 0;
        src.vertices = joan::orig::arg::as<u8*>(joan::orig::as_plane(this)->buffer(&stride));  // 0x005cdae0
        src.stride = stride;
        src.vertex_count = self->vertex_count;
        src.triangles = jo::view<u16*>(self->triangles);
        src.triangle_count = self->triangle_count;
        if (borrow) {
            jo::view<u32>(self->vertex_count) = 0;
            jo::view<u32>(self->triangle_count) = 0;
            self->vertices_f1 = nullptr;
            self->vertices_f2 = nullptr;
            self->vertices_f5 = nullptr;
            self->triangles = nullptr;
        }
        return src.vertices != nullptr && src.triangles != nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_15", name, addr, fn)
B("Plane::create_shadow_volume", 0x005e3040, &H::shadow_build);
#undef B

}  // namespace game_b106_15
}  // namespace

namespace joan::en3d {
bool gpu_shadow_volume(void* plane, void* light, float* world, float extent, render::VolumeSource& src, float* dir, float& length) {
    return reinterpret_cast<game_b106_15::H*>(plane)->gpu_volume(static_cast<std::uint32_t*>(light), world, extent, src, dir, length);
}
}  // namespace joan::en3d

// ---------------------------------------- from modules/game_b104_10.cpp (game/b104_10)
//
//    - EdgeSet::toggle 0x005e2c80 (pairs, count*, a, b), ret 0x10: two binary searches over
//      sorted u16 pairs, removes (b,a) / (a,b) when present, inserts (a,b) otherwise (the
//      original's odd insert-position updates kept). eax replicated.
namespace {
namespace game_b104_10 {

using i32 = std::int32_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_toggle, "EdgeSet::toggle");

// ---------------------------------------------------------------------------------------
// EdgeSet::toggle 0x005e2c80
// ---------------------------------------------------------------------------------------
std::uintptr_t edge_toggle(u16* pairs, i32* count, u32 a_arg, u32 b_arg) {
    c_toggle.hit();
    const u32 a = a_arg & 0xffff, b = b_arg & 0xffff;
    if (*count == 0) {
        pairs[0] = static_cast<u16>(a_arg);
        pairs[1] = static_cast<u16>(b_arg);
        *count += 1;
        return static_cast<u32>(*count);
    }
    auto midpoint = [](i32 lo, i32 hi) {
        return static_cast<i32>(static_cast<u32>(hi) + static_cast<u32>(lo)) >> 1;
    };
    i32 lo = 0, hi = *count - 1, mid = 0;
    i32 ins = 0;  // uninitialised in the original only when *count < 0 (never)
    bool found = false;
    // search (b, a)
    while (!(hi < lo)) {
        mid = midpoint(lo, hi);
        const u32 w0 = pairs[mid * 2], w1 = pairs[mid * 2 + 1];
        if (static_cast<i32>(w0) < static_cast<i32>(b)) {
            lo = mid + 1;
        } else if (static_cast<i32>(w0) > static_cast<i32>(b)) {
            hi = mid - 1;
        } else if (static_cast<i32>(w1) < static_cast<i32>(a)) {
            hi = mid - 1;
        } else if (static_cast<i32>(w1) > static_cast<i32>(a)) {
            lo = mid + 1;
        } else {
            hi = mid;
            lo = mid;
            found = true;
            break;
        }
    }
    if (!found) {
        // search (a, b), tracking the insert position
        lo = 0;
        hi = *count - 1;
        while (!(hi < lo)) {
            mid = midpoint(lo, hi);
            const u32 w0 = pairs[mid * 2], w1 = pairs[mid * 2 + 1];
            if (static_cast<i32>(w0) < static_cast<i32>(a)) {
                ins = mid + 1;
                lo = mid + 1;
            } else if (static_cast<i32>(w0) > static_cast<i32>(a)) {
                ins = mid;
                hi = mid - 1;
            } else if (static_cast<i32>(w1) < static_cast<i32>(b)) {
                ins = hi;
                hi = mid - 1;
            } else if (static_cast<i32>(w1) > static_cast<i32>(b)) {
                ins = lo;
                lo = mid + 1;
            } else {
                hi = mid;
                lo = mid;
                found = true;
                break;
            }
        }
    }
    if (found) {
        if (mid != *count && mid != *count - 1) {
            std::memcpy(static_cast<void*>(pairs + mid * 2), static_cast<void*>(pairs + mid * 2 + 2), joan::orig::arg::u32(static_cast<u32>(((*count - mid) * 2 - 2) * 2)));
        }
        *count -= 1;
        return reinterpret_cast<std::uintptr_t>(count);
    }
    if (ins != *count) {
        std::memcpy(static_cast<void*>(pairs + ins * 2 + 2), static_cast<void*>(pairs + ins * 2), joan::orig::arg::u32(static_cast<u32>((*count - ins) * 4)));
    }
    pairs[ins * 2] = static_cast<u16>(a_arg);
    pairs[ins * 2 + 1] = static_cast<u16>(b_arg);
    *count += 1;
    return static_cast<u32>(*count);
}

struct H {
    std::uintptr_t edge_toggle(u16* pairs, i32* count, u32 a, u32 b) { return game_b104_10::edge_toggle(pairs, count, a, b); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_10", name, addr, fn)
B("EdgeSet::toggle", 0x005e2c80, &H::edge_toggle);
#undef B

}  // namespace game_b104_10
}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
namespace {
namespace en3d_b83_03 {


struct H {
    // 0x005e4320 (eax = this)
    void* set_10c(std::uint32_t v) {
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->stencil) = v;
        return this;
    }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("Object::set_10c", 0x005e4320, &H::set_10c);
#undef B83_03

}  // namespace en3d_b83_03
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

// Every helper shares one class so `this` is just the object's address.
struct H {
    std::int32_t plane_stencil_value() {  // 0x005e4350
        if (jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->list_kind) == 4 && jo::self<jo::Plane>(this)->owner != nullptr)
            return jo::self<jo::Plane>(this)->owner->byte_fc;
        return jo::self<jo::Plane>(this)->stencil;
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Plane::stencil_value", 0x005e4350, &H::plane_stencil_value);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
namespace {
namespace game_b103_11 {

using joan::call::thiscall;

using u8 = std::uint8_t;

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // 0x005e43a0
    H* obj_ctor() {
        joan::orig::as_plane_equation(self() + 0x30)->init_425700();         // 0x00425700
        std::memset(joan::orig::arg::mptr(this), 0, 0x68u);  // memset (x64: needs the class of 0x005e43a0: 0x68 bytes on x86, plane equation at +0x30; no caller in joan.exe)
        return this;
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Obj::Obj_5e43a0", 0x005e43a0, &H::obj_ctor);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x005e43e0
    std::uintptr_t wrap_5e4430() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_portal(this)->clear_sectors()); }  // 0x005e4430
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Obj5e4::wrap_5e4430", 0x005e43e0, &H::wrap_5e4430);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
namespace {
namespace game_b105_16 {

struct H {
    // ---------------------------------------------------------------- stubs / getters
    void stub0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Obj::stub_5e4410", 0x005e4410, &H::stub0);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {


struct H {
    // 0x005e4430
    void* vb_reset() {
        jo::self<jo::Portal>(this)->side_a = nullptr;
        jo::self<jo::Portal>(this)->side_b = nullptr;
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("VbAlloc::reset", 0x005e4430, &H::vb_reset);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Plane::toggle_edge(void* a0, void* a1, std::uint32_t a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_10::H::edge_toggle>(this, a0, a1, a2, a3)); }  // 0x5e2c80
auto joan::orig::Plane::build_shadow_volume(void* a0, void* a1, void* a2, void* a3, void* a4, void* a5, void* a6, void* a7, float a8) -> void { joan::bind::member<&::game_b106_15::H::shadow_build>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8); }  // 0x5e3040
auto joan::orig::Plane::set_10c(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_03::H::set_10c>(this, a0)); }  // 0x5e4320
auto joan::orig::Plane::stencil_value() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers::H::plane_stencil_value>(this)); }  // 0x5e4350
auto joan::orig::Portal::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_06::H::wrap_5e4430>(this)); }  // 0x5e43e0
auto joan::orig::Portal::clear_sectors() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_06::H::vb_reset>(this)); }  // 0x5e4430
