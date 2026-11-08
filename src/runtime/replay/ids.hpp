// Record / replay: the intercepted calls. The id of a call is its position in this list and is stored in recordings,
// so the list is APPEND-ONLY (a removed entry stays as a placeholder) - otherwise old recordings no longer replay.
#pragma once

#include <cstdint>

// clang-format off
#define JOAN_REPLAY_IDS(X) \
    /* KERNEL32: time */ \
    X(QueryPerformanceCounter) X(QueryPerformanceFrequency) X(GetLocalTime) X(GetSystemTime) \
    X(GetSystemTimeAsFileTime) X(GetTimeZoneInformation) \
    /* KERNEL32: waits, sleeps, asynchronous reads */ \
    X(Sleep) X(SleepEx) X(WaitForSingleObject) X(WaitForMultipleObjectsEx) X(ReadFileEx) \
    /* KERNEL32: system / environment queries */ \
    X(GlobalMemoryStatus) X(GetDiskFreeSpaceA) X(GetDriveTypeA) X(GetVersion) X(GetVersionExA) X(GetLastError) \
    X(GetCommandLineA) \
    /* KERNEL32: files */ \
    X(CreateFileA) X(ReadFile) X(FindFirstFileA) X(FindNextFileA) X(FindClose) X(CreateEventA) \
    /* USER32: messages, window, input */ \
    X(PeekMessageA) X(GetMessageA) X(TranslateMessage) X(DispatchMessageA) X(MsgWaitForMultipleObjectsEx) \
    X(GetCursorPos) X(GetKeyState) X(ScreenToClient) X(GetClientRect) X(AdjustWindowRectEx) X(ShowCursor) \
    X(SetCursorPos) X(RegisterClassA) X(CreateWindowExA) X(ShowWindow) X(UpdateWindow) X(SetFocus) \
    X(SetWindowPos) X(SetWindowLongA) X(DestroyWindow) X(DefWindowProcA) X(MessageBoxA) X(DialogBoxParamA) \
    X(DrawTextA) X(DrawTextW) \
    /* GDI32 */ \
    X(GetTextExtentPoint32A) X(GetDeviceCaps) \
    /* comdlg32, ADVAPI32, ole32 */ \
    X(GetOpenFileNameA) X(RegOpenKeyExA) X(RegQueryValueExA) X(RegSetValueExA) X(RegCloseKey) X(CoCreateInstance) \
    /* WINMM */ \
    X(timeGetTime) X(mciSendCommandA) X(mciSendStringA) X(mixerGetNumDevs) X(mixerOpen) X(mixerClose) \
    X(mixerGetLineInfoA) X(mixerGetLineControlsA) X(mixerGetControlDetailsA) X(mixerSetControlDetails) \
    /* OpenAL32 */ \
    X(alGetSourcei) X(alGetError) X(alcOpenDevice) X(alcCreateContext) \
    /* factories */ \
    X(Direct3DCreate8) X(DirectInput8Create) X(DirectSoundCreate) \
    /* IDirectInput8A / IDirectInputDevice8A */ \
    X(DI_CreateDevice) X(DI_EnumDevices) X(DID_GetCapabilities) X(DID_SetProperty) X(DID_Acquire) X(DID_Unacquire) \
    X(DID_GetDeviceState) X(DID_GetDeviceData) X(DID_SetDataFormat) X(DID_SetCooperativeLevel) X(DID_Poll) \
    /* IDirectSound / IDirectSoundBuffer */ \
    X(DS_CreateSoundBuffer) X(DS_GetCaps) X(DSB_GetCurrentPosition) X(DSB_GetStatus) X(DSB_Restore) \
    /* IDirect3D8 / IDirect3DDevice8 */ \
    X(D3D_GetAdapterCount) X(D3D_GetAdapterIdentifier) X(D3D_GetAdapterModeCount) X(D3D_EnumAdapterModes) \
    X(D3D_GetAdapterDisplayMode) X(D3D_CheckDeviceType) X(D3D_CheckDeviceFormat) X(D3D_CheckDeviceMultiSampleType) \
    X(D3D_CheckDepthStencilMatch) X(D3D_GetDeviceCaps) X(D3D_CreateDevice) \
    X(DEV_TestCooperativeLevel) X(DEV_GetAvailableTextureMem) X(DEV_GetDeviceCaps) X(DEV_GetDisplayMode) \
    X(DEV_Reset) X(DEV_Present) X(DEV_GetRasterStatus) \
    /* instructions (patch 136) */ \
    X(rdtsc) \
    /* OpenAL results the game keeps (patch 146) */ \
    X(alGenSources) X(alGenBuffers) X(alIsSource) X(alIsBuffer) X(alSourceUnqueueBuffers) \
    /* the CRT's environment block (patch 152: its size decides the game heap's first offsets) */ \
    X(GetEnvironmentStringsW) \
    /* the platform layer's native keyboard / mouse (platform/input.hpp, phase C4) */ \
    X(IN_Acquire) X(IN_Unacquire) X(IN_Read) X(IN_PadMask) X(IN_PadRead)
// clang-format on

namespace joan::replay {

#define JOAN_REPLAY_ID_ENUM(name) name,
/// Ids of the intercepted calls (JOAN_REPLAY_IDS above); the position in the list is stored in recordings. Count is the number of ids.
enum class Id : std::uint16_t { JOAN_REPLAY_IDS(JOAN_REPLAY_ID_ENUM) Count };
#undef JOAN_REPLAY_ID_ENUM

/// The call's name for reports ("?" for an id this build does not know).
const char* id_name(std::uint16_t id);

}  // namespace joan::replay
