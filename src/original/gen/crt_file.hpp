// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CrtFile class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CrtFile {
#include "crt_file_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CrtFile) == 0x20);
static_assert(offsetof(CrtFile, ptr) == 0x0);
static_assert(offsetof(CrtFile, cnt) == 0x4);
static_assert(offsetof(CrtFile, base) == 0x8);
static_assert(offsetof(CrtFile, flag) == 0xc);
static_assert(offsetof(CrtFile, file) == 0x10);
static_assert(offsetof(CrtFile, charbuf) == 0x14);
static_assert(offsetof(CrtFile, bufsiz) == 0x18);
static_assert(offsetof(CrtFile, tmpfname) == 0x1c);
#endif

}  // namespace joan::orig
