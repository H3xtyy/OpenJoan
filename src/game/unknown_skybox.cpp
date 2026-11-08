// game/unknown_skybox.cpp: functions whose original source file is unknown (formerly game/_unplaced/skybox.cpp;
// re/attribution.csv: between ww/en3d/skybox.cpp and ww/en3d/sys3d.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../canvas_api.hpp"
#include "../render/overrides.hpp"
#include "../d3d8/d3d8_types.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "stream_cache_api.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

}  // namespace

// ---------------------------------------- from modules/en3d_helpers31.cpp (en3d/helpers31)
namespace {
namespace en3d_helpers31 {

// Sky vertex: position, colour, 3 texture floats (0x1c bytes).
struct SkyVertex {
    float x, y, z;
    std::uint8_t color[4];
    float u, v, w;
};
static_assert(sizeof(SkyVertex) == 0x1c);

SkyVertex sky_vertex(float x, float y, float z, float u, float v, float w) {
    SkyVertex out;
    out.x = x;
    out.y = y;
    out.z = z;
    out.color[0] = 0xff;
    out.color[1] = 0xff;
    out.color[2] = 0xff;
    out.color[3] = 0xff;
    out.u = u;
    out.v = v;
    out.w = w;
    return out;
}

JOAN_COUNTER(c_sky_build, "SkyBox::build");

struct H {
    // 0x005f5240 SkyBox::build(size, first material, face mask). ret 0xc.
    void sky_build(float size, std::int32_t first, std::int32_t mask_arg) {
        c_sky_build.hit();
        jo::self<jo::SkyBox>(this)->size = size;
        const std::int8_t mask = static_cast<std::int8_t>(mask_arg);
        if (static_cast<std::int8_t>(jo::self<jo::SkyBox>(this)->cube) == 0) {
            joan::orig::as_plane(this)->set_format(3);  // 0x005cdc70
            const float s = size;
            const float n = -size;
            // (The original also counts the mask bits into a local it never reads.)
            SkyVertex faces[6][4] = {
                {sky_vertex(s, n, s, 0, 0, 1), sky_vertex(s, n, n, 0, 1, 1), sky_vertex(n, n, s, 1, 0, 1), sky_vertex(n, n, n, 1, 1, 1)},
                {sky_vertex(s, s, s, 0, 0, 1), sky_vertex(s, s, n, 0, 1, 1), sky_vertex(s, n, s, 1, 0, 1), sky_vertex(s, n, n, 1, 1, 1)},
                {sky_vertex(n, s, s, 0, 0, 1), sky_vertex(n, s, n, 0, 1, 1), sky_vertex(s, s, s, 1, 0, 1), sky_vertex(s, s, n, 1, 1, 1)},
                {sky_vertex(n, n, s, 0, 0, 1), sky_vertex(n, n, n, 0, 1, 1), sky_vertex(n, s, s, 1, 0, 1), sky_vertex(n, s, n, 1, 1, 1)},
                {sky_vertex(n, s, s, 0, 0, 1), sky_vertex(s, s, s, 0, 1, 1), sky_vertex(n, n, s, 1, 0, 1), sky_vertex(s, n, s, 1, 1, 1)},
                {sky_vertex(s, s, n, 0, 0, 1), sky_vertex(n, s, n, 0, 1, 1), sky_vertex(s, n, n, 1, 0, 1), sky_vertex(n, n, n, 1, 1, 1)},
            };
            std::int8_t material = 0;
            for (std::int8_t i = 0; i < 6; i = static_cast<std::int8_t>(i + 1)) {
                if (((static_cast<std::int32_t>(mask) >> i) & 1) == 0) continue;
                const std::int32_t v = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_vertices(4));  // 0x005cd7f0 add vertices
                const std::int32_t t = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_triangles(2));  // 0x005ce6f0 add triangles
                std::memcpy(jo::view<std::uint8_t*>(jo::self<jo::SkyBox>(this)->vertices_f3) + v * 0x1c, faces[i], 0x70);
                std::uint16_t* const idx = jo::view<std::uint16_t*>(jo::self<jo::SkyBox>(this)->triangles) + t * 3;
                joan::stream_cache_api::indices_written(jo::view<void*>(jo::self<jo::SkyBox>(this)->triangles));
                idx[0] = static_cast<std::uint16_t>(v);
                idx[1] = static_cast<std::uint16_t>(v + 1);
                idx[2] = static_cast<std::uint16_t>(v + 2);
                idx[3] = static_cast<std::uint16_t>(v + 2);
                idx[4] = static_cast<std::uint16_t>(v + 1);
                idx[5] = static_cast<std::uint16_t>(v + 3);
                jo::view<std::uint16_t*>(jo::self<jo::SkyBox>(this)->triangle_materials)[t] = static_cast<std::uint16_t>(first + material);
                jo::view<std::uint16_t*>(jo::self<jo::SkyBox>(this)->triangle_materials)[t + 1] = static_cast<std::uint16_t>(first + material);
                material = static_cast<std::int8_t>(material + 1);
            }
            return;
        }
        joan::orig::as_plane(this)->set_format(3);  // 0x005cdc70
        const float s = size;
        const float n = -size;
        const std::int32_t v = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_vertices(8));  // 0x005cd7f0
        SkyVertex cube[8] = {
            sky_vertex(n, s, s, n, s, n), sky_vertex(s, s, s, n, s, s), sky_vertex(n, n, s, s, s, n), sky_vertex(s, n, s, s, s, s),
            sky_vertex(s, s, n, n, n, s), sky_vertex(n, s, n, n, n, n), sky_vertex(s, n, n, s, n, s), sky_vertex(n, n, n, s, n, n),
        };
        static const std::uint16_t kTriangles[36] = {1, 4, 3, 3, 4, 6, 0, 5, 1, 1, 5, 4, 2, 7, 0, 0, 7, 5,
                                                     3, 6, 2, 2, 6, 7, 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7};
        std::memcpy(jo::view<std::uint8_t*>(jo::self<jo::SkyBox>(this)->vertices_f3) + v * 0x1c, cube, 0xe0);
        std::int32_t t = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(this)->add_triangles(0xc));  // 0x005ce6f0
        for (std::int32_t i = 0; i < 0xc; ++i, ++t) {
            std::uint16_t* const idx = jo::view<std::uint16_t*>(jo::self<jo::SkyBox>(this)->triangles) + t * 3;
            idx[0] = static_cast<std::uint16_t>(v + kTriangles[i * 3]);
            idx[1] = static_cast<std::uint16_t>(v + kTriangles[i * 3 + 1]);
            idx[2] = static_cast<std::uint16_t>(v + kTriangles[i * 3 + 2]);
            jo::view<std::uint16_t*>(jo::self<jo::SkyBox>(this)->triangle_materials)[t] = static_cast<std::uint16_t>(first);
        }
    }
};

#define HELPER31(name, addr, fn) JOAN_REPLACE("en3d/helpers31", name, addr, fn)
HELPER31("SkyBox::build", 0x005f5240, &H::sky_build);
#undef HELPER31

}  // namespace en3d_helpers31
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SkyBox::build(float a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::member<&::en3d_helpers31::H::sky_build>(this, a0, a1, a2); }  // 0x5f5240
