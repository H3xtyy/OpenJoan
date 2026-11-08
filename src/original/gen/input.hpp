// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Input class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "input_event.hpp"
#include "joystick.hpp"
#include "mouse_click.hpp"
#include "pad.hpp"

namespace joan::orig {

struct IDirectInput8;
struct IDirectInputDevice8;

#pragma pack(push, 1)
struct Input {
#include "input_fields.inc"
#include "input_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Input) == 0xd69);
static_assert(offsetof(Input, init_flag) == 0x0);
static_assert(offsetof(Input, u8_1) == 0x1);
static_assert(offsetof(Input, inactive) == 0x2);
static_assert(offsetof(Input, skey_changed) == 0x3);
static_assert(offsetof(Input, u32_4) == 0x4);
static_assert(offsetof(Input, direct_input) == 0x8);
static_assert(offsetof(Input, di_mouse) == 0xc);
static_assert(offsetof(Input, di_keyboard) == 0x10);
static_assert(offsetof(Input, cur_x) == 0x14);
static_assert(offsetof(Input, cur_y) == 0x18);
static_assert(offsetof(Input, cur_z) == 0x1c);
static_assert(offsetof(Input, left_down) == 0x20);
static_assert(offsetof(Input, right_down) == 0x24);
static_assert(offsetof(Input, middle_down) == 0x28);
static_assert(offsetof(Input, delta_x) == 0x2c);
static_assert(offsetof(Input, delta_y) == 0x30);
static_assert(offsetof(Input, delta_z) == 0x34);
static_assert(offsetof(Input, u32_38) == 0x38);
static_assert(offsetof(Input, acc_x) == 0x44);
static_assert(offsetof(Input, acc_y) == 0x48);
static_assert(offsetof(Input, acc_z) == 0x4c);
static_assert(offsetof(Input, key_state) == 0x50);
static_assert(offsetof(Input, skey_state) == 0x150);
static_assert(offsetof(Input, prev_skey_state) == 0x152);
static_assert(offsetof(Input, key_flags) == 0x154);
static_assert(offsetof(Input, joy_state_0_) == 0x156);
static_assert(offsetof(Input, joy_state_1_) == 0x18f);
static_assert(offsetof(Input, joy_state_2_) == 0x1c8);
static_assert(offsetof(Input, joy_state_3_) == 0x201);
static_assert(offsetof(Input, prev_joy_state_0_) == 0x23a);
static_assert(offsetof(Input, prev_joy_state_1_) == 0x273);
static_assert(offsetof(Input, prev_joy_state_2_) == 0x2ac);
static_assert(offsetof(Input, prev_joy_state_3_) == 0x2e5);
static_assert(offsetof(Input, joystick_0_) == 0x31e);
static_assert(offsetof(Input, joystick_1_) == 0x3e4);
static_assert(offsetof(Input, joystick_2_) == 0x4aa);
static_assert(offsetof(Input, joystick_3_) == 0x570);
static_assert(offsetof(Input, bound_x1) == 0x636);
static_assert(offsetof(Input, bound_y1) == 0x63a);
static_assert(offsetof(Input, bound_x2) == 0x63e);
static_assert(offsetof(Input, bound_y2) == 0x642);
static_assert(offsetof(Input, bound_z1) == 0x646);
static_assert(offsetof(Input, bound_z2) == 0x64a);
static_assert(offsetof(Input, bound_mode) == 0x64e);
static_assert(offsetof(Input, u32_652) == 0x652);
static_assert(offsetof(Input, u32_656) == 0x656);
static_assert(offsetof(Input, key_repeat_delay) == 0x65a);
static_assert(offsetof(Input, key_repeat_rate) == 0x65e);
static_assert(offsetof(Input, saved_x) == 0x662);
static_assert(offsetof(Input, saved_y) == 0x666);
static_assert(offsetof(Input, event_skey_state) == 0x66a);
static_assert(offsetof(Input, has_mouse_event) == 0x66c);
static_assert(offsetof(Input, has_joy_event) == 0x66d);
static_assert(offsetof(Input, mouse_event_type) == 0x66e);
static_assert(offsetof(Input, clicks) == 0x672);
static_assert(offsetof(Input, scan_code) == 0x6aa);
static_assert(offsetof(Input, key_code) == 0x6ae);
static_assert(offsetof(Input, event_time) == 0x6b2);
static_assert(offsetof(Input, init_time) == 0x6b6);
static_assert(offsetof(Input, event_joy_index) == 0x6ba);
static_assert(offsetof(Input, event_joy_arg) == 0x6bb);
static_assert(offsetof(Input, event_joy_state) == 0x6bc);
static_assert(offsetof(Input, button_down) == 0x6f5);
static_assert(offsetof(Input, u8_6f7) == 0x6f7);
static_assert(offsetof(Input, button_press_time) == 0x6f9);
static_assert(offsetof(Input, last_scan_code) == 0x701);
static_assert(offsetof(Input, last_key_code) == 0x705);
static_assert(offsetof(Input, last_key_time) == 0x709);
static_assert(offsetof(Input, event_queue_0_) == 0x70d);
static_assert(offsetof(Input, event_queue_1_) == 0x75e);
static_assert(offsetof(Input, event_queue_2_) == 0x7af);
static_assert(offsetof(Input, event_queue_3_) == 0x800);
static_assert(offsetof(Input, event_queue_4_) == 0x851);
static_assert(offsetof(Input, event_queue_5_) == 0x8a2);
static_assert(offsetof(Input, event_queue_6_) == 0x8f3);
static_assert(offsetof(Input, event_queue_7_) == 0x944);
static_assert(offsetof(Input, event_queue_8_) == 0x995);
static_assert(offsetof(Input, event_queue_9_) == 0x9e6);
static_assert(offsetof(Input, event_queue_10_) == 0xa37);
static_assert(offsetof(Input, event_queue_11_) == 0xa88);
static_assert(offsetof(Input, event_queue_12_) == 0xad9);
static_assert(offsetof(Input, event_queue_13_) == 0xb2a);
static_assert(offsetof(Input, event_queue_14_) == 0xb7b);
static_assert(offsetof(Input, event_queue_15_) == 0xbcc);
static_assert(offsetof(Input, event_queue_16_) == 0xc1d);
static_assert(offsetof(Input, event_queue_17_) == 0xc6e);
static_assert(offsetof(Input, event_queue_18_) == 0xcbf);
static_assert(offsetof(Input, event_queue_19_) == 0xd10);
static_assert(offsetof(Input, queue_head) == 0xd61);
static_assert(offsetof(Input, queue_tail) == 0xd65);
#endif

}  // namespace joan::orig
