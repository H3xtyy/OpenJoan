// Original library sources (re/attribution.csv original_file, under lib/): d3dx8/d3dx8err.cpp, d3dx8/d3dxbuffer.cpp,
//   d3dx8/matrixstack.cpp, imports/import_stubs.cpp, openal/alut.c.
//
// Batch 114 / list 17: D3DX objects and library leftovers.
//
// Module "d3dx/objects" (D3DX8 COM objects + error strings):
//   0x00769dc0 D3DXCreateMatrixStack(DWORD flags, ID3DXMatrixStack** pp)         stdcall ret 8
//              Creates OUR object (same COM layout: +0 vtable, +4 capacity, +8 matrix array
//              (64 bytes each), +0xc top index, +0x10 refcount; 0x14 bytes from operator new
//              0x007b1d35) with OUR 18-slot vtable (order of the original 0x009e3328). The
//              original object code (ctor 0x00769da5, Init 0x00769921, methods 0x00769873..
//              0x00769d47, dtors 0x0076985e / 0x00769897) is reachable only through 0x00769dc0,
//              so replacing the creator retires all of it (the original vtable is only stored by
//              the original ctor / dtor). Surface: 0x00769dc0 (callers: game D3D::init 0x005f6b60
//              = our game_b108_11.cpp; game 0x005f6bac).
//              Methods (stdcall, this first):  0 QueryInterface (IID_ID3DXMatrixStack
//              {E3357330-CC5E-11d2-A434-00A0C90629A8} @0x009e1638, IID_IUnknown @0x009df9a8),
//              1 AddRef, 2 Release, 3 Pop, 4 Push, 5 LoadIdentity (game thunk 0x00402e9b ->
//              0x005befa0 D3DXMatrixIdentity), 6 LoadMatrix, 7 MultMatrix, 8 MultMatrixLocal,
//              9 RotateAxis, 10 RotateAxisLocal, 11 RotateYawPitchRoll, 12 ..Local, 13 Scale,
//              14 ScaleLocal, 15 Translate, 16 TranslateLocal, 17 GetTop. The matrix maths is
//              called at the original D3DX addresses (D3DXMatrixMultiply 0x007669fb, RotationAxis
//              0x0076883c, RotationYawPitchRoll 0x0076893e, Scaling 0x00766ed8, Translation
//              0x00766f34; all stdcall), so it chains to the d3dx/b113_0x replacements. Float
//              arguments go through an x87 fld/fstp dword like the original (SNaN quieting and
//              the IE / DE status flags identical).
//   ID3DXBuffer methods (vtables 0x009e3d0c plain / 0x009e3d24 growable, both use the same five
//              methods; objects are created by the original shader assembler 0x0078bed6 via ctors
//              0x0078c5d5 / 0x0078c5ed and keep their original layout +0 vtable, +4 refcount,
//              +8 size, +0xc data[, +0x10 / +0x14 write cursors]):
//              0x0078c549 QueryInterface (IID_IUnknown, IID_ID3DXBuffer @0x009e1608
//              {932E6A7E-C68E-45dd-A7BF-53D19C86DB1F}; *ppv = NULL first)   ret 0xc
//              0x0078c3a8 AddRef ret 4, 0x0078c590 Release ret 4 (base dtor 0x0078c36d: vtable
//              = 0x009e3d0c, operator delete(+0xc); then operator delete(this)),
//              0x0078c3b5 GetBufferPointer ret 4, 0x0078c3bf GetBufferSize ret 4.
//              No exported D3DXCreateBuffer is reachable (0x0078c603 is the assembler's private
//              creator; 0x0076fc48 is mesh code, unreachable).
//   0x007693c4 D3DXGetErrorStringA(hr, buf, size) stdcall ret 0xc - registered here; the C++
//              is joan::lib_b113_04::d3dx_get_error_string_a in lib/d3dx8/helpers.cpp (re-checked
//              against the asm: 30 equality cases + S_OK, VC6 _snprintf truncation rule).
//
// Module "lib/misc":
//   (The libjpeg compression API and D3DX8's jerror, 0x007a9a60.. / 0x00765e32.., moved to
//   lib/jpeg/compress_api.cpp, typed.)
//   0x007a9989 alutUnloadWAV(format, data, size, freq): if (data) free(data) 0x007b4e1d.
//   (The import stubs went with their APIs: DirectInput8Create 0x007a8940 in cleanup C1, DirectSoundCreate
//   0x007a9390 in cleanup C2 - DSound::create 0x0057a740, ilib/audio.cpp, makes no DirectSound device.)
//
// Left original (scoped, not exact-in-one-go):
//   - D3DXAssembleShaderFromFileA 0x00769163 (game 0x005bd620 / 0x006013ea = our game_b102_10 /
//     game_b102_11, item\*.vsc vertex shaders): front end = CFileMapping + an EH-framed 0x3c0-byte
//     assembler object (ctor 0x00789db6, Assemble 0x0078bed6, dtor 0x00789e56) - the D3DX8
//     vs/ps assembler itself is ~17 KB (0x00789000..0x0078d000: lexer, parser, instruction
//     tables, ID3DXBuffer output via 0x0078c603). Needs its own task (exact vs.1.x assembler with
//     D3DX8's error text / constant-buffer output) or an API-level replacement that returns
//     pre-assembled tokens for the shipped .vsc files.
//   - _purecall: family crt/exit (l114_02).
//
#include "../../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "../../original/direct.hpp"
#include "../../lib/crt/crt.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/bind.hpp"
#include "../../fp/pc24.hpp"
#include "../../fp/bits.hpp"

namespace joan::lib_b113_04 {
std::int32_t JOAN_STDCALL d3dx_get_error_string_a(std::int32_t hr, char* buf, std::uint32_t size);
}

namespace {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::cdecl_call;
using joan::call::stdcall_call;
// Field access by offset (unaligned-safe).
template <class T>
T ld(const void* p, u32 off) {
    T v;
    std::memcpy(&v, static_cast<const u8*>(p) + off, sizeof(T));
    return v;
}
template <class T>
void st(void* p, u32 off, T v) {
    std::memcpy(static_cast<u8*>(p) + off, &v, sizeof(T));
}
template <class F>
F fn(const void* table, u32 off) {
    return ld<F>(table, off);
}

// ---- CRT entry points (called at their addresses; whichever implementation is installed runs)
void* op_new(u32 n) { return joan::crt::operator_new(n); }
void op_delete(void* p) { joan::crt::operator_delete(p); }

// fld dword / fstp dword round trip, exactly what the original does with every float argument it
// forwards: an SNaN is quietened, every other bit pattern (denormals included) is kept. The x87 status flags the
// original instructions set (IE / DE) are not reproduced; nothing reads them. Checked against the CPU for all 2^32
// patterns (re/p1/d3dx).
u32 x87_float(u32 bits) { return joan::fp::quiet(bits); }

bool guid_eq(const void* a, const u32* b) {
    for (u32 i = 0; i < 4; ++i)
        if (ld<u32>(a, i * 4) != b[i]) return false;
    return true;
}
constexpr u32 kIidUnknown[4] = {0x00000000u, 0x00000000u, 0x000000c0u, 0x46000000u};  // 0x009df9a8
constexpr u32 kIidMatrixStack[4] = {0xe3357330u, 0x11d2cc5eu, 0xa00034a4u, 0xa82906c9u};  // 0x009e1638
constexpr u32 kIidBuffer[4] = {0x932e6a7eu, 0x45ddc68eu, 0xd153bfa7u, 0x1fdb869cu};  // 0x009e1608

constexpr i32 kInvalidCall = static_cast<i32>(0x8876086cu);
constexpr i32 kOutOfMemory = static_cast<i32>(0x8007000eu);
constexpr i32 kNoInterface = static_cast<i32>(0x80004002u);

// =============================================================================================
// ID3DXMatrixStack (our object, original layout)
// =============================================================================================
struct Stack {
    const std::uintptr_t* vtbl;  // +0
    i32 capacity;             // +4
    u8* matrices;             // +8 (64 bytes each)
    u32 top;                  // +0xc
    i32 refs;                 // +0x10
};
static_assert(sizeof(void*) != 4 || sizeof(Stack) == 0x14, "ID3DXMatrixStack object layout");  // x64: natural layout

JOAN_COUNTER(c_create_stack, "D3DXCreateMatrixStack");
JOAN_COUNTER(c_stack_push, "ID3DXMatrixStack::Push");
JOAN_COUNTER(c_stack_mult, "ID3DXMatrixStack::Mult*/Rotate*/Scale*/Translate*");

u8* top_of(Stack* s) { return s->matrices + (s->top << 6); }
void copy_matrix(void* dst, const void* src) {
    for (u32 i = 0; i < 16; ++i) st<u32>(dst, i * 4, ld<u32>(src, i * 4));
}
u32 vcall1(Stack* s, u32 slot) {  // stdcall method(this) through the object's vtable
    return reinterpret_cast<u32(JOAN_STDCALL*)(Stack*)>(s->vtbl[slot])(s);
}
// top = top * m (local == false) or m * top (local == true), via D3DXMatrixMultiply 0x007669fb.
void mult_top(Stack* s, const void* m, bool local) {
    u8 tmp[64];
    if (local)
        joan::orig::d3dx::matrix_multiply(joan::orig::arg::mptr(static_cast<void*>(tmp)), joan::orig::arg::mptr(m), joan::orig::arg::mptr(static_cast<const void*>(top_of(s))));
    else
        joan::orig::d3dx::matrix_multiply(joan::orig::arg::mptr(static_cast<void*>(tmp)), joan::orig::arg::mptr(static_cast<const void*>(top_of(s))), joan::orig::arg::mptr(m));
    copy_matrix(top_of(s), tmp);
}

extern const std::uintptr_t kStackVtbl[18];

// 0x00769957
i32 JOAN_STDCALL stack_qi(Stack* self, const void* riid, void** ppv) {
    if (ppv == nullptr) return kInvalidCall;
    if (!guid_eq(riid, kIidMatrixStack) && !guid_eq(riid, kIidUnknown)) return kNoInterface;
    *ppv = self;
    vcall1(self, 1);  // AddRef
    return 0;
}
// 0x00769873
u32 JOAN_STDCALL stack_addref(Stack* self) { return static_cast<u32>(++self->refs); }
// 0x007699a6 (dtor 0x0076985e + operator delete)
u32 JOAN_STDCALL stack_release(Stack* self) {
    const i32 r = --self->refs;
    if (r == 0) {
        self->vtbl = kStackVtbl;  // the dtor's vtable store (dead, kept for symmetry)
        if (self->matrices != nullptr) op_delete(self->matrices);
        op_delete(self);
    }
    return static_cast<u32>(r);
}
// 0x007698b3
i32 JOAN_STDCALL stack_pop(Stack* self) {
    if (self->top != 0) --self->top;
    return 0;
}
// 0x007699cb
i32 JOAN_STDCALL stack_push(Stack* self) {
    c_stack_push.hit();
    ++self->top;
    if (!(static_cast<i32>(self->top) < self->capacity)) {
        u8* const grown = static_cast<u8*>(op_new(static_cast<u32>(self->capacity) * 2u << 6));
        if (grown == nullptr) return kOutOfMemory;  // top stays incremented (as in the original)
        if (self->matrices != nullptr) {
            const u32 n = static_cast<u32>(self->capacity) << 6;
            for (u32 i = 0; i < n; ++i) grown[i] = self->matrices[i];
            op_delete(self->matrices);
        }
        self->capacity <<= 1;
        self->matrices = grown;
    }
    u8* const dst = top_of(self);
    copy_matrix(dst, dst - 0x40);
    return 0;
}
// 0x00769a42
i32 JOAN_STDCALL stack_load_identity(Stack* self) {
    joan::orig::mat_identity(joan::orig::arg::mptr(static_cast<void*>(top_of(self))));  // 0x005befa0 D3DXMatrixIdentity
    return 0;
}
// 0x007698c7
i32 JOAN_STDCALL stack_load_matrix(Stack* self, const void* m) {
    copy_matrix(top_of(self), m);
    return 0;
}
// 0x00769a5b / 0x00769a95
i32 JOAN_STDCALL stack_mult(Stack* self, const void* m) {
    c_stack_mult.hit();
    mult_top(self, m, false);
    return 0;
}
i32 JOAN_STDCALL stack_mult_local(Stack* self, const void* m) {
    c_stack_mult.hit();
    mult_top(self, m, true);
    return 0;
}
// 0x00769acf / 0x00769b20 (D3DXMatrixRotationAxis 0x0076883c)
i32 rotate_axis(Stack* self, const void* axis, u32 angle, bool local) {
    c_stack_mult.hit();
    u8 r[64];
    joan::orig::d3dx::matrix_rotation_axis(joan::orig::arg::mptr(static_cast<void*>(r)), joan::orig::arg::mptr(axis), joan::orig::arg::f32(x87_float(angle)));
    mult_top(self, r, local);
    return 0;
}
i32 JOAN_STDCALL stack_rotate_axis(Stack* self, const void* axis, u32 angle) { return rotate_axis(self, axis, angle, false); }
i32 JOAN_STDCALL stack_rotate_axis_local(Stack* self, const void* axis, u32 angle) { return rotate_axis(self, axis, angle, true); }
// A builder argument as the builder's replacement declares it: the bit pattern (u32: D3DXMatrixScaling /
// RotationYawPitchRoll, lib/imports/import_stubs.cpp) or the float of it (D3DXMatrixTranslation, math_matrix.cpp);
// x86_64 passes the two in different registers, x86 puts the same 4 bytes on the stack.
template <class F>
F builder_arg(u32 bits) {
    if constexpr (std::is_same_v<F, float>) return joan::fp::fbits_to_float(bits);
    else return bits;
}
// (fn)(&r, a, b, c) for the three-float D3DX builders; the original converts c, b, a in that order.
template <class F>
i32 three_float(Stack* self, std::uintptr_t builder, u32 a, u32 b, u32 c, bool local) {
    c_stack_mult.hit();
    u8 r[64];
    const u32 fc = x87_float(c);
    const u32 fb = x87_float(b);
    const u32 fa = x87_float(a);
    stdcall_call<void*>(builder, static_cast<void*>(r), builder_arg<F>(fa), builder_arg<F>(fb), builder_arg<F>(fc));
    mult_top(self, r, local);
    return 0;
}
// 0x00769b71 / 0x00769bcf (D3DXMatrixRotationYawPitchRoll 0x0076893e)
i32 JOAN_STDCALL stack_ypr(Stack* self, u32 y, u32 p, u32 r) { return three_float<u32>(self, fn::code_value(fn::D3DXMatrixRotationYawPitchRoll), y, p, r, false); }
i32 JOAN_STDCALL stack_ypr_local(Stack* self, u32 y, u32 p, u32 r) { return three_float<u32>(self, fn::code_value(fn::D3DXMatrixRotationYawPitchRoll), y, p, r, true); }
// 0x00769c2d / 0x00769c8b (D3DXMatrixScaling 0x00766ed8)
i32 JOAN_STDCALL stack_scale(Stack* self, u32 x, u32 y, u32 z) { return three_float<u32>(self, fn::code_value(fn::D3DXMatrixScaling), x, y, z, false); }
i32 JOAN_STDCALL stack_scale_local(Stack* self, u32 x, u32 y, u32 z) { return three_float<u32>(self, fn::code_value(fn::D3DXMatrixScaling), x, y, z, true); }
// 0x00769ce9 / 0x00769d47 (D3DXMatrixTranslation 0x00766f34)
i32 JOAN_STDCALL stack_translate(Stack* self, u32 x, u32 y, u32 z) { return three_float<float>(self, fn::code_value(fn::D3DXMatrixTranslation), x, y, z, false); }
i32 JOAN_STDCALL stack_translate_local(Stack* self, u32 x, u32 y, u32 z) { return three_float<float>(self, fn::code_value(fn::D3DXMatrixTranslation), x, y, z, true); }
// 0x007698e6
void* JOAN_STDCALL stack_get_top(Stack* self) { return top_of(self); }

const std::uintptr_t kStackVtbl[18] = {
    reinterpret_cast<std::uintptr_t>(&stack_qi),
    reinterpret_cast<std::uintptr_t>(&stack_addref),
    reinterpret_cast<std::uintptr_t>(&stack_release),
    reinterpret_cast<std::uintptr_t>(&stack_pop),
    reinterpret_cast<std::uintptr_t>(&stack_push),
    reinterpret_cast<std::uintptr_t>(&stack_load_identity),
    reinterpret_cast<std::uintptr_t>(&stack_load_matrix),
    reinterpret_cast<std::uintptr_t>(&stack_mult),
    reinterpret_cast<std::uintptr_t>(&stack_mult_local),
    reinterpret_cast<std::uintptr_t>(&stack_rotate_axis),
    reinterpret_cast<std::uintptr_t>(&stack_rotate_axis_local),
    reinterpret_cast<std::uintptr_t>(&stack_ypr),
    reinterpret_cast<std::uintptr_t>(&stack_ypr_local),
    reinterpret_cast<std::uintptr_t>(&stack_scale),
    reinterpret_cast<std::uintptr_t>(&stack_scale_local),
    reinterpret_cast<std::uintptr_t>(&stack_translate),
    reinterpret_cast<std::uintptr_t>(&stack_translate_local),
    reinterpret_cast<std::uintptr_t>(&stack_get_top),
};

// 0x00769dc0 D3DXCreateMatrixStack (flags unused)
i32 JOAN_STDCALL create_matrix_stack(u32 /*flags*/, Stack** pp) {
    c_create_stack.hit();
    if (pp == nullptr) return kInvalidCall;
    Stack* const s = static_cast<Stack*>(op_new(static_cast<u32>(sizeof(Stack))));  // 0x14
    if (s == nullptr) return kOutOfMemory;
    s->vtbl = kStackVtbl;  // ctor 0x00769da5
    s->matrices = nullptr;
    s->capacity = 0;
    s->top = 0;
    s->refs = 1;
    // Init(16) 0x00769921
    i32 hr;
    s->matrices = static_cast<u8*>(op_new(16u << 6));
    if (s->matrices == nullptr) {
        hr = kOutOfMemory;
    } else {
        s->top = 0;
        s->capacity = 16;
        vcall1(s, 5);  // LoadIdentity through the vtable
        hr = 0;
    }
    if (hr < 0) {
        vcall1(s, 2);  // Release
        return hr;
    }
    *pp = s;
    return 0;
}

// =============================================================================================
// ID3DXBuffer methods (objects of the original assembler, original layout)
// =============================================================================================
// 0x0078c549
i32 JOAN_STDCALL buffer_qi(u8* self, const void* riid, void** ppv) {
    *ppv = nullptr;
    if (!guid_eq(riid, kIidUnknown) && !guid_eq(riid, kIidBuffer)) return kNoInterface;
    *ppv = self;
    const std::uintptr_t* const vt = ld<const std::uintptr_t*>(self, 0);
    reinterpret_cast<u32(JOAN_STDCALL*)(void*)>(vt[1])(self);  // AddRef through the vtable
    return 0;
}
// 0x0078c3a8
u32 JOAN_STDCALL buffer_addref(u8* self) {
    st<u32>(self, 4, ld<u32>(self, 4) + 1);
    return ld<u32>(self, 4);
}
// 0x0078c590 (base dtor 0x0078c36d + operator delete)
u32 JOAN_STDCALL buffer_release(u8* self) {
    const u32 old = ld<u32>(self, 4);
    const u32 r = old - 1;
    st<u32>(self, 4, r);
    if (old == 1) {
        // the base dtor's vtable store (0x0078c36d) is left out: the object is freed below, nothing reads it
        void* const data = ld<void*>(self, 0xc);
        if (data != nullptr) op_delete(data);
        op_delete(self);
    }
    return r;
}
// 0x0078c3b5 / 0x0078c3bf
void* JOAN_STDCALL buffer_pointer(u8* self) { return ld<void*>(self, 0xc); }
u32 JOAN_STDCALL buffer_size(u8* self) { return ld<u32>(self, 8); }

// ---- ALUT ------------------------------------------------------------------------------------------
// 0x007a9989 alutUnloadWAV(format, data, size, freq)
void JOAN_CDECL alut_unload_wav(i32 /*format*/, void* data, i32 /*size*/, i32 /*freq*/) {
    if (data != nullptr) joan::orig::free_(joan::orig::arg::mptr(data));  // free
}

#define D(name, addr, f) JOAN_REPLACE("d3dx/objects", name, addr, f)
D("D3DXCreateMatrixStack", 0x00769dc0, &create_matrix_stack);
D("D3DXGetErrorStringA", 0x007693c4, &joan::lib_b113_04::d3dx_get_error_string_a);
D("ID3DXBuffer::QueryInterface", 0x0078c549, &buffer_qi);
D("ID3DXBuffer::AddRef", 0x0078c3a8, &buffer_addref);
D("ID3DXBuffer::Release", 0x0078c590, &buffer_release);
D("ID3DXBuffer::GetBufferPointer", 0x0078c3b5, &buffer_pointer);
D("ID3DXBuffer::GetBufferSize", 0x0078c3bf, &buffer_size);
#undef D

#define L(name, addr, f) JOAN_REPLACE("lib/misc", name, addr, f)
L("alutUnloadWAV", 0x007a9989, &alut_unload_wav);
#undef L

}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::d3dx::get_error_string_a(std::int32_t a0, const char* a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::joan::lib_b113_04::d3dx_get_error_string_a>(a0, a1, a2)); }  // 0x7693c4
auto joan::orig::d3dx::create_matrix_stack(std::uint32_t a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::create_matrix_stack>(a0, a1)); }  // 0x769dc0
auto joan::orig::alut_unload_wav(std::int32_t a0, void* a1, std::int32_t a2, std::int32_t a3) -> void { joan::bind::free<&::alut_unload_wav>(a0, a1, a2, a3); }  // 0x7a9989
