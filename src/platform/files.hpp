// The game's files without Windows' file system (phase C5 round F1, src/platform/README.md): joan.exe's KERNEL32 file
// imports - and the same calls in our own C++ (ilib/file.cpp, misc.cpp, gameset.cpp, date.cpp) - answered by
// portable code (C stdio, std::filesystem). Declared with the Win32 signatures the callers use; see files.cpp. A
// handle that is not ours (the CRT's standard handles, ...) goes to Windows unchanged.
#pragma once

#include <string>

#include "win32_types.hpp"

// A handle that is not one of ours (files.cpp, process.cpp): `call`, Windows' own function, on Windows (the C
// runtime's standard handles, ...); elsewhere there are no such handles - `failed` with ERROR_INVALID_HANDLE.
// Returns from the calling function; uses its set_error.
#if defined(_WIN32)
#define JOAN_NOT_OURS(call, failed) return call
#else
#define JOAN_NOT_OURS(call, failed)        \
    do {                                   \
        set_error(ERROR_INVALID_HANDLE);   \
        return failed;                     \
    } while (0)
#endif

/// The game's file calls answered by portable code (stdio, std::filesystem) with the Win32 signatures the callers use.
namespace joan::platform::files {

/// Runs the completion routines of the ReadFileEx calls of this thread that have not run yet (what an alertable wait
/// does). True if any ran (the wait then returns WAIT_IO_COMPLETION).
bool run_pending_io();

/// A file name of the game (Windows form: backslashes, any case) as this system names the file: on Windows itself; elsewhere
/// with slashes and each existing part matched ignoring case. For code that opens files without these functions (FFmpeg).
std::string native_path(const char* name);

/// [Platform] FilesInMemory: starts reading the item, particle and item-script folders into memory (host start, after
/// the settings); the game's read-only opens of those files are served from memory once they are in.
void start_memory_files();

// platform/process.cpp: its event / thread handles (CloseHandle, the waits here dispatch to them).
/// Closes `h` if it is one of process.cpp's objects.
bool sync_close(HANDLE h);
/// Waits for process.cpp's objects; false (nothing done) if any handle is not one of them.
bool sync_wait(DWORD n, const HANDLE* hs, bool all, DWORD ms, bool alertable, DWORD& result);

/// The game's KERNEL32 CreateFileA, answered by portable code for the game's own files (Win32 signature, see files.cpp).
HANDLE WINAPI CreateFileA(LPCSTR name, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES sa, DWORD disposition,
                          DWORD flags, HANDLE templ);
/// The game's KERNEL32 CloseHandle for a handle of this namespace.
BOOL WINAPI CloseHandle(HANDLE h);
/// The game's KERNEL32 ReadFile (Win32 signature, see files.cpp).
BOOL WINAPI ReadFile(HANDLE h, LPVOID buf, DWORD n, LPDWORD got, LPOVERLAPPED ov);
/// The game's KERNEL32 WriteFile (Win32 signature, see files.cpp).
BOOL WINAPI WriteFile(HANDLE h, LPCVOID buf, DWORD n, LPDWORD put, LPOVERLAPPED ov);
/// The game's KERNEL32 SetFilePointer (Win32 signature, see files.cpp).
DWORD WINAPI SetFilePointer(HANDLE h, LONG distance, PLONG high, DWORD method);
/// The game's KERNEL32 GetFileSize (Win32 signature, see files.cpp).
DWORD WINAPI GetFileSize(HANDLE h, LPDWORD high);
/// The game's KERNEL32 FindFirstFileA (Win32 signature, see files.cpp).
HANDLE WINAPI FindFirstFileA(LPCSTR pattern, LPWIN32_FIND_DATAA data);
/// The game's KERNEL32 FindNextFileA (Win32 signature, see files.cpp).
BOOL WINAPI FindNextFileA(HANDLE h, LPWIN32_FIND_DATAA data);
/// The game's KERNEL32 FindClose (Win32 signature, see files.cpp).
BOOL WINAPI FindClose(HANDLE h);
/// The game's KERNEL32 DeleteFileA (Win32 signature, see files.cpp).
BOOL WINAPI DeleteFileA(LPCSTR name);
/// The game's KERNEL32 FileTimeToLocalFileTime (Win32 signature, see files.cpp).
BOOL WINAPI FileTimeToLocalFileTime(const FILETIME* in, LPFILETIME out);
/// The game's KERNEL32 FileTimeToSystemTime (Win32 signature, see files.cpp).
BOOL WINAPI FileTimeToSystemTime(const FILETIME* in, LPSYSTEMTIME out);
/// The game's KERNEL32 CompareFileTime (Win32 signature, see files.cpp).
LONG WINAPI CompareFileTime(const FILETIME* a, const FILETIME* b);

}  // namespace joan::platform::files
