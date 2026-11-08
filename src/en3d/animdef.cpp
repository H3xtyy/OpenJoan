// en3d/animdef.cpp: functions of the original d:\ww\en3d\animdef.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include "../call.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../runtime/hitch.hpp"

#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../original/dynarray.hpp"
#include "../render/ambient_occlusion.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../build_flavor.hpp"
#include "anim_cache.hpp"
#include "../save/io.hpp"

// The .amt cache (AnimDef::write_cache 0x005b7810 / read_cache 0x005b7930) moves the key arrays with one
// file_write_array / file_read_array each: the keys must be pointer-free and 0x20 bytes on every target
// (re/save/records.json s6 AnimKey; Track::keys is a KeyFrame*, the same layout).
static_assert(sizeof(joan::orig::KeyFrame) == 0x20 && offsetof(joan::orig::KeyFrame, position) == 0x14);
static_assert(sizeof(joan::orig::AnimKey) == 0x20 && offsetof(joan::orig::AnimKey, pos) == 0x14);

namespace jo = joan::orig;

// ---------------------------------------- native animation maths (not part of the original game)
//
// Skeletal animation (AnimDef::animate) is evaluated only for actors being drawn - its callers are the
// scene's draw passes (Actor::select_mesh, ActorDef::queue_instance, the shadow pass's
// Anim::select_frame), with the camera - so its results only decide what is drawn. While it runs,
// the key blend (D3DXQuaternionSlerp) and key -> bone matrix (D3DXMatrixRotationQuaternion +
// matrix_multiply_translation) use plain float maths instead of the bit-exact x87 emulation the
// reimplementation uses everywhere else (software fsin / fpatan per bone per frame: about a seventh
// of the frame in big battles). The poses differ from the original by float rounding only.
namespace {
namespace anim_math {

bool g_animating = false;  // inside AnimDef::animate (the game's thread only)

struct Scope {
    bool previous = g_animating;
    Scope() { g_animating = true; }
    ~Scope() { g_animating = previous; }
};

// D3DXQuaternionSlerp: the shorter way round; plain linear blend when the quaternions are within 1e-5.
void slerp(float* out, const float* a, const float* b, float t) {
    float c = a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
    const float sign = c < 0.0f ? -1.0f : 1.0f;
    c *= sign;
    float k0 = 1.0f - t, k1 = t;
    if (1.0f - c > 1e-5f) {
        const float theta = std::acos(c);
        const float inv = 1.0f / std::sin(theta);
        k0 = std::sin(theta * (1.0f - t)) * inv;
        k1 = std::sin(theta * t) * inv;
    }
    k1 *= sign;
    for (int i = 0; i < 4; ++i) out[i] = k0 * a[i] + k1 * b[i];
}

// D3DXMatrixRotationQuaternion followed by matrix_multiply_translation (row-major, translation in row 3).
void key_matrix(float* m, const float* q, const float* pos) {
    const float x = q[0], y = q[1], z = q[2], w = q[3];
    const float x2 = x + x, y2 = y + y, z2 = z + z;
    const float xx = x2 * x, yy = y2 * y, zz = z2 * z, xy = y2 * x, xz = z2 * x, yz = z2 * y;
    const float xw = x2 * w, yw = y2 * w, zw = z2 * w;
    m[0] = 1.0f - yy - zz, m[1] = xy + zw, m[2] = xz - yw, m[3] = 0.0f;
    m[4] = xy - zw, m[5] = 1.0f - xx - zz, m[6] = yz + xw, m[7] = 0.0f;
    m[8] = xz + yw, m[9] = yz - xw, m[10] = 1.0f - xx - yy, m[11] = 0.0f;
    m[12] = pos[0], m[13] = pos[1], m[14] = pos[2], m[15] = 1.0f;
}

}  // namespace anim_math
}  // namespace

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using joan::orig::helpers::game_delete;                      // operator delete

using joan::call::stdcall_call;

using u8 = std::uint8_t;

std::int32_t read_int(void* reader) { return joan::orig::as_file_mem(reader)->file_get_long(); }  // 0x005893c0
void read_bytes(void* reader, void* out, std::int32_t size) {
    joan::orig::as_file_mem(reader)->file_read(joan::orig::arg::mptr(out), joan::orig::arg::u32(size));  // 0x00588a80
}

// v3d_87.dll cdecl import through its IAT slot.
using joan::call::import_call;

constexpr img::Addr kAnimdefFile(d::s_d_ww_en3d_animdef_cpp);  // "d:\ww\en3d\animdef.cpp"

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

void* const kHeap = obj(d::g_mem);

// The element size of the Track / ColourTrack / AlphaTrack arrays (MSVC array cookie + eh vector iterators).
constexpr std::uint32_t kTrackSize = sizeof(jo::Track);  // 8 on x86
static_assert(sizeof(jo::ColourTrack) == kTrackSize && sizeof(jo::AlphaTrack) == kTrackSize);

}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
//
//      0x005b4170 AnimDef::release(byte keep) (ret 4) - waits for the loader (0x005b4140 /
//                 SleepEx(5, 1) / 0x005b3100); keep: +0x148 = 0, deletes +0x144 (0x005b46f0),
//                 0x005b4740(0) per +0x140 entry (+0x128 count), deletes +0x153 / +0x157 / +0x15b,
//                 +0x14f = 0, byte +0 = 1; else also deletes +0x140 (0x005b4920(3)), frees +0x13c /
//                 +0x138 (animdef.cpp, line base 0xa16e78 + 0x58 / + 0x5f), clears +0x128 /
//                 +0x12c, releases the +0x14c sub entries (0x005b7650 / 0x00417250(1)), byte +0 = 0
namespace {
namespace en3d_b85_04 {

// KERNEL32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;

constexpr img::Addr kSleepEx = d::iat_KERNEL32_SleepEx;

void* const kMemory = obj(d::g_mem);  // allocator

const char* const kAnimdefCpp = str(d::s_d_ww_en3d_animdef_cpp);   // "d:\ww\en3d\animdef.cpp"

JOAN_COUNTER(c_anim_release, "AnimDef::release");

struct H {
    // 0x005b4170
    void anim_release(std::uint32_t keep_arg) {
        c_anim_release.hit();
        if (jo::self<jo::AnimDef>(this)->loaded == 0) return;
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_reader(this)->done()) == 0) {  // 0x005b4140
            win_call<std::uint32_t>(kSleepEx, 5u, 1);
            joan::orig::as_anim_def(this)->stream_update();  // 0x005b3100
        }
        if (static_cast<std::int8_t>(keep_arg) != 0) {
            jo::self<jo::AnimDef>(this)->bones_resolved = 0;
            if (jo::view<void*>(jo::self<jo::AnimDef>(this)->bones) != nullptr) {
                void* const p = jo::view<void*>(jo::self<jo::AnimDef>(this)->bones);
                if (p != nullptr) joan::orig::as_bone_tree(p)->destroy(1u);  // 0x005b46f0
                jo::view<void*>(jo::self<jo::AnimDef>(this)->bones) = nullptr;
            }
            for (std::int32_t i = 0; i < jo::self<jo::AnimDef>(this)->track_count; ++i) {
                void* const e = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->tracks[i]);
                joan::orig::as_anim_def(e)->scalar_delete_5b4740(0u);  // 0x005b4740
            }
            if (jo::view<void*>(jo::self<jo::AnimDef>(this)->colour_tracks) != nullptr) {
                void* const p = jo::view<void*>(jo::self<jo::AnimDef>(this)->colour_tracks);
                if (p != nullptr) joan::orig::as_anim_def(p)->vector_delete_5b4790(3u);  // 0x005b4790
                jo::view<void*>(jo::self<jo::AnimDef>(this)->colour_tracks) = nullptr;
            }
            if (jo::view<void*>(jo::self<jo::AnimDef>(this)->alpha_tracks) != nullptr) {
                void* const p = jo::view<void*>(jo::self<jo::AnimDef>(this)->alpha_tracks);
                if (p != nullptr) joan::orig::as_anim_def(p)->vector_delete_5b4830(3u);  // 0x005b4830
                jo::view<void*>(jo::self<jo::AnimDef>(this)->alpha_tracks) = nullptr;
            }
            if (jo::view<void*>(jo::self<jo::AnimDef>(this)->events) != nullptr) {
                void* const p = jo::view<void*>(jo::self<jo::AnimDef>(this)->events);
                if (p != nullptr) joan::orig::as_anim_def(p)->scalar_delete_5b48d0(1u);  // 0x005b48d0
                jo::view<void*>(jo::self<jo::AnimDef>(this)->events) = nullptr;
            }
            jo::view<std::uint32_t>(jo::self<jo::AnimDef>(this)->extra_count) = 0;
            jo::view<std::uint8_t>(jo::self<jo::AnimDef>(this)->loaded) = 1;
            return;
        }
        jo::self<jo::AnimDef>(this)->bones_resolved = 0;
        if (jo::view<void*>(jo::self<jo::AnimDef>(this)->bones) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::AnimDef>(this)->bones);
            if (p != nullptr) joan::orig::as_bone_tree(p)->destroy(1u);  // 0x005b46f0
            jo::view<void*>(jo::self<jo::AnimDef>(this)->bones) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::AnimDef>(this)->tracks) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::AnimDef>(this)->tracks);
            if (p != nullptr) joan::orig::as_anim_def(p)->vector_delete_5b4920(3u);  // 0x005b4920
            jo::view<void*>(jo::self<jo::AnimDef>(this)->tracks) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::AnimDef>(this)->colour_tracks) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::AnimDef>(this)->colour_tracks);
            if (p != nullptr) joan::orig::as_anim_def(p)->vector_delete_5b4790(3u);  // 0x005b4790
            jo::view<void*>(jo::self<jo::AnimDef>(this)->colour_tracks) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::AnimDef>(this)->alpha_tracks) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::AnimDef>(this)->alpha_tracks);
            if (p != nullptr) joan::orig::as_anim_def(p)->vector_delete_5b4830(3u);  // 0x005b4830
            jo::view<void*>(jo::self<jo::AnimDef>(this)->alpha_tracks) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::AnimDef>(this)->events) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::AnimDef>(this)->events);
            if (p != nullptr) joan::orig::as_anim_def(p)->scalar_delete_5b48d0(1u);  // 0x005b48d0
            jo::view<void*>(jo::self<jo::AnimDef>(this)->events) = nullptr;
        }
        jo::view<std::uint32_t>(jo::self<jo::AnimDef>(this)->extra_count) = 0;
        if (jo::view<void*>(jo::self<jo::AnimDef>(this)->sort_index) != nullptr) {
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::AnimDef>(this)->sort_index)), joan::orig::arg::str(kAnimdefCpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a16e78)) + 0x58));  // 0x0058ed70
            jo::view<void*>(jo::self<jo::AnimDef>(this)->sort_index) = nullptr;
        }
        jo::view<std::uint32_t>(jo::self<jo::AnimDef>(this)->track_count) = 0;
        if (jo::view<void*>(jo::self<jo::AnimDef>(this)->actions) != nullptr) {
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::AnimDef>(this)->actions)), joan::orig::arg::str(kAnimdefCpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a16e78)) + 0x5f));  // 0x0058ed70
            jo::view<void*>(jo::self<jo::AnimDef>(this)->actions) = nullptr;
        }
        jo::view<std::uint32_t>(jo::self<jo::AnimDef>(this)->action_count) = 0;
        for (std::int16_t i = jo::self<jo::AnimDef>(this)->child_count; i > 0; i = static_cast<std::int16_t>(i - 1)) {
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_anim_def(this)->entry(static_cast<std::int32_t>(i)));  // 0x005b7650 (short)
            if (p != nullptr) joan::orig::as_anim(p)->scalar_delete(1u);                             // 0x00417250
        }
        jo::self<jo::AnimDef>(this)->child_count = 0;
        jo::view<std::uint8_t>(jo::self<jo::AnimDef>(this)->loaded) = 0;
    }

    // 0x005b46f0
    H* delete_5b46f0(std::uint32_t flags) {
        joan::orig::as_bone_tree(this)->dtor_wrapper();  // 0x005b9960
        if ((flags & 1u) != 0) game_delete(this);
        return this;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("AnimationDef::cancel_async_init", 0x005b4170, &H::anim_release);
E3D("AnimSub::scalar_delete", 0x005b46f0, &H::delete_5b46f0);
#undef E3D

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/en3d_b86_04.cpp (en3d/b86_04)
namespace {
namespace en3d_b86_04 {

using joan::orig::helpers::at;

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // Scalar deleting destructor: dtor thunk, operator delete when flag bit 0.
    void* scalar_delete(std::uint32_t flags, std::uintptr_t dtor) {
        thiscall(dtor, this);
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
    // Vector deleting destructor (Track / ColourTrack / AlphaTrack elements: 8 bytes on x86, count at this - 4).
    void* vector_delete(std::uint32_t flags, std::uintptr_t dtor) {
        if ((flags & 2) != 0) {
            joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(this)), kTrackSize, joan::orig::arg::i32(f<std::int32_t>(-4)), reinterpret_cast<std::uintptr_t>(obj(dtor)));  // eh vector dtor
            if ((flags & 1) != 0) game_delete(&f<std::uint8_t>(-4));
            return &f<std::uint8_t>(-4);
        }
        thiscall(dtor, this);
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
    void* del_5b4740(std::uint32_t flags) { return scalar_delete(flags, fn::code_value(fn::AnimDef_free_5b7b80)); }  // 0x005b7b80
    void* del_5b48d0(std::uint32_t flags) { return scalar_delete(flags, fn::code_value(fn::AnimDef_free_5b96a0)); }  // 0x005b96a0
    void* del_5b4920(std::uint32_t flags) { return vector_delete(flags, fn::code_value(fn::AnimDef_free_5b7b80)); }  // 0x005b7b80
    void* del_5b4790(std::uint32_t flags) { return vector_delete(flags, fn::code_value(fn::AnimDef_free_5b8a50)); }  // 0x005b8a50
    void* del_5b4830(std::uint32_t flags) { return vector_delete(flags, fn::code_value(fn::AnimArr_free)); }  // 0x005b90d0
};

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b86_04", name, addr, fn)
EN3D("AnimDef::scalar_delete_5b4740", 0x005b4740, &H::del_5b4740);
EN3D("AnimDef::scalar_delete_5b48d0", 0x005b48d0, &H::del_5b48d0);
EN3D("AnimDef::vector_delete_5b4920", 0x005b4920, &H::del_5b4920);
EN3D("AnimDef::vector_delete_5b4790", 0x005b4790, &H::del_5b4790);
EN3D("AnimDef::vector_delete_5b4830", 0x005b4830, &H::del_5b4830);
#undef EN3D

}  // namespace en3d_b86_04
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
//
//    - empty thiscall stubs (never write eax; no references at all - dead code):
//      0x00597870 / 0x005af040 / 0x005b0a10 / 0x005b49e0 / 0x005c50d0 / 0x005e4410 /
//      0x005eb5b0 (ret 0), 0x005a0290 / 0x005ac250 / 0x005af220 / 0x005b0a30 / 0x005eb570
//      (ret 4), 0x0059fda0 / 0x0059fdc0 / 0x005a0250 / 0x005a0270 (ret 0xc).
namespace {
namespace game_b105_16 {

struct H {
    // ---------------------------------------------------------------- stubs / getters
    void stub0() {}
    u8 true0() { return 1; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Obj::true_5b49c0", 0x005b49c0, &H::true0);
B("Obj::stub_5b49e0", 0x005b49e0, &H::stub0);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
//
//  * module game/b91_03:
//    TextCursor::advanced 0x0058c120, to_lower 0x0058e330, empty methods 0x0058e720 /
//    0x0059d1b0, Stream::forward 0x005939f0, Joystick::in_rect 0x00594360,
//    Joystick::pressed_kind 0x00594a10, Key::repeat_count 0x00595fb0, word getters
//    0x005979a0 / 0x005979c0, Pad::axis 0x00597c80, Pad::axis_range 0x00597e00,
//    same16 0x00598700, Pair::clear 0x005a1730, Bitmap::address 0x005a93b0,
//    MusicStream::release 0x005a98a0, MusicStream::read_header 0x005a9ad0,
//    MusicStream::reset 0x005a9b40, MusicStream::rewind 0x005aa040, Actor::set_byte_fd
//    0x005ad000, Table::get 0x005af330, Table::missing 0x005af360, AnimDef::lookup_word
//    0x005b4a00, Names::get 0x005ba910, Rect::set 0x005bed90 / 0x00612ee0, Array::at1
//    0x005ea410, RenderTarget::create_depth 0x006032d0, RenderTarget::release_depth
//    0x00603380, z_plane_hit 0x0060e6a0, BitWriter::put 0x0060ff70, BitWriter::drop
//    0x006100a0, Viewport::set_int 0x00614110, Sound3d::set 0x00616a90, getter 0x00616b60,
//    Dialog::reset_name 0x00616f80, base dtor bodies 0x00628ad0 / 0x0062c720,
//    TextBox::init 0x0062b8d0, Scroll::set 0x0062be10 / 0x0062cd40, Scroll::at_end
//    0x0062c040 / 0x0062cf70, Scroll::set_end 0x0062ced0, TextBox::cursor_x 0x00630770,
//    TextBox::line_y 0x00630ab0, getter 0x00631770, Table16::entry 0x00638150.
namespace {
namespace game_b91_03 {

struct H {
    // 0x005b4a00
    std::int16_t lookup_word(const char* a, std::uint8_t b, void* c) {
        std::int16_t first = 0, second = 0;
        joan::orig::as_anim_def(this)->find_action_rec(&first, &second, a, static_cast<std::uint8_t>(joan::orig::arg::u32(b)), c);  // 0x005b4a60
        return first;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("AnimDef::lookup_word", 0x005b4a00, &H::lookup_word);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/en3d_b83_08.cpp (en3d/b83_08)
//
//      0x005b5be0 AnimDef::find_action(name) (ret 4) - _stricmp binary search over the sorted
//                 index +0x13c into the 0x41-byte names +0x138 (+0x12c entries), index + 1 or 0
//
//      0x005b4a60 AnimDef::find_action(&action, &child, name, flag, arg) (ret 0x14) - own table,
//                 then children 1..(short)+0x14c (0x005b7650, recursive through the thunk);
//                 "Cannot find a action" (0x00586cb0) unless byte +0x14e
namespace {
namespace en3d_b83_08 {


void* const kLog = obj(d::g_err);        // error log

JOAN_COUNTER(c_find_action, "AnimDef::find_action");
JOAN_COUNTER(c_find_action_rec, "AnimDef::find_action_rec");

struct H {
    const char* action_name(std::int32_t i) {
        const std::uint32_t index = jo::view<std::uint32_t>(jo::self<jo::AnimDef>(this)->sort_index[i].value);
        return reinterpret_cast<const char*>(jo::self<jo::AnimDef>(this)->actions) + index * 0x41u;
    }
    std::int32_t stricmp_(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

    // 0x005b5be0
    std::int32_t find_action(const char* name) {
        c_find_action.hit();
        if (jo::self<jo::AnimDef>(this)->action_count <= 0) return 0;
        std::int32_t lo = 0;
        std::int32_t hi = jo::self<jo::AnimDef>(this)->action_count - 1;
        if (stricmp_(name, action_name(lo)) < 0) return 0;
        if (stricmp_(name, action_name(hi)) > 0) return 0;
        while (hi - lo > 1) {
            const std::int32_t mid = (lo + hi) / 2;
            if (stricmp_(name, action_name(mid)) < 0)
                hi = mid;
            else
                lo = mid;
        }
        if (stricmp_(name, action_name(lo)) == 0) return static_cast<std::int32_t>(jo::view<std::uint32_t>(jo::self<jo::AnimDef>(this)->sort_index[lo].value) + 1u);
        if (stricmp_(name, action_name(hi)) == 0) return static_cast<std::int32_t>(jo::view<std::uint32_t>(jo::self<jo::AnimDef>(this)->sort_index[hi].value) + 1u);
        return 0;
    }

    // 0x005b4a60
    void find_action_rec(std::int16_t* action, std::int16_t* child, const char* name, std::uint8_t flag, void* arg) {
        c_find_action_rec.hit();
        *action = 0;
        *child = 0;
        if (jo::self<jo::AnimDef>(this)->action_count > 0) {
            *action = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_def(this)->find_action(joan::orig::arg::str(name))));  // 0x005b5be0
            if (static_cast<std::int32_t>(*action) != 0) return;
            for (std::int16_t i = 1; static_cast<std::int32_t>(i) <= static_cast<std::int32_t>(jo::self<jo::AnimDef>(this)->child_count);
                 i = static_cast<std::int16_t>(i + 1)) {
                void* const sub = joan::orig::arg::as<void*>(joan::orig::as_anim_def(this)->entry(joan::orig::arg::i32(i)));  // 0x005b7650
                joan::orig::as_anim_def(sub)->find_action_rec(joan::orig::arg::mptr(action), joan::orig::arg::mptr(child), joan::orig::arg::str(name), static_cast<std::uint8_t>(joan::orig::arg::u32(flag)), arg);  // 0x005b4a60
                if (static_cast<std::int32_t>(*action) != 0) {
                    *child = i;
                    return;
                }
            }
        }
        if (static_cast<std::int8_t>(jo::self<jo::AnimDef>(this)->quiet) == 0)
            joan::orig::error::fatal(joan::orig::arg::mptr(kLog), str(d::s_Cannot_find_a_action_for_ACT), name, static_cast<const char*>(&jo::self<jo::AnimDef>(this)->name[0]));  // 0x00586cb0 "Cannot find a action for ACTION_ID: %s in %s!"
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b83_08", name, addr, fn)
E3D("AnimDef::find_action", 0x005b5be0, &H::find_action);
E3D("AnimDef::find_action_rec", 0x005b4a60, &H::find_action_rec);
#undef E3D

}  // namespace en3d_b83_08
}  // namespace

// ---------------------------------------- from modules/en3d_helpers30.cpp (en3d/helpers30)
//
//      0x005b7810 AnimDef::write_cache(file)        - .AMT cache writer (tracks +0x140)
//
//      0x005b6800 AnimDef::load_am3(directory, name) - optional .AM3 extras (+0x144, 0x005b9af0)
//
//      0x005b7930 AnimDef::read_cache(reader)        - .AMT cache reader
//
//      0x005b5830 AnimDef::read_header(reader)       - .AM2 header (bone names, track array)
//
//      0x005b4b90 AnimDef::read(reader)              - full .AM2 read (3x4 key matrices)
namespace {
namespace en3d_helpers30 {


JOAN_COUNTER(c_write_cache, "AnimDef::write_cache");
JOAN_COUNTER(c_load_am3, "AnimDef::load_am3");
JOAN_COUNTER(c_read_cache, "AnimDef::read_cache");
JOAN_COUNTER(c_read_header, "AnimDef::read_header");
JOAN_COUNTER(c_read, "AnimDef::read");

struct alignas(4) Path {
    std::uint8_t bytes[0x194];
};
struct alignas(alignof(void*)) Reader {  // a FileMem: 0x11c on x86, 0x12c on x64
    std::uint8_t bytes[((sizeof(joan::orig::FileMem) > 0x11c ? sizeof(joan::orig::FileMem) : 0x11c) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};

const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
void* files() { return img::obj(d::g_misc); }

// new Track[count] (8 bytes each on x86, 0x005b7b40 ctor / 0x005b7b80 dtor), MSVC array cookie (a 4-byte count).
void* new_tracks(std::int32_t count) {
    constexpr std::uint32_t kSize = sizeof(jo::Track);
    std::int32_t* const block = joan::orig::arg::as<std::int32_t*>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<std::uint32_t>(count) * kSize + 4u)));
    if (block == nullptr) return nullptr;
    block[0] = count;
    joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(block + 1)), kSize, joan::orig::arg::i32(count), fn::code_value(fn::Pair_Pair_5b7b40), fn::code_value(fn::AnimDef_free_5b7b80));  // `eh vector constructor iterator'
    return block + 1;
}

struct H {
    // 0x005b7810 AnimDef::write_cache(file): track count, 0, bone track count (+0x148), 0, 0,
    // then per track the key count and the 32-byte keys; then 0x005b4fe0.
    std::int32_t write_cache(void* file) {
        c_write_cache.hit();
        if (jo::self<jo::AnimDef>(this)->loaded != 2) return 0;
        joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(jo::self<jo::AnimDef>(this)->track_count));  // 0x00587da0
        joan::orig::as_file(file)->file_put_long(0);
        joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::AnimDef>(this)->bones_resolved)));
        joan::orig::as_file(file)->file_put_long(0);
        joan::orig::as_file(file)->file_put_long(0);
        for (std::int32_t i = 0; i < jo::self<jo::AnimDef>(this)->track_count; ++i) {
            std::uint8_t* const track = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->tracks[i]);
            joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(jo::as_track(track)->key_count));
            // the keys (KeyFrame = AnimKey, 8 x 4 bytes, the same on x86 and x64): one array, as before
            joan::save::write_array(*joan::orig::as_file(file), jo::view<void*>(jo::as_track(track)->keys), joan::orig::arg::u32(jo::as_track(track)->key_count), joan::orig::arg::u32(sizeof(jo::KeyFrame)));  // 0x00587510
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_def(this)->write_extras(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x005b4fe0
        return 1;
    }

    // 0x005b6800 AnimDef::load_am3(directory, name): if <dir><name>.AM3 exists, +0x144 = new
    // 0x005b9800 (12 bytes) and read it (0x005b9af0); returns that result, 0 when missing.
    std::int32_t load_am3(const char* directory, const char* anim_name) {
        c_load_am3.hit();
        Path path;
        joan::orig::as_string(&path)->construct(joan::orig::arg::str(directory));                     // 0x005a11a0
        joan::orig::as_string(&path)->append(joan::orig::arg::str(anim_name));                               // 0x005a1550
        joan::orig::as_string(&path)->append(joan::orig::arg::str(&img::global<const char>(d::s_AM3)));  // ".AM3"
        if (joan::orig::as_misc(files())->is_file_exist(joan::orig::arg::str(c_str(path))) == 0) return 0;  // 0x00590b80
        void* object = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::BoneTree)));  // 0xc on x86
        if (object != nullptr) object = joan::orig::as_bone_tree(object)->construct();  // 0x005b9800
        jo::view<void*>(jo::self<jo::AnimDef>(this)->bones) = object;
        Reader reader;
        joan::orig::as_file_mem(&reader)->construct();                                // 0x00587f80
        joan::orig::as_file_mem(&reader)->open_588000(joan::orig::arg::str(c_str(path)), 1, static_cast<char>(joan::orig::arg::i32(0)));      // 0x00588000 open
        const std::int32_t result = joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_tree(jo::view<void*>(jo::self<jo::AnimDef>(this)->bones))->read(&reader));  // 0x005b9af0
        joan::orig::as_file_mem(&reader)->close();                                       // 0x005886d0 close
        joan::orig::as_file_mem(&reader)->destruct();                                       // 0x00587fd0 ~Reader
        return result;
    }

    // 0x005b7930 AnimDef::read_cache(reader): state must be 1 and the track count match.
    std::int32_t read_cache(void* reader) {
        c_read_cache.hit();
        if (jo::self<jo::AnimDef>(this)->loaded != 1) return 0;
        if (read_int(reader) != jo::self<jo::AnimDef>(this)->track_count) return 0;
        if (jo::view<void*>(jo::self<jo::AnimDef>(this)->tracks) == nullptr) jo::view<void*>(jo::self<jo::AnimDef>(this)->tracks) = new_tracks(jo::self<jo::AnimDef>(this)->track_count);
        read_int(reader);
        const std::int32_t bone_tracks = read_int(reader);
        read_int(reader);
        read_int(reader);
        for (std::int32_t i = 0; i < jo::self<jo::AnimDef>(this)->track_count; ++i) {
            std::uint8_t* const track = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->tracks[i]);
            const std::int32_t count = read_int(reader);
            const std::uint32_t first = joan::orig::arg::as<std::uint32_t>(joan::orig::as_track(track)->allocate(joan::orig::arg::i32(count)));  // 0x005b7bf0 allocate
            joan::save::read_array(*joan::orig::as_file_mem(reader), joan::orig::arg::mptr(static_cast<void*>(jo::as_track(track)->keys + first)), joan::orig::arg::u32(count), joan::orig::arg::u32(sizeof(jo::KeyFrame)));  // 0x00588c60
        }
        jo::view<std::int32_t>(jo::self<jo::AnimDef>(this)->bones_resolved) = bone_tracks;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_def(this)->read_extras(joan::orig::arg::mptr(reader))) == 0) return 0;  // 0x005b5350
        return 1;
    }

    // Shared bone table read of 0x005b5830 / 0x005b4b90.
    void read_counts(void* reader) {
        jo::self<jo::AnimDef>(this)->track_count = read_int(reader);
        jo::self<jo::AnimDef>(this)->action_count = read_int(reader);
        read_bytes(reader, &jo::self<jo::AnimDef>(this)->start, 4);  // +0x130
        read_bytes(reader, &jo::self<jo::AnimDef>(this)->end, 4);  // +0x134
    }
    void alloc_bones(std::int32_t line) {
        jo::view<void*>(jo::self<jo::AnimDef>(this)->actions) = joan::orig::as_mem(img::obj(d::g_mem))->add(joan::orig::arg::u32(jo::self<jo::AnimDef>(this)->action_count * 0x41), joan::orig::arg::str(&img::global<const char>(d::s_d_ww_en3d_animdef_cpp)), joan::orig::arg::i32(line));  // 0x0058e7b0
        std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::AnimDef>(this)->actions)), 0, joan::orig::arg::u32(jo::self<jo::AnimDef>(this)->action_count * 0x41));  // memset
    }
    std::uint8_t* read_bone(void* reader, std::int32_t i) {
        std::uint8_t* const bone = jo::view<std::uint8_t*>(jo::self<jo::AnimDef>(this)->actions) + i * 0x41;
        read_bytes(reader, bone, 0x32);
        jo::as_anim_action(bone)->name[0x32] = 0;
        jo::view<std::int32_t>(jo::as_anim_action(bone)->value) = read_int(reader);
        read_bytes(reader, &jo::as_anim_action(bone)->start, 4);
        read_bytes(reader, &jo::as_anim_action(bone)->end, 4);
        return bone;
    }

    // 0x005b5830 AnimDef::read_header(reader).
    std::int32_t read_header(void* reader) {
        c_read_header.hit();
        read_counts(reader);
        alloc_bones(global<std::int16_t>(d::g_a16e80) + 9);
        jo::view<void*>(jo::self<jo::AnimDef>(this)->tracks) = new_tracks(jo::self<jo::AnimDef>(this)->track_count);
        for (std::int32_t i = 0; i < jo::self<jo::AnimDef>(this)->action_count; ++i) read_bone(reader, i);
        return 1;
    }

    // 0x005b4b90 AnimDef::read(reader): header (bones / tracks allocated only when missing),
    // then per track the key count and per key a time and a 3x4 matrix (row-major in the file,
    // transposed into the identity at 0xa18a78) -> key 0x005b8770; every 512 keys the
    // streamer (0x0059b1c0 on 0xa5d0d0) and 0x005866d0 on 0xa5cd70 are pumped.
    std::int32_t read(void* reader) {
        c_read.hit();
        read_counts(reader);
        if (jo::view<void*>(jo::self<jo::AnimDef>(this)->actions) == nullptr) alloc_bones(global<std::int16_t>(d::g_a16e7c) + 0xc);
        if (jo::view<void*>(jo::self<jo::AnimDef>(this)->tracks) == nullptr) jo::view<void*>(jo::self<jo::AnimDef>(this)->tracks) = new_tracks(jo::self<jo::AnimDef>(this)->track_count);
        for (std::int32_t i = 0; i < jo::self<jo::AnimDef>(this)->action_count; ++i) read_bone(reader, i)[0x40] = 1;
        // Kept across iterations like the original's stack slots (only matter for a truncated
        // file, where the reader leaves them unwritten).
        std::uint32_t time = 0;
        std::uint32_t raw[12] = {};
        for (std::int32_t i = 0; i < jo::self<jo::AnimDef>(this)->track_count; ++i) {
            std::uint8_t* const track = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->tracks[i]);
            const std::int32_t count = read_int(reader);
            const std::int32_t first = joan::orig::arg::as<std::int32_t>(joan::orig::as_track(track)->allocate(joan::orig::arg::i32(count)));  // 0x005b7bf0 allocate
            for (std::int32_t j = 0; j < count; ++j) {
                std::uint8_t* const key = jo::view<std::uint8_t*>(jo::as_track(track)->keys) + (static_cast<std::uint32_t>(first + j) << 5);
                read_bytes(reader, &time, 4);
                read_bytes(reader, raw, 0x30);
                std::uint32_t m[16];
                std::memcpy(m, img::obj(d::g_identity_matrix), sizeof(m));
                m[0] = raw[0];
                m[4] = raw[1];
                m[8] = raw[2];
                m[12] = raw[3];
                m[1] = raw[4];
                m[5] = raw[5];
                m[9] = raw[6];
                m[13] = raw[7];
                m[2] = raw[8];
                m[6] = raw[9];
                m[10] = raw[10];
                m[14] = raw[11];
                joan::orig::as_key(key)->set(joan::orig::arg::u32(time), joan::orig::arg::mptr(static_cast<void*>(m)));  // 0x005b8770
                if ((j & 0x1ff) == 0) {
                    joan::orig::as_sound_stream(img::obj(d::g_snd))->update();  // 0x0059b1c0
                    joan::orig::as_ambient_streams(img::obj(d::g_mus))->update();  // 0x005866d0
                }
            }
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_def(this)->read_extras(joan::orig::arg::mptr(reader))) == 0) return 0;  // 0x005b5350
        return 1;
    }
};

#define HELPER30(name, addr, fn) JOAN_REPLACE("en3d/helpers30", name, addr, fn)
HELPER30("AnimDef::write_cache", 0x005b7810, &H::write_cache);
HELPER30("AnimationDef::read_hierarchy_5b6800", 0x005b6800, &H::load_am3);
HELPER30("AnimationDef::read_file_amt", 0x005b7930, &H::read_cache);
HELPER30("AnimationDef::read_file_am2_header", 0x005b5830, &H::read_header);
HELPER30("AnimDef::read", 0x005b4b90, &H::read);
#undef HELPER30

}  // namespace en3d_helpers30
}  // namespace

// ---------------------------------------- from modules/en3d_helpers32.cpp (en3d/helpers32)
//
//      0x005b5350 AnimDef::read_extras(reader)   - tagged sections after the .AM2 tracks (tag 1:
//                 per-track 16-byte (+0x153) and 8-byte (+0x157) key lists, 0x3c-byte
//                 events (+0x15b)); tag 0 ends
//
//      0x005b4fe0 AnimDef::write_extras(file)    - writer counterpart (.AMT cache)
//
//      0x005b83a0 Track::compress()              - drops keys that the interpolation of their
//                 neighbours reproduces (eps 0.01 position, 5e-6 rotation, gap < 250.1)
//
//      0x005b6b30 AnimDef::make_relative(bone)   - children first, then the bone's keys made
//                 relative to the parent's interpolated key (inverse(parent) * key)
//
//      0x005b69c0 AnimDef::read_am3(reader)      - +0x144 = new BoneTree, read it
//
//      0x005b7bf0 Track::allocate(n)             - grows the 32-byte key array, returns old count
//
//      0x005b8770 Key::set(time, matrix)         - time + rotation (0x0060c840) + translation
namespace {
namespace en3d_helpers32 {

std::uint8_t* offset_ptr(const void* p, std::uint32_t bytes) {
    return reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(p) + bytes);
}
float bits_float(std::uint32_t bits) {
    float v;
    std::memcpy(&v, &bits, 4);
    return v;
}

JOAN_COUNTER(c_read_extras, "AnimDef::read_extras");
JOAN_COUNTER(c_write_extras, "AnimDef::write_extras");
JOAN_COUNTER(c_compress, "Track::compress");
JOAN_COUNTER(c_make_relative, "AnimDef::make_relative");

constexpr img::Addr kMemory(d::g_mem);

void* op_new(std::uint32_t size) { return joan::crt::operator_new(joan::orig::arg::u32(size)); }
void write_int(void* file, std::int32_t v) { joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(v)); }  // 0x00587da0
void write_bytes(void* file, void* p, std::int32_t size) {
    joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(size));  // 0x005872e0
}

// new T[count] with ColourTrack / AlphaTrack elements (kTrackSize: 8 bytes on x86), MSVC array cookie,
// `eh vector constructor iterator'.
void* new_array8(std::int32_t count, std::uintptr_t ctor, std::uintptr_t dtor) {
    constexpr std::uint32_t size = kTrackSize;
    std::int32_t* const block = static_cast<std::int32_t*>(op_new(static_cast<std::uint32_t>(count) * size + 4u));
    if (block == nullptr) return nullptr;
    block[0] = count;
    joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(block + 1)), size, joan::orig::arg::i32(count), ctor, dtor);
    return block + 1;
}

struct alignas(4) Key {
    float v[8];  // time, then 7 values
};
struct alignas(4) Matrix {
    float m[16];
};

struct H {
    // 0x005b5350 AnimDef::read_extras(reader).
    std::int32_t read_extras(void* reader) {
        c_read_extras.hit();
        for (;;) {
            const std::int32_t tag = read_int(reader);
            if (tag == 0) break;
            if (tag != 1) continue;
            jo::self<jo::AnimDef>(this)->extra_count = read_int(reader);
            for (std::int32_t i = 0; i < jo::self<jo::AnimDef>(this)->extra_count; ++i) {
                std::int32_t n = read_int(reader);
                if (n != 0) {
                    if (jo::view<void*>(jo::self<jo::AnimDef>(this)->colour_tracks) == nullptr)
                        jo::view<void*>(jo::self<jo::AnimDef>(this)->colour_tracks) = new_array8(jo::self<jo::AnimDef>(this)->extra_count, fn::code_value(fn::Pair_Pair_5b8a10), fn::code_value(fn::AnimDef_free_5b8a50));  // 0x005b8a10 / 0x005b8a50
                    std::uint8_t* const list = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->colour_tracks[i]);
                    const std::int32_t first = joan::orig::arg::as<std::int32_t>(joan::orig::as_colour_track(list)->allocate(joan::orig::arg::i32(n)));  // 0x005b8ac0
                    for (std::int32_t j = 0; j < n; ++j) {
                        std::uint8_t* const e =
                            offset_ptr(jo::view<void*>(jo::as_colour_track(list)->keys), static_cast<std::uint32_t>(first + j) << 4);
                        read_bytes(reader, e, 4);
                        read_bytes(reader, e + 4, 4);
                        read_bytes(reader, e + 8, 4);
                        read_bytes(reader, e + 0xc, 4);
                    }
                }
                n = read_int(reader);
                if (n != 0) {
                    if (jo::view<void*>(jo::self<jo::AnimDef>(this)->alpha_tracks) == nullptr)
                        jo::view<void*>(jo::self<jo::AnimDef>(this)->alpha_tracks) = new_array8(jo::self<jo::AnimDef>(this)->extra_count, fn::code_value(fn::Pair_Pair_5b9090), fn::code_value(fn::AnimArr_free));  // 0x005b9090 / 0x005b90d0
                    std::uint8_t* const list = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->alpha_tracks[i]);
                    const std::int32_t first = joan::orig::arg::as<std::int32_t>(joan::orig::as_alpha_track(list)->allocate(joan::orig::arg::i32(n)));  // 0x005b9140
                    for (std::int32_t j = 0; j < n; ++j) {
                        std::uint8_t* const e =
                            offset_ptr(jo::view<void*>(jo::as_alpha_track(list)->keys), static_cast<std::uint32_t>(first + j) * 8u);
                        read_bytes(reader, e, 4);
                        read_bytes(reader, e + 4, 4);
                    }
                }
            }
            const std::int32_t events = read_int(reader);
            if (events > 0) {
                void* obj = op_new(static_cast<std::uint32_t>(sizeof(jo::EventList)));  // 8 on x86
                if (obj != nullptr) obj = joan::orig::as_event_list(obj)->construct();  // 0x005b9660
                jo::view<void*>(jo::self<jo::AnimDef>(this)->events) = obj;
                const std::int32_t first = joan::orig::arg::as<std::int32_t>(joan::orig::as_event_list(jo::view<void*>(jo::self<jo::AnimDef>(this)->events))->allocate(joan::orig::arg::i32(events)));  // 0x005b9710
                for (std::int32_t j = 0; j < events; ++j) {
                    std::uint8_t* const e = offset_ptr(jo::view<void*>(jo::self<jo::AnimDef>(this)->events->events),
                                                       static_cast<std::uint32_t>(first + j) * 0x3cu);
                    read_bytes(reader, e, 4);
                    const std::int32_t v = read_int(reader);
                    jo::as_anim_event(e)->bone = v == -1 ? -999 : v;
                    read_bytes(reader, e + 8, 0x32);
                    read_bytes(reader, e + 0x3b, 1);
                }
            }
        }
        return 1;
    }

    // 0x005b4fe0 AnimDef::write_extras(file).
    std::int32_t write_extras(void* file) {
        c_write_extras.hit();
        if (jo::self<jo::AnimDef>(this)->extra_count > 0 || jo::view<void*>(jo::self<jo::AnimDef>(this)->events) != nullptr) {
            write_int(file, 1);
            write_int(file, jo::self<jo::AnimDef>(this)->extra_count);
            for (std::int32_t i = 0; i < jo::self<jo::AnimDef>(this)->extra_count; ++i) {
                std::int32_t n = jo::view<void*>(jo::self<jo::AnimDef>(this)->colour_tracks) != nullptr ? jo::self<jo::AnimDef>(this)->colour_tracks[i].count : 0;
                write_int(file, n);
                if (n != 0) {
                    std::uint8_t* const list = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->colour_tracks[i]);
                    for (std::int32_t j = 0; j < n; ++j) {
                        std::uint8_t* const e = offset_ptr(jo::view<void*>(jo::as_colour_track(list)->keys), static_cast<std::uint32_t>(j) << 4);
                        write_bytes(file, e, 4);
                        write_bytes(file, e + 4, 4);
                        write_bytes(file, e + 8, 4);
                        write_bytes(file, e + 0xc, 4);
                    }
                }
                n = jo::view<void*>(jo::self<jo::AnimDef>(this)->alpha_tracks) != nullptr ? jo::self<jo::AnimDef>(this)->alpha_tracks[i].count : 0;
                write_int(file, n);
                if (n != 0) {
                    std::uint8_t* const list = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->alpha_tracks[i]);
                    for (std::int32_t j = 0; j < n; ++j) {
                        std::uint8_t* const e = offset_ptr(jo::view<void*>(jo::as_alpha_track(list)->keys), static_cast<std::uint32_t>(j) * 8u);
                        write_bytes(file, e, 4);
                        write_bytes(file, e + 4, 4);
                    }
                }
            }
            const std::int32_t events = jo::view<void*>(jo::self<jo::AnimDef>(this)->events) != nullptr ? jo::self<jo::AnimDef>(this)->events->count : 0;
            write_int(file, events);
            if (events > 0) {
                for (std::int32_t j = 0; j < events; ++j) {
                    std::uint8_t* const e =
                        offset_ptr(jo::view<void*>(jo::self<jo::AnimDef>(this)->events->events), static_cast<std::uint32_t>(j) * 0x3cu);
                    write_bytes(file, e, 4);
                    write_int(file, jo::as_anim_event(e)->bone == -999 ? -1 : jo::as_anim_event(e)->bone);
                    write_bytes(file, e + 8, 0x32);
                    write_bytes(file, e + 0x3b, 1);
                }
            }
        }
        write_int(file, 0);
        return 1;
    }

    // 0x005b83a0 Track::compress(): this = track (+0 key count, +4 32-byte keys).
    void compress() {
        c_compress.hit();
        const float max_gap = bits_float(0x437a199a);  // 250.1
        const float eps_pos = bits_float(0x3c23d70a);  // 0.01
        const float eps_rot = bits_float(0x36a7c5ac);  // 5e-6
        std::int32_t out = 0;
        std::int32_t i = 0;
        for (i = 0; i < jo::self<jo::Track>(this)->key_count && i < 2; ++i) ++out;
        Key tmp;
        joan::orig::as_anim_key(&tmp)->construct();  // 0x005aaee0
        auto key_at = [&](std::int32_t index) {
            return reinterpret_cast<float*>(offset_ptr(jo::view<void*>(jo::self<jo::Track>(this)->keys), static_cast<std::uint32_t>(index) << 5));
        };
        float* a = key_at(out - 2);
        float* b = key_at(out - 1);
        float* c = key_at(i);
        for (; i < jo::self<jo::Track>(this)->key_count; ++i, c += 8) {
            const float t = (b[0] - a[0]) / (c[0] - a[0]);
            bool merged = false;
            if (!(c[0] - a[0] >= max_gap)) {  // < 250.1 or unordered
                joan::orig::as_anim_key(&tmp)->interpolate(joan::orig::arg::mptr(static_cast<void*>(a)), joan::orig::arg::mptr(static_cast<void*>(c)), joan::orig::arg::f32(t));  // 0x005b8840 interpolate
                bool close = true;
                for (std::int32_t k = 1; k < 8 && close; ++k) {
                    const float d = tmp.v[k] - b[k];
                    const double r = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(d))));  // fabs
                    const float eps = k < 5 ? eps_pos : eps_rot;
                    close = static_cast<double>(eps) >= r;  // fails when eps < r or unordered
                }
                if (close) {
                    std::memcpy(b, c, 0x20);
                    merged = true;
                }
            }
            if (!merged) {
                if (out != i) std::memcpy(key_at(out), c, 0x20);
                ++out;
                a = key_at(out - 2);
                b = key_at(out - 1);
            }
        }
        if (out > 0 && out < jo::self<jo::Track>(this)->key_count) {
            const std::int16_t line = global<std::int16_t>(d::g_a16e90);
            void* const keys = joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(out << 5), joan::orig::arg::str(img::str(kAnimdefFile)), joan::orig::arg::i32(line + 0x45));  // 0x0058e7b0
            std::memcpy(joan::orig::arg::mptr(keys), joan::orig::arg::ptr(jo::view<void*>(jo::self<jo::Track>(this)->keys)), joan::orig::arg::u32(out << 5));  // memcpy
            joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Track>(this)->keys)), joan::orig::arg::str(img::str(kAnimdefFile)), joan::orig::arg::i32(global<std::int16_t>(d::g_a16e90) + 0x47));  // 0x0058ed70
            jo::view<void*>(jo::self<jo::Track>(this)->keys) = keys;
            jo::self<jo::Track>(this)->key_count = out;
        }
    }

    // 0x005b6b30 AnimDef::make_relative(bone).
    std::int32_t make_relative(std::int32_t bone) {
        c_make_relative.hit();
        std::int32_t child = joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_tree(jo::view<void*>(jo::self<jo::AnimDef>(this)->bones))->first_child(joan::orig::arg::i32(bone)));  // 0x005ba040 first child
        while (child != -999) {
            joan::orig::as_anim_def(this)->make_relative(joan::orig::arg::i32(child));                        // 0x005b6b30
            child = joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_tree(jo::view<void*>(jo::self<jo::AnimDef>(this)->bones))->next_child(joan::orig::arg::i32(bone), joan::orig::arg::i32(child)));  // 0x005ba0a0 next child
        }
        const std::int32_t parent = joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_tree(jo::view<void*>(jo::self<jo::AnimDef>(this)->bones))->parent(joan::orig::arg::i32(bone)));  // 0x005b9fe0
        if (parent == -1) return 1;
        std::uint8_t* const track = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->tracks[bone]);
        std::uint8_t* const ptrack = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->tracks[parent]);
        float* key = jo::view<float*>(jo::as_track(track)->keys);
        std::int32_t j = 1;
        float* pa = jo::view<float*>(jo::as_track(ptrack)->keys);
        float* pb = jo::view<float*>(jo::as_track(ptrack)->keys) + 8;
        Key tmp;
        joan::orig::as_anim_key(&tmp)->construct();  // 0x005aaee0
        const float lo = bits_float(global<std::uint32_t>(d::flt_0_001));  // 0.001
        const float hi = bits_float(global<std::uint32_t>(d::flt_0_999));  // 0.999
        for (std::int32_t i = 0; i < jo::as_track(track)->key_count; ++i, key += 8) {
            while (key[0] > pb[0] && j + 1 < jo::as_track(ptrack)->key_count) {
                ++j;
                pa += 8;
                pb += 8;
            }
            const float t = (key[0] - pa[0]) / (pb[0] - pa[0]);
            if (!(t > lo))
                std::memcpy(&tmp, pa, 0x20);
            else if (!(t < hi))
                std::memcpy(&tmp, pb, 0x20);
            else
                joan::orig::as_anim_key(&tmp)->interpolate(joan::orig::arg::mptr(static_cast<void*>(pa)), joan::orig::arg::mptr(static_cast<void*>(pb)), joan::orig::arg::f32(t));  // 0x005b8840
            Matrix m_parent, m_inverse, m_key, m_rel;
            void* const pm = joan::orig::arg::as<void*>(joan::orig::as_anim_key(&tmp)->to_matrix(&m_parent));              // 0x005b8910
            void* const inv = import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(&m_inverse), pm);  // matrix_inverse
            void* const km = joan::orig::arg::as<void*>(joan::orig::as_anim_key(key)->to_matrix(&m_key));                  // 0x005b8910
            import_call<void*>(d::iat_v3d_87_matrix_multiply_3, static_cast<void*>(&m_rel), km, inv);         // matrix_multiply
            joan::orig::as_key(key)->set(joan::orig::arg::u32(jo::as_key_frame(key)->time), joan::orig::arg::mptr(static_cast<void*>(&m_rel)));  // 0x005b8770
        }
        return 1;
    }

    // 0x005b69c0 AnimDef::read_am3(reader).
    std::int32_t read_am3(void* reader) {
        if (reader == nullptr) return 0;
        void* tree = op_new(static_cast<std::uint32_t>(sizeof(jo::BoneTree)));  // 0xc on x86
        if (tree != nullptr) tree = joan::orig::as_bone_tree(tree)->construct();  // 0x005b9800
        jo::view<void*>(jo::self<jo::AnimDef>(this)->bones) = tree;
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_tree(jo::view<void*>(jo::self<jo::AnimDef>(this)->bones))->read(joan::orig::arg::mptr(reader)));  // 0x005b9af0
    }

    // 0x005b7bf0 Track::allocate(n): grows the key array by n, returns the first new index.
    std::int32_t track_allocate(std::int32_t n) {
        jo::view<void*>(jo::self<jo::Track>(this)->keys) = joan::orig::as_mem(img::obj(kMemory))->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Track>(this)->keys)), joan::orig::arg::u32((jo::self<jo::Track>(this)->key_count + n) << 5), joan::orig::arg::str(img::str(kAnimdefFile)), joan::orig::arg::i32(global<std::int16_t>(d::g_a16e8c) + 1));  // 0x0058eb30
        const std::int32_t first = jo::self<jo::Track>(this)->key_count;
        jo::self<jo::Track>(this)->key_count = jo::self<jo::Track>(this)->key_count + n;
        return first;
    }

    // 0x005b8770 Key::set(time, matrix).
    std::uintptr_t key_set(std::uint32_t time, void* matrix) {
        jo::self<jo::KeyFrame>(this)->time = time;
        joan::orig::quat_from_matrix(joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::KeyFrame>(this)->rotation)), joan::orig::arg::mptr(matrix));  // 0x0060c840
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::matrix_position(static_cast<void*>(jo::self<jo::KeyFrame>(this)->position), joan::orig::arg::ptr(matrix)));  // 0x0060c310
    }
};

#define HELPER32(name, addr, fn) JOAN_REPLACE("en3d/helpers32", name, addr, fn)
HELPER32("AnimationDef::read_file_extension", 0x005b5350, &H::read_extras);
HELPER32("Track::compress", 0x005b83a0, &H::compress);
HELPER32("AnimDef::write_extras", 0x005b4fe0, &H::write_extras);
HELPER32("AnimDef::make_relative", 0x005b6b30, &H::make_relative);
HELPER32("AnimationDef::read_hierarchy_5b69c0", 0x005b69c0, &H::read_am3);
HELPER32("Track::allocate", 0x005b7bf0, &H::track_allocate);
HELPER32("Key::set", 0x005b8770, &H::key_set);
#undef HELPER32

}  // namespace en3d_helpers32
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
//
//    - AnimDef::resolve_bones (0x005b6a90): once, matches every bone without an index.
//
//    - AnimDef::build_sort_index (0x005b5a70, en3d/animdef.cpp): identity index then qsort.
namespace {
namespace game_leaves80b {


constexpr img::Addr kMem(d::g_mem);

JOAN_COUNTER(c_resolve_bones, "AnimDef::resolve_bones");
JOAN_COUNTER(c_sort_index, "AnimDef::build_sort_index");

struct H {
    // 0x005b6a90
    std::int32_t resolve_bones() {
        c_resolve_bones.hit();
        if (jo::view<std::int32_t>(jo::self<jo::AnimDef>(this)->bones_resolved) != 0) return 1;
        for (std::int32_t i = 0; i < *jo::view<std::int32_t*>(jo::self<jo::AnimDef>(this)->bones); ++i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_tree(jo::view<void*>(jo::self<jo::AnimDef>(this)->bones))->parent(joan::orig::arg::i32(i))) == -1) joan::orig::as_anim_def(this)->make_relative(joan::orig::arg::i32(i));  // 0x005b9fe0 / 0x005b6b30
        }
        jo::view<std::int32_t>(jo::self<jo::AnimDef>(this)->bones_resolved) = 1;
        return 1;
    }

    // 0x005b5a70
    void build_sort_index() {
        c_sort_index.hit();
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::AnimDef>(this)->loaded)) != 0) return;
        if (jo::self<jo::AnimDef>(this)->action_count <= 0) return;
        const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_a16e84)) + 0xa;
        jo::view<std::int32_t*>(jo::self<jo::AnimDef>(this)->sort_index) = joan::orig::arg::as<std::int32_t*>(joan::orig::as_mem(img::obj(kMem))->add(joan::orig::arg::u32(static_cast<std::uint32_t>(jo::self<jo::AnimDef>(this)->action_count << 2)), joan::orig::arg::str(&img::global<const char>(d::s_d_ww_en3d_animdef_cpp)), joan::orig::arg::i32(line)));  // 0x0058e7b0 alloc
        for (std::int32_t i = 0; i < jo::self<jo::AnimDef>(this)->action_count; ++i) jo::view<std::int32_t*>(jo::self<jo::AnimDef>(this)->sort_index)[i] = i;
        global<void*>(d::g_a67684) = this;
        joan::orig::libc::qsort(joan::orig::arg::mptr(static_cast<void*>(jo::view<std::int32_t*>(jo::self<jo::AnimDef>(this)->sort_index))), joan::orig::arg::u32(jo::self<jo::AnimDef>(this)->action_count), 4u, reinterpret_cast<void*>(fn::code_value(fn::AnimDef_cmp_names)));  // qsort
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("AnimationDef::arrange_hierarchy", 0x005b6a90, &H::resolve_bones);
L80B("AnimationDef::build_sorted_anim_section", 0x005b5a70, &H::build_sort_index);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/game_b102_10.cpp (game/b102_10)
//
//    - AnimDef::cmp_names 0x005b5b70 (cdecl qsort comparator).
//
//    - AnimDef::set_flag 0x005b7450 (name, v, b), ret 0xc / AnimDef::add_track 0x005b7530 (a, b, c),
//      ret 0xc / AnimDef::set_all_flags 0x005b76b0 (v, b), ret 8 / AnimDef::total_size 0x005b7740.
//
//    - Rec::set_5b87d0 0x005b87d0 (a, vec4*, vec3*), ret 0xc.
//
//  - Live references: 0x005a8ea0 (jpeg error manager, stored at 0x005a8e26), 0x005a9d80 (two callers,
//    `and eax,0xff`), 0x005b5b70 (qsort at 0x005b5b08), 0x005cd3a0 (callback, 0x005cc8d0). All the others
//    have neither callers nor data references (dead code).
//
//  - 0x005af0d0 / 0x005b7530: the original SEH frame only deletes the new object if its constructor
//    throws; the constructors (0x005aab20, 0x005b2390) only memset, so the normal path is all there is.
namespace {
namespace game_b102_10 {

using u16 = std::uint16_t;


i32 crt_stricmp(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
using joan::orig::helpers::op_new;

JOAN_COUNTER(c_add_track, "AnimDef::add_track");

// 0x005b5b70 qsort comparator on animation names (0x41-byte records at [[0xa67684]+0x138]).
i32 JOAN_CDECL anim_cmp_names(const i32* a, const i32* b) {
    u8* const pa = jo::view<u8*>((jo::as_anim_def(global<void*>(d::g_a67684)))->actions) + static_cast<u32>(*a) * 0x41u;
    u8* const pb = jo::view<u8*>((jo::as_anim_def(global<void*>(d::g_a67684)))->actions) + static_cast<u32>(*b) * 0x41u;
    return crt_stricmp(pa, pb);
}

struct H {
    // 0x005b7450
    u32 set_flag(u32 name, u32 v, u8 b) {
        const i32 r = joan::orig::arg::as<i32>(joan::orig::as_anim_def(this)->find_action(joan::orig::arg::str(name)));  // 0x005b5be0 find (1-based)
        if (r == 0) return 0;
        const u32 o = static_cast<u32>(r - 1) * 0x41u;
        (jo::as_anim_action(jo::view<u8*>(jo::self<jo::AnimDef>(this)->actions) + o))->value = v;
        (jo::as_anim_action(jo::view<u8*>(jo::self<jo::AnimDef>(this)->actions) + o))->flag = b;
        return o;
    }
    // 0x005b7530
    u32 add_track(u32 a, u32 b, u8 c) {
        c_add_track.hit();
        void* const mem = op_new(static_cast<u32>(sizeof(jo::AnimDef)));  // 0x18b on x86
        u8* const t = mem != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_anim_def(mem)->construct()) : nullptr;  // 0x005b2390 ctor
        joan::orig::as_anim_def(t)->load(joan::orig::arg::str(a), joan::orig::arg::str(b), joan::orig::arg::u32(c));                                           // 0x005b23d0
        jo::as_anim_def(t)->u16_02 = jo::self<jo::AnimDef>(this)->u16_02;
        jo::as_anim_def(t)->quiet = 1;
        const i32 idx = jo::self<jo::AnimDef>(this)->child_count;
        jo::view<u8*>(jo::self<jo::AnimDef>(this)->children[idx]) = t;
        jo::view<u16>(jo::self<jo::AnimDef>(this)->child_count) = static_cast<u16>(jo::view<u16>(jo::self<jo::AnimDef>(this)->child_count) + 1);
        return (joan::call::register_low_bits(this) & 0xffff0000u) | jo::view<u16>(jo::self<jo::AnimDef>(this)->child_count);
    }
    // 0x005b76b0
    void set_all_flags(u32 v, u8 b) {
        for (i32 i = 1; i <= jo::self<jo::AnimDef>(this)->action_count; ++i) {
            const u32 o = static_cast<u32>(i - 1) * 0x41u;
            (jo::as_anim_action(jo::view<u8*>(jo::self<jo::AnimDef>(this)->actions) + o))->value = v;
            (jo::as_anim_action(jo::view<u8*>(jo::self<jo::AnimDef>(this)->actions) + o))->flag = b;
        }
    }
    // 0x005b7740
    u32 total_size() {
        u32 sum = 0;
        for (i32 i = 0; i < jo::self<jo::AnimDef>(this)->track_count; ++i) sum += jo::view<u32>(jo::self<jo::AnimDef>(this)->tracks[i].key_count) << 5;
        return sum;
    }
    // 0x005b87d0
    u32 set_5b87d0(u32 a, const u32* b, const u32* c) {
        jo::view<u32>(jo::self<jo::AnimKey>(this)->time) = a;
        jo::view<u32>(jo::self<jo::AnimKey>(this)->rot[0]) = b[0];
        jo::view<u32>(jo::self<jo::AnimKey>(this)->rot[1]) = b[1];
        jo::view<u32>(jo::self<jo::AnimKey>(this)->rot[2]) = b[2];
        jo::view<u32>(jo::self<jo::AnimKey>(this)->rot[3]) = b[3];
        jo::view<u32>(jo::self<jo::AnimKey>(this)->pos[0]) = c[0];
        jo::view<u32>(jo::self<jo::AnimKey>(this)->pos[1]) = c[1];
        jo::view<u32>(jo::self<jo::AnimKey>(this)->pos[2]) = c[2];
        return c[2];
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_10", name, addr, fn)
B("AnimDef::cmp_names", 0x005b5b70, &anim_cmp_names);
B("AnimDef::set_flag", 0x005b7450, &H::set_flag);
B("AnimationDef::add_child", 0x005b7530, &H::add_track);
B("AnimDef::set_all_flags", 0x005b76b0, &H::set_all_flags);
B("AnimDef::total_size", 0x005b7740, &H::total_size);
B("Rec::set_5b87d0", 0x005b87d0, &H::set_5b87d0);
#undef B

}  // namespace game_b102_10
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {


// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    void* anim_frame(std::int32_t index, std::uint32_t sub) {  // 0x005b5dd0 (animdef.cpp)
        if (static_cast<std::int16_t>(sub) != 0) {
            void* const child = joan::orig::arg::as<void*>(joan::orig::as_anim_def(self())->entry(joan::orig::arg::i32(static_cast<std::uint16_t>(sub))));  // 0x005b7650
            return joan::orig::arg::as<void*>(joan::orig::as_anim_set(child)->frame(joan::orig::arg::i32(index), 0u));
        }
        return jo::view<std::uint8_t*>(jo::self<jo::AnimDef>(this)->actions) + (index - 1) * 0x41;
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("AnimationDef::get_motion_info", 0x005b5dd0, &H::anim_frame);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/en3d_helpers13.cpp (en3d/helpers13)
namespace {
namespace en3d_helpers13 {


bool below_or_nan(float a, float b) { return !(a >= b); }
std::uint8_t to_byte(float v) { return static_cast<std::uint8_t>(static_cast<std::int64_t>(v * 255.0f)); }  // fmul 255.0f, _ftol

JOAN_COUNTER(c_animate, "AnimDef::animate");

// [Debug] AnimCensus=N: every N seconds, which animations AnimDef::animate plays how often, how long each is and
// whether it moves anything at all ("static": every bone key it can sample is the same pose, and no colour / alpha
// tracks) - to see how much animating changes nothing (chests, crates).
struct CensusEntry {
    std::string name;
    float span = 0;
    bool still = false;
    std::uint64_t calls = 0;
};
std::map<std::pair<const void*, std::int32_t>, CensusEntry> g_census;
std::chrono::steady_clock::time_point g_census_since{};

bool same_pose(const jo::AnimKey& a, const jo::AnimKey& b) {
    return std::memcmp(a.rot, b.rot, sizeof a.rot) == 0 && std::memcmp(a.pos, b.pos, sizeof a.pos) == 0;
}

// Every key the action [start, end] can interpolate between (those inside it and the one on each side) is one pose.
bool action_still(const jo::AnimDef& def, float start, float end) {
    if (def.colour_tracks != nullptr || def.alpha_tracks != nullptr) return false;
    for (std::int32_t i = 0; i < def.track_count; ++i) {
        const jo::Track& track = def.tracks[i];
        if (track.key_count <= 0 || track.keys == nullptr) continue;
        const auto* const keys = reinterpret_cast<const jo::AnimKey*>(track.keys);
        std::int32_t lo = 0, hi = track.key_count - 1;
        while (lo < hi && keys[lo + 1].time <= start) ++lo;
        while (hi > lo && keys[hi - 1].time >= end) --hi;
        for (std::int32_t k = lo + 1; k <= hi; ++k)
            if (!same_pose(keys[k], keys[lo])) return false;
    }
    return true;
}

[[maybe_unused]] void census(const void* def_ptr, std::int32_t anim, const std::uint8_t* frame) {
    const int every = joan::hook::settings().anim_census;
    if (every <= 0) return;
    const auto now = std::chrono::steady_clock::now();
    if (g_census_since == std::chrono::steady_clock::time_point{}) g_census_since = now;
    CensusEntry& e = g_census[{def_ptr, anim}];
    if (e.calls++ == 0) {
        const jo::AnimDef& def = *static_cast<const jo::AnimDef*>(def_ptr);
        const jo::AnimAction& action = *reinterpret_cast<const jo::AnimAction*>(frame);
        e.name = std::string(def.name, strnlen(def.name, sizeof def.name)) + " / " +
                 std::string(action.name, strnlen(action.name, sizeof action.name));
        e.span = action.end - action.start;
        e.still = action_still(def, action.start, action.end);
    }
    const double seconds = std::chrono::duration<double>(now - g_census_since).count();
    if (seconds < every) return;
    std::vector<const CensusEntry*> list;
    std::uint64_t total = 0, still = 0;
    for (const auto& [key, entry] : g_census) {
        total += entry.calls;
        if (entry.still) still += entry.calls;
        list.push_back(&entry);
    }
    std::sort(list.begin(), list.end(), [](const CensusEntry* a, const CensusEntry* b) { return a->calls > b->calls; });
    joan::hook::log_line("[Debug] AnimCensus (%.0f s): %.0f animate calls/s, %.0f/s (%.0f%%) of animations that move nothing; "
                         "%zu animations played. Most played (calls/s, length in the animation's time units):",
                         seconds, static_cast<double>(total) / seconds, static_cast<double>(still) / seconds,
                         total ? 100.0 * static_cast<double>(still) / static_cast<double>(total) : 0.0, list.size());
    for (std::size_t i = 0; i < list.size() && i < 25; ++i)
        joan::hook::log_line("    %8.0f/s  length %.4f  %s  %s", static_cast<double>(list[i]->calls) / seconds, list[i]->span,
                             list[i]->still ? "STATIC" : "moves ", list[i]->name.c_str());
    g_census.clear();
    g_census_since = now;
}

struct H {
    // 0x005b8910: key (+0 time, +4 quaternion, +0x14 position) -> bone matrix.
    void* key_to_matrix(void* bone) {
        if (anim_math::g_animating) {  // drawn poses only: native maths (see anim_math)
            anim_math::key_matrix(static_cast<float*>(bone), jo::self<jo::AnimKey>(this)->rot, jo::self<jo::AnimKey>(this)->pos);
            return bone;
        }
        joan::orig::quaternion_to_matrix(joan::orig::arg::mptr(bone), joan::orig::arg::mptr(jo::self<jo::AnimKey>(this)->rot));          // 0x0060c880 quaternion -> matrix
        import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, bone, jo::self<jo::AnimKey>(this)->pos);      // matrix_multiply_translation
        return bone;
    }

    // --- AnimDef (en3d/animdef.cpp) --------------------------------------------------------------
    // +0x00 type (1: load on demand from +0x6 / +0x107), +0x128 bone count, +0x140 bone tracks
    // (8 bytes each), +0x14f material count, +0x153 colour tracks, +0x157 alpha tracks.
    // 0x005b5e40(plane, anim, sub, t, looping, bone map (-999 skips), blend keys (32 bytes per
    // bone, from the start pose), blend time, key cache owner, key cache).
    void animate(void* plane, std::int32_t anim, std::uint32_t sub, float time, std::uint32_t looping,
                 const std::int32_t* bone_map, std::uint8_t* blend_keys, float blend_time, void** cache_owner,
                 std::int32_t* cache) {
        c_animate.hit();
        const anim_math::Scope native_maths;  // the key blends and bone matrices below are only drawn
        if (static_cast<std::int16_t>(sub) != 0) {
            void* const child = joan::orig::arg::as<void*>(joan::orig::as_anim_def(this)->entry(joan::orig::arg::i32(static_cast<std::uint32_t>(static_cast<std::uint16_t>(sub)))));  // 0x005b7650
            joan::orig::as_anim_def(child)->animate(joan::orig::arg::mptr(plane), joan::orig::arg::i32(anim), 0u, joan::orig::arg::f32(time), joan::orig::arg::u32(looping), joan::orig::arg::mptr(bone_map), joan::orig::arg::mptr(blend_keys), joan::orig::arg::f32(blend_time), joan::orig::arg::mptr(cache_owner), joan::orig::arg::mptr(cache));
            return;
        }
        if (jo::self<jo::AnimDef>(this)->loaded == 1) {
            char name_b[0x104], name_a[0x104];
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(name_b)), joan::orig::arg::str(jo::self<jo::AnimDef>(this)->name));  // strcpy
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(name_a)), joan::orig::arg::str(jo::self<jo::AnimDef>(this)->dir));
            joan::orig::as_anim_def(this)->load(joan::orig::arg::str(name_a), joan::orig::arg::str(name_b), 0u);  // 0x005b23d0 load
        }
        const std::uint8_t* const frame = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(this)->frame(joan::orig::arg::i32(anim), 0u));  // 0x005b5dd0
        const float start = jo::as_anim_action(frame)->start, end = jo::as_anim_action(frame)->end;
        const float span = end - start;
        if constexpr (!joan::kReleaseBuild) census(this, anim, frame);  // [Debug] AnimCensus
        std::int32_t first_cycle = 1;
        float t;
        if (looping == 0) {
            t = start + time;
            if (below_or_nan(t, start)) t = start;
            if (t > end) t = end;
        } else {
            first_cycle = below_or_nan(time, span) ? 1 : 0;
            const double r = joan::orig::arg::as<double>(joan::orig::libc::fmod(joan::orig::arg::f64(static_cast<double>(time)), joan::orig::arg::f64(static_cast<double>(span))));  // CRT fmod
            // fadd dword [start] on fmod's st0 at the precision in force, then fstp dword: at 24 bits one rounding
            // of the exact sum (a double sum then a float conversion would round twice).
            namespace x87 = joan::hook::x87;
            const x87::Fpu fpu{x87::current_precision() == x87::Precision::Single};
            t = static_cast<float>(fpu.add(r, static_cast<double>(start)));
        }
        std::uint8_t blend[0x20];
        joan::orig::as_anim_key(blend)->construct();  // 0x005aaee0
        void* k0 = nullptr;
        void* k1 = nullptr;
        for (std::int32_t i = 0; i < jo::self<jo::AnimDef>(this)->track_count; ++i) {
            std::int32_t b = i;
            if (bone_map != nullptr) {
                b = bone_map[i];
                if (b == -999) continue;
            }
            void* const track = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->tracks[i]);
            std::int32_t key = -1;
            if (cache_owner != nullptr && *cache_owner == this) key = cache[b];
            float weight = joan::orig::arg::as<float>(joan::orig::as_track(track)->sample(joan::orig::arg::f32(t), &k0, &k1, &key));  // 0x005b7c70
            if (cache_owner != nullptr) cache[b] = key;
            if (blend_keys != nullptr && first_cycle != 0) {
                std::int32_t key2 = key;
                void* b0 = nullptr;
                void* b1 = nullptr;
                if (span > blend_time) {
                    const float w = joan::orig::arg::as<float>(joan::orig::as_track(track)->sample(joan::orig::arg::f32(start + blend_time), &b0, &b1, &key2));
                    if (!(w > 0.0f)) b1 = b0;
                } else {
                    const float w = joan::orig::arg::as<float>(joan::orig::as_track(track)->sample(joan::orig::arg::f32(end), &b0, &b1, &key2));
                    if (below_or_nan(w, 0.25f)) b1 = b0;  // 0x9d525c
                }
                if (below_or_nan(t, jo::view<float>(jo::as_key_frame(b1)->time))) {
                    k0 = blend_keys + (b << 5);
                    jo::view<float>(jo::as_key_frame(k0)->time) = start;
                    k1 = b1;
                    weight = (t - start) / (jo::view<float>(jo::as_key_frame(k1)->time) - start);
                }
            }
            if (k1 != nullptr) {
                joan::orig::as_anim_key(blend)->interpolate(joan::orig::arg::mptr(k0), joan::orig::arg::mptr(k1), joan::orig::arg::f32(weight));  // 0x005b8840 interpolate
                void* const bone = joan::orig::arg::as<void*>(joan::orig::as_plane(plane)->bone_5cf8f0(joan::orig::arg::i32(b)));  // 0x005cf8f0
                joan::orig::as_anim_key(blend)->to_matrix(joan::orig::arg::mptr(bone));                   // 0x005b8910
                joan::orig::as_bone(bone)->invalidate();                                 // 0x0060ee10
            } else if (k0 != nullptr) {
                void* const bone = joan::orig::arg::as<void*>(joan::orig::as_plane(plane)->bone_5cf8f0(joan::orig::arg::i32(b)));
                joan::orig::as_anim_key(k0)->to_matrix(joan::orig::arg::mptr(bone));
                joan::orig::as_bone(bone)->invalidate();
            }
        }
        if (cache_owner != nullptr) *cache_owner = this;
        if (jo::view<void*>(jo::self<jo::AnimDef>(this)->colour_tracks) == nullptr && jo::view<void*>(jo::self<jo::AnimDef>(this)->alpha_tracks) == nullptr) return;
        for (std::int32_t m = 0; m < jo::self<jo::AnimDef>(this)->extra_count; ++m) {
            std::uint8_t* const material = jo::view<std::uint8_t*>(jo::as_plane(plane)->materials) + m * static_cast<std::int32_t>(sizeof(jo::Material));
            std::uint8_t colour_obj[8];
            std::uint8_t c[4];  // B, G, R, A
            void* const white = joan::orig::as_colour(colour_obj)->construct(0xffu, 0xffu, 0xffu, 0xffu);  // 0x0042a260
            joan::orig::as_colour(c)->from_rgba(joan::orig::arg::mptr(white));                                                   // 0x005e95b0
            if (jo::view<void*>(jo::self<jo::AnimDef>(this)->colour_tracks) != nullptr) {
                std::uint8_t* const tr = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->colour_tracks[m]);
                if (jo::view<std::uint32_t>(jo::as_colour_track(tr)->count) != 0 && jo::as_colour_track(tr)->keys != nullptr) {
                    float* c0 = nullptr;
                    float* c1 = nullptr;
                    const float w = joan::orig::arg::as<float>(joan::orig::as_colour_track(tr)->sample(joan::orig::arg::f32(t), &c0, &c1));  // 0x005b8b40
                    if (c1 != nullptr) {
                        float rgb[4];
                        joan::orig::as_colour_key(rgb)->interpolate(joan::orig::arg::mptr(c0), joan::orig::arg::mptr(c1), joan::orig::arg::f32(w));  // 0x005b8960
                        c[2] = to_byte(rgb[1]);
                        c[0] = to_byte(rgb[3]);
                        c[1] = to_byte(rgb[2]);
                    } else if (c0 != nullptr) {
                        c[2] = to_byte(c0[1]);
                        c[0] = to_byte(c0[3]);
                        c[1] = to_byte(c0[2]);
                    }
                }
            }
            if (jo::view<void*>(jo::self<jo::AnimDef>(this)->alpha_tracks) != nullptr) {
                std::uint8_t* const tr = reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->alpha_tracks[m]);
                if (jo::view<std::uint32_t>(jo::as_alpha_track(tr)->count) != 0 && jo::as_alpha_track(tr)->keys != nullptr) {
                    float* a0 = nullptr;
                    float* a1 = nullptr;
                    const float w = joan::orig::arg::as<float>(joan::orig::as_alpha_track(tr)->sample(joan::orig::arg::f32(t), &a0, &a1));  // 0x005b91c0
                    if (a1 != nullptr) {
                        float alpha[2];
                        joan::orig::as_alpha_key(alpha)->interpolate(joan::orig::arg::mptr(a0), joan::orig::arg::mptr(a1), joan::orig::arg::f32(w));  // 0x005b9020
                        c[3] = to_byte(alpha[1]);
                    } else if (a0 != nullptr) {
                        c[3] = to_byte(a0[1]);
                    }
                    jo::as_material(material)->pass_mask = 2;
                    jo::as_material(material)->raster = (jo::as_material(material)->raster & 0xfffff0ffu) | 0x100u;
                    switch (jo::as_material(material)->blend) {
                        case 0: jo::as_material(material)->blend = 1; break;
                        case 5: jo::as_material(material)->blend = 6; break;
                        case 8: jo::as_material(material)->blend = 9; break;
                        default: break;
                    }
                }
            }
            std::memcpy(&jo::as_material(material)->colour, c, 4);
        }
        joan::orig::as_plane(plane)->set_104(1u);  // 0x005d6e80
        joan::orig::as_plane(plane)->invalidate(1u);   // 0x005cf920
    }
};

#define HELPER13(name, addr, fn) JOAN_REPLACE("en3d/helpers13", name, addr, fn)
HELPER13("AnimKey::to_matrix", 0x005b8910, &H::key_to_matrix);
HELPER13("AnimationDef::update_plane", 0x005b5e40, &H::animate);
#undef HELPER13

}  // namespace en3d_helpers13
}  // namespace

// ---------------------------------------- from modules/game_b104_09.cpp (game/b104_09)
//
//    - AnimDef::load_bone_keys 0x005b6dc0 (bone), ret 4: re-reads <dir><name>.AM2 and inserts
//      the keys whose time is in [bone+0x38, bone+0x3c] (0x005b7bf0 / 0x005b8770), then the
//      original's (odd, 4-byte) memcpy / memmove reorder; bone+0x40 = 1. Dead code.
namespace {
namespace game_b104_09 {

using i8 = std::int8_t;


struct alignas(4) Path {
    u8 bytes[0x194];
};
struct alignas(4) File {
    u8 bytes[0x210];  // 0x110 used (0x004179c0); room as in the larger frames
};

JOAN_COUNTER(c_load_bone_keys, "AnimDef::load_bone_keys");

struct H {
    // 0x005b6dc0 AnimDef::load_bone_keys(bone)
    void load_bone_keys(i32 index) {
        c_load_bone_keys.hit();
        jo::AnimAction* const bone = reinterpret_cast<jo::AnimAction*>(jo::view<u8*>(jo::self<jo::AnimDef>(this)->actions) + static_cast<u32>(index) * sizeof(jo::AnimAction));
        if (jo::view<i8>(bone->keys_loaded) != 0) return;
        File file;
        joan::orig::as_file(&file)->construct();  // 0x004179c0
        void* const fp = &file;
        Path path;
        joan::orig::as_string(&path)->construct(joan::orig::arg::str(jo::self<jo::AnimDef>(this)->dir));  // 0x005a11a0
        joan::orig::as_string(&path)->append(joan::orig::arg::str(jo::self<jo::AnimDef>(this)->name));  // 0x005a1550 append
        joan::orig::as_string(&path)->append(str(d::s_AM2_9cf5bc));                  // ".AM2"
        const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
        joan::orig::as_file(&file)->file_open(joan::orig::arg::str(name), 1, static_cast<char>(0));  // 0x00586da0 open
        for (i32 k = 0; k < 4; ++k) joan::orig::as_file(fp)->file_seek(4, 1);       // 0x00587ea0 seek
        joan::orig::as_file(fp)->file_seek(joan::orig::arg::i32(jo::self<jo::AnimDef>(this)->action_count * 0x3e), 1);
        u32 time = 0;        // [ebp-0x2d8]
        u32 raw[12] = {};    // [ebp-0x308]
        for (i32 i = 0; i < jo::self<jo::AnimDef>(this)->track_count; ++i) {
            u8* const track = reinterpret_cast<u8*>(&jo::self<jo::AnimDef>(this)->tracks[i]);
            i32 added = 0;
            i32 first = -1;
            const i32 count = joan::orig::as_file(fp)->file_get_long();   // 0x00587e20
            i32 j;
            for (j = 0; j < count; ++j) {
                joan::orig::as_file(fp)->file_read(static_cast<void*>(&time), 4u);  // 0x005873a0
                joan::orig::as_file(fp)->file_read(static_cast<void*>(raw), 0x30u);
                float t;
                std::memcpy(&t, &time, 4);
                if (!(t >= bone->start)) continue;  // C0: below or unordered
                if (t > bone->end) {
                    joan::orig::as_file(fp)->file_seek(joan::orig::arg::i32((count - added - 1) * 0x34), 1);
                    break;
                }
                ++added;
                const i32 slot = joan::orig::arg::as<i32>(joan::orig::as_track(track)->allocate(1));  // 0x005b7bf0
                u8* const key = jo::view<u8*>(jo::as_track(track)->keys) + (static_cast<u32>(slot) << 5);
                if (first == -1) first = slot;
                u32 m[16];
                std::memcpy(m, img::obj(d::g_identity_matrix), sizeof(m));
                m[0] = raw[0];
                m[4] = raw[1];
                m[8] = raw[2];
                m[12] = raw[3];
                m[1] = raw[4];
                m[5] = raw[5];
                m[9] = raw[6];
                m[13] = raw[7];
                m[2] = raw[8];
                m[6] = raw[9];
                m[10] = raw[10];
                m[14] = raw[11];
                joan::orig::as_key(static_cast<void*>(key))->set(joan::orig::arg::u32(time), joan::orig::arg::mptr(static_cast<void*>(m)));  // 0x005b8770
            }
            if (first == -1) continue;
            bool found = false;
            u8* const first_key = jo::view<u8*>(jo::as_track(track)->keys) + (static_cast<u32>(first) << 5);
            for (j = 0; j < jo::as_track(track)->key_count; ++j) {
                u8* const key = jo::view<u8*>(jo::as_track(track)->keys) + (static_cast<u32>(j) << 5);
                if (!(j < jo::as_track(track)->key_count - added)) break;
                if (jo::view<float>(jo::as_key_frame(key)->time) > jo::view<float>(jo::as_key_frame(first_key)->time)) {
                    found = true;
                    break;
                }
            }
            if (!found) continue;
            const i32 n = added;
            void* mem = joan::crt::operator_new(joan::orig::arg::u32(static_cast<u32>(n) << 5));  // operator new
            void* tmp;
            if (mem != nullptr) {
                joan::orig::vector(joan::orig::arg::mptr(mem), 0x20, joan::orig::arg::i32(n), joan::orig::arg::mptr(reinterpret_cast<void*>(fn::code_value(fn::AnimKey_AnimKey))));  // 0x00449af0
                tmp = mem;
            } else {
                tmp = nullptr;
            }
            std::memcpy(joan::orig::arg::mptr(tmp), static_cast<const void*>(first_key), 4u);  // memcpy
            {
                u8* const src = jo::view<u8*>(jo::as_track(track)->keys) + (static_cast<u32>(j) << 5);
                const u32 len = static_cast<u32>(first_key - src);  // pointer difference narrowed
                joan::orig::libc::memcpy(joan::orig::arg::mptr(static_cast<void*>(first_key)), joan::orig::arg::mptr(static_cast<const void*>(jo::view<u8*>(jo::as_track(track)->keys) + (static_cast<u32>(j) << 5))), joan::orig::arg::u32(len));  // memmove
            }
            std::memcpy(static_cast<void*>(jo::view<u8*>(jo::as_track(track)->keys) + (static_cast<u32>(j) << 5)), static_cast<const void*>(tmp), 4u);  // memcpy
            joan::crt::operator_delete(joan::orig::arg::mptr(tmp));                         // operator delete
        }
        joan::orig::as_file(fp)->file_close();  // 0x00587260 close
        bone->keys_loaded = 1;
        joan::orig::as_file(&file)->destruct();  // 0x005872b0 ~File
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_09", name, addr, fn)
B("AnimDef::load_bone_keys", 0x005b6dc0, &H::load_bone_keys);
#undef B

}  // namespace game_b104_09
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
//
//      0x005b74d0 Table::set_entry(i, v, c)      (thiscall, ret 0xc) - rec (i-1)*0x41: +0x34 / +0x33;
//                 eax = (this & ~0xff) | c (mov al).
//
//      0x005b9090 Pair::Pair                      (thiscall, ret 0) - [4] = [0] = 0 (vector ctor).
//
//    - neither caller reads eax, both replaced as void. 0x005b7b40 / 0x005b8a10 / 0x005b9090 /
namespace {
namespace game_b103_11 {


struct H {
    // 0x005b74d0
    u32 set_entry(i32 i, u32 v, u8 c) {
        jo::self<jo::AnimDef>(this)->actions[i - 1].value = v;
        jo::self<jo::AnimDef>(this)->actions[i - 1].flag = c;
        return (joan::call::register_low_bits(this) & 0xffffff00u) | c;  // mov al, c on eax = this
    }

    // 0x005b7b40 / 0x005b8a10
    H* pair_ctor() {
        jo::view<u32>(jo::self<jo::Track>(this)->key_count) = 0;
        jo::self<jo::Track>(this)->keys = nullptr;
        return this;
    }
    // 0x005b9090
    H* pair_ctor_rev() {
        jo::self<jo::AlphaTrack>(this)->keys = nullptr;
        jo::view<u32>(jo::self<jo::AlphaTrack>(this)->count) = 0;
        return this;
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Table::set_entry", 0x005b74d0, &H::set_entry);
B103("Pair::Pair_5b7b40", 0x005b7b40, &H::pair_ctor);
B103("Pair::Pair_5b8a10", 0x005b8a10, &H::pair_ctor);
B103("Pair::Pair_5b9090", 0x005b9090, &H::pair_ctor_rev);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {


struct H {
    joan::orig::AnimDef* anim_entry(std::int32_t index) {                             // 0x005b7650 (short index)
        const std::int16_t i = static_cast<std::int16_t>(index);
        // Through an integer, as the original: callers call into the result unchecked, and a plain null here would
        // let the optimizer drop the range check (x86 object identity).
        std::uintptr_t r = 0;
        if (i >= 1 && i <= jo::self<jo::AnimDef>(this)->child_count) r = reinterpret_cast<std::uintptr_t>(jo::self<jo::AnimDef>(this)->children[i - 1]);
        return reinterpret_cast<joan::orig::AnimDef*>(r);
    }
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("AnimationDef::get_child", 0x005b7650, &H::anim_entry);
#undef HELPER6

}  // namespace en3d_helpers6
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


JOAN_COUNTER(c_update_parts, "Model::update_parts");

struct H {
    void update_parts() {  // 0x005b77b0
        c_update_parts.hit();
        for (std::int32_t i = 0; i < jo::self<jo::AnimDef>(this)->track_count; ++i) {
            joan::orig::as_track(reinterpret_cast<std::uint8_t*>(&jo::self<jo::AnimDef>(this)->tracks[i]))->compress();  // 0x005b83a0
        }
    }
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("AnimationDef::compress", 0x005b77b0, &H::update_parts);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
//
//      0x005b7b80 AnimDef::free         (thiscall, ret 0) - 0x0058ed70 free (animdef.cpp).
namespace {
namespace game_b93_03 {


void* const kMem = obj(d::g_mem);

struct H {
    void animdef_free() {  // 0x005b7b80
        if (jo::self<jo::Track>(this)->keys != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_a16e88)) + 3;
            joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Track>(this)->keys)), str(d::s_d_ww_en3d_animdef_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::Track>(this)->keys = nullptr;
            jo::view<u32>(jo::self<jo::Track>(this)->key_count) = 0;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("AnimDef::free", 0x005b7b80, &H::animdef_free);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/en3d_helpers14.cpp (en3d/helpers14)
namespace {
namespace en3d_helpers14 {


JOAN_COUNTER(c_sample, "Track::sample");
JOAN_COUNTER(c_sample_colour, "ColourTrack::sample");
JOAN_COUNTER(c_sample_alpha, "AlphaTrack::sample");

// --- key track sampler (0x005b7c70 / 0x005b8b40 / 0x005b91c0) ---------------------------------
// Track: +0 key count, +4 keys (Stride bytes each, time float at +0). Finds the key pair around
// t: *k0 = key at or before t, *k1 = the next key (null when t is outside the keys or on the
// last one), returns the blend weight between them (0 when *k1 is null). The bone-key version
// first tries a window of 4 keys around the cached index; all three then interpolate a guess,
// refine it with the local slope and fall back to bisection.
template <class Key, bool Cached>
float sample_track(std::int32_t n, Key* typed_keys, float t, void** k0, void** k1, std::int32_t* cache_arg) {
    std::int32_t local_cache = -1;
    std::int32_t* const cache = (Cached && cache_arg != nullptr) ? cache_arg : &local_cache;
    constexpr std::int32_t Stride = static_cast<std::int32_t>(sizeof(Key));
    std::uint8_t* const keys = reinterpret_cast<std::uint8_t*>(typed_keys);
    auto key = [&](std::int32_t i) -> void* { return keys + i * Stride; };
    auto kt = [&](std::int32_t i) { return jo::view<float>(reinterpret_cast<Key*>(keys + i * Stride)->time); };

    if (n <= 1) {
        *k0 = n > 0 ? keys : nullptr;
        if (Cached) *cache = 0;
        *k1 = nullptr;
        return 0.0f;
    }
    std::int32_t lo = 0, hi = n - 1;
    if (Cached && *cache >= 0 && *cache < n) {
        bool found = false;
        const std::int32_t c = *cache;
        if (!(kt(c) > t)) {
            lo = c;
            std::int32_t c4 = c + 4;
            if (c4 >= n) c4 = n - 1;
            if (!(t > kt(c4))) {
                hi = c4;
                found = true;
            }
        } else {
            hi = c;
            std::int32_t c4 = c - 4;
            if (c4 < 0) c4 = 0;
            if (!(kt(c4) > t)) {
                lo = c4;
                found = true;
            }
        }
        if (found) {
            float next = 0.0f;
            while (lo < hi) {
                next = kt(lo + 1);
                if (next >= t) break;
                ++lo;
            }
            *k0 = key(lo);
            *cache = lo;
            if (lo < hi) {
                *k1 = key(lo + 1);
                const float a = kt(lo);
                return (t - a) / (next - a);
            }
            *k1 = nullptr;
            return 0.0f;
        }
    }

    float t_lo = kt(lo), t_hi = kt(hi);
    if (!(t > t_lo)) {
        *k0 = key(lo);
        if (Cached) *cache = lo;
        *k1 = nullptr;
        return 0.0f;
    }
    if (t >= t_hi) {
        *k0 = key(hi);
        if (Cached) *cache = hi;
        *k1 = nullptr;
        return 0.0f;
    }
    float a = 0.0f, b = 0.0f, result = 0.0f;
    // Three-way test of key pair (m, m+1): narrows [lo, hi] or finds t inside the pair.
    auto probe = [&](std::int32_t m) -> bool {
        a = kt(m);
        b = kt(m + 1);
        if (!(t >= a)) {
            hi = m;
            t_hi = a;
        } else if (!(t > b)) {
            *k0 = key(m);
            if (Cached) *cache = m;
            *k1 = key(m + 1);
            result = (t - a) / (b - a);
            return true;
        } else {
            lo = m + 1;
            t_lo = b;
        }
        return false;
    };
    while (hi - lo > 1) {
        const float guess = (t - t_lo) / (t_hi - t_lo) * static_cast<float>(hi - lo) + static_cast<float>(lo);
        std::int32_t m = joan::hook::x87::ftol(guess);
        if (m >= hi) m = hi - 1;
        if (probe(m)) return result;
        const float guess2 = (t - a) / (b - a) + static_cast<float>(m);
        if (!(static_cast<double>(lo) > guess2) && static_cast<double>(hi) > guess2) {
            m = joan::hook::x87::ftol(guess2);
            if (probe(m)) return result;
        }
        m = joan::hook::x87::ftol(static_cast<double>(lo + hi) / 2.0);  // 0x9d0670
        if (m >= hi) m = hi - 1;
        if (probe(m)) return result;
    }
    *k0 = key(lo);
    if (Cached) *cache = lo;
    if (hi == lo) {
        *k1 = nullptr;
        return 0.0f;
    }
    *k1 = key(hi);
    return (t - t_lo) / (t_hi - t_lo);
}

struct H {
    // --- en3d/helpers14 ---------------------------------------------------------------------
    float track_sample(float t, void** k0, void** k1, std::int32_t* cache) {  // 0x005b7c70, 32-byte keys
        c_sample.hit();
        return sample_track<jo::KeyFrame, true>(jo::self<jo::Track>(this)->key_count, jo::self<jo::Track>(this)->keys, t, k0, k1, cache);
    }
    float colour_sample(float t, void** k0, void** k1) {  // 0x005b8b40, 16-byte keys
        c_sample_colour.hit();
        return sample_track<jo::ColourKey, false>(jo::self<jo::ColourTrack>(this)->count, jo::self<jo::ColourTrack>(this)->keys, t, k0, k1, nullptr);
    }
    float alpha_sample(float t, void** k0, void** k1) {  // 0x005b91c0, 8-byte keys
        c_sample_alpha.hit();
        return sample_track<jo::AlphaKey, false>(jo::self<jo::AlphaTrack>(this)->count, jo::self<jo::AlphaTrack>(this)->keys, t, k0, k1, nullptr);
    }
    // 0x005b8840: bone key = time, quaternion (+4, slerped by 0x0060c8b0), position (+0x14).
    void key_interpolate(const float* k0, const float* k1, float w) {
        float* const out = reinterpret_cast<float*>(this);
        const float w1 = 1.0f - w;  // 0x9cf784
        out[0] = w1 * k0[0] + w * k1[0];
        if (anim_math::g_animating) anim_math::slerp(out + 1, k0 + 1, k1 + 1, w);  // drawn poses only (see anim_math)
        else joan::orig::quaternion_slerp(joan::orig::arg::mptr(out + 1), joan::orig::arg::mptr(k0 + 1), joan::orig::arg::mptr(k1 + 1), joan::orig::arg::f32(w));
        out[5] = w1 * k0[5] + w * k1[5];
        out[6] = w1 * k0[6] + w * k1[6];
        out[7] = w1 * k0[7] + w * k1[7];
    }
    void colour_interpolate(const float* k0, const float* k1, float w) {  // 0x005b8960
        float* const out = reinterpret_cast<float*>(this);
        const float w1 = 1.0f - w;
        for (int i = 0; i < 4; ++i) out[i] = w1 * k0[i] + w * k1[i];
    }
    void alpha_interpolate(const float* k0, const float* k1, float w) {  // 0x005b9020
        float* const out = reinterpret_cast<float*>(this);
        const float w1 = 1.0f - w;
        for (int i = 0; i < 2; ++i) out[i] = w1 * k0[i] + w * k1[i];
    }
};

#define HELPER14(name, addr, fn) JOAN_REPLACE("en3d/helpers14", name, addr, fn)
HELPER14("Track::sample", 0x005b7c70, &H::track_sample);
HELPER14("ColourTrack::sample", 0x005b8b40, &H::colour_sample);
HELPER14("AlphaTrack::sample", 0x005b91c0, &H::alpha_sample);
HELPER14("AnimKey::interpolate", 0x005b8840, &H::key_interpolate);
HELPER14("ColourKey::interpolate", 0x005b8960, &H::colour_interpolate);
HELPER14("AlphaKey::interpolate", 0x005b9020, &H::alpha_interpolate);
#undef HELPER14

}  // namespace en3d_helpers14
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
//
//    - AnimDef::free 0x005b8a50 (ret 0): heap free of +4 ("animdef.cpp"), +4 = +0 = 0.
//
//  - Return values: callers of 0x5acfa0, 0x5b8a50, 0x5c80d0, 0x5e7ce0, 0x601140, 0x616dd0,
//    0x623bd0 ignore eax (checked); 0x5d5fe0's callers use al only.
namespace {
namespace game_b100_11 {


i32 line_at(img::Addr a, i32 add_) { return static_cast<i32>(global<i16>(a)) + add_; }
void heap_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

struct H {
    // ------------------------------------------------------------------ 0x005b8a50
    void animdef_free() {
        if (jo::view<void*>(jo::self<jo::ColourTrack>(this)->keys) != nullptr) {
            heap_free(jo::view<void*>(jo::self<jo::ColourTrack>(this)->keys), str(d::s_d_ww_en3d_animdef_cpp), line_at(d::g_a16e94, 3));  // "animdef.cpp"
            jo::self<jo::ColourTrack>(this)->keys = nullptr;
            jo::view<u32>(jo::self<jo::ColourTrack>(this)->count) = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("AnimDef::free", 0x005b8a50, &H::animdef_free);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/en3d_helpers37.cpp (en3d/helpers37)
//
//      0x005b8ac0 KeyList16::allocate(n)         - grows the 16-byte entry array, returns old count
//
//      0x005b9140 KeyList8::allocate(n)          - same with 8-byte entries
//
//      0x005b9660 EventList::EventList()         - +0 / +4 = 0
//
//      0x005b9710 EventList::allocate(n)         - same with 0x3c-byte entries
namespace {
namespace en3d_helpers37 {


constexpr img::Addr kMemory(d::g_mem);

// Shared body of the animdef.cpp list growers: data = realloc(data, (count + n) * size), count += n.
template <class Elem>
std::int32_t list_allocate(std::int32_t& count, Elem*& data, std::int32_t n, img::Addr line_global) {
    const std::int32_t line = global<std::int16_t>(line_global) + 1;
    constexpr std::int32_t bytes = static_cast<std::int32_t>(sizeof(Elem));
    data = static_cast<Elem*>(joan::orig::arg::as<void*>(joan::orig::as_mem(img::obj(kMemory))->resize(joan::orig::arg::mptr(data), joan::orig::arg::u32((count + n) * bytes), joan::orig::arg::str(img::str(kAnimdefFile)), joan::orig::arg::i32(line))));  // 0x0058eb30
    const std::int32_t first = count;
    count = count + n;
    return first;
}

struct H {
    // 0x005b8ac0 / 0x005b9140 / 0x005b9710 list allocate(n): returns the first new index.
    std::int32_t list16_allocate(std::int32_t n) { return list_allocate(jo::self<jo::ColourTrack>(this)->count, jo::self<jo::ColourTrack>(this)->keys, n, d::g_a16e98); }
    std::int32_t list8_allocate(std::int32_t n) { return list_allocate(jo::self<jo::AlphaTrack>(this)->count, jo::self<jo::AlphaTrack>(this)->keys, n, d::g_a16ea0); }
    std::int32_t events_allocate(std::int32_t n) { return list_allocate(jo::self<jo::EventList>(this)->count, jo::self<jo::EventList>(this)->events, n, d::g_a16ea8); }

    // 0x005b9660 EventList::EventList().
    void* events_ctor() {
        jo::self<jo::EventList>(this)->events = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::EventList>(this)->count) = 0;
        return this;
    }
};

#define HELPER37(name, addr, fn) JOAN_REPLACE("en3d/helpers37", name, addr, fn)
HELPER37("KeyList16::allocate", 0x005b8ac0, &H::list16_allocate);
HELPER37("KeyList8::allocate", 0x005b9140, &H::list8_allocate);
HELPER37("EventList::EventList", 0x005b9660, &H::events_ctor);
HELPER37("EventList::allocate", 0x005b9710, &H::events_allocate);
#undef HELPER37

}  // namespace en3d_helpers37
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
//
//      0x005b90d0 AnimArr::free                     (thiscall, ret 0) - debug free of +4.
namespace {
namespace game_b96_06 {


void heap_free(void* p, img::Addr file, i32 line) {
    joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}

struct H {
    // ------------------------------------------------------------------ 0x005b90d0
    void anim_free() {
        if (jo::self<jo::AlphaTrack>(this)->keys == nullptr) return;
        heap_free(jo::view<void*>(jo::self<jo::AlphaTrack>(this)->keys), d::s_d_ww_en3d_animdef_cpp, static_cast<i32>(global<i16>(d::g_a16e9c)) + 3);
        jo::self<jo::AlphaTrack>(this)->keys = nullptr;
        jo::view<u32>(jo::self<jo::AlphaTrack>(this)->count) = 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("AnimArr::free", 0x005b90d0, &H::anim_free);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
//
//    - AnimDef::free 0x005b96a0 (ret 0): heap free (0x58ed70 on 0xa48838, "animdef.cpp",
//      line [0xa16ea4]+3) of +4, then +4 = +0 = 0.
//
//  - Void functions whose eax is not meaningful (0x5b96a0, 0x5bfa70 - its only caller
//    0x5bfbd0 ignores eax, 0x612dd0, 0x618090, 0x62e9a0, 0x603570 - _initterm entry) return
//    void. 0x62b510 is tail-called by 0x62b5e0 (which returns its eax), so it returns this /
//    the last 0x60a740 result exactly.
namespace {
namespace game_b98_06 {


struct H {
    // ------------------------------------------------------------------ 0x005b96a0
    void animdef_free() {
        if (jo::view<void*>(jo::self<jo::EventList>(this)->events) != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_a16ea4)) + 3;
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::EventList>(this)->events)), str(d::s_d_ww_en3d_animdef_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::EventList>(this)->events = nullptr;
            jo::view<u32>(jo::self<jo::EventList>(this)->count) = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("AnimDef::free", 0x005b96a0, &H::animdef_free);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/en3d_helpers28.cpp (en3d/helpers28)
//
//  * module en3d/helpers28: VsCache::fragments (0x00618e40) - the list of nvlinker fragment
//    IDs that make up the vertex shader for a cache entry (format, lights, colour sources,
//    fog mode) - TextureManager::load (0x006099c0) and AnimDef::load (0x005b23d0).
namespace {
namespace en3d_helpers28 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::stdcall_call;

JOAN_COUNTER(c_anim_load, "AnimDef::load");

struct H {
    // 0x005b23d0 AnimDef::load(directory, name, header_only): waits for the streamer
    // (0x005b4140 / 0x005b3100), then reads <dir><name>.AM2 (header only: 0x005b5830), or the
    // cached .AMT (+ .AM3 if newer) when 0xa6768c is set and up to date, else the full .AM2
    // through 0x005b4b90 with the optional bake / .AMT cache write (0xa67690 / 0xa67688).
    void animdef_load(const char* directory, const char* anim_name, std::uint32_t header_only) {
        c_anim_load.hit();
        if (jo::self<jo::AnimDef>(this)->loaded == 2) return;
        if (jo::self<jo::AnimDef>(this)->loaded == 1 && static_cast<std::int8_t>(header_only) != 0) return;
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_reader(this)->done()) == 0) {  // 0x005b4140
            stdcall_call<std::uint32_t>(global<std::uintptr_t>(d::iat_KERNEL32_SleepEx), 5u, 1u);  // SleepEx
            joan::orig::as_anim_def(this)->stream_update();                                                // 0x005b3100
        }
        struct alignas(4) Path {
            std::uint8_t bytes[0x194];
        };
        struct alignas(alignof(void*)) File {  // File 0x10e on x86, 0x112 on x64
            std::uint8_t bytes[((sizeof(joan::orig::File) > 0x110 ? sizeof(joan::orig::File) : 0x110) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
        };
        struct alignas(alignof(void*)) Reader {  // FileMem 0x11c on x86, 0x12c on x64
            std::uint8_t bytes[((sizeof(joan::orig::FileMem) > 0x11c ? sizeof(joan::orig::FileMem) : 0x11c) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
        };
        auto make_path = [&](Path& p, img::Addr extension) {
            joan::orig::as_string(&p)->construct(joan::orig::arg::str(directory));                // 0x005a11a0
            joan::orig::as_string(&p)->append(joan::orig::arg::str(anim_name));                          // 0x005a1550 append
            joan::orig::as_string(&p)->append(joan::orig::arg::str(&img::global<const char>(extension)));
        };
        auto c_str = [](Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); };  // 0x00417d90
        void* const files = img::obj(d::g_misc);
        Path am2, amt, am3;
        make_path(am2, d::s_AM2_9cf5bc);  // ".AM2"
        make_path(amt, d::s_AMT);  // ".AMT"
        make_path(am3, d::s_AM3);  // ".AM3"
        if (joan::orig::as_misc(files)->is_file_exist(joan::orig::arg::str(c_str(am2))) == 0) {  // 0x00590b80 exists
            joan::orig::error::fatal(joan::orig::arg::mptr(img::obj(d::g_err)), joan::orig::arg::str(&img::global<const char>(d::s_Error_loading_Animation_1s)), c_str(am2));
            return;
        }
        char* const self_name = jo::self<jo::AnimDef>(this)->name;
        if (joan::crt::stricmp(joan::orig::arg::str(self_name), joan::orig::arg::str(&img::global<const char>(d::kEmpty))) == 0 &&
            joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(anim_name))) < 0x20)
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(self_name)), joan::orig::arg::str(anim_name));
        char* const self_dir = jo::self<jo::AnimDef>(this)->dir;
        if (joan::crt::stricmp(joan::orig::arg::str(self_dir), joan::orig::arg::str(&img::global<const char>(d::kEmpty))) == 0 &&
            joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(directory))) < 0x100)
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(self_dir)), joan::orig::arg::str(directory));
        if (static_cast<std::int8_t>(header_only) != 0) {
            File file;
            joan::orig::as_file(&file)->construct();
            joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(am2)), 1, static_cast<char>(0));
            Reader reader;
            joan::orig::as_file_mem(&reader)->construct();
            joan::orig::as_file_mem(&reader)->open_file(joan::orig::arg::file(&file));
            joan::orig::as_anim_def(this)->read_header(&reader);  // 0x005b5830 header
            joan::orig::as_file_mem(&reader)->close();
            joan::orig::as_file(&file)->file_close();
            joan::orig::as_anim_def(this)->build_sort_index();  // 0x005b5a70
            jo::view<std::uint8_t>(jo::self<jo::AnimDef>(this)->loaded) = 1;
            joan::orig::as_file_mem(&reader)->destruct();  // 0x00587fd0 ~Reader
            joan::orig::as_file(&file)->destruct();  // 0x005872b0 ~File
            return;
        }
        bool use_cache = false;
        if (global<std::uint32_t>(d::g_a6768c) != 0 && joan::orig::as_misc(files)->is_file_exist(joan::orig::arg::str(c_str(amt))) != 0 &&
            joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(files)->compare_file_time_590e20(joan::orig::arg::str(c_str(amt)), joan::orig::arg::str(c_str(am2)))) > 0) {  // 0x00590e20 newer
            use_cache = joan::orig::as_misc(files)->is_file_exist(joan::orig::arg::str(c_str(am3))) == 0 ||
                        joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(files)->compare_file_time_590e20(joan::orig::arg::str(c_str(amt)), joan::orig::arg::str(c_str(am3)))) > 0;
        }
        if (use_cache) {
            File file;
            joan::orig::as_file(&file)->construct();
            joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(am2)), 1, static_cast<char>(0));
            Reader reader;
            joan::orig::as_file_mem(&reader)->construct();
            joan::orig::as_file_mem(&reader)->open_file(joan::orig::arg::file(&file));
            joan::orig::as_anim_def(this)->read_header(&reader);
            joan::orig::as_file_mem(&reader)->close();
            joan::orig::as_file(&file)->file_close();
            joan::orig::as_anim_def(this)->build_sort_index();
            jo::view<std::uint8_t>(jo::self<jo::AnimDef>(this)->loaded) = 1;
            File cache_file;
            joan::orig::as_file(&cache_file)->construct();
            joan::orig::as_file(&cache_file)->file_open(joan::orig::arg::str(c_str(amt)), 1, static_cast<char>(0));
            Reader cache_reader;
            joan::orig::as_file_mem(&cache_reader)->construct();
            joan::orig::as_file_mem(&cache_reader)->open_file(joan::orig::arg::file(&cache_file));
            joan::orig::as_anim_def(this)->read_cache(&cache_reader);  // 0x005b7930 read cache
            joan::orig::as_file_mem(&cache_reader)->close();
            joan::orig::as_file(&cache_file)->file_close();
            jo::view<std::uint8_t>(jo::self<jo::AnimDef>(this)->loaded) = 2;
            joan::orig::as_anim_def(this)->load_am3(joan::orig::arg::str(directory), joan::orig::arg::str(anim_name));  // 0x005b6800
            joan::orig::as_file_mem(&cache_reader)->destruct();
            joan::orig::as_file(&cache_file)->destruct();
            joan::orig::as_file_mem(&reader)->destruct();
            joan::orig::as_file(&file)->destruct();
            return;
        }
        Reader reader;
        joan::orig::as_file_mem(&reader)->construct();
        joan::orig::as_file_mem(&reader)->open_588000(joan::orig::arg::str(c_str(am2)), 1, static_cast<char>(joan::orig::arg::i32(0)));  // 0x00588000 open
        joan::orig::as_anim_def(this)->read(&reader);                           // 0x005b4b90 read
        joan::orig::as_file_mem(&reader)->close();
        joan::orig::as_anim_def(this)->build_sort_index();
        jo::view<std::uint8_t>(jo::self<jo::AnimDef>(this)->loaded) = 2;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_def(this)->load_am3(joan::orig::arg::str(directory), joan::orig::arg::str(anim_name))) != 0) {
            // [Game] AnimCache (anim_cache.hpp): the converted keys from ANIM_TMP when they were computed from these.
            std::uint64_t hash = 0;
            if (!joan::anim_cache::restore(this, directory, anim_name, hash)) {
                joan::orig::as_anim_def(this)->resolve_bones();  // 0x005b6a90
                joan::anim_cache::store(this, directory, anim_name, hash);
            }
        }
        if (global<std::uint32_t>(d::g_a67690) != 0) joan::orig::as_model(this)->update_parts();                         // 0x005b77b0
        if (global<std::uint32_t>(d::g_a67688) != 0) {
            const char* const dir = &img::global<const char>(d::s_ANIM_TMP);  // "ANIM_TMP"
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(files)->is_dir_exist_590c00(joan::orig::arg::str(dir))) == 0)            // 0x00590c00
                stdcall_call<std::int32_t>(global<std::uintptr_t>(d::iat_KERNEL32_CreateDirectoryA), dir, static_cast<void*>(nullptr));  // CreateDirectoryA
            File out;
            joan::orig::as_file(&out)->construct();
            joan::orig::as_file(&out)->file_create(joan::orig::arg::str(c_str(amt)), 1, static_cast<char>(0));  // 0x00586f00 create
            joan::orig::as_anim_def(this)->write_cache(&out);                            // 0x005b7810 write cache
            joan::orig::as_file(&out)->file_close();
            joan::orig::as_file(&out)->destruct();
        }
        joan::orig::as_file_mem(&reader)->destruct();
    }
};

#define HELPER28(name, addr, fn) JOAN_REPLACE("en3d/helpers28", name, addr, fn)
HELPER28("AnimationDef::init_5b23d0", 0x005b23d0, &H::animdef_load);
#undef HELPER28

}  // namespace en3d_helpers28
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
//
//      0x005b2aa0 AnimCache::load(a1..a4, dir, name, only_first) (thiscall, ret 0x1c) - state byte
//                 +0 (0/1/2); waits on 0x005b4140 (SleepEx(5, 1) + 0x005b3100); fills +0x107 /
//                 +6 names; <dir><name>.AMT cached file (0xa6768c, 0x00590b80 / 0x00590e20)
//                 or two Readers (0x00588430 / 0x005b4b90 / 0x005b69c0) + ANIM_TMP write-back.
//
//  - Direct callers / references: 0x005b2aa0 (0x00417845, 0x0047d41e) and 0x005c9350 (0x004d2519)
//
//  - 0x005b2aa0: SEH frame only for the Reader / File destructors on unwind; no callee (call graph
//    checked, 178 functions) reaches _CxxThrowException. Normal path reproduced call by call.
//    Local objects: Path 0x194 (0x005a11a0), File 0x110 (0x004179c0, frame gaps 0x110), Reader
//    0x11c (0x00587f80). In the cached branch 0x005b69c0 gets the raw a3 value (as original).
namespace {
namespace game_b103_11_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::stdcall_import;
using joan::img::obj;
using joan::img::str;

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

void* const kFiles = obj(d::g_misc);  // file system helper
const char* const kEmpty = str(d::kEmpty);    // ""
const char* const kAmt = str(d::s_AMT);      // ".AMT"
const char* const kAnimTmp = str(d::s_ANIM_TMP);  // "ANIM_TMP"

using joan::orig::helpers::stricmp_;  // _stricmp
using joan::orig::helpers::strlen_;  // strlen
using joan::orig::helpers::strcpy_;  // strcpy

struct alignas(4) Path {
    u8 bytes[0x194];
};
struct alignas(4) File {
    u8 bytes[0x210];  // 0x110 used (0x004179c0); spare room
};
struct alignas(4) Reader {
    u8 bytes[0x1a0];  // 0x11c in the frame; spare room: 0x005885e0 strcpy's the File name (<= 0x190)
};
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90

JOAN_COUNTER(c_anim_load, "AnimCache::load");

struct H {
    // 0x005b2aa0
    void anim_load(void* a1, u32 a2, void* a3, u32 a4, const char* dir, const char* name, i8 only_first) {
        joan::hitch::Span hitch_span("anim_load");  // [Debug] HitchLog
        c_anim_load.hit();
        if (static_cast<i32>(jo::self<jo::AnimDef>(this)->loaded) == 2) return;
        if (static_cast<i32>(jo::self<jo::AnimDef>(this)->loaded) == 1 && static_cast<i32>(only_first) != 0) return;
        while (joan::orig::arg::as<i32>(joan::orig::as_reader(this)->done()) == 0) {  // 0x005b4140
            stdcall_import<u32>(d::iat_KERNEL32_SleepEx, 5u, 1);       // SleepEx(5, TRUE)
            joan::orig::as_anim_def(this)->stream_update();                   // 0x005b3100
        }
        char* const name_buf = jo::self<jo::AnimDef>(this)->name;
        if (stricmp_(name_buf, kEmpty) == 0 && strlen_(name) < 0x20) strcpy_(name_buf, name);
        char* const dir_buf = jo::self<jo::AnimDef>(this)->dir;
        if (stricmp_(dir_buf, kEmpty) == 0 && strlen_(dir) < 0x100) strcpy_(dir_buf, dir);

        Path path;
        joan::orig::as_string(&path)->construct(joan::orig::arg::str(dir_buf));  // 0x005a11a0
        joan::orig::as_string(&path)->append(joan::orig::arg::str(name));  // 0x005a1550 append
        joan::orig::as_string(&path)->append(joan::orig::arg::str(kAmt));  // 0x005a1550 append ".AMT"

        if (static_cast<i32>(only_first) != 0) {
            joan::orig::as_anim_def(this)->read_header(joan::orig::arg::mptr(a1));  // 0x005b5830
            joan::orig::as_anim_def(this)->build_sort_index();      // 0x005b5a70
            jo::view<u8>(jo::self<jo::AnimDef>(this)->loaded) = 1;
            return;
        }

        bool cached = false;
        if (global<i32>(d::g_a6768c) != 0 &&
            joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(c_str(path))) != 0 &&  // 0x00590b80
            joan::orig::arg::as<i32>(joan::orig::as_misc(kFiles)->compare_file_time_590e20(joan::orig::arg::str(c_str(path)), joan::orig::arg::str(a1))) > 0) {  // 0x00590e20
            cached = a3 == nullptr || joan::orig::arg::as<i32>(joan::orig::as_misc(kFiles)->compare_file_time_590e20(joan::orig::arg::str(c_str(path)), joan::orig::arg::str(a3))) > 0;
        }
        if (cached) {
            joan::orig::as_anim_def(this)->read_header(joan::orig::arg::mptr(a1));  // 0x005b5830
            joan::orig::as_anim_def(this)->build_sort_index();      // 0x005b5a70
            jo::view<u8>(jo::self<jo::AnimDef>(this)->loaded) = 1;
            File file;
            joan::orig::as_file(&file)->construct();                             // 0x004179c0
            joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(path)), 1, static_cast<char>(0));  // 0x00586da0 open
            Reader reader;
            joan::orig::as_file_mem(&reader)->construct();  // 0x00587f80
            joan::orig::as_file_mem(&reader)->open_file(joan::orig::arg::file(&file));  // 0x005885e0
            joan::orig::as_anim_def(this)->read_cache(&reader);   // 0x005b7930
            joan::orig::as_file_mem(&reader)->close();  // 0x005886d0
            joan::orig::as_file(&file)->file_close();            // 0x00587260 close
            jo::view<u8>(jo::self<jo::AnimDef>(this)->loaded) = 2;
            joan::orig::as_anim_def(this)->read_am3(joan::orig::arg::mptr(a3));  // 0x005b69c0 (raw a3, as the original)
            joan::orig::as_file_mem(&reader)->destruct();  // 0x00587fd0 ~Reader
            joan::orig::as_file(&file)->destruct();  // 0x005872b0 ~File
            return;
        }

        Reader first;
        joan::orig::as_file_mem(&first)->construct();   // 0x00587f80
        joan::orig::as_file_mem(&first)->open_slice_mem(joan::orig::arg::mptr(a1), joan::orig::arg::i32(a2));     // 0x00588430
        joan::orig::as_anim_def(this)->read(&first);       // 0x005b4b90
        joan::orig::as_file_mem(&first)->close();     // 0x005886d0
        joan::orig::as_anim_def(this)->build_sort_index();                    // 0x005b5a70
        jo::view<u8>(jo::self<jo::AnimDef>(this)->loaded) = 2;
        Reader second;
        joan::orig::as_file_mem(&second)->construct();  // 0x00587f80
        joan::orig::as_file_mem(&second)->open_slice_mem(joan::orig::arg::mptr(a3), joan::orig::arg::i32(a4));    // 0x00588430
        if (joan::orig::arg::as<i32>(joan::orig::as_anim_def(this)->read_am3(&second)) != 0) {  // 0x005b69c0
            // [Game] AnimCache (anim_cache.hpp): as AnimDef::load, the converted keys when computed from these.
            std::uint64_t hash = 0;
            if (!joan::anim_cache::restore(this, dir_buf, name, hash)) {
                joan::orig::as_anim_def(this)->resolve_bones();  // 0x005b6a90
                joan::anim_cache::store(this, dir_buf, name, hash);
            }
        }
        joan::orig::as_file_mem(&second)->close();  // 0x005886d0
        if (global<i32>(d::g_a67690) != 0) joan::orig::as_model(this)->update_parts();  // 0x005b77b0
        if (global<i32>(d::g_a67688) != 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_misc(kFiles)->is_dir_exist_590c00(joan::orig::arg::str(kAnimTmp))) == 0)            // 0x00590c00
                stdcall_import<i32>(d::iat_KERNEL32_CreateDirectoryA, kAnimTmp, static_cast<void*>(nullptr));  // CreateDirectoryA
            File out;
            joan::orig::as_file(&out)->construct();                   // 0x004179c0
            joan::orig::as_file(&out)->file_create(joan::orig::arg::str(c_str(path)), 1, static_cast<char>(0));  // 0x00586f00 create
            joan::orig::as_anim_def(this)->write_cache(&out);               // 0x005b7810
            joan::orig::as_file(&out)->file_close();                      // 0x00587260 close
            joan::orig::as_file(&out)->destruct();                  // 0x005872b0 ~File
        }
        joan::orig::as_file_mem(&second)->destruct();  // 0x00587fd0 ~Reader
        joan::orig::as_file_mem(&first)->destruct();  // 0x00587fd0 ~Reader
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("AnimationDef::init_5b2aa0", 0x005b2aa0, &H::anim_load);
#undef B103

}  // namespace game_b103_11_2
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
//
//      0x005b2f70 AnimDef::set_source(dir, name) (ret 8) - <dir><name>.AM2 existence check (log
//                 "Error loading Animation : %1s"), strcat name into +0x107 (< 0x20) and dir into
//                 +6 (< 0x100) when empty, byte +1 = 1
namespace {
namespace en3d_b85_04_2 {

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

void* const kFiles = obj(d::g_misc);   // file system helper
void* const kLog = obj(d::g_err);     // error log

std::uint32_t game_strlen(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }
char* game_strcat(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
std::int32_t game_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

// String / path object (0x194 bytes on the stack, no destructor).
struct alignas(4) Path {
    std::uint8_t bytes[0x194];
};
static_assert(sizeof(Path) == 0x194);
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90

JOAN_COUNTER(c_anim_source, "AnimDef::set_source");

struct H {
    // 0x005b2f70
    void anim_set_source(const char* dir, const char* name) {
        c_anim_source.hit();
        if (jo::view<std::int8_t>(jo::self<jo::AnimDef>(this)->source_set) != 0) return;
        if (jo::self<jo::AnimDef>(this)->loaded == 0) {
            Path path;
            joan::orig::as_string(&path)->construct(joan::orig::arg::str(dir));  // 0x005a11a0
            joan::orig::as_string(&path)->append(joan::orig::arg::str(name));     // 0x005a1550
            joan::orig::as_string(&path)->append(str(d::s_AM2_9cf5bc));                // 0x005a1550 ".AM2"
            if (joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(c_str(path))) == 0) {  // 0x00590b80
                joan::orig::error::fatal(joan::orig::arg::mptr(kLog), str(d::s_Error_loading_Animation_1s), c_str(path));     // 0x00586cb0
                return;
            }
            char* const anim_name = jo::self<jo::AnimDef>(this)->name;
            if (game_stricmp(anim_name, str(d::kEmpty)) == 0 && game_strlen(name) < 0x20u) game_strcat(anim_name, name);
            char* const anim_dir = jo::self<jo::AnimDef>(this)->dir;
            if (game_stricmp(anim_dir, str(d::kEmpty)) == 0 && game_strlen(dir) < 0x100u) game_strcat(anim_dir, dir);
            jo::self<jo::AnimDef>(this)->source_set = 1;
        } else if (jo::self<jo::AnimDef>(this)->loaded == 1) {
            jo::self<jo::AnimDef>(this)->source_set = 1;
        }
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("AnimationDef::async_init", 0x005b2f70, &H::anim_set_source);
#undef E3D

}  // namespace en3d_b85_04_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers30.cpp (en3d/helpers30)
//
//      0x005b3100 AnimDef::stream_update()           - background streaming state machine
//                 (+1: 1 start, 3 .AM2 loading, 0xd .AMT loading, 4 .AM3 after .AM2,
//                 0xe .AM3 after .AMT; loaders +0x17f .AM2 / +0x183 .AMT / +0x187 .AM3)
namespace {
namespace en3d_helpers30_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::stdcall_call;
// KERNEL32 import through its IAT slot.
template <class R = std::int32_t, class... A>
R import_call(img::Addr slot, A... args) {
    return stdcall_call<R>(global<std::uintptr_t>(slot), args...);
}

JOAN_COUNTER(c_stream_update, "AnimDef::stream_update");

struct alignas(4) Path {
    std::uint8_t bytes[0x194];
};
struct alignas(alignof(void*)) File {  // a File: 0x10e on x86, 0x112 on x64
    std::uint8_t bytes[((sizeof(joan::orig::File) > 0x110 ? sizeof(joan::orig::File) : 0x110) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};
struct alignas(alignof(void*)) Reader {  // a FileMem: 0x11c on x86, 0x12c on x64
    std::uint8_t bytes[((sizeof(joan::orig::FileMem) > 0x11c ? sizeof(joan::orig::FileMem) : 0x11c) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};

const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
void* files() { return img::obj(d::g_misc); }

// new FileLoader (0x164 bytes, 0x00577be0) on `path`, started with no buffer (whole file).
void* start_loader(Path& path, void*& field) {
    void* loader = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::FileLoader)));  // 0x164 on x86
    if (loader != nullptr) loader = joan::orig::as_file_loader(loader)->construct();  // 0x00577be0
    field = loader;
    joan::orig::as_file_loader(field)->set_path(joan::orig::arg::str(c_str(path)));  // 0x00577c70
    joan::orig::as_file_loader(field)->start(nullptr, 0u, joan::orig::arg::u32(-1), nullptr, nullptr);  // 0x00577d10
    return loader;
}

// 0x00578110 then delete (0x005afd50 scalar deleting destructor) and clear the field.
void free_loader(void*& field) {
    joan::orig::as_file_loader(field)->stop();  // 0x00578110
    void* const loader = field;
    if (loader != nullptr) joan::orig::as_file_loader(loader)->scalar_deleting_destructor(1u);  // 0x005afd50
    field = nullptr;
}

// Reader over the loader's buffer (+0x10c) and size (+0x118).
void open_loaded(Reader& reader, void* loader) {
    joan::orig::as_file_mem(&reader)->open_mem_5881c0(joan::orig::arg::str(jo::view<void*>(jo::as_file_loader(loader)->buffer)), joan::orig::arg::i32(jo::as_file_loader(loader)->bytes_read), static_cast<char>(joan::orig::arg::i32(1)), static_cast<char>(joan::orig::arg::i32(0)));  // 0x005881c0
}

struct H {
    // 0x005b3100 AnimDef::stream_update(): background streaming state machine (+1).
    void stream_update() {
        c_stream_update.hit();
        const std::int8_t state = jo::view<std::int8_t>(jo::self<jo::AnimDef>(this)->source_set);
        if (state == 0) return;
        char* const dir = jo::self<jo::AnimDef>(this)->dir;
        char* const nm = jo::self<jo::AnimDef>(this)->name;
        auto make_path = [&](Path& p, img::Addr extension) {
            joan::orig::as_string(&p)->construct(joan::orig::arg::str(dir));  // 0x005a11a0
            joan::orig::as_string(&p)->append(joan::orig::arg::str(nm));  // 0x005a1550 append
            joan::orig::as_string(&p)->append(joan::orig::arg::str(&img::global<const char>(extension)));
        };
        if (state == 1) {
            Path am2, amt, am3;
            make_path(am2, d::s_AM2_9cf5bc);  // ".AM2"
            make_path(amt, d::s_AMT);  // ".AMT"
            make_path(am3, d::s_AM3);  // ".AM3"
            bool use_cache = false;
            if (global<std::uint32_t>(d::g_a6768c) != 0 && joan::orig::as_misc(files())->is_file_exist(joan::orig::arg::str(c_str(amt))) != 0) {
                // Argument order: c_str(am2) is evaluated (pushed) before c_str(amt).
                const char* const a2 = c_str(am2);
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(files())->compare_file_time_590e20(joan::orig::arg::str(c_str(amt)), joan::orig::arg::str(a2))) > 0) {  // 0x00590e20 newer
                    if (joan::orig::as_misc(files())->is_file_exist(joan::orig::arg::str(c_str(am3))) == 0) {
                        use_cache = true;
                    } else {
                        const char* const a3 = c_str(am3);
                        use_cache = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(files())->compare_file_time_590e20(joan::orig::arg::str(c_str(amt)), joan::orig::arg::str(a3))) > 0;
                    }
                }
            }
            if (use_cache) {
                if (jo::self<jo::AnimDef>(this)->loaded < 1) {
                    File file;
                    joan::orig::as_file(&file)->construct();                                  // 0x004179c0
                    joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(am2)), 1, static_cast<char>(0));  // 0x00586da0 open
                    Reader reader;
                    joan::orig::as_file_mem(&reader)->construct();                                // 0x00587f80
                    joan::orig::as_file_mem(&reader)->open_file(joan::orig::arg::file(&file));                                // 0x005885e0
                    joan::orig::as_anim_def(this)->read_header(&reader);                   // 0x005b5830 header
                    joan::orig::as_anim_def(this)->build_sort_index();                                          // 0x005b5a70
                    jo::view<std::uint8_t>(jo::self<jo::AnimDef>(this)->loaded) = 1;
                    joan::orig::as_file_mem(&reader)->close();                                       // 0x005886d0
                    joan::orig::as_file(&file)->file_close();                                     // 0x00587260
                    joan::orig::as_file_mem(&reader)->destruct();                                       // 0x00587fd0 ~Reader
                    joan::orig::as_file(&file)->destruct();                                 // 0x005872b0 ~File
                }
                start_loader(amt, jo::self<jo::AnimDef>(this)->loader_183);
                start_loader(am3, jo::self<jo::AnimDef>(this)->loader_187);
                jo::self<jo::AnimDef>(this)->source_set = 0xd;
            } else {
                start_loader(am2, jo::self<jo::AnimDef>(this)->loader_17f);
                start_loader(am3, jo::self<jo::AnimDef>(this)->loader_187);
                jo::self<jo::AnimDef>(this)->source_set = 3;
            }
            return;
        }
        if (state == 3) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::self<jo::AnimDef>(this)->loader_17f)->done()) == 0) return;  // 0x00578250 done
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::self<jo::AnimDef>(this)->loader_17f)->ok()) == 0) {      // 0x005b0a80 ok
                free_loader(jo::self<jo::AnimDef>(this)->loader_187);
                free_loader(jo::self<jo::AnimDef>(this)->loader_17f);
                jo::self<jo::AnimDef>(this)->source_set = 0;
                return;
            }
            Reader reader;
            joan::orig::as_file_mem(&reader)->construct();
            open_loaded(reader, jo::self<jo::AnimDef>(this)->loader_17f);
            joan::orig::as_anim_def(this)->read(&reader);  // 0x005b4b90 read
            joan::orig::as_file_mem(&reader)->close();
            joan::orig::as_anim_def(this)->build_sort_index();  // 0x005b5a70
            jo::view<std::uint8_t>(jo::self<jo::AnimDef>(this)->loaded) = 2;
            jo::self<jo::AnimDef>(this)->source_set = 4;
            joan::orig::as_file_mem(&reader)->destruct();
            return;
        }
        if (state == 0xd) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::self<jo::AnimDef>(this)->loader_183)->done()) == 0) return;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::self<jo::AnimDef>(this)->loader_183)->ok()) == 0) {
                free_loader(jo::self<jo::AnimDef>(this)->loader_187);
                free_loader(jo::self<jo::AnimDef>(this)->loader_183);
                jo::self<jo::AnimDef>(this)->source_set = 0;
                return;
            }
            Reader reader;
            joan::orig::as_file_mem(&reader)->construct();
            open_loaded(reader, jo::self<jo::AnimDef>(this)->loader_183);
            joan::orig::as_anim_def(this)->read_cache(&reader);  // 0x005b7930 read cache
            joan::orig::as_file_mem(&reader)->close();
            jo::view<std::uint8_t>(jo::self<jo::AnimDef>(this)->loaded) = 2;
            jo::self<jo::AnimDef>(this)->source_set = 0xe;
            joan::orig::as_file_mem(&reader)->destruct();
            return;
        }
        if (state == 4) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::self<jo::AnimDef>(this)->loader_187)->done()) == 0) return;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::self<jo::AnimDef>(this)->loader_187)->ok()) == 0) {
                free_loader(jo::self<jo::AnimDef>(this)->loader_187);
                free_loader(jo::self<jo::AnimDef>(this)->loader_17f);
                jo::self<jo::AnimDef>(this)->source_set = 0;
                return;
            }
            Reader reader;
            joan::orig::as_file_mem(&reader)->construct();
            open_loaded(reader, jo::self<jo::AnimDef>(this)->loader_187);
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_def(this)->read_am3(&reader)) != 0) joan::orig::as_anim_def(this)->resolve_bones();  // 0x005b69c0 / 0x005b6a90
            joan::orig::as_file_mem(&reader)->close();
            if (global<std::uint32_t>(d::g_a67690) != 0) joan::orig::as_model(this)->update_parts();  // 0x005b77b0
            if (global<std::uint32_t>(d::g_a67688) != 0) {
                const char* const tmp = &img::global<const char>(d::s_ANIM_TMP);  // "ANIM_TMP"
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(files())->is_dir_exist_590c00(joan::orig::arg::str(tmp))) == 0)          // 0x00590c00
                    import_call<std::int32_t>(d::iat_KERNEL32_CreateDirectoryA, tmp, static_cast<void*>(nullptr));  // CreateDirectoryA
                Path amt;
                make_path(amt, d::s_AMT);  // ".AMT"
                File out;
                joan::orig::as_file(&out)->construct();
                joan::orig::as_file(&out)->file_create(joan::orig::arg::str(c_str(amt)), 1, static_cast<char>(0));  // 0x00586f00 create
                joan::orig::as_anim_def(this)->write_cache(&out);             // 0x005b7810 write cache
                joan::orig::as_file(&out)->file_close();
                joan::orig::as_file(&out)->destruct();
            }
            free_loader(jo::self<jo::AnimDef>(this)->loader_187);
            free_loader(jo::self<jo::AnimDef>(this)->loader_17f);
            jo::self<jo::AnimDef>(this)->source_set = 0;
            joan::orig::as_file_mem(&reader)->destruct();
            return;
        }
        if (state == 0xe) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::self<jo::AnimDef>(this)->loader_187)->done()) == 0) return;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::self<jo::AnimDef>(this)->loader_187)->ok()) == 0) {
                free_loader(jo::self<jo::AnimDef>(this)->loader_187);
                free_loader(jo::self<jo::AnimDef>(this)->loader_183);
                jo::self<jo::AnimDef>(this)->source_set = 0;
                return;
            }
            Reader reader;
            joan::orig::as_file_mem(&reader)->construct();
            open_loaded(reader, jo::self<jo::AnimDef>(this)->loader_187);
            joan::orig::as_anim_def(this)->read_am3(&reader);  // 0x005b69c0
            joan::orig::as_file_mem(&reader)->close();
            free_loader(jo::self<jo::AnimDef>(this)->loader_187);
            free_loader(jo::self<jo::AnimDef>(this)->loader_183);
            jo::self<jo::AnimDef>(this)->source_set = 0;
            joan::orig::as_file_mem(&reader)->destruct();
        }
    }
};

#define HELPER30(name, addr, fn) JOAN_REPLACE("en3d/helpers30", name, addr, fn)
HELPER30("AnimDef::stream_update", 0x005b3100, &H::stream_update);
#undef HELPER30

}  // namespace en3d_helpers30_2
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
namespace game_leaves80a_2 {


struct H {
    std::int32_t done() { return static_cast<std::int8_t>(jo::self<jo::AnimDef>(this)->source_set) == 0 ? 1 : 0; }  // 0x005b4140
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("AnimationDef::is_async_init_finish", 0x005b4140, &H::done);
#undef L

}  // namespace game_leaves80a_2
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04_3.cpp (en3d/b85_04)
namespace {
namespace en3d_b85_04_3 {

void* game_memset(void* p, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // 0x005b2390
    H* anim_loader_ctor() {
        game_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::AnimDef)));  // 0x18b (x86)
        return this;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("AnimLoader::AnimLoader", 0x005b2390, &H::anim_loader_ctor);
#undef E3D

}  // namespace en3d_b85_04_3
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::AnimDef::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b85_04_3::H::anim_loader_ctor>(this)); }  // 0x5b2390
auto joan::orig::AnimDef::load(const char* a0, const char* a1, std::uint32_t a2) -> void { joan::bind::member<&::en3d_helpers28::H::animdef_load>(this, a0, a1, a2); }  // 0x5b23d0
auto joan::orig::AnimDef::load_streamed(void* a0, std::uint32_t a1, void* a2, std::uint32_t a3, const char* a4, const char* a5, std::int8_t a6) -> void { joan::bind::member<&::game_b103_11_2::H::anim_load>(this, a0, a1, a2, a3, a4, a5, a6); }  // 0x5b2aa0
auto joan::orig::AnimDef::set_source(const char* a0, const char* a1) -> void { joan::bind::member<&::en3d_b85_04_2::H::anim_set_source>(this, a0, a1); }  // 0x5b2f70
auto joan::orig::AnimDef::stream_update() -> void { joan::bind::member<&::en3d_helpers30_2::H::stream_update>(this); }  // 0x5b3100
auto joan::orig::Reader::done() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves80a_2::H::done>(this)); }  // 0x5b4140
auto joan::orig::AnimDef::release(std::uint32_t a0) -> void { joan::bind::member<&::en3d_b85_04::H::anim_release>(this, a0); }  // 0x5b4170
auto joan::orig::BoneTree::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b85_04::H::delete_5b46f0>(this, a0)); }  // 0x5b46f0
auto joan::orig::AnimDef::scalar_delete_5b4740(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b86_04::H::del_5b4740>(this, a0)); }  // 0x5b4740
auto joan::orig::AnimDef::vector_delete_5b4790(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b86_04::H::del_5b4790>(this, a0)); }  // 0x5b4790
auto joan::orig::AnimDef::vector_delete_5b4830(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b86_04::H::del_5b4830>(this, a0)); }  // 0x5b4830
auto joan::orig::AnimDef::scalar_delete_5b48d0(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b86_04::H::del_5b48d0>(this, a0)); }  // 0x5b48d0
auto joan::orig::AnimDef::vector_delete_5b4920(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b86_04::H::del_5b4920>(this, a0)); }  // 0x5b4920
auto joan::orig::AnimDef::lookup_word(const char* a0, std::uint8_t a1, void* a2) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b91_03::H::lookup_word>(this, a0, a1, a2)); }  // 0x5b4a00
auto joan::orig::AnimDef::find_action_rec(void* a0, void* a1, const char* a2, std::uint8_t a3, void* a4) -> void { joan::bind::member<&::en3d_b83_08::H::find_action_rec>(this, a0, a1, a2, a3, a4); }  // 0x5b4a60
auto joan::orig::AnimDef::read(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers30::H::read>(this, a0)); }  // 0x5b4b90
auto joan::orig::AnimDef::write_extras(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers32::H::write_extras>(this, a0)); }  // 0x5b4fe0
auto joan::orig::AnimDef::read_extras(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers32::H::read_extras>(this, a0)); }  // 0x5b5350
auto joan::orig::AnimDef::read_header(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers30::H::read_header>(this, a0)); }  // 0x5b5830
auto joan::orig::AnimDef::build_sort_index() -> void { joan::bind::member<&::game_leaves80b::H::build_sort_index>(this); }  // 0x5b5a70
auto joan::orig::AnimDef::find_action(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b83_08::H::find_action>(this, a0)); }  // 0x5b5be0
auto joan::orig::AnimSet::frame(std::int32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::anim_frame>(this, a0, a1)); }  // 0x5b5dd0
auto joan::orig::AnimDef::animate(void* a0, std::int32_t a1, std::uint32_t a2, float a3, std::uint32_t a4, void* a5, void* a6, float a7, void* a8, void* a9) -> void { joan::bind::member<&::en3d_helpers13::H::animate>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9); }  // 0x5b5e40
auto joan::orig::AnimDef::load_am3(const char* a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers30::H::load_am3>(this, a0, a1)); }  // 0x5b6800
auto joan::orig::AnimDef::read_am3(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers32::H::read_am3>(this, a0)); }  // 0x5b69c0
auto joan::orig::AnimDef::resolve_bones() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves80b::H::resolve_bones>(this)); }  // 0x5b6a90
auto joan::orig::AnimDef::make_relative(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers32::H::make_relative>(this, a0)); }  // 0x5b6b30
auto joan::orig::AnimDef::entry(std::int32_t a0) -> AnimDef* { return joan::bind::conv<AnimDef*>(joan::bind::member<&::en3d_helpers6::H::anim_entry>(this, a0)); }  // 0x5b7650
auto joan::orig::Model::update_parts() -> void { joan::bind::member<&::game_leaves80a::H::update_parts>(this); }  // 0x5b77b0
auto joan::orig::AnimDef::write_cache(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers30::H::write_cache>(this, a0)); }  // 0x5b7810
auto joan::orig::AnimDef::read_cache(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers30::H::read_cache>(this, a0)); }  // 0x5b7930
auto joan::orig::Track::allocate(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers32::H::track_allocate>(this, a0)); }  // 0x5b7bf0
auto joan::orig::Track::sample(float a0, void* a1, void* a2, void* a3) -> float { return joan::bind::conv<float>(joan::bind::member<&::en3d_helpers14::H::track_sample>(this, a0, a1, a2, a3)); }  // 0x5b7c70
auto joan::orig::Track::compress() -> void { joan::bind::member<&::en3d_helpers32::H::compress>(this); }  // 0x5b83a0
auto joan::orig::Key::set(std::uint32_t a0, void* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers32::H::key_set>(this, a0, a1)); }  // 0x5b8770
auto joan::orig::AnimKey::interpolate(void* a0, void* a1, float a2) -> void { joan::bind::member<&::en3d_helpers14::H::key_interpolate>(this, a0, a1, a2); }  // 0x5b8840
auto joan::orig::AnimKey::to_matrix(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers13::H::key_to_matrix>(this, a0)); }  // 0x5b8910
auto joan::orig::ColourKey::interpolate(void* a0, void* a1, float a2) -> void { joan::bind::member<&::en3d_helpers14::H::colour_interpolate>(this, a0, a1, a2); }  // 0x5b8960
auto joan::orig::ColourTrack::allocate(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers37::H::list16_allocate>(this, a0)); }  // 0x5b8ac0
auto joan::orig::ColourTrack::sample(float a0, void* a1, void* a2) -> float { return joan::bind::conv<float>(joan::bind::member<&::en3d_helpers14::H::colour_sample>(this, a0, a1, a2)); }  // 0x5b8b40
auto joan::orig::AlphaKey::interpolate(void* a0, void* a1, float a2) -> void { joan::bind::member<&::en3d_helpers14::H::alpha_interpolate>(this, a0, a1, a2); }  // 0x5b9020
auto joan::orig::AlphaTrack::allocate(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers37::H::list8_allocate>(this, a0)); }  // 0x5b9140
auto joan::orig::AlphaTrack::sample(float a0, void* a1, void* a2) -> float { return joan::bind::conv<float>(joan::bind::member<&::en3d_helpers14::H::alpha_sample>(this, a0, a1, a2)); }  // 0x5b91c0
auto joan::orig::EventList::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers37::H::events_ctor>(this)); }  // 0x5b9660
auto joan::orig::EventList::allocate(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers37::H::events_allocate>(this, a0)); }  // 0x5b9710
