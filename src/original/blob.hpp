// Original layout of Blob (d:\ww\ilib\blob.cpp): a size-tagged buffer from the
// Mem allocator.
//
// The __LINE__ values passed to Mem come from 16-bit globals (0x00a1510c ..),
// as compiled by MSVC 6 with edit-and-continue debug info (/ZI); the module
// reads them from there so error messages keep pointing at the same lines.
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

namespace joan::orig {

#pragma pack(push, 1)

/// Size-tagged byte buffer (8 bytes) owned through the Mem allocator; `data` is null for an empty blob. Copying
/// allocates; the destructor releases the buffer.
struct Blob {
    char* data;  ///< 0x00  from mem.add, or null
    std::int32_t size;  ///< 0x04

    // --- reimplemented members (src/ilib/blob.cpp) ---
    Blob* construct(std::int32_t size);  ///< 0x0057f770 Blob(int)
    Blob* construct_copy(const Blob* other);  ///< 0x0057f7d0 Blob(Blob&)
    void destruct();  ///< 0x0057f850 ~Blob
    void resize(std::int32_t size);  ///< 0x0057f8a0
    Blob* assign(const Blob* other);  ///< 0x0057f900 operator=

    // The other Blob functions the game code calls (typed, through their slots).
#include "gen/blob_methods.inc"
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Blob) == 8);
#endif

}  // namespace joan::orig
