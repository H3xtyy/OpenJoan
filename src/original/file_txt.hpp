// Original memory layout of FileTxt (d:\ww\ilib\filetxt.cpp, joan.exe
// 0x00589550-0x0058a2b4): a whole text file loaded into memory and read
// with a cursor. Used for the game's many text definitions and scripts.
//
// The text ends with an appended 0x1A (EOF) byte. Lines end in CR LF.
// Token separators are ' ', ',', ':' and TAB ('=' also ends a token).
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

#include "file.hpp"

namespace joan::orig {

#pragma pack(push, 1)

/// A whole text file in memory read with a cursor and token helpers (original FileTxt). Owns data_buf.
struct FileTxt {
    static constexpr int kTokenLen = 128;  ///< capacity of `token`
    static constexpr char kEof = 0x1a;  ///< end-of-text marker appended after the data

    File file;  ///< 0x000  (base class)
    char* ptr;  ///< 0x10e  read cursor
    char* data_buf;  ///< 0x112  data_size bytes + 0x1A
    char token[kTokenLen];  ///< 0x116  last get_token() result
    std::int32_t data_size;  ///< 0x196  (a 128-char token's terminator lands in its low byte - original bug, kept)

    // --- reimplemented members (src/ilib/filetxt.cpp) ---
    FileTxt* construct(const char* file_name);  ///< 0x00589550  FileTxt(char*)
    FileTxt* construct_from(File* src, std::int32_t size);  ///< 0x00589680  FileTxt(File*, int)
    FileTxt* construct_mem(const char* data, std::int32_t size);  ///< 0x00589780  FileTxt(char*, int)
    void destruct();  ///< 0x00589890  ~FileTxt
    char get_char(std::int32_t advance);  ///< 0x00589920
    char* next_line();  ///< 0x005899b0
    char* locate_word(const char* word);  ///< 0x00589b10
    char* get_token(std::int32_t advance);  ///< 0x00589c70
    double get_num();  ///< 0x00589ee0 atof of the next token
    void read_line(char* buf, std::int32_t max_len);  ///< 0x00589f10
    std::int32_t read_paragraph(char* buf, std::int32_t max_len);  ///< 0x0058a060
    void rewind();  ///< 0x0058a290

    // The other FileTxt functions the game code calls (typed, through their slots).
#include "gen/file_txt_methods.inc"
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(FileTxt, ptr) == 0x10e);
static_assert(offsetof(FileTxt, data_buf) == 0x112);
static_assert(offsetof(FileTxt, token) == 0x116);
static_assert(offsetof(FileTxt, data_size) == 0x196);
static_assert(sizeof(FileTxt) == 0x19a);
#endif

}  // namespace joan::orig
