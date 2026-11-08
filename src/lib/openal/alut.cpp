// Original library sources (re/attribution.csv original_file, under lib/): openal/alut.c.
//
// Batch 113 / list 05: the import-wrapper end of joan.exe (0x7a8940..0x7a9e80) - OpenAL's ALUT
// helper (statically linked alut.lib of the OpenAL 1.0 SDK), the IJG libjpeg 6b compression API
// (game copy, struct packed with /Zp1), DirectInput / DirectSound import stubs and one D3DX8
// resource helper.
//
// Identified roots (full table with the context functions: scratchpad w113_05/ids.md):
//   0x00788877 D3DX8 internal CD3DXResource::Load(HMODULE, LPCTSTR name, BOOL unicode) thiscall
//              ret 0xc (FindResource(type RT_BITMAP) / SizeofResource / LoadResource /
//              LockResource, HRESULT_FROM_WIN32(GetLastError())).  Not installable: prologue
//              push ebp / mov ebp,esp / push esi (rejected by patch_entry).
//   0x007a8940 DirectInput8Create import stub (jmp [0xa6d800]) - not installable.
//   0x007a9390 DirectSoundCreate import stub (jmp [0xa6d830], DSOUND ord 1) - later replaced, deleted in
//              cleanup C2 (no DirectSound device is made).
//   0x007a9420 alutInit(int* argc, char** argv)                    cdecl  -> IMPLEMENTED
//   0x007a9457 alutExit(void)                                      cdecl  -> IMPLEMENTED
//   0x007a949a alutLoadWAVFile(file, format*, data**, size*, freq*, loop*)   -> IMPLEMENTED (fix/uninit)
//   0x007a973c alutLoadWAVMemory(mem, format*, data**, size*, freq*, loop*)  -> IMPLEMENTED (fix/uninit)
//   0x007a9989 alutUnloadWAV(format, data, size, freq) = free(data) - not installable (prologue
//              push ebp / mov ebp,esp / cmp).
//   0x007a9a60 jpeg_CreateCompress(cinfo, version 62, size 0x15b)  - libjpeg object, no frame prologue.
//   0x007a9b30 jpeg_destroy_compress -> jmp 0x00788919 (D3DX's jpeg_destroy) - jmp stub.
//   0x007a9b40 jpeg_abort_compress   -> jmp 0x007888f7 (D3DX's jpeg_abort)   - jmp stub.
//   0x007a9ba0 jpeg_finish_compress, 0x007a9e00 jpeg_start_compress(cinfo, write_all_tables),
//   0x007a9e80 jpeg_write_scanlines - libjpeg object (vtable calls through cinfo->err / mem /
//              master ...), prologue push esi: not installable.
//
// Module "lib/b113_05": alutInit 0x007a9420, alutExit 0x007a9457.
// Module "fix/uninit":  alutLoadWAVFile 0x007a949a, alutLoadWAVMemory 0x007a973c.
//   Both originals read uninitialised stack locals when the input is not a well-formed RIFF/WAVE:
//   the 'fmt ' fields ([ebp-0x68] block) when a 'data' chunk comes before any 'fmt ' chunk, the
//   RIFF size [ebp-0x50] when fread of the 12-byte header fails (file version), and the 'fmt ' /
//   'smpl' blocks when a fread is short. Here those locals start zeroed (=> a 'data' chunk before
//   'fmt ' is ignored; an empty file gives RIFF size 0 -> remaining 0xfffffffc, and the loop stops
//   at the first chunk-header fread returning 0). For well-formed WAV files (every game sound)
//   the behaviour is identical to the original, call for call.
//
// All CRT calls go to the original CRT entry points (fopen 0x7b3fe0, fread 0x7b6b3d, fseek
// 0x7b6e63, fclose 0x7b3ef6, malloc 0x7b6c54, memset 0x7b1690, memcmp 0x7b2620, memcpy 0x7b2a20),
// so the CRT heap / stdio state stays the original's. OpenAL calls go through the original import
// stubs 0x7a9a36..0x7a9a5a (cdecl, jmp [IAT]).
#include "../../callconv.hpp"
#include "../../original/helpers.hpp"
#include <cstdint>

#include "../../original/direct.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/bind.hpp"

namespace {
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- original CRT entry points (statically linked MSVC 6 CRT)
void* crt_fopen(const char* name, const char* mode) { return joan::orig::arg::as<void*>(joan::orig::libc::fopen(joan::orig::arg::str(name), joan::orig::arg::str(mode))); }
u32 crt_fread(void* p, u32 size, u32 n, void* fp) { return joan::orig::arg::as<u32>(joan::orig::libc::fread(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::u32(n), joan::orig::arg::mptr(fp))); }
i32 crt_fseek(void* fp, i32 off, i32 whence) { return joan::orig::arg::as<i32>(joan::orig::libc::fseek(joan::orig::arg::mptr(fp), joan::orig::arg::i32(off), joan::orig::arg::i32(whence))); }
i32 crt_fclose(void* fp) { return joan::orig::arg::as<i32>(joan::orig::libc::fclose(joan::orig::arg::mptr(fp))); }
void* crt_malloc(u32 n) { return joan::orig::arg::as<void*>(joan::orig::malloc_(joan::orig::arg::u32(n))); }
using joan::orig::helpers::crt_memset;
i32 crt_memcmp(const void* a, const void* b, u32 n) { return joan::orig::arg::as<i32>(joan::orig::libc::memcmp(joan::orig::arg::mptr(a), joan::orig::arg::mptr(b), joan::orig::arg::u32(n))); }
using joan::orig::helpers::crt_memcpy;

// ---- OpenAL import stubs (cdecl)
i32 alc_make_context_current(void* ctx) { return joan::orig::arg::as<i32>(joan::orig::openal::alc_make_context_current(joan::orig::arg::mptr(ctx))); }
void* alc_create_context(void* dev, const i32* attrs) { return joan::orig::arg::as<void*>(joan::orig::openal::alc_create_context(joan::orig::arg::mptr(dev), joan::orig::arg::mptr(attrs))); }
void* alc_open_device(const char* name) { return joan::orig::arg::as<void*>(joan::orig::openal::alc_open_device(joan::orig::arg::str(name))); }
i32 alc_close_device(void* dev) { return joan::orig::arg::as<i32>(joan::orig::openal::alc_close_device(joan::orig::arg::mptr(dev))); }
i32 alc_destroy_context(void* ctx) { return joan::orig::arg::as<i32>(joan::orig::openal::alc_destroy_context(joan::orig::arg::mptr(ctx))); }
void* alc_get_contexts_device(void* ctx) { return joan::orig::arg::as<void*>(joan::orig::openal::alc_get_contexts_device(joan::orig::arg::mptr(ctx))); }
void* alc_get_current_context() { return joan::orig::arg::as<void*>(joan::orig::openal::alc_get_current_context()); }

const char* rdata(img::Addr a) { return &img::global<const char>(a); }

// Stack blocks of the two WAV parsers (same frame layout in both).
struct Riff {     // [ebp-0x54]
    u32 id;       // 'RIFF'
    u32 size;     // [ebp-0x50]
    u32 wave;     // 'WAVE'
};
struct Chunk {    // [ebp-0xc]
    u8 id[4];
    u32 size;     // [ebp-0x8]
};
struct Fmt {      // [ebp-0x68], 16 bytes read
    u16 tag;      // -0x68
    u16 channels; // -0x66
    u32 rate;     // -0x64
    u32 avg;      // -0x60
    u16 align;    // -0x5c
    u16 bits;     // -0x5a
};
struct Smpl {     // [ebp-0x48], 0x3c bytes read
    u32 w[15];    // w[7] = [ebp-0x2c] = number of sample loops
};
static_assert(sizeof(Riff) == 12 && sizeof(Chunk) == 8 && sizeof(Fmt) == 16 && sizeof(Smpl) == 0x3c);

// AL_FORMAT_MONO8 0x1100, MONO16 0x1101, STEREO8 0x1102, STEREO16 0x1103.
i32 al_format(const Fmt& fmt) {
    if (fmt.channels == 1) return fmt.bits != 8 ? 0x1101 : 0x1100;
    return fmt.bits != 8 ? 0x1103 : 0x1102;
}

// data + size computed as an integer: the original also does it (and memsets there) when malloc
// returned NULL; pointer arithmetic on a null pointer would be UB in C++.
void* tail(void* p, u32 n) { return reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(p) + n); }

JOAN_COUNTER(c_wav_file, "alutLoadWAVFile");
JOAN_COUNTER(c_wav_memory, "alutLoadWAVMemory");

// 0x007a9420 alutInit(int*, char**): arguments ignored; returns alcMakeContextCurrent's eax.
i32 JOAN_CDECL alut_init(i32* /*argc*/, char** /*argv*/) {
    void* const dev = alc_open_device(rdata(d::s_DirectSound3D));  // "DirectSound3D"
    void* const ctx = alc_create_context(dev, nullptr);
    return alc_make_context_current(ctx);
}

// 0x007a9457 alutExit(): returns alcCloseDevice's eax.
i32 JOAN_CDECL alut_exit() {
    void* const ctx = alc_get_current_context();
    void* const dev = alc_get_contexts_device(ctx);
    alc_make_context_current(nullptr);
    alc_destroy_context(ctx);
    return alc_close_device(dev);
}

// 0x007a949a alutLoadWAVFile(const char* file, ALenum* format, void** data, ALsizei* size,
// ALsizei* freq, ALboolean* loop). All sizes unsigned 32-bit (the original's `div` / wrap-around).
// Uninitialised in the original (see the header), zero here: riff, fmt, smpl.
void JOAN_CDECL load_wav_file(const char* file, i32* format, void** data, u32* size, u32* freq, u8* loop) {
    c_wav_file.hit();
    *format = 0x1101;
    *data = nullptr;
    *size = 0;
    *freq = 0x5622;
    *loop = 0;
    if (file == nullptr) return;
    void* const fp = crt_fopen(file, rdata(d::s_rb_a308ec));  // "rb"
    if (fp == nullptr) return;
    Riff riff{};
    Chunk chunk{};
    Fmt fmt{};
    Smpl smpl{};
    u32 extra = 0;  // [ebp-0x4], written and never read
    crt_fread(&riff, 1, 0xc, fp);  // result unchecked
    riff.size = ((riff.size + 1) & 0xfffffffeu) - 4;
    while (riff.size != 0) {
        if (crt_fread(&chunk, 1, 8, fp) == 0) break;
        if (crt_memcmp(chunk.id, rdata(d::s_fmt_a308f0), 4) == 0) {  // "fmt "
            crt_fread(&fmt, 1, 0x10, fp);
            if (fmt.tag == 1) {
                *format = al_format(fmt);
                *freq = fmt.rate;
                crt_fseek(fp, static_cast<i32>(chunk.size - 0x10), 1);
            } else {
                crt_fread(&extra, 1, 4, fp);
                crt_fseek(fp, static_cast<i32>(chunk.size - 0x14), 1);
            }
        } else if (crt_memcmp(chunk.id, rdata(d::s_data_a308f8), 4) == 0) {  // "data"
            if (fmt.tag == 1) {
                *size = chunk.size;
                *data = crt_malloc(chunk.size + 0x1f);
                if (*data != nullptr) crt_fread(*data, fmt.align, chunk.size / fmt.align, fp);
                crt_memset(tail(*data, chunk.size), 0, 0x1f);  // also when malloc failed
            }
        } else if (crt_memcmp(chunk.id, rdata(d::s_smpl_a30900), 4) == 0) {  // "smpl"
            crt_fread(&smpl, 1, 0x3c, fp);
            *loop = smpl.w[7] != 0 ? 1 : 0;
            crt_fseek(fp, static_cast<i32>(chunk.size - 0x3c), 1);
        } else {
            crt_fseek(fp, static_cast<i32>(chunk.size), 1);
        }
        crt_fseek(fp, static_cast<i32>(chunk.size & 1), 1);
        riff.size -= ((chunk.size + 1) & 0xfffffffeu) + 8;
    }
    crt_fclose(fp);
    (void)extra;
}

// 0x007a973c alutLoadWAVMemory(ALbyte* memory, format*, data**, size*, freq*, loop*): the same
// parser over a memory block (memcpy instead of fread, pointer advance instead of fseek).
// Uninitialised in the original (see the header), zero here: fmt, smpl.
void JOAN_CDECL load_wav_memory(const u8* memory, i32* format, void** data, u32* size, u32* freq, u8* loop) {
    c_wav_memory.hit();
    *format = 0x1101;
    *data = nullptr;
    *size = 0;
    *freq = 0x5622;
    *loop = 0;
    if (memory == nullptr) return;
    const u8* p = memory;
    if (p == nullptr) return;
    Riff riff{};
    Chunk chunk{};
    Fmt fmt{};
    Smpl smpl{};
    u32 extra = 0;  // [ebp-0x4], written and never read
    crt_memcpy(&riff, p, 0xc);
    p += 0xc;
    riff.size = ((riff.size + 1) & 0xfffffffeu) - 4;  // `and al,0xfe` == & ~1
    while (riff.size != 0) {
        if (crt_memcpy(&chunk, p, 8) == nullptr) break;  // never: memcpy returns its destination
        p += 8;
        if (crt_memcmp(chunk.id, rdata(d::s_fmt_a30908), 4) == 0) {  // "fmt "
            crt_memcpy(&fmt, p, 0x10);
            if (fmt.tag == 1) {
                *format = al_format(fmt);
                *freq = fmt.rate;
            } else {
                crt_memcpy(&extra, p, 4);
            }
            p += chunk.size;
        } else if (crt_memcmp(chunk.id, rdata(d::s_data_a30910), 4) == 0) {  // "data"
            if (fmt.tag == 1) {
                *size = chunk.size;
                *data = crt_malloc(chunk.size + 0x1f);
                if (*data != nullptr) crt_memcpy(*data, p, chunk.size);
                crt_memset(tail(*data, chunk.size), 0, 0x1f);  // also when malloc failed
                p += chunk.size;
            }
        } else if (crt_memcmp(chunk.id, rdata(d::s_smpl_a30918), 4) == 0) {  // "smpl"
            crt_memcpy(&smpl, p, 0x3c);
            *loop = smpl.w[7] != 0 ? 1 : 0;
            p += chunk.size;
        } else {
            p += chunk.size;
        }
        p += chunk.size & 1;
        riff.size -= ((chunk.size + 1) & 0xfffffffeu) + 8;
    }
    (void)extra;
}

JOAN_REPLACE("lib/b113_05", "alutInit", 0x007a9420, &alut_init);
JOAN_REPLACE("lib/b113_05", "alutExit", 0x007a9457, &alut_exit);
// fix/uninit: the originals read uninitialised fmt / smpl / RIFF-size locals on malformed WAV data
// (a 'data' chunk before 'fmt ', a short read); zero here. Identical for well-formed files.
JOAN_REPLACE("fix/uninit", "alutLoadWAVFile", 0x007a949a, &load_wav_file);
JOAN_REPLACE("fix/uninit", "alutLoadWAVMemory", 0x007a973c, &load_wav_memory);
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::alut_init(void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::alut_init>(a0, a1)); }  // 0x7a9420
auto joan::orig::alut_exit() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::alut_exit>()); }  // 0x7a9457
auto joan::orig::alut_load_wavfile(const char* a0, void* a1, void* a2, void* a3, void* a4, void* a5) -> void { joan::bind::free<&::load_wav_file>(a0, a1, a2, a3, a4, a5); }  // 0x7a949a
auto joan::orig::alut_load_wavmemory(void* a0, void* a1, void* a2, void* a3, void* a4, void* a5) -> void { joan::bind::free<&::load_wav_memory>(a0, a1, a2, a3, a4, a5); }  // 0x7a973c
