// game/unknown_d3dfont.cpp: functions whose original source file is unknown (formerly game/_unplaced/d3dfont.cpp;
// re/attribution.csv: between ww/en3d/bonetree.cpp and ww/en3d/geodef.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>

#include "../original/direct.hpp"
#include "../registry.hpp"
#include "../original/bind.hpp"
#include "../original/layout_access.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {

struct H {
    // --- trivial accessors ----------------------------------------------------------------------
    H* identity() { return this; }                                                    // 0x005bc990
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("identity_5bc990", 0x005bc990, &H::identity);
#undef HELPER6

}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

struct H {
    H* vector_ctor() { return this; }                              // 0x005bd1c0
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("Vector::Vector", 0x005bd1c0, &H::vector_ctor);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/en3d_helpers22.cpp (en3d/helpers22)
namespace {
namespace en3d_helpers22 {

struct H {
    H* vec4_ctor(std::uint32_t x, std::uint32_t y, std::uint32_t z, std::uint32_t w) {  // 0x005bd220
        jo::view<std::uint32_t>(jo::self<jo::Vector4>(this)->x) = x;
        jo::view<std::uint32_t>(jo::self<jo::Vector4>(this)->y) = y;
        jo::view<std::uint32_t>(jo::self<jo::Vector4>(this)->z) = z;
        jo::view<std::uint32_t>(jo::self<jo::Vector4>(this)->w) = w;
        return this;
    }
};

#define HELPER22(name, addr, fn) JOAN_REPLACE("en3d/helpers22", name, addr, fn)
HELPER22("Vec4::Vec4", 0x005bd220, &H::vec4_ctor);
#undef HELPER22

}  // namespace en3d_helpers22
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Vector4::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers6::H::identity>(this)); }  // 0x5bc990
auto joan::orig::Vector::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::vector_ctor>(this)); }  // 0x5bd1c0
auto joan::orig::Vector4::construct_xyzw(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers22::H::vec4_ctor>(this, a0, a1, a2, a3)); }  // 0x5bd220
