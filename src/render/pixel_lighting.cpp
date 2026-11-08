// [Graphics] PixelLighting - see pixel_lighting.hpp.
#include "pixel_lighting.hpp"

#include "../registry.hpp"

namespace joan::render {

SurfaceParams& surface_params() {
    static SurfaceParams p = [] {
        SurfaceParams r;
        r.pixel_lighting = hook::settings().pixel_lighting != 0;
        return r;
    }();
    return p;
}

}  // namespace joan::render
