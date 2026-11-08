// Original library sources (re/attribution.csv original_file, under lib/): imports/import_stubs.cpp, crt/strdup.c,
//   crt/validate.c, crt/winxfltr.c, d3dx8/d3dx8math.cpp, d3dx8/d3dx8tex.cpp, d3dx8/filemapping.cpp.
//
// Batch 116, task 15: the last statically linked library entries that still ran from joan.exe (runtime census of
// patch115) plus every library address our own C++ still calls that had no replacement.
//
// Modules: "d3dx/b116_15" (D3DX8 code), "lib/b116_15" (import stubs, C runtime helpers).
// Every function is entry-patched (library code has no incremental-link thunks); re/check_lib_entries.py passes
// for all of them.
//
// d3dx/b116_15 - D3DX maths, port P2: portable C++ transcriptions of the original x87 sequences on
// joan::fp (fp/pc24.hpp, the D3DX register model of lib/d3dx8/math_matrix.cpp: PC24 rounding with the register's
// extended exponent range, x87 NaN rules, fsincos with the FPU's 66-bit pi), one call per x87 operation, memory
// accesses in the original order. Proof: re/p2/stubs (README), bit-exact against the code in joan.exe.
//   0x00766ed8 D3DXMatrixScaling(out, sx, sy, sz)       stdcall, ret 0x10   (lib/d3dx8/objects.cpp ID3DXMatrixStack::Scale*)
//   0x0076893e D3DXMatrixRotationYawPitchRoll(out, y, p, r)  stdcall, ret 0x10   (lib/d3dx8/objects.cpp ::RotateYawPitchRoll*)
//              calls 0x00767aee (below, directly) and D3DXMatrixRotationQuaternion 0x00767101 (lib/d3dx8/math_matrix.cpp,
//              through its facade)
//   0x00767aee D3DXQuaternionRotationYawPitchRoll(out, y, p, r)  (fsincos x3): only the helper of 0x0076893e. Its
//              registration is gone (port P2): no caller of the address is left (no code reloc, no thunk, no fn:: /
//              kc_ use in src/; the original D3DX callers are not replaced, i.e. traps), so its slot is a trap.
//   0x00767a9b D3DXQuaternionLengthSq, 0x00767ac9 D3DXQuaternionDot (deleted, port P2): no caller at all
//              (lib/d3dx8/math_geometry.cpp inlines both, its slerp is C++ since P2; no code reloc / thunk / fn:: /
//              kc_ use).
// d3dx/b116_15 - C++:
//   0x007886c6 CFileMapping::CFileMapping()             thiscall, returns this (file / mapping = -1, view / size = 0)
//   0x007886d8 CFileMapping::Open(name, wide)           thiscall, ret 8, HRESULT
//   0x007887f1 CFileMapping::Close()                    thiscall, returns 0
//   0x00788832 CFileMapping::~CFileMapping()            thiscall (Close when the file handle is not -1)
//              (callers: lib/d3dx8/texture_from_file.cpp *FromFileExA, lib/d3dx8/shader_assembler.cpp has its own copy; the object layout is the
//              original 16 bytes because our callers allocate it)
// Deleted in port P2 (dead: no caller of any kind left): the register-convention D3DX texture requirement core
// 0x00764064 (eax = device; module fix/uninit) with its helpers format distance 0x00763fe4 (ecx / eax -> eax) and
// format table entry 0x00763fc0. lib/d3dx8/texture_api.cpp computes the requirements itself (req_compute, since
// P1 without a fallback into 0x00764064); its original callers are gone (the D3DX entries texture_api.cpp replaces)
// or traps (the unreferenced volume variant 0x007649a7, not replaced); no code reloc, thunk or fn:: / kc_ use of
// the three addresses outside this file: their only callers were each other.
// lib/b116_15:
//   9 import stubs `jmp dword ptr [IAT slot]` (6 bytes each) that our C++ calls: replaced by typed C++ calls through
//   the slot inside joan_image (P1; equivalence for every caller argued at the definitions), so whatever the slot
//   holds (the DLL, or our patch_import replacement) runs: comdlg32 GetOpenFileNameA 0x00763f4e (game/unknown_game_input.cpp),
//   d3d8 Direct3DCreate8 0x00763fba (game/unknown_d3dsys.cpp), the 7 OpenAL alc* stubs 0x007a9a36..0x007a9a5a
//   (lib/openal/alut.cpp); KERNEL32 RtlUnwind 0x007c5c28 went with its only caller _global_unwind2 (port round P2,
//   lib/crt/seh_unwind.cpp). The other 244 stubs of the
//   original blocks (0x00763d02..0x00763fb4, 0x007a9396..0x007a9a30, 0x007b8d28 / 0x007b8d2e __threadid /
//   __threadhandle, 0x007c5be0..0x007c5dba) have no caller and no stored code pointer: deleted in cleanup C1. Gone
//   with their APIs: 0x007a8940 DirectInput8Create (cleanup C1) and 0x007a9390 DirectSoundCreate (cleanup C2; both
//   were in lib/d3dx8/objects.cpp). Not stubs: 0x007b502c / 0x007b5041 (tail jumps inside getSystemCP, not entries).
//   0x007c5afa _strdup(s)              cdecl: strlen 0x7b1ef0 + malloc 0x7b6c54 + strcpy 0x7b2020 at their addresses
//                                      (called by game_b103_08)
//   0x007c1d9e __pxcptinfoptrs()       dead, deleted in port round P2: its only caller is the original _fpreset
//                                      0x7b78c5, which is not taken (no call, jump or pointer: lib/crt/startup.cpp);
//                                      no C++ caller, no stored code pointer
//   0x007c2174 _ValidateExecute(fp)    cdecl: IsBadCodePtr(fp) ? 0 : 1 through IAT 0x00a6da20 (lib/crt/exit.cpp)
//
// CFileMapping::Open deviation (unreachable on Windows NT): on a non-NT platform with a wide name, the original
// converts the name into an _alloca buffer; here a 1 KiB local buffer, or a std::vector for longer names. When
// WideCharToMultiByte fails (0) the original opens whatever _alloca(0) leaves at the stack top; here "". No
// fix/uninit registration for this: GetVersionExA reports VER_PLATFORM_WIN32_NT (2) on every system the hook
// runs on, so the converted path is never taken.

#include "../../callconv.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include "../../original/direct.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/bind.hpp"
#include "../../fp/pc24.hpp"
namespace {

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::cdecl_call;



template <class R = void, class... A>
R import_call(img::Addr slot, A... args) {  // stdcall through a joan.exe IAT slot
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(img::global<const std::uintptr_t>(slot))(args...);
}
template <class R = void, class... A>
R cdecl_import(img::Addr slot, A... args) {  // cdecl through a joan.exe IAT slot
    return reinterpret_cast<R(JOAN_CDECL*)(A...)>(img::global<const std::uintptr_t>(slot))(args...);
}


// ================================================================================ D3DX math (port P2)
// ---- d3dx math begin (re/p2/stubs/build.sh compiles this block into the harness)
namespace d3dx_math {

using namespace joan::fp;
// glibc declares C23's float fadd / fsub / fmul / fdiv / fsqrt (double, double) and sincos globally: ours here.
using joan::fp::fadd;
using joan::fp::fdiv;
using joan::fp::fmul;
using joan::fp::fsqrt;
using joan::fp::sincos;
using joan::fp::fsub;
using u32 = std::uint32_t;

constexpr u32 kHalf = 0x3f000000u;  // 0.5f, .rdata 0x009e2ff0

// 0x00766ed8 D3DXMatrixScaling(out, sx, sy, sz), stdcall, returns out. The scales are copied as dwords (mov, no
// x87 load: an SNaN stays signalling); the factors are bit patterns, as the only caller (lib/d3dx8/objects.cpp
// three_float) passes them (u32: the same argument registers as the caller's on x86_64 too).
void* JOAN_STDCALL matrix_scaling(void* out, u32 sx, u32 sy, u32 sz) {
    wr(out, 0x38, 0x00000000u);
    wr(out, 0x0, sx);
    wr(out, 0x34, 0x00000000u);
    wr(out, 0x14, sy);
    wr(out, 0x30, 0x00000000u);
    wr(out, 0x28, sz);
    wr(out, 0x2c, 0x00000000u);
    wr(out, 0x24, 0x00000000u);
    wr(out, 0x20, 0x00000000u);
    wr(out, 0x1c, 0x00000000u);
    wr(out, 0x18, 0x00000000u);
    wr(out, 0x10, 0x00000000u);
    wr(out, 0xc, 0x00000000u);
    wr(out, 0x8, 0x00000000u);
    wr(out, 0x4, 0x00000000u);
    wr(out, 0x3c, 0x3f800000u);
    return out;
}

// 0x00767aee D3DXQuaternionRotationYawPitchRoll(out, yaw, pitch, roll) (angles as bit patterns). Each half angle is
// stored as a float (fstp dword) before fsincos; the x87 stack is empty at every fsincos (|x| >= 2^63: see
// fp::sincos). sin(roll / 2) lands in the roll argument slot and is read back from there, as a float either way.
void quaternion_rotation_ypr(void* out, u32 yaw, u32 pitch, u32 roll) {
    u32 sr, cr, sp, cp, sy, cy;
    sincos(st(fmul_m(ld(roll), kHalf)), sr, cr);
    sincos(st(fmul_m(ld(pitch), kHalf)), sp, cp);
    sincos(st(fmul_m(ld(yaw), kHalf)), sy, cy);
    const R a = fmul_m(ld(sy), cp);  // sin y * cos p
    const R b = fmul_m(ld(cy), sp);  // cos y * sin p
    wr(out, 0x0, st(fadd(fmul(ld(cr), b), fmul_m(a, sr))));
    wr(out, 0x4, st(fsub(fmul_m(a, cr), fmul_m(b, sr))));
    const R g = fmul_m(ld(cy), cp);  // cos y * cos p
    const R h = fmul_m(ld(sy), sp);  // sin y * sin p
    wr(out, 0x8, st(fsub(fmul_m(g, sr), fmul_m(h, cr))));
    wr(out, 0xc, st(fadd(fmul_m(h, sr), fmul_m(g, cr))));
}

// 0x0076893e D3DXMatrixRotationYawPitchRoll(out, yaw, pitch, roll), stdcall, returns out (angles as bit patterns,
// see matrix_scaling). The angles reach 0x00767aee through fld / fstp dword (an SNaN quietened), the quaternion is
// a local; D3DXMatrixRotationQuaternion 0x00767101 through its facade (lib/d3dx8/math_matrix.cpp).
void* JOAN_STDCALL matrix_rotation_ypr(void* out, u32 yaw, u32 pitch, u32 roll) {
    u32 q[4];
    quaternion_rotation_ypr(q, quiet(yaw), quiet(pitch), quiet(roll));
    joan::orig::d3dx::matrix_rotation_quaternion(out, q);
    return out;
}

JOAN_REPLACE("d3dx/b116_15", "D3DXMatrixScaling", 0x00766ed8, &matrix_scaling);
JOAN_REPLACE("d3dx/b116_15", "D3DXMatrixRotationYawPitchRoll", 0x0076893e, &matrix_rotation_ypr);

}  // namespace d3dx_math
// ---- d3dx math end

// ================================================================================ CFileMapping 0x007886c6..

// IAT slots (KERNEL32)
constexpr img::Addr kWideCharToMultiByte = d::iat_KERNEL32_WideCharToMultiByte;
constexpr img::Addr kGetFileSize = d::iat_KERNEL32_GetFileSize;
constexpr img::Addr kCloseHandle = d::iat_KERNEL32_CloseHandle;
constexpr img::Addr kCreateFileA = d::iat_KERNEL32_CreateFileA;
constexpr img::Addr kGetVersionExA = d::iat_KERNEL32_GetVersionExA;
constexpr img::Addr kCreateFileW = d::iat_KERNEL32_CreateFileW;
constexpr img::Addr kUnmapViewOfFile = d::iat_KERNEL32_UnmapViewOfFile;
constexpr img::Addr kGetLastError = d::iat_KERNEL32_GetLastError;
constexpr img::Addr kIsBadCodePtr = d::iat_KERNEL32_IsBadCodePtr;
constexpr img::Addr kCreateFileMappingA = d::iat_KERNEL32_CreateFileMappingA;
constexpr img::Addr kMapViewOfFile = d::iat_KERNEL32_MapViewOfFile;

constexpr i32 kInvalidCall = static_cast<i32>(0x8876086cu);   // D3DERR_INVALIDCALL
// D3DX8 CFileMapping (the *FromFileEx wrappers' stack object, lib/d3dx8/texture_from_file.cpp declares the same):
// +0 file HANDLE, +4 mapping HANDLE (-1 = none), +8 view, +0xc size; x64: pointer-sized handles and view.
struct Mapping {
    std::uintptr_t file;
    std::uintptr_t mapping;
    void* view;
    u32 size;
};
static_assert(sizeof(void*) != 4 || sizeof(Mapping) == 0x10, "CFileMapping is 16 bytes");
constexpr std::uintptr_t kNone = ~std::uintptr_t{0};  // INVALID_HANDLE_VALUE
Mapping* mapping_of(void* self) { return static_cast<Mapping*>(self); }

// The thiscall methods are written as __fastcall free functions (ecx = self, edx unused, stack arguments popped by
// the callee): the same contract as thiscall, and a plain function pointer for the registry under every toolchain.
// x64 and Linux have one calling convention (self first, then the arguments): no edx placeholder there, or every argument after
// self would arrive one register late (thiscall<>(kMappingOpen, &map, file, 0) passes the name in rdx).
#if defined(JOAN_X86_CONVENTIONS)
#define JOAN_EDX_ARG , void* /*edx*/
#define JOAN_EDX_VAL , nullptr
#else
#define JOAN_EDX_ARG
#define JOAN_EDX_VAL
#endif

// 0x007886c6 CFileMapping::CFileMapping(), returns this.
void* JOAN_FASTCALL mapping_ctor(void* self JOAN_EDX_ARG) {
    Mapping* const m = mapping_of(self);
    m->file = kNone;
    m->mapping = kNone;
    m->view = nullptr;
    m->size = 0;
    return self;
}

// 0x007886d8 CFileMapping::Open(name, wide), ret 8.
i32 JOAN_FASTCALL mapping_open(void* self JOAN_EDX_ARG, const void* name, i32 wide) {
    if (name == nullptr) return kInvalidCall;
    char local[1024];
    std::vector<char> big;
    if (wide != 0) {
        struct {
            u32 size, major, minor, build, platform;
            char csd[128];
        } vi = {};
        vi.size = 0x94;
        import_call<i32>(kGetVersionExA, &vi);
        if (vi.platform != 2) {  // not VER_PLATFORM_WIN32_NT: convert (unreachable on NT, see the header)
            const i32 n = import_call<i32>(kWideCharToMultiByte, 0u, 0u, name, -1, static_cast<char*>(nullptr), 0,
                                           static_cast<const char*>(nullptr), static_cast<i32*>(nullptr));
            char* buf = local;
            if (n > static_cast<i32>(sizeof local)) {
                big = std::vector<char>(static_cast<std::size_t>(n));
                buf = big.data();
            }
            buf[0] = 0;
            import_call<i32>(kWideCharToMultiByte, 0u, 0u, name, -1, buf, n, static_cast<const char*>(nullptr),
                             static_cast<i32*>(nullptr));
            name = buf;
            wide = 0;
        }
    }
    Mapping* const fm = mapping_of(self);
    std::uintptr_t h;
    if (wide != 0)
        h = import_call<std::uintptr_t>(kCreateFileW, name, 0x80000000u, 1u, static_cast<void*>(nullptr), 3u, 0x10000000u,
                             static_cast<void*>(nullptr));
    else
        h = import_call<std::uintptr_t>(kCreateFileA, name, 0x80000000u, 1u, static_cast<void*>(nullptr), 3u, 0x10000000u,
                             static_cast<void*>(nullptr));
    fm->file = h;
    if (h != kNone) {
        const std::uintptr_t m = import_call<std::uintptr_t>(kCreateFileMappingA, h, static_cast<void*>(nullptr), 2u, 0u, 0u,
                                       static_cast<const char*>(nullptr));
        fm->mapping = m;
        if (m != kNone) {  // a NULL mapping passes, as in the original (MapViewOfFile then fails)
            void* const v = import_call<void*>(kMapViewOfFile, m, 4u, 0u, 0u, std::size_t{0});  // SIZE_T bytes: 8 on x64
            fm->view = v;
            if (v != nullptr) {
                const u32 size = import_call<u32>(kGetFileSize, h, static_cast<u32*>(nullptr));
                fm->size = size;
                if (size != 0xffffffffu) return 0;  // INVALID_FILE_SIZE
            }
        }
    }
    const i32 e = import_call<i32>(kGetLastError);
    if (e > 0) return static_cast<i32>((static_cast<u32>(e) & 0xffff) | 0x80070000u);
    return e;
}

// 0x007887f1 CFileMapping::Close(), returns 0.
i32 JOAN_FASTCALL mapping_close(void* self JOAN_EDX_ARG) {
    Mapping* const m = mapping_of(self);
    if (m->view != nullptr) {
        import_call<i32>(kUnmapViewOfFile, m->view);
        m->view = nullptr;
        m->size = 0;
    }
    if (m->mapping != kNone) {
        import_call<i32>(kCloseHandle, m->mapping);
        m->mapping = kNone;
    }
    if (m->file != kNone) {
        import_call<i32>(kCloseHandle, m->file);
        m->file = kNone;
    }
    return 0;
}

// 0x00788832 CFileMapping::~CFileMapping(): Close when the file handle is not -1 (the original tail-jumps to Close
// at its address; eax is not a result).
void JOAN_FASTCALL mapping_dtor(void* self JOAN_EDX_ARG) {
    if (mapping_of(self)->file != kNone) reinterpret_cast<i32(JOAN_FASTCALL*)(void* JOAN_EDX_ARG)>(fn::code_value(fn::CFileMapping_Close))(self JOAN_EDX_VAL);
}

// ================================================================================ C runtime helpers

// 0x007c5afa _strdup(s), cdecl.
char* JOAN_CDECL crt_strdup(const char* s) {
    if (s == nullptr) return nullptr;
    const u32 n = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s)));  // strlen
    char* const p = joan::orig::arg::as<char*>(joan::orig::malloc_(joan::orig::arg::u32(n + 1)));      // malloc
    if (p == nullptr) return nullptr;
    return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(p)), joan::orig::arg::str(s));  // strcpy
}

// 0x007c2174 _ValidateExecute(fp), cdecl: 1 when IsBadCodePtr says the address is executable.
i32 JOAN_CDECL validate_execute(const void* fp) {
    i32 ok = 1;
    if (import_call<i32>(kIsBadCodePtr, fp) != 0) ok = 0;
    return ok;
}

// ================================================================================ registrations

#define D(name, addr, fn) JOAN_REPLACE("d3dx/b116_15", name, addr, fn)
D("CFileMapping::CFileMapping", 0x007886c6, &mapping_ctor);
D("CFileMapping::Open", 0x007886d8, &mapping_open);
D("CFileMapping::Close", 0x007887f1, &mapping_close);
D("CFileMapping::~CFileMapping", 0x00788832, &mapping_dtor);
#undef D

#define L(name, addr, fn) JOAN_REPLACE("lib/b116_15", name, addr, fn)
L("_strdup", 0x007c5afa, &crt_strdup);
L("_ValidateExecute", 0x007c2174, &validate_execute);
#undef L

// Import stubs (joan.exe's `jmp dword ptr [IAT slot]`, 6 bytes): a typed C++ call through the IAT slot (in
// joan_image), so whatever the slot holds (the DLL, or a patch_import replacement) runs. Equivalent to the jump for
// every caller: all of them are our C++ calling through the stub's code slot with the import's convention
// (src/original/gen/free_functions.hpp: stdcall / cdecl helpers), none reads ecx / edx / flags after the call
// (caller-saved), the callee-saved registers survive both the import and this frame, and the return register is
// passed on unchanged (the result types are the callers': a void / ALCboolean import's eax reaches them as the jump
// left it). The imports see one more return address and a few bytes more stack; none of them looks at its caller.
i32 JOAN_STDCALL get_open_file_name_a(void* ofn) { return import_call<i32>(img::Addr{0x00a6dd54u}, ofn); }  // comdlg32
void* JOAN_STDCALL direct3d_create8(u32 sdk) { return import_call<void*>(d::iat_d3d8_Direct3DCreate8, sdk); }
// OpenAL32 (ALC_APIENTRY = __cdecl)
// ALCboolean results come back in al only: on x86-64 the upper bits of eax are the callee's leftovers, so the byte is
// widened there (x86 keeps the plain tail jump).
#if UINTPTR_MAX == 0xffffffffu
using AlcBool = i32;
#else
using AlcBool = std::uint8_t;
#endif
i32 alc_make_context_current(void* ctx) { return cdecl_import<AlcBool>(img::Addr{0x00a6dbc4u}, ctx); }
void* alc_create_context(void* dev, const i32* attrs) { return cdecl_import<void*>(img::Addr{0x00a6dbc8u}, dev, attrs); }
void* alc_open_device(const char* name) { return cdecl_import<void*>(img::Addr{0x00a6dbccu}, name); }
i32 alc_close_device(void* dev) { return cdecl_import<AlcBool>(img::Addr{0x00a6dbd0u}, dev); }
i32 alc_destroy_context(void* ctx) { return cdecl_import<i32>(img::Addr{0x00a6dbd4u}, ctx); }
void* alc_get_contexts_device(void* ctx) { return cdecl_import<void*>(img::Addr{0x00a6dbd8u}, ctx); }
void* alc_get_current_context() { return cdecl_import<void*>(img::Addr{0x00a6dbdcu}); }

#define S(name, addr, fn) JOAN_REPLACE("lib/b116_15", "stub " name, addr, fn)
S("comdlg32 GetOpenFileNameA", 0x00763f4e, &get_open_file_name_a);  // IAT 0x00a6dd54
S("d3d8 Direct3DCreate8", 0x00763fba, &direct3d_create8);  // IAT 0x00a6dd84
S("OpenAL32 alcMakeContextCurrent", 0x007a9a36, &alc_make_context_current);  // IAT 0x00a6dbc4
S("OpenAL32 alcCreateContext", 0x007a9a3c, &alc_create_context);  // IAT 0x00a6dbc8
S("OpenAL32 alcOpenDevice", 0x007a9a42, &alc_open_device);  // IAT 0x00a6dbcc
S("OpenAL32 alcCloseDevice", 0x007a9a48, &alc_close_device);  // IAT 0x00a6dbd0
S("OpenAL32 alcDestroyContext", 0x007a9a4e, &alc_destroy_context);  // IAT 0x00a6dbd4
S("OpenAL32 alcGetContextsDevice", 0x007a9a54, &alc_get_contexts_device);  // IAT 0x00a6dbd8
S("OpenAL32 alcGetCurrentContext", 0x007a9a5a, &alc_get_current_context);  // IAT 0x00a6dbdc
#undef S

}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::_strdup(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_strdup>(a0)); }  // 0x7c5afa
