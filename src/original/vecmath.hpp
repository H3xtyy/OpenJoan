// The en3d vector / matrix / box helpers of the 0x0060cxxx..0x0060dxxx family (cdecl, out first, returning out;
// the Aabb members are thiscall), callable directly from game code (refactor batch R7a). Pointer arguments are
// untyped like in the game (vectors are 3 floats, planes 4, matrices 16, segments 2 vectors, boxes min xyz /
// max xyz). Implemented (and registered at their original addresses) in the files named per function.
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstdint>

namespace joan::orig {

#pragma pack(push, 1)
/// Three-component float vector (12 bytes), the engine's position / direction type; plain data, no ownership.
struct Vec3 {
    float x;  ///< x component
    float y;  ///< y component
    float z;  ///< z component

    // The other Vec3 functions the game code calls (typed, through their slots).
#include "gen/vec3_methods.inc"
};
/// Axis-aligned box (24 bytes): min xyz, max xyz. The original names it Box (0x0060d430 / 0x0060d490 / 0x0060d5f0).
struct Aabb {
    float min[3];  ///< lower corner (x, y, z)
    float max[3];  ///< upper corner (x, y, z)

    bool contains(const void* p);  ///< 0x0060d5f0 Box::contains; returns al
    Aabb* set_point(const void* p);  ///< 0x0060d430 Box::set_point: min = max = p
    Aabb* extend(const void* p);  ///< 0x0060d490 Box::extend
};
#pragma pack(pop)
static_assert(sizeof(Vec3) == 12);
static_assert(sizeof(Aabb) == 24);

Vec3* vec_add(void* out, const void* a, const void* b);  ///< 0x0060cc20 out = a + b
Vec3* vec_sub(void* out, const void* a, const void* b);  ///< 0x0060cc90 out = a - b
Vec3* vec_scale(void* out, const void* v, float s);  ///< 0x0060cd00 out = s * v
Vec3* vec_scaled(void* out, float k, const void* v);  ///< 0x0060cd60 out = k * v
Vec3* vec_scale_in_place(void* v, float k);  ///< 0x0060ce80 v *= k
Vec3* vec_negate(void* out, const void* v);  ///< 0x0060cbc0 out = -v
Vec3* vec_add_in_place(void* a, const void* b);  ///< 0x0060cdc0 a += b
std::uint8_t vec_equal(const void* a, const void* b);  ///< 0x0060ced0 a == b, NaN counts as equal
Vec3* matrix_axis_z(void* out, const void* m);  ///< 0x0060c220 out = row 3
Vec3* matrix_position(void* out, const void* m);  ///< 0x0060c310 out = row 4
float* plane_from_points(void* out, const void* a, const void* b, const void* c);  ///< 0x0060d920
Vec3* segment_point_at(void* out, const void* seg, float t);  ///< 0x0060de40 seg.a + t * (seg.b - seg.a)
std::int32_t segment_hits_box(const void* seg, const void* box);  ///< 0x0060dfd0

}  // namespace joan::orig
