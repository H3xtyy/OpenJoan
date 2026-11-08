// game/remapkey.cpp: functions of the original d:\ww\remapkey.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../platform/input.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

void* const kMemory = obj(d::g_mem);

}  // namespace

// ---------------------------------------- from modules/game_b111_20.cpp (fix/uninit)
//
//    - KeyMap::load 0x004cae70 (thiscall, ret 0; returns 0x004cc220's eax): DEFAULTK table ->
//      0x30-byte entries (+0xc, copied to +0x8), KEYDESC table -> 0x28-byte entries (+0x4).
//      UNINIT READ: the 1-char modifier column is parsed with sscanf("%x", &[ebp-0x20]) and the
//      byte stored when the trimmed string is non-empty; a non-hex character leaves [ebp-0x20]
//      unwritten (stale from the previous row, garbage for the first). We start it at 0.
namespace {
namespace game_b111_20 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

// CRT (called directly by the original).
size_t c_strlen(const char* s) { return std::strlen(joan::orig::arg::str(s)); }
i32 c_atoi(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
void c_memset(void* d, i32 v, u32 n) { std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }
void c_memcpy(void* d, const void* s, u32 n) { std::memcpy(joan::orig::arg::mptr(d), joan::orig::arg::ptr(s), joan::orig::arg::u32(n)); }
i32 c_sscanf_x(const char* buf, void* out) {  // sscanf(buf, "%x", out)
    return reinterpret_cast<i32(JOAN_CDECL*)(const char*, const char*, ...)>(fn::code_value(fn::crt_sscanf))(buf, str(d::s_x_9d5780), out);
}

void* const kFiles = obj(d::g_misc);

JOAN_COUNTER(c_keys, "KeyMap::load_4cae70");

struct H {
    // ------------------------------------------------------------ 0x004cae70
    u32 load_keys() {
        c_keys.hit();
        jo::RemapKeys* const self = jo::self<jo::RemapKeys>(this);
        void* const db = joan::orig::arg::as<void*>(joan::orig::as_game_set(obj(d::g_game_set))->open_db_58b180(str(d::s_DEFAULTK)));  // 0x0058b180 "DEFAULTK"
        self->binding_count = joan::orig::arg::as<i32>(joan::orig::as_database(db)->rec_count());                             // 0x00415280
        jo::view<void*>(self->bindings) = joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(static_cast<u32>(self->binding_count) * static_cast<u32>(sizeof(jo::KeyBinding))), str(d::s_d_ww_remapkey_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a02e40)) + 0x11));  // 0x0058e7b0
        jo::view<void*>(self->defaults) = joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(static_cast<u32>(self->binding_count) * static_cast<u32>(sizeof(jo::KeyBinding))), str(d::s_d_ww_remapkey_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a02e40)) + 0x12));
        c_memset(jo::view<void*>(self->defaults), 0, static_cast<u32>(self->binding_count) * static_cast<u32>(sizeof(jo::KeyBinding)));
        char buf[8] = {};
        i32 hexv = 0;  // fix/uninit: [ebp-0x20]
        for (i32 i = 0; i < self->binding_count; ++i) {
            u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_database(db)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            jo::KeyBinding* const e = jo::as_key_binding(jo::view<u8*>(self->defaults) + static_cast<u32>(i) * static_cast<u32>(sizeof(jo::KeyBinding)));
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(e->name), joan::orig::arg::str(rec), 0x20);  // 0x00590210
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(buf), joan::orig::arg::str(rec + 0x20), 4);
            if (c_strlen(buf) != 0) c_sscanf_x(buf, &e->key);
            else e->key = 0xffff;
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(buf), joan::orig::arg::str(rec + 0x24), 4);
            if (c_strlen(buf) != 0) c_sscanf_x(buf, &e->key_alt);
            else e->key_alt = 0xffff;
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(buf), joan::orig::arg::str(rec + 0x28), 1);
            c_sscanf_x(buf, &hexv);
            if (c_strlen(buf) != 0) e->joy_button = static_cast<u8>(hexv & 0xff);
            else e->joy_button = 0xff;
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(buf), joan::orig::arg::str(rec + 0x29), 4);
            if (c_strlen(buf) != 0) c_sscanf_x(buf, &e->key_mouse);
            else e->key_mouse = 0xffff;
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(buf), joan::orig::arg::str(rec + 0x2d), 1);
            if (c_strlen(buf) != 0) e->mouse_button = static_cast<u8>(c_atoi(buf) & 0xff);
            else e->mouse_button = 0xff;
        }
        c_memcpy(jo::view<void*>(self->bindings), jo::view<void*>(self->defaults), static_cast<u32>(self->binding_count) * static_cast<u32>(sizeof(jo::KeyBinding)));
        void* const db2 = joan::orig::arg::as<void*>(joan::orig::as_game_set(obj(d::g_game_set))->open_db_58b180(str(d::s_KEYDESC)));  // 0x0058b180 "KEYDESC"
        self->desc_count = joan::orig::arg::as<i32>(joan::orig::as_database(db2)->rec_count());                                 // 0x00415280
        jo::view<void*>(self->descs) = joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(static_cast<u32>(self->desc_count) * static_cast<u32>(sizeof(jo::KeyDesc))), str(d::s_d_ww_remapkey_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a02e40)) + 0x5b));
        c_memset(jo::view<void*>(self->descs), 0, static_cast<u32>(self->desc_count) * static_cast<u32>(sizeof(jo::KeyDesc)));
        for (i32 i = 0; i < self->desc_count; ++i) {
            u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_database(db2)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            jo::KeyDesc* const e = jo::as_key_desc(jo::view<u8*>(self->descs) + static_cast<u32>(i) * static_cast<u32>(sizeof(jo::KeyDesc)));
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(buf), joan::orig::arg::str(rec), 1);
            jo::view<u8>(e->type) = static_cast<u8>(c_atoi(buf) & 0xff);
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(buf), joan::orig::arg::str(rec + 1), 4);
            c_sscanf_x(buf, &e->code);
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(e->name), joan::orig::arg::str(rec + 5), 0x20);
        }
        joan::orig::as_bindings(self)->load();  // 0x004cc220
        return 0;  // 0x004cc220 returns nothing; the only caller (game.cpp run) discards the result
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: sscanf("%x") target [ebp-0x20] starts at 0 (garbage when the first modifier is not hex).
F("KeyMap::load_4cae70", 0x004cae70, &H::load_keys);
#undef F

}  // namespace game_b111_20
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
//
//      0x004cb310 RemapKeys::free()                   ret 0   (remapkey.cpp)
namespace {
namespace game_b92_03 {

using joan::orig::helpers::line_base;
void mem_free(void* p, const char* file, std::int32_t line) {
    joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

JOAN_COUNTER(c_remap_free, "RemapKeys::free");

struct H {
    // 0x004cb310
    void remap_free() {
        c_remap_free.hit();
        if (jo::view<void*>(jo::self<jo::RemapKeys>(this)->bindings) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::RemapKeys>(this)->bindings), str(d::s_d_ww_remapkey_cpp), line_base(d::g_a02e44) + 3);
            jo::view<void*>(jo::self<jo::RemapKeys>(this)->bindings) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::RemapKeys>(this)->defaults) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::RemapKeys>(this)->defaults), str(d::s_d_ww_remapkey_cpp), line_base(d::g_a02e44) + 8);
            jo::view<void*>(jo::self<jo::RemapKeys>(this)->defaults) = nullptr;
        }
        jo::self<jo::RemapKeys>(this)->binding_count = 0;
        if (jo::view<void*>(jo::self<jo::RemapKeys>(this)->descs) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::RemapKeys>(this)->descs), str(d::s_d_ww_remapkey_cpp), line_base(d::g_a02e44) + 0xf);
            jo::view<void*>(jo::self<jo::RemapKeys>(this)->descs) = nullptr;
        }
        jo::self<jo::RemapKeys>(this)->desc_count = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("RemapKeys::free", 0x004cb310, &H::remap_free);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b95_03.cpp (game/b95_03)
//
//      0x004cae40 X::forward_4cb310        (thiscall, ret 0)
namespace {
namespace game_b95_03 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x004cae40
    u32 forward_4cb310() { joan::orig::as_bindings(this)->free(); return 0; }  // 0x004cb310 (returns nothing; the only caller is an atexit stub)
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("X::forward_4cb310", 0x004cae40, &H::forward_4cb310);
#undef B95

}  // namespace game_b95_03
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
//
//      0x004cc220 RemapKeys::load()                   ret 0
namespace {
namespace game_b92_03_2 {

using joan::hook::global;

JOAN_COUNTER(c_remap_load, "RemapKeys::load");

struct H {
    // 0x004cc220
    void remap_load() {
        c_remap_load.hit();
        if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_a5806c)) != 0) {
            for (std::int32_t i = 1; i <= jo::self<jo::RemapKeys>(this)->binding_count; ++i) {
                const std::int32_t o = (i - 1) * static_cast<std::int32_t>(sizeof(jo::KeyBinding));
                // the saved bindings: KeyBinding records at 0xa5806d (joy_button = 0xa5808f)
                const jo::KeyBinding* const src = jo::as_key_binding(&img::global<std::uint8_t>(d::g_a5808f + o) - offsetof(jo::KeyBinding, joy_button));
                jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->bindings) + o)->joy_button = src->joy_button;
                jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->bindings) + o)->mouse_button = src->mouse_button;
                jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->bindings) + o)->key = src->key;
                jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->bindings) + o)->key_alt = src->key_alt;
                jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->bindings) + o)->key_mouse = src->key_mouse;
            }
            return;
        }
        std::memcpy(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::RemapKeys>(this)->bindings)), joan::orig::arg::ptr(jo::view<void*>(jo::self<jo::RemapKeys>(this)->defaults)), joan::orig::arg::u32(static_cast<std::uint32_t>(jo::self<jo::RemapKeys>(this)->binding_count * static_cast<std::int32_t>(sizeof(jo::KeyBinding)))));  // memcpy
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("RemapKeys::load", 0x004cc220, &H::remap_load);
#undef B

}  // namespace game_b92_03_2
}  // namespace

// ---------------------------------------- from modules/game_b97_03.cpp (game/b97_03)
namespace {
namespace game_b97_03 {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ small ones
    void* zero_14() {  // 0x004cae10
        memset_(this, 0, sizeof(jo::RemapKeys));  // 0x14 on x86 (Bindings = RemapKeys: count + 3 pointers + count)
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("Struct14::zero", 0x004cae10, &H::zero_14);
#undef B97

}  // namespace game_b97_03
}  // namespace

// ---------------------------------------- from modules/game_b92_03_3.cpp (game/b92_03)
namespace {
namespace game_b92_03_3 {

JOAN_COUNTER(c_remap_restore, "RemapKeys::restore");

struct H {
    // 0x004cb410
    void remap_restore(std::int32_t what) {
        c_remap_restore.hit();
        for (std::int32_t i = 1; i <= jo::self<jo::RemapKeys>(this)->binding_count; ++i) {
            const std::int32_t o = (i - 1) * static_cast<std::int32_t>(sizeof(jo::KeyBinding));
            switch (what) {
            case 0:
                jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->bindings) + o)->key = jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->defaults) + o)->key;
                break;
            case 1:
                jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->bindings) + o)->key_alt = jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->defaults) + o)->key_alt;
                jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->bindings) + o)->joy_button = jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->defaults) + o)->joy_button;
                break;
            case 3:
                jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->bindings) + o)->key_mouse = jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->defaults) + o)->key_mouse;
                jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->bindings) + o)->mouse_button = jo::as_key_binding(jo::view<std::uint8_t*>(jo::self<jo::RemapKeys>(this)->defaults) + o)->mouse_button;
                break;
            default:
                break;
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("RemapKeys::restore", 0x004cb410, &H::remap_restore);
#undef B

}  // namespace game_b92_03_3
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
namespace {
namespace game_b98_03 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_race_test, "RaceRes::test");

struct H {
    // ------------------------------------------------------------------------ 0x004cb580
    u8* race_record(i32 i) { return jo::view<u8*>(jo::self<jo::RemapKeys>(this)->bindings) + static_cast<u32>(i - 1) * 0x30u; }

    // ------------------------------------------------------------------------ 0x004cb820
    u32 race_test(i32 kind, i32 i) {
        c_race_test.hit();
        const jo::KeyBinding* const rec = jo::as_key_binding(jo::view<u8*>(jo::self<jo::RemapKeys>(this)->bindings) + static_cast<u32>(i - 1) * 0x30u);
        switch (kind) {
            case 0:
                return joan::orig::arg::as<u32>(joan::orig::as_bindings(this)->key_is_down(joan::orig::arg::u32(rec->key)));  // 0x004cc570
            case 1: {
                const u32 c = rec->joy_button;
                if (c != 0 && c != 0xff && *global<u32*>(d::g_keys_a51806) != 0 &&
                    static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_ai(obj(d::g_keys_a51806))->is_ready(joan::orig::arg::i32(static_cast<i32>(rec->joy_button) - 1)))) != 0 &&
                    static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_ai(obj(d::g_a518ea))->is_ready(joan::orig::arg::i32(static_cast<i32>(rec->joy_button) - 1)))) == 0)
                    return 1;  // 0x00597ed0 x2
                if (rec->key_alt != 0) return joan::orig::arg::as<u32>(joan::orig::as_bindings(this)->key_is_down(joan::orig::arg::u32(rec->key_alt)));  // 0x004cc570
                return 0;
            }
            case 3: {
                if (rec->key_mouse != 0 && joan::orig::arg::as<u32>(joan::orig::as_bindings(this)->key_is_down(joan::orig::arg::u32(rec->key_mouse))) != 0)  // 0x004cc570
                    return 1;
                const u32 c = rec->mouse_button;
                if (c != 0 && c != 0xff)
                    return joan::orig::arg::as<u32>(joan::orig::as_parser(obj(d::g_mouse))->value(joan::orig::arg::i32(static_cast<i32>(rec->mouse_button) - 1)));  // 0x00594700
                return 0;
            }
            default:
                return 0;
        }
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("RaceRes::record", 0x004cb580, &H::race_record);
B98("RaceRes::test", 0x004cb820, &H::race_test);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b95_03_2.cpp (game/b95_03)
namespace {
namespace game_b95_03_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_bind_state, "Bindings::state");

struct H {
    // ------------------------------------------------------------------------ 0x004cb6d0
    i32 bind_state(i32 mode, u32 idx) {
        c_bind_state.hit();
        const jo::KeyBinding* const e = jo::as_key_binding(jo::view<u8*>(jo::self<jo::RemapKeys>(this)->bindings) + (idx - 1u) * 0x30u);
        if (mode == 0) return joan::orig::arg::as<i32>(joan::orig::as_bindings(this)->key_state(joan::orig::arg::u32(e->key)));  // 0x004cc3d0
        if (mode == 1) {
            const u32 joy = e->joy_button;
            if (joy != 0 && joy != 0xff && *global<u32*>(d::g_keys_a51806) != 0) {
                return static_cast<i8>(joan::orig::arg::as<char>(joan::orig::as_ai(obj(d::g_keys_a51806))->is_ready(joan::orig::arg::i32(joy - 1u))));  // 0x00597ed0
            }
            if (e->key_alt != 0) return joan::orig::arg::as<i32>(joan::orig::as_bindings(this)->key_state(joan::orig::arg::u32(e->key_alt)));  // 0x004cc3d0
            return 0;
        }
        if (mode == 3) {
            if (e->key_mouse != 0 && joan::orig::arg::as<i32>(joan::orig::as_bindings(this)->key_state(joan::orig::arg::u32(e->key_mouse))) != 0) return 1;
            const u32 b = e->mouse_button;
            if (b != 0 && b != 0xff) return global<i8>(d::g_a51da4 + b);
            return 0;
        }
        return 0;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("Bindings::state", 0x004cb6d0, &H::bind_state);
#undef B95

}  // namespace game_b95_03_2
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
namespace {
namespace game_b100_05 {

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

JOAN_COUNTER(c_find, "Table::find");

struct H {
    // ------------------------------------------------------------------ 0x004cb9b0
    u8* find(i32 key, i32 id) {
        c_find.hit();
        for (i32 i = 0; i < jo::self<jo::RemapKeys>(this)->desc_count; ++i) {
            jo::KeyDesc* const rec = jo::as_key_desc(jo::view<u8*>(jo::self<jo::RemapKeys>(this)->descs) + i * 0x28);
            if (static_cast<i32>(rec->type) == key && jo::view<i32>(rec->code) == id) return reinterpret_cast<u8*>(rec->name);
        }
        return nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Table::find", 0x004cb9b0, &H::find);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b106_02.cpp (game/b106_02)
namespace {
namespace game_b106_02 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_assign, "SlotTable::assign_4cba40");

// ---------------------------------------------------------------------------------------
// 0x004cba40: 55 slots (1..0x37) of 0x30 bytes at this+8 (defaults at this+0xc), a key list
// of *this entries (0x28 bytes) at this+4.
using joan::orig::helpers::add32;
constexpr i32 kBinding = static_cast<i32>(sizeof(jo::KeyBinding));  // x86: 0x30
// Slot n (1-based) of the bindings (this+8) / of the defaults (this+0xc).
jo::KeyBinding* binding(void* self, i32 n) { return jo::as_key_binding(jo::view<u8*>(jo::as_remap_keys(self)->bindings) + (n - 1) * kBinding); }
jo::KeyBinding* default_binding(void* self, i32 n) { return jo::as_key_binding(jo::view<u8*>(jo::as_remap_keys(self)->defaults) + (n - 1) * kBinding); }

// mode 1 / 3: dword field `dw` and byte field `by` of the slots.
void assign_pair(void* self, i32 k, jo::KeyBinding* e, i32 type, i32 value, i32 by_type, u32 jo::KeyBinding::*dw, u8 jo::KeyBinding::*by) {
    i32 v = 0;
    u8 b = 0;
    const u32 old_v = e->*dw;
    const u8 old_b = e->*by;
    if (type == 0) v = value;
    else if (type == by_type) b = static_cast<u8>(static_cast<u32>(value) + 1u);
    i32 found = 0;
    i32 ok = 1;
    jo::view<i32>(e->*dw) = v;
    e->*by = b;
    for (i32 m = 1; m <= 0x37; ++m) {
        if (m == k) continue;
        if (v != 0 && jo::view<i32>(binding(self, m)->*dw) == v) {
            found = m;
        } else if (static_cast<std::int8_t>(b) != 0 &&
                   static_cast<i32>(binding(self, m)->*by) == static_cast<i32>(static_cast<std::int8_t>(b))) {
            found = m;
        }
    }
    if (found == 0) return;
    for (i32 m = 1; m <= 0x37; ++m) {
        if (m == found) continue;
        if (binding(self, m)->*dw != 0 && binding(self, m)->*dw == default_binding(self, found)->*dw) {
            ok = 0;
        } else if (binding(self, m)->*by != 0 &&
                   binding(self, m)->*by == default_binding(self, found)->*by) {
            ok = 0;
        }
    }
    if (ok != 0) {
        binding(self, found)->*dw = default_binding(self, found)->*dw;
        binding(self, found)->*by = default_binding(self, found)->*by;
    } else {
        binding(self, found)->*dw = old_v;
        binding(self, found)->*by = old_b;
    }
}

struct H {
    bool assign_4cba40(i32 mode, i32 k, i32 type, i32 value) {  // 0x004cba40
        c_assign.hit();
        jo::KeyBinding* const e = jo::as_key_binding(jo::view<u8*>(jo::self<jo::RemapKeys>(this)->bindings) + (k - 1) * kBinding);
        if (k == 0x22) return false;
        bool found = false;
        for (i32 i = 0; i < jo::self<jo::RemapKeys>(this)->desc_count; ++i) {
            jo::KeyDesc* const r = jo::as_key_desc(jo::view<u8*>(jo::self<jo::RemapKeys>(this)->descs) + i * static_cast<i32>(sizeof(jo::KeyDesc)));
            if (static_cast<i32>(static_cast<std::int8_t>(jo::view<u8>(r->type))) != type) continue;
            if (type == 0 && jo::view<i32>(r->code) == value) found = true;
            else if ((type == 1 || type == 2) && jo::view<i32>(r->code) == add32(value, 1)) found = true;
        }
        if (!found) return false;
        if (mode == 0) {
            if (type == 0) {
                const u32 old = e->key;
                i32 j = 0;
                i32 ok = 1;
                jo::view<i32>(e->key) = value;
                for (i32 m = 1; m <= 0x37; ++m) {
                    if (m != k && value != 0 && jo::view<i32>(binding(this, m)->key) == value) j = m;
                }
                if (j != 0) {
                    for (i32 m = 1; m <= 0x37; ++m) {
                        if (m != k && binding(this, m)->key != 0 &&
                            binding(this, m)->key == default_binding(this, j)->key) {
                            ok = 0;
                        }
                    }
                    if (ok != 0) binding(this, j)->key = default_binding(this, j)->key;
                    else binding(this, j)->key = old;
                }
            }
        } else if (mode == 1) {
            assign_pair(this, k, e, type, value, 1, &jo::KeyBinding::key_alt, &jo::KeyBinding::joy_button);
        } else if (mode == 3) {
            assign_pair(this, k, e, type, value, 2, &jo::KeyBinding::key_mouse, &jo::KeyBinding::mouse_button);
        }
        return true;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_02", name, addr, fn)
B("SlotTable::assign_4cba40", 0x004cba40, &H::assign_4cba40);
#undef B

}  // namespace game_b106_02
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using joan::hook::global;

using joan::img::obj;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memcpy_;  // memcpy

struct H {
    void* save_snapshot() {  // 0x004cc380
        global<u8>(d::g_a5806c) = 1;
        return memcpy_(obj(d::g_a5806d), jo::view<void*>(jo::self<jo::RemapKeys>(this)->bindings), static_cast<u32>(jo::self<jo::RemapKeys>(this)->binding_count * 0x30));
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("X::save_snapshot", 0x004cc380, &H::save_snapshot);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b94_03.cpp (game/b94_03)
namespace {
namespace game_b94_03 {

using joan::hook::global;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

JOAN_COUNTER(c_key_state, "Keys::state");

struct H {
    // ------------------------------------------------------------------------ 0x004cc3d0
    u32 key_state(u32 key) {
        c_key_state.hit();
        switch (key) {
            case 0x00:
            case 0x3a:
            case 0x45:
            case 0x46:
            case 0xffff: return 0;
            case 0x1c:
            case 0x9c:
                return (global<u8>(d::g_a5179c) != 0 || global<u8>(d::g_a5171c) != 0) ? 1u : 0u;
            case 0x1d:
            case 0x9d: return static_cast<u32>(global<u16>(d::g_key_modifiers)) & 0xcu;
            case 0x2a:
            case 0x36: return static_cast<u32>(global<u16>(d::g_key_modifiers)) & 0x3u;
            case 0x38:
            case 0xb8: return static_cast<u32>(global<u16>(d::g_key_modifiers)) & 0x30u;
            default: return global<u8>(d::g_a51700 + key);
        }
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_03", name, addr, fn)
B94("Keys::state", 0x004cc3d0, &H::key_state);
#undef B94

}  // namespace game_b94_03
}  // namespace

// ---------------------------------------- from modules/game_b97_03_2.cpp (game/b97_03)
namespace {
namespace game_b97_03_2 {

using joan::hook::global;

using u32 = std::uint32_t;

using u16 = std::uint16_t;

JOAN_COUNTER(c_key_down, "Keys::is_down");

struct H {
    // ------------------------------------------------------------------------ 0x004cc570
    static u32 mod_bits(u32 mask) {
        if ((static_cast<u32>(global<u16>(d::g_a51802)) & mask) == 0 &&
            (static_cast<u32>(global<u16>(d::g_key_modifiers)) & mask) != 0)
            return 1;
        return 0;
    }
    u32 key_down(u32 key) {
        c_key_down.hit();
        switch (key) {
            case 0x00:
            case 0xffff:
            case 0x3a:
            case 0x45:
            case 0x46: return 0;
            case 0x1c:
            case 0x9c: {
                const u32 k = global<u32>(d::g_last_key_scancode);
                return (k == 0x9c || k == 0x1c) ? 1u : 0u;
            }
            case 0x1d:
            case 0x9d: return mod_bits(0xc);
            case 0x2a:
            case 0x36: return mod_bits(0x3);
            case 0x38:
            case 0xb8: return mod_bits(0x30);
            default: return global<u32>(d::g_last_key_scancode) == key ? 1u : 0u;
        }
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("Keys::is_down", 0x004cc570, &H::key_down);
#undef B97

}  // namespace game_b97_03_2
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
namespace {
namespace game_b96_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ 0x004cc7c0
    void* zero_289() {
        memset_(this, 0, sizeof(jo::Scenario));  // 0x289 on x86
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("Struct289::zero", 0x004cc7c0, &H::zero_289);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b101_06.cpp (game/b101_06)
namespace {
namespace game_b101_06 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
JOAN_COUNTER(c_lookup, "Table::lookup");

struct H {
    // ------------------------------------------------------------------------ 0x004cb5b0
    const char* table_lookup(i32 mode, i32 index) {  // eax: the key's name (0x004cb9b0's result) or 0
        c_lookup.hit();
        const jo::KeyBinding* const row = jo::as_key_binding(jo::view<u8*>(jo::self<jo::RemapKeys>(this)->bindings) + (static_cast<u32>(index) - 1u) * 0x30u);
        auto find = [this](i32 key, u32 id) { return joan::orig::arg::as<const char*>(joan::orig::as_table(this)->find_4cb9b0(joan::orig::arg::i32(key), joan::orig::arg::i32(id))); };  // 0x004cb9b0
        switch (mode) {
            case 0: return find(0, row->key);
            case 1: {
                const u32 b = row->joy_button;
                if (b != 0 && b != 0xff) {
                    // The connected pad's own button names (Xbox / PlayStation / Nintendo) for KEYDESC's "JOYPADn".
                    if (const char* name = joan::platform::input::pad_button_name(static_cast<int>(b))) return name;
                    return find(1, b);
                }
                return find(0, row->key_alt);
            }
            case 3: {
                const u32 d = row->key_mouse;
                if (d != 0 && d != 0xffff) return find(0, d);
                return find(2, row->mouse_button);
            }
            default: return nullptr;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_06", name, addr, fn)
B("Table::lookup", 0x004cb5b0, &H::table_lookup);
#undef B

}  // namespace game_b101_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Bindings::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_03::H::zero_14>(this)); }  // 0x4cae10
auto joan::orig::Bindings::free() -> void { joan::bind::member<&::game_b92_03::H::remap_free>(this); }  // 0x4cb310
auto joan::orig::Bindings::restore(std::int32_t a0) -> void { joan::bind::member<&::game_b92_03_3::H::remap_restore>(this, a0); }  // 0x4cb410
auto joan::orig::RaceRes::record(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_03::H::race_record>(this, a0)); }  // 0x4cb580
auto joan::orig::Table::lookup(std::int32_t a0, std::int32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b101_06::H::table_lookup>(this, a0, a1)); }  // 0x4cb5b0
auto joan::orig::Bindings::state(std::int32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_03_2::H::bind_state>(this, a0, a1)); }  // 0x4cb6d0
auto joan::orig::RaceRes::test(std::int32_t a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_03::H::race_test>(this, a0, a1)); }  // 0x4cb820
auto joan::orig::Table::find_4cb9b0(std::int32_t a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_05::H::find>(this, a0, a1)); }  // 0x4cb9b0
auto joan::orig::Bindings::assign(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b106_02::H::assign_4cba40>(this, a0, a1, a2, a3)); }  // 0x4cba40
auto joan::orig::Bindings::load() -> void { joan::bind::member<&::game_b92_03_2::H::remap_load>(this); }  // 0x4cc220
auto joan::orig::Bindings::save_snapshot() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_03::H::save_snapshot>(this)); }  // 0x4cc380
auto joan::orig::Bindings::key_state(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_03::H::key_state>(this, a0)); }  // 0x4cc3d0
auto joan::orig::Bindings::key_is_down(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_03_2::H::key_down>(this, a0)); }  // 0x4cc570
auto joan::orig::Scenario::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_03::H::zero_289>(this)); }  // 0x4cc7c0
