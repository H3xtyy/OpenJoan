// Minimal OpenGL 3.3 function loader (no third-party loader needed).
//
// Only the entry points the renderer actually uses are listed; add more to
// JOAN_GL_FUNCTIONS as the renderer grows. The original game rendered through
// Direct3D 8 (d3d8.dll + v3d_87.dll/v3d_sse.dll helper DLLs); this backend
// replaces that.
#pragma once

#include <cstddef>
#include <cstdint>

#if defined(_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall
#elif !defined(APIENTRY)
#define APIENTRY
#endif

namespace joan::gl {

using GLenum = unsigned int;
using GLbitfield = unsigned int;
using GLuint = unsigned int;
using GLint = int;
using GLsizei = int;
using GLfloat = float;
using GLboolean = unsigned char;
using GLchar = char;
using GLubyte = unsigned char;

using GLsizeiptr = std::ptrdiff_t;
using GLintptr = std::ptrdiff_t;

inline constexpr GLenum COLOR_BUFFER_BIT = 0x00004000;
inline constexpr GLenum DEPTH_BUFFER_BIT = 0x00000100;
inline constexpr GLenum VERSION = 0x1F02;
inline constexpr GLenum RENDERER = 0x1F01;
inline constexpr GLenum FALSE_ = 0;
inline constexpr GLenum TRUE_ = 1;
inline constexpr GLenum POINTS = 0x0000;
inline constexpr GLenum LINES = 0x0001;
inline constexpr GLenum TRIANGLES = 0x0004;
inline constexpr GLenum DEPTH_TEST = 0x0B71;
inline constexpr GLenum CULL_FACE = 0x0B44;
inline constexpr GLenum BLEND = 0x0BE2;
inline constexpr GLenum LEQUAL = 0x0203;
inline constexpr GLenum LESS = 0x0201;
inline constexpr GLenum SRC_ALPHA = 0x0302;
inline constexpr GLenum ONE_MINUS_SRC_ALPHA = 0x0303;
inline constexpr GLenum FRONT_AND_BACK = 0x0408;
inline constexpr GLenum LINE = 0x1B01;
inline constexpr GLenum FILL = 0x1B02;
inline constexpr GLenum UNSIGNED_BYTE = 0x1401;
inline constexpr GLenum UNSIGNED_SHORT = 0x1403;
inline constexpr GLenum FLOAT = 0x1406;
inline constexpr GLenum RGBA = 0x1908;
inline constexpr GLenum RGBA8 = 0x8058;
inline constexpr GLenum TEXTURE_2D = 0x0DE1;
inline constexpr GLenum TEXTURE0 = 0x84C0;
inline constexpr GLenum TEXTURE_MAG_FILTER = 0x2800;
inline constexpr GLenum TEXTURE_MIN_FILTER = 0x2801;
inline constexpr GLenum TEXTURE_WRAP_S = 0x2802;
inline constexpr GLenum TEXTURE_WRAP_T = 0x2803;
inline constexpr GLenum LINEAR = 0x2601;
inline constexpr GLenum LINEAR_MIPMAP_LINEAR = 0x2703;
inline constexpr GLenum REPEAT = 0x2901;
inline constexpr GLenum UNPACK_ALIGNMENT = 0x0CF5;
inline constexpr GLenum ARRAY_BUFFER = 0x8892;
inline constexpr GLenum ELEMENT_ARRAY_BUFFER = 0x8893;
inline constexpr GLenum STATIC_DRAW = 0x88E4;
inline constexpr GLenum DYNAMIC_DRAW = 0x88E8;
inline constexpr GLenum FRAGMENT_SHADER = 0x8B30;
inline constexpr GLenum VERTEX_SHADER = 0x8B31;
inline constexpr GLenum COMPILE_STATUS = 0x8B81;
inline constexpr GLenum LINK_STATUS = 0x8B82;
inline constexpr GLenum INFO_LOG_LENGTH = 0x8B84;

// X(return type, name, parameter list)
#define JOAN_GL_FUNCTIONS(X)                                                                          \
    X(void, Viewport, (GLint x, GLint y, GLsizei w, GLsizei h))                                       \
    X(void, ClearColor, (GLfloat r, GLfloat g, GLfloat b, GLfloat a))                                 \
    X(void, Clear, (GLbitfield mask))                                                                 \
    X(const GLubyte*, GetString, (GLenum name))                                                       \
    X(void, Enable, (GLenum cap))                                                                     \
    X(void, Disable, (GLenum cap))                                                                    \
    X(void, DepthFunc, (GLenum func))                                                                 \
    X(void, DepthMask, (GLboolean flag))                                                              \
    X(void, BlendFunc, (GLenum sfactor, GLenum dfactor))                                              \
    X(void, PolygonMode, (GLenum face, GLenum mode))                                                  \
    X(void, PixelStorei, (GLenum pname, GLint param))                                                 \
    X(GLuint, CreateShader, (GLenum type))                                                            \
    X(void, ShaderSource, (GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length)) \
    X(void, CompileShader, (GLuint shader))                                                           \
    X(void, GetShaderiv, (GLuint shader, GLenum pname, GLint* params))                                \
    X(void, GetShaderInfoLog, (GLuint shader, GLsizei max, GLsizei* length, GLchar* log))             \
    X(void, DeleteShader, (GLuint shader))                                                            \
    X(GLuint, CreateProgram, ())                                                                      \
    X(void, AttachShader, (GLuint program, GLuint shader))                                            \
    X(void, BindAttribLocation, (GLuint program, GLuint index, const GLchar* name))                   \
    X(void, LinkProgram, (GLuint program))                                                            \
    X(void, GetProgramiv, (GLuint program, GLenum pname, GLint* params))                              \
    X(void, GetProgramInfoLog, (GLuint program, GLsizei max, GLsizei* length, GLchar* log))           \
    X(void, DeleteProgram, (GLuint program))                                                          \
    X(void, UseProgram, (GLuint program))                                                             \
    X(GLint, GetUniformLocation, (GLuint program, const GLchar* name))                                \
    X(void, UniformMatrix4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* v)) \
    X(void, Uniform4fv, (GLint location, GLsizei count, const GLfloat* v))                            \
    X(void, Uniform3fv, (GLint location, GLsizei count, const GLfloat* v))                            \
    X(void, Uniform1i, (GLint location, GLint v))                                                     \
    X(void, Uniform1f, (GLint location, GLfloat v))                                                   \
    X(void, GenVertexArrays, (GLsizei n, GLuint* arrays))                                             \
    X(void, BindVertexArray, (GLuint array))                                                          \
    X(void, DeleteVertexArrays, (GLsizei n, const GLuint* arrays))                                    \
    X(void, GenBuffers, (GLsizei n, GLuint* buffers))                                                 \
    X(void, BindBuffer, (GLenum target, GLuint buffer))                                               \
    X(void, BufferData, (GLenum target, GLsizeiptr size, const void* data, GLenum usage))             \
    X(void, BufferSubData, (GLenum target, GLintptr offset, GLsizeiptr size, const void* data))       \
    X(void, DeleteBuffers, (GLsizei n, const GLuint* buffers))                                        \
    X(void, VertexAttribPointer, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)) \
    X(void, EnableVertexAttribArray, (GLuint index))                                                  \
    X(void, VertexAttrib3f, (GLuint index, GLfloat x, GLfloat y, GLfloat z))                          \
    X(void, DrawElements, (GLenum mode, GLsizei count, GLenum type, const void* indices))             \
    X(void, DrawArrays, (GLenum mode, GLint first, GLsizei count))                                    \
    X(void, GenTextures, (GLsizei n, GLuint* textures))                                               \
    X(void, BindTexture, (GLenum target, GLuint texture))                                             \
    X(void, DeleteTextures, (GLsizei n, const GLuint* textures))                                      \
    X(void, ActiveTexture, (GLenum texture))                                                          \
    X(void, TexImage2D, (GLenum target, GLint level, GLint internalformat, GLsizei w, GLsizei h, GLint border, GLenum format, GLenum type, const void* pixels)) \
    X(void, TexParameteri, (GLenum target, GLenum pname, GLint param))                                \
    X(void, GenerateMipmap, (GLenum target))

#define JOAN_GL_DECLARE(ret, name, params) \
    using PFN_##name = ret(APIENTRY*) params; \
    extern PFN_##name name;
JOAN_GL_FUNCTIONS(JOAN_GL_DECLARE)
#undef JOAN_GL_DECLARE

using GetProcAddressFn = void* (*)(const char* name);

/// Loads all functions; returns false and reports the first missing one.
bool load(GetProcAddressFn get_proc, const char** missing = nullptr);

}  // namespace joan::gl
