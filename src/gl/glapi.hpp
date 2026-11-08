// Minimal OpenGL 4.6 core loader for the hook (no system GL headers: the
// functions live in namespace joan::gl so they cannot clash with opengl32's
// exported 1.1 symbols). Functions are loaded with SDL_GL_GetProcAddress on the
// SDL window. On 32-bit Windows every GL entry point is __stdcall (APIENTRY): JOAN_GL_APIENTRY.
#pragma once
#include "../callconv.hpp"

#include <cstddef>
#include <cstdint>

#if defined(_WIN32) && !defined(_WIN64)
#define JOAN_GL_APIENTRY JOAN_STDCALL
#else
#define JOAN_GL_APIENTRY
#endif

namespace joan::gl {

using GLenum = unsigned int;  /**< OpenGL type GLenum. */
using GLboolean = unsigned char;  /**< OpenGL type GLboolean. */
using GLbitfield = unsigned int;  /**< OpenGL type GLbitfield. */
using GLint = int;  /**< OpenGL type GLint. */
using GLuint = unsigned int;  /**< OpenGL type GLuint. */
using GLsizei = int;  /**< OpenGL type GLsizei. */
using GLfloat = float;  /**< OpenGL type GLfloat. */
using GLdouble = double;  /**< OpenGL type GLdouble. */
using GLchar = char;  /**< OpenGL type GLchar. */
using GLubyte = unsigned char;  /**< OpenGL type GLubyte. */
using GLintptr = std::ptrdiff_t;  /**< OpenGL type GLintptr. */
using GLsizeiptr = std::ptrdiff_t;  /**< OpenGL type GLsizeiptr. */
using GLuint64 = std::uint64_t;  /**< OpenGL type GLuint64. */
using GLsync = struct GLsyncObject*;  /**< OpenGL type GLsync. */
/// Signature of a debug message callback (glDebugMessageCallback): source, type, id, severity, message length, text and the user pointer.
using GLDebugProc = void(JOAN_GL_APIENTRY*)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                     const GLchar* message, const void* user);

// clang-format off
#define JOAN_GL_FUNCTIONS(X) \
    X(void, glEnable, (GLenum cap)) \
    X(void, glDisable, (GLenum cap)) \
    X(GLenum, glGetError, ()) \
    X(void, glGetIntegerv, (GLenum pname, GLint* data)) \
    X(const GLubyte*, glGetString, (GLenum name)) \
    X(void, glViewport, (GLint x, GLint y, GLsizei w, GLsizei h)) \
    X(void, glScissor, (GLint x, GLint y, GLsizei w, GLsizei h)) \
    X(void, glColorMask, (GLboolean r, GLboolean g, GLboolean b, GLboolean a)) \
    X(void, glDepthMask, (GLboolean flag)) \
    X(void, glDepthFunc, (GLenum func)) \
    X(void, glBlendFunc, (GLenum sfactor, GLenum dfactor)) \
    X(void, glCullFace, (GLenum mode)) \
    X(void, glFrontFace, (GLenum mode)) \
    X(void, glPolygonMode, (GLenum face, GLenum mode)) \
    X(void, glPolygonOffset, (GLfloat factor, GLfloat units)) \
    X(void, glStencilFunc, (GLenum func, GLint ref, GLuint mask)) \
    X(void, glStencilOp, (GLenum sfail, GLenum dpfail, GLenum dppass)) \
    X(void, glStencilMask, (GLuint mask)) \
    X(void, glDrawArrays, (GLenum mode, GLint first, GLsizei count)) \
    X(void, glPixelStorei, (GLenum pname, GLint param)) \
    X(void, glReadPixels, (GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLenum type, void* pixels)) \
    X(void, glFlush, ()) \
    X(void, glPointSize, (GLfloat size))
#define JOAN_GL_EXT_FUNCTIONS(X) \
    X(void, glClipControl, (GLenum origin, GLenum depth)) \
    X(void, glDepthRangef, (GLfloat n, GLfloat f)) \
    X(void, glBlendEquation, (GLenum mode)) \
    X(void, glBlendEquationi, (GLuint buf, GLenum mode)) \
    X(void, glBlendFunci, (GLuint buf, GLenum src, GLenum dst)) \
    X(void, glEnablei, (GLenum cap, GLuint index)) \
    X(void, glColorMaski, (GLuint buf, GLboolean r, GLboolean g, GLboolean b, GLboolean a)) \
    X(void, glBlendFuncSeparate, (GLenum src_rgb, GLenum dst_rgb, GLenum src_alpha, GLenum dst_alpha)) \
    X(void, glStencilOpSeparate, (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)) \
    X(void, glCreateBuffers, (GLsizei n, GLuint* buffers)) \
    X(void, glDeleteBuffers, (GLsizei n, const GLuint* buffers)) \
    X(void, glNamedBufferData, (GLuint buffer, GLsizeiptr size, const void* data, GLenum usage)) \
    X(void, glNamedBufferSubData, (GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data)) \
    X(void, glGetNamedBufferSubData, (GLuint buffer, GLintptr offset, GLsizeiptr size, void* data)) \
    X(void, glBindBufferBase, (GLenum target, GLuint index, GLuint buffer)) \
    X(void, glBindBufferRange, (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)) \
    X(void, glNamedBufferStorage, (GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags)) \
    X(void*, glMapNamedBufferRange, (GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access)) \
    X(GLboolean, glUnmapNamedBuffer, (GLuint buffer)) \
    X(GLsync, glFenceSync, (GLenum condition, GLbitfield flags)) \
    X(GLenum, glClientWaitSync, (GLsync sync, GLbitfield flags, GLuint64 timeout)) \
    X(void, glDeleteSync, (GLsync sync)) \
    X(void, glCreateVertexArrays, (GLsizei n, GLuint* arrays)) \
    X(void, glDeleteVertexArrays, (GLsizei n, const GLuint* arrays)) \
    X(void, glBindVertexArray, (GLuint array)) \
    X(void, glVertexArrayVertexBuffer, (GLuint vao, GLuint binding, GLuint buffer, GLintptr offset, GLsizei stride)) \
    X(void, glVertexArrayElementBuffer, (GLuint vao, GLuint buffer)) \
    X(void, glEnableVertexArrayAttrib, (GLuint vao, GLuint index)) \
    X(void, glVertexArrayAttribFormat, (GLuint vao, GLuint index, GLint size, GLenum type, GLboolean normalized, GLuint offset)) \
    X(void, glVertexArrayAttribBinding, (GLuint vao, GLuint index, GLuint binding)) \
    X(void, glDrawElementsBaseVertex, (GLenum mode, GLsizei count, GLenum type, const void* indices, GLint basevertex)) \
    X(void, glDrawElementsInstancedBaseVertexBaseInstance, (GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance)) \
    X(void, glDrawArraysInstancedBaseInstance, (GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance)) \
    X(void, glMultiDrawElementsIndirect, (GLenum mode, GLenum type, const void* indirect, GLsizei drawcount, GLsizei stride)) \
    X(void, glCreateTextures, (GLenum target, GLsizei n, GLuint* textures)) \
    X(void, glDeleteTextures, (GLsizei n, const GLuint* textures)) \
    X(void, glTextureStorage2D, (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei w, GLsizei h)) \
    X(void, glTextureStorage2DMultisample, (GLuint texture, GLsizei samples, GLenum internalformat, GLsizei w, GLsizei h, GLboolean fixed_locations)) \
    X(void, glTextureSubImage2D, (GLuint texture, GLint level, GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLenum type, const void* pixels)) \
    X(void, glCompressedTextureSubImage2D, (GLuint texture, GLint level, GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLsizei size, const void* data)) \
    X(void, glTextureParameteri, (GLuint texture, GLenum pname, GLint param)) \
    X(void, glGenerateTextureMipmap, (GLuint texture)) \
    X(void, glCopyImageSubData, (GLuint src, GLenum src_target, GLint src_level, GLint src_x, GLint src_y, GLint src_z, GLuint dst, GLenum dst_target, GLint dst_level, GLint dst_x, GLint dst_y, GLint dst_z, GLsizei w, GLsizei h, GLsizei d)) \
    X(void, glTextureParameteriv, (GLuint texture, GLenum pname, const GLint* params)) \
    X(void, glBindTextureUnit, (GLuint unit, GLuint texture)) \
    X(void, glGetTextureSubImage, (GLuint texture, GLint level, GLint x, GLint y, GLint z, GLsizei w, GLsizei h, GLsizei d, GLenum format, GLenum type, GLsizei bufsize, void* pixels)) \
    X(void, glGetCompressedTextureSubImage, (GLuint texture, GLint level, GLint x, GLint y, GLint z, GLsizei w, GLsizei h, GLsizei d, GLsizei bufsize, void* pixels)) \
    X(void, glCreateSamplers, (GLsizei n, GLuint* samplers)) \
    X(void, glDeleteSamplers, (GLsizei n, const GLuint* samplers)) \
    X(void, glSamplerParameteri, (GLuint sampler, GLenum pname, GLint param)) \
    X(void, glBindSampler, (GLuint unit, GLuint sampler)) \
    X(void, glCreateFramebuffers, (GLsizei n, GLuint* framebuffers)) \
    X(void, glDeleteFramebuffers, (GLsizei n, const GLuint* framebuffers)) \
    X(void, glNamedFramebufferTexture, (GLuint fb, GLenum attachment, GLuint texture, GLint level)) \
    X(void, glNamedFramebufferRenderbuffer, (GLuint fb, GLenum attachment, GLenum rbtarget, GLuint rb)) \
    X(void, glNamedFramebufferDrawBuffer, (GLuint fb, GLenum buf)) \
    X(void, glNamedFramebufferDrawBuffers, (GLuint fb, GLsizei n, const GLenum* bufs)) \
    X(void, glNamedFramebufferReadBuffer, (GLuint fb, GLenum src)) \
    X(GLenum, glCheckNamedFramebufferStatus, (GLuint fb, GLenum target)) \
    X(void, glBindFramebuffer, (GLenum target, GLuint fb)) \
    X(void, glBlitNamedFramebuffer, (GLuint read, GLuint draw, GLint sx0, GLint sy0, GLint sx1, GLint sy1, GLint dx0, GLint dy0, GLint dx1, GLint dy1, GLbitfield mask, GLenum filter)) \
    X(void, glClearNamedFramebufferfv, (GLuint fb, GLenum buffer, GLint drawbuffer, const GLfloat* value)) \
    X(void, glClearNamedFramebufferfi, (GLuint fb, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)) \
    X(void, glClearNamedFramebufferiv, (GLuint fb, GLenum buffer, GLint drawbuffer, const GLint* value)) \
    X(void, glCreateRenderbuffers, (GLsizei n, GLuint* rbs)) \
    X(void, glDeleteRenderbuffers, (GLsizei n, const GLuint* rbs)) \
    X(void, glNamedRenderbufferStorage, (GLuint rb, GLenum internalformat, GLsizei w, GLsizei h)) \
    X(void, glNamedRenderbufferStorageMultisample, (GLuint rb, GLsizei samples, GLenum internalformat, GLsizei w, GLsizei h)) \
    X(GLuint, glCreateShader, (GLenum type)) \
    X(void, glShaderSource, (GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length)) \
    X(void, glCompileShader, (GLuint shader)) \
    X(void, glGetShaderiv, (GLuint shader, GLenum pname, GLint* params)) \
    X(void, glGetShaderInfoLog, (GLuint shader, GLsizei max, GLsizei* length, GLchar* log)) \
    X(void, glDeleteShader, (GLuint shader)) \
    X(GLuint, glCreateProgram, ()) \
    X(void, glAttachShader, (GLuint program, GLuint shader)) \
    X(void, glLinkProgram, (GLuint program)) \
    X(void, glGetProgramiv, (GLuint program, GLenum pname, GLint* params)) \
    X(void, glGetProgramInfoLog, (GLuint program, GLsizei max, GLsizei* length, GLchar* log)) \
    X(void, glDeleteProgram, (GLuint program)) \
    X(void, glUseProgram, (GLuint program)) \
    X(GLint, glGetUniformLocation, (GLuint program, const GLchar* name)) \
    X(void, glProgramUniform1i, (GLuint program, GLint location, GLint v0)) \
    X(void, glProgramUniform2i, (GLuint program, GLint location, GLint v0, GLint v1)) \
    X(void, glProgramUniform4i, (GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3)) \
    X(void, glBindBuffer, (GLenum target, GLuint buffer)) \
    X(void, glProgramUniform1fv, (GLuint program, GLint location, GLsizei count, const GLfloat* value)) \
    X(void, glProgramUniform2fv, (GLuint program, GLint location, GLsizei count, const GLfloat* value)) \
    X(void, glProgramUniform3fv, (GLuint program, GLint location, GLsizei count, const GLfloat* value)) \
    X(void, glProgramUniform4fv, (GLuint program, GLint location, GLsizei count, const GLfloat* value)) \
    X(void, glProgramUniformMatrix4fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)) \
    X(GLuint, glGetUniformBlockIndex, (GLuint program, const GLchar* name)) \
    X(void, glUniformBlockBinding, (GLuint program, GLuint index, GLuint binding)) \
    X(void, glDebugMessageCallback, (GLDebugProc callback, const void* user))
// Optional: null when the driver lacks them (load_functions does not fail; check before use).
#define JOAN_GL_OPTIONAL_FUNCTIONS(X) \
    X(const GLubyte*, glGetStringi, (GLenum name, GLuint index)) \
    X(GLuint64, glGetTextureSamplerHandleARB, (GLuint texture, GLuint sampler)) \
    X(void, glMakeTextureHandleResidentARB, (GLuint64 handle)) \
    X(void, glMakeTextureHandleNonResidentARB, (GLuint64 handle))
// clang-format on

#define JOAN_GL_DECLARE(ret, name, params) using name##_fn = ret(JOAN_GL_APIENTRY*) params; extern name##_fn name;
JOAN_GL_FUNCTIONS(JOAN_GL_DECLARE)
JOAN_GL_EXT_FUNCTIONS(JOAN_GL_DECLARE)
JOAN_GL_OPTIONAL_FUNCTIONS(JOAN_GL_DECLARE)
#undef JOAN_GL_DECLARE
/// GL_ARB_bindless_texture is listed by the driver and its functions loaded (needs a current context).
bool has_bindless_textures();

/// Loads every function; returns the name of the first missing one, or nullptr.
/// Needs a current context. `get_proc`: the context's loader (SDL_GL_GetProcAddress).
const char* load_functions(void* (*get_proc)(const char*));

// --- constants ------------------------------------------------------------------
enum : GLenum {
    GL_FALSE_ = 0, GL_TRUE_ = 1,
    GL_POINTS = 0x0000, GL_LINES = 0x0001, GL_LINE_STRIP = 0x0003, GL_TRIANGLES = 0x0004, GL_TRIANGLE_STRIP = 0x0005,
    GL_TRIANGLE_FAN = 0x0006,
    GL_NEVER = 0x0200, GL_LESS = 0x0201, GL_EQUAL = 0x0202, GL_LEQUAL = 0x0203, GL_GREATER = 0x0204,
    GL_NOTEQUAL = 0x0205, GL_GEQUAL = 0x0206, GL_ALWAYS = 0x0207,
    GL_ZERO = 0, GL_ONE = 1, GL_SRC_COLOR = 0x0300, GL_ONE_MINUS_SRC_COLOR = 0x0301, GL_SRC_ALPHA = 0x0302,
    GL_ONE_MINUS_SRC_ALPHA = 0x0303, GL_DST_ALPHA = 0x0304, GL_ONE_MINUS_DST_ALPHA = 0x0305, GL_DST_COLOR = 0x0306,
    GL_ONE_MINUS_DST_COLOR = 0x0307, GL_SRC_ALPHA_SATURATE = 0x0308,
    GL_FRONT = 0x0404, GL_BACK = 0x0405, GL_FRONT_AND_BACK = 0x0408, GL_CW = 0x0900, GL_CCW = 0x0901,
    GL_CULL_FACE = 0x0B44, GL_DEPTH_TEST = 0x0B71, GL_STENCIL_TEST = 0x0B90, GL_BLEND = 0x0BE2,
    GL_SCISSOR_TEST = 0x0C11, GL_POLYGON_OFFSET_FILL = 0x8037, GL_PROGRAM_POINT_SIZE = 0x8642,
    GL_DEBUG_OUTPUT = 0x92E0, GL_DEBUG_OUTPUT_SYNCHRONOUS = 0x8242, GL_FRAMEBUFFER_SRGB = 0x8DB9,
    GL_POINT = 0x1B00, GL_LINE = 0x1B01, GL_FILL = 0x1B02,
    GL_KEEP = 0x1E00, GL_REPLACE = 0x1E01, GL_INCR = 0x1E02, GL_DECR = 0x1E03, GL_INVERT = 0x150A,
    GL_INCR_WRAP = 0x8507, GL_DECR_WRAP = 0x8508,
    GL_FUNC_ADD = 0x8006, GL_FUNC_SUBTRACT = 0x800A, GL_FUNC_REVERSE_SUBTRACT = 0x800B, GL_MIN = 0x8007, GL_MAX = 0x8008,
    GL_BYTE = 0x1400, GL_UNSIGNED_BYTE = 0x1401, GL_SHORT = 0x1402, GL_UNSIGNED_SHORT = 0x1403, GL_INT = 0x1404,
    GL_UNSIGNED_INT = 0x1405, GL_FLOAT = 0x1406,
    GL_UNSIGNED_SHORT_5_6_5 = 0x8363, GL_UNSIGNED_SHORT_4_4_4_4_REV = 0x8365, GL_UNSIGNED_SHORT_1_5_5_5_REV = 0x8366,
    GL_UNSIGNED_INT_8_8_8_8_REV = 0x8367,
    GL_RED = 0x1903, GL_ALPHA_ = 0x1906, GL_RGB = 0x1907, GL_RGBA = 0x1908, GL_RG = 0x8227, GL_BGRA = 0x80E1,
    GL_NONE_ = 0, GL_R32F = 0x822E, GL_RG32F = 0x8230,
    GL_R8 = 0x8229, GL_RG8 = 0x822B, GL_RG8_SNORM = 0x8F95, GL_RGB8 = 0x8051, GL_RGBA8 = 0x8058, GL_RGBA16F = 0x881A,
    GL_RGB565 = 0x8D62, GL_RGB5_A1 = 0x8057, GL_RGBA4 = 0x8056, GL_DEPTH24_STENCIL8 = 0x88F0,
    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT = 0x83F1, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT = 0x83F2,
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT = 0x83F3,
    GL_TEXTURE_2D = 0x0DE1, GL_TEXTURE_2D_MULTISAMPLE = 0x9100, GL_TEXTURE_MAG_FILTER = 0x2800, GL_TEXTURE_MIN_FILTER = 0x2801,
    GL_TEXTURE_WRAP_S = 0x2802, GL_TEXTURE_WRAP_T = 0x2803, GL_TEXTURE_MAX_LEVEL = 0x813D,
    GL_TEXTURE_SWIZZLE_RGBA = 0x8E46, GL_TEXTURE_LOD_BIAS = 0x8501, GL_TEXTURE_MAX_ANISOTROPY = 0x84FE,
    GL_NEAREST = 0x2600, GL_LINEAR = 0x2601, GL_NEAREST_MIPMAP_NEAREST = 0x2700, GL_LINEAR_MIPMAP_NEAREST = 0x2701,
    GL_NEAREST_MIPMAP_LINEAR = 0x2702, GL_LINEAR_MIPMAP_LINEAR = 0x2703,
    GL_REPEAT = 0x2901, GL_CLAMP_TO_EDGE = 0x812F, GL_MIRRORED_REPEAT = 0x8370, GL_CLAMP_TO_BORDER = 0x812D,
    GL_TEXTURE_BORDER_COLOR = 0x1004,
    GL_ARRAY_BUFFER = 0x8892, GL_ELEMENT_ARRAY_BUFFER = 0x8893, GL_UNIFORM_BUFFER = 0x8A11, GL_SHADER_STORAGE_BUFFER = 0x90D2, GL_MAX_SHADER_STORAGE_BLOCK_SIZE = 0x90DE, GL_PIXEL_PACK_BUFFER = 0x88EB, GL_STREAM_READ = 0x88E1, GL_DRAW_INDIRECT_BUFFER = 0x8F3F,
    GL_STATIC_DRAW = 0x88E4, GL_DYNAMIC_DRAW = 0x88E8, GL_STREAM_DRAW = 0x88E0,
    GL_FRAMEBUFFER = 0x8D40, GL_READ_FRAMEBUFFER = 0x8CA8, GL_DRAW_FRAMEBUFFER = 0x8CA9,
    GL_RENDERBUFFER = 0x8D41, GL_COLOR_ATTACHMENT0 = 0x8CE0, GL_COLOR_ATTACHMENT1 = 0x8CE1, GL_DEPTH_STENCIL_ATTACHMENT = 0x821A,
    GL_FRAMEBUFFER_COMPLETE = 0x8CD5, GL_COLOR_BUFFER_BIT = 0x4000, GL_DEPTH_BUFFER_BIT = 0x0100,
    GL_STENCIL_BUFFER_BIT = 0x0400, GL_COLOR = 0x1800, GL_DEPTH_STENCIL = 0x84F9, GL_BACK_LEFT = 0x0402,
    GL_VERTEX_SHADER = 0x8B31, GL_FRAGMENT_SHADER = 0x8B30, GL_COMPILE_STATUS = 0x8B81, GL_LINK_STATUS = 0x8B82,
    GL_INFO_LOG_LENGTH = 0x8B84, GL_INVALID_INDEX_ = 0xFFFFFFFFu,
    GL_LOWER_LEFT = 0x8CA1, GL_UPPER_LEFT = 0x8CA2, GL_ZERO_TO_ONE = 0x935F,
    GL_PACK_ALIGNMENT = 0x0D05, GL_UNPACK_ALIGNMENT = 0x0CF5, GL_UNPACK_ROW_LENGTH = 0x0CF2,
    GL_VENDOR = 0x1F00, GL_RENDERER = 0x1F01, GL_VERSION = 0x1F02, GL_EXTENSIONS = 0x1F03, GL_NUM_EXTENSIONS = 0x821D,
    GL_DEBUG_SEVERITY_HIGH = 0x9146, GL_DEBUG_SEVERITY_MEDIUM = 0x9147, GL_DEBUG_SEVERITY_LOW = 0x9148,
    GL_DEBUG_SEVERITY_NOTIFICATION = 0x826B, GL_DEBUG_TYPE_ERROR = 0x824C,
    GL_MAX_SAMPLES = 0x8D57, GL_SAMPLE_BUFFERS = 0x80A8, GL_READ_FRAMEBUFFER_BINDING = 0x8CAA,
    GL_MAX_TEXTURE_MAX_ANISOTROPY = 0x84FF,
    GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0x8A34,
    GL_DEPTH_COMPONENT24 = 0x81A6, GL_DEPTH_COMPONENT32F = 0x8CAC, GL_DEPTH_ATTACHMENT = 0x8D00, GL_DEPTH_BUFFER = 0x1801, GL_STENCIL = 0x1802,
    GL_TEXTURE_COMPARE_MODE = 0x884C, GL_TEXTURE_COMPARE_FUNC = 0x884D, GL_COMPARE_REF_TO_TEXTURE = 0x884E,
    GL_MAP_WRITE_BIT = 0x0002, GL_MAP_PERSISTENT_BIT = 0x0040, GL_MAP_COHERENT_BIT = 0x0080, GL_DYNAMIC_STORAGE_BIT = 0x0100,
    GL_MAP_INVALIDATE_RANGE_BIT = 0x0004, GL_MAP_INVALIDATE_BUFFER_BIT = 0x0008, GL_MAP_UNSYNCHRONIZED_BIT = 0x0020,
    GL_SYNC_GPU_COMMANDS_COMPLETE = 0x9117, GL_SYNC_FLUSH_COMMANDS_BIT = 0x0001,
    GL_ALREADY_SIGNALED = 0x911A, GL_TIMEOUT_EXPIRED = 0x911B, GL_CONDITION_SATISFIED = 0x911C, GL_WAIT_FAILED = 0x911D,
};

}  // namespace joan::gl
