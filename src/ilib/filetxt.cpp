// Reimplementation of FileTxt (d:\ww\ilib\filetxt.cpp), joan.exe
// 0x00589550-0x0058a2b4 - all 12 functions. Decompiled 1:1 including quirks
// (noted inline).
//
// Until stage S7c a verify mode ran joan.exe's original cursor functions next to
// these (the new code on a snapshot of the object) and compared the results;
// joan.exe's code no longer exists and the comparison code was removed. (This note
// keeps the comment's length: the mem() calls below pass __LINE__ where the
// original passes its own source line.)
#include <cstring>
#include "../original/direct.hpp"
#include "../fn_names.hpp"
#include "../original/engine.hpp"
#include "../original/file_txt.hpp"
#include "../registry.hpp"

namespace {
constexpr const char* kSrc = "joan-re/src/ilib/filetxt.cpp";
constexpr char kCr = 0x0d;
constexpr char kLf = 0x0a;

JOAN_COUNTER(c_ctor, "FileTxt::FileTxt(name)");
JOAN_COUNTER(c_ctor_from, "FileTxt::FileTxt(File*)");
JOAN_COUNTER(c_ctor_mem, "FileTxt::FileTxt(mem)");
JOAN_COUNTER(c_dtor, "FileTxt::~FileTxt");
JOAN_COUNTER(c_get_char, "FileTxt::get_char");
JOAN_COUNTER(c_next_line, "FileTxt::next_line");
JOAN_COUNTER(c_locate, "FileTxt::locate_word");
JOAN_COUNTER(c_token, "FileTxt::get_token");
JOAN_COUNTER(c_num, "FileTxt::get_num");
JOAN_COUNTER(c_line, "FileTxt::read_line");
JOAN_COUNTER(c_para, "FileTxt::read_paragraph");
JOAN_COUNTER(c_rewind, "FileTxt::rewind");

// Field separators skipped around tokens and at line starts.
bool is_sep(char c) { return c == ' ' || c == ',' || c == ':' || c == '\t'; }

void skip_seps(char*& p) {
    while (is_sep(*p)) ++p;
}

// CRT atof of the game (0x007b2110, returns the double in ST0), so number
// parsing matches exactly.
double crt_atof(const char* s) {
    using Fn = double(JOAN_CDECL*)(const char*);
    return joan::hook::original<Fn>(fn::crt_atof)(s);
}
}  // namespace

namespace joan::orig {

// The three constructors run File::File() (0x004179c0: file_handle = INVALID)
// and then load the whole text plus a trailing EOF byte into data_buf.

// 0x00589550 - FileTxt(char* fileName)
FileTxt* FileTxt::construct(const char* file_name) {
    c_ctor.hit();
    file.file_handle = reinterpret_cast<void*>(-1);
    file.file_open(file_name, 1, 0);
    data_size = file.file_size();
    data_buf = static_cast<char*>(mem().add(static_cast<std::uint32_t>(data_size + 1), kSrc, __LINE__));
    data_buf[data_size] = kEof;
    ptr = data_buf;
    file.file_read(data_buf, static_cast<std::uint32_t>(data_size));
    file.file_close();
    return this;
}

// 0x00589680 - FileTxt(File* src, int size): reads `size` bytes from another file.
FileTxt* FileTxt::construct_from(File* src, std::int32_t size) {
    c_ctor_from.hit();
    file.file_handle = reinterpret_cast<void*>(-1);
    data_size = size;
    data_buf = static_cast<char*>(mem().add(static_cast<std::uint32_t>(data_size + 1), kSrc, __LINE__));
    data_buf[data_size] = kEof;
    ptr = data_buf;
    src->file_read(data_buf, static_cast<std::uint32_t>(data_size));
    return this;
}

// 0x00589780 - FileTxt(char* data, int size): copies text from memory.
FileTxt* FileTxt::construct_mem(const char* data, std::int32_t size) {
    c_ctor_mem.hit();
    file.file_handle = reinterpret_cast<void*>(-1);
    data_size = size;
    data_buf = static_cast<char*>(mem().add(static_cast<std::uint32_t>(data_size + 1), kSrc, __LINE__));
    data_buf[data_size] = kEof;
    std::memcpy(data_buf, data, static_cast<std::size_t>(data_size));
    ptr = data_buf;
    return this;
}

// 0x00589890
void FileTxt::destruct() {
    c_dtor.hit();
    mem().del(data_buf, kSrc, __LINE__);
    file.destruct();
}

// 0x00589920 - next character, crossing line ends; 0 at end of text.
char FileTxt::get_char(std::int32_t advance) {
    c_get_char.hit();
    while (*ptr == kCr) next_line();
    if (*ptr == kEof) return 0;
    const char c = *ptr;
    if (advance) ++ptr;
    return c;
}

// 0x005899b0 - moves to the first field of the next line; nullptr at EOF.
char* FileTxt::next_line() {
    c_next_line.hit();
    while (*ptr != kCr && *ptr != kEof) ++ptr;
    if (*ptr == kCr) ++ptr;
    if (*ptr == kLf) ++ptr;
    skip_seps(ptr);
    return *ptr == kEof ? nullptr : ptr;
}

// 0x00589b10 - moves past the next occurrence of `word`, then past separators.
// Quirk kept: after a partial match neither the cursor nor `word` is rewound,
// so the search continues with the rest of the word.
char* FileTxt::locate_word(const char* word) {
    c_locate.hit();
    for (;; ++ptr) {
        if (*ptr == kEof) return nullptr;
        if (*ptr != *word) continue;
        while (*ptr == *word && *word) {
            ++ptr;
            ++word;
        }
        if (*word == 0) {
            skip_seps(ptr);
            return ptr;
        }
    }
}

// 0x00589c70 - reads the next token (ended by ' ', '=', ',', ':', TAB or CR)
// into `token`. advance == 0 peeks. Returns nullptr at end of text.
// Quirks kept: hitting EOF inside a token returns nullptr (the partial token
// is dropped); if next_line() fails while skipping blank lines the cursor is
// not restored even when peeking; a 128-char token's terminator is written
// just past `token` (into data_size's low byte), as in the original.
char* FileTxt::get_token(std::int32_t advance) {
    c_token.hit();
    char* const saved = ptr;
    char* out = reinterpret_cast<char*>(this) + offsetof(FileTxt, token);

    skip_seps(ptr);
    while (*ptr == kCr)
        if (!next_line()) return nullptr;

    for (std::int32_t count = 0;; ++count, ++ptr) {
        const char c = *ptr;
        if (c == ' ' || c == '=' || c == ',' || c == ':' || c == '\t' || c == kCr) break;
        if (c == kEof) {
            if (!advance) ptr = saved;
            return nullptr;
        }
        if (count < kTokenLen) *out++ = c;
    }
    *out = 0;
    skip_seps(ptr);
    if (!advance) ptr = saved;
    return token;
}

// 0x00589ee0 - atof(get_token()), returned as a double (in ST0, like the
// original). Quirk kept: crashes at EOF (atof(nullptr)), like the original.
double FileTxt::get_num() {
    c_num.hit();
    return crt_atof(get_token(1));
}

// 0x00589f10 - copies the rest of the line (up to max_len chars plus NUL) and moves to the next line.
void FileTxt::read_line(char* buf, std::int32_t max_len) {
    c_line.hit();
    skip_seps(ptr);
    if (*ptr == kCr) next_line();
    for (std::int32_t count = 0; *ptr != kCr && *ptr != kEof && count < max_len; ++count) *buf++ = *ptr++;
    *buf = 0;
    next_line();
}

// 0x0058a060 - reads text up to '~' (or EOF / max_len-1 chars). CR LF becomes
// a space, VT (0x0B) becomes a hard line break. Returns chars written incl. NUL.
std::int32_t FileTxt::read_paragraph(char* buf, std::int32_t max_len) {
    c_para.hit();
    skip_seps(ptr);
    if (*ptr == kCr) next_line();

    std::int32_t count = 0;
    while (*ptr != kEof) {
        char c = *ptr++;
        if (c == '~') break;
        if (c == kCr && *ptr == kLf) {
            ++ptr;
            c = ' ';
        }
        if (c == 0x0b) {
            c = '\n';
            if (*ptr == kCr) ++ptr;
            if (*ptr == kLf) ++ptr;
        }
        *buf++ = c;
        if (++count >= max_len - 1) break;
    }
    *buf = 0;
    return count + 1;
}

// 0x0058a290
void FileTxt::rewind() {
    c_rewind.hit();
    ptr = data_buf;
}

}  // namespace joan::orig

namespace {
using joan::orig::FileTxt;
}  // namespace

#define TXT(name, addr, fn) JOAN_REPLACE("ilib/filetxt.cpp", "FileTxt::" name, addr, fn)
TXT("FileTxt_589550", 0x00589550, &FileTxt::construct);
TXT("FileTxt_589680", 0x00589680, &FileTxt::construct_from);
TXT("FileTxt_589780", 0x00589780, &FileTxt::construct_mem);
TXT("~FileTxt", 0x00589890, &FileTxt::destruct);
TXT("get_char", 0x00589920, &FileTxt::get_char);
TXT("next_line", 0x005899b0, &FileTxt::next_line);
TXT("locate_word", 0x00589b10, &FileTxt::locate_word);
TXT("get_token", 0x00589c70, &FileTxt::get_token);
TXT("get_num", 0x00589ee0, &FileTxt::get_num);
TXT("read_line", 0x00589f10, &FileTxt::read_line);
TXT("read_paragraph", 0x0058a060, &FileTxt::read_paragraph);
TXT("rewind", 0x0058a290, &FileTxt::rewind);
#undef TXT
