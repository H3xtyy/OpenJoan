// [Graphics] GpuShadowVolumes=1 (Shadows=1): the original stencil shadows' volumes made by the GPU.
//
// The original (ShadowPlane::build 0x005e3040, en3d/plane_sh.cpp) rebuilds the volume of every unit near
// the camera on the CPU each frame: it welds the shadow mesh's equal vertices, keeps the triangles facing
// the light (n . dir < 1e-7, n = (b - a) x (c - b)), collects their silhouette - the edges an odd number of
// lit triangles have (EdgeSet::toggle) - and extrudes it 300 + extent units along the light. Here each mesh
// (its triangles: the units' meshes move every frame, their triangles do not) is turned once into a mesh that
// holds every possible volume: its triangles (the cap) naming their three vertices,
// and on every edge a quad joining the two triangles that share it, each side naming its
// triangle's (an open edge: the other side always moved). Each draw streams the mesh's current vertices
// to the GPU; the vertex shader
// (gld::make_volume_vertex_shader) works out each triangle's normal from them, moves a vertex along the light
// when its triangle faces away and
// collapses unlit cap triangles: a quad whose two triangles disagree becomes the silhouette's side wall,
// wound as the original's (the edge reversed from the lit triangle's), the others stay degenerate.
#pragma once

#include <cstdint>
#include <unordered_map>

#include "../gl/glapi.hpp"

namespace joan::render {

class Renderer;

/// A ShadowPlane's geometry in its model space, as ShadowPlane::build reads it.
struct VolumeSource {
    const std::uint8_t* vertices = nullptr;  ///< positions: the first 3 floats of each vertex; not owned
    std::int32_t stride = 0;  ///< bytes between vertices
    std::int32_t vertex_count = 0;  ///< number of vertices
    const std::uint16_t* triangles = nullptr;   ///< 3 vertex indices per triangle; not owned, its address identifies the mesh
    std::int32_t triangle_count = 0;            ///< number of triangles
};

/// Draws the volume of `src` for the light direction `dir` (model space) extruded by `length` with the
/// device's current state (stencil, cull mode, world transform: set by the caller). False = not drawn.
/// `upload`: the vertices are streamed (false: the previous call's, for its second stencil pass).
/// `both` (the first of the caller's two stencil passes - culling one winding with stencil INCRSAT, then the
/// other with DECRSAT): when set to true, both passes were drawn at once (two-sided stencil, increment / decrement
/// wrapping: the same net count, in any order) and the caller skips its second draw.
bool shadow_volume_draw(const VolumeSource& src, const float* dir, float length, bool upload, bool* both = nullptr);
/// Callback of the device for shadow_volume_draw (same arguments and result).
using VolumeHook = bool (*)(const VolumeSource& src, const float* dir, float length, bool upload, bool* both);
/// Installs the shadow volume callback (done by gld::Device).
void set_volume_hook(VolumeHook hook);  // installed by gld::Device

/// GPU-side shadow volumes for the original stencil shadows: caches one volume mesh per triangle array and
/// draws it through a Renderer. Owned by gl::Device; needs its GL context current.
class ShadowVolumes {
   public:
    /// Binds the volume program and the mesh of `src` (made or remade when its geometry changed) and draws
    /// it. The uniform blocks and raster state must be current. `frame` = the presented frame count
    /// (meshes not drawn for a while are freed).
    bool draw(Renderer& r, const VolumeSource& src, const float* dir, float length, bool upload, std::uint32_t frame);
    /// Frees the cached meshes and the program (context current).
    void shutdown();

   private:
    struct Mesh {
        gl::GLuint vbo = 0, ibo = 0;
        std::int32_t index_count = 0;
        std::int32_t vertex_count = 0, triangle_count = 0;
        std::uint16_t probe[8][3] = {};  // a few triangles: the mesh is rebuilt when they change
        std::uint32_t used = 0;
    };
    bool ensure(Renderer& r);
    void build(Mesh& m, const VolumeSource& src);
    bool unchanged(const Mesh& m, const VolumeSource& src) const;
    void probe(Mesh& m, const VolumeSource& src) const;
    std::unordered_map<const void*, Mesh> meshes_;  // by triangle array
    gl::GLuint program_ = 0, vao_ = 0;
    gl::GLint loc_volume_ = -1, loc_stride_ = -1;
    bool failed_ = false;
    std::uint32_t swept_ = 0;
    unsigned logged_ = 0, builds_ = 0;
};

}  // namespace joan::render
