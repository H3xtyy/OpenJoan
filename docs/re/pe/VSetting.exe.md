# VSetting.exe

- machine: **x86 (32-bit)**, linker 6.0, entry 0x406103, base 0x400000, size 1495093

## Sections

| name | VA | vsize | rawsize | entropy | X | W | EP |
|---|---|---|---|---|---|---|---|
| `.text` | 0x401000 | 0xc3a5 | 0xd000 | 6.4 | x |  | EP |
| `.rdata` | 0x40e000 | 0x10a6 | 0x2000 | 3.36 |  |  |  |
| `.data` | 0x410000 | 0x82b8 | 0x2000 | 2.31 |  | w |  |
| `.rsrc` | 0x419000 | 0x15b000 | 0x15b000 | 7.67 |  |  |  |

## Flags

- imports WINMM.dll: WinMM (timeGetTime/mixer/mci) — must be replaced in the port

## Imports

- **KERNEL32.dll** (68): CreateFileA, SetFilePointer, CloseHandle, WriteFile, ReadFile, GetFileSize, GetCurrentProcess, DeleteFileA, LeaveCriticalSection, EnterCriticalSection, GetCurrentThreadId, InitializeCriticalSection, HeapDestroy, DeleteCriticalSection, FindClose, FindFirstFileA, GetVersionExA, FlushInstructionCache, WaitForSingleObject, GetExitCodeProcess, GetFileAttributesA, SetCurrentDirectoryA, GetCurrentDirectoryA, SetEnvironmentVariableA, FlushFileBuffers, SetStdHandle, SetConsoleCtrlHandler, LCMapStringW, LCMapStringA, GetStringTypeW, GetStringTypeA, MultiByteToWideChar, IsBadWritePtr, GetACP, GetCPInfo, WinExec, LoadLibraryA, GetProcAddress, CreateProcessA, GetOEMCP …
- **USER32.dll** (17): GetWindow, GetWindowRect, GetActiveWindow, DialogBoxParamA, LoadBitmapA, MessageBoxA, GetWindowLongA, GetParent, SetWindowPos, LoadIconA, SystemParametersInfoA, GetClientRect, MapWindowPoints, SetWindowLongA, SendMessageA, GetDlgItem, EndDialog
- **GDI32.dll** (1): DeleteObject
- **ADVAPI32.dll** (3): RegOpenKeyExA, RegQueryValueExA, RegCloseKey
- **SHELL32.dll** (1): ShellExecuteA
- **OLEAUT32.dll** (1): SysFreeString
- **WINMM.dll** (1): timeGetTime

## Data-file extensions referenced

`.x`×29
