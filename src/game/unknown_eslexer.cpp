// game/unknown_eslexer.cpp: functions whose original source file is unknown (formerly game/_unplaced/eslexer.cpp;
// re/attribution.csv: between ww/uti3d/esstate2.cpp and ww/uti3d/lflare.cpp). They move to their original file when
// evidence names it (Phase 3).
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
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/cxx_eh.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b96_07.cpp (game/b96_07)
namespace {
namespace game_b96_07 {

JOAN_COUNTER(c_list_ctor, "List::List");

struct H {
    // ------------------------------------------------------------------ 0x00661ea0
    H* list_ctor() {
        c_list_ctor.hit();
        joan::orig::as_script_tokenizer(this)->construct();  // 0x00684010
        joan::orig::dynarray(&jo::self<jo::ScriptLexer>(this)->tokens)->construct(static_cast<i32>(sizeof(jo::Token)), 0x1e, 0);  // 0x005843d0 (0xc on x86)
        jo::self<jo::ScriptLexer>(this)->vtable = img::obj(d::vt_Obj661f20);
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_07", name, addr, fn)
B96("List::List", 0x00661ea0, &H::list_ctor);
#undef B96

}  // namespace game_b96_07
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using joan::orig::helpers::op_delete;

struct H {
    // ------------------------------------------------------------------ 0x00661f20
    void* ddtor_661f20(u32 flags) {
        joan::orig::as_script_lexer(this)->destruct();  // 0x00661f70
        if ((flags & 1u) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("Obj661f20::deleting_dtor", 0x00661f20, &H::ddtor_661f20);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

struct H {
    // ------------------------------------------------------------------ 0x00661f70
    u32 destroy() {
        jo::self<jo::ScriptLexer>(this)->vtable = img::obj(d::vt_Obj661f20);
        joan::orig::dynarray(&jo::self<jo::ScriptLexer>(this)->tokens)->destruct();  // 0x005844b0
        return joan::orig::arg::as<u32>(joan::orig::as_script_tokenizer(this)->destruct());   // 0x006840a0
    }
};

JOAN_REPLACE("game/b108_07", "ScriptNode::~ScriptNode", 0x00661f70, &H::destroy);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b111_01.cpp (game/b111_01)
namespace {
namespace game_b111_01 {

using joan::img::obj;

using joan::hook::global;

using joan::img::str;

JOAN_COUNTER(c_expect_xyz, "ScriptLexer::expect_xyz");
JOAN_COUNTER(c_classify, "ScriptLexer::classify");

[[noreturn]] void throw_message(img::Addr text) {
    throw const_cast<char*>(str(text));  // throw (char*) (original/cxx_eh.hpp)
}

using joan::orig::helpers::strlen_;
i32 is_digit(i32 c) { return joan::orig::arg::as<i32>(joan::orig::libc::isdigit(joan::orig::arg::i32(c))); }
i32 is_alpha(i32 c) { return joan::orig::arg::as<i32>(joan::orig::libc::isalpha(joan::orig::arg::i32(c))); }
i32 sx(char c) { return static_cast<i32>(static_cast<signed char>(c)); }

// Script token: +0 text, +4 type, +8 line.
struct Token {
    char* text;
    i32 type;
    i32 line;
};

struct H {
    // ------------------------------------------------------------------ 0x00661ff0
    void* expect_int(i32* out) {
        Token* const t = joan::orig::arg::as<Token*>(joan::orig::as_script_lexer(this)->skip_newlines());  // 0x00642370
        if (t->type != 1) throw_message(d::s_Expecting_int);          // "Expecting int."
        *out = joan::crt::atoi(joan::orig::arg::str(t->text));          // atoi
        return joan::orig::arg::as<void*>(joan::orig::as_script_lexer(this)->next());             // 0x006423e0
    }

    // ------------------------------------------------------------------ 0x00662060
    void* expect_float(float* out) {
        Token* const t = joan::orig::arg::as<Token*>(joan::orig::as_script_lexer(this)->skip_newlines());  // 0x00642370
        if (t->type != 2) throw_message(d::s_Expecting_float);          // "Expecting float."
        *out = joan::orig::arg::as<float>(joan::orig::libc::atof(joan::orig::arg::str(t->text)));        // atof, st0 -> fstp dword
        return joan::orig::arg::as<void*>(joan::orig::as_script_lexer(this)->next());             // 0x006423e0
    }

    // one component: int token -> fild / fstp, else expect_float into the slot
    void xyz_component(float* slot) {
        Token* const t = joan::orig::arg::as<Token*>(joan::orig::as_script_lexer(this)->next());  // 0x006423e0
        if (t->type == 1) {
            i32 value = 0;  // [ebp-0xc], written by 0x00661ff0 before any read
            joan::orig::as_script_lexer(this)->expect_int(&value);       // 0x00661ff0
            *slot = static_cast<float>(value);
        } else {
            joan::orig::as_script_lexer(this)->expect_float(joan::orig::arg::mptr(slot));         // 0x00662060
        }
    }

    // ------------------------------------------------------------------ 0x006620d0
    void* expect_xyz(float* out) {
        c_expect_xyz.hit();
        Token* t = joan::orig::arg::as<Token*>(joan::orig::as_script_lexer(this)->skip_newlines());  // 0x00642370
        if (t->type != 0x1b) throw_message(d::s_Expecting_XYZ);  // "Expecting XYZ(...)!"
        t = joan::orig::arg::as<Token*>(joan::orig::as_script_lexer(this)->next());          // 0x006423e0
        if (t->type != -0x25) throw_message(d::s_Expecting_after_XYZ);  // "Expecting ( after XYZ!"
        xyz_component(out);
        t = joan::orig::arg::as<Token*>(joan::orig::as_script_lexer(this)->skip_newlines());
        if (t->type != 7) throw_message(d::s_Vector_components_must_be_se);  // "Vector components must be seperated by a comma."
        xyz_component(out + 1);
        t = joan::orig::arg::as<Token*>(joan::orig::as_script_lexer(this)->skip_newlines());
        if (t->type != 7) throw_message(d::s_Vector_components_must_be_se);
        xyz_component(out + 2);
        t = joan::orig::arg::as<Token*>(joan::orig::as_script_lexer(this)->skip_newlines());
        if (t->type != -0x24) throw_message(d::s_Expecting_to_close_vector);  // "Expecting ) to close vector."
        return joan::orig::arg::as<void*>(joan::orig::as_script_lexer(this)->next());         // 0x006423e0
    }

    // ------------------------------------------------------------------ 0x00662330
    void classify(Token* t) {
        c_classify.hit();
        t->type = 0x22;
        if (global<i32>(d::g_a6a268) == 0) {
            joan::orig::libc::qsort(joan::orig::arg::mptr(img::obj(d::strs_a214c8)), 0x2cu, static_cast<std::uint32_t>(sizeof(jo::ScriptKeyword)), reinterpret_cast<void*>(fn::code_value(fn::cmp_name_pp_662590)));  // qsort(keyword table: 0x2c ScriptKeyword, 8 bytes each on x86)
            global<i32>(d::g_a6a268) = 1;
        }
        void* const hit = joan::orig::arg::as<void*>(joan::orig::libc::bsearch(joan::orig::arg::mptr(t->text), joan::orig::arg::mptr(img::obj(d::strs_a214c8)), 0x2cu, static_cast<std::uint32_t>(sizeof(jo::ScriptKeyword)), reinterpret_cast<void*>(fn::code_value(fn::cmp_name_sp_6625c0))));  // bsearch
        if (hit != nullptr) {
            t->type = jo::as_script_keyword(hit)->type;
            return;
        }
        if (is_digit(sx(t->text[0])) != 0 || sx(t->text[0]) == 0x2d) {
            if (joan::orig::arg::as<char*>(joan::orig::libc::strchr(joan::orig::arg::str(t->text), 0x2e)) != nullptr)  // strchr '.'
                t->type = 2;
            else
                t->type = 1;
            return;
        }
        if (sx(t->text[0]) == 0x22) {
            t->type = 3;
            t->text = t->text + 1;
            const i32 len = static_cast<i32>(strlen_(t->text));
            if (len > 0 && sx(t->text[len - 1]) == 0x22) {
                t->text[len - 1] = 0;
                return;
            }
            throw reinterpret_cast<EventSystemToken*>(t);  // throw (EventSystemToken*) (original/cxx_eh.hpp)
        }
        if (is_alpha(sx(t->text[0])) == 0 && sx(t->text[0]) != 0x5f) return;
        // the bound is the character code itself (signed), as in the original
        for (i32 i = 1; i < sx(t->text[i]); ++i) {
            if (is_alpha(sx(t->text[i])) != 0) continue;
            if (sx(t->text[i]) == 0x5f) continue;
            if (is_digit(sx(t->text[i])) != 0) continue;
            return;
        }
        t->type = 0x21;
    }

    // ------------------------------------------------------------------ 0x006622d0
    u32 add_token(char* text, i32 line) {
        if (strlen_(text) == 0) return 0;  // eax = strlen result
        // type (+4) is uninitialised in the original; 0x00662330 writes it first (dead value)
        Token t{text, 0, line};
        joan::orig::as_script_lexer(this)->classify(&t);                 // 0x00662330
        joan::orig::as_dynarray(&jo::self<jo::ScriptLexer>(this)->tokens)->linkin_584620(&t);  // 0x00584620 append
        return 0;  // 0x00584620 returns nothing; reached only through Tokenizer::split's emit, whose callers discard it
    }
};

#define B111(name, addr, fn) JOAN_REPLACE("game/b111_01", name, addr, fn)
B111("ScriptLexer::expect_int", 0x00661ff0, &H::expect_int);
B111("ScriptLexer::expect_float", 0x00662060, &H::expect_float);
B111("ScriptLexer::expect_xyz", 0x006620d0, &H::expect_xyz);
B111("ScriptLexer::classify", 0x00662330, &H::classify);
B111("ScriptLexer::add_token", 0x006622d0, &H::add_token);
#undef B111

}  // namespace game_b111_01
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
namespace {
namespace game_b103_14 {

using joan::orig::helpers::stricmp_;  // _stricmp

// ------------------------------------------------------------------ comparators (qsort)
// One instance per original address (template tag) so each registration has its own code.
template <fn::Code Tag>
i32 JOAN_CDECL cmp_name_pp(const char* const* a, const char* const* b) {
    return stricmp_(*a, *b);
}
template <fn::Code Tag>
i32 JOAN_CDECL cmp_name_sp(const char* a, const char* const* b) {
    return stricmp_(a, *b);
}

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("cmp_name_pp_662590", 0x00662590, &cmp_name_pp<fn::cmp_name_pp_662590>);
B103("cmp_name_sp_6625c0", 0x006625c0, &cmp_name_sp<fn::cmp_name_sp_6625c0>);
#undef B103

}  // namespace game_b103_14
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::List::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_07::H::list_ctor>(this)); }  // 0x661ea0
auto joan::orig::ScriptLexer::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_07::H::destroy>(this)); }  // 0x661f70
auto joan::orig::ScriptLexer::expect_int(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_01::H::expect_int>(this, a0)); }  // 0x661ff0
auto joan::orig::ScriptLexer::expect_float(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_01::H::expect_float>(this, a0)); }  // 0x662060
auto joan::orig::ScriptLexer::expect_xyz(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_01::H::expect_xyz>(this, a0)); }  // 0x6620d0
auto joan::orig::ScriptLexer::classify(void* a0) -> void { joan::bind::member<&::game_b111_01::H::classify>(this, a0); }  // 0x662330
