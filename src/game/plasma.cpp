// game/plasma.cpp: functions of the original d:\ww\plasma.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdarg>
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b102_04.cpp (game/b102_04)
//
//    - HeightMap::copy_ctor 0x004c56f0 (src), ret 4 -> this / HeightMap::assign 0x004c57b0
//      (src), ret 4 -> this (0x4c5960 first): memcpy 0x14 header, new word array
//      (w + 1) * (h + 1) * 2 ("plasma.cpp"), memcpy of the source cells.
namespace {
namespace game_b102_04 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

void* const kHeap = obj(d::g_mem);

using joan::orig::helpers::crt_memcpy;

JOAN_COUNTER(c_hm_copy, "HeightMap::copy_ctor");
JOAN_COUNTER(c_hm_assign, "HeightMap::assign");

struct H {
    // ------------------------------------------------------------------ 0x004c56f0 / 0x004c57b0
    void hm_copy_from(const u8* src, i32 line) {
        crt_memcpy(this, src, sizeof(jo::HeightMap));  // 0x14 on x86: the header; cells is replaced below
        const i32 n1 = (static_cast<i32>(jo::self<jo::HeightMap>(this)->width) + 1) * (static_cast<i32>(jo::self<jo::HeightMap>(this)->height) + 1) * 2;
        jo::view<void*>(jo::self<jo::HeightMap>(this)->cells) = joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(n1), str(d::s_d_ww_plasma_cpp), joan::orig::arg::i32(line));  // 0x0058e7b0 "plasma.cpp"
        const i32 n2 = (static_cast<i32>(jo::self<jo::HeightMap>(this)->width) + 1) * (static_cast<i32>(jo::self<jo::HeightMap>(this)->height) + 1) * 2;
        crt_memcpy(jo::view<void*>(jo::self<jo::HeightMap>(this)->cells), jo::view<void*>(jo::as_height_map(src)->cells), static_cast<u32>(n2));
    }
    H* hm_copy(const u8* src) {
        c_hm_copy.hit();
        hm_copy_from(src, static_cast<i32>(global<i16>(d::g_a02414)) + 2);
        return this;
    }
    H* hm_assign(const u8* src) {
        c_hm_assign.hit();
        joan::orig::as_height_grid(this)->free_4c5960();  // 0x004c5960
        hm_copy_from(src, static_cast<i32>(global<i16>(d::g_a02418)) + 4);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("HeightMap::copy_ctor", 0x004c56f0, &H::hm_copy);
B("Plasma::operator=", 0x004c57b0, &H::hm_assign);
#undef B

}  // namespace game_b102_04
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
//
//    - HeightGrid::HeightGrid 0x004c56b0, HeightGrid::free 0x004c5870, HeightGrid::create
//      0x004c58a0 (w, h, fill), Misc::format_int 0x004d69a0, empty 0x004d69f0,
//      TextureList::find 0x004dff70 (1-based index, 0 when not found).
namespace {
namespace game_leaves82b {

JOAN_COUNTER(c_grid_create, "HeightGrid::create");

struct H {
    std::uint32_t grid_free() { joan::orig::as_height_grid(this)->free_4c5960(); return 0; }  // 0x004c5960
    // 0x004c58a0 HeightGrid::create(w, h, fill): (w+1)*(h+1) shorts allocated (plasma.cpp) and
    // filled with fill.
    std::uint32_t grid_create(std::int16_t w, std::int16_t h, std::int16_t fill) {
        c_grid_create.hit();
        joan::orig::as_height_grid(this)->free_4c5960();  // 0x004c5960
        jo::self<jo::HeightGrid>(this)->width = w;
        jo::self<jo::HeightGrid>(this)->height = h;
        const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_a0241c)) + 4;
        const std::uint32_t count = static_cast<std::uint32_t>(static_cast<std::int32_t>(w) + 1) *
                                    static_cast<std::uint32_t>(static_cast<std::int32_t>(h) + 1);
        jo::self<jo::HeightGrid>(this)->cells = static_cast<jo::ArrU16*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(count << 1), str(d::s_d_ww_plasma_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        joan::orig::as_misc(obj(d::g_misc))->fill_word_591960(joan::orig::arg::mptr(jo::self<jo::HeightGrid>(this)->cells), static_cast<std::int32_t>(fill),
                                                                joan::orig::arg::i32(count));  // 0x00591960
        return 0;  // void callee; HeightGrid::create's callers drop the eax
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Plasma::~Plasma", 0x004c5870, &H::grid_free);
L82("Plasma::init", 0x004c58a0, &H::grid_create);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
//
//    - TrackRes::free 0x004eba00 / CamRes::free 0x00428d50 (if loaded: release, free +0 with
//      0x0058ed70, clear), Plasma::free 0x004c5960, TrackRes::load 0x004eba90 /
//      CamRes::load 0x00428de0 (id + 12-byte value by value, then loaded = 1).
namespace {
namespace game_b82_18 {

// Memory::free(ptr, file, line) on the allocator 0x00a48838.
void mem_free(void* p, const char* file, std::int32_t line) {
    joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

struct H {
    void plasma_free() {  // 0x004c5960
        if (jo::self<jo::HeightGrid>(this)->cells != nullptr) {
            mem_free(jo::self<jo::HeightGrid>(this)->cells, str(d::s_d_ww_plasma_cpp),  // plasma.cpp
                     static_cast<std::int32_t>(global<std::int16_t>(d::g_a02420)) + 2);
        }
        jo::self<jo::HeightGrid>(this)->cells = nullptr;
        jo::view<std::uint16_t>(jo::self<jo::HeightGrid>(this)->height) = 0;
        jo::view<std::uint16_t>(jo::self<jo::HeightGrid>(this)->width) = 0;
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("Plasma::deinit", 0x004c5960, &H::plasma_free);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_leaves82b_2.cpp (game/leaves82b)
namespace {
namespace game_leaves82b_2 {

struct H {
    // ---------------------------------------------------------------- height grid
    void* grid_ctor() {  // 0x004c56b0
        jo::view<std::uint16_t>(jo::self<jo::HeightGrid>(this)->height) = 0;
        jo::view<std::uint16_t>(jo::self<jo::HeightGrid>(this)->width) = 0;
        jo::self<jo::HeightGrid>(this)->cells = nullptr;
        return this;
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("HeightGrid::HeightGrid", 0x004c56b0, &H::grid_ctor);
#undef L82

}  // namespace game_leaves82b_2
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

struct H {
    // 0x004c59d0 HeightGrid::at(i, j): +4 shorts, row stride +0 + 1. Returns the whole eax
    // (upper half = upper half of the index, as the original's mov ax).
    std::uint32_t grid_at(std::int32_t i, std::int32_t j) {
        const std::uint32_t idx =
            static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(j))) *
                static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::HeightGrid>(this)->width) + 1) +
            static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(i)));
        // the original's byte offset idx * 2, computed through the element size (indexing changes the i686 code)
        const std::uint16_t v = reinterpret_cast<const jo::ArrU16*>(reinterpret_cast<const std::uint8_t*>(jo::self<jo::HeightGrid>(this)->cells) + static_cast<std::int32_t>(idx * static_cast<std::uint32_t>(sizeof(jo::ArrU16))))->value;
        return (idx & 0xffff0000u) | v;
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("Plasma::get_pix", 0x004c59d0, &H::grid_at);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b82_18_2.cpp (game/b82_18)
namespace {
namespace game_b82_18_2 {

struct H {
    void grid_set(std::uint32_t x, std::uint32_t y, std::uint16_t v) {  // 0x004c5a20
        const std::uint32_t row = static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(y)));
        const std::uint32_t w = static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::HeightGrid>(this)->width)) + 1u;
        const std::uint32_t idx = row * w + static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(x)));
        reinterpret_cast<std::uint16_t*>(jo::view<std::uint8_t*>(jo::self<jo::HeightGrid>(this)->cells) + idx * 2u)[0] = v;
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("Plasma::plot", 0x004c5a20, &H::grid_set);
#undef B82

}  // namespace game_b82_18_2
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using joan::orig::helpers::addr;
using joan::orig::helpers::wadd;

JOAN_COUNTER(c_grid_seed, "Grid::seed_4c5a70");

struct H {
    // 0x004c5a70 (dead); eax: a subdivide result or this
    std::uintptr_t seed_4c5a70(i32 a1, i32 a2, u32 seed) {
        c_grid_seed.hit();
        jo::view<i32>(jo::self<jo::Plasma>(this)->roughness) = static_cast<i32>(static_cast<u32>(a2) << 3);
        joan::orig::libc::srand(joan::orig::arg::u32(seed));  // srand
        i16 w[4] = {0, 0, 0, 0};
        for (i32 i = 0; i < 4; ++i) {
            const i32 r = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->rand_5905a0());  // 0x005905a0 rand
            const i32 q = r / 256;
            const i32 m = static_cast<i32>(static_cast<u32>(q) * 0xffu);
            w[i] = static_cast<i16>((m >> 7) + 1);
        }
        joan::orig::as_height_grid(this)->set(0u, 0u, joan::orig::arg::u16(w[0]));                    // 0x004c5a20
        joan::orig::as_height_grid(this)->set(joan::orig::arg::u32(jo::self<jo::Plasma>(this)->width), 0u, joan::orig::arg::u16(w[1]));
        joan::orig::as_height_grid(this)->set(joan::orig::arg::u32(jo::self<jo::Plasma>(this)->width), joan::orig::arg::u32(jo::self<jo::Plasma>(this)->height), joan::orig::arg::u16(w[2]));
        joan::orig::as_height_grid(this)->set(0u, joan::orig::arg::u32(jo::self<jo::Plasma>(this)->height), joan::orig::arg::u16(w[3]));
        jo::self<jo::Plasma>(this)->depth = 0;
        if (a1 == 0) {
            return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_terrain(this)->subdivide_4c5c30(0, 0, static_cast<i32>(jo::self<jo::Plasma>(this)->width), static_cast<i32>(jo::self<jo::Plasma>(this)->height)));  // 0x004c5c30
        }
        i32 k = 1;
        i32 j = k;
        jo::self<jo::Plasma>(this)->scale = j;
        for (;;) {
            const u32 r = joan::orig::arg::as<u32>(joan::orig::as_terrain(this)->subdivide(0, 0, static_cast<i32>(jo::self<jo::Plasma>(this)->width), static_cast<i32>(jo::self<jo::Plasma>(this)->height), joan::orig::arg::i32(j)));  // 0x004c5f10
            if (r != 0) return r;
            k = static_cast<i32>(static_cast<u32>(k) << 1);
            if (k > static_cast<i32>(jo::self<jo::Plasma>(this)->width) && k > static_cast<i32>(jo::self<jo::Plasma>(this)->height)) return addr(this);
            j = wadd(j, 1);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Plasma::generate", 0x004c5a70, &H::seed_4c5a70);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using u8 = std::uint8_t;
using u16 = std::uint16_t;

i32 sx16(i32 v) { return static_cast<i16>(v); }

JOAN_COUNTER(c_subdivide, "Terrain::subdivide_4c5c30");

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x004c5c30
    i16 hget(i32 x, i32 y) { return joan::orig::arg::as<i16>(joan::orig::as_height_grid(me())->at(joan::orig::arg::i32(sx16(x)), joan::orig::arg::i32(sx16(y)))); }  // 0x004c59d0
    i32 hmid(i32 a, i32 b, i32 c, i32 d, i32 e, i32 f) {
        return joan::orig::arg::as<u16>(joan::orig::as_height_grid(me())->smooth(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::i16(c), joan::orig::arg::i16(d), joan::orig::arg::i16(e), joan::orig::arg::i16(f)));  // 0x004c6530, and eax,0xffff
    }
    std::uintptr_t subdivide(i32 x0, i32 y0, i32 x1, i32 y1) {  // eax: this
        c_subdivide.hit();
        auto wadd = [](i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) + static_cast<u32>(b)); };
        auto wsub = [](i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) - static_cast<u32>(b)); };
        if (wsub(x1, x0) < 2 && wsub(y1, y0) < 2) return static_cast<u32>(wsub(x1, x0));
        jo::self<jo::HeightMap>(this)->depth += 1u;
        jo::self<jo::HeightMap>(this)->scale = 0x140 >> (jo::self<jo::HeightMap>(this)->depth & 31);
        const i32 mx = wadd(x0, x1) >> 1;
        const i32 my = wadd(y0, y1) >> 1;
        i32 v = hget(mx, y0);
        if (v == 0) v = hmid(x0, y0, mx, y0, x1, y0);
        i32 sum = v;
        v = hget(x1, my);
        if (v == 0) v = hmid(x1, y0, x1, my, x1, y1);
        sum = wadd(sum, v);
        v = hget(mx, y1);
        if (v == 0) v = hmid(x0, y1, mx, y1, x1, y1);
        sum = wadd(sum, v);
        v = hget(x0, my);
        if (v == 0) v = hmid(x0, y0, x0, my, x0, y1);
        sum = wadd(sum, v);
        if (hget(mx, my) == 0)
            joan::orig::as_height_grid(me())->set(joan::orig::arg::u32(sx16(mx)), joan::orig::arg::u32(sx16(my)), joan::orig::arg::u16(wadd(sum, 2) >> 2));  // 0x004c5a20
        joan::orig::as_terrain(me())->subdivide_4c5c30(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(mx), joan::orig::arg::i32(my));  // 0x004c5c30
        joan::orig::as_terrain(me())->subdivide_4c5c30(joan::orig::arg::i32(mx), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(my));
        joan::orig::as_terrain(me())->subdivide_4c5c30(joan::orig::arg::i32(mx), joan::orig::arg::i32(my), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1));
        joan::orig::as_terrain(me())->subdivide_4c5c30(joan::orig::arg::i32(x0), joan::orig::arg::i32(my), joan::orig::arg::i32(mx), joan::orig::arg::i32(y1));
        jo::self<jo::HeightMap>(this)->depth -= 1u;
        return reinterpret_cast<std::uintptr_t>(me());
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("Terrain::subdivide_4c5c30", 0x004c5c30, &H::subdivide);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b103_04.cpp (game/b103_04)
namespace {
namespace game_b103_04 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using u8 = std::uint8_t;

using joan::hook::global;

i32 add(i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) + static_cast<u32>(b)); }

// Raw global accesses (unaligned / overlapping arrays in 0xa3a07x..0xa3a11x).
i32 ld32(img::Addr a) {
    i32 v;
    std::memcpy(&v, img::obj(a), 4);
    return v;
}
void st32(img::Addr a, i32 v) { std::memcpy(img::obj(a), &v, 4); }
u8 ld8(img::Addr a) { return img::global<volatile u8>(a); }
void st8(img::Addr a, u8 v) { img::global<volatile u8>(a) = v; }

JOAN_COUNTER(c_subdivide, "Terrain::subdivide");

struct H {
    // ------------------------------------------------------------------------ 0x004c5f10
    i16 cell(i32 a, i32 b) {  // 0x004c59d0
        return joan::orig::arg::as<i16>(joan::orig::as_height_grid(this)->at(joan::orig::arg::i32(static_cast<i16>(a)), joan::orig::arg::i32(static_cast<i16>(b))));
    }
    i32 fill(i32 a, i32 b, i32 c, i32 d, i32 e, i32 g) {  // 0x004c6530
        return static_cast<i32>(joan::orig::arg::as<u32>(joan::orig::as_height_grid(this)->smooth(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::i16(c), joan::orig::arg::i16(d), joan::orig::arg::i16(e), joan::orig::arg::i16(g))) & 0xffffu);
    }
    i32 subdivide(i32 a8, i32 ac, i32 a10, i32 a14, i32 depth) {
        c_subdivide.hit();
        jo::self<jo::HeightMap>(this)->scale = 0x140 >> (depth & 31);
        st8(d::g_a3a080, 2);
        st32(d::g_a3a081, a14);
        i32 l1c = ld32(d::g_a3a081);
        st32(d::g_a3a089, ac);
        i32 l18 = ld32(d::g_a3a089);
        st8(d::g_a3a0c3, 0);
        st8(d::g_a3a0c1, ld8(d::g_a3a0c3));
        st8(d::g_a3a0c2, 1);
        st32(d::g_a3a085, add(l18, l1c) >> 1);
        i32 l0c = ld32(d::g_a3a085);
        i32 l8 = 0, l10 = 0, l14 = 0;
        while (ld8(d::g_a3a080) >= 1) {
            while (static_cast<i32>(ld8(d::g_a3a0c0 + ld8(d::g_a3a080))) < depth) {
                st8(d::g_a3a080, static_cast<u8>(ld8(d::g_a3a080) + 1));
                st32(d::g_a3a081 + ld8(d::g_a3a080) * 4u, ld32(d::g_a3a07d + ld8(d::g_a3a080) * 4u));
                l18 = ld32(d::g_a3a081 + ld8(d::g_a3a080) * 4u);
                l1c = ld32(d::g_a3a079 + ld8(d::g_a3a080) * 4u);
                st8(d::g_a3a0c1 + ld8(d::g_a3a080), ld8(d::g_a3a0c0 + ld8(d::g_a3a080)));
                st32(d::g_a3a07d + ld8(d::g_a3a080) * 4u, add(l18, l1c) >> 1);
                l0c = ld32(d::g_a3a07d + ld8(d::g_a3a080) * 4u);
                u8 t;
                if (ld8(d::g_a3a0c1 + ld8(d::g_a3a080)) > ld8(d::g_a3a0bf + ld8(d::g_a3a080)))
                    t = ld8(d::g_a3a0c1 + ld8(d::g_a3a080));
                else
                    t = ld8(d::g_a3a0bf + ld8(d::g_a3a080));
                st8(d::g_a3a0c0 + ld8(d::g_a3a080), static_cast<u8>(t + 1));
            }
            st8(d::g_a3a0d8, 2);
            st32(d::g_a3a0d9, a10);
            l14 = ld32(d::g_a3a0d9);
            st32(d::g_a3a0e1, a8);
            l10 = ld32(d::g_a3a0e1);
            st8(d::g_a3a11b, 0);
            st8(d::g_a3a119, ld8(d::g_a3a11b));
            st8(d::g_a3a11a, 1);
            st32(d::g_a3a0dd, add(l10, l14) >> 1);
            l8 = ld32(d::g_a3a0dd);
            while (ld8(d::g_a3a0d8) >= 1) {
                while (static_cast<i32>(ld8(d::g_a3a118 + ld8(d::g_a3a0d8))) < depth) {
                    st8(d::g_a3a0d8, static_cast<u8>(ld8(d::g_a3a0d8) + 1));
                    st32(d::g_a3a0d9 + ld8(d::g_a3a0d8) * 4u, ld32(d::g_a3a0d5 + ld8(d::g_a3a0d8) * 4u));
                    l10 = ld32(d::g_a3a0d9 + ld8(d::g_a3a0d8) * 4u);
                    l14 = ld32(d::g_a3a0d1 + ld8(d::g_a3a0d8) * 4u);
                    st8(d::g_a3a119 + ld8(d::g_a3a0d8), ld8(d::g_a3a118 + ld8(d::g_a3a0d8)));
                    st32(d::g_a3a0d5 + ld8(d::g_a3a0d8) * 4u, add(l10, l14) >> 1);
                    l8 = ld32(d::g_a3a0d5 + ld8(d::g_a3a0d8) * 4u);
                    u8 t;
                    if (ld8(d::g_a3a119 + ld8(d::g_a3a0d8)) > ld8(d::g_a3a117 + ld8(d::g_a3a0d8)))
                        t = ld8(d::g_a3a119 + ld8(d::g_a3a0d8));
                    else
                        t = ld8(d::g_a3a117 + ld8(d::g_a3a0d8));
                    st8(d::g_a3a118 + ld8(d::g_a3a0d8), static_cast<u8>(t + 1));
                }
                i32 v20 = cell(l14, l0c);
                if (v20 == 0) v20 = fill(l14, l18, l14, l0c, l14, l1c);
                i32 v24 = v20;
                v20 = cell(l8, l1c);
                if (v20 == 0) v20 = fill(l10, l1c, l8, l1c, l14, l1c);
                v24 = add(v24, v20);
                if (cell(l8, l0c) == 0) {
                    v20 = cell(l8, l18);
                    if (v20 == 0) v20 = fill(l10, l18, l8, l18, l14, l18);
                    v24 = add(v24, v20);
                    v20 = cell(l10, l0c);
                    if (v20 == 0) v20 = fill(l10, l18, l10, l0c, l10, l1c);
                    v24 = add(v24, v20);
                    joan::orig::as_height_grid(this)->set(joan::orig::arg::u32(static_cast<i16>(l8)), joan::orig::arg::u32(static_cast<i16>(l0c)), joan::orig::arg::u16(add(v24, 2) >> 2));  // 0x004c5a20
                }
                if (static_cast<i32>(ld8(d::g_a3a118 + ld8(d::g_a3a0d8))) == depth)
                    st8(d::g_a3a0d8, static_cast<u8>(ld8(d::g_a3a0d8) - 2));
            }
            if (static_cast<i32>(ld8(d::g_a3a0c0 + ld8(d::g_a3a080))) == depth)
                st8(d::g_a3a080, static_cast<u8>(ld8(d::g_a3a080) - 2));
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("Terrain::subdivide", 0x004c5f10, &H::subdivide);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
namespace {
namespace game_b92_03 {

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_smooth, "HeightMap::smooth");

struct H {
    // 0x004c6530
    std::int16_t smooth(std::int16_t a, std::int16_t b, std::int16_t c, std::int16_t d, std::int16_t e,
                        std::int16_t g) {
        c_smooth.hit();
        const std::int32_t r = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(obj(d::g_misc))->rand_5905a0());  // 0x005905a0
        std::uint32_t v = (static_cast<std::uint32_t>(r) - 0x3fffu) * jo::self<jo::HeightMap>(this)->roughness;
        v = v * jo::view<std::uint32_t>(jo::self<jo::HeightMap>(this)->scale);
        std::int32_t s = static_cast<std::int32_t>(v) >> 0x12;
        const std::int32_t h1 = joan::orig::arg::as<std::int16_t>(joan::orig::as_height_grid(this)->at(joan::orig::arg::i32(a), joan::orig::arg::i32(b)));  // 0x004c59d0
        const std::int32_t h2 = joan::orig::arg::as<std::int16_t>(joan::orig::as_height_grid(this)->at(joan::orig::arg::i32(e), joan::orig::arg::i32(g)));
        s = static_cast<std::int32_t>(static_cast<std::uint32_t>((h1 + h2 + 1) >> 1) + static_cast<std::uint32_t>(s));
        if (s >= 0x100) s = 0xff;
        if (s < 1) s = 1;
        joan::orig::as_height_grid(this)->set(joan::orig::arg::u32(c), joan::orig::arg::u32(d), joan::orig::arg::u16(static_cast<std::int16_t>(s)));  // 0x004c5a20
        return static_cast<std::int16_t>(s);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Plasma::adjust", 0x004c6530, &H::smooth);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b102_04_2.cpp (game/b102_04)
namespace {
namespace game_b102_04_2 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

JOAN_COUNTER(c_hm_add, "HeightMap::add_clamp");
JOAN_COUNTER(c_hm_hist, "HeightMap::histogram");

struct H {
    // ------------------------------------------------------------------ 0x004c6620
    i32 hm_add(i32 d) {
        c_hm_add.hit();
        const i32 h1 = static_cast<i32>(jo::self<jo::HeightMap>(this)->height) + 1;
        const i32 n = (static_cast<i32>(jo::self<jo::HeightMap>(this)->width) + 1) * h1;
        i16* p = jo::view<i16*>(jo::self<jo::HeightMap>(this)->cells);
        i32 i = 0;
        for (; i < n; ++i, ++p) {
            *p = static_cast<i16>(static_cast<u16>(static_cast<u16>(*p) + static_cast<u16>(d)));
            if (static_cast<i32>(*p) < 1) *p = 1;
            if (static_cast<i32>(*p) >= 0x100) *p = 0xff;
        }
        return i > 0 ? i : h1;
    }

    // ------------------------------------------------------------------ 0x004c66e0
    i32 hm_histogram(i32 n, const i16* thresholds, i32* counts) {
        c_hm_hist.hit();
        const i32 cells = (static_cast<i32>(jo::self<jo::HeightMap>(this)->width) + 1) * (static_cast<i32>(jo::self<jo::HeightMap>(this)->height) + 1);
        for (i32 j = n - 1; j >= 0; --j) counts[j] = 0;
        for (i32 i = 0; i < cells; ++i) {
            const i32 v = jo::view<i16*>(jo::self<jo::HeightMap>(this)->cells)[i];
            for (i32 j = n - 1; j >= 0; --j) {
                if (v >= static_cast<i32>(thresholds[j])) {
                    counts[j] = static_cast<i32>(static_cast<u32>(counts[j]) + 1u);
                    break;
                }
            }
        }
        return cells;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("HeightMap::add_clamp", 0x004c6620, &H::hm_add);
B("HeightMap::histogram", 0x004c66e0, &H::hm_histogram);
#undef B

}  // namespace game_b102_04_2
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

JOAN_COUNTER(c_grid, "Grid::init_4c67e0");

struct H {
    // -------------------------------------------------------------- 0x004c67e0
    u32 grid_init(i32 a1, u32 a2, u32 seed) {
        c_grid.hit();
        jo::self<jo::Plasma>(this)->roughness = a2 << 3;
        joan::orig::libc::srand(joan::orig::arg::u32(seed));  // srand
        jo::self<jo::Plasma>(this)->depth = 0;
        if (a1 == 0) {
            return joan::orig::arg::as<u32>(joan::orig::as_terrain(this)->subdivide_4c5c30(i32{0}, i32{0}, static_cast<i32>(jo::self<jo::Plasma>(this)->width), joan::orig::arg::i32(// 0x004c5c30
                                 static_cast<i32>(jo::self<jo::Plasma>(this)->height))));
        }
        i32 size = 1;
        i32 i = size;
        jo::self<jo::Plasma>(this)->scale = i;
        for (;; ++i) {
            const u32 r = joan::orig::arg::as<u32>(joan::orig::as_terrain(this)->subdivide(i32{0}, i32{0}, static_cast<i32>(jo::self<jo::Plasma>(this)->width), joan::orig::arg::i32(// 0x004c5f10
                                        static_cast<i32>(jo::self<jo::Plasma>(this)->height)), joan::orig::arg::i32(i)));
            if (r != 0) return r;
            size = static_cast<i32>(static_cast<u32>(size) << 1);
            const i32 w0 = jo::self<jo::Plasma>(this)->width;
            if (size > w0 && size > static_cast<i32>(jo::self<jo::Plasma>(this)->height)) return static_cast<u32>(w0);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("Grid::init_4c67e0", 0x004c67e0, &H::grid_init);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b104_04.cpp (game/b104_04)
namespace {
namespace game_b104_04 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_4c68d0, "HeightMap::smooth_4c68d0");

struct H {
    // ---------------------------------------------------------------- 0x004c68d0
    i16 cell(i32 x, i32 y) { return joan::orig::arg::as<i16>(joan::orig::as_height_grid(this)->at(joan::orig::arg::i32(x), joan::orig::arg::i32(y))); }  // 0x004c59d0
    void smooth_4c68d0(u32 lo_arg, u32 hi_arg, u32 k_arg) {
        c_4c68d0.hit();
        const i32 lo = static_cast<i16>(lo_arg);
        const i32 hi = static_cast<i16>(hi_arg);
        const i32 k = static_cast<i16>(k_arg);
        for (i32 i = 1; i < jo::self<jo::HeightMap>(this)->height; ++i) {
            for (i32 j = 1; j < jo::self<jo::HeightMap>(this)->width; ++j) {
                static const i32 dx[9] = {0, -1, 0, 1, -1, 1, -1, 0, 1};
                static const i32 dy[9] = {0, -1, -1, -1, 0, 0, 1, 1, 1};
                bool ok = true;
                for (i32 n = 0; n < 9; ++n) {
                    const i32 v = cell(j + dx[n], i + dy[n]);
                    if (v < lo || v > hi) {
                        ok = false;
                        break;
                    }
                }
                if (!ok) continue;
                i16 v = cell(j, i);
                // 32-bit wrapping imul / add like the original, then the low word
                if (k >= 0)
                    v = static_cast<i16>(static_cast<u32>(lo) + static_cast<u32>(v - lo) * static_cast<u32>(k));
                else
                    v = static_cast<i16>(static_cast<u32>(hi) + static_cast<u32>(hi - v) * static_cast<u32>(k));
                while (v < lo || v > hi) {
                    if (v > hi)
                        v = static_cast<i16>(hi - (v - hi));
                    else if (v < lo)
                        v = static_cast<i16>(lo + (lo - v));
                }
                joan::orig::as_height_grid(this)->set(joan::orig::arg::u32(j), joan::orig::arg::u32(i), joan::orig::arg::u16(static_cast<i32>(v)));  // 0x004c5a20
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_04", name, addr, fn)
B("Plasma::shuffle_level", 0x004c68d0, &H::smooth_4c68d0);
#undef B

}  // namespace game_b104_04
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

using joan::call::thiscall;

using joan::hook::global;

JOAN_COUNTER(c_curve, "Curve::dx/dy/dyy");

struct H {
    // Word table lookup 0x004c59d0 (x, y).
    std::int32_t cell(std::int32_t x, std::int32_t y) {
        return joan::orig::arg::as<std::int16_t>(joan::orig::as_height_grid(this)->at(joan::orig::arg::i32(static_cast<std::int16_t>(x)), joan::orig::arg::i32(static_cast<std::int16_t>(y))));
    }

    // ------------------------------------------------------------------ 0x004c6e40
    float curve_dx(std::int16_t x, std::int32_t y) {
        c_curve.hit();
        const float two = global<float>(d::flt_2);
        if (y <= 0) {
            std::int32_t v = cell(x, y + 1) << 2;
            v -= cell(x, y + 2);
            v -= cell(x, y) * 3;
            return static_cast<float>(v) / two;
        }
        if (y >= static_cast<std::int32_t>(jo::self<jo::HeightGrid>(this)->height)) {
            std::int32_t v = cell(x, y - 2);
            v -= cell(x, y - 1) << 2;
            v += cell(x, y) * 3;
            return static_cast<float>(v) / two;
        }
        std::int32_t v = cell(x, y + 1);
        v -= cell(x, y - 1);
        return static_cast<float>(v) / two;
    }

    // ------------------------------------------------------------------ 0x004c6cd0
    float curve_dy(std::int32_t x, std::int16_t y) {
        c_curve.hit();
        const float two = global<float>(d::flt_2);
        if (x <= 0) {
            std::int32_t v = cell(x + 1, y) << 2;
            v -= cell(x + 2, y);
            v -= cell(x, y) * 3;
            return static_cast<float>(v) / two;
        }
        if (x >= static_cast<std::int32_t>(jo::self<jo::HeightGrid>(this)->width)) {
            std::int32_t v = cell(x - 2, y);
            v -= cell(x - 1, y) << 2;
            v += cell(x, y) * 3;
            return static_cast<float>(v) / two;
        }
        std::int32_t v = cell(x + 1, y);
        v -= cell(x - 1, y);
        return static_cast<float>(v) / two;
    }

    // ------------------------------------------------------------------ 0x004c6fb0
    float curve_dyy(std::int32_t x, std::int32_t y) {
        c_curve.hit();
        auto dy = [&](std::int32_t yy) { return joan::orig::arg::as<float>(joan::orig::as_height_grid(this)->dy(joan::orig::arg::i32(x), joan::orig::arg::i16(yy))); };  // 0x004c6cd0
        const float two = global<float>(d::flt_2);
        const float c98 = global<float>(d::flt_3), c9c = global<float>(d::flt_4);
        if (y <= 0) {
            const float a = dy(y + 1) * c9c;
            const float b = a - dy(y + 2);
            const float c = dy(y) * c98;
            return (b - c) / two;
        }
        if (y >= static_cast<std::int32_t>(jo::self<jo::HeightGrid>(this)->height)) {
            const float t = dy(y - 2);
            const float u = t - dy(y - 1) * c9c;
            const float c = dy(y) * c98;
            return (c + u) / two;
        }
        const float t = dy(y + 1);
        return (t - dy(y - 1)) / two;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Plasma::gradient_y", 0x004c6e40, &H::curve_dx);
B("Plasma::gradient_x", 0x004c6cd0, &H::curve_dy);
B("Plasma::gradient_xy", 0x004c6fb0, &H::curve_dyy);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b82_16.cpp (game/b82_16)
namespace {
namespace game_b82_16 {

using joan::call::thiscall;

using joan::hook::global;

JOAN_COUNTER(c_sample, "HeightGrid::sample");

// x87 compare helpers (fcomp + fnstsw): lt = "test ah,1 jnz" (a < b or unordered),
// gt = "test ah,0x41 jz" (a > b).
bool lt(float a, float b) { return !(a >= b); }
bool gt(float a, float b) { return a > b; }

struct H {
    // ------------------------------------------------------------------ 0x004c7100
    float grid_h(std::int32_t i, std::int32_t j) {  // 0x004c59d0 (words)
        return static_cast<float>(
            joan::orig::arg::as<std::int16_t>(joan::orig::as_height_grid(this)->at(joan::orig::arg::i32(static_cast<std::int16_t>(i)), joan::orig::arg::i32(static_cast<std::int16_t>(j)))));
    }
    float grid_a(std::int32_t i, std::int32_t j) { return joan::orig::arg::as<float>(joan::orig::as_height_grid(this)->dy(joan::orig::arg::i32(i), joan::orig::arg::i16(j))); }  // 0x004c6cd0
    float grid_b(std::int32_t i, std::int32_t j) { return joan::orig::arg::as<float>(joan::orig::as_height_grid(this)->dx(joan::orig::arg::i16(i), joan::orig::arg::i32(j))); }  // 0x004c6e40
    float grid_c(std::int32_t i, std::int32_t j) { return joan::orig::arg::as<float>(joan::orig::as_height_grid(this)->dyy(joan::orig::arg::i32(i), joan::orig::arg::i32(j))); }  // 0x004c6fb0

    float corner(std::int32_t i, std::int32_t j, std::int32_t hi, std::int32_t hj, float* dx, float* dy) {
        if (dx != nullptr) *dx = grid_a(i, j);
        if (dy != nullptr) *dy = grid_b(i, j);
        return grid_h(hi, hj);
    }

    float sample(float x, float y, float* dx, float* dy) {
        c_sample.hit();
        const joan::hook::x87::Fpu fpu{true};
        std::int32_t i = joan::hook::x87::ftol(x);
        if (i >= jo::self<jo::HeightGrid>(this)->width) i = jo::self<jo::HeightGrid>(this)->width - 1;
        if (i < 0) i = 0;
        const std::int32_t i1 = i + 1;
        const float fx = static_cast<float>(fpu.sub(x, static_cast<double>(i)));
        std::int32_t j = joan::hook::x87::ftol(y);
        if (j >= jo::self<jo::HeightGrid>(this)->height) j = jo::self<jo::HeightGrid>(this)->height - 1;
        if (j < 0) j = 0;
        const std::int32_t j1 = j + 1;
        const float fy = static_cast<float>(fpu.sub(y, static_cast<double>(j)));

        const float e_lo = global<float>(d::flt_m0_001);  // -0.001
        const float e_hi = global<float>(d::flt_0_001);  // 0.001
        const float o_lo = global<float>(d::flt_0_999);  // 0.999
        const float o_hi = global<float>(d::flt_1_001);  // 1.001
        auto near0 = [&](float t) { return !lt(t, e_lo) && !gt(t, e_hi); };
        auto near1 = [&](float t) { return !lt(t, o_lo) && !gt(t, o_hi); };

        if (near0(fx) && near0(fy)) return corner(i, j, i, j, dx, dy);
        if (near1(fx) || near1(fy)) {
            if (near0(fy) && i + 1 <= jo::self<jo::HeightGrid>(this)->width) return corner(i + 1, j, i + 1, j, dx, dy);
            if (near0(fx) && j + 1 <= jo::self<jo::HeightGrid>(this)->height) return corner(i, j + 1, i, j + 1, dx, dy);
            if (near1(fx) && near1(fy) && i + 1 <= jo::self<jo::HeightGrid>(this)->width && j + 1 <= jo::self<jo::HeightGrid>(this)->height)
                return corner(i + 1, j + 1, i, j, dx, dy);  // height at (i, j) as in the original
        }

        const float h00 = grid_h(i, j);    // -0x20
        const float h10 = grid_h(i1, j);   // -0x24
        const float h01 = grid_h(i, j1);   // -0x28
        const float h11 = grid_h(i1, j1);  // -0x2c
        const float a00 = grid_a(i, j), b00 = grid_b(i, j), c00 = grid_c(i, j);        // -0x30 -0x34 -0x38
        const float a10 = grid_a(i1, j), b10 = grid_b(i1, j), c10 = grid_c(i1, j);     // -0x3c -0x40 -0x44
        const float a01 = grid_a(i, j1), b01 = grid_b(i, j1), c01 = grid_c(i, j1);     // -0x48 -0x4c -0x50
        const float a11 = grid_a(i1, j1), b11 = grid_b(i1, j1), c11 = grid_c(i1, j1);  // -0x54 -0x58 -0x5c
        const float v60 = h00, v64 = a00, v68 = b00, v6c = c00;

        const float v70 = h10 + h10 + h10 - a10 - v64 - v64 - v60 - v60 - v60;
        const float v74 = a10 - h10 - h10 + v64 + v60 + v60;
        const float v78 = h01 + h01 + h01 - b01 - v68 - v68 - v60 - v60 - v60;
        const float v7c = b01 - h01 - h01 + v68 + v60 + v60;
        const float v80 = b10 + b10 + b10 - c10 - v6c - v6c - v68 - v68 - v68;
        const float v84 = c10 - b10 - b10 + v6c + v68 + v68;
        const float v88 = a01 + a01 + a01 - c01 - v6c - v6c - v64 - v64 - v64;
        const float v8c = c01 - a01 - a01 + v6c + v64 + v64;
        const float v90 = h11 - h10 - h01 - b10 - a01 + v60 + v64 + v68 + v6c;
        const float v94 = a11 - a10 - c10 - v88 - v8c;
        const float v98 = b11 - b01 - c01 - v80 - v84;
        const float v9c = c11 - c10 - c01 + v6c;
        const float k3 = global<float>(d::flt_3);  // 3.0
        const float k9 = global<float>(d::flt_9);  // 9.0
        const float k2 = global<float>(d::flt_2);  // 2.0
        const float va0 = v9c - k3 * v94 - k3 * v98 + k9 * v90;
        const float va4 = k3 * v90 - v98 - va0;
        const float va8 = k3 * v90 - v94 - va0;
        const float vac = v90 - va8 - va4 - va0;
        if (dx != nullptr) {
            const float vb0 = ((vac * fy + va4) * fy + v84) * fy + v74;
            const float vb4 = ((va8 * fy + va0) * fy + v80) * fy + v70;
            const float vb8 = ((v8c * fy + v88) * fy + v6c) * fy + v64;
            *dx = (k3 * vb0 * fx + k2 * vb4) * fx + vb8;
        }
        const float vbc = ((vac * fx + va8) * fx + v8c) * fx + v7c;
        const float vc0 = ((va4 * fx + va0) * fx + v88) * fx + v78;
        const float vc4 = ((v84 * fx + v80) * fx + v6c) * fx + v68;
        const float vc8 = ((v74 * fx + v70) * fx + v64) * fx + v60;
        if (dy != nullptr) *dy = (k3 * vbc * fy + k2 * vc0) * fy + vc4;
        return ((vbc * fy + vc0) * fy + vc4) * fy + vc8;
    }
};

#define B16(name, addr, fn) JOAN_REPLACE("game/b82_16", name, addr, fn)
B16("Plasma::interpolate_z", 0x004c7100, &H::sample);
#undef B16

}  // namespace game_b82_16
}  // namespace

// ---------------------------------------- from modules/game_world4.cpp (game/world4)
namespace {
namespace game_world4 {

using joan::call::thiscall;

JOAN_COUNTER(c_height, "Terrain::height");

struct H {
    // 0x004c7b70 Terrain::height(x, y, diagonal): +0 / +2 grid size, samples 0x004c59d0.
    float terrain_height(float x, float y, char diagonal) {
        c_height.hit();
        const joan::hook::x87::Fpu fpu{true};
        std::int32_t ix = joan::hook::x87::ftol(x);
        if (!(ix < static_cast<std::int32_t>(jo::self<jo::HeightMap>(this)->width))) ix = static_cast<std::int32_t>(jo::self<jo::HeightMap>(this)->width) - 1;
        if (ix < 0) ix = 0;
        const std::int32_t ix1 = ix + 1;
        const float fx = static_cast<float>(fpu.sub(x, static_cast<double>(ix)));
        std::int32_t iy = joan::hook::x87::ftol(y);
        if (!(iy < static_cast<std::int32_t>(jo::self<jo::HeightMap>(this)->height))) iy = static_cast<std::int32_t>(jo::self<jo::HeightMap>(this)->height) - 1;
        if (iy < 0) iy = 0;
        const std::int32_t iy1 = iy + 1;
        const float fy = static_cast<float>(fpu.sub(y, static_cast<double>(iy)));
        auto sample = [this](std::int32_t i, std::int32_t j) {
            return static_cast<float>(
                joan::orig::arg::as<std::int16_t>(joan::orig::as_height_grid(this)->at(joan::orig::arg::i32(static_cast<std::int16_t>(i)), joan::orig::arg::i32(static_cast<std::int16_t>(j)))));  // 0x004c59d0
        };
        const float h00 = sample(ix, iy);
        const float h10 = sample(ix1, iy);
        const float h01 = sample(ix, iy1);
        const float h11 = sample(ix1, iy1);
        const std::int32_t mode = static_cast<std::int32_t>(diagonal);
        if (mode == 0) {
            if (fx + fy > 1.0f) {
                const float a = (h10 - h11) * (1.0f - fy) + h11;
                const float b = (h01 - h11) * (1.0f - fx);
                return a + b;
            }
            const float a = (h10 - h00) * fx + h00;
            const float b = (h01 - h00) * fy;
            return a + b;
        }
        if (mode == 1) {
            if (fy - fx > 0.0f) {
                const float a = (h00 - h01) * (1.0f - fy) + h01;
                const float b = (h11 - h01) * fx;
                return a + b;
            }
            const float a = (h00 - h10) * (1.0f - fx) + h10;
            const float b = (h11 - h10) * fy;
            return a + b;
        }
        return 0.0f;
    }
};

#define W4(name, addr, fn) JOAN_REPLACE("game/world4", name, addr, fn)
W4("Terrain::height", 0x004c7b70, &H::terrain_height);
#undef W4

}  // namespace game_world4
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::HeightGrid::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b_2::H::grid_ctor>(this)); }  // 0x4c56b0
auto joan::orig::HeightGrid::free() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves82b::H::grid_free>(this)); }  // 0x4c5870
auto joan::orig::HeightGrid::create(std::int16_t a0, std::int16_t a1, std::int16_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves82b::H::grid_create>(this, a0, a1, a2)); }  // 0x4c58a0
auto joan::orig::HeightGrid::free_4c5960() -> void { joan::bind::member<&::game_b82_18::H::plasma_free>(this); }  // 0x4c5960
auto joan::orig::HeightGrid::at(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves81::H::grid_at>(this, a0, a1)); }  // 0x4c59d0
auto joan::orig::HeightGrid::set(std::uint32_t a0, std::uint32_t a1, std::uint16_t a2) -> void { joan::bind::member<&::game_b82_18_2::H::grid_set>(this, a0, a1, a2); }  // 0x4c5a20
auto joan::orig::Terrain::subdivide_4c5c30(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_10::H::subdivide>(this, a0, a1, a2, a3)); }  // 0x4c5c30
auto joan::orig::Terrain::subdivide(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_04::H::subdivide>(this, a0, a1, a2, a3, a4)); }  // 0x4c5f10
auto joan::orig::HeightGrid::smooth(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::int16_t a4, std::int16_t a5) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b92_03::H::smooth>(this, a0, a1, a2, a3, a4, a5)); }  // 0x4c6530
auto joan::orig::HeightGrid::dy(std::int32_t a0, std::int16_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b83_07::H::curve_dy>(this, a0, a1)); }  // 0x4c6cd0
auto joan::orig::HeightGrid::dx(std::int16_t a0, std::int32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b83_07::H::curve_dx>(this, a0, a1)); }  // 0x4c6e40
auto joan::orig::HeightGrid::dyy(std::int32_t a0, std::int32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b83_07::H::curve_dyy>(this, a0, a1)); }  // 0x4c6fb0
auto joan::orig::HeightGrid::sample(float a0, float a1, void* a2, void* a3) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b82_16::H::sample>(this, a0, a1, a2, a3)); }  // 0x4c7100
