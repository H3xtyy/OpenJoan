// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CloudLayers class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Plane;

#pragma pack(push, 1)
struct CloudLayers {
#include "cloud_layers_fields.inc"
#include "cloud_layers_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(CloudLayers, layer1) == 0x0);
static_assert(offsetof(CloudLayers, layer2) == 0x4);
static_assert(offsetof(CloudLayers, built) == 0x8);
#endif

}  // namespace joan::orig
