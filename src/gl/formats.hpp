// Direct3D 8 surface formats on OpenGL: internal format, upload format and
// type, swizzle, and the memory layout the game sees when it locks a surface
// (identical to Direct3D's, so locked data can be uploaded as is).
#pragma once

#include <cstdint>

#include "../d3d8/d3d8_types.hpp"
#include "glapi.hpp"

namespace joan::gld {

/// How one Direct3D 8 surface format is stored and uploaded in OpenGL; produced by format_info().
struct FormatInfo {
    bool supported = false;  ///< False when the format is not implemented (format_info() default case).
    bool compressed = false;  ///< True for the DXT block-compressed formats (DXT2/DXT3 map to DXT3, DXT4/DXT5 to DXT5).
    std::uint32_t bytes = 0;  ///< per pixel, or per 4x4 block when compressed
    gl::GLenum internal = 0 /**< GL internal format */, format = 0 /**< upload pixel format (0 when compressed) */, type = 0 /**< upload pixel type (0 when compressed) */;
    gl::GLint swizzle[4] = {0x1903, 0x1904, 0x1905, 0x1906};  ///< texture swizzle for R, G, B, A (GL_RED..GL_ALPHA by default): makes formats without a channel read as Direct3D does
};

/// Describes how `f` (a D3DFMT_* value) maps to OpenGL. `supported` is false for formats not handled; the rest of the
/// struct is then unset.
inline FormatInfo format_info(d3d8::Enum f) {
    using namespace gl;
    constexpr GLint R = 0x1903, G = 0x1904, B = 0x1905, ZERO = 0, ONE = 1;
    FormatInfo i;
    i.supported = true;
    auto set = [&](std::uint32_t bytes, GLenum internal, GLenum format, GLenum type) {
        i.bytes = bytes;
        i.internal = internal;
        i.format = format;
        i.type = type;
    };
    auto swz = [&](GLint r, GLint g, GLint b, GLint a) {
        i.swizzle[0] = r;
        i.swizzle[1] = g;
        i.swizzle[2] = b;
        i.swizzle[3] = a;
    };
    switch (f) {
        case d3d8::kFmtA8R8G8B8: set(4, GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE); break;
        case d3d8::kFmtX8R8G8B8: set(4, GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE); swz(R, G, B, ONE); break;
        case d3d8::kFmtR5G6B5: set(2, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5); break;
        case d3d8::kFmtX1R5G5B5: set(2, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV); swz(R, G, B, ONE); break;
        case d3d8::kFmtA1R5G5B5: set(2, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV); break;
        case d3d8::kFmtA4R4G4B4: set(2, GL_RGBA4, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV); break;
        case d3d8::kFmtX4R4G4B4: set(2, GL_RGBA4, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV); swz(R, G, B, ONE); break;
        case d3d8::kFmtA8: set(1, GL_R8, GL_RED, GL_UNSIGNED_BYTE); swz(ZERO, ZERO, ZERO, R); break;
        case d3d8::kFmtL8: set(1, GL_R8, GL_RED, GL_UNSIGNED_BYTE); swz(R, R, R, ONE); break;
        case d3d8::kFmtA8L8: set(2, GL_RG8, GL_RG, GL_UNSIGNED_BYTE); swz(R, R, R, G); break;
        case d3d8::kFmtV8U8: set(2, GL_RG8_SNORM, GL_RG, GL_BYTE); swz(R, G, ONE, ONE); break;
        case d3d8::kFmtDXT1:
            set(8, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0, 0);
            i.compressed = true;
            break;
        case d3d8::kFmtDXT2:
        case d3d8::kFmtDXT3:
            set(16, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0, 0);
            i.compressed = true;
            break;
        case d3d8::kFmtDXT4:
        case d3d8::kFmtDXT5:
            set(16, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0, 0);
            i.compressed = true;
            break;
        default: i.supported = false; break;
    }
    return i;
}

/// Size of mip level `level` along one axis: `size >> level`, never below 1.
inline std::uint32_t level_dim(std::uint32_t size, std::uint32_t level) {
    const std::uint32_t d = size >> level;
    return d ? d : 1;
}

/// Bytes per row as Direct3D reports it in D3DLOCKED_RECT::Pitch (a row of 4x4 blocks when compressed).
inline std::uint32_t row_pitch(const FormatInfo& f, std::uint32_t width) {
    return f.compressed ? ((width + 3) / 4) * f.bytes : width * f.bytes;
}
/// Number of rows of a level `height` pixels high (rows of 4x4 blocks when compressed).
inline std::uint32_t row_count(const FormatInfo& f, std::uint32_t height) {
    return f.compressed ? (height + 3) / 4 : height;
}
/// Bytes of one level of `width` x `height` pixels in Direct3D layout (row_pitch * row_count).
inline std::uint32_t level_size(const FormatInfo& f, std::uint32_t width, std::uint32_t height) {
    return row_pitch(f, width) * row_count(f, height);
}

}  // namespace joan::gld
