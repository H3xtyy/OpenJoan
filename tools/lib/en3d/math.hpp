// Small vector / matrix helpers for the 3D asset code.
//
// Conventions (all taken from the original data, see docs/FORMATS.md):
//  * right-handed, Z up, units roughly centimetres-ish (Joan is ~79 units tall),
//    characters face -Y (3ds Max "front" view), exactly as exported from Max;
//  * a bone transform is a row-major 3x4 matrix [R | t]: p' = R * p + t.
#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

namespace joan::en3d {

struct Vec2 {
    float x = 0, y = 0;
};

struct Vec3 {
    float x = 0, y = 0, z = 0;

    friend Vec3 operator+(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
    friend Vec3 operator-(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
    friend Vec3 operator*(Vec3 a, float s) { return {a.x * s, a.y * s, a.z * s}; }
    Vec3& operator+=(Vec3 b) { return *this = *this + b; }
};

inline float dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 cross(Vec3 a, Vec3 b) { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
inline float length(Vec3 a) { return std::sqrt(dot(a, a)); }
inline Vec3 normalize(Vec3 a) {
    const float l = length(a);
    return l > 1e-20f ? a * (1.0f / l) : Vec3{0, 0, 1};
}

/// RGBA colour in 0..1. The files store D3DCOLORVALUE-like floats in A,R,G,B order;
/// the loaders reorder them.
struct Color {
    float r = 1, g = 1, b = 1, a = 1;
};

struct Quat {
    float x = 0, y = 0, z = 0, w = 1;
};

/// Row-major 3x4 affine transform.
struct Mat34 {
    std::array<float, 12> m{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};

    float& operator()(int r, int c) { return m[static_cast<std::size_t>(r * 4 + c)]; }
    float operator()(int r, int c) const { return m[static_cast<std::size_t>(r * 4 + c)]; }

    [[nodiscard]] Vec3 transform_point(Vec3 p) const {
        return {m[0] * p.x + m[1] * p.y + m[2] * p.z + m[3], m[4] * p.x + m[5] * p.y + m[6] * p.z + m[7],
                m[8] * p.x + m[9] * p.y + m[10] * p.z + m[11]};
    }
    [[nodiscard]] Vec3 transform_vector(Vec3 v) const {
        return {m[0] * v.x + m[1] * v.y + m[2] * v.z, m[4] * v.x + m[5] * v.y + m[6] * v.z,
                m[8] * v.x + m[9] * v.y + m[10] * v.z};
    }
    [[nodiscard]] Vec3 translation() const { return {m[3], m[7], m[11]}; }
};

/// Splits [R*S | t] into rotation, per-axis scale (column lengths) and translation.
struct Decomposed {
    Quat rotation;
    Vec3 scale{1, 1, 1};
    Vec3 translation;
};

inline Quat quat_from_rotation(const float r[3][3]) {
    Quat q;
    const float trace = r[0][0] + r[1][1] + r[2][2];
    if (trace > 0) {
        const float s = std::sqrt(trace + 1.0f) * 2;
        q = {(r[2][1] - r[1][2]) / s, (r[0][2] - r[2][0]) / s, (r[1][0] - r[0][1]) / s, 0.25f * s};
    } else if (r[0][0] > r[1][1] && r[0][0] > r[2][2]) {
        const float s = std::sqrt(1.0f + r[0][0] - r[1][1] - r[2][2]) * 2;
        q = {0.25f * s, (r[0][1] + r[1][0]) / s, (r[0][2] + r[2][0]) / s, (r[2][1] - r[1][2]) / s};
    } else if (r[1][1] > r[2][2]) {
        const float s = std::sqrt(1.0f + r[1][1] - r[0][0] - r[2][2]) * 2;
        q = {(r[0][1] + r[1][0]) / s, 0.25f * s, (r[1][2] + r[2][1]) / s, (r[0][2] - r[2][0]) / s};
    } else {
        const float s = std::sqrt(1.0f + r[2][2] - r[0][0] - r[1][1]) * 2;
        q = {(r[0][2] + r[2][0]) / s, (r[1][2] + r[2][1]) / s, 0.25f * s, (r[1][0] - r[0][1]) / s};
    }
    const float l = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    return {q.x / l, q.y / l, q.z / l, q.w / l};
}

inline Decomposed decompose(const Mat34& a) {
    Decomposed d;
    d.translation = a.translation();
    Vec3 col[3];
    for (int c = 0; c < 3; ++c) col[c] = {a(0, c), a(1, c), a(2, c)};
    d.scale = {length(col[0]), length(col[1]), length(col[2])};
    float r[3][3];
    const float s[3] = {d.scale.x, d.scale.y, d.scale.z};
    for (int c = 0; c < 3; ++c)
        for (int row = 0; row < 3; ++row) r[row][c] = s[c] > 1e-20f ? a(row, c) / s[c] : (row == c ? 1.0f : 0.0f);
    d.rotation = quat_from_rotation(r);
    return d;
}

inline Mat34 compose(const Decomposed& d) {
    const Quat q = d.rotation;
    const float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
    const float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
    const float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;
    Mat34 m;
    m.m = {(1 - 2 * (yy + zz)) * d.scale.x, 2 * (xy - wz) * d.scale.y, 2 * (xz + wy) * d.scale.z, d.translation.x,
           2 * (xy + wz) * d.scale.x, (1 - 2 * (xx + zz)) * d.scale.y, 2 * (yz - wx) * d.scale.z, d.translation.y,
           2 * (xz - wy) * d.scale.x, 2 * (yz + wx) * d.scale.y, (1 - 2 * (xx + yy)) * d.scale.z, d.translation.z};
    return m;
}

inline Quat slerp(Quat a, Quat b, float t) {
    float d = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    if (d < 0) {
        b = {-b.x, -b.y, -b.z, -b.w};
        d = -d;
    }
    float wa = 1 - t, wb = t;
    if (d < 0.9995f) {
        const float theta = std::acos(std::clamp(d, -1.0f, 1.0f));
        const float s = std::sin(theta);
        wa = std::sin((1 - t) * theta) / s;
        wb = std::sin(t * theta) / s;
    }
    Quat q{wa * a.x + wb * b.x, wa * a.y + wb * b.y, wa * a.z + wb * b.z, wa * a.w + wb * b.w};
    const float l = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    return {q.x / l, q.y / l, q.z / l, q.w / l};
}

/// Interpolates two bone transforms (slerp rotation, lerp scale and translation).
inline Mat34 interpolate(const Mat34& a, const Mat34& b, float t) {
    if (t <= 0) return a;
    if (t >= 1) return b;
    const Decomposed da = decompose(a), db = decompose(b);
    Decomposed r;
    r.rotation = slerp(da.rotation, db.rotation, t);
    r.scale = da.scale + (db.scale - da.scale) * t;
    r.translation = da.translation + (db.translation - da.translation) * t;
    return compose(r);
}

/// Column-major 4x4 matrix for OpenGL.
struct Mat4 {
    std::array<float, 16> m{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    friend Mat4 operator*(const Mat4& a, const Mat4& b) {
        Mat4 r;
        for (int c = 0; c < 4; ++c)
            for (int row = 0; row < 4; ++row) {
                float s = 0;
                for (int k = 0; k < 4; ++k) s += a.m[static_cast<std::size_t>(k * 4 + row)] * b.m[static_cast<std::size_t>(c * 4 + k)];
                r.m[static_cast<std::size_t>(c * 4 + row)] = s;
            }
        return r;
    }

    static Mat4 perspective(float fovy_rad, float aspect, float znear, float zfar) {
        const float f = 1.0f / std::tan(fovy_rad / 2);
        Mat4 r;
        r.m = {f / aspect, 0, 0, 0, 0, f, 0, 0, 0, 0, (zfar + znear) / (znear - zfar), -1,
               0, 0, 2 * zfar * znear / (znear - zfar), 0};
        return r;
    }

    static Mat4 look_at(Vec3 eye, Vec3 target, Vec3 up) {
        const Vec3 f = normalize(target - eye);
        const Vec3 s = normalize(cross(f, up));
        const Vec3 u = cross(s, f);
        Mat4 r;
        r.m = {s.x, u.x, -f.x, 0, s.y, u.y, -f.y, 0, s.z, u.z, -f.z, 0, -dot(s, eye), -dot(u, eye), dot(f, eye), 1};
        return r;
    }
};

}  // namespace joan::en3d
