// Vertex shader linker log ([Graphics] ShaderLog=1).
//
// Ground truth for replacing SHADER\nvlinker.dll (stage 3 N3, notes NVLINK.md): every distinct
// vertex shader the game links is written to openjoan.log once - the fragment names, the
// declaration, all function tokens and each constant register the game asks the linker for.
#pragma once

/// [Graphics] ShaderLog: log of every vertex shader the game links.
namespace joan::hook::nvlink_log {

/// Called by VsCache::create_linker (0x00618990) with each new linker (nvlinker INVLink object).
/// With ShaderLog=1 the object gets a copy of its vtable whose CreateShader / GetFragmentID /
/// GetConstantID / GetConstantSlot / ReserveConstantRange entries log and forward.
void wrap_linker(void* linker);

}  // namespace joan::hook::nvlink_log
