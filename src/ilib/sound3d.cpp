// ilib/sound3d.cpp: functions of the original d:\ww\ilib\sound3d.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <bit>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../platform/win32_types.hpp"
#include "../original/engine.hpp"
#include "../original/gameset.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../runtime/hitch.hpp"
#include "sound_archives.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using i8 = std::int8_t;

using joan::call::import_call;

constexpr img::Addr kAlSourcef = d::iat_OpenAL32_alSourcef;

void* const kHeap = obj(d::g_mem);

}  // namespace

// ---------------------------------------- from modules/game_b111_10.cpp (fix/uninit)
//
//      0x0048d120 Sound3D::create_buffer()         (thiscall, ret 0)
namespace {
namespace game_b111_10 {

// OpenAL32.dll (cdecl) import through its IAT slot.
template <class R = void, class... A>
R al_call(img::Addr slot, A... args) {
    return cdecl_call<R>(global<std::uintptr_t>(slot), args...);
}
i32 sx(u16 v) { return static_cast<i32>(static_cast<i16>(v)); }

void* const kMem = obj(d::g_mem);    // memory manager (0x0058e7b0 alloc, 0x0058eb30 realloc)
void* const kTimer = obj(d::g_misc);  // 0x00591050 = timeGetTime, 0x005911a0 / 0x005910f0 sorted array

JOAN_COUNTER(c_create_buffer, "Sound3D::create_buffer");

struct H {

    // ------------------------------------------------------------------ 0x0048d120
    // fix/uninit: alGenBuffers (0xa6dbb0) does not write the id when it fails (no current context /
    // out of memory) and the original then passes the uninitialised local [ebp-0x8] to alIsBuffer,
    // whose answer decides between "return 0" and creating the sound with a garbage buffer name.
    // The code clearly means "no buffer generated -> return 0": the id starts at 0xffffffff, a name
    // no OpenAL implementation hands out (0 itself would be AL_NONE, which alIsBuffer accepts).
    void* create_buffer() {
        c_create_buffer.hit();
        if (static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_sound3dsystem(this)->is_initialized())) == 0) return nullptr;  // 0x005a1110
        u32 id = 0xffffffffu;
        al_call(d::iat_OpenAL32_alGenBuffers, i32{1}, &id);                       // alGenBuffers
        if (al_call<u8>(d::iat_OpenAL32_alIsBuffer, id) == 0) return nullptr;  // alIsBuffer
        const char* const file = str(d::s_d_ww_ilib_sound3d_cpp);            // d:\ww\ilib\sound3d.cpp
        if (sx(jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->buffer_count)) >= sx(jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->buffer_capacity))) {
            jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->buffer_capacity) = static_cast<u16>(jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->buffer_capacity) + 0x20);
            jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->buffers) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->buffers)), joan::orig::arg::u32(sx(jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->buffer_capacity)) * static_cast<i32>(sizeof(void*))), joan::orig::arg::str(file), joan::orig::arg::i32(sx(global<u16>(d::g_a16884)) + 0x15));  // 0x0058eb30
        }
        void* key = joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(i32{0x11a}), joan::orig::arg::str(file), joan::orig::arg::i32(sx(global<u16>(d::g_a16884)) + 0x1b));  // 0x0058e7b0
        joan::orig::as_al_buffer(key)->construct();  // 0x0059ed50
        jo::as_sound3dbuffer(key)->al_buffer = id;
        joan::orig::as_sound3dbuffer(key)->on_create();  // 0x0059edc0
        i32 pos = 0;  // always written by 0x005911a0 (out pointer non-null; every path stores it)
        joan::orig::as_misc(kTimer)->bin_search_5911a0(&key, joan::orig::arg::str(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->buffers)), joan::orig::arg::i32(sx(jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->buffer_count))), static_cast<i32>(sizeof(void*)), fn::code_value(fn::cmp_ptr_u32_b), &pos);  // 0x005911a0 (comparator thunk 0x00403b3e -> 0x005a10b0)
        joan::orig::as_misc(kTimer)->insert_array_rec_5910f0(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->buffers)), joan::orig::arg::i32(sx(jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->buffer_count))), static_cast<i32>(sizeof(void*)), joan::orig::arg::i32(pos), &key);  // 0x005910f0
        jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->buffer_count) = static_cast<u16>(jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->buffer_count) + 1);
        return key;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// [ebp-0x8] OpenAL buffer id unwritten when alGenBuffers fails -> 0xffffffff (alIsBuffer false -> return 0).
FIX("Sound3D::create_buffer_48d120", 0x0048d120, &H::create_buffer);
#undef FIX

}  // namespace game_b111_10
}  // namespace

// ---------------------------------------- from modules/game_b111_12.cpp (fix/uninit)
//
//     - Sound3d::alloc_source 0x004907e0 (thiscall, ret 0): alGenSources failure leaves the
//       source id slot unwritten and alIsSource reads it. Fix: id = 0 (alIsSource(0) is false ->
//       returns 0, as for any failure).
namespace {
namespace game_b111_12 {


JOAN_COUNTER(c_alloc_source, "Sound3d::alloc_source");

struct H {

    // -------------------------------------------------------------------- 0x004907e0
    jo::Sound3D* alloc_source() {
        c_alloc_source.hit();
        if ((joan::orig::arg::as<u32>(joan::orig::as_sound3dsystem(this)->is_initialized()) & 0xffu) == 0) return nullptr;  // 0x005a1110
        // fix/uninit: alGenSources leaves the id unwritten on failure; the original then calls
        // alIsSource on stack garbage. 0 is never a valid source name (alIsSource(0) = false).
        u32 source = 0;
        import_call<void>(d::iat_OpenAL32_alGenSources, i32{1}, &source);  // alGenSources
        if (import_call<i8>(d::iat_OpenAL32_alIsSource, source) == 0) return nullptr;  // alIsSource
        if (static_cast<i32>(jo::self<jo::Sound3DSystem>(this)->source_count) >= static_cast<i32>(jo::self<jo::Sound3DSystem>(this)->source_capacity)) {
            jo::self<jo::Sound3DSystem>(this)->source_capacity = static_cast<i16>(jo::self<jo::Sound3DSystem>(this)->source_capacity + 0x20);
            const i32 line = static_cast<i32>(global<i16>(d::g_a1688c)) + 0x15;
            const u32 size = static_cast<u32>(static_cast<i32>(jo::self<jo::Sound3DSystem>(this)->source_capacity)) * static_cast<u32>(sizeof(jo::Sound3DPtrArray));
            jo::self<jo::Sound3DSystem>(this)->sources = joan::orig::arg::as<jo::Sound3DPtrArray*>(joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::self<jo::Sound3DSystem>(this)->sources), joan::orig::arg::u32(size), str(d::s_d_ww_ilib_sound3d_cpp), joan::orig::arg::i32(line)));  // 0x0058eb30 realloc
        }
        const i32 line2 = static_cast<i32>(global<i16>(d::g_a1688c)) + 0x1b;
        void* node = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(i32{0x53}), str(d::s_d_ww_ilib_sound3d_cpp), joan::orig::arg::i32(line2));  // 0x0058e7b0
        joan::orig::as_sound3d(node)->construct();  // 0x0059f2e0
        jo::as_sound3d(node)->al_source = source;
        joan::orig::as_light(node)->set_defaults();  // 0x0059f340
        // 0x005911a0 writes the insertion index through its (non-NULL) out pointer on every path.
        i32 index = 0;
        joan::orig::as_misc(obj(d::g_misc))->bin_search_5911a0(&node, joan::orig::arg::str(jo::self<jo::Sound3DSystem>(this)->sources), static_cast<i32>(jo::self<jo::Sound3DSystem>(this)->source_count), static_cast<i32>(sizeof(jo::Sound3DPtrArray)), fn::code_value(fn::cmp_ptr_u32), &index);  // 0x005911a0 search (comparator 0x005a1060)
        joan::orig::as_misc(obj(d::g_misc))->insert_array_rec_5910f0(joan::orig::arg::str(jo::self<jo::Sound3DSystem>(this)->sources), static_cast<i32>(jo::self<jo::Sound3DSystem>(this)->source_count), static_cast<i32>(sizeof(jo::Sound3DPtrArray)), joan::orig::arg::i32(index), &node);  // 0x005910f0 insert
        jo::self<jo::Sound3DSystem>(this)->source_count = static_cast<i16>(jo::self<jo::Sound3DSystem>(this)->source_count + 1);
        return static_cast<jo::Sound3D*>(node);
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: alGenSources failure -> source id 0 instead of stack garbage.
FIX("Sound3d::alloc_source", 0x004907e0, &H::alloc_source);
#undef FIX

}  // namespace game_b111_12
}  // namespace

// ---------------------------------------- from modules/game_b82_16.cpp (game/b82_16)
//
//    - Sound3D::release_buffer 0x00490bc0 (ilib/sound3d.cpp, buffer): sorted-array remove +
//      alDeleteBuffers.
namespace {
namespace game_b82_16 {


JOAN_COUNTER(c_release_buf, "Sound3D::release_buffer");

struct H {

    // ------------------------------------------------------------------ 0x00490bc0
    void release_buffer(jo::Sound3DBuffer* buffer) {
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_sound3dsystem(this)->is_initialized()) == 0) return;  // 0x005a1110
        c_release_buf.hit();
        std::int32_t pos;  // written by the search
        const std::int32_t idx = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(obj(d::g_misc))->bin_search_5911a0(&buffer, joan::orig::arg::str(jo::self<jo::Sound3DSystem>(this)->buffers), static_cast<std::int32_t>(jo::self<jo::Sound3DSystem>(this)->buffer_count), static_cast<std::int32_t>(sizeof(void*)), fn::code_value(fn::cmp_ptr_u32_b), &pos));  // 0x005911a0
        std::uint32_t al_name = jo::as_sound3dbuffer(buffer)->al_buffer;
        joan::orig::as_sound3d(buffer)->cleanup();      // 0x0059ede0
        joan::orig::as_sound3dbuffer(buffer)->destroy(0u);   // 0x005a0ad0
        joan::orig::as_mem(obj(d::g_mem))->del(buffer, str(d::s_d_ww_ilib_sound3d_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a16888)) + 0x11));  // 0x0058ed70 free
        joan::orig::as_misc(obj(d::g_misc))->del_array_rec_591070(joan::orig::arg::str(jo::self<jo::Sound3DSystem>(this)->buffers), static_cast<std::int32_t>(jo::self<jo::Sound3DSystem>(this)->buffer_count), static_cast<std::int32_t>(sizeof(void*)), joan::orig::arg::i32(idx));  // 0x00591070 remove
        jo::self<jo::Sound3DSystem>(this)->buffer_count = static_cast<std::int16_t>(jo::self<jo::Sound3DSystem>(this)->buffer_count - 1);
        if (jo::self<jo::Sound3DSystem>(this)->buffer_capacity - jo::self<jo::Sound3DSystem>(this)->buffer_count >= 0x20) {
            jo::self<jo::Sound3DSystem>(this)->buffer_capacity = static_cast<std::int16_t>(jo::self<jo::Sound3DSystem>(this)->buffer_capacity - 0x20);
            if (jo::self<jo::Sound3DSystem>(this)->buffer_capacity <= 0) {
                joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::self<jo::Sound3DSystem>(this)->buffers), str(d::s_d_ww_ilib_sound3d_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a16888)) + 0x23));
                jo::self<jo::Sound3DSystem>(this)->buffers = nullptr;
                jo::self<jo::Sound3DSystem>(this)->buffer_capacity = 0;
            } else {
                jo::self<jo::Sound3DSystem>(this)->buffers = thiscall<jo::Sound3DBufferPtrArray*>(
                    fn::resize_58eb30, obj(d::g_mem), jo::self<jo::Sound3DSystem>(this)->buffers, static_cast<std::int32_t>(jo::self<jo::Sound3DSystem>(this)->buffer_capacity) * static_cast<std::int32_t>(sizeof(void*)),
                    str(d::s_d_ww_ilib_sound3d_cpp), static_cast<std::int32_t>(global<std::int16_t>(d::g_a16888)) + 0x29);  // 0x0058eb30
            }
        }
        reinterpret_cast<void(JOAN_CDECL*)(std::int32_t, const std::uint32_t*)>(global<std::uintptr_t>(d::iat_OpenAL32_alDeleteBuffers))(
            1, &al_name);  // alDeleteBuffers
    }
};

#define B16(name, addr, fn) JOAN_REPLACE("game/b82_16", name, addr, fn)
B16("Sound3D::release_buffer", 0x00490bc0, &H::release_buffer);
#undef B16

}  // namespace game_b82_16
}  // namespace

// ---------------------------------------- from modules/game_stream2.cpp (game/stream2)
//
//      0x0059f4b0 Sound3D::unqueue(short n)            - alSourceUnqueueBuffers into a temp
//                 array, release each buffer's reference (0x005a0870 / 0x00490bc0 on 0xa57ed0)
//
//      0x0059f680 Sound3D::stop()                      - alSourceStop
//
//      0x0059f700 Sound3D::set_gain(float)             - +8 = gain; alSourcef(AL_GAIN, muted
//                 (+0x2c) ? 0 : gain)
namespace {
namespace game_stream2 {

// OpenAL32 (cdecl) import through its IAT slot.
using joan::orig::helpers::al_call;

constexpr img::Addr kAlSourceUnqueueBuffers = d::iat_OpenAL32_alSourceUnqueueBuffers;
constexpr img::Addr kAlSourceStop = d::iat_OpenAL32_alSourceStop;

constexpr img::Addr kMem(d::g_mem);
constexpr img::Addr kBuffers(d::g_sound3d_system);
constexpr img::Addr kSound3dCpp(d::s_d_ww_ilib_sound3d_cpp);  // "d:\ww\ilib\sound3d.cpp"

JOAN_COUNTER(c_unqueue, "Sound3D::unqueue");

struct H {

    void unqueue(std::int32_t count_arg) {  // 0x0059f4b0 (short argument)
        const std::int16_t count = static_cast<std::int16_t>(count_arg);
        if (!(static_cast<std::int32_t>(count) > 0)) return;
        c_unqueue.hit();
        std::uint32_t* const names = thiscall<std::uint32_t*>(
            fn::add, img::obj(kMem), static_cast<std::int32_t>(count) << 2, img::str(kSound3dCpp),
            static_cast<std::int32_t>(global<std::int16_t>(d::g_a16878)) + 8);  // 0x0058e7b0
        al_call(kAlSourceUnqueueBuffers, jo::self<jo::Sound3D>(this)->al_source, static_cast<std::int32_t>(count), names);
        for (std::int16_t i = 0; static_cast<std::int32_t>(i) < static_cast<std::int32_t>(count);
             i = static_cast<std::int16_t>(i + 1)) {
            void* const buffer = joan::orig::arg::as<void*>(joan::orig::as_sound3dsystem(img::obj(kBuffers))->find_buffer(joan::orig::arg::u32(names[i])));  // 0x005a0870
            if (buffer == nullptr) continue;
            jo::as_sound3dbuffer(buffer)->refs = static_cast<std::int16_t>(jo::as_sound3dbuffer(buffer)->refs - 1);
            if (jo::as_sound3dbuffer(buffer)->refs == 0 && jo::view<std::int8_t>(jo::as_sound3dbuffer(buffer)->release_unused) != 0)
                joan::orig::as_sound3d(img::obj(kBuffers))->release_buffer(jo::as_sound3dbuffer(buffer));  // 0x00490bc0
        }
        joan::orig::as_mem(img::obj(kMem))->del(static_cast<void*>(names), joan::orig::arg::str(img::str(kSound3dCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a16878)) + 0x22));                         // 0x0058ed70
    }

    void stop() { al_call(kAlSourceStop, jo::self<jo::Sound3D>(this)->al_source); }  // 0x0059f680

    void set_gain(std::uint32_t gain_bits) {  // 0x0059f700 (float argument, kept as raw bits)
        jo::view<std::uint32_t>(jo::self<jo::Sound3D>(this)->gain) = gain_bits;
        std::uint32_t value;
        if (jo::self<jo::Sound3D>(this)->muted != 0)
            value = 0;
        else
            value = jo::view<std::uint32_t>(jo::self<jo::Sound3D>(this)->gain);
        al_call(kAlSourcef, jo::self<jo::Sound3D>(this)->al_source, 0x100a, std::bit_cast<float>(value));  // AL_GAIN (ALfloat: an xmm argument on x64)
    }
};

#define STREAM2(name, addr, fn) JOAN_REPLACE("game/stream2", name, addr, fn)
STREAM2("Sound3D::unqueue", 0x0059f4b0, &H::unqueue);
STREAM2("Sound3D::stop", 0x0059f680, &H::stop);
STREAM2("Sound3D::set_gain", 0x0059f700, &H::set_gain);
#undef STREAM2

}  // namespace game_stream2
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
//
//    - Source::rewind 0x0059f5f0 (dead code): alSourceRewind([this]).
//
//    - Listener::mute 0x0059ff10 (byte), ret 4 (dead code): alListenerf(AL_GAIN, 0 / [this]).
//
//    - Obj::nop_5a07e0 0x005a07e0 (a, b, c), ret 0xc (dead code, empty).
//
//  - 0x0058ef00, 0x005a8f70, 0x005a07e0: void; the original leaves the caller's eax (or a dead
//    temporary) in eax - no callers exist.
namespace {
namespace game_b103_10 {


using AlSourceFn = u32(JOAN_CDECL*)(u32);
using AlListenerFn = u32(JOAN_CDECL*)(i32, float);  // alListenerf(ALenum, ALfloat)

struct H {

    // ------------------------------------------------------------------ 0x0059f5f0 / 0x0059ff10
    u32 source_rewind() {
        return reinterpret_cast<AlSourceFn>(global<std::uintptr_t>(d::iat_OpenAL32_alSourceRewind))(jo::self<jo::Sound3D>(this)->al_source);  // alSourceRewind
    }
    u32 listener_mute(u8 v) {
        jo::self<jo::Listener>(this)->muted = v;
        const u32 gain = jo::self<jo::Listener>(this)->muted != 0 ? 0u : jo::view<u32>(jo::self<jo::Listener>(this)->gain);
        return reinterpret_cast<AlListenerFn>(global<std::uintptr_t>(d::iat_OpenAL32_alListenerf))(0x100a, std::bit_cast<float>(gain));  // alListenerf(AL_GAIN)
    }

    // ------------------------------------------------------------------ 0x005a07e0 (dead code)
    void nop_5a07e0(u32, u32, u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("Source::rewind", 0x0059f5f0, &H::source_rewind);
B("Listener::mute", 0x0059ff10, &H::listener_mute);
B("Obj::nop_5a07e0", 0x005a07e0, &H::nop_5a07e0);
#undef B

}  // namespace game_b103_10
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


struct H {

    void play() { import_call(d::iat_OpenAL32_alSourcePlay, jo::self<jo::Sound3D>(this)->al_source); }   // 0x0059f620 alSourcePlay
    void pause() { import_call(d::iat_OpenAL32_alSourcePause, jo::self<jo::Sound3D>(this)->al_source); }  // 0x0059f650 alSourcePause
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Sound::play", 0x0059f620, &H::play);
L("Sound::pause", 0x0059f650, &H::pause);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
//
//    - Sound3d::set_pitch 0x0059f6b0 (float), ret 4: +4 = v; alSourcef(+0, 0x1003, v).
//
//    - Listener::set_orientation 0x005a0050 (vec3* at, vec3* up), ret 8: copies to +0x1d / +0x29,
//      alListenerfv(0x100f, 6 floats).
namespace {
namespace game_b95_05 {


JOAN_COUNTER(c_listener, "Listener::set_orientation");

struct H {

    // 0x0059f6b0 (float argument kept as raw bits)
    void set_pitch(std::uint32_t bits) {
        jo::view<std::uint32_t>(jo::self<jo::Sound3D>(this)->pitch) = bits;
        using AlSourcef = void(JOAN_CDECL*)(std::uint32_t, std::int32_t, float);
        reinterpret_cast<AlSourcef>(global<std::uintptr_t>(d::iat_OpenAL32_alSourcef))(jo::self<jo::Sound3D>(this)->al_source, 0x1003, jo::self<jo::Sound3D>(this)->pitch);
    }

    // 0x005a0050
    void set_orientation(const std::uint32_t* at_v, const std::uint32_t* up_v) {
        c_listener.hit();
        jo::view<std::uint32_t>(jo::self<jo::Listener>(this)->at[0]) = at_v[0];
        jo::view<std::uint32_t>(jo::self<jo::Listener>(this)->at[1]) = at_v[1];
        jo::view<std::uint32_t>(jo::self<jo::Listener>(this)->at[2]) = at_v[2];
        jo::view<std::uint32_t>(jo::self<jo::Listener>(this)->up[0]) = up_v[0];
        jo::view<std::uint32_t>(jo::self<jo::Listener>(this)->up[1]) = up_v[1];
        jo::view<std::uint32_t>(jo::self<jo::Listener>(this)->up[2]) = up_v[2];
        std::uint32_t v[6];
        v[0] = jo::view<std::uint32_t>(jo::self<jo::Listener>(this)->at[0]);
        v[1] = jo::view<std::uint32_t>(jo::self<jo::Listener>(this)->at[1]);
        v[2] = jo::view<std::uint32_t>(jo::self<jo::Listener>(this)->at[2]);
        v[3] = jo::view<std::uint32_t>(jo::self<jo::Listener>(this)->up[0]);
        v[4] = jo::view<std::uint32_t>(jo::self<jo::Listener>(this)->up[1]);
        v[5] = jo::view<std::uint32_t>(jo::self<jo::Listener>(this)->up[2]);
        using AlListenerfv = void(JOAN_CDECL*)(std::int32_t, const std::uint32_t*);
        reinterpret_cast<AlListenerfv>(global<std::uintptr_t>(d::iat_OpenAL32_alListenerfv))(0x100f, v);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("Sound3d::set_pitch", 0x0059f6b0, &H::set_pitch);
B("Listener::set_orientation", 0x005a0050, &H::set_orientation);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
//
//      0x0059f770 AlSource::set_min_gain(thiscall(float), ret 4) - alSourcef 0x100d.
//
//      0x0059f7c0 AlSource::set_max_gain(thiscall(float), ret 4) - alSourcef 0x100e.
//
//      0x0059feb0 AlListener::set_gain  (thiscall(float), ret 4) - alListenerf 0x100a (0 when muted).
namespace {
namespace game_b93_03 {


struct H {

    void al_set_min_gain(float v) {  // 0x0059f770
        jo::self<jo::Sound3D>(this)->min_gain = v;
        import_call<void>(d::iat_OpenAL32_alSourcef, jo::self<jo::Sound3D>(this)->al_source, 0x100du, jo::self<jo::Sound3D>(this)->min_gain);  // alSourcef
    }
    void al_set_max_gain(float v) {  // 0x0059f7c0
        jo::self<jo::Sound3D>(this)->max_gain = v;
        import_call<void>(d::iat_OpenAL32_alSourcef, jo::self<jo::Sound3D>(this)->al_source, 0x100eu, jo::self<jo::Sound3D>(this)->max_gain);  // alSourcef
    }
    void al_listener_gain(float v) {  // 0x0059feb0
        jo::self<jo::Listener>(this)->gain = v;
        const u32 g = jo::self<jo::Listener>(this)->muted != 0 ? 0u : jo::view<u32>(jo::self<jo::Listener>(this)->gain);
        import_call<void>(d::iat_OpenAL32_alListenerf, 0x100au, std::bit_cast<float>(g));  // alListenerf
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("AlSource::set_min_gain", 0x0059f770, &H::al_set_min_gain);
B93("AlSource::set_max_gain", 0x0059f7c0, &H::al_set_max_gain);
B93("AlListener::set_gain", 0x0059feb0, &H::al_listener_gain);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
//
//    - Sound3d::set_rolloff 0x0059f810 (float), ret 4: +0x14 = v; alSourcef(+0, 0x1022, v).
//
//    - Listener::set_doppler 0x005a06f0 (float), ret 4: if 0x005a1110(): +0xa = v,
//      alDopplerFactor(v).
namespace {
namespace game_b97_05 {

template <class R = void, class... A>
R cdecl_import(img::Addr slot, A... args) {  // cdecl import (OpenAL) through its IAT slot
    return reinterpret_cast<R(JOAN_CDECL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}

struct H {

    // 0x0059f810
    void set_rolloff(std::uint32_t bits) {
        jo::view<std::uint32_t>(jo::self<jo::Sound3D>(this)->cone_outer_gain) = bits;
        cdecl_import(d::iat_OpenAL32_alSourcef, jo::self<jo::Sound3D>(this)->al_source, 0x1022, jo::self<jo::Sound3D>(this)->cone_outer_gain);  // alSourcef
    }

    // 0x005a06f0
    void set_doppler(std::uint32_t bits) {
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_sound3dsystem(this)->is_initialized()) == 0) return;  // 0x005a1110
        jo::view<std::uint32_t>(jo::self<jo::Sound3DSystem>(this)->doppler_factor) = bits;
        cdecl_import(d::iat_OpenAL32_alDopplerFactor, jo::self<jo::Sound3DSystem>(this)->doppler_factor);  // alDopplerFactor
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("Sound3d::set_rolloff", 0x0059f810, &H::set_rolloff);
B("Listener::set_doppler", 0x005a06f0, &H::set_doppler);
#undef B

}  // namespace game_b97_05
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
//
//    - AlSource::set_angle 0x0059f860 (v), ret 4: +0x18 = v, alSourcef(+0, 0x1001, v / k).
//
//    - Listener3d::set_orient 0x005a0110 (desc), ret 4: at / up vectors from the two angles,
//      0x005a0050, then position 0x0059ff70(desc+4 - desc->f1c * at).
//
//    - Struct2c::zero 0x005a02d0 (ret 0): memset 0x2c, returns this.
namespace {
namespace game_b100_10 {

// cdecl import (OpenAL) through its IAT slot.
template <class R = void, class... A>
R c_import(img::Addr slot, A... args) {
    return reinterpret_cast<R(JOAN_CDECL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}
using joan::orig::helpers::memset_;  // memset

JOAN_COUNTER(c_orient, "Listener3d::set_orient");

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12);

struct H {

    void* zero_2c() {  // 0x005a02d0
        memset_(this, 0, static_cast<u32>(sizeof(jo::Sound3DSystem)));  // 0x2c on x86 (four pointers: wider on x64)
        return this;
    }

    // ------------------------------------------------------------------------ 0x0059f860
    void set_angle(u32 v) {
        jo::view<u32>(jo::self<jo::Sound3D>(this)->cone_inner_angle) = v;
        const float arg = jo::self<jo::Sound3D>(this)->cone_inner_angle / global<float>(d::flt_0_017453292);
        c_import<void>(kAlSourcef, jo::self<jo::Sound3D>(this)->al_source, 0x1001u, arg);
    }

    // ------------------------------------------------------------------------ 0x005a0110
    void set_orient(u8* desc) {
        c_orient.hit();
        const float c1 = joan::orig::arg::as<float>(joan::orig::libc::cos_f(joan::orig::arg::f32(jo::view<u32>(jo::as_cam_params(desc)->orbit[1]))));  // 0x00425670
        const float c2 = joan::orig::arg::as<float>(joan::orig::libc::cos_f(joan::orig::arg::f32(jo::view<u32>(jo::as_cam_params(desc)->orbit[0]))));  // 0x00425670
        const float s1 = joan::orig::arg::as<float>(joan::orig::libc::sin_f(joan::orig::arg::f32(jo::view<u32>(jo::as_cam_params(desc)->orbit[1]))));  // 0x004256a0
        const float s2 = joan::orig::arg::as<float>(joan::orig::libc::sin_f(joan::orig::arg::f32(jo::view<u32>(jo::as_cam_params(desc)->orbit[0]))));  // 0x004256a0
        Vec3 fwd;
        fwd.x = c1 * c2;
        fwd.y = c1 * s2;
        fwd.z = -s1;
        Vec3 up;
        up.x = s1 * c2;
        up.y = s1 * s2;
        std::memcpy(&up.z, &c1, 4);
        joan::orig::as_sound3dlistener(this)->set_orientation(&fwd, &up);  // 0x005a0050
        Vec3 t50;
        const Vec3* r = joan::orig::arg::as<const Vec3*>(joan::orig::vec_scaled(&t50, joan::orig::arg::f32(jo::view<u32>(jo::as_cam_params(desc)->orbit[3])), &fwd));  // 0x0060cd60
        Vec3 scaled = *r;
        Vec3 t5c;
        r = joan::orig::arg::as<const Vec3*>(joan::orig::vec_sub(&t5c, joan::orig::arg::ptr(jo::as_cam_params(desc)->position), &scaled));  // 0x0060cc90
        Vec3 pos = *r;
        joan::orig::as_sound3dlistener(this)->set_position(&pos);  // 0x0059ff70
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("AlSource::set_angle", 0x0059f860, &H::set_angle);
B("Listener3d::set_orient", 0x005a0110, &H::set_orient);
B("Struct2c::zero", 0x005a02d0, &H::zero_2c);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
//
//    - Obj::self_59d190 0x0059d190 (returns this), Sound3d::nop 0x0059fe90.
//
//    - Sound3d::set_distance 0x0059f8c0 (float), ret 4: +0x1c = v; alSourcef(+0, 0x1002,
//      v / [0x9cf884]) (OpenAL import, cdecl).
namespace {
namespace game_b94_05 {


struct H {

    // 0x0059fe90
    void nop_59fe90() {}

    // 0x0059f8c0
    void set_distance(std::uint32_t bits) {
        jo::view<std::uint32_t>(jo::self<jo::Sound3D>(this)->cone_outer_angle) = bits;
        const float v = jo::self<jo::Sound3D>(this)->cone_outer_angle / global<float>(d::flt_0_017453292);
        using AlSourcef = void(JOAN_CDECL*)(std::uint32_t, std::int32_t, float);
        reinterpret_cast<AlSourcef>(global<std::uintptr_t>(d::iat_OpenAL32_alSourcef))(jo::self<jo::Sound3D>(this)->al_source, 0x1002, v);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("Sound3d::set_distance", 0x0059f8c0, &H::set_distance);
B("Sound3d::nop", 0x0059fe90, &H::nop_59fe90);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/en3d_b83_08.cpp (game/b83_08)
//
//      0x0059fb40 Sound3D::set_position(const float v[3]) (ret 4) - +0x2f, alSource3f(AL_POSITION)
//
//      0x0059fae0 Sound3D::set_relative(byte) (ret 4)  - +0x2e, alSourcei(AL_SOURCE_RELATIVE, != 0)
//
//      0x0059fa80 Sound3D::set_looping(byte) (ret 4)   - +0x2d, alSourcei(AL_LOOPING, != 0)
//
//      0x005a0ad0 Sound3DBuffer scalar deleting destructor (0x0059ed90) (ret 4)
namespace {
namespace en3d_b83_08 {

// OpenAL32 (cdecl) import through its IAT slot.
using joan::orig::helpers::al_call;

constexpr img::Addr kAlSource3f = d::iat_OpenAL32_alSource3f;
constexpr img::Addr kAlSourcei = d::iat_OpenAL32_alSourcei;

using joan::orig::helpers::game_delete;                      // operator delete

struct H {

    // 0x0059fb40 (dwords at +0x2f, unaligned)
    void set_position(const std::uint32_t* v) {
        std::uint8_t* const dst = reinterpret_cast<std::uint8_t*>(jo::self<jo::Sound3D>(this)->position);
        std::memcpy(dst, v, 4);
        std::memcpy(dst + 4, v + 1, 4);
        std::memcpy(dst + 8, v + 2, 4);
        al_call(kAlSource3f, jo::self<jo::Sound3D>(this)->al_source, 0x1004, jo::self<jo::Sound3D>(this)->position[0], jo::self<jo::Sound3D>(this)->position[1], jo::self<jo::Sound3D>(this)->position[2]);
    }

    // 0x0059fae0
    void set_relative(std::uint8_t v) {
        jo::self<jo::Sound3D>(this)->relative = v;
        al_call(kAlSourcei, jo::self<jo::Sound3D>(this)->al_source, 0x202, jo::self<jo::Sound3D>(this)->relative != 0 ? 1 : 0);
    }

    // 0x0059fa80
    void set_looping(std::uint8_t v) {
        jo::self<jo::Sound3D>(this)->looping = v;
        al_call(kAlSourcei, jo::self<jo::Sound3D>(this)->al_source, 0x1007, jo::self<jo::Sound3D>(this)->looping != 0 ? 1 : 0);
    }

    // 0x0059f9c0 / 0x0059f970 / 0x0059f920 (float argument kept as raw bits)
    void set_rolloff(std::uint32_t v) {
        jo::view<std::uint32_t>(jo::self<jo::Sound3D>(this)->rolloff) = v;
        al_call(kAlSourcef, jo::self<jo::Sound3D>(this)->al_source, 0x1021, jo::self<jo::Sound3D>(this)->rolloff);
    }
    void set_reference_distance(std::uint32_t v) {
        jo::view<std::uint32_t>(jo::self<jo::Sound3D>(this)->reference_distance) = v;
        al_call(kAlSourcef, jo::self<jo::Sound3D>(this)->al_source, 0x1020, jo::self<jo::Sound3D>(this)->reference_distance);
    }
    void set_max_distance(std::uint32_t v) {
        jo::view<std::uint32_t>(jo::self<jo::Sound3D>(this)->max_distance) = v;
        al_call(kAlSourcef, jo::self<jo::Sound3D>(this)->al_source, 0x1023, jo::self<jo::Sound3D>(this)->max_distance);
    }

    // 0x005a0ad0
    void* buffer_delete(std::uint32_t flags) {
        joan::orig::as_sound3d(this)->destruct();  // 0x0059ed90
        if ((flags & 1u) != 0) game_delete(this);
        return this;
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b83_08", name, addr, fn)
GAME("Sound3D::set_position", 0x0059fb40, &H::set_position);
GAME("Sound3D::set_relative", 0x0059fae0, &H::set_relative);
GAME("Sound3D::set_looping", 0x0059fa80, &H::set_looping);
GAME("Sound3D::set_rolloff", 0x0059f9c0, &H::set_rolloff);
GAME("Sound3D::set_reference_distance", 0x0059f970, &H::set_reference_distance);
GAME("Sound3D::set_max_distance", 0x0059f920, &H::set_max_distance);
GAME("Sound3DBuffer::scalar_delete", 0x005a0ad0, &H::buffer_delete);
#undef GAME

}  // namespace en3d_b83_08
}  // namespace

// ---------------------------------------- from modules/game_b102_09.cpp (game/b102_09)
//
//    - AlSource::set_mute 0x0059fa10 (byte), AlSource::set_velocity 0x0059fbb0 (vec*),
//      AlSource::set_direction 0x0059fc20 (vec*), AlListener::set_velocity 0x0059ffe0 (vec*), all
//      ret 4, OpenAL calls through the IAT (cdecl), float bits passed as dwords.
namespace {
namespace game_b102_09 {


struct H {

    // ------------------------------------------------------------------------ 0x0059fa10
    void set_mute(u32 m) {
        jo::self<jo::Sound3D>(this)->muted = static_cast<u8>(m);
        const u32 gain = jo::self<jo::Sound3D>(this)->muted != 0 ? 0u : jo::view<u32>(jo::self<jo::Sound3D>(this)->gain);
        import_call<void>(d::iat_OpenAL32_alSourcef, jo::self<jo::Sound3D>(this)->al_source, 0x100a, std::bit_cast<float>(gain));  // alSourcef(AL_GAIN)
    }

    // ------------------------------------------------------------------------ 0x0059fbb0 etc.
    static void copy_vec(u32* d, const u32* v) {
        d[0] = v[0];
        d[1] = v[1];
        d[2] = v[2];
    }
    void set_velocity(const u32* v) {
        copy_vec(&jo::view<u32>(jo::self<jo::Sound3D>(this)->velocity[0]), v);
        import_call<void>(d::iat_OpenAL32_alSource3f, jo::self<jo::Sound3D>(this)->al_source, 0x1006, jo::self<jo::Sound3D>(this)->velocity[0], jo::self<jo::Sound3D>(this)->velocity[1], jo::self<jo::Sound3D>(this)->velocity[2]);  // alSource3f
    }
    void set_direction(const u32* v) {
        copy_vec(&jo::view<u32>(jo::self<jo::Sound3D>(this)->direction[0]), v);
        import_call<void>(d::iat_OpenAL32_alSource3f, jo::self<jo::Sound3D>(this)->al_source, 0x1005, jo::self<jo::Sound3D>(this)->direction[0], jo::self<jo::Sound3D>(this)->direction[1], jo::self<jo::Sound3D>(this)->direction[2]);  // alSource3f
    }
    void listener_velocity(const u32* v) {
        copy_vec(&jo::view<u32>(jo::self<jo::Listener>(this)->velocity[0]), v);
        import_call<void>(d::iat_OpenAL32_alListener3f, 0x1006, jo::self<jo::Listener>(this)->velocity[0], jo::self<jo::Listener>(this)->velocity[1], jo::self<jo::Listener>(this)->velocity[2]);  // alListener3f
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_09", name, addr, fn)
B("AlSource::set_mute", 0x0059fa10, &H::set_mute);
B("AlSource::set_velocity", 0x0059fbb0, &H::set_velocity);
B("AlSource::set_direction", 0x0059fc20, &H::set_direction);
B("AlListener::set_velocity", 0x0059ffe0, &H::listener_velocity);
#undef B

}  // namespace game_b102_09
}  // namespace

// ---------------------------------------- from modules/game_b111_14.cpp (fix/uninit)
//
//    - 0x0059fc90 Sound3D::state() (thiscall, ret 0): alGetSourcei(AL_SOURCE_STATE) into the
//      stack dword [ebp-8]; OpenAL does not write it when the call fails (invalid source / no
//      context) and the original then switches on garbage. Fix: starts at 0 (-> default case,
//      returns 0). eax reproduced in full: the state's upper 24 bits stay in eax (`mov al, n`).
namespace {
namespace game_b111_14 {

// Imported cdecl function through its IAT slot (OpenAL).
template <class R = void, class... A>
R import_cdecl(img::Addr slot, A... args) {
    return reinterpret_cast<R(JOAN_CDECL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}

struct H {
    // 0x0059fc90 (fix/uninit)
    u32 sound_state() {
        u32 v = 0;  // [ebp-8]: FIX - not written by OpenAL when alGetSourcei fails (garbage)
        import_cdecl(d::iat_OpenAL32_alGetSourcei, jo::self<jo::Sound3D>(this)->al_source, i32{0x1010}, &v);  // alGetSourcei(AL_SOURCE_STATE)
        const u32 hi = v & 0xffffff00u;
        const u32 c = v - 0x1011u;
        if (c > 3) return hi;  // xor al, al
        return hi | (c + 1);   // mov al, 1..4
    }
};

// fix/uninit: alGetSourcei out-value [ebp-8] unwritten on failure (starts at 0 -> returns 0).
JOAN_REPLACE("fix/uninit", "Sound3D::state", 0x0059fc90, &H::sound_state);

}  // namespace game_b111_14
}  // namespace

// ---------------------------------------- from modules/game_b111_17.cpp (fix/uninit)
//
//    - Sound3D::processed 0x0059fd20 (thiscall, ret 0): alGetSourcei(+0, AL_BUFFERS_PROCESSED,
//      &n). When OpenAL rejects the call (invalid source / no context) n is never written and
//      the original returns stack garbage. The fix starts n at 0 (no processed buffers).
namespace {
namespace game_b111_17 {


struct H {

    // -------------------------------------------------------------- 0x0059fd20
    i32 sound_processed() {
        i32 n = 0;  // fix/uninit: left unwritten by a rejected alGetSourcei in the original
        import_call<void>(d::iat_OpenAL32_alGetSourcei, jo::self<jo::Sound3D>(this)->al_source, i32{0x1016}, &n);  // alGetSourcei(AL_BUFFERS_PROCESSED)
        return n;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: alGetSourcei out-param starts at 0 (garbage when OpenAL rejects the call).
FIX("Sound3D::processed", 0x0059fd20, &H::sound_processed);
#undef FIX

}  // namespace game_b111_17
}  // namespace

// ---------------------------------------- from modules/game_b111_19.cpp (fix/uninit)
//
//    - Sound3D::queued_59fd60 0x0059fd60 (thiscall, ret 0): returns the alGetSourcei
//      (AL_BUFFERS_QUEUED) out value, never written when OpenAL rejects the source name. Fix:
//      the out value starts at 0 (no buffers queued).
namespace {
namespace game_b111_19 {


// ---- fix/uninit: 0x00433af0 ------------------------------------------------------------------
struct H {
    // ---- fix/uninit: 0x0059fd60 --------------------------------------------------------------
    i32 sound_queued() {
        // fix/uninit: alGetSourcei leaves the out value unwritten on an OpenAL error; 0 here.
        i32 v = 0;  // [ebp-0x8]
        import_call<void>(d::iat_OpenAL32_alGetSourcei, jo::self<jo::Sound3D>(this)->al_source, i32{0x1015}, &v);  // alGetSourcei(AL_BUFFERS_QUEUED)
        return v;
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: 0x0059fd60 - alGetSourcei out value unwritten on error -> 0.
F("Sound3D::queued_59fd60", 0x0059fd60, &H::sound_queued);
#undef F

}  // namespace game_b111_19
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
    void stub1(u32) {}
    void stub3(u32, u32, u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Obj::stub_59fda0", 0x0059fda0, &H::stub3);
B("Obj::stub_59fdc0", 0x0059fdc0, &H::stub3);
B("Obj::stub_5a0250", 0x005a0250, &H::stub3);
B("Obj::stub_5a0270", 0x005a0270, &H::stub3);
B("Obj::stub_5a0290", 0x005a0290, &H::stub1);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
//
//    - Obj::call_598ac0 0x00598a90, Obj::self_59ec90 0x0059ec90, Obj::nop_5a0830 0x005a0830.
//
//    - Obj::ctor_59fde0 0x0059fde0: memset(this, 0, 0x35), returns this.
namespace {
namespace game_b98_05 {

struct H {
    // 0x0059fde0
    void* ctor_59fde0() {
        std::memset(static_cast<void*>(this), 0, 0x35u);
        return this;
    }

    // 0x005a0830
    void nop_5a0830() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("Obj::ctor_59fde0", 0x0059fde0, &H::ctor_59fde0);
B("Obj::nop_5a0830", 0x005a0830, &H::nop_5a0830);
#undef B

}  // namespace game_b98_05
}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
//
//    - Obj::call_6e0520 0x0056e280, Obj::call_592830 0x00592350, Sound3d::call_nop 0x0059fe10:
//      thiscall wrappers (callee's eax returned).
namespace {
namespace game_b96_05 {

struct H {
    // 0x0059fe10
    std::uint32_t call_nop() { joan::orig::as_sound3dlistener(this)->shutdown(); return 0; }  // 0x0059fe90 (the only caller discards the result)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("Sound3d::call_nop", 0x0059fe10, &H::call_nop);
#undef B

}  // namespace game_b96_05
}  // namespace

// ---------------------------------------- from modules/game_b101_11.cpp (game/b101_11)
//
//    - Light::reset_dir 0x0059fe40 (ret 0): 0x0059feb0(1.0f), builds a 0x28-byte descriptor on
//      the stack with 0x00616d50 and hands it to 0x005a0110.
namespace {
namespace game_b101_11 {

struct H {
    // ------------------------------------------------------------------ 0x0059fe40
    void reset_dir() {
        joan::orig::as_sound3dlistener(this)->set_gain(joan::orig::arg::f32(0x3f800000u));  // 0x0059feb0
        u32 desc[10];
        joan::orig::as_camera_params(desc)->construct();               // 0x00616d50 (memset 0x28 + fields)
        joan::orig::as_sound3dlistener(this)->set_from_view(joan::orig::arg::mptr(static_cast<void*>(desc)));  // 0x005a0110
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_11", name, addr, fn)
B("Light::reset_dir", 0x0059fe40, &H::reset_dir);
#undef B

}  // namespace game_b101_11
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - Listener::set_pos 0x0059ff70 (vec*), ret 4: +5 = *vec, alListener3f(AL_POSITION, ...).
//
//    - Listener::set_doppler 0x005a0790 (v), ret 4: if 0x5a1110: +0xe = v, alDopplerVelocity.
//
//  - Void functions whose eax is garbage (0x593190, 0x59ff70, 0x5a0790, 0x5aa980, 0x5aef90,
//    0x5bfaf0, 0x5f45c0; callers checked) return void; the others return what eax holds.
namespace {
namespace game_b99_07 {

// cdecl import (OpenAL) through its IAT slot.
template <class R = void, class... A>
R cimport_call(img::Addr slot, A... args) {
    return reinterpret_cast<R(JOAN_CDECL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}

constexpr img::Addr kAlListener3f = d::iat_OpenAL32_alListener3f;
constexpr img::Addr kAlDopplerVelocity = d::iat_OpenAL32_alDopplerVelocity;

struct H {

    // ------------------------------------------------------------------ 0x0059ff70
    void listener_set_pos(const u32* v) {
        jo::view<u32>(jo::self<jo::Listener>(this)->position[0]) = v[0];
        jo::view<u32>(jo::self<jo::Listener>(this)->position[1]) = v[1];
        jo::view<u32>(jo::self<jo::Listener>(this)->position[2]) = v[2];
        cimport_call(kAlListener3f, 0x1004, jo::self<jo::Listener>(this)->position[0], jo::self<jo::Listener>(this)->position[1], jo::self<jo::Listener>(this)->position[2]);
    }

    // ------------------------------------------------------------------ 0x005a0790
    void listener_set_doppler(u32 v) {
        if (static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_sound3dsystem(this)->is_initialized())) == 0) return;  // 0x005a1110
        jo::view<u32>(jo::self<jo::Sound3DSystem>(this)->doppler_velocity) = v;
        cimport_call(kAlDopplerVelocity, jo::self<jo::Sound3DSystem>(this)->doppler_velocity);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Listener::set_pos", 0x0059ff70, &H::listener_set_pos);
B("Listener::set_doppler", 0x005a0790, &H::listener_set_doppler);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b115_10.cpp (fix/uninit)
//
//    - Dead_5a02b0 0x005a02b0 (thiscall, ret 0, no references): returns an uninitialised local.
//
//  - 0x005a02b0: [ebp-0x8] -> 0.
namespace {
namespace game_b115_10 {

struct H {
    // ===================================================================== 0x005a02b0
    u32 dead_5a02b0() { return 0; }  // fix: the original returns the uninitialised [ebp-0x8]
};

// 0x005a02b0: returns the uninitialised [ebp-0x8] -> 0 (dead code).
JOAN_REPLACE("fix/uninit", "Dead_5a02b0", 0x005a02b0, &H::dead_5a02b0);

}  // namespace game_b115_10
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
//
//    - Sound3d::fwd_5a0300 0x005a0300 (ret 0): 0x005a04e0 (eax returned).
namespace {
namespace game_b108_15 {

struct H {
    // ------------------------------------------------------------------ 0x005a0300
    std::uintptr_t fwd_5a0300() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_sound3d(this)->release_5a04e0()); }  // 0x005a04e0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Sound3d::fwd_5a0300", 0x005a0300, &H::fwd_5a0300);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
//
//    - Sound3D::init_5a0330 0x005a0330 (thiscall, ret 0): OpenAL start-up (alutInit, alGetError ->
//      message box / exit), listener objects; eax of the last call.
namespace {
namespace game_b108_09 {

// cdecl import through its IAT slot.
template <class R, class... A>
R import_cdecl(img::Addr slot, A... args) {
    return reinterpret_cast<R(JOAN_CDECL*)(A...)>(global<std::uintptr_t>(slot))(args...);
}

JOAN_COUNTER(c_5a0330, "Sound3D::init_5a0330");

struct H {
    // 0x005a0330
    u32 sound_init() {
        c_5a0330.hit();
        void* const mem = obj(d::g_mem);
        const char* const file = str(d::s_d_ww_ilib_sound3d_cpp);
        jo::self<jo::Sound3DSystem>(this)->device = joan::orig::as_mem(mem)->add_clear(10u, joan::orig::arg::str(file), joan::orig::arg::i32(global<i16>(d::g_a1687c) + 4));  // 0x0058e8d0
        joan::orig::alut_init(joan::orig::arg::mptr(0), joan::orig::arg::mptr(0));
        const i32 err = import_cdecl<i32>(d::iat_OpenAL32_alGetError);  // alGetError
        if (err != 0) {
            char text[1000];
            text[0] = 0;
            import_cdecl<const char*>(d::iat_OpenAL32_alGetString, err);  // alGetString (result unused)
            if (joan::orig::arg::as<i32>(joan::orig::as_game(obj(d::g_box))->message_box(joan::orig::arg::str(text), 0u, 0x44u)) != 0) {  // 0x005a8be0
                joan::orig::as_mem(mem)->del(joan::orig::arg::mptr(jo::self<jo::Sound3DSystem>(this)->device), joan::orig::arg::str(file), joan::orig::arg::i32(global<i16>(d::g_a1687c) + 0x21));  // 0x0058ed70
                jo::self<jo::Sound3DSystem>(this)->device = nullptr;
                return 0;  // (the only caller discards the result)
            }
            joan::orig::as_mem(mem)->del(joan::orig::arg::mptr(jo::self<jo::Sound3DSystem>(this)->device), joan::orig::arg::str(file), joan::orig::arg::i32(global<i16>(d::g_a1687c) + 0x27));
            jo::self<jo::Sound3DSystem>(this)->device = nullptr;
            joan::orig::libc::exit(joan::orig::arg::u32(-3));  // exit
        }
        jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->listener) = joan::orig::as_mem(mem)->add(0x35u, joan::orig::arg::str(file), joan::orig::arg::i32(global<i16>(d::g_a1687c) + 0x3c));  // 0x0058e7b0
        joan::orig::as_sound3dlistener(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->listener))->construct();  // 0x0059fde0
        joan::orig::as_light(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->listener))->reset_dir();  // 0x0059fe40
        joan::orig::as_sound(this)->set_distance_model(static_cast<char>(joan::orig::arg::i32(global<i8>(d::g_flag2) != 0 ? 2 : 0)));  // 0x00454000
        joan::orig::as_sound3dsystem(this)->set_doppler_factor(0u);                                     // 0x005a06f0
        joan::orig::as_sound3dsystem(this)->set_doppler_velocity(0x43ab8000u);               // 0x005a0790 (343.0f)
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("Sound3D::init_5a0330", 0x005a0330, &H::sound_init);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
//
//    - Sound3D::release_5a04e0 0x005a04e0, ret 0: returns this (0 when 0x005a1110 is false; the
//      original leaves 0x0058ed70's eax when +0x12 was freed - both callers, 0x004d78c9 and the
//      atexit wrapper 0x005a0300 / 0x00567080, ignore eax).
namespace {
namespace game_b108_08 {

using joan::orig::helpers::add32;

JOAN_COUNTER(c_release, "Sound3D::release_5a04e0");

// ---------------------------------------------------------------- thiscall functions
struct H {

    // 0x005a04e0
    void* release_5a04e0() {
        c_release.hit();
        if ((joan::orig::arg::as<u32>(joan::orig::as_sound3dsystem(this)->is_initialized()) & 0xffu) == 0) return nullptr;  // 0x005a1110
        if (jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->listener) != nullptr) {
            joan::orig::as_sound3dlistener(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->listener))->shutdown();                       // 0x0059fe90
            joan::orig::as_sound3dlistener(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->listener))->destroy(joan::orig::arg::u32(static_cast<i32>(0)));  // 0x004350b0
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->listener)), str(d::s_d_ww_ilib_sound3d_cpp), joan::orig::arg::i32(// 0x0058ed70
                     add32(global<i16>(d::g_a16880), 0xa)));
        }
        if (jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->sources) != nullptr) {
            for (i16 i = static_cast<i16>(jo::self<jo::Sound3DSystem>(this)->source_count - 1); i >= 0; i = static_cast<i16>(i - 1))
                joan::orig::as_sound3dsystem(this)->delete_source(jo::self<jo::Sound3DSystem>(this)->sources[i].item);  // 0x005a0cf0
        }
        if (jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->buffers) != nullptr) {
            for (i16 i = static_cast<i16>(jo::self<jo::Sound3DSystem>(this)->buffer_count - 1); i >= 0; i = static_cast<i16>(i - 1))
                joan::orig::as_sound3d(this)->release_buffer(jo::self<jo::Sound3DSystem>(this)->buffers[i].item);  // 0x00490bc0
        }
        joan::orig::alut_exit();
        joan::orig::as_sound3dsystem(this)->on_shutdown();  // 0x005a0830
        if (jo::self<jo::Sound3DSystem>(this)->device != nullptr) {
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::self<jo::Sound3DSystem>(this)->device), str(d::s_d_ww_ilib_sound3d_cpp), joan::orig::arg::i32(add32(global<i16>(d::g_a16880), 0x2a)));
            jo::self<jo::Sound3DSystem>(this)->device = nullptr;
            jo::self<jo::Sound3DSystem>(this)->u16_16 = 0;
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Sound3D::release_5a04e0", 0x005a04e0, &H::release_5a04e0);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (game/b82_17)
//
//      0x005a0870 Cache::find(key) (ret 4)       - binary search (0x005911a0) of +0x18
namespace {
namespace en3d_b82_17 {


void* const kFiles = obj(d::g_misc);   // array helper object (0x005911a0 / 0x005910f0)

JOAN_COUNTER(c_cache_find, "Cache::find");

struct H {

    // 0x005a0870
    void* cache_find(std::uint32_t key) {
        c_cache_find.hit();
        if ((joan::orig::arg::as<std::uint32_t>(joan::orig::as_sound3dsystem(this)->is_initialized()) & 0xffu) == 0) return nullptr;  // 0x005a1110
        std::uint32_t buffer[0x48];  // only the first dword is read by the comparator 0x005a10b0
        buffer[0] = key;
        std::uint32_t* key_ptr = buffer;
        const std::int16_t r = static_cast<std::int16_t>(thiscall<std::int32_t>(
            fn::bin_search, kFiles, static_cast<void*>(&key_ptr), jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->buffers), static_cast<std::int32_t>(jo::self<jo::Sound3DSystem>(this)->buffer_count), static_cast<std::int32_t>(sizeof(void*)),
            fn::code_ptr(fn::cmp_ptr_u32_b), static_cast<void*>(nullptr)));  // 0x005911a0
        if (static_cast<std::int32_t>(r) <= 0) return nullptr;
        return static_cast<void**>(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->buffers))[static_cast<std::int32_t>(r) - 1];
    }
};

#define B82_17G(name, addr, fn) JOAN_REPLACE("game/b82_17", name, addr, fn)
B82_17G("Cache::find", 0x005a0870, &H::cache_find);
#undef B82_17G

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
//
//    - 0x005a0cf0 Sound3d::remove_source     (thiscall(src), ret 4): bsearch 0x005911a0 on the
//      +0x20 array (key = address of the argument), 0x0059f400 / 0x005a0e90(0) / free
//      (sound3d.cpp) of the source, array removal 0x00591070, shrink by 32 (free or realloc),
//      then alDeleteSources(1, &id) through IAT 0xa6dbc0 (its eax returned).
namespace {
namespace game_b108_06 {


void* const kRandom = obj(d::g_misc);

JOAN_COUNTER(c_remove_source, "Sound3d::remove_source");

struct H {

    // ------------------------------------------------------------------ 0x005a0cf0
    u32 remove_source(void* src) {
        const u32 ok = joan::orig::arg::as<u32>(joan::orig::as_sound3dsystem(this)->is_initialized()) & 0xffu;  // 0x005a1110
        if (ok == 0) return ok;
        c_remove_source.hit();
        void* key = src;  // the argument slot, passed by address
        i32 insert_at = 0;  // [ebp-0x8], written by the search only, never read
        const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_misc(kRandom)->bin_search_5911a0(&key, joan::orig::arg::str(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->sources)), static_cast<i32>(jo::self<jo::Sound3DSystem>(this)->source_count), static_cast<i32>(sizeof(jo::Sound3DPtrArray)), fn::code_value(fn::cmp_ptr_u32), &insert_at));  // 0x005911a0 (compare 0x005a1060)
        u32 id = jo::as_sound3d(key)->al_source;  // [ebp-0x10]
        joan::orig::as_sound3d(key)->stop_and_unqueue();     // 0x0059f400
        joan::orig::as_sound3d(key)->destroy(0u);  // 0x005a0e90
        joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(key), str(d::s_d_ww_ilib_sound3d_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a16890)) + 0x11));  // free
        joan::orig::as_misc(kRandom)->del_array_rec_591070(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->sources)), static_cast<i32>(jo::self<jo::Sound3DSystem>(this)->source_count), static_cast<i32>(sizeof(jo::Sound3DPtrArray)), joan::orig::arg::i32(idx));  // 0x00591070
        jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->source_count) = static_cast<u16>(jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->source_count) - 1);
        if (static_cast<i32>(jo::self<jo::Sound3DSystem>(this)->source_capacity) - static_cast<i32>(jo::self<jo::Sound3DSystem>(this)->source_count) >= 0x20) {
            jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->source_capacity) = static_cast<u16>(jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->source_capacity) - 0x20);
            if (jo::self<jo::Sound3DSystem>(this)->source_capacity <= 0) {
                joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->sources)), str(d::s_d_ww_ilib_sound3d_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a16890)) + 0x23));  // 0x0058ed70 free
                jo::self<jo::Sound3DSystem>(this)->sources = nullptr;
                jo::view<u16>(jo::self<jo::Sound3DSystem>(this)->source_capacity) = 0;
            } else {
                jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->sources) = joan::orig::as_mem(kHeap)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Sound3DSystem>(this)->sources)), joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(static_cast<i32>(jo::self<jo::Sound3DSystem>(this)->source_capacity)) * static_cast<u32>(sizeof(jo::Sound3DPtrArray)))), str(d::s_d_ww_ilib_sound3d_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a16890)) + 0x29));  // 0x0058eb30 realloc
            }
        }
        return import_call<u32>(d::iat_OpenAL32_alDeleteSources, 1, &id);  // alDeleteSources
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("Sound3d::remove_source", 0x005a0cf0, &H::remove_source);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/en3d_b83_08.cpp (game/b83_08)
//
//      0x0059f460 Sound3D::queue(buffer) (ret 4)       - alSourceQueueBuffers(1), word buffer+4 += 1
//
//      0x0059ede0 Sound3D::cleanup()                   - empty
//
//      0x005a1110 Object::has_12()                     - dword +0x12 != 0 (byte)
namespace {
namespace en3d_b83_08_2 {

using joan::hook::global;

// OpenAL32 (cdecl) import through its IAT slot.
using joan::orig::helpers::al_call;

constexpr img::Addr kAlSourceQueueBuffers = d::iat_OpenAL32_alSourceQueueBuffers;

struct H {

    // 0x0059f460 (eax = buffer)
    void* queue(void* buffer) {
        al_call(kAlSourceQueueBuffers, jo::self<jo::Sound3D>(this)->al_source, 1, buffer);
        jo::view<std::uint16_t>(jo::as_sound3dbuffer(buffer)->refs) = static_cast<std::uint16_t>(jo::view<std::uint16_t>(jo::as_sound3dbuffer(buffer)->refs) + 1);
        return buffer;
    }

    // 0x0059ede0
    void cleanup() {}

    // 0x005a1110
    std::uint8_t has_12() { return jo::self<jo::Sound3DSystem>(this)->device != nullptr ? 1 : 0; }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b83_08", name, addr, fn)
GAME("Sound3D::queue", 0x0059f460, &H::queue);
GAME("Object::has_12", 0x005a1110, &H::has_12);
GAME("Sound3D::cleanup", 0x0059ede0, &H::cleanup);
#undef GAME

}  // namespace en3d_b83_08_2
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
//
//    - ListA::step 0x0059f400 (ret 0): eax of 0x0059f4b0.
namespace {
namespace game_b108_03 {

using u32 = std::uint32_t;
using i8 = std::int8_t;
using i32 = std::int32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x0059f400
    u32 list_step() {
        joan::orig::as_sound3d(this)->stop();  // 0x0059f680
        if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_sound3d(this)->state())) == 1) {  // 0x0059fc90
            joan::orig::as_sound(this)->play();  // 0x0059f620
            joan::orig::as_sound3d(this)->stop();  // 0x0059f680
        }
        const u32 v = joan::orig::arg::as<u32>(joan::orig::as_sound3d(this)->queued_59fd60());  // 0x0059fd60
        joan::orig::as_sound3d(this)->unqueue(joan::orig::arg::i32(v));      // 0x0059f4b0
        return 0;  // (no caller uses the result)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("ListA::step", 0x0059f400, &H::list_step);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
//
//      0x005a0e90 Obj5a0e90 deleting dtor       (thiscall, ret 4)   - returns this.
namespace {
namespace game_b108_13 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::op_delete;

struct H {
    // ------------------------------------------------------------------ 0x005a0e90
    void* ddtor_5a0e90(u32 flags) {
        joan::orig::as_sound3d(this)->destruct_59f310();  // 0x0059f310
        if ((flags & 1u) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("Obj5a0e90::deleting_dtor", 0x005a0e90, &H::ddtor_5a0e90);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b100_10_2.cpp (game/b100_10)
namespace {
namespace game_b100_10_2 {

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    void* zero_11a() {  // 0x0059ed50
        memset_(this, 0, 0x11a);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("Struct11a::zero", 0x0059ed50, &H::zero_11a);
#undef B

}  // namespace game_b100_10_2
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (game/b84_01)
namespace {
namespace en3d_b84_01 {

using joan::call::thiscall;

struct H {
    // 0x0059ed90
    void sound_buffer_dtor() { joan::orig::as_sound3d(this)->cleanup(); }  // 0x0059ede0
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b84_01", name, addr, fn)
GAME("Sound3DBuffer::~Sound3DBuffer", 0x0059ed90, &H::sound_buffer_dtor);
#undef GAME

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/game_b104_09.cpp (game/b104_09)
namespace {
namespace game_b104_09 {

struct H {
    void stub_void() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_09", name, addr, fn)
B("Obj::stub_59edc0", 0x0059edc0, &H::stub_void);
#undef B

}  // namespace game_b104_09
}  // namespace

// ---------------------------------------- from modules/game_b102_09_2.cpp (game/b102_09)
namespace {
namespace game_b102_09_2 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::str;

using joan::call::import_call;

JOAN_COUNTER(c_load_wav, "AlBuffer::load_wav");

struct H {

    // ------------------------------------------------------------------------ 0x0059ee00
    void* load_wav(const char* name) {
        c_load_wav.hit();
        void* data = nullptr;  // always written by alutLoadWAVFile
        u8* const self = reinterpret_cast<u8*>(this);
        joan::orig::alut_load_wavfile(joan::orig::arg::str(name), joan::orig::arg::mptr(&jo::self<jo::Sound3DBuffer>(this)->format), &data, joan::orig::arg::mptr(&jo::self<jo::Sound3DBuffer>(this)->size), joan::orig::arg::mptr(&jo::self<jo::Sound3DBuffer>(this)->frequency), joan::orig::arg::mptr(// alutLoadWAVFile
                         &jo::self<jo::Sound3DBuffer>(this)->loop));
        import_call<void>(d::iat_OpenAL32_alBufferData, jo::self<jo::Sound3DBuffer>(this)->al_buffer, jo::self<jo::Sound3DBuffer>(this)->format, data, jo::self<jo::Sound3DBuffer>(this)->size, jo::self<jo::Sound3DBuffer>(this)->frequency);  // alBufferData
        joan::orig::alut_unload_wav(joan::orig::arg::i32(jo::self<jo::Sound3DBuffer>(this)->format), joan::orig::arg::mptr(data), joan::orig::arg::i32(jo::self<jo::Sound3DBuffer>(this)->size), joan::orig::arg::i32(jo::self<jo::Sound3DBuffer>(this)->frequency));  // alutUnloadWAV
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(self + 7))), joan::orig::arg::str(name));  // strcpy
        jo::self<jo::Sound3DBuffer>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_09", name, addr, fn)
B("AlBuffer::load_wav", 0x0059ee00, &H::load_wav);
#undef B

}  // namespace game_b102_09_2
}  // namespace

// ---------------------------------------- from modules/game_b97_05_2.cpp (game/b97_05)
namespace {
namespace game_b97_05_2 {

using i32 = std::int32_t;

using joan::call::thiscall;

using joan::img::str;

JOAN_COUNTER(c_s3d_load, "Sound3d::load");
JOAN_COUNTER(c_s3d_kept, "Sound3d::load from memory (SoundsInMemory)");

struct alignas(4) Archive {  // 0x0047cdf0 / 0x0047cb60 (frame slot 0x138 bytes)
    std::uint8_t b[((sizeof(jo::ResourceIdxL) > 0x138 ? sizeof(jo::ResourceIdxL) : 0x138) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger ResourceIdxL
};

struct H {
    // 0x0059ef00 (SEH frame: normal path)
    // EAX: the archive dtor's `this`, i.e. the (dead) address of the local archive object.
    std::uintptr_t s3d_load(const char* path, const char* name, std::uint32_t flag) {  // returns a pointer-sized eax leftover (the facade returns std::uintptr_t)
        c_s3d_load.hit();
        // The original returns the address of its (destroyed) local archive: a non-zero value both callers only
        // pass on. Returning a dead local's address draws C4172, so this returns `this`, also non-zero.
        const std::uintptr_t r = reinterpret_cast<std::uintptr_t>(this);
        if ((flag & 0xffu) != 0) {
            // [Audio] SoundsInMemory (sound_archives.hpp): the record straight from the archive kept in memory. The
            // archive object below (open, read and sort the index, read the record, close) is temporary - all of it
            // freed again before the original returns - so the buffer and the game end as from disk.
            std::uint32_t kept_size = 0;
            if (const char* const kept = joan::sound_archives::find(path, name, kept_size)) {
                c_s3d_kept.hit();
                if (joan::hitch::enabled()) joan::hitch::note("sound %s from memory (%s)", name, path);  // [Debug] HitchLog
                joan::orig::as_sound3dbuffer(this)->load_wav(kept, joan::orig::arg::u32(kept_size));  // 0x0059f0b0
                return r;
            }
            if (joan::hitch::enabled()) joan::hitch::note("sound %s from disk (%s)", name, path);  // [Debug] HitchLog
            Archive a;
            joan::orig::as_archive(&a)->construct(path, 0u, 0u);                              // 0x0047cdf0
            const std::uint32_t e = joan::orig::arg::as<std::uint32_t>(joan::orig::as_res_idx_l(&a)->get_index(name));  // 0x0059be20
            const char* const rec = joan::orig::as_res_idx_l(&a)->read_rec(joan::orig::arg::i32(e));  // 0x0059bf70
            const std::uint32_t data = joan::orig::arg::as<std::uint32_t>(joan::orig::as_res_idx_l(&a)->get_data_size(joan::orig::arg::i32(e)));  // 0x0059bdd0
            joan::orig::as_sound3dbuffer(this)->load_wav(rec, joan::orig::arg::u32(data));                        // 0x0059f0b0
            joan::orig::as_archive(&a)->dtor();                                    // 0x00417a30
        } else {
            Archive a;
            joan::orig::as_anim_set(&a)->AnimSet3(path, 0, 0);                              // 0x0047cb60
            const std::uint32_t e = joan::orig::arg::as<std::uint32_t>(joan::orig::as_res_idx(&a)->get_index(name));  // 0x0059ca60
            const char* const rec = joan::orig::as_res_idx(&a)->read_rec(joan::orig::arg::i32(e));  // 0x0059cbb0
            const std::uint32_t data = joan::orig::arg::as<std::uint32_t>(joan::orig::as_res_idx(&a)->get_data_size(joan::orig::arg::i32(e)));  // 0x0059ca10
            joan::orig::as_sound3dbuffer(this)->load_wav(rec, joan::orig::arg::u32(data));                        // 0x0059f0b0
            joan::orig::as_res_idx_file(&a)->dtor_47cac0();                                    // 0x0047cac0
        }
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("Sound3d::load", 0x0059ef00, &H::s3d_load);
#undef B

}  // namespace game_b97_05_2
}  // namespace

// ---------------------------------------- from modules/game_b96_05_2.cpp (game/b96_05)
namespace {
namespace game_b96_05_2 {

using joan::hook::global;

using joan::call::cdecl_call;

// OpenAL32 (cdecl) import through its IAT slot.
using joan::orig::helpers::al_call;

constexpr img::Addr kAlBufferData = d::iat_OpenAL32_alBufferData;


JOAN_COUNTER(c_load_wav, "Sound3d::load_wav");

struct H {
    // 0x0059f0b0
    void load_wav(const void* file, std::uint32_t /*unused*/) {
        c_load_wav.hit();
        void* data;  // alutLoadWAVFile stores 0 here first
        joan::orig::alut_load_wavmemory(const_cast<void*>(file), joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Sound3DBuffer>(this)->format)), joan::orig::arg::mptr(static_cast<void*>(&data)), joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Sound3DBuffer>(this)->size)), joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Sound3DBuffer>(this)->frequency)), joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Sound3DBuffer>(this)->loop)));  // alutLoadWAVFile
        al_call(kAlBufferData, jo::self<jo::Sound3DBuffer>(this)->al_buffer, jo::self<jo::Sound3DBuffer>(this)->format, data, jo::self<jo::Sound3DBuffer>(this)->size,
                jo::self<jo::Sound3DBuffer>(this)->frequency);
        joan::orig::alut_unload_wav(joan::orig::arg::i32(jo::self<jo::Sound3DBuffer>(this)->format), joan::orig::arg::mptr(data), joan::orig::arg::i32(jo::self<jo::Sound3DBuffer>(this)->size), joan::orig::arg::i32(jo::self<jo::Sound3DBuffer>(this)->frequency));  // alutUnloadWAV
        jo::self<jo::Sound3DBuffer>(this)->u8_7 = 0;
        jo::self<jo::Sound3DBuffer>(this)->loaded = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("Sound3d::load_wav", 0x0059f0b0, &H::load_wav);
#undef B

}  // namespace game_b96_05_2
}  // namespace

// ---------------------------------------- from modules/en3d_b83_08_3.cpp (game/b83_08)
namespace {
namespace en3d_b83_08_3 {

using joan::hook::global;

// OpenAL32 (cdecl) import through its IAT slot.
using joan::orig::helpers::al_call;

constexpr img::Addr kAlBufferData = d::iat_OpenAL32_alBufferData;


JOAN_COUNTER(c_set_data, "Sound3DBuffer::set_data");

struct H {

    // 0x0059f1a0 (eax = this)
    void* set_data(const void* data, std::uint32_t size, std::int32_t channels, std::uint32_t freq, std::int32_t bits) {
        c_set_data.hit();
        if (channels == 1 && bits == 8)
            jo::self<jo::Sound3DBuffer>(this)->format = 0x1100;
        else if (channels == 2 && bits == 8)
            jo::self<jo::Sound3DBuffer>(this)->format = 0x1102;
        else if (channels == 1 && bits == 0x10)
            jo::self<jo::Sound3DBuffer>(this)->format = 0x1101;
        else if (channels == 2 && bits == 0x10)
            jo::self<jo::Sound3DBuffer>(this)->format = 0x1103;
        jo::self<jo::Sound3DBuffer>(this)->size = size;
        jo::self<jo::Sound3DBuffer>(this)->frequency = freq;
        al_call(kAlBufferData, jo::self<jo::Sound3DBuffer>(this)->al_buffer, jo::self<jo::Sound3DBuffer>(this)->format, data, jo::self<jo::Sound3DBuffer>(this)->size, jo::self<jo::Sound3DBuffer>(this)->frequency);
        jo::self<jo::Sound3DBuffer>(this)->u8_7 = 0;
        jo::self<jo::Sound3DBuffer>(this)->loaded = 1;
        return this;
    }

    // 0x0059f2b0 (eax = this)
    void* set_119(std::uint8_t v) {
        jo::self<jo::Sound3DBuffer>(this)->release_unused = v;
        return this;
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b83_08", name, addr, fn)
GAME("Sound3DBuffer::set_data", 0x0059f1a0, &H::set_data);
GAME("Sound3DBuffer::set_119", 0x0059f2b0, &H::set_119);
#undef GAME

}  // namespace en3d_b83_08_3
}  // namespace

// ---------------------------------------- from modules/game_b99_07_2.cpp (game/b99_07)
namespace {
namespace game_b99_07_2 {

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::orig::helpers::crt_memset;

struct H {
    // ------------------------------------------------------------------ 0x0059f2e0
    H* clear_53() {
        crt_memset(this, 0, 0x53);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Obj::clear_53", 0x0059f2e0, &H::clear_53);
#undef B

}  // namespace game_b99_07_2
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x0059f310
    u32 dtor_59f310() { return joan::orig::arg::as<u32>(joan::orig::as_sound3d(this)->stop_and_unqueue()); }  // 0x0059f400
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Obj59f::dtor_59f310", 0x0059f310, &H::dtor_59f310);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b101_11_2.cpp (game/b101_11)
namespace {
namespace game_b101_11_2 {

using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_light_defaults, "Light::set_defaults");

struct H {
    // ------------------------------------------------------------------ 0x0059f340
    void light_defaults() {
        c_light_defaults.hit();
        joan::orig::as_sound3d(this)->set_pitch(0x3f800000u);  // 0x0059f6b0
        joan::orig::as_sound3d(this)->set_gain(0x3f800000u);  // 0x0059f700
        joan::orig::as_sound3d(this)->set_min_gain(joan::orig::arg::f32(0x00000000u));  // 0x0059f770
        joan::orig::as_sound3d(this)->set_max_gain(joan::orig::arg::f32(0x3f800000u));  // 0x0059f7c0
        joan::orig::as_sound3d(this)->set_cone_outer_gain(0x3f800000u);  // 0x0059f810
        joan::orig::as_sound3d(this)->set_cone_inner_angle(0x40c90fdbu);  // 0x0059f860
        joan::orig::as_sound3d(this)->set_cone_outer_angle(0x40c90fdbu);  // 0x0059f8c0
        joan::orig::as_sound3d(this)->set_max_distance(0x4b189680u);  // 0x0059f920
        joan::orig::as_sound3d(this)->set_reference_distance(0x447a0000u);  // 0x0059f970
        joan::orig::as_sound3d(this)->set_rolloff(0x3f800000u);  // 0x0059f9c0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_11", name, addr, fn)
B("Light::set_defaults", 0x0059f340, &H::light_defaults);
#undef B

}  // namespace game_b101_11_2
}  // namespace

// ---------------------------------------- from modules/game_b103_10_2.cpp (game/b103_10)
namespace {
namespace game_b103_10_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

// 0x005a1060 / 0x005a10b0
static i32 JOAN_CDECL cmp_ptr_u32(u32* const* a, u32* const* b) {
    u32* const pa = *a;
    u32* const pb = *b;
    if (*pa < *pb) return -1;
    if (*pa > *pb) return 1;
    return 0;
}
static i32 JOAN_CDECL cmp_ptr_u32_b(u32* const* a, u32* const* b) {
    u32* const pa = *a;
    u32* const pb = *b;
    if (*pa < *pb) return -1;
    if (*pa > *pb) return 1;
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("cmp_ptr_u32", 0x005a1060, &cmp_ptr_u32);
B("cmp_ptr_u32_b", 0x005a10b0, &cmp_ptr_u32_b);
#undef B

}  // namespace game_b103_10_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Sound3D::create_buffer_48d120() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b111_10::H::create_buffer>(this)); }  // 0x48d120
auto joan::orig::Sound3D::alloc_source() -> Sound3D* { return joan::bind::conv<Sound3D*>(joan::bind::member<&::game_b111_12::H::alloc_source>(this)); }  // 0x4907e0
auto joan::orig::Sound3D::release_buffer(Sound3DBuffer* a0) -> void { joan::bind::member<&::game_b82_16::H::release_buffer>(this, a0); }  // 0x490bc0
auto joan::orig::AlBuffer::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_10_2::H::zero_11a>(this)); }  // 0x59ed50
auto joan::orig::Sound3D::destruct() -> void { joan::bind::member<&::en3d_b84_01::H::sound_buffer_dtor>(this); }  // 0x59ed90
auto joan::orig::Sound3DBuffer::on_create() -> void { joan::bind::member<&::game_b104_09::H::stub_void>(this); }  // 0x59edc0
auto joan::orig::Sound3D::cleanup() -> void { joan::bind::member<&::en3d_b83_08_2::H::cleanup>(this); }  // 0x59ede0
auto joan::orig::Sound3DBuffer::load(const char* a0, const char* a1, std::uint32_t a2) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b97_05_2::H::s3d_load>(this, a0, a1, a2)); }  // 0x59ef00
auto joan::orig::Sound3DBuffer::load_wav(const void* a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b96_05_2::H::load_wav>(this, a0, a1); }  // 0x59f0b0
auto joan::orig::Sound3DBuffer::set_data(const void* a0, std::uint32_t a1, std::int32_t a2, std::uint32_t a3, std::int32_t a4) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_08_3::H::set_data>(this, a0, a1, a2, a3, a4)); }  // 0x59f1a0
auto joan::orig::Sound3DBuffer::set_119(std::uint8_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_08_3::H::set_119>(this, a0)); }  // 0x59f2b0
auto joan::orig::Sound3D::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_07_2::H::clear_53>(this)); }  // 0x59f2e0
auto joan::orig::Sound3D::destruct_59f310() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_14::H::dtor_59f310>(this)); }  // 0x59f310
auto joan::orig::Light::set_defaults() -> void { joan::bind::member<&::game_b101_11_2::H::light_defaults>(this); }  // 0x59f340
auto joan::orig::Sound3D::stop_and_unqueue() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_03::H::list_step>(this)); }  // 0x59f400
auto joan::orig::Sound3D::queue(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_08_2::H::queue>(this, a0)); }  // 0x59f460
auto joan::orig::Sound3D::unqueue(std::int32_t a0) -> void { joan::bind::member<&::game_stream2::H::unqueue>(this, a0); }  // 0x59f4b0
auto joan::orig::Sound::play() -> void { joan::bind::member<&::game_leaves80a::H::play>(this); }  // 0x59f620
auto joan::orig::Sound::pause() -> void { joan::bind::member<&::game_leaves80a::H::pause>(this); }  // 0x59f650
auto joan::orig::Sound3D::stop() -> void { joan::bind::member<&::game_stream2::H::stop>(this); }  // 0x59f680
auto joan::orig::Sound3D::set_pitch(std::uint32_t a0) -> void { joan::bind::member<&::game_b95_05::H::set_pitch>(this, a0); }  // 0x59f6b0
auto joan::orig::Sound3D::set_gain(std::uint32_t a0) -> void { joan::bind::member<&::game_stream2::H::set_gain>(this, a0); }  // 0x59f700
auto joan::orig::Sound3D::set_min_gain(float a0) -> void { joan::bind::member<&::game_b93_03::H::al_set_min_gain>(this, a0); }  // 0x59f770
auto joan::orig::Sound3D::set_max_gain(float a0) -> void { joan::bind::member<&::game_b93_03::H::al_set_max_gain>(this, a0); }  // 0x59f7c0
auto joan::orig::Sound3D::set_cone_outer_gain(std::uint32_t a0) -> void { joan::bind::member<&::game_b97_05::H::set_rolloff>(this, a0); }  // 0x59f810
auto joan::orig::Sound3D::set_cone_inner_angle(std::uint32_t a0) -> void { joan::bind::member<&::game_b100_10::H::set_angle>(this, a0); }  // 0x59f860
auto joan::orig::Sound3D::set_cone_outer_angle(std::uint32_t a0) -> void { joan::bind::member<&::game_b94_05::H::set_distance>(this, a0); }  // 0x59f8c0
auto joan::orig::Sound3D::set_max_distance(std::uint32_t a0) -> void { joan::bind::member<&::en3d_b83_08::H::set_max_distance>(this, a0); }  // 0x59f920
auto joan::orig::Sound3D::set_reference_distance(std::uint32_t a0) -> void { joan::bind::member<&::en3d_b83_08::H::set_reference_distance>(this, a0); }  // 0x59f970
auto joan::orig::Sound3D::set_rolloff(std::uint32_t a0) -> void { joan::bind::member<&::en3d_b83_08::H::set_rolloff>(this, a0); }  // 0x59f9c0
auto joan::orig::Sound3D::set_looping(std::uint8_t a0) -> void { joan::bind::member<&::en3d_b83_08::H::set_looping>(this, a0); }  // 0x59fa80
auto joan::orig::Sound3D::set_relative(std::uint8_t a0) -> void { joan::bind::member<&::en3d_b83_08::H::set_relative>(this, a0); }  // 0x59fae0
auto joan::orig::Sound3D::set_position(void* a0) -> void { joan::bind::member<&::en3d_b83_08::H::set_position>(this, a0); }  // 0x59fb40
auto joan::orig::Sound3D::state() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_14::H::sound_state>(this)); }  // 0x59fc90
auto joan::orig::Sound3D::processed() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b111_17::H::sound_processed>(this)); }  // 0x59fd20
auto joan::orig::Sound3D::queued_59fd60() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b111_19::H::sound_queued>(this)); }  // 0x59fd60
auto joan::orig::Sound3DListener::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_05::H::ctor_59fde0>(this)); }  // 0x59fde0
auto joan::orig::Light::reset_dir() -> void { joan::bind::member<&::game_b101_11::H::reset_dir>(this); }  // 0x59fe40
auto joan::orig::Sound3DListener::shutdown() -> void { joan::bind::member<&::game_b94_05::H::nop_59fe90>(this); }  // 0x59fe90
auto joan::orig::Sound3DListener::set_gain(float a0) -> void { joan::bind::member<&::game_b93_03::H::al_listener_gain>(this, a0); }  // 0x59feb0
auto joan::orig::Sound3DListener::set_position(void* a0) -> void { joan::bind::member<&::game_b99_07::H::listener_set_pos>(this, a0); }  // 0x59ff70
auto joan::orig::Sound3DListener::set_orientation(void* a0, void* a1) -> void { joan::bind::member<&::game_b95_05::H::set_orientation>(this, a0, a1); }  // 0x5a0050
auto joan::orig::Sound3DListener::set_from_view(void* a0) -> void { joan::bind::member<&::game_b100_10::H::set_orient>(this, a0); }  // 0x5a0110
auto joan::orig::Sound3DSystem::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_10::H::zero_2c>(this)); }  // 0x5a02d0
auto joan::orig::Sound3DSystem::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_15::H::fwd_5a0300>(this)); }  // 0x5a0300
auto joan::orig::Sound3D::init_5a0330() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_09::H::sound_init>(this)); }  // 0x5a0330
auto joan::orig::Sound3D::release_5a04e0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b108_08::H::release_5a04e0>(this)); }  // 0x5a04e0
auto joan::orig::Sound3DSystem::set_doppler_factor(std::uint32_t a0) -> void { joan::bind::member<&::game_b97_05::H::set_doppler>(this, a0); }  // 0x5a06f0
auto joan::orig::Sound3DSystem::set_doppler_velocity(std::uint32_t a0) -> void { joan::bind::member<&::game_b99_07::H::listener_set_doppler>(this, a0); }  // 0x5a0790
auto joan::orig::Sound3DSystem::on_shutdown() -> void { joan::bind::member<&::game_b98_05::H::nop_5a0830>(this); }  // 0x5a0830
auto joan::orig::Sound3DSystem::find_buffer(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17::H::cache_find>(this, a0)); }  // 0x5a0870
auto joan::orig::Sound3DBuffer::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_08::H::buffer_delete>(this, a0)); }  // 0x5a0ad0
auto joan::orig::Sound3DSystem::delete_source(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_06::H::remove_source>(this, a0)); }  // 0x5a0cf0
auto joan::orig::Sound3D::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b108_13::H::ddtor_5a0e90>(this, a0)); }  // 0x5a0e90
auto joan::orig::Sound3DSystem::is_initialized() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::en3d_b83_08_2::H::has_12>(this)); }  // 0x5a1110
