// elib/jpeg.cpp: functions of the original d:\ww\elib\jpeg.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../lib/jpeg/jpeglib.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

namespace jpeg = joan::jpeg;
// The Jpeg object (0x00a69b40) holds the game libjpeg's cinfo and error manager (typed in lib/jpeg/jpeglib.hpp; the
// generated jo::Jpeg names them as opaque pointers).
jpeg::j_compress_ptr cinfo_of(void* self) { return reinterpret_cast<jpeg::j_compress_ptr>(jo::self<jo::Jpeg>(self)->cinfo); }
jpeg::jpeg_error_mgr* err_of(void* self) { return reinterpret_cast<jpeg::jpeg_error_mgr*>(jo::self<jo::Jpeg>(self)->err); }
// A game function as a libjpeg method: what its code slot holds (the installed replacement, as the original's store of
// the function's address reaches it; [Hooks] / [Fixes] apply as for every call of that address).
template <class F>
F method(fn::Code code) {
    return reinterpret_cast<F>(fn::code_value(code));
}

}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
//
//    - Jpeg::ctor 0x005a8d80 (ret 0): allocates the compress struct / error manager (0x15b / 0x84 bytes on x86:
//      sizeof of the typed structures), jpeg_std_error, callbacks, jpeg_CreateCompress(62, size).
namespace {
namespace game_b100_10 {

using joan::orig::helpers::memset_;  // memset

void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_jpeg, "Jpeg::ctor");

struct H {
    // ------------------------------------------------------------------------ 0x005a8d80
    void* jpeg_ctor() {
        c_jpeg.hit();
        i32 line = static_cast<i32>(global<i16>(d::g_a16e34)) + 1;
        constexpr u32 kCinfoSize = jpeg::SIZEOF<jpeg::jpeg_compress_struct>;  // 0x15b on x86
        constexpr u32 kErrSize = jpeg::SIZEOF<jpeg::jpeg_error_mgr>;          // 0x84 on x86
        jo::view<void*>(jo::self<jo::Jpeg>(this)->cinfo) = joan::orig::as_mem(kMem)->add(kCinfoSize, str(d::s_d_ww_elib_jpeg_cpp), joan::orig::arg::i32(line));  // 0x0058e7b0
        line = static_cast<i32>(global<i16>(d::g_a16e34)) + 2;
        jo::view<void*>(jo::self<jo::Jpeg>(this)->err) = joan::orig::as_mem(kMem)->add(kErrSize, str(d::s_d_ww_elib_jpeg_cpp), joan::orig::arg::i32(line));  // 0x0058e7b0
        jo::self<jo::Jpeg>(this)->u32_08 = 0;
        memset_(cinfo_of(this), 0, kCinfoSize);
        memset_(err_of(this), 0, kErrSize);
        jpeg::jpeg_error_mgr* const err = jpeg::jpeg_std_error(err_of(this));  // jpeg_std_error 0x00765f43
        cinfo_of(this)->err = err;
        err_of(this)->error_exit = method<void (*)(jpeg::j_compress_ptr)>(fn::Jpeg_error_exit);           // 0x005a8ea0
        err_of(this)->output_message = method<void (*)(jpeg::j_compress_ptr)>(fn::jpeg_output_message);   // 0x005a8f10
        err_of(this)->emit_message = method<void (*)(jpeg::j_compress_ptr, int)>(fn::nop_5a8f70);         // 0x005a8f70
        jpeg::jpeg_CreateCompress(cinfo_of(this), jpeg::JPEG_LIB_VERSION, kCinfoSize);  // 0x007a9a60
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("Jpeg::ctor", 0x005a8d80, &H::jpeg_ctor);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b102_10.cpp (game/b102_10)
//
//    - Jpeg::error_exit 0x005a8ea0 (cdecl (cinfo)): format_message + " decompress image error" ->
//      fatal 0x00586cb0 (exits).
//
//  - Live references: 0x005a8ea0 (jpeg error manager, stored at 0x005a8e26), 0x005a9d80 (two callers,
//    `and eax,0xff`), 0x005b5b70 (qsort at 0x005b5b08), 0x005cd3a0 (callback, 0x005cc8d0). All the others
//    have neither callers nor data references (dead code).
//
//  - 0x005a8ea0 passes the formatted message as the format string of 0x00586cb0 (vsprintf); only a '%'
//    in the message would make the varargs read differ (error path, followed by exit()).
namespace {
namespace game_b102_10 {

char* crt_strcat(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_jpeg_error, "Jpeg::error_exit");

// 0x005a8ea0
void jpeg_error_exit(jpeg::j_compress_ptr cinfo) {
    c_jpeg_error.hit();
    char buf[0x100];
    buf[0] = 0;
    cinfo->err->format_message(cinfo, buf);
    crt_strcat(buf, str(d::s_decompress_image_error));  // "decompress image error"
    joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), joan::orig::arg::str(static_cast<const char*>(buf)));  // 0x00586cb0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b102_10", name, addr, fn)
B("Jpeg::error_exit", 0x005a8ea0, &jpeg_error_exit);
#undef B

}  // namespace game_b102_10
}  // namespace

// ---------------------------------------- from modules/game_b111_11.cpp (fix/uninit)
//
//    - jpeg_output_message 0x005a8f10 (cdecl (cinfo), the jpeg error manager's output_message
//      set at 0x005a8e32): passes the formatted libjpeg message as the printf FORMAT of the fatal
//      message box 0x00586bc0; a '%' in the message reads stack garbage as arguments. Fix: a
//      message without a '%' conversion ("%%" allowed) is passed exactly as the original; one with '%' is passed as the
//      argument of "%s" (the intended literal text).
namespace {
namespace game_b111_11 {

JOAN_COUNTER(c_jpeg, "jpeg_output_message");

// ============================================================================ 0x005a8f10 (fix/uninit)
void jpeg_output_message(jpeg::j_compress_ptr cinfo) {
    c_jpeg.hit();
    char buf[0x100];  // [ebp-0x100]
    buf[0] = 0;
    cinfo->err->format_message(cinfo, buf);
    buf[0xff] = 0;
    bool percent = false;
    for (const char* q = buf; *q != 0; ++q) {
        if (*q != '%') continue;
        if (q[1] == '%') {  // "%%" reads no argument: the original output is kept
            ++q;
            continue;
        }
        percent = true;
    }
    if (!percent) {
        joan::orig::warning_box(joan::orig::arg::mptr(obj(d::g_err)), joan::orig::arg::str(static_cast<const char*>(buf)));  // 0x00586bc0
    } else {
        // fix/uninit: the original uses the message as the format; '%' conversions would read stack
        // garbage. The message is shown literally.
        joan::orig::warning_box(joan::orig::arg::mptr(obj(d::g_err)), "%s", static_cast<const char*>(buf));
    }
}

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: a libjpeg message containing '%' is shown via "%s" instead of as a printf format.
FIX("jpeg_output_message", 0x005a8f10, &jpeg_output_message);
#undef FIX

}  // namespace game_b111_11
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
//
//    - nop_5a8f70 0x005a8f70 (cdecl, cinfo, msg_level): no effect; the Jpeg object's emit_message.
//
//  - 0x0058ef00, 0x005a8f70, 0x005a07e0: void; the original leaves the caller's eax (or a dead
//    temporary) in eax - no callers exist.
namespace {
namespace game_b103_10 {

// 0x005a8f70 (jpeg_error_mgr::emit_message of the Jpeg object)
void nop_5a8f70(jpeg::j_compress_ptr /*cinfo*/, i32 /*msg_level*/) {}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("nop_5a8f70", 0x005a8f70, &nop_5a8f70);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - Jpeg::free 0x005a8fa0 (ret 0): 0x7a9b30 / 0x7a9b40 on +0, heap free +4 and +0.
namespace {
namespace game_b99_07 {

using joan::call::thiscall;

void* const kHeap = obj(d::g_mem);

struct H {
    // ------------------------------------------------------------------ 0x005a8fa0
    u32 jpeg_free() {
        if (jo::self<jo::Jpeg>(this)->u32_08 == 0)
            jpeg::jpeg_destroy_compress(cinfo_of(this));  // 0x007a9b30
        else
            jpeg::jpeg_abort_compress(cinfo_of(this));  // 0x007a9b40
        joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Jpeg>(this)->err)), str(d::s_d_ww_elib_jpeg_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a16e38)) + 0xd));  // 0x0058ed70
        joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Jpeg>(this)->cinfo)), str(d::s_d_ww_elib_jpeg_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a16e38)) + 0xf));  // 0x0058ed70
        return 0;  // void callee; the only caller (atexit 0x006035a0) ignores the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Jpeg::free", 0x005a8fa0, &H::jpeg_free);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

using joan::hook::global;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::stdcall_call;
using joan::img::str;

// COM (stdcall, this first) call through the object's vtable slot (slot index = x86 byte offset / 4).
template <class R, class... A>
R com_call(void* object, std::int32_t slot, A... args) {
    using Fn = R(JOAN_STDCALL*)(void*, A...);
    Fn fn;
    std::memcpy(&fn, &joan::call::vtable_of(object)[slot], sizeof fn);
    return fn(object, args...);
}
using joan::orig::helpers::wsub;

JOAN_COUNTER(c_5a9040, "Screenshot::write_jpeg");

struct H {
    // 0x005a9040
    i32 write_jpeg(const char* name, void* image) {
        c_5a9040.hit();
        jpeg::j_compress_ptr const cinfo = cinfo_of(this);
        jpeg::jpeg_CreateCompress(cinfo, jpeg::JPEG_LIB_VERSION, jpeg::SIZEOF<jpeg::jpeg_compress_struct>);  // 0x007a9a60
        void* const fp = joan::orig::arg::as<void*>(joan::orig::libc::fopen(joan::orig::arg::str(name), str(d::s_wb)));  // fopen
        if (fp == nullptr) return 0;
        jpeg::jpeg_stdio_dest(cinfo, fp);  // 0x007aa9e0
        joan::orig::as_texture(image)->lock();           // 0x0060fe60
        const u32 cur = joan::orig::arg::as<u32>(joan::orig::asm_get_a15020());  // 0x005711b7
        jo::Surface* const surface = jo::self<jo::Surface>(image);  // (Bitmap::address 0x005a93b0 reads it as one)
        if (surface->format_bits != cur) joan::orig::set_pixel_format_5706f0(joan::orig::arg::i32(surface->format_bits));  // 0x005706f0
        cinfo->image_width = joan::orig::arg::as<u32>(joan::orig::as_camera(image)->get_1c());   // 0x0060fa50
        cinfo->image_height = joan::orig::arg::as<u32>(joan::orig::as_camera(image)->get_20());  // 0x0060fa70
        cinfo->input_components = 3;
        cinfo->in_color_space = jpeg::JCS_RGB;
        jpeg::jpeg_set_defaults(cinfo);        // 0x007aa1b0
        jpeg::jpeg_set_quality(cinfo, 80, 1);  // 0x007aa180 (quality 80, force_baseline)
        jpeg::jpeg_start_compress(cinfo, 1);   // 0x007a9e00 (write_all_tables)
        joan::orig::as_camera(image)->get_1c();          // width (the original's width * 3 is unused)
        u8 ramp[0x600];
        com_call<u32>(global<void*>(d::g_game_device), 0x4c / 4, static_cast<void*>(ramp));  // GetGammaRamp
        u8 row[0x3000];
        joan::orig::vector(joan::orig::arg::mptr(static_cast<void*>(row)), 3, 0x1000, joan::orig::arg::mptr(fn::code_value(fn::Obj_ctor_596bd0)));  // 0x00449af0 (ctor 0x00596bd0)
        while (cinfo->next_scanline < cinfo->image_height) {
            u8* src = static_cast<u8*>(joan::orig::as_bitmap(image)->address(0u, joan::orig::arg::u32(cinfo->next_scanline)));  // 0x005a93b0
            u8* dst = row;
            for (i32 x = wsub(joan::orig::arg::as<i32>(joan::orig::as_camera(image)->get_1c()), 1); x >= 0; --x) {
                const u32 px = joan::orig::arg::as<u32>(joan::orig::as_image(image)->read_pixel(src));  // 0x00610550
                joan::orig::as_color(image)->to_bytes_6101b0(joan::orig::arg::u32(px), joan::orig::arg::mptr(dst));                   // 0x006101b0
                dst[0] = joan::orig::arg::as<u8>(joan::orig::as_image(image)->ramp_channel(joan::orig::arg::mptr(static_cast<void*>(ramp)), 0, joan::orig::arg::i32(static_cast<u32>(dst[0]))));  // 0x00613090
                dst[1] = joan::orig::arg::as<u8>(joan::orig::as_image(image)->ramp_channel(joan::orig::arg::mptr(static_cast<void*>(ramp)), 1, joan::orig::arg::i32(static_cast<u32>(dst[1]))));
                dst[2] = joan::orig::arg::as<u8>(joan::orig::as_image(image)->ramp_channel(joan::orig::arg::mptr(static_cast<void*>(ramp)), 2, joan::orig::arg::i32(static_cast<u32>(dst[2]))));
                src += static_cast<u32>(surface->bytes_per_pixel);
                dst += 3;
            }
            jpeg::JSAMPROW rowp = row;
            jpeg::jpeg_write_scanlines(cinfo, &rowp, 1);  // 0x007a9e80
        }
        jpeg::jpeg_finish_compress(cinfo);  // 0x007a9ba0
        joan::orig::libc::fclose(joan::orig::arg::mptr(fp));  // fclose
        jpeg::jpeg_destroy_compress(cinfo);  // 0x007a9b30
        joan::orig::as_image(image)->unlock();       // 0x0060ff20
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("Screenshot::write_jpeg", 0x005a9040, &H::write_jpeg);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {


struct H {
    // 0x005a93b0
    void* address(std::uint32_t x, std::uint32_t y) {
        return static_cast<void*>(jo::self<jo::Surface>(this)->bits + jo::view<std::uint32_t>(jo::self<jo::Surface>(this)->pitch) * y + jo::view<std::uint32_t>(jo::self<jo::Surface>(this)->bytes_per_pixel) * x);
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("Bitmap::address", 0x005a93b0, &H::address);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::JpegWriter::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_10::H::jpeg_ctor>(this)); }  // 0x5a8d80
auto joan::orig::JpegWriter::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_07::H::jpeg_free>(this)); }  // 0x5a8fa0
auto joan::orig::JpegWriter::write_surface(const char* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_09::H::write_jpeg>(this, a0, a1)); }  // 0x5a9040
auto joan::orig::Bitmap::address(std::uint32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_03::H::address>(this, a0, a1)); }  // 0x5a93b0
