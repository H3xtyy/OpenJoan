// Adapter queries of the OpenGL renderer's IDirect3D8 (gl/direct3d.cpp):
//
//  * NativeCaps=1 ([Graphics]): the answers the real Direct3D 8 runtime used to give - caps,
//    adapter identifier, display modes, format / device type / depth-stencil checks - come from
//    here, without creating the system's IDirect3D8. Needed for the standalone port, which has
//    no d3d8.dll.
//  * Dump (always with Renderer=opengl, once per session): what the real runtime answers on the
//    user's machine is written to openjoan.log ("d3d8 adapter:" lines), so the native table can be
//    checked against real hardware. With NativeCaps=1 the native answers are dumped the same way.
//
// IMPORTANT - texture loading depends on these answers. D3DX's texture loader in joan.exe
// (D3DXCheckTextureRequirements 0x00764064) and our port of it (lib/d3dx8/texture_in_memory.cpp,
// render/image.cpp plan_texture) choose the texture size, mip count and format from
// D3DCAPS8::TextureCaps (POW2 0x2 rounds sizes up to powers of two, SQUAREONLY 0x20, MIPMAP 0x4000:
// mips only for power-of-two sizes), MaxTextureWidth / Height / AspectRatio and
// CheckDeviceFormat(..., D3DRTYPE_TEXTURE, format) for each entry of D3DX's format table. The
// game itself rounds some textures with the same POW2 / SQUAREONLY bits (0x00684d83). Changing
// the native table changes texture sizes / formats: keep it equal to what real drivers report.
#pragma once

#include <cstdint>
#include <vector>

#include "../d3d8/d3d8_api.hpp"
#include "../d3d8/d3d8_types.hpp"

/// Adapter queries of the OpenGL renderer's IDirect3D8 (caps, display modes, format checks).
namespace joan::gld::adapter {

using namespace joan::d3d8;

/// D3DADAPTER_IDENTIFIER8 (d3d8types.h), 0x42c bytes; the game keeps an array of them (0x00a33768).
struct Identifier8 {
    char driver[512];  ///< Adapter driver file name, NUL-terminated.
    char description[512];  ///< Adapter description string, NUL-terminated.
    std::uint32_t driver_version_low /**< low half of DriverVersion (LARGE_INTEGER) */, driver_version_high /**< high half */;
    DWORD vendor_id /**< PCI vendor id */, device_id /**< PCI device id */, sub_sys_id /**< subsystem id */, revision /**< revision */;
    Guid device_identifier;  ///< Driver/device identifier GUID.
    DWORD whql_level;  ///< WHQL certification level (see kEnumNoWhqlLevel for the flag that skips it).
};
static_assert(sizeof(Identifier8) == 0x42c);

/// D3DDEVTYPE_HAL as an Enum value.
inline constexpr Enum kDeviceHal = 1;  // D3DDEVTYPE_HAL
/// D3DENUM_NO_WHQL_LEVEL as a flag value.
inline constexpr DWORD kEnumNoWhqlLevel = 2;  // D3DENUM_NO_WHQL_LEVEL

// --- native answers (NativeCaps=1) ----------------------------------------------------------
/// Display devices attached to the desktop, the primary one first (Direct3D's adapter order).
UINT count();
/// Current mode of the adapter's display (platform::current_display_mode).
bool current_mode(UINT adapter, DisplayMode& out);
/// Every 32-bit mode the display offers from 640x400 up (X8R8G8B8, as the runtime of current
/// Windows lists them), without duplicates, by size, refresh rates in the driver's order.
std::vector<DisplayMode> modes(UINT adapter);
/// The monitor handle of the adapter's display.
d3d8::HMONITOR monitor(UINT adapter);
/// Fills `out` with the adapter's identifier (D3DADAPTER_IDENTIFIER8); `flags` as for GetAdapterIdentifier.
void identifier(UINT adapter, Identifier8& out, DWORD flags);
/// The fixed caps table (see adapter.cpp): what a current D3D9-class driver reports through the
/// Direct3D 8 runtime (= the real dump of a current GPU), BEFORE direct3d.cpp's patch_caps
/// (limits of our OpenGL device).
Caps8 caps(UINT adapter);
/// CheckDeviceType: whether the adapter supports the device type with these display and back buffer formats; returns the HRESULT the runtime would.
HRESULT check_device_type(UINT adapter, Enum type, Enum display_format, Enum back_format, BOOL windowed);
/// Depth / stencil and buffer formats; texture and surface formats stay with gl/formats.hpp.
HRESULT check_device_format(UINT adapter, Enum type, Enum adapter_format, DWORD usage, Enum rtype, Enum format);
/// CheckDepthStencilMatch: whether the depth / stencil format can be used with the render target format; returns the HRESULT the runtime would.
HRESULT check_depth_stencil_match(UINT adapter, Enum type, Enum adapter_format, Enum rt_format, Enum depth_format);

// --- logging (openjoan.log, "d3d8 adapter:" lines) ----------------------------------------------------
/// Name of a D3DFMT_* value for the log.
const char* format_name(Enum format);
/// "ok", "NOTAVAILABLE", "INVALIDCALL" or the HRESULT in hex.
const char* result_name(HRESULT r, char (&buf)[16]);
/// Hex of the whole D3DCAPS8 plus every field decoded, prefixed with `label`.
void log_caps(const char* label, const Caps8& caps);
/// Logs the adapter identifier, prefixed with `label`.
void log_identifier(const char* label, UINT adapter, const Identifier8& id);
/// Logs the list of display modes, prefixed with `label`.
void log_modes(const char* label, UINT adapter, const std::vector<DisplayMode>& list);
/// True the first time this query (method + arguments) is seen in the session (thread safe), so
/// each distinct query the game / D3DX makes is logged once. False while a dump runs.
bool first_query(const char* method, std::uint32_t a = 0, std::uint32_t b = 0, std::uint32_t c = 0,
                 std::uint32_t d = 0, std::uint32_t e = 0, std::uint32_t f = 0);
/// One-time dump (per `label`) of everything `answers` reports: per adapter (at most 4) the
/// identifier, current mode, mode list, HAL caps (or, with `native_table`, adapter::caps before
/// patch_caps), and the CheckDeviceFormat / CheckDeviceType / CheckDepthStencilMatch /
/// CheckDeviceMultiSampleType answers for every combination the game (SysEn 0x005f6c00,
/// 0x005ff9d0, 0x005ffc90) and D3DX (0x00764064, the texture format table) can ask. With `ours`
/// (our IDirect3D8 while `answers` is the real one) the format checks list our answer next to
/// the real one where they differ ("ours ..."). Without `native_table` (the real runtime's dump)
/// the native answers are compared too: adapter count, mode list ("= the native list" or the
/// native list), caps ("= the native table" or the fields that differ) and the depth / device
/// type / depth-stencil checks ("native ..." where they differ).
void dump(const char* label, IDirect3D8* answers, IDirect3D8* ours, bool native_table);

}  // namespace joan::gld::adapter
