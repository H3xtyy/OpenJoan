// [Debug] Profile=N: a sampling profiler of the game's main thread. A background thread stops the
// thread about 1000 times a second, records its call stack (dbghelp's StackWalk64 with openjoan.pdb)
// and every N seconds writes to openjoan.log the functions the samples were in: "self" (the
// innermost function) and "total" (anywhere on the stack). Functions without symbols (graphics
// driver, system DLLs) are counted per module. Off (0) by default; costs a few % while on.
#pragma once

#include <cstdint>
#include <string>

/// [Debug] Profile: sampling profiler of the game's main thread.
namespace joan::prof {

/// Diagnostics: the function at code address `pc` with its offset ("name+0x12", from openjoan.pdb through
/// dbghelp, loaded on first use), or the module's name ("[module]+0x..."); "?" when unknown.
std::string symbol_at(std::uintptr_t pc);

/// Starts sampling the calling thread (the game's main thread: the Direct3D device's owner),
/// once, if [Debug] Profile is set. Safe to call more than once.
void start_for_current_thread();

}  // namespace joan::prof
