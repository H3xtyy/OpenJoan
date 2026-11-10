// en2d/font2d.cpp: functions of the original d:\ww\en2d\font2d.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../canvas_api.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <limits>
#include <cstdarg>
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "cjk_glyphs.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b106_06.cpp (game/b106_06)
//
//      0x00623e90 Font2d::init            (thiscall, ret 0x24) - loads the font texture, measures
//                 every glyph from first..last (0x00628490 on the locked texture), fills the
//                 14-byte glyph table at this+0x3c and the metrics at this+0x2a..0x3a.
//
//  - 0x00623e90: error paths leak the queried texture and the 0x191-byte message string object
//    (no destructor exists) exactly like the original; line numbers are the global word
//    0x00a1974c + constant, as in the original.
namespace {
namespace game_b106_06 {

template <class R = i32, class... A>
R com_call(void* object, i32 index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

JOAN_COUNTER(c_font_init, "Font2d::init");

void font_error(i32 hr, i32 line_ofs) {
    const i32 line = static_cast<i32>(global<i16>(d::g_a1974c)) + line_ofs;
    const char* const msg = joan::orig::as_sys_en(obj(d::g_sys3d))->error_string(joan::orig::arg::i32(hr));  // 0x006002b0
    joan::orig::as_error_log(obj(d::g_err))->fatal_error(msg, str(d::s_d_ww_en2d_font2d_cpp), joan::orig::arg::u32(line));  // 0x005869d0
}

struct H {
    // 0x00623e90
    u32 font_init(const char* name, i16 cell_w, i16 cell_h, i16 per_row, u16 first, u16 last, i16 spacing,
                  u16 v38, u16 v3a) {
        c_font_init.hit();
        void* const tm = obj(d::g_texture_array);
        if (jo::view<i8>(jo::self<jo::Font2d>(this)->loaded) != 0) joan::orig::as_font(this)->release();  // 0x00624dc0
        const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(tm)->mode());           // 0x0060a720
        joan::orig::as_texture_manager(tm)->set_mode(1u);                          // 0x0060a740
        jo::view<void*>(jo::self<jo::Font2d>(this)->texture) = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(tm)->load(joan::orig::arg::str(name), 1, 0u));  // 0x006099c0
        joan::orig::as_texture_manager(tm)->flush_loaders();                             // 0x0060a770
        joan::orig::as_texture_manager(tm)->set_mode(joan::orig::arg::u32(saved));                      // 0x0060a740
        if (name != nullptr) std::strcpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(this) + 1)), joan::orig::arg::str(name));  // strcpy
        if (jo::view<void*>(jo::self<jo::Font2d>(this)->texture) == nullptr || joan::orig::arg::as<u32>(joan::orig::as_texture(jo::view<void*>(jo::self<jo::Font2d>(this)->texture))->ready()) == 0) {  // 0x00605b70
            alignas(4) char msg[0x194];  // 0x191-byte string object (0x005a11a0)
            joan::orig::as_string(msg)->construct(str(d::s_Warning_Cannot_initialize_fo));      // 0x005a11a0 'Warning! Cannot initialize font "'
            joan::orig::as_string(msg)->append(joan::orig::arg::str(name));     // 0x005a1550
            joan::orig::as_string(msg)->append(str(d::s_quote_excl));                // 0x005a1550
            const char* s = joan::orig::arg::as<const char*>(joan::orig::as_string(msg)->c_str());  // 0x00417d90
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), joan::orig::arg::str(s));         // 0x00586cb0
            return 0;
        }
        jo::self<jo::Font2d>(this)->first_char = first;
        jo::self<jo::Font2d>(this)->last_char = last;
        {
            const i32 size = (static_cast<i32>(jo::self<jo::Font2d>(this)->last_char) - static_cast<i32>(jo::self<jo::Font2d>(this)->first_char) + 1) * 0xe;
            const i32 line = static_cast<i32>(global<i16>(d::g_a1974c)) + 0x21;
            jo::view<void*>(jo::self<jo::Font2d>(this)->glyphs) = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size), str(d::s_d_ww_en2d_font2d_cpp), joan::orig::arg::i32(line));  // 0x0058e7b0
        }
        {
            const i32 size = (static_cast<i32>(jo::self<jo::Font2d>(this)->last_char) - static_cast<i32>(jo::self<jo::Font2d>(this)->first_char) + 1) * 0xe;
            std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Font2d>(this)->glyphs)), 0, joan::orig::arg::u32(size));  // memset
        }
        void* tex = nullptr;
        {
            void* res = jo::view<void*>(jo::self<jo::Font2d>(this)->texture->d3d_base);
            const i32 hr = com_call<i32>(res, 0, img::obj(d::blob_9e1708), &tex);  // QueryInterface
            if (hr < 0) {
                font_error(hr, 0x29);
                return 0;
            }
        }
        u32 desc[8];  // D3DSURFACE_DESC
        {
            const i32 hr = com_call<i32>(tex, 14, 0u, static_cast<void*>(desc));  // GetLevelDesc
            if (hr < 0) {
                font_error(hr, 0x37);
                return 0;
            }
        }
        const u32 bpp = joan::orig::arg::as<u32>(joan::orig::format::bytes_per_pixel(joan::orig::arg::u32(desc[0])));  // 0x005ffe60
        struct { i32 pitch; u8* bits; } locked;                 // D3DLOCKED_RECT {Pitch, pBits} (pBits at +0x8 on x64)
        {
            const i32 hr = com_call<i32>(tex, 16, 0u, static_cast<void*>(&locked), static_cast<void*>(nullptr), 0x10u);  // LockRect
            if (hr < 0) {
                font_error(hr, 0x50);
                return 0;
            }
        }
        // The Chinese release's Goudy12(18) black is a sheet four times as wide with 56 columns, where the game asks for the English
        // sheet's 14; read as 14 columns, every character from the second row on comes from the wrong cell (a "1" drew as "[").
        if (per_row == 14 && cell_w == 17 && jo::view<i32>(jo::self<jo::Font2d>(this)->texture->width) == 1024) per_row = 56;
        i16 x = spacing, y = spacing;
        i16 min_top = static_cast<i16>(cell_h - 1);
        i16 max_bottom = 0;
        i16 col = 0;
        auto glyph = [this](u16 ch) -> u8* {
            return static_cast<u8*>(jo::view<void*>(jo::self<jo::Font2d>(this)->glyphs)) +
                   (static_cast<i32>(ch) - static_cast<i32>(jo::self<jo::Font2d>(this)->first_char)) * 0xe;
        };
        for (u16 ch = jo::self<jo::Font2d>(this)->first_char; static_cast<i32>(ch) <= static_cast<i32>(jo::self<jo::Font2d>(this)->last_char);
             ch = static_cast<u16>(ch + 1)) {
            i16 l = 0, t = 0;
            i16 r = static_cast<i16>(cell_w - 1);
            i16 b = static_cast<i16>(cell_h - 1);
            joan::orig::as_bitmap(this)->crop_key_628490(joan::orig::arg::mptr(locked.bits), joan::orig::arg::i32(locked.pitch), joan::orig::arg::u32(static_cast<u16>(bpp)), joan::orig::arg::u32(desc[0]), joan::orig::arg::u32(x), joan::orig::arg::u32(y), &l, &t, &r, &b);  // 0x00628490
            const i16 w = static_cast<i16>(r - l + 1);
            const i16 h = static_cast<i16>(b - t + 1);
            u8* e = glyph(ch);
            jo::as_font_glyph(e)->top = t;
            jo::as_font_glyph(e)->x0 = static_cast<i16>(x + l);
            jo::as_font_glyph(e)->y0 = static_cast<i16>(y + t);
            jo::as_font_glyph(e)->x1 = static_cast<i16>(x + r);
            jo::as_font_glyph(e)->y1 = static_cast<i16>(y + b);
            jo::as_font_glyph(e)->width = w;
            jo::as_font_glyph(e)->height = h;
            if (w > jo::self<jo::Font2d>(this)->max_w) jo::self<jo::Font2d>(this)->max_w = w;
            if (h > jo::self<jo::Font2d>(this)->max_h) jo::self<jo::Font2d>(this)->max_h = h;
            if (t < min_top) min_top = t;
            if (b > max_bottom) max_bottom = b;
            x = static_cast<i16>(x + (spacing + cell_w));
            bool new_row = static_cast<i32>(x) + cell_w > jo::view<i32>(jo::self<jo::Font2d>(this)->texture->width);
            if (!new_row) {
                col = static_cast<i16>(col + 1);
                new_row = col == per_row;
            }
            if (new_row) {
                x = spacing;
                y = static_cast<i16>(y + (spacing + cell_h));
                col = 0;
            }
        }
        jo::self<jo::Font2d>(this)->glyph_height = static_cast<i16>(max_bottom - min_top + 1);
        for (u16 ch = jo::self<jo::Font2d>(this)->first_char; static_cast<i32>(ch) <= static_cast<i32>(jo::self<jo::Font2d>(this)->last_char);
             ch = static_cast<u16>(ch + 1)) {
            u8* e = glyph(ch);
            jo::as_font_glyph(e)->top = static_cast<i16>(jo::as_font_glyph(e)->top - min_top);
        }
        jo::self<jo::Font2d>(this)->line_height = jo::self<jo::Font2d>(this)->max_h;
        jo::self<jo::Font2d>(this)->space_width = jo::self<jo::Font2d>(this)->glyphs[0x31 - static_cast<i32>(jo::self<jo::Font2d>(this)->first_char)].width;
        jo::self<jo::Font2d>(this)->spacing = v38;
        jo::self<jo::Font2d>(this)->kerning = v3a;
        if (jo::view<i16>(jo::self<jo::Font2d>(this)->kerning) != 0) {
            for (u16 ch = jo::self<jo::Font2d>(this)->first_char; static_cast<i32>(ch) <= static_cast<i32>(jo::self<jo::Font2d>(this)->last_char);
                 ch = static_cast<u16>(ch + 1)) {
                u8* e = glyph(ch);
                jo::as_font_glyph(e)->width = static_cast<i16>(jo::as_font_glyph(e)->width - jo::view<i16>(jo::self<jo::Font2d>(this)->kerning));
            }
        }
        {
            const i32 hr = com_call<i32>(tex, 17, 0u);  // UnlockRect
            if (hr < 0) {
                font_error(hr, 0xa9);
                return 0;
            }
        }
        com_call<u32>(tex, 2);  // Release
        joan::orig::as_texture_manager(tm)->release(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Font2d>(this)->texture)));  // 0x0060a550
        jo::view<void*>(jo::self<jo::Font2d>(this)->texture) = nullptr;
        jo::self<jo::Font2d>(this)->loaded = 1;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_06", name, addr, fn)
B("Font2d::init", 0x00623e90, &H::font_init);
#undef B

}  // namespace game_b106_06
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
//
//    - Font::init 0x00624810 (name, w1, w2), ret 0xc: 'FONT\' name '.FIN' reader, name copy
//      (0x587650 + strncpy 0x20), sound bank load with the bank state saved / restored, glyph
//      table (+0x3c, 14 bytes per glyph, alloc font2d.cpp line [0xa19750]+0x2e), optional
//      baseline shift of glyph word +0xa by +0x3a. Warning + return 0 when the bank entry
//      fails. SEH frame only guards the reader destructor (normal path kept).
namespace {
namespace game_b104_11 {


void* const kMemory = obj(d::g_mem);
void* const kSounds = obj(d::g_texture_array);
void* const kLog = obj(d::g_err);

struct alignas(4) Path {  // 0x194 bytes in the frames (0x005a1170 writes byte 0)
    u8 bytes[0x194];
};
struct alignas(alignof(void*)) File {  // 0x110 in the frames (0x004179c0); a File: 0x10e on x86, 0x112 on x64
    u8 bytes[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};

const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
void path_append(Path& p, const char* s) { joan::orig::as_string(&p)->append(joan::orig::arg::str(s)); }  // 0x005a1550
u16 read_u16a(File& f) { return static_cast<u16>(joan::orig::as_file(&f)->file_get_unsigned_short()); }   // 0x00587d20
u16 read_u16b(File& f) { return static_cast<u16>(joan::orig::as_file(&f)->file_get_short()); }   // 0x00587c20

void* mem_alloc(u32 size, const char* file, i32 line) {
    return joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058e7b0
}

JOAN_COUNTER(c_font_init, "Font::init");

struct H {
    // ------------------------------------------------------------------ 0x00624810
    u32 font_init(const char* name, u32 w1, u32 w2) {
        c_font_init.hit();
        if (jo::view<i8>(jo::self<jo::Font>(this)->loaded) != 0) joan::orig::as_font(this)->release();  // 0x00624dc0
        Path path;
        joan::orig::as_string(&path)->construct();    // 0x005a1170
        joan::orig::as_string(&path)->assign(str(d::s_FONT));            // 0x005a1440 'FONT\'
        path_append(path, name);
        path_append(path, str(d::s_FIN));  // '.FIN'
        File file;
        joan::orig::as_file(&file)->construct();  // 0x004179c0
        joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(path)), 1, static_cast<char>(0));  // 0x00586da0 open
        joan::orig::as_file(&file)->file_read_array(joan::orig::arg::mptr(reinterpret_cast<u8*>(this) + 1), 0x20u, 1u);  // 0x00587650
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(this) + 1)), joan::orig::arg::str(name), 0x20u);  // strncpy
        const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kSounds)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kSounds)->set_mode(1u);                      // 0x0060a740
        jo::self<jo::Font>(this)->texture = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(kSounds)->load(joan::orig::arg::str(name), 1, 0u));  // 0x006099c0
        joan::orig::as_texture_manager(kSounds)->set_mode(joan::orig::arg::u32(saved));
        if (jo::self<jo::Font>(this)->texture == nullptr) {
            Path warn;
            joan::orig::as_string(&warn)->construct(str(d::s_Warning_Cannot_initialize_fo));      // 0x005a11a0 'Warning! Cannot initialize font "'
            path_append(warn, name);
            path_append(warn, str(d::s_quote_excl));
            joan::orig::error::fatal(joan::orig::arg::mptr(kLog), joan::orig::arg::str(c_str(warn)));  // 0x00586cb0
            joan::orig::as_file(&file)->destruct();  // 0x005872b0 ~File
            return 0;
        }
        jo::self<jo::Font>(this)->first_char = read_u16a(file);
        jo::self<jo::Font>(this)->last_char = read_u16a(file);
        jo::view<u16>(jo::self<jo::Font>(this)->max_w) = read_u16b(file);
        jo::view<u16>(jo::self<jo::Font>(this)->max_h) = read_u16b(file);
        jo::view<u16>(jo::self<jo::Font>(this)->glyph_height) = read_u16b(file);
        jo::view<u16>(jo::self<jo::Font>(this)->space_width) = read_u16b(file);
        joan::orig::as_file(&file)->file_get_long();   // 0x00587e20 (value unused)
        const i32 line = static_cast<i32>(global<i16>(d::g_a19750)) + 0x2e;
        const i32 bytes = (static_cast<i32>(jo::self<jo::Font>(this)->last_char) - static_cast<i32>(jo::self<jo::Font>(this)->first_char) + 1) * 0xe;
        jo::self<jo::Font>(this)->glyphs = static_cast<jo::FontGlyph*>(mem_alloc(static_cast<u32>(bytes), str(d::s_d_ww_en2d_font2d_cpp), line));
        std::memset(joan::orig::arg::mptr(jo::self<jo::Font>(this)->glyphs), 0, joan::orig::arg::u32(static_cast<u32>((static_cast<i32>(jo::self<jo::Font>(this)->last_char) - static_cast<i32>(jo::self<jo::Font>(this)->first_char) + 1) * 0xe)));
        for (u16 i = jo::self<jo::Font>(this)->first_char; static_cast<i32>(i) <= static_cast<i32>(jo::self<jo::Font>(this)->last_char); i = static_cast<u16>(i + 1)) {
            jo::FontGlyph* const g = jo::self<jo::Font>(this)->glyphs + (static_cast<i32>(i) - static_cast<i32>(jo::self<jo::Font>(this)->first_char));
            jo::view<u16>(g->top) = read_u16b(file);
            jo::view<u16>(g->x0) = read_u16b(file);
            jo::view<u16>(g->y0) = read_u16b(file);
            jo::view<u16>(g->x1) = read_u16b(file);
            jo::view<u16>(g->y1) = read_u16b(file);
            jo::view<u16>(g->width) = read_u16b(file);
            jo::view<u16>(g->height) = read_u16b(file);
        }
        joan::orig::as_file(&file)->file_close();  // 0x00587260 close
        jo::view<u16>(jo::self<jo::Font>(this)->line_height) = jo::view<u16>(jo::self<jo::Font>(this)->max_h);
        jo::view<u16>(jo::self<jo::Font>(this)->spacing) = static_cast<u16>(w1);
        jo::self<jo::Font>(this)->kerning = static_cast<u16>(w2);
        if (jo::view<i16>(jo::self<jo::Font>(this)->kerning) != 0) {
            for (u16 i = jo::self<jo::Font>(this)->first_char; static_cast<i32>(i) <= static_cast<i32>(jo::self<jo::Font>(this)->last_char);
                 i = static_cast<u16>(i + 1)) {
                u8* const g = jo::view<u8*>(jo::self<jo::Font>(this)->glyphs) + (static_cast<i32>(i) - static_cast<i32>(jo::self<jo::Font>(this)->first_char)) * 0xe;
                jo::view<u16>(jo::as_font_glyph(g)->width) = static_cast<u16>(jo::view<u16>(jo::as_font_glyph(g)->width) - jo::self<jo::Font>(this)->kerning);
            }
        }
        joan::orig::as_texture_manager(kSounds)->release(joan::orig::arg::mptr(jo::self<jo::Font>(this)->texture));  // 0x0060a550
        jo::self<jo::Font>(this)->texture = nullptr;
        jo::self<jo::Font>(this)->loaded = 1;
        joan::orig::as_file(&file)->destruct();  // ~File
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("Font::init", 0x00624810, &H::font_init);
#undef B

}  // namespace game_b104_11
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
//
//      0x00624dc0 Font2d::release()                             thiscall, returns 1
namespace {
namespace game_b92_05 {


void* const kMem = obj(d::g_mem);
void* const kTexMgr = obj(d::g_texture_array);

// Memory manager free / realloc with source file and line (0x0058ed70 / 0x0058eb30).
void mem_free(void* p, img::Addr file, std::int32_t line) {
    joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}
using joan::orig::helpers::line_base;

struct H {
    // 0x00624dc0
    std::int32_t font_release() {
        if (jo::self<jo::Font2d>(this)->glyphs != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::Font2d>(this)->glyphs), d::s_d_ww_en2d_font2d_cpp, line_base(d::g_a19754) + 3);  // 'd:\ww\en2d\font2d.cpp'
            jo::self<jo::Font2d>(this)->glyphs = nullptr;
        }
        if (jo::self<jo::Font2d>(this)->texture != nullptr) {
            joan::orig::as_texture_manager(kTexMgr)->release(joan::orig::arg::mptr(jo::self<jo::Font2d>(this)->texture));  // 0x0060a550
            jo::self<jo::Font2d>(this)->texture = nullptr;
        }
        jo::self<jo::Font2d>(this)->loaded = 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("Font2d::release", 0x00624dc0, &H::font_release);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
namespace {
namespace game_b104_11_2 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

// The four crop passes shared by 0x00627f10 / 0x00628490. pix(row, col) is the pixel test.
template <class P>
void crop_box(i16 x0, i16 y0, i16* left, i16* top, i16* right, i16* bottom, P pix) {
    // top
    bool found = false;
    i16 r = static_cast<i16>(y0 + *top);
    for (; r <= y0 + *bottom; r = static_cast<i16>(r + 1)) {
        for (i16 c = static_cast<i16>(x0 + *left); c <= x0 + *right; c = static_cast<i16>(c + 1)) {
            if (pix(r, c)) {
                found = true;
                *top = static_cast<i16>(r - y0);
                break;
            }
        }
        if (found) break;
    }
    if (r > y0 + *bottom) *top = *bottom;
    // bottom
    found = false;
    r = static_cast<i16>(y0 + *bottom);
    for (; r >= y0 + *top; r = static_cast<i16>(r - 1)) {
        for (i16 c = static_cast<i16>(x0 + *left); c <= x0 + *right; c = static_cast<i16>(c + 1)) {
            if (pix(r, c)) {
                found = true;
                *bottom = static_cast<i16>(r - y0);
                break;
            }
        }
        if (found) break;
    }
    if (r < y0 + *top) *bottom = *top;
    // left
    found = false;
    i16 c = static_cast<i16>(x0 + *left);
    for (; c <= x0 + *right; c = static_cast<i16>(c + 1)) {
        for (i16 rr = static_cast<i16>(y0 + *top); rr <= y0 + *bottom; rr = static_cast<i16>(rr + 1)) {
            if (pix(rr, c)) {
                found = true;
                *left = static_cast<i16>(c - x0);
                break;
            }
        }
        if (found) break;
    }
    if (c > x0 + *right) *left = *right;
    // right
    found = false;
    c = static_cast<i16>(x0 + *right);
    for (; c >= x0 + *left; c = static_cast<i16>(c - 1)) {
        for (i16 rr = static_cast<i16>(y0 + *top); rr <= y0 + *bottom; rr = static_cast<i16>(rr + 1)) {
            if (pix(rr, c)) {
                found = true;
                *right = static_cast<i16>(c - x0);
                break;
            }
        }
        if (found) break;
    }
    if (c < x0 + *left) *right = *left;
}

JOAN_COUNTER(c_crop_628490, "Bitmap::crop_key_628490");

struct H {
    // ------------------------------------------------------------------ 0x00628490
    u32 crop_628490(u8* buf, i32 pitch, u32 bpp, u32 key, u32 x0, u32 y0, i16* left, i16* top,
                    i16* right, i16* bottom) {
        c_crop_628490.hit();
        const i32 n = static_cast<i16>(bpp);
        crop_box(static_cast<i16>(x0), static_cast<i16>(y0), left, top, right, bottom, [&](i16 r, i16 c) {
            const u32 off = static_cast<u32>(static_cast<i32>(c)) * static_cast<u32>(n) +
                            static_cast<u32>(static_cast<i32>(r)) * static_cast<u32>(pitch);
            u8* const px = reinterpret_cast<u8*>(reinterpret_cast<std::uintptr_t>(buf) + off);
            return (joan::orig::arg::as<u32>(joan::orig::as_bitmap(this)->pixel_alpha(joan::orig::arg::mptr(px), joan::orig::arg::u32(key))) & 0xffu) != 0;  // 0x00628940
        });
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("Bitmap::crop_key_628490", 0x00628490, &H::crop_628490);
#undef B

}  // namespace game_b104_11_2
}  // namespace

// ---------------------------------------- from modules/game_b90_05.cpp (game/b90_05)
namespace {
namespace game_b90_05 {


struct H {

    // 0x00623dc0
    void* text_desc_clear() {
        jo::self<jo::Font2d>(this)->loaded = 0;
        jo::self<jo::Font2d>(this)->texture = nullptr;
        jo::self<jo::Font2d>(this)->first_char = 0;
        jo::self<jo::Font2d>(this)->last_char = 0;
        jo::self<jo::Font2d>(this)->max_w = 0;
        jo::self<jo::Font2d>(this)->max_h = 0;
        jo::self<jo::Font2d>(this)->glyph_height = 0;
        jo::self<jo::Font2d>(this)->line_height = 0;
        jo::self<jo::Font2d>(this)->space_width = 0;
        jo::self<jo::Font2d>(this)->spacing = 0;
        jo::self<jo::Font2d>(this)->kerning = 0;
        jo::self<jo::Font2d>(this)->glyphs = nullptr;
        jo::view<std::uint8_t>(jo::self<jo::Font2d>(this)->name[0]) = 0;
        return this;
    }
};

#define B9005(name, addr, fn) JOAN_REPLACE("game/b90_05", name, addr, fn)
B9005("TextDesc::clear", 0x00623dc0, &H::text_desc_clear);
#undef B9005

}  // namespace game_b90_05
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
namespace {
namespace game_leaves80a {

struct H {
    void font_dtor() {}                                       // 0x00623e70
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Font::~Font", 0x00623e70, &H::font_dtor);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

using joan::hook::x87::Fpu;

using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::img::obj;


struct H {
    // ------------------------------------------------------------------ 0x00624e60
    i32 round_scaled(float s) {
        const double v = Fpu{true}.mul(static_cast<double>(jo::self<jo::Font2d>(this)->space_width), static_cast<double>(s));  // fild + fmul
        return joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(v)));  // 0x00590a20
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Obj::round_scaled", 0x00624e60, &H::round_scaled);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

using joan::hook::x87::Fpu;

using i16 = std::int16_t;
using u32 = std::uint32_t;

using joan::img::obj;


void* const kMath = obj(d::g_misc);

struct H {
    // ------------------------------------------------------------------ 0x00624eb0
    u32 scaled_round_624eb0(float s) {
        const Fpu fpu{true};
        const double v = fpu.mul(static_cast<double>(jo::self<jo::Font2d>(this)->line_height), static_cast<double>(s));
        return joan::orig::arg::as<u32>(joan::orig::as_misc(kMath)->round(joan::orig::arg::f64(v)));  // 0x00590a20
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Obj::scaled_round_624eb0", 0x00624eb0, &H::scaled_round_624eb0);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_ui1.cpp (game/ui1)
namespace {
namespace game_ui1 {

using joan::hook::x87::Fpu;

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::call::cdecl_call;

constexpr img::Addr kFiles(d::g_misc);


constexpr img::Addr kTextures(d::g_texture_array);

JOAN_COUNTER(c_draw_glyphs, "Font::draw_glyphs");

// fild (word field) ; fmul (float scale) ; fstp qword ; Files::round (0x00590a20, ax used).
std::int16_t scaled(std::int16_t v, float scale) {
    const double p = joan::hook::x87::Fpu{true}.mul(static_cast<double>(v), static_cast<double>(scale));
    return static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(img::obj(kFiles))->round(joan::orig::arg::f64(p))));  // 0x00590a20
}

// 4-byte text cursor (0x0058b9c0 ctor) and its function-pointer dispatched methods.
void cursor_init(std::uintptr_t* it, const char* text) { joan::orig::as_text_cursor(it)->construct(joan::orig::arg::str(text)); }      // 0x0058b9c0
std::int32_t cursor_count(std::uintptr_t* it) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_text_cursor(it)->length()); }       // 0x0058c190
std::int32_t cursor_char(std::uintptr_t* it) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_text_cursor(it)->get()); }        // 0x0058ba70
void cursor_next(std::uintptr_t* it) { joan::orig::as_text_cursor(it)->next(); }                              // 0x0058bbc0
std::int32_t to_upper(std::int32_t ch) { return joan::orig::arg::as<std::int32_t>(joan::orig::text_toupper(joan::orig::arg::i32(ch))); }          // 0x0058e380

struct H {
    std::uint8_t* glyph(std::int32_t ch) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_font(this)->glyph(joan::orig::arg::u16(static_cast<std::uint16_t>(ch)))); }  // 0x00627e80

    // ---- game/ui1 ----------------------------------------------------------------------
    // 0x00625c50 Font::draw_glyphs: glyph +0 y offset, +2..+8 source rect, +0xa width, +0xc
    // height; the font texture (name at +1) is loaded into +0x26 for each glyph blit and
    // released again. Arguments +0x24 / +0x28 are unused. ret 0x2c.
    std::int16_t draw_glyphs(std::int16_t x, std::int16_t y, const char* text, void* target, std::uint32_t colour, std::int16_t mode,
                             std::int16_t flags, std::uint32_t, std::uint32_t, float scale_x, float scale_y) {
        c_draw_glyphs.hit();
        if (static_cast<std::int8_t>(jo::self<jo::Font>(this)->loaded) == 0) return x;
        std::uintptr_t it = 0;  // a TextCursor (one pointer)
        cursor_init(&it, text);
        const std::int32_t count = cursor_count(&it);
        std::int32_t first_blit = -1;
        void* blit_texture = nullptr;  // the texture of first_blit's material (ours: CJK glyphs come from another)
        std::int32_t i = 1;
        for (;;) {
            std::int32_t ch = cursor_char(&it);
            if (ch == 0 || i > count) break;
            if (mode == 1) ch = to_upper(ch);
            if (ch == 0x20) {
                const std::int16_t space = scaled(jo::self<jo::Font>(this)->space_width, scale_x);
                x = static_cast<std::int16_t>(x + space);
            } else {
                std::uint8_t* const g = glyph(ch);
                if (g != nullptr) {
                    const std::int16_t gw = scaled(jo::as_font_glyph(g)->width, scale_x);
                    const std::int16_t gh = scaled(jo::as_font_glyph(g)->height, scale_y);
                    const std::int16_t gy = static_cast<std::int16_t>(static_cast<std::int32_t>(y) + scaled(jo::as_font_glyph(g)->top, scale_y));
                    if (gw > 0) {
                        std::uint8_t* const g2 = glyph(ch);
                        if (g2 != nullptr) {
                            const std::int32_t saved = joan::orig::arg::as<std::int32_t>(joan::orig::as_texture_manager(img::obj(kTextures))->mode());  // 0x0060a720
                            joan::orig::as_texture_manager(img::obj(kTextures))->set_mode(1u);                                       // 0x0060a740
                            jo::self<jo::Font>(this)->texture = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(reinterpret_cast<const char*>(this) + 1), 1, 0u));  // 0x006099c0
                            joan::orig::as_texture_manager(img::obj(kTextures))->set_mode(joan::orig::arg::u32(saved));
                            const std::int32_t x2 = static_cast<std::int32_t>(x) + gw - 1;
                            const std::int32_t y2 = static_cast<std::int32_t>(gy) + gh - 1;
                            void* const glyph_texture = joan::cjk::texture_for(g2, jo::self<jo::Font>(this)->texture);
                            // blit_more (0x00636640) adds the quad to first_blit's material, whose texture blit set: a glyph
                            // from another texture (a CJK atlas page, cjk_glyphs.hpp) starts a material of its own. One font
                            // texture throughout, as every Latin label, takes the original's path.
                            if (first_blit == -1 || glyph_texture != blit_texture) {
                                blit_texture = glyph_texture;
                                first_blit = joan::orig::arg::as<std::int32_t>(joan::orig::as_billboard(target)->blit(joan::orig::arg::i16(x), joan::orig::arg::i16(gy), joan::orig::arg::i16(x2), joan::orig::arg::i16(y2), joan::orig::arg::u32(colour), joan::orig::arg::mptr(glyph_texture), joan::orig::arg::i16(jo::as_font_glyph(g2)->x0), joan::orig::arg::i16(jo::as_font_glyph(g2)->y0), joan::orig::arg::i16(jo::as_font_glyph(g2)->x1), joan::orig::arg::i16(jo::as_font_glyph(g2)->y1), joan::orig::arg::i16(0), joan::orig::arg::i16(flags)));  // 0x006360f0
                            } else {
                                joan::orig::as_billboard(target)->blit_more(joan::orig::arg::i16(x), joan::orig::arg::i16(gy), joan::orig::arg::i16(x2), joan::orig::arg::i16(y2), joan::orig::arg::u32(colour), joan::orig::arg::mptr(glyph_texture), joan::orig::arg::i32(first_blit), joan::orig::arg::i16(jo::as_font_glyph(g2)->x0), joan::orig::arg::i16(jo::as_font_glyph(g2)->y0), joan::orig::arg::i16(jo::as_font_glyph(g2)->x1), joan::orig::arg::i16(jo::as_font_glyph(g2)->y1), joan::orig::arg::i16(0));  // 0x00636640
                            }
                            joan::orig::as_texture_manager(img::obj(kTextures))->release(joan::orig::arg::mptr(jo::self<jo::Font>(this)->texture));  // 0x0060a550
                            jo::self<jo::Font>(this)->texture = nullptr;
                        }
                        x = static_cast<std::int16_t>(x + gw);
                    }
                } else {
                    const std::int16_t space = scaled(jo::self<jo::Font>(this)->space_width, scale_x);
                    if (ch == 9) x = static_cast<std::int16_t>(x + static_cast<std::int16_t>(static_cast<std::int32_t>(space) << 3));
                    else x = static_cast<std::int16_t>(x + space);
                }
            }
            x = static_cast<std::int16_t>(x + scaled(jo::self<jo::Font>(this)->spacing, scale_x));
            cursor_next(&it);
            ++i;
        }
        return static_cast<std::int16_t>(static_cast<std::int32_t>(x) - 1);
    }
};

#define U1(name, addr, fn) JOAN_REPLACE("game/ui1", name, addr, fn)
U1("Font::measure", 0x00624f00, &joan::orig::Font::measure);
U1("Font::draw_glyphs", 0x00625c50, &H::draw_glyphs);
#undef U1

}  // namespace game_ui1
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
namespace {
namespace game_leaves80b {

using joan::hook::x87::Fpu;

using joan::img::obj;

constexpr img::Addr kFiles(d::g_misc);

using joan::hook::global;


JOAN_COUNTER(c_text_height, "Font::text_height");

struct H {
    // 0x006253f0: both products are int * float rounded to float (PC24), passed as double to
    // the rounding helper 0x00590a20.
    std::int16_t text_height(std::int16_t lines, float scale) {
        c_text_height.hit();
        const joan::hook::x87::Fpu fpu{true};
        const double h = fpu.mul(static_cast<double>(static_cast<std::int32_t>(jo::self<jo::Font>(this)->line_height)), static_cast<double>(scale));
        std::int32_t sum = static_cast<std::int32_t>(static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(img::obj(kFiles))->round(joan::orig::arg::f64(h)))));  // 0x00590a20
        sum = sum * static_cast<std::int32_t>(global<std::int16_t>(d::g_text_lines));
        const double s = fpu.mul(static_cast<double>(static_cast<std::int32_t>(lines)), static_cast<double>(scale));
        const std::int32_t gap = static_cast<std::int32_t>(static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(img::obj(kFiles))->round(joan::orig::arg::f64(s)))));
        sum = static_cast<std::int32_t>(static_cast<std::uint32_t>(sum) +
                                        static_cast<std::uint32_t>(gap * (static_cast<std::int32_t>(global<std::int16_t>(d::g_text_lines)) - 1)));
        return static_cast<std::int16_t>(sum);
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("Font::text_height", 0x006253f0, &H::text_height);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
namespace {
namespace game_b102_12 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using u8 = std::uint8_t;
using u16 = std::uint16_t;


JOAN_COUNTER(c_add_text_at, "Panel::add_text_at");
JOAN_COUNTER(c_add_text_centered, "Panel::add_text_centered");

struct H {
    // ------------------------------------------------------------------ 0x00625490
    u32 add_text_at(u16 a1, u16 a2, u32 a3, u16 font, u32 a5, u16 a6, u16 a7, u16 a8, u16 a9, u16 a10, u16 a11) {
        c_add_text_at.hit();
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_font(this)->glyph(joan::orig::arg::u16(font)));  // 0x00627e80
        if (p == nullptr) return 0;
        jo::TextureManager* const tm = joan::orig::as_texture_manager(obj(d::g_texture_array));
        const u32 old = joan::orig::arg::as<u32>(tm->mode());  // 0x0060a720
        tm->set_mode(1u);              // 0x0060a740
        jo::self<jo::Font>(this)->texture = joan::orig::arg::as<jo::Texture*>(tm->load(joan::orig::arg::str(static_cast<void*>(jo::self<jo::Font>(this)->name)), 1, 0u));  // 0x006099c0
        tm->set_mode(joan::orig::arg::u32(old));  // 0x0060a740
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->blit_image(joan::orig::arg::i16(a1), joan::orig::arg::i16(a2), joan::orig::arg::u32(a3), joan::orig::arg::mptr(joan::cjk::texture_for(p, jo::self<jo::Font>(this)->texture)), joan::orig::arg::u32(a5), joan::orig::arg::i16(a6), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(jo::view<u16>(jo::as_font_glyph(p)->x0)), joan::orig::arg::i16(jo::view<u16>(jo::as_font_glyph(p)->y0)), joan::orig::arg::i16(jo::view<u16>(jo::as_font_glyph(p)->x1)), joan::orig::arg::i16(jo::view<u16>(jo::as_font_glyph(p)->y1)), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), joan::orig::arg::i16(a11));  // 0x006332b0
        tm->release(joan::orig::arg::mptr(jo::self<jo::Font>(this)->texture));  // 0x0060a550
        jo::self<jo::Font>(this)->texture = nullptr;
        return 0;
    }

    // ------------------------------------------------------------------ 0x00626260
    u32 add_text_centered(u16 a1, i16 a2, i16 a3, u32 a4, const char* a5, u32 a6, u16 a7, u16 a8, u16 a9, u16 a10, u16 a11,
                          u32 a12, float scale) {
        c_add_text_centered.hit();
        i32 s = static_cast<i32>(a2) + static_cast<i32>(a3);
        // fild (word +0x32) * float: both exact in float, one rounding.
        const float w = static_cast<float>(static_cast<i32>(jo::self<jo::Text>(this)->glyph_height)) * scale;
        const i16 r = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(w)))));  // 0x00590a20
        s = s - static_cast<i32>(r) + 1;
        const i32 half = s / 2;  // cdq; sub; sar
        return joan::orig::arg::as<u32>(joan::orig::as_text(this)->draw_line(joan::orig::arg::i32(a1), joan::orig::arg::i32(static_cast<u16>(half)), joan::orig::arg::u32(a4), a5, joan::orig::arg::u32(a6), joan::orig::arg::i32(a7), joan::orig::arg::i32(a8), joan::orig::arg::i32(a9), joan::orig::arg::i32(a10), joan::orig::arg::i32(a11), joan::orig::arg::f32(a12), joan::orig::arg::f32(scale)));  // 0x006255d0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("Panel::add_text_at", 0x00625490, &H::add_text_at);
B("Panel::add_text_centered", 0x00626260, &H::add_text_centered);
#undef B

}  // namespace game_b102_12
}  // namespace

// ---------------------------------------- from modules/game_b116_11.cpp (game/b116_11)
namespace {
namespace game_b116_11 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::hook::global;

using u8 = std::uint8_t;
using u16 = std::uint16_t;

using i8 = std::int8_t;


[[maybe_unused]] constexpr img::Addr kScreenW = d::g_screen_w, kScreenH = d::g_screen_h;
constexpr img::Addr kRounder = d::g_misc;   // 0x00590a20's object
constexpr img::Addr kTextures = d::g_texture_array;  // texture manager
constexpr img::Addr kSprites = d::g_sprites_a46128;   // 2D layer (sprite queue owner)
constexpr img::Addr kOverflow = d::g_help_box;  // draws the "..." box of a clipped line
constexpr img::Addr kLineNo = d::g_lineNo, kLine2 = d::g_line2, kEndPtr = d::g_endPtr, kEndY = d::g_endY;

// round(v * s): fild v; fmul s (PC24, v exact in float); fstp qword -> 0x00590a20 (ax used).
i16 scaled(i32 v, float s) {
    return static_cast<i16>(joan::orig::arg::as<i32>(joan::orig::as_misc(obj(kRounder))->round(joan::orig::arg::f64(static_cast<double>(static_cast<float>(v) * s)))));  // 0x00590a20
}

// String iterator helpers (iterator objects of one pointer: 4 bytes on x86).
void s_ctor(std::uintptr_t* s, const char* p) { joan::orig::as_text_cursor(s)->construct(joan::orig::arg::str(p)); }             // 0x0058b9c0
void s_ctor0(std::uintptr_t* s) { joan::orig::as_text_cursor(s)->construct_null(); }                      // 0x0058b990
u32 s_ch(std::uintptr_t* s) { return joan::orig::arg::as<u32>(joan::orig::as_text_cursor(s)->get()); }                    // 0x0058ba70
void s_next(std::uintptr_t* s) { joan::orig::as_text_cursor(s)->next(); }                       // 0x0058bbc0
i32 s_count(std::uintptr_t* s) { return joan::orig::arg::as<i32>(joan::orig::as_text_cursor(s)->length()); }                 // 0x0058c190
char* s_ptr(std::uintptr_t* s) { return joan::orig::as_text_iter(s)->get(); }                   // 0x0058ba50 (the current char*)
void s_post(std::uintptr_t* s, std::uintptr_t* out) { joan::orig::as_text_cursor(s)->postinc(joan::orig::arg::mptr(out), joan::orig::arg::i32(u32{0})); }  // 0x0058bbf0 (s++)
// `dst = src` through a by-value copy (0x0058b9f0 into the argument slot, then 0x0058ba20).
void s_set(std::uintptr_t* dst, std::uintptr_t* src) {
    char* tmp = nullptr;
    joan::orig::as_text_iter(&tmp)->TextIter_copy(joan::orig::arg::mptr(src));  // 0x0058b9f0
    joan::orig::as_text_iter(dst)->op_assign(tmp);   // 0x0058ba20
}
// dst < src (0x0058bfe0, by value; al).
bool s_less(std::uintptr_t* dst, std::uintptr_t* src) {
    char* tmp = nullptr;
    joan::orig::as_text_iter(&tmp)->TextIter_copy(joan::orig::arg::mptr(src));                     // 0x0058b9f0
    return static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_text_iter(dst)->less(tmp))) != 0;  // 0x0058bfe0
}

// One glyph: texture of the font (0x006099c0 with the texture manager's mode temporarily 1), the
// sprite queue call, release of the texture (0x0060a550).
void draw_glyph(void* font, i32 x1, i32 y1, i32 x2, i32 y2, u32 layer, u32 colour, i32 m1, i32 m2, i32 m3, const u8* g) {
    const u32 mode = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(obj(kTextures))->mode());                                  // 0x0060a720
    joan::orig::as_texture_manager(obj(kTextures))->set_mode(joan::orig::arg::u32(u32{1}));                                          // 0x0060a740
    jo::as_font2d(font)->texture = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(obj(kTextures))->load(joan::orig::arg::str(jo::as_font2d(font)->name), joan::orig::arg::i32(u32{1}), joan::orig::arg::u32(u32{0})));  // 0x006099c0
    joan::orig::as_texture_manager(obj(kTextures))->set_mode(joan::orig::arg::u32(mode));                                            // 0x0060a740
    joan::orig::as_sprite2d(obj(kSprites))->queue(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(layer), joan::orig::arg::u32(colour), joan::orig::arg::mptr(joan::cjk::texture_for(g, jo::as_font2d(font)->texture)), i32{3}, joan::orig::arg::i32(m1), joan::orig::arg::i32(m2), joan::orig::arg::i32(m3), joan::orig::arg::i32(jo::as_font_glyph(g)->x0), joan::orig::arg::i32(jo::as_font_glyph(g)->y0), joan::orig::arg::i32(jo::as_font_glyph(g)->x1), joan::orig::arg::i32(jo::as_font_glyph(g)->y1), i32{0});  // 0x00632a70
    joan::orig::as_texture_manager(obj(kTextures))->release(joan::orig::arg::mptr(jo::as_font2d(font)->texture));  // 0x0060a550
    jo::as_font2d(font)->texture = nullptr;
}

JOAN_COUNTER(c_draw, "Text::draw");
JOAN_COUNTER(c_draw_line, "Text::draw_line");
JOAN_COUNTER(c_draw_2, "Text::draw_2");
JOAN_COUNTER(c_draw_3, "Text::draw_3");

struct H {
    u8* glyph(i32 ch) { return joan::orig::arg::as<u8*>(joan::orig::as_font(this)->glyph(joan::orig::arg::u16(ch))); }  // 0x00627e80 (low word)
    i16 width(const char* text, i32 mode, float sx) {                           // 0x00624f00
        return static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_font(this)->measure(joan::orig::arg::str(text), -1, 0, joan::orig::arg::i16(mode), joan::orig::arg::f32(sx))));
    }

    // 0x006255d0
    u32 draw_line(i32 x_arg, i32 y_arg, u32 layer, const char* text, u32 colour, i32 right_arg, i32 mode_arg, i32 m1, i32 m2, i32 m3,
                  float sx, float sy) {
        joan::canvas::ScreenScope canvas_scope(0x006255d0);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_draw_line.hit();
        i16 x = static_cast<i16>(x_arg);
        if (jo::view<i8>(jo::self<jo::Text>(this)->loaded) == 0) return (joan::call::register_low_bits(this) & 0xffff0000u) | static_cast<u16>(x);
        const i32 y = static_cast<i16>(y_arg), mode = static_cast<i16>(mode_arg);
        i16 right = static_cast<i16>(right_arg);
        std::uintptr_t it = 0;  // a TextCursor (one pointer)
        s_ctor(&it, text);
        const i32 count = s_count(&it);
        bool clip = false;
        i32 limit = 0;
        i16 dots = 0;
        const i16 x0 = x;
        if (right < 0 || right >= global<i32>(kScreenW)) {
            right = static_cast<i16>(global<i32>(kScreenW) - 1);
        } else {
            const i32 w = width(text, mode, sx);
            if (w > right - x + 1) {
                clip = true;
                u8* const dot = glyph(0x2e);
                const i32 a = scaled(jo::as_font_glyph(dot)->width, sx);
                const i32 b = scaled(jo::view<i16>(jo::self<jo::Text>(this)->spacing), sx);
                const i16 dot_w = static_cast<i16>(a + b);
                const i32 t = right - dot_w * 3;
                limit = t < 0 ? 0 : t;
            }
        }
        for (i32 i = 1;; ++i) {
            if (i != 1) s_next(&it);
            u32 c = s_ch(&it);
            if (c == 0 || i > count || x > right) break;
            if (mode == 1) c = reinterpret_cast<u32(JOAN_CDECL*)(u32)>(fn::code_value(fn::text_toupper))(c);  // 0x0058e380
            if (clip) {
                if (dots != 0 || x > limit) {
                    c = 0x2e;
                    dots = static_cast<i16>(dots + 1);
                } else if (u8* const g = glyph(static_cast<i32>(c))) {
                    const i16 gw = scaled(jo::as_font_glyph(g)->width, sx);
                    if (dots != 0 || x + gw - 1 > limit) {
                        c = 0x2e;
                        dots = static_cast<i16>(dots + 1);
                    }
                }
                if (dots > 3) break;
            }
            if (c == 0x20) {
                const i16 space = scaled(jo::self<jo::Text>(this)->space_width, sx);
                if (x + space - 1 > right) break;
                x = static_cast<i16>(x + space);
            } else if (u8* const g = glyph(static_cast<i32>(c))) {
                const i16 gw = scaled(jo::as_font_glyph(g)->width, sx);
                const i16 gh = scaled(jo::as_font_glyph(g)->height, sy);
                const i16 gy = static_cast<i16>(y + scaled(jo::as_font_glyph(g)->top, sy));
                if (x + gw - 1 > right) break;
                if (gw > 0) {
                    if (u8* const g2 = glyph(static_cast<i32>(c)))
                        draw_glyph(this, x, gy, x + gw - 1, gy + gh - 1, layer, colour, static_cast<i16>(m1), static_cast<i16>(m2),
                                   static_cast<i16>(m3), g2);
                    x = static_cast<i16>(x + gw);
                }
            } else {
                const i16 space = scaled(jo::self<jo::Text>(this)->space_width, sx);
                if (c == 9) x = static_cast<i16>(x + static_cast<i16>(space << 3));
                else x = static_cast<i16>(x + space);
            }
            x = static_cast<i16>(x + scaled(jo::view<i16>(jo::self<jo::Text>(this)->spacing), sx));
        }
        if (dots != 0) {
            const i16 h = scaled(jo::self<jo::Text>(this)->line_height, sx);  // the original scales the height by sx here
            joan::orig::as_help_box(obj(kOverflow))->set2(static_cast<i32>(x0), joan::orig::arg::i32(y), static_cast<i32>(right), joan::orig::arg::i32(y + h), joan::orig::arg::str(text), joan::orig::arg::str(u32{0}), i32{-1}, i32{-1});  // 0x00489b30
        }
        return static_cast<u32>(static_cast<i32>(x) - 1);
    }

    // 0x00626080
    u32 draw_2(i32 x_arg, i32 y_arg, u32 layer, const char* text, u32 colour, i32 mode_arg, i32 m1, i32 m2, i32 m3, float sx, float sy) {
        joan::canvas::ScreenScope canvas_scope(0x00626080);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_draw_2.hit();
        const i32 x = static_cast<i16>(x_arg);
        const i32 w = width(text, static_cast<i16>(mode_arg), sx);
        joan::orig::as_text(this)->draw_line(joan::orig::arg::i32(x - w + 1), static_cast<i32>(static_cast<i16>(y_arg)), joan::orig::arg::u32(layer), text, joan::orig::arg::u32(colour), joan::orig::arg::i32(x), static_cast<i32>(static_cast<i16>(mode_arg)), static_cast<i32>(static_cast<i16>(m1)), static_cast<i32>(static_cast<i16>(m2)), static_cast<i32>(static_cast<i16>(m3)), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x006255d0
        return static_cast<u32>(x - w + 1);
    }

    // 0x00626130
    u32 draw_3(i32 x1_arg, i32 y1_arg, i32 x2_arg, i32 y2_arg, u32 layer, const char* text, u32 colour, i32 mode_arg, i32 m1, i32 m2, i32 m3,
               float sx, float sy) {
        joan::canvas::ScreenScope canvas_scope(0x00626130);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_draw_3.hit();
        const i32 x1 = static_cast<i16>(x1_arg), y1 = static_cast<i16>(y1_arg);
        const i32 x2 = static_cast<i16>(x2_arg), y2 = static_cast<i16>(y2_arg);
        const i32 w = width(text, static_cast<i16>(mode_arg), sx);
        i32 cx = x1 + (x2 - x1 + 1 - w) / 2;
        const i32 h = scaled(jo::self<jo::Text>(this)->line_height, sy);
        const i32 cy = y1 + (y2 - y1 + 1 - h) / 2;
        if (cx < x1) cx = x1;
        if (cx < 0) cx = 0;
        return joan::orig::arg::as<u32>(joan::orig::as_text(this)->draw_line(static_cast<i32>(static_cast<i16>(cx)), static_cast<i32>(static_cast<i16>(cy)), joan::orig::arg::u32(layer), text, joan::orig::arg::u32(colour), joan::orig::arg::i32(x2), static_cast<i32>(static_cast<i16>(mode_arg)), static_cast<i32>(static_cast<i16>(m1)), static_cast<i32>(static_cast<i16>(m2)), static_cast<i32>(static_cast<i16>(m3)), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy)));  // 0x006255d0
    }

    // 0x00626320
    u32 draw(i32 x_arg, i32 y_arg, i32 right_arg, i32 bottom_arg, u32 layer, const char* text, u32 colour, i32 spacing_arg, i32 line_arg,
             i32 visible_arg, i32 m1, i32 m2, i32 m3, float sx, float sy, i32 marks_arg, i32 max_arg) {
        joan::canvas::ScreenScope canvas_scope(0x00626320);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_draw.hit();
        if (jo::view<i8>(jo::self<jo::Text>(this)->loaded) == 0) return joan::call::register_low_bits(this) & 0xffff0000u;  // eax residue: no caller of Text::draw reads the result
        const i32 x = static_cast<i16>(x_arg), y = static_cast<i16>(y_arg);
        const i32 right = static_cast<i16>(right_arg), bottom = static_cast<i16>(bottom_arg);
        const i32 spacing = static_cast<i16>(spacing_arg);
        if (y + scaled(jo::self<jo::Text>(this)->line_height, sy) - 1 > bottom) return static_cast<u32>(bottom) & 0xffff0000u;
        const i32 visible = static_cast<i16>(visible_arg), max_glyphs = static_cast<i16>(max_arg);
        const i32 s1 = static_cast<i16>(m1), s2 = static_cast<i16>(m2), s3 = static_cast<i16>(m3);
        const bool marks = static_cast<i8>(marks_arg) != 0;
        i16 line = static_cast<i16>(line_arg);
        std::uintptr_t it = 0, it2 = 0, post1 = 0, post2 = 0;  // TextCursors (one pointer each)
        s_ctor(&it, text);
        s_ctor0(&it2);
        i8 hl = 0;
        i16 drawn = 1;
        const u32 saved = colour;
        i32 cx = x, cy = y, line_x = x;
        s_set(&it2, &it);
        bool wrapped = false;
        global<i16>(kLineNo) = 0;
        global<i16>(kLine2) = 0;
        i16& line_no = global<i16>(kLineNo);
        for (;;) {
            if (line < 1) {
                cy += scaled(jo::self<jo::Text>(this)->line_height + spacing, sy);
                line = static_cast<i16>(line + 1);
                continue;
            }
            if (wrapped) {
                if (cx - 1 - scaled(jo::view<i16>(jo::self<jo::Text>(this)->spacing), sx) > right) {
                    if (line_no >= line - 1) {
                        cy += scaled(jo::self<jo::Text>(this)->line_height + spacing, sy);
                        if (cy + scaled(jo::self<jo::Text>(this)->line_height, sy) - 1 > bottom) break;
                    }
                    cx = x;
                    line_no = static_cast<i16>(line_no + 1);
                } else {
                    cx = line_x;
                }
                // Draw the word [it2, it).
                for (;;) {
                    if (!s_less(&it2, &it) || s_ch(&it2) == 0 || s_ch(&it2) == 0x1a) break;
                    const i16 ch = static_cast<i16>(s_ch(&it2));
                    bool stop = false;
                    do {
                        if (marks) {
                            if (ch == 0x5f) {
                                hl = static_cast<i8>(hl + 1);
                                colour = (global<u32>(d::k_9da69c) & 0x00ffffffu) | (saved & 0xff000000u);
                                break;
                            }
                            if (hl == 2) {
                                colour = saved;
                                hl = 0;
                            }
                        }
                        if (ch == 0x20) {
                            const i16 space = scaled(jo::self<jo::Text>(this)->space_width, sx);
                            if (cx + space - 1 > right) {
                                stop = true;
                                break;
                            }
                            cx += space;
                            break;
                        }
                        if (ch == 9) {
                            const i16 space = scaled(jo::self<jo::Text>(this)->space_width, sx);
                            if (cx + space * 8 - 1 > right) {
                                stop = true;
                                break;
                            }
                            cx += space * 8;
                            break;
                        }
                        if (ch == 0xd) break;
                        u8* const g = glyph(ch);
                        if (!g) break;
                        const i16 gw = scaled(jo::as_font_glyph(g)->width, sx);
                        const i16 gh = scaled(jo::as_font_glyph(g)->height, sy);
                        const i16 gy = static_cast<i16>(cy + scaled(jo::as_font_glyph(g)->top, sy));
                        if (right >= 0 && cx + gw - 1 > right) {
                            stop = true;
                            break;
                        }
                        if (gw <= 0) break;
                        if (visible != 0 && line_no >= line - 1) {
                            if (u8* const g2 = glyph(ch)) {
                                if (max_glyphs == -1 || drawn <= max_glyphs)
                                    draw_glyph(this, cx, gy, cx + gw - 1, gy + gh - 1, layer, colour, s1, s2, s3, g2);
                                drawn = static_cast<i16>(drawn + 1);
                            }
                        }
                        cx += gw;
                    } while (false);
                    if (stop) break;
                    s_next(&it2);
                    cx += scaled(jo::view<i16>(jo::self<jo::Text>(this)->spacing), sx);
                }
                if (s_ch(&it) == 0xa) {
                    if (line_no >= line - 1) {
                        cy += scaled(jo::self<jo::Text>(this)->line_height + spacing, sy);
                        if (cy + scaled(jo::self<jo::Text>(this)->line_height, sy) - 1 > bottom) break;
                    }
                    cx = x;
                    s_post(&it, &post1);
                    line_no = static_cast<i16>(line_no + 1);
                }
                if (s_ch(&it) == 0 || s_ch(&it) == 0x1a) break;
                s_set(&it2, &it);
                line_x = cx;
                wrapped = false;
            }
            // Measure the next character of the current word.
            if (s_ch(&it) == 0x20 || s_ch(&it) == 9 || s_ch(&it) == 0xa || s_ch(&it) == 0 || s_ch(&it) == 0x1a) {
                wrapped = true;
            } else {
                const i16 ch = static_cast<i16>(s_ch(&it));
                if (ch == 0x20) {
                    cx += scaled(jo::self<jo::Text>(this)->space_width, sx);
                } else if (ch == 9) {
                    cx += scaled(jo::self<jo::Text>(this)->space_width, sx) * 8;
                } else if (u8* const g = glyph(ch)) {
                    cx += scaled(jo::as_font_glyph(g)->width, sx);
                } else {
                    cx += scaled(jo::self<jo::Text>(this)->space_width, sx);
                }
                cx += scaled(jo::view<i16>(jo::self<jo::Text>(this)->spacing), sx);
            }
            if (s_ch(&it) != 0 && s_ch(&it) != 0x1a && s_ch(&it) != 0xa) s_post(&it, &post2);
        }
        global<char*>(kEndPtr) = s_ptr(&it2);  // pointer slot (char* kEndPtr_a69ec4, 8 bytes on x64)
        const i16 h = scaled(jo::self<jo::Text>(this)->line_height + spacing, sy);
        global<i16>(kEndY) = static_cast<i16>(cy + h);
        line_no = static_cast<i16>(line_no + 1);
        return (static_cast<u32>(static_cast<i32>(h)) & 0xffff0000u) | 1u;
    }
};

#define B(name, address, fn) JOAN_REPLACE("game/b116_11", name, address, fn)
B("Text::draw", 0x00626320, &H::draw);
JOAN_CANVAS_SELF(0x00626320);  // ScreenScope in the body (no thunk wrapper)
B("Text::draw_line", 0x006255d0, &H::draw_line);
JOAN_CANVAS_SELF(0x006255d0);  // ScreenScope in the body (no thunk wrapper)
B("Text::draw_2", 0x00626080, &H::draw_2);
JOAN_CANVAS_SELF(0x00626080);  // ScreenScope in the body (no thunk wrapper)
B("Text::draw_3", 0x00626130, &H::draw_3);
JOAN_CANVAS_SELF(0x00626130);  // ScreenScope in the body (no thunk wrapper)
#undef B

}  // namespace game_b116_11
}  // namespace

// ---------------------------------------- from modules/game_b82_11.cpp (game/b82_11)
namespace {
namespace game_b82_11 {

using joan::hook::x87::Fpu;

using joan::call::thiscall;
using joan::img::obj;

using joan::call::cdecl_call;


constexpr img::Addr kMath = d::g_misc;     // 0x00590a20 (round) / 0x00591050 (time)

JOAN_COUNTER(c_text_box, "TextRenderer::draw_wrapped");

std::int32_t s16(std::uint32_t v) { return static_cast<std::int16_t>(v); }
std::int32_t s8(std::uint32_t v) { return static_cast<std::int8_t>(v); }

// fild int ; fmul float (PC24) ; fstp qword ; 0x00590a20 on 0x00a45fa8 (ret 8) ; movsx ax.
std::int32_t scaled(std::int32_t v, float s) {
    const double d = Fpu{true}.mul(static_cast<double>(v), static_cast<double>(s));
    return static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(obj(kMath))->round(joan::orig::arg::f64(d))));  // 0x00590a20
}

// char* iterator of the text library (one pointer: 4 bytes on x86, trivially copyable).
struct Iter {
    const char* p;
};
static_assert(sizeof(Iter) == sizeof(void*));

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"  // an output buffer behind a facade's const char* parameter
#endif
void iter_init(Iter* it, const char* s) { joan::orig::as_text_cursor(it)->construct(joan::orig::arg::str(s)); }  // 0x0058b9c0
Iter iter_copy(const Iter* src) {
    Iter t;
    joan::orig::as_text_iter(&t)->TextIter_copy(joan::orig::arg::mptr(src));  // 0x0058b9f0
    return t;
}
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
Iter iter_null() {
    Iter t;
    iter_init(&t, nullptr);
    return t;
}
void iter_assign(Iter* dst, Iter v) { joan::orig::as_text_iter(dst)->op_assign(const_cast<char*>(v.p)); }  // 0x0058ba20 (Iter by value = its one dword)
bool iter_valid(const Iter* it) { return (joan::orig::arg::as<std::uint32_t>(joan::orig::as_text_iter(it)->valid()) & 0xff) != 0; }  // 0x0058ce80
void iter_next(Iter* it) { joan::orig::as_text_cursor(it)->next(); }  // 0x0058bbc0

struct H {
    // ---------------------------------------------------------------- 0x00626e00
    void draw_wrapped(std::uint32_t ax, std::uint32_t ay, std::uint32_t ax2, std::uint32_t ay2, std::uint32_t layer,
                      const char* text, std::uint32_t colour, std::uint32_t spacing, std::uint32_t mode, std::uint32_t a,
                      std::uint32_t b, std::uint32_t c, float sx, float sy, std::uint32_t align) {
        c_text_box.hit();
        // [ebp-0x20c], 0x200 bytes in the original. Lines are cut by width, so a copied range
        // never comes near that size; the extra room only keeps a pathological overflow away
        // from the other locals.
        char buffer[0x400];
        Iter out;  // [ebp-0x210]
        iter_init(&out, buffer);
        std::int32_t x = 0, y = 0;            // [ebp-0x8], [ebp-0xc]
        std::int32_t line_x = 0, word_x = 0;  // [ebp-0x224], [ebp-0x22c] (always written before use)
        const std::int32_t al = s8(align);

        // Copies [seg, end) into the buffer (0x0058cdb0) and draws it.
        auto draw_range = [&](const Iter* seg, const Iter* end) {
            const Iter e = iter_copy(end);
            const Iter s = iter_copy(seg);
            Iter ret;
            joan::orig::as_text_cursor(&out)->assign_range(&ret, s, e);  // 0x0058cdb0
            if (al == 2) {
                joan::orig::as_text(this)->draw_line(joan::orig::arg::i32(s16(ax) + (s16(ax2) - line_x) / 2), joan::orig::arg::i32(y), joan::orig::arg::u32(layer), buffer, joan::orig::arg::u32(colour), joan::orig::arg::i32(ax2), joan::orig::arg::i32(mode), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x006255d0
            } else if (al == 1) {
                joan::orig::as_text(this)->draw_line(joan::orig::arg::i32(ax), joan::orig::arg::i32(y), joan::orig::arg::u32(layer), buffer, joan::orig::arg::u32(colour), joan::orig::arg::i32(ax2), joan::orig::arg::i32(mode), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x006255d0
            } else if (al == 3) {
                joan::orig::as_text(this)->draw_2(joan::orig::arg::i32(ax2), joan::orig::arg::i32(y), joan::orig::arg::u32(layer), buffer, joan::orig::arg::u32(colour), joan::orig::arg::i32(mode), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x00626080
            }
        };

        for (std::int32_t pass = 0; pass <= 1; ++pass) {
            if (pass == 0) {
                y = s16(ay);
            } else {
                std::int32_t t = s16(ay2) + 1 - y;
                t -= scaled(s16(jo::view<std::uint16_t>(jo::self<jo::Font2d>(this)->glyph_height)), sy);
                y = s16(ay) + t / 2;
                y = y > s16(ay) ? y : s16(ay);
            }
            Iter cur;  // [ebp-0x218]
            iter_init(&cur, text);
            x = s16(ax);
            Iter seg = iter_copy(&cur);  // [ebp-0x21c]
            Iter line;                   // [ebp-0x220]
            iter_init(&line, nullptr);
            Iter word;  // [ebp-0x228]
            iter_init(&word, nullptr);
            std::int32_t joined = 0;  // [ebp-0x230]
            for (;;) {
                std::int32_t ch = joan::orig::arg::as<std::int32_t>(joan::orig::as_text_cursor(&cur)->get());  // 0x0058ba70
                if (s16(mode) == 1) ch = joan::orig::arg::as<std::int32_t>(joan::orig::text_toupper(joan::orig::arg::i32(ch)));  // 0x0058e380
                if (ch == 10) {
                    if (joined == 0) {
                        iter_assign(&line, iter_copy(&cur));
                        line_x = x;
                    }
                    if (pass != 0 && iter_valid(&seg)) draw_range(&seg, &line);
                    iter_assign(&seg, iter_null());
                    iter_assign(&line, iter_null());
                    iter_assign(&word, iter_null());
                    y += scaled(s16(jo::view<std::uint16_t>(jo::self<jo::Font2d>(this)->line_height)) + s16(spacing), sy);
                    x = s16(ax);
                    joined = 0;
                    if (y + scaled(s16(jo::view<std::uint16_t>(jo::self<jo::Font2d>(this)->line_height)), sy) > s16(ay2)) break;
                } else if (ch == 13) {
                } else if (ch == 32) {
                    const std::int32_t space = scaled(s16(jo::view<std::uint16_t>(jo::self<jo::Font2d>(this)->space_width)), sx);  // [ebp-0x23c]
                    if (joined == 0) {
                        iter_assign(&line, iter_copy(&cur));
                        line_x = x;
                    }
                    if (x + space > s16(ax2)) {
                        iter_assign(&word, iter_copy(&cur));
                        word_x = x;
                        if (pass != 0 && iter_valid(&seg)) draw_range(&seg, &line);
                        iter_assign(&seg, iter_null());
                        iter_assign(&line, iter_null());
                        iter_assign(&word, iter_null());
                        y += scaled(s16(jo::view<std::uint16_t>(jo::self<jo::Font2d>(this)->line_height)) + s16(spacing), sy);
                        x = s16(ax);
                        joined = 0;
                        if (y + scaled(s16(jo::view<std::uint16_t>(jo::self<jo::Font2d>(this)->line_height)), sy) > s16(ay2)) break;
                    } else if (iter_valid(&seg)) {
                        const std::int32_t gap = scaled(s16(jo::self<jo::Font2d>(this)->spacing), sx);
                        x += space + gap;
                        iter_assign(&word, iter_copy(&cur));
                        word_x = x;
                        joined = 1;
                    }
                } else {
                    void* const glyph = joan::orig::arg::as<void*>(joan::orig::as_font(this)->glyph(joan::orig::arg::u16(static_cast<std::uint32_t>(s16(ch)))));  // 0x00627e80
                    if (glyph == nullptr && ch == 0) {
                        // End of the text: the last line straight from the text.
                        if (pass != 0 && iter_valid(&seg)) {
                            if (al == 2) {
                                const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_text_iter(&seg)->get());  // 0x0058ba50
                                joan::orig::as_text(this)->draw_line(joan::orig::arg::i32(s16(ax) + (s16(ax2) - x) / 2), joan::orig::arg::i32(y), joan::orig::arg::u32(layer), s, joan::orig::arg::u32(colour), joan::orig::arg::i32(ax2), joan::orig::arg::i32(mode), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x006255d0
                            } else if (al == 1) {
                                const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_text_iter(&seg)->get());  // 0x0058ba50
                                joan::orig::as_text(this)->draw_line(joan::orig::arg::i32(ax), joan::orig::arg::i32(y), joan::orig::arg::u32(layer), s, joan::orig::arg::u32(colour), joan::orig::arg::i32(ax2), joan::orig::arg::i32(mode), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x006255d0
                            } else if (al == 3) {
                                const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_text_iter(&seg)->get());  // 0x0058ba50
                                joan::orig::as_text(this)->draw_2(joan::orig::arg::i32(ax2), joan::orig::arg::i32(y), joan::orig::arg::u32(layer), s, joan::orig::arg::u32(colour), joan::orig::arg::i32(mode), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::f32(sx), joan::orig::arg::f32(sy));  // 0x00626080
                            }
                        }
                        if (!iter_valid(&seg)) y -= scaled(s16(jo::view<std::uint16_t>(jo::self<jo::Font2d>(this)->line_height)) + s16(spacing), sy);
                        joined = 0;
                        break;
                    }
                    std::int32_t width = scaled(s16(jo::view<std::uint16_t>(jo::self<jo::Font2d>(this)->space_width)), sx);  // [ebp-0x240]
                    if (glyph != nullptr) width = scaled(s16(jo::view<std::uint16_t>(jo::as_font_glyph(glyph)->width)), sx);
                    else if (ch == 9) width = static_cast<std::int32_t>(static_cast<std::uint32_t>(scaled(s16(jo::view<std::uint16_t>(jo::self<jo::Font2d>(this)->space_width)), sx)) << 3);
                    if (!iter_valid(&seg)) iter_assign(&seg, iter_copy(&cur));
                    if (x + width > s16(ax2)) {
                        if (!iter_valid(&line)) {
                            iter_assign(&line, iter_copy(&cur));
                            line_x = x;
                        }
                        if (!iter_valid(&word)) {
                            iter_assign(&word, iter_copy(&cur));
                            word_x = x;
                        } else {
                            iter_next(&word);
                        }
                        if (pass != 0 && iter_valid(&seg)) draw_range(&seg, &line);
                        y += scaled(s16(jo::view<std::uint16_t>(jo::self<jo::Font2d>(this)->line_height)) + s16(spacing), sy);
                        x = s16(ax) + x - word_x;
                        iter_assign(&seg, iter_copy(&word));
                        iter_assign(&line, iter_null());
                        iter_assign(&word, iter_null());
                        if (y + scaled(s16(jo::view<std::uint16_t>(jo::self<jo::Font2d>(this)->line_height)), sy) > s16(ay2)) break;
                    }
                    const std::int32_t gap = scaled(s16(jo::self<jo::Font2d>(this)->spacing), sx);
                    x += width + gap;
                    joined = 0;
                }
                iter_next(&cur);
            }
        }
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b82_11", name, addr, fn)
B11("TextRenderer::draw_wrapped", 0x00626e00, &H::draw_wrapped);
#undef B11

}  // namespace game_b82_11
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
namespace {
namespace game_b92_05_2 {

using joan::call::thiscall;

using joan::hook::global;

JOAN_COUNTER(c_measure_lines, "TextBox::measure_lines");

struct H {
    // 0x00627d80
    void measure_lines(std::int32_t w0, std::int32_t w1, std::int32_t w2, std::int32_t w3, const char* text, std::int32_t w1c,
                       std::int32_t b20, std::int16_t* height, std::int16_t* first, std::uint32_t f2c, std::uint32_t f30) {
        c_measure_lines.hit();
        *first = 0;
        *height = 0;
        while (static_cast<std::int32_t>(static_cast<std::int8_t>(*text)) != 0 &&
               static_cast<std::int32_t>(static_cast<std::int8_t>(*text)) != 0x1a) {
            std::uint32_t color_obj;
            const std::uint32_t color = *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_color(&color_obj)->Color_dword(joan::orig::arg::u32(-1)));  // 0x005e9640
            joan::orig::as_text(this)->draw(static_cast<std::int32_t>(static_cast<std::int16_t>(w0)), static_cast<std::int32_t>(static_cast<std::int16_t>(w1)), static_cast<std::int32_t>(static_cast<std::int16_t>(w2)), static_cast<std::int32_t>(static_cast<std::int16_t>(w3)), 0xau, text, joan::orig::arg::u32(color), static_cast<std::int32_t>(static_cast<std::int16_t>(w1c)), 1, 0, 1, 2, 0, joan::orig::arg::f32(f2c), joan::orig::arg::f32(f30), static_cast<std::int32_t>(static_cast<std::int8_t>(b20)), joan::orig::arg::i32(-1));  // 0x00626320
            if (static_cast<std::int32_t>(*first) == 0) *first = global<std::int16_t>(d::g_lineNo);
            *height = static_cast<std::int16_t>(*height + global<std::int16_t>(d::g_lineNo));
            text = global<const char*>(d::g_endPtr);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("TextBox::measure_lines", 0x00627d80, &H::measure_lines);
#undef B

}  // namespace game_b92_05_2
}  // namespace

// ---------------------------------------- from modules/game_ui2.cpp (game/ui2)
namespace {
namespace game_ui2 {


struct H {
    // 0x00627e80 Font::glyph: +0x2a first / +0x2c last char, +0x3c records; null when the
    // width (+0xa) is <= 0. ret 4.
    std::uint8_t* glyph(std::uint16_t ch) {
        const std::int32_t c = ch;
        if (c < static_cast<std::int32_t>(jo::self<jo::Font>(this)->first_char)) return nullptr;
        // Double-byte editions (RESOURCE\CODEPAGE.RES): a 16-bit character the font lacks comes from the CJK glyphs
        // (cjk_glyphs.hpp); null otherwise, as the original. Codes above 0xff reach here only in those editions.
        if (c > static_cast<std::int32_t>(jo::self<jo::Font>(this)->last_char)) return c > 0xff ? joan::cjk::glyph(this, ch) : nullptr;
        std::uint8_t* const g = jo::view<std::uint8_t*>(jo::self<jo::Font>(this)->glyphs) + (c - static_cast<std::int32_t>(jo::self<jo::Font>(this)->first_char)) * 0xe;
        if (static_cast<std::int32_t>(jo::as_font_glyph(g)->width) <= 0) return nullptr;
        return g;
    }
};

#define U2(name, addr, fn) JOAN_REPLACE("game/ui2", name, addr, fn)
U2("Font::glyph", 0x00627e80, &H::glyph);
#undef U2

}  // namespace game_ui2
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
namespace {
namespace game_b104_11_3 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using u8 = std::uint8_t;

using i8 = std::int8_t;

// The four crop passes shared by 0x00627f10 / 0x00628490. pix(row, col) is the pixel test.
template <class P>
void crop_box(i16 x0, i16 y0, i16* left, i16* top, i16* right, i16* bottom, P pix) {
    // top
    bool found = false;
    i16 r = static_cast<i16>(y0 + *top);
    for (; r <= y0 + *bottom; r = static_cast<i16>(r + 1)) {
        for (i16 c = static_cast<i16>(x0 + *left); c <= x0 + *right; c = static_cast<i16>(c + 1)) {
            if (pix(r, c)) {
                found = true;
                *top = static_cast<i16>(r - y0);
                break;
            }
        }
        if (found) break;
    }
    if (r > y0 + *bottom) *top = *bottom;
    // bottom
    found = false;
    r = static_cast<i16>(y0 + *bottom);
    for (; r >= y0 + *top; r = static_cast<i16>(r - 1)) {
        for (i16 c = static_cast<i16>(x0 + *left); c <= x0 + *right; c = static_cast<i16>(c + 1)) {
            if (pix(r, c)) {
                found = true;
                *bottom = static_cast<i16>(r - y0);
                break;
            }
        }
        if (found) break;
    }
    if (r < y0 + *top) *bottom = *top;
    // left
    found = false;
    i16 c = static_cast<i16>(x0 + *left);
    for (; c <= x0 + *right; c = static_cast<i16>(c + 1)) {
        for (i16 rr = static_cast<i16>(y0 + *top); rr <= y0 + *bottom; rr = static_cast<i16>(rr + 1)) {
            if (pix(rr, c)) {
                found = true;
                *left = static_cast<i16>(c - x0);
                break;
            }
        }
        if (found) break;
    }
    if (c > x0 + *right) *left = *right;
    // right
    found = false;
    c = static_cast<i16>(x0 + *right);
    for (; c >= x0 + *left; c = static_cast<i16>(c - 1)) {
        for (i16 rr = static_cast<i16>(y0 + *top); rr <= y0 + *bottom; rr = static_cast<i16>(rr + 1)) {
            if (pix(rr, c)) {
                found = true;
                *right = static_cast<i16>(c - x0);
                break;
            }
        }
        if (found) break;
    }
    if (c < x0 + *left) *right = *left;
}

JOAN_COUNTER(c_crop_627f10, "Bitmap::crop_627f10");

struct H {
    // ------------------------------------------------------------------ 0x00627f10
    u32 crop_627f10(u8* buf, i32 pitch, u32 bpp, u32 x0, u32 y0, i16* left, i16* top, i16* right,
                    i16* bottom) {
        c_crop_627f10.hit();
        const i16 n = static_cast<i16>(bpp);
        crop_box(static_cast<i16>(x0), static_cast<i16>(y0), left, top, right, bottom, [&](i16 r, i16 c) {
            for (i16 k = 0; k < n; k = static_cast<i16>(k + 1)) {
                const u32 off = static_cast<u32>(r) * static_cast<u32>(pitch) +
                                static_cast<u32>(c) * static_cast<u32>(static_cast<i32>(n)) +
                                static_cast<u32>(static_cast<i32>(k));
                if (static_cast<i8>(buf[static_cast<i32>(off)]) != 0) return true;
            }
            return false;
        });
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("Bitmap::crop_627f10", 0x00627f10, &H::crop_627f10);
#undef B

}  // namespace game_b104_11_3
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

// One texel of a locked surface (raw pixel bytes, not an object layout: the same on x64).
template <class T>
T pixel(const std::uint8_t* p) {
    return *reinterpret_cast<const T*>(p);
}

JOAN_COUNTER(c_pixel_alpha, "pixel_alpha");

struct H {
    // 0x00628940
    std::uint32_t pixel_alpha(const std::uint8_t* p, std::uint32_t fmt) {
        c_pixel_alpha.hit();
        switch (fmt) {
            case 0x15: return pixel<std::uint32_t>(p) >> 24;
            case 0x19: return static_cast<std::uint32_t>(pixel<std::uint16_t>(p)) >> 15;
            case 0x1a: return static_cast<std::uint32_t>(pixel<std::uint16_t>(p)) >> 12;
            case 0x1c: return (fmt & 0xffffff00u) | p[0];
            case 0x1d: return static_cast<std::uint32_t>(pixel<std::uint16_t>(p)) >> 8;
            default: return fmt | 0xffu;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("pixel_alpha", 0x00628940, &H::pixel_alpha);
#undef B

}  // namespace game_b97_06
}  // namespace

// Out-of-line definition from modules/game_ui1.cpp (declared in original/ui.hpp, registered above).
namespace {

constexpr img::Addr kFiles(d::g_misc);

using joan::orig::helpers::at;

std::int16_t& text_lines() { return global<std::int16_t>(d::g_text_lines); }

JOAN_COUNTER(c_measure, "Font::measure");

// fild (word field) ; fmul (float scale) ; fstp qword ; Files::round (0x00590a20, ax used).
std::int16_t scaled(std::int16_t v, float scale) {
    const double p = joan::hook::x87::Fpu{true}.mul(static_cast<double>(v), static_cast<double>(scale));
    return static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(img::obj(kFiles))->round(joan::orig::arg::f64(p))));  // 0x00590a20
}

// 4-byte text cursor (0x0058b9c0 ctor) and its function-pointer dispatched methods.
void cursor_init(std::uintptr_t* it, const char* text) { joan::orig::as_text_cursor(it)->construct(joan::orig::arg::str(text)); }      // 0x0058b9c0
std::int32_t cursor_count(std::uintptr_t* it) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_text_cursor(it)->length()); }       // 0x0058c190
std::int32_t cursor_char(std::uintptr_t* it) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_text_cursor(it)->get()); }        // 0x0058ba70
void cursor_next(std::uintptr_t* it) { joan::orig::as_text_cursor(it)->next(); }                              // 0x0058bbc0
std::int32_t to_upper(std::int32_t ch) { return joan::orig::arg::as<std::int32_t>(joan::orig::text_toupper(joan::orig::arg::i32(ch))); }          // 0x0058e380

}  // namespace

namespace joan::orig {

// 0x00624f00 Font::measure (+0x36 space width, +0x38 spacing, glyph +0xa width). ret 0x14.
std::int16_t Font::measure(const char* text, std::int16_t max_chars, std::int16_t wrap, std::int16_t mode, float scale) {
    c_measure.hit();
    std::int32_t width = 0, max_width = 0, line_width = 0, prev = 0;
    if (static_cast<std::int8_t>(loaded) == 0) return static_cast<std::int16_t>(width);
    std::uintptr_t it = 0;  // a TextCursor (one pointer)
    cursor_init(&it, text);
    if (max_chars < 0) max_chars = static_cast<std::int16_t>(cursor_count(&it));
    text_lines() = 1;
    std::int32_t i = 1;
    for (;;) {
        std::int32_t ch = cursor_char(&it);
        if (ch == 0 || i > max_chars) break;
        if (mode == 1) ch = to_upper(ch);
        if (wrap != 0) {
            const std::int32_t t = width - 1;
            if (t - scaled(spacing, scale) > wrap - 1) {
                max_width = wrap;
                width = line_width;
                width += scaled(spacing, scale);
                text_lines() = static_cast<std::int16_t>(text_lines() + 1);
            }
        }
        if (ch == 10) {
            if (width > max_width) max_width = width;
            width = 0;
            line_width = 0;
            text_lines() = static_cast<std::int16_t>(text_lines() + 1);
        } else {
            if (ch == 0x20) {
                width += scaled(space_width, scale);
                line_width = 0;
            } else if (ch == 9) {
                width += scaled(space_width, scale) * 8;
                line_width = 0;
            } else {
                std::uint8_t* const g = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_font(this)->glyph(joan::orig::arg::u16(static_cast<std::uint16_t>(ch))));  // 0x00627e80
                if (g != nullptr) {
                    const std::int32_t w = scaled(jo::as_font_glyph(g)->width, scale);
                    width += w;
                    line_width += w;
                } else {
                    width += scaled(space_width, scale);
                    line_width += scaled(space_width, scale);
                }
            }
            if (wrap != 0 && line_width > wrap) {
                width -= line_width - wrap;
                line_width = wrap;
            }
        }
        cursor_next(&it);
        ++i;
        width += scaled(spacing, scale);
        prev = ch;
    }
    if (wrap != 0) {
        const std::int32_t t = width - 1;
        if (t - scaled(spacing, scale) > wrap - 1) {
            max_width = wrap;
            width = line_width;
            width += scaled(spacing, scale);
            text_lines() = static_cast<std::int16_t>(text_lines() + 1);
        }
    }
    width -= scaled(spacing, scale);
    if (prev == 10) text_lines() = static_cast<std::int16_t>(text_lines() - 1);
    return static_cast<std::int16_t>(max_width > width ? max_width : width);
}

}  // namespace joan::orig

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::TextDesc::clear() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_05::H::text_desc_clear>(this)); }  // 0x623dc0
auto joan::orig::Font::dtor() -> void { joan::bind::member<&::game_leaves80a::H::font_dtor>(this); }  // 0x623e70
auto joan::orig::Font::init_623e90(const char* a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::uint16_t a4, std::uint16_t a5, std::int16_t a6, std::uint16_t a7, std::uint16_t a8) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_06::H::font_init>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x623e90
auto joan::orig::Font::init(const char* a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_11::H::font_init>(this, a0, a1, a2)); }  // 0x624810
auto joan::orig::Font::release() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_05::H::font_release>(this)); }  // 0x624dc0
auto joan::orig::Font::scaled_round(float a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_07::H::scaled_round_624eb0>(this, a0)); }  // 0x624eb0
auto joan::orig::Font::text_height(std::int16_t a0, float a1) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_leaves80b::H::text_height>(this, a0, a1)); }  // 0x6253f0
auto joan::orig::Font::draw_glyphs(std::int16_t a0, std::int16_t a1, const char* a2, void* a3, std::uint32_t a4, std::int16_t a5, std::int16_t a6, std::uint32_t a7, std::uint32_t a8, float a9, float a10) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_ui1::H::draw_glyphs>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)); }  // 0x625c50
auto joan::orig::TextRenderer::draw_wrapped(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, const char* a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10, std::uint32_t a11, float a12, float a13, std::uint32_t a14) -> void { joan::bind::member<&::game_b82_11::H::draw_wrapped>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14); }  // 0x626e00
auto joan::orig::TextBox::measure_lines(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, const char* a4, std::int32_t a5, std::int32_t a6, void* a7, void* a8, std::uint32_t a9, std::uint32_t a10) -> void { joan::bind::member<&::game_b92_05_2::H::measure_lines>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }  // 0x627d80
auto joan::orig::Font::glyph(std::uint16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_ui2::H::glyph>(this, a0)); }  // 0x627e80
auto joan::orig::Bitmap::crop_key_628490(void* a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, void* a6, void* a7, void* a8, void* a9) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_11_2::H::crop_628490>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)); }  // 0x628490
auto joan::orig::Bitmap::pixel_alpha(void* a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_06::H::pixel_alpha>(this, a0, a1)); }  // 0x628940
