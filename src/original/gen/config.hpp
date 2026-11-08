// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Config class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Config {
#include "config_fields.inc"
#include "config_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Config) == 0x16a);
static_assert(offsetof(Config, kind_value) == 0x0);
static_assert(offsetof(Config, kind_byte) == 0x4);
static_assert(offsetof(Config, u8_5) == 0x5);
static_assert(offsetof(Config, u16_6) == 0x6);
static_assert(offsetof(Config, u8_8) == 0x8);
static_assert(offsetof(Config, u8_9) == 0x9);
static_assert(offsetof(Config, kind) == 0xa);
static_assert(offsetof(Config, u32_b) == 0xb);
static_assert(offsetof(Config, u8_f) == 0xf);
static_assert(offsetof(Config, u32_10) == 0x10);
static_assert(offsetof(Config, u8_14) == 0x14);
static_assert(offsetof(Config, u8_15) == 0x15);
static_assert(offsetof(Config, u8_16) == 0x16);
static_assert(offsetof(Config, player_name) == 0x17);
static_assert(offsetof(Config, u8_2c) == 0x2c);
static_assert(offsetof(Config, u8_2d) == 0x2d);
static_assert(offsetof(Config, u32_2e) == 0x2e);
static_assert(offsetof(Config, u8_32) == 0x32);
static_assert(offsetof(Config, u8_33) == 0x33);
static_assert(offsetof(Config, u8_34) == 0x34);
static_assert(offsetof(Config, u16_35) == 0x35);
static_assert(offsetof(Config, tooltip_level) == 0x37);
static_assert(offsetof(Config, u8_38) == 0x38);
static_assert(offsetof(Config, u8_39) == 0x39);
static_assert(offsetof(Config, music_volume) == 0x3a);
static_assert(offsetof(Config, ambient_volume) == 0x3c);
static_assert(offsetof(Config, u16_3e) == 0x3e);
static_assert(offsetof(Config, sound_enabled) == 0x40);
static_assert(offsetof(Config, sfx_volume) == 0x41);
static_assert(offsetof(Config, sound_3d_enabled) == 0x43);
static_assert(offsetof(Config, u8_44) == 0x44);
static_assert(offsetof(Config, u8_45) == 0x45);
static_assert(offsetof(Config, u8_46) == 0x46);
static_assert(offsetof(Config, u8_47) == 0x47);
static_assert(offsetof(Config, u8_48) == 0x48);
static_assert(offsetof(Config, u8_49) == 0x49);
static_assert(offsetof(Config, u8_4a) == 0x4a);
static_assert(offsetof(Config, u8_4b) == 0x4b);
static_assert(offsetof(Config, u8_4c) == 0x4c);
static_assert(offsetof(Config, u8_4d) == 0x4d);
static_assert(offsetof(Config, u8_4e) == 0x4e);
static_assert(offsetof(Config, u32_4f) == 0x4f);
static_assert(offsetof(Config, u32_53) == 0x53);
static_assert(offsetof(Config, u32_57) == 0x57);
static_assert(offsetof(Config, u32_5b) == 0x5b);
static_assert(offsetof(Config, u32_5f) == 0x5f);
static_assert(offsetof(Config, u32_63) == 0x63);
static_assert(offsetof(Config, u32_67) == 0x67);
static_assert(offsetof(Config, control_scheme) == 0x6b);
static_assert(offsetof(Config, u32_6f) == 0x6f);
static_assert(offsetof(Config, invert_look) == 0x73);
static_assert(offsetof(Config, i8_74) == 0x74);
static_assert(offsetof(Config, i8_75) == 0x75);
static_assert(offsetof(Config, fov) == 0x76);
static_assert(offsetof(Config, u32_7a) == 0x7a);
static_assert(offsetof(Config, adapter_index) == 0x7e);
static_assert(offsetof(Config, screen_width) == 0x82);
static_assert(offsetof(Config, screen_height) == 0x86);
static_assert(offsetof(Config, colour_depth) == 0x8a);
static_assert(offsetof(Config, refresh_rate) == 0x8e);
static_assert(offsetof(Config, texture_quality) == 0x92);
static_assert(offsetof(Config, u8_93) == 0x93);
static_assert(offsetof(Config, u8_94) == 0x94);
static_assert(offsetof(Config, u8_95) == 0x95);
static_assert(offsetof(Config, trilinear_filtering) == 0x96);
static_assert(offsetof(Config, u8_97) == 0x97);
static_assert(offsetof(Config, draw_distance) == 0x98);
static_assert(offsetof(Config, detail) == 0x9c);
static_assert(offsetof(Config, shadow_quality) == 0x9d);
static_assert(offsetof(Config, water_reflection) == 0x9e);
static_assert(offsetof(Config, grass_enabled) == 0x9f);
static_assert(offsetof(Config, view_distance) == 0xa0);
static_assert(offsetof(Config, grass_density_percent) == 0xa4);
static_assert(offsetof(Config, brightness) == 0xa8);
static_assert(offsetof(Config, contrast) == 0xac);
static_assert(offsetof(Config, gamma) == 0xb0);
static_assert(offsetof(Config, u8_b4) == 0xb4);
static_assert(offsetof(Config, saved) == 0xb5);
#endif

}  // namespace joan::orig
