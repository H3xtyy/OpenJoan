// DirectDraw Surface loader. The game's textures are DXT3 (unit skins), DXT1/DXT5
// and a few uncompressed DDS files, loaded by D3DX in the original. We decode the
// top mip level to RGBA8 on the CPU so the renderer does not depend on the
// S3TC extension (absent on GLES / some mobile drivers).
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace joan::en3d {

struct Image {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::vector<std::uint8_t> rgba;  // width * height * 4, first row = top of the image
    std::string format;              // "DXT1", "DXT3", "DXT5", "A8R8G8B8", ...
    std::uint32_t mip_count = 1;
};

Image load_dds(std::span<const std::byte> data);

}  // namespace joan::en3d
