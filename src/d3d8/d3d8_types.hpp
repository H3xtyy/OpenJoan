// Direct3D 8 structures, constants and the resource interfaces (textures,
// surfaces, vertex / index buffers) needed to implement a device ourselves.
// Layouts follow d3d8.h / d3d8types.h / d3d8caps.h of the DirectX 8.1 SDK.
#pragma once
#include "../callconv.hpp"

#include <cstdint>

#include "d3d8_api.hpp"

namespace joan::d3d8 {

using LONG = std::int32_t;
using WORD = std::uint16_t;
using BYTE = std::uint8_t;

// --- structures --------------------------------------------------------------
struct Rect {  // RECT
    LONG left, top, right, bottom;
};
struct Point {  // POINT
    LONG x, y;
};
struct D3DRect {  // D3DRECT
    LONG x1, y1, x2, y2;
};
struct SurfaceDesc {  // D3DSURFACE_DESC
    Enum format;
    Enum type;
    DWORD usage;
    Enum pool;
    UINT size;
    Enum multi_sample;
    UINT width, height;
};
struct LockedRect {  // D3DLOCKED_RECT
    int pitch;
    void* bits;
};
struct VertexBufferDesc {  // D3DVERTEXBUFFER_DESC
    Enum format;
    Enum type;
    DWORD usage;
    Enum pool;
    UINT size;
    DWORD fvf;
};
struct IndexBufferDesc {  // D3DINDEXBUFFER_DESC
    Enum format;
    Enum type;
    DWORD usage;
    Enum pool;
    UINT size;
};
struct Viewport {  // D3DVIEWPORT8
    DWORD x, y, width, height;
    float min_z, max_z;
};
struct ColorValue {  // D3DCOLORVALUE
    float r, g, b, a;
};
struct Material {  // D3DMATERIAL8
    ColorValue diffuse, ambient, specular, emissive;
    float power;
};
struct Vector3 {  // D3DVECTOR
    float x, y, z;
};
struct Light {  // D3DLIGHT8
    Enum type;  // 1 point, 2 spot, 3 directional
    ColorValue diffuse, specular, ambient;
    Vector3 position, direction;
    float range, falloff, attenuation0, attenuation1, attenuation2, theta, phi;
};
struct DisplayMode {  // D3DDISPLAYMODE
    UINT width, height, refresh_rate;
    Enum format;
};
struct CreationParameters {  // D3DDEVICE_CREATION_PARAMETERS
    UINT adapter;
    Enum device_type;
    HWND focus_window;
    DWORD behavior;
};
struct GammaRamp {  // D3DGAMMARAMP
    WORD red[256], green[256], blue[256];
};
struct RasterStatus {  // D3DRASTER_STATUS
    BOOL in_vblank;
    UINT scan_line;
};

struct Caps8 {  // D3DCAPS8
    Enum device_type;
    UINT adapter_ordinal;
    DWORD caps, caps2, caps3, presentation_intervals, cursor_caps, dev_caps;
    DWORD primitive_misc_caps, raster_caps, z_cmp_caps, src_blend_caps, dest_blend_caps, alpha_cmp_caps, shade_caps;
    DWORD texture_caps, texture_filter_caps, cube_texture_filter_caps, volume_texture_filter_caps;
    DWORD texture_address_caps, volume_texture_address_caps, line_caps;
    DWORD max_texture_width, max_texture_height, max_volume_extent, max_texture_repeat, max_texture_aspect_ratio,
        max_anisotropy;
    float max_vertex_w, guard_band_left, guard_band_top, guard_band_right, guard_band_bottom, extents_adjust;
    DWORD stencil_caps, fvf_caps, texture_op_caps, max_texture_blend_stages, max_simultaneous_textures;
    DWORD vertex_processing_caps, max_active_lights, max_user_clip_planes, max_vertex_blend_matrices,
        max_vertex_blend_matrix_index;
    float max_point_size;
    DWORD max_primitive_count, max_vertex_index, max_streams, max_stream_stride;
    DWORD vertex_shader_version, max_vertex_shader_const, pixel_shader_version;
    float max_pixel_shader_value;
};
static_assert(sizeof(Caps8) == 0xd4);

// --- constants -------------------------------------------------------------------
inline constexpr HRESULT kInvalidCall = static_cast<HRESULT>(0x8876086c);  // D3DERR_INVALIDCALL
inline constexpr HRESULT kNotAvailable = static_cast<HRESULT>(0x8876086a);  // D3DERR_NOTAVAILABLE
inline constexpr HRESULT kNotFound = static_cast<HRESULT>(0x88760866);      // D3DERR_NOTFOUND
inline constexpr HRESULT kOutOfVideoMemory = static_cast<HRESULT>(0x8876017c);

enum Format : Enum {
    kFmtUnknown = 0, kFmtR8G8B8 = 20, kFmtA8R8G8B8 = 21, kFmtX8R8G8B8 = 22, kFmtR5G6B5 = 23, kFmtX1R5G5B5 = 24,
    kFmtA1R5G5B5 = 25, kFmtA4R4G4B4 = 26, kFmtA8 = 28, kFmtX4R4G4B4 = 30, kFmtL8 = 50, kFmtA8L8 = 51, kFmtA4L4 = 52,
    kFmtV8U8 = 60, kFmtD16Lockable = 70, kFmtD32 = 71, kFmtD15S1 = 73, kFmtD24S8 = 75, kFmtD24X8 = 77,
    kFmtD24X4S4 = 79, kFmtD16 = 80, kFmtVertexData = 100, kFmtIndex16 = 101, kFmtIndex32 = 102,
    kFmtDXT1 = 0x31545844, kFmtDXT2 = 0x32545844, kFmtDXT3 = 0x33545844, kFmtDXT4 = 0x34545844, kFmtDXT5 = 0x35545844,
};
enum ResourceType : Enum { kRtSurface = 1, kRtVolume = 2, kRtTexture = 3, kRtVolumeTexture = 4, kRtCubeTexture = 5,
                           kRtVertexBuffer = 6, kRtIndexBuffer = 7 };
enum Pool : Enum { kPoolDefault = 0, kPoolManaged = 1, kPoolSystemMem = 2, kPoolScratch = 3 };
enum Usage : DWORD { kUsageRenderTarget = 1, kUsageDepthStencil = 2, kUsageWriteOnly = 8, kUsageDynamic = 0x200 };
enum LockFlags : DWORD { kLockReadOnly = 0x10, kLockNoOverwrite = 0x1000, kLockDiscard = 0x2000 };
enum ClearFlags : DWORD { kClearTarget = 1, kClearZBuffer = 2, kClearStencil = 4 };
enum FvfBits : DWORD {
    kFvfPositionMask = 0x00e, kFvfXyz = 0x002, kFvfXyzRhw = 0x004, kFvfXyzB1 = 0x006, kFvfXyzB2 = 0x008,
    kFvfXyzB3 = 0x00a, kFvfXyzB4 = 0x00c, kFvfXyzB5 = 0x00e, kFvfNormal = 0x010, kFvfPSize = 0x020,
    kFvfDiffuse = 0x040, kFvfSpecular = 0x080, kFvfTexCountMask = 0xf00, kFvfTexCountShift = 8,
    kFvfLastBetaUByte4 = 0x1000,
};
enum TransformState : Enum { kTsView = 2, kTsProjection = 3, kTsTexture0 = 16, kTsWorld = 256 };

enum RenderState : Enum {
    kRsZEnable = 7, kRsFillMode = 8, kRsShadeMode = 9, kRsZWriteEnable = 14, kRsAlphaTestEnable = 15,
    kRsLastPixel = 16, kRsSrcBlend = 19, kRsDestBlend = 20, kRsCullMode = 22, kRsZFunc = 23, kRsAlphaRef = 24,
    kRsAlphaFunc = 25, kRsDitherEnable = 26, kRsAlphaBlendEnable = 27, kRsFogEnable = 28, kRsSpecularEnable = 29,
    kRsFogColor = 34, kRsFogTableMode = 35, kRsFogStart = 36, kRsFogEnd = 37, kRsFogDensity = 38, kRsZBias = 47,
    kRsRangeFogEnable = 48, kRsStencilEnable = 52, kRsStencilFail = 53, kRsStencilZFail = 54, kRsStencilPass = 55,
    kRsStencilFunc = 56, kRsStencilRef = 57, kRsStencilMask = 58, kRsStencilWriteMask = 59, kRsTextureFactor = 60,
    kRsClipping = 136, kRsLighting = 137, kRsAmbient = 139, kRsFogVertexMode = 140, kRsColorVertex = 141,
    kRsLocalViewer = 142, kRsNormalizeNormals = 143, kRsDiffuseMaterialSource = 145,
    kRsSpecularMaterialSource = 146, kRsAmbientMaterialSource = 147, kRsEmissiveMaterialSource = 148,
    kRsVertexBlend = 151, kRsClipPlaneEnable = 152, kRsPointSize = 154, kRsPointSizeMin = 155,
    kRsPointSpriteEnable = 156, kRsPointScaleEnable = 157, kRsMultiSampleAntiAlias = 161, kRsMultiSampleMask = 162,
    kRsPointSizeMax = 166, kRsIndexedVertexBlendEnable = 167, kRsColorWriteEnable = 168, kRsTweenFactor = 170,
    kRsBlendOp = 171, kRsCount = 176,
};
enum StageState : Enum {
    kTssColorOp = 1, kTssColorArg1 = 2, kTssColorArg2 = 3, kTssAlphaOp = 4, kTssAlphaArg1 = 5, kTssAlphaArg2 = 6,
    kTssBumpEnvMat00 = 7, kTssBumpEnvMat01 = 8, kTssBumpEnvMat10 = 9, kTssBumpEnvMat11 = 10,
    kTssTexCoordIndex = 11, kTssAddressU = 13, kTssAddressV = 14, kTssBorderColor = 15, kTssMagFilter = 16,
    kTssMinFilter = 17, kTssMipFilter = 18, kTssMipMapLodBias = 19, kTssMaxMipLevel = 20, kTssMaxAnisotropy = 21,
    kTssBumpEnvLScale = 22, kTssBumpEnvLOffset = 23, kTssTextureTransformFlags = 24, kTssAddressW = 25,
    kTssColorArg0 = 26, kTssAlphaArg0 = 27, kTssResultArg = 28, kTssCount = 29,
};
// D3DTEXTUREOP
enum TextureOp : DWORD {
    kTopDisable = 1, kTopSelectArg1 = 2, kTopSelectArg2 = 3, kTopModulate = 4, kTopModulate2x = 5, kTopModulate4x = 6,
    kTopAdd = 7, kTopAddSigned = 8, kTopAddSigned2x = 9, kTopSubtract = 10, kTopAddSmooth = 11,
    kTopBlendDiffuseAlpha = 12, kTopBlendTextureAlpha = 13, kTopBlendFactorAlpha = 14, kTopBlendTextureAlphaPm = 15,
    kTopBlendCurrentAlpha = 16, kTopPremodulate = 17, kTopModulateAlphaAddColor = 18, kTopModulateColorAddAlpha = 19,
    kTopModulateInvAlphaAddColor = 20, kTopModulateInvColorAddAlpha = 21, kTopBumpEnvMap = 22,
    kTopBumpEnvMapLuminance = 23, kTopDotProduct3 = 24, kTopMultiplyAdd = 25, kTopLerp = 26,
};
// D3DTA_* texture arguments
enum TextureArg : DWORD {
    kTaDiffuse = 0, kTaCurrent = 1, kTaTexture = 2, kTaTFactor = 3, kTaSpecular = 4, kTaTemp = 5,
    kTaComplement = 0x10, kTaAlphaReplicate = 0x20,
};
// D3DBLEND
enum Blend : DWORD {
    kBlendZero = 1, kBlendOne = 2, kBlendSrcColor = 3, kBlendInvSrcColor = 4, kBlendSrcAlpha = 5,
    kBlendInvSrcAlpha = 6, kBlendDestAlpha = 7, kBlendInvDestAlpha = 8, kBlendDestColor = 9, kBlendInvDestColor = 10,
    kBlendSrcAlphaSat = 11,
};
enum TextureAddress : DWORD { kTaddressWrap = 1, kTaddressMirror = 2, kTaddressClamp = 3, kTaddressBorder = 4 };
enum TextureFilter : DWORD { kTexfNone = 0, kTexfPoint = 1, kTexfLinear = 2 };
enum CullMode : DWORD { kCullNone = 1, kCullCw = 2, kCullCcw = 3 };
// D3DTEXTURETRANSFORMFLAGS
enum TextureTransformFlags : DWORD { kTtffDisable = 0, kTtffCount2 = 2, kTtffCount3 = 3, kTtffProjected = 0x100 };
// D3DTSS_TCI_* (or-ed into TEXCOORDINDEX)
enum TexCoordGen : DWORD {
    kTciPassThru = 0, kTciCameraSpaceNormal = 0x10000, kTciCameraSpacePosition = 0x20000,
    kTciCameraSpaceReflection = 0x30000,
};

// --- resource interfaces ------------------------------------------------------
// The texture / buffer methods are listed in vtable order; IDirect3DBaseTexture8
// is the prefix of IDirect3DTexture8, so a texture is passed to SetTexture as is.
struct IDirect3DSurface8 {
    virtual HRESULT JOAN_STDCALL QueryInterface(const Guid& riid, void** out) = 0;
    virtual ULONG JOAN_STDCALL AddRef() = 0;
    virtual ULONG JOAN_STDCALL Release() = 0;
    virtual HRESULT JOAN_STDCALL GetDevice(IDirect3DDevice8** out) = 0;
    virtual HRESULT JOAN_STDCALL SetPrivateData(const Guid& guid, const void* data, DWORD size, DWORD flags) = 0;
    virtual HRESULT JOAN_STDCALL GetPrivateData(const Guid& guid, void* data, DWORD* size) = 0;
    virtual HRESULT JOAN_STDCALL FreePrivateData(const Guid& guid) = 0;
    virtual HRESULT JOAN_STDCALL GetContainer(const Guid& riid, void** out) = 0;
    virtual HRESULT JOAN_STDCALL GetDesc(SurfaceDesc* desc) = 0;
    virtual HRESULT JOAN_STDCALL LockRect(LockedRect* locked, const Rect* rect, DWORD flags) = 0;
    virtual HRESULT JOAN_STDCALL UnlockRect() = 0;
};

struct IDirect3DBaseTexture8 {
    virtual HRESULT JOAN_STDCALL QueryInterface(const Guid& riid, void** out) = 0;
    virtual ULONG JOAN_STDCALL AddRef() = 0;
    virtual ULONG JOAN_STDCALL Release() = 0;
    virtual HRESULT JOAN_STDCALL GetDevice(IDirect3DDevice8** out) = 0;
    virtual HRESULT JOAN_STDCALL SetPrivateData(const Guid& guid, const void* data, DWORD size, DWORD flags) = 0;
    virtual HRESULT JOAN_STDCALL GetPrivateData(const Guid& guid, void* data, DWORD* size) = 0;
    virtual HRESULT JOAN_STDCALL FreePrivateData(const Guid& guid) = 0;
    virtual DWORD JOAN_STDCALL SetPriority(DWORD priority) = 0;
    virtual DWORD JOAN_STDCALL GetPriority() = 0;
    virtual void JOAN_STDCALL PreLoad() = 0;
    virtual Enum JOAN_STDCALL GetType() = 0;
    virtual DWORD JOAN_STDCALL SetLOD(DWORD lod) = 0;
    virtual DWORD JOAN_STDCALL GetLOD() = 0;
    virtual DWORD JOAN_STDCALL GetLevelCount() = 0;
};

struct IDirect3DTexture8 : IDirect3DBaseTexture8 {
    virtual HRESULT JOAN_STDCALL GetLevelDesc(UINT level, SurfaceDesc* desc) = 0;
    virtual HRESULT JOAN_STDCALL GetSurfaceLevel(UINT level, IDirect3DSurface8** out) = 0;
    virtual HRESULT JOAN_STDCALL LockRect(UINT level, LockedRect* locked, const Rect* rect, DWORD flags) = 0;
    virtual HRESULT JOAN_STDCALL UnlockRect(UINT level) = 0;
    virtual HRESULT JOAN_STDCALL AddDirtyRect(const Rect* rect) = 0;
};

struct IDirect3DVertexBuffer8 {
    virtual HRESULT JOAN_STDCALL QueryInterface(const Guid& riid, void** out) = 0;
    virtual ULONG JOAN_STDCALL AddRef() = 0;
    virtual ULONG JOAN_STDCALL Release() = 0;
    virtual HRESULT JOAN_STDCALL GetDevice(IDirect3DDevice8** out) = 0;
    virtual HRESULT JOAN_STDCALL SetPrivateData(const Guid& guid, const void* data, DWORD size, DWORD flags) = 0;
    virtual HRESULT JOAN_STDCALL GetPrivateData(const Guid& guid, void* data, DWORD* size) = 0;
    virtual HRESULT JOAN_STDCALL FreePrivateData(const Guid& guid) = 0;
    virtual DWORD JOAN_STDCALL SetPriority(DWORD priority) = 0;
    virtual DWORD JOAN_STDCALL GetPriority() = 0;
    virtual void JOAN_STDCALL PreLoad() = 0;
    virtual Enum JOAN_STDCALL GetType() = 0;
    virtual HRESULT JOAN_STDCALL Lock(UINT offset, UINT size, BYTE** data, DWORD flags) = 0;
    virtual HRESULT JOAN_STDCALL Unlock() = 0;
    virtual HRESULT JOAN_STDCALL GetDesc(VertexBufferDesc* desc) = 0;
};

struct IDirect3DIndexBuffer8 {
    virtual HRESULT JOAN_STDCALL QueryInterface(const Guid& riid, void** out) = 0;
    virtual ULONG JOAN_STDCALL AddRef() = 0;
    virtual ULONG JOAN_STDCALL Release() = 0;
    virtual HRESULT JOAN_STDCALL GetDevice(IDirect3DDevice8** out) = 0;
    virtual HRESULT JOAN_STDCALL SetPrivateData(const Guid& guid, const void* data, DWORD size, DWORD flags) = 0;
    virtual HRESULT JOAN_STDCALL GetPrivateData(const Guid& guid, void* data, DWORD* size) = 0;
    virtual HRESULT JOAN_STDCALL FreePrivateData(const Guid& guid) = 0;
    virtual DWORD JOAN_STDCALL SetPriority(DWORD priority) = 0;
    virtual DWORD JOAN_STDCALL GetPriority() = 0;
    virtual void JOAN_STDCALL PreLoad() = 0;
    virtual Enum JOAN_STDCALL GetType() = 0;
    virtual HRESULT JOAN_STDCALL Lock(UINT offset, UINT size, BYTE** data, DWORD flags) = 0;
    virtual HRESULT JOAN_STDCALL Unlock() = 0;
    virtual HRESULT JOAN_STDCALL GetDesc(IndexBufferDesc* desc) = 0;
};

}  // namespace joan::d3d8
