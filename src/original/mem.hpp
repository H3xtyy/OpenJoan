// Original layout of Mem (d:\ww\ilib\mem.cpp-equivalent), the engine's debug
// allocator, global `mem` at 0x00a48838. Every block comes from the CRT heap
// (operator new, 0x007b1d35) with a 16-byte header in front and one guard
// byte behind, and is kept on a doubly linked list:
//
//   malloc(size + 0x11) -> [Header 0x10][body: size bytes][guard 1 byte]
//
// add() fills the body with 0xee, add_clear() with zeros. The guard byte is
// never written on allocation (its setter is empty in this build); del() and
// resize() overwrite the released block (header, body and guard) with 0xff
// before freeing it. The __FILE__ / __LINE__ arguments are only used in error
// messages.
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../runtime/patch.hpp"

namespace joan::orig {

#pragma pack(push, 1)

/// The 16-byte block header in front of every Mem allocation: doubly linked list node plus size and magic.
struct MemHeader {
    MemHeader* next;  ///< 0x00
    MemHeader* prev;  ///< 0x04
    std::uint32_t size;  ///< 0x08  body size
    std::uint32_t magic;  ///< 0x0c  kMemMagic
    /// The block body that follows this header.
    char* body() { return reinterpret_cast<char*>(this) + sizeof(MemHeader); }
    /// The header of the block whose body is `body` (a pointer returned by Mem::add).
    static MemHeader* of(void* body) { return reinterpret_cast<MemHeader*>(static_cast<char*>(body) - sizeof(MemHeader)); }
};
/// Value stored in MemHeader::magic (offset 0x0c).
inline constexpr std::uint32_t kMemMagic = 0x12345678;
inline constexpr std::uint32_t kMemOverhead = sizeof(MemHeader) + 1;  ///< header + guard byte

/// The engine's debug allocator (global `mem`, accessor mem()): tracks every block on a linked list. Blocks are owned
/// by the caller and released with del(); errors are reported through Error::run.
struct Mem {
    std::int32_t count;  ///< 0x00  live blocks
    MemHeader* head;  ///< 0x04
    MemHeader* tail;  ///< 0x08

    // Calls through the incremental-link thunks, so they reach the
    // reimplementation (src/ilib/mem.cpp) when it is installed.
    void* add(std::uint32_t size, const char* file, int line) {  ///< 0x0058e7b0 (thunk 0x0040a95c)
        using Fn = void* (Mem::*)(std::uint32_t, const char*, int);
        return (this->*hook::original<Fn>(fn::add))(size, file, line);
    }
    void* resize(void* p, std::uint32_t size, const char* file, int line) {  ///< 0x0058eb30 (thunk 0x0040a5d3)
        using Fn = void* (Mem::*)(void*, std::uint32_t, const char*, int);
        return (this->*hook::original<Fn>(fn::resize_58eb30))(p, size, file, line);
    }
    void del(void* p, const char* file, int line) {  ///< 0x0058ed70 (thunk 0x0040a457)
        using Fn = void (Mem::*)(void*, const char*, int);
        (this->*hook::original<Fn>(fn::del))(p, file, line);
    }

    // --- reimplemented members (src/ilib/mem.cpp) ---
    void* add_impl(std::uint32_t size, const char* file, int line);  ///< 0x0058e7b0
    void* add_clear(std::uint32_t size, const char* file, int line);  ///< 0x0058e8d0
    void* resize_keep(void* p, std::uint32_t keep, std::uint32_t size, const char* file, int line);  ///< 0x0058e9f0
    void* resize_impl(void* p, std::uint32_t size, const char* file, int line);  ///< 0x0058eb30
    void del_impl(void* p, const char* file, int line);  ///< 0x0058ed70
    std::uint32_t get_mem_size(void* p);  ///< 0x0058ee90

    // The other Mem functions the game code calls (typed, through their slots).
#include "gen/mem_methods.inc"
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MemHeader) == 0x10);
static_assert(sizeof(Mem) == 0x0c);
#endif

/// The engine's global debug allocator `mem` (d::g_mem).
inline Mem& mem() { return hook::global<Mem>(d::g_mem); }

}  // namespace joan::orig
