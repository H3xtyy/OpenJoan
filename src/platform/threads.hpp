// Threads for our own code (process.cpp, which also answers joan.exe's thread / event imports with std::thread):
// what the C runtime's thread family and our diagnostics need besides std::thread / std::mutex.
#pragma once

#include <cstdint>

namespace joan::platform {

/// This thread's id (the one GetCurrentThreadId reports to the game).
unsigned long thread_id();

/// Ends the calling thread with `code` without returning (ExitThread / _endthread). A thread of ours (CreateThread
/// of process.cpp) unwinds back to its start and finishes there - waits on it end, its exit code is `code`; any other
/// thread ends through the system.
[[noreturn]] void exit_thread(unsigned code);

/// The calling thread's last error (what GetLastError reports to the game).
void set_last_error(unsigned long code);
/// The calling thread's last-error value (see set_last_error).
unsigned long last_error();

/// The calling thread's stack: [low, high) of what is in use or committed (the frame-chain walks of our
/// diagnostics stay inside it). False if the system does not tell.
bool stack_range(std::uintptr_t& low, std::uintptr_t& high);

}  // namespace joan::platform
