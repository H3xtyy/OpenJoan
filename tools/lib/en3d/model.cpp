#include "en3d/model.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>

#include "core/binary_reader.hpp"

namespace joan::en3d {

namespace {

constexpr std::uint32_t kVersion = 500;

Vec3 read_vec3(BinaryReader& r) {
    Vec3 v;
    v.x = r.f32();
    v.y = r.f32();
    v.z = r.f32();
    return v;
}

// Stored as four floats in A, R, G, B order (multiplied by 255 into a D3DCOLOR by the engine).
Color read_argb(BinaryReader& r) {
    Color c;
    c.a = r.f32();
    c.r = r.f32();
    c.g = r.f32();
    c.b = r.f32();
    return c;
}

std::vector<std::uint16_t> read_strip(BinaryReader& r, std::uint32_t strip_count, std::size_t vertex_count) {
    if (strip_count == 0) {
        // No index data: the vertices themselves form a triangle list.
        std::vector<std::uint16_t> list(vertex_count - vertex_count % 3);
        for (std::size_t i = 0; i < list.size(); ++i) list[i] = static_cast<std::uint16_t>(i);
        return list;
    }
    std::vector<std::uint16_t> strip(static_cast<std::size_t>(strip_count) + 2);
    for (auto& i : strip) {
        i = r.u16();
        if (i >= vertex_count) throw FormatError("mesh index out of range");
    }
    return strip_to_list(strip);
}

void read_sections(BinaryReader& r, Model& model) {
    // Optional trailing sections, terminated by any other id (u32 0) or end of file.
    while (r.remaining() >= 4) {
        Section s;
        s.id = r.u32();
        std::size_t size = 0;
        switch (s.id) {
            case 1: size = model.meshes.size() * 8; break;     // per mesh {i32, i32}
            case 2: size = model.meshes.size() * 9; break;     // per mesh {i32, i32, u8}
            case 3: size = 8; break;                            // {u32, u32}, usually last in the file
            default: return;
        }
        auto bytes = r.bytes(size);
        s.data.assign(bytes.begin(), bytes.end());
        model.sections.push_back(std::move(s));
    }
}

void read_textures(BinaryReader& r, Model& model, std::uint32_t count) {
    model.textures.resize(count);
    for (auto& t : model.textures) {
        t.name = r.fixed_string(20);
        const std::uint32_t size = r.u32();
        auto bytes = r.bytes(size);
        t.embedded.assign(bytes.begin(), bytes.end());
    }
}

void check_texture_indices(const Model& model) {
    for (const auto& m : model.meshes)
        if (m.texture >= 0 && static_cast<std::size_t>(m.texture) >= model.textures.size())
            throw FormatError("mesh texture index out of range");
}

}  // namespace

std::vector<std::uint16_t> strip_to_list(std::span<const std::uint16_t> strip) {
    std::vector<std::uint16_t> list;
    if (strip.size() < 3) return list;
    list.reserve((strip.size() - 2) * 3);
    for (std::size_t i = 0; i + 2 < strip.size(); ++i) {
        std::uint16_t a = strip[i], b = strip[i + 1];
        const std::uint16_t c = strip[i + 2];
        if (a == b || b == c || a == c) continue;
        if (i & 1) std::swap(a, b);
        list.insert(list.end(), {a, b, c});
    }
    return list;
}

Model load_am1(std::span<const std::byte> data) {
    BinaryReader r(data);
    Model model;
    model.version = r.u32();
    if (model.version != kVersion) throw FormatError("unsupported AM1 version " + std::to_string(model.version));
    model.bone_count = r.u32();
    const std::uint32_t mesh_count = r.u32();
    const std::uint32_t texture_count = r.u32();
    r.u32();  // always 0
    model.sphere_center = read_vec3(r);
    model.sphere_radius = r.f32();
    if (mesh_count > 4096 || texture_count > 4096 || model.bone_count > 4096) throw FormatError("implausible AM1 header");

    model.meshes.resize(mesh_count);
    for (auto& mesh : model.meshes) {
        const std::uint32_t strip_count = r.u32();
        const std::uint32_t vertex_count = r.u32();
        if (vertex_count > 65536) throw FormatError("too many vertices in AM1 mesh");
        mesh.material.ambient = read_argb(r);
        mesh.material.diffuse = read_argb(r);
        mesh.material.specular = read_argb(r);
        // AM1 has no emissive colour: the 4th block is the mesh's bounding sphere
        // (SMO stores both). Emissive stays black.
        mesh.sphere_center = read_vec3(r);
        mesh.sphere_radius = r.f32();
        mesh.texture = r.i32();
        mesh.indices = read_strip(r, strip_count, vertex_count);

        mesh.uv.resize(vertex_count);
        mesh.influence_begin.reserve(vertex_count + 1);
        mesh.source_begin.reserve(vertex_count + 1);
        for (std::uint32_t v = 0; v < vertex_count; ++v) {
            mesh.uv[v].x = r.f32();
            mesh.uv[v].y = r.f32();
            const std::uint32_t source_count = r.u32();
            const std::uint32_t influence_count = r.u32();
            if (source_count > 4096 || influence_count == 0 || influence_count > 64)
                throw FormatError("bad AM1 vertex");
            mesh.source_begin.push_back(static_cast<std::uint32_t>(mesh.source_ids.size()));
            for (std::uint32_t i = 0; i < source_count; ++i) mesh.source_ids.push_back(r.u32());
            mesh.influence_begin.push_back(static_cast<std::uint32_t>(mesh.influences.size()));
            for (std::uint32_t i = 0; i < influence_count; ++i) {
                Influence inf;
                inf.bone = r.u32();
                if (inf.bone >= model.bone_count) throw FormatError("AM1 bone index out of range");
                inf.weight = r.f32();
                inf.position = read_vec3(r);
                inf.normal = read_vec3(r);
                mesh.influences.push_back(inf);
            }
        }
        mesh.influence_begin.push_back(static_cast<std::uint32_t>(mesh.influences.size()));
        mesh.source_begin.push_back(static_cast<std::uint32_t>(mesh.source_ids.size()));
    }
    read_textures(r, model, texture_count);
    read_sections(r, model);
    check_texture_indices(model);
    return model;
}

Model load_smo(std::span<const std::byte> data) {
    BinaryReader r(data);
    Model model;
    model.version = r.u32();
    if (model.version != kVersion) throw FormatError("unsupported SMO version " + std::to_string(model.version));
    const std::uint32_t mesh_count = r.u32();
    const std::uint32_t texture_count = r.u32();
    r.u32();  // always 0
    model.sphere_center = read_vec3(r);
    model.sphere_radius = r.f32();
    if (mesh_count > 4096 || texture_count > 4096) throw FormatError("implausible SMO header");

    model.meshes.resize(mesh_count);
    for (auto& mesh : model.meshes) {
        const std::uint32_t strip_count = r.u32();
        const std::uint32_t vertex_count = r.u32();
        if (vertex_count > 65536) throw FormatError("too many vertices in SMO mesh");
        mesh.material.ambient = read_argb(r);
        mesh.material.diffuse = read_argb(r);
        mesh.material.specular = read_argb(r);
        mesh.material.emissive = read_argb(r);
        mesh.sphere_center = read_vec3(r);
        mesh.sphere_radius = r.f32();
        mesh.texture = r.i32();
        mesh.indices = read_strip(r, strip_count, vertex_count);

        mesh.uv.resize(vertex_count);
        mesh.positions.resize(vertex_count);
        mesh.normals.resize(vertex_count);
        for (std::uint32_t v = 0; v < vertex_count; ++v) {
            mesh.normals[v] = read_vec3(r);
            mesh.positions[v] = read_vec3(r);
            mesh.uv[v].x = r.f32();
            mesh.uv[v].y = r.f32();
        }
    }
    read_textures(r, model, texture_count);
    read_sections(r, model);
    check_texture_indices(model);
    return model;
}

Model load_model(std::span<const std::byte> data, std::string_view file_name) {
    std::string ext;
    if (const auto dot = file_name.rfind('.'); dot != std::string_view::npos)
        for (char c : file_name.substr(dot)) ext += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    if (ext == ".am1") return load_am1(data);
    if (ext == ".smo") return load_smo(data);
    throw FormatError("not a model file: " + std::string(file_name));
}

Skeleton load_am3(std::span<const std::byte> data) {
    BinaryReader r(data);
    Skeleton s;
    const std::uint32_t count = r.u32();
    if (count > 4096) throw FormatError("implausible AM3 bone count");
    s.bones.resize(count);
    for (auto& b : s.bones) {
        b.id = r.u32();
        b.name = r.fixed_string(50);
        const std::uint32_t children = r.u32();
        if (children > count) throw FormatError("bad AM3 child count");
        for (std::uint32_t i = 0; i < children; ++i) {
            const std::uint32_t c = r.u32();
            if (c >= count) throw FormatError("AM3 child index out of range");
            b.children.push_back(c);
        }
    }
    for (std::uint32_t i = 0; i < count; ++i)
        for (std::uint32_t c : s.bones[i].children) s.bones[c].parent = static_cast<std::int32_t>(i);
    return s;
}

void skin_mesh(const Mesh& mesh, std::span<const Mat34> bones, std::vector<Vec3>& positions,
               std::vector<Vec3>& normals) {
    const std::size_t n = mesh.vertex_count();
    positions.resize(n);
    normals.resize(n);
    if (!mesh.skinned()) {
        std::copy(mesh.positions.begin(), mesh.positions.end(), positions.begin());
        std::copy(mesh.normals.begin(), mesh.normals.end(), normals.begin());
        return;
    }
    for (std::size_t v = 0; v < n; ++v) {
        Vec3 p, nrm;
        for (std::uint32_t i = mesh.influence_begin[v]; i < mesh.influence_begin[v + 1]; ++i) {
            const Influence& inf = mesh.influences[i];
            const Mat34& m = inf.bone < bones.size() ? bones[inf.bone] : Mat34{};
            p += m.transform_point(inf.position) * inf.weight;
            nrm += m.transform_vector(inf.normal) * inf.weight;
        }
        positions[v] = p;
        normals[v] = normalize(nrm);
    }
}

}  // namespace joan::en3d
