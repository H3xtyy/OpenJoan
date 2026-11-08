// Direct3D 8 interface declarations (the Windows SDK no longer ships d3d8.h).
//
// The two interfaces the game creates itself - IDirect3D8 and IDirect3DDevice8 -
// are declared method by method, in vtable order, as X-macro lists:
//
//   X(return type, method name, (parameter list), (argument names))
//
// so the OpenGL implementation (gl/device.hpp) can be generated from one list. Every parameter is 4 bytes (__stdcall, `this` first),
// so a declaration's stack size is 4 * (parameters + 1). Struct and resource
// interface parameters are opaque here; the parts that are inspected are
// declared below.
#pragma once
#include "../callconv.hpp"

#include <cstdint>

namespace joan::d3d8 {

using HRESULT = std::int32_t;   // 32 bits everywhere (Windows: long; LP64 long is 64 bits - Phase 2, lib-2)
using ULONG = std::uint32_t;
using DWORD = std::uint32_t;
using UINT = std::uint32_t;
using BOOL = int;
using HWND = void*;
using HMONITOR = void*;
using D3DCOLOR = std::uint32_t;
using Enum = std::uint32_t;  // D3DFORMAT, D3DPOOL, D3DRENDERSTATETYPE, ...

inline constexpr HRESULT kOk = 0;
inline constexpr HRESULT kFail = static_cast<HRESULT>(0x80004005);
inline constexpr UINT kSdkVersion = 120;  // D3D_SDK_VERSION of DirectX 8.1

struct Guid {
    std::uint32_t d1;
    std::uint16_t d2, d3;
    std::uint8_t d4[8];
};

struct PresentParameters {  // D3DPRESENT_PARAMETERS
    UINT back_buffer_width, back_buffer_height;
    Enum back_buffer_format;
    UINT back_buffer_count;
    Enum multi_sample_type;
    Enum swap_effect;
    HWND device_window;
    BOOL windowed;
    BOOL enable_auto_depth_stencil;
    Enum auto_depth_stencil_format;
    DWORD flags;
    UINT fullscreen_refresh_rate;
    UINT presentation_interval;
};
static_assert(sizeof(void*) != 4 || sizeof(PresentParameters) == 13 * 4);

// Opaque resource interfaces (real D3D8 objects pass through untouched).
struct IDirect3DSurface8;
struct IDirect3DSwapChain8;
struct IDirect3DBaseTexture8;
struct IDirect3DTexture8;
struct IDirect3DVolumeTexture8;
struct IDirect3DCubeTexture8;
struct IDirect3DVertexBuffer8;
struct IDirect3DIndexBuffer8;
struct IDirect3DDevice8;

// clang-format off
#define JOAN_IDIRECT3D8_METHODS(X) \
    X(HRESULT, QueryInterface, (const Guid& riid, void** out), (riid, out)) \
    X(ULONG, AddRef, (), ()) \
    X(ULONG, Release, (), ()) \
    X(HRESULT, RegisterSoftwareDevice, (void* init), (init)) \
    X(UINT, GetAdapterCount, (), ()) \
    X(HRESULT, GetAdapterIdentifier, (UINT adapter, DWORD flags, void* ident), (adapter, flags, ident)) \
    X(UINT, GetAdapterModeCount, (UINT adapter), (adapter)) \
    X(HRESULT, EnumAdapterModes, (UINT adapter, UINT mode, void* display_mode), (adapter, mode, display_mode)) \
    X(HRESULT, GetAdapterDisplayMode, (UINT adapter, void* display_mode), (adapter, display_mode)) \
    X(HRESULT, CheckDeviceType, (UINT adapter, Enum dev_type, Enum display_fmt, Enum back_fmt, BOOL windowed), (adapter, dev_type, display_fmt, back_fmt, windowed)) \
    X(HRESULT, CheckDeviceFormat, (UINT adapter, Enum dev_type, Enum adapter_fmt, DWORD usage, Enum rtype, Enum check_fmt), (adapter, dev_type, adapter_fmt, usage, rtype, check_fmt)) \
    X(HRESULT, CheckDeviceMultiSampleType, (UINT adapter, Enum dev_type, Enum surface_fmt, BOOL windowed, Enum ms_type), (adapter, dev_type, surface_fmt, windowed, ms_type)) \
    X(HRESULT, CheckDepthStencilMatch, (UINT adapter, Enum dev_type, Enum adapter_fmt, Enum rt_fmt, Enum ds_fmt), (adapter, dev_type, adapter_fmt, rt_fmt, ds_fmt)) \
    X(HRESULT, GetDeviceCaps, (UINT adapter, Enum dev_type, void* caps), (adapter, dev_type, caps)) \
    X(HMONITOR, GetAdapterMonitor, (UINT adapter), (adapter)) \
    X(HRESULT, CreateDevice, (UINT adapter, Enum dev_type, HWND focus, DWORD behavior, PresentParameters* pp, IDirect3DDevice8** out), (adapter, dev_type, focus, behavior, pp, out))

#define JOAN_IDIRECT3DDEVICE8_METHODS(X) \
    X(HRESULT, QueryInterface, (const Guid& riid, void** out), (riid, out)) \
    X(ULONG, AddRef, (), ()) \
    X(ULONG, Release, (), ()) \
    X(HRESULT, TestCooperativeLevel, (), ()) \
    X(UINT, GetAvailableTextureMem, (), ()) \
    X(HRESULT, ResourceManagerDiscardBytes, (DWORD bytes), (bytes)) \
    X(HRESULT, GetDirect3D, (void** d3d), (d3d)) \
    X(HRESULT, GetDeviceCaps, (void* caps), (caps)) \
    X(HRESULT, GetDisplayMode, (void* mode), (mode)) \
    X(HRESULT, GetCreationParameters, (void* params), (params)) \
    X(HRESULT, SetCursorProperties, (UINT x_hot, UINT y_hot, IDirect3DSurface8* bitmap), (x_hot, y_hot, bitmap)) \
    X(void, SetCursorPosition, (int x, int y, DWORD flags), (x, y, flags)) \
    X(BOOL, ShowCursor, (BOOL show), (show)) \
    X(HRESULT, CreateAdditionalSwapChain, (PresentParameters* pp, IDirect3DSwapChain8** out), (pp, out)) \
    X(HRESULT, Reset, (PresentParameters* pp), (pp)) \
    X(HRESULT, Present, (const void* src_rect, const void* dst_rect, HWND dst_window, const void* dirty), (src_rect, dst_rect, dst_window, dirty)) \
    X(HRESULT, GetBackBuffer, (UINT index, Enum type, IDirect3DSurface8** out), (index, type, out)) \
    X(HRESULT, GetRasterStatus, (void* status), (status)) \
    X(void, SetGammaRamp, (DWORD flags, const void* ramp), (flags, ramp)) \
    X(void, GetGammaRamp, (void* ramp), (ramp)) \
    X(HRESULT, CreateTexture, (UINT w, UINT h, UINT levels, DWORD usage, Enum format, Enum pool, IDirect3DTexture8** out), (w, h, levels, usage, format, pool, out)) \
    X(HRESULT, CreateVolumeTexture, (UINT w, UINT h, UINT d, UINT levels, DWORD usage, Enum format, Enum pool, IDirect3DVolumeTexture8** out), (w, h, d, levels, usage, format, pool, out)) \
    X(HRESULT, CreateCubeTexture, (UINT edge, UINT levels, DWORD usage, Enum format, Enum pool, IDirect3DCubeTexture8** out), (edge, levels, usage, format, pool, out)) \
    X(HRESULT, CreateVertexBuffer, (UINT length, DWORD usage, DWORD fvf, Enum pool, IDirect3DVertexBuffer8** out), (length, usage, fvf, pool, out)) \
    X(HRESULT, CreateIndexBuffer, (UINT length, DWORD usage, Enum format, Enum pool, IDirect3DIndexBuffer8** out), (length, usage, format, pool, out)) \
    X(HRESULT, CreateRenderTarget, (UINT w, UINT h, Enum format, Enum ms_type, BOOL lockable, IDirect3DSurface8** out), (w, h, format, ms_type, lockable, out)) \
    X(HRESULT, CreateDepthStencilSurface, (UINT w, UINT h, Enum format, Enum ms_type, IDirect3DSurface8** out), (w, h, format, ms_type, out)) \
    X(HRESULT, CreateImageSurface, (UINT w, UINT h, Enum format, IDirect3DSurface8** out), (w, h, format, out)) \
    X(HRESULT, CopyRects, (IDirect3DSurface8* src, const void* src_rects, UINT count, IDirect3DSurface8* dst, const void* dst_points), (src, src_rects, count, dst, dst_points)) \
    X(HRESULT, UpdateTexture, (IDirect3DBaseTexture8* src, IDirect3DBaseTexture8* dst), (src, dst)) \
    X(HRESULT, GetFrontBuffer, (IDirect3DSurface8* dst), (dst)) \
    X(HRESULT, SetRenderTarget, (IDirect3DSurface8* rt, IDirect3DSurface8* ds), (rt, ds)) \
    X(HRESULT, GetRenderTarget, (IDirect3DSurface8** out), (out)) \
    X(HRESULT, GetDepthStencilSurface, (IDirect3DSurface8** out), (out)) \
    X(HRESULT, BeginScene, (), ()) \
    X(HRESULT, EndScene, (), ()) \
    X(HRESULT, Clear, (DWORD count, const void* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil), (count, rects, flags, color, z, stencil)) \
    X(HRESULT, SetTransform, (Enum state, const float* matrix), (state, matrix)) \
    X(HRESULT, GetTransform, (Enum state, float* matrix), (state, matrix)) \
    X(HRESULT, MultiplyTransform, (Enum state, const float* matrix), (state, matrix)) \
    X(HRESULT, SetViewport, (const void* vp), (vp)) \
    X(HRESULT, GetViewport, (void* vp), (vp)) \
    X(HRESULT, SetMaterial, (const void* mat), (mat)) \
    X(HRESULT, GetMaterial, (void* mat), (mat)) \
    X(HRESULT, SetLight, (DWORD index, const void* light), (index, light)) \
    X(HRESULT, GetLight, (DWORD index, void* light), (index, light)) \
    X(HRESULT, LightEnable, (DWORD index, BOOL enable), (index, enable)) \
    X(HRESULT, GetLightEnable, (DWORD index, BOOL* enable), (index, enable)) \
    X(HRESULT, SetClipPlane, (DWORD index, const float* plane), (index, plane)) \
    X(HRESULT, GetClipPlane, (DWORD index, float* plane), (index, plane)) \
    X(HRESULT, SetRenderState, (Enum state, DWORD value), (state, value)) \
    X(HRESULT, GetRenderState, (Enum state, DWORD* value), (state, value)) \
    X(HRESULT, BeginStateBlock, (), ()) \
    X(HRESULT, EndStateBlock, (DWORD* token), (token)) \
    X(HRESULT, ApplyStateBlock, (DWORD token), (token)) \
    X(HRESULT, CaptureStateBlock, (DWORD token), (token)) \
    X(HRESULT, DeleteStateBlock, (DWORD token), (token)) \
    X(HRESULT, CreateStateBlock, (Enum type, DWORD* token), (type, token)) \
    X(HRESULT, SetClipStatus, (const void* status), (status)) \
    X(HRESULT, GetClipStatus, (void* status), (status)) \
    X(HRESULT, GetTexture, (DWORD stage, IDirect3DBaseTexture8** out), (stage, out)) \
    X(HRESULT, SetTexture, (DWORD stage, IDirect3DBaseTexture8* tex), (stage, tex)) \
    X(HRESULT, GetTextureStageState, (DWORD stage, Enum type, DWORD* value), (stage, type, value)) \
    X(HRESULT, SetTextureStageState, (DWORD stage, Enum type, DWORD value), (stage, type, value)) \
    X(HRESULT, ValidateDevice, (DWORD* passes), (passes)) \
    X(HRESULT, GetInfo, (DWORD id, void* info, DWORD size), (id, info, size)) \
    X(HRESULT, SetPaletteEntries, (UINT palette, const void* entries), (palette, entries)) \
    X(HRESULT, GetPaletteEntries, (UINT palette, void* entries), (palette, entries)) \
    X(HRESULT, SetCurrentTexturePalette, (UINT palette), (palette)) \
    X(HRESULT, GetCurrentTexturePalette, (UINT* palette), (palette)) \
    X(HRESULT, DrawPrimitive, (Enum prim, UINT start_vertex, UINT prim_count), (prim, start_vertex, prim_count)) \
    X(HRESULT, DrawIndexedPrimitive, (Enum prim, UINT min_index, UINT num_vertices, UINT start_index, UINT prim_count), (prim, min_index, num_vertices, start_index, prim_count)) \
    X(HRESULT, DrawPrimitiveUP, (Enum prim, UINT prim_count, const void* data, UINT stride), (prim, prim_count, data, stride)) \
    X(HRESULT, DrawIndexedPrimitiveUP, (Enum prim, UINT min_index, UINT num_vertices, UINT prim_count, const void* indices, Enum index_format, const void* data, UINT stride), (prim, min_index, num_vertices, prim_count, indices, index_format, data, stride)) \
    X(HRESULT, ProcessVertices, (UINT src_start, UINT dst_index, UINT count, IDirect3DVertexBuffer8* dst, DWORD flags), (src_start, dst_index, count, dst, flags)) \
    X(HRESULT, CreateVertexShader, (const DWORD* decl, const DWORD* function, DWORD* handle, DWORD usage), (decl, function, handle, usage)) \
    X(HRESULT, SetVertexShader, (DWORD handle), (handle)) \
    X(HRESULT, GetVertexShader, (DWORD* handle), (handle)) \
    X(HRESULT, DeleteVertexShader, (DWORD handle), (handle)) \
    X(HRESULT, SetVertexShaderConstant, (DWORD reg, const void* data, DWORD count), (reg, data, count)) \
    X(HRESULT, GetVertexShaderConstant, (DWORD reg, void* data, DWORD count), (reg, data, count)) \
    X(HRESULT, GetVertexShaderDeclaration, (DWORD handle, void* data, DWORD* size), (handle, data, size)) \
    X(HRESULT, GetVertexShaderFunction, (DWORD handle, void* data, DWORD* size), (handle, data, size)) \
    X(HRESULT, SetStreamSource, (UINT stream, IDirect3DVertexBuffer8* vb, UINT stride), (stream, vb, stride)) \
    X(HRESULT, GetStreamSource, (UINT stream, IDirect3DVertexBuffer8** vb, UINT* stride), (stream, vb, stride)) \
    X(HRESULT, SetIndices, (IDirect3DIndexBuffer8* ib, UINT base_vertex), (ib, base_vertex)) \
    X(HRESULT, GetIndices, (IDirect3DIndexBuffer8** ib, UINT* base_vertex), (ib, base_vertex)) \
    X(HRESULT, CreatePixelShader, (const DWORD* function, DWORD* handle), (function, handle)) \
    X(HRESULT, SetPixelShader, (DWORD handle), (handle)) \
    X(HRESULT, GetPixelShader, (DWORD* handle), (handle)) \
    X(HRESULT, DeletePixelShader, (DWORD handle), (handle)) \
    X(HRESULT, SetPixelShaderConstant, (DWORD reg, const void* data, DWORD count), (reg, data, count)) \
    X(HRESULT, GetPixelShaderConstant, (DWORD reg, void* data, DWORD count), (reg, data, count)) \
    X(HRESULT, GetPixelShaderFunction, (DWORD handle, void* data, DWORD* size), (handle, data, size)) \
    X(HRESULT, DrawRectPatch, (UINT handle, const float* segs, const void* info), (handle, segs, info)) \
    X(HRESULT, DrawTriPatch, (UINT handle, const float* segs, const void* info), (handle, segs, info)) \
    X(HRESULT, DeletePatch, (UINT handle), (handle))
// clang-format on

#define JOAN_D3D8_DECLARE(ret, name, params, args) virtual ret JOAN_STDCALL name params = 0;

struct IDirect3D8 {
    JOAN_IDIRECT3D8_METHODS(JOAN_D3D8_DECLARE)
};

struct IDirect3DDevice8 {
    JOAN_IDIRECT3DDEVICE8_METHODS(JOAN_D3D8_DECLARE)
};

#undef JOAN_D3D8_DECLARE

// Method indices (vtable slot = index * 4), for tables and logging.
#define JOAN_D3D8_ENUM(ret, name, params, args) name,
enum class D3DMethod : int { JOAN_IDIRECT3D8_METHODS(JOAN_D3D8_ENUM) Count };
enum class DeviceMethod : int { JOAN_IDIRECT3DDEVICE8_METHODS(JOAN_D3D8_ENUM) Count };
#undef JOAN_D3D8_ENUM
static_assert(static_cast<int>(D3DMethod::Count) == 16);
static_assert(static_cast<int>(DeviceMethod::Count) == 97);
static_assert(static_cast<int>(DeviceMethod::SetRenderState) * 4 == 0xc8);
static_assert(static_cast<int>(DeviceMethod::SetTextureStageState) * 4 == 0xfc);
static_assert(static_cast<int>(DeviceMethod::DrawIndexedPrimitive) * 4 == 0x11c);
static_assert(static_cast<int>(DeviceMethod::SetVertexShaderConstant) * 4 == 0x13c);
static_assert(static_cast<int>(DeviceMethod::SetStreamSource) * 4 == 0x14c);

// Selected D3D8 constants.
enum : Enum {
    kPrimPointList = 1, kPrimLineList, kPrimLineStrip, kPrimTriangleList, kPrimTriangleStrip, kPrimTriangleFan,
};

}  // namespace joan::d3d8
