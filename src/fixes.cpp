// [Fixes] that change original behaviour by replacing or patching original functions which are
// otherwise left as they are. Installed after the replacements; nothing is touched while an
// option is 0, so the game then runs exactly as the original.
//
// MouseTurn: a modern version of the mouse look of the hero / camera, 0x00431790
// (thiscall on the player controller, 1 stack argument, ret 4; called once per frame).
// The original turns by _ftol(counts * sensitivity) whole degrees per frame (DirectInput relative
// counts 0x00a516f4 / 0x00a516f8, sensitivity ((option + 1) * 0.7 + 0.2) * 0.2, 0.4 mounted) and
// rounds the hero's heading back to whole degrees every frame. At high frame rates slow mouse
// movement turned nothing and any turn moved in 1-degree steps. Here the turn is kept in floating
// point (no rounding of the delta or the heading), with the same sensitivity, the same limits
// (45 degrees per frame for unit kind 0x1c, pitch -20..35 degrees), the same camera / terrain logic.
// With MouseTurn=0 nothing is installed here; the exact C++ version of the original is
// game/_unplaced/after_camres.cpp (game/b115_04).
#include "platform/win32_types.hpp"

#include <cmath>
#include <cstdint>
#include <cstring>

#include "original/direct.hpp"
#include "original/direct.hpp"
#include "data_names.hpp"
#include "fixes.hpp"
#include "fn_names.hpp"
#include "runtime/patch.hpp"
#include "registry.hpp"
#include "original/layout_access.hpp"

namespace jo = joan::orig;

namespace {

struct Obj {};
using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::global;

struct V3 {
    float x, y, z;
};

constexpr img::Addr kUnits(d::g_units);
constexpr img::Addr kCameraMan(d::g_camera_man);
constexpr img::Addr kWorld(d::g_world);
constexpr float kDegToRad = 0.0174532924f;  // 0x009cf884

JOAN_COUNTER(c_turn, "MouseTurn::update (fix)");

struct Turn {
    // 0x00431790 PlayerControl::mouse_look(arg) - the [Fixes] MouseTurn version.
    void update(std::int32_t /*arg*/) {
        joan::orig::Player* const self = joan::orig::as_player(this);
        if (joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(self->unit_id_)) != 0) return;  // 0x00428b70
        joan::orig::Unit* const unit = joan::orig::as_unit(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(self->unit_id_)));  // 0x0041b3c0
        const std::int32_t kind = unit->anim_kind;
        const bool steerable = (joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(unit)->can_act()) & 0xff) != 0 ||          // 0x00500ba0
                               kind == 0x49 || kind == 0x1c || kind == 0x1d;
        if (!steerable) return;
        if (unit->action == 1) return;
        if (kind == 0x51 || kind == 0x52 || kind == 0x4f || kind == 0x50) return;
        c_turn.hit();

        // Sensitivity in degrees per mouse count (the original's float arithmetic).
        const float option = joan::orig::arg::as<float>(joan::orig::as_player(self)->look_sensitivity());  // 0x004322d0
        const float base = static_cast<float>((static_cast<double>(option + 1.0f)) * 0.7 + 0.2);
        const float sens = base * (jo::view<std::int8_t>(self->mounted) != 0 ? 0.4f : 0.2f);

        alignas(4) std::uint8_t cam[sizeof(joan::orig::CamParams)];  // camera motion parameters: position = target, orbit = yaw, pitch, distance, scale
        {
            const std::int32_t index = joan::orig::arg::as<std::int32_t>(joan::orig::as_camera_follow(img::obj(kCameraMan))->target());  // 0x00563dc0
            void* const motion = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(img::obj(kCameraMan))->motion(joan::orig::arg::i32(index)));       // 0x00563c80
            joan::orig::as_camera_motion(motion)->end(joan::orig::arg::mptr(static_cast<void*>(cam)));                      // 0x00423fe0 copy out
        }

        const std::int32_t dx = global<std::int32_t>(d::g_mouse_look_dx);
        const std::int32_t dy = global<std::int32_t>(d::g_mouse_dy);
        float delta = 0.0f;  // degrees
        if (dx != 0) {
            delta = static_cast<float>(dx) * sens;
            if (kind == 0x1c) delta = delta > 45.0f ? 45.0f : delta < -45.0f ? -45.0f : delta;
            float heading = joan::orig::arg::as<float>(joan::orig::as_unit(unit)->float_1e()) - delta;  // 0x0050ac00
            heading = std::fmod(heading, 360.0f);
            if (heading < 0.0f) heading += 360.0f;
            joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(heading));               // 0x0050abd0 (degrees)
            joan::orig::as_unit(unit)->set_target(joan::orig::arg::f32(heading * kDegToRad));   // 0x004ff870 (radians)
        }

        if (jo::view<std::int8_t>(self->follow_mode) != 0) return;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_camera_follow(img::obj(kCameraMan))->target()) != 0) return;  // 0x00563dc0

        float& yaw = joan::orig::as_cam_params(cam)->orbit[0];
        float& pitch = joan::orig::as_cam_params(cam)->orbit[1];
        if (dx != 0) yaw -= delta * kDegToRad;
        if (dy != 0) {
            const float saved = pitch;
            const float invert = global<std::int8_t>(d::g_cfg_invert_look) != 0 ? -1.0f : 1.0f;
            pitch += static_cast<float>(dy) * sens * kDegToRad * invert;
            if (pitch > 0.610865235f) pitch = 0.610865235f;        // 0x3f1c61aa
            if (!(pitch >= -0.34906584f)) pitch = -0.34906584f;     // 0xbeb2b8c2
            if (static_cast<float>(dy) * invert < 0.0f) {
                // Moving the camera down: keep it above the terrain (as the original).
                // 0x0060bd00 writes a full 4x4 matrix (rep movsd x16) and 0x0060c220 reads that matrix.
                float dir[16] = {};
                V3 unit_dir{}, scaled{}, pos{};
                joan::orig::orbit_matrix(joan::orig::arg::mptr(static_cast<void*>(dir)), joan::orig::arg::f32(yaw), joan::orig::arg::f32(pitch), joan::orig::arg::f32(joan::orig::as_cam_params(cam)->orbit[2]), 0.0f);  // 0x0060bd00
                joan::orig::matrix_axis_z(&unit_dir, static_cast<void*>(dir));             // 0x0060c220
                joan::orig::vec_scale(&scaled, &unit_dir, joan::orig::arg::f32(joan::orig::as_cam_params(cam)->orbit[3]));        // 0x0060cd00
                const V3 target = *reinterpret_cast<const V3*>(joan::orig::as_cam_params(cam)->position);
                joan::orig::vec_sub(&pos, &target, &scaled);                          // 0x0060cc90
                // 0x00429090 reads a box (min, max): the degenerate box {pos, pos}, as the original builds it.
                V3 pos_box[2] = {pos, pos};
                std::int32_t count = 0;
                void** const cells = joan::orig::arg::as<void**>(joan::orig::as_world(*static_cast<void**>(img::obj(kWorld)))->cells_in_box(pos_box, &count));  // 0x00429090 (this = [0xa45f3e])
                if (count != 0) {
                    joan::orig::WorldCell* const cell = joan::orig::as_world_cell(cells[0]);
                    joan::orig::PathSpace* const tile = joan::orig::as_path_space(joan::orig::as_world_cell(cell)->grid(joan::orig::arg::i16(0)));                   // 0x005600c0
                    if (tile != nullptr) {
                        const float fx = (pos.x - static_cast<float>(cell->origin_x)) / static_cast<float>(tile->cell_w);
                        const float fy = (pos.y - static_cast<float>(cell->origin_y)) / static_cast<float>(tile->cell_h);
                        const std::int32_t parity = ((static_cast<std::int32_t>(fx) + static_cast<std::int32_t>(fy)) % 2) == 0 ? 1 : 0;
                        const float ground = joan::orig::arg::as<float>(joan::orig::as_terrain(jo::view<void*>(tile->grid))->height_4c7b70(joan::orig::arg::f32(fx), joan::orig::arg::f32(fy), static_cast<char>(joan::orig::arg::i32(parity))));  // 0x004c7b70
                        if (ground > pos.z) pitch = saved;
                    }
                }
            }
        }
        void* const motion0 = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(img::obj(kCameraMan))->motion(0));  // 0x00563c80
        joan::orig::as_camera_motion(motion0)->retarget(joan::orig::arg::mptr(static_cast<void*>(cam)));            // 0x00423f40 copy in
    }
};

}  // namespace

namespace joan::fixes {

void install() {
    if (hook::settings().fix_mouse_turn) {
        // With module game/b115_04 enabled the thunk 0x0040437c already jumps to its exact C++
        // version of 0x00431790 (install_all runs first); retarget whatever the thunk points at, so
        // each thunk is redirected exactly once more and the fix wins either way.
        const void* const fix = hook::code_address(&Turn::update);
        const void* const exact = hook::installed_replacement(0x00431790);
        int n = hook::redirect_function(0x00431790, fix);
        if (exact != nullptr) n += hook::redirect_replacement(exact, fix);
        hook::log_line("[Fixes] MouseTurn: modern mouse look installed (%d thunk%s, 0x00431790)", n, n == 1 ? "" : "s");
    }
}

}  // namespace joan::fixes
