// uti3d/video.cpp: functions of the original d:\ww\uti3d\video.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include "../platform/win32_types.hpp"
#include <cstdint>
#include <cstddef>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../media/wm_reader.hpp"
#include <cmath>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b115_10.cpp (fix/uninit)
//
//    - Video::open_684650 0x00684650 (thiscall, name, sound, ret 8): sync reader (FFmpeg's, media/wm_reader.cpp),
//      video / audio output formats, texture 0x006094c0 / 0x00605e20, sound priming. Callers ignore
//      eax (void).
//
//  - 0x00684650: COM out-params read after failed calls (stream type GUID, output numbers, media
//    type sizes) and the wide name buffer when MultiByteToWideChar fails -> 0 / zero GUID / "".
//    They are function-scope like the original slots (later iterations see earlier values).
namespace {
namespace game_b115_10 {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
// Win32 import (stdcall) through its IAT slot.
template <class R, class... A>
R win_call(img::Addr slot, A... args) {
    return (*reinterpret_cast<R(JOAN_STDCALL* const*)(A...)>(img::addr(slot)))(args...);
}
// Indirect stdcall through a function pointer value.
template <class R, class... A>
R stdcall_ptr(std::uintptr_t fn, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(fn)(args...);
}
// COM method (stdcall, interface pointer first) through the vtable: `offset` is the x86 byte offset of the slot
// (slot offset / 4), scaled to the pointer size.
template <class R, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    using Fn = R(JOAN_STDCALL*)(void*, A...);
    return (*static_cast<Fn* const*>(object))[joan::call::x86_vslot(offset)](object, args...);
}
using joan::img::obj;
using joan::img::str;

// WM_MEDIA_TYPE (wmsdkidl.h, natural alignment: 0x48 bytes on x86, 0x58 on x64; the GUIDs as bytes), the
// VIDEOINFOHEADER rectangles and WAVEFORMATEX (byte-packed) as the reader fills them (media/wm_reader.cpp).
struct WmMediaType {
    u8 majortype[16];   // 0x00
    u8 subtype[16];     // 0x10
    i32 fixed_size;     // 0x20
    i32 temporal;       // 0x24
    u32 sample_size;    // 0x28
    u8 formattype[16];  // 0x2c
    void* unk;          // 0x3c
    u32 cb_format;      // 0x40
    u8* pb_format;      // 0x44
};
struct VideoInfoRects {
    i32 source[4];  // 0x00 RECT rcSource
    i32 target[4];  // 0x10 RECT rcTarget (left, top, right, bottom)
};
#pragma pack(push, 1)
struct WaveFormat {
    u16 format_tag;       // 0x0
    u16 channels;         // 0x2
    u32 samples_per_sec;  // 0x4
    u32 avg_bytes;        // 0x8
    u16 block_align;      // 0xc
    u16 bits;             // 0xe
};
#pragma pack(pop)
#if UINTPTR_MAX == 0xffffffffu
static_assert(offsetof(WmMediaType, formattype) == 0x2c && offsetof(WmMediaType, pb_format) == 0x44 && sizeof(WmMediaType) == 0x48);
#else
static_assert(offsetof(WmMediaType, pb_format) == 0x50 && sizeof(WmMediaType) == 0x58);
#endif
static_assert(offsetof(WaveFormat, bits) == 0xe);
const WmMediaType* media(const u8* mt) { return reinterpret_cast<const WmMediaType*>(mt); }

constexpr fn::Code kMemset = fn::crt_memset;  // CRT memset

JOAN_COUNTER(c_video, "Video::open_684650");

struct H {
    // ===================================================================== 0x00684650
    void video_open(const char* name, jo::Sound3D* sound) {
        c_video.hit();
        joan::orig::as_video(this)->close();  // 0x00686160
        jo::self<jo::Video>(this)->video_time = 0;
        jo::self<jo::Video>(this)->audio_time = 0;
        i32 ok = 1;
        WCHAR wname[0x105] = {};  // fix: zeroed (+1 terminator) for a failed conversion
        win_call<i32>(d::iat_KERNEL32_MultiByteToWideChar, 0u, 0u, name, -1, wname, 0x104);  // MultiByteToWideChar
        // Phase C3: our IWMSyncReader on FFmpeg (media/wm_reader.cpp) instead of wmvcore.dll's - same calls below.
        const std::uintptr_t create = joan::media::create_sync_reader_address();
        if (stdcall_ptr<i32>(create, static_cast<void*>(nullptr), 0u, &jo::self<jo::Video>(this)->reader) != 0) {
            joan::orig::warning_box(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Error_in_Windows_Media_Forma));  // 0x00586bc0
            ok = 0;
        }
        auto reader = [&]() { return jo::self<jo::Video>(this)->reader; };
        if (ok != 0) {
            if (com_call<i32>(reader(), 0xc, static_cast<const WCHAR*>(wname)) < 0) ok = 0;  // Open
        }
        if (ok != 0) {
            // line numbers: movsx [0x00a2193c] + n, re-read at every call like the original
            auto alloc = [](u32 size, i32 dl) {
                return joan::orig::arg::as<u8*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size), str(d::s_d_ww_uti3d_video_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a2193c)) + dl)));  // 0x0058e7b0
            };
            auto release_mem = [](u8* p, i32 dl) {
                joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(p), str(d::s_d_ww_uti3d_video_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a2193c)) + dl));  // 0x0058ed70
            };
            auto guid_eq = [](const void* a, const void* b) { return joan::orig::arg::as<i32>(joan::orig::same16(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b))); };  // 0x00598700
            auto guid_ne = [](const void* a, img::Addr g) { return joan::orig::arg::as<i32>(joan::orig::str_equal(joan::orig::arg::str(a), joan::orig::arg::str(obj(g)))); };  // 0x00685910
            void* profile = nullptr;
            com_call<i32>(reader(), 0x0, obj(d::blob_9d1240), &profile);  // QueryInterface
            u32 nstreams = 0;
            com_call<i32>(profile, 0x20, &nstreams);  // GetStreamCount
            jo::self<jo::Video>(this)->audio_stream = 0;
            jo::self<jo::Video>(this)->video_stream = 0;
            jo::self<jo::Video>(this)->width = 0;
            jo::self<jo::Video>(this)->height = 0;
            jo::self<jo::Video>(this)->channels = 0;
            jo::self<jo::Video>(this)->sample_rate = 0;
            // fix: out-params read after failed calls start at 0 (function scope like the slots)
            u8 type_guid[16] = {};
            u32 out244 = 0, sz24c = 0, sz264 = 0, sz26c = 0, out280 = 0, sz288 = 0, sz2a0 = 0, sz2a8 = 0;
            for (u32 i = 0; i < nstreams && ok != 0; ++i) {
                i32 good = 1;
                void* cfg = nullptr;
                com_call<i32>(profile, 0x24, i, &cfg);  // GetStream
                u16 snum = 0;
                com_call<i32>(cfg, 0x10, &snum);       // GetStreamNumber
                com_call<i32>(cfg, 0xc, type_guid);    // GetStreamType
                const u32 hi_this = joan::call::register_low_bits(this) & 0xffff0000u;  // IWMSyncReader::GetOutputNumberForStream takes a WORD: the upper half is a register residue
                if (guid_eq(obj(d::s_vids), type_guid) != 0) {  // 'vids'
                    if (jo::self<jo::Video>(this)->video_stream == 0) {
                        jo::self<jo::Video>(this)->video_stream = snum;
                        com_call<i32>(reader(), 0x4c, hi_this | snum, &out244);  // GetOutputNumberForStream
                        void* props = nullptr;
                        com_call<i32>(reader(), 0x3c, out244, &props);  // GetOutputProps
                        com_call<i32>(props, 0x10, nullptr, &sz24c);    // GetMediaType(NULL)
                        u8* mt = alloc(sz24c, 0x77);
                        com_call<i32>(props, 0x10, mt, &sz24c);
                        if (good != 0 && guid_ne(media(mt)->subtype, d::blob_9d18b8) != 0) {
                            u32 nfmt = 0;
                            com_call<i32>(reader(), 0x44, out244, &nfmt);  // GetOutputFormatCount
                            bool found = false;
                            for (u32 j = 0; j < nfmt; ++j) {
                                void* fp = nullptr;
                                com_call<i32>(reader(), 0x48, out244, j, &fp);  // GetOutputFormat
                                com_call<i32>(fp, 0x10, nullptr, &sz264);
                                u8* m2 = alloc(sz264, 0x8f);
                                com_call<i32>(fp, 0x10, m2, &sz264);
                                if (guid_eq(m2 + 0x10, obj(d::blob_9d18b8)) != 0) {
                                    com_call<i32>(props, 0x14, m2);                // SetMediaType
                                    com_call<i32>(reader(), 0x40, out244, props);  // SetOutputProps
                                    found = true;
                                }
                                release_mem(m2, 0x9d);
                                com_call<u32>(fp, 0x8);  // Release
                            }
                            if (!found) {
                                good = 0;
                            } else {
                                release_mem(mt, 0xaa);
                                mt = nullptr;
                                com_call<u32>(props, 0x8);
                                props = nullptr;
                                com_call<i32>(reader(), 0x3c, out244, &props);
                                com_call<i32>(props, 0x10, nullptr, &sz26c);
                                mt = alloc(sz26c, 0xb4);
                                com_call<i32>(props, 0x10, mt, &sz26c);
                            }
                        }
                        if (good != 0) {
                            if (guid_eq(media(mt)->formattype, obj(d::blob_9d1540)) != 0 || guid_eq(media(mt)->formattype, obj(d::blob_9d1528)) != 0) {
                                const VideoInfoRects* const vih = reinterpret_cast<const VideoInfoRects*>(media(mt)->pb_format);
                                jo::self<jo::Video>(this)->width = static_cast<u32>(vih->target[2]) - static_cast<u32>(vih->target[0]);
                                jo::self<jo::Video>(this)->height = static_cast<u32>(vih->target[3]) - static_cast<u32>(vih->target[1]);
                            } else {
                                good = 0;
                            }
                        }
                        if (good != 0) {
                            u32 w = jo::self<jo::Video>(this)->width;
                            u32 h = jo::self<jo::Video>(this)->height;
                            if ((global<u32>(d::g_a524ac) & 2u) != 0) {
                                for (w = 1; w < jo::self<jo::Video>(this)->width; w <<= 1) {
                                }
                                for (h = 1; h < jo::self<jo::Video>(this)->height; h <<= 1) {
                                }
                            }
                            if ((global<u32>(d::g_a524ac) & 0x20u) != 0) {
                                const u32 m = w > h ? w : h;
                                w = m;
                                h = m;
                            }
                            jo::self<jo::Video>(this)->texture = joan::orig::arg::as<jo::Texture2D*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->create_2d());      // 0x006094c0
                            joan::orig::as_texture(jo::self<jo::Video>(this)->texture)->create(joan::orig::arg::u32(w), joan::orig::arg::u32(h), 1u, 3u);  // 0x00605e20
                        }
                        release_mem(mt, 0xe7);
                        com_call<u32>(props, 0x8);
                    }
                } else if (guid_eq(obj(d::s_auds), type_guid) != 0 && sound != nullptr && jo::self<jo::Video>(this)->audio_stream == 0) {  // 'auds'
                    jo::self<jo::Video>(this)->audio_stream = snum;
                    com_call<i32>(reader(), 0x4c, hi_this | snum, &out280);
                    void* props = nullptr;
                    com_call<i32>(reader(), 0x3c, out280, &props);
                    com_call<i32>(props, 0x10, nullptr, &sz288);
                    u8* mt = alloc(sz288, 0x102);
                    com_call<i32>(props, 0x10, mt, &sz288);
                    if (good != 0 && guid_ne(media(mt)->subtype, d::blob_9d16f0) != 0) {
                        u32 nfmt = 0;
                        com_call<i32>(reader(), 0x44, out280, &nfmt);
                        bool found = false;
                        for (u32 j = 0; j < nfmt; ++j) {
                            void* fp = nullptr;
                            com_call<i32>(reader(), 0x48, out280, j, &fp);
                            com_call<i32>(fp, 0x10, nullptr, &sz2a0);
                            u8* m2 = alloc(sz2a0, 0x11a);
                            com_call<i32>(fp, 0x10, m2, &sz2a0);
                            if (guid_eq(m2 + 0x10, obj(d::blob_9d16f0)) != 0) {
                                com_call<i32>(props, 0x14, m2);
                                com_call<i32>(reader(), 0x40, out280, props);
                                found = true;
                            }
                            release_mem(m2, 0x128);
                            com_call<u32>(fp, 0x8);
                        }
                        if (!found) {
                            good = 0;
                        } else {
                            release_mem(mt, 0x135);
                            mt = nullptr;
                            com_call<u32>(props, 0x8);
                            props = nullptr;
                            com_call<i32>(reader(), 0x3c, out280, &props);
                            com_call<i32>(props, 0x10, nullptr, &sz2a8);
                            mt = alloc(sz2a8, 0x13f);
                            com_call<i32>(props, 0x10, mt, &sz2a8);
                        }
                    }
                    if (good != 0) {
                        if (guid_eq(media(mt)->formattype, obj(d::blob_9d1510)) != 0) {
                            const WaveFormat* const wf = reinterpret_cast<const WaveFormat*>(media(mt)->pb_format);
                            if (wf->format_tag != 1) good = 0;
                            jo::self<jo::Video>(this)->channels = wf->channels;
                            jo::self<jo::Video>(this)->sample_rate = wf->samples_per_sec;
                            jo::self<jo::Video>(this)->bits = wf->bits;
                        } else {
                            good = 0;
                        }
                    }
                    if (good != 0 && sound != nullptr) {
                        jo::self<jo::Video>(this)->sound = sound;
                        joan::orig::as_sound3d(jo::self<jo::Video>(this)->sound)->set_looping(static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x0059fa80
                    }
                    release_mem(mt, 0x161);
                    com_call<u32>(props, 0x8);
                }
                com_call<u32>(cfg, 0x8);  // Release
                if (good == 0) ok = 0;
            }
            com_call<u32>(profile, 0x8);
        }
        if (ok != 0) {
            // SetRange(QWORD start, LONGLONG duration): two 64-bit arguments (four zero dwords on x86)
            if (com_call<i32>(reader(), 0x14, std::uint64_t{0}, std::int64_t{0}) < 0) ok = 0;  // SetRange
        }
        if (ok == 0) {
            jo::self<jo::Video>(this)->sound = nullptr;
            if (jo::self<jo::Video>(this)->texture != nullptr) joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(jo::self<jo::Video>(this)->texture));  // 0x0060a550
            jo::self<jo::Video>(this)->texture = nullptr;
            if (jo::self<jo::Video>(this)->reader != nullptr) com_call<u32>(reader(), 0x8);
            jo::self<jo::Video>(this)->reader = nullptr;
        }
        jo::view<i32>(jo::self<jo::Video>(this)->open) = ok;
        if (ok == 0) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_video(this)->has_audio()) == 0) return;  // 0x00685a30
        void* const snd = joan::orig::arg::as<void*>(joan::orig::as_sound3d(obj(d::g_sound3d_system))->create_buffer_48d120());  // 0x0048d120
        u8 buf[0x300];
        cdecl_call<void*>(kMemset, buf, 0, 0x300u);
        joan::orig::as_sound3dbuffer(snd)->set_data(buf, 0x300u, joan::orig::arg::i32(static_cast<u32>(jo::self<jo::Video>(this)->channels)), joan::orig::arg::u32(jo::self<jo::Video>(this)->sample_rate), joan::orig::arg::i32(static_cast<u32>(jo::self<jo::Video>(this)->bits)));  // 0x0059f1a0
        // idiv / idiv / imul / div as the original (a zero channel / bit count / rate faults there too)
        const i32 a = 0x180 / static_cast<i32>(jo::self<jo::Video>(this)->channels);
        const i32 b = static_cast<i32>(static_cast<u32>(a) << 4) / static_cast<i32>(jo::self<jo::Video>(this)->bits);
        const u32 ms = (static_cast<u32>(b) * 1000u) / jo::self<jo::Video>(this)->sample_rate;
        joan::orig::as_sound3d(jo::self<jo::Video>(this)->sound)->queue(joan::orig::arg::mptr(snd));  // 0x0059f460
        joan::orig::as_sound(jo::self<jo::Video>(this)->sound)->play();       // 0x0059f620
        // [Fixes] MusicStartWait: no sleep for music / ambience (no video stream). The silence stays queued in front
        // of the track, processed() below finds none played yet (unqueue(0) does nothing) and Video::update
        // unqueues it with the track's first played buffers.
        if (!joan::hook::settings().fix_music_start_wait || joan::orig::arg::as<i32>(joan::orig::as_video(this)->has_video()) != 0)  // 0x006859e0
            win_call<void>(d::iat_KERNEL32_Sleep, ms + 5u);                // Sleep
        const u32 pos = joan::orig::arg::as<u32>(joan::orig::as_sound3d(jo::self<jo::Video>(this)->sound)->processed());  // 0x0059fd20
        joan::orig::as_sound3d(jo::self<jo::Video>(this)->sound)->unqueue(joan::orig::arg::i32(pos));              // 0x0059f4b0
    }
};

// 0x00684650: COM out-params after failed calls / failed name conversion -> 0 / zero GUID / "".
JOAN_REPLACE("fix/uninit", "VideoFile::init", 0x00684650, &H::video_open);

}  // namespace game_b115_10
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
//
//      0x00685910 str_equal(a, b)            (cdecl) - 0x00598700(a, b) == 0.
namespace {
namespace game_b95_08 {

using joan::call::cdecl_call;

using i32 = std::int32_t;

// ---------------------------------------------------------------------------- 0x00685910
static i32 JOAN_CDECL str_equal(const char* a, const char* b) {
    return joan::orig::arg::as<i32>(joan::orig::same16(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b))) == 0 ? 1 : 0;  // 0x00598700
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("str_equal", 0x00685910, &str_equal);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
//
//  * module game/b82_14:
//      Video::read_sample 0x00685dd0 (ret 0xc): IWMSyncReader::GetNextSample on +0x24; a video
//        sample (stream +0xc) is copied bottom-up into the +4 texture (+0x58, LockRect /
//        UnlockRect), an audio sample (stream +0xe) is wrapped in a sound buffer (0x0048d120)
//        and queued on the +8 stream; *time = sample time / 10000. Returns 0 (no sample),
//        1 video, 2 audio, -1 other stream.
//      Video::has_audio_stream 0x00685990, Video::has_video_stream 0x00685940,
//      Video::has_audio 0x00685a30, Video::has_video 0x006859e0.
//      Zone::set_points 0x00663cb0 (ret 0xc; copies the point pointer list, bounding box),
//      Zone::link 0x00664130 (ret 0xc), PathZones::group 0x00675660 (connected components of
//      the zone links into +0x70), PathSpace::zone_count 0x0066b9f0,
//      PfdData::clear_grid 0x00665290, PfdData::free_10 0x00666d80,
//      PfdItem::scalar_deleting_destructor 0x006651e0 (ret 4),
//      Zone::vector_deleting_destructor 0x00675d70 (ret 4), Zones::~Zones 0x00675930,
//      Container::clear 0x00677e00.
//      Collisions::add 0x0068dbf0 (ret 0x14), Collisions::remove 0x0068dd80 (ret 8).
//      LensFlares::acquire_texture 0x00662d20, LensFlares::release_texture 0x00662db0,
//      FlareEntry::FlareEntry 0x00662800, FlareEntry::operator= 0x00662830 (ret 4).
//      List::~List 0x006893a0, Entry::~Entry 0x0068c800 (empty), Entry::flag 0x0068c940.
namespace {
namespace game_b82_14 {


struct H {
    std::int32_t has_audio() { return (jo::self<jo::Video>(this)->open != 0 && jo::self<jo::Video>(this)->sound != nullptr) ? 1 : 0; }
};

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("VideoFile::is_output_audio", 0x00685a30, &H::has_audio);
#undef B82_14

}  // namespace game_b82_14
}  // namespace

// ---------------------------------------- from modules/game_leaves82a.cpp (game/leaves82a)
//
//      0x00686160 Video::close
namespace {
namespace game_leaves82a {

using joan::call::thiscall;
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
using joan::img::obj;

JOAN_COUNTER(c_video_close, "Video::close");

struct H {
    // 0x00686160 Video::close: stops / frees the sound stream +8, the texture +4 (0x0060a550
    // on 0xa403a0) and releases the graph +0x24.
    void video_close() {
        c_video_close.hit();
        if (jo::self<jo::Video>(this)->open == 0) return;
        jo::self<jo::Video>(this)->start_time = 0;
        if (jo::view<void*>(jo::self<jo::Video>(this)->sound) != nullptr) {
            joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::Video>(this)->sound))->stop();                                               // 0x0059f680
            const std::int32_t pending = joan::orig::arg::as<std::int32_t>(joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::Video>(this)->sound))->processed());  // 0x0059fd20
            if (pending > 0) joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::Video>(this)->sound))->unqueue(joan::orig::arg::i32(pending));                     // 0x0059f4b0 (word)
            jo::view<void*>(jo::self<jo::Video>(this)->sound) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Video>(this)->texture) != nullptr) {
            joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Video>(this)->texture)));  // 0x0060a550
            jo::view<void*>(jo::self<jo::Video>(this)->texture) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Video>(this)->reader) != nullptr) {
            com_call(jo::view<void*>(jo::self<jo::Video>(this)->reader), 2);  // Release
            jo::view<void*>(jo::self<jo::Video>(this)->reader) = nullptr;
        }
        jo::self<jo::Video>(this)->open = 0;
    }
};

#define LEAVES82A(name, addr, fn) JOAN_REPLACE("game/leaves82a", name, addr, fn)
LEAVES82A("VideoFile::deinit", 0x00686160, &H::video_close);
#undef LEAVES82A

}  // namespace game_leaves82a
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

using joan::call::stdcall_import;

using u32 = std::uint32_t;

using i32 = std::int32_t;


using joan::orig::helpers::memset_;

struct H {
    void* self() { return this; }

    // 0x006845d0
    void* ctor_6845d0() {
        memset_(self(), 0, static_cast<u32>(sizeof(jo::Video)));  // 0x30 on x86 (three pointer members)
        stdcall_import<std::int32_t>(d::iat_ole32_CoInitialize, static_cast<void*>(nullptr));  // CoInitialize
        jo::self<jo::Video>(this)->open = 0;
        return self();
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Com684::Com684", 0x006845d0, &H::ctor_6845d0);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b93_05.cpp (game/b93_05)
namespace {
namespace game_b93_05 {

using joan::call::thiscall;
using joan::call::stdcall_import;

struct H {
    // 0x00684620
    void shutdown() {
        joan::orig::as_video(this)->close();  // 0x00686160
        stdcall_import(d::iat_ole32_CoUninitialize);  // CoUninitialize
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b93_05", name, addr, fn)
GAME("Object::shutdown_684620", 0x00684620, &H::shutdown);
#undef GAME

}  // namespace game_b93_05
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
namespace {
namespace game_b82_14_2 {

using joan::call::thiscall;

using u32 = std::uint32_t;

// COM method: vtable slot at byte offset off, __stdcall with the interface first.
template <class R = std::int32_t, class... A>
R com(void* iface, std::uint32_t off, A... args) {
    const std::uintptr_t fn = (*reinterpret_cast<std::uintptr_t* const*>(iface))[joan::call::x86_vslot(off)];
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(fn)(iface, args...);
}

JOAN_COUNTER(c_read_sample, "Video::read_sample");

struct H {
    // 0x00685dd0 Video::read_sample(unused, out time, copy) (ret 0xc).
    std::int32_t read_sample(std::uint32_t /*unused*/, std::uint32_t* time_out, std::uint32_t copy) {
        c_read_sample.hit();
        if (jo::self<jo::Video>(this)->open == 0 || jo::view<void*>(jo::self<jo::Video>(this)->reader) == nullptr) return 0;
        std::int32_t result = -1;          // ebp-0x8
        void* sample = nullptr;            // ebp-0x10
        std::uint64_t time = 0;            // ebp-0x18 (written on success)
        std::uint64_t duration = 0;        // ebp-0x20
        std::uint32_t flags = 0;           // ebp-0x24
        std::uint32_t output = 0;          // ebp-0x28
        std::uint32_t stream = 0;          // ebp-0x2c (word written; read & 0xffff)
        std::int32_t hr = com<std::int32_t>(jo::view<void*>(jo::self<jo::Video>(this)->reader), 0x1c, 0u, &sample, &time, &duration, &flags, &output, &stream);
        if (hr < 0 || static_cast<std::uint32_t>(hr) == 0xc00d0bcfu) result = 0;
        if (result != 0) {
            if ((stream & 0xffffu) == static_cast<std::uint32_t>(jo::self<jo::Video>(this)->video_stream)) {
                result = 1;
                *time_out = static_cast<std::uint32_t>(time / 10000u);  // _aulldiv
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_video()) != 0 && (copy & 0xffu) != 0) {  // 0x006859e0
                    std::uint8_t* src = nullptr;  // ebp-0x30
                    std::uint32_t len = 0;        // ebp-0x34
                    hr = com<std::int32_t>(sample, 0x1c, &src, &len);  // INSSBuffer::GetBufferAndLength
                    if (hr == 0 && src != nullptr && len != 0) {
                        void* const tex = jo::view<void*>(jo::self<jo::Video>(this)->texture->d3d_texture);  // ebp-0x38
                        com<std::uint32_t>(tex, 0x4);                      // AddRef
                        std::int32_t rect[4] = {0, 0, jo::view<std::int32_t>(jo::self<jo::Video>(this)->width), jo::view<std::int32_t>(jo::self<jo::Video>(this)->height)};  // ebp-0x50
                        struct {
                            std::int32_t pitch;   // ebp-0x40
                            std::uint8_t* bits;   // ebp-0x3c (D3DLOCKED_RECT: pointer-sized, at +8 on x64)
                        } locked = {0, nullptr};
                        hr = com<std::int32_t>(tex, 0x40, 0u, static_cast<void*>(&locked), static_cast<void*>(rect), 0x800u);  // LockRect
                        if (hr == 0) {
                            std::uint8_t* dst = locked.bits + jo::self<jo::Video>(this)->height * static_cast<std::uint32_t>(locked.pitch);  // ebp-0x54
                            const std::uint32_t row = jo::self<jo::Video>(this)->width << 1;                               // ebp-0x58
                            for (std::uint32_t i = 0; i < jo::self<jo::Video>(this)->height; ++i) {
                                dst -= static_cast<std::uint32_t>(locked.pitch);
                                std::memcpy(joan::orig::arg::mptr(dst), static_cast<const void*>(src), joan::orig::arg::u32(row));  // memcpy
                                src += row;
                            }
                            com<std::int32_t>(tex, 0x44, 0u);  // UnlockRect
                        }
                        com<std::uint32_t>(tex, 0x8);  // Release
                    }
                }
            } else if ((stream & 0xffffu) == static_cast<std::uint32_t>(jo::self<jo::Video>(this)->audio_stream)) {
                result = 2;
                *time_out = static_cast<std::uint32_t>(time / 10000u);  // _aulldiv
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_audio()) != 0) {  // 0x00685a30
                    void* buf = nullptr;     // ebp-0x60
                    std::uint32_t len = 0;   // ebp-0x64
                    hr = com<std::int32_t>(sample, 0x1c, &buf, &len);
                    if (hr == 0 && buf != nullptr && len != 0) {
                        void* const snd = joan::orig::arg::as<void*>(joan::orig::as_sound3d(img::obj(d::g_sound3d_system))->create_buffer_48d120());  // 0x0048d120
                        joan::orig::as_sound3dbuffer(snd)->set_119(static_cast<std::uint8_t>(joan::orig::arg::u32(1)));  // 0x0059f2b0
                        joan::orig::as_sound3dbuffer(snd)->set_data(buf, joan::orig::arg::u32(len), joan::orig::arg::i32(static_cast<std::uint32_t>(jo::self<jo::Video>(this)->channels)), joan::orig::arg::u32(jo::self<jo::Video>(this)->sample_rate), joan::orig::arg::i32(static_cast<std::uint32_t>(jo::self<jo::Video>(this)->bits)));  // 0x0059f1a0
                        joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::Video>(this)->sound))->queue(joan::orig::arg::mptr(snd));  // 0x0059f460
                    }
                }
            }
        }
        if (sample != nullptr) {
            com<std::uint32_t>(sample, 0x8);  // Release
            sample = nullptr;
        }
        return result;
    }

    // 0x00685990 / 0x00685940 / 0x00685a30 / 0x006859e0 (ret 0).
    std::int32_t has_audio_stream() { return (jo::self<jo::Video>(this)->open != 0 && jo::self<jo::Video>(this)->audio_stream != 0) ? 1 : 0; }
    std::int32_t has_video_stream() { return (jo::self<jo::Video>(this)->open != 0 && jo::self<jo::Video>(this)->video_stream != 0) ? 1 : 0; }
    std::int32_t has_video() { return (jo::self<jo::Video>(this)->open != 0 && jo::self<jo::Video>(this)->texture != nullptr) ? 1 : 0; }
};

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("VideoFile::get_sample", 0x00685dd0, &H::read_sample);
B82_14("VideoFile::has_input_audio", 0x00685990, &H::has_audio_stream);
B82_14("VideoFile::has_input_video", 0x00685940, &H::has_video_stream);
B82_14("VideoFile::is_output_video", 0x006859e0, &H::has_video);
#undef B82_14

}  // namespace game_b82_14_2
}  // namespace

// ---------------------------------------- from modules/game_leaves82a.cpp (game/leaves82a)
namespace {
namespace game_leaves82a_2 {

using joan::call::thiscall;

using joan::hook::global;

using joan::call::stdcall_call;
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
using joan::img::obj;

std::uint32_t timer() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050()); }  // 0x00591050

JOAN_COUNTER(c_video_update, "Video::update");

struct H {
    // 0x00685a80 Video::update(time): +0 open, +8 sound stream, +0x20 start time, +0x24 graph,
    // +0x28 last video frame time, +0x2c last audio time. Steps the decoder (0x00685dd0) until
    // it catches up with time, feeds the sound stream and returns 0 closed, 1 / 2 playing
    // (2 = a frame was shown), 3 / 4 finished.
    std::int32_t video_update(std::uint32_t time) {
        c_video_update.hit();
        if (jo::self<jo::Video>(this)->open == 0 || jo::self<jo::Video>(this)->reader == nullptr) return 0;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_video()) == 0 &&   // 0x006859e0
            joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_audio()) == 0)     // 0x00685a30
            return 0;
        // (the original copies time to a dead local here)
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_audio_stream()) != 0 && jo::self<jo::Video>(this)->start_time != 0)  // 0x00685990
            time = timer() - jo::self<jo::Video>(this)->start_time;
        if (jo::self<jo::Video>(this)->start_time == 0) jo::self<jo::Video>(this)->start_time = timer() - time;
        std::int32_t frames = 0;
        std::int32_t status = -2;
        std::int32_t reads = 0;  // [Video] Trace only
        const std::uint32_t time_in = time;
        std::uint32_t lead = joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_video_stream()) != 0 ? 0u : 0xc8u;  // 0x00685940
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_video_stream()) != 0 && time == 0) lead = 0x12c;
        for (;;) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_video_stream()) == 0 ||
                (jo::self<jo::Video>(this)->video_time != 0 && !(jo::self<jo::Video>(this)->video_time < time))) {
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_audio_stream()) == 0) break;
                if (jo::self<jo::Video>(this)->audio_time != 0 && jo::self<jo::Video>(this)->audio_time >= time + lead) break;
            }
            std::uint32_t out = 0;  // always written by 0x00685dd0 when it returns 1 / 2
            status = joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->read_sample(joan::orig::arg::u32(time), &out, joan::orig::arg::u32(static_cast<std::uint32_t>(frames == 0))));  // 0x00685dd0
            if (status == 0) break;
            ++reads;
            if (status == 1) {
                ++frames;
                jo::self<jo::Video>(this)->video_time = out;
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_audio_stream()) != 0 && time == 0 && jo::self<jo::Video>(this)->audio_time >= 0x12c) {
                    if (out >= time || frames >= 2) break;
                }
            } else if (status == 2) {
                jo::self<jo::Video>(this)->audio_time = out;
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_video_stream()) == 0 && out >= time + 0xc8) break;
            }
        }
        if (jo::view<void*>(jo::self<jo::Video>(this)->sound) != nullptr) {
            const std::int32_t pending = joan::orig::arg::as<std::int32_t>(joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::Video>(this)->sound))->processed());  // 0x0059fd20
            if (pending > 0) joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::Video>(this)->sound))->unqueue(joan::orig::arg::i32(pending));                     // 0x0059f4b0 (word)
            if (static_cast<std::int32_t>(joan::orig::arg::as<std::int8_t>(joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::Video>(this)->sound))->state())) != 2 && status != 0 &&  // 0x0059fc90
                (joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_video()) != 0 || jo::self<jo::Video>(this)->audio_time >= 0xc8)) {
                joan::orig::as_sound(jo::view<void*>(jo::self<jo::Video>(this)->sound))->play();  // 0x0059f620
                if (jo::self<jo::Video>(this)->start_time == 0) {
                    jo::self<jo::Video>(this)->start_time = timer() - time;
                    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_video()) != 0)
                        stdcall_call<void>(global<std::uintptr_t>(d::iat_KERNEL32_Sleep), 0x1f4u);  // Sleep(500)
                }
            }
        }
        std::int32_t ret;
        if (status == 0) {
            ret = joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_video()) != 0 && jo::self<jo::Video>(this)->video_time != 0 ? 4 : 3;
        } else if (status == -2) {
            ret = joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_video()) != 0 && jo::self<jo::Video>(this)->video_time != 0 ? 2 : 1;
        } else {
            ret = status == 1 ? 2 : 1;
        }
        if (joan::media::trace_video() && joan::orig::arg::as<std::int32_t>(joan::orig::as_video(this)->has_video()) != 0)
            joan::media::trace_line("update(%u) time %u reads %d frames %d last %d -> %d (video %u, audio %u, clock %u)",
                                 time_in, time, reads, frames, status, ret, jo::self<jo::Video>(this)->video_time, jo::self<jo::Video>(this)->audio_time,
                                 timer());
        return ret;
    }

    // 0x00686250 Video::seek(ms): graph +0x24 vtable +0x14 (position ms * 10000 as 100 ns
    // units, 0, 0); clears the times +0x20 / +0x2c / +0x28.
    void video_seek(std::uint32_t ms) {
        if (jo::self<jo::Video>(this)->open == 0) return;
        const std::uint64_t position = static_cast<std::uint64_t>(ms) * 10000u;  // _allmul
        // SetRange(QWORD start, LONGLONG duration = 0): the original pushes the duration as two zero dwords
        com_call(jo::view<void*>(jo::self<jo::Video>(this)->reader), 0x14 / 4, position, std::int64_t{0});
        jo::self<jo::Video>(this)->start_time = 0;
        jo::self<jo::Video>(this)->audio_time = 0;
        jo::self<jo::Video>(this)->video_time = 0;
    }
};

#define LEAVES82A(name, addr, fn) JOAN_REPLACE("game/leaves82a", name, addr, fn)
LEAVES82A("VideoFile::get_next", 0x00685a80, &H::video_update);
LEAVES82A("Video::seek", 0x00686250, &H::video_seek);
#undef LEAVES82A

}  // namespace game_leaves82a_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Video::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::ctor_6845d0>(this)); }  // 0x6845d0
auto joan::orig::Video::shutdown() -> void { joan::bind::member<&::game_b93_05::H::shutdown>(this); }  // 0x684620
auto joan::orig::Video::open_684650(const char* a0, Sound3D* a1) -> void { joan::bind::member<&::game_b115_10::H::video_open>(this, a0, a1); }  // 0x684650
auto joan::orig::str_equal(const char* a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b95_08::str_equal>(a0, a1)); }  // 0x685910
auto joan::orig::Video::has_video_stream() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_14_2::H::has_video_stream>(this)); }  // 0x685940
auto joan::orig::Video::has_audio_stream() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_14_2::H::has_audio_stream>(this)); }  // 0x685990
auto joan::orig::Video::has_video() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_14_2::H::has_video>(this)); }  // 0x6859e0
auto joan::orig::Video::has_audio() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_14::H::has_audio>(this)); }  // 0x685a30
auto joan::orig::Video::update(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves82a_2::H::video_update>(this, a0)); }  // 0x685a80
auto joan::orig::Video::read_sample(std::uint32_t a0, void* a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_14_2::H::read_sample>(this, a0, a1, a2)); }  // 0x685dd0
auto joan::orig::Video::close() -> void { joan::bind::member<&::game_leaves82a::H::video_close>(this); }  // 0x686160
auto joan::orig::Video::seek(std::uint32_t a0) -> void { joan::bind::member<&::game_leaves82a_2::H::video_seek>(this, a0); }  // 0x686250
