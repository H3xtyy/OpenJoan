// The Direct3D 8 device on OpenGL 4.6 (see device.hpp).
#include "device.hpp"

#include "../debug_menu.hpp"
#include "../fp/x87.hpp"

#include <float.h>
#include "../platform/win32_types.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>

#include "../display.hpp"
#include "../platform/window.hpp"
#include "../platform/threads.hpp"
#include "../runtime/loader/import_overrides.hpp"
#include "../runtime/profiler.hpp"
#include "../runtime/hitch.hpp"
#include "../registry.hpp"
#include "../runtime/replay/replay.hpp"
#include "../render/ambient_occlusion.hpp"
#include "../render/baked_shadows.hpp"
#include "../render/shadow_map.hpp"
#include "glapi.hpp"
#include "../game/stream_cache_api.hpp"

namespace joan::gld {

using namespace joan::gl;

class Device;
namespace {
Device* g_ao_device = nullptr;  // the device render::opaque_scene_done reaches
void ao_hook(const float* projection);
void ao_cutout_hook(const float* projection);
void shadow_begin_hook(const float* sun);
void shadow_done_hook(const float* projection);
bool water_hook(const float* projection, const float* sun);
bool volume_hook(const render::VolumeSource& src, const float* dir, float length, bool upload, bool* both);
extern const render::GrassHooks kGrassHooks;
void overlay_hook(bool begin);
}  // namespace

namespace {

constexpr GLenum GL_DEPTH_ = 0x1801;
constexpr GLenum GL_STENCIL_ = 0x1802;

float as_float(DWORD v) {
    float f;
    std::memcpy(&f, &v, 4);
    return f;
}
DWORD as_dword(float f) {
    DWORD v;
    std::memcpy(&v, &f, 4);
    return v;
}
void color4(D3DCOLOR c, float out[4]) {
    out[0] = static_cast<float>((c >> 16) & 0xff) / 255.0f;
    out[1] = static_cast<float>((c >> 8) & 0xff) / 255.0f;
    out[2] = static_cast<float>(c & 0xff) / 255.0f;
    out[3] = static_cast<float>(c >> 24) / 255.0f;
}
void copy4(const ColorValue& c, float out[4]) {
    out[0] = c.r;
    out[1] = c.g;
    out[2] = c.b;
    out[3] = c.a;
}

// Direct3D matrices are row-major and multiply row vectors (v' = v M).
Matrix multiply(const Matrix& a, const Matrix& b) {
    Matrix r;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float s = 0;
            for (int k = 0; k < 4; ++k) s += a.m[i * 4 + k] * b.m[k * 4 + j];
            r.m[i * 4 + j] = s;
        }
    return r;
}

// Inverse transpose of the upper 3x3 (normal transform for row vectors).
Matrix normal_matrix(const Matrix& m) {
    const float a = m.m[0], b = m.m[1], c = m.m[2], d = m.m[4], e = m.m[5], f = m.m[6], g = m.m[8], h = m.m[9],
                i = m.m[10];
    const float A = e * i - f * h, B = -(d * i - f * g), C = d * h - e * g;
    const float det = a * A + b * B + c * C;
    Matrix r;
    if (std::fabs(det) < 1e-20f) return r;
    const float s = 1.0f / det;
    // inverse = adjugate / det; its transpose is the cofactor matrix / det.
    r.m[0] = A * s;
    r.m[1] = B * s;
    r.m[2] = C * s;
    r.m[4] = -(b * i - c * h) * s;
    r.m[5] = (a * i - c * g) * s;
    r.m[6] = -(a * h - b * g) * s;
    r.m[8] = (b * f - c * e) * s;
    r.m[9] = -(a * f - c * d) * s;
    r.m[10] = (a * e - b * d) * s;
    r.m[3] = r.m[7] = r.m[11] = r.m[12] = r.m[13] = r.m[14] = 0;
    r.m[15] = 1;
    return r;
}

void transform_point(const Matrix& m, const Vector3& p, float out[4]) {
    for (int j = 0; j < 3; ++j) out[j] = p.x * m.m[j] + p.y * m.m[4 + j] + p.z * m.m[8 + j] + m.m[12 + j];
    out[3] = 1;
}
void transform_direction(const Matrix& m, const Vector3& d, float out[4]) {
    for (int j = 0; j < 3; ++j) out[j] = d.x * m.m[j] + d.y * m.m[4 + j] + d.z * m.m[8 + j];
    const float len = std::sqrt(out[0] * out[0] + out[1] * out[1] + out[2] * out[2]);
    if (len > 0)
        for (int j = 0; j < 3; ++j) out[j] /= len;
    out[3] = 0;
}

GLenum compare_func(DWORD f) {
    switch (f) {
        case 1: return GL_NEVER;
        case 2: return GL_LESS;
        case 3: return GL_EQUAL;
        case 4: return GL_LEQUAL;
        case 5: return GL_GREATER;
        case 6: return GL_NOTEQUAL;
        case 7: return GL_GEQUAL;
        default: return GL_ALWAYS;
    }
}
GLenum blend_factor(DWORD b) {
    switch (b) {
        case 1: return GL_ZERO;
        case 2: return GL_ONE;
        case 3: return GL_SRC_COLOR;
        case 4: return GL_ONE_MINUS_SRC_COLOR;
        case 5: return GL_SRC_ALPHA;
        case 6: return GL_ONE_MINUS_SRC_ALPHA;
        case 7: return GL_DST_ALPHA;
        case 8: return GL_ONE_MINUS_DST_ALPHA;
        case 9: return GL_DST_COLOR;
        case 10: return GL_ONE_MINUS_DST_COLOR;
        case 11: return GL_SRC_ALPHA_SATURATE;
        default: return GL_ONE;
    }
}
GLenum stencil_op(DWORD o) {
    switch (o) {
        case 2: return GL_ZERO;
        case 3: return GL_REPLACE;
        case 4: return GL_INCR;
        case 5: return GL_DECR;
        case 6: return GL_INVERT;
        case 7: return GL_INCR_WRAP;
        case 8: return GL_DECR_WRAP;
        default: return GL_KEEP;
    }
}
GLenum primitive_mode(Enum p) {
    switch (p) {
        case kPrimPointList: return GL_POINTS;
        case kPrimLineList: return GL_LINES;
        case kPrimLineStrip: return GL_LINE_STRIP;
        case kPrimTriangleStrip: return GL_TRIANGLE_STRIP;
        case kPrimTriangleFan: return GL_TRIANGLE_FAN;
        default: return GL_TRIANGLES;
    }
}
GLsizei vertex_count(Enum p, UINT prims) {
    switch (p) {
        case kPrimPointList: return static_cast<GLsizei>(prims);
        case kPrimLineList: return static_cast<GLsizei>(prims * 2);
        case kPrimLineStrip: return static_cast<GLsizei>(prims + 1);
        case kPrimTriangleStrip:
        case kPrimTriangleFan: return static_cast<GLsizei>(prims + 2);
        default: return static_cast<GLsizei>(prims * 3);
    }
}

void JOAN_GL_APIENTRY gl_debug_callback(GLenum, GLenum type, GLuint, GLenum severity, GLsizei, const GLchar* message,
                                 const void*) {
    static int count = 0;
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION || count >= 100) return;
    ++count;
    hook::log_line("GL %s: %s", type == GL_DEBUG_TYPE_ERROR ? "error" : "debug", message);
}

// 32-bit BMP; `top_down` = first row of `bgra` is the top of the image.
void write_bmp(const std::string& path, const std::uint8_t* bgra, int w, int h, bool top_down) {
    std::FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) return;
    const std::uint32_t image = static_cast<std::uint32_t>(w) * static_cast<std::uint32_t>(h) * 4;
#pragma pack(push, 1)
    struct BmpHeader {  // BITMAPFILEHEADER + BITMAPINFOHEADER
        char magic[2];
        std::uint32_t file_size;
        std::uint32_t reserved;
        std::uint32_t data_offset;
        std::uint32_t info_size;
        std::uint32_t width;
        std::uint32_t height;  // negative = top-down
        std::uint16_t planes;
        std::uint16_t bpp;
        std::uint32_t compression;
        std::uint32_t image_size;
        std::uint8_t rest[16];  // resolution, palette counts
    };
#pragma pack(pop)
    static_assert(sizeof(BmpHeader) == 54);
    BmpHeader header{};
    header.magic[0] = 'B';
    header.magic[1] = 'M';
    header.file_size = 54 + image;
    header.data_offset = 54;
    header.info_size = 40;
    header.width = static_cast<std::uint32_t>(w);
    header.height = static_cast<std::uint32_t>(top_down ? -h : h);
    header.planes = 1;
    header.bpp = 32;
    header.image_size = image;
    std::fwrite(&header, 1, sizeof header, f);
    std::fwrite(bgra, 1, image, f);
    std::fclose(f);
}

bool logged_frame(unsigned f) { return f <= 3 || f == 10 || f == 30 || f == 100 || f == 300 || f == 1000 || f % 5000 == 0; }

}  // namespace

// ---------------------------------------------------------------------------
// creation
// ---------------------------------------------------------------------------
Device::Device(Direct3D* d3d, UINT adapter, Enum type, HWND window, DWORD behavior, const PresentParameters& pp)
    : d3d_(d3d), adapter_(adapter), device_type_(type), window_(static_cast<::HWND>(window)), behavior_(behavior), pp_(pp),
      thread_(static_cast<DWORD>(joan::platform::thread_id())) {
    d3d_->AddRef();
}

bool Device::init(std::string& error) {
    // Direct3D switches the calling thread to single precision unless the game asks it not to
    // (D3DCREATE_FPU_PRESERVE): the precision of the emulated x87 arithmetic (src/fp), the same on every build.
    if (!(behavior_ & 0x2)) {
        joan::fp::set_precision(joan::fp::Precision::Single);
#if defined(_M_IX86) || defined(__i386__)
        // 32-bit x86 only: the real x87 too. The compiler still finishes some float expressions on a function's x87
        // result (st0) with x87 instructions (fsub / fadd ...); at 24 bits a chain of them rounds each step to float,
        // as SSE does on every other build. Nothing reads this control word back.
        joan::cpu::x87_single_precision();
#endif
    }

    const auto& s = hook::settings();
    {
        perf_freq_ = platform::perf_frequency();
        // (The frame limiter sleeps with SDL_Delay, a high-resolution timer: no timeBeginPeriod(1) needed.)
        int fps = s.max_fps;
        if (fps < 0) {
            fps = platform::refresh_rate(window_);
            if (fps <= 1) fps = 60;
        }
        frame_interval_ = fps > 0 ? 1.0 / fps : 0.0;
        hook::log_line("OpenGL device: frame limiter %s", fps > 0 ? (std::to_string(fps) + " fps").c_str() : "off");
    }
    if (!pp_.windowed && s.borderless) {
        // Direct3D would switch the display mode; we cover the monitor instead and scale at Present (the SDL
        // window of platform/sdl_window.cpp: SDL's desktop full screen; Window=win32: a popup over the monitor).
#ifdef _WIN32
        MONITORINFO mi = {};
        mi.cbSize = sizeof mi;
        if (!platform::cover_monitor(window_) && GetMonitorInfoA(MonitorFromWindow(window_, MONITOR_DEFAULTTONEAREST), &mi)) {
            SetWindowLongA(window_, GWL_STYLE, static_cast<LONG>(WS_POPUP | WS_VISIBLE));
            SetWindowPos(window_, nullptr, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_SHOWWINDOW);
            hook::log_line("OpenGL device: full screen requested - borderless window %ldx%ld", mi.rcMonitor.right - mi.rcMonitor.left,
                           mi.rcMonitor.bottom - mi.rcMonitor.top);
        }
#else
        platform::cover_monitor(window_);  // always the SDL window elsewhere
#endif
    }
    if (!gl::create_context(window_, s.gl_debug, s.vsync, ctx_, error)) return false;
    hook::log_line("OpenGL device: %s / %s / %s (context %d.%d) on window %p, back buffer %ux%u",
                   reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
                   reinterpret_cast<const char*>(glGetString(GL_RENDERER)),
                   reinterpret_cast<const char*>(glGetString(GL_VERSION)), ctx_.major, ctx_.minor,
                   static_cast<void*>(window_), pp_.back_buffer_width, pp_.back_buffer_height);
    if (s.gl_debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(&gl_debug_callback, nullptr);
    }
    glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    static_assert(sizeof(vs_constants_) == sizeof(float[kVsConstants][4]), "VsConst block size");
    renderer_.init();  // uniform buffers (blocks 0 and 1)
    if (!stream_ring_.init()) hook::log_line("OpenGL device: no stream ring (0x%x) - dynamic buffers renamed per buffer", glGetError());
    if (!geometry_pool_.init()) hook::log_line("OpenGL device: no geometry pool (0x%x) - cached models in their own buffers", glGetError());
    prof::start_for_current_thread();  // [Debug] Profile: the device is made on the game's main thread
    render::set_current(&renderer_);
    g_ao_device = this;
    render::set_opaque_scene_hook(&ao_hook);
    render::set_transparent_scene_hook(&ao_cutout_hook);
    render::set_shadow_hooks(&shadow_begin_hook, &shadow_done_hook);  // [Graphics] Shadows=2
    render::set_water_hook(&water_hook);  // [Graphics] Water
    render::set_volume_hook(&volume_hook);  // [Graphics] GpuShadowVolumes
    render::set_grass_hooks(&kGrassHooks);  // [Graphics] GrassBatching
    render::set_overlay_hook(&overlay_hook);  // the billboards the screen-space passes leave alone
    if (render::ao_params().mode != 0) {
        const auto& a = render::ao_params();
        hook::log_line("OpenGL device: ambient occlusion %s, radius %.1f, strength %.2f, distance %.0f..%.0f",
                       a.mode == 2 ? "on (occlusion only view)" : "on", static_cast<double>(a.radius),
                       static_cast<double>(a.strength), static_cast<double>(a.fade_start), static_cast<double>(a.fade_end));
    }
    // Graphics quality options (all off by default = the original look). Texture options act in
    // render::Renderer::sampler; MSAA in create_back_buffer / framebuffer_for / resolve_multisample.
    renderer_.set_texture_quality(s.anisotropy >= 2 ? static_cast<std::uint32_t>(s.anisotropy) : 0u, s.texture_filter == 1);
    if (renderer_.anisotropy() || s.texture_filter == 1)
        hook::log_line("OpenGL device: texture quality: anisotropy %u (asked %d), trilinear %s", renderer_.anisotropy(),
                       s.anisotropy, s.texture_filter == 1 ? "on" : "off");
    if (s.msaa >= 2) {
        GLint max_samples = 0;
        glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
        int n = 1;
        while (n * 2 <= std::min(s.msaa, 16) && n * 2 <= max_samples) n *= 2;  // a power of two the driver offers
        msaa_samples_ = n >= 2 ? n : 0;
        hook::log_line("OpenGL device: MSAA %dx (asked %d, driver maximum %d)", msaa_samples_, s.msaa, max_samples);
    }
    create_back_buffer();
    set_defaults();
    return true;
}

void Device::create_back_buffer() {
    UINT w = pp_.back_buffer_width, h = pp_.back_buffer_height;
    if (w == 0 || h == 0) {
        int cw = 0, ch = 0;
        platform::client_size(window_, cw, ch);
        w = std::max<UINT>(1, static_cast<UINT>(cw));
        h = std::max<UINT>(1, static_cast<UINT>(ch));
    }
    back_buffer_ = new Surface(this, Surface::Kind::BackBuffer, kFmtX8R8G8B8, w, h);
    if (pp_.enable_auto_depth_stencil)
        auto_depth_ = new Surface(this, Surface::Kind::DepthStencil, pp_.auto_depth_stencil_format, w, h);
    render_target_ = back_buffer_;
    render_target_->AddRef();
    depth_stencil_ = auto_depth_;
    if (depth_stencil_) depth_stencil_->AddRef();
    // [Graphics] MSAA: the game draws into multisampled storage; on failure the plain path is used.
    if (msaa_samples_ && !enable_multisample(back_buffer_, auto_depth_)) msaa_samples_ = 0;
    // GL leaves new renderbuffers undefined; start from black / far depth / stencil 0.
    const GLuint fbo = framebuffer_for(back_buffer_, auto_depth_);
    const float black[4] = {0, 0, 0, 1};
    glClearNamedFramebufferfv(fbo, GL_COLOR, 0, black);
    if (auto_depth_) glClearNamedFramebufferfi(fbo, GL_DEPTH_STENCIL, 0, 1.0f, 0);
}

// [Graphics] MSAA: gives the back buffer (and its automatic depth-stencil surface) multisampled
// renderbuffers. framebuffer_for then renders into those; gl_texture / gl_renderbuffer stay as
// the resolve target and the single-sampled depth for game render targets. Returns false (and
// leaves both surfaces as they were) if the driver cannot make a complete framebuffer of them.
bool Device::enable_multisample(Surface* color, Surface* depth) {
    const auto w = static_cast<GLsizei>(color->width), h = static_cast<GLsizei>(color->height);
    glCreateRenderbuffers(1, &color->gl_msaa);
    glNamedRenderbufferStorageMultisample(color->gl_msaa, msaa_samples_, GL_RGBA8, w, h);
    if (depth) {
        glCreateRenderbuffers(1, &depth->gl_msaa);
        glNamedRenderbufferStorageMultisample(depth->gl_msaa, msaa_samples_, GL_DEPTH24_STENCIL8,
                                              static_cast<GLsizei>(depth->width), static_cast<GLsizei>(depth->height));
    }
    const GLuint fbo = framebuffer_for(color, depth);
    if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) return true;
    hook::log_line("OpenGL device: MSAA %dx framebuffer not supported by the driver - MSAA off", msaa_samples_);
    forget_surface(color);  // the framebuffers made with the multisampled storage
    if (depth) forget_surface(depth);
    glDeleteRenderbuffers(1, &color->gl_msaa);
    color->gl_msaa = 0;
    if (depth && depth->gl_msaa) {
        glDeleteRenderbuffers(1, &depth->gl_msaa);
        depth->gl_msaa = 0;
    }
    return false;
}

Device::~Device() {
    if (render::current() == &renderer_) render::set_current(nullptr);
    if (g_ao_device == this) {
        g_ao_device = nullptr;
        render::set_opaque_scene_hook(nullptr);
        render::set_transparent_scene_hook(nullptr);
        render::set_shadow_hooks(nullptr, nullptr);
        render::set_water_hook(nullptr);
        render::set_volume_hook(nullptr);
        render::set_grass_hooks(nullptr);
        for (auto& [plane, list] : grass_)
            for (auto& r : list) grass_release(*r);
        grass_.clear();
        ++grass_gen_;
        render::set_overlay_hook(nullptr);
    }
    if (ctx_.valid()) gl::make_current(ctx_);
    for (auto*& t : textures_)
        if (t) std::exchange(t, nullptr)->Release();
    if (stream_) std::exchange(stream_, nullptr)->Release();
    if (indices_) std::exchange(indices_, nullptr)->Release();
    if (render_target_) std::exchange(render_target_, nullptr)->Release();
    if (depth_stencil_) std::exchange(depth_stencil_, nullptr)->Release();
    if (back_buffer_) std::exchange(back_buffer_, nullptr)->Release();
    if (auto_depth_) std::exchange(auto_depth_, nullptr)->Release();
    if (ctx_.valid()) {
        stream_ring_.shutdown();
        geometry_pool_.shutdown();
        shadow_.shutdown();
        baked_.shutdown();
        water_.shutdown();
        volumes_.shutdown();
        glDeleteFramebuffers(2, overlay_fbo_);
        glDeleteTextures(2, overlay_depth_);
        ao_.shutdown();
        renderer_.shutdown();  // programs, vertex arrays, samplers, uniform buffers
        for (auto& [k, f] : framebuffers_) glDeleteFramebuffers(1, &f);
        fb_memo_ = 0;
        if (resolve_fbo_) glDeleteFramebuffers(1, &resolve_fbo_);
    }
    if (ctx_.valid()) {
        if (cursor_texture_) glDeleteTextures(1, &cursor_texture_);
        if (menu_texture_) glDeleteTextures(1, &menu_texture_);
        if (cursor_program_) glDeleteProgram(cursor_program_);
        if (cursor_vao_) glDeleteVertexArrays(1, &cursor_vao_);
    }
    gl::destroy_context(ctx_);
    d3d_->Release();
}

HRESULT Device::QueryInterface(const Guid&, void** out) {
    if (!out) return kInvalidCall;
    *out = this;
    AddRef();
    return kOk;
}

ULONG Device::Release() {
    const ULONG n = --refs_;
    if (n == 0) delete this;
    return n;
}

bool Device::owner_thread() const { return joan::platform::thread_id() == thread_; }

void Device::set_defaults() {
    std::fill(std::begin(rs_), std::end(rs_), 0u);
    rs_[kRsZEnable] = pp_.enable_auto_depth_stencil ? 1 : 0;
    rs_[kRsFillMode] = 3;
    rs_[kRsShadeMode] = 2;
    rs_[kRsZWriteEnable] = 1;
    rs_[kRsLastPixel] = 1;
    rs_[kRsSrcBlend] = 2;
    rs_[kRsDestBlend] = 1;
    rs_[kRsCullMode] = 3;
    rs_[kRsZFunc] = 4;
    rs_[kRsAlphaFunc] = 8;
    rs_[kRsFogEnd] = as_dword(1.0f);
    rs_[kRsFogDensity] = as_dword(1.0f);
    rs_[kRsStencilFail] = rs_[kRsStencilZFail] = rs_[kRsStencilPass] = 1;
    rs_[kRsStencilFunc] = 8;
    rs_[kRsStencilMask] = rs_[kRsStencilWriteMask] = 0xffffffff;
    rs_[kRsTextureFactor] = 0xffffffff;
    rs_[kRsClipping] = 1;
    rs_[kRsLighting] = 1;
    rs_[kRsColorVertex] = 1;
    rs_[kRsLocalViewer] = 1;
    rs_[kRsDiffuseMaterialSource] = 1;
    rs_[kRsSpecularMaterialSource] = 2;
    rs_[kRsPointSize] = as_dword(1.0f);
    rs_[kRsPointSizeMax] = as_dword(64.0f);
    rs_[158] = as_dword(1.0f);  // POINTSCALE_A
    rs_[kRsMultiSampleAntiAlias] = 1;
    rs_[kRsMultiSampleMask] = 0xffffffff;
    rs_[164] = as_dword(1.0f);  // PATCHSEGMENTS
    rs_[kRsColorWriteEnable] = 0xf;
    rs_[kRsBlendOp] = 1;
    for (DWORD s = 0; s < kMaxStages; ++s) {
        DWORD* t = tss_[s];
        std::fill(t, t + 32, 0u);
        t[kTssColorOp] = s == 0 ? 4 : 1;
        t[kTssColorArg1] = 2;
        t[kTssColorArg2] = 1;
        t[kTssAlphaOp] = s == 0 ? 2 : 1;
        t[kTssAlphaArg1] = 2;
        t[kTssAlphaArg2] = 1;
        t[kTssTexCoordIndex] = s;
        t[kTssAddressU] = t[kTssAddressV] = t[kTssAddressW] = 1;
        t[kTssMagFilter] = t[kTssMinFilter] = 1;
        t[kTssMaxAnisotropy] = 1;
        t[kTssColorArg0] = t[kTssAlphaArg0] = t[kTssResultArg] = 1;
    }
    transforms_.clear();
    for (Matrix& m : transform_low_) m = Matrix{};
    for (Matrix& m : transform_world_) m = Matrix{};
    material_ = {};
    lights_.clear();
    light_enabled_.clear();
    reset_viewport_to_target();
    ++program_version_;
    ++light_version_;
    ++draw_version_;
}

void Device::reset_viewport_to_target() {
    viewport_ = {0, 0, render_target_ ? render_target_->width : 1, render_target_ ? render_target_->height : 1, 0.0f, 1.0f};
    ++frame_version_;
}

// ---------------------------------------------------------------------------
// device queries
// ---------------------------------------------------------------------------
HRESULT Device::GetDirect3D(void** out) {
    if (!out) return kInvalidCall;
    d3d_->AddRef();
    *out = static_cast<IDirect3D8*>(d3d_);
    return kOk;
}
HRESULT Device::GetDeviceCaps(void* caps) { return d3d_->GetDeviceCaps(adapter_, device_type_, caps); }
HRESULT Device::GetDisplayMode(void* mode) { return d3d_->GetAdapterDisplayMode(adapter_, mode); }
HRESULT Device::GetCreationParameters(void* params) {
    if (!params) return kInvalidCall;
    auto* p = static_cast<CreationParameters*>(params);
    p->adapter = adapter_;
    p->device_type = device_type_;
    p->focus_window = window_;
    p->behavior = behavior_;
    return kOk;
}
HRESULT Device::GetRasterStatus(void* status) {
    if (!status) return kInvalidCall;
    *static_cast<RasterStatus*>(status) = {};
    return kOk;
}
void Device::SetGammaRamp(DWORD, const void* ramp) {
    if (ramp) std::memcpy(&gamma_, ramp, sizeof gamma_);
}
void Device::GetGammaRamp(void* ramp) {
    if (!ramp) return;
    bool identity = true;
    for (int i = 0; i < 256 && identity; ++i) identity = gamma_.red[i] == 0;
    if (identity)
        for (int i = 0; i < 256; ++i)
            gamma_.red[i] = gamma_.green[i] = gamma_.blue[i] = static_cast<WORD>(i * 257);
    std::memcpy(ramp, &gamma_, sizeof gamma_);
}
HRESULT Device::ValidateDevice(DWORD* passes) {
    if (passes) *passes = 1;
    return kOk;
}

// ---------------------------------------------------------------------------
// cursor
// ---------------------------------------------------------------------------
HRESULT Device::SetCursorProperties(UINT x, UINT y, IDirect3DSurface8* bitmap) {
    auto* surface = static_cast<Surface*>(bitmap);
    if (!surface || surface->format != kFmtA8R8G8B8) return kInvalidCall;
    std::vector<std::uint8_t> pixels;
    if (!surface->read_pixels(pixels)) return kInvalidCall;
    if (cursor_texture_ && (cursor_width_ != surface->width || cursor_height_ != surface->height)) {
        glDeleteTextures(1, &cursor_texture_);
        cursor_texture_ = 0;
    }
    if (!cursor_texture_) {
        glCreateTextures(GL_TEXTURE_2D, 1, &cursor_texture_);
        glTextureStorage2D(cursor_texture_, 1, GL_RGBA8, static_cast<GLsizei>(surface->width),
                           static_cast<GLsizei>(surface->height));
        glTextureParameteri(cursor_texture_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(cursor_texture_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(cursor_texture_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(cursor_texture_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTextureSubImage2D(cursor_texture_, 0, 0, 0, static_cast<GLsizei>(surface->width),
                        static_cast<GLsizei>(surface->height), GL_BGRA, GL_UNSIGNED_BYTE, pixels.data());
    cursor_width_ = surface->width;
    cursor_height_ = surface->height;
    cursor_hot_x_ = x;
    cursor_hot_y_ = y;
    return kOk;
}

void Device::SetCursorPosition(int x, int y, DWORD) {
    // Full screen: screen coordinates are back-buffer pixels. Windowed: screen -> client.
    if (pp_.windowed) {
        POINT p = {x, y};
#ifdef _WIN32
        ScreenToClient(window_, &p);
#else
        static const auto to_client =
            loader::override_function<decltype(&::ScreenToClient)>("USER32.dll", "ScreenToClient");  // sdl_input.cpp's
        if (to_client) to_client(window_, &p);
#endif
        x = p.x;
        y = p.y;
    }
    cursor_x_ = x;
    cursor_y_ = y;
}

BOOL Device::ShowCursor(BOOL show) {
    const BOOL previous = cursor_visible_;
    cursor_visible_ = show;
    return previous;
}

// Textured-quad program shared by the cursor and the developer menu.
bool Device::ensure_cursor_program() {
    if (cursor_program_) return true;
    static const char* const vs =
        "#version 450 core\n"
        "uniform vec4 u_rect;  // x0, y0, x1, y1 in NDC\n"
        "out vec2 v_uv;\n"
        "void main() {\n"
        "    vec2 c = vec2(gl_VertexID & 1, gl_VertexID >> 1);\n"
        "    v_uv = c;\n"
        "    gl_Position = vec4(mix(u_rect.xy, u_rect.zw, c), 0.0, 1.0);\n"
        "}\n";
    static const char* const fs =
        "#version 450 core\n"
        "layout(binding = 0) uniform sampler2D t;\n"
        "in vec2 v_uv;\n"
        "layout(location = 0) out vec4 color;\n"
        "void main() { color = texture(t, v_uv); }\n";
    cursor_program_ = renderer_.compile_program(vs, fs, "cursor");  // owned by the device, not cached
    if (!cursor_program_) return false;
    cursor_rect_location_ = glGetUniformLocation(cursor_program_, "u_rect");
    glCreateVertexArrays(1, &cursor_vao_);
    return true;
}

void Device::draw_cursor(int ox, int oy, double scale, int window_w, int window_h) {
    if (!cursor_visible_ || !cursor_texture_ || window_w <= 0 || window_h <= 0) return;
    if (!ensure_cursor_program()) {
        cursor_visible_ = FALSE;
        return;
    }
    // Back-buffer pixels: on a 1024x768 screen the game's cursor is in layout coordinates.
    double bx = cursor_x_, by = cursor_y_;
    int ux = 0, uy = 0;
    if (display::canvas_armed()) {
        const auto& c = display::canvas();
        bx = bx * c.scale + c.x;
        by = by * c.scale + c.y;
    } else if (display::ui_cursor(ux, uy)) {
        bx = ux;  // UiScale: the game's position is remapped onto the HUD; draw the real one
        by = uy;
    }
    // Window pixels, top-down; the cursor image is enlarged by UiScale.
    const double cs = scale * display::ui_scale();
    const double x0 = ox + bx * scale - static_cast<int>(cursor_hot_x_) * cs;
    const double y0 = oy + by * scale - static_cast<int>(cursor_hot_y_) * cs;
    const double x1 = x0 + cursor_width_ * cs, y1 = y0 + cursor_height_ * cs;
    // With glClipControl(GL_UPPER_LEFT) NDC y = -1 is the window's top row.
    const float rect[4] = {static_cast<float>(2.0 * x0 / window_w - 1.0), static_cast<float>(2.0 * y0 / window_h - 1.0),
                           static_cast<float>(2.0 * x1 / window_w - 1.0), static_cast<float>(2.0 * y1 / window_h - 1.0)};
    glViewport(0, 0, window_w, window_h);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColorMask(1, 1, 1, 1);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(cursor_program_);
    glProgramUniform4fv(cursor_program_, cursor_rect_location_, 1, rect);
    glBindVertexArray(cursor_vao_);
    glBindTextureUnit(0, cursor_texture_);
    glBindSampler(0, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    renderer_.invalidate_state();
}

// Developer menu: the BGRA image from debug_menu.cpp, top-left of the window, enlarged on
// tall windows so it stays readable (x1 up to 1080 rows, x2 at 2160, ...).
void Device::draw_debug_menu(int window_w, int window_h) {
    const debugmenu::Overlay& m = debugmenu::overlay();
    if (!m.visible || !m.pixels || m.width <= 0 || m.height <= 0 || window_w <= 0 || window_h <= 0) return;
    if (!ensure_cursor_program()) return;  // shares the textured-quad program of the cursor
    if (menu_texture_ && (menu_width_ != m.width || menu_height_ != m.height)) {
        glDeleteTextures(1, &menu_texture_);
        menu_texture_ = 0;
    }
    if (!menu_texture_) {
        glCreateTextures(GL_TEXTURE_2D, 1, &menu_texture_);
        glTextureStorage2D(menu_texture_, 1, GL_RGBA8, m.width, m.height);
        glTextureParameteri(menu_texture_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(menu_texture_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(menu_texture_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(menu_texture_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        menu_width_ = m.width;
        menu_height_ = m.height;
        menu_version_ = m.version - 1;
    }
    if (menu_version_ != m.version) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTextureSubImage2D(menu_texture_, 0, 0, 0, m.width, m.height, GL_BGRA, GL_UNSIGNED_BYTE, m.pixels);
        menu_version_ = m.version;
    }
    const double s = std::max(1.0, std::floor(window_h / 1080.0 + 0.5));
    const double x0 = 16 * s, y0 = 16 * s, x1 = x0 + m.width * s, y1 = y0 + m.height * s;
    const float rect[4] = {static_cast<float>(2.0 * x0 / window_w - 1.0), static_cast<float>(2.0 * y0 / window_h - 1.0),
                           static_cast<float>(2.0 * x1 / window_w - 1.0), static_cast<float>(2.0 * y1 / window_h - 1.0)};
    glViewport(0, 0, window_w, window_h);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColorMask(1, 1, 1, 1);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(cursor_program_);
    glProgramUniform4fv(cursor_program_, cursor_rect_location_, 1, rect);
    glBindVertexArray(cursor_vao_);
    glBindTextureUnit(0, menu_texture_);
    glBindSampler(0, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    renderer_.invalidate_state();
}

// ---------------------------------------------------------------------------
// presentation
// ---------------------------------------------------------------------------
HRESULT Device::Reset(PresentParameters* pp) {
    if (!pp) return kInvalidCall;
    pp_ = *pp;
    if (render_target_) std::exchange(render_target_, nullptr)->Release();
    if (depth_stencil_) std::exchange(depth_stencil_, nullptr)->Release();
    if (back_buffer_) std::exchange(back_buffer_, nullptr)->Release();
    if (auto_depth_) std::exchange(auto_depth_, nullptr)->Release();
    create_back_buffer();
    set_defaults();
    return kOk;
}

void Device::apply_live_settings() {
    const auto& s = hook::settings();
    int fps = s.max_fps;
    if (fps < 0) {
        fps = platform::refresh_rate(window_);
        if (fps <= 1) fps = 60;
    }
    frame_interval_ = fps > 0 ? 1.0 / fps : 0.0;
    platform::gl_set_swap_interval(s.vsync);
    renderer_.set_texture_quality(s.anisotropy >= 2 ? static_cast<std::uint32_t>(s.anisotropy) : 0u, s.texture_filter == 1);
    hook::log_line("OpenGL device: options menu - frame limiter %s, vsync %d, anisotropy %u, trilinear %s",
                   fps > 0 ? (std::to_string(fps) + " fps").c_str() : "off", s.vsync, renderer_.anisotropy(), s.texture_filter == 1 ? "on" : "off");
}

void apply_live_settings() {
    if (g_ao_device != nullptr) g_ao_device->apply_live_settings();
}

void Device::pace_frame() {
    std::int64_t now = platform::perf_counter();
    if (frame_interval_ > 0 && last_present_ && !replay::unpaced()) {  // ReplayPace=0: as fast as the machine runs
        // Wait out the rest of the frame (the game's animations advance by whole milliseconds per frame).
        const auto target = last_present_ + static_cast<std::int64_t>(frame_interval_ * static_cast<double>(perf_freq_));
        while (now < target) {
            const double left_ms = static_cast<double>(target - now) * 1000.0 / static_cast<double>(perf_freq_);
            if (left_ms > 2.0) platform::sleep_ms(1);
            else joan::cpu::pause();
            now = platform::perf_counter();
        }
    }
    last_present_ = now;
    // Frame rate in the log (first minutes only).
    if (!fps_window_start_) fps_window_start_ = now;
    if (++fps_window_frames_ == 1000) {
        const double seconds = static_cast<double>(now - fps_window_start_) / static_cast<double>(perf_freq_);
        if (fps_reports_++ < 10) hook::log_line("OpenGL device: %.1f fps (frames %u-%u)", 1000.0 / seconds, frame_ - 999, frame_);
        fps_window_frames_ = 0;
        fps_window_start_ = now;
    }
}

// The 1024x768 screens are now scaled where their sprites are queued (display::canvas_sprite), because
// the game often draws the queue after the screen's draw function has returned. The clip-space canvas
// transform (u_canvas) stays available but is not used.
bool Device::canvas_on() const { return false; }

bool Device::layout_viewport() const {
    // While a 1024x768 screen is shown, a viewport other than the whole back buffer belongs to one
    // of its cameras and is given in layout pixels (all such viewports lie inside 1024x768).
    if (!display::canvas_armed() || render_target_ != back_buffer_) return false;
    if (viewport_.x == 0 && viewport_.y == 0 && viewport_.width == back_buffer_->width && viewport_.height == back_buffer_->height)
        return false;
    return viewport_.x + viewport_.width <= 1024 && viewport_.y + viewport_.height <= 768;
}

// The current camera's viewport in back-buffer pixels (top row 0, inside the back buffer): a camera of a
// 1024x768 screen (layout_viewport: the loading view, the mission-brief map) as apply_raster_state places
// it on the canvas, any other as given. For the screen-space passes (occlusion, shadows, water).
void Device::scene_viewport(int& x, int& y, int& w, int& h) const {
    const int bw = back_buffer_ ? static_cast<int>(back_buffer_->width) : 0, bh = back_buffer_ ? static_cast<int>(back_buffer_->height) : 0;
    if (layout_viewport()) {
        const D3DRect v = canvas_rect({static_cast<LONG>(viewport_.x), static_cast<LONG>(viewport_.y),
                                       static_cast<LONG>(viewport_.x + viewport_.width), static_cast<LONG>(viewport_.y + viewport_.height)});
        x = static_cast<int>(v.x1);
        y = static_cast<int>(v.y1);
        w = static_cast<int>(v.x2 - v.x1);
        h = static_cast<int>(v.y2 - v.y1);
    } else {
        x = static_cast<int>(viewport_.x);
        y = static_cast<int>(viewport_.y);
        w = static_cast<int>(viewport_.width);
        h = static_cast<int>(viewport_.height);
    }
    w = std::min(w, bw - x);
    h = std::min(h, bh - y);
}

D3DRect Device::canvas_rect(const D3DRect& r) const {
    const auto& c = display::canvas();
    return {static_cast<LONG>(std::floor(static_cast<float>(r.x1) * c.scale + c.x)),
            static_cast<LONG>(std::floor(static_cast<float>(r.y1) * c.scale + c.y)),
            static_cast<LONG>(std::ceil(static_cast<float>(r.x2) * c.scale + c.x)),
            static_cast<LONG>(std::ceil(static_cast<float>(r.y2) * c.scale + c.y))};
}

void Device::clear_canvas_bars() {
    const D3DRect c = canvas_rect({0, 0, 1024, 768});
    const LONG w = static_cast<LONG>(back_buffer_->width), h = static_cast<LONG>(back_buffer_->height);
    const D3DRect bars[] = {{0, 0, c.x1, h}, {c.x2, 0, w, h}, {c.x1, 0, c.x2, c.y1}, {c.x1, c.y2, c.x2, h}};
    const GLuint fbo = framebuffer_for(back_buffer_, nullptr);
    const float black[4] = {0, 0, 0, 1};
    // Bind it: the AMD driver ignored the scissor for a clear of a framebuffer that is not bound and
    // wiped the whole picture (the menu showed only on frames where a read-back had synchronised first).
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glEnable(GL_SCISSOR_TEST);
    glColorMask(1, 1, 1, 1);
    for (const D3DRect& b : bars) {
        if (b.x2 <= b.x1 || b.y2 <= b.y1) continue;
        glScissor(b.x1, b.y1, b.x2 - b.x1, b.y2 - b.y1);
        glClearNamedFramebufferfv(fbo, GL_COLOR, 0, black);
    }
    glDisable(GL_SCISSOR_TEST);
    renderer_.invalidate_state();
}

HRESULT Device::Present(const void*, const void*, HWND, const void*) {
    renderer_.flush_queued();  // [Graphics] GrassBatching: queued replays first
    ao_close_window();  // [Graphics] AmbientOcclusion: a scene draw that never reached its pass-2 end
    shadow_recording_ = false;  // [Graphics] Shadows=2: likewise
    overlay_valid_ = overlay_begun_ = false;  // render::overlay_begin / end of the next frame copy the depth again
    ++presented_;
    ++stream_cache_api::g_frame;  // the stream cache verifies an array once per frame
    foliage_recording_ = false;
    static bool first = true;
    if (first) {
        first = false;
        hook::log_line("OpenGL device: first frame presented");
    }
    const GLuint fbo = framebuffer_for(back_buffer_, nullptr);
    int cw = 0, ch = 0;
    platform::client_size(window_, cw, ch);
    const int bw = static_cast<int>(back_buffer_->width), bh = static_cast<int>(back_buffer_->height);
    ++frame_;
    if (hook::settings().gl_debug && display::canvas_armed() && canvas_frames_logged_ < 6) {
        ++canvas_frames_logged_;
        hook::log_line("OpenGL device: frame %u with a 1024x768 screen: %u canvas draws, %u other draws, %u clears", frame_,
                       canvas_draws_, plain_draws_, stats_.clears);
    }
    display::set_scene_bottom(drew_3d_ ? static_cast<int>(scene_bottom_) : 0);
    scene_bottom_ = 0;
    display::canvas_end_frame();
    canvas_frame_index_ = display::canvas_armed() ? canvas_frame_index_ + 1 : 0;
    canvas_dumps_ = 0;
    const bool probe_frame = hook::settings().gl_debug && (canvas_frame_index_ == 300 || canvas_frame_index_ == 1000);
    if (probe_frame) {
        glBindFramebuffer(GL_FRAMEBUFFER, resolve_multisample(back_buffer_));  // = framebuffer_for unless MSAA
        probe_pixels("of the back buffer at Present");
    }
    // A 1024x768 screen without the 3D world behind it: black bars around it.
    // ... or the world still drawn behind an opening menu (its clear colour showed in the bars).
    // A full screen drawn this frame; one that closed last frame only while no world is drawn (the world
    // after the loading screen is not boxed in for the slow first frames of a level).
    if ((display::canvas_armed() && !drew_3d_) || display::opaque_screen_active() || (display::opaque_screen_shown() && !drew_3d_))
        clear_canvas_bars();
    // The level's first frames after the loading screen: black (not the half-ready world) while they are
    // still slow (shaders, textures and lightmaps being prepared) or the loading screen's state lingers,
    // at most 3 seconds; then the game's own fade-in shows the level.
    {
        const auto now = std::chrono::steady_clock::now();
        if (drew_3d_ && display::loading_just_ended()) {
            if (!loading_black_) {
                loading_black_ = true;
                loading_black_start_ = now;
            }
            // Settled: fast, the loading screen's state gone, the shadow map intact (the first frames still
            // stream so much that it is skipped in some of them - shadows blinking at a cutscene's start).
            const bool slow = now - last_present_time_ > std::chrono::milliseconds(150);
            const bool settled = !slow && !display::canvas_armed() && !shadows_skipped_;
            loading_good_frames_ = settled ? loading_good_frames_ + 1 : 0;
            if (loading_good_frames_ < 3 && now - loading_black_start_ < std::chrono::seconds(5)) {
                const float black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
                glDisable(GL_SCISSOR_TEST);
                glColorMask(1, 1, 1, 1);
                glClearNamedFramebufferfv(framebuffer_for(back_buffer_, nullptr), GL_COLOR, 0, black);
            } else {
                display::loading_done();
                loading_good_frames_ = 0;
                loading_black_ = false;
            }
        }
        last_present_time_ = now;
        shadows_skipped_ = false;
        renderer_.clear_deleted_buffers();
    }
    drew_3d_ = false;
    if (probe_frame) probe_pixels("of the back buffer after the bars");
    canvas_draws_ = plain_draws_ = 0;
    const bool diagnostics = hook::settings().gl_debug;
    if (diagnostics) frame_diagnostics(cw, ch);
    stats_ = {};
    frame_draws_dumped_ = 0;
    if (cw > 0 && ch > 0) {
        const double scale = std::min(static_cast<double>(cw) / bw, static_cast<double>(ch) / bh);
        const int dw = static_cast<int>(bw * scale + 0.5), dh = static_cast<int>(bh * scale + 0.5);
        const int ox = (cw - dw) / 2, oy = (ch - dh) / 2;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_SCISSOR_TEST);
        glColorMask(1, 1, 1, 1);
        const float black[4] = {0, 0, 0, 1};
        glClearNamedFramebufferfv(0, GL_COLOR, 0, black);
        // [Graphics] MSAA: a multisampled framebuffer cannot be blitted with scaling, so its samples
        // are first resolved 1:1 into the back buffer's texture (after the canvas bars, before the
        // cursor / developer menu, which are drawn into the window afterwards). Without MSAA `src`
        // is `fbo`, the back buffer itself.
        const GLuint src = back_buffer_->gl_msaa ? resolve_multisample(back_buffer_) : fbo;
        // Our image is stored top row first; the window's framebuffer bottom row first.
        glBlitNamedFramebuffer(src, 0, 0, 0, bw, bh, ox, oy + dh, ox + dw, oy, GL_COLOR_BUFFER_BIT,
                               (dw == bw && dh == bh) ? GL_NEAREST : GL_LINEAR);
        draw_cursor(ox, oy, scale, cw, ch);
        draw_debug_menu(cw, ch);
        // Screenshot key: the picture as shown (without the black bars), as openjoan_shotNNN.bmp.
        const int key = hook::settings().screenshot_key;
        const bool down = key > 0 && key < 256 && platform::key_down(key) && platform::window_focused(window_);
        if (down && !screenshot_key_down_) {
            std::vector<std::uint8_t> pixels(static_cast<std::size_t>(dw) * static_cast<std::size_t>(dh) * 4);
            glNamedFramebufferReadBuffer(0, GL_BACK);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glReadPixels(ox, ch - oy - dh, dw, dh, GL_BGRA, GL_UNSIGNED_BYTE, pixels.data());
            for (std::size_t i = 3; i < pixels.size(); i += 4) pixels[i] = 0xff;
            std::string path;
            for (int n = 1; n < 10000; ++n) {
                char name[32];
                std::snprintf(name, sizeof name, "openjoan_shot%03d.bmp", n);
                path = hook::data_dir() + name;
#ifdef _WIN32
                if (GetFileAttributesA(path.c_str()) == INVALID_FILE_ATTRIBUTES) break;
#else
                std::error_code ec;
                if (!std::filesystem::exists(path, ec)) break;
#endif
            }
            write_bmp(path, pixels.data(), dw, dh, false);
            hook::log_line("OpenGL device: screenshot %dx%d written to %s", dw, dh, path.c_str());
        }
        screenshot_key_down_ = down;
        if (diagnostics && (frame_ == 100 || frame_ == 1000)) {  // what reached the window
            std::vector<std::uint8_t> pixels(static_cast<std::size_t>(cw) * static_cast<std::size_t>(ch) * 4);
            glNamedFramebufferReadBuffer(0, GL_BACK);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glReadPixels(0, 0, cw, ch, GL_BGRA, GL_UNSIGNED_BYTE, pixels.data());
            const std::string path = hook::data_dir() + "openjoan_frame" + std::to_string(frame_) + "_window.bmp";
            write_bmp(path, pixels.data(), cw, ch, false);
            hook::log_line("OpenGL device: window contents of frame %u written to %s", frame_, path.c_str());
        }
        const auto swap_start = std::chrono::steady_clock::now();
        gl::swap_buffers(ctx_);
        pace_frame();
        perf_.swap += std::chrono::steady_clock::now() - swap_start;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, current_fbo_);
    renderer_.invalidate_state();
    perf_frame();
    hitch::frame_end();  // [Debug] HitchLog
    return kOk;
}

// [Graphics] PerfLog=N: every N seconds, where the frame time went (the game's Draw* calls on the
// CPU, the buffer swap / frame limiter, everything else = the game's own work) and the GL traffic.
void Device::perf_frame() {
    const int every = hook::settings().perf_log;
    render::RendererStats& r = renderer_.stats();
    perf_.draws += perf_.frame_draws;
    perf_.frame_draws = 0;
    perf_.raster_applied += r.raster_applied, perf_.raster_skipped += r.raster_skipped;
    perf_.uniform_uploads += r.uniform_uploads, perf_.uniform_skipped += r.uniform_skipped, perf_.ring_waits += r.ring_waits;
    perf_.program_binds += r.program_binds, perf_.geometry_binds += r.geometry_binds, perf_.texture_binds += r.texture_binds;
    r = {};
    ++perf_.frames;
    const auto now = std::chrono::steady_clock::now();
    const std::uint64_t now_ticks = joan::cpu::ticks();
    if (perf_.frames == 1) perf_.start = now, perf_.start_ticks = now_ticks;
    if (every <= 0) {
        perf_ = {};
        return;
    }
    const double total = std::chrono::duration<double>(now - perf_.start).count();
    if (total < every || perf_.frames < 2) return;
    const double n = static_cast<double>(perf_.frames - 1);  // intervals since `start`
    auto ms = [n](std::chrono::steady_clock::duration d) { return std::chrono::duration<double, std::milli>(d).count() / n; };
    const double ticks = static_cast<double>(now_ticks - perf_.start_ticks);
    const double draw_ms = ticks > 0.0 ? total * 1000.0 * static_cast<double>(perf_.draw_ticks) / ticks / n : 0.0;
    const double frame_ms = total * 1000.0 / n, swap_ms = ms(perf_.swap);
    auto per = [n](unsigned long long v) { return static_cast<unsigned>(static_cast<double>(v) / n + 0.5); };
    BufferStats& b = buffer_stats();
    hook::log_line("[Graphics] PerfLog: %.1f fps, frame %.2f ms = Draw* calls %.2f + swap / limiter %.2f + game %.2f | per frame: %u draws, "
                   "raster %u set / %u same, uniforms %u uploaded / %u same, %u programs, %u geometry, %u textures, %u ring waits | "
                   "buffer writes: %u streamed (%u ring waits), %u renamed, %u appended, %u in place, %u waited, %u promoted, %u new copies | shadow map: %u draws (recorded %u + %u foliage; skipped: %u blended, %u other target, %u no depth write, %u no colour, %u 2D) (%.1f s)",
                   1000.0 / frame_ms, frame_ms, draw_ms, swap_ms, std::max(0.0, frame_ms - draw_ms - swap_ms), per(perf_.draws),
                   per(perf_.raster_applied), per(perf_.raster_skipped), per(perf_.uniform_uploads), per(perf_.uniform_skipped),
                   per(perf_.program_binds), per(perf_.geometry_binds), per(perf_.texture_binds), per(perf_.ring_waits),
                   per(b.streamed), b.stream_waits, per(b.renames), per(b.appends), per(b.in_place), per(b.waits), b.promoted, b.copies_made, per(shadow_.take_drawn()), per(shadow_stats_.recorded), per(shadow_stats_.foliage), per(shadow_stats_.blended), per(shadow_stats_.other_target),
                   per(shadow_stats_.no_depth), per(shadow_stats_.no_colour), per(shadow_stats_.two_d), total);
    shadow_stats_ = {};
    b = {};
    perf_ = {};
    perf_.frames = 1;
    perf_.start = now;
    perf_.start_ticks = now_ticks;
}

void Device::dump_draw(Enum prim, UINT start, UINT prims, bool indexed, GLuint program) {
    // The first 40 draws of the session and the first 25 of frames 100 and 1000.
    const bool special = frame_ + 1 == 100 || frame_ + 1 == 1000;
    // While a 1024x768 screen is shown: the first 25 draws of its frames 1, 2, 60, 300 and 1000.
    const unsigned cf = canvas_frame_index_ + 1;
    const bool canvas_window = hook::settings().gl_debug && display::canvas_armed() && canvas_dumps_ < 25 &&
                               (cf == 1 || cf == 2 || cf == 60 || cf == 300 || cf == 1000);
    if (canvas_window) {
        ++canvas_dumps_;
        probe_after_draw_ = cf >= 60;
        hook::log_line("draw | canvas %s: scale %.4f offset %.1f,%.1f -> u_canvas %.4f %.4f %.4f %.4f, target %ux%u%s",
                       canvas_on() ? "ON" : "off", display::canvas().scale, display::canvas().x, display::canvas().y,
                       uniforms_.canvas[0], uniforms_.canvas[1], uniforms_.canvas[2], uniforms_.canvas[3],
                       render_target_ ? render_target_->width : 0, render_target_ ? render_target_->height : 0,
                       render_target_ == back_buffer_ ? " (back buffer)" : "");
    } else if (!hook::settings().gl_debug || (draws_dumped_ >= 40 && !(special && frame_draws_dumped_ < 25))) {
        return;
    }
    ++draws_dumped_;
    ++frame_draws_dumped_;
    const unsigned frame = frame_ + 1;
    auto f = [](DWORD v) { return static_cast<double>(as_float(v)); };
    hook::log_line("draw | frame %u: %s prim %u, start %u, %u prims, %s 0x%x stride %u, program %u", frame,
                   indexed ? "DrawIndexedPrimitive" : "DrawPrimitive", prim, start, prims,
                   (vertex_shader_ & 1) ? "shader" : "FVF", vertex_shader_, stride_, program);
    hook::log_line("draw |   rs: lighting %u ambient 0x%08x colorvertex %u diffsrc %u ambsrc %u emisrc %u | fog %u "
                   "(table %u vertex %u range %u, %g..%g, colour 0x%08x) | z %u write %u func %u | cull %u | "
                   "blend %u (%u,%u op %u) | alphatest %u (func %u ref %u) | stencil %u | colorwrite 0x%x | "
                   "tfactor 0x%08x | specular %u | vertexblend %u",
                   rs_[kRsLighting], rs_[kRsAmbient], rs_[kRsColorVertex], rs_[kRsDiffuseMaterialSource],
                   rs_[kRsAmbientMaterialSource], rs_[kRsEmissiveMaterialSource], rs_[kRsFogEnable],
                   rs_[kRsFogTableMode], rs_[kRsFogVertexMode], rs_[kRsRangeFogEnable], f(rs_[kRsFogStart]),
                   f(rs_[kRsFogEnd]), rs_[kRsFogColor], rs_[kRsZEnable], rs_[kRsZWriteEnable], rs_[kRsZFunc],
                   rs_[kRsCullMode], rs_[kRsAlphaBlendEnable], rs_[kRsSrcBlend], rs_[kRsDestBlend], rs_[kRsBlendOp],
                   rs_[kRsAlphaTestEnable], rs_[kRsAlphaFunc], rs_[kRsAlphaRef], rs_[kRsStencilEnable],
                   rs_[kRsColorWriteEnable], rs_[kRsTextureFactor], rs_[kRsSpecularEnable], rs_[kRsVertexBlend]);
    for (DWORD st = 0; st < 2; ++st) {
        const DWORD* t = tss_[st];
        char tex[160] = "none";
        if (Texture* x = textures_[st]) {
            std::snprintf(tex, sizeof tex, "%ux%u format %u, %u levels, level 0 %s", x->width(), x->height(), x->format(),
                          x->levels(), x->uploaded(0) ? "uploaded" : "NEVER UPLOADED");
            if (!x->info().compressed && x->uploaded(0)) {
                std::vector<std::uint8_t> px;
                if (x->level_surface(0)->read_pixels(px) && !px.empty()) {
                    const std::size_t bpp = x->info().bytes;
                    const std::size_t mid = (static_cast<std::size_t>(x->height() / 2) * x->width() + x->width() / 2) * bpp;
                    std::uint32_t v = 0;
                    if (mid + bpp <= px.size()) std::memcpy(&v, &px[mid], bpp);
                    unsigned nonzero = 0;
                    for (std::uint8_t b : px) nonzero += b != 0;
                    const std::size_t len = std::strlen(tex);
                    std::snprintf(tex + len, sizeof tex - len, ", centre texel 0x%x, %u%% non-zero bytes", v,
                                  static_cast<unsigned>(100.0 * nonzero / static_cast<double>(px.size())));
                }
            }
        }
        hook::log_line("draw |   stage %u: colour op %u (%u, %u) alpha op %u (%u, %u) tci 0x%x ttf 0x%x "
                       "addr %u/%u filter %u/%u/%u | texture %s",
                       st, t[kTssColorOp], t[kTssColorArg1], t[kTssColorArg2], t[kTssAlphaOp], t[kTssAlphaArg1],
                       t[kTssAlphaArg2], t[kTssTexCoordIndex], t[kTssTextureTransformFlags], t[kTssAddressU],
                       t[kTssAddressV], t[kTssMagFilter], t[kTssMinFilter], t[kTssMipFilter], tex);
    }
    hook::log_line("draw |   viewport %u,%u %ux%u z %g..%g | material diffuse %g %g %g %g ambient %g %g %g emissive %g %g %g",
                   viewport_.x, viewport_.y, viewport_.width, viewport_.height, static_cast<double>(viewport_.min_z),
                   static_cast<double>(viewport_.max_z), static_cast<double>(material_.diffuse.r),
                   static_cast<double>(material_.diffuse.g), static_cast<double>(material_.diffuse.b),
                   static_cast<double>(material_.diffuse.a), static_cast<double>(material_.ambient.r),
                   static_cast<double>(material_.ambient.g), static_cast<double>(material_.ambient.b),
                   static_cast<double>(material_.emissive.r), static_cast<double>(material_.emissive.g),
                   static_cast<double>(material_.emissive.b));
    const Matrix& world = transform(kTsWorld);
    const Matrix& view = transform(kTsView);
    const Matrix& proj = transform(kTsProjection);
    auto mat = [](const char* name, const Matrix& m) {
        hook::log_line("draw |   %s [%g %g %g %g | %g %g %g %g | %g %g %g %g | %g %g %g %g]", name,
                       static_cast<double>(m.m[0]), static_cast<double>(m.m[1]), static_cast<double>(m.m[2]),
                       static_cast<double>(m.m[3]), static_cast<double>(m.m[4]), static_cast<double>(m.m[5]),
                       static_cast<double>(m.m[6]), static_cast<double>(m.m[7]), static_cast<double>(m.m[8]),
                       static_cast<double>(m.m[9]), static_cast<double>(m.m[10]), static_cast<double>(m.m[11]),
                       static_cast<double>(m.m[12]), static_cast<double>(m.m[13]), static_cast<double>(m.m[14]),
                       static_cast<double>(m.m[15]));
    };
    if (!(vertex_shader_ & 1)) {
        mat("world", world);
        mat("view", view);
        mat("proj", proj);
    }
    // The first vertices as the GPU sees them, and where the fixed-function transform puts them.
    std::vector<std::uint32_t> indices;
    if (indexed && indices_) {
        const bool wide = indices_->format() == kFmtIndex32;
        const UINT n = std::min<UINT>(vertex_count(prim, prims), 6);
        std::vector<std::uint8_t> raw(static_cast<std::size_t>(n) * (wide ? 4 : 2));
        glGetNamedBufferSubData(indices_->gl(), indices_->offset() + static_cast<GLintptr>(start) * (wide ? 4 : 2),
                                static_cast<GLsizeiptr>(raw.size()), raw.data());
        for (UINT i = 0; i < n; ++i) {
            std::uint32_t v = 0;
            std::memcpy(&v, &raw[static_cast<std::size_t>(i) * (wide ? 4 : 2)], wide ? 4 : 2);
            indices.push_back(v + base_vertex_);
        }
    } else {
        for (UINT i = 0; i < std::min<UINT>(vertex_count(prim, prims), 6); ++i) indices.push_back(start + i);
    }
    const Matrix wvp = multiply(multiply(world, view), proj);
    const DWORD fvf = vertex_shader_;
    const bool rhw = !(fvf & 1) && (fvf & kFvfPositionMask) == kFvfXyzRhw;
    for (std::uint32_t index : indices) {
        std::uint8_t v[64] = {};
        glGetNamedBufferSubData(stream_->gl(), stream_->offset() + static_cast<GLintptr>(index) * static_cast<GLintptr>(stride_),
                                static_cast<GLsizeiptr>(std::min<UINT>(stride_, 64)), v);
        float p[4] = {};
        std::memcpy(p, v, 16);
        char extra[200] = "";
        if (!(fvf & 1)) {
            std::size_t off = rhw ? 16 : 12;
            if ((fvf & kFvfPositionMask) >= kFvfXyzB1 && !rhw) off += ((fvf & kFvfPositionMask) - kFvfXyzB1) / 2 * 4 + 4;
            if (fvf & kFvfNormal) off += 12;
            if (fvf & kFvfPSize) off += 4;
            DWORD diffuse = 0xffffffff;
            if ((fvf & kFvfDiffuse) && off + 4 <= 64) {
                std::memcpy(&diffuse, v + off, 4);
                off += 4;
            }
            if (fvf & kFvfSpecular) off += 4;
            float uv[2] = {};
            if (((fvf & kFvfTexCountMask) >> kFvfTexCountShift) && off + 8 <= 64) std::memcpy(uv, v + off, 8);
            if (rhw) {
                std::snprintf(extra, sizeof extra, " diffuse 0x%08x uv %g,%g", diffuse, static_cast<double>(uv[0]),
                              static_cast<double>(uv[1]));
            } else {
                float c[4];
                for (int j = 0; j < 4; ++j)
                    c[j] = p[0] * wvp.m[j] + p[1] * wvp.m[4 + j] + p[2] * wvp.m[8 + j] + wvp.m[12 + j];
                const float w = c[3] != 0 ? c[3] : 1.0f;
                std::snprintf(extra, sizeof extra, " diffuse 0x%08x uv %g,%g -> clip %g %g %g %g -> ndc %g %g z %g",
                              diffuse, static_cast<double>(uv[0]), static_cast<double>(uv[1]), static_cast<double>(c[0]),
                              static_cast<double>(c[1]), static_cast<double>(c[2]), static_cast<double>(c[3]),
                              static_cast<double>(c[0] / w), static_cast<double>(c[1] / w), static_cast<double>(c[2] / w));
            }
        }
        hook::log_line("draw |   vertex %u: %g %g %g %g%s", index, static_cast<double>(p[0]), static_cast<double>(p[1]),
                       static_cast<double>(p[2]), static_cast<double>(p[3]), extra);
    }
}

void Device::frame_diagnostics(int window_w, int window_h) {
    if (logged_frame(frame_)) {
        const unsigned failed = renderer_.failed_program_count();
        const GLenum error = glGetError();
        hook::log_line("OpenGL device: frame %u: %u draws (skipped: %u no program, %u no stream, %u no indices, "
                       "%u other thread), %u clears; %u programs (%u failed), %u textures bound; render target %ux%u%s, "
                       "viewport %u,%u %ux%u; window %dx%d; glGetError 0x%x",
                       frame_, stats_.draws, stats_.no_program, stats_.no_stream, stats_.no_indices, stats_.no_thread,
                       stats_.clears, static_cast<unsigned>(renderer_.program_count()), failed,
                       static_cast<unsigned>(std::count_if(std::begin(textures_), std::end(textures_),
                                                           [](const Texture* t) { return t != nullptr; })),
                       render_target_ ? render_target_->width : 0, render_target_ ? render_target_->height : 0,
                       render_target_ == back_buffer_ ? " (back buffer)" : "", viewport_.x, viewport_.y,
                       viewport_.width, viewport_.height, window_w, window_h, error);
    }
    if (frame_ == 100 || frame_ == 1000) {  // what the game drew
        std::vector<std::uint8_t> pixels;
        if (back_buffer_->read_pixels(pixels)) {
            const std::string path = hook::data_dir() + "openjoan_frame" + std::to_string(frame_) + ".bmp";
            write_bmp(path, pixels.data(), static_cast<int>(back_buffer_->width), static_cast<int>(back_buffer_->height),
                      true);
            hook::log_line("OpenGL device: back buffer of frame %u written to %s", frame_, path.c_str());
        }
    }
}

HRESULT Device::GetBackBuffer(UINT, Enum, IDirect3DSurface8** out) {
    if (!out) return kInvalidCall;
    back_buffer_->AddRef();
    *out = back_buffer_;
    return kOk;
}

// ---------------------------------------------------------------------------
// resources
// ---------------------------------------------------------------------------
HRESULT Device::CreateTexture(UINT w, UINT h, UINT levels, DWORD usage, Enum format, Enum pool, IDirect3DTexture8** out) {
    if (!out) return kInvalidCall;
    auto* t = new Texture(this, w, h, levels, usage, format, pool);
    if (!t->valid()) {
        hook::log_line("OpenGL device: CreateTexture(%ux%u, format %u) not supported", w, h, format);
        t->Release();
        *out = nullptr;
        return kInvalidCall;
    }
    if (hitch::enabled()) hitch::note("texture made: %ux%u, %u levels, format %u", w, h, levels, format);  // [Debug] HitchLog
    *out = t;
    return kOk;
}

HRESULT Device::CreateVertexBuffer(UINT length, DWORD usage, DWORD fvf, Enum pool, IDirect3DVertexBuffer8** out) {
    if (!out || length == 0) return kInvalidCall;
    if (hitch::enabled()) hitch::note("vertex buffer made: %u bytes", length);  // [Debug] HitchLog
    *out = new VertexBuffer(this, length, usage, kFmtVertexData, fvf, pool, kRtVertexBuffer);
    return kOk;
}

HRESULT Device::CreateIndexBuffer(UINT length, DWORD usage, Enum format, Enum pool, IDirect3DIndexBuffer8** out) {
    if (!out || length == 0) return kInvalidCall;
    *out = new IndexBuffer(this, length, usage, format, 0, pool, kRtIndexBuffer);
    return kOk;
}

HRESULT Device::CreateRenderTarget(UINT w, UINT h, Enum format, Enum, BOOL, IDirect3DSurface8** out) {
    if (!out) return kInvalidCall;
    *out = new Surface(this, Surface::Kind::BackBuffer, format, w, h);
    return kOk;
}

HRESULT Device::CreateDepthStencilSurface(UINT w, UINT h, Enum format, Enum, IDirect3DSurface8** out) {
    if (!out) return kInvalidCall;
    *out = new Surface(this, Surface::Kind::DepthStencil, format, w, h);
    return kOk;
}

HRESULT Device::CreateImageSurface(UINT w, UINT h, Enum format, IDirect3DSurface8** out) {
    if (!out || !format_info(format).supported) return kInvalidCall;
    *out = new Surface(this, Surface::Kind::Image, format, w, h);
    return kOk;
}

HRESULT Device::CopyRects(IDirect3DSurface8* src_if, const void* src_rects, UINT count, IDirect3DSurface8* dst_if,
                          const void* dst_points) {
    if (hook::settings().gl_debug && display::canvas_armed() && canvas_frame_index_ < 1100)
        hook::log_line("OpenGL device: CopyRects(%u rects) to %s during a 1024x768 screen", count,
                       dst_if == back_buffer_ ? "the back buffer" : "a surface");
    auto* src = static_cast<Surface*>(src_if);
    auto* dst = static_cast<Surface*>(dst_if);
    if (!src || !dst) return kInvalidCall;
    const FormatInfo si = format_info(src->format), di = format_info(dst->format);
    if (!si.supported || si.bytes != di.bytes || si.compressed != di.compressed) {
        log_unimplemented("CopyRects between different formats");
        return kInvalidCall;
    }
    std::vector<std::uint8_t> sp, dp;
    if (!src->read_pixels(sp) || !dst->read_pixels(dp)) return kInvalidCall;
    const UINT spitch = row_pitch(si, src->width), dpitch = row_pitch(di, dst->width);
    const Rect full = {0, 0, static_cast<LONG>(src->width), static_cast<LONG>(src->height)};
    const auto* rects = static_cast<const Rect*>(src_rects);
    const auto* points = static_cast<const Point*>(dst_points);
    const UINT n = (count && rects) ? count : 1;
    for (UINT i = 0; i < n; ++i) {
        Rect r = (count && rects) ? rects[i] : full;
        const LONG dx = points ? points[i].x : r.left, dy = points ? points[i].y : r.top;
        const UINT block = si.compressed ? 4 : 1;
        for (LONG y = r.top; y < r.bottom; y += static_cast<LONG>(block)) {
            const LONG ty = dy + (y - r.top);
            if (y < 0 || ty < 0 || static_cast<UINT>(y) >= src->height || static_cast<UINT>(ty) >= dst->height) continue;
            const std::size_t bytes = static_cast<std::size_t>((r.right - r.left + static_cast<LONG>(block) - 1) /
                                                               static_cast<LONG>(block)) * si.bytes;
            const std::size_t so = static_cast<std::size_t>(y / static_cast<LONG>(block)) * spitch +
                                   static_cast<std::size_t>(r.left / static_cast<LONG>(block)) * si.bytes;
            const std::size_t dof = static_cast<std::size_t>(ty / static_cast<LONG>(block)) * dpitch +
                                    static_cast<std::size_t>(dx / static_cast<LONG>(block)) * di.bytes;
            if (so + bytes <= sp.size() && dof + bytes <= dp.size()) std::memcpy(dp.data() + dof, sp.data() + so, bytes);
        }
    }
    return dst->write_pixels(dp.data()) ? kOk : kInvalidCall;
}

HRESULT Device::GetFrontBuffer(IDirect3DSurface8* dst) {
    auto* d = static_cast<Surface*>(dst);
    if (!d) return kInvalidCall;
    std::vector<std::uint8_t> pixels;
    if (!back_buffer_->read_pixels(pixels)) return kInvalidCall;
    std::vector<std::uint8_t> out(static_cast<std::size_t>(d->width) * d->height * 4, 0);
    const UINT w = std::min(d->width, back_buffer_->width), h = std::min(d->height, back_buffer_->height);
    for (UINT y = 0; y < h; ++y)
        for (UINT x = 0; x < w; ++x) {
            std::memcpy(&out[(static_cast<std::size_t>(y) * d->width + x) * 4],
                        &pixels[(static_cast<std::size_t>(y) * back_buffer_->width + x) * 4], 4);
            out[(static_cast<std::size_t>(y) * d->width + x) * 4 + 3] = 0xff;
        }
    return d->write_pixels(out.data()) ? kOk : kInvalidCall;
}

// ---------------------------------------------------------------------------
// render targets
// ---------------------------------------------------------------------------
GLuint Device::framebuffer_for(Surface* color, Surface* depth) {
    if (fb_memo_ != 0 && color == fb_memo_color_ && depth == fb_memo_depth_) return fb_memo_;
    const auto key = std::make_pair(color, depth);
    if (auto it = framebuffers_.find(key); it != framebuffers_.end()) {
        fb_memo_color_ = color;
        fb_memo_depth_ = depth;
        fb_memo_ = it->second;
        return it->second;
    }
    GLuint fbo = 0;
    glCreateFramebuffers(1, &fbo);
    // [Graphics] MSAA: a multisampled colour surface (only ever the device's back buffer) is drawn
    // through its multisampled renderbuffer, and its depth-stencil surface must then be
    // multisampled too (same sample count): it gets that storage here on first use. The same depth
    // surface paired with a single-sampled target keeps using gl_renderbuffer, so the two never
    // share contents (the game clears depth / stencil at the start of each pass).
    const bool multisampled = color && color->gl_msaa;
    if (multisampled) {
        glNamedFramebufferRenderbuffer(fbo, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color->gl_msaa);
        if (depth && depth->kind == Surface::Kind::DepthStencil && !depth->gl_msaa && msaa_samples_) {
            glCreateRenderbuffers(1, &depth->gl_msaa);
            glNamedRenderbufferStorageMultisample(depth->gl_msaa, msaa_samples_, GL_DEPTH24_STENCIL8,
                                                  static_cast<GLsizei>(depth->width), static_cast<GLsizei>(depth->height));
        }
        if (depth && depth->gl_msaa)
            glNamedFramebufferRenderbuffer(fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth->gl_msaa);
    } else if (color) {
        if (color->kind == Surface::Kind::TextureLevel)
            glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, color->texture->gl(), static_cast<GLint>(color->level));
        else
            glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, color->gl_texture, 0);
    }
    if (!multisampled && depth && depth->gl_renderbuffer)
        glNamedFramebufferRenderbuffer(fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth->gl_renderbuffer);
    glNamedFramebufferDrawBuffer(fbo, GL_COLOR_ATTACHMENT0);
    glNamedFramebufferReadBuffer(fbo, GL_COLOR_ATTACHMENT0);
    GLenum status = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE && multisampled && depth && depth->gl_msaa && depth != auto_depth_) {
        // [Graphics] MSAA: a game depth surface whose multisampled storage does not fit the back
        // buffer's (sample count / size): draw without depth rather than not at all. (The automatic
        // depth surface is checked by enable_multisample, which turns MSAA off instead.)
        hook::log_line("OpenGL device: MSAA framebuffer incomplete (0x%x) with a %ux%u depth surface - drawn without it",
                       status, depth->width, depth->height);
        glNamedFramebufferRenderbuffer(fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
        status = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER);
    }
    if (status != GL_FRAMEBUFFER_COMPLETE)
        hook::log_line("OpenGL device: framebuffer incomplete (0x%x) for %ux%u target", status, color ? color->width : 0,
                       color ? color->height : 0);
    framebuffers_[key] = fbo;
    return fbo;
}

void Device::forget_surface(Surface* s) {
    fb_memo_ = 0;
    if (s == resolve_owner_) {  // [Graphics] MSAA resolve framebuffer of `s`
        if (resolve_fbo_) glDeleteFramebuffers(1, &resolve_fbo_);
        resolve_fbo_ = 0;
        resolve_owner_ = nullptr;
    }
    for (auto it = framebuffers_.begin(); it != framebuffers_.end();) {
        if (it->first.first == s || it->first.second == s) {
            if (it->second == current_fbo_) current_fbo_ = 0;
            glDeleteFramebuffers(1, &it->second);
            it = framebuffers_.erase(it);
        } else {
            ++it;
        }
    }
}

GLuint Device::resolve_multisample(Surface* s) {
    if (!s || !s->gl_msaa) return framebuffer_for(s, nullptr);
    if (resolve_owner_ != s) {
        if (resolve_fbo_) glDeleteFramebuffers(1, &resolve_fbo_);
        glCreateFramebuffers(1, &resolve_fbo_);
        glNamedFramebufferTexture(resolve_fbo_, GL_COLOR_ATTACHMENT0, s->gl_texture, 0);
        glNamedFramebufferDrawBuffer(resolve_fbo_, GL_COLOR_ATTACHMENT0);
        glNamedFramebufferReadBuffer(resolve_fbo_, GL_COLOR_ATTACHMENT0);
        resolve_owner_ = s;
    }
    // A 1:1 blit from a multisampled to a single-sampled framebuffer averages the samples. The
    // scissor test clips blits too; every draw and Clear sets the scissor again.
    const auto w = static_cast<GLint>(s->width), h = static_cast<GLint>(s->height);
    glDisable(GL_SCISSOR_TEST);
    renderer_.invalidate_state();
    glBlitNamedFramebuffer(framebuffer_for(s, nullptr), resolve_fbo_, 0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    return resolve_fbo_;
}

void Device::upload_multisample(Surface* s) {
    if (!s || !s->gl_msaa) return;
    if (!ensure_cursor_program()) {
        log_unimplemented("MSAA: writing the back buffer from the CPU (no textured-quad program)");
        return;
    }
    // Draw gl_texture over the whole multisampled target with the cursor's textured-quad program:
    // with glClipControl(GL_UPPER_LEFT) NDC y = +1 is target row 0 (the game's top row, like
    // texel row 0), so uv (0,0)..(1,1) spans NDC (-1,+1)..(1,-1). Sampling at texel centres, every
    // sample gets its pixel's exact value.
    // Raster state is set by the next draw (apply_raster_state) or Clear, as after Present.
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_for(s, nullptr));
    glViewport(0, 0, static_cast<GLsizei>(s->width), static_cast<GLsizei>(s->height));
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColorMask(1, 1, 1, 1);
    const float rect[4] = {-1.0f, 1.0f, 1.0f, -1.0f};
    glUseProgram(cursor_program_);
    glProgramUniform4fv(cursor_program_, cursor_rect_location_, 1, rect);
    glBindVertexArray(cursor_vao_);
    glBindTextureUnit(0, s->gl_texture);
    glBindSampler(0, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindFramebuffer(GL_FRAMEBUFFER, current_fbo_);
    renderer_.invalidate_state();
}

void Device::bind_render_target() {
    const GLuint fbo = framebuffer_for(render_target_, depth_stencil_);
    if (fbo != current_fbo_) {
        renderer_.flush_queued();
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        current_fbo_ = fbo;
    }
}

HRESULT Device::SetRenderTarget(IDirect3DSurface8* rt, IDirect3DSurface8* ds) {
    renderer_.flush_queued();  // [Graphics] GrassBatching: queued replays first
    if (hook::settings().gl_debug && display::canvas_armed() && canvas_frame_index_ < 1100)
        hook::log_line("OpenGL device: SetRenderTarget(%s) during a 1024x768 screen",
                       rt == back_buffer_ ? "back buffer" : rt ? "a surface" : "null");
    auto* new_rt = rt ? static_cast<Surface*>(rt) : render_target_;
    auto* new_ds = static_cast<Surface*>(ds);
    if (new_rt) new_rt->AddRef();
    if (new_ds) new_ds->AddRef();
    if (render_target_) render_target_->Release();
    if (depth_stencil_) depth_stencil_->Release();
    render_target_ = new_rt;
    depth_stencil_ = new_ds;
    reset_viewport_to_target();
    return kOk;
}

HRESULT Device::GetRenderTarget(IDirect3DSurface8** out) {
    if (!out) return kInvalidCall;
    *out = render_target_;
    if (render_target_) render_target_->AddRef();
    return kOk;
}

HRESULT Device::GetDepthStencilSurface(IDirect3DSurface8** out) {
    if (!out) return kInvalidCall;
    *out = depth_stencil_;
    if (!depth_stencil_) return kNotFound;
    depth_stencil_->AddRef();
    return kOk;
}

HRESULT Device::Clear(DWORD count, const void* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil) {
    renderer_.flush_queued();  // [Graphics] GrassBatching: queued replays first
    ++stats_.clears;
    if ((hook::settings().gl_debug && draws_dumped_ < 40) || (hook::settings().gl_debug && display::canvas_armed() && canvas_frame_index_ < 2))
        hook::log_line("OpenGL device: frame %u: Clear(%u rects, flags 0x%x, colour 0x%08x, z %g, stencil %u)", frame_ + 1,
                       count, flags, color, static_cast<double>(z), stencil);
    bind_render_target();
    const bool has_depth = depth_stencil_ != nullptr;
    glEnable(GL_SCISSOR_TEST);
    glColorMask(1, 1, 1, 1);
    glDepthMask(1);
    glStencilMask(0xff);
    const LONG vx0 = static_cast<LONG>(viewport_.x), vy0 = static_cast<LONG>(viewport_.y);
    const LONG vx1 = vx0 + static_cast<LONG>(viewport_.width), vy1 = vy0 + static_cast<LONG>(viewport_.height);
    std::vector<D3DRect> areas;
    if (count && rects) {
        const auto* r = static_cast<const D3DRect*>(rects);
        for (DWORD i = 0; i < count; ++i)
            areas.push_back({std::max(r[i].x1, vx0), std::max(r[i].y1, vy0), std::min(r[i].x2, vx1), std::min(r[i].y2, vy1)});
    } else {
        areas.push_back({vx0, vy0, vx1, vy1});
    }
    if (canvas_on() || layout_viewport())
        for (auto& a : areas) a = canvas_rect(a);
    float rgba[4];
    color4(color, rgba);
    const GLint s = static_cast<GLint>(stencil);
    for (const auto& a : areas) {
        if (a.x2 <= a.x1 || a.y2 <= a.y1) continue;
        glScissor(a.x1, a.y1, a.x2 - a.x1, a.y2 - a.y1);
        if (flags & kClearTarget) glClearNamedFramebufferfv(current_fbo_, GL_COLOR, 0, rgba);
        if (has_depth && (flags & kClearZBuffer) && (flags & kClearStencil))
            glClearNamedFramebufferfi(current_fbo_, GL_DEPTH_STENCIL, 0, z, s);
        else if (has_depth && (flags & kClearZBuffer))
            glClearNamedFramebufferfv(current_fbo_, GL_DEPTH_, 0, &z);
        else if (has_depth && (flags & kClearStencil))
            glClearNamedFramebufferiv(current_fbo_, GL_STENCIL_, 0, &s);
    }
    glDisable(GL_SCISSOR_TEST);
    renderer_.invalidate_state();  // scissor, colour / depth / stencil write masks
    return kOk;
}

// ---------------------------------------------------------------------------
// fixed-function state
// ---------------------------------------------------------------------------
HRESULT Device::SetTransform(Enum state, const float* matrix) {
    if (!matrix) return kInvalidCall;
    // The same matrix again (the game resets texture matrices to identity per material): nothing changes.
    if (std::memcmp(transform(state).m, matrix, sizeof(Matrix::m)) == 0) return kOk;
    std::memcpy(transform(state).m, matrix, sizeof(Matrix::m));
    if (state == kTsView) ++light_version_;  // lights are uploaded in view space
    if (state < 256) ++frame_version_;  // view, projection, texture matrices (block Ffp)
    else if (state < kTsWorld + 4) ++draw_version_;  // WORLD..WORLD3 (block FfpDraw)
    return kOk;
}
HRESULT Device::GetTransform(Enum state, float* matrix) {
    if (!matrix) return kInvalidCall;
    std::memcpy(matrix, transform(state).m, sizeof(Matrix::m));
    return kOk;
}
HRESULT Device::MultiplyTransform(Enum state, const float* matrix) {
    if (!matrix) return kInvalidCall;
    Matrix m;
    std::memcpy(m.m, matrix, sizeof m.m);
    transform(state) = multiply(m, transform(state));
    if (state == kTsView) ++light_version_;
    if (state < 256) ++frame_version_;
    else if (state < kTsWorld + 4) ++draw_version_;
    return kOk;
}
HRESULT Device::SetViewport(const void* vp) {
    if (!vp) return kInvalidCall;
    std::memcpy(&viewport_, vp, sizeof viewport_);
    ++frame_version_;
    return kOk;
}
HRESULT Device::GetViewport(void* vp) {
    if (!vp) return kInvalidCall;
    std::memcpy(vp, &viewport_, sizeof viewport_);
    return kOk;
}
HRESULT Device::SetMaterial(const void* mat) {
    if (!mat) return kInvalidCall;
    if (std::memcmp(&material_, mat, sizeof material_) == 0) return kOk;
    std::memcpy(&material_, mat, sizeof material_);
    ++draw_version_;  // block FfpDraw
    return kOk;
}
HRESULT Device::GetMaterial(void* mat) {
    if (!mat) return kInvalidCall;
    std::memcpy(mat, &material_, sizeof material_);
    return kOk;
}
HRESULT Device::SetLight(DWORD index, const void* light) {
    if (!light) return kInvalidCall;
    std::memcpy(&lights_[index], light, sizeof(Light));
    ++light_version_;
    ++frame_version_;
    ++program_version_;  // light types are part of the program key
    return kOk;
}
HRESULT Device::GetLight(DWORD index, void* light) {
    const auto it = lights_.find(index);
    if (!light || it == lights_.end()) return kInvalidCall;
    std::memcpy(light, &it->second, sizeof(Light));
    return kOk;
}
HRESULT Device::LightEnable(DWORD index, BOOL enable) {
    if (!lights_.count(index)) {  // Direct3D creates a default (white directional) light
        Light l = {};
        l.type = 3;
        l.diffuse = {1, 1, 1, 0};
        l.direction = {0, 0, 1};
        lights_[index] = l;
    }
    light_enabled_[index] = enable != 0;
    ++light_version_;
    ++frame_version_;
    ++program_version_;
    return kOk;
}
HRESULT Device::GetLightEnable(DWORD index, BOOL* enable) {
    if (!enable || !lights_.count(index)) return kInvalidCall;
    const auto it = light_enabled_.find(index);
    *enable = (it != light_enabled_.end() && it->second) ? 128 : 0;
    return kOk;
}
HRESULT Device::SetRenderState(Enum state, DWORD value) {
    if (state < 256 && rs_[state] != value) {
        rs_[state] = value;
        switch (state) {
        // Read by build_program_for_draw (the program key); the raster states (blend, depth, stencil, cull ...)
        // are applied on every draw by apply_raster_state.
        case kRsAlphaTestEnable: case kRsAlphaFunc: case kRsFogEnable: case kRsSpecularEnable: case kRsFogTableMode:
        case kRsFogVertexMode: case kRsRangeFogEnable: case kRsColorVertex: case kRsLocalViewer: case kRsNormalizeNormals:
        case kRsDiffuseMaterialSource: case kRsSpecularMaterialSource: case kRsAmbientMaterialSource:
        case kRsEmissiveMaterialSource: case kRsVertexBlend:
            ++program_version_;
            break;
        case kRsLighting:  // the program key, and block Ffp's lights
            ++program_version_;
            ++frame_version_;
            break;
        // Block Ffp.
        case kRsFogColor: case kRsFogStart: case kRsFogEnd: case kRsFogDensity: case kRsAmbient: case kRsPointSize:
        case kRsPointSizeMin: case kRsPointSizeMax:
            ++frame_version_;
            break;
        // Block FfpDraw.
        case kRsTextureFactor: case kRsAlphaRef:
            ++draw_version_;
            break;
        default:
            break;
        }
    }
    return kOk;
}
HRESULT Device::GetRenderState(Enum state, DWORD* value) {
    if (!value || state >= 256) return kInvalidCall;
    *value = rs_[state];
    return kOk;
}
HRESULT Device::GetTexture(DWORD stage, IDirect3DBaseTexture8** out) {
    if (!out || stage >= kMaxStages) return kInvalidCall;
    *out = textures_[stage];
    if (textures_[stage]) textures_[stage]->AddRef();
    return kOk;
}
HRESULT Device::SetTexture(DWORD stage, IDirect3DBaseTexture8* tex) {
    if (stage >= kMaxStages) return kInvalidCall;
    auto* t = static_cast<Texture*>(static_cast<IDirect3DTexture8*>(tex));
    if (t == textures_[stage]) return kOk;  // the same texture again: no reference counting
    if ((t != nullptr) != (textures_[stage] != nullptr)) ++program_version_;  // a stage with / without a texture
    if (t) t->AddRef();
    if (textures_[stage]) textures_[stage]->Release();
    textures_[stage] = t;
    return kOk;
}
HRESULT Device::GetTextureStageState(DWORD stage, Enum type, DWORD* value) {
    if (!value || stage >= kMaxStages || type >= 32) return kInvalidCall;
    *value = tss_[stage][type];
    return kOk;
}
HRESULT Device::SetTextureStageState(DWORD stage, Enum type, DWORD value) {
    if (stage >= kMaxStages || type >= 32) return kInvalidCall;
    if (tss_[stage][type] != value) {
        tss_[stage][type] = value;
        ++program_version_;
        if ((type >= kTssBumpEnvMat00 && type <= kTssBumpEnvMat11) || type == kTssBumpEnvLScale || type == kTssBumpEnvLOffset)
            ++frame_version_;  // block Ffp
    }
    return kOk;
}

// ---------------------------------------------------------------------------
// vertex shaders and streams
// ---------------------------------------------------------------------------
HRESULT Device::CreateVertexShader(const DWORD* decl, const DWORD* function, DWORD* handle, DWORD) {
    if (!decl || !handle) return kInvalidCall;
    VertexShaderInfo info;
    DWORD offset = 0;
    for (const DWORD* t = decl;; ++t) {
        info.declaration.push_back(*t);
        if (*t == 0xffffffff) break;
        const DWORD kind = *t >> 29;
        if (kind == 2) {  // stream data
            if (*t & 0x10000000) {  // skip n DWORDs
                offset += ((*t >> 16) & 0xf) * 4;
            } else {
                const DWORD reg = *t & 0x1f, type = (*t >> 16) & 0xf;
                static const DWORD sizes[] = {4, 8, 12, 16, 4, 4, 4, 8};
                info.elements.push_back({reg, type, offset});
                info.input_mask |= 1u << reg;
                offset += type < 8 ? sizes[type] : 4;
            }
        } else if (kind == 4) {  // constant memory: count, address, then count * 4 floats
            const DWORD n = (*t >> 25) & 0xf, address = *t & 0x7f;
            std::vector<float> values(n * 4);
            std::memcpy(values.data(), t + 1, values.size() * sizeof(float));
            for (DWORD i = 0; i < n * 4; ++i) info.declaration.push_back(t[1 + i]);
            info.constants.emplace_back(address, std::move(values));
            t += n * 4;
        } else if (kind == 1 && (*t & 0xf) != 0) {
            log_unimplemented("vertex declarations with streams other than 0");
        }
    }
    if (function) {
        for (const DWORD* t = function;; ++t) {
            info.function.push_back(*t);
            if (*t == 0x0000ffff) break;
        }
        // def c#: constants defined in the shader body.
        for (std::size_t i = 1; i + 5 < info.function.size(); ++i) {
            const DWORD op = info.function[i] & 0xffff;
            if (op == 0xfffe) {
                i += (info.function[i] >> 16) & 0x7fff;
                continue;
            }
            if (op == 81) {
                std::vector<float> v(4);
                std::memcpy(v.data(), &info.function[i + 2], 16);
                info.constants.emplace_back(info.function[i + 1] & 0x7ff, std::move(v));
                i += 5;
            }
        }
        info.glsl = translate_vertex_shader(info.function, info.input_mask, info.error);
        if (info.glsl.empty()) hook::log_line("OpenGL device: vertex shader not translated: %s", info.error.c_str());
    } else {
        info.error = "fixed-function vertex shader declarations are not supported";
        log_unimplemented("CreateVertexShader without a function");
    }
    const DWORD h = (next_shader_++ << 1) | 1;
    shaders_[h] = std::move(info);
    *handle = h;
    return kOk;
}

HRESULT Device::SetVertexShader(DWORD handle) {
    if (vertex_shader_ != handle) ++program_version_;
    vertex_shader_ = handle;
    if (handle & 1) {
        const auto it = shaders_.find(handle);
        if (it == shaders_.end()) return kInvalidCall;
        for (const auto& [address, values] : it->second.constants)
            for (std::size_t i = 0; i < values.size() / 4 && address + i < kVsConstants; ++i)
                std::memcpy(vs_constants_[address + i], &values[i * 4], 16);
        if (!it->second.constants.empty()) vs_constants_dirty_ = true;
    }
    return kOk;
}
HRESULT Device::GetVertexShader(DWORD* handle) {
    if (!handle) return kInvalidCall;
    *handle = vertex_shader_;
    return kOk;
}
HRESULT Device::DeleteVertexShader(DWORD handle) {
    shaders_.erase(handle);
    if (vertex_shader_ == handle) {
        vertex_shader_ = 0;
        ++program_version_;
    }
    return kOk;
}
HRESULT Device::SetVertexShaderConstant(DWORD reg, const void* data, DWORD count) {
    if (!data || reg >= kVsConstants) return kInvalidCall;
    count = std::min<DWORD>(count, kVsConstants - reg);
    std::memcpy(vs_constants_[reg], data, count * 16);
    vs_constants_dirty_ = true;
    return kOk;
}
HRESULT Device::GetVertexShaderConstant(DWORD reg, void* data, DWORD count) {
    if (!data || reg >= kVsConstants) return kInvalidCall;
    count = std::min<DWORD>(count, kVsConstants - reg);
    std::memcpy(data, vs_constants_[reg], count * 16);
    return kOk;
}
// D3D8 semantics: data == nullptr returns the size in bytes; a too small buffer fails with the size.
// Used by the nvlinker shader log ([Graphics] ShaderLog=1, render/nvlink_log.cpp).
static HRESULT copy_shader_tokens(const std::vector<DWORD>& tokens, void* data, DWORD* size) {
    if (!size) return kInvalidCall;
    const auto bytes = static_cast<DWORD>(tokens.size() * sizeof(DWORD));
    if (!data) {
        *size = bytes;
        return kOk;
    }
    if (*size < bytes) {
        *size = bytes;
        return kInvalidCall;
    }
    std::memcpy(data, tokens.data(), bytes);
    *size = bytes;
    return kOk;
}
HRESULT Device::GetVertexShaderDeclaration(DWORD handle, void* data, DWORD* size) {
    const auto it = shaders_.find(handle);
    if (it == shaders_.end()) return kInvalidCall;
    return copy_shader_tokens(it->second.declaration, data, size);
}
HRESULT Device::GetVertexShaderFunction(DWORD handle, void* data, DWORD* size) {
    const auto it = shaders_.find(handle);
    if (it == shaders_.end()) return kInvalidCall;
    return copy_shader_tokens(it->second.function, data, size);
}
HRESULT Device::SetStreamSource(UINT stream, IDirect3DVertexBuffer8* vb, UINT stride) {
    if (stream != 0) {
        log_unimplemented("SetStreamSource on streams other than 0");
        return kInvalidCall;
    }
    auto* b = static_cast<VertexBuffer*>(vb);
    if (b == stream_) {  // the same buffer again (the engine's streaming rings, almost every draw): no reference churn
        stride_ = stride;
        return kOk;
    }
    if (b) b->AddRef();
    if (stream_) stream_->Release();
    stream_ = b;
    stride_ = stride;
    return kOk;
}
HRESULT Device::GetStreamSource(UINT stream, IDirect3DVertexBuffer8** vb, UINT* stride) {
    if (!vb || !stride || stream != 0) return kInvalidCall;
    *vb = stream_;
    if (stream_) stream_->AddRef();
    *stride = stride_;
    return kOk;
}
HRESULT Device::SetIndices(IDirect3DIndexBuffer8* ib, UINT base_vertex) {
    auto* b = static_cast<IndexBuffer*>(ib);
    if (b == indices_) {
        base_vertex_ = base_vertex;
        return kOk;
    }
    if (b) b->AddRef();
    if (indices_) indices_->Release();
    indices_ = b;
    base_vertex_ = base_vertex;
    return kOk;
}
HRESULT Device::GetIndices(IDirect3DIndexBuffer8** ib, UINT* base_vertex) {
    if (!ib || !base_vertex) return kInvalidCall;
    *ib = indices_;
    if (indices_) indices_->AddRef();
    *base_vertex = base_vertex_;
    return kOk;
}
HRESULT Device::SetPixelShader(DWORD handle) {
    if (handle) log_unimplemented("SetPixelShader with a pixel shader");
    pixel_shader_ = handle;
    return kOk;
}
HRESULT Device::GetPixelShader(DWORD* handle) {
    if (!handle) return kInvalidCall;
    *handle = pixel_shader_;
    return kOk;
}

// ---------------------------------------------------------------------------
// drawing
// ---------------------------------------------------------------------------
// The program of the current state: rebuilt from the Direct3D state (render / texture stage states, which stages
// have a texture, the vertex shader, the lights) only when something it reads changed (program_version_, bumped by
// the setters when a value actually changes); most draws keep the previous draw's program.
GLuint Device::program_for_draw(bool& ffp) {
    if (program_memo_version_ == program_version_) {
        ffp = program_memo_ffp_;
        return program_memo_;
    }
    const GLuint program = build_program_for_draw(ffp);
    program_memo_ = program;
    program_memo_ffp_ = ffp;
    program_memo_version_ = program_version_;
    return program;
}

GLuint Device::build_program_for_draw(bool& ffp) {
    draw_lift_ = false;
    const bool programmable = (vertex_shader_ & 1) != 0;
    ffp = !programmable;

    FragmentKey fk;
    bool disabled = false;
    for (int s = 0; s < kMaxStages; ++s) {
        const DWORD* t = tss_[s];
        StageConfig& c = fk.stages[s];
        if (disabled || t[kTssColorOp] == 1) {
            disabled = true;
            c = StageConfig{};
            continue;
        }
        c.color_op = static_cast<std::uint8_t>(t[kTssColorOp]);
        c.color_arg1 = static_cast<std::uint8_t>(t[kTssColorArg1]);
        c.color_arg2 = static_cast<std::uint8_t>(t[kTssColorArg2]);
        c.color_arg0 = static_cast<std::uint8_t>(t[kTssColorArg0]);
        c.alpha_op = static_cast<std::uint8_t>(t[kTssAlphaOp]);
        c.alpha_arg1 = static_cast<std::uint8_t>(t[kTssAlphaArg1]);
        c.alpha_arg2 = static_cast<std::uint8_t>(t[kTssAlphaArg2]);
        c.alpha_arg0 = static_cast<std::uint8_t>(t[kTssAlphaArg0]);
        if (!textures_[s]) {
            // An op that reads D3DTA_TEXTURE on a stage without a texture passes CURRENT through
            // (Direct3D drivers / Wine behave so; the game draws its black movie bars like this).
            auto reads_texture = [](std::uint8_t op, std::uint8_t a1, std::uint8_t a2, std::uint8_t a0) {
                auto is_tex = [](std::uint8_t a) { return (a & 0x0f) == 2; };
                if (op == 1 || op == 22 || op == 23) return false;
                if (is_tex(a1) && op != 3) return true;
                if (is_tex(a2) && op != 2) return true;
                if (is_tex(a0) && (op == 25 || op == 26)) return true;
                return op == 13 || op == 15;  // BLENDTEXTUREALPHA(PM) read the texture's alpha
            };
            if (reads_texture(c.color_op, c.color_arg1, c.color_arg2, c.color_arg0)) {
                c.color_op = 2;
                c.color_arg1 = 1;
            }
            if (reads_texture(c.alpha_op, c.alpha_arg1, c.alpha_arg2, c.alpha_arg0)) {
                c.alpha_op = 2;
                c.alpha_arg1 = 1;
            }
        }
        c.result_temp = t[kTssResultArg] == 5;
        c.has_texture = textures_[s] != nullptr;
        c.coord_source = static_cast<std::uint8_t>(programmable ? (t[kTssTexCoordIndex] & 7) : s);
        c.projected = (t[kTssTextureTransformFlags] & 0x100) != 0;
        c.projected_count = static_cast<std::uint8_t>(t[kTssTextureTransformFlags] & 0xff);
    }
    fk.specular_enable = rs_[kRsSpecularEnable] != 0;
    fk.fog_enable = rs_[kRsFogEnable] != 0;
    fk.alpha_test = rs_[kRsAlphaTestEnable] != 0;
    fk.alpha_func = static_cast<std::uint8_t>(rs_[kRsAlphaFunc]);

    if (programmable) {
        const auto it = shaders_.find(vertex_shader_);
        if (it == shaders_.end() || it->second.glsl.empty()) return 0;
        return renderer_.shader_program(vertex_shader_, it->second.glsl, fk);
    } else {
        VertexKey vk;
        vk.fvf = vertex_shader_;
        const bool rhw = (vk.fvf & kFvfPositionMask) == kFvfXyzRhw;
        const DWORD blend = rs_[kRsVertexBlend];
        vk.vertex_blend = static_cast<std::uint8_t>(blend <= 3 ? blend : 0);
        vk.lighting = !rhw && rs_[kRsLighting] != 0;
        if (vk.lighting) {
            for (const auto& [index, on] : light_enabled_) {
                if (!on || vk.light_count >= kMaxLights) continue;
                const auto l = lights_.find(index);
                if (l == lights_.end()) continue;
                vk.light_types[vk.light_count++] = static_cast<std::uint8_t>(l->second.type);
            }
            vk.specular_enable = rs_[kRsSpecularEnable] != 0;
            vk.local_viewer = rs_[kRsLocalViewer] != 0;
            vk.normalize_normals = rs_[kRsNormalizeNormals] != 0;
            vk.color_vertex = rs_[kRsColorVertex] != 0;
            vk.diffuse_source = static_cast<std::uint8_t>(rs_[kRsDiffuseMaterialSource]);
            vk.specular_source = static_cast<std::uint8_t>(rs_[kRsSpecularMaterialSource]);
            vk.ambient_source = static_cast<std::uint8_t>(rs_[kRsAmbientMaterialSource]);
            vk.emissive_source = static_cast<std::uint8_t>(rs_[kRsEmissiveMaterialSource]);
        }
        if (rs_[kRsFogEnable] && !rhw) {
            const DWORD mode = rs_[kRsFogTableMode] ? rs_[kRsFogTableMode] : rs_[kRsFogVertexMode];
            vk.fog_mode = static_cast<std::uint8_t>(mode & 3);
            vk.range_fog = rs_[kRsRangeFogEnable] != 0;
        }
        for (int s = 0; s < kMaxStages; ++s) {
            if (fk.stages[s].color_op == 1) break;
            const DWORD tci = tss_[s][kTssTexCoordIndex];
            vk.tex[s].index = static_cast<std::uint8_t>(std::min<DWORD>(tci & 0xffff, 7));
            vk.tex[s].generate = static_cast<std::uint8_t>((tci >> 16) & 3);
            vk.tex[s].transform = static_cast<std::uint8_t>(tss_[s][kTssTextureTransformFlags] & 0xff);
        }
        // [Graphics] PixelLighting (render/pixel_lighting.hpp).
        if (!rhw && surface_pl_ && vk.lighting && (vk.fvf & kFvfNormal) != 0) {
            vk.pixel = fk.pixel = kPixelVaryings | kPixelLighting;
            {
                fk.light_count = vk.light_count;
                std::memcpy(fk.light_types, vk.light_types, sizeof fk.light_types);
                fk.light_specular = vk.specular_enable;
                fk.local_viewer = vk.local_viewer;
            }
        }
        // [Graphics] BakedShadows (render/baked_shadows.hpp): the ground's lightmap stage lifted by the level map.
        if (!rhw && baked_on_) {
            for (int s = 1; s < kLiftUnit && fk.stages[s].color_op != 1; ++s) {
                if (!lightmap_stage(static_cast<DWORD>(s))) continue;
                vk.pixel |= kPixelLift;
                fk.pixel |= kPixelLift;
                fk.lift_stage = static_cast<std::uint8_t>(s);
                draw_lift_ = true;
                break;
            }
        }
        return renderer_.ffp_program(vk, fk);
    }
}

GLuint Device::vertex_array_for(std::uint32_t stride) {
    if (vertex_shader_ & 1) return renderer_.vertex_array_elements(vertex_shader_, stride, shaders_[vertex_shader_].elements);
    return renderer_.vertex_array_fvf(vertex_shader_, stride);
}

render::SamplerDesc Device::sampler_desc(DWORD stage) const {
    const DWORD* t = tss_[stage];
    render::SamplerDesc d;
    d.address_u = t[kTssAddressU];
    d.address_v = t[kTssAddressV];
    d.mag = t[kTssMagFilter];
    d.min = t[kTssMinFilter];
    d.mip = t[kTssMipFilter];
    d.max_anisotropy = t[kTssMaxAnisotropy];
    return d;
}

// The GL sampler of stage `s`, looked up only when the stage's sampler states changed since its last draw.
GLuint Device::stage_sampler(DWORD s) {
    const render::SamplerDesc d = sampler_desc(s);
    if (!sampler_memo_valid_[s] || std::memcmp(&d, &sampler_memo_desc_[s], sizeof d) != 0) {
        sampler_memo_desc_[s] = d;
        sampler_memo_[s] = renderer_.sampler(d);
        sampler_memo_valid_[s] = true;
    }
    return sampler_memo_[s];
}

void Device::apply_raster_state() {
    apply_viewport_scissor();
    apply_raster_states();
}

void Device::apply_viewport_scissor() {
    if (canvas_on()) {
        // The canvas transform (u_canvas) places the game's viewport inside the full target;
        // the scissor clips to where the game's viewport ends up.
        renderer_.set_viewport(0, 0, static_cast<GLsizei>(render_target_->width), static_cast<GLsizei>(render_target_->height));
        const D3DRect v = canvas_rect({static_cast<LONG>(viewport_.x), static_cast<LONG>(viewport_.y),
                                       static_cast<LONG>(viewport_.x + viewport_.width),
                                       static_cast<LONG>(viewport_.y + viewport_.height)});
        renderer_.set_scissor(true, v.x1, v.y1, v.x2 - v.x1, v.y2 - v.y1);
    } else {
        if (layout_viewport()) {
            // A camera of a 1024x768 screen (mission-brief maps, commander portrait): its viewport is
            // in layout pixels, so it is placed on the canvas like the screen's sprites.
            const D3DRect v = canvas_rect({static_cast<LONG>(viewport_.x), static_cast<LONG>(viewport_.y),
                                           static_cast<LONG>(viewport_.x + viewport_.width),
                                           static_cast<LONG>(viewport_.y + viewport_.height)});
            renderer_.set_viewport(v.x1, v.y1, v.x2 - v.x1, v.y2 - v.y1);
        } else {
            renderer_.set_viewport(static_cast<GLint>(viewport_.x), static_cast<GLint>(viewport_.y),
                                   static_cast<GLsizei>(viewport_.width), static_cast<GLsizei>(viewport_.height));
        }
        // Only while a full screen is shown: an armed state lingers a few frames after a menu
        // closes, and the game world drawn then must not be clipped (its clear colour showed in
        // the bars).
        if (layout_viewport() && display::opaque_screen_active()) {
            // A camera of a shown 1024x768 screen (e.g. the mission-brief 3D map): nothing it draws
            // spills into the bars beside it. The full-screen world is never clipped (the level's first
            // frames after the loading screen were shown between bars); a menu opening over it has its
            // bars cleared at Present.
            const auto& c = display::canvas();
            const auto x = static_cast<GLint>(c.x), y = static_cast<GLint>(c.y);
            renderer_.set_scissor(true, x, y, static_cast<GLsizei>(std::lround(1024.0f * c.scale)),
                                  static_cast<GLsizei>(std::lround(768.0f * c.scale)));
        } else {
            renderer_.set_scissor(false, 0, 0, 0, 0);
        }
    }
}

void Device::apply_raster_states() {
    // Direct3D render states -> render::RasterState.
    render::RasterState r;
    r.depth_min = viewport_.min_z;
    r.depth_max = viewport_.max_z;

    r.depth_test = (rs_[kRsZEnable] || render::overrides().depth_test) && depth_stencil_;
    r.depth_func = compare_func(rs_[kRsZFunc]);
    r.depth_write = rs_[kRsZWriteEnable] != 0;

    const DWORD cull = rs_[kRsCullMode];
    if (cull == 2 || cull == 3) {
        // With glClipControl(GL_UPPER_LEFT) the facing test keeps the NDC orientation, which looks
        // the same as the D3D screen: a triangle clockwise on screen is clockwise in NDC, i.e. a
        // GL back face (front = CCW). D3DCULL_CW therefore culls GL back faces, D3DCULL_CCW front
        // faces. (Verified on the game's 2D quads, which are drawn with CULL_CW and are CCW.)
        bool cull_back = cull == 2;
        if (hook::settings().cull_flip) cull_back = !cull_back;
        r.cull = cull_back ? GL_BACK : GL_FRONT;
    } else {
        r.cull = 0;
    }
    const DWORD fill = rs_[kRsFillMode];
    r.polygon_mode = fill == 1 ? GL_POINT : fill == 2 ? GL_LINE : GL_FILL;

    // [Graphics] Water: a river draws opaque (a falling sheet keeps the game's blending).
    r.blend = rs_[kRsAlphaBlendEnable] != 0 && !(river_now_ && !render::water_river_blended());
    if (r.blend) {
        DWORD src = rs_[kRsSrcBlend], dst = rs_[kRsDestBlend];
        if (src == 12) {  // BOTHSRCALPHA
            src = 5;
            dst = 6;
        } else if (src == 13) {  // BOTHINVSRCALPHA
            src = 6;
            dst = 5;
        }
        r.blend_src = blend_factor(src);
        r.blend_dst = blend_factor(dst);
        switch (rs_[kRsBlendOp]) {
            case 2: r.blend_equation = GL_FUNC_SUBTRACT; break;
            case 3: r.blend_equation = GL_FUNC_REVERSE_SUBTRACT; break;
            case 4: r.blend_equation = GL_MIN; break;
            case 5: r.blend_equation = GL_MAX; break;
            default: r.blend_equation = GL_FUNC_ADD; break;
        }
    }

    r.stencil = rs_[kRsStencilEnable] && depth_stencil_;
    if (r.stencil) {
        r.stencil_func = compare_func(rs_[kRsStencilFunc]);
        r.stencil_ref = static_cast<GLint>(rs_[kRsStencilRef]);
        r.stencil_mask = rs_[kRsStencilMask];
        r.stencil_fail = stencil_op(rs_[kRsStencilFail]);
        r.stencil_depth_fail = stencil_op(rs_[kRsStencilZFail]);
        r.stencil_pass = stencil_op(rs_[kRsStencilPass]);
        r.stencil_write_mask = rs_[kRsStencilWriteMask];
    }

    r.color_mask = static_cast<std::uint8_t>(rs_[kRsColorWriteEnable] & 0xf);

    const DWORD bias = rs_[kRsZBias];
    r.depth_bias_units = bias ? -4.0f * static_cast<float>(bias) : 0.0f;

    last_raster_ = r;  // [Graphics] GrassBatching records it
    renderer_.apply(r);
}

void Device::upload_uniforms(bool ffp) {
    // Block FfpDraw: every draw (the object's matrices and material).
    FfpDrawUniforms& d = draw_uniforms_;
    const Matrix& view = transform(kTsView);
    // The normals are read by lighting and by texture coordinates made from them (TCI_CAMERASPACENORMAL /
    // REFLECTIONVECTOR); other draws leave the normal matrices as they are.
    bool normals = ffp && rs_[kRsLighting] != 0;
    for (int s = 0; ffp && !normals && s < kMaxStages && tss_[s][kTssColorOp] != kTopDisable; ++s) {
        const DWORD gen = (tss_[s][kTssTexCoordIndex] >> 16) & 3;
        normals = gen == 1 || gen == 3;
    }
    // The world matrices the program reads: WORLD..WORLDn with n vertex blend weights (vertex shaders: none
    // of them, WORLD kept for the passes that replay draws).
    const int worlds = ffp ? static_cast<int>(std::min<DWORD>(rs_[kRsVertexBlend], 3)) + 1 : 1;
    // Nothing the record holds changed since the last one (draw_version_: world matrices, material, texture
    // factor, alpha reference; the view for the normal matrices): the draw reads that record again - no copy,
    // compare or ring write. Its ring slot must still be intact.
    const DrawRecordKey key{draw_version_, normals ? frame_version_ : 0u, worlds, normals};
    const bool reuse = draw_key_valid_ && key == draw_key_ && renderer_.uniform_offset(3) != ~0u && !renderer_.block_stale(3);
    if (reuse) renderer_.reuse_ffp_draw();
    for (int i = 0; !reuse && i < worlds; ++i) {
        const Matrix& w = transform(kTsWorld + static_cast<Enum>(i));
        std::memcpy(d.world[i], w.m, 64);
        // The normal matrix (a 3x3 inverse) only when its world or the view matrix changed.
        if (normals && (!normal_valid_[i] || std::memcmp(normal_world_[i].m, w.m, 64) != 0 || std::memcmp(normal_view_[i].m, view.m, 64) != 0)) {
            std::memcpy(d.normal_matrix[i], normal_matrix(multiply(w, view)).m, 64);
            normal_world_[i] = w;
            normal_view_[i] = view;
            normal_valid_[i] = true;
        }
    }
    if (!reuse) {
        copy4(material_.diffuse, d.material_diffuse);
        copy4(material_.ambient, d.material_ambient);
        copy4(material_.specular, d.material_specular);
        copy4(material_.emissive, d.material_emissive);
        d.material_power[0] = material_.power;
        color4(rs_[kRsTextureFactor], d.texture_factor);
        d.alpha_ref[0] = static_cast<float>(rs_[kRsAlphaRef] & 0xff);
        renderer_.upload_ffp_draw(d);
        draw_key_ = key;
        draw_key_valid_ = true;
    }
    last_normals_ = normals;
    last_worlds_ = worlds;
    upload_frame_block(ffp, ffp && rs_[kRsLighting] != 0);
}

// Block Ffp: only when something it holds changed (frame_version_, the canvas, lights a lit draw needs), or its
// ring slot is about to be reused. `lit`: the draw is lit (its lights must be current).
void Device::upload_frame_block(bool ffp, bool lit) {
    const Matrix& view = transform(kTsView);
    FfpUniforms& u = uniforms_;
    float canvas[4] = {1.0f, 1.0f, 0.0f, 0.0f};
    if (canvas_on()) {
        // Game NDC (relative to the game's viewport) -> NDC of the full target after scaling the
        // game's screen pixels by the canvas: x' = A x + B (y likewise, top-down pixels).
        const auto& c = display::canvas();
        const float tw = static_cast<float>(render_target_->width), th = static_cast<float>(render_target_->height);
        const float vx = static_cast<float>(viewport_.x), vy = static_cast<float>(viewport_.y);
        const float vw = static_cast<float>(std::max<DWORD>(viewport_.width, 1)), vh = static_cast<float>(std::max<DWORD>(viewport_.height, 1));
        canvas[0] = c.scale * vw / tw;
        canvas[1] = c.scale * vh / th;
        canvas[2] = 2.0f * (c.scale * (vx + vw * 0.5f) + c.x) / tw - 1.0f;
        canvas[3] = 1.0f - 2.0f * (c.scale * (vy + vh * 0.5f) + c.y) / th;
    }
    const bool lights = lit && light_memo_version_ != light_version_;
    if (frame_memo_version_ != frame_version_ || lights || std::memcmp(canvas, u.canvas, sizeof canvas) != 0 || renderer_.block_stale(0)) {
        frame_memo_version_ = frame_version_;
        std::memcpy(u.canvas, canvas, sizeof canvas);
        std::memcpy(u.view, view.m, 64);
        std::memcpy(u.proj, transform(kTsProjection).m, 64);
        for (int s = 0; s < kMaxStages; ++s) std::memcpy(u.texture_matrix[s], transform(kTsTexture0 + static_cast<Enum>(s)).m, 64);
        color4(rs_[kRsAmbient], u.global_ambient);
        const float start = as_float(rs_[kRsFogStart]), end = as_float(rs_[kRsFogEnd]);
        u.fog_params[0] = start;
        u.fog_params[1] = end;
        u.fog_params[2] = as_float(rs_[kRsFogDensity]);
        u.fog_params[3] = end != start ? 1.0f / (end - start) : 0.0f;
        color4(rs_[kRsFogColor], u.fog_color);
        u.viewport[0] = static_cast<float>(viewport_.x);
        u.viewport[1] = static_cast<float>(viewport_.y);
        u.viewport[2] = static_cast<float>(std::max<DWORD>(viewport_.width, 1));
        u.viewport[3] = static_cast<float>(std::max<DWORD>(viewport_.height, 1));
        u.point_params[0] = as_float(rs_[kRsPointSize]);
        u.point_params[1] = as_float(rs_[kRsPointSizeMin]);
        u.point_params[2] = as_float(rs_[kRsPointSizeMax]);
        // The lights (view space) only when a light, its enable or the view matrix changed: uniforms_ keeps them.
        if (lights) {
            light_memo_version_ = light_version_;
            int n = 0;
            for (const auto& [index, on] : light_enabled_) {
                if (!on || n >= kMaxLights) continue;
                const auto it = lights_.find(index);
                if (it == lights_.end()) continue;
                const Light& l = it->second;
                copy4(l.diffuse, u.light_diffuse[n]);
                copy4(l.specular, u.light_specular[n]);
                copy4(l.ambient, u.light_ambient[n]);
                transform_point(view, l.position, u.light_position[n]);
                transform_direction(view, l.direction, u.light_direction[n]);
                u.light_attenuation[n][0] = l.attenuation0;
                u.light_attenuation[n][1] = l.attenuation1;
                u.light_attenuation[n][2] = l.attenuation2;
                u.light_attenuation[n][3] = l.range > 0 ? l.range : 3.4e38f;
                u.light_spot[n][0] = std::cos(l.theta * 0.5f);
                u.light_spot[n][1] = std::cos(l.phi * 0.5f);
                u.light_spot[n][2] = l.falloff;
                ++n;
            }
        }
        for (int s = 0; s < kMaxStages; ++s) {
            u.bump_matrix[s][0] = as_float(tss_[s][kTssBumpEnvMat00]);
            u.bump_matrix[s][1] = as_float(tss_[s][kTssBumpEnvMat01]);
            u.bump_matrix[s][2] = as_float(tss_[s][kTssBumpEnvMat10]);
            u.bump_matrix[s][3] = as_float(tss_[s][kTssBumpEnvMat11]);
            u.bump_luminance[s][0] = as_float(tss_[s][kTssBumpEnvLScale]);
            u.bump_luminance[s][1] = as_float(tss_[s][kTssBumpEnvLOffset]);
        }
        renderer_.upload_ffp(u);
    }
    if (!ffp && (vs_constants_dirty_ || renderer_.block_stale(1))) {
        renderer_.upload_vs_constants(vs_constants_, kVsConstants);
        vs_constants_dirty_ = false;
    }
}

bool Device::prepare_draw() {
    if (!owner_thread()) {
        log_unimplemented("drawing from a second thread");
        ++stats_.no_thread;
        return false;
    }
    if (!stream_ || stride_ == 0 || !render_target_) {
        ++stats_.no_stream;
        return false;
    }
    // [Graphics] PixelLighting: the program changes with the setting (the developer menu).
    if (render::surface_params().pixel_lighting != surface_pl_) {
        surface_pl_ = render::surface_params().pixel_lighting;
        ++program_version_;
    }
    // [Graphics] Water: a river draw (Plane::draw marks it) with the river shader, unblended (it shades the
    // scene behind the water itself).
    const int river_stage = render::water_river_stage();
    // Only the main view: not a 1024x768 screen's small 3D views (the mission brief's map), whose picture the
    // scene copies are not of.
    river_now_ = river_stage > 0 && water_.river_ready() && !display::canvas_armed() && !layout_viewport() &&
                 render_target_ == back_buffer_ && viewport_.width >= 640;
    bind_render_target();
    apply_raster_state();
    bool ffp = true;
    const GLuint program = program_for_draw(ffp);
    if (!program) {
        ++stats_.no_program;
        return false;
    }
    ++stats_.draws;
    if (canvas_on()) ++canvas_draws_;
    else ++plain_draws_;
    // 3D = a vertex shader or a perspective projection (the 2D layer uses an orthographic one).
    if ((vertex_shader_ & 1) || transform(kTsProjection).m[11] != 0.0f) {
        drew_3d_ = true;
        scene_bottom_ = std::max<std::uint32_t>(scene_bottom_, static_cast<std::uint32_t>(viewport_.y + viewport_.height));
    }
    last_program_ = program;
    GLuint draw_program = program;
    if (river_now_) {
        if (const GLuint rp = water_.river_program(renderer_, program, river_stage - 1)) {
            draw_program = rp;
        } else {
            river_now_ = false;
            apply_raster_state();  // blended as the game draws it
        }
    }
    renderer_.use_program(draw_program);
    last_draw_program_ = draw_program;  // [Graphics] GrassBatching records them
    last_ffp_ = ffp;
    const GLuint vao = vertex_array_for(stride_);
    // A cached model's vertices in the geometry pool: the pool stays bound at 0 and the draw starts at the model's
    // first vertex (no binding call when the previous draw was a pooled one too).
    GLintptr vertex_offset = stream_->offset();
    pool_base_ = 0;
    if (stream_->pooled() && vertex_offset % static_cast<GLintptr>(stride_) == 0) {
        pool_base_ = static_cast<std::int32_t>(vertex_offset / static_cast<GLintptr>(stride_));
        vertex_offset = 0;
    }
    renderer_.bind_geometry(vao, stream_->gl(), stride_, indices_ ? indices_->gl() : 0, vertex_offset);
    for (DWORD s = 0; s < kMaxStages; ++s) {
        if (textures_[s]) renderer_.bind_texture(s, textures_[s]->gl(), stage_sampler(s));
    }
    if (draw_lift_) {  // [Graphics] BakedShadows: the level map's lit level (and its uniform block)
        if (const GLuint lit = baked_.texture()) renderer_.bind_texture(kLiftUnit, lit, 0);
    }
    if (river_now_) water_.river_bind(draw_program);
    river_now_ = false;
    upload_uniforms(ffp);
    return true;
}

// [Graphics] BakedShadows: stage `s` (> 0, with a texture) as the ground's lightmap stage - the texture
// times the colour so far, read with the second texture coordinates, clamped (SysEn texture modes 18 and
// 19; nothing else in the game sets a stage up like that): lifted by the level map (render/baked_shadows.hpp).
bool Device::lightmap_stage(DWORD s) const {
    if (s == 0 || s >= kMaxStages || !textures_[s] || textures_[s]->render_target()) return false;
    const DWORD* const ts = tss_[s];
    return ts[kTssColorOp] == kTopModulate && ts[kTssTexCoordIndex] == 1 && ts[kTssAddressU] == kTaddressClamp &&
           ts[kTssAddressV] == kTaddressClamp &&
           ((ts[kTssColorArg1] == kTaTexture && ts[kTssColorArg2] == kTaCurrent) ||
            (ts[kTssColorArg1] == kTaCurrent && ts[kTssColorArg2] == kTaTexture));
}


// Native draws (N2): the part of prepare_draw before the program is chosen, without the stream
// checks (native code binds its own geometry through the renderer). Not used yet.
bool Device::begin_native_draw() {
    if (!owner_thread()) {
        log_unimplemented("drawing from a second thread");
        ++stats_.no_thread;
        return false;
    }
    if (!render_target_) return false;
    bind_render_target();
    apply_raster_state();
    return true;
}

bool Device::shadow_volume(const render::VolumeSource& src, const float* dir, float length, bool upload, bool* both) {
    if (!begin_native_draw()) return false;
    upload_uniforms(true);  // the world transform the caller set, the camera
    ++stats_.draws;
    ++plain_draws_;
    // Both stencil passes in one draw: the faces this pass draws (the ones its cull mode keeps) increment, the
    // others - the second pass's - decrement, culling off. The original's saturating INCR then DECR give the net
    // count of a z-pass volume (never below its start), which wrapping gives in any order. Only for the plain
    // z-pass state the caller sets (fail / depth-fail keep).
    const DWORD cull = rs_[kRsCullMode];
    const bool two = both && rs_[kRsStencilEnable] && depth_stencil_ && (cull == 2 || cull == 3) && rs_[kRsStencilPass] == 4 &&
                     stencil_op(rs_[kRsStencilFail]) == GL_KEEP && stencil_op(rs_[kRsStencilZFail]) == GL_KEEP;
    if (!two) return volumes_.draw(renderer_, src, dir, length, upload, presented_);
    bool cull_back = cull == 2;  // as apply_raster_state
    if (hook::settings().cull_flip) cull_back = !cull_back;
    const GLenum culled = cull_back ? GL_BACK : GL_FRONT, kept = cull_back ? GL_FRONT : GL_BACK;
    glDisable(GL_CULL_FACE);
    glStencilOpSeparate(kept, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    glStencilOpSeparate(culled, GL_KEEP, GL_KEEP, GL_DECR_WRAP);
    const bool drawn = volumes_.draw(renderer_, src, dir, length, upload, presented_);
    // Back to the state the Renderer recorded (cull on with its face, one stencil op for both faces).
    glEnable(GL_CULL_FACE);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    *both = drawn;
    return drawn;
}

// ---------------------------------------------------------------------------
// [Graphics] GrassBatching (render/grass_batch.hpp)
// ---------------------------------------------------------------------------
// Passes that need every draw to go through prepare_draw: the shadow map's and the foliage's recording, a 1024x768
// screen's camera. (The ambient occlusion's cut-out window is kept by the replay: the recorded cut-out flag.)
bool Device::grass_usable() const {
    return hook::settings().grass_batching != 0 && owner_thread() && render_target_ && !shadow_recording_ && !foliage_recording_ &&
           !canvas_on() && !layout_viewport();
}

std::uint32_t Device::light_signature() const {
    if (light_signature_version_ == light_version_) return light_signature_;
    std::uint32_t h = 2166136261u;
    for (const auto& [index, on] : light_enabled_) {
        if (!on) continue;
        const auto it = lights_.find(index);
        h = (h ^ index) * 16777619u;
        h = (h ^ (it != lights_.end() ? it->second.type : 0u)) * 16777619u;
    }
    light_signature_ = h;
    light_signature_version_ = light_version_;
    return h;
}

void Device::grass_release(GrassRecording& r) {
    for (GrassCmd& c : r.cmds) {
        for (Texture* t : c.textures)
            if (t) t->Release();
        if (c.vertices) c.vertices->Release();
        if (c.indices) c.indices->Release();
    }
    r.cmds.clear();
    r.ok = false;
}

bool Device::grass_record_begin(const render::GrassKey& key) {
    if (!grass_usable() || grass_recording_) return false;
    auto& list = grass_[key.plane];
    GrassRecording* r = nullptr;
    for (auto& e : list)
        if (e->ctx == key.ctx && e->variant == key.variant) r = e.get();
    if (!r) {
        r = list.emplace_back(std::make_unique<GrassRecording>()).get();
        r->plane = key.plane;
    }
    grass_release(*r);
    r->ctx = key.ctx;
    r->variant = key.variant;
    r->lights = light_signature();
    r->used = presented_;
    r->ok = true;
    grass_recording_ = r;
    grass_colour_set_ = false;
    return true;
}

void Device::grass_record_colour(std::uint32_t colour) {
    grass_colour_ = colour;
    grass_colour_set_ = true;
}

// A draw of the patch being recorded, just prepared: the state it was made with. Only an FFP draw from the cached
// (pooled) arrays of 16-bit indices can be drawn again as it is; anything else spoils the recording.
void Device::grass_record_draw(Enum prim, UINT start_index, UINT prim_count) {
    GrassRecording& r = *grass_recording_;
    if (!r.ok) return;
    std::uint8_t why = 0;
    if (!last_ffp_) why = 1;
    else if (!stream_ || !indices_ || !stream_->pooled() || !indices_->pooled()) why = 2;
    else if (indices_->format() == kFmtIndex32) why = 3;
    else if (stride_ == 0 || stream_->offset() % static_cast<GLintptr>(stride_) != 0) why = 4;
    else if (render::water_river_stage() != 0 || draw_lift_ || last_worlds_ > 2) why = 5;  // (GrassCmd keeps WORLD..WORLD1)
    if (why) {
        r.ok = false;
        r.why = why;
        return;
    }
    GrassCmd c;
    c.program = last_draw_program_;
    c.vao = vertex_array_for(stride_);
    c.stride = stride_;
    c.raster = last_raster_;
    for (DWORD s = 0; s < kMaxStages; ++s) {
        if (!textures_[s]) continue;
        c.textures[s] = textures_[s];
        c.textures[s]->AddRef();
        c.samplers[s] = stage_sampler(s);
    }
    c.vertices = stream_;
    c.vertices->AddRef();
    c.indices = indices_;
    c.indices->AddRef();
    c.mode = primitive_mode(prim);
    c.count = static_cast<GLsizei>(vertex_count(prim, prim_count));
    c.index_byte = start_index * 2u;
    c.base_vertex = static_cast<std::int32_t>(base_vertex_);
    static_assert(offsetof(FfpDrawUniforms, alpha_ref) + sizeof(FfpDrawUniforms::alpha_ref) - offsetof(FfpDrawUniforms, material_diffuse) == sizeof(GrassCmd::tail) &&
                      offsetof(FfpDrawUniforms, alpha_ref) + sizeof(FfpDrawUniforms::alpha_ref) == sizeof(FfpDrawUniforms),
                  "GrassCmd::tail = the record from material_diffuse to its end");
    std::memcpy(c.world0, draw_uniforms_.world[0], sizeof c.world0);
    std::memcpy(c.tail, &draw_uniforms_.material_diffuse, sizeof c.tail);
    c.normals = last_normals_;
    if (!c.normals) {
        c.normals_kept = std::make_unique<float[]>(32);
        std::memcpy(c.normals_kept.get(), draw_uniforms_.normal_matrix[0], 128);
    }
    c.alpha_cutout = alpha_cutout_state();
    c.has_colour = grass_colour_set_;
    c.material_colour = grass_colour_;
    r.cmds.push_back(std::move(c));
}

void Device::grass_record_end() {
    if (!grass_recording_) return;
    if (grass_recording_->ok) ++grass_stats_.recorded;
    else {
        ++grass_stats_.failed;
        ++grass_stats_.why[grass_recording_->why];
        grass_release(*grass_recording_);
    }
    grass_recording_ = nullptr;
}

void Device::grass_forget(const void* plane) {
    const auto it = grass_.find(plane);
    if (it == grass_.end()) return;
    for (auto& r : it->second) grass_release(*r);
    grass_.erase(it);
    ++grass_gen_;
}

// Recordings not drawn for a while (their patches gone without grass_forget: a level left) are dropped.
void Device::grass_sweep() {
    if (presented_ - grass_swept_ < 600) return;
    grass_swept_ = presented_;
    for (auto it = grass_.begin(); it != grass_.end();) {
        auto& list = it->second;
        for (auto& r : list)
            if (presented_ - r->used > 600) grass_release(*r);
        list.erase(std::remove_if(list.begin(), list.end(), [](const auto& r) { return r->cmds.empty() && !r->ok; }), list.end());
        it = list.empty() ? grass_.erase(it) : std::next(it);
    }
    ++grass_gen_;  // (whether or not any went)
    unsigned queued = 0, calls = 0, why[11] = {};
    renderer_.take_queue_stats(queued, calls, why);
    if (hook::settings().perf_log > 0)
        hook::log_line("[Graphics] grass multi-draw: submitted by raster %u, program %u, geometry %u, texture %u, viewport %u, uniforms %u, "
                       "ring %u, other draw %u, invalidate %u, caller %u, primitive %u",
                       why[0], why[1], why[2], why[3], why[4], why[5], why[6], why[7], why[8], why[9], why[10]);
    if (hook::settings().perf_log > 0)
        hook::log_line("[Graphics] grass batching (600 frames): %u patch draws replayed (%u draws in %u multi-draw calls, %u looked up), %u recorded, "
                       "%u not recordable (unindexed %u, not FFP %u, arrays not cached %u, 32-bit indices %u, offset %u, river/lift %u), %u "
                       "re-recorded for other lights; %zu patches kept",
                       grass_stats_.replays, queued, calls, grass_stats_.looked_up, grass_stats_.recorded, grass_stats_.failed, grass_stats_.why[0],
                       grass_stats_.why[1], grass_stats_.why[2], grass_stats_.why[3], grass_stats_.why[4], grass_stats_.why[5],
                       grass_stats_.relit, grass_.size());
    grass_stats_ = {};
}

bool Device::grass_replay(const render::GrassKey& key, const float* wind, std::uint32_t colour, bool fading) {
    grass_sweep();
    if (!grass_usable() || grass_recording_) return false;
    GrassRecording* r = nullptr;
    const bool last = grass_last_ && grass_last_gen_ == grass_gen_;
    if (last && grass_last_->next_gen == grass_gen_) {  // the one replayed after the last one, last time
        GrassRecording* const n = grass_last_->next;
        if (n->plane == key.plane && n->ctx == key.ctx && n->variant == key.variant) r = n;
    }
    if (!r) {
        ++grass_stats_.looked_up;
        const auto it = grass_.find(key.plane);
        if (it == grass_.end()) return false;
        for (auto& e : it->second)
            if (e->ctx == key.ctx && e->variant == key.variant) r = e.get();
        if (!r) return false;
    }
    if (last) {
        grass_last_->next = r;
        grass_last_->next_gen = grass_gen_;
    }
    grass_last_ = r;
    grass_last_gen_ = grass_gen_;
    if (!r->ok) return false;
    if (r->lights != light_signature()) {  // the sector's lights differ (the program): draw it the normal way again
        ++grass_stats_.relit;
        return false;
    }
    r->used = presented_;
    ++grass_stats_.replays;
    if (r->cmds.empty()) return true;  // nothing of the patch is drawn in this pass

    bind_render_target();
    apply_viewport_scissor();  // the viewport and scissor of the target (each recorded draw applies its raster state)
    bool lit = false;
    for (const GrassCmd& c : r->cmds) lit = lit || c.normals;
    upload_frame_block(true, lit);
    const Matrix& view = transform(kTsView);
    if (lit && grass_normal_version_ != frame_version_) {  // world and wind are translations: the view's normal matrix
        grass_normal_ = normal_matrix(view);
        grass_normal_version_ = frame_version_;
    }
    const bool multi = hook::settings().grass_multi_draw != 0;  // [Graphics] GrassMultiDraw
    // [Graphics] GrassBindless: the textures come from handles in the draw's record, so consecutive draws of
    // different textures stay in one multi-draw.
    const bool bindless = multi && hook::settings().grass_bindless != 0 && renderer_.bindless();
    for (const GrassCmd& c : r->cmds) {
        std::uint8_t mask = 0;
        if (bindless) {
            if (c.bindless_state == 0) {
                c.bindless_state = 2;
                std::uint8_t m = 0;
                bool ok = true;
                for (DWORD s = 0; s < kMaxStages; ++s)
                    if (c.textures[s]) {
                        if (s < 3) m = static_cast<std::uint8_t>(m | (1u << s));
                        else ok = false;  // the record holds handles for stages 0..2
                    }
                const GLuint v = ok && m ? renderer_.bindless_variant(c.program, m) : 0;
                ok = v != 0;
                for (DWORD s = 0; s < 3 && ok; ++s)
                    if ((m >> s) & 1u) ok = (c.handles[s] = renderer_.texture_handle(c.textures[s]->gl(), c.samplers[s])) != 0;
                if (ok) {
                    c.bindless_program = v;
                    c.bindless_mask = m;
                    c.bindless_state = 1;
                }
            }
            if (c.bindless_state == 1) mask = c.bindless_mask;
        }
        renderer_.apply(c.raster);
        renderer_.use_program(mask ? c.bindless_program : c.program);
        for (DWORD s = 0; s < kMaxStages; ++s)
            if (c.textures[s] && !(s < 3 && ((mask >> s) & 1u))) renderer_.bind_texture(s, c.textures[s]->gl(), c.samplers[s]);
        renderer_.bind_geometry(c.vao, c.vertices->gl(), c.stride, c.indices->gl(), 0);
        // Built in cached memory and copied into the ring in one go: written piecewise there (write-combined,
        // records not 64-byte aligned) it cost three times as much.
        // grass_rec_: WORLD2..3 and their normal matrices stay 0 (a grass program does not read them).
        FfpDrawUniforms& rec = grass_rec_;
        std::memcpy(rec.world[0], c.world0, 64);
        std::memcpy(rec.world[1], wind, 64);
        if (c.normals) {
            std::memcpy(rec.normal_matrix[0], grass_normal_.m, 64);
            std::memcpy(rec.normal_matrix[1], grass_normal_.m, 64);
        } else {
            std::memcpy(rec.normal_matrix[0], c.normals_kept.get(), 128);
        }
        std::memcpy(&rec.material_diffuse, c.tail, sizeof c.tail);
        if (fading && c.has_colour) {  // Plane::draw: the texture factor = fade colour x material colour / 255, per byte
            std::uint32_t factor = 0;
            for (std::uint32_t b = 0; b < 4; ++b) {
                const std::uint32_t v = ((colour >> (b * 8)) & 0xff) * ((c.material_colour >> (b * 8)) & 0xff) / 255;
                factor |= (v & 0xff) << (b * 8);
            }
            color4(factor, rec.texture_factor);
        }
        if (mask) {  // the handles: words material_power.yzw, alpha_ref.yzw = stage 0 lo, hi, stage 1 lo, hi, stage 2 lo, hi
            std::uint32_t w[6] = {};
            for (int s = 0; s < 3; ++s) {
                w[2 * s] = static_cast<std::uint32_t>(c.handles[s]);
                w[2 * s + 1] = static_cast<std::uint32_t>(c.handles[s] >> 32);
            }
            std::memcpy(&rec.material_power[1], &w[0], 12);
            std::memcpy(&rec.alpha_ref[1], &w[3], 12);
        }
        renderer_.upload_ffp_draw(rec);
        if (ao_window_) ao_prepare_cutout(c.alpha_cutout && ao_cutout_target());  // as DrawIndexedPrimitive's ao_prepare_cutout
        const GLint base = static_cast<GLint>(c.vertices->offset() / static_cast<GLintptr>(c.stride)) + c.base_vertex;
        const std::uintptr_t index_offset = static_cast<std::uintptr_t>(c.indices->offset()) + c.index_byte;
        if (multi) renderer_.queue_elements(c.mode, c.count, index_offset, base);  // grass_flush / any other draw submits
        else renderer_.draw_elements(c.mode, c.count, false, index_offset, base);
        ++stats_.draws;
        ++plain_draws_;
        ++grass_stats_.replayed_draws;
    }
    draw_key_valid_ = false;  // block FfpDraw holds a grass record now
    return true;
}

// [Graphics] AmbientOcclusion (render/ambient_occlusion.*): only the 3D world drawn by a
// perspective camera into the back buffer - not render-to-texture (mirrors); a camera of a 1024x768
// screen (the loading view) where it is placed on the canvas (scene_viewport).
void Device::ambient_occlusion(const float* projection) {
    ao_close_window();
    if (!owner_thread() || !back_buffer_ || render_target_ != back_buffer_ || !depth_stencil_) return;
    if (depth_stencil_->width != back_buffer_->width || depth_stencil_->height != back_buffer_->height) return;
    const auto bw = static_cast<int>(back_buffer_->width), bh = static_cast<int>(back_buffer_->height);
    int vx = 0, vy = 0, vw = 0, vh = 0;
    scene_viewport(vx, vy, vw, vh);  // a 1024x768 screen's camera (the loading view) as placed on the canvas
    if (vx < 0 || vy < 0 || vw < 64 || vh < 64) return;
    std::memcpy(ao_projection_, projection, sizeof ao_projection_);
    ao_in_ = {framebuffer_for(back_buffer_, depth_stencil_), bw, bh, vx, vy, vw, vh, viewport_.min_z, viewport_.max_z,
              ao_projection_};
    // Phase 2 window: until Scene::render's transparent pass is drawn (ao_cutout_end), the scene
    // framebuffer has the occlusion's cut-out target as draw buffer 1.
    const int samples = back_buffer_->gl_msaa ? msaa_samples_ : 0;
    if (ao_.run(ao_in_) && ao_.begin_cutout(ao_in_.scene_fbo, samples)) {
        ao_window_ = true;
        ao_viewport_ = viewport_;
        ao_cutout_draws_ = 0;
        ao_cut_valid_ = false;  // begin_cutout set buffer 1's mask itself
    }
    ao_restore_state();
}

void Device::ao_close_window() {
    if (!ao_window_) return;
    ao_window_ = false;
    ao_.end_cutout();
}

// The GL state the occlusion passes change and the next draw does not re-apply.
void Device::ao_restore_state() {
    renderer_.invalidate_state();  // the passes set raster state, program, vertex array and texture units themselves
    glBindFramebuffer(GL_FRAMEBUFFER, current_fbo_);
    for (DWORD s = 0; s < 6; ++s) {  // the shadow pass binds units 0..5
        if (textures_[s]) {
            renderer_.bind_texture(s, textures_[s]->gl(), renderer_.sampler(sampler_desc(s)));
        } else {
            glBindTextureUnit(s, 0);
            glBindSampler(s, 0);
        }
    }
}

// Phase 2 window: is the draw just made an alpha-tested, depth-writing draw of the camera that
// phase 1 ran for (foliage, fences ... in the transparent pass)? Alpha reference >= 0x40 leaves
// out water and fading units (SysEn raster mode 0x100: reference 4); draws without depth writes
// (effects, decals, projected textures) are left out by the depth write test.
bool Device::ao_cutout_draw() const { return ao_cutout_target() && alpha_cutout_state(); }

// The render states of a cut-out draw (alpha-tested, depth-writing): see ao_cutout_draw.
bool Device::alpha_cutout_state() const {
    if (!rs_[kRsZEnable] || !rs_[kRsZWriteEnable] || !rs_[kRsAlphaTestEnable]) return false;
    const DWORD func = rs_[kRsAlphaFunc];
    return (func == 5 || func == 7) && (rs_[kRsAlphaRef] & 0xffu) >= 0x40u;  // GREATER / GREATEREQUAL
}

// Phase 2 window, drawing into the scene framebuffer of the camera phase 1 ran for.
bool Device::ao_cutout_target() const {
    if (!ao_window_ || current_fbo_ != ao_in_.scene_fbo || render_target_ != back_buffer_) return false;
    return viewport_.x == ao_viewport_.x && viewport_.y == ao_viewport_.y && viewport_.width == ao_viewport_.width &&
           viewport_.height == ao_viewport_.height;
}

// Phase 2 window, before a draw (its raster state applied): into the scene framebuffer, the
// fragment shader's depth output (draw buffer 1) reaches the cut-out target only for a cut-out
// draw. The draw itself is made once, as without the occlusion.
void Device::ao_prepare_cutout() { ao_prepare_cutout(ao_cutout_draw()); }
void Device::ao_prepare_cutout(bool cutout) {
    if (!ao_window_ || current_fbo_ != ao_in_.scene_fbo) return;
    if (cutout) ++ao_cutout_draws_;
    // Buffer 1's mask / blend stay set until the cut-out flag changes or the renderer sets every buffer's.
    const std::uint32_t epoch = renderer_.blend_mask_epoch();
    if (ao_cut_valid_ && cutout == ao_cut_last_ && epoch == ao_cut_epoch_) return;
    renderer_.flush_queued();  // GL state set here directly
    render::AoPass::cutout_draw_state(cutout);
    ao_cut_valid_ = true;
    ao_cut_last_ = cutout;
    ao_cut_epoch_ = epoch;
}

// [Graphics] Shadows=2 (render/shadow_map.hpp): the scene's opaque pass starts - record its draws
// into the back buffer when there is a shadow-casting sun.
// render::overlay_begin / overlay_end (Scene::render, around the billboards drawn before the world: unit
// name labels, health plates). With a screen-space pass on, the depth buffer is copied at both ends: the
// buffer already holds depths before the billboards (the ground and the sky of the scene), so a billboard
// pixel is one whose depth the billboards changed and whose final depth is still theirs - nothing was drawn
// over it. The shadow / water passes leave those alone (their depth is not a surface in the world).
void Device::overlay(bool begin) {
    if (!begin && !overlay_begun_) return;
    overlay_begun_ = false;
    if (!owner_thread() || !back_buffer_ || render_target_ != back_buffer_ || !depth_stencil_) return;
    if (depth_stencil_->width != back_buffer_->width || depth_stencil_->height != back_buffer_->height) return;
    if (render::shadow_params().mode != 2 && render::water_params().mode != 1) return;
    const int w = static_cast<int>(back_buffer_->width), h = static_cast<int>(back_buffer_->height);
    if (overlay_w_ != w || overlay_h_ != h) {
        glDeleteFramebuffers(2, overlay_fbo_);
        glDeleteTextures(2, overlay_depth_);
        for (int i = 0; i < 2; ++i) {
            glCreateTextures(GL_TEXTURE_2D, 1, &overlay_depth_[i]);
            glTextureStorage2D(overlay_depth_[i], 1, GL_DEPTH24_STENCIL8, w, h);
            glTextureParameteri(overlay_depth_[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(overlay_depth_[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glCreateFramebuffers(1, &overlay_fbo_[i]);
            glNamedFramebufferTexture(overlay_fbo_[i], GL_DEPTH_STENCIL_ATTACHMENT, overlay_depth_[i], 0);
            glNamedFramebufferDrawBuffer(overlay_fbo_[i], GL_NONE_);
            glNamedFramebufferReadBuffer(overlay_fbo_[i], GL_NONE_);
        }
        overlay_w_ = w;
        overlay_h_ = h;
    }
    glDisable(GL_SCISSOR_TEST);
    glBlitNamedFramebuffer(framebuffer_for(back_buffer_, depth_stencil_), overlay_fbo_[begin ? 0 : 1], 0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    renderer_.invalidate_state();  // scissor
    if (begin) {
        overlay_begun_ = true;
        overlay_valid_ = false;
    } else {
        overlay_valid_ = true;
    }
}

void Device::shadow_begin(const float* sun) {
    shadow_draws_.clear();
    shadow_recording_ = false;
    foliage_recording_ = false;
    const render::ShadowParams& p = render::shadow_params();
    const bool baked = sun && p.mode == 2 && p.baked < 1.0f;
    if (baked != baked_on_) ++program_version_;  // the ground's programs change (kPixelLift)
    baked_on_ = baked;
    if (!sun || p.mode != 2 || !owner_thread() || !back_buffer_ || !depth_stencil_) return;
    shadow_scene_fbo_ = framebuffer_for(back_buffer_, depth_stencil_);
    std::memcpy(shadow_sun_, sun, sizeof shadow_sun_);
    shadow_recording_ = true;
    record_stream_switches_ = stream_ring_.switches;
    record_uniform_switches_ = renderer_.ring_switches();
    renderer_.clear_deleted_buffers();
}

// The recorded draws still name the data they were drawn with: neither ring has come back round to where
// they were written. A frame that streams more than that (a level's first frames) would replay draws over
// overwritten vertices / indices - out-of-range reads that hung the GPU (driver reset); its shadows are
// skipped instead.
bool Device::recorded_intact() const {
    if (stream_ring_.switches - record_stream_switches_ + 1 >= StreamRing::kSegments) return false;
    return renderer_.ring_switches() - record_uniform_switches_ + 1 < render::Renderer::ring_segments();
}

// Recorded draws naming a buffer deleted since (its name may already belong to a new, smaller buffer) are
// dropped - the rest of the shadow map stays.
void Device::drop_deleted(std::vector<render::ShadowDraw>& draws) {
    const std::vector<GLuint>& gone = renderer_.deleted_buffers();
    if (gone.empty()) return;
    auto named = [&gone](GLuint b) { return b != 0 && std::find(gone.begin(), gone.end(), b) != gone.end(); };
    draws.erase(std::remove_if(draws.begin(), draws.end(),
                               [&](const render::ShadowDraw& d) { return named(d.vertex_buffer) || named(d.index_buffer); }),
                draws.end());
}

// The recordable parts of the draw just prepared (program, geometry, uniform ranges, cut-out texture).
render::ShadowDraw Device::shadow_draw_now(GLenum mode, GLsizei count, GLint first, bool indexed, bool wide,
                                           std::uintptr_t index_offset) {
    render::ShadowDraw d{};
    d.program = last_program_;
    d.vao = vertex_array_for(stride_);
    d.vertex_buffer = stream_->gl();
    d.vertex_offset = stream_->offset();
    d.stride = stride_;
    d.index_buffer = indices_ ? indices_->gl() : 0;
    d.index_offset = index_offset;
    d.mode = mode;
    d.count = count;
    d.first = first;
    d.base_vertex = static_cast<GLint>(base_vertex_);
    d.indexed = indexed;
    d.wide = wide;
    d.ffp_offset = renderer_.uniform_offset(0);
    d.vs_offset = renderer_.uniform_offset(1);
    d.draw_offset = renderer_.uniform_offset(3);
    if (rs_[kRsAlphaTestEnable] && textures_[0]) {  // cut-outs keep their shape in the shadow map
        d.texture = textures_[0]->gl();
        d.sampler = renderer_.sampler(sampler_desc(0));
    }
    for (DWORD s = 1; s < static_cast<DWORD>(kLiftUnit) && tss_[s][kTssColorOp] != kTopDisable; ++s) {
        if (!lightmap_stage(s)) continue;
        d.lightmap = textures_[s]->gl();  // [Graphics] BakedShadows: the ground, for the level map
        d.lightmap_sampler = renderer_.sampler(sampler_desc(s));
        d.lightmap_stage = static_cast<int>(s);
        break;
    }
    return d;
}

// ShadowFoliage, between shadow_done and the end of the transparent pass: an alpha-tested, depth-writing
// 3D draw of the main view (as the occlusion's cut-outs: reference >= 0x40 leaves out water and fading
// units; effects write no depth). Blended or not: trees are drawn with both.
void Device::foliage_record(GLenum mode, GLsizei count, GLint first, bool indexed, bool wide, std::uintptr_t index_offset) {
    if (current_fbo_ != shadow_scene_fbo_ || render_target_ != back_buffer_ || canvas_on()) return;
    if (!rs_[kRsZEnable] || !rs_[kRsZWriteEnable] || !rs_[kRsAlphaTestEnable] || !textures_[0]) return;
    const DWORD func = rs_[kRsAlphaFunc];
    if ((func != 5 && func != 7) || (rs_[kRsAlphaRef] & 0xffu) < 0x40u) return;  // GREATER / GREATEREQUAL
    if (!(vertex_shader_ & 1) && (vertex_shader_ & kFvfPositionMask) == kFvfXyzRhw) return;
    if (renderer_.uniform_offset(0) == ~0u) return;
    ++shadow_stats_.foliage;
    foliage_draws_.push_back(shadow_draw_now(mode, count, first, indexed, wide, index_offset));
}

// A draw just prepared: a 3D, depth-writing, opaque draw of the main view is kept for the shadow map
// (its uniform blocks stay in the uniform ring, its vertices in their buffers, for the rest of the frame).
void Device::shadow_record(GLenum mode, GLsizei count, GLint first, bool indexed, bool wide, std::uintptr_t index_offset) {
    ShadowStats& st = shadow_stats_;
    if (current_fbo_ != shadow_scene_fbo_ || render_target_ != back_buffer_ || canvas_on()) {
        ++st.other_target;
        return;
    }
    if (!rs_[kRsZEnable] || !rs_[kRsZWriteEnable]) {
        ++st.no_depth;
        return;
    }
    if (rs_[kRsAlphaBlendEnable]) {  // effects, glass ... (units are drawn unblended)
        ++st.blended;
        return;
    }
    if ((rs_[kRsColorWriteEnable] & 7) == 0) {
        ++st.no_colour;
        return;
    }
    if (!(vertex_shader_ & 1) && (vertex_shader_ & kFvfPositionMask) == kFvfXyzRhw) {  // 2D
        ++st.two_d;
        return;
    }
    if (renderer_.uniform_offset(0) == ~0u) return;
    ++st.recorded;
    shadow_draws_.push_back(shadow_draw_now(mode, count, first, indexed, wide, index_offset));
}

// The opaque world is complete (render::shadow_opaque_done): shadow map and shadows for the main view.
void Device::shadow_done(const float* projection) {
    if (!shadow_recording_) return;
    shadow_recording_ = false;
    if (!recorded_intact()) {  // see recorded_intact
        if (skipped_logged_++ < 30) hook::log_line("[Graphics] Shadows: frame streamed past its recorded draws - shadows skipped");
        shadows_skipped_ = true;
        shadow_draws_.clear();
        return;
    }
    drop_deleted(shadow_draws_);
    if (shadow_draws_.empty() || !owner_thread() || !back_buffer_ || render_target_ != back_buffer_ || !depth_stencil_) return;
    if (depth_stencil_->width != back_buffer_->width || depth_stencil_->height != back_buffer_->height) return;
    const auto bw = static_cast<int>(back_buffer_->width), bh = static_cast<int>(back_buffer_->height);
    int vx = 0, vy = 0, vw = 0, vh = 0;
    scene_viewport(vx, vy, vw, vh);  // a 1024x768 screen's camera (the loading view) as placed on the canvas
    if (vx < 0 || vy < 0 || vw < 64 || vh < 64) return;
    render::ShadowPass::Input in{shadow_scene_fbo_, bw, bh, vx, vy, vw, vh, viewport_.min_z, viewport_.max_z,
                                      projection, transform(kTsView).m, shadow_sun_};
    in.overlay_depth = overlay_valid_ ? overlay_depth_[1] : 0;
    in.overlay_before = overlay_valid_ ? overlay_depth_[0] : 0;
    // [Graphics] BakedShadows: the level map from this frame's ground (the ground of the next frame reads it).
    if (baked_on_) baked_.build(renderer_, shadow_draws_, transform(kTsView).m, projection, render::shadow_params().distance,
                                render::shadow_params().baked);
    const bool ran = shadow_.run(renderer_, in, shadow_draws_);
    shadow_draws_.clear();
    ao_restore_state();  // the renderer's state and the texture units
    renderer_.rebind_uniforms();
    if (ran && render::shadow_params().foliage) {  // the transparent pass follows: record its cut-outs
        foliage_draws_.clear();
        std::memcpy(foliage_view_, transform(kTsView).m, sizeof foliage_view_);
        std::memcpy(foliage_projection_, projection, sizeof foliage_projection_);
        foliage_in_ = in;
        foliage_in_.projection = foliage_projection_;
        foliage_in_.view = foliage_view_;
        foliage_in_.sun = shadow_sun_;
        foliage_recording_ = true;
    }
}

// [Graphics] Water=1 (render/water.hpp), when the en3d scene's opaque world is complete: the modern water
// over the main view's back buffer, with the device's current fog (as the game's water would be fogged).
bool Device::water_pass(const float* projection, const float* sun) {
    if (!owner_thread() || !back_buffer_ || render_target_ != back_buffer_ || !depth_stencil_) return false;
    // Only the main view: a 1024x768 screen's small 3D views (the mission brief's map, portraits) keep the
    // game's own water (the pass shaded a strip of their viewport over the screen).
    // The loading screen's view keeps it.
    if ((display::canvas_armed() || layout_viewport()) && !display::loading_drawing()) return false;
    if (depth_stencil_->width != back_buffer_->width || depth_stencil_->height != back_buffer_->height) return false;
    const auto bw = static_cast<int>(back_buffer_->width), bh = static_cast<int>(back_buffer_->height);
    int vx = 0, vy = 0, vw = 0, vh = 0;
    scene_viewport(vx, vy, vw, vh);  // a 1024x768 screen's camera (the loading view) as placed on the canvas
    if (vx < 0 || vy < 0 || vw < 64 || vh < 64) return false;
    render::WaterPass::Input in{};
    in.scene_fbo = framebuffer_for(back_buffer_, depth_stencil_);
    in.width = bw;
    in.height = bh;
    in.vx = vx;
    in.vy = vy;
    in.vw = vw;
    in.vh = vh;
    in.depth_min = viewport_.min_z;
    in.depth_max = viewport_.max_z;
    in.projection = projection;
    in.view = transform(kTsView).m;
    in.sun = sun;
    in.overlay_depth = overlay_valid_ ? overlay_depth_[1] : 0;
    in.overlay_before = overlay_valid_ ? overlay_depth_[0] : 0;
    const DWORD fog_mode = rs_[kRsFogTableMode] ? rs_[kRsFogTableMode] : rs_[kRsFogVertexMode];
    in.fog = rs_[kRsFogEnable] && fog_mode == 3;  // linear
    in.range_fog = rs_[kRsRangeFogEnable] != 0;
    std::memcpy(&in.fog_start, &rs_[kRsFogStart], 4);
    std::memcpy(&in.fog_end, &rs_[kRsFogEnd], 4);
    const DWORD c = rs_[kRsFogColor];
    in.fog_colour[0] = static_cast<float>((c >> 16) & 0xff) / 255.0f;
    in.fog_colour[1] = static_cast<float>((c >> 8) & 0xff) / 255.0f;
    in.fog_colour[2] = static_cast<float>(c & 0xff) / 255.0f;
    const bool drawn = water_.run(in, render::water_rects(), render::water_falls());
    if (drawn || water_.state_changed()) {  // also the copies made for the rivers alone
        ao_cut_valid_ = false;  // draw buffer 1's mask was changed: set again by the next draw
        ao_restore_state();
        renderer_.rebind_uniforms();
    }
    return drawn;
}

// ShadowFoliage, at the end of the transparent pass: the cut-outs added to the shadow map, and the extra
// shadow they cast darkens the picture.
void Device::foliage_shadows() {
    if (foliage_draws_.empty() || !owner_thread() || !back_buffer_ || !depth_stencil_) return;
    if (framebuffer_for(back_buffer_, depth_stencil_) != foliage_in_.scene_fbo) return;
    if (!recorded_intact()) {
        shadows_skipped_ = true;
        foliage_draws_.clear();
        return;
    }
    drop_deleted(foliage_draws_);
    shadow_.run_foliage(renderer_, foliage_in_, foliage_draws_);
    foliage_draws_.clear();
    ao_restore_state();
    renderer_.rebind_uniforms();
}

// End of the transparent pass (render::transparent_scene_done): ShadowFoliage's shadows; phase 2 of the
// occlusion, if any cut-out was drawn.
void Device::ao_cutout_end(const float* projection) {
    const bool window = ao_window_;
    ao_close_window();  // the scene framebuffer's draw buffer 1 off before anything else draws into it
    if (std::exchange(foliage_recording_, false)) foliage_shadows();
    if (!window) return;
    if (ao_cutout_draws_ == 0 || !owner_thread() || !back_buffer_ || !depth_stencil_) return;
    if (framebuffer_for(back_buffer_, depth_stencil_) != ao_in_.scene_fbo) return;
    if (projection) std::memcpy(ao_projection_, projection, sizeof ao_projection_);
    ao_.run_cutout(ao_in_);
    ao_restore_state();
}


HRESULT Device::DrawPrimitive(Enum prim, UINT start_vertex, UINT prim_count) {
    const PerfScope perf(perf_);
    if (prim_count == 0 || !prepare_draw()) return kOk;
    if (grass_recording_ && grass_recording_->ok) {  // [Graphics] GrassBatching: unindexed draws are not recorded
        grass_recording_->ok = false;
        grass_recording_->why = 0;
    }
    if (hook::settings().gl_debug) dump_draw(prim, start_vertex, prim_count, false, last_program_);
    ao_prepare_cutout();
    if (shadow_recording_) shadow_record(primitive_mode(prim), vertex_count(prim, prim_count), static_cast<GLint>(start_vertex), false, false, 0);
    else if (foliage_recording_) foliage_record(primitive_mode(prim), vertex_count(prim, prim_count), static_cast<GLint>(start_vertex), false, false, 0);
    renderer_.draw_arrays(primitive_mode(prim), static_cast<GLint>(start_vertex) + pool_base_, vertex_count(prim, prim_count));
    if (std::exchange(probe_after_draw_, false)) probe_pixels("after the draw");
    return kOk;
}

HRESULT Device::DrawIndexedPrimitive(Enum prim, UINT, UINT, UINT start_index, UINT prim_count) {
    const PerfScope perf(perf_);
    if (prim_count == 0) return kOk;
    if (!indices_) {
        ++stats_.no_indices;
        return kOk;
    }
    if (!prepare_draw()) return kOk;
    if (grass_recording_) grass_record_draw(prim, start_index, prim_count);  // [Graphics] GrassBatching
    if (hook::settings().gl_debug) dump_draw(prim, start_index, prim_count, true, last_program_);
    const bool wide = indices_->format() == kFmtIndex32;
    const std::uintptr_t offset = static_cast<std::uintptr_t>(start_index) * (wide ? 4u : 2u) + static_cast<std::uintptr_t>(indices_->offset());
    ao_prepare_cutout();
    if (shadow_recording_) shadow_record(primitive_mode(prim), vertex_count(prim, prim_count), 0, true, wide, offset);
    else if (foliage_recording_) foliage_record(primitive_mode(prim), vertex_count(prim, prim_count), 0, true, wide, offset);
    renderer_.draw_elements(primitive_mode(prim), vertex_count(prim, prim_count), wide, offset,
                            static_cast<GLint>(base_vertex_) + pool_base_);
    if (std::exchange(probe_after_draw_, false)) probe_pixels("after the draw");
    return kOk;
}

// Diagnostics: a few pixels of the current render target (canvas centre, button column, corners).
void Device::probe_pixels(const char* when) {
    struct P {
        int x, y;
    };
    const P points[] = {{960, 540}, {400, 300}, {100, 100}, {240, 5}};
    // [Graphics] MSAA: glReadPixels cannot read a multisampled framebuffer (only the back buffer is
    // one); read its resolved picture instead, then restore the read binding.
    GLint read_fbo = 0;
    bool resolved = false;
    if (back_buffer_ && back_buffer_->gl_msaa) {
        GLint sample_buffers = 0;
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read_fbo);
        glGetIntegerv(GL_SAMPLE_BUFFERS, &sample_buffers);
        resolved = sample_buffers > 0;
        if (resolved) glBindFramebuffer(GL_READ_FRAMEBUFFER, resolve_multisample(back_buffer_));
    }
    char text[256] = {};
    int len = 0;
    for (const P& p : points) {
        std::uint8_t px[4] = {};
        glReadPixels(p.x, p.y, 1, 1, GL_BGRA, GL_UNSIGNED_BYTE, px);
        len += std::snprintf(text + len, sizeof text - static_cast<std::size_t>(len), " (%d,%d)=%02x%02x%02x%02x", p.x, p.y,
                             px[3], px[2], px[1], px[0]);
    }
    if (resolved) glBindFramebuffer(GL_READ_FRAMEBUFFER, static_cast<GLuint>(read_fbo));
    const GLenum error = glGetError();
    hook::log_line("draw |   pixels %s, fbo %u, glGetError 0x%x:%s", when, current_fbo_, error, text);
}

}  // namespace joan::gld

namespace joan::gld {
namespace {
void ao_hook(const float* projection) {
    if (g_ao_device) g_ao_device->ambient_occlusion(projection);
}
void ao_cutout_hook(const float* projection) {
    if (g_ao_device) g_ao_device->ao_cutout_end(projection);
}
void shadow_begin_hook(const float* sun) {
    if (g_ao_device) g_ao_device->shadow_begin(sun);
}
bool water_hook(const float* projection, const float* sun) { return g_ao_device && g_ao_device->water_pass(projection, sun); }
const render::GrassHooks kGrassHooks{
    [](const render::GrassKey& key, const float* wind, std::uint32_t colour, bool fading) {
        return g_ao_device && g_ao_device->grass_replay(key, wind, colour, fading);
    },
    [](const render::GrassKey& key) { return g_ao_device && g_ao_device->grass_record_begin(key); },
    [](std::uint32_t colour) {
        if (g_ao_device) g_ao_device->grass_record_colour(colour);
    },
    [] {
        if (g_ao_device) g_ao_device->grass_record_end();
    },
    [](const void* plane) {
        if (g_ao_device) g_ao_device->grass_forget(plane);
    },
    [] {
        if (g_ao_device) g_ao_device->grass_flush();
    },
};bool volume_hook(const render::VolumeSource& src, const float* dir, float length, bool upload, bool* both) {
    return g_ao_device && g_ao_device->shadow_volume(src, dir, length, upload, both);
}
void overlay_hook(bool begin) {
    if (g_ao_device) g_ao_device->overlay(begin);
}
void shadow_done_hook(const float* projection) {
    if (g_ao_device) g_ao_device->shadow_done(projection);
}
}  // namespace
}  // namespace joan::gld
