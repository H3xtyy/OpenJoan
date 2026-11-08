// game/unknown_tfanim.cpp: functions whose original source file is unknown (formerly game/_unplaced/tfanim.cpp;
// re/attribution.csv: between ww/elib/jpeg.cpp and ww/en3d/actor.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/dynarray.hpp"
#include "../x87.hpp"
#include "../original/bind.hpp"
#include "../original/layout_access.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;


// Raw reads of FLIC file data (headers, chunks, RLE runs): file bytes, the same on x64 (no game objects).
using joan::orig::helpers::at;

// The leading u32 of a frame chunk in the movie byte stream (a record read from a buffer, not a field).
std::uint32_t chunk_size(const void* chunk) {
    std::uint32_t n;
    std::memcpy(&n, chunk, sizeof n);
    return n;
}

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::str;

using u16 = std::uint16_t;

using i32 = std::int32_t;

using i16 = std::int16_t;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
namespace {
namespace game_b95_05 {

JOAN_COUNTER(c_anim_new, "TFAnimation_NewFile");

// 0x005a93f0
void* JOAN_CDECL anim_new_file(const char* path) {
    c_anim_new.hit();
    void* const anim = joan::orig::arg::as<void*>(joan::orig::malloc_(static_cast<std::uint32_t>(sizeof(jo::MusicStream))));  // malloc
    if (anim == nullptr) {
        joan::orig::call_hook_a67638(&img::global<const char>(d::s_Memory_allocation_error_in_T_9d90e0));  // 0x005aaaf0
        return nullptr;
    }
    void* const file = joan::orig::arg::as<void*>(joan::orig::libc::fopen(joan::orig::arg::str(path), joan::orig::arg::str(&img::global<const char>(d::s_rb_9d90dc))));  // fopen
    if (file == nullptr) {
        joan::orig::free_(joan::orig::arg::mptr(anim));  // free
        joan::orig::call_hook_a67638(&img::global<const char>(d::s_Error_opening_file_in_TFAnim));
        return nullptr;
    }
    void* const header = joan::orig::arg::as<void*>(joan::orig::malloc_(0x80u));
    if (header == nullptr) {
        joan::orig::free_(joan::orig::arg::mptr(anim));
        joan::orig::libc::fclose(joan::orig::arg::mptr(file));  // fclose
        joan::orig::call_hook_a67638(&img::global<const char>(d::s_Memory_allocation_error_in_T_9d90e0));
        return nullptr;
    }
    if (joan::orig::arg::as<std::uint32_t>(joan::orig::libc::fread(joan::orig::arg::mptr(header), 0x80u, 1u, joan::orig::arg::mptr(file))) != 1u) {  // fread
        joan::orig::free_(joan::orig::arg::mptr(anim));
        joan::orig::free_(joan::orig::arg::mptr(header));
        joan::orig::libc::fclose(joan::orig::arg::mptr(file));
        joan::orig::call_hook_a67638(&img::global<const char>(d::s_Error_reading_file_in_TFAnim_9d906c));
        return nullptr;
    }
    if (joan::orig::music_stream::read_header(joan::orig::arg::mptr(anim), joan::orig::arg::mptr(header)) == 0) {
        joan::orig::free_(joan::orig::arg::mptr(anim));
        joan::orig::free_(joan::orig::arg::mptr(header));
        joan::orig::libc::fclose(joan::orig::arg::mptr(file));
        joan::orig::call_hook_a67638(&img::global<const char>(d::s_Animation_format_error_in_TF_9d9030));
        return nullptr;
    }
    jo::view<std::uint16_t>(joan::orig::as_music_stream(anim)->source) = 0;
    joan::orig::as_music_stream(anim)->file = file;
    jo::view<std::uint32_t>(joan::orig::as_music_stream(anim)->file_start) = 0;
    joan::orig::music_stream::reset(joan::orig::arg::mptr(anim));
    joan::orig::as_music_stream(anim)->prev = nullptr;
    joan::orig::as_music_stream(anim)->next = global<joan::orig::MusicStream*>(d::g_music_stream_list);
    global<joan::orig::MusicStream*>(d::g_music_stream_list) = joan::orig::as_music_stream(anim);
    joan::orig::free_(joan::orig::arg::mptr(header));
    return anim;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("TFAnimation_NewFile", 0x005a93f0, &anim_new_file);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b115_11.cpp (game/b115_11)
namespace {
namespace game_b115_11 {

JOAN_COUNTER(c_tfa, "TFAnimation::new_handle_5a9610");

// ============================================================ 0x005a9610
void* JOAN_CDECL tfanim_new_handle(void* file) {
    c_tfa.hit();
    u8* const h = joan::orig::arg::as<u8*>(joan::orig::malloc_(static_cast<std::uint32_t>(sizeof(jo::MusicStream))));  // malloc
    if (h == nullptr) {
        joan::orig::call_hook_a67638(str(d::s_Memory_allocation_error_in_T_9d9194));  // 0x005aaaf0 error
        return nullptr;
    }
    void* const hdr = joan::orig::arg::as<void*>(joan::orig::malloc_(0x80u));
    if (hdr == nullptr) {
        joan::orig::free_(joan::orig::arg::mptr(static_cast<void*>(h)));  // free
        joan::orig::call_hook_a67638(str(d::s_Memory_allocation_error_in_T_9d9194));
        return nullptr;
    }
    // fread(file, 0x80, 1, hdr): the original's swapped buffer / stream.
    if (joan::orig::arg::as<u32>(joan::orig::libc::fread(joan::orig::arg::mptr(file), 0x80u, 1u, joan::orig::arg::mptr(hdr))) != 1) {
        joan::orig::free_(joan::orig::arg::mptr(static_cast<void*>(h)));
        joan::orig::free_(joan::orig::arg::mptr(hdr));
        joan::orig::call_hook_a67638(str(d::s_Error_reading_file_in_TFAnim_9d915c));
        return nullptr;
    }
    if (joan::orig::music_stream::read_header(joan::orig::arg::mptr(static_cast<void*>(h)), joan::orig::arg::mptr(hdr)) == 0) {
        joan::orig::free_(joan::orig::arg::mptr(static_cast<void*>(h)));
        joan::orig::free_(joan::orig::arg::mptr(hdr));
        joan::orig::call_hook_a67638(str(d::s_Animation_format_error_in_TF_9d911c));
        return nullptr;
    }
    jo::view<u16>(joan::orig::as_music_stream(h)->source) = 1;
    joan::orig::as_music_stream(h)->file = file;
    jo::view<u32>(joan::orig::as_music_stream(h)->file_start) = joan::orig::arg::as<u32>(joan::orig::libc::ftell(joan::orig::arg::mptr(file)));  // ftell
    joan::orig::music_stream::reset(joan::orig::arg::mptr(static_cast<void*>(h)));
    joan::orig::as_music_stream(h)->prev = nullptr;
    joan::orig::as_music_stream(h)->next = global<joan::orig::MusicStream*>(d::g_music_stream_list);
    global<joan::orig::MusicStream*>(d::g_music_stream_list) = joan::orig::as_music_stream(h);
    return h;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b115_11", name, addr, fn)
B("TFAnimation::new_handle_5a9610", 0x005a9610, &tfanim_new_handle);
#undef B

}  // namespace game_b115_11
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
namespace {
namespace game_b97_05 {

JOAN_COUNTER(c_anim_new, "TFAnimation_NewMem");

// 0x005a97b0
void* JOAN_CDECL anim_new_mem(void* data) {
    c_anim_new.hit();
    std::uint8_t* const p = joan::orig::arg::as<std::uint8_t*>(joan::orig::malloc_(static_cast<std::uint32_t>(sizeof(jo::MusicStream))));  // malloc
    if (p == nullptr) {
        joan::orig::call_hook_a67638(str(d::s_Memory_allocation_error_in_T_9d9210));  // 0x005aaaf0 "Memory allocation error ..."
        return nullptr;
    }
    if (joan::orig::arg::as<std::uint8_t>(joan::orig::music_stream::read_header(joan::orig::arg::mptr(static_cast<void*>(p)), data)) == 0) {
        joan::orig::free_(joan::orig::arg::mptr(static_cast<void*>(p)));  // free
        joan::orig::call_hook_a67638(str(d::s_Animation_format_error_in_TF_9d91d4));        // 0x005aaaf0 "Animation format error ..."
        return nullptr;
    }
    jo::view<std::uint16_t>(joan::orig::as_music_stream(p)->source) = 2;
    joan::orig::as_music_stream(p)->data = static_cast<u8*>(data);
    joan::orig::as_music_stream(p)->data_pos = 0x80;
    joan::orig::music_stream::reset(joan::orig::arg::mptr(static_cast<void*>(p)));
    joan::orig::as_music_stream(p)->prev = nullptr;
    joan::orig::as_music_stream(p)->next = global<joan::orig::MusicStream*>(d::g_music_stream_list);
    global<joan::orig::MusicStream*>(d::g_music_stream_list) = joan::orig::as_music_stream(p);
    return p;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("TFAnimation_NewMem", 0x005a97b0, &anim_new_mem);
#undef B

}  // namespace game_b97_05
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

JOAN_COUNTER(c_release, "MusicStream::release");

// 0x005a98a0 unlinks a music stream (as in the original: the +0x27 field receives +0x23),
// frees its buffers, closes the file, frees it.
std::uint8_t JOAN_CDECL stream_release(std::uint8_t* p) {
    c_release.hit();
    joan::orig::MusicStream* const s = joan::orig::as_music_stream(p);
    if (s->prev != nullptr)
        s->prev->next = s->next;
    else
        global<joan::orig::MusicStream*>(d::g_music_stream_list) = s->next;
    if (s->next != nullptr) s->next = s->prev;
    if (joan::orig::as_music_stream(p)->user_buffers == 0) {
        if (joan::orig::as_music_stream(p)->pixels != nullptr) joan::orig::free_(joan::orig::arg::mptr(joan::orig::as_music_stream(p)->pixels));  // free
        if (joan::orig::as_music_stream(p)->palette != nullptr) joan::orig::free_(joan::orig::arg::mptr(joan::orig::as_music_stream(p)->palette));
    }
    if (joan::orig::as_music_stream(p)->source == 0) joan::orig::libc::fclose(joan::orig::arg::mptr(joan::orig::as_music_stream(p)->file));  // fclose
    joan::orig::free_(joan::orig::arg::mptr(static_cast<void*>(p)));
    return 1;
}

// 0x005a9ad0 copies the header fields; true for 8-bit samples.
std::uint8_t JOAN_CDECL stream_read_header(std::uint8_t* p, const std::uint8_t* h) {
    joan::orig::as_music_stream(p)->frame_count = at<std::uint16_t>(h, 6);
    jo::view<std::uint16_t>(joan::orig::as_music_stream(p)->width) = at<std::uint16_t>(h, 8);
    jo::view<std::uint16_t>(joan::orig::as_music_stream(p)->height) = at<std::uint16_t>(h, 0xa);
    return at<std::uint16_t>(h, 0xc) == 8 ? 1 : 0;
}

// 0x005a9b40
std::uint8_t* JOAN_CDECL stream_reset(std::uint8_t* p) {
    joan::orig::as_music_stream(p)->user_buffers = 0;
    joan::orig::as_music_stream(p)->pixels = nullptr;
    joan::orig::as_music_stream(p)->palette = nullptr;
    joan::orig::as_music_stream(p)->palette_cb = nullptr;
    joan::orig::as_music_stream(p)->frame = 0;
    return p;
}

// 0x005aa040 back to the data start (+0x80 past the header).
std::uint8_t JOAN_CDECL stream_rewind(std::uint8_t* p) {
    joan::orig::as_music_stream(p)->frame = 0;
    if (joan::orig::as_music_stream(p)->source != 2)
        joan::orig::libc::fseek(joan::orig::arg::mptr(joan::orig::as_music_stream(p)->file), joan::orig::arg::i32(joan::orig::as_music_stream(p)->file_start + 0x80), 0);  // fseek
    else
        joan::orig::as_music_stream(p)->data_pos = 0x80;
    return 1;
}

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("MusicStream::release", 0x005a98a0, &stream_release);
B91("MusicStream::read_header", 0x005a9ad0, &stream_read_header);
B91("MusicStream::reset", 0x005a9b40, &stream_reset);
B91("MusicStream::rewind", 0x005aa040, &stream_rewind);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
namespace {
namespace game_b93_03 {

using joan::orig::helpers::crt_free;                                 // free / delete
std::uintptr_t low_byte_into(std::uintptr_t eax, u8 al) { return (eax & ~std::uintptr_t{0xff}) | al; }

// 0x005a99e0
std::uintptr_t JOAN_CDECL rec_export(void* src, void* dst) {
    jo::as_flc_header(dst)->width = jo::view<u16>(joan::orig::as_music_stream(src)->width);
    jo::as_flc_header(dst)->height = jo::view<u16>(joan::orig::as_music_stream(src)->height);
    jo::as_flc_header(dst)->frame_count = joan::orig::as_music_stream(src)->frame_count;
    jo::as_flc_header(dst)->frame = joan::orig::as_music_stream(src)->frame;
    jo::as_flc_header(dst)->pixels = joan::orig::as_music_stream(src)->pixels;
    jo::as_flc_header(dst)->palette = joan::orig::as_music_stream(src)->palette;
    jo::as_flc_header(dst)->loop = joan::orig::as_music_stream(src)->loop;
    return low_byte_into(reinterpret_cast<std::uintptr_t>(dst), 1);
}

// 0x005a9b90
std::uintptr_t JOAN_CDECL rec_set_buffers(void* rec, void* a, void* b) {
    if (joan::orig::as_music_stream(rec)->user_buffers == 0) {
        if (joan::orig::as_music_stream(rec)->pixels != nullptr) crt_free(joan::orig::as_music_stream(rec)->pixels);
        if (joan::orig::as_music_stream(rec)->palette != nullptr) crt_free(joan::orig::as_music_stream(rec)->palette);
    }
    joan::orig::as_music_stream(rec)->user_buffers = 1;
    joan::orig::as_music_stream(rec)->pixels = static_cast<u8*>(a);
    joan::orig::as_music_stream(rec)->palette = static_cast<u8*>(b);
    return low_byte_into(reinterpret_cast<std::uintptr_t>(b), 1);
}

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Rec::export", 0x005a99e0, &rec_export);
B93("Rec::set_buffers", 0x005a9b90, &rec_set_buffers);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
namespace {
namespace game_b103_10 {

using joan::orig::helpers::addr;

JOAN_COUNTER(c_frame_seek, "TFFrame_Seek");

// 0x005a9a70 / 0x005a9aa0 (dead code): eax = p with al = 1.
static std::uintptr_t JOAN_CDECL anim_set_2b(void* p, u8 v) {
    joan::orig::as_music_stream(p)->loop = v;
    return (addr(p) & 0xffffff00u) | 1u;
}
static std::uintptr_t JOAN_CDECL anim_set_2c(void* p, void* v) {
    joan::orig::as_music_stream(p)->palette_cb = v;  // the callback (x64: a pointer argument)
    return (addr(p) & 0xffffff00u) | 1u;
}

// 0x005aa0b0 (dead code)
static std::uintptr_t JOAN_CDECL frame_seek(void* p, i32 n) {
    c_frame_seek.hit();
    if (n < jo::view<i16>(joan::orig::as_music_stream(p)->frame)) joan::orig::music_stream::rewind(joan::orig::arg::mptr(p));  // 0x005aa040
    while (jo::view<i16>(joan::orig::as_music_stream(p)->frame) < n) {
        if (joan::orig::tfframe::decode(joan::orig::arg::mptr(p)) == 0) {  // 0x005a9d80
            joan::orig::call_hook_a67638(str(d::s_Decoding_error_in_TFFrame_Se));  // 0x005aaaf0 "Decoding error in TFFrame_Seek()"
            return 0;  // al = 0 (dead code: no caller reads the eax)
        }
    }
    return (addr(p) & 0xffffff00u) | 1u;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("TFAnimation_set_2b", 0x005a9a70, &anim_set_2b);
B("TFAnimation_set_2c", 0x005a9aa0, &anim_set_2c);
B("TFFrame_Seek", 0x005aa0b0, &frame_seek);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- from modules/game_b102_10.cpp (game/b102_10)
namespace {
namespace game_b102_10 {

// Game CRT (statically linked) - always the game's own copies.
void* crt_malloc(u32 n) { return joan::orig::arg::as<void*>(joan::orig::malloc_(joan::orig::arg::u32(n))); }
using joan::orig::helpers::crt_free;
i32 crt_fread(void* b, u32 size, u32 n, void* fp) { return joan::orig::arg::as<i32>(joan::orig::libc::fread(joan::orig::arg::mptr(b), joan::orig::arg::u32(size), joan::orig::arg::u32(n), joan::orig::arg::mptr(fp))); }
i32 crt_ftell(void* fp) { return joan::orig::arg::as<i32>(joan::orig::libc::ftell(joan::orig::arg::mptr(fp))); }
i32 crt_fgetc(void* fp) { return joan::orig::arg::as<i32>(joan::orig::libc::getc(joan::orig::arg::mptr(fp))); }
i32 crt_fseek(void* fp, i32 off, i32 whence) { return joan::orig::arg::as<i32>(joan::orig::libc::fseek(joan::orig::arg::mptr(fp), joan::orig::arg::i32(off), joan::orig::arg::i32(whence))); }

void tf_error(img::Addr msg) { joan::orig::call_hook_a67638(str(msg)); }  // 0x005aaaf0

JOAN_COUNTER(c_tf_alloc, "TFBuffers::alloc");
JOAN_COUNTER(c_tf_free, "TFBuffers::free");
JOAN_COUNTER(c_tf_decode, "TFFrame::decode");

// 0x005a9c10 TFBuffers_Alloc(handle)
u8 JOAN_CDECL tf_buffers_alloc(u8* h) {
    c_tf_alloc.hit();
    if (joan::orig::as_music_stream(h)->user_buffers == 1) {
        joan::orig::as_music_stream(h)->pixels = nullptr;
        joan::orig::as_music_stream(h)->palette = nullptr;
    }
    if (joan::orig::as_music_stream(h)->pixels == nullptr) {
        const u32 n = static_cast<u32>(static_cast<i32>(joan::orig::as_music_stream(h)->width)) *
                      static_cast<u32>(static_cast<i32>(joan::orig::as_music_stream(h)->height));  // imul
        joan::orig::as_music_stream(h)->pixels = static_cast<u8*>(crt_malloc(n));
    }
    if (joan::orig::as_music_stream(h)->palette == nullptr) joan::orig::as_music_stream(h)->palette = static_cast<u8*>(crt_malloc(0x300));
    joan::orig::as_music_stream(h)->user_buffers = 0;
    if (joan::orig::as_music_stream(h)->pixels == nullptr || joan::orig::as_music_stream(h)->palette == nullptr) {
        tf_error(d::s_Memory_allocation_error_in_T_9d9280);
        return 0;
    }
    return 1;
}

// 0x005a9ce0 TFBuffers_Free(handle)
u8 JOAN_CDECL tf_buffers_free(u8* h) {
    c_tf_free.hit();
    if (joan::orig::as_music_stream(h)->user_buffers == 1) {
        tf_error(d::s_User_supplied_buffers_presen);
        return 0;
    }
    if (joan::orig::as_music_stream(h)->pixels != nullptr) crt_free(joan::orig::as_music_stream(h)->pixels);
    if (joan::orig::as_music_stream(h)->palette != nullptr) crt_free(joan::orig::as_music_stream(h)->palette);
    joan::orig::as_music_stream(h)->pixels = nullptr;
    joan::orig::as_music_stream(h)->palette = nullptr;
    return 1;
}

void frame_inc(u8* h) { joan::orig::as_music_stream(h)->frame = static_cast<u16>(joan::orig::as_music_stream(h)->frame + 1); }

// 0x005a9d80 TFFrame_Decode(handle)
u8 JOAN_CDECL tf_frame_decode(u8* h) {
    c_tf_decode.hit();
    if (joan::orig::as_music_stream(h)->palette == nullptr || joan::orig::as_music_stream(h)->pixels == nullptr) {
        tf_error(d::s_No_buffers_present_in_TFFram);
        return 0;
    }
    if (!(static_cast<i32>(jo::view<i16>(joan::orig::as_music_stream(h)->frame)) < static_cast<i32>(jo::view<i16>(joan::orig::as_music_stream(h)->frame_count)))) {
        if (joan::orig::as_music_stream(h)->loop == 0) {
            tf_error(d::s_End_of_animation_in_TFFrame);
            return 0;
        }
        joan::orig::music_stream::rewind(joan::orig::arg::mptr(static_cast<void*>(h)));  // 0x005aa040 rewind
    }
    u8* buf;
    if (joan::orig::as_music_stream(h)->source != 2) {
        const i32 pos = crt_ftell(joan::orig::as_music_stream(h)->file);
        const i32 size = joan::orig::arg::as<i32>(joan::orig::read_u32(joan::orig::arg::mptr(joan::orig::as_music_stream(h)->file)));
        u16 magic = static_cast<u16>(crt_fgetc(joan::orig::as_music_stream(h)->file));
        const u32 hi = static_cast<u32>(crt_fgetc(joan::orig::as_music_stream(h)->file));
        magic = static_cast<u16>(magic | static_cast<u16>((hi & 0xffffu) << 8));
        crt_fseek(joan::orig::as_music_stream(h)->file, pos, 0);
        if (magic != 0xf1fa) {
            frame_inc(h);
            crt_fseek(joan::orig::as_music_stream(h)->file, size, 1);
            return 1;
        }
        buf = static_cast<u8*>(crt_malloc(static_cast<u32>(size)));
        if (buf == nullptr) {
            tf_error(d::s_Memory_allocation_error_in_T_9d92f4);
            return 0;
        }
        crt_fread(buf, static_cast<u32>(size), 1, joan::orig::as_music_stream(h)->file);
    } else {
        buf = joan::orig::as_music_stream(h)->data + joan::orig::as_music_stream(h)->data_pos;
        joan::orig::as_music_stream(h)->data_pos = joan::orig::as_music_stream(h)->data_pos + chunk_size(buf);
        if (at<u16>(buf, 0) != 0xf1fa) {
            frame_inc(h);
            return 1;
        }
    }
    frame_inc(h);
    i32 count = at<u16>(buf, 6);
    i32 off = 0x10;
    for (;;) {
        const i32 c = count;
        count = count - 1;
        if (c <= 0) break;
        const i32 chunk = at<i32>(buf, off);
        joan::orig::msg::dispatch_5aa1b0(static_cast<void*>(h), joan::orig::arg::mptr(static_cast<void*>(buf + off)));  // no thunk
        off += chunk;
    }
    if (joan::orig::as_music_stream(h)->source != 2) crt_free(buf);
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b102_10", name, addr, fn)
B("TFBuffers::alloc", 0x005a9c10, &tf_buffers_alloc);
B("TFBuffers::free", 0x005a9ce0, &tf_buffers_free);
B("TFFrame::decode", 0x005a9d80, &tf_frame_decode);
#undef B

}  // namespace game_b102_10
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
namespace {
namespace game_b94_05 {

// 0x005aa130
std::uint32_t JOAN_CDECL read_u32(void* file) {
    std::uint32_t v = joan::orig::arg::as<std::uint32_t>(joan::orig::libc::getc(joan::orig::arg::mptr(file)));
    v |= joan::orig::arg::as<std::uint32_t>(joan::orig::libc::getc(joan::orig::arg::mptr(file))) << 8;
    v |= joan::orig::arg::as<std::uint32_t>(joan::orig::libc::getc(joan::orig::arg::mptr(file))) << 16;
    v |= joan::orig::arg::as<std::uint32_t>(joan::orig::libc::getc(joan::orig::arg::mptr(file))) << 24;
    return v;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("read_u32", 0x005aa130, &read_u32);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b101_11.cpp (game/b101_11)
namespace {
namespace game_b101_11 {

JOAN_COUNTER(c_dispatch, "Msg::dispatch_5aa1b0");

// 0x005aa1b0
void JOAN_CDECL msg_dispatch(void* a, u8* msg) {
    c_dispatch.hit();
    switch (static_cast<u32>(at<u16>(msg, 4))) {
        case 4: joan::orig::gif_decode_palette(a, joan::orig::arg::mptr(msg)); break;
        case 7: joan::orig::rle_blit16(a, joan::orig::arg::mptr(msg)); break;
        case 11: joan::orig::fli_color256(a, joan::orig::arg::mptr(msg)); break;
        case 12: joan::orig::rle_blit(a, joan::orig::arg::mptr(msg)); break;
        case 13: joan::orig::image::clear(a); break;
        case 15: joan::orig::fli_brun(a, joan::orig::arg::mptr(msg)); break;
        case 16: joan::orig::image::copy_pixels(a, joan::orig::arg::mptr(msg)); break;
        default: break;
    }
}

#define B(name, addr, fn) JOAN_REPLACE("game/b101_11", name, addr, fn)
B("Msg::dispatch_5aa1b0", 0x005aa1b0, &msg_dispatch);
#undef B

}  // namespace game_b101_11
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
namespace {
namespace game_b100_10 {

JOAN_COUNTER(c_color256, "fli_color256");
JOAN_COUNTER(c_brun, "fli_brun");

// 0x005aa2e0: FLI COLOR_256 chunk.
void JOAN_CDECL fli_color256(u8* ctx, const u8* chunk) {
    c_color256.hit();
    i32 idx = 0;
    i32 off = 8;
    i32 packets = *reinterpret_cast<const u16*>(chunk + 6);
    for (;;) {
        const i32 p = packets--;
        if (p <= 0) break;
        idx += chunk[off];
        ++off;
        i32 count = chunk[off];
        ++off;
        if (count == 0) count = 0x100;
        for (;;) {
            const i32 c = count--;
            if (c <= 0) break;
            for (i32 k = 0; k < 3; ++k) {
                joan::orig::as_music_stream(ctx)->palette[idx * 3 + k] = chunk[off];
                ++off;
            }
            ++idx;
        }
    }
    if (joan::orig::as_music_stream(ctx)->palette_cb != nullptr) {
        reinterpret_cast<void(JOAN_CDECL*)(u8*)>(joan::orig::as_music_stream(ctx)->palette_cb)(joan::orig::as_music_stream(ctx)->palette);
    }
}

// 0x005aa610: FLI BYTE_RUN chunk.
void JOAN_CDECL fli_brun(u8* ctx, const u8* chunk) {
    c_brun.hit();
    u8* line = joan::orig::as_music_stream(ctx)->pixels;
    i32 off = 6;
    for (i32 y = 0; y < static_cast<i32>(joan::orig::as_music_stream(ctx)->height); ++y) {
        i32 x = 0;
        i32 packets = chunk[off];
        ++off;
        for (;;) {
            const i32 p = packets--;
            if (p <= 0) break;
            i32 count = static_cast<i8>(chunk[off]);
            ++off;
            if (count >= 0) {
                const u8 b = chunk[off];
                ++off;
                for (;;) {
                    const i32 c = count--;
                    if (c <= 0) break;
                    line[x] = b;
                    ++x;
                }
            } else {
                for (;;) {
                    const i32 c = count++;
                    if (c >= 0) break;
                    line[x] = chunk[off];
                    ++x;
                    ++off;
                }
            }
        }
        line += static_cast<i32>(joan::orig::as_music_stream(ctx)->width);
    }
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("fli_color256", 0x005aa2e0, &fli_color256);
B("fli_brun", 0x005aa610, &fli_brun);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

u8* add(const void* p, u32 n) { return reinterpret_cast<u8*>(reinterpret_cast<std::uintptr_t>(p) + n); }

JOAN_COUNTER(c_rle_blit, "rle_blit");

// ---------------------------------------------------------------------------- 0x005aa420
void JOAN_CDECL rle_blit(void* surface, const u8* rle) {
    c_rle_blit.hit();
    i32 idx = 0xa;
    const i32 x0 = static_cast<i32>(at<u16>(rle, 6));
    u8* dst = add(joan::orig::as_music_stream(surface)->pixels, static_cast<u32>(static_cast<i32>(joan::orig::as_music_stream(surface)->width)) * static_cast<u32>(x0));
    i32 rows = static_cast<i32>(at<u16>(rle, 8));
    for (;;) {
        const i32 r = rows;
        rows = r - 1;
        if (r <= 0) break;
        i32 x = 0;
        i32 runs = static_cast<i32>(rle[idx]);
        ++idx;
        for (;;) {
            const i32 n = runs;
            runs = n - 1;
            if (n <= 0) break;
            x += static_cast<i32>(rle[idx]);
            ++idx;
            i32 c = static_cast<i32>(static_cast<i8>(rle[idx]));
            ++idx;
            if (c >= 0) {
                for (;;) {
                    const i32 k = c;
                    c = k - 1;
                    if (k <= 0) break;
                    *add(dst, static_cast<u32>(x)) = rle[idx];
                    ++x;
                    ++idx;
                }
            } else {
                const u8 v = rle[idx];
                ++idx;
                for (;;) {
                    const i32 k = c;
                    c = k + 1;
                    if (k >= 0) break;
                    *add(dst, static_cast<u32>(x)) = v;
                    ++x;
                }
            }
        }
        dst = add(dst, static_cast<u32>(static_cast<i32>(joan::orig::as_music_stream(surface)->width)));
    }
}

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("rle_blit", 0x005aa420, &rle_blit);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
namespace {
namespace game_b98_05 {

// 0x005aa5c0
void* JOAN_CDECL image_clear(void* img) {
    const std::int32_t n = static_cast<std::int32_t>(joan::orig::as_music_stream(img)->width) *
                           static_cast<std::int32_t>(joan::orig::as_music_stream(img)->height);
    return std::memset(joan::orig::arg::mptr(joan::orig::as_music_stream(img)->pixels), 0, joan::orig::arg::u32(n));
}

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("Image::clear", 0x005aa5c0, &image_clear);
#undef B

}  // namespace game_b98_05
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

using joan::orig::helpers::memcpy_;  // memcpy

// ------------------------------------------------------------------------------- cdecl
void* JOAN_CDECL copy_pixels(void* img, u8* src) {  // 0x005aa770
    const u32 n = static_cast<u32>(static_cast<i32>(joan::orig::as_music_stream(img)->width)) *
                  static_cast<u32>(static_cast<i32>(joan::orig::as_music_stream(img)->height));
    return memcpy_(joan::orig::as_music_stream(img)->pixels, src + 6, n);
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Image::copy_pixels", 0x005aa770, &copy_pixels);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

JOAN_COUNTER(c_rle_blit, "rle_blit16");

// 0x005aa7c0
void JOAN_CDECL rle_blit16(void* desc, const std::uint8_t* src) {
    c_rle_blit.hit();
    std::int32_t pos = 8;
    std::uintptr_t dst = reinterpret_cast<std::uintptr_t>(joan::orig::as_music_stream(desc)->pixels);
    std::uint16_t rows = at<std::uint16_t>(src, 6);
    while (rows != 0) {
        std::int32_t x = 0;
        std::int16_t n = at<std::int16_t>(src, pos);
        pos += 2;
        if (n < 0) {
            dst += static_cast<std::uintptr_t>(static_cast<std::intptr_t>(static_cast<std::int32_t>(joan::orig::as_music_stream(desc)->width) * -static_cast<std::int32_t>(n)));
            continue;
        }
        for (;;) {
            const std::int16_t old = n;
            n = static_cast<std::int16_t>(n - 1);
            if (old <= 0) break;
            x += src[pos];
            pos += 1;
            std::int32_t c = static_cast<std::int8_t>(src[pos]);
            pos += 1;
            if (c >= 0) {
                for (;;) {
                    const std::int32_t oc = c;
                    c -= 1;
                    if (oc <= 0) break;
                    *reinterpret_cast<std::uint16_t*>(dst + static_cast<std::uintptr_t>(x)) = at<std::uint16_t>(src, pos);
                    x += 2;
                    pos += 2;
                }
            } else {
                const std::uint16_t v = at<std::uint16_t>(src, pos);
                pos += 2;
                for (;;) {
                    const std::int32_t oc = c;
                    c += 1;
                    if (oc >= 0) break;
                    *reinterpret_cast<std::uint16_t*>(dst + static_cast<std::uintptr_t>(x)) = v;
                    x += 2;
                }
            }
        }
        dst += static_cast<std::uintptr_t>(static_cast<std::intptr_t>(joan::orig::as_music_stream(desc)->width));
        rows = static_cast<std::uint16_t>(rows - 1);
    }
}

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("rle_blit16", 0x005aa7c0, &rle_blit16);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

JOAN_COUNTER(c_gif_palette, "gif_decode_palette");

// ---------------------------------------------------------------------------- 0x005aa980
static void JOAN_CDECL gif_decode_palette(u8* ctx, const u8* data) {
    c_gif_palette.hit();
    i32 pos = 8;
    i32 idx = 0;
    u16 count = at<u16>(data, 6);
    for (;;) {
        const u16 c = count;
        count = static_cast<u16>(count - 1);
        if (c == 0) break;
        idx += data[pos];
        ++pos;
        i32 n = data[pos];
        ++pos;
        if (n == 0) n = 0x100;
        for (;;) {
            const i32 m = n;
            n = n - 1;
            if (m <= 0) break;
            for (i32 k = 0; k < 3; ++k) {
                joan::orig::as_music_stream(ctx)->palette[idx * 3 + k] = static_cast<u8>(static_cast<i32>(data[pos]) >> 2);
                ++pos;
            }
            ++idx;
        }
    }
    if (joan::orig::as_music_stream(ctx)->palette_cb != nullptr) {
        reinterpret_cast<u32(JOAN_CDECL*)(u8*)>(joan::orig::as_music_stream(ctx)->palette_cb)(joan::orig::as_music_stream(ctx)->palette);
    }
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("gif_decode_palette", 0x005aa980, &gif_decode_palette);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b104_09.cpp (game/b104_09)
namespace {
namespace game_b104_09 {

// 0x005aaad0
static u32 JOAN_CDECL set_a67638(u32 v) {
    global<std::uintptr_t>(d::g_a67638) = v;
    return (v & 0xffffff00u) | 1u;  // mov al, 1 over eax = v
}

#define B(name, addr, fn) JOAN_REPLACE("game/b104_09", name, addr, fn)
B("set_a67638", 0x005aaad0, &set_a67638);
#undef B

}  // namespace game_b104_09
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

// 0x005aaaf0: calls the cdecl hook [0xa67638](arg) when set.
void JOAN_CDECL call_hook_a67638(const char* arg) {
    if (global<std::uintptr_t>(d::g_a67638) != 0) {
        reinterpret_cast<std::uint32_t(JOAN_CDECL*)(const char*)>(global<std::uintptr_t>(d::g_a67638))(arg);
    }
}

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("call_hook_a67638", 0x005aaaf0, &call_hook_a67638);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::tfanimation_new_file(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b95_05::anim_new_file>(a0)); }  // 0x5a93f0
auto joan::orig::tfanimation_new_mem(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b97_05::anim_new_mem>(a0)); }  // 0x5a97b0
auto joan::orig::music_stream::release(void* a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b91_03::stream_release>(a0)); }  // 0x5a98a0
auto joan::orig::export_(void* a0, void* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b93_03::rec_export>(a0, a1)); }  // 0x5a99e0
auto joan::orig::music_stream::read_header(void* a0, void* a1) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b91_03::stream_read_header>(a0, a1)); }  // 0x5a9ad0
auto joan::orig::music_stream::reset(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b91_03::stream_reset>(a0)); }  // 0x5a9b40
auto joan::orig::set_buffers(void* a0, void* a1, void* a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b93_03::rec_set_buffers>(a0, a1, a2)); }  // 0x5a9b90
auto joan::orig::tfframe::decode(void* a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b102_10::tf_frame_decode>(a0)); }  // 0x5a9d80
auto joan::orig::music_stream::rewind(void* a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b91_03::stream_rewind>(a0)); }  // 0x5aa040
auto joan::orig::read_u32(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b94_05::read_u32>(a0)); }  // 0x5aa130
auto joan::orig::msg::dispatch_5aa1b0(void* a0, void* a1) -> void { joan::bind::free<&::game_b101_11::msg_dispatch>(a0, a1); }  // 0x5aa1b0
auto joan::orig::fli_color256(void* a0, void* a1) -> void { joan::bind::free<&::game_b100_10::fli_color256>(a0, a1); }  // 0x5aa2e0
auto joan::orig::rle_blit(void* a0, void* a1) -> void { joan::bind::free<&::game_b96_06::rle_blit>(a0, a1); }  // 0x5aa420
auto joan::orig::image::clear(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b98_05::image_clear>(a0)); }  // 0x5aa5c0
auto joan::orig::fli_brun(void* a0, void* a1) -> void { joan::bind::free<&::game_b100_10::fli_brun>(a0, a1); }  // 0x5aa610
auto joan::orig::image::copy_pixels(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b95_06::copy_pixels>(a0, a1)); }  // 0x5aa770
auto joan::orig::rle_blit16(void* a0, void* a1) -> void { joan::bind::free<&::game_b97_06::rle_blit16>(a0, a1); }  // 0x5aa7c0
auto joan::orig::gif_decode_palette(void* a0, void* a1) -> void { joan::bind::free<&::game_b99_07::gif_decode_palette>(a0, a1); }  // 0x5aa980
auto joan::orig::set_a67638(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b104_09::set_a67638>(a0)); }  // 0x5aaad0
auto joan::orig::call_hook_a67638(const char* a0) -> void { joan::bind::free<&::game_b90_04::call_hook_a67638>(a0); }  // 0x5aaaf0
