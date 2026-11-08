// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's D3D class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct IDirect3D8;
struct IDirect3DDevice8;

#pragma pack(push, 1)
struct D3D {
#include "d3d_fields.inc"
#include "d3d_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(D3D, init_state) == 0x0);
static_assert(offsetof(D3D, d3d) == 0x4);
static_assert(offsetof(D3D, device) == 0x8);
static_assert(offsetof(D3D, matrix_stack) == 0x124);
static_assert(offsetof(D3D, brightness) == 0x2ef);
static_assert(offsetof(D3D, contrast) == 0x2f3);
static_assert(offsetof(D3D, gamma) == 0x2f7);
static_assert(offsetof(D3D, gamma_offset) == 0x2fb);
#endif

}  // namespace joan::orig
