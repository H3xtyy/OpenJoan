// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's FileLoader class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct FileLoader {
#include "file_loader_fields.inc"
#include "file_loader_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(FileLoader) == 0x164);
static_assert(offsetof(FileLoader, path) == 0x0);
static_assert(offsetof(FileLoader, u8_105) == 0x105);
static_assert(offsetof(FileLoader, status) == 0x107);
static_assert(offsetof(FileLoader, disabled) == 0x108);
static_assert(offsetof(FileLoader, done_) == 0x109);
static_assert(offsetof(FileLoader, error_code) == 0x10a);
static_assert(offsetof(FileLoader, own_buffer) == 0x10b);
static_assert(offsetof(FileLoader, buffer) == 0x10c);
static_assert(offsetof(FileLoader, offset) == 0x110);
static_assert(offsetof(FileLoader, size) == 0x114);
static_assert(offsetof(FileLoader, bytes_read) == 0x118);
static_assert(offsetof(FileLoader, priority) == 0x11c);
static_assert(offsetof(FileLoader, callback) == 0x120);
static_assert(offsetof(FileLoader, context) == 0x124);
static_assert(offsetof(FileLoader, ovl_internal) == 0x128);
static_assert(offsetof(FileLoader, ovl_internal_high) == 0x12c);
static_assert(offsetof(FileLoader, ovl_offset) == 0x130);
static_assert(offsetof(FileLoader, ovl_offset_high) == 0x134);
static_assert(offsetof(FileLoader, ovl_event) == 0x138);
static_assert(offsetof(FileLoader, handle) == 0x13c);
static_assert(offsetof(FileLoader, sector_size) == 0x140);
static_assert(offsetof(FileLoader, raw_buf) == 0x144);
static_assert(offsetof(FileLoader, aligned_buf) == 0x148);
static_assert(offsetof(FileLoader, aligned_offset) == 0x14c);
static_assert(offsetof(FileLoader, aligned_size) == 0x150);
static_assert(offsetof(FileLoader, u32_154) == 0x154);
static_assert(offsetof(FileLoader, prev) == 0x158);
static_assert(offsetof(FileLoader, next) == 0x15c);
static_assert(offsetof(FileLoader, queued) == 0x160);
#endif

}  // namespace joan::orig
