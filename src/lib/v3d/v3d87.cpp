// Original library: v3d_87.dll (the 23 vector-maths exports joan.exe imports; no static-link source).
//
// N5 (STAGE3): v3d_87.dll vector maths without the DLL.
//
// The 23 functions joan.exe imports from v3d_87.dll (IAT 0x00a6ddec..0x00a6de44) are entries of the loader's import
// override table (loader/import_overrides.hpp): the loader puts these wrappers (call counters around the
// reimplementations in src/render/v3d.cpp) straight into the IAT slots, so v3d_87.dll is never loaded - the original
// code's `call [slot]` and our modules' import_call / import_thiscall through the same slots run them. The game folder
// needs neither v3d_87.dll nor v3d_sse.dll; the start-up CPU-module check 0x00447150 (game_b108_05.cpp) no longer
// loads or copies them. Always active (no [Hooks] switch: there is no DLL to fall back to; a leftover
// `Enable=render/v3d` only produces the "matches no module" warning in openjoan.log).
//
// Calling conventions (from the decorated export names in src/import_table.inc): free functions "YA" = __cdecl,
// SkinVertex members "QAE" = __thiscall (ecx = this; add_bone pops its 16 argument bytes); returns: the first pointer
// argument in eax (D3DVECTOR& / D3DMATRIX&), int in eax (vertexf_check_cpu, add_bone), float "M" in st0 (length,
// length_sq, dot_product - returned here as double, so st0 holds the x87 value unrounded, exactly like the DLL).
//
// The shipped v3d_87.dll is the SSE build (identical to v3d_sse.dll; see render/v3d.hpp and notes V3D.md); that is the
// one reimplemented. SSE functions are exact under any x87 state. The x87 ones (length, length_sq, dot_product,
// matrix_multiply_translation, matrix_inverse x2, SkinVertex::normalize) follow the x87 precision control in force:
// 24 bits (Direct3D 8 / the game) or 53 bits, round to nearest. Any other control word (64-bit precision or a directed
// rounding mode; never seen in a game run) is computed at 53 bits (64-bit precision) or 24 bits, round to nearest, and
// counted as "v3d (x87 control word ...)": results may then differ from the DLL in the last bit. Until w129 that case
// (and a verify mode) called the DLL; the bit-for-bit comparison with the real DLL lives on in the Linux harness
// re/v3d_harness (0 mismatches in ~230M comparisons).
#include "../../callconv.hpp"
#include "../../fp/x87.hpp"

#include <cstdint>
#include <cstring>

#include "../../runtime/loader/import_overrides.hpp"
#include "../../registry.hpp"
#include "../../render/v3d.hpp"

namespace {

namespace v3d = joan::render::v3d;
using v3d::Matrix;
using v3d::SkinVertex;
using v3d::Vec3;

JOAN_COUNTER(c_vector_transform1, "v3d::vector_transform(v, m)");
JOAN_COUNTER(c_multiply_translation, "v3d::matrix_multiply_translation");
JOAN_COUNTER(c_check_cpu, "v3d::vertexf_check_cpu");
JOAN_COUNTER(c_length_sq, "v3d::length_sq");
JOAN_COUNTER(c_length, "v3d::length");
JOAN_COUNTER(c_multiply2, "v3d::matrix_multiply(a, b)");
JOAN_COUNTER(c_cross, "v3d::cross_product");
JOAN_COUNTER(c_inverse1, "v3d::matrix_inverse(m)");
JOAN_COUNTER(c_sv_deinit, "v3d::SkinVertex::deinit");
JOAN_COUNTER(c_sv_init, "v3d::SkinVertex::init");
JOAN_COUNTER(c_sv_add_bone, "v3d::SkinVertex::add_bone");
JOAN_COUNTER(c_sv_normalize, "v3d::SkinVertex::normalize");
JOAN_COUNTER(c_skin_deform, "v3d::skin_deform");
JOAN_COUNTER(c_transpose1, "v3d::matrix_transpose(m)");
JOAN_COUNTER(c_transpose2, "v3d::matrix_transpose(out, in)");
JOAN_COUNTER(c_multiply3, "v3d::matrix_multiply(out, a, b)");
JOAN_COUNTER(c_vector_transform2, "v3d::vector_transform(out, v, m)");
JOAN_COUNTER(c_point_transform1, "v3d::point_transform(v, m)");
JOAN_COUNTER(c_point_transform2, "v3d::point_transform(out, v, m)");
JOAN_COUNTER(c_normalize2, "v3d::vector_normalize(out, v)");
JOAN_COUNTER(c_inverse2, "v3d::matrix_inverse(out, m)");
JOAN_COUNTER(c_normalize1, "v3d::vector_normalize(v)");
JOAN_COUNTER(c_dot, "v3d::dot_product");
JOAN_COUNTER(c_x87_other, "v3d (x87 control word not 24/53-bit round-to-nearest: computed at 53/24 bits, nearest)");

/// Precision of the x87 functions: the emulated x87 precision of this thread (fp::current_precision).
v3d::X87 x87_mode() {
    const joan::fp::Precision p = joan::fp::current_precision();
    if (p == joan::fp::Precision::Unsupported) c_x87_other.hit();
    v3d::X87 x;
    x.single = p == joan::fp::Precision::Single;
    return x;
}

// --- cdecl functions ---------------------------------------------------------------------------
Vec3* JOAN_CDECL vector_transform1(Vec3* v, const Matrix* m) {  // 0x00a6ddec
    c_vector_transform1.hit();
    return v3d::vector_transform(v, m);
}
Matrix* JOAN_CDECL multiply_translation(Matrix* m, const Vec3* v) {  // 0x00a6ddf0
    c_multiply_translation.hit();
    return v3d::matrix_multiply_translation(m, v, x87_mode());
}
std::int32_t JOAN_CDECL check_cpu() {  // 0x00a6ddf4
    c_check_cpu.hit();
    return v3d::vertexf_check_cpu();
}
// The st0 results: double, so the caller's st0 holds the x87 value unrounded.
double JOAN_CDECL length_sq(const Vec3* v) {  // 0x00a6ddf8
    c_length_sq.hit();
    return v3d::length_sq(v, x87_mode());
}
double JOAN_CDECL length(const Vec3* v) {  // 0x00a6ddfc
    c_length.hit();
    return v3d::length(v, x87_mode());
}
double JOAN_CDECL dot_product(const Vec3* a, const Vec3* b) {  // 0x00a6de44
    c_dot.hit();
    return v3d::dot_product(a, b, x87_mode());
}
Matrix* JOAN_CDECL multiply2(Matrix* a, const Matrix* b) {  // 0x00a6de00
    c_multiply2.hit();
    return v3d::matrix_multiply(a, b);
}
Vec3* JOAN_CDECL cross(Vec3* out, const Vec3* a, const Vec3* b) {  // 0x00a6de04
    c_cross.hit();
    return v3d::cross_product(out, a, b);
}
Matrix* JOAN_CDECL inverse1(Matrix* m) {  // 0x00a6de08
    c_inverse1.hit();
    return v3d::matrix_inverse(m, x87_mode());
}
void JOAN_CDECL skin_deform(std::int32_t n, Vec3* pos, std::uint32_t pos_stride, Vec3* normal, std::uint32_t normal_stride,
                         const SkinVertex* sv, std::uint32_t sv_stride, const Matrix* bones,
                         std::uint32_t bone_stride) {  // 0x00a6de1c
    c_skin_deform.hit();
    v3d::skin_deform(n, pos, pos_stride, normal, normal_stride, sv, sv_stride, bones, bone_stride);
}
Matrix* JOAN_CDECL transpose1(Matrix* m) {  // 0x00a6de20
    c_transpose1.hit();
    return v3d::matrix_transpose(m);
}
Matrix* JOAN_CDECL transpose2(Matrix* out, const Matrix* in) {  // 0x00a6de24
    c_transpose2.hit();
    return v3d::matrix_transpose(out, in);
}
Matrix* JOAN_CDECL multiply3(Matrix* out, const Matrix* a, const Matrix* b) {  // 0x00a6de28
    c_multiply3.hit();
    return v3d::matrix_multiply(out, a, b);
}
Vec3* JOAN_CDECL vector_transform2(Vec3* out, const Vec3* v, const Matrix* m) {  // 0x00a6de2c
    c_vector_transform2.hit();
    return v3d::vector_transform(out, v, m);
}
Vec3* JOAN_CDECL point_transform1(Vec3* v, const Matrix* m) {  // 0x00a6de30
    c_point_transform1.hit();
    return v3d::point_transform(v, m);
}
Vec3* JOAN_CDECL point_transform2(Vec3* out, const Vec3* v, const Matrix* m) {  // 0x00a6de34
    c_point_transform2.hit();
    return v3d::point_transform(out, v, m);
}
Vec3* JOAN_CDECL normalize2(Vec3* out, const Vec3* v) {  // 0x00a6de38
    c_normalize2.hit();
    return v3d::vector_normalize(out, v);
}
Matrix* JOAN_CDECL inverse2(Matrix* out, const Matrix* m) {  // 0x00a6de3c
    c_inverse2.hit();
    return v3d::matrix_inverse(out, m, x87_mode());
}
Vec3* JOAN_CDECL normalize1(Vec3* v) {  // 0x00a6de40
    c_normalize1.hit();
    return v3d::vector_normalize(v);
}

// --- SkinVertex (thiscall: ecx = the SkinVertex) -----------------------------------------------
struct SkinVertexHook {
    SkinVertex* self() { return reinterpret_cast<SkinVertex*>(this); }

    void deinit() {  // 0x00a6de0c
        c_sv_deinit.hit();
        v3d::skin_deinit(self());
    }
    void init() {  // 0x00a6de10
        c_sv_init.hit();
        v3d::skin_init(self());
    }
    // The weight arrives as its 32 bits: the only caller (en3d/skin.cpp Skin::skin_add_bone) passes the float as a
    // dword; on x86-64 a `float` parameter would be read from xmm0, which a dword argument never fills.
    std::int32_t add_bone(std::int32_t bone, const Vec3* pos, const Vec3* normal, std::uint32_t weight_bits) {  // 0x00a6de14
        c_sv_add_bone.hit();
        float weight;
        std::memcpy(&weight, &weight_bits, sizeof weight);
        return v3d::skin_add_bone(self(), bone, pos, normal, weight);
    }
    void normalize() {  // 0x00a6de18
        c_sv_normalize.hit();
        v3d::skin_normalize(self(), x87_mode());
    }
};

// Export names as imported (src/import_table.inc, v3d_87.dll: 23).
#define V3D(name, export_name, fn) JOAN_IMPORT_OVERRIDE("v3d_87.dll", export_name, fn, "v3d::" name)
V3D("vector_transform(v, m)", "?vector_transform@@YAAAU_D3DVECTOR@@AAU1@AAU_D3DMATRIX@@@Z", &vector_transform1);
V3D("matrix_multiply_translation", "?matrix_multiply_translation@@YAAAU_D3DMATRIX@@AAU1@AAU_D3DVECTOR@@@Z",
    &multiply_translation);
V3D("vertexf_check_cpu", "vertexf_check_cpu", &check_cpu);
V3D("length_sq", "?length_sq@@YAMABU_D3DVECTOR@@@Z", &length_sq);
V3D("length", "?length@@YAMABU_D3DVECTOR@@@Z", &length);
V3D("matrix_multiply(a, b)", "?matrix_multiply@@YAAAU_D3DMATRIX@@AAU1@0@Z", &multiply2);
V3D("cross_product", "?cross_product@@YAAAU_D3DVECTOR@@AAU1@ABU1@1@Z", &cross);
V3D("matrix_inverse(m)", "?matrix_inverse@@YAAAU_D3DMATRIX@@AAU1@@Z", &inverse1);
V3D("SkinVertex::deinit", "?deinit@SkinVertex@@QAEXXZ", &SkinVertexHook::deinit);
V3D("SkinVertex::init", "?init@SkinVertex@@QAEXXZ", &SkinVertexHook::init);
V3D("SkinVertex::add_bone", "?add_bone@SkinVertex@@QAEHHAAU_D3DVECTOR@@0M@Z", &SkinVertexHook::add_bone);
V3D("SkinVertex::normalize", "?normalize@SkinVertex@@QAEXXZ", &SkinVertexHook::normalize);
V3D("skin_deform", "?skin_deform@@YAXHPAU_D3DVECTOR@@I0IPAUSkinVertex@@IPAU_D3DMATRIX@@I@Z", &skin_deform);
V3D("matrix_transpose(m)", "?matrix_transpose@@YAAAU_D3DMATRIX@@AAU1@@Z", &transpose1);
V3D("matrix_transpose(out, in)", "?matrix_transpose@@YAAAU_D3DMATRIX@@AAU1@0@Z", &transpose2);
V3D("matrix_multiply(out, a, b)", "?matrix_multiply@@YAAAU_D3DMATRIX@@AAU1@00@Z", &multiply3);
V3D("vector_transform(out, v, m)", "?vector_transform@@YAAAU_D3DVECTOR@@AAU1@0AAU_D3DMATRIX@@@Z", &vector_transform2);
V3D("point_transform(v, m)", "?point_transform@@YAAAU_D3DVECTOR@@AAU1@AAU_D3DMATRIX@@@Z", &point_transform1);
V3D("point_transform(out, v, m)", "?point_transform@@YAAAU_D3DVECTOR@@AAU1@0AAU_D3DMATRIX@@@Z", &point_transform2);
V3D("vector_normalize(out, v)", "?vector_normalize@@YAAAU_D3DVECTOR@@AAU1@0@Z", &normalize2);
V3D("matrix_inverse(out, m)", "?matrix_inverse@@YAAAU_D3DMATRIX@@AAU1@0@Z", &inverse2);
V3D("vector_normalize(v)", "?vector_normalize@@YAAAU_D3DVECTOR@@AAU1@@Z", &normalize1);
V3D("dot_product", "?dot_product@@YAMABU_D3DVECTOR@@0@Z", &dot_product);

}  // namespace
