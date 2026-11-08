// Reimplementation of FileMem (d:\ww\ilib\filemem.cpp), joan.exe
// 0x00587f80-0x00589520 - all 17 functions.
//
// Until stage S7c a verify mode ran joan.exe's original read functions next to these
// (memory mode) and compared data, cursor and result; joan.exe's code no longer exists
// and the comparison code was removed. (This note keeps the comment's length: the mem()
// calls below pass __LINE__ where the original passes its own source line.)
//
#include <cstring>
#include "../original/direct.hpp"
#include "../fn_names.hpp"
#include "../original/engine.hpp"
#include "../original/file_mem.hpp"
#include "../registry.hpp"
#include "../original/record_array.hpp"
#include "../platform/win32_types.hpp"
#include <cstdio>
#include "../data_names.hpp"
#include "../original/file.hpp"
#include "../runtime/save_audit.hpp"

namespace {
constexpr const char* kSrc = "joan-re/src/ilib/filemem.cpp";

JOAN_COUNTER(c_ctor, "FileMem::FileMem");
JOAN_COUNTER(c_dtor, "FileMem::~FileMem");
JOAN_COUNTER(c_open, "FileMem::open");
JOAN_COUNTER(c_open_mem, "FileMem::open(mem)");
JOAN_COUNTER(c_slice_file, "FileMem::open_slice(File*)");
JOAN_COUNTER(c_slice_mem, "FileMem::open_slice(FileMem*)");
JOAN_COUNTER(c_open_file, "FileMem::open_file");
JOAN_COUNTER(c_close, "FileMem::close");
JOAN_COUNTER(c_size, "FileMem::file_size");
JOAN_COUNTER(c_seek, "FileMem::file_seek");
JOAN_COUNTER(c_pos, "FileMem::file_pos");
JOAN_COUNTER(c_read, "FileMem::file_read");
JOAN_COUNTER(c_read_array, "FileMem::file_read_array");
JOAN_COUNTER(c_get_short, "FileMem::file_get_short");
JOAN_COUNTER(c_get_ushort, "FileMem::file_get_unsigned_short");
JOAN_COUNTER(c_get_long, "FileMem::file_get_long");
JOAN_COUNTER(c_read_raw, "FileMem::read_raw");
}  // namespace

namespace joan::orig {

// 0x00587f80 - constructor (leaves cur/size/own_buf uninitialised, as the original).
FileMem* FileMem::construct() {
    c_ctor.hit();
    buf = nullptr;
    sync_file = nullptr;
    sync_mem = nullptr;
    return this;
}

// 0x00587fd0 - destructor
void FileMem::destruct() {
    c_dtor.hit();
    close();
}

// 0x00588000 - loads a whole file into an owned buffer. Returns file_open's result.
// The handle_error argument is a char, but the original forwards its whole
// 32-bit stack slot to File::file_open, so we take it as int32 to do the same.
std::int32_t FileMem::open(const char* name, std::int32_t handle_error_, char allow_vary) {
    c_open.hit();
    close();
    handle_error = static_cast<char>(handle_error_);
    allow_vary_size = allow_vary;
    std::strcpy(file_name, name);

    File f;  // File::File() just marks the handle invalid
    f.file_handle = reinterpret_cast<void*>(-1);
    const std::int32_t ok = f.file_open(name, handle_error_, 0);
    if (ok) {
        size = f.file_size();
        buf = static_cast<char*>(mem().add(static_cast<std::uint32_t>(size), kSrc, __LINE__));
        own_buf = 1;
        sync_file = nullptr;
        sync_mem = nullptr;
        f.file_read(buf, static_cast<std::uint32_t>(size));
        f.file_close();
        cur = buf;
    }
    f.destruct();
    return ok;
}

// 0x005881c0 - reads from a caller-owned buffer.
std::int32_t FileMem::open_mem(char* data, std::int32_t size_, char handle_error_, char allow_vary) {
    c_open_mem.hit();
    close();
    handle_error = handle_error_;
    allow_vary_size = allow_vary;
    file_name[0] = 0;
    size = size_;
    buf = data;
    own_buf = 0;
    sync_file = nullptr;
    sync_mem = nullptr;
    cur = buf;
    return 1;
}

namespace {
// Shared body of the two open_slice overloads: copy `size` bytes (-1 = to the
// end) from the parent's current position; the parent is rewound, and is
// advanced by the bytes actually consumed when this FileMem is closed.
template <class Parent>
std::int32_t open_slice_impl(FileMem* self, Parent* parent, std::int32_t size, char parent_handle_error,
                             char& parent_vary) {
    if (!parent) {
        self->size = 0;
        return 0;
    }
    self->handle_error = parent_handle_error;
    self->allow_vary_size = parent_vary;
    std::strcpy(self->file_name, parent->file_name);
    const std::int32_t pos = parent->file_pos();
    if (size == -1 || pos + size > parent->file_size()) size = parent->file_size() - pos;
    self->size = size;
    self->buf = static_cast<char*>(mem().add(static_cast<std::uint32_t>(self->size), kSrc, __LINE__));
    self->own_buf = 1;
    parent_vary = 0;  // read the slice raw
    const std::int32_t ok = parent->file_read(self->buf, static_cast<std::uint32_t>(self->size));
    parent_vary = self->allow_vary_size;
    parent->file_seek(pos, 0);
    self->cur = self->buf;
    return ok;
}
}  // namespace

// 0x00588280
std::int32_t FileMem::open_slice(File* parent, std::int32_t size_) {
    c_slice_file.hit();
    close();
    if (!parent) {
        size = 0;
        return 0;
    }
    sync_file = parent;
    sync_mem = nullptr;
    return open_slice_impl(this, parent, size_, static_cast<char>(parent->handle_error), parent->allow_vary_size);
}

// 0x00588430
std::int32_t FileMem::open_slice(FileMem* parent, std::int32_t size_) {
    c_slice_mem.hit();
    close();
    if (!parent) {
        size = 0;
        return 0;
    }
    sync_file = nullptr;
    sync_mem = parent;
    return open_slice_impl(this, parent, size_, parent->handle_error, parent->allow_vary_size);
}

// 0x005885e0 - pass-through mode: every read goes straight to `f`.
std::int32_t FileMem::open_file(File* f) {
    c_open_file.hit();
    close();
    if (!f) {
        size = 0;
        return 0;
    }
    handle_error = static_cast<char>(f->handle_error);
    allow_vary_size = f->allow_vary_size;
    std::strcpy(file_name, f->file_name);
    size = f->file_size();
    buf = nullptr;
    own_buf = 0;
    sync_file = f;
    sync_mem = nullptr;
    cur = nullptr;
    return 1;
}

// 0x005886d0 - for slices, advances the parent by the bytes consumed.
void FileMem::close() {
    c_close.hit();
    if (cur && sync_file) sync_file->file_seek(static_cast<std::int32_t>(cur - buf), 1);
    if (cur && sync_mem) sync_mem->file_seek(static_cast<std::int32_t>(cur - buf), 1);
    sync_file = nullptr;
    sync_mem = nullptr;
    cur = nullptr;
    if (buf && own_buf) mem().del(buf, kSrc, __LINE__);
    buf = nullptr;
    own_buf = 0;
    size = 0;
}

// 0x00588830
std::int32_t FileMem::file_size() {
    c_size.hit();
    if (cur) return size;
    if (sync_file) return sync_file->file_size();
    if (sync_mem) return sync_mem->file_size();
    return size;
}

// 0x005888c0 - whence -1/0 = begin, 1 = current, 2 = end. Returns the new position.
std::int32_t FileMem::file_seek(std::int32_t pos, std::int32_t whence) {
    c_seek.hit();
    if (!cur) {
        if (sync_file) return sync_file->file_seek(pos, whence);
        if (sync_mem) return sync_mem->file_seek(pos, whence);
        return 0;
    }
    if (whence == -1) whence = 0;
    if (whence == 0)
        cur = buf + pos;
    else if (whence == 1)
        cur += pos;
    else if (whence == 2)
        cur = buf + size + pos;
    return static_cast<std::int32_t>(cur - buf);
}

// 0x005889f0
std::int32_t FileMem::file_pos() {
    c_pos.hit();
    if (!cur) {
        if (sync_file) return sync_file->file_pos();
        if (sync_mem) return sync_mem->file_pos();
        return 0;
    }
    return static_cast<std::int32_t>(cur - buf);
}

// 0x00589490 - copies up to `size` bytes (clamped to the end). Always returns 1.
std::int32_t FileMem::read_raw(void* out, std::int32_t n, std::int32_t* got) {
    c_read_raw.hit();
    const std::int32_t pos = file_pos();
    if (pos + n > size) n = size - pos;
    if (n <= 0) {
        *got = 0;
        return 1;
    }
    std::memcpy(out, cur, static_cast<std::size_t>(n));
    cur += n;
    *got = n;
    return 1;
}

// 0x00588a80 - same contract as File::file_read (u16 size prefix with
// allow_vary_size). The retry prompt is unreachable because read_raw never fails.
std::int32_t FileMem::file_read(void* out, std::uint32_t n) {
    c_read.hit();
    if (!cur) {
        if (sync_file) return sync_file->file_read(out, n);
        if (sync_mem) return sync_mem->file_read(out, n);
        return 0;
    }
    const std::int32_t start = file_pos();
    for (;;) {
        std::uint32_t read_size = n;
        std::uint32_t rec_size = n;
        if (allow_vary_size) {
            rec_size = file_get_unsigned_short();
            if (rec_size != 0) read_size = n < rec_size ? n : rec_size;
            else rec_size = n;
        }
        std::int32_t got = 0;
        const std::int32_t ok = read_raw(out, static_cast<std::int32_t>(read_size), &got);
        if (read_size < rec_size) file_seek(static_cast<std::int32_t>(rec_size - read_size), 1);
        if (read_size < n) std::memset(static_cast<char*>(out) + read_size, 0, n - read_size);
        if (!ok && handle_error) {
            char msg[File::kMaxPath + 64];
            std::snprintf(msg, sizeof(msg), "Error reading file %s, Retry ?", file_name);
            if (box().ask(msg)) {
                file_seek(start, -1);
                continue;
            }
        }
        return ok;
    }
}

namespace {
struct MemSrc {
    FileMem* m;
    bool vary() const { return m->allow_vary_size != 0; }
    bool handle_error() const { return m->handle_error != 0; }
    const char* name() const { return m->file_name; }
    int raw_read(void* out, std::uint32_t n) {
        std::int32_t got = 0;
        return m->read_raw(out, static_cast<std::int32_t>(n), &got);
    }
    int pos() { return m->file_pos(); }
    void seek(int p, int whence) { m->file_seek(p, whence); }
    std::uint16_t get_ushort() { return m->file_get_unsigned_short(); }
    std::int32_t get_long() { return m->file_get_long(); }
    int plain_read(void* out, std::uint32_t n) { return m->file_read(out, n); }
};

template <class T>
T read_value(FileMem* m) {
    T v{};
    std::int32_t got = 0;
    if (m->read_raw(&v, sizeof(T), &got)) return v;
    if (m->handle_error) err_run("Error reading file %s", m->file_name);  // unreachable, kept for fidelity
    return T{};
}
}  // namespace

// 0x00588c60
std::int32_t FileMem::file_read_array(void* out, std::uint32_t count, std::uint32_t n) {
    c_read_array.hit();
    if (!cur) {
        if (sync_file) return sync_file->file_read_array(out, count, n);
        if (sync_mem) return sync_mem->file_read_array(out, count, n);
        return 0;
    }
    MemSrc src{this};
    return record_array::read(src, out, count, n);
}

// 0x00589220 / 0x005892f0 / 0x005893c0
std::int16_t FileMem::file_get_short() {
    c_get_short.hit();
    if (!cur) {
        if (sync_file) return sync_file->file_get_short();
        if (sync_mem) return sync_mem->file_get_short();
        return 0;
    }
    return read_value<std::int16_t>(this);
}
std::uint16_t FileMem::file_get_unsigned_short() {
    c_get_ushort.hit();
    if (!cur) {
        if (sync_file) return sync_file->file_get_unsigned_short();
        if (sync_mem) return sync_mem->file_get_unsigned_short();
        return 0;
    }
    return read_value<std::uint16_t>(this);
}
std::int32_t FileMem::file_get_long() {
    c_get_long.hit();
    if (!cur) {
        if (sync_file) return sync_file->file_get_long();
        if (sync_mem) return sync_mem->file_get_long();
        return 0;
    }
    return read_value<std::int32_t>(this);
}

}  // namespace joan::orig

namespace {
using joan::orig::FileMem;

using SliceFile = std::int32_t (FileMem::*)(joan::orig::File*, std::int32_t);
using SliceMem = std::int32_t (FileMem::*)(FileMem*, std::int32_t);
}  // namespace

#define MEM(name, addr, fn) JOAN_REPLACE("ilib/filemem.cpp", "FileMem::" name, addr, fn)
MEM("FileMem", 0x00587f80, &FileMem::construct);
MEM("~FileMem", 0x00587fd0, &FileMem::destruct);
MEM("file_open", 0x00588000, &FileMem::open);
MEM("file_open_mem", 0x005881c0, &FileMem::open_mem);
MEM("file_open_file_section_588280", 0x00588280, static_cast<SliceFile>(&FileMem::open_slice));
MEM("file_open_file_section_588430", 0x00588430, static_cast<SliceMem>(&FileMem::open_slice));
MEM("file_open_map", 0x005885e0, &FileMem::open_file);
MEM("file_close", 0x005886d0, &FileMem::close);
MEM("file_size", 0x00588830, &FileMem::file_size);
MEM("file_seek", 0x005888c0, &FileMem::file_seek);
MEM("file_pos", 0x005889f0, &FileMem::file_pos);
MEM("file_read", 0x00588a80, &FileMem::file_read);
MEM("file_read_array", 0x00588c60, &FileMem::file_read_array);
MEM("file_get_short", 0x00589220, &FileMem::file_get_short);
MEM("file_get_unsigned_short", 0x005892f0, &FileMem::file_get_unsigned_short);
MEM("file_get_long", 0x005893c0, &FileMem::file_get_long);
MEM("low_lv_read", 0x00589490, &FileMem::read_raw);
#undef MEM

// ---------------------------------------- from ilib/file.cpp (ilib/file.cpp)
namespace {
namespace ilib_file {

using joan::orig::File;

JOAN_REPLACE("ilib/file.cpp", "File::file_read", 0x005873a0, &File::file_read);
JOAN_REPLACE("ilib/file.cpp", "File::file_read_array", 0x00587650, &File::file_read_array);

}  // namespace ilib_file
}  // namespace
