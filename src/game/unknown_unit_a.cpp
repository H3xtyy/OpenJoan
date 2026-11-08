// game/unknown_unit_a.cpp: functions whose original source file is unknown (formerly game/_unplaced/unit_a.cpp;
// re/attribution.csv: between ww/unit.cpp and ww/unitres.cpp). They move to their original file when evidence names it
// (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
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

namespace {
// v3d "M" results (length, length_sq, dot_product): the replacements (src/lib/v3d/v3d87.cpp) return double. On x86
// the caller reads st0 as the float it always used (same code as before); x64 returns the double in xmm0, so it is
// read as a double there.
template <class... A>
float v3d_float(joan::img::Addr slot, A... args) {
#if UINTPTR_MAX == 0xffffffffu
    return joan::call::import_call<float>(slot, args...);
#else
    return static_cast<float>(joan::call::import_call<double>(slot, args...));
#endif
}
}  // namespace

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::import_call;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u16 = std::uint16_t;

void* const kFx = obj(d::g_sfx_player);

using u32 = std::uint32_t;

using u8 = std::uint8_t;

using joan::hook::x87::Fpu;

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);


}  // namespace

// ---------------------------------------- from modules/game_b103_05.cpp (game/b103_05)
namespace {
namespace game_b103_05 {

using i8 = std::int8_t;

void* const kObjects = obj(d::g_items);

// 12-byte vector passed by value (raw bits, no float conversions).
struct V3 {
    float a, b, c;
};
static_assert(sizeof(V3) == 12, "V3");
V3 v3_at(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof(v));
    return v;
}

void* const kModels = obj(d::g_physic_sys);

JOAN_COUNTER(c_missile, "Missile::update");

void JOAN_CDECL init_4f9220() { joan::orig::init_a3cb78(); }
void JOAN_CDECL init_4fb2f0() { joan::orig::init_a3cb7c(); }

struct H {
    // ------------------------------------------------------------------------ 0x004fcc00
    // Steps 0x004fcc4a / 0x004fcde2 (identical code for states 1 and 2). Returns false when the
    // function returned right away (after 0x004e6fb0).
    bool missile_track() {
        if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->object_id)) != 0) return true;  // 0x00425d50
        void* const o = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->object_id));  // 0x00425d20
        if (jo::view<i8>(jo::as_map_object(o)->active) == 0) {
            joan::orig::as_missile(this)->expire();  // 0x004e6fb0
            return false;
        }
        if (jo::as_map_object(o)->actor == nullptr || jo::view<u32>(jo::as_map_object(o)->actor->record) == 0) return true;
        void* const m = joan::orig::as_players(kModels)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_map_object(o)->actor->record)));  // 0x006af310
        if (jo::view<i16>(jo::as_shot(m)->state.on_ground) != 0 || jo::view<i16>(jo::as_shot(m)->hit) != 0) {
            joan::orig::as_trail(jo::view<void*>(jo::self<jo::Missile>(this)->trail))->release_hold();  // 0x00687850
        } else if (jo::view<i16>(jo::as_shot(m)->state.move_flag) != 0) {
            float n[3];
            import_call<void*>(d::iat_v3d_87_vector_normalize_2, static_cast<void*>(n), static_cast<void*>(&jo::as_shot(m)->state));  // vector_normalize
            const float yaw = joan::orig::arg::as<float>(joan::orig::libc::atan2_f(joan::orig::arg::f32(n[1]), joan::orig::arg::f32(n[0])));  // 0x00421020 atan2
            joan::orig::as_map_object(o)->set_yaw(joan::orig::arg::bits(yaw));                           // 0x00499fd0
            const float pitch = joan::orig::arg::as<float>(joan::orig::libc::asin_f(joan::orig::arg::f32(-n[2])));     // 0x00425610
            joan::orig::as_unit(o)->set_66(joan::orig::arg::f32(pitch));                         // 0x0049ab00
        }
        if (jo::self<jo::Missile>(this)->sound_handle != 0) {
            u32 tmp[4];
            const void* const r = joan::orig::arg::as<void*>(joan::orig::vec_scale(static_cast<void*>(tmp), static_cast<void*>(&jo::as_shot(m)->state.pos_x), joan::orig::arg::f32(0x42386666u)));  // 0x0060cd00, 46.1f
            V3 pos = v3_at(r);
            void** const slot = joan::orig::arg::as<void**>(joan::orig::as_sound_list(kFx)->entry(joan::orig::arg::i16(static_cast<u32>(jo::view<u16[2]>(jo::self<jo::Missile>(this)->sound_handle)[0]))));  // 0x004cf690
            joan::orig::as_sound3d(*slot)->set_position(joan::orig::arg::mptr(static_cast<void*>(&pos)));                         // 0x0059fb40
        } else if (global<i8>(d::g_flag2) != 0) {
            jo::self<jo::Missile>(this)->sound_handle = joan::orig::arg::as<u32>(joan::orig::as_sound_list(kFx)->play_effect(str(d::s_ARROW), v3_at(jo::as_map_object(o)->pos)));  // 0x00494810 "ARROW"
        }
        return true;
    }

    void hit_effect(u32 kind, u32 sub, const V3& at_pos) {
        joan::orig::as_triggers(obj(d::g_a5ccd0))->add(joan::orig::arg::u32(kind), joan::orig::arg::u32(sub), joan::orig::arg::i16(0u), 1u, joan::orig::arg::i16(static_cast<u32>(jo::view<u16[2]>(jo::self<jo::Missile>(this)->shooter_unit_id)[0])), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), at_pos, 0x43160000u, static_cast<std::int8_t>(joan::orig::arg::i32(0u)), joan::orig::arg::u32(global<u32>(d::g_now)), joan::orig::arg::u32(global<u32>(d::g_now)), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), 0u, joan::orig::arg::i16(0u));  // 0x004eef00
    }

    void missile_update() {
        c_missile.hit();
        if (jo::self<jo::Missile>(this)->stopped != 0) return;
        bool flag = false;
        const u32 state = jo::self<jo::Missile>(this)->state;
        if (state == 1 || state == 2) {
            if (!missile_track()) return;
        }
        if (jo::self<jo::Missile>(this)->trail != nullptr) joan::orig::as_trail(jo::view<void*>(jo::self<jo::Missile>(this)->trail))->attach_update();  // 0x00687a60
        void* const m = joan::orig::as_players(kModels)->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->model_id));  // 0x006af310
        if (jo::self<jo::Missile>(this)->hit_done == 0 && jo::self<jo::Missile>(this)->state == 2 && jo::view<i16>(jo::as_shot(m)->state.on_ground) != 0) {
            u32 buf[8] = {};
            joan::orig::as_body(m)->scaled_position(joan::orig::arg::mptr(static_cast<void*>(buf)));  // 0x006b1030
            const V3 pos = v3_at(buf);
            hit_effect(0x2000, 1, pos);
            hit_effect(0x2000, 2, pos);
            jo::view<u16>(jo::self<jo::Missile>(this)->hit_done) = 1;
            u32 tmp[8];
            joan::orig::as_marker(this)->position(joan::orig::arg::mptr(static_cast<void*>(tmp)), joan::orig::arg::mptr(0));  // 0x004e5d90
            const i32 r = joan::orig::as_misc(obj(d::g_misc))->random(3);  // 0x00590540
            if (r == 0)
                joan::orig::as_sound_list(kFx)->play_effect(str(d::s_EXPLOSIVEHIT1), pos);  // 0x00494810 "EXPLOSIVEHIT1"
            else if (r == 1)
                joan::orig::as_sound_list(kFx)->play_effect(str(d::s_EXPLOSIVEHIT2), pos);  // "EXPLOSIVEHIT2"
            else if (r == 2)
                joan::orig::as_sound_list(kFx)->play_effect(str(d::s_EXPLOSIVEHIT3), pos);  // "EXPLOSIVEHIT3"
            flag = true;
        }
        if (jo::view<i16>(jo::as_shot(m)->state.on_ground) != 0) flag = true;
        if (jo::self<jo::Missile>(this)->hit_done == 0 && (jo::self<jo::Missile>(this)->state == 3 || jo::self<jo::Missile>(this)->state == 4) && jo::view<i16>(jo::as_shot(m)->state.on_ground) != 0) {
            u32 buf[8] = {};
            joan::orig::as_body(m)->scaled_position(joan::orig::arg::mptr(static_cast<void*>(buf)));  // 0x006b1030
            const V3 pos = v3_at(buf);
            hit_effect(0x40, 1, pos);
            jo::view<u16>(jo::self<jo::Missile>(this)->hit_done) = 1;
            u32 tmp[8];
            joan::orig::as_marker(this)->position(joan::orig::arg::mptr(static_cast<void*>(tmp)), joan::orig::arg::mptr(0));  // 0x004e5d90
            joan::orig::as_sound_list(kFx)->play_effect(str(d::s_CANNONHL), pos);           // 0x00494810 "CANNONHL"
            flag = true;
        }
        if (jo::self<jo::Missile>(this)->expired != 0 || jo::view<i16>(jo::as_shot(m)->hit) != 0) {
            if (jo::self<jo::Missile>(this)->trail != nullptr) {
                joan::orig::as_trail(jo::view<void*>(jo::self<jo::Missile>(this)->trail))->release();  // 0x00687410
                void* const p = jo::view<void*>(jo::self<jo::Missile>(this)->trail);
                if (p != nullptr) joan::orig::as_trail(p)->destroy(1u);  // 0x00420790 scalar deleting dtor
                jo::self<jo::Missile>(this)->trail = nullptr;
            }
            if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->object_id)) == 0) {  // 0x00425d50
                joan::orig::as_units(kObjects)->remove_49f370(joan::orig::arg::i32(jo::self<jo::Missile>(this)->object_id));         // 0x0049f370
                jo::self<jo::Missile>(this)->object_id = 0;
            }
            flag = true;
        }
        if (flag) joan::orig::as_missile(this)->expire();  // 0x004e6fb0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_05", name, addr, fn)
B("init_4f9220", 0x004f9220, &init_4f9220);
B("init_4fb2f0", 0x004fb2f0, &init_4fb2f0);
B("Missile::update", 0x004fcc00, &H::missile_update);
#undef B

}  // namespace game_b103_05
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

void JOAN_CDECL init_a3cb78() {  // 0x004f9250
    global<float>(d::g_a3cb78) = global<float>(d::g_pi_a3cb6c) / global<float>(d::flt_3);  // 3.0f
}

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("init_a3cb78", 0x004f9250, &init_a3cb78);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b111_15.cpp (fix/uninit)
namespace {
namespace game_b111_15 {

JOAN_COUNTER(c_settlement, "Settlement::load");

// CRT (called at their original addresses, as the original does)
u32 strlen_(const void* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
// strncpy without a terminator into the fixed-size field, as the original does
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
void strncpy_(void* d, const void* s, u32 n) { std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
void strcpy_(void* d, const void* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
i32 atoi_(const void* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

// ------------------------------------------------------------------ 0x004f9330 frame
// Byte image of the original's frame: at(x) is [ebp - x] (0x244 bytes of locals).
struct Frame {
    static constexpr i32 kSize = 0x244;
    alignas(4) u8 b[kSize];
    Frame() { std::memset(b, 0, sizeof b); }  // fix/uninit: the original's garbage -> 0
    u8* at(i32 x) { return b + (kSize - x); }
    bool inside(i32 x, i32 width) const { return x >= width && x <= kSize; }
    i32 rd32(i32 x) {
        if (!inside(x, 4)) return 0;
        i32 v = 0;
        std::memcpy(&v, at(x), 4);
        return v;
    }
    void wr32(i32 x, i32 v) {
        if (inside(x, 4)) std::memcpy(at(x), &v, 4);
    }
    u8 rd8(i32 x) { return inside(x, 1) ? *at(x) : u8{0}; }
    void wr8(i32 x, u8 v) {
        if (inside(x, 1)) *at(x) = v;
    }
};

// Frame offsets (ebp - x)
constexpr i32 kLine = 0x108;     // char[0x100]
constexpr i32 kName = 0x12c;     // char[0x20]
constexpr i32 kInt130 = 0x130;   // four atoi values: 0x130, 0x134, 0x138, 0x13c
constexpr i32 kOwner = 0x140;
constexpr i32 kSettle = 0x144;   // byte
constexpr i32 kKind = 0x148;     // byte
constexpr i32 kCount = 0x14c;
constexpr i32 kScr = 0x158;      // char[0xc]
constexpr i32 kDisable = 0x15c;  // byte
constexpr i32 kShape = 0x160;    // byte[4]
// per-shape int[4] arrays (record offset = 0x4c + 0x10 * n for kArr[n])
constexpr i32 kArr[9] = {0x170, 0x180, 0x190, 0x1a0, 0x1b0, 0x1c0, 0x1d0, 0x1e0, 0x1f0};

struct Loader {
    void* reader;
    Frame fr;
    const char* tok = nullptr;  // [ebp-0x8]

    // 0x00589c70; the token pointer is also kept in the frame image ([ebp-0x8]) because the
    // unbounded shape-index reads (i >= 0x5a) can reach that slot.
    void next_token() {
        tok = joan::orig::arg::as<const char*>(joan::orig::as_file_txt(reader)->get_token(1));
        fr.wr32(8, static_cast<i32>(reinterpret_cast<std::uintptr_t>(tok)));  // x64: the low 32 bits, the value of the x86 slot; read only by out-of-range shape indices (corrupt data)
    }
    // read_line(buf, 0xff) + buf[strlen(buf) - 1] = 0
    void line() {
        joan::orig::as_file_txt(reader)->read_line(static_cast<char*>(static_cast<void*>(fr.at(kLine))), 0xff);  // 0x00589f10
        const u32 n = strlen_(fr.at(kLine));
        fr.wr8(kLine + 1 - static_cast<i32>(n), 0);
    }
    bool line_is(img::Addr s) { return stricmp_(fr.at(kLine), str(s)) == 0; }
    // token, line, atoi
    i32 int_field() {
        next_token();
        line();
        return atoi_(fr.at(kLine));
    }
};

struct H {
    // ------------------------------------------------------------------ 0x004f9330 (fix)
    i32 settlement_load(void* reader) {
        c_settlement.hit();
        Loader L{reader, Frame{}};
        Frame& fr = L.fr;
        fr.wr32(4, static_cast<i32>(reinterpret_cast<std::uintptr_t>(this)));  // [ebp-0x4] = this (x64: low 32 bits, as above)

        L.next_token();
        L.next_token();
        L.line();
        strncpy_(fr.at(kName), fr.at(kLine), 0x20);
        fr.wr32(kInt130, L.int_field());
        fr.wr32(kInt130 + 4, L.int_field());
        fr.wr32(kInt130 + 8, L.int_field());
        fr.wr32(kInt130 + 0xc, L.int_field());
        L.next_token();
        L.line();
        const i32 side = joan::orig::arg::as<i32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(joan::orig::arg::str(static_cast<void*>(fr.at(kLine)))));  // 0x004b91a0
        fr.wr32(kOwner, joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(side))));                          // 0x004b67e0
        if (fr.rd32(kOwner) == 0 && L.line_is(d::s_Neutral)) fr.wr32(kOwner, -1);  // "Neutral"
        fr.wr8(kKind, 0);
        fr.wr8(kDisable, 0);
        L.next_token();
        if (stricmp_(L.tok, str(d::s_DisableBeingAttacked)) == 0) {  // "DisableBeingAttacked"
            L.line();
            fr.wr8(kDisable, static_cast<u8>(atoi_(fr.at(kLine))));
            L.next_token();
        }
        if (stricmp_(L.tok, str(d::s_ScrName)) == 0) {  // "ScrName"
            L.line();
            strcpy_(fr.at(kScr), fr.at(kLine));
            L.next_token();
        }
        if (stricmp_(L.tok, str(d::s_Type)) == 0) {  // "Type"
            L.line();
            if (L.line_is(d::s_camp)) fr.wr8(kKind, 0);        // "camp"
            else if (L.line_is(d::s_town)) fr.wr8(kKind, 1);   // "town"
            else if (L.line_is(d::s_city)) fr.wr8(kKind, 2);   // "city"
            else if (L.line_is(d::s_castle)) fr.wr8(kKind, 3);   // "castle"
            else if (L.line_is(d::s_church)) fr.wr8(kKind, 4);   // "church"
            else if (L.line_is(d::s_forest)) fr.wr8(kKind, 5);   // "forest"
            L.next_token();
        }
        L.line();
        if (L.line_is(d::s_settlement)) fr.wr8(kSettle, 0);        // "settlement"
        else if (L.line_is(d::s_small_camp)) fr.wr8(kSettle, 1);   // "small_camp"
        else if (L.line_is(d::s_large_camp)) fr.wr8(kSettle, 2);   // "large_camp"
        else if (L.line_is(d::s_castle)) fr.wr8(kSettle, 3);   // "castle"
        else if (L.line_is(d::s_paris)) fr.wr8(kSettle, 4);   // "paris"
        fr.wr32(kCount, L.int_field());
        for (i32 i = 0; i < fr.rd32(kCount); ++i) {  // count re-read: array writes can reach it
            L.next_token();
            L.line();
            if (L.line_is(d::s_circle)) fr.wr8(kShape - i, 1);        // "circle"
            else if (L.line_is(d::s_rectangle)) fr.wr8(kShape - i, 0);   // "rectangle"
            else if (L.line_is(d::s_quadrilateral)) fr.wr8(kShape - i, 2);   // "quadrilateral"
            const u8 shape = fr.rd8(kShape - i);
            if (shape == 0) {  // rectangle
                for (i32 n = 0; n < 4; ++n) {
                    const i32 v = L.int_field();
                    fr.wr32(kArr[n] - 4 * i, v);
                }
            } else if (shape == 1) {  // circle
                i32 v = L.int_field();
                fr.wr32(kArr[0] - 4 * i, v);
                v = L.int_field();
                fr.wr32(kArr[1] - 4 * i, v);
                v = L.int_field();
                fr.wr32(kArr[8] - 4 * i, v);
            } else if (shape == 2) {  // quadrilateral
                for (i32 n = 0; n < 8; ++n) {
                    const i32 v = L.int_field();
                    fr.wr32(kArr[n] - 4 * i, v);
                }
            }
        }
        const i32 index = joan::orig::arg::as<i32>(joan::orig::as_zones(obj(d::g_territory_array))->add_new());               // 0x004e4a50
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_territory_array))->get_420ff0(joan::orig::arg::i32(index)));          // 0x00420ff0
        strncpy_(jo::self<jo::ZoneDef>(rec)->name, fr.at(kName), 0x20);
        jo::self<jo::ZoneDef>(rec)->values[0] = fr.rd32(kInt130);
        jo::self<jo::ZoneDef>(rec)->values[1] = fr.rd32(kInt130 + 4);
        jo::self<jo::ZoneDef>(rec)->values[2] = fr.rd32(kInt130 + 8);
        jo::self<jo::ZoneDef>(rec)->values[3] = fr.rd32(kInt130 + 0xc);
        jo::self<jo::ZoneDef>(rec)->owner = fr.rd32(kOwner);
        jo::self<jo::ZoneDef>(rec)->disable = fr.rd8(kDisable);
        strncpy_(jo::self<jo::ZoneDef>(rec)->script, fr.at(kScr), 8);
        jo::view<u8>(jo::self<jo::ZoneDef>(rec)->kind) = fr.rd8(kKind);
        jo::self<jo::ZoneDef>(rec)->settle = fr.rd8(kSettle);
        jo::view<u8>(jo::self<jo::ZoneDef>(rec)->shape_count) = static_cast<u8>(fr.rd32(kCount));
        for (i32 i = 0; i < fr.rd32(kCount); ++i) {
            jo::self<jo::ZoneDef>(rec)->shape_kind[i] = fr.rd8(kShape - i);
            for (i32 n = 0; n < 9; ++n) jo::self<jo::ZoneDef>(rec)->coords[4 * n + i] = fr.rd32(kArr[n] - 4 * i);
        }
        jo::self<jo::ZoneDef>(rec)->flag_46 = 0;
        return 1;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// whole loader frame (absent ScrName, unmatched kind bytes, unread shape ints) -> 0
FIX("Settlement::load", 0x004f9330, &H::settlement_load);
#undef FIX

}  // namespace game_b111_15
}  // namespace

// ---------------------------------------- from modules/game_b88_02.cpp (game/b88_02)
namespace {
namespace game_b88_02 {

constexpr Fpu kFpu{true};


struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "layout");

void* const kFormations = obj(d::g_place_array);

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }
bool unit_invalid(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)) != 0; }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t* type_of(std::int16_t id) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(sx16(id))));  // 0x0042d0a0 (reads the word)
}

JOAN_COUNTER(c_arrange, "Group::arrange");

// The three per-member stack arrays of 0x004fa0f0, in frame order (ebp-0x234 / -0x144 / -0x130).
struct Slots {
    Vec3 grid[20];             // un-rotated offsets (x, y; z stays 0)
    std::uint8_t used[0x14];   // per-member "assigned" flags (written by 0x0045de50)
    Vec3 turned[20];           // rotated offsets
};
static_assert(sizeof(Slots) == 0xf0 + 0x14 + 0xf0, "layout");

struct H {
    // ------------------------------------------------------------------------ 0x004fa0f0
    // this = group: word +0 leader id, byte +2 member count, word ids +3, byte +0x37 mode,
    // Vec3 +0x38 position, +0x44 heading.
    void arrange(std::int32_t follow, std::int32_t arg2) {
        c_arrange.hit();
        if (unit_invalid(sx16(jo::self<jo::Group>(this)->leader))) return;
        std::uint8_t* const leader = unit_get(sx16(jo::self<jo::Group>(this)->leader));  // ebp-0x8
        Vec3 dir;    // ebp-0x14
        Vec3 from;   // ebp-0x20
        Vec3 to;     // ebp-0x2c
        std::memcpy(&dir, jo::self<jo::Vec3f>(obj(d::g_pos)), 12);
        std::memcpy(&from, jo::self<jo::Vec3f>(obj(d::g_pos)), 12);
        std::memcpy(&to, jo::self<jo::Vec3f>(obj(d::g_pos)), 12);
        if (jo::view<jo::TaskSlot>(jo::as_unit(leader)->task).kind == 0xc) return;
        if (sx16(jo::as_unit(leader)->place_index) != 0 && sx16(jo::as_unit(leader)->place_slot) != 0 &&
            (jo::view<jo::TaskSlot>(jo::as_unit(leader)->task).kind == 1 || jo::view<jo::TaskSlot>(jo::as_unit(leader)->task).kind == 6) &&
            jo::view<std::int32_t>(jo::view<jo::TaskSlot>(jo::as_unit(leader)->task).target) == sx16(jo::as_unit(leader)->place_index)) {
            if (follow == 0) return;
            joan::orig::as_group(this)->front_point(joan::orig::arg::mptr(static_cast<void*>(&from)));  // 0x00451dc0
            {
                void* const f = joan::orig::arg::as<void*>(joan::orig::as_list(kFormations)->get_c70(joan::orig::arg::i32(sx16(jo::as_unit(leader)->place_index))));  // 0x00456c70
                const void* const p = joan::orig::arg::as<void*>(joan::orig::as_place(f)->slot_pos(joan::orig::arg::i32(sx16(jo::as_unit(leader)->place_slot))));       // 0x004c2470
                std::memcpy(&jo::self<jo::Group>(this)->x, p, 12);
            }
            {
                void* const f = joan::orig::arg::as<void*>(joan::orig::as_list(kFormations)->get_c70(joan::orig::arg::i32(sx16(jo::as_unit(leader)->place_index))));
                jo::self<jo::Group>(this)->angle = joan::orig::arg::as<float>(joan::orig::as_place(f)->slot_angle(joan::orig::arg::i32(sx16(jo::as_unit(leader)->place_slot))));  // 0x004c24b0
            }
            joan::orig::as_group(this)->front_point2(joan::orig::arg::mptr(static_cast<void*>(&to)));  // 0x0044a950
        } else {
            const float dx = jo::view<float[3]>(jo::as_unit(leader)->position_)[0] - jo::self<jo::Group>(this)->x;
            const float dx2 = jo::view<float[3]>(jo::as_unit(leader)->position_)[0] - jo::self<jo::Group>(this)->x;
            const float dy = jo::view<float[3]>(jo::as_unit(leader)->position_)[1] - jo::self<jo::Group>(this)->y;
            const float dy2 = jo::view<float[3]>(jo::as_unit(leader)->position_)[1] - jo::self<jo::Group>(this)->y;
            const float dist = dx * dx2 + dy * dy2;  // ebp-0x30
            // fild [0xa3cc74]; fcomp dist; test ah,0x41; je -> limit > dist (not unordered)
            if (static_cast<double>(global<std::int32_t>(d::g_a3cc74)) > static_cast<double>(dist)) {
                if (follow == 0) return;
                arg2 = 0;
                std::memcpy(&to, jo::self<jo::Vec3f>(obj(d::g_pos)), 12);
                std::memcpy(&from, &to, 12);
            } else if (follow == 0) {
                joan::orig::as_group(this)->front_point(joan::orig::arg::mptr(static_cast<void*>(&from)));  // 0x00451dc0
                std::memcpy(&jo::self<jo::Group>(this)->x, jo::as_unit(leader)->position_, 12);
                jo::view<std::uint32_t>(jo::self<jo::Group>(this)->angle) = jo::view<std::uint32_t>(jo::as_unit(leader)->heading);
                joan::orig::as_group(this)->front_point2(joan::orig::arg::mptr(static_cast<void*>(&to)));  // 0x0044a950
            }
        }
        if (static_cast<std::int8_t>(jo::self<jo::Group>(this)->formation) == 0) return;
        std::uint32_t angle = 0;  // ebp-0x34 (1-byte angle object)
        joan::orig::as_angle(&angle)->construct();                         // 0x00576120 (ctor, no-op)
        joan::orig::as_angle(&angle)->set(joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::Group>(this)->angle)));  // 0x00576240 (float pushed raw)
        {
            Vec3 tmp;  // ebp-0x29c
            const Vec3* const d = joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(static_cast<void*>(&tmp), static_cast<const void*>(&to), static_cast<const void*>(&from)));  // 0x0060cc90
            std::memcpy(&dir, d, 12);  // via ebp-0x290
        }
        if (jo::self<jo::Group>(this)->formation != 1) return;

        const std::int32_t spacing = 100;  // ebp-0x3c
        const std::int32_t front = -100;   // ebp-0x40
        Slots s;
        std::memset(s.grid, 0, sizeof s.grid);
        std::memset(s.used, 0, sizeof s.used);
        std::memset(s.turned, 0, sizeof s.turned);
        std::int32_t n_first = 0;   // ebp-0x238 (type class 1 / 4, assigned with kind 1)
        std::int32_t n_third = 0;   // ebp-0x23c (type class 2 / 3, kind 3)
        std::int32_t n_second = 0;  // ebp-0x240 (type class 5 / 6 / 8, kind 2)
        const std::int32_t count = static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Group>(this)->count));
        std::int32_t cols = 5;  // ebp-0x38 (uninitialised in the original when count > 25, see header)
        if (count <= 1)
            cols = 1;
        else if (count <= 4)
            cols = 2;
        else if (count <= 9)
            cols = 3;
        else if (count <= 0x10)
            cols = 4;
        else if (count <= 0x19)
            cols = 5;
        const std::int32_t width = (cols - 1) * spacing;  // ebp-0x244
        for (std::int32_t i = 0; i < static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Group>(this)->count)); ++i) {
            const std::int32_t gx = front - (i / cols) * spacing;     // ebp-0x24c
            const std::int32_t gy = width / 2 - (i % cols) * spacing;  // ebp-0x250
            const float c1 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos());  // 0x00576ac0
            const float t1 = static_cast<float>(kFpu.mul(c1, static_cast<double>(gx)));  // ebp-0x2a4
            const float s1 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin());  // 0x005768d0
            s.turned[i].x =
                static_cast<float>(kFpu.sub(t1, kFpu.mul(s1, static_cast<double>(gy))));
            const float s2 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin());
            const float t2 = static_cast<float>(kFpu.mul(s2, static_cast<double>(gx)));  // ebp-0x2a8
            const float c2 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos());
            s.turned[i].y =
                static_cast<float>(kFpu.add(kFpu.mul(c2, static_cast<double>(gy)), t2));
            s.grid[i].x = static_cast<float>(gx);
            s.grid[i].y = static_cast<float>(gy);
            const std::int32_t id = sx16(jo::self<jo::Group>(this)->members[i]);
            if (unit_invalid(id)) continue;
            std::uint8_t* const u = unit_get(sx16(jo::self<jo::Group>(this)->members[i]));  // ebp-0x254
            switch (sx16(jo::self<jo::UnitType>(type_of(jo::as_unit(u)->type_id))->unit_class)) {
            case 1:
            case 4: ++n_first; break;
            case 2:
            case 3: ++n_third; break;
            case 5:
            case 6:
            case 8: ++n_second; break;
            default: break;
            }
        }
        std::int32_t slot = 0;  // ebp-0x258
        const std::int32_t kinds[3] = {1, 3, 2};
        const std::int32_t counts[3] = {n_first, n_third, n_second};
        for (std::int32_t pass = 0; pass < 3; ++pass) {
            for (std::int32_t k = 0; k < counts[pass]; ++k) {
                Vec3 target;  // ebp-0x268
                target.x = jo::self<jo::Group>(this)->x + s.turned[slot].x;
                target.y = jo::self<jo::Group>(this)->y + s.turned[slot].y;
                jo::view<std::uint32_t>(target.z) = 0;
                const std::int32_t idx = thiscall<std::int32_t>(
                    fn::Group_nearest_free_member, this, static_cast<void*>(&target), static_cast<void*>(&s.grid[slot]),
                    static_cast<void*>(&dir), static_cast<void*>(s.used), kinds[pass], arg2,
                    spacing / 10);  // 0x0045de50
                std::uint8_t* const u = unit_get(sx16(jo::self<jo::Group>(this)->members[idx - 1]));
                jo::view<std::uint32_t>(jo::as_unit(u)->formation_offset[0]) = jo::view<std::uint32_t>(s.turned[slot].x);
                jo::view<std::uint32_t>(jo::as_unit(u)->formation_offset[1]) = jo::view<std::uint32_t>(s.turned[slot].y);
                jo::view<float>(jo::as_unit(u)->form_width) = static_cast<float>(width);
                jo::view<std::uint32_t>(jo::as_unit(u)->formation_grid_x) = jo::view<std::uint32_t>(s.grid[slot].x);
                jo::as_unit(u)->form_slot_y = jo::view<std::uint32_t>(s.grid[slot].y);
                ++slot;
            }
        }
        jo::view<float>(jo::as_unit(leader)->form_width) = static_cast<float>(width);
        jo::view<std::uint32_t>(jo::as_unit(leader)->formation_grid_x) = 0;
        jo::as_unit(leader)->form_slot_y = 0;
    }
};

#define B88(name, addr, fn) JOAN_REPLACE("game/b88_02", name, addr, fn)
B88("Group::arrange", 0x004fa0f0, &H::arrange);
#undef B88

}  // namespace game_b88_02
}  // namespace

// ---------------------------------------- from modules/game_b109_14.cpp (game/b109_14)
namespace {
namespace game_b109_14 {

struct Obj {};
// v3d_87.dll exports are cdecl, called through their IAT slot.
using joan::orig::helpers::v3d_call;
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

i32 sx16(i16 v) { return static_cast<i32>(v); }

struct V3 {  // 12 bytes copied as raw dwords
    float x, y, z;
};
struct alignas(4) Matrix {
    u8 bytes[0x40];
};

V3 load_v3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

JOAN_COUNTER(c_special, "Hero::special_4fab10");

struct H {
    // -------------------------------------------------------------- 0x004fab10
    void fire_special(u32 target, i16 side) {
        joan::orig::as_unit(this)->place_near_4ac910(joan::orig::arg::u32(target), joan::orig::arg::i16(sx16(side)));  // 0x004ac910
        jo::view<u32>(jo::self<jo::Unit>(this)->held_item_id) = target;
    }
    u8 special(u32 target, i16 arm, i16 side) {
        c_special.hit();
        if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(target)) != 0) return 0;  // 0x00425d50
        if (jo::self<jo::Unit>(this)->actor == nullptr) return 0;
        V3 v10{0, 0, 0};
        joan::orig::as_unit(this)->position(&v10);  // 0x004fedb0
        const V3 v1c = load_v3(jo::as_actor(jo::self<jo::Unit>(this)->actor)->bone->matrix_);
        const float zero = global<float>(d::k_9cf5c4);
        if (arm != 0 && jo::view<u32>(jo::self<jo::Unit>(this)->held_item_id) == 0) {
            void* const t = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(target));  // 0x00425d20
            V3 v2c{0, 0, 0};
            joan::orig::as_map_object(t)->position(&v2c);  // 0x00499930
            V3 v38 = load_v3(obj(d::g_a18a40));
            const float a = joan::orig::arg::as<float>(joan::orig::as_map_object(t)->yaw());  // 0x00499e20
            const float b = joan::orig::arg::as<float>(joan::orig::as_unit(t)->get_66());  // 0x0049a8c0
            const float c = joan::orig::arg::as<float>(joan::orig::as_map_object(t)->roll());  // 0x0049ad10
            Matrix m{};
            joan::orig::matrix_rotation_ypr(&m, joan::orig::arg::bits(a), joan::orig::arg::bits(b), joan::orig::arg::bits(c));  // 0x0060bf40
            v3d_call<void*>(d::iat_v3d_87_vector_transform_2, &v38, &m);      // vector_transform
            {
                V3 tmp{0, 0, 0};
                const V3 d = load_v3(joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &v10, &v2c)));  // 0x0060cc90
                const float d1 = v3d_float(d::iat_v3d_87_dot_product, &v38, &d);              // dot_product
                if (d1 >= zero) {  // test ah,1 jne: less or unordered skips
                    const float d2 = v3d_float(d::iat_v3d_87_dot_product, &v38, &v1c);
                    if (!(d2 > zero) && side != 0) {
                        fire_special(target, side);
                        return 1;
                    }
                }
            }
            V3 tmp{0, 0, 0};
            const V3 d = load_v3(joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &v10, &v2c)));  // 0x0060cc90
            const float d3 = v3d_float(d::iat_v3d_87_dot_product, &v38, &d);
            if (d3 > zero) return 0;
            const float d4 = v3d_float(d::iat_v3d_87_dot_product, &v38, &v1c);
            if (!(d4 > zero)) return 0;
            if (side != 0) return 0;
            fire_special(target, side);
            return 1;
        }
        if (arm != 0) return 0;
        if (jo::view<u32>(jo::self<jo::Unit>(this)->held_item_id) == 0) return 0;
        if (jo::self<jo::Unit>(this)->actor == nullptr) return 0;
        if (jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) == 0) return 0;
        void* const t = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Unit>(this)->held_item_id)));  // 0x00425d20
        V3 v94{0, 0, 0};
        joan::orig::as_map_object(t)->position(&v94);  // 0x00499930
        float pos[3];
        std::memcpy(pos, &v10, sizeof pos);
        u32 k_b0 = 0x4234cdd3;
        u32 k_b4 = 0x4285c5a2;
        const auto hero_is = [this](img::Addr name) {
            const char* const n = joan::orig::arg::as<const char*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(sx16(jo::self<jo::Unit>(this)->type_id)))) + 4;  // 0x0042d0a0
            return std::strcmp(joan::orig::arg::str(n), joan::orig::arg::str(str(name))) == 0;  // strcmp
        };
        if (hero_is(d::s_LAHIRE_9cfb3c)) {  // LAHIRE
            k_b0 = 0x422f0419;
            k_b4 = 0x42a02560;
        } else if (hero_is(d::s_ALENCON_9cfb30)) {  // ALENCON
            k_b0 = 0x422b0b44;
            k_b4 = 0x42a72d91;
        } else if (hero_is(d::s_JEAN_DE_METZ)) {  // JEAN_DE_METZ
            k_b0 = 0x4216bf7d;
            k_b4 = 0x42aa0000;
        }
        float b4;
        std::memcpy(&b4, &k_b4, 4);
        if (side == 2) {
            V3 tmp{0, 0, 0};
            const V3 o = load_v3(joan::orig::arg::as<V3*>(joan::orig::vec_scale(&tmp, &v1c, joan::orig::arg::f32(u32{0xc1f00000}))));  // -30.0f  // 0x0060cd00
            joan::orig::vec_add_in_place(joan::orig::arg::mptr(pos), &o);                                  // 0x0060cdc0
        } else if (side == 1) {
            pos[2] = pos[2] + b4;
            V3 tmp{0, 0, 0};
            const V3 o = load_v3(joan::orig::arg::as<V3*>(joan::orig::vec_scale(&tmp, &v1c, joan::orig::arg::f32(k_b0))));  // 0x0060cd00
            joan::orig::vec_add_in_place(joan::orig::arg::mptr(pos), &o);                               // 0x0060cdc0
        } else {
            std::memcpy(&pos[2], &v94.z, 4);
        }
        V3 p;
        std::memcpy(&p, pos, sizeof p);
        joan::orig::as_ground_object(this)->set_position(p);  // 0x004fedf0
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record))));  // 0x006af310
        joan::orig::as_anim_table(obj(d::g_physic_sys))->restart_6af370(joan::orig::arg::u32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)), joan::orig::arg::u32(jo::as_shot(rec)->u32_27a), joan::orig::arg::u32(// 0x006af370
                      sx16(jo::view<i16[2]>(jo::as_shot(rec)->type)[0])), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{1}));
        if (sx16(jo::self<jo::Unit>(this)->id) == joan::orig::arg::as<i32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id()))  // 0x004203c0
            joan::orig::as_player(obj(d::g_player_unit_recno))->reset_squad_command();                          // 0x0042d1d0
        jo::view<u32>(jo::self<jo::Unit>(this)->held_item_id) = 0;
        jo::self<jo::Unit>(this)->act_time = global<u32>(d::g_now);
        for (i32 slot = 0; slot < 2; ++slot) {
            i16* const item = joan::orig::arg::as<i16*>(joan::orig::as_modifiers(jo::view<u8*>(jo::self<jo::Unit>(this)->modifiers))->slot(joan::orig::arg::i32(slot)));  // 0x004974f0
            if (item == nullptr) continue;
            const u32 id = joan::orig::arg::as<u32>(joan::orig::as_units(obj(d::g_items))->create_49ea90(joan::orig::arg::u32(sx16(*item))));  // 0x0049ea90
            void* const o1 = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
            virtual_call<u32>(o1, 0x18, i32{1});
            void* const o2 = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id));
            virtual_call<u32>(o2, 0x1c);
            u8* const desc = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(sx16(*item))));  // 0x00425ce0
            joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(sx16(static_cast<i16>(id))), joan::orig::arg::i32(sx16(jo::view<i16>(jo::as_item_def(desc)->bone))), static_cast<std::int8_t>(joan::orig::arg::i32(// 0x0050a480
                          slot)), joan::orig::arg::mptr(i32{0}));
        }
        jo::self<jo::Unit>(this)->u8_202 = 1;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_14", name, addr, fn)
B("Hero::special_4fab10", 0x004fab10, &H::special);
#undef B

}  // namespace game_b109_14
}  // namespace

// ---------------------------------------- from modules/game_units_3.cpp (game/units2)
namespace {
namespace game_units_3 {

constexpr img::Addr kGameUnits(d::g_units);

void* camera() { return global<void*>(d::g_camera); }

JOAN_COUNTER(c_plate, "UnitDraw::plate");

// The scale of an overlay at pos that keeps it a constant size on screen: the camera eye
// from its orbit frame (0x006140a0: +4 target, +0x10 pitch, +0x14 yaw, +0x1c distance),
// the ground plane (0x00617150), l1 = |eye - pos|, l2 = |eye - ground hit|.
struct Scale {
    float l1, l2;
};
Scale screen_scale(const float pos[3]) {
    const joan::hook::x87::Fpu fpu{true};
    std::uint32_t frame[10];
    joan::orig::as_camera(camera())->frame(joan::orig::arg::mptr(static_cast<void*>(frame)));  // 0x006140a0
    float f[10];
    std::memcpy(f, frame, sizeof f);
    float eye[3] = {f[1], f[2], f[3]};
    eye[2] = static_cast<float>(fpu.add(fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(f[5])))), f[7]), eye[2]));
    const float t = static_cast<float>(fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(f[5])))), f[7]));
    eye[0] = static_cast<float>(fpu.sub(eye[0], fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(f[4])))), t)));
    eye[1] = static_cast<float>(fpu.sub(eye[1], fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(f[4])))), t)));
    float plane[8] = {};
    joan::orig::as_plane_equation(static_cast<void*>(plane))->init_425700();                          // 0x00425700 Plane4::Plane4
    joan::orig::as_camera(camera())->plane_617150(0, joan::orig::arg::mptr(static_cast<void*>(plane)));             // 0x00617150 ground plane
    const float seg[6] = {eye[0], eye[1], eye[2], pos[0], pos[1], pos[2]};
    float k = 0.0f;  // (the original leaves it unset when the ray misses the plane)
    joan::orig::line_plane(&k, joan::orig::arg::mptr(seg), joan::orig::arg::mptr(static_cast<const float*>(plane)));  // 0x0060e590
    const float hit[3] = {(seg[3] - seg[0]) * k + seg[0], (seg[4] - seg[1]) * k + seg[1], (seg[5] - seg[2]) * k + seg[2]};
    float a[3] = {eye[0] - pos[0], eye[1] - pos[1], eye[2] - pos[2]};
    float b[3] = {eye[0] - hit[0], eye[1] - hit[1], eye[2] - hit[2]};
    Scale s;
    s.l1 = v3d_float(d::iat_v3d_87_length, static_cast<void*>(a));  // length
    s.l2 = v3d_float(d::iat_v3d_87_length, static_cast<void*>(b));
    return s;
}

// Billboard (0x635xxx) setters through their thunks.
void bb_position(void* bb, const float p[3]) {
    std::uint32_t v[3];
    std::memcpy(v, p, sizeof v);
    joan::orig::as_billboard(bb)->set_position(joan::orig::arg::u32(v[0]), joan::orig::arg::u32(v[1]), joan::orig::arg::u32(v[2]));  // 0x00635d40
}
void bb_scale_y(void* bb, float v) { joan::orig::as_billboard(bb)->set_19(joan::orig::arg::bits(v)); }  // 0x00635db0
void bb_scale_x(void* bb, float v) { joan::orig::as_billboard(bb)->set_15(joan::orig::arg::bits(v)); }  // 0x00635d80
constexpr float kTiny = 0.01f;                                        // 0x3c23d70a

// The bar part of a plate / bar mesh: vertices +0x94 / +0xac (left) and +0xa0 / +0xb8 and
// +0xf4 / +0x10c / +0x100 / +0x118 follow the fill level.
void set_bar(void* plane, float left, float right, float width) {
    std::uint8_t* const v = jo::view<std::uint8_t*>(jo::as_plane(plane)->vertices_f4);
    jo::as_vertex_f4(v)[10].pos[1] = left;
    jo::view<std::uint32_t>(jo::as_vertex_f4(v)[6].pos[1]) = jo::view<std::uint32_t>(jo::as_vertex_f4(v)[10].pos[1]);
    jo::as_vertex_f4(v)[11].pos[1] = right;
    jo::view<std::uint32_t>(jo::as_vertex_f4(v)[7].pos[1]) = jo::view<std::uint32_t>(jo::as_vertex_f4(v)[11].pos[1]);
    jo::as_vertex_f4(v)[10].attr[1] = width;
    jo::view<std::uint32_t>(jo::as_vertex_f4(v)[6].attr[1]) = jo::view<std::uint32_t>(jo::as_vertex_f4(v)[10].attr[1]);
    jo::as_vertex_f4(v)[11].attr[1] = width;
    jo::view<std::uint32_t>(jo::as_vertex_f4(v)[7].attr[1]) = jo::view<std::uint32_t>(jo::as_vertex_f4(v)[11].attr[1]);
    joan::orig::as_plane(plane)->invalidate(2u);  // 0x005cf920 Plane::invalidate
}

void colour(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {  // 0x0042a260 into a dead local
    std::uint32_t c = 0;
    (void)joan::orig::as_colour(&c)->construct(r, g, b, a);
}

struct H {
    // 0x004fb320 UnitDraw::plate (this = unit draw object: +0 unit id, +0x46 relation,
    // +0x4a label, +0x4e plate billboard, +0x52 kind): the ring under the unit with its
    // health bar, constant screen size.
    void plate() {
        std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(img::obj(kGameUnits))->get_b(static_cast<std::int32_t>(jo::self<jo::UnitDraw>(this)->unit_id)));
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_) == nullptr || global<std::uint32_t>(d::g_camera_lock_flag_a48ce7) == 0) return;
        c_plate.hit();
        const std::int16_t player = global<std::int16_t>(d::g_player_unit_recno);
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(player))) != static_cast<std::int32_t>(jo::self<jo::UnitDraw>(this)->i16_46)) {  // 0x005094d0
            jo::self<jo::UnitDraw>(this)->i16_46 = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(player))));
            joan::orig::as_unit_draw(this)->plate_delete();  // 0x004287b0
            joan::orig::as_unit_draw(this)->create_plate();  // 0x00520270 create
            if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_) != nullptr) {
                joan::orig::as_unit_draw(this)->label_delete();  // 0x005213b0
                joan::orig::as_unit_draw(this)->create_label();  // 0x00520e80
            }
        }
        float pos[3];
        std::memcpy(pos, jo::as_unit(unit)->position_, sizeof pos);
        std::uint8_t* const type = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(img::obj(d::g_types))->get_42d0a0(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
        switch (jo::self<jo::UnitType>(type)->kind) {
        case 4: pos[2] = pos[2] + 95.0f; break;
        case 5: pos[2] = pos[2] + 85.0f; break;
        case 6: pos[2] = pos[2] + 77.0f; break;
        case 8: pos[2] = pos[2] + 825.0f; break;
        case 10: pos[2] = pos[2] + 625.0f; break;
        case 11: pos[2] = pos[2] + 360.0f; break;
        default: pos[2] = pos[2] + 95.0f; break;
        }
        if (jo::view<std::uint32_t>(jo::as_unit(unit)->mount_id) != 0) pos[2] = pos[2] + 30.0f;
        bb_position(jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_), pos);
        void* const plane = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_))->plane());  // 0x00635e80
        std::uint8_t* const v = jo::view<std::uint8_t*>(jo::as_plane(plane)->vertices_f4);
        const float span = (jo::as_vertex_f4(v)[2].pos[1] - 0.171875f) - (jo::as_vertex_f4(v)[0].pos[1] + 0.046875f);
        const std::int32_t max_hp = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hit_points_max());  // 0x00523320
        if (max_hp != 0) {
            const joan::hook::x87::Fpu fpu{true};
            const double hp = static_cast<double>(jo::as_unit(unit)->hit_points);
            const double fill = fpu.div(fpu.mul(hp, span), static_cast<double>(max_hp));
            const float left = static_cast<float>(fpu.add(fpu.add(fill, jo::as_vertex_f4(v)[0].pos[1]), 0.046875));
            const float right = static_cast<float>(fpu.add(fpu.add(fill, jo::as_vertex_f4(v)[1].pos[1]), 0.046875));
            const float width = static_cast<float>(fpu.div(fpu.mul(hp, 0.78125), static_cast<double>(max_hp)));
            set_bar(plane, left, right, width);
        }
        // Allegiance colours (computed into a local the original never uses).
        colour(0xff, 0xff, 0xff, 0xff);
        const std::int32_t rank = jo::as_unit(unit)->state_120;
        for (std::int32_t pass = 0; pass < 2; ++pass) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(img::obj(kGameUnits))->empty_at_b(joan::orig::arg::i32(global<std::int32_t>(d::g_player_unit_recno)))) != 0) continue;
            const bool hostile = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(player))) != 0;
            static const std::uint8_t table[2][2][4][4] = {
                {{{0, 0, 0xff, 0}, {0xff, 0, 0, 0x80}, {0xff, 0x96, 0x96, 0x80}, {0xff, 0xd7, 0xb3, 0x80}},
                 {{0, 0, 0xff, 0}, {0, 0, 0xff, 0x80}, {0xaa, 0xaa, 0xff, 0x80}, {0xb1, 0xdd, 0xff, 0x80}}},
                {{{0xff, 0, 0, 0x80}, {0xff, 0x96, 0x96, 0x80}, {0xff, 0xd7, 0xb3, 0x80}, {0xff, 0xff, 0x58, 0x80}},
                 {{0, 0, 0xff, 0x80}, {0xaa, 0xaa, 0xff, 0x80}, {0xb1, 0xdd, 0xff, 0x80}, {0xc6, 0xff, 0xff, 0x80}}}};
            if (rank >= 0 && rank <= 3) {
                const std::uint8_t* const c = table[pass][hostile ? 0 : 1][rank];
                colour(c[0], c[1], c[2], c[3]);
            }
        }
        const Scale s = screen_scale(pos);
        const joan::hook::x87::Fpu fpu{true};
        const float q = s.l1 / s.l2;
        bb_scale_y(jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_), static_cast<float>(fpu.mul(q, 0.06)));
        bb_scale_x(jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_), static_cast<float>(fpu.mul(q, 0.24)));
        if (s.l1 > 1000.0f && global<std::int32_t>(d::g_control_mode) != 4) {
            bb_scale_y(jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_), kTiny);
            bb_scale_x(jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_), kTiny);
        }
        std::uint8_t* const unit2 = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(img::obj(kGameUnits))->get_b(static_cast<std::int32_t>(jo::self<jo::UnitDraw>(this)->unit_id)));
        std::uint8_t* const type2 = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(img::obj(d::g_types))->get_42d0a0(joan::orig::arg::i16(jo::as_unit(unit2)->type_id)));
        if (jo::self<jo::UnitType>(type2)->unit_class == 0 && static_cast<std::int8_t>(jo::self<jo::UnitDraw>(this)->u8_52) == 1) {
            bb_scale_y(jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_), kTiny);
            bb_scale_x(jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_), kTiny);
        }
    }
};

#define U2(name, addr, fn) JOAN_REPLACE("game/units2", name, addr, fn)
U2("UnitDraw::plate", 0x004fb320, &H::plate);
#undef U2

}  // namespace game_units_3
}  // namespace

// ---------------------------------------- from modules/game_b104_06.cpp (game/b104_06)
namespace {
namespace game_b104_06 {

void* const kObjs = obj(d::g_items);
void* const kBlood = obj(d::g_a5ccd0);

i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

struct Vec3 {
    float v[3];
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_hit_effects, "Projectile::hit_effects");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
void* unit_get(i32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
void* kind_get(i32 w) { return joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(sx16(w)))); }  // 0x00425ce0 (word)

// 0x004eef00 (ret 0x40): spawn effect.
void effect(u32 a1, u32 a2, i32 w3, void* r8, const Vec3& pos, u32 a8) {
    const u32 g = global<u32>(d::g_now);
    joan::orig::as_triggers(kBlood)->add(joan::orig::arg::u32(a1), joan::orig::arg::u32(a2), joan::orig::arg::i16(sx16(w3)), 1u, joan::orig::arg::i16(sx16(jo::as_unit(r8)->id)), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), pos, joan::orig::arg::u32(a8), static_cast<std::int8_t>(joan::orig::arg::i32(0u)), joan::orig::arg::u32(g), joan::orig::arg::u32(g), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), 0u, joan::orig::arg::i16(0u));
}

struct H {
    // ------------------------------------------------------------------------ 0x004fbc90
    std::uintptr_t hit_effects(i32 target) {
        c_hit_effects.hit();
        {
            const u32 r = joan::orig::arg::as<u32>(joan::orig::as_units(kObjs)->list.empty_at(joan::orig::arg::i32(target)));  // 0x00425d50
            if (r != 0) return r;
        }
        if (unit_missing(jo::view<i32>(jo::self<jo::Missile>(this)->shooter_unit_id)) != 0) return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_missile(this)->expire());  // 0x004e6fb0
        void* const r8 = unit_get(jo::view<i32>(jo::self<jo::Missile>(this)->shooter_unit_id));
        if (joan::orig::arg::as<u32>(joan::orig::as_units(kObjs)->list.empty_at(joan::orig::arg::i32(jo::view<i32>(jo::self<jo::Missile>(this)->object_id)))) != 0)  // 0x00425d50
            return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_missile(this)->expire());                            // 0x004e6fb0
        {
            void* const r18 = joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->model_id));  // 0x006af310
            Vec3 dead{};
            joan::orig::as_body(r18)->scaled_position(&dead);  // 0x006b1030 (result unused)
        }
        void* const r1c = joan::orig::as_units(kObjs)->list.ptr_at(joan::orig::arg::i32(target));  // 0x00425d20
        jo::view<i32>(jo::self<jo::Missile>(this)->expired) = target;
        const u32 sel = jo::self<jo::Missile>(this)->state;
        if (sel == 0 || sel == 1) {
            effect(2, 2, jo::as_map_object(r1c)->portrait_unit, r8, jo::view<Vec3>(jo::as_map_object(r1c)->pos), jo::self<jo::Missile>(this)->u32_46);
        } else if (sel == 2) {
            Vec3 tmp{};
            const Vec3 pos = *joan::orig::arg::as<Vec3*>(joan::orig::as_marker(this)->position(&tmp, joan::orig::arg::mptr(jo::as_map_object(r1c)->actor)));  // 0x004e5d90
            effect(0x2000, 2, jo::as_map_object(r1c)->portrait_unit, r8, pos, jo::self<jo::Missile>(this)->u32_46);
            effect(0x2000, 2, 0, r8, pos, jo::self<jo::Missile>(this)->u32_46);
            effect(0x2000, 1, 0, r8, pos, jo::self<jo::Missile>(this)->u32_46);
            const i32 r = joan::orig::as_misc(obj(d::g_misc))->random(3);  // 0x00590540
            if (r == 0)
                joan::orig::as_sound_list(kFx)->play_effect(str(d::s_EXPLOSIVEHIT1), pos);  // 0x00494810 'EXPLOSIVEHIT1'
            else if (r == 1)
                joan::orig::as_sound_list(kFx)->play_effect(str(d::s_EXPLOSIVEHIT2), pos);  // 'EXPLOSIVEHIT2'
            else if (r == 2)
                joan::orig::as_sound_list(kFx)->play_effect(str(d::s_EXPLOSIVEHIT3), pos);  // 'EXPLOSIVEHIT3'
        } else if (sel <= 5) {
            Vec3 tmp{};
            const Vec3 pos = *joan::orig::arg::as<Vec3*>(joan::orig::as_marker(this)->position(&tmp, joan::orig::arg::mptr(jo::as_map_object(r1c)->actor)));  // 0x004e5d90
            void* const k = kind_get(jo::as_map_object(r1c)->type_id);
            if (jo::view<i32>(jo::as_item_def(k)->struction_type) == 1 || jo::view<i32>(jo::as_item_def(k)->struction_type) == 2)
                joan::orig::as_sound_list(kFx)->play_effect(str(d::s_CANNONHH), pos);  // 0x00494810 'CANNONHH'
            else
                joan::orig::as_sound_list(kFx)->play_effect(str(d::s_CANNONHM), pos);  // 'CANNONHM'
            effect(0x40, 2, jo::as_map_object(r1c)->portrait_unit, r8, pos, jo::self<jo::Missile>(this)->u32_46);
            effect(0x40, 1, 0, r8, pos, jo::self<jo::Missile>(this)->u32_46);
            effect(0x40, 2, 0, r8, pos, jo::self<jo::Missile>(this)->u32_46);
        }
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_missile(this)->expire());  // 0x004e6fb0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_06", name, addr, fn)
B("Projectile::hit_effects", 0x004fbc90, &H::hit_effects);
#undef B

}  // namespace game_b104_06
}  // namespace

// ---------------------------------------- from modules/game_b87_01.cpp (game/b87_01)
namespace {
namespace game_b87_01 {

struct Mat44 {
    float m[16];
};

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kAnims = obj(d::g_physic_sys);

std::uint8_t* type_of(std::int16_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x0042d0a0
using joan::orig::helpers::strcmp_;
std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
void get_pos(void* unit, Vec3* out) { joan::orig::as_unit(unit)->position(joan::orig::arg::mptr(static_cast<void*>(out))); }        // 0x004fedb0
void set_pos(void* unit, Vec3 pos) { joan::orig::as_ground_object(unit)->set_position(pos); }                            // 0x004fedf0
void rotation(Mat44* m, std::uint32_t yaw) {
    joan::orig::matrix_rotation_ypr(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::u32(yaw), 0u, 0u);  // 0x0060bf40
}
Vec3 scale(const Vec3& v, float s) {
    Vec3 tmp;
    return *joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(&tmp, &v, joan::orig::arg::f32(s)));  // 0x0060cd00
}
Vec3 add(const Vec3& a, const Vec3& b) {
    Vec3 tmp;
    return *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&tmp, &a, &b));  // 0x0060cc20
}
Vec3 sub(const Vec3& a, const Vec3& b) {
    Vec3 tmp;
    return *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &a, &b));  // 0x0060cc90
}
void add_assign(Vec3* a, const Vec3* b) { joan::orig::vec_add_in_place(joan::orig::arg::mptr(a), joan::orig::arg::ptr(b)); }  // 0x0060cdc0
void sub_assign(Vec3* a, const Vec3* b) { joan::orig::vec3_sub_assign(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b)); }  // 0x0060ce20
float unit_angle(void* unit) { return joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading()); }        // 0x004ff850
void set_angle_deg(void* unit, float v) { joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(v)); }        // 0x0050abd0
std::int8_t slot_select(void* unit, std::int32_t id) { return joan::orig::arg::as<std::int8_t>(joan::orig::as_slot(unit)->select(joan::orig::arg::i32(id))); }  // 0x00482930
std::uint8_t place_ok(void* unit) {
    return joan::orig::arg::as<std::uint8_t>(joan::orig::as_collision_grid(obj(d::g_collision_grid))->is_free(joan::orig::arg::mptr(jo::as_unit(unit)->actor), joan::orig::arg::u32(jo::as_unit(unit)->layer)));  // 0x00693170
}
void anim(void* unit, std::int32_t a) { joan::orig::as_unit(unit)->set_action(joan::orig::arg::i32(a), 0, 1); }  // 0x00501190

// LAHIRE / ALENCON 45, JEAN_DE_METZ / others 30 (by the rider's type name).
float rider_distance(std::uint8_t* self) {
    std::uint32_t d = 0x41f00000u;
    if (strcmp_(reinterpret_cast<const char*>(type_of(jo::as_unit(self)->type_id) + 4), str(d::s_LAHIRE_9cfb3c)) == 0) {
        d = 0x42340000u;
    } else if (strcmp_(reinterpret_cast<const char*>(type_of(jo::as_unit(self)->type_id) + 4), str(d::s_ALENCON_9cfb30)) == 0) {
        d = 0x42340000u;
    } else if (strcmp_(reinterpret_cast<const char*>(type_of(jo::as_unit(self)->type_id) + 4), str(d::s_JEAN_DE_METZ)) == 0) {
        d = 0x41f00000u;
    }
    float f;
    std::memcpy(&f, &d, 4);
    return f;
}

constexpr float kDeg = 0.01745329238474369f;  // 0x9cf884

void finish_state(void* unit) {
    const std::int32_t st = joan::orig::arg::as<std::int32_t>(joan::orig::as_cell_object(unit)->find_anim_state());              // 0x00504010
    joan::orig::as_cell_object(unit)->set_anim_state(joan::orig::arg::i32(st), joan::orig::arg::i32(global<std::int32_t>(d::g_now)), joan::orig::arg::i32(-1));         // 0x004ff9f0
}

JOAN_COUNTER(c_mount, "Unit::mount");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    // ---- 0x004fc220 Unit::mount(id) ret 4 ----------------------------------------------------
    std::uint8_t mount(std::int32_t id) {
        c_mount.hit();
        if (id == static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id)) return 0;
        std::uint8_t* const type = type_of(jo::self<jo::Unit>(this)->type_id);  // [ebp-0x8]
        const joan::hook::x87::Fpu fpu{true};

        if (id == 0 && jo::self<jo::Unit>(this)->mount_id != 0) {
            // ---- dismount ----
            if (jo::self<jo::Unit>(this)->actor == nullptr) return 0;
            Vec3 pos;
            get_pos(this, &pos);
            Mat44 m;
            rotation(&m, jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->heading));
            const Vec3 dir{m.m[4], m.m[5], m.m[6]};
            const float dist = rider_distance(self());
            std::uint8_t ok = 1;
            set_pos(this, add(pos, scale(dir, dist)));
            jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode) = jo::view<std::int32_t>(jo::self<jo::UnitType>(type)->shape_kind);
            if (place_ok(this) == 0) ok = 0;
            if (ok == 0) {
                set_pos(this, sub(pos, scale(dir, dist)));
                if (place_ok(this) == 0) {
                    set_pos(this, pos);
                    jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode) = 0;
                    return 0;
                }
            }
            jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode) = 0;
            set_pos(this, pos);
            set_angle_deg(this, unit_angle(this) / kDeg);
            if (unit_missing(jo::self<jo::Unit>(this)->mount_id) == 0) {
                std::uint8_t* const horse = unit_get(jo::self<jo::Unit>(this)->mount_id);
                slot_select(horse, 0);
                set_angle_deg(horse, unit_angle(horse) / kDeg);
            }
            if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
                std::uint8_t* const rec =
                    joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(kAnims)->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record))));  // 0x006af310
                joan::orig::as_anim_table(kAnims)->restart_6af370(joan::orig::arg::u32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)), joan::orig::arg::u32(jo::view<std::int32_t>(jo::as_shot(rec)->u32_27a)), joan::orig::arg::u32(jo::view<std::int16_t[2]>(jo::as_shot(rec)->type)[0]), 0u, 2u, 1u);  // 0x006af370
            }
            anim(this, ok != 0 ? 0x51 : 0x52);
            finish_state(this);
            jo::self<jo::Unit>(this)->mount_busy = 0;
            jo::self<jo::Unit>(this)->dismounted_flag = 0xff;
            jo::self<jo::Unit>(this)->mounted_flag = 0;
            jo::self<jo::Unit>(this)->mount_id = 0;
            return 1;
        }

        // ---- mount ----
        if (unit_missing(id) != 0) return 2;
        if (jo::self<jo::Unit>(this)->actor == nullptr) return 0;
        std::uint8_t* const horse = unit_get(id);
        std::uint8_t* const htype = type_of(jo::as_unit(horse)->type_id);
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(this)->can_use(static_cast<std::int32_t>(jo::self<jo::UnitType>(htype)->kind))) == 0) {  // 0x0042a4d0
            return 0;
        }
        if (slot_select(horse, static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id)) == 0) return 0;
        if (jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) == 0 || jo::self<jo::Unit>(this)->mount_id != 0) return 0;
        joan::orig::as_players(kAnims)->set_active(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)), joan::orig::arg::i16(0));  // 0x006ad980
        Mat44 m;
        rotation(&m, jo::view<std::uint32_t>(jo::as_unit(horse)->heading));
        const Vec3 row0{m.m[0], m.m[1], m.m[2]};  // [ebp-0xd8]
        const Vec3 row1{m.m[4], m.m[5], m.m[6]};  // [ebp-0xe4]
        const float side = joan::orig::arg::as<float>(joan::orig::as_unit(horse)->heading_to_unit(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id)));  // 0x00505960
        Vec3 p;
        get_pos(horse, &p);
        const float dist = rider_distance(self());
        if (!(side >= 0.0f)) {  // fcomp 0.0, test ah,1: side < 0 or NaN
            const Vec3 a = scale(row1, dist);
            add_assign(&p, &a);
            const Vec3 b = scale(row0, 30.0f);
            add_assign(&p, &b);
            set_pos(this, p);
            const double t = joan::orig::arg::as<double>(joan::orig::libc::atan2_f(joan::orig::arg::f32(row0.y), joan::orig::arg::f32(row0.x)));  // 0x00421020 atan2
            const float half = global<float>(d::g_pi_a3cbb4) / 2.0f;
            const float ang = static_cast<float>(fpu.sub(t, half));
            joan::orig::as_unit(this)->set_target(joan::orig::arg::f32(ang));  // 0x004ff870
            set_angle_deg(this, ang / kDeg);
            anim(this, 0x4f);
        } else {
            const Vec3 a = scale(row1, dist);
            sub_assign(&p, &a);
            const Vec3 b = scale(row0, 30.0f);
            add_assign(&p, &b);
            set_pos(this, p);
            const double t = joan::orig::arg::as<double>(joan::orig::libc::atan2_f(joan::orig::arg::f32(row0.y), joan::orig::arg::f32(row0.x)));  // 0x00421020 atan2
            const float half = global<float>(d::g_pi_a3cbb4) / 2.0f;
            const float ang = static_cast<float>(fpu.add(t, half));
            joan::orig::as_unit(this)->set_target(joan::orig::arg::f32(ang));  // 0x004ff870
            set_angle_deg(this, ang / kDeg);
            anim(this, 0x50);
        }
        finish_state(this);
        jo::self<jo::Unit>(this)->mount_busy = 0;
        jo::self<jo::Unit>(this)->mounted_flag = 0xff;
        jo::self<jo::Unit>(this)->dismounted_flag = 0;
        jo::self<jo::Unit>(this)->mount_id = id;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_01", name, addr, fn)
B("Unit::mount", 0x004fc220, &H::mount);
#undef B

}  // namespace game_b87_01
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

struct H {
    // this: the form record of Actor::record in the players list 0x00a570a8 (the C++ caller, unitupd.cpp, as the
    // other readers of that list: a Shot), so +0x298 is Shot::hit. The upper half is eax = this as in the original.
    std::uintptr_t word_298() {                         // 0x004fd3d0
        return (reinterpret_cast<std::uintptr_t>(this) & 0xffff0000u) | jo::self<jo::Shot>(this)->hit;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("X::word_298", 0x004fd3d0, &H::word_298);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
namespace {
namespace game_b102_05 {

struct H {
    // ------------------------------------------------------------------ 0x004fd4e0
    void empty_4fd4e0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("Obj::empty_4fd4e0", 0x004fd4e0, &H::empty_4fd4e0);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

struct H {
    void* self() { return static_cast<void*>(this); }

    void* clear_247() {  // 0x004fd630
        jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->think_forced) = 0;
        return self();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Obj::clear_247", 0x004fd630, &H::clear_247);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b106_14.cpp (game/b106_14)
namespace {
namespace game_b106_14 {

JOAN_COUNTER(c_spiritual, "Unit::use_spiritual_item");

// ---------------------------------------------------------------- 0x004fd980 helpers
void* inv_list(const void* o, jo::Grid* jo::UnitData::*grid) { return jo::self<jo::UnitData>(jo::as_unit(o)->modifiers)->*grid; }

// Scan + use one item kind; returns true when an item was found (the original then returns 0).
bool use_best(void* self, void* owner, jo::Grid* jo::UnitData::*off, i16 kind, i32 diff, std::uintptr_t use_thunk, i32 force) {
    i32 best = 0, best_d = 0;
    for (i32 i = joan::orig::as_dynarray(inv_list(owner, off))->size(); i > 0; --i) {  // 0x00417ad0
        if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(inv_list(owner, off))->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;    // 0x00457f60
        void* item = joan::orig::arg::as<void*>(joan::orig::as_ptr_list(inv_list(owner, off))->get(joan::orig::arg::i32(i)));          // 0x00457e90
        void* rec = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::as_inv_item(item)->type)));  // 0x00425ce0
        if (static_cast<i32>(jo::view<i16>(jo::as_item_def(rec)->type_id)) != kind) continue;
        rec = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::as_inv_item(item)->type)));
        const i32 d = joan::orig::arg::as<i32>(joan::orig::libc::abs(joan::orig::arg::i32(jo::as_item_def(rec)->damage - diff)));  // abs
        if (best == 0 || !(d > best_d)) {
            best_d = d;
            best = i;
        }
    }
    if (best == 0) return false;
    void* item = joan::orig::arg::as<void*>(joan::orig::as_ptr_list(inv_list(owner, off))->get(joan::orig::arg::i32(best)));  // 0x00457e90
    if (thiscall<i32>(use_thunk, self, static_cast<i32>(jo::as_inv_item(item)->type), force, 1) != 0) {
        jo::as_inv_item(item)->count = static_cast<i16>(jo::as_inv_item(item)->count - 1);
        if (jo::as_inv_item(item)->count == 0) joan::orig::as_grid(inv_list(owner, off))->fill_from(joan::orig::arg::i32(best));  // 0x00457ec0
        if (jo::as_unit(self)->actor != nullptr) {
            const i32 snd = joan::orig::arg::as<i32>(joan::orig::as_name_list(obj(d::g_pemi_res))->find_4c2060(joan::orig::arg::str(&img::global<const char>(d::s_spiritual_txt)), static_cast<std::int8_t>(joan::orig::arg::i32(1))));  // 0x004c2060
            if (snd != 0) {
                void* s = joan::orig::arg::as<void*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(snd)));  // 0x0041b440
                const u32 e = joan::orig::arg::as<u32>(joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(jo::as_actor(jo::as_unit(self)->actor)->scene, jo::self<jo::PemiScript>(s)->effect, img::obj(d::g_pos), img::obj(d::g_pos), static_cast<std::int8_t>(joan::orig::arg::i32(1)), 0, 0, 0x1388));                                            // 0x0041baf0
                jo::Actor* const sprite = jo::as_unit(self)->actor;
                void* r = joan::orig::as_dynarray(obj(d::g_particle_effect_array))->ptr_at(joan::orig::arg::i32(e));  // 0x0041bd60
                joan::orig::as_effect(r)->set_owner(joan::orig::arg::mptr(sprite), joan::orig::arg::mptr(0));                      // 0x0041b0d0
            }
        }
    }
    return true;
}

struct H {
    // 0x004fd980
    u8 use_spiritual_item(i32 force) {
        c_spiritual.hit();
        void* self = this;
        if (static_cast<i32>(joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(self)->dead())) != 0 && force == 0) return 0;  // 0x0042d0e0
        if (global<i32>(d::g_player_unit_recno) == static_cast<i32>(jo::as_unit(self)->id) && global<i32>(d::g_control_mode) != 4) return 0;
        {
            const double q = joan::hook::x87::Fpu{true}.div(static_cast<double>(jo::as_unit(self)->hit_points),
                                                            static_cast<double>(jo::as_unit(self)->hit_points_base));
            if (!(q < static_cast<double>(global<float>(d::g_a3cd24)) || q != q)) return 0;
        }
        const i32 diff = joan::orig::arg::as<i32>(joan::orig::as_unit(self)->hit_points_max()) - jo::as_unit(self)->hit_points;  // 0x00523320
        void* owner = self;
        if (jo::as_unit(self)->rank >= 3) {
            if (jo::as_unit(self)->modifiers == nullptr || jo::as_modifiers(jo::as_unit(self)->modifiers)->grid_16 == nullptr) return 1;
            if (use_best(self, self, &jo::UnitData::grid_16, 0x15, diff, fn::code_value(fn::Unit_heal), force)) return 0;  // 0x0050b4f0
            if (use_best(self, self, &jo::UnitData::grid_16, 0x19, diff, fn::code_value(fn::Unit_heal_and_mana), force)) return 0;  // 0x0050bcd0
        }
        if (jo::as_unit(self)->rank < 3) {
            owner = joan::orig::arg::as<void*>(joan::orig::as_unit(self)->leader());  // 0x004216d0
            if (owner == nullptr) return 1;
        } else {
            owner = self;
        }
        if (jo::as_unit(owner)->modifiers == nullptr || jo::as_modifiers(jo::as_unit(owner)->modifiers)->grid_1a == nullptr) return 1;
        if (use_best(self, owner, &jo::UnitData::grid_1a, 0x15, diff, fn::code_value(fn::Unit_heal), force)) return 0;  // 0x0050b4f0
        if (use_best(self, owner, &jo::UnitData::grid_1a, 0x19, diff, fn::code_value(fn::Unit_heal_and_mana), force)) return 0;  // 0x0050bcd0
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_14", name, addr, fn)
B("Unit::use_spiritual_item", 0x004fd980, &H::use_spiritual_item);
#undef B

}  // namespace game_b106_14
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

struct H {
    // 0x004fe4f0 Unit::name()
    void* pos_ptr() { return jo::self<jo::Unit>(this)->name_; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Obj::pos_ptr", 0x004fe4f0, &H::pos_ptr);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_a3cb78() -> void { joan::bind::free<&::game_b97_04::init_a3cb78>(); }  // 0x4f9250
auto joan::orig::World::load_settlement(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b111_15::H::settlement_load>(this, a0)); }  // 0x4f9330
auto joan::orig::Group::arrange(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b88_02::H::arrange>(this, a0, a1); }  // 0x4fa0f0
auto joan::orig::Unit::hero_special(std::uint32_t a0, std::int16_t a1, std::int16_t a2) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b109_14::H::special>(this, a0, a1, a2)); }  // 0x4fab10
auto joan::orig::UnitDraw::plate() -> void { joan::bind::member<&::game_units_3::H::plate>(this); }  // 0x4fb320
auto joan::orig::Marker::hit_effects(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_06::H::hit_effects>(this, a0)); }  // 0x4fbc90
auto joan::orig::Unit::mount(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b87_01::H::mount>(this, a0)); }  // 0x4fc220
auto joan::orig::Marker::update_missile() -> void { joan::bind::member<&::game_b103_05::H::missile_update>(this); }  // 0x4fcc00
auto joan::orig::Body::bump_pending() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_04::H::word_298>(this)); }  // 0x4fd3d0
auto joan::orig::Unit::clear_247() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::clear_247>(this)); }  // 0x4fd630
auto joan::orig::Unit::use_spiritual_item_4fd980(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b106_14::H::use_spiritual_item>(this, a0)); }  // 0x4fd980
auto joan::orig::Unit::name() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_02::H::pos_ptr>(this)); }  // 0x4fe4f0
