# JoanofArc.exe

- machine: **x86 (32-bit)**, linker 6.0, entry 0x40cf80, base 0x400000, size 266279

## Sections

| name | VA | vsize | rawsize | entropy | X | W | EP |
|---|---|---|---|---|---|---|---|
| `.text` | 0x401000 | 0x33f40 | 0x34000 | 6.03 | x |  | EP |
| `.rdata` | 0x435000 | 0x1b68 | 0x2000 | 4.12 |  |  |  |
| `.data` | 0x437000 | 0x7488 | 0x5000 | 0.96 |  | w |  |
| `.idata` | 0x43f000 | 0xaca | 0x1000 | 3.3 |  | w |  |
| `.rsrc` | 0x440000 | 0x120c | 0x2000 | 2.91 |  |  |  |
| `.reloc` | 0x442000 | 0x1737 | 0x2000 | 4.85 |  |  |  |

## Flags

- imports WINMM.dll: WinMM (timeGetTime/mixer/mci) — must be replaced in the port

## Imports

- **KERNEL32.dll** (71): GetVersion, CreateFileA, SetFilePointer, CloseHandle, WriteFile, ReadFile, GetFileSize, DeleteFileA, FindClose, FindFirstFileA, WinExec, CopyFileA, GetProcAddress, FreeLibrary, GetModuleFileNameA, LoadLibraryA, VirtualAlloc, VirtualFree, CompareStringA, SetEnvironmentVariableA, CompareStringW, IsBadWritePtr, IsBadReadPtr, HeapValidate, ExitProcess, TerminateProcess, GetCurrentProcess, GetSystemTimeAsFileTime, GetTimeZoneInformation, GetSystemTime, GetLocalTime, GetLastError, MoveFileA, RtlUnwind, GetModuleHandleA, GetStartupInfoA, GetCommandLineA, OutputDebugStringA, HeapAlloc, DebugBreak …
- **USER32.dll** (2): ShowCursor, MessageBoxA
- **WINMM.dll** (1): timeGetTime

## Embedded source paths (6)

- `d:\swapdll\file.cpp`
- `d:\swapdll\misc.cpp`
- `d:\swapdll\str.cpp`
- `d:\vertexf\vertexf.h`
- `d:\ww\ilib\file.h`
- `d:\ww\ilib\reslx.h`

## Data-file extensions referenced

`.res`×1
