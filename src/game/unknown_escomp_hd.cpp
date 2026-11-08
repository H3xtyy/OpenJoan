// game/unknown_escomp_hd.cpp: functions whose original source file is unknown (formerly game/_unplaced/escomp_hd.cpp;
// re/attribution.csv: between ww/uti3d/esstate2.cpp and ww/uti3d/lflare.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdarg>
#include <cstddef>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/cxx_eh.hpp"
#include "../original/layout_access.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b102_13.cpp (game/b102_13)
namespace {
namespace game_b102_13 {

struct Obj {};

// thiscall through the object's vtable slot `off`.
template <class R = void, class... A>
R vcall(const void* self, u32 off, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    void* const fn = joan::call::vtable_of(self)[joan::call::x86_vslot(off)];
    std::memcpy(&method, &fn, sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}
using joan::orig::helpers::at;

JOAN_COUNTER(c_run_block, "ScriptNode::run_block_648560_649060");

struct H {
    i32 nparams() { return joan::orig::arg::as<i32>(joan::orig::as_reader(this)->count()); }          // 0x0063ecd0
    u8* cur() { return joan::orig::arg::as<u8*>(joan::orig::as_table(jo::view<void*>(jo::self<jo::Script>(this)->statements))->last()); }    // 0x00585f20
    i32 next() { return joan::orig::arg::as<i32>(joan::orig::as_reader(jo::view<void*>(jo::self<jo::Script>(this)->statements))->next()); }   // 0x0063ee20
    void end() { joan::orig::as_counter(jo::view<void*>(jo::self<jo::Script>(this)->statements))->clamp_to_one(); }                // 0x004b9d40

    // ------------------------------------------------------------------ 0x00649060
    i32 run_block_649060(std::uintptr_t arg) {  // arg: the evaluator context (pointer-sized)
        c_run_block.hit();
        if (nparams() == 0) return 1;
        for (;;) {
            const i32 r = vcall<i32>(cur(), 0xc, arg);
            if (r == -1) {
                end();
                return -1;
            }
            if (r == 0) return 0;
            if (r == 1 && next() == 0) {
                end();
                return 1;
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_13", name, addr, fn)
B("ScriptNode::run_block_649060", 0x00649060, &H::run_block_649060);
#undef B

}  // namespace game_b102_13
}  // namespace

// ---------------------------------------- from modules/game_b111_05.cpp (game/b111_05)
namespace {
namespace game_b111_05 {

using joan::call::cdecl_call;

using joan::img::str;

// ---- reader / token helpers
u8* peek(void* rd) { return joan::orig::arg::as<u8*>(joan::orig::as_script_lexer(rd)->skip_newlines()); }  // 0x00642370
u8* lex(void* rd) { return joan::orig::arg::as<u8*>(joan::orig::as_script_lexer(rd)->next()); }   // 0x006423e0 (may throw char*)
using joan::orig::helpers::type;
u32 line(const u8* tok) { return jo::view<u32>(jo::as_token(tok)->line); }
void err(const void* self, img::Addr fmt, u32 ln) {  // Script::error 0x0063eb80 (cdecl, varargs)
    joan::orig::script::error(joan::orig::arg::mptr(self), str(fmt), joan::orig::arg::i32(ln));  // 0x0063eb80 (callers drop the eax)
}
u32 vparse(void* o, void* rd) {  // virtual parse, vtable slot +0x10, thiscall(reader), ret 4
    return thiscall<u32>(static_cast<const std::uintptr_t*>(jo::self<jo::ScriptNode>(o)->vtable)[4], o, rd);
}
void* stmt_for(void* self, const u8* tok) {  // 0x00643550 (type, line), ret 8
    const u32 ln = line(tok);
    return joan::orig::arg::as<void*>(joan::orig::as_script_block(self)->make_statement(joan::orig::arg::i32(type(tok)), joan::orig::arg::u32(ln)));
}

JOAN_COUNTER(c_script, "Script::parse_649140");

struct H {
    // ------------------------------------------------------------------ 0x00649140
    std::uintptr_t parse_script(void* rd) {
        c_script.hit();
        if (joan::orig::as_dynarray(&jo::self<jo::ScriptLexer>(rd)->tokens)->size() < 2)  // 0x00417ad0
            err(this, d::s_This_script_is_too_small_to, 0u);                     // 'This script is too small to be valid.'
        u8* tok = peek(rd);
        if (type(tok) != 3) err(this, d::s_Must_name_script, line(tok));  // 'Must name script.'
        joan::orig::as_script(this)->set_name(joan::orig::arg::str(jo::as_token(tok)->text));     // 0x00649860, ret 4
        tok = lex(rd);
        if (type(tok) != 4) err(this, d::s_Missing_opening_brace_for_Sc, line(tok));  // 'Missing opening brace for Script block.'
        lex(rd);
        for (;;) {
            tok = peek(rd);
            if (type(tok) == 5) break;
            vparse(stmt_for(this, tok), rd);
        }
        return joan::orig::arg::as<u32>(joan::orig::as_counter(jo::self<jo::Script>(this)->statements)->clamp_to_one());  // 0x004b9d40 (no null check)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_05", name, addr, fn)
B("Script::parse_649140", 0x00649140, &H::parse_script);
#undef B

}  // namespace game_b111_05
}  // namespace
