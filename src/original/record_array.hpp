// Versioned record-array reading, shared by File::file_read_array
// (0x00587650) and FileMem::file_read_array (0x00588c60) - the two originals
// are the same algorithm on different primitives (checked by diffing them).
//
// With allow_vary_size the array is stored as
//     u16 count, u16 rec_size            (or 0, 0, u32 count, u32 rec_size)
//     count * rec_size bytes
// so data written by an older or newer build of the game (records with more
// or fewer fields, more or fewer entries) still loads: records are truncated
// or zero-extended to the current size, extra records are skipped and missing
// ones zero-filled. This is what keeps save games compatible across patches.
#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "engine.hpp"

/// Versioned record-array reading shared by File and FileMem.
namespace joan::orig::record_array {

// Src must provide:
//   bool vary(); bool handle_error(); const char* name();
//   int  raw_read(void* buf, std::uint32_t n);      // nonzero = ok
//   int  pos(); void seek(int pos, int whence);      // whence: -1/0 begin, 1 current
//   std::uint16_t get_ushort(); std::int32_t get_long();
//   int  plain_read(void* buf, std::uint32_t n);     // file_read() for !vary
template <class Src>
/// Reads `count` records of `size` bytes into `buf_v` from the source `s` with the versioned format described above (the algorithm of File::file_read_array 0x00587650 and FileMem::file_read_array 0x00588c60); returns nonzero on success, 0 when a read failed.
int read(Src& s, void* buf_v, std::uint32_t count, std::uint32_t size) {
    auto* buf = static_cast<char*>(buf_v);
    if (!s.vary()) return s.plain_read(buf, count * size);

    char msg[260 + 64];  // original: char[100], could overflow with long names
    std::snprintf(msg, sizeof(msg), "Error reading file %s, Retry ?", s.name());

    std::uint32_t file_count = s.get_ushort();
    std::uint32_t file_size = s.get_ushort();
    if (file_count == 0 && file_size == 0) {
        file_count = static_cast<std::uint32_t>(s.get_long());
        file_size = static_cast<std::uint32_t>(s.get_long());
    }
    std::uint32_t n = count < file_count ? count : file_count;  // records read as one block
    std::uint32_t rsize = file_size;
    if (count * size < n * rsize) n = (count * size) / rsize;  // block must fit the caller's buffer

    // 1) read n stored records in one go, then re-space them to `size`.
    const int start = s.pos();
    int ok;
    for (;;) {
        ok = s.raw_read(buf, n * rsize);
        if (!ok && s.handle_error() && box().ask(msg)) {
            s.seek(start, -1);
            continue;
        }
        break;
    }
    if (!ok) return 0;

    if (size > rsize && n > 0) {  // records grew: spread them out from the back, zero the new fields
        char* dst = buf + (n - 1) * size;
        const char* src = buf + (n - 1) * rsize;
        for (std::uint32_t i = n; i > 0; --i, dst -= size, src -= rsize) {
            std::memmove(dst, src, rsize);
            std::memset(dst + rsize, 0, size - rsize);
        }
    } else if (size < rsize) {  // records shrank: compact them
        char* dst = buf;
        const char* src = buf;
        for (std::uint32_t i = 0; i < n; ++i, dst += size, src += rsize) std::memmove(dst, src, size);
    }

    // 2) any further records that did not fit the block, one at a time.
    rsize = file_size < size ? file_size : size;
    for (; n < file_count && n < count; ++n) {
        const int rec_start = s.pos();
        for (;;) {
            ok = s.raw_read(buf + n * size, rsize);
            if (!ok && s.handle_error() && box().ask(msg)) {
                s.seek(rec_start, -1);
                continue;
            }
            break;
        }
        if (!ok) return 0;
        if (file_size > rsize) s.seek(static_cast<int>(file_size - rsize), 1);
        if (size > rsize) std::memset(buf + n * size + rsize, 0, size - rsize);
    }

    // 3) skip stored records we have no room for; zero records the file lacks.
    if (file_count > n) s.seek(static_cast<int>((file_count - n) * file_size), 1);
    if (count > n) std::memset(buf + n * size, 0, (count - n) * size);
    return 1;
}

}  // namespace joan::orig::record_array
