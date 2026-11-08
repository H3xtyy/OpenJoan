// Original memory layout of the Enlight engine's File class (d:\ww\ilib\file.cpp
// equivalent; code at 0x00586da0-0x00587f60 in joan.exe). Same design as
// Seven Kingdoms' File: a Win32 HANDLE plus the file name, 0x10e bytes packed.
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

namespace joan::orig {

#pragma pack(push, 1)

/// Win32 file wrapper of the original engine (File): holds the path and the handle. Failures raise the game's error
/// box when handle_error is 1. The destructor closes the handle.
struct File {
    static constexpr int kMaxPath = 0x104;  ///< MAX_PATH; longer names are a fatal error

    char file_name[kMaxPath + 1];  ///< 0x000
    void* file_handle;  ///< 0x105  INVALID_HANDLE_VALUE when closed
    std::int32_t handle_error;  ///< 0x109  1 = report I/O errors to the user
    char allow_vary_size;  ///< 0x10d  records carry a u16 size prefix (see file_read)

    /// 0x004179c0 File::File(): marks the file closed (file_handle = INVALID_HANDLE_VALUE, the only field it sets).
    File* construct() noexcept {
        file_handle = reinterpret_cast<void*>(~std::uintptr_t{0});
        return this;
    }

    // --- reimplemented members (src/ilib/file.cpp) ---
    std::int32_t file_open(const char* name, std::int32_t handle_error, char allow_vary_size);  ///< 0x00586da0
    std::int32_t file_create(const char* name, std::int32_t handle_error, char allow_vary_size);  ///< 0x00586f00
    File* file_close();  ///< returns this (the original leaves it in eax)                                                   // 0x00587260
    File* destruct();  ///< 0x005872b0  (~File); returns this
    std::int32_t file_write(const void* buf, std::uint32_t size);  ///< 0x005872e0
    std::int32_t file_read(void* buf, std::uint32_t size);  ///< 0x005873a0
    std::int32_t file_write_array(const void* buf, std::uint32_t count, std::uint32_t size);  ///< 0x00587510
    std::int32_t file_read_array(void* buf, std::uint32_t count, std::uint32_t size);  ///< 0x00587650
    std::int32_t file_put_short(std::int16_t value);  ///< 0x00587ba0
    std::int16_t file_get_short();  ///< 0x00587c20
    std::int32_t file_put_unsigned_short(std::uint16_t value);  ///< 0x00587ca0
    std::uint16_t file_get_unsigned_short();  ///< 0x00587d20
    std::int32_t file_put_long(std::int32_t value);  ///< 0x00587da0
    std::int32_t file_get_long();  ///< 0x00587e20
    std::int32_t file_seek(std::int32_t pos, std::int32_t whence = -1);  ///< 0x00587ea0  -1 = FILE_BEGIN
    std::int32_t file_pos();  ///< 0x00587ef0
    std::int32_t file_size();  ///< 0x00587f30

    // The other File functions the game code calls (typed, through their slots).
#include "gen/file_methods.inc"
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(File, file_handle) == 0x105);
static_assert(offsetof(File, handle_error) == 0x109);
static_assert(offsetof(File, allow_vary_size) == 0x10d);
static_assert(sizeof(File) == 0x10e);
#endif

}  // namespace joan::orig
