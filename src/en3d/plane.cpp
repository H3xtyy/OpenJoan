// en3d/plane.cpp: functions of the original d:\ww\en3d\plane.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <immintrin.h>
#include "../compiler.hpp"
#include "../platform/win32_types.hpp"

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../d3d8/d3d8_types.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/water.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../game/stream_cache_api.hpp"
#include "../render/grass_batch.hpp"
#include "../save/gen/records_s6.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

// Game memory manager (0xa48838) with file / line tracking.
constexpr img::Addr kMemory(d::g_mem);
std::int32_t line_base(img::Addr address) { return static_cast<std::int32_t>(global<std::int16_t>(address)); }

using joan::call::stdcall_call;

using u32 = std::uint32_t;

using joan::call::import_call;

using joan::orig::helpers::game_delete;

constexpr img::Addr kSysEn(d::g_sys3d);
using joan::orig::helpers::game_new;

using i32 = std::int32_t;
using u8 = std::uint8_t;

struct Vec3 {
    float x, y, z;
};

using joan::img::obj;
using joan::img::str;

using i8 = std::int8_t;
using u16 = std::uint16_t;

constexpr img::Addr kD3D(d::g_sys3d);       // en3d's Direct3D wrapper

using joan::hook::x87::ftol;

u32 bits(float v) {
    u32 r;
    std::memcpy(&r, &v, 4);
    return r;
}

struct V3 {
    u32 v[3];
};

using joan::orig::helpers::device;  // IDirect3DDevice8*

}  // namespace

// ---------------------------------------- from modules/en3d_helpers36.cpp (en3d/helpers36)
//
//      0x005cbbf0 Plane::Plane(src, share)      - copy constructor (memset 0x11c, init from the
//                 source sizes, vertex / index / strip copies unless shared, bones, children
//                 (+0xcc[1..4]) and +0xec deep-copied with new Plane(child, share), COM buffers
//                 +0x90 / +0x94 AddRef'd when shared). ret 8.
//
//      0x005cdc70 Plane::set_format(fmt)        - allocates the vertex stream of the new format,
//                 converts from the current one, frees the old stream, recurses into children.
//
//      0x005d4670 Plane::delete_children()      - deletes +0xcc[4..1], clears +0xe4 / +0xe8
//
//      0x005d4880 Plane::set_parent(parent, i)  - unlinks from +0xe8, links into parent+0xcc[i]
namespace {
namespace en3d_helpers36 {


constexpr img::Addr kPlaneCpp(d::s_d_ww_en3d_plane_cpp);  // "d:\ww\en3d\plane.cpp"
void* mem_alloc(std::int32_t size, const char* file, std::int32_t line) {
    return joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058e7b0
}
void mem_free(void* p, const char* file, std::int32_t line) {
    joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}
void* op_new(std::uint32_t size) { return joan::crt::operator_new(joan::orig::arg::u32(size)); }
void crt_memcpy(void* dst, const void* src, std::int32_t size) {
    std::memcpy(joan::orig::arg::mptr(dst), joan::orig::arg::ptr(src), joan::orig::arg::u32(size));  // memcpy
}

// The vertex arrays of the eight vertex formats (index = format) and their element sizes.
constexpr std::int32_t kStream[9] = {
    0, static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f1)), static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f2)),
    static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f3)), static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f4)),
    static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f5)), static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f6)),
    static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f7)), static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f8))};
constexpr std::int32_t kStreamSize[9] = {
    0, static_cast<std::int32_t>(sizeof(jo::VertexF1)), static_cast<std::int32_t>(sizeof(jo::VertexF2)),
    static_cast<std::int32_t>(sizeof(jo::VertexF3)), static_cast<std::int32_t>(sizeof(jo::VertexF4)),
    static_cast<std::int32_t>(sizeof(jo::VertexF5)), static_cast<std::int32_t>(sizeof(jo::VertexF6)),
    static_cast<std::int32_t>(sizeof(jo::VertexF7)), static_cast<std::int32_t>(sizeof(jo::VertexF8))};

JOAN_COUNTER(c_plane_copy, "Plane::Plane(copy)");
JOAN_COUNTER(c_set_format, "Plane::set_format");

struct H {
    // The vertex array at `off` (one of kStream).
    std::uint8_t*& stream(std::int32_t off) { return *reinterpret_cast<std::uint8_t**>(reinterpret_cast<std::uint8_t*>(this) + off); }

    // 0x005cbbf0 Plane::Plane(const Plane* src, bool share). ret 8.
    void* plane_copy(jo::Plane* src, std::uint32_t share_arg) {
        c_plane_copy.hit();
        const std::uint32_t share = share_arg & 0xffu;
        std::memset(static_cast<void*>(this), 0, sizeof(jo::Plane));  // memset 0x11c (x86)
        jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format) = jo::view<std::uint8_t>(src->vertex_format);
        jo::self<jo::Plane>(this)->buffer_mode = src->buffer_mode;
        jo::self<jo::Plane>(this)->skip_draw = 0xff;
        jo::self<jo::Plane>(this)->normals_pending_ = src->normals_pending_;
        joan::orig::as_device_resource(&jo::self<jo::Plane>(this)->resource)->set_callback(0u, 0u, 0u);  // 0x005e9010
        if (src->bones == nullptr) return this;
        if (share != 0) {
            joan::orig::as_plane(this)->init(0, 0, joan::orig::arg::i32(src->material_count), joan::orig::arg::u32(static_cast<std::int32_t>(src->bone_count)));  // 0x005cc7a0
        } else {
            joan::orig::as_plane(this)->init(joan::orig::arg::i32(src->vertex_count), joan::orig::arg::i32(src->triangle_count), joan::orig::arg::i32(src->material_count), joan::orig::arg::u32(static_cast<std::int32_t>(src->bone_count)));
            const std::int32_t n = src->vertex_count;
            if (jo::view<void*>(src->vertices_f1) != nullptr)
                crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f1), jo::view<void*>(src->vertices_f1), n * 0x24);
            else if (jo::view<void*>(src->vertices_f2) != nullptr)
                crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f2), jo::view<void*>(src->vertices_f2), n * 0x3c);
            else if (jo::view<void*>(src->vertices_f3) != nullptr)
                crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f3), jo::view<void*>(src->vertices_f3), n * 0x1c);
            else if (jo::view<void*>(src->vertices_f4) != nullptr)
                crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f4), jo::view<void*>(src->vertices_f4), n * 0x18);
            else if (jo::view<void*>(src->vertices_f5) != nullptr)
                crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f5), jo::view<void*>(src->vertices_f5), n * 0x48);
            else if (jo::view<void*>(src->vertices_f6) != nullptr)
                crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f6), jo::view<void*>(src->vertices_f6), n * 0x14);
            else if (jo::view<void*>(src->vertices_f7) != nullptr)
                crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f7), jo::view<void*>(src->vertices_f7), n * 0x48);
            else if (jo::view<void*>(src->vertices_f8) != nullptr)
                crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f8), jo::view<void*>(src->vertices_f8), n * 0x28);
        }
        jo::self<jo::Plane>(this)->transformed = src->transformed;
        if (share == 0) {
            crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->triangles), jo::view<void*>(src->triangles), jo::self<jo::Plane>(this)->triangle_count * 3 * 2);
            crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->triangle_materials), jo::view<void*>(src->triangle_materials), jo::self<jo::Plane>(this)->triangle_count * 2);
        }
        jo::self<jo::Plane>(this)->prim_type = src->prim_type;
        jo::self<jo::Plane>(this)->use_strips = src->use_strips;
        jo::self<jo::Plane>(this)->has_strips = src->has_strips;
        if (static_cast<std::int8_t>(jo::self<jo::Plane>(this)->has_strips) != 0 && share == 0) {
            jo::self<jo::Plane>(this)->strip_total = src->strip_total;
            jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths) = mem_alloc(jo::self<jo::Plane>(this)->material_capacity * 2, img::str(kPlaneCpp), line_base(d::g_a173bc) + 0x46);
            crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths), jo::view<void*>(src->strip_lengths), jo::self<jo::Plane>(this)->material_count * 2);
            jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->strip_capacity) = jo::view<std::uint32_t>(src->strip_capacity);
            jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->strip_index_count) = jo::view<std::uint32_t>(src->strip_index_count);
            jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices) = mem_alloc(jo::self<jo::Plane>(this)->strip_capacity * 2, img::str(kPlaneCpp), line_base(d::g_a173bc) + 0x4b);
            crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices), jo::view<void*>(src->strip_indices), jo::self<jo::Plane>(this)->strip_index_count * 2);
        }
        joan::stream_cache_api::indices_written(jo::view<void*>(jo::self<jo::Plane>(this)->triangles));
        joan::stream_cache_api::indices_written(jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices));
        for (std::int32_t i = 0; i < src->material_count; ++i)
            joan::orig::as_material(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material)))->assign(joan::orig::arg::mptr(jo::view<std::uint8_t*>(src->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material))));  // 0x005d53e0
        if (static_cast<std::int8_t>(jo::self<jo::Plane>(this)->owns_bones) != 0) {
            for (std::int32_t i = 0; i < static_cast<std::int32_t>(src->bone_count); ++i) {
                std::memcpy(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->bones) + i * static_cast<std::int32_t>(sizeof(jo::Bone)), jo::view<std::uint8_t*>(src->bones) + i * static_cast<std::int32_t>(sizeof(jo::Bone)), sizeof(jo::Bone::local));  // 0x40 of 0xa0-byte bones (x86)
                joan::orig::as_bone(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->bones) + i * static_cast<std::int32_t>(sizeof(jo::Bone)))->update();  // 0x0060ee60
            }
            for (std::int32_t i = 0; i < static_cast<std::int32_t>(src->bone_count); ++i) {
                void* const parent = src->bones[i].parent;
                if (parent == nullptr) continue;
                std::int32_t j = 0;
                for (; j < static_cast<std::int32_t>(src->bone_count); ++j)
                    if (static_cast<void*>(jo::view<std::uint8_t*>(src->bones) + j * static_cast<std::int32_t>(sizeof(jo::Bone))) == parent) break;
                if (j >= 0 && j < static_cast<std::int32_t>(src->bone_count))
                    joan::orig::as_bone(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->bones) + i * static_cast<std::int32_t>(sizeof(jo::Bone)))->set_parent(joan::orig::arg::mptr(static_cast<void*>(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->bones) + j * static_cast<std::int32_t>(sizeof(jo::Bone)))));  // 0x0060ec70
                else
                    joan::orig::as_bone(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->bones) + i * static_cast<std::int32_t>(sizeof(jo::Bone)))->set_parent(joan::orig::arg::mptr(parent));
                joan::orig::as_bone(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->bones) + i * static_cast<std::int32_t>(sizeof(jo::Bone)))->update();  // 0x0060ee60
            }
        }
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->list_kind) = jo::view<std::uint32_t>(src->list_kind);
        jo::self<jo::Plane>(this)->owner = src->owner;
        joan::orig::as_plane(this)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        if (share == 0) {
            if (jo::view<void*>(src->material_first) != nullptr) {
                jo::view<void*>(jo::self<jo::Plane>(this)->material_first) = mem_alloc(src->material_count * 2, img::str(kPlaneCpp), line_base(d::g_a173bc) + 0x7f);
                crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->material_first), jo::view<void*>(src->material_first), src->material_count * 2);
            }
            if (jo::view<void*>(src->material_ranges) != nullptr) {
                jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges) = mem_alloc(src->material_count * 4, img::str(kPlaneCpp), line_base(d::g_a173bc) + 0x85);
                crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges), jo::view<void*>(src->material_ranges), src->material_count * 4);
            }
        }
        jo::self<jo::Plane>(this)->list_slots_ready = 0;
        jo::self<jo::Plane>(this)->load_state = src->load_state;
        for (std::int32_t i = 1; i <= 4; ++i) {
            if (jo::view<void*>(src->children[i - 1]) == nullptr) continue;
            void* const mem = op_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
            void* const child = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_plane(mem)->Plane_copy(joan::orig::arg::mptr(jo::view<void*>(src->children[i - 1])), joan::orig::arg::u32(share)))  // 0x005cbbf0
                                               : nullptr;
            joan::orig::as_plane(this)->add_child(joan::orig::arg::i32(i), joan::orig::arg::mptr(child));                        // 0x005d4470
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Plane>(this)->children[i - 1]))->share_bones(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x005d4a00
        }
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->lod_distance) = jo::view<std::uint32_t>(src->lod_distance);
        jo::self<jo::Plane>(this)->lod_count = src->lod_count;
        if (jo::view<void*>(src->next) != nullptr) {
            void* const mem = op_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
            void* const child = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_plane(mem)->Plane_copy(joan::orig::arg::mptr(jo::view<void*>(src->next)), joan::orig::arg::u32(share))) : nullptr;
            joan::orig::as_plane(this)->set_ec(joan::orig::arg::mptr(child));                                    // 0x005d7310
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Plane>(this)->next))->share_bones(joan::orig::arg::mptr(static_cast<void*>(this)));      // 0x005d4a00
        }
        jo::self<jo::Plane>(this)->mode = src->mode;
        if (share == 0 && jo::view<void*>(src->vertex_normals) != nullptr && jo::view<void*>(src->face_normals) != nullptr) {
            jo::view<void*>(jo::self<jo::Plane>(this)->face_normals) = op_new(static_cast<std::uint32_t>(src->vertex_count * 0xc));
            jo::view<void*>(jo::self<jo::Plane>(this)->vertex_normals) = op_new(static_cast<std::uint32_t>(src->vertex_count * 0xc));
            crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->face_normals), jo::view<void*>(src->face_normals), src->vertex_count * 0xc);
            crt_memcpy(jo::view<void*>(jo::self<jo::Plane>(this)->vertex_normals), jo::view<void*>(src->vertex_normals), src->vertex_count * 0xc);
        }
        if (share != 0) {
            jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) = jo::view<void*>(src->vertex_buffer);
            if (jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) != nullptr) add_ref(jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer));
            jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) = jo::view<void*>(src->index_buffer);
            if (jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) != nullptr) add_ref(jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer));
            if (jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) != nullptr || jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) != nullptr)
                joan::orig::as_sys_en(img::obj(d::g_sys3d))->link_resource(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Plane>(this)->resource)));  // 0x005ff290
        }
        jo::view<void*>(jo::self<jo::Plane>(this)->source) = src;
        jo::self<jo::Plane>(this)->data_pending = static_cast<std::uint8_t>(share);
        return this;
    }

    // IUnknown::AddRef (vtable slot 1, stdcall).
    static void add_ref(void* com) {
        using Fn = std::uint32_t(JOAN_STDCALL*)(void*);
        const Fn fn = reinterpret_cast<Fn>(joan::call::vtable_of(com)[1]);  // the COM object's vtable
        fn(com);
    }

    // Stream conversion loop: dst stream element `to(i)` from src stream element `from(i)`.
    // dst_off / src_off: the vertex arrays (kStream[format]), dst_size / src_size: their element sizes.
    void convert(std::uintptr_t thunk, std::int32_t dst_off, std::int32_t dst_size, std::int32_t src_off,
                 std::int32_t src_size) {
        for (std::int32_t i = 0; i < jo::self<jo::Plane>(this)->vertex_count; ++i)
            thiscall(thunk, stream(dst_off) + i * dst_size, static_cast<void*>(stream(src_off) + i * src_size));
    }
    void* stream_alloc(std::int32_t elem, std::int32_t line_add) {
        return mem_alloc(jo::self<jo::Plane>(this)->vertex_capacity * elem, img::str(kPlaneCpp), line_base(d::g_a173cc) + line_add);
    }
    void stream_free(std::int32_t off, std::int32_t line_add) {
        if (stream(off) == nullptr) return;
        mem_free(stream(off), img::str(kPlaneCpp), line_base(d::g_a173cc) + line_add);
        stream(off) = nullptr;
    }

    // 0x005cdc70 Plane::set_format(fmt). ret 4.
    // eax: the old format (sign-extended) when unchanged, else the last child's result / 4.
    std::int32_t set_format(std::int32_t fmt_arg) {
        c_set_format.hit();
        std::int8_t fmt = static_cast<std::int8_t>(fmt_arg);
        if (jo::self<jo::Plane>(this)->data_pending != 0) joan::orig::as_plane(this)->load_5d9be0();  // 0x005d9be0
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format)) == fmt) return static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format));
        const std::int8_t cur = static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format));
        switch (fmt) {
            case 1:
                jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f1) = stream_alloc(0x24, 0x10);
                switch (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format))) {
                    case 2: convert(fn::code_value(fn::Obj_copy_24_60f020), kStream[1], kStreamSize[1], kStream[2], kStreamSize[2]); break;  // 0x0060f020
                    case 5: convert(fn::code_value(fn::Obj_assign_24_60f0b0), kStream[1], kStreamSize[1], kStream[5], kStreamSize[5]); break;  // 0x0060f0b0
                    case 8: convert(fn::code_value(fn::Rec28_assign), kStream[1], kStreamSize[1], kStream[8], kStreamSize[8]); break;  // 0x0060f140
                    default: break;
                }
                break;
            case 2:
                jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f2) = stream_alloc(0x3c, 0x29);
                switch (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format))) {
                    case 1: convert(fn::code_value(fn::Obj_copy_60f1d0), kStream[2], kStreamSize[2], kStream[1], kStreamSize[1]); break;  // 0x0060f1d0
                    case 5: convert(fn::code_value(fn::Obj_copy_60f280), kStream[2], kStreamSize[2], kStream[5], kStreamSize[5]); break;  // 0x0060f280
                    default: break;
                }
                break;
            case 3: jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f3) = stream_alloc(0x1c, 0x78); break;
            case 4:
                jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f4) = stream_alloc(0x18, 0x7f);
                if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format)) == 1) convert(fn::code_value(fn::Vtx_from_24), kStream[4], kStreamSize[4], kStream[1], kStreamSize[1]);  // 0x0060f330
                break;
            case 5:
                if (global<std::uint32_t>(d::g_a52623) != 0) {
                    jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f5) = stream_alloc(0x48, 0x3f);
                    switch (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format))) {
                        case 1:
                            convert(fn::code_value(fn::Obj_assign_24_60f390), kStream[5], kStreamSize[5], kStream[1], kStreamSize[1]);  // 0x0060f390
                            stream_free(kStream[1], 0x4a);
                            joan::orig::as_plane(this)->build_tangents();  // 0x005d5770
                            break;
                        case 2:
                            convert(fn::code_value(fn::Obj_copy_24_60f420), kStream[5], kStreamSize[5], kStream[2], kStreamSize[2]);  // 0x0060f420
                            stream_free(kStream[2], 0x58);
                            joan::orig::as_plane(this)->build_tangents();
                            break;
                        default: break;
                    }
                } else if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format)) == 1) {
                    jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f2) = mem_alloc(jo::self<jo::Plane>(this)->vertex_capacity * 0x3c, img::str(kPlaneCpp), line_base(d::g_a173cc) + 0x66);
                    convert(fn::code_value(fn::Obj_copy_60f1d0), kStream[2], kStreamSize[2], kStream[1], kStreamSize[1]);  // 0x0060f1d0
                    fmt = 2;
                } else if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format)) == 2) {
                    fmt = 2;
                }
                break;
            case 6: jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f6) = stream_alloc(0x14, 0x8e); break;
            case 7:
                jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count) = 0;
                jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f7) = stream_alloc(0x48, 0x94);
                break;
            case 8:
                jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f8) = stream_alloc(0x28, 0x9b);
                if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format)) == 1) convert(fn::code_value(fn::Vtx_from_28), kStream[8], kStreamSize[8], kStream[1], kStreamSize[1]);  // 0x0060f4b0
                break;
            default: break;
        }
        (void)cur;
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format)) != fmt) {
            switch (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format))) {
                case 1: stream_free(kStream[1], 0xb2); break;
                case 2: stream_free(kStream[2], 0xb9); break;
                case 3: stream_free(kStream[3], 0xc0); break;
                case 4: stream_free(kStream[4], 0xc7); break;
                case 5: stream_free(kStream[5], 0xce); break;
                case 6: stream_free(kStream[6], 0xd5); break;
                case 7: stream_free(kStream[7], 0xdc); break;
                case 8: stream_free(kStream[8], 0xe3); break;
                default: break;
            }
        }
        jo::self<jo::Plane>(this)->vertex_format = fmt;
        std::int32_t result = 0;
        for (std::int32_t i = 1; i <= 4; ++i) {
            result = i;
            if (jo::view<void*>(jo::self<jo::Plane>(this)->children[i - 1]) != nullptr)
                result = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Plane>(this)->children[i - 1]))->set_format(static_cast<std::int32_t>(fmt)));  // 0x005cdc70
        }
        return result;
    }

    // 0x005d4670 Plane::delete_children(). ret 0, eax = 0 (the loop counter).
    std::int32_t delete_children() {
        for (std::int32_t i = 4; i > 0; --i) {
            void* const child = jo::view<void*>(jo::self<jo::Plane>(this)->children[i - 1]);
            if (child != nullptr) joan::orig::as_plane(child)->destroy(1u);  // 0x00459c40 deleting dtor
            jo::view<void*>(jo::self<jo::Plane>(this)->children[i - 1]) = nullptr;
        }
        jo::self<jo::Plane>(this)->lod_count = 0;
        jo::self<jo::Plane>(this)->parent = nullptr;
        return 0;
    }

    // 0x005d4880 Plane::set_parent(parent, slot). ret 8, eax = this.
    void* set_parent(void* parent, std::int32_t slot) {
        if (jo::view<void*>(jo::self<jo::Plane>(this)->parent) == parent) return this;
        if (jo::view<void*>(jo::self<jo::Plane>(this)->parent) != nullptr) {
            jo::view<void*>(jo::self<jo::Plane>(this)->parent->children[slot - 1]) = nullptr;
            jo::view<void*>(jo::self<jo::Plane>(this)->parent) = nullptr;
        }
        if (parent != nullptr) {
            jo::view<void*>(jo::as_plane(parent)->children[slot - 1]) = this;
            jo::view<void*>(jo::self<jo::Plane>(this)->parent) = parent;
        }
        return this;
    }
};

#define HELPER36(name, addr, fn) JOAN_REPLACE("en3d/helpers36", name, addr, fn)
HELPER36("Plane::Plane_5cbbf0", 0x005cbbf0, &H::plane_copy);
HELPER36("Plane::set_format", 0x005cdc70, &H::set_format);
HELPER36("Plane::clear_all_lod", 0x005d4670, &H::delete_children);
HELPER36("Plane::set_lod_parent", 0x005d4880, &H::set_parent);
#undef HELPER36

}  // namespace en3d_helpers36
}  // namespace

// ---------------------------------------- from modules/en3d_helpers32.cpp (en3d/helpers32)
//
//      0x005cc750 Plane::~Plane()                - 0x005ccaa0, 0x005d4670, 0x005d4880(0, +0xe7)
namespace {
namespace en3d_helpers32 {


struct H {
    // 0x005cc750 Plane::~Plane().
    std::uintptr_t plane_dtor() {
        joan::render::water_unregister(this);  // [Graphics] Water: a water surface's mesh is gone
        joan::orig::as_plane(this)->release();  // 0x005ccaa0
        joan::orig::as_plane(this)->delete_children();  // 0x005d4670
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_plane(this)->set_parent(joan::orig::arg::mptr(0), static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Plane>(this)->u8_e7))));  // 0x005d4880
    }
};

#define HELPER32(name, addr, fn) JOAN_REPLACE("en3d/helpers32", name, addr, fn)
HELPER32("Plane::~Plane", 0x005cc750, &H::plane_dtor);
#undef HELPER32

}  // namespace en3d_helpers32
}  // namespace

// ---------------------------------------- from modules/en3d_helpers18.cpp (en3d/helpers19)
//
//  * module en3d/helpers19: load time - Plane::init (0x005cc7a0), Plane::add_vertices
//    (0x005cd7f0), Plane::add_materials (0x005cd470).
namespace {
namespace en3d_helpers18 {

using joan::orig::helpers::at;

constexpr img::Addr kPlaneCpp(d::s_d_ww_en3d_plane_cpp);  // "d:\ww\en3d\plane.cpp"
void* mem_realloc(void* p, std::uint32_t size, std::int32_t line) {
    return joan::orig::as_mem(img::obj(kMemory))->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(img::str(kPlaneCpp)), joan::orig::arg::i32(line));  // 0x0058eb30
}
void memset_game(void* p, std::int32_t v, std::uint32_t n) { std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

JOAN_COUNTER(c_plane_init, "Plane::init");
JOAN_COUNTER(c_add_vertices, "Plane::add_vertices");
JOAN_COUNTER(c_add_materials, "Plane::add_materials");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- en3d/helpers19 ---------------------------------------------------------------------
    // 0x005cd7f0 Plane::add_vertices(n) -> first new index; per-format vertex array grows by 31.
    std::int32_t add_vertices(std::int32_t n) {
        c_add_vertices.hit();
        if (jo::self<jo::Plane>(this)->vertex_count + n > jo::self<jo::Plane>(this)->vertex_capacity) {
            if (jo::self<jo::Plane>(this)->data_pending != 0) joan::orig::as_plane(this)->load_5d9be0();  // 0x005d9be0
            jo::self<jo::Plane>(this)->vertex_capacity = jo::self<jo::Plane>(this)->vertex_count + n + 0x1f;
            struct Format {
                std::int32_t field, size, line;
            };
#define JOAN_FMT(n, line) {static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f##n)), static_cast<std::int32_t>(sizeof(jo::VertexF##n)), line}
            static const Format kFormats[8] = {JOAN_FMT(1, 0xf),  JOAN_FMT(2, 0x12), JOAN_FMT(3, 0x15), JOAN_FMT(4, 0x18),
                                               JOAN_FMT(5, 0x1b), JOAN_FMT(6, 0x1e), JOAN_FMT(7, 0x21), JOAN_FMT(8, 0x24)};
#undef JOAN_FMT
            const std::uint32_t type = static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::Plane>(this)->vertex_format) - 1);
            if (type <= 7) {
                const Format& fm = kFormats[type];
                void*& vertices = *reinterpret_cast<void**>(self() + fm.field);  // the format's vertex array
                vertices = mem_realloc(vertices, static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_capacity * fm.size),
                                                 line_base(d::g_a173c8) + fm.line);
            }
        }
        const std::int32_t first = jo::self<jo::Plane>(this)->vertex_count;
        jo::self<jo::Plane>(this)->vertex_count = jo::self<jo::Plane>(this)->vertex_count + n;
        return first;
    }

    // 0x005cd470 Plane::add_materials(n) -> first new index (materials +0x6c, matrices +0x84,
    // ids +0x88, textures inline +0xbc for <= 4 or +0xcc, flags +0x54 when +0x51).
    std::int32_t add_materials(std::int32_t n) {
        c_add_materials.hit();
        if (jo::self<jo::Plane>(this)->material_count + n > jo::self<jo::Plane>(this)->material_capacity) {
            jo::self<jo::Plane>(this)->material_capacity = jo::self<jo::Plane>(this)->material_count + n + 7;
            jo::view<void*>(jo::self<jo::Plane>(this)->materials) = mem_realloc(jo::view<void*>(jo::self<jo::Plane>(this)->materials), static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->material_capacity * static_cast<std::int32_t>(sizeof(jo::Material))), line_base(d::g_a173c4) + 4);
            jo::view<void*>(jo::self<jo::Plane>(this)->material_matrices) = mem_realloc(jo::view<void*>(jo::self<jo::Plane>(this)->material_matrices), static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->material_capacity << 6), line_base(d::g_a173c4) + 5);
            jo::view<void*>(jo::self<jo::Plane>(this)->material_ids) = mem_realloc(jo::view<void*>(jo::self<jo::Plane>(this)->material_ids), static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->material_capacity << 2), line_base(d::g_a173c4) + 6);
            if (jo::view<void*>(jo::self<jo::Plane>(this)->list_slots) != nullptr)
                jo::view<void*>(jo::self<jo::Plane>(this)->list_slots) = mem_realloc(jo::view<void*>(jo::self<jo::Plane>(this)->list_slots), static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->material_capacity << 2), line_base(d::g_a173c4) + 0xc);
            if (jo::view<std::int8_t>(jo::self<jo::Plane>(this)->has_strips) != 0) {
                jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths) = mem_realloc(jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths), static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->material_capacity << 1), line_base(d::g_a173c4) + 0x11);
                memset_game(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->strip_lengths) + jo::self<jo::Plane>(this)->material_count * 2, 0, static_cast<std::uint32_t>(n + 2));
            }
        }
        if (jo::view<std::int32_t>(jo::self<jo::Plane>(this)->list_slots_ready) != 0 && jo::self<jo::Plane>(this)->material_count <= 4 && jo::self<jo::Plane>(this)->material_count + n > 4)
            std::memmove(jo::view<void*>(jo::self<jo::Plane>(this)->list_slots), jo::self<jo::Plane>(this)->list_slots_inline, static_cast<std::size_t>(jo::self<jo::Plane>(this)->material_count << 2));
        const std::int32_t first = jo::self<jo::Plane>(this)->material_count;
        jo::self<jo::Plane>(this)->material_count = jo::self<jo::Plane>(this)->material_count + n;
        std::int32_t i = first;
        for (std::int32_t left = n; left > 0; ++i, --left) {
            joan::orig::as_material(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material)))->construct();                      // 0x005d52a0 Material::Material
            std::memcpy(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->material_matrices) + (i << 6), img::obj(d::g_identity_matrix), 64);  // identity
            jo::view<std::int32_t*>(jo::self<jo::Plane>(this)->material_ids)[i] = -1;
        }
        if (jo::view<std::int32_t>(jo::self<jo::Plane>(this)->list_slots_ready) != 0) {
            if (jo::self<jo::Plane>(this)->material_count <= 4)
                memset_game(&jo::self<jo::Plane>(this)->list_slots_inline[first], 0, static_cast<std::uint32_t>(n << 2));
            else
                memset_game(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->list_slots) + first * 4, 0, static_cast<std::uint32_t>(n << 2));
        }
        if (jo::view<std::int8_t>(jo::self<jo::Plane>(this)->has_strips) != 0) memset_game(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->strip_lengths) + first * 2, 0, static_cast<std::uint32_t>(n + 2));
        return first;
    }

    // 0x005cc7a0 Plane::init(vertices, triangles, materials, bones): own bones (0xa0 each, array
    // new with count) or the parent's (+0xe8), then the arrays and a white colour at +0x3c.
    void* plane_init(std::int32_t vertices, std::int32_t triangles, std::int32_t materials, std::uint32_t bones) {
        c_plane_init.hit();
        joan::orig::as_plane(this)->release();  // 0x005ccaa0
        if (bones == 0) {
            jo::self<jo::Plane>(this)->owns_bones = 0;
            jo::self<jo::Plane>(this)->bone_count = jo::self<jo::Plane>(this)->parent->bone_count;
            jo::view<void*>(jo::self<jo::Plane>(this)->bones) = jo::view<void*>(jo::self<jo::Plane>(this)->parent->bones);
        } else {
            jo::self<jo::Plane>(this)->owns_bones = 1;
            jo::self<jo::Plane>(this)->bone_count = static_cast<std::uint8_t>(bones);
            const std::int32_t count = jo::self<jo::Plane>(this)->bone_count;
            std::uint8_t* const block = joan::orig::arg::as<std::uint8_t*>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<std::uint32_t>(count * static_cast<std::int32_t>(sizeof(jo::Bone)) + 4))));  // new (0xa0 each on x86)
            std::uint8_t* array = nullptr;
            if (block != nullptr) {
                at<std::int32_t>(block, 0) = count;  // array cookie (4-byte count, our own layout on every architecture)
                joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(block + 4)), static_cast<std::uint32_t>(sizeof(jo::Bone)), joan::orig::arg::i32(count), fn::code_value(fn::Bone_Bone_60eb90), fn::code_value(fn::Frame_dtor));  // eh vector ctor: Bone::Bone / ~Bone
                array = block + 4;
            }
            jo::view<void*>(jo::self<jo::Plane>(this)->bones) = array;
            for (std::int32_t b = 0; b < jo::self<jo::Plane>(this)->bone_count; ++b)
                joan::orig::as_bone(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->bones) + b * static_cast<std::int32_t>(sizeof(jo::Bone)))->set_owner(joan::orig::arg::mptr(static_cast<void*>(this)), joan::orig::arg::mptr(reinterpret_cast<void*>(fn::code_value(fn::empty_cb_5cc9e0))));  // 0x0060edd0
        }
        joan::orig::as_device_resource(&jo::self<jo::Plane>(this)->resource)->set_callback(reinterpret_cast<void*>(fn::code_value(fn::Plane_device_cb)), static_cast<void*>(this), 0u);  // 0x005e9010
        joan::orig::as_plane(this)->add_vertices(joan::orig::arg::i32(vertices));   // 0x005cd7f0
        joan::orig::as_plane(this)->add_triangles(joan::orig::arg::i32(triangles));  // 0x005ce6f0
        joan::orig::as_plane(this)->add_materials(joan::orig::arg::i32(materials));  // 0x005cd470
        jo::self<jo::Plane>(this)->mode = 0;
        jo::self<jo::Plane>(this)->face_normals = nullptr;
        jo::self<jo::Plane>(this)->vertex_normals = nullptr;
        std::uint8_t white_obj[8];
        void* const white = joan::orig::as_colour(white_obj)->construct(0xffu, 0xffu, 0xffu, 0xffu);  // 0x0042a260
        return joan::orig::arg::as<void*>(joan::orig::as_colour(jo::self<jo::Plane>(this)->colour_)->from_rgba(joan::orig::arg::mptr(white)));                              // 0x005e95b0
    }
};

#define HELPER19(name, addr, fn) JOAN_REPLACE("en3d/helpers19", name, addr, fn)
HELPER19("Plane::add_vertex", 0x005cd7f0, &H::add_vertices);
HELPER19("Plane::add_material", 0x005cd470, &H::add_materials);
HELPER19("Plane::init", 0x005cc7a0, &H::plane_init);
#undef HELPER19

}  // namespace en3d_helpers18
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
//
//    - empty cdecl functions: 0x005bfd60 (dead code), 0x005be9f0 / 0x005bea10 (atexit handlers
//      registered at 0x005be548 / 0x005be516; CRT ignores eax), 0x005aaec0 / 0x005cc9e0
//      (callbacks stored by 0x0060eae0 / 0x0060edd0 at +0x98 and called as cdecl(ctx) at
//      0x0060ee3f / 0x0060eeed; 0x0060ee10 returns the callback's eax, which the empty original
//      leaves as ctx (eax = +0x9c before the call) - so ctx is returned).
//
//    - Obj::false_* 0x005c50f0 / 0x005c5110 / 0x005d6d80 (ret 0), 0x005c5130 / 0x005c5150 /
//      0x005c5170 / 0x005c5190 (ret 4) -> 0. Dead code.
namespace {
namespace game_b105_16 {

// The original leaves the caller's eax (= ctx, loaded by 0x0060ee10 right before the call) and
// 0x0060ee10 passes that eax on as its own return value - return ctx to keep it.
void* JOAN_CDECL empty_cb(void* ctx) { return ctx; }

struct H {
    u32 zero0() { return 0; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("empty_cb_5cc9e0", 0x005cc9e0, &empty_cb);
B("Obj::false_5d6d80", 0x005d6d80, &H::zero0);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/en3d_helpers25.cpp (en3d/helpers25)
//
//  * module en3d/helpers27: load time - ActorDef::create_lods (0x005b0ab0), Plane tangents
//    (0x005d5770).
namespace {
namespace en3d_helpers25 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_tangents, "Plane::build_tangents");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // 0x005cca00 Bone vector deleting destructor (count at [-4]).
    void* bone_array_delete(std::uint32_t flags) {
        if ((flags & 2) != 0) {
            joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(this)), static_cast<std::uint32_t>(sizeof(jo::Bone)), joan::orig::arg::i32(at<std::int32_t>(this, -4)), fn::code_value(fn::Frame_dtor));  // eh vector dtor (count: the array cookie)
            if ((flags & 1) != 0) game_delete(self() - 4);
            return self() - 4;
        }
        joan::orig::as_bone(this)->dtor();  // ~Bone
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
    // 0x005d6da0 Plane: +9 = 1 unless a material has blend mode 2.
    void plane_update_opaque() {
        for (std::int32_t m = 0; m < jo::self<jo::Plane>(this)->material_count; ++m) {
            if (jo::view<std::int32_t>(jo::self<jo::Plane>(this)->materials[m].pass_mask) == 2) {
                jo::self<jo::Plane>(this)->skip_draw = 0;
                return;
            }
        }
        jo::self<jo::Plane>(this)->skip_draw = 1;
    }
    bool resource_linked() { return jo::view<void*>(jo::self<jo::Resource>(this)->owner) != nullptr; }  // 0x005d2b10

    // 0x005d5770 Plane::build_tangents: per-vertex tangent (+0x24) / binormal (+0x30) from the
    // texture-space gradients of every triangle (0x48-byte vertices, uv at +0x1c), normalised;
    // +0x3c = tangent x binormal flipped to the normal's (+0xc) side.
    std::int32_t plane_build_tangents() {
        c_tangents.hit();
        if (jo::self<jo::Plane>(this)->data_pending != 0) joan::orig::as_plane(this)->load_5d9be0();  // 0x005d9be0
        for (std::int32_t i = 0; i < jo::self<jo::Plane>(this)->vertex_count; ++i) {
            std::uint8_t* const v = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f5) + i * 0x48;
            for (std::int32_t k = 6; k <= 11; ++k) jo::view<std::uint32_t>(jo::as_vertex_f5(v)->attr[k]) = 0;  // tangent, binormal
        }
        for (std::int32_t t = 0; t < jo::self<jo::Plane>(this)->triangle_count * 3; t += 3) {
            std::uint8_t* const a = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f5) + jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[t] * 0x48;
            std::uint8_t* const b = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f5) + jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[t + 1] * 0x48;
            std::uint8_t* const c = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f5) + jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[t + 2] * 0x48;
            for (std::int32_t k = 0; k < 3; ++k) {
                const float e1[3] = {jo::as_vertex_f5(b)->pos[k] - jo::as_vertex_f5(a)->pos[k], jo::as_vertex_f5(b)->attr[4] - jo::as_vertex_f5(a)->attr[4],
                                     jo::as_vertex_f5(b)->attr[5] - jo::as_vertex_f5(a)->attr[5]};
                const float e2[3] = {jo::as_vertex_f5(c)->pos[k] - jo::as_vertex_f5(a)->pos[k], jo::as_vertex_f5(c)->attr[4] - jo::as_vertex_f5(a)->attr[4],
                                     jo::as_vertex_f5(c)->attr[5] - jo::as_vertex_f5(a)->attr[5]};
                float n[3];
                import_call<void*>(d::iat_v3d_87_cross_product, n, e1, e2);  // cross_product
                double m = static_cast<double>(n[0]);
                if (m < 0.0) m = -m;  // CRT fabs 0x007b1f6b
                if (!(m > 1e-12)) continue;
                std::uint8_t* const verts[3] = {a, b, c};
                for (std::uint8_t* v : verts) {
                    jo::as_vertex_f5(v)->attr[6 + k] = -n[1] / n[0] + jo::as_vertex_f5(v)->attr[6 + k];
                    jo::as_vertex_f5(v)->attr[9 + k] = -n[2] / n[0] + jo::as_vertex_f5(v)->attr[9 + k];
                }
            }
        }
        for (std::int32_t i = 0; i < jo::self<jo::Plane>(this)->vertex_count; ++i) {
            std::uint8_t* const v = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f5) + i * 0x48;
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, v + 0x24);  // vector_normalize (in place)
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, v + 0x30);
            import_call<void*>(d::iat_v3d_87_cross_product, v + 0x3c, v + 0x24, v + 0x30);
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, v + 0xc);
            if (!(static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, v + 0x3c, v + 0xc)) >= 0.0f)) {  // dot_product
                float neg[3];
                std::memcpy(v + 0x3c, joan::orig::arg::as<void*>(joan::orig::vec_negate(joan::orig::arg::mptr(neg), joan::orig::arg::ptr(v + 0x3c))), 12);  // 0x0060cbc0 negate
            }
        }
        return 1;
    }
};

#define HELPER25(name, addr, fn) JOAN_REPLACE("en3d/helpers25", name, addr, fn)
#define HELPER27(name, addr, fn) JOAN_REPLACE("en3d/helpers27", name, addr, fn)
HELPER25("Plane::deinit_frame", 0x005cca00, &H::bone_array_delete);
HELPER25("Plane::update_opaque", 0x005d6da0, &H::plane_update_opaque);
HELPER25("Resource::linked", 0x005d2b10, &H::resource_linked);
HELPER27("Plane::create_basis_vectors", 0x005d5770, &H::plane_build_tangents);
#undef HELPER25
#undef HELPER27

}  // namespace en3d_helpers25
}  // namespace

// ---------------------------------------- from modules/en3d_helpers22.cpp (en3d/helpers24)
//
//  * module en3d/helpers24: Plane storage - release (0x005ccaa0), load on demand (0x005d9be0),
//    upload to vertex / index buffers (0x005cfa10), sort triangles by material (0x005cf1d0).
namespace {
namespace en3d_helpers22 {

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

void* game_device() { return global<void*>(d::g_game_device); }
constexpr img::Addr kPlaneCpp(d::s_d_ww_en3d_plane_cpp);  // "d:\ww\en3d\plane.cpp"
void* mem_alloc(std::uint32_t size, const char* file, std::int32_t line) {
    return joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058e7b0
}
void* mem_realloc(void* p, std::uint32_t size, const char* file, std::int32_t line) {
    return joan::orig::as_mem(img::obj(kMemory))->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
}
void mem_free(void* p, const char* file, std::int32_t line) { joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70
void game_memset(void* p, std::int32_t v, std::uint32_t n) { std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }
void error_log(const char* fmt, const char* text) {  // 0x00586cb0 (printf-style)
    joan::orig::error::fatal(joan::orig::arg::mptr(img::obj(d::g_err)), joan::orig::arg::str(fmt), text);
}
const char* error_string(std::int32_t hr) { return joan::orig::arg::as<const char*>(joan::orig::as_sys_en(img::obj(kSysEn))->error_string(joan::orig::arg::i32(hr))); }  // 0x006002b0

// Interface methods by vtable index.
constexpr std::int32_t kRelease = 2, kCreateVertexBuffer = 23, kCreateIndexBuffer = 24;
constexpr std::int32_t kBufferLock = 11, kBufferUnlock = 12, kBufferGetDesc = 13;  // vertex / index buffer

JOAN_COUNTER(c_plane_release, "Plane::release");
JOAN_COUNTER(c_plane_load, "Plane::load");
JOAN_COUNTER(c_plane_upload, "Plane::upload");
JOAN_COUNTER(c_plane_sort, "Plane::sort_by_material");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- en3d/helpers24 ---------------------------------------------------------------------
    // 0x005ccaa0 Plane::release: frees every array, buffers and bones, LOD children first.
    H* plane_release() {
        c_plane_release.hit();
        jo::self<jo::Plane>(this)->source = nullptr;
        jo::self<jo::Plane>(this)->data_pending = 0;
        for (std::int32_t i = 4; i > 0; --i)
            if (jo::view<void*>(jo::self<jo::Plane>(this)->children[i - 1]) != nullptr) joan::orig::as_plane(jo::view<void*>(jo::self<jo::Plane>(this)->children[i - 1]))->release();  // recursion
        jo::self<jo::Plane>(this)->lod_count = 0;
        auto free_field = [&](void*& field, std::int32_t line) {
            if (field != nullptr) {
                mem_free(field, img::str(kPlaneCpp), line_base(d::g_a173c0) + line);
                field = nullptr;
            }
        };
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->list_slots), 0x13);
        jo::self<jo::Plane>(this)->list_slots_ready = 0;
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges), 0x1b);
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->material_first), 0x22);
        if (jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) != nullptr) {
            com_call(jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer), kRelease);
            jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) != nullptr) {
            com_call(jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer), kRelease);
            jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) = nullptr;
        }
        for (std::int32_t m = jo::self<jo::Plane>(this)->material_count - 1; m >= 0; --m) joan::orig::as_material(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->materials) + m * static_cast<std::int32_t>(sizeof(jo::Material)))->dtor();  // 0x005d5360
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->materials), 0x53);
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->material_matrices), 0x58);
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->material_ids), 0x5d);
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->material_capacity) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->material_count) = 0;
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f1), 0x67);
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f2), 0x6c);
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f3), 0x71);
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f4), 0x76);
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f5), 0x7b);
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f6), 0x80);
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f7), 0x85);
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f8), 0x8a);
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_capacity) = 0;
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->triangles), 0x92);
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->triangle_materials), 0x97);
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_capacity) = 0;
        joan::orig::as_plane(this)->clear_extras(1);  // 0x005d7190
        if ((joan::orig::arg::as<std::uint32_t>(joan::orig::as_device_resource(&jo::self<jo::Plane>(this)->resource)->is_linked()) & 0xff) != 0) joan::orig::as_sys_en(img::obj(kSysEn))->unlink_resource(joan::orig::arg::mptr(&jo::self<jo::Plane>(this)->resource));  // 0x005d2b10 / 0x005ff380
        joan::orig::as_device_resource(&jo::self<jo::Plane>(this)->resource)->clear_link();  // 0x005e9060
        if (jo::view<std::int8_t>(jo::self<jo::Plane>(this)->owns_bones) != 0) {
            void* const bones = jo::view<void*>(jo::self<jo::Plane>(this)->bones);
            if (bones != nullptr) joan::orig::as_bone(bones)->vector_delete(3u);  // 0x005cca00 vector deleting dtor
        }
        jo::view<void*>(jo::self<jo::Plane>(this)->bones) = nullptr;
        jo::self<jo::Plane>(this)->bone_count = 0;
        free_field(jo::view<void*>(jo::self<jo::Plane>(this)->shadow_edges), 0xb0);
        if (jo::view<void*>(jo::self<jo::Plane>(this)->next) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Plane>(this)->next);
            if (p != nullptr) joan::orig::as_plane(p)->destroy(1u);  // 0x00459c40
            jo::view<void*>(jo::self<jo::Plane>(this)->next) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Plane>(this)->linked) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Plane>(this)->linked);
            if (p != nullptr) joan::orig::as_plane(p)->destroy(1u);
            jo::view<void*>(jo::self<jo::Plane>(this)->linked) = nullptr;
        }
        jo::self<jo::Plane>(this)->back_link = nullptr;
        if (jo::view<void*>(jo::self<jo::Plane>(this)->face_normals) != nullptr) {
            game_delete(jo::view<void*>(jo::self<jo::Plane>(this)->face_normals));
            jo::view<void*>(jo::self<jo::Plane>(this)->face_normals) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Plane>(this)->vertex_normals) != nullptr) {
            game_delete(jo::view<void*>(jo::self<jo::Plane>(this)->vertex_normals));
            jo::view<void*>(jo::self<jo::Plane>(this)->vertex_normals) = nullptr;
        }
        return this;
    }

    // 0x005d9be0 Plane::load: copy the shared source plane's (+0xc) geometry into this plane.
    std::int32_t plane_load() {
        c_plane_load.hit();
        if (jo::self<jo::Plane>(this)->data_pending == 0) return 0;
        if (jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) != nullptr) {
            com_call(jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer), kRelease);
            jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) != nullptr) {
            com_call(jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer), kRelease);
            jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) = nullptr;
        }
        jo::self<jo::Plane>(this)->data_pending = 0;
        std::uint8_t* const src = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->source);
        if (jo::view<void*>(jo::as_plane(src)->bones) == nullptr) return 0;
        joan::orig::as_plane(this)->add_vertices(joan::orig::arg::i32(jo::self<jo::Plane>(this)->source->vertex_count));  // add_vertices
        joan::orig::as_plane(this)->add_triangles(joan::orig::arg::i32(jo::self<jo::Plane>(this)->source->triangle_count));  // add_triangles
        // The vertex arrays of the eight formats (offset in Plane, element size).
#define JOAN_FMT(n) {static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f##n)), static_cast<std::int32_t>(sizeof(jo::VertexF##n))}
        static const std::int32_t kFields[8][2] = {JOAN_FMT(1), JOAN_FMT(2), JOAN_FMT(3), JOAN_FMT(4),
                                                   JOAN_FMT(5), JOAN_FMT(6), JOAN_FMT(7), JOAN_FMT(8)};
#undef JOAN_FMT
        auto stream = [](void* plane, std::int32_t off) -> void*& { return *reinterpret_cast<void**>(static_cast<std::uint8_t*>(plane) + off); };
        for (const auto& fd : kFields) {
            if (stream(jo::view<void*>(jo::self<jo::Plane>(this)->source), fd[0]) != nullptr) {
                std::memmove(stream(this, fd[0]), stream(jo::view<void*>(jo::self<jo::Plane>(this)->source), fd[0]),
                             static_cast<std::size_t>(jo::self<jo::Plane>(this)->source->vertex_count * fd[1]));
                break;
            }
        }
        std::memmove(jo::view<void*>(jo::self<jo::Plane>(this)->triangles), jo::view<void*>(jo::self<jo::Plane>(this)->source->triangles), static_cast<std::size_t>(jo::self<jo::Plane>(this)->triangle_count * 3 * 2));
        std::memmove(jo::view<void*>(jo::self<jo::Plane>(this)->triangle_materials), jo::view<void*>(jo::self<jo::Plane>(this)->source->triangle_materials), static_cast<std::size_t>(jo::self<jo::Plane>(this)->triangle_count * 2));
        if (jo::view<std::int8_t>(jo::self<jo::Plane>(this)->has_strips) != 0) {
            jo::self<jo::Plane>(this)->strip_total = jo::self<jo::Plane>(this)->source->strip_total;
            jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths) = mem_alloc(static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->material_capacity << 1), img::str(kPlaneCpp), line_base(d::g_a173f0) + 0x37);
            std::memmove(jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths), jo::view<void*>(jo::self<jo::Plane>(this)->source->strip_lengths), static_cast<std::size_t>(jo::self<jo::Plane>(this)->material_count << 1));
            jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->strip_capacity) = jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->source->strip_capacity);
            jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->strip_index_count) = jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->source->strip_index_count);
            jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices) = mem_alloc(static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->strip_capacity << 1), img::str(kPlaneCpp), line_base(d::g_a173f0) + 0x3c);
            std::memmove(jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices), jo::view<void*>(jo::self<jo::Plane>(this)->source->strip_indices), static_cast<std::size_t>(jo::self<jo::Plane>(this)->strip_index_count << 1));
        }
        joan::orig::as_plane(this)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        if (jo::view<void*>(jo::self<jo::Plane>(this)->source->material_first) != nullptr) {
            jo::view<void*>(jo::self<jo::Plane>(this)->material_first) = mem_alloc(static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->source->material_count << 1), img::str(kPlaneCpp), line_base(d::g_a173f0) + 0x45);
            std::memmove(jo::view<void*>(jo::self<jo::Plane>(this)->material_first), jo::view<void*>(jo::self<jo::Plane>(this)->source->material_first), static_cast<std::size_t>(jo::self<jo::Plane>(this)->source->material_count << 1));
        }
        if (jo::view<void*>(jo::self<jo::Plane>(this)->source->material_ranges) != nullptr) {
            jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges) = mem_alloc(static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->source->material_count << 2), img::str(kPlaneCpp), line_base(d::g_a173f0) + 0x4b);
            std::memmove(jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges), jo::view<void*>(jo::self<jo::Plane>(this)->source->material_ranges), static_cast<std::size_t>(jo::self<jo::Plane>(this)->source->material_count << 2));
        }
        jo::self<jo::Plane>(this)->load_state = jo::self<jo::Plane>(this)->source->load_state;
        if (jo::view<void*>(jo::self<jo::Plane>(this)->source->vertex_normals) != nullptr && jo::view<void*>(jo::self<jo::Plane>(this)->source->face_normals) != nullptr) {
            jo::view<void*>(jo::self<jo::Plane>(this)->face_normals) = game_new(static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->source->vertex_count * 0xc));
            jo::view<void*>(jo::self<jo::Plane>(this)->vertex_normals) = game_new(static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->source->vertex_count * 0xc));
            std::memmove(jo::view<void*>(jo::self<jo::Plane>(this)->face_normals), jo::view<void*>(jo::self<jo::Plane>(this)->source->face_normals), static_cast<std::size_t>(jo::self<jo::Plane>(this)->source->vertex_count * 0xc));
            std::memmove(jo::view<void*>(jo::self<jo::Plane>(this)->vertex_normals), jo::view<void*>(jo::self<jo::Plane>(this)->source->vertex_normals), static_cast<std::size_t>(jo::self<jo::Plane>(this)->source->vertex_count * 0xc));
        }
        return 1;
    }

    // 0x005cfa10 Plane::upload: bones updated; when dirty (+0x8c & 6) or buffers missing,
    // (re)create the vertex (+0x90) / index (+0x94) buffers if the size / format no longer
    // fit and copy the vertices and indices (or the strip indices +0x60) in.
    std::int32_t plane_upload() {
        c_plane_upload.hit();
        if (jo::view<void*>(jo::self<jo::Plane>(this)->bones) != nullptr)
            for (std::int32_t b = 0; b < jo::self<jo::Plane>(this)->bone_count; ++b) joan::orig::as_bone(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->bones) + b * static_cast<std::int32_t>(sizeof(jo::Bone)))->update();  // 0x0060ee60
        if (jo::self<jo::Plane>(this)->data_pending != 0) return 0;
        std::int32_t done = 0;
        if ((jo::self<jo::Plane>(this)->dirty & 6) == 0 &&
            (jo::view<std::int8_t>(jo::self<jo::Plane>(this)->buffer_mode) != 0 || (jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) != nullptr && jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) != nullptr))) {
            return 0;
        }
        joan::orig::as_plane(this)->update_opaque();  // 0x005d6da0
        if (jo::view<std::int8_t>(jo::self<jo::Plane>(this)->buffer_mode) == 0) {
            std::int32_t stride = 0;
            void* const vertices = joan::orig::arg::as<void*>(joan::orig::as_plane(this)->buffer(&stride));  // 0x005cdae0
            const std::uint32_t vb_size = static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count * stride);
            const std::uint32_t fvf = jo::as_vertex_format(joan::orig::arg::as<void*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<std::int32_t>(jo::self<jo::Plane>(this)->vertex_format))))))->fvf;  // 0x00600200
            std::int32_t hr;
            if (jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) != nullptr) {
                std::uint32_t desc[6];
                game_memset(desc, 0, 0x18);
                hr = com_call(jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer), kBufferGetDesc, static_cast<void*>(desc));
                if (desc[4] < vb_size || desc[0] != 100) {
                    com_call(jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer), kRelease);
                    jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) = nullptr;
                }
            }
            if (jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) == nullptr) {
                const std::uint32_t usage = global<std::uint32_t>(d::g_a5261b) & 0xfffffdffu;
                if (jo::self<jo::Plane>(this)->vertex_format == 5)
                    hr = com_call(game_device(), kCreateVertexBuffer, vb_size, usage, 0u, 0u, static_cast<void*>(&jo::self<jo::Plane>(this)->vertex_buffer));
                else if (jo::self<jo::Plane>(this)->vertex_format == 6)
                    hr = com_call(game_device(), kCreateVertexBuffer, vb_size, usage | 0x40u, fvf, 0u, static_cast<void*>(&jo::self<jo::Plane>(this)->vertex_buffer));
                else
                    hr = com_call(game_device(), kCreateVertexBuffer, vb_size, usage, fvf, 0u, static_cast<void*>(&jo::self<jo::Plane>(this)->vertex_buffer));
                if (hr != 0) error_log(&img::global<const char>(d::s_Cannot_create_vertex_buffer), error_string(hr));
                if ((joan::orig::arg::as<std::uint32_t>(joan::orig::as_device_resource(&jo::self<jo::Plane>(this)->resource)->is_linked()) & 0xff) == 0) joan::orig::as_sys_en(img::obj(kSysEn))->link_resource(joan::orig::arg::mptr(&jo::self<jo::Plane>(this)->resource));  // x86: this + 0x98  // 0x005ff290
            }
            void* data = nullptr;
            hr = com_call(jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer), kBufferLock, 0u, vb_size, &data, 0x800u);
            if (hr != 0) error_log(&img::global<const char>(d::s_Cannot_lock_vertex_buffer_s), error_string(hr));
            std::memmove(data, vertices, vb_size);
            com_call(jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer), kBufferUnlock);
            std::uint32_t ib_size, ib_format;
            if (jo::view<std::int8_t>(jo::self<jo::Plane>(this)->use_strips) != 0 && global<std::uint32_t>(d::g_strips_on) != 0) {
                ib_size = static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->strip_index_count << 1);
                ib_format = joan::orig::arg::as<std::uint32_t>(joan::orig::index_format(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices))));  // 0x00600290
            } else {
                ib_size = static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_count * 3 << 1);
                ib_format = joan::orig::arg::as<std::uint32_t>(joan::orig::index_format(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Plane>(this)->triangles))));
            }
            if (jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) != nullptr) {
                std::uint32_t desc[5];
                game_memset(desc, 0, 0x14);
                hr = com_call(jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer), kBufferGetDesc, static_cast<void*>(desc));
                if (desc[4] < ib_size || desc[0] != ib_format) {
                    com_call(jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer), kRelease);
                    jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) = nullptr;
                }
            }
            if (jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) == nullptr) {
                const std::uint32_t usage = global<std::uint32_t>(d::g_a5261b) & 0xfffffdffu;
                hr = com_call(game_device(), kCreateIndexBuffer, ib_size, usage, ib_format, 0u, static_cast<void*>(&jo::self<jo::Plane>(this)->index_buffer));
                if (hr != 0) error_log(&img::global<const char>(d::s_Cannot_create_index_buffer_s), error_string(hr));
                if ((joan::orig::arg::as<std::uint32_t>(joan::orig::as_device_resource(&jo::self<jo::Plane>(this)->resource)->is_linked()) & 0xff) == 0) joan::orig::as_sys_en(img::obj(kSysEn))->link_resource(joan::orig::arg::mptr(&jo::self<jo::Plane>(this)->resource));  // x86: this + 0x98
            }
            void* indices = nullptr;
            hr = com_call(jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer), kBufferLock, 0u, ib_size, &indices, 0x800u);
            if (hr != 0) error_log(&img::global<const char>(d::s_Cannot_lock_index_buffer_s), error_string(hr));
            if (jo::view<std::int8_t>(jo::self<jo::Plane>(this)->use_strips) != 0 && global<std::uint32_t>(d::g_strips_on) != 0)
                std::memmove(indices, jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices), ib_size);
            else
                std::memmove(indices, jo::view<void*>(jo::self<jo::Plane>(this)->triangles), ib_size);
            com_call(jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer), kBufferUnlock);
        }
        done = 1;
        jo::self<jo::Plane>(this)->dirty = 0;
        return done;
    }

    // 0x005cf1d0 Plane::sort_by_material(reorder): with reorder, triangles (+0x48 / +0x4c) are
    // regrouped by material and +0xb0 holds the per-material start; otherwise +0xb0 maps each
    // material to its last triangle + 1. Then +0xb4 = per-material [min, max] vertex index.
    void plane_sort(std::int32_t reorder) {
        c_plane_sort.hit();
        if (!(jo::self<jo::Plane>(this)->triangle_count > 1) || !(jo::self<jo::Plane>(this)->material_count > 1)) {
            if (jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges) != nullptr) {
                mem_free(jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges), img::str(kPlaneCpp), line_base(d::g_line_base_a173dc) + 8);
                jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges) = nullptr;
            }
            if (jo::view<void*>(jo::self<jo::Plane>(this)->material_first) != nullptr) {
                mem_free(jo::view<void*>(jo::self<jo::Plane>(this)->material_first), img::str(kPlaneCpp), line_base(d::g_line_base_a173dc) + 0xe);
                jo::view<void*>(jo::self<jo::Plane>(this)->material_first) = nullptr;
            }
            return;
        }
        if (reorder != 0) {
            std::uint16_t* const tris = static_cast<std::uint16_t*>(mem_alloc(static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_count * 3 << 1), img::str(kPlaneCpp), line_base(d::g_line_base_a173dc) + 0x18));
            std::uint16_t* const mats = static_cast<std::uint16_t*>(mem_alloc(static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_count << 1), img::str(kPlaneCpp), line_base(d::g_line_base_a173dc) + 0x19));
            std::uint16_t* const starts = static_cast<std::uint16_t*>(mem_alloc(static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->material_count << 1), img::str(kPlaneCpp), line_base(d::g_line_base_a173dc) + 0x1a));
            game_memset(starts, 0, static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->material_count << 1));
            const std::uint16_t* const old_mats = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangle_materials);
            for (std::int32_t i = 0; i < jo::self<jo::Plane>(this)->triangle_count; ++i) starts[jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangle_materials)[i]] = static_cast<std::uint16_t>(starts[jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangle_materials)[i]] + 1);
            std::uint16_t sum = 0;
            for (std::int32_t m = 0; m < jo::self<jo::Plane>(this)->material_count; ++m) {
                const std::uint16_t n = starts[m];
                starts[m] = sum;
                sum = static_cast<std::uint16_t>(sum + n);
            }
            for (std::int32_t i = 0; i < jo::self<jo::Plane>(this)->triangle_count; ++i) {
                const std::uint32_t slot = starts[jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangle_materials)[i]];
                mats[slot] = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangle_materials)[i];
                tris[slot * 3] = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[i * 3];
                tris[slot * 3 + 1] = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[i * 3 + 1];
                tris[slot * 3 + 2] = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[i * 3 + 2];
                starts[jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangle_materials)[i]] = static_cast<std::uint16_t>(starts[jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangle_materials)[i]] + 1);
            }
            (void)old_mats;
            if (jo::view<void*>(jo::self<jo::Plane>(this)->material_first) != nullptr) mem_free(jo::view<void*>(jo::self<jo::Plane>(this)->material_first), img::str(kPlaneCpp), line_base(d::g_line_base_a173dc) + 0x44);
            jo::view<void*>(jo::self<jo::Plane>(this)->material_first) = starts;
            mem_free(jo::view<void*>(jo::self<jo::Plane>(this)->triangles), img::str(kPlaneCpp), line_base(d::g_line_base_a173dc) + 0x46);
            jo::view<void*>(jo::self<jo::Plane>(this)->triangles) = tris;
            joan::stream_cache_api::indices_written(tris);
            mem_free(jo::view<void*>(jo::self<jo::Plane>(this)->triangle_materials), img::str(kPlaneCpp), line_base(d::g_line_base_a173dc) + 0x48);
            jo::view<void*>(jo::self<jo::Plane>(this)->triangle_materials) = mats;
            jo::self<jo::Plane>(this)->triangle_capacity = jo::self<jo::Plane>(this)->triangle_count;
        } else if (jo::self<jo::Plane>(this)->triangle_count > 1 && jo::self<jo::Plane>(this)->material_count > 1) {
            jo::view<void*>(jo::self<jo::Plane>(this)->material_first) = mem_realloc(jo::view<void*>(jo::self<jo::Plane>(this)->material_first), static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->material_count << 1), img::str(kPlaneCpp), line_base(d::g_line_base_a173dc) + 0x53);
            game_memset(jo::view<void*>(jo::self<jo::Plane>(this)->material_first), 0, static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->material_count << 1));
            for (std::int32_t i = 0; i < jo::self<jo::Plane>(this)->triangle_count; ++i)
                jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->material_first)[jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangle_materials)[i]] = static_cast<std::uint16_t>(i + 1);
        }
        if (jo::self<jo::Plane>(this)->material_count > 1) {
            jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges) = mem_realloc(jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges), static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->material_count << 2), img::str(kPlaneCpp), line_base(d::g_line_base_a173dc) + 0x63);
            for (std::int32_t m = 0; m < jo::self<jo::Plane>(this)->material_count; ++m) {
                jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->material_ranges)[m * 2] = static_cast<std::uint16_t>(jo::self<jo::Plane>(this)->vertex_count - 1);
                jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->material_ranges)[m * 2 + 1] = 0;
            }
            std::int32_t k = 0;
            for (std::int32_t t = 0; t < jo::self<jo::Plane>(this)->triangle_count; ++t) {
                const std::uint32_t m = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangle_materials)[t];
                for (std::int32_t c = 0; c < 3; ++c, ++k) {
                    const std::uint32_t v = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[k];
                    if (v == 0xffff) continue;
                    if (v < jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->material_ranges)[m * 2]) jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->material_ranges)[m * 2] = static_cast<std::uint16_t>(v);
                    if (v > jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->material_ranges)[m * 2 + 1]) jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->material_ranges)[m * 2 + 1] = static_cast<std::uint16_t>(v);
                }
            }
        }
    }
};

#define HELPER24(name, addr, fn) JOAN_REPLACE("en3d/helpers24", name, addr, fn)
HELPER24("Plane::deinit", 0x005ccaa0, &H::plane_release);
HELPER24("Plane::init_plane_arrays", 0x005d9be0, &H::plane_load);
HELPER24("Plane::recalc_change", 0x005cfa10, &H::plane_upload);
HELPER24("Plane::add_tristrip_triangle", 0x005cf1d0, &H::plane_sort);
#undef HELPER24

}  // namespace en3d_helpers22
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16 {


constexpr img::Addr kPlaneCpp(d::s_d_ww_en3d_plane_cpp);     // "d:\ww\en3d\plane.cpp"
void* mem_realloc(void* p, std::uint32_t size, const char* file, std::int32_t line) {
    return joan::orig::as_mem(img::obj(kMemory))->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
}
void mem_free(void* p, const char* file, std::int32_t line) {
    joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

JOAN_COUNTER(c_material_assign, "Material::assign");

// --- free functions ---------------------------------------------------------------------------
// 0x005d2aa0: primitive kind of triangle `tri` in a mixed index list (0xffff marks padding).
std::int32_t JOAN_CDECL triangle_kind(const std::uint8_t* indices, std::int32_t tri) {
    if (jo::as_tri16(indices)[tri].v[2] != 0xffff) return 4;
    if (jo::as_tri16(indices)[tri].v[1] != 0xffff) return 2;
    return 1;
}

struct H {
    void load_if_needed() {  // plane +0xb: data not loaded yet
        if (jo::self<jo::Plane>(this)->data_pending != 0) joan::orig::as_plane(this)->load_5d9be0();  // 0x005d9be0
    }

    void* camera_type() { return jo::self<jo::Scene>(this)->ptr_b9; }  // 0x005d80a0
    H* plane_set_fade(std::uint32_t c) {                            // 0x005d5e80
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->colour_) = c;
        return this;
    }
    H* plane_set_104(std::uint32_t v) {  // 0x005d6e80
        jo::self<jo::Plane>(this)->flags_104 = v;
        return this;
    }
    // 0x005ce800 Plane::set_triangle(i, material, a, b, c).
    void plane_set_triangle(std::int32_t i, std::uint32_t material, std::uint32_t a, std::uint32_t b, std::uint32_t c) {
        load_if_needed();
        jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangle_materials)[i] = static_cast<std::uint16_t>(material);
        jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[i * 3] = static_cast<std::uint16_t>(a);
        jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[i * 3 + 1] = static_cast<std::uint16_t>(b);
        jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[i * 3 + 2] = static_cast<std::uint16_t>(c);
        joan::stream_cache_api::indices_written(jo::view<void*>(jo::self<jo::Plane>(this)->triangles));
    }
    // 0x005cd2d0 Plane::clear.
    void plane_clear() {
        jo::self<jo::Plane>(this)->source = nullptr;
        jo::self<jo::Plane>(this)->data_pending = 0;
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_count) = 0;
        jo::self<jo::Plane>(this)->prim_type = 0;
        joan::orig::as_plane(this)->clear_extras(0);  // 0x005d7190
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count) = 0;
        for (std::int32_t i = jo::self<jo::Plane>(this)->material_count - 1; i >= 0; --i) joan::orig::as_material(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material)))->dtor();  // 0x005d5360
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->material_count) = 0;
        jo::self<jo::Plane>(this)->list_slots_ready = 0;
        joan::orig::as_plane(this)->invalidate(joan::orig::arg::u32(-1));
    }
    // 0x005cf920 Plane::invalidate(flags).
    void* plane_invalidate(std::uint32_t flags) {
        jo::self<jo::Plane>(this)->dirty = jo::self<jo::Plane>(this)->dirty | flags;
        if ((flags & 4) != 0) {  // the triangles changed (1: materials, 2: vertices)
            joan::stream_cache_api::indices_written(jo::view<void*>(jo::self<jo::Plane>(this)->triangles));
            joan::stream_cache_api::indices_written(jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices));
            if (jo::view<void*>(jo::self<jo::Plane>(this)->material_first) != nullptr) {
                mem_free(jo::view<void*>(jo::self<jo::Plane>(this)->material_first), img::str(kPlaneCpp), line_base(d::g_a173e0) + 0xa);
                jo::view<void*>(jo::self<jo::Plane>(this)->material_first) = nullptr;
            }
            if (jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges) != nullptr) {
                mem_free(jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges), img::str(kPlaneCpp), line_base(d::g_a173e0) + 0x10);
                jo::view<void*>(jo::self<jo::Plane>(this)->material_ranges) = nullptr;
            }
        }
        if ((flags & 7) != 0) {
            jo::self<jo::Plane>(this)->load_state = 1;
            return this;  // eax = this
        }
        return nullptr;  // eax = flags & 4 == 0 here
    }

    // 0x005ce6f0 Plane::add_triangles(n) -> first new index; grows by 7 spare.
    std::int32_t plane_add_triangles(std::int32_t n) {
        if (jo::self<jo::Plane>(this)->triangle_count + n > jo::self<jo::Plane>(this)->triangle_capacity) {
            load_if_needed();
            jo::self<jo::Plane>(this)->triangle_capacity = jo::self<jo::Plane>(this)->triangle_count + n + 7;
            jo::view<void*>(jo::self<jo::Plane>(this)->triangles) = mem_realloc(jo::view<void*>(jo::self<jo::Plane>(this)->triangles), static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_capacity * 3 * 2), img::str(kPlaneCpp),
                                         line_base(d::g_a173d0) + 0xc);
            jo::view<void*>(jo::self<jo::Plane>(this)->triangle_materials) = mem_realloc(jo::view<void*>(jo::self<jo::Plane>(this)->triangle_materials), static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_capacity * 2), img::str(kPlaneCpp),
                                         line_base(d::g_a173d0) + 0xd);
        }
        const std::int32_t first = jo::self<jo::Plane>(this)->triangle_count;
        jo::self<jo::Plane>(this)->triangle_count = jo::self<jo::Plane>(this)->triangle_count + n;
        return first;
    }
    // 0x005d53e0 Material::operator=.
    H* material_assign(const std::uint8_t* o) {
        c_material_assign.hit();
        jo::self<jo::Material>(this)->diffuse = jo::as_material(o)->diffuse;
        jo::self<jo::Material>(this)->ambient = jo::as_material(o)->ambient;
        jo::self<jo::Material>(this)->specular = jo::as_material(o)->specular;
        jo::self<jo::Material>(this)->emissive = jo::as_material(o)->emissive;
        jo::self<jo::Material>(this)->power = jo::as_material(o)->power;
        jo::self<jo::Material>(this)->u32_14 = jo::as_material(o)->u32_14;
        jo::self<jo::Material>(this)->u32_18 = jo::as_material(o)->u32_18;
        for (std::int32_t i = 0; i < 4; ++i) joan::orig::as_material(this)->set_texture(joan::orig::arg::i32(i), joan::orig::arg::mptr(jo::as_material(o)->textures[i]));  // 0x005d5650
        jo::self<jo::Material>(this)->texture_mode = jo::as_material(o)->texture_mode;
        jo::self<jo::Material>(this)->blend = jo::as_material(o)->blend;
        jo::self<jo::Material>(this)->filter = jo::as_material(o)->filter;
        jo::self<jo::Material>(this)->pass_mask = jo::as_material(o)->pass_mask;
        jo::self<jo::Material>(this)->raster = jo::as_material(o)->raster;
        jo::self<jo::Material>(this)->colour = jo::as_material(o)->colour;
        return this;
    }
    // 0x005cf050 Plane::copy_extras(other): flags +0x50..+0x53, per-vertex (+0x54) and +0x60 arrays.
    void plane_copy_extras(const std::uint8_t* o) {
        load_if_needed();
        jo::self<jo::Plane>(this)->use_strips = jo::as_plane(o)->use_strips;  // +0x50 / +0x51 on x86; behind pointers on x64
        jo::self<jo::Plane>(this)->has_strips = jo::as_plane(o)->has_strips;
        jo::self<jo::Plane>(this)->strip_total = jo::as_plane(o)->strip_total;
        if (jo::view<void*>(jo::as_plane(o)->strip_lengths) != nullptr && jo::view<std::int8_t>(jo::self<jo::Plane>(this)->has_strips) != 0) {
            jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths) = mem_realloc(jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths), static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->material_capacity << 1), img::str(kPlaneCpp),
                                         line_base(d::g_a173d8) + 0xc);
            std::memmove(jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths), jo::view<void*>(jo::as_plane(o)->strip_lengths), static_cast<std::size_t>(jo::self<jo::Plane>(this)->material_capacity << 1));
        }
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->strip_capacity) = jo::view<std::uint32_t>(jo::as_plane(o)->strip_capacity);
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->strip_index_count) = jo::view<std::uint32_t>(jo::as_plane(o)->strip_index_count);
        if (jo::view<void*>(jo::as_plane(o)->strip_indices) != nullptr && jo::self<jo::Plane>(this)->strip_capacity != 0) {
            jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices) = mem_realloc(jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices), static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->strip_capacity << 1), img::str(kPlaneCpp),
                                         line_base(d::g_a173d8) + 0x15);
            std::memmove(jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices), jo::view<void*>(jo::as_plane(o)->strip_indices), static_cast<std::size_t>(jo::self<jo::Plane>(this)->strip_capacity << 1));
            joan::stream_cache_api::indices_written(jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices));
        }
    }
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
#define HELPER17(name, addr, fn) JOAN_REPLACE("en3d/helpers17", name, addr, fn)
HELPER16("get_primitive_type", 0x005d2aa0, &triangle_kind);
HELPER16("Camera::type", 0x005d80a0, &H::camera_type);
HELPER16("Plane::set_fade", 0x005d5e80, &H::plane_set_fade);
HELPER16("Plane::set_104", 0x005d6e80, &H::plane_set_104);
HELPER16("Plane::set_triangle", 0x005ce800, &H::plane_set_triangle);
HELPER16("Plane::clear", 0x005cd2d0, &H::plane_clear);
HELPER16("Plane::clear_tristrip_triangle", 0x005cf920, &H::plane_invalidate);
HELPER17("Plane::add_triangle", 0x005ce6f0, &H::plane_add_triangles);
HELPER17("Material::assign", 0x005d53e0, &H::material_assign);
HELPER17("Plane::merge_tristrip", 0x005cf050, &H::plane_copy_extras);
#undef HELPER16
#undef HELPER17

}  // namespace en3d_helpers16
}  // namespace

// ---------------------------------------- from modules/game_b102_10.cpp (game/b102_10)
//
//    - Plane::device_cb 0x005cd3a0 (cdecl (obj, msg)) - registered as a callback at 0x005cc8d0.
//
//    - Mesh::copy_parts 0x005d4920 (other), ret 4.
//
//  - Live references: 0x005a8ea0 (jpeg error manager, stored at 0x005a8e26), 0x005a9d80 (two callers,
//    `and eax,0xff`), 0x005b5b70 (qsort at 0x005b5b08), 0x005cd3a0 (callback, 0x005cc8d0). All the others
//    have neither callers nor data references (dead code).
namespace {
namespace game_b102_10 {

// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}

JOAN_COUNTER(c_device_cb, "Plane::device_cb");
JOAN_COUNTER(c_copy_parts, "Mesh::copy_parts");

// 0x005cd3a0 device callback (obj, msg); eax as the original leaves it.
std::uintptr_t JOAN_CDECL plane_device_cb(u8* p, i32 msg) {
    c_device_cb.hit();
    std::uintptr_t eax = reinterpret_cast<std::uintptr_t>(p);
    if (msg == 0) {
        if (jo::as_plane(p)->vertex_buffer != nullptr) {
            com_call<u32>(jo::view<void*>(jo::as_plane(p)->vertex_buffer), 0x8);
            jo::as_plane(p)->vertex_buffer = nullptr;
        }
        if (jo::as_plane(p)->index_buffer != nullptr) {
            eax = com_call<u32>(jo::view<void*>(jo::as_plane(p)->index_buffer), 0x8);
            jo::as_plane(p)->index_buffer = nullptr;
        }
    } else if (msg == 1) {
        if (jo::as_plane(p)->data_pending == 0) eax = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_plane(p)->upload());  // 0x005cfa10
    }
    return eax;
}

struct H {
    // 0x005d4920
    std::uintptr_t copy_parts(u8* other) {
        c_copy_parts.hit();
        for (i32 i = jo::self<jo::Mesh>(this)->material_count - 1; i >= 0; --i) joan::orig::as_material(jo::view<u8*>(jo::self<jo::Mesh>(this)->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material)))->dtor();  // 0x005d5360
        jo::view<u32>(jo::self<jo::Mesh>(this)->material_count) = 0;
        const std::uintptr_t r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_plane(this)->add_materials(joan::orig::arg::i32(jo::view<u32>(jo::as_plane(other)->material_count))));  // 0x005cd470
        if (other == nullptr) return r;
        for (i32 i = 0; i < jo::as_plane(other)->material_count; ++i)
            joan::orig::as_material(jo::view<u8*>(jo::self<jo::Mesh>(this)->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material)))->assign(joan::orig::arg::mptr(jo::view<u8*>(jo::as_plane(other)->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material))));  // 0x005d53e0
        return reinterpret_cast<std::uintptr_t>(other);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_10", name, addr, fn)
B("Plane::device_cb", 0x005cd3a0, &plane_device_cb);
B("Plane::copy_materials", 0x005d4920, &H::copy_parts);
#undef B

}  // namespace game_b102_10
}  // namespace

// ---------------------------------------- from modules/en3d_helpers8.cpp (en3d/helpers8)
namespace {
namespace en3d_helpers8 {


bool below_or_nan(float a, float b) { return !(a >= b); }

struct H {

    // --- Plane (en3d/plane.cpp) ----------------------------------------------------------------------
    // 0x005cdae0: the plane's vertex buffer for its vertex type (+4) and its stride.
    void* plane_buffer(std::int32_t* stride) {
        if (jo::self<jo::Plane>(this)->data_pending != 0) return joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Plane>(this)->source))->buffer(joan::orig::arg::mptr(stride)));
        std::int32_t local = 0;
        if (stride == nullptr) stride = &local;
        void* buffer = nullptr;
        *stride = 0;
#define JOAN_F(n) static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f##n))
#define JOAN_S(n) static_cast<std::int32_t>(sizeof(jo::VertexF##n))
        static const std::int32_t kField[8] = {JOAN_F(1), JOAN_F(2), JOAN_F(3), JOAN_F(4), JOAN_F(5), JOAN_F(6), JOAN_F(7), JOAN_F(8)};
        static const std::int32_t kStride[8] = {JOAN_S(1), JOAN_S(2), JOAN_S(3), JOAN_S(4), JOAN_S(5), JOAN_S(6), JOAN_S(7), JOAN_S(8)};
#undef JOAN_F
#undef JOAN_S
        const std::uint32_t k = static_cast<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_format - 1);
        if (k <= 7) {
            buffer = *reinterpret_cast<void**>(reinterpret_cast<std::uint8_t*>(this) + kField[k]);  // the format's vertex array
            *stride = kStride[k];
        }
        return buffer;
    }
    // 0x005d4760: LOD level 4..1 by the camera's distance to the owning actor's bounding sphere
    // (only for actor-owned planes, +0x70 == 4, with LODs +0xe4): thresholds 8 * +0xe0 halved per
    // level; 0 beyond / none.
    std::int32_t plane_lod(void* camera) {
        if (jo::view<std::int8_t>(jo::self<jo::Plane>(this)->lod_count) == 0) return 0;
        Vec3 eye;
        joan::orig::matrix_position(&eye, joan::orig::arg::ptr(joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_camera(camera)->bone()))->matrix())));  // 0x0060c310
        if (jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->list_kind) != 4) return 0;
        float sphere[4];
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::Plane>(this)->owner))->bounding_sphere(joan::orig::arg::mptr(sphere), 1u);  // 0x005acc80
        Vec3 d = {eye.x - sphere[0], eye.y - sphere[1], eye.z - sphere[2]};
        const float distance = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &d));  // length
        float limit = global<float>(d::flt_8) * jo::self<jo::Plane>(this)->lod_distance;  // 8
        for (std::int32_t level = 4; level > 0; --level, limit = limit / 2.0f)
            if (!below_or_nan(distance, limit)) return level;
        return 0;
    }
};

#define HELPER8(name, addr, fn) JOAN_REPLACE("en3d/helpers8", name, addr, fn)
HELPER8("Plane::get_vertex_array_vptr", 0x005cdae0, &H::plane_buffer);
HELPER8("Plane::get_lod_level", 0x005d4760, &H::plane_lod);
#undef HELPER8

}  // namespace en3d_helpers8
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
//
//      0x005c0ef0 Model::load(file, anim, flag) (ret 0xc) - waits for the loader, +2 = (short)anim,
//                 _splitpath name into +4 (strncpy 0x20); anim == 0: static mesh (0x005cbaf0(1, 0),
//                 0x005ddd70), LODs <base>#<i+1><ext> i = 1..4 (0x005d4470 / 0x005d4a00), bounding
//                 sphere 0x005c51b0, +0x30 = 1, 0x005d4720(count, max(+0x40, 1000)), "#S" model
//                 (0x005d7310), "#V.SMO" shadow volume (materials 0xd / 1, 0x005d5650 x4,
//                 0x005cf920(1) / (-1), 0x005d73b0); anim != 0: the same with 0x005da280 and a
//                 shadow list (+0x2c, 0x005f45f0 / 0x005f4dc0 / 0x005f4fe0 / 0x005f5090),
//                 0x005c53b0, +0x30 = 0; byte +0 = 1
//
//      0x005ce8a0 Plane::set(a, b, c, d) (ret 0x10) - 0x005d9be0 when byte +0xb, 0x005ce800(.., 0xffff)
//
//      0x005d7420 Plane::set_mode(m) (ret 4) - m == 1: 0x005d7680 / 0x005d7bf0
//
//      0x005cb880 Vec3Array::set(i, v) / 0x005cb8c0 set_30_34(a, b) / 0x005d5610 set_14_18(p, v) /
namespace {
namespace en3d_b84_01 {


struct H {
    // 0x005d5610
    void set_14_18(const std::uint32_t* p, std::uint32_t v) {
        jo::self<jo::Material>(this)->u32_14 = *p;
        jo::self<jo::Material>(this)->u32_18 = v;
    }

    // 0x005ce8a0
    void plane_set(std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d) {
        if (jo::self<jo::Plane>(this)->data_pending != 0) joan::orig::as_plane(this)->load_5d9be0();  // 0x005d9be0
        joan::orig::as_plane(this)->set_triangle(joan::orig::arg::i32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d), 0xffffu);             // 0x005ce800
        jo::self<jo::Plane>(this)->prim_type = 1;
    }

    // 0x005d7420
    void set_mode(std::uint32_t m) {
        jo::self<jo::Plane>(this)->mode = m;
        if (jo::self<jo::Plane>(this)->mode == 1) {
            joan::orig::as_plane(this)->build_normals();  // 0x005d7680
            joan::orig::as_plane(this)->env_map();  // 0x005d7bf0
        }
    }

    // 0x005d73b0
    void link_f0(void* p) {
        if (jo::view<void*>(jo::self<jo::Plane>(this)->linked) != nullptr) jo::self<jo::Plane>(this)->linked->back_link = nullptr;
        jo::view<void*>(jo::self<jo::Plane>(this)->linked) = p;
        jo::view<void*>(jo::self<jo::Plane>(this)->linked->back_link) = this;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("Plane::set_line", 0x005ce8a0, &H::plane_set);
E3D("Plane::set_mode", 0x005d7420, &H::set_mode);
E3D("Plane::link", 0x005d73b0, &H::link_f0);
E3D("Material::set_14_18", 0x005d5610, &H::set_14_18);
#undef E3D

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
//
//      0x005ce900 Plane::set(a, b, c) (ret 0xc) - 0x005d9be0 when byte +0xb, 0x005ce800(.., 0xffff,
//                 0xffff), byte +8 = 1
namespace {
namespace en3d_b85_04 {


struct H {
    // 0x005ce900
    std::uint32_t plane_set3(std::uint32_t a, std::uint32_t b, std::uint32_t c) {
        if (jo::self<jo::Plane>(this)->data_pending != 0) joan::orig::as_plane(this)->load_5d9be0();  // 0x005d9be0
        joan::orig::as_plane(this)->set_triangle(joan::orig::arg::i32(a), b, c, 0xffffu, 0xffffu);  // 0x005ce800
        jo::self<jo::Plane>(this)->prim_type = 1;
        return 0;  // 0x005ce800 returns nothing; callers ignore the result
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("Plane::set_point", 0x005ce900, &H::plane_set3);
#undef E3D

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {


// 0x005d56c0: add a reference to a material's texture (+0x14 owner, 0x0060a500).
std::uint32_t material_texture_ref(void* texture) {
    return joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(jo::view<void*>(jo::as_texture(texture)->manager))->use(joan::orig::arg::mptr(texture)));
}

struct H {
    // --- Plane -------------------------------------------------------------------------------------------
    // 0x005ce970: does any material (+0x6c, 0x54 each, +0x40 pass flags) or, without materials,
    // the plane itself (+0x40 set: flags 1) take part in `pass` (SysEn 0x00600b30)?
    std::int32_t plane_in_pass(std::uint32_t pass) {
        if (jo::self<jo::Plane>(this)->material_count > 0) {
            std::int32_t i = 0;
            for (; i < jo::self<jo::Plane>(this)->material_count; ++i)
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sys_en(img::obj(kSysEn))->pass_filter(joan::orig::arg::u32(jo::self<jo::Plane>(this)->materials[i].pass_mask), joan::orig::arg::u32(pass))) != 0)
                    break;
            return i < jo::self<jo::Plane>(this)->material_count ? 1 : 0;
        }
        if (jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_count) == 0) return 0;
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_sys_en(img::obj(kSysEn))->pass_filter(1u, joan::orig::arg::u32(pass))) != 0 ? 1 : 0;
    }
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("Material::texture_ref", 0x005d56c0, &material_texture_ref);
HELPER6("Plane::has_material_on_render_pass", 0x005ce970, &H::plane_in_pass);
#undef HELPER6

}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- from modules/game_b111_15.cpp (game/b111_15)
//
//    - Plane::merge 0x005cea30 (thiscall(Plane* other, int keep_space, int mode), ret 0xc;
//      plane.cpp): material map (0x0058e7b0 alloc of other+0x64 * 4 + 4), per material of
//      other: mode 0 -> always new, 1 -> search the first +0x64 (snapshot) materials, 2 ->
//      search all (0x005d5500 equality); new material 0x005cd470(1) + copy 0x005d53e0 and
//      map[i] = new index. Format change 0x005cdc70, vertex block 0x005cd7f0 / 0x005cdae0 +
//      memcpy, optional re-space (identity 0xa18a78, other's matrix, inverse of ours, point /
//      vector transforms), index copy rebased by the vertex base (0x005ce6f0), face material
//      remap, flags |=, free the map, 0x005d5ee0(0x005d5f30(other)), returns 0x005cf920(-1).
//      Old skip reason ("matched materials read uninitialised memory") re-checked: map[i] is
//      not written for a matched material, but the map comes from the debug allocator
//      0x0058e7b0 (src/ilib/mem.cpp keeps it) which fills every block with 0xee, so those
//      faces deterministically get material 0xeeee - an original bug reproduced as is.
//      [ebp-0x14] (match index) is uninitialised only on the first iteration of a mode outside
//      0..2: every caller (0x005b0e1c / 0x005b0f89 mode 0, 0x005b13d8 mode 2, and the
//      en3d_helpers11 / en3d_helpers25 replicas of them) passes a constant 0 or 2.
namespace {
namespace game_b111_15 {


using i16 = std::int16_t;

JOAN_COUNTER(c_merge, "Plane::merge");

void* const kMem = obj(d::g_mem);
const char* const kPlaneCpp = str(d::s_d_ww_en3d_plane_cpp);  // "d:\\ww\\en3d\\plane.cpp"

struct alignas(4) Mat44 {
    u8 raw[64];
};

struct H {
    // ------------------------------------------------------------------ 0x005cea30
    u8* mat(i32 j) { return jo::view<u8*>(jo::self<jo::Plane>(this)->materials) + j * static_cast<std::int32_t>(sizeof(jo::Material)); }
    static u8* other_mat(u8* other, i32 i) { return jo::view<u8*>(jo::as_plane(other)->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material)); }
    bool same_mat(i32 j, u8* other, i32 i) {
        return (joan::orig::arg::as<u32>(joan::orig::as_material(mat(j))->equals(joan::orig::arg::mptr(static_cast<void*>(other_mat(other, i))))) & 0xff) != 0;  // 0x005d5500
    }
    u32 merge(u8* other, i32 keep_space, i32 mode) {
        c_merge.hit();
        if (jo::self<jo::Plane>(this)->data_pending != 0) joan::orig::as_plane(this)->load_5d9be0();  // 0x005d9be0
        u32* const map = joan::orig::arg::as<u32*>(joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(static_cast<u32>(jo::as_plane(other)->material_count) * 4u + 4u), joan::orig::arg::str(kPlaneCpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a173d4)) + 6)));  // 0x0058e7b0 (0xee filled)
        const i32 n0 = jo::self<jo::Plane>(this)->material_count;
        i32 j = 0;  // [ebp-0x14]: set on every iteration for the modes the callers use (0 / 2)
        for (i32 i = 0; i < jo::as_plane(other)->material_count; ++i) {
            if (mode == 0) {
                j = jo::self<jo::Plane>(this)->material_count;
            } else if (mode == 1) {
                for (j = 0; j < n0; ++j)
                    if (same_mat(j, other, i)) break;
                if (j >= n0) j = jo::self<jo::Plane>(this)->material_count;
            } else if (mode == 2) {
                for (j = 0; j < jo::self<jo::Plane>(this)->material_count; ++j)
                    if (same_mat(j, other, i)) break;
            }
            if (j >= jo::self<jo::Plane>(this)->material_count) {
                const i32 k = joan::orig::arg::as<i32>(joan::orig::as_plane(this)->add_materials(1));  // 0x005cd470
                map[i] = static_cast<u32>(k);
                joan::orig::as_material(mat(k))->assign(joan::orig::arg::mptr(static_cast<void*>(other_mat(other, i))));  // 0x005d53e0
            }
            // matched: map[i] keeps the allocator's 0xeeeeeeee (original bug)
        }
        if (jo::self<jo::Plane>(this)->vertex_format != jo::as_plane(other)->vertex_format)
            joan::orig::as_plane(this)->set_format(static_cast<i32>(jo::view<u8>(jo::as_plane(other)->vertex_format)));  // 0x005cdc70
        const i32 base = joan::orig::arg::as<i32>(joan::orig::as_plane(this)->add_vertices(joan::orig::arg::i32(jo::as_plane(other)->vertex_count)));  // 0x005cd7f0
        i32 stride = 0;  // written by 0x005cdae0
        const std::uintptr_t src = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_plane(other)->buffer(joan::orig::arg::mptr(static_cast<void*>(&stride))));  // 0x005cdae0
        const std::uintptr_t dst0 = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_plane(this)->buffer(joan::orig::arg::mptr(static_cast<void*>(nullptr))));
        const std::uintptr_t dst = dst0 + static_cast<u32>(base) * static_cast<u32>(stride);
        std::memcpy(joan::orig::arg::mptr(reinterpret_cast<void*>(dst)), joan::orig::arg::ptr(reinterpret_cast<const void*>(src)), joan::orig::arg::u32(static_cast<u32>(jo::as_plane(other)->vertex_count) * static_cast<u32>(stride)));  // memcpy
        if (keep_space == 0) {
            i32 any = 0;
            Mat44 m;
            std::memcpy(&m, img::obj(d::g_identity_matrix), sizeof m);  // identity
            if (jo::view<i32>(jo::as_plane(other)->transformed) == 0) {
                void* const om = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_plane(other)->bones))->matrix());  // 0x0060ef80
                import_call<void*>(d::iat_v3d_87_matrix_multiply_2, static_cast<void*>(&m), om);         // matrix_multiply
                any = 1;
            }
            if (jo::view<i32>(jo::self<jo::Plane>(this)->transformed) == 0) {
                Mat44 inv;
                void* const tm = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Plane>(this)->bones))->matrix());                // 0x0060ef80
                import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(&inv), tm);             // matrix_inverse
                import_call<void*>(d::iat_v3d_87_matrix_multiply_2, static_cast<void*>(&m), static_cast<void*>(&inv));  // matrix_multiply
                any = 1;
            }
            if (any != 0) {
                std::uintptr_t p = dst;
                for (i32 k = 0; k < jo::as_plane(other)->vertex_count; ++k, p += static_cast<u32>(stride))
                    import_call<void*>(d::iat_v3d_87_point_transform_2, reinterpret_cast<void*>(p), static_cast<void*>(&m));  // point_transform
                const u8* const fmt = joan::orig::arg::as<u8*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<i32>(jo::view<u8>(jo::self<jo::Plane>(this)->vertex_format))))));  // 0x00600200
                if (jo::as_vertex_format(fmt)->prelit == 0) {
                    const u8* const fmt2 = joan::orig::arg::as<u8*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<i32>(jo::view<u8>(jo::self<jo::Plane>(this)->vertex_format))))));
                    std::uintptr_t q = dst + static_cast<u32>(jo::as_vertex_format(fmt2)->normal_offset);
                    for (i32 k = 0; k < jo::as_plane(other)->vertex_count; ++k, q += static_cast<u32>(stride))
                        import_call<void*>(d::iat_v3d_87_vector_transform_2, reinterpret_cast<void*>(q), static_cast<void*>(&m));  // vector_transform
                }
            }
        }
        const i32 face_off = joan::orig::arg::as<i32>(joan::orig::as_plane(this)->add_triangles(joan::orig::arg::i32(jo::as_plane(other)->triangle_count)));  // 0x005ce6f0
        const u16* s = jo::view<u16*>(jo::as_plane(other)->triangles);
        u16* d = reinterpret_cast<u16*>(reinterpret_cast<std::uintptr_t>(jo::view<u16*>(jo::self<jo::Plane>(this)->triangles)) +
                                        static_cast<u32>(face_off) * 3u * 2u);
        for (i32 c = static_cast<i32>(static_cast<u32>(jo::as_plane(other)->triangle_count) * 3u); c > 0; --c, ++s, ++d) {
            if (static_cast<i32>(*s) < jo::as_plane(other)->vertex_count)
                *d = static_cast<u16>(static_cast<u32>(*s) + static_cast<u32>(base));
            else
                *d = *s;
        }
        for (i32 c = 0; c < jo::as_plane(other)->triangle_count; ++c) {
            const u32 m = jo::view<u16*>(jo::as_plane(other)->triangle_materials)[c];
            jo::view<u16*>(jo::self<jo::Plane>(this)->triangle_materials)[face_off + c] = static_cast<u16>(map[m]);
        }
        jo::self<jo::Plane>(this)->prim_type = static_cast<u8>(jo::self<jo::Plane>(this)->prim_type | jo::as_plane(other)->prim_type);
        joan::orig::as_mem(kMem)->del(static_cast<void*>(map), joan::orig::arg::str(kPlaneCpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a173d4)) + 0x6b));  // 0x0058ed70 free
        joan::orig::as_plane(this)->set_f8(joan::orig::arg::u32(joan::orig::arg::as<u32>(joan::orig::as_plane(other)->normals_pending())));  // 0x005d5ee0(0x005d5f30)
        joan::orig::as_plane(this)->invalidate(joan::orig::arg::u32(-1));             // 0x005cf920
        return 0;  // 0x005cf920 returns nothing; callers ignore the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_15", name, addr, fn)
B("Plane::merge_material_vertex_triangle", 0x005cea30, &H::merge);
#undef B

}  // namespace game_b111_15
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {


// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- Plane (en3d/plane.cpp) ---------------------------------------------------------
    void* plane_bone() { return jo::view<void*>(jo::self<jo::Plane>(this)->bones); }  // 0x005cf8d0
    void plane_set_owner(void* owner) {          // 0x005d03f0: also its LOD levels 1-4
        jo::view<void*>(jo::self<jo::Plane>(this)->owner) = owner;
        if (jo::view<std::int8_t>(jo::self<jo::Plane>(this)->lod_count) == 0) return;
        for (std::int32_t i = 4; i > 0; --i)
            if (void* const level = jo::self<jo::Plane>(this)->children[i - 1]) jo::view<void*>(jo::as_plane(level)->owner) = owner;
    }
    void* plane_level(std::int32_t* lod, std::uint32_t step_down) {  // 0x005d45e0
        for (;;) {
            void* const level = reinterpret_cast<void* const*>(&jo::self<jo::Plane>(this)->list_slots)[*lod];  // children[*lod - 1] (list_slots for 0, as the original)
            if ((step_down & 0xff) == 0 || level != nullptr) return level;
            if (--*lod == 0) return level;
        }
    }
    std::uint32_t plane_normals_pending() { return jo::self<jo::Plane>(this)->normals_pending_; }  // 0x005d5f30
    std::uint32_t plane_flags() {                                             // 0x005d5f60
        std::uint32_t flags = 0;
        if (jo::self<jo::Plane>(this)->normals_pending_ != 0) flags |= 0x01;
        if (jo::self<jo::Plane>(this)->flags_100 != 0) flags |= 0x40;
        if (jo::self<jo::Plane>(this)->flags_104 != 0) flags |= 0x80;
        return flags;
    }

    // --- Material, 0x54 bytes (en3d/plane.cpp) ---------------------------------------------
    std::uint8_t* material_construct() {  // 0x005d52a0 (returns the colour field, like the original)
        std::memset(self(), 0, sizeof(jo::Material));  // 0x54 (x86)
        jo::self<jo::Material>(this)->ambient = global<std::uint32_t>(d::g_init_white);
        jo::self<jo::Material>(this)->diffuse = jo::self<jo::Material>(this)->ambient;
        jo::self<jo::Material>(this)->blend = 0;
        jo::self<jo::Material>(this)->texture_mode = 1;
        jo::self<jo::Material>(this)->filter = 0;
        jo::self<jo::Material>(this)->pass_mask = 1;
        jo::self<jo::Material>(this)->raster = 0;
        jo::self<jo::Material>(this)->colour = 0xffffffffu;  // Color(255, 255, 255, 255) as D3DCOLOR (0x005e95b0)
        return reinterpret_cast<std::uint8_t*>(&jo::self<jo::Material>(this)->colour);  // +0x1c
    }
    void material_destroy() {  // 0x005d5360: release the 4 textures
        for (std::int32_t i = 0; i < 4; ++i) {
            if (jo::self<jo::Material>(this)->textures[i] == nullptr) continue;
            joan::orig::texture_ref::release(joan::orig::arg::mptr(jo::self<jo::Material>(this)->textures[i]));  // 0x0042cef0 release
            jo::self<jo::Material>(this)->textures[i] = nullptr;
        }
    }
    std::uint8_t* material_to_d3d(std::uint8_t* out) {  // 0x005d56f0 -> D3DMATERIAL8
        joan::d3d8::Material* const m = reinterpret_cast<joan::d3d8::Material*>(out);
        joan::orig::as_colour(&jo::self<jo::Material>(this)->diffuse)->to_float_5e9260(joan::orig::arg::mptr(&m->diffuse));  // 0x005e9260 colour -> 4 floats
        joan::orig::as_colour(&jo::self<jo::Material>(this)->ambient)->to_float_5e9260(joan::orig::arg::mptr(&m->ambient));
        joan::orig::as_colour(&jo::self<jo::Material>(this)->specular)->to_float_5e9260(joan::orig::arg::mptr(&m->specular));
        joan::orig::as_colour(&jo::self<jo::Material>(this)->emissive)->to_float_5e9260(joan::orig::arg::mptr(&m->emissive));
        jo::view<std::uint32_t>(m->power) = jo::self<jo::Material>(this)->power;  // power
        return out;
    }

    // --- SysEn / vgabuf (Direct3D wrapper) ------------------------------------------------------
    std::uint32_t sysen_count(std::uint32_t slot, std::uint32_t n) {  // 0x005d2b40 statistics
        jo::self<jo::SysEn>(this)->stats[static_cast<std::int32_t>(slot)] += n;
        return slot;
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Plane::bone", 0x005cf8d0, &H::plane_bone);
HELPER("Plane::set_org_ptr", 0x005d03f0, &H::plane_set_owner);
HELPER("Plane::get_lod_child", 0x005d45e0, &H::plane_level);
HELPER("Plane::normals_pending", 0x005d5f30, &H::plane_normals_pending);
HELPER("Plane::flags", 0x005d5f60, &H::plane_flags);
HELPER("Material::init", 0x005d52a0, &H::material_construct);
HELPER("Material::deinit", 0x005d5360, &H::material_destroy);
HELPER("Material::to_d3d", 0x005d56f0, &H::material_to_d3d);
HELPER("SysEn::count", 0x005d2b40, &H::sysen_count);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/en3d_helpers13.cpp (en3d/helpers13)
namespace {
namespace en3d_helpers13 {


struct H {
    // --- small helpers -----------------------------------------------------------------------
    void* plane_bone(std::int32_t i) { return jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->bones) + i * static_cast<std::int32_t>(sizeof(jo::Bone)); }  // 0x005cf8f0
};

#define HELPER13(name, addr, fn) JOAN_REPLACE("en3d/helpers13", name, addr, fn)
HELPER13("Plane::bone", 0x005cf8f0, &H::plane_bone);
#undef HELPER13

}  // namespace en3d_helpers13
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
//
//    - Plane::transform_5d0070 0x005d0070 (ret 0, only called by itself: dead): applies each
//      sub-plane's matrix to its vertices / normals once (+0x18 done flag).
//
//  - 0x005d0070: when a vertex format has no normals (or the stride is 0) the original
//    transforms an uninitialised 12-byte local in place and never reads it; here it is zeroed.
namespace {
namespace game_b108_02 {


JOAN_COUNTER(c_transform, "Plane::transform_5d0070");

struct H {
    // 0x005d0070 (only called by itself)
    // One sub-plane: done flag, matrix 0x0060ef80(0x005cf8d0()); identity matrix -> nothing.
    static void transform_one(void* p) {
        jo::view<i32>(jo::as_plane(p)->transformed) = 1;
        void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(p)->bone_5cf8d0()))->matrix());
        if (joan::orig::arg::as<i32>(joan::orig::libc::memcmp(joan::orig::arg::mptr(m), joan::orig::arg::mptr(obj(d::g_identity_matrix)), 0x40u)) == 0) return;  // memcmp with identity
        i32 stride = 0;  // always written by 0x005cdae0
        u8* v = joan::orig::arg::as<u8*>(joan::orig::as_plane(p)->buffer(&stride));                   // 0x005cdae0
        u8* const d = joan::orig::arg::as<u8*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(jo::view<u8>(jo::as_plane(p)->vertex_format)))));         // 0x00600200
        const i32 noff = jo::as_vertex_format(d)->normal_offset;
        const i32 nstride = noff == 0 ? 0 : stride;
        float dummy[3] = {0.0f, 0.0f, 0.0f};  // uninitialised in the original, never read
        u8* n = nstride != 0 ? v + noff : reinterpret_cast<u8*>(dummy);
        const i32 count = jo::as_plane(p)->vertex_count;
        for (i32 j = 0; j < count; ++j) {
            import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(v), m);  // point_transform
            import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(n), m);  // vector_transform
            v += stride;
            n += nstride;
        }
    }
    void transform_5d0070() {
        c_transform.hit();
        if (jo::self<jo::Plane>(this)->data_pending != 0) joan::orig::as_plane(this)->load_5d9be0();  // 0x005d9be0
        if (jo::view<i32>(jo::self<jo::Plane>(this)->transformed) != 0) return;
        if (jo::view<i8>(jo::self<jo::Plane>(this)->u8_e7) > 0 && jo::view<void*>(jo::self<jo::Plane>(this)->parent) != nullptr) {
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Plane>(this)->parent))->transform_5d0070();  // 0x005d0070
            return;
        }
        for (i32 i = 4; i >= 0; --i) {
            void* const p = i != 0 ? jo::view<void*>(jo::self<jo::Plane>(this)->children[i - 1]) : static_cast<void*>(this);
            if (p == nullptr) continue;
            if (jo::view<i32>(jo::as_plane(p)->transformed) != 0) continue;
            transform_one(p);
        }
        void* const q = jo::view<void*>(jo::self<jo::Plane>(this)->next);
        if (q == nullptr || jo::view<i32>(jo::as_plane(q)->transformed) != 0) return;
        transform_one(q);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Plane::transform_to_world_space", 0x005d0070, &H::transform_5d0070);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/en3d_helpers2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2 {


// Every thiscall helper shares one class so `this` is just the object's address.
struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- Plane (en3d/plane.cpp) -------------------------------------------------------------
    // 0x005d0480: an instance asks its shared plane; else it needs vertices and triangles.
    // The original has an unreachable "all textures loaded" loop after `return 1` (dead code).
    std::int32_t plane_visible() {
        if (jo::self<jo::Plane>(this)->data_pending != 0) return joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::self<jo::Plane>(this)->source)->visible());
        if (jo::self<jo::Plane>(this)->vertex_count <= 0 || jo::self<jo::Plane>(this)->triangle_count <= 0) return 0;
        return 1;
    }
};

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("Plane::can_render", 0x005d0480, &H::plane_visible);
#undef HELPER2

}  // namespace en3d_helpers2
}  // namespace

// ---------------------------------------- from modules/plane_draw.cpp (en3d/plane.cpp)
namespace {
namespace plane_draw {

using Device = joan::d3d8::IDirect3DDevice8;

JOAN_COUNTER(c_draw, "Plane::draw");

// v3d_87.dll / v3d_sse.dll math, called through joan.exe's import table.
template <class... A>
void v3d(img::Addr iat_slot, A... args) {
    reinterpret_cast<void*(JOAN_CDECL*)(A...)>(global<std::uintptr_t>(iat_slot))(args...);
}
constexpr img::Addr kMatrixMultiply3 = d::iat_v3d_87_matrix_multiply_3;  // (out, a, b): out = a * b
constexpr img::Addr kMatrixMultiply2 = d::iat_v3d_87_matrix_multiply_2;  // (a, b): a = a * b
constexpr img::Addr kMatrixInverse = d::iat_v3d_87_matrix_inverse_2;    // (out, in)
constexpr img::Addr kMatrixTranspose = d::iat_v3d_87_matrix_transpose_2;  // (out, in)
constexpr img::Addr kPointTransform2 = d::iat_v3d_87_point_transform_2;  // (v, m): v = v * m
constexpr img::Addr kPointTransform3 = d::iat_v3d_87_point_transform_3;  // (out, v, m)


// COM reference counting on the game's Direct3D resources (IUnknown slots 1 / 2).
void com_add_ref(void* object) {
    using Fn = unsigned long(JOAN_STDCALL*)(void*);
    (*reinterpret_cast<Fn**>(object))[1](object);
}
void com_release(void* object) {
    using Fn = unsigned long(JOAN_STDCALL*)(void*);
    (*reinterpret_cast<Fn**>(object))[2](object);
}

// --- globals ----------------------------------------------------------------------
Device* dev() { return global<Device*>(d::g_game_device); }
constexpr img::Addr kTextures(d::g_texture_array);  // texture manager
constexpr img::Addr kAnimDb(d::g_texture_effect_res);    // animated-texture table
constexpr img::Addr kMisc(d::g_misc);
constexpr img::Addr kIdentity(d::g_identity_matrix);
bool strips_on() { return global<std::int32_t>(d::g_strips_on) != 0; }
std::uint32_t now_ms() { return global<std::uint32_t>(d::g_now); }  // unaligned global

constexpr joan::d3d8::Enum kTsWorld = 0x100, kTsWorld1 = 0x101, kTsTexture0 = 0x10;
constexpr joan::d3d8::Enum kRsStencilRef = 0x39;
constexpr joan::d3d8::Enum kTssTexCoordIndex = 0xb, kTssTextureTransformFlags = 0x18;

// --- en3d helpers (thunk addresses, as the original calls them) ---------------------
const float* bone_matrix(void* bone) { return joan::orig::arg::as<const float*>(joan::orig::as_bone(bone)->matrix()); }  // 0x0060ef80

// Animated textures (material +0x34 = 0x01SSIIII: table entry IIII, set SS): advance
// each stage's texture to its next numbered frame once its delay has passed.
void animate_textures(std::uint8_t* plane, std::uint8_t* material, std::uint32_t anim_id, std::uint32_t set) {
    auto* info = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_db(img::obj(kAnimDb))->entry(joan::orig::arg::u32(static_cast<std::uint16_t>(anim_id))));  // 0x004b27b0
    char current[0x104], next[0x104];
    for (std::int32_t stage = 0; stage < jo::view<std::int16_t>(jo::as_tex_script_rec(info)->stage_count[set]); ++stage) {
        char* const entry = &jo::as_tex_script_rec(info)->stage_names[set * 0x84 + stage * 0x21];
        const std::int32_t frames = jo::as_tex_script_rec(info)->i32_361[set * 4 + stage];
        const float delay = jo::as_tex_script_rec(info)->f32_301[set * 4 + stage];
        void* const texture = jo::as_material(material)->textures[stage];
        if (texture == nullptr) {
            if (frames > 1) {
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(current)), joan::orig::arg::str(entry));  // strcpy
                std::strcat(static_cast<char*>(joan::orig::arg::mptr(current)), joan::orig::arg::str(&img::global<const char>(d::s_1_9d8de4)));  // strcat "1"
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(next)), joan::orig::arg::str(entry));
            }
        } else {
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(current)), joan::orig::arg::str(jo::as_texture(texture)->name));  // Texture +0x18 on x86
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(next)), joan::orig::arg::str(entry));
        }
        if (frames <= 1) continue;
        std::uint32_t& last = jo::as_material(material)->runtime[stage];
        if (static_cast<double>(now_ms() - last) < static_cast<double>(delay)) continue;
        const std::uint32_t name_length = joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(entry)));  // strlen
        char* const found = joan::orig::arg::as<char*>(joan::orig::libc::strstr(joan::orig::arg::str(current), joan::orig::arg::str(next)));              // strstr
        std::int32_t frame = 0;
        if (found) frame = joan::crt::atoi(joan::orig::arg::str(found + name_length));  // atoi
        const std::int32_t next_frame = frame + 1 != frames ? (frame + 1) % frames : frames;
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(next)), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_misc(img::obj(kMisc))->format_int_58fca0(joan::orig::arg::i32(next_frame), 1))));  // Misc::format
        void* tex = joan::orig::arg::as<void*>(joan::orig::as_textures(img::obj(kTextures))->find(joan::orig::arg::str(next)));  // 0x0060a4b0 find loaded
        if (tex == nullptr) {
            const float scaled = delay * 10.0f;  // 0x009cfc20
            const float limit = scaled < 500.0f ? scaled : 500.0f;
            if (static_cast<double>(now_ms() - last) < static_cast<double>(limit)) continue;
        }
        tex = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(next), joan::orig::arg::i32(jo::as_plane(plane)->mode == 2 ? 3 : 1), 0u));  // load
        std::int32_t& priority = global<std::int32_t>(d::g_texture_priority);
        const std::int32_t saved = priority;
        priority = -5000;
        joan::orig::as_material(material)->set_texture(joan::orig::arg::i32(stage), joan::orig::arg::mptr(tex));  // Material::set_texture 0x005d5650
        priority = saved;
        joan::orig::as_texture_manager(img::obj(kTextures))->release(joan::orig::arg::mptr(tex));  // release 0x0060a550
        last = now_ms();
    }
}

// Releases a buffer the plane owns (when it switches between strips and lists).
void release_index_buffer(std::uint8_t* p) {
    if (void* const ib = jo::as_plane(p)->index_buffer) {
        com_release(ib);
        jo::view<void*>(jo::as_plane(p)->index_buffer) = nullptr;
    }
}

// Fields a geometry-sharing plane (+0x0b) borrows from its source for one draw.
void borrow_geometry(std::uint8_t* p, bool from_source) {
    jo::Plane* const d = jo::as_plane(p);
    const jo::Plane* const src = d->source;
    d->vertex_count = from_source ? src->vertex_count : 0;
    d->triangle_count = from_source ? src->triangle_count : 0;
    d->triangles = from_source ? src->triangles : nullptr;
    d->triangle_materials = from_source ? src->triangle_materials : nullptr;
    d->strip_lengths = from_source ? src->strip_lengths : nullptr;
    d->strip_index_count = from_source ? src->strip_index_count : 0;
    d->material_first = from_source ? src->material_first : nullptr;
    d->material_ranges = from_source ? src->material_ranges : nullptr;
    d->face_normals = from_source ? src->face_normals : nullptr;
    d->vertex_normals = from_source ? src->vertex_normals : nullptr;
    d->strip_total = from_source ? src->strip_total : 0;
}

void set_transform(joan::d3d8::Enum state, const float* matrix) { dev()->SetTransform(state, matrix); }

// Texture-coordinate projection of material `mi` (+0x84 matrices, +0x88 stages);
// `on` = before the draw, otherwise it is undone after it.
bool projects_texture(const std::uint8_t* p, const std::uint8_t* mat, std::uint32_t mi) {
    return global<std::int8_t>(d::g_cfg_shadow_quality) == 2 && jo::view<float>(jo::as_material(mat)->u32_18) > 0.0f && jo::view<std::int32_t*>(jo::as_plane(p)->material_ids)[mi] != -1;
}
void texture_projection(const std::uint8_t* p, std::uint32_t mi, bool on) {
    const std::uint32_t stage = static_cast<std::uint32_t>(jo::view<std::int32_t*>(jo::as_plane(p)->material_ids)[mi]);
    if (stage <= 7) set_transform(kTsTexture0 + stage, on ? reinterpret_cast<const float*>(jo::view<std::uint8_t*>(jo::as_plane(p)->material_matrices) + mi * 64) : &img::global<const float>(kIdentity));
    dev()->SetTextureStageState(stage, kTssTextureTransformFlags, on ? 0x104 : 0);
    dev()->SetTextureStageState(stage, kTssTexCoordIndex, on ? stage | 0x20000 : stage);
}

// --- the replacement ------------------------------------------------------------------
struct Plane {
    void draw(void* ctx, std::int32_t pass, std::uint32_t arg2);
};

// [Graphics] Water=1 (render/water.hpp; not part of the original game): a waterfall model - a material
// textured "WaterFal*" (texteff.res Waterfall01 / Waterfall_mod) - reports where it falls when it is drawn.
// Planes found not to be waterfalls (nearly all): a direct-mapped memo (plane, its materials array) in front of
// water_fall_info, whose map is then left with the waterfalls only. A plane evicted by another is classified again.
struct NotFall {
    const void* plane;
    const void* materials;
};
NotFall g_not_fall[8192];

static void note_waterfall(std::uint8_t* p) {
    NotFall& memo = g_not_fall[(reinterpret_cast<std::uintptr_t>(p) >> 4) & 8191];
    const void* const materials = jo::view<void*>(jo::as_plane(p)->materials);
    if (memo.plane == p && memo.materials == materials) return;
    joan::render::WaterFallInfo& info = joan::render::water_fall_info(p);
    if (info.kind == 0) {
        memo = {p, materials};
        joan::render::water_fall_forget(p);
        return;
    }
    if (info.kind < 0) {
        bool fall = false;
        for (std::int32_t m = 0; m < jo::as_plane(p)->material_count && !fall; ++m) {
            const auto* const mat = reinterpret_cast<const jo::Material*>(jo::view<std::uint8_t*>(jo::as_plane(p)->materials) + m * sizeof(jo::Material));
            for (void* const tex : mat->textures) {
                if (!tex) continue;
                const char* const name = jo::as_texture(tex)->name;
                static const char kPrefix[] = "waterfal";
                bool match = true;
                for (int i = 0; kPrefix[i] && match; ++i) match = name[i] != 0 && (name[i] | 0x20) == kPrefix[i];
                if (match) fall = true;
            }
        }
        // Its bounds (local, the vertices' positions: the first 3 floats of every format but the skinned 7).
        const std::uint8_t* geo = p;
        if (jo::as_plane(geo)->vertex_count <= 0 && jo::view<std::uint8_t*>(jo::as_plane(p)->source)) geo = jo::view<std::uint8_t*>(jo::as_plane(p)->source);
        const std::int32_t fmt = jo::as_plane(geo)->vertex_format;
        const std::uint8_t* verts = nullptr;
        std::size_t stride = 0;
        switch (fmt) {
        case 1: verts = reinterpret_cast<const std::uint8_t*>(jo::as_plane(geo)->vertices_f1); stride = sizeof(jo::VertexF1); break;
        case 2: verts = reinterpret_cast<const std::uint8_t*>(jo::as_plane(geo)->vertices_f2); stride = sizeof(jo::VertexF2); break;
        case 3: verts = reinterpret_cast<const std::uint8_t*>(jo::as_plane(geo)->vertices_f3); stride = sizeof(jo::VertexF3); break;
        case 4: verts = reinterpret_cast<const std::uint8_t*>(jo::as_plane(geo)->vertices_f4); stride = sizeof(jo::VertexF4); break;
        case 5: verts = reinterpret_cast<const std::uint8_t*>(jo::as_plane(geo)->vertices_f5); stride = sizeof(jo::VertexF5); break;
        case 6: verts = reinterpret_cast<const std::uint8_t*>(jo::as_plane(geo)->vertices_f6); stride = sizeof(jo::VertexF6); break;
        case 8: verts = reinterpret_cast<const std::uint8_t*>(jo::as_plane(geo)->vertices_f8); stride = sizeof(jo::VertexF8); break;
        default: break;
        }
        const std::int32_t n = jo::as_plane(geo)->vertex_count;
        if (fall && (!verts || n <= 0)) return;  // geometry not there yet: classified on a later draw
        info.kind = fall ? 1 : 0;
        if (!fall) {
            memo = {p, materials};
            joan::render::water_fall_forget(p);  // `info` is gone
            return;
        }
        for (int a = 0; a < 3; ++a) {
            info.lo[a] = 1e30f;
            info.hi[a] = -1e30f;
        }
        for (std::int32_t i = 0; i < n; ++i) {
            float v[3];
            std::memcpy(v, verts + static_cast<std::size_t>(i) * stride, sizeof v);
            for (int a = 0; a < 3; ++a) {
                info.lo[a] = std::min(info.lo[a], v[a]);
                info.hi[a] = std::max(info.hi[a], v[a]);
            }
        }
    }
    if (info.kind != 1) return;
    // World footprint: the bounds' 8 corners through the plane's world matrix (Direct3D row vectors).
    void* const bone = jo::as_plane(p)->bones;
    const float* m = bone && jo::as_plane(p)->transformed == 0 ? bone_matrix(bone) : nullptr;
    joan::render::WaterRect r{1e30f, 1e30f, -1e30f, -1e30f, 1e30f};
    for (int c = 0; c < 8; ++c) {
        const float x = (c & 1) ? info.hi[0] : info.lo[0], y = (c & 2) ? info.hi[1] : info.lo[1], z = (c & 4) ? info.hi[2] : info.lo[2];
        float w[3] = {x, y, z};
        if (m)
            for (int a = 0; a < 3; ++a) w[a] = x * m[a] + y * m[4 + a] + z * m[8 + a] + m[12 + a];
        r.x0 = std::min(r.x0, w[0]);
        r.y0 = std::min(r.y0, w[1]);
        r.x1 = std::max(r.x1, w[0]);
        r.y1 = std::max(r.y1, w[1]);
        r.z = std::min(r.z, w[2]);
    }
    joan::render::water_fall_seen(p, r);
}

void Plane::draw(void* ctx, std::int32_t pass, std::uint32_t arg2) {
    c_draw.hit();
    std::uint8_t* const p = reinterpret_cast<std::uint8_t*>(this);

    // [Graphics] Water=1 (not part of the original game): the scene's modern water pass drew this surface.
    if (joan::render::water_replaced(this)) return;
    if (joan::render::water_params().mode == 1) note_waterfall(p);
    // A. Strips <-> lists switch (the index buffer depends on it).
    if (pass == 2 && jo::view<std::int8_t>(jo::as_plane(p)->skip_draw) == 1) return;
    if (jo::view<std::int8_t>(jo::as_plane(p)->use_strips) != 0 && !strips_on()) {
        jo::view<std::int8_t>(jo::as_plane(p)->use_strips) = 0;
        release_index_buffer(p);
    } else if (jo::view<std::int8_t>(jo::as_plane(p)->use_strips) == 0 && strips_on() && jo::view<std::int8_t>(jo::as_plane(p)->has_strips) != 0) {
        jo::view<std::int8_t>(jo::as_plane(p)->use_strips) = 1;
        release_index_buffer(p);
    }

    // B. Level of detail, buffers, per-pass preparation.
    if (jo::view<std::int8_t>(jo::as_plane(p)->lod_count) > 0) {
        void* lod = joan::orig::arg::as<void*>(joan::orig::as_plane(p)->lod(joan::orig::arg::mptr(ctx)));  // 0x005d4760
        void* level = nullptr;
        if (lod) level = joan::orig::arg::as<void*>(joan::orig::as_plane(p)->level(&lod, 1u));  // 0x005d45e0
        if (level) {
            joan::orig::as_plane(level)->draw(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass), 0u);  // Plane::draw of the chosen level
            return;
        }
    }
    joan::orig::as_plane(p)->upload();  // 0x005cfa10 create buffers
    if (jo::as_plane(p)->u32_fc == 0 && joan::orig::arg::as<int>(joan::orig::as_plane(p)->normals_pending()) != 0 && pass == 1) {
        jo::as_plane(p)->u32_fc = 1;
        joan::orig::as_plane(p)->build_tangents();  // 0x005d5770
    }
    if (joan::orig::arg::as<int>(joan::orig::as_plane(p)->in_pass(joan::orig::arg::u32(pass))) == 0) return;  // 0x005ce970
    bool default_material_built = false;
    alignas(alignof(void*)) std::uint8_t default_material[sizeof(jo::Material)];
    if (pass == 1 && jo::as_plane(p)->mode == 1) joan::orig::as_plane(p)->env_map();  // 0x005d7bf0
    const bool shares = jo::as_plane(p)->data_pending != 0;
    if (shares) borrow_geometry(p, true);

    // C. World matrices.
    void* const bone = jo::as_plane(p)->bones;
    set_transform(kTsWorld, bone && jo::as_plane(p)->transformed == 0 ? bone_matrix(bone) : &img::global<const float>(kIdentity));
    if (jo::as_plane(p)->vertex_format == 8 && global<std::uint32_t>(d::g_a52518) >= 2) {
        if (jo::as_plane(p)->bone_count >= 2)
            set_transform(kTsWorld1, bone_matrix(static_cast<std::uint8_t*>(bone) + sizeof(jo::Bone)));  // bone + 0xa0 (x86)
        else
            set_transform(kTsWorld1, &img::global<const float>(kIdentity));
    }

    // D. Vertex / index buffers.
    const char vertex_type = jo::view<char>(jo::as_plane(p)->vertex_format);
    switch (jo::as_plane(p)->buffer_mode) {
    case 0: {
        if (shares) {
            const jo::Plane* const src = jo::as_plane(p)->source;
            auto borrow = [](void*& field, void* from) {  // the source's vertex / index buffer
                if (field != nullptr) return;
                field = from;
                if (field) com_add_ref(field);
            };
            borrow(jo::view<void*>(jo::as_plane(p)->vertex_buffer), jo::view<void*>(src->vertex_buffer));
            borrow(jo::view<void*>(jo::as_plane(p)->index_buffer), jo::view<void*>(src->index_buffer));
        }
        const auto* const format = joan::orig::arg::as<const std::uint8_t*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(vertex_type))));  // 0x00600200
        joan::d3d8::HRESULT hr = dev()->SetStreamSource(0, jo::view<joan::d3d8::IDirect3DVertexBuffer8*>(jo::as_plane(p)->vertex_buffer), jo::as_vertex_format(format)->stride);
        if (hr != 0)
            joan::orig::err_run(&img::global<const char>(d::s_Cannot_set_vertex_buffer_s), joan::orig::arg::as<const char*>(joan::orig::as_sys_en(img::obj(kD3D))->error_string(joan::orig::arg::i32(hr))));
        hr = dev()->SetIndices(jo::view<joan::d3d8::IDirect3DIndexBuffer8*>(jo::as_plane(p)->index_buffer), 0);
        if (hr != 0)
            joan::orig::err_run(&img::global<const char>(d::s_Cannot_set_index_buffer_s), joan::orig::arg::as<const char*>(joan::orig::as_sys_en(img::obj(kD3D))->error_string(joan::orig::arg::i32(hr))));
        joan::orig::as_sys_en(img::obj(kD3D))->set_vertex_type(static_cast<char>(joan::orig::arg::i32(vertex_type)));  // 0x006024e0 vertex shader
        break;
    }
    case 1: {
        const std::uint8_t* const q = shares ? jo::view<std::uint8_t*>(jo::as_plane(p)->source) : p;
        const std::uint32_t count = jo::view<std::uint32_t>(jo::as_plane(q)->vertex_count);                          // pushed first (3rd arg below)
        void* const vertices = joan::orig::arg::as<void*>(joan::orig::as_plane(q)->buffer(joan::orig::arg::mptr(0)));                         // 0x005cdae0, ret 4
        // The stream cache (game/unknown_d3dsys.cpp) serves only these two fills: a model's own vertex array
        // and triangle list. Other fills (2D screens, text, effects) rebuild temporary arrays at the same
        // address with new contents.
        const joan::stream_cache_api::ModelFill model_fill;
        joan::orig::as_sys_en(img::obj(kD3D))->fill_vertices(static_cast<char>(joan::orig::arg::i32(jo::view<char>(jo::as_plane(q)->vertex_format))), joan::orig::arg::mptr(vertices), joan::orig::arg::u32(count));                  // 0x00600490, ret 0xc
        if (jo::view<std::int8_t>(jo::as_plane(p)->use_strips) != 0 && strips_on())
            joan::orig::as_sys_en(img::obj(kD3D))->fill_indices(joan::orig::arg::mptr(jo::view<void*>(jo::as_plane(q)->strip_indices)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_plane(q)->strip_index_count)), 0u);  // 0x00600510
        else
            joan::orig::as_sys_en(img::obj(kD3D))->fill_indices(joan::orig::arg::mptr(jo::view<void*>(jo::as_plane(q)->triangles)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_plane(q)->triangle_count) * 3), 0u);
        break;
    }
    default:
        break;
    }

    // E. Stencil reference (shadow volumes).
    const std::int32_t stencil_bits = global<std::int32_t>(d::g_stencil_bits);
    if (stencil_bits >= 4 && joan::orig::arg::as<int>(joan::orig::as_scene(jo::as_camera(ctx)->scene)->shadows_on()) != 0) {  // 0x005ee0d0
        const DWORD ref = joan::orig::arg::as<int>(joan::orig::as_plane(p)->stencil_value()) != 0 ? 1u << ((stencil_bits - 1) & 31) : 0u;  // 0x005e4350
        dev()->SetRenderState(kRsStencilRef, ref);
    }

    // F. One draw per run of triangles sharing a material.
    const std::uint16_t* const tri_material = jo::view<std::uint16_t*>(jo::as_plane(p)->triangle_materials);
    const std::uint16_t* const strip_length = jo::view<std::uint16_t*>(jo::as_plane(p)->strip_lengths);
    std::int32_t strip_start = 0, strip_material = 0;  // index offset of strip_material's strips
    std::uint32_t end = 0;
    for (std::uint32_t first = 0; first < jo::view<std::uint32_t>(jo::as_plane(p)->triangle_count); first = end) {
        const std::uint16_t mi = tri_material[first];
        if (jo::as_plane(p)->triangle_count <= 1 || jo::as_plane(p)->material_count <= 1) {
            end = jo::view<std::uint32_t>(jo::as_plane(p)->triangle_count);
        } else if (const auto* const ends = jo::view<std::uint16_t*>(jo::as_plane(p)->material_first)) {
            end = ends[mi];
        } else {
            for (end = first + 1; end < jo::view<std::uint32_t>(jo::as_plane(p)->triangle_count) && tri_material[end] == mi; ++end) {
            }
        }
        std::uint32_t min_vertex = 0, max_vertex = jo::view<std::uint32_t>(jo::as_plane(p)->vertex_count) - 1;
        if (const auto* const ranges = jo::view<std::uint16_t*>(jo::as_plane(p)->material_ranges)) {
            min_vertex = ranges[mi * 2];
            max_vertex = ranges[mi * 2 + 1];
        }
        std::uint8_t* mat;
        if (static_cast<std::int32_t>(mi) < jo::as_plane(p)->material_count) {
            mat = jo::view<std::uint8_t*>(jo::as_plane(p)->materials) + mi * static_cast<std::int32_t>(sizeof(jo::Material));
        } else {
            if (!default_material_built) {
                joan::orig::as_material(default_material)->construct();  // Material::Material 0x005d52a0
                default_material_built = true;
            }
            mat = default_material;
        }
        if (joan::orig::arg::as<int>(joan::orig::as_sys_en(img::obj(kD3D))->pass_filter(joan::orig::arg::u32(jo::as_material(mat)->pass_mask), joan::orig::arg::u32(pass))) == 0) continue;  // 0x00600b30

        // [Graphics] Water=1 (not part of the original game): a river ("river*") or falling sheet ("WaterFal*")
        // material is drawn with the water shader (render/water.hpp water_river_begin), its scroll slowed.
        int river = 0, river_kind = 0;
        if (joan::render::water_params().mode == 1) {
            for (int s = 0; s < 4 && river == 0; ++s) {
                void* const tex = jo::as_material(mat)->textures[s];
                if (!tex) continue;
                if (const int kind = joan::render::water_texture_kind(jo::as_texture(tex)->name)) river = s + 1, river_kind = kind;
            }
        }

        // Material and render state.
        alignas(4) std::uint8_t d3d_material[0x44];
        joan::orig::as_material(mat)->to_d3d(joan::orig::arg::mptr(d3d_material));  // 0x005d56f0
        dev()->SetMaterial(d3d_material);
        for (DWORD stage = 0; stage < 4; ++stage) {
            void* const texture = jo::as_material(mat)->textures[stage];
            dev()->SetTexture(stage, texture ? jo::view<joan::d3d8::IDirect3DBaseTexture8*>(jo::as_texture(texture)->d3d_base) : nullptr);
        }
        joan::orig::as_sys_en(img::obj(kD3D))->set_blend(joan::orig::arg::u32(jo::as_material(mat)->blend));  // 0x005f8250
        joan::orig::as_sys_en(img::obj(kD3D))->set_texture_mode(joan::orig::arg::u32(jo::as_material(mat)->texture_mode));  // 0x005fa250
        joan::orig::as_sys_en(img::obj(kD3D))->set_filter(joan::orig::arg::u32(jo::as_material(mat)->filter));  // 0x005fec10
        if (jo::as_plane(p)->flags_104 != 0) {
            std::uint32_t factor = 0;
            for (std::uint32_t b = 0; b < 4; ++b) {
                const std::int32_t v = static_cast<std::int32_t>(jo::as_plane(p)->colour_[b]) * reinterpret_cast<const std::uint8_t*>(&jo::as_material(mat)->colour)[b] / 255;
                factor |= static_cast<std::uint32_t>(v & 0xff) << (b * 8);
            }
            joan::orig::as_sys_en(img::obj(kD3D))->set_texture_factor(joan::orig::arg::u32(factor));  // 0x005feb50 texture factor
        }
        // [Graphics] GrassBatching (render/grass_batch.hpp): the material colour of this run's draw, for the replays'
        // fade factor.
        if (joan::render::grass_recording()) joan::render::grass_record_colour(jo::view<std::uint32_t>(jo::as_material(mat)->colour));
        joan::orig::as_sys_en(img::obj(kD3D))->set_flags(joan::orig::arg::u32(joan::orig::arg::as<std::uint32_t>(joan::orig::as_plane(p)->flags()) | joan::orig::arg::as<std::uint32_t>(joan::orig::as_camera(ctx)->flags())));  // 0x00602080
        joan::orig::as_sys_en(img::obj(kD3D))->set_raster(joan::orig::arg::u32(jo::as_material(mat)->raster));  // 0x006026a0
        if (void* const object = global<void*>(d::g_vs_cache)) {
            (void)joan::orig::as_vs_cache(object)->field20();      // 0x0047b5e0: this + 0x20
            jo::as_vs_cache(object)->slider_factors[11] = 1.0f;  // field20() + 0x5c
        }

        // Animated textures.
        std::uint32_t set = 0;
        const std::uint32_t anim = jo::as_material(mat)->texture_mode;
        if ((anim & 0xff000000u) == 0x01000000u) {
            arg2 = anim & 0xffff;
            set = (anim >> 16) & 0xff;
        } else {
            arg2 = 0;
        }
        if (arg2 != 0) {
            animate_textures(p, mat, arg2, set);
            if (jo::as_plane(p)->mode == 2) {
                v3d(kPointTransform3, img::obj(d::g_a525a3), jo::as_plane(p)->origin, joan::orig::arg::as<void*>(joan::orig::as_camera(ctx)->matrix_ee()));  // 0x00616ec0
                global<float>(d::g_a525a7) = -global<float>(d::g_a525a7);
            }
        }
        if (river) joan::render::water_flow_scale() = joan::render::water_params().flow;
        joan::orig::as_sys_en(img::obj(kD3D))->apply(joan::orig::arg::u32(arg2));  // 0x006014d0
        joan::render::water_flow_scale() = 1.0f;
        if ((joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(img::obj(kD3D))->flags()) & 0xe) != 0)                   // 0x006020f0
            joan::orig::as_camera(ctx)->apply_fog(joan::orig::arg::u32(joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(img::obj(kD3D))->overlay_stage())));    // 0x00616b80 (0x00601470)
        else
            joan::orig::as_fog(jo::as_camera(ctx)->fog)->off();  // 0x005c09a0
        joan::orig::as_camera(ctx)->m_617d50();                       // 0x00617d50

        const bool projected = projects_texture(p, mat, mi);
        if (projected) texture_projection(p, mi, true);

        // Projected (light / caustic) texture matrices of the animated set.
        std::uint8_t* info = nullptr;
        if (arg2 != 0) {
            info = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_db(img::obj(kAnimDb))->entry(joan::orig::arg::u32(static_cast<std::uint16_t>(arg2))));
            for (std::int32_t k = 0; k < jo::view<std::int16_t>(jo::as_tex_script_rec(info)->stage_count[set]); ++k) {
                if (jo::view<std::int8_t>(jo::as_tex_script_rec(info)->object_mode[set * 4 + k]) != 1) continue;
                float vec[3] = {0.0f, 0.0f, 0.0f};
                float m[16], inv[16], t[16];
                if (bone && jo::as_plane(p)->transformed == 0)
                    v3d(kMatrixMultiply3, m, bone_matrix(bone), joan::orig::arg::as<const float*>(joan::orig::as_camera(ctx)->view()));  // 0x00616f20
                else
                    std::memcpy(m, joan::orig::arg::as<const float*>(joan::orig::as_camera(ctx)->matrix_22e()), sizeof m);  // 0x00616f50
                std::memcpy(t, &img::global<const float>(kIdentity), sizeof t);
                v3d(kMatrixInverse, inv, m);
                v3d(kPointTransform2, vec, m);
                t[0] = jo::as_tex_script_rec(info)->scales_u[set * 4 + k];   // _11
                t[5] = jo::as_tex_script_rec(info)->scales_v[set * 4 + k];   // _22
                t[12] = jo::as_tex_script_rec(info)->scroll_offsets_u[set * 4 + k];  // _41
                t[13] = jo::as_tex_script_rec(info)->scroll_offsets_v[set * 4 + k];  // _42
                v3d(kMatrixMultiply2, inv, t);
                v3d(kPointTransform2, vec, inv);
                if (k <= 3) set_transform(kTsTexture0 + k, inv);
            }
        }

        // Skinning matrices as vertex shader constants (c17.., 3 rows per bone).
        if (jo::as_plane(p)->vertex_format == 7) {
            alignas(4) std::uint8_t constants[0x490];
            joan::orig::vector(joan::orig::arg::mptr(static_cast<void*>(constants)), 0x10, 0x49, joan::orig::arg::mptr(fn::code_value(fn::identity_5bc990)));  // `vector constructor iterator'
            const std::int32_t bones = jo::as_plane(p)->bone_count;
            for (std::int32_t base = 0; base < bones; base += 24) {
                std::int32_t n = 0;
                std::uint8_t* dst = constants;
                for (std::int32_t b = base; n < 24 && b < bones; ++b, ++n, dst += 0x30)
                    v3d(kMatrixTranspose, dst, static_cast<std::uint8_t*>(bone) + b * static_cast<std::int32_t>(sizeof(jo::Bone)) + offsetof(jo::Bone, matrix_));  // bone + b * 0xa0 + 0x58 (x86)
                dev()->SetVertexShaderConstant(base * 3 + 0x11, constants, n * 3);
            }
        }

        // Strip index offset of this material (strips are stored per material).
        const bool strips = jo::view<std::int8_t>(jo::as_plane(p)->use_strips) != 0 && strips_on();
        if (strips) {
            if (strip_material > static_cast<std::int32_t>(mi)) strip_start = strip_material = 0;
            for (; strip_material < static_cast<std::int32_t>(mi); ++strip_material) {
                const std::int32_t c = strip_length[strip_material];
                strip_start += c > 0 ? c + 2 : 0;
            }
        }

        if (river) joan::render::water_river_begin(river, river_kind == 2);

        // The draw.
        const std::uint32_t vertex_count = max_vertex - min_vertex + 1;
        switch (jo::as_plane(p)->buffer_mode) {
        case 0:
            if (strips) {
                dev()->DrawIndexedPrimitive(5, min_vertex, vertex_count, strip_start, strip_length[mi]);
                joan::orig::as_sys_en(img::obj(kD3D))->count(0u, joan::orig::arg::u32(static_cast<std::uint32_t>(strip_length[mi])));  // 0x005d2b40 statistics
            } else {
                dev()->DrawIndexedPrimitive(4, min_vertex, vertex_count, first * 3, end - first);
                joan::orig::as_sys_en(img::obj(kD3D))->count(0u, joan::orig::arg::u32(end - first));
            }
            break;
        case 1:
            if (strips) {
                if (strip_length[mi] > 0) joan::orig::as_sys_en(img::obj(kD3D))->draw_indexed(5, joan::orig::arg::u32(strip_start), joan::orig::arg::u32(static_cast<std::uint32_t>(strip_length[mi])));  // 0x00600a90
            } else if (jo::view<std::int8_t>(jo::as_plane(p)->prim_type) == 0) {
                joan::orig::as_sys_en(img::obj(kD3D))->draw_indexed(4, joan::orig::arg::u32(first * 3), joan::orig::arg::u32(end - first));
            } else {
                // Mixed primitive types: runs of lists (4), lines (2), points (1).
                const std::uint16_t* const indices = jo::view<std::uint16_t*>(jo::as_plane(p)->triangles);
                auto type_of = [&](std::uint32_t tri) { return joan::orig::arg::as<std::int32_t>(joan::orig::triangle_kind(joan::orig::arg::mptr(indices), joan::orig::arg::i32(tri))); };
                std::uint32_t b = 0;
                for (std::uint32_t a = first; a < end; a = b) {
                    const std::int32_t type = type_of(a);
                    for (b = a + 1; b < end && type_of(b) == type; ++b) {
                    }
                    if (type == 4) {
                        joan::orig::as_sys_en(img::obj(kD3D))->draw_indexed(joan::orig::arg::i32(type), joan::orig::arg::u32(a * 3), joan::orig::arg::u32(b - a));
                    } else if (type == 2) {
                        for (std::uint32_t c = a; c < b; ++c) joan::orig::as_sys_en(img::obj(kD3D))->draw_indexed(joan::orig::arg::i32(type), joan::orig::arg::u32(c * 3), 1u);
                    } else if (type == 1) {
                        std::uint32_t e = 0;
                        for (std::uint32_t c = a; c < b; c = e) {
                            const std::uint16_t idx = indices[c * 3];
                            std::uint16_t next = static_cast<std::uint16_t>(idx + 1);
                            for (e = c + 1; e < b && indices[e * 3] == next; ++e, ++next) {
                            }
                            joan::orig::as_sys_en(img::obj(kD3D))->draw(joan::orig::arg::i32(type), joan::orig::arg::u32(static_cast<std::uint32_t>(idx)), joan::orig::arg::u32(e - c));  // 0x00600a00
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
        if (river) joan::render::water_river_end();

        // Undo the per-material texture transforms.
        if (arg2 != 0) {
            info = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_db(img::obj(kAnimDb))->entry(joan::orig::arg::u32(static_cast<std::uint16_t>(arg2))));
            for (std::int32_t k = 0; k < jo::view<std::int16_t>(jo::as_tex_script_rec(info)->stage_count[set]); ++k)
                if (jo::view<std::int8_t>(jo::as_tex_script_rec(info)->object_mode[set * 4 + k]) == 1 && k <= 3) set_transform(kTsTexture0 + k, &img::global<const float>(kIdentity));
        }
        if (projects_texture(p, mat, mi)) texture_projection(p, mi, false);
    }

    // G. Cleanup.
    if (shares) borrow_geometry(p, false);
    if (default_material_built) joan::orig::as_material(default_material)->dtor();  // Material::~Material 0x005d5360
}

// 0x005d2b80, thiscall, ret 0xc: the following copies of an instance batch
// (ActorDef::draw_instances) - one material, the device state left by the first
// copy's Plane::draw; only the world matrix / skinning constants and the draw.
struct PlaneSingle {
    void draw(void* ctx, std::int32_t pass, std::uint32_t arg2);
};
JOAN_COUNTER(c_draw_single, "Plane::draw_single");
JOAN_COUNTER(c_draw_single_fix, "Plane::draw_single fix/uninit (material index out of range)");

void PlaneSingle::draw(void* /*ctx*/, std::int32_t pass, std::uint32_t /*arg2*/) {
    c_draw_single.hit();
    std::uint8_t* const p = reinterpret_cast<std::uint8_t*>(this);
    if (pass == 2 && jo::view<std::int8_t>(jo::as_plane(p)->skip_draw) == 1) return;
    // The material of the first triangle. Out of range, the original reads an
    // uninitialised pointer ([ebp-0x28], only used as mat+0x40 for 0x00600b30): joan.exe's body
    // no longer exists (stage S5), so this version with "no material" = no draw (0x00600b30 is a pure compare, not called then); the
    // create-buffers call, the borrow / restore of the shared geometry and the world matrix
    // happen as in the original.
    const bool shares = jo::as_plane(p)->data_pending != 0;
    const std::uint16_t first_material = jo::view<std::uint16_t*>(jo::as_plane(shares ? jo::view<std::uint8_t*>(jo::as_plane(p)->source) : p)->triangle_materials)[0];
    if (static_cast<std::int32_t>(first_material) >= jo::as_plane(p)->material_count) {
        c_draw_single_fix.hit();
    }
    joan::orig::as_plane(p)->upload();  // 0x005cfa10 create buffers
    if (shares) borrow_geometry(p, true);
    const std::uint16_t mi = jo::view<std::uint16_t*>(jo::as_plane(p)->triangle_materials)[0];
    if (jo::as_plane(p)->vertex_format != 7) {
        void* const bone = jo::as_plane(p)->bones;
        set_transform(kTsWorld, bone && jo::as_plane(p)->transformed == 0 ? bone_matrix(bone) : &img::global<const float>(kIdentity));
    }
    std::int32_t strip_start = 0, strip_material = 0;
    const std::uint32_t first = 0, end = jo::view<std::uint32_t>(jo::as_plane(p)->triangle_count);
    std::uint32_t min_vertex = 0, max_vertex = jo::view<std::uint32_t>(jo::as_plane(p)->vertex_count) - 1;
    if (const auto* const ranges = jo::view<std::uint16_t*>(jo::as_plane(p)->material_ranges)) {
        min_vertex = ranges[mi * 2];
        max_vertex = ranges[mi * 2 + 1];
    }
    // fix/uninit: mi >= material count (+0x64, read here like the original's jge) -> no material.
    const bool has_material = static_cast<std::int32_t>(mi) < jo::as_plane(p)->material_count;
    const std::uint8_t* const mat = jo::view<std::uint8_t*>(jo::as_plane(p)->materials) + mi * static_cast<std::int32_t>(sizeof(jo::Material));
    if (has_material && joan::orig::arg::as<int>(joan::orig::as_sys_en(img::obj(kD3D))->pass_filter(joan::orig::arg::u32(jo::as_material(mat)->pass_mask), joan::orig::arg::u32(pass))) != 0) {  // 0x00600b30
        if (jo::as_plane(p)->vertex_format == 7) {
            alignas(4) std::uint8_t constants[0x490];
            joan::orig::vector(joan::orig::arg::mptr(static_cast<void*>(constants)), 0x10, 0x49, joan::orig::arg::mptr(fn::code_value(fn::identity_5bc990)));
            const std::int32_t bones = jo::as_plane(p)->bone_count;
            for (std::int32_t base = 0; base < bones; base += 24) {
                std::int32_t n = 0;
                std::uint8_t* dst = constants;
                for (std::int32_t b = base; n < 24 && b < bones; ++b, ++n, dst += 0x30)
                    v3d(kMatrixTranspose, dst, jo::as_plane(p)->bones[b].matrix_);
                dev()->SetVertexShaderConstant(base * 3 + 0x11, constants, n * 3);
            }
        }
        const std::uint16_t* const strip_length = jo::view<std::uint16_t*>(jo::as_plane(p)->strip_lengths);
        const bool strips = jo::view<std::int8_t>(jo::as_plane(p)->use_strips) != 0 && strips_on();
        if (strips) {
            if (strip_material > static_cast<std::int32_t>(mi)) strip_start = strip_material = 0;
            for (; strip_material < static_cast<std::int32_t>(mi); ++strip_material) {
                const std::int32_t c = strip_length[strip_material];
                strip_start += c > 0 ? c + 2 : 0;
            }
        }
        const std::uint32_t vertex_count = max_vertex - min_vertex + 1;
        switch (jo::as_plane(p)->buffer_mode) {
        case 0:
            if (strips) {
                dev()->DrawIndexedPrimitive(5, min_vertex, vertex_count, strip_start, strip_length[mi]);
                joan::orig::as_sys_en(img::obj(kD3D))->count(0u, joan::orig::arg::u32(static_cast<std::uint32_t>(strip_length[mi])));
            } else {
                dev()->DrawIndexedPrimitive(4, min_vertex, vertex_count, first * 3, end - first);
                joan::orig::as_sys_en(img::obj(kD3D))->count(0u, joan::orig::arg::u32(end - first));
            }
            break;
        case 1:
            if (strips) {
                if (strip_length[mi] > 0) joan::orig::as_sys_en(img::obj(kD3D))->draw_indexed(5, joan::orig::arg::u32(strip_start), joan::orig::arg::u32(static_cast<std::uint32_t>(strip_length[mi])));
            } else if (jo::view<std::int8_t>(jo::as_plane(p)->prim_type) == 0) {
                joan::orig::as_sys_en(img::obj(kD3D))->draw_indexed(4, joan::orig::arg::u32(first * 3), joan::orig::arg::u32(end - first));
            } else {
                const std::uint16_t* const indices = jo::view<std::uint16_t*>(jo::as_plane(p)->triangles);
                auto type_of = [&](std::uint32_t tri) { return joan::orig::arg::as<std::int32_t>(joan::orig::triangle_kind(joan::orig::arg::mptr(indices), joan::orig::arg::i32(tri))); };
                std::uint32_t b = 0;
                for (std::uint32_t a = first; a < end; a = b) {
                    const std::int32_t type = type_of(a);
                    for (b = a + 1; b < end && type_of(b) == type; ++b) {
                    }
                    if (type == 4) {
                        joan::orig::as_sys_en(img::obj(kD3D))->draw_indexed(joan::orig::arg::i32(type), joan::orig::arg::u32(a * 3), joan::orig::arg::u32(b - a));
                    } else if (type == 2) {
                        for (std::uint32_t c = a; c < b; ++c) joan::orig::as_sys_en(img::obj(kD3D))->draw_indexed(joan::orig::arg::i32(type), joan::orig::arg::u32(c * 3), 1u);
                    } else if (type == 1) {
                        std::uint32_t e = 0;
                        for (std::uint32_t c = a; c < b; c = e) {
                            const std::uint16_t idx = indices[c * 3];
                            std::uint16_t next = static_cast<std::uint16_t>(idx + 1);
                            for (e = c + 1; e < b && indices[e * 3] == next; ++e, ++next) {
                            }
                            joan::orig::as_sys_en(img::obj(kD3D))->draw(joan::orig::arg::i32(type), joan::orig::arg::u32(static_cast<std::uint32_t>(idx)), joan::orig::arg::u32(e - c));
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    if (shares) borrow_geometry(p, false);
}

JOAN_REPLACE("en3d/plane.cpp", "Plane::render", 0x005d0570, &Plane::draw);
JOAN_REPLACE("en3d/plane.cpp", "Plane::render_material_fast", 0x005d2b80, &PlaneSingle::draw);

}  // namespace plane_draw
}  // namespace

// ---------------------------------------- from modules/en3d_b117_01.cpp (en3d/b117_01)
//
//    - Plane::submit_5d35b0 0x005d35b0 (thiscall (Reorder* list, Camera* camera, int pass), ret 0xc,
//      eax undefined - every caller ignores it). Callers: SceneS::draw 0x005f03b0 (sector planes
//      +0x38 / +0x40), Actor::draw_entry 0x005abc50, and itself (level of detail) - all through
//      thunk 0x00401cc1. The deferred counterpart of Plane::draw 0x005d0570: instead of driving
//      the Direct3D device it records one draw span per material run into the reorder list
//      (en3d/reorder.cpp, 0x005e79f0 .. 0x005e7f70), which sorts and draws them later (this is the
//      path that receives the projected water / mirror textures). This function itself makes NO
//      device calls; it only calls en3d helpers through their thunks.
//      Flow: pass 2 + skip flag +0x09 -> return; level of detail (+0xe4 > 0: 0x005d4760(camera),
//      0x005d45e0(&lod, 1), recursive submit of the chosen level); 0x005cfa10 buffers; normals
//      (+0xfc, 0x005d5f30, pass 1 -> 0x005d5770); 0x005ce970(pass) == 0 -> return; pass 1 + kind
//      +0x110 == 1 -> 0x005d7bf0; world bone (+0x18 == 0: bone +0x00 and bone count byte +0x06,
//      else 0 / 0); list count (0x005e7bb0: +0x70, or [+0x74]+8 + 9 for type 4 with a positive
//      count); current record +4 = vertex type +0x04; first use (+0xb8 == 0): clear the
//      per-material list slots (inline +0xbc for <= 4 materials, else the lazily allocated
//      +0xcc block of +0x68 * 4 bytes, plane.cpp line [0xa173e4] + 0x7f); 0x005cdae0(0) vertex
//      data; then per run of triangles sharing a material: end of the run (whole list, +0xb0, or
//      scan of +0x4c), vertex range (whole buffer, +0xb4 pairs, or [+0x10] .. 0), material
//      (+0x6c[mi] or a lazily built default Material on the stack, destroyed at the end),
//      0x00600b30(material +0x40, pass) filter, list slot (0x005e79f0) -> record +0xc, flags
//      0x005d5f60 -> record +8, and by buffer mode +0x05: 0 = vertex / index buffer spans
//      (0x005e7f70: strips when +0x50 and [0xa678b0], else a triangle list), 1 = system memory
//      spans (0x005e7ec0 strips / list; mixed primitive types +0x08: runs by 0x005d2aa0, lists
//      (4) in one span, lines (2) one span each, points (1) one 0x005e7e70 each).
namespace {
namespace en3d_b117_01 {


constexpr img::Addr kPlaneCpp(d::s_d_ww_en3d_plane_cpp);  // "d:\ww\en3d\plane.cpp"

JOAN_COUNTER(c_submit, "Plane::submit_5d35b0");

// Strip length term of the per-material strip table: n > 0 ? n + 2 : 0.
std::int32_t strip_term(std::uint16_t n) { return n != 0 ? static_cast<std::int32_t>(n) + 2 : 0; }

struct H {
    // Current record of the reorder list (0x005e7b80).
    static jo::BatchKey* record(void* list) { return jo::as_batch_key(joan::orig::as_reorder(list)->state()); }

    void submit(void* list, void* camera, std::int32_t pass) {
        c_submit.hit();
        if (pass == 2 && jo::view<std::int8_t>(jo::self<jo::Plane>(this)->skip_draw) == 1) return;

        // Level of detail.
        if (jo::view<std::int8_t>(jo::self<jo::Plane>(this)->lod_count) > 0) {
            void* lod = joan::orig::arg::as<void*>(joan::orig::as_plane(this)->lod(joan::orig::arg::mptr(camera)));  // 0x005d4760
            void* level = nullptr;
            if (lod != nullptr) level = joan::orig::arg::as<void*>(joan::orig::as_plane(this)->level(&lod, 1u));  // 0x005d45e0
            if (level != nullptr) {
                joan::orig::as_plane(level)->submit_5d35b0(joan::orig::arg::mptr(list), joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass));  // 0x005d35b0
                return;
            }
        }
        // (dead switch on +0x70 storing 0xa..0x11 into an unread local: no side effects)
        joan::orig::as_plane(this)->upload();  // 0x005cfa10
        if (jo::self<jo::Plane>(this)->u32_fc == 0 && joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->normals_pending()) != 0 && pass == 1) {  // 0x005d5f30
            jo::self<jo::Plane>(this)->u32_fc = 1;
            joan::orig::as_plane(this)->build_tangents();  // 0x005d5770
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->in_pass(joan::orig::arg::u32(pass))) == 0) return;  // 0x005ce970

        std::uint32_t slot_default = 0;  // [ebp-0x68]
        bool built = false;              // [ebp-0x6c]
        alignas(alignof(void*)) std::uint8_t default_material[sizeof(jo::Material)];  // [ebp-0x64]

        if (pass == 1 && jo::self<jo::Plane>(this)->mode == 1) joan::orig::as_plane(this)->env_map();  // 0x005d7bf0

        jo::Bone* bone = nullptr;  // [ebp-0x70]
        std::uint32_t bones = 0;   // [ebp-0x74]
        if (jo::self<jo::Plane>(this)->transformed == 0) {
            bone = jo::self<jo::Plane>(this)->bones;

            bones = jo::self<jo::Plane>(this)->bone_count;
        }
        if (jo::self<jo::Plane>(this)->list_kind == 4 && jo::self<jo::Plane>(this)->owner->kind > 0)
            joan::orig::as_reorder(list)->set_count(joan::orig::arg::i32(jo::self<jo::Plane>(this)->owner->kind + 9));  // 0x005e7bb0
        else
            joan::orig::as_reorder(list)->set_count(joan::orig::arg::i32(jo::self<jo::Plane>(this)->list_kind));
        record(list)->vertex_format = jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format);

        if (jo::self<jo::Plane>(this)->list_slots_ready == 0) {
            if (jo::self<jo::Plane>(this)->material_count <= 4) {
                std::memset(static_cast<void*>(jo::self<jo::Plane>(this)->list_slots_inline), 0, joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->material_count) << 2));  // memset
            } else {
                if (jo::view<void*>(jo::self<jo::Plane>(this)->list_slots) == nullptr) {
                    const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_a173e4)) + 0x7f;
                    jo::view<void*>(jo::self<jo::Plane>(this)->list_slots) = joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->material_capacity) << 2), joan::orig::arg::str(img::str(kPlaneCpp)), joan::orig::arg::i32(line));  // 0x0058e7b0
                }
                std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Plane>(this)->list_slots)), 0, joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->material_count) << 2));
            }
            jo::self<jo::Plane>(this)->list_slots_ready = 1;
        }

        void* const vertices = joan::orig::arg::as<void*>(joan::orig::as_plane(this)->buffer(joan::orig::arg::mptr(0)));  // 0x005cdae0 -> [ebp-0x78]
        std::int32_t strip_start = 0;     // [ebp-0x84]
        std::int32_t strip_material = 0;  // [ebp-0x88]
        std::uint32_t end = 0;            // [ebp-0x80]
        for (std::uint32_t first = 0; first < jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_count); first = end) {  // [ebp-0x7c]
            const std::uint16_t mi = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangle_materials)[first];  // [ebp-0x8c]
            const std::uint32_t mi32 = mi;
            if (jo::self<jo::Plane>(this)->triangle_count <= 1 || jo::self<jo::Plane>(this)->material_count <= 1) {
                end = jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_count);
            } else if (jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->material_first) != nullptr) {
                end = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->material_first)[mi32];
            } else {
                for (end = first + 1; end < jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_count) && jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangle_materials)[end] == mi; ++end) {
                }
            }
            std::uint32_t lo, hi;  // [ebp-0x90] / [ebp-0x94]
            if (jo::self<jo::Plane>(this)->triangle_count <= 1 || jo::self<jo::Plane>(this)->material_count <= 1) {
                lo = 0;
                hi = jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count) - 1;
            } else if (jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->material_ranges) != nullptr) {
                lo = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->material_ranges)[mi32 * 2];
                hi = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->material_ranges)[mi32 * 2 + 1];
            } else {
                lo = jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count);
                hi = 0;
            }
            std::uint8_t* mat;  // [ebp-0x98]
            if (static_cast<std::int32_t>(mi32) < jo::self<jo::Plane>(this)->material_count) {
                mat = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->materials) + mi32 * static_cast<std::int32_t>(sizeof(jo::Material));
            } else {
                if (!built) {
                    joan::orig::as_material(default_material)->construct();  // Material::Material 0x005d52a0
                    built = true;
                }
                mat = default_material;
            }
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sys_en(img::obj(kD3D))->pass_filter(joan::orig::arg::u32(jo::as_material(mat)->pass_mask), joan::orig::arg::u32(pass))) == 0) continue;  // 0x00600b30

            // List slot of the material.
            if (static_cast<std::int32_t>(mi32) < jo::self<jo::Plane>(this)->material_count) {
                std::uint32_t* const slots = jo::self<jo::Plane>(this)->material_count <= 4
                                                 ? jo::self<jo::Plane>(this)->list_slots_inline
                                                 : jo::view<std::uint32_t*>(jo::self<jo::Plane>(this)->list_slots);  // [ebp-0x9c]
                slots[mi32] = joan::orig::arg::as<std::uint32_t>(joan::orig::as_reorder(list)->slot_for(joan::orig::arg::i32(slots[mi32]), static_cast<void*>(mat)));  // 0x005e79f0
                jo::BatchKey* const rec = record(list);
                jo::view<std::uint32_t>(rec->material) = slots[mi32];
            } else {
                slot_default = joan::orig::arg::as<std::uint32_t>(joan::orig::as_reorder(list)->slot_for(joan::orig::arg::i32(slot_default), static_cast<void*>(default_material)));
                jo::BatchKey* const rec = record(list);
                jo::view<std::uint32_t>(rec->material) = slot_default;
            }
            {
                const std::uint32_t flags = joan::orig::arg::as<std::uint32_t>(joan::orig::as_plane(this)->flags());  // 0x005d5f60
                jo::BatchKey* const rec = record(list);
                rec->states = flags;
            }

            const std::uint8_t mode = jo::self<jo::Plane>(this)->buffer_mode;
            if (mode != 0 && mode != 1) continue;
            const bool strips = jo::view<std::int8_t>(jo::self<jo::Plane>(this)->use_strips) != 0 && global<std::uint32_t>(d::g_strips_on) != 0;
            if (strips) {
                jo::view<std::uint32_t>(record(list)->prim) = 5;
                joan::orig::as_reorder(list)->find_or_insert(static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x005e7ce0
                if (strip_material > static_cast<std::int32_t>(mi32)) {
                    strip_start = 0;
                    strip_material = 0;
                }
                for (; strip_material < static_cast<std::int32_t>(mi32); ++strip_material)
                    strip_start += strip_term(jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->strip_lengths)[strip_material]);
                const std::int32_t term = strip_term(jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->strip_lengths)[mi32]);
                const std::uint32_t length = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->strip_lengths)[mi32];
                if (mode == 0)
                    joan::orig::as_reorder(list)->add_span_fwd(static_cast<void*>(jo::self<jo::Plane>(this)->vertex_buffer), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count)), joan::orig::arg::u32(lo), joan::orig::arg::u32(hi + 1), static_cast<void*>(jo::self<jo::Plane>(this)->index_buffer), joan::orig::arg::u32(strip_start), joan::orig::arg::u32(strip_start + term), joan::orig::arg::u32(length), bone, joan::orig::arg::u32(bones));  // 0x005e7f70
                else
                    joan::orig::as_reorder(list)->forward(vertices, joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count)), joan::orig::arg::u16(lo), joan::orig::arg::u16(hi + 1), static_cast<void*>(jo::self<jo::Plane>(this)->strip_indices), joan::orig::arg::u32(strip_start), joan::orig::arg::u32(strip_start + term), joan::orig::arg::u32(length), bone, joan::orig::arg::u32(bones));  // 0x005e7ec0
                continue;
            }
            if (mode == 0) {
                jo::view<std::uint32_t>(record(list)->prim) = 4;
                joan::orig::as_reorder(list)->find_or_insert(static_cast<std::int8_t>(joan::orig::arg::i32(0)));
                joan::orig::as_reorder(list)->add_span_fwd(static_cast<void*>(jo::self<jo::Plane>(this)->vertex_buffer), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count)), joan::orig::arg::u32(lo), joan::orig::arg::u32(hi + 1), static_cast<void*>(jo::self<jo::Plane>(this)->index_buffer), joan::orig::arg::u32(first * 3), joan::orig::arg::u32(end * 3), joan::orig::arg::u32(end - first), bone, joan::orig::arg::u32(bones));
                continue;
            }
            if (jo::view<std::int8_t>(jo::self<jo::Plane>(this)->prim_type) == 0) {
                jo::view<std::uint32_t>(record(list)->prim) = 4;
                joan::orig::as_reorder(list)->find_or_insert(static_cast<std::int8_t>(joan::orig::arg::i32(0)));
                joan::orig::as_reorder(list)->forward(vertices, joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count)), joan::orig::arg::u16(lo), joan::orig::arg::u16(hi + 1), static_cast<void*>(jo::self<jo::Plane>(this)->triangles), joan::orig::arg::u32(first * 3), joan::orig::arg::u32(end * 3), joan::orig::arg::u32(end - first), bone, joan::orig::arg::u32(bones));
                continue;
            }
            // Mixed primitive types: runs of one type (0x005d2aa0 on the index list +0x48).
            std::uint32_t b = 0;  // [ebp-0xa4]
            for (std::uint32_t a = first; a < end; a = b) {  // [ebp-0xa0]
                const std::int32_t type = joan::orig::arg::as<std::int32_t>(joan::orig::triangle_kind(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Plane>(this)->triangles)), joan::orig::arg::i32(a)));  // [ebp-0xa8]
                record(list)->prim = type;
                joan::orig::as_reorder(list)->find_or_insert(static_cast<std::int8_t>(joan::orig::arg::i32(0)));
                for (b = a + 1; b < end && joan::orig::arg::as<std::int32_t>(joan::orig::triangle_kind(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Plane>(this)->triangles)), joan::orig::arg::i32(b))) == type; ++b) {
                }
                if (type == 4) {
                    joan::orig::as_reorder(list)->forward(vertices, joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count)), joan::orig::arg::u16(lo), joan::orig::arg::u16(hi + 1), static_cast<void*>(jo::self<jo::Plane>(this)->triangles), joan::orig::arg::u32(a * 3), joan::orig::arg::u32(b * 3), joan::orig::arg::u32(b - a), bone, joan::orig::arg::u32(bones));
                } else if (type == 2) {
                    for (std::uint32_t c = a; c < b; ++c)  // [ebp-0xac]
                        joan::orig::as_reorder(list)->forward(vertices, joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count)), joan::orig::arg::u16(jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count)), joan::orig::arg::u16(0u), static_cast<void*>(jo::self<jo::Plane>(this)->triangles), joan::orig::arg::u32(c * 3), joan::orig::arg::u32(c * 3 + 2), 1u, bone, joan::orig::arg::u32(bones));
                } else if (type == 1) {
                    for (std::uint32_t c = a; c < b; ++c) {  // [ebp-0xb0]
                        const std::uint32_t index = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[c * 3];  // [ebp-0xb4]
                        joan::orig::as_mesh(list)->push_span_fwd(vertices, joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count)), joan::orig::arg::u32(index), joan::orig::arg::u32(index + 1), 1u, bone, joan::orig::arg::u32(bones));  // 0x005e7e70
                    }
                }
            }
        }
        if (built) joan::orig::as_material(default_material)->dtor();  // Material::~Material 0x005d5360
    }
};

JOAN_REPLACE("en3d/b117_01", "Plane::render_ordered", 0x005d35b0, &H::submit);

}  // namespace en3d_b117_01
}  // namespace

// ---------------------------------------- from modules/en3d_b82_09.cpp (en3d/b82_09)
//
//      0x005d6ee0 Plane::reserve_strip(mat, n)  - grows the per-material strip length +0x54[mat]
//                 by n (lazy +0x54 allocation), keeps +0x52 / +0x5c totals, reallocs and shifts
//                 the +0x60 index buffer. Returns the first index slot of the new range. ret 8.
//
//      0x005d4470 Plane::add_child(i, child)    - drops child i (0x005d4550), new Plane(this, 0)
//                 when child is null, links it with set_parent(this, i). ret 8, eax = child.
//
//      0x005d4a00 Plane::share_bones(src)       - deletes owned bones (+7), +0 = src+0. ret 4.
//
//      0x005d7270 Plane::set_flag50(on)         - ret 4.
//
//      0x005d7310 Plane::set_ec(child)          - +0xec link with back pointer child+0xf4. ret 4.
//
//      0x005d5ee0 Plane::set_f8(v)              - format 5 with 0xa52627 only. ret 4.
//
//      0x005d4720 Plane::set_lod(n, dist)       - +0xe4 / +0xe0. ret 8.
//
//      0x005d55a0 Material::set_colour8(colour*, v10). ret 8.
namespace {
namespace en3d_b82_09 {

using joan::orig::helpers::addr_of;

constexpr img::Addr kPlaneCpp(d::s_d_ww_en3d_plane_cpp);  // "d:\ww\en3d\plane.cpp"

JOAN_COUNTER(c_reserve_strip, "Plane::reserve_strip");
JOAN_COUNTER(c_add_child, "Plane::add_child");

// Strip length term: n > 0 ? n + 2 : 0 (n zero-extended word).
std::int32_t strip_len(std::uint16_t n) { return static_cast<std::int32_t>(n) > 0 ? static_cast<std::int32_t>(n) + 2 : 0; }

struct H {
    // 0x005d6ee0 Plane::reserve_strip(mat, n). ret 8.
    std::int32_t reserve_strip(std::int32_t mat, std::uint32_t n_arg) {
        c_reserve_strip.hit();
        const std::uint16_t n = static_cast<std::uint16_t>(n_arg);
        if (jo::self<jo::Plane>(this)->data_pending != 0) joan::orig::as_plane(this)->load_5d9be0();  // 0x005d9be0
        jo::self<jo::Plane>(this)->has_strips = 1;
        if (jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths) == nullptr) {
            jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths) = joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(jo::self<jo::Plane>(this)->material_count << 1), joan::orig::arg::str(img::str(kPlaneCpp)), joan::orig::arg::i32(line_base(d::g_a173e8) + 0xf));  // 0x0058e7b0
            std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths)), 0, joan::orig::arg::u32(jo::self<jo::Plane>(this)->material_count << 1));  // memset
        }
        const std::int32_t old = static_cast<std::int32_t>(jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->strip_lengths)[mat]);
        std::int32_t sum = 0;
        for (std::int32_t i = 0; i < mat; ++i) sum += strip_len(jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->strip_lengths)[i]);
        const std::int32_t old_end = sum + strip_len(jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->strip_lengths)[mat]);
        jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->strip_lengths)[mat] = static_cast<std::uint16_t>(jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->strip_lengths)[mat] + n);
        jo::self<jo::Plane>(this)->strip_total = static_cast<std::uint16_t>(jo::self<jo::Plane>(this)->strip_total + n);
        const std::int32_t new_end = sum + strip_len(jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->strip_lengths)[mat]);
        jo::self<jo::Plane>(this)->strip_index_count = jo::self<jo::Plane>(this)->strip_index_count + (new_end - old_end);
        if (jo::self<jo::Plane>(this)->strip_index_count > jo::self<jo::Plane>(this)->strip_capacity) {
            jo::self<jo::Plane>(this)->strip_capacity = jo::self<jo::Plane>(this)->strip_index_count + 0x100;
            jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices) = joan::orig::as_mem(img::obj(kMemory))->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices)), joan::orig::arg::u32(jo::self<jo::Plane>(this)->strip_capacity << 1), joan::orig::arg::str(img::str(kPlaneCpp)), joan::orig::arg::i32(line_base(d::g_a173e8) + 0x3f));  // 0x0058eb30
        }
        joan::orig::libc::memcpy(joan::orig::arg::mptr(static_cast<void*>(jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->strip_indices) + new_end)), joan::orig::arg::mptr(static_cast<void*>(jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->strip_indices) + old_end)), joan::orig::arg::u32((jo::self<jo::Plane>(this)->strip_index_count - new_end) << 1));  // memmove
        joan::stream_cache_api::indices_written(jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices));
        return sum + old;
    }

    // 0x005d4470 Plane::add_child(i, child). ret 8, eax = child.
    void* add_child(std::int32_t i, void* child) {
        c_add_child.hit();
        joan::orig::as_plane(this)->release_lod(joan::orig::arg::i32(i));  // 0x005d4550
        if (child == nullptr) {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // operator new (0x11c on x86)
            child = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_model(mem)->Model_5cbb60(joan::orig::arg::mptr(static_cast<void*>(this)), 0u)) : nullptr;  // 0x005cbb60
        }
        joan::orig::as_plane(child)->set_parent(joan::orig::arg::mptr(static_cast<void*>(this)), joan::orig::arg::i32(i));  // 0x005d4880
        return child;
    }

    // 0x005d4a00 Plane::share_bones(src). ret 4, eax = this.
    void* share_bones(const void* src) {
        if (static_cast<std::int8_t>(jo::self<jo::Plane>(this)->owns_bones) != 0) {
            void* const bones = jo::view<void*>(jo::self<jo::Plane>(this)->bones);
            if (bones != nullptr) joan::orig::as_bone(bones)->vector_delete(3u);  // 0x005cca00 vector deleting dtor
        }
        jo::self<jo::Plane>(this)->owns_bones = 0;
        jo::self<jo::Plane>(this)->bones = jo::as_plane(src)->bones;
        return this;
    }

    // 0x005d7270 Plane::set_flag50(on). ret 4.
    std::uintptr_t set_flag50(std::uint32_t on_arg) {
        const std::int8_t on = static_cast<std::int8_t>(on_arg);
        if (static_cast<std::int8_t>(jo::self<jo::Plane>(this)->use_strips) == on) return addr_of(this);
        if (on != 0 && static_cast<std::int8_t>(jo::self<jo::Plane>(this)->has_strips) == 0) return 0;
        jo::view<std::int8_t>(jo::self<jo::Plane>(this)->use_strips) = on;
        joan::orig::as_plane(this)->invalidate(4u);  // 0x005cf920
        return 0;  // 0x005cf920 returns nothing; callers ignore the result
    }

    // 0x005d7310 Plane::set_ec(child). ret 4, eax = child.
    void* set_ec(void* child) {
        if (jo::view<void*>(jo::self<jo::Plane>(this)->next) != nullptr) jo::self<jo::Plane>(this)->next->back_link = nullptr;
        jo::view<void*>(jo::self<jo::Plane>(this)->next) = child;
        jo::view<void*>(jo::self<jo::Plane>(this)->next->back_link) = this;
        return jo::view<void*>(jo::self<jo::Plane>(this)->next);
    }

    // 0x005d5ee0 Plane::set_f8(v). ret 4.
    std::uintptr_t set_f8(std::uint32_t v) {
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format)) != 5) return addr_of(this);
        if (global<std::uint32_t>(d::g_a52627) == 0) return addr_of(this);
        jo::self<jo::Plane>(this)->normals_pending_ = v;
        return v;
    }

    // 0x005d4720 Plane::set_lod(n, dist). ret 8, eax = dist (bits).
    std::uint32_t set_lod(std::uint32_t n, std::uint32_t dist_bits) {
        jo::self<jo::Plane>(this)->lod_count = static_cast<std::uint8_t>(n);
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->lod_distance) = dist_bits;
        return dist_bits;
    }

    // 0x005d5540 / 0x005d5570 / 0x005d55e0: +0 / +4 / +0xc = *colour. ret 4, eax = colour.
    const void* set_colour0(const std::uint32_t* c) {
        jo::self<jo::Material>(this)->diffuse = *c;
        return c;
    }
    const void* set_colour4(const std::uint32_t* c) {
        jo::self<jo::Material>(this)->ambient = *c;
        return c;
    }
    const void* set_colourc(const std::uint32_t* c) {
        jo::self<jo::Material>(this)->emissive = *c;
        return c;
    }
    // 0x005d55a0: +8 = *colour, +0x10 = v. ret 8, eax = this.
    void* set_colour8(const std::uint32_t* c, std::uint32_t v) {
        jo::self<jo::Material>(this)->specular = *c;
        jo::self<jo::Material>(this)->power = v;
        return this;
    }
};

#define B82_09(name, addr, fn) JOAN_REPLACE("en3d/b82_09", name, addr, fn)
B82_09("Plane::reserve_strip", 0x005d6ee0, &H::reserve_strip);
B82_09("Plane::add_lod", 0x005d4470, &H::add_child);
B82_09("Plane::share_frame", 0x005d4a00, &H::share_bones);
B82_09("Plane::set_flag50", 0x005d7270, &H::set_flag50);
B82_09("Plane::set_shadow_plane", 0x005d7310, &H::set_ec);
B82_09("Plane::set_f8", 0x005d5ee0, &H::set_f8);
B82_09("Plane::set_lod_count", 0x005d4720, &H::set_lod);
B82_09("Material::set_colour0", 0x005d5540, &H::set_colour0);
B82_09("Material::set_colour4", 0x005d5570, &H::set_colour4);
B82_09("Material::set_colour8", 0x005d55a0, &H::set_colour8);
B82_09("Material::set_colourc", 0x005d55e0, &H::set_colourc);
#undef B82_09

}  // namespace en3d_b82_09
}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
//
//      0x005d4550 Plane::release_lod(i) (ret 4)     - delete +0xcc[i], = 0
//
//      0x005d5500 Material::equals(m) (ret 4)       - memcmp 0x54 == 0
namespace {
namespace en3d_b83_03 {


struct H {
    // Scalar delete through 0x00459c40 (thunk 0x0040903e).
    static void delete_object(void* p) {
        if (p != nullptr) joan::orig::as_plane(p)->destroy(1u);
    }

    // 0x005d4550 (eax = i)
    std::int32_t release_lod(std::int32_t i) {
        if (jo::view<void*>(jo::self<jo::Plane>(this)->children[i - 1]) != nullptr) {
            delete_object(jo::view<void*>(jo::self<jo::Plane>(this)->children[i - 1]));
            jo::view<void*>(jo::self<jo::Plane>(this)->children[i - 1]) = nullptr;
        }
        return i;
    }

    // 0x005d5500
    std::int32_t material_equals(const void* other) {
        return joan::orig::arg::as<std::int32_t>(joan::orig::libc::memcmp(joan::orig::arg::mptr(static_cast<const void*>(this)), joan::orig::arg::mptr(other), static_cast<std::uint32_t>(sizeof(jo::Material)))) == 0 ? 1 : 0;  // memcmp (0x54 on x86)
    }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("Plane::remove_lod", 0x005d4550, &H::release_lod);
B83_03("Material::operator==", 0x005d5500, &H::material_equals);
#undef B83_03

}  // namespace en3d_b83_03
}  // namespace

// ---------------------------------------- from modules/game_collide1.cpp (game/collide1)
//
//  * module game/collide1:
//      Plane::segment_hit 0x005d4a80 (en3d/plane.cpp: the plane mesh (+0x40 triangle count,
//        +0x48 triangle index words, +0x1c..+0x2c vertex streams by format +4) against a
//        segment given as two points, transformed into the mesh space when its matrix is not
//        the identity; keeps the nearest parameter in [0, 1] in *out and returns the 1-based
//        triangle index of the last accepted hit, 0 when none),
//      Plane::set_100 0x005d6e20 (+0x100 = arg),
//      Plane::load 0x005ddd70 (Plane::init, then dispatches on the file version 0 / 50 / 100 /
//        500 to the version readers),
//      Shape::cylinder_hit 0x00699ec0 (shape 3 of Sky::ray_clear: vertical cylinder, side and
//        caps), Shape::box_hit 0x00698970 (shape 2: slab test on an AABB),
//      Shape::sphere_hit 0x006a0f90 (shape 1),
//      TerrainBlock::height 0x00664180 (height map of the block (cell size +0x24 / +0x28,
//        tiles +0x34) or the triangle of the block (+0x30); returns st0, which is an exact
//        integer (fild) on the degenerate triangle path, so the result is a double).
namespace {
namespace game_collide1 {


JOAN_COUNTER(c_segment, "Plane::segment_hit");
JOAN_COUNTER(c_segment_skip, "Plane::segment_hit triangles rejected early");

// ray_triangle 0x0060e810 (Moller-Trumbore, front faces: det >= 1e-4, u >= 0, v >= 0, u + v <= det, at
// 24-bit x87 precision through the emulated v3d maths - the cost of the camera's collision) redone in
// double: false only when the triangle fails one of those tests by far more than the original's rounding
// (each margin 1e-5 of the magnitudes involved, ~160 float ulps). Every other triangle - near an edge,
// grazing, a hit - is decided by ray_triangle itself, so the results are the original's.
// The segment's part of it (direction, length) is computed once per segment_hit.
struct SegmentPre {
    double sx, sy, sz, dx, dy, dz, ld2, ld;
};
SegmentPre segment_pre(const float* seg) {
    SegmentPre s;
    s.sx = seg[0];
    s.sy = seg[1];
    s.sz = seg[2];
    s.dx = double(seg[3]) - seg[0];
    s.dy = double(seg[4]) - seg[1];
    s.dz = double(seg[5]) - seg[2];
    s.ld2 = s.dx * s.dx + s.dy * s.dy + s.dz * s.dz;
    s.ld = std::sqrt(s.ld2);
    return s;
}
bool may_hit(const SegmentPre& s, const float* tri) {
    const double dx = s.dx, dy = s.dy, dz = s.dz;
    const double ax = double(tri[3]) - tri[0], ay = double(tri[4]) - tri[1], az = double(tri[5]) - tri[2];  // e1
    const double bx = double(tri[6]) - tri[0], by = double(tri[7]) - tri[1], bz = double(tri[8]) - tri[2];  // e2
    const double px = dy * bz - dz * by, py = dz * bx - dx * bz, pz = dx * by - dy * bx;  // dir x e2
    const double det = ax * px + ay * py + az * pz;
    const double la2 = ax * ax + ay * ay + az * az, lb2 = bx * bx + by * by + bz * bz;
    // Without square roots first: x < 0 with x^2 > kSq * (the squared lengths) means x < -1e-5 * (the lengths) -
    // below the margin of the full test, which rejects it too (kSq exceeds 1e-10 by far more than the rounding of
    // either side; an overflow or NaN compares false and goes on). Most triangles end here: back faces, or the
    // segment's line clearly off one side.
    constexpr double kSq = 1.0001e-10;
    if (det < 0.0 && det * det > kSq * (s.ld2 * la2 * lb2)) return false;  // det < -eps_det
    const double tx = s.sx - tri[0], ty = s.sy - tri[1], tz = s.sz - tri[2];  // tvec
    const double lt2 = tx * tx + ty * ty + tz * tz;
    const double u = tx * px + ty * py + tz * pz;
    if (u < 0.0 && u * u > kSq * (lt2 * s.ld2 * lb2)) return false;  // u < -eps_u
    const double qx = ty * az - tz * ay, qy = tz * ax - tx * az, qz = tx * ay - ty * ax;  // tvec x e1
    const double v = dx * qx + dy * qy + dz * qz;
    if (v < 0.0 && v * v > kSq * (s.ld2 * lt2 * la2)) return false;  // v < -eps_v
    // The full test.
    const double ld = s.ld, la = std::sqrt(la2), lb = std::sqrt(lb2), lt = std::sqrt(lt2);
    const double eps_det = 1e-5 * ld * la * lb;
    if (det < 1e-4 - eps_det - 1e-6) return false;
    const double eps_u = 1e-5 * lt * ld * lb;
    if (u < -eps_u || u > det + eps_u + eps_det) return false;
    const double eps_v = 1e-5 * ld * lt * la;
    if (v < -eps_v || u + v > det + eps_u + eps_v + eps_det) return false;
    return true;  // NaN compares false above: decided by ray_triangle
}

// may_hit's square-root-free rejections for 4 triangles at once (AVX, 4 doubles a register): the same double
// operations in the same order per lane (no fma: separate multiplies and adds), so a lane rejects exactly when
// may_hit's own first three tests would. Bit k: triangle k is rejected; the others go to may_hit itself.
// (GCC / Clang compile AVX / XSAVE intrinsics only in functions marked for them - MSVC anywhere; g_avx picks them at
// run time.)
JOAN_TARGET("xsave") bool cpu_has_avx() {
    int r[4] = {};
    joan::cpu::id(r, 1);
    if ((r[2] & (1 << 27)) == 0 || (r[2] & (1 << 28)) == 0) return false;  // OSXSAVE, AVX
    return (_xgetbv(0) & 6u) == 6u;                                       // the OS saves the YMM registers
}
const bool g_avx = cpu_has_avx();

// Coordinate c of the 4 triangles (floats -> doubles: exact).
JOAN_TARGET("avx") inline __m256d avx_lane(const float (*t)[9], int c) {
    return _mm256_set_pd(t[3][c], t[2][c], t[1][c], t[0][c]);
}
// (x0*x1 + y0*y1) + z0*z1
JOAN_TARGET("avx") inline __m256d avx_dot(const __m256d& x0, const __m256d& y0, const __m256d& z0, const __m256d& x1,
                                          const __m256d& y1, const __m256d& z1) {
    return _mm256_add_pd(_mm256_add_pd(_mm256_mul_pd(x0, x1), _mm256_mul_pd(y0, y1)), _mm256_mul_pd(z0, z1));
}
// x < 0 && x * x > kSq * (a * b * c), as may_hit (its products in the same order).
JOAN_TARGET("avx") inline __m256d avx_below(const __m256d& x, const __m256d& a, const __m256d& b, const __m256d& c,
                                            const __m256d& k, const __m256d& zero) {
    const __m256d lim = _mm256_mul_pd(k, _mm256_mul_pd(_mm256_mul_pd(a, b), c));
    return _mm256_and_pd(_mm256_cmp_pd(x, zero, _CMP_LT_OQ), _mm256_cmp_pd(_mm256_mul_pd(x, x), lim, _CMP_GT_OQ));
}

JOAN_TARGET("avx") int fast_rejects4(const SegmentPre& s, const float (*t)[9]) {
    const __m256d v0x = avx_lane(t, 0), v0y = avx_lane(t, 1), v0z = avx_lane(t, 2);
    const __m256d ax = _mm256_sub_pd(avx_lane(t, 3), v0x), ay = _mm256_sub_pd(avx_lane(t, 4), v0y), az = _mm256_sub_pd(avx_lane(t, 5), v0z);
    const __m256d bx = _mm256_sub_pd(avx_lane(t, 6), v0x), by = _mm256_sub_pd(avx_lane(t, 7), v0y), bz = _mm256_sub_pd(avx_lane(t, 8), v0z);
    const __m256d dx = _mm256_set1_pd(s.dx), dy = _mm256_set1_pd(s.dy), dz = _mm256_set1_pd(s.dz);
    const __m256d px = _mm256_sub_pd(_mm256_mul_pd(dy, bz), _mm256_mul_pd(dz, by));
    const __m256d py = _mm256_sub_pd(_mm256_mul_pd(dz, bx), _mm256_mul_pd(dx, bz));
    const __m256d pz = _mm256_sub_pd(_mm256_mul_pd(dx, by), _mm256_mul_pd(dy, bx));
    const __m256d det = avx_dot(ax, ay, az, px, py, pz);
    const __m256d la2 = avx_dot(ax, ay, az, ax, ay, az), lb2 = avx_dot(bx, by, bz, bx, by, bz);
    const __m256d k = _mm256_set1_pd(1.0001e-10), ld2 = _mm256_set1_pd(s.ld2), zero = _mm256_setzero_pd();
    __m256d rej = avx_below(det, ld2, la2, lb2, k, zero);
    const __m256d tx = _mm256_sub_pd(_mm256_set1_pd(s.sx), v0x), ty = _mm256_sub_pd(_mm256_set1_pd(s.sy), v0y),
                  tz = _mm256_sub_pd(_mm256_set1_pd(s.sz), v0z);
    const __m256d lt2 = avx_dot(tx, ty, tz, tx, ty, tz);
    const __m256d u = avx_dot(tx, ty, tz, px, py, pz);
    rej = _mm256_or_pd(rej, avx_below(u, lt2, ld2, lb2, k, zero));
    const __m256d qx = _mm256_sub_pd(_mm256_mul_pd(ty, az), _mm256_mul_pd(tz, ay));
    const __m256d qy = _mm256_sub_pd(_mm256_mul_pd(tz, ax), _mm256_mul_pd(tx, az));
    const __m256d qz = _mm256_sub_pd(_mm256_mul_pd(tx, ay), _mm256_mul_pd(ty, ax));
    const __m256d v = avx_dot(dx, dy, dz, qx, qy, qz);
    rej = _mm256_or_pd(rej, avx_below(v, ld2, lt2, la2, k, zero));
    const int mask = _mm256_movemask_pd(rej);
    _mm256_zeroupper();
    return mask;
}

struct H {
    // Plane field reset when the mesh borrows its data from +0xc (+0xb set).
    void clear_shared(bool with_38) {
        if (jo::self<jo::Plane>(this)->data_pending == 0) return;
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_count) = 0;
        jo::self<jo::Plane>(this)->triangles = nullptr;
        jo::self<jo::Plane>(this)->vertices_f1 = nullptr;
        jo::self<jo::Plane>(this)->vertices_f2 = nullptr;
        jo::self<jo::Plane>(this)->vertices_f3 = nullptr;
        jo::self<jo::Plane>(this)->vertices_f4 = nullptr;
        jo::self<jo::Plane>(this)->vertices_f5 = nullptr;
        jo::self<jo::Plane>(this)->vertices_f6 = nullptr;
        jo::self<jo::Plane>(this)->vertices_f7 = nullptr;
        if (with_38) jo::self<jo::Plane>(this)->vertices_f8 = nullptr;
    }

    // 0x005d4a80 Plane::segment_hit(out, segment[2]).
    std::int32_t segment_hit(float* out, const void* segment) {
        c_segment.hit();
        if (jo::self<jo::Plane>(this)->data_pending != 0) {
            std::uint8_t* const src = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->source);
            jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->vertex_count) = jo::view<std::uint32_t>(jo::as_plane(src)->vertex_count);
            jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->triangle_count) = jo::view<std::uint32_t>(jo::as_plane(src)->triangle_count);
            jo::self<jo::Plane>(this)->triangles = jo::as_plane(src)->triangles;
            jo::self<jo::Plane>(this)->vertices_f1 = jo::as_plane(src)->vertices_f1;
            jo::self<jo::Plane>(this)->vertices_f2 = jo::as_plane(src)->vertices_f2;
            jo::self<jo::Plane>(this)->vertices_f3 = jo::as_plane(src)->vertices_f3;
            jo::self<jo::Plane>(this)->vertices_f4 = jo::as_plane(src)->vertices_f4;
            jo::self<jo::Plane>(this)->vertices_f5 = jo::as_plane(src)->vertices_f5;
            jo::self<jo::Plane>(this)->vertices_f6 = jo::as_plane(src)->vertices_f6;
            jo::self<jo::Plane>(this)->vertices_f7 = jo::as_plane(src)->vertices_f7;
            jo::self<jo::Plane>(this)->vertices_f8 = jo::as_plane(src)->vertices_f8;
        }
        if (!(jo::self<jo::Plane>(this)->triangle_count > 0)) return 0;
        std::int32_t last = 0;
        const std::int32_t format = static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format));
        const std::uint8_t* const fmt = joan::orig::arg::as<std::uint8_t*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(format))));  // 0x00600200
        const std::uint32_t stride = jo::as_vertex_format(fmt)->stride;
        const std::uint8_t* base;
        switch (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format))) {
            case 1: base = reinterpret_cast<const std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f1); break;
            case 2: base = reinterpret_cast<const std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f2); break;
            case 3: base = reinterpret_cast<const std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f3); break;
            case 4: base = reinterpret_cast<const std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f4); break;
            case 5: base = reinterpret_cast<const std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f5); break;
            case 6:
            case 7:
            case 8: clear_shared(true); return 0;
            default: clear_shared(false); return 0;  // the original leaves +0x38 here
        }
        void* matrix = nullptr;
        if (jo::view<std::int32_t>(jo::self<jo::Plane>(this)->transformed) == 0 && joan::orig::arg::as<void*>(joan::orig::as_plane(this)->bone_5cf8d0()) != nullptr) {  // 0x005cf8d0
            if (joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(this)->bone_5cf8d0()))->matrix()) != nullptr) {  // 0x0060ef80
                void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(this)->bone_5cf8d0()))->matrix());
                if (joan::orig::arg::as<std::int32_t>(joan::orig::libc::memcmp(joan::orig::arg::mptr(m), joan::orig::arg::mptr(img::obj(d::g_identity_matrix)), 0x40u)) != 0) {  // memcmp
                    matrix = joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(this)->bone_5cf8d0()))->matrix());
                }
            }
        }
        float seg[6];
        std::memcpy(seg, segment, sizeof seg);
        if (matrix != nullptr) {
            float inv[16];
            import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), matrix);  // matrix_inverse
            import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(&seg[0]), static_cast<void*>(inv));  // point_transform
            import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(&seg[3]), static_cast<void*>(inv));
        }
        const SegmentPre pre = segment_pre(seg);
        // One triangle after the pre-test (`rejected`: may_hit's verdict is known to be false).
        auto triangle = [&](std::int32_t i, float* verts, bool rejected) {
            if (rejected || !may_hit(pre, verts)) {
                c_segment_skip.hit();
                // The first rejections are checked against the exact test (openjoan.log): proof that none was a hit.
                static std::uint32_t checked = 0, wrong = 0;
                if (checked < 200000) {
                    float ct, cu, cv;
                    const bool hit = joan::orig::arg::as<std::int32_t>(joan::orig::ray_triangle(&ct, joan::orig::arg::mptr(seg), joan::orig::arg::mptr(verts), &cu, &cv)) != 0 &&
                                     ct >= 0.0f && ct <= 1.0f;
                    if (hit && wrong++ < 5)
                        joan::hook::log_line("[Perf] Plane::segment_hit: early reject of a hit (t %.6f) - please report", static_cast<double>(ct));
                    if (++checked == 200000)
                        joan::hook::log_line("[Perf] Plane::segment_hit: 200000 early rejects checked against ray_triangle, %u were hits", wrong);
                }
                return;
            }
            float t, u, v;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::ray_triangle(&t, joan::orig::arg::mptr(seg), joan::orig::arg::mptr(verts), &u, &v)) == 0) return;  // 0x0060e810
            if (!(t >= 0.0f)) return;
            if (t > 1.0f) return;
            if (last != 0) {
                if (*out > t) *out = t;
            } else {
                *out = t;
            }
            last = i + 1;
        };
        // Triangles in index order, four at a time through the AVX pre-test (fast_rejects4) when the CPU has it.
        float verts[4][9] = {};
        std::int32_t index[4];
        int n = 0;
        auto run = [&] {
            const int mask = g_avx && n > 1 ? fast_rejects4(pre, verts) : 0;  // (lanes past n: stale data, ignored)
            for (int k = 0; k < n; ++k) triangle(index[k], verts[k], ((mask >> k) & 1) != 0);
            n = 0;
        };
        for (std::int32_t i = 0; i < jo::self<jo::Plane>(this)->triangle_count; ++i) {
            const std::uint8_t* const tri = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->triangles) + i * 6;
            if (jo::as_tri16(tri)->v[2] == 0xffff) continue;  // triangle_kind (0x005d2aa0) != 4, inline: padding
            for (int k = 0; k < 3; ++k) {
                const std::uint32_t idx = jo::as_tri16(tri)->v[k];
                std::memcpy(&verts[n][k * 3], base + idx * stride, 12);
            }
            index[n] = i;
            if (++n == 4) run();
        }
        run();
        clear_shared(true);
        return last;
    }

    // 0x005d6e20
    void set_100(std::uint32_t v) { jo::self<jo::Plane>(this)->flags_100 = v; }
};

#define CL1(name, addr, fn) JOAN_REPLACE("game/collide1", name, addr, fn)
CL1("Plane::line_seg_intersect", 0x005d4a80, &H::segment_hit);
CL1("Plane::set_100", 0x005d6e20, &H::set_100);
#undef CL1

}  // namespace game_collide1
}  // namespace

// ---------------------------------------- from modules/game_actors.cpp (game/actors1)
//
//  * module game/actors1: Actor::set_faded 0x005ae590 (see-through when blocking the
//    camera), Actor::set_position 0x005ac310 (moves the bone and the owner), getters
//    0x005aede0 / 0x005aee10 / 0x005d5eb0. (Actor::visible / can_draw are in en3d/helpers4 /
//    en3d/helpers.)
namespace {
namespace game_actors {


struct H {
    std::uint32_t* plane_colour(std::uint32_t* out) {  // 0x005d5eb0
        *out = jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->colour_);
        return out;
    }
};

#define A1(name, addr, fn) JOAN_REPLACE("game/actors1", name, addr, fn)
A1("Plane::colour", 0x005d5eb0, &H::plane_colour);
#undef A1

}  // namespace game_actors
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
//
//    - Plane::has_alpha_material 0x005d5fe0 (ret 0) -> bool: some material's +0x18 != 0.0f.
//
//  - Return values: callers of 0x5acfa0, 0x5b8a50, 0x5c80d0, 0x5e7ce0, 0x601140, 0x616dd0,
//    0x623bd0 ignore eax (checked); 0x5d5fe0's callers use al only.
namespace {
namespace game_b100_11 {


struct H {
    // ------------------------------------------------------------------ 0x005d5fe0
    bool has_alpha_material() {
        for (i32 i = 0; i < jo::self<jo::Plane>(this)->material_count; ++i) {
            const float v = jo::view<float>(jo::self<jo::Plane>(this)->materials[i].u32_18);
            // fcomp + test ah,0x40 jne: equal or unordered -> continue
            if (!(v == global<float>(d::k_9cf5c4) || v != v)) return true;
        }
        return false;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Plane::is_reflective", 0x005d5fe0, &H::has_alpha_material);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b109_09.cpp (game/b109_09)
//
//    - Plane::draw_mirror_5d6050 0x005d6050 (camera), ret 4: finds the material with mode 0xf,
//      takes the first triangle of that group from the vertex array of the plane's vertex
//      format (+4: 1 / 2 / 4 / 5 / 8), builds the reflection matrix (D3DXMatrixReflect
//      0x00768e60), a mirrored camera (local 0x4ca-byte Camera, 0x00613650 .. 0x00613700) and
//      draws the scene through it with stencil states on the device 0xa52428 (SEH frame:
//      normal path only - the camera destructor runs on both exits like the original).
namespace {
namespace game_b109_09 {

// COM method (stdcall, interface pointer first).
template <class R = void, class... A>
R com_call(void* iface, u32 off, A... args) {
    const std::uintptr_t* vt = *reinterpret_cast<std::uintptr_t* const*>(iface);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(vt[joan::call::x86_vslot(off)])(iface, args...);
}
// The plane's vertex array at `off` (offsetof(Plane, vertices_fN)).
u8* vertex_array(const void* plane, std::int32_t off) { return *reinterpret_cast<u8* const*>(static_cast<const u8*>(plane) + off); }

JOAN_COUNTER(c_mirror, "Plane::draw_mirror_5d6050");

// ---------------------------------------------------------------------------------------------
// 0x005d6050 helpers.
void srs(u32 state, u32 value) {
    com_call<i32>(global<void*>(d::g_game_device), 0xc8, state, value);  // SetRenderState
}
void clear(u32 flags) {
    com_call<i32>(global<void*>(d::g_game_device), 0x90, 0u, static_cast<const void*>(nullptr), flags, 0u, 1.0f,
                  0u);  // Clear(0, 0, flags, 0, 1.0f, 0)
}
void set_world(const void* m) {
    com_call<i32>(global<void*>(d::g_game_device), 0x94, 2u, m);  // SetTransform(WORLD)
}

struct H {
    // 0x005d6050
    void draw_mirror_5d6050(u8* cam) {
        c_mirror.hit();
        void* const self = this;
        i32 group = -1;
        for (i32 i = 0; i < jo::self<jo::Plane>(this)->material_count; ++i)
            if (jo::view<i32>((jo::as_material(jo::view<u8*>(jo::as_plane(self)->materials) + i * static_cast<i32>(sizeof(jo::Material))))->blend) == 0xf) group = i;
        i32 tri = 0;
        if (jo::self<jo::Plane>(this)->material_count >= 2) tri = static_cast<i32>(jo::view<u16*>(jo::self<jo::Plane>(this)->material_first)[group]) - 1;
        if (group == -1) return;
        if (tri < 0 && !(tri < jo::self<jo::Plane>(this)->triangle_count)) return;
        i32 array_off = 0;
        u32 stride = 0;
        switch (static_cast<i32>(jo::self<jo::Plane>(this)->vertex_format) - 1) {
            case 0:
                array_off = static_cast<i32>(offsetof(jo::Plane, vertices_f1));
                stride = static_cast<u32>(sizeof(jo::VertexF1));
                break;
            case 1:
                array_off = static_cast<i32>(offsetof(jo::Plane, vertices_f2));
                stride = static_cast<u32>(sizeof(jo::VertexF2));
                break;
            case 3:
                array_off = static_cast<i32>(offsetof(jo::Plane, vertices_f4));
                stride = static_cast<u32>(sizeof(jo::VertexF4));
                break;
            case 4:
                array_off = static_cast<i32>(offsetof(jo::Plane, vertices_f5));
                stride = static_cast<u32>(sizeof(jo::VertexF5));
                break;
            case 7:
                array_off = static_cast<i32>(offsetof(jo::Plane, vertices_f8));
                stride = static_cast<u32>(sizeof(jo::VertexF8));
                break;
            default:
                return;
        }
        V3 a, b, c;
        const u32 t3 = static_cast<u32>(tri) * 3u;
        std::memcpy(&a, vertex_array(self, array_off) + static_cast<i32>(t3 * stride), 12);
        std::memcpy(&b, vertex_array(self, array_off) + static_cast<i32>((t3 + 1u) * stride), 12);
        std::memcpy(&c, vertex_array(self, array_off) + static_cast<i32>((t3 + 2u) * stride), 12);

        alignas(16) u32 plane[4];
        joan::orig::as_plane_equation(plane)->init_425700();                 // 0x00425700
        joan::orig::plane_from_points(joan::orig::arg::mptr(plane), &a, &b, &c);   // 0x0060d920 plane from points
        alignas(16) u32 reflect[16];
        joan::orig::as_matrix(reflect)->construct();               // 0x005bea30
        joan::orig::d3dx::matrix_reflect(joan::orig::arg::mptr(reflect), joan::orig::arg::mptr(plane));   // D3DXMatrixReflect
        alignas(16) u32 m[16];
        std::memcpy(m, reflect, sizeof m);

        u32 view[10];
        joan::orig::as_camera(cam)->frame(joan::orig::arg::mptr(view));  // 0x006140a0 get view (0x28 bytes)
        joan::orig::as_camera(cam)->set_frame(joan::orig::arg::mptr(view));  // 0x00614040 set view
        alignas(16) u32 rot[16];
        joan::orig::orbit_matrix(joan::orig::arg::mptr(rot), joan::orig::arg::f32(view[4]), joan::orig::arg::f32(view[5]), joan::orig::arg::f32(view[6]), joan::orig::arg::f32(0u));  // 0x0060bd00
        u32 r0[3] = {rot[0], rot[1], rot[2]};
        u32 r1[3] = {rot[4], rot[5], rot[6]};
        u32 r2[3] = {rot[8], rot[9], rot[10]};
        import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(r0), static_cast<void*>(m));  // vector_transform
        import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(r1), static_cast<void*>(m));
        import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(r2), static_cast<void*>(m));
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(r2));  // vector_normalize
        import_call<void*>(d::iat_v3d_87_matrix_multiply_3, static_cast<void*>(rot), static_cast<void*>(m),
                           static_cast<void*>(rot));  // matrix_multiply(out, a, b)
        const float yaw = joan::orig::arg::as<float>(joan::orig::libc::atan2_f(joan::orig::arg::f32(r2[1]), joan::orig::arg::f32(r2[0])));  // 0x00421020
        const u32 neg_z = r2[2] ^ 0x80000000u;                          // fchs
        const float pitch = joan::orig::arg::as<float>(joan::orig::libc::asin_f(joan::orig::arg::f32(neg_z)));       // 0x00425610
        alignas(16) u32 inv[16];
        import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), static_cast<void*>(rot));  // matrix_inverse
        alignas(16) u32 turn[16];
        joan::orig::orbit_matrix(joan::orig::arg::mptr(turn), joan::orig::arg::f32(bits(yaw)), joan::orig::arg::f32(bits(pitch)), joan::orig::arg::f32(0u), joan::orig::arg::f32(0u));  // 0x0060bd00
        u32 axis[3] = {0u, 0u, 0x3f800000u};
        float angle = 0.0f;
        void* const prod = import_call<void*>(d::iat_v3d_87_matrix_multiply_2, static_cast<void*>(inv), static_cast<void*>(turn));  // matrix_multiply
        joan::orig::matrix_axis_angle(joan::orig::arg::mptr(prod), joan::orig::arg::mptr(axis), &angle);  // 0x0060c3b0 axis / angle

        alignas(16) u8 camera[((sizeof(jo::Camera) > 0x4cc ? sizeof(jo::Camera) : 0x4cc) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // a Camera: 0x4ca on x86, 0x4ea on x64
        joan::orig::as_camera(camera)->construct();  // 0x00613650 ctor
        joan::orig::as_sys_en(camera)->init_613730();   // 0x00613730
        joan::orig::as_camera(camera)->set_viewport(jo::as_camera(cam)->target, jo::as_camera(cam)->depth_target, joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->view_x1)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->view_y1)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->view_x2)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->view_y2)));  // 0x00613bf0 set_viewport
        joan::orig::as_camera(camera)->set_ranges(joan::orig::arg::f32(jo::view<u32>(jo::as_camera(cam)->near_plane)), joan::orig::arg::f32(jo::view<u32>(jo::as_camera(cam)->far_perspective)), joan::orig::arg::f32(jo::view<u32>(jo::as_camera(cam)->far_ortho)));  // 0x00613ca0
        joan::orig::as_camera(camera)->set_cf(0u, 1u, joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->clear_colour)));  // 0x00613d30
        joan::orig::as_camera(camera)->set_scene(jo::as_camera(cam)->scene);            // 0x00613db0
        jo::as_camera(camera)->u32_28 = jo::as_camera(cam)->u32_28;
        const i32 i40 = ftol(jo::as_camera(cam)->angle_extra[1]);
        const i32 i3c = ftol(jo::as_camera(cam)->angle_extra[0]);
        const i32 i38 = ftol(jo::as_camera(cam)->angle_max[1]);
        const i32 i34 = ftol(jo::as_camera(cam)->angle_max[0]);
        const i32 i30 = ftol(jo::as_camera(cam)->angle_min[1]);
        const i32 i2c = ftol(jo::as_camera(cam)->angle_min[0]);
        joan::orig::as_viewport(camera)->set_int(joan::orig::arg::i32(i2c), joan::orig::arg::i32(i30), joan::orig::arg::i32(i34), joan::orig::arg::i32(i38), joan::orig::arg::i32(i3c), joan::orig::arg::i32(i40));  // 0x00614110
        u8 colour[4] = {jo::as_camera(cam)->fog_colour.b, jo::as_camera(cam)->fog_colour.g, jo::as_camera(cam)->fog_colour.r, jo::as_camera(cam)->fog_colour.a};
        joan::orig::as_camera(camera)->set_light(joan::orig::arg::mptr(joan::orig::as_camera(cam)->light()));  // the light pointer as is (x64)  // 0x006169d0(0x00616b40)
        joan::orig::as_viewport(camera)->set_58(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_camera(cam)->get_58())));  // 0x00616a60(0x00616b60)
        joan::orig::as_camera(camera)->set_fog_colour(joan::orig::arg::mptr(colour));                           // 0x00616c10
        joan::orig::as_viewport(camera)->set_size(joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->fog_start)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->fog_end)));  // 0x00616a00
        joan::orig::as_camera(camera)->set_fog_range(joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->fog_density)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->fog_height_b)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->fog_height_a)));  // 0x00616a90
        u32 mirrored[10];
        mirrored[0] = view[0];
        mirrored[4] = bits(yaw);
        mirrored[5] = bits(pitch);
        mirrored[6] = bits(angle);
        mirrored[7] = view[7];
        mirrored[8] = view[8];
        mirrored[9] = view[9];
        import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&mirrored[1]), static_cast<void*>(&view[1]),
                           static_cast<void*>(m));  // point_transform(out, in, m)
        joan::orig::as_camera(camera)->set_frame(joan::orig::arg::mptr(mirrored));  // 0x00614040 set view
        joan::orig::as_camera(camera)->update_matrices();            // 0x006141a0

        clear(4);
        srs(0x34, 1);
        srs(0x39, 1);
        srs(0x35, 1);
        srs(0x36, 1);
        srs(0x38, 8);
        srs(0x37, 3);
        srs(0xa8, 0);
        if (com_call<i32>(global<void*>(d::g_game_device), 0x88) != 0) {  // BeginScene
            joan::orig::as_camera(camera)->destruct();                                            // 0x00613700 dtor
            return;
        }
        joan::orig::as_plane(self)->draw(joan::orig::arg::mptr(cam), 1, 0u);  // 0x005d0570
        joan::orig::as_plane(self)->draw(joan::orig::arg::mptr(cam), 2, 0u);
        srs(0xa8, 0xf);
        srs(0x38, 3);
        srs(0x37, 1);
        alignas(16) u32 world[16];
        {
            void* const w = joan::orig::arg::as<void*>(joan::orig::as_camera(cam)->view());  // 0x00616f20
            import_call<void*>(d::iat_v3d_87_matrix_multiply_3, static_cast<void*>(world), static_cast<void*>(m), w);
        }
        set_world(world);
        clear(2);
        joan::orig::as_camera(camera)->push_plane(joan::orig::arg::mptr(plane));  // 0x006170a0
        srs(0x16, 3);
        void* const scene = jo::as_camera(camera)->scene;
        if (scene != nullptr && joan::orig::arg::as<i32>(joan::orig::as_scene(scene)->active()) != 0)  // 0x005eb8b0
            joan::orig::as_scene(scene)->render(joan::orig::arg::mptr(camera));                  // 0x005ebac0
        srs(0x16, 2);
        set_world(joan::orig::arg::as<void*>(joan::orig::as_camera(cam)->view()));  // 0x00616f20
        srs(0x34, 0);
        com_call<i32>(global<void*>(d::g_game_device), 0x8c);  // EndScene
        joan::orig::as_camera(camera)->destruct();                                     // 0x00613700 dtor
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_09", name, addr, fn)
B("Plane::render_reflection", 0x005d6050, &H::draw_mirror_5d6050);
#undef B

}  // namespace game_b109_09
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
namespace {
namespace game_b103_11 {


struct H {
    // 0x005d6e50
    u32 field_100() { return jo::self<jo::Plane>(this)->flags_100; }
    // 0x005d6eb0
    u32 field_104() { return jo::self<jo::Plane>(this)->flags_104; }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Obj::field_100", 0x005d6e50, &H::field_100);
B103("Obj::field_104", 0x005d6eb0, &H::field_104);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/en3d_helpers20.cpp (en3d/helpers20)
namespace {
namespace en3d_helpers20 {


void mem_free(void* p, const char* file, std::int32_t line) { joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

struct H {
    // 0x005d7190 Plane::clear_extras(free_arrays).
    void plane_clear_extras(std::int32_t free_arrays) {
        jo::self<jo::Plane>(this)->strip_total = 0;
        jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->strip_index_count) = 0;
        if (free_arrays != 0) {
            const char* const file = &img::global<const char>(d::s_d_ww_en3d_plane_cpp);
            if (jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths) != nullptr) {
                mem_free(jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths), file, line_base(d::g_a173ec) + 0xc);
                jo::view<void*>(jo::self<jo::Plane>(this)->strip_lengths) = nullptr;
            }
            if (jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices) != nullptr) {
                mem_free(jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices), file, line_base(d::g_a173ec) + 0x11);
                jo::view<void*>(jo::self<jo::Plane>(this)->strip_indices) = nullptr;
            }
            jo::view<std::uint32_t>(jo::self<jo::Plane>(this)->strip_capacity) = 0;
        }
        jo::self<jo::Plane>(this)->has_strips = 0;
        jo::self<jo::Plane>(this)->use_strips = 0;
    }
};

#define HELPER20(name, addr, fn) JOAN_REPLACE("en3d/helpers20", name, addr, fn)
HELPER20("Plane::clear_extras", 0x005d7190, &H::plane_clear_extras);
#undef HELPER20

}  // namespace en3d_helpers20
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
//
//    - Plane::field_ec 0x005d72e0 (ret 0).
namespace {
namespace game_b98_06 {


struct H {
    // ------------------------------------------------------------------ 0x005d72e0
    void* field_ec() { return jo::self<jo::Plane>(this)->next; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Plane::get_shadow_plane", 0x005d72e0, &H::field_ec);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - Plane::field_f0 0x005d7380 (ret 0).
namespace {
namespace game_b99_07 {


struct H {
    // ------------------------------------------------------------------ 0x005d7380
    void* field_f0() { return jo::self<jo::Plane>(this)->linked; }

};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Plane::field_f0", 0x005d7380, &H::field_f0);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/en3d_b86_04.cpp (en3d/b86_04)
//
//      0x005d7480 Plane::face_normal(out, Vertex a, Vertex b, Vertex c) (ret 0xb8) - the three
//                 0x3c-byte vertices by value; 0x005d9be0 when byte +0xb; edges b-a / c-a made
//                 orthogonal to a's normal (+0xc, v3d dot_product), combined with the uv deltas
//                 (+0x1c / +0x20, sign flipped by the uv winding test), vector_normalize(out, in).
namespace {
namespace en3d_b86_04 {


// Plane vertex passed by value (0x3c bytes: +0 position, +0xc normal, +0x1c u, +0x20 v).
struct Vertex {
    float f[15];
};
static_assert(sizeof(Vertex) == 0x3c, "Vertex");

JOAN_COUNTER(c_face_normal, "Plane::face_normal");

struct H {
    // 0x005d7480
    float* face_normal(float* out, Vertex a, Vertex b, Vertex c) {
        c_face_normal.hit();
        if (jo::self<jo::Plane>(this)->data_pending != 0) joan::orig::as_plane(this)->load_5d9be0();  // 0x005d9be0
        float e1[3], e2[3];
        e1[0] = b.f[0] - a.f[0];
        e1[1] = b.f[1] - a.f[1];
        e1[2] = b.f[2] - a.f[2];
        e2[0] = c.f[0] - a.f[0];
        e2[1] = c.f[1] - a.f[1];
        e2[2] = c.f[2] - a.f[2];
        float n[3] = {a.f[3], a.f[4], a.f[5]};
        float d = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(n), static_cast<void*>(e1)));  // dot_product
        float t1[3];
        t1[0] = e1[0] - d * n[0];
        t1[1] = e1[1] - d * n[1];
        t1[2] = e1[2] - d * n[2];
        d = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(n), static_cast<void*>(e2)));
        float t2[3];
        t2[0] = e2[0] - d * n[0];
        t2[1] = e2[1] - d * n[1];
        t2[2] = e2[2] - d * n[2];
        float du1 = b.f[7] - a.f[7];  // [ebp-0x48]
        float du2 = c.f[7] - a.f[7];  // [ebp-0x4c]
        const float dv1 = b.f[8] - a.f[8];  // [ebp-0x50]
        const float dv2 = c.f[8] - a.f[8];  // [ebp-0x54]
        const float p1 = du2 * dv1;  // st1
        const float p0 = du1 * dv2;  // st0; fcompp, test ah,1 (st0 < st1 or unordered)
        if (!(p0 >= p1)) {
            du2 = -du2;
            du1 = -du1;
        }
        float r[3];
        r[0] = du2 * t1[0] - du1 * t2[0];
        r[1] = du2 * t1[1] - du1 * t2[1];
        r[2] = du2 * t1[2] - du1 * t2[2];
        import_call<void*>(d::iat_v3d_87_vector_normalize_2, static_cast<void*>(r), static_cast<void*>(r));  // vector_normalize(out, in)
        out[0] = r[0];
        out[1] = r[1];
        out[2] = r[2];
        return out;
    }
};

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b86_04", name, addr, fn)
EN3D("Plane::face_normal", 0x005d7480, &H::face_normal);
#undef EN3D

}  // namespace en3d_b86_04
}  // namespace

// ---------------------------------------- from modules/en3d_b85_03.cpp (en3d/b85_03)
//
//      0x005d7680 Plane::build_normals()     - 0x005d9be0 when byte +0xb; new +0x114 / +0x118
//                 (+0x10 * 12, zeroed); per triangle (+0x48 words, +0x40 count) 0x005d7480 face
//                 normal added to each corner; normalize +0x114, cross(+0x114 vertex +0xc) -> +0x118
namespace {
namespace en3d_b85_03 {


using joan::orig::helpers::mul;

using joan::orig::helpers::memset_;

struct Vertex {  // 0x3c-byte vertex passed by value to 0x005d7480
    std::uint32_t w[15];
};
static_assert(sizeof(Vertex) == 0x3c, "Vertex");

JOAN_COUNTER(c_normals, "Plane::build_normals");

struct H {
    // 0x005d7680
    void build_normals() {
        c_normals.hit();
        if (jo::self<jo::Plane>(this)->data_pending != 0) joan::orig::as_plane(this)->load_5d9be0();  // 0x005d9be0
        jo::view<void*>(jo::self<jo::Plane>(this)->face_normals) = game_new(static_cast<std::uint32_t>(mul(jo::self<jo::Plane>(this)->vertex_count, 0xc)));
        jo::view<void*>(jo::self<jo::Plane>(this)->vertex_normals) = game_new(static_cast<std::uint32_t>(mul(jo::self<jo::Plane>(this)->vertex_count, 0xc)));
        memset_(jo::view<void*>(jo::self<jo::Plane>(this)->face_normals), 0, static_cast<std::uint32_t>(mul(jo::self<jo::Plane>(this)->vertex_count, 0xc)));
        memset_(jo::view<void*>(jo::self<jo::Plane>(this)->vertex_normals), 0, static_cast<std::uint32_t>(mul(jo::self<jo::Plane>(this)->vertex_count, 0xc)));
        auto vertex = [&](std::int32_t k) -> Vertex { return *reinterpret_cast<const Vertex*>(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f2) + mul(k, 0x3c)); };
        auto accumulate = [&](std::int32_t k, const float* n) {
            float* const dst = reinterpret_cast<float*>(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->face_normals) + mul(k, 0xc));
            dst[0] = n[0] + dst[0];
            dst[1] = n[1] + dst[1];
            dst[2] = n[2] + dst[2];
        };
        for (std::int32_t i = 0; i < mul(jo::self<jo::Plane>(this)->triangle_count, 3); i += 3) {
            const std::int32_t a = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[i];
            const std::int32_t b = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[i + 1];
            const std::int32_t c = jo::view<std::uint16_t*>(jo::self<jo::Plane>(this)->triangles)[i + 2];
            float out[3];
            float n[3];
            const float* r = joan::orig::arg::as<const float*>(joan::orig::as_plane(this)->face_normal(joan::orig::arg::mptr(static_cast<void*>(out)), vertex(a), vertex(b), vertex(c)));  // 0x005d7480
            std::memcpy(n, r, 0xc);
            accumulate(a, n);
            r = joan::orig::arg::as<const float*>(joan::orig::as_plane(this)->face_normal(joan::orig::arg::mptr(static_cast<void*>(out)), vertex(b), vertex(a), vertex(c)));
            std::memcpy(n, r, 0xc);
            accumulate(b, n);
            r = joan::orig::arg::as<const float*>(joan::orig::as_plane(this)->face_normal(joan::orig::arg::mptr(static_cast<void*>(out)), vertex(c), vertex(a), vertex(b)));
            std::memcpy(n, r, 0xc);
            accumulate(c, n);
        }
        for (std::int32_t i = 0; i < jo::self<jo::Plane>(this)->vertex_count; ++i) {
            void* const normal = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->face_normals) + mul(i, 0xc);
            import_call<void*>(d::iat_v3d_87_vector_normalize_2, normal, normal);  // vector_normalize(out, in)
            std::uint32_t tmp[3];
            std::memcpy(tmp, jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f2) + mul(i, 0x3c) + 0xc, 0xc);
            import_call<void*>(d::iat_v3d_87_cross_product, static_cast<void*>(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertex_normals) + mul(i, 0xc)), static_cast<void*>(tmp),
                               static_cast<void*>(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->face_normals) + mul(i, 0xc)));  // cross_product(out, a, b)
        }
    }
};

#define B85_03(name, addr, fn) JOAN_REPLACE("en3d/b85_03", name, addr, fn)
B85_03("Plane::build_normals", 0x005d7680, &H::build_normals);
#undef B85_03

}  // namespace en3d_b85_03
}  // namespace

// ---------------------------------------- from modules/en3d_helpers9.cpp (en3d/helpers9)
namespace {
namespace en3d_helpers9 {

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

using joan::orig::helpers::at;

JOAN_COUNTER(c_env_map, "Plane::env_map");

struct H {
    // --- Plane (en3d/plane.cpp) -------------------------------------------------------------------
    // 0x005d7bf0: chrome / environment mapping - per vertex (+0x20, 0x3c each, count +0x10) the
    // direction from the vertex to the eye (the camera 0x005d80a0 of 0xa483f8: type 3 position
    // +0xc, type 1 orbit (+0x18, +0x1c) 500 units back from the bone), in the plane's space
    // (inverse bone matrix unless +0x18), projected on the vertex tangents (+0x114 / +0x118)
    // gives the second UV set (+0x24 / +0x28) from the first (+0x1c / +0x20) over the first
    // material's texture size (+0x24 -> +0x3c / +0x40).
    void plane_env_map() {
        c_env_map.hit();
        // 0x005d80a0 is a plain getter, so the camera type can be checked before 0x005d9be0.
        std::uint8_t* const eye_src = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_camera(img::obj(d::g_scene))->type());  // 0x005d80a0
        const jo::Light* const eye_cam = jo::self<jo::Light>(eye_src);  // the camera is a Light-shaped record (type, position, orbit angles)
        if (eye_cam->type != 3 && eye_cam->type != 1 && !fix_uninit()) {
            // The original leaves the eye position uninitialised for other camera types.
            // Batch 118/03: with fix/uninit enabled (module_enabled, read on the first call) the
            // C++ version runs instead with the eye at the origin (0, 0, 0).
            using Fn = void (H::*)();
            (this->*joan::hook::original<Fn>(fn::Plane_env_map_body))();
            return;
        }
        if (jo::self<jo::Plane>(this)->data_pending != 0) joan::orig::as_plane(this)->load_5d9be0();  // 0x005d9be0
        float world[16], inverse[16];
        if (jo::self<jo::Plane>(this)->transformed == 0) {
            std::memcpy(world, joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::self<jo::Plane>(this)->bones))->matrix()), 64);  // 0x0060ef80
            import_call<void*>(d::iat_v3d_87_matrix_inverse_2, inverse, world);                       // matrix_inverse
        } else {
            std::memcpy(world, img::obj(d::g_identity_matrix), 64);
            std::memcpy(inverse, img::obj(d::g_identity_matrix), 64);
        }
        Vec3 eye = {0.0f, 0.0f, 0.0f};  // other camera types (fix/uninit only): never written, 0
        if (eye_cam->type == 3) {
            std::memcpy(&eye, eye_cam->position, sizeof eye);  // 0x005d80a0 Light-shaped (Scene::ptr_b9); nothing writes ptr_b9 and env_map never ran in the replays
        } else if (eye_cam->type == 1) {
            float orbit[16], bone[16];
            joan::orig::orbit_matrix(joan::orig::arg::mptr(orbit), joan::orig::arg::f32(eye_cam->orbit_a), joan::orig::arg::f32(eye_cam->orbit_b), 0.0f, 0.0f);  // 0x0060bd00
            std::memcpy(bone, jo::view<void*>(jo::self<jo::Plane>(this)->bones), 64);
            const float k = global<float>(d::flt_500);  // 500
            eye.x = bone[12] - k * orbit[8];
            eye.y = bone[13] - k * orbit[9];
            eye.z = bone[14] - k * orbit[10];
        }
        Vec3 local;
        import_call<void*>(d::iat_v3d_87_point_transform_3, &local, &eye, inverse);  // point_transform
        const Vec3 e = local;
        std::uint8_t* const texture = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->materials->textures[0]);
        for (std::int32_t i = 0; i < jo::self<jo::Plane>(this)->vertex_count; ++i) {
            std::uint8_t* const v = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f2) + i * 0x3c;
            const Vec3 pos = jo::view<Vec3>(jo::as_vertex_f2(v)->pos);
            const Vec3 normal = *reinterpret_cast<const Vec3*>(&jo::as_vertex_f2(v)->attr[0]);
            Vec3 to_eye = {e.x - pos.x, e.y - pos.y, e.z - pos.z};
            Vec3 dir;
            import_call<void*>(d::iat_v3d_87_vector_normalize_2, &dir, &to_eye);  // vector_normalize(out, in)
            Vec3 n = normal;
            (void)import_call<double>(d::iat_v3d_87_dot_product, &dir, &n);  // dot_product (unused)
            float st[3];
            st[0] = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, &dir, jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->face_normals) + i * 12));
            st[1] = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, &dir, jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertex_normals) + i * 12));
            st[2] = 0.0f;
            import_call<void*>(d::iat_v3d_87_vector_normalize_2, st, st);
            jo::as_vertex_f2(v)->attr[6] = st[0] / static_cast<float>(jo::view<std::int32_t>(jo::as_texture(texture)->width)) + jo::as_vertex_f2(v)->attr[4];
            jo::as_vertex_f2(v)->attr[7] = jo::as_vertex_f2(v)->attr[5] - st[1] / static_cast<float>(jo::view<std::int32_t>(jo::as_texture(texture)->height));
        }
        joan::orig::as_plane(this)->invalidate(2u);  // 0x005cf920
    }
};

JOAN_REPLACE("en3d/helpers9", "Plane::get_effects_enabled", 0x005d7bf0, &H::plane_env_map);

}  // namespace en3d_helpers9
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
//
//      0x005d80d0 Plane::make_slope_texture         (thiscall, ret 0) - al. Dead code.
//
//  - 0x005d80d0: the word computed at [ebp-0x100] (from the centre texel) is never read.
namespace {
namespace game_b108_18 {

using uptr = std::uintptr_t;

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

JOAN_COUNTER(c_slope_tex, "Plane::make_slope_texture");

struct H {
    // ------------------------------------------------------------------ 0x005d80d0
    bool make_slope_texture() {
        c_slope_tex.hit();
        const i32 index = 0;
        u8* const source = jo::view<u8*>(jo::self<jo::Plane>(this)->materials->textures[0]);
        char name[100];  // [ebp-0x70]; names are 64-byte fields (+0x18 .. +0x58)
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(name))), joan::orig::arg::str(jo::as_texture(source)->name));  // strcpy
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(name))), str(d::s_EB));  // strcat "-EB"
        void* const found = joan::orig::arg::as<void*>(joan::orig::as_textures(obj(d::g_texture_array))->find(joan::orig::arg::str(static_cast<char*>(name))));  // 0x0060a4b0
        if (found != nullptr) {
            const i32 saved = global<i32>(d::g_texture_priority);
            global<i32>(d::g_texture_priority) = static_cast<i32>(0xffffec78u);
            joan::orig::as_material(jo::view<void*>(jo::self<jo::Plane>(this)->materials))->set_texture(joan::orig::arg::i32(index), joan::orig::arg::mptr(found));  // 0x005d5650
            global<i32>(d::g_texture_priority) = saved;
            return true;
        }
        u8* const made = joan::orig::arg::as<u8*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->create_2d());  // 0x006094c0
        void* const src = jo::view<void*>(jo::as_texture2d(source)->d3d_texture);
        u32 desc[8] = {};  // D3DSURFACE_DESC
        com_call<u32>(src, 0x38 / 4, 0u, static_cast<void*>(desc));  // GetLevelDesc(0)
        const u32 w = desc[6];
        const u32 h = desc[7];
        joan::orig::as_texture(made)->create(joan::orig::arg::u32(w), joan::orig::arg::u32(h), 1u, 0x1fu);  // 0x00605e20
        if (jo::view<void*>(jo::as_texture2d(made)->d3d_texture) == nullptr) return false;
        struct LockedRect { i32 pitch; uptr bits; };  // D3DLOCKED_RECT (pBits: 8 bytes at +0x8 on x64)
        LockedRect src_lock{};
        com_call<u32>(src, 0x40 / 4, 0u, static_cast<void*>(&src_lock), static_cast<void*>(nullptr), 0u);  // LockRect
        const u32 src_pitch = static_cast<u32>(src_lock.pitch);
        const uptr src_bits = src_lock.bits;
        uptr row = src_bits;
        const uptr last_row = src_bits + src_pitch * (h - 1);
        LockedRect dst_lock{};
        com_call<u32>(jo::view<void*>(jo::as_texture2d(made)->d3d_texture), 0x40 / 4, 0u, static_cast<void*>(&dst_lock), static_cast<void*>(nullptr),
                      0u);  // LockRect
        const u32 dst_pitch = static_cast<u32>(dst_lock.pitch);
        uptr dst_row = dst_lock.bits;
        auto byte_at = [](uptr address) { return static_cast<u32>(*reinterpret_cast<const u8*>(address)); };
        for (u32 y = 0; y < h; ++y) {
            uptr cur = row;
            uptr next = y == h - 1 ? src_bits : row + src_pitch;
            uptr prev = y == 0 ? last_row : row - src_pitch;
            uptr out = dst_row;
            for (u32 x = 0; x < w; ++x) {
                const u32 right = x == w - 1 ? byte_at(row) : byte_at(cur + 4);
                const u32 left = x == 0 ? byte_at(row + w * 4 - 4) : byte_at(cur - 4);
                const u32 below = byte_at(next);
                const u32 above = byte_at(prev);
                *reinterpret_cast<u8*>(out) = static_cast<u8>(left - right);
                *reinterpret_cast<u8*>(out + 1) = static_cast<u8>(above - below);
                out += 2;
                cur += 4;
                next += 4;
                prev += 4;
            }
            row += src_pitch;
            dst_row += dst_pitch;
        }
        com_call<u32>(jo::view<void*>(jo::as_texture2d(made)->d3d_texture), 0x44 / 4, 0u);  // UnlockRect
        com_call<u32>(src, 0x44 / 4, 0u);                    // UnlockRect
        joan::orig::as_texture(made)->set_name(joan::orig::arg::str(static_cast<char*>(name)));  // 0x00605ad0
        const i32 saved = global<i32>(d::g_texture_priority);
        global<i32>(d::g_texture_priority) = static_cast<i32>(0xffffec78u);
        joan::orig::as_material(jo::view<void*>(jo::self<jo::Plane>(this)->materials))->set_texture(joan::orig::arg::i32(index), joan::orig::arg::mptr(made));  // 0x005d5650
        global<i32>(d::g_texture_priority) = saved;
        joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(made));  // 0x0060a550
        return true;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Plane::init_env_bump_map", 0x005d80d0, &H::make_slope_texture);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/en3d_b117_02.cpp (en3d/b117_02)
//
//    - Plane::water_texture_5d8650 0x005d8650 (material index, camera, render texture, out matrix),
//      ret 0x10 -> al (1 = texture rendered, out matrix written). Only caller:
//      Camera::water_receivers 0x00617370 (C++ in en3d_helpers22.cpp, through the thunk 0x0040894a).
//      Takes the first triangle of the material's run (start 0 for material 0, else
//      (+0xb0[mi - 1] + 1) * 3 into the index list +0x48), its three positions from the vertex
//      array of the plane's vertex format (+4: 1 / 2 / 4 / 5 / 8 -> +0x1c / +0x20 / +0x28 / +0x2c /
//      +0x38, strides 0x24 / 0x3c / 0x18 / 0x48 / 0x28; any other format -> 0), the plane through
//      them (0x0060d920) and its reflection matrix (D3DXMatrixReflect 0x00768e60). Returns 0 when
//      the camera position is behind the plane. Builds the mirrored camera orientation (yaw
//      0x00421020 / pitch 0x00425610 of the reflected view direction, clamped to +-pi/2), grows the
//      shared depth buffer to the texture size if needed (SysEn 0x00603380 / 0x006032d0; 0 when no
//      depth surface), redirects the device to the texture's level 0 surface, sets up a local
//      0x4ca-byte Camera (0x00613650 .. 0x00613700) with the reflected view, then either user clip
//      planes (0x006179a0, [0xa52733] >= 1; plane shifted 2 units, transformed into clip space when
//      [0xa52514] != 0) or an oblique near-plane projection (when the view direction faces the
//      plane), scales the projection by 0.95 in x / y, clears, draws the scene (0x005ebac0) with the
//      reflected world, restores render target / viewport / transforms, and writes
//      out = projection' * (0.5, -0.5 scale + 0.5 offset) texture matrix.
namespace {
namespace en3d_b117_02 {

using u64 = std::uint64_t;

// COM method (stdcall, interface pointer first), by byte offset into the vtable.
template <class R = void, class... A>
R com_call(void* iface, u32 off, A... args) {
    const std::uintptr_t* vt = *reinterpret_cast<std::uintptr_t* const*>(iface);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(vt[joan::call::x86_vslot(off)])(iface, args...);
}
// The plane's vertex array at `off` (offsetof(Plane, vertices_fN)).
u8* vertex_array(const void* plane, std::int32_t off) { return *reinterpret_cast<u8* const*>(static_cast<const u8*>(plane) + off); }
float flt(u32 v) {
    float r;
    std::memcpy(&r, &v, 4);
    return r;
}
// x87 NaN propagation when both operands are NaN (V3D.md, measured): a QNaN beats an SNaN, else the
// larger significand wins (quietened), equal significands -> the positive one. SSE would return the
// first operand. One NaN operand / invalid operations (default NaN 0xffc00000) agree already.
float x87_nan(float a, float b, float r) {
    if (a == a || b == b) return r;
    const u32 x = bits(a), y = bits(b);
    const bool qx = (x & 0x00400000u) != 0, qy = (y & 0x00400000u) != 0;
    u32 w;
    if (qx != qy)
        w = qx ? x : y;
    else if ((x & 0x007fffffu) != (y & 0x007fffffu))
        w = (x & 0x007fffffu) > (y & 0x007fffffu) ? x : y;
    else
        w = (x & 0x80000000u) != 0 ? y : x;
    return flt(w | 0x00400000u);
}
double x87_nan(double a, double b, double r) {
    if (a == a || b == b) return r;
    u64 x, y;
    std::memcpy(&x, &a, 8);
    std::memcpy(&y, &b, 8);
    constexpr u64 kFrac = 0x000fffffffffffffull, kSign = 0x8000000000000000ull, kQuiet = 0x0008000000000000ull;
    const bool qx = (x & kQuiet) != 0, qy = (y & kQuiet) != 0;
    u64 w;
    if (qx != qy)
        w = qx ? x : y;
    else if ((x & kFrac) != (y & kFrac))
        w = (x & kFrac) > (y & kFrac) ? x : y;
    else
        w = (x & kSign) != 0 ? y : x;
    w |= kQuiet;
    double d;
    std::memcpy(&d, &w, 8);
    return d;
}
// One x87 op on float operands at PC24 (= one IEEE single op), NaN-propagation as on the x87.
float fadd(float a, float b) { return x87_nan(a, b, a + b); }
float fsub(float a, float b) { return x87_nan(a, b, a - b); }
float fmul(float a, float b) { return x87_nan(a, b, a * b); }
float fdiv(float a, float b) { return x87_nan(a, b, a / b); }
float fchs(float a) { return flt(bits(a) ^ 0x80000000u); }
u16 vread16(std::uintptr_t a) { return *reinterpret_cast<const volatile u16*>(a); }
u32 vread32(std::uintptr_t a) { return *reinterpret_cast<const volatile u32*>(a); }

constexpr img::Addr kIdentity = d::g_identity_matrix;  // identity D3DMATRIX

// IDirect3DDevice8 / IUnknown / IDirect3DTexture8 vtable offsets.
constexpr u32 kRelease = 0x08, kSetRenderTarget = 0x7c, kGetRenderTarget = 0x80, kGetDepthStencilSurface = 0x84,
              kBeginScene = 0x88, kEndScene = 0x8c, kClear = 0x90, kSetTransform = 0x94, kSetViewport = 0xa0,
              kGetViewport = 0xa4, kGetSurfaceLevel = 0x3c;
constexpr u32 kView = 2, kProjection = 3;  // D3DTS_VIEW / D3DTS_PROJECTION

static_assert(sizeof(V3) == 12, "vec3");

JOAN_COUNTER(c_water_texture, "Plane::water_texture_5d8650");

// Local Camera (0x4ca bytes, 0x00613650 ctor) destroyed by 0x00613700 on every exit (SEH state 0).
struct LocalCamera {
    alignas(16) u8 bytes[((sizeof(jo::Camera) > 0x4cc ? sizeof(jo::Camera) : 0x4cc) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // 0x4ea on x64
    void construct() { joan::orig::as_camera(bytes)->construct(); }  // 0x00613650
};
struct CameraOwner {
    u8* camera;
    ~CameraOwner() { joan::orig::as_camera(camera)->destruct(); }  // 0x00613700
};

struct H {
    // 0x005d8650
    u8 water_texture_5d8650(u32 mi, u8* cam, u8* tex, void* out) {
        c_water_texture.hit();
        void* const self = this;

        // ---- first triangle of the material's run
        i32 first = 0;
        if (mi == 0) {
            first = 0;
            if (jo::self<jo::Plane>(this)->material_count == 1)
                (void)vread32(reinterpret_cast<std::uintptr_t>(&jo::as_plane(self)->triangle_count));  // dead end index
            else
                (void)vread16(reinterpret_cast<std::uintptr_t>(jo::as_plane(self)->material_first));  // dead end index
        } else {
            const std::uintptr_t runs = reinterpret_cast<std::uintptr_t>(jo::as_plane(self)->material_first);
            first = (static_cast<i32>(vread16(runs + mi * 2u - 2u)) + 1) * 3;
            (void)vread16(runs + mi * 2u);  // dead end index
        }
        i32 array_off = 0;
        u32 stride = 0;
        switch (static_cast<u32>(static_cast<i32>(jo::self<jo::Plane>(this)->vertex_format) - 1)) {
            case 0:
                array_off = static_cast<i32>(offsetof(jo::Plane, vertices_f1));
                stride = static_cast<u32>(sizeof(jo::VertexF1));
                break;
            case 1:
                array_off = static_cast<i32>(offsetof(jo::Plane, vertices_f2));
                stride = static_cast<u32>(sizeof(jo::VertexF2));
                break;
            case 3:
                array_off = static_cast<i32>(offsetof(jo::Plane, vertices_f4));
                stride = static_cast<u32>(sizeof(jo::VertexF4));
                break;
            case 4:
                array_off = static_cast<i32>(offsetof(jo::Plane, vertices_f5));
                stride = static_cast<u32>(sizeof(jo::VertexF5));
                break;
            case 7:
                array_off = static_cast<i32>(offsetof(jo::Plane, vertices_f8));
                stride = static_cast<u32>(sizeof(jo::VertexF8));
                break;
            default:
                return 0;
        }
        V3 a, b, c;  // [ebp-0x24] / [ebp-0x30] / [ebp-0x3c]
        {
            const std::uintptr_t indices = reinterpret_cast<std::uintptr_t>(jo::as_plane(self)->triangles) + static_cast<u32>(first) * 2u;
            const u32 i0 = vread16(indices);
            std::memcpy(&a, vertex_array(self, array_off) + i0 * stride, 12);
            const u32 i1 = vread16(indices + 2u);
            std::memcpy(&b, vertex_array(self, array_off) + i1 * stride, 12);
            const u32 i2 = vread16(indices + 4u);
            std::memcpy(&c, vertex_array(self, array_off) + i2 * stride, 12);
        }

        // ---- reflection plane / matrix
        alignas(16) u32 plane[4];                         // [ebp-0x4c]
        joan::orig::as_plane_equation(plane)->init_425700();               // 0x00425700
        joan::orig::plane_from_points(joan::orig::arg::mptr(plane), &a, &b, &c); // 0x0060d920 plane from points
        alignas(16) u32 reflect[16];                      // [ebp-0x8c]
        joan::orig::as_matrix(reflect)->construct();             // 0x005bea30
        joan::orig::d3dx::matrix_reflect(joan::orig::arg::mptr(reflect), joan::orig::arg::mptr(plane));  // D3DXMatrixReflect
        alignas(16) u32 m[16];                            // [ebp-0xcc]
        std::memcpy(m, reflect, sizeof m);

        void* const frame = joan::orig::arg::as<void*>(joan::orig::as_camera(cam)->matrix());  // 0x00616e90
        V3 pos, axis_x, axis_y, dir;                           // [ebp-0xdc] / -0xe8 / -0xf4 / -0x100
        joan::orig::matrix_position(&pos, joan::orig::arg::ptr(frame));            // 0x0060c310
        joan::orig::matrix_axis_x(&axis_x, joan::orig::arg::mptr(frame));         // 0x0060c180
        joan::orig::matrix_row1(&axis_y, joan::orig::arg::mptr(frame));         // 0x0060c1d0
        joan::orig::matrix_axis_z(&dir, joan::orig::arg::ptr(frame));            // 0x0060c220
        V3 normal;                                             // [ebp-0x10c]
        joan::orig::plane_normal(&normal, joan::orig::arg::mptr(plane));         // 0x0060d9e0
        const float dist = joan::orig::arg::as<float>(joan::orig::plane_distance(joan::orig::arg::mptr(plane), &pos));  // 0x0060da30
        if (!(dist >= 0.0f)) return 0;

        u32 view[10];                             // [ebp-0x138]
        joan::orig::as_camera(cam)->frame(joan::orig::arg::mptr(view));   // 0x006140a0
        alignas(16) u32 rot[16];                  // [ebp-0x178]
        joan::orig::orbit_matrix(joan::orig::arg::mptr(rot), joan::orig::arg::f32(view[4]), joan::orig::arg::f32(view[5]), joan::orig::arg::f32(view[6]), joan::orig::arg::f32(0u));  // 0x0060bd00
        import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(&dir), static_cast<void*>(m));  // vector_transform
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(&dir));                         // vector_normalize
        import_call<void*>(d::iat_v3d_87_matrix_multiply_3, static_cast<void*>(rot), static_cast<void*>(m),
                           static_cast<void*>(rot));  // matrix_multiply(out, a, b)
        const float yaw = joan::orig::arg::as<float>(joan::orig::libc::atan2_f(joan::orig::arg::f32(dir.v[1]), joan::orig::arg::f32(dir.v[0])));  // 0x00421020
        const u32 neg_z = dir.v[2] ^ 0x80000000u;                             // fchs
        u32 pitch;
        if (!(flt(neg_z) > -1.0f))
            pitch = 0xbfc90fdbu;  // -pi/2
        else if (!(flt(neg_z) < 1.0f))
            pitch = 0x3fc90fdbu;  // pi/2
        else
            pitch = bits(joan::orig::arg::as<float>(joan::orig::libc::asin_f(joan::orig::arg::f32(neg_z))));  // 0x00425610

        // ---- render target at least as large as the texture
        if (global<i32>(d::g_a5273b) < jo::view<i32>(jo::as_texture(tex)->width) || global<i32>(d::g_a5273f) < jo::view<i32>(jo::as_texture(tex)->height)) {
            const i32 w = global<i32>(d::g_a5273b) > jo::view<i32>(jo::as_texture(tex)->width) ? global<i32>(d::g_a5273b) : jo::view<i32>(jo::as_texture(tex)->width);
            const i32 h = global<i32>(d::g_a5273f) > jo::view<i32>(jo::as_texture(tex)->height) ? global<i32>(d::g_a5273f) : jo::view<i32>(jo::as_texture(tex)->height);
            joan::orig::as_render_target(img::obj(kSysEn))->release_depth();        // 0x00603380
            joan::orig::as_render_target(img::obj(kSysEn))->create_depth(joan::orig::arg::u32(w), joan::orig::arg::u32(h));  // 0x006032d0
            if (global<void*>(d::g_a52737) == nullptr) return 0;
        }
        void* old_target = nullptr;  // [ebp-0x18c]
        void* old_depth = nullptr;   // [ebp-0x190]
        u32 viewport[6];             // [ebp-0x1b0]
        void* surface = nullptr;     // [ebp-0x1b4]
        com_call<i32>(device(), kGetRenderTarget, &old_target);
        com_call<i32>(device(), kGetDepthStencilSurface, &old_depth);
        com_call<i32>(device(), kGetViewport, static_cast<void*>(viewport));
        com_call<i32>(jo::view<void*>(jo::as_render_texture(tex)->d3d_texture), kGetSurfaceLevel, 0u, &surface);  // the render-target texture
        com_call<i32>(device(), kSetRenderTarget, surface, global<void*>(d::g_a52737));

        alignas(16) u32 inv[16];  // [ebp-0x1f4]
        import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), static_cast<void*>(rot));  // matrix_inverse
        alignas(16) u32 turn[16];  // [ebp-0x234]
        joan::orig::orbit_matrix(joan::orig::arg::mptr(turn), joan::orig::arg::f32(bits(yaw)), joan::orig::arg::f32(pitch), joan::orig::arg::f32(0u), joan::orig::arg::f32(0u));  // 0x0060bd00
        u32 axis[3] = {0u, 0u, 0x3f800000u};  // [ebp-0x240]
        u32 angle = 0;                        // [ebp-0x188] (never read)
        {
            void* const prod = import_call<void*>(d::iat_v3d_87_matrix_multiply_2, static_cast<void*>(inv), static_cast<void*>(turn));  // matrix_multiply
            joan::orig::matrix_axis_angle(joan::orig::arg::mptr(prod), joan::orig::arg::mptr(axis), &angle);  // 0x0060c3b0 axis / angle
        }

        // ---- mirrored camera
        LocalCamera local;  // [ebp-0x70c]
        local.construct();
        u8* const camera = local.bytes;
        CameraOwner owner{camera};
        joan::orig::as_sys_en(camera)->init_613730();  // 0x00613730
        joan::orig::as_camera(camera)->set_viewport(jo::as_camera(cam)->target, jo::as_camera(cam)->depth_target, joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->view_x1)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->view_y1)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->view_x2)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->view_y2)));                                       // 0x00613bf0
        joan::orig::as_camera(camera)->set_ranges(joan::orig::arg::f32(jo::view<u32>(jo::as_camera(cam)->near_plane)), joan::orig::arg::f32(jo::view<u32>(jo::as_camera(cam)->far_perspective)), joan::orig::arg::f32(jo::view<u32>(jo::as_camera(cam)->far_ortho)));  // 0x00613ca0
        joan::orig::as_camera(camera)->set_cf(0u, 1u, joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->clear_colour)));                              // 0x00613d30
        joan::orig::as_camera(camera)->set_scene(jo::as_camera(cam)->scene);                                          // 0x00613db0
        const u32 scene_state = joan::orig::arg::as<u32>(joan::orig::as_scene(jo::as_camera(cam)->scene)->shadows_on());  // 0x005ee0d0 ([ebp-0x710])
        joan::orig::as_scene(jo::as_camera(cam)->scene)->set_fog_flag(0);                     // 0x005ee140
        jo::as_camera(camera)->u32_28 = jo::as_camera(cam)->u32_28;
        {
            const i32 i40 = ftol(jo::as_camera(cam)->angle_extra[1]);
            const i32 i3c = ftol(jo::as_camera(cam)->angle_extra[0]);
            const i32 i38 = ftol(jo::as_camera(cam)->angle_max[1]);
            const i32 i34 = ftol(jo::as_camera(cam)->angle_max[0]);
            const i32 i30 = ftol(jo::as_camera(cam)->angle_min[1]);
            const i32 i2c = ftol(jo::as_camera(cam)->angle_min[0]);
            joan::orig::as_viewport(camera)->set_int(joan::orig::arg::i32(i2c), joan::orig::arg::i32(i30), joan::orig::arg::i32(i34), joan::orig::arg::i32(i38), joan::orig::arg::i32(i3c), joan::orig::arg::i32(i40));  // 0x00614110
        }
        u8 colour[4];  // [ebp-0x714]
        colour[0] = jo::as_camera(cam)->fog_colour.b;
        colour[1] = jo::as_camera(cam)->fog_colour.g;
        colour[2] = jo::as_camera(cam)->fog_colour.r;
        colour[3] = jo::as_camera(cam)->fog_colour.a;
        {
            void* const v = joan::orig::arg::as<void*>(joan::orig::as_camera(cam)->light());  // 0x00616b40 (a pointer: kept whole on x64)
            joan::orig::as_camera(camera)->set_light(joan::orig::arg::mptr(v));         // 0x006169d0
        }
        {
            const u32 v = joan::orig::arg::as<u32>(joan::orig::as_camera(cam)->get_58());  // 0x00616b60
            joan::orig::as_viewport(camera)->set_58(joan::orig::arg::i32(v));         // 0x00616a60
        }
        joan::orig::as_camera(camera)->set_fog_colour(joan::orig::arg::mptr(static_cast<void*>(colour)));                // 0x00616c10
        joan::orig::as_viewport(camera)->set_size(joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->fog_start)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->fog_end)));     // 0x00616a00
        joan::orig::as_camera(camera)->set_fog_range(joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->fog_density)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->fog_height_b)), joan::orig::arg::u32(jo::view<u32>(jo::as_camera(cam)->fog_height_a)));  // 0x00616a90
        u32 mirrored[10];  // [ebp-0x73c]
        mirrored[0] = view[0];
        mirrored[4] = bits(yaw);
        mirrored[5] = pitch;
        mirrored[6] = 0u;
        mirrored[7] = view[7];
        mirrored[8] = view[8];
        mirrored[9] = view[9];
        import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&mirrored[1]), static_cast<void*>(&view[1]),
                           static_cast<void*>(m));  // point_transform(out, in, m)
        joan::orig::as_camera(camera)->set_frame(joan::orig::arg::mptr(static_cast<void*>(mirrored)));  // 0x00614040
        joan::orig::as_camera(camera)->update_matrices();                                // 0x006141a0
        jo::as_camera(camera)->mirror = 1;
        {
            void* const r = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::as_camera(camera)->bone_)->matrix());  // 0x0060ef80
            jo::view<u32>(jo::as_camera(camera)->mirror_z) = static_cast<const u32*>(r)[14];  // the matrix's _43
        }
        void* const saved98 = (jo::as_bone(jo::as_camera(camera)->bone_))->callback;  // [ebp-0x740]
        (jo::as_bone(jo::as_camera(camera)->bone_))->callback = nullptr;
        {
            const u32* src = reinterpret_cast<const u32*>(jo::as_camera(cam)->bone_);
            u32* dst = reinterpret_cast<u32*>(jo::as_camera(camera)->bone_);
            for (i32 i = 0; i < 16; ++i) dst[i] = src[i];  // rep movsd
        }
        joan::orig::as_bone(jo::as_camera(camera)->bone_)->invalidate();  // 0x0060ee10
        joan::orig::as_bone(jo::as_camera(camera)->bone_)->update();  // 0x0060ee60
        (jo::as_bone(jo::as_camera(camera)->bone_))->callback = saved98;

        if (com_call<i32>(device(), kBeginScene) != 0) return 0;  // owner: 0x00613700

        alignas(16) u32 proj[16];  // [ebp-0x7c0]
        std::memcpy(proj, joan::orig::arg::as<void*>(joan::orig::as_dialog(cam)->reset_name()), sizeof proj);  // 0x00616f80
        alignas(16) u32 world[16];  // [ebp-0x780]
        float* const pf = reinterpret_cast<float*>(proj);
        if (global<i32>(d::g_a52733) >= 1) {
            {
                void* const v = joan::orig::arg::as<void*>(joan::orig::as_camera(cam)->view());  // 0x00616f20
                import_call<void*>(d::iat_v3d_87_matrix_multiply_3, static_cast<void*>(world), static_cast<void*>(m), v);
            }
            alignas(16) u32 clip_world[4];  // [ebp-0x7e8]
            alignas(16) u32 clip_space[4];  // [ebp-0x7f8]
            joan::orig::as_plane_equation(clip_world)->init_425700();  // 0x00425700
            joan::orig::as_plane_equation(clip_space)->init_425700();  // 0x00425700
            plane[3] = bits(fadd(flt(plane[3]), 2.0f));
            std::memcpy(clip_world, plane, 16);
            if (global<u32>(d::g_a52514) != 0) {
                V3 point, n;  // [ebp-0x7cc] / [ebp-0x7d8]
                import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&point), static_cast<void*>(&a),
                                   static_cast<void*>(world));  // point_transform(out, in, m)
                import_call<void*>(d::iat_v3d_87_vector_transform_3, static_cast<void*>(&n), static_cast<void*>(&normal),
                                   static_cast<void*>(world));                      // vector_transform(out, in, m)
                joan::orig::vec_cross(joan::orig::arg::mptr(clip_space), &point, &n);  // 0x0060d990 plane from point / normal
                float* const q = reinterpret_cast<float*>(clip_space);
                q[0] = fdiv(q[0], pf[0]);
                q[1] = fdiv(q[1], pf[5]);
                const float old_c = q[2];  // [ebp-0x7fc]
                const float old_d = q[3];  // [ebp-0x800]
                q[2] = fdiv(old_d, pf[14]);
                const float t0 = fdiv(old_d, pf[14]);
                const float t1 = fmul(t0, pf[10]);
                q[3] = fsub(old_c, t1);
            } else {
                std::memcpy(clip_space, plane, 16);
            }
            joan::orig::as_camera(camera)->set_clip_plane(0u, 1u, joan::orig::arg::mptr(static_cast<void*>(clip_world)), joan::orig::arg::mptr(static_cast<void*>(clip_space)));  // 0x006179a0
        } else {
            const double facing = import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(&normal), static_cast<void*>(&dir));  // dot_product
            if (facing > 0.0) {
                alignas(16) u32 p2[4];             // [ebp-0x828]
                joan::orig::as_plane_equation(p2)->init_425700();   // 0x00425700
                V3 tmp1, tmp2, e1, e2;             // [ebp-0x964] / [ebp-0x97c] / [ebp-0x958] / [ebp-0x970]
                std::memcpy(&e1, joan::orig::arg::as<void*>(joan::orig::vec_sub(&tmp1, &c, &a)), 12);  // 0x0060cc90 a - b
                std::memcpy(&e2, joan::orig::arg::as<void*>(joan::orig::vec_sub(&tmp2, &b, &a)), 12);
                V3 n;  // [ebp-0x80c]
                import_call<void*>(d::iat_v3d_87_cross_product, static_cast<void*>(&n), static_cast<void*>(&e2),
                                   static_cast<void*>(&e1));          // cross_product
                import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(&n));  // vector_normalize
                {
                    void* const v = joan::orig::arg::as<void*>(joan::orig::as_camera(cam)->view());  // 0x00616f20
                    import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(&n), v);  // vector_transform
                }
                V3 point;  // [ebp-0x818]
                {
                    void* const v = joan::orig::arg::as<void*>(joan::orig::as_camera(cam)->view());  // 0x00616f20
                    import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&point), static_cast<void*>(&a), v);
                }
                joan::orig::vec_cross(joan::orig::arg::mptr(p2), &point, &n);    // 0x0060d990
                joan::orig::d3dx::matrix_reflect(joan::orig::arg::mptr(reflect), joan::orig::arg::mptr(p2));     // D3DXMatrixReflect
                alignas(16) u32 m2[16];                           // [ebp-0x868]
                std::memcpy(m2, reflect, sizeof m2);
                {
                    void* const v = joan::orig::arg::as<void*>(joan::orig::as_camera(cam)->view());  // 0x00616f20
                    import_call<void*>(d::iat_v3d_87_matrix_multiply_3, static_cast<void*>(world), v, static_cast<void*>(m2));
                }
                std::memcpy(proj, joan::orig::arg::as<void*>(joan::orig::as_dialog(cam)->reset_name()), sizeof proj);  // 0x00616f80
                const float* const pl = reinterpret_cast<const float*>(p2);
                const float* const nf = reinterpret_cast<const float*>(&n);
                const float neg_d = fchs(pl[3]);
                const float near_z = fdiv(neg_d, pl[2]);              // [ebp-0x86c]
                const float zn = jo::as_camera(cam)->far_plane;                // [ebp-0x870]
                const float sx = fdiv(nf[0], nf[2]);                  // [ebp-0x874]
                const float sy = fdiv(nf[1], nf[2]);                  // [ebp-0x878]
                const joan::hook::x87::Fpu fpu{true};
                float kx;  // [ebp-0x87c]
                {
                    const double r = joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(bits(sx))));  // 0x00427d60
                    const double t = x87_nan(r, static_cast<double>(zn), fpu.mul(r, zn));
                    const double u = fpu.div(1.0, pf[0]);
                    kx = static_cast<float>(x87_nan(t, u, fpu.mul(t, u)));
                }
                float ky;  // [ebp-0x880]
                {
                    const double r = joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(bits(sy))));  // 0x00427d60
                    const double t = x87_nan(r, static_cast<double>(zn), fpu.mul(r, zn));
                    const double u = fpu.div(1.0, pf[5]);
                    ky = static_cast<float>(x87_nan(t, u, fpu.mul(t, u)));
                }
                float s = fadd(zn, kx);
                s = fadd(s, ky);
                s = fsub(s, near_z);
                const float scale = fdiv(zn, s);  // [ebp-0x884]
                pf[2] = fmul(sx, scale);
                pf[6] = fmul(sy, scale);
                proj[10] = bits(scale);
                const float neg_near = fchs(near_z);
                pf[14] = fmul(neg_near, scale);
                proj[3] = 0u;
                proj[7] = 0u;
                proj[11] = 0x3f800000u;
                proj[15] = 0u;
                com_call<i32>(device(), kSetTransform, kProjection, static_cast<void*>(proj));
            } else {
                void* const v = joan::orig::arg::as<void*>(joan::orig::as_camera(cam)->view());  // 0x00616f20
                import_call<void*>(d::iat_v3d_87_matrix_multiply_3, static_cast<void*>(world), static_cast<void*>(m), v);
            }
        }

        // ---- draw the reflection
        alignas(16) u32 shrink[16];  // [ebp-0x8c4]
        std::memcpy(shrink, img::obj(kIdentity), sizeof shrink);
        shrink[5] = 0x3f733333u;  // 0.95
        shrink[0] = shrink[5];
        alignas(16) u32 proj2[16];  // [ebp-0x904]
        import_call<void*>(d::iat_v3d_87_matrix_multiply_3, static_cast<void*>(proj2), static_cast<void*>(proj), static_cast<void*>(shrink));
        com_call<i32>(device(), kSetTransform, kView, static_cast<void*>(world));
        com_call<i32>(device(), kSetTransform, kProjection, static_cast<void*>(proj2));
        u32 clear_colour = 0;                                    // [ebp-0x908]
        joan::orig::as_colour(colour)->to_argb(&clear_colour);      // 0x005e91e0
        com_call<i32>(device(), kClear, 0u, static_cast<const void*>(nullptr), 7u, clear_colour, 1.0f, 0u);  // pRects: a pointer (8 bytes on x64), Z: a float
        joan::orig::as_camera(camera)->push_plane(joan::orig::arg::mptr(static_cast<void*>(plane)));  // 0x006170a0
        joan::orig::as_sys_en(img::obj(kSysEn))->set_cull_flip(1);                         // 0x00603140
        if (jo::as_camera(camera)->scene != nullptr && joan::orig::arg::as<i32>(joan::orig::as_scene(jo::as_camera(camera)->scene)->active()) != 0)  // 0x005eb8b0
            joan::orig::as_scene(jo::as_camera(camera)->scene)->render(joan::orig::arg::mptr(camera));                              // 0x005ebac0
        joan::orig::as_sys_en(img::obj(kSysEn))->set_cull_flip(0);                         // 0x00603140
        com_call<i32>(device(), kEndScene);
        com_call<i32>(device(), kSetRenderTarget, old_target, old_depth);
        com_call<i32>(device(), kSetViewport, static_cast<void*>(viewport));
        com_call<i32>(device(), kSetTransform, kView, joan::orig::arg::as<void*>(joan::orig::as_camera(cam)->view()));        // 0x00616f20
        com_call<i32>(device(), kSetTransform, kProjection, joan::orig::arg::as<void*>(joan::orig::as_dialog(cam)->reset_name()));  // 0x00616f80
        com_call<u32>(old_target, kRelease);
        com_call<u32>(old_depth, kRelease);
        com_call<u32>(surface, kRelease);
        joan::orig::as_scene(jo::as_camera(cam)->scene)->set_fog_flag(joan::orig::arg::i32(scene_state));  // 0x005ee140

        alignas(16) u32 bias[16];  // [ebp-0x948]
        std::memcpy(bias, img::obj(kIdentity), sizeof bias);
        bias[0] = 0x3f000000u;   // 0.5
        bias[5] = 0xbf000000u;   // -0.5
        bias[8] = 0x3f000000u;   // 0.5
        bias[9] = 0x3f000000u;   // 0.5
        import_call<void*>(d::iat_v3d_87_matrix_multiply_3, out, static_cast<void*>(proj2), static_cast<void*>(bias));
        return 1;  // owner: 0x00613700
    }
};

#define B(name, addr, fn) JOAN_REPLACE("en3d/b117_02", name, addr, fn)
B("Plane::render_reflection_to_texture", 0x005d8650, &H::water_texture_5d8650);
#undef B

}  // namespace en3d_b117_02
}  // namespace

// ---------------------------------------- from modules/en3d_b117_03.cpp (en3d/b117_03)
//
//      0x005e67b0 Reorder::draw(Scene* scene, Camera* ctx, int pass) (ret 0xc)
namespace {
namespace en3d_b117_03 {

// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    using Fn = R(JOAN_STDCALL*)(void*, A...);
    return reinterpret_cast<Fn>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}

// (functions, not namespace-scope constants: no dynamic initialisation needed)
void* sysen() { return obj(d::g_sys3d); }                 // SysEn
void* log_target() { return obj(d::g_err); }            // error log
const void* identity() { return obj(d::g_identity_matrix); }        // identity D3DMATRIX

constexpr std::uint32_t kWorld = 0x100;  // D3DTS_WORLD

void set_world(const void* m) { com_call(device(), 0x94, kWorld, m); }  // SetTransform

// The 5 leading dwords of an item, copied to the frame ([ebp-0x50] current, [ebp-0x1c] previous).
struct Key {
    std::int32_t key;       // +0x00
    std::uint32_t vtype;    // +0x04 (low byte = vertex type)
    std::uint32_t states;   // +0x08
    std::int32_t material;  // +0x0c
    std::int32_t prim;      // +0x10
};
static_assert(sizeof(Key) == 0x14);

// Bone matrices for the vertex-type-7 path: 73 vec4 (0x490 bytes) as in the original frame.
struct alignas(4) ConstBuf {
    std::uint8_t bytes[0x490];
};
static_assert(sizeof(ConstBuf) == 0x490);

JOAN_COUNTER(c_reorder_draw, "Reorder::draw");

struct H {
    void* self() { return this; }

    // Ends the owner effects of key `id` (0x005c6250 on each active one).
    void end_owners(std::int32_t id) {
        void* const k = joan::orig::arg::as<void*>(joan::orig::as_dynarray(&jo::self<jo::Reorder>(this)->sectors)->get(joan::orig::arg::i32(id)));  // 0x00417200
        for (std::int32_t j = 0; j < jo::as_light_set(k)->light_count; ++j) {
            void* const o = joan::orig::arg::as<void*>(joan::orig::as_dynarray(self())->get(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_light_set(k)->light_ids[j]))));  // 0x00417200
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_light(static_cast<std::uint8_t*>(o) + 8)->active()) != 0)  // 0x005c6070
                joan::orig::as_light(static_cast<std::uint8_t*>(o) + 8)->disable();                    // 0x005c6250
        }
    }

    // Vertex type 7: upload `count` bone matrices of the 0xa0-byte bone records at `bones` (+0x58).
    static void upload_bones(ConstBuf& buf, const jo::Bone* bones, std::int32_t count) {
        joan::orig::vector(joan::orig::arg::mptr(static_cast<void*>(buf.bytes)), 0x10, 0x49, joan::orig::arg::mptr(reinterpret_cast<void*>(fn::code_value(fn::identity_5bc990))));  // 0x00449af0 (ctor 0x005bc990)
        for (std::int32_t first = 0; first < count; first += 0x18) {
            std::int32_t n = 0;
            std::int32_t idx = first;
            std::uint8_t* dst = buf.bytes;
            for (; n < 0x18 && idx < count; ++idx, ++n, dst += 0x30)
                import_call<void*>(d::iat_v3d_87_matrix_transpose_2, static_cast<void*>(dst), reinterpret_cast<const std::uint8_t*>(bones) + idx * static_cast<std::int32_t>(sizeof(jo::Bone)) + offsetof(jo::Bone, matrix_));  // matrix_transpose (x86: bones + idx * 0xa0 + 0x58)
            com_call(device(), 0x13c, first * 3 + 0x11, static_cast<const void*>(buf.bytes), n * 3);  // SetVertexShaderConstant
        }
    }

    void draw(void* scene, void* ctx, std::int32_t pass) {
        c_reorder_draw.hit();
        std::int32_t first = 1;  // [ebp-0x08]
        Key prev{};              // [ebp-0x1c]
        Key cur{};               // [ebp-0x50]
        std::int32_t lo = 0;     // [ebp-0x30]
        std::int32_t hi = 0;     // [ebp-0x34]
        void* const keys = &jo::self<jo::Reorder>(this)->sectors;
        void* const materials = &jo::self<jo::Reorder>(this)->materials;

        for (std::int32_t i = 0; i < jo::self<jo::Reorder>(this)->group_count; ++i) jo::self<jo::Reorder>(this)->groups[i].cursor = 0;
        com_call(device(), 0x94, kWorld, identity());  // SetTransform(WORLD, identity)

        for (;;) {
            std::int32_t minkey = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(joan::orig::as_dynarray(keys)->size()) + 1u);  // 0x00417ad0
            {
                std::uint8_t* p = jo::view<std::uint8_t*>(jo::self<jo::Reorder>(this)->groups);
                for (std::int32_t i = 0; i < jo::self<jo::Reorder>(this)->group_count; ++i, p += static_cast<std::int32_t>(sizeof(jo::ReorderGroup))) {
                    if (jo::as_reorder_group(p)->cursor < jo::as_reorder_group(p)->count) {
                        const std::int32_t k = jo::as_reorder_group(p)->batches[jo::as_reorder_group(p)->cursor].key;
                        if (k < minkey) {
                            minkey = k;
                            lo = i;
                        }
                        if (k == minkey) hi = i;
                    }
                }
            }
            if (minkey > joan::orig::as_dynarray(keys)->size()) break;  // 0x00417ad0

            std::int32_t i = lo;
            std::uint8_t* p = jo::view<std::uint8_t*>(jo::self<jo::Reorder>(this)->groups) + i * static_cast<std::int32_t>(sizeof(jo::ReorderGroup));
            for (; i <= hi; ++i, p += static_cast<std::int32_t>(sizeof(jo::ReorderGroup))) {
                std::uint8_t* item = jo::view<std::uint8_t*>(jo::as_reorder_group(p)->batches) + jo::as_reorder_group(p)->cursor * static_cast<std::int32_t>(sizeof(jo::ReorderBatch));  // [ebp-0x3c]
                for (; jo::as_reorder_group(p)->cursor < jo::as_reorder_group(p)->count; ++jo::as_reorder_group(p)->cursor, item += static_cast<std::int32_t>(sizeof(jo::ReorderBatch))) {
                    if (jo::as_reorder_batch(item)->key > minkey) break;
                    if (jo::as_reorder_batch(item)->frame != global<std::uint32_t>(d::g_frame_a500ae)) continue;
                    std::memcpy(&cur, item, sizeof cur);
                    {
                        void* const m = joan::orig::arg::as<void*>(joan::orig::as_dynarray(materials)->get(joan::orig::arg::i32(cur.material)));  // 0x00417200
                        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sys_en(sysen())->pass_filter(joan::orig::arg::u32(jo::as_material(static_cast<std::uint8_t*>(m) + 8)->pass_mask), joan::orig::arg::u32(pass))) == 0)  // 0x00600b30
                            continue;
                    }

                    // key: owner effects and lighting
                    if (first != 0 || cur.key != prev.key) {
                        if (first == 0) end_owners(prev.key);
                        void* const k = joan::orig::arg::as<void*>(joan::orig::as_dynarray(keys)->get(joan::orig::arg::i32(cur.key)));  // 0x00417200
                        for (std::int32_t j = 0; j < jo::as_light_set(k)->light_count; ++j) {
                            void* const o = joan::orig::arg::as<void*>(joan::orig::as_dynarray(self())->get(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_light_set(k)->light_ids[j]))));  // 0x00417200
                            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_light(static_cast<std::uint8_t*>(o) + 8)->active()) != 0)  // 0x005c6070
                                joan::orig::as_light(static_cast<std::uint8_t*>(o) + 8)->enable();                    // 0x005c6110
                        }
                        const std::uint8_t light = jo::as_light_set(k)->lit;
                        if (light != joan::orig::arg::as<std::uint8_t>(joan::orig::as_scene(scene)->lit())) {  // 0x005e8fc0
                            if (jo::as_light_set(k)->lit != 0)
                                joan::orig::as_scene(scene)->lights_on();  // 0x005eec30
                            else
                                joan::orig::as_scene(scene)->lights_off();  // 0x005eebc0
                            joan::orig::as_scene(scene)->set_lit(joan::orig::arg::u32(jo::as_light_set(k)->lit));  // 0x005e8fe0
                        }
                    }
                    // render-state flags
                    if (first != 0 || cur.states != prev.states) {
                        const std::uint32_t camera = joan::orig::arg::as<std::uint32_t>(joan::orig::as_camera(ctx)->flags());  // 0x00616fe0
                        joan::orig::as_sys_en(sysen())->set_flags(joan::orig::arg::u32(cur.states | camera));                    // 0x00602080
                    }
                    // material
                    if (first != 0 || cur.material != prev.material) {
                        void* const m = joan::orig::arg::as<void*>(joan::orig::as_dynarray(materials)->get(joan::orig::arg::i32(cur.material)));  // 0x00417200
                        std::uint8_t* const mat = static_cast<std::uint8_t*>(m) + 8;          // [ebp-0x64]
                        std::uint8_t d3dmat[0x44];                                            // [ebp-0xa8]
                        joan::orig::as_material(mat)->to_d3d(joan::orig::arg::mptr(static_cast<void*>(d3dmat)));         // 0x005d56f0
                        com_call(device(), 0xa8, static_cast<const void*>(d3dmat));           // SetMaterial
                        for (std::int32_t s = 0; s < 4; ++s) {
                            void* const t = jo::as_material(mat)->textures[s];
                            if (t == nullptr)
                                com_call(device(), 0xf4, s, static_cast<void*>(nullptr));  // SetTexture
                            else
                                com_call(device(), 0xf4, s, jo::view<void*>(jo::as_texture(t)->d3d_base));  // SetTexture
                        }
                        joan::orig::as_sys_en(sysen())->set_blend(joan::orig::arg::u32(jo::as_material(mat)->blend));  // 0x005f8250 blend
                        joan::orig::as_sys_en(sysen())->set_texture_mode(joan::orig::arg::u32(jo::as_material(mat)->texture_mode));  // 0x005fa250 texture mode
                        joan::orig::as_sys_en(sysen())->set_filter(joan::orig::arg::u32(jo::as_material(mat)->filter));  // 0x005fec10 filter
                        joan::orig::as_sys_en(sysen())->set_raster(joan::orig::arg::u32(jo::as_material(mat)->raster));  // 0x006026a0 raster
                        joan::orig::as_sys_en(sysen())->apply(0u);                             // 0x006014d0
                        (void)joan::orig::as_vs_cache(global<void*>(d::g_vs_cache))->field20();  // 0x0047b5e0: this + 0x20
                        jo::view<std::uint32_t>(jo::as_vs_cache(global<void*>(d::g_vs_cache))->slider_factors[11]) = 0x3f800000u;  // field20() + 0x5c, 1.0f
                        if ((joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(sysen())->flags()) & 0xe) != 0)  // 0x006020f0
                            joan::orig::as_camera(ctx)->apply_fog(joan::orig::arg::u32(joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(sysen())->overlay_stage())));  // 0x00616b80(0x00601470)
                        else
                            joan::orig::as_fog(jo::as_camera(ctx)->fog)->off();  // 0x005c09a0
                    }

                    const std::int8_t vtype = static_cast<std::int8_t>(cur.vtype & 0xff);
                    const std::uint32_t stride = jo::as_vertex_format(joan::orig::arg::as<void*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(vtype)))))->stride;  // 0x00600200
                    std::int32_t states_done = 0;  // [ebp-0xb8]

                    // vertex lists
                    for (std::uint8_t* n = jo::view<std::uint8_t*>(jo::as_reorder_batch(item)->spans); n != nullptr; n = jo::view<std::uint8_t*>(jo::as_span_node(n)->next)) {
                        if (jo::as_span_node(n)->count <= 0) continue;
                        if (vtype == 7) {
                            ConstBuf buf;  // [ebp-0x54c]
                            upload_bones(buf, jo::as_span_node(n)->bone, jo::view<std::int32_t>(jo::as_span_node(n)->u32_18));
                        } else if (jo::view<void*>(jo::as_span_node(n)->bone) != nullptr) {
                            set_world(joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_span_node(n)->bone))->matrix()));  // 0x0060ef80
                        } else {
                            set_world(identity());
                        }
                        if (jo::view<void*>(jo::as_span_node(n)->vertex_buffer) != nullptr) {
                            const std::int32_t hr = com_call(device(), 0x14c, 0u, jo::view<void*>(jo::as_span_node(n)->vertex_buffer), stride);  // SetStreamSource
                            if (hr != 0)
                                joan::orig::error::fatal(joan::orig::arg::mptr(log_target()), str(d::s_Cannot_set_vertex_buffer_s), joan::orig::arg::as<const char*>(joan::orig::as_sys_en(sysen())->error_string(joan::orig::arg::i32(hr))));  // 0x00586cb0 (0x006002b0)
                            joan::orig::as_sys_en(sysen())->set_vertex_type(static_cast<char>(joan::orig::arg::i32(vtype)));  // 0x006024e0
                        } else {
                            joan::orig::as_sys_en(sysen())->fill_vertices(static_cast<char>(joan::orig::arg::i32(vtype)), joan::orig::arg::mptr(static_cast<std::uint8_t*>(jo::as_span_node(n)->vertices) + jo::view<std::uint32_t>(jo::as_span_node(n)->first) * stride), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_span_node(n)->count)));  // 0x00600490
                        }
                        if (states_done == 0) {
                            joan::orig::as_sys_en(sysen())->apply(0u);  // 0x006014d0
                            states_done = 1;
                        }
                        if (jo::view<void*>(jo::as_span_node(n)->vertex_buffer) != nullptr) {
                            const std::uint32_t prim = joan::orig::arg::as<std::uint32_t>(joan::orig::primitive_type(joan::orig::arg::i32(cur.prim)));  // 0x00600b60
                            com_call(device(), 0x118, prim, jo::view<std::uint32_t>(jo::as_span_node(n)->first), jo::as_span_node(n)->u32_10);  // DrawPrimitive
                            joan::orig::as_sys_en(sysen())->count(0u, joan::orig::arg::u32(jo::as_span_node(n)->u32_10));  // 0x005d2b40
                        } else {
                            joan::orig::as_sys_en(sysen())->draw(joan::orig::arg::i32(cur.prim), 0u, joan::orig::arg::u32(jo::as_span_node(n)->u32_10));  // 0x00600a00
                        }
                    }

                    // indexed lists
                    for (std::uint8_t* n = jo::view<std::uint8_t*>(jo::as_reorder_batch(item)->strips); n != nullptr; n = jo::view<std::uint8_t*>(jo::as_strip_node(n)->next)) {
                        if (jo::as_strip_node(n)->index_count <= 0) continue;
                        if (vtype == 7) {
                            ConstBuf buf;  // [ebp-0x9fc]
                            upload_bones(buf, jo::as_strip_node(n)->bones, jo::view<std::int32_t>(jo::as_strip_node(n)->bone_count));
                        } else if (jo::view<void*>(jo::as_strip_node(n)->bones) != nullptr) {
                            set_world(joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_strip_node(n)->bones))->matrix()));  // 0x0060ef80
                        } else {
                            set_world(identity());
                        }
                        const bool buffers = jo::view<void*>(jo::as_strip_node(n)->vertex_buffer) != nullptr && jo::view<void*>(jo::as_strip_node(n)->index_buffer) != nullptr;
                        if (buffers) {
                            const std::int32_t hr = com_call(device(), 0x14c, 0u, jo::view<void*>(jo::as_strip_node(n)->vertex_buffer), stride);  // SetStreamSource
                            if (hr != 0)
                                joan::orig::error::fatal(joan::orig::arg::mptr(log_target()), str(d::s_Cannot_set_vertex_buffer_s), joan::orig::arg::as<const char*>(joan::orig::as_sys_en(sysen())->error_string(joan::orig::arg::i32(hr))));  // 0x00586cb0 (0x006002b0)
                            com_call(device(), 0x154, jo::view<void*>(jo::as_strip_node(n)->index_buffer), 0u);  // SetIndices (result unused)
                            if (hr != 0)
                                joan::orig::error::fatal(joan::orig::arg::mptr(log_target()), str(d::s_Cannot_set_index_buffer_s), joan::orig::arg::as<const char*>(joan::orig::as_sys_en(sysen())->error_string(joan::orig::arg::i32(hr))));  // 0x00586cb0 (0x006002b0)
                            joan::orig::as_sys_en(sysen())->set_vertex_type(static_cast<char>(joan::orig::arg::i32(vtype)));  // 0x006024e0
                        } else {
                            joan::orig::as_sys_en(sysen())->fill_vertices(static_cast<char>(joan::orig::arg::i32(vtype)), joan::orig::arg::mptr(static_cast<std::uint8_t*>(jo::as_strip_node(n)->vertices) + jo::as_strip_node(n)->min_index * stride), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_strip_node(n)->vertex_range)));  // 0x00600490
                            joan::orig::as_sys_en(sysen())->fill_indices(joan::orig::arg::mptr(static_cast<std::uint8_t*>(jo::as_strip_node(n)->indices) + jo::as_strip_node(n)->start_index * 2), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_strip_node(n)->index_count)), joan::orig::arg::u32(0u - jo::as_strip_node(n)->min_index));  // 0x00600510
                        }
                        if (states_done == 0) {
                            joan::orig::as_sys_en(sysen())->apply(0u);  // 0x006014d0
                            states_done = 1;
                        }
                        if (jo::view<void*>(jo::as_strip_node(n)->vertex_buffer) != nullptr && jo::view<void*>(jo::as_strip_node(n)->index_buffer) != nullptr) {
                            const std::uint32_t prim = joan::orig::arg::as<std::uint32_t>(joan::orig::primitive_type(joan::orig::arg::i32(cur.prim)));  // 0x00600b60
                            com_call(device(), 0x11c, prim, jo::as_strip_node(n)->min_index, jo::view<std::uint32_t>(jo::as_strip_node(n)->vertex_range),
                                     jo::as_strip_node(n)->start_index, jo::as_strip_node(n)->primitive_count);  // DrawIndexedPrimitive
                            joan::orig::as_sys_en(sysen())->count(0u, joan::orig::arg::u32(jo::as_strip_node(n)->primitive_count));  // 0x005d2b40
                        } else {
                            joan::orig::as_sys_en(sysen())->draw_indexed(joan::orig::arg::i32(cur.prim), 0u, joan::orig::arg::u32(jo::as_strip_node(n)->primitive_count));  // 0x00600a90
                        }
                    }
                    prev = cur;
                    first = 0;
                }
            }
        }
        if (first == 0) end_owners(prev.key);
    }
};

JOAN_REPLACE("en3d/b117_03", "RenderOrder::render", 0x005e67b0, &H::draw);

}  // namespace en3d_b117_03
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16_2 {

using joan::call::thiscall;

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // 0x005cbaf0 Plane::Plane(type, flags).
    H* plane_ctor(std::uint32_t type, std::uint32_t flags) {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::Plane));  // memset 0x11c (x86)
        jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format) = static_cast<std::uint8_t>(type);
        jo::self<jo::Plane>(this)->buffer_mode = static_cast<std::uint8_t>(flags);
        jo::self<jo::Plane>(this)->skip_draw = 0xff;
        joan::orig::as_device_resource(&jo::self<jo::Plane>(this)->resource)->set_callback(0u, 0u, 0u);  // 0x005e9010
        return this;
    }
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
HELPER16("Plane::Plane", 0x005cbaf0, &H::plane_ctor);
#undef HELPER16

}  // namespace en3d_helpers16_2
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
//
//      0x005c0ef0 Model::load(file, anim, flag) (ret 0xc) - waits for the loader, +2 = (short)anim,
//                 _splitpath name into +4 (strncpy 0x20); anim == 0: static mesh (0x005cbaf0(1, 0),
//                 0x005ddd70), LODs <base>#<i+1><ext> i = 1..4 (0x005d4470 / 0x005d4a00), bounding
//                 sphere 0x005c51b0, +0x30 = 1, 0x005d4720(count, max(+0x40, 1000)), "#S" model
//                 (0x005d7310), "#V.SMO" shadow volume (materials 0xd / 1, 0x005d5650 x4,
//                 0x005cf920(1) / (-1), 0x005d73b0); anim != 0: the same with 0x005da280 and a
//                 shadow list (+0x2c, 0x005f45f0 / 0x005f4dc0 / 0x005f4fe0 / 0x005f5090),
//                 0x005c53b0, +0x30 = 0; byte +0 = 1
//
//      0x005da280 Mesh::load_any(reader, shadow, byte flag, byte flag2) (ret 0x10) - kind / format
//                 header, 0x005cc7a0, 0x005cdc70(7 or 1), dispatch 0 / 50 / 100 / 500
namespace {
namespace en3d_b84_01_2 {

using joan::hook::global;

using joan::call::thiscall;

JOAN_COUNTER(c_load_any, "Mesh::load_any");

struct H {
    // 0x005da280
    std::int32_t load_any(void* reader, void* shadow, std::uint32_t flag, std::uint32_t flag2) {
        c_load_any.hit();
        const std::int32_t kind = joan::orig::as_file_mem(reader)->file_get_long();    // 0x005893c0
        const std::int32_t format = joan::orig::as_file_mem(reader)->file_get_long();  // 0x005893c0
        joan::orig::as_plane(this)->init(0, 0, 0, joan::orig::arg::u32(format));                             // 0x005cc7a0
        if ((flag & 0xffu) != 0 && global<std::uint32_t>(d::g_a5262b) != 0 && format <= 0x18 && format >= 5)
            joan::orig::as_plane(this)->set_format(7);  // 0x005cdc70
        else
            joan::orig::as_plane(this)->set_format(1);  // 0x005cdc70
        joan::orig::as_skin(shadow)->destruct();  // 0x005f4670
        jo::view<std::uint32_t>(jo::self<jo::Mesh>(this)->list_kind) = 4;
        const std::uint8_t b = static_cast<std::uint8_t>(flag2);
        switch (kind) {
            case 0: return joan::orig::arg::as<std::int32_t>(joan::orig::as_mesh(this)->load_v0(joan::orig::arg::mptr(reader), joan::orig::arg::mptr(shadow), joan::orig::arg::u32(b)));    // 0x005da3d0
            case 0x32: return joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->load_skin(joan::orig::arg::mptr(reader), joan::orig::arg::mptr(shadow), joan::orig::arg::u32(b)));  // 0x005db710
            case 0x64: return joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->load_v100_5dc400(reader, shadow, joan::orig::arg::u32(b)));  // 0x005dc400
            case 0x1f4: return joan::orig::arg::as<std::int32_t>(joan::orig::as_mesh(this)->load_v500(joan::orig::arg::mptr(reader), joan::orig::arg::mptr(shadow), joan::orig::arg::u32(b))); // 0x005dc420
            default: return 0;
        }
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("Mesh::load_any", 0x005da280, &H::load_any);
#undef E3D

}  // namespace en3d_b84_01_2
}  // namespace

// ---------------------------------------- from modules/en3d_b85_02.cpp (en3d/b85_02)
//
//      0x005da3d0 Mesh::load_v0(file, skin, byte flag) (ret 0xc) - kind 0: material / texture /
//                 vertex / uv counts and an unused fifth int; per material three 0..1 colours, a
//                 texture index, its triangle count and 12-byte records (vertex, uv) x 3; the
//                 textures (20-byte name + embedded image, D3DXCreateTextureFromFileInMemory or by
//                 name); the uv table; per vertex the bone influences (0x005f4970 add_bone /
//
//      0x005dc420 Mesh::load_v500(file, skin, byte flag) (ret 0xc) - kind 500: header 16 bytes to
//                 skin +0x10 (+0x1c /= 2.0f, +0xc = 1); per material strip / vertex counts, four
//                 A R G B colours, 16 unused bytes, texture index, the triangle strip (as
//                 Plane::load_v500) and per vertex its uv (read straight into the vertex) and bone
//                 influences; kind 7: skinned vertices, else plain; then textures and the tagged
//                 sections 1 (env / lightmap stages) and 2 (per-material flags) until another tag.
namespace {
namespace en3d_b85_02 {

// File records of the mesh loaders: a triangle corner (vertex, uv index) and a uv pair (raw dwords).
struct FileCorner {
    std::uint16_t vertex, uv;
};
struct FileUv {
    std::uint32_t u, v;
};

using joan::hook::global;

struct Obj {};
using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::stdcall_call;

// Wrapping 32-bit integer arithmetic (imul / add).
using joan::orig::helpers::mul;
std::int32_t add(std::int32_t a, std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}
// Element `index` of `size` bytes from base (32-bit address arithmetic).
std::uint8_t* elem(const void* base, std::int32_t index, std::int32_t size) {
    return reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(base) +
                                           static_cast<std::uint32_t>(mul(index, size)));
}

constexpr img::Addr kTextures(d::g_texture_array);  // TextureManager
constexpr img::Addr kWhite = d::g_init_white;                  // default colour dword

// ilib File readers.
std::int32_t read_int(void* file) { return joan::orig::as_file_mem(file)->file_get_long(); }     // 0x005893c0
std::uint16_t read_u16(void* file) { return joan::orig::arg::as<std::uint16_t>(joan::orig::as_file_mem(file)->file_get_unsigned_short()); }  // 0x005892f0
std::int32_t read(void* file, void* buffer, std::int32_t size) {
    return joan::orig::as_file_mem(file)->file_read(joan::orig::arg::mptr(buffer), joan::orig::arg::u32(size));  // 0x00588a80
}
void seek(void* file, std::int32_t offset, std::int32_t mode) { joan::orig::as_file_mem(file)->file_seek(joan::orig::arg::i32(offset), joan::orig::arg::i32(mode)); }  // 0x005888c0
// Message pump / progress, called in pairs while loading.
void pump() {
    joan::orig::as_sound_stream(img::obj(d::g_snd))->update();  // 0x0059b1c0
    joan::orig::as_ambient_streams(img::obj(d::g_mus))->update();  // 0x005866d0
}

// ilib Blob: the constructor 0x0057f770 writes +0 data and +4 size.
struct Blob {
    void* data;
    std::uint32_t size;
};
// ilib VQueue: the constructor 0x005a8640 zeroes +0 data, +4, +8 used, +0xc.
struct VQueue {
    std::uint8_t* data;
    std::uint32_t capacity;
    std::int32_t length;
    std::uint8_t* cursor;
};
// x86: 8 and 16 bytes; x64: room for the (larger) ilib objects.
static_assert(offsetof(Blob, size) == sizeof(void*) && sizeof(Blob) >= sizeof(joan::orig::Blob), "Blob");
static_assert(offsetof(VQueue, cursor) == offsetof(joan::orig::VQueue, cursor) && sizeof(VQueue) >= sizeof(joan::orig::VQueue), "VQueue");
void blob_init(Blob* b, std::int32_t n) { joan::orig::as_blob(b)->construct(joan::orig::arg::i32(n)); }  // 0x0057f770
std::uint8_t* blob_data(Blob* b) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_blob(b)->data_56de20()); }  // 0x0056de20
void blob_free(Blob* b) { joan::orig::as_blob(b)->destruct(); }                         // 0x0057f850

struct Vec3 {  // D3DVECTOR as raw dwords
    std::uint32_t v[3];
};
static_assert(sizeof(Vec3) == 12, "Vec3");

// float colour component -> byte (fld; fmul 255.0f; _ftol).
std::uint8_t channel(float v) {
    return static_cast<std::uint8_t>(joan::hook::x87::ftol(static_cast<double>(v * 255.0f)));
}

// Skin object helpers.
std::int32_t skin_add(void* skin, std::int32_t n) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_skin(skin)->add(joan::orig::arg::i32(n))); }  // 0x005f47c0
void add_bone(void* skin, std::int32_t sv, std::int32_t bone, Vec3* a, Vec3* b, std::uint32_t weight) {
    joan::orig::as_skin(skin)->add_bone(joan::orig::arg::i32(sv), joan::orig::arg::i32(bone), joan::orig::arg::mptr(a), joan::orig::arg::mptr(b), joan::orig::arg::u32(weight));  // 0x005f4970 (SkinVertex::add_bone)
}
void skin_normalize(void* skin, std::int32_t sv) { joan::orig::as_skin(skin)->normalize(joan::orig::arg::i32(sv)); }  // 0x005f49c0

// Influence k of skin vertex sv into the 0x48-byte mesh vertex mv; for k >= limit the bone /
// weight are zeroed and, when `zero` is given, the two vectors set to it (load_v500; load_v0
// leaves them). limit: load_v0 passes nullptr and compares with the skin vertex's own count
// (+0 of the 0x84-byte entry, 0x005dac1c); load_v500 compares with the influence count read
// from the file (ebp-0xd0, 0x005dcd78).
void copy_influence(std::uint8_t* mv, void* skin, std::int32_t sv, std::int32_t k, const Vec3* zero,
                    const std::int32_t* limit) {
    std::uint8_t* const s = elem(jo::view<std::uint8_t*>(jo::as_skin(skin)->vertices), sv, 0x84);
    if (k < (limit != nullptr ? *limit : jo::as_skin_vertex(s)->count)) {
        const jo::SkinInfluence& inf = (&jo::as_skin_vertex(s)->bone0)[k];
        jo::VertexF7* const d = jo::as_vertex_f7(mv);
        jo::view<std::uint32_t>(d->influence_pos[k * 3 + 0]) = jo::view<std::uint32_t>(inf.pos[0]);
        jo::view<std::uint32_t>(d->influence_pos[k * 3 + 1]) = jo::view<std::uint32_t>(inf.pos[1]);
        jo::view<std::uint32_t>(d->influence_pos[k * 3 + 2]) = jo::view<std::uint32_t>(inf.pos[2]);
        jo::view<std::uint32_t>(d->influence_normal[k * 3 + 0]) = jo::view<std::uint32_t>(inf.normal[0]);
        jo::view<std::uint32_t>(d->influence_normal[k * 3 + 1]) = jo::view<std::uint32_t>(inf.normal[1]);
        jo::view<std::uint32_t>(d->influence_normal[k * 3 + 2]) = jo::view<std::uint32_t>(inf.normal[2]);
        d->bone[k] = static_cast<std::uint16_t>(mul(*static_cast<const std::int32_t*>(static_cast<const void*>(&inf.bone)), 3));  // the bone as an aligned dword read (as the original)
        d->weight[k] = jo::view<std::uint32_t>(inf.weight);
    } else {
        jo::VertexF7* const d = jo::as_vertex_f7(mv);
        if (zero != nullptr) {
            jo::view<std::uint32_t>(d->influence_pos[k * 3 + 0]) = zero->v[0];
            jo::view<std::uint32_t>(d->influence_pos[k * 3 + 1]) = zero->v[1];
            jo::view<std::uint32_t>(d->influence_pos[k * 3 + 2]) = zero->v[2];
            jo::view<std::uint32_t>(d->influence_normal[k * 3 + 0]) = zero->v[0];
            jo::view<std::uint32_t>(d->influence_normal[k * 3 + 1]) = zero->v[1];
            jo::view<std::uint32_t>(d->influence_normal[k * 3 + 2]) = zero->v[2];
        }
        d->bone[k] = 0;
        d->weight[k] = 0;
    }
}

// The triangle strip of material `mat` (n indices after the first two), vertex base vbase
// (identical to the strip code of Plane::load_v500).
void read_strip(void* mesh, void* file, std::int32_t mat, std::int32_t n, std::int32_t vbase) {
    const std::int32_t start = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->reserve_strip(joan::orig::arg::i32(mat), joan::orig::arg::u32(n)));  // 0x005d6ee0
    auto index = [&](std::int32_t k) -> std::uint16_t& { return jo::view<std::uint16_t*>(jo::as_plane(mesh)->strip_indices)[k]; };
    std::int32_t count = 0;
    std::int32_t pos = start;
    std::int32_t odd = 0;
    std::uint16_t a = read_u16(file);
    std::uint16_t b = read_u16(file);
    index(pos) = static_cast<std::uint16_t>(add(vbase, a));
    ++pos;
    index(pos) = static_cast<std::uint16_t>(add(vbase, b));
    ++pos;
    for (std::int32_t k = 0; k < n; ++k) {
        const std::uint16_t c = read_u16(file);
        index(pos) = static_cast<std::uint16_t>(add(vbase, c));
        ++pos;
        if (c != a && c != b && a != b) ++count;
        a = b;
        b = c;
        odd ^= 1;
    }
    const std::int32_t face = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->add_triangles(joan::orig::arg::i32(count)));  // 0x005ce6f0 add triangles
    count = 0;
    pos = start;
    odd = 0;
    a = index(pos);
    ++pos;
    b = index(pos);
    ++pos;
    for (std::int32_t k = 0; k < n; ++k) {
        const std::uint16_t c = index(pos);
        ++pos;
        if (c != a && c != b && a != b) {
            const std::int32_t ia = a, ib = b, ic = c;
            if (odd == 0)
                joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(add(face, count)), joan::orig::arg::u32(mat), joan::orig::arg::u32(ia), joan::orig::arg::u32(ib), joan::orig::arg::u32(ic));  // 0x005ce800 set_triangle
            else
                joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(add(face, count)), joan::orig::arg::u32(mat), joan::orig::arg::u32(ib), joan::orig::arg::u32(ia), joan::orig::arg::u32(ic));
            ++count;
        }
        a = b;
        b = c;
        odd ^= 1;
    }
}

// Texture name (20 bytes) followed by the loop counter and (v500) the texture table, as in the
// original frames: a name without a NUL in its 20 bytes runs into the counter there too.
struct NameSlot {
    char name[0x14];
    std::int32_t t;
    void** table;
};
static_assert(offsetof(NameSlot, t) == 0x14 && offsetof(NameSlot, table) == 0x18, "NameSlot");  // x86: 0x1c bytes

std::int32_t texture_kind(void* texture) {  // vtable slot 2 (thiscall)
    using Virtual = std::int32_t (Obj::*)();
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(texture)[2], sizeof(void*));
    return (static_cast<Obj*>(texture)->*method)();
}

// Creates a texture from `size` bytes of embedded image already in `data`; returns the new
// entry, or `fallback` when D3DX fails.
void* texture_from_memory(void* data, std::int32_t size, const char* name, void* fallback) {
    void* d3d_texture = nullptr;  // uninitialised in the original; only used when hr == 0
    // D3DXCreateTextureFromFileInMemory(device, data, size, &texture)
    if (joan::orig::arg::as<std::int32_t>(joan::orig::d3dx::create_texture_from_file_in_memory(joan::orig::arg::mptr(global<void*>(d::g_game_device)), joan::orig::arg::mptr(data), joan::orig::arg::u32(size), &d3d_texture)) != 0) return fallback;
    void* const entry = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTextures))->create_2d());  // 0x006094c0 new 2D texture
    joan::orig::as_texture2d(entry)->attach(joan::orig::arg::mptr(d3d_texture), 1u);                // 0x00605f00 set texture
    joan::orig::as_texture(entry)->set_name(joan::orig::arg::str(name));                          // 0x00605ad0 set name
    reinterpret_cast<std::uint32_t(JOAN_STDCALL*)(void*)>(joan::call::vtable_of(d3d_texture)[2])(d3d_texture);  // Release
    return entry;
}

JOAN_COUNTER(c_v0, "Mesh::load_v0");
JOAN_COUNTER(c_v500, "Mesh::load_v500");

struct H {
    std::uint8_t* material(std::int32_t index) { return elem(jo::view<std::uint8_t*>(jo::self<jo::Mesh>(this)->materials), index, static_cast<std::int32_t>(sizeof(jo::Material))); }
    std::uint8_t* vertex(std::int32_t index) { return elem(jo::view<std::uint8_t*>(jo::self<jo::Mesh>(this)->vertices_f1), index, 0x24); }
    std::uint8_t* skinned(std::int32_t index) { return elem(jo::view<std::uint8_t*>(jo::self<jo::Mesh>(this)->vertices_f7), index, 0x48); }

    // 0x005da3d0 Mesh::load_v0(file, skin, flag).
    std::uint32_t load_v0(void* file, void* skin, std::uint32_t flag) {
        c_v0.hit();
        const std::int32_t n_mat = read_int(file);
        const std::int32_t n_tex = read_int(file);
        const std::int32_t n_vert = read_int(file);
        const std::int32_t n_uv = read_int(file);
        read_int(file);  // unused
        Blob tex_blob;
        blob_init(&tex_blob, n_mat << 2);
        std::int32_t* const tex_of = reinterpret_cast<std::int32_t*>(blob_data(&tex_blob));
        Blob count_blob;
        blob_init(&count_blob, n_mat << 2);
        std::int32_t* const tri_count = reinterpret_cast<std::int32_t*>(blob_data(&count_blob));
        VQueue records;
        joan::orig::as_vqueue(&records)->construct();  // 0x005a8640
        auto fail = [&]() -> std::uint32_t {
            joan::orig::as_vqueue(&records)->free();  // 0x005a87b0
            blob_free(&count_blob);
            blob_free(&tex_blob);
            return 0;
        };
        const std::int32_t base = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_materials(joan::orig::arg::i32(n_mat)));  // 0x005cd470 add_materials
        std::int32_t i;
        for (i = 0; i < n_mat; ++i) {
            std::uint8_t* const m = material(add(base, i));
            joan::orig::as_material(m)->construct();  // 0x005d52a0 Material::init
            const std::int32_t n = read_int(file);
            float rgb[3];
            std::uint8_t col[4];
            auto colour = [&]() {
                col[2] = channel(rgb[0]);
                col[1] = channel(rgb[1]);
                col[0] = channel(rgb[2]);
                col[3] = 0xff;
            };
            if (read(file, rgb, 0xc) == 0) return fail();
            colour();
            joan::orig::as_material(m)->set_colour4(joan::orig::arg::mptr(static_cast<void*>(col)));  // 0x005d5570
            if (read(file, rgb, 0xc) == 0) return fail();
            colour();
            joan::orig::as_material(m)->set_colour0(joan::orig::arg::mptr(static_cast<void*>(col)));  // 0x005d5540
            if (read(file, rgb, 0xc) == 0) return fail();
            colour();
            joan::orig::as_material(m)->set_colour8(joan::orig::arg::mptr(static_cast<void*>(col)), 0u);  // 0x005d55a0
            tex_of[i] = read_int(file);
            tri_count[i] = n;
            const std::int32_t bytes = mul(n, 0xc);
            void* const buffer = joan::orig::arg::as<void*>(joan::orig::as_vqueue(&records)->push(joan::orig::arg::u32(mul(n, 0xc))));  // 0x005a88c0 VQueue::push
            if (read(file, buffer, bytes) == 0) return fail();
        }

        // Textures.
        NameSlot slot;
        for (slot.t = 0; slot.t < n_tex; ++slot.t) {
            if (read(file, slot.name, 0x14) == 0) return fail();
            const std::int32_t size = read_int(file);
            char* const dot = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(static_cast<char*>(slot.name)), 0x2e));  // strrchr
            if (dot != nullptr) *dot = 0;
            void* tex = joan::orig::arg::as<void*>(joan::orig::as_textures(img::obj(kTextures))->find(joan::orig::arg::str(static_cast<char*>(slot.name))));  // 0x0060a4b0 find
            if (tex != nullptr) {
                joan::orig::as_texture_manager(img::obj(kTextures))->use(joan::orig::arg::mptr(tex));  // 0x0060a500 add reference
                if (size > 0) seek(file, size, 1);
            } else if (size > 0) {
                Blob image;
                blob_init(&image, size);
                if (read(file, blob_data(&image), size) != 0)
                    tex = texture_from_memory(blob_data(&image), size, slot.name, tex);
                blob_free(&image);
            } else {
                tex = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(static_cast<char*>(slot.name)), 1, joan::orig::arg::u32(static_cast<std::uint32_t>(static_cast<std::uint8_t>(flag)))));  // 0x006099c0 load
            }
            for (i = 0; i < n_mat; ++i) {
                if (tex_of[i] != slot.t) continue;
                joan::orig::as_material(material(add(base, i)))->set_texture(0, joan::orig::arg::mptr(tex));  // 0x005d5650 set_texture
                if (tex != nullptr && texture_kind(tex) == 1) {
                    (jo::as_material(material(add(base, i))))->pass_mask = 2;
                    (jo::as_material(material(add(base, i))))->blend = 1;
                    (jo::as_material(material(add(base, i))))->texture_mode = 0x20000;
                }
            }
            joan::orig::as_texture_manager(img::obj(kTextures))->release(joan::orig::arg::mptr(tex));  // 0x0060a550 release
        }

        // uv table.
        Blob uv_blob;
        blob_init(&uv_blob, n_uv << 3);
        std::uint8_t* const uvs = blob_data(&uv_blob);
        if (read(file, uvs, n_uv << 3) == 0) {
            blob_free(&uv_blob);
            return fail();
        }

        // Bone influences.
        const std::int32_t vbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_vertices(joan::orig::arg::i32(n_vert)));  // 0x005cd7f0 add_vertices
        const std::int32_t sbase = skin_add(skin, n_vert);
        Vec3 pos{}, pos2{};       // ebp-0xd8 / ebp-0xe4
        std::uint32_t weight = 0;  // ebp-0xe8
        for (std::int32_t v = 0; v < n_vert; ++v) {
            const std::int32_t n_inf = read_int(file);
            const std::int32_t skip = read_int(file);
            for (std::int32_t k = 0; k < n_inf; ++k) {
                const std::int32_t bone = read_int(file);
                read(file, &pos, 0xc);
                pos2 = pos;
                read(file, &weight, 4);
                add_bone(skin, add(sbase, v), bone, &pos, &pos2, weight);
            }
            skin_normalize(skin, add(sbase, v));
            seek(file, skip << 2, 1);
        }

        std::int32_t first = 0;
        if (static_cast<std::int32_t>(jo::self<jo::Mesh>(this)->vertex_format) == 1) {
            for (std::int32_t v = 0; v < n_vert; ++v) {
                for (std::int32_t k = 0; k < 2; ++k) {
                    copy_influence(skinned(add(vbase, v)), skin, add(sbase, v), k, nullptr, nullptr);
                    jo::view<std::uint32_t>((jo::as_vertex_f7(skinned(add(vbase, v))))->tail[0]) = global<std::uint32_t>(kWhite);
                }
            }
            for (i = 0; i < n_mat; ++i) {
                const std::int32_t n = tri_count[i];
                const std::int32_t tbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_triangles(joan::orig::arg::i32(n)));  // 0x005ce6f0
                const std::uint8_t* const rec = elem(records.data, first, 0xc);
                for (std::int32_t t = 0; t < n; ++t) {
                    const std::uint8_t* const r = elem(rec, t, 0xc);
                    std::int32_t corner[3];
                    for (std::int32_t c = 0; c < 3; ++c) {
                        corner[c] = reinterpret_cast<const FileCorner*>(r)[c].vertex;
                        const std::int32_t uv = reinterpret_cast<const FileCorner*>(r)[c].uv;
                        const std::uint32_t u = reinterpret_cast<const FileUv*>(uvs)[uv].u;
                        const std::uint32_t w = reinterpret_cast<const FileUv*>(uvs)[uv].v;
                        std::uint8_t* const p = skinned(add(vbase, corner[c]));
                        jo::view<std::uint32_t>(jo::as_vertex_f7(p)->tail[1]) = u;
                        jo::view<std::uint32_t>(jo::as_vertex_f7(p)->tail[2]) = w;
                    }
                    joan::orig::as_plane(this)->set_triangle(joan::orig::arg::i32(add(tbase, t)), joan::orig::arg::u32(add(base, i)), joan::orig::arg::u32(add(vbase, corner[0])), joan::orig::arg::u32(add(vbase, corner[1])), joan::orig::arg::u32(add(vbase, corner[2])));  // 0x005ce800
                }
                first = add(first, n);
            }
        } else {
            for (std::int32_t v = 0; v < n_vert; ++v)
                jo::view<std::uint32_t>((jo::as_vertex_f1(vertex(add(vbase, v))))->attr[3]) = global<std::uint32_t>(kWhite);
            for (i = 0; i < n_mat; ++i) {
                const std::int32_t n = tri_count[i];
                const std::int32_t tbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_triangles(joan::orig::arg::i32(n)));  // 0x005ce6f0
                const std::uint8_t* const rec = elem(records.data, first, 0xc);
                for (std::int32_t t = 0; t < n; ++t) {
                    const std::uint8_t* const r = elem(rec, t, 0xc);
                    std::int32_t corner[3];
                    for (std::int32_t c = 0; c < 3; ++c) {
                        corner[c] = reinterpret_cast<const FileCorner*>(r)[c].vertex;
                        const std::int32_t uv = reinterpret_cast<const FileCorner*>(r)[c].uv;
                        const std::uint32_t u = reinterpret_cast<const FileUv*>(uvs)[uv].u;
                        const std::uint32_t w = reinterpret_cast<const FileUv*>(uvs)[uv].v;
                        std::uint8_t* const p = vertex(add(vbase, corner[c]));
                        jo::view<std::uint32_t>(jo::as_vertex_f1(p)->attr[4]) = u;
                        jo::view<std::uint32_t>(jo::as_vertex_f1(p)->attr[5]) = w;
                    }
                    joan::orig::as_plane(this)->set_triangle(joan::orig::arg::i32(add(tbase, t)), joan::orig::arg::u32(add(base, i)), joan::orig::arg::u32(add(vbase, corner[0])), joan::orig::arg::u32(add(vbase, corner[1])), joan::orig::arg::u32(add(vbase, corner[2])));  // 0x005ce800
                }
                first = add(first, n);
            }
        }
        joan::orig::as_skin(skin)->compute_bounds();       // 0x005f4a00
        joan::orig::as_plane(this)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        joan::orig::as_plane(this)->sort_by_material(0);   // 0x005cf1d0
        for (std::int32_t m = 0; m < jo::self<jo::Mesh>(this)->material_count; ++m)
            joan::orig::as_material(material(m))->set_colour4(joan::orig::arg::mptr(img::obj(kWhite)));  // 0x005d5570
        blob_free(&uv_blob);
        joan::orig::as_vqueue(&records)->free();  // 0x005a87b0
        blob_free(&count_blob);
        blob_free(&tex_blob);
        return 1;
    }

    // 0x005dc420 Mesh::load_v500(file, skin, flag).
    std::uint32_t load_v500(void* file, void* skin, std::uint32_t flag) {
        c_v500.hit();
        const std::int32_t n_mat = read_int(file);
        const std::int32_t n_tex = read_int(file);
        read_int(file);  // unused
        std::uint32_t header[4];
        if (read(file, header, 0x10) == 0) return 0;
        jo::as_skin(skin)->center[0] = header[0];
        jo::as_skin(skin)->center[1] = header[1];
        jo::as_skin(skin)->center[2] = header[2];
        jo::view<std::uint32_t>(jo::as_skin(skin)->radius) = header[3];
        jo::as_skin(skin)->radius = jo::as_skin(skin)->radius / 2.0f;  // fld; fdiv dword 0x9cf794 (2.0f); fstp
        jo::view<std::uint32_t>(jo::as_skin(skin)->i32_c) = 1;
        Blob tex_blob;
        blob_init(&tex_blob, n_mat << 2);
        std::int32_t* const tex_of = reinterpret_cast<std::int32_t*>(blob_data(&tex_blob));
        auto fail = [&]() -> std::uint32_t {
            blob_free(&tex_blob);
            return 0;
        };
        const std::int32_t base = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_materials(joan::orig::arg::i32(n_mat)));  // 0x005cd470 add_materials
        // Influence read buffers of the two vertex kinds (kind 7: ebp-0xdc / -0xe8 / -0xf4,
        // otherwise ebp-0x10c / -0x118 / -0x124).
        std::uint32_t w7 = 0, w0 = 0;
        Vec3 a7{}, b7{}, a0{}, b0{};
        std::int32_t i;
        for (i = 0; i < n_mat; ++i) {
            void* const mat = material(add(base, i));
            joan::orig::as_material(mat)->construct();  // 0x005d52a0 Material::init
            const std::int32_t n_strip = read_int(file);
            const std::int32_t n_vert = read_int(file);
            float rgba[4];
            std::uint8_t col[4];
            auto colour = [&]() {
                col[2] = channel(rgba[1]);
                col[1] = channel(rgba[2]);
                col[0] = channel(rgba[3]);
                col[3] = channel(rgba[0]);
            };
            if (read(file, rgba, 0x10) == 0) return fail();
            colour();
            joan::orig::as_material(mat)->set_colour4(joan::orig::arg::mptr(static_cast<void*>(col)));  // 0x005d5570
            if (read(file, rgba, 0x10) == 0) return fail();
            colour();
            joan::orig::as_material(mat)->set_colour0(joan::orig::arg::mptr(static_cast<void*>(col)));  // 0x005d5540
            if (read(file, rgba, 0x10) == 0) return fail();
            colour();
            joan::orig::as_material(mat)->set_colour8(joan::orig::arg::mptr(static_cast<void*>(col)), 0u);  // 0x005d55a0
            std::uint8_t unused[0x10];
            if (read(file, unused, 0x10) == 0) return fail();
            tex_of[i] = read_int(file);
            const std::int32_t vbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_vertices(joan::orig::arg::i32(n_vert)));  // 0x005cd7f0 add_vertices
            const std::int32_t sbase = skin_add(skin, n_vert);
            // (an `if (0)` triangle-list branch of the original is dead code)
            if (n_strip > 0) read_strip(this, file, add(base, i), n_strip, vbase);
            pump();
            if (static_cast<std::int32_t>(jo::self<jo::Mesh>(this)->vertex_format) == 7) {
                const Vec3 zero{};  // ebp-0xc4 / -0xc0 / -0xbc
                for (std::int32_t v = 0; v < n_vert; ++v) {
                    read(file, &jo::as_vertex_f7(skinned(add(vbase, v)))->tail[1], 8);  // uv
                    const std::int32_t skip = read_int(file);
                    const std::int32_t n_inf = read_int(file);
                    seek(file, skip << 2, 1);
                    for (std::int32_t k = 0; k < n_inf; ++k) {
                        const std::int32_t bone = read_int(file);
                        read(file, &w7, 4);
                        read(file, &a7, 0xc);
                        read(file, &b7, 0xc);
                        add_bone(skin, add(sbase, v), bone, &a7, &b7, w7);
                    }
                    skin_normalize(skin, add(sbase, v));
                    // (a "more than two influences" flag is set here and never read)
                    for (std::int32_t k = 0; k < 2; ++k) copy_influence(skinned(add(vbase, v)), skin, add(sbase, v), k, &zero, &n_inf);
                    jo::view<std::uint32_t>((jo::as_vertex_f7(skinned(add(vbase, v))))->tail[0]) = global<std::uint32_t>(kWhite);
                }
            } else {
                for (std::int32_t v = 0; v < n_vert; ++v) {
                    read(file, &jo::as_vertex_f1(vertex(add(vbase, v)))->attr[4], 8);  // uv
                    const std::int32_t skip = read_int(file);
                    const std::int32_t n_inf = read_int(file);
                    seek(file, skip << 2, 1);
                    for (std::int32_t k = 0; k < n_inf; ++k) {
                        const std::int32_t bone = read_int(file);
                        read(file, &w0, 4);
                        read(file, &a0, 0xc);
                        read(file, &b0, 0xc);
                        add_bone(skin, add(sbase, v), bone, &a0, &b0, w0);
                    }
                    skin_normalize(skin, add(sbase, v));
                    jo::view<std::uint32_t>((jo::as_vertex_f1(vertex(add(vbase, v))))->attr[3]) = global<std::uint32_t>(kWhite);
                }
            }
        }
        pump();
        joan::orig::as_plane(this)->set_flag50(1u);  // 0x005d7270

        // Textures.
        NameSlot slot;
        slot.table = joan::orig::arg::as<void**>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<std::uint32_t>(n_tex) * static_cast<std::uint32_t>(sizeof(void*)))));  // operator new (texture pointers; n_tex << 2 on x86)
        for (slot.t = 0; slot.t < n_tex; ++slot.t) {
            if (read(file, slot.name, 0x14) == 0) return fail();
            const std::int32_t size = read_int(file);
            char* const dot = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(static_cast<char*>(slot.name)), 0x2e));  // strrchr
            if (dot != nullptr) *dot = 0;
            void* tex = joan::orig::arg::as<void*>(joan::orig::as_textures(img::obj(kTextures))->find(joan::orig::arg::str(static_cast<char*>(slot.name))));  // 0x0060a4b0 find
            if (tex != nullptr) {
                joan::orig::as_texture_manager(img::obj(kTextures))->use(joan::orig::arg::mptr(tex));  // 0x0060a500 add reference
                if (size > 0) {
                    seek(file, size, 1);
                    pump();
                }
            } else if (size > 0) {
                Blob image;
                blob_init(&image, size);
                if (read(file, blob_data(&image), size) != 0)
                    tex = texture_from_memory(blob_data(&image), size, slot.name, tex);
                pump();
                blob_free(&image);
            } else {
                tex = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(static_cast<char*>(slot.name)), 1, joan::orig::arg::u32(static_cast<std::uint32_t>(static_cast<std::uint8_t>(flag)))));  // 0x006099c0 load
                if (tex != nullptr) pump();
            }
            for (i = 0; i < n_mat; ++i) {
                if (tex_of[i] == slot.t) joan::orig::as_material(material(add(base, i)))->set_texture(0, joan::orig::arg::mptr(tex));  // 0x005d5650 set_texture
            }
            slot.table[slot.t] = tex;
        }

        // Tagged extra sections.
        std::uint32_t two_sided = 0;  // ebp-0x178, only its low byte is read / tested
        for (;;) {
            const std::int32_t tag = read_int(file);
            if (tag == 1) {
                for (i = 0; i < n_mat; ++i) {
                    const std::int32_t a = read_int(file);
                    const std::int32_t b = read_int(file);
                    if (a != -1) {
                        jo::as_material(material(i))->pass_mask = 2;
                        jo::as_material(material(i))->blend = 1;
                        jo::as_material(material(i))->texture_mode = 0x20000;
                    }
                    if (b == -1) continue;
                    if (global<std::int32_t>(d::g_a52627) != 0) {
                        joan::orig::as_material(material(i))->set_texture(2, joan::orig::arg::mptr(jo::as_material(material(i))->textures[0]));
                        joan::orig::as_material(material(i))->set_texture(1, joan::orig::arg::mptr(jo::as_material(material(i))->textures[0]));
                        joan::orig::as_material(material(i))->set_texture(0, joan::orig::arg::mptr(slot.table[b]));
                        if (a == -1) {
                            jo::as_material(material(i))->blend = 8;
                            jo::as_material(material(i))->texture_mode = 9;
                        } else {
                            jo::as_material(material(i))->blend = 9;
                            jo::as_material(material(i))->texture_mode = 9;
                        }
                        joan::orig::as_plane(this)->set_format(5);  // 0x005cdc70
                        joan::orig::as_plane(this)->set_f8(1u);  // 0x005d5ee0
                    } else if (global<std::int32_t>(d::g_a5262f) != 0) {
                        joan::orig::as_material(material(i))->set_texture(1, joan::orig::arg::mptr(jo::as_material(material(i))->textures[0]));
                        joan::orig::as_material(material(i))->set_texture(0, joan::orig::arg::mptr(jo::as_material(material(i))->textures[0]));
                        jo::as_material(material(i))->blend = 0xa;
                        jo::as_material(material(i))->texture_mode = 0xb;
                    }
                }
            } else if (tag == 2) {
                for (i = 0; i < n_mat; ++i) {
                    const std::int32_t a = read_int(file);
                    read_int(file);
                    read(file, &two_sided, 1);
                    if (a != -1) {
                        jo::as_material(material(i))->power = 0x42480000u;  // 50.0f
                        if (jo::view<std::int32_t>(jo::as_material(material(i))->blend) == 0) jo::as_material(material(i))->blend = 0x14;
                    } else {
                        joan::orig::as_material(material(i))->set_colour8(joan::orig::arg::mptr(img::obj(d::g_init_black)), 0u);  // 0x005d55a0
                    }
                    if ((two_sided & 0xffu) != 0) {
                        const std::uint32_t v = jo::as_material(material(i))->raster;
                        jo::as_material(material(i))->raster = (v & 0xfffffff0u) | 2u;
                    }
                }
            } else {
                break;
            }
        }
        pump();
        joan::orig::as_plane(this)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        joan::orig::as_plane(this)->sort_by_material(0);   // 0x005cf1d0
        for (std::int32_t m = 0; m < jo::self<jo::Mesh>(this)->material_count; ++m)
            joan::orig::as_material(material(m))->set_colour4(joan::orig::arg::mptr(img::obj(kWhite)));  // 0x005d5570
        for (slot.t = 0; slot.t < n_tex; ++slot.t) joan::orig::as_texture_manager(img::obj(kTextures))->release(joan::orig::arg::mptr(slot.table[slot.t]));  // 0x0060a550 release
        joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(slot.table)));  // operator delete
        blob_free(&tex_blob);
        return 1;
    }
};

#define B85_02(name, addr, fn) JOAN_REPLACE("en3d/b85_02", name, addr, fn)
B85_02("Plane::read_file_amo0", 0x005da3d0, &H::load_v0);
B85_02("Mesh::load_v500", 0x005dc420, &H::load_v500);
#undef B85_02

}  // namespace en3d_b85_02
}  // namespace

// ---------------------------------------- from modules/en3d_b85_03.cpp (en3d/b85_03)
//
//      0x005db710 Plane::load_skin(reader, shadow, byte flag) (ret 0xc) - another Mesh::load_any
//                 format: material / texture / (unused) counts; per material tri / vertex counts,
//                 three 0..1 colours, texture index, 6-byte triangles (0x005ce800), then per vertex
//                 8 bytes into the 0x48 skin vertex (+0x34, kind 7) or the 0x24 vertex (+0x1c),
//                 a skipped block, bone weights added to the shadow list (0x005f47c0 / 0x005f4970 /
//                 0x005f49c0; kind 7 copies up to two weights into the vertex), then the textures
//                 (20-byte name + embedded image, D3DXCreateTextureFromFileInMemory or by name),
//                 0x005f4a00, 0x005cf920(-1), 0x005cf1d0(0), white material colours; 1 / 0
namespace {
namespace en3d_b85_03_2 {

using joan::hook::global;

struct Obj {};
using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::stdcall_call;
using joan::img::obj;
using joan::img::str;

using joan::orig::helpers::mul;
std::int32_t add(std::int32_t a, std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

void* const kTextures = obj(d::g_texture_array);  // texture manager
constexpr img::Addr kWhite = d::g_init_white;  // default colour dword

std::int32_t read_int(void* file) { return joan::orig::as_file_mem(file)->file_get_long(); }   // 0x005893c0
std::int32_t read(void* file, void* buffer, std::int32_t size) {
    return joan::orig::as_file_mem(file)->file_read(joan::orig::arg::mptr(buffer), joan::orig::arg::u32(size));  // 0x00588a80
}
void seek(void* file, std::int32_t offset, std::int32_t mode) { joan::orig::as_file_mem(file)->file_seek(joan::orig::arg::i32(offset), joan::orig::arg::i32(mode)); }  // 0x005888c0

struct Blob {  // ilib Blob: +0 data, +4 size (0x0057f770 writes both)
    void* data;
    std::uint32_t size;
};
static_assert(offsetof(Blob, size) == sizeof(void*) && sizeof(Blob) >= sizeof(joan::orig::Blob), "Blob");  // x86: 8 bytes
void blob_init(Blob* b, std::int32_t n) { joan::orig::as_blob(b)->construct(joan::orig::arg::i32(n)); }  // 0x0057f770
std::uint8_t* blob_data(Blob* b) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_blob(b)->data_56de20()); }  // 0x0056de20
void blob_free(Blob* b) { joan::orig::as_blob(b)->destruct(); }                         // 0x0057f850

// 20-byte texture name followed by the loop counter, as in the original frame (a name without
// a terminator runs into the counter's bytes there too).
struct NameSlot {
    char name[0x14];
    std::int32_t j;
};
static_assert(sizeof(NameSlot) == 0x18, "name + counter");

std::uint8_t component(float v) { return static_cast<std::uint8_t>(joan::hook::x87::ftol(v * 255.0f)); }  // 0x9cfb68

// Reads a 12-byte 0..1 colour; on success builds the D3DCOLOR (alpha 0xff) in *out.
bool read_colour(void* file, std::uint8_t* out) {
    float c[3];
    if (read(file, c, 0xc) == 0) return false;
    out[2] = component(c[0]);
    out[1] = component(c[1]);
    out[0] = component(c[2]);
    out[3] = 0xff;
    return true;
}

std::int32_t texture_kind(void* texture) {  // vtable slot 2 (thiscall)
    using Virtual = std::int32_t (Obj::*)();
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(texture)[2], sizeof(void*));
    return (static_cast<Obj*>(texture)->*method)();
}

JOAN_COUNTER(c_load_skin, "Plane::load_skin");

struct H {
    std::uint8_t* material(std::int32_t index) { return jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->materials) + mul(index, static_cast<std::int32_t>(sizeof(jo::Material))); }

    // Per-vertex bone weights: count / skip header, then (bone, weight, offset) records added to
    // shadow entry `entry` (0x005f4970), closed by 0x005f49c0.
    static void read_weights(void* file, void* shadow, std::int32_t entry) {
        const std::int32_t skip = read_int(file);
        const std::int32_t n = read_int(file);
        seek(file, skip << 2, 1);
        for (std::int32_t w = 0; w < n; ++w) {
            const std::int32_t bone = read_int(file);
            std::uint32_t weight;
            read(file, &weight, 4);
            std::uint32_t offset[3];
            read(file, offset, 0xc);
            std::uint32_t copy[3] = {offset[0], offset[1], offset[2]};
            joan::orig::as_skin(shadow)->add_bone(joan::orig::arg::i32(entry), joan::orig::arg::i32(bone), joan::orig::arg::mptr(static_cast<void*>(offset)), joan::orig::arg::mptr(static_cast<void*>(copy)), joan::orig::arg::u32(weight));  // 0x005f4970
        }
        joan::orig::as_skin(shadow)->normalize(joan::orig::arg::i32(entry));  // 0x005f49c0
    }

    // 0x005db710
    std::uint32_t load_skin(void* file, void* shadow, std::uint32_t flag) {
        c_load_skin.hit();
        const std::int32_t n_materials = read_int(file);
        const std::int32_t n_textures = read_int(file);
        read_int(file);  // unused count
        Blob tex_blob;
        blob_init(&tex_blob, n_materials << 2);
        std::int32_t* const tex_index = reinterpret_cast<std::int32_t*>(blob_data(&tex_blob));
        auto fail = [&]() -> std::uint32_t {
            blob_free(&tex_blob);
            return 0;
        };
        const std::int32_t base = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_materials(joan::orig::arg::i32(n_materials)));  // 0x005cd470
        for (std::int32_t i = 0; i < n_materials; ++i) {
            std::uint8_t* const m = material(add(base, i));
            joan::orig::as_material(m)->construct();  // 0x005d52a0
            const std::int32_t n_tris = read_int(file);
            const std::int32_t n_verts = read_int(file);
            std::uint8_t colour[4];
            if (!read_colour(file, colour)) return fail();
            joan::orig::as_material(m)->set_colour4(joan::orig::arg::mptr(static_cast<void*>(colour)));  // 0x005d5570
            if (!read_colour(file, colour)) return fail();
            joan::orig::as_material(m)->set_colour0(joan::orig::arg::mptr(static_cast<void*>(colour)));  // 0x005d5540
            if (!read_colour(file, colour)) return fail();
            joan::orig::as_material(m)->set_colour8(joan::orig::arg::mptr(static_cast<void*>(colour)), 0u);  // 0x005d55a0
            tex_index[i] = read_int(file);
            const std::int32_t vbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_vertices(joan::orig::arg::i32(n_verts)));  // 0x005cd7f0
            const std::int32_t tbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_triangles(joan::orig::arg::i32(n_tris)));   // 0x005ce6f0
            const std::int32_t sbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_skin(shadow)->add(joan::orig::arg::i32(n_verts)));  // 0x005f47c0
            for (std::int32_t t = 0; t < n_tris; ++t) {
                std::uint16_t tri[3];
                if (read(file, tri, 6) == 0) return fail();
                joan::orig::as_plane(this)->set_triangle(joan::orig::arg::i32(add(tbase, t)), joan::orig::arg::u32(add(base, i)), joan::orig::arg::u32(add(vbase, tri[0])), joan::orig::arg::u32(add(vbase, tri[1])), joan::orig::arg::u32(add(vbase, tri[2])));  // 0x005ce800
            }
            if (static_cast<std::int32_t>(jo::self<jo::Plane>(this)->vertex_format) == 7) {
                for (std::int32_t v = 0; v < n_verts; ++v) {
                    read(file, &jo::as_vertex_f7(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f7) + mul(add(vbase, v), 0x48))->tail[1], 8);
                    const std::int32_t skip = read_int(file);
                    const std::int32_t n = read_int(file);
                    seek(file, skip << 2, 1);
                    for (std::int32_t w = 0; w < n; ++w) {
                        const std::int32_t bone = read_int(file);
                        std::uint32_t weight;
                        read(file, &weight, 4);
                        std::uint32_t offset[3];
                        read(file, offset, 0xc);
                        std::uint32_t copy[3] = {offset[0], offset[1], offset[2]};
                        joan::orig::as_skin(shadow)->add_bone(joan::orig::arg::i32(add(sbase, v)), joan::orig::arg::i32(bone), joan::orig::arg::mptr(static_cast<void*>(offset)), joan::orig::arg::mptr(static_cast<void*>(copy)), joan::orig::arg::u32(weight));  // 0x005f4970
                    }
                    joan::orig::as_skin(shadow)->normalize(joan::orig::arg::i32(add(sbase, v)));  // 0x005f49c0
                    for (std::int32_t w = 0; w < 2; ++w) {
                        std::uint8_t* const dst = jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f7) + mul(add(vbase, v), 0x48);
                        if (w < n) {
                            const jo::SkinInfluence& src = (&jo::as_skin_vertex(jo::view<std::uint8_t*>(jo::as_skin(shadow)->vertices) + mul(add(sbase, v), static_cast<std::int32_t>(sizeof(jo::SkinVertex))))->bone0)[w];
                            std::memcpy(&jo::as_vertex_f7(dst)->influence_pos[w * 3], src.pos, 0xc);
                            std::memcpy(&jo::as_vertex_f7(dst)->influence_normal[w * 3], src.normal, 0xc);
                            jo::as_vertex_f7(dst)->bone[w] = static_cast<std::uint16_t>(mul(*static_cast<const std::int32_t*>(static_cast<const void*>(&src.bone)), 3));  // the bone as an aligned dword read (as the original)
                            jo::as_vertex_f7(dst)->weight[w] = jo::view<std::uint32_t>(src.weight);
                        } else {
                            jo::as_vertex_f7(dst)->bone[w] = 0;
                            jo::as_vertex_f7(dst)->weight[w] = 0;
                        }
                    }
                    jo::view<std::uint32_t>(jo::as_vertex_f7(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f7) + mul(add(vbase, v), 0x48))->tail[0]) = global<std::uint32_t>(kWhite);
                }
            } else {
                for (std::int32_t v = 0; v < n_verts; ++v) {
                    read(file, &jo::as_vertex_f1(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f1) + mul(add(vbase, v), 0x24))->attr[4], 8);
                    read_weights(file, shadow, add(sbase, v));
                    jo::view<std::uint32_t>(jo::as_vertex_f1(jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f1) + mul(add(vbase, v), 0x24))->attr[3]) = global<std::uint32_t>(kWhite);
                }
            }
        }
        NameSlot slot;
        for (slot.j = 0; slot.j < n_textures; ++slot.j) {
            if (read(file, slot.name, 0x14) == 0) return fail();
            const std::int32_t size = read_int(file);
            char* const dot = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(static_cast<char*>(slot.name)), 0x2e));  // strchr
            if (dot != nullptr) *dot = 0;
            void* texture = joan::orig::arg::as<void*>(joan::orig::as_textures(kTextures)->find(joan::orig::arg::str(static_cast<char*>(slot.name))));  // 0x0060a4b0 find
            if (texture != nullptr) {
                joan::orig::as_texture_manager(kTextures)->use(joan::orig::arg::mptr(texture));  // 0x0060a500 add ref
                if (size > 0) seek(file, size, 1);
            } else if (size > 0) {
                Blob image;
                blob_init(&image, size);
                if (read(file, blob_data(&image), size) != 0) {
                    void* d3d_texture = nullptr;  // uninitialised in the original; only used when hr == 0
                    const std::int32_t hr = joan::orig::arg::as<std::int32_t>(joan::orig::d3dx::create_texture_from_file_in_memory(joan::orig::arg::mptr(global<void*>(d::g_game_device)), joan::orig::arg::mptr(static_cast<void*>(blob_data(&image))), joan::orig::arg::u32(size), &d3d_texture));  // D3DXCreateTextureFromFileInMemory
                    if (hr == 0) {
                        void* const object = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(kTextures)->create_2d());  // 0x006094c0 new 2D texture
                        joan::orig::as_texture2d(object)->attach(joan::orig::arg::mptr(d3d_texture), 1u);                  // 0x00605f00
                        joan::orig::as_texture(object)->set_name(joan::orig::arg::str(static_cast<char*>(slot.name)));   // 0x00605ad0 set name
                        texture = object;
                        reinterpret_cast<std::uint32_t(JOAN_STDCALL*)(void*)>(joan::call::vtable_of(d3d_texture)[2])(
                            d3d_texture);  // Release
                    }
                }
                blob_free(&image);
            } else {
                texture = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(kTextures)->load(joan::orig::arg::str(static_cast<char*>(slot.name)), 1, joan::orig::arg::u32(flag & 0xffu)));  // 0x006099c0 load (byte flag)
            }
            for (std::int32_t i = 0; i < n_materials; ++i) {
                if (tex_index[i] != slot.j) continue;
                joan::orig::as_material(material(add(base, i)))->set_texture(0, joan::orig::arg::mptr(texture));  // 0x005d5650
                if (texture != nullptr && texture_kind(texture) == 1) {
                    (jo::as_material(material(add(base, i))))->pass_mask = 2;
                    (jo::as_material(material(add(base, i))))->blend = 1;
                    (jo::as_material(material(add(base, i))))->texture_mode = 0x20000;
                }
            }
            joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(texture));  // 0x0060a550 release
        }
        joan::orig::as_skin(shadow)->compute_bounds();    // 0x005f4a00
        joan::orig::as_plane(this)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        joan::orig::as_plane(this)->sort_by_material(0);   // 0x005cf1d0
        for (std::int32_t m = 0; m < jo::self<jo::Plane>(this)->material_count; ++m)
            joan::orig::as_material(material(m))->set_colour4(joan::orig::arg::mptr(obj(kWhite)));  // 0x005d5570
        blob_free(&tex_blob);
        return 1;
    }
};

#define B85_03(name, addr, fn) JOAN_REPLACE("en3d/b85_03", name, addr, fn)
B85_03("Plane::read_file_amo50", 0x005db710, &H::load_skin);
#undef B85_03

}  // namespace en3d_b85_03_2
}  // namespace

// ---------------------------------------- from modules/game_collide1.cpp (game/collide1)
//
//  * module game/collide1:
//      Plane::segment_hit 0x005d4a80 (en3d/plane.cpp: the plane mesh (+0x40 triangle count,
//        +0x48 triangle index words, +0x1c..+0x2c vertex streams by format +4) against a
//        segment given as two points, transformed into the mesh space when its matrix is not
//        the identity; keeps the nearest parameter in [0, 1] in *out and returns the 1-based
//        triangle index of the last accepted hit, 0 when none),
//      Plane::set_100 0x005d6e20 (+0x100 = arg),
//      Plane::load 0x005ddd70 (Plane::init, then dispatches on the file version 0 / 50 / 100 /
//        500 to the version readers),
//      Shape::cylinder_hit 0x00699ec0 (shape 3 of Sky::ray_clear: vertical cylinder, side and
//        caps), Shape::box_hit 0x00698970 (shape 2: slab test on an AABB),
//      Shape::sphere_hit 0x006a0f90 (shape 1),
//      TerrainBlock::height 0x00664180 (height map of the block (cell size +0x24 / +0x28,
//        tiles +0x34) or the triangle of the block (+0x30); returns st0, which is an exact
//        integer (fild) on the degenerate triangle path, so the result is a double).
namespace {
namespace game_collide1_2 {

using joan::call::thiscall;

JOAN_COUNTER(c_load, "Plane::load");

struct H {
    // 0x005ddd70 Plane::load(file, flag).
    std::uint32_t load(void* file, std::uint8_t flag) {
        c_load.hit();
        joan::orig::as_plane(this)->init(0, 0, 0, 1u);  // 0x005cc7a0 Plane::init
        joan::orig::as_plane(this)->set_format(1);           // 0x005cdc70
        const std::int32_t version = joan::orig::as_file_mem(file)->file_get_long();   // 0x005893c0
        jo::self<jo::Plane>(this)->list_kind = 4;
        switch (version) {
            case 0: return joan::orig::arg::as<std::uint32_t>(joan::orig::as_plane(this)->load_v0(joan::orig::arg::mptr(file), static_cast<std::uint8_t>(joan::orig::arg::u32(flag))));    // 0x005dde90
            case 50: return joan::orig::arg::as<std::uint32_t>(joan::orig::as_plane(this)->load_v50(joan::orig::arg::mptr(file), static_cast<std::uint8_t>(joan::orig::arg::u32(flag))));   // 0x005decf0
            case 100: return joan::orig::arg::as<std::uint32_t>(joan::orig::as_plane(this)->load_v100(joan::orig::arg::mptr(file), static_cast<std::uint8_t>(joan::orig::arg::u32(flag))));  // 0x005e0510
            case 500: return joan::orig::arg::as<std::uint32_t>(joan::orig::as_plane(this)->load_v500(joan::orig::arg::mptr(file), static_cast<std::uint8_t>(joan::orig::arg::u32(flag))));  // 0x005e0530
            default: return 0;
        }
    }
};

#define CL1(name, addr, fn) JOAN_REPLACE("game/collide1", name, addr, fn)
CL1("Plane::load", 0x005ddd70, &H::load);
#undef CL1

}  // namespace game_collide1_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers35.cpp (en3d/helpers35)
//
//  * module en3d/helpers35:
//      Plane::load_v0 0x005dde90 (file version 0: material / texture / uv / vertex counts and a
//        trailing block count; per material three 0..1 colours, a texture index, its triangle
//        count and a 12-byte record per triangle (vertex index, uv index) x 3; then the
//        textures (20-byte name + embedded image size, created through
//        D3DXCreateTextureFromFileInMemory or loaded by name), the uv table (8 bytes each), the
//        vertex table (position + normal, 24 bytes each), the triangles, and skipped blocks),
//      Plane::load_v50 0x005decf0 (file version 50: per material colours, texture index (bit
//        31 = no alpha flags), 6-byte triangles and 32-byte vertices (normal, position, uv);
//        then the same textures and skipped blocks).
//    Both return 1 on success and 0 on a short read, after destroying their Blob / VQueue
//    locals in the same order as the original's unwind code.
namespace {
namespace en3d_helpers35 {

// File records of the mesh loaders: a triangle corner (vertex, uv index) and a uv pair (raw dwords).
struct FileCorner {
    std::uint16_t vertex, uv;
};
struct FileUv {
    std::uint32_t u, v;
};

using joan::hook::global;

struct Obj {};
using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::stdcall_call;

using joan::orig::helpers::mul;
std::int32_t add(std::int32_t a, std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

constexpr img::Addr kTextures(d::g_texture_array);  // texture manager
constexpr img::Addr kWhite = d::g_init_white;                  // default colour dword

// File helpers (ilib File).
std::int32_t read_int(void* file) { return joan::orig::as_file_mem(file)->file_get_long(); }   // 0x005893c0
std::int32_t read(void* file, void* buffer, std::int32_t size) {
    return joan::orig::as_file_mem(file)->file_read(joan::orig::arg::mptr(buffer), joan::orig::arg::u32(size));  // 0x00588a80
}
void seek(void* file, std::int32_t offset, std::int32_t mode) { joan::orig::as_file_mem(file)->file_seek(joan::orig::arg::i32(offset), joan::orig::arg::i32(mode)); }  // 0x005888c0

struct Blob {  // ilib Blob: +0 data, +4 size
    void* data;
    std::uint32_t size;
};
struct VQueue {  // ilib VQueue: +0 data, +4, +8 used, +0xc
    std::uint8_t* data;
    std::uint32_t capacity;
    std::int32_t length;
    std::uint8_t* cursor;
};
// x86: 8 and 16 bytes; x64: room for the (larger) ilib objects.
static_assert(offsetof(Blob, size) == sizeof(void*) && sizeof(Blob) >= sizeof(joan::orig::Blob), "Blob");
static_assert(offsetof(VQueue, cursor) == offsetof(joan::orig::VQueue, cursor) && sizeof(VQueue) >= sizeof(joan::orig::VQueue), "VQueue");
void blob_init(Blob* b, std::int32_t n) { joan::orig::as_blob(b)->construct(joan::orig::arg::i32(n)); }  // 0x0057f770
std::uint8_t* blob_data(Blob* b) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_blob(b)->data_56de20()); }  // 0x0056de20
void blob_free(Blob* b) { joan::orig::as_blob(b)->destruct(); }                         // 0x0057f850

// 20-byte texture name followed by the loop counter, as in the original frame (a name without
// a terminator runs into the counter's bytes there too).
struct NameSlot {
    char name[0x14];
    std::int32_t j;
};
static_assert(sizeof(NameSlot) == 0x18, "name + counter");

JOAN_COUNTER(c_v0, "Plane::load_v0");
JOAN_COUNTER(c_v50, "Plane::load_v50");

std::uint8_t component(float v) { return static_cast<std::uint8_t>(joan::hook::x87::ftol(v * 255.0f)); }

// Reads a 12-byte 0..1 colour; on success builds the D3DCOLOR (alpha 0xff) in *out.
bool read_colour(void* file, std::uint8_t* out) {
    float c[3];
    if (read(file, c, 0xc) == 0) return false;
    out[2] = component(c[0]);
    out[1] = component(c[1]);
    out[0] = component(c[2]);
    out[3] = 0xff;
    return true;
}

// Texture block j: name, embedded image size, then texture by name or from the image bytes.
void* read_texture(void* file, NameSlot* slot, std::uint8_t flag, bool* ok) {
    *ok = true;
    if (read(file, slot->name, 0x14) == 0) {
        *ok = false;
        return nullptr;
    }
    const std::int32_t size = read_int(file);
    char* const dot = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(slot->name), 0x2e));  // strchr
    if (dot != nullptr) *dot = 0;
    void* texture = joan::orig::arg::as<void*>(joan::orig::as_textures(img::obj(kTextures))->find(joan::orig::arg::str(static_cast<char*>(slot->name))));  // 0x0060a4b0 find
    if (texture != nullptr) {
        joan::orig::as_texture_manager(img::obj(kTextures))->use(joan::orig::arg::mptr(texture));  // 0x0060a500 add ref
        if (size > 0) seek(file, size, 1);
    } else if (size > 0) {
        Blob image;
        blob_init(&image, size);
        if (read(file, blob_data(&image), size) != 0) {
            void* d3d_texture = nullptr;  // uninitialised in the original; only used when hr == 0
            const std::int32_t hr = joan::orig::arg::as<std::int32_t>(joan::orig::d3dx::create_texture_from_file_in_memory(joan::orig::arg::mptr(global<void*>(d::g_game_device)), joan::orig::arg::mptr(static_cast<void*>(blob_data(&image))), joan::orig::arg::u32(size), &d3d_texture));  // D3DXCreateTextureFromFileInMemory
            if (hr == 0) {
                void* const object = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTextures))->create_2d());  // 0x006094c0 new 2D texture
                joan::orig::as_texture2d(object)->attach(joan::orig::arg::mptr(d3d_texture), 1u);                // 0x00605f00
                joan::orig::as_texture(object)->set_name(joan::orig::arg::str(static_cast<char*>(slot->name)));  // 0x00605ad0 set name
                texture = object;
                reinterpret_cast<std::uint32_t(JOAN_STDCALL*)(void*)>(joan::call::vtable_of(d3d_texture)[2])(d3d_texture);  // Release
            }
        }
        blob_free(&image);
    } else {
        texture = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(static_cast<char*>(slot->name)), 1, joan::orig::arg::u32(static_cast<std::uint32_t>(flag))));  // 0x006099c0 load
    }
    return texture;
}

std::int32_t texture_kind(void* texture) {  // vtable slot 2
    using Virtual = std::int32_t (Obj::*)();
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(texture)[2], sizeof(void*));
    return (static_cast<Obj*>(texture)->*method)();
}

struct H {
    std::uint8_t* material(std::int32_t index) { return jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->materials) + mul(index, static_cast<std::int32_t>(sizeof(jo::Material))); }
    std::uint8_t* vertex(std::int32_t index) { return jo::view<std::uint8_t*>(jo::self<jo::Plane>(this)->vertices_f1) + mul(index, 0x24); }

    void set_alpha_flags(std::int32_t m) {
        jo::as_material(material(m))->pass_mask = 2;
        jo::as_material(material(m))->blend = 1;
        jo::as_material(material(m))->texture_mode = 0x20000;
    }
    void finish() {
        joan::orig::as_plane(this)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        joan::orig::as_plane(this)->sort_by_material(0);   // 0x005cf1d0
        for (std::int32_t m = 0; m < jo::self<jo::Plane>(this)->material_count; ++m)
            joan::orig::as_material(material(m))->set_colour4(joan::orig::arg::mptr(img::obj(kWhite)));  // 0x005d5570
    }
    // Three colours of a material: 0x005d5570, 0x005d5540, 0x005d55a0(colour, 0).
    bool read_colours(void* file, std::uint8_t* m) {
        std::uint8_t colour[4];
        if (!read_colour(file, colour)) return false;
        joan::orig::as_material(m)->set_colour4(joan::orig::arg::mptr(static_cast<void*>(colour)));  // 0x005d5570
        if (!read_colour(file, colour)) return false;
        joan::orig::as_material(m)->set_colour0(joan::orig::arg::mptr(static_cast<void*>(colour)));  // 0x005d5540
        if (!read_colour(file, colour)) return false;
        joan::orig::as_material(m)->set_colour8(joan::orig::arg::mptr(static_cast<void*>(colour)), 0u);  // 0x005d55a0
        return true;
    }

    // 0x005dde90 Plane::load_v0(file, flag).
    std::uint32_t load_v0(void* file, std::uint8_t flag) {
        c_v0.hit();
        const std::int32_t n_materials = read_int(file);
        const std::int32_t n_textures = read_int(file);
        const std::int32_t n_uvs = read_int(file);
        const std::int32_t n_vertices = read_int(file);
        const std::int32_t n_blocks = read_int(file);
        Blob tex_blob;
        blob_init(&tex_blob, n_materials << 2);
        std::int32_t* const tex_index = reinterpret_cast<std::int32_t*>(blob_data(&tex_blob));
        Blob count_blob;
        blob_init(&count_blob, n_materials << 2);
        std::int32_t* const tri_count = reinterpret_cast<std::int32_t*>(blob_data(&count_blob));
        VQueue records;
        joan::orig::as_vqueue(&records)->construct();  // 0x005a8640
        auto fail = [&]() -> std::uint32_t {
            joan::orig::as_vqueue(&records)->free();  // 0x005a87b0
            blob_free(&count_blob);
            blob_free(&tex_blob);
            return 0;
        };
        const std::int32_t base = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_materials(joan::orig::arg::i32(n_materials)));  // 0x005cd470
        for (std::int32_t i = 0; i < n_materials; ++i) {
            std::uint8_t* const m = material(add(base, i));
            joan::orig::as_material(m)->construct();  // 0x005d52a0
            const std::int32_t n = read_int(file);
            if (!read_colours(file, m)) return fail();
            tex_index[i] = read_int(file);
            tri_count[i] = n;
            const std::int32_t bytes = mul(n, 0xc);
            void* const buffer = joan::orig::arg::as<void*>(joan::orig::as_vqueue(&records)->push(joan::orig::arg::u32(mul(n, 0xc))));  // 0x005a88c0
            if (read(file, buffer, bytes) == 0) return fail();
        }
        NameSlot slot;
        for (slot.j = 0; slot.j < n_textures; ++slot.j) {
            bool ok;
            void* const texture = read_texture(file, &slot, flag, &ok);
            if (!ok) return fail();
            for (std::int32_t i = 0; i < n_materials; ++i) {
                if (tex_index[i] != slot.j) continue;
                joan::orig::as_material(material(add(base, i)))->set_texture(0, joan::orig::arg::mptr(texture));  // 0x005d5650
                if (texture != nullptr && texture_kind(texture) == 1) set_alpha_flags(add(base, i));
            }
            joan::orig::as_texture_manager(img::obj(kTextures))->release(joan::orig::arg::mptr(texture));  // 0x0060a550 release
        }
        Blob uv_blob;
        blob_init(&uv_blob, n_uvs << 3);
        std::uint8_t* const uvs = blob_data(&uv_blob);
        if (read(file, uvs, n_uvs << 3) == 0) {
            blob_free(&uv_blob);
            return fail();
        }
        Blob vertex_blob;
        blob_init(&vertex_blob, mul(n_vertices, 0x18));
        std::uint8_t* const vertices = blob_data(&vertex_blob);
        if (read(file, vertices, mul(n_vertices, 0x18)) == 0) {
            blob_free(&vertex_blob);
            blob_free(&uv_blob);
            return fail();
        }
        const std::int32_t vbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_vertices(joan::orig::arg::i32(n_vertices)));  // 0x005cd7f0
        for (std::int32_t k = 0; k < n_vertices; ++k) {
            std::uint8_t* const v = vertex(add(vbase, k));
            std::memcpy(v, vertices + mul(k, 0x18), 0xc);
            std::memcpy(v + 0xc, vertices + mul(k, 0x18) + 0xc, 0xc);
            jo::view<std::uint32_t>(jo::as_vertex_f1(v)->attr[3]) = global<std::uint32_t>(kWhite);
        }
        std::int32_t first = 0;
        for (std::int32_t i = 0; i < n_materials; ++i) {
            const std::int32_t n = tri_count[i];
            const std::int32_t tbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_triangles(joan::orig::arg::i32(n)));  // 0x005ce6f0
            const std::uint8_t* const rec = records.data + mul(first, 0xc);
            for (std::int32_t t = 0; t < n; ++t) {
                const std::uint8_t* const r = rec + mul(t, 0xc);
                std::int32_t corner[3];
                for (std::int32_t c = 0; c < 3; ++c) {
                    corner[c] = reinterpret_cast<const FileCorner*>(r)[c].vertex;
                    const std::int32_t uv = reinterpret_cast<const FileCorner*>(r)[c].uv;
                    const std::uint32_t u = reinterpret_cast<const FileUv*>(uvs)[uv].u;
                    const std::uint32_t w = reinterpret_cast<const FileUv*>(uvs)[uv].v;
                    std::uint8_t* const v = vertex(add(vbase, corner[c]));
                    jo::view<std::uint32_t>(jo::as_vertex_f1(v)->attr[4]) = u;
                    jo::view<std::uint32_t>(jo::as_vertex_f1(v)->attr[5]) = w;
                }
                joan::orig::as_plane(this)->set_triangle(joan::orig::arg::i32(add(tbase, t)), joan::orig::arg::u32(add(base, i)), joan::orig::arg::u32(add(vbase, corner[0])), joan::orig::arg::u32(add(vbase, corner[1])), joan::orig::arg::u32(add(vbase, corner[2])));  // 0x005ce800
            }
            first = add(first, n);
        }
        for (std::int32_t k = 0; k < n_blocks; ++k) {
            const std::int32_t a = read_int(file);
            const std::int32_t b = read_int(file);
            seek(file, a << 3, -1);
            seek(file, mul(b, 0x18), -1);
            for (std::int32_t i = 0; i < n_materials; ++i) {
                const std::int32_t c = read_int(file);
                seek(file, mul(c, 0xc), -1);
            }
        }
        finish();
        blob_free(&vertex_blob);
        blob_free(&uv_blob);
        joan::orig::as_vqueue(&records)->free();  // 0x005a87b0
        blob_free(&count_blob);
        blob_free(&tex_blob);
        return 1;
    }

    // 0x005decf0 Plane::load_v50(file, flag).
    std::uint32_t load_v50(void* file, std::uint8_t flag) {
        c_v50.hit();
        const std::int32_t n_materials = read_int(file);
        const std::int32_t n_textures = read_int(file);
        const std::int32_t n_blocks = read_int(file);
        Blob tex_blob;
        blob_init(&tex_blob, n_materials << 2);
        std::int32_t* const tex_index = reinterpret_cast<std::int32_t*>(blob_data(&tex_blob));
        auto fail = [&]() -> std::uint32_t {
            blob_free(&tex_blob);
            return 0;
        };
        const std::int32_t base = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_materials(joan::orig::arg::i32(n_materials)));  // 0x005cd470
        for (std::int32_t i = 0; i < n_materials; ++i) {
            std::uint8_t* const m = material(add(base, i));
            joan::orig::as_material(m)->construct();  // 0x005d52a0
            const std::int32_t n_tris = read_int(file);
            const std::int32_t n_verts = read_int(file);
            if (!read_colours(file, m)) return fail();
            tex_index[i] = read_int(file);
            const std::int32_t vbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_vertices(joan::orig::arg::i32(n_verts)));  // 0x005cd7f0
            const std::int32_t tbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_triangles(joan::orig::arg::i32(n_tris)));   // 0x005ce6f0
            for (std::int32_t t = 0; t < n_tris; ++t) {
                std::uint16_t tri[3];
                if (read(file, tri, 6) == 0) return fail();
                joan::orig::as_plane(this)->set_triangle(joan::orig::arg::i32(add(tbase, t)), joan::orig::arg::u32(add(base, i)), joan::orig::arg::u32(add(vbase, tri[0])), joan::orig::arg::u32(add(vbase, tri[1])), joan::orig::arg::u32(add(vbase, tri[2])));  // 0x005ce800
            }
            for (std::int32_t k = 0; k < n_verts; ++k) {
                std::uint32_t rec[8];  // normal, position, uv
                if (read(file, rec, 0x20) == 0) return fail();
                std::uint8_t* const v = vertex(add(vbase, k));
                std::memcpy(jo::as_vertex_f1(v)->pos, &rec[3], 0xc);
                std::memcpy(jo::as_vertex_f1(vertex(add(vbase, k)))->attr, &rec[0], 0xc);
                jo::view<std::uint32_t>((jo::as_vertex_f1(vertex(add(vbase, k))))->attr[4]) = rec[6];
                jo::view<std::uint32_t>((jo::as_vertex_f1(vertex(add(vbase, k))))->attr[5]) = rec[7];
                jo::view<std::uint32_t>((jo::as_vertex_f1(vertex(add(vbase, k))))->attr[3]) = global<std::uint32_t>(kWhite);
            }
        }
        NameSlot slot;
        for (slot.j = 0; slot.j < n_textures; ++slot.j) {
            bool ok;
            void* const texture = read_texture(file, &slot, flag, &ok);
            if (!ok) return fail();
            for (std::int32_t i = 0; i < n_materials; ++i) {
                const std::int32_t index = tex_index[i];
                if ((index & 0x7fffffff) != slot.j) continue;
                joan::orig::as_material(material(add(base, i)))->set_texture(0, joan::orig::arg::mptr(texture));  // 0x005d5650
                if (texture != nullptr && index >= 0 && texture_kind(texture) == 1) set_alpha_flags(add(base, i));
            }
            joan::orig::as_texture_manager(img::obj(kTextures))->release(joan::orig::arg::mptr(texture));  // 0x0060a550 release
        }
        for (std::int32_t k = 0; k < n_blocks; ++k) {
            for (std::int32_t i = 0; i < n_materials; ++i) {
                const std::int32_t a = read_int(file);
                const std::int32_t b = read_int(file);
                seek(file, mul(a, 6), -1);
                seek(file, b << 5, -1);
            }
        }
        finish();
        blob_free(&tex_blob);
        return 1;
    }
};

#define HELPER35(name, addr, fn) JOAN_REPLACE("en3d/helpers35", name, addr, fn)
HELPER35("Plane::load_v0", 0x005dde90, &H::load_v0);
HELPER35("Plane::load_v50", 0x005decf0, &H::load_v50);
#undef HELPER35

}  // namespace en3d_helpers35
}  // namespace

// ---------------------------------------- from modules/en3d_helpers34.cpp (en3d/helpers34)
//
//  * module en3d/helpers34:
//      Plane::load_v100 0x005e0510 (version 100 reader: a stub that returns 0),
//      Plane::load_v500 0x005e0530 (version 500 .SMO / .AM1 reader: header (material count n0,
//        texture count n1, LOD count n2, 16 bytes of which the 4th float is the LOD distance),
//        per material: strip length, vertex count, ambient / diffuse / specular / emissive
//        colours as 4 floats (A R G B -> 0..255 bytes), 16 unused bytes, texture index, the
//        triangle strip (u16 indices, degenerate triangles dropped, odd triangles flipped) and
//        32-byte vertices (normal, position, uv); then the n1 textures (20-byte name and an
//        optional embedded image of the given size, loaded with
//        D3DXCreateTextureFromFileInMemory), then the n2 LOD sub-planes (same strip / vertex
//        layout per material), then a list of tagged extra sections (1 lightmap / env
//        textures, 2 per-material flags, 3 vertex normals reset) ended by any other tag.
namespace {
namespace en3d_helpers34 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::stdcall_call;

JOAN_COUNTER(c_load500, "Plane::load_v500");

constexpr img::Addr kTextures(d::g_texture_array);  // TextureManager

// ilib File readers.
std::int32_t read_int(void* file) { return joan::orig::as_file_mem(file)->file_get_long(); }     // 0x005893c0
std::uint16_t read_u16(void* file) { return joan::orig::arg::as<std::uint16_t>(joan::orig::as_file_mem(file)->file_get_unsigned_short()); }  // 0x005892f0
std::int32_t read(void* file, void* buffer, std::int32_t size) {
    return joan::orig::as_file_mem(file)->file_read(joan::orig::arg::mptr(buffer), joan::orig::arg::u32(size));  // 0x00588a80 (zero-fills short reads)
}
// Message pump / progress, called in pairs while loading.
void pump() {
    joan::orig::as_sound_stream(img::obj(d::g_snd))->update();  // 0x0059b1c0
    joan::orig::as_ambient_streams(img::obj(d::g_mus))->update();  // 0x005866d0
}

// ilib Blob (8 bytes on x86: data pointer, size; the constructor 0x0057f770 writes both).
struct Blob {
    void* data;
    std::uint32_t size;
};
static_assert(offsetof(Blob, size) == sizeof(void*) && sizeof(Blob) >= sizeof(joan::orig::Blob), "Blob");

std::uint8_t* material(void* plane, std::int32_t i) { return jo::view<std::uint8_t*>(jo::as_plane(plane)->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material)); }

// float colour component -> byte (fld; fmul 255.0f; _ftol).
std::uint8_t channel(float v) {
    return static_cast<std::uint8_t>(joan::hook::x87::ftol(static_cast<double>(v * 255.0f)));
}

// The triangle strip of material `mat` (n indices after the first two), vertex base vbase.
void read_strip(void* plane, void* file, std::int32_t mat, std::int32_t n, std::int32_t vbase) {
    const std::int32_t start = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->reserve_strip(joan::orig::arg::i32(mat), joan::orig::arg::u32(n)));  // 0x005d6ee0
    auto index = [&](std::int32_t k) -> std::uint16_t& { return jo::view<std::uint16_t*>(jo::as_plane(plane)->strip_indices)[k]; };
    std::int32_t count = 0;
    std::int32_t pos = start;
    std::int32_t odd = 0;
    std::uint16_t a = read_u16(file);
    std::uint16_t b = read_u16(file);
    index(pos) = static_cast<std::uint16_t>(vbase + a);
    ++pos;
    index(pos) = static_cast<std::uint16_t>(vbase + b);
    ++pos;
    for (std::int32_t k = 0; k < n; ++k) {
        const std::uint16_t c = read_u16(file);
        index(pos) = static_cast<std::uint16_t>(vbase + c);
        ++pos;
        if (c != a && c != b && a != b) ++count;
        a = b;
        b = c;
        odd ^= 1;
    }
    const std::int32_t face = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_triangles(joan::orig::arg::i32(count)));  // 0x005ce6f0 add triangles
    count = 0;
    pos = start;
    odd = 0;
    a = index(pos);
    ++pos;
    b = index(pos);
    ++pos;
    for (std::int32_t k = 0; k < n; ++k) {
        const std::uint16_t c = index(pos);
        ++pos;
        if (c != a && c != b && a != b) {
            const std::int32_t ia = a, ib = b, ic = c;
            if (odd == 0)
                joan::orig::as_plane(plane)->set_triangle(joan::orig::arg::i32(face + count), joan::orig::arg::u32(mat), joan::orig::arg::u32(ia), joan::orig::arg::u32(ib), joan::orig::arg::u32(ic));  // 0x005ce800 set_triangle
            else
                joan::orig::as_plane(plane)->set_triangle(joan::orig::arg::i32(face + count), joan::orig::arg::u32(mat), joan::orig::arg::u32(ib), joan::orig::arg::u32(ia), joan::orig::arg::u32(ic));
            ++count;
        }
        a = b;
        b = c;
        odd ^= 1;
    }
}

// One 32-byte file vertex (normal, position, uv) into vertex slot k (0x24 bytes).
void store_vertex(void* plane, std::int32_t k, const std::uint32_t* in) {
    std::uint8_t* v = jo::view<std::uint8_t*>(jo::as_plane(plane)->vertices_f1) + k * 0x24;
    jo::view<std::uint32_t>(jo::as_vertex_f1(v)->pos[0]) = in[3];
    jo::view<std::uint32_t>(jo::as_vertex_f1(v)->pos[1]) = in[4];
    jo::view<std::uint32_t>(jo::as_vertex_f1(v)->pos[2]) = in[5];
    v = jo::view<std::uint8_t*>(jo::as_plane(plane)->vertices_f1) + k * 0x24;  // reloaded, as the original
    jo::view<std::uint32_t>(jo::as_vertex_f1(v)->attr[0]) = in[0];  // normal
    jo::view<std::uint32_t>(jo::as_vertex_f1(v)->attr[1]) = in[1];
    jo::view<std::uint32_t>(jo::as_vertex_f1(v)->attr[2]) = in[2];
    v = jo::view<std::uint8_t*>(jo::as_plane(plane)->vertices_f1) + k * 0x24;
    jo::view<std::uint32_t>(jo::as_vertex_f1(v)->attr[4]) = in[6];
    jo::view<std::uint32_t>(jo::as_vertex_f1(v)->attr[5]) = in[7];
    v = jo::view<std::uint8_t*>(jo::as_plane(plane)->vertices_f1) + k * 0x24;
    jo::view<std::uint32_t>(jo::as_vertex_f1(v)->attr[3]) = global<std::uint32_t>(d::g_init_white);
}

// Original stack layout of the texture name (ebp-0xf0, 20 bytes) followed by the loop
// counter (ebp-0xdc) and the texture table (ebp-0xd8): a name without a NUL in its 20 bytes
// runs into the counter as in the original.
struct NameSlot {
    char name[20];
    std::int32_t t;
    void** table;
};
static_assert(offsetof(NameSlot, t) == 20 && offsetof(NameSlot, table) == 24, "NameSlot");  // x86: 28 bytes

struct H {
    // 0x005e0510 Plane::load_v100(file, flag): not supported, returns 0.
    std::uint32_t load_v100(void* /*file*/, std::uint8_t /*flag*/) { return 0; }

    // 0x005e0530 Plane::load_v500(file, flag).
    std::uint32_t load_v500(void* file, std::uint8_t flag) {
        c_load500.hit();
        const std::int32_t n_mat = read_int(file);
        const std::int32_t n_tex = read_int(file);
        const std::int32_t n_lod = read_int(file);
        float header[4];
        if (read(file, header, 0x10) == 0) return 0;
        Blob tex_index;
        joan::orig::as_blob(&tex_index)->construct(static_cast<std::int32_t>(static_cast<std::uint32_t>(n_mat) << 2));  // 0x0057f770 Blob(size)
        std::int32_t* const tex_of = joan::orig::arg::as<std::int32_t*>(joan::orig::as_blob(&tex_index)->data_56de20());  // 0x0056de20 data
        auto fail = [&]() -> std::uint32_t {
            joan::orig::as_blob(&tex_index)->destruct();  // 0x0057f850 ~Blob
            return 0;
        };
        const std::int32_t base = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_materials(joan::orig::arg::i32(n_mat)));  // 0x005cd470 add_materials
        std::uint32_t vbuf[8];
        std::int32_t i;
        for (i = 0; i < n_mat; ++i) {
            void* const mat = material(this, base + i);
            joan::orig::as_material(mat)->construct();  // 0x005d52a0 Material::init
            const std::int32_t n_strip = read_int(file);
            const std::int32_t n_vert = read_int(file);
            float rgba[4];
            std::uint8_t col[4];
            auto colour = [&]() {
                col[2] = channel(rgba[1]);
                col[1] = channel(rgba[2]);
                col[0] = channel(rgba[3]);
                col[3] = channel(rgba[0]);
            };
            if (read(file, rgba, 0x10) == 0) return fail();
            colour();
            joan::orig::as_material(mat)->set_colour4(joan::orig::arg::mptr(static_cast<void*>(col)));  // 0x005d5570
            if (read(file, rgba, 0x10) == 0) return fail();
            colour();
            joan::orig::as_material(mat)->set_colour0(joan::orig::arg::mptr(static_cast<void*>(col)));  // 0x005d5540
            if (read(file, rgba, 0x10) == 0) return fail();
            colour();
            joan::orig::as_material(mat)->set_colour8(joan::orig::arg::mptr(static_cast<void*>(col)), 0u);  // 0x005d55a0
            if (read(file, rgba, 0x10) == 0) return fail();
            colour();
            joan::orig::as_material(mat)->set_colourc(joan::orig::arg::mptr(static_cast<void*>(col)));  // 0x005d55e0
            std::uint8_t unused[0x10];
            if (read(file, unused, 0x10) == 0) return fail();
            tex_of[i] = read_int(file);
            const std::int32_t vbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_vertices(joan::orig::arg::i32(n_vert)));  // 0x005cd7f0 add_vertices
            // (an `if (0)` triangle-list branch of the original is dead code)
            if (n_strip > 0) read_strip(this, file, base + i, n_strip, vbase);
            pump();
            for (std::int32_t k = 0; k < n_vert; ++k) {
                if (read(file, vbuf, 0x20) == 0) return fail();
                store_vertex(this, vbase + k, vbuf);
            }
            pump();
        }
        joan::orig::as_plane(this)->set_flag50(1u);  // 0x005d7270

        // Textures.
        NameSlot slot;
        slot.table = joan::orig::arg::as<void**>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<std::uint32_t>(n_tex) * static_cast<std::uint32_t>(sizeof(void*)))));  // operator new (texture pointers; n_tex << 2 on x86)
        for (slot.t = 0; slot.t < n_tex; ++slot.t) {
            if (read(file, slot.name, 0x14) == 0) return fail();
            const std::int32_t size = read_int(file);
            char* const dot = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(static_cast<char*>(slot.name)), 0x2e));  // strrchr
            if (dot != nullptr) *dot = 0;
            void* tex = joan::orig::arg::as<void*>(joan::orig::as_textures(img::obj(kTextures))->find(joan::orig::arg::str(static_cast<char*>(slot.name))));  // 0x0060a4b0 find
            if (tex != nullptr) {
                joan::orig::as_texture_manager(img::obj(kTextures))->use(joan::orig::arg::mptr(tex));  // 0x0060a500 add reference
                if (size > 0) joan::orig::as_file_mem(file)->file_seek(joan::orig::arg::i32(size), 1);  // 0x005888c0 seek (skip)
            } else if (size > 0) {
                Blob image;
                joan::orig::as_blob(&image)->construct(joan::orig::arg::i32(size));
                if (read(file, joan::orig::arg::as<void*>(joan::orig::as_blob(&image)->data_56de20()), size) != 0) {
                    void* d3d_tex = nullptr;  // written by D3DX on success
                    void* const data = joan::orig::arg::as<void*>(joan::orig::as_blob(&image)->data_56de20());
                    // D3DXCreateTextureFromFileInMemory(device, data, size, &tex)
                    if (joan::orig::arg::as<std::int32_t>(joan::orig::d3dx::create_texture_from_file_in_memory(joan::orig::arg::mptr(global<void*>(d::g_game_device)), joan::orig::arg::mptr(data), joan::orig::arg::u32(size), &d3d_tex)) == 0) {
                        void* const entry = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTextures))->create_2d());           // 0x006094c0 new entry
                        joan::orig::as_texture2d(entry)->attach(joan::orig::arg::mptr(d3d_tex), 1u);                             // 0x00605f00 set texture
                        joan::orig::as_texture(entry)->set_name(joan::orig::arg::str(static_cast<char*>(slot.name)));          // 0x00605ad0 set name
                        tex = entry;
                        void* const* const vtbl = joan::call::vtable_of(d3d_tex);
                        reinterpret_cast<std::uint32_t(JOAN_STDCALL*)(void*)>(vtbl[2])(d3d_tex);  // Release
                    }
                }
                pump();
                joan::orig::as_blob(&image)->destruct();
            } else {
                tex = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(static_cast<char*>(slot.name)), 1, joan::orig::arg::u32(static_cast<std::uint32_t>(flag))));  // 0x006099c0 load
                pump();
            }
            for (i = 0; i < n_mat; ++i) {
                if (tex_of[i] == slot.t) joan::orig::as_material(material(this, base + i))->set_texture(0, joan::orig::arg::mptr(tex));  // 0x005d5650 set_texture
            }
            slot.table[slot.t] = tex;
        }
        joan::orig::as_plane(this)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        joan::orig::as_plane(this)->sort_by_material(0);   // 0x005cf1d0

        // LOD sub-planes.
        for (std::int32_t l = 0; l < n_lod; ++l) {
            void* const sub = joan::orig::arg::as<void*>(joan::orig::as_plane(this)->add_child(joan::orig::arg::i32(l + 1), joan::orig::arg::mptr(0)));  // 0x005d4470
            joan::orig::as_plane(sub)->init(0, 0, 0, 0u);                   // 0x005cc7a0 Plane::init
            joan::orig::as_plane(sub)->share_bones(joan::orig::arg::mptr(static_cast<void*>(this)));            // 0x005d4a00
            for (i = 0; i < n_mat; ++i) {
                const std::int32_t n_strip = read_int(file);
                const std::int32_t n_vert = read_int(file);
                const std::int32_t vbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(sub)->add_vertices(joan::orig::arg::i32(n_vert)));  // 0x005cd7f0
                // (dead `if (0)` triangle-list branch)
                if (n_strip > 0) read_strip(sub, file, base + i, n_strip, vbase);
                for (std::int32_t k = 0; k < n_vert; ++k) {
                    if (read(file, vbuf, 0x20) == 0) return fail();
                    store_vertex(sub, vbase + k, vbuf);
                }
            }
            joan::orig::as_plane(sub)->set_flag50(1u);   // 0x005d7270
            joan::orig::as_plane(sub)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
            joan::orig::as_plane(sub)->sort_by_material(0);   // 0x005cf1d0
        }
        if (n_lod > 0) joan::orig::as_plane(this)->set_lod(joan::orig::arg::u32(n_lod), joan::orig::arg::bits(2.0f * header[3]));  // 0x005d4720

        // Tagged extra sections.
        std::uint8_t two_sided = 0;  // always written by the (zero-filling) reader before use
        for (;;) {
            const std::int32_t tag = read_int(file);
            if (tag == 1) {
                for (i = 0; i < n_mat; ++i) {
                    const std::int32_t a = read_int(file);
                    const std::int32_t b = read_int(file);
                    if (a != -1) {
                        void* const t24 = (jo::as_material(material(this, i)))->textures[0];
                        bool one = false;
                        if (t24 != nullptr) {
                            void* const o = (jo::as_material(material(this, i)))->textures[0];
                            void* const* const vtbl = joan::call::vtable_of((jo::as_material(material(this, i)))->textures[0]);
                            one = reinterpret_cast<std::int32_t(JOAN_THISCALL*)(void*)>(vtbl[3])(o) == 1;
                        }
                        jo::view<std::int32_t>((jo::as_material(material(this, i)))->pass_mask) = one ? 1 : 2;
                        jo::view<std::int32_t>((jo::as_material(material(this, i)))->blend) = 1;
                        jo::view<std::int32_t>((jo::as_material(material(this, i)))->texture_mode) = 0x20000;
                    }
                    if (b == -1) continue;
                    if (global<std::int32_t>(d::g_a52627) != 0) {
                        joan::orig::as_material(material(this, i))->set_texture(2, joan::orig::arg::mptr((jo::as_material(material(this, i)))->textures[0]));
                        joan::orig::as_material(material(this, i))->set_texture(1, joan::orig::arg::mptr((jo::as_material(material(this, i)))->textures[0]));
                        joan::orig::as_material(material(this, i))->set_texture(0, joan::orig::arg::mptr(slot.table[b]));
                        if (a == -1) {
                            jo::view<std::int32_t>((jo::as_material(material(this, i)))->blend) = 8;
                            jo::view<std::int32_t>((jo::as_material(material(this, i)))->texture_mode) = 9;
                        } else {
                            jo::view<std::int32_t>((jo::as_material(material(this, i)))->blend) = 9;
                            jo::view<std::int32_t>((jo::as_material(material(this, i)))->texture_mode) = 9;
                        }
                        joan::orig::as_plane(this)->set_format(5);  // 0x005cdc70
                        joan::orig::as_plane(this)->set_f8(1u);  // 0x005d5ee0
                    } else if (global<std::int32_t>(d::g_a5262f) != 0) {
                        joan::orig::as_material(material(this, i))->set_texture(1, joan::orig::arg::mptr((jo::as_material(material(this, i)))->textures[0]));
                        joan::orig::as_material(material(this, i))->set_texture(0, joan::orig::arg::mptr((jo::as_material(material(this, i)))->textures[0]));
                        jo::view<std::int32_t>((jo::as_material(material(this, i)))->blend) = 0xa;
                        jo::view<std::int32_t>((jo::as_material(material(this, i)))->texture_mode) = 0xb;
                    }
                }
            } else if (tag == 2) {
                for (i = 0; i < n_mat; ++i) {
                    const std::int32_t a = read_int(file);
                    read_int(file);
                    read(file, &two_sided, 1);
                    if (a != -1) {
                        (jo::as_material(material(this, i)))->power = 0x42480000u;  // 50.0f
                        if (jo::view<std::int32_t>((jo::as_material(material(this, i)))->blend) == 0) jo::view<std::int32_t>((jo::as_material(material(this, i)))->blend) = 0x14;
                    } else {
                        joan::orig::as_material(material(this, i))->set_colour8(joan::orig::arg::mptr(img::obj(d::g_init_black)), 0u);  // 0x005d55a0
                    }
                    if (two_sided != 0) {
                        const std::uint32_t v = (jo::as_material(material(this, i)))->raster;
                        (jo::as_material(material(this, i)))->raster = (v & 0xfffffff0u) | 2u;
                    }
                }
            } else if (tag == 3) {
                const std::int32_t reset = read_int(file);
                if (reset != 0 && jo::view<void*>(jo::self<jo::Plane>(this)->vertices_f1) != nullptr) {
                    for (std::int32_t k = 0; k < jo::self<jo::Plane>(this)->vertex_count; ++k) {
                        jo::VertexF1* const n = jo::self<jo::Plane>(this)->vertices_f1 + k;  // the normal
                        jo::view<std::uint32_t>(n->attr[0]) = global<std::uint32_t>(d::g_a18a60);
                        jo::view<std::uint32_t>(n->attr[1]) = global<std::uint32_t>(d::g_a18a64);
                        jo::view<std::uint32_t>(n->attr[2]) = global<std::uint32_t>(d::g_a18a68);
                    }
                }
            } else {
                break;
            }
        }
        pump();
        for (std::int32_t m = 0; m < jo::self<jo::Plane>(this)->material_count; ++m) {
            joan::orig::as_material(material(this, m))->set_colour4(joan::orig::arg::mptr(img::obj(d::g_init_white)));  // 0x005d5570
        }
        for (slot.t = 0; slot.t < n_tex; ++slot.t) {
            joan::orig::as_texture_manager(img::obj(kTextures))->release(joan::orig::arg::mptr(slot.table[slot.t]));  // 0x0060a550 release
        }
        joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(slot.table)));  // operator delete
        joan::orig::as_blob(&tex_index)->destruct();
        return 1;
    }
};

#define HELPER34(name, addr, fn) JOAN_REPLACE("en3d/helpers34", name, addr, fn)
HELPER34("Plane::load_v100", 0x005e0510, &H::load_v100);
HELPER34("Plane::load_v500", 0x005e0530, &H::load_v500);
#undef HELPER34

}  // namespace en3d_helpers34
}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
//
//      0x005e26c0 Mesh::read(reader, flag) (ret 8)  - PMAT / PVTX / PTRI / PSTP / PEX1 / PEX2 /
//                 PEND chunk reader, 0 on a bad tag or short read
namespace {
namespace en3d_b83_03_2 {

using joan::call::thiscall;

JOAN_COUNTER(c_mesh_read, "Mesh::read");
#if UINTPTR_MAX == 0xffffffffu  // the strides the original used (bones + i * 0xa0, materials + i * 0x54)
static_assert(sizeof(jo::Bone) == 0xa0 && offsetof(jo::Bone, local) == 0);
static_assert(sizeof(jo::Material) == 0x54);
#endif

struct H {
    // 0x005e26c0
    std::int32_t mesh_read(void* reader, std::uint8_t flag) {
        c_mesh_read.hit();
        auto read_int_r = [&]() { return joan::orig::as_file_mem(reader)->file_get_long(); };   // 0x005893c0
        auto read_block = [&](void* p, std::int32_t n, std::int32_t size) {
            return joan::orig::as_file_mem(reader)->file_read_array(joan::orig::arg::mptr(p), joan::orig::arg::u32(n), joan::orig::arg::u32(size));  // 0x00588c60
        };
        const std::int32_t first = read_int_r();
        const std::int32_t materials = read_int_r();
        joan::orig::as_plane(this)->init(0, 0, 0, joan::orig::arg::u32(materials));  // 0x005cc7a0
        for (std::int32_t i = 0; i < materials; ++i)
            if (joan::orig::as_file_mem(reader)->file_read(static_cast<void*>(jo::self<jo::Mesh>(this)->bones[i].local), 0x40u) == 0)  // 0x00588a80 (16 floats; Bone 0xa0 on x86)
                return 0;
        if (read_int_r() != 0x54414d50) return 0;  // PMAT
        const std::int32_t n_mat = read_int_r();
        const std::int32_t mat_base = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_materials(joan::orig::arg::i32(n_mat)));  // 0x005cd470
        for (std::int32_t i = 0; i < n_mat; ++i)
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_material(&jo::self<jo::Mesh>(this)->materials[mat_base + i])->read(joan::orig::arg::mptr(reader), joan::orig::arg::u32(flag))) == 0)  // 0x005e2b70 (Material 0x54 on x86)
                return 0;
        if (read_int_r() != 0x58545650) return 0;  // PVTX
        joan::orig::as_plane(this)->set_format(joan::orig::arg::i32(static_cast<std::uint8_t>(first)));  // 0x005cdc70
        jo::view<std::int32_t>(jo::self<jo::Mesh>(this)->transformed) = read_int_r();
        const std::int32_t n_vtx = read_int_r();
        const std::int32_t vtx_base = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_vertices(joan::orig::arg::i32(n_vtx)));  // 0x005cd7f0
        std::int32_t stride;
        std::uint8_t* const vertices = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(this)->buffer(joan::orig::arg::mptr(static_cast<void*>(&stride))));  // 0x005cdae0
        if (read_block(vertices + vtx_base * stride, n_vtx, stride) == 0) return 0;
        if (read_int_r() != 0x49525450) return 0;  // PTRI
        jo::self<jo::Mesh>(this)->prim_type = static_cast<std::uint8_t>(read_int_r());
        const std::int32_t n_tri = read_int_r();
        const std::int32_t tri_base = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_triangles(joan::orig::arg::i32(n_tri)));  // 0x005ce6f0
        if (read_block(jo::view<std::uint8_t*>(jo::self<jo::Mesh>(this)->triangles) + tri_base * 3 * 2, n_tri, 6) == 0) return 0;
        if (read_block(jo::view<std::uint8_t*>(jo::self<jo::Mesh>(this)->triangle_materials) + tri_base * 2, n_tri, 2) == 0) return 0;
        if (read_int_r() != 0x50545350) return 0;  // PSTP
        if (read_int_r() != 0) {
            read_int_r();  // [ebp-0x3c], unused
            for (std::int32_t i = 0; i < jo::self<jo::Mesh>(this)->material_count; ++i) {
                const std::int32_t v = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_file_mem(reader)->file_get_short_589220()));  // 0x00589220
                joan::orig::as_plane(this)->reserve_strip(joan::orig::arg::i32(i), joan::orig::arg::u32(v));  // 0x005d6ee0
            }
            const std::int32_t n = read_int_r();
            if (read_block(jo::view<void*>(jo::self<jo::Mesh>(this)->strip_indices), n, 2) == 0) return 0;
            joan::orig::as_plane(this)->set_flag50(joan::orig::arg::u32(read_int_r()));  // 0x005d7270
        }
        if (read_int_r() != 0x31584550) return 0;  // PEX1
        read_int_r();
        if (read_int_r() != 0x32584550) return 0;  // PEX2
        read_int_r();
        if (read_int_r() != 0x444e4550) return 0;  // PEND
        joan::orig::as_plane(this)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        return 1;
    }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("Plane::read_file", 0x005e26c0, &H::mesh_read);
#undef B83_03

}  // namespace en3d_b83_03_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
//
//  en3d leaf helpers called by the C++ draw path (batch 1): Material::set_texture.
namespace {
namespace en3d_helpers_2 {

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Material::set_texture", 0x005d5650, &joan::orig::Material::set_texture);
#undef HELPER

}  // namespace en3d_helpers_2
}  // namespace

// Out-of-line definition (declared in original/en3d.hpp).
void joan::orig::Material::set_texture(std::int32_t stage, void* texture) {  // 0x005d5650
    void* const old = textures[stage];
    textures[stage] = texture;
    if (texture) joan::orig::material::texture_ref(joan::orig::arg::mptr(texture));  // 0x005d56c0 add reference
    if (old) joan::orig::texture_ref::release(joan::orig::arg::mptr(old));          // release
}

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
namespace {
namespace en3d_b83_03_3 {


using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::orig::helpers::memset_;

struct H {
    // 0x005cbb60 (eax = this)
    void* model_ctor(void* src, std::uint32_t /*unused*/) {
        memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
        jo::view<std::uint8_t>(jo::self<jo::Plane>(this)->vertex_format) = jo::view<std::uint8_t>(jo::as_plane(src)->vertex_format);
        jo::self<jo::Plane>(this)->buffer_mode = jo::as_plane(src)->buffer_mode;
        jo::self<jo::Plane>(this)->skip_draw = 0xff;
        jo::self<jo::Plane>(this)->normals_pending_ = jo::as_plane(src)->normals_pending_;
        joan::orig::as_device_resource(&jo::self<jo::Plane>(this)->resource)->set_callback(0u, 0u, 0u);  // 0x005e9010
        return this;
    }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("Plane::Plane_5cbb60", 0x005cbb60, &H::model_ctor);
#undef B83_03

}  // namespace en3d_b83_03_3
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04_2.cpp (en3d/b85_04)
namespace {
namespace en3d_b85_04_2 {


JOAN_COUNTER(c_mat_write, "Material::write");

struct H {
    // 0x005e2a90 (this = material, file = writer)
    std::int32_t material_write(void* file) {
        c_mat_write.hit();
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S6): save::put_Material (textures as 0-iff-null
        // tokens on x64; Material::read clears them); 0 = the raw write.
        std::int32_t ok;
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kMaterial, this);
            joan::save::put_Material(o, *jo::self<jo::Material>(this));
            ok = o.end();  // 0x005872e0
        } else {
            ok = joan::orig::as_file(file)->file_write(static_cast<void*>(this), 0x54u);  // 0x005872e0
        }
        if (ok == 0) return 0;
        for (std::int32_t i = 0; i < 4; ++i) {
            std::uint8_t* const t = jo::view<std::uint8_t*>(jo::self<jo::Material>(this)->textures[i]);
            if (t == nullptr || static_cast<std::int8_t>(jo::as_texture(t)->name[0]) == 0) {  // Texture +0x18 on x86
                joan::orig::as_file(file)->file_put_long(0);               // 0x00587da0
            } else {
                joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(jo::as_texture(t)->usage));  // 0x00587da0
                if (joan::orig::as_file(file)->file_write(static_cast<void*>(jo::as_texture(t)->name), 0x21u) == 0) return 0;  // 0x005872e0
            }
        }
        return 1;
    }

    // 0x005dc400
    std::int32_t zero3(void*, void*, std::uint32_t) { return 0; }

};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("Material::write", 0x005e2a90, &H::material_write);
E3D("zero_5dc400", 0x005dc400, &H::zero3);
#undef E3D

}  // namespace en3d_b85_04_2
}  // namespace

// ---------------------------------------- from modules/game_b107_04.cpp (game/b107_04)
namespace {
namespace game_b107_04 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // 0x005dde60
    u32 forward_5dde60(u32 arg) { return joan::orig::arg::as<u32>(joan::orig::as_mesh(this)->save(joan::orig::arg::mptr(arg))); }  // 0x005df6a0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_04", name, addr, fn)
B("Obj::forward_5dde60", 0x005dde60, &H::forward_5dde60);
#undef B

}  // namespace game_b107_04
}  // namespace

// ---------------------------------------- from modules/game_b106_19.cpp (game/b106_19)
namespace {
namespace game_b106_19 {


using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

using joan::call::cdecl_call;
using joan::call::import_call;
using joan::img::obj;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;

using joan::orig::helpers::cf;

JOAN_COUNTER(c_mesh_save, "Mesh::save");
// One exported vertex (0x20 bytes, raw dwords): normal, position, uv.
struct ExportVertex {
    u32 normal[3];
    u32 pos[3];
    u32 uv[2];
};
// Byte b of a material colour (B, G, R, A).
u8 colour_byte(const std::uint32_t& colour, i32 b) { return reinterpret_cast<const u8*>(&colour)[b]; }

struct H {
    // ------------------------------------------------------------------------ 0x005df6a0
    struct Blob {
        void* p;
        u32 n;
    };
    static void blob_ctor(Blob* b, u32 n) { joan::orig::as_blob(b)->construct(joan::orig::arg::i32(n)); }  // 0x0057f770
    static void blob_dtor(Blob* b) { joan::orig::as_blob(b)->destruct(); }                // 0x0057f850
    static u8* blob_data(Blob* b) { return joan::orig::arg::as<u8*>(joan::orig::as_blob(b)->data_56de20()); }             // 0x0056de20
    static i32 w32(void* s, u32 v) { return joan::orig::as_file(s)->file_put_long(joan::orig::arg::i32(v)); }  // 0x00587da0
    static i32 wbytes(void* s, const void* p, u32 n) { return joan::orig::as_file(s)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }  // 0x005872e0

    i32 mesh_save(void* s) {
        c_mesh_save.hit();
        if (jo::self<jo::Mesh>(this)->vertices_f1 == nullptr) return 0;
        joan::orig::as_plane(this)->upload();  // 0x005cfa10
        if (w32(s, 0x32) == 0) return 0;
        Blob mats;  // [ebp-0x18]
        blob_ctor(&mats, jo::view<u32>(jo::self<jo::Mesh>(this)->material_count) << 2);
        i32 nmats = 0;
        for (i32 i = 0; i < jo::self<jo::Mesh>(this)->triangle_count; ++i) {
            const u32 m = jo::view<u16*>(jo::self<jo::Mesh>(this)->triangle_materials)[i];
            i32 j = 0;
            for (; j < nmats; ++j)
                if (reinterpret_cast<u32*>(blob_data(&mats))[j] == m) break;
            if (j < nmats) continue;
            reinterpret_cast<u32*>(blob_data(&mats))[nmats] = m;
            ++nmats;
        }
        if (w32(s, static_cast<u32>(nmats)) == 0) {
            blob_dtor(&mats);
            return 0;
        }
        Blob texs;  // [ebp-0x30]
        // texture pointers, sizeof(void*) each (nmats << 2 bytes on x86)
        blob_ctor(&texs, static_cast<u32>(nmats) * static_cast<u32>(sizeof(void*)));
        i32 ntex = 0;
        for (i32 i = 0; i < nmats; ++i) {
            const u32 m = reinterpret_cast<u32*>(blob_data(&mats))[i];
            void* const t = jo::self<jo::Mesh>(this)->materials[m].textures[0];
            if (t == nullptr) continue;
            i32 j = 0;
            for (; j < ntex; ++j)
                if (reinterpret_cast<void**>(blob_data(&texs))[j] == t) break;
            if (j < ntex) continue;
            reinterpret_cast<void**>(blob_data(&texs))[ntex] = t;
            ++ntex;
        }
        if (w32(s, static_cast<u32>(ntex)) == 0 || w32(s, 0) == 0) {
            blob_dtor(&texs);
            blob_dtor(&mats);
            return 0;
        }
        void* xf = nullptr;  // [ebp-0x48]
        if (jo::view<i32>(jo::self<jo::Mesh>(this)->transformed) == 0 && joan::orig::arg::as<void*>(joan::orig::as_plane(this)->bone_5cf8d0()) != nullptr &&                       // 0x005cf8d0
            joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(this)->bone_5cf8d0()))->matrix()) != nullptr) {              // 0x0060ef80
            void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(this)->bone_5cf8d0()))->matrix());
            if (joan::orig::arg::as<i32>(joan::orig::libc::memcmp(joan::orig::arg::mptr(m), joan::orig::arg::mptr(obj(d::g_identity_matrix)), joan::orig::arg::u32(u32{0x40}))) != 0)  // memcmp
                xf = joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(this)->bone_5cf8d0()))->matrix());
        }
        for (i32 mi = 0; mi < nmats; ++mi) {
            const u32 mat_index = reinterpret_cast<u32*>(blob_data(&mats))[mi];
            u8* const mat = reinterpret_cast<u8*>(&jo::self<jo::Mesh>(this)->materials[mat_index]);  // colour bytes +0..+0xa
            Blob faces, verts, vmap;  // [ebp-0x5c], [ebp-0x64], [ebp-0x6c]
            blob_ctor(&faces, static_cast<u32>(jo::self<jo::Mesh>(this)->triangle_count * 6));
            blob_ctor(&verts, jo::view<u32>(jo::self<jo::Mesh>(this)->vertex_count) << 5);
            blob_ctor(&vmap, jo::view<u32>(jo::self<jo::Mesh>(this)->vertex_count) << 2);
            i32 nfaces = 0, nverts = 0;
            for (i32 fi = 0; fi < jo::self<jo::Mesh>(this)->triangle_count; ++fi) {
                if (static_cast<u32>(jo::view<u16*>(jo::self<jo::Mesh>(this)->triangle_materials)[fi]) != mat_index) continue;
                for (i32 k = 0; k < 3; ++k) {
                    const u32 vi = jo::view<u16*>(jo::self<jo::Mesh>(this)->triangles)[fi * 3 + k];
                    i32 j = 0;
                    for (; j < nverts; ++j)
                        if (reinterpret_cast<u32*>(blob_data(&vmap))[j] == vi) break;
                    if (!(j < nverts)) {
                        u8* const e = blob_data(&verts) + nverts * 32;
                        ExportVertex* const ev = reinterpret_cast<ExportVertex*>(e);
                        {
                            const u8* const src = jo::view<u8*>(jo::self<jo::Mesh>(this)->vertices_f1) + vi * 0x24u;
                            ev->pos[0] = jo::view<u32>(jo::as_vertex_f1(src)->pos[0]);
                            ev->pos[1] = jo::view<u32>(jo::as_vertex_f1(src)->pos[1]);
                            ev->pos[2] = jo::view<u32>(jo::as_vertex_f1(src)->pos[2]);
                        }
                        {
                            const u8* const src = jo::view<u8*>(jo::self<jo::Mesh>(this)->vertices_f1) + vi * 0x24u;
                            ev->normal[0] = jo::view<u32>(jo::as_vertex_f1(src)->attr[0]);
                            ev->normal[1] = jo::view<u32>(jo::as_vertex_f1(src)->attr[1]);
                            ev->normal[2] = jo::view<u32>(jo::as_vertex_f1(src)->attr[2]);
                        }
                        {
                            const u8* const src = jo::view<u8*>(jo::self<jo::Mesh>(this)->vertices_f1) + vi * 0x24u;
                            const u32 a = jo::view<u32>(jo::as_vertex_f1(src)->attr[4]);
                            const u32 b = jo::view<u32>(jo::as_vertex_f1(src)->attr[5]);
                            ev->uv[0] = a;
                            ev->uv[1] = b;
                        }
                        if (xf != nullptr) {
                            import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(ev->pos), xf);  // point_transform
                            import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(ev->normal), xf);        // vector_transform
                        }
                        reinterpret_cast<u32*>(blob_data(&vmap))[nverts] = vi;
                        ++nverts;
                    }
                    reinterpret_cast<u16*>(blob_data(&faces))[nfaces * 3 + k] = static_cast<u16>(j);
                }
                ++nfaces;
            }
            bool ok = w32(s, static_cast<u32>(nfaces)) != 0 && w32(s, static_cast<u32>(nverts)) != 0;
            const float k255 = cf(d::flt_255);
            float col[3];
            if (ok) {
                col[0] = static_cast<float>(colour_byte(jo::as_material(mat)->ambient, 2)) / k255;
                col[1] = static_cast<float>(colour_byte(jo::as_material(mat)->ambient, 1)) / k255;
                col[2] = static_cast<float>(colour_byte(jo::as_material(mat)->ambient, 0)) / k255;
                ok = wbytes(s, col, 0xc) != 0;
            }
            if (ok) {
                col[0] = static_cast<float>(colour_byte(jo::as_material(mat)->diffuse, 2)) / k255;
                col[1] = static_cast<float>(colour_byte(jo::as_material(mat)->diffuse, 1)) / k255;
                col[2] = static_cast<float>(colour_byte(jo::as_material(mat)->diffuse, 0)) / k255;
                ok = wbytes(s, col, 0xc) != 0;
            }
            if (ok) {
                col[0] = static_cast<float>(colour_byte(jo::as_material(mat)->specular, 2)) / k255;
                col[1] = static_cast<float>(colour_byte(jo::as_material(mat)->specular, 1)) / k255;
                col[2] = static_cast<float>(colour_byte(jo::as_material(mat)->specular, 0)) / k255;
                ok = wbytes(s, col, 0xc) != 0;
            }
            if (ok) {
                i32 j = 0;
                for (; j < ntex; ++j)
                    if (reinterpret_cast<void**>(blob_data(&texs))[j] == jo::as_material(mat)->textures[0]) break;
                const u32 tex = jo::view<i32>(jo::as_material(mat)->pass_mask) == 1 ? (static_cast<u32>(j) | 0x80000000u) : static_cast<u32>(j);
                ok = w32(s, tex) != 0;
            }
            for (i32 a = 0; ok && a < nfaces; ++a) ok = wbytes(s, blob_data(&faces) + a * 6, 6) != 0;
            for (i32 a = 0; ok && a < nverts; ++a) ok = wbytes(s, blob_data(&verts) + a * 32, 0x20) != 0;
            blob_dtor(&vmap);
            blob_dtor(&verts);
            blob_dtor(&faces);
            if (!ok) {
                blob_dtor(&texs);
                blob_dtor(&mats);
                return 0;
            }
        }
        for (i32 i = 0; i < ntex; ++i) {
            u8* const t = reinterpret_cast<u8**>(blob_data(&texs))[i];
            if (wbytes(s, jo::as_texture(t)->name, 0x14) == 0 || w32(s, 0) == 0) {  // Texture +0x18 on x86
                blob_dtor(&texs);
                blob_dtor(&mats);
                return 0;
            }
        }
        blob_dtor(&texs);
        blob_dtor(&mats);
        return 1;
    }
};

#define B106(name, addr, fn) JOAN_REPLACE("game/b106_19", name, addr, fn)
B106("Mesh::save", 0x005df6a0, &H::mesh_save);
#undef B106

}  // namespace game_b106_19
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01_3.cpp (en3d/b84_01)
namespace {
namespace en3d_b84_01_3 {


using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_mesh_write, "Mesh::write");

struct H {
    // 0x005e2380
    std::int32_t mesh_write(void* file) {
        c_mesh_write.hit();
        auto put = [file](std::uint32_t v) { joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(v)); };  // 0x00587da0
        auto put_array = [file](const void* p, std::uint32_t n, std::uint32_t size) {
            return joan::orig::as_file(file)->file_write_array(joan::orig::arg::ptr(p), joan::orig::arg::u32(n), joan::orig::arg::u32(size));  // 0x00587510
        };
        put(static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::Mesh>(this)->vertex_format)));
        put(static_cast<std::uint32_t>(jo::self<jo::Mesh>(this)->bone_count));
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::Mesh>(this)->bone_count); ++i) {
            if (joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(jo::self<jo::Mesh>(this)->bones[i].local), 0x40u) == 0)  // 0x005872e0 (16 floats; Bone 0xa0 on x86)
                return 0;
        }
        put(0x54414d50u);  // "PMAT"
        put(jo::view<std::uint32_t>(jo::self<jo::Mesh>(this)->material_count));
        for (std::int32_t i = 0; i < jo::self<jo::Mesh>(this)->material_count; ++i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_material(&jo::self<jo::Mesh>(this)->materials[i])->write(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x005e2a90 (Material 0x54 on x86)
        }
        put(0x58545650u);  // "PVTX"
        put(jo::self<jo::Mesh>(this)->transformed);
        put(jo::view<std::uint32_t>(jo::self<jo::Mesh>(this)->vertex_count));
        std::uint32_t stride = 0;
        const void* const vertices = joan::orig::arg::as<const void*>(joan::orig::as_plane(this)->buffer(&stride));  // 0x005cdae0
        if (put_array(vertices, jo::view<std::uint32_t>(jo::self<jo::Mesh>(this)->vertex_count), stride) == 0) return 0;
        put(0x49525450u);  // "PTRI"
        put(static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Mesh>(this)->prim_type))));
        put(jo::view<std::uint32_t>(jo::self<jo::Mesh>(this)->triangle_count));
        if (put_array(jo::self<jo::Mesh>(this)->triangles, jo::view<std::uint32_t>(jo::self<jo::Mesh>(this)->triangle_count), 6) == 0) return 0;
        if (put_array(jo::self<jo::Mesh>(this)->triangle_materials, jo::view<std::uint32_t>(jo::self<jo::Mesh>(this)->triangle_count), 2) == 0) return 0;
        put(0x50545350u);  // "PSTP"
        put(static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Mesh>(this)->has_strips))));
        if (static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Mesh>(this)->has_strips)) != 0) {
            put(static_cast<std::uint32_t>(jo::self<jo::Mesh>(this)->strip_total));
            for (std::int32_t i = 0; i < jo::self<jo::Mesh>(this)->material_count; ++i)
                joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(jo::view<std::uint16_t*>(jo::self<jo::Mesh>(this)->strip_lengths)[i]));  // 0x00587ba0
            put(jo::view<std::uint32_t>(jo::self<jo::Mesh>(this)->strip_index_count));
            if (put_array(jo::self<jo::Mesh>(this)->strip_indices, jo::view<std::uint32_t>(jo::self<jo::Mesh>(this)->strip_index_count), 2) == 0) return 0;
            put(static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Mesh>(this)->use_strips))));
        }
        put(0x31584550u);  // "PEX1"
        put(0);
        put(0x32584550u);  // "PEX2"
        put(0);
        put(0x444e4550u);  // "PEND"
        return 1;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("Plane::write_file", 0x005e2380, &H::mesh_write);
#undef E3D

}  // namespace en3d_b84_01_3
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01_4.cpp (en3d/b84_01)
namespace {
namespace en3d_b84_01_4 {

using u32 = std::uint32_t;

using joan::img::obj;
using joan::img::str;

void* const kTextures = obj(d::g_texture_array);   // texture manager

void game_memset(void* p, std::int32_t v, std::uint32_t n) { std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

JOAN_COUNTER(c_mat_read, "Material::read");

struct H {
    // 0x005e2b70
    std::int32_t material_read(void* reader, std::uint32_t flag) {
        c_mat_read.hit();
        // [Debug] SaveFieldwise=1 (default, S6): save::get_Material; 0 = the raw read.
        std::int32_t ok;
        if (joan::save::fieldwise()) {
            joan::save::In in(joan::save::port(*joan::orig::as_file_mem(reader)), joan::save::kMaterial, this);  // 0x00588a80
            joan::save::get_Material(in, *jo::self<jo::Material>(this));
            ok = in.end();
        } else {
            ok = joan::orig::as_file_mem(reader)->file_read(static_cast<void*>(this), 0x54u);  // 0x00588a80
        }
        if (ok == 0) return 0;
        game_memset(jo::self<jo::Material>(this)->textures, 0, static_cast<std::uint32_t>(sizeof(jo::Material::textures)));  // 0x10 on x86
        for (std::int32_t i = 0; i < 4; ++i) {
            const std::int32_t n = joan::orig::as_file_mem(reader)->file_get_long();  // 0x005893c0
            if (n == 0) continue;
            char name[0x24];
            if (joan::orig::as_file_mem(reader)->file_read(static_cast<void*>(name), 0x21u) == 0) return 0;                         // 0x00588a80
            void* const texture = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(kTextures)->load(joan::orig::arg::str(static_cast<char*>(name)), joan::orig::arg::i32(n), joan::orig::arg::u32(static_cast<std::uint8_t>(flag))));  // 0x006099c0
            joan::orig::as_material(this)->set_texture(joan::orig::arg::i32(i), joan::orig::arg::mptr(texture));   // 0x005d5650
            joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(texture));  // 0x0060a550
        }
        game_memset(jo::self<jo::Material>(this)->runtime, 0, static_cast<std::uint32_t>(sizeof(jo::Material::runtime)));  // 0x10 on x86
        return 1;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("Material::read", 0x005e2b70, &H::material_read);
#undef E3D

}  // namespace en3d_b84_01_4
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Plane::construct(std::uint32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16_2::H::plane_ctor>(this, a0, a1)); }  // 0x5cbaf0
auto joan::orig::Model::Model_5cbb60(void* a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_03_3::H::model_ctor>(this, a0, a1)); }  // 0x5cbb60
auto joan::orig::Plane::Plane_copy(void* a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers36::H::plane_copy>(this, a0, a1)); }  // 0x5cbbf0
auto joan::orig::Plane::dtor() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers32::H::plane_dtor>(this)); }  // 0x5cc750
auto joan::orig::Plane::init(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::uint32_t a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers18::H::plane_init>(this, a0, a1, a2, a3)); }  // 0x5cc7a0
auto joan::orig::Bone::vector_delete(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers25::H::bone_array_delete>(this, a0)); }  // 0x5cca00
auto joan::orig::Plane::release() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers22::H::plane_release>(this)); }  // 0x5ccaa0
auto joan::orig::Plane::clear() -> void { joan::bind::member<&::en3d_helpers16::H::plane_clear>(this); }  // 0x5cd2d0
auto joan::orig::Plane::add_materials(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers18::H::add_materials>(this, a0)); }  // 0x5cd470
auto joan::orig::Plane::add_vertices(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers18::H::add_vertices>(this, a0)); }  // 0x5cd7f0
auto joan::orig::Plane::buffer(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers8::H::plane_buffer>(this, a0)); }  // 0x5cdae0
auto joan::orig::Plane::set_format(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers36::H::set_format>(this, a0)); }  // 0x5cdc70
auto joan::orig::Plane::add_triangles(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers16::H::plane_add_triangles>(this, a0)); }  // 0x5ce6f0
auto joan::orig::Plane::set_triangle(std::int32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> void { joan::bind::member<&::en3d_helpers16::H::plane_set_triangle>(this, a0, a1, a2, a3, a4); }  // 0x5ce800
auto joan::orig::Plane::set(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> void { joan::bind::member<&::en3d_b84_01::H::plane_set>(this, a0, a1, a2, a3); }  // 0x5ce8a0
auto joan::orig::Plane::set3(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b85_04::H::plane_set3>(this, a0, a1, a2)); }  // 0x5ce900
auto joan::orig::Plane::in_pass(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers6::H::plane_in_pass>(this, a0)); }  // 0x5ce970
auto joan::orig::Plane::merge(void* a0, std::int32_t a1, std::int32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_15::H::merge>(this, a0, a1, a2)); }  // 0x5cea30
auto joan::orig::Plane::copy_extras(void* a0) -> void { joan::bind::member<&::en3d_helpers16::H::plane_copy_extras>(this, a0); }  // 0x5cf050
auto joan::orig::Plane::sort_by_material(std::int32_t a0) -> void { joan::bind::member<&::en3d_helpers22::H::plane_sort>(this, a0); }  // 0x5cf1d0
auto joan::orig::Plane::bone_5cf8d0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::plane_bone>(this)); }  // 0x5cf8d0
auto joan::orig::Plane::bone_5cf8f0(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers13::H::plane_bone>(this, a0)); }  // 0x5cf8f0
auto joan::orig::Plane::invalidate(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::plane_invalidate>(this, a0)); }  // 0x5cf920
auto joan::orig::Plane::upload() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers22::H::plane_upload>(this)); }  // 0x5cfa10
auto joan::orig::Plane::transform_5d0070() -> void { joan::bind::member<&::game_b108_02::H::transform_5d0070>(this); }  // 0x5d0070
auto joan::orig::Plane::set_owner(void* a0) -> void { joan::bind::member<&::en3d_helpers::H::plane_set_owner>(this, a0); }  // 0x5d03f0
auto joan::orig::Plane::visible() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers2::H::plane_visible>(this)); }  // 0x5d0480
auto joan::orig::triangle_kind(void* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::en3d_helpers16::triangle_kind>(a0, a1)); }  // 0x5d2aa0
auto joan::orig::DeviceResource::is_linked() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::en3d_helpers25::H::resource_linked>(this)); }  // 0x5d2b10
auto joan::orig::SysEn::count(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::sysen_count>(this, a0, a1)); }  // 0x5d2b40
auto joan::orig::Plane::submit_5d35b0(void* a0, void* a1, std::int32_t a2) -> void { joan::bind::member<&::en3d_b117_01::H::submit>(this, a0, a1, a2); }  // 0x5d35b0
auto joan::orig::Plane::add_child(std::int32_t a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::add_child>(this, a0, a1)); }  // 0x5d4470
auto joan::orig::Plane::release_lod(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b83_03::H::release_lod>(this, a0)); }  // 0x5d4550
auto joan::orig::Plane::level(void* a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::plane_level>(this, a0, a1)); }  // 0x5d45e0
auto joan::orig::Plane::delete_children() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers36::H::delete_children>(this)); }  // 0x5d4670
auto joan::orig::Plane::set_lod(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b82_09::H::set_lod>(this, a0, a1)); }  // 0x5d4720
auto joan::orig::Plane::lod(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers8::H::plane_lod>(this, a0)); }  // 0x5d4760
auto joan::orig::Plane::set_parent(void* a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers36::H::set_parent>(this, a0, a1)); }  // 0x5d4880
auto joan::orig::Plane::share_bones(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::share_bones>(this, a0)); }  // 0x5d4a00
auto joan::orig::Plane::segment_hit(void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_collide1::H::segment_hit>(this, a0, a1)); }  // 0x5d4a80
auto joan::orig::Material::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::material_construct>(this)); }  // 0x5d52a0
auto joan::orig::Material::dtor() -> void { joan::bind::member<&::en3d_helpers::H::material_destroy>(this); }  // 0x5d5360
auto joan::orig::Material::assign(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::material_assign>(this, a0)); }  // 0x5d53e0
auto joan::orig::Material::equals(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b83_03::H::material_equals>(this, a0)); }  // 0x5d5500
auto joan::orig::Material::set_colour0(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::set_colour0>(this, a0)); }  // 0x5d5540
auto joan::orig::Material::set_colour4(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::set_colour4>(this, a0)); }  // 0x5d5570
auto joan::orig::Material::set_colour8(void* a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::set_colour8>(this, a0, a1)); }  // 0x5d55a0
auto joan::orig::Material::set_colourc(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::set_colourc>(this, a0)); }  // 0x5d55e0
auto joan::orig::Material::set_14_18(void* a0, std::uint32_t a1) -> void { joan::bind::member<&::en3d_b84_01::H::set_14_18>(this, a0, a1); }  // 0x5d5610
auto joan::orig::material::texture_ref(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::en3d_helpers6::material_texture_ref>(a0)); }  // 0x5d56c0
auto joan::orig::Material::to_d3d(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::material_to_d3d>(this, a0)); }  // 0x5d56f0
auto joan::orig::Plane::build_tangents() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers25::H::plane_build_tangents>(this)); }  // 0x5d5770
auto joan::orig::Plane::set_fade(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::plane_set_fade>(this, a0)); }  // 0x5d5e80
auto joan::orig::Plane::colour(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_actors::H::plane_colour>(this, a0)); }  // 0x5d5eb0
auto joan::orig::Plane::set_f8(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b82_09::H::set_f8>(this, a0)); }  // 0x5d5ee0
auto joan::orig::Plane::normals_pending() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::plane_normals_pending>(this)); }  // 0x5d5f30
auto joan::orig::Plane::flags() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::plane_flags>(this)); }  // 0x5d5f60
auto joan::orig::Plane::has_alpha_material() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b100_11::H::has_alpha_material>(this)); }  // 0x5d5fe0
auto joan::orig::Plane::draw_mirror_5d6050(void* a0) -> void { joan::bind::member<&::game_b109_09::H::draw_mirror_5d6050>(this, a0); }  // 0x5d6050
auto joan::orig::Plane::update_opaque() -> void { joan::bind::member<&::en3d_helpers25::H::plane_update_opaque>(this); }  // 0x5d6da0
auto joan::orig::Plane::set_100(std::uint32_t a0) -> void { joan::bind::member<&::game_collide1::H::set_100>(this, a0); }  // 0x5d6e20
auto joan::orig::Plane::set_104(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::plane_set_104>(this, a0)); }  // 0x5d6e80
auto joan::orig::Plane::reserve_strip(std::int32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b82_09::H::reserve_strip>(this, a0, a1)); }  // 0x5d6ee0
auto joan::orig::Plane::clear_extras(std::int32_t a0) -> void { joan::bind::member<&::en3d_helpers20::H::plane_clear_extras>(this, a0); }  // 0x5d7190
auto joan::orig::Plane::set_flag50(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b82_09::H::set_flag50>(this, a0)); }  // 0x5d7270
auto joan::orig::Plane::field_ec() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_06::H::field_ec>(this)); }  // 0x5d72e0
auto joan::orig::Plane::set_ec(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::set_ec>(this, a0)); }  // 0x5d7310
auto joan::orig::Plane::field_f0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_07::H::field_f0>(this)); }  // 0x5d7380
auto joan::orig::Plane::link(void* a0) -> void { joan::bind::member<&::en3d_b84_01::H::link_f0>(this, a0); }  // 0x5d73b0
auto joan::orig::Plane::set_mode(std::uint32_t a0) -> void { joan::bind::member<&::en3d_b84_01::H::set_mode>(this, a0); }  // 0x5d7420
auto joan::orig::Plane::build_normals() -> void { joan::bind::member<&::en3d_b85_03::H::build_normals>(this); }  // 0x5d7680
auto joan::orig::Plane::env_map() -> void { joan::bind::member<&::en3d_helpers9::H::plane_env_map>(this); }  // 0x5d7bf0
auto joan::orig::Camera::type() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::camera_type>(this)); }  // 0x5d80a0
auto joan::orig::Plane::water_texture_5d8650(std::uint32_t a0, void* a1, void* a2, void* a3) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::en3d_b117_02::H::water_texture_5d8650>(this, a0, a1, a2, a3)); }  // 0x5d8650
auto joan::orig::Plane::load_5d9be0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers22::H::plane_load>(this)); }  // 0x5d9be0
auto joan::orig::Mesh::load_any(void* a0, void* a1, std::uint32_t a2, std::uint32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b84_01_2::H::load_any>(this, a0, a1, a2, a3)); }  // 0x5da280
auto joan::orig::Mesh::load_v0(void* a0, void* a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b85_02::H::load_v0>(this, a0, a1, a2)); }  // 0x5da3d0
auto joan::orig::Plane::load_skin(void* a0, void* a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b85_03_2::H::load_skin>(this, a0, a1, a2)); }  // 0x5db710
auto joan::orig::Plane::load_v100_5dc400(void* a0, void* a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b85_04_2::H::zero3>(this, a0, a1, a2)); }  // 0x5dc400
auto joan::orig::Mesh::load_v500(void* a0, void* a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b85_02::H::load_v500>(this, a0, a1, a2)); }  // 0x5dc420
auto joan::orig::Plane::load_5ddd70(void* a0, std::uint8_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_collide1_2::H::load>(this, a0, a1)); }  // 0x5ddd70
auto joan::orig::Plane::load_v0(void* a0, std::uint8_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers35::H::load_v0>(this, a0, a1)); }  // 0x5dde90
auto joan::orig::Plane::load_v50(void* a0, std::uint8_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers35::H::load_v50>(this, a0, a1)); }  // 0x5decf0
auto joan::orig::Mesh::save(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b106_19::H::mesh_save>(this, a0)); }  // 0x5df6a0
auto joan::orig::Plane::load_v100(void* a0, std::uint8_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers34::H::load_v100>(this, a0, a1)); }  // 0x5e0510
auto joan::orig::Plane::load_v500(void* a0, std::uint8_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers34::H::load_v500>(this, a0, a1)); }  // 0x5e0530
auto joan::orig::Mesh::write(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b84_01_3::H::mesh_write>(this, a0)); }  // 0x5e2380
auto joan::orig::Mesh::read(void* a0, std::uint8_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b83_03_2::H::mesh_read>(this, a0, a1)); }  // 0x5e26c0
auto joan::orig::Material::write(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b85_04_2::H::material_write>(this, a0)); }  // 0x5e2a90
auto joan::orig::Material::read(void* a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b84_01_4::H::material_read>(this, a0, a1)); }  // 0x5e2b70
auto joan::orig::Reorder::draw(void* a0, void* a1, std::int32_t a2) -> void { joan::bind::member<&::en3d_b117_03::H::draw>(this, a0, a1, a2); }  // 0x5e67b0
