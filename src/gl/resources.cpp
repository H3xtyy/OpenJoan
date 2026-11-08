// Textures, surfaces, vertex and index buffers of the OpenGL device.
//
// Locking: textures keep a CPU copy of a level only while it is locked (then
// upload it and free it; a later lock reads the level back from the GPU), so
// managed textures do not cost their size twice. Write-only buffers (all of
// the game's) lock into a scratch block that is uploaded to the locked range.
#include "../platform/win32_types.hpp"

#include <algorithm>
#include <cstring>

#include "device.hpp"
#include "glapi.hpp"
#include "../game/stream_cache_api.hpp"

namespace joan::gld {

using namespace joan::gl;

HRESULT PrivateData::get(const Guid& g, void* data, DWORD* size) const {
    const auto it = entries.find(key(g));
    if (it == entries.end()) return kNotFound;
    if (!size) return kInvalidCall;
    const auto n = static_cast<DWORD>(it->second.size());
    if (!data || *size < n) {
        *size = n;
        return data ? static_cast<HRESULT>(0x887600a0) /* D3DERR_MOREDATA */ : kOk;
    }
    std::memcpy(data, it->second.data(), n);
    *size = n;
    return kOk;
}

// ---------------------------------------------------------------------------
// Texture
// ---------------------------------------------------------------------------
Texture::Texture(Device* device, UINT width, UINT height, UINT levels, DWORD usage, Enum format, Enum pool)
    : device_(device), width_(width), height_(height), usage_(usage), format_(format), pool_(pool),
      info_(format_info(format)) {
    UINT full = 1;
    for (UINT d = std::max(width, height); d > 1; d >>= 1) ++full;
    levels_ = (levels == 0 || levels > full) ? full : levels;
    if (render_target()) levels_ = 1;
    if (!info_.supported || width == 0 || height == 0) return;
    glCreateTextures(GL_TEXTURE_2D, 1, &gl_);
    glTextureStorage2D(gl_, static_cast<GLsizei>(levels_), info_.internal, static_cast<GLsizei>(width),
                       static_cast<GLsizei>(height));
    glTextureParameteriv(gl_, GL_TEXTURE_SWIZZLE_RGBA, info_.swizzle);
    glTextureParameteri(gl_, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(levels_ - 1));
    shadow_.resize(levels_);
    surfaces_.resize(levels_, nullptr);
    locked_.resize(levels_, false);
    uploaded_.resize(levels_, false);
    kept_.resize(levels_, false);
    lock_flags_.resize(levels_, 0);
}

Texture::~Texture() {
    for (Surface* s : surfaces_) delete s;
    if (gl_) {
        if (render::Renderer* r = render::current()) r->forget_texture(gl_);  // its cached unit bindings
        glDeleteTextures(1, &gl_);
    }
}

HRESULT Texture::GetDevice(IDirect3DDevice8** out) {
    if (!out) return kInvalidCall;
    device_->AddRef();
    *out = device_;
    return kOk;
}

HRESULT Texture::GetLevelDesc(UINT level, SurfaceDesc* desc) {
    if (level >= levels_ || !desc) return kInvalidCall;
    desc->format = format_;
    desc->type = kRtSurface;
    desc->usage = usage_;
    desc->pool = pool_;
    desc->width = level_dim(width_, level);
    desc->height = level_dim(height_, level);
    desc->size = level_size(info_, desc->width, desc->height);
    desc->multi_sample = 0;
    return kOk;
}

Surface* Texture::level_surface(UINT level) {
    if (level >= levels_) return nullptr;
    if (!surfaces_[level]) {
        auto* s = new Surface(device_, Surface::Kind::TextureLevel, format_, level_dim(width_, level), level_dim(height_, level));
        s->texture = this;
        s->level = level;
        s->gl_texture = gl_;
        surfaces_[level] = s;
    }
    return surfaces_[level];
}

HRESULT Texture::GetSurfaceLevel(UINT level, IDirect3DSurface8** out) {
    Surface* s = level_surface(level);
    if (!s || !out) return kInvalidCall;
    AddRef();  // a texture level surface shares its texture's reference count
    *out = s;
    return kOk;
}

std::vector<std::uint8_t>& Texture::shadow(UINT level, bool read_back) {
    auto& sh = shadow_[level];
    if (sh.empty()) {
        const UINT w = level_dim(width_, level), h = level_dim(height_, level);
        sh.resize(level_size(info_, w, h));
        if (uploaded_[level]) kept_[level] = true;  // written again after its upload: keep the copy from now on
        if (uploaded_[level] && read_back) {  // read the current contents back
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            if (info_.compressed)
                glGetCompressedTextureSubImage(gl_, static_cast<GLint>(level), 0, 0, 0, static_cast<GLsizei>(w),
                                               static_cast<GLsizei>(h), 1, static_cast<GLsizei>(sh.size()), sh.data());
            else
                glGetTextureSubImage(gl_, static_cast<GLint>(level), 0, 0, 0, static_cast<GLsizei>(w),
                                     static_cast<GLsizei>(h), 1, info_.format, info_.type,
                                     static_cast<GLsizei>(sh.size()), sh.data());
        }
    }
    return sh;
}

HRESULT Texture::LockRect(UINT level, LockedRect* locked, const Rect* rect, DWORD flags) {
    if (level >= levels_ || !locked || !gl_ || locked_[level] || render_target()) return kInvalidCall;
    auto& sh = shadow(level);
    const UINT pitch = row_pitch(info_, level_dim(width_, level));
    std::size_t offset = 0;
    if (rect) {
        const UINT x = static_cast<UINT>(rect->left), y = static_cast<UINT>(rect->top);
        offset = info_.compressed ? (y / 4) * pitch + (x / 4) * info_.bytes : y * pitch + x * info_.bytes;
    }
    locked->pitch = static_cast<int>(pitch);
    locked->bits = sh.data() + offset;
    locked_[level] = true;
    lock_flags_[level] = flags;
    return kOk;
}

HRESULT Texture::UnlockRect(UINT level) {
    if (level >= levels_ || !locked_[level]) return kInvalidCall;
    locked_[level] = false;
    if (!(lock_flags_[level] & kLockReadOnly)) upload_level(level);
    release_shadow(level);
    return kOk;
}

void Texture::release_shadow(UINT level) {
    if (kept_[level]) return;
    auto& sh = shadow_[level];
    sh.clear();
    sh.shrink_to_fit();
}

bool Texture::kept_contents(UINT level, std::vector<std::uint8_t>& out) const {
    if (level >= levels_ || !kept_[level] || shadow_[level].empty()) return false;
    out = shadow_[level];
    return true;
}

void Texture::upload_level(UINT level) {
    const auto& sh = shadow_[level];
    if (sh.empty()) return;
    const auto w = static_cast<GLsizei>(level_dim(width_, level)), h = static_cast<GLsizei>(level_dim(height_, level));
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (info_.compressed)
        glCompressedTextureSubImage2D(gl_, static_cast<GLint>(level), 0, 0, w, h, info_.internal,
                                      static_cast<GLsizei>(sh.size()), sh.data());
    else
        glTextureSubImage2D(gl_, static_cast<GLint>(level), 0, 0, w, h, info_.format, info_.type, sh.data());
    uploaded_[level] = true;
}

// ---------------------------------------------------------------------------
// Surface
// ---------------------------------------------------------------------------
Surface::Surface(Device* device, Kind k, Enum f, UINT w, UINT h)
    : kind(k), format(f), width(w), height(h), device_(device) {
    if (kind == Kind::BackBuffer) {
        glCreateTextures(GL_TEXTURE_2D, 1, &gl_texture);
        glTextureStorage2D(gl_texture, 1, GL_RGBA8, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
        glTextureParameteri(gl_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(gl_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else if (kind == Kind::DepthStencil) {
        glCreateRenderbuffers(1, &gl_renderbuffer);
        glNamedRenderbufferStorage(gl_renderbuffer, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
    } else if (kind == Kind::Image) {
        const FormatInfo info = format_info(f);
        cpu.resize(level_size(info, w, h));
    }
}

Surface::~Surface() {
    device_->forget_surface(this);
    if (kind == Kind::BackBuffer && gl_texture) {
        if (render::Renderer* r = render::current()) r->forget_texture(gl_texture);
        glDeleteTextures(1, &gl_texture);
    }
    if (gl_renderbuffer) glDeleteRenderbuffers(1, &gl_renderbuffer);
    if (gl_msaa) glDeleteRenderbuffers(1, &gl_msaa);  // [Graphics] MSAA storage
}

ULONG Surface::AddRef() { return texture ? texture->AddRef() : ComObject::AddRef(); }
ULONG Surface::Release() { return texture ? texture->Release() : ComObject::Release(); }

HRESULT Surface::GetDevice(IDirect3DDevice8** out) {
    if (!out) return kInvalidCall;
    device_->AddRef();
    *out = device_;
    return kOk;
}

HRESULT Surface::GetContainer(const Guid& riid, void** out) {
    if (!out) return kInvalidCall;
    if (texture) return texture->QueryInterface(riid, out);
    return device_->QueryInterface(riid, out);
}

HRESULT Surface::GetDesc(SurfaceDesc* desc) {
    if (!desc) return kInvalidCall;
    desc->format = format;
    desc->type = kRtSurface;
    desc->usage = kind == Kind::BackBuffer ? kUsageRenderTarget : kind == Kind::DepthStencil ? kUsageDepthStencil : static_cast<Usage>(0);
    desc->pool = kind == Kind::Image ? kPoolSystemMem : kPoolDefault;
    desc->width = width;
    desc->height = height;
    desc->size = level_size(format_info(format), width, height);
    desc->multi_sample = 0;
    if (texture) {
        SurfaceDesc t = {};
        texture->GetLevelDesc(level, &t);
        desc->usage = t.usage;
        desc->pool = t.pool;
    }
    return kOk;
}

HRESULT Surface::LockRect(LockedRect* locked, const Rect* rect, DWORD flags) {
    if (texture) return texture->LockRect(level, locked, rect, flags);
    if (!locked || locked_ || kind == Kind::DepthStencil) return kInvalidCall;
    if (kind == Kind::BackBuffer && !read_pixels(cpu)) return kInvalidCall;
    const FormatInfo info = format_info(format);
    const UINT pitch = row_pitch(info, width);
    std::size_t offset = 0;
    if (rect) offset = static_cast<std::size_t>(rect->top) * pitch + static_cast<std::size_t>(rect->left) * info.bytes;
    locked->pitch = static_cast<int>(pitch);
    locked->bits = cpu.data() + offset;
    locked_ = true;
    lock_flags_ = flags;
    return kOk;
}

HRESULT Surface::UnlockRect() {
    if (texture) return texture->UnlockRect(level);
    if (!locked_) return kInvalidCall;
    locked_ = false;
    if (kind == Kind::BackBuffer) {
        if (!(lock_flags_ & kLockReadOnly)) write_pixels(cpu.data());
        cpu.clear();
        cpu.shrink_to_fit();
    }
    return kOk;
}

bool Surface::read_pixels(std::vector<std::uint8_t>& out) {
    const FormatInfo info = format_info(format);
    switch (kind) {
        case Kind::Image: out = cpu; return true;
        case Kind::TextureLevel: {
            if (!info.supported) return false;
            if (texture && texture->kept_contents(level, out)) return true;  // no GPU read-back
            out.resize(level_size(info, width, height));
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            if (info.compressed)
                glGetCompressedTextureSubImage(gl_texture, static_cast<GLint>(level), 0, 0, 0, static_cast<GLsizei>(width),
                                               static_cast<GLsizei>(height), 1, static_cast<GLsizei>(out.size()), out.data());
            else
                glGetTextureSubImage(gl_texture, static_cast<GLint>(level), 0, 0, 0, static_cast<GLsizei>(width),
                                     static_cast<GLsizei>(height), 1, info.format, info.type,
                                     static_cast<GLsizei>(out.size()), out.data());
            return true;
        }
        case Kind::BackBuffer:
            if (gl_msaa) device_->resolve_multisample(this);  // MSAA: gl_texture <- the samples
            out.resize(static_cast<std::size_t>(width) * height * 4);
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glGetTextureSubImage(gl_texture, 0, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 1,
                                 GL_BGRA, GL_UNSIGNED_BYTE, static_cast<GLsizei>(out.size()), out.data());
            return true;
        default: return false;
    }
}

bool Surface::write_pixels(const std::uint8_t* pixels) {
    const FormatInfo info = format_info(format);
    switch (kind) {
        case Kind::Image: std::memcpy(cpu.data(), pixels, cpu.size()); return true;
        case Kind::TextureLevel: {
            auto& sh = texture->shadow(level, false);  // all of it is overwritten: no read-back
            std::memcpy(sh.data(), pixels, sh.size());
            texture->upload_level(level);
            texture->release_shadow(level);
            return true;
        }
        case Kind::BackBuffer:
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTextureSubImage2D(gl_texture, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), GL_BGRA,
                                GL_UNSIGNED_BYTE, pixels);
            if (gl_msaa) device_->upload_multisample(this);  // MSAA: the samples <- gl_texture
            return true;
        default: (void)info; return false;
    }
}

// ---------------------------------------------------------------------------
// Buffers
// ---------------------------------------------------------------------------
template <class I, class D>
Buffer<I, D>::Buffer(Device* device, UINT length, DWORD usage, Enum format, DWORD fvf, Enum pool, Enum type)
    : device_(device), length_(length), usage_(usage), fvf_(fvf), format_(format), pool_(pool), type_(type) {
    if (!(usage & kUsageWriteOnly)) shadow_.resize(length);
    if ((usage & kUsageDynamic) && (usage & kUsageWriteOnly) && length && device->stream_ring().ptr) {
        streamed_ = true;  // written into the device's stream ring (StreamRing)
        gl_ = device->stream_ring().buffer;
        return;
    }
    if ((usage & kUsageDynamic) && length && make_copy(0)) {  // dynamic: renamed from the start
        gl_ = copies_[0].gl;
        return;
    }
    // A stream cache buffer (it names the alignment while it creates one): a range of the geometry pool.
    if (const std::uint32_t align = stream_cache_api::g_pool_align; align && !(usage & kUsageDynamic) && length) {
        GeometryPool& geometry = device->geometry_pool();
        const std::uint32_t at = geometry.alloc(length, align, device->presented());
        if (at != ~0u) {
            pooled_ = true;
            gl_ = geometry.buffer;
            base_ = static_cast<GLintptr>(at);
            return;
        }
    }
    glCreateBuffers(1, &gl_);
    glNamedBufferData(gl_, static_cast<GLsizeiptr>(length), nullptr,
                      (usage & kUsageDynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

bool StreamRing::init() {
    const GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    glCreateBuffers(1, &buffer);
    glNamedBufferStorage(buffer, kSize, nullptr, flags);
    ptr = static_cast<std::uint8_t*>(glMapNamedBufferRange(buffer, 0, kSize, flags));
    if (!ptr) {
        glDeleteBuffers(1, &buffer);
        buffer = 0;
        return false;
    }
    cursor = 0;
    return true;
}

void StreamRing::shutdown() {
    for (GLsync& f : fences)
        if (f) glDeleteSync(std::exchange(f, nullptr));
    if (buffer) glDeleteBuffers(1, &buffer);
    buffer = 0;
    ptr = nullptr;
}

std::uint32_t StreamRing::alloc(std::uint32_t size, std::uint32_t min_pos) {
    constexpr std::uint32_t kSegment = kSize / kSegments;
    if (size > kSegment || min_pos >= kSegment) return ~0u;
    std::uint32_t at = std::max((cursor + 15u) & ~15u, min_pos);
    const std::uint32_t segment = cursor ? (cursor - 1) / kSegment : 0;  // the segment of the last write
    if (at + size > (segment + 1) * kSegment) {
        // Leaving this segment: fence it; the next one must be done with by the GPU before it is reused.
        if (fences[segment]) glDeleteSync(fences[segment]);
        fences[segment] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        ++switches;
        const std::uint32_t next = (segment + 1) % kSegments;
        if (GLsync f = std::exchange(fences[next], nullptr)) {
            GLenum r = glClientWaitSync(f, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
            if (r == GL_TIMEOUT_EXPIRED) {
                ++buffer_stats().stream_waits;
                do r = glClientWaitSync(f, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000ull);
                while (r == GL_TIMEOUT_EXPIRED);
            }
            glDeleteSync(f);
        }
        at = std::max(next * kSegment, min_pos);
    }
    cursor = at + size;
    return at;
}

bool GeometryPool::init() {
    for (int i = 0; i < 8 && glGetError() != 0; ++i) {}  // earlier errors are not this one's
    glCreateBuffers(1, &buffer);
    glNamedBufferStorage(buffer, kSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    if (glGetError() != 0) {
        glDeleteBuffers(1, &buffer);
        buffer = 0;
        return false;
    }
    free.assign(1, Range{0, kSize});
    retired.clear();
    used = 0;
    return true;
}

void GeometryPool::shutdown() {
    if (buffer) glDeleteBuffers(1, &buffer);
    buffer = 0;
    free.clear();
    retired.clear();
}

void GeometryPool::give_back(Range r) {
    auto it = std::lower_bound(free.begin(), free.end(), r.at, [](const Range& a, std::uint32_t at) { return a.at < at; });
    it = free.insert(it, r);
    if (it + 1 != free.end() && it->at + it->size == (it + 1)->at) {  // merge with the next
        it->size += (it + 1)->size;
        free.erase(it + 1);
    }
    if (it != free.begin() && (it - 1)->at + (it - 1)->size == it->at) {  // and the previous
        (it - 1)->size += it->size;
        free.erase(it);
    }
}

std::uint32_t GeometryPool::alloc(std::uint32_t size, std::uint32_t align, std::uint32_t frame) {
    if (!buffer || size == 0) return ~0u;
    if (align == 0) align = 4;
    // Ranges freed long enough ago (the draws that read them are done) go back first.
    for (std::size_t i = 0; i < retired.size();) {
        if (frame - retired[i].frame >= kRetireFrames) {
            give_back(retired[i].range);
            retired[i] = retired.back();
            retired.pop_back();
        } else {
            ++i;
        }
    }
    for (std::size_t i = 0; i < free.size(); ++i) {
        const Range r = free[i];
        const std::uint32_t at = (r.at + align - 1) / align * align;
        if (at < r.at || at - r.at + std::uint64_t{size} > r.size) continue;
        const Range before{r.at, at - r.at}, after{at + size, r.at + r.size - (at + size)};
        free.erase(free.begin() + static_cast<std::ptrdiff_t>(i));
        if (after.size) give_back(after);
        if (before.size) give_back(before);
        used += size;
        return at;
    }
    if (!full_logged) {
        full_logged = true;
        hook::log_line("[Graphics] geometry pool full (%u MB in use) - further cached models get their own buffers", used >> 20);
    }
    return ~0u;
}

void GeometryPool::release(std::uint32_t at, std::uint32_t size, std::uint32_t frame) {
    if (!buffer) return;
    used -= size;
    retired.push_back({{at, size}, frame});
}

namespace {
// All copies of renamed buffers together (32-bit address space: past this, buffers are updated in place).
std::size_t g_pooled_bytes = 0;
constexpr std::size_t kPooledLimit = 192u << 20;
}  // namespace

BufferStats& buffer_stats() {
    static BufferStats s;
    return s;
}

template <class I, class D>
bool Buffer<I, D>::make_copy(std::size_t at) {
    if (copies_.size() >= kMaxCopies || g_pooled_bytes + length_ > kPooledLimit) return false;
    const GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    Copy c{0, nullptr, nullptr};
    glCreateBuffers(1, &c.gl);
    glNamedBufferStorage(c.gl, static_cast<GLsizeiptr>(length_), nullptr, flags);
    c.ptr = static_cast<std::uint8_t*>(glMapNamedBufferRange(c.gl, 0, static_cast<GLsizeiptr>(length_), flags));
    if (!c.ptr) {
        glDeleteBuffers(1, &c.gl);
        return false;
    }
    g_pooled_bytes += length_;
    ++buffer_stats().copies_made;
    copies_.insert(copies_.begin() + static_cast<std::ptrdiff_t>(std::min(at, copies_.size())), c);
    return true;
}

// The current copy is left (fenced); the next one in turn - the longest left - is reused once the GPU is
// done with it, else a new copy goes in before it (the oldest stays next), else it is waited for.
template <class I, class D>
void Buffer<I, D>::rename() {
    ++buffer_stats().renames;
    Copy& cur = copies_[current_];
    if (cur.fence) glDeleteSync(cur.fence);
    cur.fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    std::size_t next = (current_ + 1) % copies_.size();
    auto is_free = [&](std::size_t i) {
        if (i == current_) return false;
        const GLsync f = copies_[i].fence;
        if (!f) return true;
        const GLenum r = glClientWaitSync(f, 0, 0);
        return r == GL_ALREADY_SIGNALED || r == GL_CONDITION_SATISFIED;
    };
    if (!is_free(next)) {
        if (make_copy(current_ + 1)) {
            next = current_ + 1;
        } else if (next != current_) {
            ++buffer_stats().waits;
            for (GLenum r = GL_TIMEOUT_EXPIRED; r == GL_TIMEOUT_EXPIRED;)
                r = glClientWaitSync(copies_[next].fence, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000ull);
        } else {
            wait_current();  // a single copy and no room for another
            return;
        }
    }
    if (copies_[next].fence) glDeleteSync(std::exchange(copies_[next].fence, nullptr));
    current_ = next;
    gl_ = copies_[current_].gl;
}

template <class I, class D>
void Buffer<I, D>::wait_current() {
    ++buffer_stats().waits;
    Copy& cur = copies_[current_];
    if (cur.fence) glDeleteSync(cur.fence);
    cur.fence = nullptr;
    GLsync f = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    for (GLenum r = GL_TIMEOUT_EXPIRED; r == GL_TIMEOUT_EXPIRED;) r = glClientWaitSync(f, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000ull);
    glDeleteSync(f);
}

template <class I, class D>
Buffer<I, D>::~Buffer() {
    if (streamed_) return;  // the stream ring is the device's
    if (pooled_) {  // the range goes back to the pool once the draws that may read it are done
        device_->geometry_pool().release(static_cast<std::uint32_t>(base_), length_, device_->presented());
        return;
    }
    render::Renderer* const r = render::current();
    if (!copies_.empty()) {
        for (Copy& c : copies_) {
            if (r) r->forget_buffer(c.gl);
            if (c.fence) glDeleteSync(c.fence);
            glDeleteBuffers(1, &c.gl);  // unmaps it
            g_pooled_bytes -= length_;
        }
        return;
    }
    if (gl_) {
        if (r) r->forget_buffer(gl_);  // its cached vertex array bindings
        glDeleteBuffers(1, &gl_);
    }
}

template <class I, class D>
HRESULT Buffer<I, D>::GetDevice(IDirect3DDevice8** out) {
    if (!out) return kInvalidCall;
    device_->AddRef();
    *out = device_;
    return kOk;
}

template <class I, class D>
HRESULT Buffer<I, D>::Lock(UINT offset, UINT size, BYTE** data, DWORD flags) {
    if (!data || offset > length_) return kInvalidCall;
    if (size == 0 || offset + size > length_) size = length_ - offset;
    if (lock_depth_ > 0) Unlock();  // nested locks are not used by the game; flush the first
    lock_offset_ = offset;
    lock_size_ = size;
    lock_flags_ = flags;
    if (streamed_ && !(flags & kLockReadOnly)) {
        // A fresh slice of the stream ring; the game writes straight into it (persistently mapped, coherent).
        StreamRing& ring = device_->stream_ring();
        const std::uint32_t at = ring.alloc(size, offset);
        if (at != ~0u) {
            ++buffer_stats().streamed;
            base_ = static_cast<GLintptr>(at) - static_cast<GLintptr>(offset);
            *data = ring.ptr + at;
            stream_lock_ = true;
            ++lock_depth_;
            return kOk;
        }
    }
    if (usage_ & kUsageWriteOnly) {
        scratch_.resize(size);
        *data = scratch_.data();
    } else {
        *data = shadow_.data() + offset;
    }
    ++lock_depth_;
    return kOk;
}

template <class I, class D>
HRESULT Buffer<I, D>::Unlock() {
    if (lock_depth_ == 0) return kInvalidCall;
    lock_depth_ = 0;
    if (std::exchange(stream_lock_, false)) return kOk;  // written in place (coherent mapping)
    if (streamed_) return kOk;  // a read-only lock of a streamed buffer: nothing to write back
    if (lock_size_ && !(lock_flags_ & kLockReadOnly)) {
        const std::uint8_t* src = (usage_ & kUsageWriteOnly) ? scratch_.data() : shadow_.data() + lock_offset_;
        if (pooled_) {
            // Written once when the stream cache makes it (nothing has drawn from the range yet). Written again:
            // queued draws may still read it - wait for the GPU first, as a plain Direct3D lock would.
            if (writes_++ > 0) {
                ++buffer_stats().waits;
                GLsync f = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
                for (GLenum r = GL_TIMEOUT_EXPIRED; r == GL_TIMEOUT_EXPIRED;) r = glClientWaitSync(f, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000ull);
                glDeleteSync(f);
            }
            glNamedBufferSubData(gl_, base_ + static_cast<GLintptr>(lock_offset_), static_cast<GLsizeiptr>(lock_size_), src);
            if ((usage_ & kUsageWriteOnly) && !(usage_ & kUsageDynamic)) {
                scratch_.clear();
                scratch_.shrink_to_fit();
            }
            return kOk;
        }
        const bool from_start = lock_offset_ == 0;
        // A static buffer written again (a mesh re-uploaded when it changed) becomes renamed too, if it is
        // rewritten from the start or its whole contents are known (shadow); the old buffer object lives on
        // in GL until queued draws are done with it.
        bool fresh = false;  // a copy nothing has drawn from yet: written as is
        if (copies_.empty() && writes_ > 0 && (from_start || !shadow_.empty())) {
            const GLuint old = gl_;
            if (make_copy(0)) {
                ++buffer_stats().promoted;
                if (render::Renderer* r = render::current()) r->forget_buffer(old);
                glDeleteBuffers(1, &old);
                gl_ = copies_[0].gl;
                fresh = true;
                if (!shadow_.empty()) std::memcpy(copies_[0].ptr, shadow_.data(), length_);
                else std::memcpy(copies_[0].ptr + lock_offset_, src, lock_size_);
            }
        }
        ++writes_;
        if (fresh) {
            // done
        } else if (!copies_.empty()) {
            // DISCARD, or a rewrite from the start (the rest of a write-only buffer is the game's to fill): a
            // free copy. NOOVERWRITE (append; the game does not touch what queued draws use): in place. Any
            // other lock waits for the GPU, as Direct3D does.
            bool renamed = false;
            if ((lock_flags_ & kLockDiscard) || (!(lock_flags_ & kLockNoOverwrite) && from_start)) {
                rename();
                renamed = true;
            } else if (!(lock_flags_ & kLockNoOverwrite)) {
                wait_current();
            } else {
                ++buffer_stats().appends;
            }
            if (renamed && !shadow_.empty() && !(lock_flags_ & kLockDiscard))
                std::memcpy(copies_[current_].ptr, shadow_.data(), length_);  // the whole known contents
            else
                std::memcpy(copies_[current_].ptr + lock_offset_, src, lock_size_);
        } else {
            ++buffer_stats().in_place;
            glNamedBufferSubData(gl_, static_cast<GLintptr>(lock_offset_), static_cast<GLsizeiptr>(lock_size_), src);
        }
    }
    if ((usage_ & kUsageWriteOnly) && !(usage_ & kUsageDynamic)) {
        scratch_.clear();
        scratch_.shrink_to_fit();
    }
    return kOk;
}

template <class I, class D>
HRESULT Buffer<I, D>::GetDesc(D* desc) {
    if (!desc) return kInvalidCall;
    desc->format = format_;
    desc->type = type_;
    desc->usage = usage_;
    desc->pool = pool_;
    desc->size = length_;
    if constexpr (std::is_same_v<D, VertexBufferDesc>) desc->fvf = fvf_;
    return kOk;
}

template class Buffer<IDirect3DVertexBuffer8, VertexBufferDesc>;
template class Buffer<IDirect3DIndexBuffer8, IndexBufferDesc>;

}  // namespace joan::gld
