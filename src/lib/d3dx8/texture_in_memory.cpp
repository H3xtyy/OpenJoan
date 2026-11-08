// Original library sources (re/attribution.csv original_file, under lib/): d3dx8/d3dx8tex.cpp
//   (D3DXCreateTextureFromFileInMemoryEx).
//
// N4 (STAGE3): D3DX8 texture file loading without D3DX.
//
//  * module d3dx/texture (on by default; [Hooks] Disable=d3dx/texture leaves it to D3DX):
//    D3DXCreateTextureFromFileInMemoryEx (0x00765258, statically linked D3DX8). Every texture file
//    the game loads ends here: the file wrappers D3DXCreateTextureFromFileExA (0x00765aab) /
//    FileA (0x00765d4e), the deferred loader callbacks (0x0060a21f, 0x0060a403) and 0x005bd3ec.
//
// The replacement decodes the file and computes every level with src/render/image.cpp, which
// reproduces D3DX bit for bit (loaders, D3DXCheckTextureRequirements, D3DXLoadSurfaceFromMemory,
// D3DXFilterTexture's box filter, the DXT compressor), then creates the texture through the
// device it was given (CreateTexture, LockRect / UnlockRect per level), as D3DX does. The size /
// level / format decision uses the same device queries as D3DX (GetDeviceCaps, GetDisplayMode,
// IDirect3D8::CheckDeviceFormat), so it follows whatever the device reports.
//
// Anything not reproduced exactly fails with E_FAIL (each reason logged once; joan.exe's D3DX code no longer
// exists, so there is no original to fall back to): colour keys, src_info / palette outputs, D3DPOOL_DEFAULT,
// resampling (the requested or the device-limited size differs from the file's), PNG / JPG / PPM files, source
// formats other than DXT1/3/5 / A8R8G8B8 / X8R8G8B8 / R8G8B8 / P8, target formats other than those DXTs /
// A8R8G8B8 / X8R8G8B8, mip filters other than box, and an x87 control word other than 24 / 53-bit precision with
// round-to-nearest. Cube textures (0x00765669) are lib/d3dx8/texture_api.cpp's.
#include "../../callconv.hpp"
#include "../../platform/win32_types.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <exception>
#include <mutex>
#include <string>
#include <vector>

#include "../../d3d8/d3d8_types.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../render/image.hpp"

#include "../../fp/x87.hpp"
#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
#include <xmmintrin.h>
#endif

namespace {

using joan::hook::log_line;
using joan::hook::settings;
namespace d3d8 = joan::d3d8;
namespace render = joan::render;

constexpr fn::Code kCreateTextureFromFileInMemoryEx = fn::fn_765258;

JOAN_COUNTER(c_create, "D3DXCreateTextureFromFileInMemoryEx");
JOAN_COUNTER(c_fallback, "D3DXCreateTextureFromFileInMemoryEx (D3DX fallback)");

constexpr long kOk = 0;
constexpr long kFail = static_cast<long>(0x80004005);  // E_FAIL: what every fallback answers
constexpr std::uint32_t kRtypeTexture = 3;

// --- helpers -------------------------------------------------------------------------------
/// x87 precision control in force on this thread (rounding must be to nearest). The image code
/// itself runs on SSE2 (/arch:SSE2), so the SSE control word must be the default too (round to
/// nearest, denormals kept) - a driver that sets FTZ / DAZ or another rounding mode would change
/// the results.
bool fpu_precision(render::Precision& out) {
    const joan::fp::Precision p = joan::fp::current_precision();  // the emulated x87 precision of this thread
    if (p == joan::fp::Precision::Unsupported) return false;
#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
    if ((_mm_getcsr() & 0x8040u) != 0) return false;  // SSE FTZ (bit 15) / DAZ (bit 6)
#endif
    out = p == joan::fp::Precision::Single ? render::Precision::Single : render::Precision::Double;
    return true;
}

const char* file_kind(const void* data, std::uint32_t size) {
    const auto* p = static_cast<const std::uint8_t*>(data);
    if (size >= 2 && p[0] == 'B' && p[1] == 'M') return "BMP";
    if (size >= 4 && std::memcmp(p, "DDS ", 4) == 0) return "DDS";
    if (size >= 2 && p[0] == 0xff && p[1] == 0xd8) return "JPG";
    if (size >= 4 && p[0] == 0x89 && p[1] == 'P') return "PNG";
    return "TGA/other";
}

/// Logs each distinct fallback reason once.
void note_fallback(const std::string& reason, const void* data, std::uint32_t size) {
    static std::mutex m;
    static std::vector<std::string> seen;
    {
        std::lock_guard lock(m);
        if (std::find(seen.begin(), seen.end(), reason) != seen.end()) return;
        seen.push_back(reason);
    }
    log_line("d3dx/texture: this texture fails with E_FAIL (%s; %u-byte %s file) - first occurrence of this reason",
             reason.c_str(), static_cast<unsigned>(size), data ? file_kind(data, size) : "-");
}

struct LevelDesc {
    std::uint32_t format = 0, width = 0, height = 0;
};

bool level_desc(d3d8::IDirect3DTexture8* t, std::uint32_t level, LevelDesc& out) {
    d3d8::SurfaceDesc d{};
    if (t->GetLevelDesc(level, &d) != kOk) return false;
    out = {d.format, d.width, d.height};
    return true;
}

/// Writes the planned levels into `t` (LockRect / UnlockRect per level, rows at the device's pitch).
bool fill_texture(d3d8::IDirect3DTexture8* t, const render::TexturePlan& plan, const std::vector<render::ImageLevel>& levels) {
    if (t->GetLevelCount() != plan.levels) return false;
    for (std::uint32_t l = 0; l < plan.levels; ++l) {
        LevelDesc d;
        if (!level_desc(t, l, d) || d.format != plan.format || d.width != levels[l].width || d.height != levels[l].height)
            return false;
    }
    for (std::uint32_t l = 0; l < plan.levels; ++l) {
        const render::ImageLevel& src = levels[l];
        d3d8::LockedRect lr{};
        if (t->LockRect(l, &lr, nullptr, 0) != kOk) return false;
        if (!lr.bits) {
            t->UnlockRect(l);
            return false;
        }
        const std::uint32_t rows = render::row_count(plan.format, src.height);
        for (std::uint32_t y = 0; y < rows; ++y)
            std::memcpy(static_cast<std::uint8_t*>(lr.bits) + static_cast<std::ptrdiff_t>(y) * lr.pitch,
                        src.data.data() + static_cast<std::size_t>(y) * src.pitch, src.pitch);
        t->UnlockRect(l);
    }
    return true;
}

// --- the replacement -----------------------------------------------------------------------
long JOAN_STDCALL create_texture_from_file_in_memory_ex(d3d8::IDirect3DDevice8* device, const void* data, std::uint32_t size,
                                                     std::uint32_t width, std::uint32_t height, std::uint32_t mip_levels,
                                                     std::uint32_t usage, std::uint32_t format, std::uint32_t pool,
                                                     std::uint32_t filter, std::uint32_t mip_filter, std::uint32_t color_key,
                                                     void* src_info, void* palette, d3d8::IDirect3DTexture8** out) {
    const auto fall_back = [&](const std::string& reason) -> long {
        c_fallback.hit();
        note_fallback(reason, data, size);
        return kFail;
    };
    if (!device || !data || !size || !out) return fall_back("invalid arguments");
    if (src_info || palette) return fall_back("src_info / palette requested");
    if (color_key) return fall_back("colour key");
    if (pool == 0) return fall_back("D3DPOOL_DEFAULT");

    // Malformed or huge files can make the decoders' allocations fail; a C++ exception must not
    // unwind into the game's code (MSVC 6 SEH frames), so those files fail with E_FAIL instead.
    d3d8::IDirect3D8* d3d = nullptr;
    d3d8::IDirect3DTexture8* texture = nullptr;
    try {
        std::string why;
        render::Image image;
        if (!render::decode_image(data, size, image, &why)) return fall_back(why);

        // The device answers D3DXCheckTextureRequirements uses (0x00764064).
        void* d3d_raw = nullptr;
        if (device->GetDirect3D(&d3d_raw) != kOk || !d3d_raw) return fall_back("GetDirect3D failed");
        d3d = static_cast<d3d8::IDirect3D8*>(d3d_raw);
        d3d8::Caps8 caps{};
        d3d8::DisplayMode mode{};
        const bool queried = device->GetDeviceCaps(&caps) == kOk && device->GetDisplayMode(&mode) == kOk;
        render::DeviceQuery query;
        query.texture_caps = caps.texture_caps;
        query.max_texture_width = caps.max_texture_width;
        query.max_texture_height = caps.max_texture_height;
        query.max_texture_aspect_ratio = caps.max_texture_aspect_ratio;
        const std::uint32_t check_usage = usage == 0xffffffffu ? 0 : usage;
        query.texture_format_ok = [&](std::uint32_t f) {
            return d3d->CheckDeviceFormat(caps.adapter_ordinal, caps.device_type, mode.format, check_usage, kRtypeTexture,
                                          f) == kOk;
        };
        render::LoadArgs args;
        args.width = width;
        args.height = height;
        args.mip_levels = mip_levels;
        args.usage = usage;
        args.format = format;
        args.pool = pool;
        args.filter = filter;
        args.mip_filter = mip_filter;
        args.color_key = color_key;
        render::TexturePlan plan;
        const bool planned = queried && render::plan_texture(image, args, query, plan, &why);
        d3d->Release();
        d3d = nullptr;
        if (!queried) return fall_back("GetDeviceCaps / GetDisplayMode failed");
        if (!planned) return fall_back(why);

        render::Precision precision{};
        if (!fpu_precision(precision)) return fall_back("x87 / SSE control word (precision / rounding / denormals)");
        std::vector<render::ImageLevel> levels;
        if (!render::build_texture(image, plan, precision, levels, &why)) return fall_back(why);

        if (device->CreateTexture(plan.width, plan.height, plan.levels, plan.usage, plan.format, plan.pool, &texture) != kOk ||
            !texture) {
            texture = nullptr;
            return fall_back("CreateTexture failed");
        }
        // D3DX computes after CreateTexture: follow a control word the runtime changed meanwhile.
        render::Precision now{};
        if (!fpu_precision(now)) {
            texture->Release();
            texture = nullptr;
            return fall_back("x87 / SSE control word (precision / rounding / denormals)");
        }
        if (now != precision && !render::build_texture(image, plan, now, levels, &why)) {
            texture->Release();
            texture = nullptr;
            return fall_back(why);
        }
        if (!fill_texture(texture, plan, levels)) {
            texture->Release();
            texture = nullptr;
            return fall_back("texture levels do not match the plan (level count / size / format / LockRect)");
        }
        c_create.hit();

        *out = texture;
        return kOk;
    } catch (const std::exception&) {
        if (d3d) d3d->Release();
        if (texture) texture->Release();
        c_fallback.hit();
        return kFail;
    }
}

}  // namespace

JOAN_REPLACE("d3dx/texture", "D3DXCreateTextureFromFileInMemoryEx", joan::fn::va(kCreateTextureFromFileInMemoryEx),
                    &create_texture_from_file_in_memory_ex);
