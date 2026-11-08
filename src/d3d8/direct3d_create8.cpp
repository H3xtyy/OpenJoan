// The game's Direct3DCreate8 import (d3d8.dll) is answered by our OpenGL renderer's IDirect3D8 (gl/direct3d.cpp):
// the loader puts direct3d_create8 into the IAT slot, so d3d8.dll is never loaded.
#include "../callconv.hpp"
#include "../gl/device.hpp"
#include "../runtime/loader/import_overrides.hpp"
#include "../registry.hpp"
#include "d3d8_api.hpp"

namespace joan::d3d8 {
namespace {

IDirect3D8* JOAN_STDCALL direct3d_create8(UINT sdk_version) {
    hook::log_line("Direct3DCreate8(%u) (OpenGL renderer, native adapter answers)", sdk_version);
    return gld::create_direct3d();
}

JOAN_IMPORT_OVERRIDE("d3d8.dll", "Direct3DCreate8", &direct3d_create8, "Direct3DCreate8 (OpenGL renderer)");

}  // namespace
}  // namespace joan::d3d8
