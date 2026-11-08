// SPDX-License-Identifier: GPL-3.0-or-later
// en3d/skybox.cpp: functions of the original d:\ww\en3d\skybox.cpp (re/attribution.csv). The file name is the
// Restaurant Empire 2 engine's (en3d/skybox.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include <cstdint>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/engine.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/en3d_helpers31.cpp (en3d/helpers31)
namespace {
namespace en3d_helpers31 {

struct H {
    // 0x005f5100 SkyBox::SkyBox. ret 0, returns this.
    void* sky_ctor() {
        joan::orig::as_plane(this)->construct(1u, 0u);  // 0x005cbaf0 Plane ctor
        jo::view<std::uint32_t>(jo::self<jo::SkyBox>(this)->size) = 0;
        return this;
    }

    // 0x005f51b0 SkyBox::init(cube). ret 4.
    // eax on exit: the 0x005cdc70 result, or 0 (movsx of the zero flag).
    std::uint32_t sky_init(std::int32_t cube) {
        joan::orig::as_plane(this)->init(0, 0, 0, 1u);  // 0x005cc7a0
        jo::self<jo::SkyBox>(this)->cube = static_cast<std::uint8_t>(cube);
        if (static_cast<std::int8_t>(jo::self<jo::SkyBox>(this)->cube) != 0) return joan::orig::arg::as<std::uint32_t>(joan::orig::as_plane(this)->set_format(3));  // 0x005cdc70
        return 0;
    }
};

#define HELPER31(name, addr, fn) JOAN_REPLACE("en3d/helpers31", name, addr, fn)
HELPER31("SkyBox::SkyBox", 0x005f5100, &H::sky_ctor);
HELPER31("SkyBox::init", 0x005f51b0, &H::sky_init);
#undef HELPER31

}  // namespace en3d_helpers31
}  // namespace

// ---------------------------------------- from modules/en3d_b82_09.cpp (en3d/b82_09)
namespace {
namespace en3d_b82_09 {

struct H {
    // 0x005f5140 SkyBox::~SkyBox(). eax = the base destructor's.
    std::uint32_t sky_dtor() {
        joan::orig::as_sky_box(this)->release();                         // 0x005f5210
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_plane(this)->dtor());  // 0x005cc750
    }
};

#define B82_09(name, addr, fn) JOAN_REPLACE("en3d/b82_09", name, addr, fn)
B82_09("SkyBox::~SkyBox", 0x005f5140, &H::sky_dtor);
#undef B82_09

}  // namespace en3d_b82_09
}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
namespace {
namespace en3d_b83_03 {

struct H {
    // 0x005f5210 (eax from 0x005ccaa0)
    std::uintptr_t shadow_dtor() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_plane(this)->release()); }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("SkyBox::deinit", 0x005f5210, &H::shadow_dtor);
#undef B83_03

}  // namespace en3d_b83_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SkyBox::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers31::H::sky_ctor>(this)); }  // 0x5f5100
auto joan::orig::SkyBox::dtor() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b82_09::H::sky_dtor>(this)); }  // 0x5f5140
auto joan::orig::SkyBox::init(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers31::H::sky_init>(this, a0)); }  // 0x5f51b0
auto joan::orig::SkyBox::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b83_03::H::shadow_dtor>(this)); }  // 0x5f5210
