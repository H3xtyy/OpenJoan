// OpenGL 3.3 renderer for en3d models (replaces the D3D8 fixed-function mesh path
// of en3d/ plus its vertex shaders in shader/*.nvo).
//
// Skinning is done on the CPU for now: AM1 vertices carry a separate bone-space
// position per influence, which does not map onto the usual "one position +
// 4 weights" GPU layout without converting the data first. Unit meshes are
// small (~2,000 vertices), so this is cheap; GPU skinning comes with the
// in-game renderer.
#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "en3d/asset_paths.hpp"
#include "en3d/math.hpp"
#include "en3d/model.hpp"

namespace joan::render {

class Shader {
public:
    Shader(const char* vertex_src, const char* fragment_src);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    void use() const;
    [[nodiscard]] int uniform(const char* name) const;

private:
    unsigned program_ = 0;
};

/// RGBA8 texture with mipmaps.
class Texture {
public:
    Texture(std::uint32_t width, std::uint32_t height, const std::uint8_t* rgba);
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    void bind(unsigned unit = 0) const;

private:
    unsigned id_ = 0;
};

struct DrawParams {
    en3d::Mat4 view_projection;
    en3d::Vec3 light_dir{0.4f, -0.7f, 0.6f};  // towards the light, model space
    bool wireframe = false;
};

class ModelRenderer {
public:
    /// Uploads the model; textures are looked up by name through `textures`.
    ModelRenderer(const en3d::Model& model, const en3d::TextureIndex& textures);
    ~ModelRenderer();
    ModelRenderer(const ModelRenderer&) = delete;
    ModelRenderer& operator=(const ModelRenderer&) = delete;

    /// Poses the model (ignored for static models) and draws it.
    void draw(std::span<const en3d::Mat34> bones, const DrawParams& params);

    /// Axis-aligned bounds of the last drawn pose (or of the rest data).
    [[nodiscard]] en3d::Vec3 bounds_min() const { return min_; }
    [[nodiscard]] en3d::Vec3 bounds_max() const { return max_; }
    void update_pose(std::span<const en3d::Mat34> bones);

    [[nodiscard]] std::size_t missing_textures() const { return missing_textures_; }

private:
    struct GpuMesh {
        unsigned vao = 0, vbo_dynamic = 0, vbo_uv = 0, ibo = 0;
        int index_count = 0;
        int texture = -1;
    };

    const en3d::Model& model_;
    std::vector<GpuMesh> meshes_;
    std::vector<std::unique_ptr<Texture>> textures_;
    std::vector<std::vector<float>> scratch_;  // interleaved pos+normal per mesh
    std::vector<en3d::Vec3> pos_, nrm_;
    en3d::Vec3 min_, max_;
    std::size_t missing_textures_ = 0;
};

/// Draws coloured line segments (skeleton, grid). Coordinates in model space.
class LineRenderer {
public:
    LineRenderer();
    ~LineRenderer();
    LineRenderer(const LineRenderer&) = delete;
    LineRenderer& operator=(const LineRenderer&) = delete;

    void clear() { data_.clear(); }
    void add(en3d::Vec3 a, en3d::Vec3 b, en3d::Color c);
    void draw(const en3d::Mat4& view_projection, bool on_top);

private:
    unsigned vao_ = 0, vbo_ = 0;
    std::vector<float> data_;
};

/// Frees the shared GL programs; call before the GL context is destroyed.
void release_shaders();

}  // namespace joan::render
