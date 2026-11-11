#include "glapi.hpp"

#include <cstring>
#include <type_traits>

namespace joan::gl {

#define JOAN_GL_DEFINE(ret, name, params) name##_fn name = nullptr;
JOAN_GL_FUNCTIONS(JOAN_GL_DEFINE)
JOAN_GL_EXT_FUNCTIONS(JOAN_GL_DEFINE)
JOAN_GL_OPTIONAL_FUNCTIONS(JOAN_GL_DEFINE)
#undef JOAN_GL_DEFINE

const char* load_functions(void* (*get_proc)(const char*)) {
    if (!get_proc) return "the context's loader (SDL_GL_GetProcAddress)";
    // SDL_GL_GetProcAddress: every function, the OpenGL 1.1 ones included.
#define JOAN_GL_LOAD(ret, name, params)                    \
    name = reinterpret_cast<name##_fn>(get_proc(#name));    \
    if (!name) return #name;
    JOAN_GL_FUNCTIONS(JOAN_GL_LOAD)
    JOAN_GL_EXT_FUNCTIONS(JOAN_GL_LOAD)
#undef JOAN_GL_LOAD
#define JOAN_GL_LOAD_OPTIONAL(ret, name, params) name = reinterpret_cast<name##_fn>(get_proc(#name));
    JOAN_GL_OPTIONAL_FUNCTIONS(JOAN_GL_LOAD_OPTIONAL)
#undef JOAN_GL_LOAD_OPTIONAL
    return nullptr;
}

namespace {
// What a GL function returns once the context is gone: nothing, or a zero.
template <class R>
R nothing() {
    if constexpr (!std::is_void_v<R>) return R{};
}
}  // namespace

// The game releases textures and buffers while it shuts down, after the context is gone and SDL may have unloaded the
// driver: the functions point at code that no longer exists. They do nothing from then on.
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
void unload_functions() {
#define JOAN_GL_UNLOAD(ret, name, params) name = [] params -> ret { return nothing<ret>(); };
    JOAN_GL_FUNCTIONS(JOAN_GL_UNLOAD)
    JOAN_GL_EXT_FUNCTIONS(JOAN_GL_UNLOAD)
#undef JOAN_GL_UNLOAD
#define JOAN_GL_UNLOAD_OPTIONAL(ret, name, params) name = nullptr;
    JOAN_GL_OPTIONAL_FUNCTIONS(JOAN_GL_UNLOAD_OPTIONAL)
#undef JOAN_GL_UNLOAD_OPTIONAL
}
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

bool has_bindless_textures() {
    if (!glGetStringi || !glGetTextureSamplerHandleARB || !glMakeTextureHandleResidentARB || !glMakeTextureHandleNonResidentARB)
        return false;
    GLint n = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    for (GLint i = 0; i < n; ++i) {
        const auto* e = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, static_cast<GLuint>(i)));
        if (e && std::strcmp(e, "GL_ARB_bindless_texture") == 0) return true;
    }
    return false;
}

}  // namespace joan::gl
