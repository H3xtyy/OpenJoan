// Import overrides: imports of joan.exe that openjoan.exe answers with its own functions.
//
// joan.exe's import table (src/import_table.inc) is resolved by the loader (src/loader/joan_re.cpp,
// resolve_imports) into the IAT slots inside joan_image. Before it calls LoadLibrary / GetProcAddress for an import it
// looks the import up in this table: a match puts our function into the slot instead. A DLL whose every import has an
// active override is not loaded at all. The loader logs, per DLL, whether it was loaded and how many of its imports
// were replaced; a registered override that matches no import of joan.exe is logged as a warning (a typo in a
// decorated name would otherwise silently keep the DLL).
//
// Registration (namespace scope, any .cpp linked into openjoan.exe; the table is filled during static initialisation,
// before wWinMain resolves the imports):
//     JOAN_IMPORT_OVERRIDE("v3d_87.dll", "?length@@YAMABU_D3DVECTOR@@@Z", &length, "v3d::length");
//     JOAN_IMPORT_OVERRIDE_ORDINAL("DSOUND.dll", 1, &direct_sound_create, "DirectSoundCreate");
//     JOAN_IMPORT_OVERRIDE_IF("OpenAL32.dll", "alSourcePlay", &al_source_play, "alSourcePlay", &use_openal_soft);
// - dll: as named in joan.exe's import table, compared ignoring ASCII case.
// - name: the exported name exactly as imported (MSVC-decorated for C++ exports, e.g. "?deinit@SkinVertex@@QAEXXZ").
// - fn: a free function or a single-inheritance member function (thiscall exports). It must have the export's
//   calling convention (cdecl "YA" / stdcall "YG" / thiscall "QAE" in the decoration; WINAPI for Win32 / OpenAL
//   style C exports as declared by their headers), argument list and return registers (eax, edx:eax or st0).
//   The game and our modules call it through the IAT slot exactly as they called the DLL.
// - label: shown in the log.
// - _IF variant: `enabled` (bool (*)()) is asked once per matching import while the loader resolves (after static
//   initialisation, openjoan.ini readable): false = the DLL's export is used for that import (e.g. a switch between
//   a bundled library and the system DLL).
// Two active overrides for the same import are an error (the loader stops with a message).
#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

/// The loader: resolution of the original executable's imports, including the override table.
namespace joan::loader {

/// A replacement of one import of joan.exe (a DLL function) by a function of ours.
struct ImportOverride {
    const char* dll = nullptr;   ///< import table DLL name (ASCII case-insensitive)
    const char* name = nullptr;  ///< exported name as imported; nullptr = import by ordinal
    std::uint16_t ordinal = 0;   ///< when name == nullptr
    const void* fn = nullptr;    ///< our function
    const char* label = nullptr; ///< name for logs and messages
    bool (*enabled)() = nullptr; ///< nullptr = always active
};

/// Adds an override (static initialisation; the loader reads the table once, before the game starts).
void register_import_override(const ImportOverride& o);
/// Every registered override, in registration order.
const std::vector<ImportOverride>& import_overrides();
/// ASCII case-insensitive comparison of DLL names (no locale, no Windows headers).
bool same_dll(const char* a, const char* b);
/// Does override `o` name this import (DLL + name, or DLL + ordinal)? Ignores `enabled`.
bool matches(const ImportOverride& o, const char* dll, const char* name, unsigned ordinal);
/// Index into import_overrides() of the active override for this import, -1 if none. If several active overrides
/// match, *duplicate (when not null) receives the index of the second one (an error for the loader).
int find_import_override(const char* dll, const char* name, unsigned ordinal, int* duplicate = nullptr);
/// Our function for an import of joan.exe by name (its active override) as a `Fn`, nullptr when it has none: what
/// joan.exe's import slot holds, for C++ of ours that makes the same call (the C runtime's families).
template <class Fn>
Fn override_function(const char* dll, const char* name) {
    const int i = find_import_override(dll, name, 0);
    if (i < 0) return nullptr;
    void* const fn = const_cast<void*>(import_overrides()[static_cast<std::size_t>(i)].fn);
    Fn out;
    static_assert(sizeof out == sizeof fn, "a plain function pointer");
    std::memcpy(&out, &fn, sizeof out);
    return out;
}

/// A free function pointer or a single-inheritance member function pointer (MSVC x86: one code address) as the
/// address that goes into an IAT slot. The Itanium C++ ABI's member function pointer (GCC / Clang elsewhere) is
/// {address, `this` adjustment}: its first word, when the adjustment is 0 and it is not a virtual function's.
template <class Fn>
const void* override_address(Fn fn) {
    static_assert(sizeof(Fn) == sizeof(const void*) || sizeof(Fn) == 2 * sizeof(const void*),
                  "only plain functions / single-inheritance member functions");
    const void* p = nullptr;
    if constexpr (sizeof(Fn) == 2 * sizeof(const void*)) {
        std::uintptr_t words[2] = {};
        std::memcpy(words, &fn, sizeof words);
        if ((words[0] & 1) != 0 || words[1] != 0) return nullptr;
    }
    std::memcpy(&p, &fn, sizeof p);
    return p;
}

/// Tag type: selects the by-ordinal ImportOverrideRegistrar constructor.
struct Ordinal {
    std::uint16_t value;  ///< the import ordinal
};

/// Registers an override from a namespace-scope definition (static initialisation).
struct ImportOverrideRegistrar {
    /// Overrides import `name` of `dll` with `fn` (plain or single-inheritance member function, see
    /// override_address()); `label` names it in logs; `enabled` (optional) decides at load time.
    template <class Fn>
    ImportOverrideRegistrar(const char* dll, const char* name, Fn fn, const char* label, bool (*enabled)() = nullptr) {
        ImportOverride o;
        o.dll = dll;
        o.name = name;
        o.fn = override_address(fn);
        o.label = label;
        o.enabled = enabled;
        register_import_override(o);
    }
    /// As above for an import by ordinal.
    template <class Fn>
    ImportOverrideRegistrar(const char* dll, Ordinal ordinal, Fn fn, const char* label, bool (*enabled)() = nullptr) {
        ImportOverride o;
        o.dll = dll;
        o.ordinal = ordinal.value;
        o.fn = override_address(fn);
        o.label = label;
        o.enabled = enabled;
        register_import_override(o);
    }
};

}  // namespace joan::loader

#define JOAN_IMPORT_OVERRIDE_CAT2(a, b) a##b
#define JOAN_IMPORT_OVERRIDE_CAT(a, b) JOAN_IMPORT_OVERRIDE_CAT2(a, b)
/// Our function `fn` answers joan.exe's import `dll`!`name` (see the top of this file).
#define JOAN_IMPORT_OVERRIDE(dll, name, fn, label)                                                          \
    static const ::joan::loader::ImportOverrideRegistrar JOAN_IMPORT_OVERRIDE_CAT(joan_import_override_, \
                                                                                  __LINE__)(dll, name, fn, label)
/// The same for an import by ordinal (`ordinal`: integer constant).
#define JOAN_IMPORT_OVERRIDE_ORDINAL(dll, ordinal, fn, label)                                                 \
    static const ::joan::loader::ImportOverrideRegistrar JOAN_IMPORT_OVERRIDE_CAT(joan_import_override_,   \
                                                                                  __LINE__)(              \
        dll, ::joan::loader::Ordinal{static_cast<std::uint16_t>(ordinal)}, fn, label)
/// The ordinal form, active only when `enabled()` returns true at import resolution.
#define JOAN_IMPORT_OVERRIDE_ORDINAL_IF(dll, ordinal, fn, label, enabled)                                    \
    static const ::joan::loader::ImportOverrideRegistrar JOAN_IMPORT_OVERRIDE_CAT(joan_import_override_,   \
                                                                                  __LINE__)(              \
        dll, ::joan::loader::Ordinal{static_cast<std::uint16_t>(ordinal)}, fn, label, enabled)
/// Active only when `enabled()` returns true at import resolution.
#define JOAN_IMPORT_OVERRIDE_IF(dll, name, fn, label, enabled)                                                \
    static const ::joan::loader::ImportOverrideRegistrar JOAN_IMPORT_OVERRIDE_CAT(joan_import_override_,   \
                                                                                  __LINE__)(dll, name, fn, label, enabled)
