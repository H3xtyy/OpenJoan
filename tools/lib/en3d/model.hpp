// Mesh formats of the Enlight 3D engine (en3d/): .AM1 skinned meshes,
// .SMO static meshes and .AM3 bone hierarchies.
//
// The original loaders are joan.exe 0x5da280 (AM1, dispatching on the version
// field to 0x5da3d0 / 0x5db710 / 0x5dc400 / 0x5dc420) and 0x5ddd70 (SMO). Every
// file shipped with the game is version 500, the only one supported here.
// Byte layouts are documented in docs/FORMATS.md.
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "en3d/math.hpp"

namespace joan::en3d {

struct Material {
    Color ambient, diffuse, specular;
    Color emissive{0, 0, 0, 1};  // SMO only; AM1 meshes have none
};

/// One weighted bone contribution of a skinned vertex. The position and normal
/// are expressed in that bone's space: the model-space vertex is
/// sum(weight * bone_matrix * position) over its influences (no bind pose needed).
struct Influence {
    std::uint32_t bone = 0;
    float weight = 0;
    Vec3 position;
    Vec3 normal;
};

struct Mesh {
    Material material;
    Vec3 sphere_center;  // bounding sphere of the mesh
    float sphere_radius = 0;
    std::int32_t texture = -1;  // index into Model::textures, -1 = untextured

    /// Triangle list (converted from the file's triangle strip, degenerates removed).
    std::vector<std::uint16_t> indices;

    std::vector<Vec2> uv;  // per vertex (D3D convention: v grows downwards)

    // Static meshes (SMO): model-space positions and normals.
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;

    // Skinned meshes (AM1): vertex i uses influences[influence_begin[i] .. influence_begin[i+1]).
    std::vector<std::uint32_t> influence_begin;
    std::vector<Influence> influences;

    /// AM1 only: a short per-vertex list of u32 ids whose meaning is not known yet.
    /// The values exceed the mesh's vertex count and are shared between meshes,
    /// so they probably reference the source 3ds Max mesh (e.g. its face ids).
    /// Same indexing scheme as influences; unused by the renderer.
    std::vector<std::uint32_t> source_begin;
    std::vector<std::uint32_t> source_ids;

    [[nodiscard]] std::size_t vertex_count() const { return uv.size(); }
    [[nodiscard]] bool skinned() const { return !influence_begin.empty(); }
};

struct TextureRef {
    std::string name;                 // as stored, e.g. "Joan.tga" (the game loads texture/**/Joan.dds)
    std::vector<std::byte> embedded;  // in-file image (rare); empty = load by name
};

/// Optional trailer sections (ids 1 and 2: per mesh, probably extra texture stages; id 3: 8 bytes).
struct Section {
    std::uint32_t id = 0;
    std::vector<std::byte> data;
};

struct Model {
    std::uint32_t version = 0;
    std::uint32_t bone_count = 0;  // 0 for SMO
    Vec3 sphere_center;
    float sphere_radius = 0;
    std::vector<Mesh> meshes;
    std::vector<TextureRef> textures;
    std::vector<Section> sections;

    [[nodiscard]] bool skinned() const { return bone_count > 0; }
};

Model load_am1(std::span<const std::byte> data);
Model load_smo(std::span<const std::byte> data);
/// Picks the loader by file extension (".am1" / ".smo", case-insensitive).
Model load_model(std::span<const std::byte> data, std::string_view file_name);

/// .AM3: bone names and hierarchy, in the same order as the AM1/AM2 bone indices.
struct Bone {
    std::uint32_t id = 0;
    std::string name;  // "Bip01", "Bip01 Pelvis", ...
    std::vector<std::uint32_t> children;
    std::int32_t parent = -1;
};

struct Skeleton {
    std::vector<Bone> bones;
};

Skeleton load_am3(std::span<const std::byte> data);

/// Computes model-space positions and normals of a skinned mesh for the given
/// per-bone model-space transforms (e.g. from Animation::sample). For a static
/// mesh it just copies the stored ones.
void skin_mesh(const Mesh& mesh, std::span<const Mat34> bones, std::vector<Vec3>& positions,
               std::vector<Vec3>& normals);

/// Converts a D3D triangle strip (alternating winding) to a triangle list,
/// dropping degenerate triangles.
std::vector<std::uint16_t> strip_to_list(std::span<const std::uint16_t> strip);

}  // namespace joan::en3d
