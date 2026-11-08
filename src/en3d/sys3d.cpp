// SPDX-License-Identifier: GPL-3.0-or-later
// en3d/sys3d.cpp: functions of the original d:\ww\en3d\sys3d.cpp (re/attribution.csv). The file name is the Restaurant
// Empire 2 engine's (en3d/Sys3d.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdarg>
#include "../compiler.hpp"

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
#include "../canvas_api.hpp"
#include "../render/overrides.hpp"
#include "../d3d8/d3d8_types.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

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

// ---------------------------------------- from modules/game_b116_10.cpp (fix/uninit)
namespace {
namespace game_b116_10 {

using joan::img::obj;

using joan::hook::global;

using joan::call::thiscall;
template <class R = void, class... A>
R win_call(img::Addr slot, A... args) {  // USER32 through its IAT slot (stdcall)
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}
template <class R = void, class... A>
R com_call(void* self, u32 slot, A... args) {  // COM method (stdcall, this first)
    void* const* const vtbl = joan::call::vtable_of(self);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(vtbl[slot])(self, args...);
}
u8* bp(void* p) { return static_cast<u8*>(p); }
u32 rd32(img::Addr a) {  // unaligned global dword
    u32 v;
    std::memcpy(&v, img::obj(a), sizeof v);
    return v;
}
void memset_(void* d, i32 v, u32 n) { std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }  // memset

JOAN_COUNTER(c_start, "Renderer::start_d3d");

// ---- 0x005f6c00 Renderer::start_d3d -----------------------------------------------------------
struct HStart {
    void* d3d() { return jo::view<void*>(jo::self<jo::SysEn>(this)->d3d); }
    void* dev() { return jo::view<void*>(jo::self<jo::SysEn>(this)->device); }
    i32 sys_file(img::Addr name) { return joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(str(name))); }  // 0x00590b80
    void log(img::Addr text) { joan::orig::warning_box(joan::orig::arg::mptr(obj(d::g_err)), joan::orig::arg::str(str(text))); }          // 0x00586bc0

    i32 start() {
        c_start.hit();
        u8* const self = bp(this);
        if (jo::self<jo::SysEn>(this)->started < 2) return 0;
        jo::self<jo::SysEn>(this)->device_type = 1;
        jo::self<jo::SysEn>(this)->frame_index = 0;
        jo::self<jo::SysEn>(this)->pp_back_buffer_width = 2;
        if (sys_file(d::s_MIXEDVP_SYS) != 0) jo::self<jo::SysEn>(this)->pp_back_buffer_width = 1;  // "MIXEDVP.SYS"
        if (sys_file(d::s_SOFTVP_SYS) != 0) jo::self<jo::SysEn>(this)->pp_back_buffer_width = 0;  // "SOFTVP.SYS"
        // mov [ebp-0xc], 400h @0x005f6c79 / mov [ebp-0x10], 300h @0x005f6c80 (display.cpp patches).
        const u32 want_w = joan::canvas::resolution_imm(joan::canvas::kModeWidthImm);
        const u32 want_h = joan::canvas::resolution_imm(joan::canvas::kModeHeightImm);
        i32 count = 0;          // [ebp-0x18]
        i32 best = -1;          // [ebp-0x81c]
        u32 best_rate = ~0u;    // [ebp-0x820]
        i32 best_bpp = -1;      // [ebp-0x824]
        u32 adapters[0x100];    // [ebp-0x418]
        u32 modes[0x100];       // [ebp-0x818]
        for (u32 a = 0; a < com_call<u32>(d3d(), 4) && count < 0x100; ++a) {           // GetAdapterCount
            for (u32 m = 0; m < com_call<u32>(d3d(), 6, a) && count < 0x100; ++m) {  // GetAdapterModeCount
                u32 dm[4];  // [ebp-0x83c] D3DDISPLAYMODE
                if (com_call<i32>(d3d(), 7, a, m, static_cast<void*>(dm)) != 0) continue;  // EnumAdapterModes
                if (dm[0] != want_w || dm[1] != want_h) continue;
                if (joan::orig::arg::as<i32>(joan::orig::format_bits(joan::orig::arg::i32(dm[3]))) == 0) continue;  // 0x005ff920
                if (a == rd32(d::g_a58036)) {
                    if (joan::orig::arg::as<i32>(joan::orig::format::bytes_per_pixel(joan::orig::arg::u32(dm[3]))) == 4 && dm[2] == rd32(d::g_a58046)) {  // 0x005ffe60
                        best = count;
                        best_rate = dm[2];
                        best_bpp = 4;
                    } else if (dm[2] == rd32(d::g_a58046)) {
                        if (best == -1 || best_bpp != 4) {
                            best = count;
                            best_rate = dm[2];
                            best_bpp = joan::orig::arg::as<i32>(joan::orig::format::bytes_per_pixel(joan::orig::arg::u32(dm[3])));
                        }
                    } else if (best_rate == rd32(d::g_a58046)) {
                    } else if (best_rate != rd32(d::g_a58046)) {
                        if (joan::orig::arg::as<i32>(joan::orig::format::bytes_per_pixel(joan::orig::arg::u32(dm[3]))) == 4) {
                            best = count;
                            best_rate = dm[2];
                            best_bpp = 4;
                        } else if (best == -1) {
                            best = count;
                            best_rate = dm[2];
                            best_bpp = joan::orig::arg::as<i32>(joan::orig::format::bytes_per_pixel(joan::orig::arg::u32(dm[3])));
                        }
                    }
                }
                adapters[count] = a;
                modes[count] = m;
                ++count;
            }
        }
        if (count == 0) return 0;
        const i32 k = best != -1 ? best : 0;  // [ebp-0x828]
        if (com_call<i32>(d3d(), 13, adapters[k], jo::self<jo::SysEn>(this)->device_type, static_cast<void*>(&jo::self<jo::SysEn>(this)->caps)) != 0) return 0;  // GetDeviceCaps
        if ((jo::view<i32>(jo::self<jo::SysEn>(this)->pp_back_buffer_width) == 1 || jo::view<i32>(jo::self<jo::SysEn>(this)->pp_back_buffer_width) == 2) && (jo::self<jo::SysEn>(this)->caps.dev_caps & 0x10000) == 0) jo::self<jo::SysEn>(this)->pp_back_buffer_width = 0;
        u32 mode[4];  // [ebp-0x84c]
        if (com_call<i32>(d3d(), 7, adapters[k], modes[k], static_cast<void*>(mode)) != 0) return 0;
        memset_(&jo::self<jo::SysEn>(this)->pp_back_buffer_height, 0, static_cast<u32>(offsetof(jo::SysEn, caps) - offsetof(jo::SysEn, pp_back_buffer_height)));  // D3DPRESENT_PARAMETERS (0x34 bytes on x86; the pp_* names run one field behind)
        jo::self<jo::SysEn>(this)->pp_back_buffer_height = mode[0];
        jo::self<jo::SysEn>(this)->pp_back_buffer_format = mode[1];
        jo::self<jo::SysEn>(this)->pp_back_buffer_count = joan::orig::arg::as<u32>(joan::orig::as_sys_en(self)->find_depth_format(joan::orig::arg::u32(adapters[k]), joan::orig::arg::u32(mode[3]), 0u));  // 0x005ff9d0
        jo::self<jo::SysEn>(this)->pp_multisample_type = 1;
        jo::self<jo::SysEn>(this)->pp_swap_effect = 0;
        jo::self<jo::SysEn>(this)->pp_device_window = global<i8>(d::g_a500a0) != 0 ? 2 : 1;
        jo::self<jo::SysEn>(this)->pp_windowed = global<void*>(d::g_main_hwnd);  // hDeviceWindow (an HWND)
        jo::self<jo::SysEn>(this)->pp_enable_auto_depth_stencil = 0;
        jo::self<jo::SysEn>(this)->pp_auto_depth_stencil_format = 1;
        jo::self<jo::SysEn>(this)->pp_flags = joan::orig::arg::as<u32>(joan::orig::as_sys_en(self)->pick_depth_format(joan::orig::arg::u32(adapters[k]), joan::orig::arg::u32(mode[3]), joan::orig::arg::u32(jo::self<jo::SysEn>(this)->pp_back_buffer_count)));  // 0x005ffc90
        if (jo::self<jo::SysEn>(this)->pp_flags == 0) jo::self<jo::SysEn>(this)->pp_auto_depth_stencil_format = 0;
        jo::self<jo::SysEn>(this)->pp_refresh_rate = 0;
        if (jo::self<jo::SysEn>(this)->pp_enable_auto_depth_stencil != 0)
            jo::self<jo::SysEn>(this)->pp_presentation_interval = 0;
        else if (mode[2] == 0)
            jo::self<jo::SysEn>(this)->pp_presentation_interval = 0;
        else
            jo::self<jo::SysEn>(this)->pp_presentation_interval = mode[2];
        if (jo::self<jo::SysEn>(this)->pp_enable_auto_depth_stencil == 0) jo::self<jo::SysEn>(this)->u32_4c = 0x80000000u;
        const u32 ex_style = jo::self<jo::SysEn>(this)->pp_enable_auto_depth_stencil != 0 ? 0x40000u : 0x40008u;  // [ebp-0x850]
        const u32 style = 0x92000000u;                                  // [ebp-0x854]
        i32 rc[4] = {0, 0, static_cast<i32>(mode[0]), static_cast<i32>(mode[1])};  // [ebp-0x864]
        win_call<i32>(d::iat_USER32_AdjustWindowRectEx, static_cast<void*>(rc), style, i32{0}, ex_style);                // AdjustWindowRectEx
        win_call<i32>(d::iat_USER32_SetWindowLongA, global<void*>(d::g_main_hwnd), i32{-16}, style);                     // SetWindowLongA
        win_call<i32>(d::iat_USER32_SetWindowLongA, global<void*>(d::g_main_hwnd), i32{-20}, ex_style);                  // SetWindowLongA
        win_call<i32>(d::iat_USER32_SetWindowPos, global<void*>(d::g_main_hwnd),  // SetWindowPos; HWND_TOP / HWND_TOPMOST = (HWND)-1, pointer-wide
                      reinterpret_cast<void*>(std::intptr_t{jo::self<jo::SysEn>(this)->pp_enable_auto_depth_stencil != 0 ? 0 : -1}),
                      rc[0], rc[1], rc[2] - rc[0], rc[3] - rc[1], u32{0x40});

        u32 flags = 0;  // [ebp-0x868]
        switch (jo::view<i32>(jo::self<jo::SysEn>(this)->pp_back_buffer_width)) {
            case 0: flags |= 0x20; break;  // SOFTWARE_VERTEXPROCESSING
            case 1: flags |= 0x80; break;  // MIXED
            case 2: flags |= 0x40; break;  // HARDWARE
            default: break;
        }
        const i32 hr = com_call<i32>(d3d(), 15, adapters[k], jo::self<jo::SysEn>(this)->device_type, global<void*>(d::g_main_hwnd), flags,  // CreateDevice
                                     static_cast<void*>(&jo::self<jo::SysEn>(this)->pp_back_buffer_height), static_cast<void*>(&jo::self<jo::SysEn>(this)->device));
        if (hr != 0) {
            const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_sys_en(self)->error_string(joan::orig::arg::i32(hr)));  // 0x006002b0
            joan::orig::warning_box(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_CreateDevice_fails_s), s);    // 0x00586bc0 "CreateDevice fails (%s)"
            return 0;
        }
        global<u32>(d::g_a69b28) = com_call<u32>(global<void*>(d::g_game_device), 4);  // GetAvailableTextureMem
        jo::self<jo::SysEn>(this)->resources_head = nullptr;
        jo::self<jo::SysEn>(this)->resources_tail = nullptr;
        if (jo::self<jo::SysEn>(this)->started < 3) jo::self<jo::SysEn>(this)->started = 3;
        if (com_call<i32>(dev(), 7, static_cast<void*>(&jo::self<jo::SysEn>(this)->caps)) != 0) {  // GetDeviceCaps
            log(d::s_Cannot_retrieve_device_caps);  // "Cannot retrieve device caps"
            return 0;
        }
        jo::self<jo::SysEn>(this)->vs_usable = 0;
        if (((jo::self<jo::SysEn>(this)->caps.vertex_shader_version >> 8) & 0xff) >= 1 && ((flags & 0x40) != 0 || (flags & 0x80) != 0)) jo::self<jo::SysEn>(this)->vs_usable = 1;
        jo::self<jo::SysEn>(this)->dot3_usable = jo::self<jo::SysEn>(this)->vs_usable;
        if (jo::self<jo::SysEn>(this)->caps.max_simultaneous_textures < 3) jo::self<jo::SysEn>(this)->dot3_usable = 0;
        if ((jo::self<jo::SysEn>(this)->caps.texture_op_caps & 0x800000) == 0) jo::self<jo::SysEn>(this)->dot3_usable = 0;
        if ((jo::self<jo::SysEn>(this)->caps.primitive_misc_caps & 0x400) == 0) jo::self<jo::SysEn>(this)->dot3_usable = 0;
        if (jo::self<jo::SysEn>(this)->vs_usable != 0 &&
            (((jo::self<jo::SysEn>(this)->caps.vertex_shader_version >> 8) & 0xff) >= 1 || (((jo::self<jo::SysEn>(this)->caps.vertex_shader_version >> 8) & 0xff) == 1 && (jo::self<jo::SysEn>(this)->caps.vertex_shader_version & 0xff) >= 1)))
            jo::self<jo::SysEn>(this)->hw_skinning = 1;
        else
            jo::self<jo::SysEn>(this)->hw_skinning = 0;
        if (sys_file(d::s_SOFTBONE_SYS) != 0) jo::self<jo::SysEn>(this)->hw_skinning = 0;  // "SOFTBONE.SYS"
        jo::self<jo::SysEn>(this)->mipmaps_supported = joan::orig::arg::as<u32>(joan::orig::as_sys_en(self)->supports_mipmaps()) & 0xff;  // 0x00602610
        jo::self<jo::SysEn>(this)->bump_supported = joan::orig::arg::as<u32>(joan::orig::as_sys_en(self)->probe_bump()) & 0xff;  // 0x00602540
        jo::self<jo::SysEn>(this)->texture_caps_800 = joan::orig::arg::as<u32>(joan::orig::as_mesh(self)->flag_800()) & 0xff;  // 0x00602660
        if (com_call<i32>(dev(), 16, 0u, 0u, static_cast<void*>(&jo::self<jo::SysEn>(this)->back_buffer)) != 0) {  // GetBackBuffer
            log(d::s_Cannot_create_surface);  // "Cannot create surface"
            return 0;
        }
        if (com_call<i32>(dev(), 33, static_cast<void*>(&jo::self<jo::SysEn>(this)->depth_stencil)) != 0) {  // GetDepthStencilSurface
            log(d::s_Cannot_create_z_buffer);  // "Cannot create z buffer"
            return 0;
        }
        if (jo::self<jo::SysEn>(this)->pp_auto_depth_stencil_format != 0) {
            jo::self<jo::SysEn>(this)->depth_format = jo::self<jo::SysEn>(this)->pp_flags;
        } else {
            u32 desc[8];  // [ebp-0x888] D3DSURFACE_DESC
            if (com_call<i32>(global<void*>(d::g_a52430), 8, static_cast<void*>(desc)) == 0)  // GetDesc
                jo::self<jo::SysEn>(this)->depth_format = desc[0];
            else
                jo::self<jo::SysEn>(this)->depth_format = 0;
        }
        switch (jo::self<jo::SysEn>(this)->depth_format) {
            case 0x49: jo::self<jo::SysEn>(this)->stencil_bits = 1; break;  // D15S1
            case 0x4b: jo::self<jo::SysEn>(this)->stencil_bits = 8; break;  // D24S8
            case 0x4f: jo::self<jo::SysEn>(this)->stencil_bits = 4; break;  // D24X4S4
            default: jo::self<jo::SysEn>(this)->stencil_bits = 0; break;
        }
        jo::view<u32>(jo::self<jo::SysEn>(this)->user_clip_planes) = jo::self<jo::SysEn>(this)->caps.max_user_clip_planes;  // caps MaxUserClipPlanes
        if (jo::view<u32>(jo::self<jo::SysEn>(this)->user_clip_planes) == 0) {
            // fix/uninit: the original's plane local [ebp-0x898] is never written; zeros here.
            float plane[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            joan::orig::as_plane_equation(static_cast<void*>(plane))->init_425700();  // 0x00425700 (ctor, writes nothing)
            u32 bit = 0;                                              // [ebp-0x89c]
            for (jo::self<jo::SysEn>(this)->user_clip_planes = 0; jo::self<jo::SysEn>(this)->user_clip_planes < 6; jo::self<jo::SysEn>(this)->user_clip_planes = jo::self<jo::SysEn>(this)->user_clip_planes + 1) {
                switch (jo::view<u32>(jo::self<jo::SysEn>(this)->user_clip_planes)) {  // jump table 0x005f792f
                    case 0: bit = 1; break;
                    case 1: bit = 2; break;
                    case 2: bit = 4; break;
                    case 3: bit = 8; break;
                    case 4: bit = 0x10; break;
                    case 5: bit = 0x20; break;
                    default: break;
                }
                void* const p = joan::orig::arg::as<void*>(joan::orig::as_plane_equation(static_cast<void*>(plane))->data());  // 0x005fe900
                if (com_call<i32>(dev(), 48, 0u, p) != 0) break;                         // SetClipPlane
                if (com_call<i32>(dev(), 50, 0x98u, bit) != 0) break;                    // SetRenderState CLIPPLANEENABLE
            }
            com_call<i32>(dev(), 50, 0x98u, 0u);
        }
        for (u32 n = joan::orig::arg::as<u32>(joan::orig::as_sys_en(self)->buffer_count()); n != 0; --n) {  // 0x005ff060
            u32 clear_flags = 1;                                       // [ebp-0x8a4]
            if (n == 0) clear_flags |= 2;                              // (never: n > 0 here)
            com_call<i32>(dev(), 36, 0u, static_cast<const void*>(nullptr), clear_flags, 0xff000000u, 1.0f, 0u);  // Clear(.., 1.0f, 0): Z is a float parameter
            com_call<i32>(dev(), 15, static_cast<const void*>(nullptr), static_cast<const void*>(nullptr), static_cast<void*>(nullptr), static_cast<const void*>(nullptr));  // Present (pointers: x64 passes the 4th on the stack, 8 bytes)
        }
        switch (jo::view<i32>(jo::self<jo::SysEn>(this)->pp_back_buffer_width)) {
            case 0:
                jo::self<jo::SysEn>(this)->vb_usage = 0x218;
                jo::self<jo::SysEn>(this)->ib_usage = 0x218;
                break;
            case 1:
                jo::self<jo::SysEn>(this)->vb_usage = 0x218;
                jo::self<jo::SysEn>(this)->ib_usage = 0x218;
                break;
            case 2:
                jo::self<jo::SysEn>(this)->vb_usage = 0x208;
                jo::self<jo::SysEn>(this)->ib_usage = 0x208;
                break;
            default: break;
        }
        joan::orig::as_sys_en(self)->set_default_states();  // 0x005f8110
        jo::self<jo::SysEn>(this)->cur_blend = 0xf2;
        joan::orig::as_sys_en(self)->set_blend(0u);  // 0x005f8250
        jo::self<jo::SysEn>(this)->cur_texture_mode = 0x2000002;
        joan::orig::as_sys_en(self)->set_texture_mode(1u);  // 0x005fa250
        jo::self<jo::SysEn>(this)->cur_filter = 4;
        joan::orig::as_sys_en(self)->set_filter(0u);  // 0x005fec10
        joan::orig::as_sys_en(self)->set_flags(0u);  // 0x00602080
        joan::orig::as_sys_en(self)->create_buffers();      // 0x005f7ca0
        joan::orig::as_sys_en(self)->reset_stats();      // 0x00600c20
        jo::view<u32>(jo::self<jo::SysEn>(this)->brightness) = 0;
        jo::view<u32>(jo::self<jo::SysEn>(this)->contrast) = 0x3f800000;
        jo::view<u32>(jo::self<jo::SysEn>(this)->gamma) = 0x3f800000;
        jo::view<u32>(jo::self<jo::SysEn>(this)->gamma_offset[0]) = 0x14;
        jo::view<u32>(jo::self<jo::SysEn>(this)->gamma_offset[1]) = 0xf;
        jo::view<u32>(jo::self<jo::SysEn>(this)->gamma_offset[2]) = 7;
        return 1;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: the clip-plane probe's never-written 16-byte plane local -> zeros (plane disabled
// again right after; our OpenGL device ignores clip planes). Mode-search immediates through
// joan::canvas::resolution_imm.
FIX("Sys3D::init_driver", 0x005f6c00, &HStart::start);
#undef FIX

}  // namespace game_b116_10
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
JOAN_COUNTER(c_create_buffers, "SysEn::create_buffers");
JOAN_COUNTER(c_find_format, "SysEn::find_depth_format");

struct H {
    // 0x005f7ca0 SysEn::create_buffers
    void create_buffers() {
        c_create_buffers.hit();
        auto fail = [&](std::int32_t hr, img::Addr message) {
            const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_sys_en(this)->error_string(joan::orig::arg::i32(hr)));  // 0x006002b0
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), joan::orig::arg::str(str(message)), text);           // 0x00586cb0
        };
        for (std::int8_t i = 0; i < 9; i = static_cast<std::int8_t>(i + 1)) {
            const std::uint8_t* const desc = joan::orig::arg::as<std::uint8_t*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(i))));  // 0x00600200
            const std::uint32_t fvf = jo::as_vertex_format(desc)->fvf;
            const std::uint32_t length = jo::as_vertex_format(desc)->stride << 12;
            void* const out = &jo::self<jo::SysEn>(this)->dyn_vb[i];
            std::int32_t hr;
            if (i == 5)
                hr = com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0x5c, length, jo::self<jo::SysEn>(this)->vb_usage, 0u, 0u, out);
            else if (i == 6)
                hr = com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0x5c, length, jo::self<jo::SysEn>(this)->vb_usage | 0x40u, fvf, 0u, out);
            else
                hr = com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0x5c, length, jo::self<jo::SysEn>(this)->vb_usage, fvf, 0u, out);
            if (hr != 0) fail(hr, d::s_Cannot_create_vertex_buffer);  // "Cannot create vertex buffer (%s)"
            jo::self<jo::SysEn>(this)->dyn_vb_used[i] = 0;
        }
        const std::uint32_t format = joan::orig::arg::as<std::uint32_t>(joan::orig::index_format(joan::orig::arg::mptr(0)));  // 0x00600290
        const std::int32_t hr = com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 0x60, 0x8000u, jo::self<jo::SysEn>(this)->vb_usage, format, 0u,
                                         static_cast<void*>(&jo::self<jo::SysEn>(this)->dyn_ib));
        if (hr != 0) fail(hr, d::s_Cannot_create_index_buffer_s);  // "Cannot create index buffer (%s)"
        jo::self<jo::SysEn>(this)->dyn_ib_used = 0;
        jo::self<jo::SysEn>(this)->locked_vb = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::SysEn>(this)->locked_vertex_type) = 0x7f;
    }

    // 0x005ff9d0 SysEn::find_depth_format(adapter, format, windowed)
    std::uint32_t find_format(std::uint32_t adapter, std::uint32_t format, std::uint32_t windowed) {
        c_find_format.hit();
        std::uint32_t list_a[7] = {0x18, 0x19, 0x17, 0x1a, 0x15, 0x16, 0x14};
        std::uint32_t list_b[7] = {0x17, 0x18, 0x19, 0x1a, 0x15, 0x16, 0x14};
        std::uint32_t list_c[7] = {0x14, 0x17, 0x18, 0x19, 0x1a, 0x15, 0x16};
        std::uint32_t list_d[7] = {0x16, 0x15, 0x14, 0x17, 0x18, 0x19, 0x1a};
        const std::int32_t kind = joan::orig::arg::as<std::int32_t>(joan::orig::format::bytes_per_pixel(joan::orig::arg::u32(format)));  // 0x005ffe60
        std::uint32_t* list;
        std::int32_t count;
        if (kind == 2) {
            list = (format == 0x18 || format == 0x19) ? list_a : list_b;
            count = 7;
        } else if (kind == 3) {
            list = list_c;
            count = 7;
        } else if (kind == 4) {
            list = list_d;
            count = 7;
        } else {
            list = &format;
            count = 1;
        }
        for (std::int32_t i = 0; i < count; ++i) {
            if (com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->d3d), 0x28, adapter, jo::self<jo::SysEn>(this)->device_type, format, 1u, 1u, list[i]) != 0) continue;
            if (com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->d3d), 0x24, adapter, jo::self<jo::SysEn>(this)->device_type, format, list[i], windowed) != 0) continue;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sys_en(this)->pick_depth_format(joan::orig::arg::u32(adapter), joan::orig::arg::u32(format), joan::orig::arg::u32(list[i]))) != 0) return list[i];  // 0x005ffc90
        }
        return 0;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Sys3D::init_buffers", 0x005f7ca0, &H::create_buffers);
B94("Sys3D::select_back_buffer_format", 0x005ff9d0, &H::find_format);
#undef B94


}  // namespace game_b94_06
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
JOAN_COUNTER(c_depth_format, "SysEn::pick_depth_format");

struct H {
    // 0x005ffc90
    std::uint32_t pick_depth_format(std::uint32_t adapter, std::uint32_t adapter_format, std::uint32_t rt_format) {
        c_depth_format.hit();
        std::uint32_t list[4];
        if (global<std::int32_t>(d::g_a58042) == 0x20) {
            list[0] = 0x4b; list[1] = 0x4f; list[2] = 0x47; list[3] = 0x50;
        } else {
            list[0] = 0x50; list[1] = 0x4b; list[2] = 0x4f; list[3] = 0x47;
        }
        joan::orig::format::bytes_per_pixel(joan::orig::arg::u32(rt_format));  // 0x005ffe60 (selects between identical lists)
        for (std::int32_t i = 0; i < 4; ++i) {
            if (com_call(jo::self<jo::SysEn>(this)->d3d, 0x28, adapter, jo::self<jo::SysEn>(this)->device_type, adapter_format, 2u, 1u, list[i]) != 0)
                continue;  // CheckDeviceFormat
            if (com_call(jo::self<jo::SysEn>(this)->d3d, 0x30, adapter, jo::self<jo::SysEn>(this)->device_type, adapter_format, rt_format, list[i]) != 0)
                continue;  // CheckDepthStencilMatch
            return list[i];
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("Sys3D::select_depth_buffer_format", 0x005ffc90, &H::pick_depth_format);
#undef B


}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/en3d_b87_02.cpp (en3d/b87_02)
namespace {
namespace en3d_b87_02 {

// 0x005ffe60 (cdecl)
std::int32_t JOAN_CDECL bytes_per_pixel(std::uint32_t fmt) {
    switch (fmt) {
        case 20:
            return 3;
        case 21: case 22: case 62: case 63: case 64: case 65: case 71: case 75: case 77: case 79:
            return 4;
        case 23: case 24: case 25: case 26: case 29: case 30: case 40: case 51: case 60: case 61:
        case 70: case 73: case 80:
            return 2;
        case 27: case 28: case 41: case 50: case 52:
            return 1;
        default:
            return 0;
    }
}

// 0x005fff90 (cdecl)
std::int32_t JOAN_CDECL bit_info(std::uint32_t fmt) {
    switch (fmt) {
        case 20: return 5;
        case 21: return 0xb;
        case 22: return 7;
        case 23: return 3;
        case 24: case 25: return 2;
        case 41: return 8;
        case 60: return 0x1f;
        default: return -1;
    }
}

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b87_02", name, addr, fn)
EN3D("Sys3D::get_byte_per_pixel", 0x005ffe60, &bytes_per_pixel);
EN3D("Sys3D::convert_to_format_flag", 0x005fff90, &bit_info);
#undef EN3D

}  // namespace en3d_b87_02
}  // namespace

// ---------------------------------------- from modules/game_b86_01.cpp (en3d/b86_01)
namespace {
namespace game_b86_01 {

// 0x00600080 d3d_format(code), cdecl. Unsigned compare against 0x1f (ja), jump table
// 0x006000e5 indexed by the byte table 0x00600109.
std::uint32_t JOAN_CDECL d3d_format(std::uint32_t code) {
    if (code > 0x1f) return 0;
    switch (code) {
        case 2: return 0x18;    // D3DFMT_X1R5G5B5
        case 3: return 0x17;    // D3DFMT_R5G6B5
        case 5: return 0x14;    // D3DFMT_R8G8B8
        case 7: return 0x16;    // D3DFMT_X8R8G8B8
        case 8: return 0x29;    // D3DFMT_P8
        case 11: return 0x15;   // D3DFMT_A8R8G8B8
        case 31: return 0x3c;   // D3DFMT_V8U8
        default: return 0;
    }
}

#define EN3D86(name, addr, fn) JOAN_REPLACE("en3d/b86_01", name, addr, fn)
EN3D86("Sys3D::convert_from_format_flag", 0x00600080, &d3d_format);
#undef EN3D86

}  // namespace game_b86_01
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

using joan::img::obj;

using joan::hook::global;
using Device = joan::d3d8::IDirect3DDevice8;

using joan::call::thiscall;
constexpr img::Addr kSysEn(d::g_sys3d);

// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    std::uint32_t sysen_draw(std::int32_t prim, std::uint32_t start, std::uint32_t count) {  // 0x00600a00
        auto* const d = jo::view<Device*>(jo::self<jo::SysEn>(this)->device);
        d->DrawPrimitive(joan::orig::arg::as<std::uint32_t>(joan::orig::primitive_type(joan::orig::arg::i32(prim))), jo::self<jo::SysEn>(this)->locked_start + start, count);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(img::obj(kSysEn))->count(0u, joan::orig::arg::u32(prim >= 4 && prim <= 5 ? count : 0u)));
    }
    std::uint32_t sysen_draw_indexed(std::int32_t prim, std::uint32_t start, std::uint32_t count) {  // 0x00600a90
        auto* const d = jo::view<Device*>(jo::self<jo::SysEn>(this)->device);
        d->DrawIndexedPrimitive(joan::orig::arg::as<std::uint32_t>(joan::orig::primitive_type(joan::orig::arg::i32(prim))), 0, jo::self<jo::SysEn>(this)->locked_count,
                                jo::self<jo::SysEn>(this)->index_start + start, count);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(img::obj(kSysEn))->count(0u, joan::orig::arg::u32(prim >= 4 && prim <= 5 ? count : 0u)));
    }
};

const std::uint8_t* JOAN_CDECL vertex_format(char type) {  // 0x00600200: 16-byte table entries
    return &img::global<const std::uint8_t>(d::g_a17d88) + static_cast<std::int32_t>(type) * 16;
}

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Sys3D::draw_primitive", 0x00600a00, &H::sysen_draw);
HELPER("Sys3D::draw_indexed_primitive", 0x00600a90, &H::sysen_draw_indexed);
HELPER("Sys3D::get_vertex_type_info", 0x00600200, &vertex_format);
#undef HELPER


}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/en3d_helpers25.cpp (en3d/helpers25)
namespace {
namespace en3d_helpers25 {

std::uint32_t JOAN_CDECL index_format(const void*) { return 0x65; }  // 0x00600290: D3DFMT_INDEX16

#define HELPER25(name, addr, fn) JOAN_REPLACE("en3d/helpers25", name, addr, fn)
HELPER25("Sys3D::get_index_format", 0x00600290, &index_format);
#undef HELPER25


}  // namespace en3d_helpers25
}  // namespace

// ---------------------------------------- from modules/en3d_helpers8.cpp (en3d/helpers8)
namespace {
namespace en3d_helpers8 {

using joan::img::obj;

using joan::hook::global;

using joan::call::thiscall;
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {  // COM method by vtable index
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
JOAN_COUNTER(c_vertex_lock, "SysEn::vertex_lock");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- SysEn dynamic vertex buffers: per type +0x18f VB, +0x1b7 used (vertices) ----------------
    // 0x006005b0: lock room for `count` vertices (DISCARD past 0x1000, else NOOVERWRITE) and
    // remember VB / type / start / stride / count (+0x1df..+0x1ef) for 0x00600740.
    void* vertex_lock(std::int32_t type_arg, std::uint32_t count) {
        c_vertex_lock.hit();
        const std::int8_t type = static_cast<std::int8_t>(type_arg);
        const std::uint32_t stride = jo::as_vertex_format(joan::orig::arg::as<void*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(type)))))->stride;  // 0x00600200
        void* const vb = jo::view<void*>(jo::self<jo::SysEn>(this)->dyn_vb[type]);
        std::uint32_t* const used = &jo::self<jo::SysEn>(this)->dyn_vb_used[type];
        const std::uint32_t bytes = count * stride;
        void* data = nullptr;
        std::int32_t hr;
        if (*used == 0 || *used + count > 0x1000) {
            *used = 0;
            hr = com_call(vb, 11, 0u, bytes, &data, 0x2800u);  // Lock: DISCARD | NOSYSLOCK
        } else {
            hr = com_call(vb, 11, *used * stride, bytes, &data, 0x1800u);  // NOOVERWRITE | NOSYSLOCK
        }
        if (hr != 0)
            joan::orig::error::fatal(joan::orig::arg::mptr(img::obj(d::g_err)), joan::orig::arg::str(&img::global<const char>(d::s_Cannot_lock_vertex_buffer_s)), joan::orig::arg::as<const char*>(joan::orig::as_sys_en(this)->error_string(joan::orig::arg::i32(hr))));  // log "Cannot lock vertex buffer (%s)"
        const std::uint32_t start = *used;
        *used = *used + count;
        jo::view<void*>(jo::self<jo::SysEn>(this)->locked_vb) = vb;
        jo::self<jo::SysEn>(this)->locked_vertex_type = type;
        jo::self<jo::SysEn>(this)->locked_start = start;
        jo::self<jo::SysEn>(this)->locked_stride = stride;
        jo::self<jo::SysEn>(this)->locked_count = count;
        return data;
    }
};

#define HELPER8(name, addr, fn) JOAN_REPLACE("en3d/helpers8", name, addr, fn)
HELPER8("Sys3D::begin_submit_vertex", 0x006005b0, &H::vertex_lock);
#undef HELPER8

}  // namespace en3d_helpers8
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {

// A COM method by vtable index (stdcall, `this` first).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

struct H {
    // 0x00600820: lock room for `count` indices (DISCARD when it would pass 0x4000, else
    // NOOVERWRITE); remembers the start (+0x1f3) and count (+0x1f7).
    void* index_lock(const void*, std::uint32_t count, std::uint32_t) {
        void* data = nullptr;
        std::int32_t hr;
        void* const ib = jo::view<void*>(jo::self<jo::SysEn>(this)->dyn_ib);
        if (jo::self<jo::SysEn>(this)->dyn_ib_used == 0 || jo::self<jo::SysEn>(this)->dyn_ib_used + count > 0x4000) {
            jo::self<jo::SysEn>(this)->dyn_ib_used = 0;
            hr = com_call(ib, 11, 0u, count * 2, &data, 0x2800u);  // Lock: DISCARD | NOSYSLOCK
        } else {
            hr = com_call(ib, 11, jo::self<jo::SysEn>(this)->dyn_ib_used * 2, count * 2, &data, 0x1800u);  // NOOVERWRITE | NOSYSLOCK
        }
        if (hr != 0)
            joan::orig::error::fatal(joan::orig::arg::mptr(img::obj(d::g_err)), joan::orig::arg::str(&img::global<const char>(d::s_Cannot_lock_index_buffer_s)), joan::orig::arg::as<const char*>(joan::orig::as_sys_en(this)->error_string(joan::orig::arg::i32(hr))));  // log "Cannot lock index buffer (%s)"
        const std::uint32_t start = jo::self<jo::SysEn>(this)->dyn_ib_used;
        jo::self<jo::SysEn>(this)->dyn_ib_used = jo::self<jo::SysEn>(this)->dyn_ib_used + count;
        jo::self<jo::SysEn>(this)->index_start = start;
        jo::self<jo::SysEn>(this)->index_count = count;
        return data;
    }
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("Sys3D::begin_submit_index", 0x00600820, &H::index_lock);
#undef HELPER6


}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- from modules/game_b102_11.cpp (game/b102_11)
namespace {
namespace game_b102_11 {

JOAN_COUNTER(c_items_add, "Items::add");

struct H {
    // 0x00601000 (ret 4)
    u8* add_name(const char* name) {
        c_items_add.hit();
        joan::orig::dynarray(jo::view<void*>(jo::self<jo::SysEn>(this)->frame_timer))->linkin(nullptr);          // 0x00584620
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_pool(jo::view<void*>(jo::self<jo::SysEn>(this)->frame_timer))->last_used());  // 0x004b9c30
        if (e == nullptr) return e;
        reinterpret_cast<RenderLogEntry*>(e)->kind = 1;
        const char* src;
        if (name == nullptr)
            src = str(d::s_NULL);  // "NULL"
        else if (static_cast<i32>(static_cast<i8>(name[0])) == 0)
            src = str(d::s_unnamed);  // "(unnamed)"
        else
            src = name;
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<RenderLogEntry*>(e)->name)), joan::orig::arg::str(src), 0x20u);  // strncpy
        reinterpret_cast<RenderLogEntry*>(e)->name[0x20] = 0;
        return e;
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b102_11", name, addr, fn)
B11("Sys3D::log_texture_change", 0x00601000, &H::add_name);
#undef B11


}  // namespace game_b102_11
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

struct alignas(4) File {
    u8 bytes[0x210];  // 0x110 used (0x004179c0); room as in en3d_b83_03.cpp
};
struct alignas(4) Text {
    u8 bytes[0x194];  // 0x005a1170 string (frame slot 0x194 bytes)
};

JOAN_COUNTER(c_log_dump, "RenderLog::dump");

struct H {
    // ------------------------------------------------------------------ 0x00601140
    void log_dump(const char* name) {
        c_log_dump.hit();
        if (jo::view<void*>(jo::self<jo::SysEn>(this)->prev_frame_timer) == nullptr) return;
        File file;
        joan::orig::as_file(&file)->construct();           // 0x004179c0
        joan::orig::as_file(&file)->file_create(joan::orig::arg::str(name), 1, static_cast<char>(0));  // 0x00586f00 open
        Text text;
        joan::orig::as_string(&text)->construct();  // 0x005a1170
        for (i32 i = 1; i <= joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::SysEn>(this)->prev_frame_timer))->size(); ++i) {  // 0x00417ad0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::SysEn>(this)->prev_frame_timer))->get(joan::orig::arg::i32(i)));  // 0x00417200
            const i32 kind = static_cast<i32>(reinterpret_cast<const RenderLogEntry*>(e)->kind);
            if (kind == 1) {
                joan::orig::as_string(&text)->assign(str(d::s_SetTexture));                  // 0x005a1440 "SetTexture :"
                joan::orig::as_string(&text)->append(joan::orig::arg::str(reinterpret_cast<const RenderLogEntry*>(e)->name));  // 0x005a1550
                joan::orig::as_string(&text)->append(str(d::blob_9d05dc));                      // 0x005a1550
                const u32 len = joan::orig::arg::as<u32>(joan::orig::as_name(&text)->length());                    // 0x0045e110
                const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(&text)->c_str());  // 0x00417d90
                joan::orig::as_file(&file)->file_write(joan::orig::arg::ptr(s), joan::orig::arg::u32(len));  // 0x005872e0
            } else if (kind == 2) {
                joan::orig::as_string(&text)->assign(str(d::s_DrawPrimitive));            // 0x005a1440 "DrawPrimitive :"
                joan::orig::as_string(&text)->append(joan::orig::arg::i32(reinterpret_cast<const RenderLogEntry*>(e)->value));  // 0x005a15d0
                joan::orig::as_string(&text)->append(str(d::blob_9d05dc));                // 0x005a1550
                joan::orig::as_string(&text)->append(str(d::blob_9d05dc));                // 0x005a1550
                const u32 len = joan::orig::arg::as<u32>(joan::orig::as_name(&text)->length());                // 0x0045e110
                const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(&text)->c_str());  // 0x00417d90
                joan::orig::as_file(&file)->file_write(joan::orig::arg::ptr(s), joan::orig::arg::u32(len));  // 0x005872e0
            }
        }
        joan::orig::as_file(&file)->destruct();  // 0x005872b0 ~File
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Sys3D::dump_log", 0x00601140, &H::log_dump);
#undef B


}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

using joan::img::obj;

using joan::hook::global;

using joan::call::thiscall;
template <class T>
T& at(const void* p, i32 offset) {
    return *reinterpret_cast<T*>(static_cast<u8*>(const_cast<void*>(p)) + offset);
}

JOAN_COUNTER(c_tex_create, "Texture::create_603400");

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x00603400
    std::uintptr_t tex_create(u32 w, u32 h) {
        c_tex_create.hit();
        const i32 hr = joan::orig::arg::as<i32>(joan::orig::d3dx::create_texture(joan::orig::arg::mptr(global<void*>(d::g_game_device)), joan::orig::arg::u32(w), joan::orig::arg::u32(h), joan::orig::arg::u32(u32{1}), joan::orig::arg::u32(u32{1}), joan::orig::arg::u32(jo::self<jo::SysEn>(this)->pp_back_buffer_count), joan::orig::arg::u32(u32{0}), joan::orig::arg::mptr(&jo::self<jo::SysEn>(this)->light_target)));  // D3DXCreateTexture
        if (hr != 0) {
            const char* const msg = joan::orig::arg::as<const char*>(joan::orig::as_sys_en(me())->error_string(joan::orig::arg::i32(hr)));  // 0x006002b0 (a %s vararg: pointer-wide)
            return cdecl_call<u32>(fn::Error_msg, obj(d::g_err), str(d::s_CreateTexture_fails_s), msg);  // 0x00586bc0
        }
        jo::self<jo::SysEn>(this)->light_target_width = w;
        jo::self<jo::SysEn>(this)->light_target_height = h;
        return reinterpret_cast<std::uintptr_t>(me());  // eax = this (no caller reads it as a pointer)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("Sys3D::init_extra_render_target", 0x00603400, &H::tex_create);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b102_11.cpp (game/b102_11)
namespace {
namespace game_b102_11_2 {

JOAN_COUNTER(c_items_add, "Items::add");

struct H {
    // 0x006010d0 (ret 4)
    u8* add_value(u32 v) {
        c_items_add.hit();
        joan::orig::dynarray(jo::view<void*>(jo::self<jo::SysEn>(this)->frame_timer))->linkin(nullptr);          // 0x00584620
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_pool(jo::view<void*>(jo::self<jo::SysEn>(this)->frame_timer))->last_used());  // 0x004b9c30
        if (e != nullptr) {
            reinterpret_cast<RenderLogEntry*>(e)->kind = 2;
            reinterpret_cast<RenderLogEntry*>(e)->value = static_cast<i32>(v);
        }
        return e;
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b102_11", name, addr, fn)
B11("Sys3D::log_draw_primitive", 0x006010d0, &H::add_value);
#undef B11

}  // namespace game_b102_11_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16 {

struct H {
    // 0x006002b0: D3D error text into the static buffer 0xa69ad8 (hex number when unknown).
    char* error_string(std::int32_t hr) {
        char* const buffer = &img::global<char>(d::g_a69ad8);
        if (joan::orig::arg::as<std::int32_t>(joan::orig::d3dx::get_error_string_a(joan::orig::arg::i32(hr), joan::orig::arg::str(buffer), 0x50u)) == 0) return buffer;  // D3DXGetErrorStringA
        return joan::orig::arg::as<char*>(joan::orig::libc::_ultoa(joan::orig::arg::u32(hr), joan::orig::arg::str(buffer), 0x10));                             // _itoa
    }
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
HELPER16("Sys3D::d3d_error_string", 0x006002b0, &H::error_string);
#undef HELPER16

}  // namespace en3d_helpers_216
}  // namespace

// ---------------------------------------- from modules/en3d_helpers25.cpp (en3d/helpers25)
namespace {
namespace en3d_helpers25_2 {

struct H {
    // --- en3d/helpers25 ---------------------------------------------------------------------
    // 0x005ff290 / 0x005ff380 SysEn: resource list (+0x32f head, +0x333 tail; node +0xc owner,
    // +0x10 / +0x14 links).
    void sysen_link(std::uint8_t* node) {
        if (jo::view<void*>(jo::as_resource(node)->owner) != nullptr) return;
        jo::view<void*>(jo::as_resource(node)->owner) = this;
        std::uint8_t* const tail = jo::view<std::uint8_t*>(jo::self<jo::SysEn>(this)->resources_tail);
        std::uint8_t* prev;
        if (tail == nullptr) {
            prev = jo::view<std::uint8_t*>(jo::self<jo::SysEn>(this)->resources_head);
            jo::view<std::uint8_t*>(jo::as_resource(node)->next) = jo::view<std::uint8_t*>(jo::self<jo::SysEn>(this)->resources_head);
            jo::view<std::uint8_t*>(jo::self<jo::SysEn>(this)->resources_head) = node;
        } else {
            prev = jo::view<std::uint8_t*>(jo::as_resource(tail)->next);
            jo::view<std::uint8_t*>(jo::as_resource(node)->next) = jo::view<std::uint8_t*>(jo::as_resource(tail)->next);
            jo::view<std::uint8_t*>(jo::as_resource(tail)->next) = node;
        }
        if (prev == nullptr) {
            jo::view<std::uint8_t*>(jo::as_resource(node)->prev) = jo::view<std::uint8_t*>(jo::self<jo::SysEn>(this)->resources_tail);
            jo::view<std::uint8_t*>(jo::self<jo::SysEn>(this)->resources_tail) = node;
        } else {
            jo::view<std::uint8_t*>(jo::as_resource(node)->prev) = jo::view<std::uint8_t*>(jo::as_resource(prev)->prev);
            jo::view<std::uint8_t*>(jo::as_resource(prev)->prev) = node;
        }
    }
    void sysen_unlink(std::uint8_t* node) {
        if (jo::view<void*>(jo::as_resource(node)->owner) == nullptr) return;
        std::uint8_t* const a = jo::view<std::uint8_t*>(jo::as_resource(node)->next);
        std::uint8_t* const b = jo::view<std::uint8_t*>(jo::as_resource(node)->prev);
        if (b == nullptr)
            jo::view<std::uint8_t*>(jo::self<jo::SysEn>(this)->resources_head) = a;
        else
            jo::view<std::uint8_t*>(jo::as_resource(b)->next) = a;
        if (a == nullptr)
            jo::view<std::uint8_t*>(jo::self<jo::SysEn>(this)->resources_tail) = b;
        else
            jo::view<std::uint8_t*>(jo::as_resource(a)->prev) = b;
        jo::view<void*>(jo::as_resource(node)->owner) = nullptr;
    }
};

#define HELPER25(name, addr, fn) JOAN_REPLACE("en3d/helpers25", name, addr, fn)
HELPER25("Sys3D::linkin_resource", 0x005ff290, &H::sysen_link);
HELPER25("Sys3D::linkout_resource", 0x005ff380, &H::sysen_unlink);
#undef HELPER25

}  // namespace en3d_helpers_225_2
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
struct H {
    std::int32_t device_lost() {                                                         // 0x005ff420
        if (jo::view<void*>(jo::self<jo::SysEn>(this)->device) == nullptr) return -1;
        const std::uint32_t hr = com_call<std::uint32_t>(jo::view<void*>(jo::self<jo::SysEn>(this)->device), 3);  // TestCooperativeLevel
        if (hr == 0) return 0;
        if (hr == 0x88760869u) return 1;   // D3DERR_DEVICENOTRESET
        return -1;                         // D3DERR_DEVICELOST and others
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("Sys3D::is_device_lost", 0x005ff420, &H::device_lost);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
namespace {
namespace game_b103_11 {

struct H {
    // 0x00600340
    u32 call_600490(u32 a, u32 b) { return joan::orig::arg::as<u32>(joan::orig::as_sys_en(this)->fill_vertices(static_cast<char>(joan::orig::arg::i32(1)), joan::orig::arg::mptr(a), joan::orig::arg::u32(b))); }  // 0x00600490
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Sys3D::submit_vertex_600340", 0x00600340, &H::call_600490);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers_2 {

// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    std::uint32_t sysen_user_vertices(const void* data, std::uint32_t count) {  // 0x00600310
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(self())->fill_vertices(static_cast<char>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(data), joan::orig::arg::u32(count)));
    }
};

// --- free (cdecl) helpers -----------------------------------------------------------------------
#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Sys3D::submit_vertex_600310", 0x00600310, &H::sysen_user_vertices);
#undef HELPER

}  // namespace en3d_helpers_2
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

struct H {
    // ------------------------------------------------------------------ 0x00600370..0x00600460
    u32 call_600490_2(u32 a, u32 b) { return joan::orig::arg::as<u32>(joan::orig::as_sys_en(this)->fill_vertices(static_cast<char>(joan::orig::arg::i32(2)), joan::orig::arg::mptr(a), joan::orig::arg::u32(b))); }  // 0x00600490
    u32 call_600490_3(u32 a, u32 b) { return joan::orig::arg::as<u32>(joan::orig::as_sys_en(this)->fill_vertices(static_cast<char>(joan::orig::arg::i32(3)), joan::orig::arg::mptr(a), joan::orig::arg::u32(b))); }  // 0x00600490
    u32 call_600490_4(u32 a, u32 b) { return joan::orig::arg::as<u32>(joan::orig::as_sys_en(this)->fill_vertices(static_cast<char>(joan::orig::arg::i32(4)), joan::orig::arg::mptr(a), joan::orig::arg::u32(b))); }  // 0x00600490
    u32 call_600490_5(u32 a, u32 b) { return joan::orig::arg::as<u32>(joan::orig::as_sys_en(this)->fill_vertices(static_cast<char>(joan::orig::arg::i32(5)), joan::orig::arg::mptr(a), joan::orig::arg::u32(b))); }  // 0x00600490
    u32 call_600490_6(u32 a, u32 b) { return joan::orig::arg::as<u32>(joan::orig::as_sys_en(this)->fill_vertices(static_cast<char>(joan::orig::arg::i32(6)), joan::orig::arg::mptr(a), joan::orig::arg::u32(b))); }  // 0x00600490
    u32 call_600490_8(u32 a, u32 b) { return joan::orig::arg::as<u32>(joan::orig::as_sys_en(this)->fill_vertices(static_cast<char>(joan::orig::arg::i32(8)), joan::orig::arg::mptr(a), joan::orig::arg::u32(b))); }  // 0x00600490
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Sys3D::submit_vertex_600370", 0x00600370, &H::call_600490_2);
B("Sys3D::submit_vertex_6003a0", 0x006003a0, &H::call_600490_3);
B("Sys3D::submit_vertex_6003d0", 0x006003d0, &H::call_600490_4);
B("Sys3D::submit_vertex_600400", 0x00600400, &H::call_600490_5);
B("Sys3D::submit_vertex_600430", 0x00600430, &H::call_600490_6);
B("Sys3D::submit_vertex_600460", 0x00600460, &H::call_600490_8);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6_2 {

using joan::hook::global;
using Device = joan::d3d8::IDirect3DDevice8;
// A COM method by vtable index (stdcall, `this` first).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
Device* game_device() { return global<Device*>(d::g_game_device); }

struct H {
    // --- SysEn dynamic index buffer (+0x1b3 IB, +0x1db used, +0x1e7 base vertex) ----------------
    // 0x00600980: unlock, bind as indices with the base vertex; returns the start index.
    std::uint32_t index_unlock() {
        const std::uint32_t start = jo::self<jo::SysEn>(this)->index_start;
        com_call(jo::view<void*>(jo::self<jo::SysEn>(this)->dyn_ib), 12);  // Unlock
        game_device()->SetIndices(jo::view<joan::d3d8::IDirect3DIndexBuffer8*>(jo::self<jo::SysEn>(this)->dyn_ib), jo::self<jo::SysEn>(this)->locked_start);
        return start;
    }
    // 0x00600740: unlock the dynamic vertex buffer (+0x1df) and bind it (stride +0x1eb) with
    // the index buffer, then set its vertex type (+0x1e3); returns the base vertex (+0x1e7).
    std::uint32_t vertex_unlock() {
        void* const vb = jo::view<void*>(jo::self<jo::SysEn>(this)->locked_vb);
        const std::uint8_t type = jo::view<std::uint8_t[4]>(jo::self<jo::SysEn>(this)->locked_vertex_type)[0];
        const std::uint32_t base = jo::self<jo::SysEn>(this)->locked_start;
        const std::uint32_t stride = jo::self<jo::SysEn>(this)->locked_stride;
        com_call(vb, 12);  // Unlock
        game_device()->SetStreamSource(0, static_cast<joan::d3d8::IDirect3DVertexBuffer8*>(vb), stride);
        game_device()->SetIndices(jo::view<joan::d3d8::IDirect3DIndexBuffer8*>(jo::self<jo::SysEn>(this)->dyn_ib), jo::self<jo::SysEn>(this)->locked_start);
        joan::orig::as_sys_en(this)->set_vertex_type(static_cast<char>(joan::orig::arg::i32(static_cast<char>(type))));  // 0x006024e0
        return base;
    }
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("Sys3D::end_submit_index", 0x00600980, &H::index_unlock);
HELPER6("Sys3D::end_submit_vertex", 0x00600740, &H::vertex_unlock);
#undef HELPER6

}  // namespace en3d_helpers_26_2
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {


void* crt_memset(void* d, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

JOAN_COUNTER(c_reset_600c20, "Obj::reset_600c20");

struct H {
    // 0x00600c20
    void reset_600c20() {
        c_reset_600c20.hit();
        crt_memset(jo::self<jo::SysEn>(this)->stats, 0, 0x2c);
        crt_memset(jo::self<jo::SysEn>(this)->stats_last, 0, 0x2c);
        crt_memset(jo::self<jo::SysEn>(this)->stats_sum, 0, 0x2c);
        crt_memset(jo::self<jo::SysEn>(this)->stats_avg, 0, 0x2c);
        jo::self<jo::Renderer>(this)->stats_frames = 0;  // the object is the Renderer (SysEn); Stats is a partial view
        if (jo::view<void*>(jo::self<jo::Renderer>(this)->frame_timer) == nullptr) {
            void* m = joan::crt::operator_new(static_cast<u32>(sizeof(jo::DynArray)));  // operator new (0x1d on x86)
            void* l = m != nullptr ? joan::orig::dynarray(m)->construct(0x28, 0x1e, 0) : nullptr;   // 0x005843d0
            jo::view<void*>(jo::self<jo::Renderer>(this)->frame_timer) = l;
        } else {
            joan::orig::dynarray(jo::view<void*>(jo::self<jo::Renderer>(this)->frame_timer))->zap(0);          // 0x00584580
        }
        if (jo::view<void*>(jo::self<jo::Renderer>(this)->prev_frame_timer) == nullptr) {
            void* m = joan::crt::operator_new(static_cast<u32>(sizeof(jo::DynArray)));  // operator new (0x1d on x86)
            void* l = m != nullptr ? joan::orig::dynarray(m)->construct(0x28, 0x1e, 0) : nullptr;   // 0x005843d0
            jo::view<void*>(jo::self<jo::Renderer>(this)->prev_frame_timer) = l;
        } else {
            joan::orig::dynarray(jo::view<void*>(jo::self<jo::Renderer>(this)->prev_frame_timer))->zap(0);          // 0x00584580
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Sys3D::init_profile_counts", 0x00600c20, &H::reset_600c20);
#undef B

}  // namespace game_b97_06
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
constexpr img::Addr kLog(d::g_err);

JOAN_COUNTER(c_depth, "RenderTarget::create_depth");

struct H {
    // 0x006032d0
    void create_depth(std::uint32_t width, std::uint32_t height) {
        c_depth.hit();
        void* const device = global<void*>(d::g_game_device);
        const std::int32_t hr = com_call(device, 0x68 / 4, width, height, jo::self<jo::SysEn>(this)->depth_format, 0u,
                                         &jo::self<jo::SysEn>(this)->depth_target);  // CreateDepthStencilSurface
        if (hr != 0) {
            const char* const error = joan::orig::arg::as<const char*>(joan::orig::as_sys_en(this)->error_string(joan::orig::arg::i32(hr)));  // 0x006002b0
            joan::orig::warning_box(joan::orig::arg::mptr(img::obj(kLog)), joan::orig::arg::str(&img::global<const char>(d::s_CreateDepthStencilSurface_fa)), error);  // 0x00586bc0
        } else {
            jo::self<jo::SysEn>(this)->depth_target_width = width;
            jo::self<jo::SysEn>(this)->depth_target_height = height;
        }
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("RenderTarget::create_depth", 0x006032d0, &H::create_depth);
#undef B91


}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SysEn::create_buffers() -> void { joan::bind::member<&::game_b94_06::H::create_buffers>(this); }  // 0x5f7ca0
auto joan::orig::SysEn::find_depth_format(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_06::H::find_format>(this, a0, a1, a2)); }  // 0x5ff9d0
auto joan::orig::SysEn::pick_depth_format(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_05::H::pick_depth_format>(this, a0, a1, a2)); }  // 0x5ffc90
auto joan::orig::format::bytes_per_pixel(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::en3d_b87_02::bytes_per_pixel>(a0)); }  // 0x5ffe60
auto joan::orig::format::bit_info(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::en3d_b87_02::bit_info>(a0)); }  // 0x5fff90
auto joan::orig::d3d_format(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b86_01::d3d_format>(a0)); }  // 0x600080
auto joan::orig::vertex_format(char a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::en3d_helpers::vertex_format>(a0)); }  // 0x600200
auto joan::orig::index_format(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::en3d_helpers25::index_format>(a0)); }  // 0x600290
auto joan::orig::SysEn::vertex_lock(std::int32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers8::H::vertex_lock>(this, a0, a1)); }  // 0x6005b0
auto joan::orig::SysEn::index_lock(void* a0, std::uint32_t a1, std::uint32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers6::H::index_lock>(this, a0, a1, a2)); }  // 0x600820
auto joan::orig::SysEn::draw(std::int32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::sysen_draw>(this, a0, a1, a2)); }  // 0x600a00
auto joan::orig::SysEn::draw_indexed(std::int32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::sysen_draw_indexed>(this, a0, a1, a2)); }  // 0x600a90
auto joan::orig::SysEn::dump_log(const char* a0) -> void { joan::bind::member<&::game_b100_11::H::log_dump>(this, a0); }  // 0x601140
auto joan::orig::Texture::create_603400(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_10::H::tex_create>(this, a0, a1)); }  // 0x603400
auto joan::orig::SysEn::link_resource(void* a0) -> void { joan::bind::member<&::en3d_helpers25_2::H::sysen_link>(this, a0); }  // 0x5ff290
auto joan::orig::SysEn::unlink_resource(void* a0) -> void { joan::bind::member<&::en3d_helpers25_2::H::sysen_unlink>(this, a0); }  // 0x5ff380
auto joan::orig::D3D::device_lost() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_world::H::device_lost>(this)); }  // 0x5ff420
auto joan::orig::SysEn::error_string(std::int32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::en3d_helpers16::H::error_string>(this, a0)); }  // 0x6002b0
auto joan::orig::SysEn::user_vertices(void* a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers_2::H::sysen_user_vertices>(this, a0, a1)); }  // 0x600310
auto joan::orig::SysEn::vertex_unlock() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers6_2::H::vertex_unlock>(this)); }  // 0x600740
auto joan::orig::SysEn::index_unlock() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers6_2::H::index_unlock>(this)); }  // 0x600980
auto joan::orig::SysEn::reset_stats() -> void { joan::bind::member<&::game_b97_06::H::reset_600c20>(this); }  // 0x600c20
auto joan::orig::RenderTarget::create_depth(std::uint32_t a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b91_03::H::create_depth>(this, a0, a1); }  // 0x6032d0
