// game/unknown_tokenize.cpp: functions whose original source file is unknown (formerly game/_unplaced/tokenize.cpp;
// re/attribution.csv: between ww/enpath/spindex.cpp and ww/uti3d/video.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include <cstring>
#include <cstddef>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using u8 = std::uint8_t;
using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b93_05.cpp (game/b93_05)
namespace {
namespace game_b93_05 {

struct H {
    // 0x00684010
    H* ctor_684010() {
        joan::orig::as_string_pool(&jo::self<jo::Tokenizer>(this)->strings)->construct();  // 0x005a1730
        jo::self<jo::Tokenizer>(this)->vtable = img::obj(d::vt_Obj_9dcd98);
        return this;
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b93_05", name, addr, fn)
GAME("Object::ctor_684010", 0x00684010, &H::ctor_684010);
#undef GAME

}  // namespace game_b93_05
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {

struct H {
    // ------------------------------------------------------------------ 0x00684050
    void* sdtor_684050(u32 flags) {
        joan::orig::as_script_tokenizer(this)->destruct();  // 0x006840a0
        if ((flags & 1u) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Obj::sdtor_684050", 0x00684050, &H::sdtor_684050);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

struct H {
    // ------------------------------------------------------------------ 0x006840a0
    u32 dtor_6840a0() {
        jo::self<jo::Tokenizer>(this)->vtable = img::obj(d::vt_Obj_9dcd98);
        return joan::orig::arg::as<u32>(joan::orig::as_string_pool(&jo::self<jo::Tokenizer>(this)->strings)->destruct());  // 0x005a1770
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("ObjDtor_6840a0", 0x006840a0, &H::dtor_6840a0);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
namespace {
namespace game_b103_14 {

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

struct Obj {};

JOAN_COUNTER(c_split, "Tokenizer::split");

// 0x006840d0 pre-scan: replays the state machine without emitting and reports whether an
// identifier would reach 0x100 characters (the only state that writes the buffer without a
// bound). Exact as long as no overflow happened, which is all it has to decide. isspace
// (0x007b3bdd) is a pure table lookup, so the extra calls have no side effects.
bool split_overflows(const char* t) {
    auto is_space = [](char c) { return joan::orig::arg::as<i32>(joan::orig::libc::isspace(static_cast<i32>(c))) != 0; };
    i32 state = 1;
    i32 idx = 0;
    for (; static_cast<i32>(*t) != 0; ++t) {
        const char c = *t;
        switch (state) {
            case 1:
                if (!is_space(c)) {
                    idx = 1;
                    state = (c == '/' && t[1] == '/') ? 4 : (c == '"') ? 3 : 2;
                }
                break;
            case 2:
                if (is_space(c)) {
                    idx = 0;
                    state = 1;
                } else if (c == '(' || c == ')' || c == ',' || c == '"' || c == '{' || c == '}' || c == '/' ||
                           c == ':') {
                    if (c == '/' && t[1] == '/') {
                        state = 4;
                    } else {
                        idx = 0;
                        if (c == '"') {
                            idx = 1;
                            state = 3;
                        }
                    }
                } else {
                    if (idx >= 0x100) return true;  // buf[idx] write outside the 0x100-byte buffer
                    ++idx;
                }
                break;
            case 3:
                if (idx < 0x100) ++idx;
                if (c == '"' || c == '\r' || c == '\n') {
                    idx = 0;
                    state = 1;
                }
                break;
            case 4:
                if (c == '\n') {
                    idx = 0;
                    state = 1;
                }
                break;
            default: break;
        }
    }
    return false;
}

// Tokenizer frame emulation (see header notes).
// The original's stack frame: the token buffer, then the locals an overlong token runs into.
struct SplitFrame {
    u8 buf[0x104];     // 0x000
    i32 cap;           // 0x104
    i32 line;          // 0x108
    const char* text;  // 0x10c
    i32 state;         // 0x110
    void* self;        // 0x114
};
#if UINTPTR_MAX == 0xffffffffu  // the original (x86) frame
static_assert(offsetof(SplitFrame, cap) == 0x104 && offsetof(SplitFrame, line) == 0x108 &&
              offsetof(SplitFrame, text) == 0x10c && offsetof(SplitFrame, state) == 0x110 &&
              offsetof(SplitFrame, self) == 0x114 && sizeof(SplitFrame) == 0x118);
#endif

struct H {
    // ------------------------------------------------------------------ 0x006840d0
    u32 split(const char* text) {
        c_split.hit();
        // Overlong identifier: run the original body (thunk 0x00406edd redirected, body intact).
        // Batch 118/03: with fix/uninit enabled (module_enabled, read on the first call) the frame
        // emulation below handles it (writes past the emulated `this` dropped).
        if (!fix_uninit() && split_overflows(text)) return thiscall<u32>(fn::Tokenizer_split_body, this, text);
        SplitFrame fr;
        std::memset(&fr, 0, sizeof fr);
        fr.self = this;
        fr.state = 1;
        fr.text = text;
        fr.line = 1;
        fr.cap = 0x100;
        i32 idx = 0;

        auto put = [&](i32 k, char c) {  // buf[k] = c inside the emulated frame
            if (k >= 0 && k < static_cast<i32>(sizeof fr)) reinterpret_cast<u8*>(&fr)[k] = static_cast<u8>(c);
        };
        auto p = [&]() -> const char* { return fr.text; };
        auto self = [&]() -> void* { return fr.self; };
        auto emit = [&]() -> u32 {
            const i32 line = fr.line;
            // the handler (slot 1, e.g. ScriptLexer::add_token 0x006622d0) takes the pooled token text
            char* const tok = joan::orig::as_script_tokenizer(self())->add_string(reinterpret_cast<const char*>(fr.buf), joan::orig::arg::u32(idx));  // 0x00684590
            using Tok = char*;
            void* const s = self();
            using Virtual = u32 (Obj::*)(Tok, i32);
            JOAN_MEMBER_POINTER_CHECK(Virtual);
            Virtual method{};
            std::memcpy(&method, &joan::call::vtable_of(s)[1], sizeof(void*));
            return (reinterpret_cast<Obj*>(s)->*method)(tok, line);
        };
        auto is_space = [](char c) {
            return joan::orig::arg::as<i32>(joan::orig::libc::isspace(static_cast<i32>(c))) != 0;  // isspace
        };

        while (static_cast<i32>(*p()) != 0) {
            switch (static_cast<u32>(fr.state - 1)) {
                case 0:
                    if (!is_space(*p())) {
                        idx = 0;
                        put(idx, *p());
                        ++idx;
                        if (*p() == '/' && p()[1] == '/') {
                            fr.state = 4;
                        } else {
                            fr.state = (*p() == '"') ? 3 : 2;
                        }
                    }
                    break;
                case 1:
                    if (is_space(*p())) {
                        put(idx, 0);
                        emit();
                        idx = 0;
                        fr.state = 1;
                    } else if (*p() == '(' || *p() == ')' || *p() == ',' || *p() == '"' || *p() == '{' ||
                               *p() == '}' || *p() == '/' || *p() == ':') {
                        if (*p() == '/' && p()[1] == '/') {
                            fr.state = 4;
                        } else {
                            put(idx, 0);
                            emit();
                            idx = 0;
                            put(idx, *p());
                            ++idx;
                            if (*p() == '"') {
                                fr.state = 3;
                            } else {
                                put(idx, 0);
                                emit();
                                idx = 0;
                            }
                        }
                    } else {
                        put(idx, *p());
                        ++idx;
                    }
                    break;
                case 2:
                    if (idx < 0x100) {
                        put(idx, *p());
                        ++idx;
                    }
                    if (*p() == '"' || *p() == '\r' || *p() == '\n') {
                        put(idx, 0);
                        emit();
                        idx = 0;
                        fr.state = 1;
                    }
                    break;
                case 3:
                    if (*p() == '\n') {
                        idx = 0;
                        fr.state = 1;
                    }
                    break;
                default: break;
            }
            if (*p() == '\n') fr.line = fr.line + 1;
            const char* const next = p() + 1;
            fr.text = next;
        }
        put(idx, 0);
        return emit();
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("Tokenizer::split", 0x006840d0, &H::split);
#undef B103

}  // namespace game_b103_14
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

struct H {

    char* fwd_5a18d0(const char* a, u32 b) {  // 0x00684590 (a: the token text)
        return joan::orig::arg::as<char*>(joan::orig::as_string_pool(&jo::self<jo::Tokenizer>(this)->strings)->add(joan::orig::arg::str(a), joan::orig::arg::i32(b)));  // 0x005a18d0
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::fwd_5a18d0", 0x00684590, &H::fwd_5a18d0);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ScriptTokenizer::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_05::H::ctor_684010>(this)); }  // 0x684010
auto joan::orig::ScriptTokenizer::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_03::H::dtor_6840a0>(this)); }  // 0x6840a0
auto joan::orig::ScriptTokenizer::split(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_14::H::split>(this, a0)); }  // 0x6840d0
auto joan::orig::ScriptTokenizer::add_string(const char* a0, std::uint32_t a1) -> char* { return joan::bind::conv<char*>(joan::bind::member<&::game_b98_08::H::fwd_5a18d0>(this, a0, a1)); }  // 0x684590
