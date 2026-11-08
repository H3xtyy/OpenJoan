#include "render/model_renderer.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>

#include "core/file_util.hpp"
#include "core/log.hpp"
#include "en3d/dds.hpp"
#include "render/gl.hpp"

namespace joan::render {

namespace {

const char* kMeshVs = R"(#version 330 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nrm;
layout(location = 2) in vec2 a_uv;
uniform mat4 u_mvp;
out vec3 v_nrm;
out vec2 v_uv;
void main() {
    v_nrm = a_nrm;
    v_uv = a_uv;
    gl_Position = u_mvp * vec4(a_pos, 1.0);
}
)";

// Fixed-function D3D lighting of the original, reduced to one directional light:
// colour = texture * (ambient * scene_ambient + diffuse * N.L) + emissive.
const char* kMeshFs = R"(#version 330 core
in vec3 v_nrm;
in vec2 v_uv;
uniform sampler2D u_tex;
uniform int u_textured;
uniform vec4 u_ambient;
uniform vec4 u_diffuse;
uniform vec4 u_emissive;
uniform vec3 u_light;
out vec4 o_color;
void main() {
    vec4 tex = u_textured != 0 ? texture(u_tex, v_uv) : vec4(1.0);
    if (tex.a < 0.3) discard;
    float ndl = abs(dot(normalize(v_nrm), u_light));  // meshes are drawn two-sided
    vec3 light = vec3(0.15) + u_ambient.rgb * 0.6 + u_diffuse.rgb * ndl * 1.1 + u_emissive.rgb;  // 0.15: viewer fill light
    o_color = vec4(tex.rgb * light, tex.a * u_diffuse.a);
}
)";

const char* kLineVs = R"(#version 330 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_color;
uniform mat4 u_mvp;
out vec4 v_color;
void main() {
    v_color = a_color;
    gl_Position = u_mvp * vec4(a_pos, 1.0);
}
)";

const char* kLineFs = R"(#version 330 core
in vec4 v_color;
out vec4 o_color;
void main() { o_color = v_color; }
)";

unsigned compile(gl::GLenum type, const char* src) {
    const unsigned s = gl::CreateShader(type);
    gl::ShaderSource(s, 1, &src, nullptr);
    gl::CompileShader(s);
    gl::GLint ok = 0;
    gl::GetShaderiv(s, gl::COMPILE_STATUS, &ok);
    if (!ok) {
        char msg[2048] = {};
        gl::GetShaderInfoLog(s, sizeof msg, nullptr, msg);
        gl::DeleteShader(s);
        throw std::runtime_error(std::string("shader compile failed: ") + msg);
    }
    return s;
}

std::unique_ptr<Shader> g_mesh_shader;  // created on first use, lives as long as the GL context
std::unique_ptr<Shader> g_line_shader;

Shader& mesh_shader() {
    if (!g_mesh_shader) g_mesh_shader = std::make_unique<Shader>(kMeshVs, kMeshFs);
    return *g_mesh_shader;
}
Shader& line_shader() {
    if (!g_line_shader) g_line_shader = std::make_unique<Shader>(kLineVs, kLineFs);
    return *g_line_shader;
}

void set_color(int loc, const en3d::Color& c) {
    const float v[4] = {c.r, c.g, c.b, c.a};
    gl::Uniform4fv(loc, 1, v);
}

}  // namespace

// ---------------------------------------------------------------------------

Shader::Shader(const char* vertex_src, const char* fragment_src) {
    const unsigned vs = compile(gl::VERTEX_SHADER, vertex_src);
    const unsigned fs = compile(gl::FRAGMENT_SHADER, fragment_src);
    program_ = gl::CreateProgram();
    gl::AttachShader(program_, vs);
    gl::AttachShader(program_, fs);
    gl::LinkProgram(program_);
    gl::DeleteShader(vs);
    gl::DeleteShader(fs);
    gl::GLint ok = 0;
    gl::GetProgramiv(program_, gl::LINK_STATUS, &ok);
    if (!ok) {
        char msg[2048] = {};
        gl::GetProgramInfoLog(program_, sizeof msg, nullptr, msg);
        throw std::runtime_error(std::string("shader link failed: ") + msg);
    }
}

Shader::~Shader() { gl::DeleteProgram(program_); }
void Shader::use() const { gl::UseProgram(program_); }
int Shader::uniform(const char* name) const { return gl::GetUniformLocation(program_, name); }

// ---------------------------------------------------------------------------

Texture::Texture(std::uint32_t width, std::uint32_t height, const std::uint8_t* rgba) {
    gl::GenTextures(1, &id_);
    gl::BindTexture(gl::TEXTURE_2D, id_);
    gl::PixelStorei(gl::UNPACK_ALIGNMENT, 1);
    gl::TexImage2D(gl::TEXTURE_2D, 0, static_cast<gl::GLint>(gl::RGBA8), static_cast<gl::GLsizei>(width),
                   static_cast<gl::GLsizei>(height), 0, gl::RGBA, gl::UNSIGNED_BYTE, rgba);
    gl::GenerateMipmap(gl::TEXTURE_2D);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, static_cast<gl::GLint>(gl::LINEAR_MIPMAP_LINEAR));
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, static_cast<gl::GLint>(gl::LINEAR));
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, static_cast<gl::GLint>(gl::REPEAT));
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, static_cast<gl::GLint>(gl::REPEAT));
}

Texture::~Texture() { gl::DeleteTextures(1, &id_); }

void Texture::bind(unsigned unit) const {
    gl::ActiveTexture(gl::TEXTURE0 + unit);
    gl::BindTexture(gl::TEXTURE_2D, id_);
}

// ---------------------------------------------------------------------------

ModelRenderer::ModelRenderer(const en3d::Model& model, const en3d::TextureIndex& textures) : model_(model) {
    textures_.resize(model.textures.size());
    for (std::size_t i = 0; i < model.textures.size(); ++i) {
        const auto& ref = model.textures[i];
        try {
            en3d::Image img;
            if (!ref.embedded.empty()) {
                img = en3d::load_dds(ref.embedded);
            } else if (auto path = textures.find(ref.name)) {
                img = en3d::load_dds(read_file(*path));
            } else {
                log::warn("texture {} not found under texture/", ref.name);
                ++missing_textures_;
                continue;
            }
            textures_[i] = std::make_unique<Texture>(img.width, img.height, img.rgba.data());
        } catch (const std::exception& e) {
            log::warn("texture {}: {}", ref.name, e.what());
            ++missing_textures_;
        }
    }

    meshes_.resize(model.meshes.size());
    scratch_.resize(model.meshes.size());
    for (std::size_t i = 0; i < model.meshes.size(); ++i) {
        const en3d::Mesh& m = model.meshes[i];
        GpuMesh& g = meshes_[i];
        g.index_count = static_cast<int>(m.indices.size());
        g.texture = m.texture;
        gl::GenVertexArrays(1, &g.vao);
        gl::BindVertexArray(g.vao);

        gl::GenBuffers(1, &g.vbo_dynamic);
        gl::BindBuffer(gl::ARRAY_BUFFER, g.vbo_dynamic);
        gl::BufferData(gl::ARRAY_BUFFER, static_cast<gl::GLsizeiptr>(m.vertex_count() * 6 * sizeof(float)), nullptr,
                       m.skinned() ? gl::DYNAMIC_DRAW : gl::STATIC_DRAW);
        gl::VertexAttribPointer(0, 3, gl::FLOAT, 0, 6 * sizeof(float), nullptr);
        gl::VertexAttribPointer(1, 3, gl::FLOAT, 0, 6 * sizeof(float), reinterpret_cast<const void*>(3 * sizeof(float)));
        gl::EnableVertexAttribArray(0);
        gl::EnableVertexAttribArray(1);

        gl::GenBuffers(1, &g.vbo_uv);
        gl::BindBuffer(gl::ARRAY_BUFFER, g.vbo_uv);
        gl::BufferData(gl::ARRAY_BUFFER, static_cast<gl::GLsizeiptr>(m.uv.size() * sizeof(en3d::Vec2)), m.uv.data(),
                       gl::STATIC_DRAW);
        gl::VertexAttribPointer(2, 2, gl::FLOAT, 0, sizeof(en3d::Vec2), nullptr);
        gl::EnableVertexAttribArray(2);

        gl::GenBuffers(1, &g.ibo);
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, g.ibo);
        gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, static_cast<gl::GLsizeiptr>(m.indices.size() * sizeof(std::uint16_t)),
                       m.indices.data(), gl::STATIC_DRAW);
    }
    gl::BindVertexArray(0);
    update_pose({});
}

ModelRenderer::~ModelRenderer() {
    for (auto& g : meshes_) {
        gl::DeleteBuffers(1, &g.vbo_dynamic);
        gl::DeleteBuffers(1, &g.vbo_uv);
        gl::DeleteBuffers(1, &g.ibo);
        gl::DeleteVertexArrays(1, &g.vao);
    }
}

void ModelRenderer::update_pose(std::span<const en3d::Mat34> bones) {
    min_ = {1e30f, 1e30f, 1e30f};
    max_ = {-1e30f, -1e30f, -1e30f};
    for (std::size_t i = 0; i < model_.meshes.size(); ++i) {
        const en3d::Mesh& m = model_.meshes[i];
        en3d::skin_mesh(m, bones, pos_, nrm_);
        auto& buf = scratch_[i];
        buf.resize(pos_.size() * 6);
        for (std::size_t v = 0; v < pos_.size(); ++v) {
            const en3d::Vec3 p = pos_[v], n = nrm_[v];
            float* d = &buf[v * 6];
            d[0] = p.x, d[1] = p.y, d[2] = p.z, d[3] = n.x, d[4] = n.y, d[5] = n.z;
            min_ = {std::min(min_.x, p.x), std::min(min_.y, p.y), std::min(min_.z, p.z)};
            max_ = {std::max(max_.x, p.x), std::max(max_.y, p.y), std::max(max_.z, p.z)};
        }
        gl::BindBuffer(gl::ARRAY_BUFFER, meshes_[i].vbo_dynamic);
        gl::BufferSubData(gl::ARRAY_BUFFER, 0, static_cast<gl::GLsizeiptr>(buf.size() * sizeof(float)), buf.data());
    }
}

void ModelRenderer::draw(std::span<const en3d::Mat34> bones, const DrawParams& params) {
    if (model_.skinned()) update_pose(bones);
    Shader& sh = mesh_shader();
    sh.use();
    gl::UniformMatrix4fv(sh.uniform("u_mvp"), 1, 0, params.view_projection.m.data());
    const en3d::Vec3 l = en3d::normalize(params.light_dir);
    const float lv[3] = {l.x, l.y, l.z};
    gl::Uniform3fv(sh.uniform("u_light"), 1, lv);
    gl::Uniform1i(sh.uniform("u_tex"), 0);
    gl::Enable(gl::DEPTH_TEST);
    gl::DepthFunc(gl::LEQUAL);
    gl::Disable(gl::CULL_FACE);
    gl::Enable(gl::BLEND);
    gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
    if (params.wireframe) gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
    for (std::size_t i = 0; i < meshes_.size(); ++i) {
        const GpuMesh& g = meshes_[i];
        const en3d::Material& mat = model_.meshes[i].material;
        const bool textured = g.texture >= 0 && textures_[static_cast<std::size_t>(g.texture)] != nullptr;
        if (textured) textures_[static_cast<std::size_t>(g.texture)]->bind(0);
        gl::Uniform1i(sh.uniform("u_textured"), textured ? 1 : 0);
        set_color(sh.uniform("u_ambient"), mat.ambient);
        set_color(sh.uniform("u_diffuse"), mat.diffuse);
        set_color(sh.uniform("u_emissive"), mat.emissive);
        gl::BindVertexArray(g.vao);
        gl::DrawElements(gl::TRIANGLES, g.index_count, gl::UNSIGNED_SHORT, nullptr);
    }
    gl::BindVertexArray(0);
    if (params.wireframe) gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
}

// ---------------------------------------------------------------------------

LineRenderer::LineRenderer() {
    gl::GenVertexArrays(1, &vao_);
    gl::GenBuffers(1, &vbo_);
    gl::BindVertexArray(vao_);
    gl::BindBuffer(gl::ARRAY_BUFFER, vbo_);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, 0, 7 * sizeof(float), nullptr);
    gl::VertexAttribPointer(1, 4, gl::FLOAT, 0, 7 * sizeof(float), reinterpret_cast<const void*>(3 * sizeof(float)));
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);
    gl::BindVertexArray(0);
}

LineRenderer::~LineRenderer() {
    gl::DeleteBuffers(1, &vbo_);
    gl::DeleteVertexArrays(1, &vao_);
}

void LineRenderer::add(en3d::Vec3 a, en3d::Vec3 b, en3d::Color c) {
    data_.insert(data_.end(), {a.x, a.y, a.z, c.r, c.g, c.b, c.a, b.x, b.y, b.z, c.r, c.g, c.b, c.a});
}

void LineRenderer::draw(const en3d::Mat4& view_projection, bool on_top) {
    if (data_.empty()) return;
    Shader& sh = line_shader();
    sh.use();
    gl::UniformMatrix4fv(sh.uniform("u_mvp"), 1, 0, view_projection.m.data());
    gl::BindVertexArray(vao_);
    gl::BindBuffer(gl::ARRAY_BUFFER, vbo_);
    gl::BufferData(gl::ARRAY_BUFFER, static_cast<gl::GLsizeiptr>(data_.size() * sizeof(float)), data_.data(),
                   gl::DYNAMIC_DRAW);
    if (on_top) gl::Disable(gl::DEPTH_TEST);
    else gl::Enable(gl::DEPTH_TEST);
    gl::DrawArrays(gl::LINES, 0, static_cast<gl::GLsizei>(data_.size() / 7));
    gl::Enable(gl::DEPTH_TEST);
    gl::BindVertexArray(0);
}

void release_shaders() {
    g_mesh_shader.reset();
    g_line_shader.reset();
}

}  // namespace joan::render
