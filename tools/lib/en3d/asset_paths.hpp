// Finding the files that belong together: a mesh names its textures as
// "Joan.tga" / "3dmapicon.bmp" but the game ships them as DDS somewhere under
// texture/ (texture/unit/Joan.dds, texture/3dmapicon.dds), and a unit's
// animation lives in anim/ with the mesh's base name.
#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace joan::en3d {

class TextureIndex {
public:
    /// Scans <game_dir>/texture recursively.
    explicit TextureIndex(const std::filesystem::path& game_dir);

    /// "Joan.tga" -> .../texture/unit/Joan.dds (match on the base name, any case).
    [[nodiscard]] std::optional<std::filesystem::path> find(std::string_view texture_name) const;

    [[nodiscard]] std::size_t size() const { return by_stem_.size(); }

private:
    std::unordered_map<std::string, std::filesystem::path> by_stem_;
};

/// Lower-cased file name without directory and extension.
std::string stem_lower(std::string_view path);

/// Finds <model dir>/<stem>.AM3 and anim/<stem>.AM2 (any case) for a model file.
std::optional<std::filesystem::path> find_sibling(const std::filesystem::path& model_file, std::string_view extension);
std::optional<std::filesystem::path> find_animation(const std::filesystem::path& game_dir,
                                                    const std::filesystem::path& model_file);

}  // namespace joan::en3d
