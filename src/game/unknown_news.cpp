// game/unknown_news.cpp: functions whose original source file is unknown (formerly game/_unplaced/news.cpp;
// re/attribution.csv: between ww/natres.cpp and ww/pemires.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cstddef>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i8 = std::int8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::img::obj;

using joan::call::thiscall;

using i16 = std::int16_t;
using u8 = std::uint8_t;

void* const kMsg = obj(d::g_msg_a39e88);

using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
namespace {
namespace game_b100_05 {

struct H {
    // ------------------------------------------------------------------ 0x004b9220
    H* ctor_4b9220() {
        joan::orig::dynarray(this)->construct(static_cast<std::int32_t>(sizeof(jo::NewsItem)), 0x64, 0);  // 0x005843d0 (0x52-byte records, no pointers)
        return this;
    }

    // ------------------------------------------------------------------ 0x004ba370
    i32 is_active() {
        const i32 kind = static_cast<i32>(jo::self<jo::NewsItem>(this)->kind);
        if (kind == 2) {
            const u32 g = global<u32>(d::g_player_side);
            return (g != 0 && jo::view<u32>(jo::self<jo::Order>(this)->last_ele) == g) ? 1 : 0;
        }
        return static_cast<i32>(global<i8>(d::g_a018c0 + (kind - 1) * 5));  // 5-byte records {code pointer, i8} from 0xa018bc
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Obj::ctor_4b9220", 0x004b9220, &H::ctor_4b9220);
B("Order::is_active", 0x004ba370, &H::is_active);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

void* const kGroups = obj(d::g_nations);

struct H {
    // ------------------------------------------------------------------ 0x004b9250
    jo::Font2d* reset_4b9250() {
        joan::orig::as_news_list(this)->clear();                                  // 0x004b92c0
        return joan::orig::as_news_list(this)->set_font(joan::orig::as_font2d(obj(d::g_font_verdana_10)));     // 0x004b9340
    }

    // ------------------------------------------------------------------ 0x004baab0
    void* record_3d(i32 /*unused*/) {
        const u8* const g = joan::orig::arg::as<const u8*>(joan::orig::as_dynarray(kGroups)->ptr_at(joan::orig::arg::i32(jo::view<i32>(jo::self<jo::NewsItem>(this)->unit))));  // 0x0047b600
        void* const unit = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(static_cast<i32>(jo::as_nation(g)->king_unit));  // 0x0041b3c0
        return joan::orig::arg::as<void*>(joan::orig::as_unit(unit)->name());  // 0x004fe4f0
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Obj::reset_4b9250", 0x004b9250, &H::reset_4b9250);
B98("Obj::record_3d", 0x004baab0, &H::record_3d);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
namespace {
namespace game_b84_06 {

struct H {
    // ------------------------------------------------------------------ 0x004b9290
    std::uint32_t wrap_4b9290() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_news_list(this)->clear()); }  // 0x004b92c0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("wrap_4b9290", 0x004b9290, &H::wrap_4b9290);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

struct H {
    std::uint32_t dtor_4b92c0() {  // 0x004b92c0
        joan::orig::dynarray(this)->zap(1);  // 0x00584580
        jo::self<jo::News>(this)->cached_size = 0;
        jo::self<jo::News>(this)->dirty = 1;
        joan::orig::as_action(this)->reset_bytes();  // 0x004b9310
        return 0;  // Action::reset_bytes returns nothing; all callers (0x004b925f, wrap_4b9290 and its callers) discard it
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj4b92c0::dtor", 0x004b92c0, &H::dtor_4b92c0);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
namespace {
namespace game_b86_06 {

struct H {
    // ---- 0x004b9310 -------------------------------------------------------------------------
    void reset_bytes() {
        jo::self<jo::News>(this)->flag_1d = 1;  // this: the News (its dtor 0x004b92c0 calls this); r8_x64
        jo::self<jo::News>(this)->flag_1e = 2;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("Action::reset_bytes", 0x004b9310, &H::reset_bytes);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

struct H {
    // ------------------------------------------------------------------ 0x004b9340
    jo::Font2d* set_a01894(jo::Font2d* v) {
        global<void*>(d::g_a01894) = v;
        return v;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("set_a01894", 0x004b9340, &H::set_a01894);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

struct H {
    // 0x004b9370 (dead)
    u32 flag_4b9370(u8 v) { return joan::orig::arg::as<u32>(joan::orig::as_list(this)->show_recent_4b93f0(0u, joan::orig::arg::i32(v))); }  // 0x004b93f0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Obj::flag_4b9370", 0x004b9370, &H::flag_4b9370);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
namespace {
namespace game_b108_05 {

struct H {
    // ------------------------------------------------------------------ 0x004b93b0
    u32 forward_4b93b0() { return joan::orig::arg::as<u32>(joan::orig::as_list(this)->show_recent_4b93f0(joan::orig::arg::u32(i32{1}), i32{0})); }  // 0x004b93f0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("Obj::forward_4b93b0", 0x004b93b0, &H::forward_4b93b0);
#undef B

}  // namespace game_b108_05
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

JOAN_COUNTER(c_recent, "List::show_recent_4b93f0");

struct H {
    // -------------------------------------------------------------- 0x004b93f0
    i32 show_recent(u32 arg, i32 mode_arg) {
        c_recent.hit();
        i32 count = 0;
        for (i32 n = joan::orig::as_dynarray(this)->size();; --n) {  // 0x00417ad0
            const i32 lim = jo::self<jo::NewsList>(this)->shown + 1 < 1 ? 1 : jo::self<jo::NewsList>(this)->shown + 1;
            if (n < lim) break;
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(n)));  // 0x00417200
            if (joan::orig::arg::as<i32>(joan::orig::as_order(e)->is_active()) == 0) continue;   // 0x004ba370
            if (!(global<i32>(d::g_get_a47bb0) < jo::view<i32>(jo::as_news_item(e)->date) + 0x3c)) break;
            ++count;
            if (!(count < 5)) break;
        }
        i32 y = global<i32>(d::g_screen_h) - 0x7e;
        for (i32 k = count; k > 0; --k) {
            const i32 idx = joan::orig::as_dynarray(this)->size() - k + 1;  // 0x00417ad0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(idx)));  // 0x00417200
            if (joan::orig::arg::as<i32>(joan::orig::as_order(e)->is_active()) == 0) continue;         // 0x004ba370
            if (jo::as_news_item(e)->kind == 1) {
                u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_talk(obj(d::g_queue))->get_msg(joan::orig::arg::i32(jo::view<u32>(jo::as_news_item(e)->count))));  // 0x004e3fe0
                if (jo::as_treaty(o)->reply == 1 && joan::orig::arg::as<i32>(joan::orig::as_treaty(o)->still_valid()) == 0) {            // 0x004e3180
                    joan::orig::dynarray(this)->linkout(joan::orig::arg::i32(idx));  // 0x00584a10
                    continue;
                }
            }
            i32 height = 0;
            if (joan::orig::arg::as<i32>(joan::orig::as_news_item(e)->draw_tooltip(joan::orig::arg::i32(y), joan::orig::arg::i32(arg), &height, static_cast<char>(joan::orig::arg::i32(static_cast<u8>(mode_arg))))) != 0) return 1;  // 0x004b96f0
            y -= height;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("List::show_recent_4b93f0", 0x004b93f0, &H::show_recent);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03 {

void* const kUnits = obj(d::g_units);

struct H {
    // ------------------------------------------------------------------ 0x004b95c0
    u32 cache_4b95c0() {
        const u32 v = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->size());  // 0x00417ad0
        jo::self<jo::News>(this)->cached_size = v;
        return v;
    }

    // ------------------------------------------------------------------ 0x004b9eb0 / 0x004b9ef0 / 0x004b9f30
    void* add_4b9eb0(u32 a) { return joan::orig::as_messages(this)->post(5u, 0u, joan::orig::arg::i32(a), 0, 0u); }        // 0x004b9a90
    void* add_4b9ef0(u32 a, u32 b) { return joan::orig::as_messages(this)->post(6u, 0u, joan::orig::arg::i32(a), joan::orig::arg::i32(b), 0u); }  // 0x004b9a90
    void* add_4b9f30(u32 a) { return joan::orig::as_messages(this)->post(7u, 0u, joan::orig::arg::i32(a), 0, 0u); }        // 0x004b9a90

    // ------------------------------------------------------------------ 0x004ba450
    void* destroyed_4ba450() {
        const char* const name = joan::orig::as_news_item(this)->player_c_name();  // 0x004baa30
        joan::orig::as_string(kMsg)->assign(joan::orig::arg::str(name));  // 0x005a1440
        return joan::orig::as_string(kMsg)->append(joan::orig::arg::str(&img::global<const char>(d::s_has_been_destroyed_9d53cc)));  // 0x005a1550
    }

    // ------------------------------------------------------------------ 0x004ba9d0
    void* flees_4ba9d0() {
        const char* const r = jo::as_unit_type(joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i16>(jo::self<jo::NewsItem>(this)->count)))))->title_b4;  // 0x0042d0a0
        joan::orig::as_string(kMsg)->assign(joan::orig::arg::str(r));                                // 0x005a1440
        return joan::orig::as_string(kMsg)->append(joan::orig::arg::str(&img::global<const char>(d::s_troop_flees)));  // 0x005a1550
    }

    // ------------------------------------------------------------------ 0x004bab00
    void* notify_4bab00(u32 /*unused*/) {
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(joan::orig::arg::i32(jo::self<jo::NewsItem>(this)->unit)));  // 0x0047b600
        void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(jo::as_nation(e)->king_unit));  // 0x0041b3c0
        return joan::orig::as_unit(u)->name();  // 0x004fe4f0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Obj::cache_4b95c0", 0x004b95c0, &H::cache_4b95c0);
B("Orders::add_4b9eb0", 0x004b9eb0, &H::add_4b9eb0);
B("Orders::add_4b9ef0", 0x004b9ef0, &H::add_4b9ef0);
B("Orders::add_4b9f30", 0x004b9f30, &H::add_4b9f30);
B("Msg::destroyed_4ba450", 0x004ba450, &H::destroyed_4ba450);
B("Msg::flees_4ba9d0", 0x004ba9d0, &H::flees_4ba9d0);
B("Obj::notify_4bab00", 0x004bab00, &H::notify_4bab00);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

JOAN_COUNTER(c_remove_match, "Queue::remove_match");

i32 count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------------ 0x004b95f0
    void remove_match(i32 a, i32 b, i32 c) {
        c_remove_match.hit();
        void* const q = obj(d::g_tasks);
        for (i32 i = count(this); i > 0; --i) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_news_list(q)->item(joan::orig::arg::i32(i)));  // 0x004b96c0
            if (static_cast<i32>(jo::as_news_item(e)->kind) != a) continue;
            if (b >= 0 && jo::as_news_item(e)->count != b) continue;
            if (c >= 0 && jo::view<i32>(jo::as_news_item(e)->unit) != c) continue;
            joan::orig::dynarray(q)->linkout(joan::orig::arg::i32(i));  // 0x00584a10
            // this is the NewsList (the task list g_tasks, 0xa50020), so +0x20 on x86 is NewsList::shown (x64 +0x24)
            if (i <= jo::self<jo::NewsList>(this)->shown && jo::self<jo::NewsList>(this)->shown > 0) jo::self<jo::NewsList>(this)->shown = jo::self<jo::NewsList>(this)->shown - 1;
            return;
        }
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("Queue::remove_match", 0x004b95f0, &H::remove_match);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {

struct H {
    // ------------------------------------------------------------------------ 0x004b96c0
    void* slot(i32 i) { return joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(i)); }  // 0x00417200

    // ------------------------------------------------------------------------ 0x004ba1f0
    u32 add_31(u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7) {
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_messages(this)->post(joan::orig::arg::u32(i32{0x31}), joan::orig::arg::u32(i32{0}), joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), joan::orig::arg::u32(i32{0})));  // 0x004b9a90
        if (t == nullptr) return 0;
        jo::view<u32>(jo::as_news_item(t)->count) = a1;
        jo::view<u32>(jo::as_news_item(t)->value) = a2;
        return joan::orig::arg::as<u32>(joan::orig::as_news_item(t)->set_args(joan::orig::arg::u32(a5), joan::orig::arg::u32(a6), joan::orig::arg::u32(a7), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0})));  // 0x004b9c80
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::slot", 0x004b96c0, &H::slot);
B94("TaskList::add_31", 0x004ba1f0, &H::add_31);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

JOAN_COUNTER(c_tooltip, "Item::draw_tooltip");

struct H {
    // ------------------------------------------------------------------ 0x004b96f0
    i32 draw_tooltip(i32 x, i32 with_button, i32* height, char flags) {
        c_tooltip.hit();
        char name[0x194];
        const char* const text = joan::orig::as_news_item(this)->text();  // 0x004ba320
        joan::orig::as_string(name)->construct(joan::orig::arg::str(text));  // 0x005a11a0
        const char* const label = joan::orig::as_date(obj(d::g_date))->date_str(joan::orig::arg::i32(jo::self<jo::NewsItem>(this)->date), i32{1}, i32{0});  // 0x0057fc30
        const i32 w1 = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_font(global<void*>(d::g_a01894))->measure(joan::orig::arg::str(label), -1, 0, 0, joan::orig::arg::f32(u32{0x3f800000})))) +
                       5;  // 0x00624f00
        *height = 0x10;
        const char* const name_str = joan::orig::arg::str(joan::orig::as_string(name)->c_str());  // 0x00417d90
        const i32 w2 = static_cast<i16>(
            joan::orig::arg::as<u32>(joan::orig::as_font(global<void*>(d::g_a01894))->measure(joan::orig::arg::str(name_str), -1, 0, 0, joan::orig::arg::f32(u32{0x3f800000}))));
        if (static_cast<i32>(static_cast<u32>(w1) + static_cast<u32>(w2) + 0x11u) > global<i32>(d::g_screen_w) - 0x2f) {
            x -= 0x10;
            *height = *height + 0x10;
        }
        if ((static_cast<i32>(flags) & 2) != 0) {
            const u32 shade = 0x80000000u;
            joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->queue(i32{5}, joan::orig::arg::i32(x - 3), joan::orig::arg::i32(global<i32>(d::g_screen_w) - 0x27), joan::orig::arg::i32(x + *height - 4), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(shade), joan::orig::arg::mptr(i32{0}), i32{3}, i32{1}, i32{1}, i32{0}, i32{-1}, i32{-1}, i32{-1}, i32{-1}, i32{0});  // 0x00632a70
        }
        if (with_button != 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_parser(obj(d::g_mouse))->rect_mode(i32{8}, joan::orig::arg::i32(x), i32{0x12}, joan::orig::arg::i32(x + 0xa), i32{2})) != 0) {  // 0x005940f0
                if (jo::self<jo::NewsItem>(this)->kind == 1) {
                    u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_talk(obj(d::g_queue))->get_msg(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::NewsItem>(this)->count))));  // 0x004e3fe0
                    if (jo::as_treaty(rec)->reply == 1) return 1;
                } else if (jo::self<jo::NewsItem>(this)->kind != 0x33) {
                    joan::orig::as_news_item(this)->check_state_41();  // 0x00453ce0
                }
            }
            return 0;
        }
        if (jo::self<jo::NewsItem>(this)->kind == 1) joan::orig::as_talk(obj(d::g_queue))->get_msg(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::NewsItem>(this)->count)));  // result only fed a dead local
        if ((static_cast<i32>(flags) & 4) != 0) {
            const u32 white = 0xffffffffu;
            joan::orig::as_text(global<void*>(d::g_a01894))->draw_line(i32{0x19}, joan::orig::arg::i32(x), joan::orig::arg::u32(i32{1}), label, joan::orig::arg::u32(white), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000}));  // 0x006255d0 (word x)
            const char* const s = joan::orig::arg::str(joan::orig::as_string(name)->c_str());
            joan::orig::as_text(global<void*>(d::g_a01894))->draw(joan::orig::arg::i32(w1 + 0x19), joan::orig::arg::i32(x), joan::orig::arg::i32(global<i32>(d::g_screen_w) - 0x28), joan::orig::arg::i32(x + *height), joan::orig::arg::u32(i32{1}), s, joan::orig::arg::u32(white), i32{2}, i32{1}, i32{1}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000}), i32{0}, i32{-1});  // 0x00626320 (word x)
        }
        return 0;
    }
};

JOAN_REPLACE("game/b108_07", "Item::draw_tooltip", 0x004b96f0, &H::draw_tooltip);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b89_05.cpp (game/b89_05)
namespace {
namespace game_b89_05 {

using joan::orig::helpers::s16;

void* const kNames = obj(d::g_nations);  // 0x0047b600

JOAN_COUNTER(c_post, "Messages::post");

struct H {
    // ------------------------------------------------------------ 0x004b9a90 (ret 0x14)
    void* post(std::uint32_t a, std::uint32_t b, std::int32_t id1, std::int32_t id2, std::uint32_t) {
        c_post.hit();
        global<std::uint8_t>(d::g_a39e20) = static_cast<std::uint8_t>(a);
        global<std::uint8_t>(d::g_a39e21) = static_cast<std::uint8_t>(b);
        global<std::uint32_t>(d::g_a39e34) = global<std::uint32_t>(d::g_get_a47bb0);
        global<std::uint8_t>(d::g_a39e61) = 0;
        if (id1 != 0) {
            void* const e = joan::orig::as_dynarray(kNames)->ptr_at(joan::orig::arg::i32(id1));  // 0x0047b600
            global<std::int32_t>(d::g_a39e2c) = s16(jo::view<std::uint16_t>(jo::as_nation(e)->id));
            global<std::uint8_t>(d::g_a39e22) = joan::orig::arg::as<std::uint8_t>(joan::orig::as_nation(e)->race());  // 0x004b4120
            global<std::uint32_t>(d::g_a39e24) = jo::view<std::uint32_t>(jo::as_nation(e)->colour);
        } else {
            global<std::int32_t>(d::g_a39e2c) = 0;
            joan::orig::as_color(obj(d::g_a39e24))->set_bgr(joan::orig::arg::mptr(obj(d::g_init_white)));  // 0x005e9430
        }
        if (id2 != 0) {
            void* const e = joan::orig::as_dynarray(kNames)->ptr_at(joan::orig::arg::i32(id2));  // 0x0047b600
            global<std::int32_t>(d::g_a39e30) = s16(jo::view<std::uint16_t>(jo::as_nation(e)->id));
            global<std::uint8_t>(d::g_a39e23) = joan::orig::arg::as<std::uint8_t>(joan::orig::as_nation(e)->race());  // 0x004b4120
            global<std::uint32_t>(d::g_a39e28) = jo::view<std::uint32_t>(jo::as_nation(e)->colour);
        } else {
            global<std::int32_t>(d::g_a39e30) = 0;
            joan::orig::as_color(obj(d::g_a39e28))->set_bgr(joan::orig::arg::mptr(obj(d::g_init_white)));  // 0x005e9430
        }
        if (static_cast<std::int8_t>(jo::self<jo::News>(this)->dirty) == 0) return obj(d::g_a39e20);
        if (joan::orig::as_dynarray(this)->size() >= 0x7d0) {  // 0x00417ad0
            joan::orig::as_counter(this)->clamp_to_one();             // 0x004b9d40
            joan::orig::dynarray(this)->linkout(joan::orig::arg::i32(-1));  // 0x00584a10
            if (jo::view<std::int32_t>(jo::self<jo::News>(this)->cached_size) > 1) jo::view<std::int32_t>(jo::self<jo::News>(this)->cached_size) = jo::view<std::int32_t>(jo::self<jo::News>(this)->cached_size) - 1;
        }
        joan::orig::dynarray(this)->linkin(joan::orig::arg::ptr(obj(d::g_a39e20)));  // 0x00584620
        return joan::orig::as_pool(this)->last_used();           // 0x004b9c30
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b89_05", name, addr, fn)
B("Messages::post", 0x004b9a90, &H::post);
#undef B

}  // namespace game_b89_05
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

struct H {
    // 0x004b9c30 Pool::last_used: (+8 - 1) * +0x10 + +0x19, 0 when +8 is 0.
    char* pool_last() {
        if (jo::self<jo::DynArray>(this)->cur_pos == 0) return nullptr;
        return jo::self<jo::DynArray>(this)->body_buf + static_cast<std::uint32_t>(jo::self<jo::DynArray>(this)->cur_pos - 1) * jo::view<std::uint32_t>(jo::self<jo::DynArray>(this)->ele_size);
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("Pool::last_used", 0x004b9c30, &H::pool_last);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

struct H {
    // 0x004b9c80: eax is left holding a2 (last value loaded into eax before the epilogue).
    std::uint32_t rec_set(std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t b, std::uint32_t w1,
                          std::uint32_t w2) {
        jo::self<jo::NewsItem>(this)->arg_b = static_cast<std::uint8_t>(b);
        jo::self<jo::NewsItem>(this)->arg_w1 = static_cast<std::uint16_t>(w1);
        jo::self<jo::NewsItem>(this)->arg_w2 = static_cast<std::uint16_t>(w2);
        jo::self<jo::NewsItem>(this)->args[0] = a1;
        jo::self<jo::NewsItem>(this)->args[1] = a2;
        jo::self<jo::NewsItem>(this)->args[2] = a3;
        return a2;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Rec::set", 0x004b9c80, &H::rec_set);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

struct H {
    // 0x004b9d40
    void* clamp_to_one() {
        jo::self<jo::DynArray>(this)->cur_pos = jo::self<jo::DynArray>(this)->last_ele < 1 ? jo::self<jo::DynArray>(this)->last_ele : 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("Counter::clamp_to_one", 0x004b9d40, &H::clamp_to_one);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

JOAN_COUNTER(c_events_add, "Events::add_typed");

struct H {
    // ------------------------------------------------------------------------ 0x004b9e20
    i32 events_add(u32 id, u32 data) {
        c_events_add.hit();
        u8* const d = joan::orig::arg::as<u8*>(joan::orig::as_talk(obj(d::g_queue))->get_msg(joan::orig::arg::i32(id)));  // 0x004e3fe0
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_messages(this)->post(joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{0}), joan::orig::arg::i32(// 0x004b9a90
                                    static_cast<i32>(jo::as_treaty(d)->from)), static_cast<i32>(jo::as_treaty(d)->to), joan::orig::arg::u32(i32{0})));
        if (e == nullptr) return 0;
        jo::view<u32>(jo::as_news_item(e)->count) = id;
        jo::view<u32>(jo::as_news_item(e)->value) = data;
        return 1;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("Events::add_typed", 0x004b9e20, &H::events_add);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

JOAN_COUNTER(c_add_4b9f70, "Obj::add_4b9f70");

struct H {
    // ------------------------------------------------------------------ 0x004b9f70
    u32 add_4b9f70(const char* a1, u32 a2, u32 a3, u32 a4, const u8* a5) {
        c_add_4b9f70.hit();
        i32 extra = 0;
        if (a5 != nullptr) extra = jo::as_unit(a5)->side;
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_messages(this)->post(0xcu, 0u, joan::orig::arg::i32(global<u32>(d::g_player_side)), joan::orig::arg::i32(extra), 0u));  // 0x004b9a90
        if (p == nullptr) return 0;
        joan::orig::as_misc(obj(d::g_misc))->str_cut_58f050(joan::orig::arg::str(static_cast<void*>(jo::as_news_item(p)->name)), joan::orig::arg::str(a1), 1, 0x14);  // 0x0058f050
        jo::view<u32>(jo::as_news_item(p)->value) = 0;
        if (a5 != nullptr && static_cast<i32>(jo::as_unit(a5)->side) != 0) jo::view<u32>(jo::as_news_item(p)->value) = 1;
        return joan::orig::arg::as<u32>(joan::orig::as_news_item(p)->set_args(joan::orig::arg::u32(a2), joan::orig::arg::u32(a3), joan::orig::arg::u32(a4), 0u, 0u, 0u));  // 0x004b9c80
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::add_4b9f70", 0x004b9f70, &H::add_4b9f70);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b102_04.cpp (game/b102_04)
namespace {
namespace game_b102_04 {

using u16 = std::uint16_t;


void* const kKinds = obj(d::g_types);

void* msg_assign(const void* s) { return joan::orig::as_string(kMsg)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void* msg_append(const void* s) { return joan::orig::as_string(kMsg)->append(joan::orig::arg::str(s)); }  // 0x005a1550

JOAN_COUNTER(c_news_post, "News::post");
JOAN_COUNTER(c_text_slain, "News::text_slain");
JOAN_COUNTER(c_text_destroyed, "News::text_destroyed");
JOAN_COUNTER(c_text_attacked, "News::text_attacked");

struct H {
    // ------------------------------------------------------------------ 0x004ba040
    u32 news_post(const char* name, u32 a, u32 b, u32 c, u8* who) {
        c_news_post.hit();
        i32 id = 0;
        if (who != nullptr) id = jo::as_unit(who)->side;
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_messages(this)->post(0xdu, 0u, joan::orig::arg::i32(global<u32>(d::g_player_side)), joan::orig::arg::i32(id), 0u));  // 0x004b9a90
        if (e == nullptr) return 0;
        joan::orig::as_misc(obj(d::g_misc))->str_cut_58f050(joan::orig::arg::str(static_cast<void*>(jo::as_news_item(e)->name)), joan::orig::arg::str(name), 1, 0x14);  // 0x0058f050
        jo::view<u32>(jo::as_news_item(e)->value) = 0;
        if (who != nullptr && jo::as_unit(who)->side != 0) jo::view<u32>(jo::as_news_item(e)->value) = 1;
        return joan::orig::arg::as<u32>(joan::orig::as_news_item(e)->set_args(joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), 0u, 0u, 0u));  // 0x004b9c80
    }

    // ------------------------------------------------------------------ 0x004ba4a0
    void* text_surrender() {
        msg_assign(joan::orig::arg::as<void*>(joan::orig::as_news_item(this)->player_c_name()));  // 0x004baa30
        void* r = msg_append(str(d::s_has_surrendered_to));            // " has surrendered to "
        if (global<u32>(d::g_player_side) != 0 && jo::self<jo::NewsItem>(this)->player == global<u32>(d::g_player_side)) {
            r = msg_append(str(d::s_you));  // "you."
        } else {
            msg_append(joan::orig::arg::as<void*>(joan::orig::as_news_item(this)->player_10_name()));  // 0x004baa70
            r = msg_append(str(d::s_dot_9d36e8));
        }
        return r;
    }

    // ------------------------------------------------------------------ 0x004ba540
    void* text_slain() {
        c_text_slain.hit();
        if (global<u32>(d::g_player_side) != 0 && jo::self<jo::NewsItem>(this)->unit == global<u32>(d::g_player_side)) {
            if (static_cast<i32>(jo::self<jo::NewsItem>(this)->side) >= 0)
                msg_assign(str(d::s_Your_King));  // "Your King, "
            else
                msg_assign(str(d::s_Your_All_High));  // "Your All High, "
            msg_append(joan::orig::arg::as<void*>(joan::orig::as_news_item(this)->unit_pos(0)));  // 0x004baab0
            return msg_append(str(d::s_has_been_slain_9d543c));                // ", has been slain."
        }
        if (static_cast<i32>(jo::self<jo::NewsItem>(this)->side) >= 0)
            msg_assign(str(d::s_King));  // "King "
        else
            msg_assign(str(d::s_All_High));  // "All High "
        msg_append(joan::orig::arg::as<void*>(joan::orig::as_news_item(this)->unit_pos(0)));  // 0x004baab0
        msg_append(str(d::s_of));                       // " of "
        msg_append(joan::orig::arg::as<void*>(joan::orig::as_news_item(this)->player_c_name()));     // 0x004baa30
        return msg_append(str(d::s_has_been_slain_9d540c));                // " has been slain."
    }

    // Shared tail of 0x4ba670 / 0x4ba750 (r = the previous append's result).
    void* town_by(void* r) {
        const i32 k = jo::self<jo::NewsItem>(this)->value;
        if (k == 0) return msg_append(str(d::s_dot_9d36e8));
        if (k == 1) {
            msg_append(str(d::s_by));                    // " by "
            msg_append(joan::orig::arg::as<void*>(joan::orig::as_news_item(this)->player_10_name()));  // 0x004baa70
            return msg_append(str(d::s_dot_9d36e8));
        }
        if (k == 2) return msg_append(str(d::s_by_Fryhtans));  // " by Fryhtans."
        return r;
    }

    // ------------------------------------------------------------------ 0x004ba670
    void* text_destroyed() {
        c_text_destroyed.hit();
        msg_assign(str(d::s_Your_town_of));  // "Your town of "
        msg_append(jo::self<jo::NewsItem>(this)->name);
        void* const r = msg_append(str(d::s_has_been_destroyed_9d5490));  // " has been destroyed"
        return town_by(r);
    }

    // ------------------------------------------------------------------ 0x004ba750
    void* text_attacked() {
        c_text_attacked.hit();
        msg_assign(str(d::s_Your_town_of));  // "Your town of "
        msg_append(jo::self<jo::NewsItem>(this)->name);
        void* const r = msg_append(str(d::s_has_been_attacked));  // " has been attacked"
        return town_by(r);
    }

    // ------------------------------------------------------------------ 0x004ba890 / 0x004ba930
    void* text_troop(img::Addr verb) {
        u8* const k1 = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(static_cast<u32>(static_cast<u16>(jo::self<jo::NewsItem>(this)->count)))));  // 0x0042d0a0
        msg_assign(jo::as_unit_type(k1)->title_b4);
        msg_append(str(verb));
        u8* const k2 = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(static_cast<u32>(static_cast<u16>(jo::self<jo::NewsItem>(this)->value)))));  // 0x0042d0a0
        msg_append(jo::as_unit_type(k2)->title_b4);
        return msg_append(str(d::s_troop));  // " troop."
    }
    void* text_reinforce() { return text_troop(d::s_troop_goes_to_reinforce); }  // " troop goes to reinforce "
    void* text_attack() { return text_troop(d::s_troop_goes_to_attack); }     // " troop goes to attack "
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("News::post", 0x004ba040, &H::news_post);
B("News::text_surrender", 0x004ba4a0, &H::text_surrender);
B("News::text_slain", 0x004ba540, &H::text_slain);
B("News::text_destroyed", 0x004ba670, &H::text_destroyed);
B("News::text_attacked", 0x004ba750, &H::text_attacked);
B("News::text_reinforce", 0x004ba890, &H::text_reinforce);
B("News::text_attack", 0x004ba930, &H::text_attack);
#undef B

}  // namespace game_b102_04
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

// 0x004ba300: mov ecx, 0xa39e88; call 0x00403deb (-> 0x005a1170); eax kept.
void* JOAN_CDECL init_4ba300() { return joan::orig::as_string(obj(d::g_msg_a39e88))->construct(); }  // 0x005a1170

struct H {
    void nop0() {}
    void nop1(u32) {}
    void nop2(u32, u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("init_4ba300", 0x004ba300, &init_4ba300);
B("Obj::nop_4ba830", 0x004ba830, &H::nop0);
B("Obj::nop_4ba850", 0x004ba850, &H::nop0);
B("Obj::nop_4ba870", 0x004ba870, &H::nop0);
B("Obj::nop_4ba150", 0x004ba150, &H::nop1);
B("Obj::nop_4ba110", 0x004ba110, &H::nop2);
B("Obj::nop_4ba130", 0x004ba130, &H::nop2);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

JOAN_COUNTER(c_add_entry, "X::add_entry");

struct H {
    // ------------------------------------------------------------------------ 0x004ba170
    u32 add_entry(u32 a, u32 b, u32 c, u32 d, u32 e, u32 g, u32 h) {
        c_add_entry.hit();
        u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_messages(this)->post(0x30u, 0u, joan::orig::arg::i32(c), joan::orig::arg::i32(d), 0u));  // 0x004b9a90
        if (r == nullptr) return 0;
        jo::view<u32>(jo::as_news_item(r)->count) = a;
        jo::view<u32>(jo::as_news_item(r)->value) = b;
        return joan::orig::arg::as<u32>(joan::orig::as_news_item(r)->set_args(joan::orig::arg::u32(e), joan::orig::arg::u32(g), joan::orig::arg::u32(h), 0u, 0u, 0u));  // 0x004b9c80
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("X::add_entry", 0x004ba170, &H::add_entry);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {

JOAN_COUNTER(c_spawn, "Spawner::spawn");

struct H {
    // 0x004ba270
    std::int32_t spawn(std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5) {
        c_spawn.hit();
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_messages(this)->post(0x32u, 0u, joan::orig::arg::i32(a2), 0, 0u));  // 0x004b9a90
        if (p == nullptr) return 0;
        jo::as_news_item(p)->count = a1;
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_news_item(p)->set_args(joan::orig::arg::u32(a3), joan::orig::arg::u32(a4), joan::orig::arg::u32(a5), 0u, 0u, 0u));  // 0x004b9c80
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Spawner::spawn", 0x004ba270, &H::spawn);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b107_10.cpp (game/b107_10)
namespace {
namespace game_b107_10 {

using joan::call::cdecl_call;

// ---- CRT initialiser wrappers: call the bodies directly, return the last eax ----
template <fn::Code Body>
u32 JOAN_CDECL init1() {
    return cdecl_call<u32>(Body);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_10", name, addr, fn)
B("init_0x004ba2e0", 0x004ba2e0, &init1<fn::init_4ba300>);
#undef B

}  // namespace game_b107_10
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

struct H {
    // ------------------------------------------------------------------ 0x004ba320
    const char* dispatch() {
        const i32 idx = (static_cast<i32>(jo::self<jo::NewsItem>(this)->kind) - 1) * 5;
        const img::Addr slot = d::fns_Cmd + idx;
#if JOAN_IMAGE_X64
        // x64: the image widened only the code slots of this {code, i8} table that hold a relocation; the others
        // keep 4 bytes (an 8-byte read would take the next record's bytes).
        const std::uintptr_t fn = img::offset(slot + 4) - img::offset(slot) == 4 ? global<u32>(slot) : global<std::uintptr_t>(slot);
#else
        const std::uintptr_t fn = global<std::uintptr_t>(slot);
#endif
        thiscall(fn, this);
        return joan::orig::arg::as<const char*>(joan::orig::as_string(obj(d::g_msg_a39e88))->c_str());  // 0x00417d90
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Obj::dispatch", 0x004ba320, &H::dispatch);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b107_11.cpp (game/b107_11)
namespace {
namespace game_b107_11 {

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x004ba3f0
    void* send_4ba3f0() {
        void* p = joan::orig::arg::as<void*>(joan::orig::as_talk(obj(d::g_queue))->get_msg(joan::orig::arg::i32(jo::self<jo::Cmd>(this)->u32_18)));  // 0x004e3fe0
        const char* const r = joan::orig::arg::as<const char*>(joan::orig::as_treaty(p)->format_text(joan::orig::arg::u32(global<u32>(d::g_player_side)), joan::orig::arg::u32(jo::self<jo::Cmd>(this)->u32_1c), joan::orig::arg::u32(static_cast<u32>(0))));                       // 0x004e0030
        return joan::orig::as_string(obj(d::g_msg_a39e88))->assign(joan::orig::arg::str(r));                   // 0x005a1440
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_11", name, addr, fn)
B("Cmd::send_4ba3f0", 0x004ba3f0, &H::send_4ba3f0);
#undef B

}  // namespace game_b107_11
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
namespace {
namespace game_b92_03 {

void* const kPlayers = obj(d::g_nations);

std::uint8_t* player(std::int32_t i) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kPlayers)->ptr_at(joan::orig::arg::i32(i))); }  // 0x0047b600

struct H {
    // 0x004baa30 / 0x004baa70
    const char* player_c() { return joan::orig::arg::str(joan::orig::as_nation(player(jo::view<std::int32_t>(jo::self<jo::NewsItem>(this)->unit)))->name()); }   // 0x004b4280
    const char* player_10() { return joan::orig::arg::str(joan::orig::as_nation(player(jo::view<std::int32_t>(jo::self<jo::NewsItem>(this)->player)))->name()); }  // 0x004b4280
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Obj::player_c", 0x004baa30, &H::player_c);
B("Obj::player_10", 0x004baa70, &H::player_10);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::NewsList::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_05::H::ctor_4b9220>(this)); }  // 0x4b9220
auto joan::orig::NewsList::clear_wrap() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b84_06::H::wrap_4b9290>(this)); }  // 0x4b9290
auto joan::orig::NewsList::clear() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b85_06::H::dtor_4b92c0>(this)); }  // 0x4b92c0
auto joan::orig::Action::reset_bytes() -> void { joan::bind::member<&::game_b86_06::H::reset_bytes>(this); }  // 0x4b9310
auto joan::orig::NewsList::set_font(Font2d* a0) -> Font2d* { return joan::bind::conv<Font2d*>(joan::bind::member<&::game_b95_02::H::set_a01894>(this, a0)); }  // 0x4b9340
auto joan::orig::NewsList::show_recent() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_05::H::forward_4b93b0>(this)); }  // 0x4b93b0
auto joan::orig::List::show_recent_4b93f0(std::uint32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_01::H::show_recent>(this, a0, a1)); }  // 0x4b93f0
auto joan::orig::NewsList::remove_match(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::member<&::game_b99_03::H::remove_match>(this, a0, a1, a2); }  // 0x4b95f0
auto joan::orig::NewsList::item(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_02::H::slot>(this, a0)); }  // 0x4b96c0
auto joan::orig::NewsItem::draw_tooltip(std::int32_t a0, std::int32_t a1, void* a2, char a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_07::H::draw_tooltip>(this, a0, a1, a2, a3)); }  // 0x4b96f0
auto joan::orig::Messages::post(std::uint32_t a0, std::uint32_t a1, std::int32_t a2, std::int32_t a3, std::uint32_t a4) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_05::H::post>(this, a0, a1, a2, a3, a4)); }  // 0x4b9a90
auto joan::orig::Pool::last_used() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves81::H::pool_last>(this)); }  // 0x4b9c30
auto joan::orig::NewsItem::set_args(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b89_06::H::rec_set>(this, a0, a1, a2, a3, a4, a5)); }  // 0x4b9c80
auto joan::orig::Counter::clamp_to_one() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_02::H::clamp_to_one>(this)); }  // 0x4b9d40
auto joan::orig::NewsList::add_treaty(std::uint32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_02::H::events_add>(this, a0, a1)); }  // 0x4b9e20
auto joan::orig::Spawner::spawn(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b88_06::H::spawn>(this, a0, a1, a2, a3, a4)); }  // 0x4ba270
auto joan::orig::NewsItem::text() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_02::H::dispatch>(this)); }  // 0x4ba320
auto joan::orig::Order::is_active() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_05::H::is_active>(this)); }  // 0x4ba370
auto joan::orig::NewsItem::player_c_name() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b92_03::H::player_c>(this)); }  // 0x4baa30
auto joan::orig::NewsItem::player_10_name() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b92_03::H::player_10>(this)); }  // 0x4baa70
auto joan::orig::NewsItem::unit_pos(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_02::H::record_3d>(this, a0)); }  // 0x4baab0
