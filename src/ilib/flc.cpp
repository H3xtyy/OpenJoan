// ilib/flc.cpp: functions of the original d:\ww\ilib\flc.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>

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

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

void* const kMem = obj(d::g_mem);    // memory manager (0x0058e7b0 alloc, 0x0058eb30 realloc)

}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
//
//      0x00471420 Flc::open(name)                  (thiscall, ret 4) - (dead code).
namespace {
namespace game_b102_02 {

JOAN_COUNTER(c_flc_open, "Flc::open");

void* const kHeap = obj(d::g_mem);

void* heap_alloc(u32 n, const char* file, i32 line) {
    return joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(n), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058e7b0
}

struct H {

    // 0x00471420
    i32 flc_open(const char* name) {
        c_flc_open.hit();
        joan::orig::as_flc(this)->close();  // 0x0058a5e0
        jo::view<void*>(jo::self<jo::Flc>(this)->animation) = joan::orig::arg::as<void*>(joan::orig::tfanimation_new_mem(joan::orig::arg::mptr(name)));  // 0x005a97b0
        if (jo::view<void*>(jo::self<jo::Flc>(this)->animation) == nullptr) return 0;
        if (joan::orig::arg::as<u8>(joan::orig::export_(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Flc>(this)->animation)), joan::orig::arg::mptr(&jo::self<jo::Flc>(this)->header))) != 1) {  // 0x005a99e0
            joan::orig::music_stream::release(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Flc>(this)->animation)));  // 0x005a98a0
            return 0;
        }
        const char* const src = str(d::s_d_ww_ilib_flc_cpp);  // "d:\ww\ilib\flc.cpp"
        const i32 line1 = static_cast<i32>(global<i16>(d::g_a157c4)) + 0xe;
        const i32 w = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_flc(this)->width()));  // 0x0058a480
        const i32 h = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_flc(this)->height()));  // 0x0058a4a0
        jo::view<void*>(jo::self<jo::Flc>(this)->frame_buf) = heap_alloc(static_cast<u32>(w * h), src, line1);
        jo::view<void*>(jo::self<jo::Flc>(this)->palette) = heap_alloc(0x300, src, static_cast<i32>(global<i16>(d::g_a157c4)) + 0xf);
        joan::orig::set_buffers(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Flc>(this)->animation)), jo::view<void*>(jo::self<jo::Flc>(this)->frame_buf), jo::view<void*>(jo::self<jo::Flc>(this)->palette));  // 0x005a9b90
        jo::self<jo::Flc>(this)->opened = 1;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("Flc::open", 0x00471420, &H::flc_open);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_b102_07.cpp (game/b102_07)
//
//      0x0058a320 Flc::open(name)                     (thiscall, ret 4)
namespace {
namespace game_b102_07 {

JOAN_COUNTER(c_flc_open, "Flc::open");

struct H {

    // -------------------------------------------------------------- 0x0058a320
    i32 flc_open(const char* name) {
        c_flc_open.hit();
        joan::orig::as_flc(this)->close();                         // 0x0058a5e0
        jo::self<jo::Flc>(this)->animation = joan::orig::arg::as<jo::TFAnimation*>(joan::orig::tfanimation_new_file(joan::orig::arg::str(name)));      // 0x005a93f0
        if (jo::self<jo::Flc>(this)->animation == nullptr) return 0;
        if (static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::export_(joan::orig::arg::mptr(jo::self<jo::Flc>(this)->animation), &jo::self<jo::Flc>(this)->header))) != 1) {  // 0x005a99e0
            joan::orig::music_stream::release(joan::orig::arg::mptr(jo::self<jo::Flc>(this)->animation));                                      // 0x005a98a0
            return 0;
        }
        const i32 line1 = static_cast<i32>(global<i16>(d::g_a157c0)) + 0xe;
        const i32 w = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_flc(this)->width()));  // 0x0058a480
        const i32 h = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_flc(this)->height()));  // 0x0058a4a0
        const u32 size = static_cast<u32>(w) * static_cast<u32>(h);
        jo::self<jo::Flc>(this)->frame_buf = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(size), str(d::s_d_ww_ilib_flc_cpp), joan::orig::arg::i32(line1)));  // 0x0058e7b0
        const i32 line2 = static_cast<i32>(global<i16>(d::g_a157c0)) + 0xf;
        jo::self<jo::Flc>(this)->palette = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_mem(kMem)->add(0x300u, str(d::s_d_ww_ilib_flc_cpp), joan::orig::arg::i32(line2)));
        joan::orig::set_buffers(joan::orig::arg::mptr(jo::self<jo::Flc>(this)->animation), jo::self<jo::Flc>(this)->frame_buf, jo::self<jo::Flc>(this)->palette);  // 0x005a9b90
        jo::view<u32>(jo::self<jo::Flc>(this)->opened) = 1;
        return 1;
    }
};

#define B102(name, addr, fn) JOAN_REPLACE("game/b102_07", name, addr, fn)
B102("Flc::open", 0x0058a320, &H::flc_open);
#undef B102

}  // namespace game_b102_07
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
//
//      0x0058a4a0 Flc::frame_word      (thiscall, ret 0) - ax = word +0xe (upper eax = this).
//
//      0x0058a5e0 Flc::close           (thiscall, ret 0)
namespace {
namespace game_b92_04 {

using u16 = std::uint16_t;

void mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

struct H {

    // ------------------------------------------------------------------------ 0x0058a4a0
    u32 flc_word() {
        // bits 16..31 of eax were `this`' in the original (callers read ax only); zero here, no pointer bits in a u32.
        return jo::self<jo::Flc>(this)->header.height;
    }

    // ------------------------------------------------------------------------ 0x0058a5e0
    void flc_close() {
        if (jo::view<u32>(jo::self<jo::Flc>(this)->opened) != 0) {
            joan::orig::music_stream::release(joan::orig::arg::mptr(jo::self<jo::Flc>(this)->animation));  // 0x005a98a0
            mem_free(jo::view<void*>(jo::self<jo::Flc>(this)->frame_buf), str(d::s_d_ww_ilib_flc_cpp), static_cast<i32>(global<i16>(d::g_a157c8)) + 4);  // flc.cpp
            mem_free(jo::view<void*>(jo::self<jo::Flc>(this)->palette), str(d::s_d_ww_ilib_flc_cpp), static_cast<i32>(global<i16>(d::g_a157c8)) + 5);
            jo::view<u32>(jo::self<jo::Flc>(this)->opened) = 0;
        }
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Flc::frame_word", 0x0058a4a0, &H::flc_word);
B92("Flc::close", 0x0058a5e0, &H::flc_close);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b103_08.cpp (game/b103_08)
namespace {
namespace game_b103_08 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    u32 dtor_58a2f0() { joan::orig::as_flc(this)->close(); return 0; }  // 0x0058a5e0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("Flc::dtor_58a2f0", 0x0058a2f0, &H::dtor_58a2f0);
#undef B

}  // namespace game_b103_08
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
//
//      0x0058a480 word getter (+0xc)                           thiscall, ax
namespace {
namespace game_b91_02 {


struct H {
    // 0x0058a480
    std::uint32_t word_c() { return jo::self<jo::Flc>(this)->header.width; }  // eax bits 16..31 were `this`' (callers read ax only)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("word_c", 0x0058a480, &H::word_c);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b103_08_2.cpp (game/b103_08)
namespace {
namespace game_b103_08_2 {

using u32 = std::uint32_t;


struct H {

    // ------------------------------------------------------------------------ flc
    H* reset_58a2c0() {
        jo::view<u32>(jo::self<jo::Flc>(this)->opened) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("Flc::reset_58a2c0", 0x0058a2c0, &H::reset_58a2c0);
#undef B

}  // namespace game_b103_08_2
}  // namespace

// ---------------------------------------- from modules/game_b105_15.cpp (game/b105_15)
namespace {
namespace game_b105_15 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// 0x0058a680
u32 JOAN_CDECL forward_5aaad0(u32 a) { return joan::orig::arg::as<u32>(joan::orig::set_a67638(joan::orig::arg::u32(a))); }  // 0x005aaad0

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("Obj::forward_5aaad0", 0x0058a680, &forward_5aaad0);
#undef B

}  // namespace game_b105_15
}  // namespace

// ---------------------------------------- from modules/game_b103_08_3.cpp (game/b103_08)
namespace {
namespace game_b103_08_3 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;


struct H {

    u32 is_58a6b0() { return joan::orig::tfframe::decode(joan::orig::arg::mptr(jo::self<jo::Flc>(this)->animation)) == 1 ? 1u : 0u; }  // 0x005a9d80
    u32 is_58a6f0() { return joan::orig::music_stream::rewind(joan::orig::arg::mptr(jo::self<jo::Flc>(this)->animation)) == 1 ? 1u : 0u; }  // 0x005aa040
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("Flc::is_58a6b0", 0x0058a6b0, &H::is_58a6b0);
B("Flc::is_58a6f0", 0x0058a6f0, &H::is_58a6f0);
#undef B

}  // namespace game_b103_08_3
}  // namespace

// ---------------------------------------- from modules/game_b102_08.cpp (game/b102_08)
namespace {
namespace game_b102_08 {

using u32 = std::uint32_t;

using i32 = std::int32_t;


using joan::call::thiscall;

using i16 = std::int16_t;
using u8 = std::uint8_t;

std::uintptr_t crt_memcpy(void* d, const void* s, u32 n) { return joan::orig::arg::as<std::uintptr_t>(std::memcpy(joan::orig::arg::mptr(d), joan::orig::arg::ptr(s), joan::orig::arg::u32(n))); }

JOAN_COUNTER(c_copy_rect, "Bitmap::copy_rect");

struct H {
    // ------------------------------------------------------------------ 0x0058a730
    std::uintptr_t copy_rect(u8* dst, i16 x1, i16 y1, i16 x2, i16 y2) {
        c_copy_rect.hit();
        if (x1 > x2) {
            const i16 t = x1;
            x1 = x2;
            x2 = t;
        }
        if (y1 > y2) {
            const i16 t = y1;
            y1 = y2;
            y2 = t;
        }
        const i32 w = static_cast<i32>(x2) - static_cast<i32>(x1) + 1;
        std::uintptr_t eax = static_cast<u32>(static_cast<i32>(y1));
        for (i32 y = y1; y <= static_cast<i32>(y2); ++y) {
            const i16 pitch = joan::orig::arg::as<i16>(joan::orig::as_flc(this)->width());  // 0x0058a480
            const u8* const src = jo::self<jo::Flc>(this)->frame_buf + (static_cast<u32>(y) * static_cast<u32>(static_cast<i32>(pitch)) +
                            static_cast<u32>(static_cast<i32>(x1)));
            eax = crt_memcpy(dst, src, static_cast<u32>(w));
            dst += w;
        }
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_08", name, addr, fn)
B("Bitmap::copy_rect", 0x0058a730, &H::copy_rect);
#undef B

}  // namespace game_b102_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Flc::width() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_02::H::word_c>(this)); }  // 0x58a480
auto joan::orig::Flc::height() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_04::H::flc_word>(this)); }  // 0x58a4a0
auto joan::orig::Flc::close() -> void { joan::bind::member<&::game_b92_04::H::flc_close>(this); }  // 0x58a5e0
