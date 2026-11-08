#include "patch.hpp"
#include "../platform/memory.hpp"

#include "../platform/win32_types.hpp"

namespace joan::hook {

// redirect_function / redirect_replacement / patch_entry: code_table.cpp (slots).

bool patch_import(std::uintptr_t slot, const void* replacement, std::uintptr_t* saved) {
    // `slot` is the original IAT address (the registration key); the slot itself is inside joan_image.
    if (!img::in_old_range(slot)) return false;
    auto* p = &img::global<std::uintptr_t>(img::Addr(static_cast<std::uint32_t>(slot)));
    if (*p == 0) return false;
    const platform::memory::Writable writable(p, sizeof *p);
    if (!writable.ok()) return false;
    *saved = *p;
    *p = reinterpret_cast<std::uintptr_t>(replacement);
    return true;
}

}  // namespace joan::hook
