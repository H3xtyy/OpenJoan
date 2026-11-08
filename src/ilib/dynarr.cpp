// Reimplementation of DynArray (d:\ww\ilib\dynarr.cpp, joan.exe
// 0x005843d0-0x00585274) and DynArrayB (d:\ww\ilib\dynarrb.cpp,
// 0x005856a0-0x00585c72) - 29 functions. Decompiled 1:1 including quirks.
//
// Every function replaces the original at its address (registrations at the end of the file). The
// mem().add / del calls pass kSrc / kSrcB and __LINE__ where the original passes its own source file
// and line (the game's allocator keeps them for its leak report).
// Until stage S7c a verify mode ran joan.exe's original next to these functions (the new code on a
// deep copy of the array) and compared the results. joan.exe's code no longer exists and the
// comparison code was removed; the lines above the functions were kept so that the __LINE__ values
// passed to mem() stay the same.
//
#include <cstring>
#include "../original/helpers.hpp"
#include <type_traits>
#include "../original/direct.hpp"
#include "../fn_names.hpp"
#include "../original/dynarray.hpp"
#include "../original/engine.hpp"
#include "../registry.hpp"
#include "../platform/win32_types.hpp"
#include "../data_names.hpp"
#include "../original/gameset.hpp"
#include <cstdint>
#include "../runtime/replay/replay.hpp"
#include "../x87.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../save/gen/records_s5.hpp"
namespace jo = joan::orig;

namespace {
constexpr const char* kSrc = "joan-re/src/ilib/dynarr.cpp";
constexpr const char* kSrcB = "joan-re/src/ilib/dynarr.cpp (B)";

JOAN_COUNTER(c_ctor, "DynArray::DynArray");
JOAN_COUNTER(c_dtor, "DynArray::~DynArray");
JOAN_COUNTER(c_resize, "DynArray::resize");
JOAN_COUNTER(c_zap, "DynArray::zap");
JOAN_COUNTER(c_linkin, "DynArray::linkin");
JOAN_COUNTER(c_linkin_unique, "DynArray::linkin_unique");
JOAN_COUNTER(c_insert, "DynArray::insert");
JOAN_COUNTER(c_insert_at, "DynArray::insert_at");
JOAN_COUNTER(c_linkout, "DynArray::linkout");
JOAN_COUNTER(c_update, "DynArray::update");
JOAN_COUNTER(c_add_blank, "DynArray::add_blank");
JOAN_COUNTER(c_search_ent, "DynArray::linear_search(ent)");
JOAN_COUNTER(c_read, "DynArray::read");
JOAN_COUNTER(c_search_key, "DynArray::linear_search(key)");
JOAN_COUNTER(c_compare, "DynArray::compare");
JOAN_COUNTER(c_clean_up, "DynArray::clean_up");
JOAN_COUNTER(c_free_ptr, "DynArray::free_ptr");
JOAN_COUNTER(c_sort, "DynArray::quick_sort");
JOAN_COUNTER(c_init_sort, "DynArray::init_sort");
JOAN_COUNTER(c_b_ctor, "DynArrayB::DynArrayB");
JOAN_COUNTER(c_b_dtor, "DynArrayB::~DynArrayB");
JOAN_COUNTER(c_b_linkin, "DynArrayB::linkin");
JOAN_COUNTER(c_b_linkout, "DynArrayB::linkout");
JOAN_COUNTER(c_b_write_rooms, "DynArrayB::write_empty_room");
JOAN_COUNTER(c_b_read_rooms, "DynArrayB::read_empty_room");

// Copies a record in, or (ent == nullptr) clears only its first byte, as the original.
void put_record(char* dst, const void* ent, std::int32_t size) {
    if (ent) std::memcpy(dst, ent, static_cast<std::size_t>(size));
    else *dst = 0;
}
}  // namespace

namespace joan::orig {

// 0x005843d0 - DynArray(int eleSize, int blockNum, int initNum). sort_type is
// left uninitialised, as in the original.
DynArray* DynArray::construct(std::int32_t ele_size_, std::int32_t block_num_, std::int32_t init_num) {
    c_ctor.hit();
    ele_size = ele_size_;
    block_num = block_num_;
    ele_num = init_num ? init_num : block_num_;
    body_buf = static_cast<char*>(mem().add(static_cast<std::uint32_t>(ele_size * ele_num), kSrc, __LINE__));
    cur_pos = 0;
    last_ele = 0;
    sort_offset = -1;
    return this;
}

// 0x005844b0
void DynArray::destruct() {
    c_dtor.hit();
    if (body_buf) {
        mem().del(body_buf, kSrc, __LINE__);
        body_buf = nullptr;
    }
}

// 0x00584510
void DynArray::resize(std::int32_t new_ele_num) {
    c_resize.hit();
    body_buf = static_cast<char*>(mem().resize(body_buf, static_cast<std::uint32_t>(new_ele_num * ele_size), kSrc, __LINE__));
    ele_num = new_ele_num;
}

// 0x00584580 - empties the array; with resize_flag also shrinks it back to one block.
void DynArray::zap(std::int32_t resize_flag) {
    c_zap.hit();
    if (resize_flag && ele_num != block_num) {
        ele_num = block_num;
        body_buf = static_cast<char*>(mem().resize(body_buf, static_cast<std::uint32_t>(ele_size * ele_num), kSrc, __LINE__));
    }
    cur_pos = 0;
    last_ele = 0;
}

// 0x00584620 - appends a record and makes it current.
void DynArray::linkin(const void* ent) {
    c_linkin.hit();
    cur_pos = ++last_ele;
    if (last_ele > ele_num) resize(ele_num + block_num);
    put_record(body_buf + (cur_pos - 1) * ele_size, ent, ele_size);
}

// 0x005846f0 - makes an identical existing record current, or appends.
void DynArray::linkin_unique(const void* ent) {
    c_linkin_unique.hit();
    for (std::int32_t i = 0; i < last_ele; ++i) {
        if (std::memcmp(body_buf + i * ele_size, ent, static_cast<std::size_t>(ele_size)) == 0) {
            cur_pos = i + 1;
            return;
        }
    }
    linkin(ent);
}

// 0x00584790 - inserts before the current record; the new record takes its number.
void DynArray::insert(const void* ent) {
    c_insert.hit();
    if (size() == 0) {
        linkin(ent);
        return;
    }
    if (++last_ele > ele_num) resize(ele_num + block_num);
    std::memmove(body_buf + cur_pos * ele_size, body_buf + (cur_pos - 1) * ele_size,
                 static_cast<std::size_t>((last_ele - cur_pos) * ele_size));
    put_record(body_buf + (cur_pos - 1) * ele_size, ent, ele_size);
}

// 0x005848d0 - inserts at record number `pos` (appends past the end); cursor unchanged.
void DynArray::insert_at(std::int32_t pos, const void* ent) {
    c_insert_at.hit();
    if (size() == 0 || pos > last_ele) {
        linkin(ent);
        return;
    }
    if (++last_ele > ele_num) resize(ele_num + block_num);
    std::memmove(body_buf + pos * ele_size, body_buf + (pos - 1) * ele_size,
                 static_cast<std::size_t>((last_ele - pos) * ele_size));
    put_record(body_buf + (pos - 1) * ele_size, ent, ele_size);
}

// 0x00584a10 - removes record `pos` (-1 = current); shrinks when two blocks are unused.
void DynArray::linkout(std::int32_t pos) {
    c_linkout.hit();
    if (pos < 0) pos = cur_pos;
    if (pos == 0 || pos > last_ele) return;
    if (pos != last_ele)
        std::memmove(body_buf + (pos - 1) * ele_size, body_buf + pos * ele_size,
                     static_cast<std::size_t>((last_ele - pos) * ele_size));
    --last_ele;
    if (cur_pos > last_ele) cur_pos = last_ele;
    if (last_ele < ele_num - block_num * 2) resize(ele_num - block_num);
}

// 0x00584b30 - overwrites record `pos` (-1 = current). No upper bound check, as the original.
void DynArray::update(const void* ent, std::int32_t pos) {
    c_update.hit();
    if (pos < 0) pos = cur_pos;
    if (pos <= 0) return;
    put_record(body_buf + (pos - 1) * ele_size, ent, ele_size);
}

// 0x00584bd0 - appends `count` zeroed records; the first of them becomes current.
void DynArray::add_blank(std::int32_t count) {
    c_add_blank.hit();
    cur_pos = last_ele + 1;
    last_ele += count;
    if (last_ele > ele_num) resize(last_ele + block_num);
    std::memset(body_buf + (cur_pos - 1) * ele_size, 0, static_cast<std::size_t>(count * ele_size));
}

// 0x00584c80 - record number of the first record equal to `ent`, or 0.
// Quirk kept: a failed search leaves the cursor one past the end.
std::int32_t DynArray::linear_search(const void* ent) {
    c_search_ent.hit();
    for (cur_pos = 1; cur_pos <= last_ele; ++cur_pos)
        if (std::memcmp(ent, get(), static_cast<std::size_t>(ele_size)) == 0) return cur_pos;
    return 0;
}

// 0x00584d10 - copies the current record out.
void DynArray::read(void* buf) {
    c_read.hit();
    if (buf) std::memcpy(buf, get(), static_cast<std::size_t>(ele_size));
}

// 0x00584d60 - first record whose field at `offset` matches `key` (see compare).
// The cursor is restored only on success with restore_pos; a failed search
// leaves it one past the end.
std::int32_t DynArray::linear_search(const void* key, std::int32_t offset, char type, std::int32_t restore_pos) {
    c_search_key.hit();
    const std::int32_t saved = cur_pos;
    for (cur_pos = 1; cur_pos <= last_ele; ++cur_pos) {
        if (compare(key, offset, type)) {
            const std::int32_t found = cur_pos;
            if (restore_pos) cur_pos = saved;
            return found;
        }
    }
    return 0;
}

// 0x00584e00 - compares the current record's field with `key`:
//   'C' char[] in the record, 'P' char* in the record (Misc::str_cmp, or identical pointer),
//   'c' char, 'i' int, 'd' double (equal or unordered, as the x87 test in the original).
std::int32_t DynArray::compare(const void* key, std::int32_t offset, char type) {
    c_compare.hit();
    char* const ele = get();
    switch (type) {
        case 'C':
        case 'P': {
            const char* field = type == 'C' ? ele + offset : *reinterpret_cast<char**>(ele + offset);
            if (field == key) return 1;
            return misc().str_cmp(field, static_cast<const char*>(key));
        }
        case 'c':
            return *reinterpret_cast<const signed char*>(ele + offset) == *static_cast<const signed char*>(key);
        case 'i':
            return *reinterpret_cast<const std::int32_t*>(ele + offset) == *static_cast<const std::int32_t*>(key);
        case 'd': {
            const double a = *reinterpret_cast<const double*>(ele + offset);
            const double b = *static_cast<const double*>(key);
            return !(a < b) && !(a > b);
        }
        default:
            return 0;
    }
}

// 0x00584f70 - frees the memory referenced by each record's pointer fields
// (offset list: count, then offsets), then empties the array to one block.
void DynArray::clean_up(const std::int32_t* ptr_offsets) {
    c_clean_up.hit();
    if (ptr_offsets)
        for (std::int32_t i = 0; i < last_ele; ++i) free_ptr(body_buf + i * ele_size, ptr_offsets);
    last_ele = 0;
    cur_pos = 0;
    resize(block_num);
}

// 0x00585010 - frees the pointers of one record (the fields are not cleared).
void DynArray::free_ptr(char* ele, const std::int32_t* ptr_offsets) {
    c_free_ptr.hit();
    const std::int32_t n = ptr_offsets[0];
    for (std::int32_t i = 1; i <= n; ++i) {
        void* p = *reinterpret_cast<void**>(ele + ptr_offsets[i]);
        if (p) mem().del(p, kSrc, __LINE__);
    }
}

// 0x005850b0 - sorts with the game's own qsort, so equal keys end up in the same order.
void DynArray::quick_sort(int(JOAN_CDECL* cmp)(const void*, const void*)) {
    c_sort.hit();
    crt_qsort(body_buf, static_cast<std::uint32_t>(last_ele), static_cast<std::uint32_t>(ele_size), cmp);
}

// 0x00585100 DynArray::write_file / 0x00585180 read_file (the 0x1d-byte header incl. body_buf raw, then the records)
// and their DynArrayB variants 0x005859d0 / 0x00585a60: deleted in A6/S6 - no caller in joan.exe (each is reached only
// by its own thunk jmp: 0x0040a4c5, 0x00402b17, 0x00409048, 0x0040ab00), no code reloc, no C++ caller.

// 0x00585250
void DynArray::init_sort(std::int32_t offset, char type) {
    c_init_sort.hit();
    sort_offset = offset;
    sort_type = type;
}

// ---------------------------------------------------------------------------
// DynArrayB
// ---------------------------------------------------------------------------

// 0x005856a0 - DynArrayB(int eleSize, int blockNum, int reuseIntervalDays, int initNum)
DynArrayB* DynArrayB::construct(std::int32_t ele_size_, std::int32_t block_num_, std::int32_t reuse_interval,
                                std::int32_t init_num) {
    c_b_ctor.hit();
    DynArray::construct(ele_size_, block_num_, init_num);
    empty_room_array = nullptr;
    empty_room_num = 0;
    empty_room_count = 0;
    reuse_interval_days = reuse_interval;
    flag_2d = 0;
    return this;
}

// 0x00585710 - empty_room_array is freed but not cleared, as the original.
void DynArrayB::destruct() {
    c_b_dtor.hit();
    if (empty_room_array) mem().del(empty_room_array, kSrcB, __LINE__);
    DynArray::destruct();
}

// 0x005857a0 - reuses the most recently emptied record number if there is one.
void DynArrayB::linkin(const void* ent) {
    c_b_linkin.hit();
    if (empty_room_count > 0) {
        cur_pos = empty_room_array[empty_room_count - 1].recno;
        --empty_room_count;
    } else {
        cur_pos = ++last_ele;
    }
    if (last_ele > ele_num) resize(ele_num + block_num);
    put_record(body_buf + (cur_pos - 1) * ele_size, ent, ele_size);
}

// 0x005858c0 - blanks record `pos` (-1 = current) and remembers it for reuse.
void DynArrayB::linkout(std::int32_t pos) {
    c_b_linkout.hit();
    if (pos < 0) pos = cur_pos;
    if (pos == 0 || pos > last_ele) return;
    if (++empty_room_count > empty_room_num) {
        empty_room_array = static_cast<EmptyRoom*>(mem().resize(
            empty_room_array, static_cast<std::uint32_t>((empty_room_num + block_num) * 8), kSrcB, __LINE__));
        empty_room_num += block_num;
    }
    empty_room_array[empty_room_count - 1].recno = pos;
    std::memset(body_buf + (pos - 1) * ele_size, 0, static_cast<std::size_t>(ele_size));
}

// The empty-room records as one chunk (u32 count before it). [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md
// S5): save::put_EmptyRoom / get_EmptyRoom per record; 0 = the raw call. A chunk above save::kMaxChunk (0x600 rooms)
// goes raw: EmptyRoom (2 x i32) has the same bytes on every target.
static std::int32_t write_rooms(File* file, const EmptyRoom* rooms, std::int32_t count) {
    const auto n = static_cast<std::uint32_t>(count * 8);
    if (!save::fieldwise() || n > save::kMaxChunk) return file->file_write(rooms, n);
    save::Out o(save::port(*file), n, "EmptyRoom[]", rooms);
    for (std::int32_t i = 0; i < count; ++i) save::put_EmptyRoom(o, rooms[i]);
    return o.end();  // File::file_write 0x005872e0
}
static std::int32_t read_rooms(File* file, EmptyRoom* rooms, std::int32_t count) {
    const auto n = static_cast<std::uint32_t>(count * 8);
    if (!save::fieldwise() || n > save::kMaxChunk) return file->file_read(rooms, n);
    save::In in(save::port(*file), n, "EmptyRoom[]", rooms);
    for (std::int32_t i = 0; i < count; ++i) save::get_EmptyRoom(in, rooms[i]);
    return in.end();  // File::file_read 0x005873a0
}

// 0x00585b40
std::int32_t DynArrayB::write_empty_room(File* file) {
    c_b_write_rooms.hit();
    file->file_put_long(empty_room_count);
    if (empty_room_count > 0 && !write_rooms(file, empty_room_array, empty_room_count)) return 0;
    return 1;
}

// 0x00585bb0
std::int32_t DynArrayB::read_empty_room(File* file) {
    c_b_read_rooms.hit();
    empty_room_count = file->file_get_long();
    empty_room_num = empty_room_count;
    if (empty_room_count > 0) {
        empty_room_array = static_cast<EmptyRoom*>(
            mem().resize(empty_room_array, static_cast<std::uint32_t>(empty_room_count * 8), kSrcB, __LINE__));
        if (!read_rooms(file, empty_room_array, empty_room_count)) return 0;
    } else if (empty_room_array) {
        mem().del(empty_room_array, kSrcB, __LINE__);
        empty_room_array = nullptr;
    }
    return 1;
}

}  // namespace joan::orig

namespace {
using joan::orig::DynArray;
using joan::orig::DynArrayB;
}  // namespace

#define DA(name, addr, fn) JOAN_REPLACE("ilib/dynarr.cpp", "DynArray::" name, addr, fn)
DA("DynArray", 0x005843d0, &DynArray::construct);
DA("~DynArray", 0x005844b0, &DynArray::destruct);
DA("resize", 0x00584510, &DynArray::resize);
DA("zap", 0x00584580, &DynArray::zap);
DA("linkin", 0x00584620, &DynArray::linkin);
DA("linkin_unique", 0x005846f0, &DynArray::linkin_unique);
DA("insert", 0x00584790, &DynArray::insert);
DA("insert_at", 0x005848d0, &DynArray::insert_at);
DA("linkout", 0x00584a10, &DynArray::linkout);
DA("update", 0x00584b30, &DynArray::update);
DA("add_blank", 0x00584bd0, &DynArray::add_blank);
DA("linear_search(ent)", 0x00584c80, static_cast<std::int32_t (DynArray::*)(const void*)>(&DynArray::linear_search));
DA("read", 0x00584d10, &DynArray::read);
DA("scan", 0x00584d60,
   static_cast<std::int32_t (DynArray::*)(const void*, std::int32_t, char, std::int32_t)>(&DynArray::linear_search));
DA("compare", 0x00584e00, &DynArray::compare);
DA("clean_up", 0x00584f70, &DynArray::clean_up);
DA("free_ptr", 0x00585010, &DynArray::free_ptr);
DA("quick_sort", 0x005850b0, &DynArray::quick_sort);
DA("init_sort", 0x00585250, &DynArray::init_sort);
#undef DA

#define DAB(name, addr, fn) JOAN_REPLACE("ilib/dynarrb.cpp", "DynArrayB::" name, addr, fn)
DAB("DynArrayB", 0x005856a0, &DynArrayB::construct);
#undef DAB

// ---------------------------------------- from modules/game_b111_12.cpp (fix/uninit)
namespace {
namespace game_b111_12 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::orig::helpers::at;
using joan::img::str;

JOAN_COUNTER(c_sorted_insert, "SortedList::insert");

struct H {

    // -------------------------------------------------------------------- 0x00585280
    void sorted_insert(u8* item) {
        c_sorted_insert.hit();
        // fix/uninit: [ebp-0x8] is not written for key types outside 1..5; 0 here (append).
        i32 cmp = 0;
        for (i32 i = jo::self<jo::DynArray>(this)->last_ele; i > 0; --i) {
            switch (static_cast<u32>(static_cast<i32>(jo::view<i8>(jo::self<jo::DynArray>(this)->sort_type)) - 1)) {
                case 0: {  // int
                    const i32 off = jo::self<jo::DynArray>(this)->sort_offset;
                    u8* const last = joan::orig::arg::as<u8*>(joan::orig::as_pool(this)->last_used());  // 0x004b9c30
                    cmp = at<i32>(item, off) > at<i32>(last, jo::self<jo::DynArray>(this)->sort_offset) ? 1 : 0;
                    break;
                }
                case 1: {  // short
                    const i32 a = at<i16>(item, jo::self<jo::DynArray>(this)->sort_offset);
                    u8* const last = joan::orig::arg::as<u8*>(joan::orig::as_pool(this)->last_used());
                    cmp = a > static_cast<i32>(at<i16>(last, jo::self<jo::DynArray>(this)->sort_offset)) ? 1 : 0;
                    break;
                }
                case 2: {  // char
                    const i32 a = at<i8>(item, jo::self<jo::DynArray>(this)->sort_offset);
                    u8* const last = joan::orig::arg::as<u8*>(joan::orig::as_pool(this)->last_used());
                    cmp = a > static_cast<i32>(at<i8>(last, jo::self<jo::DynArray>(this)->sort_offset)) ? 1 : 0;
                    break;
                }
                case 3: {  // char* field
                    // The sort key sits at a runtime offset of a generic record (not a class field): copy the pointer out.
                    const char* a;
                    std::memcpy(&a, item + jo::self<jo::DynArray>(this)->sort_offset, sizeof a);
                    u8* const last = joan::orig::arg::as<u8*>(joan::orig::as_pool(this)->last_used());
                    const char* b;
                    std::memcpy(&b, last + jo::self<jo::DynArray>(this)->sort_offset, sizeof b);
                    cmp = std::strcmp(joan::orig::arg::str(a), joan::orig::arg::str(b));  // strcmp
                    break;
                }
                case 4: {  // inline string
                    u8* const a = item + jo::self<jo::DynArray>(this)->sort_offset;
                    u8* const last = joan::orig::arg::as<u8*>(joan::orig::as_pool(this)->last_used());
                    u8* const b = last + jo::self<jo::DynArray>(this)->sort_offset;
                    cmp = std::strcmp(joan::orig::arg::str(a), joan::orig::arg::str(b));  // strcmp
                    break;
                }
                default:
                    break;
            }
            if (cmp >= 0) {
                joan::orig::dynarray(this)->insert_at(joan::orig::arg::i32(jo::self<jo::DynArray>(this)->last_ele + 1), joan::orig::arg::ptr(item));  // 0x005848d0
                return;
            }
        }
        joan::orig::dynarray(this)->insert_at(i32{1}, joan::orig::arg::ptr(item));
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: key type outside 1..5 -> compare result 0 instead of stack garbage (dead code).
FIX("SortedList::insert", 0x00585280, &H::sorted_insert);
#undef FIX

}  // namespace game_b111_12
}  // namespace

// ---------------------------------------- from modules/game_b101_10.cpp (game/b101_10)
namespace {
namespace game_b101_10 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::orig::helpers::at;

JOAN_COUNTER(c_insert, "SortedArray::insert_bsearch");

struct H {

    // ------------------------------------------------------------------ 0x00585470
    u32 insert_bsearch(void* item, std::uintptr_t cmp_fn) {
        c_insert.hit();
        using Cmp = i32(JOAN_CDECL*)(void*, void*);
        const Cmp cmp = reinterpret_cast<Cmp>(cmp_fn);
        if (jo::self<jo::DynArray>(this)->last_ele <= 0) {
            joan::orig::dynarray(this)->linkin(joan::orig::arg::ptr(item));  // 0x00584620
            return joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
        }
        joan::orig::as_counter(this)->clamp_to_one();  // 0x004b9d40 first
        i32 r = cmp(item, joan::orig::arg::as<void*>(joan::orig::as_pool(this)->last_used()));  // 0x004b9c30
        if (r == 0) return 0;
        if (r < 0) {
            joan::orig::dynarray(this)->insert(joan::orig::arg::ptr(item));  // 0x00584790
            return 1;
        }
        joan::orig::as_dynarray(this)->go_end();  // 0x00585630 last
        r = cmp(item, joan::orig::arg::as<void*>(joan::orig::as_pool(this)->last_used()));
        if (r == 0) return 0;
        if (r > 0) {
            joan::orig::dynarray(this)->linkin(joan::orig::arg::ptr(item));  // 0x00584620
            return joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());
        }
        i32 lo = 1;
        i32 hi = jo::self<jo::DynArray>(this)->last_ele;
        while (static_cast<i32>(static_cast<u32>(hi) - static_cast<u32>(lo)) > 1) {
            const i32 mid = static_cast<i32>(static_cast<u32>(lo) + static_cast<u32>(hi)) / 2;
            joan::orig::as_dynarray(this)->go(joan::orig::arg::i32(mid));  // 0x00585660 seek
            r = cmp(item, joan::orig::arg::as<void*>(joan::orig::as_pool(this)->last_used()));
            if (r < 0) {
                hi = mid;
            } else if (r > 0) {
                lo = mid;
            } else {
                return 0;
            }
        }
        joan::orig::as_dynarray(this)->go(joan::orig::arg::i32(hi));
        joan::orig::dynarray(this)->insert(joan::orig::arg::ptr(item));  // 0x00584790
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_10", name, addr, fn)
B("DynArray::linkin_unique_sorted", 0x00585470, &H::insert_bsearch);
#undef B

}  // namespace game_b101_10
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
namespace {
namespace game_b94_05 {

using joan::orig::helpers::at;

struct H {

    // 0x00585630
    void reset_585630() { jo::view<std::uint32_t>(jo::self<jo::DynArray>(this)->cur_pos) = jo::view<std::uint32_t>(jo::self<jo::DynArray>(this)->last_ele); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("Obj::reset_585630", 0x00585630, &H::reset_585630);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::orig::helpers::at;

struct H {

    // 0x00585660
    void set_page(std::int32_t n) {
        if (n >= 1 && n <= jo::self<jo::DynArray>(this)->last_ele) jo::self<jo::DynArray>(this)->cur_pos = n;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Pager::set_page", 0x00585660, &H::set_page);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::DynArray::go_end() -> void { joan::bind::member<&::game_b94_05::H::reset_585630>(this); }  // 0x585630
auto joan::orig::DynArray::go(std::int32_t a0) -> void { joan::bind::member<&::game_b91_02::H::set_page>(this, a0); }  // 0x585660
