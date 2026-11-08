// Record / replay: methods of the COM objects the game creates through its imports - Direct3D 8 (adapter queries,
// device state, Present = the frame boundary) - and the platform layer's keyboard / mouse / pads (input_call).
// Overview: replay.hpp. (DirectInput and DirectSound are not used any more - cleanup C1 / C2 - so nothing of theirs is
// wrapped.)
//
// The methods are wrapped by patching the object's vtable entry (shared by every object of its class), so the
// game's calls `com_call(obj, offset, ...)` reach the wrapper with `this` first (COM methods are __stdcall). The
// original entry is kept per (vtable, index). Our own Direct3D 8 device (gl::Device) is patched the same way.
#include <atomic>
#include <cstring>
#include <utility>

#include "../../compiler.hpp"
#include "../../call.hpp"
#include "../../d3d8/d3d8_types.hpp"
#include "../../platform/memory.hpp"
#include "internal.hpp"
#include "replay.hpp"
#include "sys.hpp"

namespace joan::replay {
namespace {

namespace d3 = joan::d3d8;

// ---------------------------------------------------------------- vtable patches

struct Patch {
    void** vtbl;
    int index;
    void* original;
};
constexpr int kMaxPatches = 128;
Patch g_patches[kMaxPatches];
std::atomic<int> g_patch_count{0};

/// Points vtable entry `index` of `obj`'s class at `wrapper` (once per vtable).
void patch(void* obj, int index, void* wrapper) {
    void** vt = joan::call::mutable_vtable_of(obj);
    const int n = g_patch_count.load(std::memory_order_acquire);
    for (int i = 0; i < n; ++i)
        if (g_patches[i].vtbl == vt && g_patches[i].index == index) return;
    if (n >= kMaxPatches || vt[index] == wrapper) return;
    const platform::memory::Writable w(&vt[index], sizeof(void*));
    if (!w.ok()) {
        log("cannot patch vtable %p entry %d (page protection)", static_cast<void*>(vt), index);
        return;
    }
    g_patches[n] = {vt, index, vt[index]};
    g_patch_count.store(n + 1, std::memory_order_release);
    vt[index] = wrapper;
}

/// The original method `index` of `self`'s class.
template <class F>
F original(void* self, int index) {
    void** vt = joan::call::mutable_vtable_of(self);
    const int n = g_patch_count.load(std::memory_order_acquire);
    for (int i = 0; i < n; ++i)
        if (g_patches[i].vtbl == vt && g_patches[i].index == index) return reinterpret_cast<F>(g_patches[i].original);
    return nullptr;  // cannot happen: the wrapper is only reachable through a patched entry
}

/// value: the recorded T on replay, else `real()` (recorded when recording).
template <class T, class F>
T value(Id id, std::uint32_t check, F&& real) {
    T r{};
    if (replaying() && play(id, check, r)) return r;
    r = real();
    rec(id, check, r);
    return r;
}

/// real: `real()` always runs; replay returns the recorded T.
template <class T, class F>
T real_then(Id id, std::uint32_t check, F&& real) {
    const T r = real();
    if (replaying()) {
        T recorded{};
        if (play(id, check, recorded)) return recorded;
        return r;
    }
    rec(id, check, r);
    return r;
}

/// Is `ret` (a wrapper's return address) inside openjoan.exe? The vtables are patched per class, so other modules'
/// own objects of the same class reach the wrappers too (the system's d3d8.dll may call its own methods). Those calls
/// are not the game's: they pass through unrecorded.
bool from_our_image(const void* ret) {
    static const auto range = [] {
        std::uintptr_t low = 0, high = 0;
        sys::exe_range(low, high);
        return std::pair<std::uintptr_t, std::uintptr_t>(low, high);
    }();
    const auto a = reinterpret_cast<std::uintptr_t>(ret);
    return a >= range.first && a < range.second;
}

#define COM_THREAD_OR(id, real_call)            \
    if (!on_main()) {                           \
        other_thread(Id::id);                   \
        return real_call;                       \
    }                                           \
    if (!from_our_image(JOAN_RETURN_ADDRESS())) { \
        return real_call;                       \
    }

// ---------------------------------------------------------------- IDirect3D8

constexpr int idx(d3::D3DMethod m) { return static_cast<int>(m); }
constexpr int idx(d3::DeviceMethod m) { return static_cast<int>(m); }
constexpr std::uint32_t kIdentifierSize = 512 + 512 + 8 + 4 * 4 + 16 + 4;  // D3DADAPTER_IDENTIFIER8

using D3dCountFn = UINT(WINAPI*)(void*);
using D3dIdentFn = HRESULT(WINAPI*)(void*, UINT, DWORD, void*);
using D3dModeCountFn = UINT(WINAPI*)(void*, UINT);
using D3dEnumModesFn = HRESULT(WINAPI*)(void*, UINT, UINT, void*);
using D3dDisplayModeFn = HRESULT(WINAPI*)(void*, UINT, void*);
using D3dCheck5Fn = HRESULT(WINAPI*)(void*, UINT, d3::Enum, d3::Enum, d3::Enum, BOOL);
using D3dCheck6Fn = HRESULT(WINAPI*)(void*, UINT, d3::Enum, d3::Enum, DWORD, d3::Enum, d3::Enum);
using D3dCheckMsFn = HRESULT(WINAPI*)(void*, UINT, d3::Enum, d3::Enum, BOOL, d3::Enum);
using D3dCheckDsFn = HRESULT(WINAPI*)(void*, UINT, d3::Enum, d3::Enum, d3::Enum, d3::Enum);
using D3dCapsFn = HRESULT(WINAPI*)(void*, UINT, d3::Enum, void*);
using D3dCreateDeviceFn = HRESULT(WINAPI*)(void*, UINT, d3::Enum, HWND, DWORD, d3::PresentParameters*, void**);

// value (all adapter queries: what the display / driver offers must not change a replay)
UINT WINAPI d3d_get_adapter_count(void* self) {
    const auto o = original<D3dCountFn>(self, idx(d3::D3DMethod::GetAdapterCount));
    COM_THREAD_OR(D3D_GetAdapterCount, o(self));
    return value<UINT>(Id::D3D_GetAdapterCount, 0, [&] { return o(self); });
}

HRESULT WINAPI d3d_get_adapter_identifier(void* self, UINT adapter, DWORD flags, void* ident) {
    const auto o = original<D3dIdentFn>(self, idx(d3::D3DMethod::GetAdapterIdentifier));
    COM_THREAD_OR(D3D_GetAdapterIdentifier, o(self, adapter, flags, ident));
    struct R {
        HRESULT hr;
        std::uint8_t ident[kIdentifierSize];
    };
    const R x = value<R>(Id::D3D_GetAdapterIdentifier, check_of({adapter, flags}), [&] {
        R y{};
        y.hr = o(self, adapter, flags, y.ident);
        return y;
    });
    if (ident) std::memcpy(ident, x.ident, kIdentifierSize);
    return x.hr;
}

UINT WINAPI d3d_get_adapter_mode_count(void* self, UINT adapter) {
    const auto o = original<D3dModeCountFn>(self, idx(d3::D3DMethod::GetAdapterModeCount));
    COM_THREAD_OR(D3D_GetAdapterModeCount, o(self, adapter));
    return value<UINT>(Id::D3D_GetAdapterModeCount, adapter, [&] { return o(self, adapter); });
}

struct ModeResult {
    HRESULT hr;
    d3::DisplayMode mode;
};

HRESULT WINAPI d3d_enum_adapter_modes(void* self, UINT adapter, UINT index, void* out) {
    const auto o = original<D3dEnumModesFn>(self, idx(d3::D3DMethod::EnumAdapterModes));
    COM_THREAD_OR(D3D_EnumAdapterModes, o(self, adapter, index, out));
    const ModeResult x = value<ModeResult>(Id::D3D_EnumAdapterModes, check_of({adapter, index}), [&] {
        ModeResult y{};
        y.hr = o(self, adapter, index, &y.mode);
        return y;
    });
    if (out) std::memcpy(out, &x.mode, sizeof(x.mode));
    return x.hr;
}

HRESULT WINAPI d3d_get_adapter_display_mode(void* self, UINT adapter, void* out) {
    const auto o = original<D3dDisplayModeFn>(self, idx(d3::D3DMethod::GetAdapterDisplayMode));
    COM_THREAD_OR(D3D_GetAdapterDisplayMode, o(self, adapter, out));
    const ModeResult x = value<ModeResult>(Id::D3D_GetAdapterDisplayMode, adapter, [&] {
        ModeResult y{};
        y.hr = o(self, adapter, &y.mode);
        return y;
    });
    if (out) std::memcpy(out, &x.mode, sizeof(x.mode));
    return x.hr;
}

HRESULT WINAPI d3d_check_device_type(void* self, UINT a, d3::Enum t, d3::Enum df, d3::Enum bf, BOOL windowed) {
    const auto o = original<D3dCheck5Fn>(self, idx(d3::D3DMethod::CheckDeviceType));
    COM_THREAD_OR(D3D_CheckDeviceType, o(self, a, t, df, bf, windowed));
    return value<HRESULT>(Id::D3D_CheckDeviceType, check_of({a, t, df, bf, static_cast<std::uint32_t>(windowed)}),
                          [&] { return o(self, a, t, df, bf, windowed); });
}

HRESULT WINAPI d3d_check_device_format(void* self, UINT a, d3::Enum t, d3::Enum af, DWORD usage, d3::Enum rt,
                                       d3::Enum cf) {
    const auto o = original<D3dCheck6Fn>(self, idx(d3::D3DMethod::CheckDeviceFormat));
    COM_THREAD_OR(D3D_CheckDeviceFormat, o(self, a, t, af, usage, rt, cf));
    return value<HRESULT>(Id::D3D_CheckDeviceFormat, check_of({a, t, af, usage, rt, cf}),
                          [&] { return o(self, a, t, af, usage, rt, cf); });
}

HRESULT WINAPI d3d_check_multisample(void* self, UINT a, d3::Enum t, d3::Enum sf, BOOL windowed, d3::Enum ms) {
    const auto o = original<D3dCheckMsFn>(self, idx(d3::D3DMethod::CheckDeviceMultiSampleType));
    COM_THREAD_OR(D3D_CheckDeviceMultiSampleType, o(self, a, t, sf, windowed, ms));
    return value<HRESULT>(Id::D3D_CheckDeviceMultiSampleType,
                          check_of({a, t, sf, static_cast<std::uint32_t>(windowed), ms}),
                          [&] { return o(self, a, t, sf, windowed, ms); });
}

HRESULT WINAPI d3d_check_depth_stencil(void* self, UINT a, d3::Enum t, d3::Enum af, d3::Enum rf, d3::Enum df) {
    const auto o = original<D3dCheckDsFn>(self, idx(d3::D3DMethod::CheckDepthStencilMatch));
    COM_THREAD_OR(D3D_CheckDepthStencilMatch, o(self, a, t, af, rf, df));
    return value<HRESULT>(Id::D3D_CheckDepthStencilMatch, check_of({a, t, af, rf, df}),
                          [&] { return o(self, a, t, af, rf, df); });
}

struct CapsResult {
    HRESULT hr;
    d3::Caps8 caps;
};

HRESULT WINAPI d3d_get_device_caps(void* self, UINT a, d3::Enum t, void* caps) {
    const auto o = original<D3dCapsFn>(self, idx(d3::D3DMethod::GetDeviceCaps));
    COM_THREAD_OR(D3D_GetDeviceCaps, o(self, a, t, caps));
    const CapsResult x = value<CapsResult>(Id::D3D_GetDeviceCaps, check_of({a, t}), [&] {
        CapsResult y{};
        y.hr = o(self, a, t, &y.caps);
        return y;
    });
    if (caps) std::memcpy(caps, &x.caps, sizeof(x.caps));
    return x.hr;
}

// ---------------------------------------------------------------- IDirect3DDevice8

using DevVoidFn = HRESULT(WINAPI*)(void*);
using DevUintFn = UINT(WINAPI*)(void*);
using DevOutFn = HRESULT(WINAPI*)(void*, void*);
using DevResetFn = HRESULT(WINAPI*)(void*, d3::PresentParameters*);
using DevPresentFn = HRESULT(WINAPI*)(void*, const void*, const void*, HWND, const void*);

std::uint32_t check_pp(const d3::PresentParameters* pp) {
    if (!pp) return 0;
    return check_of({pp->back_buffer_width, pp->back_buffer_height, pp->back_buffer_format, pp->back_buffer_count,
                     pp->windowed ? 1u : 0u, pp->enable_auto_depth_stencil ? 1u : 0u, pp->auto_depth_stencil_format});
}

// value (device lost / restored is the recording's)
HRESULT WINAPI dev_test_cooperative_level(void* self) {
    const auto o = original<DevVoidFn>(self, idx(d3::DeviceMethod::TestCooperativeLevel));
    COM_THREAD_OR(DEV_TestCooperativeLevel, o(self));
    return value<HRESULT>(Id::DEV_TestCooperativeLevel, 0, [&] { return o(self); });
}

UINT WINAPI dev_get_available_texture_mem(void* self) {
    const auto o = original<DevUintFn>(self, idx(d3::DeviceMethod::GetAvailableTextureMem));
    COM_THREAD_OR(DEV_GetAvailableTextureMem, o(self));
    return value<UINT>(Id::DEV_GetAvailableTextureMem, 0, [&] { return o(self); });
}

HRESULT WINAPI dev_get_device_caps(void* self, void* caps) {
    const auto o = original<DevOutFn>(self, idx(d3::DeviceMethod::GetDeviceCaps));
    COM_THREAD_OR(DEV_GetDeviceCaps, o(self, caps));
    const CapsResult x = value<CapsResult>(Id::DEV_GetDeviceCaps, 0, [&] {
        CapsResult y{};
        y.hr = o(self, &y.caps);
        return y;
    });
    if (caps) std::memcpy(caps, &x.caps, sizeof(x.caps));
    return x.hr;
}

HRESULT WINAPI dev_get_display_mode(void* self, void* mode_out) {
    const auto o = original<DevOutFn>(self, idx(d3::DeviceMethod::GetDisplayMode));
    COM_THREAD_OR(DEV_GetDisplayMode, o(self, mode_out));
    const ModeResult x = value<ModeResult>(Id::DEV_GetDisplayMode, 0, [&] {
        ModeResult y{};
        y.hr = o(self, &y.mode);
        return y;
    });
    if (mode_out) std::memcpy(mode_out, &x.mode, sizeof(x.mode));
    return x.hr;
}

HRESULT WINAPI dev_get_raster_status(void* self, void* status) {
    const auto o = original<DevOutFn>(self, idx(d3::DeviceMethod::GetRasterStatus));
    COM_THREAD_OR(DEV_GetRasterStatus, o(self, status));
    struct R {
        HRESULT hr;
        d3::RasterStatus s;
    };
    const R x = value<R>(Id::DEV_GetRasterStatus, 0, [&] {
        R y{};
        y.hr = o(self, &y.s);
        return y;
    });
    if (status) std::memcpy(status, &x.s, sizeof(x.s));
    return x.hr;
}

// real
HRESULT WINAPI dev_reset(void* self, d3::PresentParameters* pp) {
    const auto o = original<DevResetFn>(self, idx(d3::DeviceMethod::Reset));
    COM_THREAD_OR(DEV_Reset, o(self, pp));
    return real_then<HRESULT>(Id::DEV_Reset, check_pp(pp), [&] { return o(self, pp); });
}

// The frame boundary: state hashes (replay.cpp frame_boundary), then the real Present.
HRESULT WINAPI dev_present(void* self, const void* src, const void* dst, HWND window, const void* dirty) {
    const auto o = original<DevPresentFn>(self, idx(d3::DeviceMethod::Present));
    COM_THREAD_OR(DEV_Present, o(self, src, dst, window, dirty));
    frame_boundary(self);
    return real_then<HRESULT>(Id::DEV_Present, 0, [&] { return o(self, src, dst, window, dirty); });
}

void wrap_device(void* dev) {
    patch(dev, idx(d3::DeviceMethod::TestCooperativeLevel), reinterpret_cast<void*>(&dev_test_cooperative_level));
    patch(dev, idx(d3::DeviceMethod::GetAvailableTextureMem), reinterpret_cast<void*>(&dev_get_available_texture_mem));
    patch(dev, idx(d3::DeviceMethod::GetDeviceCaps), reinterpret_cast<void*>(&dev_get_device_caps));
    patch(dev, idx(d3::DeviceMethod::GetDisplayMode), reinterpret_cast<void*>(&dev_get_display_mode));
    patch(dev, idx(d3::DeviceMethod::GetRasterStatus), reinterpret_cast<void*>(&dev_get_raster_status));
    patch(dev, idx(d3::DeviceMethod::Reset), reinterpret_cast<void*>(&dev_reset));
    patch(dev, idx(d3::DeviceMethod::Present), reinterpret_cast<void*>(&dev_present));
}

// real (the device is created in both modes; it gets its methods wrapped)
HRESULT WINAPI d3d_create_device(void* self, UINT a, d3::Enum t, HWND focus, DWORD behavior, d3::PresentParameters* pp,
                                 void** out) {
    const auto o = original<D3dCreateDeviceFn>(self, idx(d3::D3DMethod::CreateDevice));
    COM_THREAD_OR(D3D_CreateDevice, o(self, a, t, focus, behavior, pp, out));
    const std::uint32_t check = check_of({a, t, hwnd_code(focus), behavior, check_pp(pp)});
    const HRESULT hr = o(self, a, t, focus, behavior, pp, out);
    if (replaying()) {
        HRESULT x = 0;
        if (play(Id::D3D_CreateDevice, check, x) && x != hr)
            diverge(Id::D3D_CreateDevice, "the Direct3D device could be created in one run only");
    } else {
        rec(Id::D3D_CreateDevice, check, hr);
    }
    if (hr == 0 && out && *out && mode() != Mode::Off) wrap_device(*out);
    return hr;
}

}  // namespace

// ---------------------------------------------------------------- internal API

// ---------------------------------------------------------------- the native keyboard / mouse (platform/input.hpp)

// value: the result (and for Read the items) of the platform layer's devices - the information the former DirectInput
// device wrappers recorded, at the native boundary. A replay never touches the real devices (no relative mouse mode).
int input_call(InputOp op, int device, void* items, unsigned* count, InputFn real) {
    if (mode() == Mode::Off || !on_main()) return real(device, items, count);
    constexpr std::uint32_t kItem = 0x14;  // sizeof(platform::input::Item)
    const std::uint32_t cap = count ? *count : 0u;
    switch (op) {
        case InputOp::Acquire:
            return value<int>(Id::IN_Acquire, static_cast<std::uint32_t>(device),
                              [&] { return real(device, nullptr, nullptr); });
        case InputOp::Unacquire:
            return value<int>(Id::IN_Unacquire, static_cast<std::uint32_t>(device),
                              [&] { return real(device, nullptr, nullptr); });
        case InputOp::Read: {
            const std::uint32_t check = check_of({static_cast<std::uint32_t>(device), cap});
            if (replaying()) {
                std::uint32_t n = 0;
                if (const std::uint8_t* d = play_raw(Id::IN_Read, check, n)) {
                    Cursor c(d, n);
                    const int r = c.get<int>();
                    const std::uint32_t got = c.get<std::uint32_t>();
                    c.get_blob(items, items ? cap * kItem : 0u);
                    if (count) *count = got;
                    return r;
                }
            }
            const int r = real(device, items, count);
            const std::uint32_t got = count ? *count : 0u;
            Bytes b;
            b.put(r).put(got).put_blob(items, items && got <= cap ? got * kItem : 0u);
            rec(Id::IN_Read, check, b);
            return r;
        }
        case InputOp::PadMask:
        case InputOp::PadRead: {
            // `items` = the result (the connected-pads mask / a pad's state), cap = its size in bytes
            const Id id = op == InputOp::PadMask ? Id::IN_PadMask : Id::IN_PadRead;
            const std::uint32_t check = check_of({static_cast<std::uint32_t>(device), cap});
            if (replaying()) {
                std::uint32_t n = 0;
                if (const std::uint8_t* d = play_raw(id, check, n)) {
                    Cursor c(d, n);
                    const int r = c.get<int>();
                    c.get_blob(items, items ? cap : 0u);
                    return r;
                }
            }
            const int r = real(device, items, count);
            Bytes b;
            b.put(r).put_blob(items, items ? cap : 0u);
            rec(id, check, b);
            return r;
        }
    }
    return real(device, items, count);
}

void wrap_direct3d(void* d3d) {
    patch(d3d, idx(d3::D3DMethod::GetAdapterCount), reinterpret_cast<void*>(&d3d_get_adapter_count));
    patch(d3d, idx(d3::D3DMethod::GetAdapterIdentifier), reinterpret_cast<void*>(&d3d_get_adapter_identifier));
    patch(d3d, idx(d3::D3DMethod::GetAdapterModeCount), reinterpret_cast<void*>(&d3d_get_adapter_mode_count));
    patch(d3d, idx(d3::D3DMethod::EnumAdapterModes), reinterpret_cast<void*>(&d3d_enum_adapter_modes));
    patch(d3d, idx(d3::D3DMethod::GetAdapterDisplayMode), reinterpret_cast<void*>(&d3d_get_adapter_display_mode));
    patch(d3d, idx(d3::D3DMethod::CheckDeviceType), reinterpret_cast<void*>(&d3d_check_device_type));
    patch(d3d, idx(d3::D3DMethod::CheckDeviceFormat), reinterpret_cast<void*>(&d3d_check_device_format));
    patch(d3d, idx(d3::D3DMethod::CheckDeviceMultiSampleType), reinterpret_cast<void*>(&d3d_check_multisample));
    patch(d3d, idx(d3::D3DMethod::CheckDepthStencilMatch), reinterpret_cast<void*>(&d3d_check_depth_stencil));
    patch(d3d, idx(d3::D3DMethod::GetDeviceCaps), reinterpret_cast<void*>(&d3d_get_device_caps));
    patch(d3d, idx(d3::D3DMethod::CreateDevice), reinterpret_cast<void*>(&d3d_create_device));
}

}  // namespace joan::replay
