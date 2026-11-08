// module render/nvlink (on by default; [Hooks] Disable=render/nvlink): our own vertex shader linker in
// place of SHADER\nvlinker.dll (stage 3 N3, notes NVLINK.md 10 (a)). See nvlink_com.cpp.
#pragma once

#include <cstdint>

/// The COM object that puts the render/nvlink linker in place of the original vertex shader linker DLL.
namespace joan::hook::nvlink {

/// True unless [Hooks] Disable excludes render/nvlink (by name or '*').
bool enabled();

/// A new INVLink-compatible object (reference count 1) backed by render/nvlink.cpp: same vtable
/// indices and stdcall conventions as nvlinker.dll's CreateLinker(a, optimize) result.
void* create_linker(std::uint32_t optimize);

}  // namespace joan::hook::nvlink
