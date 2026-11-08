// Original memory layout of the Enlight engine's resource archive classes as
// found in joan.exe (#pragma pack(1), MSVC 6, 32-bit):
//
//   ResourceIdx   d:\ww\ilib\resx.cpp   0x0059c550..  9-byte names, u16 count
//   ResourceIdxL  d:\ww\ilib\reslx.cpp  0x0059b920..  34-byte names, u32 count
//
// The two classes are the same code with a different index record, so they
// are modelled as one template. Offsets come from the disassembly of init
// (0x0059c550 / 0x0059b920) and read (0x0059cbb0 / 0x0059bf70).
//
// The portable reimplementation lives in src/ilib/res_archive.*; these
// structs exist only for the hook build.
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

#include "file.hpp"

namespace joan::orig {

#pragma pack(push, 1)

/// One entry of a resource archive index: NUL-terminated name plus the data offset. `NameLen` is 9 (ResIndex) or 34
/// (ResIndexL). Packed, so 13 / 38 bytes.
template <int NameLen>
struct BasicResIndex {
    char name[NameLen];  ///< entry name as stored in the archive
    std::int32_t pointer;  ///< file offset of the entry's data (sorted copy: 0-based record number)
};
using ResIndex = BasicResIndex<9>;  ///< 13 bytes
using ResIndexL = BasicResIndex<34>;  ///< 38 bytes
static_assert(sizeof(ResIndex) == 13 && sizeof(ResIndexL) == 38);

/// A resource archive read through its index (original ResourceIdx / ResourceIdxL): a File base plus the index table,
/// an optional whole-archive buffer and an optional caller buffer. Instantiated as ResourceIdx (short names) and
/// ResourceIdxL (long names). The object owns index_buf, data_buf and sorted_index (Mem allocator); the 0x00a500a4
/// common buffer is shared and not owned. Failures go through the game's error box (Error::run).
/// TODO(re): 0x0059c550 what `Traits::kLongCount` selects beyond the width of the stored record count is not
/// described here.
template <class Traits>
struct BasicResourceIdx {
    /// Index record type for this archive flavour.
    using Rec = BasicResIndex<Traits::kNameLen>;

    File file;  ///< 0x000  (base class in the original source)
    Rec* index_buf;  ///< 0x10e  rec_count + 1 records (last = sentinel)
    char* data_buf;  ///< 0x112
    std::uint32_t data_buf_size;  ///< 0x116
    Rec* sorted_index;  ///< 0x11a  index sorted by name; .pointer = 0-based record number
    char init_flag;  ///< 0x11e
    char read_all;  ///< 0x11f  whole archive loaded into data_buf, file closed
    char use_common_buf;  ///< 0x120  read into the shared buffer (0x00a500a4)
    std::int32_t rec_count;  ///< 0x121
    std::int32_t cur_rec_no;  ///< 0x125  0-based record currently in data_buf, -1 = none
    char* user_buf;  ///< 0x129  \  optional caller-supplied destination
    std::uint32_t user_buf_size;  ///< 0x12d   > (set_user_buf / read_into)
    std::int32_t user_skip;  ///< 0x131  /  bytes to skip at the start of the entry

    // --- reimplemented members (src/ilib/res_idx_impl.hpp) ---   short-idx / long-idx
    void init(const char* name, char read_all, char use_common_buf);  ///< 0x0059c550 / 0x0059b920
    void deinit();  ///< 0x0059c780 / 0x0059bb40
    std::int32_t read_into(const char* name, char* buf, std::uint32_t size, std::int32_t skip);  ///< 0x0059c8d0 / 0x0059bc90
    char* read(const char* name);  ///< 0x0059c950 / 0x0059bd10
    std::int32_t get_data_size_by_name(const char* name);  ///< 0x0059c9a0 / 0x0059bd60
    std::int32_t get_data_size(std::int32_t rec_no);  ///< 0x0059ca10 / 0x0059bdd0
    std::int32_t get_index(const char* name);  ///< 0x0059ca60 / 0x0059be20  1-based, 0 = none
    char* read_rec(std::int32_t rec_no);  ///< 0x0059cbb0 / 0x0059bf70
    File* get_file(const char* name, std::int32_t& size);  ///< 0x0059ce40 / 0x0059c200
    File* get_file_rec(std::int32_t rec_no, std::int32_t& size);  ///< 0x0059cee0 / 0x0059c2a0
    void set_user_buf(char* buf, std::uint32_t size, std::int32_t skip);  ///< 0x0059cf60 / 0x0059c320
    void clear_user_buf();  ///< 0x0059cfb0 / 0x0059c370
    Rec* get_index_rec(std::int32_t rec_no);  ///< 0x0059cff0 / 0x0059c3b0
    void build_sorted_index();  ///< 0x0059d050 / 0x0059c440

    // The other BasicResourceIdx functions the game code calls (typed, through their slots).
#include "gen/basic_resource_idx_methods.inc"
};

#pragma pack(pop)

/// Traits of ResourceIdx: 9-byte names, u16 record count.
struct ShortIndexTraits {
    static constexpr int kNameLen = 9;  ///< name field size including the NUL
    static constexpr bool kLongCount = false;  ///< u16 record count
    static constexpr const char* kClass = "ResourceIdx";  ///< original class name (error messages)
};
/// Traits of ResourceIdxL: 34-byte names, u32 record count.
struct LongIndexTraits {
    static constexpr int kNameLen = 34;  ///< name field size including the NUL
    static constexpr bool kLongCount = true;  ///< u32 record count
    static constexpr const char* kClass = "ResourceIdxL";  ///< original class name (error messages)
};

/// The resource archive with 9-byte names and a u16 record count (original ResourceIdx, 0x0059c550).
using ResourceIdx = BasicResourceIdx<ShortIndexTraits>;
/// The resource archive with 34-byte names and a u32 record count (original ResourceIdxL, 0x0059b920).
using ResourceIdxL = BasicResourceIdx<LongIndexTraits>;
// Instantiated once each, in src/ilib/resx.cpp and reslx.cpp (the member bodies are in res_idx_impl.hpp); game
// code calls the members directly (original/direct.hpp).
extern template struct BasicResourceIdx<ShortIndexTraits>;
extern template struct BasicResourceIdx<LongIndexTraits>;

#define JOAN_RESIDX_LAYOUT(T)                                \
    static_assert(offsetof(T, index_buf) == 0x10e);          \
    static_assert(offsetof(T, data_buf_size) == 0x116);      \
    static_assert(offsetof(T, sorted_index) == 0x11a);       \
    static_assert(offsetof(T, init_flag) == 0x11e);          \
    static_assert(offsetof(T, rec_count) == 0x121);          \
    static_assert(offsetof(T, cur_rec_no) == 0x125);         \
    static_assert(offsetof(T, user_buf) == 0x129);           \
    static_assert(offsetof(T, user_skip) == 0x131);          \
    static_assert(sizeof(T) == 0x135)
#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
JOAN_RESIDX_LAYOUT(ResourceIdx);
JOAN_RESIDX_LAYOUT(ResourceIdxL);
#endif
#undef JOAN_RESIDX_LAYOUT

}  // namespace joan::orig
