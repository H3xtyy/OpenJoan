// Texture files and texture contents, bit-exact with the D3DX8 code statically linked into
// joan.exe (D3DXCreateTextureFromFileInMemoryEx 0x00765258 and what it calls).
//
//  * decode_image: the file loaders (0x0078856c) - BMP / DIB (0x0078853c / 0x00786cd8), DDS
//    (0x00788229) and TGA (0x007875ca). The pixels stay in the D3DFORMAT the loader reports.
//  * plan_texture: D3DX's argument defaults (0x00765258) and D3DXCheckTextureRequirements
//    (0x00764064): size, level count and format of the texture, decided from the device caps
//    and CheckDeviceFormat answers the caller passes in.
//  * build_texture: the bytes of every level - D3DXLoadSurfaceFromMemory (0x00764446, blitter
//    0x00786b38) for the file's levels, D3DXFilterTexture (0x00764d45, box filter 0x0078599b)
//    for the rest, including D3DX's DXT compressor (0x0079a153) for DXT levels it re-encodes.
//
// Anything outside the exactly reproduced subset makes these functions return false with a
// reason, and the caller is expected to run the original D3DX code instead. Handled: sources
// DXT1/3/5, A8R8G8B8, X8R8G8B8, R8G8B8 and P8 (every format the game's files use), targets the
// same DXT format, A8R8G8B8 and X8R8G8B8, same-size loads (no resampling), box mip filter.
//
// Floating-point steps follow the x87 code: `Precision::Single` is precision control 24 bits
// (what Direct3D 8 sets), `Precision::Double` 53 bits; round-to-nearest is assumed (D3DX
// switches to truncation itself where it does). Portable C++ (no Windows headers).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace joan::render {

/// Direct3D 8 surface format values (D3DFMT_*) used by the texture code.
namespace d3dfmt {
inline constexpr std::uint32_t kUnknown = 0 /**< D3DFMT_UNKNOWN */, kR8G8B8 = 20 /**< D3DFMT_R8G8B8 */, kA8R8G8B8 = 21 /**< D3DFMT_A8R8G8B8 */, kX8R8G8B8 = 22 /**< D3DFMT_X8R8G8B8 */, kR5G6B5 = 23 /**< D3DFMT_R5G6B5 */,
                               kX1R5G5B5 = 24 /**< D3DFMT_X1R5G5B5 */, kA1R5G5B5 = 25 /**< D3DFMT_A1R5G5B5 */, kA4R4G4B4 = 26 /**< D3DFMT_A4R4G4B4 */, kR3G3B2 = 27 /**< D3DFMT_R3G3B2 */, kA8 = 28 /**< D3DFMT_A8 */,
                               kA8R3G3B2 = 29 /**< D3DFMT_A8R3G3B2 */, kX4R4G4B4 = 30 /**< D3DFMT_X4R4G4B4 */, kA8P8 = 40 /**< D3DFMT_A8P8 */, kP8 = 41 /**< D3DFMT_P8 */, kL8 = 50 /**< D3DFMT_L8 */, kA8L8 = 51 /**< D3DFMT_A8L8 */,
                               kA4L4 = 52 /**< D3DFMT_A4L4 */;
inline constexpr std::uint32_t kDXT1 = 0x31545844 /**< D3DFMT_DXT1 */, kDXT2 = 0x32545844 /**< D3DFMT_DXT2 */, kDXT3 = 0x33545844 /**< D3DFMT_DXT3 */, kDXT4 = 0x34545844 /**< D3DFMT_DXT4 */,
                               kDXT5 = 0x35545844 /**< D3DFMT_DXT5 */;
/// Whether `f` is one of the DXT1..DXT5 formats.
inline bool is_dxt(std::uint32_t f) { return f >= kDXT1 && f <= kDXT5 && (f & 0x00ffffff) == 0x00545844; }
}  // namespace d3dfmt

/// Bytes per row (per row of 4x4 blocks for DXT) of a tightly packed level; 0 = unknown format.
std::uint32_t row_bytes(std::uint32_t format, std::uint32_t width);
/// Rows (rows of 4x4 blocks for DXT) of a level.
std::uint32_t row_count(std::uint32_t format, std::uint32_t height);

/// One mip level of pixel data, in the Image's D3DFORMAT.
struct ImageLevel {
    std::uint32_t width = 0;  ///< width in pixels
    std::uint32_t height = 0;  ///< height in pixels
    std::uint32_t pitch = 0;          ///< bytes from one row (row of blocks) to the next
    std::vector<std::uint8_t> data;   ///< pitch * rows bytes (rows: row_count)
};

/// File formats decode_image recognises: BMP, DIB, DDS, TGA.
enum class ImageFileType { Bmp, Dib, Dds, Tga };

/// A decoded texture file (decode_image's result); owns its pixel data.
struct Image {
    ImageFileType type = ImageFileType::Bmp;  ///< the file's container format
    std::uint32_t format = 0;          ///< D3DFORMAT of the pixels as the D3DX loader reports it
    std::vector<ImageLevel> levels;    ///< top level, then the file's mip chain (DDS)
    bool has_palette = false;          ///< `palette` is valid (palettised sources)
    std::array<std::uint32_t, 256> palette{};  ///< PALETTEENTRY dwords (bytes red, green, blue, flags)
    bool cube = false;                 ///< DDS cube map (2D loads use face 0)

    /// Width of the top level in pixels; 0 when there is none.
    std::uint32_t width() const { return levels.empty() ? 0 : levels[0].width; }
    /// Height of the top level in pixels; 0 when there is none.
    std::uint32_t height() const { return levels.empty() ? 0 : levels[0].height; }
};

/// Decodes a file as D3DX8's loaders do. false when the data is not a BMP / DIB / DDS / TGA that
/// D3DX would read the same way (PNG / JPG / PPM, malformed or unusual files, ...).
bool decode_image(const void* data, std::size_t size, Image& out, std::string* why = nullptr);

/// The device answers D3DX bases its decisions on.
struct DeviceQuery {
    std::uint32_t texture_caps = 0;               ///< D3DCAPS8::TextureCaps
    std::uint32_t max_texture_width = 0;  ///< D3DCAPS8::MaxTextureWidth, pixels
    std::uint32_t max_texture_height = 0;  ///< D3DCAPS8::MaxTextureHeight, pixels
    std::uint32_t max_texture_aspect_ratio = 0;  ///< D3DCAPS8::MaxTextureAspectRatio (0 = unlimited)
    /// IDirect3D8::CheckDeviceFormat(adapter, device type, display format, usage, D3DRTYPE_TEXTURE,
    /// format) == D3D_OK. Called in D3DX's order (its format table) until a format is chosen.
    std::function<bool(std::uint32_t format)> texture_format_ok;
};

/// D3DXCreateTextureFromFileInMemoryEx arguments (D3DX_DEFAULT = 0xffffffff).
struct LoadArgs {
    std::uint32_t width = 0xffffffff;  ///< pixels; 0 or D3DX_DEFAULT = the file's size (then clamped to the device limits)
    std::uint32_t height = 0xffffffff;  ///< as width
    std::uint32_t mip_levels = 0;  ///< requested level count; 0 = a full chain (clamped to what the size allows)
    std::uint32_t usage = 0;       ///< D3DUSAGE_* flags (D3DX_DEFAULT = 0; only the values 0..3 are handled)
    std::uint32_t format = 0;      ///< requested D3DFORMAT; 0 = D3DFMT_UNKNOWN: the file's format (R8G8B8 becomes X8R8G8B8)
    std::uint32_t pool = 1;        ///< D3DPOOL_* (1 = managed; 0..2 are handled)
    std::uint32_t filter = 0xffffffff;  ///< D3DX_FILTER_* for the image; D3DX_DEFAULT = D3DX's default
    std::uint32_t mip_filter = 0xffffffff;  ///< D3DX_FILTER_* for the mip levels; D3DX_DEFAULT = D3DX's default
    std::uint32_t color_key = 0;   ///< D3DCOLOR treated as transparent; only 0 (none) is handled, plan_texture fails otherwise
};

/// What D3DX creates (CreateTexture arguments) and how it fills it.
struct TexturePlan {
    std::uint32_t width = 0;  ///< texture width, pixels
    std::uint32_t height = 0;  ///< texture height, pixels
    std::uint32_t levels = 0;             ///< mip level count
    std::uint32_t usage = 0;  ///< D3DUSAGE_* the texture is created with
    std::uint32_t format = 0;  ///< D3DFORMAT the texture is created with
    std::uint32_t pool = 0;  ///< D3DPOOL_* the texture is created in
    std::uint32_t filter = 0;  ///< resolved D3DX_FILTER_* of the image
    std::uint32_t mip_filter = 0;  ///< resolved D3DX_FILTER_* of the mip levels
    std::array<std::uint32_t, 256> palette{};  ///< the 256-entry palette D3DX passes to its loads
};

/// Decides the texture the game's D3DX call would create for `image` (size, levels, format, pool, filters) from `args` and the device answers in `device`; false (with the reason in `why` when given) when the texture cannot be created.
bool plan_texture(const Image& image, const LoadArgs& args, const DeviceQuery& device, TexturePlan& plan,
                  std::string* why = nullptr);

/// Arithmetic precision of the texture blits: Single (24-bit significands, as the x87 precision control Direct3D sets) or Double.
enum class Precision { Single, Double };

/// Contents of every level of the planned texture, tightly packed (pitch = row_bytes).
bool build_texture(const Image& image, const TexturePlan& plan, Precision precision, std::vector<ImageLevel>& out,
                   std::string* why = nullptr);

}  // namespace joan::render
