// Record / replay: options, the recording, injection of recorded window messages / completions, per-frame state
// hashes and the reports. Overview: replay.hpp. Call wrappers: imports.cpp (IAT), com.cpp (COM methods).
#include "replay.hpp"

#include <algorithm>
#include <bit>
#include <iterator>
#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../compiler.hpp"
#if defined(_WIN32)
#include <windows.h>

#include <dbghelp.h>  // after windows.h (the Windows-only write watch below)
#endif
#include "../../d3d8/d3d8_types.hpp"
#include "../code_slots_core.hpp"
#include "../code_table.hpp"
#include "../det_heap.hpp"
#include "../../platform/ini.hpp"
#include "../../platform/net.hpp"
#include "../../data_names.hpp"
#include "../image.hpp"
#include "../image_runtime.hpp"
#include "../loader/data_image.hpp"
#include "../../registry.hpp"
#include "internal.hpp"
#include "sys.hpp"
#include "typed_hash.hpp"
#include "x86_view.hpp"

namespace joan::replay {

#define JOAN_REPLAY_ID_NAME(name) #name,
const char* id_name(std::uint16_t id) {
    static const char* const kNames[] = {JOAN_REPLAY_IDS(JOAN_REPLAY_ID_NAME)};
    return id < sizeof(kNames) / sizeof(kNames[0]) ? kNames[id] : "?";
}
#undef JOAN_REPLAY_ID_NAME

namespace {

// The hashed state: .data + .bss of joan_image (original 0x009f4000..0x00a6d000; .rdata is constant and read-only,
// .idata holds the IAT, which we change ourselves).
constexpr std::uint32_t kDataVa = 0x009f4000u;  // start of .data
constexpr std::uint32_t kDataEnd = 0x00a6d000;                  // start of .idata
constexpr std::uint32_t kDataSize = kDataEnd - kDataVa;
constexpr std::uint32_t kPageSize = 0x1000;
constexpr std::uint32_t kPages = kDataSize / kPageSize;
static_assert(kDataSize % kPageSize == 0);

// What a normalised word becomes when it is a pointer / handle whose value differs from run to run.
constexpr std::uint32_t kTagPointer = 0xA110C000u;  // into a heap, a DLL, a stack, a COM object ...
constexpr std::uint32_t kTagHandle = 0x4A4E0000u;   // a file / event / find / registry handle the game got
constexpr std::uint32_t kTagWindow = 0x4A570000u;   // the game window

// Ranges left out by default in the heuristic scheme (version 1 recordings, ReplayHashTyped=0; the typed scheme's list
// is kDefaultSkipTyped below): the CpuUsage profiler object (g_timer 0x00a485a8; CpuUsage::init 0x0045c040 stores
// rdtsc stamps and the measured tsc rate in it, Profiler::frame 0x0045cef0 / Obj::forward_e750 0x0045c990 the frame's
// rdtsc stamp at +0x20 = 0x00a485c8). Since patch 136 read_tsc goes through replay::tsc (recorded), so these match
// too; the range stays out of the hash for recordings made before that.
// Also left out: copies of CRT tables our port does not use (lib/crt/x87_math.cpp / lib/crt/eh_support.cpp own their copies), which
// hold code addresses at 2-byte offsets that are written at startup: the exp _trandisp descriptor's pointers
// 0x00a311a2.. and the _adj_fprem jump table 0x00a3121e...
// And the wmvcore.dll module handle 0x00a6a3c4 (LoadLibraryA in the movie player, kept after FreeLibrary).
// And six words set once at start-up that hold (parts of) addresses of blocks the game gets outside its CRT heap
// (VirtualAlloc'd resource pools, file views) at packed / unaligned offsets the classifier cannot pin down
// (test19 / test20: the only words left, constant from frame 2 to the end), and the packed plane block float[29]
// 0x00a525a7 + 0x00a52740 (plane_draw.cpp; where its address bytes sit varies from run to run, test21 / test22).
// The first of those words grew to 0x00a51114+8: the unaligned address at 0x00a51117 has a low byte that differs
// between runs (test25). And the Video object's IWMSyncReader 0x00a5d314 (a heap address that can look like text,
// test25 frame 2; the classifier now also takes text-looking words pointing at an object with a vtable).
// And the pixel-format op table 0x00a15073 (11 unaligned pointers to our naked Pixel::* functions, stored by
// set_pixel_format 0x005706f0): unaligned exe addresses classify by luck of the build layout (test28 on the O6
// build: 0x00a15078 from frame 2 on). Only recordings made from this build on leave it out.
// And the CRT initialiser tables 0x009f4000 (xc_table, 738 code pointers resolved per build by the data relocs;
// test29 on the O12 build: 0x009f4950 = init_650fb0 classified differently from frame 2 on).
// And three unaligned pointers in packed records whose byte offset / classification moves with the build (test30 on
// the R10 build): the command table entry 0x00a018bd, and the 0x00a56c24 / 0x00a58dc4 skips widened by a word.
// And the input-device pointer copied into the key-binding records (0x00a51805, 0x00a518e9 and three unaligned
// copies in each of the five 0x144-byte records of 0x00a51db5): test32 on the R12 build classified it differently
// from frame 2 on (heap and back buffer equal). Only recordings made from this build on leave it out.
// And two text widgets whose font pointer (an address in our image) sits at an odd offset, so a word holds half of
// it (Popup 0x00a52df8 button 2 +0x328, QuickBar 0x00a5d408 item +0x176 +0x90; ReplayWatch on test33, R14 build: the
// half 0x1608 read as a det_heap address). Only recordings made from this build on leave them out.
// And the float 12800.0f (0x46480000) a Message stores at 0x00a55ab4 (Message::init, ReplayWatch on test34): the
// recording's det_heap happened to cover that address, so it was classified as a heap pointer there.
// And three unaligned code pointers (button draw callbacks, fn::code_value) in the save menu 0x00a411f8 and in
// 0x00a4f3ae: test35 on the R19 build normalised the window through this build's layout differently from frame 2 on.
// And the display palette 0x00a4f888 (0x40c bytes: device ref, state, 256 colours): a colour value can fall in this
// build's openjoan.exe range and then normalises as a code address (test36 on the R20 build: +0x1bc read as
// Group::first_ready_member). Only recordings made from this build on leave it out.
// And six more button font pointers at odd offsets (Button create / set_text at frame 2: menu 0x00a3cf6d buttons,
// popup 0x00a50808, 0x00a53ff8 create_buttons, 0x00a59abf init_6f2d70; ReplayWatch on test37, R21 build): the hashed
// word holds half of an address in our image, so it classifies differently whenever the layout moves.
// And the CRT's TLS index g_tlsindex 0x00a30fa0 (TlsAlloc at startup): it depends on how many TLS slots the DLLs
// loaded before it took (OpenAL Soft's soft_oal.dll: 12 instead of 13 with OpenAL32.dll).
// And the CRT's _NLG_Destination 0x00a30930 (debugger notification block: eax / ebp of the last local unwind, written
// by _NLG_Notify), which depends on unwinds outside the game's own code (test53 on the C1 build: +4 / +0xc zero).
// And the sprite-set pointers (BmpRes*, Button +0x93, unaligned) of the five EndScreen 0x00a46ad0 buttons: the hashed
// words hold parts of an address outside the CRT heap (test53 on the C1 build: +0x740 / +0xdcc from frame 2 on).
// Only recordings made from this build on leave them out.
constexpr char kDefaultSkip[] =
    "0x00a485a8+0x60,0x00a311a0+0x20,0x00a3121c+0x104,0x00a6a3c4+4,"
    "0x00a51114+8,0x00a52744+4,0x00a5d314+4,0x00a56c24+4,0x00a58dc4+8,0x00a58efc+4,0x00a525a4+0x7c,0x00a52740+4,"
    "0x00a15070+0x30,0x009f4000+0xb88,0x00a018bc+8,0x00a56c24+8,0x00a58dc4+0x10,"
    "0x00a51805+4,0x00a518e9+4,0x00a51dd4+4,0x00a51e25+4,0x00a51e75+4,0x00a51f18+4,0x00a51f69+4,0x00a51fb9+4,0x00a5205c+4,0x00a520ad+4,0x00a520fd+4,0x00a521a0+4,0x00a521f1+4,0x00a52241+4,0x00a522e4+4,0x00a52335+4,0x00a52385+4,"
    "0x00a538bc+8,0x00a5d60c+8,0x00a55ab4+4,"
    "0x00a43abc+8,0x00a44e60+8,0x00a4f5e0+8,0x00a4f888+0x40c,"
    "0x00a3d934+8,0x00a3e64c+8,0x00a3f9f0+8,0x00a50bf8+8,0x00a549d0+8,0x00a5b828+8,"
    "0x00a30fa0+4,"
    "0x00a30930+0x18,0x00a46b80+8,0x00a46ec8+8,0x00a4720c+8,0x00a47554+8,0x00a47898+8";
// The list of a typed recording (kFlagTypedHash): pointer fields and typed data are exact there, so only these stay
// (re/replay/gen_hash_types.py --classify sorts the list above into pointer / typed / untyped bytes): the CpuUsage
// frame stamp in its untyped gap +0x20 (rdtsc), the pixel-format op table (untyped .data bytes holding unaligned code
// pointers), the untyped byte table after the command table entry, the Message float 12800.0f at 0x00a55ab4 (it
// overlaps MessageList::ptr_2e: a type conflict, see the report), the display palette (untyped colours), the CRT's TLS
// index and _NLG_Destination (they depend on the DLLs / unwinds of the run, not on the game).
// And the untyped word 0x00a56794 of the PANEL sprite set (0x00a563a0+0x3f4; g_ever_mapped above): a heap-range
// value that was unmapped when the frame was hashed, hashed raw by recordings made before 2026-10-29. Only
// recordings made from this build on leave it out.
// And the CRT's program path _pgmname 0x00a6b43c (char[260], lib/crt/startup.cpp: GetModuleFileNameA at start-up):
// the folder and the executable's name are the environment, not game state; hashed raw, a recording replayed only
// from the same folder by an executable of the same name (the Phase 1 rename joan_re -> openjoan: every frame of
// ref-x86-2026-10-29.rec differed in those three words).
// And the CRT _locktable 0x00a30edc (48 CRITICAL_SECTION pointers, lib/crt/mt.cpp: entries allocated on first use are
// heap addresses typed u8[4] / dword tails in the layout; x64 keeps the table outside the image): 0x00a30edc+0xc0.
constexpr char kDefaultSkipTyped[] =
    "0x00a485c8+4,0x00a15070+0x30,0x00a018c0+4,0x00a55ab4+4,0x00a4f888+0x40c,0x00a30fa0+4,0x00a30930+0x18,"
    "0x00a56794+4,0x00a6b43c+0x104,0x00a30edc+0xc0";
constexpr char kSkipTag[] = "hash-skip\n";  // Env event holding the recording's skip list
constexpr char kTypesTag[] = "hash-types\n";  // Env event: the typed table of a kFlagTypedHash recording
constexpr char kFileTag[] = "file\n";       // Env event: "file\n<path below the game folder>\n<contents>"

struct Options {
    std::string record, replay;
    std::string stream;  // --stream PORT: co-op host, the recording (or the replayed one) goes to a player as it is used
    std::string follow;  // --follow HOST:PORT: co-op player, the replay comes from the host as it is made (docs/COOP.md)
    bool back_buffer = false;
    bool heap = false;
    int files = 1;  // ReplayHashFiles: 0 off, 1 quick fingerprint, 2 whole content
    std::uint32_t snapshot_every = 0;
    bool stop_on_divergence = true;
    bool exit_at_end = true;
    bool pace = true;
    bool det_heap = true;  // ReplayDetHeap
    std::uint32_t snapshot_first = 8;  // ReplaySnapshotFirst: a snapshot in each of the first N frames
    bool typed = true;  // ReplayHashTyped: record with the typed image hash (0: the heuristic scheme of version 1)
    std::string skip_text;
    std::string skip_replay_text;  // ReplayHashSkipReplay: ranges this replay leaves out on top of the recording's list (x64 width words)
    std::vector<Range> skip;
    std::vector<std::string> env_dirs;
    bool restore_saves = true;  // replay: put the recording's save folders in place (the current ones are kept aside)
};

struct Recent {
    std::uint32_t frame;
    std::uint64_t index;
    Ev type;
    std::uint16_t id;
};

// ---------------------------------------------------------------- state
std::atomic<Mode> g_mode{Mode::Off};
unsigned long g_thread = 0;
Options g_opt;
Header g_header;
Writer g_writer;
Reader g_reader;

// ---------------------------------------------------------------- co-op stream (docs/COOP.md)

constexpr std::size_t kForwardChunk = 256u << 10;  // a replay passed on: sent at least this often, between frames too
platform::net::Connection g_peer;     // the player the stream goes to, or the host it comes from
std::vector<std::uint8_t> g_outgoing;  // stream bytes not sent yet
std::size_t g_forwarded = 0;           // a replay passed on: file offset sent so far

void stream_lost() {
    log("[Coop] the player left: the stream stops, the game goes on");
    g_peer.close();
    g_outgoing.clear();
}

void send_outgoing() {
    if (!g_peer.open() || g_outgoing.empty()) return;
    if (!g_peer.send_all(g_outgoing.data(), g_outgoing.size())) stream_lost();
    g_outgoing.clear();
}

// Waits for the player of --stream; false (and a log line) when there is none.
bool wait_for_player(const std::string& port_text) {
    std::string host, error;
    std::uint16_t port = 0;
    if (!platform::net::split_address(port_text, host, port)) {
        log("[Coop] --stream %s: not a port number", port_text.c_str());
        return false;
    }
    log("[Coop] waiting for a player on port %u", static_cast<unsigned>(port));
    g_peer = platform::net::Connection::accept_one(port, error);
    if (!g_peer.open()) {
        log("[Coop] no player: %s", error.c_str());
        return false;
    }
    log("[Coop] a player joined: streaming");
    return true;
}

// A replay passed on to a player: what the replay has used so far. Also called between frames once enough has gathered, so
// that a player is not kept waiting through the start of the game (loading, the intro), which has no frames.
void forward_used() {
    if (!g_peer.open() || !g_opt.follow.empty()) return;  // only a host passes a replay on
    const std::vector<std::uint8_t> bytes = g_reader.used_since(g_forwarded);
    if (!bytes.empty() && !g_peer.send_all(bytes.data(), bytes.size())) stream_lost();
}
std::string g_file;  // the recording
std::uint32_t g_frame = 0;
bool g_stable_ground_order = false;  // kFlagStableGroundOrder (stable_ground_order())
std::uint64_t g_events = 0;  // call / message / completion events written or replayed
HWND g_window = nullptr;
WNDPROC g_game_proc = nullptr;
std::unordered_set<std::uint32_t> g_handles;
std::atomic<std::uint32_t> g_other_threads[static_cast<int>(Id::Count)];
Recent g_recent[16];
std::size_t g_recent_next = 0;
std::vector<std::string> g_report;
std::size_t g_warnings = 0;
bool g_diverged = false;
bool g_finished = false;
std::uint32_t g_mismatch_frames = 0;
std::uint32_t g_first_mismatch = 0xffffffffu;
std::uint32_t g_image_mismatches = 0, g_heap_mismatches = 0, g_bb_mismatches = 0;
std::vector<std::uint32_t> g_snapshot;  // replay: the recording's snapshot of the current frame
std::uint32_t g_snapshot_frame = 0xffffffffu;
std::map<std::string, std::pair<std::uint64_t, std::uint64_t>> g_file_hashes;  // lower-case path -> size, hash

void report_line(const char* fmt, ...) {
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    hook::log_line("replay: %s", buf);
    if (g_report.size() < 2000) g_report.emplace_back(buf);
}

std::string ini_string(const char* key, const char* def = "") {
    char buf[1024] = {};
    if constexpr (kReleaseBuild) return def;  // no record / replay in the release build (build_flavor.hpp)
    joan::platform::ini::get_string("Debug", key, def, buf, sizeof(buf), hook::ini_path().c_str());
    return buf;
}
int ini_int(const char* key, int def) {
    if constexpr (kReleaseBuild) return def;
    return static_cast<int>(joan::platform::ini::get_int("Debug", key, def, hook::ini_path().c_str()));
}

std::string absolute(const std::string& p) {
    if (p.empty()) return p;
    if (p.size() > 1 && (p[1] == ':' || (p[0] == '\\' && p[1] == '\\'))) return p;  // Windows drive / UNC
    if (p[0] == '/') return p;                                                      // POSIX absolute path
    return hook::data_dir() + p;
}

// --record FILE / --replay FILE on openjoan.exe's command line (override openjoan.ini).
void command_line(Options& o) {
    const std::string cl_text = sys::command_line();
    const char* cl = cl_text.c_str();
    std::vector<std::string> args;
    std::string cur;
    bool quoted = false, any = false;
    for (const char* p = cl; *p; ++p) {
        if (*p == '"') {
            quoted = !quoted;
            any = true;
        } else if ((*p == ' ' || *p == '\t') && !quoted) {
            if (any) args.push_back(cur);
            cur.clear();
            any = false;
        } else {
            cur += *p;
            any = true;
        }
    }
    if (any) args.push_back(cur);
    for (std::size_t i = 1; i + 1 < args.size(); ++i) {
        if (args[i] == "--record") o.record = args[i + 1];
        if (args[i] == "--replay") o.replay = args[i + 1];
        if (args[i] == "--stream") o.stream = args[i + 1];
        if (args[i] == "--follow") o.follow = args[i + 1];
    }
    if (!o.follow.empty() && o.replay.empty()) o.replay = o.follow;  // a followed host is replayed
}

Options read_options() {
    Options o;
    o.record = ini_string("Record");
    o.replay = ini_string("Replay");
    command_line(o);
    o.back_buffer = ini_int("ReplayHashBackBuffer", 0) != 0;
    o.heap = ini_int("ReplayHashHeap", 0) != 0;
    o.files = ini_int("ReplayHashFiles", 1);
    o.snapshot_every = static_cast<std::uint32_t>(std::max(0, ini_int("ReplaySnapshotEvery", 0)));
    o.stop_on_divergence = ini_string("ReplayOnDivergence", "stop") != "continue";
    o.exit_at_end = ini_int("ReplayExitAtEnd", 1) != 0;
    o.pace = ini_int("ReplayPace", 1) != 0;
    o.det_heap = ini_int("ReplayDetHeap", 1) != 0;
    o.snapshot_first = static_cast<std::uint32_t>(std::max(0, ini_int("ReplaySnapshotFirst", 8)));
    o.skip_text = ini_string("ReplayHashSkip");
    o.skip_replay_text = ini_string("ReplayHashSkipReplay");
    o.typed = ini_int("ReplayHashTyped", 1) != 0;
    o.restore_saves = ini_int("ReplayRestoreSaves", 1) != 0;
    const std::string dirs = ini_string("ReplayEnvDirs", "SAVE,CONFIG.DAT");
    std::size_t start = 0;
    while (start <= dirs.size()) {
        std::size_t c = dirs.find(',', start);
        if (c == std::string::npos) c = dirs.size();
        std::string d = dirs.substr(start, c - start);
        while (!d.empty() && d.front() == ' ') d.erase(d.begin());
        while (!d.empty() && d.back() == ' ') d.pop_back();
        if (!d.empty()) o.env_dirs.push_back(d);
        start = c + 1;
    }
    return o;
}

std::uint32_t exe_timestamp() { return sys::exe_stamp(); }  // information only (sys.hpp)

std::string lower(std::string s) {
    for (char& c : s)
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    return s;
}

// ---------------------------------------------------------------- environment fingerprint

std::uint64_t hash_file(const std::string& path, std::uint64_t* size) { return sys::hash_file(path, size); }

void list_dir(const std::string& root, const std::string& rel, int depth, std::vector<std::string>& lines) {
    std::vector<std::string> rels;
    sys::list_files(root, rel, depth, rels);
    for (const std::string& r : rels) {
        std::uint64_t size = 0;
        const std::uint64_t hash = hash_file(root + r, &size);
        char line[600];
        std::snprintf(line, sizeof(line), "%s size %llu hash %016llx", lower(r).c_str(),
                      static_cast<unsigned long long>(size), static_cast<unsigned long long>(hash));
        lines.emplace_back(line);
    }
}

bool is_plain_file(const std::string& path) { return sys::is_plain_file(path); }

// One line per file below the [Debug] ReplayEnvDirs folders (saves, profiles): name, size, content hash.
std::string environment() {
    std::vector<std::string> lines;
    for (const std::string& d : g_opt.env_dirs) {
        std::vector<std::string> part;
        if (is_plain_file(hook::data_dir() + d)) {
            std::uint64_t size = 0;
            const std::uint64_t hash = hash_file(hook::data_dir() + d, &size);
            char line[600];
            std::snprintf(line, sizeof(line), "%s size %llu hash %016llx", lower(d).c_str(),
                          static_cast<unsigned long long>(size), static_cast<unsigned long long>(hash));
            part.emplace_back(line);
        } else {
            list_dir(hook::data_dir(), d, 0, part);
        }
        if (part.empty()) lines.push_back(lower(d) + " (empty or missing)");
        lines.insert(lines.end(), part.begin(), part.end());
    }
    std::sort(lines.begin(), lines.end());
    std::string text;
    for (const std::string& l : lines) text += l + "\n";
    return text;
}

// ---------------------------------------------------------------- save folders carried in the recording
// A recording carries the files of the ReplayEnvDirs folders as they were when it started (the recorded session
// usually changes them itself - autosaves - so a replay on the same machine would otherwise start from other saves).

struct EnvFile {
    std::string path;  // below the game folder, e.g. "SAVE\\mission1-autosave.sav"
    std::string bytes;
};

void collect_files(const std::string& root, const std::string& rel, int depth, std::vector<std::string>& out) {
    sys::list_files(root, rel, depth, out);
}

bool read_whole(const std::string& path, std::string& out) { return sys::read_whole(path, out); }

bool write_whole(const std::string& path, const std::string& bytes) { return sys::write_whole(path, bytes); }

void record_env_files() {
    const std::string& root = hook::data_dir();
    std::size_t files = 0, bytes = 0;
    for (const std::string& d : g_opt.env_dirs) {
        std::vector<std::string> rels;
        if (is_plain_file(root + d))
            rels.push_back(d);  // a single file (CONFIG.DAT: the options the game reads at start-up)
        else
            collect_files(root, d, 0, rels);
        for (const std::string& r : rels) {
            std::string content;
            if (!read_whole(root + r, content)) {
                warn("environment: cannot read %s - not carried in the recording", r.c_str());
                continue;
            }
            g_writer.env(kFileTag + r + "\n" + content);
            ++files;
            bytes += content.size();
        }
    }
    log("environment: %u file(s), %u bytes of the save folders carried in the recording", static_cast<unsigned>(files),
        static_cast<unsigned>(bytes));
}

// Puts the recording's save folders in place: each current ReplayEnvDirs folder is renamed to
// "<folder>.before-replay-<date>-<time>" (nothing is deleted), then the recorded files are written.
// (folder, where it was moved) for every ReplayEnvDirs folder restore_env_files moved aside.
std::vector<std::pair<std::string, std::string>> g_moved_aside;

bool restore_env_files(const std::vector<EnvFile>& files) {
    const std::string& root = hook::data_dir();
    const std::string stamp = sys::local_time_stamp();
    std::vector<std::string> native_dirs;  // the folders as the file system names them (Save, not SAVE, on Linux)
    for (const std::string& d : g_opt.env_dirs) {
        const std::string from = sys::native(root + d);
        native_dirs.push_back(from);
        if (!sys::exists(from)) continue;
        const std::string to = from + ".before-replay-" + stamp;
        std::string error;
        if (!sys::move_path(from, to, &error)) {
            warn("environment: cannot move %s aside (%s) - the save folders are left as they are", from.c_str(),
                 error.c_str());
            return false;
        }
        g_moved_aside.emplace_back(from, to);
        report_line("environment: your %s folder was moved to %s for the replay (moved back when the replay ends)",
                    d.c_str(), to.c_str());
    }
    for (std::size_t i = 0; i < g_opt.env_dirs.size(); ++i) {
        bool recorded_as_file = false;
        for (const EnvFile& f : files) recorded_as_file = recorded_as_file || lower(f.path) == lower(g_opt.env_dirs[i]);
        if (!recorded_as_file) sys::make_dir(native_dirs[i]);  // the same name the user's folder had
    }
    for (const EnvFile& f : files)
        if (!write_whole(root + f.path, f.bytes)) {
            warn("environment: cannot write %s", f.path.c_str());
            return false;
        }
    report_line("environment: the recording's save folders (%u file(s)) put in place", static_cast<unsigned>(files.size()));
    return true;
}

// When the replay ends the process: the replay's save folders become "<folder>.replayed-<date>-<time>" and the user's
// folders go back (nothing deleted). A crash on the way leaves both folders; the log says where they are.
void put_user_saves_back() {
    if (g_moved_aside.empty()) return;
    for (const auto& [folder, aside] : g_moved_aside) {
        const std::string replayed = folder + ".replayed" + aside.substr(aside.rfind('-', aside.rfind('-') - 1));
        std::string error;
        if (sys::exists(folder) && !sys::move_path(folder, replayed, &error)) {
            report_line("environment: cannot move the replay's %s aside (%s) - your folder stays at %s",
                        folder.c_str(), error.c_str(), aside.c_str());
            continue;
        }
        if (!sys::move_path(aside, folder, &error)) {  // folder: the native name it had (g_moved_aside)
            report_line("environment: cannot move %s back (%s) - move it back by hand", aside.c_str(), error.c_str());
            continue;
        }
        report_line("environment: your %s is back (the replay's copy: %s)", folder.c_str(), replayed.c_str());
    }
    g_moved_aside.clear();
}

void compare_environment(const std::string& recorded, const std::vector<EnvFile>& files) {
    if (environment() != recorded && !files.empty() && g_opt.restore_saves) restore_env_files(files);
    const std::string now = environment();
    if (now == recorded) {
        report_line("environment: the save folders match the recording");
        return;
    }
    auto split = [](const std::string& t) {
        std::vector<std::string> v;
        std::size_t s = 0;
        while (s < t.size()) {
            std::size_t e = t.find('\n', s);
            if (e == std::string::npos) e = t.size();
            v.push_back(t.substr(s, e - s));
            s = e + 1;
        }
        return v;
    };
    const std::vector<std::string> a = split(recorded), b = split(now);
    int shown = 0;
    for (const std::string& l : a)
        if (std::find(b.begin(), b.end(), l) == b.end() && shown++ < 20) warn("environment: recorded only: %s", l.c_str());
    for (const std::string& l : b)
        if (std::find(a.begin(), a.end(), l) == a.end() && shown++ < 40) warn("environment: now only: %s", l.c_str());
    warn(files.empty() ? "environment: the save folders differ from the recording - the replay will likely diverge "
                         "(this recording carries no save files: copy the recording machine's save folder)"
                       : g_opt.restore_saves ? "environment: the save folders still differ from the recording after "
                                               "restoring them - the replay will likely diverge"
                                             : "environment: the save folders differ from the recording - the replay "
                                               "will likely diverge ([Debug] ReplayRestoreSaves=0)");
}

// ---------------------------------------------------------------- state hashes

// Committed memory of the process, merged (rebuilt every frame: pointer words are recognised by what is mapped).
// (Reserved ranges were tried in patch 135: low reserves made small numbers and text look like pointers.)
std::vector<std::pair<std::uint32_t, std::uint32_t>> g_regions;
std::vector<std::uint32_t> g_alloc_bases;
std::vector<std::pair<std::uint32_t, std::uint32_t>> g_readable;  // committed, no PAGE_GUARD / PAGE_NOACCESS  // sorted AllocationBase of every committed private / image region
std::vector<std::pair<std::uint32_t, std::uint32_t>> g_images;    // committed MEM_IMAGE ranges (exe and DLLs)
// Every range that was committed at any frame of this run, merged: a 4-aligned value in one of them that is no longer
// mapped is a pointer the game kept past a release, not a number (the word 0x00a56794 of the PANEL sprite set
// 0x00a563a0, TODO(re): 0x00a56794 which member: at frame 9124 of ref-x86-2026-10-29.rec it took a 4-aligned value in
// the heap range that was unmapped when the frame was hashed). Such a word hashed raw made the recording depend on
// where glibc had put a freed block: the ADR-0002 icon's SDL surface moved it (replay by the icon build: frame 9124
// differed, 2186 frames in all; the same build without the icon matched every frame).
std::vector<std::pair<std::uint32_t, std::uint32_t>> g_ever_mapped;

void build_regions() {
    g_regions.clear();
    g_alloc_bases.clear();
    g_readable.clear();
    g_images.clear();
    static std::vector<sys::MemRegion> regions;
    sys::committed_regions(regions);
    for (const sys::MemRegion& r : regions) {
        if (r.end > 0xffffffffull || r.begin > 0xffffffffull) continue;  // the hashes name addresses as 32-bit values
        const auto b32 = static_cast<std::uint32_t>(r.begin), e32 = static_cast<std::uint32_t>(r.end);
        if (r.readable) g_readable.emplace_back(b32, e32);
        if (r.image) {
            if (!g_images.empty() && g_images.back().second == b32)
                g_images.back().second = e32;
            else
                g_images.emplace_back(b32, e32);
        }
        const auto ab = static_cast<std::uint32_t>(r.allocation_base);
        if (g_alloc_bases.empty() || g_alloc_bases.back() != ab) g_alloc_bases.push_back(ab);
        if (!g_regions.empty() && g_regions.back().second == b32)
            g_regions.back().second = e32;
        else
            g_regions.emplace_back(b32, e32);
    }
    // the union with the ranges of the frames before (both lists sorted and merged)
    std::vector<std::pair<std::uint32_t, std::uint32_t>> all;
    all.reserve(g_ever_mapped.size() + g_regions.size());
    std::merge(g_ever_mapped.begin(), g_ever_mapped.end(), g_regions.begin(), g_regions.end(), std::back_inserter(all));
    g_ever_mapped.clear();
    for (const auto& r : all) {
        if (!g_ever_mapped.empty() && r.first <= g_ever_mapped.back().second)
            g_ever_mapped.back().second = std::max(g_ever_mapped.back().second, r.second);
        else
            g_ever_mapped.push_back(r);
    }
}

// Reads 4 bytes at a, or returns false if that faults (patch 157 recording: an access violation in the Mem-magic
// probe although VirtualQuery had called the page readable - belt and braces with SEH on Windows; elsewhere the
// mapping is checked first, sys.hpp). The stray guard (a vectored handler, which runs before __except) is told to
// leave the fault to this probe.
bool safe_read_u32(std::uint32_t a, std::uint32_t* out) {
    const image_runtime::ProbeScope probe;
    return sys::read_u32(static_cast<std::uintptr_t>(a), out);
}

// Reading [v, v + n) cannot fault (patch 146 read a stack guard page here: exception 0x80000001).
// [kFlagNoPlnCache] Cell::load_files (0x004f2f80, game/world_s.cpp) reads map\<cell>.PLN when g_a0b6ac is set and the
// file is newer than the cell's .GRI, and Cell::build_ground (0x0055b720) writes it when g_a0b6b0[0] is set. Both are 1 in joan.exe and
// nothing changes them. The cache only saves load time, but whether a PLN exists (or is complete: a replay stopped while
// one was written leaves it truncated) decides the file calls of a cell load, so recordings turn both off.
void pln_cache_off() {
    img::global<std::uint32_t>(d::g_a0b6ac) = 0;
    img::global<std::int32_t>(d::g_world_write_pln) = 0;
}

bool readable(std::uint32_t v, std::uint32_t n) {
    auto it = std::upper_bound(g_readable.begin(), g_readable.end(), v,
                               [](std::uint32_t x, const std::pair<std::uint32_t, std::uint32_t>& r) { return x < r.first; });
    if (it == g_readable.begin()) return false;
    --it;
    return v + n <= it->second;
}

bool in_image_module(std::uint32_t v) {
    auto it = std::upper_bound(g_images.begin(), g_images.end(), v,
                               [](std::uint32_t x, const std::pair<std::uint32_t, std::uint32_t>& r) { return x < r.first; });
    if (it == g_images.begin()) return false;
    --it;
    return v < it->second;
}

bool mapped(std::uint32_t v) {
    auto it = std::upper_bound(g_regions.begin(), g_regions.end(), v,
                               [](std::uint32_t x, const std::pair<std::uint32_t, std::uint32_t>& r) { return x < r.first; });
    if (it == g_regions.begin()) return false;
    --it;
    return v <= it->second;  // one past the end too (end pointers of arrays that fill their region)
}

// v was mapped at an earlier frame of this run (g_ever_mapped) and is not now: a dangling pointer.
bool was_mapped(std::uint32_t v) {
    auto it = std::upper_bound(g_ever_mapped.begin(), g_ever_mapped.end(), v,
                               [](std::uint32_t x, const std::pair<std::uint32_t, std::uint32_t>& r) { return x < r.first; });
    if (it == g_ever_mapped.begin()) return false;
    --it;
    return v <= it->second;
}

// A word of game state as a value that is the same in every run: pointers into joan_image become their original
// address, code pointers their original code address, other pointers and handles a tag.
// Four bytes of text (printable ASCII / NUL, at least two printable), or a byte pattern (three or four equal
// non-zero bytes, e.g. 0x1e1e1e1e in a byte table), are data, not a pointer.
bool looks_like_text(std::uint32_t v) {
    const std::uint32_t b0 = v & 0xff, b1 = (v >> 8) & 0xff, b2 = (v >> 16) & 0xff, b3 = v >> 24;
    if ((b0 != 0 && b0 == b1 && b1 == b2) || (b1 != 0 && b1 == b2 && b2 == b3)) return true;
    // smooth small-byte ramps of the game's byte tables (0x04030201, 0x0a090908, 0x13121212 ...)
    {
        const std::uint32_t b[4] = {b0, b1, b2, b3};
        int nonzero = 0;
        bool smooth = true;
        for (int i = 0; i < 4; ++i) {
            if (b[i] > 0x3f) smooth = false;
            if (b[i] != 0) ++nonzero;
            if (i && (b[i] > b[i - 1] ? b[i] - b[i - 1] : b[i - 1] - b[i]) > 3) smooth = false;
        }
        if (smooth && nonzero >= 3) return true;
    }
    int printable = 0;
    for (int i = 0; i < 4; ++i) {
        const std::uint32_t c = (v >> (8 * i)) & 0xff;
        if (c >= 0x20 && c < 0x7f) ++printable;
        else if (c != 0) return false;
    }
    return printable >= 2;
}

// openjoan.exe's address range (pointers into it: code slots -> original addresses, everything else one tag).
std::uint32_t g_exe_begin = 0, g_exe_end = 0;
#if JOAN_IMAGE_X64
std::uintptr_t g_exe_begin_full = 0, g_exe_end_full = 0;  // the same range as full addresses (x64: normalise_wide)
#endif
// A pointer into openjoan.exe that is not a code address of a slot: one tag, no offset - the offset changes with
// every build, and a replay must compare against a recording made by an older build (the refactoring gate).
// (Same value as kTagPointer: whether an address falls into the exe's range or a heap / view next to it can differ
// between runs, test23.)
constexpr std::uint32_t kTagExe = 0xA110C000u;

void find_exe_range() {
    std::uintptr_t low = 0, high = 0;
    sys::exe_range(low, high);
#if JOAN_IMAGE_X64
    g_exe_begin_full = low;
    g_exe_end_full = high;
#endif
    g_exe_begin = static_cast<std::uint32_t>(low);
    g_exe_end = static_cast<std::uint32_t>(high);
}

// A 64 KB-aligned number is a pointer only when it is the start of an allocation (VirtualAlloc result); otherwise
// it is a flag / size / fixed-point value (0x00aa0000, 0x10100000, 0x20000000 ...).
bool allocation_start(std::uint32_t v) {
    return std::binary_search(g_alloc_bases.begin(), g_alloc_bases.end(), v);  // (regions are walked in address order)
}

// Pointer classification shared by aligned words and unaligned fields: the normalised value, or v itself.
std::uint32_t pointer_value(std::uint32_t v, bool* is_pointer) {
    *is_pointer = true;
    if (img::in_image(static_cast<std::uintptr_t>(v))) return img::original_of(static_cast<std::uint32_t>(v - img::base()));
    if (v >= 0x10000 && mapped(v)) {
        if (const std::uint32_t code = fn::original_code_address(v)) return code;
        if (v >= g_exe_begin && v < g_exe_end) return kTagExe;
        // Data pointers are 4-aligned (heap blocks, objects); an unaligned "address" is a byte table / flags.
        // (Since patch 141 only the recording classifies and the replay uses its positions, so a false "pointer"
        // costs nothing but hiding that word; 64 KB-aligned values are taken when mapped - big blocks inside a
        // reservation are 64 KB-aligned without being an allocation start: 0x081c0000 ...)
        if ((v & 3) == 0 && ((v & 0xffff) != 0 || v >= 0x01000000 || allocation_start(v))) return kTagPointer;
    } else if (v >= 0x10000 && (v & 3) == 0 && ((v & 0xffff) != 0 || v >= 0x01000000) && was_mapped(v)) {
        return kTagPointer;  // a dangling pointer into a block freed since (g_ever_mapped)
    }
    *is_pointer = false;
    return v;
}

std::uint32_t normalise(std::uint32_t v) {
    if (v == 0) return 0;
    if (looks_like_text(v)) return v;
    if (g_window && v == recorded_bits(g_window)) return kTagWindow;
    if (v < 0x100000 && !g_handles.empty() && g_handles.count(v)) return kTagHandle;  // handles are small numbers
    bool p = false;
    return pointer_value(v, &p);
}

// The data image as it was when replay was installed (before the game ran): a word still holding that value is
// static data (tables, strings) and is hashed as it is, never guessed to be a pointer - whether a number "points"
// somewhere depends on the run's address space. Image / code pointers among them still normalise.
std::vector<std::uint32_t> g_pristine;

#if JOAN_IMAGE_X64
// x64: everything below hashes and classifies the hashed range in the ORIGINAL (x86) layout, so an x64 run compares
// with an x86 recording (x86_view.hpp): a copy of .data + .bss read field by field through image_xlate.inc's runs,
// pointers as their low 4 bytes (all the x86 layout has room for); masked_words then normalises every widened pointer
// from its full value (normalise_wide). Rebuilt on every call (a few per frame; 0x79000 bytes). x86 builds hash
// joan_image itself (image_bytes below): no copy, the same code as before.
#ifndef JOAN_IMAGE_LAYOUT_PROBE
static_assert(sizeof img::kXlate == sizeof x86_view::kX64Runs && img::kSize == x86_view::kX64ImageSize,
              "x86_view.hpp and image.hpp read the same src/data/image_xlate.inc");
#endif
std::vector<std::uint8_t> g_x86_view;
constexpr std::size_t kXlateRuns = sizeof img::kXlate / sizeof img::kXlate[0];

const std::uint8_t* x86_image() {
    g_x86_view.resize(kDataSize);
    x86_view::copy(img::kXlate, kXlateRuns, joan_image, kDataVa, kDataSize, g_x86_view.data());
    return g_x86_view.data();
}
#endif

void capture_pristine() {
#if JOAN_IMAGE_X64
    const auto* src = reinterpret_cast<const std::uint32_t*>(x86_image());
#else
    const auto* src = reinterpret_cast<const std::uint32_t*>(img::addr(img::Addr(kDataVa)));
#endif
    g_pristine.assign(src, src + kDataSize / 4);
}

// Pointer dwords of joan.exe's data (data_relocs.inc), many of them unaligned (e.g. the CRT's _trandisp
// descriptors 0x00a3119a..: code pointers at 2-byte offsets). While a site still holds what it held at install, it
// is hashed as joan.exe's original value - aligned word normalisation cannot see a pointer split over two words.
struct RelocSite {
    std::uint32_t va, original;
};
constexpr RelocSite kRelocSites[] = {
#define JOAN_DATA_RELOC(address, kind, value) {address, value},
#include "../../data/data_relocs.inc"
#undef JOAN_DATA_RELOC
};

std::uint32_t normalise_static(std::uint32_t v) {
    if (v != 0 && img::in_image(static_cast<std::uintptr_t>(v))) return img::original_of(static_cast<std::uint32_t>(v - img::base()));
    if (const std::uint32_t code = v >= 0x10000 ? fn::original_code_address(v) : 0) return code;
    return v;
}

// ---------------------------------------------------------------- typed scheme (typed_hash.hpp)
// A kFlagTypedHash recording (and its replay) knows from the types of the data image which bytes hold pointers: those
// are normalised, typed data is hashed as it is, and the heuristics below only ever look at 4-byte windows that lie
// wholly in untyped bytes. Recordings without the flag (version 1) hash with the heuristics alone, as they were made.
bool g_typed = false;
typed::Table g_table;               // the recording's table (recording: this build's)
std::vector<std::uint8_t> g_class;  // typed::Class of every byte of the hashed range

// The heuristics may consider the window at byte offset o.
bool heuristic_window(std::uint32_t o) { return !g_typed || typed::untyped4(g_class, o); }

// ---------------------------------------------------------------- pointer positions (the "mask")
// Which 4-byte positions of .data / .bss hold a pointer is decided ONLY while recording (the heuristics below) and
// written to the recording whenever it changes (Env event "mask\n" + delta). A replay applies the recording's
// positions instead of guessing with its own address space: a heuristic miss or false hit is then the same in both
// runs, and only real differences of game state remain. At a pointer position both runs hash forced_normalise(v).

std::vector<std::uint32_t> g_mask;         // sorted byte offsets (from kDataVa) of pointer positions in use
std::vector<std::uint32_t> g_new_mask;
bool g_have_recorded_mask = false;         // replay: the recording carries masks (patch 141 and later)
constexpr char kMaskTag[] = "mask\n";
// Byte offsets (from kDataVa) of the 4-byte windows that held an address inside openjoan.exe in the RECORDING run
// (any offset, see masked_words). Written like the mask (Env "blank\n" + delta, the first frame always), because the
// exe's range differs between the two runs (another build, another base): a run's own range would blank game numbers
// that happen to fall into it in one run only (test24 replayed by patch 162: 2,493 words).
std::vector<std::uint32_t> g_blank;
std::vector<std::uint32_t> g_new_blank;
bool g_have_recorded_blank = false;        // replay: the recording carries blank positions (patch 163 and later)
bool g_blank_written = false;              // recording: the first frame's set was written
constexpr char kBlankTag[] = "blank\n";

// What a pointer becomes, whatever this run's address space looks like (no "is it mapped" test).
constexpr std::uint32_t kTagHeap = 0x80000000u;  // | offset in joan::det_heap (same in every run)

// The address layout of a run: where the data image, openjoan.exe, the deterministic heap and the trap stubs are.
// The recording writes its own (Env "layout\n", patch 165). A mask position is decided by the recording, and some
// are numbers that only looked like a pointer there (a byte table forming an address of the data image: test25
// replayed by patch 164, 0x00a6cbc8 in _mbctype). Such a number is the same in the replay, but the image is
// elsewhere in another build: the replay then normalises it with the recording's layout.
// image_x64: the run's joan_image had the x64 layout (image pointers are base + an x64 offset, image_xlate.inc) and its
// trap stubs are x64 stubs. Written by x64 builds only: an x86 build writes the first kLayoutSizeX86 bytes as before,
// and such a payload (every recording made before) reads as image_x64 = 0.
struct Layout {
    std::uint32_t image = 0, exe_begin = 0, exe_end = 0, heap = 0, stubs = 0;
    std::uint32_t image_x64 = 0;
};
constexpr std::size_t kLayoutSizeX86 = 5 * sizeof(std::uint32_t);
constexpr std::size_t kLayoutWritten = JOAN_IMAGE_X64 ? sizeof(Layout) : kLayoutSizeX86;
constexpr std::uint32_t kTrapStubSizeX86 = 10, kTrapStubSizeX64 = 24;  // code_slots_core.hpp per architecture
static_assert(joan::code::kTrapStubSize == (JOAN_IMAGE_X64 ? kTrapStubSizeX64 : kTrapStubSizeX86));
static_assert(JOAN_IMAGE_X64 || img::kSize == x86_view::kX86ImageSize);
Layout g_rec_layout;          // replay: the recording's
bool g_have_rec_layout = false;
constexpr char kLayoutTag[] = "layout\n";

// (u32 addresses, the recording format's: on x64 a field keeps an address's low 32 bits. det_heap's fixed bases have
// the low 32 bits 0x20000000..0x50000000 on both architectures (x64: 0x7e00'0000'0000 above), so v - l.heap of a heap
// pointer's low bits is its offset as on x86, and kTagHeap | offset never collides with the offset's own bits)
Layout this_layout() {
    Layout l;
    l.image = static_cast<std::uint32_t>(img::base());
    l.exe_begin = g_exe_begin;
    l.exe_end = g_exe_end;
    l.heap = det_heap::active() ? static_cast<std::uint32_t>(det_heap::base()) : 0;
    l.stubs = static_cast<std::uint32_t>(code_table::stubs_begin());
    l.image_x64 = JOAN_IMAGE_X64;
    return l;
}

// A value in layout l: the image VA / heap tag / original code address of a trap stub it stands for, kExeAny for
// anything in openjoan.exe, 0 when it is none of these.
constexpr std::uint32_t kExeAny = 1;
std::uint32_t normalise_in(const Layout& l, std::uint32_t v) {
    // the heap's used part: the same size in both runs (identical request sequence)
    const std::uint32_t heap_used =
        det_heap::active() ? static_cast<std::uint32_t>(det_heap::used_end() - det_heap::base()) : 0;
    if (l.heap && v - l.heap < heap_used) return kTagHeap | (v - l.heap);
    // (the image's size and offsets are those of the run's layout: an x86 recording replayed on x64 and back)
    if (l.image && !l.image_x64 && v - l.image < x86_view::kX86ImageSize) return x86_view::kImageVa + (v - l.image);
    if (l.image && l.image_x64 && v - l.image < x86_view::kX64ImageSize) return x86_view::x64_original_of(v - l.image);
    // Anything in openjoan.exe (code of a replacement or not, our data): one tag. What a code address of ours stands
    // for changes between builds (identical functions are folded, helpers are or are not slot targets: test25).
    if (v >= l.exe_begin && v < l.exe_end) return kExeAny;
    // the trap stubs: [stubs, stubs + slots * stub size) - the same slot count in every build; without the upper
    // bound any pointer above the stubs (a Windows heap / D3D buffer) aligned like a stub "was" a stub (test31 on the
    // R11 build: 0x00a48910 read as dtor_stub_568110)
    // (stubs of the run's architecture: the same count, kTrapStubSizeX86 / X64 bytes each)
    const std::uint32_t stub_size = l.image_x64 ? kTrapStubSizeX64 : kTrapStubSizeX86;
    const std::uint32_t stub_span = static_cast<std::uint32_t>(
        (code_table::stubs_end() - code_table::stubs_begin()) / joan::code::kTrapStubSize * stub_size);
    if (l.stubs && v >= l.stubs && v - l.stubs < stub_span) {
        const std::uint32_t off = v - l.stubs;
        if (off % stub_size == 0)
            if (const std::uint32_t va = fn::va(static_cast<fn::Code>(off / stub_size))) return va;
    }
    return 0;
}

// A mask position's value in layout l (this run's, or on replay the recording's: see g_rec_raw).
std::uint32_t forced_normalise(std::uint32_t v, const Layout& l) {
    if (v == 0) return 0;
    // window / handles first: a handle value can fall inside an address range by chance (patch 150 report: the
    // window handle looked like an image pointer in one run)
    if (g_window && v == recorded_bits(g_window)) return kTagWindow;
    if (v < 0x100000 && !g_handles.empty() && g_handles.count(v)) return kTagHandle;
    const std::uint32_t n = normalise_in(l, v);
    if (n > kExeAny) return n;
    if (n == 0) {
        // code resolved into a DLL (the same address in both runs)
        if (const std::uint32_t code = fn::original_code_address(v)) return code;
    }
    return kTagPointer;
}

#if JOAN_IMAGE_X64
// x64: a pointer of this run from its full 64-bit value, as forced_normalise classifies a 32-bit one in this run's
// layout. The x86 view holds only a widened pointer's low 32 bits, and those cannot be classified: with PIE, ASLR and
// the deterministic heap above 4 GB, the low half of a pointer to a driver / SDL / glibc block or to our code falls
// into the 32-bit image of the heap's or the stubs' range by chance, differently in every run, and more often as the
// heap grows (ref-x86-2026-11-6x64test.rec replayed twice on x64, 2026-11-07: 132 hashed words differed between the
// two runs - the renderer's device and buffer pointers read as heap offsets, the CRT's function tables as heap offsets
// in one run and code in the other, and g_surface_base below; none with this function).
std::uint32_t normalise_wide(std::uint64_t p) {
    if (p == 0) return 0;
    if (g_window && p == reinterpret_cast<std::uintptr_t>(g_window)) return kTagWindow;
    if (p < 0x100000 && !g_handles.empty() && g_handles.count(static_cast<std::uint32_t>(p))) return kTagHandle;
    if (det_heap::contains(p)) return kTagHeap | static_cast<std::uint32_t>(p - det_heap::base());
    if (img::in_image(p)) return img::original_of(static_cast<std::uint32_t>(p - img::base()));
    if (p >= g_exe_begin_full && p < g_exe_end_full) return kTagPointer;  // forced_normalise: kExeAny
    const std::uintptr_t stubs = code_table::stubs_begin();
    if (p >= stubs && p < code_table::stubs_end() && (p - stubs) % joan::code::kTrapStubSize == 0)
        if (const std::uint32_t va = fn::va(static_cast<fn::Code>((p - stubs) / joan::code::kTrapStubSize))) return va;
    if (const std::uint32_t code = fn::original_code_address(p)) return code;
    return kTagPointer;
}

// 4-byte image slots in which the x64 build keeps the low half of a pointer, read by nothing (call.hpp
// pointer_low_dword): random with ASLR, so not hashed on x64 (zero in the recording and the replay; a mask position
// overlapping one is not applied). 0x00a15010 g_surface_base (blit16.cpp / blitfx.cpp).
constexpr std::uint32_t kX64LowDwordSlots[] = {0x00a15010u};
bool overlaps_low_dword_slot(std::uint32_t o) {
    for (const std::uint32_t va : kX64LowDwordSlots)
        if (o + 4 > va - kDataVa && o < va - kDataVa + 4) return true;
    return false;
}
#endif

// The raw value of every mask position as the recording saw it (Env "raw\n" deltas: offset/value pairs written
// whenever a position is added or its value changes, patch 167). On replay a position whose bytes are exactly the
// recorded ones is a number that only looked like a pointer (or a pointer to the same place, a DLL): it is
// normalised with the recording's layout, so it hashes as the recording hashed it. A real pointer of this run has
// other bytes (another image base / heap / stub address) and is normalised with this run's layout. (test26 replayed
// by patch 166: data that formed image addresses in one build, and pointers to our string literals that fell into
// the other build's image range.)
// Sorted by offset (walked together with the sorted mask: no per-word lookups, no allocations per frame - a hash map
// rebuilt every frame made recording and replay stutter, test27).
struct RawAt {
    std::uint32_t o, v;
};
std::vector<RawAt> g_rec_raw;
std::vector<RawAt> g_rec_raw_next;
constexpr char kRawTag[] = "raw\n";

// Recording: the pointer positions of one 4 KB page (byte offsets from kDataVa, sorted). Unaligned candidates
// that overlap each other are resolved by alignment (allocator results are 8 / 16-aligned, a false candidate made
// of neighbouring fields rarely is), then by position; consecutive unaligned pointers (arrays of packed records)
// are all taken - coverage is tracked per byte.
std::vector<std::vector<std::uint32_t>> g_page_relocs;  // reloc site offsets per page (built once)

// An unaligned 4-byte pointer candidate at o (changed since install): returns false or its rank.
bool unaligned_candidate(const std::uint8_t* image, std::uint32_t o, int* rank) {
    if (o + 4 > kDataSize || g_pristine.size() != kDataSize / 4 || !heuristic_window(o)) return false;
    const auto* pristine = reinterpret_cast<const std::uint8_t*>(g_pristine.data());
    std::uint32_t v = 0;
    std::memcpy(&v, image + o, 4);
    const bool in_exe = v >= g_exe_begin && v < g_exe_end;
    if (!in_exe && ((v & 3) != 0 || (v >> 24) == 0)) return false;  // cheap rejects first (most bytes)
    if (std::memcmp(image + o, pristine + o, 4) == 0) return false;
    if (in_exe) {
        // a slot's code address (any alignment), or a 4-aligned pointer to our own data (vtables / objects of the
        // replacements, stored unaligned in packed game records): those differ between builds, so they must be
        // found to be masked (patch 157, test21 replayed by another build)
        if (fn::original_code_address(v) == 0 && (v & 3) != 0) return false;
    } else {
        // a heap block address: 4-aligned, the low half and the top byte non-zero (0x001e0000 / 0x00102000 ... are
        // numbers straddling fields), mapped, not text
        if ((v & 3) != 0 || (v >> 24) == 0 || !mapped(v) || looks_like_text(v)) return false;
        bool p = false;
        pointer_value(v, &p);
        if (!p) return false;
    }
    // rank: alignment up to 16, and a zero low byte counts against - that is usually the real pointer one byte
    // further, shifted left by a zero byte in front of it
    *rank = std::min(std::countr_zero(v), 4) - ((v & 0xff) == 0 ? 8 : 0);
    // the body of a block of the game's Mem allocator (header magic 0x12345678 right before it): certainly a pointer
    std::uint32_t magic = 0;
    if (v >= 0x10010 && readable(v - 4, 4) && safe_read_u32(v - 4, &magic) && magic == 0x12345678u) *rank += 32;
    return true;
}

bool aligned_pointer_word(const std::uint8_t* image, std::uint32_t o) {
    if (!heuristic_window(o)) return false;
    std::uint32_t v = 0;
    std::memcpy(&v, image + o, 4);
    if (v == 0) return false;
    // DynArray::body_buf at +0x19: the aligned word at +0x18 is (pointer << 8) | a zero byte, and often looks like a
    // pointer too - the real one starts a byte later
    int rank = 0;
    if ((v & 0xff) == 0 && (o & 3) == 0 && unaligned_candidate(image, o + 1, &rank)) return false;
    const bool is_static = g_pristine.size() == kDataSize / 4 && v == g_pristine[o / 4];
    if ((is_static ? normalise_static(v) : normalise(v)) != v) return true;
    // A heap address whose bytes happen to be printable (0x36733754 = "T7s6": the IWMSyncReader of the Video object
    // 0x00a5d2f0+0x24, test25) is still a pointer when it points at an object with a vtable: its first word lies in
    // the exe or a DLL.
    if (!is_static && (v & 3) == 0 && looks_like_text(v) && readable(v, 4)) {
        std::uint32_t w = 0;
        if (safe_read_u32(v, &w) && in_image_module(w)) return true;
    }
    return false;
}

void classify_page(const std::uint8_t* image, std::uint32_t page, std::vector<std::uint32_t>& out) {
    out.clear();
    const bool have_pristine = g_pristine.size() == kDataSize / 4;
    const auto* pristine = reinterpret_cast<const std::uint8_t*>(g_pristine.data());
    const std::uint32_t b = page * kPageSize, e = b + kPageSize;
    std::uint8_t covered[kPageSize + 8] = {};
    auto free4 = [&](std::uint32_t o) {
        for (std::uint32_t k = 0; k < 4; ++k)
            if (covered[o + k - b]) return false;
        return true;
    };
    auto cover = [&](std::uint32_t o) {
        for (std::uint32_t k = 0; k < 4; ++k) covered[o + k - b] = 1;
        out.push_back(o);
    };
    if (have_pristine) {
        for (const std::uint32_t o : g_page_relocs[page])
            if (heuristic_window(o) && std::memcmp(image + o, pristine + o, 4) == 0) cover(o);
    }
    for (std::uint32_t o = b; o < e; o += 4)
        if (free4(o) && aligned_pointer_word(image, o)) cover(o);
    if (e < kDataSize && aligned_pointer_word(image, e))  // the next page's first word, for candidates crossing into it
        for (std::uint32_t k = 0; k < 4; ++k) covered[kPageSize + k] = 1;
    if (have_pristine) {
        struct Cand {
            std::uint32_t o;
            int align;
        };
        std::vector<Cand> cands;
        for (std::uint32_t o = b + 1; o < e && o + 4 <= kDataSize; ++o) {
            if ((o & 3) == 0 || !free4(o)) continue;
            int rank = 0;
            if (unaligned_candidate(image, o, &rank)) cands.push_back({o, rank});
        }
        std::stable_sort(cands.begin(), cands.end(), [](const Cand& x, const Cand& y) { return x.align > y.align; });
        for (const Cand& c : cands)
            if (free4(c.o)) cover(c.o);
    }
    std::sort(out.begin(), out.end());
}

// Recording: the pointer positions of this frame. Only pages that changed since the last frame (or whose next
// page changed: candidates cross the boundary) are classified again; the others keep their positions.
std::vector<std::uint8_t> g_prev_raw;
std::vector<std::vector<std::uint32_t>> g_page_mask;

void classify(const std::uint8_t* image, std::vector<std::uint32_t>& mask) {
    if (g_page_relocs.empty()) {
        g_page_relocs.resize(kPages);
        for (const RelocSite& r : kRelocSites)
            if (r.va >= kDataVa && r.va + 4 <= kDataEnd) g_page_relocs[(r.va - kDataVa) / kPageSize].push_back(r.va - kDataVa);
    }
    const bool first = g_prev_raw.size() != kDataSize;
    if (first) g_page_mask.assign(kPages, {});
    std::vector<std::uint8_t> changed(kPages + 1, 0);
    for (std::uint32_t p = 0; p < kPages; ++p)
        changed[p] = first || std::memcmp(image + p * kPageSize, g_prev_raw.data() + p * kPageSize, kPageSize) != 0;
    bool any = false;
    for (std::uint32_t p = 0; p < kPages; ++p) any = any || changed[p];
    if (any) build_regions();
    for (std::uint32_t p = 0; p < kPages; ++p)
        if (changed[p] || changed[p + 1]) classify_page(image, p, g_page_mask[p]);
    g_prev_raw.assign(image, image + kDataSize);
    mask.clear();
    for (const auto& m : g_page_mask) mask.insert(mask.end(), m.begin(), m.end());
}

// Recording: the change of the mask since the last frame, as an Env event (nothing when unchanged).
void write_delta(const char* tag, const std::vector<std::uint32_t>& now, const std::vector<std::uint32_t>& next,
                 bool always) {
    std::vector<std::uint32_t> added, removed;
    std::set_difference(next.begin(), next.end(), now.begin(), now.end(), std::back_inserter(added));
    std::set_difference(now.begin(), now.end(), next.begin(), next.end(), std::back_inserter(removed));
    if (added.empty() && removed.empty() && !always) return;
    std::string text = tag;
    auto put = [&text](std::uint32_t v) { text.append(reinterpret_cast<const char*>(&v), 4); };
    put(static_cast<std::uint32_t>(added.size()));
    for (const std::uint32_t o : added) put(o);
    put(static_cast<std::uint32_t>(removed.size()));
    for (const std::uint32_t o : removed) put(o);
    g_writer.env(text);
}

// Replay: applies a recorded delta (Env payload after the tag) to a sorted offset set.
void apply_delta(std::vector<std::uint32_t>& set, const std::uint8_t* p, std::size_t n) {
    auto get = [&](std::uint32_t& v) {
        if (n < 4) return false;
        std::memcpy(&v, p, 4);
        p += 4;
        n -= 4;
        return true;
    };
    std::uint32_t count = 0, o = 0;
    std::vector<std::uint32_t> added, removed;
    if (!get(count)) return;
    for (std::uint32_t i = 0; i < count && get(o); ++i) added.push_back(o);
    if (!get(count)) count = 0;
    for (std::uint32_t i = 0; i < count && get(o); ++i) removed.push_back(o);
    std::vector<std::uint32_t> kept;
    std::set_difference(set.begin(), set.end(), removed.begin(), removed.end(), std::back_inserter(kept));
    set.clear();
    std::merge(kept.begin(), kept.end(), added.begin(), added.end(), std::back_inserter(set));
}

// Recording: the windows (any offset) whose value lies in this run's exe range.
void exe_windows(const std::uint8_t* image, std::vector<std::uint32_t>& out) {
    out.clear();
    // and the trap stubs (VirtualAlloc'd, moves with the exe): stub addresses sit unaligned in packed records where
    // the classifier does not take them (test26: 0x00a51db5.. records)
    const std::uint32_t stubs = static_cast<std::uint32_t>(code_table::stubs_begin());
    const std::uint32_t stubs_end = static_cast<std::uint32_t>(code_table::stubs_end());
    for (std::uint32_t o = 0; o + 4 <= kDataSize; ++o) {
        std::uint32_t v = 0;
        std::memcpy(&v, image + o, 4);
        if (((v >= g_exe_begin && v < g_exe_end) || (v >= stubs && v < stubs_end)) && heuristic_window(o)) out.push_back(o);
    }
}

// The hashed words: the image as it is, every mask position replaced by forced_normalise of its 4 bytes.
void masked_words(const std::uint8_t* image, const std::vector<std::uint32_t>& mask, const std::vector<std::uint32_t>* blank,
                  std::vector<std::uint32_t>& out) {
    out.resize(kDataSize / 4);
    std::memcpy(out.data(), image, kDataSize);
    auto* dst = reinterpret_cast<std::uint8_t*>(out.data());
    const Layout here = this_layout();
    std::size_t ri = 0;
    for (const std::uint32_t o : mask) {
        if (o + 4 > kDataSize) continue;
#if JOAN_IMAGE_X64
        if (overlaps_low_dword_slot(o)) continue;
#endif
        std::uint32_t v = 0;
        std::memcpy(&v, image + o, 4);
        bool recorded_bytes = false;
        if (g_have_rec_layout) {
            while (ri < g_rec_raw.size() && g_rec_raw[ri].o < o) ++ri;
            recorded_bytes = ri < g_rec_raw.size() && g_rec_raw[ri].o == o && g_rec_raw[ri].v == v;
        }
        const std::uint32_t n = forced_normalise(v, recorded_bytes ? g_rec_layout : here);
        std::memcpy(dst + o, &n, 4);
    }
    // Typed scheme: every pointer field is a pointer of this run, normalised with this run's layout (the mask above
    // only holds positions in untyped bytes; typed data stays as it is).
    if (g_typed)
        typed::normalise_pointers(g_table, image, dst, kDataSize, [&here](std::uint32_t v) { return forced_normalise(v, here); });
#if JOAN_IMAGE_X64
    // x64: every widened pointer slot (a pointer by the layout, typed or not) from its full value; the slots that hold
    // a pointer's low half are left out
    x86_view::for_each_wide_pointer(img::kXlate, kXlateRuns, joan_image, kDataVa, kDataSize,
                                    [dst](std::uint32_t va, std::uint64_t p) {
                                        const std::uint32_t n = normalise_wide(p);
                                        std::memcpy(dst + (va - kDataVa), &n, 4);
                                    });
    for (const std::uint32_t va : kX64LowDwordSlots) std::memset(dst + (va - kDataVa), 0, 4);
#endif
    // Build independence: every 4 bytes (any offset) that hold an address inside openjoan.exe are blanked in both
    // runs, mask or not - code and data of the exe move with each build, and a pointer stored at a packed offset is
    // easily covered only partly by a mask position (test21 / test22 replayed by a newer build). What the game
    // stores there (which of our functions / objects) is not compared; that it stores the same game data is.
    // Values of game data that happen to fall into the exe's address range are blanked too (rare: ~7 MB of numbers).
    // The windows are the recording's (`blank`, see g_blank); without them (a recording from before patch 163) this
    // run's own range decides.
    // (Bytes of mask positions keep their forced value: blanking them could hit a tag in one run only.)
    static std::vector<std::uint8_t> masked;
    masked.assign(kDataSize, 0);
    for (const std::uint32_t o : mask)
        if (o + 4 <= kDataSize) std::memset(masked.data() + o, 1, 4);
    auto blank_window = [&](std::uint32_t o) {
        for (std::uint32_t k = 0; k < 4; ++k)
            if (!masked[o + k]) dst[o + k] = 0;
    };
    if (blank) {
        for (const std::uint32_t o : *blank)
            if (o + 4 <= kDataSize) blank_window(o);
    } else {
        for (std::uint32_t o = 0; o + 4 <= kDataSize; ++o) {
            std::uint32_t v = 0;
            std::memcpy(&v, image + o, 4);
            if (v >= g_exe_begin && v < g_exe_end && heuristic_window(o)) blank_window(o);
        }
    }
    for (const Range& r : g_opt.skip) {
        const std::uint32_t b = std::max(r.begin, kDataVa), e = std::min(r.end, kDataEnd);
        for (std::uint32_t va = b & ~3u; va < e; va += 4) out[(va - kDataVa) / 4] = 0;
    }
}

#if JOAN_IMAGE_X64
const std::uint8_t* image_bytes() { return x86_image(); }
#else
const std::uint8_t* image_bytes() { return reinterpret_cast<const std::uint8_t*>(img::addr(img::Addr(kDataVa))); }
#endif

// The CRT heap (_crtheap, the private heap of the ported CRT, src/lib/crt/heap.cpp) as a multiset of blocks:
// order-independent (heap layout differs between runs), each block's normalised words hashed.
std::uint64_t heap_hash() {
#if !defined(_WIN32)
    return 0;  // the CRT heap walk (HeapLock / HeapWalk) is Windows-only; sys::heap_hash_supported() says so
#else
    const auto heap = reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(img::ref(d::g_crtHeap)[0]));
    if (!heap || det_heap::active()) return 0;  // (ReplayHashHeap with the deterministic heap: not supported)
    std::uint64_t sum = 0, mix = 0, count = 0;
    if (!HeapLock(heap)) return 0;
    PROCESS_HEAP_ENTRY e;
    e.lpData = nullptr;
    std::vector<std::uint32_t> words;
    while (HeapWalk(heap, &e)) {
        if (!(e.wFlags & PROCESS_HEAP_ENTRY_BUSY)) continue;
        const auto* w = static_cast<const std::uint32_t*>(e.lpData);
        const std::uint32_t n = static_cast<std::uint32_t>(e.cbData / 4);
        words.resize(n);
        for (std::uint32_t i = 0; i < n; ++i) words[i] = normalise(w[i]);
        const std::uint64_t h = hash_words(words.data(), n, 0xcbf29ce484222325ull ^ e.cbData);
        sum += h;
        mix ^= h * 0x9e3779b97f4a7c15ull;
        ++count;
    }
    HeapUnlock(heap);
    const std::uint64_t parts[3] = {sum, mix, count};
    return hash_bytes(parts, sizeof(parts));
#endif
}

// The back buffer, sampled on a 64 x 48 grid (Direct3D 8 LockRect read-only; formats of 16 / 24 / 32 bits).
std::uint64_t back_buffer_hash(void* device) {
    auto* dev = static_cast<d3d8::IDirect3DDevice8*>(device);
    d3d8::IDirect3DSurface8* surf = nullptr;
    if (dev->GetBackBuffer(0, 0, &surf) != 0 || !surf) return 0;
    std::uint64_t h = 0;
    d3d8::SurfaceDesc desc{};
    d3d8::LockedRect lr{};
    if (surf->GetDesc(&desc) == 0 && surf->LockRect(&lr, nullptr, 0x10 /* D3DLOCK_READONLY */) == 0) {
        std::uint32_t bpp = 0;
        if (desc.format == d3d8::kFmtA8R8G8B8 || desc.format == d3d8::kFmtX8R8G8B8) bpp = 4;
        if (desc.format == d3d8::kFmtR8G8B8) bpp = 3;
        if (desc.format == d3d8::kFmtR5G6B5 || desc.format == d3d8::kFmtX1R5G5B5 || desc.format == d3d8::kFmtA1R5G5B5) bpp = 2;
        h = hash_words(&desc.width, 2);
        if (bpp && lr.bits && desc.width && desc.height) {
            for (std::uint32_t gy = 0; gy < 48; ++gy) {
                const std::uint32_t y = (2 * gy + 1) * desc.height / 96;
                const auto* row = static_cast<const std::uint8_t*>(lr.bits) + static_cast<std::ptrdiff_t>(y) * lr.pitch;
                for (std::uint32_t gx = 0; gx < 64; ++gx) {
                    const std::uint32_t x = (2 * gx + 1) * desc.width / 128;
                    std::uint32_t px = 0;
                    std::memcpy(&px, row + x * bpp, bpp);
                    if (bpp == 4) px &= 0x00ffffffu;  // X8 / alpha of the back buffer is not part of the picture
                    h = hash_words(&px, 1, h);
                }
            }
        }
        surf->UnlockRect();
    }
    surf->Release();
    return h;
}

std::string where(std::uint32_t va) {
    std::uint32_t start = 0;
    const char* name = data_image::symbol_at(va, &start);
    char buf[160];
    if (name)
        std::snprintf(buf, sizeof(buf), "0x%08x %s+0x%x", static_cast<unsigned>(va), name,
                      static_cast<unsigned>(va - start));
    else
        std::snprintf(buf, sizeof(buf), "0x%08x", static_cast<unsigned>(va));
    return buf;
}

// Typed scheme, recording: fields whose type and value disagree in this run, reported once each (the hash is only as
// exact as the types: a pointer typed as a number is compared raw and differs between runs, a number typed as a
// pointer normalises by address range). Data typed as a number that holds an address of the data image or of the
// deterministic heap (4-aligned, changed since install), and pointer fields holding a value that is no address of
// this run (not mapped, not a handle).
std::vector<std::uint8_t> g_type_reported;  // per byte offset: reported already
int g_type_reports = 0;

void type_check(const std::uint8_t* image) {
    constexpr int kMaxReports = 32;
    if (!g_typed || g_type_reports >= kMaxReports || g_pristine.size() != kDataSize / 4) return;
    if (g_type_reported.size() != kDataSize) g_type_reported.assign(kDataSize, 0);
    const auto* pristine = reinterpret_cast<const std::uint8_t*>(g_pristine.data());
    const std::uint32_t image_base = static_cast<std::uint32_t>(img::base());
    const std::uint32_t heap = det_heap::active() ? static_cast<std::uint32_t>(det_heap::base()) : 0;
    const std::uint32_t heap_used = heap ? static_cast<std::uint32_t>(det_heap::used_end() - det_heap::base()) : 0;
    for (std::uint32_t o = 0; o + 4 <= kDataSize && g_type_reports < kMaxReports; ++o) {
        if (g_class[o] != typed::kTyped || g_class[o + 1] != typed::kTyped || g_class[o + 2] != typed::kTyped ||
            g_class[o + 3] != typed::kTyped)
            continue;
        std::uint32_t v = 0;
        std::memcpy(&v, image + o, 4);
        if ((v & 3) != 0 || v == 0 || g_type_reported[o]) continue;
        const bool in_image = v - image_base < img::kSize, in_heap = heap && v - heap < heap_used;
        if ((!in_image && !in_heap) || std::memcmp(image + o, pristine + o, 4) == 0) continue;
        g_type_reported[o] = 1;
        ++g_type_reports;
        report_line("type check: %s is typed as data but holds 0x%08x, an address of this run (%s)",
                    where(kDataVa + o).c_str(), static_cast<unsigned>(v), in_image ? "data image" : "heap");
    }
    for (const std::uint32_t o : g_table.pointers) {
        if (g_type_reports >= kMaxReports) break;
        if (o + 4 > kDataSize || g_type_reported[o]) continue;
        std::uint32_t v = 0;
        std::memcpy(&v, image + o, 4);
        if (v == 0 || v == 0xffffffffu || v == recorded_bits(g_window) ||
            g_handles.count(v) || mapped(v))
            continue;
        g_type_reported[o] = 1;
        ++g_type_reports;
        report_line("type check: pointer field %s holds 0x%08x, no address of this run", where(kDataVa + o).c_str(),
                    static_cast<unsigned>(v));
    }
    if (g_type_reports >= kMaxReports) report_line("type check: %d reports, no more", kMaxReports);
}

// ---------------------------------------------------------------- reports

const char* mode_name() {
    switch (g_mode.load()) {
        case Mode::Record: return "record";
        case Mode::Replay: return "replay";
        default: return "off";
    }
}

std::string describe(const Event& e) {
    char buf[200];
    switch (e.type) {
        case Ev::Call:
            std::snprintf(buf, sizeof(buf), "call %s (check %08x)", id_name(e.id), static_cast<unsigned>(e.check));
            break;
        case Ev::WndProc:
            std::snprintf(buf, sizeof(buf), "window message 0x%x (wparam 0x%x)", static_cast<unsigned>(e.a),
                          static_cast<unsigned>(e.b));
            break;
        case Ev::Apc:
            std::snprintf(buf, sizeof(buf), "I/O completion of request %u", static_cast<unsigned>(e.a));
            break;
        case Ev::Frame:
            std::snprintf(buf, sizeof(buf), "Present of frame %u", static_cast<unsigned>(e.frame.frame));
            break;
        case Ev::End:
            std::snprintf(buf, sizeof(buf), "the game's exit after %u frames", static_cast<unsigned>(e.a));
            break;
        default:
            std::snprintf(buf, sizeof(buf), "%s event", event_name(e.type));
    }
    return buf;
}

void remember(Ev type, std::uint16_t id) {
    g_recent[g_recent_next % 16] = {g_frame, g_events, type, id};
    ++g_recent_next;
    ++g_events;
}

void write_report_file() {
    if (g_file.empty()) return;
    const std::string path = g_file + ".report.txt";
    if (std::FILE* f = std::fopen(path.c_str(), "w")) {
        for (const std::string& l : g_report) std::fprintf(f, "%s\n", l.c_str());
        std::fclose(f);
    }
}

void summary() {
    report_line("%s summary: %u frames, %llu events%s", mode_name(), static_cast<unsigned>(g_frame),
                static_cast<unsigned long long>(g_events), g_diverged ? ", DIVERGED" : "");
    if (recording())
        report_line("recording %s: %llu bytes", g_file.c_str(), static_cast<unsigned long long>(g_writer.bytes_written()));
    if (replaying() || g_diverged || g_mismatch_frames) {
        if (g_mismatch_frames)
            report_line("state hash mismatches in %u frames (image %u, heap %u, back buffer %u); first in frame %u",
                        static_cast<unsigned>(g_mismatch_frames), static_cast<unsigned>(g_image_mismatches),
                        static_cast<unsigned>(g_heap_mismatches), static_cast<unsigned>(g_bb_mismatches),
                        static_cast<unsigned>(g_first_mismatch));
        else if (!g_diverged)
            report_line("every frame hash matched the recording");
        if (g_warnings) report_line("%u warnings (see above)", static_cast<unsigned>(g_warnings));
    }
    for (int i = 0; i < static_cast<int>(Id::Count); ++i)
        if (const std::uint32_t n = g_other_threads[i].load())
            report_line("note: %u calls of %s on other threads were not recorded", static_cast<unsigned>(n),
                        id_name(static_cast<std::uint16_t>(i)));
}

UINT result_code() {
    if (g_diverged) return 3;
    if (g_mismatch_frames) return 4;
    return 0;
}

// Ends recording / replay (once): summary, files closed, report written.
void finish() {
    if (g_finished) return;
    g_finished = true;
    if (det_heap::active()) det_heap::report("at the end");
    summary();
    if (g_writer.is_open()) g_writer.close();
    write_report_file();
}

// "module+offset  function+displacement" for a code address (dbghelp names when openjoan.pdb is next to the exe on
// Windows, dladdr elsewhere: sys.hpp).
std::string describe_frame(std::uintptr_t a) { return sys::describe_frame(a); }

void report_stack(const char* title) {
    void* frames[32] = {};
    const int n = sys::capture_stack(1, frames, 32);
    report_line("%s:", title);
    for (int i = 0; i < n; ++i) report_line("  %s", describe_frame(reinterpret_cast<std::uintptr_t>(frames[i])).c_str());
}

// ---------------------------------------------------------------- write watch ([Debug] ReplayWatch)
// ReplayWatch=0x00a5d60c,0x00a538bc (up to 4 original data addresses, 4 bytes each): hardware data breakpoints on the
// main thread (debug registers); every write logs the frame, the new value and the writer's call stack (dbghelp
// StackWalk64, so frames without a frame pointer unwind too) to the report, at most ReplayWatchHits (default 48)
// times. For finding the code that stores a value the state hash reports as different.
[[maybe_unused]] std::uint32_t g_watch_va[4] = {};
[[maybe_unused]] std::uintptr_t g_watch_at[4] = {};
[[maybe_unused]] int g_watch_n = 0;
[[maybe_unused]] std::atomic<int> g_watch_left{0};

#if defined(_WIN32)

void watch_stack(const CONTEXT* ctx) {
    using StackWalk64Fn = BOOL(WINAPI*)(DWORD, HANDLE, HANDLE, LPSTACKFRAME64, PVOID, PREAD_PROCESS_MEMORY_ROUTINE64,
                                        PFUNCTION_TABLE_ACCESS_ROUTINE64, PGET_MODULE_BASE_ROUTINE64,
                                        PTRANSLATE_ADDRESS_ROUTINE64);
    static HMODULE dbghelp = LoadLibraryA("dbghelp.dll");
    static auto walk = dbghelp ? reinterpret_cast<StackWalk64Fn>(GetProcAddress(dbghelp, "StackWalk64")) : nullptr;
    static auto table = dbghelp ? reinterpret_cast<PFUNCTION_TABLE_ACCESS_ROUTINE64>(GetProcAddress(dbghelp, "SymFunctionTableAccess64")) : nullptr;
    static auto modbase = dbghelp ? reinterpret_cast<PGET_MODULE_BASE_ROUTINE64>(GetProcAddress(dbghelp, "SymGetModuleBase64")) : nullptr;
#if defined(_M_X64) || defined(__x86_64__)
    constexpr DWORD kMachine = IMAGE_FILE_MACHINE_AMD64;
    const DWORD64 pc = ctx->Rip, fp = ctx->Rbp, sp = ctx->Rsp;
#else
    constexpr DWORD kMachine = IMAGE_FILE_MACHINE_I386;
    const DWORD64 pc = ctx->Eip, fp = ctx->Ebp, sp = ctx->Esp;
#endif
    report_line("    at %s", describe_frame(static_cast<std::uintptr_t>(pc)).c_str());  // also initialises dbghelp's symbols
    if (!walk || !table || !modbase) return;
    CONTEXT c = *ctx;
    STACKFRAME64 f{};
    f.AddrPC.Offset = pc;
    f.AddrPC.Mode = AddrModeFlat;
    f.AddrFrame.Offset = fp;
    f.AddrFrame.Mode = AddrModeFlat;
    f.AddrStack.Offset = sp;
    f.AddrStack.Mode = AddrModeFlat;
    for (int i = 0; i < 12; ++i) {
        if (!walk(kMachine, GetCurrentProcess(), GetCurrentThread(), &f, &c, nullptr, table, modbase, nullptr)) break;
        if (f.AddrReturn.Offset == 0) break;
        report_line("    from %s", describe_frame(static_cast<std::uintptr_t>(f.AddrReturn.Offset)).c_str());
    }
}

LONG CALLBACK on_watch(EXCEPTION_POINTERS* info) {
    if (info->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP) return EXCEPTION_CONTINUE_SEARCH;
    CONTEXT* const ctx = info->ContextRecord;
    const auto hit = static_cast<DWORD>(ctx->Dr6 & 0xf);
    if (hit == 0) return EXCEPTION_CONTINUE_SEARCH;  // a single step of someone else's
    ctx->Dr6 = 0;
    for (int i = 0; i < g_watch_n; ++i) {
        if (!(hit & (1u << i))) continue;
        if (g_watch_left.fetch_sub(1) <= 0) {
            ctx->Dr7 &= ~(decltype(ctx->Dr7){3} << (i * 2));  // enough: this watch is off
            continue;
        }
        std::uint32_t v = 0;
        std::memcpy(&v, reinterpret_cast<const void*>(g_watch_at[i]), 4);
        report_line("watch 0x%08x: frame %u, written 0x%08x", static_cast<unsigned>(g_watch_va[i]),
                    static_cast<unsigned>(g_frame), static_cast<unsigned>(v));
        watch_stack(ctx);
    }
    return EXCEPTION_CONTINUE_EXECUTION;
}

DWORD WINAPI set_watch_registers(void* main_thread) {
    const HANDLE t = static_cast<HANDLE>(main_thread);
    if (SuspendThread(t) == static_cast<DWORD>(-1)) return 1;
    CONTEXT c{};
    c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    if (GetThreadContext(t, &c)) {
        using DebugRegister = decltype(c.Dr0);  // DWORD on x86, DWORD64 on x64
        DebugRegister* const dr[4] = {&c.Dr0, &c.Dr1, &c.Dr2, &c.Dr3};
        for (int i = 0; i < g_watch_n; ++i) {
            *dr[i] = static_cast<DebugRegister>(g_watch_at[i] & ~std::uintptr_t{3});
            c.Dr7 |= DebugRegister{1} << (i * 2);       // local enable
            c.Dr7 |= DebugRegister{1} << (16 + i * 4);  // R/W = 01: data writes
            c.Dr7 |= DebugRegister{3} << (18 + i * 4);  // LEN = 11: 4 bytes
        }
        SetThreadContext(t, &c);
    }
    ResumeThread(t);
    return 0;
}

void install_watch() {
    const std::string spec = ini_string("ReplayWatch");
    if (spec.empty()) return;
    std::size_t pos = 0;
    while (pos < spec.size() && g_watch_n < 4) {
        std::size_t end = spec.find(',', pos);
        if (end == std::string::npos) end = spec.size();
        const std::uint32_t va = static_cast<std::uint32_t>(std::strtoul(spec.substr(pos, end - pos).c_str(), nullptr, 0));
        pos = end + 1;
        if (va < kDataVa || va + 4 > kDataEnd) {
            report_line("ReplayWatch: 0x%08x is not in the data image - ignored", static_cast<unsigned>(va));
            continue;
        }
        g_watch_va[g_watch_n] = va;
        g_watch_at[g_watch_n] = img::addr(img::Addr(va));
        ++g_watch_n;
    }
    if (g_watch_n == 0) return;
    g_watch_left = std::max(1, ini_int("ReplayWatchHits", 48));
    AddVectoredExceptionHandler(1, on_watch);
    HANDLE self = nullptr;
    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &self, THREAD_ALL_ACCESS, FALSE, 0);
    const HANDLE helper = CreateThread(nullptr, 0, set_watch_registers, self, 0, nullptr);
    if (helper) {
        WaitForSingleObject(helper, INFINITE);
        CloseHandle(helper);
    }
    CloseHandle(self);
    for (int i = 0; i < g_watch_n; ++i)
        report_line("ReplayWatch: data breakpoint on writes to 0x%08x (main thread)", static_cast<unsigned>(g_watch_va[i]));
}
#else
void install_watch() {
    if (!ini_string("ReplayWatch").empty())
        report_line("ReplayWatch: hardware write watchpoints are only built on Windows - ignored");
}
#endif

// Heap trace ([Debug] ReplayHeapTrace=N, default 4000): the first N allocations / releases of the game's heap with
// their callers, to <recording>.heap-record.txt / .heap-replay.txt - the first line where the two files differ is
// where the runs stopped making the same requests (the cause of heap pointer offsets that differ).
std::FILE* g_heap_trace = nullptr;
std::uint32_t g_dump_page = 0;   // [Debug] ReplayDumpPage (original address of a page of the hashed range)
std::FILE* g_dump_page_file = nullptr;
// [Debug] ReplayDumpImage=PATH + ReplayDumpImageEvery=N / ReplayDumpImageFrom=F: the normalised hashed words of every N-th
// frame from F on (u32 frame, kDataSize bytes), for diffing an x86 and an x64 replay of the same recording word by word
// (Phase 2 step 4: tools/replay_image_diff.py names the differing words).
std::FILE* g_dump_image_file = nullptr;
std::uint32_t g_dump_image_every = 0, g_dump_image_from = 0;
std::uint32_t g_heap_trace_left = 0, g_heap_trace_index = 0;

void heap_trace(std::size_t n, std::uintptr_t offset) {
    if (!g_heap_trace || g_heap_trace_left == 0 || !on_main()) return;
    --g_heap_trace_left;
    void* frames[8] = {};
    const int k = sys::capture_stack(3, frames, 8);
    if (n == static_cast<std::size_t>(-1))
        std::fprintf(g_heap_trace, "#%u frame %u free  @%06x", g_heap_trace_index++, static_cast<unsigned>(g_frame),
                     static_cast<unsigned>(offset));
    else
        std::fprintf(g_heap_trace, "#%u frame %u alloc %u @%06x", g_heap_trace_index++, static_cast<unsigned>(g_frame),
                     static_cast<unsigned>(n), static_cast<unsigned>(offset));
    for (int i = 0; i < k && i < 4; ++i)
        std::fprintf(g_heap_trace, " | %s", describe_frame(reinterpret_cast<std::uintptr_t>(frames[i])).c_str());
    std::fputc('\n', g_heap_trace);
    if (g_heap_trace_left == 0) {
        std::fclose(g_heap_trace);
        g_heap_trace = nullptr;
    }
}

void divergence(const std::string& what) {
    g_diverged = true;
    report_line("DIVERGENCE in frame %u after %llu replayed events: %s", static_cast<unsigned>(g_frame),
                static_cast<unsigned long long>(g_events), what.c_str());
    const std::size_t n = std::min<std::size_t>(g_recent_next, 16);
    if (n) report_line("the last %u events that matched:", static_cast<unsigned>(n));
    for (std::size_t k = g_recent_next - n; k < g_recent_next; ++k) {
        const Recent& r = g_recent[k % 16];
        report_line("  #%llu frame %u: %s", static_cast<unsigned long long>(r.index), static_cast<unsigned>(r.frame),
                    r.type == Ev::Call ? id_name(r.id) : event_name(r.type));
    }
    report_stack("the game's call stack at the divergence");
    if (g_opt.stop_on_divergence) {
        report_line("stopping (ReplayOnDivergence=stop)");
        exit_now(3);
    }
    report_line("continuing live: the game now gets real inputs (ReplayOnDivergence=continue)");
    finish();
    g_mode = Mode::Off;
}

void end_of_recording(const char* next_call) {
    if (const char* err = g_reader.error()) {
        report_line("the recording is damaged at offset %u: %s", static_cast<unsigned>(g_reader.position()), err);
        g_diverged = true;
    } else {
        report_line("the recording ends here (frame %u, before %s) - it was cut short (crash or kill while recording)",
                    static_cast<unsigned>(g_frame), next_call);
    }
    if (g_opt.exit_at_end) exit_now(result_code());
    finish();
    g_mode = Mode::Off;
}

void inject_wndproc(std::uint32_t msg, std::uint32_t wp, std::uint32_t lp) {
    remember(Ev::WndProc, 0);
    if (g_game_proc) g_game_proc(g_window, msg, static_cast<WPARAM>(wp), static_cast<LPARAM>(lp));
}

// Replay: injects recorded window messages / completions until the next event of another kind, which is returned
// (not consumed). False when replay ended (end of the recording) - the caller then goes on live.
bool next_event(Event& e, const char* for_what) {
    if (g_peer.open() && g_opt.follow.empty() && g_reader.position() - g_forwarded >= kForwardChunk) forward_used();
    for (;;) {
        if (g_mode != Mode::Replay) return false;
        if (!g_reader.peek(e)) {
            end_of_recording(for_what);
            return false;
        }
        switch (e.type) {
            case Ev::WndProc:
                g_reader.next();
                inject_wndproc(e.a, e.b, e.c);
                continue;
            case Ev::Apc:
                g_reader.next();
                remember(Ev::Apc, 0);
                inject_apc(e.a, e.b, e.c);
                continue;
            case Ev::Snapshot:
            case Ev::Env:
                g_reader.next();  // a snapshot without its frame (or a late environment block): nothing to compare
                continue;
            default:
                return true;
        }
    }
}

// This run's normalised image of the previous frame and the pages that mismatched in it (replay): without a snapshot
// of the recording, the words this run changed in a mismatching page (and changed again in the next frame) are the
// candidates for the differing word ([Debug] ReplayWatch on them shows the writers).
std::vector<std::uint32_t> g_prev_words;
std::vector<std::uint32_t> g_prev_bad_pages;
unsigned g_change_reports = 0;

void report_changes(const char* when, std::uint32_t page, const std::vector<std::uint32_t>& words) {
    if (g_prev_words.size() != words.size()) return;
    const std::size_t begin = static_cast<std::size_t>(page) * (kPageSize / 4);
    const std::size_t end = std::min(words.size(), begin + kPageSize / 4);
    int n = 0;
    for (std::size_t i = begin; i < end; ++i) {
        if (words[i] == g_prev_words[i]) continue;
        if (n++ < 24)
            report_line("    %s: %s 0x%08x -> 0x%08x", when, where(static_cast<std::uint32_t>(kDataVa + i * 4)).c_str(),
                        static_cast<unsigned>(g_prev_words[i]), static_cast<unsigned>(words[i]));
    }
    if (n == 0) report_line("    %s: no word of page 0x%08x changed in this run", when,
                            static_cast<unsigned>(kDataVa + page * kPageSize));
    else if (n > 24) report_line("    %s: ... %d words changed in all", when, n);
}

void compare_frame(const FrameHash& rec, const FrameHash& now, const std::vector<std::uint32_t>& words) {
    if (!g_prev_bad_pages.empty() && g_change_reports < 40) {
        ++g_change_reports;
        report_line("  frame %u, this run's changes in the pages that differed in the frame before:",
                    static_cast<unsigned>(now.frame));
        for (const std::uint32_t p : g_prev_bad_pages) report_changes("next frame", p, words);
    }
    g_prev_bad_pages.clear();
    struct Remember {
        const std::vector<std::uint32_t>& w;
        ~Remember() { g_prev_words = w; }
    } remember_words{words};
    if (rec.frame != now.frame)
        warn("frame numbering: the recording's frame %u is this run's frame %u", static_cast<unsigned>(rec.frame),
             static_cast<unsigned>(now.frame));
    const bool image = rec.image != now.image;
    const bool heap = (g_header.flags & kFlagHeap) && rec.heap != now.heap;
    const bool bb = (g_header.flags & kFlagBackBuffer) && rec.back_buffer != now.back_buffer;
    if (!image && !heap && !bb) return;
    ++g_mismatch_frames;
    g_image_mismatches += image ? 1u : 0u;
    g_heap_mismatches += heap ? 1u : 0u;
    g_bb_mismatches += bb ? 1u : 0u;
    const bool first = g_first_mismatch == 0xffffffffu;
    if (first) g_first_mismatch = now.frame;
    // Detailed for the first mismatching frame, and for every frame that has a snapshot (ReplaySnapshotEvery).
    const bool have_snapshot = g_snapshot_frame == now.frame && g_snapshot.size() == words.size();
    if (!first && !have_snapshot) {
        if (g_mismatch_frames <= 20)
            report_line("frame %u: state hash mismatch (%s%s%s)", static_cast<unsigned>(now.frame), image ? "image " : "",
                        heap ? "heap " : "", bb ? "back-buffer" : "");
        if (image)
            for (std::size_t p = 0; p < rec.pages.size() && p < now.pages.size(); ++p) {
                if (rec.pages[p] == now.pages[p]) continue;
                g_prev_bad_pages.push_back(static_cast<std::uint32_t>(p));
                if (g_change_reports < 40) {
                    ++g_change_reports;
                    report_line("  page 0x%08x differs; this run's changes since the frame before:",
                                static_cast<unsigned>(kDataVa + p * kPageSize));
                    report_changes("this frame", static_cast<std::uint32_t>(p), words);
                }
            }
        return;
    }
    report_line("frame %u: STATE HASH MISMATCH%s%s%s", static_cast<unsigned>(now.frame), image ? " - image .data/.bss" : "",
                heap ? " - CRT heap" : "", bb ? " - back buffer" : "");
    if (image) {
        int shown = 0;
        for (std::size_t p = 0; p < rec.pages.size() && p < now.pages.size(); ++p) {
            if (rec.pages[p] == now.pages[p]) continue;
            if (shown++ < 16) {
                const auto va = static_cast<std::uint32_t>(kDataVa + p * kPageSize);
                report_line("  page 0x%08x..0x%08x differs (begins in %s)", static_cast<unsigned>(va),
                            static_cast<unsigned>(va + kPageSize), where(va).c_str());
            }
        }
        if (shown > 16) report_line("  ... %d pages differ in all", shown);
        if (have_snapshot) {
            int diffs = 0;
            for (std::size_t i = 0; i < words.size(); ++i) {
                if (words[i] == g_snapshot[i]) continue;
                if (diffs++ < (g_snapshot_frame == 0 ? 400 : 48))
                    report_line("  %s: recorded 0x%08x, now 0x%08x",
                                where(static_cast<std::uint32_t>(kDataVa + i * 4)).c_str(),
                                static_cast<unsigned>(g_snapshot[i]), static_cast<unsigned>(words[i]));
            }
            report_line("  %d words differ (normalised: image pointers as original addresses, other pointers 0x%08x, "
                        "handles 0x%08x)", diffs, static_cast<unsigned>(kTagPointer), static_cast<unsigned>(kTagHandle));
        } else {
            report_line("  (no snapshot of this frame: record with ReplaySnapshotEvery=1 to see the differing words)");
            for (std::size_t p = 0; p < rec.pages.size() && p < now.pages.size(); ++p) {
                if (rec.pages[p] == now.pages[p]) continue;
                g_prev_bad_pages.push_back(static_cast<std::uint32_t>(p));
                if (g_change_reports < 40) {
                    ++g_change_reports;
                    report_line("  page 0x%08x: this run's changes since the frame before:",
                                static_cast<unsigned>(kDataVa + p * kPageSize));
                    report_changes("this frame", static_cast<std::uint32_t>(p), words);
                }
            }
        }
    }
}

}  // namespace

// ---------------------------------------------------------------- internal API

Mode mode() { return g_mode.load(std::memory_order_relaxed); }

bool on_main() { return g_mode.load(std::memory_order_relaxed) != Mode::Off && sys::thread_id() == g_thread; }

void other_thread(Id id) {
    if (g_mode.load(std::memory_order_relaxed) != Mode::Off)
        g_other_threads[static_cast<int>(id)].fetch_add(1, std::memory_order_relaxed);
}

void log(const char* fmt, ...) {
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    hook::log_line("replay: %s", buf);
}

void warn(const char* fmt, ...) {
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    ++g_warnings;
    if (g_warnings <= 200) report_line("warning (frame %u): %s", static_cast<unsigned>(g_frame), buf);
}

// The writer may make system calls (a full buffer): the game's last-error value must survive them.
void record_call(Id id, std::uint32_t check, const void* payload, std::uint32_t n) {
    if (g_mode != Mode::Record) return;
    const sys::LastErrorGuard err;
    g_writer.call(static_cast<std::uint16_t>(id), check, payload, n);
    remember(Ev::Call, static_cast<std::uint16_t>(id));
}

void record_wndproc(std::uint32_t msg, std::uint32_t wparam, std::uint32_t lparam) {
    if (g_mode != Mode::Record) return;
    const sys::LastErrorGuard err;
    g_writer.wndproc(msg, wparam, lparam);
    remember(Ev::WndProc, 0);
}

void record_apc(std::uint32_t seq, std::uint32_t error, std::uint32_t bytes) {
    if (g_mode != Mode::Record) return;
    const sys::LastErrorGuard err;
    g_writer.apc(seq, error, bytes);
    remember(Ev::Apc, 0);
}

const std::uint8_t* play_raw(Id id, std::uint32_t check, std::uint32_t& n) {
    if (g_mode != Mode::Replay) return nullptr;
    Event e;
    if (!next_event(e, id_name(static_cast<std::uint16_t>(id)))) return nullptr;
    if (e.type != Ev::Call || e.id != static_cast<std::uint16_t>(id) || e.check != check) {
        char buf[300];
        std::snprintf(buf, sizeof(buf), "the game called %s (check %08x), the recording has %s at offset %u",
                      id_name(static_cast<std::uint16_t>(id)), static_cast<unsigned>(check), describe(e).c_str(),
                      static_cast<unsigned>(e.offset));
        divergence(buf);
        return nullptr;
    }
    g_reader.next();
    remember(Ev::Call, e.id);
    n = e.size;
    return e.payload;
}

bool payload_size_mismatch(Id id, std::uint32_t recorded, std::uint32_t expected) {
    char buf[200];
    std::snprintf(buf, sizeof(buf), "%s: the recording's result has %u bytes, this build expects %u (recorded by another "
                  "build of the replay code?)", id_name(static_cast<std::uint16_t>(id)), static_cast<unsigned>(recorded),
                  static_cast<unsigned>(expected));
    divergence(buf);
    return false;
}

void diverge(Id id, const char* what) {
    if (g_mode != Mode::Replay) return;
    divergence(std::string(id_name(static_cast<std::uint16_t>(id))) + ": " + what);
}

HWND game_window() { return g_window; }
void set_game_window(HWND h) { g_window = h; }
WNDPROC game_wndproc() { return g_game_proc; }
void set_game_wndproc(WNDPROC p) { g_game_proc = p; }

bool relevant_message(UINT msg, WPARAM wp) {
    switch (msg) {
        case WM_CREATE:
        case WM_DESTROY:
        case WM_PAINT:
        case WM_CLOSE:
        case WM_ACTIVATEAPP:
        case WM_INPUTLANGCHANGEREQUEST:
            return true;
        case WM_SYSCOMMAND:
            return (wp & 0xfff0) == 0xf170;  // Window::proc 0x00603e20 minimises the window on it
        default:
            return false;
    }
}

// Record: the relevant messages are logged and handled by the game. Replay: the game sees only injected messages
// (inject_wndproc calls its procedure directly); what Windows sends now goes to DefWindowProc - except a close
// request, which would destroy the window under the replay.
LRESULT CALLBACK wndproc_proxy(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    const WNDPROC game = g_game_proc;
    const Mode m = g_mode.load(std::memory_order_relaxed);
    if (m == Mode::Replay) {
        if (msg == WM_CLOSE) return 0;
        return real_def_window_proc(h, msg, wp, lp);
    }
    if (m == Mode::Record && sys::thread_id() == g_thread && relevant_message(msg, wp))
        record_wndproc(msg, static_cast<std::uint32_t>(wp), msg == WM_SYSCOMMAND ? static_cast<std::uint32_t>(lp) : 0u);
    return game ? game(h, msg, wp, lp) : real_def_window_proc(h, msg, wp, lp);
}

std::uint32_t hwnd_code(HWND h) {
    if (!h) return 0;
    if (h == g_window) return 1;
    return recorded_bits(h);
}
HWND hwnd_from_code(std::uint32_t code) {
    if (code == 0) return nullptr;
    if (code == 1) return g_window;
    return reinterpret_cast<HWND>(static_cast<std::uintptr_t>(code));
}

void track_handle(const void* h) {
    const auto v = recorded_bits(h);
    if (v != 0 && v != 0xffffffffu) g_handles.insert(v);
}
void untrack_handle(const void* h) { g_handles.erase(recorded_bits(h)); }

bool hash_files() { return g_opt.files != 0; }

bool wants_deterministic_heap() { return g_mode != Mode::Off && g_opt.det_heap; }

void pace_to(std::uint32_t recorded_ms) {
    if (!g_opt.pace) return;
    static bool started = false;
    static std::uint32_t t0 = 0;
    static unsigned long long r0 = 0;
    const unsigned long long now = sys::tick_ms();
    if (!started) {
        started = true;
        t0 = recorded_ms;
        r0 = now;
        return;
    }
    const std::uint32_t game = recorded_ms - t0;  // wraps like timeGetTime
    const unsigned long long real = now - r0;
    if (game > real + 2) sys::sleep_ms(static_cast<unsigned>(std::min<unsigned long long>(game - real, 100)));
    else if (real > game + 1000) r0 = now - game;  // far behind (a slow stretch): do not rush to catch up
}

// Size plus the first and last 64 KB: reading whole archives / music / movies on their first open made the
// recording stutter while a mission loads (patch 137 report: lags every 2-3 s for the first 15-20 s).
std::uint64_t quick_file_hash(const char* path, std::uint64_t* size) { return sys::quick_hash_file(path, size); }

std::uint64_t file_content_hash(const char* path, std::uint64_t* size) {
    *size = 0;
    if (!g_opt.files || !path) return 0;
    const std::string key = lower(path);
    const auto it = g_file_hashes.find(key);
    if (it != g_file_hashes.end()) {
        *size = it->second.first;
        return it->second.second;
    }
    const sys::LastErrorGuard err;
    std::uint64_t n = 0;
    const std::uint64_t h = g_opt.files >= 2 ? hash_file(path, &n) : quick_file_hash(path, &n);
    g_file_hashes[key] = {n, h};
    *size = n;
    return h;
}

std::uint32_t frame_number() { return g_frame; }

void frame_boundary(void* device) {
    if (!on_main()) return;
    const sys::LastErrorGuard err;
    const std::uint32_t flags = g_header.flags;
    FrameHash now;
    now.frame = g_frame;
    std::vector<std::uint32_t> words;
    if (recording()) {
        if (!g_blank_written) {
            const Layout l = this_layout();
            std::string text = kLayoutTag;
            text.append(reinterpret_cast<const char*>(&l), kLayoutWritten);
            g_writer.env(text);
        }
        classify(image_bytes(), g_new_mask);
        write_delta(kMaskTag, g_mask, g_new_mask, false);
        g_mask.swap(g_new_mask);
        {
            // raw values of the mask positions: written when a position is new or its bytes changed
            static std::string text;
            text = kRawTag;
            const std::uint8_t* img = image_bytes();
            g_rec_raw_next.clear();
            std::size_t r = 0;
            for (const std::uint32_t o : g_mask) {
                if (o + 4 > kDataSize) continue;
                std::uint32_t v = 0;
                std::memcpy(&v, img + o, 4);
                g_rec_raw_next.push_back({o, v});
                while (r < g_rec_raw.size() && g_rec_raw[r].o < o) ++r;
                if (r >= g_rec_raw.size() || g_rec_raw[r].o != o || g_rec_raw[r].v != v) {
                    text.append(reinterpret_cast<const char*>(&o), 4);
                    text.append(reinterpret_cast<const char*>(&v), 4);
                }
            }
            g_rec_raw.swap(g_rec_raw_next);
            if (text.size() > std::strlen(kRawTag)) g_writer.env(text);
        }
        type_check(image_bytes());
        exe_windows(image_bytes(), g_new_blank);
        write_delta(kBlankTag, g_blank, g_new_blank, !g_blank_written);
        g_blank_written = true;
        g_blank.swap(g_new_blank);
    } else if (replaying()) {
        Event m;
        auto tagged = [&m](const char* tag) {
            const std::size_t n = std::strlen(tag);
            return m.size >= n && std::memcmp(m.payload, tag, n) == 0;
        };
        while (g_reader.peek(m) && m.type == Ev::Env && (tagged(kMaskTag) || tagged(kBlankTag) || tagged(kLayoutTag) || tagged(kRawTag))) {
            g_reader.next();
            if (tagged(kRawTag)) {
                // updates come sorted by offset (the recording walks its sorted mask): merge, updates win
                const std::uint8_t* q = m.payload + std::strlen(kRawTag);
                const std::size_t count = (m.size - std::strlen(kRawTag)) / 8;
                g_rec_raw_next.clear();
                std::size_t r = 0;
                for (std::size_t i = 0; i < count; ++i, q += 8) {
                    RawAt u{};
                    std::memcpy(&u.o, q, 4);
                    std::memcpy(&u.v, q + 4, 4);
                    while (r < g_rec_raw.size() && g_rec_raw[r].o < u.o) g_rec_raw_next.push_back(g_rec_raw[r++]);
                    if (r < g_rec_raw.size() && g_rec_raw[r].o == u.o) ++r;
                    g_rec_raw_next.push_back(u);
                }
                while (r < g_rec_raw.size()) g_rec_raw_next.push_back(g_rec_raw[r++]);
                g_rec_raw.swap(g_rec_raw_next);
            } else if (tagged(kLayoutTag)) {
                if (m.size >= std::strlen(kLayoutTag) + kLayoutSizeX86) {
                    g_rec_layout = Layout{};
                    std::memcpy(&g_rec_layout, m.payload + std::strlen(kLayoutTag),
                                std::min(m.size - std::strlen(kLayoutTag), sizeof(Layout)));
                    g_have_rec_layout = true;
                }
            } else if (tagged(kMaskTag)) {
                apply_delta(g_mask, m.payload + std::strlen(kMaskTag), m.size - std::strlen(kMaskTag));
                g_have_recorded_mask = true;
            } else {
                apply_delta(g_blank, m.payload + std::strlen(kBlankTag), m.size - std::strlen(kBlankTag));
                g_have_recorded_blank = true;
            }
        }
        if (!g_have_recorded_mask) {  // a recording from before patch 141: this run's own guesses
            classify(image_bytes(), g_mask);
        }
    }
    masked_words(image_bytes(), g_mask, recording() || g_have_recorded_blank ? &g_blank : nullptr, words);
    // [Debug] ReplayDumpPage=0x00a52000: the raw words of that page of the hashed range, every frame, appended to
    // <recording>.pages.bin (u32 frame, 1024 x u32) - for diffing two runs word by word (diagnostics).
    if (g_dump_page) {
        const std::uint32_t off = g_dump_page - kDataVa;
        if (off + kPageSize <= kDataSize) {
            std::fwrite(&g_frame, 4, 1, g_dump_page_file);
            std::fwrite(image_bytes() + off, 1, kPageSize, g_dump_page_file);
        }
    }
    if (g_dump_image_file && g_frame >= g_dump_image_from && (g_frame - g_dump_image_from) % g_dump_image_every == 0) {
        std::fwrite(&g_frame, 4, 1, g_dump_image_file);
        std::fwrite(words.data(), 4, words.size(), g_dump_image_file);
    }
    now.image = hash_words(words.data(), words.size());
    now.pages.resize(kPages);
    for (std::uint32_t p = 0; p < kPages; ++p)
        now.pages[p] = fold(hash_words(words.data() + p * (kPageSize / 4), kPageSize / 4));
    if (flags & kFlagHeap) now.heap = heap_hash();
    if ((flags & kFlagBackBuffer) && device) now.back_buffer = back_buffer_hash(device);
    if (recording()) {
        g_writer.frame(now);
        if ((g_opt.snapshot_every && g_frame % g_opt.snapshot_every == 0) || g_frame < g_opt.snapshot_first)
            g_writer.snapshot(g_frame, words);
        g_writer.flush();
        remember(Ev::Frame, 0);
    } else if (replaying()) {
        Event e;
        if (next_event(e, "Present")) {
            if (e.type != Ev::Frame) {
                divergence("the game presents frame " + std::to_string(g_frame) + ", the recording has " + describe(e) +
                           " at offset " + std::to_string(e.offset));
            } else {
                g_reader.next();
                remember(Ev::Frame, 0);
                Event s;
                if (g_reader.peek(s) && s.type == Ev::Snapshot) {
                    g_reader.next();
                    g_snapshot = std::move(s.words);
                    g_snapshot_frame = s.a;
                }
                compare_frame(e.frame, now, words);
            }
        }
        forward_used();
    }
    if ((g_peer.open() || !g_opt.follow.empty()) && g_frame % 1000 == 0)
        log("[Coop] frame %u (%s)", static_cast<unsigned>(g_frame), g_opt.follow.empty() ? "host" : "player");
    ++g_frame;
}

void game_exit(UINT code) {
    if (g_mode == Mode::Record && sys::thread_id() == g_thread) {
        g_writer.end(g_frame);
        send_outgoing();  // the end reaches the player too
        report_line("the game exits (code %u) after %u frames", static_cast<unsigned>(code), static_cast<unsigned>(g_frame));
    } else if (g_mode == Mode::Replay) {
        Event e;
        if (next_event(e, "the game's exit")) {
            if (e.type == Ev::End) {
                g_reader.next();
                if (g_opt.follow.empty()) forward_used();  // a replay passed on: its end too
                report_line("the game exits where the recording ends (frame %u)", static_cast<unsigned>(g_frame));
            } else {
                g_diverged = true;
                report_line("DIVERGENCE: the game exits (code %u) in frame %u, the recording goes on with %s",
                            static_cast<unsigned>(code), static_cast<unsigned>(g_frame), describe(e).c_str());
            }
        }
    }
    put_user_saves_back();
    finish();
}

void exit_now(UINT code) {
    put_user_saves_back();
    finish();
    if (g_diverged && g_mode == Mode::Replay) {  // a stop on divergence is not a crash: say so
        const std::string text = "The replay stopped: the game did something the recording did not (frame " +
                                 std::to_string(g_frame) + ").\n\nDetails: " + g_file + ".report.txt";
        sys::replay_stopped_notice(text, g_window);
    }
    g_mode = Mode::Off;
    real_exit_process(code);
}

UINT replay_exit_code(UINT game_code) { return g_file.empty() || recording() ? game_code : result_code(); }

// ---------------------------------------------------------------- install

void install() {
    g_opt = read_options();
    if (g_opt.record.empty() && g_opt.replay.empty()) return;
    if (!g_opt.record.empty() && !g_opt.replay.empty()) {
        log("both Record and Replay are set - replaying (%s), not recording", g_opt.replay.c_str());
        g_opt.record.clear();
    }
    // The ranges left out of the image hash are part of the recording (a replay uses the recording's list).
    g_typed = !g_opt.record.empty() && g_opt.typed;
    const char* const default_skip = g_typed ? kDefaultSkipTyped : kDefaultSkip;
    std::string skip_text = default_skip;
    if (!g_opt.skip_text.empty()) skip_text += "," + g_opt.skip_text;
    std::string bad;
    if (!parse_ranges(skip_text, g_opt.skip, &bad)) {
        log("[Debug] ReplayHashSkip: cannot read \"%s\" - using the default list only", bad.c_str());
        g_opt.skip.clear();
        skip_text = default_skip;
        parse_ranges(skip_text, g_opt.skip);
    }
    g_thread = sys::thread_id();
    const bool uninit_fix = hook::settings().fix_uninit_reads;
    const bool trigger_sort_fix = hook::settings().fix_trigger_sort;
    const bool unit_stuck_fix = hook::settings().fix_unit_stuck;
    const bool unit_stuck_watch = unit_stuck_fix && hook::settings().fix_unit_stuck_watch;
    const bool immovable_siege_fix = hook::settings().fix_immovable_siege;
    const bool animal_pen_fix = hook::settings().fix_animal_pen;
    const bool mission_count_fix = hook::settings().fix_mission_count;

    if (g_opt.heap && !sys::heap_hash_supported()) {
        log("[Debug] ReplayHashHeap=1: the CRT heap hash is only built on Windows - off");
        g_opt.heap = false;
    }
    if (!g_opt.record.empty()) {
        g_file = absolute(g_opt.record);
        g_header.flags = (g_opt.back_buffer ? kFlagBackBuffer : 0u) | (g_opt.heap ? kFlagHeap : 0u) |
                         (uninit_fix ? kFlagUninitFix : 0u) | (g_typed ? kFlagTypedHash : 0u) |
                         (trigger_sort_fix ? kFlagTriggerSortFix : 0u) | (unit_stuck_fix ? kFlagUnitStuckFix : 0u) |
                         (unit_stuck_watch ? kFlagUnitStuckWatch : 0u) | (immovable_siege_fix ? kFlagImmovableSiege : 0u) |
                         (animal_pen_fix ? kFlagAnimalPen : 0u) |
                         (mission_count_fix ? kFlagMissionCount : 0u) | kFlagNoPlnCache | kFlagStableGroundOrder |
                         (hook::settings().fix_popup_buttons ? kFlagPopupButtons : 0u) |
                         (hook::settings().fix_ending_layout ? kFlagEndingLayout : 0u) |
                         (hook::settings().fix_wheel_zoom ? kFlagWheelZoom : 0u) |
                         (hook::settings().fix_mouse_options ? (static_cast<std::uint32_t>(hook::settings().cursor_sensitivity) + 1u) << kCursorSensitivityShift : 0u);
        pln_cache_off();
        g_stable_ground_order = true;
        g_header.version = g_typed ? kFormatVersion : kFormatVersionUntyped;
        if (g_typed) g_table = typed::builtin(kDataVa, kDataSize);
        g_header.data_va = kDataVa;
        g_header.data_size = kDataSize;
        g_header.page_size = kPageSize;
        g_header.exe_stamp = exe_timestamp();
        g_header.snapshot_every = g_opt.snapshot_every;
        if (!g_opt.stream.empty() && wait_for_player(g_opt.stream))
            g_writer.tee([](const void* p, std::size_t n) {
                if (g_peer.open()) g_outgoing.insert(g_outgoing.end(), static_cast<const std::uint8_t*>(p), static_cast<const std::uint8_t*>(p) + n);
            }, [] { send_outgoing(); });
        if (!g_writer.open(g_file, g_header)) {
            log("cannot create the recording %s - not recording", g_file.c_str());
            return;
        }
        g_writer.env(environment());
        g_writer.env(kSkipTag + skip_text);
        if (g_typed) g_writer.env(kTypesTag + typed::serialise(g_table));
        record_env_files();
        g_mode = Mode::Record;
        report_line("RECORDING to %s (state hashes: image .data/.bss 0x%08x..0x%08x%s%s; snapshot every %u frames)",
                    g_file.c_str(), static_cast<unsigned>(kDataVa), static_cast<unsigned>(kDataEnd),
                    g_opt.heap ? ", CRT heap" : "", g_opt.back_buffer ? ", back buffer" : "",
                    static_cast<unsigned>(g_opt.snapshot_every));
        report_line(g_typed ? "image hash: typed (pointer fields normalised, typed data compared as it is, untyped bytes "
                              "by the heuristics)"
                            : "image hash: heuristic pointer positions only ([Debug] ReplayHashTyped=0)");
        if (!uninit_fix)
            report_line("warning: [Fixes] UninitReads=0 - reads of uninitialised stack memory differ between builds; set "
                        "UninitReads=1 for recordings that check refactored code");
    } else {
        const char* err = nullptr;
        if (!g_opt.follow.empty()) {
            g_file = g_opt.follow;
            std::string host, error;
            std::uint16_t port = 0;
            if (!platform::net::split_address(g_opt.follow, host, port)) {
                err = "not a HOST:PORT address";
            } else {
                log("[Coop] joining %s", g_opt.follow.c_str());
                g_peer = platform::net::Connection::connect_to(host, port, 60000, error);
                if (!g_peer.open()) {
                    log("[Coop] cannot reach the host: %s", error.c_str());
                    err = "cannot reach the host";
                } else {
                    err = g_reader.follow([](std::vector<std::uint8_t>& data) {
                        std::uint8_t chunk[1 << 16];
                        const long got = g_peer.receive(chunk, sizeof chunk);
                        if (got <= 0) return false;
                        data.insert(data.end(), chunk, chunk + got);
                        return true;
                    });
                }
            }
        } else {
            g_file = absolute(g_opt.replay);
            err = g_reader.load(g_file);
        }
        if (err != nullptr) {
            report_line("cannot replay %s: %s", g_file.c_str(), err);
            write_report_file();
            real_exit_process(5);
        }
        g_header = g_reader.header();
        if (g_header.data_va != kDataVa || g_header.data_size != kDataSize || g_header.page_size != kPageSize) {
            report_line("cannot replay %s: it hashes another data range (0x%08x + 0x%x)", g_file.c_str(),
                        static_cast<unsigned>(g_header.data_va), static_cast<unsigned>(g_header.data_size));
            write_report_file();
            real_exit_process(5);
        }
        if (g_opt.follow.empty() && !g_opt.stream.empty()) wait_for_player(g_opt.stream);  // a replay passed on (tests)
        g_mode = Mode::Replay;
        report_line("REPLAYING %s (%u bytes; recorded by an executable of stamp %08x, this is %08x; hashes: image%s%s)",
                    g_file.c_str(), static_cast<unsigned>(g_reader.size()), static_cast<unsigned>(g_header.exe_stamp),
                    static_cast<unsigned>(exe_timestamp()), (g_header.flags & kFlagHeap) ? ", CRT heap" : "",
                    (g_header.flags & kFlagBackBuffer) ? ", back buffer" : "");
        if (((g_header.flags & kFlagUninitFix) != 0) != uninit_fix)
            warn("[Fixes] UninitReads is %d now but was %d while recording", uninit_fix ? 1 : 0,
                 (g_header.flags & kFlagUninitFix) ? 1 : 0);
#if !JOAN_RELEASE  // the [Fixes] switches are constants of the release build (registry.hpp JOAN_DEBUG_SETTING), which
                   // has no record / replay (build_flavor.hpp)
        // The trigger order is part of the recorded behaviour: the replay sorts as the recording did (the ini value
        // stays for live play and recording).
        if (((g_header.flags & kFlagTriggerSortFix) != 0) != trigger_sort_fix) {
            hook::mutable_settings().fix_trigger_sort = (g_header.flags & kFlagTriggerSortFix) != 0;
            report_line("[Fixes] TriggerSort=%d for this replay (the recording's value; the ini says %d)",
                        (g_header.flags & kFlagTriggerSortFix) ? 1 : 0, trigger_sort_fix ? 1 : 0);
        }
        // Likewise how escorting soldiers move ([Fixes] UnitStuck): recordings made before the switch existed replay
        // with it off, as they were played.
        if (((g_header.flags & kFlagUnitStuckFix) != 0) != unit_stuck_fix) {
            hook::mutable_settings().fix_unit_stuck = (g_header.flags & kFlagUnitStuckFix) != 0;
            report_line("[Fixes] UnitStuck=%d for this replay (the recording's value; the ini says %d)",
                        (g_header.flags & kFlagUnitStuckFix) ? 1 : 0, unit_stuck_fix ? 1 : 0);
        }
        if (((g_header.flags & kFlagUnitStuckWatch) != 0) != unit_stuck_watch) {
            hook::mutable_settings().fix_unit_stuck_watch = (g_header.flags & kFlagUnitStuckWatch) != 0;
            report_line("[Fixes] UnitStuck progress check %s for this replay (as recorded)",
                        (g_header.flags & kFlagUnitStuckWatch) ? "on" : "off");
        }
        // The voice for an immovable siege weapon ([Fixes] ImmovableSiege): recordings made before the switch replay
        // with it off, as they were played.
        if (((g_header.flags & kFlagImmovableSiege) != 0) != immovable_siege_fix) {
            hook::mutable_settings().fix_immovable_siege = (g_header.flags & kFlagImmovableSiege) != 0;
            report_line("[Fixes] ImmovableSiege=%d for this replay (the recording's value; the ini says %d)",
                        (g_header.flags & kFlagImmovableSiege) ? 1 : 0, immovable_siege_fix ? 1 : 0);
        }
        // Penned animals ([Fixes] AnimalPen): recordings made before the switch replay with it off, as they were played.
        if (((g_header.flags & kFlagAnimalPen) != 0) != animal_pen_fix) {
            hook::mutable_settings().fix_animal_pen = (g_header.flags & kFlagAnimalPen) != 0;
            report_line("[Fixes] AnimalPen=%d for this replay (the recording's value; the ini says %d)",
                        (g_header.flags & kFlagAnimalPen) ? 1 : 0, animal_pen_fix ? 1 : 0);
        }
        // The message boxes' buttons: as recorded (a click on the OK button depends on where it is).
        hook::mutable_settings().fix_popup_buttons = (g_header.flags & kFlagPopupButtons) != 0;
        // The ending's placement: as recorded (a 1024x768 screen changes where the game keeps the mouse).
        hook::mutable_settings().fix_ending_layout = (g_header.flags & kFlagEndingLayout) != 0;
        hook::mutable_settings().fix_wheel_zoom = (g_header.flags & kFlagWheelZoom) != 0;  // the zoom step, as recorded
        // The mouse options ([Fixes] MouseOptions): the recording's switch and cursor speed.
        {
            const std::uint32_t recorded = (g_header.flags >> kCursorSensitivityShift) & kCursorSensitivityMask;
            hook::mutable_settings().fix_mouse_options = recorded != 0;
            if (recorded != 0) hook::mutable_settings().cursor_sensitivity = static_cast<int>(recorded) - 1;
            report_line("[Fixes] MouseOptions=%d for this replay (the recording's value)", recorded != 0 ? 1 : 0);
        }
        // The last mission ([Fixes] MissionCount): recordings made before the switch replay with it off, as they were
        // played (with it on the game looks for the scenario files, and the replay holds those lookups only if recorded).
        if (((g_header.flags & kFlagMissionCount) != 0) != mission_count_fix) {
            hook::mutable_settings().fix_mission_count = (g_header.flags & kFlagMissionCount) != 0;
            report_line("[Fixes] MissionCount=%d for this replay (the recording's value; the ini says %d)",
                        (g_header.flags & kFlagMissionCount) ? 1 : 0, mission_count_fix ? 1 : 0);
        }
#endif
        // The ground cache as recorded (recordings made before the flag replay with it on, as they were played).
        if (g_header.flags & kFlagNoPlnCache) {
            pln_cache_off();
            report_line("ground cache (map\\*.PLN) off, as recorded");
        }
        if (g_header.flags & kFlagStableGroundOrder) g_stable_ground_order = true;  // the tile order of the recording
        g_typed = (g_header.flags & kFlagTypedHash) != 0;
        bool have_table = false;
        Event e;
        std::string recorded_env;
        bool have_env = false;
        std::vector<EnvFile> env_files;
        while (g_reader.peek(e) && e.type == Ev::Env) {
            g_reader.next();
            const std::string text(reinterpret_cast<const char*>(e.payload), e.size);
            if (text.rfind(kFileTag, 0) == 0) {
                const std::size_t nl = text.find('\n', std::strlen(kFileTag));
                if (nl != std::string::npos)
                    env_files.push_back({text.substr(std::strlen(kFileTag), nl - std::strlen(kFileTag)), text.substr(nl + 1)});
            } else if (text.rfind(kSkipTag, 0) == 0) {
                const std::string recorded = text.substr(std::strlen(kSkipTag));
                // exactly the recording's list: its hashes were made with it (adding this build's newer defaults made
                // those words differ whenever they were not zero - test25 replayed by patch 164)
                g_opt.skip.clear();
                parse_ranges(recorded, g_opt.skip);
                // [Debug] ReplayHashSkipReplay (Phase 2 step 4, x64): words whose VALUE depends on the pointer width (DynArray
                // element sizes, buffer byte counts, stdio descriptors) are left out by THIS replay as well, so that the
                // comparison reaches the words that differ for a real reason. "@file" reads the list from a file.
                if (!g_opt.skip_replay_text.empty()) {
                    std::string extra = g_opt.skip_replay_text;
                    if (extra[0] == '@') {
                        extra.clear();
                        if (FILE* f = std::fopen(g_opt.skip_replay_text.c_str() + 1, "rb")) {
                            char buf[4096];
                            std::size_t n;
                            while ((n = std::fread(buf, 1, sizeof buf, f)) > 0) extra.append(buf, n);
                            std::fclose(f);
                            for (char& ch : extra) if (ch == '\n' || ch == '\r' || ch == ' ' || ch == '\t') ch = ',';
                        } else {
                            log("[Debug] ReplayHashSkipReplay: cannot open %s", g_opt.skip_replay_text.c_str() + 1);
                        }
                    }
                    std::vector<Range> more;
                    std::string bad2;
                    if (parse_ranges(extra, more, &bad2)) {
                        g_opt.skip.insert(g_opt.skip.end(), more.begin(), more.end());
                        log("[Debug] ReplayHashSkipReplay: %u more ranges left out of this replay's state hash", static_cast<unsigned>(more.size()));
                    } else {
                        log("[Debug] ReplayHashSkipReplay: cannot read \"%s\" - ignored", bad2.c_str());
                    }
                }
            } else if (text.rfind(kTypesTag, 0) == 0) {
                const std::size_t tag = std::strlen(kTypesTag);
                have_table = typed::deserialise(e.payload + tag, e.size - tag, g_table);
            } else {
                recorded_env = text;
                have_env = true;
            }
        }
        if (have_env) compare_environment(recorded_env, env_files);
        if (g_typed) {
            // the recording's table: its hashes were made with it (a later build may have typed more fields)
            const typed::Table mine = typed::builtin(kDataVa, kDataSize);
            if (!have_table) {
                warn("the recording has no usable typed table (Env \"hash-types\") - hashing with this build's");
                g_table = mine;
            } else if (typed::fingerprint(g_table) != typed::fingerprint(mine)) {
                report_line("image hash: the recording's typed table differs from this build's (%u / %u pointer fields, "
                            "%u / %u untyped ranges) - hashing with the recording's",
                            static_cast<unsigned>(g_table.pointers.size()), static_cast<unsigned>(mine.pointers.size()),
                            static_cast<unsigned>(g_table.untyped.size()), static_cast<unsigned>(mine.untyped.size()));
            }
        }
        report_line(g_typed ? "image hash: typed (recording version %u)"
                            : "image hash: heuristic pointer positions only (recording version %u, made before the typed "
                              "scheme or with ReplayHashTyped=0)",
                    static_cast<unsigned>(g_header.version));
    }
    if (g_typed && !typed::classes(g_table, kDataSize, g_class)) {
        report_line("image hash: the typed table is damaged (overlapping entries) - this build's is used");
        g_table = typed::builtin(kDataVa, kDataSize);
        typed::classes(g_table, kDataSize, g_class);
    }
    if (g_typed) {
        std::size_t untyped = 0;
        for (const typed::Span& u : g_table.untyped) untyped += u.end - u.begin;
        log("typed image hash: %u pointer fields, %u untyped bytes in %u ranges, %u bytes typed data",
            static_cast<unsigned>(g_table.pointers.size()), static_cast<unsigned>(untyped),
            static_cast<unsigned>(g_table.untyped.size()),
            static_cast<unsigned>(kDataSize - 4 * g_table.pointers.size() - untyped));
    }
    for (const Range& r : g_opt.skip)
        log("left out of the image hash: %s, 0x%x bytes", where(r.begin).c_str(), static_cast<unsigned>(r.end - r.begin));
    capture_pristine();
    find_exe_range();
    if (const int n = ini_int("ReplayHeapTrace", 4000); n > 0) {
        const std::string path = g_file + (g_mode == Mode::Record ? ".heap-record.txt" : ".heap-replay.txt");
        g_heap_trace = std::fopen(path.c_str(), "w");
        g_heap_trace_left = static_cast<std::uint32_t>(n);
        det_heap::set_trace(&heap_trace);
    }
    if (const std::string page = ini_string("ReplayDumpPage"); !page.empty()) {
        g_dump_page = static_cast<std::uint32_t>(std::strtoul(page.c_str(), nullptr, 0)) & ~(kPageSize - 1);
        const std::string path = g_file + (g_mode == Mode::Record ? ".pages-record.bin" : ".pages-replay.bin");
        g_dump_page_file = g_dump_page >= kDataVa && g_dump_page < kDataEnd ? std::fopen(path.c_str(), "wb") : nullptr;
        if (!g_dump_page_file) g_dump_page = 0;
        else log("ReplayDumpPage: page 0x%08x of every frame -> %s", static_cast<unsigned>(g_dump_page), path.c_str());
    }
    if (const std::string path = ini_string("ReplayDumpImage"); !path.empty()) {
        g_dump_image_every = static_cast<std::uint32_t>(std::max(1, ini_int("ReplayDumpImageEvery", 1)));
        g_dump_image_from = static_cast<std::uint32_t>(std::max(0, ini_int("ReplayDumpImageFrom", 0)));
        g_dump_image_file = std::fopen(path.c_str(), "wb");
        if (g_dump_image_file) log("ReplayDumpImage: the normalised words of every %u. frame from %u -> %s", static_cast<unsigned>(g_dump_image_every), static_cast<unsigned>(g_dump_image_from), path.c_str());
        else log("ReplayDumpImage: cannot create %s", path.c_str());
    }
    install_watch();
    install_imports();
}

// (last in the file: the functions above keep their code layout)
bool active() { return g_mode != Mode::Off; }
bool unpaced() { return g_mode == Mode::Replay && !g_opt.pace; }
bool stable_ground_order() { return g_stable_ground_order; }
void describe_code(unsigned long address, char* out, unsigned size) {
    std::snprintf(out, size, "%s", describe_frame(static_cast<std::uintptr_t>(address)).c_str());
}

}  // namespace joan::replay

