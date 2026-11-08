// Original memory layouts of DynArray (d:\ww\ilib\dynarr.cpp, joan.exe
// 0x005843d0-0x00585274) and DynArrayB (d:\ww\ilib\dynarrb.cpp,
// 0x005856a0-0x00585c72): the engine's growable array of fixed-size records,
// used by almost every game system. Records are numbered from 1; `cur_pos`
// is a cursor that most operations move.
//
// DynArrayB adds "empty rooms": linkout() only blanks a record and remembers
// its number, and the next linkin() reuses it, so record numbers stay stable.
#pragma once
#include "../callconv.hpp"

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

#include "file.hpp"

namespace joan::orig {

#pragma pack(push, 1)

/// Growable array of fixed-size records, numbered from 1, with a cursor. Owns body_buf (Mem allocator). Out-of-range
/// record numbers give null / -1 results rather than failures.
struct DynArray {
    std::int32_t ele_num;  ///< 0x00  capacity in records
    std::int32_t block_num;  ///< 0x04  growth step
    std::int32_t cur_pos;  ///< 0x08  1-based cursor, 0 = none
    std::int32_t last_ele;  ///< 0x0c  number of records
    std::int32_t ele_size;  ///< 0x10  record size in bytes
    std::int32_t sort_offset;  ///< 0x14  set by init_sort (-1 = none)
    char sort_type;  ///< 0x18
    char* body_buf;  ///< 0x19  ele_num * ele_size bytes, from the game's Mem allocator

    // Inline accessors of the original (compiled into each caller).
    /// Pointer to the record under the cursor, null when the cursor is 0 (inline in the original).
    [[nodiscard]] char* get() const { return cur_pos ? body_buf + (cur_pos - 1) * ele_size : nullptr; }
    /// Pointer to record `rec` (1-based), null when out of range (inline in the original).
    [[nodiscard]] char* get(std::int32_t rec) const {
        return rec < 1 || rec > last_ele ? nullptr : body_buf + (rec - 1) * ele_size;
    }
    /// Number of records in use (last_ele).
    [[nodiscard]] std::int32_t size() const { return last_ele; }
    void start() { cur_pos = last_ele < 1 ? last_ele : 1; }  ///< 0x004b9d40
    // Accessors the game calls out of line (its "List" functions are DynArrays of pointers):
    [[nodiscard]] std::int32_t cursor() const { return cur_pos; }  ///< 0x00417520
    /// The pointer stored in record `rec`, null when out of range (0x0041b3f0 and its wrappers 0x0041b3c0,
    /// 0x0041bd60, 0x00487b80, 0x0047b600, 0x00425d90).
    [[nodiscard]] void* ptr_at(std::int32_t rec) const {
        char* const e = get(rec);
        return e ? *reinterpret_cast<void* const*>(e) : nullptr;
    }
    /// 1 when record `rec` is out of range or holds null (0x0041bd90, 0x00428b70, 0x00487bb0, 0x006b0790).
    [[nodiscard]] std::int32_t empty_at(std::int32_t rec) const { return ptr_at(rec) == nullptr ? 1 : 0; }

    // --- reimplemented members (src/ilib/dynarr.cpp) ---
    DynArray* construct(std::int32_t ele_size, std::int32_t block_num, std::int32_t init_num);  ///< 0x005843d0
    void destruct();  ///< 0x005844b0
    void resize(std::int32_t new_ele_num);  ///< 0x00584510
    void zap(std::int32_t resize_flag);  ///< 0x00584580
    void linkin(const void* ent);  ///< 0x00584620
    void linkin_unique(const void* ent);  ///< 0x005846f0
    void insert(const void* ent);  ///< 0x00584790
    void insert_at(std::int32_t pos, const void* ent);  ///< 0x005848d0
    void linkout(std::int32_t pos);  ///< 0x00584a10
    void update(const void* ent, std::int32_t pos);  ///< 0x00584b30
    void add_blank(std::int32_t count);  ///< 0x00584bd0
    std::int32_t linear_search(const void* ent);  ///< 0x00584c80
    void read(void* buf);  ///< 0x00584d10
    std::int32_t linear_search(const void* key, std::int32_t offset, char type, std::int32_t restore_pos);  ///< 0x00584d60
    std::int32_t compare(const void* key, std::int32_t offset, char type);  ///< 0x00584e00
    void clean_up(const std::int32_t* ptr_offsets);  ///< 0x00584f70
    void free_ptr(char* ele, const std::int32_t* ptr_offsets);  ///< 0x00585010
    void quick_sort(int(JOAN_CDECL* cmp)(const void*, const void*));  ///< 0x005850b0
    void init_sort(std::int32_t offset, char type);  ///< 0x00585250

    // The other DynArray functions the game code calls (typed, through their slots).
#include "gen/dyn_array_methods.inc"
};

/// One blanked-out record slot of a DynArrayB, waiting for reuse.
struct EmptyRoom {
    std::int32_t recno;  ///< record number of the free slot
    std::int32_t deleted_date;  ///< never written by this build (stale memory is saved as is)
};

/// DynArray whose linkout() blanks a record and remembers it, so that linkin() can reuse the slot and record numbers
/// stay stable. Owns empty_room_array.
struct DynArrayB : DynArray {
    EmptyRoom* empty_room_array;  ///< 0x1d
    std::int32_t empty_room_num;  ///< 0x21  capacity
    std::int32_t empty_room_count;  ///< 0x25  used
    std::int32_t reuse_interval_days;  ///< 0x29
    char flag_2d;  ///< 0x2d  cleared by the constructor, otherwise unused here

    // --- reimplemented members ---
    /// Constructor: record size, growth step, reuse interval in days, initial capacity. Returns this.
    DynArrayB* construct(std::int32_t ele_size, std::int32_t block_num, std::int32_t reuse_interval,
                         std::int32_t init_num);                        // 0x005856a0
    void destruct();  ///< 0x00585710
    void linkin(const void* ent);  ///< 0x005857a0
    void linkout(std::int32_t pos);  ///< 0x005858c0
    std::int32_t write_empty_room(File* file);  ///< 0x00585b40
    std::int32_t read_empty_room(File* file);  ///< 0x00585bb0

    // The other DynArrayB functions the game code calls (typed, through their slots).
#include "gen/dyn_array_b_methods.inc"
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(DynArray, sort_type) == 0x18);
static_assert(offsetof(DynArray, body_buf) == 0x19);
static_assert(sizeof(DynArray) == 0x1d);
static_assert(offsetof(DynArrayB, empty_room_array) == 0x1d);
static_assert(offsetof(DynArrayB, reuse_interval_days) == 0x29);
static_assert(sizeof(DynArrayB) == 0x2e);
static_assert(sizeof(EmptyRoom) == 8);
#endif

// --- Direct calls from game code that does not have typed objects yet (helpers: original/direct.hpp) ---
/// The DynArray at `p` (an untyped game pointer).
inline DynArray* dynarray(const void* p) noexcept { return static_cast<DynArray*>(const_cast<void*>(p)); }
/// The DynArrayB at `p` (an untyped game pointer).
inline DynArrayB* dynarray_b(const void* p) noexcept { return static_cast<DynArrayB*>(const_cast<void*>(p)); }

}  // namespace joan::orig
