// game/unknown_chatbox.cpp: functions whose original source file is unknown (formerly game/_unplaced/chatbox.cpp;
// re/attribution.csv: between ww/camres.cpp and ww/cpuusage.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
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

using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::str;

using i32 = std::int32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::hook::global;

using joan::img::obj;

}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ tiny methods
    void nop() {}
    void* self() { return this; }
    u32 get_0_45b9b0() { return jo::self<jo::ChatBox>(this)->open_; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("Obj::get_0_45b9b0", 0x0045b9b0, &H::get_0_45b9b0);
B("Obj::self_45b550", 0x0045b550, &H::self);
B("Obj::nop_45b570", 0x0045b570, &H::nop);
B("Obj::nop_45bf60", 0x0045bf60, &H::nop);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
namespace {
namespace game_b100_02 {

using joan::call::thiscall;

i32 sb(u8 b) { return static_cast<i32>(static_cast<i8>(b)); }  // byte argument

JOAN_COUNTER(c_panel_init, "Panel::init");
JOAN_COUNTER(c_text_push, "TextRing::push");

using joan::orig::helpers::game_new;      // operator new

struct H {
    // ------------------------------------------------------------------ 0x0045b590
    std::uintptr_t panel_init(i32 x, i32 y, i32 h, i32 n, void* font, i32 a1c, i32 w20, i32 w24, i32 w28, i32 b2c,
                   i32 b30) {
        c_panel_init.hit();
        jo::self<jo::ChatBox>(this)->u8_2e4 = 0;
        jo::view<void*>(jo::self<jo::ChatBox>(this)->font) = font;
        jo::view<i32>(jo::self<jo::ChatBox>(this)->colour) = a1c;
        jo::self<jo::ChatBox>(this)->box_x1 = static_cast<i16>(x);
        jo::self<jo::ChatBox>(this)->box_y1 = static_cast<i16>(y);
        jo::self<jo::ChatBox>(this)->box_x2 = static_cast<i16>(h);
        jo::self<jo::ChatBox>(this)->box_y2 = static_cast<i16>(static_cast<u32>(static_cast<i32>(jo::self<jo::ChatBox>(this)->box_y1)) +
                                         static_cast<u32>(n) * 0x23u + 5u);
        const i32 sx = static_cast<i32>(static_cast<i16>(x));
        const i32 tw = joan::orig::arg::as<i32>(joan::orig::as_font(font)->measure(str(d::s_gt_sp), -1, 0, 0, 1.0f));  // 0x00624f00
        const i16 l14 = static_cast<i16>(sx + static_cast<i32>(static_cast<i16>(tw)) + 0xa);
        const i16 l18 = static_cast<i16>(static_cast<i32>(static_cast<i16>(y)) + 5);
        const i16 l1c = static_cast<i16>(static_cast<i32>(static_cast<i16>(h)) - 0xa);
        jo::self<jo::ChatBox>(this)->history_pos = -1;
        jo::self<jo::ChatBox>(this)->line_count = 0;
        jo::self<jo::ChatBox>(this)->i32_2d0 = n;
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::LineEdit)));  // 0x91 on x86
        void* const child = mem != nullptr ? joan::orig::as_line_edit(mem)->construct() : nullptr;  // 0x0045b790
        jo::view<void*>(jo::self<jo::ChatBox>(this)->edit) = child;
        return joan::orig::arg::as<u32>(joan::orig::as_text(jo::view<void*>(jo::self<jo::ChatBox>(this)->edit))->draw_aligned(joan::orig::arg::i16(static_cast<i32>(l14)), joan::orig::arg::i16(static_cast<i32>(l18)), joan::orig::arg::i16(static_cast<i32>(l1c)), joan::orig::arg::i16(static_cast<i32>(l18) + 0x1e), jo::self<jo::ChatBox>(this)->prompt, joan::orig::arg::i16(0x40), joan::orig::arg::mptr(font), joan::orig::arg::u32(a1c), joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(w20))), joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(w24))), joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(w28))), joan::orig::arg::u32(sb(static_cast<u8>(b2c))), static_cast<std::uint8_t>(joan::orig::arg::u32(sb(static_cast<u8>(b30))))));  // 0x0062d630
    }

    // ------------------------------------------------------------------ 0x0045be60
    char* text_push(const char* s) {
        c_text_push.hit();
        if (jo::self<jo::ChatBox>(this)->line_count < 0xa) {
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::ChatBox>(this)->lines + jo::self<jo::ChatBox>(this)->line_count * 0x40)), joan::orig::arg::str(s));  // strcpy
            jo::self<jo::ChatBox>(this)->line_count = jo::self<jo::ChatBox>(this)->line_count + 1;
            return reinterpret_cast<char*>(this);
        }
        i32 i = 0;
        for (; i < 9; ++i) {
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::ChatBox>(this)->lines + i * 0x40)), joan::orig::arg::str(jo::self<jo::ChatBox>(this)->lines + (i + 1) * 0x40));
        }
        return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::ChatBox>(this)->lines + i * 0x40)), joan::orig::arg::str(s));
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("Panel::init", 0x0045b590, &H::panel_init);
B100("TextRing::push", 0x0045be60, &H::text_push);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

using joan::call::thiscall;

using joan::orig::helpers::at;

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x0045b720
    std::uint32_t set_level(std::int32_t v) {
        if (jo::self<jo::Unit>(this)->hit_points_base < v) {
            const std::int32_t d = v - jo::self<jo::Unit>(this)->hit_points_base;
            jo::self<jo::Unit>(this)->hit_points_base = v;
            return joan::orig::arg::as<std::uint32_t>(joan::orig::as_unit(self())->set_value_135(joan::orig::arg::i32(jo::self<jo::Unit>(this)->hit_points + d)));  // 0x004fe510
        }
        jo::self<jo::Unit>(this)->hit_points_base = v;
        return static_cast<std::uint32_t>(v);
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Unit::set_level", 0x0045b720, &H::set_level);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using joan::call::thiscall;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x0045b790
    void* ctor_45b790() {
        joan::orig::as_gadget(this)->ctor();  // 0x0061de00
        jo::self<jo::LineEdit>(this)->vtable = img::obj(d::vt_Obj_9d037c);
        return this;
    }

    // ------------------------------------------------------------------ 0x0045b830
    void* scalar_dtor_45b830(u32 flags) {
        joan::orig::as_line_edit(this)->destruct();  // 0x0045b880
        if (flags & 1u) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::Obj_45b790", 0x0045b790, &H::ctor_45b790);
B95("Obj::scalar_dtor_45b830", 0x0045b830, &H::scalar_dtor_45b830);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

using joan::call::thiscall;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x0045b7c0
    void free_4() {
        if (jo::self<jo::ChatBox>(this)->edit == nullptr) return;
        void* const p = jo::self<jo::ChatBox>(this)->edit;
        if (p != nullptr) joan::orig::as_line_edit(p)->destroy(1u);  // 0x0045b830 scalar dtor
        jo::self<jo::ChatBox>(this)->edit = nullptr;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::free_4", 0x0045b7c0, &H::free_4);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::call::thiscall;

using joan::call::thiscall;

struct H {
    // 0x0045b880
    void* forward_61dfb0() { return joan::orig::arg::as<void*>(joan::orig::as_gadget(this)->destruct()); }  // 0x0061dfb0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("forward_61dfb0", 0x0045b880, &H::forward_61dfb0);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

using joan::call::thiscall;

using joan::hook::x87::ftol;

using joan::orig::helpers::at;

struct H {
    // 0x0045b8b0
    void finish() {
        jo::view<i32>(jo::self<jo::ChatBox>(this)->open_) = 1;
        if (jo::view<void*>(jo::self<jo::ChatBox>(this)->edit) != nullptr) joan::orig::as_line_edit(jo::view<void*>(jo::self<jo::ChatBox>(this)->edit))->set_focus();  // 0x0045bf80
        if (global<u8>(d::g_game_running) != 0) {
            global<u8>(d::g_game_running) = 0;
            global<i16>(d::g_saved_game_speed) = static_cast<i16>(joan::hook::x87::ftol(global<float>(d::g_game_speed)));
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(0u);  // 0x0042f930
        }
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::finish", 0x0045b8b0, &H::finish);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

using joan::call::thiscall;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x0045b930
    u32 reset_45b930() {
        if (jo::self<jo::ChatBox>(this)->edit != nullptr) joan::orig::as_line_edit(jo::self<jo::ChatBox>(this)->edit)->clear_focus();  // 0x0045bfb0
        jo::self<jo::ChatBox>(this)->open_ = 0;
        const u32 flag = global<u8>(d::g_game_running);
        if (flag != 0) return flag;
        global<u8>(d::g_game_running) = 1;
        const float v = static_cast<float>(static_cast<i32>(global<i16>(d::g_saved_game_speed)));  // fild (exact)
        joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(v));  // 0x0042f930
        return 0;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Obj::reset_45b930", 0x0045b930, &H::reset_45b930);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

using joan::call::thiscall;


JOAN_COUNTER(c_chat_draw, "ChatBox::draw");

struct H {

    // 0x0045b9d0 ChatBox::draw(unused): box (sprite queue 0xa46128), the last lines (64-byte
    // entries from +0x10) prefixed "> ", then the "> " prompt; the edit +4 placed below.
    void chat_draw(std::uint32_t) {
        c_chat_draw.hit();
        if (jo::self<jo::ChatBox>(this)->open_ == 0) return;
        const std::uint32_t colour = 0x5a000000u;
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->queue(joan::orig::arg::i32(jo::self<jo::ChatBox>(this)->box_x1), joan::orig::arg::i32(jo::self<jo::ChatBox>(this)->box_y1), joan::orig::arg::i32(jo::self<jo::ChatBox>(this)->box_x2), joan::orig::arg::i32(jo::self<jo::ChatBox>(this)->box_y2), 1u, joan::orig::arg::u32(colour), joan::orig::arg::mptr(0), 3, 0, 0, 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0);  // 0x00632a70
        std::int32_t n = jo::self<jo::ChatBox>(this)->i32_2d0 - 1;
        if (!(n < jo::self<jo::ChatBox>(this)->line_count)) n = jo::self<jo::ChatBox>(this)->line_count;
        const std::int16_t x = static_cast<std::int16_t>(static_cast<std::int32_t>(jo::self<jo::ChatBox>(this)->box_x1) + 10);
        std::int16_t y = static_cast<std::int16_t>(static_cast<std::int32_t>(jo::self<jo::ChatBox>(this)->box_y1) + 5);
        while (n != 0) {
            char buf[0x80];
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(buf))), str(d::s_gt_sp));  // strcpy "> "
            const std::uint32_t off = static_cast<std::uint32_t>(jo::self<jo::ChatBox>(this)->line_count - n) << 6;
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(buf))), joan::orig::arg::str(jo::self<jo::ChatBox>(this)->lines + off));  // strcat
            joan::orig::as_text(jo::view<void*>(jo::self<jo::ChatBox>(this)->font))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), 1u, static_cast<const char*>(buf), joan::orig::arg::u32(jo::self<jo::ChatBox>(this)->colour), joan::orig::arg::i32(-1), 0, 1, 0, 0, 1.0f, 1.0f);  // 0x006255d0
            y = static_cast<std::int16_t>(y + 0x23);
            n = n - 1;
        }
        joan::orig::as_text(jo::view<void*>(jo::self<jo::ChatBox>(this)->font))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), 1u, str(d::s_gt_sp), joan::orig::arg::u32(jo::self<jo::ChatBox>(this)->colour), joan::orig::arg::i32(-1), 0, 1, 0, 0, 1.0f, 1.0f);  // 0x006255d0
        jo::self<jo::ChatBox>(this)->edit->y = y;
        joan::orig::as_text_box(jo::view<void*>(jo::self<jo::ChatBox>(this)->edit))->draw(1u, 1u, 1u, 7u);  // 0x0062d730
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("ChatBox::draw", 0x0045b9d0, &H::chat_draw);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using joan::call::thiscall;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------------ 0x0045bba0
    void set_level(i32 v) {
        if (jo::self<jo::Unit>(this)->base_max_ep < v) {
            const i32 d = v - jo::self<jo::Unit>(this)->base_max_ep;
            jo::self<jo::Unit>(this)->base_max_ep = v;
            joan::orig::as_unit(this)->set_145(joan::orig::arg::i32(jo::self<jo::Unit>(this)->mana + d));  // 0x004fe5d0
        } else {
            jo::self<jo::Unit>(this)->base_max_ep = v;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Unit::set_level", 0x0045bba0, &H::set_level);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

using joan::call::thiscall;

using joan::orig::helpers::at;

JOAN_COUNTER(c_45bc10, "Menu::key_45bc10");

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x0045bc10
    u32 key_45bc10() {
        c_45bc10.hit();
        if (jo::self<jo::ChatBox>(this)->open_ == 0) return 0;
        const i32 key = joan::orig::arg::as<i32>(joan::orig::as_text(jo::self<jo::ChatBox>(this)->edit)->poll_62dc70(joan::orig::arg::u32(i32{1})));  // 0x0062dc70
        char* const line = jo::self<jo::ChatBox>(this)->prompt;
        auto entry = [this]() { return jo::self<jo::ChatBox>(this)->lines + jo::self<jo::ChatBox>(this)->history_pos * 0x40; };
        if (key == 0x1b) {
            joan::orig::as_chat_box(this)->close();  // 0x0045b930
        } else if (key == 0 || key == 1) {
        } else if (key == 0xd) {
            joan::orig::as_chat_box(this)->push_history(joan::orig::arg::str(jo::self<jo::ChatBox>(this)->edit->text));  // 0x0045be60
            joan::orig::as_line_edit(jo::self<jo::ChatBox>(this)->edit)->set_focus();                       // 0x0045bf80
            joan::orig::as_chat_box(this)->execute_input();                                  // 0x0045bf30
            joan::orig::as_edit(jo::self<jo::ChatBox>(this)->edit)->clear();                       // 0x0062e900
            jo::self<jo::ChatBox>(this)->history_pos = -1;
        } else if (key == 0x4800) {
            if (jo::self<jo::ChatBox>(this)->history_pos < 0) {
                jo::self<jo::ChatBox>(this)->history_pos = static_cast<i32>(jo::view<u32>(jo::self<jo::ChatBox>(this)->line_count) - 1u);
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(line)), joan::orig::arg::str(entry()));
            } else if (jo::self<jo::ChatBox>(this)->history_pos > 0) {
                jo::self<jo::ChatBox>(this)->history_pos = static_cast<i32>(jo::view<u32>(jo::self<jo::ChatBox>(this)->history_pos) - 1u);
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(line)), joan::orig::arg::str(entry()));
            }
        } else if (key == 0x5000) {
            if (jo::self<jo::ChatBox>(this)->history_pos >= 0 && jo::self<jo::ChatBox>(this)->history_pos < static_cast<i32>(jo::view<u32>(jo::self<jo::ChatBox>(this)->line_count) - 1u)) {
                jo::self<jo::ChatBox>(this)->history_pos = static_cast<i32>(jo::view<u32>(jo::self<jo::ChatBox>(this)->history_pos) + 1u);
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(line)), joan::orig::arg::str(entry()));
            } else if (jo::self<jo::ChatBox>(this)->history_pos == static_cast<i32>(jo::view<u32>(jo::self<jo::ChatBox>(this)->line_count) - 1u)) {
                jo::self<jo::ChatBox>(this)->history_pos = -1;
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(line)), str(d::kEmpty));
            }
        } else {
            jo::self<jo::ChatBox>(this)->history_pos = -1;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Menu::key_45bc10", 0x0045bc10, &H::key_45bc10);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

// qsort comparator of the trigger list (Triggers::update 0x004ef090 sorts the DynArray of Trigger POINTERS with it). The
// original reads byte +0x12 of the element SLOT, not of the record the slot points to: with 4-byte slots that is byte 2 of
// the pointer four slots further on, i.e. bits 16..23 of some trigger's heap address - an order that depends on the heap
// layout and that a 64-bit build (8-byte slots, other addresses) cannot reproduce. [Fixes] TriggerSort=1 (default)
// compares the records' own byte +0x12 (Trigger::b12, the creation argument the key was clearly meant to be); =0 keeps
// the original's slot-relative read (what the x86 build did before, and what a recording made that way replays).
i32 JOAN_CDECL cmp_byte12_45bdf0(const u8* a, const u8* b) {
    if (joan::hook::settings().fix_trigger_sort) {
        a = *reinterpret_cast<const u8* const*>(a);
        b = *reinterpret_cast<const u8* const*>(b);
    }
    const i8 x = static_cast<i8>(a[0x12]);
    const i8 y = static_cast<i8>(b[0x12]);
    if (x == y) return 0;
    return x <= y ? -1 : 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("cmp_byte12_45bdf0", 0x0045bdf0, &cmp_byte12_45bdf0);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

using joan::call::thiscall;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x0045bf30
    u32 query_70() { return joan::orig::arg::as<u32>(joan::orig::as_console(obj(d::g_console))->execute(joan::orig::arg::str(jo::self<jo::ChatBox>(this)->edit->text))); }  // 0x00454560
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Obj::query_70", 0x0045bf30, &H::query_70);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01 {

using joan::orig::helpers::at;

struct H {
    void set_83() { jo::self<jo::LineEdit>(this)->focused = 1; }             // 0x0045bf80
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("Obj::set_83", 0x0045bf80, &H::set_83);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x0045bfb0
    H* clear_83() {
        jo::self<jo::LineEdit>(this)->focused = 0;
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Obj::clear_83", 0x0045bfb0, &H::clear_83);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

void* memset_(void* d, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // ------------------------------------------------------------------ 0x0045bfe0 / 0x0045e980 / 0x00463920
    void* clear_60() {
        memset_(this, 0, static_cast<u32>(sizeof(jo::Profiler)));  // 0x60 on x86 (the Profiler 0xa485a8)
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::clear_60", 0x0045bfe0, &H::clear_60);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ChatBox::construct() -> void { joan::bind::member<&::game_b105_02::H::nop>(this); }  // 0x45b570
auto joan::orig::Panel::init(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, void* a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8, std::int32_t a9, std::int32_t a10) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_02::H::panel_init>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)); }  // 0x45b590
auto joan::orig::Unit::set_level_45b720(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b89_06::H::set_level>(this, a0)); }  // 0x45b720
auto joan::orig::LineEdit::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_01::H::ctor_45b790>(this)); }  // 0x45b790
auto joan::orig::LineEdit::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_01::H::scalar_dtor_45b830>(this, a0)); }  // 0x45b830
auto joan::orig::LineEdit::destruct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_02::H::forward_61dfb0>(this)); }  // 0x45b880
auto joan::orig::ChatBox::open() -> void { joan::bind::member<&::game_b94_01::H::finish>(this); }  // 0x45b8b0
auto joan::orig::ChatBox::close() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_01::H::reset_45b930>(this)); }  // 0x45b930
auto joan::orig::ChatBox::draw(std::uint32_t a0) -> void { joan::bind::member<&::game_leaves82b::H::chat_draw>(this, a0); }  // 0x45b9d0
auto joan::orig::Unit::set_level_45bba0(std::int32_t a0) -> void { joan::bind::member<&::game_b93_01::H::set_level>(this, a0); }  // 0x45bba0
auto joan::orig::ChatBox::push_history(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_02::H::text_push>(this, a0)); }  // 0x45be60
auto joan::orig::ChatBox::execute_input() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_18::H::query_70>(this)); }  // 0x45bf30
auto joan::orig::LineEdit::set_focus() -> void { joan::bind::member<&::game_b92_01::H::set_83>(this); }  // 0x45bf80
auto joan::orig::LineEdit::clear_focus() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_01::H::clear_83>(this)); }  // 0x45bfb0
auto joan::orig::Profiler::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_01::H::clear_60>(this)); }  // 0x45bfe0
