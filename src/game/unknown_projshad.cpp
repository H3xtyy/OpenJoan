// game/unknown_projshad.cpp: functions whose original source file is unknown (formerly game/_unplaced/projshad.cpp;
// re/attribution.csv: between ww/en3d/portal.cpp and ww/en3d/pshader.cpp). They move to their original file when
// evidence names it (Phase 3).
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
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::img::obj;

void* const kTextures = obj(d::g_texture_array);   // texture manager

}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
namespace {
namespace en3d_b84_01 {

void game_memset(void* p, std::int32_t v, std::uint32_t n) { std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    void* ctor_30() {
        game_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::TexGen)));  // 0x30 on x86 (TexGen holds the texture pointer)
        return this;
    }

    // 0x005e55f0
    void set_texture(void* t) {
        if (jo::self<jo::TexGen>(this)->texture != nullptr) joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(jo::self<jo::TexGen>(this)->texture));  // 0x0060a550
        jo::self<jo::TexGen>(this)->texture = t;
        if (t != nullptr) joan::orig::as_texture_manager(kTextures)->use(joan::orig::arg::mptr(jo::self<jo::TexGen>(this)->texture));  // 0x0060a500
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("Object::set_texture", 0x005e55f0, &H::set_texture);
E3D("SceneObject::SceneObject", 0x005e5540, &H::ctor_30);
#undef E3D

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
namespace {
namespace en3d_b85_04 {

struct H {
    std::uint32_t dtor_5e5570() {
        joan::orig::as_tex_projector(this)->release_texture();  // 0x005e55a0
        return 0;  // release_texture returns nothing; callers (0x005efb9f, scene_s.cpp) discard the result
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("dtor_5e5570", 0x005e5570, &H::dtor_5e5570);
#undef E3D

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/en3d_b86_04.cpp (en3d/b86_04)
namespace {
namespace en3d_b86_04 {

struct H {
    // 0x005e55a0
    void release_texture() {
        if (jo::self<jo::TexGen>(this)->texture != nullptr) {
            joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(jo::self<jo::TexGen>(this)->texture));  // 0x0060a550
            jo::self<jo::TexGen>(this)->texture = nullptr;
        }
    }
};

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b86_04", name, addr, fn)
EN3D("Object::release_texture", 0x005e55a0, &H::release_texture);
#undef EN3D

}  // namespace en3d_b86_04
}  // namespace

// ---------------------------------------- from modules/en3d_helpers25.cpp (en3d/helpers25)
namespace {
namespace en3d_helpers25 {

using joan::call::cdecl_call;

using joan::hook::global;

using joan::call::import_call;

using joan::call::stdcall_call;

JOAN_COUNTER(c_tex_matrix, "TexGen::matrix");

struct H {
    // 0x005e5660 TexGen::matrix(view matrix by value, out): projected texture matrix -
    // view * inverse(orbit placement) * ortho(+0x1c, +0x20, 50, 4000) * scale 0.5 * optional
    // rotation by time * +0x28, then +0.5 translation. Disabled when +0 is set.
    // The 16 by-value dwords are copied by name: they are contiguous on the x86 stack only (x64 passes the first
    // ones in registers and every one in an 8-byte slot).
    void texgen_matrix(std::uint32_t m00, std::uint32_t m01, std::uint32_t m02, std::uint32_t m03, std::uint32_t m10,
                       std::uint32_t m11, std::uint32_t m12, std::uint32_t m13, std::uint32_t m20, std::uint32_t m21,
                       std::uint32_t m22, std::uint32_t m23, std::uint32_t m30, std::uint32_t m31, std::uint32_t m32,
                       std::uint32_t m33, float* out) {
        c_tex_matrix.hit();
        if (jo::self<jo::TexGen>(this)->disabled != 0) return;
        const std::uint32_t view[16] = {m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33};
        std::memcpy(out, view, sizeof view);
        float m[16];
        joan::orig::orbit_matrix(joan::orig::arg::mptr(m), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::TexGen>(this)->orbit[0])), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::TexGen>(this)->orbit[1])), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::TexGen>(this)->orbit[2])), joan::orig::arg::f32(0x43480000u));  // 0x0060bd00 orbit, 200
        import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, m, jo::self<jo::TexGen>(this)->position);  // matrix_multiply_translation
        import_call<void*>(d::iat_v3d_87_matrix_inverse_1, m);              // matrix_inverse (in place)
        import_call<void*>(d::iat_v3d_87_matrix_multiply_2, out, m);         // matrix_multiply (in place)
        float p[16];
        joan::orig::as_matrix(p)->construct();                                                     // 0x005bea30 Matrix
        joan::orig::d3dx::matrix_ortho_lh(joan::orig::arg::mptr(p), joan::orig::arg::f32(jo::self<jo::TexGen>(this)->width), joan::orig::arg::f32(jo::self<jo::TexGen>(this)->height), 50.0f, 4000.0f);  // D3DXMatrixOrthoLH
        import_call<void*>(d::iat_v3d_87_matrix_multiply_2, out, p);
        std::memcpy(m, img::obj(d::g_identity_matrix), 64);  // identity
        m[5] = 0.5f;
        m[0] = 0.5f;
        import_call<void*>(d::iat_v3d_87_matrix_multiply_2, out, m);
        if (jo::self<jo::TexGen>(this)->f32_28 < 0.0f || jo::self<jo::TexGen>(this)->f32_28 > 0.0f) {
            float angle = 0.0f;
            const joan::hook::x87::Fpu fpu{true};
            bool done = false;
            if (jo::view<std::uint32_t>(jo::self<jo::TexGen>(this)->f32_2c) != 0) {
                void* const units = img::obj(d::g_items);
                if (joan::orig::as_units(units)->list.empty_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::TexGen>(this)->f32_2c))) == 0 &&  // 0x00425d50
                    jo::view<void*>(jo::as_map_object(joan::orig::as_units(units)->list.ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::TexGen>(this)->f32_2c))))->actor) != nullptr) {  // 0x00425d20
                    const std::uint8_t* const unit = jo::view<std::uint8_t*>(jo::as_map_object(joan::orig::as_units(units)->list.ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::TexGen>(this)->f32_2c))))->actor);
                    const std::uint32_t dt = global<std::uint32_t>(d::g_now) - jo::as_actor(unit)->start_time;
                    angle = static_cast<float>(fpu.mul(static_cast<double>(dt), static_cast<double>(jo::self<jo::TexGen>(this)->f32_28)));
                    done = true;
                }
            }
            if (!done) angle = static_cast<float>(fpu.mul(static_cast<double>(global<std::uint32_t>(d::g_now)), static_cast<double>(jo::self<jo::TexGen>(this)->f32_28)));
            joan::orig::matrix_rotation(joan::orig::arg::mptr(m), joan::orig::arg::f32(angle));  // 0x0060bc50 rotation
            import_call<void*>(d::iat_v3d_87_matrix_multiply_2, out, m);
        }
        const float half[3] = {0.5f, 0.5f, 0.0f};
        import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, out, half);
    }
};

#define HELPER25(name, addr, fn) JOAN_REPLACE("en3d/helpers25", name, addr, fn)
HELPER25("TexGen::matrix", 0x005e5660, &H::texgen_matrix);
#undef HELPER25

}  // namespace en3d_helpers25
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

void* crt_memset(void* d, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // 0x005e5930 PixelShaderCache ctor (new(0xc) at 0x005f66cb, stored in SysEn::ps_cache)
    void* clear_c() {
        crt_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::PixelShaderCache)));  // 0xc on x86
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Obj::clear_c", 0x005e5930, &H::clear_c);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SceneObject::SceneObject_5e5540() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b84_01::H::ctor_30>(this)); }  // 0x5e5540
auto joan::orig::TexProjector::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b85_04::H::dtor_5e5570>(this)); }  // 0x5e5570
auto joan::orig::TexProjector::release_texture() -> void { joan::bind::member<&::en3d_b86_04::H::release_texture>(this); }  // 0x5e55a0
auto joan::orig::TexProjector::set_texture(void* a0) -> void { joan::bind::member<&::en3d_b84_01::H::set_texture>(this, a0); }  // 0x5e55f0
auto joan::orig::TexProjector::texture_matrix(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10, std::uint32_t a11, std::uint32_t a12, std::uint32_t a13, std::uint32_t a14, std::uint32_t a15, void* a16) -> void { joan::bind::member<&::en3d_helpers25::H::texgen_matrix>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16); }  // 0x5e5660
auto joan::orig::PixelShaderCache::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_06::H::clear_c>(this)); }  // 0x5e5930
