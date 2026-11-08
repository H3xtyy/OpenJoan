// uti3d/videocap.cpp: functions of the original d:\ww\uti3d\videocap.cpp (re/attribution.csv).
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
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cmath>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b115_12.cpp (fix/uninit)
//
//    - 0x00686350 VideoCapture::init(file, unused, format, w, h) (thiscall, ret 0x14; one caller, the developer key
//      combination of 0x0052f930, constant format 11). The original wrote the captured frames to `file` with Windows
//      Media's IWMWriter (wmvcore.dll); that writer went in cleanup C1. What stays: the capture's frame size and a
//      scratch frame buffer (the size of the writer's sample), so video_copy_frame 0x004d7f80 still has somewhere
//      to copy each frame; write_frame 0x00687060 finds no writer and does nothing. Nothing is written to `file`.
//    - 0x00686f50 VideoCapture::release: frees that buffer (the original released the writer's objects and
//      FreeLibrary'd wmvcore.dll; like the original it leaves `active` set).
namespace {
namespace game_b115_12 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_vcap, "VideoCapture::init_686350");
JOAN_COUNTER(c_com_release, "Com686::release");

struct H {
    // -------------------------------------------------------------- 0x00686350
    void vcap_init(const char* /*file*/, i32 /*unused*/, i32 format, i32 w, i32 h) {
        c_vcap.hit();
        jo::VideoCapture* const s = jo::self<jo::VideoCapture>(this);
        joan::orig::as_video_capture(s)->release();  // 0x00686f50
        const i32 bpp = joan::orig::arg::as<i32>(joan::orig::format::bytes_per_pixel(joan::orig::arg::u32(joan::orig::arg::as<i32>(joan::orig::d3d_format(joan::orig::arg::u32(format))))));  // 0x005ffe60(0x00600080)
        s->bpp = bpp;
        s->width = w;
        s->height = h;
        s->input = -1;
        const u32 size = static_cast<u32>(bpp) * static_cast<u32>(w) * static_cast<u32>(h);
        s->buffer = static_cast<std::uint8_t*>(joan::crt::operator_new(joan::orig::arg::u32(size)));
        s->active = 1;
    }

    // -------------------------------------------------------------- 0x00686f50
    void* com_release() {
        c_com_release.hit();
        jo::VideoCapture* const s = jo::self<jo::VideoCapture>(this);
        if (s->active == 0) return this;
        if (s->buffer != nullptr) {
            joan::crt::operator_delete(joan::orig::arg::mptr(s->buffer));
            s->buffer = nullptr;
        }
        return this;
    }
};

// 0x00686350: registered with its former fix/uninit key (the Windows Media writer it fixed is gone).
JOAN_REPLACE("fix/uninit", "VideoCapture::init", 0x00686350, &H::vcap_init);
JOAN_REPLACE("game/b91_04", "Com686::release", 0x00686f50, &H::com_release);

}  // namespace game_b115_12
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {

using u32 = std::uint32_t;

using i32 = std::int32_t;

using joan::orig::helpers::crt_memset;

struct H {
    // ------------------------------------------------------------------ 0x006862d0
    H* com_init_ctor() {
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::VideoCapture)));  // 0x28 on x86
        (*reinterpret_cast<i32(JOAN_STDCALL* const*)(void*)>(img::addr(d::iat_ole32_CoInitialize)))(nullptr);  // CoInitialize
        jo::self<jo::VideoCapture>(this)->active = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("ComInit::ctor", 0x006862d0, &H::com_init_ctor);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b99_09.cpp (game/b99_09)
namespace {
namespace game_b99_09 {

using joan::call::thiscall;

using joan::hook::global;

using u32 = std::uint32_t;

struct H {
    u32 shutdown_com() {  // 0x00686320
        joan::orig::as_video_capture(this)->release();  // 0x00686f50
        return reinterpret_cast<u32(JOAN_STDCALL*)()>(global<std::uintptr_t>(d::iat_ole32_CoUninitialize))();  // CoUninitialize
    }
};

#define B99(name, addr_, fn) JOAN_REPLACE("game/b99_09", name, addr_, fn)
B99("X::shutdown_com", 0x00686320, &H::shutdown_com);
#undef B99

}  // namespace game_b99_09
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::VideoCapture::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_15::H::com_init_ctor>(this)); }  // 0x6862d0
auto joan::orig::VideoCapture::shutdown() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_09::H::shutdown_com>(this)); }  // 0x686320
auto joan::orig::VideoCapture::init(const char* a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> void { joan::bind::member<&::game_b115_12::H::vcap_init>(this, a0, a1, a2, a3, a4); }  // 0x686350
auto joan::orig::VideoCapture::release() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b115_12::H::com_release>(this)); }  // 0x686f50
