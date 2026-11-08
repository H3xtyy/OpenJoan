// ilib/audio.cpp: functions of the original d:\ww\ilib\audio.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>
#include <initializer_list>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::cdecl_call;

using joan::call::thiscall;
using joan::call::stdcall_call;

constexpr img::Addr kCreateFileA = d::iat_KERNEL32_CreateFileA;
constexpr img::Addr kGetLastError = d::iat_KERNEL32_GetLastError;
constexpr img::Addr kGetFileSize = d::iat_KERNEL32_GetFileSize;
constexpr img::Addr kCloseHandle = d::iat_KERNEL32_CloseHandle;
constexpr img::Addr kEnterCriticalSection = d::iat_KERNEL32_EnterCriticalSection;
constexpr img::Addr kLeaveCriticalSection = d::iat_KERNEL32_LeaveCriticalSection;
constexpr img::Addr kSetEvent = d::iat_KERNEL32_SetEvent;

void crt_delete(void* p) { joan::crt::operator_delete(joan::orig::arg::mptr(p)); }                               // operator delete
void* crt_new(std::uint32_t n) { return joan::crt::operator_new(joan::orig::arg::u32(n)); }           // operator new
// INVALID_HANDLE_VALUE (CreateFileA failure, the closed-handle marker).
void* invalid_handle() { return reinterpret_cast<void*>(static_cast<std::intptr_t>(-1)); }

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::img::obj;
using joan::img::str;

using i8 = std::int8_t;

void* const kMem = obj(d::g_mem);    // memory manager (0x0058e7b0 alloc, 0x0058eb30 realloc)

// KERNEL32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;

using u16 = std::uint16_t;

}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
//
//    - FilePath::set (0x00577c70): strncpy 0x104, checks the file exists ("Cannot open %s").
namespace {
namespace game_leaves80b {


constexpr img::Addr kFiles(d::g_misc);
constexpr img::Addr kErr(d::g_err);

JOAN_COUNTER(c_path_set, "FilePath::set");

struct H {

    // 0x00577c70
    std::int32_t path_set(const char* path) {
        c_path_set.hit();
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(this))), joan::orig::arg::str(path), 0x104u);  // strncpy
        jo::view<std::uint8_t>(jo::self<jo::FileLoader>(this)->path[260]) = 0;
        if (joan::orig::as_misc(img::obj(kFiles))->is_file_exist(joan::orig::arg::str(path)) == 0) {  // 0x00590b80 file exists
            joan::orig::error::fatal(joan::orig::arg::mptr(img::obj(kErr)), joan::orig::arg::str(&img::global<const char>(d::s_Cannot_open_s)), path);  // 0x00586cb0 "Cannot open %s"
            return 0;
        }
        jo::view<std::uint8_t>(jo::self<jo::FileLoader>(this)->disabled) = 0;
        jo::self<jo::FileLoader>(this)->status = 1;
        return 1;
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("AsyncFile::file_open", 0x00577c70, &H::path_set);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/en3d_helpers30.cpp (en3d/helpers30)
//
//      0x00577d10 FileLoader::start(buffer, offset, size, callback, context) - 0x164-byte
//                 background loader: queues on the streamer 0xa5ce00 or reads synchronously
namespace {
namespace en3d_helpers30 {

// KERNEL32 import through its IAT slot.
template <class R = std::int32_t, class... A>
R import_call(img::Addr slot, A... args) {
    return stdcall_call<R>(global<std::uintptr_t>(slot), args...);
}

JOAN_COUNTER(c_loader_start, "FileLoader::start");

struct H {

    // 0x00577d10 FileLoader::start(buffer, offset, size, callback, context). Loader: +0 path,
    // +0x107 status (2 queued, 4 done), +0x108, +0x109 / +0x10a error (0x00578340), +0x10b
    // owns buffer, +0x10c buffer, +0x110 offset, +0x114 size (-1 whole file), +0x118 bytes
    // read, +0x11c, +0x120 callback, +0x124 context.
    std::int32_t loader_start(void* buffer, std::uint32_t offset, std::uint32_t size, void* callback,
                              void* context) {
        c_loader_start.hit();
        if (buffer != nullptr) {
            jo::view<void*>(jo::self<jo::FileLoader>(this)->buffer) = buffer;
            jo::view<std::uint8_t>(jo::self<jo::FileLoader>(this)->own_buffer) = 0;
        } else {
            if (size == 0xffffffffu)
                jo::view<void*>(jo::self<jo::FileLoader>(this)->buffer) = nullptr;
            else
                joan::orig::as_file_loader(this)->allocate(joan::orig::arg::u32(size));  // 0x00578280 allocate
            jo::view<std::uint8_t>(jo::self<jo::FileLoader>(this)->own_buffer) = 1;
        }
        jo::self<jo::FileLoader>(this)->offset = offset;
        jo::self<jo::FileLoader>(this)->size = size;
        jo::self<jo::FileLoader>(this)->bytes_read = 0;
        jo::view<std::uint8_t>(jo::self<jo::FileLoader>(this)->done_) = 0;
        jo::self<jo::FileLoader>(this)->callback = callback;
        jo::self<jo::FileLoader>(this)->context = context;
        jo::view<std::uint32_t>(jo::self<jo::FileLoader>(this)->priority) = 0x64;
        jo::view<std::uint8_t>(jo::self<jo::FileLoader>(this)->error_code) = 0;
        void* const streamer = img::obj(d::g_async_file_loader);
        const std::int32_t mode = global<std::int32_t>(d::g_a5ce04);
        auto set_error = [&](std::int32_t code) { joan::orig::as_file_loader(this)->set_error(static_cast<char>(joan::orig::arg::i32(code))); };  // 0x00578340
        auto error_code = [](std::uint32_t error) { return error == 2 ? 1 : 2; };
        if (mode == 2) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_streamer(streamer)->can_start()) != 0) {  // 0x00579f10
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(this)->begin_read()) != 0)    // 0x00578540
                    joan::orig::as_streamer(streamer)->add_active();                    // 0x00579f40
            } else {
                joan::orig::as_streamer(streamer)->enqueue(joan::orig::arg::mptr(this));  // 0x0057a040
                jo::self<jo::FileLoader>(this)->status = 2;
            }
        } else if (mode == 1) {
            joan::orig::as_streamer(streamer)->enqueue_signal(joan::orig::arg::mptr(this));  // 0x00579220
            jo::self<jo::FileLoader>(this)->status = 2;
        } else if (mode == 0) {
            // (A CloseHandle of the still -1 handle before CreateFileA is dead code.)
            if (jo::view<std::int8_t>(jo::self<jo::FileLoader>(this)->disabled) != 0) return 1;
            // HANDLE / pointer parameters typed as such: a u32 0 or handle in a 64-bit argument slot leaves its upper half undefined.
            void* handle = import_call<void*>(d::iat_KERNEL32_CreateFileA, static_cast<const char*>(static_cast<void*>(this)),
                                              0x80000000u, 1u, static_cast<void*>(nullptr), 3u, 0x80u, static_cast<void*>(nullptr));  // CreateFileA
            if (handle == invalid_handle()) set_error(error_code(import_call<std::uint32_t>(d::iat_KERNEL32_GetLastError)));  // GetLastError
            if (handle != invalid_handle() && jo::self<jo::FileLoader>(this)->size == 0xffffffffu) {
                std::uint32_t file_size = import_call<std::uint32_t>(d::iat_KERNEL32_GetFileSize, handle, static_cast<void*>(nullptr));  // GetFileSize
                if (file_size != 0xffffffffu) {
                    file_size -= jo::self<jo::FileLoader>(this)->offset;
                    if (jo::view<std::int8_t>(jo::self<jo::FileLoader>(this)->own_buffer) != 0)
                        joan::orig::as_file_loader(this)->allocate(joan::orig::arg::u32(file_size));
                    else
                        jo::self<jo::FileLoader>(this)->size = file_size;
                } else {
                    set_error(error_code(import_call<std::uint32_t>(d::iat_KERNEL32_GetLastError)));
                    import_call<std::int32_t>(d::iat_KERNEL32_CloseHandle, handle);  // CloseHandle
                    handle = invalid_handle();
                }
            }
            if (handle != invalid_handle()) {
                if (jo::self<jo::FileLoader>(this)->offset != 0)
                    import_call<std::uint32_t>(d::iat_KERNEL32_SetFilePointer, handle, jo::self<jo::FileLoader>(this)->offset, static_cast<void*>(nullptr), 0u);  // SetFilePointer
                std::uint32_t bytes_read;
                const std::int32_t ok = import_call<std::int32_t>(d::iat_KERNEL32_ReadFile, handle, jo::view<void*>(jo::self<jo::FileLoader>(this)->buffer), jo::self<jo::FileLoader>(this)->size,
                                                                  &bytes_read, static_cast<void*>(nullptr));  // ReadFile
                std::uint32_t error = 0;
                if (ok == 0) error = import_call<std::uint32_t>(d::iat_KERNEL32_GetLastError);
                if (error == 0 && (bytes_read > 0 || jo::self<jo::FileLoader>(this)->offset == 0)) {
                    jo::self<jo::FileLoader>(this)->bytes_read = bytes_read;
                    set_error(0);
                    jo::self<jo::FileLoader>(this)->status = 4;
                } else {
                    set_error(error_code(error));
                }
                import_call<std::int32_t>(d::iat_KERNEL32_CloseHandle, handle);  // CloseHandle
            }
        }
        return 1;
    }
};

#define HELPER30(name, addr, fn) JOAN_REPLACE("en3d/helpers30", name, addr, fn)
HELPER30("AsyncFile::file_read", 0x00577d10, &H::loader_start);
#undef HELPER30

}  // namespace en3d_helpers30
}  // namespace

// ---------------------------------------- from modules/game_stream1.cpp (game/stream1)
//
//      0x00578540 FileLoader::begin_read()          - root dir -> sector size, CreateFileA
//                 (FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING), sector-aligned ReadFileEx
//                 with completion routine 0x00578410 (thunk 0x004073e2); 1 when queued
//
//      0x00579220 Streamer::enqueue_signal(loader)  - priority insert into the pending list,
//                 or becomes the current loader (+0x28) and SetEvent(+0x30)
//
//      0x0057a040 Streamer::enqueue(loader)         - priority insert into the pending list
//
//      0x00578110 FileLoader::stop()                - cancel / close / wait (SleepEx alertable,
//                 100 x 100 ms), free the owned buffer; returns 1
//
//      0x00578280 FileLoader::allocate(size)        - (re)allocate the owned buffer +0x10c
//
//      0x00578340 FileLoader::set_error(code)       - +0x109 = 1, +0x10a = code, +0x107 = 4
//
//      0x00578250 FileLoader::done()                - (int)(char)+0x109
//
//      0x00579f40 Streamer::add_active()            - ++(+0x4c)
//
//      0x00579f10 Streamer::can_start()             - +0x4c < 16
namespace {
namespace game_stream1 {

// KERNEL32 import through its IAT slot.
template <class R = std::int32_t, class... A>
R import_call(img::Addr slot, A... args) {
    return stdcall_call<R>(global<std::uintptr_t>(slot), args...);
}

constexpr img::Addr kReadFileEx = d::iat_KERNEL32_ReadFileEx;
constexpr img::Addr kGetCurrentDirectoryA = d::iat_KERNEL32_GetCurrentDirectoryA;
constexpr img::Addr kSleepEx = d::iat_KERNEL32_SleepEx;

constexpr img::Addr kStreamer(d::g_async_file_loader);
constexpr img::Addr kErrorLog(d::g_err);

char* crt_strstr(const char* s, const char* t) { return joan::orig::arg::as<char*>(joan::orig::libc::strstr(joan::orig::arg::str(s), joan::orig::arg::str(t))); }
char* crt_strchr(const char* s, std::int32_t c) { return joan::orig::arg::as<char*>(joan::orig::libc::strchr(joan::orig::arg::str(s), joan::orig::arg::i32(c))); }
void crt_strncpy(char* d, const char* s, std::uint32_t n) { std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }

JOAN_COUNTER(c_begin_read, "FileLoader::begin_read");
JOAN_COUNTER(c_enqueue_signal, "Streamer::enqueue_signal");
JOAN_COUNTER(c_enqueue, "Streamer::enqueue");
JOAN_COUNTER(c_stop, "FileLoader::stop");
JOAN_COUNTER(c_allocate, "FileLoader::allocate");

char error_code(std::uint32_t error) { return error == 2 ? 1 : 2; }

// Shared body of 0x00579220 / 0x0057a040 (inside the critical section): unlink if listed, then
// insert before the first node whose priority (+0x11c) is <= the loader's, walking from +0x24.
void list_insert(void* self, void* loader) {
    if (jo::as_file_loader(loader)->queued != 0) {
        if (jo::view<void*>(jo::as_file_loader(loader)->next) != nullptr)
            jo::view<void*>(jo::as_file_loader(loader)->next->prev) = jo::view<void*>(jo::as_file_loader(loader)->prev);
        else
            jo::view<void*>(jo::as_streamer(self)->tail) = jo::view<void*>(jo::as_file_loader(loader)->prev);
        if (jo::view<void*>(jo::as_file_loader(loader)->prev) != nullptr)
            jo::view<void*>(jo::as_file_loader(loader)->prev->next) = jo::view<void*>(jo::as_file_loader(loader)->next);
        else
            jo::view<void*>(jo::as_streamer(self)->head) = jo::view<void*>(jo::as_file_loader(loader)->next);
        jo::view<void*>(jo::as_file_loader(loader)->next) = nullptr;
        jo::view<void*>(jo::as_file_loader(loader)->prev) = nullptr;
        jo::as_file_loader(loader)->queued = 0;
    }
    void* node = jo::view<void*>(jo::as_streamer(self)->head);
    void* prev = nullptr;
    while (node != nullptr && jo::as_file_loader(loader)->priority < jo::as_file_loader(node)->priority) {
        prev = node;
        node = jo::view<void*>(jo::as_file_loader(node)->next);
    }
    jo::view<void*>(jo::as_file_loader(loader)->next) = node;
    if (node != nullptr)
        jo::view<void*>(jo::as_file_loader(node)->prev) = loader;
    else
        jo::view<void*>(jo::as_streamer(self)->tail) = loader;
    jo::view<void*>(jo::as_file_loader(loader)->prev) = prev;
    if (prev != nullptr)
        jo::view<void*>(jo::as_file_loader(prev)->next) = loader;
    else
        jo::view<void*>(jo::as_streamer(self)->head) = loader;
    jo::as_file_loader(loader)->queued = 1;
}

struct H {
    const char* path() { return reinterpret_cast<const char*>(this); }

    std::int32_t begin_read() {  // 0x00578540
        c_begin_read.hit();
        std::int32_t result = 0;
        if (jo::view<std::int8_t>(jo::self<jo::FileLoader>(this)->disabled) != 0) return result;
        char root[0x104];
        std::memcpy(root, img::obj(d::s_C_9d8704), 4);  // "C:\"
        std::memset(root + 4, 0, 0x100);
        const char* p = crt_strstr(path(), &img::global<const char>(d::s_colon_bslash));  // ":\"
        if (p != nullptr) {
            const std::int32_t n = static_cast<std::int32_t>(p - path()) + 2;
            crt_strncpy(root, path(), static_cast<std::uint32_t>(n));
            root[n] = 0;
        } else if ((p = crt_strstr(path(), &img::global<const char>(d::s_bslash_bslash))) != nullptr) {  // "\\"
            char* q = crt_strchr(p + 2, 0x5c);
            if (q != nullptr) {
                q = crt_strchr(q + 1, 0x5c);
                if (q != nullptr) {
                    const std::int32_t n = static_cast<std::int32_t>(q - p) + 1;
                    crt_strncpy(root, p, static_cast<std::uint32_t>(n));
                    root[n] = 0;
                }
            }
        } else {
            import_call<std::uint32_t>(kGetCurrentDirectoryA, 0x104u, root);
            bool done = false;
            if (root[0] != 0 && root[1] == 0x3a && root[2] == 0x5c) {
                root[3] = 0;
                done = true;
            } else if (root[0] == 0x5c && root[1] == 0x5c) {
                char* q = crt_strchr(root + 2, 0x5c);
                if (q != nullptr) {
                    q = crt_strchr(q + 1, 0x5c);
                    if (q != nullptr) {
                        q[1] = 0;
                        done = true;
                    }
                }
            }
            if (!done)
                joan::orig::error::fatal(joan::orig::arg::mptr(img::obj(kErrorLog)), joan::orig::arg::str(&img::global<const char>(d::s_cannot_get_root_directory_na)), path());  // 0x00586cb0
        }
        const std::uint32_t sector = joan::orig::arg::as<std::uint32_t>(joan::orig::as_streamer(img::obj(kStreamer))->sector_size(joan::orig::arg::str(static_cast<const char*>(root))));  // 0x00579e10
        if (sector == 0)
            joan::orig::error::fatal(joan::orig::arg::mptr(img::obj(kErrorLog)), joan::orig::arg::str(&img::global<const char>(d::s_cannot_get_sector_size_of_fi)), path());  // 0x00586cb0
        jo::self<jo::FileLoader>(this)->sector_size = sector;
        jo::self<jo::FileLoader>(this)->handle = import_call<void*>(kCreateFileA, path(), 0x80000000u, 1u, static_cast<void*>(nullptr), 3u,
                                                             0x60000080u, static_cast<void*>(nullptr));
        if (jo::self<jo::FileLoader>(this)->handle == invalid_handle())
            joan::orig::as_file_loader(this)->set_error(static_cast<char>(joan::orig::arg::i32(error_code(import_call<std::uint32_t>(kGetLastError)))));  // 0x00578340
        if (jo::self<jo::FileLoader>(this)->handle != invalid_handle() && jo::self<jo::FileLoader>(this)->size == 0xffffffffu) {
            std::uint32_t size = import_call<std::uint32_t>(kGetFileSize, jo::self<jo::FileLoader>(this)->handle, static_cast<void*>(nullptr));
            if (size != 0xffffffffu) {
                size -= jo::self<jo::FileLoader>(this)->offset;
                if (jo::view<std::int8_t>(jo::self<jo::FileLoader>(this)->own_buffer) != 0)
                    joan::orig::as_file_loader(this)->allocate(joan::orig::arg::u32(size));  // 0x00578280
                else
                    jo::self<jo::FileLoader>(this)->size = size;
            } else {
                joan::orig::as_file_loader(this)->set_error(static_cast<char>(joan::orig::arg::i32(error_code(import_call<std::uint32_t>(kGetLastError)))));  // 0x00578340
                import_call<std::int32_t>(kCloseHandle, jo::self<jo::FileLoader>(this)->handle);
                jo::self<jo::FileLoader>(this)->handle = invalid_handle();
            }
        }
        if (jo::self<jo::FileLoader>(this)->handle == invalid_handle()) return result;
        jo::self<jo::FileLoader>(this)->aligned_offset = (jo::self<jo::FileLoader>(this)->offset / jo::self<jo::FileLoader>(this)->sector_size) * jo::self<jo::FileLoader>(this)->sector_size;
        jo::self<jo::FileLoader>(this)->aligned_size =
            ((jo::self<jo::FileLoader>(this)->offset + jo::self<jo::FileLoader>(this)->size + jo::self<jo::FileLoader>(this)->sector_size - 1u) / jo::self<jo::FileLoader>(this)->sector_size) *
                jo::self<jo::FileLoader>(this)->sector_size -
            jo::self<jo::FileLoader>(this)->aligned_offset;
        joan::orig::as_file_loader(this)->alloc_aligned(joan::orig::arg::u32(jo::self<jo::FileLoader>(this)->aligned_size), joan::orig::arg::u32(jo::self<jo::FileLoader>(this)->sector_size));  // 0x00578bd0
        jo::self<jo::FileLoader>(this)->u32_154 = 0;
        jo::self<jo::FileLoader>(this)->ovl_offset = jo::self<jo::FileLoader>(this)->aligned_offset;
        jo::self<jo::FileLoader>(this)->ovl_offset_high = 0;
        jo::view<void*>(jo::self<jo::FileLoader>(this)->ovl_event) = this;
        const std::int32_t ok = import_call<std::int32_t>(kReadFileEx, jo::self<jo::FileLoader>(this)->handle, jo::view<void*>(jo::self<jo::FileLoader>(this)->aligned_buf),
                                                          jo::self<jo::FileLoader>(this)->aligned_size, static_cast<void*>(&jo::self<jo::FileLoader>(this)->ovl_internal),
                                                          static_cast<std::uintptr_t>(fn::code_value(fn::Stream_read_done)));  // -> 0x00578410
        std::uint32_t error = 0;
        if (ok != 0)
            error = 0;
        else
            error = import_call<std::uint32_t>(kGetLastError);
        if (error != 0 && error != 0x3e5) {
            joan::orig::as_file_loader(this)->set_error(static_cast<char>(joan::orig::arg::i32(error_code(error))));  // 0x00578340
            import_call<std::int32_t>(kCloseHandle, jo::self<jo::FileLoader>(this)->handle);
            jo::self<jo::FileLoader>(this)->handle = invalid_handle();
            return result;
        }
        jo::self<jo::FileLoader>(this)->status = 3;
        result = 1;
        return result;
    }

    std::int32_t stop() {  // 0x00578110
        c_stop.hit();
        if (global<std::int32_t>(d::g_a5ce04) == 2) {  // mode, read twice like the original
            if (jo::self<jo::FileLoader>(this)->queued != 0) joan::orig::as_streamer(img::obj(kStreamer))->remove(joan::orig::arg::mptr(this));  // 0x0057a220
            if (jo::self<jo::FileLoader>(this)->handle != invalid_handle()) {
                import_call<std::int32_t>(kCloseHandle, jo::self<jo::FileLoader>(this)->handle);
                jo::self<jo::FileLoader>(this)->handle = invalid_handle();
                for (std::int32_t i = 100; i > 0; --i) {
                    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(this)->done()) != 0) break;  // 0x00578250
                    import_call<std::uint32_t>(kSleepEx, 100u, 1);
                }
                joan::orig::as_file_loader(this)->free_aligned();  // 0x00578c70
            }
        } else if (global<std::int32_t>(d::g_a5ce04) == 1) {
            joan::orig::as_streamer(img::obj(kStreamer))->cancel(joan::orig::arg::mptr(this));  // 0x00579440
        }
        if (jo::view<std::int8_t>(jo::self<jo::FileLoader>(this)->own_buffer) != 0) {
            crt_delete(jo::view<void*>(jo::self<jo::FileLoader>(this)->buffer));
            jo::view<std::uint8_t>(jo::self<jo::FileLoader>(this)->own_buffer) = 0;
        }
        jo::view<void*>(jo::self<jo::FileLoader>(this)->buffer) = nullptr;
        jo::self<jo::FileLoader>(this)->status = 0;
        return 1;
    }

    void allocate(std::uint32_t size) {  // 0x00578280
        c_allocate.hit();
        if (jo::view<std::int8_t>(jo::self<jo::FileLoader>(this)->own_buffer) != 0) {
            if (jo::self<jo::FileLoader>(this)->size != size) {
                crt_delete(jo::view<void*>(jo::self<jo::FileLoader>(this)->buffer));
                jo::view<void*>(jo::self<jo::FileLoader>(this)->buffer) = crt_new(size);
            }
        } else {
            jo::view<void*>(jo::self<jo::FileLoader>(this)->buffer) = crt_new(size);
            jo::view<std::uint8_t>(jo::self<jo::FileLoader>(this)->own_buffer) = 1;
        }
        jo::self<jo::FileLoader>(this)->size = size;
    }

    void set_error(char code) {  // 0x00578340
        jo::view<std::uint8_t>(jo::self<jo::FileLoader>(this)->done_) = 1;
        jo::self<jo::FileLoader>(this)->error_code = code;
        jo::self<jo::FileLoader>(this)->status = 4;
    }

    std::int32_t done() { return static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::FileLoader>(this)->done_)); }  // 0x00578250

    void enqueue_signal(void* loader) {  // 0x00579220
        if (loader == nullptr) return;
        c_enqueue_signal.hit();
        std::int32_t signal = 0;
        import_call<void>(kEnterCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));
        if (jo::view<void*>(jo::self<jo::Streamer>(this)->current) == nullptr) {
            jo::view<void*>(jo::self<jo::Streamer>(this)->current) = loader;
            signal = 1;
        } else {
            list_insert(this, loader);
        }
        import_call<void>(kLeaveCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));
        if (signal != 0) import_call<std::int32_t>(kSetEvent, jo::self<jo::Streamer>(this)->event_wake);
    }

    void enqueue(void* loader) {  // 0x0057a040
        if (loader == nullptr) return;
        c_enqueue.hit();
        import_call<void>(kEnterCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));
        list_insert(this, loader);
        import_call<void>(kLeaveCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));
    }

    void add_active() { jo::self<jo::Streamer>(this)->active_count = jo::self<jo::Streamer>(this)->active_count + 1; }        // 0x00579f40
    std::int32_t can_start() { return jo::self<jo::Streamer>(this)->active_count < 0x10 ? 1 : 0; }       // 0x00579f10
};

#define STREAM1(name, addr, fn) JOAN_REPLACE("game/stream1", name, addr, fn)
STREAM1("AsyncFile::overlap_load_now", 0x00578540, &H::begin_read);
STREAM1("AsyncFileLoader::add_queue", 0x00579220, &H::enqueue_signal);
STREAM1("Streamer::enqueue", 0x0057a040, &H::enqueue);
STREAM1("AsyncFile::file_close", 0x00578110, &H::stop);
STREAM1("AsyncFile::alloc_buf", 0x00578280, &H::allocate);
STREAM1("AsyncFile::set_read_status", 0x00578340, &H::set_error);
STREAM1("FileLoader::done", 0x00578250, &H::done);
STREAM1("Streamer::add_active", 0x00579f40, &H::add_active);
STREAM1("Streamer::can_start", 0x00579f10, &H::can_start);
#undef STREAM1

}  // namespace game_stream1
}  // namespace

// ---------------------------------------- from modules/game_b99_06.cpp (game/b99_06)
//
//    - Sound::set_11c 0x00578390 (v), ret 4: when [0xa5ce04] is 2 or 1 calls 0x005795b0 on
//      0xa5ce00 (this, v); +0x11c = v; returns it.
//
//    - Obj::dec_4c 0x00579f70 (ret 0): --[+0x4c] (eax = this).
//
//    - SoundSys::shutdown 0x0057a920 (ret 0): release buffers (0x0057dd30), heap free +0xb
//      ("audio.cpp", line [0xa150e8]+5), byte +2 = 0 (the original also released the DirectSound device +0x281
//      when byte +2 was set; there is none since cleanup C2, see DSound::create 0x0057a740).
namespace {
namespace game_b99_06 {


JOAN_COUNTER(c_shutdown, "SoundSys::shutdown");

struct H {

    // 0x00578390
    u32 set_11c(u32 v) {
        if (global<i32>(d::g_a5ce04) == 2) {
            joan::orig::as_streamer(obj(d::g_async_file_loader))->requeue(joan::orig::arg::mptr(this), joan::orig::arg::i32(v));  // 0x005795b0
        } else if (global<i32>(d::g_a5ce04) == 1) {
            joan::orig::as_streamer(obj(d::g_async_file_loader))->requeue(joan::orig::arg::mptr(this), joan::orig::arg::i32(v));  // 0x005795b0
        }
        jo::view<u32>(jo::self<jo::FileLoader>(this)->priority) = v;
        return jo::view<u32>(jo::self<jo::FileLoader>(this)->priority);
    }

    // 0x00579f70
    H* dec_4c() {
        jo::self<jo::Streamer>(this)->active_count = jo::self<jo::Streamer>(this)->active_count - 1;
        return this;
    }

    // 0x0057a920
    void shutdown() {
        c_shutdown.hit();
        joan::orig::as_audio(this)->release_buffers();  // 0x0057dd30
        if (jo::self<jo::Audio>(this)->wav_buf != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_a150e8)) + 5;
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Audio>(this)->wav_buf)), str(d::s_d_ww_ilib_audio_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::Audio>(this)->wav_buf = nullptr;
        }
        jo::view<u8>(jo::self<jo::Audio>(this)->wav_init_flag) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_06", name, addr, fn)
B("Sound::set_11c", 0x00578390, &H::set_11c);
B("Obj::dec_4c", 0x00579f70, &H::dec_4c);
B("SoundSys::shutdown", 0x0057a920, &H::shutdown);
#undef B

}  // namespace game_b99_06
}  // namespace

// ---------------------------------------- from modules/game_b102_07.cpp (game/b102_07)
//
//      0x00578410 Stream::read_done(err, bytes, ovl)  (stdcall, ret 0xc) - ReadFileEx completion
//                 routine (this = ovl->hEvent): memmove into the buffer, 0x00578c70,
//                 0x00578340(err != 0 && err != 0x26), 0x00579f70 / 0x00579fa0 on 0xa5ce00.
//
//      0x0057a3c0 Audio::~Audio                       (thiscall, ret 0) - 0x0057a6c0, member dtor
//                 0x00417a30 (+0x14c), base dtor 0x0047cac0 (+0x17) (SEH unwind states only).
//
//      0x0057aa30 Audio::load_sample(id)              (thiscall, ret 4)
//
//  - No code / data references at all (dead code): every function here except 0x00578410 (its
//    thunk is passed to ReadFileEx at 0x005789a0) and 0x0057a3c0 (called at 0x00566dde on
//    0xa50ec8). (The DirectSound buffer functions 0x0057baf0 Audio::stop_buffer / 0x0057c750
//    Audio::stop_stream went in cleanup C2.)
namespace {
namespace game_b102_07 {


JOAN_COUNTER(c_read_done, "Stream::read_done");
JOAN_COUNTER(c_load_sample, "Audio::load_sample");

// ------------------------------------------------------------------ 0x00578410
void JOAN_STDCALL stream_read_done(u32 err, u32 bytes, void* ovl) {
    c_read_done.hit();
    // ovl = &loader->ovl_internal (the OVERLAPPED passed to ReadFileEx); its hEvent field holds the loader.
    u8* s = reinterpret_cast<u8*>(reinterpret_cast<jo::FileLoader*>(static_cast<u8*>(ovl) - offsetof(jo::FileLoader, ovl_internal))->ovl_event);
    jo::as_file_loader(s)->u32_154 = bytes;
    jo::view<i32>(jo::as_file_loader(s)->bytes_read) = static_cast<i32>(jo::as_file_loader(s)->u32_154 - (jo::as_file_loader(s)->offset - jo::as_file_loader(s)->aligned_offset));
    if (jo::view<i32>(jo::as_file_loader(s)->bytes_read) > jo::view<i32>(jo::as_file_loader(s)->size)) jo::view<i32>(jo::as_file_loader(s)->bytes_read) = jo::view<i32>(jo::as_file_loader(s)->size);
    const u32 n = jo::as_file_loader(s)->bytes_read;
    const u8* const src = jo::as_file_loader(s)->aligned_buf + (jo::as_file_loader(s)->offset - jo::as_file_loader(s)->aligned_offset);
    std::memcpy(joan::orig::arg::mptr(jo::as_file_loader(s)->buffer), joan::orig::arg::ptr(src), joan::orig::arg::u32(n));  // memmove
    joan::orig::as_file_loader(s)->free_aligned();                                    // 0x00578c70
    if (err == 0 || err == 0x26)
        joan::orig::as_file_loader(s)->set_error(static_cast<char>(joan::orig::arg::i32(0)));  // 0x00578340
    else
        joan::orig::as_file_loader(s)->set_error(static_cast<char>(joan::orig::arg::i32(1)));
    joan::orig::as_streamer(obj(d::g_async_file_loader))->dec_count_4c();  // 0x00579f70
    joan::orig::as_streamer(obj(d::g_async_file_loader))->pump();  // 0x00579fa0
}

struct H {

    // -------------------------------------------------------------- 0x0057a3c0
    u32 audio_dtor() {
        joan::orig::as_audio(this)->shutdown();                                   // 0x0057a6c0
        joan::orig::as_archive(&jo::self<jo::Audio>(this)->wav_res2)->dtor();                          // 0x00417a30
        return joan::orig::arg::as<u32>(joan::orig::as_res_idx_file(&jo::self<jo::Audio>(this)->wav_res)->dtor_47cac0());               // 0x0047cac0
    }

    // -------------------------------------------------------------- 0x0057aa30
    i32 load_sample(u32 id) {
        c_load_sample.hit();
        if (jo::view<i8>(jo::self<jo::Audio>(this)->mid_init_flag) == 0 || jo::view<i8>(jo::self<jo::Audio>(this)->mid_flag) == 0) return 0;
        joan::orig::as_audio(this)->cd_stop_57ab20();  // 0x0057ab20
        i32 len;
        void* r = thiscall<void*>(fn::get_file_name_59ce40, &jo::self<jo::Audio>(this)->wav_res, id, &len);  // 0x0059ce40
        if (r == nullptr) return 0;
        if (len > jo::self<jo::Audio>(this)->mid_buf_size) {
            jo::self<jo::Audio>(this)->mid_buf_size = len;
            const i32 line = static_cast<i32>(global<i16>(d::g_a150f0)) + 0x12;
            jo::self<jo::Audio>(this)->mid_buf = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::self<jo::Audio>(this)->mid_buf), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Audio>(this)->mid_buf_size)), str(d::s_d_ww_ilib_audio_cpp), joan::orig::arg::i32(line)));  // 0x0058eb30
        }
        if (joan::orig::as_file(r)->file_read(joan::orig::arg::mptr(jo::self<jo::Audio>(this)->mid_buf), joan::orig::arg::u32(len)) == 0) return 0;  // 0x005873a0
        return 1;
    }
};

#define B102(name, addr, fn) JOAN_REPLACE("game/b102_07", name, addr, fn)
B102("Stream::read_done", 0x00578410, &stream_read_done);
B102("Audio::~Audio", 0x0057a3c0, &H::audio_dtor);
B102("Audio::load_sample", 0x0057aa30, &H::load_sample);
#undef B102

}  // namespace game_b102_07
}  // namespace

// ---------------------------------------- from modules/game_stream2.cpp (game/stream2)
//
//      0x00578bd0 FileLoader::alloc_aligned(size, align) - new[] size + align at +0x144,
//                 +0x148 = start rounded up to align; returns the aligned pointer
//
//      0x00578c70 FileLoader::free_aligned()           - delete +0x144, clear +0x144 / +0x148
//
//      0x00579440 Streamer::cancel(loader)             - current loader: ResetEvent(+0x34) and
//                 wait for it (10 s) outside the lock; else unlink it from the pending list
//
//      0x00579e10 Streamer::sector_size(root)          - GetDiskFreeSpaceA bytes per sector,
//                 cached per drive letter at +0x50; 0 on failure
//
//      0x0057a220 Streamer::remove(loader)             - unlink from the pending list
namespace {
namespace game_stream2 {


constexpr img::Addr kResetEvent = d::iat_KERNEL32_ResetEvent;
constexpr img::Addr kWaitForSingleObject = d::iat_KERNEL32_WaitForSingleObject;
constexpr img::Addr kGetDiskFreeSpaceA = d::iat_KERNEL32_GetDiskFreeSpaceA;

std::int32_t crt_toupper(std::int32_t c) { return joan::orig::arg::as<std::int32_t>(joan::orig::libc::toupper(joan::orig::arg::i32(c))); }

JOAN_COUNTER(c_cancel, "Streamer::cancel");
JOAN_COUNTER(c_sector, "Streamer::sector_size");
JOAN_COUNTER(c_remove, "Streamer::remove");

// Unlink a loader from the streamer's pending list (+0x20 tail / +0x24 head).
void unlink(void* self, void* loader) {
    if (jo::view<void*>(jo::as_file_loader(loader)->next) != nullptr)
        jo::view<void*>(jo::as_file_loader(loader)->next->prev) = jo::view<void*>(jo::as_file_loader(loader)->prev);
    else
        jo::view<void*>(jo::as_streamer(self)->tail) = jo::view<void*>(jo::as_file_loader(loader)->prev);
    if (jo::view<void*>(jo::as_file_loader(loader)->prev) != nullptr)
        jo::view<void*>(jo::as_file_loader(loader)->prev->next) = jo::view<void*>(jo::as_file_loader(loader)->next);
    else
        jo::view<void*>(jo::as_streamer(self)->head) = jo::view<void*>(jo::as_file_loader(loader)->next);
    jo::view<void*>(jo::as_file_loader(loader)->next) = nullptr;
    jo::view<void*>(jo::as_file_loader(loader)->prev) = nullptr;
    jo::as_file_loader(loader)->queued = 0;
}

struct H {

    void* alloc_aligned(std::uint32_t size, std::uint32_t align) {  // 0x00578bd0
        if (jo::view<void*>(jo::self<jo::FileLoader>(this)->raw_buf) != nullptr) crt_delete(jo::view<void*>(jo::self<jo::FileLoader>(this)->raw_buf));
        jo::view<void*>(jo::self<jo::FileLoader>(this)->raw_buf) = crt_new(align + size);
        const std::uintptr_t aligned = ((reinterpret_cast<std::uintptr_t>(jo::self<jo::FileLoader>(this)->raw_buf) + align - 1u) / align) * align;
        jo::self<jo::FileLoader>(this)->aligned_buf = reinterpret_cast<std::uint8_t*>(aligned);
        return reinterpret_cast<void*>(aligned);
    }

    void free_aligned() {  // 0x00578c70
        if (jo::view<void*>(jo::self<jo::FileLoader>(this)->raw_buf) != nullptr) crt_delete(jo::view<void*>(jo::self<jo::FileLoader>(this)->raw_buf));
        jo::view<void*>(jo::self<jo::FileLoader>(this)->raw_buf) = nullptr;
        jo::self<jo::FileLoader>(this)->aligned_buf = nullptr;
    }

    void cancel(void* loader) {  // 0x00579440
        if (loader == nullptr) return;
        c_cancel.hit();
        void* const cs = static_cast<void*>(&jo::self<jo::Streamer>(this)->lock);
        win_call<void>(kEnterCriticalSection, cs);
        if (jo::view<void*>(jo::self<jo::Streamer>(this)->current) == loader) {
            win_call<std::int32_t>(kResetEvent, jo::self<jo::Streamer>(this)->event_done);
            win_call<void>(kLeaveCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));
            win_call<std::uint32_t>(kWaitForSingleObject, jo::self<jo::Streamer>(this)->event_done, 0x2710u);
            win_call<void>(kEnterCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));
        } else if (jo::as_file_loader(loader)->queued != 0) {
            unlink(this, loader);
        }
        win_call<void>(kLeaveCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));
    }

    std::uint32_t sector_size(const char* root) {  // 0x00579e10
        c_sector.hit();
        std::int32_t drive = -1;
        if (crt_toupper(static_cast<std::int8_t>(root[0])) >= 0x41 && crt_toupper(static_cast<std::int8_t>(root[0])) <= 0x5a &&
            static_cast<std::int8_t>(root[1]) == 0x3a && static_cast<std::int8_t>(root[2]) == 0x5c) {
            drive = crt_toupper(static_cast<std::int8_t>(root[0])) - 0x41;
            if (jo::self<jo::Streamer>(this)->sector_size_cache[drive] != 0) return jo::self<jo::Streamer>(this)->sector_size_cache[drive];
        }
        std::uint32_t sectors_per_cluster, bytes_per_sector, free_clusters, total_clusters;
        if (win_call<std::int32_t>(kGetDiskFreeSpaceA, root, &sectors_per_cluster, &bytes_per_sector, &free_clusters,
                                   &total_clusters) == 0)
            return 0;
        if (drive >= 0) jo::self<jo::Streamer>(this)->sector_size_cache[drive] = bytes_per_sector;
        return bytes_per_sector;
    }

    void remove(void* loader) {  // 0x0057a220
        if (loader == nullptr) return;
        c_remove.hit();
        win_call<void>(kEnterCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));
        if (jo::as_file_loader(loader)->queued != 0) unlink(this, loader);
        win_call<void>(kLeaveCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));
    }
};

#define STREAM2(name, addr, fn) JOAN_REPLACE("game/stream2", name, addr, fn)
STREAM2("FileLoader::alloc_aligned", 0x00578bd0, &H::alloc_aligned);
STREAM2("FileLoader::free_aligned", 0x00578c70, &H::free_aligned);
STREAM2("AsyncFileLoader::remove_queue", 0x00579440, &H::cancel);
STREAM2("Streamer::sector_size", 0x00579e10, &H::sector_size);
STREAM2("Streamer::remove", 0x0057a220, &H::remove);
#undef STREAM2

}  // namespace game_stream2
}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
//
//    - SoundSlots::ctor 0x00578ce0: +4 = +0x48 = -1, memset(+0x50, 0, 0x68), +0 = 0.
//
//    - DSound::create 0x0057a740 -> int: once (byte +2). The original created a DirectSound device (DirectSoundCreate
//      through the import stub 0x007a9390 into +0x281, then SetCooperativeLevel(hwnd [0xa50090], DSSCL_NORMAL)) for
//      the game's 2D wave player. Every sound plays through OpenAL; until cleanup C2 the import was answered by a null
//      device (the former lib/openal/legacy_audio.cpp) that succeeded (DS_OK, a device object in +0x281,
//      SetCooperativeLevel DS_OK) and refused every buffer (CreateSoundBuffer DSERR_NODRIVER). Now no device is made:
//      the function leaves what that successful create left - byte +2 = 1, returns 1 (so Audio::init still opens
//      RESOURCE\A_WAVE2.RES and allocates the 0x8000-byte wave buffer) - except that +0x281 is not written (nothing
//      reads it any more). The player's functions further down answer "no buffer" where their CreateSoundBuffer
//      failed, so no DirectSound buffer is ever made and every buffer slot (+0x285 / +0x309 / +0x352) stays null.
namespace {
namespace game_b96_05 {


JOAN_COUNTER(c_ds_create, "DSound::create");

struct H {
    // 0x00578ce0
    H* slots_ctor() {
        jo::self<jo::Streamer>(this)->thread = -1;
        jo::self<jo::Streamer>(this)->file = invalid_handle();
        std::memset(static_cast<void*>(jo::self<jo::Streamer>(this)->sector_size_cache), 0, 0x68u);                        // memset
        jo::view<std::uint32_t>(jo::self<jo::Streamer>(this)->mode) = 0;
        return this;
    }

    // 0x0057a740
    std::int32_t ds_create() {
        c_ds_create.hit();
        // (DirectSoundCreate + SetCooperativeLevel: no device, see above; the null device always succeeded)
        jo::view<std::uint8_t>(jo::self<jo::Audio>(this)->wav_init_flag) = 1;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("SoundSlots::ctor", 0x00578ce0, &H::slots_ctor);
B("DSound::create", 0x0057a740, &H::ds_create);
#undef B

}  // namespace game_b96_05
}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
//
//    - Obj::dtor_578d40 0x00578d40: 0x00579040 on this (callee's eax passed through).
//
//    - Obj::ctor_57a340 0x0057a340: 0x00417980 on +0x17, 0x004179f0 on +0x14c, byte [0] = 0
//      (SEH frame: normal path).
namespace {
namespace game_b95_05 {


struct H {
    // 0x00578d40
    std::uintptr_t dtor_578d40() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_streamer(this)->shutdown()); }  // 0x00579040

    // 0x0057a340
    void* ctor_57a340() {
        joan::orig::as_anim_set(&jo::self<jo::Audio>(this)->wav_res)->construct();   // 0x00417980
        joan::orig::as_res_idx(&jo::self<jo::Audio>(this)->wav_res2)->construct();  // 0x004179f0
        jo::view<std::uint8_t>(jo::self<jo::Audio>(this)->init_flag) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("AsyncFileLoader::~AsyncFileLoader", 0x00578d40, &H::dtor_578d40);
B("Obj::ctor_57a340", 0x0057a340, &H::ctor_57a340);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
//
//    - PalStream::init 0x00578d70: probes RESOURCE\PAL_STD.RES with ReadFileEx (mode 2) or
//      _beginthread (mode 1), then sets up the critical section / events / worker thread.
//
//    - PalStream::pump 0x00579fa0 -> int: takes queued items under the critical section.
namespace {
namespace game_b97_05 {

using joan::call::stdcall_import;

JOAN_COUNTER(c_pal_init, "PalStream::init");
JOAN_COUNTER(c_pal_pump, "PalStream::pump");

struct H {
    // 0x00578d70
    void pal_init() {
        c_pal_init.hit();
        joan::orig::as_streamer(this)->shutdown();  // 0x00579040
        if (jo::self<jo::Streamer>(this)->thread == -1) {
            global<std::uint32_t>(d::g_a642d0) = 0;
            global<std::uint32_t>(d::g_a642d4) = 0;
            void* const h = stdcall_import<void*>(d::iat_KERNEL32_CreateFileA, str(d::s_RESOURCE_PAL_STD_RES), 0x80000000u, 1u, static_cast<void*>(nullptr),
                                                  3u, 0x40000080u, static_cast<void*>(nullptr));  // CreateFileA
            const std::int32_t ok = stdcall_import<std::int32_t>(d::iat_KERNEL32_ReadFileEx, h, obj(d::g_a642dc), 4u, obj(d::g_a642c8),
                                                                 fn::code_ptr(fn::Io_completion_stub));  // ReadFileEx
            if (ok != 0 || stdcall_import<std::uint32_t>(d::iat_KERNEL32_GetLastError) == 0x3e5u) {  // GetLastError
                jo::self<jo::Streamer>(this)->thread = 2;
            }
            stdcall_import<std::int32_t>(d::iat_KERNEL32_CloseHandle, h);  // CloseHandle
        }
        if (jo::self<jo::Streamer>(this)->thread == -1) {
            const std::int32_t r = joan::orig::arg::as<std::int32_t>(joan::orig::libc::_beginthread(joan::orig::arg::mptr(fn::code_ptr(fn::Thread_proc_endthread)), 0x1000u, joan::orig::arg::mptr(static_cast<void*>(nullptr))));
            if (r != -1) jo::self<jo::Streamer>(this)->thread = 1;
        }
        if (jo::self<jo::Streamer>(this)->thread == -1) jo::self<jo::Streamer>(this)->thread = 0;
        if (jo::self<jo::Streamer>(this)->thread == 2) {
            jo::view<std::uint32_t>(jo::self<jo::Streamer>(this)->active_count) = 0;
            std::memset(static_cast<void*>(jo::self<jo::Streamer>(this)->sector_size_cache), 0, 0x68u);                        // memset
            stdcall_import(d::iat_KERNEL32_InitializeCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));               // InitializeCriticalSection
            jo::self<jo::Streamer>(this)->tail = nullptr;
            jo::self<jo::Streamer>(this)->head = nullptr;
        }
        if (jo::self<jo::Streamer>(this)->thread == 1) {
            stdcall_import(d::iat_KERNEL32_InitializeCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));  // InitializeCriticalSection
            jo::self<jo::Streamer>(this)->current = nullptr;
            jo::self<jo::Streamer>(this)->tail = nullptr;
            jo::self<jo::Streamer>(this)->head = nullptr;
            for (void** ev : {&jo::self<jo::Streamer>(this)->event_wake, &jo::self<jo::Streamer>(this)->event_done, &jo::self<jo::Streamer>(this)->event_38,
                              &jo::self<jo::Streamer>(this)->event_3c, &jo::self<jo::Streamer>(this)->event_finished}) {
                *ev = stdcall_import<void*>(d::iat_KERNEL32_CreateEventA, static_cast<void*>(nullptr), 0, 0,
                                            static_cast<void*>(nullptr));  // CreateEventA
            }
            jo::self<jo::Streamer>(this)->u32_2c =
                joan::orig::arg::as<std::uint32_t>(joan::orig::libc::_beginthread(joan::orig::arg::mptr(fn::code_ptr(fn::Thread_proc_579810)), 0u, joan::orig::arg::mptr(static_cast<void*>(this))));  // _beginthread
        }
        jo::view<std::uint32_t>(jo::self<jo::Streamer>(this)->mode) = 1;
    }

    // 0x00579fa0
    std::int32_t pal_pump() {
        c_pal_pump.hit();
        for (;;) {
            stdcall_import(d::iat_KERNEL32_EnterCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));  // EnterCriticalSection
            void* const item = jo::view<void*>(jo::self<jo::Streamer>(this)->tail);
            if (item == nullptr) {
                stdcall_import(d::iat_KERNEL32_LeaveCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));  // LeaveCriticalSection
                return 0;
            }
            joan::orig::as_streamer(this)->remove(joan::orig::arg::mptr(item));              // 0x0057a220
            stdcall_import(d::iat_KERNEL32_LeaveCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));  // LeaveCriticalSection
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(item)->begin_read()) != 0) {         // 0x00578540
                joan::orig::as_streamer(this)->add_active();               // 0x00579f40
                return 1;
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("AsyncFileLoader::init", 0x00578d70, &H::pal_init);
B("PalStream::pump", 0x00579fa0, &H::pal_pump);
#undef B

}  // namespace game_b97_05
}  // namespace

// ---------------------------------------- from modules/game_b105_15.cpp (game/b105_15)
//
//      0x00578fd0 thread_proc_endthread   (cdecl, _beginthread proc at 0x578e16) - _endthread.
//
//      0x00578ff0 thread_proc_579810      (cdecl, _beginthread proc at 0x578f32) - arg->0x00579810,
//                 then _endthread (0x7b44ed, never returns).
//
//      0x00579020 io_completion_stub      (stdcall, ret 0xc, passed to the import 0xa6d938) - empty.
namespace {
namespace game_b105_15 {

// 0x00578fd0: thread procedure that only ends the thread.
void JOAN_CDECL thread_endthread(void*) { joan::orig::libc::_endthread(); }  // _endthread
// 0x00578ff0: thread procedure: arg->0x00579810(), then _endthread.
void JOAN_CDECL thread_579810(void* arg) {
    joan::orig::as_sound_stream(arg)->thread_loop();  // 0x00579810
    joan::orig::libc::_endthread();    // _endthread
}
// 0x00579020: completion routine, empty.
void JOAN_STDCALL io_completion_stub(u32, u32, void*) {}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("Thread::proc_endthread", 0x00578fd0, &thread_endthread);
B("Thread::proc_579810", 0x00578ff0, &thread_579810);
B("Io::completion_stub", 0x00579020, &io_completion_stub);
#undef B

}  // namespace game_b105_15
}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
//
//    - Stream::requeue 0x005795b0 (item, priority), ret 8: unlinks the item (when queued)
//      and re-inserts it in the priority-sorted list (head +0x20, tail +0x24) under the
//      critical section at +8; always returns 0.
namespace {
namespace game_b98_05 {


JOAN_COUNTER(c_requeue, "Stream::requeue");

struct H {
    // 0x005795b0
    std::int32_t requeue(std::uint8_t* item, std::int32_t priority) {
        c_requeue.hit();
        if (jo::as_file_loader(item)->queued == 0) return 0;
        using CsFn = void(JOAN_STDCALL*)(void*);
        reinterpret_cast<CsFn>(global<std::uintptr_t>(d::iat_KERNEL32_EnterCriticalSection))(&jo::self<jo::Streamer>(this)->lock);  // EnterCriticalSection
        if (jo::as_file_loader(item)->queued != 0) {
            if (jo::view<std::uint8_t*>(jo::as_file_loader(item)->next) != nullptr) {
                jo::view<std::uint8_t*>(jo::as_file_loader(item)->next->prev) = jo::view<std::uint8_t*>(jo::as_file_loader(item)->prev);
            } else {
                jo::view<std::uint8_t*>(jo::self<jo::Streamer>(this)->tail) = jo::view<std::uint8_t*>(jo::as_file_loader(item)->prev);
            }
            if (jo::view<std::uint8_t*>(jo::as_file_loader(item)->prev) != nullptr) {
                jo::view<std::uint8_t*>(jo::as_file_loader(item)->prev->next) = jo::view<std::uint8_t*>(jo::as_file_loader(item)->next);
            } else {
                jo::view<std::uint8_t*>(jo::self<jo::Streamer>(this)->head) = jo::view<std::uint8_t*>(jo::as_file_loader(item)->next);
            }
            jo::as_file_loader(item)->next = nullptr;
            jo::as_file_loader(item)->prev = nullptr;
            jo::view<std::uint32_t>(jo::as_file_loader(item)->queued) = 0;
        }
        std::uint8_t* prev;
        std::uint8_t* next;
        if (priority == jo::as_file_loader(item)->priority) {
            prev = nullptr;
            next = jo::view<std::uint8_t*>(jo::self<jo::Streamer>(this)->tail);
            while (next != nullptr && jo::as_file_loader(item)->priority > jo::as_file_loader(next)->priority) {
                prev = next;
                next = jo::view<std::uint8_t*>(jo::as_file_loader(next)->prev);
            }
        } else {
            // Scans from the tail using the item's previous priority (as the original).
            prev = jo::view<std::uint8_t*>(jo::self<jo::Streamer>(this)->head);
            next = nullptr;
            while (prev != nullptr && jo::as_file_loader(item)->priority < jo::as_file_loader(prev)->priority) {
                next = prev;
                prev = jo::view<std::uint8_t*>(jo::as_file_loader(prev)->next);
            }
        }
        jo::view<std::uint8_t*>(jo::as_file_loader(item)->next) = prev;
        if (prev != nullptr) {
            jo::view<std::uint8_t*>(jo::as_file_loader(prev)->prev) = item;
        } else {
            jo::view<std::uint8_t*>(jo::self<jo::Streamer>(this)->tail) = item;
        }
        jo::view<std::uint8_t*>(jo::as_file_loader(item)->prev) = next;
        if (next != nullptr) {
            jo::view<std::uint8_t*>(jo::as_file_loader(next)->next) = item;
        } else {
            jo::view<std::uint8_t*>(jo::self<jo::Streamer>(this)->head) = item;
        }
        jo::view<std::uint32_t>(jo::as_file_loader(item)->queued) = 1;
        jo::as_file_loader(item)->priority = priority;
        reinterpret_cast<CsFn>(global<std::uintptr_t>(d::iat_KERNEL32_LeaveCriticalSection))(&jo::self<jo::Streamer>(this)->lock);  // LeaveCriticalSection
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("Stream::requeue", 0x005795b0, &H::requeue);
#undef B

}  // namespace game_b98_05
}  // namespace

// ---------------------------------------- from modules/game_b104_08.cpp (game/b104_08)
//
//    - SoundStream::thread_loop 0x00579810 (ret 0): WaitForMultipleObjectsEx on +0x30/+0x38/+0x3c,
//      (re)opens the current sound's file (+0x28), size / seek / ReadFile under the critical
//      section +8, reports through SoundThread::finish 0x00579bc0, SetEvent +0x40 at the end.
//
//    (Audio::play_oneshot 0x0057ab80, the 2D player's named-resource sound, had no references: deleted in cleanup C2.)
namespace {
namespace game_b104_08 {

constexpr img::Addr kWaitForMultipleObjectsEx = d::iat_KERNEL32_WaitForMultipleObjectsEx;
constexpr img::Addr kDebugBreak = d::iat_KERNEL32_DebugBreak;
constexpr img::Addr kSetFilePointer = d::iat_KERNEL32_SetFilePointer;
constexpr img::Addr kReadFile = d::iat_KERNEL32_ReadFile;

JOAN_COUNTER(c_thread_loop, "SoundStream::thread_loop");

struct H {
    // ---- 0x00579810 ----
    void finish(u32 a, u32 b) { joan::orig::as_streamer(this)->finish_active(joan::orig::arg::u32(a), joan::orig::arg::u32(b)); }  // 0x00579bc0
    void open_and_read() {
        if (jo::self<jo::Streamer>(this)->file != invalid_handle()) {
            win_call<i32>(kCloseHandle, jo::self<jo::Streamer>(this)->file);
            jo::self<jo::Streamer>(this)->file = invalid_handle();
        }
        jo::self<jo::Streamer>(this)->file = win_call<void*>(kCreateFileA, jo::view<const char*>(jo::self<jo::Streamer>(this)->current), 0x80000000u, 1u,
                                         static_cast<void*>(nullptr), 3u, 0x80u, static_cast<void*>(nullptr));
        if (jo::self<jo::Streamer>(this)->file == invalid_handle()) {
            const u32 err = win_call<u32>(kGetLastError);
            finish(err, 0);
        }
        if (jo::self<jo::Streamer>(this)->file != invalid_handle() && jo::view<i32>(jo::self<jo::Streamer>(this)->current->size) == -1) {
            u32 size = win_call<u32>(kGetFileSize, jo::self<jo::Streamer>(this)->file, static_cast<void*>(nullptr));
            if (size != 0xffffffffu) {
                size -= jo::self<jo::Streamer>(this)->current->offset;
                if (jo::view<i8>(jo::self<jo::Streamer>(this)->current->own_buffer) != 0) {
                    joan::orig::as_file_loader(jo::view<u8*>(jo::self<jo::Streamer>(this)->current))->allocate(joan::orig::arg::u32(size));  // 0x00578280
                } else {
                    jo::self<jo::Streamer>(this)->current->size = size;
                }
            } else {
                const u32 err = win_call<u32>(kGetLastError);
                finish(err, 0);
            }
        }
        if (jo::self<jo::Streamer>(this)->file != invalid_handle()) {
            if (jo::self<jo::Streamer>(this)->current->offset != 0) {
                win_call<u32>(kSetFilePointer, jo::self<jo::Streamer>(this)->file, jo::self<jo::Streamer>(this)->current->offset,
                              static_cast<void*>(nullptr), 0u);
            }
            u32 got = 0;
            const i32 ok = win_call<i32>(kReadFile, jo::self<jo::Streamer>(this)->file, jo::view<void*>(jo::self<jo::Streamer>(this)->current->buffer),
                                         jo::self<jo::Streamer>(this)->current->size, &got, static_cast<void*>(nullptr));
            if (ok != 0) {
                finish(0, got);
            } else {
                const u32 err = win_call<u32>(kGetLastError);
                finish(err, 0);
            }
        }
    }
    i32 thread_loop() {
        c_thread_loop.hit();
        for (;;) {
            if (jo::self<jo::Streamer>(this)->u32_44 != 0 && jo::self<jo::Streamer>(this)->current == nullptr) break;
            const u32 n = 3;
            void* handles[3] = {jo::self<jo::Streamer>(this)->event_wake, jo::self<jo::Streamer>(this)->event_38, jo::self<jo::Streamer>(this)->event_3c};
            const u32 r = win_call<u32>(kWaitForMultipleObjectsEx, n, static_cast<void*>(handles), 0, 0xffffffffu, 1);
            if (r < n) {
                if (r == 0) {
                    win_call<void>(kEnterCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));
                    alignas(4) u8 lock[0x194];
                    joan::orig::as_string(lock)->construct();  // 0x005a1170 (byte [0] = 0)
                    if (jo::view<i8>(jo::self<jo::Streamer>(this)->current->disabled) == 0) open_and_read();
                    win_call<void>(kLeaveCriticalSection, static_cast<void*>(&jo::self<jo::Streamer>(this)->lock));
                    joan::orig::as_streamer(this)->take_pending();  // 0x00579ce0
                } else if (r == 1) {
                } else if (r == 2) {
                    if (jo::self<jo::Streamer>(this)->current == nullptr) break;
                    jo::self<jo::Streamer>(this)->u32_44 = 1;
                }
            } else {
                if (r >= 0x80u && r < n + 0x80u) {
                    win_call<void>(kDebugBreak);
                    break;
                }
                if (r == 0xc0u) {
                    if (jo::self<jo::Streamer>(this)->u32_44 != 0) break;
                    joan::orig::as_streamer(this)->take_pending();  // 0x00579ce0
                } else if (r == 0xffffffffu) {
                    break;
                }
            }
        }
        return win_call<i32>(kSetEvent, jo::self<jo::Streamer>(this)->event_finished);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_08", name, addr, fn)
B("AsyncFileLoader::process_thread", 0x00579810, &H::thread_loop);
#undef B

}  // namespace game_b104_08
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
//
//    - SoundThread::finish 0x00579bc0 (a, b), ret 8: under the critical section +8 detaches the
//      current sound +0x28, SetEvent +0x34, Sound 0x00578340 with 0 (after +0x118 = b) or 1 / 2,
//      CloseHandle +0x48, SetEvent +0x3c when +0x44.
//
//  - 0x00579bc0 with a == 0 and b == 0 dereferences the just-cleared +0x28 (+0x110 of null) in the
//    original; reproduced with volatile reads (same access violation if ever reached).
namespace {
namespace game_b100_10 {


JOAN_COUNTER(c_finish, "SoundThread::finish");

struct H {

    // ------------------------------------------------------------------------ 0x00579bc0
    void finish(u32 a, u32 b) {
        c_finish.hit();
        u8* const cs = reinterpret_cast<u8*>(&jo::self<jo::Streamer>(this)->lock);
        win_call<u32>(kEnterCriticalSection, cs);
        if (jo::self<jo::Streamer>(this)->current != nullptr) {
            u8* const snd = jo::view<u8*>(jo::self<jo::Streamer>(this)->current);
            jo::self<jo::Streamer>(this)->current = nullptr;
            win_call<i32>(kSetEvent, jo::self<jo::Streamer>(this)->event_done);
            bool direct = false;
            if (a == 0) {
                if (b != 0) {
                    direct = true;
                } else {
                    // The original re-reads the just-cleared +0x28 here (null + 0x110).
                    const std::uintptr_t p = *reinterpret_cast<volatile std::uintptr_t*>(&jo::self<jo::Streamer>(this)->current);
                    direct = *reinterpret_cast<volatile u32*>(p + offsetof(jo::FileLoader, offset)) == 0;
                }
            }
            if (direct) {
                jo::as_file_loader(snd)->bytes_read = b;
                joan::orig::as_file_loader(snd)->set_error(static_cast<char>(joan::orig::arg::i32(0u)));  // 0x00578340
            } else {
                const u32 mode = (a == 2) ? 1u : 2u;
                joan::orig::as_file_loader(snd)->set_error(static_cast<char>(joan::orig::arg::i32(mode)));  // 0x00578340
            }
            if (jo::self<jo::Streamer>(this)->file != invalid_handle()) {
                win_call<i32>(kCloseHandle, jo::self<jo::Streamer>(this)->file);
                jo::self<jo::Streamer>(this)->file = invalid_handle();
            }
            if (jo::self<jo::Streamer>(this)->u32_44 != 0) win_call<i32>(kSetEvent, jo::self<jo::Streamer>(this)->event_3c);
        }
        win_call<u32>(kLeaveCriticalSection, cs);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("SoundThread::finish", 0x00579bc0, &H::finish);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
//
//    - Stream::take_pending 0x00579ce0 -> int (critical section at +8, SetEvent +0x30).
//
//    - Sound::attenuation 0x0057c960 (int), ret 4: +0x34d * v - 10000 clamped to [-10000, 0].
namespace {
namespace game_b94_05 {


JOAN_COUNTER(c_take_pending, "Stream::take_pending");

struct H {
    // 0x00579ce0
    std::int32_t take_pending() {
        c_take_pending.hit();
        std::int32_t result = 0;
        if (jo::view<std::int32_t>(jo::self<jo::Streamer>(this)->u32_44) != 0) return result;
        using CsFn = void(JOAN_STDCALL*)(void*);
        reinterpret_cast<CsFn>(global<std::uintptr_t>(d::iat_KERNEL32_EnterCriticalSection))(&jo::self<jo::Streamer>(this)->lock);  // EnterCriticalSection
        if (jo::self<jo::Streamer>(this)->current == nullptr && jo::self<jo::Streamer>(this)->tail != nullptr) {
            std::uint8_t* const item = jo::view<std::uint8_t*>(jo::self<jo::Streamer>(this)->tail);
            jo::view<std::uint8_t*>(jo::self<jo::Streamer>(this)->current) = item;
            std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_file_loader(item)->prev);
            jo::view<std::uint8_t*>(jo::self<jo::Streamer>(this)->tail) = next;
            if (next != nullptr) {
                jo::as_file_loader(next)->next = nullptr;
            } else {
                jo::self<jo::Streamer>(this)->head = nullptr;
            }
            jo::as_file_loader(item)->prev = nullptr;
            jo::as_file_loader(item)->next = nullptr;
            jo::view<std::uint32_t>(jo::as_file_loader(item)->queued) = 0;
            jo::as_file_loader(item)->status = 3;
            result = 1;
        }
        reinterpret_cast<CsFn>(global<std::uintptr_t>(d::iat_KERNEL32_LeaveCriticalSection))(&jo::self<jo::Streamer>(this)->lock);  // LeaveCriticalSection
        if (result != 0) {
            using EvFn = std::int32_t(JOAN_STDCALL*)(void*);
            reinterpret_cast<EvFn>(global<std::uintptr_t>(d::iat_KERNEL32_SetEvent))(jo::self<jo::Streamer>(this)->event_wake);  // SetEvent
        }
        return result;
    }

    // 0x0057c960
    std::int32_t attenuation(std::int32_t v) {
        std::int32_t r = static_cast<std::int32_t>(jo::view<std::uint32_t>(jo::self<jo::Audio>(this)->wav_volume) * static_cast<std::uint32_t>(v) - 10000u);
        if (r > 0) {
            r = 0;
        } else if (r < -10000) {
            r = -10000;
        }
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("Stream::take_pending", 0x00579ce0, &H::take_pending);
B("Sound::attenuation", 0x0057c960, &H::attenuation);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
//
//    - Audio::init_57a440 0x0057a440 (ret 0, audio.cpp): returns 1.
namespace {
namespace game_b108_10 {

template <class T>
T& at(const void* p, i32 offset) {
    return *reinterpret_cast<T*>(static_cast<u8*>(const_cast<void*>(p)) + offset);
}

JOAN_COUNTER(c_audio_init, "Audio::init_57a440");

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x0057a440
    u32 audio_init() {
        c_audio_init.hit();
        jo::view<u8>(jo::self<jo::Audio>(this)->stream_pending) = 0;
        jo::view<u8>(jo::self<jo::Audio>(this)->mid_init_flag) = 0;
        jo::view<u8>(jo::self<jo::Audio>(this)->wav_init_flag) = 0;
        jo::view<u8>(jo::self<jo::Audio>(this)->cd_init_flag) = 0;
        jo::view<u8>(jo::self<jo::Audio>(this)->mid_flag) = 1;
        jo::view<u8>(jo::self<jo::Audio>(this)->wav_flag) = 1;
        jo::view<u8>(jo::self<jo::Audio>(this)->cd_flag) = 1;
        jo::self<jo::Audio>(this)->mid_buf = nullptr;
        jo::self<jo::Audio>(this)->wav_buf = nullptr;
        jo::view<u32>(jo::self<jo::Audio>(this)->mid_buf_size) = 0;
        jo::view<u32>(jo::self<jo::Audio>(this)->wav_buf_size) = 0;
        for (i32 i = 0; i < 8; ++i) {
            jo::self<jo::Audio>(this)->wav_dsb[i] = nullptr;
            jo::view<u32>(jo::self<jo::Audio>(this)->wav_serial_no[i]) = 0;
        }
        jo::view<u32>(jo::self<jo::Audio>(this)->max_lwav_serial_no) = 0;
        for (i32 i = 0; i < 4; ++i) {
            jo::self<jo::Audio>(this)->lwav_dsb[i] = nullptr;
            jo::view<u32>(jo::self<jo::Audio>(this)->lwav_serial_no[i]) = 0;
            jo::self<jo::Audio>(this)->lwav_file[i] = nullptr;
        }
        jo::view<u32>(jo::self<jo::Audio>(this)->max_lwav_serial_no) = 0;
        for (i32 i = 0; i < 4; ++i) {
            jo::self<jo::Audio>(this)->loop_dsb[i] = nullptr;
            jo::self<jo::Audio>(this)->loop_file[i] = nullptr;
        }
        jo::view<u32>(jo::self<jo::Audio>(this)->wav_volume) = 0x64;
        if (joan::orig::arg::as<i32>(joan::orig::as_audio(me())->create_dsound()) != 0) {  // 0x0057a740
            joan::orig::as_res_idx_l(&jo::self<jo::Audio>(this)->wav_res2)->init(str(d::s_RESOURCE_A_WAVE2_RES), static_cast<char>(i32{0}), static_cast<char>(i32{0}));  // 0x0059b920 "RESOURCE\\A_WAVE2.RES"
            jo::self<jo::Audio>(this)->wav_buf = joan::orig::arg::as<jo::WavHeader*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(i32{0x8000}), str(d::s_d_ww_ilib_audio_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a150e4)) + 0x2f)));  // 0x0058e7b0
            jo::view<u32>(jo::self<jo::Audio>(this)->wav_buf_size) = 0x8000;
        }
        // CD audio removed (the game plays its music from files): no MCI "cdaudio" device (CdAudio::open 0x0057a810)
        // and no saved CD-line mixer volume (Mixer::get_volume 0x0057ebf0) - as on a PC without a CD drive.
        jo::view<u8>(jo::self<jo::Audio>(this)->cd_init_flag) = 0;
        global<u32>(d::g_a150e0) = 0xffffffffu;  // "no saved volume"
        jo::view<u8>(jo::self<jo::Audio>(this)->init_flag) = (jo::view<i8>(jo::self<jo::Audio>(this)->wav_init_flag) != 0 || jo::view<i8>(jo::self<jo::Audio>(this)->cd_init_flag) != 0) ? 1 : 0;
        return 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("Audio::init_57a440", 0x0057a440, &H::audio_init);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b101_09.cpp (game/b101_09)
//
//    - Audio::stop_57a6c0 0x0057a6c0 (ret 0): when byte +0 is set: clears +0x351 / +0, 0x0057e9d0
//      ([0xa150e0]) unless -1, then 0x0057a920, 0x0057a9c0, 0x0057a8d0 (returns its eax; this
//      when idle).
namespace {
namespace game_b101_09 {


JOAN_COUNTER(c_audio_stop, "Audio::stop_57a6c0");

struct H {

    // 0x0057a6c0
    std::uintptr_t audio_stop() {  // returns a pointer-sized eax leftover (the facade returns std::uintptr_t)
        c_audio_stop.hit();
        if (jo::view<i8>(jo::self<jo::Audio>(this)->init_flag) == 0) return reinterpret_cast<std::uintptr_t>(this);
        jo::view<u8>(jo::self<jo::Audio>(this)->stream_pending) = 0;
        jo::view<u8>(jo::self<jo::Audio>(this)->init_flag) = 0;
        // (the CD-line volume restore 0x0057e9d0 and the CD closes 0x0057a9c0 / 0x0057a8d0: CD audio removed)
        joan::orig::as_audio(this)->release_dsound();                // 0x0057a920
        return 0;  // callers (0x004d7890, 0x0057a3c0 and their replacements) discard the result; CdAudio::close returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_09", name, addr, fn)
B("Audio::stop_57a6c0", 0x0057a6c0, &H::audio_stop);
#undef B

}  // namespace game_b101_09
}  // namespace

// ---------------------------------------- from modules/game_b103_07.cpp (game/b103_07)
//
//    - 0x0057a7d0 Obj::mark_1                 (thiscall, ret 0) - byte +1 = 1, returns 1.
//
//    - 0x0057c9c0 Sound::volume_percent(v)    (thiscall, ret 4) - clamp((v+10000)/+0x34d).
//
//  - Almost everything here has no reference at all (dead code): all ByteVal / Turn helpers,
//    0x005706a0, 0x0057a7d0, 0x0057c9c0, 0x0057e*, all bitmap helpers. Live: the four static
//    initialisers, the atexit dtor and the error callback.
namespace {
namespace game_b103_07 {


struct H {

    // ------------------------------------------------------------ misc
    // 0x0057a7d0
    i32 mark_1() {
        if (jo::view<i8>(jo::self<jo::Audio>(this)->mid_init_flag) == 0) jo::view<u8>(jo::self<jo::Audio>(this)->mid_init_flag) = 1;
        return 1;
    }
    // 0x0057c9c0
    i32 volume_percent(i32 v) {
        if (jo::self<jo::Audio>(this)->wav_volume == 0) return 0;
        i32 r = static_cast<i32>(static_cast<u32>(v) + 10000u) / jo::self<jo::Audio>(this)->wav_volume;  // add wraps
        if (r < 0) {
            r = 0;
        } else if (r > 100) {
            r = 100;
        }
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("Obj::mark_1_57a7d0", 0x0057a7d0, &H::mark_1);
B("Sound::volume_percent", 0x0057c9c0, &H::volume_percent);
#undef B

}  // namespace game_b103_07
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
//
//    - wav_find_data 0x0057b030 (cdecl, no thunk): "data" chunk after the fmt chunk, 0 when
//      past 0x80 bytes.
//
//    (Obj::scalar_dtor 0x0057c700, the deleting destructor of the 2D player's stream files, lost its callers with
//    the player's buffer code in cleanup C2 and went.)
namespace {
namespace game_b90_04 {

std::int32_t strncmp_(const void* a, const void* b, std::uint32_t n) { return joan::orig::arg::as<std::int32_t>(joan::orig::libc::strncmp(joan::orig::arg::str(a), joan::orig::arg::str(b), joan::orig::arg::u32(n))); }

JOAN_COUNTER(c_wav_data, "wav_find_data");

// 0x0057b030: RIFF/WAVE - skips the chunk at +0x10 (its size at +0x10), then walks chunks
// (strncmp(p, "data", 4)) until "data"; 0 once p - base > 0x80 (signed).
void* JOAN_CDECL wav_find_data(std::uint8_t* base) {
    c_wav_data.hit();
    std::uint8_t* p = base + 0x10;
    std::int32_t n = *reinterpret_cast<std::int32_t*>(p);
    p = p + n + 4;
    for (;;) {
        if (strncmp_(p, &img::global<const char>(d::s_data_9d876c), 4u) == 0) return p;  // "data"
        p = p + 4;
        n = *reinterpret_cast<std::int32_t*>(p);
        if (static_cast<std::int32_t>(p - base) > 0x80) return nullptr;
        p = p + n + 4;
    }
}

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("wav_find_data", 0x0057b030, &wav_find_data);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b111_10.cpp (fix/uninit)
//
//      0x0057b0d0 Audio::play_resource(key, vol, pan) (thiscall, ret 0xc) - the 2D player's sound by resource key
//
//  - The original reads the wave's header from RESOURCE\A_WAVE2.RES (+0x14c) into the wave buffer, seeks the
//    resource to its data and then asks the DirectSound device for a buffer (CreateSoundBuffer), which never
//    succeeded (null device, see DSound::create 0x0057a740): returns 0 ("no sound") there. Since cleanup C2 the
//    function ends at that point; what came after (free-slot search with GetStatus - the uninitialised status this
//    fix/uninit entry fixed -, Lock / Unlock / SetVolume / SetPan / Play, the duration and serial number) went, with
//    0x0057bbf0 Audio::buffer_playing (no references). Registered with its former fix/uninit key.
namespace {
namespace game_b111_10 {

i32 sx(u16 v) { return static_cast<i32>(static_cast<i16>(v)); }

JOAN_COUNTER(c_play_resource, "Audio::play_resource");

struct H {

    // ------------------------------------------------------------------ 0x0057b0d0
    u32 play_resource(u32 key, i32 /*volume*/, i32 /*pan*/) {
        c_play_resource.hit();
        if (jo::view<i8>(jo::self<jo::Audio>(this)->wav_init_flag) == 0 || jo::view<i8>(jo::self<jo::Audio>(this)->wav_flag) == 0) return 0;
        u32 unused = 0;  // [ebp-0x8], written by 0x0059c2a0, never read
        void* const file = thiscall<void*>(fn::get_file_rec_59c2a0, &jo::self<jo::Audio>(this)->wav_res2, sx(static_cast<u16>(key)),
                                           &unused);  // 0x0059c2a0
        if (file == nullptr) return 0;
        if (joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(jo::self<jo::Audio>(this)->wav_buf), joan::orig::arg::u32(i32{0x80})) == 0) return 0;  // 0x005873a0
        u8* const chunk = joan::orig::arg::as<u8*>(joan::orig::wav_find_data(joan::orig::arg::mptr(jo::self<jo::Audio>(this)->wav_buf)));
        if (chunk == nullptr) return 0;
        const i32 offset = static_cast<i32>(chunk - reinterpret_cast<u8*>(jo::self<jo::Audio>(this)->wav_buf) + 8);
        joan::orig::as_file(file)->file_seek(joan::orig::arg::i32(offset - 0x80), i32{1});  // 0x00587ea0 seek
        return 0;  // CreateSoundBuffer: no DirectSound device
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1, "widths");

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
FIX("Audio::play_resource_57b0d0", 0x0057b0d0, &H::play_resource);
#undef FIX

}  // namespace game_b111_10
}  // namespace

// ---------------------------------------- from modules/game_b111_14.cpp (fix/uninit)
//
//    - 0x0057b580 Audio::play_memory(wav, vol, pan) (thiscall, ret 0xc): the 2D player's sound from a wave in
//      memory. The original finds the wave's "data" chunk (0x0057b030, reads only), takes the time and asks the
//      DirectSound device for a buffer (CreateSoundBuffer), which never succeeded (null device, see DSound::create
//      0x0057a740): it returns 0 ("no sound") on every path. Since cleanup C2 it returns 0 at once; the free-slot
//      search with its unchecked GetStatus (the uninitialised read this fix/uninit entry fixed), Lock / Unlock /
//      SetVolume / SetPan / Play went. Registered with its former fix/uninit key.
namespace {
namespace game_b111_14 {

JOAN_COUNTER(c_play, "Audio::play_memory_57b580");

struct H {
    // 0x0057b580
    u32 play_memory(u8* /*wav*/, u32 /*vol*/, u32 /*pan*/) {
        c_play.hit();
        return 0;  // CreateSoundBuffer: no DirectSound device
    }
};

JOAN_REPLACE("fix/uninit", "Audio::play_memory", 0x0057b580, &H::play_memory);

}  // namespace game_b111_14
}  // namespace

// ---------------------------------------- from modules/game_b111_16.cpp (fix/uninit)
//
//      0x0057b9c0 SoundSys::reap_buffers_57b9c0 (thiscall, ret 0) -> count of free 2D-player slots. The original
//                 releases the buffers of the 8 slots +0x285 that stopped playing (GetStatus, whose DWORD this
//                 fix/uninit entry initialised) and counts the empty ones. No buffer is ever made (see DSound::create
//                 0x0057a740): every slot is empty, so it returns 8 (its time stamp was only used for a filled slot).
//                 Registered with its former fix/uninit key.
namespace {
namespace game_b111_16 {

JOAN_COUNTER(c_reap, "SoundSys::reap_buffers_57b9c0");

struct H {
    // -------------------------------------------------------------- 0x0057b9c0
    i32 reap_buffers() {
        c_reap.hit();
        return 8;  // 8 empty slots
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
F("SoundSys::reap_buffers_57b9c0", 0x0057b9c0, &H::reap_buffers);
#undef F

}  // namespace game_b111_16
}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
//
//    - SoundSys::release_buffers 0x0057dd30: the original stops and releases the 2D player's DirectSound buffers -
//      the 8 at +0x285, the 4 streams +0x309 (0x0057de40, with their files +0x32d) and the 4 loops +0x352
//      (0x0057df80(1..4), files +0x362) - when the wave player is on (bytes +2 and +5). No buffer is ever made (see
//      DSound::create 0x0057a740): every slot is null, nothing to release. Since cleanup C2 it does nothing
//      (0x0057de40 / 0x0057df80 had no other caller and went).
namespace {
namespace game_b98_05_2 {

JOAN_COUNTER(c_release_buffers, "SoundSys::release_buffers");

struct H {
    // 0x0057dd30
    void release_buffers() { c_release_buffers.hit(); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("SoundSys::release_buffers", 0x0057dd30, &H::release_buffers);
#undef B

}  // namespace game_b98_05_2
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
//
//      0x0057e1a0 Cd::stop              (thiscall, ret 0) - mciSendCommandA(dev, 0x808, 2, 0).
namespace {
namespace game_b93_03 {

using joan::hook::global;


using i8 = std::int8_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct H {

    void cd_stop() {}  // 0x0057e1a0 (CD audio removed: nothing plays)
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Cd::stop", 0x0057e1a0, &H::cd_stop);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
//
//      0x00579040 Streamer::shutdown()                         thiscall (KERNEL32 via IAT)
//
//      0x0057ab20 CdAudio::stop()                              thiscall (mciSendCommandA)
//
//      (0x0057ee10 next_id, the 2D player's serial numbers, lost its callers with the player's buffer code in
//      cleanup C2 and went.)
namespace {
namespace game_b91_02 {

using joan::hook::global;

using joan::call::thiscall;
using joan::orig::helpers::win_call;

JOAN_COUNTER(c_shutdown, "Streamer::shutdown");

struct H {
    std::uintptr_t self_bits() const { return reinterpret_cast<std::uintptr_t>(this); }

    // 0x00579040
    H* shutdown() {
        c_shutdown.hit();
        if (jo::self<jo::Streamer>(this)->mode == 0) return this;
        void* const cs = &jo::self<jo::Streamer>(this)->lock;
        if (jo::self<jo::Streamer>(this)->thread == 2) {
            win_call(d::iat_KERNEL32_EnterCriticalSection, cs);  // EnterCriticalSection
            for (void* p = jo::view<void*>(jo::self<jo::Streamer>(this)->head); p != nullptr;) {
                void* const next = jo::as_file_loader(p)->next;
                joan::orig::as_file_loader(p)->stop();  // 0x00578110
                p = next;
            }
            win_call(d::iat_KERNEL32_LeaveCriticalSection, cs);  // LeaveCriticalSection
            win_call(d::iat_KERNEL32_DeleteCriticalSection, cs);  // DeleteCriticalSection
        }
        if (jo::self<jo::Streamer>(this)->thread == 1) {
            win_call(d::iat_KERNEL32_EnterCriticalSection, cs);
            for (void* p = jo::view<void*>(jo::self<jo::Streamer>(this)->head); p != nullptr;) {
                void* const next = jo::as_file_loader(p)->next;
                joan::orig::as_file_loader(p)->stop();
                p = next;
            }
            if (jo::view<void*>(jo::self<jo::Streamer>(this)->current) != nullptr) joan::orig::as_file_loader(jo::view<void*>(jo::self<jo::Streamer>(this)->current))->stop();
            win_call(d::iat_KERNEL32_LeaveCriticalSection, cs);
            win_call(d::iat_KERNEL32_SetEvent, jo::self<jo::Streamer>(this)->event_3c);               // SetEvent
            win_call(d::iat_KERNEL32_WaitForSingleObject, jo::self<jo::Streamer>(this)->event_finished, 0x2710u);      // WaitForSingleObject
            win_call(d::iat_KERNEL32_DeleteCriticalSection, cs);
            win_call(d::iat_KERNEL32_CloseHandle, jo::self<jo::Streamer>(this)->event_wake);  // CloseHandle
            win_call(d::iat_KERNEL32_CloseHandle, jo::self<jo::Streamer>(this)->event_done);
            win_call(d::iat_KERNEL32_CloseHandle, jo::self<jo::Streamer>(this)->event_38);
            win_call(d::iat_KERNEL32_CloseHandle, jo::self<jo::Streamer>(this)->event_3c);
            win_call(d::iat_KERNEL32_CloseHandle, jo::self<jo::Streamer>(this)->event_finished);
            if (jo::self<jo::Streamer>(this)->file != invalid_handle()) {
                win_call(d::iat_KERNEL32_CloseHandle, jo::self<jo::Streamer>(this)->file);
                jo::self<jo::Streamer>(this)->file = invalid_handle();
            }
        }
        jo::self<jo::Streamer>(this)->mode = 0;
        return this;
    }

    // 0x0057ab20
    std::uintptr_t cd_stop() {  // CD audio removed: nothing to stop
        if (jo::view<std::int8_t>(jo::self<jo::Audio>(this)->mid_init_flag) == 0) return self_bits();
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("AsyncFileLoader::deinit", 0x00579040, &H::shutdown);
B("CdAudio::stop", 0x0057ab20, &H::cd_stop);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
namespace {
namespace game_leaves80a {


struct H {

    void* loader_ctor() {  // 0x00577be0
        std::memset(static_cast<void*>(this), 0, sizeof(jo::FileLoader));  // memset 0x164 (x86; the pointers are wider on x64)
        jo::self<jo::FileLoader>(this)->handle = invalid_handle();
        return this;
    }
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Loader::Loader", 0x00577be0, &H::loader_ctor);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/en3d_helpers37.cpp (en3d/helpers37)
namespace {
namespace en3d_helpers37 {

using joan::call::thiscall;


struct H {

    // 0x00577c30.
    void* reset_138() {
        joan::orig::as_file_loader(this)->stop();  // 0x00578110
        jo::self<jo::FileLoader>(this)->ovl_event = nullptr;
        return this;
    }
};

#define HELPER37(name, addr, fn) JOAN_REPLACE("en3d/helpers37", name, addr, fn)
HELPER37("reset_138", 0x00577c30, &H::reset_138);
#undef HELPER37

}  // namespace en3d_helpers37
}  // namespace

// ---------------------------------------- from modules/game_b103_07.cpp (game/b103_07)
namespace {
namespace game_b103_07_2 {

using u8 = std::uint8_t;
using i32 = std::int32_t;

using joan::call::thiscall;

struct H {

    // 0x0057e200
    i32 zero_57e200() { return 0; }
    // 0x0057e3e0 / 0x0057e420 / 0x0057e460
    void* enable_4(i32 on) {
        if (on == 0) joan::orig::as_audio(this)->cd_stop_57ab20();  // 0x0057ab20
        jo::view<u8>(jo::self<jo::Audio>(this)->mid_flag) = static_cast<u8>(on);
        return this;
    }
    void* enable_5(i32 on) {
        if (on == 0) joan::orig::as_audio(this)->release_buffers();  // 0x0057dd30
        jo::view<u8>(jo::self<jo::Audio>(this)->wav_flag) = static_cast<u8>(on);
        return this;
    }
    void* enable_6(i32 on) {
        if (on == 0) joan::orig::as_audio(this)->cd_stop();  // 0x0057e1a0
        jo::view<u8>(jo::self<jo::Audio>(this)->cd_flag) = static_cast<u8>(on);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("Obj::zero_57e200", 0x0057e200, &H::zero_57e200);
B("Obj::enable_4", 0x0057e3e0, &H::enable_4);
B("Obj::enable_5", 0x0057e420, &H::enable_5);
B("Obj::enable_6", 0x0057e460, &H::enable_6);
#undef B

}  // namespace game_b103_07_2
}  // namespace

// ---------------------------------------- from modules/game_b105_15.cpp (game/b105_15)
namespace {
namespace game_b105_15_2 {

using u32 = std::uint32_t;

struct H {
    // 0x0057e4a0
    void empty_ret4(u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("Obj::empty_57e4a0", 0x0057e4a0, &H::empty_ret4);
#undef B

}  // namespace game_b105_15_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::FileLoader::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves80a::H::loader_ctor>(this)); }  // 0x577be0
auto joan::orig::FileLoader::stop_and_reset() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers37::H::reset_138>(this)); }  // 0x577c30
auto joan::orig::FileLoader::set_path(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves80b::H::path_set>(this, a0)); }  // 0x577c70
auto joan::orig::FileLoader::start(void* a0, std::uint32_t a1, std::uint32_t a2, void* a3, void* a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers30::H::loader_start>(this, a0, a1, a2, a3, a4)); }  // 0x577d10
auto joan::orig::FileLoader::stop() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_stream1::H::stop>(this)); }  // 0x578110
auto joan::orig::FileLoader::done() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_stream1::H::done>(this)); }  // 0x578250
auto joan::orig::FileLoader::allocate(std::uint32_t a0) -> void { joan::bind::member<&::game_stream1::H::allocate>(this, a0); }  // 0x578280
auto joan::orig::FileLoader::set_error(char a0) -> void { joan::bind::member<&::game_stream1::H::set_error>(this, a0); }  // 0x578340
auto joan::orig::Sound::set_11c(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_06::H::set_11c>(this, a0)); }  // 0x578390
auto joan::orig::FileLoader::begin_read() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_stream1::H::begin_read>(this)); }  // 0x578540
auto joan::orig::FileLoader::alloc_aligned(std::uint32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_stream2::H::alloc_aligned>(this, a0, a1)); }  // 0x578bd0
auto joan::orig::FileLoader::free_aligned() -> void { joan::bind::member<&::game_stream2::H::free_aligned>(this); }  // 0x578c70
auto joan::orig::Streamer::init() -> void { joan::bind::member<&::game_b97_05::H::pal_init>(this); }  // 0x578d70
auto joan::orig::Streamer::shutdown() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_02::H::shutdown>(this)); }  // 0x579040
auto joan::orig::Streamer::enqueue_signal(void* a0) -> void { joan::bind::member<&::game_stream1::H::enqueue_signal>(this, a0); }  // 0x579220
auto joan::orig::Streamer::cancel(void* a0) -> void { joan::bind::member<&::game_stream2::H::cancel>(this, a0); }  // 0x579440
auto joan::orig::Streamer::requeue(void* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_05::H::requeue>(this, a0, a1)); }  // 0x5795b0
auto joan::orig::SoundStream::thread_loop() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_08::H::thread_loop>(this)); }  // 0x579810
auto joan::orig::Streamer::finish_active(std::uint32_t a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b100_10::H::finish>(this, a0, a1); }  // 0x579bc0
auto joan::orig::Streamer::take_pending() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_05::H::take_pending>(this)); }  // 0x579ce0
auto joan::orig::Streamer::sector_size(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_stream2::H::sector_size>(this, a0)); }  // 0x579e10
auto joan::orig::Streamer::can_start() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_stream1::H::can_start>(this)); }  // 0x579f10
auto joan::orig::Streamer::add_active() -> void { joan::bind::member<&::game_stream1::H::add_active>(this); }  // 0x579f40
auto joan::orig::Streamer::dec_count_4c() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_06::H::dec_4c>(this)); }  // 0x579f70
auto joan::orig::Streamer::pump() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_05::H::pal_pump>(this)); }  // 0x579fa0
auto joan::orig::Streamer::enqueue(void* a0) -> void { joan::bind::member<&::game_stream1::H::enqueue>(this, a0); }  // 0x57a040
auto joan::orig::Streamer::remove(void* a0) -> void { joan::bind::member<&::game_stream2::H::remove>(this, a0); }  // 0x57a220
auto joan::orig::Audio::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_05::H::ctor_57a340>(this)); }  // 0x57a340
auto joan::orig::Audio::init() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_10::H::audio_init>(this)); }  // 0x57a440
auto joan::orig::Audio::shutdown() -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b101_09::H::audio_stop>(this)); }  // 0x57a6c0
auto joan::orig::Audio::create_dsound() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_05::H::ds_create>(this)); }  // 0x57a740
auto joan::orig::Audio::release_dsound() -> void { joan::bind::member<&::game_b99_06::H::shutdown>(this); }  // 0x57a920
auto joan::orig::Audio::cd_stop_57ab20() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_02::H::cd_stop>(this)); }  // 0x57ab20
auto joan::orig::wav_find_data(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b90_04::wav_find_data>(a0)); }  // 0x57b030
auto joan::orig::Audio::play_resource(std::uint32_t a0, std::int32_t a1, std::int32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_10::H::play_resource>(this, a0, a1, a2)); }  // 0x57b0d0
auto joan::orig::Audio::play_memory(void* a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_14::H::play_memory>(this, a0, a1, a2)); }  // 0x57b580
auto joan::orig::Audio::reap_buffers() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b111_16::H::reap_buffers>(this)); }  // 0x57b9c0
auto joan::orig::Sound::attenuation(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_05::H::attenuation>(this, a0)); }  // 0x57c960
auto joan::orig::Audio::release_buffers() -> void { joan::bind::member<&::game_b98_05_2::H::release_buffers>(this); }  // 0x57dd30
auto joan::orig::Audio::cd_stop() -> void { joan::bind::member<&::game_b93_03::H::cd_stop>(this); }  // 0x57e1a0
