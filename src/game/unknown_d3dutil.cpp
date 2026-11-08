// game/unknown_d3dutil.cpp: functions whose original source file is unknown (formerly game/_unplaced/d3dutil.cpp;
// re/attribution.csv: between ww/en3d/bonetree.cpp and ww/en3d/geodef.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../registry.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/en3d_helpers2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2 {

// Every thiscall helper shares one class so `this` is just the object's address.
struct H {
    // --- small ones -----------------------------------------------------------------------
    H* identity_self() { return this; }  // 0x005bea30 Matrix ctor (leaves it uninitialised) / 0x005fe900 clip plane data
};

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("Matrix::Matrix", 0x005bea30, &H::identity_self);
#undef HELPER2

}  // namespace en3d_helpers2
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

struct H {
    // 0x005bed90 / 0x00612ee0
    H* set4(std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d) {
        jo::view<std::uint32_t>(jo::self<jo::Rect>(this)->left) = a;
        jo::view<std::uint32_t>(jo::self<jo::Rect>(this)->top) = b;
        jo::view<std::uint32_t>(jo::self<jo::Rect>(this)->right) = c;
        jo::view<std::uint32_t>(jo::self<jo::Rect>(this)->bottom) = d;
        return this;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("Rect::set_5bed90", 0x005bed90, &H::set4);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

JOAN_COUNTER(c_cross, "vec_cross");
JOAN_COUNTER(c_identity, "mat_identity");

// 0x005beef0: out = a x b (via a local vector, ctor 0x005bd1c0).
float* JOAN_CDECL vec_cross(float* out, const float* a, const float* b) {
    c_cross.hit();
    float tmp[3];
    joan::orig::as_vector(tmp)->construct();  // 0x005bd1c0
    tmp[0] = a[1] * b[2] - a[2] * b[1];
    tmp[1] = a[2] * b[0] - a[0] * b[2];
    tmp[2] = a[0] * b[1] - a[1] * b[0];
    std::uint32_t w[3];
    std::memcpy(w, tmp, sizeof w);
    std::uint32_t* const o = reinterpret_cast<std::uint32_t*>(out);
    o[0] = w[0];
    o[1] = w[1];
    o[2] = w[2];
    return out;
}

// 0x005befa0: 4x4 identity (same store order).
std::uint8_t* JOAN_CDECL mat_identity(std::uint8_t* m) {
    c_identity.hit();
    const std::int32_t zeros[] = {14, 13, 12, 11, 9, 8, 7, 6, 4, 3, 2, 1};  // m[] indices
    for (std::int32_t i = 0; i < 12; ++i) jo::view<std::uint32_t>(jo::as_matrix44(m)->m[zeros[i]]) = 0;
    jo::view<std::uint32_t>(jo::as_matrix44(m)->m[15]) = 0x3f800000u;
    jo::view<std::uint32_t>(jo::as_matrix44(m)->m[10]) = 0x3f800000u;
    jo::view<std::uint32_t>(jo::as_matrix44(m)->m[5]) = 0x3f800000u;
    jo::view<std::uint32_t>(jo::as_matrix44(m)->m[0]) = 0x3f800000u;
    return m;
}

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("vec_cross", 0x005beef0, &vec_cross);
M("mat_identity", 0x005befa0, &mat_identity);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Matrix::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers2::H::identity_self>(this)); }  // 0x5bea30
auto joan::orig::Rect::set_5bed90(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_03::H::set4>(this, a0, a1, a2, a3)); }  // 0x5bed90
auto joan::orig::vec_cross_5beef0(void* a0, void* a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b90_04::vec_cross>(a0, a1, a2)); }  // 0x5beef0
auto joan::orig::mat_identity(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b90_04::mat_identity>(a0)); }  // 0x5befa0
