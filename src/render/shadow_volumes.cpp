// [Graphics] GpuShadowVolumes - see shadow_volumes.hpp.
#include "shadow_volumes.hpp"

#include <algorithm>
#include <cstring>
#include <vector>

#include "../gl/shaders.hpp"
#include "../registry.hpp"
#include "renderer.hpp"

namespace joan::render {

using namespace gl;

namespace {

VolumeHook g_hook = nullptr;

struct VolumeVertex {
    float ref[4];  // this vertex, its triangle's three (welded indices into the mesh's vertices)
    float kind;    // 0 edge, 1 open edge (always moved), 2 cap
};
static_assert(sizeof(VolumeVertex) == 20, "vertex layout");

const float* position(const VolumeSource& s, std::int32_t i) {
    return reinterpret_cast<const float*>(s.vertices + static_cast<std::size_t>(i) * static_cast<std::size_t>(s.stride));
}

constexpr std::uint32_t kKeepFrames = 600;  // a mesh not drawn for this many frames is freed

}  // namespace

bool shadow_volume_draw(const VolumeSource& src, const float* dir, float length, bool upload, bool* both) {
    if (both) *both = false;
    return g_hook ? g_hook(src, dir, length, upload, both) : false;
}
void set_volume_hook(VolumeHook hook) { g_hook = hook; }

bool ShadowVolumes::ensure(Renderer& r) {
    if (failed_) return false;
    if (program_) return true;
    program_ = r.compile_program(gld::make_volume_vertex_shader(), gld::make_volume_fragment_shader(), "shadow volumes");
    if (!program_) {
        hook::log_line("[Graphics] GpuShadowVolumes: shader compilation failed - volumes built by the CPU");
        failed_ = true;
        return false;
    }
    loc_volume_ = glGetUniformLocation(program_, "u_volume");
    loc_stride_ = glGetUniformLocation(program_, "u_stride");
    glCreateVertexArrays(1, &vao_);
    glEnableVertexArrayAttrib(vao_, 0);
    glVertexArrayAttribFormat(vao_, 0, 4, GL_FLOAT, GL_FALSE_, 0);
    glVertexArrayAttribBinding(vao_, 0, 0);
    glEnableVertexArrayAttrib(vao_, 1);
    glVertexArrayAttribFormat(vao_, 1, 1, GL_FLOAT, GL_FALSE_, 16);
    glVertexArrayAttribBinding(vao_, 1, 0);
    return true;
}

void ShadowVolumes::probe(Mesh& m, const VolumeSource& src) const {
    for (int k = 0; k < 8; ++k) std::memcpy(m.probe[k], src.triangles + (src.triangle_count - 1) * k / 7 * 3, 6);
}

bool ShadowVolumes::unchanged(const Mesh& m, const VolumeSource& src) const {
    if (m.vertex_count != src.vertex_count || m.triangle_count != src.triangle_count) return false;
    for (int k = 0; k < 8; ++k)
        if (std::memcmp(m.probe[k], src.triangles + (src.triangle_count - 1) * k / 7 * 3, 6) != 0) return false;
    return true;
}

void ShadowVolumes::build(Mesh& m, const VolumeSource& src) {
    const std::int32_t n = src.vertex_count;
    // The weld (ShadowPlane +0x108, made once like the original's): each vertex -> the first earlier vertex at
    // the same position.
    std::vector<std::int32_t> weld(static_cast<std::size_t>(n));
    {
        struct Key {
            float x, y, z;
            bool operator==(const Key& o) const { return x == o.x && y == o.y && z == o.z; }
        };
        struct Hash {
            std::size_t operator()(const Key& k) const {
                auto bits = [](float f) {
                    if (f == 0.0f) f = 0.0f;  // -0 == +0
                    std::uint32_t u;
                    std::memcpy(&u, &f, 4);
                    return static_cast<std::size_t>(u);
                };
                return bits(k.x) * 73856093u ^ bits(k.y) * 19349663u ^ bits(k.z) * 83492791u;
            }
        };
        std::unordered_map<Key, std::int32_t, Hash> first;
        first.reserve(static_cast<std::size_t>(n));
        for (std::int32_t i = 0; i < n; ++i) {
            const float* p = position(src, i);
            weld[static_cast<std::size_t>(i)] = first.try_emplace(Key{p[0], p[1], p[2]}, i).first->second;
        }
    }

    std::vector<VolumeVertex> vertices;
    std::vector<std::uint32_t> indices;
    struct Half {
        std::int32_t a, b;
        std::int32_t tri[3];
        bool used;
    };
    std::vector<Half> halves;
    auto vertex = [&](std::int32_t v, const std::int32_t* tri, float kind) {
        const std::int32_t t0 = tri ? tri[0] : v, t1 = tri ? tri[1] : v, t2 = tri ? tri[2] : v;
        vertices.push_back({{static_cast<float>(v), static_cast<float>(t0), static_cast<float>(t1), static_cast<float>(t2)}, kind});
        return static_cast<std::uint32_t>(vertices.size() - 1);
    };
    for (std::int32_t t = 0; t < src.triangle_count; ++t) {
        const std::int32_t a = weld[src.triangles[t * 3]], b = weld[src.triangles[t * 3 + 1]], c = weld[src.triangles[t * 3 + 2]];
        if (a == b || a == c || b == c) continue;
        const std::int32_t tri[3] = {a, b, c};
        for (std::int32_t v : tri) indices.push_back(vertex(v, tri, 2.0f));
        halves.push_back({a, b, {a, b, c}, false});
        halves.push_back({b, c, {a, b, c}, false});
        halves.push_back({c, a, {a, b, c}, false});
    }
    // Each edge: a quad joining the triangle of a half-edge a -> b with the one of its opposite b -> a.
    std::unordered_map<std::uint64_t, std::vector<std::size_t>> by_edge;
    by_edge.reserve(halves.size());
    auto edge_key = [](const Half& h) {
        return static_cast<std::uint64_t>(std::min(h.a, h.b)) << 32 | static_cast<std::uint64_t>(std::max(h.a, h.b));
    };
    for (std::size_t h = 0; h < halves.size(); ++h) by_edge[edge_key(halves[h])].push_back(h);
    for (std::size_t h = 0; h < halves.size(); ++h) {
        Half& t = halves[h];
        if (t.used) continue;
        t.used = true;
        const Half* u = nullptr;
        for (std::size_t g : by_edge[edge_key(t)]) {
            Half& o = halves[g];
            if (o.used || o.a != t.b || o.b != t.a) continue;
            o.used = true;
            u = &o;
            break;
        }
        // Lit t: the wall (b, a, b'), (a, a', b') - the edge reversed from t, as EdgeSet::toggle keeps it.
        const std::uint32_t ta = vertex(t.a, t.tri, 0.0f), tb = vertex(t.b, t.tri, 0.0f);
        const std::uint32_t ub = vertex(t.b, u ? u->tri : nullptr, u ? 0.0f : 1.0f);
        const std::uint32_t ua = vertex(t.a, u ? u->tri : nullptr, u ? 0.0f : 1.0f);
        indices.insert(indices.end(), {tb, ta, ub, ta, ua, ub});
    }

    if (m.vbo) glDeleteBuffers(1, &m.vbo);
    if (m.ibo) glDeleteBuffers(1, &m.ibo);
    m.vbo = m.ibo = 0;
    m.index_count = static_cast<std::int32_t>(indices.size());
    if (!indices.empty()) {
        glCreateBuffers(1, &m.vbo);
        glNamedBufferStorage(m.vbo, static_cast<GLsizeiptr>(vertices.size() * sizeof(VolumeVertex)), vertices.data(), 0);
        glCreateBuffers(1, &m.ibo);
        glNamedBufferStorage(m.ibo, static_cast<GLsizeiptr>(indices.size() * sizeof(std::uint32_t)), indices.data(), 0);
    }
    ++builds_;
    m.vertex_count = src.vertex_count;
    m.triangle_count = src.triangle_count;
    probe(m, src);
    if (logged_ < 3) {
        ++logged_;
        hook::log_line("[Graphics] GpuShadowVolumes: mesh of %d vertices / %d triangles -> %zu volume vertices, %d indices",
                       src.vertex_count, src.triangle_count, vertices.size(), m.index_count);
    }
}

bool ShadowVolumes::draw(Renderer& r, const VolumeSource& src, const float* dir, float length, bool upload, std::uint32_t frame) {
    if (!src.vertices || !src.triangles || src.vertex_count < 3 || src.triangle_count < 1 || src.stride < 12 || (src.stride & 3) != 0)
        return false;
    if (!ensure(r)) return false;
    // Meshes not drawn for a while go (a level's planes are freed with it; a new one may reuse the address).
    if (frame - swept_ >= kKeepFrames) {
        if (builds_ > 0)
            hook::log_line("[Graphics] GpuShadowVolumes: %zu meshes cached, %u built in the last %u frames", meshes_.size(), builds_,
                           frame - swept_);
        builds_ = 0;
        swept_ = frame;
        for (auto it = meshes_.begin(); it != meshes_.end();) {
            if (frame - it->second.used >= kKeepFrames) {
                r.forget_buffer(it->second.vbo);
                r.forget_buffer(it->second.ibo);
                if (it->second.vbo) glDeleteBuffers(1, &it->second.vbo);
                if (it->second.ibo) glDeleteBuffers(1, &it->second.ibo);
                it = meshes_.erase(it);
            } else {
                ++it;
            }
        }
    }
    auto [it, added] = meshes_.try_emplace(src.triangles);
    Mesh& m = it->second;
    if (added || !unchanged(m, src)) {
        if (!added) {
            r.forget_buffer(m.vbo);
            r.forget_buffer(m.ibo);
        }
        build(m, src);
    }
    m.used = frame;
    if (m.index_count == 0) return false;
    // This frame's vertices (the units' meshes are animated on the CPU), read by the shader as storage buffer 0.
    if (upload && !r.stream_storage(0, src.vertices, static_cast<std::uint32_t>(src.vertex_count) * static_cast<std::uint32_t>(src.stride)))
        return false;
    r.use_program(program_);
    const float volume[4] = {dir[0], dir[1], dir[2], length};
    glProgramUniform4fv(program_, loc_volume_, 1, volume);
    glProgramUniform1i(program_, loc_stride_, src.stride / 4);
    r.bind_geometry(vao_, m.vbo, sizeof(VolumeVertex), m.ibo, 0);
    r.draw_elements(GL_TRIANGLES, m.index_count, true, 0, 0);
    return true;
}

void ShadowVolumes::shutdown() {
    for (auto& [k, m] : meshes_) {
        if (m.vbo) glDeleteBuffers(1, &m.vbo);
        if (m.ibo) glDeleteBuffers(1, &m.ibo);
    }
    meshes_.clear();
    if (program_) glDeleteProgram(program_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    program_ = vao_ = 0;
}

}  // namespace joan::render
