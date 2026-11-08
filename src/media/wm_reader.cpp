// Windows Media's synchronous reader for the game, decoding with FFmpeg (phase C3, docs/RUNTIME.md).
//
// The game plays its movies (movie\*.wmv: WMV3 video + WMA v2 audio) and its music and ambience (music\*.wma) through
// one class (uti3d/video.cpp, Video): the original loaded wmvcore.dll, created an IWMSyncReader (WMCreateSyncReader), opens
// the file, walks its IWMProfile streams, asks for RGB565 video (rows bottom-up, copied into a texture) and 16-bit
// PCM audio (queued as OpenAL buffers), and then pulls samples with GetNextSample, seeking with SetRange. Here those
// interfaces are ours, with exactly the calls the game makes, and media_file.cpp decodes with FFmpeg into the formats
// the game asks for. wmvcore.dll (Windows Media Format runtime, missing on Windows "N" editions and on Linux) is not
// loaded (the game's IWMWriter screen capture, uti3d/videocap.cpp, is a no-op since cleanup C1).
//
// Interfaces (vtable order as in wmsdkidl.h; methods the game never calls answer E_NOTIMPL):
//   IWMSyncReader: Open, Close, SetRange, GetNextSample, GetOutputProps, SetOutputProps, GetOutputFormatCount,
//     GetOutputFormat, GetOutputNumberForStream, GetStreamNumberForOutput, GetOutputCount; QueryInterface for
//     IWMProfile.
//   IWMProfile: GetStreamCount, GetStream. IWMStreamConfig: GetStreamType, GetStreamNumber.
//   IWMOutputMediaProps: GetType, GetMediaType, SetMediaType. INSSBuffer: GetLength, GetMaxLength, GetBuffer,
//     GetBufferAndLength.
#include "wm_reader.hpp"

#include "../platform/win32_types.hpp"

#include <cstdarg>
#include <atomic>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "../registry.hpp"
#include "media_file.hpp"
#include "../platform/ini.hpp"
#include "../platform/window.hpp"
#include "../platform/files.hpp"
#include "../platform/text.hpp"
#include "../ilib/sound_archives.hpp"

extern "C" {
#include <libavcodec/version.h>
#include <libavformat/version.h>
#include <libavutil/log.h>
}

namespace joan::media {
namespace {

// ---------------------------------------------------------------- constants

constexpr HRESULT kOk = 0;
constexpr HRESULT kNotImpl = static_cast<HRESULT>(0x80004001u);      // E_NOTIMPL
constexpr HRESULT kNoInterface = static_cast<HRESULT>(0x80004002u);  // E_NOINTERFACE
constexpr HRESULT kPointer = static_cast<HRESULT>(0x80004003u);      // E_POINTER
constexpr HRESULT kInvalidArg = static_cast<HRESULT>(0x80070057u);   // E_INVALIDARG
constexpr HRESULT kFail = static_cast<HRESULT>(0x80004005u);         // E_FAIL
constexpr HRESULT kNoMoreSamples = static_cast<HRESULT>(0xC00D0BCFu);  // NS_E_NO_MORE_SAMPLES
constexpr HRESULT kFileNotFound = static_cast<HRESULT>(0xC00D001Au);   // NS_E_FILE_NOT_FOUND
constexpr HRESULT kBufferTooSmall = static_cast<HRESULT>(0xC00D07D1u);  // ASF_E_BUFFERTOOSMALL
constexpr HRESULT kInvalidOutput = static_cast<HRESULT>(0xC00D0041u);   // NS_E_INVALID_OUTPUT_FORMAT
constexpr HRESULT kNotOpen = static_cast<HRESULT>(0xC00D001Du);         // NS_E_INVALID_REQUEST

constexpr GUID kIidUnknown = {0x00000000, 0x0000, 0x0000, {0xC0, 0, 0, 0, 0, 0, 0, 0x46}};
constexpr GUID kIidProfile = {0x96406bdb, 0x2b2b, 0x11d3, {0xb3, 0x6b, 0x00, 0xc0, 0x4f, 0x61, 0x08, 0xff}};
constexpr GUID kMajorVideo = {0x73646976, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};  // 'vids'
constexpr GUID kMajorAudio = {0x73647561, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};  // 'auds'
constexpr GUID kSubRgb565 = {0xe436eb7b, 0x524f, 0x11ce, {0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}};
constexpr GUID kSubPcm = {0x00000001, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};
constexpr GUID kFormatVideoInfo = {0x05589f80, 0xc356, 0x11ce, {0xbf, 0x01, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a}};
constexpr GUID kFormatWaveFormatEx = {0x05589f81, 0xc356, 0x11ce, {0xbf, 0x01, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a}};

bool same(const GUID* a, const GUID& b) { return a && std::memcmp(a, &b, sizeof(GUID)) == 0; }

// WM_MEDIA_TYPE (wmsdkidl.h, natural alignment), 0x48 bytes on x86 and 0x58 on x64; the format block follows it in
// the caller's buffer.
struct WmMediaType {
    GUID majortype;
    GUID subtype;
    BOOL bFixedSizeSamples;
    BOOL bTemporalCompression;
    ULONG lSampleSize;
    GUID formattype;
    void* pUnk;
    ULONG cbFormat;
    BYTE* pbFormat;
};
#pragma pack(push, 4)
struct VideoInfoHeader {  // VIDEOINFOHEADER + the three BI_BITFIELDS masks of RGB565
    RECT rcSource;
    RECT rcTarget;
    DWORD dwBitRate;
    DWORD dwBitErrorRate;
    LONGLONG AvgTimePerFrame;
    BITMAPINFOHEADER bmiHeader;
    DWORD masks[3];
};
#pragma pack(pop)
#pragma pack(push, 1)
struct WaveFormat {  // WAVEFORMATEX (byte-packed: 18 bytes)
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
    WORD wBitsPerSample;
    WORD cbSize;
};
#pragma pack(pop)
static_assert(sizeof(WmMediaType) == (sizeof(void*) == 4 ? 0x48 : 0x58), "WM_MEDIA_TYPE");
static_assert(sizeof(VideoInfoHeader) == 0x58 + 12, "VIDEOINFOHEADER");
static_assert(sizeof(WaveFormat) == 18, "WAVEFORMATEX");

/// The media type (with its format block) of one output: the only format offered for it.
std::vector<std::uint8_t> media_type_of(const StreamInfo& s) {
    std::vector<std::uint8_t> out(sizeof(WmMediaType) + (s.video ? sizeof(VideoInfoHeader) : sizeof(WaveFormat)), 0);
    WmMediaType mt{};
    if (s.video) {
        VideoInfoHeader v{};
        v.rcSource = {0, 0, s.width, s.height};
        v.rcTarget = v.rcSource;
        v.AvgTimePerFrame = s.frame_duration;
        v.dwBitRate = static_cast<DWORD>(static_cast<LONGLONG>(s.width) * s.height * 16 * 10000000 /
                                         (s.frame_duration > 0 ? s.frame_duration : 333333));
        v.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        v.bmiHeader.biWidth = s.width;
        v.bmiHeader.biHeight = s.height;  // positive: bottom-up rows
        v.bmiHeader.biPlanes = 1;
        v.bmiHeader.biBitCount = 16;
        v.bmiHeader.biCompression = BI_BITFIELDS;
        v.bmiHeader.biSizeImage = static_cast<DWORD>(s.width * s.height * 2);
        v.masks[0] = 0xf800;
        v.masks[1] = 0x07e0;
        v.masks[2] = 0x001f;
        mt.majortype = kMajorVideo;
        mt.subtype = kSubRgb565;
        mt.bFixedSizeSamples = TRUE;
        mt.lSampleSize = v.bmiHeader.biSizeImage;
        mt.formattype = kFormatVideoInfo;
        mt.cbFormat = sizeof(v);
        std::memcpy(out.data() + sizeof(mt), &v, sizeof(v));
    } else {
        WaveFormat w{};
        w.wFormatTag = 1;  // WAVE_FORMAT_PCM
        w.nChannels = static_cast<WORD>(s.channels);
        w.nSamplesPerSec = static_cast<DWORD>(s.rate);
        w.wBitsPerSample = 16;
        w.nBlockAlign = static_cast<WORD>(s.channels * 2);
        w.nAvgBytesPerSec = w.nSamplesPerSec * w.nBlockAlign;
        mt.majortype = kMajorAudio;
        mt.subtype = kSubPcm;
        mt.bFixedSizeSamples = TRUE;
        mt.lSampleSize = w.nBlockAlign;
        mt.formattype = kFormatWaveFormatEx;
        mt.cbFormat = sizeof(w);
        std::memcpy(out.data() + sizeof(mt), &w, sizeof(w));
    }
    std::memcpy(out.data(), &mt, sizeof(mt));
    return out;
}

// ---------------------------------------------------------------- COM plumbing

/// Every object: its vtable first, then a reference count; Release deletes through `destroy`.
struct Object {
    const void* const* vtbl;
    std::atomic<LONG> refs{1};
    void (*destroy)(Object*);
};

template <class T>
void destroy_as(Object* o) {
    delete static_cast<T*>(o);
}
HRESULT WINAPI unknown_qi(Object* self, const GUID* iid, void** out) {
    if (!out) return kPointer;
    if (same(iid, kIidUnknown)) {
        ++self->refs;
        *out = self;
        return kOk;
    }
    *out = nullptr;
    return kNoInterface;
}
ULONG WINAPI add_ref(Object* self) { return static_cast<ULONG>(++self->refs); }
ULONG WINAPI release(Object* self) {
    const LONG n = --self->refs;
    if (n == 0) self->destroy(self);
    return static_cast<ULONG>(n);
}
// Methods the game never calls: E_NOTIMPL, each with the stack size of its arguments (stdcall: the callee pops them;
// this + N dwords, QWORD arguments count as two).
HRESULT WINAPI ni1(void*, DWORD) { return kNotImpl; }
HRESULT WINAPI ni2(void*, DWORD, DWORD) { return kNotImpl; }
HRESULT WINAPI ni3(void*, DWORD, DWORD, DWORD) { return kNotImpl; }
HRESULT WINAPI ni5(void*, DWORD, DWORD, DWORD, DWORD, DWORD) { return kNotImpl; }

#define FN(f) reinterpret_cast<const void*>(&f)
#define NI1 FN(ni1)
#define NI2 FN(ni2)
#define NI3 FN(ni3)
#define NI5 FN(ni5)

// ---------------------------------------------------------------- INSSBuffer

struct Buffer : Object {
    std::vector<std::uint8_t> data;
};
HRESULT WINAPI buf_get_length(Buffer* self, DWORD* len) {
    if (!len) return kPointer;
    *len = static_cast<DWORD>(self->data.size());
    return kOk;
}
HRESULT WINAPI buf_set_length(Buffer* self, DWORD len) {
    if (len > self->data.size()) return kInvalidArg;
    self->data.resize(len);
    return kOk;
}
HRESULT WINAPI buf_get_buffer(Buffer* self, BYTE** p) {
    if (!p) return kPointer;
    *p = self->data.data();
    return kOk;
}
HRESULT WINAPI buf_get_buffer_and_length(Buffer* self, BYTE** p, DWORD* len) {
    if (!p || !len) return kPointer;
    *p = self->data.data();
    *len = static_cast<DWORD>(self->data.size());
    return kOk;
}
const void* const kBufferVtbl[] = {FN(unknown_qi), FN(add_ref), FN(release), FN(buf_get_length), FN(buf_set_length),
                                   FN(buf_get_length) /* GetMaxLength */, FN(buf_get_buffer),
                                   FN(buf_get_buffer_and_length)};

// ---------------------------------------------------------------- IWMOutputMediaProps

struct Props : Object {
    std::vector<std::uint8_t> type;  // WM_MEDIA_TYPE + format block
};
HRESULT WINAPI props_get_type(Props* self, GUID* major) {
    if (!major) return kPointer;
    std::memcpy(major, self->type.data(), sizeof(GUID));
    return kOk;
}
HRESULT WINAPI props_get_media_type(Props* self, void* type, DWORD* size) {
    if (!size) return kPointer;
    const DWORD need = static_cast<DWORD>(self->type.size());
    if (!type) {
        *size = need;
        return kOk;
    }
    if (*size < need) {
        *size = need;
        return kBufferTooSmall;
    }
    std::memcpy(type, self->type.data(), need);
    auto* mt = static_cast<WmMediaType*>(type);
    mt->pbFormat = static_cast<BYTE*>(type) + sizeof(WmMediaType);
    *size = need;
    return kOk;
}
HRESULT WINAPI props_set_media_type(Props* self, const void* type) {
    if (!type) return kPointer;
    const auto* mt = static_cast<const WmMediaType*>(type);
    // Only our own format is decodable; the game picks it from GetOutputFormat (the same type).
    if (std::memcmp(&mt->subtype, self->type.data() + offsetof(WmMediaType, subtype), sizeof(GUID)) != 0)
        return kInvalidOutput;
    return kOk;
}
const void* const kPropsVtbl[] = {FN(unknown_qi), FN(add_ref), FN(release), FN(props_get_type),
                                  FN(props_get_media_type), FN(props_set_media_type), NI2 /* GetStreamGroupName */,
                                  NI2 /* GetConnectionName */};

Props* new_props(const StreamInfo& s) {
    auto* p = new Props;
    p->vtbl = kPropsVtbl;
    p->destroy = &destroy_as<Props>;
    p->type = media_type_of(s);
    return p;
}

// ---------------------------------------------------------------- the reader, its profile and stream configs

struct Reader;

struct StreamConfig : Object {
    StreamInfo info;
};
HRESULT WINAPI cfg_get_stream_type(StreamConfig* self, GUID* type) {
    if (!type) return kPointer;
    *type = self->info.video ? kMajorVideo : kMajorAudio;
    return kOk;
}
HRESULT WINAPI cfg_get_stream_number(StreamConfig* self, WORD* number) {
    if (!number) return kPointer;
    *number = self->info.number;
    return kOk;
}
// IWMStreamConfig: GetStreamType, GetStreamNumber, SetStreamNumber, GetStreamName, SetStreamName, GetConnectionName,
// SetConnectionName, GetBitrate, SetBitrate, GetBufferWindow, SetBufferWindow.
const void* const kStreamConfigVtbl[] = {FN(unknown_qi), FN(add_ref), FN(release), FN(cfg_get_stream_type),
                                         FN(cfg_get_stream_number), NI1, NI2, NI1, NI2, NI1, NI1, NI1, NI1, NI1};

struct Profile : Object {
    Reader* reader = nullptr;  // holds a reference
};

struct Reader : Object {
    MediaFile file;
    bool open = false;
};

HRESULT WINAPI profile_get_stream_count(Profile* self, DWORD* n) {
    if (!n) return kPointer;
    *n = static_cast<DWORD>(self->reader->file.streams().size());
    return kOk;
}
HRESULT WINAPI profile_get_stream(Profile* self, DWORD index, void** out) {
    if (!out) return kPointer;
    *out = nullptr;
    const auto& streams = self->reader->file.streams();
    if (index >= streams.size()) return kInvalidArg;
    auto* c = new StreamConfig;
    c->vtbl = kStreamConfigVtbl;
    c->destroy = &destroy_as<StreamConfig>;
    c->info = streams[index];
    *out = c;
    return kOk;
}
void destroy_profile(Object* o) {
    auto* p = static_cast<Profile*>(o);
    release(p->reader);
    delete p;
}
// IWMProfile: GetVersion, GetName, SetName, GetDescription, SetDescription, GetStreamCount, GetStream,
// GetStreamByNumber, RemoveStream, RemoveStreamByNumber, AddStream, ReconfigStream, CreateNewStream,
// GetMutualExclusionCount, GetMutualExclusion, RemoveMutualExclusion, AddMutualExclusion, CreateNewMutualExclusion.
const void* const kProfileVtbl[] = {FN(unknown_qi), FN(add_ref), FN(release), NI1, NI2, NI1, NI2, NI1,
                                    FN(profile_get_stream_count), FN(profile_get_stream), NI2, NI1, NI1, NI1, NI1,
                                    NI2, NI1, NI2, NI1, NI1, NI1};

HRESULT WINAPI reader_qi(Reader* self, const GUID* iid, void** out) {
    if (!out) return kPointer;
    if (same(iid, kIidProfile)) {
        auto* p = new Profile;
        p->vtbl = kProfileVtbl;
        p->destroy = &destroy_profile;
        p->reader = self;
        add_ref(self);
        *out = p;
        return kOk;
    }
    return unknown_qi(self, iid, out);
}

HRESULT WINAPI reader_open(Reader* self, const WCHAR* name) {
    if (!name) return kPointer;
    // [Audio] SoundsInMemory: a music / ambience track from memory (ilib/sound_archives.hpp).
    const std::string game_name = joan::platform::text::utf16_to_utf8(reinterpret_cast<const char16_t*>(name));
    if (auto bytes = joan::sound_archives::music_file(game_name.c_str())) {
        self->open = self->file.open_memory(std::move(bytes), game_name);
        if (self->open) return kOk;
        hook::log_line("video: %s (in memory): %s (FFmpeg) - opening the file", game_name.c_str(), self->file.error().c_str());
    }
    // The game's wide name (UTF-16, as Windows Media took it) as UTF-8 for FFmpeg.
#ifdef _WIN32
    const std::string path = joan::platform::text::utf16_to_utf8(reinterpret_cast<const char16_t*>(name));
#else
    // Elsewhere FFmpeg gets the file as this system names it (slashes, the case of the files on disk).
    const std::string path = joan::platform::files::native_path(
        joan::platform::text::utf16_to_utf8(reinterpret_cast<const char16_t*>(name)).c_str());
#endif
    self->open = self->file.open(path);
    if (!self->open) {
        hook::log_line("video: %s: %s (FFmpeg)", path.c_str(), self->file.error().c_str());
        return kFileNotFound;
    }
    return kOk;
}
HRESULT WINAPI reader_close(Reader* self) {
    self->open = false;
    return kOk;
}
HRESULT WINAPI reader_set_range(Reader* self, ULONGLONG start, LONGLONG /*duration*/) {
    if (!self->open) return kNotOpen;
    return self->file.seek(static_cast<std::int64_t>(start)) ? kOk : kFail;
}
HRESULT WINAPI reader_get_next_sample(Reader* self, WORD stream, void** sample, ULONGLONG* time, ULONGLONG* duration,
                                      DWORD* flags, DWORD* output, WORD* stream_out) {
    if (!sample) return kPointer;
    *sample = nullptr;
    if (!self->open) return kNotOpen;
    Sample s;
    for (;;) {
        if (!self->file.next(s)) return kNoMoreSamples;
        if (stream == 0 || s.stream == stream) break;
    }
    auto* b = new Buffer;
    b->vtbl = kBufferVtbl;
    b->destroy = &destroy_as<Buffer>;
    b->data = std::move(s.data);
    *sample = b;
    if (time) *time = static_cast<ULONGLONG>(s.time);
    if (duration) *duration = static_cast<ULONGLONG>(s.duration);
    if (flags) *flags = 0;
    if (trace_video()) trace_line("  sample stream %u time %.3f bytes %u", s.stream, static_cast<double>(s.time) / 1e7,
                                  static_cast<unsigned>(b->data.size()));
    if (stream_out) *stream_out = s.stream;
    if (output) {
        const auto& streams = self->file.streams();
        for (DWORD i = 0; i < streams.size(); ++i)
            if (streams[i].number == s.stream) *output = i;
    }
    return kOk;
}
HRESULT WINAPI reader_get_output_count(Reader* self, DWORD* n) {
    if (!n) return kPointer;
    *n = static_cast<DWORD>(self->file.streams().size());
    return kOk;
}
HRESULT WINAPI reader_get_output_props(Reader* self, DWORD output, void** out) {
    if (!out) return kPointer;
    *out = nullptr;
    const auto& streams = self->file.streams();
    if (output >= streams.size()) return kInvalidArg;
    *out = new_props(streams[output]);
    return kOk;
}
HRESULT WINAPI reader_set_output_props(Reader* self, DWORD output, void* props) {
    if (!props) return kPointer;
    if (output >= self->file.streams().size()) return kInvalidArg;
    return kOk;  // the props can only hold our own format (props_set_media_type)
}
HRESULT WINAPI reader_get_output_format_count(Reader* self, DWORD output, DWORD* n) {
    if (!n) return kPointer;
    if (output >= self->file.streams().size()) return kInvalidArg;
    *n = 1;
    return kOk;
}
HRESULT WINAPI reader_get_output_format(Reader* self, DWORD output, DWORD format, void** out) {
    if (!out) return kPointer;
    *out = nullptr;
    if (format != 0) return kInvalidArg;
    return reader_get_output_props(self, output, out);
}
HRESULT WINAPI reader_get_output_number_for_stream(Reader* self, WORD stream, DWORD* output) {
    if (!output) return kPointer;
    const auto& streams = self->file.streams();
    for (DWORD i = 0; i < streams.size(); ++i) {
        if (streams[i].number == stream) {
            *output = i;
            return kOk;
        }
    }
    return kInvalidArg;
}
HRESULT WINAPI reader_get_stream_number_for_output(Reader* self, DWORD output, WORD* stream) {
    if (!stream) return kPointer;
    const auto& streams = self->file.streams();
    if (output >= streams.size()) return kInvalidArg;
    *stream = streams[output].number;
    return kOk;
}

// IWMSyncReader (wmsdkidl.h): Open, Close, SetRange, SetRangeByFrame, GetNextSample, SetStreamsSelected,
// GetStreamSelected, SetReadStreamSamples, GetReadStreamSamples, GetOutputSetting, SetOutputSetting, GetOutputCount,
// GetOutputProps, SetOutputProps, GetOutputFormatCount, GetOutputFormat, GetOutputNumberForStream,
// GetStreamNumberForOutput, GetMaxOutputSampleSize, GetMaxStreamSampleSize, OpenStream.
const void* const kReaderVtbl[] = {
    FN(reader_qi), FN(add_ref), FN(release), FN(reader_open), FN(reader_close), FN(reader_set_range), NI5,
    FN(reader_get_next_sample), NI3, NI2, NI2, NI2, NI5, NI5, FN(reader_get_output_count), FN(reader_get_output_props),
    FN(reader_set_output_props), FN(reader_get_output_format_count), FN(reader_get_output_format),
    FN(reader_get_output_number_for_stream), FN(reader_get_stream_number_for_output), NI2, NI2, NI1};

#undef FN
#undef NI1
#undef NI2
#undef NI3
#undef NI5

void log_ffmpeg(void*, int level, const char* fmt, va_list ap) {
    if (level > AV_LOG_ERROR) return;
    char line[512];
    std::vsnprintf(line, sizeof line, fmt, ap);
    std::size_t n = std::strlen(line);
    while (n > 0 && (line[n - 1] == '\n' || line[n - 1] == '\r')) line[--n] = '\0';
    hook::log_line("video: FFmpeg: %s", line);
}

HRESULT WINAPI create_sync_reader(void* /*cert*/, DWORD /*rights*/, void** out) {
    if (!out) return kPointer;
    static const bool logged = [] {
        av_log_set_callback(&log_ffmpeg);
        hook::log_line("video: movies and music decoded by FFmpeg (avformat %u.%u, avcodec %u.%u)",
                       LIBAVFORMAT_VERSION_MAJOR, LIBAVFORMAT_VERSION_MINOR, LIBAVCODEC_VERSION_MAJOR,
                       LIBAVCODEC_VERSION_MINOR);
        return true;
    }();
    (void)logged;
    auto* r = new Reader;
    r->vtbl = kReaderVtbl;
    r->destroy = &destroy_as<Reader>;
    *out = r;
    return kOk;
}

}  // namespace

bool trace_video() {
    static const bool on = [] {
        const std::filesystem::path& ini = joan::platform::settings_file();
        return !joan::kReleaseBuild && joan::platform::ini::get_int("Video", "Trace", 0, ini) != 0;
    }();
    return on;
}

void trace_line(const char* fmt, ...) {
    static FILE* out = [] {
        return std::fopen((joan::platform::exe_dir() / "openjoan_video_ffmpeg.log").string().c_str(), "w");
    }();
    if (!out) return;
    va_list ap;
    va_start(ap, fmt);
    std::vfprintf(out, fmt, ap);
    va_end(ap);
    std::fputc('\n', out);
    std::fflush(out);
}

std::uintptr_t create_sync_reader_address() { return reinterpret_cast<std::uintptr_t>(&create_sync_reader); }

}  // namespace joan::media
