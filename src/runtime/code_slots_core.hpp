// The code slot table (docs/RUNTIME.md): the platform-independent part of src/code_table.cpp - slot
// data, the install operations and the resolution rule - without OS calls, so the Linux harness
// (re/code_slots_harness.cpp) runs exactly this code against stage S5's generated code range.
//
// A slot stands for one original code address (src/code_slots.inc, re/gen_code_slots.py):
//   - a THUNK slot (one of joan.exe's 8015 incremental-link thunks): `current` starts as the thunk's original target,
//     exactly the `jmp` S5 generated; redirect() retargets it as S5 retargeted the thunk's jmp (install_all, fixes),
//     redirect_ours() retargets a slot that already leads to a pointer of ours (fixes, canvas wrappers of an installed
//     replacement). At seal it holds what the thunk reaches: `current` if a redirect put a pointer of ours there, else
//     a trap (the original target has no replacement) - S5 codegen_core.hpp finalize_image, thunks.
//     Stage S7d: whether `current` is an original address or a pointer of ours is recorded per slot (`ours`), not
//     read from the value: with ASLR our code and memory can lie anywhere, also inside joan.exe's old code range, so
//     a pointer of ours may be numerically equal to an original address (S5 / S7c told them apart by the range).
//   - a REAL slot (a function entry / other place code arrives): the S5 entry rule decides between the installed
//     replacement (installed_replacement of the address) and a trap - S5 finalize_image, entries. patch_entry() only
//     records an entry patch (canvas asks whether a function is bypassed); like S5's entry jump it is overwritten by
//     the rule at seal.
// Differences to S5 by design: a slot needs no room for a 5-byte jmp, so the 44 TIGHT entries (S5: int3) follow the
// rule like every other entry (S5 run T1: none of them needed a jump), and an address that is no slot at all traps
// (S5: int3, also a trap).
#pragma once

#include <cstddef>
#include <cstdint>

/// Platform-independent part of the code slot table: slot data, install operations and the resolution rule.
namespace joan::code {

/// Static description of one slot (generated, src/code_slots.inc).
struct SlotInfo {
    std::uint32_t va;       ///< original address (0: free slot index, never used)
    std::uint32_t target;   ///< thunk slots: the thunk's original target; 0 otherwise
    std::uint16_t flags;    ///< Flag bits
};

/// Bits of SlotInfo::flags: why a slot exists (generated from the original's function and thunk lists, src/code_slots.inc).
enum Flag : std::uint16_t {
    kFunc = 1,  ///< function start (functions.json / registered)
    kThunk = 2,  ///< an incremental-link thunk jumps here
    kData = 4,  ///< pointed at by a relocated dword of .rdata / .data / .idata / .rsrc
    kHook = 8,  ///< constant of our C++ outside the module that registers it (call by real address, callback, SEH)
    kSelf = 16,  ///< constant of our C++ only inside the registering module (fallback / verify call of the body)
    kReg = 32,  ///< a registered replacement address
    kEntry = 64,  ///< joan.exe's entry point (the loader calls it)
    kSlotThunk = 0x80,  ///< the slot is an incremental-link thunk
    kTight = 0x100,  ///< S5 left this entry int3 (room < 5); informational
};

/// First address of the original code range (.text), 0x00401000.
inline constexpr std::uint32_t kOldCodeBegin = 0x00401000;  // joan.exe .text
/// End (exclusive) of the original code range, 0x009ce700.
inline constexpr std::uint32_t kOldCodeEnd = 0x009ce700;

/// Address of the first incremental-link thunk of the original executable (thunks are 5 bytes each).
inline constexpr std::uint32_t kThunkFirst = kOldCodeBegin + 5;  // joan.exe's incremental-link thunks: 5 bytes each
/// Number of incremental-link thunks, 8015 (src/code_slots.inc).
inline constexpr std::uint32_t kThunkCount = 8015;

/// v lies in joan.exe's old code range. Only meaningful for ORIGINAL code addresses (slot keys, save files, a value
/// that faulted where nothing is mapped): since stage S7d a pointer of ours can lie in this range too.
inline bool in_old_code(std::uintptr_t v) { return v - kOldCodeBegin < kOldCodeEnd - kOldCodeBegin; }
/// An incremental-link thunk address of joan.exe.
inline bool is_thunk(std::uintptr_t v) { return v - kThunkFirst < 5u * kThunkCount && (v - kThunkFirst) % 5u == 0; }

/// Stage S5's entry rule (codegen_core.hpp, unchanged): where the real address of an original function leads once the
/// replacements are installed.
///  - Not replaced: a trap (the original code is gone).
///  - Replaced and reached only through thunks (no data / C++ / entry-point reference): a trap too. Nothing calls
///    the real address except the replacement's own module (fallback / verify calls of the "untouched original
///    body", kSelf); they must not land in the replacement itself.
///  - Otherwise the replacement (data pointers, calls by real address, SEH handlers, callbacks, and entry-patched
///    functions that have no thunk).
inline bool entry_jumps_to_replacement(std::uint16_t flags, bool replaced) {
    if (!replaced) return false;
    if (flags & (kData | kHook | kEntry)) return true;
    return (flags & kThunk) == 0;
}

/// How a slot was resolved (statistics, harness).
enum class Resolution {
    thunk_code,           // thunk slot leads to our C++ (replacement, wrapper, fix)
    thunk_trap,           // thunk slot still led into the old code range: trap
    entry_replacement,    // real slot -> the installed replacement
    entry_trap,           // real slot, not replaced: trap
    entry_trap_replaced,  // real slot of a replaced function reached only through thunks (fallback calls trap)
    unused,               // free index
};

/// The install-time state of the table: `current` and `ours` per slot (see the file comment). Owns no memory.
class Slots {
public:
    /// `current` / `ours`: `count` entries each. `by_va`: the indices of the slots that have an address, sorted by
    /// address (`by_va_count` of them).
    constexpr Slots(const SlotInfo* info, std::size_t count, std::uintptr_t* current, std::uint8_t* ours,
                    const std::uint32_t* by_va, std::size_t by_va_count)
        : info_(info), count_(count), current_(current), ours_(ours), by_va_(by_va), by_va_count_(by_va_count) {}

    /// Number of slots.
    std::size_t count() const { return count_; }
    /// The static description of slot `i` (`i < count()`).
    const SlotInfo& info(std::size_t i) const { return info_[i]; }
    /// What slot `i` holds now: an original address or, when ours(i), a pointer of ours; 0 for a real slot with no
    /// entry patch.
    std::uintptr_t current(std::size_t i) const { return current_[i]; }
    /// `current` of slot i is a pointer of ours (a redirect / entry patch put it there), not an original address.
    bool ours(std::size_t i) const { return ours_[i] != 0; }

    /// Thunk slots at their original targets, no entry patches (S5 build_image).
    void reset() {
        for (std::size_t i = 0; i < count_; ++i) {
            current_[i] = (info_[i].flags & kSlotThunk) ? info_[i].target : 0;
            ours_[i] = 0;
        }
    }

    /// Slot of an original address, or -1 (binary search over the slots sorted by address).
    std::ptrdiff_t find(std::uintptr_t va) const {
        std::size_t lo = 0, hi = by_va_count_;
        while (lo < hi) {
            const std::size_t mid = lo + (hi - lo) / 2;
            if (info_[by_va_[mid]].va < va) lo = mid + 1;
            else hi = mid;
        }
        return lo < by_va_count_ && info_[by_va_[lo]].va == va ? static_cast<std::ptrdiff_t>(by_va_[lo]) : -1;
    }

    /// S5 redirect_function for an ORIGINAL address: every thunk slot that still leads to original address
    /// `original` (no redirect yet) now leads to `replacement`, a pointer of ours. Returns the number of slots.
    int redirect(std::uintptr_t original, std::uintptr_t replacement) {
        int n = 0;
        for (std::size_t i = 0; i < count_; ++i)
            if ((info_[i].flags & kSlotThunk) && !ours_[i] && current_[i] == original) {
                current_[i] = replacement;
                ours_[i] = 1;
                ++n;
            }
        return n;
    }

    /// S5 redirect_function for a pointer of OURS: every thunk slot that an earlier redirect pointed at `pointer` now
    /// leads to `replacement` (fixes / canvas wrap an installed replacement). Returns the number of slots.
    int redirect_ours(std::uintptr_t pointer, std::uintptr_t replacement) {
        int n = 0;
        for (std::size_t i = 0; i < count_; ++i)
            if ((info_[i].flags & kSlotThunk) && ours_[i] && current_[i] == pointer) {
                current_[i] = replacement;
                ++n;
            }
        return n;
    }

    /// S5 patch_entry (entry jump): records `replacement` as the entry patch of the real slot of `original`. False
    /// when the address has no real slot. The rule decides at seal (S5 finalize overwrote every entry jump too).
    bool patch_entry(std::uintptr_t original, std::uintptr_t replacement) {
        const std::ptrdiff_t i = find(original);
        if (i < 0 || (info_[i].flags & kSlotThunk)) return false;
        current_[i] = replacement;
        ours_[i] = 1;
        return true;
    }

    /// Thunk slots that currently lead to `target`, a pointer of ours.
    int count_thunks_to(std::uintptr_t target) const {
        int n = 0;
        for (std::size_t i = 0; i < count_; ++i)
            if ((info_[i].flags & kSlotThunk) && ours_[i] && current_[i] == target) ++n;
        return n;
    }

    /// The entry patch of the real slot of `original` (0: none).
    std::uintptr_t entry_patch(std::uintptr_t original) const {
        const std::ptrdiff_t i = find(original);
        return i < 0 || (info_[i].flags & kSlotThunk) ? 0 : current_[i];
    }

    /// What slot i resolves to: `rep_of(va)` = installed replacement of an original address (0: none), `trap_of(i)` =
    /// the trap stub of slot i.
    template <class RepOf, class TrapOf>
    std::uintptr_t resolve(std::size_t i, RepOf&& rep_of, TrapOf&& trap_of, Resolution* how = nullptr) const {
        const SlotInfo& s = info_[i];
        Resolution r;
        std::uintptr_t v;
        if (s.va == 0) {
            r = Resolution::unused;
            v = trap_of(i);
        } else if (s.flags & kSlotThunk) {
            const std::uintptr_t to = current_[i];
            const bool ours = ours_[i] && to != 0;
            r = ours ? Resolution::thunk_code : Resolution::thunk_trap;
            v = ours ? to : trap_of(i);
        } else {
            const std::uintptr_t rep = rep_of(s.va);
            const bool to_rep = entry_jumps_to_replacement(s.flags, rep != 0);
            r = to_rep ? Resolution::entry_replacement : rep != 0 ? Resolution::entry_trap_replaced : Resolution::entry_trap;
            v = to_rep ? rep : trap_of(i);
        }
        if (how) *how = r;
        return v;
    }

private:
    const SlotInfo* info_;
    std::size_t count_;
    std::uintptr_t* current_;
    std::uint8_t* ours_;
    const std::uint32_t* by_va_;
    std::size_t by_va_count_;
};

/// Trap stub at `out` (running at `at`), calling handler(slot, caller) - `caller` is the return address on top of the
/// stack when the slot was called.
///  x86: push slot; call handler. The handler is cdecl: at its entry [esp] = return into the stub, [esp+4] = slot,
///       [esp+8] = the caller's return address.
///  x64: mov ecx, slot; mov rdx, [rsp]; mov rax, handler; jmp rax (Microsoft x64 convention: arguments in rcx / rdx;
///       the stack is as at the call of the slot, so it has the alignment of a function entry; the handler never
///       returns). 21 bytes, padded with int3 to 24.
#if UINTPTR_MAX == 0xffffffffu
inline constexpr std::size_t kTrapStubSize = 10;
inline void put_trap_stub(std::uint8_t* out, std::uintptr_t at, std::uint32_t slot, std::uintptr_t handler) {
    const std::uint32_t rel = static_cast<std::uint32_t>(handler - (at + 10));
    out[0] = 0x68;
    for (int k = 0; k < 4; ++k) out[1 + k] = static_cast<std::uint8_t>(slot >> (8 * k));
    out[5] = 0xE8;
    for (int k = 0; k < 4; ++k) out[6 + k] = static_cast<std::uint8_t>(rel >> (8 * k));
}
#else
inline constexpr std::size_t kTrapStubSize = 24;
/// x64 form of put_trap_stub (see the comment above): mov ecx, slot; mov rdx, [rsp]; mov rax, handler; jmp rax; 21 bytes padded with int3 to kTrapStubSize.
inline void put_trap_stub(std::uint8_t* out, std::uintptr_t /*at*/, std::uint32_t slot, std::uintptr_t handler) {
    out[0] = 0xB9;  // mov ecx, imm32
    for (int k = 0; k < 4; ++k) out[1 + k] = static_cast<std::uint8_t>(slot >> (8 * k));
    out[5] = 0x48;  // mov rdx, [rsp]
    out[6] = 0x8B;
    out[7] = 0x14;
    out[8] = 0x24;
    out[9] = 0x48;  // mov rax, imm64
    out[10] = 0xB8;
    for (int k = 0; k < 8; ++k) out[11 + k] = static_cast<std::uint8_t>(static_cast<std::uint64_t>(handler) >> (8 * k));
    out[19] = 0xFF;  // jmp rax
    out[20] = 0xE0;
    for (std::size_t k = 21; k < kTrapStubSize; ++k) out[k] = 0xCC;
}
#endif

}  // namespace joan::code
