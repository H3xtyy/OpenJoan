// The game's Direct3D 8 device implemented on OpenGL 4.6 ([Graphics]
// Renderer=opengl). Scope: what docs/D3D8_USAGE.md lists; anything else logs
// once and fails with D3DERR_INVALIDCALL.
//
// Rendering goes into an off-screen framebuffer the size of the Direct3D back
// buffer; Present scales it into the window (letterboxed) and swaps. Images
// are stored top row first (glClipControl UPPER_LEFT), like Direct3D's.
//
// COM objects keep their own reference counts; texture surfaces share their
// texture's count (Direct3D semantics). The device holds references on bound
// textures, buffers and render targets, as Direct3D 8 does.
#pragma once
#include "../callconv.hpp"

#include "../platform/win32_types.hpp"

#include <atomic>
#include <chrono>
#include "../compiler.hpp"
#include <cstdint>
#include <map>
#include <memory>
#include <type_traits>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../d3d8/d3d8_api.hpp"
#include "../d3d8/d3d8_types.hpp"
#include "../render/ao_pass.hpp"
#include "../render/baked_shadows.hpp"
#include "../render/shadow_volumes.hpp"
#include "../render/shadow_map.hpp"
#include "../render/water.hpp"
#include "../render/grass_batch.hpp"
#include "../render/pixel_lighting.hpp"
#include "../registry.hpp"
#include "../render/renderer.hpp"
#include "context.hpp"
#include "formats.hpp"
#include "shaders.hpp"

namespace joan::gld {

using namespace joan::d3d8;

class Device;

/// Reference counting, QueryInterface and private data shared by all objects.
template <class Interface>
class ComObject : public Interface {
   public:
    virtual ~ComObject() = default;
    /// IUnknown::QueryInterface: returns this object for any interface id, with a reference added; kInvalidCall when
    /// `out` is null.
    HRESULT JOAN_STDCALL QueryInterface(const Guid&, void** out) override {
        if (!out) return kInvalidCall;
        *out = this;
        this->AddRef();
        return kOk;
    }
    /// Adds a reference and returns the new count.
    ULONG JOAN_STDCALL AddRef() override { return ++refs_; }
    /// Drops a reference and returns the new count; deletes the object at zero.
    ULONG JOAN_STDCALL Release() override {
        const ULONG n = --refs_;
        if (n == 0) delete this;
        return n;
    }

   protected:
    std::atomic<ULONG> refs_{1};  ///< Reference count, starts at 1 (the creator's reference).
};

/// Per-object storage for the Direct3D 8 SetPrivateData calls, keyed by GUID bytes.
struct PrivateData {
    std::map<std::string, std::vector<char>> entries;  ///< The stored blobs, keyed by the bytes of the GUID.
    /// The map key of a GUID: its raw bytes.
    static std::string key(const Guid& g) { return std::string(reinterpret_cast<const char*>(&g), sizeof g); }
    /// Replaces the data stored under `g` with `size` bytes from `data`; always kOk.
    HRESULT set(const Guid& g, const void* data, DWORD size) {
        entries[key(g)].assign(static_cast<const char*>(data), static_cast<const char*>(data) + size);
        return kOk;
    }
    /// Copies the data stored under `g` into `data` (size/NOTFOUND handling in device.cpp).
    HRESULT get(const Guid& g, void* data, DWORD* size) const;
    /// Removes the data under `g`; kOk if there was some, kNotFound otherwise.
    HRESULT free(const Guid& g) { return entries.erase(key(g)) ? kOk : kNotFound; }
};

class Texture;

// ---------------------------------------------------------------------------
/// IDirect3DSurface8: a texture level, the back buffer, a depth-stencil buffer or a CPU-side image. Texture-level
/// surfaces share their texture's reference count (Direct3D semantics).
class Surface final : public ComObject<IDirect3DSurface8> {
   public:
    /// What a Surface is backed by: a level of a Texture, the back buffer (or a render target), a depth-stencil
    /// buffer, or a plain CPU image.
    enum class Kind { TextureLevel, BackBuffer, DepthStencil, Image };
    /// `device` is the owning device; for kind TextureLevel the caller sets `texture` and `level` afterwards.
    Surface(Device* device, Kind kind, Enum format, UINT width, UINT height);
    ~Surface() override;

    /// Texture levels forward to the texture's count; other surfaces use their own.
    ULONG JOAN_STDCALL AddRef() override;
    /// See AddRef.
    ULONG JOAN_STDCALL Release() override;
    /// The owning device, with a reference added.
    HRESULT JOAN_STDCALL GetDevice(IDirect3DDevice8** out) override;
    /// Stores application data under `g` (the flags argument is ignored).
    HRESULT JOAN_STDCALL SetPrivateData(const Guid& g, const void* d, DWORD s, DWORD) override { return data_.set(g, d, s); }
    /// Reads data stored by SetPrivateData.
    HRESULT JOAN_STDCALL GetPrivateData(const Guid& g, void* d, DWORD* s) override { return data_.get(g, d, s); }
    /// Removes the data stored under `g`.
    HRESULT JOAN_STDCALL FreePrivateData(const Guid& g) override { return data_.free(g); }
    /// The containing texture (a TextureLevel surface only).
    HRESULT JOAN_STDCALL GetContainer(const Guid& riid, void** out) override;
    /// Fills `desc` from format, size and kind.
    HRESULT JOAN_STDCALL GetDesc(SurfaceDesc* desc) override;
    /// Locks the surface (or `rect`) and returns a pointer to its pixels in Direct3D layout; GPU surfaces are read
    /// back first.
    HRESULT JOAN_STDCALL LockRect(LockedRect* locked, const Rect* rect, DWORD flags) override;
    /// Ends the lock; written data is uploaded to the GPU unless the lock was read-only.
    HRESULT JOAN_STDCALL UnlockRect() override;

    Kind kind;  ///< What backs this surface.
    Enum format;  ///< Direct3D surface format (D3DFMT_*).
    UINT width /**< width in pixels */, height /**< height in pixels */;
    Texture* texture = nullptr;  ///< owning texture of a TextureLevel surface (not referenced here)
    UINT level = 0;  ///< Mip level of a TextureLevel surface.
    gl::GLuint gl_texture = 0;    ///< BackBuffer: colour texture; TextureLevel: the texture's
    gl::GLuint gl_renderbuffer = 0;  ///< DepthStencil
    // [Graphics] MSAA=N only (0 otherwise, so MSAA=0 never touches it): multisampled storage the
    // device renders into instead of gl_texture / gl_renderbuffer. The device's own back buffer gets
    // a colour renderbuffer (gl_texture then holds the resolved picture, see Device::resolve_multisample);
    // a depth-stencil surface gets one (lazily, Device::framebuffer_for) once it is paired with that
    // back buffer. Game-created render targets and texture levels stay single-sampled.
    gl::GLuint gl_msaa = 0;  ///< GL renderbuffer holding the multisampled storage (see the comment above); 0 when MSAA is off.
    std::vector<std::uint8_t> cpu;   ///< Image surfaces, and read-backs of GPU surfaces while locked

    /// Copies the surface's pixels (Direct3D layout) into `out`; false if unsupported.
    bool read_pixels(std::vector<std::uint8_t>& out);
    /// Replaces the surface's pixels with `pixels` (Direct3D layout).
    bool write_pixels(const std::uint8_t* pixels);

   private:
    Device* device_;
    PrivateData data_;
    bool locked_ = false;
    DWORD lock_flags_ = 0;
};

// ---------------------------------------------------------------------------
/// IDirect3DTexture8 as an OpenGL 2D texture with a mip chain; levels are exposed as Surface objects and locked
/// through a CPU copy.
class Texture final : public ComObject<IDirect3DTexture8> {
   public:
    /// Creates the GL texture; check valid() afterwards (false when the format is unsupported or allocation failed).
    Texture(Device* device, UINT width, UINT height, UINT levels, DWORD usage, Enum format, Enum pool);
    ~Texture() override;
    /// True when the GL texture was created.
    bool valid() const { return gl_ != 0; }

    /// The owning device, with a reference added.
    HRESULT JOAN_STDCALL GetDevice(IDirect3DDevice8** out) override;
    /// Stores application data under `g` (the flags argument is ignored).
    HRESULT JOAN_STDCALL SetPrivateData(const Guid& g, const void* d, DWORD s, DWORD) override { return data_.set(g, d, s); }
    /// Reads data stored by SetPrivateData.
    HRESULT JOAN_STDCALL GetPrivateData(const Guid& g, void* d, DWORD* s) override { return data_.get(g, d, s); }
    /// Removes the data stored under `g`.
    HRESULT JOAN_STDCALL FreePrivateData(const Guid& g) override { return data_.free(g); }
    /// Stores and returns the previous priority; no effect on resource management.
    DWORD JOAN_STDCALL SetPriority(DWORD p) override { return std::exchange(priority_, p); }
    /// The stored priority.
    DWORD JOAN_STDCALL GetPriority() override { return priority_; }
    /// No-op.
    void JOAN_STDCALL PreLoad() override {}
    /// Always D3DRTYPE_TEXTURE.
    Enum JOAN_STDCALL GetType() override { return kRtTexture; }
    /// Stores and returns the previous LOD value; no effect on sampling.
    DWORD JOAN_STDCALL SetLOD(DWORD lod) override { return std::exchange(lod_, lod); }
    /// The stored LOD value.
    DWORD JOAN_STDCALL GetLOD() override { return lod_; }
    /// Number of mip levels.
    DWORD JOAN_STDCALL GetLevelCount() override { return levels_; }
    /// Description (format, size, usage) of level `level`.
    HRESULT JOAN_STDCALL GetLevelDesc(UINT level, SurfaceDesc* desc) override;
    /// The level's Surface, with a reference added.
    HRESULT JOAN_STDCALL GetSurfaceLevel(UINT level, IDirect3DSurface8** out) override;
    /// Locks level `level` (or `rect`) and returns a pointer to its pixels in Direct3D layout, through the CPU copy
    /// of shadow().
    HRESULT JOAN_STDCALL LockRect(UINT level, LockedRect* locked, const Rect* rect, DWORD flags) override;
    /// Ends the lock of `level` and uploads it unless the lock was read-only.
    HRESULT JOAN_STDCALL UnlockRect(UINT level) override;
    /// Accepted and ignored: uploads happen at UnlockRect.
    HRESULT JOAN_STDCALL AddDirtyRect(const Rect*) override { return kOk; }

    /// The GL texture name (0 when invalid).
    gl::GLuint gl() const { return gl_; }
    /// Width of level 0 in pixels.
    UINT width() const { return width_; }
    /// Height of level 0 in pixels.
    UINT height() const { return height_; }
    /// Direct3D format (D3DFMT_*).
    Enum format() const { return format_; }
    /// How the format is stored in OpenGL.
    const FormatInfo& info() const { return info_; }
    /// True when created with the render-target usage flag.
    bool render_target() const { return (usage_ & kUsageRenderTarget) != 0; }
    /// The Surface of level `level`, created on first use; owned by the texture.
    Surface* level_surface(UINT level);
    /// Sends the level's CPU copy to the GL texture.
    void upload_level(UINT level);
    /// The level's CPU copy; `read_back` = fill it from the GPU when it has none (not needed when
    /// the caller overwrites all of it).
    std::vector<std::uint8_t>& shadow(UINT level, bool read_back = true);
    /// Done with the CPU copy: freed, unless the level is rewritten repeatedly (kept_), where it
    /// stays as the level's contents so the next lock needs no read-back from the GPU (which waits
    /// for every queued draw: a texture updated every frame, e.g. the minimap, stalled each frame).
    void release_shadow(UINT level);
    /// The level's contents from its kept CPU copy; false = none (read the GPU).
    bool kept_contents(UINT level, std::vector<std::uint8_t>& out) const;
    /// Number of mip levels.
    UINT levels() const { return levels_; }
    /// True once level `level` has been uploaded to the GPU.
    bool uploaded(UINT level) const { return level < levels_ && uploaded_[level]; }

   private:
    Device* device_;
    PrivateData data_;
    UINT width_, height_, levels_;
    DWORD usage_;
    Enum format_, pool_;
    FormatInfo info_;
    gl::GLuint gl_ = 0;
    DWORD priority_ = 0, lod_ = 0;
    std::vector<std::vector<std::uint8_t>> shadow_;
    std::vector<Surface*> surfaces_;
    std::vector<bool> locked_, uploaded_;
    std::vector<bool> kept_;  // the level's CPU copy is kept (written again after its first upload)
    std::vector<DWORD> lock_flags_;
};

// ---------------------------------------------------------------------------
/// Vertex / index buffer write paths since the last reset ([Graphics] PerfLog).
struct BufferStats {
    unsigned renames = 0 /**< moves to another copy on DISCARD or whole rewrite */, appends = 0 /**< NOOVERWRITE locks */,
             waits = 0 /**< locks that waited for the GPU */, promoted = 0 /**< static buffers that became pooled */,
             copies_made = 0 /**< new persistent copies created */, in_place = 0 /**< writes straight into the current copy */;
    unsigned streamed = 0 /**< locks served by the stream ring */, stream_waits = 0 /**< stream ring locks that waited on a fence */;
};
/// The process-wide BufferStats.
BufferStats& buffer_stats();

/// The device's stream ring: one persistently mapped buffer that the engine's dynamic write-only
/// vertex / index buffers (SysEn's per-frame streaming rings: every model's vertices are copied in
/// each frame) are written into. Every lock of such a buffer gets its own fresh slice, returned to the
/// game to write into directly; the buffer remembers where its data went and draws bind the ring
/// there. No GL call per lock: fences only when the writes move into another of kSegments parts.
struct StreamRing {
    static constexpr std::uint32_t kSize = 64u << 20 /**< ring size in bytes */, kSegments = 8 /**< fenced parts of the ring */;
    gl::GLuint buffer = 0;  ///< GL buffer name of the ring (persistently mapped); 0 before init().
    std::uint8_t* ptr = nullptr;  ///< Mapped address of the buffer; null before init().
    std::uint32_t cursor = 0;  ///< Write position in bytes.
    gl::GLsync fences[kSegments] = {};  ///< Fence of each segment, set when writes left it.
    std::uint32_t switches = 0;  ///< segment changes so far (data written at count N is intact below N + kSegments)
    /// Creates and maps the buffer; false on failure.
    bool init();
    /// Unmaps and deletes the buffer.
    void shutdown();
    /// `size` bytes at a 16-byte aligned position >= min_pos (so that position - lock offset >= 0);
    /// ~0u = no room (size larger than a segment).
    std::uint32_t alloc(std::uint32_t size, std::uint32_t min_pos);
};

/// The stream cache's buffers (game/stream_cache_api.hpp: a model's unchanged vertex array / triangle list, static
/// and written once) sub-allocated from one GL buffer: draws of different cached models keep the same vertex and
/// index buffer bound and pass where the model lies as the base vertex / index offset - no buffer binding call per
/// draw. Vertex ranges start at a multiple of their stride. Video memory only (not mapped: no address space).
/// A freed range is reused kRetireFrames frames later (the GPU is done with the draws that read it).
struct GeometryPool {
    static constexpr std::uint32_t kSize = 320u << 20 /**< pool size in bytes */, kRetireFrames = 4 /**< frames before a freed range is reused */;  // a level's ~120 MB plus the last one's until it is evicted
    gl::GLuint buffer = 0;  ///< GL buffer name of the pool; 0 before init().
    /// A byte range of the pool.
    struct Range {
        std::uint32_t at /**< start in bytes */, size /**< length in bytes */;
    };
    std::vector<Range> free;  ///< free ranges by position, adjacent ranges merged
    /// A freed range waiting to be reused.
    struct Retired {
        Range range;  ///< The freed range.
        std::uint32_t frame;  ///< Presented-frame count when it was freed.
    };
    std::vector<Retired> retired;  ///< freed ranges not yet reusable
    std::uint32_t used = 0;  ///< Bytes currently allocated.
    bool full_logged = false;  ///< Set once the pool-full message was logged, so it is logged once.
    /// Creates the buffer; false on failure.
    bool init();
    /// Deletes the buffer.
    void shutdown();
    /// `size` bytes starting at a multiple of `align`; ~0u = no room. `frame` = the presented frame count.
    std::uint32_t alloc(std::uint32_t size, std::uint32_t align, std::uint32_t frame);
    /// Gives the range back; it becomes reusable kRetireFrames frames after `frame`.
    void release(std::uint32_t at, std::uint32_t size, std::uint32_t frame);

   private:
    void give_back(Range r);
};

template <class Interface, class Desc>
/// IDirect3DVertexBuffer8 / IDirect3DIndexBuffer8 as OpenGL buffers; `Desc` is VertexBufferDesc or IndexBufferDesc.
/// Dynamic buffers are streamed through the device's StreamRing, others are renamed copies or pooled in the
/// GeometryPool.
class Buffer final : public ComObject<Interface> {
   public:
    /// `format_or_zero` is the index format (index buffers) or 0; `fvf` the vertex format (vertex buffers).
    Buffer(Device* device, UINT length, DWORD usage, Enum format_or_zero, DWORD fvf, Enum pool, Enum type);
    ~Buffer() override;

    /// The owning device, with a reference added.
    HRESULT JOAN_STDCALL GetDevice(IDirect3DDevice8** out) override;
    /// Stores application data under `g` (the flags argument is ignored).
    HRESULT JOAN_STDCALL SetPrivateData(const Guid& g, const void* d, DWORD s, DWORD) override { return data_.set(g, d, s); }
    /// Reads data stored by SetPrivateData.
    HRESULT JOAN_STDCALL GetPrivateData(const Guid& g, void* d, DWORD* s) override { return data_.get(g, d, s); }
    /// Removes the data stored under `g`.
    HRESULT JOAN_STDCALL FreePrivateData(const Guid& g) override { return data_.free(g); }
    /// Stores and returns the previous priority; no effect on resource management.
    DWORD JOAN_STDCALL SetPriority(DWORD p) override { return std::exchange(priority_, p); }
    /// The stored priority.
    DWORD JOAN_STDCALL GetPriority() override { return priority_; }
    /// No-op.
    void JOAN_STDCALL PreLoad() override {}
    /// The buffer's resource type (vertex or index buffer).
    Enum JOAN_STDCALL GetType() override { return type_; }
    /// Locks `size` bytes at `offset` (0, 0 = all) and returns a pointer to write into; write-only buffers hand out a
    /// slice of the stream ring or a persistently mapped copy.
    HRESULT JOAN_STDCALL Lock(UINT offset, UINT size, BYTE** data, DWORD flags) override;
    /// Ends the lock; data written through a scratch copy is uploaded.
    HRESULT JOAN_STDCALL Unlock() override;
    /// Fills `desc` with the buffer's format, usage, pool and size.
    HRESULT JOAN_STDCALL GetDesc(Desc* desc) override;

    /// The GL buffer name currently used (the ring, a copy or the pool's buffer).
    gl::GLuint gl() const { return gl_; }
    /// Byte offset of this buffer's offset 0 in gl() (the stream ring's slice of the last lock, its place in the
    /// geometry pool; 0 otherwise).
    gl::GLintptr offset() const { return base_; }
    /// Index format (D3DFMT_INDEX16/32), or 0 for a vertex buffer.
    Enum format() const { return format_; }
    /// A range of the device's GeometryPool (vertex buffers: offset() is a multiple of their stride).
    bool pooled() const { return pooled_; }

   private:
    bool pooled_ = false;
    // Streamed (dynamic write-only): gl_ is the device's stream ring, base_ where the last lock's slice
    // lies minus the lock's offset (draws use the data of the last lock, as the engine's do).
    bool streamed_ = false, stream_lock_ = false;
    gl::GLintptr base_ = 0;
    // Renamed buffers (D3D drivers' "buffer renaming"): a buffer the game rewrites while queued
    // draws may still read it - the dynamic streaming rings (DISCARD / NOOVERWRITE locks) and
    // meshes re-uploaded from offset 0 - is a pool of persistently mapped copies. Writes go
    // straight into the current copy's memory (no GL call, so no wait on the driver); a DISCARD or
    // whole rewrite moves to a copy the GPU is done with (fence), or adds one. gl_ = the current copy.
    struct Copy {
        gl::GLuint gl;
        std::uint8_t* ptr;
        gl::GLsync fence;  // set when the copy was left: the GPU is done with it once signalled
    };
    static constexpr std::size_t kMaxCopies = 48;
    bool make_copy(std::size_t at);
    void rename();         // move to a free copy
    void wait_current();   // a plain lock: wait until the GPU no longer reads the current copy
    std::vector<Copy> copies_;
    std::size_t current_ = 0;
    unsigned writes_ = 0;  // write locks so far (a static buffer written again becomes pooled)

    Device* device_;
    PrivateData data_;
    UINT length_;
    DWORD usage_, fvf_, priority_ = 0;
    Enum format_, pool_, type_;
    gl::GLuint gl_ = 0;
    std::vector<std::uint8_t> shadow_;   // full copy, only for buffers the game may read back
    std::vector<std::uint8_t> scratch_;  // write-only buffers: the locked range
    UINT lock_offset_ = 0, lock_size_ = 0;
    DWORD lock_flags_ = 0;
    int lock_depth_ = 0;
};
/// A vertex buffer of the device (Buffer over IDirect3DVertexBuffer8 with its descriptor).
using VertexBuffer = Buffer<IDirect3DVertexBuffer8, VertexBufferDesc>;
/// An index buffer of the device (Buffer over IDirect3DIndexBuffer8 with its descriptor).
using IndexBuffer = Buffer<IDirect3DIndexBuffer8, IndexBufferDesc>;

// ---------------------------------------------------------------------------
/// IDirect3D8 of the OpenGL renderer; CreateDevice makes our device. Adapter queries (caps, formats, modes,
/// identifier) are answered natively (gl/adapter.*: display modes + a fixed caps table); the system's IDirect3D8 is
/// never created. Every distinct query is logged once, and the answers are dumped once (adapter::dump).
/// D3DX's texture loader (and lib/d3dx8/texture_in_memory.cpp) decide texture sizes / formats from
/// these answers: see gl/adapter.hpp.
class Direct3D final : public ComObject<IDirect3D8> {
   public:
    /// Not supported. TODO(docs): return value, see direct3d.cpp.
    HRESULT JOAN_STDCALL RegisterSoftwareDevice(void* f) override;
    /// Number of display devices (adapter::count).
    UINT JOAN_STDCALL GetAdapterCount() override;
    /// Fills a D3DADAPTER_IDENTIFIER8 for the adapter (adapter::identifier).
    HRESULT JOAN_STDCALL GetAdapterIdentifier(UINT a, DWORD f, void* id) override;
    /// Number of display modes of the adapter.
    UINT JOAN_STDCALL GetAdapterModeCount(UINT a) override;
    /// Mode `m` of the adapter's list (see modes()).
    HRESULT JOAN_STDCALL EnumAdapterModes(UINT a, UINT m, void* dm) override;
    /// Current display mode of the adapter.
    HRESULT JOAN_STDCALL GetAdapterDisplayMode(UINT a, void* dm) override;
    /// Native answer (adapter::check_device_type).
    HRESULT JOAN_STDCALL CheckDeviceType(UINT a, Enum t, Enum df, Enum bf, BOOL w) override;
    /// Native answer for depth/stencil and buffer formats; texture formats come from gl/formats.hpp.
    HRESULT JOAN_STDCALL CheckDeviceFormat(UINT a, Enum t, Enum af, DWORD u, Enum rt, Enum cf) override;
    /// Multisample support check. TODO(docs): answer, see direct3d.cpp.
    HRESULT JOAN_STDCALL CheckDeviceMultiSampleType(UINT a, Enum t, Enum f, BOOL w, Enum ms) override;
    /// Native answer (adapter::check_depth_stencil_match).
    HRESULT JOAN_STDCALL CheckDepthStencilMatch(UINT a, Enum t, Enum af, Enum rf, Enum df) override;
    /// Fills D3DCAPS8: adapter::caps patched with the limits of the OpenGL device.
    HRESULT JOAN_STDCALL GetDeviceCaps(UINT a, Enum t, void* caps) override;
    /// The monitor handle of the adapter (adapter::monitor).
    HMONITOR JOAN_STDCALL GetAdapterMonitor(UINT a) override;
    /// Creates the Device on `focus` with `pp`; `*out` receives it, or an error code when its initialisation fails.
    HRESULT JOAN_STDCALL CreateDevice(UINT adapter, Enum type, HWND focus, DWORD behavior, PresentParameters* pp,
                                   IDirect3DDevice8** out) override;

   private:
    const std::vector<DisplayMode>& modes(UINT adapter);  // the adapter's modes + the [Display] size if missing
    std::map<UINT, std::vector<DisplayMode>> modes_;
};

// ---------------------------------------------------------------------------
/// Logs the first call of an unimplemented method and fails it.
void log_unimplemented(const char* name);
template <class R>
/// Logs the first call of method `name` (log_unimplemented) and returns the failure value for R: kInvalidCall for HRESULT, a value-initialised R otherwise (nothing for void).
R unimplemented(const char* name) {
    log_unimplemented(name);
    if constexpr (std::is_same_v<R, HRESULT>) return kInvalidCall;
    else if constexpr (!std::is_void_v<R>) return R{};
}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4100)  // unreferenced parameters of the stubs
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
/// The Direct3D 8 device methods the renderer does not implement: each logs once and fails (see unimplemented()).
/// Device overrides the ones the game uses.
class DeviceStubs : public IDirect3DDevice8 {
   public:
    virtual ~DeviceStubs() = default;
#define JOAN_GL_STUB(ret, name, params, args) \
    ret JOAN_STDCALL name params override { return unimplemented<ret>(#name); }
    JOAN_IDIRECT3DDEVICE8_METHODS(JOAN_GL_STUB)
#undef JOAN_GL_STUB
};
#if defined(_MSC_VER)
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

/// A 4x4 matrix as 16 floats in Direct3D's row-major order; defaults to identity.
struct Matrix {
    float m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};  ///< the 16 elements, row-major
};

/// A vertex shader created with CreateVertexShader: tokens, the parsed declaration and the translated GLSL.
struct VertexShaderInfo {
    std::vector<DWORD> declaration /**< declaration tokens as given by the game */, function /**< function (byte code) tokens */;
    std::uint32_t input_mask = 0;  ///< Bit mask of the input registers the declaration uses.
    /// One vertex declaration element (input register, data type, byte offset).
    using Element = render::VertexElement;  // reg, type, offset
    std::vector<Element> elements;  ///< Declaration elements: register, type and byte offset in the stream.
    std::vector<std::pair<DWORD, std::vector<float>>> constants;  ///< from CONSTMEM tokens / def
    std::string glsl;     ///< empty if translation failed
    std::string error;  ///< Why translation failed (empty when it worked).
};

/// The OpenGL 4.6 implementation of IDirect3DDevice8 the game renders through. All methods must be called on the
/// thread that created it. Methods that are not overridden here fail as DeviceStubs. See gl/README.md.
class Device final : public DeviceStubs {
   public:
    /// Records the creation parameters; call init() before use.
    Device(Direct3D* d3d, UINT adapter, Enum type, HWND window, DWORD behavior, const PresentParameters& pp);
    ~Device() override;
    /// Creates the GL context, back buffer and renderer objects; false with a message in `error`.
    bool init(std::string& error);
    /// VSync, the frame limiter and the texture quality options again from hook::settings() (the options menu).
    void apply_live_settings();
    /// Co-op (docs/COOP.md): while on, the back buffer and its depth are scratch surfaces of the same size, so a view of
    /// another player is drawn as usual but never shown. Off: the real ones again.
    void set_hidden_view(bool on);
    /// The ring that dynamic vertex / index locks write into.
    StreamRing& stream_ring() { return stream_ring_; }
    /// The pool holding the stream cache's static geometry.
    GeometryPool& geometry_pool() { return geometry_pool_; }
    /// Number of frames presented so far.
    std::uint32_t presented() const { return presented_; }

    // IUnknown
    /// Returns this device for any interface id, with a reference added.
    HRESULT JOAN_STDCALL QueryInterface(const Guid&, void** out) override;
    /// Adds a reference.
    ULONG JOAN_STDCALL AddRef() override { return ++refs_; }
    /// Drops a reference; the device is deleted at zero.
    ULONG JOAN_STDCALL Release() override;

    /// Always kOk: the device is never lost.
    HRESULT JOAN_STDCALL TestCooperativeLevel() override { return kOk; }
    /// Always 0x10000000 (256 MiB), a fixed answer.
    UINT JOAN_STDCALL GetAvailableTextureMem() override { return 0x10000000; }
    /// No-op, kOk.
    HRESULT JOAN_STDCALL ResourceManagerDiscardBytes(DWORD) override { return kOk; }
    /// The owning IDirect3D8, with a reference added.
    HRESULT JOAN_STDCALL GetDirect3D(void** out) override;
    /// Forwards to the IDirect3D8's caps for this adapter and device type.
    HRESULT JOAN_STDCALL GetDeviceCaps(void* caps) override;
    /// The adapter's current display mode.
    HRESULT JOAN_STDCALL GetDisplayMode(void* mode) override;
    /// Fills the creation parameters given to CreateDevice.
    HRESULT JOAN_STDCALL GetCreationParameters(void* params) override;
    /// Sets the software cursor image; the device draws it over the presented picture.
    HRESULT JOAN_STDCALL SetCursorProperties(UINT x, UINT y, IDirect3DSurface8* bitmap) override;
    /// Moves the software cursor (back-buffer pixels).
    void JOAN_STDCALL SetCursorPosition(int x, int y, DWORD flags) override;
    /// Shows or hides the software cursor; returns the previous state.
    BOOL JOAN_STDCALL ShowCursor(BOOL show) override;
    /// Re-creates the back buffer for the new present parameters.
    HRESULT JOAN_STDCALL Reset(PresentParameters* pp) override;
    /// Draws the cursor and overlays, scales the off-screen picture into the window (letterboxed) and swaps; the
    /// source / destination rectangles and window arguments are ignored.
    HRESULT JOAN_STDCALL Present(const void* src, const void* dst, HWND window, const void* dirty) override;
    /// The back buffer, with a reference added; index and type are ignored.
    HRESULT JOAN_STDCALL GetBackBuffer(UINT index, Enum type, IDirect3DSurface8** out) override;
    /// Zeroed raster status, kOk.
    HRESULT JOAN_STDCALL GetRasterStatus(void* status) override;
    /// Stores the ramp (flags ignored).
    void JOAN_STDCALL SetGammaRamp(DWORD flags, const void* ramp) override;
    /// Returns the stored ramp, or the identity ramp if none was set.
    void JOAN_STDCALL GetGammaRamp(void* ramp) override;
    /// Creates a Texture; fails with the invalid-call code when the format is unsupported.
    HRESULT JOAN_STDCALL CreateTexture(UINT w, UINT h, UINT levels, DWORD usage, Enum format, Enum pool,
                                    IDirect3DTexture8** out) override;
    /// Creates a vertex buffer of `length` bytes.
    HRESULT JOAN_STDCALL CreateVertexBuffer(UINT length, DWORD usage, DWORD fvf, Enum pool,
                                         IDirect3DVertexBuffer8** out) override;
    /// Creates an index buffer of `length` bytes.
    HRESULT JOAN_STDCALL CreateIndexBuffer(UINT length, DWORD usage, Enum format, Enum pool,
                                        IDirect3DIndexBuffer8** out) override;
    /// Creates a render-target Surface; the multisample and lockable arguments are ignored.
    HRESULT JOAN_STDCALL CreateRenderTarget(UINT w, UINT h, Enum format, Enum ms, BOOL lockable,
                                         IDirect3DSurface8** out) override;
    /// Creates a depth-stencil Surface; the multisample argument is ignored.
    HRESULT JOAN_STDCALL CreateDepthStencilSurface(UINT w, UINT h, Enum format, Enum ms, IDirect3DSurface8** out) override;
    /// Creates a CPU-side image Surface; fails for unsupported formats.
    HRESULT JOAN_STDCALL CreateImageSurface(UINT w, UINT h, Enum format, IDirect3DSurface8** out) override;
    /// Copies rectangles between surfaces of the same pixel size and compression through CPU copies; fails (logged
    /// once) between different formats.
    HRESULT JOAN_STDCALL CopyRects(IDirect3DSurface8* src, const void* src_rects, UINT count, IDirect3DSurface8* dst,
                                const void* dst_points) override;
    /// Copies the back buffer's pixels into `dst` as opaque 32-bit pixels.
    HRESULT JOAN_STDCALL GetFrontBuffer(IDirect3DSurface8* dst) override;
    /// Sets the render target and depth-stencil surface.
    HRESULT JOAN_STDCALL SetRenderTarget(IDirect3DSurface8* rt, IDirect3DSurface8* ds) override;
    /// The current render target, with a reference added.
    HRESULT JOAN_STDCALL GetRenderTarget(IDirect3DSurface8** out) override;
    /// The current depth-stencil surface, with a reference added.
    HRESULT JOAN_STDCALL GetDepthStencilSurface(IDirect3DSurface8** out) override;
    /// No-op, kOk.
    HRESULT JOAN_STDCALL BeginScene() override { return kOk; }
    /// No-op, kOk.
    HRESULT JOAN_STDCALL EndScene() override { return kOk; }
    /// Clears the render target / depth / stencil within the viewport or `rects`.
    HRESULT JOAN_STDCALL Clear(DWORD count, const void* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil) override;
    /// Stores the matrix for `state` (view, projection, texture, world).
    HRESULT JOAN_STDCALL SetTransform(Enum state, const float* matrix) override;
    /// Reads the stored matrix.
    HRESULT JOAN_STDCALL GetTransform(Enum state, float* matrix) override;
    /// Multiplies the stored matrix by `matrix`.
    HRESULT JOAN_STDCALL MultiplyTransform(Enum state, const float* matrix) override;
    /// Sets the Direct3D viewport.
    HRESULT JOAN_STDCALL SetViewport(const void* vp) override;
    /// Reads the viewport.
    HRESULT JOAN_STDCALL GetViewport(void* vp) override;
    /// Sets the material.
    HRESULT JOAN_STDCALL SetMaterial(const void* mat) override;
    /// Reads the material.
    HRESULT JOAN_STDCALL GetMaterial(void* mat) override;
    /// Stores the light `index`.
    HRESULT JOAN_STDCALL SetLight(DWORD index, const void* light) override;
    /// Reads light `index`.
    HRESULT JOAN_STDCALL GetLight(DWORD index, void* light) override;
    /// Enables or disables light `index`.
    HRESULT JOAN_STDCALL LightEnable(DWORD index, BOOL enable) override;
    /// Reads whether light `index` is enabled.
    HRESULT JOAN_STDCALL GetLightEnable(DWORD index, BOOL* enable) override;
    /// No-op, kOk: clip planes are not implemented.
    HRESULT JOAN_STDCALL SetClipPlane(DWORD, const float*) override { return kOk; }
    /// Stores a render state (D3DRS_*) and marks dependent state dirty.
    HRESULT JOAN_STDCALL SetRenderState(Enum state, DWORD value) override;
    /// Reads a render state.
    HRESULT JOAN_STDCALL GetRenderState(Enum state, DWORD* value) override;
    /// The texture bound to `stage`, with a reference added.
    HRESULT JOAN_STDCALL GetTexture(DWORD stage, IDirect3DBaseTexture8** out) override;
    /// Binds `tex` to `stage`, holding a reference.
    HRESULT JOAN_STDCALL SetTexture(DWORD stage, IDirect3DBaseTexture8* tex) override;
    /// Reads a texture stage state (D3DTSS_*).
    HRESULT JOAN_STDCALL GetTextureStageState(DWORD stage, Enum type, DWORD* value) override;
    /// Stores a texture stage state.
    HRESULT JOAN_STDCALL SetTextureStageState(DWORD stage, Enum type, DWORD value) override;
    /// Always reports one pass, kOk.
    HRESULT JOAN_STDCALL ValidateDevice(DWORD* passes) override;
    /// Draws non-indexed primitives from the bound stream; kOk without drawing when `prim_count` is 0 or no program
    /// can be chosen.
    HRESULT JOAN_STDCALL DrawPrimitive(Enum prim, UINT start_vertex, UINT prim_count) override;
    /// Draws indexed primitives using the bound indices; kOk without drawing when there are no indices. `min_index`
    /// and `num_vertices` are ignored.
    HRESULT JOAN_STDCALL DrawIndexedPrimitive(Enum prim, UINT min_index, UINT num_vertices, UINT start_index,
                                           UINT prim_count) override;
    /// Parses the declaration and translates vs.1.x byte code to GLSL (gld::translate_vertex_shader); `*handle`
    /// receives an odd handle.
    HRESULT JOAN_STDCALL CreateVertexShader(const DWORD* decl, const DWORD* function, DWORD* handle, DWORD usage) override;
    /// Selects an FVF code or a shader handle.
    HRESULT JOAN_STDCALL SetVertexShader(DWORD handle) override;
    /// The selected FVF code or shader handle.
    HRESULT JOAN_STDCALL GetVertexShader(DWORD* handle) override;
    /// Deletes the shader.
    HRESULT JOAN_STDCALL DeleteVertexShader(DWORD handle) override;
    /// Sets `count` vec4 vertex shader constants from `reg`.
    HRESULT JOAN_STDCALL SetVertexShaderConstant(DWORD reg, const void* data, DWORD count) override;
    /// Reads `count` constants from `reg`.
    HRESULT JOAN_STDCALL GetVertexShaderConstant(DWORD reg, void* data, DWORD count) override;
    /// Copies the shader's declaration tokens.
    HRESULT JOAN_STDCALL GetVertexShaderDeclaration(DWORD handle, void* data, DWORD* size) override;
    /// Copies the shader's function tokens.
    HRESULT JOAN_STDCALL GetVertexShaderFunction(DWORD handle, void* data, DWORD* size) override;
    /// Binds a vertex buffer with `stride` to stream `stream`.
    HRESULT JOAN_STDCALL SetStreamSource(UINT stream, IDirect3DVertexBuffer8* vb, UINT stride) override;
    /// The bound vertex buffer and stride, with a reference added.
    HRESULT JOAN_STDCALL GetStreamSource(UINT stream, IDirect3DVertexBuffer8** vb, UINT* stride) override;
    /// Binds the index buffer and sets the base vertex index.
    HRESULT JOAN_STDCALL SetIndices(IDirect3DIndexBuffer8* ib, UINT base_vertex) override;
    /// The bound index buffer and base vertex, with a reference added.
    HRESULT JOAN_STDCALL GetIndices(IDirect3DIndexBuffer8** ib, UINT* base_vertex) override;
    /// Pixel shaders are not implemented: a non-zero handle is logged once and stored.
    HRESULT JOAN_STDCALL SetPixelShader(DWORD handle) override;
    /// The stored pixel shader handle.
    HRESULT JOAN_STDCALL GetPixelShader(DWORD* handle) override;

    // --- used by the resources ---
    /// Cached framebuffer object for the pair of surfaces; created on first use.
    gl::GLuint framebuffer_for(Surface* color, Surface* depth);
    /// [Graphics] MSAA: for a multisampled surface (the back buffer), resolves its samples into
    /// its texture and returns a framebuffer reading that texture. Every read-back of the back
    /// buffer (Present, LockRect, CopyRects, GetFrontBuffer, frame dumps) goes through here. For
    /// any other surface it does nothing and returns framebuffer_for(s, nullptr).
    gl::GLuint resolve_multisample(Surface* s);
    /// [Graphics] MSAA: after the CPU replaced a multisampled surface's texture (write_pixels),
    /// copies that picture into every sample (a textured quad; a blit cannot write multisampled).
    void upload_multisample(Surface* s);
    /// Drops cached framebuffers that use `s`; call before deleting a surface.
    void forget_surface(Surface* s);  // drops cached framebuffers using `s`
    /// True when called on the thread that created the device.
    bool owner_thread() const;

    /// Entry point for native draws (stage 3, N2): checks the thread, binds the current render
    /// target and applies the device's current Direct3D viewport / raster state, i.e. what a
    /// Direct3D draw does before choosing its program. The caller then draws through
    /// render::current() (program, geometry, textures, uniforms). False = do not draw.
    bool begin_native_draw();
    /// [Graphics] AmbientOcclusion: called (render::opaque_scene_done) when the en3d scene's
    /// opaque geometry is drawn; darkens the back buffer with the occlusion of its depth.
    void ambient_occlusion(const float* projection);
    /// [Graphics] GpuShadowVolumes (render/shadow_volumes.hpp): draws a stencil shadow volume with the current
    /// Direct3D state (render::shadow_volume_draw; `both`: draw both stencil passes at once when the state allows).
    bool shadow_volume(const render::VolumeSource& src, const float* dir, float length, bool upload, bool* both);
    /// [Graphics] AmbientOcclusion phase 2: called (render::transparent_scene_done) when the
    /// scene's transparent pass is drawn; adds the occlusion of the alpha-tested cut-outs
    /// (foliage) drawn in it, which the draws repeated into the occlusion's depth.
    void ao_cutout_end(const float* projection);
    /// [Graphics] Shadows=2: render::shadow_scene_begin / shadow_opaque_done.
    void shadow_begin(const float* sun);
    /// render::overlay_begin / overlay_end.
    void overlay(bool begin);
    /// [Graphics] Shadows=2: render::shadow_opaque_done; `projection` is the camera projection.
    void shadow_done(const float* projection);
    /// [Graphics] Water=1: render::water_scene_done; true = the water was drawn.
    bool water_pass(const float* projection, const float* sun);
    /// [Graphics] GrassBatching (render/grass_batch.hpp).
    bool grass_replay(const render::GrassKey& key, const float* wind, std::uint32_t colour, bool fading);
    /// Starts recording draws of the patch `key`; false when recording is not possible.
    bool grass_record_begin(const render::GrassKey& key);
    /// Records the colour a recorded patch is drawn with.
    void grass_record_colour(std::uint32_t colour);
    /// Finishes the recording started by grass_record_begin.
    void grass_record_end();
    /// Drops the recordings of the patch mesh `plane`.
    void grass_forget(const void* plane);
    /// Sends the queued replayed draws to the GPU.
    void grass_flush() { renderer_.flush_queued(); }  // the queued replayed draws to the GPU

   private:
    // --- [Graphics] GrassBatching: one recorded draw of a grass patch.
    struct GrassCmd {
        gl::GLuint program = 0, vao = 0;
        std::uint32_t stride = 0;
        render::RasterState raster{};
        Texture* textures[kMaxStages] = {};  // referenced
        gl::GLuint samplers[kMaxStages] = {};
        VertexBuffer* vertices = nullptr;    // referenced (pooled: offset() is a multiple of stride)
        IndexBuffer* indices = nullptr;      // referenced
        gl::GLenum mode = 0;
        gl::GLsizei count = 0;
        std::uint32_t index_byte = 0;        // into `indices` (16-bit)
        std::int32_t base_vertex = 0;        // into `vertices`
        // Block FfpDraw as recorded, the parts the replay keeps (a grass program reads WORLD..WORLD1 and their normal
        // matrices; WORLD1 is the wind and the normal matrices the view's when `normals`): 176 bytes, not the record's
        // 624 - the replay reads every patch's data every frame, from memory that is not in the caches.
        float world0[16] = {};
        float tail[28] = {};                       // material (diffuse, ambient, specular, emissive, power), factor, alpha ref
        std::unique_ptr<float[]> normals_kept;     // normal matrices 0..1 as recorded, when the draw does not set them
        // [Graphics] GrassBindless: worked out on the first replay - 0 not yet, 1 the program's bindless variant
        // samples stages `bindless_mask` from handles in the record, 2 not possible (bound as usual).
        mutable std::uint8_t bindless_state = 0, bindless_mask = 0;
        mutable gl::GLuint bindless_program = 0;
        mutable gl::GLuint64 handles[3] = {};
        bool normals = false, has_colour = false;
        bool alpha_cutout = false;           // alpha-tested, depth-writing (the ambient occlusion's cut-outs)
        std::uint32_t material_colour = 0;
    };
    struct GrassRecording {
        const void* plane = nullptr;
        const void* ctx = nullptr;
        std::uint32_t variant = 0, lights = 0, used = 0;
        bool ok = false;
        std::uint8_t why = 0;  // PerfLog: what spoilt it (grass_sweep's reasons)
        std::vector<GrassCmd> cmds;
        // The recording replayed after this one last time (valid while next_gen == grass_gen_): patches are drawn in
        // much the same order every frame, so the next one is usually found without the map.
        GrassRecording* next = nullptr;
        std::uint32_t next_gen = 0;
    };
    // By patch mesh; each recording at a fixed address (grass_recording_, the next links).
    std::unordered_map<const void*, std::vector<std::unique_ptr<GrassRecording>>> grass_;
    GrassRecording* grass_recording_ = nullptr;
    GrassRecording* grass_last_ = nullptr;  // the last one replayed (valid while grass_last_gen_ == grass_gen_)
    std::uint32_t grass_gen_ = 1, grass_last_gen_ = 0;  // grass_gen_: bumped whenever a recording is destroyed
    std::uint32_t grass_colour_ = 0;
    bool grass_colour_set_ = false;
    std::uint32_t grass_swept_ = 0;
    struct GrassStats {
        unsigned replays = 0, replayed_draws = 0, recorded = 0, failed = 0, relit = 0, looked_up = 0;
        unsigned why[6] = {};  // failed, by reason: unindexed, not FFP, arrays not cached, 32-bit indices, offset, other
    } grass_stats_;
    Matrix grass_normal_{};
    std::uint32_t grass_normal_version_ = 0;
    FfpDrawUniforms grass_rec_{};  // the record a replayed draw is built in (cached memory), then copied to the ring
    bool grass_usable() const;          // no pass that needs the full path is running
    std::uint32_t light_signature() const;  // the enabled lights and their types (the program's), memoised on light_version_
    mutable std::uint32_t light_signature_ = 0, light_signature_version_ = 0;
    void grass_release(GrassRecording& r);
    void grass_record_draw(Enum prim, UINT start_index, UINT prim_count);  // DrawIndexedPrimitive while recording
    void grass_sweep();
    // What prepare_draw / upload_uniforms / apply_raster_state did last (the recording reads them).
    render::RasterState last_raster_{};
    gl::GLuint last_draw_program_ = 0;
    bool last_ffp_ = false, last_normals_ = false;
    int last_worlds_ = 1;
    void upload_frame_block(bool ffp, bool lit);
    void set_defaults();
    void bind_render_target();
    void reset_viewport_to_target();
    gl::GLuint program_for_draw(bool& ffp);        // memoised on program_version_
    gl::GLuint build_program_for_draw(bool& ffp);
    // Bumped whenever state that program_for_draw / the light uniforms read changes (the setters, set_defaults).
    std::uint32_t program_version_ = 1, program_memo_version_ = 0;
    gl::GLuint program_memo_ = 0;
    bool program_memo_ffp_ = true;
    std::uint32_t light_version_ = 1, light_memo_version_ = 0;
    gl::GLuint vertex_array_for(std::uint32_t stride);
    render::SamplerDesc sampler_desc(DWORD stage) const;
    gl::GLuint stage_sampler(DWORD stage);  // prepare_draw: renderer_.sampler(sampler_desc(stage)), memoised per stage
    render::SamplerDesc sampler_memo_desc_[kMaxStages] = {};
    gl::GLuint sampler_memo_[kMaxStages] = {};
    bool sampler_memo_valid_[kMaxStages] = {};
    void apply_raster_state();
    void upload_uniforms(bool ffp);
    bool prepare_draw();
    void create_back_buffer();
    void draw_cursor(int ox, int oy, double scale, int window_w, int window_h);
    void draw_debug_menu(int window_w, int window_h);  // debug_menu.hpp overlay
    bool ensure_cursor_program();
    bool canvas_on() const;              // drawing a 1024x768 screen into the back buffer (display.hpp)
    D3DRect canvas_rect(const D3DRect& r) const;  // game-screen pixels -> back-buffer pixels
    bool layout_viewport() const;  // the viewport is a 1024x768 screen's camera viewport (layout pixels)
    void scene_viewport(int& x, int& y, int& w, int& h) const;  // the camera's viewport in back-buffer pixels
    void clear_canvas_bars();
    void frame_diagnostics(int window_w, int window_h);  // GLDebug=1: per-frame counters, frame dumps
    void dump_draw(Enum prim, UINT start, UINT prims, bool indexed, gl::GLuint program);  // GLDebug=1

    struct FrameStats {
        unsigned draws = 0, no_thread = 0, no_stream = 0, no_program = 0, no_indices = 0, clears = 0;
    };

    std::atomic<ULONG> refs_{1};
    Direct3D* d3d_;
    UINT adapter_;
    Enum device_type_;
    ::HWND window_;  // the Win32 handle type (d3d8::HWND is void*)
    DWORD behavior_;
    PresentParameters pp_;
    DWORD thread_;
    gl::Context ctx_;

    // Direct3D state
    DWORD rs_[256] = {};
    DWORD tss_[kMaxStages][32] = {};
    // Transform matrices: VIEW, PROJECTION, TEXTURE0..7 and WORLD..WORLD3 in arrays (read for every draw),
    // any other index in the map.
    Matrix transform_low_[24], transform_world_[4];
    std::map<Enum, Matrix> transforms_;
    Matrix& transform(Enum state) {
        if (state < 24) return transform_low_[state];
        if (state >= kTsWorld && state < kTsWorld + 4) return transform_world_[state - kTsWorld];
        return transforms_[state];
    }
    Material material_ = {};
    // Block FfpDraw's inputs: draw_version_ changes with a world matrix, the material, the texture factor or the
    // alpha reference; the last record written is reused while its key is the same (upload_uniforms).
    std::uint32_t draw_version_ = 1;
    struct DrawRecordKey {
        std::uint32_t version, view;  // draw_version_, frame_version_ (only when it holds normal matrices)
        int worlds;
        bool normals;
        bool operator==(const DrawRecordKey& o) const { return version == o.version && view == o.view && worlds == o.worlds && normals == o.normals; }
    };
    DrawRecordKey draw_key_ = {};
    bool draw_key_valid_ = false;
    std::map<DWORD, Light> lights_;
    std::map<DWORD, bool> light_enabled_;
    Viewport viewport_ = {};
    Texture* textures_[kMaxStages] = {};
    VertexBuffer* stream_ = nullptr;
    UINT stride_ = 0;
    IndexBuffer* indices_ = nullptr;
    UINT base_vertex_ = 0;
    std::int32_t pool_base_ = 0;  // the bound pooled vertex buffer's place in the geometry pool, in vertices (prepare_draw)
    DWORD vertex_shader_ = 0;  // FVF code, or a shader handle (odd)
    DWORD pixel_shader_ = 0;
    std::map<DWORD, VertexShaderInfo> shaders_;
    DWORD next_shader_ = 1;
    float vs_constants_[kVsConstants][4] = {};
    bool vs_constants_dirty_ = true;
    Surface* back_buffer_ = nullptr;
    Surface* auto_depth_ = nullptr;
    Surface* hidden_back_ = nullptr;   // set_hidden_view: the scratch back buffer (swapped with back_buffer_ while on)
    Surface* hidden_depth_ = nullptr;  // and its depth (swapped with auto_depth_)
    bool hidden_view_ = false;
    Surface* render_target_ = nullptr;
    Surface* depth_stencil_ = nullptr;
    GammaRamp gamma_ = {};
    // Hardware cursor: drawn by us over the presented picture (the game hides the Windows cursor).
    gl::GLuint cursor_texture_ = 0, cursor_program_ = 0, cursor_vao_ = 0;
    gl::GLint cursor_rect_location_ = -1;
    UINT cursor_width_ = 0, cursor_height_ = 0, cursor_hot_x_ = 0, cursor_hot_y_ = 0;
    int cursor_x_ = 0, cursor_y_ = 0;  // back-buffer pixels
    BOOL cursor_visible_ = FALSE;
    // Developer menu overlay (debug_menu.hpp), drawn with the cursor program.
    gl::GLuint menu_texture_ = 0;
    int menu_width_ = 0, menu_height_ = 0;
    unsigned menu_version_ = 0;

    // OpenGL objects (programs, vertex arrays, samplers and uniform buffers are the renderer's)
    render::Renderer renderer_;
    render::AoPass ao_;  // [Graphics] AmbientOcclusion
    StreamRing stream_ring_;  // the engine's dynamic vertex / index data (see StreamRing)
    GeometryPool geometry_pool_;  // the stream cache's buffers (see GeometryPool)
    // Phase 2 window (ambient_occlusion .. ao_cutout_end): the camera's input, its viewport,
    // and how many cut-out draws reached the cut-out target.
    render::AoPass::Input ao_in_ = {};
    float ao_projection_[16] = {};
    Viewport ao_viewport_ = {};
    bool ao_window_ = false;
    unsigned ao_cutout_draws_ = 0;
    bool ao_cut_valid_ = false, ao_cut_last_ = false;  // buffer 1 state last set by ao_prepare_cutout
    std::uint32_t ao_cut_epoch_ = 0;
    bool ao_cutout_draw() const;
    void ao_prepare_cutout();
    void ao_prepare_cutout(bool cutout);
    bool alpha_cutout_state() const;  // the render states of an alpha-tested, depth-writing draw
    bool ao_cutout_target() const;    // phase 2 window, into its camera's scene framebuffer
    // [Graphics] Shadows=2 (render/shadow_map.hpp): the opaque pass's draws, replayed into the shadow map.
    void shadow_record(gl::GLenum mode, gl::GLsizei count, gl::GLint first, bool indexed, bool wide, std::uintptr_t index_offset);
    void foliage_record(gl::GLenum mode, gl::GLsizei count, gl::GLint first, bool indexed, bool wide, std::uintptr_t index_offset);
    render::ShadowDraw shadow_draw_now(gl::GLenum mode, gl::GLsizei count, gl::GLint first, bool indexed, bool wide,
                                       std::uintptr_t index_offset);
    render::ShadowPass shadow_;
    std::vector<render::ShadowDraw> shadow_draws_;
    bool shadow_recording_ = false;
    // The rings and buffer renames when recording started (recorded_intact).
    std::uint32_t record_stream_switches_ = 0, record_uniform_switches_ = 0;
    unsigned skipped_logged_ = 0;
    void drop_deleted(std::vector<render::ShadowDraw>& draws);
    bool shadows_skipped_ = false;  // this frame (Present: the level is not settled yet)
    unsigned loading_good_frames_ = 0;  // Present: settled frames in a row after the loading screen
    bool recorded_intact() const;
    float shadow_sun_[3] = {};
    // ShadowFoliage: the transparent pass's cut-outs (shadow_done .. ao_cutout_end), added to the shadow map
    // at the end of the pass (ShadowPass::run_foliage) with the camera of shadow_done.
    std::vector<render::ShadowDraw> foliage_draws_;
    render::ShadowPass::Input foliage_in_ = {};
    float foliage_view_[16] = {}, foliage_projection_[16] = {};
    bool foliage_recording_ = false;
    void foliage_shadows();
    // [Graphics] BakedShadows: the level map lifting the ground's lightmaps (on while a shadow-map scene draws).
    render::WaterPass water_;  // [Graphics] Water
    bool river_now_ = false;   // prepare_draw: this draw is a river drawn with the water shader
    bool surface_pl_ = false;  // [Graphics] PixelLighting in effect (the program choice changes with it)
    render::BakedShadows baked_;
    render::ShadowVolumes volumes_;  // [Graphics] GpuShadowVolumes
    std::uint32_t presented_ = 0;    // frames presented
    bool baked_on_ = false;
    bool draw_lift_ = false;  // the program just chosen lifts a lightmap stage (gld::kPixelLift)
    bool lightmap_stage(DWORD stage) const;  // stage `stage` reads the ground's lightmap
    struct ShadowStats {  // [Graphics] PerfLog: the opaque pass's draws by what the shadow map does with them
        unsigned recorded = 0, blended = 0, other_target = 0, no_depth = 0, no_colour = 0, two_d = 0, foliage = 0;
    } shadow_stats_;
    gl::GLuint shadow_scene_fbo_ = 0;
    void ao_close_window();
    void ao_restore_state();
    std::map<std::pair<Surface*, Surface*>, gl::GLuint> framebuffers_;
    // framebuffer_for's last answer (the replayed grass asks per patch); cleared by forget_surface.
    Surface* fb_memo_color_ = nullptr;
    Surface* fb_memo_depth_ = nullptr;
    gl::GLuint fb_memo_ = 0;
    void apply_viewport_scissor();  // apply_raster_state's viewport and scissor part
    void apply_raster_states();     // its render states part (RasterState)
    gl::GLuint current_fbo_ = 0;
    // [Graphics] MSAA: sample count of the back buffer (0 = off, the original single-sampled path).
    int msaa_samples_ = 0;
    Surface* resolve_owner_ = nullptr;  // the multisampled surface resolve_fbo_ reads into
    gl::GLuint resolve_fbo_ = 0;        // framebuffer on resolve_owner_->gl_texture
    bool enable_multisample(Surface* color, Surface* depth);  // create_back_buffer; false = MSAA unavailable
    FfpUniforms uniforms_ = {};           // block Ffp: rebuilt when frame_version_ moved (upload_uniforms)
    FfpDrawUniforms draw_uniforms_ = {};  // block FfpDraw: every draw
    // Bumped by the setters of what block Ffp holds (view / projection / texture matrices, the viewport, lights,
    // fog / ambient / point render states, bump stage states).
    std::uint32_t frame_version_ = 1, frame_memo_version_ = 0;
    // The world / view matrices draw_uniforms_.normal_matrix[i] was computed from (upload_uniforms).
    Matrix normal_world_[4], normal_view_[4];
    bool normal_valid_[4] = {};
    // [Graphics] PerfLog: time and counters since `start` (perf_frame).
    struct Perf {
        std::chrono::steady_clock::time_point start;
        std::chrono::steady_clock::duration swap{};
        // Draw* calls in CPU timestamp ticks (__rdtsc: one instruction; the clock's QueryPerformanceCounter around
        // every draw cost a few % of the frame), converted with the ticks counted over the same interval.
        std::uint64_t draw_ticks = 0, start_ticks = 0;
        unsigned frames = 0, frame_draws = 0;
        unsigned long long draws = 0, raster_applied = 0, raster_skipped = 0, uniform_uploads = 0, uniform_skipped = 0,
                           ring_waits = 0, program_binds = 0, geometry_binds = 0, texture_binds = 0;
    } perf_;
    // Times one Draw* call into perf_ (only with PerfLog on).
    struct PerfScope {
        Perf& p;
        const bool on;
        std::uint64_t t0;
        explicit PerfScope(Perf& perf)
            : p(perf), on(hook::settings().perf_log > 0), t0(on ? joan::cpu::ticks() : 0) {
            ++p.frame_draws;
        }
        ~PerfScope() {
            if (on) p.draw_ticks += joan::cpu::ticks() - t0;
        }
    };
    void perf_frame();

    // diagnostics
    unsigned frame_ = 0;
    FrameStats stats_;
    bool screenshot_key_down_ = false;
    // frame pacing
    void pace_frame();
    double frame_interval_ = 0.0;      // seconds, 0 = no limiter
    std::int64_t last_present_ = 0, perf_freq_ = 1, fps_window_start_ = 0;
    unsigned fps_window_frames_ = 0, fps_reports_ = 0;
    unsigned canvas_draws_ = 0, plain_draws_ = 0;  // this frame
    bool drew_3d_ = false;
    // The level's first frames after the loading screen are kept black (Present).
    // render::overlay_begin / overlay_end: the depth buffer before ([0]) and after ([1]) the billboards drawn
    // before the world (this frame: overlay_valid_; overlay_begun_ between the two).
    gl::GLuint overlay_depth_[2] = {}, overlay_fbo_[2] = {};
    int overlay_w_ = 0, overlay_h_ = 0;
    bool overlay_begun_ = false, overlay_valid_ = false;
    bool loading_black_ = false;
    std::chrono::steady_clock::time_point loading_black_start_{}, last_present_time_{};
    std::uint32_t scene_bottom_ = 0;  // lowest viewport edge of this frame's 3D draws (cinematic bars)                          // this frame
    unsigned canvas_dumps_ = 0, canvas_frames_logged_ = 0, canvas_frame_index_ = 0;
    bool probe_after_draw_ = false;
    void probe_pixels(const char* when);  // diagnostics of the first 1024x768 screen
    unsigned draws_dumped_ = 0, frame_draws_dumped_ = 0;
    gl::GLuint last_program_ = 0;
};

/// Returns our IDirect3D8 (the game's Direct3DCreate8, d3d8/direct3d_create8.cpp).
IDirect3D8* create_direct3d();

/// apply_live_settings of the running device, if there is one.
void apply_live_settings();

/// set_hidden_view of the running device, if there is one (gl/views.hpp).
void set_hidden_view(bool on);

}  // namespace joan::gld
