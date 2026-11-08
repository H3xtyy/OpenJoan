// uti3d/spindex.cpp: functions of the original d:\ww\uti3d\spindex.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cmath>
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cstdarg>
#include "../original/cxx_eh.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b93_05.cpp (game/b93_05)
//
//      0x00681ee0 SpIndex::~SpIndex - releases entries count..1 not yet released (0x00683650 /
//                 0x00683b00), frees +0x14 (spindex.cpp), dtor 0x00585710 of the array at +0x18.
//
//      0x00683350 SpIndex::fill_rect(id, x0, y0, x1, y1) - marks the cells of a rectangle.
//
//      0x00683850 Key::equals - byte +4 and dword +0 equal.
namespace {
namespace game_b93_05 {

constexpr img::Addr kMemory(d::g_mem);  // allocator (0x0058ed70 free)

JOAN_COUNTER(c_spindex_dtor, "SpIndex::dtor");
JOAN_COUNTER(c_fill_rect, "SpIndex::fill_rect");

struct H {
    // 0x00681ee0
    void spindex_dtor() {
        c_spindex_dtor.hit();
        void* const arr = &jo::self<jo::SpIndex>(this)->entries;
        for (std::int32_t i = joan::orig::as_dynarray(arr)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_spatial_index(this)->slot_free(joan::orig::arg::i32(i))) == 0) {  // 0x00683650
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_dynarray(arr)->get(joan::orig::arg::i32(i)));  // 0x00417200
                joan::orig::as_sp_index(e)->free_buffers();                               // 0x00683b00
            }
        }
        if (jo::self<jo::SpIndex>(this)->cells != nullptr) {
            const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_a2189c)) + 9;
            joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::SpIndex>(this)->cells)), joan::orig::arg::str(&img::global<const char>(d::s_d_ww_uti3d_spindex_cpp)), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::SpIndex>(this)->cells = nullptr;
        }
        joan::orig::dynarray_b(arr)->destruct();  // 0x00585710
    }

    // 0x00683350 (ret 0x14)
    void fill_rect(std::int32_t id, std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1) {
        c_fill_rect.hit();
        void* const arr = &jo::self<jo::SpIndex>(this)->entries;
        void* const entry = joan::orig::arg::as<void*>(joan::orig::as_dynarray(arr)->get(joan::orig::arg::i32(id)));  // 0x00417200
        for (std::int32_t y = y0; y <= y1; ++y) {
            for (std::int32_t x = x0; x <= x1; ++x) {
                std::int32_t* const cell = joan::orig::arg::as<std::int32_t*>(joan::orig::as_spatial_index(this)->cell(joan::orig::arg::i32(x), joan::orig::arg::i32(y)));  // 0x00683280
                if (cell[1] == 0) {
                    joan::orig::as_sp_index(entry)->set_b(joan::orig::arg::i32(x), joan::orig::arg::i32(y), 0);  // 0x00683f60
                    joan::orig::as_sp_index(entry)->set_a(joan::orig::arg::i32(x), joan::orig::arg::i32(y), 0);  // 0x00683eb0
                    cell[0] = id;
                    cell[1] = id;
                } else {
                    const std::int32_t old = cell[1];
                    void* const prev = joan::orig::arg::as<void*>(joan::orig::as_dynarray(arr)->get(joan::orig::arg::i32(old)));  // 0x00417200
                    joan::orig::as_sp_index(entry)->set_a(joan::orig::arg::i32(x), joan::orig::arg::i32(y), 0);    // 0x00683eb0
                    joan::orig::as_sp_index(entry)->set_b(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(old));  // 0x00683f60
                    joan::orig::as_sp_index(prev)->set_a(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(id));    // 0x00683eb0
                    cell[1] = id;
                }
            }
        }
    }

    // 0x00683850
    bool equals(const void* o) {
        return static_cast<std::int8_t>(jo::self<jo::Key>(this)->kind) == static_cast<std::int8_t>(jo::as_key(o)->kind) &&
               jo::self<jo::Key>(this)->code == jo::as_key(o)->code;
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b93_05", name, addr, fn)
GAME("SpatialIndex::~SpatialIndex", 0x00681ee0, &H::spindex_dtor);
GAME("SpatialIndex::add_to_zone", 0x00683350, &H::fill_rect);
GAME("Key::equals", 0x00683850, &H::equals);
#undef GAME

}  // namespace game_b93_05
}  // namespace

// ---------------------------------------- from modules/game_b99_09.cpp (game/b99_09)
//
//      0x00681fe0 SpIndex::init               (thiscall(w, h, cell), ret 0xc) - spindex.cpp.
//
//      0x00682560 SpIndex::move_by_key        (thiscall(key, 4 dwords), ret 0x14).
//
//      0x006838b0 Key::differs                (thiscall(Key*), ret 4).
//
//  - Return values in eax that are leftovers of the original code (the last callee's result,
//    the loop counter in 0x00682560, the upper bytes of 0x006838b0, CoUninitialize's eax) are
//    reproduced even where callers ignore them.
namespace {
namespace game_b99_09 {

using i8 = std::int8_t;

JOAN_COUNTER(c_sp_init, "SpIndex::init");
JOAN_COUNTER(c_sp_move_key, "SpIndex::move_by_key");

struct H {

    u32 sp_init(u32 w, u32 h, u32 cell) {  // 0x00681fe0
        c_sp_init.hit();
        joan::orig::as_sp_index(this)->reset();  // 0x006820d0
        jo::self<jo::SpIndex>(this)->cell_size = cell;
        jo::self<jo::SpIndex>(this)->width = w;
        jo::self<jo::SpIndex>(this)->height = h;
        jo::self<jo::SpIndex>(this)->cols = (w + cell - 1u) / cell;
        jo::self<jo::SpIndex>(this)->rows = (h + cell - 1u) / cell;
        const i32 line = static_cast<i32>(global<i16>(d::g_a218a0)) + 9;
        const u32 size = (jo::self<jo::SpIndex>(this)->rows << 3) * jo::self<jo::SpIndex>(this)->cols;
        jo::self<jo::SpIndex>(this)->cells = joan::orig::arg::as<u8*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size), str(d::s_d_ww_uti3d_spindex_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        const u32 size2 = (jo::self<jo::SpIndex>(this)->rows << 3) * jo::self<jo::SpIndex>(this)->cols;
        std::memset(joan::orig::arg::mptr(jo::self<jo::SpIndex>(this)->cells), i32{0}, joan::orig::arg::u32(size2));  // memset
        joan::orig::as_sound(&jo::self<jo::SpIndex>(this)->entries)->reset_585d20();  // 0x00585d20
        return 0;  // void callee; callers ignore the result
    }

    u32 sp_move_by_key(const void* key, u32 a, u32 b, u32 c, u32 d) {  // 0x00682560 (key: a Key*)
        c_sp_move_key.hit();
        i32 i = joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->size();  // 0x00417ad0
        for (; i > 0; --i) {
            if (joan::orig::arg::as<u32>(joan::orig::as_spatial_index(this)->slot_free(joan::orig::arg::i32(i))) != 0) continue;           // 0x00683650
            void* e = joan::orig::arg::as<void*>(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->get(joan::orig::arg::i32(i)));  // 0x00417200
            if ((joan::orig::arg::as<u32>(joan::orig::as_key(e)->equals(joan::orig::arg::mptr(key))) & 0xff) == 0) continue;   // 0x00683850
            return joan::orig::arg::as<u32>(joan::orig::as_sp_index(this)->move(joan::orig::arg::u32(i), joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d)));           // 0x00682610
        }
        return static_cast<u32>(i);
    }

    u32 differs(const u8* other) {  // 0x006838b0
        const i32 b2 = static_cast<i8>(other[4]);
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Key>(this)->kind)) != b2) return (static_cast<u32>(b2) & 0xffffff00u) | 1u;
        const u32 e = jo::self<jo::Key>(this)->code;
        std::uint32_t o;
        std::memcpy(&o, other, 4);
        if (e != o) return (e & 0xffffff00u) | 1u;
        return e & 0xffffff00u;
    }
};

#define B99(name, addr_, fn) JOAN_REPLACE("game/b99_09", name, addr_, fn)
B99("SpatialIndex::init", 0x00681fe0, &H::sp_init);
B99("SpatialIndex::change_pos_682560", 0x00682560, &H::sp_move_by_key);
B99("Key::differs", 0x006838b0, &H::differs);
#undef B99

}  // namespace game_b99_09
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
//
//      0x006820d0 SpIndex::reset                    (thiscall, ret 0)
//
//      0x00683bb0 SpIndex::resize(a..h)             (thiscall, ret 0x20)
//
//  - 0x00683bb0: when the realloc'd pointer is null the memset hits this+0x30 / this+0x44
//    (original behaviour kept).
namespace {
namespace game_b97_08 {

void* const kMemory = obj(d::g_mem);
const char* const kSpindexCpp = str(d::s_d_ww_uti3d_spindex_cpp);  // "d:\ww\uti3d\spindex.cpp"

using joan::orig::helpers::line_base;

JOAN_COUNTER(c_spindex_reset, "SpIndex::reset");
JOAN_COUNTER(c_spindex_resize, "SpIndex::resize");

struct H {

    // ------------------------------------------------------------------ 0x006820d0
    void spindex_reset() {
        c_spindex_reset.hit();
        for (i32 i = joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_spatial_index(this)->slot_free(joan::orig::arg::i32(i))) == 0) {                     // 0x00683650
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->get(joan::orig::arg::i32(i)));  // 0x00417200
                joan::orig::as_sp_index(e)->free_buffers();                                       // 0x00683b00
            }
        }
        joan::orig::as_sound(&jo::self<jo::SpIndex>(this)->entries)->reset_585d20();  // 0x00585d20
        if (jo::view<void*>(jo::self<jo::SpIndex>(this)->cells) != nullptr) {
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::SpIndex>(this)->cells)), joan::orig::arg::str(kSpindexCpp), joan::orig::arg::i32(line_base(d::g_a218a4) + 0xa));  // 0x0058ed70
            jo::self<jo::SpIndex>(this)->cells = nullptr;
        }
        jo::self<jo::SpIndex>(this)->height = 0;
        jo::self<jo::SpIndex>(this)->width = 0;
        jo::self<jo::SpIndex>(this)->rows = 0;
        jo::self<jo::SpIndex>(this)->cols = 0;
    }

    // ------------------------------------------------------------------ 0x00683bb0
    void spindex_resize(u32 a, u32 b, u32 c, u32 d, i32 x0, i32 y0, i32 x1, i32 y1) {
        c_spindex_resize.hit();
        jo::view<u32>(jo::self<jo::SpEntry>(this)->a) = a;
        jo::view<u32>(jo::self<jo::SpEntry>(this)->b) = b;
        jo::view<u32>(jo::self<jo::SpEntry>(this)->c) = c;
        jo::view<u32>(jo::self<jo::SpEntry>(this)->d) = d;
        jo::self<jo::SpEntry>(this)->x0 = x0;
        jo::self<jo::SpEntry>(this)->y0 = y0;
        jo::self<jo::SpEntry>(this)->x1 = x1;
        jo::self<jo::SpEntry>(this)->y1 = y1;
        const i32 n = static_cast<i32>(static_cast<u32>(x1 - x0 + 1) * static_cast<u32>(y1 - y0 + 1));
        const u32 bytes = static_cast<u32>(n) << 2;
        if (jo::self<jo::SpEntry>(this)->capacity < n) {
            jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_a) = joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_a)), joan::orig::arg::u32(bytes), joan::orig::arg::str(kSpindexCpp), joan::orig::arg::i32(line_base(d::g_a218b0) + 8));  // 0x0058eb30
            jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_b) = joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_b)), joan::orig::arg::u32(bytes), joan::orig::arg::str(kSpindexCpp), joan::orig::arg::i32(line_base(d::g_a218b0) + 9));  // 0x0058eb30
            jo::self<jo::SpEntry>(this)->capacity = n;
        }
        if (jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_a) == nullptr) memset_(jo::self<jo::SpEntry>(this)->inline_a, 0, bytes);
        else memset_(jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_a), 0, bytes);
        if (jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_b) == nullptr) memset_(jo::self<jo::SpEntry>(this)->inline_b, 0, bytes);
        else memset_(jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_b), 0, bytes);
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("SpatialIndex::deinit", 0x006820d0, &H::spindex_reset);
B97("SpatialIndex::SPObjRefStore::change_pos", 0x00683bb0, &H::spindex_resize);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
//
//      0x006821d0 SpIndex::insert         (thiscall(5), ret 0x14)
//
//      0x00682610 SpIndex::move           (thiscall(5), ret 0x14)
namespace {
namespace game_b98_08 {

JOAN_COUNTER(c_sp_insert, "SpIndex::insert");
JOAN_COUNTER(c_sp_move, "SpIndex::move");

struct H {

    // ------------------------------------------------------------------------ 0x006821d0
    u32 sp_insert(void* a8, u32 ac, u32 a10, u32 a14, u32 a18) {
        c_sp_insert.hit();
        u32 x0 = 0, y0 = 0, x1 = 0, y1 = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_spatial_index(this)->cells_for_rect(joan::orig::arg::i32(ac), joan::orig::arg::i32(a10), joan::orig::arg::i32(a14), joan::orig::arg::i32(a18), &x0, &y0, &x1, &y1)) == 0) return 0;  // 0x00683110
        joan::orig::dynarray_b(&jo::self<jo::SpIndex>(this)->entries)->linkin(joan::orig::arg::ptr(i32{0}));  // 0x005857a0
        const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->cursor());  // 0x00417520
        jo::SpEntry* const node = jo::as_sp_entry(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->get(joan::orig::arg::i32(idx)));  // 0x00417200
        jo::view<u32>(node->active) = idx;
        joan::orig::as_sp_index(node)->init_683910(a8, joan::orig::arg::i32(ac), joan::orig::arg::i32(a10), joan::orig::arg::i32(a14), joan::orig::arg::i32(a18), joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1));  // 0x00683910
        joan::orig::as_sp_index(this)->fill_rect(joan::orig::arg::i32(idx), joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1));                   // 0x00683350
        return idx;
    }

    // ------------------------------------------------------------------------ 0x00682610
    u32 sp_move(u32 a8, u32 ac, u32 a10, u32 a14, u32 a18) {
        c_sp_move.hit();
        jo::SpEntry* const node = jo::as_sp_entry(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->get(joan::orig::arg::i32(a8)));  // 0x00417200
        u32 x0 = 0, y0 = 0, x1 = 0, y1 = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_spatial_index(this)->cells_for_rect(joan::orig::arg::i32(ac), joan::orig::arg::i32(a10), joan::orig::arg::i32(a14), joan::orig::arg::i32(a18), &x0, &y0, &x1, &y1)) == 0) return 0;  // 0x00683110
        if (jo::view<u32>(node->x0) == x0 && jo::view<u32>(node->y0) == y0 && jo::view<u32>(node->x1) == x1 &&
            jo::view<u32>(node->y1) == y1) {
            u8* const n = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->get(joan::orig::arg::i32(a8)));  // 0x00417200
            joan::orig::as_sp_index(n)->resize(joan::orig::arg::u32(ac), joan::orig::arg::u32(a10), joan::orig::arg::u32(a14), joan::orig::arg::u32(a18), joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1));  // 0x00683bb0
            return 0;  // void callee; only caller chain (0x00682560 / 0x006824a0) is unreferenced
        }
        joan::orig::as_grid(this)->link_range(joan::orig::arg::i32(a8), joan::orig::arg::i32(jo::view<u32>(node->x0)), joan::orig::arg::i32(jo::view<u32>(node->y0)), joan::orig::arg::i32(jo::view<u32>(node->x1)), joan::orig::arg::i32(jo::view<u32>(node->y1)));                                             // 0x006834c0
        u8* const n = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->get(joan::orig::arg::i32(a8)));  // 0x00417200
        joan::orig::as_sp_index(n)->resize(joan::orig::arg::u32(ac), joan::orig::arg::u32(a10), joan::orig::arg::u32(a14), joan::orig::arg::u32(a18), joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1));                 // 0x00683bb0
        joan::orig::as_sp_index(this)->fill_rect(joan::orig::arg::i32(a8), joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1));  // 0x00683350
        return 0;  // void callee; result unused
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("SpatialIndex::add_obj", 0x006821d0, &H::sp_insert);
B98("SpatialIndex::change_pos_682610", 0x00682610, &H::sp_move);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
//
//    - Grid::remove_at 0x006822d0 (5 args), ret 0x14 / Grid::move_at 0x006824a0 (9 args),
//      ret 0x24: cell range (0x00683110) -> cell lookup (0x006832c0) -> 0x00682410 /
//      0x00682610, else the slow path 0x00682370 / 0x00682560.
//
//    - Grid::check 0x006836b0 (x, y), ret 8: returns 1 immediately (the rest of the body is
//      unreachable and reads uninitialised locals; not executed).
//
//  - Functions whose eax is not obviously void return the value the original leaves in eax
//    (0x00666d00, 0x00675e10, 0x00689ca0, 0x0068ae90, 0x006b3130, 0x006822d0, 0x006824a0).
namespace {
namespace game_b102_14 {

JOAN_COUNTER(c_remove_at, "Grid::remove_at");
JOAN_COUNTER(c_move_at, "Grid::move_at");

struct H {
    // ------------------------------------------------------------------ 0x006822d0
    u32 remove_at(const void* id, i32 x0, i32 y0, i32 x1, i32 y1) {  // id: the key (a Key*)
        c_remove_at.hit();
        i32 cx0, cy0, cx1, cy1;
        if (joan::orig::arg::as<i32>(joan::orig::as_spatial_index(this)->cells_for_rect(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), &cx0, &cy0, &cx1, &cy1)) != 0) {  // 0x00683110
            const i32 cell = joan::orig::arg::as<i32>(joan::orig::as_sp_index(this)->walk_chain(id, joan::orig::arg::u32(cx0), joan::orig::arg::u32(cy0)));  // 0x006832c0
            if (cell == 0) return 0;
            return joan::orig::arg::as<u32>(joan::orig::as_grid(this)->remove(joan::orig::arg::i32(cell)));  // 0x00682410
        }
        return joan::orig::arg::as<u32>(joan::orig::as_sp_index(this)->remove_matching(id));  // 0x00682370
    }
    // ------------------------------------------------------------------ 0x006824a0
    u32 move_at(const void* id, i32 x0, i32 y0, i32 x1, i32 y1, u32 a, u32 b, u32 c, u32 d) {  // id: the key (a Key*)
        c_move_at.hit();
        i32 cx0, cy0, cx1, cy1;
        if (joan::orig::arg::as<i32>(joan::orig::as_spatial_index(this)->cells_for_rect(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), &cx0, &cy0, &cx1, &cy1)) != 0) {  // 0x00683110
            const i32 cell = joan::orig::arg::as<i32>(joan::orig::as_sp_index(this)->walk_chain(id, joan::orig::arg::u32(cx0), joan::orig::arg::u32(cy0)));  // 0x006832c0
            if (cell != 0) return joan::orig::arg::as<u32>(joan::orig::as_sp_index(this)->move(joan::orig::arg::u32(cell), joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d)));  // 0x00682610
        }
        return joan::orig::arg::as<u32>(joan::orig::as_sp_index(this)->move_by_key(id, joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d)));  // 0x00682560
    }
    // ------------------------------------------------------------------ 0x006836b0
    i32 grid_check(i32, i32) { return 1; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("SpatialIndex::del_obj_6822d0", 0x006822d0, &H::remove_at);
B("SpatialIndex::change_pos_6824a0", 0x006824a0, &H::move_at);
B("SpatialIndex::validate_zone", 0x006836b0, &H::grid_check);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
//
//    - SpIndex::remove_matching 0x00682370 (a), ret 4 (counter): last-to-first over +0x18,
//      skips 0x683650(i) != 0, 0x682410(i) when byte 0x683850(e, a) != 0.
//
//    - SpIndex::rects_overlap 0x006829e0 (8 ints), ret 0x20: signed interval overlap test.
//
//    - SpIndex::walk_chain 0x006832c0 (a, b, c), ret 0xc (counter): p = *0x683280(b, c);
//      while p: e = 0x417200(+0x18, p), stop unless byte 0x6838b0(e, a), p = 0x683d70(e, b, c).
//      Returns 0.
//
//  - 0x006832c0: MSVC pushes `a` before the 0x417200 call; it is the 0x6838b0 argument.
namespace {
namespace game_b100_15 {

JOAN_COUNTER(c_remove_matching, "SpIndex::remove_matching");
JOAN_COUNTER(c_walk_chain, "SpIndex::walk_chain");

struct H {
    // ------------------------------------------------------------------ 0x00682370
    i32 remove_matching(const void* a) {  // a: the key (a Key*)
        c_remove_matching.hit();
        i32 i = joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->size();  // 0x00417ad0
        for (; i > 0; --i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_spatial_index(this)->slot_free(joan::orig::arg::i32(i))) != 0) continue;  // 0x00683650
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->get(joan::orig::arg::i32(i)));  // 0x00417200
            if ((joan::orig::arg::as<u32>(joan::orig::as_key(e)->equals(joan::orig::arg::mptr(a))) & 0xffu) != 0)                // 0x00683850
                joan::orig::as_grid(this)->remove(joan::orig::arg::i32(i));                            // 0x00682410
        }
        return i;
    }

    // ------------------------------------------------------------------ 0x006829e0
    i32 rects_overlap(i32 a0, i32 a1, i32 a2, i32 a3, i32 b0, i32 b1, i32 b2, i32 b3) {
        if (b1 > a1 || b3 < a1) {
            if (a1 > b1) return 0;
            if (a3 < b1) return 0;
        }
        if (b0 > a0 || b2 < a0) {
            if (a0 > b0) return 0;
            if (a2 < b0) return 0;
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x006832c0
    i32 walk_chain(const void* a, u32 b, u32 c) {  // a: the key (a Key*)
        c_walk_chain.hit();
        u32 p = *joan::orig::arg::as<u32*>(joan::orig::as_spatial_index(this)->cell(joan::orig::arg::i32(b), joan::orig::arg::i32(c)));  // 0x00683280
        while (p != 0) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->get(joan::orig::arg::i32(p)));  // 0x00417200
            if ((joan::orig::arg::as<u32>(joan::orig::as_key(e)->differs(joan::orig::arg::mptr(a))) & 0xffu) == 0) break;          // 0x006838b0
            p = joan::orig::arg::as<u32>(joan::orig::as_grid(e)->at(joan::orig::arg::i32(b), joan::orig::arg::i32(c)));                             // 0x00683d70
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("SpatialIndex::del_obj_682370", 0x00682370, &H::remove_matching);
B("SpIndex::rects_overlap", 0x006829e0, &H::rects_overlap);
B("SpatialIndex::find_from_zone", 0x006832c0, &H::walk_chain);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
//
//      0x00682410 Grid::remove(k)            (thiscall, ret 4) - 0x00417200 / 0x006834c0 /
//                 0x00683b00 / +8 = 0 / 0x005858c0.
//
//  - 0x00682410 / 0x006f4a10 / 0x006f60d0 / 0x007364c0 / 0x00738b50 / 0x006ed4e0 /
namespace {
namespace game_b95_08 {

JOAN_COUNTER(c_grid_remove, "Grid::remove");

struct H {

    // ------------------------------------------------------------------ 0x00682410
    i32 grid_remove(i32 k) {
        c_grid_remove.hit();
        jo::SpEntry* const e = jo::as_sp_entry(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->get(joan::orig::arg::i32(k)));  // 0x00417200
        joan::orig::as_grid(this)->link_range(joan::orig::arg::i32(k), joan::orig::arg::i32(e->x0), joan::orig::arg::i32(e->y0), joan::orig::arg::i32(e->x1), joan::orig::arg::i32(e->y1));                            // 0x006834c0
        joan::orig::as_sp_index(e)->free_buffers();                                    // 0x00683b00
        e->active = 0;
        return thiscall<i32>(fn::linkout_5858c0, &jo::self<jo::SpIndex>(this)->entries, k);         // 0x005858c0
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("SpatialIndex::del_obj_682410", 0x00682410, &H::grid_remove);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
//
//      0x006827a0 SpatialIndex::operator[](i)    (thiscall, ret 4) - fatal error when missing.
//
//  - Many of these have no references at all (dead code): 0x00665230..0x00666f70 except none,
//    0x00663a80, 0x006637a0, 0x006827a0, 0x00687c00. 0x00663630 is the element ctor passed to
//    the vector-constructor iterator at 0x0067a29e; 0x006612e0 / 0x0067d690 are vtable slots.
namespace {
namespace game_b103_14 {

struct H {
    // ------------------------------------------------------------------ 0x006827a0
    void* spatial_at(i32 i) {
        jo::SpEntry* const e = jo::as_sp_entry(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->get(joan::orig::arg::i32(i)));  // 0x00417200
        if (e == nullptr || e->active == 0) {
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_SpatialIndex_operator));  // 0x00586cb0 'SpatialIndex::operator[]'
        }
        return e;
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("SpatialIndex::operator[]", 0x006827a0, &H::spatial_at);
#undef B103

}  // namespace game_b103_14
}  // namespace

// ---------------------------------------- from modules/game_b101_14.cpp (game/b101_14)
//
//    - SpIndex::query 0x00682800 (list, x0, y0, x1, y1) -> int, ret 0x14: walks the cells
//      covered by the rectangle (0x00683110 / 0x00683280 / 0x00683d70), tests each entry with
namespace {
namespace game_b101_14 {

void* const kMath = obj(d::g_misc);

JOAN_COUNTER(c_query, "SpIndex::query");

struct H {
    // ------------------------------------------------------------------ 0x00682800
    i32 query(void* list, i32 x0, i32 y0, i32 x1, i32 y1) {
        c_query.hit();
        i32 n = 0;
        i32 cx0 = 0, cy0 = 0, cx1 = 0, cy1 = 0;  // [ebp-0xc], [ebp-0x10], [ebp-0x14], [ebp-0x18]
        if (joan::orig::arg::as<i32>(joan::orig::as_spatial_index(this)->cells_for_rect(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), &cx0, &cy0, &cx1, &cy1)) == 0) return n;  // 0x00683110
        for (i32 y = cy0; y <= cy1; ++y) {
            for (i32 x = cx0; x <= cx1; ++x) {
                const u32* const cell = joan::orig::arg::as<u32*>(joan::orig::as_spatial_index(this)->cell(joan::orig::arg::i32(x), joan::orig::arg::i32(y)));  // 0x00683280
                u32 id = *cell;
                while (id != 0) {
                    jo::SpEntry* const item = jo::as_sp_entry(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->get(joan::orig::arg::i32(id)));  // 0x00417200
                    if (joan::orig::arg::as<i32>(joan::orig::as_sp_index(kMath)->rects_overlap(joan::orig::arg::i32(jo::view<u32>(item->a)), joan::orig::arg::i32(jo::view<u32>(item->b)), joan::orig::arg::i32(jo::view<u32>(item->c)), joan::orig::arg::i32(jo::view<u32>(item->d)), joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1))) != 0) {  // 0x006829e0
                        const i32 my = item->y0 > cy0 ? item->y0 : cy0;
                        if (y == my) {
                            const i32 mx = item->x0 > cx0 ? item->x0 : cx0;
                            if (x == mx) {
                                joan::orig::dynarray(list)->linkin(joan::orig::arg::ptr(item));  // 0x00584620
                                ++n;
                            }
                        }
                    }
                    id = joan::orig::arg::as<u32>(joan::orig::as_grid(item)->at(joan::orig::arg::i32(x), joan::orig::arg::i32(y)));  // 0x00683d70
                }
            }
        }
        return n;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_14", name, addr, fn)
B("SpatialIndex::search_obj", 0x00682800, &H::query);
#undef B

}  // namespace game_b101_14
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {

// 0x00682a70: qsort comparator.
i32 JOAN_CDECL cmp_int_682a70(const i32* a, const i32* b) {
    return static_cast<i32>(static_cast<u32>(*a) - static_cast<u32>(*b));
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("cmp_int_682a70", 0x00682a70, &cmp_int_682a70);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b104_12.cpp (game/b104_12)
//
//    - SpIndex::nearest 0x00682a90 (thiscall(out, x, y, n, max_d), ret 0x14): up to n nearest
//      entries sorted by distance (0x00591e70), spiral over 25 rings (0x005913b0) or a full
//      scan; local blob (0x0057f770 / 0x0057f850). No references at all: dead code.
namespace {
namespace game_b104_12 {

void* const kArrays = obj(d::g_misc);

JOAN_COUNTER(c_nearest, "SpIndex::nearest");

struct H {
    // ------------------------------------------------------------------------ 0x00682a90
    struct Blob {
        void* data;
        u32 size;
    };
    i32 abs_(i32 v) { return joan::orig::arg::as<i32>(joan::orig::libc::abs(joan::orig::arg::i32(v))); }
    void insert_sorted(i32& cnt, i32 n, i32* dists, i32* d, void* out, jo::SpEntry* e) {
        if (cnt == 0) {
            reinterpret_cast<u32*>(out)[cnt * 2] = e->key;
            u32 flag_word;  // the original reads key_flag (u8) as a dword: key_flag + gap_5 (same bytes on x64)
            std::memcpy(&flag_word, &e->key_flag, sizeof flag_word);
            reinterpret_cast<u32*>(out)[cnt * 2 + 1] = flag_word;
            dists[cnt] = *d;
            ++cnt;
        } else if (cnt < n || *d < dists[n - 1]) {
            if (cnt >= n) cnt = n - 1;
            i32 pos;
            joan::orig::as_misc(kArrays)->bin_search_5911a0(joan::orig::arg::mptr(d), joan::orig::arg::str(dists), joan::orig::arg::i32(cnt), 4, fn::code_value(fn::cmp_int_682a70), &pos);  // 0x005911a0
            joan::orig::as_misc(kArrays)->insert_array_rec_5910f0(joan::orig::arg::str(dists), joan::orig::arg::i32(cnt), 4, joan::orig::arg::i32(pos), joan::orig::arg::mptr(static_cast<void*>(d)));          // 0x005910f0
            joan::orig::as_misc(kArrays)->insert_array_rec_5910f0(joan::orig::arg::str(out), joan::orig::arg::i32(cnt), 8, joan::orig::arg::i32(pos), joan::orig::arg::mptr(e));
            ++cnt;
        }
    }
    i32 nearest(void* out, i32 x, i32 y, i32 n, i32 max_d) {
        c_nearest.hit();
        i32 cnt = 0;
        if (max_d == 0) max_d = jo::view<i32>(jo::self<jo::SpIndex>(this)->width) + jo::view<i32>(jo::self<jo::SpIndex>(this)->height);
        Blob blob;
        joan::orig::as_blob(&blob)->construct(joan::orig::arg::i32(static_cast<u32>(n) << 2));  // 0x0057f770
        i32* const dists = joan::orig::arg::as<i32*>(joan::orig::as_blob(&blob)->data_56de20());             // 0x0056de20
        i32 c0, r0, c1, r1;
        void* const list = &jo::self<jo::SpIndex>(this)->entries;  // +0x18
        if (joan::orig::arg::as<i32>(joan::orig::as_spatial_index(this)->cells_for_rect(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(x), joan::orig::arg::i32(y), &c0, &r0, &c1, &r1)) != 0) {  // 0x00683110
            const i32 cx = c0, cy = r0;
            for (i32 ring = 1; ring <= 0x19; ++ring) {
                i32 dx, dy;
                joan::orig::as_misc(kArrays)->move_around_a_point_cached_5913b0(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(ring))), joan::orig::arg::i16(1), joan::orig::arg::i16(1), &dx, &dy);  // 0x005913b0
                i32 lim = max_d;
                if (cnt >= n && dists[n - 1] < lim) lim = dists[n - 1];
                bool stop = false;
                if (abs_(dx) > 1 && static_cast<i32>(static_cast<u32>(abs_(dx) - 1) * jo::self<jo::SpIndex>(this)->cell_size) > lim) {
                    stop = true;
                } else if (abs_(dy) > 1 && static_cast<i32>(static_cast<u32>(abs_(dy) - 1) * jo::self<jo::SpIndex>(this)->cell_size) > lim) {
                    stop = true;
                }
                if (stop) break;
                const i32 gx = dx + cx, gy = dy + cy;
                if (gx < 0 || gy < 0 || gx >= jo::view<i32>(jo::self<jo::SpIndex>(this)->cols) || gy >= jo::view<i32>(jo::self<jo::SpIndex>(this)->rows)) continue;
                const u32* const head = joan::orig::arg::as<u32*>(joan::orig::as_spatial_index(this)->cell(joan::orig::arg::i32(gx), joan::orig::arg::i32(gy)));  // 0x00683280
                u32 node = *head;
                while (node != 0) {
                    jo::SpEntry* const e = jo::as_sp_entry(joan::orig::as_dynarray(list)->get(joan::orig::arg::i32(node)));  // 0x00417200
                    const u32 next = joan::orig::arg::as<u32>(joan::orig::as_grid(e)->at(joan::orig::arg::i32(gx), joan::orig::arg::i32(gy)));    // 0x00683d70
                    node = next;
                    // x range filter
                    if (e->x0 != e->x1) {
                        if (e->x1 < cx && gx != e->x1) continue;
                        if (cx < e->x0 && gx != e->x0) continue;
                        if (!(e->x0 > cx || cx > e->y0 || gx == cx)) continue;
                    }
                    // y range filter
                    if (e->y0 != e->y1) {
                        if (e->y1 < cy && gy != e->y1) continue;
                        if (cy < e->y0 && gy != e->y0) continue;
                        if (!(e->y0 > cy || cy > e->y0 || gy == cy)) continue;
                    }
                    i32 d = joan::orig::arg::as<i32>(joan::orig::as_misc(kArrays)->area_distance_591e70(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(jo::view<u32>(e->a)), joan::orig::arg::i32(jo::view<u32>(e->b)), joan::orig::arg::i32(jo::view<u32>(e->c)), joan::orig::arg::i32(jo::view<u32>(e->d))));  // 0x00591e70
                    if (d > lim) continue;
                    insert_sorted(cnt, n, dists, &d, out, e);
                }
            }
        } else {
            for (i32 k = joan::orig::as_dynarray(list)->size(); k > 0; --k) {  // 0x00417ad0
                if (joan::orig::arg::as<i32>(joan::orig::as_spatial_index(this)->slot_free(joan::orig::arg::i32(k))) != 0) continue;      // 0x00683650
                jo::SpEntry* const e = jo::as_sp_entry(joan::orig::as_dynarray(list)->get(joan::orig::arg::i32(k)));
                i32 d = joan::orig::arg::as<i32>(joan::orig::as_misc(kArrays)->area_distance_591e70(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(jo::view<u32>(e->a)), joan::orig::arg::i32(jo::view<u32>(e->b)), joan::orig::arg::i32(jo::view<u32>(e->c)), joan::orig::arg::i32(jo::view<u32>(e->d))));
                if (max_d != 0 && d > max_d) continue;
                insert_sorted(cnt, n, dists, &d, out, e);
            }
        }
        const i32 result = cnt;
        joan::orig::as_blob(&blob)->destruct();  // 0x0057f850
        return result;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_12", name, addr, fn)
B("SpatialIndex::search_nearest_obj", 0x00682a90, &H::nearest);
#undef B

}  // namespace game_b104_12
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
//
//    - SpatialIndex (uti3d/spindex.cpp): cells_for_rect 0x00683110, cell 0x00683280,
//      slot_free 0x00683650, Grid::at 0x00683d70;
namespace {
namespace game_b90_06 {

JOAN_COUNTER(c_cells, "SpatialIndex::cells_for_rect");

struct H {
    // 0x00683110 SpatialIndex::cells_for_rect: +0 cell size, +4 / +8 width / height,
    // +0xc / +0x10 cell counts.
    std::int32_t cells_for_rect(std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1, std::int32_t* cx0,
                                std::int32_t* cy0, std::int32_t* cx1, std::int32_t* cy1) {
        c_cells.hit();
        if (x0 > x1 || y0 > y1) return 0;
        if (x0 < 0)
            *cx0 = 0;
        else if (x0 >= jo::view<std::int32_t>(jo::self<jo::SpIndex>(this)->width))
            *cx0 = jo::view<std::int32_t>(jo::self<jo::SpIndex>(this)->cols) - 1;
        else
            *cx0 = static_cast<std::int32_t>(static_cast<std::uint32_t>(x0) / jo::self<jo::SpIndex>(this)->cell_size);
        if (y0 < 0)
            *cy0 = 0;
        else if (y0 >= jo::view<std::int32_t>(jo::self<jo::SpIndex>(this)->height))
            *cy0 = jo::view<std::int32_t>(jo::self<jo::SpIndex>(this)->rows) - 1;
        else
            *cy0 = static_cast<std::int32_t>(static_cast<std::uint32_t>(y0) / jo::self<jo::SpIndex>(this)->cell_size);
        if (x1 < 0)
            *cx1 = 0;
        else if (x1 >= jo::view<std::int32_t>(jo::self<jo::SpIndex>(this)->width))
            *cx1 = jo::view<std::int32_t>(jo::self<jo::SpIndex>(this)->cols) - 1;
        else
            *cx1 = static_cast<std::int32_t>(static_cast<std::uint32_t>(x1) / jo::self<jo::SpIndex>(this)->cell_size);
        if (y1 < 0)
            *cy1 = 0;
        else if (y1 >= jo::view<std::int32_t>(jo::self<jo::SpIndex>(this)->height))
            *cy1 = jo::view<std::int32_t>(jo::self<jo::SpIndex>(this)->rows) - 1;
        else
            *cy1 = static_cast<std::int32_t>(static_cast<std::uint32_t>(y1) / jo::self<jo::SpIndex>(this)->cell_size);
        return 1;
    }
    // 0x00683280 SpatialIndex::cell(x, y): +0x14 cells (8 bytes each), row length +0xc.
    void* cell(std::int32_t x, std::int32_t y) {
        const std::uint32_t idx = static_cast<std::uint32_t>(y) * jo::self<jo::SpIndex>(this)->cols + static_cast<std::uint32_t>(x);
        return reinterpret_cast<void*>(jo::view<std::uintptr_t>(jo::self<jo::SpIndex>(this)->cells) + idx * 8u);
    }
    // 0x00683650 SpatialIndex::slot_free(n): list +0x18 element n missing or its +8 is 0.
    std::int32_t slot_free(std::int32_t n) {
        jo::SpEntry* const e = jo::as_sp_entry(joan::orig::as_dynarray(&jo::self<jo::SpIndex>(this)->entries)->get(joan::orig::arg::i32(n)));  // 0x00417200
        std::int32_t r;
        if (e != nullptr && e->active != 0)
            r = 0;
        else
            r = 1;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("SpatialIndex::calc_zone", 0x00683110, &H::cells_for_rect);
B("SpatialIndex::get_zone_info", 0x00683280, &H::cell);
B("SpatialIndex::is_deleted", 0x00683650, &H::slot_free);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
//
//      0x00683910 SpIndex::init(...)            (thiscall, ret 0x24) - 2 grids n = w*h dwords.
namespace {
namespace game_b96_08 {

JOAN_COUNTER(c_spindex_init, "SpIndex::init");

struct H {

    // ------------------------------------------------------------------ 0x00683910
    void* spindex_init(const jo::Key* src, i32 a, i32 b, i32 c, i32 d, i32 x0, i32 y0, i32 x1, i32 y1) {
        c_spindex_init.hit();
        jo::self<jo::SpEntry>(this)->key = src->code;
        jo::self<jo::SpEntry>(this)->key_flag = src->kind;
        jo::self<jo::SpEntry>(this)->a = a;
        jo::self<jo::SpEntry>(this)->b = b;
        jo::self<jo::SpEntry>(this)->c = c;
        jo::self<jo::SpEntry>(this)->d = d;
        jo::self<jo::SpEntry>(this)->x0 = x0;
        jo::self<jo::SpEntry>(this)->y0 = y0;
        jo::self<jo::SpEntry>(this)->x1 = x1;
        jo::self<jo::SpEntry>(this)->y1 = y1;
        jo::self<jo::SpEntry>(this)->grid_a = nullptr;
        jo::self<jo::SpEntry>(this)->grid_b = nullptr;
        jo::self<jo::SpEntry>(this)->capacity = 4;
        const i32 n = static_cast<i32>((static_cast<u32>(x1) - static_cast<u32>(x0) + 1u) *
                                       (static_cast<u32>(y1) - static_cast<u32>(y0) + 1u));
        if (jo::self<jo::SpEntry>(this)->capacity < n) {
            jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_a) = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(static_cast<u32>(n) << 2), str(d::s_d_ww_uti3d_spindex_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a218a8)) + 0x18));  // 0x0058e7b0
            jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_b) = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(static_cast<u32>(n) << 2), str(d::s_d_ww_uti3d_spindex_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a218a8)) + 0x19));  // 0x0058e7b0
            jo::self<jo::SpEntry>(this)->capacity = n;
        }
        if (jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_a) == nullptr)
            memset_(jo::self<jo::SpEntry>(this)->inline_a, 0, static_cast<u32>(n) << 2);
        else
            memset_(jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_a), 0, static_cast<u32>(n) << 2);
        if (jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_b) == nullptr) return memset_(jo::self<jo::SpEntry>(this)->inline_b, 0, static_cast<u32>(n) << 2);
        return memset_(jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_b), 0, static_cast<u32>(n) << 2);
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("SpatialIndex::SPObjRefStore::init", 0x00683910, &H::spindex_init);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
//
//      0x00683b00 SpIndex::free_buffers()                       thiscall (spindex.cpp, frees +0x2c / +0x40)
namespace {
namespace game_b92_06 {

void* const kMemory = obj(d::g_mem);

struct H {
    // 0x00683b00
    H* free_buffers() {
        if (jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_a) != nullptr)
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_a)), str(d::s_d_ww_uti3d_spindex_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a218ac)) + 2));  // 0x0058ed70
        jo::self<jo::SpEntry>(this)->grid_a = nullptr;
        if (jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_b) != nullptr)
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::SpEntry>(this)->grid_b)), str(d::s_d_ww_uti3d_spindex_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a218ac)) + 5));
        jo::self<jo::SpEntry>(this)->grid_b = nullptr;
        jo::self<jo::SpEntry>(this)->capacity = 4;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("SpatialIndex::SPObjRefStore::deinit", 0x00683b00, &H::free_buffers);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
//
//      0x00683eb0 SpIndex::set_a(x, y, v)                       thiscall, ret 0xc
//
//      0x00683f60 SpIndex::set_b(x, y, v)                       thiscall, ret 0xc
namespace {
namespace game_b92_06_2 {


std::uint32_t u(std::int32_t v) { return static_cast<std::uint32_t>(v); }
using joan::orig::helpers::s;

struct H {
    std::int32_t grid_index(std::int32_t x, std::int32_t y) {
        const std::uint32_t row = u(jo::self<jo::SpEntry>(this)->x1) - u(jo::self<jo::SpEntry>(this)->x0) + 1u;
        return s((u(y) - u(jo::self<jo::SpEntry>(this)->y0)) * row + (u(x) - u(jo::self<jo::SpEntry>(this)->x0)));
    }

    // 0x00683eb0
    void set_a(std::int32_t x, std::int32_t y, std::int32_t v) {
        const std::int32_t i = grid_index(x, y);
        if (jo::self<jo::SpEntry>(this)->grid_a != nullptr)
            jo::self<jo::SpEntry>(this)->grid_a[i].value = v;
        else
            jo::self<jo::SpEntry>(this)->inline_a[i] = v;
    }

    // 0x00683f60
    void set_b(std::int32_t x, std::int32_t y, std::int32_t v) {
        const std::int32_t i = grid_index(x, y);
        if (jo::self<jo::SpEntry>(this)->grid_b != nullptr)
            jo::self<jo::SpEntry>(this)->grid_b[i].value = v;
        else
            jo::self<jo::SpEntry>(this)->inline_b[i] = v;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("SpIndex::set_a", 0x00683eb0, &H::set_a);
B("SpIndex::set_b", 0x00683f60, &H::set_b);
#undef B

}  // namespace game_b92_06_2
}  // namespace

// ---------------------------------------- from modules/game_b94_07.cpp (game/b94_07)
//
//      0x006834c0 Grid::link_range(...)         (thiscall, ret 0x14) - returns the final j.
namespace {
namespace game_b94_07 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;

JOAN_COUNTER(c_link_range, "Grid::link_range");

struct H {
    // ------------------------------------------------------------------ 0x006834c0
    i32 link_range(i32 a, i32 i0, i32 j0, i32 i1, i32 j1) {
        c_link_range.hit();
        void* const list = &jo::self<jo::SpIndex>(this)->entries;  // +0x18
        void* const cell = joan::orig::arg::as<void*>(joan::orig::as_dynarray(list)->get(joan::orig::arg::i32(a)));  // 0x00417200
        i32 j = j0;
        for (; j <= j1; ++j) {
            for (i32 i = i0; i <= i1; ++i) {
                i32* const link = joan::orig::arg::as<i32*>(joan::orig::as_spatial_index(this)->cell(joan::orig::arg::i32(i), joan::orig::arg::i32(j)));  // 0x00683280
                const i32 r18 = joan::orig::arg::as<i32>(joan::orig::as_grid(cell)->cell(joan::orig::arg::i32(i), joan::orig::arg::i32(j)));    // 0x00683e10
                const i32 r1c = joan::orig::arg::as<i32>(joan::orig::as_grid(cell)->at(joan::orig::arg::i32(i), joan::orig::arg::i32(j)));    // 0x00683d70
                if (r18 == 0) {
                    link[0] = r1c;
                } else {
                    void* const o = joan::orig::arg::as<void*>(joan::orig::as_dynarray(list)->get(joan::orig::arg::i32(r18)));  // 0x00417200
                    joan::orig::as_sp_index(o)->set_a(joan::orig::arg::i32(i), joan::orig::arg::i32(j), joan::orig::arg::i32(r1c));                     // 0x00683eb0
                }
                if (r1c == 0) {
                    link[1] = r18;
                } else {
                    void* const o = joan::orig::arg::as<void*>(joan::orig::as_dynarray(list)->get(joan::orig::arg::i32(r1c)));  // 0x00417200
                    joan::orig::as_sp_index(o)->set_b(joan::orig::arg::i32(i), joan::orig::arg::i32(j), joan::orig::arg::i32(r18));                     // 0x00683f60
                }
                joan::orig::as_sp_index(cell)->set_b(joan::orig::arg::i32(i), joan::orig::arg::i32(j), 0);  // 0x00683f60
                joan::orig::as_sp_index(cell)->set_a(joan::orig::arg::i32(i), joan::orig::arg::i32(j), 0);  // 0x00683eb0
            }
        }
        return j;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("SpatialIndex::remove_from_zone", 0x006834c0, &H::link_range);
#undef B94

}  // namespace game_b94_07
}  // namespace

// ---------------------------------------- from modules/game_b94_07_2.cpp (game/b94_07)
namespace {
namespace game_b94_07_2 {

using i32 = std::int32_t;
using u8 = std::uint8_t;


struct H {
    // ------------------------------------------------------------------ 0x00681ea0
    void* grid_ctor() {
        joan::orig::dynarray_b(&jo::self<jo::SpIndex>(this)->entries)->construct(static_cast<std::int32_t>(sizeof(jo::SpEntry)), 0x1e, 0, 0);  // 0x005856a0
        jo::self<jo::SpIndex>(this)->cells = nullptr;
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("SpatialIndex::SpatialIndex", 0x00681ea0, &H::grid_ctor);
#undef B94

}  // namespace game_b94_07_2
}  // namespace

// ---------------------------------------- from modules/game_b90_06_2.cpp (game/b90_06)
namespace {
namespace game_b90_06_2 {


struct H {
    // 0x00683d70 Grid::at(x, y): origin +0x1c / +0x20, max x +0x24, cells at *(+0x2c) or
    // inline at +0x30.
    std::uint32_t grid_at(std::int32_t x, std::int32_t y) {
        const std::uint32_t idx =
            (static_cast<std::uint32_t>(y) - jo::view<std::uint32_t>(jo::self<jo::SpEntry>(this)->y0)) * (jo::view<std::uint32_t>(jo::self<jo::SpEntry>(this)->x1) - jo::view<std::uint32_t>(jo::self<jo::SpEntry>(this)->x0) + 1u) +
            (static_cast<std::uint32_t>(x) - jo::view<std::uint32_t>(jo::self<jo::SpEntry>(this)->x0));
        if (jo::self<jo::SpEntry>(this)->grid_a != nullptr)
            return *reinterpret_cast<std::uint32_t*>(jo::view<std::uintptr_t>(jo::self<jo::SpEntry>(this)->grid_a) + idx * 4u);
        return *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(jo::self<jo::SpEntry>(this)->inline_a) + idx * 4u);  // +0x30
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("SpatialIndex::SPObjRefStore::get_next_obj", 0x00683d70, &H::grid_at);
#undef B

}  // namespace game_b90_06_2
}  // namespace

// ---------------------------------------- from modules/game_b93_05.cpp (game/b93_05)
namespace {
namespace game_b93_05_2 {


struct H {
    // 0x00683e10
    std::uint32_t cell(std::int32_t x, std::int32_t y) {
        const std::uint32_t idx = static_cast<std::uint32_t>(y - jo::self<jo::SpEntry>(this)->y0) *
                                      static_cast<std::uint32_t>(jo::self<jo::SpEntry>(this)->x1 - jo::self<jo::SpEntry>(this)->x0 + 1) +
                                  static_cast<std::uint32_t>(x - jo::self<jo::SpEntry>(this)->x0);
        if (jo::self<jo::SpEntry>(this)->grid_b != nullptr) return jo::view<std::uint32_t*>(jo::self<jo::SpEntry>(this)->grid_b)[static_cast<std::int32_t>(idx)];
        return reinterpret_cast<std::uint32_t*>(jo::self<jo::SpEntry>(this)->inline_b)[static_cast<std::int32_t>(idx)];
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b93_05", name, addr, fn)
GAME("SpatialIndex::SPObjRefStore::get_prev_obj", 0x00683e10, &H::cell);
#undef GAME

}  // namespace game_b93_05_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Grid::Grid_681ea0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_07_2::H::grid_ctor>(this)); }  // 0x681ea0
auto joan::orig::SpIndex::dtor() -> void { joan::bind::member<&::game_b93_05::H::spindex_dtor>(this); }  // 0x681ee0
auto joan::orig::SpIndex::init_681fe0(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_09::H::sp_init>(this, a0, a1, a2)); }  // 0x681fe0
auto joan::orig::SpIndex::reset() -> void { joan::bind::member<&::game_b97_08::H::spindex_reset>(this); }  // 0x6820d0
auto joan::orig::SpIndex::insert(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::sp_insert>(this, a0, a1, a2, a3, a4)); }  // 0x6821d0
auto joan::orig::SpIndex::remove_matching(const void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_15::H::remove_matching>(this, a0)); }  // 0x682370
auto joan::orig::Grid::remove(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_08::H::grid_remove>(this, a0)); }  // 0x682410
auto joan::orig::SpIndex::move_by_key(const void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_09::H::sp_move_by_key>(this, a0, a1, a2, a3, a4)); }  // 0x682560
auto joan::orig::SpIndex::move(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::sp_move>(this, a0, a1, a2, a3, a4)); }  // 0x682610
auto joan::orig::SpIndex::query(void* a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_14::H::query>(this, a0, a1, a2, a3, a4)); }  // 0x682800
auto joan::orig::SpIndex::rects_overlap(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_15::H::rects_overlap>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x6829e0
auto joan::orig::SpatialIndex::cells_for_rect(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, void* a4, void* a5, void* a6, void* a7) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_06::H::cells_for_rect>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x683110
auto joan::orig::SpatialIndex::cell(std::int32_t a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_06::H::cell>(this, a0, a1)); }  // 0x683280
auto joan::orig::SpIndex::walk_chain(const void* a0, std::uint32_t a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_15::H::walk_chain>(this, a0, a1, a2)); }  // 0x6832c0
auto joan::orig::SpIndex::fill_rect(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> void { joan::bind::member<&::game_b93_05::H::fill_rect>(this, a0, a1, a2, a3, a4); }  // 0x683350
auto joan::orig::Grid::link_range(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_07::H::link_range>(this, a0, a1, a2, a3, a4)); }  // 0x6834c0
auto joan::orig::SpatialIndex::slot_free(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_06::H::slot_free>(this, a0)); }  // 0x683650
auto joan::orig::Key::equals(void* a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b93_05::H::equals>(this, a0)); }  // 0x683850
auto joan::orig::Key::differs(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_09::H::differs>(this, a0)); }  // 0x6838b0
auto joan::orig::SpIndex::init_683910(void* a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_08::H::spindex_init>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x683910
auto joan::orig::SpIndex::free_buffers() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_06::H::free_buffers>(this)); }  // 0x683b00
auto joan::orig::SpIndex::resize(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7) -> void { joan::bind::member<&::game_b97_08::H::spindex_resize>(this, a0, a1, a2, a3, a4, a5, a6, a7); }  // 0x683bb0
auto joan::orig::Grid::at(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_06_2::H::grid_at>(this, a0, a1)); }  // 0x683d70
auto joan::orig::Grid::cell(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b93_05_2::H::cell>(this, a0, a1)); }  // 0x683e10
auto joan::orig::SpIndex::set_a(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::member<&::game_b92_06_2::H::set_a>(this, a0, a1, a2); }  // 0x683eb0
auto joan::orig::SpIndex::set_b(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::member<&::game_b92_06_2::H::set_b>(this, a0, a1, a2); }  // 0x683f60
