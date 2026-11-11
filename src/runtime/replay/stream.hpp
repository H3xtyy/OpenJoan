// Record / replay (docs/RUNTIME.md "Replay-identical"): the recording file format, its writer and reader, and the
// hash / normalisation helpers of the per-frame state hashes. Platform independent (no <windows.h>): the Windows side
// is src/replay/replay.cpp (state, frame hashes, reports), imports.cpp (IAT wrappers) and com.cpp (COM methods).
//
// File layout (little endian, version kFormatVersion; version 1 = the same without the typed image hash):
//   Header (kHeaderSize bytes), then a stream of events, each starting with its type byte:
//   Call     u8 1, u16 id, u32 check, u32 n, n payload bytes   one intercepted call (id: replay/ids.hpp), its results
//   WndProc  u8 2, u32 msg, u32 wparam, u32 lparam            a window message the game's WndProc received
//   Apc      u8 3, u32 seq, u32 error, u32 bytes               a ReadFileEx completion routine ran (seq = its request)
//   Frame    u8 4, u32 frame, u64 image, u64 heap, u64 back_buffer, u32 pages, pages * u32 page hashes
//   Snapshot u8 5, u32 frame, u32 words, words * u32           the normalised .data / .bss image of that frame
//   Env      u8 6, u32 n, n bytes (text)                       environment fingerprint (save folder, files opened)
//   End      u8 7, u32 frames                                  the game ended normally (ExitProcess)
// Env events also carry the recording's hash settings (replay.cpp): "hash-skip\n" ranges, "hash-types\n" the typed
// table (typed_hash.hpp, version 2 with kFlagTypedHash), "mask\n" / "blank\n" / "raw\n" / "layout\n" for the
// heuristic pointer positions, "file\n" the save files.
// Events appear in the order they happened on the game thread. A call's event is written when the call returns, so
// window messages / completions delivered inside it come first; replay injects them at the same place.
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <string>
#include <vector>

namespace joan::replay {

/// Magic bytes at the start of a recording file ("JOANRPL1").
inline constexpr char kMagic[8] = {'J', 'O', 'A', 'N', 'R', 'P', 'L', '1'};
/// 2: the image hash may use the typed scheme (kFlagTypedHash). Version 1 recordings (heuristic image hash only) are
/// still read; a typed recording is written as version 2 so that older builds refuse it instead of mis-hashing it.
inline constexpr std::uint32_t kFormatVersion = 2;
/// Format version of a recording without the typed image hash (see kFormatVersion).
inline constexpr std::uint32_t kFormatVersionUntyped = 1;
/// Size of the file header in bytes (64).
inline constexpr std::uint32_t kHeaderSize = 64;

// Header flags.
inline constexpr std::uint32_t kFlagBackBuffer = 1;  ///< Frame events carry a back buffer hash
inline constexpr std::uint32_t kFlagHeap = 2;  ///< Frame events carry a CRT heap hash
inline constexpr std::uint32_t kFlagUninitFix = 4;  ///< [Fixes] UninitReads=1 was on while recording
inline constexpr std::uint32_t kFlagTypedHash = 8;  ///< typed image hash (typed_hash.hpp; table in Env "hash-types")
inline constexpr std::uint32_t kFlagTriggerSortFix = 16;  ///< [Fixes] TriggerSort=1 was on while recording (a replay applies it)
inline constexpr std::uint32_t kFlagUnitStuckFix = 32;  ///< [Fixes] UnitStuck=1 was on while recording (a replay applies it)
inline constexpr std::uint32_t kFlagUnitStuckWatch = 64;  ///< ... including its progress check (part 2; recordings made before it
                                                          // have 32 without 64 and replay with part 1 only)
inline constexpr std::uint32_t kFlagNoPlnCache = 128;  ///< the ground cache (map\<cell>.PLN, read when newer than the .GRI and
                                                          // written after a build) was off: it is game-made disk state outside
                                                          // the recording, so a cache present for one run and not the other
                                                          // changed which files a cell load touches
inline constexpr std::uint32_t kFlagImmovableSiege = 512;  ///< [Fixes] ImmovableSiege=1 was on while recording (a replay applies
                                                           // it: the voice it picks is a different sound load)
inline constexpr std::uint32_t kFlagAnimalPen = 1024;  ///< [Fixes] AnimalPen=1 was on while recording (a replay applies it:
                                                       // penned animals get no wander routes with it)
inline constexpr std::uint32_t kFlagMissionCount = 2048;  ///< [Fixes] MissionCount=1 was on while recording (a replay applies it:
                                                          // it looks for the scenario files when a mission ends)
/// Bits 12..16 of the flags: 0 = [Fixes] MouseOptions was off (every recording made before it), else the cursor slider's
/// position + 1 at the start of the recording (a replay applies both).
inline constexpr std::uint32_t kFlagEndingLayout = 1u << 18;  ///< made with the ending drawn as a 1024x768 screen in the mission's frame too
inline constexpr std::uint32_t kFlagWheelZoom = 1u << 19;  ///< made with a wheel notch zooming the same at any frame rate
inline constexpr std::uint32_t kFlagPopupButtons = 1u << 17;  ///< made with a message box's buttons centred on the real screen (a replay of an older recording keeps the old position)
inline constexpr std::uint32_t kFlagNoUiAnimations = 1u << 24;  ///< made with [Display] UiAnimations=0 (a replay applies it)
inline constexpr int kCursorSensitivityShift = 12;  ///< first bit of the cursor sensitivity field in the flags
inline constexpr std::uint32_t kCursorSensitivityMask = 0x1f;  ///< width of that field, after shifting
inline constexpr std::uint32_t kFlagStableGroundOrder = 256;  ///< Cell::build_ground grouped its tiles by the owner sector's index
                                                              // in the cell, not by the sector's address (the address order is
                                                              // the heap's, and the x64 heap lays the sectors out differently)

/// The recording's file header (kHeaderSize bytes on disk).
struct Header {
    std::uint32_t version = kFormatVersion;  ///< file format version (kFormatVersion or kFormatVersionUntyped)
    std::uint32_t flags = 0;                 ///< kFlag* bits
    std::uint32_t data_va = 0;     ///< original address of the hashed range (.data + .bss)
    std::uint32_t data_size = 0;   ///< bytes
    std::uint32_t page_size = 0;   ///< granularity of the page hashes
    std::uint32_t exe_stamp = 0;   ///< openjoan.exe's PE timestamp (information only: replays may use another build)
    std::uint32_t snapshot_every = 0;  ///< a Snapshot event every this many frames (0: TODO(docs): confirm "none")
    std::uint32_t reserved = 0;        ///< unused
};

/// Event types of a recording; the value is the type byte that starts the event (layout in the file comment above).
enum class Ev : std::uint8_t { Call = 1, WndProc = 2, Apc = 3, Frame = 4, Snapshot = 5, Env = 6, End = 7 };
/// Name of an event type for reports.
const char* event_name(Ev e);

/// The state hashes of one frame (a Frame event).
struct FrameHash {
    std::uint32_t frame = 0;               ///< frame number
    std::uint64_t image = 0;               ///< hash of the data image (.data + .bss)
    std::uint64_t heap = 0;                ///< hash of the CRT heap (kFlagHeap)
    std::uint64_t back_buffer = 0;         ///< hash of the back buffer (kFlagBackBuffer)
    std::vector<std::uint32_t> pages;      ///< one folded hash per page_size bytes of the image
};

// ---------------------------------------------------------------- hashing

/// 64-bit FNV-1a over 32-bit words (fast enough for ~0.5 MB per frame; not cryptographic).
inline std::uint64_t hash_words(const std::uint32_t* w, std::size_t n, std::uint64_t h = 0xcbf29ce484222325ull) {
    for (std::size_t i = 0; i < n; ++i) {
        h ^= w[i];
        h *= 0x100000001b3ull;
    }
    return h;
}
/// FNV-1a (64 bit) of `n` bytes at `p`, continuing from `h` (the default is the FNV offset basis).
inline std::uint64_t hash_bytes(const void* p, std::size_t n, std::uint64_t h = 0xcbf29ce484222325ull) {
    const auto* b = static_cast<const std::uint8_t*>(p);
    for (std::size_t i = 0; i < n; ++i) {
        h ^= b[i];
        h *= 0x100000001b3ull;
    }
    return h;
}
/// The 64-bit hash `h` folded to 32 bits (low half xor high half).
inline std::uint32_t fold(std::uint64_t h) { return static_cast<std::uint32_t>(h ^ (h >> 32)); }
/// Check value of call arguments (compared on replay: a different argument is a divergence).
inline std::uint32_t check_of(std::initializer_list<std::uint32_t> args) {
    std::uint64_t h = 0xcbf29ce484222325ull;
    for (std::uint32_t a : args) {
        h ^= a;
        h *= 0x100000001b3ull;
    }
    return fold(h);
}
/// Check value of a string argument (compared on replay), ignoring case and treating '/' as '\\'; `extra` is mixed in.
inline std::uint32_t check_str(const char* s, std::uint32_t extra = 0) {
    std::uint64_t h = 0xcbf29ce484222325ull ^ extra;
    if (s)
        for (; *s; ++s) {
            char c = *s;
            if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');  // paths: case does not matter
            if (c == '/') c = '\\';
            h ^= static_cast<std::uint8_t>(c);
            h *= 0x100000001b3ull;
        }
    return fold(h);
}

/// Address ranges (original image addresses) left out of the image hash: their words count as zero.
struct Range {
    std::uint32_t begin /**< first original address */, end /**< one past the last */;
};
/// "0x00a485a8+0x60, 0x00a6b704" -> ranges (default length 4). Returns false (and the bad token) on a syntax error.
bool parse_ranges(const std::string& text, std::vector<Range>& out, std::string* bad = nullptr);

// ---------------------------------------------------------------- payload builder / reader

/// A call payload under construction: little-endian values appended in order (read back with Cursor).
class Bytes {
public:
    std::vector<std::uint8_t> data;  ///< the bytes so far
    /// Appends the object representation of `v` (trivially copyable); returns *this.
    template <class T>
    Bytes& put(const T& v) {
        const auto* p = reinterpret_cast<const std::uint8_t*>(&v);
        data.insert(data.end(), p, p + sizeof(T));
        return *this;
    }
    /// Appends `n` bytes from `p` (nothing when `n` is 0); returns *this.
    Bytes& put_raw(const void* p, std::size_t n) {
        const auto* b = static_cast<const std::uint8_t*>(p);
        if (n) data.insert(data.end(), b, b + n);
        return *this;
    }
    /// u32 length + bytes (p may be null when n == 0).
    Bytes& put_blob(const void* p, std::uint32_t n) {
        put(n);
        return put_raw(p, n);
    }
};

/// A bounds-checked reader over a payload: reading past the end sets ok() to false and yields zeros.
class Cursor {
public:
    /// Reads `n` bytes at `p` (not owned; must outlive the cursor).
    Cursor(const std::uint8_t* p, std::size_t n) : p_(p), end_(p + n) {}
    /// Reads the bytes of `v` (not owned).
    explicit Cursor(const std::vector<std::uint8_t>& v) : p_(v.data()), end_(v.data() + v.size()) {}
    /// Reads one `T` (zero-initialised when the data ran out).
    template <class T>
    T get() {
        T v{};
        get_raw(&v, sizeof(T));
        return v;
    }
    /// Copies `n` bytes to `out`; false (and ok() becomes false) when fewer are left.
    bool get_raw(void* out, std::size_t n);
    /// u32 length + bytes; copies at most cap bytes into out, returns the stored length.
    std::uint32_t get_blob(void* out, std::uint32_t cap);
    /// False once a read ran past the end.
    bool ok() const { return ok_; }
    /// Bytes not read yet.
    std::size_t left() const { return static_cast<std::size_t>(end_ - p_); }

private:
    const std::uint8_t* p_;
    const std::uint8_t* end_;
    bool ok_ = true;
};

// ---------------------------------------------------------------- file

/// Writes a recording file: the header, then events in the order they are reported. Not thread-safe.
class Writer {
public:
    /// Creates `path` and writes `h`; false when the file cannot be created.
    bool open(const std::string& path, const Header& h);
    /// The file is open.
    bool is_open() const { return f_ != nullptr; }
    /// Call event: intercepted call `id` (replay/ids.hpp), argument check value `check`, `n` bytes of results at `payload`.
    void call(std::uint16_t id, std::uint32_t check, const void* payload, std::uint32_t n);
    /// WndProc event: a window message the game's WndProc received.
    void wndproc(std::uint32_t msg, std::uint32_t wparam, std::uint32_t lparam);
    /// Apc event: a ReadFileEx completion routine ran (`seq` = its request).
    void apc(std::uint32_t seq, std::uint32_t error, std::uint32_t bytes);
    /// Frame event: the state hashes of one frame.
    void frame(const FrameHash& f);
    /// Snapshot event: the normalised .data / .bss image (`words`) of `frame`.
    void snapshot(std::uint32_t frame, const std::vector<std::uint32_t>& words);
    /// Env event: environment fingerprint and hash settings as text.
    void env(const std::string& text);
    /// End event: the game ended normally after `frames` frames.
    void end(std::uint32_t frames);
    /// Flushes the file to the OS.
    void flush();
    /// Closes the file (no more events).
    void close();
    /// Bytes written to the file so far.
    std::uint64_t bytes_written() const { return bytes_; }

private:
    void put(const void* p, std::size_t n);
    template <class T>
    void put(const T& v) {
        put(&v, sizeof(T));
    }
    std::FILE* f_ = nullptr;
    std::uint64_t bytes_ = 0;
};

/// One decoded event (views into the loaded file).
struct Event {
    Ev type{};               ///< which kind of event
    std::size_t offset = 0;  ///< file offset of the type byte (for reports)
    // Call
    std::uint16_t id = 0;     ///< Call: the intercepted call's id (replay/ids.hpp)
    std::uint32_t check = 0;  ///< Call: check value of the arguments
    const std::uint8_t* payload = nullptr;  ///< Call: the results (inside the loaded file)
    std::uint32_t size = 0;                 ///< Call: payload bytes
    // WndProc: a = msg, b = wparam, c = lparam; Apc: a = seq, b = error, c = bytes; End: a = frames
    std::uint32_t a = 0 /**< first value (see above) */, b = 0 /**< second value */, c = 0 /**< third value */;
    // Frame / Snapshot
    FrameHash frame;                  ///< Frame: the hashes; Snapshot: only `frame.frame` is set
    std::vector<std::uint32_t> words; ///< Snapshot: the image words
};

/// Reads a recording: loads the file, then yields its events in order.
class Reader {
public:
    /// Loads the whole file. Returns an error text, or nullptr.
    const char* load(const std::string& path);
    /// The file header (valid after a successful load()).
    const Header& header() const { return header_; }
    /// Decodes the event at the read position without consuming it. False at the end of the data (or on a damaged
    /// event: then error() is set).
    bool peek(Event& e);
    /// Consumes the event peek() returned.
    void next() { pos_ = next_pos_; }
    /// Read position, as a file offset.
    std::size_t position() const { return pos_; }
    /// Size of the loaded file in bytes.
    std::size_t size() const { return data_.size(); }
    /// Text of the last decoding error, or nullptr.
    const char* error() const { return error_; }

private:
    std::vector<std::uint8_t> data_;
    Header header_;
    std::size_t pos_ = 0;
    std::size_t next_pos_ = 0;
    const char* error_ = nullptr;
};

}  // namespace joan::replay
