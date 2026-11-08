// Original library sources (re/attribution.csv original_file, under lib/): d3dx8/d3dx8err.cpp (D3DXGetErrorStringA
//   body, registered in objects.cpp), d3dx8/d3dx8mesh.cpp.
//
// Batch 113 / list 04: statically linked D3DX8 helpers (0x00769163..0x0076bb6b) and the D3DX8
// image library (blitter + file-format loaders + file mapping, 0x00783868..0x00788857).
//
// Root identification (D3DERR_INVALIDCALL = 0x8876086c, E_OUTOFMEMORY = 0x8007000e):
//   0x00769163 D3DXAssembleShaderFromFileA(file, flags, ppConstants, ppCode, ppErrors) stdcall ret 0x14
//              CFileMapping + the vertex-shader assembler object (0x00789db6 ctor, 0x0078bed6
//              Assemble, 0x00789e56 dtor; heap) - STATEFUL, left original.
//              (Neighbours without callers: 0x007691ef ...FromFileW ret 0x14, 0x007692b8
//              D3DXAssembleShader ret 0x18, 0x0076931c D3DXGetFVFVertexSize ret 4.)
//   0x007693c4 D3DXGetErrorStringA(HRESULT hr, char* buf, UINT size)  stdcall ret 0xc, returns 0.
//              PURE; implemented below (d3dx_get_error_string_a, reviewed against the asm: 30
//              equality cases, VC6 _snprintf truncation) but NOT registered. (The writer's reason,
//              patch_entry rejecting `push esi`, no longer holds: library-range entries are now
//              accepted.) Ready to be registered or wired into its caller en3d_helpers16.cpp.
//   0x00769dc0 D3DXCreateMatrixStack(DWORD flags, LPD3DXMATRIXSTACK* ppStack)  stdcall ret 8 (not
//              D3DXCreateBuffer): NULL pp -> D3DERR_INVALIDCALL; operator new 0x007b1d35(0x14), ctor
//              0x00769da5 (vtable 0x009e3328 = 18-slot ID3DXMatrixStack, refcount +0x10 = 1),
//              Init 0x00769921(16) (new[16 * 64] matrices at +8, capacity +4, top +0xc = 0, then
//              vtbl[5] LoadIdentity); failure -> vtbl[2] Release; flags unused. Heap + COM object,
//              left original.
//   0x0076ba93 D3DXDeclaratorFromFVF(DWORD fvf, DWORD decl[MAX_FVF_DECL_SIZE]) stdcall ret 8 -
//              PURE, replaced here (inlines its CFVFVertex(fvf) helper 0x0076af06, also pure).
// ("not patchable" below = the writer's patch_entry prologue check, obsolete since patch_entry
//  accepts any library-range entry; these stay original because they are stateful / object code.)
//   0x00783868 CD3DXBlt::CD3DXBlt (zero +4, +0)            - entry `mov eax,ecx`: not patchable.
//   0x00783872 CD3DXBlt::~CD3DXBlt (deletes both codecs)   - d3dx-object, not patchable.
//   0x00786b38 CD3DXBlt::Blt(dst image, src image, filter) - heap/codec objects, not patchable.
//   0x00786c4a CD3DXImage::CD3DXImage                      - not patchable.
//   0x00786c63 CD3DXImage::~CD3DXImage (operator delete)   - heap, not patchable.
//   0x0078856c CD3DXImage::Load(data, size, D3DXIMAGE_INFO*) - heap, prologue `push ebp; mov ebp,
//              esp; push ecx` rejected by patch_entry.
//   0x007886c6 / 0x007886d8 / 0x00788832  CFileMapping ctor / Open(name, wide) / dtor - not
//              patchable (Open starts `push ebp; lea ebp,[esp-0x70]`).
//   0x0078883d / 0x0078884a  CD3DXResource ctor / dtor (DeleteObject) - not patchable.
// Full per-function table (with the context functions): scratchpad w113_04/ids.md.
//
// Modules: d3dx/b113_04.
#include "../../callconv.hpp"
#include <cstdint>

#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

namespace {

using u32 = std::uint32_t;
using i32 = std::int32_t;

JOAN_COUNTER(c_declarator, "D3DXDeclaratorFromFVF");

// 0x0076ba93 D3DXDeclaratorFromFVF (stdcall, ret 8).
// The original builds a CFVFVertex (0x0076af06) on the stack and reads only: the FVF's
// D3DFVF_LASTBETA_UBYTE4 bit (byte +1 & 0x10), the normal / diffuse / specular offsets (+8,
// +0x10, +0x14: tested `> 0`; each is the running vertex offset, which starts at 12 or 16, so
// it is non-zero exactly when its FVF bit 0x10 / 0x40 / 0x80 is set), the blend-weight count
// +0x1c (0 unless (fvf & 0xe) >= 6, then ((fvf & 0xe) - 4) >> 1) and the texture count +0x20
// ((fvf >> 8) & 0xf). The helper has no side effects outside the stack object.
static i32 JOAN_STDCALL declarator_from_fvf(u32 fvf, u32* decl) {
    c_declarator.hit();
    const u32 position = fvf & 0xe;
    const u32 blend = position >= 6 ? (position - 4) >> 1 : 0u;
    const u32 tex_count = (fvf >> 8) & 0xf;

    decl[0] = 0x20000000u;  // D3DVSD_STREAM(0)
    decl[1] = 0x40020000u;  // D3DVSD_REG(0, FLOAT3)
    u32 i = 2;
    if (blend > 0) {
        if ((fvf & 0x1000u) != 0) {  // D3DFVF_LASTBETA_UBYTE4
            if (blend > 1) {
                decl[2] = ((blend << 16) - 0x1ffffu) | 0x40000001u;
                i = 3;
            }
            decl[i] = 0x40050002u;
            ++i;
        } else {
            if (blend == 5) return static_cast<i32>(0x8876086cu);  // D3DERR_INVALIDCALL, no terminator
            decl[2] = ((blend << 16) - 0xffffu) | 0x40000001u;
            i = 3;
        }
    }
    if ((fvf & 0x10u) != 0) decl[i++] = 0x40020003u;  // normal
    if ((fvf & 0x20u) != 0) decl[i++] = 0x40000004u;  // point size
    if ((fvf & 0x40u) != 0) decl[i++] = 0x40040005u;  // diffuse
    if ((fvf & 0x80u) != 0) decl[i++] = 0x40040006u;  // specular
    for (u32 t = 0; t < tex_count; ++t) decl[i++] = (t + 7) | 0x40010000u;
    decl[i] = 0xffffffffu;  // D3DVSD_END
    return 0;
}

// --- 0x007693c4 D3DXGetErrorStringA (not installable, see header) ---------------------------

struct ErrorName {
    u32 hr;
    const char* name;
};

// Every comparison in the original is an equality test, so the binary-search tree it compiles
// to is equivalent to this flat table.
constexpr ErrorName kErrorNames[] = {
    {0x8876081au, "D3DERR_UNSUPPORTEDCOLORARG"},
    {0x88760819u, "D3DERR_UNSUPPORTEDCOLOROPERATION"},
    {0x88760818u, "D3DERR_WRONGTEXTUREFORMAT"},
    {0x8876017cu, "D3DERR_OUTOFVIDEOMEMORY"},
    {0x8007000eu, "E_OUTOFMEMORY"},
    {0x80004005u, "E_FAIL"},
    {0x80004001u, "E_NOTIMPL"},
    {0x8876081bu, "D3DERR_UNSUPPORTEDALPHAOPERATION"},
    {0x8876081cu, "D3DERR_UNSUPPORTEDALPHAARG"},
    {0x8876081du, "D3DERR_TOOMANYOPERATIONS"},
    {0x8876081eu, "D3DERR_CONFLICTINGTEXTUREFILTER"},
    {0x8876081fu, "D3DERR_UNSUPPORTEDFACTORVALUE"},
    {0x88760821u, "D3DERR_CONFLICTINGRENDERSTATE"},
    {0x88760822u, "D3DERR_UNSUPPORTEDTEXTUREFILTER"},
    {0x88760826u, "D3DERR_CONFLICTINGTEXTUREPALETTE"},
    {0x88760827u, "D3DERR_DRIVERINTERNALERROR"},
    {0x88760b54u, "D3DXERR_CANNOTMODIFYINDEXBUFFER"},
    {0x88760866u, "D3DERR_NOTFOUND"},
    {0x88760867u, "D3DERR_MOREDATA"},
    {0x88760868u, "D3DERR_DEVICELOST"},
    {0x88760869u, "D3DERR_DEVICENOTRESET"},
    {0x8876086au, "D3DERR_NOTAVAILABLE"},
    {0x8876086bu, "D3DERR_INVALIDDEVICE"},
    {0x8876086cu, "D3DERR_INVALIDCALL"},
    {0x88760b55u, "D3DXERR_INVALIDMESH"},
    {0x88760b56u, "D3DXERR_CANNOTATTRSORT"},
    {0x88760b57u, "D3DXERR_SKINNINGNOTSUPPORTED"},
    {0x88760b58u, "D3DXERR_TOOMANYINFLUENCES"},
    {0x88760b59u, "D3DXERR_INVALIDDATA"},
    {0x00000000u, "S_OK"},
};

// MSVC 6 _snprintf(buf, count, fmt, ...) (0x007b77e3) for an already formatted text: a string
// FILE with _cnt = (int)count; each character is stored while --_cnt >= 0, the first failing
// store makes the result -1 (nothing later can succeed); the closing NUL is stored only while
// --_cnt >= 0 still holds. So: n < count -> text + NUL, returns n; n == count -> text without
// NUL, returns n; n > count -> first count chars, returns -1.
static i32 vc6_snprintf_text(char* buf, u32 count, const char* text, i32 n) {
    i32 cnt = static_cast<i32>(count);
    i32 result = n;
    char* p = buf;
    for (i32 k = 0; k < n; ++k) {
        if (--cnt >= 0) {
            *p++ = text[k];
        } else {
            result = -1;
            break;
        }
    }
    if (--cnt >= 0) *p = '\0';
    return result;
}

JOAN_REPLACE("d3dx/b113_04", "D3DXDeclaratorFromFVF", 0x0076ba93, &declarator_from_fvf);

}  // namespace

namespace joan::lib_b113_04 {

// External linkage (not registered, so it would otherwise be an unreferenced internal function:
// MSVC C4505). Callable as joan::lib_b113_04::d3dx_get_error_string_a once a caller is wired.
std::int32_t JOAN_STDCALL d3dx_get_error_string_a(std::int32_t hr, char* buf, std::uint32_t size) {
    if (buf == nullptr || size == 0) return 0;
    char text[64];
    i32 n = 0;
    const char* name = nullptr;
    for (const ErrorName& e : kErrorNames) {
        if (e.hr == static_cast<u32>(hr)) {
            name = e.name;
            break;
        }
    }
    if (name != nullptr) {  // "%s"
        while (name[n] != '\0') {
            text[n] = name[n];
            ++n;
        }
    } else {  // "Unrecognized error: 0x%0.8x": precision 8 = exactly 8 lowercase digits for 32 bits
        const char* prefix = "Unrecognized error: 0x";
        while (prefix[n] != '\0') {
            text[n] = prefix[n];
            ++n;
        }
        const u32 v = static_cast<u32>(hr);
        for (int d = 7; d >= 0; --d) {
            const u32 nibble = (v >> (d * 4)) & 0xf;
            text[n++] = static_cast<char>(nibble < 10 ? '0' + nibble : 'a' + (nibble - 10));
        }
    }
    if (vc6_snprintf_text(buf, size, text, n) < 0 && size > 0) buf[size - 1] = '\0';
    return 0;
}

}  // namespace joan::lib_b113_04

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::d3dx::declarator_from_fvf(std::uint32_t a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::declarator_from_fvf>(a0, a1)); }  // 0x76ba93
