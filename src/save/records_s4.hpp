// Batch S4 (campaign, re/a6/SAVES_PLAN.md 4): the generated records of section s4 (gen/records_s4.hpp) plus the
// chunks the generator cannot express (it covers whole members only):
//   - .SAV step 17, the Hud's message-log fields (g_a4d618, Sub::save_14d 0x004843d0 / Sub::load_14d 0x004845a0):
//     nine chunks of the Hud members blink_on .. obj_id, three of them partial arrays - joan.exe writes 0x28 bytes of
//     the u16[40] arrays obj_alpha / obj_1a7 and 0x50 bytes of obj_time[40] (the first 20 elements), all of obj_id;
//   - .SAV step 19, the byte +0x1d of the trigger list g_a5ccd0 (ListC::save 0x00485e70 / List::load_485f50
//     0x00485f50; no typed member yet: Triggers derives from List, whose header ends at +0x1d on x86).
#pragma once

#include <cstddef>
#include <cstdint>

#include "gen/records_s4.hpp"
#include "io.hpp"

namespace joan::save {

namespace s4_detail {
inline constexpr Field kHudBlinkOn[] = {{0x0, 0x1, 1, "blink_on", Type::U8, Ptr::Flag}};
inline constexpr Field kHudBlinkState[] = {{0x0, 0x1, 1, "blink_state", Type::U8, Ptr::Flag}};
inline constexpr Field kHudBlinkTime[] = {{0x0, 0x4, 4, "blink_time", Type::I32, Ptr::Flag}};
inline constexpr Field kHudBlinkStart[] = {{0x0, 0x4, 4, "blink_start", Type::U32, Ptr::Flag}};
inline constexpr Field kHudObjAlpha[] = {{0x0, 0x28, 2, "obj_alpha[0..19]", Type::U16, Ptr::Flag}};
inline constexpr Field kHudObj1a7[] = {{0x0, 0x28, 2, "obj_1a7[0..19]", Type::U16, Ptr::Flag}};
inline constexpr Field kHudObjCount[] = {{0x0, 0x2, 2, "obj_count", Type::I16, Ptr::Flag}};
inline constexpr Field kHudObjTime[] = {{0x0, 0x50, 4, "obj_time[0..19]", Type::U32, Ptr::Flag}};
inline constexpr Field kHudObjId[] = {{0x0, 0xa0, 4, "obj_id", Type::U32, Ptr::Flag}};
inline constexpr Field kTriggersByte1d[] = {{0x0, 0x1, 1, "u8_1d", Type::U8, Ptr::Flag}};
}  // namespace s4_detail

/// The step 17 chunks: Hud offset (x86) and record of each, in file order.
struct HudChunk {
    std::uint32_t off;
    Record rec;
};
inline constexpr HudChunk kHudLog[9] = {
    {0x14d, {"Hud+0x14d", "Hud", 0x1, Legacy::Raw, s4_detail::kHudBlinkOn, 1}},
    {0x14e, {"Hud+0x14e", "Hud", 0x1, Legacy::Raw, s4_detail::kHudBlinkState, 1}},
    {0x14f, {"Hud+0x14f", "Hud", 0x4, Legacy::Raw, s4_detail::kHudBlinkTime, 1}},
    {0x153, {"Hud+0x153", "Hud", 0x4, Legacy::Raw, s4_detail::kHudBlinkStart, 1}},
    {0x157, {"Hud+0x157", "Hud", 0x28, Legacy::Raw, s4_detail::kHudObjAlpha, 1}},
    {0x1a7, {"Hud+0x1a7", "Hud", 0x28, Legacy::Raw, s4_detail::kHudObj1a7, 1}},
    {0x1f7, {"Hud+0x1f7", "Hud", 0x2, Legacy::Raw, s4_detail::kHudObjCount, 1}},
    {0x1f9, {"Hud+0x1f9", "Hud", 0x50, Legacy::Raw, s4_detail::kHudObjTime, 1}},
    {0x299, {"Hud+0x299", "Hud", 0xa0, Legacy::Raw, s4_detail::kHudObjId, 1}},
};
/// Step 19: the trigger list's byte +0x1d.
inline constexpr Record kTriggersByte1d{"Triggers+0x1d", "Triggers", 0x1, Legacy::Raw, s4_detail::kTriggersByte1d, 1};

/// Chunk k of step 17 from the Hud members (T: joan::orig::Hud or a mirror with the same member names).
template <class T>
void put_HudLog(Out& o, std::uint32_t k, const T& h) {
#if UINTPTR_MAX == 0xffffffffu  // x86: the members are where the chunks have them
    static_assert(offsetof(T, blink_on) == 0x14d && offsetof(T, blink_state) == 0x14e && offsetof(T, blink_time) == 0x14f &&
                  offsetof(T, blink_start) == 0x153 && offsetof(T, obj_alpha) == 0x157 && offsetof(T, obj_1a7) == 0x1a7 &&
                  offsetof(T, obj_count) == 0x1f7 && offsetof(T, obj_time) == 0x1f9 && offsetof(T, obj_id) == 0x299);
#endif
    switch (k) {
    case 0: o.u8(h.blink_on); break;
    case 1: o.u8(h.blink_state); break;
    case 2: o.i32(h.blink_time); break;
    case 3: o.u32(h.blink_start); break;
    case 4: for (int e = 0; e < 20; ++e) o.u16(h.obj_alpha[e]); break;
    case 5: for (int e = 0; e < 20; ++e) o.u16(h.obj_1a7[e]); break;
    case 6: o.i16(h.obj_count); break;
    case 7: for (int e = 0; e < 20; ++e) o.u32(h.obj_time[e]); break;
    default: for (int e = 0; e < 40; ++e) o.u32(h.obj_id[e]); break;
    }
}
template <class T>
void get_HudLog(In& i, std::uint32_t k, T& h) {
    switch (k) {
    case 0: h.blink_on = i.u8(); break;
    case 1: h.blink_state = i.u8(); break;
    case 2: h.blink_time = i.i32(); break;
    case 3: h.blink_start = i.u32(); break;
    case 4: for (int e = 0; e < 20; ++e) h.obj_alpha[e] = i.u16(); break;
    case 5: for (int e = 0; e < 20; ++e) h.obj_1a7[e] = i.u16(); break;
    case 6: h.obj_count = i.i16(); break;
    case 7: for (int e = 0; e < 20; ++e) h.obj_time[e] = i.u32(); break;
    default: for (int e = 0; e < 40; ++e) h.obj_id[e] = i.u32(); break;
    }
}

}  // namespace joan::save
