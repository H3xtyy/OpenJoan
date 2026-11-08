// interfac/i_work.cpp: functions of the original d:\ww\interfac\i_work.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include <cstdarg>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using u8 = std::uint8_t;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
using joan::orig::helpers::addr;  // a pointer left in eax

}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
//
//    - Workshop::release 0x006e9a50 (interfac/i_work.cpp) -> this: like 0x6bcf00 with 0x6ed5b0,
//      singles +0x28 (0x629830 / 0x6bd440), +0x2c, +0x30, arrays +0xc..+0x18 of 4 (line
//      [0xa264e8] + ..), singles +0x1c/+0x20/+0x24; dword +0x4d = 0, word +0x35 = -1.
namespace {
namespace game_b103_15 {

using u16 = std::uint16_t;

JOAN_COUNTER(c_work_release, "Workshop::release");

// ---- release helpers (0x6bcf00 / 0x6e9a50) ----
// Single member: pre-call, scalar delete (flags 1), clear.
template <class P>
void release_one(P*& slot, std::uintptr_t pre, std::uintptr_t del) {
    if (slot == nullptr) return;
    thiscall<u32>(pre, slot);
    void* const q = slot;
    if (q != nullptr) thiscall<void*>(del, q, 1);
    slot = nullptr;
}
// Array member of n buttons: 0x6217e0 + delete 0x6b33a0 on each, then 0x58ed70 free.
void release_arr(jo::ButtonPtrArray*& arr, i32 n, img::Addr line_global, i32 line_add, img::Addr file) {
    if (arr == nullptr) return;
    for (i32 i = 0; i < n; ++i) {
        joan::orig::as_menu_button(arr->item[i])->clear();  // 0x006217e0
        void* const q = arr->item[i];
        if (q != nullptr) joan::orig::as_combo_box(q)->scalar_dtor_6b33a0(1u);  // 0x006b33a0
        arr->item[i] = nullptr;
    }
    const i32 line = static_cast<i32>(global<i16>(line_global)) + line_add;
    joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(arr), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
    arr = nullptr;
}

struct H {
    // ------------------------------------------------------------------------ 0x006e9a50
    void* work_release() {
        c_work_release.hit();
        joan::orig::as_workshop_screen(this)->free_sprites();  // 0x006ed5b0
        release_one(jo::self<jo::WorkScreen>(this)->background, fn::code_value(fn::Image2d_release), fn::code_value(fn::Sprite_scalar_dtor));  // 0x00629830 / 0x006bd440
        release_one(jo::self<jo::WorkScreen>(this)->info_frame, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));  // 0x006217e0 / 0x006b33a0
        release_one(jo::self<jo::WorkScreen>(this)->title_frame, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        release_arr(jo::self<jo::WorkScreen>(this)->row_frames, 4, d::g_a264e8, 0x22, d::s_d_ww_interfac_i_work_cpp);
        release_arr(jo::self<jo::WorkScreen>(this)->row_icons, 4, d::g_a264e8, 0x2e, d::s_d_ww_interfac_i_work_cpp);
        release_arr(jo::self<jo::WorkScreen>(this)->add_buttons, 4, d::g_a264e8, 0x3a, d::s_d_ww_interfac_i_work_cpp);
        release_arr(jo::self<jo::WorkScreen>(this)->remove_buttons, 4, d::g_a264e8, 0x46, d::s_d_ww_interfac_i_work_cpp);
        release_one(jo::self<jo::WorkScreen>(this)->ok_button, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        release_one(jo::self<jo::WorkScreen>(this)->cancel_button, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        release_one(jo::self<jo::WorkScreen>(this)->button_24, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        jo::self<jo::WorkScreen>(this)->loaded = 0;
        jo::view<u16>(jo::self<jo::WorkScreen>(this)->hover_row) = 0xffff;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Workshop::release", 0x006e9a50, &H::work_release);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
//
//    - empty thiscall (ret 0): 0x0068c820, 0x006b8240, 0x006bd490, 0x006ddef0, 0x006e1530,
//      0x006ea000; empty thiscall (arg), ret 4: 0x006dded0 - all unreferenced (dead code).
namespace {
namespace game_b105_19 {

struct H {
    void nop0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("Obj::nop_6ea000", 0x006ea000, &H::nop0);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
//
//    - WorkScreen::refresh 0x006ea020 (ret 0): once (+0x4d), copies the record's per-slot
//      counts (+0xdc) into +0x52 (sum +0x49, +0x62 cleared), +0x41 from [0xa596c4] / +0x2f0a.
//
//  - Return values: callers of 0x6b5320, 0x6c8d00, 0x6ea020, 0x6fe240, 0x7076c0, 0x70db10,
//    0x717f40, 0x724670, 0x7349b0, 0x741a70, 0x75fb00, 0x6e8010, 0x68b900 ignore eax (checked);
//    the replacements still return the original eax where it is a simple value, void otherwise.
namespace {
namespace game_b99_10 {

JOAN_COUNTER(c_work_refresh, "WorkScreen::refresh");

struct H {
    // ------------------------------------------------------------------ 0x006ea020
    std::uintptr_t work_refresh() {  // eax: this or 0x0044cf80's result
        c_work_refresh.hit();
        if (jo::self<jo::WorkScreen>(this)->loaded != 0) return addr(this);
        void* const records = obj(d::g_places);
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_list(records)->empty_at_e(joan::orig::arg::i32(jo::self<jo::WorkScreen>(this)->place)));  // 0x0044cf80
        if (r != 0) return r;
        void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(records)->get_e(joan::orig::arg::i32(jo::self<jo::WorkScreen>(this)->place)));  // 0x0043c450
        u8* const d = virtual_call<u8*>(o, 0x1c);
        const i32 n = jo::as_siege(d)->n_types;
        jo::self<jo::WorkScreen>(this)->total = 0;
        for (i32 i = 0; i < n; ++i) {
            jo::self<jo::WorkScreen>(this)->stock[i] = jo::view<u32>(jo::as_siege(d)->type_made[i]);
            jo::self<jo::WorkScreen>(this)->total = jo::self<jo::WorkScreen>(this)->total + jo::self<jo::WorkScreen>(this)->stock[i];
            jo::self<jo::WorkScreen>(this)->orders[i] = 0;
        }
        if (jo::as_siege(d)->i32_79 == 0) {
            jo::self<jo::WorkScreen>(this)->gold = global<u32>(d::g_get_a596c4);
        } else if (jo::as_siege(d)->i32_79 == 1) {
            jo::self<jo::WorkScreen>(this)->gold = jo::view<u32>(jo::as_siege(d)->i32_2f0a);
        } else if (jo::as_siege(d)->i32_79 == 2) {
            jo::self<jo::WorkScreen>(this)->gold = jo::view<u32>(jo::as_siege(d)->i32_2f0a);
        }
        // dead local read in the original: it emits no load (GCC never accessed the volatile object either)
        jo::self<jo::WorkScreen>(this)->cost = 0;
        jo::self<jo::WorkScreen>(this)->loaded = 1;
        return addr(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("WorkScreen::refresh", 0x006ea020, &H::work_refresh);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
//
//      0x006ea1a0 Work::finish            (thiscall, ret 0) - i_work.cpp.
namespace {
namespace game_b98_08 {

JOAN_COUNTER(c_work_finish, "Work::finish");

struct H {
    // ------------------------------------------------------------------------ 0x006ea1a0
    std::uintptr_t work_finish() {  // eax: this, the facility, the gold or 0x0044cf80's result
        c_work_finish.hit();
        void* const objects = obj(d::g_places);
        const u32 gone = joan::orig::arg::as<u32>(joan::orig::as_list(objects)->empty_at_e(joan::orig::arg::i32(jo::self<jo::WorkScreen>(this)->place)));  // 0x0044cf80
        if (gone != 0) return gone;
        u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_list(objects)->get_e(joan::orig::arg::i32(jo::self<jo::WorkScreen>(this)->place)));  // 0x0043c450
        using Virtual = u8* (Obj::*)();
        JOAN_MEMBER_POINTER_CHECK(Virtual);
        Virtual method{};
        std::memcpy(&method, &joan::call::vtable_of(o)[0x1c / 4], sizeof(void*));
        u8* const q = (reinterpret_cast<Obj*>(o)->*method)();
        for (i32 i = 0; i < static_cast<i32>(jo::as_siege(q)->n_types); ++i) {
            for (i32 j = 0; j < jo::view<i32>(jo::self<jo::WorkScreen>(this)->orders[i]); ++j) {
                joan::orig::as_workshop_facility(q)->queue_build(static_cast<i32>(jo::as_siege(q)->type_kind[i]), i32{0x270f}, str(d::s_TEST), str(d::s_TEST));  // 0x0046ab40 "TEST"
            }
        }
        jo::self<jo::WorkScreen>(this)->gold = jo::self<jo::WorkScreen>(this)->gold - jo::self<jo::WorkScreen>(this)->cost;
        std::uintptr_t r;
        if (jo::view<u32>(jo::as_siege(q)->i32_79) == 0) {
            global<u32>(d::g_get_a596c4) = jo::self<jo::WorkScreen>(this)->gold;
            r = addr(this);
        } else if (jo::view<u32>(jo::as_siege(q)->i32_79) == 1) {
            jo::view<u32>(jo::as_siege(q)->i32_2f0a) = jo::self<jo::WorkScreen>(this)->gold;
            r = addr(q);
        } else if (jo::view<u32>(jo::as_siege(q)->i32_79) == 2) {
            const u32 g = static_cast<u32>(jo::self<jo::WorkScreen>(this)->gold);
            r = g;
            jo::view<u32>(jo::as_siege(q)->i32_2f0a) = g;
        } else {
            r = addr(q);
        }
        jo::self<jo::WorkScreen>(this)->cost = 0;
        return r;
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("Work::finish", 0x006ea1a0, &H::work_finish);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b82_04.cpp (game/b82_04)
//
//    - WorkshopPanel::draw 0x006ea310 (thiscall, ret 0): the siege-engine workshop panel. Owner
//      [+0x39] -> 0x00a5d100 lookup, virtual +0x1c = workshop. Per buildable type (workshop
//      +0xa2 count / +0xa4 word ids): the four buttons (+0x10 / +0x14 / +0x18 / +0xc arrays), the
//      "%s (%d %s)" cost line (resource kind workshop +0x79: 0 gold / 1 iron / 2 wood), ICON_1 /
//      L_* / ICON_2, the att_r / att_y / att_b pips and the kind text; then the summary box
//      (+0x2c): workshop name, two "%d %s" text boxes, the resource label and gold / iron / wood
//      icons, the queued N_* (counts +0x52[i]) and ready R_* (counts +0x62[i]) icons, the empty
//      slots up to workshop +0x7d, and finally 0x006d3cb0 on 0x00a52df8 when byte +0x51 is set.
namespace {
namespace game_b82_04 {

using joan::call::cdecl_call;
// `mov eax, [obj]; mov ecx, obj; call [eax + index * 4]`: thiscall without arguments.
template <class R = void>
R virtual0(void* object, std::int32_t index) {
    using Fn = R (Obj::*)();
    const std::uintptr_t fn = reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(object)[index]);
    return (static_cast<Obj*>(object)->*joan::hook::original<Fn>(fn))();
}

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite queue
constexpr img::Addr kStrings = d::g_strings;  // game strings
constexpr img::Addr kTypes = d::g_types;    // unit type table
constexpr img::Addr kText = d::g_font_goudy14_22_white;     // text renderer
constexpr img::Addr kIcons = d::g_icons;    // ICON_1 / ICON_2 sheet
constexpr img::Addr kSheet = d::g_sheet;    // panel sprite sheet

void strcpy_(char* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

// 0x0042a260 Color(r, g, b, a) on a 4-byte local, returned dword copied out.
std::uint32_t color(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    std::uint32_t c = 0;
    std::uint32_t* const p = joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
    return *p;
}
// fld dword [0x009cfb68] (255.0f); fmul dword [o + 0x11]; call _ftol.
std::uint32_t alpha(const void* o) {
    const joan::hook::x87::Fpu fpu{true};
    return static_cast<std::uint32_t>(joan::hook::x87::ftol(
        fpu.mul(static_cast<double>(global<float>(d::flt_255)), static_cast<double>(jo::as_button(o)->scale))));
}
// 0x00622cc0 Button::draw(mode, flags), ret 8.
void button(void* b) { joan::orig::as_button(b)->draw(3u, 7); }  // 0x00622cc0

// 0x00633500 SpriteQueue::draw_named(x, y, 3, name, sheet, colour, 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1),
// ret 0x44.
void icon(std::int32_t x, std::int32_t y, const char* name, img::Addr sheet, std::uint32_t c) {
    joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 3u, joan::orig::arg::str(name), img::obj(sheet), joan::orig::arg::u32(c), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// 0x006255d0 Text::draw_line(x, y, 3, text, colour, -1, 0, 1, 2, 0, 1.0f, 1.0f), ret 0x30.
void text_line(std::int32_t x, std::int32_t y, const char* s, std::uint32_t c) {
    joan::orig::as_text(obj(kText))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), 3u, s, joan::orig::arg::u32(c), joan::orig::arg::i32(-1), 0, 1, 2, 0, 1.0f, 1.0f);  // 0x006255d0
}
// 0x00626320 Text::draw(x1, y1, x2, y2, 3, text, colour, 2, 1, 1, 1, 2, 0, 1.0f, 1.0f, 0, -1), ret 0x44.
void text_box(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, const char* s, std::uint32_t c) {
    joan::orig::as_text(obj(kText))->draw(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), 3u, s, joan::orig::arg::u32(c), 2, 1, 1, 1, 2, 0, 1.0f, 1.0f, 0, joan::orig::arg::i32(-1));  // 0x00626320
}
// Resource-name getters on the string table (thiscall, no args).
const char* str_gold() { return joan::orig::arg::as<const char*>(joan::orig::as_text(obj(kStrings))->workshop_3()); }   // 0x004ddd50
const char* str_iron() { return joan::orig::arg::as<const char*>(joan::orig::as_text(obj(kStrings))->workshop_4()); }   // 0x004ddd80
const char* str_wood() { return joan::orig::arg::as<const char*>(joan::orig::as_text(obj(kStrings))->workshop_5()); }   // 0x004dddb0
const char* str_kind0() { return joan::orig::arg::as<const char*>(joan::orig::as_text(obj(kStrings))->workshop_6()); }  // 0x004ddde0
const char* str_kind1() { return joan::orig::arg::as<const char*>(joan::orig::as_text(obj(kStrings))->workshop_7()); }  // 0x004dde10
const char* str_res() { return joan::orig::arg::as<const char*>(joan::orig::as_text(obj(kStrings))->workshop_15()); }    // 0x004ddf90

std::int32_t type_id(const char* name) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_unit_types(obj(kTypes))->find(joan::orig::arg::str(name))); }  // 0x005270b0

// The if / else-if chain on CANNON / FAUCONNEAU / BATTERRAM / TREBUCHET: strcpy the matching
// icon name into `out`; leaves `out` unchanged when nothing matches.
void siege_icon(char* out, std::int32_t id, img::Addr cn, img::Addr fc, img::Addr br,
                img::Addr tb) {
    if (id == type_id(str(d::s_CANNON))) {  // 'CANNON'
        strcpy_(out, str(cn));
    } else if (id == type_id(str(d::s_FAUCONNEAU))) {  // 'FAUCONNEAU'
        strcpy_(out, str(fc));
    } else if (id == type_id(str(d::s_BATTERRAM))) {  // 'BATTERRAM'
        strcpy_(out, str(br));
    } else if (id == type_id(str(d::s_TREBUCHET))) {  // 'TREBUCHET'
        strcpy_(out, str(tb));
    }
}

JOAN_COUNTER(c_workshop, "WorkshopPanel::draw");

struct H {
    void* unit_button(jo::ButtonPtrArray* jo::WorkScreen::* array, std::int32_t i) { return (jo::self<jo::WorkScreen>(this)->*array)->item[i]; }
    void* box() { return jo::view<void*>(jo::self<jo::WorkScreen>(this)->info_frame); }
    std::int32_t box_x() { return static_cast<std::int32_t>(jo::as_button(box())->x); }
    std::int32_t box_y() { return static_cast<std::int32_t>(jo::as_button(box())->y); }
    void* workshop() {
        return virtual0<void*>(joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(jo::self<jo::WorkScreen>(this)->place))), 7);  // 0x0043c450, +0x1c
    }

    // sprintf(text, fmt, value, resource name of workshop +0x79); nothing when +0x79 is not 0..2.
    // `with_name`: the "%s (%d %s)" form (def + 0xb4, value) vs "%d %s" (value).
    static void resource_line(char* text, void* ws, std::int32_t gold_value, std::int32_t other_value,
                              const char* name) {
        const std::int32_t kind = jo::as_siege(ws)->i32_79;
        const char* res;
        std::int32_t value;
        if (kind == 0) {
            res = str_gold();
            value = gold_value;
        } else if (kind == 1) {
            res = str_iron();
            value = other_value;
        } else if (kind == 2) {
            res = str_wood();
            value = other_value;
        } else {
            return;
        }
        if (name != nullptr) {
            joan::orig::libc::sprintf(joan::orig::arg::str(text), str(d::s_s_d_s), name, value, res);  // '%s (%d %s)'
        } else {
            joan::orig::libc::sprintf(joan::orig::arg::str(text), str(d::s_d_s), value, res);  // '%d %s'
        }
    }

    // One ICON_1 / name / ICON_2 slot of the summary box, then the 4-per-row advance.
    void slot(std::int32_t& x, std::int32_t& y, std::int32_t& n, const char* name, void* name_alpha) {
        {
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
            icon(x, y, str(d::s_ICON_1), kIcons, c);  // 'ICON_1'
        }
        if (name != nullptr) {
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(name_alpha));
            icon(x, y, name, kSheet, c);
        }
        {
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
            icon(x, y, str(d::s_ICON_2), kIcons, c);  // 'ICON_2'
        }
        x += 0x4b;
        if (n == 3) {
            x = box_x() + 0x1e;
            y = box_y() + 0xed;
        }
        if (n == 7) {
            x = box_x() + 0x1e;
            y = box_y() + 0x13d;
        }
        ++n;
    }

    // 0x006ea310
    void draw() {
        joan::canvas::ScreenScope canvas_scope(0x006ea310);  // canvas: laid out for 1024x768 (canvas_api.hpp)
        c_workshop.hit();
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::WorkScreen>(this)->open)) == 0) return;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(obj(d::g_places))->empty_at_e(joan::orig::arg::i32(jo::self<jo::WorkScreen>(this)->place))) != 0) return;  // 0x0044cf80
        std::uint8_t* const ws = static_cast<std::uint8_t*>(workshop());
        joan::orig::as_label(jo::view<void*>(jo::self<jo::WorkScreen>(this)->background))->draw(3u, 7u);  // 0x00629bd0
        button(jo::view<void*>(jo::self<jo::WorkScreen>(this)->title_frame));
        button(jo::view<void*>(jo::self<jo::WorkScreen>(this)->ok_button));
        button(jo::view<void*>(jo::self<jo::WorkScreen>(this)->cancel_button));
        button(jo::view<void*>(jo::self<jo::WorkScreen>(this)->button_24));

        char text[0x100] = {};     // [ebp-0x108], see header
        char type_icon[0x10] = {};  // [ebp-0x118], see header

        const std::int32_t count = static_cast<std::int32_t>(jo::as_siege(ws)->n_types);
        for (std::int32_t i = 0; i < count; ++i) {
            button(unit_button(&jo::WorkScreen::row_icons, i));
            button(unit_button(&jo::WorkScreen::add_buttons, i));
            button(unit_button(&jo::WorkScreen::remove_buttons, i));
            button(unit_button(&jo::WorkScreen::row_frames, i));
            const std::int32_t id = static_cast<std::int32_t>(jo::as_siege(ws)->type_kind[i]);
            std::uint8_t* const def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(obj(kTypes))->get(joan::orig::arg::i16(id)));  // 0x0042d0a0 (word)
            resource_line(text, ws, jo::as_unit_type(def)->cost, jo::as_unit_type(def)->siege_165,
                          jo::as_unit_type(def)->title_b4);
            void* const b = unit_button(&jo::WorkScreen::row_frames, i);
            {
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(b));
                text_line(static_cast<std::int32_t>(jo::as_button(b)->x) + 0x60,
                          static_cast<std::int32_t>(jo::as_button(b)->y) + 0x10, text, c);
            }
            std::int32_t x = static_cast<std::int32_t>(jo::as_button(b)->x) + 0x10;
            std::int32_t y = static_cast<std::int32_t>(jo::as_button(b)->y) + 0x10;
            siege_icon(type_icon, id, d::s_L_CN, d::s_L_FC, d::s_L_BR, d::s_L_TB);  // L_CN L_FC L_BR L_TB
            {
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(b));
                icon(x, y, str(d::s_ICON_1), kIcons, c);  // 'ICON_1'
            }
            {
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(b));
                icon(x, y, type_icon, kSheet, c);
            }
            {
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(b));
                icon(x, y, str(d::s_ICON_2), kIcons, c);  // 'ICON_2'
            }

            x = static_cast<std::int32_t>(jo::as_button(b)->x) + 0x60;
            y = static_cast<std::int32_t>(jo::as_button(b)->y) + 0x40;
            if (static_cast<std::int32_t>(jo::as_unit_type(def)->item_stack) > 0) {
                for (std::int32_t k = 1; k <= static_cast<std::int32_t>(jo::as_unit_type(def)->dexterity); k += 0x14) {
                    const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(b));
                    icon(x, y, str(d::s_att_r), kSheet, c);  // 'att_r'
                    x += 0x12;
                    if (k == 0x51) break;
                }
            } else {
                for (std::int32_t k = 1; k <= static_cast<std::int32_t>(jo::as_unit_type(def)->dexterity); k += 5) {
                    const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(b));
                    icon(x, y, str(d::s_att_r), kSheet, c);  // 'att_r'
                    x += 0x12;
                    if (k == 0x15) break;
                }
            }
            x = static_cast<std::int32_t>(jo::as_button(b)->x) + 0xc8;
            for (std::int32_t k = 1; k <= static_cast<std::int32_t>(jo::as_unit_type(def)->defense); k += 5) {
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(b));
                icon(x, y, str(d::s_att_y), kSheet, c);  // 'att_y'
                x += 0x12;
                if (k == 0x15) break;
            }
            x = static_cast<std::int32_t>(jo::as_button(b)->x) + 0x130;
            for (std::int32_t k = 1;; ++k) {
                // fild k; fcomp float [def + 0xa0]; test ah, 0x41; je exit: leave when k > value
                // (NaN keeps drawing).
                if (static_cast<double>(k) > static_cast<double>(jo::as_unit_type(def)->speed_run)) break;
                const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(b));
                icon(x, y, str(d::s_att_b), kSheet, c);  // 'att_b'
                x += 0x12;
                if (k == 5) break;
            }
            x = static_cast<std::int32_t>(jo::as_button(b)->x) + 0x198;
            const char* const kind =
                static_cast<std::int32_t>(jo::as_unit_type(def)->item_stack) > 0 ? str_kind1() : str_kind0();
            {
                const std::uint32_t c = color(0, 0, 0, alpha(b));
                text_line(x, y, kind, c);
            }
        }

        button(box());
        {
            // [ebp-0x140]: the owner's workshop +0x6d, read and never used (calls kept).
            jo::Place* volatile unused = jo::as_siege(workshop())->owner;
            (void)unused;
        }
        {
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
            const char* const name = jo::as_recruit_entry(joan::orig::as_recruit_res(obj(d::g_recruit_res))->group_at(joan::orig::arg::u32(static_cast<std::int32_t>(static_cast<std::int16_t>(jo::as_siege(ws)->group_id)))))->location;  // 0x00469520 (word); +0x2b on x86, after the rows pointer
            text_box(box_x() + 0x16, box_y() + 0x2a, box_x() + 0x152, box_y() + 0x5c, name, c);
        }
        resource_line(text, ws, jo::view<std::int32_t>(jo::self<jo::WorkScreen>(this)->cost), jo::view<std::int32_t>(jo::self<jo::WorkScreen>(this)->cost), nullptr);
        {
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
            text_box(box_x() + 0x16, box_y() + 0x1b4, box_x() + 0x152, box_y() + 0x1d0, text, c);
        }
        {
            const std::int32_t kind = jo::as_siege(ws)->i32_79;
            std::uintptr_t res_icon = 0;
            if (kind == 0) {
                const std::uint32_t c = color(0x20, 0x20, 0x20, alpha(box()));
                text_line(box_x() + 0x17, box_y() + 0x1e0, str_gold(), c);
                res_icon = img::addr(d::s_gold);  // 'gold'
            } else if (kind == 1) {
                const std::uint32_t c = color(0x20, 0x20, 0x20, alpha(box()));
                text_line(box_x() + 0x17, box_y() + 0x1e0, str_res(), c);
                res_icon = img::addr(d::s_iron);  // 'iron'
            } else if (kind == 2) {
                const std::uint32_t c = color(0x20, 0x20, 0x20, alpha(box()));
                text_line(box_x() + 0x17, box_y() + 0x1e0, str_res(), c);
                res_icon = img::addr(d::s_wood);  // 'wood'
            }
            if (res_icon != 0) {
                resource_line(text, ws, jo::view<std::int32_t>(jo::self<jo::WorkScreen>(this)->gold), jo::view<std::int32_t>(jo::self<jo::WorkScreen>(this)->gold), nullptr);
                {
                    const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
                    icon(box_x() + 0x100, box_y() + 0x184, str(res_icon), kSheet, c);
                }
                {
                    const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
                    icon(box_x() + 0x100, box_y() + 0x1d4, str(res_icon), kSheet, c);
                }
            }
        }
        {
            const std::uint32_t c = color(0xff, 0xff, 0xff, alpha(box()));
            text_box(box_x() + 0x16, box_y() + 0x204, box_x() + 0x152, box_y() + 0x220, text, c);
        }

        std::int32_t x = box_x() + 0x1e;
        std::int32_t y = box_y() + 0x9d;
        std::int32_t n = 0;
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::as_siege(ws)->n_types); ++i) {
            const std::int32_t id = static_cast<std::int32_t>(jo::as_siege(ws)->type_kind[i]);
            siege_icon(type_icon, id, d::s_N_CN, d::s_N_FC, d::s_N_BR, d::s_N_TB);  // N_CN N_FC N_BR N_TB
            for (std::int32_t j = 0; j < jo::view<std::int32_t>(jo::self<jo::WorkScreen>(this)->stock[i]); ++j) {
                slot(x, y, n, type_icon, unit_button(&jo::WorkScreen::row_frames, i));
            }
        }
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::as_siege(ws)->n_types); ++i) {
            const std::int32_t id = static_cast<std::int32_t>(jo::as_siege(ws)->type_kind[i]);
            siege_icon(type_icon, id, d::s_R_CN, d::s_R_FC, d::s_R_BR, d::s_R_TB);  // R_CN R_FC R_BR R_TB
            for (std::int32_t j = 0; j < jo::view<std::int32_t>(jo::self<jo::WorkScreen>(this)->orders[i]); ++j) {
                slot(x, y, n, type_icon, unit_button(&jo::WorkScreen::row_frames, i));
            }
        }
        for (std::int32_t i = n; i < jo::view<std::int32_t>(jo::as_siege(ws)->u32_7d); ++i) {
            slot(x, y, n, nullptr, nullptr);
        }

        if (static_cast<std::int8_t>(jo::self<jo::WorkScreen>(this)->u8_51) != 0) {
            joan::orig::as_popup(obj(d::g_popup_a52df8))->draw(3, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1));  // 0x006d3cb0
        }
    }
};

JOAN_REPLACE("game/b82_04", "WorkshopPanel::draw", 0x006ea310, &H::draw);
JOAN_CANVAS_SELF(0x006ea310);  // ScreenScope in the body

}  // namespace game_b82_04
}  // namespace

// ---------------------------------------- from modules/game_b109_14.cpp (game/b109_14)
//
//    - Workshop::update_6ec480 0x006ec480 (thiscall, ret 0, interfac/i_work.cpp; caller reads al):
//      item highlight / slide, help text box ("DES\\workshop_*_help.txt"), buy / sell buttons.
//      SEH frame: normal path only.
namespace {
namespace game_b109_14 {

using i8 = std::int8_t;

i32 sx16(i16 v) { return static_cast<i32>(v); }

struct alignas(4) Path {  // 0x005a1170 string/path object (no destructor in the originals)
    u8 bytes[0x194];
};
struct alignas(4) File {  // 0x004179c0 / 0x005872b0
    u8 bytes[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger File
};

JOAN_COUNTER(c_workshop, "Workshop::update_6ec480");

struct H {
    // -------------------------------------------------------------- 0x006ec480
    void slide(jo::ButtonPtrArray* jo::WorkScreen::* list, i32 i, i32 x, i32 y, u32 on) {
        void* const o = (jo::self<jo::WorkScreen>(this)->*list)->item[i];
        joan::orig::as_widget(o)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 300.0f, -200.0f, 0, 0u);  // 0x0061e020
        void* const o2 = (jo::self<jo::WorkScreen>(this)->*list)->item[i];
        joan::orig::as_widget(o2)->scale_to(joan::orig::arg::i16(on), 10.0f, 0.0f, 0, 0u);  // 0x0061f1b0
    }
    i32 pressed(void* button, i32 key) {
        return joan::orig::arg::as<i32>(joan::orig::as_ui_button(button)->mouse(joan::orig::arg::i32(key), i32{0}, i32{-1}, i32{0}, i32{0}, i32{0}));  // 0x006218c0
    }
    u8 workshop_update() {
        c_workshop.hit();
        if (jo::self<jo::WorkScreen>(this)->open == 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_places))->empty_at_e(joan::orig::arg::i32(jo::self<jo::WorkScreen>(this)->place))) != 0) return 0;  // 0x0044cf80
        void* const holder = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(jo::self<jo::WorkScreen>(this)->place)));  // 0x0043c450
        u8* const shop = virtual_call<u8*>(holder, 0x1c);
        jo::self<jo::WorkScreen>(this)->hover_row = -1;
        const i32 count = sx16(jo::as_siege(shop)->n_types);
        for (i32 i = 0; i < count; ++i) {
            bool hit = false;
            if (sx16(jo::self<jo::WorkScreen>(this)->state) == 1) {
                u8* const b = reinterpret_cast<u8*>(jo::self<jo::WorkScreen>(this)->row_frames->item[i]);
                hit = joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(sx16(jo::as_button(b)->x)), joan::orig::arg::i32(sx16(jo::as_button(b)->y)), joan::orig::arg::i32(// 0x00593980
                                    sx16(jo::as_button(b)->x2) + 0x4c), joan::orig::arg::i32(sx16(jo::as_button(b)->y2)))) != 0;
            }
            if (hit) {
                jo::self<jo::WorkScreen>(this)->hover_row = static_cast<i16>(i);
                jo::view<u8>(jo::self<jo::WorkScreen>(this)->row_frames->item[i]->held) = 1;
                slide(&jo::WorkScreen::row_icons, i, 0x224, i * 0x64 + 0x68, 1);
                slide(&jo::WorkScreen::add_buttons, i, 0x238, i * 0x64 + 0x7c, 1);
                slide(&jo::WorkScreen::remove_buttons, i, 0x238, i * 0x64 + 0x9c, 1);
            } else {
                jo::view<u8>(jo::self<jo::WorkScreen>(this)->row_frames->item[i]->held) = 0;
                slide(&jo::WorkScreen::row_icons, i, 0x1d8, i * 0x64 + 0x68, 0);
                slide(&jo::WorkScreen::add_buttons, i, 0x1ec, i * 0x64 + 0x7c, 0);
                slide(&jo::WorkScreen::remove_buttons, i, 0x1ec, i * 0x64 + 0x9c, 0);
            }
        }
        if (sx16(jo::self<jo::WorkScreen>(this)->state) != 1) return 1;
        if (jo::view<i8>(jo::self<jo::WorkScreen>(this)->u8_51) != 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_popup(obj(d::g_popup_a52df8))->poll_buttons()) != 0 &&      // 0x006d5120
                joan::orig::arg::as<i32>(joan::orig::as_popup(obj(d::g_popup_a52df8))->result()) == 4) {      // 0x006d5370
                jo::self<jo::WorkScreen>(this)->u8_51 = 0;
                joan::orig::as_popup(obj(d::g_popup_a52df8))->free_gpopup();  // 0x006d3c60
            }
            return 1;
        }
        if (pressed(jo::self<jo::WorkScreen>(this)->cancel_button, 0x1b) != 0) {
            joan::orig::as_workshop_screen(this)->set_open(joan::orig::arg::u32(i32{0}));  // 0x006ed3e0
            return 1;
        }
        if (pressed(jo::self<jo::WorkScreen>(this)->ok_button, 0xd) != 0) {
            joan::orig::as_workshop_screen(this)->finish();          // 0x006ea1a0
            joan::orig::as_workshop_screen(this)->set_open(joan::orig::arg::u32(i32{0}));  // 0x006ed3e0
            return 1;
        }
        if (pressed(jo::self<jo::WorkScreen>(this)->button_24, 0) != 0) {
            File file;
            joan::orig::as_file(&file)->construct();  // 0x004179c0
            char name[0x100];
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(name)), str(d::s_DES));  // strcpy "DES\\"
            if (jo::view<u32>(jo::as_siege(shop)->i32_79) != 0)
                std::strcat(static_cast<char*>(joan::orig::arg::mptr(name)), str(d::s_workshop_resource_help_txt));  // strcat workshop_resource_help.txt
            else
                std::strcat(static_cast<char*>(joan::orig::arg::mptr(name)), str(d::s_workshop_normal_help_txt));  // strcat workshop_normal_help.txt
            joan::orig::as_file(&file)->file_open(joan::orig::arg::str(static_cast<char*>(name)), i32{1}, static_cast<char>(i32{0}));  // 0x00586da0
            const i32 size = joan::orig::as_file(&file)->file_size() + 1;                         // 0x00587f30
            char* const buf = joan::orig::arg::as<char*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size + 1), str(d::s_d_ww_interfac_i_work_cpp), joan::orig::arg::i32(// 0x0058e7b0
                                              sx16(global<i16>(d::g_a264ec)) + 0x53)));
            std::memset(joan::orig::arg::mptr(buf), i32{0}, joan::orig::arg::u32(size + 1));  // memset
            joan::orig::as_file(&file)->file_read(joan::orig::arg::mptr(buf), joan::orig::arg::u32(size));  // 0x005873a0
            joan::orig::as_popup(obj(d::g_popup_a52df8))->set_text_6d5230(joan::orig::arg::str(buf), joan::orig::as_font2d(img::obj(d::g_font_goudy12_bold_black)));  // 0x006d5230
            const char* const menu = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->workshop_text_19());       // 0x004de050
            joan::orig::as_popup(obj(d::g_popup_a52df8))->set_title(joan::orig::arg::str(menu), joan::orig::as_font2d(img::obj(d::g_font_goudy_bold_14)));  // 0x006d52c0
            joan::orig::as_panel(obj(d::g_popup_a52df8))->set_pos(joan::orig::arg::i16(i32{0x262}), joan::orig::arg::i16(i32{-1}));    // 0x006d50e0
            joan::orig::as_popup(obj(d::g_popup_a52df8))->set_buttons(joan::orig::arg::u32(i32{4}));                 // 0x006d5340
            joan::orig::as_popup(obj(d::g_popup_a52df8))->load_gpopup();                         // 0x006d3be0
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(buf), str(d::s_d_ww_interfac_i_work_cpp), joan::orig::arg::i32(// 0x0058ed70
                          sx16(global<i16>(d::g_a264ec)) + 0x5d));
            jo::self<jo::WorkScreen>(this)->u8_51 = 1;
            joan::orig::as_file(&file)->destruct();  // 0x005872b0
            return 1;
        }
        const i32 kinds = sx16(jo::as_siege(shop)->n_types);
        for (i32 i = 0; i < kinds; ++i) {
            const i32 id = sx16(jo::as_siege(shop)->type_kind[i]);
            u8* const item = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(id)));  // 0x0042d0a0
            if (pressed(reinterpret_cast<u8*>(jo::self<jo::WorkScreen>(this)->add_buttons->item[i]), 0) != 0) {
                if (!(static_cast<i32>(jo::self<jo::WorkScreen>(this)->total) < static_cast<i32>(jo::as_siege(shop)->u32_7d))) return 1;
                if (jo::view<u32>(jo::as_siege(shop)->i32_79) == 0) {
                    if (static_cast<i32>(jo::self<jo::WorkScreen>(this)->cost + jo::view<u32>(jo::as_unit_type(item)->cost)) > static_cast<i32>(jo::self<jo::WorkScreen>(this)->gold))
                        return 1;
                    jo::self<jo::WorkScreen>(this)->cost += jo::view<u32>(jo::as_unit_type(item)->cost);
                } else if (jo::view<u32>(jo::as_siege(shop)->i32_79) == 1) {
                    if (static_cast<i32>(jo::self<jo::WorkScreen>(this)->cost + jo::view<u32>(jo::as_unit_type(item)->siege_165)) > static_cast<i32>(jo::self<jo::WorkScreen>(this)->gold))
                        return 1;
                    jo::self<jo::WorkScreen>(this)->cost += jo::view<u32>(jo::as_unit_type(item)->siege_165);
                } else if (jo::view<u32>(jo::as_siege(shop)->i32_79) == 2) {
                    if (static_cast<i32>(jo::self<jo::WorkScreen>(this)->cost + jo::view<u32>(jo::as_unit_type(item)->siege_165)) > static_cast<i32>(jo::self<jo::WorkScreen>(this)->gold))
                        return 1;
                    jo::self<jo::WorkScreen>(this)->cost += jo::view<u32>(jo::as_unit_type(item)->siege_165);
                }
                jo::self<jo::WorkScreen>(this)->orders[i] += 1;
                jo::self<jo::WorkScreen>(this)->total += 1;
                return 1;
            }
            if (pressed(reinterpret_cast<u8*>(jo::self<jo::WorkScreen>(this)->remove_buttons->item[i]), 0) != 0) {
                if (!(static_cast<i32>(jo::self<jo::WorkScreen>(this)->orders[i]) > 0)) return 1;
                if (jo::view<u32>(jo::as_siege(shop)->i32_79) == 0)
                    jo::self<jo::WorkScreen>(this)->cost -= jo::view<u32>(jo::as_unit_type(item)->cost);
                else if (jo::view<u32>(jo::as_siege(shop)->i32_79) == 1)
                    jo::self<jo::WorkScreen>(this)->cost -= jo::view<u32>(jo::as_unit_type(item)->siege_165);
                else if (jo::view<u32>(jo::as_siege(shop)->i32_79) == 2)
                    jo::self<jo::WorkScreen>(this)->cost -= jo::view<u32>(jo::as_unit_type(item)->siege_165);
                jo::self<jo::WorkScreen>(this)->orders[i] -= 1;
                jo::self<jo::WorkScreen>(this)->total -= 1;
                return 1;
            }
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_14", name, addr, fn)
B("Workshop::update_6ec480", 0x006ec480, &H::workshop_update);
#undef B

}  // namespace game_b109_14
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
//
//    - WorkScreen::set_open 0x006ed3e0 (char open), ret 4.
//
//  - Return values: callers of 0x6e0900, 0x6e4c00, 0x6e4cc0, 0x6ed3e0, 0x6f1ff0, 0x6f8050,
//    0x707b20, 0x707ce0, 0x7180f0, 0x723f90, 0x732940, 0x732af0, 0x736d80, 0x7388a0, 0x742f90
//    ignore eax (checked, none is referenced as data / vtable); the replacements return void or
//    a simple value.
namespace {
namespace game_b100_16 {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

void* const kMouse = obj(d::g_mouse);
void* const kSpeed = obj(d::g_game_a50088);

i32 ftol(double v) { return joan::hook::x87::ftol(v); }

// Resumes the game speed after a pause (shared tail of 0x6e0900 / 0x6ed3e0).
void resume_speed() {
    if (global<u8>(d::g_game_running) != 0) {
        global<u8>(d::g_game_running) = 0;
        global<i16>(d::g_saved_game_speed) = static_cast<i16>(ftol(global<float>(d::g_game_speed)));
        joan::orig::as_game(kSpeed)->set_speed(joan::orig::arg::bits(0.0f));  // 0x0042f930
    }
}

JOAN_COUNTER(c_work_set_open, "WorkScreen::set_open");

struct H {
    // ------------------------------------------------------------------ 0x006ed3e0
    void work_set_open(u32 open) {
        c_work_set_open.hit();
        if (static_cast<i8>(open) <= 0) {
            if (jo::self<jo::WorkScreen>(this)->open == 1) {
                jo::self<jo::WorkScreen>(this)->loaded = 0;
                jo::view<u16>(jo::self<jo::WorkScreen>(this)->hover_row) = 0xffff;
            }
            jo::view<u16>(jo::self<jo::WorkScreen>(this)->state) = 2;
            return;
        }
        joan::orig::as_workshop_screen(this)->refresh();  // 0x006ea020
        jo::view<u8>(jo::self<jo::WorkScreen>(this)->open) = 1;
        joan::orig::as_menu(this)->load_barr();  // 0x006ed4e0
        resume_speed();
        joan::orig::as_cursor(kMouse)->reset_pos();  // 0x00592c30
        jo::view<u16>(jo::self<jo::WorkScreen>(this)->state) = 0;
        jo::self<jo::WorkScreen>(this)->u8_51 = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("WorkScreen::set_open", 0x006ed3e0, &H::work_set_open);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

using joan::img::obj;
using joan::orig::helpers::at;

void* const kImages = obj(d::g_texture_array);

void image_release(void* h) { joan::orig::as_texture_manager(kImages)->release(joan::orig::arg::mptr(h)); }                    // 0x0060a550

struct H {
    // WorkScreen::tex_a / tex_b / tex_c (+0 / +4 / +8): texture handles.
    static void release_one(void*& tex) {
        if (tex != nullptr) {
            image_release(tex);
            tex = nullptr;
        }
    }

    // 0x006ed5b0
    void release3() {
        release_one(jo::self<jo::WorkScreen>(this)->tex_a);
        release_one(jo::self<jo::WorkScreen>(this)->tex_b);
        release_one(jo::self<jo::WorkScreen>(this)->tex_c);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Images3::release", 0x006ed5b0, &H::release3);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b106_12.cpp (game/b106_12)
//
//    - WorkInterface::WorkInterface 0x006e8f40 (thiscall, ret 0; called on 0xa48530 by
//      0x0046f3a0, eax ignored): +0/+4/+8 = 0, new(0xb7) 0x00629400 -> +0x28 (0x00629660 with
//      colour 0x0042a260(0,0,0,0xff), 0x0061f1b0), five new(0x346) 0x00620ee0 widgets
//      (+0x2c / +0x30 via 0x00621540, arrays of 4 at +0xc (0x00621540), +0x10 "ia_ov"
//      (0x006211d0), +0x14 "aw_r_*", +0x18 "aw_l_*", allocated through 0x0058e7b0 on 0xa48838
//      with i_work.cpp lines [0xa264e4] + 0x16 / 0x22 / 0x2b / 0x34), buttons +0x1c / +0x20 /
//      +0x24 (0x00621540 + text 0x004ddcf0 / 0x004ddd20 / 0x004de020 via 0x00623ce0),
//      dword +0x4d = 0, memset(+0x52, 0, 16), memset(+0x62, 0, 16); returns memset's eax.
//
// Notes:
//  - Nested argument pushes (0x0042a260 / 0x004ddcf0 inside 0x00629660 / 0x00623ce0) are
//    interleaved in the original; the call order is kept (0x0042a260 runs before 0x004ddcf0).
//  - 0x006e8f40: SEH frame only guards new + ctor (normal path only).
namespace {
namespace game_b106_12 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using joan::hook::x87::Precision;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_work_ctor, "WorkInterface::WorkInterface");

// 0x0042a260 colour on a stack dword: bytes (a, b, c, d), returns its address; [eax] read.
u32 make_colour(u32 a, u32 b, u32 c, u32 d) {
    u32 colour = 0;
    u32* const r = joan::orig::arg::as<u32*>(joan::orig::as_colour(&colour)->construct(a, b, c, d));  // 0x0042a260
    return *r;
}
// new(0x346) + 0x00620ee0 (null when new fails).
void* new_widget() {
    void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Button)));  // 0x346 on x86; operator new
    return mem ? joan::orig::arg::as<void*>(joan::orig::as_widget(mem)->ctor_620ee0()) : nullptr;      // 0x00620ee0
}
void widget_show(void* w) {  // 0x0061f1b0(0, 1.0f, 0, 1, 0)
    joan::orig::as_widget(w)->scale_to(0, joan::orig::arg::f32(u32{0x3f800000}), 0.0f, 1, 0u);
}
// 0x00621540(x, y, w, h, callback, 1, 0, 0, 1, 1, last)
void widget_place(void* w, i32 x, i32 y, i32 cx, i32 cy, std::uintptr_t callback, u32 last) {
    joan::orig::as_button(w)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(cx), joan::orig::arg::i32(cy), callback, joan::orig::arg::i32(u32{1}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{1}), joan::orig::arg::i32(u32{1}), joan::orig::arg::i32(last));
}
void** alloc_array4(i32 line_offset) {  // 0x0058e7b0 on 0xa48838 (16 bytes, i_work.cpp, line)
    const i32 line = static_cast<i32>(global<i16>(d::g_a264e4)) + line_offset;
    return joan::orig::arg::as<void**>(joan::orig::as_mem(obj(d::g_mem))->add(static_cast<u32>(4 * sizeof(void*)) /* 4 widget pointers: 0x10 on x86 */, str(d::s_d_ww_interfac_i_work_cpp), joan::orig::arg::i32(line)));
}
// 0x006211d0(x, y, font, a, b, c, 0, 1, 0, 0, 1, 1, 0)
void widget_sprites(void* w, i32 x, i32 y, img::Addr font, const char* a, const char* b, const char* c) {
    joan::orig::as_menu_button(w)->setup(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::mptr(img::addr(font)), joan::orig::arg::str(a), joan::orig::arg::str(b), joan::orig::arg::str(c), joan::orig::arg::str(u32{0}), static_cast<std::uint8_t>(joan::orig::arg::u32(u32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(u32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(u32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(u32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(u32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(u32{0})));
}
// Button text: 0x00623ce0(text, 0xa51608, colour(0xff x4), 1, 2, 0, 2, 2); the colour is
// built before the text getter (thiscall on 0xa57c08) runs.
void widget_text(void* w, std::uintptr_t getter) {
    const u32 colour = make_colour(0xff, 0xff, 0xff, 0xff);
    void* const text = thiscall<void*>(getter, obj(d::g_strings));
    joan::orig::as_button(w)->set_text(joan::orig::arg::str(text), img::obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(colour), joan::orig::arg::i32(u32{1}), joan::orig::arg::i32(u32{2}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{2}), joan::orig::arg::i32(u32{2}));  // 0x00623ce0
}

struct H {
    // ------------------------------------------------------------------ 0x006e8f40
    void* work_ctor() {
        c_work_ctor.hit();
        jo::self<jo::WorkScreen>(this)->tex_a = nullptr;
        jo::self<jo::WorkScreen>(this)->tex_b = nullptr;
        jo::self<jo::WorkScreen>(this)->tex_c = nullptr;
        {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Image2d)));  // 0xb7 on x86; operator new
            jo::view<void*>(jo::self<jo::WorkScreen>(this)->background) = mem ? joan::orig::as_image2d(mem)->construct() : nullptr;  // 0x00629400
        }
        {
            const u32 colour = make_colour(0, 0, 0, 0xff);
            // 0x00629660(0, 0, 0xa5cf88, 0x9ddbcc, colour, 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1, 0, 1, 1)
            joan::orig::as_label(jo::view<void*>(jo::self<jo::WorkScreen>(this)->background))->create(joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{0}), img::obj(d::g_sheet), str(d::s_bg), joan::orig::arg::i32(colour), joan::orig::arg::i32(u32{3}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{2}), joan::orig::arg::i32(u32{0}), i32{-1}, i32{-1}, i32{-1}, i32{-1}, joan::orig::arg::i32(u32{0}), i32{-1}, i32{-1}, joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{1}), joan::orig::arg::i32(u32{1}));
            widget_show(jo::view<void*>(jo::self<jo::WorkScreen>(this)->background));
        }
        jo::view<void*>(jo::self<jo::WorkScreen>(this)->info_frame) = new_widget();
        widget_place(jo::view<void*>(jo::self<jo::WorkScreen>(this)->info_frame), 0x400, 0x5b, 0x568, 0x29f, fn::code_value(fn::draw_info_frame_6ee790), 0);  // ->0x006ee790
        widget_show(jo::view<void*>(jo::self<jo::WorkScreen>(this)->info_frame));
        jo::view<void*>(jo::self<jo::WorkScreen>(this)->title_frame) = new_widget();
        widget_place(jo::view<void*>(jo::self<jo::WorkScreen>(this)->title_frame), 0x138, 7, 0x2c4, 0x23, fn::code_value(fn::TitleButton_draw_cb), 0);  // ->0x006eee90
        widget_show(jo::view<void*>(jo::self<jo::WorkScreen>(this)->title_frame));

        jo::view<void**>(jo::self<jo::WorkScreen>(this)->row_frames) = alloc_array4(0x16);
        for (i32 i = 0; i < 4; ++i) {
            jo::view<void**>(jo::self<jo::WorkScreen>(this)->row_frames)[i] = new_widget();
            widget_place(jo::view<void**>(jo::self<jo::WorkScreen>(this)->row_frames)[i], 0x24, -100, 0x224, 0, fn::code_value(fn::overview_draw_6ee210), 0);  // ->0x006ee210
            widget_show(jo::view<void**>(jo::self<jo::WorkScreen>(this)->row_frames)[i]);
        }
        jo::view<void**>(jo::self<jo::WorkScreen>(this)->row_icons) = alloc_array4(0x22);
        for (i32 i = 0; i < 4; ++i) {
            jo::view<void**>(jo::self<jo::WorkScreen>(this)->row_icons)[i] = new_widget();
            const char* const ia = str(d::s_ia_ov);  // "ia_ov"
            widget_sprites(jo::view<void**>(jo::self<jo::WorkScreen>(this)->row_icons)[i], 0x1d8, i * 0x64 + 0x68, d::g_sheet, ia, ia, ia);
            widget_show(jo::view<void**>(jo::self<jo::WorkScreen>(this)->row_icons)[i]);
        }
        jo::view<void**>(jo::self<jo::WorkScreen>(this)->add_buttons) = alloc_array4(0x2b);
        for (i32 i = 0; i < 4; ++i) {
            jo::view<void**>(jo::self<jo::WorkScreen>(this)->add_buttons)[i] = new_widget();
            widget_sprites(jo::view<void**>(jo::self<jo::WorkScreen>(this)->add_buttons)[i], 0x1ec, i * 0x64 + 0x7c, d::g_bmp_commele2, str(d::s_aw_r_ot),
                           str(d::s_aw_r_dw), str(d::s_aw_r_ov));  // aw_r_ot / aw_r_dw / aw_r_ov
            widget_show(jo::view<void**>(jo::self<jo::WorkScreen>(this)->add_buttons)[i]);
        }
        jo::view<void**>(jo::self<jo::WorkScreen>(this)->remove_buttons) = alloc_array4(0x34);
        for (i32 i = 0; i < 4; ++i) {
            jo::view<void**>(jo::self<jo::WorkScreen>(this)->remove_buttons)[i] = new_widget();
            widget_sprites(jo::view<void**>(jo::self<jo::WorkScreen>(this)->remove_buttons)[i], 0x1ec, i * 0x64 + 0x9c, d::g_bmp_commele2, str(d::s_aw_l_ot),
                           str(d::s_aw_l_dw), str(d::s_aw_l_ov));  // aw_l_ot / aw_l_dw / aw_l_ov
            widget_show(jo::view<void**>(jo::self<jo::WorkScreen>(this)->remove_buttons)[i]);
        }

        jo::view<void*>(jo::self<jo::WorkScreen>(this)->ok_button) = new_widget();
        widget_place(jo::view<void*>(jo::self<jo::WorkScreen>(this)->ok_button), 0x28b, 0x2be, 0x327, 0x2ea, fn::code_value(fn::draw_check_button_6ed650), 1);  // ->0x006ed650
        widget_show(jo::view<void*>(jo::self<jo::WorkScreen>(this)->ok_button));
        widget_text(jo::view<void*>(jo::self<jo::WorkScreen>(this)->ok_button), fn::code_value(fn::Workshop_open_1));  // 0x004ddcf0
        jo::view<void*>(jo::self<jo::WorkScreen>(this)->cancel_button) = new_widget();
        widget_place(jo::view<void*>(jo::self<jo::WorkScreen>(this)->cancel_button), 0x32e, 0x2be, 0x3ca, 0x2ea, fn::code_value(fn::draw_check_button_6ed650), 1);  // ->0x006ed650
        widget_show(jo::view<void*>(jo::self<jo::WorkScreen>(this)->cancel_button));
        widget_text(jo::view<void*>(jo::self<jo::WorkScreen>(this)->cancel_button), fn::code_value(fn::Workshop_open_2));  // 0x004ddd20
        jo::view<void*>(jo::self<jo::WorkScreen>(this)->button_24) = new_widget();
        widget_place(jo::view<void*>(jo::self<jo::WorkScreen>(this)->button_24), 0x26, 0x2be, 0xc2, 0x2ea, fn::code_value(fn::Button_draw_6edce0), 1);  // ->0x006edce0
        widget_show(jo::view<void*>(jo::self<jo::WorkScreen>(this)->button_24));
        widget_text(jo::view<void*>(jo::self<jo::WorkScreen>(this)->button_24), fn::code_value(fn::Menu_workshop_12));  // 0x004de020

        jo::self<jo::WorkScreen>(this)->loaded = 0;
        std::memset(jo::self<jo::WorkScreen>(this)->stock, i32{0}, u32{0x10});         // memset
        return std::memset(jo::self<jo::WorkScreen>(this)->orders, i32{0}, u32{0x10});  // memset
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_12", name, addr, fn)
B("WorkInterface::WorkInterface", 0x006e8f40, &H::work_ctor);
#undef B

}  // namespace game_b106_12
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

using joan::hook::global;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::orig::helpers::wadd;
// x87 fcomp + test ah,0x40 jnz: equal or unordered.
bool eq_or_nan(float a, float b) { return !(a < b) && !(a > b); }

JOAN_COUNTER(c_6ece80, "Cutscene::update_6ece80");

struct H {
    // 0x006ece80
    void cutscene_update() {
        c_6ece80.hit();
        if (jo::self<jo::WorkScreen>(this)->open == 0) return;
        auto arr = [this](jo::ButtonPtrArray* jo::WorkScreen::* list, i32 i) { return (jo::self<jo::WorkScreen>(this)->*list)->item[i]; };
        for (i32 i = 0; i < 4; ++i) {
            joan::orig::as_widget(arr(&jo::WorkScreen::row_frames, i))->update();  // 0x0061f3b0
            joan::orig::as_widget(arr(&jo::WorkScreen::row_icons, i))->update();
            joan::orig::as_widget(arr(&jo::WorkScreen::add_buttons, i))->update();
            joan::orig::as_widget(arr(&jo::WorkScreen::remove_buttons, i))->update();
        }
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::WorkScreen>(this)->ok_button))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::WorkScreen>(this)->cancel_button))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::WorkScreen>(this)->button_24))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::WorkScreen>(this)->background))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::WorkScreen>(this)->info_frame))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::WorkScreen>(this)->title_frame))->update();
        const i32 state = jo::self<jo::WorkScreen>(this)->state;
        const float zero = global<float>(d::k_9cf5c4), one = global<float>(d::flt_1);
        // ok / cancel / +0x24 buttons, background, title and info frames, in the original's order
        auto fade_all = [this](i16 on) {
            joan::orig::as_widget(jo::self<jo::WorkScreen>(this)->ok_button)->scale_to(on, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0u);
            joan::orig::as_widget(jo::self<jo::WorkScreen>(this)->cancel_button)->scale_to(on, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0u);
            joan::orig::as_widget(jo::self<jo::WorkScreen>(this)->button_24)->scale_to(on, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0u);
            joan::orig::as_widget(jo::self<jo::WorkScreen>(this)->background)->scale_to(on, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0u);
            joan::orig::as_widget(jo::self<jo::WorkScreen>(this)->title_frame)->scale_to(on, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0u);
            joan::orig::as_widget(jo::self<jo::WorkScreen>(this)->info_frame)->scale_to(on, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0u);
        };
        if (state == 0) {
            if (eq_or_nan(jo::self<jo::WorkScreen>(this)->background->scale, zero)) {
                i32 delay = 0;
                for (i32 i = 3; i >= 0; --i) {
                    joan::orig::as_widget(arr(&jo::WorkScreen::row_frames, i))->move_to(0x24, joan::orig::arg::i16(wadd(i * 0x64, 0x68)), joan::orig::arg::f32(0x455ac000u), joan::orig::arg::f32(0xc4bb8000u), 0, joan::orig::arg::u32(delay));  // 0x0061e020
                    joan::orig::as_widget(arr(&jo::WorkScreen::row_frames, i))->scale_to(1, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0u);  // 0x0061f1b0
                    delay += 0x1e;
                }
                fade_all(1);
                joan::orig::as_widget(jo::view<void*>(jo::self<jo::WorkScreen>(this)->info_frame))->move_to(0x27b, 0x5b, joan::orig::arg::f32(0x455ac000u), joan::orig::arg::f32(0xc4bb8000u), 0, 0u);
            }
            if (eq_or_nan(jo::as_button(arr(&jo::WorkScreen::row_frames, 0))->scale, one) && jo::as_button(arr(&jo::WorkScreen::row_frames, 0))->y == 0x68) jo::self<jo::WorkScreen>(this)->state = 1;
            return;
        }
        if (state == 1) return;
        if (state == 2) {
            if (eq_or_nan(jo::self<jo::WorkScreen>(this)->background->scale, one)) {
                i32 delay = 0;
                for (i32 i = 0; i < 4; ++i) {
                    joan::orig::as_widget(arr(&jo::WorkScreen::row_frames, i))->move_to(0x24, -100, joan::orig::arg::f32(0x455ac000u), joan::orig::arg::f32(0xc4bb8000u), 0, joan::orig::arg::u32(delay));  // 0x0061e020
                    joan::orig::as_widget(arr(&jo::WorkScreen::row_frames, i))->scale_to(0, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0u);                     // 0x0061f1b0
                    delay += 0x1e;
                }
                fade_all(0);
                joan::orig::as_widget(jo::view<void*>(jo::self<jo::WorkScreen>(this)->info_frame))->move_to(0x400, 0x5b, joan::orig::arg::f32(0x455ac000u), joan::orig::arg::f32(0xc4bb8000u), 0, 0u);
            }
            if (eq_or_nan(jo::as_button(arr(&jo::WorkScreen::row_frames, 3))->scale, zero) && jo::as_button(arr(&jo::WorkScreen::row_frames, 3))->y == -100) jo::self<jo::WorkScreen>(this)->state = 3;
            return;
        }
        if (state != 3) return;
        jo::view<u8>(jo::self<jo::WorkScreen>(this)->open) = 0;
        joan::orig::as_unit(obj(d::g_player_unit_recno))->state_sound();  // 0x0042fef0
        joan::orig::as_workshop_screen(this)->free_sprites();             // 0x006ed5b0
        if (global<u8>(d::g_game_running) == 0) {
            global<u8>(d::g_game_running) = 1;
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(static_cast<float>(static_cast<i32>(global<i16>(d::g_saved_game_speed)))));  // 0x0042f930
        }
        joan::orig::as_cursor(obj(d::g_mouse))->set_pos();  // 0x00592bd0
        jo::self<jo::WorkScreen>(this)->state = 0;
        global<u32>(d::g_a5ba65) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("Cutscene::update_6ece80", 0x006ece80, &H::cutscene_update);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

using u8 = std::uint8_t;

struct H {
    u8 byte_34() { return jo::view<u8>(jo::self<jo::WorkScreen>(this)->open); }  // 0x006ed4c0
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::byte_34", 0x006ed4c0, &H::byte_34);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::img::str;

void* const kSprites = obj(d::g_texture_array);

JOAN_COUNTER(c_load_barr, "Menu::load_barr");

i32 sprite_get_flag() { return joan::orig::arg::as<i32>(joan::orig::as_texture_manager(kSprites)->mode()); }                  // 0x0060a720
i32 sprite_set_flag(i32 v) { joan::orig::as_texture_manager(kSprites)->set_mode_60a740(joan::orig::arg::u32(v)); return 0; }  // 0x0060a740 (void; result unused)
void* sprite_load(img::Addr name) { return joan::orig::arg::as<void*>(joan::orig::as_texture_manager(kSprites)->load(joan::orig::arg::str(str(name)), 1, 0u)); }  // 0x006099c0

struct H {
    // ------------------------------------------------------------------ 0x006ed4e0
    i32 load_barr() {
        c_load_barr.hit();
        const i32 saved = sprite_get_flag();
        sprite_set_flag(1);
        if (jo::self<jo::WorkScreen>(this)->tex_a == 0) jo::self<jo::WorkScreen>(this)->tex_a = sprite_load(d::s_BARR);  // 'BARR'
        if (jo::self<jo::WorkScreen>(this)->tex_b == 0) jo::self<jo::WorkScreen>(this)->tex_b = sprite_load(d::s_BARR_BG);  // 'BARR_BG'
        if (jo::self<jo::WorkScreen>(this)->tex_c == 0) jo::self<jo::WorkScreen>(this)->tex_c = sprite_load(d::s_COMMELE2);  // 'COMMELE2'
        return sprite_set_flag(saved);
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Menu::load_barr", 0x006ed4e0, &H::load_barr);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b109_13.cpp (game/b109_13)
namespace {
namespace game_b109_13 {

using joan::hook::global;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::img::str;

JOAN_COUNTER(c_check, "draw_check_button_6ed650");

// ------------------------------------------------------------------ 0x006ed650 helpers
u32 button_colour(u8* button, u32* slot) {
    const float a = global<float>(d::flt_255) * jo::as_button(button)->scale;
    const i32 alpha = joan::hook::x87::ftol(static_cast<double>(a));
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(i32{0xff}, i32{0xff}, i32{0xff}, alpha));  // 0x0042a260
}
void draw_frame(u8* button, i32 x, i32 y, u32 ctx, const char* name) {
    u32 slot = 0;
    const u32 colour = button_colour(button, &slot);
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(ctx), joan::orig::arg::str(name), obj(d::g_sheet), joan::orig::arg::u32(colour), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}

// ------------------------------------------------------------------ 0x006ed650
static i32 JOAN_CDECL draw_check_button(u8* button, u32 ctx, u32 state_arg) {
    c_check.hit();
    const u32 state = state_arg & 0xff;
    if (state != 7 && (state & 1) != 0) return 0;
    i16 x = jo::as_button(button)->x;
    i16 y = jo::as_button(button)->y;
    i16 w = jo::as_button(button)->x2;
    i16 h = jo::as_button(button)->y2;
    float sx = 1.0f;
    float sy = 1.0f;
    if (jo::as_button(button)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(static_cast<i32>(jo::as_button(button)->x)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(button)->y)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(button)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(button)->y2)), &sx, &sy, &x, &y, &w, &h, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    if (jo::view<i8>(jo::as_button(button)->caption[0]) != 0) {
        joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(joan::orig::arg::str(jo::as_button(button)->caption), -1, 0, 0, 1.0f);  // 0x00624f00
        joan::orig::as_font(obj(d::g_font_goudy14_22_white))->text_height(joan::orig::arg::i16(i32{2}), 1.0f);                                      // 0x006253f0
    }
    if (jo::as_button(button)->enabled != 0) {
        if (jo::as_button(button)->pressed != 0) {
            draw_frame(button, x, y, ctx, str(d::s_B_DW));  // "B_DW"
        } else if (jo::as_button(button)->pressed == 0 &&
                   joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(x), static_cast<i32>(y), static_cast<i32>(w), static_cast<i32>(h))) != 0) {  // 0x00593980
            draw_frame(button, x, y, ctx, str(d::s_B_OV));  // "B_OV"
        } else {
            draw_frame(button, x, y, ctx, str(d::s_B_OT));  // "B_OT"
        }
        if (std::strcmp(joan::orig::arg::str(jo::as_button(button)->caption), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->workshop_text_1()))) == 0) {  // 0x004ddcf0
            draw_frame(button, x + 5, y + 8, ctx, str(d::s_CHECK));  // "CHECK"
        } else if (std::strcmp(joan::orig::arg::str(jo::as_button(button)->caption), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->workshop_text_2()))) == 0) {  // 0x004ddd20
            draw_frame(button, x + 5, y + 8, ctx, str(d::s_CROSS_9ddc7c));  // "CROSS"
        }
    } else {
        draw_frame(button, x, y, ctx, str(d::s_B_OT));  // "B_OT"
    }
    if (jo::view<i8>(jo::as_button(button)->caption[0]) != 0) {
        i32 dy = 0;
        if (jo::as_button(button)->pressed != 0) {
            dy = joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(global<float>(d::flt_1) * sy))));  // 0x00590a20
        }
        i32 dx = 0;
        if (jo::as_button(button)->pressed != 0) {
            dx = joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(global<float>(d::flt_1) * sx))));
        }
        u32 slot = 0;
        const u32 colour = button_colour(button, &slot);
        joan::orig::as_text(obj(d::g_font_goudy14_22_white))->draw_line(joan::orig::arg::i32(static_cast<i32>(x) + dx + 0x28), joan::orig::arg::i32(static_cast<i32>(y) + dy + 0xa), joan::orig::arg::u32(ctx), joan::orig::arg::str(jo::as_button(button)->caption), joan::orig::arg::u32(colour), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, 1.0f, 1.0f);  // 0x006255d0
    }
    return 1;
}
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_13", name, addr, fn)
B("draw_check_button_6ed650", 0x006ed650, &draw_check_button);
#undef B

}  // namespace game_b109_13
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::img::str;

using joan::hook::x87::ftol;

JOAN_COUNTER(c_btn_h, "Button::draw_6edce0");

// 0x0042a260 (thunk 0x004098b8, ret 0x10): builds a 4-byte colour in `c`, returns &c.
u32 make_color(i32 a, i32 b, i32 c, i32 d) {
    u32 color = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&color)->construct(a, b, c, d));
}

// ------------------------------------------------- 0x006edce0 / 0x0075fba0 shared pieces
struct Rect16 {
    i16 x, y, w, h;
    float sx, sy;
};

Rect16 button_layout(void* w) {
    Rect16 r{};
    r.x = jo::as_button(w)->x;
    r.y = jo::as_button(w)->y;
    r.w = jo::as_button(w)->x2;
    r.h = jo::as_button(w)->y2;
    r.sx = 1.0f;
    r.sy = 1.0f;
    if (jo::as_button(w)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(i32{jo::as_button(w)->x}), joan::orig::arg::i16(i32{jo::as_button(w)->y}), joan::orig::arg::i16(i32{jo::as_button(w)->x2}), joan::orig::arg::i16(i32{jo::as_button(w)->y2}), &r.sx, &r.sy, &r.x, &r.y, &r.w, &r.h, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    if (jo::view<i8>(jo::as_button(w)->caption[0]) != 0) {
        // Text width / height queries; the results are dead locals in the original.
        joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(joan::orig::arg::str(jo::as_button(w)->caption), -1, 0, 0, 1.0f);  // 0x00624f00
        joan::orig::as_font(obj(d::g_font_goudy14_22_white))->text_height(joan::orig::arg::i16(i32{2}), 1.0f);                                   // 0x006253f0
    }
    return r;
}

i32 alpha_of(void* w) { return ftol(255.0f * jo::as_button(w)->scale); }  // 0x9cfb68 = 255.0f

// 0x00633500 (thunk 0x00401bbd, ret 0x44) with the fixed tail (3, 0, 2, 0, -1 x4, 0, -1, -1).
void draw_sprite(const Rect16& r, u32 param, const char* name, void* set, u32 color) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(i32{r.x}), joan::orig::arg::i16(i32{r.y}), joan::orig::arg::u32(param), joan::orig::arg::str(name), set, joan::orig::arg::u32(color), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);
}

bool hovered(const Rect16& r) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{r.x}, i32{r.y}, i32{r.w}, i32{r.h})) != 0;  // 0x00593980
}

static u32 JOAN_CDECL button_draw_6edce0(void* w, u32 param, u32 flags) {
    c_btn_h.hit();
    if ((flags & 0xff) != 7 && (flags & 1) != 0) return 0;
    Rect16 r = button_layout(w);
    void* const set = img::obj(d::g_sheet);
    if (jo::as_button(w)->enabled != 0) {
        if (jo::as_button(w)->pressed != 0) {
            const u32 c = make_color(0xff, 0xff, 0xff, alpha_of(w));
            draw_sprite(r, param, str(d::s_H_DW), set, c);  // 'H_DW'
        } else if (hovered(r)) {
            const u32 c = make_color(0xff, 0xff, 0xff, alpha_of(w));
            draw_sprite(r, param, str(d::s_H_OV), set, c);  // 'H_OV'
        } else {
            const u32 c = make_color(0xff, 0xff, 0xff, alpha_of(w));
            draw_sprite(r, param, str(d::s_H_OT), set, c);  // 'H_OT'
        }
    } else {
        const u32 c = make_color(0xff, 0xff, 0xff, alpha_of(w));
        draw_sprite(r, param, str(d::s_H_OT), set, c);  // 'H_OT'
    }
    if (jo::view<i8>(jo::as_button(w)->caption[0]) != 0) {
        i32 dy = 0, dx = 0;
        if (jo::as_button(w)->pressed != 0) {
            const double arg = static_cast<double>(1.0f * r.sy);
            dy = joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(arg)));  // 0x00590a20
        }
        if (jo::as_button(w)->pressed != 0) {
            const double arg = static_cast<double>(1.0f * r.sx);
            dx = joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(arg)));
        }
        const u32 c = make_color(0xff, 0xff, 0xff, alpha_of(w));
        joan::orig::as_text(obj(d::g_font_goudy14_22_white))->draw_line(joan::orig::arg::i32(i32{r.x} + dx + 0x28), joan::orig::arg::i32(i32{r.y} + dy + 0xa), joan::orig::arg::u32(param), joan::orig::arg::str(jo::as_button(w)->caption), joan::orig::arg::u32(c), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, 1.0f, 1.0f);  // 0x006255d0
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Button::draw_6edce0", 0x006edce0, &button_draw_6edce0);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using joan::hook::global;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::img::str;

using u16 = std::uint16_t;

using joan::hook::x87::ftol;

JOAN_COUNTER(c_overview_6ee210, "overview_draw_6ee210");

// ---- 0x006ee210 helpers ----
u32 colour(u32 r, u32 g, u32 b, u32 a) {  // 0x0042a260 on a 4-byte stack object, *result
    u32 tmp = 0;
    void* const c = joan::orig::as_colour(&tmp)->construct(r, g, b, a);
    return jo::view<u32>(*jo::as_colour(c));
}
i32 alpha_of(const void* elem) { return ftol(static_cast<double>(global<float>(d::flt_255) * jo::as_button(elem)->scale)); }
void sprite(i32 x, i32 y, u32 surface, const char* name, u32 col) {  // 0x00633500, ret 0x44 (surface: the layer the draw callback gets, a number as in combobox.cpp / i_barr.cpp)
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(surface), joan::orig::arg::str(name), img::obj(d::g_sheet), joan::orig::arg::u32(col), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);
}
void text_line(std::uintptr_t getter, i32 x, i32 y, u32 surface, const void* elem) {  // 0x006255d0, ret 0x30
    const u32 col = colour(0x20, 0x20, 0x20, static_cast<u32>(alpha_of(elem)));
    const char* const text = thiscall<const char*>(getter, obj(d::g_strings));
    joan::orig::as_text(obj(d::g_font_goudy12_bold_black))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(surface), text, joan::orig::arg::u32(col), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000}));
}

// ------------------------------------------------------------------ 0x006ee210 (UI draw callback)
u32 JOAN_CDECL overview_draw(void* elem, u32 surface, u32 flags) {
    c_overview_6ee210.hit();
    if ((flags & 0xffu) != 7 && (flags & 0xffu & 1u) != 0) return 0;
    i16 x = jo::as_button(elem)->x, y = jo::as_button(elem)->y, w = jo::as_button(elem)->x2, h = jo::as_button(elem)->y2;
    float sx = 1.0f, sy = 1.0f;
    if (jo::as_button(elem)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(jo::view<u16>(jo::as_button(elem)->x)), joan::orig::arg::i16(jo::view<u16>(jo::as_button(elem)->y)), joan::orig::arg::i16(jo::view<u16>(jo::as_button(elem)->x2)), joan::orig::arg::i16(jo::view<u16>(jo::as_button(elem)->y2)), &sx, &sy, &x, &y, &w, &h, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    if (jo::as_button(elem)->enabled == 0) return 1;
    if (static_cast<i8>(jo::as_button(elem)->held) != 0) {
        sprite(x, y, surface, str(d::s_I_OV), colour(0xff, 0xff, 0xff, static_cast<u32>(alpha_of(elem))));  // "I_OV"
    } else if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{x}, i32{y}, i32{w}, i32{h})) != 0) {  // 0x00593980
        sprite(x, y, surface, str(d::s_I_OV), colour(0xff, 0xff, 0xff, static_cast<u32>(alpha_of(elem))));
    } else {
        sprite(x, y, surface, str(d::s_I_OT), colour(0xff, 0xff, 0xff, static_cast<u32>(alpha_of(elem))));  // "I_OT"
    }
    sprite(x + 0x5c, y + 0x28, surface, str(d::s_graybar), colour(0xff, 0xff, 0xff, static_cast<u32>(alpha_of(elem))));  // "graybar"
    text_line(fn::code_value(fn::Workshop_open_8), x + 0x60, y + 0x30, surface, elem);   // 0x004dde40
    text_line(fn::code_value(fn::Workshop_open_09), x + 0xc8, y + 0x30, surface, elem);   // 0x004dde70
    text_line(fn::code_value(fn::Workshop_open_10_4ddea0), x + 0x130, y + 0x30, surface, elem);  // 0x004ddea0
    text_line(fn::code_value(fn::Workshop_open_0b), x + 0x198, y + 0x30, surface, elem);  // 0x004dded0
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("overview_draw_6ee210", 0x006ee210, &overview_draw);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b109_17.cpp (game/b109_17)
namespace {
namespace game_b109_17 {

using joan::hook::global;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::img::str;

using joan::hook::x87::ftol;

JOAN_COUNTER(c_info2, "draw_info_frame_6ee790");

// ------------------------------------------------------------------ shared draw helpers
// Colour object 0x0042a260 (r, g, b, a) through thunk 0x004098b8; returns the packed dword.
u32 rgba(i32 r, i32 g, i32 b, i32 a) {
    u32 slot = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&slot)->construct(r, g, b, a));  // 0x0042a260
}
// fld [0x9cfb68] (255.0f); fmul [button+0x11]; _ftol.
i32 alpha_of(const u8* button) {
    return ftol(static_cast<double>(global<float>(d::flt_255) * jo::as_button(button)->scale));
}
// Sprite 0x00633500 on 0xa46128 (17 args, ret 0x44).
void sprite(i32 x, i32 y, u32 ctx, const char* name, void* tex, u32 color, i32 e7, i32 e8, i32 e9, i32 e10) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(ctx), joan::orig::arg::str(name), tex, joan::orig::arg::u32(color), joan::orig::arg::i16(e7), joan::orig::arg::i16(e8), joan::orig::arg::i16(e9), joan::orig::arg::i16(e10), -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// Text 0x006255d0 (12 args, ret 0x30).
void text(img::Addr font, i32 x, i32 y, u32 ctx, const char* s, u32 color, i32 e6, i32 e7, i32 e8, i32 e9,
          i32 e10) {
    joan::orig::as_text(obj(font))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(ctx), s, joan::orig::arg::u32(color), joan::orig::arg::i32(e6), joan::orig::arg::i32(e7), joan::orig::arg::i32(e8), joan::orig::arg::i32(e9), joan::orig::arg::i32(e10), 1.0f, 1.0f);  // 0x006255d0
}

// Button rectangle (+9/+0xb/+0xd/+0xf words), scaled by 0x00633e60 when +6 is set.
struct Rect {
    i16 x, y, w, h;
    float sx, sy;  // ebp-0x14 / ebp-0x18
};
Rect button_rect(u8* b) {
    Rect r{jo::as_button(b)->x, jo::as_button(b)->y, jo::as_button(b)->x2, jo::as_button(b)->y2, 1.0f, 1.0f};
    if (jo::as_button(b)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(// 0x00633e60
                      static_cast<i32>(jo::as_button(b)->x)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(b)->y)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(b)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(b)->y2)), &r.sx, &r.sy, &r.x, &r.y, &r.w, &r.h, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));
    }
    return r;
}
bool skip_state(u32 state_arg) {
    const u32 state = state_arg & 0xff;
    return state != 7 && (state & 1) != 0;
}

// Frame pieces of 0x006c1180 / 0x006ee790 (sprite tex 0xa5cf88).
void frame_sprite(const u8* b, i32 x, i32 y, u32 ctx, img::Addr name) {
    const u32 c = rgba(0xff, 0xff, 0xff, alpha_of(b));
    sprite(x, y, ctx, str(name), img::obj(d::g_sheet), c, 3, 0, 2, 0);
}
u32 grey(const u8* b) { return rgba(0x20, 0x20, 0x20, alpha_of(b)); }
void frame_text(const Rect& r, i32 dy, u32 ctx, const char* s, u32 color) {
    text(d::g_font_goudy14_22_white, static_cast<i32>(r.x) + 0x17, static_cast<i32>(r.y) + dy, ctx, s, color, -1, 0, 1, 2, 0);
}

// ------------------------------------------------------------------ 0x006ee790
static u32 JOAN_CDECL draw_info_frame_6ee790(u8* b, u32 ctx, u32 state) {
    c_info2.hit();
    if (skip_state(state)) return 0;
    const Rect r = button_rect(b);
    if (jo::as_button(b)->enabled == 0) return 1;
    const i32 x = r.x, y = r.y;
    frame_sprite(b, x, y, ctx, d::s_FRAME);                // FRAME
    frame_sprite(b, x + 0x13, y + 0xf, ctx, d::s_titlebar);   // titlebar
    {
        const u32 c = grey(b);
        frame_text(r, 0x11, ctx, joan::orig::arg::as<const char*>(joan::orig::as_menu(obj(d::g_strings))->workshop_c()), c);  // 0x004ddf00
    }
    frame_sprite(b, x + 0x13, y + 0x73, ctx, d::s_goldbar);  // goldbar
    frame_sprite(b, x + 0x13, y + 0x7f, ctx, d::s_titlebar);  // titlebar
    {
        const u32 c = grey(b);
        frame_text(r, 0x81, ctx, joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->workshop_text_0d()), c);  // 0x004ddf30
    }
    frame_sprite(b, x + 0x13, y + 0x182, ctx, d::s_goldbar);  // goldbar
    frame_sprite(b, x + 0x13, y + 0x18e, ctx, d::s_titlebar);  // titlebar
    {
        const u32 c = grey(b);
        frame_text(r, 0x190, ctx, joan::orig::arg::as<const char*>(joan::orig::as_menu(obj(d::g_strings))->workshop_e()), c);  // 0x004ddf60
    }
    frame_sprite(b, x + 0x13, y + 0x1d2, ctx, d::s_goldbar);  // goldbar
    frame_sprite(b, x + 0x13, y + 0x1de, ctx, d::s_titlebar);  // titlebar
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_17", name, addr, fn)
B("draw_info_frame_6ee790", 0x006ee790, &draw_info_frame_6ee790);
#undef B

}  // namespace game_b109_17
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using joan::hook::global;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::img::str;

using joan::hook::x87::ftol;

void* const kSprites = obj(d::g_sprites_a46128);
void* const kFonts = obj(d::g_font_goudy14_22_white);

JOAN_COUNTER(c_title_cb, "TitleButton::draw_cb");

// ---------------------------------------------------------------------------- draw helpers
// ftol(255 * button alpha) -> 0x0042a260 colour (r, g, b, a) through a 4-byte stack object.
u32 white_alpha(const u8* btn) {
    const i32 alpha = ftol(static_cast<double>(global<float>(d::flt_255) * jo::as_button(btn)->scale));
    u32 colour_obj = 0;
    const u32* const c = joan::orig::arg::as<u32*>(joan::orig::as_colour(&colour_obj)->construct(0xffu, 0xffu, 0xffu, static_cast<u32>(alpha)));  // 0x0042a260
    return *c;
}
// 0x00633500 named sprite (17 args), the trailing 11 constant in all callers here.
void sprite(i32 x, i32 y, u32 a, const char* name, void* tex, u32 colour) {
    joan::orig::as_sprite_queue(kSprites)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a), joan::orig::arg::str(name), tex, joan::orig::arg::u32(colour), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// Button rectangle +9/+0xb/+0xd/+0xf and the optional 640x480 rescale 0x00633e60.
struct Rect {
    i16 x1, y1, x2, y2;
    float sx, sy;  // [ebp-0x14], [ebp-0x18]
};
Rect button_rect(const u8* btn) {
    Rect r{jo::as_button(btn)->x, jo::as_button(btn)->y, jo::as_button(btn)->x2, jo::as_button(btn)->y2, 1.0f, 1.0f};
    if (jo::as_button(btn)->fit != 0)
        joan::orig::as_sprite2d(kSprites)->fit_rect(joan::orig::arg::i16(static_cast<i32>(jo::as_button(btn)->x)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(btn)->y)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(btn)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(btn)->y2)), &r.sx, &r.sy, &r.x1, &r.y1, &r.x2, &r.y2, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    return r;
}

i32 JOAN_CDECL title_draw_cb(u8* btn, u32 a, u32 flags) {  // 0x006eee90
    if ((flags & 0xff) != 7 && (flags & 0xff & 1) != 0) return 0;
    c_title_cb.hit();
    Rect r = button_rect(btn);
    const char* title = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->workshop_text_10());  // 0x004ddfc0
    const i32 w = static_cast<i32>(joan::orig::as_font(kFonts)->measure(joan::orig::arg::str(title), -1, 0, 0, 1.0f));  // 0x00624f00
    if (jo::as_button(btn)->enabled == 0) return 1;
    void* const tex = obj(d::g_sheet);
    {
        const u32 colour = white_alpha(btn);
        sprite(r.x1, r.y1, a, str(d::s_TITLE), tex, colour);  // "TITLE"
    }
    {
        const u32 colour = white_alpha(btn);
        const i32 x = (static_cast<i32>(r.x1) + static_cast<i32>(r.x2) - w) / 2 - 0x48;
        sprite(x, static_cast<i32>(r.y1) + 4, a, str(d::s_L_ARROW), tex, colour);  // "L_ARROW"
    }
    {
        const u32 colour = white_alpha(btn);
        const i32 x = (static_cast<i32>(r.x2) + (w + static_cast<i32>(r.x1))) / 2 + 4;
        sprite(x, static_cast<i32>(r.y1) + 4, a, str(d::s_R_ARROW), tex, colour);  // "R_ARROW"
    }
    const u32 colour = white_alpha(btn);
    const char* text = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->workshop_text_10());  // 0x004ddfc0
    joan::orig::as_text(kFonts)->draw_3(static_cast<i32>(r.x1), joan::orig::arg::i32(static_cast<i32>(r.y1) - 1), static_cast<i32>(r.x2), joan::orig::arg::i32(static_cast<i32>(r.y2) - 5), joan::orig::arg::u32(a), text, joan::orig::arg::u32(colour), 0, 1, 2, 0, 1.0f, 1.0f);  // 0x00626130
    return 1;
}

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("TitleButton::draw_cb", 0x006eee90, &title_draw_cb);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    void* clear_72() { memset_(this, 0, static_cast<u32>(sizeof(jo::WorkScreen))); return this; }  // 0x72 on x86
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj6e8f10::Obj", 0x006e8f10, &H::clear_72);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::WorkshopScreen::release() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b103_15::H::work_release>(this)); }  // 0x6e9a50
auto joan::orig::WorkshopScreen::refresh() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_10::H::work_refresh>(this)); }  // 0x6ea020
auto joan::orig::WorkshopScreen::finish() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::work_finish>(this)); }  // 0x6ea1a0
auto joan::orig::WorkshopScreen::draw() -> void { joan::bind::member<&::game_b82_04::H::draw>(this); }  // 0x6ea310
auto joan::orig::WorkshopScreen::set_open(std::uint32_t a0) -> void { joan::bind::member<&::game_b100_16::H::work_set_open>(this, a0); }  // 0x6ed3e0
auto joan::orig::WorkshopScreen::is_open() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b94_08::H::byte_34>(this)); }  // 0x6ed4c0
auto joan::orig::Menu::load_barr() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_08::H::load_barr>(this)); }  // 0x6ed4e0
auto joan::orig::WorkshopScreen::free_sprites() -> void { joan::bind::member<&::game_b92_06::H::release3>(this); }  // 0x6ed5b0
