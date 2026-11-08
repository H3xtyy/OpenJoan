// game/unknown_ptoken.cpp: functions whose original source file is unknown (formerly game/_unplaced/ptoken.cpp;
// re/attribution.csv: between ww/enparticle/petoken.h and ww/enparticle/petoken.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

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

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

struct H {
    // ------------------------------------------------------------------ 0x0067caa0
    void* ctor_67caa0() {
        joan::orig::as_script_tokenizer(this)->construct();                          // 0x00684010
        joan::orig::dynarray(&jo::self<jo::TokenList>(this)->tokens)->construct(static_cast<i32>(sizeof(jo::Token)), 0x1e, 0);  // 0x005843d0 (Token: 0xc on x86)
        jo::view<std::uintptr_t>(jo::self<jo::TokenList>(this)->vtable) = img::addr(d::vt_Obj67c);
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj67caa0::Obj", 0x0067caa0, &H::ctor_67caa0);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

struct H {
    // ------------------------------------------------------------------------ 0x0067cb20
    void* scalar_delete(u32 flags) {
        joan::orig::as_token_list(this)->destruct();  // 0x0067cb70
        if ((flags & 1u) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(this));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Obj67c::scalar_delete_67cb20", 0x0067cb20, &H::scalar_delete);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

struct H {
    // -------------------------------------------------------------- 0x0067cb70
    u32 dtor_67cb70() {
        jo::view<std::uintptr_t>(jo::self<jo::TokenList>(this)->vtable) = img::addr(d::vt_Obj67c);
        joan::orig::dynarray(&jo::self<jo::TokenList>(this)->tokens)->destruct();  // 0x005844b0
        return joan::orig::arg::as<u32>(joan::orig::as_script_tokenizer(this)->destruct());     // 0x006840a0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B("ScriptInit::parse_63f8a0", 0x0063f8a0, &H::init_parse);
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B("ScriptExit::parse_6409f0", 0x006409f0, &H::exit_parse);
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B("ScriptVar::parse_6439d0", 0x006439d0, &H::var_parse);
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B("Script::parse_649140", 0x00649140, &H::script_parse);
B("Obj::dtor_67cb70", 0x0067cb70, &H::dtor_67cb70);
#undef B

}  // namespace game_b108_01
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
B103("cmp_name_pp_67d860", 0x0067d860, &cmp_name_pp<fn::cmp_name_pp_67d860>);
B103("cmp_name_sp_67d890", 0x0067d890, &cmp_name_sp<fn::cmp_name_sp_67d890>);
#undef B103

}  // namespace game_b103_14
}  // namespace


// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::TokenList::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_08::H::ctor_67caa0>(this)); }  // 0x67caa0
auto joan::orig::TokenList::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_01::H::dtor_67cb70>(this)); }  // 0x67cb70
