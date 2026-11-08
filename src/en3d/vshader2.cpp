// SPDX-License-Identifier: GPL-3.0-or-later
// en3d/vshader2.cpp: functions of the original d:\ww\en3d\vshader2.cpp (re/attribution.csv). The file name is the
// Restaurant Empire 2 engine's (en3d/VShader2.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../d3d8/d3d8_types.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

namespace {
// v3d "M" results (length, length_sq, dot_product): the replacements (src/lib/v3d/v3d87.cpp) return double. On x86
// the caller reads st0 as the float it always used (same code as before); x64 returns the double in xmm0, so it is
// read as a double there.
template <class... A>
float v3d_float(joan::img::Addr slot, A... args) {
#if UINTPTR_MAX == 0xffffffffu
    return joan::call::import_call<float>(slot, args...);
#else
    return static_cast<float>(joan::call::import_call<double>(slot, args...));
#endif
}

// The vertex-shader cache key (0x00618200 builds it on the stack, find 0x006182c0 / create 0x00618590 read it): the
// first three members of VsEntry. format: the vertex format descriptor of vertex_format 0x00600200 (a pointer).
struct VsKey {
    std::uint8_t* format;
    std::uint32_t flags;
    std::uint32_t extra;
};
}  // namespace

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using joan::img::obj;

using joan::call::cdecl_call;

using joan::hook::global;

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::orig::helpers::op_delete;

using Device = joan::d3d8::IDirect3DDevice8;

using joan::orig::helpers::game_delete;  // operator delete

using joan::orig::helpers::game_new;
void out_of_memory() { joan::orig::as_error_log(img::obj(d::g_err))->mem_error(); }  // 0x00586af0

using joan::call::import_call;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/en3d_helpers20.cpp (en3d/helpers21)
namespace {
namespace en3d_helpers20 {

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
void* game_device() { return global<void*>(d::g_game_device); }

JOAN_COUNTER(c_vs_create, "VsCache::create");

struct H {
    // 0x00618590 VsCache::create(key): grows the entry table (+8, capacity +0xc, count +0x10)
    // by 8, builds a 0x20-byte entry {key[3], builder, handle, ...} and assembles the shader
    // through the builder (0x00618990) from the declaration (0x00618e40) and function
    // (0x00618bb0) for this key. Returns the entry, 0 on failure.
    void* vs_create(const VsKey* key) {
        c_vs_create.hit();
        if (key == nullptr) return nullptr;
        if (jo::view<void*>(jo::self<jo::VsCache>(this)->entries) == nullptr || jo::self<jo::VsCache>(this)->i32_c <= 0) {
            jo::self<jo::VsCache>(this)->i32_c = 8;
            jo::view<void*>(jo::self<jo::VsCache>(this)->entries) = game_new(static_cast<std::uint32_t>(jo::self<jo::VsCache>(this)->i32_c * sizeof(jo::VsEntryPtr)));  // pointer array (4-byte entries on x86)
            if (jo::view<void*>(jo::self<jo::VsCache>(this)->entries) == nullptr) out_of_memory();
            jo::self<jo::VsCache>(this)->i32_10 = 0;
        } else if (jo::self<jo::VsCache>(this)->i32_c == jo::self<jo::VsCache>(this)->i32_10) {
            void* const old = jo::view<void*>(jo::self<jo::VsCache>(this)->entries);
            jo::self<jo::VsCache>(this)->i32_c = jo::self<jo::VsCache>(this)->i32_c + 8;
            jo::view<void*>(jo::self<jo::VsCache>(this)->entries) = game_new(static_cast<std::uint32_t>(jo::self<jo::VsCache>(this)->i32_c * sizeof(jo::VsEntryPtr)));  // pointer array (4-byte entries on x86)
            if (jo::view<void*>(jo::self<jo::VsCache>(this)->entries) == nullptr) out_of_memory();
            std::memmove(jo::view<void*>(jo::self<jo::VsCache>(this)->entries), old, static_cast<std::size_t>(jo::self<jo::VsCache>(this)->i32_10) * sizeof(jo::VsEntryPtr));
            game_delete(old);
        }
        // the entry without its constants block: 0x20 on x86 (key_format / constant_table are pointers)
        std::uint8_t* const entry = static_cast<std::uint8_t*>(game_new(static_cast<std::uint32_t>(offsetof(jo::VsEntry, constants))));
        if (entry == nullptr) out_of_memory();
        jo::as_vs_entry(entry)->key_format = key->format;
        jo::as_vs_entry(entry)->key_flags = key->flags;
        jo::as_vs_entry(entry)->key_extra = key->extra;
        jo::as_vs_entry(entry)->constant_table = joan::orig::arg::as<void*>(joan::orig::as_vs_cache(this)->create_linker());  // 0x00618990 builder
        jo::view<std::uint32_t>(jo::as_vs_entry(entry)->dir_lights) = 0;
        jo::view<std::uint32_t>(jo::as_vs_entry(entry)->point_lights) = 0;
        jo::view<std::uint32_t>(jo::as_vs_entry(entry)->spot_lights) = 0;
        jo::as_vs_entry(entry)->u32_10 = 0;
        struct Frame {  // original locals, lowest address first
            std::uint8_t declaration[0x190];  // ebp-0x1f0
            std::int32_t hr;                  // ebp-0x60
            std::uint8_t function[0x50];      // ebp-0x5c
        } s;
        std::memset(static_cast<void*>(s.function), 0, 0x50u);                        // memset
        joan::orig::as_vs_cache(this)->declaration(joan::orig::arg::mptr(s.function), joan::orig::arg::mptr(jo::as_vs_entry(entry)->key_format));        // 0x00618bb0
        joan::orig::as_vs_cache(this)->fragments(joan::orig::arg::mptr(s.declaration), joan::orig::arg::mptr(static_cast<void*>(entry)));      // 0x00618e40
        void* const builder = jo::as_vs_entry(entry)->constant_table;
        com_call(builder, 9);
        com_call(jo::as_vs_entry(entry)->constant_table, 10);
        if (static_cast<std::int8_t>(key->format[0]) == 7)
            com_call(jo::as_vs_entry(entry)->constant_table, 7, 0u, 0x58u);
        else
            com_call(jo::as_vs_entry(entry)->constant_table, 7, 0u, 0x16u);
        const std::uint32_t usage = global<std::uint32_t>(d::g_a52438) == 0 ? 0x10u : 0u;
        s.hr = com_call(jo::as_vs_entry(entry)->constant_table, 12, game_device(), static_cast<void*>(s.function), static_cast<void*>(s.declaration),
                        static_cast<void*>(&jo::as_vs_entry(entry)->u32_10), usage, 0u);
        if (s.hr != 0) {
            com_call(jo::as_vs_entry(entry)->constant_table, 2);  // Release
            game_delete(entry);
            return nullptr;
        }
        jo::view<void**>(jo::self<jo::VsCache>(this)->entries)[jo::self<jo::VsCache>(this)->i32_10] = entry;
        jo::self<jo::VsCache>(this)->i32_10 = jo::self<jo::VsCache>(this)->i32_10 + 1;
        return entry;
    }
};

#define HELPER21(name, addr, fn) JOAN_REPLACE("en3d/helpers21", name, addr, fn)
HELPER21("VertexShaderFactory::create_shader", 0x00618590, &H::vs_create);
#undef HELPER21

}  // namespace en3d_helpers20
}  // namespace

// ---------------------------------------- from modules/en3d_helpers28.cpp (en3d/helpers28)
namespace {
namespace en3d_helpers28 {

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
void* game_device() { return global<void*>(d::g_game_device); }
constexpr std::int32_t kGetLight = 45, kGetLightEnable = 47, kGetRenderState = 51;
constexpr std::int32_t kLinkerFragment = 4;  // nvlinker: fragment ID by name

JOAN_COUNTER(c_fragments, "VsCache::fragments");

// Fragment names in .rdata.
const char* name(img::Addr address) { return &img::global<const char>(address); }

struct H {
    // 0x00618e40 VsCache::fragments(out, entry): entry +0 format (+0 type, +1 texture
    // coordinates, +3 pre-lit, +0xc FVF), +4 flags (1 dot3, 2 linear fog, 4 / 8 texture fog,
    // 0x40 scattering), +0xc linker, +0x14 / +0x18 / +0x1c point / directional / spot light
    // counts. Writes the IDs to out, 0-terminated.
    void vs_fragments(std::uint32_t* out, std::uint8_t* entry) {
        c_fragments.hit();
        std::int32_t n = 0;
        auto add = [&](const char* fragment) {
            std::uint32_t* const slot = out + n;
            ++n;
            com_call(jo::as_vs_entry(entry)->constant_table, kLinkerFragment, fragment, slot);
        };
        auto add_texture_coordinates = [&]() {
            for (std::int32_t i = 0; i < jo::self<jo::VertexFormat>(jo::as_vs_entry(entry)->key_format)->tex_coords; ++i) {
                char buffer[36];
                std::memcpy(buffer, name(d::s_write_default_texture_coordi), 36);  // "write_default_texture_coordinate_" (9 dwords)
                buffer[std::strlen(buffer)] = static_cast<char>(i + 0x30);
                add(buffer);
            }
        };
        auto render_state = [](std::uint32_t state) {
            std::uint32_t value = 0;
            com_call(game_device(), kGetRenderState, state, &value);
            return value;
        };
        // Enabled lights: type 1 point, 2 spot, 3 directional.
        auto for_lights = [&](auto&& on_light) {
            for (std::uint32_t i = 0; i < global<std::uint32_t>(d::g_a5259f); ++i) {
                std::int32_t enabled = 0;
                if (com_call(game_device(), kGetLightEnable, i, &enabled) != 0 || enabled == 0) continue;
                std::uint8_t light[0x68];
                com_call(game_device(), kGetLight, i, static_cast<void*>(light));
                on_light(jo::object<joan::d3d8::Light>(light)->type);
            }
        };
        const std::uint8_t* const format = jo::view<std::uint8_t*>(jo::as_vs_entry(entry)->key_format);
        const std::uint32_t flags = jo::as_vs_entry(entry)->key_flags;
        if (static_cast<std::int8_t>(format[0]) != 7) {
            const std::int8_t type = static_cast<std::int8_t>(format[0]);
            const std::uint32_t fvf = jo::self<jo::VertexFormat>(format)->fvf;
            add((fvf & 2) != 0 ? name(d::s_transform_and_write_position) : name(d::s_write_untransformed_position));  // transform_and_write_position / write_untransformed_position
            if ((fvf & 0x20) != 0) add(name(d::s_write_vertex_psize));                 // write_vertex_psize
            if ((flags & 4) != 0 || (flags & 8) != 0) add(name(d::s_clear_texture_coordinates));  // clear_texture_coordinates
            add_texture_coordinates();
            const std::uint32_t lighting = render_state(0x89);
            const std::uint32_t color_vertex = render_state(0x8d);
            if ((flags & 0x40) != 0) {
                add(name(d::s_calculate_objectspace_positi));  // calculate_objectspace_position
                add(name(d::s_transform_to_eyespace));  // transform_to_eyespace
                add(name(d::s_create_normalized_eye_to_ver));  // create_normalized_eye_to_vertex
                add(name(d::s_scattering));  // scattering
            } else if (static_cast<std::int8_t>(format[3]) == 0) {
                if ((flags & 1) != 0 && type == 5) {
                    add(name(d::s_clear_diffuse_color));  // clear_diffuse_color
                    add(name(d::s_clear_specular_color));  // clear_specular_color
                    add(name(d::s_calculate_objectspace_positi));
                    add(name(d::s_calculate_objectspace_normal));  // calculate_objectspace_normal
                    for_lights([&](std::uint32_t light) {
                        if (light == 1) {
                            add(name(d::s_add_point_light_no_specular));  // add_point_light_no_specular
                            jo::as_vs_entry(entry)->point_lights = jo::as_vs_entry(entry)->point_lights + 1;
                        } else if (light == 3) {
                            add(name(d::s_add_directional_light_no_spe));  // add_directional_light_no_specular
                            jo::as_vs_entry(entry)->dir_lights = jo::as_vs_entry(entry)->dir_lights + 1;
                        } else if (light == 2) {
                            add(name(d::s_add_spot_light_no_specular));  // add_spot_light_no_specular
                            jo::as_vs_entry(entry)->spot_lights = jo::as_vs_entry(entry)->spot_lights + 1;
                        }
                    });
                    add(name(d::s_calculate_dot3_light));  // calculate_dot3_light
                } else if (lighting != 0) {
                    add(name(d::s_clear_diffuse_color));
                    add(name(d::s_clear_specular_color));
                    add(name(d::s_calculate_objectspace_positi));
                    add(name(d::s_calculate_objectspace_normal));
                    add(name(d::s_create_normalized_eye_to_ver));
                    for_lights([&](std::uint32_t light) {
                        if (light == 1) {
                            add(name(d::s_add_point_light));  // add_point_light
                            jo::as_vs_entry(entry)->point_lights = jo::as_vs_entry(entry)->point_lights + 1;
                        } else if (light == 3) {
                            add(name(d::s_add_directional_light));  // add_directional_light
                            jo::as_vs_entry(entry)->dir_lights = jo::as_vs_entry(entry)->dir_lights + 1;
                        } else if (light == 2) {
                            add(name(d::s_add_spot_light));  // add_spot_light
                            jo::as_vs_entry(entry)->spot_lights = jo::as_vs_entry(entry)->spot_lights + 1;
                        }
                    });
                } else {
                    add(name(d::s_set_default_diffuse_color));  // set_default_diffuse_color
                    add(name(d::s_set_default_specular_color));  // set_default_specular_color
                }
            }
            // Colour output (0x0061972b).
            if ((flags & 0x40) == 0) {
                if (static_cast<std::int8_t>(format[3]) != 0) {
                    add(name(d::s_write_vertex_diffuse_color));  // write_vertex_diffuse_color
                    add(name(d::s_write_vertex_specular_color));  // write_vertex_specular_color
                } else if (lighting != 0 || color_vertex != 0) {
                    if (render_state(0x91) == 0)
                        add(name(d::s_write_diffuse_color_with_mat));  // write_diffuse_color_with_material_diffuse
                    else if ((fvf & 0x40) != 0)
                        add(name(d::s_write_diffuse_color_with_ver));  // write_diffuse_color_with_vertex_diffuse
                    else
                        add(name(d::s_write_diffuse_color));  // write_diffuse_color
                    if ((flags & 1) != 0 && type == 5)
                        add(name(d::s_write_specular_color));  // write_specular_color
                    else if (render_state(0x92) == 0)
                        add(name(d::s_write_specular_color_with_ma));  // write_specular_color_with_material_specular
                    else if ((fvf & 0x80) != 0)
                        add(name(d::s_write_specular_color_with_ve));  // write_specular_color_with_vertex_specular
                    else
                        add(name(d::s_write_specular_color));
                } else {
                    add(name(d::s_write_diffuse_color));
                }
            }
        } else {
            // Hardware skinning (type 7).
            add(name(d::s_calculate_skin_vertex));  // calculate_skin_vertex
            add_texture_coordinates();
            const std::uint32_t lighting = render_state(0x89);
            render_state(0x8d);
            if (static_cast<std::int8_t>(format[3]) == 0) {
                if ((flags & 1) != 0 || lighting != 0) {
                    add(name(d::s_clear_diffuse_color));
                    add(name(d::s_clear_specular_color));
                    for_lights([&](std::uint32_t light) {
                        if (light == 1)
                            jo::as_vs_entry(entry)->point_lights = jo::as_vs_entry(entry)->point_lights + 1;
                        else if (light == 3)
                            jo::as_vs_entry(entry)->dir_lights = jo::as_vs_entry(entry)->dir_lights + 1;
                        else if (light == 2)
                            jo::as_vs_entry(entry)->spot_lights = jo::as_vs_entry(entry)->spot_lights + 1;
                    });
                    if (jo::as_vs_entry(entry)->point_lights > 1) jo::as_vs_entry(entry)->point_lights = 1;
                    if (jo::as_vs_entry(entry)->dir_lights > 1) jo::as_vs_entry(entry)->dir_lights = 1;
                    if (jo::as_vs_entry(entry)->spot_lights > 0) jo::as_vs_entry(entry)->spot_lights = 0;
                    for (std::int32_t i = 0; i < jo::as_vs_entry(entry)->point_lights; ++i) add(name(d::s_add_point_light_no_specular));
                    for (std::int32_t i = 0; i < jo::as_vs_entry(entry)->dir_lights; ++i) add(name(d::s_add_directional_light_no_spe));
                    for (std::int32_t i = 0; i < jo::as_vs_entry(entry)->spot_lights; ++i) add(name(d::s_add_spot_light_no_specular));
                } else {
                    add(name(d::s_set_default_diffuse_color));  // set_default_diffuse_color
                }
            }
            if (static_cast<std::int8_t>(jo::view<std::uint8_t*>(jo::as_vs_entry(entry)->key_format)[3]) != 0) {
                add(name(d::s_write_vertex_diffuse_color));
            } else if (jo::as_vs_entry(entry)->point_lights == 0 && jo::as_vs_entry(entry)->dir_lights == 0 && jo::as_vs_entry(entry)->spot_lights == 0) {
                add(name(d::s_write_diffuse_color));
            } else if (render_state(0x91) == 0) {
                add(name(d::s_write_diffuse_color_with_mat));
            } else {
                add(name(d::s_write_diffuse_color_with_ver));
            }
            add(name(d::s_clip_plane));  // clip_plane
        }
        // Fog (0x00619fca).
        if ((flags & 2) != 0) {
            add(name(d::s_transform_to_eyespace));
            add(name(d::s_create_normalized_eye_to_ver));
            add(name(d::s_calculate_linear_normal_fog));  // calculate_linear_normal_fog_factor
            add(name(d::s_write_d3d_fog_factor));  // write_d3d_fog_factor
        } else if ((flags & 4) != 0) {
            add(name(d::s_transform_to_eyespace));
            add(name(d::s_create_normalized_eye_to_ver));
            add(name(d::s_calculate_linear_normal_fog));
            add(joan::orig::arg::as<std::int32_t>(joan::orig::as_sys_en(img::obj(d::g_sys3d))->overlay_stage()) == 1 ? name(d::s_write_fog_to_texture_coordin_9da1e0) : name(d::s_write_fog_to_texture_coordin_9da1b8));  // 0x00601470
        } else if ((flags & 8) != 0) {
            add(name(d::s_transform_to_eyespace));
            add(name(d::s_create_normalized_eye_to_ver));
            add(name(d::s_calculate_objectspace_positi));
            add(name(d::s_calculate_height_fog_factor));  // calculate_height_fog_factor
            add(joan::orig::arg::as<std::int32_t>(joan::orig::as_sys_en(img::obj(d::g_sys3d))->overlay_stage()) == 1 ? name(d::s_write_fog_to_texture_coordin_9da1e0) : name(d::s_write_fog_to_texture_coordin_9da1b8));
        }
        out[n] = 0;
    }
};

#define HELPER28(name, addr, fn) JOAN_REPLACE("en3d/helpers28", name, addr, fn)
HELPER28("VertexShaderFactory::get_required_fragments", 0x00618e40, &H::vs_fragments);
#undef HELPER28

}  // namespace en3d_helpers28
}  // namespace

// ---------------------------------------- from modules/en3d_helpers18.cpp (en3d/helpers18)
namespace {
namespace en3d_helpers18 {

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

void* game_device() { return global<void*>(d::g_game_device); }

JOAN_COUNTER(c_vs_matrices, "VsCache::set_matrices");

struct H {
    // --- en3d/helpers18 ---------------------------------------------------------------------
    // 0x0061aba0: world / view / projection -> c1 (W*V*P), c13 (W*V), c5 (W, 3 rows), c17..c19
    // inverse(W*V) rows, c10 eye position (the 4th float is inverse(W*V)._11, as laid out on the
    // original stack frame).
    std::int32_t vs_set_matrices() {
        c_vs_matrices.hit();
        struct Frame {  // the original locals, lowest address first
            float view_inverse[16];  // ebp-0x1d0
            float eye[3];            // ebp-0x190
            float wv_inverse[16];    // ebp-0x184
            float wvp[16];           // ebp-0x144
            float wv[16];            // ebp-0x104
            float projection[16];    // ebp-0xc4
            float view[16];          // ebp-0x84
            float world[16];         // ebp-0x44
        } s;
        static_assert(sizeof(Frame) == 0x1d0 - 4);
        void* const d = game_device();
        com_call(d, 38, 0x100u, s.world);  // GetTransform(WORLD)
        com_call(game_device(), 38, 2u, s.view);
        com_call(game_device(), 38, 3u, s.projection);
        import_call<void*>(d::iat_v3d_87_matrix_multiply_3, s.wv, s.world, s.view);  // matrix_multiply(out, a, b)
        import_call<void*>(d::iat_v3d_87_matrix_multiply_3, s.wvp, s.wv, s.projection);
        import_call<void*>(d::iat_v3d_87_matrix_inverse_2, s.wv_inverse, s.wv);  // matrix_inverse(out, m)
        import_call<void*>(d::iat_v3d_87_matrix_transpose_1, s.world);             // matrix_transpose
        import_call<void*>(d::iat_v3d_87_matrix_transpose_1, s.wv);
        import_call<void*>(d::iat_v3d_87_matrix_transpose_1, s.wvp);
        import_call<void*>(d::iat_v3d_87_matrix_transpose_1, s.wv_inverse);
        import_call<void*>(d::iat_v3d_87_matrix_inverse_2, s.view_inverse, s.view);
        joan::orig::matrix_position(joan::orig::arg::mptr(s.eye), joan::orig::arg::ptr(s.view_inverse));  // 0x0060c310 translation
        com_call(game_device(), 79, 1u, static_cast<const void*>(s.wvp), 4u);  // SetVertexShaderConstant
        com_call(game_device(), 79, 0xdu, static_cast<const void*>(s.wv), 4u);
        com_call(game_device(), 79, 5u, static_cast<const void*>(s.world), 3u);
        com_call(game_device(), 79, 0x11u, static_cast<const void*>(s.wv_inverse), 1u);
        com_call(game_device(), 79, 0x12u, static_cast<const void*>(s.wv_inverse + 4), 1u);
        com_call(game_device(), 79, 0x13u, static_cast<const void*>(s.wv_inverse + 8), 1u);
        (void)d;
        return com_call(game_device(), 79, 0xau, static_cast<const void*>(s.eye), 1u);
    }
};

#define HELPER18(name, addr, fn) JOAN_REPLACE("en3d/helpers18", name, addr, fn)
HELPER18("VertexShaderFactory::set_matrix_constants", 0x0061aba0, &H::vs_set_matrices);
#undef HELPER18

}  // namespace en3d_helpers18
}  // namespace

// ---------------------------------------- from modules/en3d_helpers25.cpp (en3d/helpers26)
namespace {
namespace en3d_helpers25 {

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
void* game_device() { return global<void*>(d::g_game_device); }

// IDirect3DDevice8 methods.
constexpr std::int32_t kGetTransform = 38, kGetMaterial = 43, kGetLight = 45, kGetLightEnable = 47, kGetClipPlane = 49,
                       kGetRenderState = 51, kSetVertexShaderConstant = 79;
// nvlinker shader object methods (entry +0xc).
constexpr std::int32_t kLinkerId = 5, kLinkerRegister = 16;

std::int32_t set_constant(std::uint32_t reg, const void* data, std::uint32_t count) {
    return com_call(game_device(), kSetVertexShaderConstant, reg, data, count);
}

JOAN_COUNTER(c_vs_skin, "VsCache::setup_skin");
JOAN_COUNTER(c_vs_scatter, "Scatter::set_constants");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- en3d/helpers26 ---------------------------------------------------------------------
    // 0x0061c390 VsCache::setup_skin(entry): constants for the hardware-skinning shader -
    // c0 (0, 0.5, 1, log2 e), material c11 / c12, view-projection c1, view c13, world c5
    // (identity), eye c10; then the nearest point light (the main light 0xa48cd0 wins) and the
    // strongest directional light through the linker's named registers, and the first enabled
    // user clip plane.
    std::int32_t vs_setup_skin(std::uint8_t* entry) {
        c_vs_skin.hit();
        void* const linker = jo::as_vs_entry(entry)->constant_table;
        jo::self<jo::VsCache>(this)->u32_0 = 1;
        float c0[4];
        set_constant(0, joan::orig::arg::as<void*>(joan::orig::as_vector4(c0)->construct_xyzw(0u, 0x3f000000u, 0x3f800000u, 0x3fb8aa3bu)), 1);  // 0x005bd220
        struct Frame {  // original locals -0x1ec .. -0x4, lowest address first
            float view_inverse[16];  // -0x1ec
            float origin[3];         // -0x1ac
            float view_axis[3];      // -0x1a0
            float eye[3];            // -0x194
            float view_proj[16];     // -0x188
            float view_copy[16];     // -0x148
            float projection[16];    // -0x108
            float view[16];          // -0xc8
            float world[16];         // -0x88
            float material[17];      // -0x48 (D3DMATERIAL8)
        } s;
        static_assert(sizeof(Frame) == 0x1ec - 4);
        com_call(game_device(), kGetMaterial, static_cast<void*>(s.material));
        set_constant(0xb, s.material + 4, 1);  // ambient
        set_constant(0xc, s.material, 1);      // diffuse
        std::memcpy(s.world, img::obj(d::g_identity_matrix), 64);
        com_call(game_device(), kGetTransform, 2u, static_cast<void*>(s.view));
        com_call(game_device(), kGetTransform, 3u, static_cast<void*>(s.projection));
        std::memcpy(s.view_copy, s.view, 64);
        import_call<void*>(d::iat_v3d_87_matrix_multiply_3, s.view_proj, s.view_copy, s.projection);  // matrix_multiply(out, a, b)
        joan::orig::matrix_axis_z(joan::orig::arg::mptr(s.view_axis), joan::orig::arg::ptr(s.view));                       // 0x0060c220
        joan::orig::matrix_position(joan::orig::arg::mptr(s.origin), joan::orig::arg::ptr(s.world));                         // 0x0060c310
        import_call<void*>(d::iat_v3d_87_matrix_transpose_1, s.view_copy);                              // transpose
        import_call<void*>(d::iat_v3d_87_matrix_transpose_1, s.view_proj);
        import_call<void*>(d::iat_v3d_87_matrix_inverse_2, s.view_inverse, s.view);  // matrix_inverse(out, m)
        joan::orig::matrix_position(joan::orig::arg::mptr(s.eye), joan::orig::arg::ptr(s.view_inverse));
        set_constant(1, s.view_proj, 4);
        set_constant(0xd, s.view_copy, 4);
        set_constant(5, s.world, 3);
        set_constant(0xa, s.eye, 1);  // 4th float = view_proj[0] (stack layout)
        if (jo::self<jo::VertexFormat>(jo::as_vs_entry(entry)->key_format)->prelit == 0) {
            std::uint32_t ambient_rs;
            com_call(game_device(), kGetRenderState, 0x8bu, &ambient_rs);
            float ambient[4];
            joan::orig::as_colour(&ambient_rs)->to_float_5e9260(joan::orig::arg::mptr(ambient));  // 0x005e9260
            set_constant(9, ambient, 1);
            std::uint32_t lighting;
            com_call(game_device(), kGetRenderState, 0x89u, &lighting);
            bool have_point = false, have_dir = false;
            if (lighting != 0) {
                float best_distance = 3.4028234663852886e+38f;  // 0x7f7fffff
                float best_dot = 0.0f;
                std::uint32_t point_index = 0, dir_index = 0;
                for (std::uint32_t i = 0; i < global<std::uint32_t>(d::g_a5259f); ++i) {
                    std::int32_t enabled = 0;
                    if (com_call(game_device(), kGetLightEnable, i, &enabled) != 0 || enabled == 0) continue;
                    std::uint8_t light[0x68];
                    com_call(game_device(), kGetLight, i, static_cast<void*>(light));
                    const std::uint32_t type = jo::object<joan::d3d8::Light>(light)->type;
                    if (type == 1) {
                        const float* const pos = reinterpret_cast<const float*>(light + 0x34);
                        const float dx = s.origin[0] - pos[0], dy = s.origin[1] - pos[1], dz = s.origin[2] - pos[2];
                        const float d2 = dx * dx + dy * dy + dz * dz;
                        if (have_point && d2 >= best_distance) continue;
                        std::uint8_t main_light[0x68];
                        const std::uint8_t* const ml = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_light(global<void*>(d::g_main_light))->params(joan::orig::arg::mptr(main_light)));  // 0x0061ddc0
                        if ((joan::orig::arg::as<std::uint32_t>(joan::orig::vec_equal(joan::orig::arg::ptr(ml + 0x34), joan::orig::arg::ptr(pos))) & 0xff) != 0) {  // 0x0060ced0 same position
                            if (!have_point) {
                                have_point = true;
                                point_index = i;
                            }
                        } else {
                            have_point = true;
                            best_distance = d2;
                            point_index = i;
                        }
                    } else if (type == 3) {
                        const float dot = v3d_float(d::iat_v3d_87_dot_product, light + 0x40, s.view_axis);  // dot_product
                        if (have_dir && !(dot > best_dot)) continue;
                        have_dir = true;
                        best_dot = dot;
                        dir_index = i;
                    }
                }
                std::uint32_t reg = 0;
                if (jo::as_vs_entry(entry)->point_lights != 0) {
                    std::uint32_t id_pos = 0, id_att = 0, id_col = 0;
                    com_call(linker, kLinkerId, global<const char*>(d::g_a192b4), &id_pos);
                    com_call(linker, kLinkerId, global<const char*>(d::g_a192d4), &id_att);
                    com_call(linker, kLinkerId, global<const char*>(d::g_a19314), &id_col);
                    if (have_point) {
                        std::uint8_t light[0x68];
                        com_call(game_device(), kGetLight, point_index, static_cast<void*>(light));
                        com_call(linker, kLinkerRegister, id_pos, 0u, &reg);
                        set_constant(reg, light + 0x34, 1);  // position (+ direction.x)
                        com_call(linker, kLinkerRegister, id_att, 0u, &reg);
                        float v[4];
                        set_constant(reg, joan::orig::arg::as<void*>(joan::orig::as_vector4(v)->construct_xyzw(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::object<joan::d3d8::Light>(light)->attenuation0)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::object<joan::d3d8::Light>(light)->attenuation1)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::object<joan::d3d8::Light>(light)->attenuation2)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::object<joan::d3d8::Light>(light)->range)))),
                                     1);  // attenuation, range
                        com_call(linker, kLinkerRegister, id_col, 0u, &reg);
                        set_constant(reg, light + 4, 1);  // diffuse
                    } else {
                        const float zero[4] = {0, 0, 0, 0}, one[4] = {1.0f, 0, 0, 0};
                        com_call(linker, kLinkerRegister, id_pos, 0u, &reg);
                        set_constant(reg, zero, 1);
                        com_call(linker, kLinkerRegister, id_att, 0u, &reg);
                        set_constant(reg, one, 1);
                        com_call(linker, kLinkerRegister, id_col, 0u, &reg);
                        set_constant(reg, zero, 1);
                    }
                }
                if (jo::as_vs_entry(entry)->dir_lights != 0) {
                    std::uint32_t id_dir = 0, id_col = 0;
                    com_call(linker, kLinkerId, global<const char*>(d::g_a19354), &id_dir);
                    com_call(linker, kLinkerId, global<const char*>(d::g_a19394), &id_col);
                    if (have_dir) {
                        std::uint8_t light[0x68];
                        com_call(game_device(), kGetLight, dir_index, static_cast<void*>(light));
                        com_call(linker, kLinkerRegister, id_dir, 0u, &reg);
                        set_constant(reg, light + 0x40, 1);  // direction (+ range)
                        com_call(linker, kLinkerRegister, id_col, 0u, &reg);
                        set_constant(reg, light + 4, 1);
                    } else {
                        const float zero[4] = {0, 0, 0, 0}, one[4] = {1.0f, 0, 0, 0};
                        com_call(linker, kLinkerRegister, id_dir, 0u, &reg);
                        set_constant(reg, one, 1);
                        com_call(linker, kLinkerRegister, id_col, 0u, &reg);
                        set_constant(reg, zero, 1);
                    }
                }
            }
        }
        std::uint32_t id_clip = 0, reg_clip = 0;
        com_call(linker, kLinkerId, global<const char*>(d::g_a19654), &id_clip);
        com_call(linker, kLinkerRegister, id_clip, 0u, &reg_clip);
        std::uint32_t clip_enable = 0;
        com_call(game_device(), kGetRenderState, 0x98u, &clip_enable);
        for (std::uint32_t i = 0; i < 6; ++i) {
            if ((clip_enable & (1u << i)) == 0) continue;
            float plane[4];
            com_call(game_device(), kGetClipPlane, i, static_cast<void*>(plane));
            return set_constant(reg_clip, plane, 1);
        }
        const float zero[4] = {0, 0, 0, 0};
        return set_constant(reg_clip, zero, 1);
    }

    // 0x0061d240 Scatter::set_constants(entry): sky / aerial-perspective scattering constants
    // by name (each looked up, skipped when the register is -1), from the parameters at +0x20 ..
    // +0x7c; texture-0 transform rows last. Clears +0 (dirty). `this` is the VsCache (g_a52597): its
    // +0x20 .. +0x80 block (gap_20 + slider_factors) holds the LightParams record that VsCache::set_constants
    // (0x0061d200) copies in from the current light set (camera3d.cpp reads the same record through
    // LightParams). The VsCache has pointer fields before the block, so it is addressed through the VsCache
    // layout, not through x86 offsets (the Scatter view put it 12 bytes too early on x64: dark sky).
    void scatter_set_constants(std::uint8_t* entry) {
        static_assert(offsetof(jo::VsCache, slider_factors) == offsetof(jo::VsCache, gap_20) + 0x30 &&
                          sizeof(jo::LightParams) == 0x60,
                      "the VsCache's +0x20 block is one LightParams record");
        const jo::LightParams* const atm = reinterpret_cast<const jo::LightParams*>(jo::self<jo::VsCache>(this)->gap_20);
        c_vs_scatter.hit();
        void* const linker = jo::as_vs_entry(entry)->constant_table;
        struct Frame {  // original locals -0xbc .. -0x4, lowest address first
            float texture[16];  // -0xbc (row 2 at -0xac)
            float c7c[4];       // -0x7c
            float c6c[3];       // -0x6c
            float c60[3];       // -0x60
            float c54[4];       // -0x54
            float c44[3];       // -0x44
            float c38[3];       // -0x38
            float c2c[4];       // -0x2c
            float x;            // -0x1c
            float c18[3];       // -0x18
            std::int32_t reg;   // -0xc
            std::uint32_t id;   // -0x8
        } s;
        static_assert(sizeof(Frame) == 0xbc - 4);
        auto lookup = [&](img::Addr name) {
            com_call(linker, kLinkerId, global<const char*>(name), &s.id);  // the name pointer (a u32 would be cut on x64)
            com_call(linker, kLinkerRegister, s.id, 0u, &s.reg);
            return s.reg != -1;
        };
        auto copy3 = [](float* dst, const void* src) { std::memcpy(dst, src, 12); };
        float t0[3], t1[3], t2[3], t3[3], t4[3];
        if (lookup(d::g_a194f4)) {
            copy3(s.c18, joan::orig::arg::as<void*>(joan::orig::vec_negate(joan::orig::arg::mptr(t0), joan::orig::arg::ptr(atm->sun_dir))));  // 0x0060cbc0 negate (+0x50)
            set_constant(static_cast<std::uint32_t>(s.reg), s.c18, 1);        // 4th float = the register number
        }
        if (lookup(d::g_a19514)) set_constant(static_cast<std::uint32_t>(s.reg), atm->scale_b, 1);  // +0x5c
        s.x = atm->scale_a[3];  // +0x78
        s.c2c[0] = 1.0f - s.x * s.x;
        s.c2c[1] = 1.0f + s.x;
        s.c2c[2] = 2.0f * s.x;
        s.c2c[3] = 0.0f;
        if (lookup(d::strs_a19534)) set_constant(static_cast<std::uint32_t>(s.reg), s.c2c, 1);
        {
            float a[3], b[3], c[3], d[3];
            copy3(a, joan::orig::arg::as<void*>(joan::orig::vec_add(joan::orig::arg::mptr(t0), joan::orig::arg::ptr(atm->vec_18), joan::orig::arg::ptr(atm->vec_24))));  // 0x0060cc20 add
            copy3(b, joan::orig::arg::as<void*>(joan::orig::vec_scaled(joan::orig::arg::mptr(t1), joan::orig::arg::f32(atm->scale_a[1] * 100.0f), joan::orig::arg::ptr(a))));     // 0x0060cd60 scale
            copy3(c, joan::orig::arg::as<void*>(joan::orig::vec_scaled(joan::orig::arg::mptr(t2), joan::orig::arg::f32(atm->scale_a[0] * 100.0f), joan::orig::arg::ptr(atm->vec_c))));
            copy3(d, joan::orig::arg::as<void*>(joan::orig::vec_add(joan::orig::arg::mptr(t3), joan::orig::arg::ptr(c), joan::orig::arg::ptr(b))));
            copy3(s.c38, joan::orig::arg::as<void*>(joan::orig::vec_scale(joan::orig::arg::mptr(t4), joan::orig::arg::ptr(d), joan::orig::arg::f32(atm->scale_a[4]))));  // 0x0060cd00 scale
        }
        if (lookup(d::g_a19554)) set_constant(static_cast<std::uint32_t>(s.reg), s.c38, 1);  // 4th = c2c[0]
        {
            float a[3], b[3], c[3];
            copy3(a, joan::orig::arg::as<void*>(joan::orig::vec_add(joan::orig::arg::mptr(t0), joan::orig::arg::ptr(atm->vec_18), joan::orig::arg::ptr(atm->vec_24))));
            copy3(b, joan::orig::arg::as<void*>(joan::orig::vec_scaled(joan::orig::arg::mptr(t1), joan::orig::arg::f32(atm->scale_a[1] * 100.0f), joan::orig::arg::ptr(a))));
            copy3(c, joan::orig::arg::as<void*>(joan::orig::vec_scaled(joan::orig::arg::mptr(t2), joan::orig::arg::f32(atm->scale_a[0] * 100.0f), joan::orig::arg::ptr(atm->vec_c))));
            copy3(s.c44, joan::orig::arg::as<void*>(joan::orig::vec_add(joan::orig::arg::mptr(t3), joan::orig::arg::ptr(c), joan::orig::arg::ptr(b))));
        }
        s.c54[0] = 1.0f / s.c44[0];
        s.c54[1] = 1.0f / s.c44[1];
        s.c54[2] = 1.0f / s.c44[2];
        s.c54[3] = 0.0f;
        if (lookup(d::g_a19574)) set_constant(static_cast<std::uint32_t>(s.reg), s.c54, 1);
        copy3(s.c60, joan::orig::arg::as<void*>(joan::orig::vec_scaled(joan::orig::arg::mptr(t0), joan::orig::arg::f32(5.968310356140137f * atm->scale_a[0]), joan::orig::arg::ptr(atm->vec_c))));  // 0x9d9d38
        if (lookup(d::g_a19594)) set_constant(static_cast<std::uint32_t>(s.reg), s.c60, 1);           // 4th = c54[0]
        copy3(s.c6c, joan::orig::arg::as<void*>(joan::orig::vec_scaled(joan::orig::arg::mptr(t1), joan::orig::arg::f32(7.957746982574463f * atm->scale_a[1]), joan::orig::arg::ptr(atm->vec_18))));  // 0x9d9d34
        if (lookup(d::g_a195b4)) set_constant(static_cast<std::uint32_t>(s.reg), s.c6c, 1);           // 4th = c60[0]
        if (lookup(d::g_a195d4)) set_constant(static_cast<std::uint32_t>(s.reg), atm->vec_0, 1);  // +0x20
        s.c7c[0] = 0.0f;
        std::memcpy(&s.c7c[1], &atm->scale_a[2], 4);  // +0x74
        s.c7c[2] = 0.0f;
        s.c7c[3] = 0.0f;
        if (lookup(d::g_a195f4)) set_constant(static_cast<std::uint32_t>(s.reg), s.c7c, 1);
        com_call(game_device(), kGetTransform, 0x10u, static_cast<void*>(s.texture));
        import_call<void*>(d::iat_v3d_87_matrix_transpose_1, s.texture);  // transpose
        if (lookup(d::strs_a19614)) set_constant(static_cast<std::uint32_t>(s.reg), s.texture, 1);
        if (lookup(d::strs_a19634)) set_constant(static_cast<std::uint32_t>(s.reg), s.texture + 4, 1);
        jo::self<jo::VsCache>(this)->u32_0 = 0;  // dirty flag (+0)
    }
};

#define HELPER26(name, addr, fn) JOAN_REPLACE("en3d/helpers26", name, addr, fn)
HELPER26("VertexShaderFactory::set_shader_light_constants", 0x0061c390, &H::vs_setup_skin);
HELPER26("VertexShaderFactory::set_scattering_shader_constant", 0x0061d240, &H::scatter_set_constants);
#undef HELPER26

}  // namespace en3d_helpers25
}  // namespace

// ---------------------------------------- direct calls (re/bind/gen_bridges.py)
auto joan::orig::VsCache::create(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers20::H::vs_create>(this, a0)); }  // 0x618590
auto joan::orig::VsCache::fragments(void* a0, void* a1) -> void { joan::bind::member<&::en3d_helpers28::H::vs_fragments>(this, a0, a1); }  // 0x618e40
auto joan::orig::VsCache::set_matrices() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers18::H::vs_set_matrices>(this)); }  // 0x61aba0
auto joan::orig::VsCache::setup_skin(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers25::H::vs_setup_skin>(this, a0)); }  // 0x61c390
auto joan::orig::VsCache::set_scatter_constants(void* a0) -> void { joan::bind::member<&::en3d_helpers25::H::scatter_set_constants>(this, a0); }  // 0x61d240

// ---------------------------------------- from modules/en3d_helpers8.cpp (en3d/helpers8)
namespace {
namespace en3d_helpers8 {

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {  // COM method by vtable index
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
Device* game_device() { return global<Device*>(d::g_game_device); }

struct H {
    // 0x0061a8c0: make an entry current - its shader setup (0x0061c390 for type 7, else
    // 0x0061aa50 / 0x0061aba0 then 0x0061d240 (flags & 0x40) or 0x0061ae30) and, with flags & 2,
    // the fog constant {start, end, 1 / (end - start), -start / (end - start)} at the register
    // its constant table (+0xc) gives for the name at 0xa19274.
    void vs_touch(std::uint32_t* e) {
        if (static_cast<std::int8_t>(*static_cast<const std::uint8_t*>(jo::as_vs_entry(e)->key_format)) == 7) {
            joan::orig::as_vs_cache(this)->setup_skin(joan::orig::arg::mptr(e));
        } else {
            joan::orig::as_sys_en(this)->vs_material_constants();
            joan::orig::as_vs_cache(this)->set_matrices();
            if ((jo::as_vs_entry(e)->key_flags & 0x40) != 0)
                joan::orig::as_vs_cache(this)->set_scatter_constants(joan::orig::arg::mptr(e));
            else
                joan::orig::as_vs_cache(this)->set_light_constants(joan::orig::arg::mptr(e));
        }
        if ((jo::as_vs_entry(e)->key_flags & 2) == 0) return;
        void* const table = jo::as_vs_entry(e)->constant_table;
        std::uint32_t handle = 0;
        com_call(table, 5, global<const char*>(d::g_a19274), &handle);
        std::int32_t reg = 0;
        com_call(table, 16, handle, 0u, &reg);
        if (reg == -1) return;
        float start = 0.0f, end = 0.0f;
        game_device()->GetRenderState(36, reinterpret_cast<joan::d3d8::DWORD*>(&start));  // FOGSTART
        game_device()->GetRenderState(37, reinterpret_cast<joan::d3d8::DWORD*>(&end));    // FOGEND
        const float scale = 1.0f / (end - start);
        const float c[4] = {start, end, scale, -start * scale};
        game_device()->SetVertexShaderConstant(static_cast<joan::d3d8::DWORD>(reg), c, 1);
    }
};

#define HELPER8(name, addr, fn) JOAN_REPLACE("en3d/helpers8", name, addr, fn)
HELPER8("VertexShaderFactory::set_shader_constants", 0x0061a8c0, &H::vs_touch);
#undef HELPER8

}  // namespace en3d_helpers8
}  // namespace

// ---------------------------------------- from modules/en3d_helpers22.cpp (en3d/helpers22)
namespace {
namespace en3d_helpers22 {

struct H {
    // 0x00618bb0 VsCache::declaration(out, format): fixed declarations for formats 5 and 7,
    // else D3DXDeclaratorFromFVF.
    std::intptr_t vs_declaration(std::uint32_t* out, const std::uint8_t* format) {
        const std::int8_t type = static_cast<std::int8_t>(format[0]);
        if (type == 5) {
            static const std::uint32_t kDecl[] = {0x20000000, 0x40020000, 0x40020003, 0x40040005, 0x40010007,
                                                  0x40020008, 0x40020009, 0x4002000a, 0xffffffff};
            std::memcpy(out, kDecl, sizeof(kDecl));
            return reinterpret_cast<std::intptr_t>(out);
        }
        if (type == 7) {
            static const std::uint32_t kDecl[] = {0x20000000, 0x40020000, 0x40020001, 0x40020008, 0x40020009,
                                                  0x4006000c, 0x4001000d, 0x40040005, 0x40010007, 0xffffffff};
            std::memcpy(out, kDecl, sizeof(kDecl));
            return reinterpret_cast<std::intptr_t>(out);
        }
        return joan::orig::arg::as<std::int32_t>(joan::orig::d3dx::declarator_from_fvf(joan::orig::arg::u32(jo::self<jo::VertexFormat>(format)->fvf), joan::orig::arg::mptr(out)));
    }
};

#define HELPER22(name, addr, fn) JOAN_REPLACE("en3d/helpers22", name, addr, fn)
HELPER22("VertexShaderFactory::generate_declaration", 0x00618bb0, &H::vs_declaration);
#undef HELPER22

}  // namespace en3d_helpers22
}  // namespace

// ---------------------------------------- from modules/game_b111_11.cpp (game/b111_11)
namespace {
namespace game_b111_11 {

template <class R = void, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
constexpr u32 kOne = 0x3f800000;  // 1.0f bits

JOAN_COUNTER(c_vsmat, "SysEn::vs_material_constants");

constexpr std::int32_t kGetMaterial = 43, kSetVertexShaderConstant = 79;
void* game_device() { return global<void*>(d::g_game_device); }
void set_vs_constant(u32 reg, const void* data, u32 count) {
    com_call<i32>(game_device(), kSetVertexShaderConstant, reg, data, count);
}

struct H {
    // ------------------------------------------------------------------ 0x0061aa50
    void vs_material_constants() {
        c_vsmat.hit();
        float c0[4];  // [ebp-0x58]
        set_vs_constant(0, joan::orig::arg::as<void*>(joan::orig::as_vector4(c0)->construct_xyzw(0u, 0x3f000000u, joan::orig::arg::u32(kOne), 0x3fb8aa3bu)), 1);  // 0x005bd220
        float mat[17];  // [ebp-0x48] D3DMATERIAL8: diffuse, ambient, specular, emissive, power
        com_call<i32>(game_device(), kGetMaterial, static_cast<void*>(mat));
        set_vs_constant(0xb, &mat[4], 1);   // ambient
        set_vs_constant(0xc, &mat[0], 1);   // diffuse
        set_vs_constant(0x14, &mat[8], 1);  // specular
        set_vs_constant(0x15, &mat[12], 1); // emissive
        const u32 power = jo::view<u32>(mat[16]);
        float c22[4];  // [ebp-0x68]
        set_vs_constant(0x16, joan::orig::arg::as<void*>(joan::orig::as_vector4(c22)->construct_xyzw(joan::orig::arg::u32(power), joan::orig::arg::u32(power), joan::orig::arg::u32(power), joan::orig::arg::u32(power))), 1);
        jo::self<jo::VsCache>(this)->u32_4 = 0;
    }
};

#define G(name, addr, fn) JOAN_REPLACE("game/b111_11", name, addr, fn)
G("VertexShaderFactory::set_shader_common_constants", 0x0061aa50, &H::vs_material_constants);
#undef G

}  // namespace game_b111_11
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::VsCache::declaration(void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers22::H::vs_declaration>(this, a0, a1)); }  // 0x618bb0
auto joan::orig::VsCache::touch(void* a0) -> void { joan::bind::member<&::en3d_helpers8::H::vs_touch>(this, a0); }  // 0x61a8c0
auto joan::orig::SysEn::vs_material_constants() -> void { joan::bind::member<&::game_b111_11::H::vs_material_constants>(this); }  // 0x61aa50
