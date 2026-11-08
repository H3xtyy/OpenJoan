#include "render/gl.hpp"

namespace joan::gl {

#define JOAN_GL_DEFINE(ret, name, params) PFN_##name name = nullptr;
JOAN_GL_FUNCTIONS(JOAN_GL_DEFINE)
#undef JOAN_GL_DEFINE

bool load(GetProcAddressFn get_proc, const char** missing) {
#define JOAN_GL_LOAD(ret, name, params)                                   \
    name = reinterpret_cast<PFN_##name>(get_proc("gl" #name));            \
    if (!name) {                                                          \
        if (missing) *missing = "gl" #name;                               \
        return false;                                                     \
    }
    JOAN_GL_FUNCTIONS(JOAN_GL_LOAD)
#undef JOAN_GL_LOAD
    return true;
}

}  // namespace joan::gl
