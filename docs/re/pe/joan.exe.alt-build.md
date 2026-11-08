# joan.exe.alt-build-ts1074504714-sha3ef3047

- machine: **x86 (32-bit)**, linker 6.0, entry 0x7b71e6, base 0x400000, size 6672428

## Sections

| name | VA | vsize | rawsize | entropy | X | W | EP |
|---|---|---|---|---|---|---|---|
| `.text` | 0x401000 | 0x5cd700 | 0x5ce000 | 6.84 | x |  | EP |
| `.rdata` | 0x9cf000 | 0x24c86 | 0x25000 | 4.88 |  |  |  |
| `.data` | 0x9f4000 | 0x78d44 | 0x40000 | 1.25 |  | w |  |
| `.idata` | 0xa6d000 | 0x25fd | 0x3000 | 4.13 |  | w |  |
| `.rsrc` | 0xa70000 | 0x13d0 | 0x2000 | 3.86 |  |  |  |
| `.reloc` | 0xa72000 | 0x23699 | 0x24000 | 6.09 |  |  |  |

## Flags

- imports WINMM.dll: WinMM (timeGetTime/mixer/mci) — must be replaced in the port
- imports d3d8.dll: Direct3D 8 — must be replaced in the port
- imports DINPUT8.dll: DirectInput 8 — must be replaced in the port
- imports DSOUND.dll: DirectSound — must be replaced in the port
- imports OpenAL32.dll: Creative OpenAL — must be replaced in the port

## Imports

- **KERNEL32.dll** (126): WaitForSingleObject, DeleteCriticalSection, LeaveCriticalSection, EnterCriticalSection, ResetEvent, DebugBreak, WaitForMultipleObjectsEx, GetDiskFreeSpaceA, MulDiv, FileTimeToSystemTime, InitializeCriticalSection, SetEvent, FindFirstFileA, MultiByteToWideChar, SizeofResource, LoadResource, FindResourceA, WriteFile, CreateDirectoryA, WideCharToMultiByte, InterlockedIncrement, InterlockedDecrement, ReadFileEx, CreateEventA, GetCurrentDirectoryA, GetFileSize, CloseHandle, CreateFileA, GetModuleHandleA, SetFilePointer, ReadFile, GetProcAddress, GetModuleFileNameA, LoadLibraryA, GlobalMemoryStatus, FreeLibrary, CopyFileA, FindClose, FindNextFileA, FreeEnvironmentStringsA …
- **USER32.dll** (34): EndDialog, SendMessageA, DrawTextA, DialogBoxParamA, ShowCursor, GetDlgItem, SetFocus, AdjustWindowRectEx, UpdateWindow, CreateWindowExA, MsgWaitForMultipleObjectsEx, RegisterClassA, LoadIconA, GetMessageA, DispatchMessageA, TranslateMessage, GetCursorPos, PeekMessageA, MessageBoxA, ShowWindow, SetCursorPos, wsprintfA, SetWindowLongA, SetWindowPos, ReleaseDC, GetIconInfo, GetDC, PostQuitMessage, GetClientRect, ScreenToClient, GetKeyState, DefWindowProcA, DestroyWindow, DrawTextW
- **GDI32.dll** (18): SetBkMode, DeleteDC, DeleteObject, GetStockObject, GetTextExtentPoint32A, SetTextAlign, ExtTextOutA, SetTextColor, SelectObject, SetBkColor, GetDeviceCaps, SetMapMode, CreateFontA, CreateCompatibleDC, GetDIBits, CreateDIBSection, CreateFontIndirectA, GetObjectA
- **comdlg32.dll** (1): GetOpenFileNameA
- **ADVAPI32.dll** (4): RegQueryValueExA, RegOpenKeyExA, RegCloseKey, RegSetValueExA
- **ole32.dll** (3): CoUninitialize, CoCreateInstance, CoInitialize
- **WINMM.dll** (10): mixerClose, mciSendCommandA, mciSendStringA, mixerSetControlDetails, mixerGetLineControlsA, mixerGetLineInfoA, mixerOpen, mixerGetNumDevs, mixerGetControlDetailsA, timeGetTime
- **d3d8.dll** (1): Direct3DCreate8
- **DINPUT8.dll** (1): DirectInput8Create
- **DSOUND.dll** (1): #1
- **v3d_87.dll** (23): ?vector_transform@@YAAAU_D3DVECTOR@@AAU1@AAU_D3DMATRIX@@@Z, ?matrix_multiply_translation@@YAAAU_D3DMATRIX@@AAU1@AAU_D3DVECTOR@@@Z, vertexf_check_cpu, ?length_sq@@YAMABU_D3DVECTOR@@@Z, ?length@@YAMABU_D3DVECTOR@@@Z, ?matrix_multiply@@YAAAU_D3DMATRIX@@AAU1@0@Z, ?cross_product@@YAAAU_D3DVECTOR@@AAU1@ABU1@1@Z, ?matrix_inverse@@YAAAU_D3DMATRIX@@AAU1@@Z, ?deinit@SkinVertex@@QAEXXZ, ?init@SkinVertex@@QAEXXZ, ?add_bone@SkinVertex@@QAEHHAAU_D3DVECTOR@@0M@Z, ?normalize@SkinVertex@@QAEXXZ, ?skin_deform@@YAXHPAU_D3DVECTOR@@I0IPAUSkinVertex@@IPAU_D3DMATRIX@@I@Z, ?matrix_transpose@@YAAAU_D3DMATRIX@@AAU1@@Z, ?matrix_transpose@@YAAAU_D3DMATRIX@@AAU1@0@Z, ?matrix_multiply@@YAAAU_D3DMATRIX@@AAU1@00@Z, ?vector_transform@@YAAAU_D3DVECTOR@@AAU1@0AAU_D3DMATRIX@@@Z, ?point_transform@@YAAAU_D3DVECTOR@@AAU1@AAU_D3DMATRIX@@@Z, ?point_transform@@YAAAU_D3DVECTOR@@AAU1@0AAU_D3DMATRIX@@@Z, ?vector_normalize@@YAAAU_D3DVECTOR@@AAU1@0@Z, ?matrix_inverse@@YAAAU_D3DMATRIX@@AAU1@0@Z, ?vector_normalize@@YAAAU_D3DVECTOR@@AAU1@@Z, ?dot_product@@YAMABU_D3DVECTOR@@0@Z
- **OpenAL32.dll** (32): alSourceRewind, alSourceUnqueueBuffers, alSourcePause, alSourceQueueBuffers, alSourcePlay, alSourcef, alSourcei, alDistanceModel, alGetSourcei, alListenerf, alListener3f, alListenerfv, alGetString, alGetError, alBufferData, alDopplerFactor, alDopplerVelocity, alIsBuffer, alGenBuffers, alDeleteBuffers, alIsSource, alGenSources, alDeleteSources, alcMakeContextCurrent, alcCreateContext, alcOpenDevice, alcCloseDevice, alcDestroyContext, alcGetContextsDevice, alcGetCurrentContext, alSourceStop, alSource3f

## Embedded source paths (119)

- `D:\ww\en3d\texturea.cpp`
- `D:\ww\unit.cpp`
- `d:\ww\animgrp.cpp`
- `d:\ww\animres.cpp`
- `d:\ww\animscr.cpp`
- `d:\ww\battle.cpp`
- `d:\ww\camman.cpp`
- `d:\ww\camres.cpp`
- `d:\ww\cpuusage.cpp`
- `d:\ww\ditemres.cpp`
- `d:\ww\dyngrass.cpp`
- `d:\ww\elib\jpeg.cpp`
- `d:\ww\en2d\bmpres.cpp`
- `d:\ww\en2d\font2d.cpp`
- `d:\ww\en2d\image2d.cpp`
- `d:\ww\en2d\popupwin.cpp`
- `d:\ww\en2d\txtbox2d.cpp`
- `d:\ww\en2d\widget2d.cpp`
- `d:\ww\en3d\actor.cpp`
- `d:\ww\en3d\actordef.cpp`
- `d:\ww\en3d\actordeg.cpp`
- `d:\ww\en3d\animdef.cpp`
- `d:\ww\en3d\bonetree.cpp`
- `d:\ww\en3d\morph.cpp`
- `d:\ww\en3d\plane.cpp`
- `d:\ww\en3d\plane_sh.cpp`
- `d:\ww\en3d\pshader.cpp`
- `d:\ww\en3d\reorder.cpp`
- `d:\ww\en3d\scene.cpp`
- `d:\ww\en3d\scene_s.cpp`
- `d:\ww\en3d\skin.cpp`
- `d:\ww\en3d\sysen.cpp`
- `d:\ww\en3d\texdir.cpp`
- `d:\ww\en3d\vgabuf.cpp`
- `d:\ww\en3d\vgalock.cpp`
- `d:\ww\end_con.cpp`
- `d:\ww\f_shop.cpp`
- `d:\ww\flocksys.cpp`
- `d:\ww\game.cpp`
- `d:\ww\gamefil2.cpp`
- `d:\ww\gamefila.cpp`
- `d:\ww\gamefile.cpp`
- `d:\ww\gameplay.cpp`
- `d:\ww\gfile3.cpp`
- `d:\ww\grassres.cpp`
- `d:\ww\help.cpp`
- `d:\ww\i_gate.cpp`
- `d:\ww\i_wall.cpp`
- `d:\ww\ilib\audio.cpp`
- `d:\ww\ilib\bitmap.cpp`
- `d:\ww\ilib\bitmapd.cpp`
- `d:\ww\ilib\bitmapm.cpp`
- `d:\ww\ilib\bitmapt.cpp`
- `d:\ww\ilib\bitmapw.cpp`
- `d:\ww\ilib\blob.cpp`
- `d:\ww\ilib\db.cpp`
- `d:\ww\ilib\dynarr.cpp`
- `d:\ww\ilib\dynarrb.cpp`
- `d:\ww\ilib\filemem.cpp`
- `d:\ww\ilib\filetxt.cpp`
- `d:\ww\ilib\flc.cpp`
- `d:\ww\ilib\gameset.cpp`
- `d:\ww\ilib\mousecr.cpp`
- `d:\ww\ilib\resdb.cpp`
- `d:\ww\ilib\reslx.cpp`
- `d:\ww\ilib\resx.cpp`
- `d:\ww\ilib\se.cpp`
- `d:\ww\ilib\sound3d.cpp`
- `d:\ww\ilib\txtres.cpp`
- `d:\ww\ilib\vqueue.cpp`
- `d:\ww\interfac\combobox.cpp`
- `d:\ww\interfac\i_barr.cpp`
- `d:\ww\interfac\i_buy.cpp`
- `d:\ww\interfac\i_letter.cpp`
- `d:\ww\interfac\i_sms.cpp`
- `d:\ww\interfac\i_work.cpp`
- `d:\ww\interfac\list.cpp`
- `d:\ww\interfac\minires.cpp`
- `d:\ww\interfac\objmenu.cpp`
- `d:\ww\interfac\objmenub.cpp`
- `d:\ww\interfac\objmenuw.cpp`
- `d:\ww\interfac\optmenui.cpp`
- `d:\ww\itemres.cpp`
- `d:\ww\levelres.cpp`
- `d:\ww\lightsys.cpp`
- `d:\ww\modelres.cpp`
- `d:\ww\msglog.cpp`
- `d:\ww\musicres.cpp`
- `d:\ww\nation.cpp`
- `d:\ww\nation2.cpp`
- `d:\ww\natres.cpp`
- `d:\ww\pemires.cpp`
- `d:\ww\plasma.cpp`
- `d:\ww\profile.cpp`
- `d:\ww\raceres.cpp`
- `d:\ww\recrures.cpp`
- `d:\ww\remapkey.cpp`
- `d:\ww\scenario.cpp`
- `d:\ww\sdateres.cpp`
- `d:\ww\sitemres.cpp`
- `d:\ww\speech_s.cpp`
- `d:\ww\stackt.h`
- `d:\ww\t_scrres.cpp`
- `d:\ww\trackres.cpp`
- `d:\ww\unitlvl.cpp`
- `d:\ww\unitres.cpp`
- `d:\ww\unitures.cpp`
- `d:\ww\upgrares.cpp`
- `d:\ww\uscrres.cpp`
- `d:\ww\uti3d\blkarrb.cpp`
- `d:\ww\uti3d\dlgres.cpp`
- `d:\ww\uti3d\esstate2.cpp`
- `d:\ww\uti3d\lflare.cpp`
- `d:\ww\uti3d\pathsp.cpp`
- `d:\ww\uti3d\spindex.cpp`
- `d:\ww\uti3d\video.cpp`
- `d:\ww\uti3d\videocap.cpp`
- `d:\ww\world.cpp`
- `d:\ww\world_s.cpp`

## Data-file extensions referenced

`.am1`×3, `.am2`×2, `.am3`×1, `.bm`×1, `.bmp`×1, `.cm`×3, `.dds`×1, `.fin`×1, `.jpg`×1, `.lgt`×1, `.mda`×1, `.nvo`×7, `.obj`×1, `.pro`×1, `.pso`×2, `.res`×23, `.sav`×3, `.sca`×1, `.sce`×3, `.scr`×1, `.set`×2, `.smo`×2, `.tga`×1, `.txr`×2, `.txt`×20, `.uni`×1, `.wav`×1, `.wmv`×4, `.x`×66
