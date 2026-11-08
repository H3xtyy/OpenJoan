// game/camman.cpp: functions of the original d:\ww\camman.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include "../compiler.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "pad_feedback.hpp"
#include "../build_flavor.hpp"
#include "../runtime/profiler.hpp"
#include <cmath>
#include <string>

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using u32 = std::uint32_t;

using u8 = std::uint8_t;

using joan::call::cdecl_call;

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
//
//    - CamMan::init_423600 0x00423600: 0x00423e70, +0x98 = 1.0f, +0xc0 = alloc 0x11 bytes
//      (0x0058e7b0, camman.cpp, line [0x9f7728] + 5) zeroed, +0xc4 = 0.
//
//    - CamMan::dtor_425520 0x00425520: 0x006e8b90, Widget::~Widget on +0xd38 / +0x9f2 /
//      +0x6ac / +0x366 / +0x20 (SEH frame: normal path only).
namespace {
namespace game_b100_01 {

void widget_dtor(void* w) { joan::orig::as_widget(w)->dtor_6211a0(); }  // 0x006211a0

JOAN_COUNTER(c_camman_init, "CamMan::init_423600");

struct H {
    // 0x00423600 (eax left = the allocated block, returned as well)
    void* init_423600() {
        c_camman_init.hit();
        joan::orig::as_camera_motion(this)->set_default_modes();  // 0x00423e70
        jo::view<std::uint32_t>(jo::self<jo::CameraMotion>(this)->t) = 0x3f800000u;
        const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_9f7728)) + 5;
        jo::view<std::uint8_t*>(jo::self<jo::CameraMotion>(this)->tracked) =
            joan::orig::arg::as<std::uint8_t*>(joan::orig::as_mem(obj(d::g_mem))->add(static_cast<std::uint32_t>(sizeof(jo::CamTrackNode)), str(d::s_d_ww_camman_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0 (0x11 on x86)
        jo::self<jo::CameraMotion>(this)->tracked->u32_0 = nullptr;
        jo::self<jo::CameraMotion>(this)->tracked->prev = nullptr;
        jo::self<jo::CameraMotion>(this)->tracked->next = nullptr;
        jo::self<jo::CameraMotion>(this)->tracked->marked = 0;
        void* const block = jo::view<std::uint8_t*>(jo::self<jo::CameraMotion>(this)->tracked);
        jo::view<std::uint32_t>(jo::as_cam_track_node(block)->unit) = 0;
        jo::view<std::uint32_t>(jo::self<jo::CameraMotion>(this)->tracked_count) = 0;
        return block;
    }

    // 0x00425520
    void dtor_425520() {
        joan::orig::as_victory_screen(this)->release();  // 0x006e8b90
        widget_dtor(&jo::self<jo::EndScreen>(this)->joan_button);  // x86: CamMan view buttons at +0xd38 / +0x9f2 / +0x6ac / +0x366 / +0x20; the object is g_a46ad0, an EndScreen
        widget_dtor(&jo::self<jo::EndScreen>(this)->es_button);
        widget_dtor(&jo::self<jo::EndScreen>(this)->quit_button);
        widget_dtor(&jo::self<jo::EndScreen>(this)->next1_button);
        widget_dtor(&jo::self<jo::EndScreen>(this)->next_button);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("CamMan::init_423600", 0x00423600, &H::init_423600);
B("CamMan::dtor_425520", 0x00425520, &H::dtor_425520);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b86_05.cpp (game/b86_05)
//
//    - CamMan::free_list 0x004236d0 (+0xc0 list, "d:\ww\camman.cpp").
//
//  - Leftover eax values that are well defined (this, a callee's result) are returned:
//    0x00461f10 / 0x0043ac60 / the setters leave this, 0x004214b0 / 0x00417aa0 their last
//    callee's result, 0x004236d0 this (empty list) or the last 0x0058ed70 result.
//    0x00432300 / 0x00438f80 return void (callers never read eax).
namespace {
namespace game_b86_05 {

void* const kMemory = obj(d::g_mem);     // allocator (0x0058ed70 free)
const char* const kCammanCpp = str(d::s_d_ww_camman_cpp);  // "d:\ww\camman.cpp"

std::uint32_t mem_free(void* p, const char* file, std::int32_t line) {
    return thiscall<std::uint32_t>(fn::del, kMemory, p, file, line);  // 0x0058ed70
}

JOAN_COUNTER(c_free_list, "CamMan::free_list");

struct H {
    // ---- 0x004236d0 CamMan::free_list --------------------------------------------------------
    std::uintptr_t free_list() {
        c_free_list.hit();
        std::uintptr_t eax = reinterpret_cast<std::uintptr_t>(this);
        std::uint8_t* cur = jo::view<std::uint8_t*>(jo::self<jo::CameraMotion>(this)->tracked);
        while (cur != nullptr) {
            std::uint8_t* old = cur;
            if (jo::as_cam_track_node(cur)->next != nullptr) {
                cur = jo::view<std::uint8_t*>(jo::as_cam_track_node(cur)->next);
                jo::as_cam_track_node(cur)->prev = nullptr;
                mem_free(old, kCammanCpp, static_cast<std::int32_t>(global<std::int16_t>(d::g_9f772c)) + 0xf);
                old = nullptr;
                jo::view<std::uint8_t*>(jo::self<jo::CameraMotion>(this)->tracked) = cur;
                eax = reinterpret_cast<std::uintptr_t>(this);
            } else {
                eax = mem_free(cur, kCammanCpp, static_cast<std::int32_t>(global<std::int16_t>(d::g_9f772c)) + 0x15);
                cur = nullptr;
            }
            (void)old;
        }
        return eax;
    }
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_05", name, addr, fn)
B86("CamMan::free_list", 0x004236d0, &H::free_list);
#undef B86

}  // namespace game_b86_05
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
//
//    - CamMan::~dtor_423d20 0x00423d20 (ret 0, camman.cpp): base 0x006c27c0, then member dtors
//      0x005e9a90 (+0x148, +0x5f), 0x00495980 (+0x38, +0x1a); eax of the last one returned.
//      SEH unwind frame: normal path only.
namespace {
namespace game_b108_19 {

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x00423d20
    u32 dtor_423d20() {
        joan::orig::as_shop(this)->release_6c27c0();                // 0x006c27c0
        joan::orig::as_scene(&jo::as_buy_screen(this)->stage_shop)->dtor();    // 0x005e9a90
        joan::orig::as_scene(&jo::as_buy_screen(this)->stage_player)->dtor();     // 0x005e9a90
        joan::orig::as_unit_data(&jo::as_buy_screen(this)->items)->destruct();     // 0x00495980
        return joan::orig::arg::as<u32>(joan::orig::as_unit_data(&jo::as_buy_screen(this)->items_b)->destruct());  // 0x00495980
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("CamMan::dtor_423d20", 0x00423d20, &H::dtor_423d20);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
//
//      0x00423db0 Obj::~Obj_423db0              (thiscall, ret 0) - SEH frame, normal path.
//
//  - 0x00423db0 / 0x00426280 / 0x00416f60: SEH frames for unwinding member destructors only;
//    normal path reproduced.
namespace {
namespace game_b95_01 {

JOAN_COUNTER(c_dtor_423db0, "Obj::~Obj_423db0");

struct H {
    // ------------------------------------------------------------------ 0x00423db0
    void dtor_423db0() {  // this = the quick save screen (0xa5d408)
        c_dtor_423db0.hit();
        joan::orig::as_panel(this)->reset_6e37d0();                // 0x006e37d0
        joan::orig::as_widget(&jo::self<jo::QuickBar>(this)->loadc_bar)->dtor_6294c0();      // 0x006294c0
        joan::orig::as_widget(&jo::self<jo::QuickBar>(this)->loadc_text)->dtor_6294c0();
        joan::orig::as_widget(&jo::self<jo::QuickBar>(this)->load_bar)->dtor_6294c0();
        joan::orig::as_widget(&jo::self<jo::QuickBar>(this)->load_text)->dtor_6294c0();
        joan::orig::as_widget(&jo::self<jo::QuickBar>(this)->save_bar)->dtor_6294c0();
        joan::orig::as_widget(&jo::self<jo::QuickBar>(this)->save_text)->dtor_6294c0();
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::~Obj_423db0", 0x00423db0, &H::dtor_423db0);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
//
//      0x00423e70 Obj::set_flags_78_88_a4                   (thiscall, ret 0)
namespace {
namespace game_b99_01 {

struct H {
    void set_flags_78_88_a4() {
        jo::self<jo::CameraMotion>(this)->time_mode = 1;
        jo::self<jo::CameraMotion>(this)->blend_mode = 1;
        jo::self<jo::CameraMotion>(this)->skip_collision = 1;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::set_flags_78_88_a4", 0x00423e70, &H::set_flags_78_88_a4);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

constexpr img::Addr kFiles(d::g_misc);
std::uint32_t clock_ms() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(img::obj(kFiles))->get_time_591050()); }  // 0x00591050 timeGetTime wrapper

struct H {
    void* motion_end(void* out) {                                                        // 0x00423fe0
        std::memcpy(out, &jo::self<jo::CameraMotion>(this)->to, sizeof(jo::CamPose));
        return out;
    }
    void motion_start(const void* start, const void* end) {                              // 0x00423ec0
        std::memcpy(&jo::self<jo::CameraMotion>(this)->from, start, sizeof(jo::CamPose));
        std::memcpy(&jo::self<jo::CameraMotion>(this)->current_, &jo::self<jo::CameraMotion>(this)->from, sizeof(jo::CamPose));
        std::memcpy(&jo::self<jo::CameraMotion>(this)->to, end, sizeof(jo::CamPose));
        jo::view<std::uint32_t>(jo::self<jo::CameraMotion>(this)->t) = 0;
        jo::self<jo::CameraMotion>(this)->start_ms = clock_ms();
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("CameraMotion::end", 0x00423fe0, &H::motion_end);
W1("CameraMotion::start", 0x00423ec0, &H::motion_start);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_camera.cpp (game/camera1)
//
//  * module game/camera1: CameraZone::update 0x00425fc0, Zones::find 0x00429bb0,
//    CameraMan::set_mode 0x00429860 / restore_mode 0x004297d0, World::cells_in_box
//    0x00429090, CameraMotion::retarget 0x00423f40 / current 0x00423fb0, the tracked-unit
//    list (0x00425720 add, 0x00425840 mark, 0x004258a0 unmark all, 0x004258f0 sweep),
//    small getters (0x00425ce0, 0x00425d90, 0x006af310, 0x006b0790), Effect::set_owner
//    0x0041b0d0, lerp 0x00427bb0, Hermite 0x004279b0.
namespace {
namespace game_camera {

constexpr img::Addr kUnits(d::g_items);
constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kCammanCpp(d::s_d_ww_camman_cpp);  // "d:\ww\camman.cpp"
std::uint32_t clock_ms() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(img::obj(d::g_misc))->get_time_591050()); }  // 0x00591050

JOAN_COUNTER(c_sweep, "CameraMan::sweep_tracked");

// Debug builds: who turns the follow camera away from the angle the pad's right stick holds it at (openjoan.log,
// "[Pad] camera turned by ..." with the callers), a few times a second at most.
[[maybe_unused]] void report_moved_hold(const void* end) {
    const joan::padfx::CameraHold& hold = joan::padfx::g_camera_hold;
    const std::uint32_t now = global<std::uint32_t>(d::g_now);
    if (hold.motion == nullptr || now - hold.ms > 100u) return;
    const jo::CamPose* const to = static_cast<const jo::CamPose*>(end);
    float dyaw = to->target[0] - hold.yaw;
    if (dyaw > 3.1415927f) dyaw -= 6.2831855f;
    if (dyaw < -3.1415927f) dyaw += 6.2831855f;
    const float dpitch = to->target[1] - hold.pitch;
    if (std::fabs(dyaw) < 0.01f && std::fabs(dpitch) < 0.01f) return;
    static std::uint32_t last = 0, lines = 0;
    if (now - last < 300u || lines >= 60u) return;
    last = now;
    ++lines;
    void* frames[6] = {};
    const unsigned n = JOAN_STACK_TRACE(2, 6, frames);
    std::string who;
    for (unsigned i = 0; i < n; ++i) who += (i ? " < " : "") + joan::prof::symbol_at(reinterpret_cast<std::uintptr_t>(frames[i]));
    joan::hook::log_line("[Pad] camera turned by yaw %.1f pitch %.1f degrees: %s", dyaw * 57.29578f, dpitch * 57.29578f, who.c_str());
}

struct H {
    std::uint32_t motion_retarget(const void* end) {  // 0x00423f40: start = current, new end
        if constexpr (!joan::kReleaseBuild) report_moved_hold(end);
        std::memcpy(&jo::self<jo::CameraMotion>(this)->from, &jo::self<jo::CameraMotion>(this)->current_, sizeof(jo::CamPose));
        std::memcpy(&jo::self<jo::CameraMotion>(this)->to, end, sizeof(jo::CamPose));
        jo::view<std::uint32_t>(jo::self<jo::CameraMotion>(this)->t) = 0;
        const std::uint32_t now = clock_ms();
        jo::self<jo::CameraMotion>(this)->start_ms = now;
        return now;  // eax = clock_ms() result (also stored at +0x9c)
    }
    void motion_current(void* out) { std::memcpy(out, &jo::self<jo::CameraMotion>(this)->current_, sizeof(jo::CamPose)); }  // 0x00423fb0

    // Tracked units (+0xc0 list of 0x11-byte nodes: +0 object, +4 prev, +8 next, +0xc
    // marked, +0xd unit id; count +0xc4; the last node is an empty sentinel).
    void tracked_add(void* obj) {  // 0x00425720
        std::uint8_t* n = jo::view<std::uint8_t*>(jo::self<jo::CameraMotion>(this)->tracked);
        for (std::int32_t i = 0; i < jo::self<jo::CameraMotion>(this)->tracked_count; ++i) n = jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->next);
        jo::as_cam_track_node(n)->u32_0 = obj;
        jo::as_cam_track_node(n)->marked = 1;
        jo::view<std::int32_t>(jo::as_cam_track_node(n)->unit) = jo::as_map_object(jo::as_actor(obj)->owner)->index;
        const std::int32_t line = global<std::int16_t>(d::g_9f7730) + 0xb;
        std::uint8_t* const fresh = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_mem(img::obj(kMemory))->add(static_cast<std::uint32_t>(sizeof(jo::CamTrackNode)), joan::orig::arg::str(img::str(kCammanCpp)), joan::orig::arg::i32(line)));  // 0x0058e7b0 alloc (0x11 on x86)
        jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->next) = fresh;
        jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->next->prev) = n;
        jo::as_cam_track_node(n)->next->u32_0 = nullptr;
        jo::as_cam_track_node(n)->next->next = nullptr;
        jo::as_cam_track_node(n)->next->marked = 0;
        jo::view<std::uint32_t>(jo::as_cam_track_node(n)->next->unit) = 0;
        jo::self<jo::CameraMotion>(this)->tracked_count = jo::self<jo::CameraMotion>(this)->tracked_count + 1;
    }
    void tracked_mark(void* obj) {  // 0x00425840
        for (std::uint8_t* n = jo::view<std::uint8_t*>(jo::self<jo::CameraMotion>(this)->tracked); n != nullptr; n = jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->next))
            if (jo::as_cam_track_node(n)->u32_0 == obj) {
                jo::as_cam_track_node(n)->marked = 1;
                return;
            }
    }
    void tracked_unmark_all() {  // 0x004258a0
        for (std::uint8_t* n = jo::view<std::uint8_t*>(jo::self<jo::CameraMotion>(this)->tracked); n != nullptr; n = jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->next)) jo::as_cam_track_node(n)->marked = 0;
    }
    void tracked_sweep() {  // 0x004258f0: unmarked nodes are freed (their unit's fade reset)
        c_sweep.hit();
        std::uint8_t* n = jo::view<std::uint8_t*>(jo::self<jo::CameraMotion>(this)->tracked);
        while (n != nullptr) {
            if (static_cast<std::int8_t>(jo::as_cam_track_node(n)->marked) != 0) {
                n = jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->next);
                continue;
            }
            if (jo::as_cam_track_node(n)->u32_0 != nullptr && joan::orig::arg::as<std::int32_t>(joan::orig::as_units(img::obj(kUnits))->empty_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_cam_track_node(n)->unit)))) == 0 &&  // 0x00425d50
                jo::as_map_object(joan::orig::arg::as<void*>(joan::orig::as_units(img::obj(kUnits))->get_425d20(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_cam_track_node(n)->unit)))))->actor != nullptr)                   // 0x00425d20
                joan::orig::as_actor(jo::as_map_object(joan::orig::arg::as<void*>(joan::orig::as_units(img::obj(kUnits))->get_425d20(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_cam_track_node(n)->unit)))))->actor)->set_faded(static_cast<char>(joan::orig::arg::i32(0)));    // 0x005ae590
            if (jo::view<void*>(jo::as_cam_track_node(n)->next) == nullptr) {
                n = jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->next);
                continue;
            }
            std::uint8_t* const dead = n;
            if (jo::view<void*>(jo::as_cam_track_node(n)->prev) != nullptr) jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->prev->next) = jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->next);
            if (n == jo::view<std::uint8_t*>(jo::self<jo::CameraMotion>(this)->tracked)) jo::view<std::uint8_t*>(jo::self<jo::CameraMotion>(this)->tracked) = jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->next);
            jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->next->prev) = jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->prev);
            n = jo::view<std::uint8_t*>(jo::as_cam_track_node(n)->next);
            joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(static_cast<void*>(dead)), joan::orig::arg::str(img::str(kCammanCpp)), joan::orig::arg::i32(global<std::int16_t>(d::g_9f7734) + 0x19));  // 0x0058ed70 free
            jo::self<jo::CameraMotion>(this)->tracked_count = jo::self<jo::CameraMotion>(this)->tracked_count - 1;
        }
    }
};

#define C1(name, addr, fn) JOAN_REPLACE("game/camera1", name, addr, fn)
C1("CameraMotion::retarget", 0x00423f40, &H::motion_retarget);
C1("CameraMotion::current", 0x00423fb0, &H::motion_current);
C1("CameraMan::track", 0x00425720, &H::tracked_add);
C1("CameraMan::mark_tracked", 0x00425840, &H::tracked_mark);
C1("CameraMan::unmark_tracked", 0x004258a0, &H::tracked_unmark_all);
C1("CameraMan::sweep_tracked", 0x004258f0, &H::tracked_sweep);
#undef C1

}  // namespace game_camera
}  // namespace

// ---------------------------------------- from modules/game_camzone.cpp (game/camera3)
//
//  * module game/camera3: CameraMotion::collide 0x00424010 - pulls the orbit camera in front
//    of scene meshes, actors and ground objects between it and its target, fading actors
//    that block the view.
namespace {
namespace game_camzone {

using joan::call::import_call;

struct V3 {
    float x, y, z;
};
V3 load3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}
float dot(V3 a, V3 b) { return static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(&a), static_cast<void*>(&b))); }  // v3d returns a double (x64: xmm0)
bool equal_or_nan(float a, float b) { return !(a < b || a > b); }

JOAN_COUNTER(c_collide, "CameraMotion::collide");

struct H {
    // Triangle tri (1-based) of a mesh plane in world space.
    static void triangle(std::uint8_t* plane, std::int32_t tri, V3 v[3]) {
        std::int32_t stride = 0;
        std::uint8_t* const verts = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(plane)->buffer(&stride));  // 0x005cdae0
        const std::uint16_t* const idx = jo::as_plane(plane)->data_pending != 0 ? jo::view<std::uint16_t*>(jo::as_plane(plane)->source->triangles)
                                                                       : jo::view<std::uint16_t*>(jo::as_plane(plane)->triangles);
        for (std::int32_t k = 0; k < 3; ++k) v[k] = load3(verts + stride * idx[(tri - 1) * 3 + k]);
        if (jo::as_plane(plane)->transformed == 0)
            for (std::int32_t k = 0; k < 3; ++k) {
                void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_plane(plane)->bone_5cf8d0()))->matrix());  // 0x0060ef80(0x005cf8d0)
                import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(&v[k]), m);                      // point_transform (in place)
            }
    }
    // The distance limit from the hit plane's normal: r stays unless |r / (n . d)| is larger.
    static float plane_limit(const V3& n, const V3& d, float r) {
        float limit = r;
        const float nd = dot(n, d);
        if (!equal_or_nan(nd, 0.0f)) {
            const double a = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(r / nd))));
            if (a < static_cast<double>(limit) || a != a)
                limit = static_cast<float>(static_cast<double>(limit));
            else
                limit = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(r / nd)))));
        }
        return limit;
    }

    // 0x00424010 CameraMotion::collide: this +0x54 target, +0x5c height offset, +0x60..+0x68
    // angles, +0x6c distance, +0xa8 / +0xac / +0xb0 and +0xb4 / +0xb8 / +0xbc limits for mesh /
    // actor hits, +0xc0 tracked (faded) actors.
    void collide() {
        c_collide.hit();
        const joan::hook::x87::Fpu fpu{true};
        const float r = 4.0f;
        void* const scene = jo::view<void*>(jo::as_world(global<void*>(d::g_world))->scene);
        float m[16];
        joan::orig::orbit_matrix(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::f32(jo::self<jo::CameraMotion>(this)->current_.target[0]), joan::orig::arg::f32(jo::self<jo::CameraMotion>(this)->current_.target[1]), joan::orig::arg::f32(jo::self<jo::CameraMotion>(this)->current_.target[2]), joan::orig::arg::f32(0u));  // 0x0060bd00
        V3 d;
        joan::orig::matrix_axis_z(static_cast<void*>(&d), static_cast<void*>(m));  // 0x0060c220
        float k = jo::view<float>(jo::self<jo::CameraMotion>(this)->current_.u32_1c) + 5.0f;
        // seg_tc = {target, camera}, seg_ct = {camera, target}
        float tc[6];
        std::memcpy(tc, jo::self<jo::CameraMotion>(this)->current_.eye, sizeof jo::self<jo::CameraMotion>(this)->current_.eye);
        auto place = [&]() {
            tc[3] = tc[0] - d.x * k;
            tc[4] = tc[1] - d.y * k;
            tc[5] = tc[2] - d.z * k;
        };
        place();
        float ct[6] = {tc[3], tc[4], tc[5], tc[0], tc[1], tc[2]};
        float box[6];
        joan::orig::as_aabb(box)->set_point(static_cast<void*>(&tc[0]));  // 0x0060d430
        joan::orig::as_aabb(box)->extend(static_cast<void*>(&tc[3]));  // 0x0060d490
        std::int32_t count = 0;
        void** const nodes = joan::orig::arg::as<void**>(joan::orig::as_scene(scene)->nodes_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x005ef610
        bool hit = false, mesh_hit = false;
        float plane[4] = {};
        joan::orig::as_plane_equation(static_cast<void*>(plane))->init_425700();  // 0x00425700
        std::uint8_t* actor_hit = nullptr;
        joan::orig::as_camera_man(this)->unmark_tracked();  // 0x004258a0 unmark tracked
        auto set_plane = [&](std::uint8_t* mesh, std::int32_t tri) {
            V3 v[3];
            triangle(mesh, tri, v);
            joan::orig::plane_from_points(static_cast<void*>(plane), static_cast<void*>(&v[0]), static_cast<void*>(&v[1]), static_cast<void*>(&v[2]));  // 0x0060d920
        };
        for (std::int32_t i = 0; i < count; ++i) {
            std::uint8_t* const node = static_cast<std::uint8_t*>(nodes[i]);
            if (joan::orig::arg::as<std::int32_t>(joan::orig::segment_hits_box(static_cast<void*>(tc), static_cast<void*>(jo::as_scene_s(node)->desc))) != 0 && jo::view<void*>(jo::as_scene_s(node)->plane) != nullptr) {  // 0x0060dfd0
                float t = 0.0f;
                const std::int32_t tri = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::as_scene_s(node)->plane))->segment_hit(&t, joan::orig::arg::mptr(static_cast<void*>(tc))));  // 0x005d4a80
                if (tri != 0) {
                    hit = true;
                    mesh_hit = true;
                    k = k * t;
                    place();
                    std::memcpy(ct, &tc[3], 12);
                    set_plane(jo::view<std::uint8_t*>(jo::as_scene_s(node)->plane), tri);
                }
            }
            for (std::uint8_t* a = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene_s(node)->first_actor()); a != nullptr; a = jo::view<std::uint8_t*>(jo::as_actor(a)->scene_prev)) {  // 0x005effa0
                if (jo::as_actor(a)->kind != 1) continue;
                float sphere[4];
                std::memcpy(sphere, jo::as_actor(a)->def->model->sphere, sizeof sphere);  // Model::sphere (+0x34 on x86)
                sphere[3] = sphere[3] * jo::as_actor(a)->scale;
                float off[3];
                joan::orig::as_actor(a)->position(joan::orig::arg::mptr(static_cast<void*>(off)));  // 0x005ac270 position
                sphere[0] = sphere[0] + off[0];
                sphere[1] = sphere[1] + off[1];
                sphere[2] = sphere[2] + off[2];
                const std::uint8_t* const def =
                    joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(img::obj(d::g_item_res))->get(joan::orig::arg::i16(jo::as_map_object(jo::as_actor(a)->owner)->type_id)));  // 0x00425ce0
                if (jo::as_item_def(def)->modify_camera != 0 && joan::orig::arg::as<std::int32_t>(joan::orig::segment_hits_sphere(joan::orig::arg::mptr(static_cast<void*>(ct)), joan::orig::arg::mptr(static_cast<void*>(sphere)))) != 0) {  // 0x0060e3f0
                    float t = 0.0f;
                    const std::int32_t tri = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::as_actor(a)->plane))->segment_hit(&t, joan::orig::arg::mptr(static_cast<void*>(tc))));
                    if (tri != 0) {
                        hit = true;
                        mesh_hit = false;
                        k = k * t;
                        place();
                        std::memcpy(ct, &tc[3], 12);
                        set_plane(jo::view<std::uint8_t*>(jo::as_actor(a)->plane), tri);
                        actor_hit = a;
                    }
                }
                // [Graphics] CameraFade=0 (not part of the original game): objects between the camera and the
                // player stay solid (fading is only how they are drawn).
                if (jo::as_item_def(def)->can_transparent != 0 && joan::hook::settings().camera_fade != 0) {  // ItemDef +0x1df / +0x1e0 on x86
                    bool fade = false;
                    if ((joan::orig::arg::as<std::uint8_t>(joan::orig::as_bound_sphere(static_cast<void*>(sphere))->contains(joan::orig::arg::mptr(static_cast<void*>(ct)))) & 0xff) != 0) {  // 0x0060d810
                        fade = true;
                    } else if (joan::orig::arg::as<std::int32_t>(joan::orig::segment_hits_sphere(joan::orig::arg::mptr(static_cast<void*>(ct)), joan::orig::arg::mptr(static_cast<void*>(sphere)))) != 0) {
                        float t = 0.0f;
                        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::as_actor(a)->plane))->segment_hit(&t, joan::orig::arg::mptr(static_cast<void*>(ct)))) != 0) fade = true;
                    }
                    if (fade) {
                        if (jo::view<std::uint32_t>(jo::as_actor(a)->faded) == 0) {
                            joan::orig::as_actor(a)->set_faded(static_cast<char>(joan::orig::arg::i32(1)));  // 0x005ae590 fade out
                            joan::orig::as_camera_man(this)->track(joan::orig::arg::mptr(static_cast<void*>(a)));  // 0x00425720 track
                        } else {
                            joan::orig::as_camera_man(this)->mark_tracked(joan::orig::arg::mptr(static_cast<void*>(a)));  // 0x00425840 mark
                        }
                    }
                }
            }
        }
        void* const ground = img::obj(d::g_water_array);
        for (std::int32_t i = joan::orig::as_dynarray(ground)->size(); i > 0; --i) {
            std::uint8_t* const o = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(ground)->ptr_at(joan::orig::arg::i32(i)));  // 0x00425d90
            if (joan::orig::arg::as<std::int32_t>(joan::orig::segment_hits_box(static_cast<void*>(tc), static_cast<void*>(o + 4))) == 0) continue;
            if (jo::as_scene_s(jo::as_water(o)->u32_30)->plane == nullptr) continue;  // Water::u32_30: the SceneS of the region
            float t = 0.0f;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::as_scene_s(jo::as_water(o)->u32_30)->plane)->segment_hit(&t, joan::orig::arg::mptr(static_cast<void*>(tc)))) == 0) continue;
            hit = true;
            mesh_hit = true;
            k = k * t;
            place();
            std::memcpy(ct, &tc[3], 12);
        }
        joan::orig::as_camera_man(this)->sweep_tracked();  // 0x004258f0 sweep tracked
        if (!hit) return;
        const float p_angle = mesh_hit ? jo::self<jo::CameraMotion>(this)->collide_[0] : jo::self<jo::CameraMotion>(this)->collide_[3];
        const float p_scale = mesh_hit ? jo::self<jo::CameraMotion>(this)->collide_[2] : jo::self<jo::CameraMotion>(this)->collide_[5];
        const float p_height = mesh_hit ? jo::self<jo::CameraMotion>(this)->collide_[1] : jo::self<jo::CameraMotion>(this)->collide_[4];
        auto F670 = [](float x) { return joan::orig::arg::as<double>(joan::orig::libc::cos_f(joan::orig::arg::f32(x))); };     // 0x00425670
        auto F6a0 = [](float x) { return joan::orig::arg::as<double>(joan::orig::libc::sin_f(joan::orig::arg::f32(x))); };     // 0x004256a0
        auto sqrt_f = [](float x) { return joan::orig::arg::as<float>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(x))); };    // 0x004256d0 stored
        auto sqrt_d = [](float x) { return joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(x))); };
        auto F640_f = [](float x) { return joan::orig::arg::as<float>(joan::orig::libc::atan_f(joan::orig::arg::f32(x))); };    // 0x00425640 stored
        auto F610_f = [](float x) { return joan::orig::arg::as<float>(joan::orig::libc::asin_f(joan::orig::arg::f32(x))); };    // 0x00425610 stored
        const float h = ((d.x * d.x + d.y * d.y) * k) * k;
        const float q = static_cast<float>(fpu.mul(F670(p_angle), p_scale));
        const float q2 = q * q;
        float blend = 1.0f;
        if (below_or_nan(q * q, h)) {
            const float u = static_cast<float>(fpu.mul(F670(jo::self<jo::CameraMotion>(this)->current_.target[1]), jo::view<float>(jo::self<jo::CameraMotion>(this)->current_.u32_1c)));
            const float v = u * u;
            blend = (v - h) / (v - q2);
            blend = joan::orig::arg::as<float>(joan::orig::as_misc(img::obj(d::g_misc))->limit_double(joan::orig::arg::f64(static_cast<double>(blend)), joan::orig::arg::f64(0.0), joan::orig::arg::f64(1.0)));  // 0x00590b00 clamp
        }
        float angle = 0.0f, dist = 0.0f, lift = 0.0f;
        if (mesh_hit) {
            V3 n;
            joan::orig::plane_normal(joan::orig::arg::mptr(static_cast<void*>(&n)), joan::orig::arg::mptr(static_cast<void*>(plane)));  // 0x0060d9e0 normal
            float limit = plane_limit(n, d, r);
            const float half = jo::view<float>(jo::self<jo::CameraMotion>(this)->current_.u32_1c) * 0.5f;
            limit = half > limit ? limit : half;
            angle = (p_angle - jo::self<jo::CameraMotion>(this)->current_.target[1]) * blend + jo::self<jo::CameraMotion>(this)->current_.target[1];
            const float horiz = sqrt_f(h);
            dist = static_cast<float>(fpu.sub(fpu.div(horiz, F670(angle)), limit));
            lift = p_height * blend;
        } else {
            V3 n;
            joan::orig::plane_normal(joan::orig::arg::mptr(static_cast<void*>(&n)), joan::orig::arg::mptr(static_cast<void*>(plane)));
            float limit = r;
            if (!(n.z > -0.949999988079071f)) {
                lift = p_height * blend;
                const float e = (-d.z * k) - lift;
                angle = F640_f(static_cast<float>(fpu.div(e, sqrt_d(h))));
                dist = static_cast<float>(fpu.sub(sqrt_d(e * e + h), r));
            } else if (static_cast<double>(n.z) >= -0.005) {  // 0x9cf868 (double)
                limit = plane_limit(n, d, limit);
                angle = (p_angle - jo::self<jo::CameraMotion>(this)->current_.target[1]) * blend + jo::self<jo::CameraMotion>(this)->current_.target[1];
                const float horiz = sqrt_f(h);
                dist = static_cast<float>(fpu.sub(fpu.div(horiz, F670(angle)), limit));
                lift = p_height * blend;
                tc[2] = tc[2] + lift;
                tc[5] = static_cast<float>(fpu.add(fpu.mul(F6a0(angle), dist), tc[2]));
                float t = 0.0f;
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::as_actor(actor_hit)->plane))->segment_hit(&t, joan::orig::arg::mptr(static_cast<void*>(tc)))) != 0 && below_or_nan(t, 0.9900000095367432f)) {
                    lift = p_height * blend;
                    const float e = (-d.z * k) - lift;
                    angle = F640_f(static_cast<float>(fpu.div(e, sqrt_d(h))));
                    dist = static_cast<float>(fpu.sub(sqrt_d(e * e + h), limit));
                }
            } else {
                limit = plane_limit(n, d, limit);
                lift = p_height * blend;
                tc[2] = tc[2] + lift;
                tc[5] = lift * 0.5f + tc[5];
                const V3 v{tc[0] - tc[3], tc[1] - tc[4], tc[2] - tc[5]};
                const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&v)));  // v3d returns a double (x64: xmm0)
                float t = 0.0f;
                if (joan::orig::arg::as<std::int32_t>(joan::orig::line_plane(&t, joan::orig::arg::mptr(static_cast<void*>(tc)), joan::orig::arg::mptr(static_cast<void*>(plane)))) != 0 && t > 0.0f &&  // 0x0060e590
                    below_or_nan(t, 1.0f)) {
                    dist = len * t - limit;
                    angle = F610_f(-v.z / len);
                } else {
                    angle = F610_f(-v.z / len);
                    dist = len - limit;
                }
            }
        }
        jo::self<jo::CameraMotion>(this)->current_.target[1] = angle;
        jo::view<float>(jo::self<jo::CameraMotion>(this)->current_.u32_1c) = dist;
        jo::self<jo::CameraMotion>(this)->current_.eye[2] = lift + jo::self<jo::CameraMotion>(this)->current_.eye[2];
    }
    static bool below_or_nan(float a, float b) { return !(a >= b); }
};

#define C3(name, addr, fn) JOAN_REPLACE("game/camera3", name, addr, fn)
C3("CameraMotion::collide", 0x00424010, &H::collide);
#undef C3

}  // namespace game_camzone
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
//
//    - Unit::release_4251b0 0x004251b0 (virtual), ret 0.
namespace {
namespace game_b102_01 {

using i16 = std::int16_t;

using joan::orig::helpers::addr;

void* const kGroups = obj(d::g_particle_effect_array);
void* const kTypes = obj(d::g_item_res);

void* type_record(i16 type) { return joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(type))); }  // 0x00425ce0
i32 group_free(i32 id) { return joan::orig::as_dynarray(kGroups)->empty_at(joan::orig::arg::i32(id)); }  // 0x0041bd90
void* delete_obj(void* p) { return joan::orig::arg::as<void*>(joan::orig::as_scene_object(p)->destroy(1u)); }           // 0x00420840

JOAN_COUNTER(c_release_4251b0, "Unit::release_4251b0");

struct H {
    // Frees the two group ids at +0x96 that are still in use.
    void free_groups_96() {
        for (i32 i = 0; i < 2; ++i) {
            if (group_free(jo::self<jo::MapObject>(this)->sound_slots[i]) != 0) continue;
            joan::orig::as_effect_registry(kGroups)->remove(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[i]));  // 0x0041bbf0
            jo::self<jo::MapObject>(this)->sound_slots[i] = 0;
        }
    }

    // ---------------------------------------------------------------- 0x004251b0
    // Releases the group slot objects owned by +0x86 (both +0x96 and +0x9a ids).
    void release_owned(i32 first) {  // sound_slots[first], [first + 1]
        for (i32 i = 0; i < 2; ++i) {
            if (group_free(jo::self<jo::MapObject>(this)->sound_slots[first + i]) != 0) continue;
            u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kGroups)->ptr_at(static_cast<i32>(jo::self<jo::MapObject>(this)->sound_slots[first + i])));  // 0x0041bd60
            if (jo::self<jo::MapObject>(this)->actor != nullptr && jo::as_model_obj(g)->owner == jo::self<jo::MapObject>(this)->actor) joan::orig::as_effect(g)->set_owner(joan::orig::arg::mptr(0), joan::orig::arg::mptr(0));  // 0x0041b0d0
        }
    }
    void detach_delete(jo::Actor*& slot) {
        if (slot->scene != nullptr)
            joan::orig::as_scene(slot->scene)->detach(joan::orig::arg::mptr(slot));  // 0x005efdb0
        void* const p = slot;
        if (p) delete_obj(p);
        slot = nullptr;
    }
    std::uintptr_t release_4251b0() {
        c_release_4251b0.hit();
        joan::orig::as_map_object(this)->set_world(0);  // 0x004994b0
        release_owned(0);
        release_owned(2);
        if (jo::self<jo::MapObject>(this)->actor_b != nullptr) detach_delete(jo::self<jo::MapObject>(this)->actor_b);
        joan::orig::as_map_object(this)->release_light();  // 0x0049c1a0
        joan::orig::as_map_object(this)->release_marker();  // 0x0049c920
        free_groups_96();
        if (jo::self<jo::MapObject>(this)->actor != nullptr) {
            detach_delete(jo::self<jo::MapObject>(this)->actor);
            const u32 arg = jo::self<jo::MapObject>(this)->busy;  // pushed before the type lookup
            void* const type = type_record(jo::self<jo::MapObject>(this)->type_id);
            joan::orig::as_res(type)->release_slot(joan::orig::arg::i32(arg), 0);  // 0x004a3e30
        }
        std::uintptr_t eax = addr(this);
        if (jo::self<jo::MapObject>(this)->world_item != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::MapObject>(this)->world_item);
            eax = p ? joan::orig::arg::as<std::uintptr_t>(joan::orig::as_world_item(p)->destroy(1u)) : 0u;  // 0x00416ca0
            jo::self<jo::MapObject>(this)->world_item = nullptr;
        }
        jo::self<jo::MapObject>(this)->active = 0;
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Unit::release_4251b0", 0x004251b0, &H::release_4251b0);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
//
//      0x004254a0 Obj::dtor_4254a0 (SEH)                   (thiscall, ret 0)
namespace {
namespace game_b98_01 {

struct H {
    u32 dtor_4254a0() {
        joan::orig::as_ending(this)->release();                         // 0x006ce390
        joan::orig::as_widget(&jo::self<jo::Ending>(this)->frame)->dtor_6294c0();               // 0x006294c0
        joan::orig::as_widget(&jo::self<jo::Ending>(this)->text)->dtor_6294c0();                // 0x006294c0
        joan::orig::as_widget(&jo::self<jo::Ending>(this)->title)->dtor_6294c0();     // 0x006294c0
        return 0;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Obj::dtor_4254a0", 0x004254a0, &H::dtor_4254a0);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
//
//    - Obj::cam_check_4255c0 0x004255c0 (ret 0): eax = movsx byte +0x3a unless it is 2, then
//      0x00614630 / 0x006146e0 on 0xa46600.
namespace {
namespace game_b103_01 {

using i8 = std::int8_t;

using joan::orig::helpers::at;

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // ------------------------------------------------------------------ 0x004255c0
    u32 cam_check_4255c0() {
        const i32 v = static_cast<i8>(f<u8>(0x3a));  // x64: class of 0x004255c0 unknown
        if (v != 2) return static_cast<u32>(v);
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_camera(obj(d::g_view3d))->begin_frame());  // 0x00614630
        if (r == 0) return r;
        return joan::orig::arg::as<u32>(joan::orig::as_camera(obj(d::g_view3d))->release_scene());  // 0x006146e0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::cam_check_4255c0", 0x004255c0, &H::cam_check_4255c0);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b115_03.cpp (game/b115_03)
//
//  * module game/b115_03 (all cdecl, ret 0, except the two thiscall ones, also ret 0):
//      crt_asin  0x00425610 (float)            -> CRT asin  0x007b2564
//      crt_atan  0x00425640 (float)            -> CRT atan  0x007b2184
//      crt_cos   0x00425670 (float)            -> CRT cos   0x007b24b4
//      crt_sin   0x004256a0 (float)            -> CRT sin   0x007b2404
//      Turns::sin 0x00577b10 (thiscall, this = float turns): sin(*this * 2pi), 2pi = qword 0x009d8670
//      Turns::cos 0x00577b50 (thiscall):                      cos(*this * 2pi)
//    (crt_atan2 0x00421020: boids.cpp; crt_fabs 0x00427d60 / crt_pow 0x00428810: camzone.cpp; crt_acos
//    0x00635e50: billboard.cpp)
//
// The originals are MSVC 6 /Od frames: `fld dword [ebp+8]; sub esp,8; fstp qword [esp]` (exact), call the CRT,
// `fst dword [ebp-4]` (dead store, no pop) and return the CRT's st0. The C++ passes the argument as a double and
// returns the CRT result as the double its slot call gives; the facades of these wrappers (libc::asin_f ...,
// Turns::sin_577b10 ...) return double, so callers see the same value either way (re/p1/gamefloat compares the
// originals with these functions, both calling the original CRT code). Turns::sin / cos first compute
// `fld dword [this]; fmul qword [2pi]` under PC24 (24-bit significand, x87 exponent range: pc24_mul), which
// `fstp qword` stores exactly.
namespace {
namespace game_b115_03 {

using joan::hook::global;

// x87 arithmetic at the game's precision control (PC24) with the x87 exponent range: src/fp/x87.hpp.
using joan::fp::pc24_mul;

double JOAN_CDECL crt_asin(float x) { return joan::orig::arg::as<double>(joan::orig::libc::asin(joan::orig::arg::f64(static_cast<double>(x)))); }  // 0x00425610
double JOAN_CDECL crt_atan(float x) { return joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(static_cast<double>(x)))); }  // 0x00425640
double JOAN_CDECL crt_cos(float x) { return joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(x)))); }    // 0x00425670
double JOAN_CDECL crt_sin(float x) { return joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(x)))); }    // 0x004256a0

// thiscall, this = the float turns value.
struct Turns {
    double radians() const { return pc24_mul(static_cast<double>(*reinterpret_cast<const float*>(this)), global<double>(d::dbl_6_283185307179586)); }
    double sin_577b10() { return joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(radians()))); }  // 0x00577b10
    double cos_577b50() { return joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(radians()))); }  // 0x00577b50
};

#define R(name, addr, fn) JOAN_REPLACE("game/b115_03", name, addr, fn)
R("crt_asin", 0x00425610, &crt_asin);
R("crt_atan", 0x00425640, &crt_atan);
R("crt_cos", 0x00425670, &crt_cos);
R("crt_sin", 0x004256a0, &crt_sin);
R("Turns::sin", 0x00577b10, &Turns::sin_577b10);
R("Turns::cos", 0x00577b50, &Turns::cos_577b50);
#undef R

}  // namespace game_b115_03
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
//
//  * module game/leaves80a:
//      Font::~Font 0x00623e70 (empty), String::c_str 0x00417d90 (returns this),
//      Table::last_slot 0x00417520 (+0x08), SoundEntry::id 0x004d0440 (+0x04),
//      SceneNode::attachments 0x005efff0 (+0xce), Textures::mode / set_mode 0x0060a720 /
//      0x0060a740 (+0x30), File::File 0x004179c0 (+0x105 = -1), Bytes::at 0x0041bf10 (byte
//      [this + i + 1]), SoundList::entry 0x004cf690 (this + i * 0x18 - 4), Sound::play / pause
//      0x0059f620 / 0x0059f650 (OpenAL alSourcePlay / alSourcePause), Reader::done 0x005b4140
//      (byte +1 == 0), Light::params 0x0061ddc0 (copies 0x68 bytes from +0x5e), Actor::position
//      0x005ac270 (bone matrix translation), Loader::Loader 0x00577be0 (zeroed, +0x13c = -1),
//      `vector constructor iterator' 0x00449af0 (stdcall), scalar deleting destructor 0x00459c40
//      (destructor 0x005cc750), Progress::redraw 0x0075f4f0 (Game::draw 0x004d48f0 on the
//      loading screen), Model::update_parts 0x005b77b0, Unit::attach_a / attach_b 0x00421960 /
//      0x005227f0 (adds the +0x4e / +0x4a object to the world's scene list +0x6e),
//      Texture::set_name 0x00605ad0, the float sqrt wrapper 0x004256d0 (CRT sqrt 0x007b2344
//      runs fsqrt under the caller's precision control, so its st0 is exactly a double: the
//      C++ double return is exact; the negative-argument result is the x87 indefinite NaN,
//      which also survives the double).
namespace {
namespace game_leaves80a {

// CRT wrappers (cdecl, float arguments, st0 result returned as left by the CRT).
double JOAN_CDECL crt_sqrt(float x) { return joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(x)))); }   // 0x004256d0

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("crt_sqrt", 0x004256d0, &crt_sqrt);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/en3d_helpers4.cpp (en3d/helpers4)
namespace {
namespace en3d_helpers4 {

struct H {
    // --- small accessors ------------------------------------------------------------------------
    H* trivial_ctor() { return this; }  // 0x00425700 Plane4 ctor (no initialisation)
};

#define HELPER4(name, addr, fn) JOAN_REPLACE("en3d/helpers4", name, addr, fn)
HELPER4("Plane4::Plane4", 0x00425700, &H::trivial_ctor);
#undef HELPER4

}  // namespace en3d_helpers4
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

// g_out = g_in / g_div (fld dword / fdiv dword / fstp dword).
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a3a078() { div_global(d::g_a3a078, d::g_pi_a3a06c, d::flt_4); }  // 0x00425810

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("init_a3a078", 0x00425810, &init_a3a078);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
//
//    - destructors / scalar deleting destructors 0x00416580, 0x00561160, 0x004eeeb0,
//      0x004c5320, 0x004b65f0, 0x004b05f0, 0x00436780, 0x00521340, 0x004235d0, 0x004b92c0,
//      0x00417470; zeroing ctors 0x00546af0, 0x004eff60, 0x0041b5c0; getters / setters /
//      wrappers 0x00456ca0, 0x004217e0, 0x00428b30, 0x0045e110, 0x00456c70, 0x00438b00,
//      0x00420ff0, 0x004af070, 0x004af040, 0x004eea60; empty 0x004af8a0, 0x004343c0,
//      0x00432d10, 0x00430460.
namespace {
namespace game_b85_06 {

using joan::call::thiscall;

struct H {
    std::uint32_t dtor_4235d0() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_camera_motion(this)->free_list()); }  // 0x004236d0
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj4235d0::dtor", 0x004235d0, &H::dtor_4235d0);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_camera.cpp (game/camera1)
//
//  * module game/camera1: CameraZone::update 0x00425fc0, Zones::find 0x00429bb0,
//    CameraMan::set_mode 0x00429860 / restore_mode 0x004297d0, World::cells_in_box
//    0x00429090, CameraMotion::retarget 0x00423f40 / current 0x00423fb0, the tracked-unit
//    list (0x00425720 add, 0x00425840 mark, 0x004258a0 unmark all, 0x004258f0 sweep),
//    small getters (0x00425ce0, 0x00425d90, 0x006af310, 0x006b0790), Effect::set_owner
//    0x0041b0d0, lerp 0x00427bb0, Hermite 0x004279b0.
namespace {
namespace game_camera_2 {

using joan::call::thiscall;
struct H {
    void* record_2bf(std::int16_t i) { return jo::self<jo::ItemRes>(this)->items + i - 1; }  // 0x00425ce0 (0x2bf-byte records on x86)
    void* list_get_wrap(std::int32_t i) { return joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_41b3f0(joan::orig::arg::i32(i))); }    // 0x00425d90
};

#define C1(name, addr, fn) JOAN_REPLACE("game/camera1", name, addr, fn)
C1("Records::get_2bf", 0x00425ce0, &H::record_2bf);
C1("List::get_i", 0x00425d90, &H::list_get_wrap);
#undef C1

}  // namespace game_camera_2
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

using joan::call::thiscall;

constexpr fn::Code kListGet = fn::List_get_41b3f0;     // 0x0041b3f0

void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

struct H {
    void* list4_get(std::int32_t i) { return list_get(&jo::self<jo::Units>(this)->list, i); }                     // 0x00425d20
    std::int32_t list4_empty_at(std::int32_t i) { return list_get(&jo::self<jo::Units>(this)->list, i) == nullptr ? 1 : 0; }  // 0x00425d50
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("Units::get", 0x00425d20, &H::list4_get);
FX1("Units::empty_at", 0x00425d50, &H::list4_empty_at);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world2)
//
//  * module game/world2: CameraMotion::update (0x004237a0) with its interpolators, the
//    camera-follow start (0x00563a20), map-cell streaming (0x00547630, 0x005479a0,
//    0x004457e0, 0x0043f4e0), the unit draw gate (0x0043b890).
namespace {
namespace game_world_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;
bool equal_or_nan(float a, float b) { return !(a < b || a > b); }

constexpr img::Addr kFiles(d::g_misc);
std::uint32_t clock_ms() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(img::obj(kFiles))->get_time_591050()); }  // 0x00591050 timeGetTime wrapper

JOAN_COUNTER(c_motion, "CameraMotion::update");

struct H {
    // 0x004237a0 CameraMotion::update: +0x98 progress by mode +0x78 (1 jump, 2 linear over
    // +0x7c ms, 3 ease in / out (+0x80 / +0x84 fractions), 4 callback +0x7c), angles +0x10 /
    // +0x38 wrapped to [-pi, pi], interpolation +0x88 (1 built-in, 2 callback +0x8c) from
    // the start (+0) to the end (+0x28) state into +0x50. Returns 0 when finished.
    std::int32_t motion_update() {
        // The pad's right stick holds the follow camera's angle (Ctrl::update_432d30, joan::padfx::g_camera_hold);
        // the collision below still tilts / pulls the camera in at walls, as with the mouse.
        const joan::padfx::CameraHold& hold = joan::padfx::g_camera_hold;
        if (hold.motion == this && global<std::uint32_t>(d::g_now) - hold.ms <= 100u) {
            jo::self<jo::CameraMotion>(this)->to.target[0] = hold.yaw;
            jo::self<jo::CameraMotion>(this)->to.target[1] = hold.pitch;
        }
        if (equal_or_nan(jo::self<jo::CameraMotion>(this)->t, 1.0f)) return 0;
        c_motion.hit();
        const joan::hook::x87::Fpu fpu{true};
        const std::uint32_t dt = clock_ms() - jo::self<jo::CameraMotion>(this)->start_ms;
        switch (jo::view<std::int32_t>(jo::self<jo::CameraMotion>(this)->time_mode)) {
        case 1:
            jo::self<jo::CameraMotion>(this)->t = 1.0f;
            break;
        case 2: {
            const std::uint32_t d = jo::self<jo::CameraMotion>(this)->duration_ms;
            if (dt >= d)
                jo::self<jo::CameraMotion>(this)->t = 1.0f;
            else
                jo::self<jo::CameraMotion>(this)->t = static_cast<float>(fpu.div(static_cast<double>(dt), static_cast<double>(static_cast<std::int32_t>(d))));
            break;
        }
        case 3: {
            const std::uint32_t d = jo::self<jo::CameraMotion>(this)->duration_ms;
            const float in = jo::self<jo::CameraMotion>(this)->ease_in;
            const float out = jo::self<jo::CameraMotion>(this)->ease_out;
            if (dt >= d) {
                jo::self<jo::CameraMotion>(this)->t = 1.0f;
                break;
            }
            const double di = static_cast<double>(static_cast<std::int32_t>(d));
            const float k = static_cast<float>(fpu.div(2.0f / ((2.0f - in) - out), di));
            if (fpu.mul(static_cast<double>(d), in) > static_cast<double>(dt)) {
                const std::uint32_t sq = static_cast<std::uint32_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_camera_motion(this)->square(joan::orig::arg::i32(dt))));  // 0x00425a90
                double r = fpu.mul(static_cast<double>(sq), k);
                r = fpu.mul(r, 0.5);
                r = fpu.div(r, in);
                jo::self<jo::CameraMotion>(this)->t = static_cast<float>(fpu.div(r, di));
            } else if (fpu.mul(static_cast<double>(1.0f - out), di) > static_cast<double>(dt)) {
                double z = fpu.mul(static_cast<double>(d), in);
                z = fpu.mul(z, 0.5);
                jo::self<jo::CameraMotion>(this)->t = static_cast<float>(fpu.mul(fpu.sub(static_cast<double>(dt), z), k));
            } else {
                const std::uint32_t sq = static_cast<std::uint32_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_camera_motion(this)->square(joan::orig::arg::i32(d - dt))));
                double r = fpu.mul(static_cast<double>(sq), k);
                r = fpu.div(r, out);
                r = fpu.div(r, di);
                r = fpu.mul(r, 0.5);
                jo::self<jo::CameraMotion>(this)->t = static_cast<float>(fpu.sub(1.0, r));
            }
            break;
        }
        case 4:
            jo::self<jo::CameraMotion>(this)->t = cdecl_call<float>(jo::self<jo::CameraMotion>(this)->duration_ms, dt, static_cast<void*>(&jo::self<jo::CameraMotion>(this)->duration_ms));  // mode 4 is never set (its +0x7c would hold a function pointer, which a u32 cannot on x64)
            break;
        default:
            break;
        }
        float pi;
        std::uint32_t pi_bits = 0x40490fdbu;
        std::memcpy(&pi, &pi_bits, 4);
        float* const wrapped[2] = {&jo::self<jo::CameraMotion>(this)->to.target[0], &jo::self<jo::CameraMotion>(this)->from.target[0]};
        for (float* const a : wrapped) {
            for (;;) {
                if (!(*a > pi) && !(-pi > *a)) break;
                if (*a > pi)
                    *a = *a - 2.0f * pi;
                else if (-pi > *a)
                    *a = 2.0f * pi + *a;
            }
        }
        switch (jo::view<std::int32_t>(jo::self<jo::CameraMotion>(this)->blend_mode)) {
        case 1: {
            float pos[3];
            joan::orig::as_camera_motion(this)->vec_lerp(joan::orig::arg::mptr(static_cast<void*>(pos)), joan::orig::arg::mptr(jo::self<jo::CameraMotion>(this)->from.eye), joan::orig::arg::mptr(jo::self<jo::CameraMotion>(this)->to.eye), joan::orig::arg::f32(jo::self<jo::CameraMotion>(this)->t));  // 0x00425ac0
            std::memcpy(jo::self<jo::CameraMotion>(this)->current_.eye, pos, sizeof pos);
            float lo = -pi;
            jo::self<jo::CameraMotion>(this)->current_.target[0] = joan::orig::arg::as<float>(joan::orig::as_camera_motion(this)->angle_lerp(joan::orig::arg::mptr(jo::self<jo::CameraMotion>(this)->from.target), joan::orig::arg::mptr(jo::self<jo::CameraMotion>(this)->to.target), joan::orig::arg::f32(jo::self<jo::CameraMotion>(this)->t), joan::orig::arg::mptr(static_cast<const float*>(&lo)), joan::orig::arg::mptr(static_cast<const float*>(&pi))));  // 0x00425b90
            for (std::int32_t k = 0; k < 5; ++k)
                (&jo::self<jo::CameraMotion>(this)->current_.target[1])[k] = joan::orig::arg::as<float>(joan::orig::as_camera_motion(this)->lerp(joan::orig::arg::mptr(&jo::self<jo::CameraMotion>(this)->from.target[1] + k), joan::orig::arg::mptr(&jo::self<jo::CameraMotion>(this)->to.target[1] + k), joan::orig::arg::f32(jo::self<jo::CameraMotion>(this)->t)));  // 0x00425c80 (the 5 floats after target[0])
            break;
        }
        case 2:
            cdecl_call(jo::self<jo::CameraMotion>(this)->blend_callback, static_cast<void*>(&jo::self<jo::CameraMotion>(this)->current_), static_cast<void*>(&jo::self<jo::CameraMotion>(this)->from), static_cast<void*>(&jo::self<jo::CameraMotion>(this)->to),
                       jo::self<jo::CameraMotion>(this)->t, static_cast<void*>(&jo::self<jo::CameraMotion>(this)->blend_callback));  // mode 2 is never set (its +0x8c would hold a pointer, which a u32 cannot on x64)
            break;
        default:
            break;
        }
        if ((joan::orig::arg::as<std::int32_t>(joan::orig::state_same(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::CameraMotion>(this)->current_)), joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::CameraMotion>(this)->to)))) & 0xff) != 0)  // 0x00425cb0
            jo::self<jo::CameraMotion>(this)->t = 1.0f;
        if (jo::self<jo::CameraMotion>(this)->skip_collision == 0) joan::orig::as_camera_motion(this)->collide();  // 0x00424010
        return 1;
    }
};

#define W2(name, addr, fn) JOAN_REPLACE("game/world2", name, addr, fn)
W2("CameraMotion::update", 0x004237a0, &H::motion_update);
#undef W2

}  // namespace game_world_2
}  // namespace

// ---------------------------------------- from modules/game_b87_05.cpp (game/b87_05)
namespace {
namespace game_b87_05 {

using joan::call::thiscall;
using joan::img::obj;

using i32 = std::int32_t;

std::uintptr_t u32(const void* p) { return reinterpret_cast<std::uintptr_t>(p); }

void* const kStations = obj(d::g_nations);   // 0x00484870 / 0x0047b600

std::uint32_t station_missing(std::int32_t id) { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_list(kStations)->is_null(joan::orig::arg::i32(id))); }  // 0x00484870
void* station_get(std::int32_t id) { return joan::orig::as_dynarray(kStations)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0047b600

struct H {
    // ------------------------------------------------------------ 0x00425a40
    std::uintptr_t detach() {
        if (station_missing(jo::self<jo::Place>(this)->nation) == 0)
            joan::orig::as_nation(station_get(jo::self<jo::Place>(this)->nation))->remove_member(joan::orig::arg::mptr(this));  // 0x0045c460
        return u32(joan::orig::arg::as<void*>(joan::orig::as_place(this)->set_owner(0)));  // 0x004366f0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_05", name, addr, fn)
B("Place::detach", 0x00425a40, &H::detach);
#undef B

}  // namespace game_b87_05
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world_3 {

using joan::call::cdecl_call;

std::int32_t JOAN_CDECL state_same(const void* a, const void* b) { return joan::orig::arg::as<std::int32_t>(joan::orig::libc::memcmp(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b), 0x28u)) == 0 ? 1 : 0; }

struct H {
    std::int32_t square(std::int32_t v) { return v * v; }                               // 0x00425a90 (wraps)
    float lerp(const float* a, const float* b, float t) { return (*b - *a) * t + *a; }   // 0x00425c80

    // 0x00425ac0 out = a + t * (b - a).
    void* vec_lerp(void* out, const float* a, const float* b, float t) {
        const float d[3] = {b[0] - a[0], b[1] - a[1], b[2] - a[2]};
        const float r[3] = {a[0] + t * d[0], a[1] + t * d[1], a[2] + t * d[2]};
        std::memcpy(out, r, 12);
        return out;
    }
    // 0x00425b90 angle lerp the short way round in [*lo, *hi).
    float angle_lerp(const float* a, const float* b, float t, const float* lo, const float* hi) {
        const float range = *hi - *lo;
        const float half = range / 2.0f;
        const float d = *b - *a;
        float r;
        if (d > half) {
            r = (d - range) * t + *a;
            if (!(r >= *lo)) r = r + range;
        } else if (-d > half) {
            r = (d + range) * t + *a;
            if (r >= *hi) r = r - range;
        } else {
            r = t * d + *a;
        }
        return r;
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
#define W2(name, addr, fn) JOAN_REPLACE("game/world2", name, addr, fn)
W1("square", 0x00425a90, &H::square);
W1("lerp_float", 0x00425c80, &H::lerp);
W1("state_same", 0x00425cb0, &state_same);
W2("vec_lerp", 0x00425ac0, &H::vec_lerp);
W2("angle_lerp", 0x00425b90, &H::angle_lerp);
#undef W1
#undef W2

}  // namespace game_world_3
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using i32 = std::int32_t;

struct H {
    // ------------------------------------------------------------------ 0x00425b60
    i32 always_1(void*) { return 1; }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::always_1", 0x00425b60, &H::always_1);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01_2.cpp (game/b99_01)
namespace {
namespace game_b99_01_2 {

using u32 = std::uint32_t;

struct H {
    void nop_425c50(u32 /*unused*/) {}
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::nop_425c50", 0x00425c50, &H::nop_425c50);
#undef B99

}  // namespace game_b99_01_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::CameraMotion::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b85_06::H::dtor_4235d0>(this)); }  // 0x4235d0
auto joan::orig::CameraMotion::init() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_01::H::init_423600>(this)); }  // 0x423600
auto joan::orig::CameraMotion::free_list() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b86_05::H::free_list>(this)); }  // 0x4236d0
auto joan::orig::CameraMotion::update() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_world_2::H::motion_update>(this)); }  // 0x4237a0
auto joan::orig::Shop::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_19::H::dtor_423d20>(this)); }  // 0x423d20
auto joan::orig::CameraMotion::set_default_modes() -> void { joan::bind::member<&::game_b99_01::H::set_flags_78_88_a4>(this); }  // 0x423e70
auto joan::orig::CameraMotion::start(void* a0, void* a1) -> void { joan::bind::member<&::game_world::H::motion_start>(this, a0, a1); }  // 0x423ec0
auto joan::orig::CameraMotion::current(void* a0) -> void { joan::bind::member<&::game_camera::H::motion_current>(this, a0); }  // 0x423fb0
auto joan::orig::CameraMotion::collide() -> void { joan::bind::member<&::game_camzone::H::collide>(this); }  // 0x424010
auto joan::orig::Unit::release_4251b0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b102_01::H::release_4251b0>(this)); }  // 0x4251b0
auto joan::orig::libc::sqrt_f(float a0) -> double { return joan::bind::conv<double>(joan::bind::free<&::game_leaves80a::crt_sqrt>(a0)); }  // 0x4256d0
auto joan::orig::PlaneEquation::init_425700() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers4::H::trivial_ctor>(this)); }  // 0x425700
auto joan::orig::CameraMan::track(void* a0) -> void { joan::bind::member<&::game_camera::H::tracked_add>(this, a0); }  // 0x425720
auto joan::orig::init_a3a078() -> void { joan::bind::free<&::game_b94_01::init_a3a078>(); }  // 0x425810
auto joan::orig::CameraMan::mark_tracked(void* a0) -> void { joan::bind::member<&::game_camera::H::tracked_mark>(this, a0); }  // 0x425840
auto joan::orig::CameraMan::unmark_tracked() -> void { joan::bind::member<&::game_camera::H::tracked_unmark_all>(this); }  // 0x4258a0
auto joan::orig::CameraMan::sweep_tracked() -> void { joan::bind::member<&::game_camera::H::tracked_sweep>(this); }  // 0x4258f0
auto joan::orig::Place::detach() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b87_05::H::detach>(this)); }  // 0x425a40
auto joan::orig::CameraMotion::square(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_world_3::H::square>(this, a0)); }  // 0x425a90
auto joan::orig::CameraMotion::vec_lerp(void* a0, void* a1, void* a2, float a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_world_3::H::vec_lerp>(this, a0, a1, a2, a3)); }  // 0x425ac0
auto joan::orig::Ai::cond_always_1_425b60(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_01::H::always_1>(this, a0)); }  // 0x425b60
auto joan::orig::CameraMotion::angle_lerp(void* a0, void* a1, float a2, void* a3, void* a4) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_world_3::H::angle_lerp>(this, a0, a1, a2, a3, a4)); }  // 0x425b90
auto joan::orig::CameraMotion::lerp(void* a0, void* a1, float a2) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_world_3::H::lerp>(this, a0, a1, a2)); }  // 0x425c80
auto joan::orig::state_same(void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_world_3::state_same>(a0, a1)); }  // 0x425cb0
auto joan::orig::Units::get_425d20(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::list4_get>(this, a0)); }  // 0x425d20
auto joan::orig::Units::empty_at(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_fx::H::list4_empty_at>(this, a0)); }  // 0x425d50
