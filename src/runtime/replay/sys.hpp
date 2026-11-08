// SPDX-License-Identifier: GPL-3.0-or-later
// Record / replay: the system services the recorder itself needs (not the game's imports, which it wraps), behind
// one interface for every platform - Win32 on Windows (what replay.cpp called directly until Phase 1), the platform
// layer and POSIX elsewhere (sys.cpp). Nothing here is recorded: these are the recorder's own files, clocks, memory
// maps and diagnostics. Overview of the recorder: replay.hpp; design: docs/RUNTIME.md "replay" (Phase 1).
#pragma once

#include <cstdint>
#include <string>
#include <vector>

/// The system services the replay recorder itself needs (threads, files, clocks, memory maps), one interface for every platform.
namespace joan::replay::sys {

// ---------------------------------------------------------------- threads, errors
/// The calling thread's id (what the game's GetCurrentThreadId reports).
unsigned long thread_id();
/// The game's last-error value of the calling thread (GetLastError / SetLastError as the game sees them): the
/// recorder's own system calls must not change it.
unsigned long last_error();
/// Sets the last-error value of the calling thread.
void set_last_error(unsigned long e);
/// Keeps the last-error value over a block of recorder code.
class LastErrorGuard {
   public:
    LastErrorGuard() : e_(last_error()) {}
    ~LastErrorGuard() { set_last_error(e_); }
    LastErrorGuard(const LastErrorGuard&) = delete;
    LastErrorGuard& operator=(const LastErrorGuard&) = delete;

   private:
    unsigned long e_;
};

// ---------------------------------------------------------------- the process
/// The command line the game sees (for --record FILE / --replay FILE).
std::string command_line();
/// A stamp of this executable's build (information only, stored in recordings): the PE timestamp on Windows, 0
/// elsewhere (an ELF has none; the loader log names the build).
std::uint32_t exe_stamp();
/// The address range of our own executable's image.
void exe_range(std::uintptr_t& low, std::uintptr_t& high);
/// Ends the process now, without exit handlers.
[[noreturn]] void end_process(unsigned code);

// ---------------------------------------------------------------- memory
/// A committed region of the address space, as the state hash classifies pointers: [begin, end), the start of the
/// allocation it belongs to, whether reading it cannot fault, whether it is an image (the executable, a DLL / .so).
struct MemRegion {
    std::uintptr_t begin /**< first address */, end /**< one past the last address */,
        allocation_base /**< start of the allocation the region belongs to */;
    bool readable /**< reading it cannot fault */, image /**< part of an executable / shared-library image */;
};
/// Every committed region in address order (VirtualQuery walk / /proc/self/maps).
void committed_regions(std::vector<MemRegion>& out);
/// Reads 4 bytes at `a` unless that faults (SEH on Windows; elsewhere the mapping is checked first).
bool read_u32(std::uintptr_t a, std::uint32_t* out);

// ---------------------------------------------------------------- clocks
/// A monotonic millisecond clock of the recorder's own (not the game's: the game's clock is recorded).
unsigned long long tick_ms();
/// Sleeps the calling thread for `ms` milliseconds.
void sleep_ms(unsigned ms);
/// "YYYYMMDD-HHMMSS" of the local time.
std::string local_time_stamp();

// ---------------------------------------------------------------- files (paths of the game folder: '\\' as the
// game writes them is accepted everywhere; on case-sensitive file systems the parts are matched ignoring case as
// platform/files.cpp does for the game)
/// The native path of a game-folder path (case resolved where the file system is case-sensitive).
std::string native(const std::string& path);
/// Whether `path` (a game-folder path) is a regular file.
bool is_plain_file(const std::string& path);
/// Whether `path` (a game-folder path) exists.
bool exists(const std::string& path);
/// FNV-1a of the whole file; *size = its length; 0 when unreadable.
std::uint64_t hash_file(const std::string& path, std::uint64_t* size);
/// Size plus the first and last 64 KB (big archives / movies without reading them whole).
std::uint64_t quick_hash_file(const std::string& path, std::uint64_t* size);
/// Reads the whole file into `out`; false when it cannot be read.
bool read_whole(const std::string& path, std::string& out);
/// Creates the missing directories of the path, then writes the file.
bool write_whole(const std::string& path, const std::string& bytes);
/// Files below root + rel (rel uses '\\' separators, as stored in recordings), at most 4 levels deep: "rel\\name".
void list_files(const std::string& root, const std::string& rel, int depth, std::vector<std::string>& out);
/// Creates the directory `path` (a game-folder path); false on failure.
bool make_dir(const std::string& path);
/// Renames a file or folder; on failure *error (when given) gets the system's text.
bool move_path(const std::string& from, const std::string& to, std::string* error = nullptr);

// ---------------------------------------------------------------- diagnostics
/// "module+offset  function+displacement" of a code address (dbghelp with openjoan.pdb on Windows, dladdr elsewhere).
std::string describe_frame(std::uintptr_t a);
/// Return addresses of the calling thread's stack, skipping the innermost `skip`; returns the count written.
int capture_stack(unsigned skip, void** frames, int n);
/// Tells the user that a replay stopped (a message box on Windows, the log elsewhere); the game window (when given)
/// is lowered first so the box is visible.
void replay_stopped_notice(const std::string& text, void* window);
/// Hardware write watchpoints ([Debug] ReplayWatch) and the CRT heap hash ([Debug] ReplayHashHeap) are Windows-only
/// diagnostics; these say so for the log.
bool watch_supported();
/// Whether the CRT heap hash ([Debug] ReplayHashHeap) is available on this platform.
bool heap_hash_supported();

}  // namespace joan::replay::sys
