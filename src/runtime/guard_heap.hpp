// Guard-page heap for the game's CRT allocations (src/lib/crt/heap.cpp), a debugging aid: [Debug] HeapGuard=1.
//
// Why: a write past the end of a heap block corrupts the next block's header, and Windows' heap notices only much
// later (STATUS_HEAP_CORRUPTION inside a later, unrelated HeapAlloc / HeapFree), far from the writer. Here every
// block has its own pages, placed so that its end touches a page that is never committed (PAGE_NOACCESS): the first
// byte written (or read) past the end faults at the writing instruction, and the crash report names it.
//
// Layout: one reserved region; each block gets a slot of whole pages = its data pages + one guard page at the end.
// The block starts at the highest address aligned to kAlign (16 on x64, 8 on x86, as HeapAlloc) that leaves its end
// at or below the guard page; the at most kAlign - 1 bytes between the end and the guard page are filled with 0xfd
// and checked when the block is freed or reallocated (a small overrun that the page cannot catch is logged then).
// Freed slots are decommitted but stay reserved (a use after free faults too) and are reused only once the region's
// fresh space is used up. realloc always moves (allocates, copies, frees). Accesses before the start of a block are
// not caught (the page in front belongs to another slot).
//
// When an access violation hits the region, a vectored exception handler logs what the address is (N bytes past the
// end of block B of size S, or inside a freed block) with the block's allocation (and free) call stack as
// module+offset, before the crash report.
//
// Off by default; read once, when the heap is created. Mutually exclusive with record / replay (det_heap.hpp): when
// both are set, the replay keeps its heap and a warning is logged. Costs at least two pages of address space and one
// committed page per block, and a VirtualAlloc / VirtualFree per call: slow, for diagnosis only.
#pragma once

#include <cstddef>
#include <cstdint>

/// Guard-page heap for the game's CRT allocations ([Debug] HeapGuard).
namespace joan::guard_heap {

/// [Debug] HeapGuard of openjoan.ini (read on the first call).
bool requested();

/// Reserves the region (once) and installs the fault describer. False if it cannot be reserved.
bool init();
/// Whether init() succeeded and the heap is in use.
bool active();

/// HeapAlloc / HeapFree / HeapReAlloc / HeapSize equivalents (zero: HEAP_ZERO_MEMORY). size() of a pointer that is
/// not a live block returns (size_t)-1 like HeapSize; release() of one returns false (and logs it).
void* alloc(std::size_t n, bool zero);
/// Frees the block `p`; false (logged) when `p` is not a live block.
bool release(void* p);
/// Resizes the block `p`; always moves it (allocate, copy, free).
void* realloc(void* p, std::size_t n);
/// Size of the live block `p`, (size_t)-1 when it is not one.
std::size_t size(const void* p);

}  // namespace joan::guard_heap
