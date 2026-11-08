// en3d/skin.cpp: functions of the original d:\ww\en3d\skin.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include <cmath>
#include <limits>
#include <cstdarg>
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

namespace jo = joan::orig;

// ---------------------------------------- from modules/en3d_b86_04.cpp (en3d/b86_04)
//
//      0x005f46a0 Skin::clear() - SkinVertex::deinit from the last entry down, frees +8, deletes
//                 +0x36 (0x005c5060 scalar deleting dtor), zeroes +0x3a / +0xc.
//
//      0x005f47c0 Skin::add(n) (ret 4) - grows +8 (capacity +4 = count + n + 0x20, 0x84-byte
//                 entries), SkinVertex::init on the n new entries, returns the old count.
//
//      0x005f4620 Skin::reset() - 0x005f46a0, 0x005f4f10, 0x005f5010(0, (char)+0x31).
namespace {
namespace en3d_b86_04 {

using joan::hook::global;

using joan::call::thiscall;
template <class R = void, class... A>
R import_thiscall(img::Addr slot, const void* self, A... args) {  // v3d_87.dll member function
    return thiscall<R>(global<std::uintptr_t>(slot), self, args...);
}
using joan::img::obj;
using joan::img::str;

using joan::orig::helpers::mul;
std::int32_t add(std::int32_t a, std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

void* const kMemory = obj(d::g_mem);    // allocator (0x0058eb30 realloc, 0x0058ed70 free)

// Skin vertex entry i (0x84 bytes) of the skin's +8 array.
void* skin_vertex(const void* skin, std::int32_t i) {
    return reinterpret_cast<void*>(jo::view<std::uintptr_t>(jo::as_skin(skin)->vertices) + static_cast<std::uint32_t>(mul(i, static_cast<std::int32_t>(sizeof(jo::SkinVertex)))));
}

JOAN_COUNTER(c_skin_clear, "Skin::clear");
JOAN_COUNTER(c_skin_add, "Skin::add");

struct H {
    // 0x005f46a0
    void skin_clear() {
        c_skin_clear.hit();
        while (jo::self<jo::Skin>(this)->vertex_count > 0) {
            import_thiscall(d::iat_v3d_87_SkinVertex_deinit, skin_vertex(this, jo::self<jo::Skin>(this)->vertex_count - 1));  // SkinVertex::deinit
            jo::self<jo::Skin>(this)->vertex_count = jo::self<jo::Skin>(this)->vertex_count - 1;
        }
        jo::self<jo::Skin>(this)->vertex_count = 0;
        if (jo::view<void*>(jo::self<jo::Skin>(this)->vertices) != nullptr) {
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Skin>(this)->vertices)), str(d::s_d_ww_en3d_skin_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a1796c)) + 7));  // 0x0058ed70
            jo::view<void*>(jo::self<jo::Skin>(this)->vertices) = nullptr;
            jo::self<jo::Skin>(this)->vertex_cap = 0;
        }
        if (jo::view<void*>(jo::self<jo::Skin>(this)->shadow) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Skin>(this)->shadow);
            if (p != nullptr) joan::orig::as_skin(p)->destroy(1u);  // 0x005c5060 scalar deleting dtor
            jo::view<void*>(jo::self<jo::Skin>(this)->shadow) = nullptr;
        }
        jo::self<jo::Skin>(this)->shadow_of = nullptr;
        jo::self<jo::Skin>(this)->i32_c = 0;
    }

    // 0x005f47c0
    std::int32_t skin_add(std::int32_t n) {
        c_skin_add.hit();
        if (jo::self<jo::Skin>(this)->vertex_cap < add(jo::self<jo::Skin>(this)->vertex_count, n)) {
            jo::self<jo::Skin>(this)->vertex_cap = add(add(jo::self<jo::Skin>(this)->vertex_count, n), 0x20);
            jo::view<void*>(jo::self<jo::Skin>(this)->vertices) = joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Skin>(this)->vertices)), joan::orig::arg::u32(mul(jo::self<jo::Skin>(this)->vertex_cap, 0x84)), str(d::s_d_ww_en3d_skin_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a17970)) + 5));  // 0x0058eb30
        }
        const std::int32_t old = jo::self<jo::Skin>(this)->vertex_count;
        for (std::int32_t k = n; k > 0; --k) {
            import_thiscall(d::iat_v3d_87_SkinVertex_init, skin_vertex(this, jo::self<jo::Skin>(this)->vertex_count));  // SkinVertex::init
            jo::self<jo::Skin>(this)->vertex_count = add(jo::self<jo::Skin>(this)->vertex_count, 1);
        }
        return old;
    }
};

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b86_04", name, addr, fn)
EN3D("Skin::deinit", 0x005f46a0, &H::skin_clear);
EN3D("Skin::add_vertex", 0x005f47c0, &H::skin_add);
#undef EN3D

}  // namespace en3d_b86_04
}  // namespace

// ---------------------------------------- from modules/en3d_b86_04.cpp (en3d/b86_04)
//
//      0x005f4620 Skin::reset() - 0x005f46a0, 0x005f4f10, 0x005f5010(0, (char)+0x31).
namespace {
namespace en3d_b86_04_2 {

using joan::call::thiscall;

struct H {
    // 0x005f4620
    void skin_reset() {
        joan::orig::as_skin(this)->clear();  // 0x005f46a0
        joan::orig::as_skin(this)->clear_slots();  // 0x005f4f10
        joan::orig::as_skin(this)->attach_to(joan::orig::arg::mptr(0), static_cast<std::int32_t>(jo::self<jo::Skin>(this)->slot_index));  // 0x005f5010
    }
};

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b86_04", name, addr, fn)
EN3D("Skin::~Skin", 0x005f4620, &H::skin_reset);
#undef EN3D

}  // namespace en3d_b86_04_2
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
namespace {
namespace en3d_b85_04 {

using joan::call::thiscall;

struct H {
    std::uint32_t dtor_5f4670() { joan::orig::as_skin(this)->clear(); return 0; }  // 0x005f46a0 (returns nothing; callers discard)
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("Skin::init", 0x005f4670, &H::dtor_5f4670);
#undef E3D

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/game_b102_11.cpp (game/b102_11)
namespace {
namespace game_b102_11 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;


JOAN_COUNTER(c_groups, "Groups::scan");

struct H {
    // 0x005f48b0 (ret)
    i32 next_id() {
        c_groups.hit();
        i32 best = 0;
        for (i32 i = 0; i < jo::self<jo::Skin>(this)->vertex_count; ++i) {
            jo::SkinVertex* const g = &jo::self<jo::Skin>(this)->vertices[i];
            for (i32 j = 0; j < g->count; ++j) {
                const i32 v = (&g->bone0)[j].bone;
                if (v >= best) best = static_cast<i32>(static_cast<u32>(v) + 1u);  // add ecx,1 (wraps)
            }
        }
        return best;
    }

    // 0x005f4b90 (ret)
    i32 max_count() {
        c_groups.hit();
        i32 best = 0;
        for (i32 i = 0; i < jo::self<jo::Skin>(this)->vertex_count; ++i) {
            const i32 v = jo::self<jo::Skin>(this)->vertices[i].count;
            if (v > best) best = jo::self<jo::Skin>(this)->vertices[i].count;
        }
        return best;
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b102_11", name, addr, fn)
B11("Groups::next_id", 0x005f48b0, &H::next_id);
B11("Groups::max_count", 0x005f4b90, &H::max_count);
#undef B11

}  // namespace game_b102_11
}  // namespace

// ---------------------------------------- from modules/en3d_b86_04.cpp (en3d/b86_04)
namespace {
namespace en3d_b86_04_3 {

using joan::call::cdecl_call;
using joan::call::import_call;

using joan::hook::global;

using joan::call::thiscall;
template <class R = void, class... A>
R import_thiscall(img::Addr slot, const void* self, A... args) {  // v3d_87.dll member function
    return thiscall<R>(global<std::uintptr_t>(slot), self, args...);
}

using joan::orig::helpers::mul;
std::int32_t add(std::int32_t a, std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

// Skin vertex entry i (0x84 bytes) of the skin's +8 array.
void* skin_vertex(const void* skin, std::int32_t i) {
    return reinterpret_cast<void*>(jo::view<std::uintptr_t>(jo::as_skin(skin)->vertices) + static_cast<std::uint32_t>(mul(i, static_cast<std::int32_t>(sizeof(jo::SkinVertex)))));
}

JOAN_COUNTER(c_skin_bounds, "Skin::compute_bounds");

struct H {
    // 0x005f4a00
    void skin_bounds() {
        c_skin_bounds.hit();
        std::int32_t count = 1;
        float max_sq = 0.0f;
        for (std::int32_t i = 0; i < jo::self<jo::Skin>(this)->vertex_count; ++i) {
            std::uint8_t* const sv = static_cast<std::uint8_t*>(skin_vertex(this, i));
            for (std::int32_t j = 0; j < jo::as_skin_vertex(sv)->count; ++j) {
                jo::SkinInfluence* const inf = jo::as_skin_influence(sv + (j << 5) + 4);
                if (inf->weight > 0.0f) {  // test ah,0x41 jne: weight > 0, not NaN
                    if (inf->bone >= count) count = add(inf->bone, 1);
                    const float sq = static_cast<float>(import_call<double>(d::iat_v3d_87_length_sq, static_cast<void*>(inf->pos)));  // length_sq
                    if (sq > max_sq) max_sq = sq;
                }
            }
        }
        jo::self<jo::Skin>(this)->center[0] = global<std::uint32_t>(d::g_pos);
        jo::self<jo::Skin>(this)->center[1] = global<std::uint32_t>(d::g_a69cdc);
        jo::self<jo::Skin>(this)->center[2] = global<std::uint32_t>(d::g_a69ce0);
        const joan::hook::x87::Fpu fpu{true};
        const double log_count = joan::orig::arg::as<double>(joan::orig::libc::log(joan::orig::arg::f64(static_cast<double>(count))));  // log, fstp qword
        const double log_two = joan::orig::arg::as<double>(joan::orig::libc::log(joan::orig::arg::f64(2.0)));
        const double q = fpu.div(log_count, log_two);  // fdivr qword (see header)
        const double root = joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(max_sq))));  // sqrt
        jo::self<jo::Skin>(this)->radius = static_cast<float>(fpu.mul(root, q));
    }

    // 0x005f4970
    std::int32_t skin_add_bone(std::int32_t sv, std::int32_t bone, void* a, void* b, std::uint32_t weight) {
        return import_thiscall<std::int32_t>(d::iat_v3d_87_SkinVertex_add_bone, skin_vertex(this, sv), bone, a, b, weight);  // SkinVertex::add_bone
    }

    // 0x005f49c0
    void skin_normalize(std::int32_t sv) {
        import_thiscall(d::iat_v3d_87_SkinVertex_normalize, skin_vertex(this, sv));  // SkinVertex::normalize
    }
};

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b86_04", name, addr, fn)
EN3D("Skin::estimate_bounding_sphere", 0x005f4a00, &H::skin_bounds);
EN3D("Skin::vertex_add_bone", 0x005f4970, &H::skin_add_bone);
EN3D("Skin::vertex_normalize", 0x005f49c0, &H::skin_normalize);
#undef EN3D

}  // namespace en3d_b86_04_3
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16 {

using joan::call::cdecl_call;
using joan::call::import_call;

using joan::call::thiscall;

JOAN_COUNTER(c_skin, "Skin::deform");

struct H {

    void* mesh_lod(std::int32_t i) { return jo::self<jo::Skin>(this)->slots[i - 1]; }  // 0x005f4fb0 (levels 1..4)

    // 0x005f4c10 Skin::deform(plane): update the bones, then v3d_87 skin_deform into the
    // plane's vertex buffer (+0 vertex count, +8 skin vertices of 0x84 bytes).
    void skin_deform(std::uint8_t* plane) {
        c_skin.hit();
        if (jo::as_plane(plane)->vertex_format == 7) {
            for (std::int32_t b = 0; b < jo::as_plane(plane)->bone_count; ++b) joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(plane)->bone_5cf8f0(joan::orig::arg::i32(b))))->update();  // 0x0060ee60
            return;
        }
        if (jo::as_plane(plane)->data_pending != 0) joan::orig::as_plane(plane)->load_5d9be0();
        std::int32_t stride = 0;
        std::uint8_t* const vb = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(plane)->buffer(&stride));  // 0x005cdae0
        std::uint8_t dummy_normal[12];
        std::uint8_t* normals;
        std::int32_t normal_stride;
        if (jo::as_vertex_format(joan::orig::arg::as<void*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<std::int32_t>(jo::as_plane(plane)->vertex_format))))))->prelit != 0) {
            normals = dummy_normal;
            normal_stride = 0;
        } else {
            normals = vb + jo::as_vertex_format(joan::orig::arg::as<void*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<std::int32_t>(jo::as_plane(plane)->vertex_format))))))->normal_offset;
            normal_stride = stride;
        }
        for (std::int32_t b = 0; b < jo::as_plane(plane)->bone_count; ++b) joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(plane)->bone_5cf8f0(joan::orig::arg::i32(b))))->update();
        import_call(d::iat_v3d_87_skin_deform, jo::self<jo::Skin>(this)->vertex_count, vb, stride, normals, normal_stride, jo::view<void*>(jo::self<jo::Skin>(this)->vertices), 0x84,
                    jo::view<std::uint8_t*>(jo::as_plane(plane)->bones) + offsetof(jo::Bone, matrix_), static_cast<std::int32_t>(sizeof(jo::Bone)));  // bones + 0x58, stride 0xa0 (x86)
        jo::as_plane(plane)->transformed = 1;
        const std::uint8_t changed = plane[0xa];
        joan::orig::as_plane(plane)->invalidate(2u);  // 0x005cf920
        plane[0xa] = changed;
    }
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
#define HELPER17(name, addr, fn) JOAN_REPLACE("en3d/helpers17", name, addr, fn)
HELPER16("Skin::get_lod_child", 0x005f4fb0, &H::mesh_lod);
HELPER17("Skin::update_plane", 0x005f4c10, &H::skin_deform);
#undef HELPER16
#undef HELPER17

}  // namespace en3d_helpers16
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
namespace {
namespace en3d_b84_01 {

using joan::call::cdecl_call;

using joan::call::thiscall;

using joan::orig::helpers::game_new;  // operator new

struct H {
    static void* new_shadow() {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Skin)));  // 0x3e on x86
        return mem != nullptr ? joan::orig::as_skin(mem)->construct() : nullptr;  // 0x005f45f0
    }

    // 0x005f4fe0
    void set_30_byte(std::uint8_t v) { jo::self<jo::Skin>(this)->u8_30 = v; }

    // 0x005f5090
    void link_36(void* p) {
        if (jo::view<void*>(jo::self<jo::Skin>(this)->shadow) != nullptr) jo::self<jo::Skin>(this)->shadow->shadow_of = nullptr;
        jo::view<void*>(jo::self<jo::Skin>(this)->shadow) = p;
        jo::view<void*>(jo::self<jo::Skin>(this)->shadow->shadow_of) = this;
    }

    // 0x005f4dc0
    void* shadow_add(std::int32_t i, void* shadow) {
        joan::orig::as_skin(this)->remove_lod(joan::orig::arg::i32(i));  // 0x005f4e90
        void* const s = shadow != nullptr ? shadow : new_shadow();
        joan::orig::as_skin(s)->attach_to(joan::orig::arg::mptr(static_cast<void*>(this)), joan::orig::arg::i32(i));  // 0x005f5010
        return s;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("Skin::add_lod", 0x005f4dc0, &H::shadow_add);
E3D("Shadow::link", 0x005f5090, &H::link_36);
E3D("Skin::set_lod_count", 0x005f4fe0, &H::set_30_byte);
#undef E3D

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04_2.cpp (en3d/b85_04)
namespace {
namespace en3d_b85_04_2 {

using joan::call::thiscall;

struct H {
    // 0x005f5010
    H* shadow_attach(std::uint8_t* list, std::int32_t i) {
        if (jo::view<std::uint8_t*>(jo::self<jo::Skin>(this)->owner) == list) return this;
        if (jo::view<std::uint8_t*>(jo::self<jo::Skin>(this)->owner) != nullptr) {
            jo::self<jo::Skin>(this)->owner->slots[i - 1] = nullptr;
            jo::view<std::uint8_t*>(jo::self<jo::Skin>(this)->owner) = nullptr;
        }
        if (list != nullptr) {
            jo::view<void*>(jo::as_skin(list)->slots[i - 1]) = this;
            jo::view<std::uint8_t*>(jo::self<jo::Skin>(this)->owner) = list;
        }
        return this;
    }

    // 0x005f4e90
    std::int32_t shadow_remove(std::int32_t i) {
        if (jo::self<jo::Skin>(this)->slots[i - 1] != nullptr) {
            void* const p = jo::self<jo::Skin>(this)->slots[i - 1];
            if (p != nullptr) joan::orig::as_skin(p)->destroy(1u);  // 0x005c5060
            jo::self<jo::Skin>(this)->slots[i - 1] = nullptr;
        }
        return i;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("Skin::set_lod_parent", 0x005f5010, &H::shadow_attach);
E3D("Skin::remove_lod", 0x005f4e90, &H::shadow_remove);
#undef E3D

}  // namespace en3d_b85_04_2
}  // namespace

// ---------------------------------------- from modules/en3d_b87_02.cpp (en3d/b87_02)
namespace {
namespace en3d_b87_02 {

using joan::call::thiscall;

struct H {
    // 0x005f4f10
    void skin_clear_slots() {
        for (std::int32_t i = 4; i > 0; --i) {
            void* const p = jo::self<jo::Skin>(this)->slots[i - 1];
            if (p != nullptr) joan::orig::as_skin(p)->destroy(1u);  // 0x005c5060 scalar deleting dtor
            jo::self<jo::Skin>(this)->slots[i - 1] = nullptr;
        }
        jo::self<jo::Skin>(this)->u8_30 = 0;
        jo::self<jo::Skin>(this)->owner = nullptr;
    }
};

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b87_02", name, addr, fn)
EN3D("Skin::clear_all_lod", 0x005f4f10, &H::skin_clear_slots);
#undef EN3D

}  // namespace en3d_b87_02
}  // namespace

// ---------------------------------------- from modules/game_b104_10.cpp (game/b104_10)
namespace {
namespace game_b104_10 {

using i32 = std::int32_t;
using u32 = std::uint32_t;


struct H {
    jo::Skin* get_36() { return jo::self<jo::Skin>(this)->shadow; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_10", name, addr, fn)
B("Obj::get_36_5f50e0", 0x005f50e0, &H::get_36);
#undef B

}  // namespace game_b104_10
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01_2.cpp (en3d/b84_01)
namespace {
namespace en3d_b84_01_2 {

void game_memset(void* p, std::int32_t v, std::uint32_t n) { std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    void* ctor_3e() {
        game_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::Shadow)));  // 0x3e (x86)
        return this;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("Skin::Skin", 0x005f45f0, &H::ctor_3e);
#undef E3D

}  // namespace en3d_b84_01_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Skin::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b84_01_2::H::ctor_3e>(this)); }  // 0x5f45f0
auto joan::orig::Skin::reset() -> void { joan::bind::member<&::en3d_b86_04_2::H::skin_reset>(this); }  // 0x5f4620
auto joan::orig::Skin::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b85_04::H::dtor_5f4670>(this)); }  // 0x5f4670
auto joan::orig::Skin::clear() -> void { joan::bind::member<&::en3d_b86_04::H::skin_clear>(this); }  // 0x5f46a0
auto joan::orig::Skin::add(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b86_04::H::skin_add>(this, a0)); }  // 0x5f47c0
auto joan::orig::Skin::add_bone(std::int32_t a0, std::int32_t a1, void* a2, void* a3, std::uint32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b86_04_3::H::skin_add_bone>(this, a0, a1, a2, a3, a4)); }  // 0x5f4970
auto joan::orig::Skin::normalize(std::int32_t a0) -> void { joan::bind::member<&::en3d_b86_04_3::H::skin_normalize>(this, a0); }  // 0x5f49c0
auto joan::orig::Skin::compute_bounds() -> void { joan::bind::member<&::en3d_b86_04_3::H::skin_bounds>(this); }  // 0x5f4a00
auto joan::orig::Skin::deform(void* a0) -> void { joan::bind::member<&::en3d_helpers16::H::skin_deform>(this, a0); }  // 0x5f4c10
auto joan::orig::Skin::add_lod(std::int32_t a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b84_01::H::shadow_add>(this, a0, a1)); }  // 0x5f4dc0
auto joan::orig::Skin::remove_lod(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b85_04_2::H::shadow_remove>(this, a0)); }  // 0x5f4e90
auto joan::orig::Skin::clear_slots() -> void { joan::bind::member<&::en3d_b87_02::H::skin_clear_slots>(this); }  // 0x5f4f10
auto joan::orig::Mesh::lod(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::mesh_lod>(this, a0)); }  // 0x5f4fb0
auto joan::orig::Skin::set_lod_count(std::uint8_t a0) -> void { joan::bind::member<&::en3d_b84_01::H::set_30_byte>(this, a0); }  // 0x5f4fe0
auto joan::orig::Skin::attach_to(void* a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b85_04_2::H::shadow_attach>(this, a0, a1)); }  // 0x5f5010
auto joan::orig::Skin::link_36(void* a0) -> void { joan::bind::member<&::en3d_b84_01::H::link_36>(this, a0); }  // 0x5f5090
auto joan::orig::Skin::linked_36() -> Skin* { return joan::bind::conv<Skin*>(joan::bind::member<&::game_b104_10::H::get_36>(this)); }  // 0x5f50e0
