// The Windows types, structures and constants of our code: the Win32 signatures of joan.exe's imports we answer
// (platform/, lib/crt), the game's own uses (HWND, RECT, DWORD ...). Windows' <windows.h> on Windows; elsewhere our
// own definitions with Windows' sizes and layouts (LONG and DWORD 32 bits, WCHAR 16 bits, handles pointers), the
// constants' values and the functions' declarations taken from the Windows SDK headers. Only names our code uses are
// here; JOAN_WIN32_OWN_TYPES is defined when these are ours.
//
// The check on Windows: compile the sources with JOAN_OWN_WIN_TYPES defined (MSVC, syntax only: /Zs), which takes our
// definitions instead of <windows.h> - any error outside an `#if defined(_WIN32)` block is a name still missing here.
#pragma once

#if defined(_WIN32) && !defined(JOAN_OWN_WIN_TYPES)

#include <windows.h>

#else

#define JOAN_WIN32_OWN_TYPES 1  // our definitions below, not Windows' headers

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "../callconv.hpp"

// ---------------------------------------------------------------- keywords
#define WINAPI JOAN_STDCALL
#define CALLBACK JOAN_STDCALL
#define APIENTRY JOAN_STDCALL
#define WINAPIV JOAN_CDECL
#define NTAPI JOAN_STDCALL
#define CONST const
#define VOID void
#define FAR
#define NEAR

// ---------------------------------------------------------------- integer types
using BYTE = std::uint8_t;
using WORD = std::uint16_t;
using BOOL = int;
using BOOLEAN = BYTE;
using CHAR = char;
using UCHAR = unsigned char;
using SHORT = std::int16_t;
using USHORT = std::uint16_t;
using INT = int;
using UINT = unsigned int;
// 32 bits like Windows' long (`long` itself where it is 32 bits, so the types match Windows' there).
using LONG = std::conditional_t<sizeof(long) == 4, long, std::int32_t>;
using ULONG = std::conditional_t<sizeof(long) == 4, unsigned long, std::uint32_t>;
using DWORD = ULONG;
using LONGLONG = std::int64_t;
using ULONGLONG = std::uint64_t;
using DWORD64 = std::uint64_t;
using FLOAT = float;
// 16-bit UTF-16 units like Windows' wchar_t: char16_t (wchar_t is 32 bits elsewhere; on Windows this check build then
// finds every place that still mixes wchar_t with WCHAR).
using WCHAR = char16_t;
using INT_PTR = std::intptr_t;
using UINT_PTR = std::uintptr_t;
using LONG_PTR = std::intptr_t;
using ULONG_PTR = std::uintptr_t;
using DWORD_PTR = ULONG_PTR;
using SIZE_T = std::size_t;
using WPARAM = UINT_PTR;
using LPARAM = LONG_PTR;
using LRESULT = LONG_PTR;
using HRESULT = LONG;
using ATOM = WORD;
using COLORREF = DWORD;
using LCID = DWORD;
using LCTYPE = DWORD;
using LANGID = WORD;

// ---------------------------------------------------------------- pointers
using PVOID = void*;
using LPVOID = void*;
using LPCVOID = const void*;
using LPSTR = char*;
using PSTR = char*;
using LPCSTR = const char*;
using PCSTR = const char*;
using LPWSTR = WCHAR*;
using PWSTR = WCHAR*;
using LPCWSTR = const WCHAR*;
using PCWSTR = const WCHAR*;
using LPCH = char*;
using LPWCH = WCHAR*;
using LPBYTE = BYTE*;
using PBYTE = BYTE*;
using LPWORD = WORD*;
using PWORD = WORD*;
using LPDWORD = DWORD*;
using PDWORD = DWORD*;
using LPLONG = LONG*;
using PLONG = LONG*;
using PULONG = ULONG*;
using LPBOOL = BOOL*;
using PBOOL = BOOL*;
using LPINT = int*;
using PUINT = UINT*;
using PSIZE_T = SIZE_T*;

// ---------------------------------------------------------------- handles (distinct types, as Windows' STRICT)
using HANDLE = void*;
using PHANDLE = HANDLE*;
using LPHANDLE = HANDLE*;
#define JOAN_WIN32_HANDLE(name) \
    struct name##__ {           \
        int unused;             \
    };                          \
    using name = name##__*
JOAN_WIN32_HANDLE(HWND);
JOAN_WIN32_HANDLE(HINSTANCE);
JOAN_WIN32_HANDLE(HDC);
JOAN_WIN32_HANDLE(HGLRC);
JOAN_WIN32_HANDLE(HICON);
JOAN_WIN32_HANDLE(HBRUSH);
JOAN_WIN32_HANDLE(HMENU);
JOAN_WIN32_HANDLE(HKEY);
JOAN_WIN32_HANDLE(HMONITOR);
JOAN_WIN32_HANDLE(HBITMAP);
JOAN_WIN32_HANDLE(HFONT);
JOAN_WIN32_HANDLE(HRGN);
JOAN_WIN32_HANDLE(HPALETTE);
JOAN_WIN32_HANDLE(HRSRC);
JOAN_WIN32_HANDLE(DPI_AWARENESS_CONTEXT);
#undef JOAN_WIN32_HANDLE
using HMODULE = HINSTANCE;
using HCURSOR = HICON;
using HGDIOBJ = void*;
using HGLOBAL = HANDLE;
using HLOCAL = HANDLE;
using FARPROC = INT_PTR(WINAPI*)();
using WNDPROC = LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM);

// ---------------------------------------------------------------- structures
/// RECT: a rectangle in integer coordinates. Win32 type; fields as the SDK header.
struct tagRECT {
    LONG left, top, right, bottom;  ///< Documented below.
};
/** @var tagRECT::left
 * Left edge (x of the top-left corner). */
/** @var tagRECT::top
 * Top edge (y of the top-left corner). */
/** @var tagRECT::right
 * Right edge (exclusive). */
/** @var tagRECT::bottom
 * Bottom edge (exclusive). */
using RECT = tagRECT;
using LPRECT = RECT*;
using LPCRECT = const RECT*;
/// POINT: an integer point. Win32 type; fields as the SDK header.
struct tagPOINT {
    LONG x, y;  ///< Documented below.
};
/** @var tagPOINT::x
 * Horizontal coordinate. */
using POINT = tagPOINT;
using LPPOINT = POINT*;
/// SIZE: an integer extent. Win32 type; fields as the SDK header.
struct tagSIZE {
    LONG cx, cy;  ///< Documented below.
};
/** @var tagSIZE::cx
 * Width. */
using SIZE = tagSIZE;

// Anonymous structs inside unions, as the Windows headers declare them (LARGE_INTEGER, OVERLAPPED, SYSTEM_INFO ...): a
// GCC / Clang extension in C++ (__extension__ keeps -Wpedantic quiet), standard in MSVC.
#if defined(__GNUC__)
#define JOAN_ANON_STRUCT __extension__ struct
#else
#define JOAN_ANON_STRUCT struct
#endif
// Clang also reports an anonymous struct inside an anonymous union (OVERLAPPED, SYSTEM_INFO) separately.
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

/// LARGE_INTEGER: a signed 64-bit value, whole or as two halves. Win32 type; fields as the SDK header.
union LARGE_INTEGER {
    JOAN_ANON_STRUCT {
        DWORD LowPart;  ///< Low 32 bits.
        LONG HighPart;  ///< High 32 bits.
    };
    JOAN_ANON_STRUCT {
        DWORD LowPart;  ///< Low 32 bits.
        LONG HighPart;  ///< High 32 bits.
    } u;  ///< The same two parts under a name.
    LONGLONG QuadPart;  ///< The whole 64-bit value.
};
/// ULARGE_INTEGER: an unsigned 64-bit value, whole or as two halves. Win32 type; fields as the SDK header.
union ULARGE_INTEGER {
    JOAN_ANON_STRUCT {
        DWORD LowPart;  ///< Low 32 bits.
        DWORD HighPart;  ///< High 32 bits.
    };
    JOAN_ANON_STRUCT {
        DWORD LowPart;  ///< Low 32 bits.
        DWORD HighPart;  ///< High 32 bits.
    } u;  ///< The same two parts under a name.
    ULONGLONG QuadPart;  ///< The whole 64-bit value.
};
/// FILETIME: a time as 100 ns ticks since 1601-01-01 UTC. Win32 type; fields as the SDK header.
struct _FILETIME {
    DWORD dwLowDateTime;  ///< Low 32 bits of the 100 ns tick count since 1601-01-01.
    DWORD dwHighDateTime;  ///< High 32 bits of the tick count.
};
using FILETIME = _FILETIME;
using LPFILETIME = FILETIME*;
/// SYSTEMTIME: a calendar date and time. Win32 type; fields as the SDK header.
struct _SYSTEMTIME {
    WORD wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMilliseconds;  ///< Documented below.
};
/** @var _SYSTEMTIME::wYear
 * Year. */
/** @var _SYSTEMTIME::wMonth
 * Month, 1..12. */
/** @var _SYSTEMTIME::wDayOfWeek
 * Day of the week, 0 = Sunday. */
/** @var _SYSTEMTIME::wDay
 * Day of the month, 1..31. */
/** @var _SYSTEMTIME::wHour
 * Hour, 0..23. */
/** @var _SYSTEMTIME::wMinute
 * Minute, 0..59. */
/** @var _SYSTEMTIME::wSecond
 * Second, 0..59. */
using SYSTEMTIME = _SYSTEMTIME;
using LPSYSTEMTIME = SYSTEMTIME*;
#define MAX_PATH 260
/// WIN32_FIND_DATAA: one directory entry of FindFirstFileA / FindNextFileA. Win32 type; fields as the SDK header.
struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;  ///< FILE_ATTRIBUTE_* bits.
    FILETIME ftCreationTime;  ///< Creation time.
    FILETIME ftLastAccessTime;  ///< Last access time.
    FILETIME ftLastWriteTime;  ///< Last write time.
    DWORD nFileSizeHigh;  ///< High 32 bits of the file size.
    DWORD nFileSizeLow;  ///< Low 32 bits of the file size.
    DWORD dwReserved0;  ///< Reserved by the system.
    DWORD dwReserved1;  ///< Reserved by the system.
    CHAR cFileName[MAX_PATH];  ///< File name (NUL-terminated, ANSI code page).
    CHAR cAlternateFileName[14];  ///< 8.3 short name.
};
using WIN32_FIND_DATAA = _WIN32_FIND_DATAA;
using LPWIN32_FIND_DATAA = WIN32_FIND_DATAA*;
/// OVERLAPPED: state of an asynchronous file operation. Win32 type; fields as the SDK header.
struct _OVERLAPPED {
    ULONG_PTR Internal;  ///< Reserved for the system.
    ULONG_PTR InternalHigh;  ///< Reserved for the system.
    union {
        JOAN_ANON_STRUCT {
            DWORD Offset;  ///< Low 32 bits of the file position.
            DWORD OffsetHigh;  ///< High 32 bits of the file position.
        };
        PVOID Pointer;  ///< Reserved.
    };
    HANDLE hEvent;  ///< Event signalled on completion.
};
using OVERLAPPED = _OVERLAPPED;
using LPOVERLAPPED = OVERLAPPED*;
using LPOVERLAPPED_COMPLETION_ROUTINE = void(WINAPI*)(DWORD error, DWORD transferred, LPOVERLAPPED ov);
/// SECURITY_ATTRIBUTES: handle inheritance and security. Win32 type; fields as the SDK header.
struct _SECURITY_ATTRIBUTES {
    DWORD nLength;  ///< Size of the structure in bytes.
    LPVOID lpSecurityDescriptor;  ///< Security descriptor, or null.
    BOOL bInheritHandle;  ///< Whether child processes inherit the handle.
};
using SECURITY_ATTRIBUTES = _SECURITY_ATTRIBUTES;
using LPSECURITY_ATTRIBUTES = SECURITY_ATTRIBUTES*;
/// MSG: a window message as GetMessage returns it. Win32 type; fields as the SDK header.
struct tagMSG {
    HWND hwnd;  ///< Window the message is for.
    UINT message;  ///< Message id (WM_*).
    WPARAM wParam;  ///< First message parameter.
    LPARAM lParam;  ///< Second message parameter.
    DWORD time;  ///< Time the message was posted (ms).
    POINT pt;  ///< Cursor position when posted.
};
using MSG = tagMSG;
using LPMSG = MSG*;
/// GUID: a 128-bit identifier. Win32 type; fields as the SDK header.
struct _GUID {
    DWORD Data1;  ///< First 32 bits.
    WORD Data2;  ///< Next 16 bits.
    WORD Data3;  ///< Next 16 bits.
    BYTE Data4[8];  ///< Last 8 bytes.
};
using GUID = _GUID;
using IID = GUID;
using REFIID = const IID&;
using REFGUID = const GUID&;

using LPSIZE = SIZE*;
using LPCCH = const CHAR*;
using PCNZCH = const CHAR*;
using LPCWCH = const WCHAR*;
using PCNZWCH = const WCHAR*;

// kernel32
/// CRITICAL_SECTION: the layout of the Windows lock. Win32 type; fields as the SDK header.
struct _RTL_CRITICAL_SECTION {
    PVOID DebugInfo;  ///< Debug record, or null.
    LONG LockCount;  ///< Lock state counter.
    LONG RecursionCount;  ///< Times the owner has entered.
    HANDLE OwningThread;  ///< Id of the owning thread.
    HANDLE LockSemaphore;  ///< Wait object for contenders.
    ULONG_PTR SpinCount;  ///< Spin count before waiting.
};
using CRITICAL_SECTION = _RTL_CRITICAL_SECTION;
using LPCRITICAL_SECTION = CRITICAL_SECTION*;
using LPTHREAD_START_ROUTINE = DWORD(WINAPI*)(LPVOID parameter);
using PHANDLER_ROUTINE = BOOL(WINAPI*)(DWORD ctrl_type);
using LOCALE_ENUMPROCA = BOOL(CALLBACK*)(LPSTR locale);
/// CPINFO: code page description of GetCPInfo. Win32 type; fields as the SDK header.
struct _cpinfo {
    UINT MaxCharSize;  ///< Maximum bytes per character (1 or 2).
    BYTE DefaultChar[2];  ///< Default character for unmappable input.
    BYTE LeadByte[12];  ///< Lead-byte ranges, pairs terminated by 0,0.
};
using CPINFO = _cpinfo;
using LPCPINFO = CPINFO*;
/// OSVERSIONINFOA: GetVersionExA result. Win32 type; fields as the SDK header.
struct _OSVERSIONINFOA {
    DWORD dwOSVersionInfoSize;  ///< Size of the structure in bytes.
    DWORD dwMajorVersion;  ///< Major version.
    DWORD dwMinorVersion;  ///< Minor version.
    DWORD dwBuildNumber;  ///< Build number.
    DWORD dwPlatformId;  ///< VER_PLATFORM_* id.
    CHAR szCSDVersion[128];  ///< Service pack text.
};
using OSVERSIONINFOA = _OSVERSIONINFOA;
using LPOSVERSIONINFOA = OSVERSIONINFOA*;
/// OSVERSIONINFOEXA: GetVersionExA result with service pack data. Win32 type; fields as the SDK header.
struct _OSVERSIONINFOEXA {
    DWORD dwOSVersionInfoSize;  ///< Size of the structure in bytes.
    DWORD dwMajorVersion;  ///< Major version.
    DWORD dwMinorVersion;  ///< Minor version.
    DWORD dwBuildNumber;  ///< Build number.
    DWORD dwPlatformId;  ///< VER_PLATFORM_* id.
    CHAR szCSDVersion[128];  ///< Service pack text.
    WORD wServicePackMajor;  ///< Service pack major version.
    WORD wServicePackMinor;  ///< Service pack minor version.
    WORD wSuiteMask;  ///< VER_SUITE_* bits.
    BYTE wProductType;  ///< VER_NT_* product type.
    BYTE wReserved;  ///< Reserved.
};
using OSVERSIONINFOEXA = _OSVERSIONINFOEXA;
/// MEMORYSTATUS: GlobalMemoryStatus result. Win32 type; fields as the SDK header.
struct _MEMORYSTATUS {
    DWORD dwLength;  ///< Size of the structure in bytes.
    DWORD dwMemoryLoad;  ///< Percentage of memory in use.
    SIZE_T dwTotalPhys;  ///< Physical memory in bytes.
    SIZE_T dwAvailPhys;  ///< Free physical memory in bytes.
    SIZE_T dwTotalPageFile;  ///< Commit limit in bytes.
    SIZE_T dwAvailPageFile;  ///< Free commit in bytes.
    SIZE_T dwTotalVirtual;  ///< User address space in bytes.
    SIZE_T dwAvailVirtual;  ///< Free user address space in bytes.
};
using MEMORYSTATUS = _MEMORYSTATUS;
using LPMEMORYSTATUS = MEMORYSTATUS*;
/// STARTUPINFOA: GetStartupInfoA result. Win32 type; fields as the SDK header.
struct _STARTUPINFOA {
    DWORD cb;  ///< Size of the structure in bytes.
    LPSTR lpReserved;  ///< Reserved.
    LPSTR lpDesktop;  ///< Desktop name.
    LPSTR lpTitle;  ///< Console window title.
    DWORD dwX;  ///< Window x (STARTF_USEPOSITION).
    DWORD dwY;  ///< Window y.
    DWORD dwXSize;  ///< Window width.
    DWORD dwYSize;  ///< Window height.
    DWORD dwXCountChars;  ///< Console width in characters.
    DWORD dwYCountChars;  ///< Console height in characters.
    DWORD dwFillAttribute;  ///< Console fill attribute.
    DWORD dwFlags;  ///< STARTF_* bits (MONITORINFO: MONITORINFOF_PRIMARY).
    WORD wShowWindow;  ///< SW_* show command.
    WORD cbReserved2;  ///< Reserved.
    LPBYTE lpReserved2;  ///< Reserved.
    HANDLE hStdInput;  ///< Standard input handle.
    HANDLE hStdOutput;  ///< Standard output handle.
    HANDLE hStdError;  ///< Standard error handle.
};
using STARTUPINFOA = _STARTUPINFOA;
using LPSTARTUPINFOA = STARTUPINFOA*;
/// SYSTEM_INFO: GetSystemInfo result. Win32 type; fields as the SDK header.
struct _SYSTEM_INFO {
    union {
        DWORD dwOemId;  ///< Obsolete OEM id.
        JOAN_ANON_STRUCT {
            WORD wProcessorArchitecture;  ///< PROCESSOR_ARCHITECTURE_*.
            WORD wReserved;  ///< Reserved.
        };
    };
    DWORD dwPageSize;  ///< Page size in bytes.
    LPVOID lpMinimumApplicationAddress;  ///< Lowest usable address.
    LPVOID lpMaximumApplicationAddress;  ///< Highest usable address.
    DWORD_PTR dwActiveProcessorMask;  ///< Bit mask of the active processors.
    DWORD dwNumberOfProcessors;  ///< Logical processor count.
    DWORD dwProcessorType;  ///< Obsolete processor type.
    DWORD dwAllocationGranularity;  ///< Granularity of VirtualAlloc reservations in bytes.
    WORD wProcessorLevel;  ///< Architecture-dependent level.
    WORD wProcessorRevision;  ///< Architecture-dependent revision.
};
using SYSTEM_INFO = _SYSTEM_INFO;
using LPSYSTEM_INFO = SYSTEM_INFO*;
/// TIME_ZONE_INFORMATION: GetTimeZoneInformation result. Win32 type; fields as the SDK header.
struct _TIME_ZONE_INFORMATION {
    LONG Bias;  ///< Minutes to add to local time to get UTC.
    WCHAR StandardName[32];  ///< Standard-time name (UTF-16).
    SYSTEMTIME StandardDate;  ///< When standard time starts.
    LONG StandardBias;  ///< Extra bias in standard time.
    WCHAR DaylightName[32];  ///< Daylight-time name (UTF-16).
    SYSTEMTIME DaylightDate;  ///< When daylight time starts.
    LONG DaylightBias;  ///< Extra bias in daylight time.
};
using TIME_ZONE_INFORMATION = _TIME_ZONE_INFORMATION;
using LPTIME_ZONE_INFORMATION = TIME_ZONE_INFORMATION*;
/// EXCEPTION_RECORD: description of one exception. Win32 type; fields as the SDK header.
struct _EXCEPTION_RECORD {
    DWORD ExceptionCode;  ///< EXCEPTION_* / NTSTATUS code.
    DWORD ExceptionFlags;  ///< Continuability flags.
    _EXCEPTION_RECORD* ExceptionRecord;  ///< Next record of a nested exception (EXCEPTION_POINTERS: the exception).
    PVOID ExceptionAddress;  ///< Address of the faulting instruction.
    DWORD NumberParameters;  ///< Valid entries of ExceptionInformation.
    ULONG_PTR ExceptionInformation[15];  ///< Code-specific data (access violation: [0] = access kind, [1] = address).
};
using EXCEPTION_RECORD = _EXCEPTION_RECORD;
using PEXCEPTION_RECORD = EXCEPTION_RECORD*;
struct _CONTEXT;  // the processor state: not ours to read outside Windows
using CONTEXT = _CONTEXT;
using PCONTEXT = CONTEXT*;
/// EXCEPTION_POINTERS: what an exception filter receives. Win32 type; fields as the SDK header.
struct _EXCEPTION_POINTERS {
    PEXCEPTION_RECORD ExceptionRecord;  ///< Next record of a nested exception (EXCEPTION_POINTERS: the exception).
    PCONTEXT ContextRecord;  ///< Processor state at the exception (opaque here).
};
using EXCEPTION_POINTERS = _EXCEPTION_POINTERS;
using PEXCEPTION_POINTERS = EXCEPTION_POINTERS*;
using LPTOP_LEVEL_EXCEPTION_FILTER = LONG(WINAPI*)(EXCEPTION_POINTERS* info);
using PVECTORED_EXCEPTION_HANDLER = LONG(NTAPI*)(EXCEPTION_POINTERS* info);
/// MEMORY_BASIC_INFORMATION: VirtualQuery result. Win32 type; fields as the SDK header.
struct _MEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;  ///< Page-aligned address queried.
    PVOID AllocationBase;  ///< Base of the allocation the page belongs to.
    DWORD AllocationProtect;  ///< Protection when the allocation was made.
#if UINTPTR_MAX > 0xffffffffu
    WORD PartitionId;  ///< Memory partition (64-bit Windows only).
#endif
    SIZE_T RegionSize;  ///< Size in bytes of the run of pages with the same state.
    DWORD State;  ///< MEM_COMMIT / MEM_RESERVE / MEM_FREE.
    DWORD Protect;  ///< PAGE_* protection of the run.
    DWORD Type;  ///< MEM_IMAGE / MEM_MAPPED / MEM_PRIVATE.
};
using MEMORY_BASIC_INFORMATION = _MEMORY_BASIC_INFORMATION;
using PMEMORY_BASIC_INFORMATION = MEMORY_BASIC_INFORMATION*;

// user32 / gdi32 / comdlg32
/// BITMAPINFOHEADER: DIB header. Win32 type; fields as the SDK header.
struct tagBITMAPINFOHEADER {
    DWORD biSize;  ///< Size of the structure in bytes.
    LONG biWidth;  ///< Width in pixels.
    LONG biHeight;  ///< Height in pixels (negative = top-down).
    WORD biPlanes;  ///< Always 1.
    WORD biBitCount;  ///< Bits per pixel.
    DWORD biCompression;  ///< BI_* compression.
    DWORD biSizeImage;  ///< Image size in bytes (may be 0 for BI_RGB).
    LONG biXPelsPerMeter;  ///< Horizontal resolution.
    LONG biYPelsPerMeter;  ///< Vertical resolution.
    DWORD biClrUsed;  ///< Palette entries used (0 = all).
    DWORD biClrImportant;  ///< Palette entries that matter (0 = all).
};
using BITMAPINFOHEADER = tagBITMAPINFOHEADER;
/// RGBQUAD: a palette colour (blue, green, red order). Win32 type; fields as the SDK header.
struct tagRGBQUAD {
    BYTE rgbBlue;  ///< Blue.
    BYTE rgbGreen;  ///< Green.
    BYTE rgbRed;  ///< Red.
    BYTE rgbReserved;  ///< Zero.
};
using RGBQUAD = tagRGBQUAD;
/// BITMAPINFO: DIB header plus palette. Win32 type; fields as the SDK header.
struct tagBITMAPINFO {
    BITMAPINFOHEADER bmiHeader;  ///< Header.
    RGBQUAD bmiColors[1];  ///< Palette (variable length).
};
using BITMAPINFO = tagBITMAPINFO;
using LPBITMAPINFO = BITMAPINFO*;
#pragma pack(push, 2)
/// BITMAPFILEHEADER: header of a .bmp file (2-byte packing). Win32 type; fields as the SDK header.
struct tagBITMAPFILEHEADER {
    WORD bfType;  ///< File signature "BM".
    DWORD bfSize;  ///< File size in bytes.
    WORD bfReserved1;  ///< Zero.
    WORD bfReserved2;  ///< Zero.
    DWORD bfOffBits;  ///< Offset of the pixel data.
};
#pragma pack(pop)
using BITMAPFILEHEADER = tagBITMAPFILEHEADER;
/// PALETTEENTRY: a logical palette colour. Win32 type; fields as the SDK header.
struct tagPALETTEENTRY {
    BYTE peRed;  ///< Red.
    BYTE peGreen;  ///< Green.
    BYTE peBlue;  ///< Blue.
    BYTE peFlags;  ///< PC_* flags.
};
using PALETTEENTRY = tagPALETTEENTRY;
using LPPALETTEENTRY = PALETTEENTRY*;
/// LOGFONTA: a logical font description. Win32 type; fields as the SDK header.
struct tagLOGFONTA {
    LONG lfHeight;  ///< Character height (negative = by character rather than cell).
    LONG lfWidth;  ///< Average character width (0 = default).
    LONG lfEscapement;  ///< Text angle in tenths of a degree.
    LONG lfOrientation;  ///< Character angle in tenths of a degree.
    LONG lfWeight;  ///< FW_* weight, 0..1000.
    BYTE lfItalic;  ///< Non-zero = italic.
    BYTE lfUnderline;  ///< Non-zero = underlined.
    BYTE lfStrikeOut;  ///< Non-zero = struck out.
    BYTE lfCharSet;  ///< *_CHARSET id.
    BYTE lfOutPrecision;  ///< OUT_* precision.
    BYTE lfClipPrecision;  ///< CLIP_* precision.
    BYTE lfQuality;  ///< *_QUALITY.
    BYTE lfPitchAndFamily;  ///< Pitch and family bits.
    CHAR lfFaceName[32];  ///< Typeface name (ANSI).
};
using LOGFONTA = tagLOGFONTA;
/// ICONINFO: GetIconInfo result. Win32 type; fields as the SDK header.
struct _ICONINFO {
    BOOL fIcon;  ///< TRUE = icon, FALSE = cursor.
    DWORD xHotspot;  ///< Cursor hotspot x.
    DWORD yHotspot;  ///< Cursor hotspot y.
    HBITMAP hbmMask;  ///< Monochrome mask bitmap.
    HBITMAP hbmColor;  ///< Colour bitmap.
};
using ICONINFO = _ICONINFO;
using PICONINFO = ICONINFO*;
/// MONITORINFO: GetMonitorInfo result. Win32 type; fields as the SDK header.
struct tagMONITORINFO {
    DWORD cbSize;  ///< Size of the structure in bytes.
    RECT rcMonitor;  ///< Monitor rectangle in virtual-screen coordinates.
    RECT rcWork;  ///< Work area (without the taskbar).
    DWORD dwFlags;  ///< STARTF_* bits (MONITORINFO: MONITORINFOF_PRIMARY).
};
using MONITORINFO = tagMONITORINFO;
using LPMONITORINFO = MONITORINFO*;
/// DISPLAY_DEVICEA: EnumDisplayDevicesA result. Win32 type; fields as the SDK header.
struct _DISPLAY_DEVICEA {
    DWORD cb;  ///< Size of the structure in bytes.
    CHAR DeviceName[32];  ///< Adapter name (DISPLAYn under the device namespace).
    CHAR DeviceString[128];  ///< Adapter description.
    DWORD StateFlags;  ///< DISPLAY_DEVICE_* bits.
    CHAR DeviceID[128];  ///< Plug-and-play id.
    CHAR DeviceKey[128];  ///< Registry key.
};
using DISPLAY_DEVICEA = _DISPLAY_DEVICEA;
using PDISPLAY_DEVICEA = DISPLAY_DEVICEA*;
/// WNDCLASSA: window class registration. Win32 type; fields as the SDK header.
struct tagWNDCLASSA {
    UINT style;  ///< Window / class style bits.
    WNDPROC lpfnWndProc;  ///< Window procedure.
    int cbClsExtra;  ///< Extra bytes per class.
    int cbWndExtra;  ///< Extra bytes per window.
    HINSTANCE hInstance;  ///< Owning module.
    HICON hIcon;  ///< Class icon.
    HCURSOR hCursor;  ///< Class cursor.
    HBRUSH hbrBackground;  ///< Background brush.
    LPCSTR lpszMenuName;  ///< Menu resource name.
    LPCSTR lpszClassName;  ///< Class name.
};
using WNDCLASSA = tagWNDCLASSA;
/// CREATESTRUCTA: WM_CREATE parameters. Win32 type; fields as the SDK header.
struct tagCREATESTRUCTA {
    LPVOID lpCreateParams;  ///< The lpParam given to CreateWindowEx.
    HINSTANCE hInstance;  ///< Owning module.
    HMENU hMenu;  ///< Menu or child id.
    HWND hwndParent;  ///< Parent window.
    int cy;  ///< Height.
    int cx;  ///< Width.
    int y;  ///< Vertical coordinate.
    int x;  ///< Horizontal coordinate.
    LONG style;  ///< Window / class style bits.
    LPCSTR lpszName;  ///< Window name.
    LPCSTR lpszClass;  ///< Class name.
    DWORD dwExStyle;  ///< Extended style.
};
using CREATESTRUCTA = tagCREATESTRUCTA;
using DLGPROC = INT_PTR(CALLBACK*)(HWND, UINT, WPARAM, LPARAM);
using LPOFNHOOKPROC = UINT_PTR(CALLBACK*)(HWND, UINT, WPARAM, LPARAM);
/// OPENFILENAMEA: GetOpenFileNameA / GetSaveFileNameA parameters. Win32 type; fields as the SDK header.
struct tagOFNA {
    DWORD lStructSize;  ///< Size of the structure in bytes.
    HWND hwndOwner;  ///< Owner window.
    HINSTANCE hInstance;  ///< Owning module.
    LPCSTR lpstrFilter;  ///< Filter pairs, NUL-separated.
    LPSTR lpstrCustomFilter;  ///< Buffer for the user filter.
    DWORD nMaxCustFilter;  ///< Size of that buffer.
    DWORD nFilterIndex;  ///< Selected filter, 1-based.
    LPSTR lpstrFile;  ///< Buffer for the file name.
    DWORD nMaxFile;  ///< Size of that buffer.
    LPSTR lpstrFileTitle;  ///< Buffer for the name without path.
    DWORD nMaxFileTitle;  ///< Size of that buffer.
    LPCSTR lpstrInitialDir;  ///< Starting folder.
    LPCSTR lpstrTitle;  ///< Dialog title.
    DWORD Flags;  ///< OFN_* bits.
    WORD nFileOffset;  ///< Offset of the file name in lpstrFile.
    WORD nFileExtension;  ///< Offset of the extension in lpstrFile.
    LPCSTR lpstrDefExt;  ///< Default extension.
    LPARAM lCustData;  ///< Data for the hook.
    LPOFNHOOKPROC lpfnHook;  ///< Hook procedure.
    LPCSTR lpTemplateName;  ///< Dialog template name.
    void* pvReserved;  ///< Reserved.
    DWORD dwReserved;  ///< Reserved.
    DWORD FlagsEx;  ///< OFN_EX_* bits.
};
using OPENFILENAMEA = tagOFNA;
using LPOPENFILENAMEA = OPENFILENAMEA*;

// ---------------------------------------------------------------- constants
#define TRUE 1
#define FALSE 0

#define INVALID_HANDLE_VALUE (reinterpret_cast<HANDLE>(static_cast<LONG_PTR>(-1)))
#define INVALID_FILE_SIZE (static_cast<DWORD>(0xffffffff))
#define INVALID_SET_FILE_POINTER (static_cast<DWORD>(-1))
#define INVALID_FILE_ATTRIBUTES (static_cast<DWORD>(-1))

#define GENERIC_READ 0x80000000u
#define GENERIC_WRITE 0x40000000u
#define FILE_SHARE_READ 0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define FILE_SHARE_DELETE 0x00000004
#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5
#define FILE_ATTRIBUTE_READONLY 0x00000001
#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define FILE_ATTRIBUTE_SYSTEM 0x00000004
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_ARCHIVE 0x00000020
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_FLAG_OVERLAPPED 0x40000000
#define FILE_FLAG_NO_BUFFERING 0x20000000
#define FILE_FLAG_RANDOM_ACCESS 0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN 0x08000000
#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_END 2

#define ERROR_SUCCESS 0L
#define NO_ERROR 0L
#define ERROR_INVALID_FUNCTION 1L
#define ERROR_FILE_NOT_FOUND 2L
#define ERROR_PATH_NOT_FOUND 3L
#define ERROR_TOO_MANY_OPEN_FILES 4L
#define ERROR_ACCESS_DENIED 5L
#define ERROR_INVALID_HANDLE 6L
#define ERROR_NOT_ENOUGH_MEMORY 8L
#define ERROR_INVALID_DATA 13L
#define ERROR_OUTOFMEMORY 14L
#define ERROR_NO_MORE_FILES 18L
#define ERROR_HANDLE_EOF 38L
#define ERROR_NOT_SUPPORTED 50L
#define ERROR_FILE_EXISTS 80L
#define ERROR_INVALID_PARAMETER 87L
#define ERROR_BROKEN_PIPE 109L
#define ERROR_CALL_NOT_IMPLEMENTED 120L
#define ERROR_INSUFFICIENT_BUFFER 122L
#define ERROR_ALREADY_EXISTS 183L
#define ERROR_ENVVAR_NOT_FOUND 203L
#define ERROR_MORE_DATA 234L
#define ERROR_NO_MORE_ITEMS 259L
#define ERROR_IO_INCOMPLETE 996L
#define ERROR_IO_PENDING 997L
#define ERROR_NOACCESS 998L

#define S_OK (static_cast<HRESULT>(0))
#define S_FALSE (static_cast<HRESULT>(1))
#define E_NOTIMPL (static_cast<HRESULT>(0x80004001))
#define E_NOINTERFACE (static_cast<HRESULT>(0x80004002))
#define E_POINTER (static_cast<HRESULT>(0x80004003))
#define E_FAIL (static_cast<HRESULT>(0x80004005))
#define E_OUTOFMEMORY (static_cast<HRESULT>(0x8007000E))
#define E_INVALIDARG (static_cast<HRESULT>(0x80070057))
#define SUCCEEDED(hr) (static_cast<HRESULT>(hr) >= 0)
#define FAILED(hr) (static_cast<HRESULT>(hr) < 0)

#define INFINITE 0xFFFFFFFF
#define WAIT_OBJECT_0 0x00000000L
#define WAIT_ABANDONED 0x00000080L
#define WAIT_IO_COMPLETION 0x000000C0L
#define WAIT_TIMEOUT 258L
#define WAIT_FAILED (static_cast<DWORD>(0xFFFFFFFF))

#define IDOK 1
#define IDCANCEL 2
#define IDABORT 3
#define IDRETRY 4
#define IDIGNORE 5
#define IDYES 6
#define IDNO 7
#define TRANSPARENT 1

// ---------------------------------------------------------------- more constants (values from the Windows SDK headers)
#define ANSI_CHARSET 0
#define ANTIALIASED_QUALITY 4
#define BI_BITFIELDS 3
#define BI_RGB 0L
#define C1_ALPHA 0x0100
#define C1_DEFINED 0x0200
#define C1_PUNCT 0x0010
#define CLIP_DEFAULT_PRECIS 0
#define CP_ACP 0
#define CP_MACCP 2
#define CP_OEMCP 1
#define CP_THREAD_ACP 3
#define CP_UTF8 65001
#define CREATE_SUSPENDED 0x00000004
#define CSTR_EQUAL 2
#define CSTR_GREATER_THAN 3
#define CSTR_LESS_THAN 1
#define CT_CTYPE1 0x00000001
#define CW_USEDEFAULT ((int)0x80000000)
#define DIB_RGB_COLORS 0
#define DISPLAY_DEVICE_ATTACHED_TO_DESKTOP 0x00000001
#define DISPLAY_DEVICE_PRIMARY_DEVICE 0x00000004
#define DRIVE_FIXED 3
#define DRIVE_NO_ROOT_DIR 1
#define ERROR_DIRECTORY 267L
#define ERROR_DIR_NOT_EMPTY 145L
#define ERROR_DISK_FULL 112L
#define ERROR_INVALID_ADDRESS 487L
#define ERROR_INVALID_FLAGS 1004L
#define ERROR_INVALID_INDEX 1413L
#define ERROR_INVALID_NAME 123L
#define ERROR_MOD_NOT_FOUND 126L
#define ERROR_NEGATIVE_SEEK 131L
#define ERROR_READ_FAULT 30L
#define ERROR_SEEK 25L
#define EXCEPTION_BREAKPOINT ((DWORD)0x80000003L)
#define EXCEPTION_CONTINUE_SEARCH 0x0
#define EXCEPTION_ILLEGAL_INSTRUCTION ((DWORD)0xC000001DL)
#define FF_MODERN 0x30
#define FILE_FLAG_DELETE_ON_CLOSE 0x04000000
#define FILE_TYPE_DISK 0x0001
#define FILE_TYPE_UNKNOWN 0x0000
#define FIXED_PITCH 1
#define FW_BOLD 700
#define FW_NORMAL 400
#define GENERIC_ALL (0x10000000L)
#define GWLP_WNDPROC (-4)
#define GWL_EXSTYLE (-20)
#define GWL_STYLE (-16)
#define HEAP_REALLOC_IN_PLACE_ONLY 0x00000010
#define HEAP_ZERO_MEMORY 0x00000008
#define IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE 0x0040
#define IMAGE_DLLCHARACTERISTICS_NX_COMPAT 0x0100
#define LCID_INSTALLED 0x00000001
#define LCMAP_BYTEREV 0x00000800
#define LCMAP_LOWERCASE 0x00000100
#define LCMAP_SORTKEY 0x00000400
#define LCMAP_UPPERCASE 0x00000200
#define LOCALE_ICENTURY 0x00000024
#define LOCALE_ICOUNTRY 0x00000005
#define LOCALE_ICURRDIGITS 0x00000019
#define LOCALE_ICURRENCY 0x0000001B
#define LOCALE_IDATE 0x00000021
#define LOCALE_IDAYLZERO 0x00000026
#define LOCALE_IDEFAULTANSICODEPAGE 0x00001004
#define LOCALE_IDEFAULTCODEPAGE 0x0000000B
#define LOCALE_IDEFAULTCOUNTRY 0x0000000A
#define LOCALE_IDEFAULTLANGUAGE 0x00000009
#define LOCALE_IDIGITS 0x00000011
#define LOCALE_IINTLCURRDIGITS 0x0000001A
#define LOCALE_ILANGUAGE 0x00000001
#define LOCALE_ILDATE 0x00000022
#define LOCALE_ILZERO 0x00000012
#define LOCALE_IMEASURE 0x0000000D
#define LOCALE_IMONLZERO 0x00000027
#define LOCALE_INEGCURR 0x0000001C
#define LOCALE_INEGSEPBYSPACE 0x00000057
#define LOCALE_INEGSIGNPOSN 0x00000053
#define LOCALE_INEGSYMPRECEDES 0x00000056
#define LOCALE_IPOSSEPBYSPACE 0x00000055
#define LOCALE_IPOSSIGNPOSN 0x00000052
#define LOCALE_IPOSSYMPRECEDES 0x00000054
#define LOCALE_ITIME 0x00000023
#define LOCALE_ITLZERO 0x00000025
#define LOCALE_NOUSEROVERRIDE 0x80000000
#define LOCALE_RETURN_NUMBER 0x20000000
#define LOCALE_S1159 0x00000028
#define LOCALE_S2359 0x00000029
#define LOCALE_SABBREVCTRYNAME 0x00000007
#define LOCALE_SABBREVDAYNAME1 0x00000031
#define LOCALE_SABBREVDAYNAME7 0x00000037
#define LOCALE_SABBREVLANGNAME 0x00000003
#define LOCALE_SABBREVMONTHNAME1 0x00000044
#define LOCALE_SABBREVMONTHNAME12 0x0000004F
#define LOCALE_SCOUNTRY 0x00000006
#define LOCALE_SCURRENCY 0x00000014
#define LOCALE_SDATE 0x0000001D
#define LOCALE_SDAYNAME1 0x0000002A
#define LOCALE_SDAYNAME7 0x00000030
#define LOCALE_SDECIMAL 0x0000000E
#define LOCALE_SENGCOUNTRY 0x00001002
#define LOCALE_SENGLANGUAGE 0x00001001
#define LOCALE_SGROUPING 0x00000010
#define LOCALE_SINTLSYMBOL 0x00000015
#define LOCALE_SLANGUAGE 0x00000002
#define LOCALE_SLIST 0x0000000C
#define LOCALE_SLONGDATE 0x00000020
#define LOCALE_SMONDECIMALSEP 0x00000016
#define LOCALE_SMONGROUPING 0x00000018
#define LOCALE_SMONTHNAME1 0x00000038
#define LOCALE_SMONTHNAME12 0x00000043
#define LOCALE_SMONTHOUSANDSEP 0x00000017
#define LOCALE_SNATIVECTRYNAME 0x00000008
#define LOCALE_SNATIVEDIGITS 0x00000013
#define LOCALE_SNATIVELANGNAME 0x00000004
#define LOCALE_SNEGATIVESIGN 0x00000051
#define LOCALE_SPOSITIVESIGN 0x00000050
#define LOCALE_SSHORTDATE 0x0000001F
#define LOCALE_STHOUSAND 0x0000000F
#define LOCALE_STIME 0x0000001E
#define LOCALE_STIMEFORMAT 0x00001003
#define LOCALE_USE_CP_ACP 0x40000000
#define MB_ABORTRETRYIGNORE 0x00000002L
#define MB_DEFMASK 0x00000F00L
#define MB_ERR_INVALID_CHARS 0x00000008
#define MB_ICONERROR 0x00000010L
#define MB_ICONINFORMATION 0x00000040L
#define MB_ICONMASK 0x000000F0L
#define MB_ICONQUESTION 0x00000020L
#define MB_ICONWARNING 0x00000030L
#define MB_OK 0x00000000L
#define MB_OKCANCEL 0x00000001L
#define MB_PRECOMPOSED 0x00000001
#define MB_RETRYCANCEL 0x00000005L
#define MB_TYPEMASK 0x0000000FL
#define MB_YESNO 0x00000004L
#define MB_YESNOCANCEL 0x00000003L
#define MEM_COMMIT 0x00001000
#define MEM_RELEASE 0x00008000
#define MEM_RESERVE 0x00002000
#define MONITOR_DEFAULTTONEAREST 0x00000002
#define MONITOR_DEFAULTTOPRIMARY 0x00000001
#define MWMO_ALERTABLE 0x0002
#define MWMO_WAITALL 0x0001
#define NORM_IGNORECASE 0x00000001
#define OFN_FILEMUSTEXIST 0x00001000
#define OFN_NOCHANGEDIR 0x00000008
#define OPENFILENAME_SIZE_VERSION_400A (offsetof(OPENFILENAMEA, lpTemplateName) + sizeof(LPCSTR))
#define OUT_DEFAULT_PRECIS 0
#define PAGE_EXECUTE 0x10
#define PAGE_EXECUTE_READ 0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_WRITECOPY 0x80
#define PAGE_GUARD 0x100
#define PAGE_NOACCESS 0x01
#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#define PAGE_WRITECOPY 0x08
#define PM_REMOVE 0x0001
#define STD_ERROR_HANDLE ((DWORD)-12)
#define STD_INPUT_HANDLE ((DWORD)-10)
#define STD_OUTPUT_HANDLE ((DWORD)-11)
#define STILL_ACTIVE ((DWORD)0x00000103L)
#define SWP_FRAMECHANGED 0x0020
#define SWP_HIDEWINDOW 0x0080
#define SWP_NOMOVE 0x0002
#define SWP_NOSIZE 0x0001
#define SWP_NOZORDER 0x0004
#define SWP_SHOWWINDOW 0x0040
#define SW_FORCEMINIMIZE 11
#define SW_HIDE 0
#define SW_MAXIMIZE 3
#define SW_MINIMIZE 6
#define SW_RESTORE 9
#define SW_SHOWMINIMIZED 2
#define SW_SHOWMINNOACTIVE 7
#define TIME_ZONE_ID_UNKNOWN 0
#define TLS_OUT_OF_INDEXES ((DWORD)0xFFFFFFFF)
#define VER_NT_WORKSTATION 0x0000001
#define VER_PLATFORM_WIN32_NT 2
#define VK_ADD 0x6B
#define VK_BACK 0x08
#define VK_CAPITAL 0x14
#define VK_CONTROL 0x11
#define VK_DECIMAL 0x6E
#define VK_DELETE 0x2E
#define VK_DIVIDE 0x6F
#define VK_DOWN 0x28
#define VK_END 0x23
#define VK_ESCAPE 0x1B
#define VK_F1 0x70
#define VK_F12 0x7B
#define VK_F13 0x7C
#define VK_F24 0x87
#define VK_HOME 0x24
#define VK_INSERT 0x2D
#define VK_LCONTROL 0xA2
#define VK_LEFT 0x25
#define VK_LMENU 0xA4
#define VK_LSHIFT 0xA0
#define VK_MENU 0x12
#define VK_MULTIPLY 0x6A
#define VK_NEXT 0x22
#define VK_NUMLOCK 0x90
#define VK_NUMPAD0 0x60
#define VK_NUMPAD1 0x61
#define VK_NUMPAD9 0x69
#define VK_OEM_1 0xBA
#define VK_OEM_102 0xE2
#define VK_OEM_2 0xBF
#define VK_OEM_3 0xC0
#define VK_OEM_4 0xDB
#define VK_OEM_5 0xDC
#define VK_OEM_6 0xDD
#define VK_OEM_7 0xDE
#define VK_OEM_COMMA 0xBC
#define VK_OEM_MINUS 0xBD
#define VK_OEM_PERIOD 0xBE
#define VK_OEM_PLUS 0xBB
#define VK_PAUSE 0x13
#define VK_PRIOR 0x21
#define VK_RBUTTON 0x02
#define VK_RCONTROL 0xA3
#define VK_RETURN 0x0D
#define VK_RIGHT 0x27
#define VK_RMENU 0xA5
#define VK_RSHIFT 0xA1
#define VK_SCROLL 0x91
#define VK_SHIFT 0x10
#define VK_SNAPSHOT 0x2C
#define VK_SPACE 0x20
#define VK_SUBTRACT 0x6D
#define VK_TAB 0x09
#define VK_UP 0x26
#define WC_COMPOSITECHECK 0x00000200
#define WC_SEPCHARS 0x00000020
#define WHEEL_DELTA 120
#define WM_CLOSE 0x0010
#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_PAINT 0x000F
#define WM_QUIT 0x0012
#define WS_CAPTION 0x00C00000L
#define WS_POPUP 0x80000000L
#define WS_VISIBLE 0x10000000L

// ---------------------------------------------------------------- macros
#define LOBYTE(w) (static_cast<BYTE>(static_cast<DWORD_PTR>(w) & 0xff))
#define HIBYTE(w) (static_cast<BYTE>((static_cast<DWORD_PTR>(w) >> 8) & 0xff))
#define LOWORD(l) (static_cast<WORD>(static_cast<DWORD_PTR>(l) & 0xffff))
#define HIWORD(l) (static_cast<WORD>((static_cast<DWORD_PTR>(l) >> 16) & 0xffff))
#define MAKEWORD(a, b) \
    (static_cast<WORD>(static_cast<BYTE>(static_cast<DWORD_PTR>(a) & 0xff) | \
                       (static_cast<WORD>(static_cast<BYTE>(static_cast<DWORD_PTR>(b) & 0xff)) << 8)))
#define MAKELONG(a, b) \
    (static_cast<LONG>(static_cast<WORD>(static_cast<DWORD_PTR>(a) & 0xffff) | \
                       (static_cast<DWORD>(static_cast<WORD>(static_cast<DWORD_PTR>(b) & 0xffff)) << 16)))
#define MAKELPARAM(l, h) (static_cast<LPARAM>(static_cast<DWORD>(MAKELONG(l, h))))
#define MAKEWPARAM(l, h) (static_cast<WPARAM>(static_cast<DWORD>(MAKELONG(l, h))))
#define GET_X_LPARAM(lp) (static_cast<int>(static_cast<short>(LOWORD(lp))))
#define GET_Y_LPARAM(lp) (static_cast<int>(static_cast<short>(HIWORD(lp))))
#define MAKEINTRESOURCEA(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
#define IS_INTRESOURCE(r) ((((ULONG_PTR)(r)) >> 16) == 0)
#define RGB(r, g, b) \
    (static_cast<COLORREF>(static_cast<BYTE>(r) | (static_cast<WORD>(static_cast<BYTE>(g)) << 8) | \
                           (static_cast<DWORD>(static_cast<BYTE>(b)) << 16)))


// ---------------------------------------------------------------- functions
// The Windows functions our code names (from the Windows SDK headers): declarations only - their types for
// joan::loader::override_function<decltype(&::Name)> (our implementations of them, platform/), nothing defines them
// outside Windows.
extern "C" {
PVOID WINAPI AddVectoredExceptionHandler(ULONG First, PVECTORED_EXCEPTION_HANDLER Handler);
BOOL WINAPI AdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle);
BOOL WINAPI Arc(HDC hdc, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
BOOL WINAPI CloseHandle(HANDLE hObject);
LONG WINAPI CompareFileTime(CONST FILETIME* lpFileTime1, CONST FILETIME* lpFileTime2);
int WINAPI CompareStringA(LCID Locale, DWORD dwCmpFlags, PCNZCH lpString1, int cchCount1, PCNZCH lpString2, int cchCount2);
int WINAPI CompareStringW(LCID Locale, DWORD dwCmpFlags, PCNZWCH lpString1, int cchCount1, PCNZWCH lpString2, int cchCount2);
BOOL WINAPI CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists);
HDC WINAPI CreateCompatibleDC(HDC hdc);
HBITMAP WINAPI CreateDIBSection(HDC hdc, CONST BITMAPINFO* pbmi, UINT usage, VOID** ppvBits, HANDLE hSection, DWORD offset);
BOOL WINAPI CreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
HANDLE WINAPI CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName);
HANDLE WINAPI CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
HANDLE WINAPI CreateFileMappingA(HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName);
HANDLE WINAPI CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
HFONT WINAPI CreateFontA(int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCSTR pszFaceName);
HFONT WINAPI CreateFontIndirectA(CONST LOGFONTA*lplf);
HBRUSH WINAPI CreateSolidBrush(COLORREF color);
HANDLE WINAPI CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
HWND WINAPI CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
VOID WINAPI DebugBreak(VOID);
VOID WINAPI DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
BOOL WINAPI DeleteDC(HDC hdc);
BOOL WINAPI DeleteFileA(LPCSTR lpFileName);
BOOL WINAPI DeleteObject(HGDIOBJ ho);
LRESULT WINAPI DefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI DestroyWindow(HWND hWnd);
INT_PTR WINAPI DialogBoxParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
LRESULT WINAPI DispatchMessageA(CONST MSG*lpMsg);
int WINAPI DrawTextA(HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT format);
int WINAPI DrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format);
BOOL WINAPI EndDialog(HWND hDlg, INT_PTR nResult);
VOID WINAPI EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
BOOL WINAPI EnumDisplayDevicesA(LPCSTR lpDevice, DWORD iDevNum, PDISPLAY_DEVICEA lpDisplayDevice, DWORD dwFlags);
BOOL WINAPI EnumSystemLocalesA(LOCALE_ENUMPROCA lpLocaleEnumProc, DWORD dwFlags);
VOID WINAPI ExitProcess(UINT uExitCode);
VOID WINAPI ExitThread(DWORD dwExitCode);
BOOL WINAPI ExtTextOutA(HDC hdc, int x, int y, UINT options, CONST RECT* lprect, LPCSTR lpString, UINT c, CONST INT* lpDx);
VOID WINAPI FatalAppExitA(UINT uAction, LPCSTR lpMessageText);
BOOL WINAPI FileTimeToLocalFileTime(CONST FILETIME* lpFileTime, LPFILETIME lpLocalFileTime);
BOOL WINAPI FileTimeToSystemTime(CONST FILETIME* lpFileTime, LPSYSTEMTIME lpSystemTime);
int WINAPI FillRect(HDC hDC, CONST RECT*lprc, HBRUSH hbr);
BOOL WINAPI FindClose(HANDLE hFindFile);
HANDLE WINAPI FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
BOOL WINAPI FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);
HRSRC WINAPI FindResourceA(HMODULE hModule, LPCSTR lpName, LPCSTR lpType);
HRSRC WINAPI FindResourceW(HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType);
BOOL WINAPI FlushFileBuffers(HANDLE hFile);
BOOL WINAPI FlushInstructionCache(HANDLE hProcess, LPCVOID lpBaseAddress, SIZE_T dwSize);
BOOL WINAPI FreeEnvironmentStringsA(LPCH penv);
BOOL WINAPI FreeEnvironmentStringsW(LPWCH penv);
BOOL WINAPI FreeLibrary(HMODULE hLibModule);
BOOL WINAPI GdiFlush(void);
UINT WINAPI GetACP(void);
SHORT WINAPI GetAsyncKeyState(int vKey);
BOOL WINAPI GetCPInfo(UINT CodePage, LPCPINFO lpCPInfo);
BOOL WINAPI GetClientRect(HWND hWnd, LPRECT lpRect);
LPSTR WINAPI GetCommandLineA(VOID);
DWORD WINAPI GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer);
HANDLE WINAPI GetCurrentProcess(VOID);
HANDLE WINAPI GetCurrentThread(VOID);
DWORD WINAPI GetCurrentThreadId(VOID);
BOOL WINAPI GetCursorPos(LPPOINT lpPoint);
HDC WINAPI GetDC(HWND hWnd);
int WINAPI GetDIBits(HDC hdc, HBITMAP hbm, UINT start, UINT cLines, LPVOID lpvBits, LPBITMAPINFO lpbmi, UINT usage);
int WINAPI GetDeviceCaps(HDC hdc, int index);
BOOL WINAPI GetDiskFreeSpaceA(LPCSTR lpRootPathName, LPDWORD lpSectorsPerCluster, LPDWORD lpBytesPerSector, LPDWORD lpNumberOfFreeClusters, LPDWORD lpTotalNumberOfClusters);
HWND WINAPI GetDlgItem(HWND hDlg, int nIDDlgItem);
UINT WINAPI GetDriveTypeA(LPCSTR lpRootPathName);
LPCH WINAPI GetEnvironmentStrings(VOID);
LPWCH WINAPI GetEnvironmentStringsW(VOID);
DWORD WINAPI GetFileAttributesA(LPCSTR lpFileName);
DWORD WINAPI GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
DWORD WINAPI GetFileType(HANDLE hFile);
DWORD WINAPI GetFullPathNameA(LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer, LPSTR* lpFilePart);
BOOL WINAPI GetIconInfo(HICON hIcon, PICONINFO piconinfo);
SHORT WINAPI GetKeyState(int nVirtKey);
DWORD WINAPI GetLastError(VOID);
VOID WINAPI GetLocalTime(LPSYSTEMTIME lpSystemTime);
int WINAPI GetLocaleInfoA(LCID Locale, LCTYPE LCType, LPSTR lpLCData, int cchData);
int WINAPI GetLocaleInfoW(LCID Locale, LCTYPE LCType, LPWSTR lpLCData, int cchData);
BOOL WINAPI GetMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName);
HMODULE WINAPI GetModuleHandleW(LPCWSTR lpModuleName);
BOOL WINAPI GetMonitorInfoA(HMONITOR hMonitor, LPMONITORINFO lpmi);
UINT WINAPI GetOEMCP(void);
int WINAPI GetObjectA(HANDLE h, int c, LPVOID pv);
BOOL APIENTRY GetOpenFileNameA(LPOPENFILENAMEA);
UINT WINAPI GetPaletteEntries(HPALETTE hpal, UINT iStart, UINT cEntries, LPPALETTEENTRY pPalEntries);
FARPROC WINAPI GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
BOOL WINAPI GetProcessDEPPolicy(HANDLE hProcess, LPDWORD lpFlags, PBOOL lpPermanent);
HANDLE WINAPI GetProcessHeap(VOID);
VOID WINAPI GetStartupInfoA(LPSTARTUPINFOA lpStartupInfo);
HANDLE WINAPI GetStdHandle(DWORD nStdHandle);
HGDIOBJ WINAPI GetStockObject(int i);
BOOL WINAPI GetStringTypeA(LCID Locale, DWORD dwInfoType, LPCSTR lpSrcStr, int cchSrc, LPWORD lpCharType);
BOOL WINAPI GetStringTypeW(DWORD dwInfoType, LPCWCH lpSrcStr, int cchSrc, LPWORD lpCharType);
VOID WINAPI GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);
VOID WINAPI GetSystemTime(LPSYSTEMTIME lpSystemTime);
VOID WINAPI GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime);
UINT WINAPI GetTempFileNameA(LPCSTR lpPathName, LPCSTR lpPrefixString, UINT uUnique, LPSTR lpTempFileName);
DWORD WINAPI GetTempPathA(DWORD nBufferLength, LPSTR lpBuffer);
BOOL APIENTRY GetTextExtentPoint32A(HDC hdc, LPCSTR lpString, int c, LPSIZE psizl);
DWORD WINAPI GetTickCount(VOID);
DWORD WINAPI GetTimeZoneInformation(LPTIME_ZONE_INFORMATION lpTimeZoneInformation);
LCID WINAPI GetUserDefaultLCID(void);
DWORD WINAPI GetVersion(VOID);
BOOL WINAPI GetVersionExA(LPOSVERSIONINFOA lpVersionInformation);
VOID WINAPI GlobalMemoryStatus(LPMEMORYSTATUS lpBuffer);
LPVOID WINAPI HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
HANDLE WINAPI HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);
BOOL WINAPI HeapDestroy(HANDLE hHeap);
BOOL WINAPI HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
LPVOID WINAPI HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes);
SIZE_T WINAPI HeapSize(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem);
BOOL WINAPI HeapValidate(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem);
VOID WINAPI InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
BOOL WINAPI IsBadCodePtr(FARPROC lpfn);
BOOL WINAPI IsBadReadPtr(CONST VOID*lp, UINT_PTR ucb);
BOOL WINAPI IsBadWritePtr(LPVOID lp, UINT_PTR ucb);
BOOL WINAPI IsValidCodePage(UINT CodePage);
BOOL WINAPI IsValidLocale(LCID Locale, DWORD dwFlags);
int WINAPI LCMapStringA(LCID Locale, DWORD dwMapFlags, LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int WINAPI LCMapStringW(LCID Locale, DWORD dwMapFlags, LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
VOID WINAPI LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
HICON WINAPI LoadIconA(HINSTANCE hInstance, LPCSTR lpIconName);
HMODULE WINAPI LoadLibraryA(LPCSTR lpLibFileName);
HGLOBAL WINAPI LoadResource(HMODULE hModule, HRSRC hResInfo);
LPVOID WINAPI LockResource(HGLOBAL hResData);
LPVOID WINAPI MapViewOfFile(HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh, DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap);
int WINAPI MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
HMONITOR WINAPI MonitorFromPoint(POINT pt, DWORD dwFlags);
HMONITOR WINAPI MonitorFromWindow(HWND hwnd, DWORD dwFlags);
BOOL WINAPI MoveFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName);
DWORD WINAPI MsgWaitForMultipleObjectsEx(DWORD nCount, CONST HANDLE*pHandles, DWORD dwMilliseconds, DWORD dwWakeMask, DWORD dwFlags);
int WINAPI MulDiv(int nNumber, int nNumerator, int nDenominator);
int WINAPI MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
VOID WINAPI OutputDebugStringA(LPCSTR lpOutputString);
BOOL WINAPI PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
BOOL WINAPI Polygon(HDC hdc, CONST POINT*apt, int cpt);
VOID WINAPI PostQuitMessage(int nExitCode);
BOOL WINAPI QueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount);
BOOL WINAPI QueryPerformanceFrequency(LARGE_INTEGER* lpFrequency);
VOID WINAPI RaiseException(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments, CONST ULONG_PTR* lpArguments);
BOOL WINAPI ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
BOOL WINAPI ReadFileEx(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
ATOM WINAPI RegisterClassA(CONST WNDCLASSA*lpWndClass);
int WINAPI ReleaseDC(HWND hWnd, HDC hDC);
BOOL WINAPI ResetEvent(HANDLE hEvent);
DWORD WINAPI ResumeThread(HANDLE hThread);
VOID NTAPI RtlUnwind(PVOID TargetFrame, PVOID TargetIp, PEXCEPTION_RECORD ExceptionRecord, PVOID ReturnValue);
BOOL WINAPI ScreenToClient(HWND hWnd, LPPOINT lpPoint);
HGDIOBJ WINAPI SelectObject(HDC hdc, HGDIOBJ h);
LRESULT WINAPI SendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
COLORREF WINAPI SetBkColor(HDC hdc, COLORREF color);
int WINAPI SetBkMode(HDC hdc, int mode);
BOOL WINAPI SetConsoleCtrlHandler(PHANDLER_ROUTINE HandlerRoutine, BOOL Add);
BOOL WINAPI SetCurrentDirectoryA(LPCSTR lpPathName);
BOOL WINAPI SetCursorPos(int X, int Y);
BOOL WINAPI SetEndOfFile(HANDLE hFile);
BOOL WINAPI SetEnvironmentVariableA(LPCSTR lpName, LPCSTR lpValue);
BOOL WINAPI SetEvent(HANDLE hEvent);
DWORD WINAPI SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);
HWND WINAPI SetFocus(HWND hWnd);
UINT WINAPI SetHandleCount(UINT uNumber);
VOID WINAPI SetLastError(DWORD dwErrCode);
int WINAPI SetMapMode(HDC hdc, int iMode);
UINT WINAPI SetPaletteEntries(HPALETTE hpal, UINT iStart, UINT cEntries, CONST PALETTEENTRY*pPalEntries);
BOOL WINAPI SetProcessDPIAware(VOID);
BOOL WINAPI SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT value);
BOOL WINAPI SetStdHandle(DWORD nStdHandle, HANDLE hHandle);
UINT WINAPI SetTextAlign(HDC hdc, UINT align);
COLORREF WINAPI SetTextColor(HDC hdc, COLORREF color);
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);
LONG WINAPI SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong);
BOOL WINAPI SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
int WINAPI ShowCursor(BOOL bShow);
BOOL WINAPI ShowWindow(HWND hWnd, int nCmdShow);
DWORD WINAPI SizeofResource(HMODULE hModule, HRSRC hResInfo);
VOID WINAPI Sleep(DWORD dwMilliseconds);
DWORD WINAPI SleepEx(DWORD dwMilliseconds, BOOL bAlertable);
BOOL WINAPI TerminateProcess(HANDLE hProcess, UINT uExitCode);
BOOL WINAPI TextOutA(HDC hdc, int x, int y, LPCSTR lpString, int c);
DWORD WINAPI TlsAlloc(VOID);
BOOL WINAPI TlsFree(DWORD dwTlsIndex);
LPVOID WINAPI TlsGetValue(DWORD dwTlsIndex);
BOOL WINAPI TlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue);
BOOL WINAPI TranslateMessage(CONST MSG*lpMsg);
LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo);
BOOL WINAPI UnmapViewOfFile(LPCVOID lpBaseAddress);
BOOL WINAPI UpdateWindow(HWND hWnd);
LPVOID WINAPI VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
BOOL WINAPI VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
BOOL WINAPI VirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
SIZE_T WINAPI VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
DWORD WINAPI WaitForMultipleObjectsEx(DWORD nCount, CONST HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds, BOOL bAlertable);
DWORD WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
int WINAPI WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar);
BOOL WINAPI WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
DWORD WINAPI timeGetTime(void);
int WINAPIV wsprintfA(LPSTR, LPCSTR, ...);
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#endif
