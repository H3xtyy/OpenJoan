// game/unknown_inventory.cpp: functions whose original source file is unknown (formerly game/_unplaced/inventory.cpp;
// re/attribution.csv: between ww/camres.cpp and ww/cpuusage.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::img::obj;

using i32 = std::int32_t;

using i16 = std::int16_t;

using joan::call::cdecl_call;

using u8 = std::uint8_t;

using u32 = std::uint32_t;

using u16 = std::uint16_t;

using i8 = std::int8_t;

void* const kItemTypes = obj(d::g_item_res);

i32 sb(u8 b) { return static_cast<i32>(static_cast<i8>(b)); }  // byte argument

i32 inv_size(void* inv) { return joan::orig::as_dynarray(inv)->size(); }  // 0x00417ad0

u8* item_type(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItemTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0 (word)

}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

using joan::call::thiscall;


using joan::orig::helpers::memset_;

void* const kRes = obj(d::g_item_res);

JOAN_COUNTER(c_set_state, "Obj::set_state_457120");

struct H {
    void set_2e(std::uint32_t v) { jo::view<std::uint32_t>(jo::self<jo::WorldItem>(this)->value) = v; }  // 0x004571b0
    void* obj_ctor() {  // 0x00456eb0
        memset_(jo::self<jo::WorldItem>(this)->pos, 0, sizeof(jo::WorldItem) - sizeof(jo::HeldItem));  // all after the stack: +0x12, 0x40 bytes on i686
        return this;
    }
    std::int32_t set_pos(std::uint32_t x, std::uint32_t y, std::uint32_t z) {  // 0x00456fe0
        jo::view<std::uint32_t>(jo::self<jo::WorldItem>(this)->pos[0]) = x;
        jo::view<std::uint32_t>(jo::self<jo::WorldItem>(this)->pos[1]) = y;
        jo::view<std::uint32_t>(jo::self<jo::WorldItem>(this)->pos[2]) = z;
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_effect(this)->attach_zone());  // 0x00457070
    }

    // ---- 0x00457120 Obj::set_state ----------------------------------------------------------
    void set_state(jo::World* state) {
        c_set_state.hit();
        if (jo::self<jo::WorldItem>(this)->world == state) return;
        if (jo::view<void*>(jo::self<jo::WorldItem>(this)->sector) != nullptr) joan::orig::as_sector(jo::view<void*>(jo::self<jo::WorldItem>(this)->sector))->unlink_object(joan::orig::arg::mptr(this));  // 0x0054de20
        if (jo::view<void*>(jo::self<jo::WorldItem>(this)->cell) != nullptr) joan::orig::as_world_cell(jo::view<void*>(jo::self<jo::WorldItem>(this)->cell))->unlink_object(joan::orig::arg::mptr(this));  // 0x0054f580
        jo::self<jo::WorldItem>(this)->world = state;
    }

    // ---- 0x00456cd0 Obj::init(type, b, c, -, -, -) ret 0x18 -------------------------------------
    void obj_init(std::int16_t type, std::int16_t b, std::int16_t c, std::uint32_t, std::uint32_t, std::uint32_t) {
        jo::self<jo::InvItem>(this)->type = type;
        std::int16_t* const res = joan::orig::arg::as<std::int16_t*>(joan::orig::as_records_2bf(kRes)->get(joan::orig::arg::i16(type)));  // 0x00425ce0
        volatile std::int32_t dead = *res;  // [ebp-0xc], unused
        (void)dead;
        jo::self<jo::InvItem>(this)->count = c;
        jo::self<jo::InvItem>(this)->u32_6 = 0;
        jo::self<jo::InvItem>(this)->u32_a = 0;
        jo::self<jo::InvItem>(this)->u32_e = 0;
        jo::self<jo::InvItem>(this)->level = b;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("Obj::set_state_457120", 0x00457120, &H::set_state);
B("Obj::init_456cd0", 0x00456cd0, &H::obj_init);
B("Obj::set_pos_456fe0", 0x00456fe0, &H::set_pos);
B("Obj::ctor_456eb0", 0x00456eb0, &H::obj_ctor);
B("Obj::set_2e", 0x004571b0, &H::set_2e);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01 {

using joan::call::thiscall;

using joan::orig::helpers::game_delete;                      // operator delete

using joan::orig::helpers::s16;
using joan::orig::helpers::s8;

void* const kItems = obj(d::g_item_res);   // 0x00425ce0

JOAN_COUNTER(c_take, "Stack::take");
JOAN_COUNTER(c_grid_free, "Grid::free");
JOAN_COUNTER(c_grid_find, "Grid::find_place");
JOAN_COUNTER(c_grid_fill, "Grid::fill");

struct H {
    // ------------------------------------------------------------ 0x00456d60 (ret 4)
    void stack_take(std::uint16_t* n) {
        c_take.hit();
        std::uint8_t* const item =
            joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(kItems)->get(joan::orig::arg::i16(static_cast<std::int16_t>(jo::self<jo::ItemStack>(this)->item))));  // 0x00425ce0
        const std::int32_t room = s16(jo::as_item_def(item)->stats[4]) - s16(jo::self<jo::ItemStack>(this)->count);
        const std::int32_t want = s16(*n);
        const std::uint16_t d = static_cast<std::uint16_t>(want < room ? want : room);
        jo::self<jo::ItemStack>(this)->count = static_cast<std::uint16_t>(jo::self<jo::ItemStack>(this)->count + d);
        *n = static_cast<std::uint16_t>(*n - d);
    }

    // ------------------------------------------------------------ grid (+0x2e rows[i][j] -> bytes)
    std::uint8_t* cell_row(std::int32_t i, std::int32_t j) { return jo::view<std::uint8_t***>(jo::self<jo::Grid>(this)->cells)[i][j]; }

    void grid_free() {  // 0x004573c0
        c_grid_free.hit();
        for (std::int32_t i = 0; i < jo::self<jo::Grid>(this)->rows; ++i) {
            for (std::int32_t j = 0; j < jo::self<jo::Grid>(this)->cols; ++j) game_delete(cell_row(i, j));
            game_delete(jo::view<std::uint8_t***>(jo::self<jo::Grid>(this)->cells)[i]);
        }
        game_delete(jo::view<void*>(jo::self<jo::Grid>(this)->cells));
        jo::view<void*>(jo::self<jo::Grid>(this)->cells) = nullptr;
    }

    // 0x004575a0 (char w, char h, char* out_layer, char* out_x, char* out_y, int value), ret 0x18
    std::int32_t grid_find(std::uint32_t w, std::uint32_t h, std::uint8_t* out_layer, std::uint8_t* out_x,
                           std::uint8_t* out_y, std::int32_t value) {
        c_grid_find.hit();
        for (std::int32_t i = 0; i < jo::self<jo::Grid>(this)->rows; ++i) {
            for (std::int32_t j = 0; j <= jo::self<jo::Grid>(this)->cols - s8(h); ++j) {
                std::uint8_t* const row = cell_row(i, j);
                for (std::int32_t k = 0; k <= jo::self<jo::Grid>(this)->cell_size - s8(w); ++k) {
                    if (row[k] != 0 && static_cast<std::int32_t>(row[k]) != value) continue;
                    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_grid(this)->area_free(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::int8_t>(w))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::int8_t>(h))), joan::orig::arg::i32(static_cast<std::int8_t>(i)), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::int8_t>(k))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::int8_t>(j))), joan::orig::arg::i32(value))) == 0) {  // 0x004574a0
                        continue;
                    }
                    *out_layer = 0;
                    *out_x = static_cast<std::uint8_t>(k);
                    *out_y = static_cast<std::uint8_t>(j);
                    return 1;
                }
            }
        }
        return 0;
    }

    // 0x00458090 (char w, char h, char layer, char x, char y, char value), ret 0x18
    void grid_fill(std::uint32_t w, std::uint32_t h, std::uint32_t layer, std::uint32_t x, std::uint32_t y,
                   std::uint32_t value) {
        c_grid_fill.hit();
        for (std::int32_t j = s8(y); j < s8(y) + s8(h); ++j) {
            std::uint8_t* const row = cell_row(s8(layer), j);
            for (std::int32_t k = s8(x); k < s8(x) + s8(w); ++k) row[k] = static_cast<std::uint8_t>(value);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("Stack::take", 0x00456d60, &H::stack_take);
B("Grid::free", 0x004573c0, &H::grid_free);
B("Grid::find_place", 0x004575a0, &H::grid_find);
B("Grid::fill", 0x00458090, &H::grid_fill);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {


struct H {
    // ------------------------------------------------------------------ 0x00456e10 / 0x00456f80
    void* copy12_456e10(const void* src) { return std::memcpy(static_cast<void*>(this), joan::orig::arg::ptr(src), sizeof(jo::HeldItem)); }  // 0x12 on x86 (the stack record)
    void* copy12_456f80(const void* src) { return std::memcpy(static_cast<void*>(this), joan::orig::arg::ptr(src), sizeof(jo::HeldItem)); }  // 0x12 on x86 (the stack record)
    // ------------------------------------------------------------------ 0x00457030
    u32 get_pos_457030(u32* out) {
        out[0] = jo::view<u32>(jo::self<jo::WorldItem>(this)->pos[0]);
        out[1] = jo::view<u32>(jo::self<jo::WorldItem>(this)->pos[1]);
        const u32 z = jo::view<u32>(jo::self<jo::WorldItem>(this)->pos[2]);
        out[2] = z;
        return z;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::copy12_456e10", 0x00456e10, &H::copy12_456e10);
B("Obj::copy12_456f80", 0x00456f80, &H::copy12_456f80);
B("Obj::get_pos_457030", 0x00457030, &H::get_pos_457030);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

using joan::call::thiscall;


struct H {
    // ------------------------------------------------------------------ 0x00456e40
    void set_12_13(i32 a, i32 b) {
        jo::view<u8>(jo::self<jo::InvItem>(this)->w) = static_cast<u8>(a);
        jo::view<u8>(jo::self<jo::InvItem>(this)->h) = static_cast<u8>(b);
    }

    // ------------------------------------------------------------------ 0x00456ef0 / 0x00467560
    std::uintptr_t dtor_456ef0() { return joan::orig::arg::as<u32>(joan::orig::as_world_item(this)->detach()); }  // 0x00456f20
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Obj::set_12_13", 0x00456e40, &H::set_12_13);
B98("Obj::dtor_456ef0", 0x00456ef0, &H::dtor_456ef0);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using joan::call::thiscall;
using joan::img::str;


struct H {
    // ------------------------------------------------------------------------ 0x00456e70
    void set_bytes_14(u8 a, u8 b, u8 c) {
        jo::view<u8>(jo::self<jo::InvItem>(this)->u8_14) = a;
        jo::view<u8>(jo::self<jo::InvItem>(this)->x) = b;
        jo::view<u8>(jo::self<jo::InvItem>(this)->y) = c;
    }

    // ------------------------------------------------------------------------ 0x00457e00
    i32 lookup_find(const char* key) {  // `this` is the Se object g_a58b00 (see init_457ad0)
        if (jo::self<jo::Se>(this)->enabled == 0) return 0;
        i32 r = joan::orig::arg::as<i32>(joan::orig::as_res_idx_l(&jo::self<jo::Se>(this)->archive)->get_index(joan::orig::arg::str(key)));  // 0x0059be20
        if (r != 0) return r;
        r = joan::orig::arg::as<i32>(joan::orig::as_res_idx_l(jo::self<jo::Se>(this)->ext_archive)->get_index(joan::orig::arg::str(key)));
        if (r != 0) return r + jo::self<jo::Se>(this)->count_main;
        return 0;
    }

    // ------------------------------------------------------------------------ 0x00458130
    void grid_fill(u8 a, u8 b, u8 c, u8 d, u8 e) {
        joan::orig::as_grid(this)->fill_458090(joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d), joan::orig::arg::u32(e), joan::orig::arg::u32(static_cast<u8>(0)));  // 0x00458090
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Obj::set_bytes_14", 0x00456e70, &H::set_bytes_14);
B93("Lookup::find", 0x00457e00, &H::lookup_find);
B93("Grid::fill", 0x00458130, &H::grid_fill);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x00456f20
    // eax = this: either reloaded at 0x00456f44, or 0x0054f580's last eax (its obj argument = this).
    void* detach() {
        if (jo::self<jo::WorldItem>(this)->sector != nullptr) joan::orig::as_sector(jo::self<jo::WorldItem>(this)->sector)->unlink_object(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x0054de20
        if (jo::self<jo::WorldItem>(this)->cell != nullptr) joan::orig::as_world_cell(jo::self<jo::WorldItem>(this)->cell)->unlink_object(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x0054f580
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Obj::detach", 0x00456f20, &H::detach);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

using joan::call::thiscall;

struct H {
    std::uint32_t wrap_456fb0() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_effect(this)->attach_zone()); }  // 0x00457070
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("wrap_00456fb0", 0x00456fb0, &H::wrap_456fb0);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

using joan::call::thiscall;

JOAN_COUNTER(c_attach_zone, "Effect::attach_zone");

struct H {
    // 0x00457070 Effect::attach_zone: unless the current zone +0x3a still contains +0x12,
    // asks the zone list +0x36 for the zone at +0x12 and attaches (0x0042c140). Returns eax as
    // the original on every path.
    std::uintptr_t attach_zone() {
        c_attach_zone.hit();
        if (jo::view<void*>(jo::self<jo::WorldItem>(this)->cell) != nullptr) {
            const std::uint32_t in = joan::orig::arg::as<std::uint32_t>(joan::orig::as_aabb(static_cast<void*>(jo::self<jo::WorldItem>(this)->cell->box))->contains(joan::orig::arg::ptr(jo::self<jo::WorldItem>(this)->pos))) & 0xffu;  // 0x0060d5f0
            if (in != 0) return in;
        }
        if (jo::view<void*>(jo::self<jo::WorldItem>(this)->world) == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        std::int32_t n = 0;
        alignas(4) std::uint8_t box[0x18];
        joan::orig::as_aabb(static_cast<void*>(box))->set_point(joan::orig::arg::ptr(jo::self<jo::WorldItem>(this)->pos));  // 0x0060d430
        void** const r = joan::orig::arg::as<void**>(joan::orig::as_world(jo::view<void*>(jo::self<jo::WorldItem>(this)->world))->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &n));  // 0x00429090
        if (n <= 0) return reinterpret_cast<std::uintptr_t>(r);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_map_region(*r)->link_object(joan::orig::arg::mptr(static_cast<void*>(this))));  // 0x0042c140
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Effect::attach_zone", 0x00457070, &H::attach_zone);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {


struct H {
    float value_457190() { return jo::view<float>(jo::self<jo::WorldItem>(this)->value); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("Obj::value_457190", 0x00457190, &H::value_457190);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {


using joan::orig::helpers::game_new;  // operator new

JOAN_COUNTER(c_grid_alloc, "Grid::alloc");

struct H {
    // 0x00457290
    void grid_alloc(std::int32_t n, std::int32_t size, std::int32_t m) {
        c_grid_alloc.hit();
        jo::self<jo::Grid>(this)->rows = n;
        jo::self<jo::Grid>(this)->cell_size = size;
        jo::self<jo::Grid>(this)->cols = m;
        jo::view<void*>(jo::self<jo::Grid>(this)->cells) = game_new(static_cast<std::uint32_t>(n * static_cast<std::int32_t>(sizeof(void*))));  // n row pointers (n << 2 on i686)
        for (std::int32_t i = 0; i < n; ++i) {
            void* const row = game_new(static_cast<std::uint32_t>(m * static_cast<std::int32_t>(sizeof(void*))));  // m cell pointers
            jo::view<void**>(jo::self<jo::Grid>(this)->cells)[i] = row;
            for (std::int32_t j = 0; j < m; ++j) {
                void* const cell = game_new(static_cast<std::uint32_t>(size));
                static_cast<void**>(jo::view<void**>(jo::self<jo::Grid>(this)->cells)[i])[j] = cell;
                std::memset(joan::orig::arg::mptr(static_cast<void**>(jo::view<void**>(jo::self<jo::Grid>(this)->cells)[i])[j]), 0, joan::orig::arg::u32(size));  // memset
            }
        }
    }

    // 0x004571e0
    void* grid_ctor() {
        joan::orig::dynarray_b(this)->construct(0x18, 0x1e, 0, 0);  // 0x005856a0
        jo::self<jo::Grid>(this)->cells = nullptr;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("Grid::alloc", 0x00457290, &H::grid_alloc);
B("Grid::Grid", 0x004571e0, &H::grid_ctor);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x00457220
    void dtor_457220() {
        joan::orig::as_grid(this)->free();  // 0x004573c0
        joan::orig::dynarray_b(this)->destruct();  // 0x00585710
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::~Obj_457220", 0x00457220, &H::dtor_457220);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::call::thiscall;

using joan::call::thiscall;

JOAN_COUNTER(c_area_free, "Grid::area_free");

struct H {
    // 0x004574a0
    std::int32_t area_free(std::int8_t w, std::int8_t h, std::int32_t, std::int8_t x, std::int8_t y, std::int32_t owner) {
        c_area_free.hit();
        if (x < 0 || x + w > jo::self<jo::Grid>(this)->cell_size) return 0;
        if (y < 0 || y + h > jo::self<jo::Grid>(this)->cols) return 0;
        for (std::int32_t j = y + h - 1; j >= y; --j) {
            for (std::int32_t i = x + w - 1; i >= x; --i) {
                const std::int32_t v = joan::orig::arg::as<std::int32_t>(joan::orig::as_grid(this)->find_at(joan::orig::arg::i32(i), joan::orig::arg::i32(j)));  // 0x00458170
                if (v != 0 && v != owner) return 0;
            }
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Grid::area_free", 0x004574a0, &H::area_free);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

using joan::call::thiscall;


void* const kSounds = obj(d::g_item_res);

JOAN_COUNTER(c_add_item, "List::add_item");
JOAN_COUNTER(c_count_of, "List::count_of");

i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

struct H {
    // ------------------------------------------------------------------ 0x004576f0
    i32 add_item(u8 a, u8 b, u8 c, u8 d, u8 e) {
        c_add_item.hit();
        joan::orig::dynarray_b(this)->linkin(nullptr);               // 0x005857a0
        const i32 idx = joan::orig::as_dynarray(this)->cursor();  // 0x00417520
        joan::orig::as_grid(this)->fill_458090(joan::orig::arg::u32(static_cast<u32>(a)), joan::orig::arg::u32(static_cast<u32>(b)), joan::orig::arg::u32(static_cast<u32>(c)), joan::orig::arg::u32(static_cast<u32>(d)), joan::orig::arg::u32(static_cast<u32>(e)), joan::orig::arg::u32(idx));  // 0x00458090
        u8* const it = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(idx)));  // 0x00417200
        jo::as_inv_item(it)->placed = static_cast<u8>(idx);
        joan::orig::as_inventory_item(it)->set_size(joan::orig::arg::i32(static_cast<u32>(a)), joan::orig::arg::i32(static_cast<u32>(b)));  // 0x00456e40
        joan::orig::as_inventory_item(it)->set_grid_pos(static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(c))), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(d))), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(e))));  // 0x00456e70
        return idx;
    }

    // ------------------------------------------------------------------ 0x004582e0
    i32 count_of(i32 type) {
        c_count_of.hit();
        i32 sum = 0;
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(this)->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00457f60
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(this)->get(joan::orig::arg::i32(i)));       // 0x00457e90
            u8* const def = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kSounds)->get(joan::orig::arg::i16(sx16(jo::view<u16>(jo::as_inv_item(e)->type)))));  // 0x00425ce0
            if (static_cast<i32>(jo::view<i16>(jo::as_item_def(def)->class_id)) == type) sum += static_cast<i32>(jo::as_inv_item(e)->count);
        }
        return sum;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("List::add_item", 0x004576f0, &H::add_item);
B99("List::count_of", 0x004582e0, &H::count_of);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b115_09.cpp (fix/uninit)
namespace {
namespace game_b115_09 {

using joan::call::thiscall;

i32 inv_free(void* inv, i32 i) { return joan::orig::arg::as<i32>(joan::orig::as_ptr_list(inv)->is_empty_at(joan::orig::arg::i32(i))); }  // 0x00457f60
u8* inv_slot(void* inv, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(inv)->get(joan::orig::arg::i32(i))); }  // 0x00457e90


JOAN_COUNTER(c_inv_add, "Inventory::add_4577a0");

// ======================================================================= 0x004577a0
struct HInv {
    std::uintptr_t add(i32 id_arg, i32 a2, i16* cnt, i32 a4, i32 a5, i32 a6, i32 force) {
        c_inv_add.hit();
        const i16 id = static_cast<i16>(id_arg);
        if (static_cast<i32>(id) <= 0) return 0;
        u8* const type = item_type(static_cast<i32>(id));
        const i32 bw = sb(static_cast<u8>(jo::as_item_def(type)->stats[0]));
        const i32 bh = sb(static_cast<u8>(jo::as_item_def(type)->stats[1]));
        // fix/uninit: written by 0x004575a0 only when it finds a slot; 0 otherwise.
        u8 b14 = 0, b18 = 0, b1c = 0;
        i32 found = joan::orig::arg::as<i32>(joan::orig::as_grid(this)->find_place(joan::orig::arg::u32(bw), joan::orig::arg::u32(bh), &b14, &b18, &b1c, i32{0}));  // 0x004575a0
        if (static_cast<i32>(*cnt) < 0) {
            const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_list(this)->add_item(static_cast<std::uint8_t>(joan::orig::arg::u32(bw)), static_cast<std::uint8_t>(joan::orig::arg::u32(bh)), static_cast<std::uint8_t>(joan::orig::arg::u32(sb(b14))), static_cast<std::uint8_t>(joan::orig::arg::u32(sb(b18))), static_cast<std::uint8_t>(joan::orig::arg::u32(sb(b1c)))));  // 0x004576f0
            u8* const e = inv_slot(this, idx);
            jo::as_inv_item(e)->type = id;
            jo::as_inv_item(e)->count = *cnt;
            jo::view<i32>(jo::as_inv_item(e)->u32_6) = a4;
            jo::view<i32>(jo::as_inv_item(e)->u32_a) = a5;
            jo::view<i32>(jo::as_inv_item(e)->u32_e) = a6;
            jo::as_inv_item(e)->level = static_cast<i16>(a2);
            return 1;
        }
        i32 room = 0;
        for (i32 i = inv_size(this); i > 0; --i) {
            if (inv_free(this, i) != 0) continue;
            u8* const e = inv_slot(this, i);
            if (static_cast<i32>(jo::as_inv_item(e)->type) == static_cast<i32>(id) && jo::view<i32>(jo::as_inv_item(e)->u32_6) == a4 && jo::view<i32>(jo::as_inv_item(e)->u32_a) == a5)
                room = static_cast<i32>(static_cast<u32>(room) + static_cast<u32>(static_cast<i32>(jo::view<i16>(jo::as_item_def(type)->stats[4])) - static_cast<i32>(jo::as_inv_item(e)->count)));
        }
        if (force == 0 && found == 0 && room < static_cast<i32>(*cnt)) return 0;
        for (i32 i = inv_size(this); i > 0; --i) {
            if (inv_free(this, i) != 0) continue;
            u8* const e = inv_slot(this, i);
            if (static_cast<i32>(jo::as_inv_item(e)->type) == static_cast<i32>(id) && jo::view<i32>(jo::as_inv_item(e)->u32_6) == a4 && jo::view<i32>(jo::as_inv_item(e)->u32_a) == a5)
                joan::orig::as_item_stack(e)->take(joan::orig::arg::mptr(cnt));  // 0x00456d60
            if (static_cast<i32>(*cnt) == 0) break;
        }
        // Exit with the count pointer left in eax (original; callers test the result against 0).
        if (static_cast<i32>(*cnt) <= 0 || found == 0)
            return reinterpret_cast<std::uintptr_t>(cnt);
        do {
            const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_list(this)->add_item(static_cast<std::uint8_t>(joan::orig::arg::u32(bw)), static_cast<std::uint8_t>(joan::orig::arg::u32(bh)), static_cast<std::uint8_t>(joan::orig::arg::u32(sb(b14))), static_cast<std::uint8_t>(joan::orig::arg::u32(sb(b18))), static_cast<std::uint8_t>(joan::orig::arg::u32(sb(b1c)))));  // 0x004576f0
            u8* const e = inv_slot(this, idx);
            jo::as_inv_item(e)->type = id;
            jo::as_inv_item(e)->count = static_cast<i32>(*cnt) < static_cast<i32>(jo::view<i16>(jo::as_item_def(type)->stats[4])) ? *cnt : jo::view<i16>(jo::as_item_def(type)->stats[4]);
            jo::view<i32>(jo::as_inv_item(e)->u32_6) = a4;
            jo::view<i32>(jo::as_inv_item(e)->u32_a) = a5;
            jo::view<i32>(jo::as_inv_item(e)->u32_e) = a6;
            jo::as_inv_item(e)->level = static_cast<i16>(a2);
            if (static_cast<i32>(*cnt) > static_cast<i32>(jo::view<i16>(jo::as_item_def(type)->stats[4]))) {
                u8* const t2 = item_type(static_cast<i32>(id));
                *cnt = static_cast<i16>(static_cast<u16>(*cnt) - jo::as_item_def(t2)->stats[4]);
            } else {
                *cnt = 0;
            }
            found = joan::orig::arg::as<i32>(joan::orig::as_grid(this)->find_place(joan::orig::arg::u32(bw), joan::orig::arg::u32(bh), &b14, &b18, &b1c, i32{0}));  // 0x004575a0
        } while (static_cast<i32>(*cnt) > 0 && found != 0);
        return 1;
    }
};

static_assert(sizeof(i16) == 2 && sizeof(float) == 4, "widths");

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: page / x / y bytes of a failed free-slot search placed for count < 0 -> 0 / 0 / 0.
FIX("Inventory::add_4577a0", 0x004577a0, &HInv::add);
#undef FIX

}  // namespace game_b115_09
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using joan::call::thiscall;


JOAN_COUNTER(c_init_457ad0, "Obj::init_457ad0");
JOAN_COUNTER(c_item_remove, "ItemList::remove");

i32 list_count(void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------ 0x00457ad0
    // `this` is the Se object g_a58b00 (sound effects; Lookup::find 0x00457e00 and Res::lookup_45a7e0 0x0045a7e0 read
    // it too): typed as Se so its pointer members (+0x4 entries, +0x8 playing, +0x64 audio) move with Se's x64 layout.
    void* init_457ad0(u8* owner) {
        c_init_457ad0.hit();
        jo::Se* const se = jo::self<jo::Se>(this);
        se->audio = jo::as_audio(owner);
        joan::orig::as_res_idx(&se->archive)->construct();  // 0x004179f0
        se->ext_archive = &jo::as_audio(owner)->wav_res2;  // owner + 0x14c on x86
        se->loaded = 0;
        se->enabled = 0;
        se->entries = nullptr;
        se->playing = nullptr;
        se->count_main = 0;
        se->count_ext = 0;
        se->count = 0;
        jo::view<u32>(se->gap_0) = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x00458430
    i32 item_remove(i32 key, i32 n) {
        c_item_remove.hit();
        i32 done = 0;
        for (i32 i = list_count(this); i > 0; --i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(this)->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00457f60
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(this)->get(joan::orig::arg::i32(i)));       // 0x00457e90
            if (static_cast<i32>(jo::as_inv_item(e)->type) != key) continue;
            if (n > 0) {
                jo::as_inv_item(e)->count = static_cast<i16>(jo::as_inv_item(e)->count - 1);
                if (jo::as_inv_item(e)->count == 0) joan::orig::as_grid(this)->fill_from(joan::orig::arg::i32(i));  // 0x00457ec0
                ++done;
                if (done == n) break;
            } else {
                joan::orig::as_grid(this)->fill_from(joan::orig::arg::i32(i));
            }
        }
        return 1;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::init_457ad0", 0x00457ad0, &H::init_457ad0);
B95("ItemList::remove", 0x00458430, &H::item_remove);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
namespace {
namespace game_b100_02 {

using joan::call::thiscall;

i32 inv_free(void* inv, i32 i) { return joan::orig::arg::as<i32>(joan::orig::as_ptr_list(inv)->is_empty_at(joan::orig::arg::i32(i))); }  // 0x00457f60
u8* inv_slot(void* inv, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(inv)->get(joan::orig::arg::i32(i))); }  // 0x00457e90


JOAN_COUNTER(c_inv_add, "Inventory::add");

struct H {
    // ------------------------------------------------------------------ 0x00457ba0
    i32 inv_add(i32 id, i32 a2, i32 cnt_arg, i32 a4, i32 a5, i32 a6, i32 x, i32 y, i32 z) {
        c_inv_add.hit();
        const i16 wid = static_cast<i16>(id);
        i16 cnt = static_cast<i16>(cnt_arg);
        if (static_cast<i32>(wid) <= 0) return 0;
        u8* const type = item_type(static_cast<i32>(wid));
        const u16 w24 = jo::as_item_def(type)->stats[0];
        const u16 w26 = jo::as_item_def(type)->stats[1];
        const i32 b24 = sb(static_cast<u8>(w24));
        const i32 b26 = sb(static_cast<u8>(w26));
        const i32 bx = sb(static_cast<u8>(x)), by = sb(static_cast<u8>(y)), bz = sb(static_cast<u8>(z));
        if (static_cast<i32>(cnt) < 0) {
            const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_list(this)->add_item(static_cast<std::uint8_t>(joan::orig::arg::u32(b24)), static_cast<std::uint8_t>(joan::orig::arg::u32(b26)), static_cast<std::uint8_t>(joan::orig::arg::u32(bx)), static_cast<std::uint8_t>(joan::orig::arg::u32(by)), static_cast<std::uint8_t>(joan::orig::arg::u32(bz))));  // 0x004576f0
            u8* const e = inv_slot(this, idx);
            jo::as_inv_item(e)->type = wid;
            jo::as_inv_item(e)->count = cnt;
            jo::view<i32>(jo::as_inv_item(e)->u32_6) = a4;
            jo::view<i32>(jo::as_inv_item(e)->u32_a) = a5;
            jo::view<i32>(jo::as_inv_item(e)->u32_e) = a6;
            jo::as_inv_item(e)->level = static_cast<i16>(a2);
            return 1;
        }
        // Room count (dead: the flag [ebp-0x14] is always 1), calls kept.
        i32 room = 0;
        for (i32 i = inv_size(this); i > 0; --i) {
            if (inv_free(this, i) != 0) continue;
            u8* const e = inv_slot(this, i);
            if (static_cast<i32>(jo::as_inv_item(e)->type) == static_cast<i32>(wid) && jo::view<i32>(jo::as_inv_item(e)->u32_6) == a4 &&
                jo::view<i32>(jo::as_inv_item(e)->u32_a) == a5) {
                room = static_cast<i32>(static_cast<u32>(room) +
                                        static_cast<u32>(static_cast<i32>(jo::view<i16>(jo::as_item_def(type)->stats[4])) -
                                                         static_cast<i32>(jo::as_inv_item(e)->count)));
            }
        }
        (void)room;
        i32 last_eax = inv_size(this);
        for (i32 i = last_eax; i > 0; --i) {
            const i32 fr = inv_free(this, i);
            if (fr != 0) {
                last_eax = fr;
                continue;
            }
            u8* const e = inv_slot(this, i);
            if (static_cast<i32>(jo::as_inv_item(e)->type) == static_cast<i32>(wid) && jo::view<i32>(jo::as_inv_item(e)->u32_6) == a4 &&
                jo::view<i32>(jo::as_inv_item(e)->u32_a) == a5) {
                joan::orig::as_item_stack(e)->take(joan::orig::arg::mptr(static_cast<void*>(&cnt)));  // 0x00456d60
            }
            last_eax = static_cast<i32>(cnt);
            if (static_cast<i32>(cnt) == 0) break;
        }
        if (static_cast<i32>(cnt) == 0) return last_eax;
        const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_list(this)->add_item(static_cast<std::uint8_t>(joan::orig::arg::u32(b24)), static_cast<std::uint8_t>(joan::orig::arg::u32(b26)), static_cast<std::uint8_t>(joan::orig::arg::u32(bx)), static_cast<std::uint8_t>(joan::orig::arg::u32(by)), static_cast<std::uint8_t>(joan::orig::arg::u32(bz))));  // 0x004576f0
        u8* const e = inv_slot(this, idx);
        jo::as_inv_item(e)->type = wid;
        jo::as_inv_item(e)->count = cnt;
        jo::view<i32>(jo::as_inv_item(e)->u32_6) = a4;
        jo::view<i32>(jo::as_inv_item(e)->u32_a) = a5;
        jo::view<i32>(jo::as_inv_item(e)->u32_e) = a6;
        jo::as_inv_item(e)->level = static_cast<i16>(a2);
        return 1;
    }

    // ------------------------------------------------------------------ 0x00458260
    i32 inv_find(i32 id) {
        for (i32 i = inv_size(this); i > 0; --i) {
            if (inv_free(this, i) != 0) continue;
            u8* const e = inv_slot(this, i);
            if (static_cast<i32>(jo::as_inv_item(e)->type) == id) return i;
        }
        return 0;
    }

    // ------------------------------------------------------------------ 0x00458390
    i32 inv_count(i32 id) {
        i32 sum = 0;
        for (i32 i = inv_size(this); i > 0; --i) {
            if (inv_free(this, i) != 0) continue;
            u8* const e = inv_slot(this, i);
            if (static_cast<i32>(jo::as_inv_item(e)->type) == id)
                sum = static_cast<i32>(static_cast<u32>(sum) + static_cast<u32>(static_cast<i32>(jo::as_inv_item(e)->count)));
        }
        return sum;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("Inventory::add", 0x00457ba0, &H::inv_add);
B100("Inventory::find", 0x00458260, &H::inv_find);
B100("Inventory::count", 0x00458390, &H::inv_count);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_units4.cpp (game/units4)
namespace {
namespace game_units4 {

struct H {
    // 0x00457f60 PtrList::is_empty_at(i): 1 unless the slot exists with byte +0x17 set.
    std::int32_t list_empty_at(std::int32_t i) {
        std::uint8_t* const slot = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(i)));  // 0x00417200
        if (slot != nullptr && slot[0x17] != 0) return 0;
        return 1;
    }
    // 0x00457e90 PtrList::get(i)
    void* list_get(std::int32_t i) { return joan::orig::arg::as<void*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(i))); }
};

#define U4(name, addr, fn) JOAN_REPLACE("game/units4", name, addr, fn)
U4("PtrList::is_empty_at", 0x00457f60, &H::list_empty_at);
U4("PtrList::get", 0x00457e90, &H::list_get);
#undef U4

}  // namespace game_units4
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

using joan::call::thiscall;


void* const kTypes = obj(d::g_item_res);

JOAN_COUNTER(c_fill_from, "Grid::fill_from");

struct H {
    // 0x00457ec0
    void fill_from(i32 i) {
        c_fill_from.hit();
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(i)));  // 0x00417200
        if (p == nullptr) return;
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(jo::as_inv_item(p)->type)));  // 0x00425ce0
        joan::orig::as_grid(this)->fill_458130(static_cast<std::uint8_t>(joan::orig::arg::u32(*reinterpret_cast<const i8*>(&jo::as_item_def(t)->stats[0]))), static_cast<std::uint8_t>(joan::orig::arg::u32(*reinterpret_cast<const i8*>(&jo::as_item_def(t)->stats[1]))), static_cast<std::uint8_t>(joan::orig::arg::u32(jo::as_inv_item(p)->u8_14)), static_cast<std::uint8_t>(joan::orig::arg::u32(jo::as_inv_item(p)->x)), static_cast<std::uint8_t>(joan::orig::arg::u32(jo::as_inv_item(p)->y)));  // 0x00458130
        jo::as_inv_item(p)->placed = 0;
        joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));  // 0x005858c0
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Grid::fill_from", 0x00457ec0, &H::fill_from);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

using joan::call::thiscall;

JOAN_COUNTER(c_apply_457fc0, "Grp::apply_457fc0");

struct H {
    // ---------------------------------------------------------------- 0x00457fc0
    u32 apply_457fc0(u32 id, u8 b1, u8 b2, u8 b3) {
        c_apply_457fc0.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(this)->is_empty_at(joan::orig::arg::i32(id))) != 0) return 1;  // 0x00457f60
        u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(id)));  // 0x00417200
        const u8 c12 = g[0x12];
        const u8 c13 = g[0x13];
        const u8 c16 = g[0x16], c15 = g[0x15], c14 = g[0x14];
        joan::orig::as_grid(this)->fill_458130(static_cast<std::uint8_t>(joan::orig::arg::u32(c12)), static_cast<std::uint8_t>(joan::orig::arg::u32(c13)), static_cast<std::uint8_t>(joan::orig::arg::u32(c14)), static_cast<std::uint8_t>(joan::orig::arg::u32(c15)), static_cast<std::uint8_t>(joan::orig::arg::u32(c16)));   // 0x00458130
        joan::orig::as_inventory_item(g)->set_grid_pos(static_cast<std::uint8_t>(joan::orig::arg::u32(b1)), static_cast<std::uint8_t>(joan::orig::arg::u32(b2)), static_cast<std::uint8_t>(joan::orig::arg::u32(b3)));                     // 0x00456e70
        joan::orig::as_grid(this)->fill_458090(joan::orig::arg::u32(c12), joan::orig::arg::u32(c13), joan::orig::arg::u32(b1), joan::orig::arg::u32(b2), joan::orig::arg::u32(b3), joan::orig::arg::u32(id));   // 0x00458090
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Grp::apply_457fc0", 0x00457fc0, &H::apply_457fc0);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

using joan::call::thiscall;

JOAN_COUNTER(c_find_at, "Grid::find_at");

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00458170
    std::int32_t find_at(std::int32_t x, std::int32_t y) {
        c_find_at.hit();
        for (std::int32_t i = joan::orig::as_dynarray(self())->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_ptr_list(self())->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;           // 0x00457f60
            const std::uint8_t* const r = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_ptr_list(self())->get(joan::orig::arg::i32(i)));  // 0x00457e90
            const std::int32_t x0 = static_cast<std::int8_t>(r[0x15]);
            const std::int32_t x1 = static_cast<std::int8_t>(r[0x15]) + static_cast<std::int8_t>(r[0x12]) - 1;
            const std::int32_t y0 = static_cast<std::int8_t>(r[0x16]);
            const std::int32_t y1 = static_cast<std::int8_t>(r[0x16]) + static_cast<std::int8_t>(r[0x13]) - 1;
            if (x0 <= x && x1 >= x && y0 <= y && y1 >= y) return i;
        }
        return 0;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Grid::find_at", 0x00458170, &H::find_at);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b91_01.cpp (game/b91_01)
namespace {
namespace game_b91_01 {

using joan::call::thiscall;

JOAN_COUNTER(c_grid_assign, "Grid::assign");

struct H {
    // 0x00458530
    void* grid_assign(void* src) {
        c_grid_assign.hit();
        if (jo::as_grid(src)->rows != jo::self<jo::Grid>(this)->rows ||
            jo::as_grid(src)->cell_size != jo::self<jo::Grid>(this)->cell_size ||
            jo::as_grid(src)->cols != jo::self<jo::Grid>(this)->cols) {
            joan::orig::as_grid(this)->free();  // 0x004573c0
            joan::orig::as_grid(this)->alloc(joan::orig::arg::i32(jo::as_grid(src)->rows), joan::orig::arg::i32(jo::as_grid(src)->cell_size), joan::orig::arg::i32(jo::as_grid(src)->cols));  // 0x00457290
        }
        for (std::int32_t i = 0; i < jo::self<jo::Grid>(this)->rows; ++i) {
            for (std::int32_t j = 0; j < jo::self<jo::Grid>(this)->cols; ++j) {
                void* const s = static_cast<void**>(jo::as_grid(src)->cells[i].rows)[j];
                void* const d = static_cast<void**>(jo::self<jo::Grid>(this)->cells[i].rows)[j];
                std::memcpy(joan::orig::arg::mptr(d), joan::orig::arg::ptr(s), joan::orig::arg::u32(jo::self<jo::Grid>(this)->cell_size));  // memcpy
            }
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
        for (std::int32_t k = 1; k <= joan::orig::as_dynarray(src)->size(); ++k) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_ptr_list(src)->is_empty_at(joan::orig::arg::i32(k))) != 0) {  // 0x00457f60
                joan::orig::dynarray(this)->add_blank(1);              // 0x00584bd0
            } else {
                const char* const v = joan::orig::as_dynarray(src)->get(joan::orig::arg::i32(k));  // the element (x64: was kept in an i32)  // 0x00417200
                joan::orig::dynarray_b(this)->linkin(v);             // 0x005857a0
            }
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_01", name, addr, fn)
B("Grid::assign", 0x00458530, &H::grid_assign);
#undef B

}  // namespace game_b91_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ItemStack::init(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> void { joan::bind::member<&::game_b84_05::H::obj_init>(this, a0, a1, a2, a3, a4, a5); }  // 0x456cd0
auto joan::orig::ItemStack::take(void* a0) -> void { joan::bind::member<&::game_b92_01::H::stack_take>(this, a0); }  // 0x456d60
auto joan::orig::InventoryItem::set_size(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b98_01::H::set_12_13>(this, a0, a1); }  // 0x456e40
auto joan::orig::InventoryItem::set_grid_pos(std::uint8_t a0, std::uint8_t a1, std::uint8_t a2) -> void { joan::bind::member<&::game_b93_01::H::set_bytes_14>(this, a0, a1, a2); }  // 0x456e70
auto joan::orig::WorldItem::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_05::H::obj_ctor>(this)); }  // 0x456eb0
auto joan::orig::WorldItem::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_01::H::dtor_456ef0>(this)); }  // 0x456ef0
auto joan::orig::WorldItem::detach() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_01::H::detach>(this)); }  // 0x456f20
auto joan::orig::WorldItem::attach_zone_456fb0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves81::H::wrap_456fb0>(this)); }  // 0x456fb0
auto joan::orig::WorldItem::set_pos(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::set_pos>(this, a0, a1, a2)); }  // 0x456fe0
auto joan::orig::Effect::attach_zone() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves82b::H::attach_zone>(this)); }  // 0x457070
auto joan::orig::WorldItem::set_state(World* a0) -> void { joan::bind::member<&::game_b84_05::H::set_state>(this, a0); }  // 0x457120
auto joan::orig::WorldItem::set_heading(std::uint32_t a0) -> void { joan::bind::member<&::game_b84_05::H::set_2e>(this, a0); }  // 0x4571b0
auto joan::orig::Grid::Grid_4571e0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_02::H::grid_ctor>(this)); }  // 0x4571e0
auto joan::orig::Inventory::destruct() -> void { joan::bind::member<&::game_b96_01::H::dtor_457220>(this); }  // 0x457220
auto joan::orig::Grid::alloc(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::member<&::game_b90_02::H::grid_alloc>(this, a0, a1, a2); }  // 0x457290
auto joan::orig::Grid::free() -> void { joan::bind::member<&::game_b92_01::H::grid_free>(this); }  // 0x4573c0
auto joan::orig::Grid::area_free(std::int8_t a0, std::int8_t a1, std::int32_t a2, std::int8_t a3, std::int8_t a4, std::int32_t a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::area_free>(this, a0, a1, a2, a3, a4, a5)); }  // 0x4574a0
auto joan::orig::Grid::find_place(std::uint32_t a0, std::uint32_t a1, void* a2, void* a3, void* a4, std::int32_t a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_01::H::grid_find>(this, a0, a1, a2, a3, a4, a5)); }  // 0x4575a0
auto joan::orig::List::add_item(std::uint8_t a0, std::uint8_t a1, std::uint8_t a2, std::uint8_t a3, std::uint8_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_01::H::add_item>(this, a0, a1, a2, a3, a4)); }  // 0x4576f0
auto joan::orig::SoundEffects::construct(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_01::H::init_457ad0>(this, a0)); }  // 0x457ad0
auto joan::orig::Inventory::add_457ba0(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_02::H::inv_add>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x457ba0
auto joan::orig::SoundEffects::find(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_01::H::lookup_find>(this, a0)); }  // 0x457e00
auto joan::orig::PtrList::get(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_units4::H::list_get>(this, a0)); }  // 0x457e90
auto joan::orig::Grid::fill_from(std::int32_t a0) -> void { joan::bind::member<&::game_b94_01::H::fill_from>(this, a0); }  // 0x457ec0
auto joan::orig::PtrList::is_empty_at(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_units4::H::list_empty_at>(this, a0)); }  // 0x457f60
auto joan::orig::Grid::fill_458090(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> void { joan::bind::member<&::game_b92_01::H::grid_fill>(this, a0, a1, a2, a3, a4, a5); }  // 0x458090
auto joan::orig::Grid::fill_458130(std::uint8_t a0, std::uint8_t a1, std::uint8_t a2, std::uint8_t a3, std::uint8_t a4) -> void { joan::bind::member<&::game_b93_01::H::grid_fill>(this, a0, a1, a2, a3, a4); }  // 0x458130
auto joan::orig::Grid::find_at(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_03::H::find_at>(this, a0, a1)); }  // 0x458170
auto joan::orig::Inventory::find(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_02::H::inv_find>(this, a0)); }  // 0x458260
auto joan::orig::List::count_of(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_01::H::count_of>(this, a0)); }  // 0x4582e0
auto joan::orig::Inventory::count(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_02::H::inv_count>(this, a0)); }  // 0x458390
auto joan::orig::ItemList::remove(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_01::H::item_remove>(this, a0, a1)); }  // 0x458430
auto joan::orig::Grid::assign(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_01::H::grid_assign>(this, a0)); }  // 0x458530
