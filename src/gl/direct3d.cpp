// IDirect3D8 of the OpenGL renderer. Adapter queries are answered natively (gl/adapter.*: display devices / modes,
// a fixed caps table equal to the real runtime's on current hardware) - the system's Direct3D 8 is never created -
// and the caps are adjusted to what the OpenGL device implements (patch_caps). Texture, surface, cube / volume
// texture formats and multisampling are answered from gl/formats.hpp (no multisampling).
//
// D3DX's texture loader (and our port of it, lib/d3dx8/texture_in_memory.cpp + render/image.cpp) takes
// its size / mip / format decisions from GetDeviceCaps (TextureCaps POW2 / SQUAREONLY / MIPMAP,
// max sizes) and CheckDeviceFormat: see gl/adapter.hpp before changing any answer here.
//
// Logging (openjoan.log): the answers are dumped once per session ("d3d8 adapter:" lines) and each distinct query
// the game / D3DX makes is logged once with its answer ("d3d8 query:" lines).
#include "../platform/win32_types.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <set>
#include <string>

#include "../display.hpp"
#include "../registry.hpp"
#include "adapter.hpp"
#include "device.hpp"
#include "../platform/window.hpp"

namespace joan::gld {

void log_unimplemented(const char* name) {
    static std::mutex m;
    static std::set<std::string> seen;
    std::lock_guard lock(m);
    if (seen.insert(name).second) hook::log_line("OpenGL device: %s is not implemented (first call)", name);
}

namespace {
/// The limits of our OpenGL device, applied to the native caps table.
void patch_caps(Caps8* c) {
    c->pixel_shader_version = 0xffff0000;  // ps.0.0: the fixed-function pixel pipeline only
    c->max_pixel_shader_value = 0.0f;
    c->vertex_shader_version = 0xfffe0101;
    c->max_vertex_shader_const = kVsConstants;
    c->max_active_lights = kMaxLights;
    c->max_user_clip_planes = 0;
    if (c->max_texture_blend_stages > kMaxStages) c->max_texture_blend_stages = kMaxStages;
    if (c->max_simultaneous_textures > kMaxStages) c->max_simultaneous_textures = kMaxStages;
    c->max_streams = 1;
    c->texture_op_caps &= ~0x00010000u;  // PREMODULATE is not emulated
}

const char* rname(HRESULT r, char (&buf)[16]) { return adapter::result_name(r, buf); }
}  // namespace

HRESULT Direct3D::RegisterSoftwareDevice(void*) {
    log_unimplemented("IDirect3D8::RegisterSoftwareDevice");
    return kInvalidCall;
}

UINT Direct3D::GetAdapterCount() {
    const UINT n = adapter::count();
    if (adapter::first_query("GetAdapterCount")) hook::log_line("d3d8 query: GetAdapterCount -> %u", n);
    return n;
}

HRESULT Direct3D::GetAdapterIdentifier(UINT a, DWORD flags, void* id) {
    if (!id || a >= adapter::count()) return kInvalidCall;
    adapter::identifier(a, *static_cast<adapter::Identifier8*>(id), flags);
    const HRESULT r = kOk;
    if (adapter::first_query("GetAdapterIdentifier", a, flags, static_cast<std::uint32_t>(r))) {
        char rb[16];
        hook::log_line("d3d8 query: GetAdapterIdentifier(adapter %u, flags 0x%x) -> %s", a, static_cast<unsigned>(flags),
                       rname(r, rb));
        adapter::log_identifier("query", a, *static_cast<const adapter::Identifier8*>(id));
    }
    return r;
}

const std::vector<DisplayMode>& Direct3D::modes(UINT adapter) {
    auto it = modes_.find(adapter);
    if (it != modes_.end()) return it->second;
    std::vector<DisplayMode> list = adapter::modes(adapter);
    // A [Display] size the monitor does not offer (we scale it at Present) is added as an extra mode.
    const display::Size want = display::game_size();
    bool found = false;
    UINT refresh = 60;
    for (const DisplayMode& m : list) {
        if (m.format == kFmtX8R8G8B8) refresh = std::max(refresh, m.refresh_rate);
        found |= m.width == static_cast<UINT>(want.width) && m.height == static_cast<UINT>(want.height) &&
                 m.format == kFmtX8R8G8B8;
    }
    if (!found) {
        list.push_back({static_cast<UINT>(want.width), static_cast<UINT>(want.height), refresh, kFmtX8R8G8B8});
        hook::log_line("OpenGL device: adapter %u has no %dx%d mode - added", adapter, want.width, want.height);
    }
    return modes_[adapter] = std::move(list);
}

UINT Direct3D::GetAdapterModeCount(UINT adapter) {
    if (adapter >= adapter::count()) return 0;
    const UINT n = static_cast<UINT>(modes(adapter).size());
    if (adapter::first_query("GetAdapterModeCount", adapter, n))
        hook::log_line("d3d8 query: GetAdapterModeCount(adapter %u) -> %u (incl. the [Display] size)", adapter, n);
    return n;
}

HRESULT Direct3D::EnumAdapterModes(UINT adapter, UINT mode, void* out) {
    if (adapter >= adapter::count()) return kInvalidCall;
    const auto& list = modes(adapter);
    if (adapter::first_query("EnumAdapterModes", adapter)) {
        hook::log_line("d3d8 query: EnumAdapterModes(adapter %u, ...) - the game reads this list:", adapter);
        adapter::log_modes("native", adapter, list);
    }
    if (!out || mode >= list.size()) return kInvalidCall;
    std::memcpy(out, &list[mode], sizeof(DisplayMode));
    return kOk;
}

HRESULT Direct3D::GetAdapterDisplayMode(UINT a, void* dm) {
    DisplayMode m = {};
    const HRESULT r = dm && adapter::current_mode(a, m) ? kOk : kInvalidCall;
    if (r == kOk) std::memcpy(dm, &m, sizeof m);
    if (r == kOk) {
        if (adapter::first_query("GetAdapterDisplayMode", a, m.width, m.height, m.refresh_rate, m.format))
            hook::log_line("d3d8 query: GetAdapterDisplayMode(adapter %u) -> %ux%u@%u %s", a, m.width, m.height,
                           m.refresh_rate, adapter::format_name(m.format));
    } else if (adapter::first_query("GetAdapterDisplayMode", a, static_cast<std::uint32_t>(r))) {
        char rb[16];
        hook::log_line("d3d8 query: GetAdapterDisplayMode(adapter %u) -> %s", a, rname(r, rb));
    }
    return r;
}

HRESULT Direct3D::CheckDeviceType(UINT a, Enum t, Enum df, Enum bf, BOOL w) {
    const HRESULT r = adapter::check_device_type(a, t, df, bf, w);
    if (adapter::first_query("CheckDeviceType", a, t, df, bf, static_cast<std::uint32_t>(w))) {
        char rb[16];
        hook::log_line("d3d8 query: CheckDeviceType(adapter %u, type %u, display %s, back buffer %s, %s) -> %s", a, t,
                       adapter::format_name(df), adapter::format_name(bf), w ? "windowed" : "full screen", rname(r, rb));
    }
    return r;
}

HRESULT Direct3D::CheckDeviceFormat(UINT a, Enum t, Enum af, DWORD usage, Enum rtype, Enum cf) {
    HRESULT r;
    if (usage & kUsageDepthStencil)
        r = adapter::check_device_format(a, t, af, usage, rtype, cf);
    else if (rtype == kRtTexture || rtype == kRtSurface)
        r = format_info(cf).supported ? kOk : kNotAvailable;
    else if (rtype == kRtCubeTexture || rtype == kRtVolumeTexture)
        r = kNotAvailable;
    else
        r = adapter::check_device_format(a, t, af, usage, rtype, cf);
    if (adapter::first_query("CheckDeviceFormat", a, t, af, usage, rtype, cf)) {
        char rb[16];
        hook::log_line("d3d8 query: CheckDeviceFormat(adapter %u, type %u, adapter format %s, usage 0x%x, rtype %u, %s) -> %s",
                       a, t, adapter::format_name(af), static_cast<unsigned>(usage), rtype, adapter::format_name(cf),
                       rname(r, rb));
    }
    return r;
}

HRESULT Direct3D::CheckDeviceMultiSampleType(UINT a, Enum t, Enum f, BOOL w, Enum ms) {
    const HRESULT r = ms == 0 ? kOk : kNotAvailable;  // our device does not multisample
    if (adapter::first_query("CheckDeviceMultiSampleType", a, t, f, static_cast<std::uint32_t>(w), ms)) {
        char rb[16];
        hook::log_line("d3d8 query: CheckDeviceMultiSampleType(adapter %u, type %u, %s, %s, %u samples) -> %s", a, t,
                       adapter::format_name(f), w ? "windowed" : "full screen", ms, rname(r, rb));
    }
    return r;
}

HRESULT Direct3D::CheckDepthStencilMatch(UINT a, Enum t, Enum af, Enum rf, Enum df) {
    const HRESULT r = adapter::check_depth_stencil_match(a, t, af, rf, df);
    if (adapter::first_query("CheckDepthStencilMatch", a, t, af, rf, df)) {
        char rb[16];
        hook::log_line("d3d8 query: CheckDepthStencilMatch(adapter %u, type %u, adapter format %s, target %s, depth %s) -> %s",
                       a, t, adapter::format_name(af), adapter::format_name(rf), adapter::format_name(df), rname(r, rb));
    }
    return r;
}

HRESULT Direct3D::GetDeviceCaps(UINT a, Enum t, void* caps) {
    HRESULT r;
    if (!caps || a >= adapter::count()) {
        r = kInvalidCall;
    } else if (t != adapter::kDeviceHal) {
        r = kNotAvailable;  // no reference rasterizer
    } else {
        const Caps8 table = adapter::caps(a);
        std::memcpy(caps, &table, sizeof table);
        r = kOk;
    }
    if (r == kOk) patch_caps(static_cast<Caps8*>(caps));
    if (adapter::first_query("GetDeviceCaps", a, t, static_cast<std::uint32_t>(r))) {
        char rb[16];
        hook::log_line("d3d8 query: GetDeviceCaps(adapter %u, type %u) -> %s", a, t, rname(r, rb));
        if (r == kOk) {
            char label[64];
            std::snprintf(label, sizeof label, "game sees (native, after patch_caps) %u/%u", a, t);
            adapter::log_caps(label, *static_cast<const Caps8*>(caps));
        }
    }
    return r;
}

HMONITOR Direct3D::GetAdapterMonitor(UINT a) {
    const HMONITOR m = adapter::monitor(a);
    if (adapter::first_query("GetAdapterMonitor", a))
        hook::log_line("d3d8 query: GetAdapterMonitor(adapter %u) -> %p", a, m);
    return m;
}

HRESULT Direct3D::CreateDevice(UINT adapter, Enum type, HWND focus, DWORD behavior, PresentParameters* pp,
                               IDirect3DDevice8** out) {
    if (!pp || !out) return kInvalidCall;
    HWND window = pp->device_window ? pp->device_window : focus;
    if (pp->windowed && (pp->back_buffer_width == 0 || pp->back_buffer_height == 0)) {
        int cw = 0, ch = 0;
        platform::client_size(window, cw, ch);
        if (pp->back_buffer_width == 0) pp->back_buffer_width = static_cast<UINT>(cw);
        if (pp->back_buffer_height == 0) pp->back_buffer_height = static_cast<UINT>(ch);
    }
    hook::log_line("OpenGL device: CreateDevice %ux%u format %u x%u, swap effect %u, %s, depth %s (format %u), "
                   "flags 0x%x, refresh %u, interval 0x%x, behavior 0x%x",
                   pp->back_buffer_width, pp->back_buffer_height, pp->back_buffer_format, pp->back_buffer_count,
                   pp->swap_effect, pp->windowed ? "windowed" : "full screen",
                   pp->enable_auto_depth_stencil ? "yes" : "no", pp->auto_depth_stencil_format, pp->flags,
                   pp->fullscreen_refresh_rate, pp->presentation_interval, behavior);
    auto* device = new Device(this, adapter, type, window, behavior, *pp);
    std::string error;
    if (!device->init(error)) {
        hook::log_line("OpenGL device: %s - no device", error.c_str());
        device->Release();
        return kNotAvailable;
    }
    *out = device;
    return kOk;
}

IDirect3D8* create_direct3d() {
    auto* d3d = new Direct3D();
    adapter::dump("native", d3d, nullptr, true);
    return d3d;
}

}  // namespace joan::gld
