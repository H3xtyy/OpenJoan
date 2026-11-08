// uti3d/esstate2.cpp: functions of the original d:\ww\uti3d\esstate2.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/cxx_eh.hpp"
#include "../original/dynarray.hpp"
#include "../x87.hpp"
#include <cstdarg>
#include <cmath>
#include "../canvas_api.hpp"
#include <cstddef>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

namespace {
// ScriptNode::value holds a pointer or, for value_type 1 / 2, an i32 / float in its first 4 bytes (0x0063f140 hands
// out its address). x64: that value stays in the low 4 bytes of the 8-byte slot.
template <class T>
T& inline_value(void*& field) noexcept {
    static_assert(sizeof(T) == 4, "a 4-byte script value");
    return *reinterpret_cast<T*>(&field);
}
// The vtable entry of `o` at x86 byte offset `off` (slot off / 4), as a thiscall target.
inline std::uintptr_t vtable_slot(const void* o, std::int32_t off) noexcept {
    return reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(o)[joan::call::x86_vslot(off)]);
}
}  // namespace

// ---------------------------------------- from modules/game_b111_05.cpp (game/b111_05)
//
//    - ScriptLoad::parse_645de0   0x00645de0 ret 4: Script ( name ): looks the script up in 0xa50460 and
//      otherwise loads "<name>" from disk and compiles it with 0x00649280. FuncInfo 0x009efdf0: state 0 =
//      file object [ebp-0x2bc] (ctor 0x004179c0, dtor 0x005872b0 via 0x00405b55) -> RAII ScriptFile;
//      state 1 = operator delete 0x0063e4f0 (thunk 0x004057f4) of the 0x70-byte script while its ctor
namespace {
namespace game_b111_05 {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

// ---- reader / token helpers
u8* peek(void* rd) { return joan::orig::arg::as<u8*>(joan::orig::as_script_lexer(rd)->skip_newlines()); }  // 0x00642370
u8* lex(void* rd) { return joan::orig::arg::as<u8*>(joan::orig::as_script_lexer(rd)->next()); }   // 0x006423e0 (may throw char*)
using joan::orig::helpers::type;
u32 line(const u8* tok) { return jo::view<u32>(jo::as_token(tok)->line); }
void err(const void* self, img::Addr fmt, u32 ln) {  // Script::error 0x0063eb80 (cdecl, varargs)
    joan::orig::script::error(joan::orig::arg::mptr(self), str(fmt), joan::orig::arg::i32(ln));  // 0x0063eb80 (callers drop the eax)
}
constexpr img::Addr kScripts(d::g_scripts);

// operator delete of a new'd block while its ctor runs (unwind-map action of the originals).
struct DeleteOnUnwind {
    void* p;
    std::uintptr_t deleter;
    bool armed;
    DeleteOnUnwind(void* mem, std::uintptr_t fn) : p(mem), deleter(fn), armed(true) {}
    DeleteOnUnwind(const DeleteOnUnwind&) = delete;
    DeleteOnUnwind& operator=(const DeleteOnUnwind&) = delete;
    ~DeleteOnUnwind() noexcept(false) {
        if (armed) cdecl_call<void>(deleter, p);
    }
};

// File object [ebp-0x2bc] of 0x00645de0 (0x110-byte slot; ctor writes +0x105).
struct ScriptFile {
    alignas(alignof(void*)) u8 raw[sizeof(joan::orig::File) > 0x110 ? sizeof(joan::orig::File) : 0x110];  // 0x112 on x64
    ScriptFile() { joan::orig::as_file(raw)->construct(); }  // 0x004179c0
    ScriptFile(const ScriptFile&) = delete;
    ScriptFile& operator=(const ScriptFile&) = delete;
    ~ScriptFile() noexcept(false) { joan::orig::as_file(raw)->destruct(); }  // 0x005872b0
};

JOAN_COUNTER(c_load, "ScriptLoad::parse_645de0");

struct H {
    // ------------------------------------------------------------------ 0x00645de0
    void load_from_disk(u8* tok) {
        alignas(4) u8 path[0x194];  // [ebp-0x1ac], no dtor / no unwind action
        joan::orig::as_string(path)->construct();         // 0x005a1170
        joan::orig::as_string(path)->assign(joan::orig::arg::str(jo::as_token(tok)->text));  // 0x005a1440, ret 4
        ScriptFile file;                                   // state 0
        {
            const char* const p = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
            if (joan::orig::as_file(file.raw)->file_open(joan::orig::arg::str(p), 1, static_cast<char>(0)) == 0)  // 0x00586da0, ret 0xc
                err(this, d::s_Cannot_open_script, line(tok));                           // 'Cannot open script'
        }
        const i32 l1 = global<i16>(d::g_a19820) + 0x26;
        const u32 size1 = joan::orig::arg::as<u32>(joan::orig::as_file(file.raw)->file_size());  // 0x00587f30
        char* const buf = joan::orig::arg::as<char*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size1 + 1u), str(d::s_d_ww_uti3d_esstate2_cpp), joan::orig::arg::i32(l1)));  // 0x0058e7b0
        const u32 size2 = joan::orig::arg::as<u32>(joan::orig::as_file(file.raw)->file_size());
        joan::orig::as_file(file.raw)->file_read(joan::orig::arg::mptr(buf), joan::orig::arg::u32(size2));  // 0x005873a0, ret 8
        const u32 size3 = joan::orig::arg::as<u32>(joan::orig::as_file(file.raw)->file_size());
        buf[size3] = 0;
        void* const save_bc = global<void*>(d::g_parser_arena);
        void* const save_c0 = global<void*>(d::g_parser_arena_counter);
        global<void*>(d::g_parser_arena) = nullptr;
        global<void*>(d::g_parser_arena_counter) = nullptr;
        void* const mem = joan::orig::arg::as<void*>(joan::orig::pool::alloc(static_cast<std::uint32_t>(sizeof(jo::Script))));  // 0x0063e450 (0x70 on x86)
        void* script = nullptr;
        if (mem != nullptr) {
            DeleteOnUnwind guard(mem, fn::code_value(fn::Pool_free));          // state 1: 0x0063e4f0
            script = joan::orig::arg::as<void*>(joan::orig::as_ctrl(mem)->Ctrl_63f190());      // 0x0063f190
            guard.armed = false;
        }
        global<void*>(d::g_parser_arena) = save_bc;
        global<void*>(d::g_parser_arena_counter) = save_c0;
        thiscall<u32>(vtable_slot(script, 4), script, 0u);  // virtual slot +4
        jo::self<jo::ScriptLoad>(this)->script = joan::orig::arg::as<u32>(joan::orig::as_name_list(img::obj(kScripts))->add_648bd0(joan::orig::arg::str(jo::as_token(tok)->text), script));  // 0x00648bd0, ret 8
        const char* const p2 = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());
        joan::orig::as_script(script)->compile(joan::orig::arg::mptr(buf), p2);  // 0x00649280 (compile, catches), ret 8
        const i32 l2 = global<i16>(d::g_a19820) + 0x44;
        joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(buf), str(d::s_d_ww_uti3d_esstate2_cpp), joan::orig::arg::i32(l2));  // 0x0058ed70, ret 0xc
        joan::orig::as_file(file.raw)->file_close();   // 0x00587260
        const char* const p3 = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());
        const char* const dot = joan::orig::arg::as<const char*>(joan::orig::libc::strchr(joan::orig::arg::str(p3), 0x2e));  // strrchr
        const char* const p4 = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());
        const u32 count = static_cast<u32>(dot - p4);  // pointer difference narrowed
        const char* const p5 = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());
        char name[0x194];  // original: 0x100 bytes [ebp-0x3d0], see the header (no unwind action)
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(name)), joan::orig::arg::str(p5), joan::orig::arg::u32(count));  // strncpy
        const char* const p6 = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());
        const i32 end = static_cast<i32>(reinterpret_cast<std::uintptr_t>(dot) - reinterpret_cast<std::uintptr_t>(p6));
        name[end] = 0;
        joan::orig::as_widget(script)->load_ex_res(joan::orig::arg::str(name));  // 0x0063f580, ret 4
    }  // state -1, file dtor 0x005872b0
    std::uintptr_t parse_load(void* rd) {
        c_load.hit();
        global<u32>(d::g_a6a114) = global<u32>(d::g_a6a114) + 1u;
        u8* tok = peek(rd);
        if (type(tok) != 0x1c) err(this, d::s_Expecting_Script, line(tok));  // 'Expecting Script!'
        tok = lex(rd);
        if (type(tok) != -0x25) err(this, d::s_Expecting_after_Script, line(tok));  // 'Expecting ( after Script!'
        tok = lex(rd);
        if (type(tok) != 3) err(this, d::s_Expecting_script_name, line(tok));  // 'Expecting script name'
        const u32 old = jo::self<jo::ScriptLoad>(this)->script;
        jo::self<jo::ScriptLoad>(this)->script = joan::orig::arg::as<u32>(joan::orig::as_list(img::obj(kScripts))->find_name_648c50(joan::orig::arg::str(jo::as_token(tok)->text)));  // 0x00648c50, ret 4
        if (jo::self<jo::ScriptLoad>(this)->script == 0) load_from_disk(tok);
        else joan::orig::as_script_cache(img::obj(kScripts))->add_ref(joan::orig::arg::u32(jo::self<jo::ScriptLoad>(this)->script));  // 0x00648d20, ret 4
        if (old != 0) joan::orig::as_script_cache(img::obj(kScripts))->release(joan::orig::arg::i32(old));  // 0x00648d60, ret 4
        tok = lex(rd);
        if (type(tok) != -0x24) err(this, d::s_Expecting_for_the_ending_scr, line(tok));  // 'Expecting ) for the ending script!'
        return reinterpret_cast<std::uintptr_t>(lex(rd));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_05", name, addr, fn)
B("ScriptLoad::parse_645de0", 0x00645de0, &H::parse_load);
#undef B

}  // namespace game_b111_05
}  // namespace

// ---------------------------------------- from modules/game_b90_05.cpp (game/b90_05)
//
//    - Camera::set_ranges 0x00613ca0 (stores positive values to +0x44 / +0x4c / +0x50, then
//      0x00614170), Grid::to_cell 0x006152b0 (offsets from the origin, inside test),
//      Camera::screen_ray 0x006154d0 (two points through the view matrix),
//      Viewport::set_size 0x00616a00, Viewport::set_58 0x00616a60, CamParams::init 0x00616d50,
//      Rect9::clear 0x00620a20, Widget::~Widget 0x006211a0, Button::create 0x00621540,
//      Button::enabled 0x006226e0, Button::set_text 0x00623ce0, TextDesc::clear 0x00623dc0,
//      Image2D::create 0x00629530, Label::create 0x00629660, List::get 0x0062a430,
//      Scroll::set_pos 0x0062be50, clamp 0x0062c200 (cdecl), Pool::alloc 0x0063e450 (cdecl),
//      Pool::free 0x0063e4f0 (cdecl), Reader::Reader 0x0063e550, Reader::list_get 0x0063e9f0,
//      Script::error 0x0063eb80 (cdecl varargs), Reader::count 0x0063ecd0,
//      Reader::next 0x0063ee20, Obj::size_14 0x00641a40.
namespace {
namespace game_b90_05 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

std::size_t c_strlen(const char* s) { return std::strlen(joan::orig::arg::str(s)); }
char* c_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_script_error, "Script::error");

// 0x0063e450: 4-aligned allocation from the pool (0x00a6a0bc) or the CRT heap.
void* JOAN_CDECL pool_alloc(std::uint32_t n) {
    if (global<void*>(d::g_parser_arena) == nullptr) return joan::crt::operator_new(joan::orig::arg::u32(n));
    const std::uintptr_t p = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_string_pool(global<void*>(d::g_parser_arena))->add(joan::orig::arg::str(0), joan::orig::arg::i32(n + 3)));  // 0x005a18d0
    const std::uintptr_t r = p % 4u;
    std::int32_t* const count = global<std::int32_t*>(d::g_parser_arena_counter);
    *count = *count + 1;
    if (r == 0) return reinterpret_cast<void*>(p);
    return reinterpret_cast<void*>(p + (4u - r));
}

// 0x0063eb80 Script::error(unused, fmt, line, ...): "<file> line:N <message>" into the global
// buffer 0x00a69f38, then Error::fatal(0x00a47d78, buffer) (buffer used as the format).
void JOAN_CDECL script_error(void* unused, const char* fmt, std::int32_t line, ...) {  // unused: the caller's this
    c_script_error.hit();
    (void)unused;
    char* const buf = &img::global<char>(d::g_a69f38);
    buf[0] = 0;
    std::int32_t n = 0;
    if (global<const char*>(d::g_sce_script_name) != nullptr) {
        c_strcpy(buf + n, global<const char*>(d::g_sce_script_name));
        n = n + static_cast<std::int32_t>(c_strlen(buf + n));
        c_strcpy(buf + n, &img::global<const char>(d::s_sp_9d05f0));  // " "
        n = n + 1;
    }
    if (line > 0) {
        joan::orig::libc::sprintf(joan::orig::arg::str(buf + n), joan::orig::arg::str(&img::global<const char>(d::s_line_d)), line);  // sprintf "line:%d "
        n = n + static_cast<std::int32_t>(c_strlen(buf + n));
    }
    if (fmt != nullptr) {
        va_list args;
        va_start(args, line);
        joan::orig::libc::vsprintf(joan::orig::arg::str(buf + n), joan::orig::arg::str(fmt), joan::orig::arg::str(static_cast<void*>(args)));  // vsprintf
        n = n + static_cast<std::int32_t>(c_strlen(buf + n));
        va_end(args);
    }
    joan::orig::error::fatal(joan::orig::arg::mptr(img::obj(d::g_err)), joan::orig::arg::str(static_cast<const char*>(buf)));  // 0x00586cb0
}

#define B9005(name, addr, fn) JOAN_REPLACE("game/b90_05", name, addr, fn)
B9005("Pool::alloc", 0x0063e450, &pool_alloc);
B9005("Script::error", 0x0063eb80, &script_error);
#undef B9005

}  // namespace game_b90_05
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
//
//      0x00648d60 SharedRes::release(key)                       thiscall, ret 4
namespace {
namespace game_b92_05 {

using joan::hook::global;

struct Obj {};
using joan::call::thiscall;
using joan::orig::helpers::win_call;
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

JOAN_COUNTER(c_shared_release, "SharedRes::release");

struct H {
    // 0x0063f190
    H* ctrl_ctor_63f190() {
        joan::orig::as_script_block(this)->construct();  // 0x0063e6e0
        jo::self<jo::Script>(this)->vtable = img::obj(d::vt_Obj_9da9c8);
        jo::self<jo::Script>(this)->strings = nullptr;
        jo::self<jo::Script>(this)->kind = 1;
        jo::self<jo::Script>(this)->string_pool = nullptr;
        jo::view<std::int32_t>(jo::self<jo::Script>(this)->pool_state) = 0;
        return this;
    }

    // 0x00648d60
    std::int32_t shared_release(std::int32_t key) {
        c_shared_release.hit();
        std::uint8_t* const res = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(key)));  // 0x00417200
        const std::int32_t n = win_call<std::int32_t>(d::iat_KERNEL32_InterlockedDecrement, static_cast<void*>(res));  // InterlockedDecrement
        if (n != 0) return n;
        void* const s0 = global<void*>(d::g_parser_arena);
        void* const s1 = global<void*>(d::g_parser_arena_counter);
        global<void*>(d::g_parser_arena) = nullptr;
        global<void*>(d::g_parser_arena_counter) = nullptr;
        void* const o = jo::as_name_list_entry(res)->value;
        if (o != nullptr) virtual_call<void*>(o, 0, 1);  // scalar deleting destructor
        global<void*>(d::g_parser_arena) = s0;
        global<void*>(d::g_parser_arena_counter) = s1;
        joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(key));  // 0x005858c0
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("Ctrl::Ctrl_63f190", 0x0063f190, &H::ctrl_ctor_63f190);
B("SharedRes::release", 0x00648d60, &H::shared_release);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
//
//      0x00642370 Obj642::skip_9()                 - skip tokens of type 9 on +0xc, counting +0x29
namespace {
namespace game_b91_04 {

using joan::call::thiscall;

struct H {
    void* self() { return this; }

    // 0x00642370
    void* skip_9() {
        void* tok = joan::orig::arg::as<void*>(joan::orig::as_pool(&jo::self<jo::ScriptLexer>(this)->tokens)->last_used());  // 0x004b9c30
        while (jo::as_token(tok)->kind == 9) {
            jo::self<jo::ScriptLexer>(this)->i32_29 = jo::self<jo::ScriptLexer>(this)->i32_29 + 1;
            joan::orig::as_reader(&jo::self<jo::ScriptLexer>(this)->tokens)->next();                // 0x0063ee20
            tok = joan::orig::arg::as<void*>(joan::orig::as_pool(&jo::self<jo::ScriptLexer>(this)->tokens)->last_used());  // 0x004b9c30
        }
        return tok;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj642::skip_9", 0x00642370, &H::skip_9);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b111_01.cpp (game/b111_01)
//
//   0x006423e0 ScriptLexer::next          (thiscall, ret)    throws "Unexpected end-of-file."
namespace {
namespace game_b111_01 {

using joan::call::thiscall;
using joan::img::str;

using i32 = std::int32_t;

[[noreturn]] void throw_message(img::Addr text) {
    throw const_cast<char*>(str(text));  // throw (char*) (original/cxx_eh.hpp)
}

struct H {
    // ------------------------------------------------------------------ 0x006423e0
    void* lexer_next() {
        if (joan::orig::arg::as<i32>(joan::orig::as_range(&jo::self<jo::ScriptLexer>(this)->tokens)->at_end()) != 0)  // 0x00642440 at end
            throw_message(d::s_Unexpected_end_of_file);                       // "Unexpected end-of-file."
        joan::orig::as_reader(&jo::self<jo::ScriptLexer>(this)->tokens)->next();          // 0x0063ee20 advance
        return joan::orig::arg::as<void*>(joan::orig::as_script_lexer(this)->skip_newlines());            // 0x00642370 current
    }
};

#define B111(name, addr, fn) JOAN_REPLACE("game/b111_01", name, addr, fn)
B111("ScriptLexer::next", 0x006423e0, &H::lexer_next);
#undef B111

}  // namespace game_b111_01
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
//
//    - NameList::add 0x00648bd0 (name, v), ret 8: new slot, strcpy(+4), +0x108 = v, +0 = 1.
namespace {
namespace game_b98_06 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::str;

// false positive: the destination is the name field of a name list entry, a trailing array in the original layout
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

JOAN_COUNTER(c_name_add, "NameList::add");

struct H {
    // ------------------------------------------------------------------ 0x00648bd0
    i32 name_add(const char* name, void* v) {
        c_name_add.hit();
        joan::orig::dynarray_b(this)->linkin(nullptr);          // 0x005857a0
        const i32 k = joan::orig::as_dynarray(this)->cursor();  // 0x00417520
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(k)));  // 0x00417200
        crt_strcpy(jo::as_name_list_entry(e)->name, name);
        jo::as_name_list_entry(e)->value = static_cast<jo::Script*>(v);
        jo::as_name_list_entry(e)->refs = 1;
        return k;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("NameList::add", 0x00648bd0, &H::name_add);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b100_12.cpp (game/b100_12)
//
//    - List::find_name 0x00648c50 (name), ret 4: last index whose entry name (+4) strcmp-equals.
namespace {
namespace game_b100_12 {

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

JOAN_COUNTER(c_find_name, "List::find_name");

struct H {
    // ------------------------------------------------------------------ 0x00648c50
    i32 find_name(const char* name) {
        c_find_name.hit();
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_script_cache(this)->is_free(joan::orig::arg::u32(i))) != 0) continue;    // 0x00648e40
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(i)));  // 0x00417200
            if (joan::crt::stricmp(joan::orig::arg::str(name), joan::orig::arg::str(reinterpret_cast<const char*>(e + 4))) == 0) return i;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_12", name, addr, fn)
B("List::find_name", 0x00648c50, &H::find_name);
#undef B

}  // namespace game_b100_12
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
//
//      0x00648d20 Obj::add_ref(key)                 (thiscall, ret 4) - InterlockedIncrement.
namespace {
namespace game_b96_06 {

using joan::hook::global;

// KERNEL32 / ADVAPI32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;

using i32 = std::int32_t;
using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x00648d20
    i32 add_ref(u32 key) {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(key)));  // 0x00417200
        return win_call<i32>(d::iat_KERNEL32_InterlockedIncrement, p);                     // InterlockedIncrement
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Obj::add_ref", 0x00648d20, &H::add_ref);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b111_01.cpp (game/b111_01)
//
//   0x00649280 ScriptStatementCompile::compile (thiscall, ret 8), the catcher (FuncInfo 0x9eff30)
//
// 0x00649280 FuncInfo 0x9eff30 (decoded):
//   unwind state 0 -> -1: lexer [ebp-0x44] dtor 0x00403bd4 (0x00661f70)    -> LexerOwner
//   unwind state 1 ->  0: operator delete 0x007b1d2a([ebp-0x53c]) while the
//                          8-byte object from operator new is being constructed -> NewGuard
//   states 2 / 3: no action. try 2..2 (catchHigh 3), catches in order:
//     EventSystemToken* (disp -0x54)  funclet 0x6493a0
//     char*             (disp -0x204) funclet 0x6494c2
//     ...                             funclet 0x6495cc
//   every funclet returns to a continuation that destroys the lexer and returns al = 0.
//   The catch funclets build a 0x191-byte fixed string (0x005a1170 / 0x005a11a0 family)
//   in the parent frame and never destroy it (no destructor exists, no heap): plain locals.
namespace {
namespace game_b111_01_2 {

using joan::hook::global;

struct Obj {};
using joan::call::thiscall;
using joan::call::cdecl_call;
template <class R = void, class... A>
R vcall(const void* self, std::int32_t slot_offset, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(self)[joan::call::x86_vslot(slot_offset)], sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}
using joan::img::str;

using i32 = std::int32_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_compile, "ScriptStatementCompile::compile");

using joan::orig::helpers::strlen_;

// ---------------------------------------------------------------- 0x00649280 helpers
struct LexerOwner {  // unwind state 0: lexer destructor
    void* lexer;
    ~LexerOwner() { joan::orig::as_script_lexer(lexer)->destruct(); }  // 0x00661f70
};
struct NewGuard {  // unwind state 1: operator delete of the object under construction
    void* memory;
    bool armed;
    ~NewGuard() {
        if (armed) joan::crt::operator_delete(joan::orig::arg::mptr(memory));  // operator delete
    }
};

struct alignas(4) FixedString {  // 0x191-byte string (frame slot 0x194 bytes)
    char text[0x194];
};
void s_append(FixedString& s, const char* t) { joan::orig::as_string(&s)->append(joan::orig::arg::str(t)); }  // 0x005a1550
void s_append_int(FixedString& s, i32 v) { joan::orig::as_string(&s)->append(joan::orig::arg::i32(v)); }  // 0x005a15d0
char* s_cstr(FixedString& s) { return joan::orig::as_string(&s)->c_str(); }  // 0x00417d90

struct CompileSaved {
    char* a6a218;  // [ebp-0x48]
    void* a6a0bc;  // [ebp-0x4c]
    void* a6a0c0;  // [ebp-0x50]
};

// Common tail of the three catch funclets: log, report, reset, restore the globals.
void report_error(void* self, FixedString& s, const CompileSaved& saved) {
    joan::orig::warning_box(joan::orig::arg::mptr(img::obj(d::g_err)), joan::orig::arg::str(s_cstr(s)));  // 0x00586bc0 (varargs)
    joan::orig::as_script_statement_compile(self)->set_error_text(joan::orig::arg::str(s_cstr(s)));                           // 0x006498a0
    vcall(self, 0x8);
    if (jo::as_script(self)->string_pool != nullptr) {
        global<void*>(d::g_parser_arena) = saved.a6a0bc;
        global<void*>(d::g_parser_arena_counter) = saved.a6a0c0;
    }
    global<char*>(d::g_sce_script_name) = saved.a6a218;
}

// funclets 0x006494c2 (char*) and 0x006495cc (...): message, lexer line, current token.
void report_with_token(void* self, void* lexer, const char* message, const CompileSaved& saved) {
    FixedString s;
    joan::orig::as_string(&s)->construct(joan::orig::arg::str(message));  // 0x005a11a0
    s_append(s, str(d::s_Line_number_9db814));             // "\nLine number: "
    s_append_int(s, jo::as_script_lexer(lexer)->i32_29);    // [ebp-0x1b]
    void* const token = joan::orig::arg::as<void*>(joan::orig::as_script_lexer(lexer)->skip_newlines());  // 0x00642370
    s_append(s, str(d::s_Error_token_string));             // "\nError token string: "
    s_append(s, jo::as_token(token)->text);
    s_append(s, str(d::s_Error_token_type));             // "\nError token type: "
    s_append_int(s, jo::as_token(token)->kind);
    report_error(self, s, saved);
}

// try body (state 2); no objects with destructors here.
void compile_body(void* self, void* lexer, void* source) {
    joan::orig::as_script_tokenizer(lexer)->split(joan::orig::arg::str(source));  // 0x006840d0
    joan::orig::as_script_lexer(lexer)->rewind();          // 0x006498e0
    vcall(self, 0x10, lexer);
}

// MSVC 6's catch (...) of the original also took structured exceptions (FindHandlerForForeignException 0x007b8586);
// this port's catch (...) takes C++ exceptions only. Nothing on the try path faults with a well-formed script: the
// lexer checks its end (0x006423e0 / 0x00642440: "Unexpected end-of-file." is a C++ throw), Tokenizer::split
// 0x006840d0 always appends a final token, and the statement compilers report bad input with C++ throws. A fault
// there would be a bug of the port (crash handler instead of "Unhandled exception in ..."); the test54 gate replays
// the default run through this catcher.
struct H {
    // ------------------------------------------------------------------ 0x00649280
    bool compile(void* source, const char* context) {
        c_compile.hit();
        void* const self = this;
        vcall(self, 0x8);
        joan::orig::as_script_statement_compile(self)->set_error_text(str(d::s_0_error_s_you_re_good_to_go));  // "0 error(s), you're good to go!"
        alignas(alignof(void*)) unsigned char lexer[((sizeof(jo::ScriptLexer) > 0x30 ? sizeof(jo::ScriptLexer) : 0x30) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // [ebp-0x44] (x64: the larger ScriptLexer)
        joan::orig::as_list(lexer)->construct();                   // 0x00661ea0
        LexerOwner owner{lexer};
        CompileSaved saved{global<char*>(d::g_sce_script_name), nullptr, nullptr};
        global<char*>(d::g_sce_script_name) = const_cast<char*>(context);
        if (jo::view<void*>(jo::self<jo::Script>(this)->string_pool) == nullptr) {
            void* const memory = joan::crt::operator_new(static_cast<u32>(sizeof(jo::StringPool)));  // operator new (8 bytes on x86: head, tail)
            void* object = nullptr;
            if (memory != nullptr) {
                NewGuard guard{memory, true};
                object = joan::orig::as_string_pool(memory)->construct();  // 0x005a1730
                guard.armed = false;
            }
            jo::view<void*>(jo::self<jo::Script>(this)->string_pool) = object;
            joan::orig::as_string_pool(jo::view<void*>(jo::self<jo::Script>(this)->string_pool))->reset();  // 0x005a17a0
        }
        saved.a6a0bc = global<void*>(d::g_parser_arena);
        saved.a6a0c0 = global<void*>(d::g_parser_arena_counter);
        global<void*>(d::g_parser_arena) = jo::view<void*>(jo::self<jo::Script>(this)->string_pool);
        global<void*>(d::g_parser_arena_counter) = &jo::self<jo::Script>(this)->pool_state;
        try {  // state 2
            compile_body(self, lexer, source);
        } catch (EventSystemToken* token) {  // funclet 0x006493a0
            FixedString s;
            joan::orig::as_string(&s)->construct();         // 0x005a1170
            joan::orig::as_string(&s)->assign(str(d::s_Bad_token));            // 0x005a1440 "Bad token \n"
            if (strlen_(jo::as_token(token)->text) < 0x14u) {
                s_append(s, jo::as_token(token)->text);
            } else {
                char cut[0x18];  // [ebp-0x200]
                std::strncpy(static_cast<char*>(joan::orig::arg::mptr(cut)), joan::orig::arg::str(jo::as_token(token)->text), 0x12u);  // strncpy
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(cut + 0x12)), str(d::s_dot_dot_dot));  // strcpy "..."
                s_append(s, cut);
            }
            s_append(s, str(d::s_Line_number_9db828));  // "\nLine number : "
            s_append_int(s, jo::as_token(token)->line);
            report_error(self, s, saved);
            return false;
        } catch (char* message) {  // funclet 0x006494c2
            report_with_token(self, lexer, message, saved);
            return false;
        } catch (...) {  // funclet 0x006495cc
            report_with_token(self, lexer, str(d::s_Unhandled_exception_in_Scrip), saved);  // "Unhandled exception in ..."
            return false;
        }
        if (jo::self<jo::Script>(this)->string_pool != nullptr) {
            global<void*>(d::g_parser_arena) = saved.a6a0bc;
            global<void*>(d::g_parser_arena_counter) = saved.a6a0c0;
        }
        global<char*>(d::g_sce_script_name) = saved.a6a218;
        return true;
    }
};

#define B111(name, addr, fn) JOAN_REPLACE("game/b111_01", name, addr, fn)
B111("ScriptStatementCompile::compile", 0x00649280, &H::compile);
#undef B111

}  // namespace game_b111_01_2
}  // namespace

// ---------------------------------------- from modules/game_b90_05_2.cpp (game/b90_05)
namespace {
namespace game_b90_05_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::hook::global;


void* c_memset(void* p, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // 0x0063e550
    void* reader_ctor() {
        jo::self<jo::ScriptNode>(this)->vtable = img::obj(d::vt_Obj_9da944);
        c_memset(&jo::self<jo::ScriptNode>(this)->parent, 0, sizeof(jo::ScriptNode) - offsetof(jo::ScriptNode, parent));  // parent .. value
        jo::self<jo::ScriptNode>(this)->kind = -1;
        jo::self<jo::ScriptNode>(this)->value_type = 0x22;
        return this;
    }
    // 0x0063e9f0
    void* reader_list_get(std::int32_t i) { return joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements))->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0
    // 0x0063ecd0
    std::int32_t reader_count() {
        if (jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements) == nullptr) return 0;
        return joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements))->size();  // 0x00417ad0
    }
    // 0x0063ee20
    std::int32_t reader_next() {
        if (jo::self<jo::DynArray>(this)->cur_pos < jo::self<jo::DynArray>(this)->last_ele) {
            jo::self<jo::DynArray>(this)->cur_pos = jo::self<jo::DynArray>(this)->cur_pos + 1;
            return 1;
        }
        return 0;
    }
    // 0x00641a40
    std::int32_t size_14() { return 0x14; }
};

// 0x0063e4f0
void JOAN_CDECL pool_free(void* p) {
    if (global<void*>(d::g_parser_arena) == nullptr) {
        joan::crt::operator_delete(joan::orig::arg::mptr(p));
        return;
    }
    if (p == nullptr) return;
    std::int32_t* const count = global<std::int32_t*>(d::g_parser_arena_counter);
    *count = *count - 1;
}

#define B9005(name, addr, fn) JOAN_REPLACE("game/b90_05", name, addr, fn)
B9005("Pool::free", 0x0063e4f0, &pool_free);
B9005("Reader::Reader", 0x0063e550, &H::reader_ctor);
B9005("Reader::list_get", 0x0063e9f0, &H::reader_list_get);
B9005("Reader::count", 0x0063ecd0, &H::reader_count);
B9005("Reader::next", 0x0063ee20, &H::reader_next);
B9005("Obj::size_14", 0x00641a40, &H::size_14);
#undef B9005

}  // namespace game_b90_05_2
}  // namespace

// ---------------------------------------- from modules/game_b103_13.cpp (game/b103_13)
namespace {
namespace game_b103_13 {

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using joan::call::thiscall;

void node_delete(void* p) { joan::orig::pool::free_(joan::orig::arg::mptr(p)); }  // 0x0063e4f0

struct H {
    // Scalar deleting destructor: dtor (via its thunk), operator delete when flags & 1.
    template <fn::Code Dtor>
    H* sdtor(u32 flags) {
        thiscall(Dtor, this);
        if ((flags & 1) != 0) node_delete(this);
        return this;
    }

    void* list_last_63ed10() { return joan::orig::as_table(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements))->last(); }  // 0x00585f20 (the last record)
    u32 list_next_63ed70() { return joan::orig::arg::as<u32>(joan::orig::as_reader(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements))->next()); }  // 0x0063ee20
    std::uintptr_t find_by_name(const char* name) {  // 0x0063edd0: the node (pointer) named `name`, or 0
        if (jo::view<void*>(jo::self<jo::ScriptNode>(this)->parent) == nullptr) return 0;
        return virtual_call<std::uintptr_t>(jo::view<void*>(jo::self<jo::ScriptNode>(this)->parent), 0x18, name);
    }
    std::uintptr_t find_by_id(u32 key) {  // 0x006436b0: the node (pointer) with id `key`, or 0
        if (jo::view<void*>(jo::self<jo::ScriptNode>(this)->parent) == nullptr) return 0;
        return virtual_call<std::uintptr_t>(jo::view<void*>(jo::self<jo::ScriptNode>(this)->parent), 0x14, key);
    }
    void* data() {  // 0x0063f140
        const i32 kind = jo::self<jo::ScriptNode>(this)->value_type;
        if (kind == 1) return &jo::self<jo::ScriptNode>(this)->value;
        if (kind == 2) return &jo::self<jo::ScriptNode>(this)->value;
        return jo::self<jo::ScriptNode>(this)->value;
    }

    template <fn::Code Base, u32 Plus>
    u32 size_plus() {
        return thiscall<u32>(Base, this) + Plus;
    }
    template <class M>
    struct owner;
    template <class C, class T>
    struct owner<T C::*> {
        using type = C;
    };
    template <fn::Code Base, auto Field>
    u32 size_array() {  // 0x00641e30 / 0x00642310
        u32 n = thiscall<u32>(Base, this) + 4;
        if (jo::self<typename owner<decltype(Field)>::type>(this)->*Field != nullptr) {
            const jo::DynArray* const p = jo::self<typename owner<decltype(Field)>::type>(this)->*Field;
            n += jo::view<u32>(p->ele_num) * jo::view<u32>(p->ele_size);
        }
        return n;
    }
    template <fn::Code Base, auto Field>
    u32 size_child() {  // 0x00641ec0 / 0x00642250
        u32 n = thiscall<u32>(Base, this) + 4;
        void* const child = jo::self<typename owner<decltype(Field)>::type>(this)->*Field;
        if (child != nullptr) n += virtual_call<u32>(child, 0x2c);
        return n;
    }
    void* ref_data() { return virtual_call<void*>(jo::view<void*>(jo::self<jo::Value>(this)->ref), 0x1c); }  // 0x00644330
    u32 ref_kind() { return jo::view<u32>(jo::self<jo::Value>(this)->ref->value_type); }               // 0x00644360
};

constexpr fn::Code kSize14 = fn::Obj_size_14;  // 0x00641a40 (returns 0x14)
constexpr fn::Code kSizeA60 = fn::Obj641_size_641a60;  // 0x00641a60
constexpr fn::Code kSizeC30 = fn::Obj641_size_641c30;  // 0x00641c30
constexpr fn::Code kSize190 = fn::Ctrl_height_plus_2;  // 0x00642190

#define B(name, addr, fn) JOAN_REPLACE("game/b103_13", name, addr, fn)
B("Node::sdtor_63e5b0", 0x0063e5b0, &H::sdtor<fn::Ctrl_dtor_Ctrl_63e600>);  // 0x0063e600
B("Node::sdtor_63e730", 0x0063e730, &H::sdtor<fn::Obj63e7_dtor>);  // 0x0063e780
B("Node::list_last_63ed10", 0x0063ed10, &H::list_last_63ed10);
B("Node::list_next_63ed70", 0x0063ed70, &H::list_next_63ed70);
B("Leaf::find_by_name", 0x0063edd0, &H::find_by_name);
B("Node::data", 0x0063f140, &H::data);
B("Node::sdtor_640aa0", 0x00640aa0, &H::sdtor<fn::Obj_dtor_Obj_640af0>);  // 0x00640af0
B("Node::size_641c80", 0x00641c80, (&H::size_plus<kSizeA60, 5>));
B("Node::size_641cb0", 0x00641cb0, (&H::size_plus<kSizeC30, 1>));
B("Node::size_641ce0", 0x00641ce0, (&H::size_plus<kSizeC30, 1>));
B("Node::size_641d10", 0x00641d10, (&H::size_plus<kSize14, 4>));
B("Node::size_641d40", 0x00641d40, (&H::size_plus<kSize14, 4>));
B("Node::size_641d70", 0x00641d70, (&H::size_plus<kSize14, 4>));
B("Node::size_641da0", 0x00641da0, (&H::size_plus<kSizeA60, 1>));
B("Node::size_641dd0", 0x00641dd0, (&H::size_plus<kSize14, 4>));
B("Node::size_641e00", 0x00641e00, (&H::size_plus<kSize14, 0>));
B("Node::size_641e30", 0x00641e30, (&H::size_array<kSize14, &jo::ScriptChoice::options>));
B("Node::size_641e90", 0x00641e90, (&H::size_plus<kSizeA60, 0>));
B("Node::size_641ec0", 0x00641ec0, (&H::size_child<kSizeA60, &jo::ScriptIf::else_branch>));
B("Node::size_642040", 0x00642040, (&H::size_plus<kSize14, 0x34>));
B("Node::size_6421c0", 0x006421c0, (&H::size_plus<kSize190, 0>));
B("Node::size_6421f0", 0x006421f0, (&H::size_plus<kSize190, 0>));
B("Node::size_642220", 0x00642220, (&H::size_plus<kSize190, 2>));
B("Node::size_642250", 0x00642250, (&H::size_child<kSize14, &jo::Value::ref>));
B("Node::size_6422b0", 0x006422b0, (&H::size_plus<kSize14, 0xc>));
B("Node::size_6422e0", 0x006422e0, (&H::size_plus<kSize14, 4>));
B("Node::size_642310", 0x00642310, (&H::size_array<kSizeA60, &jo::MathExpr::rpn>));
B("Node::sdtor_6425b0", 0x006425b0, &H::sdtor<fn::Obj_dtor_642600>);  // 0x00642600
B("Node::sdtor_642810", 0x00642810, &H::sdtor<fn::Obj_dtor_Obj_642860>);  // 0x00642860
B("Node::sdtor_6428d0", 0x006428d0, &H::sdtor<fn::Obj_dtor_642920>);  // 0x00642920
B("Node::sdtor_642990", 0x00642990, &H::sdtor<fn::Widget_dtor_6429e0>);  // 0x006429e0
B("Node::sdtor_642a60", 0x00642a60, &H::sdtor<fn::Obj_dtor_Obj_642ab0>);  // 0x00642ab0
B("Node::sdtor_642b20", 0x00642b20, &H::sdtor<fn::Obj_dtor_642b70>);  // 0x00642b70
B("Node::sdtor_642be0", 0x00642be0, &H::sdtor<fn::Obj_dtor_642c30>);  // 0x00642c30
B("Node::sdtor_642ca0", 0x00642ca0, &H::sdtor<fn::Obj_dtor_642cf0>);  // 0x00642cf0
B("Node::sdtor_642d60", 0x00642d60, &H::sdtor<fn::Obj_dtor_642db0>);  // 0x00642db0
B("Node::sdtor_642e60", 0x00642e60, &H::sdtor<fn::Node_dtor_Node_642eb0>);  // 0x00642eb0
B("Node::sdtor_642f80", 0x00642f80, &H::sdtor<fn::Node_dtor_642fd0>);  // 0x00642fd0
B("Node::sdtor_643040", 0x00643040, &H::sdtor<fn::Obj_dtor_Obj_643090>);  // 0x00643090
B("Node::sdtor_643100", 0x00643100, &H::sdtor<fn::Obj_dtor_643150>);  // 0x00643150
B("Node::sdtor_6431c0", 0x006431c0, &H::sdtor<fn::Obj_dtor_643210>);  // 0x00643210
B("Node::sdtor_643280", 0x00643280, &H::sdtor<fn::Obj_dtor_6432d0>);  // 0x006432d0
B("Node::sdtor_643340", 0x00643340, &H::sdtor<fn::Obj_dtor_Obj_643390>);  // 0x00643390
B("Node::sdtor_643400", 0x00643400, &H::sdtor<fn::Node643_dtor>);  // 0x00643450
B("Node::sdtor_6434d0", 0x006434d0, &H::sdtor<fn::Obj_dtor_Obj_645ca0>);  // 0x00645ca0
B("Leaf::find_by_id", 0x006436b0, &H::find_by_id);
B("Node::sdtor_643e80", 0x00643e80, &H::sdtor<fn::Obj_dtor_643ed0>);  // 0x00643ed0
B("Ref::data", 0x00644330, &H::ref_data);
B("Ref::kind", 0x00644360, &H::ref_kind);
B("Node::sdtor_644430", 0x00644430, &H::sdtor<fn::Obj_dtor_644480>);  // 0x00644480
B("Node::sdtor_644800", 0x00644800, &H::sdtor<fn::Node_dtor_Node_644850>);  // 0x00644850
#undef B

}  // namespace game_b103_13
}  // namespace

// ---------------------------------------- from modules/game_b92_05_2.cpp (game/b92_05)
namespace {
namespace game_b92_05_2 {

using joan::call::thiscall;

struct H {
    // 0x0063e600
    std::int32_t ctrl_dtor() {
        jo::self<jo::ScriptNode>(this)->vtable = img::obj(d::vt_Obj_9da944);
        joan::orig::as_script_node(this)->free_value();  // 0x0063e660
        return 0;  // void callee; destructor, result unused
    }

    // 0x00642190
    std::int32_t height_plus_2() { return joan::orig::arg::as<std::int32_t>(joan::orig::as_script_block(this)->byte_size()) + 2; }  // 0x00641a60

    // 0x00643000
    H* ctrl_ctor_643000() {
        joan::orig::as_script_block(this)->construct();  // 0x0063e6e0
        jo::self<jo::ScriptFuncCall>(this)->vtable = img::obj(d::vt_ScriptNode_9db040);
        jo::self<jo::ScriptFuncCall>(this)->kind = 0x15;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("Ctrl::~Ctrl_63e600", 0x0063e600, &H::ctrl_dtor);
B("Ctrl::height_plus_2", 0x00642190, &H::height_plus_2);
B("Ctrl::Ctrl_643000", 0x00643000, &H::ctrl_ctor_643000);
#undef B

}  // namespace game_b92_05_2
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x0063e630
    void set_4(jo::ScriptBlock* v) { jo::self<jo::ScriptNode>(this)->parent = v; }

    // ------------------------------------------------------------------------ 0x0063e780
    u32 dtor_63e780() {
        jo::self<jo::ScriptBlock>(this)->vtable = img::obj(d::vt_Node_9da980);
        joan::orig::as_script(this)->clear();                  // 0x0063e7f0
        return joan::orig::arg::as<u32>(joan::orig::as_ctrl(this)->dtor_Ctrl_63e600());  // 0x0063e600
    }

    // ------------------------------------------------------------------------ 0x00641c30
    u32 size_641c30() {
        u32 v = joan::orig::arg::as<u32>(joan::orig::as_script_block(this)->byte_size()) + 0x50;  // 0x00641a60
        if (jo::view<void*>(jo::self<jo::Script>(this)->strings) != nullptr) v += joan::orig::arg::as<u32>(joan::orig::as_dlg_res(jo::view<void*>(jo::self<jo::Script>(this)->strings))->byte_size());  // 0x00638d50
        return v;
    }

    // ------------------------------------------------------------------------ node ctors
    void* node_6427d0() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::ScriptSpeaker>(this)->vtable = img::obj(d::vt_Obj_9dade8);
        jo::view<u32>(jo::self<jo::ScriptSpeaker>(this)->kind) = 5;
        return this;
    }
    void* node_642ae0() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::ScriptGoto>(this)->vtable = img::obj(d::vt_Node_9daed8);
        jo::view<u32>(jo::self<jo::ScriptGoto>(this)->kind) = 9;
        return this;
    }
    void* node_6430c0() {
        joan::orig::as_ctrl(this)->Ctrl_643000();  // 0x00643000
        jo::self<jo::ScriptCheckCall>(this)->vtable = img::obj(d::vt_Node_9db084);
        jo::view<u32>(jo::self<jo::ScriptCheckCall>(this)->kind) = 0x16;
        return this;
    }
    void* node_6447a0() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::ScriptExpr>(this)->vtable = img::obj(d::vt_Obj_9db3bc);
        jo::view<u32>(jo::self<jo::ScriptExpr>(this)->kind) = 0x10;
        jo::self<jo::ScriptExpr>(this)->lhs = nullptr;
        jo::self<jo::ScriptExpr>(this)->rhs = nullptr;
        return this;
    }

    // ------------------------------------------------------------------------ 0x00643450
    u32 dtor_643450() {
        jo::self<jo::ScriptInt>(this)->vtable = img::obj(d::vt_Obj_9db18c);
        return joan::orig::arg::as<u32>(joan::orig::as_ctrl(this)->dtor_Ctrl_63e600());  // 0x0063e600
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Obj63e::set_4", 0x0063e630, &H::set_4);
B95("Obj63e7::~Obj63e7", 0x0063e780, &H::dtor_63e780);
B95("Obj641::size", 0x00641c30, &H::size_641c30);
B95("Node6427::ctor", 0x006427d0, &H::node_6427d0);
B95("Node642a::ctor", 0x00642ae0, &H::node_642ae0);
B95("Node6430::ctor", 0x006430c0, &H::node_6430c0);
B95("Node643::~Node643", 0x00643450, &H::dtor_643450);
B95("Node6447::ctor", 0x006447a0, &H::node_6447a0);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b91_04_2.cpp (game/b91_04)
namespace {
namespace game_b91_04_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

// Virtual thiscall: vtable slot at byte offset `off`.
template <class R = void, class... A>
R vcall(void* o, std::int32_t off, A... args) {
    return thiscall<R>(vtable_slot(o, off), o, args...);
}

using joan::orig::helpers::game_delete;  // operator delete
using joan::orig::helpers::memset_;

std::int32_t list_count(void* l) { return joan::orig::as_dynarray(l)->size(); }  // 0x00417ad0
void* list_item(void* l, std::int32_t i) { return joan::orig::as_dynarray(l)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0

JOAN_COUNTER(c_size, "Obj641::size");

struct H {
    void* self() { return this; }

    // 0x0063e660
    void free_10() {
        const std::int32_t type = jo::self<jo::ScriptNode>(this)->value_type;
        if (type == 1) {
            jo::self<jo::ScriptNode>(this)->value = 0;
        } else if (type == 2) {
            jo::self<jo::ScriptNode>(this)->value = 0;
        } else {
            game_delete(jo::view<void*>(jo::self<jo::ScriptNode>(this)->value));
            jo::self<jo::ScriptNode>(this)->value = 0;
        }
    }
    // 0x0063e6e0
    void* ctor_63e6e0() {
        joan::orig::as_reader(self())->construct();  // 0x0063e550
        jo::self<jo::ScriptBlock>(this)->vtable = img::obj(d::vt_Node_9da980);
        memset_(&jo::self<jo::ScriptBlock>(this)->statements, 0, 3 * sizeof(jo::DynArray*));  // statements, vars, labels
        return self();
    }
    // 0x0063ef20
    void* item_63ef20(std::int32_t i) { return list_item(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->vars), i); }
    // 0x0063f100
    std::int32_t count_63f100() {
        if (jo::view<void*>(jo::self<jo::ScriptBlock>(this)->vars) == nullptr) return 0;
        return list_count(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->vars));
    }
    // 0x00641a60
    std::int32_t size_641a60() {
        c_size.hit();
        std::int32_t total = joan::orig::arg::as<std::int32_t>(joan::orig::as_script_node(self())->byte_size()) + 0xc;  // 0x00641a40
        jo::DynArray* const* const lists = &jo::self<jo::ScriptBlock>(this)->statements;  // statements, vars, labels
        for (std::int32_t k = 0; k < 3; ++k) {
            if (lists[k] == nullptr) continue;
            total += static_cast<std::int32_t>(jo::view<std::uint32_t>(lists[k]->ele_num) * jo::view<std::uint32_t>(lists[k]->ele_size));
            for (std::int32_t i = 1; i <= list_count(lists[k]); ++i) {
                void* const e = list_item(lists[k], i);
                total += vcall<std::int32_t>(e, 0x2c);
            }
        }
        return total;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj63e::free_10", 0x0063e660, &H::free_10);
B91("Obj63e::Obj63e", 0x0063e6e0, &H::ctor_63e6e0);
B91("Obj63e::item", 0x0063ef20, &H::item_63ef20);
B91("Obj63e::count", 0x0063f100, &H::count_63f100);
B91("Obj641::size", 0x00641a60, &H::size_641a60);
#undef B91

}  // namespace game_b91_04_2
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using joan::call::thiscall;

JOAN_COUNTER(c_script_clear, "Script::clear");
JOAN_COUNTER(c_add_node, "Script::add_node");

struct H {
    // 0x0063e7f0 Script::clear
    std::int32_t script_clear() {
        c_script_clear.hit();
        if (jo::view<void*>(jo::self<jo::ScriptBlock>(this)->labels) != nullptr) {
            for (std::int32_t n = joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->labels))->size(); n > 0; --n) {  // 0x00417ad0
                void* const p = joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->labels))->ptr_at(joan::orig::arg::i32(n));  // 0x0041b3f0
                if (p != nullptr) virtual_call<void*>(p, 0, 1);                   // scalar deleting dtor
                joan::orig::dynarray(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->labels))->linkout(joan::orig::arg::i32(n));   // 0x00584a10
            }
            if (void* const q = jo::view<void*>(jo::self<jo::ScriptBlock>(this)->labels)) joan::orig::as_dynarray(q)->destroy(1u);  // 0x00463fc0
            jo::view<void*>(jo::self<jo::ScriptBlock>(this)->labels) = nullptr;
        }
        for (std::int32_t n = joan::orig::arg::as<std::int32_t>(joan::orig::as_reader(this)->count()); n > 0; --n) joan::orig::as_script_block(this)->delete_child(joan::orig::arg::i32(n));  // 0x0063ecd0 / 0x0063eb00
        if (jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements) != nullptr) {
            if (void* const q = jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements)) joan::orig::as_dynarray(q)->destroy(1u);
            jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements) = nullptr;
        }
        for (std::int32_t n = joan::orig::arg::as<std::int32_t>(joan::orig::as_script_block(this)->var_count()); n > 0; --n) joan::orig::as_script_block(this)->delete_var(joan::orig::arg::i32(n));  // 0x0063f100 / 0x0063f080
        if (jo::view<void*>(jo::self<jo::ScriptBlock>(this)->vars) != nullptr) {
            if (void* const q = jo::view<void*>(jo::self<jo::ScriptBlock>(this)->vars)) joan::orig::as_dynarray(q)->destroy(1u);
            jo::view<void*>(jo::self<jo::ScriptBlock>(this)->vars) = nullptr;
        }
        return thiscall<std::int32_t>(fn::Obj63e_free_10, this);  // 0x0063e660
    }

    // 0x0063ef50 Script::add_node
    std::int32_t add_node() {
        c_add_node.hit();
        if (jo::view<void*>(jo::self<jo::ScriptBlock>(this)->vars) == nullptr) {
            void* const m = joan::crt::operator_new(static_cast<u32>(sizeof(jo::DynArray)));  // operator new (0x1d on x86)
            void* const r = m != nullptr ? joan::orig::dynarray(m)->construct(static_cast<i32>(sizeof(void*)), 1, 0) : nullptr;  // 0x005843d0 (node pointers: 4 on x86)
            jo::view<void*>(jo::self<jo::ScriptBlock>(this)->vars) = r;
        }
        void* const m2 = joan::orig::arg::as<void*>(joan::orig::pool::alloc(static_cast<std::uint32_t>(sizeof(jo::ScriptVarDecl))));  // 0x0063e450 (0x48 on x86)
        void* node = m2 != nullptr ? joan::orig::as_script_var_decl(m2)->construct() : nullptr;  // 0x00642f20
        virtual_call(node, 4, static_cast<void*>(this));
        joan::orig::dynarray(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->vars))->linkin(static_cast<void*>(&node));  // 0x00584620
        return joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->vars))->cursor();  // 0x00417520
    }

    // 0x00642540
    std::uint32_t get21() { return jo::view<std::uint32_t>(jo::self<jo::ScriptId>(this)->label_id); }

    // 0x00642950
    void* node_ctor_642950() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::view<std::uintptr_t>(jo::self<jo::ScriptObject>(this)->vtable) = img::addr(d::vt_Obj_9dae60);
        jo::view<std::uint32_t>(jo::self<jo::ScriptObject>(this)->kind) = 7;
        return this;
    }

    // 0x00642fd0
    void node_dtor_642fd0() {
        jo::view<std::uintptr_t>(jo::self<jo::ScriptVarDecl>(this)->vtable) = img::addr(d::vt_Obj_9db004);
        joan::orig::as_ctrl(this)->dtor_Ctrl_63e600();  // 0x0063e600
    }

    // 0x00643e30
    void* node_ctor_643e30() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::view<std::uintptr_t>(jo::self<jo::Value>(this)->vtable) = img::addr(d::vt_Node_9db25c);
        jo::view<std::uint32_t>(jo::self<jo::Value>(this)->kind) = 0x14;
        jo::self<jo::Value>(this)->ref = nullptr;
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Script::clear", 0x0063e7f0, &H::script_clear);
B94("Script::add_node", 0x0063ef50, &H::add_node);
B94("Obj::get21", 0x00642540, &H::get21);
B94("Node::ctor_642950", 0x00642950, &H::node_ctor_642950);
B94("Node::dtor_642fd0", 0x00642fd0, &H::node_dtor_642fd0);
B94("Node::ctor_643e30", 0x00643e30, &H::node_ctor_643e30);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b107_14.cpp (game/b107_14)
namespace {
namespace game_b107_14 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

struct H {
    // 0x0063ea20
    u32 push_value(u32 arg) {
        if (jo::self<jo::ScriptBlock>(this)->statements == nullptr) {
            void* const mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::DynArray)));   // operator new (0x1d on x86)
            void* arr = nullptr;
            if (mem != nullptr)
                arr = joan::orig::dynarray(mem)->construct(static_cast<std::int32_t>(sizeof(void*)), 1, 0);    // 0x005843d0 (node pointers: 4 on x86)
            jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements) = arr;
        }
        jo::ScriptNode* value = joan::orig::as_script_node(this)->create_child(joan::orig::arg::i32(arg));             // 0x00640db0
        joan::orig::dynarray(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements))->linkin(&value);         // 0x00584620
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements))->cursor());  // 0x00417520
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_14", name, addr, fn)
B("Obj63ea20::push_value", 0x0063ea20, &H::push_value);
#undef B

}  // namespace game_b107_14
}  // namespace

// ---------------------------------------- from modules/game_b93_04.cpp (game/b93_04)
namespace {
namespace game_b93_04 {

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

struct Obj {};

using joan::call::thiscall;
// Virtual call through vtable slot `index` (thiscall).
template <class R = void, class... A>
R vcall(void* o, std::int32_t index, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(o)[index], sizeof(void*));
    return (reinterpret_cast<Obj*>(o)->*method)(args...);
}

JOAN_COUNTER(c_remove_a, "Owner::remove_a");
JOAN_COUNTER(c_remove_b, "Owner::remove_b");

struct H {
    // ------------------------------------------------------------------ 0x0063eb00
    i32 remove_a(i32 x) {
        c_remove_a.hit();
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_reader(this)->list_get(joan::orig::arg::i32(x)));  // 0x0063e9f0
        if (p != nullptr) vcall<i32>(p, 0, 1);                 // scalar deleting destructor
        joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements))->linkout_584a10(x);   // 0x00584a10
        return 0;  // void callee; every caller ignores the result
    }

    // ------------------------------------------------------------------ 0x0063f080
    i32 remove_b(i32 x) {
        c_remove_b.hit();
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_script_block(this)->var_at(joan::orig::arg::i32(x)));  // 0x0063ef20
        if (p != nullptr) vcall<i32>(p, 0, 1);                 // scalar deleting destructor
        joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->vars))->linkout_584a10(x);   // 0x00584a10
        return 0;  // void callee; every caller ignores the result
    }

    // ------------------------------------------------------------------ 0x00642440
    i32 at_end() { return jo::self<jo::Range>(this)->current >= jo::self<jo::Range>(this)->end ? 1 : 0; }

    // ------------------------------------------------------------------ 0x006424f0
    void* node2_ctor() {
        joan::orig::as_script_block(this)->construct();  // 0x0063e6e0
        jo::self<jo::ScriptId>(this)->vtable = img::obj(d::vt_Node_9dad34);
        jo::self<jo::ScriptId>(this)->kind = 2;
        jo::self<jo::ScriptId>(this)->active = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x00642f20
    void* node11_ctor() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::ScriptVarDecl>(this)->vtable = img::obj(d::vt_Obj_9db004);
        jo::self<jo::ScriptVarDecl>(this)->kind = 0x11;
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("Owner::remove_a", 0x0063eb00, &H::remove_a);
B93("Owner::remove_b", 0x0063f080, &H::remove_b);
B93("Range::at_end", 0x00642440, &H::at_end);
B93("Node2::Node2", 0x006424f0, &H::node2_ctor);
B93("Node11::Node11", 0x00642f20, &H::node11_ctor);
#undef B93

}  // namespace game_b93_04
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

using joan::call::thiscall;

struct H {
    // 0x0063ed40
    std::uintptr_t sub_reset() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_counter(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements))->clamp_to_one()); }  // 0x004b9d40
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Obj::sub_reset", 0x0063ed40, &H::sub_reset);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b98_06_2.cpp (game/b98_06)
namespace {
namespace game_b98_06_2 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using joan::call::thiscall;

JOAN_COUNTER(c_dtor_644480, "Obj::dtor_644480");

struct H {
    // ------------------------------------------------------------------ 0x0063eda0
    u32 call_585660(u32 a) { joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements))->go(joan::orig::arg::i32(a)); return 0; }  // 0x00585660 (void; callers ignore the result)

    // ------------------------------------------------------------------ 0x00642700
    H* ctor_642700() {
        joan::orig::as_ctrl(this)->Ctrl_63f190();  // 0x0063f190
        jo::self<jo::ScriptReset>(this)->vtable = img::obj(d::vt_Obj_9dadac);
        jo::view<u32>(jo::self<jo::ScriptReset>(this)->kind) = 4;
        jo::self<jo::ScriptReset>(this)->done = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x00642b70
    u32 dtor_642b70() {
        jo::self<jo::ScriptGoto>(this)->vtable = img::obj(d::vt_Node_9daed8);
        return joan::orig::arg::as<u32>(joan::orig::as_ctrl(this)->dtor_Ctrl_63e600());  // 0x0063e600
    }

    // ------------------------------------------------------------------ 0x00642cf0
    u32 dtor_642cf0() {
        jo::self<jo::ScriptElse>(this)->vtable = img::obj(d::vt_Node_9daf50);
        return joan::orig::arg::as<u32>(joan::orig::as_script_block(this)->destruct());  // 0x0063e780
    }

    // ------------------------------------------------------------------ 0x00643210
    u32 dtor_643210() {
        jo::self<jo::ScriptCameraCall>(this)->vtable = img::obj(d::vt_Obj_9db0c8);
        joan::orig::as_script_func_call(this)->destruct();  // 0x00643090
        return 0;  // void callee; destructor, result unused
    }

    // ------------------------------------------------------------------ 0x00643480
    H* ctor_643480() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::ScriptLoad>(this)->vtable = img::obj(d::vt_Node_9db1c8);
        jo::view<u32>(jo::self<jo::ScriptLoad>(this)->kind) = 0x1a;
        jo::self<jo::ScriptLoad>(this)->script = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x00644480
    u32 dtor_644480() {
        c_dtor_644480.hit();
        jo::self<jo::ScriptAssign>(this)->vtable = img::obj(d::vt_Obj_9db318);
        if (jo::view<void*>(jo::self<jo::ScriptAssign>(this)->expr) != nullptr) virtual_call<u32>(jo::view<void*>(jo::self<jo::ScriptAssign>(this)->expr), 0, 1u);  // scalar deleting dtor
        return joan::orig::arg::as<u32>(joan::orig::as_ctrl(this)->dtor_Ctrl_63e600());  // 0x0063e600
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Obj::call_585660", 0x0063eda0, &H::call_585660);
B("Obj::ctor_642700", 0x00642700, &H::ctor_642700);
B("Obj::dtor_642b70", 0x00642b70, &H::dtor_642b70);
B("Obj::dtor_642cf0", 0x00642cf0, &H::dtor_642cf0);
B("Obj::dtor_643210", 0x00643210, &H::dtor_643210);
B("Obj::ctor_643480", 0x00643480, &H::ctor_643480);
B("Obj::dtor_644480", 0x00644480, &H::dtor_644480);
#undef B

}  // namespace game_b98_06_2
}  // namespace

// ---------------------------------------- from modules/game_b105_18.cpp (game/b105_18)
namespace {
namespace game_b105_18 {

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using joan::call::thiscall;

JOAN_COUNTER(c_node_find, "Node::find");

struct H {
    i32 true_4(u32) { return 1; }                                  // 0x00640040 ... 0x006439b0
    i32 minus1_4(u32) { return -1; }                               // 0x006409d0
    u32 get_c() { return jo::view<u32>(jo::self<jo::ScriptNode>(this)->value_type); }                            // 0x00642470
    i32 zero() { return 0; }                                       // 0x00642490 / 0x006424b0
    H* self() { return this; }                                     // 0x006424d0 / 0x00642590

    // ------------------------------------------------------------------------ 0x0063ee70
    std::uintptr_t node_find(const char* name) {  // the child node (pointer), or 0
        c_node_find.hit();
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_script_block(this)->var_count()); i > 0; --i) {  // 0x0063f100
            void* const child = joan::orig::arg::as<void*>(joan::orig::as_script_block(this)->var_at(joan::orig::arg::i32(i)));  // 0x0063ef20
            const char* const child_name = joan::orig::arg::as<const char*>(joan::orig::as_script_var_decl(child)->name());  // 0x00642f60
            if (joan::crt::stricmp(joan::orig::arg::str(child_name), joan::orig::arg::str(name)) == 0) {  // _stricmp
                return reinterpret_cast<std::uintptr_t>(child);
            }
        }
        if (jo::view<void*>(jo::self<jo::ScriptBlock>(this)->parent) != nullptr) return virtual_call<std::uintptr_t>(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->parent), 0x18, name);
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_18", name, addr, fn)
B("Node::find", 0x0063ee70, &H::node_find);
B("Obj::true_640040", 0x00640040, &H::true_4);
B("Obj::true_640150", 0x00640150, &H::true_4);
B("Obj::true_640260", 0x00640260, &H::true_4);
B("Obj::minus1_6409d0", 0x006409d0, &H::minus1_4);
B("Obj::true_640bb0", 0x00640bb0, &H::true_4);
B("Obj::get_c_642470", 0x00642470, &H::get_c);
B("Obj::zero_642490", 0x00642490, &H::zero);
B("Obj::zero_6424b0", 0x006424b0, &H::zero);
B("Obj::self_6424d0", 0x006424d0, &H::self);
B("Obj::self_642590", 0x00642590, &H::self);
B("Obj::true_6437b0", 0x006437b0, &H::true_4);
B("Obj::true_643800", 0x00643800, &H::true_4);
B("Obj::true_6439b0", 0x006439b0, &H::true_4);
#undef B

}  // namespace game_b105_18
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x0063f200
    void* scalar_dtor_63f200(u32 flags) {
        joan::orig::as_script(this)->dtor_63f250();  // 0x0063f250
        if ((flags & 1u) != 0) joan::orig::pool::free_(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x0063e4f0
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("ScriptObj::scalar_dtor_63f200", 0x0063f200, &H::scalar_dtor_63f200);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x0063f250
    u32 dtor_63f250() {
        jo::self<jo::Script>(this)->vtable = img::obj(d::vt_Obj_9da9c8);
        joan::orig::as_gadget(this)->destroy_tree();         // 0x0063f2c0
        return joan::orig::arg::as<u32>(joan::orig::as_script_block(this)->destruct());  // 0x0063e780
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Script::dtor_63f250", 0x0063f250, &H::dtor_63f250);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::hook::global;

using joan::call::thiscall;

JOAN_COUNTER(c_destroy_tree, "Gadget::destroy_tree_63f2c0");

struct H {
    // ------------------------------------------------------------------ 0x0063f2c0
    std::uintptr_t destroy_tree() {  // vtable slot 2 (0x9da9c8 / 0x9dad70 / 0x9dadac): eax as in the original
        c_destroy_tree.hit();
        void* saved_a = global<void*>(d::g_parser_arena);  // see header note
        void* saved_b = global<void*>(d::g_parser_arena_counter);
        if (jo::self<jo::Script>(this)->string_pool != nullptr) {
            saved_a = global<void*>(d::g_parser_arena);
            saved_b = global<void*>(d::g_parser_arena_counter);
            global<void*>(d::g_parser_arena) = jo::view<void*>(jo::self<jo::Script>(this)->string_pool);
            global<void*>(d::g_parser_arena_counter) = &jo::self<jo::Script>(this)->pool_state;  // this + 0x6c
        }
        for (i32 n = joan::orig::arg::as<i32>(joan::orig::as_reader(this)->count()); n > 0; --n) joan::orig::as_script_block(this)->delete_child(joan::orig::arg::i32(n));  // 0x0063ecd0 / 0x0063eb00
        if (jo::view<void*>(jo::self<jo::Script>(this)->statements) != nullptr) {
            joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Script>(this)->statements))->destroy(1u);  // 0x00463fc0
            jo::self<jo::Script>(this)->statements = nullptr;
        }
        for (i32 n = joan::orig::arg::as<i32>(joan::orig::as_script_block(this)->var_count()); n > 0; --n) joan::orig::as_script_block(this)->delete_var(joan::orig::arg::i32(n));  // 0x0063f100 / 0x0063f080
        if (jo::view<void*>(jo::self<jo::Script>(this)->vars) != nullptr) {
            joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Script>(this)->vars))->destroy(1u);  // 0x00463fc0
            jo::self<jo::Script>(this)->vars = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Script>(this)->strings) != nullptr) {
            joan::orig::as_widget(jo::view<void*>(jo::self<jo::Script>(this)->strings))->scalar_dtor_63f4e0(1u);  // 0x0063f4e0
            jo::self<jo::Script>(this)->strings = nullptr;
        }
        const u32 eax = joan::orig::arg::as<u32>(joan::orig::as_script(this)->clear());  // 0x0063e7f0
        if (jo::view<void*>(jo::self<jo::Script>(this)->string_pool) == nullptr) return eax;
        joan::orig::as_string_pool(jo::view<void*>(jo::self<jo::Script>(this)->string_pool))->destroy(1u);  // 0x0063f530
        jo::self<jo::Script>(this)->string_pool = nullptr;
        global<void*>(d::g_parser_arena) = saved_a;
        global<void*>(d::g_parser_arena_counter) = saved_b;
        return reinterpret_cast<std::uintptr_t>(saved_a);  // eax: the old pool pointer
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("Gadget::destroy_tree", 0x0063f2c0, &H::destroy_tree);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b100_12.cpp (game/b100_12)
namespace {
namespace game_b100_12_2 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using joan::orig::helpers::op_delete;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0063f4e0
    H* scalar_dtor_63f4e0(u32 flags) {
        joan::orig::as_dlg_res(this)->destruct();  // 0x00643520
        if ((flags & 1u) != 0) op_delete(this);
        return this;
    }

    // ------------------------------------------------------------------ widget ctors
    H* ctor_640a50() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::ScriptChoice>(this)->vtable = img::obj(d::vt_Obj_9dac7c);
        jo::view<u32>(jo::self<jo::ScriptChoice>(this)->kind) = 0xb;
        jo::self<jo::ScriptChoice>(this)->options = nullptr;
        return this;
    }
    H* ctor_642630() {
        joan::orig::as_ctrl(this)->Ctrl_63f190();  // 0x0063f190
        jo::self<jo::ScriptInit>(this)->vtable = img::obj(d::vt_ScriptNode_9dad70);
        jo::view<u32>(jo::self<jo::ScriptInit>(this)->kind) = 0x3;
        jo::self<jo::ScriptInit>(this)->done = 0;
        return this;
    }
    u32 dtor_6429e0() {
        jo::self<jo::ScriptObject>(this)->vtable = img::obj(d::vt_Obj_9dae60);
        return joan::orig::arg::as<u32>(joan::orig::as_ctrl(this)->dtor_Ctrl_63e600());  // 0x0063e600
    }
    H* ctor_642ba0() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::ScriptExit>(this)->vtable = img::obj(d::vt_Obj_9daf14);
        jo::view<u32>(jo::self<jo::ScriptExit>(this)->kind) = 0xa;
        return this;
    }
    H* ctor_642e20() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::Expr>(this)->vtable = img::obj(d::vt_Obj_9dafc8);
        jo::view<u32>(jo::self<jo::Expr>(this)->kind) = 0xf;
        return this;
    }
    H* ctor_643180() {
        joan::orig::as_ctrl(this)->Ctrl_643000();  // 0x00643000
        jo::self<jo::ScriptCameraCall>(this)->vtable = img::obj(d::vt_Obj_9db0c8);
        jo::view<u32>(jo::self<jo::ScriptCameraCall>(this)->kind) = 0x17;
        return this;
    }
    H* ctor_643240() {
        joan::orig::as_ctrl(this)->Ctrl_643000();  // 0x00643000
        jo::self<jo::ScriptCall>(this)->vtable = img::obj(d::vt_Node_9db10c);
        jo::view<u32>(jo::self<jo::ScriptCall>(this)->kind) = 0x18;
        return this;
    }
    H* ctor_6433c0() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::ScriptInt>(this)->vtable = img::obj(d::vt_Obj_9db18c);
        jo::view<u32>(jo::self<jo::ScriptInt>(this)->kind) = 0x19;
        return this;
    }
    H* ctor_6443d0() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::ScriptAssign>(this)->vtable = img::obj(d::vt_Obj_9db318);
        jo::view<u32>(jo::self<jo::ScriptAssign>(this)->kind) = 0x12;
        jo::self<jo::ScriptAssign>(this)->target = nullptr;
        jo::self<jo::ScriptAssign>(this)->expr = nullptr;
        return this;
    }

    // ------------------------------------------------------------------ 0x006452f0
    u32 dtor_body_6452f0() {
        if (void* const a = jo::view<void*>(jo::self<jo::ScriptCond>(this)->rhs)) virtual_call<u32>(a, 0, 1u);
        jo::self<jo::ScriptCond>(this)->rhs = nullptr;
        if (void* const b = jo::view<void*>(jo::self<jo::ScriptCond>(this)->lhs)) virtual_call<u32>(b, 0, 1u);
        jo::self<jo::ScriptCond>(this)->lhs = nullptr;
        joan::orig::as_script_node(this)->free_value();  // 0x0063e660
        return 0;  // void callee; destructor body, result unused
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_12", name, addr, fn)
B("Widget::scalar_dtor_63f4e0", 0x0063f4e0, &H::scalar_dtor_63f4e0);
B("Widget::ctor_640a50", 0x00640a50, &H::ctor_640a50);
B("Widget::ctor_642630", 0x00642630, &H::ctor_642630);
B("Widget::dtor_6429e0", 0x006429e0, &H::dtor_6429e0);
B("Widget::ctor_642ba0", 0x00642ba0, &H::ctor_642ba0);
B("Widget::ctor_642e20", 0x00642e20, &H::ctor_642e20);
B("Widget::ctor_643180", 0x00643180, &H::ctor_643180);
B("Widget::ctor_643240", 0x00643240, &H::ctor_643240);
B("Widget::ctor_6433c0", 0x006433c0, &H::ctor_6433c0);
B("Widget::ctor_6443d0", 0x006443d0, &H::ctor_6443d0);
B("Widget::dtor_body_6452f0", 0x006452f0, &H::dtor_body_6452f0);
#undef B

}  // namespace game_b100_12_2
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

struct H {
    // 0x0063f530
    void* scalar_deleting_dtor(u32 flags) {
        joan::orig::as_string_pool(this)->destruct();  // 0x005a1770
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("Obj63f530::scalar_deleting_dtor", 0x0063f530, &H::scalar_deleting_dtor);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b102_13.cpp (game/b102_13)
namespace {
namespace game_b102_13 {

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::img::obj;
using joan::img::str;

struct Obj {};

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

using joan::call::thiscall;
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

JOAN_COUNTER(c_run_seq, "ScriptNode::run_seq");
JOAN_COUNTER(c_run_once, "ScriptNode::run_once");
JOAN_COUNTER(c_run_until, "ScriptNode::run_until");
JOAN_COUNTER(c_find_param, "ScriptNode::find_param");
JOAN_COUNTER(c_run_params, "ScriptNode::run_params");
JOAN_COUNTER(c_assign, "ScriptNode::assign");
JOAN_COUNTER(c_logic_op, "ScriptNode::logic_op");
JOAN_COUNTER(c_call_func, "ScriptNode::call_func");

struct H {
    u8* param(i32 n) { return joan::orig::arg::as<u8*>(joan::orig::as_reader(this)->list_get(joan::orig::arg::i32(n))); }  // 0x0063e9f0
    i32 nparams() { return joan::orig::arg::as<i32>(joan::orig::as_reader(this)->count()); }          // 0x0063ecd0
    u8* cur() { return joan::orig::arg::as<u8*>(joan::orig::as_table(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements))->last()); }    // 0x00585f20
    i32 next() { return joan::orig::arg::as<i32>(joan::orig::as_reader(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements))->next()); }   // 0x0063ee20
    void end() { joan::orig::as_counter(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->statements))->clamp_to_one(); }                // 0x004b9d40

    // ------------------------------------------------------------------ 0x0063f790
    i32 run_seq(std::uintptr_t arg) {  // arg: the evaluation context (a pointer or a small integer; whole register on x64)
        c_run_seq.hit();
        if (nparams() == 0) return 1;
        for (;;) {
            u8* const it = cur();
            const i32 r = vcall<i32>(it, 0xc, arg);
            switch (r) {
                case -1: end(); return -1;
                case 0: return 0;
                case 1:
                    if (next() == 0) {
                        end();
                        return 1;
                    }
                    break;
                case 2: return 2;
                default: break;
            }
        }
    }

    // ------------------------------------------------------------------ 0x0063fb80 / 0x0063fde0
    i32 run_once(std::uintptr_t arg) {
        c_run_once.hit();
        if (nparams() == 0 || jo::self<jo::ScriptInit>(this)->done != 0) return 1;
        for (;;) {
            u8* const it = cur();
            const i32 r = vcall<i32>(it, 0xc, arg);
            switch (r) {
                case -1: end(); return -1;
                case 0: return 0;
                case 1:
                    if (next() == 0) {
                        end();
                        jo::self<jo::ScriptInit>(this)->done = 1;
                        return 1;
                    }
                    break;
                case 2: return 2;
                default: break;
            }
        }
    }

    // ------------------------------------------------------------------ 0x00640370
    i32 run_until(std::uintptr_t arg) {
        c_run_until.hit();
        if (nparams() == 0) return 1;
        if (jo::self<jo::ScriptWait>(this)->waiting == 0) {
            do {
                vcall<i32>(cur(), 0xc, arg);
            } while (next() != 0);
            jo::self<jo::ScriptWait>(this)->waiting = 1;
            end();
        }
        do {
            u8* const it = cur();
            if (jo::as_script_node(it)->kind == 0x14) {
                if ((joan::orig::arg::as<u32>(joan::orig::as_script_value(it)->forward_34(arg)) & 0xffu) == 0) {  // 0x00644390 (arg: the evaluator context pointer, passed whole)
                    end();
                    return 0;
                }
            } else if (jo::as_script_node(it)->kind == 0x12) {
                // kind 0x12 = ScriptAssign (ctor 0x006443d0); its expr is always a ScriptExpr (create_child(0x10), 0x00644680)
                void* const t = jo::as_script_expr(jo::as_script_assign(it)->expr)->rhs;
                if ((joan::orig::arg::as<u32>(joan::orig::as_script_value(t)->forward_34(arg)) & 0xffu) == 0) {  // 0x00644390
                    end();
                    return 0;
                }
            }
        } while (next() != 0);
        end();
        jo::self<jo::ScriptWait>(this)->waiting = 0;
        return 1;
    }

    // ------------------------------------------------------------------ 0x00641f20 / 0x00641fb0 / 0x00642070 / 0x00642100
    template <class C>
    i32 size_sum(i32 base, jo::ScriptNode* C::*o1, jo::ScriptNode* C::*o2) {
        i32 s = joan::orig::arg::as<i32>(joan::orig::as_script_node(this)->byte_size()) + base;  // 0x00641a40
        if (jo::self<C>(this)->*o1 != nullptr) s += vcall<i32>(jo::self<C>(this)->*o1, 0x2c);
        if (jo::self<C>(this)->*o2 != nullptr) s += vcall<i32>(jo::self<C>(this)->*o2, 0x2c);
        return s;
    }
    // (op, lhs +0x18, rhs +0x1c) and (lhs +0x14, rhs +0x18) nodes
    i32 size_641f20() { return size_sum(0xc, &jo::Expr::lhs, &jo::Expr::rhs); }
    i32 size_641fb0() { return size_sum(0xc, &jo::Expr::lhs, &jo::Expr::rhs); }
    i32 size_642070() { return size_sum(0x8, &jo::ScriptExpr::lhs, &jo::ScriptExpr::rhs); }
    i32 size_642100() { return size_sum(0x8, &jo::ScriptExpr::lhs, &jo::ScriptExpr::rhs); }

    // ------------------------------------------------------------------ 0x00643700
    std::uintptr_t find_param(u32 key) {  // the parameter node (pointer) with id `key`, or 0
        c_find_param.hit();
        for (i32 i = nparams(); i > 0; --i) {
            u8* const p = param(i);
            if (jo::as_script_node(p)->kind == 2) {
                if (joan::orig::arg::as<u32>(joan::orig::as_script_id(p)->get_21()) == key) return reinterpret_cast<std::uintptr_t>(p);  // 0x00642540
            }
        }
        if (jo::view<void*>(jo::self<jo::ScriptBlock>(this)->parent) == nullptr) return 0;
        return vcall<std::uintptr_t>(jo::view<void*>(jo::self<jo::ScriptBlock>(this)->parent), 0x14, key);
    }

    // ------------------------------------------------------------------ 0x00643b60
    u32 run_params(std::uintptr_t arg) {
        c_run_params.hit();
        for (i32 i = nparams(); i > 0; --i) {
            param(i);  // result unused in the original
            u8* const q = param(i);
            vcall<i32>(q, 0xc, arg);
        }
        vcall<u32>(this, 0x30, arg);
        return 1;
    }

    // ------------------------------------------------------------------ 0x00644530
    i32 assign(std::uintptr_t arg) {
        c_assign.hit();
        const i32 r = vcall<i32>(jo::view<void*>(jo::self<jo::ScriptAssign>(this)->expr), 0xc, arg);
        if (r == 0) return 1;
        if (r == -1) return -1;
        switch (jo::view<u32>(jo::self<jo::ScriptAssign>(this)->expr->value_type)) {
            case 1:
            case 2: jo::self<jo::ScriptAssign>(this)->target->value = jo::self<jo::ScriptAssign>(this)->expr->value; break;
            case 3:
                joan::orig::as_string(jo::view<void*>(jo::self<jo::ScriptAssign>(this)->target->value))->assign(joan::orig::arg::str(jo::self<jo::ScriptAssign>(this)->expr->value));  // 0x005a1440
                break;
            case 0x1b: {
                const u32* const s = jo::view<const u32*>(jo::self<jo::ScriptAssign>(this)->expr->value);
                u32* const d = jo::view<u32*>(jo::self<jo::ScriptAssign>(this)->target->value);
                d[0] = s[0];
                d[1] = s[1];
                d[2] = s[2];
                break;
            }
            default: break;
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x006453a0
    u32 logic_op(std::uintptr_t arg) {
        c_logic_op.hit();
        // Unexpected op: the original writes an uninitialised local to +0x10; run the original
        // body (thunk 0x00402fef is retargeted, the body at 0x006453a0 is untouched).
        // Batch 118/03: with fix/uninit enabled (module_enabled, read on the first call) the C++
        // version runs instead and stores 0 (the never-written result [ebp-0x18]).
        const i32 op = jo::self<jo::ScriptCond>(this)->op;
        if (op != -0x27 && op != -0x26 && op != 0x22 && !fix_uninit()) {
            using Fn = u32 (H::*)(std::uintptr_t);
            return (this->*joan::hook::original<Fn>(fn::ScriptNode_logic_op_body))(arg);
        }
        u8* const l = jo::view<u8*>(jo::self<jo::ScriptCond>(this)->lhs);
        vcall<i32>(l, 0xc, arg);
        u8* const r = jo::view<u8*>(jo::self<jo::ScriptCond>(this)->rhs);
        if (r != nullptr) vcall<i32>(r, 0xc, arg);
        const u32 a = inline_value<u32>(jo::as_script_node(l)->value);
        const u32 b = r != nullptr ? inline_value<u32>(jo::as_script_node(r)->value) : 0;
        u32 res = 0;  // [ebp-0x18]: never written for other ops (fix/uninit: 0)
        switch (jo::self<jo::ScriptCond>(this)->op) {  // re-read after the child calls, as the original
            case -0x27: res = (a != 0 && b != 0) ? 1 : 0; break;
            case -0x26: res = (a == 0 && b == 0) ? 0 : 1; break;
            case 0x22: res = a; break;
            default: break;  // fix/uninit only (entry check) - or a child call changed +0x14 (never)
        }
        jo::view<u32>(jo::self<jo::ScriptCond>(this)->value_type) = 1;
        inline_value<u32>(jo::self<jo::ScriptCond>(this)->value) = res;
        return 1;
    }

    // ------------------------------------------------------------------ 0x00645d40
    u32 call_func(std::uintptr_t arg) {
        c_call_func.hit();
        void* const o = joan::orig::arg::as<void*>(joan::orig::as_script_cache(obj(d::g_scripts))->compiled_at(joan::orig::arg::u32(jo::self<jo::ScriptLoad>(this)->script)));  // 0x00648ce0
        const i32 r = vcall<i32>(o, 0xc, arg);
        jo::view<u32>(jo::self<jo::ScriptLoad>(this)->value_type) = 1;
        inline_value<i32>(jo::self<jo::ScriptLoad>(this)->value) = r;
        return r == 0 ? 0u : 1u;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_13", name, addr, fn)
B("ScriptNode::run_seq", 0x0063f790, &H::run_seq);
B("ScriptNode::run_once_63fb80", 0x0063fb80, &H::run_once);
B("ScriptNode::run_once_63fde0", 0x0063fde0, &H::run_once);
B("ScriptNode::run_until", 0x00640370, &H::run_until);
B("ScriptNode::size_641f20", 0x00641f20, &H::size_641f20);
B("ScriptNode::size_641fb0", 0x00641fb0, &H::size_641fb0);
B("ScriptNode::size_642070", 0x00642070, &H::size_642070);
B("ScriptNode::size_642100", 0x00642100, &H::size_642100);
B("ScriptNode::find_param", 0x00643700, &H::find_param);
B("ScriptNode::run_params", 0x00643b60, &H::run_params);
B("ScriptNode::assign", 0x00644530, &H::assign);
B("ScriptNode::logic_op", 0x006453a0, &H::logic_op);
B("ScriptNode::call_func", 0x00645d40, &H::call_func);
#undef B

}  // namespace game_b102_13
}  // namespace

// ---------------------------------------- from modules/game_b111_04.cpp (game/b111_04)
namespace {
namespace game_b111_04 {

using u8 = std::uint8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using joan::call::thiscall;

u8* cur(void* rd) { return joan::orig::arg::as<u8*>(joan::orig::as_script_lexer(rd)->skip_newlines()); }   // 0x00642370
u8* next(void* rd) { return joan::orig::arg::as<u8*>(joan::orig::as_script_lexer(rd)->next()); }  // 0x006423e0 (may throw char*)
using joan::orig::helpers::type;
u32 line(const u8* tok) { return jo::view<u32>(jo::as_token(tok)->line); }
char* text(const u8* tok) { return jo::as_token(tok)->text; }

u32 error(void* self, img::Addr msg, u32 ln) {
    return cdecl_call<u32>(fn::Script_error, self, str(msg), ln);  // 0x0063eb80
}
void error_s(void* self, img::Addr msg, u32 ln, const char* name) {
    joan::orig::script::error(self, str(msg), joan::orig::arg::i32(ln), name);  // 0x0063eb80 (callers drop the eax)
}
void expect(void* self, const u8* tok, i32 t, img::Addr msg) {
    if (type(tok) != t) error(self, msg, line(tok));
}
// Virtual parse (slot +0x10) of a node.
std::uintptr_t vparse(void* node, void* rd) { return thiscall<std::uintptr_t>(vtable_slot(node, 0x10), node, rd); }

// Memory from an operator new whose ctor call the original covers with an unwind state: if the ctor
// throws, the unwind funclet frees it with `del` (cdecl). Disarmed once the ctor returned.
struct NewGuard {
    void* mem;
    std::uintptr_t del;
    NewGuard(void* m, std::uintptr_t d) : mem(m), del(d) {}
    ~NewGuard() {
        if (mem != nullptr) cdecl_call(del, mem);
    }
    NewGuard(const NewGuard&) = delete;
    NewGuard& operator=(const NewGuard&) = delete;
};
constexpr fn::Code kOperatorNew = fn::op_new;
constexpr fn::Code kOperatorDelete = fn::op_delete;

// new(0x1d) + list ctor 0x005843d0(elem, a, b), state 0 -> operator delete.
jo::DynArray* new_list(u32 elem, u32 a, u32 b) {
    void* const mem = cdecl_call<void*>(kOperatorNew, static_cast<u32>(sizeof(jo::DynArray)));  // 0x1d on x86
    NewGuard g(mem, fn::code_value(kOperatorDelete));
    jo::DynArray* list = nullptr;
    if (mem != nullptr) list = joan::orig::arg::as<jo::DynArray*>(joan::orig::dynarray(mem)->construct(joan::orig::arg::i32(elem), joan::orig::arg::i32(a), joan::orig::arg::i32(b)));  // 0x005843d0
    g.mem = nullptr;
    return list;
}

// Shared tail: `if (this+0x14) eax = 0x004b9d40(this+0x14)`; eax otherwise is `fallback`.
std::uintptr_t finish_14(void* self, std::uintptr_t fallback) {
    if (jo::as_script_block(self)->statements != nullptr) return joan::orig::arg::as<u32>(joan::orig::as_counter(jo::as_script_block(self)->statements)->clamp_to_one());  // 0x004b9d40
    return fallback;
}

JOAN_COUNTER(c_id, "ScriptId::parse");
JOAN_COUNTER(c_block, "ScriptInit/Reset::parse");
JOAN_COUNTER(c_named, "ScriptSpeaker/Unit/Object::parse");
JOAN_COUNTER(c_wait, "ScriptWait::parse");
JOAN_COUNTER(c_goto, "ScriptGoto::parse");
JOAN_COUNTER(c_choice, "ScriptChoice::parse");
JOAN_COUNTER(c_const, "ScriptConst::parse");
JOAN_COUNTER(c_vardecl, "ScriptVarDecl::parse");
JOAN_COUNTER(c_call, "ScriptCall::parse");
JOAN_COUNTER(c_func, "ScriptFunc::parse");
JOAN_COUNTER(c_assign, "ScriptAssign::parse");
JOAN_COUNTER(c_compare, "ScriptCompare::parse");

struct H {
    void* self() { return this; }

    // "{ statement* }" loop of Id / Init / Reset (0x00643550 creates the node by token type).
    void statements(void* rd, img::Addr close_msg) {
        for (;;) {
            u8* const tok = cur(rd);
            if (type(tok) == 5) return;
            void* const node = joan::orig::arg::as<void*>(joan::orig::as_script_block(self())->make_statement(joan::orig::arg::i32(type(tok)), joan::orig::arg::u32(line(tok))));  // 0x00643550
            vparse(node, rd);
            if (node == nullptr && type(tok) != 5) error(self(), close_msg, line(tok));
        }
    }

    // ------------------------------------------------------------------ 0x0063f8a0
    std::uintptr_t parse_id(void* rd) {
        c_id.hit();
        u8* tok = cur(rd);
        expect(self(), tok, 0xb, d::s_Expecting_Id);  // 'Expecting Id!'
        tok = next(rd);
        if (type(tok) != 1) {
            error(self(), d::s_Expecting_int_after_ID, line(tok));  // 'Expecting int after ID.'
        } else {
            joan::orig::as_script_lexer(rd)->expect_int(joan::orig::arg::mptr(&jo::self<jo::ScriptId>(this)->label_id));  // 0x00661ff0
        }
        tok = cur(rd);
        expect(self(), tok, 4, d::s_Expecting_after_ID);  // "Expecting '{' after ID."
        next(rd);
        statements(rd, d::s_Expecting_after_Init_9daa04);  // "Expecting '}' after Init."
        void* root = this;
        while (jo::as_script_block(root)->parent != nullptr) root = jo::as_script_block(root)->parent;
        if (jo::as_script_block(root)->labels == nullptr) jo::as_script_block(root)->labels = new_list(static_cast<u32>(sizeof(void*)), 1u, 0u);  // label entry pointers
        void* entry = nullptr;
        {
            void* const mem = joan::orig::arg::as<void*>(joan::orig::pool::alloc(joan::orig::arg::u32(sizeof(jo::ScriptId))));  // 0x0063e450 (0x25)
            NewGuard g(mem, fn::code_value(fn::Pool_free));                             // 0x0063e4f0
            if (mem != nullptr) entry = joan::orig::arg::as<void*>(joan::orig::as_script_id(mem)->construct());  // 0x006424f0
            g.mem = nullptr;
        }
        void* const e = entry;
        thiscall<u32>(vtable_slot(e, 4), e, jo::self<jo::ScriptId>(this)->parent);  // entry->v4(this+4)
        joan::orig::as_script_id(e)->set_21(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::ScriptId>(this)->label_id)));          // 0x00642560
        joan::orig::dynarray(jo::as_script_block(root)->labels)->linkin(&entry);          // 0x00584620
        const std::uintptr_t r = reinterpret_cast<std::uintptr_t>(next(rd));
        return finish_14(this, r);
    }

    // ------------------------------------------------------------------ 0x0063fca0 / 0x0063ff00
    std::uintptr_t parse_block(void* rd, i32 kw, img::Addr m_kw, img::Addr m_open, img::Addr m_close) {
        c_block.hit();
        u8* tok = cur(rd);
        expect(self(), tok, kw, m_kw);
        tok = next(rd);
        expect(self(), tok, 4, m_open);
        next(rd);
        statements(rd, m_close);
        const std::uintptr_t r = reinterpret_cast<std::uintptr_t>(next(rd));
        return finish_14(this, r);
    }
    std::uintptr_t parse_init(void* rd) {
        // 'Expecting Init!', "Expecting '{' after Init.", "Expecting '}' after Init."
        return parse_block(rd, 0xc, d::s_Expecting_Init, d::s_Expecting_after_Init_9daa6c, d::s_Expecting_after_Init_9daa04);
    }
    std::uintptr_t parse_reset(void* rd) {
        // 'Expecting Reset!', "Expecting '{' after Reset.", "Expecting '}' after Reset."
        return parse_block(rd, 0xd, d::s_Expecting_Reset, d::s_Expecting_after_Reset_9daac0, d::s_Expecting_after_Reset_9daaa0);
    }

    // ------------------------------------------------------------------ 0x00640060 / 170 / 280
    std::uintptr_t parse_named(void* rd, i32 kw, img::Addr m_kw, img::Addr m_colon, std::uintptr_t lookup,
                    img::Addr table) {
        c_named.hit();
        u8* tok = cur(rd);
        expect(self(), tok, kw, m_kw);
        tok = next(rd);
        expect(self(), tok, 8, m_colon);
        tok = next(rd);
        expect(self(), tok, 3, d::s_Expecting_string_after);  // "Expecting string after ':'."
        jo::self<jo::ScriptNamed>(this)->type_id = thiscall<u32>(lookup, obj(table), text(tok));
        return reinterpret_cast<std::uintptr_t>(next(rd));
    }
    std::uintptr_t parse_speaker(void* rd) {
        // 'Expecting Speaker!', "Expecting ':' after Speaker."; 0x005270b0
        return parse_named(rd, 0x12, d::s_Expecting_Speaker, d::s_Expecting_after_Speaker, fn::code_value(fn::UnitTypes_find), d::g_types);
    }
    std::uintptr_t parse_unit(void* rd) {
        // 'Expecting Unit!', "Expecting ':' after Unit."; 0x005270b0
        return parse_named(rd, 0x13, d::s_Expecting_Unit, d::s_Expecting_after_Unit, fn::code_value(fn::UnitTypes_find), d::g_types);
    }
    std::uintptr_t parse_object(void* rd) {
        // 'Expecting Object!', 'Expecting : after Object.'; 0x004a36c0
        return parse_named(rd, 0x14, d::s_Expecting_Object, d::s_Expecting_after_Object, fn::code_value(fn::NameTable_find_4a36c0), d::g_item_res);
    }

    // ------------------------------------------------------------------ 0x006404e0
    void sub_node(u32 kind, void* rd) {
        const u32 a = joan::orig::arg::as<u32>(joan::orig::as_script_block(this)->add_child(joan::orig::arg::u32(kind)));        // 0x0063ea20
        void* const n = joan::orig::arg::as<void*>(joan::orig::as_reader(this)->list_get(joan::orig::arg::i32(a)));       // 0x0063e9f0
        vparse(n, rd);
    }
    std::uintptr_t parse_wait(void* rd) {
        c_wait.hit();
        u8* tok = cur(rd);
        expect(self(), tok, 0xe, d::s_Expecting_Wait);  // 'Expecting Wait!'
        tok = next(rd);
        expect(self(), tok, 4, d::s_Expecting_after_Wait_9dabe0);  // "Expecting '{' after Wait."
        next(rd);
        for (;;) {
            tok = cur(rd);
            if (type(tok) == 0x17 || type(tok) == 0x18 || type(tok) == 0x19 || type(tok) == 0x1a) {
                sub_node(0x14u, rd);
            } else if (type(tok) == 0x21) {
                sub_node(0x12u, rd);
            } else {
                if (type(tok) == 5) break;
                error(self(), d::s_Expecting_after_Wait_9dabc0, line(tok));  // "Expecting '}' after Wait."
            }
        }
        const std::uintptr_t r = reinterpret_cast<std::uintptr_t>(next(rd));
        return finish_14(this, r);
    }

    // ------------------------------------------------------------------ 0x006408f0
    std::uintptr_t parse_goto(void* rd) {
        c_goto.hit();
        u8* tok = cur(rd);
        expect(self(), tok, 0x15, d::s_Expecting_Goto);  // 'Expecting Goto!'
        tok = next(rd);
        expect(self(), tok, 8, d::s_Expecting_after_Goto);  // "Expecting ':' after Goto."
        tok = next(rd);
        expect(self(), tok, 1, d::s_Expecting_int_after);                     // "Expecting int after ':'."
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_script_lexer(rd)->expect_int(joan::orig::arg::mptr(&jo::self<jo::ScriptGoto>(this)->target_label)));  // 0x00661ff0
    }

    // ------------------------------------------------------------------ 0x006409f0
    std::uintptr_t parse_exit(void* rd) {
        expect(self(), cur(rd), 0x16, d::s_Expecting_Exit);  // 'Expecting Exit!'
        return reinterpret_cast<std::uintptr_t>(next(rd));
    }

    // ------------------------------------------------------------------ 0x00640bd0
    std::uintptr_t parse_choice(void* rd) {
        c_choice.hit();
        u8* tok = cur(rd);
        expect(self(), tok, 0xf, d::s_Expecting_Choice);  // 'Expecting Choice!'
        tok = next(rd);
        expect(self(), tok, 4, d::s_Expecting_after_Choice_9dacdc);  // "Expecting '{' after Choice."
        tok = next(rd);
        for (;;) {
            if (type(tok) == 3) {
                if (jo::self<jo::ScriptChoice>(this)->options == nullptr) jo::self<jo::ScriptChoice>(this)->options = new_list(0x191u, 1u, 0u);
                joan::orig::dynarray(jo::self<jo::ScriptChoice>(this)->options)->linkin(joan::orig::arg::ptr(tok));  // 0x00584620
                tok = next(rd);
                if (type(tok) == 7) {
                    tok = next(rd);
                } else {
                    if (type(tok) == 5) break;
                    error(self(), d::s_Expecting_after_Choice_9dacb8, line(tok));  // "Expecting '}' after Choice."
                }
            } else {
                if (type(tok) == 5) break;
                error(self(), d::s_Expecting_after_Choice_9dacb8, line(tok));
            }
        }
        return reinterpret_cast<std::uintptr_t>(next(rd));
    }

    // ------------------------------------------------------------------ 0x006437d0
    std::uintptr_t parse_int(void* rd) { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_script_lexer(rd)->expect_xyz(joan::orig::arg::mptr(jo::self<jo::ScriptInt>(this)->vec))); }  // 0x006620d0

    // ------------------------------------------------------------------ 0x00643820
    std::uintptr_t parse_const(void* rd) {
        c_const.hit();
        u8* const tok = cur(rd);
        jo::self<jo::ScriptConst>(this)->value_type = type(tok);
        const u32 k = static_cast<u32>(type(tok)) - 1u;
        // byte table 0x0064393a: 0->case 0, 1->1, 2->2, 26->3, else 4 (default); ja for k > 0x1a.
        if (k == 0) return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_script_lexer(rd)->expect_int(joan::orig::arg::mptr(&jo::self<jo::ScriptConst>(this)->value)));  // 0x00661ff0
        if (k == 1) return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_script_lexer(rd)->expect_float(joan::orig::arg::mptr(&jo::self<jo::ScriptConst>(this)->value)));  // 0x00662060
        if (k == 2) {
            const u32 len = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(text(tok))));  // strlen
            jo::self<jo::ScriptConst>(this)->value = cdecl_call<void*>(kOperatorNew, len + 1u);
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::ScriptConst>(this)->value)), joan::orig::arg::str(text(tok)));  // strcpy
            return reinterpret_cast<std::uintptr_t>(next(rd));
        }
        if (k == 26) {
            jo::self<jo::ScriptConst>(this)->value = cdecl_call<void*>(kOperatorNew, 0xcu);
            return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_script_lexer(rd)->expect_xyz(joan::orig::arg::mptr(jo::self<jo::ScriptConst>(this)->value)));  // 0x006620d0
        }
        return error(self(), d::s_Undefined_Constant, line(tok));  // 'Undefined Constant!'
    }

    // ------------------------------------------------------------------ 0x006439d0
    std::uintptr_t parse_vardecl(void* rd) {
        c_vardecl.hit();
        u8* tok = cur(rd);
        const u32 k = static_cast<u32>(type(tok)) - 0x1du;
        if (k == 0) {
            jo::view<u32>(jo::self<jo::ScriptVarDecl>(this)->value_type) = 1;
            jo::self<jo::ScriptVarDecl>(this)->value = 0;
        } else if (k == 1) {
            jo::view<u32>(jo::self<jo::ScriptVarDecl>(this)->value_type) = 2;
            jo::self<jo::ScriptVarDecl>(this)->value = 0;
        } else if (k == 2) {
            jo::view<u32>(jo::self<jo::ScriptVarDecl>(this)->value_type) = 3;
            void* const mem = cdecl_call<void*>(kOperatorNew, 0x191u);
            NewGuard g(mem, fn::code_value(kOperatorDelete));
            void* v = nullptr;
            if (mem != nullptr) v = joan::orig::arg::as<void*>(joan::orig::as_string(mem)->construct());  // 0x005a1170
            g.mem = nullptr;
            jo::self<jo::ScriptVarDecl>(this)->value = v;
        } else if (k == 3) {
            jo::view<u32>(jo::self<jo::ScriptVarDecl>(this)->value_type) = 0x1b;
            jo::self<jo::ScriptVarDecl>(this)->value = cdecl_call<void*>(kOperatorNew, 0xcu);
        }
        tok = next(rd);
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::ScriptVarDecl>(this)->name_)), joan::orig::arg::str(text(tok)));  // strcpy
        return reinterpret_cast<std::uintptr_t>(next(rd));
    }

    // ------------------------------------------------------------------ 0x00643c00
    std::uintptr_t parse_call(void* rd) {
        c_call.hit();
        u8* tok = cur(rd);
        expect(self(), tok, 3, d::s_Expecting_a_function_name);  // 'Expecting a function name!'
        jo::view<u32>(jo::self<jo::ScriptFuncCall>(this)->value_type) = 1;
        jo::self<jo::ScriptFuncCall>(this)->func_id = joan::orig::arg::as<i16>(joan::orig::random_table_lookup(joan::orig::arg::str(text(tok)), joan::orig::arg::mptr(&jo::self<jo::ScriptFuncCall>(this)->value_type)));  // 0x00661e10
        if (jo::self<jo::ScriptFuncCall>(this)->func_id == 0)
            error_s(self(), d::s_Wrong_function_name_s, line(tok), text(tok));  // 'Wrong function name (%s)!'
        tok = next(rd);
        while (type(tok) == 7) {
            next(rd);
            sub_node(0x10u, rd);
            tok = cur(rd);
        }
        return finish_14(this, reinterpret_cast<std::uintptr_t>(this));
    }

    // ------------------------------------------------------------------ 0x00644190
    std::uintptr_t parse_func(void* rd) {
        c_func.hit();
        u8* tok = cur(rd);
        const i32 kind = type(tok);
        if (kind < 0x17 || kind > 0x1a)
            error(self(), d::s_Expecting_FUNC_DIALOG_CAMERA, line(tok));  // 'Expecting FUNC/DIALOG/CAMERA/ACTION!'
        tok = next(rd);
        expect(self(), tok, -0x25, d::s_Expecting_after_a_function);  // 'Expecting ( after a function.'
        next(rd);
        const u32 k = static_cast<u32>(kind) - 0x17u;
        if (k <= 3) jo::self<jo::Value>(this)->ref = joan::orig::arg::as<jo::ScriptNode*>(joan::orig::as_script_node(this)->create_child(joan::orig::arg::i32(0x15u + k)));  // 0x00640db0
        vparse(jo::self<jo::Value>(this)->ref, rd);
        tok = cur(rd);
        expect(self(), tok, -0x24, d::s_Expecting_at_the_end_of_a_fu);  // 'Expecting ) at the end of a function.'
        return reinterpret_cast<std::uintptr_t>(next(rd));
    }

    // ------------------------------------------------------------------ 0x00644680
    std::uintptr_t parse_assign(void* rd) {
        c_assign.hit();
        u8* tok = cur(rd);
        expect(self(), tok, 0x21, d::s_Expecting_Variable_Name);  // 'Expecting Variable Name!'
        jo::self<jo::ScriptAssign>(this)->target = thiscall<jo::ScriptNode*>(vtable_slot(this, 0x18), this, text(tok));  // this->v18
        if (jo::self<jo::ScriptAssign>(this)->target == nullptr)
            error_s(self(), d::s_Variable_not_found_s, line(tok), text(tok));  // 'Variable not found ! (%s)'
        tok = next(rd);
        expect(self(), tok, 6, d::s_Expecting_after_Variable_Nam);  // 'Expecting = after Variable Name.'
        next(rd);
        jo::self<jo::ScriptAssign>(this)->expr = joan::orig::arg::as<jo::ScriptNode*>(joan::orig::as_script_node(this)->create_child(0x10));  // 0x00640db0
        return vparse(jo::self<jo::ScriptAssign>(this)->expr, rd);
    }

    // ------------------------------------------------------------------ 0x00645210
    std::uintptr_t parse_compare(void* rd) {
        c_compare.hit();
        cur(rd);
        jo::self<jo::Expr>(this)->lhs = joan::orig::arg::as<jo::ScriptNode*>(joan::orig::as_script_node(this)->create_child(0x10));  // 0x00640db0
        vparse(jo::self<jo::Expr>(this)->lhs, rd);
        u8* const tok = cur(rd);
        const i32 t = type(tok);
        if (t < -0x2d || t > -0x28) {
            jo::self<jo::Expr>(this)->op = 0x22;
            return reinterpret_cast<std::uintptr_t>(tok);  // eax: the token pointer
        }
        jo::self<jo::Expr>(this)->op = type(tok);
        next(rd);
        jo::self<jo::Expr>(this)->rhs = joan::orig::arg::as<jo::ScriptNode*>(joan::orig::as_script_node(this)->create_child(0x10));  // 0x00640db0
        return vparse(jo::self<jo::Expr>(this)->rhs, rd);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_04", name, addr, fn)
B("ScriptId::parse_63f8a0", 0x0063f8a0, &H::parse_id);
B("ScriptInit::parse_63fca0", 0x0063fca0, &H::parse_init);
B("ScriptReset::parse_63ff00", 0x0063ff00, &H::parse_reset);
B("ScriptSpeaker::parse_640060", 0x00640060, &H::parse_speaker);
B("ScriptUnit::parse_640170", 0x00640170, &H::parse_unit);
B("ScriptObject::parse_640280", 0x00640280, &H::parse_object);
B("ScriptWait::parse_6404e0", 0x006404e0, &H::parse_wait);
B("ScriptGoto::parse_6408f0", 0x006408f0, &H::parse_goto);
B("ScriptExit::parse_6409f0", 0x006409f0, &H::parse_exit);
B("ScriptChoice::parse_640bd0", 0x00640bd0, &H::parse_choice);
B("ScriptInt::parse_6437d0", 0x006437d0, &H::parse_int);
B("ScriptConst::parse_643820", 0x00643820, &H::parse_const);
B("ScriptVarDecl::parse_6439d0", 0x006439d0, &H::parse_vardecl);
B("ScriptCall::parse_643c00", 0x00643c00, &H::parse_call);
B("ScriptFunc::parse_644190", 0x00644190, &H::parse_func);
B("ScriptAssign::parse_644680", 0x00644680, &H::parse_assign);
B("ScriptCompare::parse_645210", 0x00645210, &H::parse_compare);
#undef B

}  // namespace game_b111_04
}  // namespace

// ---------------------------------------- from modules/game_b101_12.cpp (game/b101_12)
namespace {
namespace game_b101_12 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::str;

using joan::orig::helpers::op_new;

using joan::call::thiscall;
// Virtual slot of an object (thiscall target address).
std::uintptr_t vslot(const void* o, std::int32_t off) { return vtable_slot(o, off); }

JOAN_COUNTER(c_tree_select, "Tree::select");
JOAN_COUNTER(c_assign_643f80, "Value::assign_643f80");
JOAN_COUNTER(c_assign_644900, "Value::assign_644900");

struct H {
    // 0x00640690
    i32 tree_select(u32 /*unused*/) {
        c_tree_select.hit();
        void* root = this;
        bool found = false;
        i32 more = 1;
        while (jo::view<void*>(jo::as_script_block(root)->parent) != nullptr) root = jo::view<void*>(jo::as_script_block(root)->parent);
        joan::orig::as_counter(jo::view<void*>(jo::as_script_block(root)->labels))->clamp_to_one();  // 0x004b9d40
        void* item = nullptr;
        i32 index = 0;
        for (i32 i = 1; i <= joan::orig::as_dynarray(jo::view<void*>(jo::as_script_block(root)->labels))->size() && more != 0; ++i) {  // 0x00417ad0
            void* const cur = joan::orig::arg::as<void*>(joan::orig::as_table(jo::view<void*>(jo::as_script_block(root)->labels))->last());                 // 0x00585f20
            item = thiscall<void*>(vslot(cur, 0x28), cur);
            if (joan::orig::arg::as<i32>(joan::orig::as_script_id(item)->get_21()) == jo::self<jo::ScriptGoto>(this)->target_label) {  // 0x00642540
                found = true;
                index = i;
                break;
            }
            more = joan::orig::arg::as<i32>(joan::orig::as_reader(jo::view<void*>(jo::as_script_block(root)->labels))->next());  // 0x0063ee20
        }
        if (!found) return -2;
        for (void* n = jo::view<void*>(jo::self<jo::ScriptGoto>(this)->parent); n != nullptr; n = jo::view<void*>(jo::as_script_block(n)->parent)) {
            if (thiscall<void*>(vslot(n, 0x24), n) != nullptr) {
                void* const o = thiscall<void*>(vslot(n, 0x24), n);
                joan::orig::as_counter(jo::view<void*>(jo::as_script_block(o)->statements))->clamp_to_one();
            }
        }
        void* node = jo::view<void*>(jo::as_script_block(item)->parent);
        void* const list = thiscall<void*>(vslot(node, 0x24), node);
        void* const entry = joan::orig::arg::as<void*>(joan::orig::as_reader(list)->list_get(joan::orig::arg::i32(index)));  // 0x0063e9f0
        item = thiscall<void*>(vslot(entry, 0x28), entry);
        joan::orig::as_counter(jo::view<void*>(jo::as_script_block(item)->statements))->clamp_to_one();
        if (list == nullptr) return 2;
        joan::orig::as_script_block(list)->seek_child(joan::orig::arg::u32(index));  // 0x0063eda0
        while (jo::view<void*>(jo::as_script_block(node)->parent) != nullptr) {
            void* const next = jo::view<void*>(jo::as_script_block(node)->parent);
            if (thiscall<void*>(vslot(next, 0x24), next) != nullptr) {
                void* const next2 = jo::view<void*>(jo::as_script_block(node)->parent);
                void* const sub = thiscall<void*>(vslot(next2, 0x24), next2);
                for (i32 j = 1; j <= joan::orig::arg::as<i32>(joan::orig::as_reader(sub)->count()); ++j) {  // 0x0063ecd0
                    if (joan::orig::as_dynarray(jo::view<void*>(jo::as_script_block(sub)->statements))->ptr_at(joan::orig::arg::i32(j)) == node)  // 0x0041b3f0
                        joan::orig::as_script_block(sub)->seek_child(joan::orig::arg::u32(j));
                }
            }
            node = jo::view<void*>(jo::as_script_block(node)->parent);
        }
        return 2;
    }

    // 0x00642db0
    u32 dtor_642db0() {
        jo::self<jo::ScriptCond>(this)->vtable = img::obj(d::vt_Obj_9daf8c);
        joan::orig::as_widget(this)->dtor_body_6452f0();           // 0x006452f0
        return joan::orig::arg::as<u32>(joan::orig::as_ctrl(this)->dtor_Ctrl_63e600());  // 0x0063e600
    }

    // Allocation part of the kind switch shared by 0x00643f80 / 0x00644900 (kinds 3 / 0x1b).
    void assign_kind() {
        switch (jo::self<jo::ScriptNode>(this)->value_type) {
        case 3:
            if (jo::view<void*>(jo::self<jo::ScriptNode>(this)->value) == nullptr) {
                void* const m = op_new(0x191);
                jo::view<void*>(jo::self<jo::ScriptNode>(this)->value) = m != nullptr ? joan::orig::as_string(m)->construct() : nullptr;  // 0x005a1170
            }
            break;
        case 0x1b:
            if (jo::view<void*>(jo::self<jo::ScriptNode>(this)->value) == nullptr) jo::view<void*>(jo::self<jo::ScriptNode>(this)->value) = op_new(0xc);
            break;
        default:
            break;
        }
    }

    // 0x00643f80
    u32 assign_643f80(std::uintptr_t arg) {
        c_assign_643f80.hit();
        void* const s0 = jo::view<void*>(jo::self<jo::Value>(this)->ref);
        const u32 r = thiscall<u32>(vslot(s0, 0xc), s0, arg);
        jo::view<u32>(jo::self<jo::Value>(this)->value_type) = jo::view<u32>(jo::self<jo::Value>(this)->ref->value_type);
        switch (jo::self<jo::Value>(this)->value_type) {
        case 1:
        case 2:
            jo::self<jo::Value>(this)->value = jo::self<jo::Value>(this)->ref->value;
            break;
        case 3:
            assign_kind();
            if (jo::view<void*>(jo::self<jo::Value>(this)->ref->value) != nullptr)
                joan::orig::as_string(jo::view<void*>(jo::self<jo::Value>(this)->value))->assign(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Value>(this)->ref->value)));  // 0x005a1440
            else
                joan::orig::as_string(jo::view<void*>(jo::self<jo::Value>(this)->value))->assign(str(d::kEmpty));
            break;
        case 0x1b: {
            assign_kind();
            const u32* const s = jo::view<u32*>(jo::self<jo::Value>(this)->ref->value);
            u32* const d = jo::view<u32*>(jo::self<jo::Value>(this)->value);
            d[0] = s[0];
            d[1] = s[1];
            d[2] = s[2];
            break;
        }
        default:
            break;
        }
        return r;
    }

    // 0x00644900
    i32 assign_644900(std::uintptr_t arg) {
        c_assign_644900.hit();
        void* src;
        if (jo::view<void*>(jo::self<jo::ScriptExpr>(this)->rhs) != nullptr) {
            src = jo::view<void*>(jo::self<jo::ScriptExpr>(this)->rhs);
        } else {
            src = jo::view<void*>(jo::self<jo::ScriptExpr>(this)->lhs);
        }
        thiscall<u32>(vslot(src, 0xc), src, arg);
        jo::view<u32>(jo::self<jo::ScriptExpr>(this)->value_type) = jo::view<u32>(jo::as_script_node(src)->value_type);
        switch (jo::self<jo::ScriptExpr>(this)->value_type) {
        case 1:
        case 2:
            jo::self<jo::ScriptExpr>(this)->value = jo::as_script_node(src)->value;
            break;
        case 3:
            assign_kind();
            joan::orig::as_string(jo::view<void*>(jo::self<jo::ScriptExpr>(this)->value))->assign(joan::orig::arg::str(jo::view<void*>(jo::as_script_node(src)->value)));  // 0x005a1440
            break;
        case 0x1b: {
            assign_kind();
            const u32* const s = jo::view<u32*>(jo::as_script_node(src)->value);
            u32* const d = jo::view<u32*>(jo::self<jo::ScriptExpr>(this)->value);
            d[0] = s[0];
            d[1] = s[1];
            d[2] = s[2];
            break;
        }
        default:
            break;
        }
        return 1;
    }
};

#define R12(name, addr, fn) JOAN_REPLACE("game/b101_12", name, addr, fn)
R12("Tree::select", 0x00640690, &H::tree_select);
R12("Obj::dtor_642db0", 0x00642db0, &H::dtor_642db0);
R12("Value::assign_643f80", 0x00643f80, &H::assign_643f80);
R12("Value::assign_644900", 0x00644900, &H::assign_644900);
#undef R12

}  // namespace game_b101_12
}  // namespace

// ---------------------------------------- from modules/game_b96_06_2.cpp (game/b96_06)
namespace {
namespace game_b96_06_2 {

using u32 = std::uint32_t;

using joan::img::obj;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x00640af0
    void dtor_640af0() {
        jo::self<jo::ScriptChoice>(this)->vtable = img::obj(d::vt_Obj_9dac7c);
        if (jo::self<jo::ScriptChoice>(this)->options != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::ScriptChoice>(this)->options);
            if (p != nullptr) joan::orig::as_dynarray(p)->destroy(1u);  // 0x00463fc0 scalar deleting dtor
            jo::self<jo::ScriptChoice>(this)->options = nullptr;
        }
        joan::orig::as_ctrl(this)->dtor_Ctrl_63e600();  // 0x0063e600
    }

    // ------------------------------------------------------------------ 0x00642860
    void dtor_642860() {
        jo::self<jo::ScriptSpeaker>(this)->vtable = img::obj(d::vt_Obj_9dade8);
        joan::orig::as_ctrl(this)->dtor_Ctrl_63e600();  // 0x0063e600
    }

    // ------------------------------------------------------------------ 0x00642ab0
    void dtor_642ab0() {
        jo::self<jo::ScriptWait>(this)->vtable = img::obj(d::vt_Node_9dae9c);
        joan::orig::as_script_block(this)->destruct();  // 0x0063e780
    }

    // ------------------------------------------------------------------ 0x00642d20
    void* ctor_642d20() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::ScriptCond>(this)->vtable = img::obj(d::vt_Obj_9daf8c);
        jo::view<u32>(jo::self<jo::ScriptCond>(this)->kind) = 0xe;
        return this;
    }

    // ------------------------------------------------------------------ 0x00643090
    void dtor_643090() {
        jo::self<jo::ScriptFuncCall>(this)->vtable = img::obj(d::vt_ScriptNode_9db040);
        joan::orig::as_script_block(this)->destruct();  // 0x0063e780
    }

    // ------------------------------------------------------------------ 0x00643390
    void dtor_643390() {
        jo::self<jo::ScriptConst>(this)->vtable = img::obj(d::vt_Obj_9db150);
        joan::orig::as_ctrl(this)->dtor_Ctrl_63e600();  // 0x0063e600
    }

    // ------------------------------------------------------------------ 0x00644390
    u32 forward_34(std::uintptr_t a) { return virtual_call<u32>(jo::view<void*>(jo::self<jo::Value>(this)->ref), 0x34, a); }

    // ------------------------------------------------------------------ 0x00645ca0
    void dtor_645ca0() {
        jo::self<jo::ScriptLoad>(this)->vtable = img::obj(d::vt_Node_9db1c8);
        if (jo::self<jo::ScriptLoad>(this)->script != 0) {
            joan::orig::as_script_cache(obj(d::g_scripts))->release(joan::orig::arg::i32(jo::self<jo::ScriptLoad>(this)->script));  // 0x00648d60
            jo::self<jo::ScriptLoad>(this)->script = 0;
        }
        joan::orig::as_ctrl(this)->dtor_Ctrl_63e600();  // 0x0063e600
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Obj::~Obj_640af0", 0x00640af0, &H::dtor_640af0);
B96("Obj::~Obj_642860", 0x00642860, &H::dtor_642860);
B96("Obj::~Obj_642ab0", 0x00642ab0, &H::dtor_642ab0);
B96("Obj::Obj_642d20", 0x00642d20, &H::ctor_642d20);
B96("Obj::~Obj_643090", 0x00643090, &H::dtor_643090);
B96("Obj::~Obj_643390", 0x00643390, &H::dtor_643390);
B96("Obj::forward_34", 0x00644390, &H::forward_34);
B96("Obj::~Obj_645ca0", 0x00645ca0, &H::dtor_645ca0);
#undef B96

}  // namespace game_b96_06_2
}  // namespace

// ---------------------------------------- from modules/game_b106_07.cpp (game/b106_07)
namespace {
namespace game_b106_07 {

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

JOAN_COUNTER(c_factory, "game/b106_07 Factory::create");

// ---- 0x00640db0 table --------------------------------------------------------------------
struct Kind {
    u32 size;       // the node class (x86 sizes in the comments; x64 nodes are larger: vtable and pointer members)
    fn::Code ctor;  // thunk
};
constexpr Kind kKinds[27] = {
    {static_cast<u32>(sizeof(jo::Script)), fn::Ctrl_Ctrl_63f190},  //  1 0x0063f190 Script, 0x70 on x86
    {static_cast<u32>(sizeof(jo::ScriptId)), fn::Node2_Node2},  //  2 0x006424f0 ScriptId, 0x25 on x86
    {static_cast<u32>(sizeof(jo::ScriptInit)), fn::Widget_ctor_642630},  //  3 0x00642630 ScriptInit, 0x71 on x86
    {static_cast<u32>(sizeof(jo::ScriptReset)), fn::Obj_ctor_642700},  //  4 0x00642700 ScriptReset, 0x71 on x86
    {static_cast<u32>(sizeof(jo::ScriptSpeaker)), fn::Node6427_ctor},  //  5 0x006427d0 ScriptSpeaker, 0x18 on x86
    {static_cast<u32>(sizeof(jo::ScriptUnit)), fn::Node_Node_642890},  //  6 0x00642890 ScriptUnit, 0x18 on x86
    {static_cast<u32>(sizeof(jo::ScriptObject)), fn::Node_ctor_642950},  //  7 0x00642950 ScriptObject, 0x18 on x86
    {static_cast<u32>(sizeof(jo::ScriptWait)), fn::Node_Node_642a10},  //  8 0x00642a10 ScriptWait, 0x21 on x86
    {static_cast<u32>(sizeof(jo::ScriptGoto)), fn::Node642a_ctor},  //  9 0x00642ae0 ScriptGoto, 0x18 on x86
    {static_cast<u32>(sizeof(jo::ScriptExit)), fn::Widget_ctor_642ba0},  // 10 0x00642ba0 ScriptExit, 0x14 on x86
    {static_cast<u32>(sizeof(jo::ScriptChoice)), fn::Widget_ctor_640a50},  // 11 0x00640a50 ScriptChoice, 0x18 on x86
    {static_cast<u32>(sizeof(jo::ScriptIf)), fn::Node6487_ctor},  // 12 0x00648770 ScriptIf, 0x24 on x86
    {static_cast<u32>(sizeof(jo::ScriptElse)), fn::Node_Node_642c60},  // 13 0x00642c60 ScriptElse, 0x20 on x86
    {static_cast<u32>(sizeof(jo::ScriptCond)), fn::Obj_Obj_642d20},  // 14 0x00642d20 ScriptCond, 0x20 on x86
    {static_cast<u32>(sizeof(jo::Expr)), fn::Widget_ctor_642e20},  // 15 0x00642e20 Expr, 0x20 on x86
    {static_cast<u32>(sizeof(jo::ScriptExpr)), fn::Node6447_ctor},  // 16 0x006447a0 ScriptExpr, 0x1c on x86
    {static_cast<u32>(sizeof(jo::ScriptVarDecl)), fn::Node11_Node11},  // 17 0x00642f20 ScriptVarDecl, 0x48 on x86
    {static_cast<u32>(sizeof(jo::ScriptAssign)), fn::Widget_ctor_6443d0},  // 18 0x006443d0 ScriptAssign, 0x1c on x86
    {static_cast<u32>(sizeof(jo::ScriptConst)), fn::Node_Node_643300},  // 19 0x00643300 ScriptConst, 0x14 on x86
    {static_cast<u32>(sizeof(jo::Value)), fn::Node_ctor_643e30},  // 20 0x00643e30 Value, 0x18 on x86
    {static_cast<u32>(sizeof(jo::ScriptFuncCall)), fn::Ctrl_Ctrl_643000},  // 21 0x00643000 ScriptFuncCall, 0x22 on x86
    {static_cast<u32>(sizeof(jo::ScriptCheckCall)), fn::Node6430_ctor},  // 22 0x006430c0 ScriptCheckCall, 0x22 on x86
    {static_cast<u32>(sizeof(jo::ScriptCameraCall)), fn::Widget_ctor_643180},  // 23 0x00643180 ScriptCameraCall, 0x22 on x86
    {static_cast<u32>(sizeof(jo::ScriptCall)), fn::Widget_ctor_643240},  // 24 0x00643240 ScriptCall, 0x24 on x86
    {static_cast<u32>(sizeof(jo::ScriptInt)), fn::Widget_ctor_6433c0},  // 25 0x006433c0 ScriptInt, 0x20 on x86
    {static_cast<u32>(sizeof(jo::ScriptLoad)), fn::Obj_ctor_643480},  // 26 0x00643480 ScriptLoad, 0x18 on x86
    {static_cast<u32>(sizeof(jo::MathExpr)), fn::Node_ctor_646290},  // 27 0x00646290 MathExpr, 0x24 on x86
};

struct H {
    // 0x00640db0
    void* create(i32 kind) {
        c_factory.hit();
        void* o = nullptr;
        const u32 k = static_cast<u32>(kind - 1);
        if (k <= 0x1a) {
            void* const mem = joan::orig::arg::as<void*>(joan::orig::pool::alloc(joan::orig::arg::u32(kKinds[k].size)));  // 0x0063e450
            o = mem != nullptr ? thiscall<void*>(kKinds[k].ctor, mem) : nullptr;
        }
        void* const* const vt = *static_cast<void* const* volatile*>(o);  // faults like the original when o == 0
        thiscall<u32>(reinterpret_cast<std::uintptr_t>(vt[1]), o, this);  // vtable slot +4
        return o;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_07", name, addr, fn)
B("Factory::create", 0x00640db0, &H::create);
#undef B

}  // namespace game_b106_07
}  // namespace

// ---------------------------------------- from modules/game_b99_08.cpp (game/b99_08)
namespace {
namespace game_b99_08 {

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
// Virtual thiscall: vtable slot at byte offset `off`.
template <class R = void, class... A>
R vcall(void* o, std::int32_t off, A... args) {
    return thiscall<R>(vtable_slot(o, off), o, args...);
}

JOAN_COUNTER(c_dtor_642eb0, "Node::~Node_642eb0");
JOAN_COUNTER(c_dtor_644850, "Node::~Node_644850");

struct H {
    // ------------------------------------------------------------------ 0x00642560
    void* set_21(i32 v) {
        jo::self<jo::ScriptId>(this)->label_id = v;
        return this;
    }

    // ------------------------------------------------------------------ 0x00642890
    void* ctor_642890() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::ScriptUnit>(this)->vtable = img::obj(d::vt_Obj_9dae24);
        jo::self<jo::ScriptUnit>(this)->kind = 6;
        return this;
    }

    // ------------------------------------------------------------------ 0x00642a10
    void* ctor_642a10() {
        joan::orig::as_script_block(this)->construct();  // 0x0063e6e0
        jo::self<jo::ScriptWait>(this)->vtable = img::obj(d::vt_Node_9dae9c);
        jo::self<jo::ScriptWait>(this)->kind = 8;
        jo::self<jo::ScriptWait>(this)->waiting = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x00642c60
    void* ctor_642c60() {
        joan::orig::as_script_block(this)->construct();  // 0x0063e6e0
        jo::self<jo::ScriptElse>(this)->vtable = img::obj(d::vt_Node_9daf50);
        jo::self<jo::ScriptElse>(this)->kind = 0xd;
        return this;
    }

    // ------------------------------------------------------------------ 0x00642eb0
    u32 dtor_642eb0() {
        c_dtor_642eb0.hit();
        jo::self<jo::Expr>(this)->vtable = img::obj(d::vt_Obj_9dafc8);
        joan::orig::as_script_expr(this)->release();                // 0x00644cd0
        return joan::orig::arg::as<u32>(joan::orig::as_ctrl(this)->dtor_Ctrl_63e600());    // 0x0063e600
    }

    // ------------------------------------------------------------------ 0x00643300
    void* ctor_643300() {
        joan::orig::as_reader(this)->construct();  // 0x0063e550
        jo::self<jo::ScriptConst>(this)->vtable = img::obj(d::vt_Obj_9db150);
        jo::self<jo::ScriptConst>(this)->kind = 0x13;
        return this;
    }

    // ------------------------------------------------------------------ 0x00644850
    u32 dtor_644850() {
        c_dtor_644850.hit();
        jo::self<jo::ScriptExpr>(this)->vtable = img::obj(d::vt_Obj_9db3bc);
        if (jo::view<void*>(jo::self<jo::ScriptExpr>(this)->rhs) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::ScriptExpr>(this)->rhs);
            if (p != nullptr) vcall<void*>(p, 0, 1);  // scalar deleting dtor
        }
        return joan::orig::arg::as<u32>(joan::orig::as_ctrl(this)->dtor_Ctrl_63e600());  // 0x0063e600
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_08", name, addr, fn)
B99("Obj::set_21", 0x00642560, &H::set_21);
B99("Node::Node_642890", 0x00642890, &H::ctor_642890);
B99("Node::Node_642a10", 0x00642a10, &H::ctor_642a10);
B99("Node::Node_642c60", 0x00642c60, &H::ctor_642c60);
B99("Node::~Node_642eb0", 0x00642eb0, &H::dtor_642eb0);
B99("Node::Node_643300", 0x00643300, &H::ctor_643300);
B99("Node::~Node_644850", 0x00644850, &H::dtor_644850);
#undef B99

}  // namespace game_b99_08
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using joan::call::thiscall;

JOAN_COUNTER(c_release_644cd0, "Obj::release_644cd0");

struct H {
    // dtors: vtable, then base dtor (its eax returned).
    std::uint32_t dtor_642600() {
        jo::self<jo::ScriptId>(this)->vtable = img::obj(d::vt_Node_9dad34);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_script_block(this)->destruct());  // 0x0063e780
    }
    std::uint32_t dtor_642920() {
        jo::self<jo::ScriptUnit>(this)->vtable = img::obj(d::vt_Obj_9dae24);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_ctrl(this)->dtor_Ctrl_63e600());  // 0x0063e600
    }
    std::uint32_t dtor_642c30() {
        jo::self<jo::ScriptExit>(this)->vtable = img::obj(d::vt_Obj_9daf14);
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_ctrl(this)->dtor_Ctrl_63e600());  // 0x0063e600
    }
    std::uint32_t dtor_643150() {
        jo::self<jo::ScriptCheckCall>(this)->vtable = img::obj(d::vt_Node_9db084);
        joan::orig::as_script_func_call(this)->destruct();  // 0x00643090
        return 0;  // void callee; destructor, result unused
    }
    std::uint32_t dtor_6432d0() {
        jo::self<jo::ScriptCall>(this)->vtable = img::obj(d::vt_Node_9db10c);
        joan::orig::as_script_func_call(this)->destruct();  // 0x00643090
        return 0;  // void callee; destructor, result unused
    }
    // 0x00643ed0
    std::uint32_t dtor_643ed0() {
        jo::self<jo::Value>(this)->vtable = img::obj(d::vt_Node_9db25c);
        if (jo::view<void*>(jo::self<jo::Value>(this)->ref) != nullptr) virtual_call<std::uint32_t>(jo::view<void*>(jo::self<jo::Value>(this)->ref), 0, 1u);  // scalar deleting dtor
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_ctrl(this)->dtor_Ctrl_63e600());  // 0x0063e600
    }

    // 0x00644cd0
    std::uint32_t release_644cd0() {
        c_release_644cd0.hit();
        if (void* p = jo::view<void*>(jo::self<jo::Expr>(this)->rhs)) virtual_call<std::uint32_t>(p, 0, 1u);
        jo::self<jo::Expr>(this)->rhs = nullptr;
        if (void* p = jo::view<void*>(jo::self<jo::Expr>(this)->lhs)) virtual_call<std::uint32_t>(p, 0, 1u);
        jo::self<jo::Expr>(this)->lhs = nullptr;
        joan::orig::as_script_node(this)->free_value();  // 0x0063e660
        return 0;  // void callee; the only caller ignores the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Obj::dtor_642600", 0x00642600, &H::dtor_642600);
B("Obj::dtor_642920", 0x00642920, &H::dtor_642920);
B("Obj::dtor_642c30", 0x00642c30, &H::dtor_642c30);
B("Obj::dtor_643150", 0x00643150, &H::dtor_643150);
B("Obj::dtor_6432d0", 0x006432d0, &H::dtor_6432d0);
B("Obj::dtor_643ed0", 0x00643ed0, &H::dtor_643ed0);
B("Obj::release_644cd0", 0x00644cd0, &H::release_644cd0);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x00642680
    void* scalar_dtor(u32 flags) {
        joan::orig::as_script_init(me())->destruct();  // 0x006426d0
        if ((flags & 1) != 0) joan::orig::pool::free_(joan::orig::arg::mptr(me()));  // 0x0063e4f0
        return me();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B("Script::parse_init_63fca0", 0x0063fca0, &H::parse_init);
B("ScriptNode::scalar_dtor_642680", 0x00642680, &H::scalar_dtor);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x006426d0
    u32 node_dtor() {
        jo::self<jo::ScriptInit>(this)->vtable = img::obj(d::vt_ScriptNode_9dad70);
        return joan::orig::arg::as<u32>(joan::orig::as_script(this)->dtor_63f250());  // 0x0063f250
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("ScriptNode::dtor_6426d0", 0x006426d0, &H::node_dtor);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x00642750
    H* scalar_dtor_642750(u32 flags) {
        joan::orig::as_script_reset(this)->destruct();  // 0x006427a0
        if ((flags & 1u) != 0) joan::orig::pool::free_(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x0063e4f0
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Obj::scalar_dtor_642750", 0x00642750, &H::scalar_dtor_642750);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x006427a0
    u32 node_dtor_body() {
        jo::self<jo::ScriptReset>(this)->vtable = img::obj(d::vt_Obj_9dadac);
        return joan::orig::arg::as<u32>(joan::orig::as_script(this)->dtor_63f250());  // 0x0063f250
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Node::dtor_body", 0x006427a0, &H::node_dtor_body);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
namespace {
namespace game_b104_11 {

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::img::str;

struct Obj {};


// x87 compare "equal or unordered" (fcomp + test ah,0x40 jnz)
bool eq_or_nan(float a, float b) { return !(a < b) && !(a > b); }

// Child expression slot 3 (same signature as Expr::compare): thiscall (ctx), ret 4.
u32 eval_child(void* node, std::uintptr_t ctx) {
    using Fn = u32 (Obj::*)(std::uintptr_t);
    JOAN_MEMBER_POINTER_CHECK(Fn);
    Fn method{};
    std::memcpy(&method, &joan::call::vtable_of(node)[3], sizeof(void*));
    return (static_cast<Obj*>(node)->*method)(ctx);
}

JOAN_COUNTER(c_compare, "Expr::compare_644d80");

struct H {
    void* sub14() { return jo::self<jo::ScriptVarDecl>(this)->name_; }

    // ------------------------------------------------------------------ 0x00644d80
    u32 compare(std::uintptr_t ctx) {
        c_compare.hit();
        void* const a = jo::view<void*>(jo::self<jo::Expr>(this)->lhs);
        eval_child(a, ctx);
        void* const b = jo::view<void*>(jo::self<jo::Expr>(this)->rhs);
        eval_child(b, ctx);
        u32 res = 0;
        const i32 type = jo::as_script_node(a)->value_type;
        auto ia = [&]() { return inline_value<i32>(jo::as_script_node(a)->value); };
        auto ib = [&]() { return inline_value<i32>(jo::as_script_node(b)->value); };
        auto fa = [&]() { return inline_value<float>(jo::as_script_node(a)->value); };
        auto fb = [&]() { return inline_value<float>(jo::as_script_node(b)->value); };
        auto sc = [&]() { return std::strcmp(joan::orig::arg::str(jo::view<char*>(jo::as_script_node(a)->value)), joan::orig::arg::str(jo::view<char*>(jo::as_script_node(b)->value))); };  // strcmp
        switch (static_cast<u32>(jo::self<jo::Expr>(this)->op + 0x2d)) {
            case 0:  // <
                if (type == 1) res = ia() < ib();
                else if (type == 2) res = !(fa() >= fb());
                else if (type == 3) res = sc() < 0;
                break;
            case 1:  // >
                if (type == 1) res = ia() > ib();
                else if (type == 2) res = fa() > fb();
                else if (type == 3) res = sc() > 0;
                break;
            case 2:  // <=
                if (type == 1) res = ia() <= ib();
                else if (type == 2) res = !(fa() > fb());
                else if (type == 3) res = sc() <= 0;
                break;
            case 3:  // >=
                if (type == 1) res = ia() >= ib();
                else if (type == 2) res = fa() >= fb();
                else if (type == 3) res = sc() >= 0;
                break;
            case 4:  // ==
                if (type == 1) res = ia() == ib();
                else if (type == 2) res = eq_or_nan(fa(), fb());
                else if (type == 3) res = sc() == 0;
                break;
            case 5:  // !=
                if (type == 1) res = ia() != ib();
                else if (type == 2) res = !eq_or_nan(fa(), fb());
                else if (type == 3) res = sc() != 0;
                break;
            default:
                break;
        }
        jo::self<jo::Expr>(this)->value_type = 1;
        inline_value<u32>(jo::self<jo::Expr>(this)->value) = res;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("Obj::sub14_642f60", 0x00642f60, &H::sub14);
B("Expr::compare_644d80", 0x00644d80, &H::compare);
#undef B

}  // namespace game_b104_11
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::orig::helpers::new_;  // operator new

using joan::call::thiscall;

JOAN_COUNTER(c_make_643550, "Expr::make_643550");

struct H {
    // --------------------------------------------------------------------- 0x00643550
    void* make_643550(i32 a, u32 b) {  // the new node, or null
        c_make_643550.hit();
        u32 r14 = 0;
        u32 r18 = 0;
        if (a >= 0x1d && a <= 0x20) {
            r18 = joan::orig::arg::as<u32>(joan::orig::as_script(this)->add_node());  // 0x0063ef50
        } else {
            if (jo::self<jo::ScriptBlock>(this)->statements == nullptr) {
                void* const mem = new_(static_cast<u32>(sizeof(jo::DynArray)));  // 0x1d on x86
                void* const lst = mem != nullptr ? joan::orig::dynarray(mem)->construct(static_cast<i32>(sizeof(void*)), i32{1}, i32{0}) : nullptr;  // 0x005843d0 (node pointers: 4 on x86)
                jo::self<jo::ScriptBlock>(this)->statements = static_cast<jo::DynArray*>(lst);
            }
            std::uintptr_t item = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_script(this)->statement_6488d0(joan::orig::arg::u32(a), joan::orig::arg::u32(b)));  // 0x006488d0 (a node pointer)
            joan::orig::dynarray(jo::self<jo::ScriptBlock>(this)->statements)->linkin(&item);      // 0x00584620
            r14 = joan::orig::arg::as<u32>(joan::orig::as_dynarray(jo::self<jo::ScriptBlock>(this)->statements)->cursor());  // 0x00417520
        }
        if (r14 != 0) return joan::orig::arg::as<void*>(joan::orig::as_reader(this)->list_get(joan::orig::arg::i32(r14)));  // 0x0063e9f0
        if (r18 != 0) return joan::orig::arg::as<void*>(joan::orig::as_script_block(this)->var_at(joan::orig::arg::i32(r18)));  // 0x0063ef20
        return nullptr;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B108("ObjectParser::parse", 0x00640280, &H::obj_parse);
B108("Expr::make_643550", 0x00643550, &H::make_643550);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b111_05.cpp (game/b111_05)
namespace {
namespace game_b111_05_2 {

using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::str;

using joan::call::thiscall;

// ---- reader / token helpers
u8* peek(void* rd) { return joan::orig::arg::as<u8*>(joan::orig::as_script_lexer(rd)->skip_newlines()); }  // 0x00642370
u8* lex(void* rd) { return joan::orig::arg::as<u8*>(joan::orig::as_script_lexer(rd)->next()); }   // 0x006423e0 (may throw char*)
using joan::orig::helpers::type;
u32 line(const u8* tok) { return jo::view<u32>(jo::as_token(tok)->line); }
u32 err(const void* self, img::Addr fmt, u32 ln) {  // Script::error 0x0063eb80 (cdecl, varargs)
    return cdecl_call<u32>(fn::Script_error, self, str(fmt), ln);
}
std::uintptr_t vparse(void* o, void* rd) {  // virtual parse, vtable slot +0x10, thiscall(reader), ret 4
    return thiscall<std::uintptr_t>(vtable_slot(o, 0x10), o, rd);
}
void* stmt_for(void* self, const u8* tok) {  // 0x00643550 (type, line), ret 8
    const u32 ln = line(tok);
    return joan::orig::arg::as<void*>(joan::orig::as_script_block(self)->make_statement(joan::orig::arg::i32(type(tok)), joan::orig::arg::u32(ln)));
}

JOAN_COUNTER(c_cond, "ScriptCond::parse_6454e0");
JOAN_COUNTER(c_if, "ScriptIf::parse_645a40");
JOAN_COUNTER(c_call, "ScriptCall::parse_643d40");

struct H {
    // ------------------------------------------------------------------ 0x006454e0
    std::uintptr_t cond_sub(jo::ScriptNode* jo::ScriptCond::*slot, u32 kind, void* rd) {
        jo::self<jo::ScriptCond>(this)->*slot = joan::orig::arg::as<jo::ScriptNode*>(joan::orig::as_script_node(this)->create_child(joan::orig::arg::i32(kind)));  // 0x00640db0, ret 4
        return vparse(jo::self<jo::ScriptCond>(this)->*slot, rd);
    }
    // "(" operand ")" (kind 0xe) or a plain operand (kind 0xf); eax = last lexer / vparse result.
    // tok = current token (peeked for the first operand, the lexer result after the operator for the second).
    std::uintptr_t cond_operand(jo::ScriptNode* jo::ScriptCond::*slot, u8* tok, void* rd) {
        if (type(tok) == -0x25) {
            lex(rd);
            cond_sub(slot, 0xe, rd);
            tok = peek(rd);
            if (type(tok) != -0x24) err(this, d::s_Conditions_must_be_ended_wit, line(tok));  // 'Conditions must be ended with )'
            return reinterpret_cast<std::uintptr_t>(lex(rd));
        }
        return cond_sub(slot, 0xf, rd);
    }
    std::uintptr_t parse_cond(void* rd) {
        c_cond.hit();
        cond_operand(&jo::ScriptCond::lhs, peek(rd), rd);
        u8* const tok = peek(rd);
        const i32 t = type(tok);
        if (t >= -0x27 && t <= -0x26) jo::self<jo::ScriptCond>(this)->op = type(tok);
        else jo::self<jo::ScriptCond>(this)->op = 0x22;
        // No operator: the original leaves eax = this (0x6455c8 mov eax,[ebp-4]; jmp 0x645678).
        if (jo::self<jo::ScriptCond>(this)->op == 0x22) return reinterpret_cast<std::uintptr_t>(this);
        return cond_operand(&jo::ScriptCond::rhs, lex(rd), rd);
    }

    // ------------------------------------------------------------------ 0x00645a40
    // Statements up to '}' (tok = first token inside the block, already current).
    void block_stmts(u8* tok, void* rd) {
        while (type(tok) != 5) {
            vparse(stmt_for(this, tok), rd);
            tok = peek(rd);
        }
    }
    std::uintptr_t parse_if(void* rd) {
        c_if.hit();
        u8* tok = peek(rd);
        if (type(tok) != 0x10) err(this, d::s_Expecting_IF, line(tok));  // 'Expecting IF!'
        lex(rd);
        const u32 k = joan::orig::arg::as<u32>(joan::orig::as_script_block(this)->add_child(0x1bu));       // 0x0063ea20, ret 4
        void* const cond = joan::orig::arg::as<void*>(joan::orig::as_reader(this)->list_get(joan::orig::arg::i32(k)));    // 0x0063e9f0, ret 4
        vparse(cond, rd);
        tok = peek(rd);
        if (type(tok) != 4) err(this, d::s_Expecting_for_the_IF, line(tok));  // 'Expecting { for the IF!'
        tok = lex(rd);
        block_stmts(tok, rd);
        tok = lex(rd);
        std::uintptr_t eax = reinterpret_cast<std::uintptr_t>(tok);
        if (type(tok) == 0x11) {
            stmt_for(this, tok);  // ELSE statement object, result unused
            tok = lex(rd);
            if (type(tok) == 0x10) {
                eax = vparse(stmt_for(this, tok), rd);
            } else if (type(tok) == 4) {
                tok = lex(rd);
                block_stmts(tok, rd);
                eax = reinterpret_cast<std::uintptr_t>(lex(rd));
            } else {
                eax = err(this, d::s_Expecting_or_if_for_the_else, line(tok));  // 'Expecting { or if for the else!'
            }
        }
        if (jo::self<jo::ScriptIf>(this)->statements != nullptr) eax = joan::orig::arg::as<u32>(joan::orig::as_counter(jo::self<jo::ScriptIf>(this)->statements)->clamp_to_one());  // 0x004b9d40
        return eax;
    }

    // ------------------------------------------------------------------ 0x00643d40
    std::uintptr_t parse_call(void* rd) {
        c_call.hit();
        u32 eax = joan::orig::arg::as<u32>(joan::orig::as_script_call(this)->parse(joan::orig::arg::mptr(rd)));  // 0x00643c00, ret 4
        const i32 id = jo::self<jo::ScriptCall>(this)->func_id;
        std::uintptr_t conv = 0;
        if (id == 0x143) conv = fn::code_value(fn::Action_from_name);                     // 0x004bbbc0
        else if (id > 0x145 && id <= 0x147) conv = fn::code_value(fn::ai_task_from_name);  // 0x004e4610
        if (conv != 0) {
            void* const a = joan::orig::arg::as<void*>(joan::orig::as_reader(this)->list_get(2));  // 0x0063e9f0
            thiscall<u32>(vtable_slot(a, 0xc), a, 0u);       // virtual slot +0xc, ret 4
            void* const b = joan::orig::arg::as<void*>(joan::orig::as_reader(this)->list_get(2));
            eax = cdecl_call<u32>(conv, jo::as_script_node(b)->value);
            jo::view<u16>(jo::self<jo::ScriptCall>(this)->arg_id) = static_cast<u16>(eax);
        }
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_05", name, addr, fn)
B("ScriptCond::parse_6454e0", 0x006454e0, &H::parse_cond);
B("ScriptIf::parse_645a40", 0x00645a40, &H::parse_if);
B("ScriptCall::parse_643d40", 0x00643d40, &H::parse_call);
#undef B

}  // namespace game_b111_05_2
}  // namespace

// ---------------------------------------- from modules/game_b115_10.cpp (fix/uninit)
namespace {
namespace game_b115_10 {

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::str;

using joan::call::thiscall;

JOAN_COUNTER(c_operand, "ScriptExpr::parse_operand_644b10");

struct H {
    // ===================================================================== 0x00644b10
    std::uintptr_t parse_operand(void* rd) {
        c_operand.hit();
        u8* const tok = joan::orig::arg::as<u8*>(joan::orig::as_script_lexer(rd)->skip_newlines());  // 0x00642370
        const u32 sel = static_cast<u32>(jo::as_token(tok)->kind + 0x25);
        i32 kind = 0;
        bool have_kind = true;
        if (sel > 0x46) {
            have_kind = false;
        } else {
            switch (static_cast<i32>(sel) - 0x25) {
            case -0x25: kind = 0x1b; break;
            case 1: case 2: case 3: case 0x1b: kind = 0x13; break;
            case 0x17: case 0x18: case 0x19: case 0x1a: kind = 0x14; break;
            case 0x1c: kind = 0x1a; break;
            case 0x21: kind = 0x11; break;
            default: have_kind = false; break;
            }
        }
        if (!have_kind)
            joan::orig::script::error(this, str(d::s_Expecting_script_function_va), joan::orig::arg::i32(jo::view<u32>(jo::as_token(tok)->line)));  // 0x0063eb80
        if (jo::as_token(tok)->kind == 0x21) {
            jo::ScriptNode* const v = thiscall<jo::ScriptNode*>(vtable_slot(this, 0x18), this, jo::as_token(tok)->text);
            jo::self<jo::ScriptExpr>(this)->lhs = v;
            if (jo::self<jo::ScriptExpr>(this)->lhs == nullptr)
                joan::orig::script::error(this, str(d::s_Variable_not_found_s), joan::orig::arg::i32(jo::view<u32>(jo::as_token(tok)->line)), jo::as_token(tok)->text);  // 0x0063eb80
            return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_script_lexer(rd)->next());  // 0x006423e0
        }
        if (!have_kind) return 0;  // fix: kind [ebp-0xc] unset -> node creation skipped
        jo::self<jo::ScriptExpr>(this)->rhs = joan::orig::arg::as<jo::ScriptNode*>(joan::orig::as_script_node(this)->create_child(joan::orig::arg::i32(kind)));  // 0x00640db0
        void* const node = jo::self<jo::ScriptExpr>(this)->rhs;
        return thiscall<std::uintptr_t>(vtable_slot(node, 0x10), node, rd);
    }
};

// 0x00644b10: kind unset after the (returning) error call -> node creation skipped, eax 0.
JOAN_REPLACE("fix/uninit", "ScriptExpr::parse_operand_644b10", 0x00644b10, &H::parse_operand);

}  // namespace game_b115_10
}  // namespace

// ---------------------------------------- from modules/game_b101_13.cpp (game/b101_13)
namespace {
namespace game_b101_13 {

using u8 = std::uint8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::img::obj;
using joan::img::str;
using joan::hook::global;

struct Obj {};

using joan::call::thiscall;
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

struct Vec3 {
    u32 x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_run_block, "ScriptNode::run_block");

struct H {
    // 0x0063e9f0: n-th parameter node; its value is the dword at +0x10.
    u8* param(i32 n) { return joan::orig::arg::as<u8*>(joan::orig::as_reader(this)->list_get(joan::orig::arg::i32(n))); }

    // ------------------------------------------------------------------ 0x006456f0
    i32 run_block(std::uintptr_t arg) {
        c_run_block.hit();
        u8* const first = param(1);
        u8* it = joan::orig::arg::as<u8*>(joan::orig::as_table(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->last());  // 0x00585f20
        u8 flag = 1;
        if (first == it) {
            vcall<i32>(it, 0xc, arg);
            const i32 type = jo::as_script_node(it)->value_type;
            if (type == 1) {
                flag = inline_value<i32>(jo::as_script_node(it)->value) != 0 ? 1 : 0;
            } else if (type == 2) {
                const float v = inline_value<float>(jo::as_script_node(it)->value);
                const float zero = global<float>(d::k_9cf5c4);
                flag = (v < zero || v > zero) ? 1 : 0;  // jne on C3: equal / unordered -> 0
            } else if (type == 3) {
                u8 buf[0x194];
                joan::orig::as_string(buf)->construct(joan::orig::arg::str(jo::as_script_node(it)->value));  // 0x005a11a0
                flag = joan::orig::arg::as<i32>(joan::orig::as_name(buf)->length()) > 0 ? 1 : 0;      // 0x0045e110
            } else if (type == 0x1b) {
                Vec3 v = *jo::view<const Vec3*>(jo::as_script_node(it)->value);
                const u8 r = joan::orig::arg::as<u8>(joan::orig::vec_equal(&v, joan::orig::arg::ptr(obj(d::g_pos))));  // 0x0060ced0
                flag = r == 0 ? 1 : 0;
            } else {
                return -2;
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_reader(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->next()) == 0) return 1;  // 0x0063ee20
            it = joan::orig::arg::as<u8*>(joan::orig::as_table(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->last());
            if (flag != 0) {
                if (jo::as_script_node(it)->kind == 0xd) {
                    joan::orig::as_counter(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->clamp_to_one();  // 0x004b9d40
                    return 1;
                }
            } else {
                while (jo::as_script_node(it)->kind != 0xd) {
                    if (joan::orig::arg::as<i32>(joan::orig::as_reader(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->next()) == 0) {
                        joan::orig::as_counter(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->clamp_to_one();
                        return 1;
                    }
                    it = joan::orig::arg::as<u8*>(joan::orig::as_table(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->last());
                }
                if (joan::orig::arg::as<i32>(joan::orig::as_reader(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->next()) == 0) {
                    joan::orig::as_counter(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->clamp_to_one();
                    return 1;
                }
                it = joan::orig::arg::as<u8*>(joan::orig::as_table(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->last());
            }
        }
        while (jo::as_script_node(it)->kind != 0xd) {
            const i32 r = vcall<i32>(it, 0xc, arg);
            if (r == 0) return r;
            if (r == -1 || r == 2) {
                joan::orig::as_counter(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->clamp_to_one();
                return r;
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_reader(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->next()) == 0) break;
            it = joan::orig::arg::as<u8*>(joan::orig::as_table(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->last());
        }
        joan::orig::as_counter(jo::view<void*>(jo::self<jo::ScriptIf>(this)->statements))->clamp_to_one();
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_13", name, addr, fn)
B("ScriptNode::run_block", 0x006456f0, &H::run_block);
#undef B

}  // namespace game_b101_13
}  // namespace

// ---------------------------------------- from modules/game_b94_06_2.cpp (game/b94_06)
namespace {
namespace game_b94_06_2 {

using joan::call::thiscall;

using u32 = std::uint32_t;


struct H {
    // 0x00646290
    void* node_ctor_646290() {
        joan::orig::as_script_block(this)->construct();  // 0x0063e6e0
        jo::view<std::uintptr_t>(jo::self<jo::MathExpr>(this)->vtable) = img::addr(d::vt_Node_9db578);
        jo::view<std::uint32_t>(jo::self<jo::MathExpr>(this)->kind) = 0x1b;
        jo::self<jo::MathExpr>(this)->rpn = nullptr;
        return this;
    }

    // 0x00648ea0
    void* zero2() {
        jo::self<jo::Stack>(this)->top = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::Stack>(this)->count) = 0;
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Node::ctor_646290", 0x00646290, &H::node_ctor_646290);
B94("Obj::zero2", 0x00648ea0, &H::zero2);
#undef B94

}  // namespace game_b94_06_2
}  // namespace

// ---------------------------------------- from modules/game_b103_13_2.cpp (game/b103_13)
namespace {
namespace game_b103_13_2 {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

void node_delete(void* p) { joan::orig::pool::free_(joan::orig::arg::mptr(p)); }  // 0x0063e4f0

struct H {
    // Scalar deleting destructor: dtor (via its thunk), operator delete when flags & 1.
    template <fn::Code Dtor>
    H* sdtor(u32 flags) {
        thiscall(Dtor, this);
        if ((flags & 1) != 0) node_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_13", name, addr, fn)
B("Node::sdtor_6462e0", 0x006462e0, &H::sdtor<fn::Node_dtor_Node_646330>);  // 0x00646330
B("Node::sdtor_6487c0", 0x006487c0, &H::sdtor<fn::Widget_dtor_648810>);  // 0x00648810
#undef B

}  // namespace game_b103_13_2
}  // namespace

// ---------------------------------------- from modules/game_b99_08_2.cpp (game/b99_08)
namespace {
namespace game_b99_08_2 {

using joan::call::thiscall;

using u32 = std::uint32_t;


JOAN_COUNTER(c_dtor_646330, "Node::~Node_646330");

struct H {
    // ------------------------------------------------------------------ 0x00646330
    u32 dtor_646330() {
        c_dtor_646330.hit();
        jo::self<jo::MathExpr>(this)->vtable = img::obj(d::vt_Node_9db578);
        if (jo::view<void*>(jo::self<jo::MathExpr>(this)->rpn) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::MathExpr>(this)->rpn);
            if (p != nullptr) joan::orig::as_dynarray(p)->destroy(1u);  // 0x00463fc0 scalar deleting dtor
            jo::view<void*>(jo::self<jo::MathExpr>(this)->rpn) = nullptr;
        }
        return joan::orig::arg::as<u32>(joan::orig::as_script_block(this)->destruct());  // 0x0063e780
    }

    // ------------------------------------------------------------------ 0x00648b50
    u32 call_648b80() { joan::orig::as_list(this)->clear_648b80(); return 0; }  // 0x00648b80 (void; the loader table ignores the result)
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_08", name, addr, fn)
B99("Node::~Node_646330", 0x00646330, &H::dtor_646330);
B99("Obj::call_648b80", 0x00648b50, &H::call_648b80);
#undef B99

}  // namespace game_b99_08_2
}  // namespace

// ---------------------------------------- from modules/game_b115_11.cpp (fix/uninit)
namespace {
namespace game_b115_11 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::img::str;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;

const Fpu fpu{true};

JOAN_COUNTER(c_expr, "Expression::evaluate_6463f0 (fix/uninit)");

// x87 compare with 0.0 + `test ah,0x40`: equal or unordered.
bool eqz(float v) { return !(v < 0.0f) && !(v > 0.0f); }

// ============================================================ 0x006463f0 helpers
// Emulated bytes [ebp - 0x19cc, ebp) of Expression::evaluate (x86 offsets): vectors[15] (-0x19cc), strings[15]
// x 0x191 (-0x1918), n (-0x198), markers[16] (-0x194), values[16] x 0x14 (-0x154), 0xf (-0x14),
// this (-0x10), SEH slots (-0xc .. -4). The offsets are derived from the value (ScriptNode) and pointer sizes,
// so the same frame holds the x64 objects (values[16] x 0x20, pointer-sized this / SEH slots); on x86 they are
// the original's constants (static_assert below).
constexpr std::int64_t kPtr = static_cast<std::int64_t>(sizeof(void*));
constexpr std::int64_t kNode = static_cast<std::int64_t>(sizeof(jo::ScriptNode));  // a value (0x00643300): 0x14 on x86
constexpr std::int64_t kStr = 0x191;                                                // a String (0x005a1170)
constexpr std::int64_t kTail = 4 + kPtr + 2 * kPtr + 4;  // 0xf, this, SEH next / handler, state
constexpr std::int64_t kOffF = -kTail;                   // the 0xf dword
constexpr std::int64_t kOffThis = kOffF + 4;             // this
constexpr std::int64_t kOffHandler = -4 - kPtr;          // SEH handler
constexpr std::int64_t kOffState = -4;                   // SEH state
constexpr std::int64_t kOffVals = -kTail - 16 * kNode;   // values[16]
constexpr std::int64_t kOffMarkers = kOffVals - 16 * 4;  // markers[16]
constexpr std::int64_t kOffN = kOffMarkers - 4;          // n
constexpr std::int64_t kOffStrings = kOffN - 0x1780;     // strings[15] (the original's 15 x 0x191 overlap n by 0xf bytes)
constexpr std::int64_t kEvalLow = kOffStrings - 15 * 0xc;  // vectors[15]
static_assert(sizeof(void*) != 4 || (kNode == 0x14 && kOffF == -0x14 && kOffThis == -0x10 && kOffHandler == -8 &&
                                     kOffVals == -0x154 && kOffMarkers == -0x194 && kOffN == -0x198 &&
                                     kOffStrings == -0x1918 && kEvalLow == -0x19cc),
              "x86 frame of 0x006463f0");
struct alignas(alignof(void*)) EvalFrame {
    u8 b[-kEvalLow];
    alignas(alignof(void*)) u8 dummy[kNode];  // a value slot outside the frame (malformed RPN only)
    u8* at_off(std::int64_t off, std::int64_t size) {
        if (off >= kEvalLow && off + size <= 0) return b + (off - kEvalLow);
        return nullptr;
    }
    i32 rd(std::int64_t off) {
        u8* const p = at_off(off, 4);
        return p != nullptr ? rd32(p) : 0;
    }
    void wr(std::int64_t off, i32 v) {
        u8* const p = at_off(off, 4);
        if (p != nullptr) std::memcpy(p, &v, 4);
    }
    // A pointer-sized slot (this / the SEH handler).
    void* rd_ptr(std::int64_t off) {
        void* v = nullptr;
        if (u8* const p = at_off(off, kPtr)) std::memcpy(&v, p, sizeof v);
        return v;
    }
    void wr_ptr(std::int64_t off, std::uintptr_t v) {
        if (u8* const p = at_off(off, kPtr)) std::memcpy(p, &v, sizeof v);
    }
    static i32 rd32(const u8* p) {
        i32 v = 0;
        std::memcpy(&v, p, 4);
        return v;
    }
    i32 n() { return rd(kOffN); }
    void set_n(i32 v) { wr(kOffN, v); }
    i32 marker(i32 k) { return rd(kOffMarkers + static_cast<std::int64_t>(k) * 4); }
    u8* slot(i32 k) {
        u8* const p = at_off(kOffVals + static_cast<std::int64_t>(k) * kNode, kNode);
        if (p != nullptr) return p;
        std::memset(dummy, 0, sizeof dummy);
        jo::as_script_node(dummy)->kind = 0x13;  // no virtual evaluate on it
        return dummy;
    }
};
// Value (0x14 bytes on x86): +0 vtable, +8 kind (0x13 = plain value), +0xc type (1 int, 2 float,
// 3 string*, 0x1b vec3*), +0x10 value.
i32 vt_evaluate(u8* node, void* ctx) {
    void* const* const vt = joan::call::vtable_of(node);
    return thiscall<i32>(reinterpret_cast<std::uintptr_t>(vt[3]), node, ctx);
}
// crt::eh_vector_constructor_iterator / eh_vector_destructor_iterator (0x007b2f98 / 0x007b3054, lib/crt/mem_intrinsics.cpp)
using EhVec = void(JOAN_STDCALL*)(void*, u32, i32, std::uintptr_t, std::uintptr_t);
using EhVecDtor = void(JOAN_STDCALL*)(void*, u32, i32, std::uintptr_t);
struct ValuesGuard {  // the original's unwind action for state 0: eh vector dtor of the values
    u8* vals;
    bool armed = true;
    ~ValuesGuard() {
        if (armed) reinterpret_cast<EhVecDtor>(fn::code_value(fn::crt_eh_vector_destructor_iterator))(vals, static_cast<u32>(kNode), 0x10, fn::code_value(fn::Obj_dtor_Obj_643390));
    }
};

struct H {
    // ======================================================== 0x006463f0
    // Expression::evaluate (vtable 0x009db578 slot 3): RPN over the token list +0x20 (positive
    // = node id, negative = operator -0x32 .. -0x26), result stored in +0xc (type) / +0x10.
    i32 expr_evaluate(void* ctx) {
        c_expr.hit();
        EvalFrame F;
        std::memset(F.b, 0, sizeof F.b);
        F.wr(kOffF, 0xf);
        F.wr_ptr(kOffThis, reinterpret_cast<std::uintptr_t>(this));
        F.wr_ptr(kOffHandler, fn::code_value(fn::fn_7e20c4));
        F.wr(kOffState, -1);
        u8* const vals = F.at_off(kOffVals, 16 * kNode);
        reinterpret_cast<EhVec>(fn::code_value(fn::crt_eh_vector_constructor_iterator))(vals, static_cast<u32>(kNode), 0x10, fn::code_value(fn::Node_Node_643300), fn::code_value(fn::Obj_dtor_Obj_643390));  // 0x00643300 / 0x00643390
        F.wr(kOffState, 0);
        ValuesGuard guard{vals};
        auto finish = [&F, &guard, vals](i32 r) {
            F.wr(kOffState, -1);
            guard.armed = false;
            reinterpret_cast<EhVecDtor>(fn::code_value(fn::crt_eh_vector_destructor_iterator))(vals, static_cast<u32>(kNode), 0x10, fn::code_value(fn::Obj_dtor_Obj_643390));
            return r;
        };
        auto self = [&F]() { return static_cast<u8*>(F.rd_ptr(kOffThis)); };
        auto node_of = [&self](i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_reader(self())->list_get(joan::orig::arg::i32(id))); };  // 0x0063e9f0
        F.set_n(0);
        // strings[15] (0x191 bytes, ctor 0x005a1170) through the game's vector ctor 0x00449af0.
        reinterpret_cast<void(JOAN_STDCALL*)(void*, i32, i32, void*)>(fn::code_value(fn::vector_constructor_iterator))(F.at_off(kOffStrings, 15 * kStr), 0x191,
                                                                            0xf, fn::code_ptr(fn::String));
        const i32 count = joan::orig::as_dynarray(jo::as_math_expr(self())->rpn)->size();  // 0x00417ad0
        for (i32 i = 1; i <= count; ++i) {
            const i32 id = *joan::orig::arg::as<i32*>(joan::orig::as_dynarray(jo::as_math_expr(self())->rpn)->get(joan::orig::arg::i32(i)));  // 0x00417200
            if (id > 0) {
                if (count != 1) {
                    F.wr(kOffMarkers + static_cast<std::int64_t>(F.n()) * 4, id);
                    F.set_n(F.n() + 1);
                    continue;
                }
                u8* const node = node_of(id);
                const i32 r = vt_evaluate(node, ctx);
                if (r == 0) continue;
                if (r == -1) return finish(-1);
                const i32 k = F.n();
                u8* const s = F.slot(k);
                F.set_n(F.n() + 1);
                jo::as_script_node(s)->value_type = jo::as_script_node(node)->value_type;
                const i32 type = jo::as_script_node(node)->value_type;
                if (type == 1 || type == 2) {
                    jo::as_script_node(s)->value = jo::as_script_node(node)->value;
                } else if (type == 3) {
                    u8* const str = F.at_off(kOffStrings + static_cast<std::int64_t>(k) * kStr, kStr);
                    joan::orig::as_string(str)->assign(joan::orig::arg::str(jo::as_script_node(node)->value));  // 0x005a1440
                    jo::as_script_node(s)->value = str;
                } else if (type == 0x1b) {
                    u8* const v = F.at_off(kEvalLow + static_cast<std::int64_t>(k) * 0xc, 0xc);
                    std::memcpy(v, jo::as_script_node(node)->value, 0xc);
                    jo::as_script_node(s)->value = v;
                } else {
                    return finish(-2);
                }
                continue;
            }
            if (id == 0) continue;
            // ---- binary operator
            F.set_n(F.n() - 1);
            u8* const right = F.marker(F.n()) > 0 ? node_of(F.marker(F.n())) : F.slot(F.n());  // [ebp-0x19f4]
            F.set_n(F.n() - 1);
            u8* const left = F.marker(F.n()) > 0 ? node_of(F.marker(F.n())) : F.slot(F.n());  // [ebp-0x19f8]
            u8* const res = F.slot(F.n());                                                  // [ebp-0x19f0]
            if (jo::as_script_node(left)->kind != 0x13) vt_evaluate(left, ctx);
            if (id != -0x27) {
                if (jo::as_script_node(right)->kind != 0x13) vt_evaluate(right, ctx);
            } else {
                if (jo::as_script_node(left)->value_type == 1 && inline_value<i32>(jo::as_script_node(left)->value) != 0 && jo::as_script_node(right)->kind != 0x13)
                    vt_evaluate(right, ctx);
                if (jo::as_script_node(left)->value_type == 2 && !eqz(inline_value<float>(jo::as_script_node(left)->value)) && jo::as_script_node(right)->kind != 0x13)
                    vt_evaluate(right, ctx);
            }
            const u32 op = static_cast<u32>(id + 0x32);
            const i32 lt = jo::as_script_node(left)->value_type;
            const i32 rt = jo::as_script_node(right)->value_type;
            if (lt == rt && lt == 1) {
                const i32 a = inline_value<i32>(jo::as_script_node(left)->value);
                const i32 b = inline_value<i32>(jo::as_script_node(right)->value);
                if (op > 0xc) return finish(-2);
                jo::as_script_node(res)->value_type = op == 3 ? 2 : 1;
                i32 v = 0;
                switch (op) {
                    case 0: v = static_cast<i32>(static_cast<u32>(a) + static_cast<u32>(b)); break;
                    case 1: v = static_cast<i32>(static_cast<u32>(a) - static_cast<u32>(b)); break;
                    case 2: v = static_cast<i32>(static_cast<u32>(a) * static_cast<u32>(b)); break;
                    case 3: {
                        const float q = static_cast<float>(fpu.div(static_cast<double>(a), static_cast<double>(b)));
                        std::memcpy(&v, &q, 4);
                        break;
                    }
                    case 4:
                        v = ftol(joan::orig::arg::as<double>(joan::orig::libc::pow(joan::orig::arg::f64(static_cast<double>(a)), joan::orig::arg::f64(static_cast<double>(b)))));  // pow
                        break;
                    case 5: v = a < b ? 1 : 0; break;
                    case 6: v = a > b ? 1 : 0; break;
                    case 7: v = a <= b ? 1 : 0; break;
                    case 8: v = a >= b ? 1 : 0; break;
                    case 9: v = a == b ? 1 : 0; break;
                    case 10: v = a != b ? 1 : 0; break;
                    case 11: v = (a != 0 && b != 0) ? 1 : 0; break;
                    default: v = (a == 0 && b == 0) ? 0 : 1; break;
                }
                inline_value<i32>(jo::as_script_node(res)->value) = v;
            } else if ((lt == 2 && rt == 2) || (lt == 1 && rt == 2) || (lt == 2 && rt == 1)) {
                float fa = 0.0f, fb = 0.0f;  // [ebp-0x1a08] / [ebp-0x1a0c]
                if (lt == 2) {
                    fa = inline_value<float>(jo::as_script_node(left)->value);
                } else if (lt == 1) {
                    fa = static_cast<float>(inline_value<i32>(jo::as_script_node(left)->value));
                } else {
                    return finish(-2);
                }
                if (rt == 2) {
                    fb = inline_value<float>(jo::as_script_node(right)->value);
                } else if (rt == 1) {
                    fb = static_cast<float>(inline_value<i32>(jo::as_script_node(right)->value));
                } else {
                    return finish(-2);
                }
                if (op > 0xc) return finish(-2);
                const bool fop = op <= 4;
                jo::as_script_node(res)->value_type = fop ? 2 : 1;
                const double da = fa, db = fb;
                float fv = 0.0f;
                i32 v = 0;
                switch (op) {
                    case 0: fv = static_cast<float>(fpu.add(da, db)); break;
                    case 1: fv = static_cast<float>(fpu.sub(da, db)); break;
                    case 2: fv = static_cast<float>(fpu.mul(da, db)); break;
                    case 3: fv = static_cast<float>(fpu.div(da, db)); break;
                    case 4: fv = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::pow(joan::orig::arg::f64(da), joan::orig::arg::f64(db)))); break;  // pow
                    case 5: v = !(fa >= fb) ? 1 : 0; break;   // test ah,1: below or unordered
                    case 6: v = fa > fb ? 1 : 0; break;       // test ah,0x41 == 0
                    case 7: v = !(fa > fb) ? 1 : 0; break;
                    case 8: v = fa >= fb ? 1 : 0; break;
                    case 9: v = !(fa < fb) && !(fa > fb) ? 1 : 0; break;  // equal or unordered
                    case 10: v = (fa < fb || fa > fb) ? 1 : 0; break;
                    case 11: v = (!eqz(fa) && !eqz(fb)) ? 1 : 0; break;
                    default: v = (!eqz(fa) || !eqz(fb)) ? 1 : 0; break;
                }
                if (fop) {
                    std::memcpy(&inline_value<i32>(jo::as_script_node(res)->value), &fv, 4);
                } else {
                    inline_value<i32>(jo::as_script_node(res)->value) = v;
                }
            } else if (lt == 3 && rt == 3) {
                void* const sa = jo::as_script_node(left)->value;
                void* const sb = jo::as_script_node(right)->value;
                if (op > 0xa || (op >= 1 && op <= 4)) return finish(-2);
                if (op == 0) {
                    jo::as_script_node(res)->value_type = 3;
                    joan::orig::as_string(jo::as_script_node(res)->value)->append_str(joan::orig::as_string(sb));  // 0x005a14f0
                } else {
                    jo::as_script_node(res)->value_type = 1;
                    const char* const cb = joan::orig::arg::as<const char*>(joan::orig::as_string(sb)->c_str());  // 0x00417d90
                    const char* const ca = joan::orig::arg::as<const char*>(joan::orig::as_string(sa)->c_str());
                    const i32 c = std::strcmp(joan::orig::arg::str(ca), joan::orig::arg::str(cb));  // strcmp
                    i32 v = 0;
                    switch (op) {
                        case 5: v = c < 0 ? 1 : 0; break;
                        case 6: v = c > 0 ? 1 : 0; break;
                        case 7: v = c <= 0 ? 1 : 0; break;
                        case 8: v = c >= 0 ? 1 : 0; break;
                        case 9: v = c == 0 ? 1 : 0; break;
                        default: v = c != 0 ? 1 : 0; break;
                    }
                    inline_value<i32>(jo::as_script_node(res)->value) = v;
                }
            } else if (lt == rt && lt == 0x1b) {
                void* const va = jo::as_script_node(left)->value;
                void* const vb = jo::as_script_node(right)->value;
                if (op == 0) {
                    jo::as_script_node(res)->value_type = 0x1b;
                    joan::orig::vec_add_in_place(va, vb);  // 0x0060cdc0
                } else if (op == 1) {
                    jo::as_script_node(res)->value_type = 0x1b;
                    joan::orig::vec3_sub_assign(va, vb);  // 0x0060ce20
                } else if (op == 9) {
                    jo::as_script_node(res)->value_type = 1;
                    inline_value<i32>(jo::as_script_node(res)->value) = static_cast<i32>(joan::orig::arg::as<u32>(joan::orig::vec_equal(va, vb)) & 0xffu);  // 0x0060ced0
                } else if (op == 10) {
                    jo::as_script_node(res)->value_type = 1;
                    inline_value<i32>(jo::as_script_node(res)->value) = static_cast<i32>(joan::orig::arg::as<u32>(joan::orig::vector_differs(va, vb)) & 0xffu);  // 0x0060cf40
                } else {
                    return finish(-2);
                }
            } else if (id == -0x27) {
                jo::as_script_node(res)->value_type = 1;
                inline_value<i32>(jo::as_script_node(res)->value) = 0;
            } else {
                return finish(-2);
            }
            F.set_n(F.n() + 1);
            F.wr(kOffN + static_cast<std::int64_t>(F.n()) * 4, -1);  // marker[n - 1] = value (kOffN = marker[-1])
        }
        // fix/uninit: an empty value stack (no tokens, or the single node answered 0 = "not
        // ready") makes the original read values[-1] (never-written marker words [ebp-0x15c] /
        // [ebp-0x158]) as the result. Here it returns 0 ("not ready", as 0x0063f790 propagates a
        // child's 0) without touching +0xc / +0x10.
        if (F.n() == 0) return finish(0);
        F.set_n(F.n() - 1);
        u8* const res = F.slot(F.n());  // [ebp-0x1a20]
        jo::as_script_node(self())->value_type = jo::as_script_node(res)->value_type;
        const i32 type = jo::as_script_node(res)->value_type;
        if (type == 1 || type == 2) {
            jo::as_script_node(self())->value = jo::as_script_node(res)->value;
        } else if (type == 3) {
            if (jo::as_script_node(self())->value == nullptr) {
                void* const mem = joan::crt::operator_new(0x191u);  // operator new
                F.b[sizeof F.b - static_cast<std::size_t>(-kOffState)] = 1;  // state byte 1 ([ebp-4])
                void* const str = mem != nullptr ? joan::orig::as_string(mem)->construct() : nullptr;  // 0x005a1170
                F.b[sizeof F.b - static_cast<std::size_t>(-kOffState)] = 0;
                jo::view<void*>(jo::as_script_node(self())->value) = str;
            }
            joan::orig::as_string(jo::as_script_node(self())->value)->assign(joan::orig::as_string(jo::as_script_node(res)->value));  // 0x005a1400
        } else if (type == 0x1b) {
            if (jo::as_script_node(self())->value == nullptr) jo::view<void*>(jo::as_script_node(self())->value) = joan::crt::operator_new(0xcu);
            std::memcpy(jo::as_script_node(self())->value, jo::as_script_node(res)->value, 0xc);
        } else {
            return finish(-2);
        }
        return finish(1);
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: empty value stack (vals[-1] read) -> returns 0 ("not ready").
F("Expression::evaluate_6463f0", 0x006463f0, &H::expr_evaluate);
#undef F

}  // namespace game_b115_11
}  // namespace

// ---------------------------------------- from modules/game_b111_05_3.cpp (game/b111_05)
namespace {
namespace game_b111_05_3 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::img::str;

using i8 = std::int8_t;


// ---- reader / token helpers
u8* peek(void* rd) { return joan::orig::arg::as<u8*>(joan::orig::as_script_lexer(rd)->skip_newlines()); }  // 0x00642370
u8* lex(void* rd) { return joan::orig::arg::as<u8*>(joan::orig::as_script_lexer(rd)->next()); }   // 0x006423e0 (may throw char*)
using joan::orig::helpers::type;
u32 line(const u8* tok) { return jo::view<u32>(jo::as_token(tok)->line); }
void err(const void* self, img::Addr fmt, u32 ln) {  // Script::error 0x0063eb80 (cdecl, varargs)
    joan::orig::script::error(joan::orig::arg::mptr(self), str(fmt), joan::orig::arg::i32(ln));  // 0x0063eb80 (callers drop the eax)
}
std::uintptr_t vparse(void* o, void* rd) {  // virtual parse, vtable slot +0x10, thiscall(reader), ret 4
    return thiscall<std::uintptr_t>(vtable_slot(o, 0x10), o, rd);
}
void* stmt_for(void* self, const u8* tok) {  // 0x00643550 (type, line), ret 8
    const u32 ln = line(tok);
    return joan::orig::arg::as<void*>(joan::orig::as_script_block(self)->make_statement(joan::orig::arg::i32(type(tok)), joan::orig::arg::u32(ln)));
}

// operator delete of a new'd block while its ctor runs (unwind-map action of the originals).
struct DeleteOnUnwind {
    void* p;
    std::uintptr_t deleter;
    bool armed;
    DeleteOnUnwind(void* mem, std::uintptr_t fn) : p(mem), deleter(fn), armed(true) {}
    DeleteOnUnwind(const DeleteOnUnwind&) = delete;
    DeleteOnUnwind& operator=(const DeleteOnUnwind&) = delete;
    ~DeleteOnUnwind() noexcept(false) {
        if (armed) cdecl_call<void>(deleter, p);
    }
};

// Dynamic array [ebp-0x30] of 0x00647c90 (0x1d bytes used, 0x20-byte slot). release() = the original's
// normal-path "state -1; dtor" and returns the dtor's eax; the destructor runs only on unwind.
struct DynArr {
    alignas(alignof(void*)) u8 raw[sizeof(joan::orig::DynArray) > 0x20 ? sizeof(joan::orig::DynArray) : 0x20];  // 0x21 on x64
    bool armed;
    DynArr(u32 a, u32 b, u32 c) : armed(false) {
        joan::orig::dynarray(raw)->construct(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c));  // 0x005843d0
        armed = true;
    }
    DynArr(const DynArr&) = delete;
    DynArr& operator=(const DynArr&) = delete;
    u32 release() {
        armed = false;
        return thiscall<u32>(fn::dtor_DynArray, raw);  // 0x005844b0
    }
    ~DynArr() noexcept(false) {
        if (armed) joan::orig::dynarray(raw)->destruct();
    }
};

// Operator stack [ebp-0x58] of 0x00647c90 (8 bytes).
struct OpStack {
    alignas(alignof(void*)) u8 raw[sizeof(jo::Stack)];  // a Stack (top, count): 8 bytes on x86, 0xc on x64
    bool armed;
    OpStack() : armed(false) {
        joan::orig::as_linked_stack(raw)->construct();  // 0x00648ea0
        armed = true;
    }
    OpStack(const OpStack&) = delete;
    OpStack& operator=(const OpStack&) = delete;
    void release() {
        armed = false;
        joan::orig::as_linked_stack(raw)->free_nodes();  // 0x00648ee0
    }
    ~OpStack() noexcept(false) {
        if (armed) joan::orig::as_linked_stack(raw)->free_nodes();
    }
    i32 count() { return joan::orig::arg::as<i32>(joan::orig::as_linked_stack(raw)->count()); }  // 0x00648f40
    i32 top() { return joan::orig::arg::as<i32>(joan::orig::as_linked_stack(raw)->top()); }    // 0x00649040
    i32 pop() { return joan::orig::arg::as<i32>(joan::orig::as_linked_stack(raw)->pop()); }    // 0x00648fd0
    void push(i32* v) { joan::orig::as_linked_stack(raw)->push(joan::orig::arg::mptr(v)); }  // 0x00648f60, ret 4
};

void arr_push(void* arr, void* elem) { joan::orig::dynarray(arr)->linkin(joan::orig::arg::ptr(elem)); }  // 0x00584620, ret 4
i32 arr_count(void* arr) { return joan::orig::as_dynarray(arr)->size(); }  // 0x00417ad0
i32 arr_get(void* arr, i32 i) { return *joan::orig::arg::as<i32*>(joan::orig::as_dynarray(arr)->get(joan::orig::arg::i32(i))); }  // 0x00417200, ret 4

JOAN_COUNTER(c_math, "MathExpr::parse_647c90");
JOAN_COUNTER(c_else, "ScriptElse::parse_648630");

struct H {
    // ------------------------------------------------------------------ 0x00645a40
    // Statements up to '}' (tok = first token inside the block, already current).
    void block_stmts(u8* tok, void* rd) {
        while (type(tok) != 5) {
            vparse(stmt_for(this, tok), rd);
            tok = peek(rd);
        }
    }

    // ------------------------------------------------------------------ 0x00647c90
    void* out() { return jo::self<jo::MathExpr>(this)->rpn; }
    i32 prec(i32 op) { return joan::orig::arg::as<i32>(joan::orig::as_math_expr(this)->op_precedence(joan::orig::arg::i32(op))); }  // 0x006484b0, ret 4
    std::uintptr_t parse_math(void* rd) {
        c_math.hit();
        DynArr list(4u, 0x14u, 0u);  // state 0
        u8* tok = peek(rd);
        if (type(tok) != -0x25) err(this, d::s_Expecting, line(tok));  // 'Expecting (!'
        tok = lex(rd);
        i32 depth = 0;
        for (;;) {
            if (type(tok) == -0x24 && depth == 0) break;
            const i32 t = type(tok);
            const u32 sel = static_cast<u32>(t) + 0x32u;
            if (sel <= 0xeu) {  // -0x32 .. -0x24: operators / parentheses
                arr_push(list.raw, &jo::as_token(tok)->kind);
                if (type(tok) == -0x25) ++depth;
                else if (type(tok) == -0x24) --depth;
                lex(rd);
            } else if (t == 1 || t == 2 || t == 3 || t == 0x17 || t == 0x1b || t == 0x1c || t == 0x21) {
                u32 k = joan::orig::arg::as<u32>(joan::orig::as_script_block(this)->add_child(0x10u));         // 0x0063ea20
                void* const operand = joan::orig::arg::as<void*>(joan::orig::as_reader(this)->list_get(joan::orig::arg::i32(k)));  // 0x0063e9f0
                vparse(operand, rd);
                arr_push(list.raw, &k);
            } else {
                err(this, d::s_Unrecognized_entity_in_math, line(tok));  // 'Unrecognized entity in math expression!'
            }
            tok = peek(rd);
        }
        lex(rd);  // closing ')', result unused; tok stays the peeked ')' token

        i8 prev = static_cast<i8>(-0x32);
        depth = 0;
        for (i32 i = 1; i <= arr_count(list.raw); ++i) {
            const i32 cur = arr_get(list.raw, i);
            const i32 p = prev;
            bool illegal = false;
            if (p > 0) illegal = cur > 0 || cur == -0x25;
            else if (p >= 0) return list.release();
            else if (p == -0x25) illegal = cur < 0 && cur != -0x25;
            else if (p == -0x24) illegal = cur > 0 || cur == -0x25;
            else illegal = cur < 0 && cur != -0x25;
            if (illegal) err(this, d::s_Error_Illegal_expression, line(tok));  // 'Error: Illegal expression.'
            if (cur == -0x25) {
                ++depth;
            } else if (cur == -0x24) {
                --depth;
                if (depth < 0) err(this, d::s_Error_Unmatched_closing_pare, line(tok));  // 'Error: Unmatched closing parenthesis.'
            }
            prev = static_cast<i8>(static_cast<u8>(static_cast<u32>(cur)));
        }
        if (depth != 0) err(this, d::s_Error_Unmatched_opening_pare, line(tok));  // 'Error: Unmatched opening parenthesis.'
        if (jo::self<jo::MathExpr>(this)->rpn == nullptr) {
            void* const mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::DynArray)));  // operator new (0x1d on x86)
            jo::DynArray* arr = nullptr;
            if (mem != nullptr) {
                DeleteOnUnwind guard(mem, fn::code_value(fn::op_delete));  // state 1: operator delete
                arr = joan::orig::arg::as<jo::DynArray*>(joan::orig::dynarray(mem)->construct(4, 0x14, 0));  // 0x005843d0
                guard.armed = false;
            }
            jo::self<jo::MathExpr>(this)->rpn = arr;
        }
        OpStack stack;  // state 2
        for (i32 i = 1; i <= arr_count(list.raw); ++i) {
            i32 v = arr_get(list.raw, i);
            if (v > 0) {
                arr_push(out(), &v);
                continue;
            }
            if (v >= 0) {
                stack.release();
                return list.release();
            }
            if (v == -0x25) {
                stack.push(&v);
                continue;
            }
            if (v == -0x24) {
                for (;;) {
                    if (!(stack.count() > 0)) {
                        stack.release();
                        return list.release();
                    }
                    i32 top = stack.top();
                    if (top == -0x25) {
                        stack.pop();
                        break;
                    }
                    if (top > 0 || top == -0x24) {
                        stack.release();
                        return list.release();
                    }
                    arr_push(out(), &top);
                    stack.pop();
                }
                continue;
            }
            if (stack.count() < 1) {
                stack.push(&v);
                continue;
            }
            for (i32 j = 0;; ++j) {
                i32 top = stack.top();
                if (top == -0x25) {
                    stack.push(&v);
                    break;
                }
                if (top > 0 || top == -0x24) {
                    stack.release();
                    return list.release();
                }
                const i32 pa = prec(top);
                const i32 pb = prec(v);
                if (pa < pb) {
                    stack.push(&v);
                    break;
                }
                arr_push(out(), &top);
                stack.pop();
                if (!(stack.count() > 0)) {
                    stack.push(&v);
                    break;
                }
                if (j < 0) {  // dead in practice (j counts up from 0), kept as in the original
                    stack.release();
                    return list.release();
                }
            }
        }
        while (stack.count() != 0) {
            i32 r = stack.pop();
            arr_push(out(), &r);
        }
        stack.release();
        if (jo::self<jo::MathExpr>(this)->statements != nullptr) joan::orig::as_counter(jo::self<jo::MathExpr>(this)->statements)->clamp_to_one();  // 0x004b9d40
        return list.release();
    }

    // ------------------------------------------------------------------ 0x00648630
    std::uintptr_t parse_else(void* rd) {
        c_else.hit();
        u8* tok = peek(rd);
        if (type(tok) != 0x11) return reinterpret_cast<std::uintptr_t>(tok);
        tok = lex(rd);
        if (type(tok) == 4) {
            tok = lex(rd);
            block_stmts(tok, rd);
        } else if (type(tok) == 0x10) {
            vparse(stmt_for(this, tok), rd);
        } else {
            err(this, d::s_Expecting_or_IF_for_the_ELSE, line(tok));  // 'Expecting { or IF for the ELSE'
        }
        lex(rd);
        if (jo::self<jo::ScriptElse>(this)->statements == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        return joan::orig::arg::as<u32>(joan::orig::as_counter(jo::self<jo::ScriptElse>(this)->statements)->clamp_to_one());  // 0x004b9d40
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_05", name, addr, fn)
B("MathExpr::parse_647c90", 0x00647c90, &H::parse_math);
B("ScriptElse::parse_648630", 0x00648630, &H::parse_else);
#undef B

}  // namespace game_b111_05_3
}  // namespace

// ---------------------------------------- from modules/game_b98_06_3.cpp (game/b98_06)
namespace {
namespace game_b98_06_3 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x006484b0
    i32 key_code_index(i32 k) {
        const u32 s = static_cast<u32>(k) + 0x32u;
        if (s > 0xc) return 0;
        switch (s) {
            case 0:
            case 1:
                return 4;
            case 2:
            case 3:
                return 5;
            case 4:
                return 6;
            case 11:
                return 2;
            case 12:
                return 1;
            default:
                return 3;  // 5..10
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("key_code_index", 0x006484b0, &H::key_code_index);
#undef B

}  // namespace game_b98_06_3
}  // namespace

// ---------------------------------------- from modules/game_b102_13_2.cpp (game/b102_13)
namespace {
namespace game_b102_13_2 {

struct Obj {};
using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

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

JOAN_COUNTER(c_run_block, "ScriptNode::run_block_648560_649060");

struct H {
    u8* param(i32 n) { return joan::orig::arg::as<u8*>(joan::orig::as_reader(this)->list_get(joan::orig::arg::i32(n))); }  // 0x0063e9f0
    u8* cur() { return joan::orig::arg::as<u8*>(joan::orig::as_table(jo::view<void*>(jo::self<jo::ScriptElse>(this)->statements))->last()); }    // 0x00585f20
    i32 next() { return joan::orig::arg::as<i32>(joan::orig::as_reader(jo::view<void*>(jo::self<jo::ScriptElse>(this)->statements))->next()); }   // 0x0063ee20
    void end() { joan::orig::as_counter(jo::view<void*>(jo::self<jo::ScriptElse>(this)->statements))->clamp_to_one(); }                // 0x004b9d40

    // ------------------------------------------------------------------ 0x00648560
    i32 run_block_648560(std::uintptr_t arg) {
        c_run_block.hit();
        param(1);  // result unused in the original
        do {
            const i32 r = vcall<i32>(cur(), 0xc, arg);
            if (r == 0) return r;
            if (r == -1 || r == 2) {
                end();
                return r;
            }
        } while (next() != 0);
        end();
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_13", name, addr, fn)
B("ScriptNode::run_block_648560", 0x00648560, &H::run_block_648560);
#undef B

}  // namespace game_b102_13_2
}  // namespace

// ---------------------------------------- from modules/game_b95_06_2.cpp (game/b95_06)
namespace {
namespace game_b95_06_2 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;


struct H {
    void* node_648770() {
        joan::orig::as_script_block(this)->construct();  // 0x0063e6e0
        jo::self<jo::ScriptIf>(this)->vtable = img::obj(d::vt_Node_9db69c);
        jo::view<u32>(jo::self<jo::ScriptIf>(this)->kind) = 0xc;
        jo::self<jo::ScriptIf>(this)->else_branch = nullptr;
        return this;
    }

    // ------------------------------------------------------------------------ 0x00648e40
    i32 is_empty(u32 k) {
        const i32* const r = joan::orig::arg::as<const i32*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(k)));  // 0x00417200
        if (r != nullptr && *r != 0) return 0;
        return 1;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Node6487::ctor", 0x00648770, &H::node_648770);
B95("Obj648::is_empty", 0x00648e40, &H::is_empty);
#undef B95

}  // namespace game_b95_06_2
}  // namespace

// ---------------------------------------- from modules/game_b100_12_3.cpp (game/b100_12)
namespace {
namespace game_b100_12_3 {

struct Obj {};
using joan::call::thiscall;
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
using joan::img::obj;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::img::str;


void* const kHeap = obj(d::g_mem);

struct H {
    // ------------------------------------------------------------------ 0x00648810
    u32 dtor_648810() {
        jo::self<jo::ScriptIf>(this)->vtable = img::obj(d::vt_Node_9db69c);
        if (jo::view<void*>(jo::self<jo::ScriptIf>(this)->else_branch) != nullptr) {
            if (void* const p = jo::view<void*>(jo::self<jo::ScriptIf>(this)->else_branch)) virtual_call<u32>(p, 0, 1u);
            jo::self<jo::ScriptIf>(this)->else_branch = nullptr;
        }
        return joan::orig::arg::as<u32>(joan::orig::as_script_block(this)->destruct());  // 0x0063e780
    }

    // ------------------------------------------------------------------ 0x00648fd0
    u32 stack_pop() {
        jo::StackNode* const node = jo::self<jo::Stack>(this)->top;
        const u32 value = node->value;
        jo::self<jo::Stack>(this)->top = node->next;
        joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(node), str(d::s_d_ww_stackt_h), 0x3b);  // 0x0058ed70 "d:\ww\stackt.h"
        jo::self<jo::Stack>(this)->count = jo::self<jo::Stack>(this)->count - 1;
        return value;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_12", name, addr, fn)
B("Widget::dtor_648810", 0x00648810, &H::dtor_648810);
B("Stack::pop", 0x00648fd0, &H::stack_pop);
#undef B

}  // namespace game_b100_12_3
}  // namespace

// ---------------------------------------- from modules/game_b107_12.cpp (game/b107_12)
namespace {
namespace game_b107_12 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::img::str;

JOAN_COUNTER(c_statement, "Script::statement_6488d0");

struct H {
    // 0x006488d0
    jo::ScriptNode* statement(u32 token, u32 line) {  // the new node, or null
        c_statement.hit();
        i32 kind = 0;
        switch (token - 11u) {  // cmp 0x16 / ja: unsigned
        case 0: kind = 2; break;      // 11
        case 1: kind = 3; break;      // 12
        case 2: kind = 4; break;      // 13
        case 3: kind = 8; break;      // 14
        case 4: kind = 0xb; break;    // 15
        case 5: kind = 0xc; break;    // 16
        case 6: kind = 0xd; break;    // 17
        case 7: kind = 5; break;      // 18
        case 8: kind = 6; break;      // 19
        case 9: kind = 7; break;      // 20
        case 10: kind = 9; break;     // 21
        case 11: kind = 0xa; break;   // 22
        case 12:                      // 23..26
        case 13:
        case 14:
        case 15: kind = 0x14; break;
        case 17: kind = 0x1a; break;  // 28
        case 22: kind = 0x12; break;  // 33
        default:                      // 27, 29..32, out of range
            joan::orig::script::error(static_cast<void*>(this), str(d::s_Token_type_does_not_correspo), joan::orig::arg::i32(line));  // 0x0063eb80
            return 0;
        }
        return joan::orig::as_script_node(this)->create_child(joan::orig::arg::i32(kind));  // 0x00640db0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_12", name, addr, fn)
B("Script::statement_6488d0", 0x006488d0, &H::statement);
#undef B

}  // namespace game_b107_12
}  // namespace

// ---------------------------------------- from modules/game_b97_06_2.cpp (game/b97_06)
namespace {
namespace game_b97_06_2 {

using i32 = std::int32_t;


struct H {
    // 0x00648aa0
    void* ctor_648aa0() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(jo::NameListEntry)), 0x1e, 0, 0);  // 0x10c           // 0x005856a0
        return this;
    }

    // 0x00648ce0
    void* field_108(std::uint32_t i) {  // the entry's value pointer
        void* p = joan::orig::arg::as<void*>(joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(i)));  // 0x00417200
        return jo::as_name_list_entry(p)->value;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Obj::ctor_648aa0", 0x00648aa0, &H::ctor_648aa0);
B("Obj::field_108", 0x00648ce0, &H::field_108);
#undef B

}  // namespace game_b97_06_2
}  // namespace

// ---------------------------------------- from modules/game_b96_06_3.cpp (game/b96_06)
namespace {
namespace game_b96_06_3 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x00648ae0
    void dtor_648ae0() {
        joan::orig::as_list(this)->clear_648b80();  // 0x00648b80
        joan::orig::dynarray_b(this)->destruct();  // 0x00585710
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Obj::~Obj_648ae0", 0x00648ae0, &H::dtor_648ae0);
#undef B96

}  // namespace game_b96_06_3
}  // namespace

// ---------------------------------------- from modules/game_b91_04_3.cpp (game/b91_04)
namespace {
namespace game_b91_04_3 {

using joan::call::thiscall;

std::int32_t list_count(void* l) { return joan::orig::as_dynarray(l)->size(); }  // 0x00417ad0

struct H {
    void* self() { return this; }

    // 0x00648b80
    void clear_648b80() {
        std::int32_t i = list_count(self());
        while (i > 0) --i;  // empty loop of the original
        joan::orig::as_sound(self())->reset_585d20();  // 0x00585d20
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("List::clear_648b80", 0x00648b80, &H::clear_648b80);
#undef B91

}  // namespace game_b91_04_3
}  // namespace

// ---------------------------------------- from modules/game_b93_04_2.cpp (game/b93_04)
namespace {
namespace game_b93_04_2 {

using joan::call::thiscall;
using joan::img::obj;

using u32 = std::uint32_t;

using joan::img::str;

using joan::orig::helpers::u;


void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_free_nodes, "Stack::free_nodes");

struct H {
    // ------------------------------------------------------------------ 0x00648ee0
    std::uintptr_t free_nodes() {
        c_free_nodes.hit();
        std::uintptr_t eax = u(this);
        void* p = jo::view<void*>(jo::self<jo::Stack>(this)->top);
        while (p != nullptr) {
            void* const next = jo::as_stack_node(p)->next;
            joan::orig::as_mem(kMem)->del(p, str(d::s_d_ww_stackt_h), 0x20);  // 0x0058ed70 "d:\ww\stackt.h" (void; callers ignore the result)
            p = next;
        }
        return eax;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("Stack::free_nodes", 0x00648ee0, &H::free_nodes);
#undef B93

}  // namespace game_b93_04_2
}  // namespace

// ---------------------------------------- from modules/game_b104_12.cpp (game/b104_12)
namespace {
namespace game_b104_12 {

using u32 = std::uint32_t;


struct H {
    // ------------------------------------------------------------------------ small leaves
    u32 get_4() { return jo::view<u32>(jo::self<jo::Stack>(this)->count); }                                   // 0x00648f40
    u32 deref2() { return jo::self<jo::Stack>(this)->top->value; }                     // 0x00649040
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_12", name, addr, fn)
B("Obj::get_4_648f40", 0x00648f40, &H::get_4);
B("Obj::deref2_649040", 0x00649040, &H::deref2);
#undef B

}  // namespace game_b104_12
}  // namespace

// ---------------------------------------- from modules/game_b99_08_3.cpp (game/b99_08)
namespace {
namespace game_b99_08_3 {

using joan::img::obj;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::img::str;

using joan::img::obj;
using joan::img::str;

using i32 = std::int32_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_push, "PtrStack::push");

struct H {
    // ------------------------------------------------------------------ 0x00648f60
    i32 push(const i32* v) {
        c_push.hit();
        jo::StackNode* const node = joan::orig::arg::as<jo::StackNode*>(joan::orig::as_mem(obj(d::g_mem))->add(static_cast<std::uint32_t>(sizeof(jo::StackNode)), str(d::s_d_ww_stackt_h), 0x2b));  // 0x0058e7b0 (8 bytes on x86)
        node->value = static_cast<std::uint32_t>(*v);
        node->next = jo::self<jo::Stack>(this)->top;
        jo::self<jo::Stack>(this)->top = node;
        const i32 n = jo::self<jo::Stack>(this)->count + 1;
        jo::self<jo::Stack>(this)->count = n;
        return n;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_08", name, addr, fn)
B99("PtrStack::push", 0x00648f60, &H::push);
#undef B99

}  // namespace game_b99_08_3
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::pool::alloc(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b90_05::pool_alloc>(a0)); }  // 0x63e450
auto joan::orig::pool::free_(void* a0) -> void { joan::bind::free<&::game_b90_05_2::pool_free>(a0); }  // 0x63e4f0
auto joan::orig::Reader::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_05_2::H::reader_ctor>(this)); }  // 0x63e550
auto joan::orig::Ctrl::dtor_Ctrl_63e600() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_05_2::H::ctrl_dtor>(this)); }  // 0x63e600
auto joan::orig::ScriptNode::set_4(ScriptBlock* a0) -> void { joan::bind::member<&::game_b95_06::H::set_4>(this, a0); }  // 0x63e630
auto joan::orig::ScriptNode::free_value() -> void { joan::bind::member<&::game_b91_04_2::H::free_10>(this); }  // 0x63e660
auto joan::orig::ScriptBlock::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04_2::H::ctor_63e6e0>(this)); }  // 0x63e6e0
auto joan::orig::ScriptBlock::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_06::H::dtor_63e780>(this)); }  // 0x63e780
auto joan::orig::Script::clear() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_06::H::script_clear>(this)); }  // 0x63e7f0
auto joan::orig::Reader::list_get(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_05_2::H::reader_list_get>(this, a0)); }  // 0x63e9f0
auto joan::orig::ScriptBlock::add_child(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_14::H::push_value>(this, a0)); }  // 0x63ea20
auto joan::orig::ScriptBlock::delete_child(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_04::H::remove_a>(this, a0)); }  // 0x63eb00
auto joan::orig::Reader::count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_05_2::H::reader_count>(this)); }  // 0x63ecd0
auto joan::orig::ScriptBlock::rewind() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b89_06::H::sub_reset>(this)); }  // 0x63ed40
auto joan::orig::ScriptBlock::seek_child(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_06_2::H::call_585660>(this, a0)); }  // 0x63eda0
auto joan::orig::Reader::next() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_05_2::H::reader_next>(this)); }  // 0x63ee20
auto joan::orig::ScriptBlock::var_at(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04_2::H::item_63ef20>(this, a0)); }  // 0x63ef20
auto joan::orig::Script::add_node() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_06::H::add_node>(this)); }  // 0x63ef50
auto joan::orig::ScriptBlock::delete_var(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_04::H::remove_b>(this, a0)); }  // 0x63f080
auto joan::orig::ScriptBlock::var_count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_04_2::H::count_63f100>(this)); }  // 0x63f100
auto joan::orig::Ctrl::Ctrl_63f190() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_05::H::ctrl_ctor_63f190>(this)); }  // 0x63f190
auto joan::orig::Script::dtor_63f250() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_14::H::dtor_63f250>(this)); }  // 0x63f250
auto joan::orig::Gadget::destroy_tree() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_03::H::destroy_tree>(this)); }  // 0x63f2c0
auto joan::orig::Widget::scalar_dtor_63f4e0(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_12_2::H::scalar_dtor_63f4e0>(this, a0)); }  // 0x63f4e0
auto joan::orig::StringPool::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b108_09::H::scalar_deleting_dtor>(this, a0)); }  // 0x63f530
auto joan::orig::ScriptNode::create_child(std::int32_t a0) -> ScriptNode* { return joan::bind::conv<ScriptNode*>(joan::bind::member<&::game_b106_07::H::create>(this, a0)); }  // 0x640db0
auto joan::orig::ScriptNode::byte_size() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_05_2::H::size_14>(this)); }  // 0x641a40
auto joan::orig::ScriptBlock::byte_size() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_04_2::H::size_641a60>(this)); }  // 0x641a60
auto joan::orig::ScriptLexer::skip_newlines() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::skip_9>(this)); }  // 0x642370
auto joan::orig::ScriptLexer::next() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_01::H::lexer_next>(this)); }  // 0x6423e0
auto joan::orig::Range::at_end() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_04::H::at_end>(this)); }  // 0x642440
auto joan::orig::ScriptId::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_04::H::node2_ctor>(this)); }  // 0x6424f0
auto joan::orig::ScriptId::get_21() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_06::H::get21>(this)); }  // 0x642540
auto joan::orig::ScriptId::set_21(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_08::H::set_21>(this, a0)); }  // 0x642560
auto joan::orig::ScriptInit::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_17::H::node_dtor>(this)); }  // 0x6426d0
auto joan::orig::ScriptReset::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_18::H::node_dtor_body>(this)); }  // 0x6427a0
auto joan::orig::ScriptVarDecl::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_04::H::node11_ctor>(this)); }  // 0x642f20
auto joan::orig::ScriptVarDecl::name() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b104_11::H::sub14>(this)); }  // 0x642f60
auto joan::orig::Ctrl::Ctrl_643000() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_05_2::H::ctrl_ctor_643000>(this)); }  // 0x643000
auto joan::orig::ScriptFuncCall::destruct() -> void { joan::bind::member<&::game_b96_06_2::H::dtor_643090>(this); }  // 0x643090
auto joan::orig::ScriptBlock::make_statement(std::int32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b108_16::H::make_643550>(this, a0, a1)); }  // 0x643550
auto joan::orig::ScriptCall::parse(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_04::H::parse_call>(this, a0)); }  // 0x643c00
auto joan::orig::ScriptValue::forward_34(std::uintptr_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_06_2::H::forward_34>(this, a0)); }  // 0x644390
auto joan::orig::ScriptExpr::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_06::H::release_644cd0>(this)); }  // 0x644cd0
auto joan::orig::Widget::dtor_body_6452f0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_12_2::H::dtor_body_6452f0>(this)); }  // 0x6452f0
auto joan::orig::MathExpr::op_precedence(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_06_3::H::key_code_index>(this, a0)); }  // 0x6484b0
auto joan::orig::Script::statement_6488d0(std::uint32_t a0, std::uint32_t a1) -> ScriptNode* { return joan::bind::conv<ScriptNode*>(joan::bind::member<&::game_b107_12::H::statement>(this, a0, a1)); }  // 0x6488d0
auto joan::orig::ScriptCache::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_06_2::H::ctor_648aa0>(this)); }  // 0x648aa0
auto joan::orig::List::clear_648b80() -> void { joan::bind::member<&::game_b91_04_3::H::clear_648b80>(this); }  // 0x648b80
auto joan::orig::NameList::add_648bd0(const char* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_06::H::name_add>(this, a0, a1)); }  // 0x648bd0
auto joan::orig::List::find_name_648c50(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_12::H::find_name>(this, a0)); }  // 0x648c50
auto joan::orig::ScriptCache::compiled_at(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_06_2::H::field_108>(this, a0)); }  // 0x648ce0
auto joan::orig::ScriptCache::add_ref(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_06::H::add_ref>(this, a0)); }  // 0x648d20
auto joan::orig::ScriptCache::release(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_05::H::shared_release>(this, a0)); }  // 0x648d60
auto joan::orig::ScriptCache::is_free(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_06_2::H::is_empty>(this, a0)); }  // 0x648e40
auto joan::orig::LinkedStack::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_06_2::H::zero2>(this)); }  // 0x648ea0
auto joan::orig::LinkedStack::free_nodes() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b93_04_2::H::free_nodes>(this)); }  // 0x648ee0
auto joan::orig::LinkedStack::count() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_12::H::get_4>(this)); }  // 0x648f40
auto joan::orig::LinkedStack::push(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_08_3::H::push>(this, a0)); }  // 0x648f60
auto joan::orig::LinkedStack::pop() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_12_3::H::stack_pop>(this)); }  // 0x648fd0
auto joan::orig::LinkedStack::top() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_12::H::deref2>(this)); }  // 0x649040
auto joan::orig::Script::compile(void* a0, const char* a1) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b111_01_2::H::compile>(this, a0, a1)); }  // 0x649280
