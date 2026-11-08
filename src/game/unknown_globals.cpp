// game/unknown_globals.cpp: functions whose original source file is unknown (formerly game/_unplaced/globals.cpp;
// re/attribution.csv: between ww/world_s.cpp and ww/ilib/angle.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
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

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b107_08.cpp (game/b107_08)
namespace {
namespace game_b107_08 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- static initialiser chains ----
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}
u32 JOAN_CDECL init_566e00() { return cdecl_call<u32>(fn::obj_stub_566e30); }
std::uintptr_t JOAN_CDECL init_56daf0() { return joan::orig::arg::as<u32>(joan::orig::as_console(obj(d::g_console))->construct()); }  // 0x00454270

#define B(name, addr, fn) JOAN_REPLACE("game/b107_08", name, addr, fn)
B("init_563e10", 0x00563e10, (&init_pair<fn::init_heap_a48838, fn::init_563e70>));
B("init_564910", 0x00564910, (&init_pair<fn::fn_564930, fn::StaticInit_atexit_564980>));
B("init_5654a0", 0x005654a0, (&init_pair<fn::clear_a50048, fn::fn_5654f0>));
B("init_565ec0", 0x00565ec0, (&init_pair<fn::init_565ee0, fn::fn_565f10>));
B("init_5668e0", 0x005668e0, (&init_pair<fn::init_566900, fn::init_atexit_566930>));
B("init_566e00", 0x00566e00, &init_566e00);
B("init_567830", 0x00567830, (&init_pair<fn::init_567850, fn::init_atexit_567880>));
B("init_568250", 0x00568250, (&init_pair<fn::init_568270, fn::init_atexit_5682a0>));
B("init_568c70", 0x00568c70, (&init_pair<fn::init_568c90, fn::init_atexit_568cc0>));
B("init_569720", 0x00569720, (&init_pair<fn::init_569740, fn::init_atexit_569770>));
B("init_56a1d0", 0x0056a1d0, (&init_pair<fn::obj_stub_56a1f0, fn::init_56a210>));
B("init_56ac80", 0x0056ac80, (&init_pair<fn::obj_stub_56aca0, fn::init_56acc0>));
B("init_56c1d0", 0x0056c1d0, (&init_pair<fn::init_obj_56c1f0, fn::init_56c210>));
B("init_56cf70", 0x0056cf70, (&init_pair<fn::StaticInit_ctor_a59780, fn::StaticInit_atexit_56cfe0>));
B("init_56daf0", 0x0056daf0, &init_56daf0);
#undef B

}  // namespace game_b107_08
}  // namespace

// ---------------------------------------- from modules/game_b101_08.cpp (game/b101_08)
namespace {
namespace game_b101_08 {

using joan::call::thiscall;
using joan::img::obj;

void* const kHeap = obj(d::g_mem);

// ---------------------------------------------------------------------- 0x00563e30
static void* JOAN_CDECL init_heap_a48838() { return joan::orig::as_mem(kHeap)->construct(); }  // 0x0058e770

#define B(name, addr, fn) JOAN_REPLACE("game/b101_08", name, addr, fn)
B("init_heap_a48838", 0x00563e30, &init_heap_a48838);
#undef B

}  // namespace game_b101_08
}  // namespace

// ---------------------------------------- from modules/game_b105_07.cpp (game/b105_07)
namespace {
namespace game_b105_07 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// CRT initialiser forwarders (direct calls)
template <fn::Code Target>
u32 JOAN_CDECL forward() {
    return cdecl_call<u32>(Target);
}
// static ctor / dtor on a global object
template <fn::Code Thunk, auto Global>
u32 JOAN_CDECL on_global() {
    return thiscall<u32>(Thunk, obj(Global));
}
template <fn::Code Fn>
u32 JOAN_CDECL register_atexit() {
    return joan::orig::arg::as<u32>(joan::crt::atexit(fn::code_ptr(Fn)));
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_07", name, addr, fn)
B("init_563ec0", 0x00563ec0, &forward<fn::init_563ee0>);
B("init_563f10", 0x00563f10, &forward<fn::init_563f30>);
B("init_563e70", 0x00563e70, &register_atexit<fn::dtor_563e90>);
B("init_564030", 0x00564030, &register_atexit<fn::fn_564050>);
B("dtor_563e90", 0x00563e90, (&on_global<fn::Obj_empty_dtor_58eee0, d::g_mem>));  // 0x0058eee0
B("init_563f80", 0x00563f80, (&on_global<fn::Obj_zero_50, d::g_calendar>));  // 0x00495230
B("dtor_563fd0", 0x00563fd0, (&on_global<fn::X_call_4953b0, d::g_calendar>));  // 0x00495260
B("init_564010", 0x00564010, (&on_global<fn::Obj_clear_337, d::g_sys3d>));  // 0x005f65b0
B("init_5640a0", 0x005640a0, (&on_global<fn::Obj_clear_40c, d::g_vga>));  // 0x0060f540
B("dtor_5640f0", 0x005640f0, (&on_global<fn::Obj_call_60f700, d::g_vga>));  // 0x0060f590
B("init_5641c0", 0x005641c0, (&on_global<fn::Obj_clear_1c, d::g_texture_dir>));  // 0x006040b0
B("dtor_564210", 0x00564210, (&on_global<fn::Obj_call_604150, d::g_texture_dir>));  // 0x006040e0
#undef B

}  // namespace game_b105_07
}  // namespace

// ---------------------------------------- from modules/game_b101_09.cpp (game/b101_09)
namespace {
namespace game_b101_09 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

// ---------------------------------------------------------------- static initialisers / terminators
std::uintptr_t JOAN_CDECL init_563ee0() { return joan::orig::arg::as<u32>(joan::orig::as_error_log(obj(d::g_err))->construct()); }  // 0x00586950
std::uintptr_t JOAN_CDECL init_564250() { return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(obj(d::g_texture_array))->construct()); }  // 0x00608db0
std::uintptr_t JOAN_CDECL init_567030() { return joan::orig::arg::as<u32>(joan::orig::as_sound3dsystem(obj(d::g_sound3d_system))->construct()); }  // 0x005a02d0
std::uintptr_t JOAN_CDECL init_567310() { return joan::orig::arg::as<u32>(joan::orig::as_ini(obj(d::g_steer))->construct()); }  // 0x0047c370
std::uintptr_t JOAN_CDECL init_56ade0() { return joan::orig::arg::as<u32>(joan::orig::as_news_list(obj(d::g_tasks))->construct()); }  // 0x004b9220
std::uintptr_t JOAN_CDECL init_56d4e0() { return joan::orig::arg::as<u32>(joan::orig::as_debug_overlay(obj(d::g_debug_overlay))->construct()); }  // 0x0056ded0

#define B(name, addr, fn) JOAN_REPLACE("game/b101_09", name, addr, fn)
B("init_563ee0", 0x00563ee0, &init_563ee0);
B("init_564250", 0x00564250, &init_564250);
B("init_567030", 0x00567030, &init_567030);
B("init_567310", 0x00567310, &init_567310);
B("init_56ade0", 0x0056ade0, &init_56ade0);
B("init_56d4e0", 0x0056d4e0, &init_56d4e0);
#undef B

}  // namespace game_b101_09
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- static initialisers ----
using joan::orig::helpers::text_desc_clear;  // 0x00623dc0
void crt_atexit(const void* fn) { joan::crt::atexit(fn); }

void JOAN_CDECL init_564c50() { text_desc_clear(d::g_a56a98); }
void JOAN_CDECL init_565040() { text_desc_clear(d::g_a50748); }
void JOAN_CDECL init_565430() { text_desc_clear(d::g_a507c8); }
void JOAN_CDECL init_565820() { text_desc_clear(d::g_font_18pt_dark_numbers); }
void JOAN_CDECL init_565c10() { text_desc_clear(d::g_font_verdana_12_gold_black_outline); }
void JOAN_CDECL init_566000() { text_desc_clear(d::g_font_a5cdc0); }
void JOAN_CDECL init_564320() { crt_atexit(fn::code_ptr(fn::fn_564350)); }
void JOAN_CDECL init_5647a0() { crt_atexit(fn::code_ptr(fn::fn_5647d0)); }

// 0x00563f30
void JOAN_CDECL init_563f30() {
    joan::orig::as_misc(obj(d::g_misc))->construct_591cb0();  // 0x00591cb0
    joan::orig::as_misc(obj(d::g_a50490))->construct_591cb0();  // 0x00591cb0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("init_563f30", 0x00563f30, &init_563f30);
B("init_564320", 0x00564320, &init_564320);
B("init_5647a0", 0x005647a0, &init_5647a0);
B("init_564c50", 0x00564c50, &init_564c50);
B("init_565040", 0x00565040, &init_565040);
B("init_565430", 0x00565430, &init_565430);
B("init_565820", 0x00565820, &init_565820);
B("init_565c10", 0x00565c10, &init_565c10);
B("init_566000", 0x00566000, &init_566000);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b107_09.cpp (game/b107_09)
namespace {
namespace game_b107_09 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- static initialiser wrappers ----
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_09", name, addr, fn)
B("init_563f60", 0x00563f60, (&init_pair<fn::init_563f80, fn::init_563fa0>));
B("init_5649a0", 0x005649a0, (&init_pair<fn::fn_5649c0, fn::init_5649e0>));
B("init_565530", 0x00565530, (&init_pair<fn::init_565550, fn::fn_565580>));
B("init_565f50", 0x00565f50, (&init_pair<fn::init_565f70, fn::fn_565fa0>));
B("init_566970", 0x00566970, (&init_pair<fn::init_566990, fn::init_atexit_5669c0>));
B("init_566ef0", 0x00566ef0, (&init_pair<fn::obj_stub_566f20, fn::init_atexit_566f40>));
B("init_5678c0", 0x005678c0, (&init_pair<fn::init_5678e0, fn::init_atexit_567910>));
B("init_5682e0", 0x005682e0, (&init_pair<fn::init_568300, fn::init_atexit_568330>));
B("init_568d00", 0x00568d00, (&init_pair<fn::init_568d20, fn::init_atexit_568d50>));
B("init_5697b0", 0x005697b0, (&init_pair<fn::init_5697d0, fn::init_atexit_569800>));
B("init_56a260", 0x0056a260, (&init_pair<fn::obj_stub_56a280, fn::init_56a2a0>));
B("init_56ad10", 0x0056ad10, (&init_pair<fn::obj_stub_56ad30, fn::init_56ad50>));
B("init_56c380", 0x0056c380, (&init_pair<fn::fn_56c3a0, fn::init_atexit_56c3d0>));
B("init_56d0a0", 0x0056d0a0, (&init_pair<fn::StaticInit_ctor_a54c20, fn::init_56d0f0>));
#undef B

}  // namespace game_b107_09
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

i32 crt_atexit(const void* fn) { return joan::crt::atexit(fn); }
i32 JOAN_CDECL init_563fa0() { return crt_atexit(fn::code_ptr(fn::dtor_563fd0)); }
i32 JOAN_CDECL init_564440() { return crt_atexit(fn::code_ptr(fn::fn_564470)); }
i32 JOAN_CDECL init_564710() { return crt_atexit(fn::code_ptr(fn::fn_564740)); }
i32 JOAN_CDECL init_5648c0() { return crt_atexit(fn::code_ptr(fn::fn_5648f0)); }

std::intptr_t text_desc_clear(img::Addr g) { return joan::orig::arg::as<i32>(joan::orig::as_text_desc(obj(g))->clear()); }  // 0x00623dc0
std::intptr_t JOAN_CDECL init_564f20() { return text_desc_clear(d::g_a528e8); }
std::intptr_t JOAN_CDECL init_5651f0() { return text_desc_clear(d::g_font_verdana_10); }
std::intptr_t JOAN_CDECL init_5655e0() { return text_desc_clear(d::g_font_a47d18); }
std::intptr_t JOAN_CDECL init_565940() { return text_desc_clear(d::g_font_24pt_large_numbers_gold); }
std::intptr_t JOAN_CDECL init_565ca0() { return text_desc_clear(d::g_font_celtic_10pt_english); }
std::intptr_t JOAN_CDECL init_565ee0() { return text_desc_clear(d::g_a52808); }

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("init_563fa0", 0x00563fa0, &init_563fa0);
B("init_564440", 0x00564440, &init_564440);
B("init_564710", 0x00564710, &init_564710);
B("init_5648c0", 0x005648c0, &init_5648c0);
B("init_564f20", 0x00564f20, &init_564f20);
B("init_5651f0", 0x005651f0, &init_5651f0);
B("init_5655e0", 0x005655e0, &init_5655e0);
B("init_565940", 0x00565940, &init_565940);
B("init_565ca0", 0x00565ca0, &init_565ca0);
B("init_565ee0", 0x00565ee0, &init_565ee0);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b107_07.cpp (game/b107_07)
namespace {
namespace game_b107_07 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- static initialiser pairs (direct calls; eax of the second call) ----
template <fn::Code First, fn::Code Second>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(First);
    return cdecl_call<u32>(Second);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_07", name, addr, fn)
B("init_563ff0", 0x00563ff0, (&init_pair<fn::init_564010, fn::init_564030>));
B("init_564a30", 0x00564a30, (&init_pair<fn::fn_564a50, fn::StaticInit_atexit_564b00>));
B("init_5655c0", 0x005655c0, (&init_pair<fn::init_5655e0, fn::fn_565610>));
B("init_565fe0", 0x00565fe0, (&init_pair<fn::init_566000, fn::fn_566030>));
B("init_566a00", 0x00566a00, (&init_pair<fn::init_566a20, fn::init_atexit_566a50>));
B("init_567130", 0x00567130, (&init_pair<fn::init_567160, fn::init_atexit_567190>));
B("init_567950", 0x00567950, (&init_pair<fn::init_567970, fn::init_atexit_5679a0>));
B("init_568370", 0x00568370, (&init_pair<fn::init_568390, fn::init_atexit_5683c0>));
B("init_568e20", 0x00568e20, (&init_pair<fn::init_568e40, fn::init_atexit_568e70>));
B("init_5698d0", 0x005698d0, (&init_pair<fn::init_5698f0, fn::init_atexit_569920>));
B("init_56a380", 0x0056a380, (&init_pair<fn::obj_stub_56a3a0, fn::init_56a3c0>));
B("init_56adc0", 0x0056adc0, (&init_pair<fn::init_56ade0, fn::init_atexit_56ae10>));
B("init_56c640", 0x0056c640, (&init_pair<fn::init_obj_56c660, fn::init_56c680>));
B("init_56d390", 0x0056d390, (&init_pair<fn::StaticInit_ctor_a52968, fn::StaticInit_atexit_56d400>));
#undef B

}  // namespace game_b107_07
}  // namespace

// ---------------------------------------- from modules/game_b107_14.cpp (game/b107_14)
namespace {
namespace game_b107_14 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---------------------------------------------------------------------------------------------
// static initialisers / global object stubs
template <fn::Code First, fn::Code Second>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(First);           // direct call (global ctor stub)
    return cdecl_call<u32>(Second);   // direct call (atexit registration stub)
}
template <auto G, fn::Code Thunk>
u32 JOAN_CDECL obj_stub() { return thiscall<u32>(Thunk, obj(G)); }

#define B(name, addr, fn) JOAN_REPLACE("game/b107_14", name, addr, fn)
B("init_564080", 0x00564080, (&init_pair<fn::init_5640a0, fn::atexit_5640f0>));
B("init_564c30", 0x00564c30, (&init_pair<fn::init_564c50, fn::fn_564c80>));
B("init_565650", 0x00565650, (&init_pair<fn::init_565670, fn::fn_5656a0>));
B("init_566070", 0x00566070, (&init_pair<fn::init_566090, fn::fn_5660c0>));
B("init_566a90", 0x00566a90, (&init_pair<fn::init_566ab0, fn::init_atexit_566ae0>));
B("init_5674c0", 0x005674c0, (&init_pair<fn::obj_stub_5674f0, fn::init_atexit_567510>));
B("init_5679e0", 0x005679e0, (&init_pair<fn::init_567a00, fn::init_atexit_567a30>));
B("init_568400", 0x00568400, (&init_pair<fn::init_568420, fn::init_atexit_568450>));
B("init_568eb0", 0x00568eb0, (&init_pair<fn::init_568ed0, fn::init_atexit_568f00>));
B("init_569960", 0x00569960, (&init_pair<fn::init_569980, fn::init_atexit_5699b0>));
B("init_56a410", 0x0056a410, (&init_pair<fn::obj_stub_56a430, fn::init_56a450>));
B("obj_stub_56ae30", 0x0056ae30, (&obj_stub<d::g_tasks, fn::Obj56ae50_forward>));  // 0x0056ae50
B("init_56c7e0", 0x0056c7e0, (&init_pair<fn::init_56c800, fn::init_atexit_56c880>));
B("obj_stub_56db30", 0x0056db30, (&obj_stub<d::g_console, fn::Obj_nop_454290>));  // 0x00454290
#undef B

}  // namespace game_b107_14
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
namespace {
namespace game_b100_08 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

i32 JOAN_CDECL atexit_5640f0() { return joan::crt::atexit(fn::code_ptr(fn::dtor_5640f0)); }  // 0x005640c0
i32 JOAN_CDECL atexit_564500() { return joan::crt::atexit(fn::code_ptr(fn::fn_564500)); }  // 0x005644d0
i32 JOAN_CDECL atexit_564860() { return joan::crt::atexit(fn::code_ptr(fn::dtor_stub_564860)); }  // 0x00564830
std::uintptr_t text_desc_clear(img::Addr g) { return joan::orig::arg::as<u32>(joan::orig::as_text_desc(obj(g))->clear()); }  // 0x00623dc0
std::uintptr_t JOAN_CDECL clear_a50640() { return text_desc_clear(d::g_a50640); }  // 0x00564ce0
std::uintptr_t JOAN_CDECL clear_a5d340() { return text_desc_clear(d::g_a5d340); }  // 0x00564d70
std::uintptr_t JOAN_CDECL clear_a403e0() { return text_desc_clear(d::g_a403e0); }  // 0x00565160
std::uintptr_t JOAN_CDECL clear_a50048() { return text_desc_clear(d::g_font_verdana_12_regular_black_out); }  // 0x005654c0
std::uintptr_t JOAN_CDECL clear_a40420() { return text_desc_clear(d::g_font_18pt_attribute_numbers_over); }  // 0x00565790
std::uintptr_t JOAN_CDECL clear_a57f40() { return text_desc_clear(d::g_a57f40); }  // 0x005659d0
std::uintptr_t JOAN_CDECL clear_a48a68() { return text_desc_clear(d::g_font_celtic_18pt_black); }  // 0x00565dc0
std::uintptr_t JOAN_CDECL clear_a48d48() { return text_desc_clear(d::g_font_goudy_bold_18); }  // 0x005661b0
std::uintptr_t JOAN_CDECL clear_a51608() { return text_desc_clear(d::g_font_goudy14_22_white); }  // 0x00566240
std::uintptr_t JOAN_CDECL clear_a48fb0() { return text_desc_clear(d::g_font_goudy_18); }  // 0x00566630

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("atexit_5640f0", 0x005640c0, &atexit_5640f0);
B("atexit_564500", 0x005644d0, &atexit_564500);
B("atexit_564860", 0x00564830, &atexit_564860);
B("clear_a50640", 0x00564ce0, &clear_a50640);
B("clear_a5d340", 0x00564d70, &clear_a5d340);
B("clear_a403e0", 0x00565160, &clear_a403e0);
B("clear_a50048", 0x005654c0, &clear_a50048);
B("clear_a40420", 0x00565790, &clear_a40420);
B("clear_a57f40", 0x005659d0, &clear_a57f40);
B("clear_a48a68", 0x00565dc0, &clear_a48a68);
B("clear_a48d48", 0x005661b0, &clear_a48d48);
B("clear_a51608", 0x00566240, &clear_a51608);
B("clear_a48fb0", 0x00566630, &clear_a48fb0);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

void crt_atexit(const void* fn) { joan::crt::atexit(fn); }
using joan::orig::helpers::text_desc_clear;  // 0x00623dc0

void JOAN_CDECL init_564150() { crt_atexit(fn::code_ptr(fn::atexit_416ce0)); }
void JOAN_CDECL init_564560() { crt_atexit(fn::code_ptr(fn::fn_564590)); }
void JOAN_CDECL init_5649e0() { crt_atexit(fn::code_ptr(fn::fn_564a10)); }
void JOAN_CDECL init_564e00() { text_desc_clear(d::g_a47b68); }
void JOAN_CDECL init_565280() { text_desc_clear(d::g_font_verdana_10_bold); }
void JOAN_CDECL init_565670() { text_desc_clear(d::g_a460e8); }
void JOAN_CDECL init_565a60() { text_desc_clear(d::g_dialogs); }
void JOAN_CDECL init_565e50() { text_desc_clear(d::g_a596d8); }
void JOAN_CDECL init_5662d0() { text_desc_clear(d::g_font_goudy12_bold_black); }

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("init_564150", 0x00564150, &init_564150);
B("init_564560", 0x00564560, &init_564560);
B("init_5649e0", 0x005649e0, &init_5649e0);
B("init_564e00", 0x00564e00, &init_564e00);
B("init_565280", 0x00565280, &init_565280);
B("init_565670", 0x00565670, &init_565670);
B("init_565a60", 0x00565a60, &init_565a60);
B("init_565e50", 0x00565e50, &init_565e50);
B("init_5662d0", 0x005662d0, &init_5662d0);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b107_05.cpp (game/b107_05)
namespace {
namespace game_b107_05 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- static initialisers ----
template <fn::Code First, fn::Code Second>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(First);
    return cdecl_call<u32>(Second);
}

// 0x0056aee0 (atexit destructor)
u32 JOAN_CDECL init_56aee0() { return joan::orig::arg::as<u32>(joan::orig::as_profile_list(obj(d::g_profile_list))->destruct()); }  // 0x004c9c60

#define B(name, addr, fn) JOAN_REPLACE("game/b107_05", name, addr, fn)
B("init_5641a0", 0x005641a0, (&init_pair<fn::init_5641c0, fn::StaticInit_atexit_564210>));
B("init_564cc0", 0x00564cc0, (&init_pair<fn::clear_a50640, fn::fn_564d10>));
B("init_5656e0", 0x005656e0, (&init_pair<fn::StaticInit_clear_a56ad8, fn::fn_565730>));
B("init_566100", 0x00566100, (&init_pair<fn::init_566120, fn::fn_566150>));
B("init_566b20", 0x00566b20, (&init_pair<fn::init_566b40, fn::init_atexit_566b70>));
B("init_567550", 0x00567550, (&init_pair<fn::obj_stub_567580, fn::init_atexit_5675a0>));
B("init_567a70", 0x00567a70, (&init_pair<fn::init_567a90, fn::init_atexit_567ac0>));
B("init_568490", 0x00568490, (&init_pair<fn::init_5684b0, fn::init_atexit_5684e0>));
B("init_568f40", 0x00568f40, (&init_pair<fn::init_568f60, fn::init_atexit_568f90>));
B("init_5699f0", 0x005699f0, (&init_pair<fn::init_569a10, fn::init_atexit_569a40>));
B("init_56a4a0", 0x0056a4a0, (&init_pair<fn::obj_stub_56a4c0, fn::init_56a4e0>));
B("init_56aee0", 0x0056aee0, &init_56aee0);
B("init_56c960", 0x0056c960, (&init_pair<fn::init_56c980, fn::init_atexit_56c9b0>));
#undef B

}  // namespace game_b107_05
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

i32 JOAN_CDECL atexit_564210() { return joan::crt::atexit(fn::code_ptr(fn::dtor_564210)); }  // 0x005641e0
i32 JOAN_CDECL atexit_564620() { return joan::crt::atexit(fn::code_ptr(fn::fn_564620)); }  // 0x005645f0
i32 JOAN_CDECL atexit_564b00() { return joan::crt::atexit(fn::code_ptr(fn::fn_564b00)); }  // 0x00564a70
std::uintptr_t JOAN_CDECL clear_a50600() { return joan::orig::arg::as<u32>(joan::orig::as_text_desc(obj(d::g_a50600))->clear()); }  // 0x00564e90 (0x00623dc0)
std::uintptr_t JOAN_CDECL clear_a5b9a8() { return joan::orig::arg::as<u32>(joan::orig::as_text_desc(obj(d::g_font_verdana_12))->clear()); }  // 0x00565310
std::uintptr_t JOAN_CDECL clear_a56ad8() { return joan::orig::arg::as<u32>(joan::orig::as_text_desc(obj(d::g_font_18pt_attribute_numbers_out))->clear()); }  // 0x00565700
std::uintptr_t JOAN_CDECL clear_a58ac0() { return joan::orig::arg::as<u32>(joan::orig::as_text_desc(obj(d::g_text_a58ac0))->clear()); }  // 0x00565b80

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("StaticInit::atexit_564210", 0x005641e0, &atexit_564210);
B95("StaticInit::atexit_564620", 0x005645f0, &atexit_564620);
B95("StaticInit::atexit_564b00", 0x00564a70, &atexit_564b00);
B95("StaticInit::clear_a50600", 0x00564e90, &clear_a50600);
B95("StaticInit::clear_a5b9a8", 0x00565310, &clear_a5b9a8);
B95("StaticInit::clear_a56ad8", 0x00565700, &clear_a56ad8);
B95("StaticInit::clear_a58ac0", 0x00565b80, &clear_a58ac0);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b105_08.cpp (game/b105_08)
namespace {
namespace game_b105_08 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// 0x00564230
u32 JOAN_CDECL init_pair_564230() {
    joan::orig::init_564250();
    joan::orig::init_564280();
    return 0;  // void callee; _initterm ignores the eax
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_08", name, addr, fn)
B("init_pair_564230", 0x00564230, &init_pair_564230);
#undef B

}  // namespace game_b105_08
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

void crt_atexit(const void* fn) { joan::crt::atexit(fn); }
using joan::orig::helpers::text_desc_clear;  // 0x00623dc0
void JOAN_CDECL init_564280() { crt_atexit(fn::code_ptr(fn::fn_5642b0)); }
void JOAN_CDECL init_564680() { crt_atexit(fn::code_ptr(fn::exit_5646b0)); }
void JOAN_CDECL init_564b90() { crt_atexit(fn::code_ptr(fn::fn_564c10)); }
void JOAN_CDECL init_5650d0() { text_desc_clear(d::g_a56eb0); }
void JOAN_CDECL init_5653a0() { text_desc_clear(d::g_font_a53f28); }
void JOAN_CDECL init_5658b0() { text_desc_clear(d::g_font_18pt_light_numbers); }
void JOAN_CDECL init_565d30() { text_desc_clear(d::g_a506a0); }
void JOAN_CDECL init_566120() { text_desc_clear(d::g_a59738); }
void JOAN_CDECL init_566510() { text_desc_clear(d::g_font_goudy_bold_20); }

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("init_564280", 0x00564280, &init_564280);
B98("init_564680", 0x00564680, &init_564680);
B98("init_564b90", 0x00564b90, &init_564b90);
B98("init_5650d0", 0x005650d0, &init_5650d0);
B98("init_5653a0", 0x005653a0, &init_5653a0);
B98("init_5658b0", 0x005658b0, &init_5658b0);
B98("init_565d30", 0x00565d30, &init_565d30);
B98("init_566120", 0x00566120, &init_566120);
B98("init_566510", 0x00566510, &init_566510);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b107_06.cpp (game/b107_06)
namespace {
namespace game_b107_06 {

using joan::call::cdecl_call;

// ---- static initialiser pairs ----
template <fn::Code First, fn::Code Second>
i32 JOAN_CDECL init_pair() {
    cdecl_call<i32>(First);
    return cdecl_call<i32>(Second);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_06", name, addr, fn)
B("init_pair_5642e0", 0x005642e0, (&init_pair<fn::fn_564300, fn::init_564320>));
B("init_pair_564d50", 0x00564d50, (&init_pair<fn::clear_a5d340, fn::fn_564da0>));
B("init_pair_565770", 0x00565770, (&init_pair<fn::clear_a40420, fn::fn_5657c0>));
B("init_pair_566190", 0x00566190, (&init_pair<fn::clear_a48d48, fn::fn_5661e0>));
B("init_pair_566bb0", 0x00566bb0, (&init_pair<fn::init_566bd0, fn::init_atexit_566c00>));
B("init_pair_567b00", 0x00567b00, (&init_pair<fn::init_567b20, fn::init_atexit_567b50>));
B("init_pair_568520", 0x00568520, (&init_pair<fn::init_568540, fn::init_atexit_568570>));
B("init_pair_568fd0", 0x00568fd0, (&init_pair<fn::init_568ff0, fn::init_atexit_569020>));
B("init_pair_569a80", 0x00569a80, (&init_pair<fn::init_569aa0, fn::init_atexit_569ad0>));
B("init_pair_56a530", 0x0056a530, (&init_pair<fn::obj_stub_56a550, fn::init_56a570>));
B("init_pair_56af00", 0x0056af00, (&init_pair<fn::obj_stub_56af30, fn::init_56af50>));
B("init_pair_56afa0", 0x0056afa0, (&init_pair<fn::init_56afc0, fn::init_atexit_56aff0>));
B("init_pair_56caa0", 0x0056caa0, (&init_pair<fn::init_obj_56cac0, fn::init_56cae0>));
#undef B

}  // namespace game_b107_06
}  // namespace

// ---------------------------------------- from modules/game_b105_08_2.cpp (game/b105_08)
namespace {
namespace game_b105_08_2 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

u32 on_global(fn::Code thunk, img::Addr g) { return thiscall<u32>(thunk, obj(g)); }
u32 crt_atexit(const void* fn) { return joan::orig::arg::as<u32>(joan::crt::atexit(fn)); }

u32 JOAN_CDECL stub_564300() { return on_global(fn::SoundSlots_ctor, d::g_async_file_loader); }  // 0x00578ce0
u32 JOAN_CDECL stub_564350() { return on_global(fn::AsyncFileLoader_dtor, d::g_async_file_loader); }  // 0x00578d40
u32 JOAN_CDECL stub_564390() { return on_global(fn::Scene_Scene, d::g_scene); }  // 0x005e9a00
u32 JOAN_CDECL stub_5643e0() { return on_global(fn::Scene_dtor, d::g_scene); }  // 0x005e9a90
u32 JOAN_CDECL stub_564420() { return on_global(fn::Obj_ctor_4a4ed0, d::g_light_sys); }  // 0x004a4ed0
u32 JOAN_CDECL stub_564470() { return on_global(fn::ObjK_dtor, d::g_light_sys); }  // 0x004a4fb0
u32 JOAN_CDECL stub_5644b0() { return on_global(fn::Obj_clear_15_4586c0, d::g_cloud_layers); }  // 0x004586c0
u32 JOAN_CDECL stub_564500() { return on_global(fn::Obj_forward_459b70, d::g_cloud_layers); }  // 0x004586f0
u32 JOAN_CDECL stub_564540() { return on_global(fn::Obj689b10_Obj, d::g_weather_system); }  // 0x00689b10
u32 JOAN_CDECL stub_564590() { return on_global(fn::Obj689bf0_dtor_dtor, d::g_weather_system); }  // 0x00689bf0
u32 JOAN_CDECL stub_5645d0() { return on_global(fn::Obj_ctor_613650, d::g_view3d); }  // 0x00613650
u32 JOAN_CDECL stub_564620() { return on_global(fn::View3D_dtor, d::g_view3d); }  // 0x00613700
u32 JOAN_CDECL stub_564660() { return on_global(fn::Obj631_ctor, d::g_sprites_a46128); }  // 0x006317a0
u32 JOAN_CDECL stub_5646f0() { return on_global(fn::Obj_self_5a8ba0, d::g_box); }  // 0x005a8ba0
u32 JOAN_CDECL stub_564740() { return on_global(fn::Obj_stub_5a8bc0, d::g_box); }  // 0x005a8bc0
u32 JOAN_CDECL stub_564780() { return on_global(fn::Obj_ctor_5922d0, d::g_mouse); }  // 0x005922d0
u32 JOAN_CDECL stub_5647d0() { return on_global(fn::Obj_call_592830, d::g_mouse); }  // 0x00592350
u32 JOAN_CDECL stub_564810() { return on_global(fn::Input_ctor_596210, d::g_mouse_cursor); }  // 0x00596210
u32 JOAN_CDECL stub_5648a0() { return on_global(fn::Obj_clear_160, d::g_help_box); }  // 0x00488390
u32 JOAN_CDECL stub_5648f0() { return on_global(fn::Obj_reset_4884c0, d::g_help_box); }  // 0x004883d0
u32 JOAN_CDECL stub_564930() { return on_global(fn::Obj_ctor_419c80, d::g_game_set); }  // 0x00419c80
u32 JOAN_CDECL stub_564980() { return on_global(fn::Obj_dtor_419f70, d::g_game_set); }  // 0x00419f70
u32 JOAN_CDECL stub_5649c0() { return on_global(fn::TextDesc_clear, d::g_text_a5d140); }  // 0x00623dc0
u32 JOAN_CDECL stub_564a10() { return on_global(fn::Font_dtor, d::g_text_a5d140); }  // 0x00623e70
u32 JOAN_CDECL stub_564a50() { return on_global(fn::TextDesc_clear, d::g_a5cc90); }  // 0x00623dc0
u32 JOAN_CDECL stub_564b70() { return on_global(fn::TextDesc_clear, d::g_a3cef0); }  // 0x00623dc0
u32 JOAN_CDECL stub_564c10() { return on_global(fn::Font_dtor, d::g_a3cef0); }  // 0x00623e70
u32 JOAN_CDECL stub_564ca0() { return on_global(fn::Font_dtor, d::g_a56a98); }  // 0x00623e70
u32 JOAN_CDECL stub_564d30() { return on_global(fn::Font_dtor, d::g_a50640); }  // 0x00623e70
u32 JOAN_CDECL stub_564dc0() { return on_global(fn::Font_dtor, d::g_a5d340); }  // 0x00623e70
u32 JOAN_CDECL stub_564e50() { return on_global(fn::Font_dtor, d::g_a47b68); }  // 0x00623e70
u32 JOAN_CDECL stub_564ee0() { return on_global(fn::Font_dtor, d::g_a50600); }  // 0x00623e70
u32 JOAN_CDECL stub_564f70() { return on_global(fn::Font_dtor, d::g_a528e8); }  // 0x00623e70
u32 JOAN_CDECL stub_565000() { return on_global(fn::Font_dtor, d::g_a50580); }  // 0x00623e70
u32 JOAN_CDECL stub_565090() { return on_global(fn::Font_dtor, d::g_a50748); }  // 0x00623e70
u32 JOAN_CDECL stub_565120() { return on_global(fn::Font_dtor, d::g_a56eb0); }  // 0x00623e70
u32 JOAN_CDECL stub_5651b0() { return on_global(fn::Font_dtor, d::g_a403e0); }  // 0x00623e70
u32 JOAN_CDECL stub_565240() { return on_global(fn::Font_dtor, d::g_font_verdana_10); }  // 0x00623e70
u32 JOAN_CDECL stub_5652d0() { return on_global(fn::Font_dtor, d::g_font_verdana_10_bold); }  // 0x00623e70
u32 JOAN_CDECL stub_565360() { return on_global(fn::Font_dtor, d::g_font_verdana_12); }  // 0x00623e70
u32 JOAN_CDECL stub_5653f0() { return on_global(fn::Font_dtor, d::g_font_a53f28); }  // 0x00623e70
u32 JOAN_CDECL stub_565480() { return on_global(fn::Font_dtor, d::g_a507c8); }  // 0x00623e70
u32 JOAN_CDECL stub_565510() { return on_global(fn::Font_dtor, d::g_font_verdana_12_regular_black_out); }  // 0x00623e70

u32 JOAN_CDECL stub_564c80() { return crt_atexit(fn::code_ptr(fn::fn_564ca0)); }
u32 JOAN_CDECL stub_564d10() { return crt_atexit(fn::code_ptr(fn::fn_564d30)); }
u32 JOAN_CDECL stub_564da0() { return crt_atexit(fn::code_ptr(fn::fn_564dc0)); }
u32 JOAN_CDECL stub_564e30() { return crt_atexit(fn::code_ptr(fn::fn_564e50)); }
u32 JOAN_CDECL stub_564ec0() { return crt_atexit(fn::code_ptr(fn::fn_564ee0)); }
u32 JOAN_CDECL stub_564f50() { return crt_atexit(fn::code_ptr(fn::fn_564f70)); }
u32 JOAN_CDECL stub_564fe0() { return crt_atexit(fn::code_ptr(fn::fn_565000)); }
u32 JOAN_CDECL stub_565070() { return crt_atexit(fn::code_ptr(fn::fn_565090)); }
u32 JOAN_CDECL stub_565100() { return crt_atexit(fn::code_ptr(fn::fn_565120)); }
u32 JOAN_CDECL stub_565190() { return crt_atexit(fn::code_ptr(fn::fn_5651b0)); }
u32 JOAN_CDECL stub_565220() { return crt_atexit(fn::code_ptr(fn::fn_565240)); }
u32 JOAN_CDECL stub_5652b0() { return crt_atexit(fn::code_ptr(fn::fn_5652d0)); }
u32 JOAN_CDECL stub_565340() { return crt_atexit(fn::code_ptr(fn::fn_565360)); }
u32 JOAN_CDECL stub_5653d0() { return crt_atexit(fn::code_ptr(fn::fn_5653f0)); }
u32 JOAN_CDECL stub_565460() { return crt_atexit(fn::code_ptr(fn::fn_565480)); }
u32 JOAN_CDECL stub_5654f0() { return crt_atexit(fn::code_ptr(fn::fn_565510)); }
u32 JOAN_CDECL stub_565580() { return crt_atexit(fn::code_ptr(fn::fn_5655a0)); }

#define B(name, addr, fn) JOAN_REPLACE("game/b105_08", name, addr, fn)
B("ctor_564300", 0x00564300, &stub_564300);
B("dtor_564350", 0x00564350, &stub_564350);
B("ctor_564390", 0x00564390, &stub_564390);
B("dtor_5643e0", 0x005643e0, &stub_5643e0);
B("ctor_564420", 0x00564420, &stub_564420);
B("dtor_564470", 0x00564470, &stub_564470);
B("ctor_5644b0", 0x005644b0, &stub_5644b0);
B("dtor_564500", 0x00564500, &stub_564500);
B("ctor_564540", 0x00564540, &stub_564540);
B("dtor_564590", 0x00564590, &stub_564590);
B("ctor_5645d0", 0x005645d0, &stub_5645d0);
B("dtor_564620", 0x00564620, &stub_564620);
B("ctor_564660", 0x00564660, &stub_564660);
B("ctor_5646f0", 0x005646f0, &stub_5646f0);
B("dtor_564740", 0x00564740, &stub_564740);
B("ctor_564780", 0x00564780, &stub_564780);
B("dtor_5647d0", 0x005647d0, &stub_5647d0);
B("ctor_564810", 0x00564810, &stub_564810);
B("ctor_5648a0", 0x005648a0, &stub_5648a0);
B("dtor_5648f0", 0x005648f0, &stub_5648f0);
B("ctor_564930", 0x00564930, &stub_564930);
B("dtor_564980", 0x00564980, &stub_564980);
B("ctor_5649c0", 0x005649c0, &stub_5649c0);
B("dtor_564a10", 0x00564a10, &stub_564a10);
B("ctor_564a50", 0x00564a50, &stub_564a50);
B("ctor_564b70", 0x00564b70, &stub_564b70);
B("dtor_564c10", 0x00564c10, &stub_564c10);
B("atexit_564c80", 0x00564c80, &stub_564c80);
B("dtor_564ca0", 0x00564ca0, &stub_564ca0);
B("atexit_564d10", 0x00564d10, &stub_564d10);
B("dtor_564d30", 0x00564d30, &stub_564d30);
B("atexit_564da0", 0x00564da0, &stub_564da0);
B("dtor_564dc0", 0x00564dc0, &stub_564dc0);
B("atexit_564e30", 0x00564e30, &stub_564e30);
B("dtor_564e50", 0x00564e50, &stub_564e50);
B("atexit_564ec0", 0x00564ec0, &stub_564ec0);
B("dtor_564ee0", 0x00564ee0, &stub_564ee0);
B("atexit_564f50", 0x00564f50, &stub_564f50);
B("dtor_564f70", 0x00564f70, &stub_564f70);
B("atexit_564fe0", 0x00564fe0, &stub_564fe0);
B("dtor_565000", 0x00565000, &stub_565000);
B("atexit_565070", 0x00565070, &stub_565070);
B("dtor_565090", 0x00565090, &stub_565090);
B("atexit_565100", 0x00565100, &stub_565100);
B("dtor_565120", 0x00565120, &stub_565120);
B("atexit_565190", 0x00565190, &stub_565190);
B("dtor_5651b0", 0x005651b0, &stub_5651b0);
B("atexit_565220", 0x00565220, &stub_565220);
B("dtor_565240", 0x00565240, &stub_565240);
B("atexit_5652b0", 0x005652b0, &stub_5652b0);
B("dtor_5652d0", 0x005652d0, &stub_5652d0);
B("atexit_565340", 0x00565340, &stub_565340);
B("dtor_565360", 0x00565360, &stub_565360);
B("atexit_5653d0", 0x005653d0, &stub_5653d0);
B("dtor_5653f0", 0x005653f0, &stub_5653f0);
B("atexit_565460", 0x00565460, &stub_565460);
B("dtor_565480", 0x00565480, &stub_565480);
B("atexit_5654f0", 0x005654f0, &stub_5654f0);
B("dtor_565510", 0x00565510, &stub_565510);
B("atexit_565580", 0x00565580, &stub_565580);
#undef B

}  // namespace game_b105_08_2
}  // namespace

// ---------------------------------------- from modules/game_b107_11.cpp (game/b107_11)
namespace {
namespace game_b107_11 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---------------------------------------------------------------- static initialiser pairs
template <fn::Code Ctor, fn::Code Reg>
i32 JOAN_CDECL init_pair() {
    cdecl_call<i32>(Ctor);
    return cdecl_call<i32>(Reg);
}

// 0x00566cb0
std::intptr_t JOAN_CDECL init_566cb0() { return joan::orig::arg::as<i32>(joan::orig::as_txt_res(obj(d::g_strings))->construct()); }  // 0x00566d40

#define B(name, addr, fn) JOAN_REPLACE("game/b107_11", name, addr, fn)
B("init_564370", 0x00564370, (&init_pair<fn::fn_564390, fn::StaticInit_atexit_5643e0>));
B("init_564de0", 0x00564de0, (&init_pair<fn::init_564e00, fn::fn_564e30>));
B("init_565800", 0x00565800, (&init_pair<fn::init_565820, fn::fn_565850>));
B("init_566220", 0x00566220, (&init_pair<fn::clear_a51608, fn::fn_566270>));
B("init_566cb0", 0x00566cb0, &init_566cb0);
B("init_567700", 0x00567700, (&init_pair<fn::obj_stub_567730, fn::init_atexit_567750>));
B("init_567b90", 0x00567b90, (&init_pair<fn::init_567bb0, fn::init_atexit_567be0>));
B("init_5685b0", 0x005685b0, (&init_pair<fn::init_5685d0, fn::init_atexit_568600>));
B("init_569060", 0x00569060, (&init_pair<fn::init_569080, fn::init_atexit_5690b0>));
B("init_569b10", 0x00569b10, (&init_pair<fn::init_569b30, fn::init_atexit_569b60>));
B("init_56a5c0", 0x0056a5c0, (&init_pair<fn::obj_stub_56a5e0, fn::init_56a600>));
B("init_56b040", 0x0056b040, (&init_pair<fn::init_obj_56b060, fn::init_56b080>));
B("init_56c260", 0x0056c260, (&init_pair<fn::init_obj_56c310, fn::init_atexit_56c330>));
B("init_56cc50", 0x0056cc50, (&init_pair<fn::init_56cc70, fn::init_atexit_56cca0>));
#undef B

}  // namespace game_b107_11
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

i32 JOAN_CDECL atexit_5643e0() { return joan::crt::atexit(fn::code_ptr(fn::fn_5643e0)); }  // 0x005643b0
i32 JOAN_CDECL atexit_564980() { return joan::crt::atexit(fn::code_ptr(fn::fn_564980)); }  // 0x00564950

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("StaticInit::atexit_5643e0", 0x005643b0, &atexit_5643e0);
B94("StaticInit::atexit_564980", 0x00564950, &atexit_564980);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b107_12.cpp (game/b107_12)
namespace {
namespace game_b107_12 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// Two direct calls (cdecl, no args), eax of the second.
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

// 0x0056cd60
u32 JOAN_CDECL init_56cd60() { return joan::orig::arg::as<u32>(joan::orig::as_panel(obj(d::g_msg_log))->destruct()); }  // 0x0056cf50

#define B(name, addr, fn) JOAN_REPLACE("game/b107_12", name, addr, fn)
B("StaticInit::init_564400", 0x00564400, (&init_pair<fn::fn_564420, fn::init_564440>));
B("StaticInit::init_564e70", 0x00564e70, (&init_pair<fn::StaticInit_clear_a50600, fn::fn_564ec0>));
B("StaticInit::init_565890", 0x00565890, (&init_pair<fn::init_5658b0, fn::fn_5658e0>));
B("StaticInit::init_5662b0", 0x005662b0, (&init_pair<fn::init_5662d0, fn::fn_566300>));
B("StaticInit::init_566d60", 0x00566d60, (&init_pair<fn::init_566d80, fn::init_atexit_566db0>));
B("StaticInit::init_567790", 0x00567790, (&init_pair<fn::obj_stub_5677c0, fn::init_5677e0>));
B("StaticInit::init_567c20", 0x00567c20, (&init_pair<fn::init_567c40, fn::init_atexit_567c70>));
B("StaticInit::init_568640", 0x00568640, (&init_pair<fn::init_568660, fn::init_atexit_568690>));
B("StaticInit::init_5690f0", 0x005690f0, (&init_pair<fn::init_569110, fn::init_atexit_569140>));
B("StaticInit::init_569ba0", 0x00569ba0, (&init_pair<fn::init_569bc0, fn::init_atexit_569bf0>));
B("StaticInit::init_56a650", 0x0056a650, (&init_pair<fn::obj_stub_56a670, fn::init_56a690>));
B("StaticInit::init_56b0d0", 0x0056b0d0, (&init_pair<fn::init_obj_56b0f0, fn::init_56b110>));
B("StaticInit::init_56cd60", 0x0056cd60, &init_56cd60);
#undef B

}  // namespace game_b107_12
}  // namespace

// ---------------------------------------- from modules/game_b107_13.cpp (game/b107_13)
namespace {
namespace game_b107_13 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- static initialisers: two direct calls, eax of the second ----
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init2() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_13", name, addr, fn)
B("init_564490", 0x00564490, (&init2<fn::fn_5644b0, fn::atexit_564500>));
B("init_564f00", 0x00564f00, (&init2<fn::init_564f20, fn::fn_564f50>));
B("init_565920", 0x00565920, (&init2<fn::init_565940, fn::fn_565970>));
B("init_566340", 0x00566340, (&init2<fn::init_566360, fn::fn_566390>));
B("init_566e50", 0x00566e50, (&init2<fn::init_566e70, fn::init_atexit_566ea0>));
B("init_567cb0", 0x00567cb0, (&init2<fn::init_567cd0, fn::init_atexit_567d00>));
B("init_5686d0", 0x005686d0, (&init2<fn::init_5686f0, fn::init_atexit_568720>));
B("init_569180", 0x00569180, (&init2<fn::init_5691a0, fn::init_atexit_5691d0>));
B("init_569c30", 0x00569c30, (&init2<fn::init_569c50, fn::init_atexit_569c80>));
B("init_56a6e0", 0x0056a6e0, (&init2<fn::obj_stub_56a700, fn::init_56a720>));
B("init_56b160", 0x0056b160, (&init2<fn::init_obj_56b180, fn::init_56b1a0>));
B("init_56bb00", 0x0056bb00, (&init2<fn::init_obj_56bb30, fn::init_atexit_56bb50>));
B("init_56cd80", 0x0056cd80, (&init2<fn::init_56cda0, fn::init_atexit_56cdd0>));
#undef B

}  // namespace game_b107_13
}  // namespace

// ---------------------------------------- from modules/game_b107_15.cpp (game/b107_15)
namespace {
namespace game_b107_15 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- CRT initialiser pairs (eax of the second call, ignored by _initterm) ----
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_15", name, addr, fn)
B("init_564520", 0x00564520, (&init_pair<fn::fn_564540, fn::init_564560>));
B("init_564f90", 0x00564f90, (&init_pair<fn::init_564fb0, fn::fn_564fe0>));
B("init_5659b0", 0x005659b0, (&init_pair<fn::clear_a57f40, fn::fn_565a00>));
B("init_5663d0", 0x005663d0, (&init_pair<fn::init_5663f0, fn::fn_566420>));
B("init_566f90", 0x00566f90, (&init_pair<fn::init_566fb0, fn::init_atexit_566fd0>));
B("init_567d40", 0x00567d40, (&init_pair<fn::init_567d60, fn::init_atexit_567d90>));
B("init_568760", 0x00568760, (&init_pair<fn::init_568780, fn::init_atexit_5687b0>));
B("init_569210", 0x00569210, (&init_pair<fn::init_569230, fn::init_atexit_569260>));
B("init_569cc0", 0x00569cc0, (&init_pair<fn::init_569ce0, fn::init_atexit_569d10>));
B("init_56a770", 0x0056a770, (&init_pair<fn::obj_stub_56a790, fn::init_56a7b0>));
B("init_56b1f0", 0x0056b1f0, (&init_pair<fn::init_obj_56b210, fn::init_56b230>));
B("init_56bc80", 0x0056bc80, (&init_pair<fn::init_obj_56bcb0, fn::init_56bcd0>));
B("init_56ce20", 0x0056ce20, (&init_pair<fn::StaticInit_ctor_a411d0, fn::init_56ce60>));
#undef B

}  // namespace game_b107_15
}  // namespace

// ---------------------------------------- from modules/game_b107_04.cpp (game/b107_04)
namespace {
namespace game_b107_04 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- CRT initialisers ----
template <fn::Code First, fn::Code Second>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(First);
    return cdecl_call<u32>(Second);
}

u32 JOAN_CDECL init_56ce90() { return joan::orig::arg::as<u32>(joan::orig::as_save_menu(obj(d::g_save_menu))->destruct()); }  // 0x004375a0

#define B(name, addr, fn) JOAN_REPLACE("game/b107_04", name, addr, fn)
B("init_5645b0", 0x005645b0, (&init_pair<fn::fn_5645d0, fn::StaticInit_atexit_564620>));
B("init_565020", 0x00565020, (&init_pair<fn::init_565040, fn::fn_565070>));
B("init_565a40", 0x00565a40, (&init_pair<fn::init_565a60, fn::fn_565a90>));
B("init_566460", 0x00566460, (&init_pair<fn::init_566480, fn::fn_5664b0>));
B("init_567010", 0x00567010, (&init_pair<fn::init_567030, fn::init_atexit_567060>));
B("init_567dd0", 0x00567dd0, (&init_pair<fn::init_567df0, fn::init_atexit_567e20>));
B("init_5687f0", 0x005687f0, (&init_pair<fn::init_568810, fn::init_atexit_568840>));
B("init_5692a0", 0x005692a0, (&init_pair<fn::init_5692c0, fn::init_atexit_5692f0>));
B("init_569d50", 0x00569d50, (&init_pair<fn::init_569d70, fn::init_atexit_569da0>));
B("init_56a800", 0x0056a800, (&init_pair<fn::obj_stub_56a820, fn::init_56a840>));
B("init_56ae70", 0x0056ae70, (&init_pair<fn::obj_stub_56aea0, fn::init_atexit_56aec0>));
B("init_56b280", 0x0056b280, (&init_pair<fn::init_obj_56b2a0, fn::init_56b2c0>));
B("init_56cb30", 0x0056cb30, (&init_pair<fn::init_obj_56cbe0, fn::init_atexit_56cc00>));
B("init_56ce90", 0x0056ce90, &init_56ce90);
#undef B

}  // namespace game_b107_04
}  // namespace

// ---------------------------------------- from modules/game_b107_19.cpp (game/b107_19)
namespace {
namespace game_b107_19 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- static initialiser pairs ----
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

// 0x0056ba00
std::uintptr_t JOAN_CDECL init_56ba00() { return joan::orig::arg::as<u32>(joan::orig::as_message_indicator(obj(d::g_message_indicator))->construct()); }  // 0x004af4a0

#define B(name, addr, fn) JOAN_REPLACE("game/b107_19", name, addr, fn)
B("init_pair_564640", 0x00564640, (&init_pair<fn::fn_564660, fn::init_564680>));
B("init_pair_5650b0", 0x005650b0, (&init_pair<fn::init_5650d0, fn::fn_565100>));
B("init_pair_565ad0", 0x00565ad0, (&init_pair<fn::init_565af0, fn::fn_565b20>));
B("init_pair_5664f0", 0x005664f0, (&init_pair<fn::init_566510, fn::fn_566540>));
B("init_pair_5670a0", 0x005670a0, (&init_pair<fn::init_5670c0, fn::init_atexit_5670f0>));
B("init_pair_567e60", 0x00567e60, (&init_pair<fn::init_567e80, fn::init_atexit_567eb0>));
B("init_pair_568880", 0x00568880, (&init_pair<fn::init_5688a0, fn::init_atexit_5688d0>));
B("init_pair_569330", 0x00569330, (&init_pair<fn::init_569350, fn::init_atexit_569380>));
B("init_pair_569de0", 0x00569de0, (&init_pair<fn::init_569e00, fn::init_atexit_569e30>));
B("init_pair_56a890", 0x0056a890, (&init_pair<fn::obj_stub_56a8b0, fn::init_56a8d0>));
B("init_56ba00", 0x0056ba00, &init_56ba00);
B("init_pair_56bf60", 0x0056bf60, (&init_pair<fn::init_obj_56bf90, fn::init_atexit_56bfb0>));
B("init_pair_56d010", 0x0056d010, (&init_pair<fn::init_56d030, fn::StaticInit_atexit_56d080>));
#undef B

}  // namespace game_b107_19
}  // namespace

// ---------------------------------------- from modules/game_b107_20.cpp (game/b107_20)
namespace {
namespace game_b107_20 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- CRT initialiser-table entries ----
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}
template <fn::Code A>
u32 JOAN_CDECL init_one() {
    return cdecl_call<u32>(A);
}

// 0x005646b0 (atexit handler)
u32 JOAN_CDECL exit_5646b0() { return joan::orig::arg::as<u32>(joan::orig::as_panel(obj(d::g_sprites_a46128))->dtor_6317e0()); }  // 0x006317e0

#define B(name, addr, fn) JOAN_REPLACE("game/b107_20", name, addr, fn)
B("exit_5646b0", 0x005646b0, &exit_5646b0);
B("init_565140", 0x00565140, (&init_pair<fn::clear_a403e0, fn::fn_565190>));
B("init_565b60", 0x00565b60, (&init_pair<fn::StaticInit_clear_a58ac0, fn::fn_565bb0>));
B("init_566580", 0x00566580, (&init_pair<fn::init_5665a0, fn::fn_5665d0>));
B("init_5671d0", 0x005671d0, (&init_pair<fn::init_5671f0, fn::init_atexit_567220>));
B("init_567ef0", 0x00567ef0, (&init_pair<fn::init_567f10, fn::init_atexit_567f40>));
B("init_568910", 0x00568910, (&init_pair<fn::init_568930, fn::init_atexit_568960>));
B("init_5693c0", 0x005693c0, (&init_pair<fn::init_5693e0, fn::init_atexit_569410>));
B("init_569ec0", 0x00569ec0, (&init_one<fn::obj_stub_569ee0>));
B("init_56a920", 0x0056a920, (&init_pair<fn::obj_stub_56a940, fn::init_56a960>));
B("init_56ba70", 0x0056ba70, (&init_pair<fn::init_obj_56ba90, fn::init_56bab0>));
B("init_56c090", 0x0056c090, (&init_pair<fn::init_obj_56c0c0, fn::init_56c0e0>));
B("init_56d140", 0x0056d140, (&init_pair<fn::fn_56d160, fn::StaticInit_atexit_56d1b0>));
#undef B

}  // namespace game_b107_20
}  // namespace

// ---------------------------------------- from modules/game_b107_10.cpp (game/b107_10)
namespace {
namespace game_b107_10 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init2() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_10", name, addr, fn)
B("init_0x005646d0", 0x005646d0, (&init2<fn::fn_5646f0, fn::init_564710>));
B("init_0x005651d0", 0x005651d0, (&init2<fn::init_5651f0, fn::fn_565220>));
B("init_0x00565bf0", 0x00565bf0, (&init2<fn::init_565c10, fn::fn_565c40>));
B("init_0x00566610", 0x00566610, (&init2<fn::clear_a48fb0, fn::fn_566660>));
B("init_0x00567260", 0x00567260, (&init2<fn::fn_567280, fn::init_atexit_5672b0>));
B("init_0x00567f80", 0x00567f80, (&init2<fn::init_567fa0, fn::init_atexit_567fd0>));
B("init_0x005689a0", 0x005689a0, (&init2<fn::init_5689c0, fn::init_atexit_5689f0>));
B("init_0x00569450", 0x00569450, (&init2<fn::init_569470, fn::init_atexit_5694a0>));
B("init_0x00569f00", 0x00569f00, (&init2<fn::init_569f20, fn::init_atexit_569f50>));
B("init_0x0056a9b0", 0x0056a9b0, (&init2<fn::obj_stub_56a9d0, fn::init_56a9f0>));
B("init_0x0056b310", 0x0056b310, (&init2<fn::fn_56b3b0, fn::init_atexit_56b3e0>));
B("init_0x0056bba0", 0x0056bba0, (&init2<fn::init_56bbc0, fn::init_atexit_56bc40>));
B("init_0x0056c410", 0x0056c410, (&init2<fn::init_obj_56c440, fn::init_56c460>));
B("init_0x0056d300", 0x0056d300, (&init2<fn::StaticInit_ctor_a52930, fn::init_56d340>));
#undef B

}  // namespace game_b107_10
}  // namespace

// ---------------------------------------- from modules/game_b107_16.cpp (game/b107_16)
namespace {
namespace game_b107_16 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- static initialiser pairs (direct calls, no thunks) ----
template <fn::Code Ctor, fn::Code Reg>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(Ctor);
    return cdecl_call<u32>(Reg);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_16", name, addr, fn)
B("init_564760", 0x00564760, (&init_pair<fn::fn_564780, fn::init_5647a0>));
B("init_565260", 0x00565260, (&init_pair<fn::init_565280, fn::fn_5652b0>));
B("init_565c80", 0x00565c80, (&init_pair<fn::init_565ca0, fn::fn_565cd0>));
B("init_5666a0", 0x005666a0, (&init_pair<fn::init_5666c0, fn::init_atexit_5666f0>));
B("init_5672f0", 0x005672f0, (&init_pair<fn::init_567310, fn::init_atexit_567340>));
B("init_568010", 0x00568010, (&init_pair<fn::init_568030, fn::init_atexit_568060>));
B("init_568a30", 0x00568a30, (&init_pair<fn::init_568a50, fn::init_atexit_568a80>));
B("init_5694e0", 0x005694e0, (&init_pair<fn::init_569500, fn::init_atexit_569530>));
B("init_569f90", 0x00569f90, (&init_pair<fn::init_569fb0, fn::init_atexit_569fe0>));
B("init_56aa40", 0x0056aa40, (&init_pair<fn::obj_stub_56aa60, fn::init_56aa80>));
B("init_56be30", 0x0056be30, (&init_pair<fn::init_56be50, fn::init_atexit_56be80>));
B("init_56c6d0", 0x0056c6d0, (&init_pair<fn::init_obj_56c770, fn::init_atexit_56c790>));
B("init_56c8c0", 0x0056c8c0, (&init_pair<fn::init_obj_56c8f0, fn::init_56c910>));
B("init_56d430", 0x0056d430, (&init_pair<fn::StaticInit_ctor_a50808, fn::StaticInit_atexit_56d490>));
#undef B

}  // namespace game_b107_16
}  // namespace

// ---------------------------------------- from modules/game_b107_03.cpp (game/b107_03)
namespace {
namespace game_b107_03 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// CRT initialiser pair: call first, return eax of second (both direct calls, no thunk).
u32 JOAN_CDECL init_pair_5647f0() {
    cdecl_call<u32>(fn::fn_564810);
    return joan::orig::arg::as<u32>(joan::orig::atexit_564860());
}
u32 JOAN_CDECL init_pair_5652f0() {
    joan::orig::clear_a5b9a8();
    return cdecl_call<u32>(fn::fn_565340);
}
u32 JOAN_CDECL init_pair_565d10() {
    joan::orig::init_565d30();
    return cdecl_call<u32>(fn::fn_565d60);
}
u32 JOAN_CDECL init_pair_566730() {
    joan::orig::init_566750();
    return cdecl_call<u32>(fn::init_atexit_566780);
}
u32 JOAN_CDECL init_pair_567380() {
    joan::orig::init_5673a0();
    return cdecl_call<u32>(fn::init_atexit_5673d0);
}
u32 JOAN_CDECL init_pair_5680a0() {
    joan::orig::init_5680c0();
    return cdecl_call<u32>(fn::init_atexit_5680f0);
}
u32 JOAN_CDECL init_pair_568ac0() {
    joan::orig::init_568ae0();
    return cdecl_call<u32>(fn::init_atexit_568b10);
}
u32 JOAN_CDECL init_pair_569570() {
    joan::orig::init_569590();
    return cdecl_call<u32>(fn::init_atexit_5695c0);
}
u32 JOAN_CDECL init_pair_56a030() {
    joan::orig::obj_stub_56a050();
    return cdecl_call<u32>(fn::init_atexit_56a070);
}
u32 JOAN_CDECL init_pair_56aad0() {
    joan::orig::obj_stub_56aaf0();
    joan::orig::init_56ab10();
    return 0;  // void callee; _initterm ignores the eax
}
u32 JOAN_CDECL init_pair_56bed0() {
    joan::orig::init_obj_56bef0();
    joan::orig::init_56bf10();
    return 0;  // void callee; _initterm ignores the eax
}
u32 JOAN_CDECL init_pair_56d4c0() {
    joan::orig::init_56d4e0();
    return cdecl_call<u32>(fn::StaticInit_atexit_56d530);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_03", name, addr, fn)
B("init_pair_5647f0", 0x005647f0, &init_pair_5647f0);
B("init_pair_5652f0", 0x005652f0, &init_pair_5652f0);
B("init_pair_565d10", 0x00565d10, &init_pair_565d10);
B("init_pair_566730", 0x00566730, &init_pair_566730);
B("init_pair_567380", 0x00567380, &init_pair_567380);
B("init_pair_5680a0", 0x005680a0, &init_pair_5680a0);
B("init_pair_568ac0", 0x00568ac0, &init_pair_568ac0);
B("init_pair_569570", 0x00569570, &init_pair_569570);
B("init_pair_56a030", 0x0056a030, &init_pair_56a030);
B("init_pair_56aad0", 0x0056aad0, &init_pair_56aad0);
B("init_pair_56bed0", 0x0056bed0, &init_pair_56bed0);
B("init_pair_56d4c0", 0x0056d4c0, &init_pair_56d4c0);
#undef B

}  // namespace game_b107_03
}  // namespace

// ---------------------------------------- from modules/game_b107_17.cpp (game/b107_17)
namespace {
namespace game_b107_17 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- static initialisers / destructor stubs ----
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}
template <auto G, fn::Code Thunk>
u32 JOAN_CDECL obj_call() { return thiscall<u32>(Thunk, obj(G)); }

struct H {
    // 0x0056e6b0
    u32 fwd_56e6b0() { return joan::orig::arg::as<u32>(joan::orig::as_obj_menu(this)->release_71e800()); }  // 0x0071e800
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_17", name, addr, fn)
B("dtor_stub_564860", 0x00564860, (&obj_call<d::g_mouse_cursor, fn::Obj_dtor_454210>));  // 0x00454210
B("init_565380", 0x00565380, (&init_pair<fn::init_5653a0, fn::fn_5653d0>));
B("init_565da0", 0x00565da0, (&init_pair<fn::clear_a48a68, fn::fn_565df0>));
B("init_5667c0", 0x005667c0, (&init_pair<fn::init_5667e0, fn::init_atexit_566810>));
B("init_567410", 0x00567410, (&init_pair<fn::init_567430, fn::init_atexit_567460>));
B("init_568130", 0x00568130, (&init_pair<fn::init_568150, fn::init_atexit_568180>));
B("init_568b50", 0x00568b50, (&init_pair<fn::init_568b70, fn::init_atexit_568ba0>));
B("init_569600", 0x00569600, (&init_pair<fn::init_569620, fn::init_atexit_569650>));
B("init_56a0b0", 0x0056a0b0, (&init_pair<fn::obj_stub_56a0d0, fn::init_56a0f0>));
B("dtor_stub_56ab80", 0x0056ab80, (&obj_call<d::g_orders, fn::Array_init_static_4e8a50>));  // 0x004e8a50
B("init_56c000", 0x0056c000, (&init_pair<fn::init_56c020, fn::init_atexit_56c050>));
B("init_56cce0", 0x0056cce0, (&init_pair<fn::init_obj_56cd10, fn::init_56cd30>));
B("dtor_stub_56d580", 0x0056d580, (&obj_call<d::g_chat_box, fn::Obj_self_45b550>));  // 0x0045b550
B("Obj::fwd_56e6b0", 0x0056e6b0, &H::fwd_56e6b0);
#undef B

}  // namespace game_b107_17
}  // namespace

// ---------------------------------------- from modules/game_b107_18.cpp (game/b107_18)
namespace {
namespace game_b107_18 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

u32 JOAN_CDECL init_56d5c0() { joan::orig::as_chat_box(obj(d::g_chat_box))->construct(); return 0; }  // 0x0045b570 (atexit callback: result ignored)

template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_18", name, addr, fn)
B("init_564880", 0x00564880, (&init_pair<fn::fn_5648a0, fn::init_5648c0>));
B("init_564b40", 0x00564b40, (&init_pair<fn::fn_564b70, fn::init_564b90>));
B("init_565410", 0x00565410, (&init_pair<fn::init_565430, fn::fn_565460>));
B("init_565e30", 0x00565e30, (&init_pair<fn::init_565e50, fn::fn_565e80>));
B("init_566850", 0x00566850, (&init_pair<fn::init_566870, fn::init_atexit_5668a0>));
B("init_567610", 0x00567610, (&init_pair<fn::init_567630, fn::init_atexit_5676c0>));
B("init_5681c0", 0x005681c0, (&init_pair<fn::init_5681e0, fn::init_atexit_568210>));
B("init_568be0", 0x00568be0, (&init_pair<fn::init_568c00, fn::init_atexit_568c30>));
B("init_569690", 0x00569690, (&init_pair<fn::init_5696b0, fn::init_atexit_5696e0>));
B("init_56a140", 0x0056a140, (&init_pair<fn::obj_stub_56a160, fn::init_56a180>));
B("init_56abf0", 0x0056abf0, (&init_pair<fn::obj_stub_56ac10, fn::init_56ac30>));
B("init_56c130", 0x0056c130, (&init_pair<fn::init_56c150, fn::init_atexit_56c180>));
B("init_56ceb0", 0x0056ceb0, (&init_pair<fn::StaticInit_ctor_a53ff8, fn::StaticInit_atexit_56cf20>));
B("init_56d5c0", 0x0056d5c0, &init_56d5c0);
#undef B

}  // namespace game_b107_18
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
namespace {
namespace game_b94_05 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

JOAN_COUNTER(c_dialog_dtor, "Dialog::~Dialog");

// ---- static initialisers ----
using joan::orig::helpers::text_desc_clear;  // 0x00623dc0
using joan::orig::helpers::rect9_clear;      // 0x00620a20
void crt_atexit(const void* fn) { joan::crt::atexit(fn); }

void JOAN_CDECL init_564fb0() { text_desc_clear(d::g_a50580); }
void JOAN_CDECL init_565550() { text_desc_clear(d::g_a52888); }
void JOAN_CDECL init_565af0() { text_desc_clear(d::g_font_a489c8); }
void JOAN_CDECL init_566090() { text_desc_clear(d::g_a4d518); }
void JOAN_CDECL init_5665a0() { text_desc_clear(d::g_font_goudy_bold_48); }
void JOAN_CDECL init_566b40() { text_desc_clear(d::g_font_times_10); }
void JOAN_CDECL init_567b20() { rect9_clear(d::g_bmp_res_menu1024); }
void JOAN_CDECL init_5680c0() { rect9_clear(d::g_bmp_res_msglog); }
void JOAN_CDECL init_568660() { rect9_clear(d::g_bmp_res_victory); }
void JOAN_CDECL init_568c00() { rect9_clear(d::g_bmp_res_optmenub); }
void JOAN_CDECL init_5691a0() { rect9_clear(d::g_bmp_res_sl_bg2); }
void JOAN_CDECL init_569740() { rect9_clear(d::g_sprites_a528c8); }
void JOAN_CDECL init_56a210() { crt_atexit(fn::code_ptr(fn::obj_stub_56a240)); }
void JOAN_CDECL init_56a7b0() { crt_atexit(fn::code_ptr(fn::obj_stub_56a7e0)); }
void JOAN_CDECL init_56ad50() { crt_atexit(fn::code_ptr(fn::atexit_56ad80)); }
void JOAN_CDECL init_56bf10() { crt_atexit(fn::code_ptr(fn::dtor_stub_56bf40)); }
void JOAN_CDECL init_56d340() { crt_atexit(fn::code_ptr(fn::StaticInit_dtor_a52930)); }

struct H {
    // 0x0056e3a0
    void dialog_dtor() {
        c_dialog_dtor.hit();
        joan::orig::as_ingame_menu(this)->release();                // 0x006f3270
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->caption)->dtor_6294c0();     // 0x006294c0
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->title)->dtor_6294c0();     // 0x006294c0
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->back_button)->dtor_6211a0();     // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->button7)->dtor_6211a0();     // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->button6)->dtor_6211a0();     // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->button5)->dtor_6211a0();     // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->button4)->dtor_6211a0();      // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->button3)->dtor_6211a0();      // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->button2)->dtor_6211a0();      // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->button1)->dtor_6211a0();      // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::IngameMenu>(this)->button0)->dtor_6211a0();       // 0x006211a0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("init_564fb0", 0x00564fb0, &init_564fb0);
B("init_565550", 0x00565550, &init_565550);
B("init_565af0", 0x00565af0, &init_565af0);
B("init_566090", 0x00566090, &init_566090);
B("init_5665a0", 0x005665a0, &init_5665a0);
B("init_566b40", 0x00566b40, &init_566b40);
B("init_567b20", 0x00567b20, &init_567b20);
B("init_5680c0", 0x005680c0, &init_5680c0);
B("init_568660", 0x00568660, &init_568660);
B("init_568c00", 0x00568c00, &init_568c00);
B("init_5691a0", 0x005691a0, &init_5691a0);
B("init_569740", 0x00569740, &init_569740);
B("init_56a210", 0x0056a210, &init_56a210);
B("init_56a7b0", 0x0056a7b0, &init_56a7b0);
B("init_56ad50", 0x0056ad50, &init_56ad50);
B("init_56bf10", 0x0056bf10, &init_56bf10);
B("init_56d340", 0x0056d340, &init_56d340);
B("Dialog::~Dialog", 0x0056e3a0, &H::dialog_dtor);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
namespace {
namespace game_b95_05 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- static initialisers ----
using joan::orig::helpers::text_desc_clear;  // 0x00623dc0
using joan::orig::helpers::rect9_clear;      // 0x00620a20
void crt_atexit(const void* fn) { joan::crt::atexit(fn); }

void JOAN_CDECL init_565f70() { text_desc_clear(d::g_font_goudy18_27_white); }
void JOAN_CDECL init_566360() { text_desc_clear(d::g_font_goudy12_18_target_unit); }
void JOAN_CDECL init_5667e0() { text_desc_clear(d::g_a4d4d8); }
void JOAN_CDECL init_567a00() { rect9_clear(d::g_bmp_res_hurt); }
void JOAN_CDECL init_567df0() { rect9_clear(d::g_bmp_res_invent); }
void JOAN_CDECL init_568270() { rect9_clear(d::g_bmp_res_kill); }
void JOAN_CDECL init_5685d0() { rect9_clear(d::g_bmp_res_vict); }
void JOAN_CDECL init_568a50() { rect9_clear(d::g_bmp_commele2); }
void JOAN_CDECL init_568e40() { rect9_clear(d::g_bmp_res_ld_cp); }
void JOAN_CDECL init_5692c0() { rect9_clear(d::g_bmp_res_m1); }
void JOAN_CDECL init_569620() { rect9_clear(d::g_bmp_res_m7); }
void JOAN_CDECL init_569aa0() { rect9_clear(d::g_bmp_res_strategy); }
void JOAN_CDECL init_56a3c0() { crt_atexit(fn::code_ptr(fn::obj_stub_56a3f0)); }
void JOAN_CDECL init_56a840() { crt_atexit(fn::code_ptr(fn::obj_stub_56a870)); }
void JOAN_CDECL init_56aba0() { crt_atexit(fn::code_ptr(fn::obj_stub_56abd0)); }
void JOAN_CDECL init_56b230() { crt_atexit(fn::code_ptr(fn::dtor_stub_56b260)); }
void JOAN_CDECL init_56c210() { crt_atexit(fn::code_ptr(fn::dtor_stub_56c240)); }
void JOAN_CDECL init_56d0f0() { crt_atexit(fn::code_ptr(fn::StaticInit_dtor_a54c20)); }

struct H {
    // 0x00566c60
    void* ctor_566c60() {
        joan::orig::as_txt_res(this)->construct_5a2000();  // 0x005a2000
        return this;
    }

    // 0x0056e1d0
    std::uint32_t dtor_56e1d0() { joan::orig::as_goal_panel(this)->release(); return 0; }  // 0x006df140 (atexit stub: result ignored)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("init_565f70", 0x00565f70, &init_565f70);
B("init_566360", 0x00566360, &init_566360);
B("init_5667e0", 0x005667e0, &init_5667e0);
B("Obj::ctor_566c60", 0x00566c60, &H::ctor_566c60);
B("init_567a00", 0x00567a00, &init_567a00);
B("init_567df0", 0x00567df0, &init_567df0);
B("init_568270", 0x00568270, &init_568270);
B("init_5685d0", 0x005685d0, &init_5685d0);
B("init_568a50", 0x00568a50, &init_568a50);
B("init_568e40", 0x00568e40, &init_568e40);
B("init_5692c0", 0x005692c0, &init_5692c0);
B("init_569620", 0x00569620, &init_569620);
B("init_569aa0", 0x00569aa0, &init_569aa0);
B("init_56a3c0", 0x0056a3c0, &init_56a3c0);
B("init_56a840", 0x0056a840, &init_56a840);
B("init_56aba0", 0x0056aba0, &init_56aba0);
B("init_56b230", 0x0056b230, &init_56b230);
B("init_56c210", 0x0056c210, &init_56c210);
B("init_56d0f0", 0x0056d0f0, &init_56d0f0);
B("Obj::dtor_56e1d0", 0x0056e1d0, &H::dtor_56e1d0);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
namespace {
namespace game_b96_05 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- static initialisers ----
void init_on(std::uintptr_t thunk, img::Addr g) { thiscall<std::uint32_t>(thunk, obj(g)); }
void crt_atexit(const void* fn) { joan::crt::atexit(fn); }

void JOAN_CDECL init_5663f0() { init_on(fn::code_value(fn::TextDesc_clear), d::g_font_goudy_bold_24); }  // 0x00623dc0
void JOAN_CDECL init_566750() { init_on(fn::code_value(fn::TextDesc_clear), d::g_font_goudy_bold_14); }  // 0x00623dc0
void JOAN_CDECL init_566bd0() { init_on(fn::code_value(fn::Obj_ctor_566c60), d::g_txt); }  // 0x00566c60
void JOAN_CDECL init_5671f0() { init_on(fn::code_value(fn::Ctrl_Ctrl_63f190), d::g_a4d598); }  // 0x0063f190
void JOAN_CDECL init_567970() { init_on(fn::code_value(fn::Rect9_clear), d::g_bmp_res_mgexp); }  // 0x00620a20
void JOAN_CDECL init_567e80() { init_on(fn::code_value(fn::Rect9_clear), d::g_bmp_res_iunitatt); }  // 0x00620a20
void JOAN_CDECL init_5681e0() { init_on(fn::code_value(fn::Rect9_clear), d::g_bmp_res_buy); }  // 0x00620a20
void JOAN_CDECL init_568540() { init_on(fn::code_value(fn::Rect9_clear), d::g_a5cd60); }  // 0x00620a20
void JOAN_CDECL init_5689c0() { init_on(fn::code_value(fn::Rect9_clear), d::g_bmp_commele); }  // 0x00620a20
void JOAN_CDECL init_568db0() { init_on(fn::code_value(fn::Rect9_clear), d::g_bmp_res_ld_bg); }  // 0x00620a20
void JOAN_CDECL init_569110() { init_on(fn::code_value(fn::Rect9_clear), d::g_bmp_res_sl_bg1); }  // 0x00620a20
void JOAN_CDECL init_569590() { init_on(fn::code_value(fn::Rect9_clear), d::g_bmp_res_m6); }  // 0x00620a20
void JOAN_CDECL init_569980() { init_on(fn::code_value(fn::Rect9_clear), d::g_bmp_debrief); }  // 0x00620a20
void JOAN_CDECL init_56cc70() { init_on(fn::code_value(fn::Obj_ctor_4ad6e0), d::g_names); }  // 0x004ad6e0
void JOAN_CDECL init_56a0f0() { crt_atexit(fn::code_ptr(fn::static_dtor_56a120)); }
void JOAN_CDECL init_56a570() { crt_atexit(fn::code_ptr(fn::obj_stub_56a5a0)); }
void JOAN_CDECL init_56a960() { crt_atexit(fn::code_ptr(fn::obj_stub_56a990)); }
void JOAN_CDECL init_56acc0() { crt_atexit(fn::code_ptr(fn::obj_stub_56acf0)); }
void JOAN_CDECL init_56b2c0() { crt_atexit(fn::code_ptr(fn::dtor_stub_56b2f0)); }
void JOAN_CDECL init_56c460() { crt_atexit(fn::code_ptr(fn::dtor_stub_56c540)); }
void JOAN_CDECL init_56d9e0() { crt_atexit(fn::code_ptr(fn::StaticInit_dtor_a48cc0)); }

struct H {
    // 0x0056e280
    std::uint32_t call_6e0520() { joan::orig::as_letter(this)->free(); return 0; }  // 0x006e0520 (atexit stub: result ignored)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("init_5663f0", 0x005663f0, &init_5663f0);
B("init_566750", 0x00566750, &init_566750);
B("init_566bd0", 0x00566bd0, &init_566bd0);
B("init_5671f0", 0x005671f0, &init_5671f0);
B("init_567970", 0x00567970, &init_567970);
B("init_567e80", 0x00567e80, &init_567e80);
B("init_5681e0", 0x005681e0, &init_5681e0);
B("init_568540", 0x00568540, &init_568540);
B("init_5689c0", 0x005689c0, &init_5689c0);
B("init_568db0", 0x00568db0, &init_568db0);
B("init_569110", 0x00569110, &init_569110);
B("init_569590", 0x00569590, &init_569590);
B("init_569980", 0x00569980, &init_569980);
B("init_56a0f0", 0x0056a0f0, &init_56a0f0);
B("init_56a570", 0x0056a570, &init_56a570);
B("init_56a960", 0x0056a960, &init_56a960);
B("init_56acc0", 0x0056acc0, &init_56acc0);
B("init_56b2c0", 0x0056b2c0, &init_56b2c0);
B("init_56c460", 0x0056c460, &init_56c460);
B("init_56cc70", 0x0056cc70, &init_56cc70);
B("init_56d9e0", 0x0056d9e0, &init_56d9e0);
B("Obj::call_6e0520", 0x0056e280, &H::call_6e0520);
#undef B

}  // namespace game_b96_05
}  // namespace

// ---------------------------------------- from modules/game_b99_06.cpp (game/b99_06)
namespace {
namespace game_b99_06 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- static initialisers ----
using joan::orig::helpers::text_desc_clear;  // 0x00623dc0
using joan::orig::helpers::rect9_clear;      // 0x00620a20
void crt_atexit(const void* fn) { joan::crt::atexit(fn); }

void JOAN_CDECL init_566480() { text_desc_clear(d::g_font_goudy_bold_gold_24); }
void JOAN_CDECL init_566870() { text_desc_clear(d::g_fontNum); }
void JOAN_CDECL init_566a20() { text_desc_clear(d::g_font_goudy_italic_12); }
void JOAN_CDECL init_5670c0() { joan::orig::as_sounds(obj(d::g_sfx_player))->zero(); }  // 0x004ceab0
void JOAN_CDECL init_5677e0() { crt_atexit(fn::code_ptr(fn::obj_stub_567810)); }
void JOAN_CDECL init_567bb0() { rect9_clear(d::g_bmp_res_menu_b); }
void JOAN_CDECL init_567f10() { rect9_clear(d::g_bmp_res_iattcomb); }
void JOAN_CDECL init_568390() { rect9_clear(d::g_bmp_res_quit); }
void JOAN_CDECL init_568780() { rect9_clear(d::g_bmp_res_victory3); }
void JOAN_CDECL init_568930() { rect9_clear(d::g_sheet); }
void JOAN_CDECL init_568c90() { rect9_clear(d::g_bmp_res_igamemnu); }
void JOAN_CDECL init_568ff0() { rect9_clear(d::g_bmp_res_gameover); }
void JOAN_CDECL init_5693e0() { rect9_clear(d::g_bmp_res_m3); }
void JOAN_CDECL init_569860() { rect9_clear(d::g_sprites_a460d8); }
void JOAN_CDECL init_569bc0() { joan::orig::as_bindings(obj(d::g_keys_a48820))->construct(); }  // 0x004cae10
void JOAN_CDECL init_569f20() { joan::orig::as_unit_types(obj(d::g_types))->construct(); }  // 0x00524c50
void JOAN_CDECL init_56a180() { crt_atexit(fn::code_ptr(fn::obj_stub_56a1b0)); }
void JOAN_CDECL init_56a4e0() { crt_atexit(fn::code_ptr(fn::obj_stub_56a510)); }
void JOAN_CDECL init_56a8d0() { crt_atexit(fn::code_ptr(fn::obj_stub_56a900)); }
void JOAN_CDECL init_56af50() { crt_atexit(fn::code_ptr(fn::dtor_stub_56af80)); }
void JOAN_CDECL init_56b1a0() { crt_atexit(fn::code_ptr(fn::dtor_stub_56b1d0)); }
void JOAN_CDECL init_56bcd0() { crt_atexit(fn::code_ptr(fn::dtor_stub_56be10)); }
void JOAN_CDECL init_56c020() { joan::orig::as_quit_popup(obj(d::g_quit_popup))->construct(); }  // 0x006e4600
void JOAN_CDECL init_56c680() { crt_atexit(fn::code_ptr(fn::dtor_stub_56c6b0)); }
void JOAN_CDECL init_56c980() { joan::orig::as_obj_menu(obj(d::g_objective_menu))->zero(); }  // 0x0071d290

struct H {
    // 0x0056dd30
    H* set_first(std::uintptr_t v) {  // the facade passes the callback's address (a function pointer: pointer-sized)
        jo::self<jo::ErrorLog>(this)->callback = reinterpret_cast<void*>(v);
        return this;
    }

    // 0x0056e110
    u32 call_75f5a0() { return joan::orig::arg::as<u32>(joan::orig::as_start_objectives_screen(this)->reset()); }  // 0x0075f5a0

    // 0x0056e650
    u32 call_4af4f0() { joan::orig::as_message_indicator(this)->release_texture(); return 0; }  // 0x004af4f0 (atexit stub: result ignored)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_06", name, addr, fn)
B("init_566480", 0x00566480, &init_566480);
B("init_566870", 0x00566870, &init_566870);
B("init_566a20", 0x00566a20, &init_566a20);
B("init_5670c0", 0x005670c0, &init_5670c0);
B("init_5677e0", 0x005677e0, &init_5677e0);
B("init_567bb0", 0x00567bb0, &init_567bb0);
B("init_567f10", 0x00567f10, &init_567f10);
B("init_568390", 0x00568390, &init_568390);
B("init_568780", 0x00568780, &init_568780);
B("init_568930", 0x00568930, &init_568930);
B("init_568c90", 0x00568c90, &init_568c90);
B("init_568ff0", 0x00568ff0, &init_568ff0);
B("init_5693e0", 0x005693e0, &init_5693e0);
B("init_569860", 0x00569860, &init_569860);
B("init_569bc0", 0x00569bc0, &init_569bc0);
B("init_569f20", 0x00569f20, &init_569f20);
B("init_56a180", 0x0056a180, &init_56a180);
B("init_56a4e0", 0x0056a4e0, &init_56a4e0);
B("init_56a8d0", 0x0056a8d0, &init_56a8d0);
B("init_56af50", 0x0056af50, &init_56af50);
B("init_56b1a0", 0x0056b1a0, &init_56b1a0);
B("init_56bcd0", 0x0056bcd0, &init_56bcd0);
B("init_56c020", 0x0056c020, &init_56c020);
B("init_56c680", 0x0056c680, &init_56c680);
B("init_56c980", 0x0056c980, &init_56c980);
B("Obj::set_first", 0x0056dd30, &H::set_first);
B("Obj::call_75f5a0", 0x0056e110, &H::call_75f5a0);
B("Obj::call_4af4f0", 0x0056e650, &H::call_4af4f0);
#undef B

}  // namespace game_b99_06
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
namespace {
namespace game_b97_05 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- static initialisers ----
using joan::orig::helpers::text_desc_clear;  // 0x00623dc0
using joan::orig::helpers::rect9_clear;      // 0x00620a20
void crt_atexit(const void* fn) { joan::crt::atexit(fn); }

void JOAN_CDECL init_5666c0() { text_desc_clear(d::g_font_goudy_20); }
void JOAN_CDECL init_566ab0() { text_desc_clear(d::g_font_dauphin_14_bold); }
void JOAN_CDECL init_567160() { joan::orig::as_video(obj(d::g_video_file))->construct(); }  // 0x006845d0
void JOAN_CDECL init_567a90() { rect9_clear(d::g_bmp_res_tui); }
void JOAN_CDECL init_567d60() { rect9_clear(d::g_bmp_res_criti_k); }
void JOAN_CDECL init_568300() { rect9_clear(d::g_bmp_res_qsave); }
void JOAN_CDECL init_5686f0() { rect9_clear(d::g_bmp_res_victory2); }
void JOAN_CDECL init_568ae0() { rect9_clear(d::g_bmp_res_optmenu); }
void JOAN_CDECL init_568ed0() { rect9_clear(d::g_a527b8); }
void JOAN_CDECL init_569350() { rect9_clear(d::g_bmp_res_m2); }
void JOAN_CDECL init_5696b0() { rect9_clear(d::g_bmp_res_m8); }
void JOAN_CDECL init_569b30() { rect9_clear(d::g_bmp_credits); }
void JOAN_CDECL init_56a2a0() { crt_atexit(fn::code_ptr(fn::obj_stub_56a2d0)); }
void JOAN_CDECL init_56a690() { crt_atexit(fn::code_ptr(fn::obj_stub_56a6c0)); }
void JOAN_CDECL init_56aa80() { crt_atexit(fn::code_ptr(fn::obj_stub_56aab0)); }
void JOAN_CDECL init_56b080() { crt_atexit(fn::code_ptr(fn::dtor_stub_56b0b0)); }
void JOAN_CDECL init_56bab0() { crt_atexit(fn::code_ptr(fn::dtor_stub_56bae0)); }
void JOAN_CDECL init_56c580() { joan::orig::as_talk_prompt(obj(d::g_talk_prompt))->construct(); }  // 0x006d2300
void JOAN_CDECL init_56cae0() { crt_atexit(fn::code_ptr(fn::dtor_stub_56cb10)); }
void JOAN_CDECL init_56d8b0() { joan::orig::as_player(obj(d::g_player_unit_recno))->construct(); }  // 0x0042d810

struct H {
    // 0x0056e140
    std::uint32_t call_755860() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_next_objectives_screen(this)->reset()); }  // 0x00755860
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("init_5666c0", 0x005666c0, &init_5666c0);
B("init_566ab0", 0x00566ab0, &init_566ab0);
B("init_567160", 0x00567160, &init_567160);
B("init_567a90", 0x00567a90, &init_567a90);
B("init_567d60", 0x00567d60, &init_567d60);
B("init_568300", 0x00568300, &init_568300);
B("init_5686f0", 0x005686f0, &init_5686f0);
B("init_568ae0", 0x00568ae0, &init_568ae0);
B("init_568ed0", 0x00568ed0, &init_568ed0);
B("init_569350", 0x00569350, &init_569350);
B("init_5696b0", 0x005696b0, &init_5696b0);
B("init_569b30", 0x00569b30, &init_569b30);
B("init_56a2a0", 0x0056a2a0, &init_56a2a0);
B("init_56a690", 0x0056a690, &init_56a690);
B("init_56aa80", 0x0056aa80, &init_56aa80);
B("init_56b080", 0x0056b080, &init_56b080);
B("init_56bab0", 0x0056bab0, &init_56bab0);
B("init_56c580", 0x0056c580, &init_56c580);
B("init_56cae0", 0x0056cae0, &init_56cae0);
B("init_56d8b0", 0x0056d8b0, &init_56d8b0);
B("Obj::call_755860", 0x0056e140, &H::call_755860);
#undef B

}  // namespace game_b97_05
}  // namespace

// ---------------------------------------- from modules/game_b105_10.cpp (game/b105_10)
namespace {
namespace game_b105_10 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// atexit(fn): eax = atexit result.
using joan::orig::helpers::reg_atexit;
// Stub: mov ecx, global; call thunk (thiscall, no args); eax kept.
template <auto G, fn::Code Thunk>
u32 JOAN_CDECL obj_stub() { return thiscall<u32>(Thunk, obj(G)); }

// 0x00566fb0: mov ecx, 0xa58b00; push 0xa50ec8; call 0x004036a7 (-> 0x00457ad0).
std::uintptr_t JOAN_CDECL init_566fb0() {
    return joan::orig::arg::as<u32>(joan::orig::as_sound_effects(obj(d::g_sound_effects))->construct(joan::orig::arg::mptr(obj(d::g_audio))));  // 0x00457ad0
}

struct H {
    // 0x00566d40: base ctor 0x0040956b (-> 0x005a2000) on this; returns this.
    void* ctor_566d40() {
        joan::orig::as_txt_res(this)->construct_5a2000();  // 0x005a2000
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_10", name, addr, fn)
B("init_atexit_5666f0", 0x005666f0, &reg_atexit<fn::obj_stub_566710>);
B("obj_stub_566710", 0x00566710, (&obj_stub<d::g_font_goudy_20, fn::Font_dtor>));  // 0x00623e70
B("init_atexit_566780", 0x00566780, &reg_atexit<fn::obj_stub_5667a0>);
B("obj_stub_5667a0", 0x005667a0, (&obj_stub<d::g_font_goudy_bold_14, fn::Font_dtor>));  // 0x00623e70
B("init_atexit_566810", 0x00566810, &reg_atexit<fn::obj_stub_566830>);
B("obj_stub_566830", 0x00566830, (&obj_stub<d::g_a4d4d8, fn::Font_dtor>));  // 0x00623e70
B("init_atexit_5668a0", 0x005668a0, &reg_atexit<fn::obj_stub_5668c0>);
B("obj_stub_5668c0", 0x005668c0, (&obj_stub<d::g_fontNum, fn::Font_dtor>));  // 0x00623e70
B("init_atexit_566930", 0x00566930, &reg_atexit<fn::obj_stub_566950>);
B("obj_stub_566950", 0x00566950, (&obj_stub<d::g_font_goudy_bold_italic_24, fn::Font_dtor>));  // 0x00623e70
B("init_atexit_5669c0", 0x005669c0, &reg_atexit<fn::obj_stub_5669e0>);
B("obj_stub_5669e0", 0x005669e0, (&obj_stub<d::g_font_goudy_12, fn::Font_dtor>));  // 0x00623e70
B("init_atexit_566a50", 0x00566a50, &reg_atexit<fn::obj_stub_566a70>);
B("obj_stub_566a70", 0x00566a70, (&obj_stub<d::g_font_goudy_italic_12, fn::Font_dtor>));  // 0x00623e70
B("init_atexit_566ae0", 0x00566ae0, &reg_atexit<fn::obj_stub_566b00>);
B("obj_stub_566b00", 0x00566b00, (&obj_stub<d::g_font_dauphin_14_bold, fn::Font_dtor>));  // 0x00623e70
B("init_atexit_566b70", 0x00566b70, &reg_atexit<fn::obj_stub_566b90>);
B("obj_stub_566b90", 0x00566b90, (&obj_stub<d::g_font_times_10, fn::Font_dtor>));  // 0x00623e70
B("init_atexit_566c00", 0x00566c00, &reg_atexit<fn::init_0x00566c20>);
B("init_atexit_566cd0", 0x00566cd0, &reg_atexit<fn::init_566cf0>);
B("Obj566d40::ctor", 0x00566d40, &H::ctor_566d40);
B("init_atexit_566db0", 0x00566db0, &reg_atexit<fn::fn_566dd0>);
B("obj_stub_566e30", 0x00566e30, (&obj_stub<d::g_music_res, fn::Obj_clear_flag8>));  // 0x004b3280
B("init_atexit_566ea0", 0x00566ea0, &reg_atexit<fn::fn_566ec0>);
B("obj_stub_566f20", 0x00566f20, (&obj_stub<d::g_mus, fn::Obj_clear_50>));  // 0x00585f70
B("init_atexit_566f40", 0x00566f40, &reg_atexit<fn::fn_566f60>);
B("init_566fb0", 0x00566fb0, &init_566fb0);
B("init_atexit_566fd0", 0x00566fd0, &reg_atexit<fn::obj_stub_566ff0>);
B("obj_stub_566ff0", 0x00566ff0, (&obj_stub<d::g_sound_effects, fn::Archive_holder_dtor>));  // 0x0059dfa0
B("init_atexit_567060", 0x00567060, &reg_atexit<fn::atexit_567080>);
B("init_atexit_5670f0", 0x005670f0, &reg_atexit<fn::init_567110>);
B("init_atexit_567190", 0x00567190, &reg_atexit<fn::obj_stub_5671b0>);
B("obj_stub_5671b0", 0x005671b0, (&obj_stub<d::g_video_file, fn::Object_shutdown_684620>));  // 0x00684620
B("init_atexit_567220", 0x00567220, &reg_atexit<fn::StaticInit_dtor_567240>);
B("init_atexit_5672b0", 0x005672b0, &reg_atexit<fn::atexit_5672d0>);
B("init_atexit_567340", 0x00567340, &reg_atexit<fn::obj_stub_567360>);
B("obj_stub_567360", 0x00567360, (&obj_stub<d::g_steer, fn::IniFile_dtor>));  // 0x0047c3a0
B("init_atexit_5673d0", 0x005673d0, &reg_atexit<fn::obj_stub_5673f0>);
B("obj_stub_5673f0", 0x005673f0, (&obj_stub<d::g_bone_name_res, fn::BoneNameRes_dtor>));  // 0x005ba500
B("init_atexit_567460", 0x00567460, &reg_atexit<fn::fn_567480>);
B("obj_stub_5674f0", 0x005674f0, (&obj_stub<d::g_model_refs, fn::Obj_clear7>));  // 0x00418da0
B("init_atexit_567510", 0x00567510, &reg_atexit<fn::f_567530>);
B("obj_stub_567580", 0x00567580, (&obj_stub<d::g_model_res_mgr, fn::ObjL_ctor>));  // 0x004b0980
B("init_atexit_5675a0", 0x005675a0, &reg_atexit<fn::init_5675c0>);
B("init_atexit_5676c0", 0x005676c0, &reg_atexit<fn::obj_stub_5676e0>);
B("obj_stub_5676e0", 0x005676e0, (&obj_stub<d::g_pemi_res, fn::Obj_forward_4c19e0>));  // 0x004c1970
B("obj_stub_567730", 0x00567730, (&obj_stub<d::g_texture_effect_res, fn::Struct7_zero_4de9f0>));  // 0x004de9f0
B("init_atexit_567750", 0x00567750, &reg_atexit<fn::obj_stub_567770>);
B("obj_stub_567770", 0x00567770, (&obj_stub<d::g_texture_effect_res, fn::Obj_forward_4dea50>));  // 0x004dea20
B("obj_stub_5677c0", 0x005677c0, (&obj_stub<d::g_bmp_res_lflare, fn::Rect9_clear>));  // 0x00620a20
B("obj_stub_567810", 0x00567810, (&obj_stub<d::g_bmp_res_lflare, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_567880", 0x00567880, &reg_atexit<fn::obj_stub_5678a0>);
B("obj_stub_5678a0", 0x005678a0, (&obj_stub<d::g_bmp_res_popupwin, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_567910", 0x00567910, &reg_atexit<fn::obj_stub_567930>);
B("obj_stub_567930", 0x00567930, (&obj_stub<d::g_bmp_res_maingame, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_5679a0", 0x005679a0, &reg_atexit<fn::obj_stub_5679c0>);
B("obj_stub_5679c0", 0x005679c0, (&obj_stub<d::g_bmp_res_mgexp, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_567a30", 0x00567a30, &reg_atexit<fn::obj_stub_567a50>);
B("obj_stub_567a50", 0x00567a50, (&obj_stub<d::g_bmp_res_hurt, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_567ac0", 0x00567ac0, &reg_atexit<fn::obj_stub_567ae0>);
B("obj_stub_567ae0", 0x00567ae0, (&obj_stub<d::g_bmp_res_tui, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
#undef B

}  // namespace game_b105_10
}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
namespace {
namespace game_b98_05 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- static initialisers ----
using joan::orig::helpers::text_desc_clear;  // 0x00623dc0
using joan::orig::helpers::rect9_clear;      // 0x00620a20
void crt_atexit(const void* fn) { joan::crt::atexit(fn); }

void JOAN_CDECL init_566900() { text_desc_clear(d::g_font_goudy_bold_italic_24); }
void JOAN_CDECL init_566e70() { joan::orig::as_sound_stream(obj(d::g_snd))->construct(); }  // 0x0059acd0
void JOAN_CDECL init_567850() { rect9_clear(d::g_bmp_res_popupwin); }
void JOAN_CDECL init_567c40() { rect9_clear(d::g_bmp_res_critical); }
void JOAN_CDECL init_568030() { rect9_clear(d::g_icons); }
void JOAN_CDECL init_568420() { rect9_clear(d::g_bmp_ending); }
void JOAN_CDECL init_5688a0() { rect9_clear(d::g_bmp_res_gmsg); }
void JOAN_CDECL init_568d20() { rect9_clear(d::g_bmp_objmenu); }
void JOAN_CDECL init_569080() { rect9_clear(d::g_bmp_res_rtsicon); }
void JOAN_CDECL init_569500() { rect9_clear(d::g_bmp_res_m5); }
void JOAN_CDECL init_569a10() { rect9_clear(d::g_set); }
void JOAN_CDECL init_569e90() { joan::orig::as_nation_res(obj(d::g_nation_res))->construct(); }  // 0x004b8f20
void JOAN_CDECL init_56a330() { crt_atexit(fn::code_ptr(fn::obj_stub_56a360)); }
void JOAN_CDECL init_56a600() { crt_atexit(fn::code_ptr(fn::obj_stub_56a630)); }
void JOAN_CDECL init_56ab10() { crt_atexit(fn::code_ptr(fn::obj_stub_56ab40)); }
void JOAN_CDECL init_56b110() { crt_atexit(fn::code_ptr(fn::dtor_stub_56b140)); }
void JOAN_CDECL init_56bbc0() { joan::orig::as_start_objectives_screen(obj(d::g_start_objectives_screen))->zero(); }  // 0x0075f550
void JOAN_CDECL init_56c0e0() { crt_atexit(fn::code_ptr(fn::dtor_stub_56c110)); }
void JOAN_CDECL init_56cd30() { crt_atexit(fn::code_ptr(fn::StaticInit_init_56cd60)); }
void JOAN_CDECL init_56d630() { crt_atexit(fn::code_ptr(fn::atexit_56d870)); }

struct H {
    // 0x0056e2c0
    void call_6d2380() { joan::orig::as_talk_prompt(this)->release(); }  // 0x006d2380
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("init_566900", 0x00566900, &init_566900);
B("init_566e70", 0x00566e70, &init_566e70);
B("init_567850", 0x00567850, &init_567850);
B("init_567c40", 0x00567c40, &init_567c40);
B("init_568030", 0x00568030, &init_568030);
B("init_568420", 0x00568420, &init_568420);
B("init_5688a0", 0x005688a0, &init_5688a0);
B("init_568d20", 0x00568d20, &init_568d20);
B("init_569080", 0x00569080, &init_569080);
B("init_569500", 0x00569500, &init_569500);
B("init_569a10", 0x00569a10, &init_569a10);
B("init_569e90", 0x00569e90, &init_569e90);
B("init_56a330", 0x0056a330, &init_56a330);
B("init_56a600", 0x0056a600, &init_56a600);
B("init_56ab10", 0x0056ab10, &init_56ab10);
B("init_56b110", 0x0056b110, &init_56b110);
B("init_56bbc0", 0x0056bbc0, &init_56bbc0);
B("init_56c0e0", 0x0056c0e0, &init_56c0e0);
B("init_56cd30", 0x0056cd30, &init_56cd30);
B("init_56d630", 0x0056d630, &init_56d630);
B("Obj::call_6d2380", 0x0056e2c0, &H::call_6d2380);
#undef B

}  // namespace game_b98_05
}  // namespace

// ---------------------------------------- from modules/game_b100_09.cpp (game/b100_09)
namespace {
namespace game_b100_09 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

void crt_atexit(const void* fn) { joan::crt::atexit(fn); }

void JOAN_CDECL init_566990() { joan::orig::as_text_desc(obj(d::g_font_goudy_12))->clear(); }  // 0x00623dc0
void JOAN_CDECL init_566d80() { joan::orig::as_audio(obj(d::g_audio))->construct(); }  // 0x0057a340
void JOAN_CDECL init_5673a0() { joan::orig::as_bone_name_table(obj(d::g_bone_name_res))->construct(); }  // 0x005ba4b0
void JOAN_CDECL init_567430() { joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->construct(); }  // 0x00416f60
void JOAN_CDECL init_567630() { joan::orig::as_pemi_res(obj(d::g_pemi_res))->reset(); }  // 0x004c1940
void JOAN_CDECL init_5678e0() { joan::orig::as_rect9(obj(d::g_bmp_res_maingame))->clear(); }  // 0x00620a20
void JOAN_CDECL init_567cd0() { joan::orig::as_rect9(obj(d::g_bmp_res_criti_f))->clear(); }  // 0x00620a20
void JOAN_CDECL init_567fa0() { joan::orig::as_rect9(obj(d::g_bmp_res_milibal))->clear(); }  // 0x00620a20
void JOAN_CDECL init_568150() { joan::orig::as_rect9(obj(d::g_heroIcons))->clear(); }  // 0x00620a20
void JOAN_CDECL init_5684b0() { joan::orig::as_rect9(obj(d::g_bmp_res_letter))->clear(); }  // 0x00620a20
void JOAN_CDECL init_568810() { joan::orig::as_rect9(obj(d::g_bmp_res_victorye))->clear(); }  // 0x00620a20
void JOAN_CDECL init_568b70() { joan::orig::as_rect9(obj(d::g_bmp_res_optmenuf))->clear(); }  // 0x00620a20
void JOAN_CDECL init_568f60() { joan::orig::as_rect9(obj(d::g_bmp_res_savemenu))->clear(); }  // 0x00620a20
void JOAN_CDECL init_569230() { joan::orig::as_rect9(obj(d::g_bmp_res_sl_com))->clear(); }  // 0x00620a20
void JOAN_CDECL init_569470() { joan::orig::as_rect9(obj(d::g_bmp_res_m4))->clear(); }  // 0x00620a20
void JOAN_CDECL init_5697d0() { joan::orig::as_rect9(obj(d::g_bmp_res_helpbox))->clear(); }  // 0x00620a20
void JOAN_CDECL init_5698f0() { joan::orig::as_rect9(obj(d::g_bmp_res_armory))->clear(); }  // 0x00620a20
void JOAN_CDECL init_569c50() { joan::orig::as_sdate_res(obj(d::g_sdate_res))->construct(); }  // 0x004cdfd0
void JOAN_CDECL init_569d70() { joan::orig::as_triggers(obj(d::g_a5ccd0))->construct(); }  // 0x004eed50
void JOAN_CDECL init_569e00() { joan::orig::as_race_res(obj(d::g_race_res))->construct(); }  // 0x004ca2e0
void JOAN_CDECL init_569fb0() { joan::orig::as_script_cache(obj(d::g_scripts))->construct(); }  // 0x00648aa0
void JOAN_CDECL init_56a450() { crt_atexit(fn::code_ptr(fn::obj_stub_56a480)); }
void JOAN_CDECL init_56a720() { crt_atexit(fn::code_ptr(fn::obj_stub_56a750)); }
void JOAN_CDECL init_56a9f0() { crt_atexit(fn::code_ptr(fn::obj_stub_56aa20)); }
void JOAN_CDECL init_56ac30() { crt_atexit(fn::code_ptr(fn::obj_stub_56ac60)); }
void JOAN_CDECL init_56afc0() { joan::orig::as_grass(obj(d::g_dyn_grass))->construct(); }  // 0x00464130
void JOAN_CDECL init_56ba20() { crt_atexit(fn::code_ptr(fn::dtor_stub_56ba50)); }
void JOAN_CDECL init_56be50() { joan::orig::as_shop(obj(d::g_buy))->construct(); }  // 0x006c2070
void JOAN_CDECL init_56c150() { joan::orig::as_sms(obj(d::g_sms))->construct(); }  // 0x006e4e20
void JOAN_CDECL init_56c800() { joan::orig::as_fade_title(obj(d::g_fade_title))->zero(); }  // 0x007612a0
void JOAN_CDECL init_56c910() { crt_atexit(fn::code_ptr(fn::StaticDtor_a4fef8)); }
void JOAN_CDECL init_56cda0() { joan::orig::as_msg_log(obj(d::g_portraits))->ctor(); }  // 0x004b2a90
void JOAN_CDECL init_56ce60() { crt_atexit(fn::code_ptr(fn::init_56ce90)); }

#define B(name, addr, fn) JOAN_REPLACE("game/b100_09", name, addr, fn)
B("init_566990", 0x00566990, &init_566990);
B("init_566d80", 0x00566d80, &init_566d80);
B("init_5673a0", 0x005673a0, &init_5673a0);
B("init_567430", 0x00567430, &init_567430);
B("init_567630", 0x00567630, &init_567630);
B("init_5678e0", 0x005678e0, &init_5678e0);
B("init_567cd0", 0x00567cd0, &init_567cd0);
B("init_567fa0", 0x00567fa0, &init_567fa0);
B("init_568150", 0x00568150, &init_568150);
B("init_5684b0", 0x005684b0, &init_5684b0);
B("init_568810", 0x00568810, &init_568810);
B("init_568b70", 0x00568b70, &init_568b70);
B("init_568f60", 0x00568f60, &init_568f60);
B("init_569230", 0x00569230, &init_569230);
B("init_569470", 0x00569470, &init_569470);
B("init_5697d0", 0x005697d0, &init_5697d0);
B("init_5698f0", 0x005698f0, &init_5698f0);
B("init_569c50", 0x00569c50, &init_569c50);
B("init_569d70", 0x00569d70, &init_569d70);
B("init_569e00", 0x00569e00, &init_569e00);
B("init_569fb0", 0x00569fb0, &init_569fb0);
B("init_56a450", 0x0056a450, &init_56a450);
B("init_56a720", 0x0056a720, &init_56a720);
B("init_56a9f0", 0x0056a9f0, &init_56a9f0);
B("init_56ac30", 0x0056ac30, &init_56ac30);
B("init_56afc0", 0x0056afc0, &init_56afc0);
B("init_56ba20", 0x0056ba20, &init_56ba20);
B("init_56be50", 0x0056be50, &init_56be50);
B("init_56c150", 0x0056c150, &init_56c150);
B("init_56c800", 0x0056c800, &init_56c800);
B("init_56c910", 0x0056c910, &init_56c910);
B("init_56cda0", 0x0056cda0, &init_56cda0);
B("init_56ce60", 0x0056ce60, &init_56ce60);
#undef B

}  // namespace game_b100_09
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- 0x0056dad0: _initterm entry ---------------------------------------------------------
u32 JOAN_CDECL init_56dad0() {
    joan::orig::init_56daf0();
    return joan::orig::arg::as<u32>(joan::orig::init_56db10());
}
// ---- 0x00566c20 (dead) -------------------------------------------------------------------
u32 JOAN_CDECL init_566c20() { return joan::orig::arg::as<u32>(joan::orig::as_txt_res(obj(d::g_txt))->destruct_566c40()); }  // 0x00566c40

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("init_0x00566c20", 0x00566c20, &init_566c20);
B("init_0x0056dad0", 0x0056dad0, &init_56dad0);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
namespace {
namespace game_b108_05 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x00566c40
    u32 forward_566c40() { return joan::orig::arg::as<u32>(joan::orig::as_txt_res(this)->destruct()); }  // 0x005a2040
    // ------------------------------------------------------------------ 0x0056e380
    u32 forward_56e380() { return joan::orig::arg::as<u32>(joan::orig::as_opt_menu(this)->release_all_73c410()); }  // 0x0073c410
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("Obj::forward_566c40", 0x00566c40, &H::forward_566c40);
B("Obj::forward_56e380", 0x0056e380, &H::forward_56e380);
#undef B

}  // namespace game_b108_05
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ------------------------------------------------------------------ cdecl static initialisers
static u32 JOAN_CDECL init_566c90() {
    joan::orig::init_566cb0();
    return cdecl_call<u32>(fn::init_atexit_566cd0);
}

JOAN_REPLACE("game/b108_07", "init_566c90", 0x00566c90, &init_566c90);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

// ---------------------------------------------------------------- cdecl functions
// 0x00566cf0
u32 JOAN_CDECL init_566cf0() { return joan::orig::arg::as<u32>(joan::orig::as_txt_res(obj(d::g_strings))->destruct_566d20()); }  // 0x00566d20
// 0x00567110
u32 JOAN_CDECL init_567110() { return joan::orig::arg::as<u32>(joan::orig::as_sounds(obj(d::g_sfx_player))->destruct()); }  // 0x004ceaf0

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("init_566cf0", 0x00566cf0, &init_566cf0);
B("init_567110", 0x00567110, &init_567110);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // -------------------------------------------------------------- 0x00566d20
    u32 wrap_566d20() { return joan::orig::arg::as<u32>(joan::orig::as_txt_res(this)->destruct()); }  // 0x005a2040
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("Obj::wrap_566d20", 0x00566d20, &H::wrap_566d20);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

// ------------------------------------------------------------------ 0x00567080
static u32 JOAN_CDECL atexit_567080() {
    return joan::orig::arg::as<u32>(joan::orig::as_sound3dsystem(obj(d::g_sound3d_system))->destruct());  // 0x005a0300
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("atexit_567080", 0x00567080, &atexit_567080);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

// 0x00567240: mov ecx, 0xa4d598; call 0x0040270c (-> 0x0063f250); eax kept.
u32 JOAN_CDECL dtor_567240() { return joan::orig::arg::as<u32>(joan::orig::as_script(obj(d::g_a4d598))->dtor_63f250()); }

struct H {
    // 0x005675e0
    u32 forward_5675e0() { return joan::orig::arg::as<u32>(joan::orig::as_string_pool(reinterpret_cast<u8*>(this) + 0xd)->destruct()); }  // 0x005a1770
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("StaticInit::dtor_567240", 0x00567240, &dtor_567240);
B("Obj::forward_5675e0", 0x005675e0, &H::forward_5675e0);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

// ------------------------------------------------------------------ atexit callbacks
static u32 JOAN_CDECL atexit_5672d0() { return joan::orig::arg::as<u32>(joan::orig::as_dialogue(obj(d::g_dialogue))->destruct()); }  // 0x0063bf90
static u32 JOAN_CDECL atexit_56bea0() { return joan::orig::arg::as<u32>(joan::orig::as_shop(obj(d::g_buy))->destruct()); }  // 0x00423d20

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("atexit_5672d0", 0x005672d0, &atexit_5672d0);
B("atexit_56bea0", 0x0056bea0, &atexit_56bea0);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ------------------------------------------------------------------ cdecl / free functions
static u32 JOAN_CDECL f_567530() {
    return joan::orig::arg::as<u32>(joan::orig::as_model_refs(obj(d::g_model_refs))->release_thunk());  // 0x00418dd0
}
static u32 JOAN_CDECL f_56d550() {
    joan::orig::dtor_stub_56d580();
    return cdecl_call<u32>(fn::StaticInit_atexit_56d5c0);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("f_567530", 0x00567530, &f_567530);
B("init_0x0056d550", 0x0056d550, &f_56d550);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

// ------------------------------------------------------------------ static initialisers
u32 JOAN_CDECL init_5675c0() { return joan::orig::arg::as<u32>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->destruct()); }  // 0x005675e0
u32 JOAN_CDECL init_56dbc0() { return joan::orig::arg::as<u32>(joan::orig::as_game(obj(d::g_game_a50088))->destruct()); }  // 0x004d6be0

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("init_5675c0", 0x005675c0, &init_5675c0);
B("init_56dbc0", 0x0056dbc0, &init_56dbc0);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b105_11.cpp (game/b105_11)
namespace {
namespace game_b105_11 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// atexit(fn): eax = atexit result.
using joan::orig::helpers::reg_atexit;
// Destructor stub: mov ecx, global; call 0x00402bf8 (-> 0x0054b460); eax kept.
template <auto G>
u32 JOAN_CDECL dtor_stub() { return joan::orig::arg::as<u32>(joan::orig::as_bmp_res(obj(G))->free_thunk()); }  // 0x0054b460

#define B(name, addr, fn) JOAN_REPLACE("game/b105_11", name, addr, fn)
B("init_atexit_567b50", 0x00567b50, &reg_atexit<fn::dtor_stub_567b70>);
B("dtor_stub_567b70", 0x00567b70, &dtor_stub<d::g_bmp_res_menu1024>);  // global 0xa50eb8
B("init_atexit_567be0", 0x00567be0, &reg_atexit<fn::dtor_stub_567c00>);
B("dtor_stub_567c00", 0x00567c00, &dtor_stub<d::g_bmp_res_menu_b>);  // global 0xa45f88
B("init_atexit_567c70", 0x00567c70, &reg_atexit<fn::dtor_stub_567c90>);
B("dtor_stub_567c90", 0x00567c90, &dtor_stub<d::g_bmp_res_critical>);  // global 0xa48f40
B("init_atexit_567d00", 0x00567d00, &reg_atexit<fn::dtor_stub_567d20>);
B("dtor_stub_567d20", 0x00567d20, &dtor_stub<d::g_bmp_res_criti_f>);  // global 0xa50718
B("init_atexit_567d90", 0x00567d90, &reg_atexit<fn::dtor_stub_567db0>);
B("dtor_stub_567db0", 0x00567db0, &dtor_stub<d::g_bmp_res_criti_k>);  // global 0xa40390
B("init_atexit_567e20", 0x00567e20, &reg_atexit<fn::dtor_stub_567e40>);
B("dtor_stub_567e40", 0x00567e40, &dtor_stub<d::g_bmp_res_invent>);  // global 0xa4f810
B("init_atexit_567eb0", 0x00567eb0, &reg_atexit<fn::dtor_stub_567ed0>);
B("dtor_stub_567ed0", 0x00567ed0, &dtor_stub<d::g_bmp_res_iunitatt>);  // global 0xa57068
B("init_atexit_567f40", 0x00567f40, &reg_atexit<fn::dtor_stub_567f60>);
B("dtor_stub_567f60", 0x00567f60, &dtor_stub<d::g_bmp_res_iattcomb>);  // global 0xa45cf0
B("init_atexit_567fd0", 0x00567fd0, &reg_atexit<fn::dtor_stub_567ff0>);
B("dtor_stub_567ff0", 0x00567ff0, &dtor_stub<d::g_bmp_res_milibal>);  // global 0xa411c0
B("init_atexit_568060", 0x00568060, &reg_atexit<fn::dtor_stub_568080>);
B("dtor_stub_568080", 0x00568080, &dtor_stub<d::g_icons>);  // global 0xa51648
B("init_atexit_5680f0", 0x005680f0, &reg_atexit<fn::dtor_stub_568110>);
B("dtor_stub_568110", 0x00568110, &dtor_stub<d::g_bmp_res_msglog>);  // global 0xa5cd10
B("init_atexit_568180", 0x00568180, &reg_atexit<fn::dtor_stub_5681a0>);
B("dtor_stub_5681a0", 0x005681a0, &dtor_stub<d::g_heroIcons>);  // global 0xa5d180
B("init_atexit_568210", 0x00568210, &reg_atexit<fn::dtor_stub_568230>);
B("dtor_stub_568230", 0x00568230, &dtor_stub<d::g_bmp_res_buy>);  // global 0xa48810
B("init_atexit_5682a0", 0x005682a0, &reg_atexit<fn::dtor_stub_5682c0>);
B("dtor_stub_5682c0", 0x005682c0, &dtor_stub<d::g_bmp_res_kill>);  // global 0xa52de8
B("init_atexit_568330", 0x00568330, &reg_atexit<fn::dtor_stub_568350>);
B("dtor_stub_568350", 0x00568350, &dtor_stub<d::g_bmp_res_qsave>);  // global 0xa40460
B("init_atexit_5683c0", 0x005683c0, &reg_atexit<fn::dtor_stub_5683e0>);
B("dtor_stub_5683e0", 0x005683e0, &dtor_stub<d::g_bmp_res_quit>);  // global 0xa47c98
B("init_atexit_568450", 0x00568450, &reg_atexit<fn::dtor_stub_568470>);
B("dtor_stub_568470", 0x00568470, &dtor_stub<d::g_bmp_ending>);  // global 0xa45d00
B("init_atexit_5684e0", 0x005684e0, &reg_atexit<fn::dtor_stub_568500>);
B("dtor_stub_568500", 0x00568500, &dtor_stub<d::g_bmp_res_letter>);  // global 0xa527f8
B("init_atexit_568570", 0x00568570, &reg_atexit<fn::dtor_stub_568590>);
B("dtor_stub_568590", 0x00568590, &dtor_stub<d::g_a5cd60>);  // global 0xa5cd60
B("init_atexit_568600", 0x00568600, &reg_atexit<fn::dtor_stub_568620>);
B("dtor_stub_568620", 0x00568620, &dtor_stub<d::g_bmp_res_vict>);  // global 0xa3ce60
B("init_atexit_568690", 0x00568690, &reg_atexit<fn::dtor_stub_5686b0>);
B("dtor_stub_5686b0", 0x005686b0, &dtor_stub<d::g_bmp_res_victory>);  // global 0xa50738
B("init_atexit_568720", 0x00568720, &reg_atexit<fn::dtor_stub_568740>);
B("dtor_stub_568740", 0x00568740, &dtor_stub<d::g_bmp_res_victory2>);  // global 0xa47d58
B("init_atexit_5687b0", 0x005687b0, &reg_atexit<fn::dtor_stub_5687d0>);
B("dtor_stub_5687d0", 0x005687d0, &dtor_stub<d::g_bmp_res_victory3>);  // global 0xa58ca8
B("init_atexit_568840", 0x00568840, &reg_atexit<fn::dtor_stub_568860>);
B("dtor_stub_568860", 0x00568860, &dtor_stub<d::g_bmp_res_victorye>);  // global 0xa48848
B("init_atexit_5688d0", 0x005688d0, &reg_atexit<fn::dtor_stub_5688f0>);
B("dtor_stub_5688f0", 0x005688f0, &dtor_stub<d::g_bmp_res_gmsg>);  // global 0xa4d608
B("init_atexit_568960", 0x00568960, &reg_atexit<fn::dtor_stub_568980>);
B("dtor_stub_568980", 0x00568980, &dtor_stub<d::g_sheet>);  // global 0xa5cf88
B("init_atexit_5689f0", 0x005689f0, &reg_atexit<fn::dtor_stub_568a10>);
B("dtor_stub_568a10", 0x00568a10, &dtor_stub<d::g_bmp_commele>);  // global 0xa57f30
B("init_atexit_568a80", 0x00568a80, &reg_atexit<fn::dtor_stub_568aa0>);
B("dtor_stub_568aa0", 0x00568aa0, &dtor_stub<d::g_bmp_commele2>);  // global 0xa48f60
B("init_atexit_568b10", 0x00568b10, &reg_atexit<fn::dtor_stub_568b30>);
B("dtor_stub_568b30", 0x00568b30, &dtor_stub<d::g_bmp_res_optmenu>);  // global 0xa3ce70
B("init_atexit_568ba0", 0x00568ba0, &reg_atexit<fn::dtor_stub_568bc0>);
B("dtor_stub_568bc0", 0x00568bc0, &dtor_stub<d::g_bmp_res_optmenuf>);  // global 0xa50690
B("init_atexit_568c30", 0x00568c30, &reg_atexit<fn::obj_stub_568c50>);
#undef B

}  // namespace game_b105_11
}  // namespace

// ---------------------------------------- from modules/game_b105_12.cpp (game/b105_12)
namespace {
namespace game_b105_12 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using joan::orig::helpers::reg_atexit;
template <auto G, fn::Code Thunk>
u32 JOAN_CDECL obj_stub() { return thiscall<u32>(Thunk, obj(G)); }

#define B(name, addr, fn) JOAN_REPLACE("game/b105_12", name, addr, fn)
B("obj_stub_568c50", 0x00568c50, (&obj_stub<d::g_bmp_res_optmenub, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_568cc0", 0x00568cc0, &reg_atexit<fn::obj_stub_568ce0>);
B("obj_stub_568ce0", 0x00568ce0, (&obj_stub<d::g_bmp_res_igamemnu, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_568d50", 0x00568d50, &reg_atexit<fn::obj_stub_568d70>);
B("obj_stub_568d70", 0x00568d70, (&obj_stub<d::g_bmp_objmenu, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_568de0", 0x00568de0, &reg_atexit<fn::obj_stub_568e00>);
B("obj_stub_568e00", 0x00568e00, (&obj_stub<d::g_bmp_res_ld_bg, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_568e70", 0x00568e70, &reg_atexit<fn::obj_stub_568e90>);
B("obj_stub_568e90", 0x00568e90, (&obj_stub<d::g_bmp_res_ld_cp, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_568f00", 0x00568f00, &reg_atexit<fn::obj_stub_568f20>);
B("obj_stub_568f20", 0x00568f20, (&obj_stub<d::g_a527b8, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_568f90", 0x00568f90, &reg_atexit<fn::obj_stub_568fb0>);
B("obj_stub_568fb0", 0x00568fb0, (&obj_stub<d::g_bmp_res_savemenu, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569020", 0x00569020, &reg_atexit<fn::obj_stub_569040>);
B("obj_stub_569040", 0x00569040, (&obj_stub<d::g_bmp_res_gameover, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_5690b0", 0x005690b0, &reg_atexit<fn::obj_stub_5690d0>);
B("obj_stub_5690d0", 0x005690d0, (&obj_stub<d::g_bmp_res_rtsicon, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569140", 0x00569140, &reg_atexit<fn::obj_stub_569160>);
B("obj_stub_569160", 0x00569160, (&obj_stub<d::g_bmp_res_sl_bg1, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_5691d0", 0x005691d0, &reg_atexit<fn::obj_stub_5691f0>);
B("obj_stub_5691f0", 0x005691f0, (&obj_stub<d::g_bmp_res_sl_bg2, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569260", 0x00569260, &reg_atexit<fn::obj_stub_569280>);
B("obj_stub_569280", 0x00569280, (&obj_stub<d::g_bmp_res_sl_com, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_5692f0", 0x005692f0, &reg_atexit<fn::obj_stub_569310>);
B("obj_stub_569310", 0x00569310, (&obj_stub<d::g_bmp_res_m1, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569380", 0x00569380, &reg_atexit<fn::obj_stub_5693a0>);
B("obj_stub_5693a0", 0x005693a0, (&obj_stub<d::g_bmp_res_m2, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569410", 0x00569410, &reg_atexit<fn::obj_stub_569430>);
B("obj_stub_569430", 0x00569430, (&obj_stub<d::g_bmp_res_m3, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_5694a0", 0x005694a0, &reg_atexit<fn::obj_stub_5694c0>);
B("obj_stub_5694c0", 0x005694c0, (&obj_stub<d::g_bmp_res_m4, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569530", 0x00569530, &reg_atexit<fn::obj_stub_569550>);
B("obj_stub_569550", 0x00569550, (&obj_stub<d::g_bmp_res_m5, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_5695c0", 0x005695c0, &reg_atexit<fn::obj_stub_5695e0>);
B("obj_stub_5695e0", 0x005695e0, (&obj_stub<d::g_bmp_res_m6, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569650", 0x00569650, &reg_atexit<fn::obj_stub_569670>);
B("obj_stub_569670", 0x00569670, (&obj_stub<d::g_bmp_res_m7, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_5696e0", 0x005696e0, &reg_atexit<fn::obj_stub_569700>);
B("obj_stub_569700", 0x00569700, (&obj_stub<d::g_bmp_res_m8, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569770", 0x00569770, &reg_atexit<fn::obj_stub_569790>);
B("obj_stub_569790", 0x00569790, (&obj_stub<d::g_sprites_a528c8, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569800", 0x00569800, &reg_atexit<fn::obj_stub_569820>);
B("obj_stub_569820", 0x00569820, (&obj_stub<d::g_bmp_res_helpbox, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569890", 0x00569890, &reg_atexit<fn::obj_stub_5698b0>);
B("obj_stub_5698b0", 0x005698b0, (&obj_stub<d::g_sprites_a460d8, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569920", 0x00569920, &reg_atexit<fn::obj_stub_569940>);
B("obj_stub_569940", 0x00569940, (&obj_stub<d::g_bmp_res_armory, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_5699b0", 0x005699b0, &reg_atexit<fn::obj_stub_5699d0>);
B("obj_stub_5699d0", 0x005699d0, (&obj_stub<d::g_bmp_debrief, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569a40", 0x00569a40, &reg_atexit<fn::obj_stub_569a60>);
B("obj_stub_569a60", 0x00569a60, (&obj_stub<d::g_set, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569ad0", 0x00569ad0, &reg_atexit<fn::obj_stub_569af0>);
B("obj_stub_569af0", 0x00569af0, (&obj_stub<d::g_bmp_res_strategy, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569b60", 0x00569b60, &reg_atexit<fn::obj_stub_569b80>);
B("obj_stub_569b80", 0x00569b80, (&obj_stub<d::g_bmp_credits, fn::fn_0x0054b460_forward_620ab0>));  // 0x0054b460
B("init_atexit_569bf0", 0x00569bf0, &reg_atexit<fn::obj_stub_569c10>);
B("obj_stub_569c10", 0x00569c10, (&obj_stub<d::g_keys_a48820, fn::X_forward_4cb310>));  // 0x004cae40
B("init_atexit_569c80", 0x00569c80, &reg_atexit<fn::obj_stub_569ca0>);
B("obj_stub_569ca0", 0x00569ca0, (&obj_stub<d::g_sdate_res, fn::Obj_forward_4ce070>));  // 0x004ce000
B("init_atexit_569d10", 0x00569d10, &reg_atexit<fn::obj_stub_569d30>);
B("obj_stub_569d30", 0x00569d30, (&obj_stub<d::g_physic_sys, fn::PhysicSystem_dtor>));  // 0x006acf80
#undef B

}  // namespace game_b105_12
}  // namespace

// ---------------------------------------- from modules/game_b107_02.cpp (game/b107_02)
namespace {
namespace game_b107_02 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// Aggregate initialiser: first(); return second();  (both cdecl, no args)
template <fn::Code First, fn::Code Second>
std::int32_t JOAN_CDECL init_pair() {
    cdecl_call<std::int32_t>(First);
    return cdecl_call<std::int32_t>(Second);
}

// 0x0056ad80: atexit destructor of the global 0xa48f08.
std::uint32_t JOAN_CDECL atexit_56ad80() {
    return joan::orig::arg::as<std::uint32_t>(joan::orig::as_talk(obj(d::g_queue))->destruct());  // 0x0056ada0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_02", name, addr, fn)
B("init_568d90", 0x00568d90, (&init_pair<fn::init_568db0, fn::init_atexit_568de0>));
B("init_569840", 0x00569840, (&init_pair<fn::init_569860, fn::init_atexit_569890>));
B("init_56a2f0", 0x0056a2f0, (&init_pair<fn::obj_stub_56a310, fn::init_56a330>));
B("init_56c560", 0x0056c560, (&init_pair<fn::init_56c580, fn::init_atexit_56c5b0>));
B("atexit_56ad80", 0x0056ad80, &atexit_56ad80);
#undef B

}  // namespace game_b107_02
}  // namespace

// ---------------------------------------- from modules/game_b106_16.cpp (game/b106_16)
namespace {
namespace game_b106_16 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

// 0x00569ce0: static initialiser of the global at 0xa570a8.
std::uintptr_t JOAN_CDECL init_569ce0() { return joan::orig::arg::as<u32>(joan::orig::as_players(obj(d::g_physic_sys))->construct()); }  // 0x006aced0

#define B(name, addr, fn) JOAN_REPLACE("game/b106_16", name, addr, fn)
B("init_569ce0", 0x00569ce0, &init_569ce0);
#undef B

}  // namespace game_b106_16
}  // namespace

// ---------------------------------------- from modules/game_b105_13.cpp (game/b105_13)
namespace {
namespace game_b105_13 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using joan::orig::helpers::reg_atexit;
template <auto G, fn::Code Thunk>
u32 JOAN_CDECL obj_stub() { return thiscall<u32>(Thunk, obj(G)); }

u32 JOAN_CDECL init_569e70() {  // direct call in the original
    joan::orig::init_569e90();
    return 0;  // void callee; _initterm ignores the eax
}

struct H {
    // 0x0056ada0
    u32 forward_plus4() { return thiscall<u32>(fn::dtor_DynArrayB, &jo::self<jo::Talk>(this)->msgs); }  // 0x00585710
    // 0x0056ae50
    u32 forward() { joan::orig::as_dynarray(this)->destruct_5844b0(); return 0; }  // 0x005844b0 (atexit stub: result ignored)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_13", name, addr, fn)
B("init_atexit_569da0", 0x00569da0, &reg_atexit<fn::obj_stub_569dc0>);
B("obj_stub_569dc0", 0x00569dc0, (&obj_stub<d::g_a5ccd0, fn::X_dtor_4eed80>));  // 0x004eed80
B("init_atexit_569e30", 0x00569e30, &reg_atexit<fn::obj_stub_569e50>);
B("obj_stub_569e50", 0x00569e50, (&obj_stub<d::g_race_res, fn::Obj_fwd_56deb0>));  // 0x0056deb0
B("init_569e70", 0x00569e70, &init_569e70);
B("obj_stub_569ee0", 0x00569ee0, (&obj_stub<d::g_level_res, fn::Obj_clear_byte8>));  // 0x004a4bc0
B("init_atexit_569f50", 0x00569f50, &reg_atexit<fn::obj_stub_569f70>);
B("obj_stub_569f70", 0x00569f70, (&obj_stub<d::g_types, fn::fn_524c90>));  // 0x00524c90
B("init_atexit_569fe0", 0x00569fe0, &reg_atexit<fn::fn_56a000>);
B("obj_stub_56a050", 0x0056a050, (&obj_stub<d::g_uscr_res, fn::Struct9_zero_5460e0>));  // 0x005460e0
B("init_atexit_56a070", 0x0056a070, &reg_atexit<fn::obj_stub_56a090>);
B("obj_stub_56a090", 0x0056a090, (&obj_stub<d::g_uscr_res, fn::Obj_forward_546140>));  // 0x00546110
B("obj_stub_56a0d0", 0x0056a0d0, (&obj_stub<d::g_item_res, fn::Obj_ctor_49f570>));  // 0x0049f570
B("obj_stub_56a160", 0x0056a160, (&obj_stub<d::g_randList, fn::Obj_clear_9>));  // 0x00463920
B("obj_stub_56a1b0", 0x0056a1b0, (&obj_stub<d::g_randList, fn::Obj_fwd_463950>));  // 0x00463950
B("obj_stub_56a1f0", 0x0056a1f0, (&obj_stub<d::g_sell_item_res, fn::Struct9_zero_4ce2a0>));  // 0x004ce2a0
B("obj_stub_56a240", 0x0056a240, (&obj_stub<d::g_sell_item_res, fn::SellItemRes_dtor>));  // 0x004ce2d0
B("obj_stub_56a280", 0x0056a280, (&obj_stub<d::g_recruit_res, fn::Struct9_zero_4ca5e0>));  // 0x004ca5e0
B("obj_stub_56a2d0", 0x0056a2d0, (&obj_stub<d::g_recruit_res, fn::RecruitRes_dtor>));  // 0x004ca610
B("obj_stub_56a310", 0x0056a310, (&obj_stub<d::g_a50680, fn::Struct9_zero_487be0>));  // 0x00487be0
B("obj_stub_56a360", 0x0056a360, (&obj_stub<d::g_a50680, fn::Obj_call_487c10>));  // 0x00487c10
B("obj_stub_56a3a0", 0x0056a3a0, (&obj_stub<d::g_upgrade_res, fn::X_clear_10>));  // 0x005315f0
B("obj_stub_56a3f0", 0x0056a3f0, (&obj_stub<d::g_upgrade_res, fn::UpgraRes_dtor>));  // 0x00531620
B("obj_stub_56a430", 0x0056a430, (&obj_stub<d::g_sound_def_table, fn::Struct6_zero>));  // 0x004d0b90
B("obj_stub_56a480", 0x0056a480, (&obj_stub<d::g_sound_def_table, fn::Obj_call_4d0bc0>));  // 0x004d0bc0
B("obj_stub_56a4c0", 0x0056a4c0, (&obj_stub<d::g_sound_res_table, fn::Obj_clear6>));  // 0x00638fb0
B("obj_stub_56a510", 0x0056a510, (&obj_stub<d::g_sound_res_table, fn::Obj638_wrap_639030>));  // 0x00638fe0
B("obj_stub_56a550", 0x0056a550, (&obj_stub<d::g_particle_effect_array, fn::Obj_ctor_41b470>));  // 0x0041b470
B("obj_stub_56a5a0", 0x0056a5a0, (&obj_stub<d::g_particle_effect_array, fn::ParticleEffectArray_dtor>));  // 0x0041b4a0
B("obj_stub_56a5e0", 0x0056a5e0, (&obj_stub<d::g_saves, fn::Obj_ctor_476ee0>));  // 0x00476ee0
B("obj_stub_56a630", 0x0056a630, (&obj_stub<d::g_saves, fn::GameFileList_dtor>));  // 0x00476f30
B("obj_stub_56a670", 0x0056a670, (&obj_stub<d::g_lights, fn::Obj_Obj_4b0440>));  // 0x004b0440
B("obj_stub_56a6c0", 0x0056a6c0, (&obj_stub<d::g_lights, fn::Obj_dtor_4b0480>));  // 0x004b0480
B("obj_stub_56a700", 0x0056a700, (&obj_stub<d::g_a45f08, fn::Obj_ctor_560f90>));  // 0x00560f90
B("obj_stub_56a750", 0x0056a750, (&obj_stub<d::g_a45f08, fn::Mgr561090_dtor_Mgr>));  // 0x00560fe0
B("obj_stub_56a790", 0x0056a790, (&obj_stub<d::g_nations, fn::List4_14_ctor>));  // 0x004b6420
B("obj_stub_56a7e0", 0x0056a7e0, (&obj_stub<d::g_nations, fn::ObjM_dtor>));  // 0x004b6450
B("obj_stub_56a820", 0x0056a820, (&obj_stub<d::g_place_array, fn::Obj_ctor_4c51a0>));  // 0x004c51a0
B("obj_stub_56a870", 0x0056a870, (&obj_stub<d::g_place_array, fn::X_dtor_c5>));  // 0x004c51d0
B("obj_stub_56a8b0", 0x0056a8b0, (&obj_stub<d::g_places, fn::List_List_46cd00>));  // 0x0046cd00
B("obj_stub_56a900", 0x0056a900, (&obj_stub<d::g_places, fn::Obj_dtor_46cd30>));  // 0x0046cd30
B("obj_stub_56a940", 0x0056a940, (&obj_stub<d::g_territory_array, fn::X_ctor_5843d0>));  // 0x004e48f0
B("obj_stub_56a990", 0x0056a990, (&obj_stub<d::g_territory_array, fn::X_dtor_e49>));  // 0x004e4920
B("obj_stub_56a9d0", 0x0056a9d0, (&obj_stub<d::g_units, fn::Obj51d_ctor>));  // 0x0051dbe0
B("obj_stub_56aa20", 0x0056aa20, (&obj_stub<d::g_units, fn::Obj_dtor_Obj_51dc50>));  // 0x0051dc50
B("obj_stub_56aa60", 0x0056aa60, (&obj_stub<d::g_items, fn::ObjJ_ctor>));  // 0x0049e8d0
B("obj_stub_56aab0", 0x0056aab0, (&obj_stub<d::g_items, fn::Obj_dtor_Obj_49e910>));  // 0x0049e910
B("obj_stub_56aaf0", 0x0056aaf0, (&obj_stub<d::g_a50530, fn::Obj_ctor_4af910>));  // 0x004af910
B("obj_stub_56ab40", 0x0056ab40, (&obj_stub<d::g_a50530, fn::Obj_dtor_Obj_4af950>));  // 0x004af950
B("obj_stub_56abd0", 0x0056abd0, (&obj_stub<d::g_orders, fn::X_dtor_4e95f0>));  // 0x004e95f0
B("obj_stub_56ac10", 0x0056ac10, (&obj_stub<d::g_water_array, fn::X_ctor_5856a0>));  // 0x00546cc0
B("obj_stub_56ac60", 0x0056ac60, (&obj_stub<d::g_water_array, fn::X546cf0_dtor>));  // 0x00546cf0
B("obj_stub_56aca0", 0x0056aca0, (&obj_stub<d::g_actor_list, fn::Obj_ctor_56eca0>));  // 0x0056eca0
B("obj_stub_56acf0", 0x0056acf0, (&obj_stub<d::g_actor_list, fn::Obj_dtor_56ecd0>));  // 0x0056ecd0
B("obj_stub_56ad30", 0x0056ad30, (&obj_stub<d::g_queue, fn::X_ctor_e37>));  // 0x004e3700
B("Obj56ada0::forward", 0x0056ada0, &H::forward_plus4);
B("init_atexit_56ae10", 0x0056ae10, &reg_atexit<fn::obj_stub_56ae30>);
B("Obj56ae50::forward", 0x0056ae50, &H::forward);
B("obj_stub_56aea0", 0x0056aea0, (&obj_stub<d::g_profile_list, fn::Obj_ctor_4c9c30>));  // 0x004c9c30
B("init_atexit_56aec0", 0x0056aec0, &reg_atexit<fn::init_56aee0>);
B("obj_stub_56af30", 0x0056af30, (&obj_stub<d::g_slots_a3ce90, fn::List4_List4>));  // 0x00487600
#undef B

}  // namespace game_b105_13
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

// ------------------------------------------------------------------ 0x0056a120 (cdecl)
static u32 JOAN_CDECL static_dtor_56a120() {
    return joan::orig::arg::as<u32>(joan::orig::as_item_res(obj(d::g_item_res))->destruct());  // 0x0049f5b0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("static_dtor_56a120", 0x0056a120, &static_dtor_56a120);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ------------------------------------------------------------------ 0x0056ab60
static u32 JOAN_CDECL init_56ab60() {
    joan::orig::dtor_stub_56ab80();
    joan::orig::init_56aba0();
    return 0;  // void callee; _initterm ignores the eax
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("init_0x0056ab60", 0x0056ab60, &init_56ab60);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b105_14.cpp (game/b105_14)
namespace {
namespace game_b105_14 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using joan::orig::helpers::reg_atexit;
template <auto G, fn::Code Thunk>
u32 JOAN_CDECL obj_call() { return thiscall<u32>(Thunk, obj(G)); }

#define B(name, addr, fn) JOAN_REPLACE("game/b105_14", name, addr, fn)
B("dtor_stub_56af80", 0x0056af80, (&obj_call<d::g_slots_a3ce90, fn::List_dtor_List_487640>));  // 0x00487640
B("init_atexit_56aff0", 0x0056aff0, &reg_atexit<fn::fn_56b010>);
B("init_obj_56b060", 0x0056b060, (&obj_call<d::g_kinds, fn::ObjA_ctor>));  // 0x0046ec00
B("dtor_stub_56b0b0", 0x0056b0b0, (&obj_call<d::g_kinds, fn::FlockList_dtor>));  // 0x0046ec30
B("init_obj_56b0f0", 0x0056b0f0, (&obj_call<d::g_player_hud, fn::Obj_ctor_6f9440>));  // 0x006f9440
B("dtor_stub_56b140", 0x0056b140, (&obj_call<d::g_player_hud, fn::Obj_dtor_41cba0>));  // 0x0041cba0
B("init_obj_56b180", 0x0056b180, (&obj_call<d::g_main_menu, fn::Obj_ctor_434ce0>));  // 0x00434ce0
B("dtor_stub_56b1d0", 0x0056b1d0, (&obj_call<d::g_main_menu, fn::Obj_dtor_Obj_435ea0>));  // 0x00435ea0
B("init_obj_56b210", 0x0056b210, (&obj_call<d::g_critical_hud, fn::Obj_ctor_45f7a0>));  // 0x0045f7a0
B("dtor_stub_56b260", 0x0056b260, (&obj_call<d::g_critical_hud, fn::Obj_dtor_Obj_45f7f0>));  // 0x0045f7f0
B("init_obj_56b2a0", 0x0056b2a0, (&obj_call<d::g_a488e0, fn::Obj_self_6d65b0>));  // 0x006d65b0
B("dtor_stub_56b2f0", 0x0056b2f0, (&obj_call<d::g_a488e0, fn::Obj_fwd_56e0d0>));  // 0x0056e0d0
B("init_atexit_56b3e0", 0x0056b3e0, &reg_atexit<fn::dtor_stub_56b400>);
B("dtor_stub_56b400", 0x0056b400, (&obj_call<d::g_a56398, fn::Obj_dtor_420620>));  // 0x00420620
B("init_atexit_56b700", 0x0056b700, &reg_atexit<fn::fn_56b720>);
B("dtor_stub_56ba50", 0x0056ba50, (&obj_call<d::g_message_indicator, fn::Obj_call_4af4f0>));  // 0x0056e650
B("init_obj_56ba90", 0x0056ba90, (&obj_call<d::g_game_a5d380, fn::Obj_init_6ca040>));  // 0x006ca040
B("dtor_stub_56bae0", 0x0056bae0, (&obj_call<d::g_game_a5d380, fn::Obj_fwd_56e0f0>));  // 0x0056e0f0
B("init_obj_56bb30", 0x0056bb30, (&obj_call<d::g_roster_screen, fn::Obj6e6050_Obj>));  // 0x006e6050
B("init_atexit_56bb50", 0x0056bb50, &reg_atexit<fn::dtor_stub_56bb70>);
B("dtor_stub_56bb70", 0x0056bb70, (&obj_call<d::g_roster_screen, fn::Obj_fwd_56e180>));  // 0x0056e180
B("init_atexit_56bc40", 0x0056bc40, &reg_atexit<fn::dtor_stub_56bc60>);
B("dtor_stub_56bc60", 0x0056bc60, (&obj_call<d::g_start_objectives_screen, fn::Obj_call_75f5a0>));  // 0x0056e110
B("init_obj_56bcb0", 0x0056bcb0, (&obj_call<d::g_next_objectives_screen, fn::Struct1_zero>));  // 0x00755810
B("dtor_stub_56be10", 0x0056be10, (&obj_call<d::g_next_objectives_screen, fn::Obj_call_755860>));  // 0x0056e140
B("init_atexit_56be80", 0x0056be80, &reg_atexit<fn::atexit_56bea0>);
B("init_obj_56bef0", 0x0056bef0, (&obj_call<d::g_goal_panel, fn::Struct14_zero_6df0c0>));  // 0x006df0c0
B("dtor_stub_56bf40", 0x0056bf40, (&obj_call<d::g_goal_panel, fn::Obj_dtor_56e1d0>));  // 0x0056e1d0
B("init_obj_56bf90", 0x0056bf90, (&obj_call<d::g_quick_save_screen, fn::X_ctor_6e32d0>));  // 0x006e32d0
B("init_atexit_56bfb0", 0x0056bfb0, &reg_atexit<fn::fn_56bfd0>);
B("init_atexit_56c050", 0x0056c050, &reg_atexit<fn::dtor_stub_56c070>);
B("dtor_stub_56c070", 0x0056c070, (&obj_call<d::g_quit_popup, fn::Obj_dtor_Obj_41aae0>));  // 0x0041aae0
B("init_obj_56c0c0", 0x0056c0c0, (&obj_call<d::g_ending_screen, fn::Obj6ce_ctor>));  // 0x006ce050
B("dtor_stub_56c110", 0x0056c110, (&obj_call<d::g_ending_screen, fn::Obj_dtor_4254a0>));  // 0x004254a0
B("init_atexit_56c180", 0x0056c180, &reg_atexit<fn::fn_56c1a0>);
B("init_obj_56c1f0", 0x0056c1f0, (&obj_call<d::g_letter, fn::Obj_clear_1b>));  // 0x006e0490
B("dtor_stub_56c240", 0x0056c240, (&obj_call<d::g_letter, fn::Obj_call_6e0520>));  // 0x0056e280
B("init_obj_56c310", 0x0056c310, (&obj_call<d::g_victory_screen, fn::Screen_ctor_6e88d0>));  // 0x006e88d0
B("init_atexit_56c330", 0x0056c330, &reg_atexit<fn::fn_56c350>);
B("init_atexit_56c3d0", 0x0056c3d0, &reg_atexit<fn::dtor_stub_56c3f0>);
B("dtor_stub_56c3f0", 0x0056c3f0, (&obj_call<d::g_glory_screen, fn::Panel_dtor>));  // 0x00421420
B("init_obj_56c440", 0x0056c440, (&obj_call<d::g_video_player, fn::Obj_clear_f>));  // 0x006d5a40
B("dtor_stub_56c540", 0x0056c540, (&obj_call<d::g_video_player, fn::Obj_fwd_56e330>));  // 0x0056e330
B("init_atexit_56c5b0", 0x0056c5b0, &reg_atexit<fn::fn_56c5d0>);
B("init_obj_56c660", 0x0056c660, (&obj_call<d::g_barracks_screen, fn::Obj_clear_82>));  // 0x006bc4f0
B("dtor_stub_56c6b0", 0x0056c6b0, (&obj_call<d::g_barracks_screen, fn::Obj_fwd_56e2f0>));  // 0x0056e2f0
B("init_obj_56c770", 0x0056c770, (&obj_call<d::g_workshop_screen, fn::Obj6e8f10_Obj>));  // 0x006e8f10
B("init_atexit_56c790", 0x0056c790, &reg_atexit<fn::dtor_stub_56c7b0>);
B("dtor_stub_56c7b0", 0x0056c7b0, (&obj_call<d::g_workshop_screen, fn::Obj_fwd_56e310>));  // 0x0056e310
B("init_atexit_56c880", 0x0056c880, &reg_atexit<fn::dtor_stub_56c8a0>);
B("dtor_stub_56c8a0", 0x0056c8a0, (&obj_call<d::g_fade_title, fn::Obj_fwd_56e350>));  // 0x0056e350
B("init_obj_56c8f0", 0x0056c8f0, (&obj_call<d::g_opt_menu, fn::Struct124_zero>));  // 0x0073ba40
B("init_atexit_56c9b0", 0x0056c9b0, &reg_atexit<fn::atexit_56c9d0>);
B("init_obj_56cac0", 0x0056cac0, (&obj_call<d::g_ingame_menu, fn::Screen_ctor_6f2c10>));  // 0x006f2c10
B("dtor_stub_56cb10", 0x0056cb10, (&obj_call<d::g_ingame_menu, fn::Dialog_dtor>));  // 0x0056e3a0
B("init_obj_56cbe0", 0x0056cbe0, (&obj_call<d::g_progress, fn::Obj755df0_Obj>));  // 0x00755df0
B("init_atexit_56cc00", 0x0056cc00, &reg_atexit<fn::fn_56cc20>);
B("init_atexit_56cca0", 0x0056cca0, &reg_atexit<fn::dtor_stub_56ccc0>);
B("dtor_stub_56ccc0", 0x0056ccc0, (&obj_call<d::g_names, fn::Obj_dtor_43b440>));  // 0x0043b440
B("init_obj_56cd10", 0x0056cd10, (&obj_call<d::g_msg_log, fn::Obj_self_6e0e20>));  // 0x006e0e20
B("init_atexit_56cdd0", 0x0056cdd0, &reg_atexit<fn::fn_56cdf0>);
#undef B

}  // namespace game_b105_14
}  // namespace

// ---------------------------------------- from modules/game_b106_07.cpp (game/b106_07)
namespace {
namespace game_b106_07 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// 0x0056b420: CRT initialiser.
u32 JOAN_CDECL static_init_56b420() {
    cdecl_call<void>(fn::fn_56b6d0);
    return cdecl_call<u32>(fn::init_atexit_56b700);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b106_07", name, addr, fn)
B("static_init_56b420", 0x0056b420, &static_init_56b420);
#undef B

}  // namespace game_b106_07
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ------------------------------------------------------------------ 0x0056b9e0 (_initterm entry)
u32 JOAN_CDECL init_56b9e0() {
    joan::orig::init_56ba00();
    joan::orig::init_56ba20();
    return 0;  // void callee; _initterm ignores the eax
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("init_56b9e0", 0x0056b9e0, &init_56b9e0);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

u32 JOAN_CDECL static_dtor_a4fef8() { return joan::orig::arg::as<u32>(joan::orig::as_opt_menu(obj(d::g_opt_menu))->release_all_56e380()); }  // 0x0056c940 -> 0x0056e380

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("StaticDtor::a4fef8", 0x0056c940, &static_dtor_a4fef8);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

using joan::call::thiscall;
using joan::img::obj;

// ---- 0x0056c9d0 atexit destructor --------------------------------------------------------
void JOAN_CDECL atexit_56c9d0() { joan::orig::as_obj_menu(obj(d::g_objective_menu))->release(); }  // 0x0056e6b0

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("atexit_56c9d0", 0x0056c9d0, &atexit_56c9d0);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b105_15.cpp (game/b105_15)
namespace {
namespace game_b105_15 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// `mov ecx, Global; call Thunk` - static ctor / atexit dtor on a global object.
template <fn::Code Thunk, auto Global>
u32 JOAN_CDECL on_global() {
    return thiscall<u32>(Thunk, obj(Global));
}
// `push Fn; call atexit; add esp, 4`
using joan::orig::helpers::reg_atexit;
// Two direct calls (cdecl, no args), eax of the second.
template <fn::Code A, fn::Code B>
u32 JOAN_CDECL init_pair() {
    cdecl_call<u32>(A);
    return cdecl_call<u32>(B);
}

struct H {
    // 0x0056cf50
    u32 forward_6e1070() { joan::orig::as_msg_log(this)->release(); return 0; }  // 0x006e1070 (atexit stub: result ignored)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
// static ctors
B("StaticInit::ctor_a411d0", 0x0056ce40, (&on_global<fn::Screen_ctor_74c8d0, d::g_save_menu>));  // 0x0074c8d0
B("StaticInit::ctor_a53ff8", 0x0056cee0, (&on_global<fn::Screen9ed_ctor_6b6b70, d::g_game_over_screen>));  // 0x006b6b70
B("StaticInit::ctor_a59780", 0x0056cfa0, (&on_global<fn::Pair_clear_6b8d20, d::g_tooltip>));  // 0x006b8d20
B("StaticInit::ctor_a54c20", 0x0056d0d0, (&on_global<fn::Screen_ctor_6b94e0, d::g_armory_screen>));  // 0x006b94e0
B("StaticInit::ctor_a52930", 0x0056d320, (&on_global<fn::Obj_clear_c_6f5a20, d::g_screen_mgr>));  // 0x006f5a20
B("StaticInit::ctor_a52968", 0x0056d3c0, (&on_global<fn::Obj6b4_ctor, d::g_credits_screen>));  // 0x006b4e00
B("StaticInit::ctor_a50808", 0x0056d450, (&on_global<fn::Screen_ctor_62a560, d::g_popup_a50808>));  // 0x0062a560
// atexit dtors
B("StaticInit::dtor_a59780", 0x0056cfe0, (&on_global<fn::Obj_fwd_56ec80, d::g_tooltip>));  // 0x0056ec80
B("StaticInit::dtor_a55ad0", 0x0056d080, (&on_global<fn::Obj6f17e0_dtor_dtor, d::g_hud>));  // 0x006f17e0
B("StaticInit::dtor_a54c20", 0x0056d120, (&on_global<fn::ItemScreen_dtor, d::g_armory_screen>));  // 0x006b96b0
B("StaticInit::dtor_a52930", 0x0056d370, (&on_global<fn::Obj_nop_6f5a50, d::g_screen_mgr>));  // 0x006f5a50
B("StaticInit::dtor_a59a78", 0x0056d530, (&on_global<fn::Obj_empty_dtor_56df00, d::g_debug_overlay>));  // 0x0056df00
B("StaticInit::dtor_a48cc0", 0x0056da10, (&on_global<fn::Obj_call_5624c0, d::g_camera_man>));  // 0x00561590
// atexit registrations
B("StaticInit::atexit_56cf20", 0x0056cf00, &reg_atexit<fn::fn_56cf20>);
B("StaticInit::atexit_56cfe0", 0x0056cfc0, &reg_atexit<fn::StaticInit_dtor_a59780>);
B("StaticInit::atexit_56d080", 0x0056d060, &reg_atexit<fn::StaticInit_dtor_a55ad0>);
B("StaticInit::atexit_56d1b0", 0x0056d190, &reg_atexit<fn::atexit_56d1b0>);
B("StaticInit::atexit_56d400", 0x0056d3e0, &reg_atexit<fn::fn_56d400>);
B("StaticInit::atexit_56d490", 0x0056d470, &reg_atexit<fn::fn_56d490>);
B("StaticInit::atexit_56d530", 0x0056d510, &reg_atexit<fn::StaticInit_dtor_a59a78>);
B("StaticInit::atexit_56d5c0", 0x0056d5a0, &reg_atexit<fn::init_56d5c0>);
// CRT initialiser-table entries
B("StaticInit::init_56d5e0", 0x0056d5e0, (&init_pair<fn::init_56d610, fn::init_56d630>));
B("StaticInit::init_56d890", 0x0056d890, (&init_pair<fn::init_56d8b0, fn::init_56d8e0>));
B("StaticInit::init_56d9a0", 0x0056d9a0, (&init_pair<fn::init_56d9c0, fn::init_56d9e0>));
B("StaticInit::init_56da30", 0x0056da30, (&init_pair<fn::init_56da60, fn::init_56da80>));
B("StaticInit::init_56db60", 0x0056db60, (&init_pair<fn::init_56db80, fn::init_56dba0>));
B("StaticInit::init_56dbf0", 0x0056dbf0, (&init_pair<fn::init_56dc10, fn::init_56dc30>));
B("StaticInit::init_56dc80", 0x0056dc80, (&init_pair<fn::init_56dca0, fn::init_56dcc0>));
// others
B("Obj::forward_6e1070", 0x0056cf50, &H::forward_6e1070);
#undef B

}  // namespace game_b105_15
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
namespace {
namespace game_b100_10 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

// ------------------------------------------------------------------------------ cdecl / stdcall
std::uintptr_t JOAN_CDECL init_56d030() { return joan::orig::arg::as<u32>(joan::orig::as_ingame_frame(obj(d::g_hud))->construct()); }  // 0x006f1660

struct H {
    // ------------------------------------------------------------------------ small ones
    void* self_56ded0() { return this; }                                       // 0x0056ded0
    std::uintptr_t call_756190() { return joan::orig::arg::as<u32>(joan::orig::as_progress(this)->free_images()); }             // 0x0056e540
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("init_56d030", 0x0056d030, &init_56d030);
B("Obj::self_56ded0", 0x0056ded0, &H::self_56ded0);
B("Obj::call_756190", 0x0056e540, &H::call_756190);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b102_06.cpp (game/b102_06)
namespace {
namespace game_b102_06 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

void JOAN_CDECL atexit_56d1b0() { joan::orig::as_popup(obj(d::g_popup_a52df8))->destruct(); }  // 0x006d38a0
void JOAN_CDECL atexit_56d900() { joan::orig::as_player(obj(d::g_player_unit_recno))->release(); }  // 0x0042d900

#define B(name, addr, fn) JOAN_REPLACE("game/b102_06", name, addr, fn)
B("atexit_56d1b0", 0x0056d1b0, &atexit_56d1b0);
B("atexit_56d900", 0x0056d900, &atexit_56d900);
#undef B

}  // namespace game_b102_06
}  // namespace

// ---------------------------------------- from modules/game_b104_07.cpp (game/b104_07)
namespace {
namespace game_b104_07 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// 0x0056d610 / 0x0056d9c0 / 0x0056da60 / 0x0056db80: global constructors (thunk calls).
void* JOAN_CDECL init_56d610() { return joan::orig::as_profiler(obj(d::g_cpu_usage))->construct(); }  // 0x0045bfe0
void* JOAN_CDECL init_56d9c0() { return joan::orig::as_camera_man(obj(d::g_camera_man))->construct(); }  // 0x00561560
void* JOAN_CDECL init_56da60() { return joan::orig::arg::as<void*>(joan::orig::as_game(obj(d::g_game_state))->Game_46f090()); }  // 0x0046f090
void* JOAN_CDECL init_56db80() { return joan::orig::as_game(obj(d::g_game_a50088))->construct(); }  // 0x004d6b90

// 0x0056d8e0 / 0x0056da80 / 0x0056db10 / 0x0056dba0: atexit registrations.
i32 crt_atexit(const void* fn) { return joan::crt::atexit(fn); }
i32 JOAN_CDECL init_56d8e0() { return crt_atexit(fn::code_ptr(fn::atexit_56d900)); }
i32 JOAN_CDECL init_56da80() { return crt_atexit(fn::code_ptr(fn::fn_56daa0)); }
i32 JOAN_CDECL init_56db10() { return crt_atexit(fn::code_ptr(fn::obj_stub_56db30)); }
i32 JOAN_CDECL init_56dba0() { return crt_atexit(fn::code_ptr(fn::init_56dbc0)); }

#define B(name, addr, fn) JOAN_REPLACE("game/b104_07", name, addr, fn)
B("init_56d610", 0x0056d610, &init_56d610);
B("init_56d8e0", 0x0056d8e0, &init_56d8e0);
B("init_56d9c0", 0x0056d9c0, &init_56d9c0);
B("init_56da60", 0x0056da60, &init_56da60);
B("init_56da80", 0x0056da80, &init_56da80);
B("init_56db10", 0x0056db10, &init_56db10);
B("init_56db80", 0x0056db80, &init_56db80);
B("init_56dba0", 0x0056dba0, &init_56dba0);
#undef B

}  // namespace game_b104_07
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

// ------------------------------------------------------------------ 0x0056d870
static u32 JOAN_CDECL atexit_56d870() {
    return joan::orig::arg::as<u32>(joan::orig::as_profiler(obj(d::g_cpu_usage))->shutdown());  // 0x0045c010
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("atexit_56d870", 0x0056d870, &atexit_56d870);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b104_08.cpp (game/b104_08)
namespace {
namespace game_b104_08 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---- static initialisers ----
std::uintptr_t JOAN_CDECL init_56dc10() { return joan::orig::arg::as<u32>(joan::orig::as_direct_music(obj(d::g_a3ce50))->construct()); }  // 0x00582c20
i32 JOAN_CDECL init_56dc30() { return joan::crt::atexit(fn::code_ptr(fn::fn_56dc50)); }    // atexit
std::uintptr_t JOAN_CDECL init_56dca0() { return joan::orig::arg::as<u32>(joan::orig::as_music_segment(obj(d::g_a58f78))->construct()); }  // 0x00583aa0
i32 JOAN_CDECL init_56dcc0() { return joan::crt::atexit(fn::code_ptr(fn::atexit_dtor_a58f78)); }    // atexit

struct H {
    // ---- forwarders ----
    u32 fwd_56deb0() { joan::orig::as_race_res(this)->free(); return 0; }  // 0x004ca350 (atexit stub: result ignored)
    u32 fwd_56e0d0() { joan::orig::as_inventory(this)->release(); return 0; }  // 0x006d6650 (atexit stub: result ignored)
    u32 fwd_56e0f0() { joan::orig::as_cutscene(this)->release(); return 0; }  // 0x006ca0d0 (atexit stub: result ignored)
    u32 fwd_56e180() { joan::orig::as_roster_screen(this)->shutdown(); return 0; }  // 0x006e60a0 (atexit stub: result ignored)
    std::uintptr_t fwd_56e2f0() { return joan::orig::arg::as<u32>(joan::orig::as_barracks_screen(this)->release()); }  // 0x006bcf00
    std::uintptr_t fwd_56e310() { return joan::orig::arg::as<u32>(joan::orig::as_workshop_screen(this)->release()); }  // 0x006e9a50
    u32 fwd_56e330() { joan::orig::as_video_player(this)->shutdown(); return 0; }  // 0x006d5ab0 (atexit stub: result ignored)
    u32 fwd_56e350() { joan::orig::as_screen(this)->nop_7612f0(); return 0; }  // 0x007612f0 (atexit stub: result ignored)
    u32 fwd_56ec80() { joan::orig::as_help_box(this)->release(); return 0; }  // 0x006b8d80 (atexit stub: result ignored)
    void empty_dtor() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_08", name, addr, fn)
B("init_56dc10", 0x0056dc10, &init_56dc10);
B("init_56dc30", 0x0056dc30, &init_56dc30);
B("init_56dca0", 0x0056dca0, &init_56dca0);
B("init_56dcc0", 0x0056dcc0, &init_56dcc0);
B("Obj::fwd_56deb0", 0x0056deb0, &H::fwd_56deb0);
B("Obj::empty_dtor_56df00", 0x0056df00, &H::empty_dtor);
B("Obj::fwd_56e0d0", 0x0056e0d0, &H::fwd_56e0d0);
B("Obj::fwd_56e0f0", 0x0056e0f0, &H::fwd_56e0f0);
B("Obj::fwd_56e180", 0x0056e180, &H::fwd_56e180);
B("Obj::fwd_56e2f0", 0x0056e2f0, &H::fwd_56e2f0);
B("Obj::fwd_56e310", 0x0056e310, &H::fwd_56e310);
B("Obj::fwd_56e330", 0x0056e330, &H::fwd_56e330);
B("Obj::fwd_56e350", 0x0056e350, &H::fwd_56e350);
B("Obj::fwd_56ec80", 0x0056ec80, &H::fwd_56ec80);
#undef B

}  // namespace game_b104_08
}  // namespace

// ---------------------------------------- from modules/game_b103_07.cpp (game/b103_07)
namespace {
namespace game_b103_07 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// ---------------------------------------------------------------- static init / atexit
// 0x0056dce0
void JOAN_CDECL atexit_dtor_a58f78() { joan::orig::as_music_segment(obj(d::g_a58f78))->destruct(); }  // 0x00583c40
// 0x0056dd70
void JOAN_CDECL error_callback_nop() {}
// 0x0056df20 .. 0x0056dff0 (direct calls, no thunk in the original)
void JOAN_CDECL init_56df20() { joan::orig::init_pi_a4f81c(); }
void JOAN_CDECL init_56df60() { joan::orig::init_a59734(); }
void JOAN_CDECL init_56dfb0() { joan::orig::init_a59778(); }
void JOAN_CDECL init_56dff0() { joan::orig::init_a5977c(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("atexit_dtor_a58f78", 0x0056dce0, &atexit_dtor_a58f78);
B("error_callback_nop", 0x0056dd70, &error_callback_nop);
B("static_init_56df20", 0x0056df20, &init_56df20);
B("static_init_56df60", 0x0056df60, &init_56df60);
B("static_init_56dfb0", 0x0056dfb0, &init_56dfb0);
B("static_init_56dff0", 0x0056dff0, &init_56dff0);
#undef B

}  // namespace game_b103_07
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

struct H {
    // 0x0056ddc0
    bool ptr_valid() { return jo::self<jo::Surface>(this)->surface != nullptr; }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Ptr::valid", 0x0056ddc0, &H::ptr_valid);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b82_10.cpp (game/b82_10)
namespace {
namespace game_b82_10 {

struct H {
    // 0x0056de20: first dword.
    void* first_dword() { return jo::self<jo::Blob>(this)->data; }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_10", name, addr, fn)
B82("Blob::data", 0x0056de20, &H::first_dword);
#undef B82

}  // namespace game_b82_10
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x0056e060
    u32 wrap_70e3c0() { return joan::orig::arg::as<u32>(joan::orig::as_screen(this)->release_70e3c0()); }  // 0x0070e3c0
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("X::wrap_70e3c0", 0x0056e060, &H::wrap_70e3c0);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b103_06.cpp (game/b103_06)
//
//    - C++ static initialisers (cdecl, no args; entries of the initialiser table at 0x9f43b8..)
//      calling one direct (non-thunk) helper that computes a float global: 0x00523f00 (0x0041bd30),
//      0x00524a30 (0x00524a80), 0x0052b8c0 (0x00415040), 0x0052b910 (0x0052b940), 0x0054d7d0
//      (0x0054d820), 0x0054da10 (0x0054da70); 0x00564110 (0x004168a0 then 0x00564150).
//
//    - atexit terminators / ctor wrappers (cdecl, no args) calling one thiscall on a global
//      (eax = the callee's result): 0x00564050, 0x005642b0, 0x00564b00, 0x00566dd0, 0x00566ec0,
//      0x00566f60, 0x00567280, 0x00567480, 0x0056a000, 0x0056b010, 0x0056b3b0, 0x0056b6d0,
//      0x0056b720, 0x0056bfd0, 0x0056c1a0, 0x0056c350, 0x0056c3a0, 0x0056c5d0, 0x0056cc20,
//      0x0056cdf0, 0x0056cf20, 0x0056d160, 0x0056d400, 0x0056d490, 0x0056daa0, 0x0056dc50.
#define M "game/b103_06"

namespace {
namespace game_b103_06 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

u32 JOAN_CDECL init_564110() {
    joan::orig::static_dtor_4168a0();
    joan::orig::init_564150();
    return 0;  // void callee; _initterm ignores the eax
}

// ---- terminators / ctor wrappers: one thiscall on a global ----
u32 on_global(std::uintptr_t thunk, img::Addr g) { return thiscall<u32>(thunk, obj(g)); }
u32 JOAN_CDECL term_564050() { return on_global(fn::code_value(fn::Renderer_shutdown_wrapper), d::g_sys3d); }  // 0x005f65f0
u32 JOAN_CDECL term_5642b0() { return on_global(fn::code_value(fn::Obj_call_609260), d::g_texture_array); }  // 0x00608de0
u32 JOAN_CDECL term_564b00() { return on_global(fn::code_value(fn::Font_dtor), d::g_a5cc90); }  // 0x00623e70
u32 JOAN_CDECL term_566dd0() { return on_global(fn::code_value(fn::Audio_dtor), d::g_audio); }  // 0x0057a3c0
u32 JOAN_CDECL term_566ec0() { return on_global(fn::code_value(fn::Obj_call_59ad70), d::g_snd); }  // 0x0059ad00
u32 JOAN_CDECL term_566f60() { return on_global(fn::code_value(fn::Obj_call_586010), d::g_mus); }  // 0x00585fa0
u32 JOAN_CDECL ctor_567280() { return on_global(fn::code_value(fn::Obj63bf40_init), d::g_dialogue); }  // 0x0063bf40
u32 JOAN_CDECL term_567480() { return on_global(fn::code_value(fn::Obj_dtor_4187b0), d::g_anim_res_mgr); }  // 0x004187b0
u32 JOAN_CDECL term_56a000() { return on_global(fn::code_value(fn::Obj_dtor_Obj_648ae0), d::g_scripts); }  // 0x00648ae0
u32 JOAN_CDECL term_56b010() { return on_global(fn::code_value(fn::Obj_forward_464390), d::g_dyn_grass); }  // 0x00464170
u32 JOAN_CDECL ctor_56b3b0() { return on_global(fn::code_value(fn::Screen_ctor_6f5f60), d::g_a56398); }  // 0x006f5f60
u32 JOAN_CDECL ctor_56b6d0() { return on_global(fn::code_value(fn::Screen_ctor_6ef290), d::g_combat_screen); }  // 0x006ef290
u32 JOAN_CDECL term_56b720() { return on_global(fn::code_value(fn::Big_dtor_427830), d::g_combat_screen); }  // 0x00427830
u32 JOAN_CDECL term_56bfd0() { return on_global(fn::code_value(fn::Obj_dtor_Obj_423db0), d::g_quick_save_screen); }  // 0x00423db0
u32 JOAN_CDECL term_56c1a0() { return on_global(fn::code_value(fn::Obj_dtor_41c710), d::g_sms); }  // 0x0041c710
u32 JOAN_CDECL term_56c350() { return on_global(fn::code_value(fn::CamMan_dtor_425520), d::g_victory_screen); }  // 0x00425520
u32 JOAN_CDECL ctor_56c3a0() { return on_global(fn::code_value(fn::Screen_ctor_6e6830), d::g_glory_screen); }  // 0x006e6830
u32 JOAN_CDECL term_56c5d0() { return on_global(fn::code_value(fn::Obj_call_6d2380), d::g_talk_prompt); }  // 0x0056e2c0
u32 JOAN_CDECL term_56cc20() { return on_global(fn::code_value(fn::Obj_call_756190), d::g_progress); }  // 0x0056e540
u32 JOAN_CDECL term_56cdf0() { return on_global(fn::code_value(fn::Obj_dtor_4b2b10), d::g_portraits); }  // 0x004b2b10
u32 JOAN_CDECL term_56cf20() { return on_global(fn::code_value(fn::Obj_dtor_Obj_426280), d::g_game_over_screen); }  // 0x00426280
u32 JOAN_CDECL ctor_56d160() { return on_global(fn::code_value(fn::Screen_ctor_6d37f0), d::g_popup_a52df8); }  // 0x006d37f0
u32 JOAN_CDECL term_56d400() { return on_global(fn::code_value(fn::Obj6b4ea0_dtor_dtor), d::g_credits_screen); }  // 0x006b4ea0
u32 JOAN_CDECL term_56d490() { return on_global(fn::code_value(fn::Obj_dtor_62a5f0), d::g_popup_a50808); }  // 0x0062a5f0
u32 JOAN_CDECL term_56daa0() { return on_global(fn::code_value(fn::SlotOwner_dtor), d::g_game_state); }  // 0x0046f130
u32 JOAN_CDECL term_56dc50() { return on_global(fn::code_value(fn::DSound_shutdown), d::g_a3ce50); }  // 0x00582c60

JOAN_REPLACE(M, "init_564110", 0x00564110, &init_564110);
JOAN_REPLACE(M, "term_564050", 0x00564050, &term_564050);
JOAN_REPLACE(M, "term_5642b0", 0x005642b0, &term_5642b0);
JOAN_REPLACE(M, "term_564b00", 0x00564b00, &term_564b00);
JOAN_REPLACE(M, "term_566dd0", 0x00566dd0, &term_566dd0);
JOAN_REPLACE(M, "term_566ec0", 0x00566ec0, &term_566ec0);
JOAN_REPLACE(M, "term_566f60", 0x00566f60, &term_566f60);
JOAN_REPLACE(M, "ctor_567280", 0x00567280, &ctor_567280);
JOAN_REPLACE(M, "term_567480", 0x00567480, &term_567480);
JOAN_REPLACE(M, "term_56a000", 0x0056a000, &term_56a000);
JOAN_REPLACE(M, "term_56b010", 0x0056b010, &term_56b010);
JOAN_REPLACE(M, "ctor_56b3b0", 0x0056b3b0, &ctor_56b3b0);
JOAN_REPLACE(M, "ctor_56b6d0", 0x0056b6d0, &ctor_56b6d0);
JOAN_REPLACE(M, "term_56b720", 0x0056b720, &term_56b720);
JOAN_REPLACE(M, "term_56bfd0", 0x0056bfd0, &term_56bfd0);
JOAN_REPLACE(M, "term_56c1a0", 0x0056c1a0, &term_56c1a0);
JOAN_REPLACE(M, "term_56c350", 0x0056c350, &term_56c350);
JOAN_REPLACE(M, "ctor_56c3a0", 0x0056c3a0, &ctor_56c3a0);
JOAN_REPLACE(M, "term_56c5d0", 0x0056c5d0, &term_56c5d0);
JOAN_REPLACE(M, "term_56cc20", 0x0056cc20, &term_56cc20);
JOAN_REPLACE(M, "term_56cdf0", 0x0056cdf0, &term_56cdf0);
JOAN_REPLACE(M, "term_56cf20", 0x0056cf20, &term_56cf20);
JOAN_REPLACE(M, "ctor_56d160", 0x0056d160, &ctor_56d160);
JOAN_REPLACE(M, "term_56d400", 0x0056d400, &term_56d400);
JOAN_REPLACE(M, "term_56d490", 0x0056d490, &term_56d490);
JOAN_REPLACE(M, "term_56daa0", 0x0056daa0, &term_56daa0);
JOAN_REPLACE(M, "term_56dc50", 0x0056dc50, &term_56dc50);

}  // namespace game_b103_06
}  // namespace

#undef M

// ---------------------------------------- from modules/game_b105_09.cpp (game/b105_09)
//
//    - atexit terminators (cdecl, no args, ret 0): `mov ecx, global; call 0x0040aa33`
//      (-> 0x00623e70, an empty destructor that only spills ecx). eax = whatever the callee
//      leaves (in the original: the eax the terminator was entered with, i.e. stale); the only
//      caller is the CRT doexit loop, which ignores it. Function (global object):
//      0x005655a0 (0x00a52888), 0x00565630 (0x00a47d18), 0x005656c0 (0x00a460e8), 0x00565750
//      (0x00a56ad8), 0x005657e0 (0x00a40420), 0x00565870 (0x00a5cd20), 0x00565900 (0x00a55a48),
//      0x00565990 (0x00a56de0), 0x00565a20 (0x00a57f40), 0x00565ab0 (0x00a50788), 0x00565b40
//      (0x00a489c8), 0x00565bd0 (0x00a58ac0), 0x00565c60 (0x00a52848), 0x00565cf0 (0x00a48f70),
//      0x00565d80 (0x00a506a0), 0x00565e10 (0x00a48a68), 0x00565ea0 (0x00a596d8), 0x00565f30
//      (0x00a52808), 0x00565fc0 (0x00a46060), 0x00566050 (0x00a5cdc0), 0x005660e0 (0x00a4d518),
//      0x00566170 (0x00a59738), 0x00566200 (0x00a48d48), 0x00566290 (0x00a51608), 0x00566320
//      (0x00a5d3c8), 0x005663b0 (0x00a56da0), 0x00566440 (0x00a48d08), 0x005664d0 (0x00a56f60),
//      0x00566560 (0x00a47dc8), 0x005665f0 (0x00a4d558), 0x00566680 (0x00a48fb0)
//
//    - static initialisers (cdecl, no args, ret 0): `push terminator; call atexit 0x007b361a`,
//      eax = atexit's result. Function (registered terminator, original address kept):
//      0x00565610 (0x00565630), 0x005656a0 (0x005656c0), 0x00565730 (0x00565750), 0x005657c0
//      (0x005657e0), 0x00565850 (0x00565870), 0x005658e0 (0x00565900), 0x00565970 (0x00565990),
//      0x00565a00 (0x00565a20), 0x00565a90 (0x00565ab0), 0x00565b20 (0x00565b40), 0x00565bb0
//      (0x00565bd0), 0x00565c40 (0x00565c60), 0x00565cd0 (0x00565cf0), 0x00565d60 (0x00565d80),
//      0x00565df0 (0x00565e10), 0x00565e80 (0x00565ea0), 0x00565f10 (0x00565f30), 0x00565fa0
//      (0x00565fc0), 0x00566030 (0x00566050), 0x005660c0 (0x005660e0), 0x00566150 (0x00566170),
//      0x005661e0 (0x00566200), 0x00566270 (0x00566290), 0x00566300 (0x00566320), 0x00566390
//      (0x005663b0), 0x00566420 (0x00566440), 0x005664b0 (0x005664d0), 0x00566540 (0x00566560),
//      0x005665d0 (0x005665f0), 0x00566660 (0x00566680)
#define M "game/b105_09"

namespace {
namespace game_b105_09 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// thiscall 0x0040aa33 -> 0x00623e70 on a global object
u32 empty_dtor(img::Addr g) { joan::orig::as_font(obj(g))->dtor(); return 0; }  // 0x00623e70 (atexit terminators: result ignored)
// atexit 0x007b361a (cdecl), pushes the terminator's original address
i32 crt_atexit(std::uintptr_t fn) { return joan::crt::atexit(obj(fn)); }

u32 JOAN_CDECL term_5655a0() { return empty_dtor(d::g_a52888); }
u32 JOAN_CDECL term_565630() { return empty_dtor(d::g_font_a47d18); }
u32 JOAN_CDECL term_5656c0() { return empty_dtor(d::g_a460e8); }
u32 JOAN_CDECL term_565750() { return empty_dtor(d::g_font_18pt_attribute_numbers_out); }
u32 JOAN_CDECL term_5657e0() { return empty_dtor(d::g_font_18pt_attribute_numbers_over); }
u32 JOAN_CDECL term_565870() { return empty_dtor(d::g_font_18pt_dark_numbers); }
u32 JOAN_CDECL term_565900() { return empty_dtor(d::g_font_18pt_light_numbers); }
u32 JOAN_CDECL term_565990() { return empty_dtor(d::g_font_24pt_large_numbers_gold); }
u32 JOAN_CDECL term_565a20() { return empty_dtor(d::g_a57f40); }
u32 JOAN_CDECL term_565ab0() { return empty_dtor(d::g_dialogs); }
u32 JOAN_CDECL term_565b40() { return empty_dtor(d::g_font_a489c8); }
u32 JOAN_CDECL term_565bd0() { return empty_dtor(d::g_text_a58ac0); }
u32 JOAN_CDECL term_565c60() { return empty_dtor(d::g_font_verdana_12_gold_black_outline); }
u32 JOAN_CDECL term_565cf0() { return empty_dtor(d::g_font_celtic_10pt_english); }
u32 JOAN_CDECL term_565d80() { return empty_dtor(d::g_a506a0); }
u32 JOAN_CDECL term_565e10() { return empty_dtor(d::g_font_celtic_18pt_black); }
u32 JOAN_CDECL term_565ea0() { return empty_dtor(d::g_a596d8); }
u32 JOAN_CDECL term_565f30() { return empty_dtor(d::g_a52808); }
u32 JOAN_CDECL term_565fc0() { return empty_dtor(d::g_font_goudy18_27_white); }
u32 JOAN_CDECL term_566050() { return empty_dtor(d::g_font_a5cdc0); }
u32 JOAN_CDECL term_5660e0() { return empty_dtor(d::g_a4d518); }
u32 JOAN_CDECL term_566170() { return empty_dtor(d::g_a59738); }
u32 JOAN_CDECL term_566200() { return empty_dtor(d::g_font_goudy_bold_18); }
u32 JOAN_CDECL term_566290() { return empty_dtor(d::g_font_goudy14_22_white); }
u32 JOAN_CDECL term_566320() { return empty_dtor(d::g_font_goudy12_bold_black); }
u32 JOAN_CDECL term_5663b0() { return empty_dtor(d::g_font_goudy12_18_target_unit); }
u32 JOAN_CDECL term_566440() { return empty_dtor(d::g_font_goudy_bold_24); }
u32 JOAN_CDECL term_5664d0() { return empty_dtor(d::g_font_goudy_bold_gold_24); }
u32 JOAN_CDECL term_566560() { return empty_dtor(d::g_font_goudy_bold_20); }
u32 JOAN_CDECL term_5665f0() { return empty_dtor(d::g_font_goudy_bold_48); }
u32 JOAN_CDECL term_566680() { return empty_dtor(d::g_font_goudy_18); }
i32 JOAN_CDECL init_565610() { return crt_atexit(fn::code_value(fn::fn_565630)); }
i32 JOAN_CDECL init_5656a0() { return crt_atexit(fn::code_value(fn::fn_5656c0)); }
i32 JOAN_CDECL init_565730() { return crt_atexit(fn::code_value(fn::fn_565750)); }
i32 JOAN_CDECL init_5657c0() { return crt_atexit(fn::code_value(fn::fn_5657e0)); }
i32 JOAN_CDECL init_565850() { return crt_atexit(fn::code_value(fn::fn_565870)); }
i32 JOAN_CDECL init_5658e0() { return crt_atexit(fn::code_value(fn::fn_565900)); }
i32 JOAN_CDECL init_565970() { return crt_atexit(fn::code_value(fn::fn_565990)); }
i32 JOAN_CDECL init_565a00() { return crt_atexit(fn::code_value(fn::fn_565a20)); }
i32 JOAN_CDECL init_565a90() { return crt_atexit(fn::code_value(fn::fn_565ab0)); }
i32 JOAN_CDECL init_565b20() { return crt_atexit(fn::code_value(fn::fn_565b40)); }
i32 JOAN_CDECL init_565bb0() { return crt_atexit(fn::code_value(fn::fn_565bd0)); }
i32 JOAN_CDECL init_565c40() { return crt_atexit(fn::code_value(fn::fn_565c60)); }
i32 JOAN_CDECL init_565cd0() { return crt_atexit(fn::code_value(fn::fn_565cf0)); }
i32 JOAN_CDECL init_565d60() { return crt_atexit(fn::code_value(fn::fn_565d80)); }
i32 JOAN_CDECL init_565df0() { return crt_atexit(fn::code_value(fn::fn_565e10)); }
i32 JOAN_CDECL init_565e80() { return crt_atexit(fn::code_value(fn::fn_565ea0)); }
i32 JOAN_CDECL init_565f10() { return crt_atexit(fn::code_value(fn::fn_565f30)); }
i32 JOAN_CDECL init_565fa0() { return crt_atexit(fn::code_value(fn::fn_565fc0)); }
i32 JOAN_CDECL init_566030() { return crt_atexit(fn::code_value(fn::fn_566050)); }
i32 JOAN_CDECL init_5660c0() { return crt_atexit(fn::code_value(fn::fn_5660e0)); }
i32 JOAN_CDECL init_566150() { return crt_atexit(fn::code_value(fn::fn_566170)); }
i32 JOAN_CDECL init_5661e0() { return crt_atexit(fn::code_value(fn::fn_566200)); }
i32 JOAN_CDECL init_566270() { return crt_atexit(fn::code_value(fn::fn_566290)); }
i32 JOAN_CDECL init_566300() { return crt_atexit(fn::code_value(fn::fn_566320)); }
i32 JOAN_CDECL init_566390() { return crt_atexit(fn::code_value(fn::fn_5663b0)); }
i32 JOAN_CDECL init_566420() { return crt_atexit(fn::code_value(fn::fn_566440)); }
i32 JOAN_CDECL init_5664b0() { return crt_atexit(fn::code_value(fn::fn_5664d0)); }
i32 JOAN_CDECL init_566540() { return crt_atexit(fn::code_value(fn::fn_566560)); }
i32 JOAN_CDECL init_5665d0() { return crt_atexit(fn::code_value(fn::fn_5665f0)); }
i32 JOAN_CDECL init_566660() { return crt_atexit(fn::code_value(fn::fn_566680)); }

JOAN_REPLACE(M, "term_5655a0", 0x005655a0, &term_5655a0);
JOAN_REPLACE(M, "term_565630", 0x00565630, &term_565630);
JOAN_REPLACE(M, "term_5656c0", 0x005656c0, &term_5656c0);
JOAN_REPLACE(M, "term_565750", 0x00565750, &term_565750);
JOAN_REPLACE(M, "term_5657e0", 0x005657e0, &term_5657e0);
JOAN_REPLACE(M, "term_565870", 0x00565870, &term_565870);
JOAN_REPLACE(M, "term_565900", 0x00565900, &term_565900);
JOAN_REPLACE(M, "term_565990", 0x00565990, &term_565990);
JOAN_REPLACE(M, "term_565a20", 0x00565a20, &term_565a20);
JOAN_REPLACE(M, "term_565ab0", 0x00565ab0, &term_565ab0);
JOAN_REPLACE(M, "term_565b40", 0x00565b40, &term_565b40);
JOAN_REPLACE(M, "term_565bd0", 0x00565bd0, &term_565bd0);
JOAN_REPLACE(M, "term_565c60", 0x00565c60, &term_565c60);
JOAN_REPLACE(M, "term_565cf0", 0x00565cf0, &term_565cf0);
JOAN_REPLACE(M, "term_565d80", 0x00565d80, &term_565d80);
JOAN_REPLACE(M, "term_565e10", 0x00565e10, &term_565e10);
JOAN_REPLACE(M, "term_565ea0", 0x00565ea0, &term_565ea0);
JOAN_REPLACE(M, "term_565f30", 0x00565f30, &term_565f30);
JOAN_REPLACE(M, "term_565fc0", 0x00565fc0, &term_565fc0);
JOAN_REPLACE(M, "term_566050", 0x00566050, &term_566050);
JOAN_REPLACE(M, "term_5660e0", 0x005660e0, &term_5660e0);
JOAN_REPLACE(M, "term_566170", 0x00566170, &term_566170);
JOAN_REPLACE(M, "term_566200", 0x00566200, &term_566200);
JOAN_REPLACE(M, "term_566290", 0x00566290, &term_566290);
JOAN_REPLACE(M, "term_566320", 0x00566320, &term_566320);
JOAN_REPLACE(M, "term_5663b0", 0x005663b0, &term_5663b0);
JOAN_REPLACE(M, "term_566440", 0x00566440, &term_566440);
JOAN_REPLACE(M, "term_5664d0", 0x005664d0, &term_5664d0);
JOAN_REPLACE(M, "term_566560", 0x00566560, &term_566560);
JOAN_REPLACE(M, "term_5665f0", 0x005665f0, &term_5665f0);
JOAN_REPLACE(M, "term_566680", 0x00566680, &term_566680);
JOAN_REPLACE(M, "init_565610", 0x00565610, &init_565610);
JOAN_REPLACE(M, "init_5656a0", 0x005656a0, &init_5656a0);
JOAN_REPLACE(M, "init_565730", 0x00565730, &init_565730);
JOAN_REPLACE(M, "init_5657c0", 0x005657c0, &init_5657c0);
JOAN_REPLACE(M, "init_565850", 0x00565850, &init_565850);
JOAN_REPLACE(M, "init_5658e0", 0x005658e0, &init_5658e0);
JOAN_REPLACE(M, "init_565970", 0x00565970, &init_565970);
JOAN_REPLACE(M, "init_565a00", 0x00565a00, &init_565a00);
JOAN_REPLACE(M, "init_565a90", 0x00565a90, &init_565a90);
JOAN_REPLACE(M, "init_565b20", 0x00565b20, &init_565b20);
JOAN_REPLACE(M, "init_565bb0", 0x00565bb0, &init_565bb0);
JOAN_REPLACE(M, "init_565c40", 0x00565c40, &init_565c40);
JOAN_REPLACE(M, "init_565cd0", 0x00565cd0, &init_565cd0);
JOAN_REPLACE(M, "init_565d60", 0x00565d60, &init_565d60);
JOAN_REPLACE(M, "init_565df0", 0x00565df0, &init_565df0);
JOAN_REPLACE(M, "init_565e80", 0x00565e80, &init_565e80);
JOAN_REPLACE(M, "init_565f10", 0x00565f10, &init_565f10);
JOAN_REPLACE(M, "init_565fa0", 0x00565fa0, &init_565fa0);
JOAN_REPLACE(M, "init_566030", 0x00566030, &init_566030);
JOAN_REPLACE(M, "init_5660c0", 0x005660c0, &init_5660c0);
JOAN_REPLACE(M, "init_566150", 0x00566150, &init_566150);
JOAN_REPLACE(M, "init_5661e0", 0x005661e0, &init_5661e0);
JOAN_REPLACE(M, "init_566270", 0x00566270, &init_566270);
JOAN_REPLACE(M, "init_566300", 0x00566300, &init_566300);
JOAN_REPLACE(M, "init_566390", 0x00566390, &init_566390);
JOAN_REPLACE(M, "init_566420", 0x00566420, &init_566420);
JOAN_REPLACE(M, "init_5664b0", 0x005664b0, &init_5664b0);
JOAN_REPLACE(M, "init_566540", 0x00566540, &init_566540);
JOAN_REPLACE(M, "init_5665d0", 0x005665d0, &init_5665d0);
JOAN_REPLACE(M, "init_566660", 0x00566660, &init_566660);

}  // namespace game_b105_09
}  // namespace

#undef M

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_564150() -> void { joan::bind::free<&::game_b97_04::init_564150>(); }  // 0x564150
auto joan::orig::init_564250() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_09::init_564250>()); }  // 0x564250
auto joan::orig::init_564280() -> void { joan::bind::free<&::game_b98_04::init_564280>(); }  // 0x564280
auto joan::orig::atexit_564860() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_08::atexit_564860>()); }  // 0x564830
auto joan::orig::clear_a5b9a8() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b95_04::clear_a5b9a8>()); }  // 0x565310
auto joan::orig::init_565d30() -> void { joan::bind::free<&::game_b98_04::init_565d30>(); }  // 0x565d30
auto joan::orig::init_566750() -> void { joan::bind::free<&::game_b96_05::init_566750>(); }  // 0x566750
auto joan::orig::TxtRes::destruct_566c40() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_05::H::forward_566c40>(this)); }  // 0x566c40
auto joan::orig::init_566cb0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b107_11::init_566cb0>()); }  // 0x566cb0
auto joan::orig::TxtRes::destruct_566d20() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_01::H::wrap_566d20>(this)); }  // 0x566d20
auto joan::orig::TxtRes::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b105_10::H::ctor_566d40>(this)); }  // 0x566d40
auto joan::orig::init_5673a0() -> void { joan::bind::free<&::game_b100_09::init_5673a0>(); }  // 0x5673a0
auto joan::orig::ModelResMgr::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_09::H::forward_5675e0>(this)); }  // 0x5675e0
auto joan::orig::init_5680c0() -> void { joan::bind::free<&::game_b94_05::init_5680c0>(); }  // 0x5680c0
auto joan::orig::init_568ae0() -> void { joan::bind::free<&::game_b97_05::init_568ae0>(); }  // 0x568ae0
auto joan::orig::init_569590() -> void { joan::bind::free<&::game_b96_05::init_569590>(); }  // 0x569590
auto joan::orig::init_569e90() -> void { joan::bind::free<&::game_b98_05::init_569e90>(); }  // 0x569e90
auto joan::orig::init_56ab10() -> void { joan::bind::free<&::game_b98_05::init_56ab10>(); }  // 0x56ab10
auto joan::orig::init_56aba0() -> void { joan::bind::free<&::game_b95_05::init_56aba0>(); }  // 0x56aba0
auto joan::orig::Talk::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_13::H::forward_plus4>(this)); }  // 0x56ada0
auto joan::orig::init_56ba00() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b107_19::init_56ba00>()); }  // 0x56ba00
auto joan::orig::init_56ba20() -> void { joan::bind::free<&::game_b100_09::init_56ba20>(); }  // 0x56ba20
auto joan::orig::init_56bf10() -> void { joan::bind::free<&::game_b94_05::init_56bf10>(); }  // 0x56bf10
auto joan::orig::Panel::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_15::H::forward_6e1070>(this)); }  // 0x56cf50
auto joan::orig::init_56d4e0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b101_09::init_56d4e0>()); }  // 0x56d4e0
auto joan::orig::init_56daf0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b107_08::init_56daf0>()); }  // 0x56daf0
auto joan::orig::init_56db10() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b104_07::init_56db10>()); }  // 0x56db10
auto joan::orig::ErrorLog::set_callback(std::uintptr_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_06::H::set_first>(this, a0)); }  // 0x56dd30
auto joan::orig::Canvas::valid() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b90_04::H::ptr_valid>(this)); }  // 0x56ddc0
auto joan::orig::Blob::data_56de20() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_10::H::first_dword>(this)); }  // 0x56de20
auto joan::orig::DebugOverlay::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_10::H::self_56ded0>(this)); }  // 0x56ded0
auto joan::orig::Screen::release_thunk() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b93_02::H::wrap_70e3c0>(this)); }  // 0x56e060
auto joan::orig::OptMenu::release_all_56e380() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_05::H::forward_56e380>(this)); }  // 0x56e380
auto joan::orig::ObjMenu::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_17::H::fwd_56e6b0>(this)); }  // 0x56e6b0
