// Shared reimplementation of the engine's two resource archive classes
// (see src/original/res_idx.hpp). Included by resx.cpp and reslx.cpp only.
//
//   ResourceIdx   short index: u16 count; { char name[9];  u32 offset; }[count+1]
//   ResourceIdxL  long index:  u32 count; { char name[34]; u32 offset; }[count+1]
//
// Decompiled from the unoptimised originals; behaviour is kept 1:1.
// Every function has a call counter. (The verify-mode wrappers of get_index / get_data_size* went with
// joan.exe's code in stage S7c; this note keeps the __LINE__ values passed to mem() below unchanged.)
#pragma once

#include <cstring>

#include "../original/engine.hpp"
#include "../original/res_idx.hpp"
#include "../registry.hpp"

namespace joan::orig {

template <class Traits>
struct ResIdxCounters {
    using C = hook::Counter;
    C init{"init", Traits::kClass};
    C deinit{"deinit", Traits::kClass};
    C read_into{"read_into", Traits::kClass};
    C read_name{"read(name)", Traits::kClass};
    C size_name{"get_data_size(name)", Traits::kClass};
    C size{"get_data_size(rec)", Traits::kClass};
    C index{"get_index", Traits::kClass};
    C read_rec{"read(rec)", Traits::kClass};
    C file{"get_file(name)", Traits::kClass};
    C file_rec{"get_file(rec)", Traits::kClass};
    C set_user{"set_user_buf", Traits::kClass};
    C clear_user{"clear_user_buf", Traits::kClass};
    C index_rec{"get_index_rec", Traits::kClass};
    C sort{"build_sorted_index", Traits::kClass};

    static ResIdxCounters& get() {
        static ResIdxCounters c;  // first use happens while hooks are installed (see resx.cpp)
        return c;
    }
};

namespace residx_detail {
inline constexpr const char* kSrc = "joan-re/src/ilib/res_idx_impl.hpp";  // shown by the game's debug reports

// 0x0059d160 / 0x0059c410 - qsort comparator: plain strcmp on the names.
inline int JOAN_CDECL compare_names(const void* a, const void* b) {
    return std::strcmp(static_cast<const char*>(a), static_cast<const char*>(b));
}
}  // namespace residx_detail

// init - opens the archive and loads its index. With read_all the whole
// archive is loaded and the file closed; with use_common_buf entries are read
// into the shared buffer.
template <class T>
void BasicResourceIdx<T>::init(const char* name, char read_all_, char use_common_buf_) {
    ResIdxCounters<T>::get().init.hit();
    if (init_flag) deinit();
    file.file_open(name, 1, 0);
    read_all = read_all_;
    use_common_buf = use_common_buf_;
    data_buf = nullptr;
    data_buf_size = 0;
    if constexpr (T::kLongCount)
        rec_count = file.file_get_long();
    else
        rec_count = file.file_get_short();
    cur_rec_no = -1;
    user_buf = nullptr;
    user_buf_size = 0;
    user_skip = 0;

    const auto index_bytes = static_cast<std::uint32_t>(rec_count + 1) * static_cast<std::uint32_t>(sizeof(Rec));
    index_buf = static_cast<Rec*>(mem().add(index_bytes, residx_detail::kSrc, __LINE__));
    file.file_read(index_buf, index_bytes);
    build_sorted_index();

    if (read_all) {
        const auto size = static_cast<std::uint32_t>(index_buf[rec_count].pointer - index_buf[0].pointer);
        data_buf = static_cast<char*>(mem().add(size, residx_detail::kSrc, __LINE__));
        file.file_read(data_buf, size);
        file.file_close();
    } else if (use_common_buf) {
        data_buf = common_res_buf();
    }
    init_flag = 1;
}

template <class T>
void BasicResourceIdx<T>::deinit() {
    ResIdxCounters<T>::get().deinit.hit();
    if (!init_flag) return;
    if (sorted_index) {
        mem().del(sorted_index, residx_detail::kSrc, __LINE__);
        sorted_index = nullptr;
    }
    if (index_buf) {
        mem().del(index_buf, residx_detail::kSrc, __LINE__);
        index_buf = nullptr;
    }
    if (data_buf && data_buf != common_res_buf()) {
        mem().del(data_buf, residx_detail::kSrc, __LINE__);
        data_buf = nullptr;
    }
    if (!read_all) file.file_close();
    init_flag = 0;
}

// Reads the named entry into a caller buffer (skipping `skip` bytes); returns
// 1 on success, 0 if missing or larger than `size`.
template <class T>
std::int32_t BasicResourceIdx<T>::read_into(const char* name, char* buf, std::uint32_t size, std::int32_t skip) {
    ResIdxCounters<T>::get().read_into.hit();
    const std::int32_t rec = get_index(name);
    if (rec == 0) return 0;
    set_user_buf(buf, size, skip);
    const std::int32_t ok = read_rec(rec) != nullptr ? 1 : 0;
    clear_user_buf();
    return ok;
}

template <class T>
char* BasicResourceIdx<T>::read(const char* name) {
    ResIdxCounters<T>::get().read_name.hit();
    const std::int32_t rec = get_index(name);
    return rec ? read_rec(rec) : nullptr;
}

template <class T>
std::int32_t BasicResourceIdx<T>::get_data_size_by_name(const char* name) {
    ResIdxCounters<T>::get().size_name.hit();
    const std::int32_t rec = get_index(name);
    return rec ? index_buf[rec].pointer - index_buf[rec - 1].pointer : 0;
}

// Size of record `rec_no` (1-based): index[rec_no] - index[rec_no-1].
template <class T>
std::int32_t BasicResourceIdx<T>::get_data_size(std::int32_t rec_no) {
    ResIdxCounters<T>::get().size.hit();
    return index_buf[rec_no].pointer - index_buf[rec_no - 1].pointer;
}

// Binary search over the name-sorted index (case-sensitive CRT strcmp,
// exactly as the original). Returns the 1-based record number or 0.
template <class T>
std::int32_t BasicResourceIdx<T>::get_index(const char* name) {
    ResIdxCounters<T>::get().index.hit();
    if (rec_count == 0) return 0;
    std::int32_t lo = 0;
    std::int32_t hi = rec_count - 1;
    while (hi - lo > 1) {
        const std::int32_t mid = (lo + hi) / 2;
        if (std::strcmp(sorted_index[mid].name, name) > 0)
            hi = mid;
        else
            lo = mid;
    }
    if (std::strcmp(sorted_index[lo].name, name) == 0) return sorted_index[lo].pointer + 1;
    if (std::strcmp(sorted_index[hi].name, name) == 0) return sorted_index[hi].pointer + 1;
    return 0;
}

// Returns a pointer to the data of record `rec_no` (1-based). Destination, in
// order of precedence: the preloaded archive (read_all), the caller buffer
// (user_buf), the shared buffer (use_common_buf), or the object's own
// growable buffer, which caches the last record read.
template <class T>
char* BasicResourceIdx<T>::read_rec(std::int32_t rec_no) {
    ResIdxCounters<T>::get().read_rec.hit();
    const std::int32_t i = rec_no - 1;
    if (read_all) return data_buf + index_buf[i].pointer - index_buf[0].pointer;
    if (!use_common_buf && !user_buf && i == cur_rec_no) return data_buf;

    auto size = static_cast<std::uint32_t>(index_buf[i + 1].pointer - index_buf[i].pointer);
    file.file_seek(index_buf[i].pointer, -1);

    if (user_buf) {
        if (user_skip > 0) {
            file.file_seek(user_skip, 1 /* FILE_CURRENT */);
            size -= static_cast<std::uint32_t>(user_skip);
        }
        if (size > user_buf_size) return nullptr;
        file.file_read(user_buf, size);
        return user_buf;
    }
    if (use_common_buf) {
        data_buf = common_res_buf();
        file.file_read(data_buf, size);
        return data_buf;
    }
    if (data_buf_size < size) {
        data_buf_size = size;
        data_buf = static_cast<char*>(mem().resize(data_buf, size, residx_detail::kSrc, __LINE__));
    }
    file.file_read(data_buf, size);
    cur_rec_no = i;
    return data_buf;
}

// Seeks the archive to the named entry and returns the File positioned at its
// data (size via `size`), or nullptr if not found.
template <class T>
File* BasicResourceIdx<T>::get_file(const char* name, std::int32_t& size) {
    ResIdxCounters<T>::get().file.hit();
    const std::int32_t idx = get_index(name) - 1;
    if (idx < 0) return nullptr;
    file.file_seek(index_buf[idx].pointer, -1);
    size = index_buf[idx + 1].pointer - index_buf[idx].pointer;
    return &file;
}

// Same, by 1-based record number (no range check, as the original).
template <class T>
File* BasicResourceIdx<T>::get_file_rec(std::int32_t rec_no, std::int32_t& size) {
    ResIdxCounters<T>::get().file_rec.hit();
    file.file_seek(index_buf[rec_no - 1].pointer, -1);
    size = index_buf[rec_no].pointer - index_buf[rec_no - 1].pointer;
    return &file;
}

template <class T>
void BasicResourceIdx<T>::set_user_buf(char* buf, std::uint32_t size, std::int32_t skip) {
    ResIdxCounters<T>::get().set_user.hit();
    user_buf = buf;
    user_buf_size = size;
    user_skip = skip;
}

// Note: the original clears buf and size but leaves user_skip.
template <class T>
void BasicResourceIdx<T>::clear_user_buf() {
    ResIdxCounters<T>::get().clear_user.hit();
    user_buf = nullptr;
    user_buf_size = 0;
}

// Index record for a 1-based record number, nullptr if out of range.
template <class T>
typename BasicResourceIdx<T>::Rec* BasicResourceIdx<T>::get_index_rec(std::int32_t rec_no) {
    ResIdxCounters<T>::get().index_rec.hit();
    if (rec_no < 1 || rec_no > rec_count) return nullptr;
    return &index_buf[rec_no - 1];
}

// Copies the index, stores each record's 0-based number in .pointer and sorts
// the copy by name with the game's own qsort, so the order of any duplicate
// names matches the original exactly.
template <class T>
void BasicResourceIdx<T>::build_sorted_index() {
    ResIdxCounters<T>::get().sort.hit();
    const auto bytes = static_cast<std::uint32_t>(rec_count) * static_cast<std::uint32_t>(sizeof(Rec));
    sorted_index = static_cast<Rec*>(mem().add(bytes, residx_detail::kSrc, __LINE__));
    std::memcpy(sorted_index, index_buf, bytes);
    for (std::int32_t i = 0; i < rec_count; ++i) sorted_index[i].pointer = i;
    if (rec_count > 1)
        crt_qsort(sorted_index, static_cast<std::uint32_t>(rec_count), sizeof(Rec), residx_detail::compare_names);
}

}  // namespace joan::orig
