// game/unknown_dlgtext.cpp: functions whose original source file is unknown (formerly game/_unplaced/dlgtext.cpp;
// re/attribution.csv: between ww/uti3d/dlgres.cpp and ww/uti3d/esstate2.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../fp/fma.hpp"
#include "../original/helpers.hpp"
#include "../callconv.hpp"
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
#include "../original/cxx_eh.hpp"
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using u8 = std::uint8_t;
using i16 = std::int16_t;

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using i8 = std::int8_t;

using joan::orig::helpers::op_delete;

using joan::orig::helpers::op_new;

using joan::hook::global;

using u16 = std::uint16_t;

void* const kInput = obj(d::g_mouse);

using joan::orig::helpers::game_new;  // operator new

}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {


using joan::orig::helpers::memset_;

struct H {
    void* self() { return this; }

    // 0x00639af0
    void* ctor_639af0() {
        joan::orig::as_string(self())->construct();  // 0x005a1170
        joan::orig::as_string(&jo::self<jo::Dialogue>(this)->text)->construct();  // 0x005a1170
        memset_(self(), 0, static_cast<std::uint32_t>(sizeof(jo::Dialogue)));  // 0x357 bytes on x86
        return self();
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj639::Obj639", 0x00639af0, &H::ctor_639af0);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b106_07.cpp (game/b106_07)
namespace {
namespace game_b106_07 {

using joan::call::thiscall;
int stricmp_(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp
void strcpy_(void* d, const void* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }  // strcpy

JOAN_COUNTER(c_model_prefix, "game/b106_07 Unit::set_model_prefix");

// ---- 0x00639b40 tables -------------------------------------------------------------------
struct Armor {
    img::Addr lookup;        // unit name for 0x005270b0
    img::Addr lv[3];   // armour record names (_stricmp against rec+0x10)
    img::Addr prefix[3];  // copied for lv1 / lv2 / lv3
    img::Addr fallback;      // copied when the unit is not found
};
const Armor kArmor[] = {
    {d::s_ALENCON_9cfb30, {d::s_ALENCON_ARMOR_LV1, d::s_ALENCON_ARMOR_LV2, d::s_ALENCON_ARMOR_LV3}, {d::s_ALENCON1, d::s_ALENCON2, d::s_ALENCON3}, d::s_ALENCON1},  // ALENCON
    {d::s_LAHIRE_9cfb3c, {d::s_LAHIRE_ARMOR_LV1, d::s_LAHIRE_ARMOR_LV2, d::s_LAHIRE_ARMOR_LV3}, {d::s_LAHIRE1, d::s_LAHIRE2, d::s_LAHIRE3}, d::s_LAHIRE1},  // LAHIRE
    {d::s_JEAN_DE_METZ, {d::s_JEAN_ARMOR_LV1, d::s_JEAN_ARMOR_LV2, d::s_JEAN_ARMOR_LV3}, {d::s_METZ1, d::s_METZ2, d::s_METZ3}, d::s_METZ1},  // JEAN_DE_METZ -> METZ
};
struct Prefix {
    img::Addr name;
    img::Addr prefix;
    const Armor* armor;
};
const Prefix kPrefixes[] = {
    {d::s_ALENCON_9cfb30, img::kNoAddr, &kArmor[0]},  // ALENCON (armor levels)
    {d::s_ORLEAN, d::s_BASTARD, nullptr},  // ORLEAN -> BASTARD
    {d::s_FLAVY, d::s_FLAVY, nullptr},  // FLAVY -> FLAVY
    {d::s_BEGLEY, d::s_FLAVY, nullptr},  // BEGLEY -> FLAVY
    {d::s_BEDFORD, d::s_BEDFORD, nullptr},  // BEDFORD -> BEDFORD
    {d::s_ENGLISH_POLEMAN_CAPTAIN, d::s_EPOLECAP, nullptr},  // ENGLISH_POLEMAN_CAPTAIN -> EPOLECAP
    {d::s_ENGLISH_LIGHT_INFANTRY_CAPTA, d::s_ELINFCAP, nullptr},  // ENGLISH_LIGHT_INFANTRY_CAPTAIN -> ELINFCAP
    {d::s_ENGLISH_LIGHT_INFANTRY, d::s_ELINF, nullptr},  // ENGLISH_LIGHT_INFANTRY -> ELINF
    {d::s_ENGLISH_SPY, d::s_ELINF, nullptr},  // ENGLISH_SPY -> ELINF
    {d::s_ENGLISH_HEAVY_INFANTRY_CAPTA, d::s_EHINFCAP, nullptr},  // ENGLISH_HEAVY_INFANTRY_CAPTAIN -> EHINFCAP
    {d::s_BUR_LIGHTINFANTRY, d::s_BLINF, nullptr},  // BUR_LIGHTINFANTRY -> BLINF
    {d::s_BUR_LIGHTINFANTRY_CAPTAIN, d::s_BLINFC, nullptr},  // BUR_LIGHTINFANTRY_CAPTAIN -> BLINFC
    {d::s_BUR_ARCHER_CAPTAIN, d::s_BLINFC, nullptr},  // BUR_ARCHER_CAPTAIN -> BLINFC
    {d::s_FRENCH_ARCHER_CAPTAIN, d::s_F_ARCCAP, nullptr},  // FRENCH_ARCHER_CAPTAIN -> F_ARCCAP
    {d::s_FRENCH_CROSSBOW_MAN_CAPTAIN, d::s_FCROSSC, nullptr},  // FRENCH_CROSSBOW_MAN_CAPTAIN -> FCROSSC
    {d::s_FRENCH_HEAVY_INFANTRY_CAPTAI, d::s_FHINFCAP, nullptr},  // FRENCH_HEAVY_INFANTRY_CAPTAIN -> FHINFCAP
    {d::s_DUNOIS, d::s_FHINFCAP, nullptr},  // DUNOIS -> FHINFCAP
    {d::s_DLLLIER, d::s_FHINFCAP, nullptr},  // DLLLIER -> FHINFCAP
    {d::s_FRENCH_LIGHT_INFANTRY_CAPTAI, d::s_FLINFCAP, nullptr},  // FRENCH_LIGHT_INFANTRY_CAPTAIN -> FLINFCAP
    {d::s_FRENCH_POLEMAN_CAPTAIN, d::s_FPOLECAP, nullptr},  // FRENCH_POLEMAN_CAPTAIN -> FPOLECAP
    {d::s_FRENCH_ARCHER, d::s_F_ARCH, nullptr},  // FRENCH_ARCHER -> F_ARCH
    {d::s_FRENCH_CROSSBOW_MAN, d::s_FCROSS, nullptr},  // FRENCH_CROSSBOW_MAN -> FCROSS
    {d::s_FRENCH_HEAVY_INFANTRY, d::s_FHINF, nullptr},  // FRENCH_HEAVY_INFANTRY -> FHINF
    {d::s_FRENCH_LIGHT_INFANTRY, d::s_FLINF, nullptr},  // FRENCH_LIGHT_INFANTRY -> FLINF
    {d::s_FRENCH_POLEMAN, d::s_FPOLE, nullptr},  // FRENCH_POLEMAN -> FPOLE
    {d::s_FRENCH_MACEMAN_CAPTAIN, d::s_FMACECAP, nullptr},  // FRENCH_MACEMAN_CAPTAIN -> FMACECAP
    {d::s_FRENCH_MACEMAN, d::s_FMACE, nullptr},  // FRENCH_MACEMAN -> FMACE
    {d::s_MER_LIGHT_INFANTRY, d::s_mltinf, nullptr},  // MER_LIGHT_INFANTRY -> mltinf
    {d::s_MER_LIGHT_INFANTRY_CAPTAIN, d::s_mltinfc, nullptr},  // MER_LIGHT_INFANTRY_CAPTAIN -> mltinfc
    {d::s_MER_POLEMAN, d::s_mpole, nullptr},  // MER_POLEMAN -> mpole
    {d::s_MER_POLEMAN_CAPTAIN, d::s_mpolecap, nullptr},  // MER_POLEMAN_CAPTAIN -> mpolecap
    {d::s_WILLIAM_GLASDALE, d::s_GLASDALE, nullptr},  // WILLIAM_GLASDALE -> GLASDALE
    {d::s_LAHIRE_9cfb3c, img::kNoAddr, &kArmor[1]},  // LAHIRE (armor levels)
    {d::s_JEAN_DE_METZ, img::kNoAddr, &kArmor[2]},  // JEAN_DE_METZ (armor levels)
    {d::s_JEAN_DE_METZ_2, img::kNoAddr, &kArmor[2]},  // JEAN_DE_METZ_2 (armor levels)
    {d::s_ARTHUR_DE_RICHEMONT, d::s_RICHEMON, nullptr},  // ARTHUR_DE_RICHEMONT -> RICHEMON
    {d::s_LORD_SCALES, d::s_SCALES, nullptr},  // LORD_SCALES -> SCALES
    {d::s_JOHN_FASTOLF, d::s_FASTOLF, nullptr},  // JOHN_FASTOLF -> FASTOLF
    {d::s_ENGLISH_MACEMAN_CAPTAIN, d::s_EMACECAP, nullptr},  // ENGLISH_MACEMAN_CAPTAIN -> EMACECAP
    {d::s_ENGLISH_MACEMAN, d::s_EMACE, nullptr},  // ENGLISH_MACEMAN -> EMACE
    {d::s_LORD_TALBOT, d::s_TALBOT, nullptr},  // LORD_TALBOT -> TALBOT
    {d::s_PRIEST_01, d::s_C_PRIEST, nullptr},  // PRIEST_01 -> C_PRIEST
    {d::s_POTION_SHOPKEEPER, d::s_SHOPPE, nullptr},  // POTION_SHOPKEEPER -> SHOPPE
    {d::s_POTION_SHOPKEEPER_2, d::s_SHOPPE, nullptr},  // POTION_SHOPKEEPER_2 -> SHOPPE
    {d::s_WEAPON_SHOPKEEPER, d::s_BLACKSM, nullptr},  // WEAPON_SHOPKEEPER -> BLACKSM
    {d::s_WEAPON_SHOPKEEPER_2, d::s_BLACKSM, nullptr},  // WEAPON_SHOPKEEPER_2 -> BLACKSM
    {d::s_william_de_la_pole, d::s_WILLIAM, nullptr},  // william_de_la_pole -> WILLIAM
    {d::s_ALEXANDER, d::s_ALEXDLP, nullptr},  // ALEXANDER -> ALEXDLP
    {d::s_John_de_la_pole, d::s_JOHNDLP, nullptr},  // John_de_la_pole -> JOHNDLP
    {d::s_Luxembourg, d::s_LUX, nullptr},  // Luxembourg -> LUX
    {d::s_MATTHEWGOUGH, d::s_GOUGH, nullptr},  // MATTHEWGOUGH -> GOUGH
    {d::s_MARTINGOUGH, d::s_GOUGH, nullptr},  // MARTINGOUGH -> GOUGH
    {d::s_Perrinet, d::s_GRESSART, nullptr},  // Perrinet -> GRESSART
    {d::s_Philip_the_good, d::s_phillp, nullptr},  // Philip_the_good -> phillp
    {d::s_WANDOMME, d::s_WANDOMME, nullptr},  // WANDOMME -> WANDOMME
    {d::s_HUDLEG, d::s_WANDOMME, nullptr},  // HUDLEG -> WANDOMME
    {d::s_CITIZEN_01, d::s_CITI1, nullptr},  // CITIZEN_01 -> CITI1
    {d::s_CITIZEN_01_2, d::s_CITI1, nullptr},  // CITIZEN_01_2 -> CITI1
    {d::s_CITIZEN_02B, d::s_CITI1A, nullptr},  // CITIZEN_02B -> CITI1A
    {d::s_CITIZEN_02B_2, d::s_CITI1A, nullptr},  // CITIZEN_02B_2 -> CITI1A
    {d::s_CITIZEN_02A, d::s_CITI2B, nullptr},  // CITIZEN_02A -> CITI2B
    {d::s_CITIZEN_02A_2, d::s_CITI2B, nullptr},  // CITIZEN_02A_2 -> CITI2B
    {d::s_CITIZEN_03, d::s_CITI3A, nullptr},  // CITIZEN_03 -> CITI3A
    {d::s_CITIZEN_03_2, d::s_CITI3A, nullptr},  // CITIZEN_03_2 -> CITI3A
    {d::s_CITIZEN_04A, d::s_CITI4, nullptr},  // CITIZEN_04A -> CITI4
    {d::s_CITIZEN_04A_2, d::s_CITI4, nullptr},  // CITIZEN_04A_2 -> CITI4
    {d::s_CITIZEN_04B, d::s_CITI4B, nullptr},  // CITIZEN_04B -> CITI4B
    {d::s_CITIZEN_04B_2, d::s_CITI4B, nullptr},  // CITIZEN_04B_2 -> CITI4B
    {d::s_CHARLIE7, d::s_CHARLES_9d4790, nullptr},  // CHARLIE7 -> CHARLES
    {d::s_PRISONER_01, d::s_CITI1, nullptr},  // PRISONER_01 -> CITI1
    {d::s_COUNCIL_01, d::s_CITI1, nullptr},  // COUNCIL_01 -> CITI1
    {d::s_ENGLISH_ARCHER_CAPTAIN, d::s_EARCCAP, nullptr},  // ENGLISH_ARCHER_CAPTAIN -> EARCCAP
    {d::s_ENGLISH_EX_ARCHER_CAPTAIN, d::s_EARCCAP, nullptr},  // ENGLISH_EX_ARCHER_CAPTAIN -> EARCCAP
};

struct H {
    // 0x00639b40
    void set_model_prefix() {
        if (joan::orig::arg::as<u32>(joan::orig::as_name(this)->length()) == 0) return;  // 0x0045e110
        c_model_prefix.hit();
        char* const dst = jo::self<jo::Dialogue>(this)->portrait;
        for (const Prefix& p : kPrefixes) {
            if (stricmp_(joan::orig::arg::as<const char*>(joan::orig::as_string(this)->c_str()), str(p.name)) != 0) continue;  // 0x00417d90
            if (p.armor == nullptr) {
                strcpy_(dst, str(p.prefix));
                return;
            }
            const Armor& a = *p.armor;
            const u32 key = joan::orig::arg::as<u32>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(str(a.lookup))));  // 0x005270b0
            const u32 id = joan::orig::arg::as<u32>(joan::orig::as_units(obj(d::g_units))->find_by_3b(joan::orig::arg::i32(key)));            // 0x0051fd50
            if (id == 0) {
                strcpy_(dst, str(a.fallback));
                return;
            }
            u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
            const i16 level = jo::as_unit(unit)->stats[5];
            for (int k = 0; k < 3; ++k) {
                u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(level))));  // 0x00425ce0
                if (stricmp_(jo::as_item_def(rec)->name, str(a.lv[k])) == 0) {
                    strcpy_(dst, str(a.prefix[k]));
                    return;
                }
            }
            return;
        }
        strcpy_(dst, str(d::s_FLINF));  // 'FLINF'
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_07", name, addr, fn)
B("Unit::set_model_prefix", 0x00639b40, &H::set_model_prefix);
#undef B

}  // namespace game_b106_07
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

using joan::call::thiscall;
void* const kSnd = obj(d::g_sfx_player);

JOAN_COUNTER(c_panel_release, "Panel::release_63b390");

struct H {
    // ------------------------------------------------------------------------ 0x0063b390
    void panel_release() {
        c_panel_release.hit();
        if (jo::self<jo::Dialogue>(this)->sub_lines != nullptr) {
            for (i32 i = joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Dialogue>(this)->sub_lines))->size(); i > 0; --i) {  // 0x00417ad0
                void* const e = joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Dialogue>(this)->sub_lines))->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
                if (e != nullptr) joan::orig::as_dialogue_line(e)->destroy(1u);                  // 0x0063b540
            }
            void* const list = jo::view<void*>(jo::self<jo::Dialogue>(this)->sub_lines);
            if (list != nullptr) joan::orig::as_dynarray(list)->destroy(1u);  // 0x00463fc0
            jo::self<jo::Dialogue>(this)->sub_lines = nullptr;
        }
        if (jo::self<jo::Dialogue>(this)->speech != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Dialogue>(this)->speech);
            if (p != nullptr) joan::orig::as_sound_res_table(p)->destroy(1u);  // 0x0063b590
            jo::self<jo::Dialogue>(this)->speech = nullptr;
        }
        if (jo::self<jo::Dialogue>(this)->i16_340 != 0 && jo::view<i8>(jo::self<jo::Dialogue>(this)->u8_342) != 0) {
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_sounds(kSnd)->has_kind(static_cast<i32>(jo::self<jo::Dialogue>(this)->i16_340)))) != 0)  // 0x004cfee0
                joan::orig::as_scenario(kSnd)->trigger_slot(static_cast<i32>(jo::self<jo::Dialogue>(this)->i16_340));                     // 0x004cecd0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("Panel::release_63b390", 0x0063b390, &H::panel_release);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

using joan::call::thiscall;

struct H {
    // -------------------------------------------------------------- 0x0063b540
    void* scalar_dtor_63b540(u32 flags) {
        joan::orig::as_dialogue_line(this)->release_63e270();  // 0x0063e270
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("Obj::scalar_dtor_63b540", 0x0063b540, &H::scalar_dtor_63b540);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b101_12.cpp (game/b101_12)
namespace {
namespace game_b101_12 {

using joan::call::thiscall;

struct H {
    // 0x0063b590
    void* scalar_dtor_63b590(u32 flags) {
        joan::orig::as_speech(this)->destruct();  // 0x004d15f0
        if ((flags & 1u) != 0) op_delete(this);
        return this;
    }
};

#define R12(name, addr, fn) JOAN_REPLACE("game/b101_12", name, addr, fn)
R12("Obj::scalar_dtor_63b590", 0x0063b590, &H::scalar_dtor_63b590);
#undef R12

}  // namespace game_b101_12
}  // namespace

// ---------------------------------------- from modules/game_b107_09.cpp (game/b107_09)
namespace {
namespace game_b107_09 {

using joan::call::thiscall;
JOAN_COUNTER(c_slot_update, "Slot::update");

struct H {
    // 0x0063b5e0
    void slot_update() {
        c_slot_update.hit();
        if (global<u32>(d::g_a5d38b) != 0x437f0000u || global<u32>(d::g_a5d38f) != 2u) {
            return;
        }
        if (jo::self<jo::Dialogue>(this)->i16_340 != 0 && jo::view<i8>(jo::self<jo::Dialogue>(this)->u8_342) == 0) {
            joan::orig::as_unit(obj(d::g_sfx_player))->play_sound(static_cast<std::int32_t>(jo::self<jo::Dialogue>(this)->i16_340));  // 0x004cf0f0
            jo::self<jo::Dialogue>(this)->u8_342 = 1;
        }
        if (jo::self<jo::Dialogue>(this)->mode != 1) {
            return;
        }
        if (jo::self<jo::Dialogue>(this)->speech == nullptr) {
            void* m1 = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::FileTxt)));  // operator new (0x19a on x86)
            void* first = nullptr;
            if (m1 != nullptr) {
                first = joan::orig::arg::as<void*>(joan::orig::as_file_txt(m1)->construct_mem(joan::orig::arg::str(jo::self<jo::Dialogue>(this)->speech_text), static_cast<std::int32_t>(jo::self<jo::Dialogue>(this)->speech_len)));  // 0x00589780
            }
            void* m2 = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Speech)));  // 0x138 on x86
            void* second = nullptr;
            if (m2 != nullptr) {
                second = joan::orig::arg::as<void*>(joan::orig::as_speech(m2)->zero());  // 0x004d15b0
            }
            jo::view<void*>(jo::self<jo::Dialogue>(this)->speech) = second;
            joan::orig::as_speech(jo::view<void*>(jo::self<jo::Dialogue>(this)->speech))->load_header(joan::orig::arg::mptr(first));  // 0x004d1770
            if (jo::view<u32>(jo::self<jo::Dialogue>(this)->unit_id) != 0) {
                void* p = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Dialogue>(this)->unit_id)));  // 0x0041b3c0
                jo::Actor* const v = jo::as_unit(p)->actor;
                joan::orig::as_speech(jo::view<void*>(jo::self<jo::Dialogue>(this)->speech))->set_4(v);  // 0x004d1af0
            }
        }
        if (jo::self<jo::Dialogue>(this)->done != 0) {
            return;
        }
        if (joan::orig::arg::as<u8>(joan::orig::as_speech(jo::view<void*>(jo::self<jo::Dialogue>(this)->speech))->mode_2_or_3()) == 0) {  // 0x004d1b60
            joan::orig::as_speech(jo::view<void*>(jo::self<jo::Dialogue>(this)->speech))->set_state_2();                          // 0x004d1b20
        }
        joan::orig::as_speech(jo::view<void*>(jo::self<jo::Dialogue>(this)->speech))->update();  // 0x004d1be0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_09", name, addr, fn)
B("Slot::update", 0x0063b5e0, &H::slot_update);
#undef B

}  // namespace game_b107_09
}  // namespace

// ---------------------------------------- from modules/game_b116_14.cpp (game/b116_14)
namespace {
namespace game_b116_14 {

// Not joan::call::thiscall: a __thiscall function pointer, not a pointer to member (a class-type R is returned
// as a free function returns it); the target is an fn:: name or a computed address (see src/call.hpp).
template <class R = void, class T, class... A>
R thiscall(T address, const void* self, A... args) {
    return reinterpret_cast<R(JOAN_THISCALL*)(void*, A...)>(joan::fn::target(address))(const_cast<void*>(self), args...);
}
i32 sx(i16 v) { return static_cast<i32>(v); }
using joan::orig::helpers::wadd;
using joan::orig::helpers::wsub;
float u32_to_float(u32 v) { return static_cast<float>(static_cast<double>(v)); }  // fild qword (v, 0); fstp dword

// x87 arithmetic at PC24 with the x87's own exponent range: the exact result hi + lo (|lo| <= half
// an ulp of hi) rounded to a 24-bit significand WITHOUT float overflow / gradual underflow (a
// register value; only the final fstp rounds to the float format). Fpu{true} rounds to the float
// format at every step, which differs when an intermediate leaves the float range (e.g. a large
// tick * dt in Dialogue::draw that the following / 1000 brings back). hi is finite and normal for
// every operand combination here (|values| within 2^-200..2^200), NaN / inf pass through.
double pc24(double hi, double lo) {
    if (hi == 0.0 || !(hi - hi == 0.0)) return hi;  // zero, inf, NaN
    std::uint64_t bits;
    std::memcpy(&bits, &hi, sizeof bits);
    const i32 e = static_cast<i32>((bits >> 52) & 0x7ff) - 1023;
    auto pow2 = [](i32 n) {
        const std::uint64_t b = static_cast<std::uint64_t>(n + 1023) << 52;
        double d;
        std::memcpy(&d, &b, sizeof d);
        return d;
    };
    const double down = pow2(-e), up = pow2(e);  // exact scalings into [1, 2) and back
    return joan::hook::x87::round_to_float(hi * down, lo * down) * up;
}
double mul24(double a, double b) {
    const double p = a * b;
    return pc24(p, joan::fp::fma_exact(a, b, -p));
}
double div24(double a, double b) {
    const double q = a / b;
    return pc24(q, b != 0.0 ? joan::fp::fma_exact(-q, b, a) / b : 0.0);
}
double add24(double a, double b) {
    const double s = a + b;
    const double bb = s - a;
    return pc24(s, (a - (s - bb)) + (b - bb));
}

constexpr u32 kOne = 0x3f800000u;  // 1.0f pushed as an immediate
constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite drawer
constexpr img::Addr kFont = d::g_font_goudy14_22_white;     // dialogue font

JOAN_COUNTER(c_dialogue, "Dialogue::draw");
JOAN_COUNTER(c_layout, "Dialogue::layout_text");

// 0x0042a260 colour ctor (thiscall (r, g, b, a), ret 0x10) into `slot`; returns the dword.
u32 make_colour(u32* slot, u32 r, u32 g, u32 b, u32 a) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(r, g, b, a));  // 0x0042a260
}

// Dialogue::draw's icon sprite: 0x00633500 (ret 0x44) at (0x1c, 0x2b1).
void icon(const char* name, u32 colour) {
    joan::orig::as_sprite_queue(obj(kSprites))->draw_named(0x1c, 0x2b1, 5u, joan::orig::arg::str(name), joan::orig::arg::mptr(global<joan::orig::BmpRes*>(d::g_a55ab6)), joan::orig::arg::u32(colour), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}

// 0x0063da60's line measurement: 0x00624f00(text, -1, width, 0, 1.0f) then height = 0x006253f0(4,
// 1.0f) into +0x32e; returns the 16-bit increment (height + 4). `hi` = the upper half of the
// register the original pushes the width word with.
i16 measure(u8* line, u32 hi) {
    const u32 width = (hi & 0xffff0000u) | static_cast<u16>(jo::as_dialogue(line)->w);
    const char* text = joan::orig::arg::as<const char*>(joan::orig::as_string(&jo::as_dialogue(line)->text)->c_str());  // 0x00417d90
    joan::orig::as_font(obj(kFont))->measure(joan::orig::arg::str(text), -1, joan::orig::arg::i16(width), 0, joan::orig::arg::f32(kOne));  // 0x00624f00
    jo::as_dialogue(line)->h = joan::orig::arg::as<i16>(joan::orig::as_font(obj(kFont))->text_height(joan::orig::arg::i16(i32{4}), joan::orig::arg::f32(kOne)));  // 0x006253f0
    return static_cast<i16>(wadd(sx(jo::as_dialogue(line)->h), 4));
}

struct H {
    // -----------------------------------------------------------------------------------------
    // 0x0063b830 Dialogue::draw(x, y, float dt, unused)
    void dialogue_draw(u32 xa, u32 ya, u32 dt_bits, u32) {
        joan::canvas::ScreenScope canvas_scope(0x0063b830);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        if (global<i32>(d::g_a5d38f) != 2 || global<u32>(d::g_a5d38b) != 0x437f0000u) return;
        c_dialogue.hit();
        const i32 x = sx(static_cast<i16>(xa));
        const i32 y = sx(static_cast<i16>(ya));
        if (jo::self<jo::Dialogue>(this)->mode == 1 && jo::self<jo::Dialogue>(this)->done == 0) {
            const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_speech(jo::view<void*>(jo::self<jo::Dialogue>(this)->speech))->text());  // 0x004d2910
            joan::orig::as_string(&jo::self<jo::Dialogue>(this)->text)->assign(joan::orig::arg::str(text));     // 0x005a1440
            const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(&jo::self<jo::Dialogue>(this)->text)->c_str());  // 0x00417d90
            std::uintptr_t string8 = 0;                                                     // [ebp-0x8]
            joan::orig::as_text_cursor(&string8)->construct(joan::orig::arg::str(s));                              // 0x0058b9c0
            jo::self<jo::Dialogue>(this)->shown = u32_to_float(joan::orig::arg::as<u32>(joan::orig::as_text_cursor(&string8)->length()));  // 0x0058c190
            jo::self<jo::Dialogue>(this)->done = joan::orig::arg::as<u8>(joan::orig::as_speech(jo::view<void*>(jo::self<jo::Dialogue>(this)->speech))->is_state4());  // 0x004d1bb0
        }
        u32 colour_c = 0;                                // [ebp-0xc], constructed and unused
        joan::orig::as_color(&colour_c)->Color_dword(joan::orig::arg::u32(i32{-1}));  // 0x005e9640
        if (joan::orig::arg::as<u32>(joan::orig::as_name(this)->length()) != 0) {     // 0x0045e110
            u32 white = 0;  // [ebp-0x10]
            joan::orig::as_colour(&white)->construct(0xffu, 0xffu, 0xffu, 0xffu);  // 0x0042a260
            icon(str(d::s_ICON_1), white);  // "ICON_1"
            icon(str(d::s_ICON_2), white);  // "ICON_2"
            u32 white2 = 0;  // [ebp-0x14]
            joan::orig::as_colour(&white2)->construct(0xffu, 0xffu, 0xffu, 0xffu);  // 0x0042a260
            if (joan::crt::stricmp(joan::orig::arg::str(jo::self<jo::Dialogue>(this)->portrait), str(d::s_NONE_9da90c)) != 0)  // _stricmp(portrait, "!NONE")
                icon(jo::self<jo::Dialogue>(this)->portrait, white2);
        }
        if (static_cast<i16>(joan::hook::x87::ftol(jo::self<jo::Dialogue>(this)->shown)) > 0) {
            const bool white = jo::self<jo::Dialogue>(this)->visible != 0;
            const i32 shown = joan::hook::x87::ftol(jo::self<jo::Dialogue>(this)->shown);
            u32 slot = 0;  // [ebp-0x2c] / [ebp-0x34]
            const u32 colour = white ? make_colour(&slot, 0xffu, 0xffu, 0xffu, 0xffu)
                                     : make_colour(&slot, 0xffu, 0u, 0u, 0xffu);
            const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_string(&jo::self<jo::Dialogue>(this)->text)->c_str());  // 0x00417d90
            const u32 font = jo::self<jo::Dialogue>(this)->font;
            const i32 y2 = y + sx(joan::orig::arg::as<i16>(joan::orig::as_dialogue(this)->bottom()));  // 0x0063e2e0
            const i32 x2 = x + sx(joan::orig::arg::as<i16>(joan::orig::as_dialogue(this)->right()));  // 0x0063e2a0
            const i32 y1 = y + sx(jo::self<jo::Dialogue>(this)->y);
            const i32 x1 = x + sx(jo::self<jo::Dialogue>(this)->x);
            joan::orig::as_text(obj(kFont))->draw(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(font), text, joan::orig::arg::u32(colour), i32{4}, i32{1}, i32{1}, i32{1}, joan::orig::arg::i32(// 0x00626320
                          i32{2}), i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne), joan::orig::arg::i32(white ? i32{1} : i32{0}), joan::orig::arg::i32(shown));
        }
        if (jo::self<jo::Dialogue>(this)->mode != 0 || jo::self<jo::Dialogue>(this)->done != 0) return;
        const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(&jo::self<jo::Dialogue>(this)->text)->c_str());  // 0x00417d90
        std::uintptr_t string18 = 0;                                           // [ebp-0x18]
        joan::orig::as_text_cursor(&string18)->construct(joan::orig::arg::str(s));                    // 0x0058b9c0
        float dt;
        std::memcpy(&dt, &dt_bits, sizeof dt);
        double v = mul24(static_cast<double>(global<u32>(d::g_frame_elapsed_ms)), static_cast<double>(dt));  // fild qword; fmul
        v = div24(v, static_cast<double>(global<float>(d::flt_1000)));  // fdiv 1000.0f
        v = add24(v, static_cast<double>(jo::self<jo::Dialogue>(this)->shown));     // fadd [+0x330]
        jo::self<jo::Dialogue>(this)->shown = static_cast<float>(v);
        const u32 shown = static_cast<u16>(joan::hook::x87::ftol(jo::self<jo::Dialogue>(this)->shown));
        if (shown < joan::orig::arg::as<u32>(joan::orig::as_text_cursor(&string18)->length())) return;  // 0x0058c190
        jo::self<jo::Dialogue>(this)->shown = u32_to_float(joan::orig::arg::as<u32>(joan::orig::as_text_cursor(&string18)->length()));
        jo::self<jo::Dialogue>(this)->done = 1;
    }

    // -----------------------------------------------------------------------------------------
    // 0x0063c000 Dialogue::text_y
    u32 text_y() {
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_dialogue(this)->is_open()) & 0xffu;  // 0x0063e320
        if (r != 0) return r;
        jo::view<i32>(jo::self<jo::DialogueList>(this)->texture) = 0;
        jo::self<jo::DialogueList>(this)->x = 0x68;
        jo::self<jo::DialogueList>(this)->y = static_cast<i16>(wsub(joan::canvas::size_read(0x0063c035), 0x69));  // mov edx, [H]
        jo::self<jo::DialogueList>(this)->debug = 0;
        jo::self<jo::DialogueList>(this)->w = 0x37c;
        jo::self<jo::DialogueList>(this)->h = 0x69;
        jo::view<u32>(jo::self<jo::DialogueList>(this)->line_gap) = 0x42200000u;  // 40.0f
        jo::self<jo::DialogueList>(this)->open = 1;
        return joan::call::register_low_bits(this);
    }

    // -----------------------------------------------------------------------------------------
    // 0x0063da60 Dialogue::layout_text
    i32 count() { return joan::orig::arg::as<i32>(joan::orig::as_streams(this)->count()); }  // 0x0063d210
    bool skip(i32 i) { return joan::orig::arg::as<i32>(joan::orig::as_list(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->empty_at_g(joan::orig::arg::i32(i))) != 0; }  // 0x0063e3f0
    u8* line(i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_streams(this)->get(joan::orig::arg::i32(i))); }  // 0x0063d1e0

    u32 layout_text() {
        c_layout.hit();
        jo::self<jo::DialogueList>(this)->w = 0x37c;
        jo::self<jo::DialogueList>(this)->h = 0x69;
        jo::self<jo::DialogueList>(this)->y = static_cast<i16>(wsub(joan::canvas::size_read(0x0063da87), 0x69));  // mov edx, [H]
        for (i32 i = 1; i <= count(); ++i) {
            if (skip(i)) continue;
            u8* const c = line(i);
            jo::as_dialogue(c)->w = static_cast<i16>(sx(jo::self<jo::DialogueList>(this)->w) - 0x1e - sx(jo::as_dialogue(c)->x) + 1);
            jo::as_dialogue(c)->x = 0xf;
            jo::as_dialogue(c)->y = 0x1e;
            if (jo::as_dialogue(c)->sub_lines == nullptr) continue;
            for (i32 j = joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(c)->sub_lines))->size(); j > 0; --j) {  // 0x00417ad0
                u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(c)->sub_lines))->ptr_at(joan::orig::arg::i32(j)));  // 0x0041b3f0
                jo::as_dialogue(p)->w = static_cast<i16>(sx(jo::self<jo::DialogueList>(this)->w) - 0x32);
            }
        }
        for (;;) {
            const i16 limit = static_cast<i16>(sx(jo::self<jo::DialogueList>(this)->h) - 0x20);
            i16 sum = 0;
            for (i32 i = 1; i <= count(); ++i) {
                if (skip(i)) continue;
                u8* const c = line(i);
                const i16 top = jo::as_dialogue(c)->y;
                const u32 len = joan::orig::arg::as<u32>(joan::orig::as_name(&jo::as_dialogue(c)->text)->length());  // 0x0045e110
                if (len != 0) sum = static_cast<i16>(static_cast<u16>(sum) + static_cast<u16>(measure(c, len)));
                if (jo::as_dialogue(c)->sub_lines == nullptr) continue;
                for (i32 j = 1; j <= sx(joan::orig::arg::as<i16>(joan::orig::as_dialogue(c)->line_count())); ++j) {  // 0x0063e240
                    u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(c)->sub_lines))->ptr_at(joan::orig::arg::i32(j)));  // 0x0041b3f0
                    if (jo::as_dialogue(p)->visible == 0 && jo::self<jo::DialogueList>(this)->debug == 0) continue;
                    jo::as_dialogue(p)->y = static_cast<i16>(sx(top) + sx(sum));
                    sum = static_cast<i16>(static_cast<u16>(sum) +
                                           static_cast<u16>(measure(p, static_cast<u32>(sx(sum)))));
                }
            }
            if (sx(sum) <= sx(limit)) return static_cast<u32>(sx(limit));
            if (sx(jo::self<jo::DialogueList>(this)->w) < wsub(joan::canvas::size_read(0x0063dd5e), 0x64)) {  // mov eax, [W]
                jo::self<jo::DialogueList>(this)->w = static_cast<i16>(static_cast<u16>(jo::self<jo::DialogueList>(this)->w) + 0xau);
                for (i32 i = 1; i <= count(); ++i) {
                    if (skip(i)) continue;
                    u8* const c = line(i);
                    jo::as_dialogue(c)->w = static_cast<i16>(static_cast<u16>(jo::as_dialogue(c)->w) + 0xau);
                    if (jo::as_dialogue(c)->sub_lines == nullptr) continue;
                    for (i32 j = joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(c)->sub_lines))->size(); j > 0; --j) {  // 0x00417ad0
                        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(c)->sub_lines))->ptr_at(joan::orig::arg::i32(j)));  // 0x0041b3f0
                        jo::as_dialogue(p)->w = static_cast<i16>(static_cast<u16>(jo::as_dialogue(p)->w) + 0xau);
                    }
                }
            } else {
                jo::self<jo::DialogueList>(this)->y = static_cast<i16>(static_cast<u16>(jo::self<jo::DialogueList>(this)->y) - 0xau);
                jo::self<jo::DialogueList>(this)->h = static_cast<i16>(static_cast<u16>(jo::self<jo::DialogueList>(this)->h) + 0xau);
            }
        }
    }
};

JOAN_REPLACE("game/b116_14", "Dialogue::draw", 0x0063b830, &H::dialogue_draw);
JOAN_CANVAS_SELF(0x0063b830);  // ScreenScope in the body (no thunk wrapper)
JOAN_REPLACE("game/b116_14", "Dialogue::text_y", 0x0063c000, &H::text_y);
JOAN_REPLACE("game/b116_14", "Dialogue::layout_text", 0x0063da60, &H::layout_text);

}  // namespace game_b116_14
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
namespace {
namespace game_b102_12 {

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------ 0x0063bd70
    i32 next_same(i32 start) {
        u8* const ref = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Team>(this)->sub_lines))->ptr_at(1));  // 0x0041b3f0
        for (i32 i = start; i <= static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_dialogue(this)->line_count())); ++i) {  // 0x0063e240
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Team>(this)->sub_lines))->ptr_at(joan::orig::arg::i32(i)));
            if (static_cast<i32>(jo::as_dialogue(e)->y) == static_cast<i32>(jo::as_dialogue(ref)->y)) return i;
        }
        return 0;
    }

    // ------------------------------------------------------------------ 0x0063be10
    i32 prev_same(i32 start) {
        u8* const ref = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Team>(this)->sub_lines))->ptr_at(1));  // 0x0041b3f0
        for (i32 i = start; i > 0; --i) {
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Team>(this)->sub_lines))->ptr_at(joan::orig::arg::i32(i)));
            if (static_cast<i32>(jo::as_dialogue(e)->y) == static_cast<i32>(jo::as_dialogue(ref)->y)) return i;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("Team::next_same", 0x0063bd70, &H::next_same);
B("Team::prev_same", 0x0063be10, &H::prev_same);
#undef B

}  // namespace game_b102_12
}  // namespace

// ---------------------------------------- from modules/game_b103_13.cpp (game/b103_13)
namespace {
namespace game_b103_13 {

using joan::call::thiscall;

void* const kDialogs = obj(d::g_dialogs);

struct H {
    u32 trigger_63beb0() {  // 0x0063beb0
        if (joan::orig::arg::as<char>(joan::orig::as_bytes(this)->get(0)) == 0) return 0;  // 0x0063bf10
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_string(this)->c_str());  // 0x00417d90
        return joan::orig::arg::as<u32>(joan::orig::as_font(kDialogs)->measure(joan::orig::arg::str(p), -1, 0, 0, joan::orig::arg::f32(0x3f800000u)));  // 0x00624f00
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_13", name, addr, fn)
B("Dlg::trigger_63beb0", 0x0063beb0, &H::trigger_63beb0);
#undef B

}  // namespace game_b103_13
}  // namespace

// ---------------------------------------- from modules/game_b93_04.cpp (game/b93_04)
namespace {
namespace game_b93_04 {

using joan::call::thiscall;
JOAN_COUNTER(c_release_all, "List::release_all");

u32 u(const void* p) { return joan::call::register_low_bits(p); }  // eax residue: Bytes::get callers read al only

struct H {
    // ------------------------------------------------------------------ 0x0063bf10
    u32 byte_at(i32 i) {
        const u8* const p = reinterpret_cast<const u8*>(this) + i;
        return (u(p) & 0xffffff00u) | static_cast<u32>(*p);  // al = the byte, the rest of eax = the address (x86)
    }

    // ------------------------------------------------------------------ 0x0063c800
    i32 release_all() {
        c_release_all.hit();
        i32 i = joan::orig::arg::as<i32>(joan::orig::as_streams(this)->count());  // 0x0063d210
        for (; i > 0; --i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::view<void*>(jo::self<jo::Streams>(this)->lines))->empty_at_g(joan::orig::arg::i32(i))) != 0) continue;  // 0x0063e3f0
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_streams(this)->get(joan::orig::arg::i32(i)));           // 0x0063d1e0
            if (jo::as_dialogue(e)->i16_340 != 0) {
                joan::orig::as_scenario(obj(d::g_sfx_player))->trigger_slot(static_cast<i32>(jo::as_dialogue(e)->i16_340));  // 0x004cecd0
            }
        }
        return i;
    }

    // ------------------------------------------------------------------ 0x0063e420
    i32 flag_66d() { return static_cast<i32>(jo::view<i8>(jo::self<jo::Input>(this)->has_joy_event)); }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("Bytes::get", 0x0063bf10, &H::byte_at);
B93("List::release_all", 0x0063c800, &H::release_all);
B93("Obj::flag_66d", 0x0063e420, &H::flag_66d);
#undef B93

}  // namespace game_b93_04
}  // namespace

// ---------------------------------------- from modules/game_b102_13.cpp (game/b102_13)
namespace {
namespace game_b102_13 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0063bf40
    void* init_63bf40() {
        joan::orig::as_widget(&jo::self<jo::DialogueList>(this)->debug_button)->ctor_620ee0();  // 0x00620ee0
        std::memset(static_cast<void*>(this), 0, sizeof(jo::DialogueList));  // memset (0x36a bytes on x86)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_13", name, addr, fn)
B("Obj63bf40::init", 0x0063bf40, &H::init_63bf40);
#undef B

}  // namespace game_b102_13
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
namespace {
namespace game_b108_05 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0063bf90
    std::uintptr_t dtor_body_63bf90() {
        joan::orig::as_dialogue(this)->close();                   // 0x0063c0a0
        return joan::orig::arg::as<u32>(joan::orig::as_widget(&jo::self<jo::DialogueList>(this)->debug_button)->dtor_6211a0());  // 0x006211a0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("Obj::dtor_body_63bf90", 0x0063bf90, &H::dtor_body_63bf90);
#undef B

}  // namespace game_b108_05
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

using joan::call::thiscall;

JOAN_COUNTER(c_close_63c0a0, "Obj::close_63c0a0");

struct H {
    // ------------------------------------------------------------------ 0x0063c0a0
    void close_63c0a0() {
        c_close_63c0a0.hit();
        if (joan::orig::arg::as<u8>(joan::orig::as_dialogue(this)->is_open()) == 0) return;  // 0x0063e320
        if (jo::self<jo::DialogueList>(this)->texture != 0) {
            joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(jo::self<jo::DialogueList>(this)->texture));  // 0x0060a550
            jo::self<jo::DialogueList>(this)->texture = 0;
        }
        joan::orig::as_menu_button(&jo::self<jo::DialogueList>(this)->debug_button)->clear();  // 0x006217e0
        if (jo::self<jo::DialogueList>(this)->lines != nullptr) {
            joan::orig::as_dialogue_line_list(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->clear();  // 0x0063e060
            void* const p = jo::view<void*>(jo::self<jo::DialogueList>(this)->lines);
            if (p != nullptr) joan::orig::as_dialogue(p)->sdtor_63c170(joan::orig::arg::u32(i32{1}));  // 0x0063c170
            jo::self<jo::DialogueList>(this)->lines = nullptr;
        }
        jo::self<jo::DialogueList>(this)->open = 0;
    }

    // ------------------------------------------------------------------ 0x0063c650
    void refresh_63c650() {
        for (i32 i = 1; i <= joan::orig::arg::as<i32>(joan::orig::as_streams(this)->count()); ++i) {  // 0x0063d210
            if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->empty_at_g(joan::orig::arg::i32(i))) != 0) continue;  // 0x0063e3f0
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_streams(this)->get(joan::orig::arg::i32(i)));              // 0x0063d1e0
            joan::orig::as_slot(e)->update();                                      // 0x0063b5e0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("Obj::close_63c0a0", 0x0063c0a0, &H::close_63c0a0);
B("Obj::refresh_63c650", 0x0063c650, &H::refresh_63c650);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0063c170
    void* sdtor_63c170(u32 flags) {
        joan::orig::as_dialogue_line_list(this)->destruct();  // 0x0063dfd0
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Dialogue::sdtor_63c170", 0x0063c170, &H::sdtor_63c170);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using joan::orig::helpers::new_;  // operator new

using joan::call::thiscall;

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_panel_update, "Panel::update_buttons");

struct H {
    // ------------------------------------------------------------------ 0x0063c1c0
    static float u32_to_float(u32 v) { return static_cast<float>(static_cast<double>(v)); }  // fild qword {v, 0}
    static void set_value_330(u8* e) {
        const char* s = joan::orig::arg::as<const char*>(joan::orig::as_string(&jo::as_dialogue(e)->text)->c_str());  // 0x00417d90
        std::uintptr_t holder = 0;  // 4-byte object [ebp-0x24] / [ebp-0x30]
        joan::orig::as_text_cursor(&holder)->construct(joan::orig::arg::str(s));              // 0x0058b9c0
        const u32 v = joan::orig::arg::as<u32>(joan::orig::as_text_cursor(&holder)->length());     // 0x0058c190
        jo::as_dialogue(e)->shown = u32_to_float(v);
    }
    i32 update_buttons(u32 flag) {
        jo::self<jo::DialogueList>(this)->busy = 0;
        i32 result = 0;  // [ebp-0x14]
        if ((flag & 0xff) != 0) return result;
        c_panel_update.hit();
        jo::view<u32>(jo::self<jo::DialogueList>(this)->line_gap) = 0x42200000u;  // 40.0f
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(&jo::self<jo::DialogueList>(this)->debug_button)->mouse(0, 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0) {  // 0x006218c0
            jo::self<jo::DialogueList>(this)->debug = static_cast<u8>(jo::self<jo::DialogueList>(this)->debug ^ 1);
            joan::orig::as_widget(&jo::self<jo::DialogueList>(this)->debug_button)->set_texts(joan::orig::arg::str(jo::self<jo::DialogueList>(this)->debug != 0 ? str(d::s_Debug_Mode_On) : str(d::s_Debug_Mode_Off)), joan::orig::arg::str(0));  // 0x00623c40
            joan::orig::as_dialogue(this)->layout_text();  // 0x0063da60
            result = 1;
        }
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_streams(this)->count()); i > 0; --i) {  // 0x0063d210
            if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->empty_at_g(joan::orig::arg::i32(i))) != 0) continue;  // 0x0063e3f0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_streams(this)->get(joan::orig::arg::i32(i)));              // 0x0063d1e0
            if (jo::as_dialogue(e)->visible == 0 && jo::self<jo::DialogueList>(this)->debug == 0) continue;
            bool hit = false;
            if (joan::orig::arg::as<i32>(joan::orig::as_parser(kInput)->value(0)) != 0) {  // 0x00594700
                hit = true;
            } else {
                if (joan::orig::arg::as<i32>(joan::orig::as_input(kInput)->flag_6aa()) != 0 &&  // 0x00631770
                    (global<i32>(d::g_key) == 0x20 || global<i32>(d::g_key) == 0xd) &&
                    joan::orig::arg::as<i32>(joan::orig::as_key(kInput)->repeat_count()) == 1)  // 0x00595fb0
                    hit = true;
                if (!hit) {
                    if (joan::orig::arg::as<i32>(joan::orig::as_input(kInput)->flag_66d()) == 0) continue;  // 0x0063e420
                    const i8 k = global<i8>(d::g_a51d6b);
                    if (!(k == 0 || k == 1 || k == 2 || k == 3)) continue;
                }
            }
            jo::self<jo::DialogueList>(this)->busy = 1;
            if (jo::as_dialogue(e)->mode != 0) {
                if (jo::as_dialogue(e)->speech == nullptr) {
                    void* const p = new_(static_cast<std::uint32_t>(sizeof(jo::FileTxt)));  // 0x19a on x86
                    void* const model = p != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_file_txt(p)->construct_mem(joan::orig::arg::str(jo::as_dialogue(e)->speech_text), static_cast<i32>(jo::as_dialogue(e)->speech_len)))
                                                     : nullptr;  // 0x00589780
                    void* const q = new_(static_cast<std::uint32_t>(sizeof(jo::Speech)));  // 0x138 on x86
                    jo::view<void*>(jo::as_dialogue(e)->speech) = q != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_speech(q)->zero()) : nullptr;  // 0x004d15b0
                    joan::orig::as_speech(jo::view<void*>(jo::as_dialogue(e)->speech))->load_header(joan::orig::arg::mptr(model));  // 0x004d1770
                    if (jo::view<u32>(jo::as_dialogue(e)->unit_id) != 0) {
                        u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_dialogue(e)->unit_id))));  // 0x0041b3c0
                        joan::orig::as_speech(jo::view<void*>(jo::as_dialogue(e)->speech))->set_4(jo::as_unit(u)->actor);  // 0x004d1af0
                    }
                }
                joan::orig::as_speech(jo::view<void*>(jo::as_dialogue(e)->speech))->set_state_3();  // 0x004d2930
            } else {
                set_value_330(e);
            }
            if (jo::as_dialogue(e)->sub_lines != nullptr) {
                for (i32 j = i; j <= static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_dialogue(e)->line_count())); ++j) {  // 0x0063e240
                    u8* const e2 = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(e)->sub_lines))->ptr_at(joan::orig::arg::i32(j)));  // 0x0041b3f0
                    set_value_330(e2);
                }
            }
            result = 1;
        }
        return result;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("Panel::update_buttons", 0x0063c1c0, &H::update_buttons);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

using joan::call::thiscall;
constexpr fn::Code kListCount = fn::List_count_417ad0;  // 0x00417ad0
constexpr fn::Code kListGet = fn::List_get_41b3f0;    // 0x0041b3f0
std::int32_t list_count(const void* list) { return thiscall<std::int32_t>(kListCount, list); }
void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

struct H {
    std::int32_t list_empty_at(std::int32_t i) { return list_get(this, i) == nullptr ? 1 : 0; }  // 0x0044cf80 / 0x00485760 / 0x0063e3f0
    void* list_get_wrap(std::int32_t i) { return list_get(this, i); }                   // 0x0043c450 / 0x00485730 / 0x0063e3c0
    void* sounds_get(std::int32_t i) { return joan::orig::arg::as<void*>(joan::orig::as_list(jo::view<void*>(jo::self<jo::Streams>(this)->lines))->get_g(joan::orig::arg::i32(i))); }  // 0x0063d1e0 -> 0x0063e3c0
    std::int32_t sounds_count() {                                                        // 0x0063d210
        if (jo::view<void*>(jo::self<jo::Streams>(this)->lines) == nullptr) return 0;
        return list_count(jo::view<void*>(jo::self<jo::Streams>(this)->lines));
    }
    // 0x0063c6d0 Streams::pause_all(on): the sounds (+0x340 id) of every stream.
    void streams_pause(char on) {
        void* const sounds = img::obj(d::g_sfx_player);
        for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_streams(this)->count()); i > 0; --i) {  // 0x0063d210
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(jo::view<void*>(jo::self<jo::Streams>(this)->lines))->empty_at_g(joan::orig::arg::i32(i))) != 0) continue;     // 0x0063e3f0
            void* const s = joan::orig::arg::as<void*>(joan::orig::as_streams(this)->get(joan::orig::arg::i32(i)));                       // 0x0063d1e0
            if (jo::as_dialogue(s)->i16_340 == 0) continue;
            if (static_cast<std::int32_t>(on) != 0)
                joan::orig::as_sounds(sounds)->pause(static_cast<std::int32_t>(jo::as_dialogue(s)->i16_340));  // 0x004ced90
            else
                joan::orig::as_sounds(sounds)->resume(static_cast<std::int32_t>(jo::as_dialogue(s)->i16_340));  // 0x00470c90
        }
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("List::empty_at_g", 0x0063e3f0, &H::list_empty_at);
W1("List::get_g", 0x0063e3c0, &H::list_get_wrap);
W1("Streams::get", 0x0063d1e0, &H::sounds_get);
W1("Streams::count", 0x0063d210, &H::sounds_count);
W1("Streams::pause_all", 0x0063c6d0, &H::streams_pause);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_ui7.cpp (game/ui7)
namespace {
namespace game_ui7 {

using joan::call::thiscall;
constexpr img::Addr kResources = d::g_texture_array; // resource group manager
constexpr img::Addr kMouse = d::g_mouse;

JOAN_COUNTER(c_dialogues, "DialogueList::draw");

// 0x0063b830 Dialogue::draw (x, y, arg, flags), ret 0x10 (hooked by canvas_hooks.cpp).
void draw_dialogue(void* dialogue, const void* list, std::uint8_t flags) {
    joan::orig::as_dialogue(dialogue)->draw(joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_dialogue_list(list)->x)), joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_dialogue_list(list)->y)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_dialogue_list(list)->line_gap)), joan::orig::arg::u32(flags));  // 0x0063b830
}

struct H {
    // 0x0063c8a0 DialogueList::draw. ret 4.
    void draw_dialogues(std::uint8_t flags) {
        c_dialogues.hit();
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_streams(this)->count()) == 0) {  // 0x0063d210
            if (jo::self<jo::DialogueList>(this)->last_time != 0 && global<std::uint32_t>(d::g_game_time_ms) - jo::self<jo::DialogueList>(this)->last_time > 0x2710u) {
                if (jo::self<jo::DialogueList>(this)->texture != 0) {
                    joan::orig::as_texture_manager(obj(kResources))->release(joan::orig::arg::mptr(jo::self<jo::DialogueList>(this)->texture));  // 0x0060a550
                    jo::self<jo::DialogueList>(this)->texture = 0;
                }
                jo::self<jo::DialogueList>(this)->last_time = 0;
            }
            return;
        }
        jo::self<jo::DialogueList>(this)->last_time = global<std::uint32_t>(d::g_game_time_ms);
        for (std::int32_t i = 1; i <= joan::orig::arg::as<std::int32_t>(joan::orig::as_streams(this)->count()); ++i) {  // 0x0063d210
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->empty_at_g(joan::orig::arg::i32(i))) != 0) continue;     // 0x0063e3f0
            std::uint8_t* const d = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_streams(this)->get(joan::orig::arg::i32(i)));      // 0x0063d1e0
            joan::orig::as_widget(&jo::self<jo::DialogueList>(this)->debug_button)->reset_child();                               // 0x0062b660
            if ((flags & 4) != 0) draw_dialogue(d, this, flags);
            if (jo::as_dialogue(d)->done != 0 && jo::as_dialogue(d)->sub_lines == nullptr && (flags & 1) != 0)
                joan::orig::as_dialogue(this)->blink_dot();  // 0x0063cc80
            if (jo::as_dialogue(d)->done == 0 || jo::as_dialogue(d)->sub_lines == nullptr) continue;
            for (std::int32_t j = 1; j <= static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_dialogue(d)->line_count())); ++j) {  // 0x0063e240
                std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(d)->sub_lines))->ptr_at(joan::orig::arg::i32(j)));  // 0x0041b3f0
                if (jo::as_dialogue(e)->visible == 0 && jo::self<jo::DialogueList>(this)->debug == 0) continue;
                if ((flags & 4) != 0) draw_dialogue(e, this, flags);
                if (jo::as_dialogue(e)->done == 0) return;
            }
            for (std::int32_t j = 1; j <= static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_dialogue(d)->line_count())); ++j) {  // 0x0063e240
                std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(d)->sub_lines))->ptr_at(joan::orig::arg::i32(j)));  // 0x0041b3f0
                if (jo::as_dialogue(e)->visible == 0 && jo::self<jo::DialogueList>(this)->debug == 0) continue;
                const std::int32_t by = static_cast<std::int32_t>(jo::self<jo::DialogueList>(this)->y);
                const std::int32_t y2 = by + static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_dialogue(e)->bottom()));  // 0x0063e2e0
                const std::int32_t bx = static_cast<std::int32_t>(jo::self<jo::DialogueList>(this)->x);
                const std::int32_t x2 = bx + static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_dialogue(e)->right()));  // 0x0063e2a0
                const std::int32_t y1 = static_cast<std::int32_t>(jo::self<jo::DialogueList>(this)->y) + static_cast<std::int32_t>(jo::as_dialogue(e)->y);
                const std::int32_t x1 = static_cast<std::int32_t>(jo::self<jo::DialogueList>(this)->x) + static_cast<std::int32_t>(jo::as_dialogue(e)->x) - 0x14;
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_point(obj(kMouse))->inside(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2))) != 0 && (flags & 4) != 0)  // 0x00593980
                    draw_dialogue(e, this, flags);
            }
        }
    }
};

#define U7(name, addr, fn) JOAN_REPLACE("game/ui7", name, addr, fn)
U7("DialogueList::draw", 0x0063c8a0, &H::draw_dialogues);
#undef U7

}  // namespace game_ui7
}  // namespace

// ---------------------------------------- from modules/game_b82_15.cpp (game/b82_15)
namespace {
namespace game_b82_15 {

using joan::call::thiscall;
void* const kTimer = obj(d::g_misc);

JOAN_COUNTER(c_dot, "DialogueList::blink_dot");

// Local objects of 0x0071bf10: two 0x191-byte strings and the 0x11c-byte file reader.
struct alignas(4) Str {
    char b[0x194];
};

struct H {
    // 0x0063e2a0. ret 0.
    std::int32_t dlg_right() {
        return static_cast<std::int32_t>(jo::self<jo::Dialogue>(this)->x) + static_cast<std::int32_t>(jo::self<jo::Dialogue>(this)->w) - 1;
    }
    // 0x0063e2e0. ret 0.
    std::int32_t dlg_bottom() {
        return static_cast<std::int32_t>(jo::self<jo::Dialogue>(this)->y) + static_cast<std::int32_t>(jo::self<jo::Dialogue>(this)->h) - 1;
    }
    // 0x0063e240. ret 0.
    std::uint32_t dlg_lines() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Dialogue>(this)->sub_lines))->size()); }  // 0x00417ad0

    // 0x0063cc80. ret 0.
    void blink_dot() {
        c_dot.hit();
        if (joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(kTimer)->get_time_591050()) - jo::self<jo::DialogueList>(this)->blink_time > 0x1f4u) {  // 0x00591050
            jo::self<jo::DialogueList>(this)->blink = static_cast<std::uint8_t>(jo::self<jo::DialogueList>(this)->blink + 1);
            if (static_cast<std::int32_t>(static_cast<std::int8_t>(jo::self<jo::DialogueList>(this)->blink)) > 3) jo::self<jo::DialogueList>(this)->blink = 1;
            jo::self<jo::DialogueList>(this)->blink_time = joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(kTimer)->get_time_591050());
        }
        Str s;
        joan::orig::as_string(&s)->construct(str(d::s_dot_9da940));  // 0x005a11a0 'dot'
        joan::orig::as_string(&s)->append(static_cast<std::int32_t>(static_cast<std::int8_t>(jo::self<jo::DialogueList>(this)->blink)));    // 0x005a15d0
        const std::int32_t a = static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_dialog(this)->bottom())) + 0x10;  // 0x0063e380
        const std::int32_t b = static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_dialog(this)->right())) - 0x18;  // 0x0063e340
        const std::int32_t c = static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_dialog(this)->bottom()));
        const std::int32_t d = static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_dialog(this)->right())) - 0x3c;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_point(kInput)->inside(joan::orig::arg::i32(d), joan::orig::arg::i32(c), joan::orig::arg::i32(b), joan::orig::arg::i32(a))) != 0) {  // 0x00593980
            joan::orig::as_string(&s)->append(str(d::s_hl));         // 0x005a1550 '_hl'
        } else {
            joan::orig::as_string(&s)->append(str(d::s_up));         // '_up'
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b82_15", name, addr, fn)
B("DialogueList::blink_dot", 0x0063cc80, &H::blink_dot);
B("Dialogue::bottom", 0x0063e2e0, &H::dlg_bottom);
B("Dialogue::right", 0x0063e2a0, &H::dlg_right);
B("Dialogue::line_count", 0x0063e240, &H::dlg_lines);
#undef B

}  // namespace game_b82_15
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using joan::call::thiscall;

void* const kObjects = obj(d::g_units);

JOAN_COUNTER(c_message, "Dialog::show_message_63cdc0");

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x0063cdc0
    i32 show_message_63cdc0(i32 id, void* number, u32 a3, u32 a4, const char* a5, u32 a6, u32 a7, u32 a8, u32 a9,
                            u32 a10) {
        c_message.hit();
        if (joan::orig::as_dynarray(kObjects)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
        if (jo::self<jo::DialogueList>(this)->lines == nullptr) {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::DynArrayB)));  // 0x2e on x86
            void* const q = mem != nullptr ? joan::orig::as_dialogue_line_list(mem)->construct() : nullptr;  // 0x0063dfa0
            jo::view<void*>(jo::self<jo::DialogueList>(this)->lines) = q;
            joan::orig::as_dialogue_line_list(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->init();  // 0x0063e040
        }
        // 0x358 in the x86 frame; a Dialogue is 0x357 bytes on x86, 0x363 on x64 (ctor_639af0 clears sizeof(Dialogue))
        alignas(alignof(void*)) u8 m[((sizeof(jo::Dialogue) > 0x358 ? sizeof(jo::Dialogue) : 0x358) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];
        std::memset(m, 0, sizeof m);
        joan::orig::as_dialogue_line(m)->construct();  // 0x00639af0
        joan::orig::as_unit(m)->set_model_prefix();         // 0x00639b40
        if (joan::orig::as_dynarray(kObjects)->empty_at(joan::orig::arg::i32(id)) == 0) {
            u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kObjects)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
            u8* const s = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(o)->type_id)));  // 0x0042d0a0
            joan::orig::as_string(m)->assign(joan::orig::arg::str(jo::as_unit_type(s)->name));  // 0x005a1440
        }
        jo::as_dialogue(m)->mode = a5 != nullptr ? 1 : 0;
        joan::orig::as_string(&jo::as_dialogue(m)->text)->assign(str(d::s_us_9cf774));
        u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kObjects)->ptr_at(joan::orig::arg::i32(id)));
        joan::orig::as_string(&jo::as_dialogue(m)->text)->append(joan::orig::arg::str(jo::as_unit(o)->name_));  // 0x005a1550
        joan::orig::as_string(&jo::as_dialogue(m)->text)->append(str(d::s_colon_us_sp));         // ":_ "
        joan::orig::as_string(&jo::as_dialogue(m)->text)->append_str(joan::orig::as_string(number));  // 0x005a14f0
        jo::as_dialogue(m)->speech = nullptr;
        jo::as_dialogue(m)->speech_text = const_cast<char*>(a5);  // +0x322
        jo::view<u16>(jo::as_dialogue(m)->speech_len) = static_cast<u16>(a6);
        jo::as_dialogue(m)->unit_id = id;
        jo::as_dialogue(m)->font = a4;
        jo::view<u16>(jo::as_dialogue(m)->i16_340) = static_cast<u16>(a3);
        if (static_cast<i16>(a7) == -1) {
            jo::as_dialogue(m)->x = 0xf;
            jo::as_dialogue(m)->y = 0x1e;
            jo::as_dialogue(m)->w = static_cast<i16>(jo::self<jo::DialogueList>(this)->w - 0x1e - jo::as_dialogue(m)->x + 1);
            jo::as_dialogue(m)->h = static_cast<i16>(jo::self<jo::DialogueList>(this)->h - 0x28 - jo::as_dialogue(m)->y + 1);
        } else {
            jo::view<u16>(jo::as_dialogue(m)->x) = static_cast<u16>(a7);
            jo::view<u16>(jo::as_dialogue(m)->y) = static_cast<u16>(a8);
            jo::as_dialogue(m)->w = static_cast<i16>(static_cast<i16>(a9) - static_cast<i16>(a7) + 1);
            jo::as_dialogue(m)->h = static_cast<i16>(static_cast<i16>(a10) - static_cast<i16>(a8) + 1);
        }
        jo::view<u32>(jo::as_dialogue(m)->shown) = 0;
        jo::as_dialogue(m)->done = 0;
        jo::as_dialogue(m)->visible = 1;
        jo::as_dialogue(m)->sub_lines = nullptr;
        jo::view<u32>(jo::as_dialogue(m)->choice) = 0;
        joan::orig::as_unit(m)->set_model_prefix();               // 0x00639b40
        joan::orig::as_dialogue(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->push_copy(joan::orig::arg::mptr(m));  // 0x0063e0d0
        joan::orig::as_dialogue(this)->layout_text();            // 0x0063da60
        const u32 g = global<u32>(d::g_now);
        const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_string(&jo::as_dialogue(m)->text)->c_str());  // 0x00417d90
        joan::orig::as_msg_log(obj(d::g_portraits))->add(joan::orig::arg::u32(global<i16>(d::g_mission)), joan::orig::arg::u32(static_cast<i32>(0)), text, joan::orig::arg::u32(g), joan::orig::arg::str(// 0x004b2d30
                 jo::as_dialogue(m)->portrait));
        const i32 r = joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->cursor();  // 0x00417520
        joan::orig::as_dialogue_line(m)->release_63e270();                               // 0x0063e270
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Dialog::show_message_63cdc0", 0x0063cdc0, &H::show_message_63cdc0);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using joan::call::thiscall;
struct H {
    // -------------------------------------------------------------- 0x0063d120
    u32 say_63d120(u32 arg) { return joan::orig::arg::as<u32>(joan::orig::as_ptr_list(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->delete_at(joan::orig::arg::i32(arg))); }  // 0x0063e1d0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Speaker::say_63d120", 0x0063d120, &H::say_63d120);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

using joan::call::thiscall;
JOAN_COUNTER(c_purge, "Script::purge_63d150");

struct H {
    // ------------------------------------------------------------------------ 0x0063d150
    u32 purge() {
        c_purge.hit();
        if (jo::self<jo::DialogueList>(this)->lines == nullptr) return joan::call::register_low_bits(this);  // eax only
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_streams(this)->count()); i > 0; --i) {                      // 0x0063d210
            if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->empty_at_g(joan::orig::arg::i32(i))) == 0) {                   // 0x0063e3f0
                joan::orig::as_ptr_list(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->delete_at(joan::orig::arg::i32(i));                          // 0x0063e1d0
            }
        }
        joan::orig::as_sound(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->reset_585d20();  // 0x00585d20
        return 0;  // void callee; every caller (esfuncs.cpp, i_buy.cpp) discards the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Script::purge_63d150", 0x0063d150, &H::purge);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

using u64 = std::uint64_t;

using joan::call::thiscall;

JOAN_COUNTER(c_add_line, "Text::add_line_63d250");

struct H {
    // 0x0063d250
    static void measure(void* text_owner) {  // 0x00624f00 on 0xa51608 with the owner's string
        const u16 width = jo::view<u16>(jo::as_dialogue(text_owner)->w);
        const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(&jo::as_dialogue(text_owner)->text)->c_str());  // 0x00417d90
        joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(joan::orig::arg::str(s), -1, joan::orig::arg::i16(static_cast<u32>(width)), 0, joan::orig::arg::f32(0x3f800000u));
    }
    static i16 line_height() {
        return joan::orig::arg::as<i16>(joan::orig::as_font(obj(d::g_font_goudy14_22_white))->text_height(joan::orig::arg::i16(4), joan::orig::arg::f32(0x3f800000u)));  // 0x006253f0
    }
    static float text_width(void* string_obj) {
        const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(string_obj)->c_str());  // 0x00417d90
        std::uintptr_t wrap = 0;  // 4-byte object (0x0058b9c0 stores the pointer)
        joan::orig::as_text_cursor(&wrap)->construct(joan::orig::arg::str(s));           // 0x0058b9c0
        const u32 w = joan::orig::arg::as<u32>(joan::orig::as_text_cursor(&wrap)->length());  // 0x0058c190
        return static_cast<float>(static_cast<u64>(w));  // fild qword (w, 0); fstp dword
    }
    u32 add_line_63d250(u32 a1, void* text, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7) {
        c_add_line.hit();
        u8* const owner = joan::orig::arg::as<u8*>(joan::orig::as_streams(this)->get(joan::orig::arg::i32(a1)));  // 0x0063d1e0
        if (jo::as_dialogue(owner)->sub_lines == nullptr) {
            void* const m = game_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
            void* const list = m != nullptr ? joan::orig::dynarray(m)->construct(static_cast<std::int32_t>(sizeof(void*)), 5, 0) : nullptr;  // 0x005843d0 (pointers: 4 on x86)
            jo::view<void*>(jo::as_dialogue(owner)->sub_lines) = list;
        }
        void* const m2 = game_new(static_cast<u32>(sizeof(jo::Dialogue)));  // 0x357 on x86 (the ctor clears sizeof)
        u8* line = m2 != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_dialogue_line(m2)->construct()) : nullptr;  // 0x00639af0
        joan::orig::as_string(line)->assign(str(d::kEmpty));              // 0x005a1440
        joan::orig::as_string(&jo::as_dialogue(line)->text)->assign(joan::orig::as_string(text));  // 0x005a1400
        jo::as_dialogue(line)->font = jo::as_dialogue(owner)->font;
        if (static_cast<i16>(a4) == -1) {
            jo::view<u16>(jo::as_dialogue(line)->x) = 0x1e;
            jo::view<u16>(jo::as_dialogue(line)->w) = static_cast<u16>(static_cast<i32>(jo::self<jo::DialogueList>(this)->w) - 0x32);
            if (joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(owner)->sub_lines))->size() == 0) {  // 0x00417ad0
                u16 y = jo::view<u16>(jo::as_dialogue(owner)->y);
                if (joan::orig::arg::as<i32>(joan::orig::as_name(&jo::as_dialogue(owner)->text)->length()) != 0) {  // 0x0045e110
                    measure(owner);
                    const i16 h = line_height();
                    y = static_cast<u16>(y + static_cast<u16>(static_cast<i32>(h) + 4));
                }
                jo::view<u16>(jo::as_dialogue(line)->y) = y;
                measure(line);
                const i16 h = line_height();
                jo::view<u16>(jo::as_dialogue(line)->h) = static_cast<u16>(static_cast<i32>(h) - 1);
            } else {
                const i32 n = joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(owner)->sub_lines))->size();
                void* const last = joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(owner)->sub_lines))->ptr_at(joan::orig::arg::i32(n));  // 0x0041b3f0
                const i16 top = static_cast<i16>(static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_dialogue(last)->bottom())) + 5);  // 0x0063e2e0
                measure(line);
                const i32 top_i = top;
                const i16 h = line_height();
                const i16 bottom = static_cast<i16>(top_i + static_cast<i32>(h) - 1);
                const i32 limit = joan::orig::arg::as<i16>(joan::orig::as_dialogue(owner)->bottom());
                if (static_cast<i32>(bottom) > limit) {
                    u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(owner)->sub_lines))->ptr_at(1));
                    jo::view<u16>(jo::as_dialogue(line)->y) = jo::view<u16>(jo::as_dialogue(first)->y);
                    jo::view<u16>(jo::as_dialogue(line)->h) = static_cast<u16>(static_cast<i32>(bottom) - static_cast<i32>(top));
                } else {
                    jo::view<u16>(jo::as_dialogue(line)->y) = static_cast<u16>(top);
                    jo::view<u16>(jo::as_dialogue(line)->h) = static_cast<u16>(static_cast<i32>(bottom) - static_cast<i32>(top) + 1);
                }
            }
        } else {
            jo::view<u16>(jo::as_dialogue(line)->x) = static_cast<u16>(a4);
            jo::view<u16>(jo::as_dialogue(line)->y) = static_cast<u16>(a5);
            jo::view<u16>(jo::as_dialogue(line)->w) =
                static_cast<u16>(static_cast<i32>(static_cast<i16>(a6)) - static_cast<i32>(static_cast<i16>(a4)) + 1);
            jo::view<u16>(jo::as_dialogue(line)->h) =
                static_cast<u16>(static_cast<i32>(static_cast<i16>(a7)) - static_cast<i32>(static_cast<i16>(a5)) + 1);
        }
        jo::as_dialogue(line)->shown = text_width(text);
        jo::as_dialogue(line)->visible = static_cast<u8>(a3);
        jo::as_dialogue(line)->sub_lines = nullptr;
        jo::view<u32>(jo::as_dialogue(line)->choice) = 0;
        joan::orig::dynarray(jo::view<void*>(jo::as_dialogue(owner)->sub_lines))->linkin(&line);      // 0x00584620
        jo::as_dialogue(owner)->shown = text_width(&jo::as_dialogue(owner)->text);
        joan::orig::as_dialogue(this)->layout_text();                          // 0x0063da60
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(owner)->sub_lines))->cursor());  // 0x00417520
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Text::add_line_63d250", 0x0063d250, &H::add_line_63d250);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------ 0x0063d770
    u32 field_353(u32 k) {
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_streams(this)->get(joan::orig::arg::i32(k)));  // 0x0063d1e0
        return jo::view<u32>(jo::as_dialogue(p)->choice);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Obj::field_353", 0x0063d770, &H::field_353);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

using joan::call::thiscall;
JOAN_COUNTER(c_all_ready, "Obj::all_ready");

struct H {
    // ------------------------------------------------------------------ 0x0063d7b0
    bool all_ready() {
        c_all_ready.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_streams(this)->count()) == 0) return true;  // 0x0063d210
        if (jo::self<jo::DialogueList>(this)->busy == 0) return false;
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_streams(this)->count()); i > 0; --i) {  // 0x0063d210
            if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::view<void*>(jo::self<jo::DialogueList>(this)->lines))->empty_at_g(joan::orig::arg::i32(i))) != 0) continue;  // 0x0063e3f0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_streams(this)->get(joan::orig::arg::i32(i)));              // 0x0063d1e0
            if (jo::as_dialogue(e)->done == 0) return false;
        }
        return true;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Obj::all_ready", 0x0063d7b0, &H::all_ready);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b100_12.cpp (game/b100_12)
namespace {
namespace game_b100_12 {

using joan::call::thiscall;
void* const kMouse = obj(d::g_mouse);

using joan::orig::helpers::crt_memcpy;

JOAN_COUNTER(c_hit_choice, "Dialogue::hit_choice");
JOAN_COUNTER(c_push_copy, "Dialogue::push_copy");

struct H {
    // ------------------------------------------------------------------ 0x0063d860
    bool hit_choice(u32 k) {
        c_hit_choice.hit();
        if (jo::self<jo::DialogueList>(this)->busy == 0) return false;
        u8* const d = joan::orig::arg::as<u8*>(joan::orig::as_streams(this)->get(joan::orig::arg::i32(k)));  // 0x0063d1e0
        if (jo::as_dialogue(d)->sub_lines == nullptr) return true;
        for (i32 i = 1; i <= static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_dialogue(d)->line_count())); ++i) {  // 0x0063e240
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(d)->sub_lines))->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (jo::as_dialogue(e)->visible == 0 && jo::self<jo::DialogueList>(this)->debug == 0) continue;
            if (jo::as_dialogue(e)->done == 0) return false;
        }
        for (i32 i = 1; i <= static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_dialogue(d)->line_count())); ++i) {  // 0x0063e240
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::view<void*>(jo::as_dialogue(d)->sub_lines))->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (jo::as_dialogue(e)->visible == 0 && jo::self<jo::DialogueList>(this)->debug == 0) continue;
            const i32 by = jo::self<jo::DialogueList>(this)->y;
            const i32 y2 = by + static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_dialogue(e)->bottom()));  // 0x0063e2e0
            const i32 bx = jo::self<jo::DialogueList>(this)->x;
            const i32 x2 = bx + static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_dialogue(e)->right()));  // 0x0063e2a0
            const i32 y1 = static_cast<i32>(jo::self<jo::DialogueList>(this)->y) + static_cast<i32>(jo::as_dialogue(e)->y);
            const i32 x1 = static_cast<i32>(jo::self<jo::DialogueList>(this)->x) + static_cast<i32>(jo::as_dialogue(e)->x) - 0xf;
            if (joan::orig::arg::as<i32>(joan::orig::as_parser(kMouse)->rect_mode(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), 0)) != 0) {  // 0x005940f0
                jo::as_dialogue(d)->choice = i;
                return true;
            }
        }
        return false;
    }

    // ------------------------------------------------------------------ 0x0063e0d0
    u32 push_copy(const void* src) {
        c_push_copy.hit();
        // the element is a Dialogue (0x00639af0 is Dialogue's ctor; 0x357 bytes on x86): allocation and bitwise copy
        void* const p = op_new(static_cast<u32>(sizeof(jo::Dialogue)));
        void* item = p != nullptr ? joan::orig::as_dialogue_line(p)->construct() : nullptr;  // 0x00639af0
        crt_memcpy(item, src, static_cast<u32>(sizeof(jo::Dialogue)));
        joan::orig::as_list(this)->add_63e1a0(&item);  // 0x0063e1a0
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_12", name, addr, fn)
B("Dialogue::hit_choice", 0x0063d860, &H::hit_choice);
B("Dialogue::push_copy", 0x0063e0d0, &H::push_copy);
#undef B

}  // namespace game_b100_12
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

struct H {
    // 0x0063dfa0
    void* ctor_63dfa0() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0, 0);           // 0x005856a0 (Dialogue pointers, push_copy 0x0063e0d0: 4 bytes on x86)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Obj::ctor_63dfa0", 0x0063dfa0, &H::ctor_63dfa0);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

using joan::call::thiscall;

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x0063dfd0
    u32 node_dtor() {
        joan::orig::as_dialogue_line_list(me())->clear();         // 0x0063e060
        return thiscall<u32>(fn::dtor_DynArrayB, me());  // 0x00585710
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("ScriptNode::dtor_63dfd0", 0x0063dfd0, &H::node_dtor);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
namespace {
namespace game_b104_11 {

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("Obj::nop_63e040", 0x0063e040, &H::nop);
#undef B

}  // namespace game_b104_11
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

using joan::call::thiscall;

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x0063e060
    u32 rebuild_63e060() {
        for (i32 n = joan::orig::as_dynarray(this)->size(); n > 0; --n) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->empty_at_g(joan::orig::arg::i32(n))) == 0) {            // 0x0063e3f0
                joan::orig::as_ptr_list(this)->delete_at(joan::orig::arg::i32(n));                   // 0x0063e1d0
            }
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
        return 0;  // void callee; both callers (0x0063c0a0 / ScriptNode::dtor_63dfd0) discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Obj::rebuild_63e060", 0x0063e060, &H::rebuild_63e060);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b99_08.cpp (game/b99_08)
namespace {
namespace game_b99_08 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0063e1a0
    u32 add(void* e) {
        joan::orig::as_dynarray(this)->linkin_584620(e);  // 0x00584620
        return 0;  // void callee; the one caller (Dialogue::push_copy) discards the result
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_08", name, addr, fn)
B99("List::add_63e1a0", 0x0063e1a0, &H::add);
#undef B99

}  // namespace game_b99_08
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0063e1d0
    u32 delete_at(i32 i) {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_g(joan::orig::arg::i32(i)));  // 0x0063e3c0
        if (p != nullptr) joan::orig::as_dialogue_line(p)->destroy(1u);  // 0x0063b540 deleting dtor
        return thiscall<u32>(fn::linkout_5858c0, this, i);            // 0x005858c0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("PtrList::delete_at", 0x0063e1d0, &H::delete_at);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

using joan::call::thiscall;

struct H {
    // --------------------------------------------------------------------- 0x0063e270
    u32 fwd_63b390() { return thiscall<u32>(fn::Panel_release_63b390, this); }  // 0x0063b390
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("Obj::fwd_63b390", 0x0063e270, &H::fwd_63b390);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

struct H {
    // 0x0063e320
    std::uint8_t byte0() { return jo::self<jo::DialogueList>(this)->open; }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Obj::byte0", 0x0063e320, &H::byte0);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b83_05.cpp (game/b83_05)
namespace {
namespace game_b83_05 {

struct H {
    // 0x0063e380 / 0x0063e340. ret 0.
    std::int32_t dialog_bottom() {
        return static_cast<std::int32_t>(jo::self<jo::DialogueList>(this)->y) + static_cast<std::int32_t>(jo::self<jo::DialogueList>(this)->h) - 1;
    }
    std::int32_t dialog_right() {
        return static_cast<std::int32_t>(jo::self<jo::DialogueList>(this)->x) + static_cast<std::int32_t>(jo::self<jo::DialogueList>(this)->w) - 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_05", name, addr, fn)
B("Dialog::bottom", 0x0063e380, &H::dialog_bottom);
B("Dialog::right", 0x0063e340, &H::dialog_right);
#undef B

}  // namespace game_b83_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::DialogueLine::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::ctor_639af0>(this)); }  // 0x639af0
auto joan::orig::Unit::set_model_prefix() -> void { joan::bind::member<&::game_b106_07::H::set_model_prefix>(this); }  // 0x639b40
auto joan::orig::Panel::release_63b390() -> void { joan::bind::member<&::game_b108_17::H::panel_release>(this); }  // 0x63b390
auto joan::orig::DialogueLine::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b108_01::H::scalar_dtor_63b540>(this, a0)); }  // 0x63b540
auto joan::orig::SoundResTable::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b101_12::H::scalar_dtor_63b590>(this, a0)); }  // 0x63b590
auto joan::orig::Slot::update() -> void { joan::bind::member<&::game_b107_09::H::slot_update>(this); }  // 0x63b5e0
auto joan::orig::Bytes::get(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b93_04::H::byte_at>(this, a0)); }  // 0x63bf10
auto joan::orig::Dialogue::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_05::H::dtor_body_63bf90>(this)); }  // 0x63bf90
auto joan::orig::Dialogue::sdtor_63c170(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b108_04::H::sdtor_63c170>(this, a0)); }  // 0x63c170
auto joan::orig::Panel::update_buttons(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_06::H::update_buttons>(this, a0)); }  // 0x63c1c0
auto joan::orig::Streams::pause_all(char a0) -> void { joan::bind::member<&::game_world::H::streams_pause>(this, a0); }  // 0x63c6d0
auto joan::orig::List::release_all() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_04::H::release_all>(this)); }  // 0x63c800
auto joan::orig::Dialogue::draw_63c8a0(std::uint8_t a0) -> void { joan::bind::member<&::game_ui7::H::draw_dialogues>(this, a0); }  // 0x63c8a0
auto joan::orig::Dialogue::blink_dot() -> void { joan::bind::member<&::game_b82_15::H::blink_dot>(this); }  // 0x63cc80
auto joan::orig::Dialog::show_message_63cdc0(std::int32_t a0, void* a1, std::uint32_t a2, std::uint32_t a3, const char* a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_08::H::show_message_63cdc0>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x63cdc0
auto joan::orig::Dialogue::delete_line(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_20::H::say_63d120>(this, a0)); }  // 0x63d120
auto joan::orig::Script::purge_63d150() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_14::H::purge>(this)); }  // 0x63d150
auto joan::orig::Streams::get(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_world::H::sounds_get>(this, a0)); }  // 0x63d1e0
auto joan::orig::Streams::count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_world::H::sounds_count>(this)); }  // 0x63d210
auto joan::orig::Text::add_line_63d250(std::uint32_t a0, void* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_02::H::add_line_63d250>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x63d250
auto joan::orig::Dialogue::choice_of(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_06::H::field_353>(this, a0)); }  // 0x63d770
auto joan::orig::Dialogue::all_ready() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b96_06::H::all_ready>(this)); }  // 0x63d7b0
auto joan::orig::Dialogue::hit_choice(std::uint32_t a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b100_12::H::hit_choice>(this, a0)); }  // 0x63d860
auto joan::orig::DialogueLineList::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_10::H::node_dtor>(this)); }  // 0x63dfd0
auto joan::orig::DialogueLineList::init() -> void { joan::bind::member<&::game_b104_11::H::nop>(this); }  // 0x63e040
auto joan::orig::DialogueLineList::clear() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_12::H::rebuild_63e060>(this)); }  // 0x63e060
auto joan::orig::Dialogue::push_copy(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_12::H::push_copy>(this, a0)); }  // 0x63e0d0
auto joan::orig::List::add_63e1a0(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_08::H::add>(this, a0)); }  // 0x63e1a0
auto joan::orig::PtrList::delete_at(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_13::H::delete_at>(this, a0)); }  // 0x63e1d0
auto joan::orig::Dialogue::line_count() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_15::H::dlg_lines>(this)); }  // 0x63e240
auto joan::orig::DialogueLine::release_63e270() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_16::H::fwd_63b390>(this)); }  // 0x63e270
auto joan::orig::Dialogue::right() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_15::H::dlg_right>(this)); }  // 0x63e2a0
auto joan::orig::Dialogue::bottom() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_15::H::dlg_bottom>(this)); }  // 0x63e2e0
auto joan::orig::Dialogue::is_open() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b94_06::H::byte0>(this)); }  // 0x63e320
auto joan::orig::Dialog::right() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_05::H::dialog_right>(this)); }  // 0x63e340
auto joan::orig::Dialog::bottom() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_05::H::dialog_bottom>(this)); }  // 0x63e380
auto joan::orig::List::get_g(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_world::H::list_get_wrap>(this, a0)); }  // 0x63e3c0
auto joan::orig::List::empty_at_g(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_world::H::list_empty_at>(this, a0)); }  // 0x63e3f0
auto joan::orig::Input::flag_66d() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_04::H::flag_66d>(this)); }  // 0x63e420
