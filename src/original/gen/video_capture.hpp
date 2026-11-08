// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VideoCapture class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct INSSBuffer;
struct IWMWriter;
struct IWMWriterAdvanced;

#pragma pack(push, 1)
struct VideoCapture {
#include "video_capture_fields.inc"
#include "video_capture_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VideoCapture) == 0x28);
static_assert(offsetof(VideoCapture, active) == 0x0);
static_assert(offsetof(VideoCapture, bpp) == 0x4);
static_assert(offsetof(VideoCapture, width) == 0x8);
static_assert(offsetof(VideoCapture, height) == 0xc);
static_assert(offsetof(VideoCapture, buffer) == 0x10);
static_assert(offsetof(VideoCapture, input) == 0x14);
static_assert(offsetof(VideoCapture, module) == 0x18);
static_assert(offsetof(VideoCapture, writer) == 0x1c);
static_assert(offsetof(VideoCapture, writer_adv) == 0x20);
static_assert(offsetof(VideoCapture, sample) == 0x24);
#endif

}  // namespace joan::orig
