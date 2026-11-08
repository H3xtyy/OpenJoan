// Record / replay of a game session at the platform boundary, with per-frame state hashes (docs/RUNTIME.md
// "Replay-identical"): the verification step for changes that alter code (exact ports -> idiomatic C++).
//
// Every nondeterministic input reaches the game through joan.exe's import table (the IAT inside joan_image) or
// through a COM object one of those imports created. install() overwrites the relevant IAT slots with wrappers
// (imports.cpp) after the loader and the hook resolved them; the COM objects get their methods wrapped on creation
// (com.cpp: vtable patches of the Direct3D 8 objects). On every platform: the slots hold the platform layer's own
// functions where the original's DLLs are not loaded (Linux: all of them), and the recorder's own system needs
// (files, memory maps, clocks, symbol names) are behind sys.hpp.
//   Record ([Debug] Record=file): each wrapper calls the real function and writes its results (return value, out
//     parameters) to the recording, together with a check value of its arguments. Window messages the game's
//     WndProc reacts to and ReadFileEx completions are written where they happened. Every Present writes a frame
//     event with hashes of the game state.
//   Replay ([Debug] Replay=file): each wrapper returns the recorded results (pure queries do not call the system at
//     all: no real input, time, sleeping or waiting), recorded messages / completions are injected at the same point
//     of the call sequence, and every Present compares the frame hashes. A call that is not the next recorded one (or
//     has other arguments) is a divergence: the report names the frame, the event number and both calls.
// Reports: openjoan.log ("replay:" lines) and <recording>.report.txt; exit code of a replay 0 = identical,
// 3 = divergence, 4 = state hash mismatches, 5 = the recording could not be used.
#pragma once

/// Record / replay of a game session at the platform boundary, with per-frame state hashes.
namespace joan::replay {

/// Reads [Debug] Record / Replay (and the command line options --record FILE / --replay FILE) and, if one is set,
/// installs the wrappers. Call once on the game thread from host::start, after every other IAT / code slot change
/// (replacements, fixes, the Direct3D hook) - the wrappers call whatever the slots held.
void install();

/// The game's rdtsc (read_tsc 0x0045e750): recorded on the main thread while recording, the recorded value on
/// replay, `real` otherwise. rdtsc is not an import, but it is an input: UnitManager::update_units 0x0051e280 sets
/// how many units it updates per pass from cycle-counter time, and CpuUsage::init 0x0045c040 calibrates the rate.
unsigned long long tsc(unsigned long long real);
/// The processor's time-stamp counter (rdtsc), unrecorded; tsc(cpu_tsc()) is the game's read_tsc.
unsigned long long cpu_tsc();

/// The native keyboard / mouse of the platform layer (platform/input.hpp, [Platform] Input=sdl): recorded like the
/// DirectInput calls they replace (Id::IN_*). `real` reads the real device (not called on replay); for Read, `items`
/// holds up to *count items of sizeof(platform::input::Item) bytes and *count is the number delivered.
/// PadMask / PadRead (game pads): `items` is the result buffer and *count its size in bytes.
enum class InputOp : int { Acquire = 0, Unacquire = 1, Read = 2, PadMask = 3, PadRead = 4 };
/// A real read of the native input device: `device` is the device number, `items` and `count` as for input_call().
using InputFn = int (*)(int device, void* items, unsigned* count);
/// Records or replays one call of the native input layer (`op` on `device`; `real` performs it when recording or when no replay is active); returns the call's result.
int input_call(InputOp op, int device, void* items, unsigned* count, InputFn real);

/// True while recording or replaying ([Debug] ReplayDetHeap=1, default): the game's CRT heap is then joan::det_heap.
bool wants_deterministic_heap();
/// True while recording or replaying.
bool active();
/// True while replaying with [Debug] ReplayPace=0: the frame limiter ([Graphics] MaxFps) does not hold frames back
/// then (a replay's results come from the recording's clock, not the wall clock).
bool unpaced();
/// True while recording, or replaying a recording made with kFlagStableGroundOrder: Cell::build_ground (0x0055b720)
/// then orders its tiles by the owner sector's index in the cell instead of the sector's heap address (the order the
/// original's qsort comparator 0x0055f2c0 gives depends on where the heap put the sectors, which differs on x64).
bool stable_ground_order();

/// "module+offset  function+displacement" of a code address (dbghelp names when openjoan.pdb is next to the exe),
/// written to `out` (diagnostics).
void describe_code(unsigned long address, char* out, unsigned size);

}  // namespace joan::replay
