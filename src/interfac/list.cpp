// interfac/list.cpp: functions of the original d:\ww\interfac\list.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::img::obj;
using joan::img::str;

using u16 = std::uint16_t;

using joan::orig::helpers::addr;  // a pointer left in eax

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
//
//    - List::free 0x006f8470 (ret 0): +0x37 = +0xa0 = 0, heap free of +0x11 ("list.cpp"), +5 = 0.
namespace {
namespace game_b99_10 {

void* const kHeap = obj(d::g_mem);

JOAN_COUNTER(c_list_free, "List::free");

struct H {
    // ------------------------------------------------------------------ 0x006f8470
    void list_free() {
        c_list_free.hit();
        jo::self<jo::ListBox>(this)->bitmap = nullptr;
        jo::self<jo::ListBox>(this)->font = nullptr;
        if (jo::view<void*>(jo::self<jo::ListBox>(this)->items) != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_a27920)) + 7;
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ListBox>(this)->items)), str(d::s_d_ww_interfac_list_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::ListBox>(this)->items = nullptr;
            jo::view<u32>(jo::self<jo::ListBox>(this)->count) = 0;
            jo::view<u32>(jo::self<jo::ListBox>(this)->capacity) = 0;
        }
        jo::self<jo::ListBox>(this)->ready = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("List::free", 0x006f8470, &H::list_free);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
//
//      0x006f8510 List::set_geometry      (thiscall(7), ret 0x1c) - list.cpp.
namespace {
namespace game_b98_08 {

struct H {
    std::uintptr_t set_geometry(jo::Font2d* a, u32 b, u32 c, u32 d, u32 e, u32 g, u32 h) {  // 0x006f8510
        if (jo::self<jo::ListBox>(this)->ready == 0) return addr(this);
        jo::self<jo::ListBox>(this)->font = a;
        jo::self<jo::ListBox>(this)->colour = b;
        jo::self<jo::ListBox>(this)->style[0] = static_cast<u16>(c);
        jo::self<jo::ListBox>(this)->style[1] = static_cast<u16>(d);
        jo::self<jo::ListBox>(this)->style[2] = static_cast<u16>(e);
        jo::self<jo::ListBox>(this)->style[3] = static_cast<u16>(g);
        jo::self<jo::ListBox>(this)->style[4] = static_cast<u16>(h);
        return (addr(this) & ~std::uintptr_t{0xffff}) | (g & 0xffffu);  // eax = this, ax = g
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("List::set_geometry", 0x006f8510, &H::set_geometry);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
//
//    - List::set_item_6f85b0 0x006f85b0 (idx, str, d, w, w, w), ret 0x18 (no references: dead
//      code): eax = this when +5 == 0, else idx * 0x13 as the original leaves it.
namespace {
namespace game_b102_15 {

using joan::orig::helpers::strcpy_;  // strcpy
using joan::orig::helpers::strlen_;  // strlen

JOAN_COUNTER(c_set_item, "List::set_item_6f85b0");

struct H {
    // ------------------------------------------------------------------ 0x006f85b0
    std::uintptr_t set_item_6f85b0(i32 idx, const char* s, u32 d, u32 w1, u32 w2, u32 w3) {
        c_set_item.hit();
        if (jo::self<jo::ListBox>(this)->ready == 0) return addr(this);
        const i32 o = static_cast<i32>(static_cast<u32>(idx) * sizeof(jo::ListBoxColumn));  // eax as the original leaves it
        jo::ListBoxColumn& c = (&jo::self<jo::ListBox>(this)->column0)[idx];  // column0..column3
        if (s != nullptr && strlen_(s) <= 8) strcpy_(c.name, s);
        c.colour = d;
        c.u16_d = static_cast<u16>(w1);
        c.u16_f = static_cast<u16>(w2);
        c.u16_11 = static_cast<u16>(w3);
        return static_cast<u32>(o);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("List::set_item_6f85b0", 0x006f85b0, &H::set_item_6f85b0);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
//
//      0x006f8670 List::append(item)         (thiscall, ret 4) - returns the old count.
namespace {
namespace game_b95_08 {

using joan::call::thiscall;

JOAN_COUNTER(c_list_append, "List::append");

struct H {
    // ------------------------------------------------------------------ 0x006f8670
    i32 list_append(void* item) {
        c_list_append.hit();
        if (jo::self<jo::ListBox>(this)->items == nullptr || jo::self<jo::ListBox>(this)->count == jo::self<jo::ListBox>(this)->capacity) {
            jo::self<jo::ListBox>(this)->capacity = jo::self<jo::ListBox>(this)->capacity + 5;
            const i32 line = static_cast<i32>(global<i16>(d::g_a27924)) + 4;
            jo::view<void*>(jo::self<jo::ListBox>(this)->items) = joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ListBox>(this)->items)), joan::orig::arg::u32(// 0x0058eb30
                                             static_cast<i32>(static_cast<u32>(jo::self<jo::ListBox>(this)->capacity) * static_cast<u32>(sizeof(jo::ListBoxItem)))), str(d::s_d_ww_interfac_list_cpp), joan::orig::arg::i32(line));
        }
        const i32 n = jo::self<jo::ListBox>(this)->count;
        joan::orig::as_string(&jo::self<jo::ListBox>(this)->items[n])->assign(joan::orig::arg::str(item));  // 0x005a1440
        jo::self<jo::ListBox>(this)->items[n].selected = 0;
        jo::self<jo::ListBox>(this)->count = jo::self<jo::ListBox>(this)->count + 1;
        if (jo::self<jo::ListBox>(this)->scroll != nullptr) {
            if (jo::self<jo::ListBox>(this)->count == 1)
                joan::orig::as_scroll(jo::view<void*>(jo::self<jo::ListBox>(this)->scroll))->set_62cd40(1u, 1u, 1u);           // 0x0062cd40
            else
                joan::orig::as_scroll(jo::view<void*>(jo::self<jo::ListBox>(this)->scroll))->set_end(joan::orig::arg::u32(jo::self<jo::ListBox>(this)->count));      // 0x0062ced0
        }
        return n;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("List::append", 0x006f8670, &H::list_append);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
//
//    - List::dtor 0x006f8050 (ret 0, SEH frame: normal path only): 0x6f8470, base 0x61e000.
//
//  - Return values: callers of 0x6e0900, 0x6e4c00, 0x6e4cc0, 0x6ed3e0, 0x6f1ff0, 0x6f8050,
//    0x707b20, 0x707ce0, 0x7180f0, 0x723f90, 0x732940, 0x732af0, 0x736d80, 0x7388a0, 0x742f90
//    ignore eax (checked, none is referenced as data / vtable); the replacements return void or
//    a simple value.
namespace {
namespace game_b100_16 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x006f8050
    u32 list_dtor() {
        joan::orig::as_list(this)->free();         // 0x006f8470
        joan::orig::as_widget_child(this)->destruct();  // 0x0061e000
        return 0;  // void callee; the one caller (combobox.cpp) discards the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("List::dtor", 0x006f8050, &H::list_dtor);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------ 0x006f8010
    void* ctor_6f8010() {
        joan::orig::as_gadget_flags(this)->construct();  // 0x00634b20
        memset_(this, 0, static_cast<std::uint32_t>(offsetof(jo::ListBox, style) + 9));  // 0xb1 on x86: the ListBox (new 0xb6 in ComboBox 0x006b29b0) up to inside style[4]
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj6f8010::Obj", 0x006f8010, &H::ctor_6f8010);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b104_14.cpp (game/b104_14)
namespace {
namespace game_b104_14 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::img::str;

using u16 = std::uint16_t;


// Rgba(r, g, b, a) 0x0042a260 / Rgba(dword) 0x005e9640 into a local, value returned
u32 colour(u32 r, u32 g, u32 b, u32 a) {
    u32 storage = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&storage)->construct(r, g, b, a));  // 0x0042a260
}

JOAN_COUNTER(c_grid_setup, "Grid::setup_6f80c0");

struct H {
    // ------------------------------------------------------------------ 0x006f80c0
    void setup_6f80c0(u32 a8, u32 ac, u32 a10, u32 a14, u32 a18, void* a1c, jo::Font2d* a20, u32 a24, u32 a28,
                      u32 a2c, const char* a30, const char* a34, const char* a38, const char* a3c, void* a40,
                      u32, u32, u32, u32, u32 a54, u32 a58) {
        c_grid_setup.hit();
        if (a1c == nullptr) return;
        jo::self<jo::ListBox>(this)->bitmap = a1c;
        u8* bm = nullptr;
        if (a34 != nullptr)
            bm = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(a1c)->find(joan::orig::arg::str(a34)));  // 0x00620b40
        else if (a38 != nullptr)
            bm = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(a1c)->find(joan::orig::arg::str(a38)));
        else if (a3c != nullptr)
            bm = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(a1c)->find(joan::orig::arg::str(a3c)));
        if (bm == nullptr) return;
        jo::view<u16>(jo::self<jo::ListBox>(this)->x0) = static_cast<u16>(a8);
        jo::view<u16>(jo::self<jo::ListBox>(this)->y0) = static_cast<u16>(ac);
        jo::view<u16>(jo::self<jo::ListBox>(this)->x1) = static_cast<u16>(a10);
        jo::view<u16>(jo::self<jo::ListBox>(this)->y1) = static_cast<u16>(a14);
        jo::view<u16>(jo::self<jo::ListBox>(this)->row_height) = static_cast<u16>(a2c);
        jo::view<u16>(jo::self<jo::ListBox>(this)->text_x) = static_cast<u16>(a24);
        jo::view<u16>(jo::self<jo::ListBox>(this)->text_y) = static_cast<u16>(a28);
        jo::view<u16>(jo::self<jo::ListBox>(this)->text_x1) = static_cast<u16>(static_cast<u32>(static_cast<i32>(static_cast<i16>(a24))) +
                                        static_cast<u32>(static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(bm)->width))) - 1u);
        jo::view<u16>(jo::self<jo::ListBox>(this)->text_y1) = static_cast<u16>(static_cast<u32>(static_cast<i32>(static_cast<i16>(a28))) +
                                        static_cast<u32>(static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(bm)->height))) - 1u);
        const char* const names[4] = {a30, a34, a38, a3c};
        jo::ListBoxColumn* const cols = &jo::self<jo::ListBox>(this)->column0;  // column0..column3
        for (i32 i = 0; i < 4; ++i) {
            if (names[i] != nullptr && joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(names[i]))) <= 8u)  // strlen
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(cols[i].name)), joan::orig::arg::str(names[i]));  // strcpy
        }
        for (i32 i = 0; i < 4; ++i) {
            const u32 c = colour(0xff, 0xff, 0xff, 0xff);
            cols[i].colour = c;
            cols[i].u16_d = 0;
            cols[i].u16_f = 0;
            cols[i].u16_11 = 0;
        }
        jo::self<jo::ListBox>(this)->font = a20;
        jo::self<jo::ListBox>(this)->colour = colour(0xff, 0xff, 0xff, 0xff);
        jo::self<jo::ListBox>(this)->style[0] = 1;
        jo::self<jo::ListBox>(this)->style[1] = 2;
        jo::self<jo::ListBox>(this)->style[2] = 0;
        jo::self<jo::ListBox>(this)->style[3] = 0;
        jo::self<jo::ListBox>(this)->style[4] = 0;
        jo::self<jo::ListBox>(this)->u8_3 = static_cast<u8>(a54);
        jo::self<jo::ListBox>(this)->u8_4 = static_cast<u8>(a58);
        if (a40 != nullptr) joan::orig::as_scroll(a40)->set_62cd40(0u, 0u, 0u);  // 0x0062cd40
        jo::view<u32>(jo::self<jo::ListBox>(this)->fixed_pages) = a18;
        jo::view<u32>(jo::self<jo::ListBox>(this)->was_drawn) = 0;
        jo::view<u32>(jo::self<jo::ListBox>(this)->pressed) = 0;
        jo::self<jo::ListBox>(this)->selected = -1;
        jo::self<jo::ListBox>(this)->draw_callback = nullptr;
        jo::self<jo::ListBox>(this)->ready = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_14", name, addr, fn)
B("Grid::setup_6f80c0", 0x006f80c0, &H::setup_6f80c0);
#undef B

}  // namespace game_b104_14
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

using joan::img::str;

using i32 = std::int32_t;
using u32 = std::uint32_t;


JOAN_COUNTER(c_names_find, "Names::find");

struct H {
    i32 names_find(const char* name) {  // 0x006f87b0
        c_names_find.hit();
        for (i32 i = jo::self<jo::ListBox>(this)->count; i > 0; --i) {
            const char* const entry = jo::self<jo::ListBox>(this)->items[i - 1].text;
            if (std::strcmp(joan::orig::arg::str(name), joan::orig::arg::str(entry)) == 0) return i;  // strcmp
        }
        return 0;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Names::find", 0x006f87b0, &H::names_find);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_list_remove, "List::remove");

struct H {
    // ------------------------------------------------------------------ 0x006f8830
    void list_remove(i32 n) {
        c_list_remove.hit();
        if (n != jo::self<jo::ListBox>(this)->count) {
            jo::ListBoxItem* const base = jo::self<jo::ListBox>(this)->items;
            std::memcpy(joan::orig::arg::mptr(&base[n - 1]), joan::orig::arg::ptr(&base[n]), joan::orig::arg::u32(static_cast<u32>(jo::self<jo::ListBox>(this)->count - n)));  // memmove
        }
        jo::self<jo::ListBox>(this)->count = jo::self<jo::ListBox>(this)->count - 1;
        if (jo::view<void*>(jo::self<jo::ListBox>(this)->scroll) != nullptr) {
            if (jo::self<jo::ListBox>(this)->count == 0) joan::orig::as_scroll(jo::view<void*>(jo::self<jo::ListBox>(this)->scroll))->set_62cd40(0u, 0u, 0u);  // 0x0062cd40
            else joan::orig::as_scroll(jo::view<void*>(jo::self<jo::ListBox>(this)->scroll))->set_end(joan::orig::arg::u32(jo::self<jo::ListBox>(this)->count));             // 0x0062ced0
        }
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("List::remove", 0x006f8830, &H::list_remove);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;


struct H {
    i32 page_count() {  // 0x006f8900
        const i32 span = static_cast<i32>(jo::self<jo::ListBox>(this)->y1) - static_cast<i32>(jo::self<jo::ListBox>(this)->y0) + 1;
        const i32 q = span / static_cast<i32>(jo::self<jo::ListBox>(this)->row_height);  // idiv (traps on 0 like the original)
        if (jo::self<jo::ListBox>(this)->fixed_pages != 0) return q;
        if (q > jo::self<jo::ListBox>(this)->count) return jo::self<jo::ListBox>(this)->count;
        return q;
    }

    void* item(void* out, i32 i) {  // 0x006f9360
        jo::ListBoxItem* const p = &jo::self<jo::ListBox>(this)->items[i - 1];
        joan::orig::as_string(out)->construct_copy(joan::orig::as_string(p));  // 0x005a11f0
        return out;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("List::page_count", 0x006f8900, &H::page_count);
B93("List::item", 0x006f9360, &H::item);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::call::thiscall;
i32 sx16(i32 v) { return static_cast<i16>(v); }

JOAN_COUNTER(c_gauge_draw, "Gauge::draw_6f8970");

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x006f8970
    jo::ListBoxColumn& column(i32 k) { return (&jo::self<jo::ListBox>(this)->column0)[k]; }  // column0..column3
    void draw_633500(i32 x, i32 y, u32 a1, i32 k) {
        const jo::ListBoxColumn& c = column(k);
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a1), joan::orig::arg::str(c.name), joan::orig::arg::mptr(jo::self<jo::ListBox>(this)->bitmap), joan::orig::arg::u32(c.colour), 3, joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(c.u16_d))), joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(c.u16_f))), joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(c.u16_11))), -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
    }
    u32 gauge_draw(u32 a1, u32 flags_arg) {
        c_gauge_draw.hit();
        if (jo::self<jo::ListBox>(this)->ready == 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_widget_child(me())->visible()) == 0) {  // 0x00623360
            jo::view<u32>(jo::self<jo::ListBox>(this)->was_drawn) = 0;
            return 0;
        }
        jo::view<u32>(jo::self<jo::ListBox>(this)->was_drawn) = 0;
        const u8 flags = static_cast<u8>(flags_arg);
        if (jo::self<jo::ListBox>(this)->draw_callback != nullptr) {
            using Cb = i32(JOAN_CDECL*)(void*, u32, u8);
            if (reinterpret_cast<Cb>(jo::self<jo::ListBox>(this)->draw_callback)(me(), a1, flags) != 0) {
                jo::view<u32>(jo::self<jo::ListBox>(this)->was_drawn) = 1;
                return 1;
            }
            return 0;
        }
        i32 base = 1;
        if (jo::self<jo::ListBox>(this)->scroll != nullptr) base = jo::self<jo::ListBox>(this)->scroll->pos;
        if (flags == 7 || (flags & 1) != 0) {
            for (i32 i = 0; i < joan::orig::arg::as<i32>(joan::orig::as_list(me())->page_count()); ++i) {  // 0x006f8900
                const i32 idx = base + i;
                const i32 x = jo::self<jo::ListBox>(this)->x0;
                const i32 y = static_cast<i32>(jo::self<jo::ListBox>(this)->y0) + static_cast<i32>(jo::self<jo::ListBox>(this)->row_height) * i;
                if (jo::view<i8>(jo::self<jo::ListBox>(this)->column0.name[0]) != 0 && jo::self<jo::ListBox>(this)->bitmap != nullptr) draw_633500(sx16(x), sx16(y), a1, 0);
                i32 k = 1;
                if (i == jo::self<jo::ListBox>(this)->selected) {
                    k = 3;
                    if (idx <= jo::self<jo::ListBox>(this)->count && jo::view<u32>(jo::self<jo::ListBox>(this)->pressed) != 0) k = 2;
                }
                if (static_cast<i8>(column(k).name[0]) != 0 && jo::self<jo::ListBox>(this)->bitmap != nullptr) {
                    draw_633500(x + jo::self<jo::ListBox>(this)->text_x, y + jo::self<jo::ListBox>(this)->text_y, a1, k);
                    jo::view<u32>(jo::self<jo::ListBox>(this)->was_drawn) = 1;
                }
            }
        }
        if ((flags == 7 || (flags & 4) != 0) && jo::self<jo::ListBox>(this)->font != nullptr) {
            for (i32 j = 0; j < joan::orig::arg::as<i32>(joan::orig::as_list(me())->page_count()); ++j) {  // 0x006f8900
                if (base + j > jo::self<jo::ListBox>(this)->count) break;
                const i32 x = static_cast<i32>(jo::self<jo::ListBox>(this)->x0) + jo::self<jo::ListBox>(this)->text_x + jo::view<i16>(jo::self<jo::ListBox>(this)->style[3]);
                const i32 y = static_cast<i32>(jo::self<jo::ListBox>(this)->y0) + jo::self<jo::ListBox>(this)->text_y +
                              static_cast<i32>(jo::self<jo::ListBox>(this)->row_height) * j + jo::view<i16>(jo::self<jo::ListBox>(this)->style[4]);
                const char* const item = jo::self<jo::ListBox>(this)->items[base + j - 1].text;
                joan::orig::as_text(jo::self<jo::ListBox>(this)->font)->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(a1), item, joan::orig::arg::u32(jo::self<jo::ListBox>(this)->colour), i32{-1}, i32{0}, static_cast<i32>(jo::view<i16>(jo::self<jo::ListBox>(this)->style[0])), static_cast<i32>(jo::view<i16>(jo::self<jo::ListBox>(this)->style[1])), static_cast<i32>(jo::view<i16>(jo::self<jo::ListBox>(this)->style[2])), joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000}));  // 0x006255d0
                jo::view<u32>(jo::self<jo::ListBox>(this)->was_drawn) = 1;
            }
        }
        return jo::view<u32>(jo::self<jo::ListBox>(this)->was_drawn);
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("Gauge::draw_6f8970", 0x006f8970, &H::gauge_draw);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b104_14_2.cpp (game/b104_14)
namespace {
namespace game_b104_14_2 {

using joan::hook::global;

using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

constexpr img::Addr kMouse = d::g_mouse;

using joan::call::thiscall;

JOAN_COUNTER(c_grid_hit, "Grid::hit_6f8e40");

struct H {
    // ------------------------------------------------------------------ 0x006f8e40
    u32* cell_flags(i32 idx) {  // record idx (1-based)
        return reinterpret_cast<u32*>(&jo::self<jo::ListBox>(this)->items[idx - 1].selected);
    }
    // cell index from a cursor position; false when outside
    bool cell_at(i32 my, i32& q, i32& idx) {
        i32 v = static_cast<i32>(static_cast<u32>(my) - static_cast<u32>(static_cast<i32>(jo::self<jo::ListBox>(this)->y0)));
        const i32 h = jo::self<jo::ListBox>(this)->row_height;
        q = v / h;
        v = static_cast<i32>(static_cast<u32>(v) - static_cast<u32>(q) * static_cast<u32>(h));
        const i32 base = jo::self<jo::ListBox>(this)->scroll != nullptr ? jo::self<jo::ListBox>(this)->scroll->pos : 1;
        idx = static_cast<i32>(static_cast<u32>(q) + static_cast<u32>(base));
        if (idx < 1) return false;
        if (idx > jo::self<jo::ListBox>(this)->count) return false;
        if (static_cast<i32>(jo::self<jo::ListBox>(this)->text_y) > v) return false;
        if (v > static_cast<i32>(jo::self<jo::ListBox>(this)->text_y1)) return false;
        return true;
    }
    i32 left() { return static_cast<i32>(jo::self<jo::ListBox>(this)->x0) + static_cast<i32>(jo::self<jo::ListBox>(this)->text_x); }
    i32 right() { return static_cast<i32>(jo::self<jo::ListBox>(this)->x0) + static_cast<i32>(jo::self<jo::ListBox>(this)->text_x1); }

    i32 hit_6f8e40() {
        c_grid_hit.hit();
        if (jo::self<jo::ListBox>(this)->ready == 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_button(this)->enabled_slot()) == 0) return 0;  // 0x006226e0
        if (jo::view<u32>(jo::self<jo::ListBox>(this)->was_drawn) == 0) return 0;
        for (i32 i = 0; i < jo::self<jo::ListBox>(this)->count; ++i)
            jo::self<jo::ListBox>(this)->items[i].selected = 0;
        jo::self<jo::ListBox>(this)->selected = -1;
        i32 q = 0, idx = 0;
        if (!(left() > global<i32>(d::g_mouse_x)) && !(global<i32>(d::g_mouse_x) > right())) {
            if (cell_at(global<i32>(d::g_mouse_y), q, idx)) {
                jo::self<jo::ListBox>(this)->selected = q;
                *cell_flags(idx) = 2;
                if (jo::view<u32>(jo::self<jo::ListBox>(this)->pressed) != 0) *cell_flags(idx) = *cell_flags(idx) | 1u;
            }
        }
        i32 pressed = 0;
        if (jo::view<u32>(jo::self<jo::ListBox>(this)->pressed) == 0 && joan::orig::arg::as<i32>(joan::orig::as_parser(obj(kMouse))->value(0)) != 0) {  // 0x00594700
            const i32 l = left();
            if (!(l > joan::orig::arg::as<i32>(joan::orig::as_table(obj(kMouse))->at_672(0)))) {  // 0x0043c3c0
                const i32 mx = joan::orig::arg::as<i32>(joan::orig::as_table(obj(kMouse))->at_672(0));
                if (!(mx > right())) {
                    pressed = 1;
                    jo::view<u32>(jo::self<jo::ListBox>(this)->pressed) = 1;
                    const i32 my = joan::orig::arg::as<i32>(joan::orig::as_table(obj(kMouse))->at_676(0));  // 0x0043c3f0
                    if (cell_at(my, q, idx)) *cell_flags(idx) = *cell_flags(idx) | 1u;
                }
            }
        }
        if (jo::view<u32>(jo::self<jo::ListBox>(this)->pressed) != 0 && joan::orig::arg::as<i32>(joan::orig::as_joystick(obj(kMouse))->pressed_kind(0)) != 0) {  // 0x00594a10
            const i32 l = left();
            if (!(l > joan::orig::arg::as<i32>(joan::orig::as_table(obj(kMouse))->get_67a(0)))) {  // 0x006def10
                const i32 mx = joan::orig::arg::as<i32>(joan::orig::as_table(obj(kMouse))->get_67a(0));
                if (!(mx > right())) {
                    const i32 my = joan::orig::arg::as<i32>(joan::orig::as_table(obj(kMouse))->get_67e(0));  // 0x006def40
                    if (cell_at(my, q, idx)) {
                        jo::view<u32>(jo::self<jo::ListBox>(this)->pressed) = 0;
                        return idx;
                    }
                }
            }
        }
        return pressed != 0 ? -1 : 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_14", name, addr, fn)
B("Grid::hit_6f8e40", 0x006f8e40, &H::hit_6f8e40);
#undef B

}  // namespace game_b104_14_2
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

using u32 = std::uint32_t;


struct H {
    // ------------------------------------------------------------------------ 0x006f93b0
    u32 get50_6f93b0() { return jo::view<u32>(jo::self<jo::ListBox>(this)->selected); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::get50_6f93b0", 0x006f93b0, &H::get50_6f93b0);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b102_15_2.cpp (game/b102_15)
namespace {
namespace game_b102_15_2 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x006f93d0
    i32 reset_6f93d0() {
        joan::orig::as_widget_child(this)->disable();  // 0x006ca020
        jo::self<jo::ListBox>(this)->selected = -1;
        i32 i = 0;
        for (; i < jo::self<jo::ListBox>(this)->count; ++i) jo::view<u32>(jo::self<jo::ListBox>(this)->items->selected) = 0;
        return i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("List::reset_6f93d0", 0x006f93d0, &H::reset_6f93d0);
#undef B

}  // namespace game_b102_15_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::List::construct_6f8010() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_08::H::ctor_6f8010>(this)); }  // 0x6f8010
auto joan::orig::List::dtor_6f8050() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_16::H::list_dtor>(this)); }  // 0x6f8050
auto joan::orig::Grid::setup_6f80c0(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, void* a5, Font2d* a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, const char* a10, const char* a11, const char* a12, const char* a13, void* a14, std::uint32_t a15, std::uint32_t a16, std::uint32_t a17, std::uint32_t a18, std::uint32_t a19, std::uint32_t a20) -> void { joan::bind::member<&::game_b104_14::H::setup_6f80c0>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20); }  // 0x6f80c0
auto joan::orig::List::free() -> void { joan::bind::member<&::game_b99_10::H::list_free>(this); }  // 0x6f8470
auto joan::orig::List::set_geometry(Font2d* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::set_geometry>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x6f8510
auto joan::orig::List::append(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_08::H::list_append>(this, a0)); }  // 0x6f8670
auto joan::orig::Names::find_6f87b0(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_08::H::names_find>(this, a0)); }  // 0x6f87b0
auto joan::orig::List::remove_6f8830(std::int32_t a0) -> void { joan::bind::member<&::game_b97_08::H::list_remove>(this, a0); }  // 0x6f8830
auto joan::orig::List::page_count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_06::H::page_count>(this)); }  // 0x6f8900
auto joan::orig::List::draw(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_10::H::gauge_draw>(this, a0, a1)); }  // 0x6f8970
auto joan::orig::Grid::hit_6f8e40() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_14_2::H::hit_6f8e40>(this)); }  // 0x6f8e40
auto joan::orig::List::item(void* a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_06::H::item>(this, a0, a1)); }  // 0x6f9360
auto joan::orig::List::hover_index() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_15::H::get50_6f93b0>(this)); }  // 0x6f93b0
