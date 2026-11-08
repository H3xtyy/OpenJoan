// SPDX-License-Identifier: GPL-3.0-or-later
// enparticle/petoken.cpp: functions of the original d:\ww\enparticle\petoken.cpp (re/attribution.csv). The file name is
// the Restaurant Empire 2 engine's (enparticle/PEToken.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string
// for it.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
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
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/bind.hpp"
#include "../original/layout_access.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using i32 = std::int32_t;

using joan::hook::global;

using joan::img::str;

// One {name, kind} entry of the image table strs_a21680 (0x38 entries), packed as joan_image is (data.inc,
// #pragma pack(1)): 8 bytes on x86, 12 on x64 (the name pointer is pointer-wide there).
#pragma pack(push, 1)
struct NameKind {
    std::uintptr_t name;
    u32 kind;
};
#pragma pack(pop)

}  // namespace

// ---------------------------------------- from modules/game_b111_03.cpp (game/b111_03)
namespace {
namespace game_b111_03 {

u8* peek(void* ts) { return joan::orig::arg::as<u8*>(joan::orig::as_token_list(ts)->peek()); }  // 0x0067ac80
u8* next(void* ts) { return joan::orig::arg::as<u8*>(joan::orig::as_token_list(ts)->next()); }  // 0x0067acb0

// throw (char*) (original/cxx_eh.hpp).
[[noreturn]] void fail(img::Addr message) { throw static_cast<char*>(&img::global<char>(message)); }

using joan::orig::helpers::type;
const char* text(const u8* tok) { return jo::as_token(tok)->text; }
i32 stricmp_(const char* a, img::Addr b) {
    return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(&img::global<const char>(b)));  // _stricmp
}
float atof_(const char* s) { return joan::orig::arg::as<float>(joan::orig::libc::atof(joan::orig::arg::str(s))); }  // atof: st0 -> fstp dword

// 8-byte {min, max} value with the game's (empty) ctor / dtor, owned on unwind like the original.
struct Range {
    u32 v[2];
    Range() { joan::orig::as_float_range(this)->construct(); }   // 0x0067a4a0
    ~Range() { joan::orig::as_float_range(this)->destruct(); }  // 0x0067a4e0
    Range(const Range&) = delete;
    Range& operator=(const Range&) = delete;
};
float rf(const Range& r, int i) { return jo::view<float>(r.v[i]); }

JOAN_COUNTER(c_range, "Parser::range");
JOAN_COUNTER(c_xyz, "Parser::xyz_range");
JOAN_COUNTER(c_rgba, "Parser::rgba_range");
JOAN_COUNTER(c_time, "Parser::time_spec");
JOAN_COUNTER(c_ops, "Parser::color_op/alpha_op");

// ------------------------------------------------------------------ 0x0067cbf0
u8* parse_range(void* ts, float* out) {
    c_range.hit();
    u8* tok = peek(ts);
    const i32 kind = type(tok);
    if (kind == 1) {
        if (type(tok) != 1) fail(d::s_Expecting_number);  // 'Expecting number.'
        out[0] = atof_(text(tok));
        jo::view<u32>(out[1]) = jo::view<u32>(out[0]);
        return next(ts);
    }
    if (kind != 0xb) fail(d::s_Expecting_either_Random_or_a);  // 'Expecting either Random(...) or a number value.'
    tok = next(ts);
    if (type(tok) != 4) fail(d::s_Expecting_opening_paren_afte);  // 'Expecting opening paren after Random keyword.'
    tok = next(ts);
    if (type(tok) != 1) fail(d::s_Expecting_first_number_withi);  // 'Expecting first number within Random(...).'
    out[0] = atof_(text(tok));
    tok = next(ts);
    if (type(tok) != 3) fail(d::s_Expecting_comma_within_Rando);  // 'Expecting comma within Random(...).'
    tok = next(ts);
    if (type(tok) != 1) fail(d::s_Expecting_second_number_with);  // 'Expecting second number within Random(...).'
    out[1] = atof_(text(tok));
    tok = next(ts);
    if (type(tok) != 5) fail(d::s_Missing_close_paren_on_Rando);  // 'Missing close paren on Random(...).'
    return next(ts);
}
u8* range_thunk(void* ts, void* out) { return joan::orig::arg::as<u8*>(joan::orig::as_parser(ts)->range(joan::orig::arg::mptr(out))); }  // 0x0067cbf0

// Shared "<comma> <range>" step of the vector parsers.
void comma_then_range(void* ts, Range& r, img::Addr message) {
    if (type(peek(ts)) != 3) fail(message);
    next(ts);
    range_thunk(ts, r.v);
}

// ------------------------------------------------------------------ 0x0067cdf0
std::uintptr_t parse_xyz(void* ts, u32* out) {  // EAX = out + 3 (pointer-wide on x64)
    c_xyz.hit();
    if (type(peek(ts)) != 0xc) fail(d::s_Expecting_XYZ);  // 'Expecting XYZ(...)!'
    if (type(next(ts)) != 4) fail(d::s_Expecting_after_XYZ);    // 'Expecting ( after XYZ!'
    Range x;
    Range y;
    Range z;
    next(ts);
    range_thunk(ts, x.v);
    comma_then_range(ts, y, d::s_Vector_components_must_be_se);  // 'Vector components must be seperated by a comma.'
    comma_then_range(ts, z, d::s_Vector_components_must_be_se);
    if (type(peek(ts)) != 5) fail(d::s_Expecting_to_close_vector);  // 'Expecting ) to close vector.'
    next(ts);
    out[0] = x.v[0];
    out[1] = y.v[0];
    out[2] = z.v[0];
    out[3] = x.v[1];
    out[4] = y.v[1];
    out[5] = z.v[1];
    return reinterpret_cast<std::uintptr_t>(out + 3);  // eax: out + 0xc
}

// ------------------------------------------------------------------ 0x0067d050
u32 pack(const Range& r, const Range& g, const Range& b, const Range& a, int i) {
    // Pushed right to left: a, b, g, r (_ftol has no side effects; order kept anyway).
    const i32 ia = joan::hook::x87::ftol(rf(a, i));
    const i32 ib = joan::hook::x87::ftol(rf(b, i));
    const i32 ig = joan::hook::x87::ftol(rf(g, i));
    const i32 ir = joan::hook::x87::ftol(rf(r, i));
    u32 colour = 0;
    u32* const c = joan::orig::arg::as<u32*>(joan::orig::as_colour(&colour)->construct(ir, ig, ib, ia));  // 0x0042a260
    return *c;
}
u32 parse_rgba(void* ts, u32* out) {
    c_rgba.hit();
    if (type(peek(ts)) != 0xd) fail(d::s_Expecting_RGBA);  // 'Expecting RGBA(...)!'
    if (type(next(ts)) != 4) fail(d::s_Expecting_after_RGBA);    // 'Expecting ( after RGBA!'
    Range r;
    Range g;
    Range b;
    Range a;
    next(ts);
    range_thunk(ts, r.v);
    comma_then_range(ts, g, d::s_Color_components_must_be_sep);  // 'Color components must be seperated by a comma.'
    comma_then_range(ts, b, d::s_Color_components_must_be_sep);
    comma_then_range(ts, a, d::s_Color_components_must_be_sep);
    if (type(peek(ts)) != 5) fail(d::s_Expecting_to_close_vector);  // 'Expecting ) to close vector.'
    next(ts);
    out[0] = pack(r, g, b, a, 0);
    const u32 hi = pack(r, g, b, a, 1);
    out[1] = hi;
    return hi;  // eax: the packed max colour
}

// ------------------------------------------------------------------ 0x0067d340
u8* parse_time(void* ts, u32* out, u8* fade, u32 initial, u32 final_) {
    c_time.hit();
    u8* tok = peek(ts);
    if (type(tok) == 0x10) {
        tok = next(ts);
        if (type(tok) != 0x11) fail(d::s_Expecting_so_after_fade);  // 'Expecting "so" after "fade".'
        *fade = 1;
        tok = next(ts);
    } else {
        *fade = 0;
    }
    const i32 kind = type(tok);
    if (kind == 0x12) {
        next(ts);
        return range_thunk(ts, out);
    }
    if (kind == 0x13) {
        if (*fade != 0) fail(d::s_Cannot_use_fade_so_modifier);  // 'Cannot use "fade so" modifier on "initial" times.'
        out[0] = initial;
        out[1] = initial;
        return next(ts);
    }
    if (kind == 0x14) {
        out[0] = final_;
        out[1] = final_;
        return next(ts);
    }
    fail(d::s_Expected_time_specifier_fade);  // 'Expected time specifier: "[fade so] at", "initial", or "[fade so] final"'
}

// ------------------------------------------------------------------ 0x0067d490 / 0x0067d590
u8* parse_op(void* ts, u32* out, i32 kind, img::Addr expect_msg, const img::Addr names[3],
             const u32 values[3], img::Addr invalid_msg) {
    c_ops.hit();
    u8* const tok = peek(ts);
    if (type(tok) != kind) fail(expect_msg);
    int i = 0;
    for (; i < 3; ++i) {
        if (stricmp_(text(tok), names[i]) == 0) {
            *out = values[i];
            break;
        }
    }
    if (i == 3) fail(invalid_msg);
    return next(ts);
}

struct H {
    u8* range(float* out) { return parse_range(this, out); }
    std::uintptr_t xyz(u32* out) { return parse_xyz(this, out); }
    u32 rgba(u32* out) { return parse_rgba(this, out); }
    u8* time_spec(u32* out, u8* fade, u32 initial, u32 final_) { return parse_time(this, out, fade, initial, final_); }
    u8* color_op(u32* out) {
        const img::Addr names[3] = {d::s_COLOR_MODULATE, d::s_COLOR_VERTEX, d::s_COLOR_TEXTURE};  // COLOR_MODULATE / _VERTEX / _TEXTURE
        const u32 values[3] = {0, 1, 2};
        // 'Expecting color op mode (COLOR_MODULATE, COLOR_VERTEX or COLOR_TEXTURE).' / 'Invalid color op mode!'
        return parse_op(this, out, 0x15, d::s_Expecting_color_op_mode_COLO, names, values, d::s_Invalid_color_op_mode);
    }
    u8* alpha_op(u32* out) {
        const img::Addr names[3] = {d::s_ALPHA_MODULATE, d::s_ALPHA_VERTEX, d::s_ALPHA_TEXTURE};  // ALPHA_MODULATE / _VERTEX / _TEXTURE
        const u32 values[3] = {0, 0x10, 0x20};
        // 'Expecting alpha op mode (ALPHA_MODULATE, ALPHA_VERTEX or ALPHA_TEXTURE).' / 'Invalid alpha op mode!'
        return parse_op(this, out, 0x16, d::s_Expecting_alpha_op_mode_ALPH, names, values, d::s_Invalid_alpha_op_mode);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_03", name, addr, fn)
B("ParticleEmitterTokenizer::process_number", 0x0067cbf0, &H::range);
B("ParticleEmitterTokenizer::process_vector", 0x0067cdf0, &H::xyz);
B("ParticleEmitterTokenizer::process_color", 0x0067d050, &H::rgba);
B("ParticleEmitterTokenizer::process_time", 0x0067d340, &H::time_spec);
B("ParticleEmitterTokenizer::process_color_op_mode", 0x0067d490, &H::color_op);
B("ParticleEmitterTokenizer::process_alpha_op_mode", 0x0067d590, &H::alpha_op);
#undef B

}  // namespace game_b111_03
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
namespace {
namespace game_b103_14 {

JOAN_COUNTER(c_token_add, "TokenList::add");

struct H {
    // ------------------------------------------------------------------ 0x0067d690
    u32 token_add(const char* s, i32 line) {
        c_token_add.hit();
        if (joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))) == 0) return 0;  // strlen
        jo::Token e;  // {text, kind, line}: 0xc on x86, the element of TokenList::tokens
        e.text = const_cast<char*>(s);
        e.kind = 0;  // always written by 0x0067d6f0
        e.line = line;
        joan::orig::as_token_list(this)->classify(&e);                                           // 0x0067d6f0
        return thiscall<u32>(fn::linkin_584620, &jo::as_token_list(this)->tokens, &e);  // 0x00584620
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("ParticleEmitterTokenizer::add_token", 0x0067d690, &H::token_add);
#undef B103

}  // namespace game_b103_14
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

using joan::img::obj;

using i8 = std::int8_t;

using joan::orig::helpers::at;

JOAN_COUNTER(c_classify, "Tokenizer::classify");

struct H {
    // ------------------------------------------------------------------------ 0x0067d6f0
    void classify(u8* tok) {
        c_classify.hit();
        jo::view<u32>(jo::as_token(tok)->kind) = 0x1f;
        const char* const s = jo::as_token(tok)->text;
        if (joan::orig::arg::as<i32>(joan::orig::libc::isdigit(static_cast<i32>(static_cast<i8>(s[0])))) != 0 ||  // isdigit
            static_cast<i32>(static_cast<i8>(jo::as_token(tok)->text[0])) == 0x2d) {
            jo::view<u32>(jo::as_token(tok)->kind) = 1;
            return;
        }
        if (static_cast<i32>(static_cast<i8>(jo::as_token(tok)->text[0])) == 0x22) {
            jo::view<u32>(jo::as_token(tok)->kind) = 8;
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::libc::_strnicmp(joan::orig::arg::str(jo::as_token(tok)->text), str(d::s_COLOR_9dca98), i32{6})) == 0) {  // "COLOR_"
            jo::view<u32>(jo::as_token(tok)->kind) = 0x15;
            return;
        }
        if (joan::orig::arg::as<i32>(joan::orig::libc::_strnicmp(joan::orig::arg::str(jo::as_token(tok)->text), str(d::s_ALPHA), i32{6})) == 0) {  // "ALPHA_"
            jo::view<u32>(jo::as_token(tok)->kind) = 0x16;
            return;
        }
        if (global<i32>(d::g_a6a3c0) == 0) {
            joan::orig::libc::qsort(joan::orig::arg::mptr(obj(d::strs_a21680)), joan::orig::arg::u32(u32{0x38}), static_cast<u32>(sizeof(NameKind)), joan::orig::arg::cmp(fn::code_value(fn::cmp_name_pp_67d860)));  // qsort
            global<i32>(d::g_a6a3c0) = 1;
        }
        const NameKind* const e = joan::orig::arg::as<const NameKind*>(joan::orig::libc::bsearch(joan::orig::arg::mptr(jo::as_token(tok)->text), joan::orig::arg::mptr(obj(d::strs_a21680)), joan::orig::arg::u32(u32{0x38}), static_cast<u32>(sizeof(NameKind)), joan::orig::arg::cmp(fn::code_value(fn::cmp_name_sp_67d890))));  // bsearch
        if (e != nullptr)
            jo::view<u32>(jo::as_token(tok)->kind) = e->kind;
        else
            jo::view<u32>(jo::as_token(tok)->kind) = 0x1f;
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("ParticleEmitterTokenizer::set_token_type", 0x0067d6f0, &H::classify);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Parser::range(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_03::H::range>(this, a0)); }  // 0x67cbf0
auto joan::orig::Parser::xyz_range(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_03::H::xyz>(this, a0)); }  // 0x67cdf0
auto joan::orig::Parser::time_spec(void* a0, void* a1, std::uint32_t a2, std::uint32_t a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_03::H::time_spec>(this, a0, a1, a2, a3)); }  // 0x67d340
auto joan::orig::Parser::color_op(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_03::H::color_op>(this, a0)); }  // 0x67d490
auto joan::orig::Parser::alpha_op(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_03::H::alpha_op>(this, a0)); }  // 0x67d590
auto joan::orig::TokenList::classify(void* a0) -> void { joan::bind::member<&::game_b98_08::H::classify>(this, a0); }  // 0x67d6f0
