// Engine-wide globals and services of joan.exe that reimplemented code still
// calls into: the error reporter, the message box, the debug memory manager.
// Addresses are for the 2004-01-19 build (timestamp 0x400BA40A).
#pragma once
#include "../callconv.hpp"

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstdint>

#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../runtime/patch.hpp"
#include "mem.hpp"
#include "misc.hpp"

namespace joan::orig {

// --- Error (d:\ww\ilib\err.cpp-equivalent), global `err` --------------------
// Error::run(const char* fmt, ...) at 0x00586cb0 is a variadic member, which
// MSVC compiles as __cdecl with `this` pushed first. It shows a message box and
// calls exit(-2); a re-entrancy flag (0x00a64528) makes nested calls return.
/// The global error object `err` (d::g_err), the `this` of Error::run.
inline constexpr img::Addr kErr = d::g_err;
/// Signature of Error::run: a variadic member compiled as __cdecl with `this` pushed first.
using ErrRunFn = void(JOAN_CDECL*)(void* self, const char* fmt, ...);
template <class... Args>
/// Calls Error::run (0x00586cb0) through its replacement: shows the message box and ends the process, as the original does.
void err_run(const char* fmt, Args... args) {
    hook::original<ErrRunFn>(fn::Error_fatal_body)(img::obj(kErr), fmt, args...);
}

// --- Box, global `box`: int Game::message_box(const char* text, void* owner, u32 flags), 0x005a8be0 (the replacement
// in game/unknown_msgbox.cpp: owner null = the game window; the same parameter types, so x64 passes them alike)
/// The message-box helper object (global `box`, d::g_box): shows a modal text box and returns the button chosen.
struct Box {
    /// Shows `msg` in a message box (Game::message_box 0x005a8be0). `owner` null = the game window; `flags` are the
    /// MessageBox style bits the callers pass. Returns the button id of the original call.
    int ask(const char* msg, void* owner = nullptr, std::uint32_t flags = 0) {
        using Fn = int (Box::*)(const char*, void*, std::uint32_t);
        return (this->*hook::original<Fn>(fn::Game_message_box_body))(msg, owner, flags);
    }

    // The other Box functions the game code calls (typed, through their slots).
#include "gen/box_methods.inc"
};
/// The global message-box object `box` (d::g_box).
inline Box& box() { return hook::global<Box>(d::g_box); }

// --- Mem, global `mem`: debug allocator taking __FILE__/__LINE__ (mem.hpp) --

// --- Misc, global `misc` (layout and members in misc.hpp) ---------------
/// The global helper object `misc` (d::g_misc).
inline Misc& misc() { return hook::global<Misc>(d::g_misc); }

// --- statically linked MSVC 6 CRT ------------------------------------------
// The game's own qsort is used where element order must match the original
// exactly (its pivot choice decides the order of equal keys).
/// Comparison function of qsort.
using QsortCmp = int(JOAN_CDECL*)(const void*, const void*);
/// Signature of the game's CRT qsort (0x007b2d55).
using QsortFn = void(JOAN_CDECL*)(void* base, std::uint32_t num, std::uint32_t width, QsortCmp cmp);
/// qsort of the game's CRT (0x007b2d55: the original's algorithm, so that equal keys end up in its order).
inline void crt_qsort(void* base, std::uint32_t num, std::uint32_t width, QsortCmp cmp) {
    hook::original<QsortFn>(fn::crt_qsort)(base, num, width, cmp);
}

// Shared read buffer used by ResourceIdx objects created with use_common_buf.
/// The shared read buffer of ResourceIdx objects created with use_common_buf (d::g_common_res_buf).
inline char*& common_res_buf() { return hook::global<char*>(d::g_common_res_buf); }

}  // namespace joan::orig
