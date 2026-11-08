// The game's files on portable code (phase C5 round F1, src/platform/README.md; declarations in files.hpp).
//
// What the game does with files (census of a session with saving / loading: openjoan_imports.txt, and our own C++):
//   - File (ilib/file.cpp): the game's archive / save / profile files - CreateFile, ReadFile / WriteFile,
//     SetFilePointer, GetFileSize, CloseHandle, DeleteFile;
//   - the C runtime (lib/crt/lowio.cpp, time.cpp: fopen, _findfirst, remove, _mkdir, _chdir, _getcwd);
//   - D3DX's CFileMapping (lib/imports/import_stubs.cpp, lib/d3dx8/shader_assembler.cpp): textures / shaders read
//     through a read-only mapping of the whole file;
//   - FindFirstFile / FindNextFile (Misc::is_file_exist, Directory::read for the save lists, the CRT's _findfirst for
//     scripts, textures, items) and the FILETIME conversions of the save headers / file lists;
//   - the streaming loader (ilib/audio.cpp FileLoader, mode 2): ReadFileEx with a completion routine, delivered by
//     the game's alertable waits (SleepEx(5, TRUE) while models / animations load, the main loop's
//     MsgWaitForMultipleObjectsEx(MWMO_ALERTABLE)).
// Here:
//   - handles: our own table (values 0x6A000004 + 4 n, never a Windows handle); a call with a handle that is not
//     ours (the CRT's GetStdHandle results, events, threads) goes to Windows unchanged;
//   - files: C stdio streams (binary), with Windows' creation dispositions, sharing ignored, read / write switches
//     done as the C standard requires, FILE_FLAG_DELETE_ON_CLOSE honoured;
//   - mappings: read-only only (all the game uses): the view is the file's bytes read into memory;
//   - directory searches: std::filesystem, Windows' wildcard rules (* and ?, case-insensitive, "*.*" = "*"), "." and
//     ".." listed like NTFS does, entries in NTFS order (upper-case ordinal), times from the last write;
//   - ReadFileEx: the read is done at once; its completion routine is queued for the calling thread and runs in its
//     next alertable wait (SleepEx / WaitForMultipleObjectsEx here, MsgWaitForMultipleObjectsEx in sdl_window.cpp),
//     exactly where Windows may run it;
//   - errors: Windows' error codes through SetLastError (ERROR_FILE_NOT_FOUND, ERROR_PATH_NOT_FOUND,
//     ERROR_NO_MORE_FILES, ERROR_HANDLE_EOF, ...), which the game and the CRT read with GetLastError.
// The replay layer wraps these imports on top as it wrapped KERNEL32's.
#include "files.hpp"
#include "threads.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <io.h>
#include <share.h>
#else
#include <strings.h>  // strcasecmp
#include <unistd.h>
#endif

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <deque>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <thread>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL.h>

#include "../runtime/loader/import_overrides.hpp"
#include "../runtime/hitch.hpp"
#include "../registry.hpp"
#include "text.hpp"

namespace fs = std::filesystem;

namespace joan::platform::files {
namespace {

void set_error(DWORD e) { joan::platform::set_last_error(e); }

// ---------------------------------------------------------------- objects

struct FileObj {
    std::FILE* f = nullptr;
    // [Platform] FilesInMemory: a read-only file served from memory instead of `f` (which is then null)
    std::shared_ptr<const std::vector<std::uint8_t>> mem;
    std::int64_t mem_pos = 0;
    fs::path path;
    bool can_read = false;
    bool can_write = false;
    bool delete_on_close = false;
    int last = 0;  // 1 = read, 2 = write (C streams need a seek between the two)
    // [Debug] HitchLog: time spent reading this file and the bytes read (noted when it is closed)
    double read_ms = 0;
    std::uint64_t read_bytes = 0;
};
struct MappingObj {
    HANDLE file = nullptr;
};
struct FindEntry {
    WIN32_FIND_DATAA data;
};
struct FindObj {
    std::vector<FindEntry> entries;
    std::size_t next = 0;
};
struct Object {
    enum class Kind { File, Mapping, Find } kind = Kind::File;
    FileObj file;
    MappingObj mapping;
    FindObj find;
};

constexpr std::uintptr_t kHandleBase = 0x6A000000u;
constexpr std::uintptr_t kHandleSpan = 0x00400000u;  // up to 1M objects

std::mutex g_lock;
std::vector<std::unique_ptr<Object>> g_objects;  // index i <-> handle kHandleBase + 4 (i + 1)
std::vector<std::size_t> g_free;
std::unordered_map<const void*, std::size_t> g_views;  // MapViewOfFile pointer -> size

bool ours(HANDLE h) {
    const auto v = reinterpret_cast<std::uintptr_t>(h);
    return v > kHandleBase && v < kHandleBase + kHandleSpan && (v & 3) == 0;
}

HANDLE add(std::unique_ptr<Object> o) {
    std::lock_guard<std::mutex> l(g_lock);
    std::size_t i;
    if (!g_free.empty()) {
        i = g_free.back();
        g_free.pop_back();
        g_objects[i] = std::move(o);
    } else {
        i = g_objects.size();
        g_objects.push_back(std::move(o));
    }
    return reinterpret_cast<HANDLE>(kHandleBase + 4 * (i + 1));
}

Object* get(HANDLE h) {
    if (!ours(h)) return nullptr;
    const std::size_t i = (reinterpret_cast<std::uintptr_t>(h) - kHandleBase) / 4 - 1;
    std::lock_guard<std::mutex> l(g_lock);
    return i < g_objects.size() ? g_objects[i].get() : nullptr;
}

std::unique_ptr<Object> take(HANDLE h) {
    if (!ours(h)) return nullptr;
    const std::size_t i = (reinterpret_cast<std::uintptr_t>(h) - kHandleBase) / 4 - 1;
    std::lock_guard<std::mutex> l(g_lock);
    if (i >= g_objects.size() || !g_objects[i]) return nullptr;
    g_free.push_back(i);
    return std::move(g_objects[i]);
}

FileObj* file_of(HANDLE h) {
    Object* o = get(h);
    return o && o->kind == Object::Kind::File ? &o->file : nullptr;
}

// ---------------------------------------------------------------- paths, errors

fs::path to_path(const char* name) {
#ifdef _WIN32
    return fs::path(name);  // the system code page, as Windows' A functions
#else
    std::string s(name);
    std::replace(s.begin(), s.end(), '\\', '/');
    fs::path p(s);
    std::error_code ec;
    if (s.empty() || fs::exists(p, ec)) return p;
    // Windows' names are case-insensitive and the game does not always write a path as the file is named: each part
    // that is not there as written is matched ignoring case; from the first part that has no match on, as written (a
    // file being created).
    fs::path out = p.root_path();
    bool matching = true;
    for (const fs::path& part : p.relative_path()) {
        fs::path next = out / part;
        if (matching && !fs::exists(next, ec)) {
            matching = false;
            const fs::path dir = out.empty() ? fs::path(".") : out;
            for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec)) {
                const fs::path found = it->path().filename();
                if (strcasecmp(found.c_str(), part.c_str()) == 0) {
                    next = out / found;
                    matching = true;
                    break;
                }
            }
        }
        out = std::move(next);
    }
    return out;
#endif
}

DWORD error_of(const std::error_code& ec, const fs::path& p) {
    if (ec == std::errc::no_such_file_or_directory) {
        std::error_code e2;
        const fs::path parent = p.parent_path();
        if (!parent.empty() && !fs::exists(parent, e2)) return ERROR_PATH_NOT_FOUND;
        return ERROR_FILE_NOT_FOUND;
    }
    if (ec == std::errc::permission_denied || ec == std::errc::operation_not_permitted) return ERROR_ACCESS_DENIED;
    if (ec == std::errc::file_exists) return ERROR_ALREADY_EXISTS;
    if (ec == std::errc::directory_not_empty) return ERROR_DIR_NOT_EMPTY;
    return ERROR_ACCESS_DENIED;
}

DWORD missing_error(const fs::path& p) {
    std::error_code ec;
    const fs::path parent = p.parent_path();
    if (!parent.empty() && !fs::is_directory(parent, ec)) return ERROR_PATH_NOT_FOUND;
    return ERROR_FILE_NOT_FOUND;
}

// ---------------------------------------------------------------- stdio helpers

std::int64_t tell64(std::FILE* f) {
#ifdef _WIN32
    return _ftelli64(f);
#else
    return ftello(f);
#endif
}
bool seek64(std::FILE* f, std::int64_t off, int whence) {
#ifdef _WIN32
    return _fseeki64(f, off, whence) == 0;
#else
    return fseeko(f, static_cast<off_t>(off), whence) == 0;
#endif
}
std::int64_t size64(std::FILE* f) {
    std::fflush(f);
#ifdef _WIN32
    return _filelengthi64(_fileno(f));
#else
    struct stat st {};
    return fstat(fileno(f), &st) == 0 ? static_cast<std::int64_t>(st.st_size) : -1;
#endif
}
void prepare(FileObj& fo, int op) {
    if (fo.f && fo.last != 0 && fo.last != op) seek64(fo.f, 0, SEEK_CUR);  // C11 7.21.5.3: a seek between read and write
    fo.last = op;
}

// A file's contents through its C stream, or its copy in memory ([Platform] FilesInMemory).
std::int64_t file_tell(const FileObj& fo) { return fo.f ? tell64(fo.f) : fo.mem_pos; }
std::int64_t file_size(const FileObj& fo) { return fo.f ? size64(fo.f) : static_cast<std::int64_t>(fo.mem->size()); }
bool file_seek(FileObj& fo, std::int64_t pos) {
    if (fo.f) return seek64(fo.f, pos, SEEK_SET);
    if (pos < 0) return false;
    fo.mem_pos = pos;  // past the end is allowed (reads there give 0 bytes), as with a C stream
    return true;
}
std::size_t file_read(FileObj& fo, void* buf, std::size_t n) {
    if (fo.f) return n ? std::fread(buf, 1, n, fo.f) : 0;
    const auto size = static_cast<std::int64_t>(fo.mem->size());
    if (n == 0 || fo.mem_pos >= size) return 0;
    const std::size_t r = static_cast<std::size_t>(std::min<std::int64_t>(static_cast<std::int64_t>(n), size - fo.mem_pos));
    std::memcpy(buf, fo.mem->data() + fo.mem_pos, r);
    fo.mem_pos += static_cast<std::int64_t>(r);
    return r;
}
// Whether the last read failed (not just reached the end); clears the stream's error / end flags.
bool file_failed(FileObj& fo) {
    if (!fo.f) return false;
    const bool failed = std::ferror(fo.f) != 0;
    std::clearerr(fo.f);
    return failed;
}

// ---------------------------------------------------------------- FILETIME


std::uint64_t ticks(const FILETIME& ft) { return (static_cast<std::uint64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime; }
FILETIME filetime(std::uint64_t t) {
    FILETIME ft;
    ft.dwLowDateTime = static_cast<DWORD>(t & 0xffffffffu);
    ft.dwHighDateTime = static_cast<DWORD>(t >> 32);
    return ft;
}

FILETIME to_filetime(fs::file_time_type t) {
#ifdef _MSC_VER
    // MSVC's file_clock counts 100 ns ticks since 1601-01-01: FILETIME.
    return filetime(static_cast<std::uint64_t>(t.time_since_epoch().count()));
#else
    const auto sys = std::chrono::file_clock::to_sys(t);
    const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(sys.time_since_epoch()).count();
    return filetime(static_cast<std::uint64_t>(ns / 100) + 116444736000000000ull);  // 1601 .. 1970 in 100 ns
#endif
}

// The local time's current offset from UTC in seconds (what FileTimeToLocalFileTime applies: today's bias,
// daylight saving included).
std::int64_t local_offset_seconds() {
    const std::time_t now = std::time(nullptr);
    std::tm local{};
    std::tm utc{};
#ifdef _WIN32
    localtime_s(&local, &now);
    gmtime_s(&utc, &now);
#else
    localtime_r(&now, &local);
    gmtime_r(&now, &utc);
#endif
    auto days = [](const std::tm& t) { return static_cast<std::int64_t>(t.tm_year) * 366 + t.tm_yday; };
    std::int64_t d = days(local) - days(utc);
    if (d > 1) d = -1;  // year boundary
    if (d < -1) d = 1;
    return d * 86400 + (local.tm_hour - utc.tm_hour) * 3600 + (local.tm_min - utc.tm_min) * 60 + (local.tm_sec - utc.tm_sec);
}

// Days since 1970-01-01 -> civil date (H. Hinnant's algorithm).
void civil_from_days(std::int64_t z, int& y, unsigned& m, unsigned& d) {
    z += 719468;
    const std::int64_t era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z - era * 146097);
    const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    const std::int64_t yy = static_cast<std::int64_t>(yoe) + era * 400;
    const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const unsigned mp = (5 * doy + 2) / 153;
    d = doy - (153 * mp + 2) / 5 + 1;
    m = mp < 10 ? mp + 3 : mp - 9;
    y = static_cast<int>(yy + (m <= 2 ? 1 : 0));
}

// ---------------------------------------------------------------- directory searches

char upper(char c) { return (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c; }

// Windows' wildcard match (case-insensitive): * any run, ? one character.
bool wild_match(const char* p, const char* s) {
    const char* star = nullptr;
    const char* resume = nullptr;
    while (*s) {
        if (*p == '?' || (*p && upper(*p) == upper(*s))) {
            ++p;
            ++s;
        } else if (*p == '*') {
            star = p++;
            resume = s;
        } else if (star) {
            p = star + 1;
            s = ++resume;
        } else {
            return false;
        }
    }
    while (*p == '*') ++p;
    return *p == '\0';
}

bool has_wildcards(const std::string& s) { return s.find_first_of("*?") != std::string::npos; }

void fill(WIN32_FIND_DATAA& d, const std::string& name, const fs::directory_entry* e, bool dir) {
    std::memset(&d, 0, sizeof d);
    std::error_code ec;
    if (dir) {
        d.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    } else {
        d.dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
        if (e) {
            const std::uintmax_t size = e->file_size(ec);
            if (!ec) {
                d.nFileSizeLow = static_cast<DWORD>(size & 0xffffffffu);
                d.nFileSizeHigh = static_cast<DWORD>(static_cast<std::uint64_t>(size) >> 32);
            }
        }
    }
    if (e) {
        const fs::perms p = e->status(ec).permissions();
        if (!ec && (p & fs::perms::owner_write) == fs::perms::none) d.dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
        const fs::file_time_type t = e->last_write_time(ec);
        if (!ec) d.ftLastWriteTime = d.ftCreationTime = d.ftLastAccessTime = to_filetime(t);
    }
    std::strncpy(d.cFileName, name.c_str(), MAX_PATH - 1);
}

std::string narrow(const fs::path& p) {
#ifdef _WIN32
    // The system code page, as FindFirstFileA reports names (an unconvertible name: '?' like Windows).
    const std::wstring w = p.native();
    const int n = WideCharToMultiByte(CP_ACP, 0, w.c_str(), static_cast<int>(w.size()), nullptr, 0, nullptr, nullptr);
    std::string s(static_cast<std::size_t>(n > 0 ? n : 0), '\0');
    if (n > 0) WideCharToMultiByte(CP_ACP, 0, w.c_str(), static_cast<int>(w.size()), s.data(), n, nullptr, nullptr);
    return s;
#else
    return p.string();
#endif
}

// ---------------------------------------------------------------- ReadFileEx completions

struct Completion {
    LPOVERLAPPED_COMPLETION_ROUTINE routine;
    DWORD error;
    DWORD bytes;
    LPOVERLAPPED ov;
};
thread_local std::deque<Completion> t_pending;

}  // namespace

std::string native_path(const char* name) { return to_path(name ? name : "").string(); }

bool run_pending_io() {
    if (t_pending.empty()) return false;
    while (!t_pending.empty()) {
        const Completion c = t_pending.front();
        t_pending.pop_front();
        c.routine(c.error, c.bytes, c.ov);  // may queue more: they run in this wait too (like Windows' APC queue)
    }
    return true;
}

// ---------------------------------------------------------------- files

namespace {

// [Debug] HitchLog: the time of one read of a file (counted for the file and for the frame) while the log is on.
class ReadTimer {
public:
    explicit ReadTimer(FileObj* fo) : fo_(fo), on_(hitch::enabled()) {
        if (on_) t0_ = std::chrono::steady_clock::now();
    }
    void done(std::size_t bytes) {
        if (!on_) return;
        const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0_).count();
        fo_->read_ms += ms;
        fo_->read_bytes += bytes;
        hitch::add_file_time(ms);
    }

private:
    FileObj* fo_;
    bool on_;
    std::chrono::steady_clock::time_point t0_{};
};

// CreateFileA / CreateFileW.
HANDLE create_file(const fs::path& p, DWORD access, DWORD disposition, DWORD flags) {
    std::error_code ec;
    const fs::file_status st = fs::status(p, ec);
    const bool exists = fs::exists(st);
    if (exists && fs::is_directory(st)) {
        set_error(ERROR_ACCESS_DENIED);
        return INVALID_HANDLE_VALUE;
    }
    const bool rd = (access & (GENERIC_READ | GENERIC_ALL)) != 0;
    const bool wr = (access & (GENERIC_WRITE | GENERIC_ALL)) != 0;
    bool create = false;
    bool truncate = false;
    switch (disposition) {
        case CREATE_NEW:
            if (exists) {
                set_error(ERROR_FILE_EXISTS);
                return INVALID_HANDLE_VALUE;
            }
            create = true;
            break;
        case CREATE_ALWAYS:
            create = truncate = true;
            break;
        case OPEN_EXISTING:
            if (!exists) {
                set_error(missing_error(p));
                return INVALID_HANDLE_VALUE;
            }
            break;
        case OPEN_ALWAYS:
            create = !exists;
            break;
        case TRUNCATE_EXISTING:
            if (!exists) {
                set_error(missing_error(p));
                return INVALID_HANDLE_VALUE;
            }
            truncate = true;
            break;
        default:
            set_error(ERROR_INVALID_PARAMETER);
            return INVALID_HANDLE_VALUE;
    }
    const char* mode = nullptr;
    if (create || truncate) mode = (wr && !rd) ? "wb" : "w+b";
    else mode = wr ? "r+b" : "rb";
    std::FILE* f = nullptr;
#ifdef _WIN32
    const std::wstring wmode(mode, mode + std::strlen(mode));
    f = _wfsopen(p.c_str(), wmode.c_str(), _SH_DENYNO);
#else
    f = std::fopen(p.c_str(), mode);
#endif
    if (!f) {
        const int e = errno;
        set_error(e == ENOENT ? missing_error(p) : ERROR_ACCESS_DENIED);
        return INVALID_HANDLE_VALUE;
    }
    auto o = std::make_unique<Object>();
    o->kind = Object::Kind::File;
    o->file.f = f;
    o->file.path = p;
    o->file.can_read = rd || (!wr);
    o->file.can_write = wr;
    o->file.delete_on_close = (flags & FILE_FLAG_DELETE_ON_CLOSE) != 0;
    const HANDLE h = add(std::move(o));
    // Windows reports an existing file it opened / replaced for OPEN_ALWAYS / CREATE_ALWAYS.
    set_error(exists && (disposition == OPEN_ALWAYS || disposition == CREATE_ALWAYS) ? ERROR_ALREADY_EXISTS : 0);
    return h;
}

// ---- [Platform] FilesInMemory
// Opening a file during play could take 4-21 ms (an antivirus scan on open, measured with [Debug] HitchLog) while
// reading it took 0.03 ms: every first drop of an item, particle effect or item script stuttered. The folders below
// are read into memory at launch on a thread of their own, and the game's read-only opens of their files are served
// from there without touching the disk. TEXTURE\ITEM (125 MB) is only opened and read once at launch, so that later
// opens of it are quick, and not kept.
constexpr const char* kMemoryFolders[] = {"ITEM", "TEXTURE\\PARTICLE", "SCRIPT\\PSCRIPT"};
constexpr const char* kWarmFolders[] = {"TEXTURE\\ITEM"};

struct MemoryFiles {
    std::atomic<bool> ready{false};
    bool logged = false;
    std::unordered_map<std::string, std::shared_ptr<const std::vector<std::uint8_t>>> files;  // by memory_key
    std::uint64_t bytes = 0, warmed_bytes = 0;
    std::size_t warmed = 0;
    double ms = 0;
};
MemoryFiles* g_memory_files = nullptr;  // never freed: its reading thread may still run at exit

// A game file name as the key: lower case, backslashes, without a leading ".\".
std::string memory_key(std::string s) {
    for (char& c : s) {
        if (c == '/') c = '\\';
        else if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    }
    if (s.rfind(".\\", 0) == 0) s.erase(0, 2);
    return s;
}

bool read_whole(const fs::path& p, std::vector<std::uint8_t>& out) {
    std::ifstream in(p, std::ios::binary);
    std::error_code ec;
    const auto n = fs::file_size(p, ec);
    if (!in || ec) return false;
    out.resize(static_cast<std::size_t>(n));
    return n == 0 || static_cast<bool>(in.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(n)));
}

void load_memory_files(MemoryFiles* m) {
    const auto t0 = std::chrono::steady_clock::now();
    std::error_code ec;
    for (const char* folder : kMemoryFolders) {
        const fs::path root = to_path((hook::data_dir() + folder).c_str());  // (from the game folder, whatever the current one)
        for (fs::recursive_directory_iterator it(root, ec), end; !ec && it != end; it.increment(ec)) {
            if (!it->is_regular_file(ec)) continue;
            auto bytes = std::make_shared<std::vector<std::uint8_t>>();
            if (!read_whole(it->path(), *bytes)) continue;
            const std::string key = memory_key(std::string(folder) + "\\" + fs::relative(it->path(), root, ec).string());
            m->bytes += bytes->size();
            m->files[key] = std::move(bytes);
        }
        ec.clear();
    }
    std::vector<std::uint8_t> scratch;
    for (const char* folder : kWarmFolders) {
        for (fs::recursive_directory_iterator it(to_path((hook::data_dir() + folder).c_str()), ec), end; !ec && it != end; it.increment(ec)) {
            if (!it->is_regular_file(ec) || !read_whole(it->path(), scratch)) continue;
            ++m->warmed;
            m->warmed_bytes += scratch.size();
        }
        ec.clear();
    }
    m->ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
    m->ready.store(true, std::memory_order_release);
}

// The game's read-only open of a file kept in memory; INVALID_HANDLE_VALUE when it is not one (open it from disk).
HANDLE open_from_memory(const char* name, DWORD access, DWORD disposition) {
    MemoryFiles* const m = g_memory_files;
    if (m == nullptr || !m->ready.load(std::memory_order_acquire)) return INVALID_HANDLE_VALUE;
    if (!m->logged) {
        m->logged = true;
        hook::log_line("[Platform] FilesInMemory: %zu files in memory (%.1f MB), %zu files (%.1f MB) opened once; %.0f ms",
                       m->files.size(), static_cast<double>(m->bytes) / 1048576.0, m->warmed, static_cast<double>(m->warmed_bytes) / 1048576.0, m->ms);
    }
    if ((access & (GENERIC_WRITE | GENERIC_ALL)) != 0 || disposition != OPEN_EXISTING) return INVALID_HANDLE_VALUE;
    const auto it = m->files.find(memory_key(name));
    if (it == m->files.end()) return INVALID_HANDLE_VALUE;
    auto o = std::make_unique<Object>();
    o->kind = Object::Kind::File;
    o->file.mem = it->second;
    o->file.path = fs::path(name);
    o->file.can_read = true;
    const HANDLE h = add(std::move(o));
    set_error(0);
    return h;
}

}  // namespace

void start_memory_files() {
    if (hook::settings().files_in_memory == 0 || g_memory_files != nullptr) return;
    g_memory_files = new MemoryFiles;
    std::thread(load_memory_files, g_memory_files).detach();
}

HANDLE WINAPI CreateFileA(LPCSTR name, DWORD access, DWORD /*share: ignored*/, LPSECURITY_ATTRIBUTES /*sa*/,
                          DWORD disposition, DWORD flags, HANDLE /*templ*/) {
    if (!name || !*name) {
        set_error(ERROR_PATH_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
    if (const HANDLE h = open_from_memory(name, access, disposition); h != INVALID_HANDLE_VALUE) {
        if (hitch::enabled()) hitch::note("file opened: %s (from memory)", name);  // [Debug] HitchLog
        return h;
    }
    if (!hitch::enabled()) return create_file(to_path(name), access, disposition, flags);
    // [Debug] HitchLog: the open with the time it took (finding the file's name in any case, opening it)
    const auto t0 = std::chrono::steady_clock::now();
    const HANDLE h = create_file(to_path(name), access, disposition, flags);
    const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
    hitch::add_file_time(ms);
    hitch::note("file opened: %s (%.2f ms)", name, ms);
    return h;
}

namespace {
// D3DX's CFileMapping opens its files with the wide name when it has one.
HANDLE WINAPI j_CreateFileW(LPCWSTR name, DWORD access, DWORD, LPSECURITY_ATTRIBUTES, DWORD disposition, DWORD flags, HANDLE) {
    if (!name || !*name) {
        set_error(ERROR_PATH_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
#ifdef _WIN32
    return create_file(fs::path(name), access, disposition, flags);
#else
    const std::string u = joan::platform::text::utf16_to_utf8(name);  // WCHAR: char16_t here
    return create_file(to_path(u.c_str()), access, disposition, flags);
#endif
}
}  // namespace

BOOL WINAPI CloseHandle(HANDLE h) {
    if (!ours(h) && sync_close(h)) return TRUE;  // an event / thread of platform/process.cpp
    if (!ours(h)) JOAN_NOT_OURS(::CloseHandle(h), FALSE);
    std::unique_ptr<Object> o = take(h);
    if (!o) {
        set_error(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if (o->kind == Object::Kind::File && (o->file.f || o->file.mem)) {
        if (hitch::enabled() && o->file.read_bytes != 0)  // [Debug] HitchLog
            hitch::note("file read: %s, %llu bytes in %.2f ms", o->file.path.filename().string().c_str(),
                        static_cast<unsigned long long>(o->file.read_bytes), o->file.read_ms);
        if (o->file.f) std::fclose(o->file.f);
        if (o->file.delete_on_close) {
            std::error_code ec;
            fs::remove(o->file.path, ec);
        }
    }
    return TRUE;
}

BOOL WINAPI ReadFile(HANDLE h, LPVOID buf, DWORD n, LPDWORD got, LPOVERLAPPED ov) {
    if (!ours(h)) JOAN_NOT_OURS(::ReadFile(h, buf, n, got, ov), FALSE);
    FileObj* fo = file_of(h);
    if (got) *got = 0;
    if (!fo) {
        set_error(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if (!fo->can_read) {
        set_error(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    if (ov) {  // a positioned read (non-overlapped handle: synchronous at Offset)
        const std::int64_t off = (static_cast<std::int64_t>(ov->OffsetHigh) << 32) | ov->Offset;
        file_seek(*fo, off);
        fo->last = 0;
    }
    prepare(*fo, 1);
    ReadTimer timer(fo);
    const std::size_t r = file_read(*fo, buf, n);
    timer.done(r);
    if (file_failed(*fo) && r < n) {  // (clears the flags: EOF is not an error for ReadFile)
        set_error(ERROR_READ_FAULT);
        return FALSE;
    }
    if (got) *got = static_cast<DWORD>(r);
    if (ov) {
        ov->Internal = 0;
        ov->InternalHigh = r;
    }
    return TRUE;
}

BOOL WINAPI WriteFile(HANDLE h, LPCVOID buf, DWORD n, LPDWORD put, LPOVERLAPPED ov) {
    if (!ours(h)) JOAN_NOT_OURS(::WriteFile(h, buf, n, put, ov), FALSE);
    FileObj* fo = file_of(h);
    if (put) *put = 0;
    if (!fo) {
        set_error(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if (!fo->can_write) {
        set_error(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    if (ov) {
        const std::int64_t off = (static_cast<std::int64_t>(ov->OffsetHigh) << 32) | ov->Offset;
        seek64(fo->f, off, SEEK_SET);
        fo->last = 0;
    }
    prepare(*fo, 2);
    const std::size_t w = n ? std::fwrite(buf, 1, n, fo->f) : 0;
    if (put) *put = static_cast<DWORD>(w);
    if (w < n) {
        std::clearerr(fo->f);
        set_error(ERROR_DISK_FULL);
        return FALSE;
    }
    return TRUE;
}

DWORD WINAPI SetFilePointer(HANDLE h, LONG distance, PLONG high, DWORD method) {
    if (!ours(h)) JOAN_NOT_OURS(::SetFilePointer(h, distance, high, method), INVALID_SET_FILE_POINTER);
    FileObj* fo = file_of(h);
    if (!fo) {
        set_error(ERROR_INVALID_HANDLE);
        return INVALID_SET_FILE_POINTER;
    }
    std::int64_t move = high ? ((static_cast<std::int64_t>(*high) << 32) | static_cast<std::uint32_t>(distance))
                             : static_cast<std::int64_t>(distance);
    std::int64_t base = 0;
    if (method == FILE_CURRENT) base = file_tell(*fo);
    else if (method == FILE_END) base = file_size(*fo);
    else if (method != FILE_BEGIN) {
        set_error(ERROR_INVALID_PARAMETER);
        return INVALID_SET_FILE_POINTER;
    }
    const std::int64_t pos = base + move;
    if (pos < 0) {
        set_error(ERROR_NEGATIVE_SEEK);
        return INVALID_SET_FILE_POINTER;
    }
    if (!file_seek(*fo, pos)) {
        set_error(ERROR_SEEK);
        return INVALID_SET_FILE_POINTER;
    }
    fo->last = 0;
    if (high) *high = static_cast<LONG>(pos >> 32);
    set_error(0);  // a position of 0xFFFFFFFF is told from an error by GetLastError
    return static_cast<DWORD>(pos & 0xffffffffu);
}

DWORD WINAPI GetFileSize(HANDLE h, LPDWORD high) {
    if (!ours(h)) JOAN_NOT_OURS(::GetFileSize(h, high), INVALID_FILE_SIZE);
    FileObj* fo = file_of(h);
    if (!fo) {
        set_error(ERROR_INVALID_HANDLE);
        return INVALID_FILE_SIZE;
    }
    const std::int64_t s = file_size(*fo);
    if (s < 0) {
        set_error(ERROR_ACCESS_DENIED);
        return INVALID_FILE_SIZE;
    }
    if (high) *high = static_cast<DWORD>(static_cast<std::uint64_t>(s) >> 32);
    set_error(0);
    return static_cast<DWORD>(s & 0xffffffffu);
}

namespace {

DWORD WINAPI j_GetFileType(HANDLE h) {
    if (!ours(h)) JOAN_NOT_OURS(::GetFileType(h), FILE_TYPE_UNKNOWN);
    if (!file_of(h)) {
        set_error(ERROR_INVALID_HANDLE);
        return FILE_TYPE_UNKNOWN;
    }
    set_error(0);
    return FILE_TYPE_DISK;
}

BOOL WINAPI j_FlushFileBuffers(HANDLE h) {
    if (!ours(h)) JOAN_NOT_OURS(::FlushFileBuffers(h), FALSE);
    FileObj* fo = file_of(h);
    if (!fo) {
        set_error(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if (!fo->f) return TRUE;  // in memory: nothing to write
    return std::fflush(fo->f) == 0 ? TRUE : FALSE;
}

BOOL WINAPI j_SetEndOfFile(HANDLE h) {
    if (!ours(h)) JOAN_NOT_OURS(::SetEndOfFile(h), FALSE);
    FileObj* fo = file_of(h);
    if (!fo) {
        set_error(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if (!fo->f) {  // in memory: read-only
        set_error(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    const std::int64_t pos = tell64(fo->f);
    std::fflush(fo->f);
#ifdef _WIN32
    const bool ok = _chsize_s(_fileno(fo->f), pos) == 0;
#else
    const bool ok = ftruncate(fileno(fo->f), static_cast<off_t>(pos)) == 0;
#endif
    fo->last = 0;
    if (!ok) set_error(ERROR_ACCESS_DENIED);
    return ok ? TRUE : FALSE;
}

// The streaming loader's read: done now, completion queued for this thread's next alertable wait.
BOOL WINAPI j_ReadFileEx(HANDLE h, LPVOID buf, DWORD n, LPOVERLAPPED ov, LPOVERLAPPED_COMPLETION_ROUTINE routine) {
    if (!ours(h)) JOAN_NOT_OURS(::ReadFileEx(h, buf, n, ov, routine), FALSE);
    FileObj* fo = file_of(h);
    if (!fo || !ov) {
        set_error(fo ? ERROR_INVALID_PARAMETER : ERROR_INVALID_HANDLE);
        return FALSE;
    }
    const std::int64_t off = (static_cast<std::int64_t>(ov->OffsetHigh) << 32) | ov->Offset;
    DWORD error = 0;
    std::size_t r = 0;
    if (off >= file_size(*fo)) {
        error = ERROR_HANDLE_EOF;
    } else {
        file_seek(*fo, off);
        fo->last = 1;
        ReadTimer timer(fo);
        r = file_read(*fo, buf, n);
        timer.done(r);
        if (file_failed(*fo) && r < n) error = ERROR_READ_FAULT;
    }
    ov->Internal = error;
    ov->InternalHigh = r;
    if (routine) t_pending.push_back(Completion{routine, error, static_cast<DWORD>(r), ov});
    set_error(0);
    return TRUE;
}

DWORD WINAPI j_SleepEx(DWORD ms, BOOL alertable) {
    if (alertable && run_pending_io()) return WAIT_IO_COMPLETION;
    if (ms == 0) std::this_thread::yield();
    else if (ms == INFINITE) {
        for (;;) SDL_Delay(1000);  // nothing can wake it: as Windows without APCs
    } else {
        SDL_Delay(static_cast<Uint32>(ms));
    }
    return 0;
}

// The streaming worker (mode 1) waits on events: Windows' wait, after our pending completions.
DWORD WINAPI j_WaitForMultipleObjectsEx(DWORD n, const HANDLE* hs, BOOL all, DWORD ms, BOOL alertable) {
    if (alertable && run_pending_io()) return WAIT_IO_COMPLETION;
    DWORD r = 0;
    if (sync_wait(n, hs, all != FALSE, ms, alertable != FALSE, r)) return r;  // events / threads of process.cpp
    JOAN_NOT_OURS(::WaitForMultipleObjectsEx(n, hs, all, ms, alertable), WAIT_FAILED);
}

// ---------------------------------------------------------------- read-only mappings

HANDLE WINAPI j_CreateFileMappingA(HANDLE file, [[maybe_unused]] LPSECURITY_ATTRIBUTES sa, DWORD protect, [[maybe_unused]] DWORD max_high, [[maybe_unused]] DWORD max_low,
                                   [[maybe_unused]] LPCSTR name) {
    if (!ours(file)) JOAN_NOT_OURS(::CreateFileMappingA(file, sa, protect, max_high, max_low, name), nullptr);
    if (!file_of(file)) {
        set_error(ERROR_INVALID_HANDLE);
        return nullptr;
    }
    if ((protect & 0xff) != PAGE_READONLY && (protect & 0xff) != PAGE_WRITECOPY) {
        set_error(ERROR_INVALID_PARAMETER);  // the game maps read-only only
        return nullptr;
    }
    auto o = std::make_unique<Object>();
    o->kind = Object::Kind::Mapping;
    o->mapping.file = file;
    set_error(0);
    return add(std::move(o));
}

LPVOID WINAPI j_MapViewOfFile(HANDLE mapping, [[maybe_unused]] DWORD access, DWORD off_high, DWORD off_low, SIZE_T bytes) {
    if (!ours(mapping)) JOAN_NOT_OURS(::MapViewOfFile(mapping, access, off_high, off_low, bytes), nullptr);
    Object* m = get(mapping);
    FileObj* fo = m && m->kind == Object::Kind::Mapping ? file_of(m->mapping.file) : nullptr;
    if (!fo) {
        set_error(ERROR_INVALID_HANDLE);
        return nullptr;
    }
    const std::int64_t size = file_size(*fo);
    const std::int64_t off = (static_cast<std::int64_t>(off_high) << 32) | off_low;
    if (off > size) {
        set_error(ERROR_ACCESS_DENIED);
        return nullptr;
    }
    const std::size_t n = bytes ? bytes : static_cast<std::size_t>(size - off);
    auto* view = static_cast<std::uint8_t*>(std::calloc(n ? n : 1, 1));
    if (!view) {
        set_error(ERROR_NOT_ENOUGH_MEMORY);
        return nullptr;
    }
    const std::int64_t keep = file_tell(*fo);
    file_seek(*fo, off);
    ReadTimer timer(fo);
    const std::size_t r = file_read(*fo, view, n);
    timer.done(r);
    (void)r;  // a view past the end reads as zeros
    file_failed(*fo);
    file_seek(*fo, keep);
    fo->last = 0;
    {
        std::lock_guard<std::mutex> l(g_lock);
        g_views[view] = n;
    }
    set_error(0);
    return view;
}

BOOL WINAPI j_UnmapViewOfFile(LPCVOID view) {
    {
        std::lock_guard<std::mutex> l(g_lock);
        auto it = g_views.find(view);
        if (it == g_views.end()) JOAN_NOT_OURS(::UnmapViewOfFile(view), FALSE);
        g_views.erase(it);
    }
    std::free(const_cast<void*>(view));
    return TRUE;
}

}  // namespace

// ---------------------------------------------------------------- directory searches

HANDLE WINAPI FindFirstFileA(LPCSTR pattern, LPWIN32_FIND_DATAA data) {
    if (!pattern || !*pattern || !data) {
        set_error(ERROR_PATH_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
    const std::string pat(pattern);
    const std::size_t cut = pat.find_last_of("\\/:");
    std::string dir = cut == std::string::npos ? std::string() : pat.substr(0, cut + 1);
    std::string mask = cut == std::string::npos ? pat : pat.substr(cut + 1);
    if (mask.empty()) {  // "dir\" names no file
        set_error(ERROR_FILE_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
    if (mask == "*.*") mask = "*";
    const fs::path dpath = dir.empty() ? fs::path(".") : to_path(dir.c_str());
    std::error_code ec;
    auto o = std::make_unique<Object>();
    o->kind = Object::Kind::Find;
    std::vector<FindEntry>& out = o->find.entries;
    if (!has_wildcards(mask)) {
        // A name: that entry (the existence checks of Misc::is_file_exist / is_dir_exist).
        const fs::path p = to_path(pat.c_str());  // the whole path (elsewhere: each part matched ignoring case)
        fs::directory_entry e(p, ec);
        if (ec || !e.exists(ec)) {
            set_error(fs::is_directory(dpath, ec) ? ERROR_FILE_NOT_FOUND : ERROR_PATH_NOT_FOUND);
            return INVALID_HANDLE_VALUE;
        }
        FindEntry fe{};
        fill(fe.data, mask, &e, e.is_directory(ec));
        out.push_back(fe);
    } else {
        if (!fs::is_directory(dpath, ec)) {
            set_error(ERROR_PATH_NOT_FOUND);
            return INVALID_HANDLE_VALUE;
        }
        std::vector<FindEntry> files;
        for (fs::directory_iterator it(dpath, fs::directory_options::skip_permission_denied, ec), end; !ec && it != end;
             it.increment(ec)) {
            const std::string name = narrow(it->path().filename());
            if (!wild_match(mask.c_str(), name.c_str())) continue;
            FindEntry fe{};
            fill(fe.data, name, &*it, it->is_directory(ec));
            files.push_back(fe);
        }
        // NTFS order: upper-case ordinal comparison.
        std::sort(files.begin(), files.end(), [](const FindEntry& a, const FindEntry& b) {
            const char* x = a.data.cFileName;
            const char* y = b.data.cFileName;
            for (; *x && *y; ++x, ++y) {
                const auto cx = static_cast<unsigned char>(upper(*x));
                const auto cy = static_cast<unsigned char>(upper(*y));
                if (cx != cy) return cx < cy;
            }
            return *x == '\0' && *y != '\0';
        });
        // "." and ".." first, as NTFS lists them in a directory that is not a drive's root.
        const fs::path abs = fs::absolute(dpath, ec);
        const bool root = !ec && abs.has_root_path() && abs.relative_path().empty();
        if (!root) {
            for (const char* special : {".", ".."}) {
                if (!wild_match(mask.c_str(), special)) continue;
                FindEntry fe{};
                fs::directory_entry e(dpath, ec);
                fill(fe.data, special, ec ? nullptr : &e, true);
                out.push_back(fe);
            }
        }
        out.insert(out.end(), files.begin(), files.end());
        if (out.empty()) {
            set_error(ERROR_FILE_NOT_FOUND);
            return INVALID_HANDLE_VALUE;
        }
    }
    *data = out.front().data;
    o->find.next = 1;
    set_error(0);
    return add(std::move(o));
}

BOOL WINAPI FindNextFileA(HANDLE h, LPWIN32_FIND_DATAA data) {
    if (!ours(h)) JOAN_NOT_OURS(::FindNextFileA(h, data), FALSE);
    Object* o = get(h);
    if (!o || o->kind != Object::Kind::Find) {
        set_error(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if (o->find.next >= o->find.entries.size()) {
        set_error(ERROR_NO_MORE_FILES);
        return FALSE;
    }
    *data = o->find.entries[o->find.next++].data;
    return TRUE;
}

BOOL WINAPI FindClose(HANDLE h) {
    if (!ours(h)) JOAN_NOT_OURS(::FindClose(h), FALSE);
    std::unique_ptr<Object> o = take(h);
    if (!o) {
        set_error(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    return TRUE;
}

// ---------------------------------------------------------------- paths

BOOL WINAPI DeleteFileA(LPCSTR name) {
    const fs::path p = to_path(name ? name : "");
    std::error_code ec;
    const fs::file_status st = fs::status(p, ec);
    if (!fs::exists(st)) {
        set_error(missing_error(p));
        return FALSE;
    }
    if (fs::is_directory(st)) {
        set_error(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    if (!fs::remove(p, ec)) {
        set_error(error_of(ec, p));
        return FALSE;
    }
    return TRUE;
}

namespace {

BOOL WINAPI j_CreateDirectoryA(LPCSTR name, LPSECURITY_ATTRIBUTES) {
    const fs::path p = to_path(name ? name : "");
    std::error_code ec;
    if (fs::exists(p, ec)) {
        set_error(ERROR_ALREADY_EXISTS);
        return FALSE;
    }
    if (!fs::create_directory(p, ec)) {
        set_error(ec ? error_of(ec, p) : ERROR_ALREADY_EXISTS);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI j_MoveFileA(LPCSTR from, LPCSTR to) {
    const fs::path a = to_path(from ? from : "");
    const fs::path b = to_path(to ? to : "");
    std::error_code ec;
    if (fs::exists(b, ec)) {
        set_error(ERROR_ALREADY_EXISTS);
        return FALSE;
    }
    fs::rename(a, b, ec);
    if (ec) {
        set_error(error_of(ec, a));
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI j_CopyFileA(LPCSTR from, LPCSTR to, BOOL fail_if_exists) {
    const fs::path a = to_path(from ? from : "");
    const fs::path b = to_path(to ? to : "");
    std::error_code ec;
    const auto opt = fail_if_exists ? fs::copy_options::none : fs::copy_options::overwrite_existing;
    if (!fs::copy_file(a, b, opt, ec)) {
        set_error(ec == std::errc::file_exists ? ERROR_FILE_EXISTS : error_of(ec, a));
        return FALSE;
    }
    return TRUE;
}

// Copies `s` (with its terminator) into buf / size as Windows' path functions do: the length without the
// terminator, or the size needed (with it) when the buffer is too small.
DWORD copy_out(const std::string& s, DWORD size, LPSTR buf) {
    const auto n = static_cast<DWORD>(s.size());
    if (!buf || size <= n) return n + 1;
    std::memcpy(buf, s.c_str(), s.size() + 1);
    return n;
}

DWORD WINAPI j_GetCurrentDirectoryA(DWORD size, LPSTR buf) {
    std::error_code ec;
    const fs::path p = fs::current_path(ec);
    if (ec) {
        set_error(ERROR_ACCESS_DENIED);
        return 0;
    }
    return copy_out(narrow(p), size, buf);
}

BOOL WINAPI j_SetCurrentDirectoryA(LPCSTR name) {
    std::error_code ec;
    const fs::path p = to_path(name ? name : "");
    fs::current_path(p, ec);
    if (ec) {
        set_error(fs::exists(p, ec) ? ERROR_DIRECTORY : ERROR_PATH_NOT_FOUND);
        return FALSE;
    }
    return TRUE;
}

DWORD WINAPI j_GetFullPathNameA(LPCSTR name, DWORD size, LPSTR buf, LPSTR* file_part) {
    std::error_code ec;
    const fs::path p = fs::absolute(to_path(name ? name : ""), ec);
    if (ec) {
        set_error(ERROR_INVALID_NAME);
        return 0;
    }
    std::string s = narrow(p.lexically_normal());
#ifdef _WIN32
    // "X:." -> "X:\dir" (lexically_normal may leave a trailing separator)
    if (s.size() > 3 && (s.back() == '\\' || s.back() == '/')) s.pop_back();
#endif
    const DWORD r = copy_out(s, size, buf);
    if (file_part && buf && r < size) {
        char* last = std::strrchr(buf, '\\');
        if (!last) last = std::strrchr(buf, '/');
        *file_part = last && last[1] ? last + 1 : nullptr;
    }
    return r;
}

// The CRT's _validdrive (the game's CD drive scan is removed): a drive whose root exists is a fixed disk.
UINT WINAPI j_GetDriveTypeA(LPCSTR root) {
    std::error_code ec;
    if (!root || !*root) return DRIVE_FIXED;  // the current directory's drive
    return fs::is_directory(to_path(root), ec) ? DRIVE_FIXED : DRIVE_NO_ROOT_DIR;
}

// The streaming loader asks for the sector size (alignment of its unbuffered reads, which this layer does not
// need): 512-byte sectors and the volume's real free / total space.
BOOL WINAPI j_GetDiskFreeSpaceA(LPCSTR root, LPDWORD sectors_per_cluster, LPDWORD bytes_per_sector, LPDWORD free_clusters,
                                LPDWORD total_clusters) {
    std::error_code ec;
    const fs::space_info si = fs::space(root && *root ? to_path(root) : fs::current_path(ec), ec);
    if (ec) {
        set_error(ERROR_PATH_NOT_FOUND);
        return FALSE;
    }
    constexpr std::uint64_t kCluster = 4096;
    auto clamp = [](std::uint64_t v) { return static_cast<DWORD>(std::min<std::uint64_t>(v, 0xffffffffu)); };
    if (sectors_per_cluster) *sectors_per_cluster = 8;
    if (bytes_per_sector) *bytes_per_sector = 512;
    if (free_clusters) *free_clusters = clamp(si.available / kCluster);
    if (total_clusters) *total_clusters = clamp(si.capacity / kCluster);
    return TRUE;
}

}  // namespace

// ---------------------------------------------------------------- FILETIME

BOOL WINAPI FileTimeToLocalFileTime(const FILETIME* in, LPFILETIME out) {
    if (!in || !out) return FALSE;
    const std::int64_t t = static_cast<std::int64_t>(ticks(*in)) + local_offset_seconds() * 10000000;
    *out = filetime(static_cast<std::uint64_t>(t < 0 ? 0 : t));
    return TRUE;
}

BOOL WINAPI FileTimeToSystemTime(const FILETIME* in, LPSYSTEMTIME out) {
    if (!in || !out) return FALSE;
    const std::uint64_t t = ticks(*in);
    if (t > 0x7fffffffffffffffull) {
        set_error(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    const std::uint64_t ms_total = t / 10000;
    const std::uint64_t days_since_1601 = ms_total / 86400000ull;
    const std::uint64_t ms_of_day = ms_total % 86400000ull;
    const std::int64_t days_since_1970 = static_cast<std::int64_t>(days_since_1601) - 134774;
    int y = 0;
    unsigned m = 0;
    unsigned d = 0;
    civil_from_days(days_since_1970, y, m, d);
    out->wYear = static_cast<WORD>(y);
    out->wMonth = static_cast<WORD>(m);
    out->wDay = static_cast<WORD>(d);
    out->wDayOfWeek = static_cast<WORD>((days_since_1601 + 1) % 7);  // 1601-01-01 was a Monday
    out->wHour = static_cast<WORD>(ms_of_day / 3600000);
    out->wMinute = static_cast<WORD>(ms_of_day / 60000 % 60);
    out->wSecond = static_cast<WORD>(ms_of_day / 1000 % 60);
    out->wMilliseconds = static_cast<WORD>(ms_of_day % 1000);
    return TRUE;
}

LONG WINAPI CompareFileTime(const FILETIME* a, const FILETIME* b) {
    const std::uint64_t x = ticks(*a);
    const std::uint64_t y = ticks(*b);
    return x < y ? -1 : (x > y ? 1 : 0);
}

}  // namespace joan::platform::files

namespace {
namespace pf = joan::platform::files;
}  // namespace

#define F(name, fn) JOAN_IMPORT_OVERRIDE("KERNEL32.dll", #name, fn, "portable " #name)
F(CreateFileA, &pf::CreateFileA);
F(CreateFileW, &pf::j_CreateFileW);
F(CloseHandle, &pf::CloseHandle);
F(ReadFile, &pf::ReadFile);
F(WriteFile, &pf::WriteFile);
F(SetFilePointer, &pf::SetFilePointer);
F(GetFileSize, &pf::GetFileSize);
F(GetFileType, &pf::j_GetFileType);
F(FlushFileBuffers, &pf::j_FlushFileBuffers);
F(SetEndOfFile, &pf::j_SetEndOfFile);
F(ReadFileEx, &pf::j_ReadFileEx);
F(SleepEx, &pf::j_SleepEx);
F(WaitForMultipleObjectsEx, &pf::j_WaitForMultipleObjectsEx);
F(CreateFileMappingA, &pf::j_CreateFileMappingA);
F(MapViewOfFile, &pf::j_MapViewOfFile);
F(UnmapViewOfFile, &pf::j_UnmapViewOfFile);
F(FindFirstFileA, &pf::FindFirstFileA);
F(FindNextFileA, &pf::FindNextFileA);
F(FindClose, &pf::FindClose);
F(DeleteFileA, &pf::DeleteFileA);
F(CreateDirectoryA, &pf::j_CreateDirectoryA);
F(MoveFileA, &pf::j_MoveFileA);
F(CopyFileA, &pf::j_CopyFileA);
F(GetCurrentDirectoryA, &pf::j_GetCurrentDirectoryA);
F(SetCurrentDirectoryA, &pf::j_SetCurrentDirectoryA);
F(GetFullPathNameA, &pf::j_GetFullPathNameA);
F(GetDriveTypeA, &pf::j_GetDriveTypeA);
F(GetDiskFreeSpaceA, &pf::j_GetDiskFreeSpaceA);
F(FileTimeToLocalFileTime, &pf::FileTimeToLocalFileTime);
F(FileTimeToSystemTime, &pf::FileTimeToSystemTime);
F(CompareFileTime, &pf::CompareFileTime);
#undef F
