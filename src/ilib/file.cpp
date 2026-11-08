// Reimplementation of the engine's File class (Win32 file I/O wrapper),
// joan.exe 0x00586da0-0x00587f60. Decompiled from the unoptimised original;
// behaviour is kept 1:1, including its quirks (noted inline). Two fixed-size
// stack buffers that could overflow in the original are bounds-checked here.
//
// These functions have side effects (file position), so the former verify mode
// could not run original and new side by side; they are covered by the call
// counters and by the game simply working.
#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include "../platform/win32_types.hpp"

#include "../platform/files.hpp"

#include <cstdio>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../original/engine.hpp"
#include "../original/file.hpp"
#include "../registry.hpp"
#include "../runtime/save_audit.hpp"
#include "../original/record_array.hpp"
#include <cstdint>
#include "../fn_names.hpp"
#include "../original/dynarray.hpp"
#include <cstdarg>
#include "../original/vecmath.hpp"
#include "../x87.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

namespace {
JOAN_COUNTER(c_open, "File::file_open");
JOAN_COUNTER(c_create, "File::file_create");
JOAN_COUNTER(c_close, "File::file_close");
JOAN_COUNTER(c_destruct, "File::~File");
JOAN_COUNTER(c_write, "File::file_write");
JOAN_COUNTER(c_read, "File::file_read");
JOAN_COUNTER(c_write_array, "File::file_write_array");
JOAN_COUNTER(c_read_array, "File::file_read_array");
JOAN_COUNTER(c_put_short, "File::file_put_short");
JOAN_COUNTER(c_get_short, "File::file_get_short");
JOAN_COUNTER(c_put_ushort, "File::file_put_unsigned_short");
JOAN_COUNTER(c_get_ushort, "File::file_get_unsigned_short");
JOAN_COUNTER(c_put_long, "File::file_put_long");
JOAN_COUNTER(c_get_long, "File::file_get_long");
JOAN_COUNTER(c_seek, "File::file_seek");
JOAN_COUNTER(c_pos, "File::file_pos");
JOAN_COUNTER(c_size, "File::file_size");

// Directories file_open() tries in turn (global array at 0x00a1579c, 1 entry: "").
constexpr int kSearchDirCount = 1;
const char* search_dir(int i) { return joan::hook::global<const char* const[kSearchDirCount]>(d::ptrs_a1579c)[i]; }

template <class T>
bool write_raw(joan::orig::File* f, const T& value) {
    DWORD written = 0;
    if (joan::platform::files::WriteFile(f->file_handle, &value, sizeof(T), &written, nullptr)) return true;
    if (f->handle_error) joan::orig::err_run("Error writing file %s", f->file_name);
    return false;
}

template <class T>
T read_raw(joan::orig::File* f) {
    T value{};
    DWORD read = 0;
    if (joan::platform::files::ReadFile(f->file_handle, &value, sizeof(T), &read, nullptr)) return value;
    if (f->handle_error) joan::orig::err_run("Error reading file %s", f->file_name);
    return T{};
}

// [Debug] SaveAudit=1 (save_audit.hpp): the bytes about to be written, at the current file position.
void audit(const joan::orig::File* f, const void* buf, std::uint32_t size, const char* what, const void* caller) {
    if (!joan::hook::save_audit::enabled()) return;
    const auto pos = static_cast<std::int32_t>(joan::platform::files::SetFilePointer(f->file_handle, 0, nullptr, FILE_CURRENT));
    joan::hook::save_audit::scan(f->file_name, pos, buf, size, what, caller);
}
}  // namespace

namespace joan::orig {

// 0x00586da0 - opens an existing file for reading, trying each search dir.
// Quirk: failure is reported through err.run (fatal) regardless of handle_error.
std::int32_t File::file_open(const char* name, std::int32_t handle_error_, char allow_vary_size_) {
    c_open.hit();
    if (std::strlen(name) > kMaxPath) err_run("File : file name is too long.");
    if (file_handle != INVALID_HANDLE_VALUE) file_close();
    std::strcpy(file_name, name);
    handle_error = handle_error_;
    allow_vary_size = allow_vary_size_;

    for (int i = 0; i < kSearchDirCount; ++i) {
        char path[2 * MAX_PATH];  // original: char[MAX_PATH], could overflow
        std::snprintf(path, sizeof(path), "%s%s", search_dir(i), name);
        file_handle = joan::platform::files::CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file_handle != INVALID_HANDLE_VALUE) return 1;
    }
    err_run("Error opening file %s.", file_name);
    return 0;
}

// 0x00586f00 - creates/truncates a file for writing. Always returns 1
// (failure is reported through err.run, which does not return).
std::int32_t File::file_create(const char* name, std::int32_t handle_error_, char allow_vary_size_) {
    c_create.hit();
    if (std::strlen(name) > kMaxPath) err_run("File : file name is too long.");
    std::strcpy(file_name, name);
    handle_error = handle_error_;
    allow_vary_size = allow_vary_size_;
    file_handle = joan::platform::files::CreateFileA(name, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file_handle == INVALID_HANDLE_VALUE) err_run("Error creating file %s", file_name);
    return 1;
}

// 0x00587260
File* File::file_close() {
    c_close.hit();
    if (file_handle != INVALID_HANDLE_VALUE && joan::hook::save_audit::enabled())
        joan::hook::save_audit::close(file_name);  // SaveAudit summary
    if (file_handle != INVALID_HANDLE_VALUE) {
        joan::platform::files::CloseHandle(file_handle);
        file_handle = INVALID_HANDLE_VALUE;
    }
    return this;
}

// 0x005872b0 - File::~File
File* File::destruct() {
    c_destruct.hit();
    return file_close();
}

// 0x005872e0 - with allow_vary_size the record is prefixed by its size as a
// u16 (0 when it does not fit). Returns the WriteFile result.
std::int32_t File::file_write(const void* buf, std::uint32_t size) {
    c_write.hit();
    audit(this, buf, size, "file_write", JOAN_SAVE_AUDIT_CALLER());
    if (allow_vary_size) file_put_unsigned_short(size > 0xffff ? 0 : static_cast<std::uint16_t>(size));
    DWORD written = 0;
    const BOOL ok = joan::platform::files::WriteFile(file_handle, buf, size, &written, nullptr);
    if (!ok && handle_error) err_run("Error writing file %s", file_name);
    return ok;
}

// 0x005873a0 - with allow_vary_size the stored record may be larger or smaller
// than `size`: excess bytes are skipped, missing bytes are zero-filled. On a
// read error with handle_error set, the user is asked whether to retry.
// Quirk kept: a short read (EOF) is not an error, only a failing ReadFile is.
std::int32_t File::file_read(void* buf, std::uint32_t size) {
    c_read.hit();
    const std::int32_t start = file_pos();
    for (;;) {
        std::uint32_t read_size = size;
        std::uint32_t rec_size = size;
        if (allow_vary_size) {
            rec_size = file_get_unsigned_short();
            if (rec_size != 0) read_size = size < rec_size ? size : rec_size;
            else rec_size = size;  // original leaves rec_size at the size prefix value 0 -> treated as "no prefix"
        }
        DWORD got = 0;
        const BOOL ok = joan::platform::files::ReadFile(file_handle, buf, read_size, &got, nullptr);
        if (read_size < rec_size) file_seek(static_cast<std::int32_t>(rec_size - read_size), FILE_CURRENT);
        if (read_size < size) std::memset(static_cast<char*>(buf) + read_size, 0, size - read_size);

        if (!ok && handle_error) {
            char msg[MAX_PATH + 64];  // original: char[100], could overflow with long names
            std::snprintf(msg, sizeof(msg), "Error reading file %s, Retry ?", file_name);
            if (box().ask(msg)) {
                file_seek(start, -1);
                continue;
            }
        }
        return ok;
    }
}

// 0x00587510 - writes `count` records of `size` bytes. With allow_vary_size
// the array is prefixed with u16 count + u16 size, or with 0,0 + u32 count +
// u32 size when either does not fit 16 bits (or both are 0).
std::int32_t File::file_write_array(const void* buf, std::uint32_t count, std::uint32_t size) {
    c_write_array.hit();
    if (!allow_vary_size) return file_write(buf, count * size);
    if (count > 0xffff || size > 0xffff || (count == 0 && size == 0)) {
        file_put_unsigned_short(0);
        file_put_unsigned_short(0);
        file_put_long(static_cast<std::int32_t>(count));
        file_put_long(static_cast<std::int32_t>(size));
    } else {
        file_put_unsigned_short(static_cast<std::uint16_t>(count));
        file_put_unsigned_short(static_cast<std::uint16_t>(size));
    }
    audit(this, buf, count * size, "file_write_array", JOAN_SAVE_AUDIT_CALLER());
    DWORD written = 0;
    const BOOL ok = joan::platform::files::WriteFile(file_handle, buf, count * size, &written, nullptr);
    if (!ok && handle_error) err_run("Error writing file %s", file_name);
    return ok;
}

namespace {
// Primitives for record_array::read on a File.
struct FileSrc {
    File* f;
    bool vary() const { return f->allow_vary_size != 0; }
    bool handle_error() const { return f->handle_error != 0; }
    const char* name() const { return f->file_name; }
    int raw_read(void* buf, std::uint32_t n) {
        DWORD got = 0;
        return joan::platform::files::ReadFile(f->file_handle, buf, n, &got, nullptr);
    }
    int pos() { return f->file_pos(); }
    void seek(int p, int whence) { f->file_seek(p, whence); }
    std::uint16_t get_ushort() { return f->file_get_unsigned_short(); }
    std::int32_t get_long() { return f->file_get_long(); }
    int plain_read(void* buf, std::uint32_t n) { return f->file_read(buf, n); }
};
}  // namespace

// 0x00587650 - reads `count` records of `size` bytes written by
// file_write_array, adapting to a different stored count/record size
// (see record_array.hpp).
std::int32_t File::file_read_array(void* buf, std::uint32_t count, std::uint32_t size) {
    c_read_array.hit();
    FileSrc src{this};
    return record_array::read(src, buf, count, size);
}

// 0x00587ba0 / 0x00587ca0 / 0x00587da0
std::int32_t File::file_put_short(std::int16_t v) {
    c_put_short.hit();
    return write_raw(this, v);
}
std::int32_t File::file_put_unsigned_short(std::uint16_t v) {
    c_put_ushort.hit();
    return write_raw(this, v);
}
std::int32_t File::file_put_long(std::int32_t v) {
    c_put_long.hit();
    audit(this, &v, sizeof v, "file_put_long", JOAN_SAVE_AUDIT_CALLER());
    return write_raw(this, v);
}

// 0x00587c20 / 0x00587d20 / 0x00587e20 - return 0 on error
std::int16_t File::file_get_short() {
    c_get_short.hit();
    return read_raw<std::int16_t>(this);
}
std::uint16_t File::file_get_unsigned_short() {
    c_get_ushort.hit();
    return read_raw<std::uint16_t>(this);
}
std::int32_t File::file_get_long() {
    c_get_long.hit();
    return read_raw<std::int32_t>(this);
}

// 0x00587ea0 - whence -1 means FILE_BEGIN. Returns the new position.
std::int32_t File::file_seek(std::int32_t pos, std::int32_t whence) {
    c_seek.hit();
    if (whence == -1) whence = FILE_BEGIN;
    return static_cast<std::int32_t>(joan::platform::files::SetFilePointer(file_handle, pos, nullptr, static_cast<DWORD>(whence)));
}

// 0x00587ef0
std::int32_t File::file_pos() {
    c_pos.hit();
    return static_cast<std::int32_t>(joan::platform::files::SetFilePointer(file_handle, 0, nullptr, FILE_CURRENT));
}

// 0x00587f30
std::int32_t File::file_size() {
    c_size.hit();
    return static_cast<std::int32_t>(joan::platform::files::GetFileSize(file_handle, nullptr));
}

}  // namespace joan::orig

using joan::orig::File;
JOAN_REPLACE("ilib/file.cpp", "File::file_open", 0x00586da0, &File::file_open);
JOAN_REPLACE("ilib/file.cpp", "File::file_create", 0x00586f00, &File::file_create);
JOAN_REPLACE("ilib/file.cpp", "File::file_close", 0x00587260, &File::file_close);
JOAN_REPLACE("ilib/file.cpp", "File::~File", 0x005872b0, &File::destruct);
JOAN_REPLACE("ilib/file.cpp", "File::file_write", 0x005872e0, &File::file_write);
JOAN_REPLACE("ilib/file.cpp", "File::file_write_array", 0x00587510, &File::file_write_array);
JOAN_REPLACE("ilib/file.cpp", "File::file_put_short", 0x00587ba0, &File::file_put_short);
JOAN_REPLACE("ilib/file.cpp", "File::file_get_short", 0x00587c20, &File::file_get_short);
JOAN_REPLACE("ilib/file.cpp", "File::file_put_unsigned_short", 0x00587ca0, &File::file_put_unsigned_short);
JOAN_REPLACE("ilib/file.cpp", "File::file_get_unsigned_short", 0x00587d20, &File::file_get_unsigned_short);
JOAN_REPLACE("ilib/file.cpp", "File::file_put_long", 0x00587da0, &File::file_put_long);
JOAN_REPLACE("ilib/file.cpp", "File::file_get_long", 0x00587e20, &File::file_get_long);
JOAN_REPLACE("ilib/file.cpp", "File::file_seek", 0x00587ea0, &File::file_seek);
JOAN_REPLACE("ilib/file.cpp", "File::file_pos", 0x00587ef0, &File::file_pos);
JOAN_REPLACE("ilib/file.cpp", "File::file_size", 0x00587f30, &File::file_size);

// ---------------------------------------- from modules/game_b102_07.cpp (game/b102_07)
//
//      0x00586fe0 File::open_append(name, a, c)       (thiscall, ret 0xc)
//
//      0x00587100 File::open_rw(name, a, c)           (thiscall, ret 0xc)
//
//  - 0x00587100: the original buffer is 0x104 bytes; a 0x104-char name writes its NUL into the
//    low byte of the (zero) loop counter, harmless - the buffer here is 0x108 bytes.
namespace {
namespace game_b102_07 {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::stdcall_call;
// Win32 import through its IAT slot.
template <class R = u32, class... A>
R import_call(img::Addr slot, A... args) {
    return stdcall_call<R>(global<std::uintptr_t>(slot), args...);
}
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

void* const kLog = obj(d::g_err);    // error reporter for 0x00586cb0

constexpr img::Addr kCreateFileA = d::iat_KERNEL32_CreateFileA;
constexpr img::Addr kSetFilePointer = d::iat_KERNEL32_SetFilePointer;

u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
void crt_strcpy(void* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
void crt_strcat(void* d, const char* s) { std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_open_append, "File::open_append");
JOAN_COUNTER(c_open_rw, "File::open_rw");

struct H {

    // -------------------------------------------------------------- 0x00586fe0
    i32 open_append(const char* name, u32 a, u8 c) {
        c_open_append.hit();
        if (crt_strlen(name) > 0x104) joan::orig::error::fatal(joan::orig::arg::mptr(kLog), str(d::s_File_file_name_is_too_long));  // 0x00586cb0
        if (jo::self<jo::File>(this)->file_handle != INVALID_HANDLE_VALUE) joan::orig::as_file(this)->file_close();  // 0x00587260
        crt_strcpy(this, name);
        jo::view<u32>(jo::self<jo::File>(this)->handle_error) = a;
        jo::view<u8>(jo::self<jo::File>(this)->allow_vary_size) = c;
        jo::self<jo::File>(this)->file_handle = import_call<HANDLE>(kCreateFileA, name, 0x40000000u, 0u, static_cast<void*>(nullptr), 4u, 0x80u, static_cast<void*>(nullptr));
        if (jo::self<jo::File>(this)->file_handle == INVALID_HANDLE_VALUE) {
            joan::orig::error::fatal(joan::orig::arg::mptr(kLog), str(d::s_Error_appending_file_s), this);  // 'Error appending file %s.'
            return 0;
        }
        import_call<u32>(kSetFilePointer, jo::self<jo::File>(this)->file_handle, 0, static_cast<void*>(nullptr), 2u);
        return 1;
    }

    // -------------------------------------------------------------- 0x00587100
    i32 open_rw(const char* name, u32 a, u8 c) {
        c_open_rw.hit();
        if (crt_strlen(name) > 0x104) joan::orig::error::fatal(joan::orig::arg::mptr(kLog), str(d::s_File_file_name_is_too_long));  // 0x00586cb0
        if (jo::self<jo::File>(this)->file_handle != INVALID_HANDLE_VALUE) joan::orig::as_file(this)->file_close();  // 0x00587260
        crt_strcpy(this, name);
        jo::view<u32>(jo::self<jo::File>(this)->handle_error) = a;
        jo::view<u8>(jo::self<jo::File>(this)->allow_vary_size) = c;
        char buf[0x108];
        for (u32 i = 0; i < 1; ++i) {
            crt_strcpy(buf, search_dir(static_cast<int>(i)));  // [0xa1579c + i * 4]
            crt_strcat(buf, name);
            jo::self<jo::File>(this)->file_handle = import_call<HANDLE>(kCreateFileA, buf, 0xc0000000u, 0u, static_cast<void*>(nullptr), 3u, 0x80u, static_cast<void*>(nullptr));
            if (jo::self<jo::File>(this)->file_handle != INVALID_HANDLE_VALUE) return 1;
        }
        joan::orig::error::fatal(joan::orig::arg::mptr(kLog), str(d::s_Error_opening_file_s), this);  // 'Error opening file %s.'
        return 0;
    }
};

#define B102(name, addr, fn) JOAN_REPLACE("game/b102_07", name, addr, fn)
B102("File::file_append", 0x00586fe0, &H::open_append);
B102("File::file_open_for_update", 0x00587100, &H::open_rw);
#undef B102

}  // namespace game_b102_07
}  // namespace

// ---------------------------------------- from modules/game_b104_08.cpp (game/b104_08)
namespace {
namespace game_b104_08 {

using joan::hook::global;
using i32 = std::int32_t;

// Win32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;

constexpr img::Addr kDeleteFileA = d::iat_KERNEL32_DeleteFileA;

// 0x00587f60
i32 JOAN_CDECL delete_file(const char* path) { return win_call<i32>(kDeleteFileA, path); }

#define B(name, addr, fn) JOAN_REPLACE("game/b104_08", name, addr, fn)
B("delete_file", 0x00587f60, &delete_file);
#undef B

}  // namespace game_b104_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::delete_file(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b104_08::delete_file>(a0)); }  // 0x587f60
