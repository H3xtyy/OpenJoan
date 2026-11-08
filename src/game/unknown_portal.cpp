// game/unknown_portal.cpp: functions whose original source file is unknown (formerly game/_unplaced/portal.cpp;
// re/attribution.csv: segment edge ww/en3d/plane_sh.cpp|ww/en3d/portal.cpp; class/call graph -> ww/en3). They move to
// their original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstddef>
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::cdecl_call;

using joan::call::thiscall;

using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

// Every helper shares one class so `this` is just the object's address.
struct H {
    // --- Portal / sector (between plane_sh.cpp and scene.cpp) --------------------------------
    void* portal_other_side(void* sector) {  // 0x005e4460
        if (sector == jo::self<jo::Portal>(this)->side_a) return jo::self<jo::Portal>(this)->side_b;
        if (sector == jo::self<jo::Portal>(this)->side_b) return jo::self<jo::Portal>(this)->side_a;
        return nullptr;
    }
    // 0x005e54b0: true unless some plane has the portal (+0x40) entirely behind it.
    std::uint8_t portal_inside(const std::uint8_t* planes, std::int32_t count) {
        for (std::int32_t i = 0; i < count; ++i) {
            float near_d = 0.0f, far_d = 0.0f;
            joan::orig::box_plane_extent(joan::orig::arg::mptr(planes + (i << 4)), joan::orig::arg::mptr(jo::self<jo::Portal>(this)->box), &near_d, &far_d);  // 0x0060dc80
            if (far_d < 0.0f || far_d != far_d) return 0;  // x87: unordered counts as "below"
        }
        return 1;
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Portal::other_side", 0x005e4460, &H::portal_other_side);
HELPER("Portal::inside", 0x005e54b0, &H::portal_inside);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/en3d_helpers4.cpp (en3d/helpers4)
namespace {
namespace en3d_helpers4 {

bool below_or_nan(float a, float b) { return !(a >= b); }

using joan::orig::helpers::plane_distance;
float* plane_from_points(float* out, const void* a, const void* b, const void* c) {  // 0x0060d920
    return joan::orig::arg::as<float*>(joan::orig::plane_from_points(joan::orig::arg::mptr(out), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b), joan::orig::arg::ptr(c)));
}

struct Vec3 {
    float x, y, z;
};

JOAN_COUNTER(c_portal_clip, "Portal::clip");
JOAN_COUNTER(c_portal_clip_frame, "Portal::clip fix/uninit (more than 6 planes, frame emulation)");

struct H {
    // --- Portal (between plane_sh.cpp and scene.cpp) --------------------------------------------------
    // 0x005e4ee0: seen from `sector` through this portal (4 corners at +0 / +0xc / +0x18 / +0x24),
    // build the clip planes for the far side: an edge plane through the eye for each portal edge
    // not entirely outside the camera frustum (planes 1-5) nor the incoming planes, plus the
    // incoming planes that cut an edge. False when the portal does not lead anywhere / does not
    // face the camera.
    bool portal_clip(void* sector, void* camera, const float* planes, std::int32_t count, float* out,
                     std::int32_t* out_count) {
        c_portal_clip.hit();
        void* const other = joan::orig::arg::as<void*>(joan::orig::as_portal(this)->other_side(joan::orig::arg::mptr(sector)));  // 0x005e4460
        if (other == nullptr) return false;
        if (!joan::orig::arg::as<bool>(joan::orig::as_portal(this)->facing(joan::orig::arg::mptr(sector), joan::orig::arg::mptr(camera)))) return false;    // 0x005e44b0
        if (count > 6) {
            // The original keeps one byte per incoming plane in a 6-byte stack array: more than
            // 6 planes overrun it. joan.exe's body no longer exists (stage S5): the frame emulation below.
            return portal_clip_frame(other, sector, camera, planes, count, out, out_count);
        }
        std::uint8_t keep[6] = {};
        std::uint32_t outside_camera[4], outside_in[4];
        const float* const corners = jo::self<jo::Portal>(this)->corners;
        for (std::int32_t k = 0; k < 4; ++k) {
            outside_camera[k] = 0;
            outside_in[k] = 0;
            float plane[4];
            for (std::int32_t j = 1; j < 6; ++j) {
                joan::orig::as_camera(camera)->plane_617150(joan::orig::arg::i32(j), joan::orig::arg::mptr(plane));  // 0x00617150
                const float d = plane_distance(plane, corners + k * 3);
                if (below_or_nan(d, 0.0f)) outside_camera[k] |= 1u << j;
            }
            for (std::int32_t j = 0; j < count; ++j) {
                const float d = plane_distance(planes + (j << 2), corners + k * 3);
                if (below_or_nan(d, 0.0f)) outside_in[k] |= 1u << j;
            }
        }
        *out_count = 0;
        if (jo::view<std::uint32_t>(jo::as_camera(camera)->params.projection) == 0) {
            float plane[4];
            Vec3 a, b, c, centre;
            const Vec3 s1 = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&a, joan::orig::arg::ptr(corners), joan::orig::arg::ptr(corners + 3)));  // 0x0060cc20 add
            const Vec3 s2 = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&b, &s1, joan::orig::arg::ptr(corners + 6)));
            const Vec3 s3 = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&c, &s2, joan::orig::arg::ptr(corners + 9)));
            const Vec3 mid = *joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(&centre, &s3, 0.25f));  // 0x0060cd00 scale
            const void* const eye = jo::as_camera(camera)->params.position;
            plane_from_points(plane, eye, corners, corners + 3);
            const bool flip = below_or_nan(plane_distance(plane, &mid), 0.0f);
            for (std::int32_t k = 0; k < 4; ++k) {
                const std::int32_t a_i = k, b_i = (k + 1 == 4) ? 0 : k + 1;
                const std::uint32_t both = outside_in[a_i] & outside_in[b_i];
                if (both == 0) {
                    if ((outside_camera[a_i] & outside_camera[b_i]) != 0) continue;
                    plane_from_points(plane, eye, corners + a_i * 3, corners + b_i * 3);
                    if (flip) {
                        plane[0] = -plane[0];
                        plane[1] = -plane[1];
                        plane[2] = -plane[2];
                        plane[3] = -plane[3];
                    }
                    std::memcpy(out + (*out_count << 2), plane, 16);
                    *out_count = *out_count + 1;
                } else {
                    for (std::int32_t j = 0; j < count; ++j)
                        if ((both & (1u << j)) != 0) keep[j] = 1;
                }
            }
        }
        for (std::int32_t j = 0; j < count; ++j) {
            if (keep[j] == 0) continue;
            std::memcpy(out + (*out_count << 2), planes + (j << 2), 16);
            *out_count = *out_count + 1;
        }
        return true;
    }

    // 0x005e4ee0 with count > 6 (fix/uninit; from the asm, frame 0x12c): keep[] is the 6-byte
    // array at [ebp-0x10] (memset 6), so keep[j] for j >= 6 is the frame above it. Emulated as
    // the 48 bytes [ebp-0x10, ebp+0x20): keep[0..5], 2 padding bytes [ebp-0xa] (never written:
    // stack garbage in the original -> 0 here, the fix/uninit value), [ebp-8] = the 0x005e4460
    // result (dead after the entry test), [ebp-4] = this, saved ebp, return address, then the
    // six arguments. Every read the original makes of [ebp-4] / [ebp+0xc..0x1c] after the memset
    // reads the emulated bytes, so keep writes into `this`, camera, planes, count, out, out_count
    // change the rest of the run exactly as in the original. Writes to the saved ebp / return
    // address bytes (keep[16..23]) and above the arguments (keep[48..], the caller's frame) are
    // dropped and those bytes read as 0 (in the original: a crash on return / the caller's data).
    // Bit masks: shl by cl = 1 << (j & 31) as on x86. Everything else as the normal path.
    bool portal_clip_frame(void* other, void* sector, void* camera, const float* planes, std::int32_t count,
                           float* out, std::int32_t* out_count) {
        c_portal_clip_frame.hit();
        // x64: emulates joan.exe's x86 stack frame at ebp offsets; no layout member applies. The pointer slots hold
        // their own frame offset as a 4-byte token (a pointer does not fit the dword) and the pointer is looked up
        // by it, so the keep[] bytes still overlay the words as in the original (a damaged token reads as null,
        // where the original used a damaged pointer).
        std::uint8_t fr[48] = {};  // [ebp-0x10 + i]
        const void* slot_ptr[48] = {};
        const auto put = [&fr, &slot_ptr](std::int32_t i, const void* v) {
            slot_ptr[i] = v;
            const std::uint32_t token = static_cast<std::uint32_t>(i);
            std::memcpy(fr + i, &token, 4);
        };
        put(8, other);
        put(12, this);
        put(24, sector);
        put(28, camera);
        put(32, planes);
        std::memcpy(fr + 36, &count, 4);
        put(40, out);
        put(44, out_count);
        const auto ld = [&fr](std::int32_t i) {
            std::uint32_t v;
            std::memcpy(&v, fr + i, 4);
            return v;
        };
        const auto slot = [&](std::int32_t i) -> const void* { return ld(i) == static_cast<std::uint32_t>(i) ? slot_ptr[i] : nullptr; };
        const auto p_this = [&]() { return static_cast<const std::uint8_t*>(slot(12)); };
        const auto p_camera = [&]() { return static_cast<std::uint8_t*>(const_cast<void*>(slot(28))); };
        const auto n_count = [&]() { return static_cast<std::int32_t>(ld(36)); };
        // out + (n << 4) / planes + (j << 4) bytes (x64: the 32-bit wrap-around of the original is moot, the base is a token lookup).
        const auto out_slot = [&](std::int32_t n) {
            return static_cast<float*>(const_cast<void*>(slot(40))) + static_cast<std::ptrdiff_t>(n) * 4;
        };
        const auto plane_at = [&](std::int32_t j) {
            return static_cast<const float*>(slot(32)) + static_cast<std::ptrdiff_t>(j) * 4;
        };
        const auto p_out_count = [&]() { return static_cast<std::int32_t*>(const_cast<void*>(slot(44))); };

        const auto keep_set = [&fr](std::int32_t j) {  // mov byte [ebp+j-0x10], 1
            if (j < 48 && !(j >= 16 && j < 24)) fr[j] = 1;
        };
        const auto keep_get = [&fr](std::int32_t j) -> std::uint8_t { return j < 48 ? fr[j] : 0; };
        const auto bit = [](std::int32_t j) { return 1u << (static_cast<std::uint32_t>(j) & 31u); };

        std::uint32_t outside_camera[4], outside_in[4];
        for (std::int32_t k = 0; k < 4; ++k) {
            outside_camera[k] = 0;
            outside_in[k] = 0;
            float plane[4];
            for (std::int32_t j = 1; j < 6; ++j) {
                joan::orig::as_camera(p_camera())->plane_617150(joan::orig::arg::i32(j), joan::orig::arg::mptr(plane));  // 0x00617150
                const float d = plane_distance(plane, jo::as_portal(p_this())->corners + k * 3);
                if (below_or_nan(d, 0.0f)) outside_camera[k] |= bit(j);
            }
            for (std::int32_t j = 0; j < n_count(); ++j) {
                const float d = plane_distance(plane_at(j), jo::as_portal(p_this())->corners + k * 3);
                if (below_or_nan(d, 0.0f)) outside_in[k] |= bit(j);
            }
        }
        *p_out_count() = 0;
        if (jo::view<std::uint32_t>(jo::as_camera(p_camera())->params.projection) == 0) {
            float plane[4];
            Vec3 a, b, c, centre;
            const float* const corners = jo::as_portal(p_this())->corners;
            const Vec3 s1 = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&a, joan::orig::arg::ptr(corners), joan::orig::arg::ptr(corners + 3)));  // 0x0060cc20 add
            const Vec3 s2 = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&b, &s1, joan::orig::arg::ptr(corners + 6)));
            const Vec3 s3 = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&c, &s2, joan::orig::arg::ptr(corners + 9)));
            const Vec3 mid = *joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(&centre, &s3, 0.25f));  // 0x0060cd00 scale
            plane_from_points(plane, jo::as_camera(p_camera())->params.position, corners, corners + 3);
            const bool flip = below_or_nan(plane_distance(plane, &mid), 0.0f);
            for (std::int32_t k = 0; k < 4; ++k) {
                const std::int32_t a_i = k, b_i = (k + 1 == 4) ? 0 : k + 1;
                const std::uint32_t both = outside_in[a_i] & outside_in[b_i];
                if (both == 0) {
                    if ((outside_camera[a_i] & outside_camera[b_i]) != 0) continue;
                    plane_from_points(plane, jo::as_camera(p_camera())->params.position, jo::as_portal(p_this())->corners + a_i * 3, jo::as_portal(p_this())->corners + b_i * 3);
                    if (flip) {
                        plane[0] = -plane[0];
                        plane[1] = -plane[1];
                        plane[2] = -plane[2];
                        plane[3] = -plane[3];
                    }
                    std::memcpy(out_slot(*p_out_count()), plane, 16);
                    *p_out_count() = *p_out_count() + 1;
                } else {
                    for (std::int32_t j = 0; j < n_count(); ++j)
                        if ((both & bit(j)) != 0) keep_set(j);
                }
            }
        }
        for (std::int32_t j = 0; j < n_count(); ++j) {
            if (keep_get(j) == 0) continue;
            std::memcpy(out_slot(*p_out_count()), plane_at(j), 16);
            *p_out_count() = *p_out_count() + 1;
        }
        return true;
    }
};

#define HELPER4(name, addr, fn) JOAN_REPLACE("en3d/helpers4", name, addr, fn)
HELPER4("Portal::clip", 0x005e4ee0, &H::portal_clip);
#undef HELPER4

}  // namespace en3d_helpers4
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Portal::other_side(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::portal_other_side>(this, a0)); }  // 0x5e4460
auto joan::orig::Portal::clip(void* a0, void* a1, void* a2, std::int32_t a3, void* a4, void* a5) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::en3d_helpers4::H::portal_clip>(this, a0, a1, a2, a3, a4, a5)); }  // 0x5e4ee0
auto joan::orig::Portal::inside(void* a0, std::int32_t a1) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::en3d_helpers::H::portal_inside>(this, a0, a1)); }  // 0x5e54b0
