// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's JpegErrorMgr class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct JpegErrorMgr {
#include "jpeg_error_mgr_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(JpegErrorMgr) == 0x84);
static_assert(offsetof(JpegErrorMgr, error_exit) == 0x0);
static_assert(offsetof(JpegErrorMgr, emit_message) == 0x4);
static_assert(offsetof(JpegErrorMgr, output_message) == 0x8);
static_assert(offsetof(JpegErrorMgr, format_message) == 0xc);
static_assert(offsetof(JpegErrorMgr, reset_error_mgr) == 0x10);
static_assert(offsetof(JpegErrorMgr, msg_code) == 0x14);
static_assert(offsetof(JpegErrorMgr, msg_parm) == 0x18);
static_assert(offsetof(JpegErrorMgr, trace_level) == 0x68);
static_assert(offsetof(JpegErrorMgr, num_warnings) == 0x6c);
static_assert(offsetof(JpegErrorMgr, jpeg_message_table) == 0x70);
static_assert(offsetof(JpegErrorMgr, last_jpeg_message) == 0x74);
static_assert(offsetof(JpegErrorMgr, addon_message_table) == 0x78);
static_assert(offsetof(JpegErrorMgr, first_addon_message) == 0x7c);
static_assert(offsetof(JpegErrorMgr, last_addon_message) == 0x80);
#endif

}  // namespace joan::orig
