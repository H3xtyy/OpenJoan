// Original memory layout of FileMem (d:\ww\ilib\filemem.cpp, joan.exe
// 0x00587f80-0x00589520): a File-like reader over a memory buffer. It can own
// a copy of a whole file, borrow a caller's buffer, copy a slice of another
// File/FileMem (and advance that parent's position by what was consumed when
// closed), or pass straight through to a File/FileMem (cur == nullptr).
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

#include "file.hpp"

namespace joan::orig {

#pragma pack(push, 1)

/// File-like reader over a memory buffer, a slice of another File / FileMem, or (cur == nullptr) a pass-through to a
/// File. Owns buf when own_buf is 1.
struct FileMem {
    char file_name[File::kMaxPath + 1];  ///< 0x000
    char allow_vary_size;  ///< 0x105  same meaning as File::allow_vary_size
    char handle_error;  ///< 0x106
    char own_buf;  ///< 0x107  1 = buf allocated by us
    char* cur;  ///< 0x108  read cursor; nullptr = pass-through mode
    char* buf;  ///< 0x10c
    std::int32_t size;  ///< 0x110
    File* sync_file;  ///< 0x114  parent File (slice / pass-through)
    FileMem* sync_mem;  ///< 0x118  parent FileMem (slice)

    // --- reimplemented members (src/ilib/filemem.cpp) ---
    FileMem* construct();  ///< 0x00587f80
    void destruct();  ///< 0x00587fd0
    std::int32_t open(const char* name, std::int32_t handle_error, char allow_vary);  ///< 0x00588000 (char arg; whole slot forwarded)
    std::int32_t open_mem(char* data, std::int32_t size, char handle_error, char allow_vary);  ///< 0x005881c0
    std::int32_t open_slice(File* parent, std::int32_t size);  ///< 0x00588280
    std::int32_t open_slice(FileMem* parent, std::int32_t size);  ///< 0x00588430
    std::int32_t open_file(File* f);  ///< 0x005885e0
    void close();  ///< 0x005886d0
    std::int32_t file_size();  ///< 0x00588830
    std::int32_t file_seek(std::int32_t pos, std::int32_t whence);  ///< 0x005888c0
    std::int32_t file_pos();  ///< 0x005889f0
    std::int32_t file_read(void* buf, std::uint32_t size);  ///< 0x00588a80
    std::int32_t file_read_array(void* buf, std::uint32_t count, std::uint32_t size);  ///< 0x00588c60
    std::int16_t file_get_short();  ///< 0x00589220
    std::uint16_t file_get_unsigned_short();  ///< 0x005892f0
    std::int32_t file_get_long();  ///< 0x005893c0
    std::int32_t read_raw(void* buf, std::int32_t size, std::int32_t* got);  ///< 0x00589490

    // The other FileMem functions the game code calls (typed, through their slots).
#include "gen/file_mem_methods.inc"
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(FileMem, allow_vary_size) == 0x105);
static_assert(offsetof(FileMem, handle_error) == 0x106);
static_assert(offsetof(FileMem, own_buf) == 0x107);
static_assert(offsetof(FileMem, cur) == 0x108);
static_assert(offsetof(FileMem, buf) == 0x10c);
static_assert(offsetof(FileMem, size) == 0x110);
static_assert(offsetof(FileMem, sync_file) == 0x114);
static_assert(offsetof(FileMem, sync_mem) == 0x118);
#endif

}  // namespace joan::orig
