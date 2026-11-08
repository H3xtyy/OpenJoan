// game/camres.cpp: functions of the original d:\ww\camres.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cstddef>
#include <cstdarg>
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::str;

using joan::call::cdecl_call;

// 12-byte vector passed by value: floats, as the callers declare it (System V passes it in XMM registers).
struct V3 {
    float x, y, z;
};

}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
//
//    - TrackRes::free 0x004eba00 / CamRes::free 0x00428d50 (if loaded: release, free +0 with
//      0x0058ed70, clear), Plasma::free 0x004c5960, TrackRes::load 0x004eba90 /
//      CamRes::load 0x00428de0 (id + 12-byte value by value, then loaded = 1).
namespace {
namespace game_b82_18 {

using joan::img::obj;

// Memory::free(ptr, file, line) on the allocator 0x00a48838.
void mem_free(void* p, const char* file, std::int32_t line) {
    joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

struct Vec12 {
    float a, b, c;
};
static_assert(sizeof(Vec12) == 12);

struct H {
    // 0x004eba00 / 0x00428d50: free a loaded resource.
    void res_free(std::uintptr_t release, img::Addr line_global, img::Addr file) {
        if (static_cast<std::int8_t>(jo::self<jo::CamRes>(this)->loaded) == 1) {
            thiscall(release, this);
            if (jo::self<jo::CamRes>(this)->tracks != nullptr) {
                mem_free(jo::self<jo::CamRes>(this)->tracks, str(file),
                         static_cast<std::int32_t>(global<std::int16_t>(line_global)) + 7);
                jo::self<jo::CamRes>(this)->tracks = nullptr;
                jo::view<std::uint32_t>(jo::self<jo::CamRes>(this)->count) = 0;
            }
            jo::self<jo::CamRes>(this)->loaded = 0;
        }
    }
    void cam_free() { res_free(fn::code_value(fn::CamRes_release_tracks), d::g_9f7c2c, d::s_d_ww_camres_cpp); }    // 0x00428e30, camres.cpp

    void cam_load(const char* id, Vec12 v) {  // 0x00428de0 (x64: the path is a pointer, not a u32)
        joan::orig::as_cam_tracks(this)->load_file(id, v);       // 0x00428e90
        jo::self<jo::CamRes>(this)->loaded = 1;
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("CamRes::free", 0x00428d50, &H::cam_free);
B82("CamRes::load", 0x00428de0, &H::cam_load);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b106_11.cpp (game/b106_11)
//
//    - CamRes::release_tracks 0x00428e30 (thiscall, void, ret 0): for (short i = 1;
//      i <= +4; ++i) 0x00429f30(0x00428b30(this, i)). Only reached through 0x00428d50, whose
//      callers ignore eax (see game_b105_01.cpp notes) - replaced as void.
namespace {
namespace game_b106_11 {

using i16 = std::int16_t;
using i32 = std::int32_t;

JOAN_COUNTER(c_cam_release, "CamRes::release_tracks");

struct H {

    // ------------------------------------------------------------------ 0x00428e30
    void cam_release_tracks() {
        c_cam_release.hit();
        for (i16 i = 1; static_cast<i32>(i) <= jo::self<jo::CamRes>(this)->count; i = static_cast<i16>(i + 1)) {
            void* const t = joan::orig::arg::as<void*>(joan::orig::as_tracks(this)->get(joan::orig::arg::i16(i)));  // 0x00428b30
            joan::orig::as_track(t)->destruct();                               // 0x00429f30
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_11", name, addr, fn)
B("CamRes::release_tracks", 0x00428e30, &H::cam_release_tracks);
#undef B

}  // namespace game_b106_11
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
//
//    - CameraTracks::load 0x00428e90 (path, V3) ret 0x10 (camres.cpp).
//
//  * SEH frames (0x0041baf0, 0x00428e90, 0x004ebb40) only guard operator new + ctor and the
//    stack text reader: normal path reproduced (0x00589890 on every exit).
namespace {
namespace game_b83_07 {

static_assert(sizeof(V3) == 12, "V3");
struct alignas(4) Path {  // 0x005a1170 string, no destructor
    std::uint8_t b[0x194];
};
struct alignas(8) Script {  // 0x00589550 / 0x00589890 text reader
    std::uint8_t b[((sizeof(jo::FileTxt) > 0x1a0 ? sizeof(jo::FileTxt) : 0x1a0) + 8 - 1) / 8 * 8];  // 0x1a0 on x86; FileTxt is 0x1a6 on x64
};

JOAN_COUNTER(c_cam_load, "CameraTracks::load");

using joan::orig::helpers::stricmp_;
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90

struct H {
    // ------------------------------------------------------------------ 0x00428e90
    void cam_load(const char* path, V3 v) {
        c_cam_load.hit();
        Path p;
        joan::orig::as_string(&p)->construct();               // 0x005a1170
        joan::orig::as_string(&p)->assign(joan::orig::arg::str(path));    // 0x005a1440
        Script r;
        joan::orig::as_file_txt(&r)->construct(joan::orig::arg::str(c_str(p)));            // 0x00589550
        std::int32_t n = 0;
        for (;;) {
            const char* const tok = joan::orig::arg::as<const char*>(joan::orig::as_file_txt(&r)->get_token(1));  // 0x00589c70
            if (tok == nullptr) break;
            if (stricmp_(tok, str(d::s_CreateCameraTrackArray)) == 0) {  // "CreateCameraTrackArray"
                joan::orig::as_cam_tracks(this)->create_array(joan::orig::arg::mptr(static_cast<void*>(&r)));  // 0x00429980
                if (jo::view<std::uint32_t>(jo::self<jo::CameraTracks>(this)->count) == 0) {
                    joan::orig::as_file_txt(&r)->destruct();  // 0x00589890
                    return;
                }
            }
            if (stricmp_(tok, str(d::s_CreateCameraTrack)) != 0 && stricmp_(tok, str(d::s_CreateCameraAnimatedTrack)) != 0) continue;
            if (jo::view<std::uint32_t>(jo::self<jo::CameraTracks>(this)->count) == 0) {
                joan::orig::as_file_txt(&r)->destruct();
                return;
            }
            ++n;
            std::uint8_t* const e = jo::view<std::uint8_t*>(jo::self<jo::CameraTracks>(this)->tracks) + n * 0xaca - 0xaca;
            jo::as_cam_track(e)->id = static_cast<std::int16_t>(n);
            jo::view<std::int32_t>(jo::as_cam_track(e)->kind) = stricmp_(tok, str(d::s_CreateCameraTrack)) == 0 ? 0 : 1;
            joan::orig::as_cam_tracks(this)->parse_track(joan::orig::arg::mptr(static_cast<void*>(&r)), joan::orig::arg::mptr(static_cast<void*>(e)), v);  // 0x00429110
        }
        joan::orig::as_file_txt(&r)->destruct();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("CameraTracks::load", 0x00428e90, &H::cam_load);
#undef B

}  // namespace game_b83_07
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

JOAN_COUNTER(c_mode, "CameraMan::set_mode");

struct H {
    // CameraMan (0xa48cc0): +4 motions, +0xc camera, +0x2f mode, +0x33 previous mode.
    void camera_start_motion() {
        void* const m = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(this)->motion(joan::orig::arg::i32(joan::orig::arg::as<std::uint32_t>(joan::orig::as_camera_follow(this)->target()))));  // 0x00563c80(0x00563dc0)
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CameraFollow>(this)->camera))->frame(joan::orig::arg::mptr(m));  // 0x006140a0 frame -> motion start
        jo::view<std::uint32_t>(jo::as_camera_motion(m)->t) = 0;
    }
    void set_mode(std::int32_t mode) {  // 0x00429860
        if (mode == jo::view<std::int32_t>(jo::self<jo::CameraFollow>(this)->follow_target)) return;
        if (mode == 3 && jo::view<std::int32_t>(jo::self<jo::CameraFollow>(this)->follow_target) == 1) return;
        if (jo::view<std::int32_t>(jo::self<jo::CameraFollow>(this)->follow_target) == 3 && mode != 0) return;
        c_mode.hit();
        void* const input = img::obj(d::g_dyn_grass);
        // [Fixes] CutsceneGrass: the original turns the grass off in every camera mode but 0 / 1
        // (cutscenes, scripted cameras); the fix keeps the player's grass option in all modes.
        if (mode == 0 || mode == 1 || joan::hook::settings().fix_cutscene_grass)
            joan::orig::as_grass(input)->set_enabled(static_cast<char>(joan::orig::arg::i32(static_cast<char>(global<std::uint8_t>(d::g_cfg_grass_enable)))));  // 0x00464600
        else
            joan::orig::as_grass(input)->set_enabled(static_cast<char>(joan::orig::arg::i32(static_cast<char>(0))));
        if (jo::view<void*>(jo::self<jo::CameraFollow>(this)->motions) != nullptr) {
            joan::orig::as_camera_man(joan::orig::arg::as<void*>(joan::orig::as_camera_follow(this)->motion(joan::orig::arg::i32(jo::self<jo::CameraFollow>(this)->follow_target))))->unmark_tracked();  // 0x004258a0
            joan::orig::as_camera_man(joan::orig::arg::as<void*>(joan::orig::as_camera_follow(this)->motion(joan::orig::arg::i32(jo::self<jo::CameraFollow>(this)->follow_target))))->sweep_tracked();  // 0x004258f0
        }
        jo::view<std::int32_t>(jo::self<jo::CameraFollow>(this)->mode) = jo::view<std::int32_t>(jo::self<jo::CameraFollow>(this)->follow_target);
        jo::view<std::int32_t>(jo::self<jo::CameraFollow>(this)->follow_target) = mode;
        if (jo::view<void*>(jo::self<jo::CameraFollow>(this)->motions) != nullptr) camera_start_motion();
    }
    void restore_mode() {  // 0x004297d0
        jo::view<std::int32_t>(jo::self<jo::CameraFollow>(this)->follow_target) = jo::view<std::int32_t>(jo::self<jo::CameraFollow>(this)->mode);
        void* const input = img::obj(d::g_dyn_grass);
        if (jo::view<std::int32_t>(jo::self<jo::CameraFollow>(this)->follow_target) == 0 || jo::view<std::int32_t>(jo::self<jo::CameraFollow>(this)->follow_target) == 1 || joan::hook::settings().fix_cutscene_grass)
            joan::orig::as_grass(input)->set_enabled(static_cast<char>(joan::orig::arg::i32(static_cast<char>(global<std::uint8_t>(d::g_cfg_grass_enable)))));
        else
            joan::orig::as_grass(input)->set_enabled(static_cast<char>(joan::orig::arg::i32(static_cast<char>(0))));
        if (jo::view<void*>(jo::self<jo::CameraFollow>(this)->motions) != nullptr) camera_start_motion();
    }
    // 0x00429090 World::cells_in_box(box, count_out): the cell pointer buffer (+0x82).
    void* cells_in_box(const void* box, std::int32_t* count) {
        joan::orig::as_world(this)->build_tree();  // 0x0044a260
        *count = 0;
        if (jo::view<std::uint32_t>(jo::self<jo::World>(this)->cell_count) == 0 || jo::view<void*>(jo::self<jo::World>(this)->box_tree) == nullptr) return nullptr;
        joan::orig::as_box_tree(jo::view<void*>(jo::self<jo::World>(this)->box_tree))->collect_54ad90(joan::orig::arg::mptr(box), joan::orig::arg::mptr(count), joan::orig::arg::mptr(jo::self<jo::World>(this)->cell_query));  // 0x0054ad90
        return jo::self<jo::World>(this)->cell_query;
    }
};

#define C1(name, addr, fn) JOAN_REPLACE("game/camera1", name, addr, fn)
C1("CameraMan::set_mode", 0x00429860, &H::set_mode);
C1("CameraMan::restore_mode", 0x004297d0, &H::restore_mode);
C1("World::cells_in_box", 0x00429090, &H::cells_in_box);
#undef C1

}  // namespace game_camera
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
//
//    - CameraTracks::parse_track 0x00429110 (reader, entry, V3 offset) ret 0x14 (camres.cpp).
//
//  * 0x00429110 / 0x004ebd50: the original line buffer ([ebp-0x2c]) is 0x24 bytes but read_line
//    (0x00589f10) is called with max 0x40 (writes up to 0x41 bytes); a 0x24..0x2b char line
//    only clobbers dead locals (token, this), a longer one overwrites the saved ebp / return
//    address (original crashes). Here the buffer is 0x41 bytes (+ a leading pad byte for the
//    buf[strlen - 1] = 0 write of an empty line, which in the original hits the dead high byte
//    of the loop counter [ebp-0x30]).
//
//  * x87: atof (0x007b2110, double in st0) + fadd float -> x87::Fpu; final 0x00429110
//    computation fild int / fsub / fdivr / fdiv / fdivr -> x87::Fpu (constants 1.0f, 0.05f).
namespace {
namespace game_b84_03 {

using joan::hook::x87::Fpu;

using joan::call::import_call;

static_assert(sizeof(V3) == 12, "V3");
// Line buffer for read_line (0x00589f10, max 0x40 + NUL) with a pad byte in front (buf[-1]).
struct alignas(4) Line {
    char raw[4 + 0x44];
    char* buf() { return raw + 4; }
};

JOAN_COUNTER(c_cam_parse, "CameraTracks::parse_track");

std::int32_t atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
double atof_(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }
std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }
char* strcpy_(void* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

// Text script reader (0x00589550 object).
struct Reader {
    void* r;
    const char* tok() { return joan::orig::arg::as<const char*>(joan::orig::as_file_txt(r)->get_token(1)); }  // 0x00589c70
    void line(char* buf) { joan::orig::as_file_txt(r)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), 0x40); }          // 0x00589f10
    // tok(); atoi(tok())
    std::int32_t skip_int() {
        tok();
        return atoi_(tok());
    }
    double skip_num() {
        tok();
        return atof_(tok());
    }
};

// atof() + float at PC24, stored as float.
float add_f(double a, float b) { return static_cast<float>(Fpu{true}.add(a, static_cast<double>(b))); }

struct H {
    // ------------------------------------------------------------------ 0x00429110
    void* cam_parse(void* reader, std::uint8_t* e, V3 v) {
        c_cam_parse.hit();
        Reader rd{reader};
        const float vx = v.x, vy = v.y;
        Line ln;
        char* const buf = ln.buf();
        rd.tok();
        rd.tok();
        rd.line(buf);
        buf[static_cast<std::int32_t>(strlen_(buf)) - 1] = 0;
        strcpy_(e + 0x27, buf);
        if (jo::view<std::int32_t>(jo::as_cam_track(e)->kind) == 1) {
            rd.tok();
            rd.line(buf);
            buf[static_cast<std::int32_t>(strlen_(buf)) - 1] = 0;
            strcpy_(jo::as_cam_track(e)->name, buf);
        } else {
            jo::as_cam_track(e)->box[0] = add_f(rd.skip_num(), vx);
            jo::as_cam_track(e)->box[1] = add_f(rd.skip_num(), vy);
            jo::as_cam_track(e)->box[2] = static_cast<float>(rd.skip_num());
            jo::as_cam_track(e)->box[3] = add_f(rd.skip_num(), vx);
            jo::as_cam_track(e)->box[4] = add_f(rd.skip_num(), vy);
            jo::as_cam_track(e)->box[5] = static_cast<float>(rd.skip_num());
            jo::as_cam_track(e)->view_distance = static_cast<std::int16_t>(rd.skip_int());
            jo::as_cam_track(e)->yaw_deg = static_cast<float>(rd.skip_num());
            jo::as_cam_track(e)->pitch_deg = static_cast<float>(rd.skip_num());
            jo::as_cam_track(e)->per_key_angles = static_cast<std::int16_t>(rd.skip_int());
            jo::as_cam_track(e)->scale_pitch = static_cast<std::int16_t>(rd.skip_int());
            jo::as_cam_track(e)->face_unit = static_cast<std::int16_t>(rd.skip_int());
            jo::as_cam_track(e)->free_pitch = static_cast<std::int16_t>(rd.skip_int());
        }
        jo::as_cam_track(e)->key_count = rd.skip_int();
        for (std::int32_t i = 0; i < jo::as_cam_track(e)->key_count; ++i) {
            jo::CamKey* const p = &jo::as_cam_track(e)->keys[i];
            if (jo::view<std::int32_t>(jo::as_cam_track(e)->kind) == 1) p->time = rd.skip_int();
            p->point[0] = add_f(rd.skip_num(), vx);
            p->point[1] = add_f(rd.skip_num(), vy);
            p->point[2] = static_cast<float>(rd.skip_num());
            p->yaw = static_cast<float>(rd.skip_num());
            p->pitch = static_cast<float>(rd.skip_num());
            if (jo::view<std::int32_t>(jo::as_cam_track(e)->kind) == 1) {
                p->roll = static_cast<float>(rd.skip_num());
                p->zoom = static_cast<float>(rd.skip_num());
            }
        }
        rd.tok();
        for (std::int32_t i = 0; i < jo::as_cam_track(e)->key_count; ++i) {
            std::int16_t prev = static_cast<std::int16_t>(i - 1);
            std::int16_t next = static_cast<std::int16_t>(i + 1);
            if (static_cast<std::int32_t>(prev) < 0) prev = static_cast<std::int16_t>(i);
            if (!(static_cast<std::int32_t>(next) < jo::as_cam_track(e)->key_count)) next = static_cast<std::int16_t>(i);
            const V3 vn = *reinterpret_cast<const V3*>(jo::as_cam_track(e)->keys[next].point);
            const V3 vc = *reinterpret_cast<const V3*>(jo::as_cam_track(e)->keys[i].point);
            const V3 vp = *reinterpret_cast<const V3*>(jo::as_cam_track(e)->keys[prev].point);
            joan::orig::as_track(e)->spline_point(joan::orig::arg::mptr(static_cast<void*>(jo::as_cam_track(e)->keys[i].tangent)), vp, vc, vn, joan::orig::arg::f32(0));  // 0x00429a60
        }
        float len = 0.0f;
        for (std::int32_t i = 0;
             i < static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::as_cam_track(e)->key_count) - 1u); ++i) {
            V3 tmp;
            const V3* const d = joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, static_cast<void*>(jo::as_cam_track(e)->keys[i + 1].point), static_cast<void*>(jo::as_cam_track(e)->keys[i].point)));  // 0x0060cc90 a - b
            V3 diff = *d;
            len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &diff));  // v3d length
        }
        const Fpu fpu{true};
        const double one = global<float>(d::flt_1);
        double t = fpu.sub(static_cast<double>(jo::as_cam_track(e)->key_count), one);  // fild; fsub
        t = fpu.div(static_cast<double>(len), t);                                 // fdivr len
        t = fpu.div(t, static_cast<double>(global<float>(d::flt_0_05)));          // fdiv 0.05
        t = fpu.div(one, t);                                                      // fdivr 1.0
        jo::as_cam_track(e)->rail_step = static_cast<float>(t);
        return e;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("CameraTracks::parse_track", 0x00429110, &H::cam_parse);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
//
//      0x00428d20 Obj::forward_428d50    (thiscall, ret 0)
namespace {
namespace game_b94_01 {

using joan::call::thiscall;

struct H {
    void forward_428d50() { joan::orig::as_cam_tracks(this)->free(); }  // 0x00428d20 -> 0x00428d50
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::forward_428d50", 0x00428d20, &H::forward_428d50);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
//
//    - Tracks::create_array 0x004ec240 (reader) ret 4 / CameraTracks::create_array 0x00429980
//      (reader) ret 4.
namespace {
namespace game_b84_03_2 {

using joan::hook::global;

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_create_array, "Tracks::create_array");

constexpr img::Addr kMemory = d::g_mem;

std::int32_t atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
double atof_(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }
void* memset_(void* d, std::int32_t c, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(c), joan::orig::arg::u32(n)); }
using joan::orig::helpers::line_base;
void* mem_realloc(void* p, std::int32_t n, img::Addr file, std::int32_t line) {
    return joan::orig::as_mem(obj(kMemory))->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(n), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058eb30
}

// Text script reader (0x00589550 object).
struct Reader {
    void* r;
    const char* tok() { return joan::orig::arg::as<const char*>(joan::orig::as_file_txt(r)->get_token(1)); }  // 0x00589c70
    void line(char* buf) { joan::orig::as_file_txt(r)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), 0x40); }          // 0x00589f10
    // tok(); atoi(tok())
    std::int32_t skip_int() {
        tok();
        return atoi_(tok());
    }
    double skip_num() {
        tok();
        return atof_(tok());
    }
};

struct H {
    // ------------------------------------------------------------------ 0x004ec240 / 0x00429980
    const void* create_array(void* reader, std::int32_t stride, img::Addr file, img::Addr line_global) {
        c_create_array.hit();
        Reader rd{reader};
        rd.tok();
        rd.tok();
        jo::self<jo::CameraTracks>(this)->count = atoi_(rd.tok());
        const char* const t = rd.tok();
        if (jo::view<std::uint32_t>(jo::self<jo::CameraTracks>(this)->count) == 0) return t;
        jo::view<void*>(jo::self<jo::CameraTracks>(this)->tracks) = mem_realloc(jo::view<void*>(jo::self<jo::CameraTracks>(this)->tracks), jo::self<jo::CameraTracks>(this)->count * stride, file, line_base(line_global) + 0x13);
        return memset_(jo::view<void*>(jo::self<jo::CameraTracks>(this)->tracks), 0, static_cast<std::uint32_t>(jo::self<jo::CameraTracks>(this)->count * stride));
    }
    const void* cam_array(void* reader) { return create_array(reader, 0xaca, d::s_d_ww_camres_cpp, d::g_9f7c30); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("CameraTracks::create_array", 0x00429980, &H::cam_array);
#undef B

}  // namespace game_b84_03_2
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::hook::global;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12);

JOAN_COUNTER(c_spline, "Spline::point");

struct H {
    // ---- 0x00429a60 spline point (hidden out) ----------------------------------------------
    std::uint32_t spline_point(Vec3* out, Vec3 a, Vec3 b, Vec3 c, float t) {
        c_spline.hit();
        Vec3 tmp1, tmp2, tmp3, tmp4, tmp5;
        const Vec3 d1 = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp1, &c, &b));  // 0x0060cc90 ebp-0x28
        const Vec3 s1 = *joan::orig::arg::as<Vec3*>(joan::orig::vec_scaled(&tmp2, (global<float>(d::flt_1) - t) / global<float>(d::flt_2), &d1));  // 0x0060cd60 ebp-0x1c
        const Vec3 d2 = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp3, &b, &a));  // ebp-0x58
        const Vec3 s2 = *joan::orig::arg::as<Vec3*>(joan::orig::vec_scaled(&tmp4, (global<float>(d::flt_1) + t) / global<float>(d::flt_2), &d2));  // ebp-0x4c
        const Vec3 r = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&tmp5, &s2, &s1));  // 0x0060cc20 ebp-0x10
        *out = r;
        std::uint32_t y;
        std::memcpy(&y, &r.y, 4);
        return y;
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Spline::point", 0x00429a60, &H::spline_point);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

struct H {
    // 0x00429b70
    std::int32_t yes_429b70(void*) { return 1; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("Obj::yes_429b70", 0x00429b70, &H::yes_429b70);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_camera_2.cpp (game/camera1)
namespace {
namespace game_camera_2 {

using joan::call::cdecl_call;

struct V3 {
    float x, y, z;
};

JOAN_COUNTER(c_zone_find, "Zones::find");

struct H {
    // 0x00429bb0 Zones::find(name, pos): 1-based index of the first sample zone with that
    // name containing pos, 0 if none.
    std::int32_t zones_find(const char* name, V3 pos) {
        c_zone_find.hit();
        for (std::int32_t i = 0; i < jo::self<jo::CamTracks>(this)->count; ++i) {
            std::uint8_t* const z = jo::view<std::uint8_t*>(jo::self<jo::CamTracks>(this)->tracks) + i * 0xaca;
            if (joan::crt::stricmp(joan::orig::arg::str(static_cast<const void*>(z + 0x27)), joan::orig::arg::str(name)) != 0) continue;  // _stricmp
            if (jo::view<std::int32_t>(jo::as_cam_track(z)->kind) != 0) continue;
            if ((joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(z + 0x48)->contains(static_cast<void*>(&pos))) & 0xff) != 0) return i + 1;
        }
        return 0;
    }
};

#define C1(name, addr, fn) JOAN_REPLACE("game/camera1", name, addr, fn)
C1("Zones::find", 0x00429bb0, &H::zones_find);
#undef C1

}  // namespace game_camera_2
}  // namespace

// ---------------------------------------- from modules/game_b86_05.cpp (game/b86_05)
namespace {
namespace game_b86_05 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::import_call;

using joan::hook::x87::Fpu;

using joan::orig::helpers::stricmp_;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12);

JOAN_COUNTER(c_prepare, "CamTracks::prepare");
JOAN_COUNTER(c_find, "CamTracks::find");

struct H {
    // ---- 0x00429d00 CamTracks::prepare(id) ---------------------------------------------------
    std::int32_t prepare(std::int32_t id) {
        c_prepare.hit();
        const std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(jo::self<jo::CamTracks>(this)->tracks) + static_cast<std::uint32_t>(id - 1) * 0xacau;
        if (addr == 0) return 0;
        std::uint8_t* const rec = reinterpret_cast<std::uint8_t*>(addr);
        for (std::int32_t i = 0; i < jo::as_cam_track(rec)->key_count; ++i) {
            std::int16_t prev = static_cast<std::int16_t>(i - 1);
            std::int16_t next = static_cast<std::int16_t>(i + 1);
            if (prev < 0) prev = static_cast<std::int16_t>(i);
            if (next >= jo::as_cam_track(rec)->key_count) next = static_cast<std::int16_t>(i);
            const Vec3 c = *reinterpret_cast<const Vec3*>(jo::as_cam_track(rec)->keys[next].point);
            const Vec3 b = *reinterpret_cast<const Vec3*>(jo::as_cam_track(rec)->keys[i].point);
            const Vec3 a = *reinterpret_cast<const Vec3*>(jo::as_cam_track(rec)->keys[prev].point);
            joan::orig::as_track(rec)->spline_point(joan::orig::arg::mptr(reinterpret_cast<Vec3*>(jo::as_cam_track(rec)->keys[i].tangent)), a, b, c, 0.0f);  // 0x00429a60
        }
        float len = 0.0f;
        for (std::int32_t i = 0; i < jo::as_cam_track(rec)->key_count - 1; ++i) {
            Vec3 tmp;
            const Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(reinterpret_cast<Vec3*>(jo::as_cam_track(rec)->keys[i + 1].point)), joan::orig::arg::ptr(reinterpret_cast<Vec3*>(jo::as_cam_track(rec)->keys[i].point))));  // 0x0060cc90
            len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&d)));  // v3d returns a double (x64: xmm0)
        }
        const joan::hook::x87::Fpu fpu{true};
        double v = fpu.sub(static_cast<double>(jo::as_cam_track(rec)->key_count), static_cast<double>(global<float>(d::flt_1)));
        v = fpu.div(static_cast<double>(len), v);
        v = fpu.div(v, static_cast<double>(global<float>(d::flt_0_05)));
        v = fpu.div(static_cast<double>(global<float>(d::flt_1)), v);
        jo::as_cam_track(rec)->rail_step = static_cast<float>(v);
        return 1;
    }

    // ---- 0x00429c70 CamTracks::find(name) ----------------------------------------------------
    std::int32_t find(const char* name) {
        c_find.hit();
        for (std::int32_t i = 0; i < jo::self<jo::CamTracks>(this)->count; ++i) {
            if (stricmp_(reinterpret_cast<const char*>(jo::view<std::uint8_t*>(jo::self<jo::CamTracks>(this)->tracks) + i * 0xaca + 6), name) == 0) {
                return jo::self<jo::CamTracks>(this)->tracks[i].id;
            }
        }
        return 0;
    }
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_05", name, addr, fn)
B86("CamTracks::prepare", 0x00429d00, &H::prepare);
B86("CamTracks::find", 0x00429c70, &H::find);
#undef B86

}  // namespace game_b86_05
}  // namespace

// ---------------------------------------- from modules/game_b82_18_2.cpp (game/b82_18)
namespace {
namespace game_b82_18_2 {

struct H {
    void* ctor_memset9() {  // 0x004eb9a0 / 0x00428cf0
        std::memset(static_cast<void*>(this), 0, sizeof(jo::CamRes));        // memset (9 bytes on x86)
        return this;
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("CamRes::CamRes", 0x00428cf0, &H::ctor_memset9);
#undef B82

}  // namespace game_b82_18_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::CamTracks::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_18_2::H::ctor_memset9>(this)); }  // 0x428cf0
auto joan::orig::CamTracks::free() -> void { joan::bind::member<&::game_b82_18::H::cam_free>(this); }  // 0x428d50
auto joan::orig::CameraMan::restore_mode() -> void { joan::bind::member<&::game_camera::H::restore_mode>(this); }  // 0x4297d0
auto joan::orig::CameraMan::set_mode(std::int32_t a0) -> void { joan::bind::member<&::game_camera::H::set_mode>(this, a0); }  // 0x429860
auto joan::orig::CamTracks::create_array(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_03_2::H::cam_array>(this, a0)); }  // 0x429980
auto joan::orig::Ai::cond_always_1_429b70(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_01::H::yes_429b70>(this, a0)); }  // 0x429b70
auto joan::orig::CamTracks::find(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b86_05::H::find>(this, a0)); }  // 0x429c70
auto joan::orig::CamTracks::prepare(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b86_05::H::prepare>(this, a0)); }  // 0x429d00
