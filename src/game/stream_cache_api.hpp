// The stream cache of SysEn::fill_vertices / fill_indices (game/unknown_d3dsys.cpp, namespace stream_cache):
// it only serves fills made inside a ModelFill scope - Plane::draw's copy of a model's own vertex array and
// triangle list (en3d/plane.cpp). Every other fill (2D screens, text, effects: temporary arrays rebuilt at the
// same address with new contents) is copied into the ring as the original does.
#pragma once

namespace joan::stream_cache_api {

extern bool g_model_fill;
/// Non-zero while the stream cache creates one of its buffers: the device places it in its geometry pool
/// (gl/device.hpp GeometryPool), starting at a multiple of this (a vertex buffer's stride).
extern unsigned g_pool_align;
/// Frames presented (gld::Device::Present advances it): a cached array verified in this frame is not checked
/// again when it is drawn again in the same frame.
extern unsigned g_frame;

struct ModelFill {
    bool previous = g_model_fill;
    ModelFill() { g_model_fill = true; }
    ~ModelFill() { g_model_fill = previous; }
    ModelFill(const ModelFill&) = delete;
    ModelFill& operator=(const ModelFill&) = delete;
};

/// A plane's index list (triangles / strip indices) at `data` was written in place: its next fill is checked
/// in full. Index lists are otherwise checked by a sample of their words, as vertex arrays are; the Plane
/// functions that edit them (set_triangle, sort_by_material, reserve_strip, copy_extras, invalidate) report here.
void indices_written(const void* data);
/// The same for a plane's vertex array at `data`: Sprite2D::queue (game/unknown_sprite2d.cpp) reports the 2D layer's
/// meshes, rebuilt every frame with a few quads changing (a hover highlight, a text): a sample missed those (old
/// glyphs and highlights shown).
void vertices_written(const void* data);

}  // namespace joan::stream_cache_api
