// game/unknown_pathmesh.cpp: functions whose original source file is unknown (formerly game/_unplaced/pathmesh.cpp;
// re/attribution.csv: between ww/enpath/pathsp.cpp and ww/enpath/pathsp.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::x87::Fpu;

using joan::call::thiscall;

using joan::call::cdecl_call;

struct P2 {
    std::int32_t x, y;
};

using joan::hook::global;

using joan::call::import_call;

using u8 = std::uint8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::img::obj;

// ---------------------------------------------------------------------------- 0x00669e50
struct Pt {
    i32 x, y;
};

using u16 = std::uint16_t;

using joan::hook::x87::ftol;

using u32 = std::uint32_t;
}  // namespace

// ---------------------------------------- from modules/game_light1.cpp (game/light1)
namespace {
namespace game_light1 {

struct H {
    // 0x00664fd0 PfdData::PfdData() (ret 0).
    void* pfd_construct() {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::PathSpace));       // memset (0x38 on x86)
        return this;
    }

    // 0x0066a440 P2::length_sq() (ret 0).
    std::int32_t length_sq() {
        const std::uint32_t x = jo::view<std::uint32_t>(jo::self<jo::Vec2i>(this)->x), y = jo::view<std::uint32_t>(jo::self<jo::Vec2i>(this)->y);
        return static_cast<std::int32_t>(x * x + y * y);
    }

    // 0x0066a4f0 P2::copy_to(out) (ret 4).
    P2* copy_to(P2* out) {
        out->x = jo::self<jo::Vec2i>(this)->x;
        out->y = jo::self<jo::Vec2i>(this)->y;
        return out;
    }
};

#define L1(name, addr, fn) JOAN_REPLACE("game/light1", name, addr, fn)
L1("PfdData::PfdData", 0x00664fd0, &H::pfd_construct);
L1("P2::length_sq", 0x0066a440, &H::length_sq);
L1("P2::copy_to", 0x0066a4f0, &H::copy_to);
#undef L1

}  // namespace game_light1
}  // namespace

// ---------------------------------------- from modules/game_b100_14.cpp (game/b100_14)
namespace {
namespace game_b100_14 {

using u32 = std::uint32_t;

// ---------------------------------------------------------------------- 0x0066b6d0
static void JOAN_CDECL scale_a6a27c() {
    global<float>(d::g_a6a27c) = global<float>(d::g_a6a274) / global<float>(d::flt_2);
}

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_14", name, addr, fn)
B100("scale_a6a27c", 0x0066b6d0, &scale_a6a27c);
#undef B100

}  // namespace game_b100_14
}  // namespace

// ---------------------------------------- from modules/game_b97_07.cpp (game/b97_07)
namespace {
namespace game_b97_07 {

struct H {
    // ------------------------------------------------------------------ 0x00664b00
    i32 not_after(void* other) {
        const i32 r = joan::orig::arg::as<i32>(joan::orig::key_compare(joan::orig::arg::i32(jo::self<jo::TerrainBlock>(this)->key), joan::orig::arg::i32(jo::self<jo::TerrainBlock>(this)->key_c), static_cast<i32>(jo::self<jo::TerrainBlock>(this)->key_b), joan::orig::arg::i32(jo::as_terrain_block(other)->key), joan::orig::arg::i32(jo::as_terrain_block(other)->key_c), static_cast<i32>(jo::as_terrain_block(other)->key_b)));
        return r <= 0 ? 1 : 0;
    }

    // ------------------------------------------------------------------ 0x0066a2e0
    i32* get_pair_ind(i32* out) {
        const i32* const p = jo::self<jo::MeshVertex>(this)->point->xyz;
        const i32 a = p[0];
        const i32 b = p[1];
        out[0] = a;
        out[1] = b;
        return out;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_07", name, addr, fn)
B97("Obj::not_after", 0x00664b00, &H::not_after);
B97("Obj::get_pair_ind", 0x0066a2e0, &H::get_pair_ind);
#undef B97

}  // namespace game_b97_07
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

struct H {
    void* self() { return this; }

    // 0x006657d0
    std::int32_t pair_contains(void* p) {
        std::int32_t r;
        if (p == jo::view<void*>(jo::self<jo::Pair>(this)->a) || p == jo::view<void*>(jo::self<jo::Pair>(this)->b) ||
            joan::orig::arg::as<std::int32_t>(joan::orig::as_triple(jo::view<void*>(jo::self<jo::Pair>(this)->a))->ret0_66a5e0(joan::orig::arg::mptr(p))) != 0 ||  // 0x0066a5e0
            joan::orig::arg::as<std::int32_t>(joan::orig::as_triple(jo::view<void*>(jo::self<jo::Pair>(this)->b))->ret0_66a5e0(joan::orig::arg::mptr(p))) != 0)    // 0x0066a5e0
            r = 1;
        else
            r = 0;
        return r;
    }
    // 0x00665c20
    void* pair_ctor(jo::Triple* a, jo::Triple* b) {
        jo::self<jo::Pair>(this)->a = a;
        jo::self<jo::Pair>(this)->b = b;
        return self();
    }
    // 0x0066a4b0
    void* triple_ctor(std::uint32_t a, std::uint32_t b, std::uint32_t c) {
        jo::self<jo::Triple>(this)->a = a;
        jo::self<jo::Triple>(this)->b = b;
        jo::self<jo::Triple>(this)->c = c;
        return self();
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Pair::contains", 0x006657d0, &H::pair_contains);
B91("Pair::Pair", 0x00665c20, &H::pair_ctor);
B91("Triple::Triple", 0x0066a4b0, &H::triple_ctor);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
namespace {
namespace game_b82_14 {

using joan::orig::helpers::op_delete;

struct H {
    // 0x006651e0 PfdItem scalar deleting destructor (ret 4).
    void* item_delete(std::uint32_t flags) {
        joan::orig::as_blk_arr(this)->dtor();  // 0x00637b00
        if ((flags & 1u) != 0) op_delete(this);
        return this;
    }
};

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("PfdItem::scalar_deleting_destructor", 0x006651e0, &H::item_delete);
#undef B82_14

}  // namespace game_b82_14
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
namespace {
namespace game_b103_14 {

using u32 = std::uint32_t;

struct H {
    std::uintptr_t self_addr() { return reinterpret_cast<std::uintptr_t>(this); }

    // ------------------------------------------------------------------ 0x00665230
    void* clear_30_34() {
        jo::self<jo::PathSpace>(this)->grid = nullptr;
        jo::self<jo::PathSpace>(this)->sub_grids = nullptr;
        return this;
    }

    // ------------------------------------------------------------------ list wrappers
    std::uintptr_t list_item_at0(i32 i) {  // 0x00665850 (EAX: this / the removed item, pointer-wide on x64)
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->points) == nullptr) return self_addr();
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_list(jo::view<void*>(jo::self<jo::PathSpace>(this)->points))->remove_at(joan::orig::arg::i32(i)));  // 0x00637e20
    }
    u32 list_count_at0() {  // 0x006659c0
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->points) == nullptr) return 0;
        return joan::orig::arg::as<u32>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathSpace>(this)->points))->get_2c());  // 0x0066b7a0
    }
    using Slot = jo::BlkArr* jo::PathSpace::*;
    std::uintptr_t list_item(Slot m, i32 i) {  // EAX: this / the count / the removed item (pointer-wide on x64)
        if (jo::self<jo::PathSpace>(this)->*m == nullptr) return self_addr();
        if (!(i > 0)) {
            const u32 n = joan::orig::arg::as<u32>(joan::orig::as_path_space(jo::self<jo::PathSpace>(this)->*m)->get_2c());  // 0x0066b7a0
            if (i > static_cast<i32>(n)) return n;
        }
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_list(jo::self<jo::PathSpace>(this)->*m)->remove_at(joan::orig::arg::i32(i)));  // 0x00637e20
    }
    u32 list_count(Slot m) {
        if (jo::self<jo::PathSpace>(this)->*m == nullptr) return 0;
        return joan::orig::arg::as<u32>(joan::orig::as_path_space(jo::self<jo::PathSpace>(this)->*m)->get_2c());  // 0x0066b7a0
    }
    u32 list_count_at4() { return list_count(&jo::PathSpace::pairs4); }          // 0x00665f30
    std::uintptr_t list_item_at8(i32 i) { return list_item(&jo::PathSpace::pairs8, i); }    // 0x006664f0
    u32 list_count_at8() { return list_count(&jo::PathSpace::pairs8); }          // 0x00666550
    u32 list_count_atc() { return list_count(&jo::PathSpace::links); }          // 0x00666b50

};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("Obj::clear_30_34", 0x00665230, &H::clear_30_34);
B103("List0::item", 0x00665850, &H::list_item_at0);
B103("List0::count", 0x006659c0, &H::list_count_at0);
B103("List4::count", 0x00665f30, &H::list_count_at4);
B103("List8::item", 0x006664f0, &H::list_item_at8);
B103("List8::count", 0x00666550, &H::list_count_at8);
B103("ListC::count", 0x00666b50, &H::list_count_atc);
#undef B103

}  // namespace game_b103_14
}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
namespace {
namespace game_b83_04 {

struct H {
    // 0x00665430 (ret 4)
    H* delete_665430(std::uint32_t flags) {
        joan::orig::as_height_grid(this)->free();  // 0x004c5870
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("Obj665430::scalar_deleting_destructor", 0x00665430, &H::delete_665430);
#undef B83_04

}  // namespace game_b83_04
}  // namespace

// ---------------------------------------- from modules/game_b102_13.cpp (game/b102_13)
namespace {
namespace game_b102_13 {

using u32 = std::uint32_t;

// Block array helpers (uti3d blkarrb).
i32 arr_count(void* a) { return joan::orig::arg::as<i32>(joan::orig::as_path_space(a)->get_2c()); }          // 0x0066b7a0
u8* arr_get(void* a, i32 n) { return joan::orig::arg::as<u8*>(joan::orig::as_billboard(a)->item(joan::orig::arg::i32(n))); }  // 0x00637da0
const u32* make_pt(u32* pt, u32 x, u32 y) { return joan::orig::arg::as<const u32*>(joan::orig::as_point(pt)->set(joan::orig::arg::u32(x), joan::orig::arg::u32(y))); }  // 0x004eeb80

JOAN_COUNTER(c_regions, "Regions::b102_13");

struct H {
    // ================================================================== Regions (dead code)
    // ------------------------------------------------------------------ 0x00665890
    i32 find_point(u32 x, u32 y, u32 r) {
        c_regions.hit();
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->points) == nullptr) return 0;
        for (i32 i = 1; i <= arr_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->points)); ++i) {
            u8* const e = arr_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->points), i);
            u32 pt[2];
            const u32* const p = make_pt(pt, x, y);
            if (joan::orig::arg::as<i32>(joan::orig::as_triple(e)->contains(joan::orig::arg::i32(p[0]), joan::orig::arg::i32(p[1]), joan::orig::arg::i32(r))) != 0) return i;  // 0x0066a530
        }
        return 0;
    }

};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_13", name, addr, fn)
B("Regions::find_point_665890", 0x00665890, &H::find_point);
#undef B

}  // namespace game_b102_13
}  // namespace

// ---------------------------------------- from modules/game_b105_18.cpp (game/b105_18)
namespace {
namespace game_b105_18 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_tree_check, "Tree::check_666260");

void JOAN_CDECL init_66b650() { joan::orig::init_pi_a6a274(); }
void JOAN_CDECL init_66b6b0() { joan::orig::scale_a6a27c(); }
void JOAN_CDECL init_66b700() { joan::orig::init_a6a280(); }
void JOAN_CDECL init_66b750() { joan::orig::scale_quarter_a6a284(); }

// 0x00667170 (qsort comparator)
i32 JOAN_CDECL cmp_int_667170(const u32* a, const u32* b) { return static_cast<i32>(*a - *b); }

struct H {
    // ------------------------------------------------------------------------ 0x00666260
    i32 tree_check(i32 index) {
        c_tree_check.hit();
        if (jo::view<void*>(jo::self<jo::PathMesh>(this)->pairs8) == nullptr || index <= 0) return 0;
        if (index > joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathMesh>(this)->pairs8))->get_2c())) return 0;  // 0x0066b7a0
        void* const item = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathMesh>(this)->pairs8))->item(joan::orig::arg::i32(index)));  // 0x00637da0
        if (jo::view<void*>(jo::self<jo::PathMesh>(this)->points) != nullptr) {
            for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathMesh>(this)->points))->get_2c()); i > 0; --i) {
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathMesh>(this)->points))->item(joan::orig::arg::i32(i)));
                if (joan::orig::arg::as<i32>(joan::orig::as_triple(e)->overlaps_item(item)) != 0) return 0;  // 0x0066a600
            }
        }
        jo::BlkArr* jo::PathSpace::* const offs[3] = {&jo::PathSpace::pairs4, &jo::PathSpace::pairs8, &jo::PathSpace::links};  // +4 +8 +0xc
        for (i32 k = 0; k < 3; ++k) {
            void* const list = jo::self<jo::PathMesh>(this)->*offs[k];
            if (list == nullptr) continue;
            for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_path_space(list)->get_2c()); i > 0; --i) {
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::self<jo::PathMesh>(this)->*offs[k])->item(joan::orig::arg::i32(i)));
                if (joan::orig::arg::as<i32>(joan::orig::as_path_line(e)->overlaps_item(item)) != 0) return 0;  // 0x006664d0
            }
        }
        if (jo::view<void*>(jo::self<jo::PathMesh>(this)->zones) != nullptr) {
            for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathMesh>(this)->zones))->get_2c()); i > 0; --i) {
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathMesh>(this)->zones))->item(joan::orig::arg::i32(i)));
                if (joan::orig::arg::as<i32>(joan::orig::as_zone(e)->overlaps_item(item)) != 0) return 0;  // 0x0066a620
            }
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_18", name, addr, fn)
B("Tree::check_666260", 0x00666260, &H::tree_check);
B("cmp_int_667170", 0x00667170, &cmp_int_667170);
B("init_66b650", 0x0066b650, &init_66b650);
B("init_66b6b0", 0x0066b6b0, &init_66b6b0);
B("init_66b700", 0x0066b700, &init_66b700);
B("init_66b750", 0x0066b750, &init_66b750);
#undef B

}  // namespace game_b105_18
}  // namespace

// ---------------------------------------- from modules/game_b104_12.cpp (game/b104_12)
namespace {
namespace game_b104_12 {

struct H {
    i32 ret0_6664d0(void*) { return 0; }                                  // 0x006664d0
    i32 ret0_66a600(void*) { return 0; }                                // 0x0066a600
    i32 ret0_66a620(void*) { return 0; }                                // 0x0066a620
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_12", name, addr, fn)
B("X::ret0_6664d0", 0x006664d0, &H::ret0_6664d0);
B("X::ret0_66a600", 0x0066a600, &H::ret0_66a600);
B("X::ret0_66a620", 0x0066a620, &H::ret0_66a620);
#undef B

}  // namespace game_b104_12
}  // namespace

// ---------------------------------------- from modules/game_b96_07.cpp (game/b96_07)
namespace {
namespace game_b96_07 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x00666e80
    H* quad_ctor() {
        jo::self<jo::PathGroup>(this)->count = 0;
        reinterpret_cast<void(JOAN_STDCALL*)(void*, i32, i32, std::uintptr_t)>(fn::code_value(fn::vector_constructor_iterator))(
            jo::self<jo::PathGroup>(this)->entries, static_cast<i32>(sizeof(jo::Triple)), 4, fn::code_value(fn::Obj66a480_Obj));  // 0x00449af0, ctor 0x0066a480
        return this;
    }

    // ------------------------------------------------------------------ 0x0066a320
    H* rec_set(void* a, void* b, i32 c, void* d, i32 e) {
        jo::self<jo::PathRec>(this)->a = static_cast<jo::MeshVertex*>(a);
        jo::self<jo::PathRec>(this)->b = static_cast<jo::MeshVertex*>(b);
        jo::self<jo::PathRec>(this)->c = static_cast<u16>(c);
        jo::self<jo::PathRec>(this)->e = static_cast<u16>(e);
        jo::self<jo::PathRec>(this)->d = d;
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_07", name, addr, fn)
B96("Quad::Quad", 0x00666e80, &H::quad_ctor);
B96("Rec::set", 0x0066a320, &H::rec_set);
#undef B96

}  // namespace game_b96_07
}  // namespace

// ---------------------------------------- from modules/game_b99_09.cpp (game/b99_09)
namespace {
namespace game_b99_09 {

using u32 = std::uint32_t;
// ------------------------------------------------------------------------- other cdecl
void JOAN_CDECL init_pi_a6a274() {  // 0x0066b670
    const Fpu fpu{true};
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a6a274) = static_cast<float>(fpu.mul(t, global<double>(d::dbl_4)));  // 4.0
}
void JOAN_CDECL init_a6a280() {  // 0x0066b720
    global<float>(d::g_a6a280) = global<float>(d::g_a6a274) / global<float>(d::flt_3);
}

#define B99(name, addr_, fn) JOAN_REPLACE("game/b99_09", name, addr_, fn)
B99("Globals::init_pi_a6a274", 0x0066b670, &init_pi_a6a274);
B99("Globals::init_a6a280", 0x0066b720, &init_a6a280);
#undef B99

}  // namespace game_b99_09
}  // namespace

// ---------------------------------------- from modules/game_b95_07.cpp (game/b95_07)
namespace {
namespace game_b95_07 {

// ---------------------------------------------------------------------------- 0x0066b770
static void JOAN_CDECL scale_quarter_a6a284() {
    global<float>(d::g_a6a284) = global<float>(d::g_a6a274) / global<float>(d::flt_4);  // 4.0f
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_07", name, addr, fn)
B95("scale_quarter_a6a284", 0x0066b770, &scale_quarter_a6a284);
#undef B95

}  // namespace game_b95_07
}  // namespace

// ---------------------------------------- from modules/game_b98_07.cpp (game/b98_07)
namespace {
namespace game_b98_07 {

struct H {
    // ------------------------------------------------------------------ 0x0066a280
    // The MeshVertex constructor (its only callers are in PathMesh::build 0x00667190): point = v, links null.
    // v: the point.
    void* quad_ctor(void* v) {
        jo::MeshVertex* const m = jo::self<jo::MeshVertex>(this);
        m->point = static_cast<jo::PfdPoint*>(v);
        m->assist_prev = nullptr;
        m->assist_next = nullptr;
        m->prev = nullptr;
        m->next = nullptr;
        return this;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_07", name, addr, fn)
B98("Quad::Quad", 0x0066a280, &H::quad_ctor);
#undef B98

}  // namespace game_b98_07
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
namespace {
namespace game_b84_04 {

struct H {
    // ---- tiny ones ----------------------------------------------------------------------
    void* pair_assign(const std::uint32_t* src) {  // 0x0066a380
        jo::view<std::uint32_t>(jo::self<jo::Vec2i>(this)->x) = src[0];
        jo::view<std::uint32_t>(jo::self<jo::Vec2i>(this)->y) = src[1];
        return this;
    }
    void* ctor_66a480() {
        joan::orig::as_point(this)->construct();  // 0x004eea60
        return this;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("Pair::assign", 0x0066a380, &H::pair_assign);
M("Obj66a480::Obj", 0x0066a480, &H::ctor_66a480);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- from modules/game_b93_04.cpp (game/b93_04)
namespace {
namespace game_b93_04 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_within, "Point::within");

struct H {
    // ------------------------------------------------------------------ 0x0066a3c0
    i32 dot(i32 x, i32 y) {
        return static_cast<i32>(static_cast<u32>(jo::self<jo::Vec2i>(this)->x) * static_cast<u32>(x) +
                                static_cast<u32>(jo::self<jo::Vec2i>(this)->y) * static_cast<u32>(y));
    }

    // ------------------------------------------------------------------ 0x0066a530
    i32 within(i32 ax, i32 ay, i32 r) {
        c_within.hit();
        i32 l1[2];
        const i32* const q = joan::orig::arg::as<const i32*>(joan::orig::as_vec2i(this)->copy_to(joan::orig::arg::mptr(static_cast<void*>(l1))));  // 0x0066a4f0
        const i32 qy = q[1];
        const i32 qx = q[0];
        i32 l2[2];
        void* const d = joan::orig::p2::sub(static_cast<void*>(l2), P2{ax, ay}, P2{qx, qy});  // 0x006639e0
        const i32 len2 = joan::orig::arg::as<i32>(joan::orig::as_vec2i(d)->length_sq());                                         // 0x0066a440
        const i32 rr = static_cast<i32>(static_cast<u32>(r) * static_cast<u32>(r));
        return len2 <= rr ? 1 : 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("Point::dot", 0x0066a3c0, &H::dot);
B93("Point::within", 0x0066a530, &H::within);
#undef B93

}  // namespace game_b93_04
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
namespace {
namespace game_b90_06 {

struct H {
    // 0x0066a400 2D cross product with (a, b): x * b - y * a.
    std::int32_t cross(std::int32_t a, std::int32_t b) {
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::self<jo::Vec2i>(this)->x) * static_cast<std::uint32_t>(b) -
                                         static_cast<std::uint32_t>(jo::self<jo::Vec2i>(this)->y) * static_cast<std::uint32_t>(a));
    }
    // 0x0066a5e0
    std::int32_t stub_zero(void*) { return 0; }  // the facade passes a pointer (Phase 2 unplaced-b-2)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Vec2i::cross", 0x0066a400, &H::cross);
B("stub_zero", 0x0066a5e0, &H::stub_zero);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_b94_07.cpp (game/b94_07)
namespace {
namespace game_b94_07 {

struct H {
    // ------------------------------------------------------------------ 0x0066a5a0
    i32 set3(i32 x, i32 y, i32 z) {
        i32 pair[2];
        pair[0] = x;
        pair[1] = y;
        joan::orig::as_vec2i(this)->assign(joan::orig::arg::mptr(static_cast<void*>(pair)));  // 0x0066a380
        jo::self<jo::Point>(this)->z = z;
        return 1;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("Point::set3", 0x0066a5a0, &H::set3);
#undef B94

}  // namespace game_b94_07
}  // namespace

// ---------------------------------------- from modules/game_b93_04.cpp (game/b93_04)
namespace {
namespace game_b93_04_2 {

// Integer point built with 0x004eeb80 (x, y) on a stack local, then copied to `out`.
void* make_point(void* out, i32 x, i32 y) {
    i32 local[2];
    const i32* const p = joan::orig::arg::as<const i32*>(joan::orig::as_point(local)->set(joan::orig::arg::u32(x), joan::orig::arg::u32(y)));  // 0x004eeb80
    const i32 px = p[0];
    const i32 py = p[1];
    static_cast<i32*>(out)[0] = px;
    static_cast<i32*>(out)[1] = py;
    return out;
}

// ---------------------------------------------------------------------------- 0x00663990
static void* JOAN_CDECL point_add(void* out, i32 ax, i32 ay, i32 bx, i32 by) {
    const i32 y = static_cast<i32>(static_cast<u32>(ay) + static_cast<u32>(by));
    const i32 x = static_cast<i32>(static_cast<u32>(ax) + static_cast<u32>(bx));
    return make_point(out, x, y);
}

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("Point::add", 0x00663990, &point_add);
#undef B93

}  // namespace game_b93_04_2
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
namespace {
namespace game_b103_14_2 {

// ------------------------------------------------------------------ 0x00663a80
struct Point2 {
    i32 x, y;
};
Point2* JOAN_CDECL scaled_point(Point2* out, float k, i32 x, i32 y) {
    const i32 iy = joan::hook::x87::ftol(Fpu{true}.mul(static_cast<double>(y), static_cast<double>(k)));
    const i32 ix = joan::hook::x87::ftol(Fpu{true}.mul(static_cast<double>(x), static_cast<double>(k)));
    Point2 tmp;
    const Point2* const p = joan::orig::arg::as<Point2*>(joan::orig::as_point(&tmp)->set(joan::orig::arg::u32(ix), joan::orig::arg::u32(iy)));  // 0x004eeb80
    const i32 px = p->x;
    const i32 py = p->y;
    out->x = px;
    out->y = py;
    return out;
}

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("scaled_point", 0x00663a80, &scaled_point);
#undef B103

}  // namespace game_b103_14_2
}  // namespace

// ---------------------------------------- from modules/game_b95_07.cpp (game/b95_07)
namespace {
namespace game_b95_07_2 {

JOAN_COUNTER(c_rect_dist2, "point_rect_dist2");

// ---------------------------------------------------------------------------- 0x00663ad0
static i32 JOAN_CDECL point_rect_dist2(i32 x, i32 y, const i32* lo, const i32* hi) {
    c_rect_dist2.hit();
    i32 dx;
    if (x < lo[0]) {
        dx = static_cast<i32>(static_cast<u32>(x) - static_cast<u32>(lo[0]));
    } else if (x > hi[0]) {
        dx = static_cast<i32>(static_cast<u32>(x) - static_cast<u32>(hi[0]));
    } else {
        dx = 0;
    }
    i32 dy;
    if (y < lo[1]) {
        dy = static_cast<i32>(static_cast<u32>(y) - static_cast<u32>(lo[1]));
    } else if (y > hi[1]) {
        dy = static_cast<i32>(static_cast<u32>(y) - static_cast<u32>(hi[1]));
    } else {
        dy = 0;
    }
    return static_cast<i32>(static_cast<u32>(dx) * static_cast<u32>(dx) + static_cast<u32>(dy) * static_cast<u32>(dy));
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_07", name, addr, fn)
B95("point_rect_dist2", 0x00663ad0, &point_rect_dist2);
#undef B95

}  // namespace game_b95_07_2
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
namespace {
namespace game_b84_04_2 {

struct H {
    void wrap_663bd0() { joan::orig::as_path_node(this)->free_buffer(); }  // 0x00663c00
    void* ctor_663b90() {
        joan::orig::as_point(&jo::self<jo::Buffer>(this)->cx)->construct();  // 0x004eea60
        // this = &PfdZone::search_stamp (the only caller is the PfdZone constructor 0x00663c50): clears up to path_space
        // (0x34 bytes on x86). x64: needs Buffer+0xc ptr:PfdZone (PfdZone::open_next) so Buffer matches this range.
        std::memset(static_cast<void*>(this), 0, offsetof(jo::PfdZone, path_space) - offsetof(jo::PfdZone, search_stamp));  // memset
        return this;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("Obj663b90::Obj", 0x00663b90, &H::ctor_663b90);
M("Obj663bd0::wrap", 0x00663bd0, &H::wrap_663bd0);
#undef M

}  // namespace game_b84_04_2
}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
namespace {
namespace game_b83_04_3 {

struct H {
    // 0x00663f30 (ret 4)
    H* assign(const std::uint8_t* other) {
        joan::orig::as_vec2i(this)->assign(joan::orig::arg::mptr(other));  // 0x0066a380
        jo::view<std::uint32_t>(jo::self<jo::Point>(this)->z) = jo::view<std::uint32_t>(jo::as_point(other)->z);
        return this;
    }
};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("Obj663f30::assign", 0x00663f30, &H::assign);
#undef B83_04

}  // namespace game_b83_04_3
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::point::add(void* a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b93_04_2::point_add>(a0, a1, a2, a3, a4)); }  // 0x663990
auto joan::orig::point_rect_dist2(std::int32_t a0, std::int32_t a1, void* a2, void* a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_07_2::point_rect_dist2>(a0, a1, a2, a3)); }  // 0x663ad0
auto joan::orig::PathNode::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_04_2::H::ctor_663b90>(this)); }  // 0x663b90
auto joan::orig::PathNode::destruct() -> void { joan::bind::member<&::game_b84_04_2::H::wrap_663bd0>(this); }  // 0x663bd0
auto joan::orig::Triple::assign(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_04_3::H::assign>(this, a0)); }  // 0x663f30
auto joan::orig::Zone::not_after(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_07::H::not_after>(this, a0)); }  // 0x664b00
auto joan::orig::PathSpace::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_light1::H::pfd_construct>(this)); }  // 0x664fd0
auto joan::orig::BlkArr::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_14::H::item_delete>(this, a0)); }  // 0x6651e0
auto joan::orig::HeightGrid::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_04::H::delete_665430>(this, a0)); }  // 0x665430
auto joan::orig::PathLine::construct(Triple* a0, Triple* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::pair_ctor>(this, a0, a1)); }  // 0x665c20
auto joan::orig::PathLine::overlaps_item(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_12::H::ret0_6664d0>(this, a0)); }  // 0x6664d0
auto joan::orig::PathGroup::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_07::H::quad_ctor>(this)); }  // 0x666e80
auto joan::orig::MeshVertex::construct(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_07::H::quad_ctor>(this, a0)); }  // 0x66a280
auto joan::orig::MeshVertex::position(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_07::H::get_pair_ind>(this, a0)); }  // 0x66a2e0
auto joan::orig::MeshEdge::set(void* a0, void* a1, std::int32_t a2, void* a3, std::int32_t a4) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_07::H::rec_set>(this, a0, a1, a2, a3, a4)); }  // 0x66a320
auto joan::orig::Vec2i::assign(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_04::H::pair_assign>(this, a0)); }  // 0x66a380
auto joan::orig::Vec2i::dot(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_04::H::dot>(this, a0, a1)); }  // 0x66a3c0
auto joan::orig::Vec2i::cross(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_06::H::cross>(this, a0, a1)); }  // 0x66a400
auto joan::orig::Vec2i::length_sq() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_light1::H::length_sq>(this)); }  // 0x66a440
auto joan::orig::Triple::construct_default() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_04::H::ctor_66a480>(this)); }  // 0x66a480
auto joan::orig::Triple::construct(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::triple_ctor>(this, a0, a1, a2)); }  // 0x66a4b0
auto joan::orig::Vec2i::copy_to(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_light1::H::copy_to>(this, a0)); }  // 0x66a4f0
auto joan::orig::Triple::contains(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_04::H::within>(this, a0, a1, a2)); }  // 0x66a530
auto joan::orig::Triple::set(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_07::H::set3>(this, a0, a1, a2)); }  // 0x66a5a0
auto joan::orig::Triple::ret0_66a5e0(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_06::H::stub_zero>(this, a0)); }  // 0x66a5e0
auto joan::orig::Triple::overlaps_item(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_12::H::ret0_66a600>(this, a0)); }  // 0x66a600
auto joan::orig::Zone::overlaps_item(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_12::H::ret0_66a620>(this, a0)); }  // 0x66a620
auto joan::orig::init_pi_a6a274() -> void { joan::bind::free<&::game_b99_09::init_pi_a6a274>(); }  // 0x66b670
auto joan::orig::scale_a6a27c() -> void { joan::bind::free<&::game_b100_14::scale_a6a27c>(); }  // 0x66b6d0
auto joan::orig::init_a6a280() -> void { joan::bind::free<&::game_b99_09::init_a6a280>(); }  // 0x66b720
auto joan::orig::scale_quarter_a6a284() -> void { joan::bind::free<&::game_b95_07::scale_quarter_a6a284>(); }  // 0x66b770
