// Original library sources (re/attribution.csv original_file, under lib/): d3dx8/d3dx8tex.cpp.
//
// Batch 114 / list 16: the D3DX8 texture API block (statically linked d3dx8tex code in joan.exe), module d3dx/api.
//
// Surface replaced here (all __stdcall, no incremental-link thunks: entry patches, re/check_lib_entries.py safe):
//   0x00764892 D3DXCheckTextureRequirements(dev, pW, pH, pMips, Usage, pFmt, Pool)                    ret 0x1c
//   0x007648ba D3DXCreateTexture(dev, W, H, Mips, Usage, Fmt, Pool, ppTex)                             ret 0x20
//   0x00764920 D3DXCheckCubeTextureRequirements(dev, pSize, pMips, Usage, pFmt, Pool)                 ret 0x18
//   0x00764948 D3DXCreateCubeTexture(dev, Size, Mips, Usage, Fmt, Pool, ppCube)                        ret 0x1c
//   0x00765669 D3DXCreateCubeTextureFromFileInMemoryEx(dev, data, size, Size, Mips, Usage, Fmt, Pool,
//              Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppCube)                                ret 0x38
//   0x00765c3d D3DXCreateTextureFromFileInMemory(dev, data, size, ppTex)     -> 0x00765258             ret 0x10
//   0x00765d26 D3DXCreateCubeTextureFromFileInMemory(dev, data, size, ppCube) -> 0x00765669            ret 0x10
//   0x00765d4e D3DXCreateTextureFromFileA(dev, file, ppTex)                  -> 0x00765aab             ret 0xc
//   0x00765dea D3DXCreateCubeTextureFromFileA(dev, file, ppCube)             -> 0x00765c66             ret 0xc
// The four requirement / create entries share D3DX's internal 0x00764064 (cdecl, device in eax: pW, pH, pDepth,
// pMips, Usage, pFmt, Pool, type 0 texture / 1 cube / 2 volume), ported here exactly (req_compute). 0x00764064
// itself is replaced by lib/imports/import_stubs.cpp (module fix/uninit: exact where D3DX's result is defined, deterministic
// where it is not); after this module its only remaining callers are the volume variant 0x007649a7
// (D3DXCheckVolumeTextureRequirements <- D3DXCreateVolumeTexture 0x007649d0, neither referenced by anything) and
// the fallbacks below. Callers of the block: game code (0x0045b34a.., 0x00602592.., 0x00606ae7, 0x00609b30..,
// 0x0060a30d, 0x005da85d..) and our C++ (game_b108_*, game_b111_*, en3d_*, game_b102_10/11, lib/d3dx8/texture_from_file.cpp).
//
// Not here, already replaced elsewhere: 0x00765258 D3DXCreateTextureFromFileInMemoryEx (lib/d3dx8/texture_in_memory.cpp,
// d3dx/texture), 0x00765aab / 0x00765c66 *FromFileExA (lib/d3dx8/texture_from_file.cpp, d3dx/b113_01; they call 0x00765258 /
// 0x00765669 at their addresses, so they reach this module's cube loader). The forwarders here call 0x00765258,
// 0x00765669, 0x00765aab, 0x00765c66 at their addresses too, so whichever implementation is installed runs.
// D3DXLoadSurfaceFrom* (0x00764446 / 0x0076467c / 0x00764a3d / 0x00764af3) and D3DXFilter(Cube)Texture
// (0x00764d45 / 0x00764e76) have no caller outside D3DX (0x007980f9.. / 0x007987ae are in the dead D3DXSprite /
// D3DXFont code), so they are not entry points of the block.
//
// Cube loader (0x00765669): D3DX decodes the file (CD3DXImage::Load 0x0078856c), requires exactly 6 faces
// (E_FAIL otherwise, after filling pSrcInfo), resolves the defaults (size = max(width, height), format, filters
// | D3DX_FILTER_MIRROR 0x70000), builds the 256-entry palette, calls D3DXCheckCubeTextureRequirements, creates
// the cube (and a D3DPOOL_SYSTEMMEM twin for D3DPOOL_DEFAULT, copied with UpdateTexture), loads each face's file
// levels with D3DXLoadSurfaceFromMemory and either resamples the missing levels from the last file level (mip
// filter not point / box and filter not none) or runs D3DXFilterCubeTexture (0x00764e76) from it. The pixels are
// computed with src/render/image.* (the D3DX-exact DDS decoder, same-size loads and box filter of d3dx/texture),
// face by face (each face is an independent 2D level chain for D3DXLoadSurfaceFromMemory / the box filter).
//
// Fallbacks (cube loader): joan.exe's D3DX code no longer exists, so the paths that would need the D3DX image
// library (blitter 0x00786b38, CD3DXImage 0x0078856c, codecs 0x007928e7, libpng / libjpeg) fail with E_FAIL, as
// in lib/d3dx8/texture_in_memory.cpp (none runs in a normal game): files render/image.cpp does not decode (PNG / JPG / PPM, malformed or unusual BMP / TGA / DDS),
// colour key != 0, D3DFMT_P8 requested (D3DX then reads a NULL source palette and crashes), invalid filter values,
// resampling (requested / device-limited size differs from the file's, non-square faces, missing levels with a
// resampling mip filter), mip filters other than box, source / target formats image.cpp does not convert,
// x87 / SSE control words other than 24 / 53-bit round-to-nearest, LockRect / surface failures, a created cube
// whose levels differ from the plan. The game passes colour key 0, NULL info / palette and managed pool at every
// call site; its cube files are DDS cube maps. pSrcInfo / pPalette are written (commit_outputs) only on paths
// that do not fall back, so a fallback returns with nothing written; the fallbacks after
// D3DXCheckCubeTextureRequirements / CreateCubeTexture release what was created first
// (as lib/d3dx8/texture_in_memory.cpp does).
// 0x00764064 (req_compute) where D3DX's result is not defined gives the deterministic result of
// lib/imports/import_stubs.cpp's fix/uninit replacement of 0x00764064 (port P1: no fallback into joan.exe code any
// more; logged once): the format search compares bits-per-pixel against a not yet written local when the first
// convertible candidate is incompatible (e.g. a bump-map format requested and not supported) -> that local counts as
// 0 (fix/uninit a); power-of-two rounding of a size above 2^31 (D3DX hangs, only with fake caps) ->
// D3DERR_NOTAVAILABLE (b); GetDirect3D / GetDeviceCaps / GetDisplayMode failing (D3DX ignores those HRESULTs and
// reads uninitialised structures) -> D3DERR_NOTAVAILABLE after all three calls (c).
//
// Tested on Linux against joan.exe's own D3DX code (scratchpad w114_16: harness h/d3dx_run modes req / cube +
// core test b114_16_test.cpp which #includes this file with JOAN_B114_16_CORE_TEST).
//
// Modules: d3dx/api.
#include "../../callconv.hpp"
#ifndef JOAN_B114_16_CORE_TEST
#include "../../platform/win32_types.hpp"
#endif

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <exception>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include "../../d3d8/d3d8_types.hpp"
#include "../../fn_names.hpp"
#include "../../render/image.hpp"
#ifndef JOAN_B114_16_CORE_TEST
#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

#include "../../fp/x87.hpp"
#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
#include <xmmintrin.h>
#endif
#endif

namespace {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
namespace d3d8 = joan::d3d8;
namespace render = joan::render;

constexpr long kOk = 0;
constexpr long kInvalidCall = static_cast<std::int32_t>(0x8876086cu);   // D3DERR_INVALIDCALL
constexpr long kNotAvailable = static_cast<std::int32_t>(0x8876086au);  // D3DERR_NOTAVAILABLE
constexpr long kFail = static_cast<std::int32_t>(0x80004005u);          // E_FAIL
constexpr u32 kDefault = 0xffffffffu;                          // D3DX_DEFAULT

// ---------------------------------------------------------------------------------------------------------------
// D3DX format table 0x00a2dcb0..0x00a2e138 (end pointer 0x00a2e160, never written): format, type (0 RGB, 1 palette,
// 2 luminance, 3 bump), bits per pixel, five channel widths, "convertible", "listed" (+0x24, 1 everywhere).
struct FormatEntry {
    u32 format, type, bpp, bits[5], convertible, listed;
};
constexpr FormatEntry kFormats[] = {
    {0x14, 0, 24, {0, 0, 8, 8, 8}, 0, 1},       {0x15, 0, 32, {0, 8, 8, 8, 8}, 1, 1},
    {0x16, 0, 32, {0, 0, 8, 8, 8}, 1, 1},       {0x17, 0, 16, {0, 0, 5, 6, 5}, 1, 1},
    {0x18, 0, 16, {0, 0, 5, 5, 5}, 1, 1},       {0x28, 1, 16, {0, 8, 8, 8, 8}, 1, 1},
    {0x29, 1, 8, {0, 8, 8, 8, 8}, 1, 1},        {0x19, 0, 16, {0, 1, 5, 5, 5}, 1, 1},
    {0x1a, 0, 16, {0, 4, 4, 4, 4}, 1, 1},       {0x1b, 0, 8, {0, 0, 3, 3, 2}, 1, 1},
    {0x1c, 0, 8, {0, 8, 0, 0, 0}, 1, 1},        {0x1d, 0, 16, {0, 8, 3, 3, 2}, 1, 1},
    {0x1e, 0, 16, {0, 0, 4, 4, 4}, 1, 1},       {0x32, 2, 8, {0, 0, 8, 8, 8}, 1, 1},
    {0x33, 2, 16, {0, 8, 8, 8, 8}, 1, 1},       {0x34, 2, 8, {0, 4, 4, 4, 4}, 1, 1},
    {0x3c, 3, 16, {0, 0, 0, 8, 8}, 1, 1},       {0x3d, 3, 16, {6, 0, 0, 5, 5}, 1, 1},
    {0x3e, 3, 32, {8, 0, 0, 8, 8}, 1, 1},       {0x3f, 3, 32, {0, 8, 8, 8, 8}, 1, 1},
    {0x40, 3, 32, {0, 0, 0, 16, 16}, 1, 1},     {0x41, 3, 32, {0, 0, 11, 11, 10}, 1, 1},
    {0x59565955, 0, 16, {0, 0, 8, 8, 8}, 0, 1}, {0x32595559, 0, 16, {0, 0, 8, 8, 8}, 0, 1},
    {0x31545844, 0, 16, {0, 1, 5, 6, 5}, 0, 1}, {0x32545844, 0, 16, {0, 4, 5, 6, 5}, 0, 1},
    {0x33545844, 0, 16, {0, 4, 5, 6, 5}, 0, 1}, {0x34545844, 0, 16, {0, 3, 5, 6, 5}, 0, 1},
    {0x35545844, 0, 16, {0, 3, 5, 6, 5}, 0, 1},
};
constexpr std::size_t kFormatCount = sizeof kFormats / sizeof kFormats[0];
static_assert(kFormatCount == 29, "0x00a2dcb0 holds 29 entries");
constexpr FormatEntry kNoFormat = {0, 0, 32, {0, 8, 8, 8, 8}, 0, 0};  // 0x00a2e138, the terminator

// 0x00763fc0: table entry of a format, the terminator when it is not listed.
const FormatEntry& format_entry(u32 format) {
    for (const FormatEntry& e : kFormats)
        if (e.format == format) return e;
    return kNoFormat;
}

// 0x00763fe4 (ecx = entry, eax = wanted): distance of `e` from `want`, ~0 = not a conversion target.
u32 format_distance(const FormatEntry& e, const FormatEntry& want) {
    if (!e.convertible) return ~0u;
    if (e.type != want.type) {
        if (e.type == 0) {
            if (want.type == 3) return ~0u;
        } else if (e.type == 1 || e.type == 2 || e.type == 3) {
            return ~0u;
        }
    }
    u32 score = 0;
    for (int i = 0; i < 5; ++i) {
        const u32 req = want.bits[i], have = e.bits[i];
        if (req < have)
            score += req == 0 ? 0x100u : have - req;
        else if (req > have)
            score += have == 0 ? 0x1000000u : (req - have) << 16;
    }
    return score;
}

bool is_dxt(u32 f) { return f == 0x31545844 || f == 0x32545844 || f == 0x33545844 || f == 0x34545844 || f == 0x35545844; }

// ---------------------------------------------------------------------------------------------------------------
// 0x00764064, D3DX's texture requirement core.

/// The D3DCAPS8 fields 0x00764064 reads.
struct ReqCaps {
    u32 texture_caps = 0;  // +0x3c
    u32 max_width = 0, max_height = 0, max_volume = 0, max_aspect = 0;  // +0x58, +0x5c, +0x60, +0x68
};

/// Argument checks before any device call. Normalises Usage (D3DX_DEFAULT -> 0) as D3DX does.
long req_validate(bool have_device, u32& usage, u32 pool, u32 type) {
    if (!have_device) return kInvalidCall;
    if (usage == kDefault) usage = 0;
    if ((usage & 3) != usage) return kInvalidCall;
    if (type == 2 && usage != 0) return kInvalidCall;
    if (pool != 0 && pool != 1 && pool != 2) return kInvalidCall;
    return kOk;
}

/// The rest of 0x00764064, after GetDirect3D / GetDeviceCaps / GetDisplayMode. `check(usage, rtype, format)` is
/// IDirect3D8::CheckDeviceFormat (adapter / device type / display format bound by the caller), called in D3DX's
/// table order. Outputs are written only on success. `undefined` = D3DX's own result is not defined (see the file
/// header); the result is then fix/uninit's choice.
template <class Check>
long req_compute(const ReqCaps& caps, Check&& check, u32* pw, u32* ph, u32* pd, u32* pmips, u32 usage, u32* pfmt,
                 u32 type, bool& undefined) {
    undefined = false;
    u32 w = pw ? *pw : kDefault;
    u32 h = ph ? *ph : kDefault;
    u32 d = pd ? *pd : kDefault;
    bool zero_checks = true;
    if (w == kDefault) {
        if (h == kDefault) {
            w = h = 256;
            zero_checks = false;
        } else {
            w = h;
        }
    } else if (h == kDefault) {
        h = w;
    }
    if (zero_checks) {
        if (w == 0) w = 1;
        if (h == 0) h = 1;
    }
    if (type == 1) {
        if (w > h) h = w;
        w = h;
    }
    if (d == kDefault || d == 0) d = 1;
    if (type == 2) {
        if (d > caps.max_volume) d = caps.max_volume;
        if (w > caps.max_volume) w = caps.max_volume;
        if (h > caps.max_volume) h = caps.max_volume;
    } else {
        if (w > caps.max_width) w = caps.max_width;
        if (h > caps.max_height) h = caps.max_height;
        if (type == 0) {
            const u32 aspect = (caps.texture_caps & 0x20) ? 1u : caps.max_aspect;  // D3DPTEXTURECAPS_SQUAREONLY
            if (aspect) {
                if (w > aspect * h) w = aspect * h;
                if (h > aspect * w) h = aspect * w;
            }
        }
    }
    static constexpr u32 kPow2Cap[3] = {0x2, 0x20000, 0x40000};    // POW2, CUBEMAP_POW2, VOLUMEMAP_POW2
    static constexpr u32 kMipCap[3] = {0x4000, 0x10000, 0x8000};   // MIPMAP, MIPCUBEMAP, MIPVOLUMEMAP
    static constexpr u32 kRtype[3] = {3, 5, 4};                    // D3DRTYPE_TEXTURE, CUBETEXTURE, VOLUMETEXTURE
    if (caps.texture_caps & kPow2Cap[type]) {
        // D3DX doubles until >= the size: above 2^31 it shifts to 0 and never ends.
        if (w > 0x80000000u || h > 0x80000000u || d > 0x80000000u) {  // fix/uninit b)
            undefined = true;
            return kNotAvailable;
        }
        const auto up = [](u32 v) {
            u32 p = 1;
            while (p < v) p <<= 1;
            return p;
        };
        w = up(w);
        h = up(h);
        d = up(d);
    }
    if (pfmt && is_dxt(*pfmt)) {
        w = (w + 3) & ~3u;
        h = (h + 3) & ~3u;
    }
    u32 mips = pmips ? *pmips : kDefault;
    const auto pow2 = [](u32 v) { return (v & (v - 1)) == 0; };
    if ((caps.texture_caps & kMipCap[type]) && pow2(w) && pow2(h) && pow2(d)) {
        const auto count = [](u32 v) {
            u32 n = 1;
            while (v != 0 && !(v & 1)) {
                v >>= 1;
                ++n;
            }
            return n;
        };
        const u32 cw = count(w), ch = count(h), cd = count(d);
        u32 most = cw > ch ? cw : ch;
        if (type == 2 && cd > most) most = cd;
        if (mips > most || mips == 0) mips = most;
    } else {
        mips = 1;
    }

    // Format: the requested one when the device takes it, else the closest convertible table entry (smaller
    // bits per pixel on a tie). The device is asked in table order until the requested format is accepted.
    const u32 want = pfmt ? *pfmt : 0;
    const FormatEntry& want_entry = format_entry(want);
    bool ok[kFormatCount] = {};
    std::size_t end = kFormatCount;
    for (std::size_t i = 0; i < kFormatCount; ++i) {
        const FormatEntry& e = kFormats[i];
        if (!e.listed) continue;
        if (check(usage, kRtype[type], e.format) < 0) continue;
        ok[i] = true;
        if (e.format == want) {
            end = i + 1;
            break;
        }
    }
    // `garbage` is the uninitialised bits-per-pixel D3DX compares a tie at distance ~0 with before the first
    // candidate is taken (0x007643c2 reads [ebp-0x12c]); when the value classes disagree D3DX's result is
    // undefined and fix/uninit a) takes 0.
    const auto select = [&](u32 garbage) {
        u32 chosen = 0, best = ~0u, saved_bpp = garbage;
        for (std::size_t i = 0; i < end; ++i) {
            if (!ok[i]) continue;
            const FormatEntry& e = kFormats[i];
            if (e.format == want) return want;
            if (!e.convertible) continue;
            const u32 dist = format_distance(e, want_entry);
            if (dist < best || (dist == best && e.bpp < saved_bpp)) {
                chosen = e.format;
                saved_bpp = e.bpp;
                best = dist;
            }
        }
        return chosen;
    };
    const u32 chosen = select(0);
    static constexpr u32 kGarbageClasses[] = {9, 17, 25, 33, ~0u};
    for (u32 g : kGarbageClasses)
        if (select(g) != chosen) undefined = true;
    if (chosen == 0) return kNotAvailable;
    if (pw) *pw = w;
    if (ph) *ph = h;
    if (pd) *pd = d;
    if (pmips) *pmips = mips;
    if (pfmt) *pfmt = chosen;
    return kOk;
}

// ---------------------------------------------------------------------------------------------------------------
// Cube loader 0x00765669, device-independent parts.

/// Arguments of D3DXCreateCubeTextureFromFileInMemoryEx.
struct CubeArgs {
    u32 size = kDefault, mips = kDefault, usage = 0, format = 0, pool = 1, filter = kDefault, mip_filter = kDefault;
    u32 color_key = 0;
};

/// What the loader creates and computes. `hr` != 0 with `original` false: D3DX returns that HRESULT before
/// creating anything; `original`: run the original (reason in `why`). pSrcInfo / pPalette are not written by
/// plan_cube: `commit_outputs` writes them on every path that does not fall back (a fallback must not write
/// anything before the original runs).
struct CubeJob {
    long hr = kOk;
    bool original = false;
    std::string why;
    u32 size = 0, levels = 0, usage = 0, format = 0, pool = 0, filter = 0, mip_filter = 0;
    bool info_valid = false;   // CD3DXImage::Load succeeded: D3DX writes pSrcInfo
    u32 info[5] = {};          // D3DXIMAGE_INFO {Width, Height, Depth, MipLevels, Format}
    bool palette_valid = false;  // D3DX reached the palette copy (256 x 0xffffffff for a DDS)
    std::array<render::Image, 6> faces;
};

/// Writes what D3DX writes to pSrcInfo (CD3DXImage::Load) and pPalette (0x00765847) - only on paths that do not
/// fall back to the original.
void commit_outputs(const CubeJob& job, u32* info, u32* palette) {
    if (info && job.info_valid)
        for (int i = 0; i < 5; ++i) info[i] = job.info[i];
    if (palette && job.palette_valid)
        for (int i = 0; i < 256; ++i) palette[i] = 0xffffffffu;
}

/// 0x00765785..0x007657de: the D3DFORMAT D3DX picks for D3DFMT_UNKNOWN.
u32 cube_default_format(u32 src, u32 color_key) {
    const i32 s = static_cast<i32>(src);  // D3DX compares signed (jle)
    if (color_key == 0) return src == 0x14 ? 0x16u : src;
    if (src == 0x14) return 0x15;
    if (s <= 0x15) return src;
    if (s <= 0x17) return 0x15;
    if (src == 0x18 || src == 0x1b || src == 0x1e) return 0x19;
    if (src == 0x32) return 0x33;
    return src;
}

/// The six faces of a decoded DDS cube map (face-major file layout, as 0x00788229 reads it; render::decode_image
/// has validated every face's levels against the data size and keeps face 0).
bool split_faces(const u8* data, std::size_t size, const render::Image& image, std::array<render::Image, 6>& faces) {
    if (!image.cube || image.levels.empty() || image.type != render::ImageFileType::Dds) return false;
    const u32 format = image.format;
    std::size_t at = 4 + 0x7c;
    for (u32 face = 0; face < 6; ++face) {
        render::Image& f = faces[face];
        f = render::Image{};
        f.type = render::ImageFileType::Dds;
        f.format = format;
        u32 w = image.width(), h = image.height();
        for (std::size_t l = 0; l < image.levels.size(); ++l) {
            u32 pitch, rows;
            if (is_dxt(format)) {
                pitch = ((w + 3) >> 2) * (format == 0x31545844 ? 8u : 16u);
                rows = (h + 3) >> 2;
            } else if (format == 0x15 || format == 0x16) {
                pitch = w * 4;
                rows = h;
            } else if (format == 0x17 || format == 0x18 || format == 0x19 || format == 0x1a) {
                pitch = w * 2;
                rows = h;
            } else {
                return false;
            }
            const std::uint64_t bytes = static_cast<std::uint64_t>(pitch) * rows;
            if (bytes > size - at) return false;
            render::ImageLevel level;
            level.width = w;
            level.height = h;
            level.pitch = pitch;
            level.data.assign(data + at, data + at + static_cast<std::size_t>(bytes));
            f.levels.push_back(std::move(level));
            at += static_cast<std::size_t>(bytes);
            w = w == 1 ? 1 : w >> 1;
            h = h == 1 ? 1 : h >> 1;
        }
    }
    // Face 0 must be what render::decode_image read.
    for (std::size_t l = 0; l < image.levels.size(); ++l)
        if (faces[0].levels[l].data != image.levels[l].data || faces[0].levels[l].pitch != image.levels[l].pitch)
            return false;
    return true;
}

/// 0x00765669 up to the device creation. `info` / `palette`: the caller's pSrcInfo / pPalette (may be NULL).
/// `check_cube(&size, &mips, usage, &format, pool)` is D3DXCheckCubeTextureRequirements.
template <class CheckCube>
CubeJob plan_cube(const void* data, u32 size, const CubeArgs& args, u32* info, u32* palette, CheckCube&& check_cube) {
    CubeJob job;
    const auto original = [&](const char* why) {
        job.original = true;
        job.why = why;
        return job;
    };
    render::Image image;
    std::string why;
    if (!render::decode_image(data, size, image, &why)) return original(why.c_str());
    // CD3DXImage::Load 0x0078856c: D3DXIMAGE_INFO {Width, Height, Depth 1, MipLevels, Format} (written later by
    // commit_outputs; `info` is not read here).
    (void)info;
    job.info_valid = true;
    job.info[0] = image.width();
    job.info[1] = image.height();
    job.info[2] = 1;
    job.info[3] = static_cast<u32>(image.levels.size());
    job.info[4] = image.format;
    if (!image.cube) {
        job.hr = kFail;  // one face
        return job;
    }
    if (!split_faces(static_cast<const u8*>(data), size, image, job.faces)) return original("cube faces");
    u32 edge = args.size;
    if (edge == 0 || edge == kDefault) {
        const i32 w = static_cast<i32>(image.width()), h = static_cast<i32>(image.height());
        edge = static_cast<u32>(w > h ? w : h);
    }
    const u32 src = image.format;
    const bool bump = format_entry(src).type == 3;
    u32 filter = args.filter, mip_filter = args.mip_filter;
    if (filter == kDefault) filter = bump ? 0x80002u : 0x80004u;
    if (mip_filter == kDefault) mip_filter = bump ? 2u : 5u;
    filter |= 0x70000;
    mip_filter |= 0x70000;
    u32 format = args.format == 0 ? cube_default_format(src, args.color_key) : args.format;
    // Palette: a DDS image has none, so D3DX's table is 256 x 0xffffffff and the colour key stays for the blits.
    if (image.has_palette) return original("palette image");  // unreachable: only DDS has 6 faces
    if (!palette) {
        if (format == 0x28)
            format = 0x15;
        else if (format == 0x29)
            return original("D3DFMT_P8 without a source palette (D3DX reads address 3)");
    }
    if (args.color_key != 0) return original("colour key");
    // Checked before D3DXCheckCubeTextureRequirements (D3DX fails on them later, in D3DXLoadSurfaceFromMemory /
    // by resampling): falling back earlier only saves the device queries, the original repeats them anyway.
    const auto valid = [](u32 f) { return (f & 0xffff) >= 1 && (f & 0xffff) <= 5 && !(f & 0xfff00000u); };
    if (!valid(filter)) return original("filter");
    for (const render::ImageLevel& l : job.faces[0].levels)
        if (l.width != l.height) return original("non-square faces (resampling)");
    u32 levels = args.mips;
    long hr = check_cube(&edge, &levels, args.usage, &format, args.pool);
    job.palette_valid = true;  // D3DX copied the palette before 0x00764920
    if (hr < 0) {
        job.hr = hr;
        return job;
    }
    if (image.width() != edge) return original("size differs from the file (resampling)");
    job.size = edge;
    job.levels = levels;
    job.usage = args.usage;
    job.format = format;
    job.pool = args.pool;
    job.filter = filter;
    job.mip_filter = mip_filter;
    return job;
}

/// Every level of every face (tightly packed, pitch = render::row_bytes), as D3DXLoadSurfaceFromMemory +
/// D3DXFilterCubeTexture compute them.
bool build_cube(const CubeJob& job, render::Precision precision, std::array<std::vector<render::ImageLevel>, 6>& out,
                std::string* why) {
    render::TexturePlan plan;
    plan.width = plan.height = job.size;
    plan.levels = job.levels;
    plan.usage = job.usage;
    plan.format = job.format;
    plan.pool = job.pool;
    plan.filter = job.filter;
    plan.mip_filter = job.mip_filter;
    plan.palette.fill(0xffffffffu);
    for (int f = 0; f < 6; ++f)
        if (!render::build_texture(job.faces[f], plan, precision, out[f], why)) return false;
    return true;
}

#ifndef JOAN_B114_16_CORE_TEST
// =================================================================================================================
// Device glue (Windows).

using joan::hook::log_line;

JOAN_COUNTER(c_req, "D3DXCheck(Cube)TextureRequirements");
JOAN_COUNTER(c_cube, "D3DXCreateCubeTextureFromFileInMemoryEx");
JOAN_COUNTER(c_cube_original, "D3DXCreateCubeTextureFromFileInMemoryEx (D3DX fallback)");

constexpr fn::Code kCheckTex = fn::D3DXCheckTextureRequirements;
constexpr fn::Code kCheckCube = fn::D3DXCheckCubeTextureRequirements;
constexpr fn::Code kTexFromMemEx = fn::fn_765258;
constexpr fn::Code kCubeFromMemEx = fn::D3DXCreateCubeTextureFromFileInMemoryEx;
constexpr fn::Code kTexFromFileExA = fn::D3DXCreateTextureFromFileExA;
constexpr fn::Code kCubeFromFileExA = fn::D3DXCreateCubeTextureFromFileExA;

/// IDirect3DCubeTexture8 (vtable order of d3d8.h).
struct CubeTexture8 {
    virtual d3d8::HRESULT JOAN_STDCALL QueryInterface(const d3d8::Guid& riid, void** out) = 0;
    virtual d3d8::ULONG JOAN_STDCALL AddRef() = 0;
    virtual d3d8::ULONG JOAN_STDCALL Release() = 0;
    virtual d3d8::HRESULT JOAN_STDCALL GetDevice(d3d8::IDirect3DDevice8** out) = 0;
    virtual d3d8::HRESULT JOAN_STDCALL SetPrivateData(const d3d8::Guid& guid, const void* data, d3d8::DWORD size,
                                                   d3d8::DWORD flags) = 0;
    virtual d3d8::HRESULT JOAN_STDCALL GetPrivateData(const d3d8::Guid& guid, void* data, d3d8::DWORD* size) = 0;
    virtual d3d8::HRESULT JOAN_STDCALL FreePrivateData(const d3d8::Guid& guid) = 0;
    virtual d3d8::DWORD JOAN_STDCALL SetPriority(d3d8::DWORD priority) = 0;
    virtual d3d8::DWORD JOAN_STDCALL GetPriority() = 0;
    virtual void JOAN_STDCALL PreLoad() = 0;
    virtual d3d8::Enum JOAN_STDCALL GetType() = 0;
    virtual d3d8::DWORD JOAN_STDCALL SetLOD(d3d8::DWORD lod) = 0;
    virtual d3d8::DWORD JOAN_STDCALL GetLOD() = 0;
    virtual d3d8::DWORD JOAN_STDCALL GetLevelCount() = 0;
    virtual d3d8::HRESULT JOAN_STDCALL GetLevelDesc(d3d8::UINT level, d3d8::SurfaceDesc* desc) = 0;
    virtual d3d8::HRESULT JOAN_STDCALL GetCubeMapSurface(d3d8::Enum face, d3d8::UINT level, d3d8::IDirect3DSurface8** out) = 0;
    virtual d3d8::HRESULT JOAN_STDCALL LockRect(d3d8::Enum face, d3d8::UINT level, d3d8::LockedRect* locked,
                                             const d3d8::Rect* rect, d3d8::DWORD flags) = 0;
    virtual d3d8::HRESULT JOAN_STDCALL UnlockRect(d3d8::Enum face, d3d8::UINT level) = 0;
    virtual d3d8::HRESULT JOAN_STDCALL AddDirtyRect(d3d8::Enum face, const d3d8::Rect* rect) = 0;
};

/// Logs each distinct reason once.
void note_once(const char* what, const std::string& reason) {
    static std::mutex m;
    static std::vector<std::string> seen;
    const std::string key = std::string(what) + ": " + reason;
    {
        std::lock_guard lock(m);
        if (std::find(seen.begin(), seen.end(), key) != seen.end()) return;
        seen.push_back(key);
    }
    log_line("d3dx/api: %s - first occurrence", key.c_str());
}

/// 0x00764064 with the device calls in D3DX's order (GetDirect3D, GetDeviceCaps, GetDisplayMode, CheckDeviceFormat
/// in table order, Release).
long requirements(d3d8::IDirect3DDevice8* device, u32* pw, u32* ph, u32* pd, u32* pmips, u32 usage, u32* pfmt,
                  u32 pool, u32 type) {
    const u32 in_w = pw ? *pw : 0, in_h = ph ? *ph : 0, in_d = pd ? *pd : 0, in_m = pmips ? *pmips : 0,
              in_f = pfmt ? *pfmt : 0;
    long hr = req_validate(device != nullptr, usage, pool, type);
    if (hr != kOk) return hr;
    void* d3d_raw = nullptr;
    d3d8::Caps8 caps{};
    d3d8::DisplayMode mode{};
    const bool got_d3d = device->GetDirect3D(&d3d_raw) >= 0 && d3d_raw;
    const bool caps_ok = device->GetDeviceCaps(&caps) >= 0;
    const bool mode_ok = device->GetDisplayMode(&mode) >= 0;
    if (!got_d3d || !caps_ok || !mode_ok) {  // fix/uninit c)
        if (got_d3d) static_cast<d3d8::IDirect3D8*>(d3d_raw)->Release();
        note_once("requirements", "GetDirect3D / GetDeviceCaps / GetDisplayMode failed: D3DERR_NOTAVAILABLE");
        return kNotAvailable;
    }
    auto* d3d = static_cast<d3d8::IDirect3D8*>(d3d_raw);
    ReqCaps rc;
    rc.texture_caps = caps.texture_caps;
    rc.max_width = caps.max_texture_width;
    rc.max_height = caps.max_texture_height;
    rc.max_volume = caps.max_volume_extent;
    rc.max_aspect = caps.max_texture_aspect_ratio;
    bool undefined = false;
    // Outputs go to locals first and are stored only on success.
    u32 w = in_w, h = in_h, d = in_d, m = in_m, f = in_f;
    u32* lw = pw ? &w : nullptr;
    u32* lh = ph ? (ph == pw ? lw : &h) : nullptr;
    hr = req_compute(
        rc,
        [&](u32 u, u32 rtype, u32 format) {
            return static_cast<long>(d3d->CheckDeviceFormat(caps.adapter_ordinal, caps.device_type, mode.format, u, rtype, format));
        },
        lw, lh, pd ? &d : nullptr, pmips ? &m : nullptr, usage, pfmt ? &f : nullptr, type, undefined);
    d3d->Release();
    if (undefined) note_once("requirements", "D3DX result undefined (uninitialised compare / endless loop): fix/uninit choice");
    c_req.hit();
    if (hr >= 0) {
        if (pw) *pw = w;
        if (ph && ph != pw) *ph = h;
        if (pd) *pd = d;
        if (pmips) *pmips = m;
        if (pfmt) *pfmt = f;
    }
    return hr;
}

// 0x00764892 D3DXCheckTextureRequirements, ret 0x1c.
long JOAN_STDCALL check_texture_requirements(d3d8::IDirect3DDevice8* device, u32* pw, u32* ph, u32* pmips, u32 usage,
                                          u32* pfmt, u32 pool) {
    return requirements(device, pw, ph, nullptr, pmips, usage, pfmt, pool, 0);
}

// 0x00764920 D3DXCheckCubeTextureRequirements, ret 0x18 (D3DX passes pSize as both width and height; the
// height is written last).
long JOAN_STDCALL check_cube_texture_requirements(d3d8::IDirect3DDevice8* device, u32* psize, u32* pmips, u32 usage,
                                               u32* pfmt, u32 pool) {
    return requirements(device, psize, psize, nullptr, pmips, usage, pfmt, pool, 1);
}

using CheckTexFn = long(JOAN_STDCALL*)(d3d8::IDirect3DDevice8*, u32*, u32*, u32*, u32, u32*, u32);
using CheckCubeFn = long(JOAN_STDCALL*)(d3d8::IDirect3DDevice8*, u32*, u32*, u32, u32*, u32);

// 0x007648ba D3DXCreateTexture, ret 0x20.
long JOAN_STDCALL create_texture(d3d8::IDirect3DDevice8* device, u32 width, u32 height, u32 mips, u32 usage, u32 format,
                              u32 pool, d3d8::IDirect3DTexture8** out) {
    if (!device || !out) return kInvalidCall;
    if (usage == kDefault) usage = 0;
    long hr = reinterpret_cast<CheckTexFn>(fn::code_value(kCheckTex))(device, &width, &height, &mips, usage, &format, pool);
    if (hr < 0) return hr;
    hr = device->CreateTexture(width, height, mips, usage, format, pool, out);
    if (hr < 0) return hr;
    return kOk;
}

// 0x00764948 D3DXCreateCubeTexture, ret 0x1c.
long JOAN_STDCALL create_cube_texture(d3d8::IDirect3DDevice8* device, u32 size, u32 mips, u32 usage, u32 format, u32 pool,
                                   d3d8::IDirect3DCubeTexture8** out) {
    if (!device || !out) return kInvalidCall;
    if (usage == kDefault) usage = 0;
    long hr = reinterpret_cast<CheckCubeFn>(fn::code_value(kCheckCube))(device, &size, &mips, usage, &format, pool);
    if (hr < 0) return hr;
    hr = device->CreateCubeTexture(size, mips, usage, format, pool, out);
    if (hr < 0) return hr;
    return kOk;
}

// --- 0x00765669 ---
using CubeFromMemExFn = long(JOAN_STDCALL*)(d3d8::IDirect3DDevice8* device, const void* data, u32 size, u32 edge, u32 mips,
                                         u32 usage, u32 format, u32 pool, u32 filter, u32 mip_filter, u32 color_key,
                                         void* src_info, void* palette, CubeTexture8** out);

/// x87 precision control in force (round to nearest; SSE default) - as lib/d3dx8/texture_in_memory.cpp.
bool fpu_precision(render::Precision& out) {
    const joan::fp::Precision p = joan::fp::current_precision();  // the emulated x87 precision of this thread
    if (p == joan::fp::Precision::Unsupported) return false;
#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
    if ((_mm_getcsr() & 0x8040u) != 0) return false;  // SSE FTZ (bit 15) / DAZ (bit 6)
#endif
    out = p == joan::fp::Precision::Single ? render::Precision::Single : render::Precision::Double;
    return true;
}

/// Writes the computed levels into the six faces of `cube` (GetCubeMapSurface, LockRect, UnlockRect, Release).
bool fill_cube(CubeTexture8* cube, const CubeJob& job, const std::array<std::vector<render::ImageLevel>, 6>& levels) {
#if defined(__GNUC__) && !defined(__clang__)
// false positive: GCC flags the virtual call through the CubeTexture8 interface (its `this`) as uninitialised
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
#endif
    if (cube->GetLevelCount() != job.levels) return false;
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
    for (u32 l = 0; l < job.levels; ++l) {
        d3d8::SurfaceDesc desc{};
        if (cube->GetLevelDesc(l, &desc) != kOk || desc.format != job.format || desc.width != levels[0][l].width ||
            desc.height != levels[0][l].height)
            return false;
    }
    for (u32 face = 0; face < 6; ++face)
        for (u32 l = 0; l < job.levels; ++l) {
            const render::ImageLevel& src = levels[face][l];
            d3d8::IDirect3DSurface8* surface = nullptr;
            if (cube->GetCubeMapSurface(face, l, &surface) != kOk || !surface) return false;
            d3d8::LockedRect lr{};
            if (surface->LockRect(&lr, nullptr, 0) != kOk) {
                surface->Release();
                return false;
            }
            const bool ok = lr.bits != nullptr;
            if (ok) {
                const u32 rows = render::row_count(job.format, src.height);
                for (u32 y = 0; y < rows; ++y)
                    std::memcpy(static_cast<u8*>(lr.bits) + static_cast<std::ptrdiff_t>(y) * lr.pitch,
                                src.data.data() + static_cast<std::size_t>(y) * src.pitch, src.pitch);
            }
            surface->UnlockRect();
            surface->Release();
            if (!ok) return false;
        }
    return true;
}

// 0x00765669 D3DXCreateCubeTextureFromFileInMemoryEx, ret 0x38.
long JOAN_STDCALL create_cube_texture_from_file_in_memory_ex(d3d8::IDirect3DDevice8* device, const void* data, u32 size,
                                                          u32 edge, u32 mips, u32 usage, u32 format, u32 pool,
                                                          u32 filter, u32 mip_filter, u32 color_key, void* src_info,
                                                          void* palette, CubeTexture8** out) {
    // joan.exe's D3DX code no longer exists (stage S5 / S7c): what the loader does not reproduce fails with E_FAIL.
    const auto fall_back = [&](const std::string& reason) -> long {
        c_cube_original.hit();
        note_once("cube texture fails with E_FAIL", reason);
        return kFail;
    };
    if (!device || !data || !size || !out) return kInvalidCall;
    CubeTexture8* cube = nullptr;
    CubeTexture8* temp = nullptr;
    // A C++ exception (allocation failure on a huge file) must not unwind into joan.exe.
    try {
        CubeArgs args;
        args.size = edge;
        args.mips = mips;
        args.usage = usage;
        args.format = format;
        args.pool = pool;
        args.filter = filter;
        args.mip_filter = mip_filter;
        args.color_key = color_key;
        CubeJob job = plan_cube(data, size, args, static_cast<u32*>(src_info), static_cast<u32*>(palette),
                                [&](u32* pe, u32* pm, u32 u, u32* pf, u32 p) {
                                    return reinterpret_cast<CheckCubeFn>(fn::code_value(kCheckCube))(device, pe, pm, u, pf, p);
                                });
        if (job.original) return fall_back(job.why);
        const auto commit = [&]() { commit_outputs(job, static_cast<u32*>(src_info), static_cast<u32*>(palette)); };
        if (job.hr != kOk) {
            commit();
            c_cube.hit();
            return job.hr;
        }
        render::Precision precision{};
        if (!fpu_precision(precision)) return fall_back("x87 / SSE control word");
        std::array<std::vector<render::ImageLevel>, 6> levels;
        std::string why;
        if (!build_cube(job, precision, levels, &why)) return fall_back(why);

        long hr = device->CreateCubeTexture(job.size, job.levels, job.usage, job.format, job.pool,
                                            reinterpret_cast<d3d8::IDirect3DCubeTexture8**>(&cube));
        if (hr < 0) {
            commit();
            c_cube.hit();
            return hr;
        }
        CubeTexture8* target = cube;
        if (job.pool == 0) {
            hr = device->CreateCubeTexture(job.size, job.levels, 0, job.format, 2,
                                           reinterpret_cast<d3d8::IDirect3DCubeTexture8**>(&temp));
            if (hr < 0) {
                cube->Release();
                cube = nullptr;
                commit();
                c_cube.hit();
                return hr;
            }
            target = temp;
        }
        const auto release_all = [&]() {  // D3DX's order (0x00765a7b): the cube, then the system-memory twin
            if (cube) cube->Release();
            cube = nullptr;
            if (temp) temp->Release();
            temp = nullptr;
        };
        // D3DX computes after CreateCubeTexture: follow a control word the runtime changed meanwhile.
        render::Precision now{};
        if (!fpu_precision(now)) {
            release_all();
            return fall_back("x87 / SSE control word");
        }
        if (now != precision && !build_cube(job, now, levels, &why)) {
            release_all();
            return fall_back(why);
        }
        if (!fill_cube(target, job, levels)) {
            release_all();
            return fall_back("cube levels do not match the plan / surface or LockRect failed");
        }
        if (job.pool == 0) {
            hr = device->UpdateTexture(reinterpret_cast<d3d8::IDirect3DBaseTexture8*>(temp),
                                       reinterpret_cast<d3d8::IDirect3DBaseTexture8*>(cube));
            if (hr < 0) {
                release_all();
                commit();
                c_cube.hit();
                return hr;
            }
            temp->Release();
            temp = nullptr;
        }
        commit();
        c_cube.hit();
        *out = cube;
        return kOk;
    } catch (const std::exception&) {
        if (temp) temp->Release();
        if (cube) cube->Release();
        c_cube_original.hit();
        return kFail;
    }
}

// --- forwarders (D3DX_DEFAULT arguments, managed pool) ---
using TexFromMemExFn = long(JOAN_STDCALL*)(void* device, const void* data, u32 size, u32 width, u32 height, u32 mips,
                                       u32 usage, u32 format, u32 pool, u32 filter, u32 mip_filter, u32 color_key,
                                       void* src_info, void* palette, void* out);
using TexFromFileExFn = long(JOAN_STDCALL*)(void* device, const char* file, u32 width, u32 height, u32 mips, u32 usage,
                                        u32 format, u32 pool, u32 filter, u32 mip_filter, u32 color_key, void* src_info,
                                        void* palette, void* out);
using CubeFromFileExFn = long(JOAN_STDCALL*)(void* device, const char* file, u32 edge, u32 mips, u32 usage, u32 format,
                                         u32 pool, u32 filter, u32 mip_filter, u32 color_key, void* src_info,
                                         void* palette, void* out);

// 0x00765c3d D3DXCreateTextureFromFileInMemory, ret 0x10.
long JOAN_STDCALL create_texture_from_file_in_memory(void* device, const void* data, u32 size, void* out) {
    return reinterpret_cast<TexFromMemExFn>(fn::code_value(kTexFromMemEx))(device, data, size, kDefault, kDefault, kDefault, 0, 0, 1,
                                                            kDefault, kDefault, 0, nullptr, nullptr, out);
}
// 0x00765d26 D3DXCreateCubeTextureFromFileInMemory, ret 0x10.
long JOAN_STDCALL create_cube_texture_from_file_in_memory(void* device, const void* data, u32 size, void* out) {
    return reinterpret_cast<CubeFromMemExFn>(fn::code_value(kCubeFromMemEx))(static_cast<d3d8::IDirect3DDevice8*>(device), data, size,
                                                              kDefault, kDefault, 0, 0, 1, kDefault, kDefault, 0,
                                                              nullptr, nullptr, static_cast<CubeTexture8**>(out));
}
// 0x00765d4e D3DXCreateTextureFromFileA, ret 0xc.
long JOAN_STDCALL create_texture_from_file_a(void* device, const char* file, void* out) {
    return reinterpret_cast<TexFromFileExFn>(fn::code_value(kTexFromFileExA))(device, file, kDefault, kDefault, kDefault, 0, 0, 1,
                                                              kDefault, kDefault, 0, nullptr, nullptr, out);
}
// 0x00765dea D3DXCreateCubeTextureFromFileA, ret 0xc.
long JOAN_STDCALL create_cube_texture_from_file_a(void* device, const char* file, void* out) {
    return reinterpret_cast<CubeFromFileExFn>(fn::code_value(kCubeFromFileExA))(device, file, kDefault, kDefault, 0, 0, 1, kDefault,
                                                                kDefault, 0, nullptr, nullptr, out);
}
#endif  // JOAN_B114_16_CORE_TEST

}  // namespace

#ifndef JOAN_B114_16_CORE_TEST
JOAN_REPLACE("d3dx/api", "D3DXCheckTextureRequirements", 0x00764892, &check_texture_requirements);
JOAN_REPLACE("d3dx/api", "D3DXCreateTexture", 0x007648ba, &create_texture);
JOAN_REPLACE("d3dx/api", "D3DXCheckCubeTextureRequirements", 0x00764920, &check_cube_texture_requirements);
JOAN_REPLACE("d3dx/api", "D3DXCreateCubeTexture", 0x00764948, &create_cube_texture);
JOAN_REPLACE("d3dx/api", "D3DXCreateCubeTextureFromFileInMemoryEx", 0x00765669, &create_cube_texture_from_file_in_memory_ex);
JOAN_REPLACE("d3dx/api", "D3DXCreateTextureFromFileInMemory", 0x00765c3d, &create_texture_from_file_in_memory);
JOAN_REPLACE("d3dx/api", "D3DXCreateCubeTextureFromFileInMemory", 0x00765d26, &create_cube_texture_from_file_in_memory);
JOAN_REPLACE("d3dx/api", "D3DXCreateTextureFromFileA", 0x00765d4e, &create_texture_from_file_a);
JOAN_REPLACE("d3dx/api", "D3DXCreateCubeTextureFromFileA", 0x00765dea, &create_cube_texture_from_file_a);
#endif

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::d3dx::create_texture(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, void* a7) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::create_texture>(a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x7648ba
auto joan::orig::d3dx::create_cube_texture(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, void* a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::create_cube_texture>(a0, a1, a2, a3, a4, a5, a6)); }  // 0x764948
auto joan::orig::d3dx::create_cube_texture_from_file_in_memory_ex(void* a0, void* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10, void* a11, void* a12, void* a13) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::create_cube_texture_from_file_in_memory_ex>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)); }  // 0x765669
auto joan::orig::d3dx::create_texture_from_file_in_memory(void* a0, void* a1, std::uint32_t a2, void* a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::create_texture_from_file_in_memory>(a0, a1, a2, a3)); }  // 0x765c3d
auto joan::orig::d3dx::create_cube_texture_from_file_in_memory(void* a0, void* a1, std::uint32_t a2, void* a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::create_cube_texture_from_file_in_memory>(a0, a1, a2, a3)); }  // 0x765d26
auto joan::orig::d3dx::create_texture_from_file_a(void* a0, const char* a1, void* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::create_texture_from_file_a>(a0, a1, a2)); }  // 0x765d4e
auto joan::orig::d3dx::create_cube_texture_from_file_a(void* a0, const char* a1, void* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::create_cube_texture_from_file_a>(a0, a1, a2)); }  // 0x765dea
