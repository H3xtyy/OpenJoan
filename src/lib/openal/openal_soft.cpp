// OpenAL Soft for the game's OpenAL calls (phase C2, docs/RUNTIME.md).
//
// joan.exe imports 32 OpenAL 1.0 functions from OpenAL32.dll (Creative's 1.0 router, shipped in the game folder; it
// plays through DirectSound3D and loads eax.dll). Every one of them is part of the OpenAL 1.1 API that OpenAL Soft
// implements with the same C ABI (cdecl, the same argument and result types), so the loader fills the 32 IAT slots
// with the functions below, which call OpenAL Soft, and OpenAL32.dll is not loaded at all (openjoan_loader.log:
// "imports: OpenAL32.dll: not loaded (all 32 imports replaced)"). The game, ALUT (lib/openal/alut.cpp) and the replay
// layer's OpenAL wrappers (replay/imports.cpp, installed on top of these slots) call them through the slots as before.
// The functions here are the place for the differences between Creative's 1.0 and OpenAL 1.1 (none handled yet).
//
// openjoan.ini [Audio]:
//   Trace=1: every OpenAL call of the game, its arguments and the OpenAL error it caused go to openjoan_audio.log
//           (TraceLines= calls in full, default 20000; after that only calls that fail), plus the device's settings
//           and, at exit, a count of the calls and errors per function and parameter.
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include "../../platform/win32_types.hpp"
#include "../../build_flavor.hpp"

#include <cstdarg>
#include <mutex>
#include <shared_mutex>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#include <map>
#include <set>
#include <string>

#include "../../runtime/loader/import_overrides.hpp"
#include "../../platform/ini.hpp"
#include "../../platform/window.hpp"
#include "../../platform/threads.hpp"

namespace {

// ---------------------------------------------------------------- settings (read before host::start)

struct AudioSettings {
    bool trace = false;
    int trace_lines = 20000;
    int hrtf = 0;  // ALC_FALSE / ALC_TRUE / ALC_DONT_CARE_SOFT
    bool near_centered = true;
};

// Asked by the loader while it resolves joan.exe's imports - before host::start reads the settings, so the ini is
// read here (next to openjoan.exe, like everything else of the loader).
const AudioSettings& settings() {
    static const AudioSettings s = [] {
        AudioSettings r;
        const std::filesystem::path& ini = joan::platform::settings_file();
        char value[32] = {};
        if constexpr (joan::kReleaseBuild) {  // fixed in the player build (build_flavor.hpp): no trace, HRTF off, near sounds centred
            r.hrtf = ALC_FALSE;
            r.near_centered = true;
            return r;
        }
        r.trace = joan::platform::ini::get_int("Audio", "Trace", 0, ini.c_str()) != 0;
        r.trace_lines = static_cast<int>(joan::platform::ini::get_int("Audio", "TraceLines", 20000, ini.c_str()));
        joan::platform::ini::get_string("Audio", "HRTF", "0", value, sizeof(value), ini.c_str());
        r.hrtf = _stricmp(value, "auto") == 0 ? ALC_DONT_CARE_SOFT : (std::atoi(value) != 0 ? ALC_TRUE : ALC_FALSE);
        r.near_centered = joan::platform::ini::get_int("Audio", "NearSoundsCentered", 1, ini.c_str()) != 0;
        return r;
    }();
    return s;
}

// ---------------------------------------------------------------- trace

std::recursive_mutex g_lock;  // (recursive, as the critical section it was)
FILE* g_log = nullptr;
long g_lines = 0;
std::map<std::string, long>* g_counts = nullptr;  // "fn param" -> calls; "fn param ERROR" -> failures

const char* enum_name(ALenum e) {
    switch (e) {
        case 0x202: return "SOURCE_RELATIVE";
        case 0x1001: return "CONE_INNER_ANGLE";
        case 0x1002: return "CONE_OUTER_ANGLE";
        case 0x1003: return "PITCH";
        case 0x1004: return "POSITION";
        case 0x1005: return "DIRECTION";
        case 0x1006: return "VELOCITY";
        case 0x1007: return "LOOPING";
        case 0x1009: return "BUFFER";
        case 0x100A: return "GAIN";
        case 0x100D: return "MIN_GAIN";
        case 0x100E: return "MAX_GAIN";
        case 0x100F: return "ORIENTATION";
        case 0x1010: return "SOURCE_STATE";
        case 0x1011: return "INITIAL";
        case 0x1012: return "PLAYING";
        case 0x1013: return "PAUSED";
        case 0x1014: return "STOPPED";
        case 0x1015: return "BUFFERS_QUEUED";
        case 0x1016: return "BUFFERS_PROCESSED";
        case 0x1020: return "REFERENCE_DISTANCE";
        case 0x1021: return "ROLLOFF_FACTOR";
        case 0x1022: return "CONE_OUTER_GAIN";
        case 0x1023: return "MAX_DISTANCE";
        case 0x1100: return "MONO8";
        case 0x1101: return "MONO16";
        case 0x1102: return "STEREO8";
        case 0x1103: return "STEREO16";
        case 0xA001: return "INVALID_NAME";
        case 0xA002: return "INVALID_ENUM";
        case 0xA003: return "INVALID_VALUE";
        case 0xA004: return "INVALID_OPERATION";
        case 0xA005: return "OUT_OF_MEMORY";
        case 0xB001: return "VENDOR";
        case 0xB002: return "VERSION";
        case 0xB003: return "RENDERER";
        case 0xB004: return "EXTENSIONS";
        case 0xD001: return "INVERSE_DISTANCE";
        case 0xD002: return "INVERSE_DISTANCE_CLAMPED";
        case 0xD003: return "LINEAR_DISTANCE";
        case 0: return "NONE";
        default: return nullptr;
    }
}
std::string en(ALenum e) {
    if (const char* n = enum_name(e)) return n;
    char b[16];
    std::snprintf(b, sizeof b, "0x%x", static_cast<unsigned>(e));
    return b;
}

void write_line(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    std::vfprintf(g_log, fmt, ap);
    va_end(ap);
    std::fputc('\n', g_log);
}

void summary() {
    if (!g_log) return;
    g_lock.lock();
    write_line("---- %ld OpenAL calls; per function / parameter (ERROR lines: calls that failed):", g_lines);
    for (const auto& [k, n] : *g_counts) write_line("%-48s %ld", k.c_str(), n);
    std::fflush(g_log);
    g_lock.unlock();
}

bool tracing() {
    static const bool on = [] {
        if (!settings().trace) return false;
        g_log = std::fopen((joan::platform::exe_dir() / "openjoan_audio.log").string().c_str(), "w");
        if (!g_log) return false;
        g_counts = new std::map<std::string, long>;
        write_line("OpenJoan OpenAL trace (OpenAL Soft)");
        return true;
    }();
    return on;
}

// The error a traced call left: taken with alGetError right after the call (the game's own alGetError then gets
// the first error since its previous alGetError, as OpenAL would have answered).
ALenum g_pending = AL_NO_ERROR;

/// Logs one call: `key` = function (+ parameter) for the counts, `text` = the call with its arguments.
void trace(const std::string& key, const std::string& text, bool check_error = true) {
    if (!tracing()) return;
    ALenum err = AL_NO_ERROR;
    if (check_error) {
        err = alGetError();
        if (g_pending == AL_NO_ERROR) g_pending = err;
    }
    g_lock.lock();
    const long n = ++g_lines;
    ++(*g_counts)[key];
    if (err != AL_NO_ERROR) ++(*g_counts)[key + " ERROR " + en(err)];
    if (n <= settings().trace_lines || err != AL_NO_ERROR)
        write_line("#%ld t%lu %s%s%s", n, joan::platform::thread_id(), text.c_str(), err != AL_NO_ERROR ? "  -> ERROR " : "",
                   err != AL_NO_ERROR ? en(err).c_str() : "");
    if (n == settings().trace_lines) write_line("(TraceLines reached: from here on only calls that fail)");
    g_lock.unlock();
}

std::string fmt(const char* f, ...) {
    char b[512];
    va_list ap;
    va_start(ap, f);
    std::vsnprintf(b, sizeof b, f, ap);
    va_end(ap);
    return b;
}

// ---------------------------------------------------------------- streaming: OpenAL 1.0 queue semantics
//
// The game streams music (and movie sound) as a queue of short buffers on one source (Sound3D streaming): it starts
// the source with a first small buffer, queues more, and every frame unqueues and deletes AL_BUFFERS_PROCESSED
// buffers, then plays the source again if it has STOPPED. With Creative's 1.0 (DirectSound3D, ~100 ms latency) the
// first buffer was still playing when the next ones were queued. OpenAL Soft plays the 4 ms start buffer at once,
// the source STOPS, and OpenAL 1.1 reports every buffer of a stopped source as processed - also the ones queued
// after it stopped, which never played. The game then deletes each new buffer before it is heard: no music.
// 1.0 behaviour kept here: buffers queued on a STOPPED source count as unplayed until the next alSourcePlay (the
// source restarts from its queue head there; buffers that did play are unqueued by the game first).
std::shared_mutex g_stream_lock;
std::map<ALuint, ALint>* g_unplayed = nullptr;  // source -> buffers queued while it was stopped

ALint source_int(ALuint s, ALenum p) {
    ALint v = 0;
    alGetSourcei(s, p, &v);
    return v;
}
void set_unplayed(ALuint s, ALint n) {
    g_stream_lock.lock();
    if (!g_unplayed) g_unplayed = new std::map<ALuint, ALint>;
    if (n > 0)
        (*g_unplayed)[s] = n;
    else
        g_unplayed->erase(s);
    g_stream_lock.unlock();
}
ALint unplayed(ALuint s) {
    g_stream_lock.lock_shared();
    ALint n = 0;
    if (g_unplayed) {
        const auto it = g_unplayed->find(s);
        if (it != g_unplayed->end()) n = it->second;
    }
    g_stream_lock.unlock_shared();
    return n;
}

// ---------------------------------------------------------------- 3D: what Creative's 1.0 did differently
//
// - Near sounds: in the original the hero's own sounds stay in the middle (the user's comparison; not yet confirmed in
//   Creative's OpenAL32.dll code, whose own mixer checks the distance model before it computes a pan angle), while
//   OpenAL Soft pans each sound by its exact direction. The game keeps the
//   listener about 50 units above and behind the hero and gives every sound a reference distance of 1000 (or
//   10000), so the hero's own steps and blows were heard from the middle, but with OpenAL Soft they swing left and
//   right as the camera turns. Here each source gets an OpenAL Soft source radius (AL_EXT_SOURCE_RADIUS) equal to
//   its reference distance: inside it the sound spreads around the listener (no direction at the centre), outside
//   it becomes a point source again. [Audio] NearSoundsCentered=0 turns this off.
// - The in-game "3D sound" option off sets alDistanceModel(AL_NONE). For Creative that meant no 3D processing; in
//   OpenAL 1.1 it only drops the distance attenuation and keeps the panning. Here AL_NONE also switches every source
//   to non-spatialized (AL_SOURCE_SPATIALIZE_SOFT = AL_FALSE: centred, as the original played them).
std::set<ALuint>* g_sources = nullptr;  // live sources; main thread only
bool g_spatial = true;

void apply_spatial(ALuint s) { alSourcei(s, AL_SOURCE_SPATIALIZE_SOFT, g_spatial ? AL_AUTO_SOFT : AL_FALSE); }

// ---------------------------------------------------------------- the 32 imports

void AL_APIENTRY j_alSourceRewind(ALuint s) {
    alSourceRewind(s);
    set_unplayed(s, 0);
    if (tracing()) trace("alSourceRewind", fmt("alSourceRewind(%u)", s));
}
// Unqueueing from a looping source: OpenAL Soft refuses it (AL_INVALID_VALUE) even when the source is stopped, where
// Creative's 1.0 let it through. The game stops a looping effect and unqueues its buffer to free the source
// (Sound3D list cleanup); refused, the buffer stayed queued and the source counted as busy for the rest of the run
// (fewer free sources for voices). For a stopped / initial source the looping flag is lifted around the call.
void AL_APIENTRY j_alSourceUnqueueBuffers(ALuint s, ALsizei n, ALuint* b) {
    bool relooped = false;
    if (source_int(s, AL_LOOPING) != 0) {
        const ALint state = source_int(s, AL_SOURCE_STATE);
        if (state == AL_STOPPED || state == AL_INITIAL) {
            alSourcei(s, AL_LOOPING, AL_FALSE);
            relooped = true;
        }
    }
    alSourceUnqueueBuffers(s, n, b);
    if (relooped) alSourcei(s, AL_LOOPING, AL_TRUE);
    if (const ALint u = unplayed(s); u > 0) set_unplayed(s, std::min(u, source_int(s, AL_BUFFERS_QUEUED)));
    if (tracing()) trace("alSourceUnqueueBuffers", fmt("alSourceUnqueueBuffers(%u, %d) -> %u", s, n, n > 0 && b ? b[0] : 0u));
}
void AL_APIENTRY j_alSourcePause(ALuint s) {
    alSourcePause(s);
    if (tracing()) trace("alSourcePause", fmt("alSourcePause(%u)", s));
}
void AL_APIENTRY j_alSourceQueueBuffers(ALuint s, ALsizei n, const ALuint* b) {
    const bool stopped = source_int(s, AL_SOURCE_STATE) == AL_STOPPED;
    const ALint before = stopped ? source_int(s, AL_BUFFERS_QUEUED) : 0;
    alSourceQueueBuffers(s, n, b);
    if (stopped) {
        const ALint added = source_int(s, AL_BUFFERS_QUEUED) - before;
        if (added > 0) set_unplayed(s, unplayed(s) + added);
    }
    if (tracing()) trace("alSourceQueueBuffers", fmt("alSourceQueueBuffers(%u, %d, %u)", s, n, n > 0 && b ? b[0] : 0u));
}
void AL_APIENTRY j_alSourcePlay(ALuint s) {
    alSourcePlay(s);
    set_unplayed(s, 0);
    if (tracing()) trace("alSourcePlay", fmt("alSourcePlay(%u)", s));
}
void AL_APIENTRY j_alSourcef(ALuint s, ALenum p, ALfloat v) {
    alSourcef(s, p, v);
    if (p == AL_REFERENCE_DISTANCE && settings().near_centered && v > 0.0f) alSourcef(s, AL_SOURCE_RADIUS, v);
    if (tracing()) trace("alSourcef " + en(p), fmt("alSourcef(%u, %s, %g)", s, en(p).c_str(), static_cast<double>(v)));
}
void AL_APIENTRY j_alSourcei(ALuint s, ALenum p, ALint v) {
    alSourcei(s, p, v);
    if (p == AL_BUFFER) set_unplayed(s, 0);
    if (tracing()) trace("alSourcei " + en(p), fmt("alSourcei(%u, %s, %d)", s, en(p).c_str(), v));
}
void AL_APIENTRY j_alDistanceModel(ALenum m) {
    alDistanceModel(m);
    g_spatial = m != AL_NONE;
    if (g_sources)
        for (const ALuint src : *g_sources) apply_spatial(src);
    if (tracing()) trace("alDistanceModel " + en(m), fmt("alDistanceModel(%s)", en(m).c_str()));
}
void AL_APIENTRY j_alGetSourcei(ALuint s, ALenum p, ALint* v) {
    alGetSourcei(s, p, v);
    if (p == AL_BUFFERS_PROCESSED && v) {
        if (const ALint u = unplayed(s); u > 0) {
            if (source_int(s, AL_SOURCE_STATE) == AL_STOPPED)
                *v = std::max(0, *v - u);
            else
                set_unplayed(s, 0);
        }
    }
    const ALint r = v ? *v : 0;
    if (tracing()) trace("alGetSourcei " + en(p), fmt("alGetSourcei(%u, %s) -> %s", s, en(p).c_str(),
                                         p == AL_SOURCE_STATE ? en(r).c_str() : std::to_string(r).c_str()));
}
void AL_APIENTRY j_alListenerf(ALenum p, ALfloat v) {
    alListenerf(p, v);
    if (tracing()) trace("alListenerf " + en(p), fmt("alListenerf(%s, %g)", en(p).c_str(), static_cast<double>(v)));
}
void AL_APIENTRY j_alListener3f(ALenum p, ALfloat a, ALfloat b, ALfloat c) {
    alListener3f(p, a, b, c);
    if (tracing()) trace("alListener3f " + en(p), fmt("alListener3f(%s, %g, %g, %g)", en(p).c_str(), static_cast<double>(a),
                                         static_cast<double>(b), static_cast<double>(c)));
}
void AL_APIENTRY j_alListenerfv(ALenum p, const ALfloat* v) {
    alListenerfv(p, v);
    if (v && p == AL_ORIENTATION)
        trace("alListenerfv " + en(p), fmt("alListenerfv(%s, at %g %g %g, up %g %g %g)", en(p).c_str(),
                                             static_cast<double>(v[0]), static_cast<double>(v[1]), static_cast<double>(v[2]),
                                             static_cast<double>(v[3]), static_cast<double>(v[4]), static_cast<double>(v[5])));
    else
        trace("alListenerfv " + en(p), fmt("alListenerfv(%s)", en(p).c_str()));
}
const ALchar* AL_APIENTRY j_alGetString(ALenum p) {
    const ALchar* r = alGetString(p);
    if (tracing()) trace("alGetString " + en(p), fmt("alGetString(%s) -> %s", en(p).c_str(), r ? r : "(null)"));
    return r;
}
ALenum AL_APIENTRY j_alGetError() {
    ALenum r = alGetError();
    if (tracing()) {
        if (r == AL_NO_ERROR) r = g_pending;
        g_pending = AL_NO_ERROR;
        trace("alGetError", fmt("alGetError() -> %s", en(r).c_str()), false);
    }
    return r;
}
void AL_APIENTRY j_alBufferData(ALuint b, ALenum f, const ALvoid* d, ALsizei n, ALsizei rate) {
    alBufferData(b, f, d, n, rate);
    if (tracing()) trace("alBufferData " + en(f) + " " + std::to_string(rate),
          fmt("alBufferData(%u, %s, %d bytes, %d Hz)", b, en(f).c_str(), n, rate));
}
void AL_APIENTRY j_alDopplerFactor(ALfloat v) {
    alDopplerFactor(v);
    if (tracing()) trace("alDopplerFactor", fmt("alDopplerFactor(%g)", static_cast<double>(v)));
}
void AL_APIENTRY j_alDopplerVelocity(ALfloat v) {
    alDopplerVelocity(v);
    if (tracing()) trace("alDopplerVelocity", fmt("alDopplerVelocity(%g)", static_cast<double>(v)));
}
ALboolean AL_APIENTRY j_alIsBuffer(ALuint b) {
    const ALboolean r = alIsBuffer(b);
    if (tracing()) trace("alIsBuffer", fmt("alIsBuffer(%u) -> %d", b, r));
    return r;
}
void AL_APIENTRY j_alGenBuffers(ALsizei n, ALuint* b) {
    alGenBuffers(n, b);
    if (tracing()) trace("alGenBuffers", fmt("alGenBuffers(%d) -> %u", n, n > 0 && b ? b[0] : 0u));
}
void AL_APIENTRY j_alDeleteBuffers(ALsizei n, const ALuint* b) {
    alDeleteBuffers(n, b);
    if (tracing()) trace("alDeleteBuffers", fmt("alDeleteBuffers(%d, %u)", n, n > 0 && b ? b[0] : 0u));
}
ALboolean AL_APIENTRY j_alIsSource(ALuint s) {
    const ALboolean r = alIsSource(s);
    if (tracing()) trace("alIsSource", fmt("alIsSource(%u) -> %d", s, r));
    return r;
}
void AL_APIENTRY j_alGenSources(ALsizei n, ALuint* s) {
    alGenSources(n, s);
    for (ALsizei i = 0; s && i < n; ++i) {
        if (!g_sources) g_sources = new std::set<ALuint>;
        g_sources->insert(s[i]);
        if (!g_spatial) apply_spatial(s[i]);
    }
    if (tracing()) trace("alGenSources", fmt("alGenSources(%d) -> %u", n, n > 0 && s ? s[0] : 0u));
}
void AL_APIENTRY j_alDeleteSources(ALsizei n, const ALuint* s) {
    alDeleteSources(n, s);
    for (ALsizei i = 0; s && i < n; ++i) {
        set_unplayed(s[i], 0);
        if (g_sources) g_sources->erase(s[i]);
    }
    if (tracing()) trace("alDeleteSources", fmt("alDeleteSources(%d, %u)", n, n > 0 && s ? s[0] : 0u));
}
void AL_APIENTRY j_alSourceStop(ALuint s) {
    alSourceStop(s);
    set_unplayed(s, 0);
    if (tracing()) trace("alSourceStop", fmt("alSourceStop(%u)", s));
}
void AL_APIENTRY j_alSource3f(ALuint s, ALenum p, ALfloat a, ALfloat b, ALfloat c) {
    alSource3f(s, p, a, b, c);
    if (tracing()) trace("alSource3f " + en(p), fmt("alSource3f(%u, %s, %g, %g, %g)", s, en(p).c_str(), static_cast<double>(a),
                                       static_cast<double>(b), static_cast<double>(c)));
}

// ALC: no AL error state to read (and before a context exists alGetError has none).
ALCboolean ALC_APIENTRY j_alcMakeContextCurrent(ALCcontext* c) {
    const ALCboolean r = alcMakeContextCurrent(c);
    if (tracing()) trace("alcMakeContextCurrent", fmt("alcMakeContextCurrent(%p) -> %d", static_cast<void*>(c), r), false);
    return r;
}
// [Audio] HRTF: OpenAL Soft switches its HRTF (binaural 3D for headphones) on by itself when Windows reports
// headphones. Creative's 1.0 panned like DirectSound3D (no HRTF), so the default here is off (HRTF=0); HRTF=1 always
// on, HRTF=auto leaves it to OpenAL Soft. The game passes no attributes (alutInit).
ALCcontext* ALC_APIENTRY j_alcCreateContext(ALCdevice* d, const ALCint* attrs) {
    const ALCint ours[] = {ALC_HRTF_SOFT, settings().hrtf, 0};
    ALCcontext* const r = alcCreateContext(d, attrs ? attrs : ours);
    if (tracing()) {
        ALCint freq = 0, mono = 0, stereo = 0, hrtf = 0;
        if (d) {
            alcGetIntegerv(d, ALC_HRTF_SOFT, 1, &hrtf);
            alcGetIntegerv(d, ALC_FREQUENCY, 1, &freq);
            alcGetIntegerv(d, ALC_MONO_SOURCES, 1, &mono);
            alcGetIntegerv(d, ALC_STEREO_SOURCES, 1, &stereo);
        }
        trace("alcCreateContext", fmt("alcCreateContext(%p, attrs %s) -> %p; device: %s, %d Hz, %d mono + %d stereo sources, HRTF %s",
                                      static_cast<void*>(d), attrs ? "given" : "none", static_cast<void*>(r),
                                      d ? alcGetString(d, ALC_DEVICE_SPECIFIER) : "-", freq, mono, stereo, hrtf ? "on" : "off"),
              false);
    }
    return r;
}
ALCdevice* ALC_APIENTRY j_alcOpenDevice(const ALCchar* name) {
#ifndef _WIN32
    // alutInit asks for "DirectSound3D". OpenAL Soft takes Windows' old fixed device names (DirectSound3D,
    // DirectSound, MMSYSTEM) for the default device on Windows only; elsewhere it looks for a device of that name and
    // finds none (no sound: the game's empty "continue without sound?" question). The default device here too.
    if (name && (std::strcmp(name, "DirectSound3D") == 0 || std::strcmp(name, "DirectSound") == 0 ||
                 std::strcmp(name, "MMSYSTEM") == 0))
        name = nullptr;
#endif
    ALCdevice* const r = alcOpenDevice(name);
    if (tracing()) trace("alcOpenDevice", fmt("alcOpenDevice(\"%s\") -> %p", name ? name : "(default)", static_cast<void*>(r)), false);
    return r;
}
ALCboolean ALC_APIENTRY j_alcCloseDevice(ALCdevice* d) {
    const ALCboolean r = alcCloseDevice(d);
    if (tracing()) trace("alcCloseDevice", fmt("alcCloseDevice(%p) -> %d", static_cast<void*>(d), r), false);
    return r;
}
void ALC_APIENTRY j_alcDestroyContext(ALCcontext* c) {
    alcDestroyContext(c);
    if (tracing()) trace("alcDestroyContext", fmt("alcDestroyContext(%p)", static_cast<void*>(c)), false);
}
ALCdevice* ALC_APIENTRY j_alcGetContextsDevice(ALCcontext* c) {
    ALCdevice* const r = alcGetContextsDevice(c);
    if (tracing()) trace("alcGetContextsDevice", fmt("alcGetContextsDevice(%p) -> %p", static_cast<void*>(c), static_cast<void*>(r)), false);
    return r;
}
ALCcontext* ALC_APIENTRY j_alcGetCurrentContext() {
    ALCcontext* const r = alcGetCurrentContext();
    if (tracing()) trace("alcGetCurrentContext", fmt("alcGetCurrentContext() -> %p", static_cast<void*>(r)), false);
    return r;
}

}  // namespace

namespace joan::openal {
/// End-of-run report of [Audio] Trace (called from the loader's ExitProcess: the game's own CRT exit does not run
/// our atexit handlers).
void write_trace_summary() { summary(); }
}  // namespace joan::openal

// The 32 imports of joan.exe (src/import_table.inc, OpenAL32.dll).
#define AL(name) JOAN_IMPORT_OVERRIDE("OpenAL32.dll", #name, &j_##name, "OpenAL Soft " #name)
AL(alSourceRewind);
AL(alSourceUnqueueBuffers);
AL(alSourcePause);
AL(alSourceQueueBuffers);
AL(alSourcePlay);
AL(alSourcef);
AL(alSourcei);
AL(alDistanceModel);
AL(alGetSourcei);
AL(alListenerf);
AL(alListener3f);
AL(alListenerfv);
AL(alGetString);
AL(alGetError);
AL(alBufferData);
AL(alDopplerFactor);
AL(alDopplerVelocity);
AL(alIsBuffer);
AL(alGenBuffers);
AL(alDeleteBuffers);
AL(alIsSource);
AL(alGenSources);
AL(alDeleteSources);
AL(alcMakeContextCurrent);
AL(alcCreateContext);
AL(alcOpenDevice);
AL(alcCloseDevice);
AL(alcDestroyContext);
AL(alcGetContextsDevice);
AL(alcGetCurrentContext);
AL(alSourceStop);
AL(alSource3f);
#undef AL
