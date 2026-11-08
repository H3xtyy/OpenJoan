// game/unknown_antiport.cpp: functions whose original source file is unknown (formerly game/_unplaced/antiport.cpp;
// re/attribution.csv: between ww/en3d/morph.cpp and ww/en3d/plane.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
namespace {
namespace en3d_b84_01 {


struct H {
    // 0x005cb880
    void vec_set(std::int32_t i, const std::uint32_t* v) {
        jo::Vec3f* const d = jo::self<jo::Vec3f>(this) + i;  // this: a Vec3f array
        jo::view<std::uint32_t>(d->x) = v[0];
        jo::view<std::uint32_t>(d->y) = v[1];
        jo::view<std::uint32_t>(d->z) = v[2];
    }
    // 0x005cb8c0
    void set_30_34(void* a, std::uint32_t b) {
        jo::self<jo::Mesh>(this)->vertices_f6 = static_cast<jo::VertexF6*>(a);
        jo::self<jo::Mesh>(this)->vertices_f7 = jo::arg::as<jo::VertexF7*>(b);  // x64: integer passed as pointer (the facade's a1 is still u32)
    }
    // 0x005cb860
    void nothing() {}
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("Vec3Array::set", 0x005cb880, &H::vec_set);
E3D("Mesh::set_30_34", 0x005cb8c0, &H::set_30_34);
E3D("Mesh::nothing", 0x005cb860, &H::nothing);
#undef E3D

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/game_b102_10.cpp (game/b102_10)
namespace {
namespace game_b102_10 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::obj;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::import_call;

JOAN_COUNTER(c_quad_transform, "Quad::transform");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // 0x005cb8f0
    u32 quad_transform(u32 a, u32 b, u32 c, float scale) {
        c_quad_transform.hit();
        u8 m[64];
        std::memcpy(m, obj(d::g_identity_matrix), 64);
        float r[16];
        joan::orig::matrix_rotation_ypr(joan::orig::arg::mptr(static_cast<void*>(r)), joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c));  // 0x0060bf40
        u32 bits;
        std::memcpy(&bits, &scale, 4);
        if (bits != 0x3f800000u) {
            r[0] = r[0] * scale;
            r[1] = r[1] * scale;
            r[2] = r[2] * scale;
            r[4] = r[4] * scale;
            r[5] = r[5] * scale;
            r[6] = r[6] * scale;
            r[8] = r[8] * scale;
            r[9] = r[9] * scale;
            r[10] = r[10] * scale;
        }
        joan::orig::matrix_copy3x3(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::mptr(static_cast<void*>(r)));  // 0x0060c270
        for (i32 i = 0; i < 4; ++i)
            import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(self() + i * 12), static_cast<void*>(self() + i * 12),
                               static_cast<void*>(m));  // point_transform(out, in, m)
        return 4;
    }
    // 0x005cba40
    u32 quad_translate(const float* v) {
        for (i32 i = 0; i < 4; ++i) {
            float* const p = reinterpret_cast<float*>(self() + i * 12);
            p[0] = p[0] + v[0];
            p[1] = p[1] + v[1];
            p[2] = p[2] + v[2];
        }
        return 4;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_10", name, addr, fn)
B("Quad::transform", 0x005cb8f0, &H::quad_transform);
B("Quad::translate", 0x005cba40, &H::quad_translate);
#undef B

}  // namespace game_b102_10
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Mesh::nothing() -> void { joan::bind::member<&::en3d_b84_01::H::nothing>(this); }  // 0x5cb860
auto joan::orig::AntiPortal::set_corner(std::int32_t a0, void* a1) -> void { joan::bind::member<&::en3d_b84_01::H::vec_set>(this, a0, a1); }  // 0x5cb880
auto joan::orig::Mesh::set_30_34(void* a0, std::uint32_t a1) -> void { joan::bind::member<&::en3d_b84_01::H::set_30_34>(this, a0, a1); }  // 0x5cb8c0
