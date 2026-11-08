# v3d_sse.dll

- machine: **x86 (32-bit)**, linker 6.0, entry 0x10006610, base 0x10000000, size 270375

## Sections

| name | VA | vsize | rawsize | entropy | X | W | EP |
|---|---|---|---|---|---|---|---|
| `.text` | 0x10001000 | 0x359e0 | 0x36000 | 6.53 | x |  | EP |
| `.rdata` | 0x10037000 | 0x2a10 | 0x3000 | 4.41 |  |  |  |
| `.data` | 0x1003a000 | 0x6b3c | 0x5000 | 1.67 |  | w |  |
| `.idata` | 0x10041000 | 0xa35 | 0x1000 | 3.33 |  | w |  |
| `.reloc` | 0x10042000 | 0x1b5d | 0x2000 | 5.52 |  |  |  |

## Flags


## Imports

- **KERNEL32.dll** (75): HeapDestroy, GetCommandLineA, GetVersion, GetProcAddress, GetModuleHandleA, DebugBreak, GetStdHandle, WriteFile, InterlockedDecrement, OutputDebugStringA, LoadLibraryA, InterlockedIncrement, GetModuleFileNameA, ExitProcess, TerminateProcess, GetCurrentProcess, GetCurrentThreadId, TlsSetValue, TlsAlloc, TlsFree, SetLastError, TlsGetValue, GetLastError, GetCurrentThread, SetHandleCount, GetFileType, GetStartupInfoA, DeleteCriticalSection, IsBadWritePtr, IsBadReadPtr, HeapValidate, FreeEnvironmentStringsA, FreeEnvironmentStringsW, WideCharToMultiByte, GetEnvironmentStrings, GetEnvironmentStringsW, GetEnvironmentVariableA, GetVersionExA, RtlUnwind, HeapCreate …

## Exports

- `??4SkinVertex@@QAEAAU0@ABU0@@Z`
- `?add_bone@SkinVertex@@QAEHHAAU_D3DVECTOR@@0M@Z`
- `?cross_product@@YAAAU_D3DVECTOR@@AAU1@ABU1@1@Z`
- `?deinit@SkinVertex@@QAEXXZ`
- `?dot_product@@YAMABU_D3DVECTOR@@0@Z`
- `?init@SkinVertex@@QAEXXZ`
- `?length@@YAMABU_D3DVECTOR@@@Z`
- `?length_sq@@YAMABU_D3DVECTOR@@@Z`
- `?matrix_inverse@@YAAAU_D3DMATRIX@@AAU1@0@Z`
- `?matrix_inverse@@YAAAU_D3DMATRIX@@AAU1@@Z`
- `?matrix_multiply@@YAAAU_D3DMATRIX@@AAU1@00@Z`
- `?matrix_multiply@@YAAAU_D3DMATRIX@@AAU1@0@Z`
- `?matrix_multiply_translation@@YAAAU_D3DMATRIX@@AAU1@AAU_D3DVECTOR@@@Z`
- `?matrix_transpose@@YAAAU_D3DMATRIX@@AAU1@0@Z`
- `?matrix_transpose@@YAAAU_D3DMATRIX@@AAU1@@Z`
- `?normalize@SkinVertex@@QAEXXZ`
- `?point_transform@@YAAAU_D3DVECTOR@@AAU1@0AAU_D3DMATRIX@@@Z`
- `?point_transform@@YAAAU_D3DVECTOR@@AAU1@AAU_D3DMATRIX@@@Z`
- `?skin_deform@@YAXHPAU_D3DVECTOR@@I0IPAUSkinVertex@@IPAU_D3DMATRIX@@I@Z`
- `?v4d_transform@@YAAAUD3DXVECTOR4@@AAU1@0AAU_D3DMATRIX@@@Z`
- `?v4d_transform@@YAAAUD3DXVECTOR4@@AAU1@AAU_D3DMATRIX@@@Z`
- `?vector_normalize@@YAAAU_D3DVECTOR@@AAU1@0@Z`
- `?vector_normalize@@YAAAU_D3DVECTOR@@AAU1@@Z`
- `?vector_transform@@YAAAU_D3DVECTOR@@AAU1@0AAU_D3DMATRIX@@@Z`
- `?vector_transform@@YAAAU_D3DVECTOR@@AAU1@AAU_D3DMATRIX@@@Z`
- `vertexf_check_cpu`
