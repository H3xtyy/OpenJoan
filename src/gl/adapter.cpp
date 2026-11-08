// Native adapter answers (NativeCaps=1) and the adapter dump (see adapter.hpp).
#include "adapter.hpp"

#include "../platform/win32_types.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <mutex>
#include <set>
#include <string>
#include <utility>

#include "../registry.hpp"
#include "../platform/window.hpp"

namespace joan::gld::adapter {

namespace {

// ---------------------------------------------------------------------------------------------
// Displays (adapters). Direct3D numbers the display devices attached to the desktop, the primary
// one being adapter 0 (D3DADAPTER_DEFAULT); the list is fixed when the IDirect3D8 is created.
// Checked against the real runtime (two monitors on one GPU): 2 adapters, the primary
// first, the secondary second - the order of this list.
// ---------------------------------------------------------------------------------------------
struct Display {
    std::string name;    // DeviceName ("\\.\DISPLAY1"), empty = the primary display
    std::string gpu;     // DeviceString (the adapter's name)
    std::string pci_id;  // DeviceID (the "PCI\VEN_...&DEV_...&SUBSYS_...&REV_..." string)
};

const std::vector<Display>& displays() {
    static const std::vector<Display> list = [] {
        std::vector<Display> out;
#ifdef _WIN32
        for (DWORD i = 0;; ++i) {
            DISPLAY_DEVICEA dd = {};
            dd.cb = sizeof dd;
            if (!EnumDisplayDevicesA(nullptr, i, &dd, 0)) break;
            if (!(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) continue;
            Display d{dd.DeviceName, dd.DeviceString, dd.DeviceID};
            if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
                out.insert(out.begin(), std::move(d));
            else
                out.push_back(std::move(d));
        }
#else
        // No device names elsewhere: one adapter per display SDL knows (the primary first, as SDL lists it).
        for (int i = 0; i < platform::display_count(); ++i) out.push_back(Display{});
#endif
        if (out.empty()) out.push_back(Display{});  // no list: the primary display (nullptr device name)
        return out;
    }();
    return list;
}

/// The hex number after `key` in a PCI device id ("VEN_1002" -> 0x1002), 0 if absent.
DWORD pci_field(const std::string& id, const char* key) {
    const auto at = id.find(key);
    if (at == std::string::npos) return 0;
    return static_cast<DWORD>(std::strtoul(id.c_str() + at + std::strlen(key), nullptr, 16));
}

/// Display-mode format of a Win32 mode. The Direct3D 8 runtime of current Windows (d3d8 on d3d9)
/// lists only the 32-bit modes, as X8R8G8B8 (the real dump has no R5G6B5 mode on either display,
/// although CheckDeviceType accepts an R5G6B5 display format).
Enum mode_format(DWORD bits) { return bits == 32 ? kFmtX8R8G8B8 : kFmtUnknown; }

/// Display formats CheckDeviceType accepts (real dump: X1R5G5B5 is NOTAVAILABLE with any back buffer).
bool is_display_format(Enum f) { return f == kFmtX8R8G8B8 || f == kFmtR5G6B5; }
bool is_render_target_format(Enum f) {
    return f == kFmtX8R8G8B8 || f == kFmtA8R8G8B8 || f == kFmtR5G6B5 || f == kFmtX1R5G5B5 || f == kFmtA1R5G5B5 ||
           f == kFmtA4R4G4B4;
}
/// Depth / stencil formats, = the real dump (AMD, D3DUSAGE_DEPTHSTENCIL surface): D24S8, D16,
/// D24X8, D16_LOCKABLE ok; D24X4S4, D32, D15S1 NOTAVAILABLE. The game asks D24S8, D24X4S4, D32,
/// D16 in that order for a 32-bit mode (0x005ffc90), so D24S8 is picked.
bool is_depth_format(Enum f) { return f == kFmtD16 || f == kFmtD24X8 || f == kFmtD24S8 || f == kFmtD16Lockable; }

// ---------------------------------------------------------------------------------------------
// The caps table.
//
// = what the real Direct3D 8 runtime reports through IDirect3D8::GetDeviceCaps(HAL) on a current
// D3D9-class driver (Windows 10/11 d3d8.dll), checked word by word against the "d3d8 adapter: real
// HAL caps" dump of a current desktop GPU (both displays identical).
// Every bit the game or D3DX tests is listed with the place that tests it. direct3d.cpp's
// patch_caps then lowers what our OpenGL device does not implement (pixel shaders, vs.1.1
// constant count, lights, clip planes, streams, PREMODULATE) - the same patch as with the real
// runtime, so the game sees identical caps in both modes (verified: the "game sees (real, after
// patch_caps)" dump equals patch_caps(make_caps())).
// ---------------------------------------------------------------------------------------------
Caps8 make_caps(UINT adapter) {
    Caps8 c = {};
    c.device_type = kDeviceHal;
    c.adapter_ordinal = adapter;
    c.caps = 0x00020000;  // READ_SCANLINE
    // FULLSCREENGAMMA 0x20000 (the game sets a gamma ramp, 0x006120cb) | CANRENDERWINDOWED 0x80000 |
    // DYNAMICTEXTURES 0x20000000.
    c.caps2 = 0x200a0000;
    // ALPHA_FULLSCREEN_FLIP_OR_DISCARD 0x20 | 0x80 (D3D9 LINEAR_TO_SRGB_PRESENTATION, passed through).
    c.caps3 = 0x000000a0;
    c.presentation_intervals = 0x8000000f;  // IMMEDIATE | ONE | TWO | THREE | FOUR
    // No hardware cursor caps (current drivers report 0). The game tests COLOR (bit 0) at
    // 0x004940cc (SysEn caps copy 0x00a52488): clear -> ShowCursor(FALSE), the game draws its own
    // cursor; set -> the Windows cursor would stay visible over it.
    c.cursor_caps = 0;
    // EXECUTESYSTEMMEMORY 0x10 | EXECUTEVIDEOMEMORY 0x20 | TLVERTEXSYSTEMMEMORY 0x40 | TLVERTEXVIDEOMEMORY
    // 0x80 | TEXTUREVIDEOMEMORY 0x200 | DRAWPRIMTLVERTEX 0x400 | CANRENDERAFTERFLIP 0x800 |
    // TEXTURENONLOCALVIDMEM 0x1000 | DRAWPRIMITIVES2 0x2000 | DRAWPRIMITIVES2EX 0x8000 |
    // HWTRANSFORMANDLIGHT 0x10000 (SysEn keeps hardware vertex processing, 0x005f6f84) |
    // CANBLTSYSTONONLOCAL 0x20000 | HWRASTERIZATION 0x80000 | PUREDEVICE 0x100000 (no patches / N-patches).
    c.dev_caps = 0x001bbef0;
    // MASKZ 0x2 | CULLNONE / CW / CCW 0x70 | COLORWRITEENABLE 0x80 (shadows / water, 0x005ee586,
    // 0x005f16cd, 0x005f19e2) | TSSARGTEMP 0x400 (SysEn 0x005f7411) | BLENDOP 0x800, plus the D3D9
    // bits PERSTAGECONSTANT 0x8000 .. POSTBLENDSRGBCONVERT 0x200000 the runtime passes through.
    // No CLIPPLANESCALEDPOINTS 0x100 / CLIPTLVERTS 0x200.
    c.primitive_misc_caps = 0x003f8cf2;
    // DITHER 0x1 (0x005c0543) | ROP2 0x2 | ZTEST 0x10 | FOGVERTEX 0x80 | FOGTABLE 0x100 |
    // MIPMAPLODBIAS 0x2000 | ZBIAS 0x4000 | FOGRANGE 0x10000 (0x005c0628) | ANISOTROPY 0x20000 |
    // WFOG 0x100000 | ZFOG 0x200000 | COLORPERSPECTIVE 0x400000, plus the D3D9 bits DEPTHBIAS
    // 0x4000000 and MULTISAMPLE_TOGGLE 0x8000000 the runtime passes through.
    c.raster_caps = 0x0c736193;
    c.z_cmp_caps = 0xff;      // all eight compare functions
    c.alpha_cmp_caps = 0xff;
    c.src_blend_caps = 0x1fff;   // ZERO .. BOTHINVSRCALPHA
    c.dest_blend_caps = 0x1fff;
    // COLORGOURAUDRGB 0x8 | SPECULARGOURAUDRGB 0x200 | ALPHAGOURAUDBLEND 0x4000 | FOGGOURAUD 0x80000
    c.shade_caps = 0x00084208;
    // TextureCaps - drives the texture sizes (D3DX 0x00764064 / render/image.cpp plan_texture, and
    // the game at 0x00684d83):
    //   PERSPECTIVE 0x1 (0x005c0a2c) | ALPHA 0x4 | TEXREPEATNOTSCALEDBYSIZE 0x40 | PROJECTED 0x400 |
    //   CUBEMAP 0x800 (SysEn 0x00602660) | VOLUMEMAP 0x2000 | MIPMAP 0x4000 (D3DX makes mips only
    //   when this is set) | MIPVOLUMEMAP 0x8000 | MIPCUBEMAP 0x10000.
    //   POW2 0x2, NONPOW2CONDITIONAL 0x100 and SQUAREONLY 0x20 are CLEAR (confirmed by the dump):
    //   unconditional non-power-of-two textures, so D3DX keeps a 640x480 image 640x480 (one level,
    //   as MIPMAP needs power-of-two sizes) instead of padding it to 1024x512.
    c.texture_caps = 0x0001ec45;
    // NEAREST .. LINEARMIPLINEAR 0x3f (the legacy D3D7 bits) | MINFPOINT 0x100 | MINFLINEAR 0x200 |
    // MINFANISOTROPIC 0x400 | MIPFPOINT 0x10000 | MIPFLINEAR 0x20000 | MAGFPOINT 0x1000000 |
    // MAGFLINEAR 0x2000000 | MAGFANISOTROPIC 0x4000000
    c.texture_filter_caps = 0x0703073f;
    c.cube_texture_filter_caps = 0x03030300;
    c.volume_texture_filter_caps = 0x07030700;
    c.texture_address_caps = 0x3f;  // WRAP | MIRROR | CLAMP | BORDER | INDEPENDENTUV | MIRRORONCE
    c.volume_texture_address_caps = 0x3f;
    c.line_caps = 0x1f;  // TEXTURE | ZTEST | BLEND | ALPHACMP | FOG
    // Size limits: D3DX clamps the requested size to these, then to the aspect ratio.
    c.max_texture_width = 16384;
    c.max_texture_height = 16384;
    c.max_volume_extent = 8192;
    c.max_texture_repeat = 8192;
    c.max_texture_aspect_ratio = 8192;
    c.max_anisotropy = 16;
    c.max_vertex_w = 1e10f;
    c.guard_band_left = -32768.0f;  // not used by the game
    c.guard_band_top = -32768.0f;
    c.guard_band_right = 32768.0f;
    c.guard_band_bottom = 32768.0f;
    c.extents_adjust = 0.0f;
    c.stencil_caps = 0xff;  // KEEP ZERO REPLACE INCRSAT DECRSAT INVERT INCR DECR
    // TEXCOORDCOUNTMASK = 8 (the game needs >= 4 and compares it with MaxSimultaneousTextures,
    // 0x0055b75b / 0x005f3af7) | PSIZE 0x100000
    c.fvf_caps = 0x00100008;
    // Every D3D8 texture op 0x1 .. LERP 0x2000000 (patch_caps removes PREMODULATE 0x10000):
    // ADDSIGNED 0x80 (SysEn 0x00602610), BUMPENVMAP 0x200000 (0x0045b407, 0x00602540),
    // BUMPENVMAPLUMINANCE 0x400000, DOTPRODUCT3 0x800000 (SysEn 0x005f73ee).
    c.texture_op_caps = 0x03ffffff;
    c.max_texture_blend_stages = 8;   // >= 2 (0x0045b45d, 0x00602540)
    c.max_simultaneous_textures = 8;  // >= 3 (SysEn 0x005f73d5, 0x0055b752)
    // TEXGEN 0x1 | MATERIALSOURCE7 0x2 | DIRECTIONALLIGHTS 0x8 | POSITIONALLIGHTS 0x10 | LOCALVIEWER 0x20
    // | TWEENING 0x40
    c.vertex_processing_caps = 0x0000007b;
    c.max_active_lights = 8;
    c.max_user_clip_planes = 6;
    c.max_vertex_blend_matrices = 4;        // >= 2 enables blended skinning (0x005d082e, 0x00602301)
    c.max_vertex_blend_matrix_index = 0;    // no indexed fixed-function blending
    c.max_point_size = 256.0f;
    c.max_primitive_count = 0x00555555;
    c.max_vertex_index = 0x00ffffff;
    c.max_streams = 16;
    c.max_stream_stride = 508;
    c.vertex_shader_version = 0xfffe0101;   // vs.1.1, the highest Direct3D 8 knows (SysEn 0x005f7386)
    c.max_vertex_shader_const = 256;
    c.pixel_shader_version = 0xffff0104;    // ps.1.4, the highest Direct3D 8 knows (0x0045b313, 0x005e5a74)
    c.max_pixel_shader_value = 3.4028235e38f;  // FLT_MAX
    return c;
}

std::string hex_word(DWORD v) {
    char b[16];
    std::snprintf(b, sizeof b, "0x%08x", static_cast<unsigned>(v));
    return b;
}

// ---------------------------------------------------------------------------------------------
// Query log
// ---------------------------------------------------------------------------------------------
std::mutex g_query_mutex;
std::set<std::array<std::uint32_t, 7>> g_queries;
std::atomic<int> g_dumping{0};

std::uint32_t hash_name(const char* s) {
    std::uint32_t h = 2166136261u;
    for (; *s; ++s) h = (h ^ static_cast<unsigned char>(*s)) * 16777619u;
    return h;
}

struct DumpScope {
    DumpScope() { ++g_dumping; }
    ~DumpScope() { --g_dumping; }
};

/// Appends items to a line and writes it out when it gets long.
class LineWriter {
   public:
    explicit LineWriter(std::string prefix) : prefix_(std::move(prefix)) {}
    ~LineWriter() { flush(); }
    void add(const std::string& item) {
        if (!line_.empty() && line_.size() + item.size() > 180) flush();
        line_ += line_.empty() ? item : ", " + item;
    }
    void flush() {
        if (!line_.empty()) hook::log_line("%s%s", prefix_.c_str(), line_.c_str());
        line_.clear();
    }

   private:
    std::string prefix_, line_;
};

}  // namespace

// ---------------------------------------------------------------------------------------------
// Native answers
// ---------------------------------------------------------------------------------------------
UINT count() { return static_cast<UINT>(displays().size()); }

bool current_mode(UINT adapter, DisplayMode& out) {
    if (adapter >= count()) return false;
    platform::ScreenMode sm;
    if (!platform::current_display_mode(static_cast<int>(adapter), sm)) return false;
    out.width = static_cast<UINT>(sm.width);
    out.height = static_cast<UINT>(sm.height);
    out.refresh_rate = sm.refresh > 1 ? static_cast<UINT>(sm.refresh) : 0;  // 0 / 1 = hardware default
    out.format = mode_format(static_cast<DWORD>(sm.bits));
    if (out.format == kFmtUnknown) out.format = kFmtX8R8G8B8;
    return true;
}

std::vector<DisplayMode> modes(UINT adapter) {
    // The real list (both displays of the dump) is Windows' 32-bit modes from 640x400 up (720x400 is listed, nothing
    // smaller), each once, ordered by width then height. The order matters: the game takes the first mode of the
    // requested size unless one has its preferred refresh rate (0x005f6d73 .. 0x005f6ed5). The modes come from SDL
    // (platform::display_modes); the refresh rates of one size are listed lowest first, as the driver mostly does
    // (e.g. 640x480 @60 @59 @67 @72 @75 @119 @120 @144 @165 in the dump).
    std::vector<DisplayMode> list;
    if (adapter >= count()) return list;
    for (const platform::ScreenMode& sm : platform::display_modes(static_cast<int>(adapter))) {
        const Enum format = mode_format(static_cast<DWORD>(sm.bits));
        if (format == kFmtUnknown || sm.width < 640 || sm.height < 400) continue;
        const DisplayMode m = {static_cast<UINT>(sm.width), static_cast<UINT>(sm.height),
                               sm.refresh > 1 ? static_cast<UINT>(sm.refresh) : 0, format};
        const bool seen = std::any_of(list.begin(), list.end(), [&](const DisplayMode& o) {
            return o.width == m.width && o.height == m.height && o.refresh_rate == m.refresh_rate && o.format == m.format;
        });
        if (!seen) list.push_back(m);
    }
    std::stable_sort(list.begin(), list.end(), [](const DisplayMode& a, const DisplayMode& b) {
        if (a.width != b.width) return a.width < b.width;
        if (a.height != b.height) return a.height < b.height;
        return a.refresh_rate < b.refresh_rate;
    });
    return list;
}

d3d8::HMONITOR monitor(UINT adapter) {
    if (adapter >= count()) return nullptr;
    [[maybe_unused]] POINT at = {0, 0};
    int x = 0, y = 0;
    if (platform::display_origin(static_cast<int>(adapter), x, y)) {
        at.x = x;
        at.y = y;
    }
#ifdef _WIN32
    return static_cast<d3d8::HMONITOR>(MonitorFromPoint(at, MONITOR_DEFAULTTOPRIMARY));
#else
    return reinterpret_cast<d3d8::HMONITOR>(static_cast<std::uintptr_t>(adapter) + 1);  // one handle per display
#endif
}

void identifier(UINT adapter, Identifier8& out, DWORD flags) {
    std::memset(&out, 0, sizeof out);
    // Description and PCI ids as the real runtime reports them (the display device's DeviceString
    // / DeviceID). The game looks for "geforce3" in Description (0x0045b2b1 .. 0x0045b2e1),
    // which no current GPU name contains. Driver name / version and the GUID stay ours: the real
    // ones come from the driver's registry entry and the
    // game does not use them.
    const Display& d = displays()[adapter < count() ? adapter : 0];
    std::snprintf(out.driver, sizeof out.driver, "joan_re_opengl");
    if (!d.gpu.empty())
        std::snprintf(out.description, sizeof out.description, "%s", d.gpu.c_str());
    else
        std::snprintf(out.description, sizeof out.description, "joan_re OpenGL renderer (display %u)", adapter);
    out.vendor_id = pci_field(d.pci_id, "VEN_");
    out.device_id = pci_field(d.pci_id, "DEV_");
    out.sub_sys_id = pci_field(d.pci_id, "SUBSYS_");
    out.revision = pci_field(d.pci_id, "REV_");
    out.driver_version_high = 0x00010000;  // 1.0.0.0
    out.device_identifier = {0x6a6f616e, 0x5f72, 0x6500, {0x6f, 0x70, 0x65, 0x6e, 0x67, 0x6c, 0x00, 0x01}};
    out.whql_level = (flags & kEnumNoWhqlLevel) ? 0 : 1;  // real: 0 with NO_WHQL_LEVEL, 1 without
}

Caps8 caps(UINT adapter) { return make_caps(adapter); }

HRESULT check_device_type(UINT adapter, Enum type, Enum display_format, Enum back_format, BOOL /*windowed*/) {
    if (adapter >= count()) return kInvalidCall;
    if (type != kDeviceHal) return kNotAvailable;  // no reference rasterizer / software device
    // = the real dump, the same full screen and windowed (the windowed R5G6B5/R5G6B5 is ok on a
    // 32-bit desktop too): X8R8G8B8 with X8R8G8B8 / A8R8G8B8, R5G6B5 with R5G6B5, nothing else
    // (no X1R5G5B5 display format, no format conversion).
    switch (display_format) {
        case kFmtX8R8G8B8: return back_format == kFmtX8R8G8B8 || back_format == kFmtA8R8G8B8 ? kOk : kNotAvailable;
        case kFmtR5G6B5: return back_format == kFmtR5G6B5 ? kOk : kNotAvailable;
        default: return kNotAvailable;
    }
}

HRESULT check_device_format(UINT adapter, Enum type, Enum adapter_format, DWORD usage, Enum rtype, Enum format) {
    if (adapter >= count()) return kInvalidCall;
    if (type != kDeviceHal || !is_display_format(adapter_format)) return kNotAvailable;
    if (usage & kUsageDepthStencil) return rtype == kRtSurface && is_depth_format(format) ? kOk : kNotAvailable;
    if (rtype == kRtVertexBuffer) return format == kFmtVertexData ? kOk : kNotAvailable;
    if (rtype == kRtIndexBuffer) return format == kFmtIndex16 || format == kFmtIndex32 ? kOk : kNotAvailable;
    return kNotAvailable;
}

HRESULT check_depth_stencil_match(UINT adapter, Enum type, Enum adapter_format, Enum rt_format, Enum depth_format) {
    if (adapter >= count()) return kInvalidCall;
    if (type != kDeviceHal || !is_display_format(adapter_format)) return kNotAvailable;
    // Real dump: every supported depth format with every target (also D16 with a 32-bit target);
    // a depth format the device does not have is INVALIDCALL (not NOTAVAILABLE). A target format
    // that is not a render target format is taken to be the same (not in the dump).
    return is_render_target_format(rt_format) && is_depth_format(depth_format) ? kOk : kInvalidCall;
}

// ---------------------------------------------------------------------------------------------
// Logging
// ---------------------------------------------------------------------------------------------
const char* format_name(Enum f) {
    switch (f) {
        case 0: return "UNKNOWN";
        case 20: return "R8G8B8";
        case 21: return "A8R8G8B8";
        case 22: return "X8R8G8B8";
        case 23: return "R5G6B5";
        case 24: return "X1R5G5B5";
        case 25: return "A1R5G5B5";
        case 26: return "A4R4G4B4";
        case 27: return "R3G3B2";
        case 28: return "A8";
        case 29: return "A8R3G3B2";
        case 30: return "X4R4G4B4";
        case 40: return "A8P8";
        case 41: return "P8";
        case 50: return "L8";
        case 51: return "A8L8";
        case 52: return "A4L4";
        case 60: return "V8U8";
        case 61: return "L6V5U5";
        case 62: return "X8L8V8U8";
        case 63: return "Q8W8V8U8";
        case 64: return "V16U16";
        case 65: return "W11V11U10";
        case 70: return "D16_LOCKABLE";
        case 71: return "D32";
        case 73: return "D15S1";
        case 75: return "D24S8";
        case 77: return "D24X8";
        case 79: return "D24X4S4";
        case 80: return "D16";
        case 100: return "VERTEXDATA";
        case 101: return "INDEX16";
        case 102: return "INDEX32";
        case 0x59565955: return "UYVY";
        case 0x32595559: return "YUY2";
        case kFmtDXT1: return "DXT1";
        case kFmtDXT2: return "DXT2";
        case kFmtDXT3: return "DXT3";
        case kFmtDXT4: return "DXT4";
        case kFmtDXT5: return "DXT5";
        default: {
            static thread_local char buf[16];
            std::snprintf(buf, sizeof buf, "fmt%u", static_cast<unsigned>(f));
            return buf;
        }
    }
}

const char* result_name(HRESULT r, char (&buf)[16]) {
    if (r == kOk) return "ok";
    if (r == kNotAvailable) return "NOTAVAILABLE";
    if (r == kInvalidCall) return "INVALIDCALL";
#if defined(__GNUC__) && !defined(__clang__)
// "0x%08lx" of a 32-bit HRESULT: at most 10 characters; GCC assumes a 64-bit unsigned long on x64
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
#endif
    std::snprintf(buf, sizeof buf, "0x%08lx", static_cast<unsigned long>(r));
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
    return buf;
}

namespace {
struct Field {
    const char* name;
    std::size_t offset;
    bool is_float;
};
#define JOAN_CAPS_FIELD(n) {#n, offsetof(Caps8, n), false}
#define JOAN_CAPS_FLOAT(n) {#n, offsetof(Caps8, n), true}
const Field kFields[] = {
    JOAN_CAPS_FIELD(device_type), JOAN_CAPS_FIELD(adapter_ordinal), JOAN_CAPS_FIELD(caps), JOAN_CAPS_FIELD(caps2),
    JOAN_CAPS_FIELD(caps3), JOAN_CAPS_FIELD(presentation_intervals), JOAN_CAPS_FIELD(cursor_caps),
    JOAN_CAPS_FIELD(dev_caps), JOAN_CAPS_FIELD(primitive_misc_caps), JOAN_CAPS_FIELD(raster_caps),
    JOAN_CAPS_FIELD(z_cmp_caps), JOAN_CAPS_FIELD(src_blend_caps), JOAN_CAPS_FIELD(dest_blend_caps),
    JOAN_CAPS_FIELD(alpha_cmp_caps), JOAN_CAPS_FIELD(shade_caps), JOAN_CAPS_FIELD(texture_caps),
    JOAN_CAPS_FIELD(texture_filter_caps), JOAN_CAPS_FIELD(cube_texture_filter_caps),
    JOAN_CAPS_FIELD(volume_texture_filter_caps), JOAN_CAPS_FIELD(texture_address_caps),
    JOAN_CAPS_FIELD(volume_texture_address_caps), JOAN_CAPS_FIELD(line_caps), JOAN_CAPS_FIELD(max_texture_width),
    JOAN_CAPS_FIELD(max_texture_height), JOAN_CAPS_FIELD(max_volume_extent), JOAN_CAPS_FIELD(max_texture_repeat),
    JOAN_CAPS_FIELD(max_texture_aspect_ratio), JOAN_CAPS_FIELD(max_anisotropy), JOAN_CAPS_FLOAT(max_vertex_w),
    JOAN_CAPS_FLOAT(guard_band_left), JOAN_CAPS_FLOAT(guard_band_top), JOAN_CAPS_FLOAT(guard_band_right),
    JOAN_CAPS_FLOAT(guard_band_bottom), JOAN_CAPS_FLOAT(extents_adjust), JOAN_CAPS_FIELD(stencil_caps),
    JOAN_CAPS_FIELD(fvf_caps), JOAN_CAPS_FIELD(texture_op_caps), JOAN_CAPS_FIELD(max_texture_blend_stages),
    JOAN_CAPS_FIELD(max_simultaneous_textures), JOAN_CAPS_FIELD(vertex_processing_caps),
    JOAN_CAPS_FIELD(max_active_lights), JOAN_CAPS_FIELD(max_user_clip_planes),
    JOAN_CAPS_FIELD(max_vertex_blend_matrices), JOAN_CAPS_FIELD(max_vertex_blend_matrix_index),
    JOAN_CAPS_FLOAT(max_point_size), JOAN_CAPS_FIELD(max_primitive_count), JOAN_CAPS_FIELD(max_vertex_index),
    JOAN_CAPS_FIELD(max_streams), JOAN_CAPS_FIELD(max_stream_stride), JOAN_CAPS_FIELD(vertex_shader_version),
    JOAN_CAPS_FIELD(max_vertex_shader_const), JOAN_CAPS_FIELD(pixel_shader_version),
    JOAN_CAPS_FLOAT(max_pixel_shader_value),
};
#undef JOAN_CAPS_FIELD
#undef JOAN_CAPS_FLOAT

DWORD field_word(const Caps8& c, const Field& f) {
    DWORD v;
    std::memcpy(&v, reinterpret_cast<const unsigned char*>(&c) + f.offset, 4);
    return v;
}

std::string field_text(const Caps8& c, const Field& f) {
    const DWORD v = field_word(c, f);
    char item[64];
    if (f.is_float) {
        float x;
        std::memcpy(&x, &v, 4);
        std::snprintf(item, sizeof item, "%g", static_cast<double>(x));
    } else if (v < 0x10000) {
        std::snprintf(item, sizeof item, "%u", static_cast<unsigned>(v));
    } else {
        std::snprintf(item, sizeof item, "0x%08x", static_cast<unsigned>(v));
    }
    return item;
}

/// Where the native caps table differs from the real runtime's caps of this machine.
void log_caps_diff(const char* label, UINT adapter, const Caps8& real, const Caps8& native) {
    LineWriter out(std::string("d3d8 adapter: ") + label + " caps " + std::to_string(adapter) +
                   " differ from the native table (real/native): ");
    int n = 0;
    for (const Field& f : kFields) {
        if (field_word(real, f) == field_word(native, f)) continue;
        out.add(std::string(f.name) + " " + field_text(real, f) + "/" + field_text(native, f));
        ++n;
    }
    out.flush();
    if (n == 0) hook::log_line("d3d8 adapter: %s caps %u = the native table", label, adapter);
}
}  // namespace

void log_caps(const char* label, const Caps8& c) {
    const auto* words = reinterpret_cast<const DWORD*>(&c);
    constexpr int kWords = sizeof(Caps8) / 4;
    for (int i = 0; i < kWords; i += 8) {
        std::string line;
        for (int j = i; j < std::min(i + 8, kWords); ++j) line += (j > i ? " " : "") + hex_word(words[j]);
        hook::log_line("d3d8 adapter: %s caps +0x%02x: %s", label, i * 4, line.c_str());
    }
    LineWriter out(std::string("d3d8 adapter: ") + label + " caps: ");
    for (const Field& f : kFields) out.add(std::string(f.name) + "=" + field_text(c, f));
}

void log_identifier(const char* label, UINT adapter, const Identifier8& id) {
    char driver[513] = {}, description[513] = {};
    std::memcpy(driver, id.driver, 512);
    std::memcpy(description, id.description, 512);
    const Guid& g = id.device_identifier;
    hook::log_line("d3d8 adapter: %s identifier %u: driver \"%s\" version %u.%u.%u.%u, description \"%s\", vendor 0x%04x "
                   "device 0x%04x subsys 0x%08x revision %u, guid %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x, whql 0x%x",
                   label, adapter, driver, id.driver_version_high >> 16, id.driver_version_high & 0xffff,
                   id.driver_version_low >> 16, id.driver_version_low & 0xffff, description,
                   static_cast<unsigned>(id.vendor_id), static_cast<unsigned>(id.device_id),
                   static_cast<unsigned>(id.sub_sys_id), static_cast<unsigned>(id.revision), g.d1, g.d2, g.d3, g.d4[0],
                   g.d4[1], g.d4[2], g.d4[3], g.d4[4], g.d4[5], g.d4[6], g.d4[7], static_cast<unsigned>(id.whql_level));
}

void log_modes(const char* label, UINT adapter, const std::vector<DisplayMode>& list) {
    hook::log_line("d3d8 adapter: %s adapter %u has %u modes", label, adapter, static_cast<unsigned>(list.size()));
    char prefix[96];
    std::snprintf(prefix, sizeof prefix, "d3d8 adapter: %s modes %u: ", label, adapter);
    LineWriter out(prefix);
    for (const DisplayMode& m : list) {
        char item[64];
        std::snprintf(item, sizeof item, "%ux%u@%u %s", m.width, m.height, m.refresh_rate, format_name(m.format));
        out.add(item);
    }
}

bool first_query(const char* method, std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d,
                 std::uint32_t e, std::uint32_t f) {
    if (g_dumping.load() != 0) return false;
    std::lock_guard lock(g_query_mutex);
    return g_queries.insert({hash_name(method), a, b, c, d, e, f}).second;
}

void dump(const char* label, IDirect3D8* d3d, IDirect3D8* ours, bool native_table) {
    static std::mutex m;
    static std::set<std::string> done;
    {
        std::lock_guard lock(m);
        if (!done.insert(label).second) return;
    }
    DumpScope scope;
    char rb[16], ob[16];
    const UINT adapters = d3d->GetAdapterCount();
    hook::log_line("d3d8 adapter: ---- %s answers (one-time dump): %u adapter(s)", label, adapters);
    if (!native_table && adapters != count())
        hook::log_line("d3d8 adapter: %s has %u adapter(s), the native list %u", label, adapters, count());

    // Formats D3DX's texture loader asks for (its table at 0x00a2dcb0, render/image.cpp kFormatTable).
    static const Enum kTextureFormats[] = {20, 21, 22, 23, 24, 40, 41, 25, 26, 27, 28, 29, 30, 50, 51, 52, 60,
                                           61, 62, 63, 64, 65, 0x59565955, 0x32595559, kFmtDXT1, kFmtDXT2,
                                           kFmtDXT3, kFmtDXT4, kFmtDXT5};
    // Back buffer candidates of SysEn 0x005ff9d0 and depth candidates of 0x005ffc90.
    static const Enum kTargetFormats[] = {kFmtX8R8G8B8, kFmtA8R8G8B8, kFmtR5G6B5, kFmtX1R5G5B5, kFmtA1R5G5B5,
                                          kFmtA4R4G4B4, kFmtR8G8B8};
    static const Enum kDepthFormats[] = {kFmtD24S8, kFmtD24X4S4, kFmtD32, kFmtD16, kFmtD24X8, kFmtD15S1,
                                         kFmtD16Lockable};
    static const Enum kDisplayFormats[] = {kFmtX8R8G8B8, kFmtR5G6B5, kFmtX1R5G5B5};

    for (UINT a = 0; a < std::min<UINT>(adapters, 4); ++a) {
        Identifier8 id = {};
        if (d3d->GetAdapterIdentifier(a, kEnumNoWhqlLevel, &id) == kOk) log_identifier(label, a, id);
        hook::log_line("d3d8 adapter: %s adapter %u monitor %p", label, a, d3d->GetAdapterMonitor(a));
        DisplayMode now = {};
        const HRESULT mr = d3d->GetAdapterDisplayMode(a, &now);
        hook::log_line("d3d8 adapter: %s current mode %u: %ux%u@%u %s (%s)", label, a, now.width, now.height,
                       now.refresh_rate, format_name(now.format), result_name(mr, rb));
        const Enum af = mr == kOk && now.format ? now.format : kFmtX8R8G8B8;
        std::vector<DisplayMode> list;
        const UINT n = d3d->GetAdapterModeCount(a);
        for (UINT i = 0; i < n; ++i) {
            DisplayMode dm = {};
            if (d3d->EnumAdapterModes(a, i, &dm) == kOk) list.push_back(dm);
        }
        log_modes(label, a, list);
        if (!native_table) {  // the native mode list (NativeCaps=1) next to the real one
            const std::vector<DisplayMode> mine = modes(a);
            const bool same = mine.size() == list.size() &&
                              std::equal(mine.begin(), mine.end(), list.begin(), [](const DisplayMode& x, const DisplayMode& y) {
                                  return x.width == y.width && x.height == y.height && x.refresh_rate == y.refresh_rate &&
                                         x.format == y.format;
                              });
            if (same)
                hook::log_line("d3d8 adapter: %s modes %u = the native list", label, a);
            else
                log_modes("native (differs)", a, mine);
        }

        Caps8 c = {};
        char caps_label[96];
        if (native_table) {
            c = caps(a);
            std::snprintf(caps_label, sizeof caps_label, "%s HAL %u (table before patch_caps)", label, a);
            log_caps(caps_label, c);
        } else {
            const HRESULT cr = d3d->GetDeviceCaps(a, kDeviceHal, &c);
            std::snprintf(caps_label, sizeof caps_label, "%s HAL %u", label, a);
            if (cr == kOk) {
                log_caps(caps_label, c);
                log_caps_diff(label, a, c, caps(a));
            } else
                hook::log_line("d3d8 adapter: %s GetDeviceCaps(%u, HAL) -> %s", label, a, result_name(cr, rb));
        }
        Caps8 ref = {};
        const HRESULT rr = d3d->GetDeviceCaps(a, 2, &ref);  // D3DDEVTYPE_REF
        hook::log_line("d3d8 adapter: %s GetDeviceCaps(%u, REF) -> %s", label, a, result_name(rr, rb));

        // CheckDeviceFormat, adapter format = the current mode's.
        // In the real dump: the native answer (NativeCaps=1) where it differs, for the checks
        // adapter.cpp answers itself (texture / surface formats are ours in both modes, "ours").
        const auto native_note = [&](HRESULT real_answer, HRESULT native_answer) {
            if (native_table || real_answer == native_answer) return std::string();
            return std::string(" (native ") + result_name(native_answer, ob) + ")";
        };
        const auto formats = [&](const char* what, DWORD usage, Enum rtype, const Enum* list_, std::size_t count_) {
            char prefix[160];
            std::snprintf(prefix, sizeof prefix, "d3d8 adapter: %s %s %u (adapter format %s, usage 0x%x, rtype %u): ", label,
                          what, a, format_name(af), static_cast<unsigned>(usage), static_cast<unsigned>(rtype));
            LineWriter out(prefix);
            for (std::size_t i = 0; i < count_; ++i) {
                const HRESULT r = d3d->CheckDeviceFormat(a, kDeviceHal, af, usage, rtype, list_[i]);
                std::string item = std::string(format_name(list_[i])) + " " + result_name(r, rb);
                if (ours) {
                    const HRESULT o = ours->CheckDeviceFormat(a, kDeviceHal, af, usage, rtype, list_[i]);
                    if (o != r) item += std::string(" (ours ") + result_name(o, ob) + ")";
                }
                if (usage & kUsageDepthStencil)
                    item += native_note(r, check_device_format(a, kDeviceHal, af, usage, rtype, list_[i]));
                out.add(item);
            }
        };
        formats("texture formats", 0, kRtTexture, kTextureFormats, std::size(kTextureFormats));
        formats("render target formats", kUsageRenderTarget, kRtSurface, kTargetFormats, std::size(kTargetFormats));
        formats("render target texture formats", kUsageRenderTarget, kRtTexture, kTargetFormats, std::size(kTargetFormats));
        formats("depth formats", kUsageDepthStencil, kRtSurface, kDepthFormats, std::size(kDepthFormats));
        formats("cube texture formats", 0, kRtCubeTexture, kTargetFormats, 2);

        {
            char prefix[96];
            std::snprintf(prefix, sizeof prefix, "d3d8 adapter: %s CheckDeviceType %u (display/back/windowed): ", label, a);
            LineWriter out(prefix);
            for (Enum df : kDisplayFormats)
                for (std::size_t i = 0; i < 5; ++i)
                    for (BOOL w = 0; w <= 1; ++w) {
                        const HRESULT r = d3d->CheckDeviceType(a, kDeviceHal, df, kTargetFormats[i], w);
                        out.add(std::string(format_name(df)) + "/" + format_name(kTargetFormats[i]) + (w ? "/w " : "/f ") +
                                result_name(r, rb) + native_note(r, check_device_type(a, kDeviceHal, df, kTargetFormats[i], w)));
                    }
        }
        {
            char prefix[96];
            std::snprintf(prefix, sizeof prefix, "d3d8 adapter: %s CheckDepthStencilMatch %u (target/depth): ", label, a);
            LineWriter out(prefix);
            for (std::size_t i = 0; i < 3; ++i)
                for (Enum df : kDepthFormats) {
                    const HRESULT r = d3d->CheckDepthStencilMatch(a, kDeviceHal, af, kTargetFormats[i], df);
                    out.add(std::string(format_name(kTargetFormats[i])) + "/" + format_name(df) + " " + result_name(r, rb) +
                            native_note(r, check_depth_stencil_match(a, kDeviceHal, af, kTargetFormats[i], df)));
                }
        }
        {
            char prefix[96];
            std::snprintf(prefix, sizeof prefix, "d3d8 adapter: %s CheckDeviceMultiSampleType %u: ", label, a);
            LineWriter out(prefix);
            for (Enum f : {kFmtX8R8G8B8, kFmtD24S8})
                for (BOOL w = 0; w <= 1; ++w) {
                    std::string ok;
                    for (Enum ms = 2; ms <= 16; ++ms)
                        if (d3d->CheckDeviceMultiSampleType(a, kDeviceHal, f, w, ms) == kOk) ok += (ok.empty() ? "" : " ") + std::to_string(ms);
                    out.add(std::string(format_name(f)) + (w ? " windowed: " : " full screen: ") + (ok.empty() ? "none" : ok));
                }
        }
    }
    hook::log_line("d3d8 adapter: ---- end of %s answers", label);
}

}  // namespace joan::gld::adapter
