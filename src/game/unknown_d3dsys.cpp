// game/unknown_d3dsys.cpp: functions whose original source file is unknown (formerly game/_unplaced/d3dsys.cpp;
// re/attribution.csv: between ww/en3d/sys3d.cpp and ww/en3d/sys3d.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>
#include "../compiler.hpp"
#include <nmmintrin.h>
#include <chrono>
#include <unordered_map>
#include <unordered_set>

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
#include "../render/nvlink_com.hpp"
#include "../render/water.hpp"
#include "../render/nvlink_log.hpp"
#include "../canvas_api.hpp"
#include "../render/overrides.hpp"
#include "../d3d8/d3d8_types.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "stream_cache_api.hpp"
#include "../render/grass_batch.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::str;

using joan::call::cdecl_call;

using joan::orig::helpers::op_delete;

using i8 = std::int8_t;

using joan::call::stdcall_call;

template <class R = void, class... A>
R com(void* object, std::int32_t offset, A... args) {  // COM vtable call (stdcall, object first)
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}

// A render log record (SysEn::frame_timer pool, 0x25 bytes): kind 1 = texture name, 2 = primitive count.
#pragma pack(push, 1)
struct RenderLogEntry {
    u32 kind;  // 0x0
    union {
        char name[0x21];  // 0x4 (kind 1)
        i32 value;        // 0x4 (kind 2)
    };
};
#pragma pack(pop)
static_assert(sizeof(RenderLogEntry) == 0x25, "RenderLogEntry");

}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

using joan::call::thiscall;
using joan::orig::helpers::crt_memset;

struct H {
    H* sdtor_5f6ac0(u32 flags) {
        joan::orig::as_vs_cache(this)->release_all();  // 0x00618090
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }

    // ------------------------------------------------------------------ memset ctors
    H* clear_337() {  // 0x005f65b0
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::SysEn)));  // 0x337 bytes on x86
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Obj::clear_337", 0x005f65b0, &H::clear_337);
B("Obj::sdtor_5f6ac0", 0x005f6ac0, &H::sdtor_5f6ac0);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b102_11.cpp (game/b102_11)
namespace {
namespace game_b102_11 {


struct H {
    // 0x005f65f0 (ret)
    u32 shutdown_wrapper() {
        joan::orig::as_sys_en(this)->shutdown();  // 0x005f6770
        return 0;  // 0x005f6770 returns nothing; the atexit caller (0x00564050) ignores the result
    }

};

#define B11(name, addr, fn) JOAN_REPLACE("game/b102_11", name, addr, fn)
B11("Renderer::shutdown_wrapper", 0x005f65f0, &H::shutdown_wrapper);
#undef B11

}  // namespace game_b102_11
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

using joan::call::thiscall;
using joan::orig::helpers::game_new;  // operator new

JOAN_COUNTER(c_create, "Obj::create_5f6620");

struct H {
    // 0x005f6620
    i32 create_5f6620() {
        c_create.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_d3d(this)->init_5f6b60()) == 0) return 0;  // 0x005f6b60
        if (joan::orig::arg::as<i32>(joan::orig::as_sys_en(this)->start_d3d()) == 0) return 0;  // 0x005f6c00
        jo::self<jo::SysEn>(this)->dot3_vertex_shader = 0;
        jo::self<jo::SysEn>(this)->dot3_created = 1;
        void* const m1 = game_new(static_cast<std::uint32_t>(sizeof(jo::VsCache)));  // 0x80 on x86
        void* const p1 = m1 != nullptr ? joan::orig::as_vs_cache(m1)->construct() : nullptr;  // 0x00618030
        jo::view<void*>(jo::self<jo::SysEn>(this)->vs_cache) = p1;
        void* const m2 = game_new(static_cast<std::uint32_t>(sizeof(jo::PixelShaderCache)));  // 0xc on x86
        void* const p2 = m2 != nullptr ? joan::orig::as_pixel_shader_cache(m2)->construct() : nullptr;  // 0x005e5930
        jo::view<void*>(jo::self<jo::SysEn>(this)->ps_cache) = p2;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Obj::create_5f6620", 0x005f6620, &H::create_5f6620);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b101_11.cpp (game/b101_11)
namespace {
namespace game_b101_11 {

using joan::call::thiscall;
// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}
JOAN_COUNTER(c_shutdown, "Renderer::shutdown");

struct H {
    // ------------------------------------------------------------------ 0x005f6770
    void shutdown() {
        c_shutdown.hit();
        if (jo::self<jo::SysEn>(this)->dot3_created != 0) {
            if (void* const p = jo::view<void*>(jo::self<jo::SysEn>(this)->frame_timer)) joan::orig::as_dynarray(p)->destroy(1u);  // 0x00463fc0
            jo::self<jo::SysEn>(this)->frame_timer = nullptr;
            if (void* const p = jo::view<void*>(jo::self<jo::SysEn>(this)->prev_frame_timer)) joan::orig::as_dynarray(p)->destroy(1u);
            jo::self<jo::SysEn>(this)->prev_frame_timer = nullptr;
            joan::orig::as_sys_en(this)->release_buffers();  // 0x005f7f60
            joan::orig::as_render_target(this)->release_depth();  // 0x00603380
            joan::orig::as_sys_en(this)->release_323();  // 0x006034b0
            if (jo::self<jo::SysEn>(this)->dot3_vertex_shader != 0) com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0x138, jo::self<jo::SysEn>(this)->dot3_vertex_shader);  // DeleteVertexShader
            if (jo::self<jo::SysEn>(this)->depth_stencil != nullptr) {
                com_call<u32>(jo::view<void*>(jo::self<jo::SysEn>(this)->depth_stencil), 0x8);  // Release
                jo::self<jo::SysEn>(this)->depth_stencil = nullptr;
            }
            if (jo::self<jo::SysEn>(this)->back_buffer != nullptr) {
                com_call<u32>(jo::view<void*>(jo::self<jo::SysEn>(this)->back_buffer), 0x8);
                jo::self<jo::SysEn>(this)->back_buffer = nullptr;
            }
            if (jo::self<jo::SysEn>(this)->vs_cache != nullptr) {
                if (void* const p = jo::view<void*>(jo::self<jo::SysEn>(this)->vs_cache)) joan::orig::as_vs_cache(p)->destroy(1u);  // 0x005f6ac0
                jo::self<jo::SysEn>(this)->vs_cache = nullptr;
            }
            if (jo::self<jo::SysEn>(this)->ps_cache != nullptr) {
                if (void* const p = jo::view<void*>(jo::self<jo::SysEn>(this)->ps_cache)) joan::orig::as_pshader_list(p)->destroy(1u);  // 0x005f6b10
                jo::self<jo::SysEn>(this)->ps_cache = nullptr;
            }
            jo::self<jo::SysEn>(this)->dot3_created = 0;
        }
        if (jo::self<jo::SysEn>(this)->device != nullptr) {
            com_call<u32>(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0x8);
            jo::self<jo::SysEn>(this)->device = nullptr;
        }
        if (jo::self<jo::SysEn>(this)->matrix_stack != nullptr) {
            com_call<u32>(jo::view<void*>(jo::self<jo::SysEn>(this)->matrix_stack), 0x8);
            jo::self<jo::SysEn>(this)->matrix_stack = nullptr;
        }
        if (jo::self<jo::SysEn>(this)->d3d != nullptr) {
            com_call<u32>(jo::view<void*>(jo::self<jo::SysEn>(this)->d3d), 0x8);
            jo::self<jo::SysEn>(this)->d3d = nullptr;
        }
        if (jo::self<jo::SysEn>(this)->dot3_buffer_b != nullptr) {
            op_delete(jo::view<void*>(jo::self<jo::SysEn>(this)->dot3_buffer_b));
            jo::self<jo::SysEn>(this)->dot3_buffer_b = nullptr;
        }
        if (jo::self<jo::SysEn>(this)->dot3_buffer_a != nullptr) {
            op_delete(jo::view<void*>(jo::self<jo::SysEn>(this)->dot3_buffer_a));
            jo::self<jo::SysEn>(this)->dot3_buffer_a = nullptr;
        }
        jo::view<u32>(jo::self<jo::SysEn>(this)->started) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_11", name, addr, fn)
B("Renderer::shutdown", 0x005f6770, &H::shutdown);
#undef B

}  // namespace game_b101_11
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x005f6b10
    H* scalar_dtor_5f6b10(u32 flags) {
        joan::orig::as_pixel_shader_cache(this)->destruct();  // 0x005e5960
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Obj::scalar_dtor_5f6b10", 0x005f6b10, &H::scalar_dtor_5f6b10);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

JOAN_COUNTER(c_d3d_init, "D3D::init_5f6b60");

struct H {
    // ------------------------------------------------------------------ 0x005f6b60
    u32 d3d_init() {
        c_d3d_init.hit();
        if (jo::self<jo::SysEn>(this)->started >= 3) return 1;
        if (jo::self<jo::SysEn>(this)->started < 1) {
            jo::self<jo::SysEn>(this)->d3d = static_cast<jo::IDirect3D8*>(joan::orig::d3d8::direct3dcreate8(joan::orig::arg::u32(u32{0x78})));  // Direct3DCreate8 stub
            if (jo::self<jo::SysEn>(this)->d3d == nullptr) return 0x80004005u;
            joan::orig::d3dx::create_matrix_stack(joan::orig::arg::u32(u32{0}), joan::orig::arg::mptr(&jo::self<jo::SysEn>(this)->matrix_stack));
            jo::self<jo::SysEn>(this)->started = 1;
        }
        if (jo::self<jo::SysEn>(this)->started < 2) jo::self<jo::SysEn>(this)->started = 2;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("D3D::init_5f6b60", 0x005f6b60, &H::d3d_init);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

using joan::img::obj;

using joan::call::thiscall;
// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    using Fn = R(JOAN_STDCALL*)(void*, A...);
    return reinterpret_cast<Fn>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}
JOAN_COUNTER(c_cull, "SysEn::set_cull_flip");

struct H {
    // 0x00603140 SysEn::set_cull_flip(flip)
    void set_cull(std::int32_t flip) {
        c_cull.hit();
        if (jo::self<jo::SysEn>(this)->mirrored == flip) return;
        jo::self<jo::SysEn>(this)->mirrored = flip;
        if (jo::self<jo::SysEn>(this)->dirty != 0) return;
        const std::uint32_t mode = jo::self<jo::SysEn>(this)->cur_raster & 0xf;
        std::uint32_t cull;
        if (jo::self<jo::SysEn>(this)->mirrored == 0) {
            if (mode == 0) cull = 2;
            else if (mode == 1) cull = 3;
            else if (mode == 2) cull = 1;
            else return;
        } else {
            if (mode == 0) cull = 3;
            else if (mode == 1) cull = 2;
            else if (mode == 2) cull = 1;
            else return;
        }
        com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0xc8, 0x16u, cull);  // SetRenderState(D3DRS_CULLMODE)
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("SysEn::set_cull_flip", 0x00603140, &H::set_cull);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
namespace {
namespace game_b92_04 {

JOAN_COUNTER(c_sound_release, "Sound3d::release");

struct H {
    // ------------------------------------------------------------------------ 0x005f7f60
    void sound_release() {
        c_sound_release.hit();
        if (jo::view<void*>(jo::self<jo::SysEn>(this)->dyn_ib) != nullptr) {
            com<u32>(jo::view<void*>(jo::self<jo::SysEn>(this)->dyn_ib), 8);  // Release
            jo::self<jo::SysEn>(this)->dyn_ib = nullptr;
        }
        for (std::int8_t i = 0; static_cast<i32>(i) < 9; i = static_cast<std::int8_t>(i + 1)) {
            if (jo::view<void*>(jo::self<jo::SysEn>(this)->dyn_vb[i]) != nullptr) {
                com<u32>(jo::view<void*>(jo::self<jo::SysEn>(this)->dyn_vb[i]), 8);  // Release
                jo::self<jo::SysEn>(this)->dyn_vb[i] = nullptr;
            }
        }
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Sound3d::release", 0x005f7f60, &H::sound_release);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16 {

using joan::hook::global;

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
JOAN_COUNTER(c_light_slot, "SysEn::light_slot");

struct H {
    // --- en3d/helpers16 ---------------------------------------------------------------------
    void light_slot_free(std::int32_t) {}  // 0x005f80f0
    // 0x005f8030 SysEn::light_slot: first device light that is not enabled (or a new one).
    std::uint32_t light_slot() {
        c_light_slot.hit();
        std::uint32_t i = 0;
        for (; i < jo::self<jo::SysEn>(this)->light_slot_count; ++i) {
            std::int32_t enabled;
            const std::int32_t hr = com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 47, i, &enabled);  // GetLightEnable
            if (hr != 0 || enabled == 0) break;
        }
        if (!(i < jo::self<jo::SysEn>(this)->light_slot_count)) {
            i = jo::self<jo::SysEn>(this)->light_slot_count;
            jo::self<jo::SysEn>(this)->light_slot_count = jo::self<jo::SysEn>(this)->light_slot_count + 1;
        }
        return i;
    }
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
HELPER16("SysEn::light_slot_free", 0x005f80f0, &H::light_slot_free);
HELPER16("SysEn::light_slot", 0x005f8030, &H::light_slot);
#undef HELPER16

}  // namespace en3d_helpers16
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
namespace {
namespace game_b92_05 {

using joan::hook::global;

// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}
JOAN_COUNTER(c_default_states, "SysEn::set_default_states");

struct H {
    void* dev() { return jo::self<jo::SysEn>(this)->device; }

    // 0x005f8110
    void set_default_states() {
        c_default_states.hit();
        com_call(dev(), 0xc8, 0x16, 2);  // SetRenderState
        for (std::uint32_t i = 0; i < jo::self<jo::SysEn>(this)->caps.max_simultaneous_textures; ++i) {
            com_call(dev(), 0xfc, i, 0x10, 2);  // SetTextureStageState
            com_call(dev(), 0xfc, i, 0x11, 2);
            com_call(dev(), 0xfc, i, 0x12, 2);
        }
        com_call(dev(), 0xc8, 0x18, 0x80);
        com_call(dev(), 0xc8, 0x19, 7);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("SysEn::set_default_states", 0x005f8110, &H::set_default_states);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/en3d_helpers22.cpp (en3d/helpers22)
namespace {
namespace en3d_helpers22 {

struct H {
    std::uint32_t sysen_vs_version() { return jo::self<jo::SysEn>(this)->cur_blend; }             // 0x005f8220
};

#define HELPER22(name, addr, fn) JOAN_REPLACE("en3d/helpers22", name, addr, fn)
HELPER22("SysEn::vs_version", 0x005f8220, &H::sysen_vs_version);
#undef HELPER22

}  // namespace en3d_helpers22
}  // namespace

// ---------------------------------------- from modules/sysen_state.cpp (en3d/sysen.cpp)
namespace {
namespace sysen_state {

using joan::img::obj;

using joan::hook::global;
using namespace joan::d3d8;  // NOLINT: enum names
using Device = IDirect3DDevice8;

using joan::call::thiscall;
// v3d_87.dll / v3d_sse.dll in-place math through joan.exe's import table.
template <class... A>
void v3d(img::Addr iat_slot, A... args) {
    reinterpret_cast<void*(JOAN_CDECL*)(A...)>(global<std::uintptr_t>(iat_slot))(args...);
}
constexpr img::Addr kMatrixTransposeInPlace = d::iat_v3d_87_matrix_transpose_1;  // (m): m = transpose(m)
constexpr img::Addr kMatrixMultiplyInPlace = d::iat_v3d_87_matrix_multiply_2;   // (a, b): a = a * b

constexpr img::Addr kIdentity(d::g_identity_matrix);
// Camera-space reflection / normal -> texture coordinates (0x00a17e18, 0x00a17e58).
constexpr img::Addr kSphereMap(d::g_sphereMap);
constexpr img::Addr kNormalMap(d::g_normalMap);
constexpr img::Addr kNormalMapB(d::g_normalMapB);  // same values as 0x00a17e58
constexpr img::Addr kAnimDb(d::g_texture_effect_res);  // animated-texture table (0x004b27b0 = entry)
std::uint32_t now_ms() { return global<std::uint32_t>(d::g_now); }

// x87 `fcomp 0.0; test ah,0x40` is also true for NaN (unordered sets C3).
bool x87_is_zero(float f) { return f == 0.0f || f != f; }

// Pending flags (+0x15b) - named by their effect in apply().
constexpr std::uint32_t kFlagDot3 = 0x01;          // DOT3 bump mapping (blend 8/9, texture 9, vertex type 5)
constexpr std::uint32_t kFlagOverlayA = 0x04;      // overlay texture (both A and B needed)
constexpr std::uint32_t kFlagOverlayB = 0x08;
constexpr std::uint32_t kFlagBlend15 = 0x20;       // allows blend mode 15
constexpr std::uint32_t kFlagTextureMode15 = 0x40; // texture modes 15 / 16 (exclusive)
constexpr std::uint32_t kFlagTint = 0x80;          // texture-factor tint stage

struct SysEn {
    Device* dev() { return jo::view<Device*>(jo::self<jo::SysEn>(this)->device); }
    std::uint32_t misc_caps() { return jo::self<jo::SysEn>(this)->caps.primitive_misc_caps; }
    std::uint32_t max_blend_stages() { return jo::self<jo::SysEn>(this)->caps.max_texture_blend_stages; }
    std::uint32_t max_textures() { return jo::self<jo::SysEn>(this)->caps.max_simultaneous_textures; }

    std::int8_t& cur_vertex_type() { return jo::self<jo::SysEn>(this)->cur_vertex_type; }
    std::int8_t& new_vertex_type() { return jo::self<jo::SysEn>(this)->new_vertex_type; }
    std::uint32_t& cur_blend() { return jo::self<jo::SysEn>(this)->cur_blend; }
    std::uint32_t& new_blend() { return jo::self<jo::SysEn>(this)->new_blend; }
    std::uint32_t& cur_texture() { return jo::self<jo::SysEn>(this)->cur_texture_mode; }
    std::uint32_t& new_texture() { return jo::self<jo::SysEn>(this)->new_texture_mode; }
    std::uint32_t& cur_filter() { return jo::self<jo::SysEn>(this)->cur_filter; }
    std::uint32_t& new_filter() { return jo::self<jo::SysEn>(this)->new_filter; }
    std::uint32_t& cur_flags() { return jo::self<jo::SysEn>(this)->cur_flags; }
    std::uint32_t& new_flags() { return jo::self<jo::SysEn>(this)->new_flags; }
    std::uint32_t& cur_tfactor() { return jo::view<std::uint32_t>(jo::self<jo::SysEn>(this)->cur_texture_factor); }
    std::uint32_t& new_tfactor() { return jo::view<std::uint32_t>(jo::self<jo::SysEn>(this)->new_texture_factor); }
    std::uint32_t& cur_raster() { return jo::self<jo::SysEn>(this)->cur_raster; }
    std::uint32_t& new_raster() { return jo::self<jo::SysEn>(this)->new_raster; }
    std::uint32_t& dirty() { return jo::view<std::uint32_t>(jo::self<jo::SysEn>(this)->dirty); }
    std::int32_t& cur_overlay() { return jo::self<jo::SysEn>(this)->cur_overlay_stage; }
    std::int32_t& new_overlay() { return jo::self<jo::SysEn>(this)->new_overlay_stage; }
    std::int32_t& cur_tint() { return jo::self<jo::SysEn>(this)->cur_tint_stage; }
    std::int32_t& new_tint() { return jo::self<jo::SysEn>(this)->new_tint_stage; }
    std::uint32_t dot3_usable() { return jo::self<jo::SysEn>(this)->dot3_usable; }
    std::uint32_t mirrored() { return jo::view<std::uint32_t>(jo::self<jo::SysEn>(this)->mirrored); }

    void rs(Enum state, DWORD value) { dev()->SetRenderState(state, value); }
    void tss(DWORD stage, Enum state, DWORD value) { dev()->SetTextureStageState(stage, state, value); }
    void disable_stage(DWORD stage) {
        tss(stage, kTssColorOp, kTopDisable);
        tss(stage, kTssAlphaOp, kTopDisable);
    }

    // Requests (thiscall, `ret 4`). A request equal to the applied state only
    // cancels a pending change; anything else marks the state dirty.
    void set_blend(std::uint32_t mode);
    void set_texture_mode(std::uint32_t mode);
    void set_filter(std::uint32_t mode);
    void set_flags(std::uint32_t flags);
    void set_raster(std::uint32_t mode);
    void set_vertex_type(char type);
    void set_texture_factor(std::uint32_t color);
    std::uint32_t flags();
    std::int32_t overlay_stage();

    void apply(std::uint32_t anim_id);
    void apply_blend();
    void apply_texture();
    void apply_filter();
    void apply_raster();
    void multi_texture(std::uint32_t count);
    void apply_fixed_texture_mode(std::uint32_t mode);
    void apply_shaders();
};

// --- requests -----------------------------------------------------------------------
void SysEn::set_blend(std::uint32_t mode) {  // 0x005f8250
    if (mode == new_blend()) return;
    if (mode == cur_blend()) {
        new_blend() = mode;
        return;
    }
    new_blend() = mode;
    dirty() = 1;
}
void SysEn::set_texture_mode(std::uint32_t mode) {  // 0x005fa250
    if (mode == new_texture()) return;
    if (mode == cur_texture()) {
        new_texture() = mode;
        return;
    }
    new_texture() = mode;
    dirty() = 1;
}
void SysEn::set_filter(std::uint32_t mode) {  // 0x005fec10
    if (mode == new_filter()) return;
    if (mode == cur_filter()) {
        new_filter() = mode;
        return;
    }
    new_filter() = mode;
    dirty() = 1;
}
void SysEn::set_flags(std::uint32_t flags) {  // 0x00602080
    if (flags == new_flags()) return;
    if (flags == cur_flags()) {
        new_flags() = flags;
        return;
    }
    new_flags() = flags;
    dirty() = 1;
}
void SysEn::set_raster(std::uint32_t mode) {  // 0x006026a0
    if (mode == new_raster()) return;
    if (mode == cur_raster()) {
        new_raster() = mode;
        return;
    }
    new_raster() = mode;
    dirty() = 1;
}
void SysEn::set_vertex_type(char type) {  // 0x006024e0
    if (new_vertex_type() == static_cast<std::int8_t>(type)) return;
    new_vertex_type() = static_cast<std::int8_t>(type);
    dirty() = 1;
}
void SysEn::set_texture_factor(std::uint32_t color) {  // 0x005feb50 (compares with the applied colour only)
    if (cur_tfactor() == color) return;
    new_tfactor() = color;
    dirty() = 1;
}
std::uint32_t SysEn::flags() { return cur_flags(); }          // 0x006020f0
std::int32_t SysEn::overlay_stage() { return cur_overlay(); }  // 0x00601470

// --- apply ------------------------------------------------------------------------------
// 0x006014d0. `anim_id` = the material's animated-texture entry (0 = none).
void SysEn::apply(std::uint32_t anim_id) {
    if (dirty() == 0) {
        apply_shaders();
        return;
    }
    // (0x00600200 is called for the vertex format here; it is a pure lookup and its result unused.)
    std::uint32_t& flags = new_flags();
    if ((flags & kFlagTextureMode15) && new_texture() != 15 && new_texture() != 16) flags &= ~kFlagTextureMode15;
    if (new_texture() == 16 || new_texture() == 15) flags &= ~0x0eu;
    if (flags & kFlagTextureMode15) flags = kFlagTextureMode15;
    if ((flags & kFlagBlend15) && new_blend() != 15) flags &= ~kFlagBlend15;
    if (new_blend() == 15 && !(flags & kFlagBlend15)) new_blend() = 0;
    if ((new_texture() & 0xff000000u) == 0x01000000u) flags &= ~kFlagTint;
    if ((flags & kFlagTint) && new_tfactor() == 0xffffffffu) flags &= ~kFlagTint;  // white tint = no tint

    // Leaving an animated-texture mode: texture transforms off (unless the next material animates too).
    if ((cur_texture() & 0xff000000u) == 0x01000000u && anim_id == 0)
        for (DWORD stage = 0; stage < 4; ++stage) tss(stage, kTssTextureTransformFlags, kTtffDisable);
    if (cur_texture() == 0x10) tss(0, kTssTextureTransformFlags, kTtffDisable);

    if (dot3_usable() != 0 && (new_blend() == 8 || new_blend() == 9) && new_texture() == 9 && new_vertex_type() == 5 &&
        (flags & kFlagDot3)) {
        // DOT3 bump mapping: overlay on stage 2, tint on stage 2 or 3.
        if (((flags & kFlagOverlayA) || (flags & kFlagOverlayB)) && max_textures() >= 3) {
            new_overlay() = 2;
        } else {
            flags &= ~(kFlagOverlayA | kFlagOverlayB);
            new_overlay() = -1;
        }
        if ((flags & kFlagTint) && max_textures() >= 4) {
            new_tint() = 2;
        } else {
            flags &= ~kFlagTint;
            new_tint() = -1;
        }
        if (flags & kFlagTint) new_tint() = 3;
    } else {
        if (new_blend() == 8) new_blend() = 0;
        if (new_blend() == 9) new_blend() = 1;
        if (new_texture() == 9) new_texture() = 1;
        if (flags & kFlagDot3) flags &= ~kFlagDot3;
        // The stage after the texture mode's own stages takes the overlay / tint.
        const std::uint32_t mode = new_texture();
        if (mode & 0xff000000u) {
            if ((mode & 0xff000000u) == 0x01000000u) new_overlay() = new_tint() = static_cast<std::int32_t>(mode & 0xffff);
        } else if (mode & 0xffff0000u) {
            if ((mode & 0x10000u) || new_blend() == 3 || new_blend() == 4) {
                new_overlay() = -1;
                new_tint() = 1;
            } else if (mode & 0x20000u) {
                new_overlay() = 1;
                new_tint() = 1;
            }
        } else {
            static constexpr std::int32_t kNextStage[15] = {0, 1, 2, 3, 4, 2, 2, -1, 2, -1, -1, -1, -1, 2, 1};
            const std::int32_t stage = mode < 15 ? kNextStage[mode] : -1;
            new_overlay() = new_tint() = stage;
        }
        if (new_overlay() == -1)
            flags &= ~(kFlagOverlayA | kFlagOverlayB);
        else if (!(flags & kFlagOverlayA) || !(flags & kFlagOverlayB))
            new_overlay() = -1;
        if (new_tint() == -1)
            flags &= ~kFlagTint;
        else if (!(flags & kFlagTint))
            new_tint() = -1;
        if (static_cast<std::uint32_t>(new_overlay() + 1) > max_textures()) {
            new_overlay() = -1;
            flags &= ~(kFlagOverlayA | kFlagOverlayB);
        }
    }

    if (new_vertex_type() != cur_vertex_type() || new_blend() != cur_blend() || new_texture() != cur_texture() ||
        new_filter() != cur_filter() || new_flags() != cur_flags() || new_overlay() != cur_overlay() ||
        new_tint() != cur_tint() || new_tfactor() != cur_tfactor() || new_raster() != cur_raster()) {
        apply_blend();
        apply_texture();
        apply_filter();
        apply_raster();
        dirty() = 0;
    }
    apply_shaders();
}

// 0x00602120: the vertex shader (FVF or a cached vs.1.x program for the vertex type, blend
// and texture modes and flags), vertex blending on / off, and the pixel shader. The
// original's SEH frame only guards the cache's construction (`new` + constructor).
void SysEn::apply_shaders() {
    if (new_vertex_type() == cur_vertex_type() && cur_flags() == new_flags()) return;
    const std::int8_t old_type = cur_vertex_type();
    cur_vertex_type() = new_vertex_type();
    cur_flags() = new_flags();
    if (jo::view<void*>(jo::self<jo::SysEn>(this)->vs_cache) == nullptr) {
        void* const memory = reinterpret_cast<void*(JOAN_CDECL*)(std::uint32_t)>(fn::code_value(fn::op_new))(static_cast<std::uint32_t>(sizeof(jo::VsCache)));  // operator new (0x80 on x86)
        jo::view<void*>(jo::self<jo::SysEn>(this)->vs_cache) = memory ? joan::orig::as_vs_cache(memory)->construct() : nullptr;                   // 0x00618030
    }
    auto fvf_of = [](std::int8_t type) {
        const auto* const format = reinterpret_cast<const std::uint8_t*(JOAN_CDECL*)(char)>(fn::code_value(fn::Sys3D_get_vertex_type_info))(static_cast<char>(type));
        return static_cast<DWORD>(jo::as_vertex_format(format)->fvf);  // 0x00600200 vertex format
    };
    if (void* const vs_cache = jo::view<void*>(jo::self<jo::SysEn>(this)->vs_cache)) {
        void* const entry = joan::orig::arg::as<void*>(joan::orig::as_vs_cache(vs_cache)->lookup(joan::orig::arg::i32(static_cast<char>(cur_vertex_type())), joan::orig::arg::u32(cur_blend()), joan::orig::arg::u32(cur_texture()), joan::orig::arg::u32(cur_flags())));  // 0x00618200
        if (entry == nullptr) {
            dev()->SetVertexShader(fvf_of(new_vertex_type()));
            jo::self<jo::SysEn>(this)->vs_bound = 0;
        } else {
            dev()->SetVertexShader(static_cast<DWORD>(jo::as_vs_entry(entry)->u32_10));  // the shader handle
            joan::orig::as_vs_cache(vs_cache)->touch(joan::orig::arg::mptr(entry));  // 0x0061a8c0
            jo::self<jo::SysEn>(this)->vs_bound = 1;
        }
    } else {
        dev()->SetVertexShader(fvf_of(new_vertex_type()));
    }
    auto* const global_dev = global<Device*>(d::g_game_device);
    if (cur_vertex_type() != old_type && global<std::uint32_t>(d::g_a52518) >= 2) {  // 2+ blend matrices
        if (cur_vertex_type() == 8 && old_type != 8) global_dev->SetRenderState(kRsVertexBlend, 1);
        if (cur_vertex_type() != 8 && old_type == 8) global_dev->SetRenderState(kRsVertexBlend, 0);
    }
    if (void* const ps_cache = jo::view<void*>(jo::self<jo::SysEn>(this)->ps_cache)) {
        void* const entry = joan::orig::arg::as<void*>(joan::orig::as_pixel_shader_cache(ps_cache)->lookup(joan::orig::arg::i32(static_cast<char>(cur_vertex_type())), joan::orig::arg::u32(cur_texture())));  // 0x005e5a50
        if (entry != nullptr) {
            global_dev->SetPixelShader(static_cast<const jo::PixelShaderEntry*>(entry)->handle);
            joan::orig::as_pixel_shader_cache(ps_cache)->apply(joan::orig::arg::mptr(entry), joan::orig::arg::u32((cur_flags() & kFlagTint) != 0 ? 1u : 0u));  // 0x005e5de0
        } else {
            global_dev->SetPixelShader(0);
        }
    }
}

// --- blend modes ----------------------------------------------------------------------
// 0x005f82e0: depth, lighting, alpha blending / testing and the stage-0 (to 2)
// texture addressing of each en3d blend mode.
void SysEn::apply_blend() {
    const std::uint32_t mode = new_blend();
    if (mode == cur_blend()) return;
    // Two-step setters used by most modes.
    auto common = [this](DWORD lighting, DWORD z, DWORD z_write) {
        rs(kRsLighting, lighting);
        rs(kRsZEnable, z);
        rs(kRsZWriteEnable, z_write);
    };
    auto address0 = [this](DWORD mode0) {
        tss(0, kTssAddressU, mode0);
        tss(0, kTssAddressV, mode0);
    };
    auto blend = [this](DWORD src, DWORD dst) {
        rs(kRsAlphaBlendEnable, 1);
        rs(kRsSrcBlend, src);
        rs(kRsDestBlend, dst);
    };
    auto tail = [this](DWORD alpha_test, DWORD specular) {
        rs(kRsAlphaTestEnable, alpha_test);
        rs(kRsLocalViewer, specular);
        rs(kRsSpecularEnable, specular);
    };
    if (mode & 0xffff0000u) {
        if ((mode & 0xffff0000u) == 0x10000u) {  // custom blend: 0x1DS (D = dest, S = source factor index)
            rs(kRsLighting, 0);
            rs(kRsZEnable, 1);
            rs(kRsZWriteEnable, 0);
            rs(kRsAlphaBlendEnable, 1);
            const DWORD* const factors = &img::global<const DWORD>(d::g_a17ee4);  // 0x00600c00
            rs(kRsSrcBlend, factors[mode & 0xf]);
            rs(kRsDestBlend, factors[(mode & 0xf0) >> 4]);
            tail(0, 0);
        }
    } else {
        switch (mode) {
        case 0:  // opaque, lit
            common(1, 1, 1), address0(kTaddressWrap), rs(kRsAlphaBlendEnable, 0), tail(0, 0);
            break;
        case 1:  // alpha blended + alpha test, lit
            common(1, 1, 1), address0(kTaddressWrap), blend(kBlendSrcAlpha, kBlendInvSrcAlpha), tail(1, 0);
            break;
        case 2:  // opaque, unlit, no depth
            common(0, 0, 0), address0(kTaddressWrap), rs(kRsAlphaBlendEnable, 0), tail(0, 0);
            break;
        case 3:  // alpha blended, unlit, no depth, clamped
            common(0, 0, 0), address0(kTaddressClamp), blend(kBlendSrcAlpha, kBlendInvSrcAlpha), tail(0, 0);
            break;
        case 4:  // additive, unlit, no depth, clamped
            common(0, 0, 0), address0(kTaddressClamp), blend(kBlendSrcAlpha, kBlendOne), tail(0, 0);
            break;
        case 5:  // opaque, unlit
            common(0, 1, 1), address0(kTaddressWrap), rs(kRsAlphaBlendEnable, 0), tail(0, 0);
            break;
        case 6:  // alpha blended + alpha test, unlit
            common(0, 1, 1), address0(kTaddressWrap), blend(kBlendSrcAlpha, kBlendInvSrcAlpha), tail(1, 0);
            break;
        case 7:  // additive + alpha test, unlit
            common(0, 1, 1), address0(kTaddressWrap), blend(kBlendSrcAlpha, kBlendOne), tail(1, 0);
            break;
        case 8:   // DOT3 opaque (stencil off, stages 0-2 wrap)
        case 9: { // DOT3 alpha blended
            common(0, 1, 1);
            rs(kRsStencilEnable, 0);
            if (mode == 8)
                rs(kRsAlphaBlendEnable, 0);
            else
                blend(kBlendSrcAlpha, kBlendInvSrcAlpha);
            tail(mode == 8 ? 0 : 1, 0);
            for (DWORD stage = 0; stage < 3; ++stage) {
                tss(stage, kTssAddressU, kTaddressWrap);
                tss(stage, kTssAddressV, kTaddressWrap);
            }
            break;
        }
        case 10:  // multiplied by source alpha (alpha test, local viewer, specular untouched)
            rs(kRsZEnable, 1);
            rs(kRsLighting, 1);
            rs(kRsZWriteEnable, 1);
            blend(kBlendSrcAlpha, kBlendZero);
            for (DWORD stage = 0; stage < 2; ++stage) {
                tss(stage, kTssAddressU, kTaddressWrap);
                tss(stage, kTssAddressV, kTaddressWrap);
            }
            break;
        case 11:  // additive + alpha test, unlit, no depth
            common(0, 0, 0), address0(kTaddressWrap), blend(kBlendSrcAlpha, kBlendOne), tail(1, 0);
            break;
        case 12:  // alpha blended, depth test without depth writes (addressing untouched)
            common(0, 1, 0), blend(kBlendSrcAlpha, kBlendInvSrcAlpha), tail(0, 0);
            break;
        case 13:  // invisible (depth only); the blend is skipped where COLORWRITEENABLE exists
            common(0, 1, 0);
            if (!(misc_caps() & 0x80)) blend(kBlendZero, kBlendOne);
            tail(0, 0);
            break;
        case 14:  // alpha blended, no depth (addressing untouched)
            common(0, 0, 0), blend(kBlendSrcAlpha, kBlendInvSrcAlpha), tail(0, 0);
            break;
        case 15:  // multiplicative (source colour), lit
            common(1, 1, 1), address0(kTaddressWrap), blend(kBlendSrcColor, kBlendZero), tail(0, 0);
            break;
        case 16:  // alpha blended + alpha test, lit, no depth
            common(1, 0, 0), address0(kTaddressWrap), blend(kBlendSrcAlpha, kBlendInvSrcAlpha), tail(1, 0);
            break;
        case 17:  // additive + alpha test, unlit, no depth
            common(0, 0, 0), address0(kTaddressWrap), blend(kBlendSrcAlpha, kBlendOne), tail(1, 0);
            break;
        case 18:  // opaque, lit, specular
        case 20:
            common(1, 1, 1), address0(kTaddressWrap), rs(kRsAlphaBlendEnable, 0), tail(0, 1);
            break;
        case 19:  // alpha blended + alpha test, depth test without writes
            common(0, 1, 0), address0(kTaddressWrap), blend(kBlendSrcAlpha, kBlendInvSrcAlpha), tail(1, 0);
            break;
        default:
            break;
        }
    }
    cur_blend() = new_blend();
}

// --- filter and raster modes --------------------------------------------------------------
// 0x005fec90: 0 = all stages bilinear; 1 = stage 0 point sampled; 2 = stage 1 without mip-maps.
void SysEn::apply_filter() {
    if (new_filter() == cur_filter()) return;
    const DWORD mip = global<std::int8_t>(d::g_a5804e) != 0 ? kTexfLinear : kTexfPoint;  // trilinear option
    auto linear = [&](DWORD first) {
        for (DWORD stage = first; stage < max_textures(); ++stage) {
            tss(stage, kTssMagFilter, kTexfLinear);
            tss(stage, kTssMinFilter, kTexfLinear);
            tss(stage, kTssMipFilter, mip);
        }
    };
    switch (new_filter()) {
    case 0:
        linear(0);
        break;
    case 1:
        tss(0, kTssMagFilter, kTexfPoint);
        tss(0, kTssMinFilter, kTexfPoint);
        tss(0, kTssMipFilter, kTexfPoint);
        linear(1);
        break;
    case 2:
        tss(0, kTssMagFilter, kTexfLinear);
        tss(0, kTssMinFilter, kTexfLinear);
        tss(0, kTssMipFilter, mip);
        tss(1, kTssMagFilter, kTexfLinear);
        tss(1, kTssMinFilter, kTexfLinear);
        tss(1, kTssMipFilter, kTexfNone);
        linear(2);
        break;
    default:
        break;
    }
    cur_filter() = new_filter();
}

// 0x00602750: 0x00F cull (0 back, 1 front, 2 none; swapped when mirrored),
// 0x0F0 depth bias 0-3, 0xF00 alpha reference (0 = 0x80, 1 = 4).
void SysEn::apply_raster() {
    if (new_raster() == cur_raster()) return;
    const std::uint32_t mode = new_raster();
    switch (mode & 0xf) {
    case 0:
        rs(kRsCullMode, mirrored() ? kCullCcw : kCullCw);
        break;
    case 1:
        rs(kRsCullMode, mirrored() ? kCullCw : kCullCcw);
        break;
    case 2:
        rs(kRsCullMode, kCullNone);
        break;
    default:
        break;
    }
    switch (mode & 0xf0) {
    case 0x00: rs(kRsZBias, 0); break;
    case 0x10: rs(kRsZBias, 1); break;
    case 0x20: rs(kRsZBias, 2); break;
    case 0x30: rs(kRsZBias, 3); break;
    default: break;
    }
    switch (mode & 0xf00) {
    case 0x000: rs(kRsAlphaRef, 0x80); break;
    case 0x100: rs(kRsAlphaRef, 4); break;
    default: break;
    }
    cur_raster() = new_raster();
}

// 0x005fe920: texture modes 1-4 - `count` textures blended by each one's alpha.
void SysEn::multi_texture(std::uint32_t count) {
    tss(0, kTssColorOp, kTopModulate);
    tss(0, kTssColorArg1, kTaTexture);
    tss(0, kTssColorArg2, kTaDiffuse);
    tss(0, kTssAlphaOp, kTopSelectArg1);
    tss(0, kTssAlphaArg1, kTaTexture);
    std::uint32_t stage = 1;
    for (; stage < count; ++stage) {
        tss(stage, kTssColorOp, kTopBlendTextureAlpha);
        tss(stage, kTssColorArg1, kTaTexture);
        tss(stage, kTssColorArg2, kTaCurrent);
        tss(stage, kTssAlphaOp, kTopAddSmooth);
        tss(stage, kTssAlphaArg1, kTaTexture);
        tss(stage, kTssAlphaArg2, kTaCurrent);
    }
    if (stage < max_blend_stages()) disable_stage(stage);
}

// --- texture (combiner) modes ------------------------------------------------------------
// One stage of an animated-texture table entry (0x004b27b0): per set (16 bytes
// per set, 4 stages) the stage's combiner, addressing, scroll and scale.
// table: one of TexScriptRec's [6 sets][4 stages] dword / float arrays.
template <class T>
float anim_float(const T (&table)[24], std::uint32_t set, DWORD stage) {
    float v;
    std::memcpy(&v, &table[set * 4 + stage], 4);
    return v;
}
template <class T>
DWORD anim_dword(const T (&table)[24], std::uint32_t set, DWORD stage) {
    DWORD v;
    std::memcpy(&v, &table[set * 4 + stage], 4);
    return v;
}

// The view matrix without translation, transposed, times `generator` - turns
// camera-space normals / reflection vectors into texture coordinates.
void camera_texgen_matrix(Device* dev, const float* generator, float out[16]) {
    std::memcpy(out, &img::global<const float>(kIdentity), 64);
    dev->GetTransform(kTsView, out);
    out[12] = out[13] = out[14] = 0.0f;  // _41 _42 _43
    v3d(kMatrixTransposeInPlace, out);
    v3d(kMatrixMultiplyInPlace, out, generator);
}

// 0x005fa360. Modes: 0x01IISSSS... see below; 0x02....NN projected layers;
// 0x0001CA00 / 0x0002.... single-stage colour (C) / alpha (A) ops; 0-19 fixed setups.
// Then the overlay stage (+0x16f) and the tint stage (+0x173) are added.
void SysEn::apply_texture() {
    if (new_tint() != -1) {
        if (new_tfactor() != cur_tfactor()) {
            rs(kRsTextureFactor, new_tfactor());
            cur_tfactor() = new_tfactor();
        }
    } else {
        new_tfactor() = cur_tfactor();
    }
    if (new_texture() == cur_texture() && new_overlay() == cur_overlay() && new_tint() == cur_tint()) return;

    // Undo what the previous mode set beyond the stage combiners.
    const std::uint32_t old = cur_texture();
    if (old == 0x11) tss(1, kTssTexCoordIndex, 1);
    if (old == 0x13) {
        tss(0, kTssTexCoordIndex, 0);
        tss(1, kTssTexCoordIndex, 1);
        tss(2, kTssTexCoordIndex, 2);
        tss(2, kTssAddressU, kTaddressWrap);
        tss(2, kTssAddressV, kTaddressWrap);
    }
    if (old == 0x12) {
        tss(1, kTssAddressU, kTaddressWrap);
        tss(1, kTssAddressV, kTaddressWrap);
    }
    if ((old & 0xff000000u) == 0x01000000u) {
        for (DWORD stage = 0; stage < 4; ++stage) {
            tss(stage, kTssAddressU, kTaddressWrap);
            tss(stage, kTssAddressV, kTaddressWrap);
        }
        dev()->SetTransform(kTsTexture0, &img::global<const float>(kIdentity));
        for (DWORD stage = 0; stage < 4; ++stage) tss(stage, kTssTextureTransformFlags, kTtffDisable);
        for (DWORD stage = 0; stage < 4; ++stage) tss(stage, kTssTexCoordIndex, stage);
    }
    if (old == 10) {
        tss(2, kTssTextureTransformFlags, kTtffDisable);
        tss(2, kTssTexCoordIndex, 2);
    }
    if (old == 12) {
        tss(0, kTssTexCoordIndex, 0);
        tss(1, kTssTexCoordIndex, 1);
        tss(1, kTssTextureTransformFlags, kTtffDisable);
    }
    if (old == 11) {
        tss(0, kTssTexCoordIndex, 0);
        tss(1, kTssTexCoordIndex, 1);
    }
    if (old == 9) tss(0, kTssResultArg, kTaCurrent);
    if ((old & 0xff000000u) == 0x02000000u) {
        const std::int32_t layers = static_cast<std::int32_t>(old & 0xff);
        for (std::int32_t stage = 0; stage < layers; ++stage) {
            tss(stage, kTssAddressU, kTaddressWrap);
            tss(stage, kTssAddressV, kTaddressWrap);
            tss(stage, kTssTextureTransformFlags, kTtffDisable);
            tss(stage, kTssTexCoordIndex, stage);
        }
    }

    const std::uint32_t mode = new_texture();
    if (mode & 0xff000000u) {
        if ((mode & 0xff000000u) == 0x01000000u) {
            // Animated-texture table entry IIII (low 16 bits), set SS (bits 16-23): every stage's
            // combiner, addressing and texture coordinates come from the table.
            const jo::TexScriptRec* const info = jo::as_tex_script_rec(joan::orig::arg::as<const std::uint8_t*>(joan::orig::as_anim_db(img::obj(kAnimDb))->entry(joan::orig::arg::u32(static_cast<std::uint16_t>(mode & 0xffff)))));
            const std::uint32_t set = (mode >> 16) & 0xff;
            // Colour 0x0042a260(255, 255, 255, alpha) stored as D3DCOLOR by 0x005e95b0.
            new_tfactor() = (static_cast<std::uint32_t>(jo::view<std::uint8_t[2]>(info->tfactor_alphas[set])[0]) << 24) | 0x00ffffffu;
            cur_tfactor() = new_tfactor();
            rs(kRsTextureFactor, cur_tfactor());
            rs(kRsAlphaRef, 4);
            std::int16_t stages;
            std::memcpy(&stages, &info->stage_count[set], 2);
            DWORD stage = 0;
            for (; static_cast<std::int32_t>(stage) < stages; ++stage) {
                if (anim_dword(info->has_extra, set, stage) != 0) {  // bump environment map
                    tss(stage, kTssBumpEnvMat00, anim_dword(info->bump_env_mat00s, set, stage));
                    tss(stage, kTssBumpEnvMat01, anim_dword(info->bump_env_mat01s, set, stage));
                    tss(stage, kTssBumpEnvMat10, anim_dword(info->bump_env_mat10s, set, stage));
                    tss(stage, kTssBumpEnvMat11, anim_dword(info->bump_env_mat11s, set, stage));
                    tss(stage, kTssBumpEnvLScale, anim_dword(info->bump_env_lscales, set, stage));
                    tss(stage, kTssBumpEnvLOffset, anim_dword(info->bump_env_loffsets, set, stage));
                } else if (anim_dword(info->sphere_map_flags, set, stage) != 0) {  // sphere-mapped reflection
                    rs(kRsLocalViewer, 1);
                    float m[16];
                    camera_texgen_matrix(dev(), &img::global<const float>(kSphereMap), m);
                    dev()->SetTransform(kTsTexture0 + stage, m);
                    tss(stage, kTssTextureTransformFlags, kTtffCount3);
                    tss(stage, kTssTexCoordIndex, stage | kTciCameraSpaceReflection);
                } else if (anim_dword(info->projected_flags, set, stage) != 0) {  // projected from camera-space position
                    tss(stage, kTssTextureTransformFlags, kTtffCount3 | kTtffProjected);
                    tss(stage, kTssTexCoordIndex, stage | kTciCameraSpacePosition);
                } else if (static_cast<std::int8_t>(info->object_mode[set * 4 + stage]) == 1) {  // light projection (Plane::draw)
                    tss(stage, kTssTexCoordIndex, stage | kTciCameraSpacePosition);
                } else {
                    tss(stage, kTssTexCoordIndex, 0);
                }
                tss(stage, kTssAlphaOp, anim_dword(info->tex_op_121, set, stage));
                tss(stage, kTssAlphaArg1, anim_dword(info->tex_op_61, set, stage));
                tss(stage, kTssAlphaArg2, anim_dword(info->tex_op_c1, set, stage));
                tss(stage, kTssColorOp, anim_dword(info->tex_op_241, set, stage));
                tss(stage, kTssColorArg1, anim_dword(info->tex_op_181, set, stage));
                tss(stage, kTssColorArg2, anim_dword(info->tex_op_1e1, set, stage));
                // Scrolling / scaling texture coordinates.
                const float speed_u = anim_float(info->scroll_speeds_u, set, stage);
                const float speed_v = anim_float(info->scroll_speeds_v, set, stage);
                const float offset_u = anim_float(info->scroll_offsets_u, set, stage);
                const float offset_v = anim_float(info->scroll_offsets_v, set, stage);
                const DWORD scale_u = anim_dword(info->scales_u, set, stage);
                const DWORD scale_v = anim_dword(info->scales_v, set, stage);
                if (static_cast<std::int8_t>(info->object_mode[set * 4 + stage]) == 1) {
                    tss(stage, kTssTextureTransformFlags, kTtffCount3);
                } else if (x87_is_zero(speed_u) && x87_is_zero(speed_v) && x87_is_zero(offset_u) && x87_is_zero(offset_v) &&
                           scale_u == 0x3f800000u && scale_v == 0x3f800000u) {
                    if (anim_dword(info->sphere_map_flags, set, stage) == 0 && anim_dword(info->projected_flags, set, stage) == 0)
                        tss(stage, kTssTextureTransformFlags, kTtffDisable);
                } else {
                    float m[16];
                    std::memcpy(m, &img::global<const float>(kIdentity), 64);
                    std::memcpy(&m[0], &scale_u, 4);  // _11
                    std::memcpy(&m[5], &scale_v, 4);  // _22
                    // Scroll: (time mod 1000/speed) * speed / 1000 (0x005fe8c0 = fmod as float).
                    // [Graphics] WaterFlow (render/water.hpp): a river material's scroll slowed.
                    auto scroll = [](float speed) {
                        speed *= joan::render::water_flow_scale();
                        const float period = 1000.0f / speed;
                        const float time = static_cast<float>(now_ms());
                        const float phase = static_cast<float>(std::fmod(static_cast<double>(time), static_cast<double>(period)));
                        return phase * speed / 1000.0f;
                    };
                    if (!x87_is_zero(speed_u)) m[8] = scroll(speed_u);  // _31
                    m[8] = m[8] + offset_u;
                    if (!x87_is_zero(speed_v)) m[9] = scroll(speed_v);  // _32
                    m[9] = m[9] + offset_v;
                    if (stage <= 3) dev()->SetTransform(kTsTexture0 + stage, m);
                    tss(stage, kTssTextureTransformFlags, kTtffCount2);
                }
                tss(stage, kTssAddressU, anim_dword(info->address_modes_u, set, stage));
                tss(stage, kTssAddressV, anim_dword(info->address_modes_v, set, stage));
            }
            disable_stage(stage);
        } else if ((mode & 0xff000000u) == 0x02000000u) {
            // NN projected layers (camera-space position, border addressed), alpha accumulated.
            const std::int32_t layers = static_cast<std::int32_t>(mode & 0xff);
            auto projected = [this](DWORD stage) {
                tss(stage, kTssAddressU, kTaddressBorder);
                tss(stage, kTssAddressV, kTaddressBorder);
                tss(stage, kTssTextureTransformFlags, kTtffCount3);
                tss(stage, kTssTexCoordIndex, kTciCameraSpacePosition);
                tss(stage, kTssBorderColor, 0);
            };
            tss(0, kTssColorOp, kTopSelectArg1);
            tss(0, kTssColorArg1, kTaTexture);
            tss(0, kTssAlphaOp, kTopSelectArg1);
            tss(0, kTssAlphaArg1, kTaTexture);
            projected(0);
            for (std::int32_t stage = 1; stage < layers; ++stage) {
                tss(stage, kTssColorOp, kTopModulate);
                tss(stage, kTssColorArg1, kTaTexture);
                tss(stage, kTssColorArg2, kTaCurrent);
                tss(stage, kTssAlphaOp, kTopAdd);
                tss(stage, kTssAlphaArg1, kTaTexture);
                tss(stage, kTssAlphaArg2, kTaCurrent);
                projected(stage);
            }
            disable_stage(layers);
        }
    } else if (mode & 0xffff0000u) {
        // Single stage: texture against diffuse, colour op from the low bits, alpha op above.
        auto color = [this](DWORD op, bool texture, bool diffuse) {
            tss(0, kTssColorOp, op);
            if (texture) tss(0, kTssColorArg1, kTaTexture);
            if (diffuse) tss(0, kTssColorArg2, kTaDiffuse);
        };
        auto alpha = [this](DWORD op, bool texture, bool diffuse) {
            tss(0, kTssAlphaOp, op);
            if (texture) tss(0, kTssAlphaArg1, kTaTexture);
            if (diffuse) tss(0, kTssAlphaArg2, kTaDiffuse);
        };
        if ((mode & 0xffff0000u) == 0x10000u) {
            switch (mode & 0xf) {
            case 0: color(kTopModulate, true, true); break;
            case 1: color(kTopSelectArg2, false, true); break;
            case 2: color(kTopSelectArg1, true, false); break;
            case 3: color(kTopModulate2x, true, true); break;
            case 4: color(kTopModulate4x, true, true); break;
            default: break;
            }
            switch (mode & 0xf0) {
            case 0x00: alpha(kTopModulate, true, true); break;
            case 0x10: alpha(kTopSelectArg2, false, true); break;
            case 0x20: alpha(kTopSelectArg1, true, false); break;
            case 0x30: alpha(kTopModulate2x, true, true); break;
            case 0x40: alpha(kTopModulate4x, true, true); break;
            default: break;
            }
            disable_stage(1);
        } else if ((mode & 0xffff0000u) == 0x20000u) {
            switch (mode & 0x3) {
            case 0: color(kTopModulate, true, true); break;
            case 1: color(kTopSelectArg2, false, true); break;
            case 2: color(kTopSelectArg1, true, false); break;
            default: break;
            }
            switch (mode & 0x30) {
            case 0x00: alpha(kTopModulate, true, true); break;
            case 0x10: alpha(kTopSelectArg2, false, true); break;
            case 0x20: alpha(kTopSelectArg1, true, false); break;
            default: break;
            }
            disable_stage(1);
        }
    } else {
        apply_fixed_texture_mode(mode);
    }

    // Overlay texture (blended by its alpha, clamped, no mip-maps).
    if (new_overlay() != -1) {
        if (new_texture() == 0) {
            tss(0, kTssColorOp, kTopModulate);
            tss(0, kTssColorArg1, kTaTexture);
            tss(0, kTssColorArg2, kTaDiffuse);
            tss(0, kTssAlphaOp, kTopModulate);
            tss(0, kTssAlphaArg1, kTaTexture);
            tss(0, kTssAlphaArg2, kTaDiffuse);
            tss(0, kTssMipFilter, kTexfNone);
            tss(0, kTssAddressU, kTaddressClamp);
            tss(0, kTssAddressV, kTaddressClamp);
        } else {
            const DWORD s = static_cast<DWORD>(new_overlay());
            tss(s, kTssColorOp, kTopBlendTextureAlpha);
            tss(s, kTssColorArg1, kTaTexture);
            tss(s, kTssColorArg2, kTaCurrent);
            tss(s, kTssAlphaOp, kTopSelectArg1);
            tss(s, kTssAlphaArg1, kTaTexture);
            tss(s, kTssMipFilter, kTexfNone);
            tss(s, kTssAddressU, kTaddressClamp);
            tss(s, kTssAddressV, kTaddressClamp);
            if (new_texture() == 9) {
                tss(s + 1, kTssColorArg1, kTaCurrent);
                tss(s + 1, kTssColorArg2, kTaDiffuse);
                tss(s + 1, kTssColorOp, kTopAddSmooth);
                disable_stage(s + 2);
            } else {
                disable_stage(s + 1);
            }
        }
    }
    cur_texture() = new_texture();
    cur_overlay() = new_overlay();
    // Tint: modulate by the texture factor.
    if (new_tint() != -1) {
        if (new_texture() == 0) {
            tss(0, kTssColorOp, kTopModulate);
            tss(0, kTssColorArg1, kTaTFactor);
            tss(0, kTssColorArg2, kTaDiffuse);
            tss(0, kTssAlphaOp, kTopModulate);
            tss(0, kTssAlphaArg1, kTaTFactor);
            tss(0, kTssAlphaArg2, kTaDiffuse);
        } else {
            const DWORD s = static_cast<DWORD>(new_tint());
            tss(s, kTssColorOp, kTopModulate);
            tss(s, kTssColorArg1, kTaTFactor);
            tss(s, kTssColorArg2, kTaCurrent);
            tss(s, kTssAlphaOp, kTopModulate);
            tss(s, kTssAlphaArg1, kTaTFactor);
            tss(s, kTssAlphaArg2, kTaCurrent);
            disable_stage(s + 1);
        }
    }
    cur_texture() = new_texture();
    cur_tint() = new_tint();
}

// 0x005fa360, modes 0-19 (the switch at 0x005fbaeb).
void SysEn::apply_fixed_texture_mode(std::uint32_t mode) {
    constexpr DWORD kOne = 0x3f800000;  // 1.0f
    float m[16];
    switch (mode) {
    case 0:
        tss(0, kTssColorOp, kTopSelectArg2);
        tss(0, kTssColorArg2, kTaDiffuse);
        tss(0, kTssAlphaOp, kTopSelectArg2);
        tss(0, kTssAlphaArg2, kTaDiffuse);
        disable_stage(1);
        break;
    case 1:  // 1-4 textures, each blended over the previous by its alpha
        multi_texture(1);
        break;
    case 2:
    case 3:
    case 4:
        multi_texture(max_textures() <= mode ? max_textures() : mode);
        break;
    case 5:
        tss(0, kTssColorOp, kTopModulate);
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorArg2, kTaDiffuse);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(0, kTssAlphaArg1, kTaDiffuse);
        tss(1, kTssColorOp, kTopModulate);
        tss(1, kTssColorArg1, kTaTexture);
        tss(1, kTssColorArg2, kTaCurrent);
        tss(1, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssAlphaArg1, kTaDiffuse);
        disable_stage(2);
        break;
    case 6:
        tss(0, kTssColorOp, kTopModulate);
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorArg2, kTaDiffuse);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(0, kTssAlphaArg1, kTaDiffuse);
        tss(1, kTssColorOp, kTopSelectArg1);
        tss(1, kTssColorArg1, kTaCurrent);
        tss(1, kTssAlphaOp, kTopModulate);
        tss(1, kTssAlphaArg1, kTaTexture);
        tss(1, kTssAlphaArg2, kTaDiffuse);
        tss(2, kTssColorOp, kTopDisable);
        break;
    case 7:
        tss(0, kTssColorOp, kTopModulate2x);
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorArg2, kTaDiffuse);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(0, kTssAlphaArg1, kTaDiffuse);
        tss(1, kTssColorOp, kTopModulate4x);
        tss(1, kTssColorArg1, kTaTexture);
        tss(1, kTssColorArg2, kTaCurrent);
        tss(1, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssAlphaArg1, kTaCurrent);
        disable_stage(2);
        break;
    case 8:
        tss(0, kTssColorOp, kTopModulate);
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorArg2, kTaDiffuse);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(0, kTssAlphaArg1, kTaDiffuse);
        tss(1, kTssColorOp, kTopBlendDiffuseAlpha);
        tss(1, kTssColorArg1, kTaTexture);
        tss(1, kTssColorArg2, kTaCurrent);
        tss(1, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssAlphaArg1, kTaTexture);
        disable_stage(2);
        break;
    case 9:
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorArg2, kTaSpecular);
        tss(0, kTssColorOp, kTopDotProduct3);
        tss(0, kTssResultArg, kTaTemp);
        rs(kRsTextureFactor, 0x80808080u);
        tss(1, kTssColorArg1, kTaTFactor);
        tss(1, kTssColorArg2, kTaDiffuse);
        tss(1, kTssColorArg0, kTaTemp);
        tss(1, kTssColorOp, kTopLerp);
        tss(1, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssAlphaArg1, kTaTexture);
        tss(2, kTssColorArg2, kTaTexture);
        tss(2, kTssColorArg1, kTaCurrent);
        tss(2, kTssColorOp, kTopModulate);
        break;
    case 10:
        tss(0, kTssColorOp, kTopModulate);
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorArg2, kTaDiffuse);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(0, kTssAlphaArg1, kTaTexture);
        tss(0, kTssAlphaArg2, kTaDiffuse);
        tss(1, kTssColorOp, kTopBumpEnvMapLuminance);
        tss(1, kTssColorArg1, kTaTexture);
        tss(1, kTssColorArg2, kTaCurrent);
        tss(2, kTssColorOp, kTopModulate);
        tss(2, kTssColorArg1, kTaTexture);
        tss(2, kTssColorArg2, kTaCurrent);
        tss(1, kTssBumpEnvMat00, kOne);
        tss(1, kTssBumpEnvMat01, 0);
        tss(1, kTssBumpEnvMat10, 0);
        tss(1, kTssBumpEnvMat11, kOne);
        tss(1, kTssBumpEnvLScale, kOne);
        tss(1, kTssBumpEnvLOffset, 0);
        camera_texgen_matrix(dev(), &img::global<const float>(kNormalMap), m);
        dev()->SetTransform(kTsTexture0 + 2, m);
        tss(2, kTssTexCoordIndex, kTciCameraSpaceReflection);
        tss(2, kTssTextureTransformFlags, kTtffCount3);
        break;
    case 11:
        tss(0, kTssTexCoordIndex, 0);
        tss(0, kTssColorOp, kTopModulate);
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorArg2, kTaDiffuse);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(0, kTssAlphaArg1, kTaTexture);
        tss(1, kTssTexCoordIndex, 1);
        tss(1, kTssColorOp, kTopSelectArg2);
        tss(1, kTssColorArg1, kTaTexture);
        tss(1, kTssColorArg2, kTaCurrent);
        tss(1, kTssAlphaOp, kTopAddSigned);
        tss(1, kTssAlphaArg1, kTaTexture | kTaComplement);
        tss(1, kTssAlphaArg2, kTaCurrent);
        rs(kRsAlphaBlendEnable, 1);
        rs(kRsSrcBlend, kBlendSrcAlpha);
        rs(kRsDestBlend, kBlendZero);
        break;
    case 12:
        tss(0, kTssTexCoordIndex, 0);
        tss(0, kTssColorOp, kTopSelectArg2);
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorArg2, kTaDiffuse);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(0, kTssAlphaArg1, kTaDiffuse);
        tss(1, kTssColorOp, kTopModulate);
        tss(1, kTssColorArg1, kTaTexture);
        tss(1, kTssColorArg2, kTaCurrent);
        tss(1, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssAlphaArg1, kTaDiffuse);
        rs(kRsLocalViewer, 1);
        camera_texgen_matrix(dev(), &img::global<const float>(kNormalMapB), m);
        dev()->SetTransform(kTsTexture0 + 1, m);
        tss(1, kTssTexCoordIndex, kTciCameraSpaceReflection);
        tss(1, kTssTextureTransformFlags, kTtffCount3);
        disable_stage(2);
        break;
    case 13:
        tss(0, kTssColorOp, kTopModulate);
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorArg2, kTaDiffuse);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(0, kTssAlphaArg1, kTaTexture);
        tss(1, kTssColorOp, kTopBlendTextureAlpha);
        tss(1, kTssColorArg1, kTaCurrent);
        tss(1, kTssColorArg2, kTaTexture);
        tss(1, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssAlphaArg1, kTaDiffuse);
        disable_stage(2);
        break;
    case 14:
        rs(kRsTextureFactor, 0xff000000u | (global<std::uint32_t>(d::g_a17ed8) & 0xff) << 16 |
                                  (global<std::uint32_t>(d::g_a17edc) & 0xff) << 8 | (global<std::uint32_t>(d::g_a17ee0) & 0xff));
        tss(0, kTssColorOp, kTopAdd);
        tss(0, kTssColorArg1, kTaTFactor);
        tss(0, kTssColorArg2, kTaTexture);
        disable_stage(1);
        break;
    case 15:
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorArg2, kTaDiffuse);
        tss(0, kTssColorOp, kTopSelectArg1);
        tss(0, kTssAlphaArg1, kTaTexture);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssColorArg1, kTaCurrent);
        tss(1, kTssColorArg2, kTaSpecular);
        tss(1, kTssColorOp, kTopModulate);
        tss(1, kTssAlphaArg1, kTaCurrent);
        tss(1, kTssAlphaOp, kTopSelectArg1);
        disable_stage(2);
        break;
    case 16:
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorArg2, kTaSpecular);
        tss(0, kTssColorOp, kTopModulate);
        tss(0, kTssAlphaArg1, kTaTexture);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssColorArg1, kTaCurrent);
        tss(1, kTssColorArg2, kTaSpecular);
        tss(1, kTssColorOp, kTopAddSmooth);
        tss(1, kTssAlphaArg1, kTaTexture);
        tss(1, kTssAlphaOp, kTopSelectArg1);
        disable_stage(2);
        break;
    case 17:
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorOp, kTopSelectArg1);
        tss(0, kTssAlphaArg1, kTaTexture);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssColorArg1, kTaTexture);
        tss(1, kTssColorArg2, kTaCurrent);
        tss(1, kTssColorOp, kTopBlendDiffuseAlpha);
        tss(1, kTssAlphaArg1, kTaTexture);
        tss(1, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssTexCoordIndex, 0);
        tss(2, kTssColorArg1, kTaCurrent);
        tss(2, kTssColorArg2, kTaDiffuse);
        tss(2, kTssColorOp, kTopModulate);
        tss(2, kTssAlphaArg1, kTaCurrent);
        tss(2, kTssAlphaOp, kTopSelectArg1);
        disable_stage(3);
        break;
    case 18:
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorArg2, kTaDiffuse);
        tss(0, kTssColorOp, kTopModulate);
        tss(0, kTssAlphaArg1, kTaTexture);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssColorArg1, kTaCurrent);
        tss(1, kTssColorArg2, kTaTexture);
        tss(1, kTssColorOp, kTopModulate);
        tss(1, kTssAlphaArg1, kTaCurrent);
        tss(1, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssAddressU, kTaddressClamp);
        tss(1, kTssAddressV, kTaddressClamp);
        disable_stage(2);
        break;
    case 19:
        tss(0, kTssColorArg1, kTaTexture);
        tss(0, kTssColorOp, kTopSelectArg1);
        tss(0, kTssAlphaArg1, kTaTexture);
        tss(0, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssColorArg2, kTaCurrent);
        tss(1, kTssColorArg1, kTaTexture);
        tss(1, kTssColorOp, kTopBlendDiffuseAlpha);
        tss(1, kTssAlphaArg1, kTaTexture);
        tss(1, kTssAlphaOp, kTopSelectArg1);
        tss(1, kTssTexCoordIndex, 0);
        tss(2, kTssColorArg2, kTaCurrent);
        tss(2, kTssColorArg1, kTaTexture);
        tss(2, kTssColorOp, kTopModulate);
        tss(2, kTssAlphaArg1, kTaCurrent);
        tss(2, kTssAlphaOp, kTopSelectArg1);
        tss(2, kTssTexCoordIndex, 1);
        tss(2, kTssAddressU, kTaddressClamp);
        tss(2, kTssAddressV, kTaddressClamp);
        tss(3, kTssColorArg1, kTaCurrent);
        tss(3, kTssColorArg2, kTaDiffuse);
        tss(3, kTssColorOp, kTopModulate);
        tss(3, kTssAlphaArg1, kTaCurrent);
        tss(3, kTssAlphaArg2, kTaCurrent);
        tss(3, kTssAlphaOp, kTopSelectArg1);
        disable_stage(4);
        break;
    default:
        break;
    }
}

// --- the replacements ---------------------------------------------------------------------
struct Entry {
    void set_blend(std::uint32_t m) { reinterpret_cast<SysEn*>(this)->set_blend(m); }
    void set_texture_mode(std::uint32_t m) { reinterpret_cast<SysEn*>(this)->set_texture_mode(m); }
    void set_filter(std::uint32_t m) { reinterpret_cast<SysEn*>(this)->set_filter(m); }
    // eax: m when it only re-selects the current flags (m != pending, m == current), else this.
    std::uintptr_t set_flags(std::uint32_t m) {
        SysEn* const s = reinterpret_cast<SysEn*>(this);
        const std::uintptr_t r = (m != s->new_flags() && m == s->cur_flags()) ? std::uintptr_t{m} : reinterpret_cast<std::uintptr_t>(this);
        s->set_flags(m);
        return r;
    }
    void set_raster(std::uint32_t m) { reinterpret_cast<SysEn*>(this)->set_raster(m); }
    void set_vertex_type(char t) { reinterpret_cast<SysEn*>(this)->set_vertex_type(t); }
    void set_texture_factor(std::uint32_t c) { reinterpret_cast<SysEn*>(this)->set_texture_factor(c); }
    std::uint32_t flags() { return reinterpret_cast<SysEn*>(this)->flags(); }
    std::int32_t overlay_stage() { return reinterpret_cast<SysEn*>(this)->overlay_stage(); }
    void apply(std::uint32_t anim_id) { reinterpret_cast<SysEn*>(this)->apply(anim_id); }
    void apply_blend() { reinterpret_cast<SysEn*>(this)->apply_blend(); }
    void apply_texture() { reinterpret_cast<SysEn*>(this)->apply_texture(); }
    void apply_filter() { reinterpret_cast<SysEn*>(this)->apply_filter(); }
    void apply_raster() { reinterpret_cast<SysEn*>(this)->apply_raster(); }
    void multi_texture(std::uint32_t n) { reinterpret_cast<SysEn*>(this)->multi_texture(n); }
    void apply_shaders() { reinterpret_cast<SysEn*>(this)->apply_shaders(); }
};

#define SYSEN(name, addr, fn) JOAN_REPLACE("en3d/sysen.cpp", "SysEn::" name, addr, fn)
SYSEN("set_blend", 0x005f8250, &Entry::set_blend);
SYSEN("set_texture_mode", 0x005fa250, &Entry::set_texture_mode);
SYSEN("set_filter", 0x005fec10, &Entry::set_filter);
SYSEN("set_flags", 0x00602080, &Entry::set_flags);
SYSEN("set_raster", 0x006026a0, &Entry::set_raster);
SYSEN("set_vertex_type", 0x006024e0, &Entry::set_vertex_type);
SYSEN("set_texture_factor", 0x005feb50, &Entry::set_texture_factor);
SYSEN("flags", 0x006020f0, &Entry::flags);
SYSEN("overlay_stage", 0x00601470, &Entry::overlay_stage);
SYSEN("apply", 0x006014d0, &Entry::apply);
SYSEN("apply_blend", 0x005f82e0, &Entry::apply_blend);
SYSEN("apply_texture", 0x005fa360, &Entry::apply_texture);
SYSEN("apply_filter", 0x005fec90, &Entry::apply_filter);
SYSEN("apply_raster", 0x00602750, &Entry::apply_raster);
SYSEN("multi_texture", 0x005fe920, &Entry::multi_texture);
SYSEN("apply_shaders", 0x00602120, &Entry::apply_shaders);
#undef SYSEN

}  // namespace sysen_state
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
namespace {
namespace game_b103_11 {

using joan::call::thiscall;
// 0x005ff990
i32 JOAN_CDECL is_kind_15_16(i32 x) { return (x >= 0x15 && x <= 0x16) ? 1 : 0; }

struct H {
    // 0x005fa220
    u32 field_135() { return jo::self<jo::SysEn>(this)->cur_texture_mode; }
    // 0x005febb0
    u32* get_141(u32* out) {
        *out = jo::view<u32>(jo::self<jo::SysEn>(this)->cur_texture_factor);
        return out;
    }
    // 0x005febe0
    u32 field_139() { return jo::self<jo::SysEn>(this)->cur_filter; }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Obj::field_135", 0x005fa220, &H::field_135);
B103("Obj::get_141", 0x005febb0, &H::get_141);
B103("Obj::field_139", 0x005febe0, &H::field_139);
B103("is_kind_15_16", 0x005ff990, &is_kind_15_16);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/en3d_helpers2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2 {

// Every thiscall helper shares one class so `this` is just the object's address.
struct H {
    // --- small ones -----------------------------------------------------------------------
    H* identity_self() { return this; }  // 0x005bea30 Matrix ctor (leaves it uninitialised) / 0x005fe900 clip plane data
};

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("ClipPlane::data", 0x005fe900, &H::identity_self);
#undef HELPER2

}  // namespace en3d_helpers2
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (en3d/b82_17)
namespace {
namespace en3d_b82_17 {

struct H {
    // 0x005ff060
    std::uint32_t buffer_count() {
        switch (jo::self<jo::SysEn>(this)->pp_device_window - 1u) {
            case 0:
            case 1:
                return jo::self<jo::SysEn>(this)->pp_multisample_type + 1u;
            case 2:
            case 3:
                return jo::self<jo::SysEn>(this)->pp_multisample_type;
            default:
                return 1;
        }
    }
};

#define B82_17E(name, addr, fn) JOAN_REPLACE("en3d/b82_17", name, addr, fn)
B82_17E("Device::buffer_count", 0x005ff060, &H::buffer_count);
#undef B82_17E

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/game_leaves82a.cpp (en3d/helpers38)
namespace {
namespace game_leaves82a {

using joan::hook::global;

using joan::call::thiscall;
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
JOAN_COUNTER(c_stats_end, "Stats::end_frame");

struct H {
    // 0x005ff0e0: when the device state (+0) >= 3, Present(0, 0, 0, 0) (vtable +0x3c) on the
    // device +8, then frame index +0x128 = (+0x128 + 1) wrapped by the count from 0x005ff060.
    void device_present() {
        if (jo::self<jo::SysEn>(this)->started < 3) return;
        com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0x3c / 4, static_cast<const void*>(nullptr), static_cast<const void*>(nullptr), static_cast<void*>(nullptr), static_cast<const void*>(nullptr));  // Present (pointer arguments)
        jo::self<jo::SysEn>(this)->frame_index = jo::self<jo::SysEn>(this)->frame_index + 1;
        const std::uint32_t count = joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(this)->buffer_count());  // 0x005ff060
        if (jo::self<jo::SysEn>(this)->frame_index >= count) {
            const std::uint32_t divisor = joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(this)->buffer_count());  // 0x005ff060
            jo::self<jo::SysEn>(this)->frame_index = jo::self<jo::SysEn>(this)->frame_index % divisor;
        }
    }

    // The frame statistics of SysEn (this = the SysEn object g_sysEn, called as Stats::begin_frame /
    // end_frame): accessed through the SysEn members, whose x64 offsets account for the pointers before
    // them (the low-confidence Stats layout is all gaps there and put the counters over vs_usable /
    // hw_skinning / ... on x64, clearing hardware skinning after the first frames).
    // 0x00600df0: memset(+0x21b, 0, 0x2c); starts the frame timer +0x2d7 (0x00584580).
    void stats_begin() {
        std::memset(joan::orig::arg::mptr(jo::self<jo::SysEn>(this)->stats), 0, 0x2cu);  // memset: the 11 current counters (+0x21b)
        if (jo::view<void*>(jo::self<jo::SysEn>(this)->frame_timer) != nullptr) joan::orig::dynarray(jo::view<void*>(jo::self<jo::SysEn>(this)->frame_timer))->zap(0);  // 0x00584580
    }

    // 0x00600e50: last frame +0x247 = current +0x21b (11 dwords); sums +0x273 += last; frame
    // count +0x2cb; every 3000 ms (0xa500b2 - +0x2cf) the averages +0x29f = sum / frames, sums
    // cleared. Then timer +0x2d3 restarted (0x00584580) and swapped with +0x2d7.
    void stats_end() {
        c_stats_end.hit();
        std::memcpy(jo::self<jo::SysEn>(this)->stats_last, jo::self<jo::SysEn>(this)->stats, 0x2c);
        for (std::int32_t i = 0; i < 11; ++i)
            jo::self<jo::SysEn>(this)->stats_sum[i] = jo::self<jo::SysEn>(this)->stats_sum[i] + jo::self<jo::SysEn>(this)->stats_last[i];
        jo::self<jo::SysEn>(this)->stats_frames = jo::self<jo::SysEn>(this)->stats_frames + 1;
        if (global<std::uint32_t>(d::g_game_time_ms) - jo::self<jo::SysEn>(this)->stats_time >= 0xbb8u) {
            for (std::int32_t i = 0; i < 11; ++i)
                jo::self<jo::SysEn>(this)->stats_avg[i] = jo::self<jo::SysEn>(this)->stats_sum[i] / jo::self<jo::SysEn>(this)->stats_frames;
            std::memset(joan::orig::arg::mptr(jo::self<jo::SysEn>(this)->stats_sum), 0, 0x2cu);  // memset
            jo::self<jo::SysEn>(this)->stats_frames = 0;
            jo::self<jo::SysEn>(this)->stats_time = global<std::uint32_t>(d::g_game_time_ms);
        }
        if (jo::view<void*>(jo::self<jo::SysEn>(this)->frame_timer) != nullptr && jo::view<void*>(jo::self<jo::SysEn>(this)->prev_frame_timer) != nullptr) {
            void* const timer_object = jo::view<void*>(jo::self<jo::SysEn>(this)->prev_frame_timer);
            joan::orig::dynarray(timer_object)->zap(0);  // 0x00584580
            jo::view<void*>(jo::self<jo::SysEn>(this)->prev_frame_timer) = jo::view<void*>(jo::self<jo::SysEn>(this)->frame_timer);
            jo::view<void*>(jo::self<jo::SysEn>(this)->frame_timer) = timer_object;
        }
    }
};

#define HELPER38(name, addr, fn) JOAN_REPLACE("en3d/helpers38", name, addr, fn)
HELPER38("Device::present", 0x005ff0e0, &H::device_present);
HELPER38("Stats::begin_frame", 0x00600df0, &H::stats_begin);
HELPER38("Stats::end_frame", 0x00600e50, &H::stats_end);
#undef HELPER38

}  // namespace game_leaves82a
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
namespace {
namespace game_b93_03 {

using joan::call::thiscall;
JOAN_COUNTER(c_flush_extra, "SysEn::flush_extra");
JOAN_COUNTER(c_flush_all, "SysEn::flush_all");

std::uintptr_t low_byte_into(std::uintptr_t eax, u8 al) { return (eax & ~std::uintptr_t{0xff}) | al; }

// 0x005ff920
i32 JOAN_CDECL format_bits(i32 fmt) {
    switch (static_cast<u32>(fmt - 0x15)) {
        case 0:
        case 1:
            return 0x20;
        case 2:
        case 3:
        case 4:
            return 0x10;
        default:
            return 0;
    }
}

struct H {
    void flush_extra() {  // 0x005ff180
        if (jo::self<jo::SysEn>(this)->started < 3) return;
        if (jo::self<jo::SysEn>(this)->frame_index == 0) return;
        c_flush_extra.hit();
        i32 n = joan::orig::arg::as<i32>(joan::orig::as_sys_en(this)->buffer_count()) - jo::view<i32>(jo::self<jo::SysEn>(this)->frame_index);  // 0x005ff060
        for (; n > 0; --n) com<u32>(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0x3c, static_cast<const void*>(nullptr), static_cast<const void*>(nullptr), static_cast<void*>(nullptr), static_cast<const void*>(nullptr));  // Present (pointer arguments)
    }
    void flush_all() {  // 0x005ff210
        if (jo::self<jo::SysEn>(this)->started < 3) return;
        if (jo::self<jo::SysEn>(this)->frame_index == 0) return;
        c_flush_all.hit();
        for (i32 n = jo::view<i32>(jo::self<jo::SysEn>(this)->frame_index); n > 0; --n) com<u32>(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0x3c, static_cast<const void*>(nullptr), static_cast<const void*>(nullptr), static_cast<void*>(nullptr), static_cast<const void*>(nullptr));  // Present (pointer arguments)
    }
    std::uintptr_t flag_800() {  // 0x00602660
        const u8 r = (jo::self<jo::SysEn>(this)->caps.texture_caps & 0x800u) != 0 ? 1 : 0;
        return low_byte_into(reinterpret_cast<std::uintptr_t>(this), r);  // eax: al = r, the rest is the x86 this register
    }
    void set_clamp_2ef(float v) {  // 0x00602a50
        if (!(v >= -1.0f)) {  // fcomp [0x9cf834]; test ah,1 (less or unordered)
            jo::view<u32>(jo::self<jo::SysEn>(this)->brightness) = 0xbf800000u;
        } else if (v > 1.0f) {  // fcomp [0x9cf784]; test ah,0x41 jz
            jo::view<u32>(jo::self<jo::SysEn>(this)->brightness) = 0x3f800000u;
        } else {
            jo::self<jo::SysEn>(this)->brightness = v;
        }
    }
    void set_clamp_2f3(float v) {  // 0x00602ad0
        if (!(static_cast<double>(v) >= 0.5)) {  // fcomp qword [0x9d3e18]
            jo::view<u32>(jo::self<jo::SysEn>(this)->contrast) = 0x3f000000u;
        } else if (v > 3.0f) {  // fcomp [0x9cf798]
            jo::view<u32>(jo::self<jo::SysEn>(this)->contrast) = 0x40400000u;
        } else {
            jo::self<jo::SysEn>(this)->contrast = v;
        }
    }
    void release_323() {  // 0x006034b0
        if (jo::self<jo::SysEn>(this)->light_target != nullptr) {
            com<u32>(jo::view<void*>(jo::self<jo::SysEn>(this)->light_target), 8);  // Release
            jo::self<jo::SysEn>(this)->light_target = nullptr;
            jo::self<jo::SysEn>(this)->light_target_width = 0;
            jo::self<jo::SysEn>(this)->light_target_height = 0;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("SysEn::flush_extra", 0x005ff180, &H::flush_extra);
B93("SysEn::flush_all", 0x005ff210, &H::flush_all);
B93("format_bits", 0x005ff920, &format_bits);
B93("Mesh::flag_800", 0x00602660, &H::flag_800);
B93("Obj::set_clamp_2ef", 0x00602a50, &H::set_clamp_2ef);
B93("Obj::set_clamp_2f3", 0x00602ad0, &H::set_clamp_2f3);
B93("Obj::release_323", 0x006034b0, &H::release_323);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

using joan::img::obj;

using joan::call::thiscall;
// COM method (stdcall, interface pointer first) through the vtable slot (byte offset).
template <class R = u32, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    const std::uintptr_t fn = reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)]);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(fn)(object, args...);
}
JOAN_COUNTER(c_reset, "Display::reset_device");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------ 0x005ff4a0
    // eax as in the original: this ([+8] == 0), the 0x005ff420 result (<= 0), the Reset HRESULT
    // (failure) or the eax of the last call 0x00602080 (the caller 0x00603fd0 can return it).
    std::uintptr_t reset_device() {
        c_reset.hit();
        if (jo::self<jo::SysEn>(this)->device == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        const i32 state = joan::orig::arg::as<i32>(joan::orig::as_d3d(this)->device_lost());  // 0x005ff420
        if (state <= 0) return static_cast<u32>(state);
        joan::orig::as_sys_en(this)->release_buffers();                                     // 0x005f7f60
        const bool fog = jo::self<jo::SysEn>(this)->depth_target != nullptr;
        const u32 fog_a = jo::self<jo::SysEn>(this)->depth_target_width;
        const u32 fog_b = jo::self<jo::SysEn>(this)->depth_target_height;
        const bool light = jo::self<jo::SysEn>(this)->light_target != nullptr;
        const u32 light_a = jo::self<jo::SysEn>(this)->light_target_width;
        const u32 light_b = jo::self<jo::SysEn>(this)->light_target_height;
        joan::orig::as_render_target(this)->release_depth();  // 0x00603380
        joan::orig::as_sys_en(this)->release_323();  // 0x006034b0
        if (jo::view<void*>(jo::self<jo::SysEn>(this)->back_buffer) != nullptr) {
            com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->back_buffer), 0x8);  // Release
            jo::self<jo::SysEn>(this)->back_buffer = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::SysEn>(this)->depth_stencil) != nullptr) {
            com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->depth_stencil), 0x8);
            jo::self<jo::SysEn>(this)->depth_stencil = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::SysEn>(this)->vs_cache) != nullptr) joan::orig::as_vs_cache(jo::view<void*>(jo::self<jo::SysEn>(this)->vs_cache))->release_shaders();  // 0x0061c280
        if (jo::view<void*>(jo::self<jo::SysEn>(this)->ps_cache) != nullptr) joan::orig::as_pixel_shader_cache(jo::view<void*>(jo::self<jo::SysEn>(this)->ps_cache))->free_all();  // 0x005e5f10
        for (u8* node = jo::view<u8*>(jo::self<jo::SysEn>(this)->resources_tail); node != nullptr;) {
            u8* const next = jo::view<u8*>(jo::as_resource(node)->prev);
            reinterpret_cast<u32(JOAN_CDECL*)(void*, u32, u32)>(jo::view<std::uintptr_t>(jo::as_resource(node)->callback))(jo::as_resource(node)->context, 0, jo::as_resource(node)->arg);
            node = next;
        }
        const u32 hr = com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0x38, &jo::self<jo::SysEn>(this)->pp_back_buffer_height);  // IDirect3DDevice8::Reset (the D3DPRESENT_PARAMETERS block)
        if (hr != 0) return hr;
        for (u8* node = jo::view<u8*>(jo::self<jo::SysEn>(this)->resources_head); node != nullptr;) {
            u8* const next = jo::view<u8*>(jo::as_resource(node)->next);
            reinterpret_cast<u32(JOAN_CDECL*)(void*, u32, u32)>(jo::view<std::uintptr_t>(jo::as_resource(node)->callback))(jo::as_resource(node)->context, 1, jo::as_resource(node)->arg);
            joan::orig::as_sound_stream(obj(d::g_snd))->update();  // 0x0059b1c0
            joan::orig::as_ambient_streams(obj(d::g_mus))->update();  // 0x005866d0
            node = next;
        }
        com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0x40, u32{0}, u32{0}, &jo::self<jo::SysEn>(this)->back_buffer);  // GetBackBuffer
        com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0x84, &jo::self<jo::SysEn>(this)->depth_stencil);                 // GetDepthStencilSurface
        joan::orig::as_sys_en(this)->create_buffers();                            // 0x005f7ca0
        if (fog) joan::orig::as_render_target(this)->create_depth(joan::orig::arg::u32(fog_a), joan::orig::arg::u32(fog_b));        // 0x006032d0
        if (light) joan::orig::as_texture(this)->create_603400(joan::orig::arg::u32(light_a), joan::orig::arg::u32(light_b));  // 0x00603400
        jo::self<jo::SysEn>(this)->frame_index = 0;
        jo::self<jo::SysEn>(this)->light_slot_count = 0;
        joan::orig::as_sys_en(this)->set_default_states();  // 0x005f8110
        u32 old = jo::self<jo::SysEn>(this)->cur_blend;
        jo::self<jo::SysEn>(this)->cur_blend = 0xf2;
        jo::self<jo::SysEn>(this)->new_blend = 0xf2;
        joan::orig::as_sys_en(this)->set_blend(joan::orig::arg::u32(old));  // 0x005f8250
        old = jo::self<jo::SysEn>(this)->cur_texture_mode;
        jo::self<jo::SysEn>(this)->cur_texture_mode = 0x2000002;
        jo::self<jo::SysEn>(this)->new_texture_mode = 0x2000002;
        joan::orig::as_sys_en(this)->set_texture_mode(joan::orig::arg::u32(old));  // 0x005fa250
        old = jo::self<jo::SysEn>(this)->cur_filter;
        jo::self<jo::SysEn>(this)->cur_filter = 4;
        jo::self<jo::SysEn>(this)->new_filter = 4;
        joan::orig::as_sys_en(this)->set_filter(joan::orig::arg::u32(old));  // 0x005fec10
        old = jo::self<jo::SysEn>(this)->cur_raster;
        jo::self<jo::SysEn>(this)->cur_raster = 0xffffffffu;
        jo::self<jo::SysEn>(this)->new_raster = 0xffffffffu;
        joan::orig::as_sys_en(this)->set_raster(joan::orig::arg::u32(old));  // 0x006026a0
        const char old_byte = jo::view<char>(jo::self<jo::SysEn>(this)->cur_vertex_type);
        jo::view<u8>(jo::self<jo::SysEn>(this)->cur_vertex_type) = 0xff;
        jo::view<u8>(jo::self<jo::SysEn>(this)->new_vertex_type) = 0xff;
        joan::orig::as_sys_en(this)->set_vertex_type(static_cast<char>(joan::orig::arg::i32(old_byte)));  // 0x006024e0 (byte arg)
        old = jo::self<jo::SysEn>(this)->cur_flags;
        jo::self<jo::SysEn>(this)->cur_flags = 0xff000000u;
        jo::self<jo::SysEn>(this)->new_flags = 0xff000000u;
        return joan::orig::arg::as<u32>(joan::orig::as_sys_en(this)->set_flags(joan::orig::arg::u32(old)));  // 0x00602080
    }
};

JOAN_REPLACE("game/b108_07", "Display::reset_device", 0x005ff4a0, &H::reset_device);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b105_17.cpp (game/b105_17)
namespace {
namespace game_b105_17 {

// ---- global address getters ----
template <auto A>
u32 JOAN_CDECL addr_getter() {
    return static_cast<u32>(img::addr(A));
}

JOAN_REPLACE("game/b105_17", "addr_a17d88", 0x00600160, &addr_getter<d::g_a17d88>);
JOAN_REPLACE("game/b105_17", "addr_a17d98", 0x00600180, &addr_getter<d::g_a17d98>);
JOAN_REPLACE("game/b105_17", "addr_a17da8", 0x006001a0, &addr_getter<d::g_a17da8>);
JOAN_REPLACE("game/b105_17", "addr_a17dd8", 0x006001c0, &addr_getter<d::g_a17dd8>);
JOAN_REPLACE("game/b105_17", "addr_a17de8", 0x006001e0, &addr_getter<d::g_a17de8>);
JOAN_REPLACE("game/b105_17", "addr_a17db8", 0x00600230, &addr_getter<d::g_a17db8>);
JOAN_REPLACE("game/b105_17", "addr_a17dc8", 0x00600250, &addr_getter<d::g_a17dc8>);
JOAN_REPLACE("game/b105_17", "addr_a17e08", 0x00600270, &addr_getter<d::g_a17e08>);

}  // namespace game_b105_17
}  // namespace

// ---------------------------------------- stream cache (not part of the original game)
//
// Models (mdl.cpp sets their buffer_mode to 1) have their whole vertex array and triangle list copied
// into SysEn's dynamic rings for every draw, every frame (SysEn::fill_vertices / fill_indices): with
// the widened draw distance ~3000 copies a frame, a quarter of the frame time. Most of them are the
// same data every frame (buildings, trees, grass patches). Each fill is checked against what the
// same source (address, count, vertex type) held before by a CRC32C of its bytes (SSE4.2): after
// the second identical fill it gets its own static buffer, drawn from with no copy as long as the
// data stays the same. A source whose data keeps changing (animated meshes) is left to the rings.
namespace joan::stream_cache_api {
bool g_model_fill = false;
unsigned g_pool_align = 0;
unsigned g_frame = 1;
}

namespace {
namespace stream_cache {

using Device = joan::d3d8::IDirect3DDevice8;

template <class R = std::int32_t, class... A>
R com(void* object, std::int32_t index, A... args) {  // COM method by vtable index
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
constexpr std::int32_t kRelease = 2, kLock = 11, kUnlock = 12;
constexpr std::uint32_t kMinBytes = 512;      // smaller fills are cheaper to copy than to check
constexpr std::uint32_t kMinGrassBytes = 16;  // ... unless a grass patch is being recorded
constexpr std::uint32_t kVolatileAfter = 3;   // mismatches before a source counts as changing
// Buffers unused for this many frames are freed. Frames, not seconds (Phase 1, 2026-10-29): which arrays are cached
// decides which fills go through SysEn's dynamic rings, whose cursors and counters are part of the game state the
// replay hashes - a wall-clock policy made replays differ by their timing (and under CPU load).
constexpr std::uint32_t kEvictFrames = 600;
constexpr std::uint32_t kSweepEveryFrames = 120;
constexpr std::uint32_t kFullEvery = 64;     // a cached source's full check every this many first uses in a frame (sampled otherwise)
constexpr std::uint32_t kSamples = 48;       // words sampled across the array, plus its first and last 16 bytes
// Arrays up to this size are checked in full on every use (not sampled): 2D screens, text and counters edit a
// few vertices of an otherwise unchanged array in place, which a sample misses until the next full check (the
// old picture shown for a few frames). Summing them costs a few hundred cycles.
constexpr std::uint32_t kSampleFrom = 32 * 1024;

bool crc_supported() {
    static const bool ok = [] {
        int r[4] = {};
        joan::cpu::id(r, 1);
        return (r[2] & (1 << 20)) != 0;  // SSE4.2
    }();
    return ok;
}

// A Fletcher-style sum over 16-byte blocks in four 32-bit lanes (SSE2): `a` adds the blocks, `b` adds the
// running `a`, so a change shows in `a`, and changes that cancel out in `a` still differ by position in `b`.
// Runs at memory speed (two vector adds per 16 bytes); the lanes are folded with CRC32C at the end. Every
// array a model is drawn from is checked this way (index lists on every use), so it has to be cheap.
JOAN_TARGET("sse4.2") std::uint64_t checksum(const void* data, std::uint32_t bytes) {
    const auto* p = static_cast<const std::uint8_t*>(data);
    __m128i a = _mm_set1_epi32(0x9e3779b9), b = _mm_setzero_si128();
    std::uint32_t n = bytes;
    for (; n >= 16; n -= 16, p += 16) {
        a = _mm_add_epi32(a, _mm_loadu_si128(reinterpret_cast<const __m128i*>(p)));
        b = _mm_add_epi32(b, a);
    }
    if (n) {  // the tail, zero padded
        alignas(16) std::uint8_t tail[16] = {};
        std::memcpy(tail, p, n);
        a = _mm_add_epi32(a, _mm_load_si128(reinterpret_cast<const __m128i*>(tail)));
        b = _mm_add_epi32(b, a);
    }
    alignas(16) std::uint32_t la[4], lb[4];
    _mm_store_si128(reinterpret_cast<__m128i*>(la), a);
    _mm_store_si128(reinterpret_cast<__m128i*>(lb), b);
    std::uint32_t h1 = bytes, h2 = ~bytes;
    for (int i = 0; i < 4; ++i) {
        h1 = _mm_crc32_u32(h1, la[i]);
        h2 = _mm_crc32_u32(h2, lb[i]);
    }
    return (static_cast<std::uint64_t>(h1) << 32) | h2;
}

// CRC32C of the 16 bytes at byte offset `at`, continuing `c`.
JOAN_TARGET("sse4.2") inline std::uint32_t crc_run(const std::uint8_t* p, std::uint32_t at, std::uint32_t c) {
    std::uint32_t w[4];
    std::memcpy(w, p + at, 16);
    for (const std::uint32_t x : w) c = _mm_crc32_u32(c, x);
    return c;
}

// CRC32C of kSamples words spread over the array and of its first and last 16 bytes. The words are read
// as kSamples / 4 runs of 16 bytes (one cache line each, not one line per word): a check of a cached array
// costs its cache misses, and data that changes (skinning, colour fades) changes words all over the array.
JOAN_TARGET("sse4.2") std::uint32_t sample(const void* data, std::uint32_t bytes) {
    const auto* p = static_cast<const std::uint8_t*>(data);
    std::uint32_t c = bytes;
    constexpr std::uint32_t kRuns = kSamples / 4;
    if (bytes < 16 * (kRuns + 2)) return static_cast<std::uint32_t>(checksum(data, bytes));
    c = crc_run(p, 0, c);
    c = crc_run(p, bytes - 16, c);
    const std::uint32_t step = (bytes - 16) / kRuns;
    for (std::uint32_t i = 0; i < kRuns; ++i) c = crc_run(p, (i * step + step / 2) & ~15u, c);
    return c;
}

// The light check of an array drawn again in a frame it was already checked in: CRC32C of 4 runs of 16 bytes
// (start, a quarter, the middle, end). Data rewritten between two draws of a frame (a shared mesh skinned per
// unit) differs all over, which this sees.
JOAN_TARGET("sse4.2") std::uint32_t light_sample(const void* data, std::uint32_t bytes) {
    const auto* p = static_cast<const std::uint8_t*>(data);
    if (bytes < 64) return static_cast<std::uint32_t>(checksum(data, bytes));
    std::uint32_t c = ~bytes;
    for (const std::uint32_t at : {0u, (bytes / 4) & ~15u, (bytes / 2) & ~15u, bytes - 16}) {
        std::uint32_t w[4];
        std::memcpy(w, p + at, 16);
        for (const std::uint32_t x : w) c = _mm_crc32_u32(c, x);
    }
    return c;
}

struct Key {
    const void* data;
    std::uint32_t count, kind;  // kind: vertex type 0..8, or 0x100 for 16-bit indices
    bool operator==(const Key& o) const { return data == o.data && count == o.count && kind == o.kind; }
};
struct KeyHash {
    std::size_t operator()(const Key& k) const {
        return std::hash<std::uintptr_t>()(reinterpret_cast<std::uintptr_t>(k.data) ^ (std::uintptr_t{k.count} << 8) ^ k.kind);
    }
};
struct Entry {
    void* buffer = nullptr;  // IDirect3DVertexBuffer8 / IDirect3DIndexBuffer8 (ours)
    std::uint64_t sum = 0;    // CRC of all the bytes
    std::uint32_t sample = 0; // CRC of the sampled words (sample())
    std::uint32_t light = 0;  // light_sample() of the cached bytes
    std::uint32_t checked_frame = 0;  // stream_cache_api::g_frame when it was last verified (sample or full)
    std::uint32_t bytes = 0, mismatches = 0, uses = 0;
    std::uint32_t epoch = 0;  // Cache::epoch of its last full check (index lists)
    bool seen = false, changing = false;
    std::uint32_t used = 0;  // stream_cache_api::g_frame of its last use
};

constexpr std::size_t kMaxWritten = 4096;  // index lists reported written and not drawn since; more: all checked in full

struct Cache {
    std::unordered_map<Key, Entry, KeyHash> entries;
    // Index lists written in place since their last fill (stream_cache_api::indices_written). When the set
    // overflows (lists written and never drawn here: 2D, effects) it is cleared and `epoch` moves on, which
    // sends every index list to one full check.
    std::unordered_set<const void*> written;
    const void* last_written = nullptr;  // set_triangle reports the same list once per triangle
    std::uint32_t epoch = 1;
    std::uint32_t last_sweep = 0;                 // frame of the last sweep
    unsigned long long last_log = 0, clock_ms = 0;  // the wall clock: only for the perf log's cadence
    unsigned hits = 0, misses = 0, created = 0, clock_calls = 0;
    std::uint64_t cached_bytes = 0, full_bytes[2] = {}, sampled[2] = {}, repeats[2] = {};  // [0] vertices, [1] indices

    void mark_written(const void* data) {
        if (data == nullptr || data == last_written) return;
        last_written = data;
        if (written.size() >= kMaxWritten) {
            written.clear();
            ++epoch;
        }
        written.insert(data);
    }
    // The fill of index list `data`: true = it was written since the last one (the mark is used up).
    bool take_written(const void* data) {
        if (written.empty() || written.erase(data) == 0) return false;
        if (last_written == data) last_written = nullptr;
        return true;
    }

    void release(Entry& e) {
        if (!e.buffer) return;
        com<std::uint32_t>(e.buffer, kRelease);
        e.buffer = nullptr;
        cached_bytes -= e.bytes;
    }
    void sweep(std::uint32_t frame, unsigned long long now) {
        if (frame - last_sweep < kSweepEveryFrames) return;
        last_sweep = frame;
        for (auto it = entries.begin(); it != entries.end();) {
            if (frame - it->second.used > kEvictFrames) {
                release(it->second);
                it = entries.erase(it);
            } else {
                ++it;
            }
        }
        if (joan::hook::settings().perf_log > 0 && now - last_log >= static_cast<unsigned long long>(joan::hook::settings().perf_log) * 1000ull) {
            joan::hook::log_line("[Graphics] stream cache: %u fills from cached buffers, %u copied, %u buffers made; %zu sources, %.1f MB cached; "
                                 "full checks %.1f MB vertices / %.1f MB indices, sampled %llu / %llu, again in the frame %llu / %llu",
                           hits, misses, created, entries.size(), static_cast<double>(cached_bytes) / (1024.0 * 1024.0),
                           static_cast<double>(full_bytes[0]) / (1024.0 * 1024.0), static_cast<double>(full_bytes[1]) / (1024.0 * 1024.0),
                           static_cast<unsigned long long>(sampled[0]), static_cast<unsigned long long>(sampled[1]),
                           static_cast<unsigned long long>(repeats[0]), static_cast<unsigned long long>(repeats[1]));
            hits = misses = created = 0;
            full_bytes[0] = full_bytes[1] = sampled[0] = sampled[1] = repeats[0] = repeats[1] = 0;
            last_log = now;
        }
    }
    // The cached buffer holding exactly these bytes, or nullptr (copy into the ring as before).
    void* find(Device* dev, const void* data, std::uint32_t count, std::uint32_t kind, std::uint32_t bytes, std::uint32_t usage,
               std::uint32_t fvf_or_format) {
        if (!joan::stream_cache_api::g_model_fill || !joan::hook::settings().stream_cache) return nullptr;  // only a model's own arrays (stream_cache_api.hpp)
        // A grass patch being recorded (render/grass_batch.hpp) can only be replayed from cached arrays: small ones too.
        if (bytes < (joan::render::grass_recording() ? kMinGrassBytes : kMinBytes) || !data || !crc_supported()) return nullptr;
        // The clock (QueryPerformanceCounter) only every 64th fill: ~5000 fills a frame, read for eviction
        // after seconds.
        if ((clock_calls++ & 63) == 0)
            clock_ms = static_cast<unsigned long long>(
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
        const unsigned long long now = clock_ms;
        sweep(joan::stream_cache_api::g_frame, now);
        Entry& e = entries[Key{data, count, kind}];
        e.used = joan::stream_cache_api::g_frame;
        if (e.changing) {
            ++misses;
            return nullptr;
        }
        // A cached source is checked by a sample of its words (the whole array on every kFullEvery-th use):
        // reading every byte of every model each frame cost as much as copying it. Data that changes (skinning,
        // colour fades) changes words all over the array, which the sample sees.
        // Index lists are edited in place a few indices at a time, which a sample can miss: the Plane functions
        // that edit them report it (stream_cache_api::indices_written), and such a list is checked in full.
        const int ix = kind == 0x100 ? 1 : 0;
        bool full = false;
        if (ix) {  // index lists: also every one at an epoch change (the set of written ones overflowed)
            full = take_written(data) || e.epoch != epoch;
            e.epoch = epoch;
        } else {  // vertex arrays reported written (stream_cache_api::vertices_written: the 2D layer)
            full = take_written(data);
        }
        // Drawn again in a frame it was verified in (a mesh many units share, a second pass): the light check.
        const std::uint32_t frame = joan::stream_cache_api::g_frame;
        if (!full && bytes > kSampleFrom && e.buffer && e.bytes == bytes) {
            if (e.checked_frame == frame) {
                ++repeats[ix];
                if (light_sample(data, bytes) == e.light) {
                    ++hits;
                    return e.buffer;
                }
            } else if (++e.uses % kFullEvery != 0) {
                ++sampled[ix];
                if (sample(data, bytes) == e.sample) {
                    e.checked_frame = frame;
                    ++hits;
                    return e.buffer;
                }
            }
        }
        full_bytes[ix] += bytes;
        const std::uint64_t sum = checksum(data, bytes);
        const bool same = e.seen && e.sum == sum && e.bytes == bytes;
        if (same && e.buffer) {
            e.checked_frame = frame;
            ++hits;
            return e.buffer;
        }
        if (same) {  // the second identical fill: give it a buffer
            void* buffer = nullptr;
            // Into the device's geometry pool: a vertex array at a multiple of its stride (draws then pass its
            // place as the base vertex), indices at 4 bytes.
            joan::stream_cache_api::g_pool_align = kind == 0x100 ? 4u : (count ? bytes / count : 0u);
            const auto hr = kind == 0x100 ? dev->CreateIndexBuffer(bytes, usage, fvf_or_format, 0, reinterpret_cast<joan::d3d8::IDirect3DIndexBuffer8**>(&buffer))
                                             : dev->CreateVertexBuffer(bytes, usage, fvf_or_format, 0, reinterpret_cast<joan::d3d8::IDirect3DVertexBuffer8**>(&buffer));
            joan::stream_cache_api::g_pool_align = 0;
            void* p = nullptr;
            if (hr == 0 && buffer && com(buffer, kLock, 0u, bytes, &p, 0u) == 0 && p) {
                std::memcpy(p, data, bytes);
                com(buffer, kUnlock);
                e.buffer = buffer;
                e.sample = sample(data, bytes);
                e.light = light_sample(data, bytes);
                e.checked_frame = frame;
                e.uses = 0;
                cached_bytes += bytes;
                ++created;
                ++hits;
                return buffer;
            }
            if (buffer) com<std::uint32_t>(buffer, kRelease);
            ++misses;
            return nullptr;
        }
        if (e.seen && ++e.mismatches >= kVolatileAfter) e.changing = true;
        release(e);
        e.seen = true;
        e.sum = sum;
        e.bytes = bytes;
        ++misses;
        return nullptr;
    }
};

Cache& cache() {
    static Cache c;
    return c;
}

}  // namespace stream_cache
}  // namespace

void joan::stream_cache_api::indices_written(const void* data) { stream_cache::cache().mark_written(data); }
void joan::stream_cache_api::vertices_written(const void* data) { stream_cache::cache().mark_written(data); }

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

using joan::img::obj;

using joan::hook::global;
using Device = joan::d3d8::IDirect3DDevice8;

using joan::call::thiscall;
[[maybe_unused]] constexpr img::Addr kSysEn(d::g_sys3d);

// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    std::uint32_t sysen_fill_vertices(char type, const void* data, std::uint32_t count) {  // 0x00600490
        const std::uint32_t stride = jo::as_vertex_format(joan::orig::arg::as<const void*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(type)))))->stride;
        const std::uint32_t bytes = count * stride;
        // Stream cache: unchanged data is drawn from its own buffer, bound as vertex_lock + vertex_unlock
        // would bind the ring (base vertex 0).
        if (type >= 0 && type < 9) {
            auto* const dev = jo::view<Device*>(jo::self<jo::SysEn>(this)->device);
            const std::uint32_t fvf = type == 5 ? 0u : jo::as_vertex_format(joan::orig::arg::as<const void*>(joan::orig::vertex_format(type)))->fvf;
            const std::uint32_t usage = (jo::self<jo::SysEn>(this)->vb_usage & ~0x200u) | (type == 6 ? 0x40u : 0u);  // static, write-only
            if (void* const vb = stream_cache::cache().find(dev, data, count, static_cast<std::uint32_t>(type), bytes, usage, fvf)) {
                jo::view<void*>(jo::self<jo::SysEn>(this)->locked_vb) = vb;
                jo::self<jo::SysEn>(this)->locked_vertex_type = type;
                jo::self<jo::SysEn>(this)->locked_start = 0;
                jo::self<jo::SysEn>(this)->locked_stride = stride;
                jo::self<jo::SysEn>(this)->locked_count = count;
                dev->SetStreamSource(0, static_cast<joan::d3d8::IDirect3DVertexBuffer8*>(vb), stride);
                dev->SetIndices(jo::view<joan::d3d8::IDirect3DIndexBuffer8*>(jo::self<jo::SysEn>(this)->dyn_ib), 0);
                joan::orig::as_sys_en(self())->set_vertex_type(static_cast<char>(joan::orig::arg::i32(type)));  // 0x006024e0
                return 0;
            }
        }
        void* const dst = joan::orig::arg::as<void*>(joan::orig::as_sys_en(self())->vertex_lock(joan::orig::arg::i32(type), joan::orig::arg::u32(count)));  // 0x006005b0 lock
        std::memmove(dst, data, bytes);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(self())->vertex_unlock());  // 0x00600740 unlock
    }
    std::uint32_t sysen_pass_filter(std::uint32_t a, std::uint32_t b) { return a == b ? 1 : 0; }  // 0x00600b30
};

// --- free (cdecl) helpers -----------------------------------------------------------------------
std::uint32_t JOAN_CDECL primitive_type(std::int32_t prim) {  // 0x00600b60: en3d 1-6 = D3DPT 1-6, else list
    return prim >= 1 && prim <= 6 ? static_cast<std::uint32_t>(prim) : 4u;
}

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Sys3D::submit_vertex", 0x00600490, &H::sysen_fill_vertices);
HELPER("SysEn::pass_filter", 0x00600b30, &H::sysen_pass_filter);
HELPER("primitive_type", 0x00600b60, &primitive_type);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

using joan::call::thiscall;
struct H {

    u32 field_16b() { return jo::view<u32>(jo::self<jo::SysEn>(this)->cur_tint_stage); }  // 0x006014a0
    u32 field_145() { return jo::self<jo::SysEn>(this)->cur_raster; }  // 0x00602720
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Obj::field_16b", 0x006014a0, &H::field_16b);
B("Obj::field_145", 0x00602720, &H::field_145);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {

using joan::img::obj;

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}
using joan::hook::global;
using Device = joan::d3d8::IDirect3DDevice8;

using joan::call::thiscall;
// A COM method by vtable index (stdcall, `this` first).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
Device* game_device() { return global<Device*>(d::g_game_device); }

JOAN_COUNTER(c_index_fill, "SysEn::fill_indices");

struct H {
    // 0x00600510: copy `count` indices in, adding `base`.
    std::uint32_t index_fill(const std::uint16_t* src, std::uint32_t count, std::uint32_t base) {
        c_index_fill.hit();
        if (base != 0 && count == 0 && !fix_uninit()) {
            // The original's add loop is a do-while on the count: 0 would run 2^32 times (it
            // writes past the locked buffer until it faults). Batch 118/03: with fix/uninit
            // enabled (module_enabled, read on the first call) the C++ version runs instead and
            // adds nothing (lock / unlock of 0 indices as in the original, no loop pass).
            using Fn = std::uint32_t (H::*)(const std::uint16_t*, std::uint32_t, std::uint32_t);
            return (this->*joan::hook::original<Fn>(fn::SysEn_fill_indices_body))(src, count, base);
        }
        // Stream cache (the stream_cache namespace): unchanged indices (base 0: a model's own triangle list) are
        // drawn from their own buffer, bound as index_unlock binds the ring (base vertex of the last vertex fill).
        if (base == 0 && count != 0) {
            const std::uint32_t usage = jo::self<jo::SysEn>(this)->ib_usage & ~0x200u;
            if (void* const ib = stream_cache::cache().find(game_device(), src, count, 0x100u, count * 2, usage, 101u /* D3DFMT_INDEX16 */)) {
                jo::self<jo::SysEn>(this)->index_start = 0;
                jo::self<jo::SysEn>(this)->index_count = count;
                game_device()->SetIndices(static_cast<joan::d3d8::IDirect3DIndexBuffer8*>(ib), jo::self<jo::SysEn>(this)->locked_start);
                return 0;
            }
        }
        std::uint16_t* const dst = joan::orig::arg::as<std::uint16_t*>(joan::orig::as_sys_en(this)->index_lock(joan::orig::arg::mptr(src), joan::orig::arg::u32(count), joan::orig::arg::u32(base)));  // 0x00600820
        if (base == 0) {
            std::memcpy(joan::orig::arg::mptr(dst), joan::orig::arg::ptr(src), joan::orig::arg::u32(count * 2));  // memmove
        } else {
            for (std::uint32_t i = 0; i < count; ++i) dst[i] = static_cast<std::uint16_t>(src[i] + base);
        }
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(this)->index_unlock());  // 0x00600980
    }
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("Sys3D::submit_index", 0x00600510, &H::index_fill);
#undef HELPER6

}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- from modules/game_b104_10.cpp (game/b104_10)
namespace {
namespace game_b104_10 {

using joan::hook::global;

u32 JOAN_CDECL table_a17ee4(i32 i) { return global<u32>(d::g_a17ee4 + static_cast<u32>(i) * 4u); }

#define B(name, addr, fn) JOAN_REPLACE("game/b104_10", name, addr, fn)
B("table_a17ee4", 0x00600c00, &table_a17ee4);
#undef B

}  // namespace game_b104_10
}  // namespace

// ---------------------------------------- from modules/game_b111_12.cpp (fix/uninit)
namespace {
namespace game_b111_12 {

using joan::call::thiscall;
JOAN_COUNTER(c_probe_bump, "Device::probe_bump_602540");

struct H {
    // -------------------------------------------------------------------- 0x00602540
    u8 probe_bump() {
        c_probe_bump.hit();
        if ((jo::self<jo::SysEn>(this)->caps.texture_op_caps & 0x600000u) == 0) return 0;
        if ((jo::self<jo::SysEn>(this)->caps.texture_op_caps & 0x200000u) == 0) return 0;
        // fix/uninit: the original Releases the pointer even when D3DXCreateTexture failed
        // without writing it; here it starts NULL and only a created texture is Released.
        void* tex = nullptr;
        const i32 hr = joan::orig::arg::as<i32>(joan::orig::d3dx::create_texture(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::SysEn>(this)->device)), joan::orig::arg::u32(u32{0x100}), joan::orig::arg::u32(u32{0x100}), joan::orig::arg::u32(u32{1}), joan::orig::arg::u32(u32{0}), joan::orig::arg::u32(u32{0x3c}), joan::orig::arg::u32(u32{1}), &tex));  // D3DXCreateTexture (V8U8, managed)
        if (tex != nullptr) {
            void* const* const vt = joan::call::vtable_of(tex);
            reinterpret_cast<u32(JOAN_STDCALL*)(void*)>(vt[2])(tex);  // Release
        }
        if (hr != 0) return 0;
        if (jo::self<jo::SysEn>(this)->caps.max_texture_blend_stages < 2) return 0;
        if ((joan::orig::arg::as<u32>(joan::orig::as_mesh(this)->flag_800()) & 0xffu) == 0) return 0;  // 0x00602660
        return 1;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: failed D3DXCreateTexture -> NULL texture, not Released (original Releases garbage).
FIX("Device::probe_bump_602540", 0x00602540, &H::probe_bump);
#undef FIX

}  // namespace game_b111_12
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

struct H {
    // ------------------------------------------------------------------------ 0x00602610
    bool is_mipmapped() {
        if ((jo::self<jo::SysEn>(this)->caps.texture_op_caps & 0x80) == 0) return false;
        return jo::self<jo::SysEn>(this)->caps.max_texture_blend_stages >= 2;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Tex::is_mipmapped", 0x00602610, &H::is_mipmapped);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx2)
namespace {
namespace game_fx {

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
// x87 comparisons (fcomp + fnstsw): C0 = below or unordered, C3 = equal or unordered.
bool below_or_nan(float a, float b) { return !(a >= b); }

JOAN_COUNTER(c_gamma, "Gamma::flash_ramp");

struct H {
    // 0x00602b50 D3D::set_gamma(g): clamped to [0.5, 6].
    void gamma_set(float g) {
        if (below_or_nan(g, 0.5f))
            jo::view<std::uint32_t>(jo::self<jo::SysEn>(this)->gamma) = 0x3f000000u;
        else if (g > 6.0f)
            jo::view<std::uint32_t>(jo::self<jo::SysEn>(this)->gamma) = 0x40c00000u;
        else
            jo::self<jo::SysEn>(this)->gamma = g;
    }
    // 0x00602cd0 D3D::apply_gamma: ramp = 65535 * ((i + offset) * contrast + 128 * brightness)
    // / 255) ^ (1 / gamma), per channel offsets +0x2fb / +0x2ff / +0x303; SetGammaRamp.
    void gamma_apply() {
        c_gamma.hit();
        const joan::hook::x87::Fpu fpu{true};
        std::uint16_t ramp[3][256];
        const std::int32_t top = 0xffff;
        for (std::int32_t i = 0; i < 0x100; ++i) {
            for (std::int32_t ch = 0; ch < 3; ++ch) {
                const std::int32_t n = i + jo::self<jo::SysEn>(this)->gamma_offset[ch];
                float x = static_cast<float>(fpu.add(fpu.mul(static_cast<double>(n), jo::self<jo::SysEn>(this)->contrast),
                                                     static_cast<double>(128.0f * jo::self<jo::SysEn>(this)->brightness)));
                if (below_or_nan(x, 0.0f)) x = 0.0f;
                x = x / 255.0f;
                const float e = 1.0f / jo::self<jo::SysEn>(this)->gamma;
                x = joan::orig::arg::as<float>(joan::orig::libc::pow(joan::orig::arg::f64(static_cast<double>(x)), joan::orig::arg::f64(static_cast<double>(e))));  // pow
                x = static_cast<float>(fpu.mul(static_cast<double>(top), x));
                if (below_or_nan(x, 0.0f))
                    ramp[ch][i] = 0;
                else if (below_or_nan(static_cast<float>(top), x))
                    ramp[ch][i] = static_cast<std::uint16_t>(top);
                else
                    ramp[ch][i] = static_cast<std::uint16_t>(joan::hook::x87::ftol(x));
            }
        }
        void* const device = jo::view<void*>(jo::self<jo::SysEn>(this)->device);
        com_call<std::int32_t>(device, 0x48 / 4, 1u, static_cast<void*>(ramp));  // SetGammaRamp
    }
};

#define FX2(name, addr, fn) JOAN_REPLACE("game/fx2", name, addr, fn)
FX2("D3D::set_gamma", 0x00602b50, &H::gamma_set);
FX2("D3D::apply_gamma", 0x00602cd0, &H::gamma_apply);
#undef FX2

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

JOAN_COUNTER(c_rgb_offsets, "Obj::set_rgb_offsets");

struct H {
    // ------------------------------------------------------------------ 0x00602bd0
    void set_rgb_offsets(i32 r, i32 g, i32 b) {
        c_rgb_offsets.hit();
        jo::self<jo::SysEn>(this)->gamma_offset[0] = r < -255 ? -255 : (r > 0xff ? 0xff : r);
        jo::self<jo::SysEn>(this)->gamma_offset[1] = g < -255 ? -255 : (g > 0xff ? 0xff : g);
        jo::self<jo::SysEn>(this)->gamma_offset[2] = b < -255 ? -255 : (b > 0xff ? 0xff : b);
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Obj::set_rgb_offsets", 0x00602bd0, &H::set_rgb_offsets);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

using joan::img::obj;

using joan::hook::global;

using joan::call::thiscall;
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
struct H {
    // 0x00603380
    void release_depth() {
        if (jo::self<jo::SysEn>(this)->depth_target == nullptr) return;
        com_call<std::uint32_t>(jo::view<void*>(jo::self<jo::SysEn>(this)->depth_target), 2);  // Release
        jo::self<jo::SysEn>(this)->depth_target = nullptr;
        jo::self<jo::SysEn>(this)->depth_target_width = 0;
        jo::self<jo::SysEn>(this)->depth_target_height = 0;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("RenderTarget::release_depth", 0x00603380, &H::release_depth);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SysEn::create() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_02::H::create_5f6620>(this)); }  // 0x5f6620
auto joan::orig::SysEn::shutdown() -> void { joan::bind::member<&::game_b101_11::H::shutdown>(this); }  // 0x5f6770
auto joan::orig::VsCache::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_11::H::sdtor_5f6ac0>(this, a0)); }  // 0x5f6ac0
auto joan::orig::PShaderList::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_06::H::scalar_dtor_5f6b10>(this, a0)); }  // 0x5f6b10
auto joan::orig::D3D::init_5f6b60() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_11::H::d3d_init>(this)); }  // 0x5f6b60
auto joan::orig::SysEn::release_buffers() -> void { joan::bind::member<&::game_b92_04::H::sound_release>(this); }  // 0x5f7f60
auto joan::orig::SysEn::light_slot() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers16::H::light_slot>(this)); }  // 0x5f8030
auto joan::orig::SysEn::light_slot_free(std::int32_t a0) -> void { joan::bind::member<&::en3d_helpers16::H::light_slot_free>(this, a0); }  // 0x5f80f0
auto joan::orig::SysEn::set_default_states() -> void { joan::bind::member<&::game_b92_05::H::set_default_states>(this); }  // 0x5f8110
auto joan::orig::SysEn::vs_version() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers22::H::sysen_vs_version>(this)); }  // 0x5f8220
auto joan::orig::PlaneEquation::data() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers2::H::identity_self>(this)); }  // 0x5fe900
auto joan::orig::SysEn::buffer_count() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b82_17::H::buffer_count>(this)); }  // 0x5ff060
auto joan::orig::SysEn::present() -> void { joan::bind::member<&::game_leaves82a::H::device_present>(this); }  // 0x5ff0e0
auto joan::orig::SysEn::flush_extra() -> void { joan::bind::member<&::game_b93_03::H::flush_extra>(this); }  // 0x5ff180
auto joan::orig::SysEn::flush_all() -> void { joan::bind::member<&::game_b93_03::H::flush_all>(this); }  // 0x5ff210
auto joan::orig::SysEn::reset_device() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_07::H::reset_device>(this)); }  // 0x5ff4a0
auto joan::orig::format_bits(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_03::format_bits>(a0)); }  // 0x5ff920
auto joan::orig::SysEn::fill_vertices(char a0, void* a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::sysen_fill_vertices>(this, a0, a1, a2)); }  // 0x600490
auto joan::orig::SysEn::fill_indices(void* a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers6::H::index_fill>(this, a0, a1, a2)); }  // 0x600510
auto joan::orig::SysEn::pass_filter(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::sysen_pass_filter>(this, a0, a1)); }  // 0x600b30
auto joan::orig::primitive_type(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::en3d_helpers::primitive_type>(a0)); }  // 0x600b60
auto joan::orig::Stats::begin_frame() -> void { joan::bind::member<&::game_leaves82a::H::stats_begin>(this); }  // 0x600df0
auto joan::orig::Stats::end_frame() -> void { joan::bind::member<&::game_leaves82a::H::stats_end>(this); }  // 0x600e50
auto joan::orig::SysEn::probe_bump() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b111_12::H::probe_bump>(this)); }  // 0x602540
auto joan::orig::SysEn::supports_mipmaps() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b95_06::H::is_mipmapped>(this)); }  // 0x602610
auto joan::orig::Mesh::flag_800() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b93_03::H::flag_800>(this)); }  // 0x602660
auto joan::orig::SysEn::set_brightness(float a0) -> void { joan::bind::member<&::game_b93_03::H::set_clamp_2ef>(this, a0); }  // 0x602a50
auto joan::orig::SysEn::set_contrast(float a0) -> void { joan::bind::member<&::game_b93_03::H::set_clamp_2f3>(this, a0); }  // 0x602ad0
auto joan::orig::D3D::set_gamma(float a0) -> void { joan::bind::member<&::game_fx::H::gamma_set>(this, a0); }  // 0x602b50
auto joan::orig::SysEn::set_rgb_offsets(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::member<&::game_b96_06::H::set_rgb_offsets>(this, a0, a1, a2); }  // 0x602bd0
auto joan::orig::D3D::apply_gamma() -> void { joan::bind::member<&::game_fx::H::gamma_apply>(this); }  // 0x602cd0
auto joan::orig::SysEn::set_cull_flip(std::int32_t a0) -> void { joan::bind::member<&::game_b94_06::H::set_cull>(this, a0); }  // 0x603140
auto joan::orig::RenderTarget::release_depth() -> void { joan::bind::member<&::game_b91_03::H::release_depth>(this); }  // 0x603380
auto joan::orig::SysEn::release_323() -> void { joan::bind::member<&::game_b93_03::H::release_323>(this); }  // 0x6034b0
