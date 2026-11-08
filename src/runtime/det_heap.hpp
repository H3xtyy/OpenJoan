// Deterministic heap for the game's CRT allocations (src/lib/crt/heap.cpp), used while recording or
// replaying ([Debug] Record / Replay).
//
// Why: the game orders some arrays by heap address (e.g. Sound3D buffer keys, binary-searched with a pointer
// comparator 0x005a10b0), so its behaviour depends on where blocks land. The Win32 heap's low-fragmentation front end
// randomises that placement, so a replay of the same inputs laid blocks out differently, iterated in another order
// and diverged (docs/RUNTIME.md, test14-16). This allocator places blocks only as a function of the sequence of
// requests: one reserved region, a bump pointer and LIFO free lists per size class - identical requests give
// identical offsets from the region's base in every run, so address comparisons agree and replay state hashes can
// name heap pointers by their offset.
#pragma once

#include <cstddef>
#include <cstdint>

/// Deterministic heap for the game's CRT allocations, used while recording or replaying.
namespace joan::det_heap {

/// Reserves the region (once). False if it cannot be reserved (the caller then uses the Win32 heap).
bool init();
/// Whether init() succeeded and the heap is in use.
bool active();

/// HeapAlloc / HeapFree / HeapReAlloc / HeapSize equivalents (zero: HEAP_ZERO_MEMORY). size() of a pointer that is
/// not a live block returns (size_t)-1 like HeapSize.
void* alloc(std::size_t n, bool zero);
/// Frees the block `p`.
bool release(void* p);
/// Resizes the block `p`.
void* realloc(void* p, std::size_t n);
/// Size of the live block `p`, (size_t)-1 when it is not one.
std::size_t size(const void* p);

/// The reserved region [base, base + reserved) and how much of it is in use (bump pointer).
std::uintptr_t base();
/// End of the used part of the region (the bump pointer).
std::uintptr_t used_end();
/// Whether `v` lies in the used part of the region [base(), used_end()).
inline bool contains(std::uintptr_t v) { return active() && v >= base() && v < used_end(); }

/// Diagnostics: called for every allocation (size, offset from base) and release (size = ~0) while set.
void set_trace(void (*fn)(std::size_t n, std::uintptr_t offset));
/// Logs the arena's use (bump pointer, free lists, counts) to openjoan.log; `when` names the moment.
void report(const char* when);

}  // namespace joan::det_heap
